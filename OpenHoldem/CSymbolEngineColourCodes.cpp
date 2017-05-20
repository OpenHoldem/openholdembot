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

#include "stdafx.h"
#include "CSymbolEngineColourCodes.h"

#include "CBetroundCalculator.h"
#include "CPreferences.h"
#include "CScraper.h"
#include "CStringMatch.h"
#include "CTableState.h"
#include "MagicNumbers.h"

CSymbolEngineColourCodes *p_symbol_engine_colourcodes = NULL;

CSymbolEngineColourCodes::CSymbolEngineColourCodes() {
	// The values of some symbol-engines depend on other engines.
	// As the engines get later called in the order of initialization
	// we assure correct ordering by checking if they are initialized.
  //
	// This engine does not use any other engines.
}

CSymbolEngineColourCodes::~CSymbolEngineColourCodes() {
}

void CSymbolEngineColourCodes::InitOnStartup() {
}

void CSymbolEngineColourCodes::UpdateOnConnection() {
}

void CSymbolEngineColourCodes::UpdateOnHandreset() {
}

void CSymbolEngineColourCodes::UpdateOnNewRound() {
}

void CSymbolEngineColourCodes::UpdateOnMyTurn(){
}

void CSymbolEngineColourCodes::UpdateOnHeartbeat() {
}

bool CSymbolEngineColourCodes::EvaluateSymbol(const CString name, double *result, bool log /* = false */) {
  FAST_EXIT_ON_OPENPPL_SYMBOLS(name);
	if (memcmp(name, "colour", 6) != 0) {
		// Symbol of a different symbol-engine
	  return false;
	}
  if ((memcmp(name, "colourcode", 10) == 0) && (strlen(name) == 11)) {
    // colourcode0..colourcode9
    int index = RightDigitCharacterToNumber(name);
    *result = p_table_state->Player(index)->colourcode();
    return true;
  }
	// Symbol of a different symbol-engine
	return false;
}

CString CSymbolEngineColourCodes::SymbolsProvided() {
  return RangeOfSymbols("colourcode%i", kFirstChair, kLastChair);
}

int CSymbolEngineColourCodes::ColourCodeToDisplay(const int chair) {
  assert(chair >= 0);
  assert(chair <= kLastChair);
  // Scraped colour-code 
  // Coloour-code for PT-Icon removed in rev. 4283,
  // as PT4 does no longer support the auto-rate-icon
  return p_table_state->Player(chair)->colourcode();
}