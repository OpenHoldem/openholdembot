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
#include "CSymbolEngineChecksBetsFolds.h"

#include "CBetroundCalculator.h"
#include "CEngineContainer.h"
#include "CScraper.h"
#include "CStringMatch.h"
#include "CSymbolEngineActiveDealtPlaying.h"
#include "CSymbolEngineAutoplayer.h"
#include "CSymbolEngineChipAmounts.h"
#include "CSymbolEngineDealerchair.h"
#include "CSymbolEngineHistory.h"
#include "CSymbolEngineTableLimits.h"
#include "CSymbolEngineUserchair.h"

#include "CTableState.h"

#include "StringFunctions/string_functions.h"

CSymbolEngineChecksBetsFolds::CSymbolEngineChecksBetsFolds() {
	// The values of some symbol-engines depend on other engines.
	// As the engines get later called in the order of initialization
	// we assure correct ordering by checking if they are initialized.
	assert(p_engine_container->symbol_engine_active_dealt_playing() != NULL);
  assert(p_engine_container->symbol_engine_autoplayer() != NULL);
	assert(p_engine_container->symbol_engine_chip_amounts() != NULL);
	assert(p_engine_container->symbol_engine_dealerchair() != NULL);
	assert(p_engine_container->symbol_engine_tablelimits() != NULL);
	assert(p_engine_container->symbol_engine_userchair() != NULL);
	// Also using p_engine_container->symbol_engine_history() one time,
	// but because we use "old" information here
	// there is no dependency on this cycle.
}

CSymbolEngineChecksBetsFolds::~CSymbolEngineChecksBetsFolds() {
}

void CSymbolEngineChecksBetsFolds::InitOnStartup() {
}

void CSymbolEngineChecksBetsFolds::UpdateOnConnection() {
	UpdateOnHandreset();
}

void CSymbolEngineChecksBetsFolds::UpdateOnHandreset() {
	for (int i=kBetroundPreflop; i<=kBetroundRiver; i++) {
		_foldbits[i] = 0;
	}
	_nplayerscallshort  = 0;
	_nopponentsbetting  = 0;
	_nopponentsfolded   = 0;
	_nopponentschecking = 0;
}

void CSymbolEngineChecksBetsFolds::UpdateOnNewRound() {
}

void CSymbolEngineChecksBetsFolds::UpdateOnMyTurn() {
}

void CSymbolEngineChecksBetsFolds::UpdateOnHeartbeat() {
	CalculateNOpponentsCheckingBettingFolded();
	CalculateFoldBits();
}

void CSymbolEngineChecksBetsFolds::CalculateNOpponentsCheckingBettingFolded() {
	_nplayerscallshort  = 0;
	_nopponentsbetting  = 0;
	_nopponentsfolded   = 0;
	_nopponentschecking = 0;
  assert(p_tablemap->nchairs() <= kMaxNumberOfPlayers);
	for (int i=0; i<p_tablemap->nchairs(); i++)	{
		double current_players_bet = p_table_state->Player(i)->_bet.GetValue();
		if (current_players_bet < RaisersBet()
        && p_table_state->Player(i)->HasAnyCards())	{
			_nplayerscallshort++;
		}
		if (i == p_engine_container->symbol_engine_userchair()->userchair()) {
			// No opponent;
			// Nothing more to do
			continue;
		}
		if (current_players_bet > 0) {
			_nopponentsbetting++;
		}
		// Players might have been betting, but folded, so no else for the if
		if ((p_engine_container->symbol_engine_active_dealt_playing()->playersdealtbits() & (1<<(i)))
        && !p_table_state->Player(i)->HasAnyCards())	{
			_nopponentsfolded++;					
		}
		if (p_table_state->Player(i)->HasAnyCards() 
			  && current_players_bet == 0) {
			_nopponentschecking++;
		}
	}
	AssertRange(_nplayerscallshort,  0, kMaxNumberOfOpponentsAtFullRingTable);
	// Using kMaxNumberOfPlayers instead of kMaxNumberOfOpponentsAtFullRingTable below,
	// as it might be that the user is not seated or user-chair not recognized
	AssertRange(_nopponentsbetting,  0, kMaxNumberOfPlayers);
	AssertRange(_nopponentsfolded,   0, kMaxNumberOfPlayers);
	AssertRange(_nopponentschecking, 0, kMaxNumberOfPlayers);
}

double CSymbolEngineChecksBetsFolds::RaisersBet() {
	// The raisers bet is simply the largest bet at the table.
	// So we don't have to know the raisers chair for that.
	double result = 0;
	for (int i=0; i<p_tablemap->nchairs(); i++)	{
		double current_players_bet = p_table_state->Player(i)->_bet.GetValue();
		if (current_players_bet > result && p_table_state->Player(i)->HasAnyCards()) 	{
			result = current_players_bet;
		}
	}
	return result;
}

void CSymbolEngineChecksBetsFolds::CalculateFoldBits() {
	// foldbits (very late, as they depend on the dealt symbols)
	int new_foldbits = 0;
	for (int i=0; i<p_tablemap->nchairs(); i++)	{
		if (!p_table_state->Player(i)->HasAnyCards()) {
			new_foldbits |= k_exponents[i];
		}
	}
	// remove players, who didn't get dealt.
	new_foldbits &= p_engine_container->symbol_engine_active_dealt_playing()->playersdealtbits();
	// remove players, who folded in earlier betting-rounds.
	if (BETROUND >= kBetroundFlop) {
		new_foldbits &= (~_foldbits[kBetroundPreflop]);
	}
	if (BETROUND >= kBetroundTurn) {
		new_foldbits &= (~_foldbits[kBetroundFlop]);
	}
	if (BETROUND >= kBetroundRiver)	{
		new_foldbits &= (~_foldbits[kBetroundTurn]);
	}
	_foldbits[BETROUND] = new_foldbits;
  // We have to be very careful
  // if we accumulate info based on dozens of unstable frames
  // when it is not our turn and the casino potentially
  // updates its display, causing garbabe input that sums up.
  // This affects raisbits, callbits, foldbits.
  // Special fail-safe-code for foldbits: 
  // when it is our turn and we have stable input we try to repair foldbits 
  // of all betrounds and remove all playing players again.
  if (p_engine_container->symbol_engine_autoplayer()->ismyturn()) {
    for (int i = kBetroundPreflop; i <= kBetroundRiver; ++i) {
      _foldbits[i] &= ~p_engine_container->symbol_engine_active_dealt_playing()->playersplayingbits();
    }
  }
}

bool CSymbolEngineChecksBetsFolds::EvaluateSymbol(const CString name, double *result, bool log /* = false */) {
  FAST_EXIT_ON_OPENPPL_SYMBOLS(name);
	if (memcmp(name, "nopponents", 10)==0) {
		if (memcmp(name, "nopponentschecking", 18)==0 && strlen(name)==18) {
			*result = nopponentschecking();
		}	else if (memcmp(name, "nopponentsbetting", 17)==0 && strlen(name)==17) {
			*result = nopponentsbetting();
		}	else if (memcmp(name, "nopponentsfolded", 16)==0 && strlen(name)==16)	{
			*result = nopponentsfolded();
		}	else {
			// Invalid symbol
			return false;
		}
		// Valid symbol
		return true;
	}
	if (memcmp(name, "nplayerscallshort", 17)==0 && strlen(name)==17)	{
		*result = nplayerscallshort();
	}	else if (memcmp(name, "foldbits", 8)==0 && strlen(name)==9) {
		*result = foldbits(name[8]-'0');
  }	else {
		// Symbol of a different symbol-engine
		return false;
	}
	// Valid symbol
	return true;
}

CString CSymbolEngineChecksBetsFolds::SymbolsProvided() {
  CString list = "nopponentschecking "
    "nopponentsbetting nopponentsfolded nplayerscallshort ";
  list += RangeOfSymbols("foldbits%i", kBetroundPreflop, kBetroundRiver);
  return list;
}
