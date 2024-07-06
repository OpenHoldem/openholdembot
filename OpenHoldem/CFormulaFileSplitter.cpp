//******************************************************************************
//
// This file is part of the OpenHoldem project
//    Source code:           https://github.com/OpenHoldem/openholdembot/
//    Forums:                http://www.maxinmontreal.com/forums/index.php
//    Licensed under GPL v3: http://www.gnu.org/licenses/gpl.html
//
//******************************************************************************
//
// Purpose: Extract functions from an input-file
//   and put them into the function-collection (not yet parsed)
//
//******************************************************************************

#include "stdafx.h"
#include "CFormulaFileSplitter.h"

#include "CFunction.h"
#include "CFunctionCollection.h"
#include "COHScriptObject.h"
#include "COHScriptList.h"
#include "CParseErrors.h"
#include "WindowFunctions/window_functions.h"

// Format of a formula file:
// * Date and time (OH-script only)
//     ##2014-02-09 23:16:55##
// * Notes (OH-script only)
//     ##Notes##
//     Comments below
// * Lists
//     ##list_preflop_monsters##
//     AA KK ...  
// * formulas
//     ##f$alli##
//     C-style expressions or Shanky-style WHEN-conditions
//     Somewhat special cases: f$test and f$debug

const CString kErroneousFunctionName = "-- undefined --";

// public
CFormulaFileSplitter::CFormulaFileSplitter() {
}

// public
CFormulaFileSplitter::~CFormulaFileSplitter() {
}

// public
void CFormulaFileSplitter::SplitFile(CArchive &formula_file) {
  _first_function_processed = false;
  _total_lines_processed = 0;
  _starting_line_of_next_function = 0;
  _splitting_shanky_ppl = false;
  SkipShankyOptionSettings(formula_file);
  // Line-counter needs to be aware of skipped Shanky option-settings
  _starting_line_of_next_function = _total_lines_processed;
  COHScriptObject* next_function_or_list = GetNextObject(formula_file);
  if (next_function_or_list == NULL) {
    CParseErrors::Error("Input file does not contain any bot-logic\n");
    return;
  }
  while (next_function_or_list != NULL) {
    p_function_collection->Add(next_function_or_list);
    next_function_or_list = GetNextObject(formula_file);
  }
}

// Returns 0 / false if no Shanky-style betround-function
// Returns 1..4 for preflop..river
int CFormulaFileSplitter::IsShankyFunction(CString line_of_code) {
  if (!isalpha(line_of_code[0])) {
    return int(false);
  }
  if (toupper(line_of_code[0]) == 'W') {
    // Quick exit on common when conditions
    return false;
  }
  for (int i = kBetroundPreflop; i <= kBetroundRiver; ++i) {
    int length = strlen(k_Shanky_style_betrounds[i]);
      // Valid input is expected to be lowercase
    if (memcmp(line_of_code, k_Shanky_style_betrounds[i], length) != 0) {
      continue;
    }
    if (isalnum(line_of_code[length])) {
      // Sort out longer identifiers at beginning of line.
      // But oder characters may follow, e.g. spaces and comments
      continue;
    }
    assert(i != int(false));
    return i;
  }
  return int(false);
}

void CFormulaFileSplitter::SkipShankyOptionSettings(CArchive &formula_file) {
  bool keyword_custom_seen = false;
  while (true) {
    if (!formula_file.ReadString(_next_line)) {
      break;
    }
    ++_total_lines_processed;
    if (_total_lines_processed == 1) {
      SanityChecksForWrongFileTypes();
    }
    if (IsFunctionHeader(_next_line)) {
      // Function found (OH-script or Shankynese)
      // No longer any Shanky option settings expected
      if (IsShankyFunction(_next_line)) {
        // preflop, flop, turn or river
        if (!keyword_custom_seen) {
          // No parse-error here.
          // Outside of functions we don't have any reasonable context
          MessageBox_Error_Warning("Shanky-style code missing keyword \"custom\".");
        }
      }
      break;
    }
    // !!!!! top-level commewnt outside function
    if (_next_line.Find('=') > 0) {
      // Probably Shanky-style option setting, e.g.
      // MaxSessionHands = 10000
      // MaxSessionTime = 720
      _splitting_shanky_ppl = true;
      continue;
    }
    // Avoid problems with extra spaces
    _next_line.TrimLeft();
    _next_line.TrimRight();
    _next_line.MakeLower();
    if (_next_line == "custom") {
      // Shanky keyword custom found.
      // Usually end of option settings
      keyword_custom_seen = true;
      _splitting_shanky_ppl = true;
      continue;
    }
    // Any other kind of input like empty lines.
    // continue
  }
};

COHScriptObject* CFormulaFileSplitter::GetNextObject(CArchive &formula_file) {
  // We have to get the starting line of the current-function
  // (= end of last found function) before we scan for its content,
  // First splitting the function-text would read up to the next function-header
  // and then get the starting line wrong.
  // http://www.maxinmontreal.com/forums/viewtopic.php?f=111&t=18337
  int starting_line_of_current_function = _starting_line_of_next_function;
  ScanForNextFunctionOrList(formula_file);
  if (_function_name == kErroneousFunctionName) {
    // Input really mal-formed.
    // Shanky-PPL, old-style OpenPPL or something else.
    // We already threw a warning.
    // Returning NULL will terminate parsing (of this file),
    // but this easy solution is OK here.
    return NULL;
  }
  COHScriptObject* new_function_or_list = NULL;
  if (_function_name.Left(4) == "list") {
    new_function_or_list = new COHScriptList(
      _function_name, 
      _function_text, 
      formula_file.GetFile()->GetFilePath(),
      starting_line_of_current_function);
  } else {
    new_function_or_list = new CFunction(
      _function_name,
      _function_text,
      formula_file.GetFile()->GetFilePath(),
      starting_line_of_current_function);
    if (_splitting_shanky_ppl) {
      new_function_or_list->MarkAsImportedFromShankyPPL();
    }
  }
  return new_function_or_list;
}

CString CFormulaFileSplitter::ExtractFunctionName(const CString function_header) {
  // Check correct start of function header 
  // and especially throw warnings on old-style and Shanky-style PPL.
  // Most common error: identifier instead of function header
  // This can only happen before the first function
  CString function_name_lower_case = function_header;
  function_name_lower_case.MakeLower();
  int potential_shanky_betround_function = IsShankyFunction(function_name_lower_case);
  if (potential_shanky_betround_function != int(false)) {
    // We can't use the original input, 
    // as the input might contain additional spaces and comments
    CString correct_name = k_OpenPPL_function_names[potential_shanky_betround_function];
    return correct_name;
  } else if (function_name_lower_case.Left(3) == "new") {
    CParseErrors::Error("Old-style OpenPPL function.\n"
      "OpenHoldem-style ##f$function## expected.\n");
    return kErroneousFunctionName;
  } else if ((function_name_lower_case.Left(2) == "//")
    || (function_name_lower_case.Left(2) == "/*")) {
    CParseErrors::Error("Top-level comment outside function.\n"
      "Technically a formula-file is a set of functions\n"
      "and every comment belongs to such a function.\n"
      "A top-level comment outside of a function would get lost.\n"
      "Please put it for example into \"##notes##\".\n");
    return kErroneousFunctionName;
  } else if (function_name_lower_case == "") {
    // End of file reached;
    // no more functions found
    return kErroneousFunctionName;
  } else if (function_name_lower_case.Left(2) != "##") {
    CParseErrors::Error("Shanky-style option settings?\n"
      "Options for Shankys default-bot are not supported.\n"
      "They need to be removed.\n"
      "Expecting a ##f$function## or ##listXYZ## instead.\n");
    return kErroneousFunctionName;
  }
  // Leading ## found
  if (function_name_lower_case.Right(2) != "##") {
    CParseErrors::Error("Malformed function-header. Trailing ## expected.\n");
    return kErroneousFunctionName;
  }
  // New header verified
  CString function_name = function_header;
  // Get rid pf ## at both ends
  int length = function_name.GetLength();
  assert(length >= 4);
  function_name = function_name.Left(length - 2);
  function_name = function_name.Right(length - 4);
  if (function_name.GetLength() <= 0) {
    CParseErrors::Error("Empty function or list name.\n"
      "Expecting a ##f$function## here.\n");
    return kErroneousFunctionName;
  }
  return function_name;
}

// Returns the next function (including header),
// i.e. everything up to the second-next-function or end of file.
void CFormulaFileSplitter::ScanForNextFunctionOrList(CArchive &formula_file) {
  // Function-header is the first line, 
  // (usually the last line of last scan)
  // rest is content
  CString function_header = _next_line;
  _function_text = ""; 
  while (true) {
    if (!formula_file.ReadString(_next_line)) {
	    break;
    }
    ++_total_lines_processed;
    // Avoid problems with "empty" lines before first function header
    // that  contain spaces.
    _next_line.TrimRight();
#ifdef DEBUG_FORMULA_FILESPLITTER
    //printf("[CFormulaFileSplitter] next line: %s\n", _next_line);
#endif
    if (IsFunctionHeader(_next_line)
        && _first_function_processed) {
      // Next OH-script function found
      // Only continue, if we found the first one
      //
      // In case of break: keep that function-header for the next query
      _starting_line_of_next_function = _total_lines_processed;
      break;
	  }
    if (!IsFunctionHeader(function_header)) {
      // Escpecially meant to catch OpenGeeks newlines 
      // (which are not empty) at the beginning of the file.
      function_header = _next_line;
    } else {
      _first_function_processed = true;
      // Add non-function-header (content) to the functions body
      _function_text += _next_line;
      _function_text += "\n";
    }
  } 
  _function_name = ExtractFunctionName(function_header);
  // Remove superfluous new-lines at the end of the function
  // to avoid blowing up the files on saving...
  _function_text.TrimRight();
  // except one, that is necessary for the tokenizer
  // and results in better formatting
  _function_text += "\n";
#ifdef DEBUG_FORMULA_FILESPLITTER
  //printf("[CFormulaFileSplitter] next function: %s\n", _formula_content);
  MessageBox_Interactive(_function_text, "Function", 0);
#endif
}

// A function header (or list header) starts with ##
bool CFormulaFileSplitter::IsFunctionHeader(CString line_of_code)
{
  if ((line_of_code.GetAt(0) == '#') && (line_of_code.GetAt(1) == '#')) {
    return true;
  }
  if (_splitting_shanky_ppl) {
    if (IsShankyFunction(line_of_code)) {
      return true;
    }
  }
  return false;
}

void CFormulaFileSplitter::SanityChecksForWrongFileTypes() {
  // Some people manage to feed OpenHoldem with unexpected file-types, including
  //  * Downloaded web-pages
  //  * tablemaps
  // We try to diagnose such PEBKACs here
  CString first_line = _next_line;
  first_line.MakeLower();
  if ((first_line.Left(4) == "<htm")
    || (first_line.Left(4) == "<!do")
    || (first_line.Left(4) == "<xml")) {
    // http://www.maxinmontreal.com/forums/viewtopic.php?f=297&t=19814&p=139309
    CParseErrors::Error("Invalid file-type.\n"
      "The input looks like HTML or XML.\n"
      "Did you download a demo from the internet\n"
      "and save a web-page instead of plain text?\n");
  }
  else if (first_line.Left(5) == ".osdb") {
    // http://www.maxinmontreal.com/forums/viewtopic.php?f=117&t=20046#p140821
    CParseErrors::Error("Invalid file-type.\n"
      "The input looks like a tablemap.\n"
      "Tablemaps have to be put into the scraper-directory\n"
      "and then get loaded automatically.\n"
      "Menu->File->Open is only for bot-logic.\n");
  }
}