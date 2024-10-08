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

#ifndef INC_CTABLELIMITS_H
#define INC_CTABLELIMITS_H

#include "CVirtualSymbolEngine.h"

#define BIG_BLIND p_engine_container->symbol_engine_tablelimits()->bblind()
#define SMALL_BLIND p_engine_container->symbol_engine_tablelimits()->sblind()

struct STableLimit {
	double sblind;
	double bblind;
	double bbet;
	double ante;
	// We don't deal with antes here, as they are very rarely used.
	// They get stored separately and not auto-locked.
};

class CSymbolEngineTableLimits: public CVirtualSymbolEngine {
 public:
	CSymbolEngineTableLimits();
	~CSymbolEngineTableLimits();
 public:
	// public mutators
	void InitOnStartup();
	void UpdateOnConnection();
	void UpdateOnHandreset();
	void UpdateOnNewRound();
	void UpdateOnMyTurn();
	void UpdateOnHeartbeat();
	void CalcTableLimits();
 public:
	bool EvaluateSymbol(const CString name, double *result, bool log = false);
  CString SymbolsProvided();
 public:
	// public accessors, formerly part of the symbol-structure.
	double sblind();
	double bblind();
	double bigbet();
	double ante();
	double bet(int betround);
	double bet(); // for current betting round
	double buyin();
	double prizepool();
	double prizepoolmultiplier();
// private functions
 private:
  bool TableLimitsNeedToBeComputed();
 private:
	void AutoLockBlinds();
	void AutoLockBlindsForCurrentHand();
	void AutoLockBlindsForCashgamesAfterNHands();
	void RememberBlindsForCashgames();
	bool IsCalculationNeccessary();
  STableLimit BestTableLimitsToBeUsed();
	// private data members
 private:
	bool blinds_locked_for_current_hand; 
	bool blinds_locked_for_complete_session;
 private:
	STableLimit tablelimit_best_guess;
	STableLimit tablelimit_locked_for_current_hand;
	STableLimit tablelimit_locked_for_complete_session;
 private:
	static const int k_number_of_hands_to_autolock_blinds_for_cashgames = 5;
	int number_of_saved_tablelimits;
 private:
	// We store the table-limits for the first N hands in separate arrays
	// instead of TableLimit structures, as we need to operate on arrays later (median function).
	double tablelimits_first_N_hands_sblind[k_number_of_hands_to_autolock_blinds_for_cashgames];
	double tablelimits_first_N_hands_bblind[k_number_of_hands_to_autolock_blinds_for_cashgames];
	double tablelimits_first_N_hands_bbet[k_number_of_hands_to_autolock_blinds_for_cashgames];
 private:
	double _ante;
};

#endif // INC_CTABLELIMITS_H
