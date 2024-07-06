# parse and analyse dll dumped state and symbols
# uses classes defined in analyseOutput.py
    
################################################

from collections import OrderedDict

newSymbolsPrevDumpThisHand = None
newSymbolsFinalPrevHand = None
newSymbolsPrevBetround = None

#symbol notes:
#handsplayed: at least in OH 2.1.9.4 it sometimes randomly increments more than once for otherwise identical tests, we just check that it changes for new hand


#eval order is important!

#return error if value is not different than in previous hand
symMustChangeBetweenHands = ['handsplayed', 'randomhand']

#return error if value changes during hand
symMustBeSameDuringHand = ['handsplayed', 'randomhand']

#return error if value does not change per heartbeat
symMustChangeEachHeardbeat = ['random', 'randomheartbeat']

#return error if value same as in prev betround (or hand)
symMustChangePerBetround = ['betround', 'br', ]

#return error if value different than in previous betround
symMustBeSameDuringBetround = ['betround', 'br', ]


#just prefix is enough ! NO error for these if value not same as in expected log, but maybe it is treated by one of the previous rules
symDontRequireSameValuePrefixes = ['random', 'clocks', 'elapsed', 'ron$clocks', 'run$clocks', 'handsplayed', 'version']

def floatEq(a, b):
  return abs(a-b) < 0.0001

cardSymbolPrefix = ['$$cc', '$$pc', '$$cs', '$$ps', 'tsuit', 'tsuitcommon']
def convertCardValue(settings, name, newVal, newSymbols): #that ugly because 0 is valid suite and no-card at the same time in trunk atm
  if settings.cardsTrunkVsOld:
    newVal = int(newVal)
    #for trunk (compared to old) we get 0 for h(3), 3 for s(4), 1 for d(2), 2 for c(1)
    if newVal == 253 or newVal == 254: #usage of 253 seems to be a bug
      newVal = 0
    #we have to check if rank is not 0 (means no card), ugly, should be fixed in trunk...
    elif newVal != 0 or (name[3] == "s" and newSymbols.getValue(name[:3] + "r" + name[4:]) != 0) or (name[0] == "t" and newSymbols.getValue("nsuited" + name[5:]) > 0):
      suit = newVal & 0xf
      if suit == 0:
        suit = 3
      elif suit == 3:
        suit = 4
      elif suit == 1:
        suit = 2
      elif suit == 2:
        suit = 1
      newVal = (newVal & ~0xf) | suit
  elif settings.cardsOldVsTrunk:
    #//dw FIXME: TODO: add code for cardsOldVsTrunk card conversion
    pass
  return float(newVal)

#if returns False an error that the symbol value changed is logged. For other errors create the error here and return True
def compareSymbol(settings, testName, name, newSym, expSym, newSymbols, expSymbols, errors):
  newVal = newSym.value
  newLiNo = newSym.liNo
  expVal = expSym.value
  expLiNo = expSym.liNo
  if name in symMustChangeBetweenHands and not newSymbolsFinalPrevHand is None and newVal == newSymbolsFinalPrevHand.getValue(name):
    createErrorEntry(testName, errors.symbols, msg="Symbol '" + name + "' should change value between hands but does not", nLiNo=newLiNo)
    return True
  if name in symMustBeSameDuringHand and not newSymbolsPrevDumpThisHand is None and newVal != newSymbolsPrevDumpThisHand.getValue(name):
    createErrorEntry(testName, errors.symbols, msg="Symbol '" + name + "' should not change value during hand but does", nLiNo=newLiNo)
    return True
  #fixme: what if we dumped multiple times in one heardbeat (e.g. prefold call and alli call, or wait and alli, or 2 alli because of illegal action)???
  if name in symMustChangeEachHeardbeat and not newSymbolsPrevDumpThisHand is None and newVal == newSymbolsPrevDumpThisHand.getValue(name):
    createErrorEntry(testName, errors.symbols, msg="Symbol '" + name + "' should change each heartbeat but does not", nLiNo=newLiNo)
    return True
  if name in symMustChangePerBetround and not newSymbolsPrevBetround is None and newVal == newSymbolsPrevBetround.getValue(name):
    createErrorEntry(testName, errors.symbols, msg="Symbol '" + name + "' should change each betround but does not", nLiNo=newLiNo)
    return True
  if name in symMustBeSameDuringBetround and not newSymbolsPrevDumpThisHand is None and newSymbols.getValue('betround') ==  newSymbolsPrevDumpThisHand.getValue('betround') and newVal != newSymbolsPrevDumpThisHand.getValue(name):
    createErrorEntry(testName, errors.symbols, msg="Symbol '" + name + "' should not change value during betround but does", nLiNo=newLiNo)
    return True
  for prefix in symDontRequireSameValuePrefixes:
    if name.startswith(prefix):
      return True
    
  #convert card values between different OH versions if needed
  for p in cardSymbolPrefix:
    if name.startswith(p):
      newVal = convertCardValue(settings, name, newVal, newSymbols)
  #return newVal == expVal
  return floatEq(newVal, expVal)
  

################################################
#class defs:

import re
from analyseOutput import createErrorEntry, createErrorMapEntry

class NamedValue:
  def __init__(self, name, value, liNo):
    self.name = name
    self.value = value
    self.liNo = liNo

#Symbols and State are instances of the following class, the map items are of class NamedValue

class NamedValueContainer:
  def __init__(self, inHandMsgNum, isAllInMsg, liNo):
    self.map_ = OrderedDict()
    self.inHandMsgNum = inHandMsgNum
    self.isAllInMsg = isAllInMsg
    self.liNo = liNo
  def add(self, name, value, liNo):
    self.map_[name] = NamedValue(name, value, liNo)
  def get(self, name):
    if not name in self.map_:
      return None
    return self.map_[name]
  def getValue(self, name):
    if not name in self.map_:
      return None
    return self.map_[name].value
  def keys(self):
    return self.map_.keys()
  def values(self):
    return self.map_.values()
    
################################################
def isSameBoard(syms1, syms2):
  boardTheSame = True
  for i in range(0,5):
    boardTheSame &= syms1.getValue('$$cc' + str(i)) == syms2.getValue('$$cc' + str(i))

def compareSymbols(settings, testName, newSymbols, expSymbols, errors):
  #fixme: bad idea? config value to allow change? for sure since symbols can be added or removed to OH
  if len(newSymbols.keys()) != len(expSymbols.keys()):
    createErrorEntry(testName, errors.symbols, msg='Number of symbols does not match', nLiNo=newSymbols.liNo, eLiNo=expSymbols.liNo)
  #do some extra eval on betround since we rely on it!
  if not newSymbolsPrevDumpThisHand is None:
    if newSymbols.getValue('betround') ==  newSymbolsPrevDumpThisHand.getValue('betround') and not isSameBoard(newSymbols, newSymbolsPrevDumpThisHand):
      createErrorEntry(testName, errors.symbols, msg='Board cards changed during betround', nLiNo=newSymbols.liNo)
    elif newSymbols.getValue('betround') !=  newSymbolsPrevDumpThisHand.getValue('betround') and isSameBoard(newSymbols, newSymbolsPrevDumpThisHand):
      createErrorEntry(testName, errors.symbols, msg='Board cards the same but is new betround', nLiNo=newSymbols.liNo)
      
  for n in newSymbols.keys():
    
    #to compare with earlier OH versions with different symbols
    #and some symbols are new or do no longer exist
    ignoreSym = False
    for p in settings.ignoreSymbolsPrefix:
      if n.startswith(p):
        ignoreSym = True
        break
    if ignoreSym or n in settings.ignoreSymbolsExact:
      continue
    
    newNV = newSymbols.get(n)
    expNV = expSymbols.get(n)
    newVal = newNV.value
    newLiNo = newNV.liNo
    expVal = None
    expLiNo = expSymbols.liNo
    if not expNV is None:
      expVal = expNV.value
      expLiNo = expNV.liNo
      if not compareSymbol(settings, testName, n, newNV, expNV, newSymbols, expSymbols, errors):
        createErrorEntry(testName, errors.symbols, msg="Symbol '" + n + "' does not match: '" + str(newVal) + "' != '" + str(expVal) + "'", nLiNo=newLiNo, eLiNo=expLiNo)
    else:
      createErrorEntry(testName, errors.symbols, msg="Symbol '" + n + "' missing in expected output log", nLiNo=newLiNo, eLiNo=expSymbols.liNo)
    


def compareStates(testName, newState, expState, errors):
  if len(newState.keys()) != len(expState.keys()):
    createErrorEntry(testName, errors.state, msg='Number of entries in state does not match', nLiNo=newState.liNo, eLiNo=expState.liNo)
  for n in newState.keys():
    newNV = newState.get(n)
    expNV = expState.get(n)
    newVal = newNV.value
    newLiNo = newNV.liNo
    expVal = None
    expLiNo = None
    if not expNV is None:
      expVal = expNV.value
      expLiNo = expNV.liNo
      if expVal != newVal:
        createErrorEntry(testName, errors.state, msg="State entry '" + n + "' does not match: '" + str(newVal) + "' != '" + str(expVal) + "'", nLiNo=newLiNo, eLiNo=expLiNo)
    else:
      createErrorEntry(testName, errors.state, msg="State entry '" + n + "' missing in expected output log", nLiNo=newLiNo, eLiNo=expState.liNo)
    

#returns allInCaseSymPairList, sets hash for all sym/state combos
def hashAndFindAllInStates(symStatePairList):
  allInCaseSymPairList = []
  for syms, state in symStatePairList:
    if syms.isAllInMsg and state.isAllInMsg:
      allInCaseSymPairList.append((syms, state))
    #create hash to know if it is for the same table situation
  return allInCaseSymPairList


   
   
namedValueLineRe = re.compile("^([^=]+)=(.*)$")
beginSymbolDumpRe = re.compile('^==== BEGIN SYMBOLS DUMP inHandMsgNum=([0-9]*) isAllInMsg=([0-9]*) .*')
endSymbolDumpRe = re.compile(  '^==== END SYMBOLS DUMP inHandMsgNum=([0-9]*) isAllInMsg=([0-9]*) .*')
beginStateDumpRe = re.compile( '^==== BEGIN STATE DUMP inHandMsgNum=([0-9]*) isAllInMsg=([0-9]*) .*')
endStateDumpRe = re.compile(   '^==== END STATE DUMP inHandMsgNum=([0-9]*) isAllInMsg=([0-9]*) .*')
    
#we dump symbols first and then the state
def parseNamedValues(testName, lineEntries, isExpectedLog, errors):
  inParseSymbolMode = False
  inParseStateMode = False
  curSymbolContainer = None
  curContainer = None
  newLiNo = None
  expLiNo = None
  containerPairList = []
  inExpStr = ''
  if isExpectedLog:
    inExpStr = '(in expected log!): '
  for e in lineEntries:
    #ensure we log line numbers for the correct log file
    if not isExpectedLog:
      newLiNo = e.liNo
      expLiNo = None
    else:
      newLiNo = -1
      expLiNo = e.liNo
    mSym = beginSymbolDumpRe.match(e.line)
    mState = beginStateDumpRe.match(e.line)
    if mSym or mState:
      if inParseSymbolMode or inParseStateMode:
        createErrorEntry(testName, errors.fatal, msg=inExpStr + 'Next Dump seems to start inside another dump', nLiNo=newLiNo, eLiNo=expLiNo)
      if mSym:
        inParseSymbolMode = True
        inParseStateMode = False
        curContainer = NamedValueContainer(int(mSym.group(1)), bool(mSym.group(2)), e.liNo)
      elif mState:
        inParseSymbolMode = False
        inParseStateMode = True
        curContainer = NamedValueContainer(int(mState.group(1)), bool(mState.group(2)), e.liNo)
      continue
    
    if endSymbolDumpRe.match(e.line):
      if not inParseSymbolMode or inParseStateMode:
        createErrorEntry(testName, errors.fatal, msg=inExpStr + 'Unexpected Symbol Dump end marker', nLiNo=newLiNo, eLiNo=expLiNo)
      #we add it to result if we have state too
      inParseSymbolMode = False
      inParseStateMode = False
      curSymbolContainer = curContainer
      curContainer = None
      continue
      
    if endStateDumpRe.match(e.line):
      if inParseSymbolMode or not inParseStateMode:
        createErrorEntry(testName, errors.fatal, msg=inExpStr + 'Unexpected State Dump end marker', nLiNo=newLiNo, eLiNo=expLiNo)
      if curSymbolContainer is None:
        createErrorEntry(testName, errors.fatal, msg=inExpStr + "Fatal: State without previous matching Symbols", nLiNo=newLiNo, eLiNo=expLiNo)
        curSymbolContainer = NamedValueContainer(curContainer.inHandMsgNum, curContainer.isAllInMsg, curContainer.liNo)
      if curSymbolContainer.isAllInMsg != curContainer.isAllInMsg:
        createErrorEntry(testName, errors.fatal, msg=inExpStr + "Fatal: 'isAllInMsg' of state and symbols not matching", nLiNo=newLiNo, eLiNo=expLiNo)
      inParseSymbolMode = False
      inParseStateMode = False
      containerPairList.append((curSymbolContainer, curContainer))
      curSymbolContainer = None
      curContainer = None
      continue
    
    if inParseSymbolMode or inParseStateMode:
      m = namedValueLineRe.match(e.line)
      if m:
        curContainer.add(m.group(1), m.group(2), e.liNo)
      else:
        createErrorEntry(testName, errors.fatal, msg=inExpStr + 'In Dump and cannot parse line as named value', nLine=e.line, nLiNo=newLiNo, eLiNo=expLiNo)
  
  if inParseSymbolMode or inParseStateMode:
    createErrorEntry(testName, errors.fatal, msg=inExpStr + 'Dump seems to have no end inside test case', nLiNo=newLiNo, eLiNo=expLiNo)
  return containerPairList    


def castSymbolValuesToFloat(symList1, symList2):
  for symList in (symList1, symList2):
    for symContainer, stateContainer in symList:
      for sym in symContainer.values():
        sym.value = float(sym.value)

#fixme: ensure that only called with non-empty lists
def handleSymbolsAndStates(settings, testName, newLineEntries, expLineEntries, errors):
  newSymbolsPrevDumpThisHand = None
  newSymbolsPrevBetround = None
  curBetround = -1
  
  newSymStatePairList = parseNamedValues(testName, newLineEntries, False, errors)
  expSymStatePairList = parseNamedValues(testName, expLineEntries, True, errors)
  
  #print("DEBUG: len(newLineEntries)=%i len(expLineEntries)=%i len(newSymStatePairList)=%i len(expSymStatePairList)=%i" % (len(newLineEntries), len(expLineEntries), len(newSymStatePairList), len(expSymStatePairList))) 
  
  castSymbolValuesToFloat(newSymStatePairList, expSymStatePairList)
  
  newAllInCaseSymPairList = hashAndFindAllInStates(newSymStatePairList)
  expAllInCaseSymPairList = hashAndFindAllInStates(expSymStatePairList)
  
  #fixme: the first one is empty!!!!
  #if len(newAllInCaseSymPairList) == 0:
  #  createErrorEntry(testName, errors.fatal, msg='No all-in messages in this test case', nLiNo=newLineEntries[0].liNo, eLiNo=expLineEntries[0].liNo)
  #  return
  
  if len(newAllInCaseSymPairList) != len(expAllInCaseSymPairList):
    if len(newLineEntries) > 0 and  len(expLineEntries) > 0:
        createErrorEntry(testName, errors.fatal, msg='Number of all-in messages in this test case do not match', nLiNo=newLineEntries[0].liNo, eLiNo=expLineEntries[0].liNo)
    elif len(newLineEntries) > 0:
        createErrorEntry(testName, errors.fatal, msg='Number of all-in messages in this test case do not match', nLiNo=newLineEntries[0].liNo)
    else:
        createErrorEntry(testName, errors.fatal, msg='Number of all-in messages in this test case do not match')
  
  for i in range(0, min(len(newAllInCaseSymPairList), len(expAllInCaseSymPairList))):
    newSymbols = newAllInCaseSymPairList[i][0]
    expSymbols = expAllInCaseSymPairList[i][0]
    newState = newAllInCaseSymPairList[i][1]
    expState = expAllInCaseSymPairList[i][1]
    compareStates(testName, newState, expState, errors)
    compareSymbols(settings, testName, newSymbols, expSymbols, errors)
    newSymbolsPrevDumpThisHand = newSymbols
    if curBetround + 0.1 < newSymbols.getValue('betround'):
      newSymbolsPrevBetround = newSymbols
      curBetround = newSymbols.getValue('betround')
    
  newSymbolsFinalPrevHand = newSymbolsPrevDumpThisHand
  
  


