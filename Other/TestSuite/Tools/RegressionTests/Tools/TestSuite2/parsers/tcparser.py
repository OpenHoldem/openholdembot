import os
import re
import shlex
from configparser import SafeConfigParser
from PyQt4.Qt import QObject

class TestCaseParser(QObject):
    """ Parses testcase from txt config file"""
    
    def __init__(self, tcfile):
        QObject.__init__(self)
        
        self.sblind = None
        self.bblind = None
        self.bbet = None
        self.ante = None
        self.gtype = None
        self.network = None
        self.tournament = None
        self.balances = []
        self.hero = None
        
        self.heroHand = None
        self.flopCards = []
        self.turnCard = None
        self.riverCard = None

        self.players = []
        self.pfActions = []
        self.flopActions = []
        self.turnActions = []
        self.riverActions = []
        
        self.tcfile = tcfile;


    def _parseActions(self, actionLine, preflop):
        """ Parse action line in single actions and check if all actions are valid"""
        
        actions = []
        for action in self._nextActionToken(actionLine):
            # remove parenthesis and whitespaces
            action = action.strip()
            
            # split by whitespace and remove empty whitespace parts
            actionParts = [a for a in shlex.split(action) if a] 
            
            
            for i in range(1,len(actionParts)):
                actionParts[i] = actionParts[i].replace(")","").replace("(","")
            
            if(not self._validAction(actionParts,preflop,'"' in action)):
                raise ParserException("Invalid action: " + action)
            
            actionParts[0] = actionParts[0].replace("\"","") # remove quotes from name
            
            #check if expected hero action is in possible actions
            if len(actionParts) > 3:
                for a in actionParts[4:]:
                    if (a in "FCKRA") and (a not in actionParts[2]):
                        raise ParserException("Expected action is not in possible actions: " + action)
            
            actions.append(actionParts)
        return actions
    
    def _nextActionToken(self, actionLine):
        """ Return next action token seperated by comma """
        
        iterObj = iter(actionLine.split(','))
        # iterate manually over tokens in order to merge splited tokens in parenthesis again
        while iterObj:
            try:
                token = iterObj.__next__()
                if not "(" in token:
                    yield token
                # if left parenthesis found -> add tokens until right parenthesis found
                else:
                    while not ")" in token:
                        token += iterObj.__next__()
                    yield token
                    
            except StopIteration:
                break
                
    
    def _validAction(self, actionParts,preflop,quotedName):
        """Checks if all parts of an action are valid"""
        
        if(len(actionParts) >= 2):
            # name
            if quotedName:
                return len(actionParts[0]) > 0
            else:
                if not re.match(r".*[a-zA-Z]{1}.*", actionParts[0]):
                    return False
            
            # opponent action
            if len(actionParts) == 2:  
                if(preflop):
                    return re.match(r"[FKCRASB]{1}",actionParts[1])
                else:
                    return re.match(r"[FKCRA]{1}",actionParts[1])
            elif len(actionParts) == 3:   
                return re.match(r"[0-9.]+",actionParts[2]) and actionParts[1] == "R"
            # hero action
            elif len(actionParts) >= 4:
                
                if(actionParts[1] == "can" and actionParts[3] == "do" and re.match(r"[FKCRA]+",actionParts[2])):
                    lastAction = None
                    for a in actionParts[4:]:
                        if (not re.match(r"[FKCRA]{1}",a)) and (not ((re.match(r"[0-9.]+",a) and lastAction == "R"))):
                            return False
                        lastAction = a
                    return True
        return False

    def _validCard(self, card):
        """ Checks if the card is a valid poker card """
        return re.match(r"^[2-9TJQKA]{1}[hsdc]{1}$",card)
    
    def _validPlayerBalance(self, playerBalance,quotedName):
        """ Checks if it is a valid player balance """
        if quotedName:
            return re.match(r"^(.+)[\s]+[0-9.]+$",playerBalance)
        else:
            return re.match(r"^(.*[a-zA-Z]{1}.*)[\s]+[0-9.]+$",playerBalance)

    def parse(self):
        """ Start parsing the config file"""
        
        try:     
            config = SafeConfigParser()
            if(not os.path.isfile(self.tcfile)):
                raise ParserException("Can't find file " + self.tcfile)
            
            config.read(self.tcfile)

            self._parsePreflop(config)
            self._parsePostflop(config)
            
            self._parseConfig(config)
                                  
        # map all exceptions to parser exception
        except Exception as e:
            raise ParserException(e)
        
    def _parsePreflop(self,config):
        """Parse the preflop section"""
        
        #preflop section
        self.pfActions = self._parseActions(config.get('preflop', 'actions'),True)
        self.heroHand = [c.strip() for c in config.get('preflop', 'hand').split(',')]
        
        if len(self.heroHand) != 2:
                raise ParserException("Invalid number of hero cards")
        for c in self.heroHand:
            if(not self._validCard(c)):
                raise ParserException("Invalid hero card: " + c)
            
        #find players and hero
        for playerAction in self.pfActions:
            if playerAction[0] not in self.players:
                self.players.append(playerAction[0])
            if len(playerAction) > 3:
                self.hero = playerAction[0]
                
        if not self.hero:
            raise ParserException("Could not find hero")
        if len(self.players) < 2:
            raise ParserException("Could not enough players")
        if "S" not in self.pfActions[0]:
            raise ParserException("First preflop action has to define small blind")
           
        # put hero at pos 0
        for i in range(0, len(self.players) - self.players.index(self.hero)):
            p = self.players.pop()
            self.players.insert(0, p)
        
        
                
        
    def _parsePostflop(self,config):
        """Parse all postflop sections"""
        
        #flop section
        if(config.has_section('flop')):
            self.flopActions = self._parseActions(config.get('flop', 'actions'),False)
            self.flopCards = [c.strip() for c in config.get('flop', 'cards').split(',')]
            
            if len(self.flopCards) != 3:
                raise ParserException("Invalid number of flop cards")
            for c in self.flopCards:
                if(not self._validCard(c)):
                    raise ParserException("Invalid flop card: " + c)
            for playerAction in self.flopActions:
                if(playerAction[0] not in self.players):
                    raise ParserException("Unknown player at the flop: " +playerAction[0])
    
        #turn section
        if(config.has_section('turn')):
            self.turnActions = self._parseActions(config.get('turn', 'actions'),False)
            self.turnCard = config.get('turn', 'card')
            
            if(not self._validCard(self.turnCard)):
                raise ParserException("Invalid turn card: " + self.turnCard)
            for playerAction in self.turnActions:
                if(playerAction[0] not in self.players):
                    raise ParserException("Unknown player at the turn: " +playerAction[0])
    
        #river section
        if(config.has_section('river')):
            self.riverActions = self._parseActions(config.get('river', 'actions'),False)
            self.riverCard = config.get('river', 'card')
            
            if(not self._validCard(self.riverCard)):
                raise ParserException("Invalid river card: " + self.riverCard)
            for playerAction in self.riverActions:
                if(playerAction[0] not in self.players):
                    raise ParserException("Unknown player at the river: " +playerAction[0])
        
    def _parseConfig(self,config):
        """ Parse the table config section"""   
        
        #table config section is optional
        if(config.has_section('table')):
            if(config.has_option('table','sblind')):
                self.sblind = config.getfloat('table', 'sblind')
            if(config.has_option('table','bblind')):
                self.bblind = config.getfloat('table', 'bblind')
            if(config.has_option('table','bbet')):
                self.bbet = float(config.getfloat('table', 'bbet'))
            if(config.has_option('table','ante')):
                self.ante = float(config.getfloat('table', 'ante'))
            if(config.has_option('table','gtype')):
                self.gtype = config.get('table', 'gtype')
                if not self.gtype in ('NL', 'PL', 'FL'):
                    raise ParserException("Unknown game type: " +self.gtype)
                                
            if(config.has_option('table','network')):
                self.network = config.get('table', 'network')
            if(config.has_option('table','tournament')):
                self.tournament = config.getboolean('table', 'tournament')
            if(config.has_option('table','balances')):
                balances = config.get('table', 'balances').split(",")
                
                #parse player balances
                for balance in balances: 
                    balance = balance.strip() 
                    if(not self._validPlayerBalance(balance, '"' in balance)):
                        raise ParserException("Invalid player balance: " + balance) 
                    playerBalance = shlex.split(balance)
                    if(playerBalance[0] not in self.players):
                        raise ParserException("Player isn't playing: " +playerBalance[0] ) 
                    self.balances.append(playerBalance)
            
            
class ParserException(Exception):
    pass
         

