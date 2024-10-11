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

#ifndef INC_CSYMBOLENGINEHISTORY_H
#define INC_CSYMBOLENGINEHISTORY_H

#include "CVirtualSymbolEngine.h"
#include "CBetroundCalculator.h"
#include "CSymbolEngineUserChair.h"

const int k_hist_sym_count = 93; 

class CSymbolEngineHistory: public CVirtualSymbolEngine {
 public:
	CSymbolEngineHistory();
	~CSymbolEngineHistory();
 public:
	// Mandatory reset-functions
	void InitOnStartup();
	void UpdateOnConnection();
	void UpdateOnHandreset();
	void UpdateOnNewRound();
	void UpdateOnMyTurn();
	void UpdateOnHeartbeat();
  void UpdateAfterAutoplayerAction(int autoplayer_action_code);
 public:
	bool EvaluateSymbol(const CString name, double *result, bool log = false);
  CString SymbolsProvided();
 public:
	// Public accessors
	// Public accessors
	bool DidAct();	
	bool DidAct(int betround);
	bool DidActThisHand()	{	return DidAct(kBetroundPreflop); }
  // For the handreset-detector
  bool DidFoldThisHand();
 public:
	int nplayersround(int betround)		{ return _nplayersround[betround]; }
	int botslastaction(int betround)	{ return _botslastaction[betround]; }
	double nbetsround(int betround)		{ return _nbetsround[betround]; }
  // Didchec can only be 0 or 1 poker-logically,
  // but the counter might be > 0 if the autoplayer did act more than once
  // or if the user scraped check- and call-buttons incorrectly.
	int didchec(int betround)			{ return ((_autoplayer_actions[betround][k_autoplayer_function_check] > 0) ? 1:0) ; }
	int didcall(int betround)			{ return _autoplayer_actions[betround][k_autoplayer_function_call]; }
	int didrais(int betround)			{ return _autoplayer_actions[betround][k_autoplayer_function_raise]; }
	int didswag(int betround)			{ return _autoplayer_actions[betround][k_autoplayer_function_betsize]; }
	int didfold(int betround)			{ return _autoplayer_actions[betround][k_autoplayer_function_fold]; }
	int didalli(int betround)			{ return _autoplayer_actions[betround][k_autoplayer_function_allin]; }
	int prevaction()			    	{ return _prevaction; }
 private:
	void SetPrevaction(int autoplayer_action_code);
	void CalculateHistory();
  double HistorySymbol(const CString sym, const int round);
 private:
	int _prevaction;
 private:
	// Element 0 is unused
	// Elements 1..4 are for the 4 betrounds
	int _nplayersround[kNumberOfBetrounds + 1];		
	int _botslastaction[kNumberOfBetrounds + 1];		
	double _nbetsround[kNumberOfBetrounds + 1];
public:
	// Autoplayer-actions for the 4 bet-rounds
	// First dimension: betround; element 0 is unused again.
	// Second dimension: only some actions are really used,
	//   but it does not hurt to have a simple and extensible interface.
	//   "betpot" gets treated as swag, etc.
	int _autoplayer_actions[kNumberOfBetrounds + 1][k_autoplayer_function_fold];
 private:
  // Remebering symbol values of former streets
  // Index 0 is unused
	double _hist_sym[k_hist_sym_count][kNumberOfBetrounds+1];
};

#endif INC_CSYMBOLENGINEHISTORY_H


