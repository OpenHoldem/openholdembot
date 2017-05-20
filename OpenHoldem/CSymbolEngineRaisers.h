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
//
// nopponentstruelyraising counts all people who voluntarily bet more than needed,
// especially:
//  * all raisers
//  * the first voluntary better postflop
// but not
//  * the infamous "blind-raisers" (Winholdem)
//  * people posting antes 
//
// nopponentstruelyraising counts only the info that is visible at the table,
// i.e. one orbit (max). Formerly it was well-defined only at out turn,
// but we try to make it well-defined all the time, mainly because
// people don't understand the restrictions of "Raises" (OpenPPl,
// implemented with the use of nopponentsraising:
// http://www.maxinmontreal.com/forums/viewtopic.php?f=297&t=18141)
//
// Now nopponentstruelyraising should count:
//  * a full orbit when it is our turn
//  * a partial orbit from dealer to hero if we did not yet act
//  * a partial orbit behind us if we already acted 
//    (similar to RaisesSinceLastPlay, but might include a bettor)
//  * an orbit after the dealer if the userchair is unknown
//    (not really usable for OpenPPL which updates at our turn, 
//    but at least somewhat meaningful in the debug-tab).
//
//******************************************************************************


#ifndef INC_CSYMBOLENGINERAISERS_H
#define INC_CSYMBOLENGINERAISERS_H

#include "CVirtualSymbolEngine.h"
#include "CBetroundCalculator.h"
#include "NumericalFunctions.h"

class CSymbolEngineRaisers: public CVirtualSymbolEngine {
 friend class CSymbolEngineCallers;
 public:
	CSymbolEngineRaisers();
	~CSymbolEngineRaisers();
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
	int raischair()	{ return _raischair; }
	int firstraiser_chair()      { return _firstraiser_chair; }
 public:
  int raisbits(int betround);
	int nopponentstruelyraising()	{ return _nopponentstruelyraising; }
 protected:
  // Also used by CSymbolEngineCallers
  // Return values are NOT normalized to 0..(nchairs-1)
  // because this is more convenient 
  // for the exit-condition of the for-loop.
  // that searches for raisers (callers).
  // Normalization happens then inside the loop
  int ChairInFrontOfFirstPossibleActor();
  int FirstPossibleActor();
  int LastPossibleActor();	
 private:
	void CalculateRaisers();
 private:
	double MinimumStartingBetCurrentOrbit(bool searching_for_raisers);
 private:
  int LastRaised(const int round);
 private:
	int _raischair;
	int _firstraiser_chair;
	int _nopponentstruelyraising;
 private:
  int _temp_raisbits_current_orbit;
	// Indices 1..4 are for the betrounds preflop..river.
	// Index 0 is unused.
	int _raisbits[kNumberOfBetrounds + 1];
  int _lastraised[kNumberOfBetrounds + 1]; 
};

extern CSymbolEngineRaisers *p_symbol_engine_raisers;

#endif INC_CSYMBOLENGINERAISERS_H
