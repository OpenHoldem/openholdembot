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

#ifndef INC_CFORMULAPARSER_H
#define INC_CFORMULAPARSER_H

#include "afx.h"
#include "atlstr.h"
#include "CFormulaFileSplitter.h"
#include "COHScriptList.h"
#include "CParseTreeNode.h"
#include "CParseTreeOperatorNode.h"
#include "CParseTreeRotator.h"
#include "CParseTreeTerminalNode.h"
#include "CParseTreeTerminalNodeBetsizeAction.h"
#include "CTokenizer.h"

class CFormulaParser {
 public:
  CFormulaParser();
  ~CFormulaParser();
 public:
  void InitNewParse();
 public:
  // This function will
  // * load the OpenPPL-library, if needed and posisble
  // * then load the formula file with user-defined bot-logic
  void ParseFormulaFileWithUserDefinedBotLogic(CArchive & formula_file);
 public:
  // it is important to get the line-number first and pass it to the functions below.
  // First splitting the function-text would read up to the next function-header
  // and then get the starting line wrong.
  // http://www.maxinmontreal.com/forums/viewtopic.php?f=111&t=18337
  void ParseSingleFormula(CString name, CString function_text, int starting_line);
  void ParseSingleFormula(CString function_text, int starting_line);
  void ParseDefaultLibraries();
  void ParseLibrary(CString library_path);
 public:
  static CString CurrentFunctionName();
  static CString CurrentFile();
  bool IsParsing()	                      { return (_is_parsing_counter > 0); }
  bool IsParsingReadOnlyFunctionLibrary() { return _is_parsing_read_only_function_library; }
  bool IsParsingDebugTab()                { return _is_parsing_debug_tab; }
 private:
  void ParseFile(CArchive & formula_file);
 private:
  bool VerifyFunctionHeader(CString function_header);
  bool VerifyFunctionNamingConventions(CString name);
  void ExpectMatchingBracketClose(int opening_bracket);
  bool ExpectConditionalThen();
  bool ExpectKeywordForce(int last_important_roken_ID);
  void CheckForExtraTokensAfterEndOfFunction();
 private:
  // OH-script and OpenPPL
  void ParseListBody(COHScriptList *list);
  TPParseTreeNode ParseFunctionBody();
  void ParseDebugTab(CString function_text);
 private:
  // OH-script
  TPParseTreeNode ParseExpression(); 
  TPParseTreeOperatorNode ParseBracketExpression();
  TPParseTreeOperatorNode ParseUnaryExpression();
  TPParseTreeTerminalNode ParseSimpleExpression();
  void ParseConditionalPartialThenElseExpressions(
	  TPParseTreeNode *then_expression, TPParseTreeNode *else_expression);
 private:
  // OpenPPL
  TPParseTreeOperatorNode ParseOpenEndedWhenConditionSequence();
  TPParseTreeTerminalNodeBetsizeAction ParseOpenPPLRaiseToExpression();
  TPParseTreeTerminalNodeBetsizeAction ParseOpenPPLRaiseByExpression();
  TPParseTreeNode ParseOpenPPLAction();
 private:
  TPParseTreeTerminalNode ParseOpenPPLUserVar(); 
 private:
  void BackPatchOpenEndedWhenConditionSequence(
    TPParseTreeNode first_when_condition_of_a_function);
 private:
  bool IsValidFunctionName(CString name);
  void ErrorMissingAction(int token_ID);
 private:
  void EnterParserCode();
  void LeaveParserCode();
 private:
  CFormulaFileSplitter _formula_file_splitter;
  CTokenizer _tokenizer;
  CParseTreeRotator _parse_tree_rotator;
 private:
  CString _token;
  int _is_parsing_counter;
  bool _is_parsing_read_only_function_library;
  // The operator % can mean both modulo (OH-script) and percentage (OpenPPL).
  // Its meaning gets detected by function context.
  // But we can't do so in the debug-tab, so we want to throw a warning.
  // http://www.maxinmontreal.com/forums/viewtopic.php?f=297&t=19973&p=140389#p140389
  bool _is_parsing_debug_tab;
};

extern CFormulaParser *p_formula_parser;

#endif INC_CFORMULAPARSER_H