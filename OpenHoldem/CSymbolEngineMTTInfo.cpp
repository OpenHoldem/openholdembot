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
#include "CSymbolEngineMTTInfo.h"

#include "CBetroundCalculator.h"
#include "CPreferences.h"
#include "CScraper.h"
#include "CStringMatch.h"
#include "CTableState.h"
#include "MagicNumbers.h"

CSymbolEngineMTTInfo *p_symbol_engine_mtt_info = NULL;

CSymbolEngineMTTInfo::CSymbolEngineMTTInfo() {
	// The values of some symbol-engines depend on other engines.
	// As the engines get later called in the order of initialization
	// we assure correct ordering by checking if they are initialized.
	//
	// This engine does not use any other engines.
}

CSymbolEngineMTTInfo::~CSymbolEngineMTTInfo() {
}

void CSymbolEngineMTTInfo::InitOnStartup() {
	UpdateOnConnection();
}

void CSymbolEngineMTTInfo::UpdateOnConnection() {
	_mtt_number_entrants.Reset();
	_mtt_players_remaining.Reset();
	_mtt_my_rank.Reset();
	_mtt_paid_places.Reset();
	_mtt_largest_stack.Reset();
	_mtt_average_stack.Reset();
	_mtt_smallest_stack.Reset();
}

void CSymbolEngineMTTInfo::UpdateOnHandreset() {
}

void CSymbolEngineMTTInfo::UpdateOnNewRound() {
}

void CSymbolEngineMTTInfo::UpdateOnMyTurn() {
}

void CSymbolEngineMTTInfo::UpdateOnHeartbeat() {
}

bool CSymbolEngineMTTInfo::EvaluateSymbol(const CString name, double *result, bool log /* = false */)
{
  FAST_EXIT_ON_OPENPPL_SYMBOLS(name);
  if (memcmp(name, "mtt_", 4) != 0) {
    // Symbol of a different symbol-engine
	  return false;
  }
	// MTT symbols
  if (memcmp(name, "mtt_number_entrants", 19)==0)        *result = _mtt_number_entrants.GetValue();
  else if (memcmp(name, "mtt_players_remaining", 21)==0) *result = _mtt_players_remaining.GetValue();
  else if (memcmp(name, "mtt_my_rank", 11)==0)	         *result = _mtt_my_rank.GetValue();
  else if (memcmp(name, "mtt_paid_places", 15)==0)       *result = _mtt_paid_places.GetValue();
  else if (memcmp(name, "mtt_largest_stack", 17)==0)	   *result = _mtt_largest_stack.GetValue();
  else if (memcmp(name, "mtt_average_stack", 17)==0)	   *result = _mtt_average_stack.GetValue();
  else if (memcmp(name, "mtt_smallest_stack", 18)==0) 	 *result = _mtt_smallest_stack.GetValue();
  else {
	  // Symbol of a different symbol-engine
	  return false;
  }
  return true;
}

// If any of these are true then we are connected to a MTT
bool CSymbolEngineMTTInfo::ConnectedToMTT() {
  return (_mtt_number_entrants.GetValue() > p_tablemap->nchairs()
	  || _mtt_players_remaining.GetValue() > p_tablemap->nchairs()
	  || _mtt_paid_places.GetValue() > p_tablemap->nchairs()
	  || _mtt_my_rank.GetValue() > p_tablemap->nchairs());
}

// If any of these are true then we are connected to a MTT or SNG
bool CSymbolEngineMTTInfo::ConnectedToAnyTournament() {
  return (_mtt_number_entrants.GetValue() > 0
    || _mtt_players_remaining.GetValue() > 0
    || _mtt_paid_places.GetValue() > 0
    || _mtt_my_rank.GetValue() > 0);
}

CString CSymbolEngineMTTInfo::SymbolsProvided() {
  return "mtt_number_entrants mtt_players_remaining mtt_paid_places mtt_my_rank "
    "mtt_largest_stack mtt_average_stack mtt_smallest_stack ";
}
