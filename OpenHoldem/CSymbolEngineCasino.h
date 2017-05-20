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

#ifndef INC_CSYMBOLENGINECASINO_H
#define INC_CSYMBOLENGINECASINO_H

#include "CVirtualSymbolEngine.h"

class CSymbolEngineCasino: public CVirtualSymbolEngine {
 public:
	// public functions
	CSymbolEngineCasino();
	~CSymbolEngineCasino();
 public:
	// Mandatory reset-functions
	void InitOnStartup();
	void UpdateOnConnection();
	void UpdateOnHandreset();
	void UpdateOnNewRound();
	void UpdateOnMyTurn();
	void UpdateOnHeartbeat();
 public:
	bool EvaluateSymbol(const CString name, double *result, bool log = false);
	CString SymbolsProvided();
 public:
  bool ConnectedToManualMode();
  bool ConnectedToOHReplay();
  bool ConnectedToOfflineSimulation();
  bool ConnectedToRealCasino();
  bool ConnectedToBring();
  bool ConnectedToDDPoker();
 private:
  bool SitenameContainsCasinoIdentifier(const char *casino);
};

extern CSymbolEngineCasino *p_symbol_engine_casino;

#endif /* INC_CSYMBOLENGINECASINO_H */
