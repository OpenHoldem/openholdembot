//******************************************************************************
//
// This file is part of the OpenHoldem project
//    Source code:           https://github.com/OpenHoldem/openholdembot/
//    Forums:                http://www.maxinmontreal.com/forums/index.php
//    Licensed under GPL v3: http://www.gnu.org/licenses/gpl.html
//
//******************************************************************************
// Purpose: Validating the game-state, especially scraped data
//
// Purpose: Validating the game-state, especially scraped data
//
//******************************************************************************
#ifndef WINVER				// Allow use of features specific to Windows XP or later.
#define WINVER 0x0501
#endif

#include <afxwin.h>
#include "GamestateValidation.h"
// !!!!! move to numerical DLL
#include "OpenHoldem/FloatingPoint_Comparisions.h"
#include "WindowFunctions/window_functions.h"
#include "MagicNumbers/MagicNumbers.h"

// Constants for the validators range-checks
#define UNDEFINED_ZERO 0
#define	UNDEFINED_NEGATIVE_ONE -1

// Global var to be shared between interface and macro magic
bool heuristic_rules = false;

/////////////////////////////////////////////////////////////////////////////
//
// Macros for the validator,
// turning rules in "pseudo-code"
// into executable C-code.

// BEGIN_TESTCASE
//   
// Action: Ignore this by turning it into an empty string.
//
#define BEGIN_TESTCASE

// REASONING
//
// Action: Assign the reasoning to a private string-pointer.
//
#define REASONING(R) { _reasoning = (R); }

// TESTCASE_ID
//
// Action: Assign the testcase-ID to a private variable.
//
#define TESTCASE_ID(N) { _testcase_id = (N); }

// HEURISTIC
//
// Action: Assign the heuristic-flag to a private variable
//
//
#define HEURISTIC_RULE(H) { _rule_is_heuristic = (H); }

// PRECONDITION
//
// Action: Assign the precondition to a private variable.
//
#define PRECONDITION(C) { _precondition = (C); }

// POSTCONDITION
//
// Action: Assign the postcondition to a private variable.
//
#define POSTCONDITION(C) { _postcondition = (C); }

// SYMBOLS_POSSIBLY_AFFECTED
//
// Action: Defines a message about the symbols, that
//   could be affected by this rule. 
//
#define SYMBOLS_POSSIBLY_AFFECTED(S) { _symbols_possibly_affected = (S); }

// END_TESTCASE
//
// Actions: ValidateSingleRule(), i.e.: 
//   * execution of the testcase
//     (if precondition for the rule applies).
//   * message on error (postcondition not met).
//   * stop autoplayer on error (optional)
//
#define END_TESTCASE ValidateSingleRule();
	
//
// End of macro code
//
/////////////////////////////////////////////////////////////////////////////

char *_testcase_id;
bool _rule_is_heuristic;
char *_reasoning;
bool _precondition;
bool _postcondition;
char *_symbols_possibly_affected;
bool _no_errors_this_heartbeat;

// !! can be replaced by static linking once the symbol-engines are in a DLL
typedef double(*t_GetSymbol)(const char* name_of_single_symbol__not_expression);
t_GetSymbol p_GetSymbol = NULL;

double gws(const char *the_symbol) {
  if (p_GetSymbol == NULL) {
    // https://msdn.microsoft.com/en-us/library/windows/desktop/ms683199(v=vs.85).aspx
    HMODULE openholdem_main_module = GetModuleHandle(NULL);
    if (openholdem_main_module == NULL) {
      MessageBox_Error_Warning("GamestateValidation.DLL can't find OpenHoldem main module");
      return kUndefined;
    }
    // https://msdn.microsoft.com/en-us/library/windows/desktop/ms683212(v=vs.85).aspx
    p_GetSymbol = (t_GetSymbol)GetProcAddress(openholdem_main_module, "GetSymbol");
    if (p_GetSymbol == NULL) {
      MessageBox_Error_Warning("GamestateValidation.DLL can't find GetSymbol-function in OpenHoldem");
      return kUndefined;
    }
  }
  double result = p_GetSymbol(the_symbol);
  return result;
}

// Create a stringified list of (symbol: value)-pairs
// for output in the error-message.
CString Symbols_And_Values(const CString symbols_possibly_affected) {
  CString Result = "";
  int Token_Pos = 0;
  while (Token_Pos < symbols_possibly_affected.GetLength()) {
    // Tokenize the string, using space or commas as delimiters.
    CString Symbol = symbols_possibly_affected.Tokenize(" ,", Token_Pos);
    double Symbol_Value = gws(Symbol);
    CString Symbol_Value_String;
    // Convert symbol value to string, 7 digits total, 2 digits precision
    Symbol_Value_String.Format("%7.2f", Symbol_Value);
    Result += "\n    ("
      + Symbol
      + ": "
      + Symbol_Value_String
      + ")";
  }
  return Result;
}

void ValidateSingleRule() {
  // Heuristic rules and not to be tested?
  if (_rule_is_heuristic && !heuristic_rules) {
    return;
  }
  // Test to be executed?
  if (_precondition) {
    // Test failed?
    if (!_postcondition) {
      if (_no_errors_this_heartbeat) {
        // Keep error-status
        // Needed for replay-frames and autoplayer
        _no_errors_this_heartbeat = false;
      }
      // Create error message
      CString the_ErrorMessage = "TESTCASE ID: "
        + CString(_testcase_id)
        + "\n\n";
      if (_rule_is_heuristic) {
        the_ErrorMessage += "HEURISTIC RULE: yes\n\n";
      }
      the_ErrorMessage += "REASONING: "
        + CString(_reasoning)
        + "\n\n"
        + "SYMBOLS AFFECTED: "
        + Symbols_And_Values(_symbols_possibly_affected)
        + "\n\n"
        + "(The validator is a tool to help you finding errors in your tablemap.)\n";
      // Show error message, if appropriate
      // The error-message gets logged by the message-box
      MessageBox_Error_Warning(the_ErrorMessage, "VALIDATOR ERROR");
    }
  }
  return;
}

bool ValidateGamestate(
    bool use_heuristic_rules,
	  bool is_tournament,
	  bool versus_bin_loaded) {
  heuristic_rules = use_heuristic_rules;
  _no_errors_this_heartbeat = true;
#include "Validator_Rules\range_checks_general_symbols_inline.cpp_"
#include "Validator_Rules\range_checks_tablemap_symbols_inline.cpp_"
#include "Validator_Rules\range_checks_formula_file_inline.cpp_"
#include "Validator_Rules\range_checks_limits_inline.cpp_"
#include "Validator_Rules\range_checks_handrank_inline.cpp_"
#include "Validator_Rules\range_checks_chairs_inline.cpp_"
#include "Validator_Rules\range_checks_rounds_positions_inline.cpp_"
#include "Validator_Rules\range_checks_probabilities_inline.cpp_"
#include "Validator_Rules\range_checks_chip_amounts_inline.cpp_"
#include "Validator_Rules\range_checks_number_of_bets_inline.cpp_"
#include "Validator_Rules\range_checks_list_tests_inline.cpp_"
#include "Validator_Rules\range_checks_poker_values_inline.cpp_"
#include "Validator_Rules\range_checks_players_friends_opponents_inline.cpp_"
#include "Validator_Rules\range_checks_flags_inline.cpp_"
#include "Validator_Rules\range_checks_common_cards_inline.cpp_"
#include "Validator_Rules\range_checks_known_cards_inline.cpp_"
#include "Validator_Rules\range_checks_nhands_inline.cpp_"
#include "Validator_Rules\range_checks_flushes_straights_sets_inline.cpp_"
#include "Validator_Rules\range_checks_rank_bits_inline.cpp_"
#include "Validator_Rules\range_checks_rank_hi_inline.cpp_"
#include "Validator_Rules\range_checks_rank_lo_inline.cpp_"
#include "Validator_Rules\range_checks_time_inline.cpp_"
#include "Validator_Rules\range_checks_autoplayer_inline.cpp_"
#include "Validator_Rules\range_checks_action_symbols_inline.cpp_"
#include "Validator_Rules\range_checks_table_stats_inline.cpp_"
#include "Validator_Rules\range_checks_card_symbols_inline.cpp_"
#include "Validator_Rules\range_checks_NOT_TO_DO_inline.cpp_"
#include "Validator_Rules\consistency_checks_cards_inline.cpp_"
#include "Validator_Rules\consistency_checks_buttons_inline.cpp_"
#include "Validator_Rules\consistency_checks_handreset_inline.cpp_"
#include "Validator_Rules\consistency_checks_memory_symbols_inline.cpp_"
#include "Validator_Rules\consistency_checks_time_inline.cpp_"
#include "Validator_Rules\consistency_checks_table_stats_inline.cpp_"
#include "Validator_Rules\consistency_checks_general_inline.cpp_"
#include "Validator_Rules\consistency_checks_history_inline.cpp_"
#include "Validator_Rules\consistency_checks_bets_inline.cpp_"
#include "Validator_Rules\consistency_checks_autoplayer_inline.cpp_"
#include "Validator_Rules\consistency_checks_players_friends_opponents_inline.cpp_"
#include "Validator_Rules\consistency_checks_chip_amounts_inline.cpp_"
#include "Validator_Rules\consistency_checks_limits_inline.cpp_"
#include "Validator_Rules\consistency_checks_number_of_bets_inline.cpp_"
#include "Validator_Rules\consistency_checks_action_symbols_inline.cpp_"
  if  (is_tournament) {
#include "Validator_Rules\range_checks_icm_symbols_inline.cpp_"
  }
  if (versus_bin_loaded) {
#include "Validator_Rules\range_checks_versus_inline.cpp_"
  }
  return _no_errors_this_heartbeat;
} 