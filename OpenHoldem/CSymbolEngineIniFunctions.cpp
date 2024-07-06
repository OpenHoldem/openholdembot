//******************************************************************************
//
// This file is part of the OpenHoldem project
//    Source code:           https://github.com/OpenHoldem/openholdembot/
//    Forums:                http://www.maxinmontreal.com/forums/index.php
//    Licensed under GPL v3: http://www.gnu.org/licenses/gpl.html
//
//******************************************************************************
//
// Purpose: Providing functions (f$init_once_per_hand, etc.)
//   to deal with memory-symbols and other initializations (DLL maybe)
//   and get rid of the old advice "put it into f$alli". 
//
// Not really a symbol-engine, but easy to implement with this concept.
//
//******************************************************************************

#include "stdafx.h"
#include "CSymbolEngineIniFunctions.h"

#include "CEngineContainer.h"
#include "CFormulaParser.h"
#include "CFunctionCollection.h"

#include "CSymbolEngineActiveDealtPlaying.h"
#include "CSymbolEngineAutoplayer.h"
#include "CSymbolEngineBlinds.h"
#include "CSymbolEngineCards.h"
#include "CSymbolEngineChipAmounts.h"
#include "CSymbolEngineDealerchair.h"
#include "CSymbolEngineHandrank.h"
#include "CSymbolEngineHistory.h"
#include "CSymbolEngineIsOmaha.h"
#include "CSymbolEngineIsTournament.h"
#include "CSymbolEnginePokerTracker.h"
#include "CSymbolEnginePokerval.h"
#include "CSymbolEnginePositions.h"
#include "CSymbolEnginePrwin.h"
#include "CSymbolEngineRaisers.h"
#include "CSymbolEngineRandom.h"
#include "CSymbolEngineTime.h"
#include "CSymbolEngineUserchair.h"

// We can't evaluate ini-functions if no formula is loaded.
// This was no problem on startup, but caused crashes
// when we tried to load a new formula.
// http://www.maxinmontreal.com/forums/viewtopic.php?f=110&t=16788&start=30
#define RETURN_IF_LOADING_NEW_FORMULA if ((p_function_collection == NULL) || ((p_formula_parser != NULL) && (p_formula_parser->IsParsing()))) return;

CSymbolEngineIniFunctions::CSymbolEngineIniFunctions() {
  // The values of some symbol-engines depend on other engines.
  // As the engines get later called in the order of initialization
  // we assure correct ordering by checking if they are initialized.
  //
  // CSymbolEngineIniFunctions() "depends" on all other engines,
  // as it can only be called after all symbols have been initialized.
  assert(p_engine_container->symbol_engine_active_dealt_playing() != NULL);
  assert(p_engine_container->symbol_engine_autoplayer() != NULL);
  assert(p_engine_container->symbol_engine_blinds() != NULL);
  assert(p_engine_container->symbol_engine_cards() != NULL);
  assert(p_engine_container->symbol_engine_chip_amounts() != NULL);
  assert(p_engine_container->symbol_engine_dealerchair() != NULL);
  assert(p_engine_container->symbol_engine_handrank() != NULL);
  assert(p_engine_container->symbol_engine_history() != NULL);
  assert(p_engine_container->symbol_engine_isomaha() != NULL);
  assert(p_engine_container->symbol_engine_istournament() != NULL);
  assert(p_engine_container->symbol_engine_pokertracker() != NULL);
  assert(p_engine_container->symbol_engine_pokerval() != NULL);
  assert(p_engine_container->symbol_engine_positions() != NULL);
  assert(p_engine_container->symbol_engine_prwin() != NULL);
  assert(p_engine_container->symbol_engine_raisers() != NULL);
  assert(p_engine_container->symbol_engine_random() != NULL);
  assert(p_engine_container->symbol_engine_time() != NULL);
  assert(p_engine_container->symbol_engine_userchair() != NULL);
}

CSymbolEngineIniFunctions::~CSymbolEngineIniFunctions() {
}

void CSymbolEngineIniFunctions::InitOnStartup() {
  RETURN_IF_LOADING_NEW_FORMULA
  write_log(Preferences()->debug_symbolengine(), "[Symbolengine] CSymbolEngineIniFunctions::InitOnStartup()\n");
  UpdateOnConnection();
  p_function_collection->Evaluate(k_standard_function_names[k_init_on_startup], Preferences()->log_ini_functions());
}

void CSymbolEngineIniFunctions::UpdateOnConnection() {
  RETURN_IF_LOADING_NEW_FORMULA
  write_log(Preferences()->debug_symbolengine(), "[Symbolengine] CSymbolEngineIniFunctions::UpdateOnConnection()\n");
  p_function_collection->Evaluate(k_standard_function_names[k_init_on_connection],
    Preferences()->log_ini_functions());
  write_log(Preferences()->debug_symbolengine(), "[Symbolengine] CSymbolEngineIniFunctions::UpdateOnConnection() completed\n");
}

void CSymbolEngineIniFunctions::UpdateOnHandreset(){
  RETURN_IF_LOADING_NEW_FORMULA
  write_log(Preferences()->debug_symbolengine(), "[Symbolengine] CSymbolEngineIniFunctions::UpdateOnHandreset()\n");
  p_function_collection->Evaluate(k_standard_function_names[k_init_on_handreset],
    Preferences()->log_ini_functions());
}

void CSymbolEngineIniFunctions::UpdateOnNewRound() {
  RETURN_IF_LOADING_NEW_FORMULA
  write_log(Preferences()->debug_symbolengine(), "[Symbolengine] CSymbolEngineIniFunctions::UpdateOnNewRound()\n");
  p_function_collection->Evaluate(k_standard_function_names[k_init_on_new_round],
    Preferences()->log_ini_functions());
}

void CSymbolEngineIniFunctions::UpdateOnMyTurn() {
  RETURN_IF_LOADING_NEW_FORMULA
  write_log(Preferences()->debug_symbolengine(), "[Symbolengine] CSymbolEngineIniFunctions::UpdateOnMyTurn()\n");
  p_function_collection->Evaluate(k_standard_function_names[k_init_on_my_turn],
    Preferences()->log_ini_functions());
}

void CSymbolEngineIniFunctions::UpdateOnHeartbeat() {
  RETURN_IF_LOADING_NEW_FORMULA
  write_log(Preferences()->debug_symbolengine(), "[Symbolengine] CSymbolEngineIniFunctions::UpdateOnHeartbeat()\n");
  write_log(Preferences()->debug_symbolengine(), "[Symbolengine] CSymbolEngineIniFunctions::UpdateOnHeartbeat() evaluating %s\n",
	  k_standard_function_names[k_init_on_heartbeat]);
  p_function_collection->Evaluate(k_standard_function_names[k_init_on_heartbeat],
    Preferences()->log_ini_functions());
}

CString CSymbolEngineIniFunctions::SymbolsProvided() {
  // This symbol-engine does not really provide any symbols.
  // It just makes use of certain events (UpdateOnHeartbeat, etc.)
  // and all f$-functions will get added by the function-collection
  // for syntax-highlighting at once.
  return "";
}