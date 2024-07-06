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
//
// nopponentstruelyraising counts all people who voluntarily bet more than needed,
// especially:
//  * all raisers
//  * the first voluntary better postflop
// but not
//  * the infamous "blind-raisers" (Winholdem)
//  * people posting antes 
//
// nopponentstruelyraising counts only the info that is visible at the table,
// i.e. one orbit (max). Formerly it was well-defined only at out turn,
// but we try to make it well-defined all the time, mainly because
// people don't understand the restrictions of "Raises" (OpenPPl,
// implemented with the use of nopponentsraising:
// http://www.maxinmontreal.com/forums/viewtopic.php?f=297&t=18141)
//
// Now nopponentstruelyraising should count:
//  * a full orbit when it is our turn
//  * a partial orbit from dealer to hero if we did not yet act
//  * a partial orbit behind us if we already acted 
//    (similar to RaisesSinceLastPlay, but might include a bettor)
//  * an orbit after the dealer if the userchair is unknown
//    (not really usable for OpenPPL which updates at our turn, 
//    but at least somewhat meaningful in the debug-tab).
//
//******************************************************************************

#include "stdafx.h"
#include "CSymbolEngineRaisers.h"

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
#include "CSymbolEngineTableLimits.h"
#include "CSymbolEngineUserchair.h"

#include "CTableState.h"

#include "StringFunctions/string_functions.h"

CSymbolEngineRaisers::CSymbolEngineRaisers() {
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
	//
	// Also using p_symbol_engine_debug
	// which doesn't depend on anything and which we want to place last
	// for performance reasons (very rarely used).
	UpdateOnHandreset();
}

CSymbolEngineRaisers::~CSymbolEngineRaisers() {
}

void CSymbolEngineRaisers::InitOnStartup() {
	UpdateOnConnection();
}

void CSymbolEngineRaisers::UpdateOnConnection() {
	UpdateOnHandreset();
}

void CSymbolEngineRaisers::UpdateOnHandreset() {
	for (int i = kBetroundPreflop; i <= kBetroundRiver; i++) {
		_raisbits[i] = 0;
		_lastraised[i] = kUndefined;
	}
	_raischair = kUndefined;
	_firstraiser_chair = kUndefined;
	_nopponentstruelyraising = 0;
}

void CSymbolEngineRaisers::UpdateOnNewRound() {
	_firstraiser_chair = kUndefined;
}

void CSymbolEngineRaisers::UpdateOnMyTurn() {
}

void CSymbolEngineRaisers::UpdateOnHeartbeat() {
	CalculateRaisers();
}

int CSymbolEngineRaisers::ChairInFrontOfFirstPossibleActor() {
	if (p_engine_container->symbol_engine_userchair()->userchair_confirmed()) {
		if (p_engine_container->symbol_engine_autoplayer()->ismyturn()) {
			// userchair known and my turn
			// * a full orbit when it is our turn
			return p_engine_container->symbol_engine_userchair()->userchair();
		}
		else if (p_engine_container->symbol_engine_history()->DidAct()) {
			// userchair known and not my turn, but did act
			// * a partial orbit behind us if we already acted 
			//   (similar to RaisesSinceLastPlay, but might include a bettor)
			return p_engine_container->symbol_engine_userchair()->userchair();
		}
		else {
			// userchair known and not my turn, did not yetact
			// * a partial orbit from dealer to hero if we did not yet act
			return p_engine_container->symbol_engine_dealerchair()->dealerchair();
		}
	}
	else {
		// user-chair unknown
		// * an orbit after the dealer if the userchair is unknown
		//   (not really usable for OpenPPL which updates at our turn, 
		//   but at least somewhat meaningful in the debug-tab).
		return p_engine_container->symbol_engine_dealerchair()->dealerchair();
	}
}

int CSymbolEngineRaisers::FirstPossibleActor() {
	return (ChairInFrontOfFirstPossibleActor() + 1);
}

int CSymbolEngineRaisers::LastPossibleActor() {
	int result = kUndefined;
	if (p_engine_container->symbol_engine_userchair()->userchair_confirmed()) {
		if (p_engine_container->symbol_engine_autoplayer()->ismyturn()) {
			// userchair known and my turn
			// * a full orbit when it is our turn
			result = p_engine_container->symbol_engine_userchair()->userchair();
		}
		else if (p_engine_container->symbol_engine_history()->DidAct()) {
			// userchair known and not my turn, but did act
			// * a partial orbit behind us if we already acted 
			//   (similar to RaisesSinceLastPlay, but might include a bettor)
			result = p_engine_container->symbol_engine_userchair()->userchair();
		}
		else {
			// userchair known and not my turn, did not yetact
			// * a partial orbit from dealer to hero if we did not yet act
			result = p_engine_container->symbol_engine_userchair()->userchair();
		}
	}
	else {
		// user-chair unknown
		// * an orbit after the dealer if the userchair is unknown
		//   (not really usable for OpenPPL which updates at our turn, 
		//   but at least somewhat meaningful in the debug-tab).
		result = p_engine_container->symbol_engine_dealerchair()->dealerchair();
	}
	int first_possible_actor = FirstPossibleActor();
	int nchairs = p_tablemap->nchairs();
	int chairs_inspected = result - first_possible_actor;
	if (result < first_possible_actor) {
		// Make sure tat our simple for-loops don't terminate too early
		// when searching for raisers
		result += nchairs;
	}
	return result;
}

double CSymbolEngineRaisers::MinimumStartingBetCurrentOrbit(bool searching_for_raisers) {
	if (p_engine_container->symbol_engine_history()->DidAct()) {
		int last_known_actor = ChairInFrontOfFirstPossibleActor();
		int nchairs = p_tablemap->nchairs();
		AssertRange(last_known_actor, 0, (nchairs - 1));
		return p_table_state->Player(last_known_actor)->_bet.GetValue();
	}
	// Not yet acted: 0 bb (postflop) or 1 bb (preflop) for the first orbit
	if (p_betround_calculator->betround() > kBetroundPreflop) {
		// Postflop
		return 0.0;
	}
	if (searching_for_raisers) {
		// Preflop:
		// Start with big blind and forget about WinHoldem "blind raisers".
		return p_engine_container->symbol_engine_tablelimits()->bblind();
	}
	// http://www.maxinmontreal.com/forums/viewtopic.php?f=124&t=20361
	// http://www.maxinmontreal.com/forums/viewtopic.php?f=124&t=20387
	// http://www.maxinmontreal.com/forums/viewtopic.php?f=261&t=20226
	// We can't start with 1 bb, as this would recognize the BB as a caller.
	// We can't start with 0 bb either, as this would discard a limping SB
	// as a raiser if we are in BB.
	// The soluition seems to be our bet as a starting point.
	// This discards blinds as raisers if we are outside of the blinds.
	// This discards the BB if we are in SB.
	// This discards a (already folded) SB if we are in BB
	// This counts any limper (including limping SB) if we are in BB.
	// And finally the "fake User" will return 0.0 if the userchair is unknown.
	return (p_table_state->User()->_bet.GetValue());
}

void CSymbolEngineRaisers::CalculateRaisers() {
	_nopponentstruelyraising = 0;
	_temp_raisbits_current_orbit = 0;
	if (BETROUND > 1 && !p_engine_container->symbol_engine_history()->DidAct() && p_engine_container->symbol_engine_history()->prevaction() > 1 && p_engine_container->symbol_engine_autoplayer()->ismyturn()) {
		_lastraised[BETROUND - 1] = p_engine_container->symbol_engine_userchair()->userchair();
	}
	if (BETROUND > 1 && !p_engine_container->symbol_engine_history()->DidAct() && (p_engine_container->symbol_engine_history()->didrais(BETROUND - 1) || p_engine_container->symbol_engine_history()->didswag(BETROUND - 1)) && p_engine_container->symbol_engine_autoplayer()->ismyturn()) {
		_raisbits[BETROUND - 1] = _raisbits[BETROUND - 1] | 1 << p_engine_container->symbol_engine_userchair()->userchair();
	}
	if (p_engine_container->symbol_engine_chip_amounts()->call() <= 0.0) {
		// There are no bets and raises.
		// Skip the calculations to keep the raischair of the previous round.
		// http://www.maxinmontreal.com/forums/viewtopic.php?f=156&t=16806
		write_log(Preferences()->debug_symbolengine(),
			"[CSymbolEngineRaisers] No bet to call, therefore no raises\n");
		return;
	}
	int first_possible_raiser = FirstPossibleActor();
	int last_possible_raiser = LastPossibleActor();
	assert(last_possible_raiser > first_possible_raiser);
	assert(p_engine_container->symbol_engine_debug() != NULL);
	double highest_bet = MinimumStartingBetCurrentOrbit(true);
	write_log(Preferences()->debug_symbolengine(), "[CSymbolEngineRaisers] Searching for raisers from chair %i to %i with a bet higher than %.2f\n",
		first_possible_raiser, last_possible_raiser, highest_bet);
	for (int i = first_possible_raiser; i <= last_possible_raiser; ++i) {
		int chair = i % p_tablemap->nchairs();
		double current_players_bet = p_table_state->Player(chair)->_bet.GetValue();
		write_log(Preferences()->debug_symbolengine(),
			"[CSymbolEngineRaisers] chair %d bet %.2f\n",
			chair, current_players_bet);
		// Raisers are people
		// * with a higher bet than players before them
		// * who are still playing, not counting people who bet/fold in later orbits
		// * either betting/raising postflop or truely raising preflop
		//   (not counting the infamous "blind raisers")
		if (!p_table_state->Player(chair)->HasAnyCards()) {
			write_log(Preferences()->debug_symbolengine(),
				"[CSymbolEngineRaisers] chair %d has no cards.\n", chair);
			continue;
		}
		else if (current_players_bet <= highest_bet) {
			write_log(Preferences()->debug_symbolengine(),
				"[CSymbolEngineRaisers] chair %d is not raising\n", chair);
			continue;
		}
		else if ((p_betround_calculator->betround() == kBetroundPreflop)
			&& (current_players_bet <= p_engine_container->symbol_engine_tablelimits()->bblind())) {
			write_log(Preferences()->debug_symbolengine(),
				"[CSymbolEngineRaisers] chair %d so-called \"blind raiser\". To be ignored.\n", chair);
			continue;
		}
		else if ((p_betround_calculator->betround() == kBetroundPreflop)
			&& p_table_state->Player(chair)->PostingBothBlinds()) {
			write_log(Preferences()->debug_symbolengine(),
				"[CSymbolEngineRaisers] chair %d is posting both blinds at once. To be ignored.\n", chair);
			continue;
		}
		highest_bet = current_players_bet;
		write_log(Preferences()->debug_symbolengine(), "[CSymbolEngineRaisers] Opponent %i raising to %s\n",
			chair, Number2CString(highest_bet));
		// Updating some other symbols
		// Be easy with symbols that get calculated every turn,
		// they can easily be repared...
		++_nopponentstruelyraising;
		_raischair = chair;
		assert(chair != p_engine_container->symbol_engine_userchair()->userchair());
		// First raiser chair detection fail in case hero is bb and last raiser is sb
		// http://www.maxinmontreal.com/forums/viewtopic.php?f=111&t=22541
		// If we did not acted yet and rasier are more than 1 ispecting all palying players (again) 
		if ((!p_engine_container->symbol_engine_history()->DidAct()) && (p_engine_container->symbol_engine_autoplayer()->ismyturn())) {
			write_log(Preferences()->debug_symbolengine(),
				"[CSymbolEngineRaisers] Updating Firstraiser \n");
			if (_nopponentstruelyraising > 1) {
				write_log(Preferences()->debug_symbolengine(),
					"[CSymbolEngineRaisers] More than one raiser acted before our first orbit \n");
				int Flag = 0;
				double MinActualBet = 0;

				for (int h = 0; h < p_tablemap->nchairs(); h++) {

					if (!p_table_state->Player(h)->HasAnyCards())
						continue;

					if (p_table_state->Player(h)->_bet.GetValue() >= (p_engine_container->symbol_engine_tablelimits()->bblind() * 2) && Flag == 0)
					{
						_firstraiser_chair = h;
						Flag = 1;
						MinActualBet = p_table_state->Player(h)->_bet.GetValue();
						write_log(Preferences()->debug_symbolengine(),
							"[CSymbolEngineRaisers] Rasier detected assigned as Firstraiser %d bet %.2f\n",
							h, MinActualBet);
					}

					if ((p_table_state->Player(h)->_bet.GetValue() < MinActualBet) && (p_table_state->Player(h)->_bet.GetValue() >= p_engine_container->symbol_engine_tablelimits()->bblind() * 2) && Flag == 1)
					{
						_firstraiser_chair = h;
						MinActualBet = p_table_state->Player(h)->_bet.GetValue();
						write_log(Preferences()->debug_symbolengine(),
							"[CSymbolEngineRaisers] Updating firstraiser %d bet %.2f\n",
							h, MinActualBet);
					}
				}
			}
			else {
				write_log(Preferences()->debug_symbolengine(),
					"[CSymbolEngineRaisers] FirstRaiser Kundef - Fisrt Riaser to Opponent %i\n", chair);
				_firstraiser_chair = chair;
			}
		}
		AssertRange(_raischair, kUndefined, kLastChair);
		_lastraised[BETROUND] = _raischair;
		// We have to be very careful
		// if we accumulate info based on dozens of unstable frames
		// when it is not our turn and the casino potentially
		// updates its display, causing garbabe input that sums up.
		// This affects raisbits, callbits, foldbits.
		// Special fail-safe-code for raisbits: 
		// We calcuklate stable raisbits and unstable temp_raisbits.
		// When it is our turn we update the stable data.
		// Completeness of historical info of previous betrounds
		// is guaranteed, as after an opponents raise
		// it will be our turn for the update once more again
		// (at least as long as we are playing, and that's all that matters).
		_temp_raisbits_current_orbit |= k_exponents[chair];
		if (p_engine_container->symbol_engine_autoplayer()->ismyturn()) {
			_raisbits[BETROUND] |= _temp_raisbits_current_orbit;
		}
	}
	write_log(Preferences()->debug_symbolengine(), "[CSymbolEngineRaisers] nopponentstruelyraising: %i\n", _nopponentstruelyraising);
	write_log(Preferences()->debug_symbolengine(), "[CSymbolEngineRaisers] raischair: %i\n", _raischair);
	write_log(Preferences()->debug_symbolengine(), "[CSymbolEngineRaisers]  firstraiser_chair (empty if no raiser) : %i\n", _firstraiser_chair);
}

int CSymbolEngineRaisers::raisbits(int betround) {
	AssertRange(betround, kBetroundPreflop, kBetroundRiver);
	if (betround == BETROUND) {
		// Compute the result based on known good data and temp data
		// (potentially unstable, but best what we have)
		return (_raisbits[betround] | _temp_raisbits_current_orbit);
	}
	if ((betround >= kBetroundPreflop)
		&& (betround <= kBetroundRiver)) {
		// Return stable historical data
		return _raisbits[betround];
	}
	else {
		// index out of range, can happen e.g. for bad user-DLLs
		return kUndefined;
	}
}

int CSymbolEngineRaisers::LastRaised(const int round) {
	AssertRange(round, kBetroundPreflop, kBetroundRiver);
	return _lastraised[round];
}

bool CSymbolEngineRaisers::EvaluateSymbol(const CString name, double *result, bool log /* = false */) {
	FAST_EXIT_ON_OPENPPL_SYMBOLS(name);
	if (memcmp(name, "nopponents", 10) == 0) {
		if (memcmp(name, "nopponentstruelyraising", 23) == 0 && strlen(name) == 23) {
			*result = nopponentstruelyraising();
		}
		else {
			// Invalid symbol
			return false;
		}
		// Valid symbol
		return true;
	}
	else if (name == "raischair") {
		*result = raischair();
	}
	else if (name == "firstraiserchair") {
		*result = firstraiser_chair();
	}
	else if (name == "lastraiserchair") {
		*result = raischair();
	}
	else if (memcmp(name, "raisbits", 8) == 0 && strlen(name) == 9) {
		*result = raisbits(name[8] - '0');
	}
	else if (memcmp(name, "lastraised", 10) == 0 && strlen(name) == 11) {
		*result = LastRaised(name[10] - '0');
	}
	else {
		// Symbol of a different symbol-engine
		return false;
	}
	// Valid symbol
	return true;
}

CString CSymbolEngineRaisers::SymbolsProvided() {
	CString list = "nopponentstruelyraising ";
	list += "raischair firstraiserchair lastraiserchair ";
	list += RangeOfSymbols("raisbits%i", kBetroundPreflop, kBetroundRiver);
	list += RangeOfSymbols("lastraised%i", kBetroundPreflop, kBetroundRiver);
	return list;
}
