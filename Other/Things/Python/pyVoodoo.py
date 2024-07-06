import OpenHoldem

def getDecision():
    print('Python function getDecision() called ', file=open('pyVoodoo.log', 'a'))
    c = OpenHoldem.getSymbol("balance")
    return c
