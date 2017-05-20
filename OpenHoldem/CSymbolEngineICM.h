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

#ifndef INC_CSYMBOLENGINEICM
#define INC_CSYMBOLENGINEICM

#include "CVirtualSymbolEngine.h"

class CSymbolEngineICM: public CVirtualSymbolEngine {
 public:
  CSymbolEngineICM();
  ~CSymbolEngineICM();
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
	CString SymbolsProvided();
 private:
  int GetChairFromDealPos(const char* pquery);
  double EquityICM(double *stacks, double *prizes, int playerNB, int player);
};

extern CSymbolEngineICM *p_symbol_engine_icm;

#endif INC_CSYMBOLENGINEICM