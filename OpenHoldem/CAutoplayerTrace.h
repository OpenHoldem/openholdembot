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

#ifndef INC_CAUTOPLAYER_TRACE_H
#define INC_CAUTOPLAYER_TRACE_H

#include <map>
#include "CSpaceOptimizedGlobalObject.h"

class CAutoplayerTrace : public CSpaceOptimizedGlobalObject {
 public:
   CAutoplayerTrace();
   ~CAutoplayerTrace();
 public:
  void Clear();
  void Add(CString symbol, double value, bool undefined = false);
  // Logging function, value currently unknown
  // Returning index (line number in log)
  int  Add(CString symbol); 
  void BackPatchValueAndLine(int index, double value, CString function_text, int starting_line_of_function, CString path);
  void Indent(bool more);
  void Print(const char *action_taken, bool full_log_for_primary_formulas);
 public:
  void SetLastEvaluatedRelativeLineNumber(int relative_line_number) { _last_evaluated_relative_line_number = relative_line_number; }
  int  GetLastEvaluatedRelativeLineNumber() { return _last_evaluated_relative_line_number; }
 private:
  CString BestAction();
 private:
  void LogBasicInfo(const char *action_taken);
  void LogPlayers();
  void LogSecondaryAction(const char *action_taken);
  void LogAutoPlayerTrace();
  bool SymbolNeedsToBeLogged(CString name);
 private:
  CString Indentation();
 private:
  CArray <CString, CString> _symboltrace_collection;// Used to trace function execution
  std::map<CString, bool> _already_logged_symbols;
  int _indentation;
  int _number_of_log_lines;
  CCritSec m_critsec;
  int _last_evaluated_relative_line_number;
};

extern CAutoplayerTrace *p_autoplayer_trace;

#endif INC_CAUTOPLAYER_TRACE_H