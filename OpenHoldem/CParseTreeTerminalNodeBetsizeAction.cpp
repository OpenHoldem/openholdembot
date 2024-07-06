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
#include "CParseTreeTerminalNodeBetsizeAction.h"

#include <math.h>
#include "CAutoplayerTrace.h"
#include "CEngineContainer.h"
#include "CFunction.h"
#include "CFunctionCollection.h"
#include "CMemoryPool.h"
#include "CParserSymbolTable.h"
#include "CParseTreeTerminalNode.h"
#include "CParseTreeTerminalNodeIdentifier.h"

#include "CSymbolEngineChipAmounts.h"
#include "CSymbolEngineMemorySymbols.h"
#include "CSymbolEngineOpenPPLUserVariables.h"
#include "CSymbolEngineTableLimits.h"
#include "FloatingPoint_Comparisions.h"

#include "WindowFunctions/window_functions.h"
#include "StringFunctions/string_functions.h"
#include "TokenizerConstants.h"

CParseTreeTerminalNodeBetsizeAction::CParseTreeTerminalNodeBetsizeAction(int relative_line_number) : 
    CParseTreeNode(relative_line_number)  {
}

CParseTreeTerminalNodeBetsizeAction::~CParseTreeTerminalNodeBetsizeAction() {
}

void CParseTreeTerminalNodeBetsizeAction::MakeRaiseToAction(TPParseTreeNode raise_by_amount_in_big_blinds) {
	_node_type = kTokenActionRaiseToBigBlinds;
	_first_sibbling = raise_by_amount_in_big_blinds;
}

void CParseTreeTerminalNodeBetsizeAction::MakeRaiseByAction(TPParseTreeNode raise_by_amount_in_big_blinds) {
	_node_type = kTokenActionRaiseByBigBlinds;
	_first_sibbling = raise_by_amount_in_big_blinds;
}

// Values to be expected in the range (0..100] or more, not (0..1]
void CParseTreeTerminalNodeBetsizeAction::MakeRaiseByPercentagedPotsizeAction(
  	TPParseTreeNode raise_by_amount_percentage) {
	_node_type = kTokenActionRaiseByPercentagedPotsize;
	_first_sibbling = raise_by_amount_percentage;
}

double CParseTreeTerminalNodeBetsizeAction::Evaluate(bool log /* = false */) {
 write_log(Preferences()->debug_formula(), 
    "[CParseTreeTerminalNodeBetsizeAction] Evaluating node type %i %s\n", 
		_node_type, TokenString(_node_type));
  p_autoplayer_trace->SetLastEvaluatedRelativeLineNumber(_relative_line_number);
	// Actions first, which are "unary".
	// We have to encode all possible outcomes in a single floating-point,
	// therefore:
	// * positive values mean: raise size (by big-blinds, raise-to-semantics) 
	// * negative values mean: elementary actions
	if (_node_type == kTokenActionRaiseToBigBlinds)	{
    // RaiseTo N Force
		return _first_sibbling->Evaluate(log);
	}	else if (_node_type == kTokenActionRaiseByBigBlinds)	{
    // RaiseBy N Force
    double raise_by_amount_in_bblinds = _first_sibbling->Evaluate(log);
    double final_betsize_in_bblinds = p_engine_container->symbol_engine_chip_amounts()->ncallbets()
      + raise_by_amount_in_bblinds;
    write_log(Preferences()->debug_formula(), 
      "[CParseTreeTerminalNodeBetsizeAction] raiseby = %.2f ncallbets = %.2f final = %.2f\n",
      raise_by_amount_in_bblinds,
      p_engine_container->symbol_engine_chip_amounts()->ncallbets(),
      final_betsize_in_bblinds);
		return final_betsize_in_bblinds;
	}	else if (_node_type == kTokenActionRaiseByPercentagedPotsize)	{
    // RaiseBy X% Force
		double raise_by_percentage = _first_sibbling->Evaluate(log);
    assert(p_engine_container->symbol_engine_tablelimits()->bet() > 0);
		double pot_size_after_call_in_big_blinds = 
      (p_engine_container->symbol_engine_chip_amounts()->pot() / p_engine_container->symbol_engine_tablelimits()->bet()) 
      + p_engine_container->symbol_engine_chip_amounts()->nbetstocall();
    assert(pot_size_after_call_in_big_blinds >= 0);
		double raise_by_amount_in_bblinds = 0.01 * raise_by_percentage
			* pot_size_after_call_in_big_blinds;
    double final_betsize_in_bblinds = p_engine_container->symbol_engine_chip_amounts()->ncallbets()
      + raise_by_amount_in_bblinds;
    write_log(Preferences()->debug_formula(), 
      "[CParseTreeTerminalNodeBetsizeAction] raiseby percentage = %.2f pot after call = %.2f raiseby = %.2f final = %.2f\n",
      raise_by_percentage,
      pot_size_after_call_in_big_blinds,
      raise_by_amount_in_bblinds,
      final_betsize_in_bblinds);
    return final_betsize_in_bblinds;
  } 
	assert(false);
	return kUndefined;
}

CString CParseTreeTerminalNodeBetsizeAction::EvaluateToString(bool log /* = false */) {
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

CString CParseTreeTerminalNodeBetsizeAction::Serialize() {
  if (TokenIsBracketOpen(_node_type)) {
    return ("(" + _first_sibbling->Serialize() + ")");
  } else if (TokenIsUnary(_node_type)) {
    assert(_first_sibbling != NULL);
    return TokenString(_node_type) + "(" + _first_sibbling->Serialize() + ")";
  } else if (TokenIsBinary(_node_type)) {
    assert(_first_sibbling != NULL);
    assert(_second_sibbling != NULL);
    return "(" + _first_sibbling->Serialize() + " "
      + TokenString(_node_type) + " " + _second_sibbling->Serialize() + ")";
  } else if (_node_type == kTokenOperatorConditionalIf) {
    assert(_first_sibbling != NULL);
    assert(_second_sibbling != NULL);
    assert(_third_sibbling != NULL);  
    return "(" + _first_sibbling->Serialize() + " ? "
      + _second_sibbling->Serialize() + " : "
      + _third_sibbling->Serialize() + ")";
  } else if (IsOpenEndedWhenCondition()) {
    return "WHEN: " + _first_sibbling->Serialize() + "\n"
      + (_second_sibbling? _second_sibbling->Serialize(): "")
      // No third sibbling to serialize, because this is the next open-ender
      // and TWO pointers point to it (one from a normal "when")
      + "WEND";
  } else if (IsWhenConditionWithAction()) {
    return "    WHEN: " + _first_sibbling->Serialize() + "WRETURN: "
      + (_second_sibbling? _second_sibbling->Serialize(): "") + "\n"
      // Third sibbling: either next when-condition or next open-ended when-condition
      + (_third_sibbling? _third_sibbling->Serialize(): "");
  } else {
    // Unhandled note-type, probably new and therefore not yet handled
   write_log(k_always_log_errors, "[CParseTreeTerminalNodeBetsizeAction] ERROR: Unhandled node-type %i in serialization of parse-tree\n",
      _node_type);
    return "";
  }
}

bool CParseTreeTerminalNodeBetsizeAction::EvaluatesToBinaryNumber() {
  if (TokenEvaluatesToBinaryNumber(_node_type)) {
    // Operation that evaluates to binary number,
    // e.g. bit-shift, logical and, etc.
    return true;
  } else if (TokenIsBracketOpen(_node_type)) {
    // Have a look at the sub-extreesopn
    TPParseTreeNode sub_expression = _first_sibbling;
    // There has to be an expresison inside the brackets
    assert (_first_sibbling != NULL);
    return _first_sibbling->EvaluatesToBinaryNumber();
  }
  // Nothing binary
  return false;
}
