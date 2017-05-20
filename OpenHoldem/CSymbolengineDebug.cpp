//******************************************************************************
//
// This file is part of the OpenHoldem project
//    Source code:           https://github.com/OpenHoldem/openholdembot/
//    Forums:                http://www.maxinmontreal.com/forums/index.php
//    Licensed under GPL v3: http://www.gnu.org/licenses/gpl.html
//
//******************************************************************************
//
// Purpose: providing extra debug symbols "debug0" .. "debug9"
//  for OpenHoldems debug-tab to monitor internal values easily
//  without having to need the GUI of Visual Studio and breakpoints
//  or log-files.
//
//******************************************************************************

#include "stdafx.h"
#include "CSymbolEngineDebug.h"

#include "CBetroundCalculator.h"
#include "CPreferences.h"
#include "CScraper.h"
#include "CStringMatch.h"
#include "CTableState.h"
#include "MagicNumbers.h"

CSymbolEngineDebug *p_symbol_engine_debug = NULL;

CSymbolEngineDebug::CSymbolEngineDebug() {
	// The values of some symbol-engines depend on other engines.
	// As the engines get later called in the order of initialization
	// we assure correct ordering by checking if they are initialized.
  //
	// This engine does not use any other engines.
  for (int i = 0; i < kNumberOfDebugValues; ++i) {
    _debug_values[i] = kUndefined;
  }
}

CSymbolEngineDebug::~CSymbolEngineDebug() {
}

void CSymbolEngineDebug::InitOnStartup() {
}

void CSymbolEngineDebug::UpdateOnConnection() {
}

void CSymbolEngineDebug::UpdateOnHandreset() {
}

void CSymbolEngineDebug::UpdateOnNewRound() {
}

void CSymbolEngineDebug::UpdateOnMyTurn(){
}

void CSymbolEngineDebug::UpdateOnHeartbeat() {
}

void CSymbolEngineDebug::SetValue(int index, double value) {
  AssertRange(index, 0, kLastDebugIndex);
  _debug_values[index] = value;
}

bool CSymbolEngineDebug::EvaluateSymbol(const CString name, double *result, bool log /* = false */) {
  FAST_EXIT_ON_OPENPPL_SYMBOLS(name);
	if (memcmp(name, "debug", 5) != 0) {
		// Symbol of a different symbol-engine
	  return false;
	}
  if (strlen(name) != 6) {
    // invalid format
    return false;
  }
  // debug0..debug9
  int index = RightDigitCharacterToNumber(name);
  AssertRange(index, 0, kLastDebugIndex);
  *result = _debug_values[index];
  return true;
}

CString CSymbolEngineDebug::SymbolsProvided() {
  return RangeOfSymbols("debug%i", 0, kLastDebugIndex);
}

