//******************************************************************************
//
// This file is part of the OpenHoldem project
//    Source code:           https://github.com/OpenHoldem/openholdembot/
//    Forums:                http://www.maxinmontreal.com/forums/index.php
//    Licensed under GPL v3: http://www.gnu.org/licenses/gpl.html
//
//******************************************************************************
//
// Purpose:
//
//******************************************************************************

#ifndef INC_CSYMBOLENGINEUSERCHAIR_H
#define INC_CSYMBOLENGINEUSERCHAIR_H

#include "CVirtualSymbolEngine.h"

#define USER_CHAIR p_symbol_engine_userchair->userchair()

class CSymbolEngineUserchair: public CVirtualSymbolEngine
{
public:
	CSymbolEngineUserchair();
	~CSymbolEngineUserchair();
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
	int userchair()				{ return _userchair; }
	int userchairbit()			{ return 1 << (_userchair); }
	bool userchair_confirmed()	{ return (_userchair != kUndefined); }
private:
	void CalculateUserChair();
  bool IsNotShowdown();
private:
	int _userchair;
};

extern CSymbolEngineUserchair *p_symbol_engine_userchair;

#endif INC_CSYMBOLENGINEUSERCHAIR_H