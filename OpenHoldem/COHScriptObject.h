//******************************************************************************
//
// This file is part of the OpenHoldem project
//    Source code:           https://github.com/OpenHoldem/openholdembot/
//    Forums:                http://www.maxinmontreal.com/forums/index.php
//    Licensed under GPL v3: http://www.gnu.org/licenses/gpl.html
//
//******************************************************************************
//
// Purpose: virtual base-class for CFunction and COHList
//
//******************************************************************************

#ifndef INC_COHSCRIPTOBJECT_H
#define INC_COHSCRIPTOBJECT_H

#include "CSpaceOptimizedFormulaObject.h"

// Either auto-generated for completion
// or created with the editor
const CString kNoSourceFileForThisCode = "no source file";

class COHScriptObject: public CSpaceOptimizedFormulaObject {
  friend class CFunctionCollection;
 public:
  COHScriptObject(); 
  COHScriptObject(
    CString new_name, 
    CString new_function_text,
    CString file_path,
    int starting_line_of_function);
  virtual ~COHScriptObject();
 public:
  CString name()			            { return _name; }
  virtual CString function_text() { return _function_text; }
  CString GetPath()               { return _file_path; }
  int StartingLine()              { return _starting_line_of_function; }
 public:
  void SetName(CString name)      { _name = name; }
  void SetText(CString text, bool is_read_only_library_symbol = true);
 public:
  virtual double Evaluate(bool log = false);
  virtual bool EvaluatesToBinaryNumber();
 public:
  virtual bool ImportedFromShankyPPL() { return false; }
  virtual void MarkAsImportedFromShankyPPL() { /* Nothing to be done for lists and denug-tab */ }
 public:
  static bool IsFunction(CString name)      { return name.Left(2) ==  "f$"; }
  static bool IsList(CString name)          { return name.Left(4) ==  "list"; }
  static bool IsNotes(CString name)         { return name == "notes"; }
  static bool IsOpenPPLSymbol(CString name);
  // f$preflop, f$flop, f$turn, f$river
  static bool IsMainOpenPPLFunction(CString name);
 public:
  bool IsList()                  { return _name.Left(4) == "list"; }
  bool IsFunction()              { return _name.Left(2) ==  "f$"; }
  bool IsOpenPPLSymbol()         { return isupper(_name[0]) != 0; }
  bool IsReadOnlyLibrarySymbol() { return _is_read_only; }
  // f$preflop, f$flop, f$turn, f$river
  bool IsMainOpenPPLFunction();
  // Autoplayer, Secondary, Ini, PrWin
  bool IsStandardFunction();
  bool IsAutoplayerFunction();
  bool IsSecondaryFunction();
  bool IsPrWinFunction();
  bool IsIniFunction();
  bool IsHopperFunction();
  bool IsDebugFunction();
  bool IsNotes();
  bool IsICMConfigurationFunction();
  bool IsUserDefinedFunction()  { return (IsFunction()
                                    && !IsStandardFunction()
                                    && !IsDebugFunction())
                                    && !IsNotes()
                                    && !IsMainOpenPPLFunction(); }
 public:
  int EditorGroupingCategory();
 public:
  // For saving
  CString Serialize();   
  // For debugging
  virtual void Dump();
 public:
  virtual void Parse();
 public:
  // For the function collection
  // Read-only library functions 
  // * will be loaded once
  // * won't be shown in the editor
  // * won't be saved
  void SetAsReadOnlyLibraryFunction();
 protected:
  virtual bool NeedsToBeParsed();
  void MarkAsParsed();
  // To be used by CFunctionCollection::parseAll()
  virtual bool EmptyParseTree() { return false; }
 protected:
  CString _name;
  CString _function_text;
  CString _file_path;
  int _starting_line_of_function;
 private:
  bool _is_read_only;
  bool _modified_since_last_parse;
};

#endif INC_COHSCRIPTOBJECT_H