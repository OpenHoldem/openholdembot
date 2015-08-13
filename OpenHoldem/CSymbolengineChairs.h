//*******************************************************************************
//
// This file is part of the OpenHoldem project
//   Download page:         http://code.google.com/p/openholdembot/
//   Forums:                http://www.maxinmontreal.com/forums/index.php
//   Licensed under GPL v3: http://www.gnu.org/licenses/gpl.html
//
//*******************************************************************************
//
// Purpose: Various chair symbols, ecept dealerchair, userchair and raischair,
//   which are of special importance and get handled by other symbol-engines.
//
//*******************************************************************************

#ifndef INC_CSYMBOLENGINECHAIRS_H
#define INC_CSYMBOLENGINECHAIRS_H

#include "CVirtualSymbolEngine.h"

class CSymbolEngineChairs: public CVirtualSymbolEngine {
 public:
	CSymbolEngineChairs();
	~CSymbolEngineChairs();
 public:
	// Mandatory reset-functions
	void InitOnStartup();
	void ResetOnConnection();
	void ResetOnHandreset();
	void ResetOnNewRound();
	void ResetOnMyTurn();
	void ResetOnHeartbeat();
 public:
	// Public accessors
	bool EvaluateSymbol(const char *name, double *result, bool log = false);
	CString SymbolsProvided();
 public:
  int opponent_headsup_chair() { return _opponent_headsup_chair; }
  int smallblind_chair()       { return _smallblind_chair; }
  int bigblind_chair()         { return _bigblind_chair; }
  int cutoff_chair()           { return _cutoff_chair; }
 private:
  void CalculateOpponentHeadsupChair();
  void CalculateSmallBlindChair();
  void CalculateBigBlindChair();
  void CalculateCutOffChair();
 private:
  int GetChairByDealposition(int dealposition);
 private:
  int _opponent_headsup_chair;
  int _smallblind_chair;
  int _bigblind_chair;
  int _cutoff_chair;
 private:
  int _nchairs;
};

extern CSymbolEngineChairs *p_symbol_engine_chairs;

#endif INC_CSYMBOLENGINECHAIRS_H

  
