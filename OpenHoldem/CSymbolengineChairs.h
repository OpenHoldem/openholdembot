//******************************************************************************
//
// This file is part of the OpenHoldem project
//    Source code:           https://github.com/OpenHoldem/openholdembot/
//    Forums:                http://www.maxinmontreal.com/forums/index.php
//    Licensed under GPL v3: http://www.gnu.org/licenses/gpl.html
//
//******************************************************************************
//
// Purpose: Various chair symbols, ecept dealerchair, userchair and raischair,
//   which are of special importance and get handled by other symbol-engines.
//
//******************************************************************************

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
	void UpdateOnConnection();
	void UpdateOnHandreset();
	void UpdateOnNewRound();
	void UpdateOnMyTurn();
	void UpdateOnHeartbeat();
 public:
	// Public accessors
	bool EvaluateSymbol(const CString name, double *result, bool log = false);
	CString SymbolsProvided();
 private:
   int HeadsupChair();
  int SmallBlindChair();
  int BigBlindChair();
  int UTGChair();
 private:
  bool MissingSmallBlind();
  int CalculateMissingSmallBlind();
  bool PlayersBehindDealPosition2ChairDidAct();
 private:
  int ChairByDealposition(int dealposition);
  int ChairByLogicalPosition(int offset_from_dealer);
 private:
  int _nchairs;
  bool _missing_smallblind;
  bool _missing_smallblind_calculated;
};

extern CSymbolEngineChairs *p_symbol_engine_chairs;

#endif INC_CSYMBOLENGINECHAIRS_H
