#run test:
#start OpenHoldem
#start ManualMode-XMLRPC
#start TestSuite2 and start executing all tests
#if test finished:
# kill ManualMode-XMLRPC and with it OpenHoldem (TestSuite2 and ManualMode-XMLRPC are killed by runTestSuite2.py)
# evaluate result logs


#####################
## Settings:
#####################

#edit DefaultSettings.py or create new XYSettings.py file and create a new runTestXY.bat

#####################################################


SettingNames = [ 'WaitSecondsBeforeStartingTests', 'TestcasesDir', 'ExpectedOutputDir', 'TestSuite2LogFilename',
                'TestSuite2ExpectedLogFilename', 'DumperDllExpectedLogFilename',
                'DumperDllLogFilename', 'OpenHoldemExeDir', 'TempRunDir', 'TestSuite2Dir', 'ManualModeXMLRPCexe',
                'ManualModeXMLRPCTableMap', 'OHFilesToCopy', 'LocalFilesToCopy', 
                'cardsTrunkVsOld', 'cardsOldVsTrunk', 'ignoreSymbolsPrefix', 'ignoreSymbolsExact' ]

#####################################################

### loading global settings
import os
from utils import ln, loadSettings
#print line with '-----------...'
ln()

#### init settings: look if passed as first arg on command line,
#### or from other script as gobal or local var, use default from SettingsDefault.py otherwise
import sys
if __name__ == "__main__" and len(sys.argv) > 1 and not sys.argv[0].endswith(".log"):
  SettingsImportName = sys.argv[1]
else:
  SettingsImportName = None

settings = loadSettings(SettingsImportName, SettingNames)

settings.TestcasesDir = os.path.abspath(settings.TestcasesDir)

ln()


####################

import re
import glob
from os.path import join
from subprocess import call
from shutil import copy2

import threading
from time import sleep

#make dir of this script the current dir such that stuff is found with relative addressing:
#in case called from other dirs
import utils
this_scripts_path = utils.module_path()
origWorkDir = os.getcwd()
print("changing working dir to '%s'" % (this_scripts_path))
os.chdir(this_scripts_path)


#we must add TestSuite2 to python lib path to use it
#fixme: maybe do it in runTestSuite2.py
sys.path = [os.path.abspath(settings.TestSuite2Dir)] + sys.path
import runTestSuite2
import analyseOutput

#####################################################


#we should delete ini files in target dir for OH to have just one
def cleanTargetDir():
  files = os.listdir(settings.TempRunDir)
  for f in files:
    if f.endswith(".ini"):
      f = join(settings.TempRunDir, f)
      print("Deleting '%s'" % (f))
      os.remove(f)
  warnfile = join(settings.TempRunDir, "WARN_files_in_here_are_delete_or_changed_automatically.txt")
  if not os.path.exists(warnfile):
    out = open(warnfile, 'w')
    out.write('Warning:\nFiles in this directory are deleted or changed automatically by the tests.\n')
    out.write('Do not change the files in here but the original files in\n' + os.getcwd() + '\n')
    out.write('or the OH release dir.\n\n')
    out.close()
    

def smartCopyFile(src, dest):
    #todo: add exception handler
    # copy if more than 1 second difference
    if not os.path.exists(dest) or os.stat(src).st_mtime - os.stat(dest).st_mtime > 1 or os.stat(src).st_mtime - os.stat(dest).st_mtime < -1:
        print("copying file '" + src + "' to '" + dest + "'")
        copy2(src, dest)
  
def copyNeededFiles():
  for f in settings.LocalFilesToCopy:
    smartCopyFile(f, settings.TempRunDir + '\\' + os.path.basename(f))
  for f in settings.OHFilesToCopy:
    #supporting wildcards in this makes it ugly
    if f[1] != ':':
        src = settings.OpenHoldemExeDir + '\\' + f
    else:
        src = f
    if '*' in src:
        src = glob.glob(src)[0]
    srcBase = os.path.basename(src)
    #logic for scraper dir would no longer be needed, is config param of its own now
    if os.path.basename(os.path.dirname(src)) == 'scraper':
        dest = settings.TempRunDir + '\\' + 'scraper\\' + srcBase
    else:
        dest = settings.TempRunDir + '\\' + srcBase
    smartCopyFile(src, dest)
  #handle tablemap file, delet existing if they have other name
  src = glob.glob(settings.ManualModeXMLRPCTableMap)[0]
  glob_existing_basname = '*.tm' # can give problems if somebody hardcodes a version path in config: os.path.basename(settings.ManualModeXMLRPCTableMap)
  for exisingTm in glob.glob(settings.TempRunDir + '\\' + 'scraper\\' + glob_existing_basname):
    if os.path.basename(src) != os.path.basename(exisingTm):
      print("Deleting '" + exisingTm + "' because we now use '" + os.path.basename(src) + "'")
      os.remove(exisingTm)
  smartCopyFile(src, settings.TempRunDir + '\\' + 'scraper\\' + os.path.basename(src))
      

def createDumperDllCfg():
    fn = settings.TempRunDir + '\\' + 'DumperDll.cfg'
    print("creating '" + fn + "'")
    out = open(fn, 'w')
    out.write('[DumperDllCfg]\n')
    out.write('dumpOutFileName=' + settings.DumperDllLogFilename + '\n')
    out.write('testfilesDir=' + settings.TestcasesDir + '\n')
    out.write('\n')
    out.close()
    
def executeCommandInThread(cmd, waitSecs):
    def doCall():
      sleep(waitSecs)
      call(cmd)
    print("executing in thread: " + str(cmd))
    t = threading.Thread(target=doCall, daemon=True)
    t.start()
    
  
def runTestSuite(logfile):
    print("running TestSuite2")
    #don't show the gui
    runTestSuite2.startGUI(settings.TestcasesDir, False, logfile)



################## main function

def runTest():
  ln()
  #make dirs
  if not os.path.exists(settings.TempRunDir):
    os.makedirs(settings.TempRunDir)
  if not os.path.exists(join(settings.TempRunDir, 'scraper')):
    os.makedirs(join(settings.TempRunDir, 'scraper'))
  
  #copy/generate stuff
  cleanTargetDir()
  copyNeededFiles()
  createDumperDllCfg()
  dumperLog = os.path.abspath(settings.TempRunDir + '\\' + settings.DumperDllLogFilename)
  testSuiteLog = os.path.abspath(settings.TempRunDir + '\\' + settings.TestSuite2LogFilename)
  
  executeCommandInThread(settings.ManualModeXMLRPCexe, 0)
  
  #ensure stuff is started
  sleep(1)
  
    #don't do this too early, we copy relative, OH needs OH dir as current dir
  os.chdir(settings.TempRunDir)
  #start OH
  #get ini file:
  OHIni = [ x for x in settings.LocalFilesToCopy if x.endswith(".ini") ][0]
  executeCommandInThread(['OpenHoldem.exe', '/ini:~\\' + OHIni], settings.WaitSecondsBeforeStartingTests)
  #wait to ensure that OH is open, such that MM is in foreground
  
  ln()
  runTestSuite(testSuiteLog)
  ln()
  
  #come back to our dir as working dir
  os.chdir(this_scripts_path)
  
  print("Testing done. Analysing results...")
  expDumperLog = os.path.abspath(settings.ExpectedOutputDir + '\\' + settings.DumperDllExpectedLogFilename)
  expTestSuiteLog = os.path.abspath(settings.ExpectedOutputDir + '\\' + settings.TestSuite2ExpectedLogFilename)
  analyseOutput.analyseLogs(settings, dumperLog, expDumperLog, testSuiteLog, expTestSuiteLog)
  ln()

  #finally restore orig working dir
  print("restoring working dir to '%s'" % (origWorkDir))
  os.chdir(origWorkDir)
  ln()

    
if __name__ == '__main__':
  runTest()
  
  
  