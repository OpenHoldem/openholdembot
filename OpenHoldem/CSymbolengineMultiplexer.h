//*******************************************************************************
//
// This file is part of the OpenHoldem project
//   Download page:         http://code.google.com/p/openholdembot/
//   Forums:                http://www.maxinmontreal.com/forums/index.php
//   Licensed under GPL v3: http://www.gnu.org/licenses/gpl.html
//
//*******************************************************************************
//
// Purpose: multiplexer functionality to support postfixes
//   like "dealer" or "last_caller_chair" instead of chair numbers.
//   This should be one of the very first symbol-engines to be called
//   to allow proper replacement of the postfix and a new query 
//   before other symbol engines try to handle this query.
//
//*******************************************************************************

#ifndef INC_CSYMBOLENGINEMULTIPLEXER_H
#define INC_CSYMBOLENGINEMULTIPLEXER_H

#include "CVirtualSymbolEngine.h"

class CSymbolEngineMultiplexer: public CVirtualSymbolEngine {
 public:
	CSymbolEngineMultiplexer();
	~CSymbolEngineMultiplexer();
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
	bool EvaluateSymbol(const CString name, double *result, bool log = false);
	bool EvaluateSymbol(const CString name, CString *result, bool log = false);
	CString SymbolsProvided();
	bool IsValidPostfix() {
		return (_no_postfix || _valid_postfix);
	}
	// Not inline, because it is recursive
	CString MultiplexedSymbolName(CString name);
 private:
  // These functions get called only once
  // and at least FastExitOnLastCharacter(( gets called very often
  // so we make them inline.
  inline bool FastExitOnLastCharacter(int last_character_of_symbol);
private:
	bool _no_postfix;
	bool _valid_postfix;
};

#endif INC_CSYMBOLENGINEMULTIPLEXER_H