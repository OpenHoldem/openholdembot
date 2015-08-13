//*******************************************************************************
//
// This file is part of the OpenHoldem project
//   Download page:         http://code.google.com/p/openholdembot/
//   Forums:                http://www.maxinmontreal.com/forums/index.php
//   Licensed under GPL v3: http://www.gnu.org/licenses/gpl.html
//
//*******************************************************************************
//
// Purpose: Various chair symbols, ecept dealerchair, userchair and raischair,
//   which are of special importance and get handled by other symbol-engines.
//
//*******************************************************************************

#include "stdafx.h"
#include "CSymbolEngineChairs.h"

#include "CBetroundCalculator.h"
#include "CScraper.h"
#include "CSymbolEngineActiveDealtPlaying.h"
#include "CSymbolEnginePokerAction.h"
#include "CSymbolEngineTableLimits.h"
#include "CSymbolEngineUserchair.h"
#include "CTableState.h"
#include "..\CTablemap\CTablemap.h"

CSymbolEngineChairs *p_symbol_engine_chairs = NULL;

CSymbolEngineChairs::CSymbolEngineChairs() {
  // The values of some symbol-engines depend on other engines.
	// As the engines get later called in the order of initialization
	// we assure correct ordering by checking if they are initialized.
  assert(p_symbol_engine_active_dealt_playing != NULL);
  assert(p_symbol_engine_poker_action != NULL);
  assert(p_symbol_engine_userchair != NULL);
  assert(p_symbol_engine_tablelimits != NULL);
}

CSymbolEngineChairs::~CSymbolEngineChairs() {
}

void CSymbolEngineChairs::InitOnStartup() {
  ResetOnConnection();
}

void CSymbolEngineChairs::ResetOnConnection() {
  _nchairs = p_tablemap->nchairs();
  ResetOnHandreset();
}

void CSymbolEngineChairs::ResetOnHandreset() {
  ResetOnNewRound();
}

void CSymbolEngineChairs::ResetOnNewRound() {
  _opponent_headsup_chair = kUndefined;
  _smallblind_chair = kUndefined;
  _bigblind_chair = kUndefined;
  _cutoff_chair = kUndefined;

}

void CSymbolEngineChairs::ResetOnMyTurn() {
  // Only well-defined at my turn and requires userchair for calculation.
  // But it seems some "casinos" like test-suite can break that condition.
  // http://www.maxinmontreal.com/forums/viewtopic.php?f=110&t=17915#p124550
  //assert(p_symbol_engine_userchair->userchair_confirmed());
  CalculateOpponentHeadsupChair();
  CalculateSmallBlindChair();
  CalculateBigBlindChair();
  CalculateCutOffChair();
}

void CSymbolEngineChairs::ResetOnHeartbeat() {
}

void CSymbolEngineChairs::CalculateOpponentHeadsupChair(){
  _opponent_headsup_chair = kUndefined;
	if (p_symbol_engine_active_dealt_playing->nopponentsplaying() > 1) return;
	for (int i = 0; i < kMaxNumberOfPlayers; ++i)	{
		if (IsBitSet(p_symbol_engine_active_dealt_playing->opponentsplayingbits(), i)) {
      _opponent_headsup_chair = i;
      return;
    }
	}
}

void CSymbolEngineChairs::CalculateSmallBlindChair() {
  _smallblind_chair = GetChairByDealposition(1);
}

void CSymbolEngineChairs::CalculateBigBlindChair() {
  _bigblind_chair = GetChairByDealposition(2);
}

void CSymbolEngineChairs::CalculateCutOffChair() {
  int cutoff_dealposition = p_symbol_engine_active_dealt_playing->nplayersdealt() - 1;
  _cutoff_chair = GetChairByDealposition(cutoff_dealposition);
}


int CSymbolEngineChairs::GetChairByDealposition(int dealposition) {
  for (int i=0; i<_nchairs; ++i) {
    if (p_symbol_engine_poker_action->DealPosition(i) == dealposition) {
      return i;
    }
  }
  return kUndefined;
}

bool CSymbolEngineChairs::EvaluateSymbol(const char *name, double *result, bool log /* = false */) {
  if (memcmp(name, "opponent_chair_headsup", 22)==0) {
		*result = _opponent_headsup_chair;  
		return true;
	} else if (memcmp(name, "smallblind_chair", 16)==0) {
		*result = _smallblind_chair;
		return true;
	} else if (memcmp(name, "bigblind_chair", 14)==0) {
		*result = _bigblind_chair;
		return true;
	} else if (memcmp(name, "cutoff_chair", 12)==0) {
		*result = _cutoff_chair;
		return true;
	} 
  // Symbol of a different symbol-engine
	return false;
}

CString CSymbolEngineChairs::SymbolsProvided() {
  return "opponent_chair_headsup smallblind_chair bigblind_chair "
    "cutoff_chair ";
}

