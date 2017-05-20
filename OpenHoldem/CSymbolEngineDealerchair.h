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

#ifndef INC_CSYMBOLENGINEDEALERCHAIR_H
#define INC_CSYMBOLENGINEDEALERCHAIR_H

#include "CVirtualSymbolEngine.h"

#define DEALER_CHAIR p_symbol_engine_dealerchair->dealerchair()

class CSymbolEngineDealerchair: public CVirtualSymbolEngine {
 public:
  CSymbolEngineDealerchair();
  ~CSymbolEngineDealerchair();
 public:
  // Mandatory reset-functions
  void InitOnStartup();
  void UpdateOnConnection();
  void UpdateOnHandreset();
  void UpdateOnNewRound();
  void UpdateOnMyTurn();
  void UpdateOnHeartbeat();
 public:
  // Public accessors
  bool EvaluateSymbol(const CString name, double *result, bool log = false);
  inline int dealerchair()			{ return _dealerchair; }
  CString SymbolsProvided();
 private:
  void CalculateDealerChair();
 private:
  int _dealerchair;
};

extern CSymbolEngineDealerchair *p_symbol_engine_dealerchair;

#endif INC_CSYMBOLENGINEUSERCHAIR_H