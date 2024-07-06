import sys
from analyzer.txtanalyzer import TxtAnalyzer
from analyzer.analyzer import AnalyzerException
from handprinter import HandPrinter

class HandHistoryParser:
    
    def parseHistoryTexts(self, historyTexts, ignoreBetSize, useSimpleNames, excludeNoHeroHH):
        """Parse list of input histories and return tuple list with hand id and parsed history"""
        
        parseResult = []
        
        for historyText in historyTexts:
            analyzer = TxtAnalyzer(historyText)
            analyzer.analyze()
            
            #exclude hand history without hero if option is active
            if((excludeNoHeroHH and analyzer.hero) or not excludeNoHeroHH):
                handprinter = HandPrinter(analyzer,ignoreBetSize, useSimpleNames)
                parsedHistory = handprinter.printHand()
                parseResult.append((analyzer.handId, parsedHistory))
            
        return parseResult

    def parseHandHistory(self, inputFile, outputFile):
        
        try:
            
            with open(inputFile) as file:
                history = file.read()
                analyzer = TxtAnalyzer(history)
                analyzer.analyze()
            
            handprinter = HandPrinter(analyzer,False,False)
            handprinter.printHandToFile(outputFile)    
            
            print("Parsing successful")
            
        except AnalyzerException as e:
            print("Parsing failed")
            print("Error: ",e) 
    
  
if __name__ == "__main__":
    if(len(sys.argv) < 3):
        print("Invalid arguments -> hhp inputfile outputfile")
    
    inputFile = sys.argv[1]
    outputFile = sys.argv[2]
    hhp = HandHistoryParser()
    hhp.parseHandHistory(inputFile, outputFile)  




    
    
