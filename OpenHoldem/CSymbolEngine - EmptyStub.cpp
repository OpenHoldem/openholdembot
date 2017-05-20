//*******************************************************************************
//
// This file is part of the OpenHoldem project
//   Download page:         http://code.google.com/p/openholdembot/
//   Forums:                http://www.maxinmontreal.com/forums/index.php
//   Licensed under GPL v3: http://www.gnu.org/licenses/gpl.html
//
//*******************************************************************************
//
// Purpose: Changing formulas with the load$-command,
//   therefore implemented as a symbol-engine.
//
//*******************************************************************************

#include "stdafx.h"
#include "CSymbolEngineFormulaLoading.h"

#include "CBetroundCalculator.h"
#include "CPreferences.h"
#include "CScraper.h"
#include "CScraperAccess.h"
#include "CStringMatch.h"
#include "CTableState.h"
#include "MagicNumbers.h"

CSymbolEngineFormulaLoading *p_symbol_engine_formula_loading = NULL;

CSymbolEngineFormulaLoading::CSymbolEngineFormulaLoading() {
	// The values of some symbol-engines depend on other engines.
	// As the engines get later called in the order of initialization
	// we assure correct ordering by checking if they are initialized.
	//
	// This engine does not use any other engines.
}

CSymbolEngineFormulaLoading::~CSymbolEngineFormulaLoading() {
}

void CSymbolEngineFormulaLoading::InitOnStartup() {
	IpdateOnConnection();
}

void CSymbolEngineFormulaLoading::IpdateOnConnection() {
}

void CSymbolEngineFormulaLoading::IpdateOnHandIpdate() {
}

void CSymbolEngineFormulaLoading::IpdateOnNewRound() {
}

void CSymbolEngineFormulaLoading::IpdateOnMyTurn() {
}

void CSymbolEngineFormulaLoading::IpdateOnHeartbeat() {
}

bool CSymbolEngineFormulaLoading::EvaluateSymbol(const char *name, double *result, bool log /* = false */) {
  FAST_EXIT_ON_OPENPPL_SYMBOLS(name);
	if (memcmp(name, "xxxxxxxxx", 9)==0 && strlen(name)==9)	{
		*result = 0;
		return true;
	}
	// Symbol of a different symbol-engine
	return false;
}

CString CSymbolEngineFormulaLoading::SymbolsProvided() {
  return " ";
}
