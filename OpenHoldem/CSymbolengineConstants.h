//*******************************************************************************
//
// This file is part of the OpenHoldem project
//   Download page:         http://code.google.com/p/openholdembot/
//   Forums:                http://www.maxinmontreal.com/forums/index.php
//   Licensed under GPL v3: http://www.gnu.org/licenses/gpl.html
//
//*******************************************************************************
//
// Purpose:Constants symbols for more verbosity
//   like ace, ten, preflop, diamonds.
//
//*******************************************************************************

#pragma once

#include "CVirtualSymbolEngine.h"

class CSymbolEngineConstants: public CVirtualSymbolEngine {
 public:
	CSymbolEngineConstants();
	~CSymbolEngineConstants();
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
	CString SymbolsProvided();;
 public:
};

extern CSymbolEngineConstants *p_symbol_engine_constants;

