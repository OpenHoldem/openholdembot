//*******************************************************************************
//
// This file is part of the OpenHoldem project
//   Download page:         http://code.google.com/p/openholdembot/
//   Forums:                http://www.maxinmontreal.com/forums/index.php
//   Licensed under GPL v3: http://www.gnu.org/licenses/gpl.html
//
//*******************************************************************************
//
// Purpose:
//
//*******************************************************************************

#include "stdafx.h"
#include "CSymbolEngineRaisers.h"

#include <assert.h>
#include "CBetroundCalculator.h"
#include "CScraper.h"
#include "CScraperAccess.h"
#include "CStringMatch.h"
#include "CSymbolEngineActiveDealtPlaying.h"
#include "CSymbolEngineAutoplayer.h"
#include "CSymbolEngineChipAmounts.h"
#include "CSymbolEngineDealerchair.h"
#include "CSymbolEngineHistory.h"
#include "CSymbolEngineTableLimits.h"
#include "CSymbolEngineUserchair.h"
#include "CPreferences.h"
#include "CTableState.h"
#include "NumericalFunctions.h"
#include "StringFunctions.h"

CSymbolEngineRaisers *p_symbol_engine_raisers = NULL;

// Some symbols are only well-defined if it is my turn
#define RETURN_UNDEFINED_VALUE_IF_NOT_MY_TURN { if (!p_symbol_engine_autoplayer->ismyturn()) *result = kUndefined; }

CSymbolEngineRaisers::CSymbolEngineRaisers() {
	// The values of some symbol-engines depend on other engines.
	// As the engines get later called in the order of initialization
	// we assure correct ordering by checking if they are initialized.
	assert(p_symbol_engine_active_dealt_playing != NULL);
  assert(p_symbol_engine_autoplayer != NULL);
	assert(p_symbol_engine_chip_amounts != NULL);
	assert(p_symbol_engine_dealerchair != NULL);
	assert(p_symbol_engine_tablelimits != NULL);
	assert(p_symbol_engine_userchair != NULL);
	// Also using p_symbol_engine_history one time,
	// but because we use "old" information here
	// there is no dependency on this cycle.
}

CSymbolEngineRaisers::~CSymbolEngineRaisers() {
}

void CSymbolEngineRaisers::InitOnStartup() {
}

void CSymbolEngineRaisers::ResetOnConnection() {
	ResetOnHandreset();
}

void CSymbolEngineRaisers::ResetOnHandreset() {
	// callbits, raisbits, etc.
	for (int i=kBetroundPreflop; i<=kBetroundRiver; i++) {
		_callbits[i] = 0;
		_raisbits[i] = 0;
		_foldbits[i] = 0;
    _lastraised[i] = kUndefined;
	}
	_raischair = kUndefined;
	_firstraiser_chair = kUndefined;
	_nplayerscallshort  = 0;
	_nopponentsbetting  = 0;
	_nopponentstruelyraising = 0;
	_nopponentsfolded   = 0;
	_nopponentschecking = 0;
}

void CSymbolEngineRaisers::ResetOnNewRound() {
	_firstraiser_chair = kUndefined;
}

void CSymbolEngineRaisers::ResetOnMyTurn() {
	CalculateRaisers();
}

void CSymbolEngineRaisers::ResetOnHeartbeat() {
	CalculateNOpponentsCheckingBettingFolded();
	CalculateFoldBits();
}

double CSymbolEngineRaisers::LastOrbitsLastRaisersBet() {
	// Not yet acted: 0
	if (!p_symbol_engine_history->DidAct()) {
    if (p_betround_calculator->betround() == kBetroundPreflop) {
      // Preflop:
      // Start with big blind and forget about former blind raisers
      return p_symbol_engine_tablelimits->bblind();
    } else {
      // Postflop
		  return 0.0;
    }
	}
	if (p_table_state->User()->HasKnownCards())	{
		// Otherwise: either we are the raiser (highest bet)
		// Or we called the raise (highest bet too)
		return p_table_state->User()->_bet;
	}
	// Otherwise meaningless
	return 0.0;
}

void CSymbolEngineRaisers::CalculateRaisers() {
	_nopponentstruelyraising = 0;
	if (p_symbol_engine_chip_amounts->call() <= 0.0) 	{
		// There are no bets and raises.
		// Skip the calculations to keep the raischair of the previous round.
		// http://www.maxinmontreal.com/forums/viewtopic.php?f=156&t=16806
    write_log(preferences.debug_symbolengine(), 
      "[CSymbolEngineRaisers] No bet to call, therefore no raises\n");
		return;
	}
	// Raischair, nopponentstruelyraising, raisbits
	int first_possible_raiser = FirstPossibleRaiser();
	int last_possible_raiser  = LastPossibleRaiser();
	double highest_bet = LastOrbitsLastRaisersBet();
  write_log(preferences.debug_symbolengine(), "[CSymbolEngineRaisers] Searching for raisers from chair %i to %i with a bet higher than %.2f\n",
		first_possible_raiser, last_possible_raiser, highest_bet); 
	for (int i=first_possible_raiser; i<=last_possible_raiser; ++i) {
		int chair = i % p_tablemap->nchairs();
		double current_players_bet = p_table_state->_players[chair]._bet;
    write_log(preferences.debug_symbolengine(), 
      "[CSymbolEngineRaisers] chair %d bet %.2f\n",
      chair, current_players_bet);
		// Raisers are people
		// * with a higher bet than players before them
		// * who are still playing, not counting people who bet/fold in later orbits
    // * either betting/raising postflop or truely raising preflop
    //   (not counting the infamous "blind raisers")
    if (!p_table_state->_players[chair].HasAnyCards()) {
      write_log(preferences.debug_symbolengine(), 
        "[CSymbolEngineRaisers] chair %d has no cards.\n", chair);
      continue;
    } else if (current_players_bet <= highest_bet) {
      write_log(preferences.debug_symbolengine(), 
        "[CSymbolEngineRaisers] chair %d is not raising\n", chair);
      continue;
    } else if ((p_betround_calculator->betround() == kBetroundPreflop)
				&& (current_players_bet <= p_symbol_engine_tablelimits->bblind())) {
      write_log(preferences.debug_symbolengine(), 
        "[CSymbolEngineRaisers] chair %d so-called \"blind raiser\". To be ignored.\n", chair);
      continue;
    }
    else if ((p_betround_calculator->betround() == kBetroundPreflop)
      && p_table_state->_players[chair].PostingBothBlinds()) {
      write_log(preferences.debug_symbolengine(), 
        "[CSymbolEngineRaisers] chair %d is posting both blinds at once. To be ignored.\n", chair);
      continue;
    }
		highest_bet = current_players_bet;
    write_log(preferences.debug_symbolengine(), "[CSymbolEngineRaisers] Opponent %i raising to %s\n",
			chair, Number2CString(highest_bet));
		_raischair = chair;
		int new_raisbits = _raisbits[BETROUND] | k_exponents[chair];
		_raisbits[BETROUND] = new_raisbits;
		assert(chair != USER_CHAIR);
			if (_firstraiser_chair == kUndefined ){
			_firstraiser_chair = chair;
		}
		++_nopponentstruelyraising;
	}
	AssertRange(_raischair, kUndefined, kLastChair);
  _lastraised[BETROUND] = _raischair;
	write_log(preferences.debug_symbolengine(), "[CSymbolEngineRaisers] nopponentstruelyraising: %i\n", _nopponentstruelyraising);
	write_log(preferences.debug_symbolengine(), "[CSymbolEngineRaisers] raischair: %i\n", _raischair);
	write_log(preferences.debug_symbolengine(), "[CSymbolEngineRaisers]  firstraiser_chair (empty if no raiser) : %i\n", _firstraiser_chair);
}

int CSymbolEngineRaisers::FirstPossibleRaiser() {
  // For nopponentstruelyraising we can always 
  // start behind the userchair and search clockwise
  // until the last player in front of the user.
  // This works both preflop and postflop,
  // both in and out of the blinds,
  // both before and after our first action. ;-)
	int first_possible_raiser = (USER_CHAIR + 1) % p_tablemap->nchairs();
	return first_possible_raiser;
}

int CSymbolEngineRaisers::LastPossibleRaiser() {
	// For technical reasons (for-loop) we handle the modulo-operation 
	// inside the loop and not here.
	return (FirstPossibleRaiser() + p_tablemap->nchairs() - 1);
}

void CSymbolEngineRaisers::CalculateNOpponentsCheckingBettingFolded()
{
	_nplayerscallshort  = 0;
	_nopponentsbetting  = 0;
	_nopponentsfolded   = 0;
	_nopponentschecking = 0;

  assert(p_tablemap->nchairs() <= kMaxNumberOfPlayers);
	for (int i=0; i<p_tablemap->nchairs(); i++)
	{
		double current_players_bet = p_table_state->_players[i]._bet;
		if (current_players_bet < RaisersBet()
      && p_table_state->_players[i].HasAnyCards())
		{
			_nplayerscallshort++;
		}
		if (i == USER_CHAIR)
		{
			// No opponent;
			// Nothing more to do
			continue;
		}
		if (current_players_bet > 0) 
		{
			_nopponentsbetting++;
		}
		// Players might have been betting, but folded, so no else for the if
		if ((p_symbol_engine_active_dealt_playing->playersdealtbits() & (1<<(i)))
        && !p_table_state->_players[i].HasAnyCards())	{
			_nopponentsfolded++;					
		}
		if (p_table_state->_players[i].HasAnyCards() 
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

double CSymbolEngineRaisers::RaisersBet()
{
	// The raisers bet is simply the largest bet at the table.
	// So we don't have to know the raisers chair for that.
	double result = 0;
	for (int i=0; i<p_tablemap->nchairs(); i++)
	{
		double current_players_bet = p_table_state->_players[i]._bet;
		if (current_players_bet > result
      && p_table_state->_players[i].HasAnyCards())
		{
			result = current_players_bet;
		}
	}
	return result;
}

void CSymbolEngineRaisers::CalculateFoldBits()
{
	// foldbits (very late, as they depend on the dealt symbols)
	int new_foldbits = 0;
	for (int i=0; i<p_tablemap->nchairs(); i++)
	{
		if (!p_table_state->_players[i].HasAnyCards()) {
			new_foldbits |= k_exponents[i];
		}
	}
	// remove players, who didn't get dealt.
	new_foldbits &= p_symbol_engine_active_dealt_playing->playersdealtbits();

	// remove players, who folded in earlier betting-rounds.
	if (BETROUND >= kBetroundFlop)
	{
		new_foldbits &= (~_foldbits[kBetroundPreflop]);
	}
	if (BETROUND >= kBetroundTurn)
	{
		new_foldbits &= (~_foldbits[kBetroundFlop]);
	}
	if (BETROUND >= kBetroundRiver)   
	{
		new_foldbits &= (~_foldbits[kBetroundTurn]);
	}
	_foldbits[BETROUND] = new_foldbits;
}

int CSymbolEngineRaisers::LastRaised(const int round) {
  AssertRange(round, kBetroundPreflop, kBetroundRiver);
  return _lastraised[round];
}

bool CSymbolEngineRaisers::EvaluateSymbol(const char *name, double *result, bool log /* = false */) {
  FAST_EXIT_ON_OPENPPL_SYMBOLS(name);
	if (memcmp(name, "nopponents", 10)==0) {
		if (memcmp(name, "nopponentschecking", 18)==0 && strlen(name)==18) {
			*result = nopponentschecking();
		}	else if (memcmp(name, "nopponentstruelyraising", 23)==0 && strlen(name)==23) {
      RETURN_UNDEFINED_VALUE_IF_NOT_MY_TURN
			*result = nopponentstruelyraising();
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
    RETURN_UNDEFINED_VALUE_IF_NOT_MY_TURN
		*result = nplayerscallshort();
	}
	else if (memcmp(name, "raischair", 9) == 0 && strlen(name) == 9) {
		*result = raischair();
	}else if (memcmp(name, "firstraiser_chair", 17) == 0) {
		*result = firstraiser_chair();
	}	else if (memcmp(name, "raisbits", 8)==0 && strlen(name)==9) {
    RETURN_UNDEFINED_VALUE_IF_NOT_MY_TURN
		*result = raisbits(name[8]-'0');
	}	else if (memcmp(name, "foldbits", 8)==0 && strlen(name)==9) {
    RETURN_UNDEFINED_VALUE_IF_NOT_MY_TURN
		*result = foldbits(name[8]-'0');
	} else if (memcmp(name, "lastraised", 10)==0 && strlen(name)==11) { 
    *result = LastRaised(name[10]-'0');
  }	else {
		// Symbol of a different symbol-engine
		return false;
	}
	// Valid symbol
	return true;
}

CString CSymbolEngineRaisers::SymbolsProvided() {
  CString list = "nopponentschecking "
    "nopponentstruelyraising nopponentsbetting nopponentsfolded "
    "nplayerscallshort raischair firstraiser_chair  ";
  list += RangeOfSymbols("raisbits%i", kBetroundPreflop, kBetroundRiver);
  list += RangeOfSymbols("foldbits%i", kBetroundPreflop, kBetroundRiver);
  list += RangeOfSymbols("lastraised%i", kBetroundPreflop, kBetroundRiver);
  return list;
}
