
#####################
## Settings:
#####################

#do not close TestSuite2 Gui after tests, ignored if not showing gui
keepGuiOpen = True
#if to kill ManualMode-XMLRPC if finished
killManualMode = True

# 0=very fast, 2000=very slow
speed=0
#if to write the original log too (note: appending), if gui is shown it is always written
writeOrigLog = False


#####################
## the following are just default values, normally overridden by caller:
#####################

#if to show the TestSuite2 Gui
showAGui = True
#file is overridden on each run
logfilename='DumpTestTestSuite.log'
#directory with the test cases
testfiledir = 'testcases'


#####################

from testsuite import *
import threading
#for log stealing:
import types
import sys
from time import sleep

#steal all log messages, if showing gui we maybe also write the original log file
def stealLog(tsw, logfile, writeOrigLog):
    if writeOrigLog: tsw.tmpLogMeth = tsw.logMessage
    def logMessageDumpTest(self, msg, style = None):      
        logfile.write(msg + "\n")
        if writeOrigLog: self.tmpLogMeth(msg, style)
        if msg == "Can't connect to Manual Mode":
            print("ERROR: " + msg)
    tsw.logMessage = types.MethodType(logMessageDumpTest, tsw)


def startGUI(testfiledir, showAGui, logfilename):
    logfile = open(logfilename, "w")
    
    app = QApplication(sys.argv)
    tsw = TestsuiteWindow()
    
    stealLog(tsw, logfile, writeOrigLog or showAGui)
    
    testfiles = os.listdir(testfiledir)
    testfiles = [ testfiledir + "/" + x for x in testfiles ]
    testfiles.sort()
    print("Starting to execute " + str(len(testfiles)) + " test cases...")
    tsw.sliderSpeed.setValue(speed)
    tsw._addTestCasesToList(testfiles)
    #tsw.startExecutingAllTestCases()
        
    #helper function to kill gui if showing gui
    def clickExecuteAndWaitForEnd():
        #wait to be sure gui is started
        sleep(1)
        tsw.buttonExecuteAll.click()
        #if no Gui or should kill Gui after tests, wait till test thread finished and kill Gui/App
        if not keepGuiOpen or not showAGui:
            #wait till tests are running so thread exists
            sleep(1)
            if hasattr(tsw, 'testThread'):
                tsw.testThread.wait()
            tsw.actionExit.trigger()
    
    guiStopThread = threading.Thread(target=clickExecuteAndWaitForEnd, daemon=True)
    guiStopThread.start()
    if showAGui: tsw.show()
    app.exec_()

    logfile.close()
    if killManualMode:
        try:
            #this fails because app can not answer
            tsw.testThread.autoPlayer.mm.Quit()
        except:
            pass
    print("TestSuite2 execution done.")

if __name__ == "__main__":
    if len(sys.argv) > 1:
        testfiledir = sys.argv[1]
    if len(sys.argv) > 2:
        logfilename = testfiledir = sys.argv[2]
    startGUI(testfiledir, showAGui, logfilename)
    
    