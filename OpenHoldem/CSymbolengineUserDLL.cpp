//*******************************************************************************
//
// This file is part of the OpenHoldem project
//   Download page:         http://code.google.com/p/openholdembot/
//   Forums:                http://www.maxinmontreal.com/forums/index.php
//   Licensed under GPL v3: http://www.gnu.org/licenses/gpl.html
//
//*******************************************************************************
//
// Purpose: Interface to the user.DLL
//
//*******************************************************************************

#include "stdafx.h"
#include "CSymbolEngineUserDLL.h"

#include "CEngineContainer.h"
#include "CFormulaParser.h"
#include "CFunctionCollection.h"
#include "CHandresetDetector.h"
#include "CIteratorThread.h"
#include "COcclusionCheck.h"
#include "COpenHoldemTitle.h"
#include "CScraper.h"
#include "CSymbolengineVersus.h"
#include "CSymbolEngineHistory.h"
#include "CTableState.h"
#include "..\DLLs\User_DLL\user.h"

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
  DLLUpdateOnNewFormula();
}

void CSymbolEngineUserDLL::UpdateOnConnection() {
  DLLUpdateOnConnection();
}

void CSymbolEngineUserDLL::UpdateOnHandreset() {
  DLLUpdateOnHandreset();
  previous_sum_betround_action = -1;
  write_log(Preferences()->debug_symbolengine(),
      "[CSymbolEngineUserDLL] Mandatory symbols reset on Hand Reset : %i \n", previous_sum_betround_action);
}

void CSymbolEngineUserDLL::UpdateOnNewRound() {
  DLLUpdateOnNewRound();
  previous_sum_betround_action = -1;
  write_log(Preferences()->debug_symbolengine(),
      "[CSymbolEngineUserDLL] Mandatory symbols reset on New Round : %i \n", previous_sum_betround_action);
}

void CSymbolEngineUserDLL::UpdateOnMyTurn() {
  DLLUpdateOnMyTurn();
}

void CSymbolEngineUserDLL::UpdateOnHeartbeat() {
  DLLUpdateOnHeartbeat();
}

int CSymbolEngineUserDLL::sum_betround_action() {
    int action_number = 0;

    for (int i = 0; i <= k_autoplayer_function_fold; i++) {
        action_number += p_engine_container->symbol_engine_history()->_autoplayer_actions[BETROUND][i];
    }
    write_log(Preferences()->debug_symbolengine(),
        "[CSymbolEngineUserDLL] Number of actions executed by the autoplayer before the current orbit : %i \n", action_number);

    return action_number;

}

bool CSymbolEngineUserDLL::EvaluateSymbol(const CString name, double* result, bool log /* = false */) {
    FAST_EXIT_ON_OPENPPL_SYMBOLS(name);
    if (memcmp(name, "dll$", 4) != 0) {
        // Symbol of a different symbol-engine
        return false;
    }
    if (previous_sum_betround_action >= 0 && previous_sum_betround_action == sum_betround_action()) {
        // Return previously calculated result
        *result = previous_result;
        write_log(Preferences()->debug_symbolengine(),
            "[CSymbolEngineUserDLL] Detected multiple call to ProcessQuery for user.dll evaluation in the same orbit - Number of Autoplayer Actions - actual: %i -- previous: %i \n", sum_betround_action(), previous_sum_betround_action);
        write_log(Preferences()->debug_symbolengine(),
            "[CSymbolEngineUserDLL] Returning stored usr.dll results : %lf \n", previous_result);
        return true;
    }
    else {
        // Calculate new result and update previous_sum_betround_action and previous_result
        previous_sum_betround_action = sum_betround_action();
        previous_result = ProcessQuery(name);
        *result = previous_result;
        write_log(Preferences()->debug_symbolengine(),
            "[CSymbolEngineUserDLL] User.dll evaluation symbol returned true -- Calculated user.dll value : %lf -- Number of Autoplayer Actions - actual: %i -- previous: %i \n", *result, sum_betround_action(), previous_sum_betround_action);

        return true;
    }
}

CString CSymbolEngineUserDLL::SymbolsProvided() {
  return " ";
}

//*******************************************************************************
//
// Exported functions, needed by the DLL
//
//*******************************************************************************

EXE_IMPLEMENTS double GetSymbol(const char* name_of_single_symbol__not_expression) {
  CString	str = "";
  str.Format("%s", name_of_single_symbol__not_expression);
  if (strcmp(str, "cmd$recalc") == 0) {
    // restart iterator thread
    p_iterator_thread->RestartPrWinComputations();
    // Recompute versus tables
    p_engine_container->symbol_engine_versus()->GetCounts();
    // Busy waiting until recalculation got finished.
    // Nothing better to do, as we already evaluate bot-logic,
    // so we can't continue with another heartbeat or something else.
    // http://www.maxinmontreal.com/forums/viewtopic.php?f=111&t=19012
    while (!p_iterator_thread->IteratorThreadComplete()) {
      Sleep(100);
    }
    return 0;
  }
  double result = kUndefined;
  p_engine_container->EvaluateSymbol(name_of_single_symbol__not_expression,
    &result,
    kAlwaysLogDLLMessages);
  return result;
}

EXE_IMPLEMENTS void* GetPrw1326() {
  assert(p_iterator_thread != NULL);
  return (void *)(p_iterator_thread->prw1326());
}

EXE_IMPLEMENTS char* GetHandnumber() {
  return p_handreset_detector->GetHandNumber().GetBuffer();
}

EXE_IMPLEMENTS char* GetPlayerName(int chair) {
  if (chair < 0) {
    return nullptr;
  }
  if (chair > kLastChair) {
    return nullptr;
  }
  return p_table_state->Player(chair)->name().GetBuffer();
}

EXE_IMPLEMENTS char* GetTableTitle() {
  return p_table_state->TableTitle()->PreprocessedTitle().GetBuffer();
}

EXE_IMPLEMENTS void ParseHandList(const char* name_of_list, const char* list_body) {
  COHScriptList* p_new_list = new COHScriptList(name_of_list, list_body);
  p_formula_parser->ParseFormula(p_new_list);
  p_function_collection->Add(p_new_list);
}

EXE_IMPLEMENTS char* ScrapeTableMapRegion(char* p_region, int& p_returned_lengh) {
  CString result;
  bool success = p_scraper->EvaluateRegion(p_region, &result);
  if (success) {
    p_returned_lengh = result.GetLength() + 1;
    char* returnStr = static_cast<char*>(LocalAlloc(LPTR, p_returned_lengh));
    strcat(returnStr, result);
    return returnStr;
  }
  p_returned_lengh = kUndefined;
  return nullptr;
}

// EXE_IMPLEMENTS void SendChatMessage(char *message)
// gets implemented by PokerChat.cpp

EXE_IMPLEMENTS void WriteLog(char* fmt, va_list args) {
  // Docu about ellipsis and variadic macro:
  // http://msdn.microsoft.com/en-us/library/ms177415(v=vs.80).aspx
  // http://stackoverflow.com/questions/1327854/how-to-convert-a-variable-argument-function-into-a-macro

  write_log_vl(kAlwaysLogDLLMessages, fmt, args);
}