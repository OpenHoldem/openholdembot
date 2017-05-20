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
#include "CParseTreeTerminalNodeEndOfFunction.h"

#include "CFormulaParser.h"
#include "CFunctionCollection.h"
#include "COHScriptObject.h"
#include "CPreferences.h"

CParseTreeTerminalNodeEndOfFunction::CParseTreeTerminalNodeEndOfFunction(int relative_line_number) : 
  // Increment line number by 1 
  // to point to the first empty line after the function code
  // to avoid confusion and emphasize that we reached the end.
  CParseTreeTerminalNodeIdentifier((relative_line_number + 1), "") {
  assert(p_formula_parser != NULL);
  CString currenttly_parsed_function = p_formula_parser->CurrentFunctionName();
  write_log(preferences.debug_parser(), "[CParseTreeTerminalNodeEndOfFunction] %s\n",
    currenttly_parsed_function);
  if (COHScriptObject::IsMainOpenPPLFunction(currenttly_parsed_function)) {
    assert(currenttly_parsed_function.Left(2) == "f$");
    // Build new Name: "Default" + old name without "f$" 
    // plus uppercase for the first character of the 2nd part,
    // e.g. f$preflop -> DefaultPreflop
    CString default_function = "Default" + currenttly_parsed_function.Mid(2);
    default_function.SetAt(7, toupper(default_function.GetAt(7)));
    write_log(preferences.debug_parser(), "[CParseTreeTerminalNodeEndOfFunction] %s\n",
      default_function);
    if (p_function_collection->Exists(default_function)) {
      _terminal_name = default_function;
      write_log(preferences.debug_parser(), 
        "[CParseTreeTerminalNodeEndOfFunction] Default bot-logic referenced.\n");
      return;
    }
  }
  // Not a main OPPL-function or fallback-logic does not exist
  write_log(preferences.debug_parser(),
    "[CParseTreeTerminalNodeEndOfFunction] Falling back to %s\n",
    kEmptyExpression_False_Zero_WhenOthersFoldForce);
  _terminal_name = kEmptyExpression_False_Zero_WhenOthersFoldForce;
}

CParseTreeTerminalNodeEndOfFunction::~CParseTreeTerminalNodeEndOfFunction() {
}
