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

#include "pch.h"
#include "CSymbolEngineUserDLL.h"

CSymbolEngineUserDLL *p_symbol_engine_formula_loading = NULL;

CSymbolEngineUserDLL::CSymbolEngineUserDLL() {
	// The values of some symbol-engines depend on other engines.
	// As the engines get later called in the order of initialization
	// we assure correct ordering by checking if they are initialized.
	//
	// This engine does not use any other engines.
}

CSymbolEngineUserDLL::~CSymbolEngineUserDLL() {
}

void CSymbolEngineUserDLL::InitOnStartup() {
	UpdateOnConnection();
}

void CSymbolEngineUserDLL::UpdateOnConnection() {
}

void CSymbolEngineUserDLL::UpdateOnHandreset() {
}

void CSymbolEngineUserDLL::UpdateOnNewRound() {
}

void CSymbolEngineUserDLL::UpdateOnMyTurn() {
}

void CSymbolEngineUserDLL::UpdateOnHeartbeat() {
}

bool CSymbolEngineUserDLL::EvaluateSymbol(const CString name, double *result, bool log /* = false */) {
  FAST_EXIT_ON_OPENPPL_SYMBOLS(name);
	if (memcmp(name, "xxxxxxxxx", 9)==0 && strlen(name)==9)	{
		*result = 0;
		return true;
	}
	// Symbol of a different symbol-engine
	return false;
}

CString CSymbolEngineUserDLL::SymbolsProvided() {
  return " ";
}
