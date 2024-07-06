#!/usr/bin/python
# -*- coding: utf-8 -*-

#to use other than standard OpenHoldem dir
OH_main_dir = "OpenHoldem"
#OH_main_dir = "OpenHoldem_Scraper_Optimization"

import os
from os.path import join, getsize, dirname
import sys
import re

import locale
#locale.setlocale(locale.LC_ALL, 'en_US.UTF8')


import codecs
import glob


# writting no comment after symbol name to easily diff files, e.g. OH 2.1.9.4 vs trunk
noCommentEasyDiff = True


SymbolFiles = [ "CSymbolEngine*.cpp" ]
ignoreFiles = [ "CSymbolEnginePokerTracker.cpp" ]

def usage():
  print(sys.argv[0], """<openholdem_base_folder> <targetHeaderFile>""")

#pairs of (filepath, message)
feedback = []


def rangeValueToInt(rangeVal):
  if rangeVal == 'k_betround_preflop':
    return 1
  #fixme: check where this is used, really want to use flop and not preflop? we assume it is an error and should be k_betround_preflop
  elif rangeVal == 'k_betround_flop':
  #  return 2
    return 1
  elif rangeVal == 'k_betround_river':
    return 4
  elif rangeVal == 'k_first_chair':
    return 0
  elif rangeVal == 'k_last_chair':
    return 9
  elif rangeVal == 'k_number_of_cards_per_player-1':
    return 1
  elif rangeVal == 'k_number_of_community_cards-1':
    return 4
  elif re.match('^[0-9]+$', rangeVal):
    return int(rangeVal)
  else:
    feedback.append((fn, "ERROR: unknown RangeOfSymbols start/end: '%s'" % (rangeVal)))
    return None


def extractSymbols(fn):
  res = []
  inFile = codecs.open(fn, "r", codec)
  t = inFile.read()
	
  m = re.compile('[\t ]*CString[\t ]*CSymbol[$a-zA-Z0-9_]*::SymbolsProvided[\t ]*\(.*?[{](.*?[\r\n])[}]', re.DOTALL | re.MULTILINE).search(t)
  if not m:
    return res
  
  methodBody = m.group(1)
  
  #remove all comments
  commentsRe = re.compile('(//.*?)[\r\n]|(/[*].*?[*]/)', re.MULTILINE)
  m = commentsRe.search(methodBody)
  while m:
    if not m.group(1) is None:
      grp = 1
    else:
      grp = 2
    methodBody = methodBody[0 : m.start(grp)] + methodBody[m.end(grp):]
    m = commentsRe.search(methodBody)

  
  #cases:
  #return "dealerchair ";
  #  CString list = "didchec didcall didrais didswag "
  #  "nplayersround nbetsround prevaction ";
  #list += RangeOfSymbols("didchecround%i", k_betround_flop, k_betround_river); 
  rangeRe = re.compile('RangeOfSymbols\([ \t]*"([$a-zA-Z0-9_]+)[%]i"[ \t]*,[ \t]*([^ \t,)]*)[ \t]*,[ \t]*([^,)]*?)[ \t]*\)')
  stringRe = re.compile('(?:return[ \t]+|(?:CString[ \t]+)?[a-zA-Z0-9_]+[ \t]*[+]?=[ \t]*)"([$ a-zA-Z0-9_]*)"')
  commands = methodBody.split(';')
  for c in commands:
    c = c.replace('\n', ' ')
    c = c.replace('\r', ' ')
    c = c.strip()
    #print('looking at:', c)
    
    m = rangeRe.search(c)
    while m:
      start = rangeValueToInt(m.group(2))
      end = rangeValueToInt(m.group(3))
     
      if not start is None and not end is None:  
        for i in range(start, end + 1):
          res.append((m.group(1) + str(i), fn))
      else:
        feedback.append((fn, "ERROR: The range that failed was: %s" % (m.group(0))))
      #remove what we found
      c = c[0 : m.start(0)] + c[m.end(0):]
      c = c.strip()
      m = rangeRe.search(c)
    
    m = stringRe.search(c)
    while m:
      for sym in m.group(1).split(' '):
        if len(sym) > 0:
          res.append((sym, fn))
      #remove what we found
      c = c[0 : m.start(1) - 1] + c[m.end(1) + 1:]
      c = c.strip()
      m = stringRe.search(c)
    
    if len(c) > 0 and not re.match("^(?:CString[ \t]+)?[a-zA-Z0-9_]+[ \t]*[+]?=$", c) and not re.match('^return(?:[\t $a-zA-Z0-9_]*)$', c):
      feedback.append((fn, "DEBUG: not handled: '%s'" % (c)))
    
  inFile.close()
  
  return res


def createHeaderFile(holdemFolder, targetFile):
  
  symNames = []
  text = '''#ifndef __ALL_SYMBOL_NAMES_H__
#define __ALL_SYMBOL_NAMES_H__

static const char* all_symbol_names[] = {
'''
  
  for symGlob in SymbolFiles:
    path = join(join(holdemFolder, OH_main_dir), insensitive_glob(symGlob))
    fileList = glob.glob(path)
    for f in fileList:
      if os.path.basename(f) in ignoreFiles:
        continue
      print("Looking at", os.path.basename(f))
      newSyms = extractSymbols(f)
      symNames.extend(newSyms)
      print("Added", len(newSyms), "more Symbols")
    
  alreadyAdded = set()
  
  #sort sym names
  symNames.sort()
  
  for idx, (sn, fn) in enumerate(symNames):
    if not sn in alreadyAdded:
      comment = ' //' + os.path.basename(fn)
      if noCommentEasyDiff:
        comment = ''
      if not sn[-1] in ('$', '_') :
        text += '"' + sn + '",' + comment + '\n'
      else:
        text += '//"' + sn + '",' + comment + '\n'
      alreadyAdded.add(sn)
  text += '"" //to recognize end\n'
  
  text += '};\n\n#endif // __SYMBOL_NAMES_H__\n\n'
  
  text = convert_line_endings(text, 2)
  
  out = codecs.open(targetFile, "w", codec)
  out.write(text)
  out.close()
  print("done.")


def main():
  if len(sys.argv) != 3:
    usage()
    sys.exit(0)
    

  holdemFolder = os.path.abspath(sys.argv[1])
  targetFile = os.path.abspath(sys.argv[2])
    
  print("using holdemFolder", holdemFolder)
  print("using targetFile", targetFile)
  
  createHeaderFile(holdemFolder, targetFile)
  
  if len(feedback) > 0:
    print()
    print('Errors and Messages:')
    print('====================')
    for fn, msg in feedback:
      print(os.path.basename(fn), msg)


########## helpers:

#make glob case insensitive on linux too, see: http://stackoverflow.com/questions/8151300/ignore-case-in-glob-on-linux
def insensitive_glob(pattern):
    def either(c):
        return '[%s%s]'%(c.lower(),c.upper()) if c.isalpha() else c
    return ''.join(map(either,pattern))

codec="ISO-8859-15"

#http://code.activestate.com/recipes/66434-change-line-endings/
import string
def convert_line_endings(temp, mode):
        #modes:  0 - Unix, 1 - Mac, 2 - DOS
        if mode == 0:
                temp = string.replace(temp, '\r\n', '\n')
                temp = string.replace(temp, '\r', '\n')
        elif mode == 1:
                temp = string.replace(temp, '\r\n', '\r')
                temp = string.replace(temp, '\n', '\r')
        elif mode == 2:
                import re
                #(?<!...) Matches if the current position in the string is not preceded by a match for ... This is called a negative lookbehind assertion. 
                #(?!...)  Matches if ... doesn’t match next. This is a negative lookahead assertion.
                temp = re.sub("\r(?!\n)|(?<!\r)\n", "\r\n", temp)
        return temp
      

######### call main

if __name__ == "__main__":
  main()
  
  

