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

#ifndef INC_CSymbolEngineMTTInfo_H
#define INC_CSymbolEngineMTTInfo_H

#include "CVirtualSymbolEngine.h"
#include "CScrapedMoney.h"

class CSymbolEngineMTTInfo: public CVirtualSymbolEngine {
 public:
	CSymbolEngineMTTInfo();
	~CSymbolEngineMTTInfo();
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
	bool ConnectedToMTT();
  bool ConnectedToAnyTournament();
 public:
	void set_mtt_number_entrants(CString s)   { _mtt_number_entrants.SetValue(s); }
	void set_mtt_players_remaining(CString s) { _mtt_players_remaining.SetValue(s); }
	void set_mtt_my_rank(CString s)           { _mtt_my_rank.SetValue(s); }
	void set_mtt_paid_places(CString s)       { _mtt_paid_places.SetValue(s); }
	void set_mtt_largest_stack(CString s)     { _mtt_largest_stack.SetValue(s); }
	void set_mtt_average_stack(CString s)     { _mtt_average_stack.SetValue(s); }
	void set_mtt_smallest_stack(CString s)    { _mtt_smallest_stack.SetValue(s); }
 private:
	// MTT regions
	CScrapedMoney _mtt_number_entrants;
  CScrapedMoney _mtt_players_remaining;
  CScrapedMoney _mtt_my_rank;
  CScrapedMoney _mtt_paid_places;
  CScrapedMoney _mtt_largest_stack;
  CScrapedMoney _mtt_average_stack;
  CScrapedMoney _mtt_smallest_stack;
};

extern CSymbolEngineMTTInfo *p_symbol_engine_mtt_info;

#endif INC_CSymbolEngineMTTInfo_H