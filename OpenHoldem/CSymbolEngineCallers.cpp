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
#include "CSymbolEngineCallers.h"

#include "CBetroundCalculator.h"
#include "CEngineContainer.h"
#include "CScraper.h"
#include "CStringMatch.h"
#include "CSymbolEngineActiveDealtPlaying.h"
#include "CSymbolEngineAutoplayer.h"
#include "CSymbolEngineChipAmounts.h"
#include "CSymbolEngineDealerchair.h"
#include "CSymbolEngineDebug.h"
#include "CSymbolEngineHistory.h"
#include "CSymbolEngineRaisers.h"
#include "CSymbolEngineTableLimits.h"
#include "CSymbolEngineUserchair.h"

#include "CTableState.h"

#include "StringFunctions/string_functions.h"

// Some symbols are only well-defined if it is my turn
#define RETURN_UNDEFINED_VALUE_IF_NOT_MY_TURN { if (!p_engine_container->symbol_engine_autoplayer()->ismyturn()) *result = kUndefined; }

CSymbolEngineCallers::CSymbolEngineCallers() {
	// The values of some symbol-engines depend on other engines.
	// As the engines get later called in the order of initialization
	// we assure correct ordering by checking if they are initialized.
	assert(p_engine_container->symbol_engine_active_dealt_playing() != NULL);
  assert(p_engine_container->symbol_engine_autoplayer() != NULL);
	assert(p_engine_container->symbol_engine_chip_amounts() != NULL);
	assert(p_engine_container->symbol_engine_dealerchair() != NULL);
  assert(p_engine_container->symbol_engine_raisers() != NULL);
	assert(p_engine_container->symbol_engine_tablelimits() != NULL);
	assert(p_engine_container->symbol_engine_userchair() != NULL);
	// Also using p_engine_container->symbol_engine_history() one time,
	// but because we use "old" information here
	// there is no dependency on this cycle.
  //
  // Also using p_symbol_engine_debug
  // which doesn't depend on anything and which we want to place last
  // for performance reasons (very rarely used).
  UpdateOnHandreset();
}

CSymbolEngineCallers::~CSymbolEngineCallers() {
}

void CSymbolEngineCallers::InitOnStartup() {
  UpdateOnConnection();
}

void CSymbolEngineCallers::UpdateOnConnection() {
  _nchairs = p_tablemap->nchairs();
	UpdateOnHandreset();
}

void CSymbolEngineCallers::UpdateOnHandreset() {
	for (int i=kBetroundPreflop; i<=kBetroundRiver; i++) {
		_callbits[i] = 0;
	}
  _nopponentscalling = 0;
  _firstcaller_chair = kUndefined;
  _lastcaller_chair = kUndefined;
  _allinbits_previous_orbit = 0;
}

void CSymbolEngineCallers::UpdateOnNewRound() {
  _firstcaller_chair = kUndefined;
  _lastcaller_chair = kUndefined;
}

void CSymbolEngineCallers::UpdateOnMyTurn() {
}

void CSymbolEngineCallers::UpdateOnHeartbeat() {
  CalculateCallers();
}

void CSymbolEngineCallers::UpdateAfterAutoplayerAction(int autoplayer_action_code) {
  _allinbits_previous_orbit = p_engine_container->symbol_engine_active_dealt_playing()->playersallinbits();
  write_log(Preferences()->debug_symbolengine(),
    "[CSymbolEngineCallers] Players allin from previous orbit: %x\n",
    _allinbits_previous_orbit);
}

void CSymbolEngineCallers::CalculateCallers() {
  write_log(Preferences()->debug_symbolengine(),
    "[CSymbolEngineCallers] CalculateCallers()\n");
  _nopponentscalling = 0;
  _firstcaller_chair = kUndefined;
  _lastcaller_chair = kUndefined;
  int first_possible_raiser = p_engine_container->symbol_engine_raisers()->FirstPossibleActor();
  int last_possible_raiser = p_engine_container->symbol_engine_raisers()->LastPossibleActor() + _nchairs;
  assert(last_possible_raiser > first_possible_raiser);
  assert((last_possible_raiser - first_possible_raiser) <= (2 * _nchairs));
  assert(p_engine_container->symbol_engine_debug() != NULL);
  int chairs_seen = 0;
  double highest_bet = p_engine_container->symbol_engine_raisers()->MinimumStartingBetCurrentOrbit(false);
  write_log(Preferences()->debug_symbolengine(),
    "[CSymbolEngineCallers] current highest bet: %.2f\n", highest_bet);
  write_log(Preferences()->debug_symbolengine(),
    "[CSymbolEngineCallers] Players allin from previous orbit: %x\n",
    _allinbits_previous_orbit);
  for (int i = first_possible_raiser; i <= last_possible_raiser; ++i) {
    ++chairs_seen;
    if (chairs_seen > _nchairs) {
      // Don't count any chair twice
      write_log(Preferences()->debug_symbolengine(),
        "[CSymbolEngineCallers] all chairs inspected\n");
      break;
    }
    int chair = i % _nchairs;
    if (!p_table_state->Player(chair)->HasAnyCards()) {
      // Folded or not dealt, therefore of no interest
      write_log(Preferences()->debug_symbolengine(),
        "[CSymbolEngineCallers] Chair %i folded or not dealt\n", chair);
      continue;
    }
    double current_players_bet = p_table_state->Player(chair)->_bet.GetValue();
    if (current_players_bet == 0) {
      // Player is checking
      write_log(Preferences()->debug_symbolengine(),
        "[CSymbolEngineCallers] Chair %i checking\n", chair);
      continue;
    }
    if (current_players_bet < p_engine_container->symbol_engine_tablelimits()->bblind()) {
      // Player is posting the small-blind or ante
      write_log(Preferences()->debug_symbolengine(),
        "[CSymbolEngineCallers] Chair %i posts SB or ante\n", chair);
      continue;
    }
    if (current_players_bet > highest_bet) {
      // Raiser
      write_log(Preferences()->debug_symbolengine(),
        "[CSymbolEngineCallers] Chair %i raising\n", chair);
      highest_bet = current_players_bet;
      continue;
    }
    if (chair == p_engine_container->symbol_engine_userchair()->userchair()) {
      // User is no opponent
      // and its bet is of no interest either (start or end of search)
      write_log(Preferences()->debug_symbolengine(),
        "[CSymbolEngineCallers] Chair %i userchair\n", chair);
      continue;
    }
    if (current_players_bet == 0) {
      // Allin from a previous betting round
      // or not yet acted
      continue;
    }
    if ((current_players_bet < highest_bet) && !p_table_state->Player(chair)->IsAllin()) {
      // Not a caller
      write_log(Preferences()->debug_symbolengine(),
        "[CSymbolEngineCallers] Chair %i not calling (bet too small)\n", chair);
      // End of search loop reached.
      // We found somebody who was raising or calling in a previous orbit.
      // If we continue then we would find outdated callers,
      // which does not meet the definition of "CallsSinceLastPlay".
      // Aggregated OpenPPL-history-symbols like "Raises" would become wrong
      // if we count some callers twice.
      break;
    }
    if (p_table_state->Player(chair)->IsAllin()) {
      // Raisers already handled
      assert(current_players_bet <= highest_bet);
      if (IsBitSet(_allinbits_previous_orbit, chair)) {
        // Not a caller
        write_log(Preferences()->debug_symbolengine(),
          "[CSymbolEngineCallers] Chair %i allin from previous orbit\n", i);
        continue;
      }
      // Else: calling and being allin
      // http://www.maxinmontreal.com/forums/viewtopic.php?f=217&t=21128
    }
    write_log(Preferences()->debug_symbolengine(),
      "[CSymbolEngineCallers] Chair %i CALLS\n", chair);
    assert(current_players_bet == highest_bet);
		++_nopponentscalling;
    // We have a caller, at least the temporary last one
    _lastcaller_chair = chair;
    if (_firstcaller_chair == kUndefined) {
      // We found the first caller
      _firstcaller_chair = chair;
    }
    // We have to be very careful
    // if we accumulate info based on dozens of unstable frames
    // when it is not our turn and the casino potentially
    // updates its display, causing garbabe input that sums up.
    // This affects raisbits, callbits, foldbits.
    // Special fail-safe-code for callbits: currently none,
    // because it is very unlikely that a mis-scrape
    // causes the bet of a raiser to look like a call.
    int new_callbits = _callbits[BETROUND] | k_exponents[chair];
    _callbits[BETROUND] = new_callbits;
	}
	AssertRange(_callbits[BETROUND], 0, k_bits_all_ten_players_1_111_111_111);
  AssertRange(_nopponentscalling,   0, kMaxNumberOfPlayers);
}

bool CSymbolEngineCallers::EvaluateSymbol(const CString name, double *result, bool log /* = false */) {
  FAST_EXIT_ON_OPENPPL_SYMBOLS(name);
	if (memcmp(name, "nopponentscalling", 17)==0 && strlen(name)==17) {
		*result = nopponentscalling();
		return true;
	}	else if (memcmp(name, "callbits", 8)==0 && strlen(name)==9) {
		*result = callbits(RightDigitCharacterToNumber(name));
    return true;
  } else if (memcmp(name, "firstcallerchair", 16)==0) {
		*result = _firstcaller_chair;
		return true;
	} else if (memcmp(name, "lastcallerchair", 15)==0) {
		*result = _lastcaller_chair;
		return true;
  }
	// Symbol of a different symbol-engine
	return false;
}

CString CSymbolEngineCallers::SymbolsProvided() {
  CString list = "nopponentscalling firstcallerchair lastcallerchair ";
  list += RangeOfSymbols("callbits%i", kBetroundPreflop, kBetroundRiver);
  return list;
}