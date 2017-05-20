//******************************************************************************
//
// This file is part of the OpenHoldem project
//    Source code:           https://github.com/OpenHoldem/openholdembot/
//    Forums:                http://www.maxinmontreal.com/forums/index.php
//    Licensed under GPL v3: http://www.gnu.org/licenses/gpl.html
//
//******************************************************************************
//
// Purpose: Detecting if we play rush / zoom / ...
//   depending on the time since our last action on handreset.
//
//******************************************************************************

#include "stdafx.h"
#include "CSymbolEngineIsRush.h"

#include "CPreferences.h"
#include "CScraper.h"
#include "CSymbolEngineActiveDealtPlaying.h"
#include "CSymbolEngineIsTournament.h"
#include "CSymbolEngineTime.h"
#include "CTableState.h"
#include "MagicNumbers.h"

CSymbolEngineIsRush *p_symbol_engine_isrush = NULL;

// Maximum average time between last action and handreset
// if the game should be considered rush / zoom.
// Ploease note: detecting hand-resets can take several heartbeats (2..4)
// so this value might need some fine-tuning.
const int kMaxAverageDurationOfHandreset = 4;

CSymbolEngineIsRush::CSymbolEngineIsRush() {
  // The values of some symbol-engines depend on other engines.
  // As the engines get later called in the order of initialization
  // we assure correct ordering by checking if they are initialized.
  assert(p_symbol_engine_active_dealt_playing !=NULL);
  assert(p_symbol_engine_time != NULL);
  assert(p_symbol_engine_istournament != NULL);
}

CSymbolEngineIsRush::~CSymbolEngineIsRush() {
}

void CSymbolEngineIsRush::InitOnStartup() {
	UpdateOnConnection();
}

void CSymbolEngineIsRush::UpdateOnConnection() {
	sum_of_handreset_durations = 0.0;
  // Init to 1 to avoid division by zero
  handresets = 1;
}

void CSymbolEngineIsRush::UpdateOnHandreset() {
  if (p_symbol_engine_time->elapsedauto() > 60) {
    // Unreliable value, we might have been sitting out
    return;
  }
  sum_of_handreset_durations += p_symbol_engine_time->elapsedauto();
  ++handresets;
}

void CSymbolEngineIsRush::UpdateOnNewRound() {
}

void CSymbolEngineIsRush::UpdateOnMyTurn() {
}

void CSymbolEngineIsRush::UpdateOnHeartbeat() {
}

bool CSymbolEngineIsRush::isrush() {
  if (p_symbol_engine_istournament->istournament()) {
    // Not a cash-game, therefore not rush
    write_log(preferences.debug_symbolengine(),
      "[CSymbolEngineIsRush] Tournament, therefore not rush / zoom\n");
    return false;
  }
  else if (p_symbol_engine_active_dealt_playing->nopponentsdealt()<2){
    // Game may be HU
    write_log(preferences.debug_symbolengine(),
      "[CSymbolEngineIsRush] One dealt opponent, game may or may not be rush / zoom\n");
      return false;
  }
  assert(handresets > 0);
  write_log(preferences.debug_symbolengine(),
    "[CSymbolEngineIsRush] Average duration of hand-reset: %.2f\n",
    sum_of_handreset_durations / handresets);
  if (sum_of_handreset_durations / handresets > kMaxAverageDurationOfHandreset) {
    // Too long duration
    return false;
  }
  return true;
}

bool CSymbolEngineIsRush::EvaluateSymbol(const CString name, double *result, bool log /* = false */) { 
  FAST_EXIT_ON_OPENPPL_SYMBOLS(name);
  if (memcmp(name, "isrush", 6)==0)  {
    *result = isrush();
    return true;
  }
  // Symbol of a different symbol-engine
  return false;
}

CString CSymbolEngineIsRush::SymbolsProvided() {
  return "isrush ";
}
	