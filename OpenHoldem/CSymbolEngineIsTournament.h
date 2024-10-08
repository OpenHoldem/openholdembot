//******************************************************************************
//
// This file is part of the OpenHoldem project
//    Source code:           https://github.com/OpenHoldem/openholdembot/
//    Forums:                http://www.maxinmontreal.com/forums/index.php
//    Licensed under GPL v3: http://www.gnu.org/licenses/gpl.html
//
//******************************************************************************
//
// Purpose: Detecting if we play a tournament, especially
//   to enable / disable automatic blind-locking (stability) 
//
//******************************************************************************

#ifndef INC_CSYMBOLENGINEISTOURNAMENT_H
#define INC_CSYMBOLENGINEISTOURNAMENT_H

#include "CVirtualSymbolEngine.h"

class CSymbolEngineIsTournament: public CVirtualSymbolEngine {
 public:
	CSymbolEngineIsTournament();
	~CSymbolEngineIsTournament();
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
	bool istournament()		{ return _istournament == int(true); }
 private:
	void TryToDetectTournament();
	bool BetsAndBalancesAreTournamentLike();
	bool AntesPresent();
	bool CSymbolEngineIsTournament::TitleStringContainsIdentifier(
    const char *identifiers[], int number_of_identifiers);
 public:
  bool IsMTT();
  bool IsSNG();
  bool IsDON();
  bool IsTRIPLEUP();
  bool IsSHOOTOUT();
  bool IsFREEROLL();
  bool IsKNOCKOUT();
  bool IsREBUY();
  bool IsSATELITTE();
  bool IsSPIN();
  bool IsTURBO();
  bool IsSEMITURBO();
  bool IsSUPERTURBO();
  bool IsHYPERTURBO();
  bool IsULTRATURBO();
 private:
	// Internally we have an extra state, kUndefined (-1).  But that's
	// not exposed in the API as it's used internally to avoid locking
	// the istournament decision too quickly.
  // The other valous are true (1) and false (0).
	int _istournament;
	bool _decision_locked;
};

#endif INC_CSYMBOLENGINEISTOURNAMENT_H
