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

#ifndef INC_CSYMBOLENGINEPOKERACTION_H
#define INC_CSYMBOLENGINEPOKERACTION_H

#include "CVirtualSymbolEngine.h"

class CSymbolEnginePokerAction: public CVirtualSymbolEngine {
 public:
  CSymbolEnginePokerAction();
  ~CSymbolEnginePokerAction();
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
 public:
  const int		DealPosition(const int chairnum);
 private:
  // private functions and variables - not available via accessors or mutators
	const int		PreflopPos();
	const int		PreflopRaisPos();
	const int		PostflopPos();
	const bool	FirstIntoPot();	
	const int		BetPosition(const int chairnum);
	const bool	AgchairAfter();
};

extern CSymbolEnginePokerAction *p_symbol_engine_poker_action;

#endif INC_CSYMBOLENGINEPOKERACTION_H