//*******************************************************************************
//
// This file is part of the OpenHoldem project
//   Download page:         http://code.google.com/p/openholdembot/
//   Forums:                http://www.maxinmontreal.com/forums/index.php
//   Licensed under GPL v3: http://www.gnu.org/licenses/gpl.html
//
//*******************************************************************************
//
// Purpose: multiplexer functionality to support postfixes
//   like "dealer" or "last_caller_chair" instead of chair numbers.
//   This should be one of the very first symbol-engines to be called
//   to allow proper replacement of the postfix and a new query 
//   before other symbol engines try to handle this query.
//
//*******************************************************************************

#include "stdafx.h"
#include "CSymbolEngineMultiplexer.h"

#include "CEngineContainer.h"
#include "CFormulaParser.h"

#include "..\DLLs\WindowFunctions_DLL\window_functions.h"
#include "..\DLLs\StringFunctions_DLL\string_functions.h"

const int kNumberOfSupportedPostfixes = 31;
// If you extend the list below please don't forget to update FastExitOnLastCharacter()
const char* kSupportedPostFixes[kNumberOfSupportedPostfixes] = {
  // Chairs, special ones
  "userchair",
  "headsupchair",
  // Chairs, by position
  "smallblindchair",
  "bigblindchair",
  "dealerchair",
  "buttonchair",
  "cutoffchair",
  "mp3chair",
  "mp2chair",
  "mp1chair",
  "ep3chair",
  "ep2chair",
  "ep1chair",
  "utgchair",
  // Chairs, raisers and callers
  "firstcallerchair",
  "lastcallerchair",
  "firstraiserchair",
  "lastraiserchair",
  // lastraiserchair and raischair are synonyms
  "raischair",
  // big-stack and small-stack cjair
  "bigstackchair",
  "smallstackchair",
  // Betrounds
  "preflop",
  "flop",
  "turn",
  "river",
  "previousround", 
  "currentround", 
  // Suits
  "clubs",
  "diamonds",
  "hearts",
  "spades"
};

CSymbolEngineMultiplexer::CSymbolEngineMultiplexer() {
	// The values of some symbol-engines depend on other engines.
	// As the engines get later called in the order of initialization
	// we assure correct ordering by checking if they are initialized.
	//
	// This engine does not use any other engines.
	_no_postfix = false;
	_valid_postfix = false;
}

CSymbolEngineMultiplexer::~CSymbolEngineMultiplexer() {
}

void CSymbolEngineMultiplexer::InitOnStartup() {
  ResetOnConnection();
}

void CSymbolEngineMultiplexer::ResetOnConnection() {
}

void CSymbolEngineMultiplexer::ResetOnHandreset() {
}

void CSymbolEngineMultiplexer::ResetOnNewRound() {
}

void CSymbolEngineMultiplexer::ResetOnMyTurn() {
}

void CSymbolEngineMultiplexer::ResetOnHeartbeat() {
}

inline bool CSymbolEngineMultiplexer::FastExitOnLastCharacter(int last_character_of_symbol) {
  switch (last_character_of_symbol) {
    // Last characters of multiplexed postfixes
    case 'd':
    case 'n':
    case 'p':
    case 'r': 
    case 's': return false;
    default: return true;
  }
}

CString CSymbolEngineMultiplexer::MultiplexedSymbolName(CString name) {
  int underscore_position = 0;
  _no_postfix = false;
  // Searching from the end, because a symbol might contain
  // multiple multiplexer postfixes at the end.
  underscore_position = name.ReverseFind('_');
  if (underscore_position < 0) {
    // No postfix found
	_no_postfix = true;
    return name;
  }
  CString postfix = name.Mid(underscore_position + 1);
  write_log(Preferences()->debug_multiplexer(), "[CSymbolEngineMultiplexer] Postfix %s\n", postfix);
  _valid_postfix = false;
  for (int i = 0; i < kNumberOfSupportedPostfixes; ++i) {
    if (postfix == kSupportedPostFixes[i]) {
		_valid_postfix = true;
      break;
    }
  }
  if (!_valid_postfix) {
    return name;
  }
  double evaluated_postfix = kUndefined;
  p_engine_container->EvaluateSymbol(postfix, &evaluated_postfix, false);
  if (evaluated_postfix == kUndefined) {
    // Valid postfix, but invalid (negative) value
	  if (p_formula_parser->IsParsing()) {
      // Some (chair) values are not available,
      // but we want to continue to verify the entire multiplexer-symbol.
      // Continue with chair 0
      evaluated_postfix = 0;
    } else {
      // Playing, but this chair does not exist,
      // e.g. ep1chair in a short-handed game.
      return kEmptyExpression_False_Zero_WhenOthersFoldForce;
    }
  }
  CString pure_name = name.Left(underscore_position);
  write_log(Preferences()->debug_multiplexer(), "[CSymbolEngineMultiplexer] Pure name %s\n", pure_name);
  // Recurse, to resolve multiple postfixes
  CString multiplexed_name = MultiplexedSymbolName(pure_name);
  // Append multiplexed postfix by its evaluation,
  // e.g. balance_user -> balance3 if the user sits in chair 3.
  multiplexed_name += Number2CString(evaluated_postfix, 0);
  write_log(Preferences()->debug_multiplexer(), "[CSymbolEngineMultiplexer] Multiplexed name %s\n",
    multiplexed_name);
  return multiplexed_name;
}

bool CSymbolEngineMultiplexer::EvaluateSymbol(const CString name, double *result, bool log /* = false */) {
	// This function gets called a lot, 
	// very early and therefore for nearly every symbol.
	// Therefore we implement several fast exits.
	// But no FAST_EXIT_ON_OPENPPL_SYMBOLS(name),
	// because we also want to multiplex OpenPPL-symbols.
	//
	// Fast exit on last character
	char last_character = RightCharacter(name);
	if (FastExitOnLastCharacter(last_character)) {
		return false;
	}
	CString multiplexed_symbol_name = MultiplexedSymbolName(name);
	if (multiplexed_symbol_name == name) {
		// Not a multiplexer-symbol
		return false;
	}
	bool success = p_engine_container->EvaluateSymbol(multiplexed_symbol_name, result, log);
	write_log(Preferences()->debug_multiplexer(), "[CSymbolEngineMultiplexer] %s -> %s -> %.2f\n",
		name, multiplexed_symbol_name, *result);
	return success;
}

bool CSymbolEngineMultiplexer::EvaluateSymbol(const CString name, CString *result, bool log /* = false */) {
	// This function gets called a lot, 
	// very early and therefore for nearly every symbol.
	// Therefore we implement several fast exits.
	// But no FAST_EXIT_ON_OPENPPL_SYMBOLS(name),
	// because we also want to multiplex OpenPPL-symbols.
	//
	// Fast exit on last character
	char last_character = RightCharacter(name);
	if (FastExitOnLastCharacter(last_character)) {
		return false;
	}
	CString multiplexed_symbol_name = MultiplexedSymbolName(name);
	if (multiplexed_symbol_name == name) {
		// Not a multiplexer-symbol
		return false;
	}
	bool success = p_engine_container->EvaluateSymbol(multiplexed_symbol_name, result, log);
	write_log(Preferences()->debug_multiplexer(), "[CSymbolEngineMultiplexer] %s -> %s -> %s\n",
		name, multiplexed_symbol_name, *result);
	return success;
}

CString CSymbolEngineMultiplexer::SymbolsProvided() {
  // Way too much to name them all.
  // List depends heavily on other symbol-egines,
  // so we leave it empty at the cost of
  // losing syntax-highlighting for multiplexed symbols.
  return "";
}




