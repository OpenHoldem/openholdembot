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

#include "pch.h"
#include "CSymbolEngineFormulaLoading.h"

#include "OpenHoldemDoc.h"
#include "CPreferences.h"
#include "MagicNumbers.h"
#include "OH_MessageBox.h"

CSymbolEngineFormulaLoading *p_symbol_engine_formula_loading = NULL;

CSymbolEngineFormulaLoading::CSymbolEngineFormulaLoading() {
	// The values of some symbol-engines depend on other engines.
	// As the engines get later called in the order of initialization
	// we assure correct ordering by checking if they are initialized.
	//
	// This engine does not use any other engines.
  _profile_to_be_loaded = "";
}

CSymbolEngineFormulaLoading::~CSymbolEngineFormulaLoading() {
}

void CSymbolEngineFormulaLoading::InitOnStartup() {
	ResetOnConnection();
}

void CSymbolEngineFormulaLoading::ResetOnConnection() {
  _profile_to_be_loaded = "";
}

void CSymbolEngineFormulaLoading::ResetOnHandreset() {
}

void CSymbolEngineFormulaLoading::ResetOnNewRound() {
}

void CSymbolEngineFormulaLoading::ResetOnMyTurn() {
}

void CSymbolEngineFormulaLoading::ResetOnHeartbeat() {
}

void CSymbolEngineFormulaLoading::ChangeProfileOnLoadCommand() {
  if (_profile_to_be_loaded == "") {
    return;
  }
  COpenHoldemDoc *pDoc = COpenHoldemDoc::GetDocument();
  if (pDoc == NULL) {
    return;
  }
  (COpenHoldemDoc*)pDoc->
  _profile_to_be_loaded = "";
}

void CSymbolEngineFormulaLoading::RememberProfileForLoading(const char* symbol_name) {
  CString symbol = symbol_name;
  assert(symbol.Left(5) == "load$");
  CString filename = (symbol_name + 5);
  if (filename.GetLength() <= 0) {
    OH_MessageBox_Error_Warning("Empty filename in load$-command");
    return;
  }
  // Replace "$" by ".", as dots are not allowed in aa symbol
  filename.Replace("$", ".");
  // ATTENTION!
  // We must not reload a formula in the middle of an evaluation.
  // Therefore we store the filename and load it later
  // at the beginning / end of the heartbeat.
  _profile_to_be_loaded = filename;
}

bool CSymbolEngineFormulaLoading::EvaluateSymbol(const char *name, double *result, bool log /* = false */) {
  FAST_EXIT_ON_OPENPPL_SYMBOLS(name);
	if (memcmp(name, "load$", 5)==0)	{
    RememberProfileForLoading(name);
		*result = kUndefinedZero;
    return true;
	}
	// Symbol of a different symbol-engine
	return false;
}

CString CSymbolEngineFormulaLoading::SymbolsProvided() {
  return "load$ ";
}
