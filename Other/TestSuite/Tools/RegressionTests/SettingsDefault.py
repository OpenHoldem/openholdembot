
#####################
## Settings: what you want to change are probably the first 6 or 7 properties
#####################


#where the testcases to execute are
TestcasesDir = 'Testcases\TestsDefault'

#output logfile names that are created and compared to expected output, pure filename, no dirs
TestSuite2LogFilename = 'DefaultTestSuite.log'
DumperDllLogFilename = 'DefaultDumperDll.log'
#Name files in Expected to compare to
TestSuite2ExpectedLogFilename = TestSuite2LogFilename
DumperDllExpectedLogFilename = DumperDllLogFilename

#how long to wait after starting ManualMode-XMLRPC and OH before starting tests,
#raise if your ManualMode-XMLRPC opens outside the screen such that you have time to move it,
#don't set to 0 to be sure stuff is started
WaitSecondsBeforeStartingTests = 3

#must be in dir of this file, no wildcards, pure filename, no dirs
#first file ending in '.ini' is assumed to be OH ini file
LocalFilesToCopy = [ 'OH_default.ini', 'OH_default.ohf']

##############################

#ouput is compared to logfiles with same names in this dir, relative to dir of this file or absolute
ExpectedOutputDir = 'ExpectedOutput'

#allows wildcards
ManualModeXMLRPCTableMap = '..\\##_OpenHoldem_Release_Directory_##\\scraper\\OpenHoldem.ManualMode*.tm'

#dir of OpenHoldem.exe, so OpenHoldem Release dir
OpenHoldemExeDir = '..\\release'
#OpenHoldemExeDir = '..\\Debug'

#OH files to copy
#allows * as only wildcard (but only first match is copied) and relative and absolute paths,
#relative paths are relative to OpenHoldemExeDir
OHFilesToCopy = [ 'OpenHoldem.exe', 'mouse.dll', 'keyboard.dll',
    'DumperDll.dll', 'pokertracker_query_definitions.dll' ]

#temp dir where tests are run
#copies OH and all needed files to that dir, creates it if not there
TempRunDir = 'temp_run_dir'

#location of TestSuite2 source files
TestSuite2Dir = 'Tools\\TestSuite2'

#ManualMode-XMLRPC exe file
ManualModeXMLRPCexe = '..\\release\\ManualMode-XMLRPC.exe'


#####################################################

#special: to compare to earlier OH or vice-versa where card values were different
cardsTrunkVsOld = False
cardsOldVsTrunk = False
ignoreSymbolsPrefix = []
ignoreSymbolsExact = []


