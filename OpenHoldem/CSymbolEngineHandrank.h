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

#ifndef INC_CSYMBOLENGINEHANDRANK_H
#define INC_CSYMBOLENGINEHANDRANK_H

#include "CVirtualSymbolEngine.h"

class CSymbolEngineHandrank: public CVirtualSymbolEngine
{
public:
	CSymbolEngineHandrank();
	~CSymbolEngineHandrank();
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
	double handrank169()      { return _handrank169; }
	double handrank2652()     { return _handrank2652; }
	double handrank1326()     { return _handrank1326; }
	double handrank1000()     { return _handrank1000; }
	double handrankp()        { return _handrankp; }
private:
  void CalculateHandrank();
	void GetCardstring(char *c, unsigned int c0, unsigned int c1);
private:
	double _handrank169;
	double _handrank2652;
	double _handrank1326;
	double _handrank1000;
	double _handrankp;
};

extern CSymbolEngineHandrank *p_symbol_engine_handrank;

#endif INC_CSYMBOLENGINEHANDRANK_H