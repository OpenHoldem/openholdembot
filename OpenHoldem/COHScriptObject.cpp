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
#include "COHScriptObject.h"

#include "CFunctionCollection.h"
#include "CMemoryPool.h"

COHScriptObject::COHScriptObject() {
	_name = "";
	_function_text = "";
  _file_path = kNoSourceFileForThisCode;
  _starting_line_of_function = kUndefinedZero;
  _is_read_only = false;
  _modified_since_last_parse = false;
}

COHScriptObject::COHScriptObject(
    CString new_name, 
    CString new_function_text,
    CString file_path,
    int starting_line_of_function) {
  _name = new_name;
  _function_text = new_function_text;
  _file_path = file_path;
  _starting_line_of_function = starting_line_of_function;
  _is_read_only = false;
  _modified_since_last_parse = true;
}

COHScriptObject::~COHScriptObject() {
}

void COHScriptObject::Parse() {
  // virtual
}

void COHScriptObject::MarkAsParsed() {
  _modified_since_last_parse = false;
}

void COHScriptObject::SetAsReadOnlyLibraryFunction() {
#ifdef _DEBUG
  // Just to be able to set a conditional break-point
  if (_name == "f$preflop") {
    _is_read_only = true;
  }
#endif
  _is_read_only = true; 
}

double COHScriptObject::Evaluate(bool log /* = false */) {
	bool this_method_should_always_get_overwritten_and_never_called = false;
	assert(this_method_should_always_get_overwritten_and_never_called);
	return 0.0;
}

void COHScriptObject::SetText(CString text, bool is_read_only_library_symbol /* = true */) { 
  _function_text = text; 
  _modified_since_last_parse = true;
  if (is_read_only_library_symbol == false) {
    // Functions and lists that get modified with the editor
    // need to get marked as not read-only.
    // This is especially necessary for new user-defined functions 
    // that didn't get loaded from a formula-file
    // and default autoplayer-functions which got modified.
    //
    // We don't want to set this flag here,
    // only remove it on users modification
    _is_read_only = false;
  }
}

// virtual
bool COHScriptObject::EvaluatesToBinaryNumber() {
  return false;
}

bool COHScriptObject::IsOpenPPLSymbol(CString name) { 
  // "Random" is no longer an OpenPPL-symbol
  // from a technical point of view
  // as it is implemented internally to avoid caching.
  // IsOpenPPLSymbol() needs to consider this fact
  // as it affects e.g. the autoplayer-trace.
  if (name == "Random") return false;
  return isupper(name[0]) != 0; 
}

bool COHScriptObject::IsMainOpenPPLFunction(CString name) {
  for (int i=kBetroundPreflop; i<=kBetroundRiver; ++i) {
    if (name == k_OpenPPL_function_names[i]) {
      return true;
    }
  }
  return false;
}

bool COHScriptObject::IsMainOpenPPLFunction() {
  return IsMainOpenPPLFunction(_name);
}

bool COHScriptObject::IsStandardFunction() {
  for (int i=0; i<k_number_of_standard_functions; ++i) {
    if (_name == k_standard_function_names[i]) return true;
  }
  return false;
}

bool COHScriptObject::IsAutoplayerFunction() {
  for (int i=k_autoplayer_function_beep; i<=k_autoplayer_function_fold; ++i) {
    if (_name == k_standard_function_names[i]) return true;
  }
  return false;
}

bool COHScriptObject::IsSecondaryFunction() {
  for (int i=k_standard_function_prefold; i<=k_standard_function_chat; ++i) {
    if (_name == k_standard_function_names[i]) return true;
  }
  return false;
}

bool COHScriptObject::IsIniFunction() {
  for (int i=k_init_on_startup; i<=k_init_on_heartbeat; ++i) {
    if (_name == k_standard_function_names[i]) return true;
  }
  return false;
}

bool COHScriptObject::IsHopperFunction() {
  for (int i=k_hopper_function_sitin; i<=k_hopper_function_rebuy; ++i) {
    if (_name == k_standard_function_names[i]) return true;
  }
  return false;
}

bool COHScriptObject::IsPrWinFunction() {
  for (int i=k_prwin_number_of_opponents; i<=k_prwin_wontplay; ++i) {
    if (_name == k_standard_function_names[i]) return true;
  }
  return false;
}

bool COHScriptObject::IsICMConfigurationFunction() {
  for (int i=k_icm_prize1; i<=k_icm_prize9; ++i) {
    if (_name == k_standard_function_names[i]) return true;
  }
  return false;
}

bool COHScriptObject::IsDebugFunction() {
  if (_name == "f$debug") return true;
  if (_name == "f$test")  return true;
  return false;
}

bool COHScriptObject::IsNotes() {
  return (_name == "notes");
}

int COHScriptObject::EditorGroupingCategory() {
  // Category 0: autoplayer / OpenPPL
  if (p_function_collection->IsOpenPPLProfile()  && IsMainOpenPPLFunction()) return 0;
  if (!p_function_collection->IsOpenPPLProfile() && IsAutoplayerFunction())  return 0;
  // Category 1: Secondary (f$chat,..) DLL, notes)
  if (IsSecondaryFunction()) return 1;
  if (IsNotes()) return 1;
  // Category 2: Hopper funvtions (f$sitin, f$close, ...)
  if (IsHopperFunction()) return 2;
  // Category 3: Ini-fucntions
  if (IsIniFunction()) return 3;
  // Category 4: PrWin functions
  if (IsPrWinFunction()) return 4;
  // Category 5: ICM-functions
  if (IsICMConfigurationFunction()) return 5;
  // Category 6: debug-functions
  if (IsDebugFunction()) return 6;
  // Category 7: hand-lists
  if (IsList()) return 7;
  // Category 8: other user defined functions
  if (IsUserDefinedFunction()) return 8;
  // Treat undefined as user-defined;
  return 7;
}

CString COHScriptObject::Serialize() {
  CString result = "##" + _name + "##\r\n" + _function_text;
  // Make sure that we have at least 2 new-lines at the end
  if (result.Right(2) != "\r\n") {
    result += "\r\n\r\n";
  } else if (result.Right(4) != "\r\n\r\n") {
    result += "\r\n";
  }
  return result;
}

void COHScriptObject::Dump() {
  // Default: don't do anything
  // ATM we are only interested in formulas
}

// virtual 
bool COHScriptObject::NeedsToBeParsed() {
  //!!!!! attention: care about failed parse
  return _modified_since_last_parse;
}