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

#ifndef INC_CPARSEERRORS_H
#define INC_CPARSEERRORS_H

#include "pch.h"

class CParseErrors {
 public:
  CParseErrors();
  ~CParseErrors();
  public:
  // These functions are static,
  // as we want to encapsulate everything in a class,
  // but have no state and don't want/need the instantiation of an object.
  static void ErrorUnknownIdentifier(CString name);
  static void Error(CString short_message);
  static void UnexpectedToken(CString short_error_message, CString expected, int token_found_instead);
  static void MessageBox_Formula_Error(CString Message, CString Title);
 public:
  static void ClearErrorStatus();
  static bool AnyError();
 private:
  static CString ErroneousCodeSnippet();
 private:
  static bool _is_error;
  static CString _last_error_message;
};

#endif INC_CPARSEERRORS_H
