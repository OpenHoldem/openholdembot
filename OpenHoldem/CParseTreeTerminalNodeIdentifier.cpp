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
#include "CParseTreeTerminalNodeIdentifier.h"

#include <math.h>
#include "CAutoplayerTrace.h"
#include "CEngineContainer.h"
#include "CFunctionCollection.h"
#include "CMemoryPool.h"
#include "CParserSymbolTable.h"

#include "CSymbolEngineChipAmounts.h"
#include "CSymbolEngineMemorySymbols.h"
#include "CSymbolEngineOpenPPLUserVariables.h"
#include "CSymbolEngineTableLimits.h"
#include "FloatingPoint_Comparisions.h"

#include "..\DLLs\WindowFunctions_DLL\window_functions.h"
#include "..\DLLs\StringFunctions_DLL\string_functions.h"
#include "TokenizerConstants.h"

CParseTreeTerminalNodeIdentifier::CParseTreeTerminalNodeIdentifier(
    int relative_line_number, CString name) : 
    CParseTreeTerminalNode(relative_line_number) {
  _node_type = kTokenIdentifier;
  _terminal_name = name;
  assert(p_parser_symbol_table != NULL);
  p_parser_symbol_table->VerifySymbol(name);
}

CParseTreeTerminalNodeIdentifier::~CParseTreeTerminalNodeIdentifier() {
}

double CParseTreeTerminalNodeIdentifier::Evaluate(bool log /* = false */){
 write_log(Preferences()->debug_formula(), 
    "[CParseTreeTerminalNode] Evaluating node type %i %s\n", 
		_node_type, TokenString(_node_type));
  p_autoplayer_trace->SetLastEvaluatedRelativeLineNumber(_relative_line_number);
	// Most common types first: numbers and identifiers
  if (_node_type == kTokenIdentifier) {
    assert(_first_sibbling  == NULL);
    assert(_second_sibbling == NULL);
    assert(_third_sibbling  == NULL);
		assert(_terminal_name != "");
		double value = EvaluateIdentifier(_terminal_name, log);
		write_log(Preferences()->debug_formula(), 
      "[CParseTreeTerminalNode] Identifier %s evaluates to %.3f\n", 
      _terminal_name, value);
    // In case of f$-functions the line changed inbetween,
    // so we have to set it to the current location (again)
    // for the next log.
    p_autoplayer_trace->SetLastEvaluatedRelativeLineNumber(_relative_line_number);
		return value;
	}
	// This must not happen for a terminal node
	assert(false);
	return kUndefined;
}

CString CParseTreeTerminalNodeIdentifier::EvaluateToString(bool log /* = false */) {
  double numerical_result = Evaluate(log);
  CString result;
  if (IsInteger(numerical_result) && EvaluatesToBinaryNumber()) {
    // Generqate binary representation;
    result.Format("%s", IntToBinaryString(int(numerical_result)));
  } else {
    // Generate floating-point representation
    // with 3 digits precision
    result.Format("%.3f", numerical_result);
  }
  return result;
}

double CParseTreeTerminalNodeIdentifier::EvaluateIdentifier(CString name, bool log) {
	// EvaluateSymbol cares about ALL symbols, 
	// including DLL and PokerTracker.
	double result;
	p_engine_container->EvaluateSymbol(name, &result, log);
	return result;
}

CString CParseTreeTerminalNodeIdentifier::Serialize() {
  if (_node_type == kTokenIdentifier) {
    return _terminal_name;
  } else {
    // Unhandled note-type, probably new and therefore not yet handled
   write_log(k_always_log_errors, "[CParseTreeTerminalNode] ERROR: Unhandled node-type %i in serialization of parse-tree\n",
      _node_type);
    return "";
  }
}

bool CParseTreeTerminalNodeIdentifier::IsBinaryIdentifier() {
  const int kNumberOfElementaryBinaryIdentifiers = 21;
  static const char* kElementaryBinaryIdentifiers[kNumberOfElementaryBinaryIdentifiers] = {
    "pcbits",              "playersactivebits",  "playersdealtbits",
    "playersplayingbits",  "playersblindbits",   "opponentsseatedbits",
    "opponentsactivebits", "opponentsdealtbits", "opponentsplayingbits",
    "opponentsblindbits",  "flabitgs",           "rankbits",
    "rankbitscommon",      "rankbitsplayer",     "rankbitspoker",
    "srankbits",           "srankbitscommon",    "srankbitsplayer",
    "srankbitspoker",      "myturnbits",         "pcbits"};
  const int kNumberOfParameterizedBinaryIdentifiers = 5;
  static const char* kParameterizedBinaryIdentifiers[kNumberOfParameterizedBinaryIdentifiers] = {
    "chairbit$", "raisbits", "callbits", "foldbits", "suitbits"};

  if (_node_type != kTokenIdentifier) return false;
  assert(_terminal_name != "");
  // Check elementary binary identifiers first
  for (int i=0; i<kNumberOfElementaryBinaryIdentifiers; ++i) {
    if (_terminal_name == kElementaryBinaryIdentifiers[i]) return true;
  }
  // Then check parameterized binary symbols
  for (int i=0; i<kNumberOfParameterizedBinaryIdentifiers; ++i) {
    if (StringAIsPrefixOfStringB(kParameterizedBinaryIdentifiers[i],
        _terminal_name)) {
      return true;
    }                                 
  }
  // Not a binary identifier
  return false;
}

bool CParseTreeTerminalNodeIdentifier::EvaluatesToBinaryNumber() {
  if (TokenEvaluatesToBinaryNumber(_node_type)) {
    // Operation that evaluates to binary number,
    // e.g. bit-shift, logical and, etc.
    return true;
  }
  else if (IsBinaryIdentifier()) return true;
  else if ((_node_type == kTokenIdentifier)
      && p_function_collection->EvaluatesToBinaryNumber(_terminal_name)) {
    return true;
  }
  // Nothing binary
  return false;
}

