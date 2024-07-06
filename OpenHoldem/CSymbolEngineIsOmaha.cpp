//******************************************************************************
//
// This file is part of the OpenHoldem project
//    Source code:           https://github.com/OpenHoldem/openholdembot/
//    Forums:                http://www.maxinmontreal.com/forums/index.php
//    Licensed under GPL v3: http://www.gnu.org/licenses/gpl.html
//
//******************************************************************************
//
// Purpose: Detecting if we play a holdem or omaha,
//   needed e.g. for automatic adaption of PT-queries. 
//
//******************************************************************************

#include "stdafx.h"
#include "CSymbolEngineIsOmaha.h"

#include "CEngineContainer.h"

#include "CScraper.h"
#include "CTablemap/CTablemap.h"
#include "CTableState.h"

#include "StringFunctions/string_functions.h"

// The number of cards per player depends on the game-type.
// This affects cards to be scraped and evaluated.
// The data containers must be large enough to store kMaxNumberOfCardsPerPlayer.
int NumberOfCardsPerPlayer() {
  if (p_engine_container->symbol_engine_isomaha() == NULL) {
    // Not yet initialized. Keep the OpenHoldem default
    return kNumberOfCardsPerPlayerHoldEm;
  }
  if (p_engine_container->symbol_engine_isomaha()->isomaha()) {
    return kNumberOfCardsPerPlayerOmaha;
  }
  return kNumberOfCardsPerPlayerHoldEm;
}

CSymbolEngineIsOmaha::CSymbolEngineIsOmaha() {
	// The values of some symbol-engines depend on other engines.
	// As the engines get later called in the order of initialization
	// we assure correct ordering by checking if they are initialized.
  _isomaha = false;
}

CSymbolEngineIsOmaha::~CSymbolEngineIsOmaha()
{}

void CSymbolEngineIsOmaha::InitOnStartup() {
	UpdateOnConnection();
}

void CSymbolEngineIsOmaha::UpdateOnConnection() {
  _isomaha = false;
}

void CSymbolEngineIsOmaha::UpdateOnHandreset()
{}

void CSymbolEngineIsOmaha::UpdateOnNewRound()
{}

void CSymbolEngineIsOmaha::UpdateOnMyTurn() {
}

void CSymbolEngineIsOmaha::UpdateOnHeartbeat() {
  if (_isomaha) {
    write_log(Preferences()->debug_symbolengine(), "[CSymbolEngineIsOmaha] Already Omaha\n");
    return;
  }
  if (!p_tablemap->SupportsOmaha()) {
    write_log(Preferences()->debug_symbolengine(), "[CSymbolEngineIsOmaha] Omaha not supported by tablemap\n");
    return;
  }
  // Checking the two "additional" cards
  if (p_table_state->User()->hole_cards(2)->IsKnownCard()
    && p_table_state->User()->hole_cards(3)->IsKnownCard()) {
    write_log(Preferences()->debug_symbolengine(), "[CSymbolEngineIsOmaha] Found Omaha hole-cards\n");
    _isomaha = true;
    return;
  }
  write_log(Preferences()->debug_symbolengine(), "[CSymbolEngineIsOmaha] No indications for Omaha found\n");
}

bool CSymbolEngineIsOmaha::EvaluateSymbol(const CString name, double *result, bool log /* = false */) {
  FAST_EXIT_ON_OPENPPL_SYMBOLS(name);
  if (memcmp(name, "isomaha", 7)==0 && strlen(name)==7)	{
    // Up to now only Hold'Em supported
		*result = isomaha();
    return true;
	}
  // Symbol of a different symbol-engine
  return false;
}

CString CSymbolEngineIsOmaha::SymbolsProvided() {
  return "isomaha ";
}