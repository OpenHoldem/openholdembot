#analyse the new logs versus expected logs

#analyses files test case by test case

###########
## Settings
###########

#is per Log file type
PrintFirstNumErrors = 100
#don't print an error if it is in the expected file too
PrintOnlyIfNotExpected = False


debugOutput = False

#####################################################

import re
import sys
import os
from collections import OrderedDict
import zipfile

from utils import *

  
dumperLogTestSeparator = re.compile("^cur test file: (.*)")
#in test suite log we treat the final summary as a final test case
testSuiteLogTestSeparator = re.compile("^(?: => Test case : | => Execution )(.*)")
  
class CompareResult:
  def __init__(self, newFn, expFn, isDumpLog):
    self.isDumpLog = isDumpLog
    self.newFn = newFn
    self.expFn = expFn

  def close_(self):
    self.newF.close()
    self.newF = None
    self.expF.close()
    self.expF = None
    
  def open_(self):
    self.newF = open(self.newFn, "r")
    self.expF = open(self.expFn, "r")

  #Use to iterate over testcases, returns matching cases with line numbers and list of missing cases (missing in newLog),
  #returns each case of newLog, if exp log misses a case  that is in newLog then None is returned as exp case content (expTestCaseLines).
  #returns: (testCaseName, newTestCaseLines, expTestCaseLines, newLineNumber, expLineNumber, newMissingCases, isLastCase)
  def iterateCases(self):
    self.open_()
    newLiNo = expLiNo = 0
    newLiPrev = expLiPrev = 1
    newTestCase = expTestCase = 'Init'
    newTestCaseNew = expTestCaseNew = 'Init'

    if self.isDumpLog:
      caseSepRe = dumperLogTestSeparator
    else:
      caseSepRe = testSuiteLogTestSeparator
      
    expFoundC = False
    expCase = []
    newL = expL = "x"
    while newL != "":
      
      newFoundC = False
      newCase = []
      newMissingCases = []
      expMatchingCase = None

      #is ok since we always handle complete cases in the loops below,
      #but in the loop we must reset for exp if we jump over cases missing in new
      if caseSepRe.match(newL):
        newLiPrev = newLiNo
      if caseSepRe.match(expL):
        expLiPrev = expLiNo
                
      while True: #newL != '' or expL != '':
        if newL != "" and not newFoundC:
          newL = self.newF.readline()
          newLiNo += 1
        if expL != "" and not expFoundC:
          expL = self.expF.readline()
          expLiNo += 1
        
        if newL == '':
          newFoundC = True
          newTestCaseNew = 'End'
        elif caseSepRe.match(newL):
          #fixme: use common part of dir of all test dirs if we want to support multiple test dirs at same time
          tmpTestName = os.path.basename(caseSepRe.match(newL).group(1).strip())
          if tmpTestName != newTestCase:
            newTestCaseNew = tmpTestName
            newFoundC = True
          else:
            newCase.append(newL)
        else:
          newCase.append(newL)
          
        if expL == '':
          expFoundC = True
          expTestCaseNew = 'End'
        elif caseSepRe.match(expL):
          tmpTestName = os.path.basename(caseSepRe.match(expL).group(1).strip())
          if tmpTestName != expTestCase:
            expTestCaseNew = tmpTestName
            expFoundC = True
          else:
            expCase.append(expL)
        else:
          expCase.append(expL)
          
        if newFoundC:
          if expFoundC:
            if newTestCase == expTestCase:
              #we have the same test case in both files
              expFoundC = False
              expTestCase = expTestCaseNew
              expMatchingCase = list(expCase)
              expCase.clear()
              break
            elif newTestCase < expTestCase:
              #exp is at later test, so some are missing in exp; return news cases and hold exp
              #must not do more because expMatchingCase is initialized to None
              print('Testcase missing in expected log: ' + newTestCase + ', should be at line: ' + str(expLiPrev))
              break
            else:
              #new is at later test, so some are missing in new: continue reading exp until we find news case, except if at end of exp
              newMissingCases.append(expTestCase)
              print('Testcase missing in new log: ' + expTestCase + ', should be at line: ' + str(newLiPrev))
              #we throw away this exp test case to look at the next one, so reset the infos
              expCase.clear()
              expFoundC = False
              expLiPrev = expLiNo
              expTestCase = expTestCaseNew
              #except if at end of exp file, then end loop
              if expL == '':
                break
          
        
      res = (newTestCase, newCase, expMatchingCase, newLiPrev, expLiPrev, newMissingCases, newL == '')
      newTestCase = newTestCaseNew
      if debugOutput:
          ln()
          print("DEBUG: testcase=%s, newStartLine=%s, expStartLine=%s, newMissing=%s, isLast=%s" % (res[0], res[3], res[4], res[5], str(res[6])))
          print("DEBUG: newCase: %s" % (str(res[1])))
          print("DEBUG: expCase: %s" % (str(res[2])))
          ln()
      yield res
        
    self.close_()
  


def printStartComparing(cmpRes):
  ln()
  if cmpRes.isDumpLog:
    kind = "DumperDll"
  else:
    kind = "TestSuite"
  print("comparing " + kind + " logs:")
  print("new='%s'" % (cmpRes.newFn))
  print("expected='%s'" % (cmpRes.expFn))


def printErrorMap(errorMap):
  numErrors = 0
  numExpected = 0
  numPrinted = 0
  for testName, errorList in errorMap.items():
    #error list format: newLE, expLE, relativeMove
    for idx, e in enumerate(errorList):
      if (not e.expected or not PrintOnlyIfNotExpected) and numPrinted < PrintFirstNumErrors:
        msg = newL = expL = newLiNoStr = expLiNoStr = ''
        if not e.nLine is None and not e.showNone:
          newL = e.nLine
        if not e.nLiNo is None:
          newLiNoStr = ", line: " + str(e.nLiNo)

        if not e.eLine is None:
          if e.showBoth and not e.showNone and not e.nLine is None:
            expL = " --VS-- " + e.eLine
          elif not e.showNone and e.nLine is None:
            expL = e.eLine
            
        if not e.eLiNo is None and len(newLiNoStr) > 0:
          expLiNoStr = '/' + str(e.eLiNo)
          
        if not e.msg is None:
          if len(newL) > 0 or len(expL) > 0:
            msg = e.msg + ": "
          else:
            msg = e.msg
            
        msg = msg + newL + expL
        
        relMoveStr = expectedStr = ""
        if e.relativeMove:
          relMoveStr = ", rel. moved"
        if e.expected:
          expectedStr = ", expected"
        
        print("[" + testName + newLiNoStr + expLiNoStr + expectedStr + relMoveStr + "]: " + msg)
         
        numPrinted += 1
        
      numErrors += 1
      if e.expected:
        numExpected += 1
  return numPrinted


def printErrorContainerEntry(ee):
  numPrinted = 0
  if len(ee.warn) > 0 or len(ee.error) > 0:
    ln()
    print("Printing errors of type: " + ee.msg)
    #fixme: improve
    #print("Printing first " + str(PrintFirstNumErrors) + " cases:")
    
    if len(ee.error) > 0:
      #fixme: improve
      if ee.numErrors() > PrintFirstNumErrors:
        print("Printing errors (total: %i/%i):" % (PrintFirstNumErrors, ee.numErrors()))
      else:
        print("Printing errors (total: %i):" % (ee.numErrors()))
      numPrinted += printErrorMap(ee.error)
      
    if len(ee.warn) > 0:
      #fixme: improve
      if ee.numWarns() > PrintFirstNumErrors:
        print("Printing warnings (total: %i/%i) ('expected' means problem exists in expected output too):" % (PrintFirstNumErrors, ee.numWarns()))
      else:
        print("Printing warnings (total: %i) ('expected' means problem exists in expected output too):" % (ee.numWarns()))
      if PrintOnlyIfNotExpected:
        print("Not printing cases where error is expected (config value)")
      numPrinted += printErrorMap(ee.warn)
    ln()
  if numPrinted == 0:
        ln()
        print("No unexpected errors of type: " + ee.msg)
        ln()
  return numPrinted
  

#fixme: we want to control the order
def printErrorContainer(errors):
  for key in errors.__dict__.keys():
    dictVal = getattr(errors, key)
    if isinstance(dictVal, ErrorContainerEntry):
      printErrorContainerEntry(dictVal)
      
     

findErrorRe = re.compile(".*ERROR.*", re.I)



class ErrorMapEntry:
  def __init__(self, msg = None, nLine = None, eLine = None, nLiNo = None, eLiNo = None, expected = False, showBoth = False, showNone = False, relativeMove = False):
    self.msg = msg
    self.nLine = nLine
    self.eLine = eLine
    self.nLiNo = nLiNo
    self.eLiNo = eLiNo
    self.expected = expected
    self.showBoth = showBoth
    self.showNone = showNone
    self.relativeMove = relativeMove


#if you need control if it is an error or warning
def createErrorMapEntry(testName, map_, msg = None, nLE = None, eLE = None, nLine = None, eLine = None, nLiNo = None, eLiNo = None, expected = False, showBoth = False, showNone = False, relativeMove = False):
    if not testName in map_:
      map_[testName] = list()
      
    if not nLE is None:
      nLine = nLE.line
      nLiNo = nLE.liNo
      
    if not eLE is None:
      eLine = eLE.line
      eLiNo = eLE.liNo
      
    entry = ErrorMapEntry(msg, nLine, eLine, nLiNo, eLiNo, expected, showBoth, showNone, relativeMove)
    map_[testName].append(entry)
    
#if it is expected output it is treated as warning, otherwise error, if you need control over that call createErrorMapEntry
def createErrorEntry(testName, errorContainerEntry, msg = None, nLE = None, eLE = None, nLine = None, eLine = None, nLiNo = None, eLiNo = None, expected = False, showBoth = False, showNone = False, relativeMove = False):
    if expected:
      map_ = errorContainerEntry.warn
    else:
      map_ = errorContainerEntry.error
    createErrorMapEntry(testName, map_, msg, nLE, eLE, nLine, eLine, nLiNo, eLiNo, expected, showBoth, showNone, relativeMove)


def nextNonSkip(lineEntries, idx):
  #skip skip stuff
  while idx < len(lineEntries):
    LE = lineEntries[idx]
    if LE.skip:
      idx += 1
    else:
      return (LE, idx)
  return (None, -1)

def findStringContainingError(testName, newLineEntries, expLineEntries, errorStrs):
  relativeMove = False
  newIdx = expIdx = 0
  for idx in range(0, len(newLineEntries)):  
      expIsErr = False
      
      newLE, newIdx = nextNonSkip(newLineEntries, newIdx)
      newIdx += 1
      if newLE is None:
        break
        
      expLE, expIdx = nextNonSkip(expLineEntries, expIdx)
      expIdx += 1
      
      newIsErr = findErrorRe.match(newLE.line)
      if not expLE is None:
        expIsErr = findErrorRe.match(expLE.line)
      
      if newIsErr:
        if not expIsErr:
          #go over all of exp and look for an identical error
          expIdx2 = 0
          for idx2 in range(0, len(expLineEntries)):
            expLE, expIdx2 = nextNonSkip(expLineEntries, expIdx2)
            expIdx2 += 1
            if expLE is None:
              break
            if findErrorRe.match(expLE.line) and newLE.line == expLE.line:
              relativeMove = True
              break
            else:
              expLE = None
        #what lineNumber if not found in exp, could take start or end of case. we take location parallel to new if is inside exp case
        if not expLE is None:
          expected = newLE.line == expLE.line
          createErrorEntry(testName, errorStrs, nLE=newLE, eLE=expLE, expected=expected, relativeMove = relativeMove)
          newLE.skip = True
          expLE.skip = True
        else:
          fakeExpLiNo = None
          if len(expLineEntries) > 0:
            if expIdx < len(expLineEntries):
              fakeExpLiNo = expLineEntries[expIdx].liNo
            else:
              fakeExpLiNo = expLineEntries[-1].liNo
            
          createErrorEntry(testName, errorStrs, nLE=newLE, eLiNo=fakeExpLiNo, expected=False, relativeMove = relativeMove)
          newLE.skip = True
  #don't just set all errors in exp to skip otherwise we miss if error is no longer error (all in new should already be set to skip)
  #collect those cases
  for idx, expLE in enumerate(expLineEntries):
    if findErrorRe.match(expLE.line):
      fakeNewLiNo = newLineEntries[idx].liNo if idx < len(newLineEntries) else newLineEntries[-1].liNo
      createErrorMapEntry(testName, errorStrs.warn, msg="There is an error in expected file that is no longer there", eLE=expLE, nLiNo=fakeNewLiNo)
      expLE.skip = True
  removeSkippedOnes(newLineEntries, expLineEntries)  
      
      
def analyseTestSuiteFinalLine(newLineEntries, expLineEntries, errorFinalLine):
  #should be at lines[-1], but just be sure
  numErrorsRe = re.compile('Error: ([0-9]*)', re.I)
  numFailsRe = re.compile('Failed: ([0-9]*)', re.I)
  idx = 0
  while idx < len(newLineEntries):
    newLastLE = newLineEntries[idx]
    if numErrorsRe.search(newLastLE.line):
      break
    idx += 1
    
  if idx >= len(newLineEntries):
    if len(newLineEntries) > 0:
      createErrorMapEntry('Summary', errorFinalLine.error, msg='Expected to find the final TestSuite summary but not found', nLE=newLineEntries[0])
    else:
      createErrorMapEntry('Summary', errorFinalLine.error, msg='Expected to find the final TestSuite summary but not found')
    return
    
  expLastLE = expLineEntries[idx]
  
  newL = newLastLE.line
  expL = expLastLE.line
  
  newNumErrors = int(numErrorsRe.search(newL).group(1))
  newNumFails =  int(numFailsRe.search(newL).group(1))
  
  if newNumErrors == 0 and newNumFails == 0:
    if newL != expL:
      createErrorMapEntry('Summary', errorFinalLine.warn, msg='you got no errors/fails, but your expected log file has them',
                        nLE=newLastLE, eLE=expLastLE, expected=False, showBoth = True)
  else:
    expected = newL == expL
    createErrorEntry('Summary', errorFinalLine, nLE=newLastLE, eLE=expLastLE, expected=expected, showBoth = not expected)

  
#order is FCKRA, right? NO, saw examples with different order, e.g. (R, C)
#Expected R, got R.
#must also match multi action thing: Expected (R, A), got R.
#and R 2.20 vs R 2.2 and R 0.10 vs R .1
#=> extract all and check by hand
def constructExpectedGotRe():
  GetExpectChar = '(?P<FCKRA%i>[FCKRA])?(?P<num%i> [0-9]*\.?[0-9]*)?'
  MayComma = '(?:, )?'
  res = '^Expected \(?' + (GetExpectChar % (0, 0))
  for i in range(1,5):
    res += MayComma + (GetExpectChar % (i, i))
  res += '\)?, got (?P<got>[FCKRA])(?P<gotNum> [0-9]*\.?[0-9]*)?\.$'
  return res

expectedGotRe = re.compile(constructExpectedGotRe())

def evalExpectedGotRe(l):
  m = expectedGotRe.match(l)
  if not m:
    return False
  else:
    fckra = []
    num = None
    gotNum = None
    for i in range(0,5):
      tmp = m.group('FCKRA' + str(i))
      if not tmp is None and len(tmp) > 0:
        fckra.append(tmp)
      tmp = m.group('num' + str(i))
      if not tmp is None and len(tmp.strip()) > 0:
        num = float(tmp)
        
    got = m.group('got')
    tmp = m.group('gotNum')
    if not tmp is None and len(tmp.strip()) > 0:
      gotNum = float(tmp)

    res = len(got) == 1 and got in fckra and ((num is None and gotNum is None) or (not num is None and num == gotNum))
    return res
      

def checkTestSuiteHand(testName, newLineEntries, expLineEntries, errors):
  mismatch = False
  for idx, nLE in enumerate(newLineEntries):
    eLE = None
    if idx < len(expLineEntries):
      eLE = expLineEntries[idx]
    
    if (not eLE is None and nLE.line == eLE.line) or mismatch:
      if not nLE.line.startswith('Expected '):
        continue
    else:
      if not eLE is None:
        createErrorEntry(testName, errors.cases, msg='Hand starts to be different at', nLE=nLE, eLE=eLE, showBoth=True)
        mismatch = True
      else:
        if len(expLineEntries) > 0:
          createErrorEntry(testName, errors.cases, msg='Hand starts to be different (should be finished already?) at', nLE=nLE, eLiNo=expLineEntries[-1].liNo)
        else:
          createErrorEntry(testName, errors.cases, msg='Hand starts to be different (should be finished already?) at', nLE=nLE)
        mismatch = True
      
    #this stuff ends up as cases and actions error, but think better that way
    if nLE.line.startswith('Expected '):
      rightRes = evalExpectedGotRe(nLE.line)
      if rightRes:
        continue
      else:
        if eLE is None or eLE.line != nLE.line:
          createErrorEntry(testName, errors.actions, msg='Choosen wrong action', nLE=nLE, eLE=eLE, expected=False)
        else:
          createErrorEntry(testName, errors.actions, msg='Choosen wrong action', nLE=nLE, eLE=eLE, expected=True)

def handleSymbolsAndStates(settings, testName, newLineEntries, expLineEntries, errors):
  import SymbolsAndStateHandler
  SymbolsAndStateHandler.handleSymbolsAndStates(settings, testName, newLineEntries, expLineEntries, errors)

def compareDumperTestCase(settings, testName, newLineEntries, expLineEntries, errors):
  findStringContainingError(testName, newLineEntries, expLineEntries, errors.errorStrs)
  handleSymbolsAndStates(settings, testName, newLineEntries, expLineEntries, errors)
    

def compareTestSuiteTestCase(testName, newLineEntries, expLineEntries, errors, isLast):
    #last 'case' for TestSuite is the Summary, treat it differently
    if not isLast:
      findStringContainingError(testName, newLineEntries, expLineEntries, errors.errorStrs)
      checkTestSuiteHand(testName, newLineEntries, expLineEntries, errors)
      
    else:
      analyseTestSuiteFinalLine(newLineEntries, expLineEntries, errors.summary)

def removeSkippedOnes(lineEntries1, lineEntries2 = None):
  lineEntries1[:] = [ x for x in lineEntries1 if not x.skip ]
  if not lineEntries2 is None:
    lineEntries2[:] = [ x for x in lineEntries2 if not x.skip ]


#helper class
class LineEntry:
  def __init__(self, line, lineNumber, idx):
    self.line = line
    self.liNo = lineNumber
    self.idx = idx
    #fixme: do we really use/need skip?
    self.skip = False


def createLineEntries(lines, firstLiNo):
  res = []
  if lines is None or len(lines) == 0:
    #tmpEnt = LineEntry(None, firstLiNo, 0)
    #tmpEnt.skip = True
    #res.append(tmpEnt)
    return res
  else:
    for idx, l in enumerate(lines):
      tmpEnt = LineEntry(l.strip(), firstLiNo + idx, idx)
      res.append(tmpEnt)
  return res

#helper class used by classes ErrorContainer
class ErrorContainerEntry:
    def __init__(self, msg):
      self.error = OrderedDict()
      self.warn = OrderedDict()
      self.msg = msg
    def numErrors(self):
      res = 0
      for l in self.error.values():
        res += len(l)
      return res
    def numWarns(self):
      res = 0
      for l in self.warn.values():
        res += len(l)
      return res

def compareDumperLogs(settings, cmpRes):
  printStartComparing(cmpRes)
  
  #fixme: use this to gather test cases mismatches too??
  class ErrorContainer:
    def __init__(self):
      self.errorStrs = ErrorContainerEntry("String 'ERROR' in file")
      self.symbols = ErrorContainerEntry("Symbols from dll mismatch")
      self.state = ErrorContainerEntry("State passed to dll mismatch")
      self.query = ErrorContainerEntry("Query call to dll mismatch")
      self.fatal = ErrorContainerEntry("Fatal or very bad Errors: tests maybe don't say much until you fix them")
      
  errors = ErrorContainer()
  
  #identical = True
  #errorMap = OrderedDict()
  #numExpectedErrors = 0
  for (testName, newCase, expCase, newStartL, expStartL, newMissing, isLast) in cmpRes.iterateCases():
    #create LineEntry instances
    newLineEntries = createLineEntries(newCase, newStartL)
    expLineEntries = createLineEntries(expCase, expStartL)
    
    compareDumperTestCase(settings, testName, newLineEntries, expLineEntries, errors)
  
  printErrorContainer(errors)
  
    
    

def compareTestSuiteLogs(cmpRes):
  printStartComparing(cmpRes)
  
    #fixme: use this to gather test cases mismatches too??
  class ErrorContainer:
    def __init__(self):
      self.errorStrs = ErrorContainerEntry("String 'ERROR' in file")
      self.summary = ErrorContainerEntry("Summary of errors and fails mismatch")
      self.cases = ErrorContainerEntry("Difference in execution of Test Cases")
      self.actions = ErrorContainerEntry("OH Dumper Dll Bot did not take the expected Action")
      self.fatal = ErrorContainerEntry("Fatal or very bad Errors: tests maybe don't say much until you fix them")
      
  errors = ErrorContainer()
  
  #errorMap = OrderedDict()
  numExpectedErrors = 0 # fixme:
  for (testName, newCase, expCase, newStartL, expStartL, newMissing, isLast) in cmpRes.iterateCases():
    #create LineEntry instances
    newLineEntries = createLineEntries(newCase, newStartL)
    expLineEntries = createLineEntries(expCase, expStartL)
    
    compareTestSuiteTestCase(testName, newLineEntries, expLineEntries, errors, isLast)
      
  printErrorContainer(errors)




def checkExists(fn):
  if os.path.exists(fn):
    return True
  else:
    print("ERROR: File '" + fn + "' missing")
    return False
  
def analyseLogs(settings, dumperLog, expectedDumperLog, testSuiteLog, expectedTestSuiteLog):
  #ensureExpectedLogExists maybe extracts ExpectedOutput.zip (function is defined in utils.py)
  ln()
  if not (testSuiteLog == '' or expectedTestSuiteLog == '') and checkExists(testSuiteLog) and extractFromZipIfNeeded(expectedTestSuiteLog, 'ExpectedOutput.zip'):
    cmpResTestSuite = CompareResult(testSuiteLog, expectedTestSuiteLog, False)
    compareTestSuiteLogs(cmpResTestSuite)
  ln()
  if not (dumperLog == '' or expectedDumperLog == '') and checkExists(dumperLog) and extractFromZipIfNeeded(expectedDumperLog, 'ExpectedOutput.zip'):
    cmpResDumper = CompareResult(dumperLog, expectedDumperLog, True)
    compareDumperLogs(settings, cmpResDumper)
    
    
def printUsage():
  print('Usage: if optional <Settings[.py]> is missing, SettingsDefault.py is used.')
  print('       Pass empty string "" to not evaluate a log file')
  print('       %s [<Settings[.py]>]' % (sys.argv[0]))
  print('       %s [<Settings[.py]>] <expectedDumperDllLog> <expectedTestSuiteLog>' % (sys.argv[0]))
  print('       %s [<Settings[.py]>] <newDumperDllLog> <newTestSuiteLog> <expectedDumperDllLog> <expectedTestSuiteLog>' % (sys.argv[0]))
    
if __name__ == '__main__':
  ln()
  printUsage()
  ln()
  SettingNames = ['ExpectedOutputDir', 'TestSuite2LogFilename', 'DumperDllLogFilename',
                  'TestSuite2ExpectedLogFilename', 'DumperDllExpectedLogFilename', 'TempRunDir',
                  'cardsTrunkVsOld', 'cardsOldVsTrunk', 'ignoreSymbolsPrefix', 'ignoreSymbolsExact' ]
  
  #### init settings: look if passed as first arg on command line,
  #### or from other script as gobal or local var, use default from SettingsDefault.py otherwise
  specificSettings = False
  if len(sys.argv) > 1 and not sys.argv[1].endswith(".log"):
    SettingsImportName = sys.argv[1]
    specificSettings = True
  else:
    SettingsImportName = None
  settings = loadSettings(SettingsImportName, SettingNames)
  
  ln()
  
  if (specificSettings and len(sys.argv) == 2) or (not specificSettings and len(sys.argv) == 1):
    newDumpLog = settings.TempRunDir + '\\' + settings.DumperDllLogFilename
    newTestSuiteLog = settings.TempRunDir + '\\' + settings.TestSuite2LogFilename
    expDumpLog = settings.ExpectedOutputDir + '\\' + settings.DumperDllExpectedLogFilename
    expTestSuiteLog = settings.ExpectedOutputDir + '\\' + settings.TestSuite2ExpectedLogFilename
  elif (specificSettings and len(sys.argv) == 4) or (not specificSettings and len(sys.argv) == 3):
    newDumpLog = settings.TempRunDir + '\\' + settings.DumperDllLogFilename
    newTestSuiteLog = settings.TempRunDir + '\\' + settings.TestSuite2LogFilename
    expDumpLog = sys.argv[2] if specificSettings else sys.argv[1]
    expTestSuiteLog = sys.argv[3] if specificSettings else sys.argv[2]
  elif (specificSettings and len(sys.argv) == 6) or (not specificSettings and len(sys.argv) == 5):
    newDumpLog = sys.argv[2] if specificSettings else sys.argv[1]
    newTestSuiteLog = sys.argv[3] if specificSettings else sys.argv[2]
    expDumpLog = sys.argv[4] if specificSettings else sys.argv[3]
    expTestSuiteLog = sys.argv[5] if specificSettings else sys.argv[4]
  else:
    sys.exit(0)
    
  analyseLogs(settings, newDumpLog, expDumpLog, newTestSuiteLog, expTestSuiteLog)
  
  
  