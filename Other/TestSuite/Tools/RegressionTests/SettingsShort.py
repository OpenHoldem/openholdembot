
#####################
## SettingsShort.py: for more params see SettingsDefault.py
#####################

#import default settings and then override some of them
from SettingsDefault import *



#where the testcases to execute are
TestcasesDir = 'Testcases\\TestsShort'

#output logfile names that are created and compared to expected output, pure filename, no dirs
TestSuite2LogFilename = 'ShortTestSuite.log'
DumperDllLogFilename = 'ShortDumperDll.log'
#Name files in Expected to compare to
TestSuite2ExpectedLogFilename = TestSuite2LogFilename
DumperDllExpectedLogFilename = DumperDllLogFilename

