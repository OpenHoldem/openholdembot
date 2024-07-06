import time
import re
import xmlrpc.client
from PyQt4.Qt import QObject, SIGNAL
from testsuite_utility import LogStyle

class AutoPlayer(QObject):
    """ Connects to manual mode, configures the table and peforms actions."""

    def __init__(self, executionDelay):
        QObject.__init__(self)
        self._connect()
        self.executionDelay = executionDelay
        
    def _connect(self):
        self.mm = xmlrpc.client.ServerProxy('http://localhost:9092') 

    def startTest(self,tc,handNumber):
        
        self.testFailed = False 
        self.tc = tc
        self._initTable(handNumber)
        self._performActions()
        
        if self.testFailed:
            return False
        else: 
            return True
        
    def stop(self):
        self.aborted = True
        self.testFailed = True
        
        
    def _performActions(self):
        """ Perform preflop and postflop actions """
    
        tc = self.tc
        #TODO: Refactor check if execution was aborted   
        if self.aborted:
                return;
        self.emit(SIGNAL('logMessage'), "--- Preflop --- Cards : {0}, {1}".format(tc.heroHand[0], tc.heroHand[1]))
        for action in tc.pfActions:
            if self.aborted:
                    return;
            self._doAction(action)
        
        if self.aborted:
                    return;
                           
        if tc.flopCards:
            self.emit(SIGNAL('logMessage'), "--- Flop --- Cards : {0}, {1}, {2}".format(tc.flopCards[0],tc.flopCards[1],tc.flopCards[2])) 
            self.mm.SetFlopCards(tc.flopCards[0], tc.flopCards[1], tc.flopCards[2])
            for action in tc.flopActions:
                if self.aborted:
                    return;
                self._doAction(action)
        
        if self.aborted:
                    return;
                        
        if tc.turnCard:
            self.emit(SIGNAL('logMessage'), "--- Turn --- Card : {0}".format(tc.turnCard)) 
            self.mm.SetTurnCard(tc.turnCard)
            for action in tc.turnActions:
                if self.aborted:
                    return;
                self._doAction(action)
        
        if self.aborted:
                    return; 
           
        if tc.riverCard:
            self.emit(SIGNAL('logMessage'), "--- River --- Card : {0}".format(tc.riverCard)) 
            self.mm.SetRiverCard(tc.riverCard)
            for action in tc.riverActions:
                if self.aborted:
                    return;
                self._doAction(action)
    
    def _initTable(self, handNumber):
        """ Initialize the table with all start values"""
        
        tc = self.tc
        self.aborted = False
        self._resetTable()
        self._configureTable()
        self._addPlayers(tc.players)
        
        #set hand number
        self.mm.SetHandNumber(handNumber)
        
        # set hero
        self.mm.SetCards(tc.players.index(tc.hero), tc.heroHand[0], tc.heroHand[1])
        
        # sb is always player with first pf action
        sb = tc.pfActions[0][0]

        if len(tc.players) > 2:
            # dealer is sitting before SB
            dealer = tc.players[tc.players.index(sb) - 1]
        else:
            # dealer is SB
            dealer = sb
            
        self.mm.SetDealer(tc.players.index(dealer))
        
    def _resetTable(self):
        """Reset all table values"""
        
        self.mm.CancelGetAction()
        
        for c in range(0, 10):
            self.mm.SetActive(c, False)
            self.mm.SetSeated(c, False)
            self.mm.SetCards(c, 'NN', 'NN')
            self.mm.SetBalance(c, 1000.0)
            self.mm.SetBet(c, 0.0)
            self.mm.SetPot(0.0)
            self.mm.SetFlopCards('NN', 'NN', 'NN')
            self.mm.SetTurnCard('NN')
            self.mm.SetRiverCard('NN')
            self.mm.SetTournament(False)
            self.mm.SetGType('NL')
        for b in 'FCKRA':
            self.mm.SetButton(b, False)
        self.mm.Refresh()
            
    def _configureTable(self):
        """Configure table for this testcase """
        tc = self.tc

        if tc.sblind:
            self.mm.SetSBlind(tc.sblind)
        if tc.bblind:
            self.mm.SetBBlind(tc.bblind)
        if tc.bbet:
            self.mm.SetBBet(tc.bbet)
        if tc.ante:
            self.mm.SetAnte(tc.ante)
        if tc.gtype:
            self.mm.SetGType(tc.gtype)
        if tc.network:
            self.mm.SetNetwork(tc.network)
        if tc.tournament:
            self.mm.SetTournament(tc.tournament)
        if tc.balances:
            for player, balance in tc.balances:
                self.mm.SetBalance(tc.players.index(player), float(balance))
        
    def _doAction(self, action):
        """Do single action on the table. """
        
        tc = self.tc
        
        # Build action string for logging
        actionString = ""
        for a in action:
            actionString += a + " "
        
        self.emit(SIGNAL('logMessage'), "Action: " + actionString)
        # delay execution
        time.sleep(self.executionDelay)
        
        if len(action) == 2:
            if action[1] == 'S':
                self.mm.PostSB(tc.players.index(action[0]))
            elif action[1] == 'B':
                self.mm.PostBB(tc.players.index(action[0]))
            elif action[1] == 'C':
                self.mm.DoCall(tc.players.index(action[0]))
            elif action[1] == 'R':             
                self.mm.DoRaise(tc.players.index(action[0]))
            elif action[1] == 'F':
                self.mm.DoFold(tc.players.index(action[0]))
            elif action[1] == 'K':
                pass
            elif action[1] == 'A':
                self.mm.DoAllin(tc.players.index(action[0]))
            else:
                raise ValueError("Unknown action: " + action[1])
        elif len(action) == 3:
            if action[1] == 'R':               
                self.mm.DoRaise(tc.players.index(action[0]),float(action[2]))
            else:
                raise ValueError("Unknown action: " + action[1])
        else:
            # it's heroes turn
            self._handleHeroAction(action)
            
        self.mm.Refresh()
        
    def _handleHeroAction(self,action):
        """ Set buttons for hero action and evaluate the performed action """

        self._showHeroButtons(action[2])

        try:
            # get performed action from manual mode
            result = self.mm.GetAction()
        except: # TODO: Problem with stopping execution when waiting for action
            self.aborted = True
            return
        button = result['button']
        betsize = result['betsize']
        self._resetButtons()
        self._performHeroAction(button,betsize)
        self._compareActionWithTestCase(button, betsize, action)
            
    def _showHeroButtons(self, buttons):
        for b in buttons:
            self.mm.SetButton(b, True)
    
    def _performHeroAction(self, button, betsize):
        """ Perform hero action in manual mode"""  
        
        tc = self.tc   

        if button == 'F':
            self.mm.DoFold(tc.players.index(tc.hero))
            # Abort testcase after fold
            self.aborted = True
        elif button == 'C':
            self.mm.DoCall(tc.players.index(tc.hero))
        elif button == 'K':
            pass
        elif button == 'R': # raise/swag
            if betsize:
                self.mm.DoRaise(tc.players.index(tc.hero),float(betsize))
            else:
                self.mm.DoRaise(tc.players.index(tc.hero))
        elif button == 'A': # allin
            self.mm.DoAllin(tc.players.index(tc.hero))
            self.aborted = True # Abort testcase after all-in
                
        self.mm.Refresh()
        
    def _compareActionWithTestCase(self, button, betsize, heroAction):
        
        expectedActions = heroAction[4:] # all possible hero actions
        eaString = ""

        # is number, support .02 and 2. too
        betNumberReg = re.compile(r"^[ \t]*(?:[0-9]+\.?[0-9]*|[0-9]*\.?[0-9]+)[ \t]*$")
            
        # build expected action string without numbers
        for i,ea in enumerate(expectedActions):  
            if not betNumberReg.match(ea): # is not number
                if(i > 0): 
                    eaString += ", "
                eaString += ea
        if len(eaString) > 1:
            eaString = "(" + eaString + ")"       
            
        # find bet size
        expectedBetsize = "Any" #default bet size is any
        for a in expectedActions:
            if betNumberReg.match(a): # is number
              expectedBetsize = a

        #correct button pressed
        if button in expectedActions:
            
            self.emit(SIGNAL('logMessage'), "Expected {0}, got {1}.".format(eaString, button), LogStyle.SUCCESS)
            
            #consider no betsize if we ask for it an error for now
            #if button == "R" and betsize and self.tc.gtype == "NL":
            if button == "R" and self.tc.gtype == "NL":    
                #correct bet size    
                if expectedBetsize == "Any" or (not betsize is None and betNumberReg.match(betsize) and float(betsize) == float(expectedBetsize)):
                    self.emit(SIGNAL('logMessage'), "Expected R {0}, got R {1}.".format(expectedBetsize, betsize), LogStyle.SUCCESS)
                #wrong bet size
                else:
                    self.emit(SIGNAL('logMessage'), "Expected R {0}, got R {1}.".format(expectedBetsize, betsize), LogStyle.ERROR)
                    self.testFailed = True      
        #wrong button pressed 
        else:
            self.emit(SIGNAL('logMessage'), "Expected {0}, got {1}.".format(eaString, button), LogStyle.ERROR)
            self.testFailed = True
                
    def _addPlayers(self,players):
        """Add players from testcase to the table."""
        for c,p in enumerate(players):
            self.mm.SetActive(c, True)
            self.mm.SetSeated(c, True)
            self.mm.SetCards(c, 'BB', 'BB')
            self.mm.SetName(c, p)
        self.mm.Refresh()
        
    def _resetButtons(self):
        for b in 'FCKRA':
            self.mm.SetButton(b, False)

        
