//***************************************************************************** 
//
// This file is part of the OpenHoldem project
//    Source code:           https://github.com/OpenHoldem/openholdembot/
//    Forums:                http://www.maxinmontreal.com/forums/index.php
//    Licensed under GPL v3: http://www.gnu.org/licenses/gpl.html
//
//***************************************************************************** 
//
// Purpose:
//
//***************************************************************************** 

#include "stdafx.h"
#include "CSymbolEngineTableStats.h"

#include "CBetroundCalculator.h"
#include "CHandresetDetector.h"
#include "CPreferences.h"
#include "CScraper.h"
#include "CStringMatch.h"
#include "CSymbolEngineActiveDealtPlaying.h"
#include "CSymbolEngineChipAmounts.h"
#include "MagicNumbers.h"

CSymbolEngineTableStats *p_symbol_engine_table_stats = NULL;

CSymbolEngineTableStats::CSymbolEngineTableStats() {
	// The values of some symbol-engines depend on other engines.
	// As the engines get later called in the order of initialization
	// we assure correct ordering by checking if they are initialized.
  assert(p_symbol_engine_active_dealt_playing != NULL);
  assert(p_symbol_engine_chip_amounts != NULL);
  UpdateOnConnection();
}

CSymbolEngineTableStats::~CSymbolEngineTableStats() {
}

void CSymbolEngineTableStats::InitOnStartup() {
}

void CSymbolEngineTableStats::UpdateOnConnection() {
  for (int i=0; i<=kNumberOfHands; ++i) {
    seen_flop[i] = 0;
    seen_turn[i] = 0;
    seen_river[i] = 0;
    bets_preflop[i] = 0;
    raised_preflop[i] = 0;
    dealt_players[i] = 0;
  }
}

void CSymbolEngineTableStats::UpdateOnHandreset() {
  ShiftOldestDataAway();
}

void CSymbolEngineTableStats::UpdateOnNewRound() {
}

void CSymbolEngineTableStats::UpdateOnMyTurn() {
}

void CSymbolEngineTableStats::UpdateOnHeartbeat() {
  UpdateData();
}

void CSymbolEngineTableStats::ShiftOldestDataAway() {
  for (int i=0; i<kNumberOfHands; ++i) {
    seen_flop[i]      = seen_flop[i+1];
    seen_turn[i]      = seen_turn[i+1];
    seen_river[i]     = seen_river[i+1];
    bets_preflop[i]   = bets_preflop[i+1];
    raised_preflop[i] = raised_preflop[i+1];
    dealt_players[i]  = dealt_players[i+1];
  }
  seen_flop[kIndexCurrentHand] = 0;
  seen_turn[kIndexCurrentHand] = 0;
  seen_river[kIndexCurrentHand] = 0;
  bets_preflop[kIndexCurrentHand] = 0;
  raised_preflop[kIndexCurrentHand] = 0;
  dealt_players[kIndexCurrentHand] = 0;
}

void CSymbolEngineTableStats::UpdateData() {
  switch (BETROUND) {
    case kBetroundPreflop:
      dealt_players[kIndexCurrentHand] = p_symbol_engine_active_dealt_playing->nplayersdealt();
      bets_preflop[kIndexCurrentHand] = p_symbol_engine_chip_amounts->ncallbets();
      if (p_symbol_engine_chip_amounts->ncallbets() > 1) {
        // raised_preflop is a "counter" with state 0/1
        // so that we can use SumUp().
        raised_preflop[kIndexCurrentHand] = 1;
      }
      break;
    case kBetroundFlop:
      if (seen_flop[kIndexCurrentHand] == 0) {
        seen_flop[kIndexCurrentHand] = p_symbol_engine_active_dealt_playing->nopponentsplaying();
      }
      break;
    case kBetroundTurn:
      if (seen_turn[kIndexCurrentHand] == 0) {
        seen_turn[kIndexCurrentHand] = p_symbol_engine_active_dealt_playing->nopponentsplaying();
      }
      break;
    case kBetroundRiver:
      if (seen_river[kIndexCurrentHand] == 0) {
        seen_river[kIndexCurrentHand] = p_symbol_engine_active_dealt_playing->nopponentsplaying();
      }
      break;
  }
}

double CSymbolEngineTableStats::SumUp(int *data_set_for_n_hands) {
  // Return-tzpe is double, not int.
  // This way we easily avoid the unexpected effects of integer-division.
  int sum = 0;
  for (int i=0; i<kNumberOfHands; ++i) {
    sum += data_set_for_n_hands[i];
  }
  return sum;
}

int CSymbolEngineTableStats::NumberOfHandsStored() {
  int hands_played = p_handreset_detector->hands_played();
  if (hands_played > kNumberOfHands) {
    // All entries contains valid date
    return kNumberOfHands;
  } else if (hands_played == 0) {
    // No data at all
    // Avoid potential division by zero
    return 1;
  } else {
    // Parts of the arrays contain meaningful data
    // Rest is 0, at the moment unused
    return hands_played;
  }
}

double CSymbolEngineTableStats::FlopTurnRiverPct(int betround) {
  switch (betround) {
    case kBetroundFlop:
      return SumUp(seen_flop) / SumUp(dealt_players);
    case kBetroundTurn:
      return SumUp(seen_turn) / SumUp(dealt_players);
    case kBetroundRiver:
      return SumUp(seen_river) / SumUp(dealt_players);
    default: 
      return kUndefinedZero;
  }
}

double CSymbolEngineTableStats::AvgBetsPf() {
	return SumUp(bets_preflop) / NumberOfHandsStored();
}

double CSymbolEngineTableStats::TablePfr() {
	return SumUp(raised_preflop) / NumberOfHandsStored();
}

bool CSymbolEngineTableStats::EvaluateSymbol(const CString name, double *result, bool log /* = false */) {
  if (memcmp(name, "floppct", 7)==0 && strlen(name)==7) {  				
    *result = FlopTurnRiverPct(kBetroundFlop);
    return true;
  }
  if (memcmp(name, "turnpct", 7)==0 && strlen(name)==7) {  	
    *result = FlopTurnRiverPct(kBetroundTurn);
    return true;
  }
  if (memcmp(name, "riverpct", 8)==0 && strlen(name)==8) {  	
    *result = FlopTurnRiverPct(kBetroundRiver);
    return true;
  }
  if (memcmp(name, "avgbetspf", 9)==0 && strlen(name)==9) {  
    *result = AvgBetsPf();
    return true;
  }
  if (memcmp(name, "tablepfr", 8)==0 && strlen(name)==8) { 	
    *result = TablePfr();	
    return true;
  }
	// Symbol of a different symbol-engine
	return false;
}

CString CSymbolEngineTableStats::SymbolsProvided() {
  return "floppct turnpct riverpct avgbetspf tablepfr ";
}