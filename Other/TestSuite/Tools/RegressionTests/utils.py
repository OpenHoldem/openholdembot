# helper methods and trick to find the directory our scripts are in (for cases it is called from
#.bat files or scripts in other directories).

#####################################################

#find script directory trick (cannot be in main py file):
#see: http://stackoverflow.com/questions/2632199/how-do-i-get-the-path-of-the-current-executed-file-in-python

import sys
import os

def we_are_frozen():
    # All of the modules are built-in to the interpreter, e.g., by py2exe
    return hasattr(sys, "frozen")

def module_path():
    if we_are_frozen():
        return os.path.dirname(sys.executable)
    return os.path.dirname(__file__)
  

#####################################################

#helper methods:

import glob
import zipfile


def ln():
  print('-' * 70)


DefaultSettingsPyFile = "SettingsDefault.py"
settings = None
loadedSettings = None

#can be loaded only once, after that original settings are returned: that means original loader must load all needed settings at once!!!
def loadSettings(settingsFile, settingNames):
  global settings, loadedSettings
  if settingsFile is None:
    settingsFile = DefaultSettingsPyFile
    
  if settingsFile.endswith(".py"):
    settingsFile = settingsFile[:-3]
    
  if settings is None:
    #import the settings
    print("trying to load settings '%s':" % (settingsFile))
    settings = __import__(settingsFile, globals(), locals(), settingNames, 0)
    loadedSettings = settingsFile
  elif loadedSettings == settingsFile:
    print("Settings '%s' already loaded earlier, verifying all needed properties have been loaded." % (settingsFile))
  else:
    print("Settings '%s' already loaded earlier, now trying to load ''. You can only use one settings file." % (loadedSettings, settingsFile))
  for name in settingNames:
    if not hasattr(settings, name):
      print('ERROR: Setting not defined: ' + name)
      sys.exit(1)
    val = getattr(settings, name)
    print("%s = '%s'" % (name, str(val)))
  return settings


def getFileInfo(path):
  class FInfo:
    pass
  res = FInfo()
  res.path = path
  res.exists = os.path.exists(path)
  res.ts = None
  if res.exists:
    res.ts = os.stat(path).st_mtime
  return res

#Does maybe extract files from XY.zip if it contains the file and zip is newer than file or file missing,
#assumes that zip file is in folder of the file that we are looking for.
#Supports wildcard '*' for the filename, in that case the time stamp of just the first match is used and
#the whole zip file is extracted !!!
def extractFromZipIfNeeded(targetFileGlob, zipFilename):
  expectDir = os.path.dirname(targetFileGlob)
  #just look for the first if glob
  if '*' in targetFileGlob:
    firstTargetFilename = glob.glob(targetFileGlob)[0] if len(glob.glob(targetFileGlob)) > 0 else None
  else:
    firstTargetFilename = targetFileGlob
  
  if not firstTargetFilename is None:
    targetF = getFileInfo(firstTargetFilename)
  zipF = getFileInfo(expectDir + "\\" + zipFilename)
  
  if zipF.exists:
    extractZip = False
    zf = zipfile.ZipFile(zipF.path, 'r')
    if targetF is None:
      extractZip = True
      reason = "no file matching '" + targetFileGlob + "' found"
    elif os.path.basename(targetF.path) in zf.namelist():
      if not targetF.exists:
        extractZip = True
        reason = "file '" + targetF.path + "' does not exist but is in zip"
      elif targetF.exists and zipF.ts - targetF.ts > 1:
        extractZip = True
        reason = "file '" + targetF.path + "' older than zip file and is in zip"  
    if extractZip:
      if '*' in targetFileGlob:
        #extract all
        print("Trying to extract '" + zipF.path + "' (" + reason + ')')
        zf.extractall(expectDir)
      else:
        #extract single file
        print("Trying to extract single file from '" + zipF.path + "' (" + reason + ')')
        zf.extract(os.path.basename(firstTargetFilename), expectDir)
    zf.close()
    #re-eval glob
    if '*' in targetFileGlob:
      firstTargetFilename = glob.glob(targetFileGlob)[0] if len(glob.glob(targetFileGlob)) > 0 else None
      
  #check if we have a target file now
  if not firstTargetFilename is None:
    targetF = getFileInfo(firstTargetFilename)
    if targetF.exists:
      return True
  print('ERROR: ' + targetFileGlob + ' does not exist')
  return False


  
  