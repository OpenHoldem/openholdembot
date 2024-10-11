#pragma once
//*******************************************************************************
//
// This file is part of the OpenHoldem project
//   Download page:         http://code.google.com/p/openholdembot/
//   Forums:                http://www.maxinmontreal.com/forums/index.php
//   Licensed under GPL v3: http://www.gnu.org/licenses/gpl.html
//
//*******************************************************************************
//
// Purpose: 
//
//*******************************************************************************

#include "CVirtualSymbolEngine.h"

class CSymbolEngineUserDLL : public CVirtualSymbolEngine {
 public:
	CSymbolEngineUserDLL();
	~CSymbolEngineUserDLL();
 public:
	// Mandatory reset-functions
	void InitOnStartup();
	void UpdateOnConnection();
	void UpdateOnHandreset();
	void UpdateOnNewRound();
	void UpdateOnMyTurn();
	void UpdateOnHeartbeat();
	int sum_betround_action();
 public:
	// Public accessors
	bool EvaluateSymbol(const CString name, double *result, bool log = false);
	CString SymbolsProvided();;
private:
	double previous_result = 0.0;
	int previous_sum_betround_action = -1;
};