//*******************************************************************************
//
// This file is part of the OpenHoldem project
//   Download page:         http://code.google.com/p/openholdembot/
//   Forums:                http://www.maxinmontreal.com/forums/index.php
//   Licensed under GPL v3: http://www.gnu.org/licenses/gpl.html
//
//*******************************************************************************
//
// Purpose: Calculating the OpenPL-symbol "nutfullhouseorfourofakind"
//   by relative "simple" iterations, as the functions in the OpenPPL-library
//   proved to be way to complicated and error-prone.
//
//*******************************************************************************

#pragma once

#include "CVirtualSymbolEngine.h"
#include <map>

// Using a large number for no full-house",
// compatible to the former implementation in the OpenPPL-library
const int kNutFullhouseOrFourOfAKind_NoFullHouseAtAll = 999;
// The best fullhouse or quads has a value of 1 by Shanky definition
const int kNutFullhouseOrFourOfAKind_WeHaveTheNuts = 1;

class CSymbolEngineNutFullhouseOrFourOfAKind: public CVirtualSymbolEngine {
 public:
	CSymbolEngineNutFullhouseOrFourOfAKind();
	~CSymbolEngineNutFullhouseOrFourOfAKind();
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
	CString SymbolsProvided();;
 private:
  void CalculateNutFullhouseOrFourOfAKind();
  bool IsFullhouseOrFourOfAKind(int hand_val);
  int Handstrength(CardMask cards, int n_cards);
 private:
  int FindUnusedCard(CardMask used_cards, int desired_rank);
 private:
  int _nutfullhouseorfourofakind;
  int _nutfullhouseorfourofakind_total_count;
 private:
  std::map<int, bool> _list_all_fullhouses;
  std::map<int, bool> _list_better_fullhouses;
};

extern CSymbolEngineNutFullhouseOrFourOfAKind *p_symbol_engine_nutfullhouse_or_four_of_a_kind;