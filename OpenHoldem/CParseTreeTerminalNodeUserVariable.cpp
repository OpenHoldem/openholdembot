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
#include "CParseTreeTerminalNodeUserVariable.h"

#include <math.h>
#include "CAutoplayerTrace.h"
#include "CEngineContainer.h"
#include "CFunctionCollection.h"
#include "CMemoryPool.h"
#include "CParserSymbolTable.h"

#include "CSymbolEngineChipAmounts.h"
#include "CSymbolEngineMemorySymbols.h"
#include "CSymbolEngineOpenPPLUserVariables.h"
#include "CSymbolEngineVersus.h"
#include "CIteratorThread.h"
#include "CSymbolEnginePrwin.h"
#include "CSymbolEngineRange.h"
#include "CSymbolengineMultiplexer.h"
#include "CSymbolEngineTableLimits.h"
#include "FloatingPoint_Comparisions.h"
#include "CFormulaParser.h"
#include "CParseErrors.h"

#include "..\DLLs\WindowFunctions_DLL\window_functions.h"
#include "..\DLLs\StringFunctions_DLL\string_functions.h"
#include "TokenizerConstants.h"

CParseTreeTerminalNodeUserVariable::CParseTreeTerminalNodeUserVariable(
  int relative_line_number, CString name) : 
  CParseTreeTerminalNodeIdentifier(relative_line_number, 
    name) {
   _node_type = kTokenActionUserVariableToBeSet;
}

CParseTreeTerminalNodeUserVariable::~CParseTreeTerminalNodeUserVariable() {
}

double CParseTreeTerminalNodeUserVariable::Evaluate(bool log /* = false */){
  CString name = _terminal_name;
  write_log(Preferences()->debug_formula(), 
    "[CParseTreeTerminalNode] Evaluating node type %i %s\n", 
		_node_type, name);
  p_autoplayer_trace->SetLastEvaluatedRelativeLineNumber(_relative_line_number);
  if (name.Left(4).MakeLower() == "user") {
    p_engine_container->symbol_engine_openppl_user_variables()->Set(name);
    return true;
  }
  // Covering both me_st_ and me_inc_ here.
  // me_re_ is a normal identifier, as it
  // doesn't require a continuation at the next condition.
  // Also me_add_ amd me_sub_ since openHoldem 11.1.0.
  if (name.Left(3) == "me_") {
	  double temp_result;
	  p_engine_container->symbol_engine_memory_symbols()->EvaluateSymbol(name,
		  &temp_result, true);
	  return temp_result;
  }
  //Retrieve the set string part for SET action with variable string
  int position = name.Find('"', 0);
  CString set_string = "";
  if (position != -1) {
	  set_string = name.Mid(position);
  }
  set_string.Remove('"');
  //Retrieve the set symbol part for SET action with variable string
  position = name.Find(' ', 0);
  if (position != -1) {
	  name = name.Left(position);
  }
  if (name.Left(11) == "prw1326_use") {
  	  if (set_string.MakeLower() != "true" && set_string.MakeLower() != "false") {
		  CString error_message = "Invalid SET prw1326 use command\n"
			  "Only \"TRUE\" or \"FALSE\" are allowed.";
		  CParseErrors::Error(error_message);
		  return false;
	  }
	  p_engine_container->symbol_engine_prwin()->SetPrw1326(name, set_string);
	  return true;
  }
  if (name.Left(11) == "prw1326_cmd") {
	  if (set_string.MakeLower() != "recalc" && set_string.MakeLower() != "clearall") {
		  CString error_message = "Invalid SET prw1326 command\n"
			  "Only \"RECALC\" or \"CLEARALL\" are allowed.";
		  CParseErrors::Error(error_message);
		  return false;
	  }
	  p_engine_container->symbol_engine_prwin()->SetPrw1326(name, set_string);
	  if (set_string == "recalc") {
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
	  }
	  if (set_string == "clearall") {
		 p_engine_container->symbol_engine_range()->ClearAll();
		 p_engine_container->symbol_engine_range()->UnSetLists();
	  }
	  return true;
  }
  if (name.Left(5) == "range") {
	  name = p_engine_container->symbol_engine_multiplexer()->MultiplexedSymbolName(name);
	  if (!p_engine_container->symbol_engine_multiplexer()->IsValidPostfix())
		  return false;
	  p_engine_container->symbol_engine_range()->SetRange(name, set_string);
	  return true;
  }
  assert(kThisMustNotHappen);
  return false;
}

CString CParseTreeTerminalNodeUserVariable::EvaluateToString(bool log /* = false */) {
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

CString CParseTreeTerminalNodeUserVariable::Serialize() {
  if (_node_type == kTokenIdentifier) {
    return _terminal_name;
  } else {
    // Unhandled note-type, probably new and therefore not yet handled
   write_log(k_always_log_errors, "[CParseTreeTerminalNode] ERROR: Unhandled node-type %i in serialization of parse-tree\n",
      _node_type);
    return "";
  }
}

