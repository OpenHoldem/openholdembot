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
#include "CFunctionCollection.h"

#include "CAutoplayerTrace.h"
#include "CDebugTab.h"
#include "CEngineContainer.h"
#include "CFormulaParser.h"
#include "CFunction.h"
#include "CParseErrors.h"
#include "CParserSymbolTable.h"

#include "CSelftestParserEvaluator.h"

#include "..\DLLs\WindowFunctions_DLL\window_functions.h"

CFunctionCollection *p_function_collection = NULL;

CFunctionCollection::CFunctionCollection(){
  SetFormulaName("NoName");
  _path = "";
  _openPPL_library_loaded = false;
  DeleteAll(true, true);
}

CFunctionCollection::~CFunctionCollection() {
  DeleteAll(true, true);
  assert(_function_map.size() == 0);
}

void CFunctionCollection::DeleteAll(bool delete_read_only_library_functions, bool delete_user_defined) {
  write_log(Preferences()->debug_formula(), 
    "[CFunctionCollection] DeleteAll()\n");
  COHScriptObject *p_nextObject = GetFirst();
  while (p_nextObject != NULL) {
    bool needs_deletion = false;
    if (delete_read_only_library_functions && p_nextObject->IsReadOnlyLibrarySymbol()) {
      needs_deletion = true;
    }
    else if (delete_user_defined && !p_nextObject->IsReadOnlyLibrarySymbol()) {
      // The debug-tab must not be deleted.
      // It is a global object that is NOT in the function-collection
      // http://www.maxinmontreal.com/forums/viewtopic.php?f=111&t=19616
      assert(p_nextObject->name() != "f$debug");
      needs_deletion = true;
    }
    if (needs_deletion) {
      write_log(Preferences()->debug_formula(),
        "[CFunctionCollection] Going to delete %s\n", p_nextObject->name());
      Delete(p_nextObject->name());
    }
    p_nextObject = GetNext();
  }
  if (delete_read_only_library_functions) {
    p_memory_pool_library_logic->ReleaseAll();
  }
  if (delete_user_defined) {
    p_memory_pool_user_logic->ReleaseAll();
  }
}

bool CFunctionCollection::CheckForOutdatedFunction(CString name) {
  if (name == "f$alli") {
    CParseErrors::Error("f$alli got replaced by f$allin.\n");
    return true;
  }
  if (name == "f$rais") {
    CParseErrors::Error("f$rais got replaced by f$raise.\n");
    return true;
  }
  if (name  == "f$play") {
    CParseErrors::Error("f$play got replaced\n"
      "by f$sitin, f$sitout, f$leave.\n");
    return true;
  }
  if (name  == "f$P") {
    CParseErrors::Error("f$P got replaced\n"
      "by f$prwin_number_of_opponents.\n");
    return true;
  }
  if (name  == "f$swag") {
    CParseErrors::Error("f$swag and its supporting functions got replaced\n"
      "by a single function f$betsize (raise-to-semantics)\n"
      "and OpenHoldem automatically cares about casino-specific adjustments\n"
      "if you provide the casinos swagtextmethod in the tablemap.\n");
    return true;
  } 
  if (name.Left(9) == "f$OpenPPL") {
    CParseErrors::Error("This is old-style translated OpenPPL.\n"
      "OpenHoldem supports now plain-text OpenPPL.\n"
      "The old technology is no longer compatible.\n"
      "Please have a look at the latest OpenPPL-manual.\n"
      "Things became far more easy.\n");
	  return true;
  }
  return false;
}

bool CFunctionCollection::CheckForMisspelledOpenPPLMainFunction(CString name) {
  // OpenPPL main-functions are the only ones 
  // that have to be created by the user and get only called by OH
  // and nowhere got checked.
  // Looks like potential for PEBKAC, therefore we check that here.
  // http://www.maxinmontreal.com/forums/viewtopic.php?f=297&t=17891
  CString name_lowercases = name;
  name_lowercases.MakeLower();
  if (name == name_lowercases) {
    // Can't be misspelled
    return false;
  }
  for (int i=kBetroundPreflop; i<kBetroundRiver; ++i) {
    if (name_lowercases == k_OpenPPL_function_names[i]) {
      CParseErrors::Error("Misspelled OpenPPL main-function?\n"
        "f$preflop, f$flop, f$turn and f$river are expected to be all lower-cases.\n");
      return true;
    }
  }
  return false;
}

void CFunctionCollection::Add(COHScriptObject *new_function) {
  CSLock lock(m_critsec);
  assert(new_function != NULL);
  CString name = new_function->name();
  if (name == "f$debug") {
    // f$debug is a special global object 
    // and must not be added to the collection
    // (to avoid deletion)
    // http://www.maxinmontreal.com/forums/viewtopic.php?f=111&t=19616
    assert(p_debug_tab != NULL);
    p_debug_tab->SetText(new_function->function_text());
    delete new_function;
    return;
  }
  if (name == "") {
    write_log(Preferences()->debug_formula(), 
	    "[CFunctionCollection] Invalid empty name\n");
    return;
  }
#ifdef _DEBUG
  if (name == "f$custom_test") {
    // Just to be able to set a debugging break-point in debug-mode
    // that gets triggered only for the custom-library.
    // (And we can't do NULL; because of error 4555.)
    name.Append("");
  }
#endif
  if (Exists(name)) {
    write_log(Preferences()->debug_formula(), 
	    "[CFunctionCollection] Name %s already exists. Deleting it\n", name);
    Delete(name);
  }
  if (CheckForOutdatedFunction(name) || CheckForMisspelledOpenPPLMainFunction(name)) {
    // Ignore it
    write_log(Preferences()->debug_formula(), 
      "[CFunctionCollection] Ignoring bad function %s\n", name);
    return;
  }
  write_log(Preferences()->debug_formula(), 
	  "[CFunctionCollection] Adding %s -> %i\n", name, new_function);
  if (p_formula_parser->IsParsingReadOnlyFunctionLibrary()) { 
    write_log(Preferences()->debug_formula(),
      "[CFunctionCollection] Making function read-only\n");
    new_function->SetAsReadOnlyLibraryFunction();
  }
  _function_map[name] = new_function;
}

bool CFunctionCollection::Exists(CString name) {
  CSLock lock(m_critsec);
  std::map<CString, COHScriptObject*>::iterator it; 
  it = _function_map.find(name); 
  return (it != _function_map.end());
}

// Generates smart error-messages on failure
// To be used by the parser
void CFunctionCollection::VerifyExistence(CString name) {
  write_log(Preferences()->debug_formula(),
    "[CFunctionCollection] VerifyExistence: %s\n", name);
  // The OpenPPL-symbol "Random" is no longer implemented in the library
  // but as a built-in symbol to prevent symbol-caching.
  // Therefore we don't want to check if it is "missing" in the library.
  // http://www.maxinmontreal.com/forums/viewtopic.php?f=111&t=19611
  if (name == "Random") {
    return;
  }
  // First (and most common) case: simple symbol
  if (Exists(name)) {
    write_log(Preferences()->debug_formula(),
      "[CFunctionCollection] VerifyExistence: symbol exists in function collection\n");
    return;
  }
  // Second case: multiplexed function or OpenPPL-symbol
  if (p_engine_container->IsStringSymbol(name)) {
	  CString str_dummy_result;
	  // !!!!! false result on unknown symbol
	  if (p_engine_container->EvaluateSymbol(name, &str_dummy_result)) {
		  write_log(Preferences()->debug_formula(),
			  "[CFunctionCollection] VerifyExistence: symbol exists in engine container\n");
		  return;
	  }
  }
  else {
	  double dummy_result;
	  // !!!!! false result on unknown symbol
	  if (p_engine_container->EvaluateSymbol(name, &dummy_result)) {
		  write_log(Preferences()->debug_formula(),
			  "[CFunctionCollection] VerifyExistence: symbol exists in engine container\n");
		  return;
	  }
  }
  // Error: function does not exist
  write_log(Preferences()->debug_formula(),
    "[CFunctionCollection] VerifyExistence: symbol does not exist\n");
  CString message;
  message.Format("Function used but never defined: %s\n\n", name);
  CString similar_name = GetSimilarNameWithDifferentCases(name);
  name = name.MakeLower();
  if (similar_name != "") {
     message += "Did you mean ";
     message += similar_name;
     message += " instead?";
  } else if (name.Left(5) == "hand$" || name.Left(6) == "board$" || name.Left(4) == "user") {
    message += "hand$ and board$ expressions and user-variables\n";
    message += "are built-in and must be lower-cases.";
  } else {
    // Else: general error-message
  }
  MessageBox_Interactive(message, "Error", 0);
}

CString CFunctionCollection::GetSimilarNameWithDifferentCases(CString function_name) {
  function_name.MakeLower();
  COHScriptObject *p_nextObject = GetFirst();
  while (p_nextObject != NULL) {
    if (function_name == p_nextObject->name().MakeLower()) {
      // Found similar name, just different cases
      return p_nextObject->name();
    }
    p_nextObject = GetNext();
  }
  // Nothing found
  return NULL;
}

COHScriptObject *CFunctionCollection::LookUp(CString name) {
  CSLock lock(m_critsec);
  if (name == "f$debug") {
    // Lookup of the special function f$debug
    // that is a global object and not in the collection
    // (to avoid deletion)
    // http://www.maxinmontreal.com/forums/viewtopic.php?f=111&t=19616
    assert(p_debug_tab != NULL);
    return p_debug_tab;
  }
  write_log(Preferences()->debug_formula(), "[CFunctionCollection] Lookup %s\n", name); 
  std::map<CString, COHScriptObject*>::iterator it; 
  it = _function_map.find(name); 
  if (it == _function_map.end()) {
    // Function or list does not exist
    write_log(Preferences()->debug_formula(), "[CFunctionCollection] Function does not exist\n");
    return NULL;
  }
  return it->second;
}

double CFunctionCollection::Evaluate(CString function_name, bool log /* = false */) {
  CSLock lock(m_critsec);
  double result = kUndefined;
  EvaluateSymbol(function_name, &result, log);
  return result;
}

double CFunctionCollection::EvaluateAutoplayerFunction(int function_code) {
  CString function_name = k_standard_function_names[function_code];
  assert(function_name != "");
  return Evaluate(function_name, true);
}

bool CFunctionCollection::EvaluatesToBinaryNumber(CString function_name) {
  COHScriptObject *p_function = LookUp(function_name);
  if (p_function == NULL) return false;
  return p_function->EvaluatesToBinaryNumber();
}

void CFunctionCollection::SetEmptyDefaultBot() {
  write_log(Preferences()->debug_formula(), 
    "[CFunctionCollection] SetEmptyDefaultBot()\n");
  CSLock lock(m_critsec);
  DeleteAll(false, true);
  SetFormulaName("NoName");
  // After setting the bot we should parse it so that OH can evaluate and act
  ParseAll();
  // Parser, function-collection and library initialized 
  // and a ParseAll to come:
  // this is the perfect place to execute the selftest. 
  ExecuteSelftest();
}

void CFunctionCollection::ExecuteSelftest() {
  write_log(Preferences()->debug_formula(), 
    "[CFunctionCollection] Executing self-test\n");
  /*!!!!!if (p_function_collection->Exists(kSelftestName)) {
    //
    return;
  }*/
  CFunction *p_function = new CFunction(kSelftestName, kSelftestFunction);
  // The parser assunes that every function to ber parsed
  // exists in the collection
  p_function_collection->Add(p_function);
  assert(p_function_collection->Exists(kSelftestName));
  p_function->SetAsReadOnlyLibraryFunction();
  p_function->Parse();
  CSelftestParserEvaluator selftest;
  selftest.Test();
  /*!!!!!!!// The function should stay in the collection until the very end
  // and then should get released together with the OpenPPL-symbols.
  // As VLD indicates that this DONOWORKS we delete it here.*/
  p_function_collection->Delete(kSelftestName);
}

void CFunctionCollection::CheckForDefaultFormulaEntries() {
  write_log(Preferences()->debug_formula(), 
    "[CFunctionCollection] CheckForDefaultFormulaEntries(\n");
  CSLock lock(m_critsec);
  // Header comment
  CreateEmptyDefaultFunctionIfFunctionDoesNotExist(CString("notes"));
  // OpenPPL-functions
  if (IsOpenPPLProfile()) {
    for (int i=kBetroundPreflop; i<=kBetroundRiver; ++i) {
      CString function_name = k_OpenPPL_function_names[i];
      CreateEmptyDefaultFunctionIfFunctionDoesNotExist(function_name);
      assert(LookUp(function_name) != NULL);
    }
  } 
  // Autoplayer-functions
  // We need these functions always,
  // for OH-script naturally and for OpenPPL technically,
  // when we translate the decision to actions
  for (int i=k_autoplayer_function_beep; i<=k_autoplayer_function_fold; ++i) {
	  CreateEmptyDefaultFunctionIfFunctionDoesNotExist(CString(k_standard_function_names[i]));
  }
  // hopper,standard, ini and PrWin functions
  for (int i=k_hopper_function_sitin; i<=k_icm_prize9; ++i) {
	  CreateEmptyDefaultFunctionIfFunctionDoesNotExist(CString(k_standard_function_names[i]));
  }
  // Debug functions	
  CreateEmptyDefaultFunctionIfFunctionDoesNotExist(CString("f$test"));
  // No longer adding f$debug,
  // because f$debug is a global object that always exists
  //http://www.maxinmontreal.com/forums/viewtopic.php?f=111&t=19616
  //
  // PrWin-functions, used by the GUI
  CreateEmptyDefaultFunctionIfFunctionDoesNotExist(
    CString(k_standard_function_names[k_prwin_number_of_iterations]));
}

void CFunctionCollection::SetAutoplayerFunctionValue(int function_code, double value) {
  CString function_name = k_standard_function_names[function_code];
  assert(function_name != "");
  CFunction *p_function = (CFunction *)LookUp(function_name);
  if (p_function == NULL) return;
  p_function->SetValue(value);
}

void CFunctionCollection::CreateEmptyDefaultFunctionIfFunctionDoesNotExist(CString &function_name) {
  if (Exists(function_name)) {
    write_log(Preferences()->debug_formula(), 
      "[CFunctionCollection] Function already exists : %s\n", function_name);
	  return;
  }
  // Formula not found.
  // Add the standard one.
  CString function_text;
  if (function_name == k_standard_function_names[k_autoplayer_function_betsize]) {
    function_text = 
      "// Betsize in dollars, raise-to semantics.\n"
      "// OpenHoldem will auto-adapt the betsize to the casinos input-requirements.\n"
      "// Please define your tablemaps \"betsizeinterpretationmethod\"\n"
      "// to configure this feature.\n";
  } else if (function_name == k_standard_function_names[k_autoplayer_function_check]) {    
    function_text = 
      "// Check whenever it is free to call\n"
      "(call == 0) "; 
  } else if (function_name == k_standard_function_names[k_autoplayer_function_fold]) {
    function_text = 
      "// f$fold should always evaluate to true per default\n"
      "// for auto-check-folding instead of time-outs.\n"
      "1 ";
  } else if (function_name == k_standard_function_names[k_hopper_function_rebuy]) {
    function_text = 
      "// f$rebuy should evaluate to the target-amount in dollars.\n"
      "// This value will be passed to the rebuy-script as a parameter.\n"
      "0\n";
  } else if (function_name == k_standard_function_names[k_hopper_function_select_formula_file]) {
    function_text =
      "// Selects a formula-file that will be loaded on next hand-reset.\n"
      "// WHEN (...) RETURN loadformula$my_winning_bot$ohf FORCE\n";
  } else if (function_name == k_standard_function_names[k_standard_function_delay]) {
    function_text = 
      "// Autoplayer-delay in milli-seconds\n"
      "0\n";
  } else if (function_name == k_standard_function_names[k_standard_function_allin_on_betsize_balance_ratio]) {
    function_text = 
      "// Allin if betsize is more than X% of our total balance (including currentbet).\n"
      "// This affects f$betsize and all f$betpot_X_Y_ functions.\n" 
      "//  0.00 =  0% = disabled\n"
      "//  0.50 = 50%\n"
      "0.00\n";
  } else if (function_name == k_standard_function_names[k_init_on_startup]) {
    function_text = 
      "// Ini-functions are meant for\n"    
      "//   * Initialization of memory-symbols at well-defined times\n"
      "//   * Triggering calculations, e.g. in a DLL\n";
  } else if (function_name == k_standard_function_names[k_prwin_number_of_opponents]) {
    function_text = 
      "// \"Reasonable\" default to get standard PrWin running for beginners.\n"
      "// Adding one extra opponent has two advantages:\n"
      "//   * works even with mis-scraped \"no opponents\"\n"
      "//   * adjusts over-optimistic standard PrWin (random ranges).\n"
      "nopponentsplaying + 1 \n";
  } else if (function_name == k_standard_function_names[k_prwin_number_of_iterations]) {
    function_text = 
      "// \"Reasonable\" default to get PrWin running for beginners.\n"
      "// Large enough to get usable results,\n"
      "// small enough to save CPU-time.\n"
      "1000 ";
  } else if (function_name == k_standard_function_names[k_icm_prize1]) {
    function_text = 
      "// f$icm_prizeX functions are used to configure the payout-structure in tournaments.\n"
      "// The sum of all f$icm_prizeX functions should be 1.00 (= 100%).\n"
      "// Default to get things going: the winner takes it all.\n"
      "1.00\n";
  } else {
    // Add an empty function.
    // The function-text should contain at least one space.
    // The editor does somehow not work for completely empty formulas.
    function_text = " "; 
  }
  write_log(Preferences()->debug_formula(), 
     "[CFunctionCollection] Adding default function: %s\n", function_name);
  // The added functions stays in the collection 
  // until a new profile gets loaded, until it gets overwritten]
  // or until the ebtire collection gets released
  CFunction *p_function = new CFunction(function_name, function_text); 
  Add((COHScriptObject *)p_function);
}

COHScriptObject *CFunctionCollection::GetFirst() {
  CSLock lock(m_critsec);
  enumerator_it = _function_map.begin();
  return GetNext();
}

COHScriptObject *CFunctionCollection::GetNext() {
  CSLock lock(m_critsec);
  if (enumerator_it == _function_map.end()) {
    // "end" points behind the last element
    return NULL;
  }
  COHScriptObject *result = enumerator_it->second;
  write_log(Preferences()->debug_formula(),
    "[CFunctionCollection] GetNext() %s -> %i\n",
    enumerator_it->first, enumerator_it->second);
  enumerator_it++;
  return result;
}

void CFunctionCollection::ClearCache() {
  CSLock lock(m_critsec);
  COHScriptObject *p_oh_script_object = GetFirst();
  while (p_oh_script_object != NULL) {
    if (p_oh_script_object->IsFunction() || p_oh_script_object->IsOpenPPLSymbol()) {
     write_log(Preferences()->debug_formula(),
        "[CFunctionCollection] Clearing cache of %s\n",
        ((CFunction*)p_oh_script_object)->name());
      ((CFunction*)p_oh_script_object)->ClearCache();
    }
    p_oh_script_object = GetNext();
  }
}

void CFunctionCollection::Save(CArchive &ar) {
  CSLock lock(m_critsec);
  // In case this is Shanky-style code we have to
  // show a warning and mark it as "custom"
  // so that OpenHoldem can correctly import it next time.
  if (Exists(k_OpenPPL_function_names[kBetroundPreflop])) {
    if (LookUp(k_OpenPPL_function_names[kBetroundPreflop])->ImportedFromShankyPPL()) {
      MessageBox_Error_Warning(
        "Saving Shanky-style bot-logic to semi-OpenPPL-format\n"
        "Full conversion to OpenPPL is recommended.\n",
        "Warning");
      ar.WriteString("custom\n"
        "// Bot-logic originally imported from Shanky-style PPL.\n"
        "// Remove this keyword and comment once properly converted.\n\n\n");
    }
  }
  // First write the date
  char nowtime[26] = {0};
  CString s;
  /*!!!!!s.Format("##%s##\r\n\r\n", get_time(nowtime)); */
  ar.WriteString(s);
  // Notes are a bit special "functions",
  // so they get extra treatment.
  SaveObject(ar, LookUp("notes"));
  // Then write the standard formula functions...
  // These are functions and symbols, that
  //   * are essential to control the behaviour 
  //	 of (nearly) every poker bot.
  //   * configure some very important constants.
  for (int i=k_autoplayer_function_beep; 
      i<k_number_of_standard_functions; 
      ++i) {
	  SaveObject(ar, LookUp(k_standard_function_names[i]));
  }
  // f$test and f$debug are again special
  SaveObject(ar, LookUp("f$test"));
  SaveObject(ar, LookUp("f$debug"));
  // Handlists
  COHScriptObject *next_object = GetFirst();
  while (next_object != NULL) {
    if (next_object->IsList()) {
      SaveObject(ar, next_object);
    }
    next_object = GetNext();
  }
  // OpenPPL-functions
  for (int i=kBetroundPreflop; i<=kBetroundRiver; ++i) {
    assert(k_OpenPPL_function_names[i] != "");
    SaveObject(ar, LookUp(k_OpenPPL_function_names[i]));
  }
  // User defined functions
  // We already saved the standard-functions
  next_object = GetFirst();
  while (next_object != NULL) {
    if (next_object->IsUserDefinedFunction()
        && !next_object->IsReadOnlyLibrarySymbol()) {
      SaveObject(ar, next_object);
    }
    next_object = GetNext();
  }
}

void CFunctionCollection::SaveObject(
    CArchive &ar, 
    COHScriptObject *function_or_list) {
  CSLock lock(m_critsec);
  if (function_or_list == NULL) {
    write_log(Preferences()->debug_formula(),
      "[CFunctionCollection] Not saving NULL-function %s\n", FormulaName());
    return;
  }
  // Don't save OpenPPL-symbols from the OpenPPL(-library
  // or other read-only library functions
  // to user-defined bot-logic files
  if (function_or_list->IsReadOnlyLibrarySymbol()) {
    write_log(Preferences()->debug_formula(),
      "[CFunctionCollection] Not saving read-only %s\n", FormulaName());
    return;
  }
  ar.WriteString(function_or_list->Serialize());
}

void CFunctionCollection::Dump() {
  COHScriptObject *p_oh_script_object = GetFirst();
  while (p_oh_script_object != NULL) {
    p_oh_script_object->Dump();
    p_oh_script_object = GetNext();  
  }
}

bool CFunctionCollection::Rename(CString from_name, CString to_name) {
  CSLock lock(m_critsec);
  COHScriptObject *object_to_rename = LookUp(from_name);
  if (object_to_rename == NULL) return false;
  if (p_function_collection->LookUp(to_name) != NULL) {
    MessageBox_Interactive("Cannot rename to a function/list that already exists", "Error", 0);
    return false;
  }
  RemoveFromBinaryTree(from_name);
  // ...then rename...
  object_to_rename->SetName(to_name); //!!!! creates a dangling pointer
  // ...and insert again.
  Add(object_to_rename);
  return true;
}

void CFunctionCollection::RemoveFromBinaryTree(CString function_name) {
  CSLock lock(m_critsec);
  COHScriptObject *object_to_delete = LookUp(function_name);
  if (object_to_delete != NULL) {
    std::map<CString, COHScriptObject*>::iterator it;
    it = _function_map.find(function_name);
    if (it != _function_map.end()) {
      write_log(Preferences()->debug_formula(),
        "[CFunctionCollection] Removing %s from lookuo-table\n", function_name);
      // Remove it from the lookup-table...
      _function_map.erase(it);
    }
  }
}

void CFunctionCollection::Delete(CString name) {
  // The debug-tab must not be deleted.
  // It is a global object that is NOT in the function-collection
  // http://www.maxinmontreal.com/forums/viewtopic.php?f=111&t=19616
  assert (name != "f$debug");    
  CSLock lock(m_critsec);
  COHScriptObject *object_to_delete = LookUp(name);
  RemoveFromBinaryTree(name);
  if (object_to_delete != NULL) {
    // No longer deleting any lists or functions here,
    // as these objects get handled by CMemoryPool mow
  }
}

void CFunctionCollection::SetFunctionText(CString name, CString content, bool read_only_library_symbol /*= false */) {
  CSLock lock(m_critsec);
  // Works for both functions and lists
  COHScriptObject *function = LookUp(name);
  if (function == NULL) {
    // Function does not yet exist; new one
    // We need to create name and text on the heap, can't point to the stack
    CString my_text = content;
    CString my_name = name;
    // The added functions stays in the collection 
    // until a new profile gets loaded, until it gets overwritten]
    // or until the ebtire collection gets released
    function = new CFunction(my_name, my_text);
    Add(function); //!!!!! read_only_library_symbol
  } else {
    write_log(Preferences()->debug_formula(), 
      "[CFunctionCollection] Setting function text for %s\n", name);
    function->SetText(content, read_only_library_symbol);
  }
}

bool CFunctionCollection::BotLogicCorrectlyParsed() {
  return (!CParseErrors::AnyError());
}

bool CFunctionCollection::ParseAll() {
  write_log(Preferences()->debug_formula(), 
    "[CFunctionCollection] ParseAll()\n");
  // Adding empty standard-functions
  // http://www.maxinmontreal.com/forums/viewtopic.php?f=156&t=16230
  p_function_collection->CheckForDefaultFormulaEntries();
  CSLock lock(m_critsec);
  assert(p_formula_parser != NULL);
  p_parser_symbol_table->Clear();
  COHScriptObject *p_oh_script_object = GetFirst();
  while (p_oh_script_object != NULL) {
    if (p_oh_script_object->IsFunction() 
      || p_oh_script_object->IsList()
      || p_oh_script_object->EmptyParseTree()) {
      p_oh_script_object->Parse();
    }
    p_oh_script_object = GetNext();  
  }
  // Finally parse the debug-tab,
  // that is no longer in the collection.
  assert(p_debug_tab != NULL);
  p_debug_tab->Parse();
  return true;
}

bool CFunctionCollection::IsOpenPPLProfile() {
  // Since OpenHoldem version 12 OpenPPL is the default mode
  return (IsOHScriptProfile() == false);
}

bool CFunctionCollection::IsOHScriptProfile() {
  // A profile is OH-script if at least f$call exists
  // and f$call is not empty.
  // We chose f$call, because
  //   * "every" profile needs a call function, even push-fold-bots
  //   * f$call is empty per default
  if (!Exists(k_standard_function_names[k_autoplayer_function_call])) return false;
  COHScriptObject *p_call_function = LookUp(k_standard_function_names[k_autoplayer_function_call]);
  assert(p_call_function != NULL);
  CString function_text = p_call_function->function_text();
  // Counting nearly empty functions as empty (default: 1 space)
  return (function_text.GetLength() > 1);
}

void CFunctionCollection::InitOnStartup() {
}

void CFunctionCollection::UpdateOnConnection() {
  ClearCache();
}

void CFunctionCollection::UpdateOnHandreset() {
}

void CFunctionCollection::UpdateOnNewRound() {
}

void CFunctionCollection::UpdateOnMyTurn() {
}

void CFunctionCollection::UpdateOnHeartbeat() {
  ClearCache();
}

bool CFunctionCollection::EvaluateSymbol(const CString name, double *result, bool log /* = false */) {
  CSLock lock(m_critsec);
  if (COHScriptObject::IsFunction(name)
      || COHScriptObject::IsList(name)
      || COHScriptObject::IsOpenPPLSymbol(name)) {
#ifdef _DEBUG
    if (COHScriptObject::IsOpenPPLSymbol(name)) {
      // Just to be able to set a break-point,
      // that doesn't get triggered every heartbeat by an ini-function
      int i = 0;
    }
#endif
    COHScriptObject *p_function = LookUp(name);
    if (p_function == NULL) {
      if (memcmp(name, "Random", 6) == 0) {
        // The OpenPPL-symbol "Random" is no longer implemented in the library
        // but as a built-in symbol to prevent symbol-caching.
        // This special case of a "function-look-alike symbol
        // can't be handled by the function collection.
        // http://www.maxinmontreal.com/forums/viewtopic.php?f=111&t=19611
        return false;
      }
      // Function does not exist
      *result = kUndefinedZero;
      if (p_formula_parser->IsParsing()) {
        // EvaluateSymbol() got called as part of the parse-time-verification
        return false;
      }
      // Runtime-evaluatzion
      // This symbol is something that HAS TO be evaluated here,
      // We didn't find it, so we treat it as zero/false/whatever.
      // Symbol-verification happens at parse-time,
      // so this kind of error can only happen for DLL-guys
      // who call ill-named OH-script-functions.
      if (log) {
        write_log(Preferences()->debug_auto_trace(),
          "[CFunctionCollection] %s -> 0.000 [does not exist]\n", name);
        p_autoplayer_trace->Add(name, *result);
      }
      return true;
    }
    // Function/list found
    *result = p_function->Evaluate(log);
    return true;
  }
  // Not a function and not a list
  return false;
}

CString CFunctionCollection::SymbolsProvided() {
  CString result;
  COHScriptObject *p_oh_script_object = GetFirst();
  while (p_oh_script_object != NULL) {
    result += p_oh_script_object->name(); 
    result += " ";
    p_oh_script_object = GetNext();  
  }
  return result;
}