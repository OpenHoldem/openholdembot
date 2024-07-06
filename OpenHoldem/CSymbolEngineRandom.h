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

#ifndef INC_CSYMBOLENGINERANDOM_H
#define INC_CSYMBOLENGINERANDOM_H

#include "CVirtualSymbolEngine.h"
#include "CTransform/CTransform.h"

class CSymbolEngineRandom: public CVirtualSymbolEngine {
 public:
	CSymbolEngineRandom();
	~CSymbolEngineRandom();
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
	// "random" gets calculated each time new
	double random()				{ return Random_0_1(); }
	double randomhand()			{ return _randomhand; }
	double randomheartbeat()	{ return _randomheartbeat; }
	double randomround()		{ return _randomround; }
 private:
	double Random_0_1();
 private:
	double _randomhand;
	double _randomheartbeat;
	double _randomround;
};

#endif INC_CSYMBOLENGINERANDOM_H