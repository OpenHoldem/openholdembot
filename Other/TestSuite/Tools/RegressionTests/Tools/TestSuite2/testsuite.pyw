import sys
import os
import datetime
from ui_testsuite import Ui_TestSuite
from parsers.tcparser import TestCaseParser, ParserException
from testsuite_utility import ItemIcon, LogStyle
from PyQt4.QtGui import QMainWindow, QKeySequence, QShortcut, QIcon, QFileDialog,\
    QListWidgetItem, QApplication
from PyQt4.QtCore import SIGNAL, Qt
from test.testthread import TestThread

     
class TestsuiteWindow(QMainWindow, Ui_TestSuite):

    def __init__(self, parent=None):
        super(TestsuiteWindow, self).__init__(parent)
        
        self.setupUi(self)         
        self._connectSignals()      
        self._loadIcons()  
        self.testCollectionFile = None   
        self._lastDirectory = "."
        
    def dragEnterEvent(self, event):
        """ Drag enter mouse event"""
        if event.mimeData().hasUrls:
            event.accept()
        else:
            event.ignore()

    def dropEvent(self, event):
        """ Drop mouse event. Accept if urls contain test cases or test collection."""
        if event.mimeData().hasUrls:
            event.setDropAction(Qt.CopyAction)
            
            files = []
            tcFile = None
            for url in event.mimeData().urls():
                file = url.toLocalFile()
                extension = os.path.splitext(file)[1]
                if extension == ".txt":
                    files.append(file)
                elif extension == ".tc":
                    tcFile = file
            if tcFile:
                event.accept()
                self._loadTestCollectionFile(tcFile)
            if files:
                event.accept()
                self._addTestCasesToList(files)  
            else:
                event.ignore()       
        else:
            event.ignore()
        
    
    def _connectSignals(self):
        
        #connect buttons
        self.connect(self.buttonExecute,SIGNAL("clicked()"), self.startExecutingTestCases)
        self.connect(self.buttonExecuteAll,SIGNAL("clicked()"), self.startExecutingAllTestCases)
        self.connect(self.buttonStop,SIGNAL("clicked()"), self.stopExecuting)
        self.connect(self.buttonAdd,SIGNAL("clicked()"), self.addTestCases)
        self.connect(self.buttonEdit,SIGNAL("clicked()"), self.editSelectedItem)
        self.connect(self.buttonRemove,SIGNAL("clicked()"), self.removeTestCases)
                
        #connect actions
        self.connect(self.actionNew,SIGNAL("triggered()"), self.newTestCollection)
        self.connect(self.actionOpen,SIGNAL("triggered()"), self.openTestCollection)
        self.connect(self.actionSave,SIGNAL("triggered()"), self.saveTestCollection)
        self.connect(self.actionSaveAs,SIGNAL("triggered()"), self.saveAsTestCollection)
        self.connect(self.actionExit,SIGNAL("triggered()"), self.close)
        
        #connect list handling
        self.connect(self.listTestCollection,SIGNAL("itemDoubleClicked(QListWidgetItem *)"), self.editTestcase)
        delKey = QShortcut(QKeySequence(Qt.Key_Delete), self.listTestCollection)
        self.connect(delKey, SIGNAL('activated()'), self.removeTestCases)
        self.connect(self.listTestCollection,SIGNAL("itemSelectionChanged ()"), self.listSelectionChanged)

    def _loadIcons(self):
        self.iconSuccess = QIcon("images/circle_green") 
        self.iconError = QIcon("images/circle_error")
        self.iconFailed = QIcon("images/circle_red")
        self.iconDefault = QIcon("images/circle_grey")
        
    def addTestCases(self):
        """Add test cases clicked. Open file selection dialog and add selection to list """
        if(self._isExecuting()):
            self.stopExecuting()
        fnames = QFileDialog.getOpenFileNames(self, "Select test cases", self._lastDirectory, "Text files (*.txt)")
        self._addTestCasesToList(fnames)
        
    def _isExecuting(self):
        """ Testsuite is executing test case if stop button is enabled and test collection is not empty"""
        return self.buttonStop.isEnabled() and self.listTestCollection.count() > 0
    
    
    def editTestcase(self, item):
        """Edit test case in default editor """      
        file = item.data(Qt.UserRole) # get file path
        os.startfile(file, 'open')
        
    def editSelectedItem(self):
        """User wants to edit the selected item"""
        if(self.listTestCollection.count() > 0):
            self.editTestcase(self.listTestCollection.selectedItems()[0])
            
    def _fnameInCollection(self,fname):
        """ Check if file name is in test collection"""
        items = [self.listTestCollection.item(i) for i in range(self.listTestCollection.count())]
        for item in items:
            if(fname == item.data(Qt.UserRole)):
                return True
        return False
        
           
    def _addTestCasesToList(self, fnames): 
        """Add all files to the list and parse them"""       
        for file in fnames: 
            
            if(self._fnameInCollection(file)):
                self.logMessage("File {0} already in test collection".format(os.path.basename(file)))
            else:            
                item = QListWidgetItem(os.path.basename(file))
                item.setData(Qt.UserRole,file)
                try:
                    tcParser = TestCaseParser(file)
                    tcParser.parse()
                    self.setItemIcon(item)
                    self.logMessage("Added file {0}".format(os.path.basename(file)))
                except ParserException as e:
                    self.setItemIcon(item, ItemIcon.ERROR)
                    self.logMessage("Parsing error in file {0}. {1}".format(os.path.basename(file),str(e)),LogStyle.ERROR)
    
                self.listTestCollection.addItem(item)
                self._updateLastDirectory(file) 
            
        self.updateButtonsToListChange() 
        self.listTestCollection.sortItems()
            
    def listSelectionChanged(self):
        """List selection changed -> update buttons based on selection """
        if(len(self.listTestCollection.selectedItems()) > 0):
            if(not self._isExecuting()):
                self.buttonExecute.setEnabled(True)
            self.buttonEdit.setEnabled(True)
            self.buttonRemove.setEnabled(True)
        else:
            self.buttonExecute.setEnabled(False)
            self.buttonEdit.setEnabled(False)
            self.buttonRemove.setEnabled(False)
            
    def removeTestCases(self):
        """Remove selected test cases from the list """
        if(self._isExecuting()):
            self.stopExecuting()
            
        newSelIndex = 0
        if(len(self.listTestCollection.selectedItems()) > 0):
            newSelIndex = self.listTestCollection.indexFromItem(self.listTestCollection.selectedItems()[0]).row() -1
        
        for item in self.listTestCollection.selectedItems():
            self.logMessage("Removed file {0}".format(item.text())) 
            self.listTestCollection.takeItem(self.listTestCollection.row(item))
               
        if self.listTestCollection.count() > 0:
            if(newSelIndex < 0):
                newSelIndex = 0
            self.listTestCollection.item(newSelIndex).setSelected(True)
            
        self.updateButtonsToListChange() 
            
    def startExecutingTestCases(self):
        """Execute selected test cases """
        self.logMessage("=== Excecute test cases ===",LogStyle.TITLE)
        self._executeTestCases(self.listTestCollection.selectedItems())
      
    def startExecutingAllTestCases(self):
        """Execute all test cases """
        self.logMessage("=== Excecute all test cases ===",LogStyle.TITLE)
        items = [self.listTestCollection.item(i) for i in range(self.listTestCollection.count())]
        self._executeTestCases(items)
        
    def stopExecuting(self): 
        """Stop executing and send signal to test thread"""      
        self.emit(SIGNAL('executionStopped'))
        self.logMessage("=== Executing stopped ===",LogStyle.TITLE)
        self.updateExecutionButtons(True)
        
    def updateExecutionButtons(self, value):
        """Update buttons for execution """
        if(len(self.listTestCollection.selectedItems()) > 0):
            self.buttonExecute.setEnabled(value)
        self.buttonExecuteAll.setEnabled(value)
        self.buttonStop.setEnabled(not value)
        
    def updateButtonsToListChange(self):
        """Update buttons when number of items in list changed"""
        if self.listTestCollection.count() == 0:
            self.buttonExecuteAll.setEnabled(False)
            self.buttonStop.setEnabled(False)
        else:
            self.buttonExecuteAll.setEnabled(True)
            
    def _executeTestCases(self, items):
        """Execute test cases in test thread """
        self.testThread = TestThread(self,items)
        self.connect(self.testThread,SIGNAL("logMessage"), self.logMessage)
        self.connect(self.testThread,SIGNAL("setItemIcon"), self.setItemIcon)
        self.connect(self.testThread,SIGNAL("updateExecutionButtons"), self.updateExecutionButtons)
        self.testThread.start()              
                
    def logMessage(self, msg, style = None):      
        
        if(style == LogStyle.TITLE):
            self.textBrowserLog.append('<b>{0}</b>'.format(msg))
        elif(style == LogStyle.ERROR):
            self.textBrowserLog.append('<font color="#FF0000">{0}</font><font color="#000000"></font>'.format(msg))
        elif(style == LogStyle.SUCCESS):
            self.textBrowserLog.append('<font color="#009900">{0}</font><font color="#000000"></font>'.format(msg))
        else:
            self.textBrowserLog.append(msg)
        with open("ts.log","a") as file:
            timeStamp =datetime.datetime.now().strftime("[%Y-%m-%d %H:%M:%S] ")
            file.write(timeStamp +msg + "\n")
            
    def setItemIcon(self, item, icon = None):    
        if(icon == ItemIcon.ERROR):
            item.setIcon(self.iconError)
        elif(icon == ItemIcon.FAILED):
            item.setIcon(self.iconFailed)
        elif(icon == ItemIcon.SUCCESS):
            item.setIcon(self.iconSuccess)
        else:
            item.setIcon(self.iconDefault)

        
    def _updateLastDirectory(self, path):
        if(path):
            self._lastDirectory = os.path.dirname(path)
    
    def saveTestCollection(self):       
        if self.testCollectionFile == None:
            outputFile =  QFileDialog.getSaveFileName(self, "Select file", self._lastDirectory, "Test collection file (*.tc)")  
            self._updateLastDirectory(outputFile)      
        else:
            outputFile = self.testCollectionFile
        self._saveTestCollectionToFile(outputFile)      
        
    def _saveTestCollectionToFile(self, outputFile):
        """Save test cases to test collection file"""
        if(outputFile):
            tcContent = ""
            items = [self.listTestCollection.item(i) for i in range(self.listTestCollection.count())]
            for item in items:
                path = item.data(Qt.UserRole)
                tcContent += path + "\n"
            with open(outputFile,'w') as file:
                file.write(tcContent)
            self.testCollectionFile = outputFile
            self.logMessage("Saved test collection file " + os.path.basename(outputFile))
    
    def saveAsTestCollection(self):       
        outputFile = QFileDialog.getSaveFileName(self, "Select file", self._lastDirectory, "Test collection file (*.tc)")
        self._updateLastDirectory(outputFile)
        self._saveTestCollectionToFile(outputFile) 
        
    def _loadTestCollectionFile(self,inputFile):
        fNames = []
        with open(inputFile) as file:
            for line in file:
                fNames.append(line.replace("\n",""))
        self.listTestCollection.clear()
        self._addTestCasesToList(fNames)
        self.testCollectionFile = inputFile
        self.logMessage("Loaded test collection file " + os.path.basename(inputFile))
        self.updateButtonsToListChange()
    
    def openTestCollection(self):
        """Open test collection file and load testcases"""
        inputFile = QFileDialog.getOpenFileName(self, "Select file", self._lastDirectory, "Test collection file (*.tc)")
        self._updateLastDirectory(inputFile)
        if(inputFile):
            self._loadTestCollectionFile(inputFile)
            
               
    def newTestCollection(self):
        self.listTestCollection.clear()
        self.testCollectionFile = None
        self.logMessage("Created new test collection file")
        self.updateButtonsToListChange()
    
               
def startGUI():
    app = QApplication(sys.argv)
    tsw = TestsuiteWindow()
    tsw.show()
    sys.exit(app.exec_())

if __name__ == "__main__":
    startGUI()