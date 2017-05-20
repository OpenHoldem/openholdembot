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

#ifndef INC_CSYMBOLENGINE_HAND_AND_BOARD_EXPRESSION_H
#define INC_CSYMBOLENGINE_HAND_AND_BOARD_EXPRESSION_H

#include "CVirtualSymbolEngine.h"

class CSymbolEngineOpenPPLHandAndBoardExpression: public CVirtualSymbolEngine
{
public:
	CSymbolEngineOpenPPLHandAndBoardExpression();
	~CSymbolEngineOpenPPLHandAndBoardExpression();
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
  // Checking for expressions like hand$AKs
  // which get regularly confused with hand$AKsuited.
  // Creating an error-message at parse-time only
  void CheckForProbablyMistakenSpadesInsteadOfSuited(CString expression);
private:
  // defaulting to kUndefinedZero, because this value gets used as an index
	int PrimeCodedRanks(int rank_0,	int rank_1, 
		int opt_rank_2 = kUndefinedZero, int opt_rank_3 = kUndefinedZero, 
		int opt_rank_4 = kUndefinedZero);
	int PrimeCodedRanks(CString card_expression);
private:
	int _prime_coded_hole_cards;
	int _prime_coded_board_cards;
private:
	int prime_coded_available_ranks;
	bool is_hand_expression;
	bool is_board_expression;
	CString hand_or_board_expression;
};

extern CSymbolEngineOpenPPLHandAndBoardExpression *p_symbol_engine_open_ppl_hand_and_board_expression;

#endif INC_CSYMBOLENGINE_HAND_AND_BOARD_EXPRESSION_H