//******************************************************************************
//
// This file is part of the OpenHoldem project
//    Source code:           https://github.com/OpenHoldem/openholdembot/
//    Forums:                http://www.maxinmontreal.com/forums/index.php
//    Licensed under GPL v3: http://www.gnu.org/licenses/gpl.html
//
//******************************************************************************
//
// Purpose: Detecting if we play rush / zoom / ...
//   depending on the time since our last action on handreset.
//
//******************************************************************************

#ifndef INC_CSYMBOLENGINEISRUSH_H
#define INC_CSYMBOLENGINEISRUSH_H

#include "CVirtualSymbolEngine.h"

class CSymbolEngineIsRush: public CVirtualSymbolEngine {
 public:
	CSymbolEngineIsRush();
	~CSymbolEngineIsRush();
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
  bool isrush();
 private:
  double sum_of_handreset_durations;
  int handresets;
};

extern CSymbolEngineIsRush *p_symbol_engine_isrush;

#endif INC_CSYMBOLENGINEUSERCHAIR_H