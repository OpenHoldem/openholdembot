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
#include "CSelftestParserEvaluator.h"

#include "CFunctionCollection.h"
#include "CSymbolEngineMemorySymbols.h"
#include "WindowFunctions/window_functions.h"

CSelftestParserEvaluator::CSelftestParserEvaluator() {
}

CSelftestParserEvaluator::~CSelftestParserEvaluator() {
}

void CSelftestParserEvaluator::Test() {
  if (!p_function_collection->Exists(kSelftestName)) {
    MessageBox_Error_Warning("Can't find selftest-function\n"
      "Please get in contact with the developmen team.\n");
    return;
  }
  double result = p_function_collection->Evaluate(kSelftestName);
  if (result != kSelftestExpectedResult) {
    CString message;
    message.Format("Selftest failed.\n"
      "Please get in contact with the developmen team.\n"
      "Calculated result = %.3f\n"
      "Expected result = %.3f\n",
      result, kSelftestExpectedResult);
    MessageBox_Error_Warning(message);
  }
}

