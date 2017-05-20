//******************************************************************************
//
// This file is part of the OpenHoldem project
//    Source code:           https://github.com/OpenHoldem/openholdembot/
//    Forums:                http://www.maxinmontreal.com/forums/index.php
//    Licensed under GPL v3: http://www.gnu.org/licenses/gpl.html
//
//******************************************************************************
//
// Purpose: providing extra debug symbols "debug0" .. "debug9"
//  for OpenHoldems debug-tab to monitor internal values easily
//  without having to need the GUI of Visual Studio and breakpoints
//  or log-files.
//
//******************************************************************************

#ifndef INC_CSYMBOLENGINEDEBUG_H
#define INC_CSYMBOLENGINEDEBUG_H

#include "CVirtualSymbolEngine.h"

const int kNumberOfDebugValues = 10;
const int kLastDebugIndex = kNumberOfDebugValues - 1;

class CSymbolEngineDebug: public CVirtualSymbolEngine {
 public:
	CSymbolEngineDebug();
	~CSymbolEngineDebug();
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
  void SetValue(int index, double value);
	bool EvaluateSymbol(const CString name, double *result, bool log = false);
	CString SymbolsProvided();;
 private:
  double _debug_values[kNumberOfDebugValues];
};

extern CSymbolEngineDebug *p_symbol_engine_debug;

#endif INC_CSYMBOLENGINEDEBUG_H