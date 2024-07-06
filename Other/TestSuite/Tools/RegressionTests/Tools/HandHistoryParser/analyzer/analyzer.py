from io import StringIO

class Analyzer:
    '''
    Analyzer base class
    '''

    def __init__(self,history):
        self.history = StringIO(history)
        self.handId = None
        self.pokerNetwork = None
        self.bb = None
        self.sb = None
        self.hero = None
        self.playerBalances = []
        self.gameType = None
        self.heroCards = []
        self.flopCards = []
        self.turnCard = None
        self.riverCard = None
        self.pfActions = []
        self.flopActions = []
        self.turnActions = []
        self.riverActions = []
        self.players = []
        
class AnalyzerException(Exception):
    
    def __init__(self,msg,handId=""):
        self.msg = msg
        self.handId = handId
        
    def __str__(self):
        if(self.handId):
            return self.msg + " -> handId: "+self.handId;
        return self.msg;
    
    def __repr__(self):
        if(self.handId):
            return self.msg + " -> handId: "+self.handId;
        return self.msg;

        
