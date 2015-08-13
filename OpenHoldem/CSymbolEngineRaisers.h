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

#ifndef INC_CSYMBOLENGINERAISERS_H
#define INC_CSYMBOLENGINERAISERS_H

#include "CVirtualSymbolEngine.h"
#include "CBetroundCalculator.h"
#include "NumericalFunctions.h"

class CSymbolEngineRaisers: public CVirtualSymbolEngine {
 public:
	CSymbolEngineRaisers();
	~CSymbolEngineRaisers();
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
	bool EvaluateSymbol(const char *name, double *result, bool log = false);
	CString SymbolsProvided();
 public:
	int raischair()	{ return _raischair; }
	int firstraiser_chair()      { return _firstraiser_chair; }
	int raisbits(int betround) {
		if ((betround >= kBetroundPreflop)
			  && (betround <= kBetroundRiver)) {
			return _raisbits[betround];
		}	else {
			return kUndefined;
		}
	}
 public:
	int foldbits(int betround) {
		if ((betround >= kBetroundPreflop)
			  && (betround <= kBetroundRiver)) {
			return _foldbits[betround];
		}	else {
			return kUndefined;
		}
	}
 public:
	int nplayerscallshort()			{ return _nplayerscallshort; }
	int nopponentstruelyraising()	{ return _nopponentstruelyraising; }
	int nopponentsbetting()			{ return _nopponentsbetting; }
	int nopponentsfolded()			{ return _nopponentsfolded; }
	int nopponentschecking()		{ return _nopponentschecking; }

 private:
	void CalculateRaisers();
	void CalculateFoldBits();
	void CalculateNOpponentsCheckingBettingFolded();
 private:
	int FirstPossibleRaiser();
	int LastPossibleRaiser();
 private:
	double RaisersBet();
	double LastOrbitsLastRaisersBet();
 private:
  int LastRaised(const int round);
 private:
	int _raischair;
	int _firstraiser_chair;
	int _nplayerscallshort;
	int _nopponentsbetting;
	int _nopponentstruelyraising;
	int _nopponentsfolded;
	int _nopponentschecking;
 private:
	// Indices 1..4 are for the betrounds preflop..river.
	// Index 0 is unused.
	int _raisbits[kNumberOfBetrounds + 1];
	int _foldbits[kNumberOfBetrounds + 1];
	int _callbits[kNumberOfBetrounds + 1]; 
  int _lastraised[kNumberOfBetrounds + 1]; 
};

extern CSymbolEngineRaisers *p_symbol_engine_raisers;

#endif INC_CSYMBOLENGINERAISERS_H
