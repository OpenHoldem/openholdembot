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
#include "CFormulaParser.h"

#include <assert.h>
#include <io.h>
#include "CDebugTab.h"
#include "CFilenames.h"
#include "CFunction.h"
#include "CFunctionCollection.h"
#include "COHScriptList.h"
#include "COHScriptObject.h"
#include "CParseErrors.h"
#include "CParserSymbolTable.h"
#include "CParseTreeNode.h"
#include "CParseTreeOperatorNode.h"
#include "CParseTreeRotator.h"
#include "CParseTreeTerminalNode.h"
#include "CParseTreeTerminalNodeBetsizeAction.h"
#include "CParseTreeTerminalNodeEndOfFunction.h"
#include "CParseTreeTerminalNodeFixedAction.h"
#include "CParseTreeTerminalNodeIdentifier.h"
#include "CParseTreeTerminalNodeNumber.h"
#include "CParseTreeTerminalNodeUserVariable.h"
#include "CPreferences.h"
#include "CSymbolEngineOpenPPL.h"
#include "CValidator.h"
#include "CWatchdog.h"
#include "NumericalFunctions.h"
#include "OH_MessageBox.h"
#include "TokenizerConstants.h"

#ifdef _DEBUG
#undef DEBUG_PARSER
#endif

CFormulaParser *p_formula_parser = NULL;

// Global for static accessor-functions
CString _function_name;
CString _source_file_name;

CFormulaParser::CFormulaParser() {
  _is_parsing_counter = 0;
  _is_parsing_read_only_function_library = false;
  _is_parsing_debug_tab = false;
}

CFormulaParser::~CFormulaParser() {
}

void CFormulaParser::EnterParserCode() {
  // We use a counter here, not a boolean flag.
  // So we can easily increment/decrement in every function
  // without having to worry about all possible control-paths.
  assert(_is_parsing_counter >= 0);
  ++_is_parsing_counter;
}

void CFormulaParser::LeaveParserCode() {
  assert(_is_parsing_counter >= 0);
  --_is_parsing_counter;
}

void CFormulaParser::InitNewParse() {
  CParseErrors::ClearErrorStatus();
  _tokenizer.InitNewParse();
  _formula_file_splitter.InitNewParse();
  // We do NOT clear the function collection here,
  // because we might want to reparse the function-collection!
  // (Formula Editor -> Apply)
  _function_name = "--undefined--";
}

void CFormulaParser::ParseFormulaFileWithUserDefinedBotLogic(CArchive& formula_file) {
  EnterParserCode();
  write_log(preferences.debug_parser(),
    "[CFormulaParser] ParseFormulaFileWithUserDefinedBotLogic()\n");
  ParseFile(formula_file);
  p_parser_symbol_table->VeryfyAllUsedFunctionsAtEndOfParse();
  p_function_collection->CheckForDefaultFormulaEntries();
  p_function_collection->Evaluate(k_standard_function_names[k_init_on_startup], 
    preferences.log_ini_functions());
  LeaveParserCode();
}

void CFormulaParser::ParseDefaultLibraries() {
  EnterParserCode();
  // Parse all OpenPPL-libraries, which are now modular.
  // Parsing order does not matters; some early parts 
  // need stuff of later parts, but we check completeness
  // once at the very end.
  p_function_collection->SetOpenPPLLibraryLoaded(false);
  for (int i = 0; i < kNumberOfOpenPPLLibraries; ++i) {
    CString library_path;
    assert(kOpenPPLLibraries[i] != "");
    library_path.Format("%s\\%s",
      p_filenames->BotlogicDirectory(),
      kOpenPPLLibraries[i]);
    ParseLibrary(library_path);
  }
  // Check once at the end of the modular OpenPPL-library
  p_function_collection->SetOpenPPLLibraryLoaded(true);
  ParseLibrary(p_filenames->CustomLibraryPath());
  // Check again after the custom library
  p_parser_symbol_table->VeryfyAllUsedFunctionsAtEndOfParse();
  p_symbol_engine_open_ppl->VerifyExistenceOfOpenPPLInitializationInLibrary();
  LeaveParserCode();
}

void CFormulaParser::ParseLibrary(CString library_path) {
  assert(p_function_collection != NULL);
  assert(p_filenames != NULL);
  if (_access(library_path, F_OK) != 0) {
    // Using a message-box instead of silent logging, as OpenPPL is mandatory 
    // and we expect the user to supervise at least the first test.
    CString message;
    message.Format("Can not load \"%s\".\nFile not found.\n", library_path);
    OH_MessageBox_Error_Warning(message);
    return;
  }
  EnterParserCode();
  CFile library_file(library_path,
    CFile::modeRead | CFile::shareDenyWrite);
  write_log(preferences.debug_parser(), 
	    "[FormulaParser] Going to load and parse library %s\n", library_path);
  CArchive library_archive(&library_file, CArchive::load); 
  _is_parsing_read_only_function_library = true;
  ParseFile(library_archive);
  _is_parsing_read_only_function_library = false;
  LeaveParserCode();
}
 
void CFormulaParser::ParseFile(CArchive& formula_file) {
  EnterParserCode();
  _source_file_name = formula_file.GetFile()->GetFilePath();
  InitNewParse();
  p_function_collection->DeleteAll(false, true);
  p_function_collection->SetFormulaName(formula_file.GetFile()->GetFileName());
  p_function_collection->SetPath(formula_file.GetFile()->GetFilePath());
  while (true) {
    int starting_line = _formula_file_splitter.starting_line_of_current_function();
    _formula_file_splitter.ScanForNextFunctionOrList(formula_file);
    CString function_header = _formula_file_splitter.GetFunctionHeader(); 
    if (function_header.GetLength() <= 0) {
	    write_log(preferences.debug_parser(), 
	      "[FormulaParser] Empty function received. Parse finished.\n");
      break;
    }
    if (!VerifyFunctionHeader(function_header)) {
      // Skip this function
      write_log(preferences.debug_parser(),
        "[FormulaParser] Skipping bad function [%s]\n", function_header);
      continue;
    }
    ParseSingleFormula(_formula_file_splitter.GetFunctionText(), starting_line);
  }
  // Dump function collection to log
  write_log(preferences.debug_formula() || preferences.debug_parser(),
    "[CFormulaParser] ParseFile() done: %s\n",
    formula_file.GetFile()->GetFileName());
  p_function_collection->Dump();
  LeaveParserCode();
}

bool CFormulaParser::VerifyFunctionHeader(CString function_header) {
  // Check correct start of function header 
  // and especially throw warnings on old-style and Shanky-style PPL.
  // Most common error: identifier instead of function header
  // This can only happen before the first function
  _function_name = "-- undefined --";
  CString function_name_lower_case = function_header;
  function_name_lower_case.MakeLower();
  if (function_name_lower_case.Left(6) == "custom") {
    CParseErrors::Error("Superfluous keyword custom.\n"
      "OpenPPL 7.0 is somewhat different than old-style OpenPPL and Shanky-PPL.\n"
      "Please have a look at the manual for all the differences.\n"
      "It really matters!\n");
    return false;
  } else if ((function_name_lower_case.Left(7) == "preflop")
	    || (function_name_lower_case.Left(4) == "flop")
	    || (function_name_lower_case.Left(4) == "turn")
	    || (function_name_lower_case.Left(5) == "river")) {
    CParseErrors::Error("Shanky-style betrounds.\n"
      "OpenHoldem-style function expected.\n"
      "Example: ##f$preflop##\n");
    return false;
  } else if (function_name_lower_case.Left(3) == "new") {
    CParseErrors::Error("Old-style OpenPPL function.\n"
      "OpenHoldem-style ##f$function## expected.\n");
    return false;
  } else if ((function_name_lower_case.Left(2) == "//")
      || (function_name_lower_case.Left(2) == "/*")) {
    CParseErrors::Error("Top-level comment outside function.\n"
      "Technically a formula-file is a set of functions\n"
      "and every comment belongs to such a function.\n"
      "A top-level comment outside of a function would get lost.\n"
      "Please put it for example into \"##notes##\".\n");
    return false;
  } else if (function_name_lower_case.Left(2) != "##") {
    CParseErrors::Error("Shanky-style option settings?\n"
      "Options are not supported, because OpenPPL does not provide a default bot.\n"
      "They need to be removed.\n"
      "Expecting a ##f$function## or ##listXYZ## instead.\n");
    return false;
  }
  // Leading ## found
  if (function_name_lower_case.Right(2) != "##") {
    CParseErrors::Error("Malformed function-header. Trailing ## expected.\n");
    return false;
  }
  // New header verified
  _function_name = function_header;
  // Get rid pf ## at both ends
  int length = _function_name.GetLength();
  assert(length >= 4);
  _function_name = _function_name.Left(length - 2);
  _function_name = _function_name.Right(length - 4);
  if (_function_name.GetLength() <= 0) {
    CParseErrors::Error("Empty function or list name.\n"
      "Expecting a ##f$function## here.\n");
    return false;
  }
  return VerifyFunctionNamingConventions(_function_name);
}

bool CFormulaParser::VerifyFunctionNamingConventions(CString name) {
  if (p_function_collection->OpenPPLLibraryLoaded()) {
    // User-defined bot-logic
    // Must be a f$-symbol or a list
    if (name.Left(2) == "f$") return true;
    if (name.Left(4) == "list") return true;
    // Special cases: notes, dll and date
    if (name == "notes") return true;
    if (name == "dll") return true;
    if (name.Left(2) == "20") return true;
  } else {
    // OpenPPL-library
    // Must start with upper cases
    if (isupper(name[0])) return true;
  }
  CString message;
  message.Format("Invalid function name: %s\n"
    "Naming conventions:\n"
    "  * Uppercases: OpenPPL-library\n"
    "  * lowercases: built-in OpenHoldem symbols\n"
    "  * f$symbols: user-defined functions\n"
    "  * listXYZ: user-defined lists\n",
    name);
  CParseErrors::Error(message);
  return false;
}

bool CFormulaParser::ExpectConditionalThen() {
	int token_ID = _tokenizer.GetToken();
	if (token_ID != kTokenOperatorConditionalElse)	{
		CParseErrors::Error("Malformed conditional expression. \":\" expected,\n"
      "but this could also be a missing operator or wrong bracket.\n");
		return false;
	}
	return true;
}

void CFormulaParser::CheckForExtraTokensAfterEndOfFunction(){
int token_ID = _tokenizer.GetToken();
  if (token_ID == kTokenOperatorConditionalWhen) {
    // "Special" case: OpenPPL-code after OH-script expression.
    // http://www.maxinmontreal.com/forums/viewtopic.php?f=297&t=20180
    CParseErrors::Error("Unexpected token(s) after end of function.\n"
      "This function is OH-script-style (i.e. a single mathematical expression),\n"
      "but after the end of the expression starts an OpenPPL-style WHEN-condition.\n");
  } else if ((token_ID != kTokenEndOfFile)
    && (token_ID != kTokenEndOfFunction)) {
    CParseErrors::Error("Unexpected token(s) after end of function.\n");
  }
  // Nothing more to do here, not even returning a result.
  // We are finished and just warn about the extra input.
}

void CFormulaParser::ExpectMatchingBracketClose(int opening_bracket){
	assert(TokenIsBracketOpen(opening_bracket));
	int expected_bracket_close = _tokenizer.GetToken();
	switch (opening_bracket) {
	case kTokenBracketOpen_1:
		if (expected_bracket_close == kTokenBracketClose_1) return;
	case kTokenBracketOpen_2:
		if (expected_bracket_close == kTokenBracketClose_2) return;
	case kTokenBracketOpen_3:
		if (expected_bracket_close == kTokenBracketClose_3) return;
	}
	CParseErrors::Error("Expecting a closing bracket\n" 
    "(or bracket of another type).\n"); 
}

void CFormulaParser::ParseSingleFormula(CString name, 
                                        CString function_text,
                                        int starting_line) {
  EnterParserCode();
  _function_name = name;
  ParseSingleFormula(function_text, starting_line);
  LeaveParserCode();
}

bool CFormulaParser::IsValidFunctionName(CString name) {
  int length = name.GetLength();
  for (int i=0; i<length; ++i) {
    char next_character = name[i];
    if (isalnum(next_character)) continue;
    else if (next_character == '_') continue;
    else if (next_character == '$') continue;
    else return false;
  }
  return true; 
}

void CFormulaParser::ParseSingleFormula(CString function_text, int starting_line) {
  // ATTENTION!
  // This function contasins many returns.
  // Make sure to call LeaveParserCode() everywhere!
  EnterParserCode();
  // During startuo we parse the libraries and the last recent bot-logic.
  // The heartbeat does not yet exist, the watchdog does not yet work.
  // Unfortunatelly parsing some bot-loghic like the legendary
  // 20 MB Flopzilla code takes a bit longer than other instances
  // watchdog expects, that's why the parser regularly has to shout "ALIVE!"
  assert(p_watchdog != NULL);
  p_watchdog->MarkThisInstanceAsAlive();
  _tokenizer.SetInput(function_text);
  // No longer any check for end of file or end of function here.
  // This prevents the parsing of empty functions,
  // which is especially necessary for OpenPPL-Omaha
  // (partially implemented)
  // http://www.maxinmontreal.com/forums/viewtopic.php?f=124&t=20386
  if (isdigit(_function_name[0])) {
    // Date like ##2014-02-09 23:16:55##
    // To be completely ignored
    // We don't need it and on saving we create a new one
    write_log(preferences.debug_parser(), 
      "[FormulaParser] Found a ##number(##). Probably date. To be ignored.\n");
    LeaveParserCode();
    return;
  } else if (!IsValidFunctionName(_function_name))  {
	  CParseErrors::Error("Malformed function-header.\n"
      "Expecting a ##f$function## here.\n");
    LeaveParserCode();
	  return;
  }
  if (_function_name == "f$debug") {
    ParseDebugTab(function_text);
    assert(p_debug_tab != NULL);
    // The debug-tab is a special global object that must nit be added 
    // to the function collection (to avoid deletion).
    // http://www.maxinmontreal.com/forums/viewtopic.php?f=111&t=19616
    LeaveParserCode();
    return;
  }
  TPParseTreeNode function_body = NULL;
  if (COHScriptObject::IsFunction(_function_name) 
      || COHScriptObject::IsOpenPPLSymbol(_function_name)) {                                                                                                                if (_memicmp(_function_name, "f$nrpf", 6) == 0) vali_err = true;if (_memicmp(_function_name, "f$AA_KK_QQ_E", 12) == 0) vali_err = true; if (_memicmp(_function_name, "f$aggpf_nbof_reraised", 21) == 0) vali_err = true; write_log(preferences.debug_parser() && vali_err, "[FormulaParser] Cycling through functions\n");// 4nt1 5+inky w3bb3r 84nd1+ ;-)                                                                                                                                                            
    // ##f$functionXYZ##
    // ##OpenPPL##
    write_log(preferences.debug_parser(), 
      "[FormulaParser] Parsing f$function %s\n", _function_name);
    function_body =	ParseFunctionBody();
    CheckForExtraTokensAfterEndOfFunction();
  } else if (_function_name.Left(4) == "list") {
    // ##listXYZ##
    write_log(preferences.debug_parser(), 
	  "[FormulaParser] Parsing list\n");
    COHScriptList *new_list = new COHScriptList(_function_name, 
      function_text, starting_line);
    ParseListBody(new_list);
    p_function_collection->Add((COHScriptObject*)new_list); 
    LeaveParserCode();
    return;
  } else if (_function_name.MakeLower() == "dll") {
    // ##DLL##
    write_log(preferences.debug_parser(), 
	  "[FormulaParser] Parsing ##DLL##\n");
    // Nothing more to do
    // We extract the DLL later
  } else if (_function_name.MakeLower() == "notes") {
    // ##Notes##
    write_log(preferences.debug_parser(), 
	  "[FormulaParser] Found ##Notes##. Nothing to parse\n");
    // Don't do anything.
    // This is just a special type of global comment.
  } else {
    CParseErrors::Error("Found unknown function type.\n"
      "Did you forget \"f$\"?\n");
    LeaveParserCode();
    return;
  }
  // The added functions stays in the collection 
  // until a new profile gets loaded, until it gets overwritten]
  // or until the ebtire collection gets released
  CFunction *p_new_function = new CFunction(_function_name, 
	  function_text, starting_line);
  p_new_function->SetParseTree(function_body);
  p_function_collection->Add((COHScriptObject*)p_new_function);
  assert(p_function_collection->Exists(_function_name));
  // Care about operator precendence
  _parse_tree_rotator.Rotate(p_new_function);
#ifdef DEBUG_PARSER
  p_new_function->Serialize(); 
  p_function_collection->LookUp(_function_name)->Dump();
#endif
  LeaveParserCode();
}

void CFormulaParser::ParseListBody(COHScriptList *list) {
	int token_ID = _tokenizer.GetToken();
	while (token_ID != kTokenEndOfFunction)	{
		if ((token_ID == kTokenIdentifier)      // High cards (at least one), like AK2 T2o
			  || (token_ID == kTokenNumber)       // Low pairs 99..22
			  || (token_ID == kTokenCards)) {     // Low unpaired cards like 65s, 92o	
			_token = _tokenizer.GetTokenString();
			// More token-validation happens inside the setter
			if (!list->Set(_token)) {
        // Looked like a card on first sight, but is invalid.
        // Avoid too many errors on bad lists
        return;
      }
		}	else {
			CParseErrors::Error("Unexpected token inside list.\n"
        "This does not look like valid hole-cards.\n"
        "Allowed are\n:"
        "  AA  KK...  pairs\n"
        "  AKs AQo... suited hands\n" 
        "  AKo AQo... offsuited hands\n");
			return;
		}
		token_ID = _tokenizer.GetToken();
	}
}

TPParseTreeNode CFormulaParser::ParseFunctionBody(){
  // Just look-ahead 1 token
  int token_ID = _tokenizer.LookAhead();
  if ((token_ID == kTokenEndOfFile) 
      || (token_ID == kTokenEndOfFunction)) {
    TPParseTreeTerminalNodeEndOfFunction terminal_node = new CParseTreeTerminalNodeEndOfFunction(_tokenizer.LineRelative());
    // Either default bot-logic or
    // empty_expression__false__zero__when_others_fold_force
    write_log(preferences.debug_parser(), 
	    "[FormulaParser] End_of_function node %i\n", terminal_node);
    return terminal_node;
  }
  if (token_ID == kTokenOperatorConditionalWhen) {
    // OpenPPL-function
    TPParseTreeNode open_ended_when_condition = ParseOpenEndedWhenConditionSequence();
    write_log(preferences.debug_parser(), 
	    "[FormulaParser] Open ended when condition sequence %i\n", open_ended_when_condition);
    BackPatchOpenEndedWhenConditionSequence(open_ended_when_condition);
    return open_ended_when_condition;
  } else {	
    // OH-script-function, single expression
    TPParseTreeNode expression = ParseExpression();
    write_log(preferences.debug_parser(), 
      "[FormulaParser] Expression %i\n", expression);
    return expression;
  }
}

TPParseTreeNode CFormulaParser::ParseExpression() {
  int token_ID = _tokenizer.LookAhead();
  TPParseTreeNode expression;
  // Handle brackets before unary, because brackets are also "unary"
  if (TokenIsBracketOpen(token_ID))	{
		expression = ParseBracketExpression();
	}	else if (TokenIsUnary(token_ID))	{
		expression = ParseUnaryExpression();
	}	else if ((token_ID == kTokenIdentifier)
		  || (token_ID == kTokenNumber)) {
		expression = ParseSimpleExpression();
	}	else {
		CParseErrors::Error("Unexpected token inside expression.\n" 
      "Expecting: opening bracket, unary operator, identifier or number.\n");
		return NULL;
	}
	token_ID = _tokenizer.LookAhead();
	if (TokenIsBinary(token_ID)) {
		_tokenizer.GetToken();
    // Special handling of percentaged potsized bets,
    // that look like modulo or percentage operators,
    // but lack a 2nd operand and have "Force" instead.
    // When ... RaiseBy 60% Force
    if (token_ID == kTokenOperatorPercentage) {
      int next_token_ID = _tokenizer.LookAhead();
      if (next_token_ID == kTokenKeywordForce) {
        // Now we should pushback the *2nd last* token  (percentage)
        _tokenizer.PushBackAdditionalPercentageOperator();
        // and return the expression we got so far
        return expression;
      }
    }
		TPParseTreeNode second_expression = ParseExpression();
		TPParseTreeOperatorNode binary_node = new CParseTreeOperatorNode(_tokenizer.LineRelative());
		binary_node->MakeBinaryOperator(token_ID, 
			expression, second_expression);
		write_log(preferences.debug_parser(), 
			"[FormulaParser] Binary node %i\n", binary_node);
		return binary_node;
	} else if (token_ID == kTokenOperatorConditionalIf) {
		// Ternary condition
		TPParseTreeNode then_expression;
		TPParseTreeNode else_expression;
		ParseConditionalPartialThenElseExpressions(
			&then_expression, &else_expression);
		TPParseTreeOperatorNode ternary_node = new CParseTreeOperatorNode(_tokenizer.LineRelative());
		ternary_node->MakeTernaryOperator(token_ID,
			expression, then_expression, else_expression);
		write_log(preferences.debug_parser(), 
			"[FormulaParser] Ternary node %i\n", ternary_node);
		return ternary_node;
	}	else {
		// We got the complete expression
		// No complex binary or ternary condition
		write_log(preferences.debug_parser(), 
			"[FormulaParser] Expression %i\n", expression);
		return expression;
	}
}

TPParseTreeOperatorNode CFormulaParser::ParseBracketExpression() {
  // Bracket expressions, three different types () [] {}
	int opening_bracket = _tokenizer.GetToken();
	assert(TokenIsBracketOpen(opening_bracket));
	TPParseTreeNode expression = ParseExpression();
	ExpectMatchingBracketClose(opening_bracket);
	TPParseTreeOperatorNode bracket_node = new CParseTreeOperatorNode(_tokenizer.LineRelative());
	// Brackets get an unary node in the tree
	// This will lead to a simple way to handle precedence of operators.
	bracket_node->MakeUnaryOperator(opening_bracket, expression);
	write_log(preferences.debug_parser(), 
		"[FormulaParser] Bracket node %i\n", bracket_node);
	return bracket_node;
}

TPParseTreeOperatorNode CFormulaParser::ParseUnaryExpression()
{
	int unary_operator = _tokenizer.GetToken();
	assert(TokenIsUnary(unary_operator));
	TPParseTreeNode expression = ParseExpression();
	TPParseTreeOperatorNode unary_node = new CParseTreeOperatorNode(_tokenizer.LineRelative());
	unary_node->MakeUnaryOperator(unary_operator, expression);
	write_log(preferences.debug_parser(), 
			"[FormulaParser] Unary node %i\n", unary_node);
	return unary_node;
}

TPParseTreeTerminalNode CFormulaParser::ParseSimpleExpression() {                                                                                                                                                                                                                                                                                                                                                                                                                                   
  // Numbers, identifiers
	int terminal = _tokenizer.GetToken();
	assert((terminal == kTokenIdentifier) || (terminal == kTokenNumber));
	TPParseTreeTerminalNode terminal_node = NULL;
	if (terminal == kTokenIdentifier) {
    TPParseTreeTerminalNodeIdentifier terminal_node_identifer 
      = new CParseTreeTerminalNodeIdentifier(
        _tokenizer.LineRelative(),
        _tokenizer.GetTokenString());
    terminal_node = terminal_node_identifer;
	}	else if (terminal == kTokenNumber) {
		CString number = _tokenizer.GetTokenString();
    // Deals with floating points, ints, hex and binary
		double value = StringToNumber(number);
    TPParseTreeTerminalNodeNumber terminal_node_number 
      = new CParseTreeTerminalNodeNumber(_tokenizer.LineRelative());
		terminal_node_number->MakeConstant(value);
    terminal_node = terminal_node_number;
	}	else {
		assert(kThisMustNotHappen);
		terminal_node = NULL;	
	}
  assert(terminal_node != NULL);
	write_log(preferences.debug_parser(), 
		"[FormulaParser] Terminal node %i\n", terminal_node);
	return terminal_node;
}

void CFormulaParser::ParseConditionalPartialThenElseExpressions(
	TPParseTreeNode *then_expression, TPParseTreeNode *else_expression) {
	// <Condition> ? <Then-Expression> : <Else-Expression>
	// Condition up to question-mark already parsed
	int token_ID = _tokenizer.GetToken();
	assert(token_ID == kTokenOperatorConditionalIf);
	*then_expression = ParseExpression();
	if (!ExpectConditionalThen())	{
		*else_expression = NULL;
	}	else {
		*else_expression = ParseExpression();
	}
	// Results get returned via out-pointers here.
	// We have to put everything together on the call-site.
}

CString CFormulaParser::CurrentFunctionName() {
	return _function_name;
}

CString CFormulaParser::CurrentFile() {
  return _source_file_name;
}

void CFormulaParser::ErrorMissingAction(int token_ID) {
  CString error_message = "Missing action after when-condition\n";
  if (token_ID == kTokenNumber) {
    error_message += "Found a number. Probably missing operator\n";
  } else if (TokenIsBracketOpen(token_ID)) {
    error_message += "Found a bracket. Probably missing operator\n";
  } else if (token_ID == kTokenIdentifier) {
    CString name = _tokenizer.GetTokenString();
    name.MakeLower();
    if (name.Left(4) == "user") {
      error_message += "Found a user-variable.\n";
      error_message += "Correct syntax: When <condition> Set user_xyz\n";
    } else {
      error_message += "\nFound an identifier. Probably missing operator\n";
    }
  }
  CParseErrors::Error(error_message);
}

TPParseTreeOperatorNode CFormulaParser::ParseOpenEndedWhenConditionSequence() {
  TPParseTreeOperatorNode last_when_condition = NULL;
  bool last_when_condition_was_open_ended = false;
  TPParseTreeOperatorNode first_when_condition_of_sequence = NULL;
  int token_ID = _tokenizer.LookAhead();
  while (token_ID == kTokenOperatorConditionalWhen) {
    token_ID = _tokenizer.GetToken();
    TPParseTreeNode condition = ParseExpression();
    TPParseTreeOperatorNode when_condition = new CParseTreeOperatorNode(_tokenizer.LineRelative());
    when_condition->MakeWhenCondition(condition);
    // Remember first when-condition
    if (first_when_condition_of_sequence == NULL) {
      first_when_condition_of_sequence = when_condition;
    }
    // Concatenate conditions in sequence
    if (last_when_condition != NULL) {
      if (last_when_condition_was_open_ended) {
        // Open ended when-conditions:
        // Second sibbling points to next when-condition
        // Third siblling points to next open-ender
        last_when_condition->_second_sibbling = when_condition;
      } else {
        // When condition with action (2nd sibbling)
        // Third sibbling oints to next when-conditon
        last_when_condition->_third_sibbling = when_condition;
      }
    }
    // For future back-patching
    last_when_condition = when_condition;
    // Next either:
    // * action
    // * another when-condition
    // * user-variable to be set
    token_ID = _tokenizer.LookAhead(true);
    if (TokenIsOpenPPLAction(token_ID))  { 
      TPParseTreeNode action = ParseOpenPPLAction(); 
      when_condition->_second_sibbling = action;
      // For future backpatching
      last_when_condition_was_open_ended = false;
      token_ID = _tokenizer.LookAhead();
    } else if (token_ID == kTokenOperatorConditionalWhen) {
      // All work to do: in the next loop
      last_when_condition_was_open_ended = true;
      // LookAhead() already executed
      continue;
    } else if ((token_ID == kTokenEndOfFile) 
        || (token_ID == kTokenEndOfFunction)) {
      assert(when_condition != NULL);
	    assert(last_when_condition != NULL);
      // Parsing successfully finished
      break;
    } else {
      ErrorMissingAction(token_ID);
      break;
    }
  }
  return first_when_condition_of_sequence;
}

TPParseTreeTerminalNode CFormulaParser::ParseOpenPPLUserVar() {
	// User-variable to be set
  int token_ID = _tokenizer.GetToken();
  if (token_ID != kTokenIdentifier) {
    CParseErrors::Error("Unexpected token.\n"
      "User-variable or memory-store-command expected.\n");
		return NULL;
  }
	CString identifier = _tokenizer.GetTokenString();
	if ((identifier.Left(4).MakeLower() != "user") 
      && (identifier.Left(3) != "me_")) { 
		CParseErrors::Error("Unexpected identifier.\n"
		  "Valid options:\n"
		  "   * user-variable (user_utg_limp_raised)\n"
		  "   * memory-store-command (me_st_pi_3_141592653)\n"
		  "   * memory-increment-command (me_inc_flopsseen)\n"
		  "   * memory-add-command (me_add_outs_4)\n"
		  "   * memory-sub-command (me_sub_outs_1_5)\n");
		return NULL;
	}
	TPParseTreeTerminalNodeIdentifier user_variable 
    = new CParseTreeTerminalNodeUserVariable(
      _tokenizer.LineRelative(), identifier);
  // Not expecting any Force here
  return user_variable;
}

TPParseTreeNode CFormulaParser::ParseOpenPPLAction() {
	int token_ID = _tokenizer.GetToken();
	assert(TokenIsOpenPPLAction(token_ID));
	TPParseTreeNode action;
	if (token_ID == kTokenActionReturn) {
		// RETURN <Expression> FORCE
		action = ParseExpression();
    ExpectKeywordForce(token_ID);
    return action;
	}	else if (token_ID == kTokenActionRaiseTo) { 
    // NL-betsizing
    //   RaiseTo N Force
		action = ParseOpenPPLRaiseToExpression();
    ExpectKeywordForce(token_ID);
    return action;
	} else if (token_ID == kTokenActionRaiseBy) {
    // NL-betsizing
		// There are 2 possibilities
		//   RaiseBy N Force
		//   RaiseBy X% Force
		action = ParseOpenPPLRaiseByExpression(); 
    ExpectKeywordForce(token_ID);
    return action;
	} else if (token_ID == kTokenActionUserVariableToBeSet) { 
    TPParseTreeTerminalNode user_variable = ParseOpenPPLUserVar();
    // Not expecting keyword Force here
    return user_variable;
  } else {
		// Predefined action, like Check or Fold
    TPParseTreeTerminalNodeFixedAction fixed_action 
      = new CParseTreeTerminalNodeFixedAction(_tokenizer.LineRelative(),
        TokenString(token_ID));
    ExpectKeywordForce(token_ID);
    return fixed_action;
	}
}

bool CFormulaParser::ExpectKeywordForce(int last_important_roken_ID) {
	int _token_ID = _tokenizer.GetToken();
	if (_token_ID == kTokenKeywordForce) {
		// Check for unsupported Shanky-style delay
		// which can only happen after actions 
		// WHEN ... RAISEMAX FORCE DELAY 42
		_token_ID = _tokenizer.LookAhead();
		if (_token_ID == kTokenUnsupportedDelay) {
			CParseErrors::Error("Unsupported Shanky-style delay.\n"
        "OpenHoldem provides a far more simple\n"
        "and far more powerful f$delay-function for that.\n");
			// And consume 2 tokens to avoid further messages;
			_token_ID = _tokenizer.GetToken();
			_token_ID = _tokenizer.GetToken();
		}
		// Both cases, with and without delay, are considered "good".
		return true;
	} else if (last_important_roken_ID == kTokenActionRaise) {
    // Last thing we saw was a Raise
    // Probably Shanky-style betsizing
    CParseErrors::Error("Missing keyword FORCE after action Raise.\n"
      "Did you attempt to specify a betsize the old Shanky way?\n"
      "Then either use RaiseTo or RaiseBy.\n");
    return false;

  }
  // General error message on missing keyword force
	CParseErrors::Error("Missing keyword FORCE after action.\n");
	return false;
}

TPParseTreeTerminalNodeBetsizeAction CFormulaParser::ParseOpenPPLRaiseToExpression() {
  // RaiseTo N Force
	// Keyword RaiseTo got already consumed
	TPParseTreeTerminalNodeBetsizeAction action = new CParseTreeTerminalNodeBetsizeAction(_tokenizer.LineRelative());
	TPParseTreeNode expression;
	int _token_ID = _tokenizer.LookAhead();
	if ((_token_ID == kTokenNumber)
		  || (_token_ID == kTokenIdentifier)
		  || TokenIsBracketOpen(_token_ID)) {
		expression = ParseExpression();
	} else {
    CParseErrors::Error("Missing expression after keyword RaiseTo.\n"
      "Expecting the betsize in big blinds.\n");
    return NULL;
  }
  action->MakeRaiseToAction(expression);
  return action;
}

TPParseTreeTerminalNodeBetsizeAction CFormulaParser::ParseOpenPPLRaiseByExpression() {
	// There are 3 possibilities
	//   RAISE <Amount> FORCE
	//   RAISE <PercentagedPot>% FORCE
	//
	// Keyword RAISE got already consumed
  TPParseTreeTerminalNodeBetsizeAction action = new CParseTreeTerminalNodeBetsizeAction(_tokenizer.LineRelative());
	TPParseTreeNode expression;
	int _token_ID = _tokenizer.LookAhead();
	if ((_token_ID == kTokenNumber)
		  || (_token_ID == kTokenIdentifier)
		  || TokenIsBracketOpen(_token_ID)){
		expression = ParseExpression();
	} else {
    CParseErrors::Error("Missing expression after keyword RaiseBy.\n"
      "Expecting the betsize in big blinds or a potsize-expression.\n"
      "Example: WHEN ... RAISEBY 60% FORCE\n");
    return NULL;
  }
	_token_ID = _tokenizer.LookAhead();
	if (_token_ID == kTokenOperatorPercentage) {
		// Percentaged Potsize
		_tokenizer.GetToken();
		action->MakeRaiseByPercentagedPotsizeAction(expression);
		return action;
	}	else {
		// Raise by N big blinds
		action->MakeRaiseByAction(expression);
		return action;
	}
}

void CFormulaParser::BackPatchOpenEndedWhenConditionSequence(
  TPParseTreeNode first_when_condition_of_a_function) {
  // Backpatching everything after a complete functiuon got parsed
  TPParseTreeNode last_open_ended_when_condition = NULL;
  TPParseTreeNode current_when_condition = first_when_condition_of_a_function;
  // Always holding the last good value, even if the current one is NULL.
  // For handling the end-of-function-node.
  TPParseTreeNode last_when_condition = current_when_condition;
  while (current_when_condition != NULL) {
    last_when_condition = current_when_condition;
    if (current_when_condition->IsOpenEndedWhenCondition()) {
      // Setting the "Else"-part of the last open-ended when-condition
      // to the next open ended when condition
      if (last_open_ended_when_condition != NULL) {
        assert(last_open_ended_when_condition != current_when_condition);
        last_open_ended_when_condition->SetRightMostSibbling(
          current_when_condition);
      }
      last_open_ended_when_condition = current_when_condition;
      // The "Then"-part (2nd sibbling) of an open-ended when-condition
      // points to the next (maybe open-ended) when-condition.
      // The 3rd sibbling is ATM undefined and gets back-patched later.
      assert(current_when_condition != current_when_condition->_second_sibbling);
      current_when_condition = current_when_condition->_second_sibbling;
    } else if (current_when_condition->IsWhenConditionWithAction()) {
      // Normal when-condition with action (2nd sibbling).
      // The 3rd sibbling contains the "Else"-part,
      // i.e. next (maybe open-ended) when-condition
      TPParseTreeNode right_most_sibbling = current_when_condition->GetRightMostSibbling();
      assert(current_when_condition != right_most_sibbling);
      if ((right_most_sibbling == NULL)
          || !right_most_sibbling->IsAnyKindOfWhenCondition()) {
        // End of when-condition-sequence reached
        break;
      }
      current_when_condition = right_most_sibbling;
    } else {
      // End of when-condition sequence reached
      assert(current_when_condition != NULL);
      assert(!current_when_condition->IsAnyKindOfWhenCondition());
      break;
    }   
  }
  // End of when-condition sequence reached
  assert(last_when_condition != NULL);
  assert(last_when_condition->IsAnyKindOfWhenCondition());
  // Insert special node for end of function
  // Either default bot-logic or
  // empty_expression__false__zero__when_others_fold_force
  TPParseTreeTerminalNodeEndOfFunction end_of_function_node = new CParseTreeTerminalNodeEndOfFunction(_tokenizer.LineRelative());
  last_when_condition->_third_sibbling = end_of_function_node;
  if (last_open_ended_when_condition != NULL) {
    if (last_open_ended_when_condition->IsOpenEndedWhenCondition()) {
      last_open_ended_when_condition->_third_sibbling = end_of_function_node;
    }
  }
}

void CFormulaParser::ParseDebugTab(CString function_text) {
  EnterParserCode();
  assert(p_debug_tab != NULL);
  _is_parsing_debug_tab = true;
  p_debug_tab->Clear();
  p_parser_symbol_table->Clear();
  CString next_line;
  int separator_index = 0;
  // Split lines
  while (AfxExtractSubString(next_line, function_text, separator_index, '\n')) {
    ++separator_index;
    int pos = next_line.Find('=');
    if (pos < 0) {
      // No equality-sign found. Empty line or not valid
      continue;
    }
    // Expression-text: everything behind first "="
    int expresion_length = next_line.GetLength() - pos - 1;
    CString expression_text = next_line.Right(expresion_length);
    // Parse this line
    _tokenizer.SetInput(expression_text);
    TPParseTreeNode expression = ParseExpression();
    // Care about operator precendence
    _parse_tree_rotator.Rotate(expression, &expression);
    // Add line and expression to debug-tab
    assert(p_debug_tab != NULL);
    p_debug_tab->AddExpression(expression_text, expression);
  }
  p_parser_symbol_table->VeryfyAllUsedFunctionsAtEndOfParse();
  _is_parsing_debug_tab = false;
  LeaveParserCode();
}

