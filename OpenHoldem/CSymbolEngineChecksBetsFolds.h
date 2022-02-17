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

#ifndef INC_CSYMBOLENGINECHECKSBETSFOLDS_H
#define INC_CSYMBOLENGINECHECKSBETSFOLDS_H

#include "CVirtualSymbolEngine.h"
#include "CBetroundCalculator.h"


class CSymbolEngineChecksBetsFolds : public CVirtualSymbolEngine {
public:
	CSymbolEngineChecksBetsFolds();
	~CSymbolEngineChecksBetsFolds();
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
	int foldbits(int betround) {
		if ((betround >= kBetroundPreflop)
			&& (betround <= kBetroundRiver)) {
			return _foldbits[betround];
		}
		else {
			return kUndefined;
		}
	}
public:
	int nplayerscallshort() { return _nplayerscallshort; }
	int nopponentsbetting() { return _nopponentsbetting; }
	int nopponentsfolded() { return _nopponentsfolded; }
	int nopponentsfoldedround() { return bitcount(_foldbits[BETROUND]); }
	int nopponentschecking() { return _nopponentschecking; }
private:
	void CalculateFoldBits();
	void CalculateNOpponentsCheckingBettingFolded();
private:
	double RaisersBet();
private:
	int _nplayerscallshort;
	int _nopponentsbetting;
	int _nopponentsfolded;
	int _nopponentschecking;
private:
	// Indices 1..4 are for the betrounds preflop..river.
	// Index 0 is unused.
	int _foldbits[kNumberOfBetrounds + 1];
};

#endif INC_CSYMBOLENGINECHECKSBETSFOLDS_H
