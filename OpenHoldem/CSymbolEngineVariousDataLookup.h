//******************************************************************************
//
// This file is part of the OpenHoldem project
//    Source code:           https://github.com/OpenHoldem/openholdembot/
//    Forums:                http://www.maxinmontreal.com/forums/index.php
//    Licensed under GPL v3: http://www.gnu.org/licenses/gpl.html
//
//******************************************************************************
//
// Purpose: Symbol lookup for various symbols 
//   that are not part of a regular symbol-engine
//
//******************************************************************************

#ifndef INC_CSYMBOLS_H
#define INC_CSYMBOLS_H

#include "CVirtualSymbolEngine.h"

class CSymbolEngineVariousDataLookup: public CVirtualSymbolEngine
{
public:
	// public functions
	CSymbolEngineVariousDataLookup();
	~CSymbolEngineVariousDataLookup();
public:
	// Mandatory reset-functions
	void InitOnStartup();
	void UpdateOnConnection();
	void UpdateOnHandreset();
	void UpdateOnNewRound();
	void UpdateOnMyTurn();
	void UpdateOnHeartbeat();
public:
	// public accessors
	int games_played() { return _games_played; };
	bool EvaluateSymbol(const CString name, double *result, bool log = false);
	CString SymbolsProvided();
private:
	// private functions and variables - not available via accessors or mutators
	int _games_played;
};

#endif /* INC_CSYMBOLS_H */
