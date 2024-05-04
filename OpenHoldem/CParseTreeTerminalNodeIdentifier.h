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

#ifndef INC_CPARSETREETERMINALNODEIDENTIFIER_H
#define INC_CPARSETREETERMINALNODEIDENTIFIER_H

#include "CParsetreeTerminalNode.h"

class CParseTreeTerminalNodeIdentifier: public CParseTreeTerminalNode {
  friend class CFormulaParser;
  friend class CParseTreeRotator;
 public:
  CParseTreeTerminalNodeIdentifier(int relative_line_number, CString name);
  virtual ~CParseTreeTerminalNodeIdentifier();
 public:
  virtual double Evaluate(bool log = false);
  virtual CString EvaluateString(bool log = false);
  virtual CString EvaluateToString(bool log = false);
  virtual bool EvaluatesToBinaryNumber();
 public:
  // For debugging output
  CString Serialize();
 public:
  // Might be called by CSymbolEngineMemorySymbols
  // to evaluate right-hand-side expressions
  static double EvaluateIdentifier(CString name, bool log);
  static CString EvaluateStringIdentifier(CString name, bool log);
 private:
  bool IsBinaryIdentifier();
 protected:
  CString _terminal_name;
  // Line number relative to current function
};

typedef CParseTreeTerminalNodeIdentifier *TPParseTreeTerminalNodeIdentifier;

#endif INC_CPARSETREETERMINALNODEIDENTIFIER_H