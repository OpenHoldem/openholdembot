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
#include "CFunction.h"

#include "CAutoplayer.h"
#include "CAutoplayerTrace.h"
#include "CFormulaParser.h"
#include "CMemoryPool.h"
#include "COHScriptObject.h"
#include "CParseTreeNode.h"

#include "..\DLLs\WindowFunctions_DLL\window_functions.h"

// Global counter to detect potentially endless recursion_depth
int recursion_depth = 0;
const int kMaxRecursionDepth = 100;

CFunction dummy_function("undefined (probably debug-tab)", "", 
  kNoSourceFileForThisCode, kUndefinedZero);
CFunction* CFunction::_currently_evaluated_function = &dummy_function;

CFunction::CFunction(
    CString new_name,
    CString new_function_text) : COHScriptObject(new_name, new_function_text, kNoSourceFileForThisCode, kUndefinedZero) {
  _is_result_cached = false;
  _cached_result = kUndefinedZero;
  _parse_tree_node = NULL;
  _imported_from_shanky_ppl = false;
}

CFunction::CFunction(
    CString new_name, 
    CString new_function_text,
    CString file_path,
    int absolute_line) : COHScriptObject(new_name, new_function_text, file_path, absolute_line) {
  _is_result_cached = false;
  _cached_result = kUndefinedZero;
  _parse_tree_node = NULL;
  _imported_from_shanky_ppl = false;
}

CFunction::~CFunction() {
  // No longer deleting any parse-trees here,
  // as parse-trees get handled by CMemoryPool
}

void CFunction::SetParseTree(TPParseTreeNode _new_parse_tree) {
  // Don't worry about the old parse-tree here.
  // It gets now handled by CMemoryPool.
  // Explicit deletion is no longer possible
  // as it didn't get allocated explicitly either.
	_parse_tree_node = _new_parse_tree;
}

void CFunction::Parse() {
  if (NeedsToBeParsed()) { 
    write_log(Preferences()->debug_formula() || Preferences()->debug_parser(),
      "[CFunction] Parsing %s\n", _name);
    p_formula_parser->ParseFormula(this);
    MarkAsParsed();
  } else {
    write_log(Preferences()->debug_formula() || Preferences()->debug_parser(),
      "[CFunction] No need to parse %s\n", _name);
  }
};

double CFunction::Evaluate(bool log /* = false */) {
  write_log(Preferences()->debug_formula(), 
    "[CFunction] Evaluating function %s\n", _name); 
  // Check recursion depth of DoCalcF$symbol 
  // to detect a recursive formula.
  // Decrease recursion_depth on every function exit!
  recursion_depth++;
  if (recursion_depth > kMaxRecursionDepth) {
    CString error_message; 
    error_message.Format("Recursion to deep.\n"
      "Probably endless.\n"
      "Stopping autoplayer.\n"
      "\n"
      "Last function: %s", _name);
	  MessageBox_Error_Warning(error_message);
	  p_autoplayer->EngageAutoplayer(false);
	  ++recursion_depth;
	  return kUndefinedZero;
  }
  // Keep track of the currently evaluated function
  // to be able generate better error-messages in other modules
  // (CParseTreeOperatorNode, division by zero)
  _previously_evaluated_function = _currently_evaluated_function;
  _currently_evaluated_function = this;
  // Result already cached
  if (_is_result_cached) {
    if (log) {
      write_log(Preferences()->debug_formula(),
        "[CFunction] %s -> %.3f [cached]\n", _name, _cached_result);
      p_autoplayer_trace->Add(_name, _cached_result);  
    }
    // Keep cached result: do nothing
  } else {
    // Evaluate symbol
    if (_parse_tree_node != NULL) {
      int log_line;
      if (log) {
        // Reserve a line in the log, without result ATM
        log_line = p_autoplayer_trace->Add(_name);
      }
      p_autoplayer_trace->Indent(true);
      _cached_result = _parse_tree_node->Evaluate(log);
      _is_result_cached = true;
      if (log) {
        p_autoplayer_trace->BackPatchValueAndLine(
          log_line, _cached_result, _function_text, _starting_line_of_function, _file_path);
      }
      p_autoplayer_trace->Indent(false);
    } else {
      // Undefined, parse-tree-node is NULL
      // keep _cached_result as 0.0
      if (log) {
        p_autoplayer_trace->Add(_name, kUndefinedZero, true);
      }
    }
  }
  --recursion_depth;
  _currently_evaluated_function = _previously_evaluated_function;
  return _cached_result;
}

bool CFunction::EvaluatesToBinaryNumber() {
  if (_parse_tree_node == NULL) return false;
  return _parse_tree_node->EvaluatesToBinaryNumber();
}

void CFunction::ClearCache() {
	_cached_result = 0.0;
	_is_result_cached = false;
}

CString CFunction::Serialize() {
  CString result;
  if (_parse_tree_node != NULL) {
    result = _parse_tree_node->Serialize();
  }
  return result;
}

void CFunction::Dump() {
  CString data;
  data.Format("%s %s %.3f %i\n",
    _name,
    (_is_result_cached ? "[cached]" : "[not chached]"),
    _cached_result,
    _parse_tree_node);
  write_log(Preferences()->debug_formula() , (char*)(LPCTSTR)data);
}

void CFunction::SetValue(double value) {
  _cached_result = value;
  _is_result_cached = true;
}