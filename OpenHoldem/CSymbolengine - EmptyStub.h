//*******************************************************************************
//
// This file is part of the OpenHoldem project
//   Download page:         http://code.google.com/p/openholdembot/
//   Forums:                http://www.maxinmontreal.com/forums/index.php
//   Licensed under GPL v3: http://www.gnu.org/licenses/gpl.html
//
//*******************************************************************************
//
// Purpose: Changing formulas with the load$-command,
//   therefore implemented as a symbol-engine.
//
//*******************************************************************************

#ifndef INC_CSYMBOLENGINEFORMULA_LOADING_H
#define INC_CSYMBOLENGINEFORMULA_LOADING_H

#include "CVirtualSymbolEngine.h"

class CSymbolEngineFormulaLoading: public CVirtualSymbolEngine {
 public:
	CSymbolEngineFormulaLoading();
	~CSymbolEngineFormulaLoading();
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
	bool EvaluateSymbol(const char *name, double *result, bool log = false);
	CString SymbolsProvided();;
 public:
};

extern CSymbolEngineFormulaLoading *p_symbol_engine_formula_loading;

#endif INC_CSYMBOLENGINEFORMULA_LOADING_H