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
#include "CDebugTab.h"

#include "CFormulaParser.h"
#include "CEngineContainer.h"


CDebugTab *p_debug_tab = NULL; 

// These arrays are global, 
// because -- when private members --
// all entries of the 2nd array (except the first element)
// magically disappeared after function_text()
// and before EvaluateAll.
// No matter what order, 
// no matter if CStrings or CString* or char*,
// no matter if CArray or normal arrays.
// But now it works. ;-)
CString _expression_texts[kMaxSizeOfDebugTab];
TPParseTreeNode _expressions[kMaxSizeOfDebugTab];

CDebugTab::CDebugTab() : COHScriptObject("f$debug", "", kNoSourceFileForThisCode, kUndefinedZero) {
  // Name is alwayss the same and required for function lookup
  _name = CString("f$debug");
  _number_of_expressions = 0;
  Clear();
}

CDebugTab::~CDebugTab() {
  Clear();
}

void CDebugTab::Parse() {
  p_formula_parser->ParseDebugTab(_function_text);
}

CString CDebugTab::EvaluateAll() {
  CString all_results;
  CString current_line;
  for (int i=0; i<_number_of_expressions; ++i) {
	CString result;
	if (p_engine_container->IsStringSymbol(_expression_texts[i])) {
		result = _expressions[i]->EvaluateString();
		assert(_expression_texts[i] != "");
		// Left side is complete string value
		current_line.Format("%s = %s\n", result,
			_expression_texts[i]);
	}
	else {
		result = _expressions[i]->EvaluateToString();
		assert(_expression_texts[i] != "");
		// Left side at least 12 digits for bitwise values like 0b1011100101
		current_line.Format("%12s = %s\n", result,
			_expression_texts[i]);
	}
    all_results += current_line;
  }
  return all_results;
}

void CDebugTab::Clear() {
  write_log(Preferences()->debug_alltherest(), "[CDebugTab] Clear()\n");
  assert(_number_of_expressions >= 0);
  assert(_number_of_expressions <= kMaxSizeOfDebugTab);
  write_log(Preferences()->debug_alltherest(), "[CDebugTab] Going to delete expressions\n");
  for (int i=0; i<kMaxSizeOfDebugTab; ++i) {
    _expression_texts[i] = "";
    // No longer deleting any parse-trees here,
    // as parse-trees get handled by CMemoryPool
    _expressions[i] = NULL;
  }
  write_log(Preferences()->debug_alltherest(), "[CDebugTab] Expressions deleted\n");
  _number_of_expressions = 0;
  write_log(Preferences()->debug_alltherest(), "[CDebugTab] Done\n");
}

void CDebugTab::AddExpression(CString expression_text, TPParseTreeNode expression) {
  CString text = expression_text.TrimLeft().TrimRight();
  if (text == "") return;
  if (expression == NULL) return;
  if (_number_of_expressions >= kMaxSizeOfDebugTab) {
    bool too_many_expressions_for_debug_tab = false;
    assert(too_many_expressions_for_debug_tab);
    return;
  }
  _expression_texts[_number_of_expressions] = CString(text); 
  _expressions[_number_of_expressions] = expression;
  ++_number_of_expressions;
}

void* CDebugTab::operator new(size_t size) {
  assert(PMemoryPoolParser() != NULL);
  write_log(Preferences()->debug_memory_usage(), "[CDebugTab] Allocating %i bytes\n", size);
  return p_memory_pool_global->Allocate(size);
}

CString CDebugTab::function_text() {
  // It seems we need the current _function_text for parsing,
  // if changed in the editor, so we can't construct a new 
  // clean one from the old parsed data.
  return _function_text; 
}