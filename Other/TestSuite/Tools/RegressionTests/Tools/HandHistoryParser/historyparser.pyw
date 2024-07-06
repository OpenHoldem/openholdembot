import sys
import os
import codecs
from ui_historyparser import Ui_HandHistoryParserDlg
from PyQt4.QtCore import *
from PyQt4.QtGui import *
from hhp import HandHistoryParser
from _io import StringIO
from analyzer.analyzer import AnalyzerException
 
class HandHistoryParserDlg(QDialog, Ui_HandHistoryParserDlg):
    def __init__(self, parent=None):
        super(HandHistoryParserDlg, self).__init__(parent)
        self.setupUi(self)
        
        self.connect(self.buttonInputFile,SIGNAL("clicked()"), self.selectInputFile)
        self.connect(self.buttonOutputFile,SIGNAL("clicked()"), self.selectOutputFile)
        self.connect(self.buttonInputFolder,SIGNAL("clicked()"), self.selectInputFolder)
        self.connect(self.buttonOutputFolder,SIGNAL("clicked()"), self.selectOutputFolder)
        self.connect(self.buttonParse,SIGNAL("clicked()"), self.parseHistory)
        
        
    def selectInputFile(self):
        dirName = "."
        if(self.lineEditInputFile.text()):
            dirName = os.path.dirname(self.lineEditInputFile.text())
        
        fname = QFileDialog.getOpenFileName(self, "Select file", dirName, "Text files (*.txt)")
        self.lineEditInputFile.setText(fname)    
                
    def selectOutputFile(self):  
        dirName = "."
        if(self.lineEditInputFile.text()):
            dirName = os.path.dirname(self.lineEditInputFile.text())  
        fname = QFileDialog.getSaveFileName(self, "Select file", dirName, "Text files (*.txt)")
        self.lineEditOutputFile.setText(fname)
        
    def selectInputFolder(self):  
        dirName = "."
        if(self.lineEditInputFolder.text()):
            dirName = self.lineEditInputFolder.text()
        dirname = QFileDialog.getExistingDirectory(self,"Select folder",dirName,QFileDialog.ShowDirsOnly);  
        self.lineEditInputFolder.setText(dirname)

    def selectOutputFolder(self):  
        dirName = "."
        if(self.lineEditOutputFolder.text()):
            dirName = self.lineEditOutputFolder.text()  
        dirname = QFileDialog.getExistingDirectory(self,"Select folder",dirName,QFileDialog.ShowDirsOnly);  
        self.lineEditOutputFolder.setText(dirname)
            
            
    def splitHistoryText(self,historyText):
        """ Remove empty lines and save history texts in list"""
        
        texts = []      
        text = ""
                
        for line in StringIO(historyText):
            if(line != "\n" and line != "\r\n"):
                text += line 
            elif((line == "\n" or line == "\r\n") and text != ""):
                texts.append(text)
                text = ""
                
        if(text):
            texts.append(text)
        return texts
                
    
    def writeHistory(self,parsedTexts):
        
        defaultFolder = "parsed"
        if(not self.lineEditOutputFile.text() and not self.lineEditOutputFolder.text()):
            if not os.path.exists(defaultFolder):
                    os.makedirs(defaultFolder)
        
        # single parsed text
        if(len(parsedTexts) == 1):
            outputfile = self.lineEditOutputFile.text()
            if(not outputfile):
                if(self.lineEditOutputFolder.text()):
                    outputfile =  os.path.join(self.lineEditOutputFolder.text(), parsedTexts[0][0] + ".txt");
                else:
                    outputfile =  os.path.join(defaultFolder, parsedTexts[0][0] + ".txt");
            
            with codecs.open(outputfile,'w','utf-8') as file:
                file.write(parsedTexts[0][1])
        # multiple parsed texts
        elif(len(parsedTexts) > 0):
            outputfolder = self.lineEditOutputFolder.text()
            
            if(not outputfolder):
                outputfolder = defaultFolder
                     
            for text in parsedTexts:
                with codecs.open(os.path.join(outputfolder,text[0]+".txt"),'w','utf-8') as file:
                    file.write(text[1])
                   
        else:
            raise ParserException("No parse result")            
            
    def parseHistory(self):
        
        try:
        
            hhp = HandHistoryParser()
            text = None
            inputText = self.textEditHistory.toPlainText()
            inputFile = self.lineEditInputFile.text()
            inputFolder = self.lineEditInputFolder.text()
            
            if(inputText):
                text = inputText        
            elif(inputFile):
                with codecs.open(inputFile,'r','utf-8') as file:
                    text = file.read()
            elif(inputFolder):
                pass
            else:
                raise ParserException("Specify input")
            
            if(not text and not inputFolder):
                raise ParserException("Input is empty")
            
            QApplication.setOverrideCursor(QCursor(Qt.BusyCursor))
            
            # parse all txt files in folder
            if(inputFolder):
                for filename in os.listdir(inputFolder):
                    if filename.endswith(".txt"):
                        try:
                            with codecs.open(os.path.join(inputFolder,filename),'r','utf-8') as file:
                                text = file.read()
                            historyTexts = self.splitHistoryText(text)
                            parsedTexts = hhp.parseHistoryTexts(historyTexts,self.checkBoxIgnoreBetSize.isChecked(),
                                            self.checkBoxSimpleNames.isChecked(),self.checkBoxExcludeNoHeroHH.isChecked())
                            self.writeHistory(parsedTexts)
                        except AnalyzerException as e:
                            QApplication.restoreOverrideCursor()
                            QMessageBox.critical(self, "Parsing Error", "File : {0}. {1}".format(filename,str(e)))
            #parse text
            else:
                historyTexts = self.splitHistoryText(text)
                parsedTexts = hhp.parseHistoryTexts(historyTexts,self.checkBoxIgnoreBetSize.isChecked(),
                                self.checkBoxSimpleNames.isChecked(),self.checkBoxExcludeNoHeroHH.isChecked())
                self.writeHistory(parsedTexts)
                 
        except (IOError, OSError) as e:
            QApplication.restoreOverrideCursor()
            QMessageBox.warning(self, "File Error", str(e))
        except (ParserException, AnalyzerException) as e:
            QApplication.restoreOverrideCursor()
            QMessageBox.critical(self, "Parsing Error", str(e))
        finally:
            QApplication.restoreOverrideCursor()
                  
        
        
class ParserException(Exception):
    pass        
        
app = QApplication(sys.argv)
dlg = HandHistoryParserDlg()
dlg.show()
sys.exit(app.exec_())



    
 
 
