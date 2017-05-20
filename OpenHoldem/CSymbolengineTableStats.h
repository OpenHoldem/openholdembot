//***************************************************************************** 
//
// This file is part of the OpenHoldem project
//    Source code:           https://github.com/OpenHoldem/openholdembot/
//    Forums:                http://www.maxinmontreal.com/forums/index.php
//    Licensed under GPL v3: http://www.gnu.org/licenses/gpl.html
//
//***************************************************************************** 
//
// Purpose:
//
//***************************************************************************** 

#ifndef INC_CSYMBOLENGINE_TABLESTATS_H
#define INC_CSYMBOLENGINE_TABLESTATS_H

#include "CVirtualSymbolEngine.h"

const int kNumberOfHands = 15;
// We need 1 additional entry at the end for the current hand
const int kIndexCurrentHand = kNumberOfHands;

class CSymbolEngineTableStats: public CVirtualSymbolEngine {
 public:
	CSymbolEngineTableStats();
	~CSymbolEngineTableStats();
 public:
	// Mandatory reset-functions
	void InitOnStartup();
	void UpdateOnConnection();
	void UpdateOnHandreset();
	void UpdateOnNewRound();
	void UpdateOnMyTurn();
	void UpdateOnHeartbeat();
 public:
	// Public accessors
	bool EvaluateSymbol(const CString name, double *result, bool log = false);
	CString SymbolsProvided();
 public:
 private:
  double FlopTurnRiverPct(int betround);
  double AvgBetsPf();
	double TablePfr();
 private:
  void ShiftOldestDataAway();
  void UpdateData();
 private:
  double SumUp(int *data_set_for_n_hands);
  int NumberOfHandsStored();
 private:
  // We need 1 additional entry at the end for the current hand
  int seen_flop[kNumberOfHands + 1];
  int seen_turn[kNumberOfHands + 1];
  int seen_river[kNumberOfHands + 1];
  int bets_preflop[kNumberOfHands + 1];
  int raised_preflop[kNumberOfHands + 1];
  int dealt_players[kNumberOfHands + 1];
};

extern CSymbolEngineTableStats *p_symbol_engine_table_stats;

#endif INC_CSYMBOLENGINE_TABLESTATS_H