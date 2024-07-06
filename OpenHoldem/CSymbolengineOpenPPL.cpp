//***************************************************************************** 
//
// This file is part of the OpenHoldem project
//    Source code:           https://github.com/OpenHoldem/openholdembot/
//    Forums:                http://www.maxinmontreal.com/forums/index.php
//    Licensed under GPL v3: http://www.gnu.org/licenses/gpl.html
//
//***************************************************************************** 
//
// Purpose: This symbol-engine does not provide any symbols,
//   but triggers only the calculation of history/symbols, etc.
//   All OpenPPL-symbols are functions that get handled by the function-collection.
//
//***************************************************************************** 

#include "stdafx.h"
#include "CSymbolEngineOpenPPL.h"

#include "CEngineContainer.h"
#include "CFunctionCollection.h"

#include "CScraper.h"
#include "CStringMatch.h"
#include "CSymbolEngineOpenPPLUserVariables.h"
#include "CSymbolEngineVariousDataLookup.h"

#include "WindowFunctions/window_functions.h"

const char* kOpenPPLUpdateOnHandReset =
  "UpdateMemorySymbolsOnHandReset";
const char* kOpenPPLUpdateOnNewRound =
  "UpdateMemorySymbolsOnNewRound";
const char* kOpenPPLUpdateMemorySymbolsAfterAutoplayerAction =
  "UpdateMemorySymbolsAfterAutoplayerAction";

CSymbolEngineOpenPPL::CSymbolEngineOpenPPL() {
	// The values of some symbol-engines depend on other engines.
	// As the engines get later called in the order of initialization
	// we assure correct ordering by checking if they are initialized.
	//
	// This engine does not use any other engines, 
  // but it triggers calculations in the OpenPPL-library
  // that use nearly all other symbol-engines.
  // For convenience we assure only 
  // that the last other engines got initialiyed
  assert(p_engine_container->symbol_engine_various_data_lookup() != NULL);
  assert(p_engine_container->symbol_engine_openppl_user_variables() != NULL);
}

CSymbolEngineOpenPPL::~CSymbolEngineOpenPPL() {
}

void CSymbolEngineOpenPPL::CheckExistenceOfOpenPPLFunction(CString function) {
  if (!p_function_collection->Exists(function)) {
    CString message;
    message.Format("Can't find initialization-function\n"
      "%s\n"
      "in OpenPPL-library.\n"
      "\n"
      "Without this function most history-symbols will be broken.\n"
      "Probably caused by one-sided renaming of that function.",
      function);
    MessageBox_Error_Warning(message);
  }
}

void CSymbolEngineOpenPPL::InitOnStartup() {
}

void CSymbolEngineOpenPPL::UpdateOnConnection() {
  VerifyExistenceOfOpenPPLInitializationInLibrary();
  UpdateOnHandreset();
}

void CSymbolEngineOpenPPL::UpdateOnHandreset() {
  p_function_collection->Evaluate(
    kOpenPPLUpdateOnHandReset,
    kAlwaysLogOpenPPLInitialization);
}

void CSymbolEngineOpenPPL::UpdateOnNewRound() {
  p_function_collection->Evaluate(
    kOpenPPLUpdateOnNewRound,
    kAlwaysLogOpenPPLInitialization);
}

void CSymbolEngineOpenPPL::UpdateOnMyTurn() {
}

void CSymbolEngineOpenPPL::VerifyExistenceOfOpenPPLInitializationInLibrary() {
  // Verify that ini-functions exists.
  // Avoid PEBKACs by stupid renaming, that breaks everything
  CheckExistenceOfOpenPPLFunction(kOpenPPLUpdateOnHandReset);
  CheckExistenceOfOpenPPLFunction(kOpenPPLUpdateOnNewRound);
  CheckExistenceOfOpenPPLFunction(kOpenPPLUpdateMemorySymbolsAfterAutoplayerAction);
}

void CSymbolEngineOpenPPL::UpdateOnHeartbeat() {
}

void CSymbolEngineOpenPPL::UpdateAfterAutoplayerAction(int autoplayer_action_code) {
  p_function_collection->Evaluate(
    kOpenPPLUpdateMemorySymbolsAfterAutoplayerAction,
    kAlwaysLogOpenPPLInitialization);
}

bool CSymbolEngineOpenPPL::EvaluateSymbol(const CString name, double *result, bool log /* = false */) {
	// This symbol-engine does not provide any symbols,
  // but triggers only the calculation of history/symbols, etc.
  // All OpenPPL-symbols are functions that get handled by the function-collection.
	return false;
}