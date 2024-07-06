//******************************************************************************
//
// This file is part of the OpenHoldem project
//    Source code:           https://github.com/OpenHoldem/openholdembot/
//    Forums:                http://www.maxinmontreal.com/forums/index.php
//    Licensed under GPL v3: http://www.gnu.org/licenses/gpl.html
//
//******************************************************************************
//
// Purpose: Various chair symbols, ecept dealerchair, userchair and raischair,
//   which are of special importance and get handled by other symbol-engines.
//
//******************************************************************************

#include "stdafx.h"
#include "CSymbolEngineChairs.h"

#include "CBetroundCalculator.h"
#include "CEngineContainer.h"
#include "CScraper.h"
#include "CSymbolEngineActiveDealtPlaying.h"
#include "CSymbolEngineChecksBetsFolds.h"
#include "CSymbolEngineHistory.h"
#include "CSymbolEnginePokerAction.h"
#include "CSymbolEnginePositions.h"
#include "CSymbolEngineTableLimits.h"
#include "CSymbolEngineUserchair.h"
#include "CTableState.h"
#include "CTablemap/CTablemap.h"
#include "StringFunctions/string_functions.h"

enum {
  // offset from the dealer, counter-clockwise
  kOffsetDealer = 0,
  kOffsetCutoff,
  kOffsetMP3,
  kOffsetMP2,
  kOffsetMP1,
  kOffsetEP3,
  kOffsetEP2,
  kOffsetEP1,
  // No offsets for SB, BB and UTG.
  // They get calculated clockwise.
};

CSymbolEngineChairs::CSymbolEngineChairs() {
  // The values of some symbol-engines depend on other engines.
	// As the engines get later called in the order of initialization
	// we assure correct ordering by checking if they are initialized.
  assert(p_engine_container->symbol_engine_active_dealt_playing() != NULL);
  assert(p_engine_container->symbol_engine_checks_bets_folds() != NULL);
  assert(p_engine_container->symbol_engine_poker_action() != NULL);
  assert(p_engine_container->symbol_engine_positions() != NULL);
  assert(p_engine_container->symbol_engine_userchair() != NULL);
  assert(p_engine_container->symbol_engine_tablelimits() != NULL);
  // Also using p_engine_container->symbol_engine_history()->DidAct()
// but that's no real dependency, as it is from the past
}

CSymbolEngineChairs::~CSymbolEngineChairs() {
}

void CSymbolEngineChairs::InitOnStartup() {
  UpdateOnConnection();
}

void CSymbolEngineChairs::UpdateOnConnection() {
  _nchairs = p_tablemap->nchairs();
  UpdateOnHandreset();
}

void CSymbolEngineChairs::UpdateOnHandreset() {
  _missing_smallblind = false;
  _missing_smallblind_calculated = false;
}

void CSymbolEngineChairs::UpdateOnNewRound() {
}

void CSymbolEngineChairs::UpdateOnMyTurn() {
  if (!_missing_smallblind_calculated) {
    CalculateMissingSmallBlind();
  }
}

void CSymbolEngineChairs::UpdateOnHeartbeat() {
}

int CSymbolEngineChairs::HeadsupChair() {
  if (p_engine_container->symbol_engine_active_dealt_playing()->nopponentsplaying() != 1) {
    return kUndefined;
  }
  for (int i = 0; i < _nchairs; ++i) {
    if (IsBitSet(p_engine_container->symbol_engine_active_dealt_playing()->opponentsplayingbits(), i)) {
      return i;
    }
  }
  assert(kThisMustNotHappen);
  return kUndefined;
}

int CSymbolEngineChairs::ChairByDealposition(int dealposition) {
  for (int i = 0; i < _nchairs; ++i) {
    if (p_engine_container->symbol_engine_poker_action()->DealPosition(i) == dealposition) {
      return i;
    }
  }
  return kUndefined;
}

int CSymbolEngineChairs::NBlindsAtTheTable() {
  if (MissingSmallBlind()) {
    return 1;
  }
  return 2;
}

int CSymbolEngineChairs::ChairByLogicalPosition(int offset_from_dealer) {
  assert(offset_from_dealer >= kOffsetDealer);
  assert(offset_from_dealer <= kOffsetEP1);
  int dealposition = p_engine_container->symbol_engine_active_dealt_playing()->nplayersdealt() - offset_from_dealer;
  if (dealposition <= NBlindsAtTheTable()) {
    // This is a blind poster
    // His position can't be looked up counter-clockwise from the dealer
    // http://www.maxinmontreal.com/forums/viewtopic.php?f=111&t=20921
    return kUndefined;
  }
  // Regular position
  return ChairByDealposition(dealposition);
}

int CSymbolEngineChairs::UTGChair() {
  if (p_engine_container->symbol_engine_active_dealt_playing()->nplayersdealt() <= 2) {
    // Headsup
    return kUndefined;
  }
  if (MissingSmallBlind()) {
    // BB got dealt first, UTG 2nd
    return ChairByDealposition(2);
  }
  // Normal case: player behind BB
  return ChairByDealposition(3);
}

int CSymbolEngineChairs::SmallStackChair() {
  int result = kUndefined;
  int nchairs = p_tablemap->nchairs();
  int userchair = p_engine_container->symbol_engine_userchair()->userchair();
  // Init with huge number that we will never see in practice
  double smallest_stack = 999999999999;
  for (int i = 0; i < nchairs; ++i) {
    if (i == userchair) {
      // not an opponent
      continue;
    }
    if (!p_table_state->Player(i)->HasAnyCards()) {
      // Not seated, not dealt or folded
      continue;
    }
    if (p_table_state->Player(i)->stack() < smallest_stack) {
      smallest_stack = p_table_state->Player(i)->stack();
      result = i;
    }
  }
  return result;
}

int CSymbolEngineChairs::BigStackChair() {
  int result = kUndefined;
  int nchairs = p_tablemap->nchairs();
  int userchair = p_engine_container->symbol_engine_userchair()->userchair();
  double biggest_stack = 0.0;
  for (int i = 0; i < nchairs; ++i) {
    if (i == userchair) {
      // not an opponent
      continue;
    }
    if (!p_table_state->Player(i)->HasAnyCards()) {
      // Not seated, not dealt or folded
      continue;
    }
    if (p_table_state->Player(i)->stack() > biggest_stack) {
      biggest_stack = p_table_state->Player(i)->stack();
      result = i;
    }
  }
  return result;
}

int CSymbolEngineChairs::SmallBlindChair() {
  if (p_engine_container->symbol_engine_active_dealt_playing()->nplayersdealt() < 2) {
    return kUndefined;
  }
  // Headsup
  if (p_engine_container->symbol_engine_active_dealt_playing()->nplayersdealt() == 2) {
    return ChairByDealposition(2);
  }
  if (MissingSmallBlind()) {
    return kUndefined;
  }
  return ChairByDealposition(1);
}

int CSymbolEngineChairs::BigBlindChair() {
  if (p_engine_container->symbol_engine_active_dealt_playing()->nplayersdealt() < 2) {
    return kUndefined;
  }
  // Headsup
  if (p_engine_container->symbol_engine_active_dealt_playing()->nplayersdealt() == 2) {
    return ChairByDealposition(1);
  }
  if (MissingSmallBlind()) {
    return ChairByDealposition(1);
  }
  return ChairByDealposition(2);
}

bool CSymbolEngineChairs::MissingSmallBlind() {
  if (p_engine_container->symbol_engine_active_dealt_playing()->nplayersseated() < 2) {
    return false;
  }
  if (p_betround_calculator->betround() > kBetroundPreflop) {
    // Gets initialized at our first action preflop
    return _missing_smallblind;
  }
  if (p_engine_container->symbol_engine_history()->DidAct()) {
    return _missing_smallblind;
  }
  // Not yet initialized
  return CalculateMissingSmallBlind() != 0;
}

int CSymbolEngineChairs::CalculateMissingSmallBlind() {
  // True, if SB is missing, false otherwise
  // Should be called at our first action preflop only.
  assert(!_missing_smallblind_calculated);
  if (p_engine_container->symbol_engine_active_dealt_playing()->nopponentsdealt() <= 0) {
    return false;
  }
  int dealposition1_chair = ChairByDealposition(1);
  assert(dealposition1_chair >= 0);
  double currentbet_of_dealposition1_chair = p_table_state->Player(dealposition1_chair)->_bet.GetValue();
  assert(currentbet_of_dealposition1_chair >= 0);
  if (currentbet_of_dealposition1_chair == p_engine_container->symbol_engine_tablelimits()->sblind()) {
    // Small blind found
    return false;
  }
  // If we are NOT the Second player to be dealt and see a bet 
  // of 1 big blind left to the dealer, then it is the big blind (SB missing)
  int dealposition = p_engine_container->symbol_engine_positions()->dealposition();
  double bblind = p_engine_container->symbol_engine_tablelimits()->bblind();
  if ((dealposition != 2) && (currentbet_of_dealposition1_chair == bblind)) {
    return true;
  }
  // Problematic is only the case when I am in "big blind" (Second player to be dealt)
  // * if bet of DealPosition1Chair > 1 bblind then SB raised and is present
  // * if bet = 1 big blind and players "behind me" did act, then SB is present and limped
  // * if bet = 1 big blind and players behind me still to act, then SB is missing
  if ((dealposition == 2) && (currentbet_of_dealposition1_chair > bblind)) {
    return false;
  }
  // Case 2 and 3: precondition: bet = 1 big blind
  if ((dealposition == 2) && PlayersBehindDealPosition2ChairDidAct()) {
    // Everybody acted, small blind limping
    return false;
  }
  if ((dealposition == 2) && !PlayersBehindDealPosition2ChairDidAct()) {
    // First orbit not yet finished
    // person with 1 bb must be the big-blind, small blind is missing
    return true;
  }
  if (currentbet_of_dealposition1_chair <= 0) {
    // Bad input, assume the normal case
    return false;
  }
  // All cases should be handled
  // Can still be reached in simulations with bad input
  return false;
}

bool CSymbolEngineChairs::PlayersBehindDealPosition2ChairDidAct() {
  // Needed for detection of missing small blind.
  // First checking for folded players,
  // because lots of newbies won't scrape-players correctly and mix everything up.
  // This affects calculation of small blind, big blind,
  // all positions and finally lots of other things...
  assert(p_betround_calculator->betround() == kBetroundPreflop);
  if (p_engine_container->symbol_engine_checks_bets_folds()->nopponentsfolded() > 0) {
    return true;
  }
  for (int i = 0; i < _nchairs; ++i) {
    if (p_table_state->Player(i)->HasAnyCards()
      && (p_table_state->Player(i)->_bet.GetValue() == 0.0)) {
      // Playing, but not yet acted
      return false;
    }
  }
  // Everybody acted
  return true;
}

bool CSymbolEngineChairs::EvaluateSymbol(const CString name, double *result, bool log /* = false */) {
  if (name == "missingsmallblind") {
    *result = MissingSmallBlind();
    return true;
  }
  if (RightCharacter(name) != 'r') {
    // Not a chair symbol
    return false;
  }
  if (name == "headsupchair") {
    *result = HeadsupChair();
		return true;
	} 
  // Dealerchair gets handled by CSymbolEngineDealerChair
  if (name == "cutoffchair") {
    *result = ChairByLogicalPosition(kOffsetCutoff);
    return true;
  }
  if (name == "mp3chair") {
    *result = ChairByLogicalPosition(kOffsetMP3);
    return true;
  }
  if (name == "mp2chair") {
    *result = ChairByLogicalPosition(kOffsetMP2);
    return true;
  }
  if (name == "mp1chair") {
    *result = ChairByLogicalPosition(kOffsetMP1);
    return true;
  }
  if (name == "ep3chair") {
    *result = ChairByLogicalPosition(kOffsetEP3);
    return true;
  }
  if (name == "ep2chair") {
    *result = ChairByLogicalPosition(kOffsetEP2);
    return true;
  }
  if (name == "ep1chair") {
    *result = ChairByLogicalPosition(kOffsetEP1);
    return true;
  }
  if (name == "utgchair") {
    *result = UTGChair();
    return true;
  }
  if (name == "smallblindchair") {
    *result = SmallBlindChair();
    return true;
  }
  if (name == "bigblindchair") {
    *result = BigBlindChair();
    return true;
  }
  if (name == "smallstackchair") {
    *result = SmallStackChair();
    return true;
  }
  if (name == "bigstackchair") {
    *result = BigStackChair();
    return true;
  }
  // Symbol of a different symbol-engine
	return false;
}

CString CSymbolEngineChairs::SymbolsProvided() {
  return "headsupchair "
    "smallblindchair bigblindchair "
  	"cutoffchair "
	  "ep3chair ep2chair ep1chair "
	  "mp3chair mp2chair mp1chair "
    "missingsmallblind "
    "smallstackchair bigstackchair ";
}

