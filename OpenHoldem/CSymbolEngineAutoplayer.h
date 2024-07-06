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

#ifndef INC_CSYMBOLENGINEAUTOPLAYER_H
#define INC_CSYMBOLENGINEAUTOPLAYER_H

#include "CVirtualSymbolEngine.h"
#include "CSymbolEngineTableLimits.h"

class CSymbolEngineAutoplayer: public CVirtualSymbolEngine {
 public:
	CSymbolEngineAutoplayer();
	~CSymbolEngineAutoplayer();
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
	int  myturnbits()    { return _myturnbits; }
	bool ismyturn()      { return ((_myturnbits & k_my_turn_bits_fold_call_raise) != 0); }
	bool issittingin()   { return _issittingin; }
	bool issittingout()  { return !issittingin(); }
	bool isautopost()		 { return _isautopost; }
	bool isfinalanswer() { return _isfinalanswer; }
  bool isfinaltable();
 public:
	// Especially needed to start the PrWin-calculations
	bool IsFirstHeartbeatOfMyTurn();
	// Visible buttons
	CString GetFCKRAString();
 private:
	void CalculateMyTurnBits();
	void CalculateSitInState();
	void CalculateFinalAnswer();
 private:
	int  _myturnbits;
	bool _issittingin;
	bool _isautopost;
	bool _isfinalanswer; 
 private:
	bool _last_myturnbits;
};

#endif INC_CSYMBOLENGINEAUTOPLAYER_H