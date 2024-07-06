//******************************************************************************
//
// This file is part of the OpenHoldem project
//    Source code:           https://github.com/OpenHoldem/openholdembot/
//    Forums:                http://www.maxinmontreal.com/forums/index.php
//    Licensed under GPL v3: http://www.gnu.org/licenses/gpl.html
//
//******************************************************************************
//
// Purpose: Detecting if we play a tournament, especially
//   to enable / disable automatic blind-locking (stability) 
//
//******************************************************************************

#include "stdafx.h"
#include "CSymbolEngineIsTournament.h"

#include "CEngineContainer.h"
#include "CHandresetDetector.h"

#include "CScraper.h"
#include "CSymbolEngineActiveDealtPlaying.h"
#include "CSymbolEngineAutoplayer.h"
#include "CSymbolEngineCasino.h"
#include "CSymbolEngineChecksBetsFolds.h"
#include "CSymbolengineMTTInfo.h"
#include "CSymbolEngineChipAmounts.h"
#include "CSymbolEngineTime.h"
#include "CSymbolEngineTableLimits.h"
#include "CTablemap/CTablemap.h"
#include "CTableState.h"

#include "StringFunctions/string_functions.h"

const double k_lowest_bigblind_ever_seen_in_tournament           = 10.0;
const double k_large_bigblind_probably_later_table_in_tournament = 500.0;

const int k_number_of_tournament_identifiers = 65;
// Partial tournament strings of various casinos
// Sources: PokerStars, and lots of unnamed casinos (by PM)
// These strings have to be lower-cases for comparison
// http://www.maxinmontreal.com/forums/viewtopic.php?f=117&t=16104
const char* k_tournament_identifiers[k_number_of_tournament_identifiers] = {	
  " ante ",		
	" ante:",
	"(ante ",		
	"(ante:",
  "(beginner ds)",
	"buy-in:",		
	"buyin:",
	"buy-in ",
	"buyin ",
	"double ",
	"double-",
  " event",
	"free $",
	"freeroll",
	"garantis",			// french for "guaranteed"
	"gratuit ",			// french for "free"
  " gtd",
	"guaranteed",
  "hyper",
  " knockout",
  " k.o.",
  " level ",
  "miniroll",
	"mise initiale",		// french for "ante"
	" mtt",
	"mtt ",
	"(mtt",
	"multitable",
	"multi-table",
  "no limit hold'em - tbl#",
	" nothing",
	"-nothing",
	"on demand",
	"qualif ",			  // french abbreviation
	"qualificatif",		// french for "qualifier"
	"qualification",
	"qualifier",
  "r/a",            // rebuy and add-on 
	"rebuy",
  "satellite",
  " semifinal",
  " series",
	"shootout ",
	"sit and go",
	"sit&go",
	"sit & go",
	"sit 'n go",
	"sit'n go",
  "sit n go",
	" sng",
	"sng ",
	"(sng",
	"sup turbo",
	"super turbo",
	"superturbo",
    "super-turbo",
    "TBL#",
    "TBL #",
	"ticket ",
	"tour ",
  "tourney",
  "tournament",
  "triple-up",
  "triple up",
  "10K chips",
};

const int kNumberOfDONIdentifiers = 9;
const char* kDONIdentifiers[kNumberOfDONIdentifiers] = {	
	"(beginner ds)",
  "double ",
	"double-",
	" nothing",
	"-nothing",
	"shootout ",
	"ticket ",
  // Triple-up: not really DON, but a liitle bit like DON.
  "triple-up",
  "triple up",
};

const int kNumberOfMTTIdentifiers = 26;
const char* kMTTIdentifiers[kNumberOfMTTIdentifiers] = {
	" event",
  "free $",
	"freeroll",
	"garantis",			// french for "guaranteed"
	"gratuit ",			// french for "free"
  " gtd",
	"guaranteed",
  " knockout",
  " k.o.",
	"miniroll",
	" mtt",
	"mtt ",
	"(mtt",
	"multitable",
	"multi-table",
  "no limit hold'em - tbl#",
	"qualif ",			  // french abbreviation
	"qualificatif",		// french for "qualifier"
	"qualification",
	"qualifier",
  "r/a",            // rebuz and add-on 
	"rebuy",
  "satellite",
  " semifinal",
  " series",
  "10K chips",
};

CSymbolEngineIsTournament::CSymbolEngineIsTournament() {
	// The values of some symbol-engines depend on other engines.
	// As the engines get later called in the order of initialization
	// we assure correct ordering by checking if they are initialized.
	assert(p_engine_container->symbol_engine_active_dealt_playing() != NULL);
	assert(p_engine_container->symbol_engine_autoplayer() != NULL);
	assert(p_engine_container->symbol_engine_casino() != NULL);
  assert(p_engine_container->symbol_engine_checks_bets_folds() != NULL);
  assert(p_engine_container->symbol_engine_chip_amounts() != NULL);
	assert(p_engine_container->symbol_engine_mtt_info() != NULL);
	assert(p_engine_container->symbol_engine_tablelimits() != NULL);
	assert(p_engine_container->symbol_engine_time() != NULL);
}

CSymbolEngineIsTournament::~CSymbolEngineIsTournament() {
}

void CSymbolEngineIsTournament::InitOnStartup() {
	UpdateOnConnection();
}

void CSymbolEngineIsTournament::UpdateOnConnection() {
	_istournament    = kUndefined;
	_decision_locked = false;
}

void CSymbolEngineIsTournament::UpdateOnHandreset() {
}

void CSymbolEngineIsTournament::UpdateOnNewRound() {
}

void CSymbolEngineIsTournament::UpdateOnMyTurn() {
	TryToDetectTournament();
}

void CSymbolEngineIsTournament::UpdateOnHeartbeat() {
  if (_istournament == kUndefined) {
    // Beginning pf session and not yet sure.
    // Temporary maximum effort on every heartbeat
    TryToDetectTournament();
  }
}

bool CSymbolEngineIsTournament::BetsAndBalancesAreTournamentLike() {
  // "Beautiful" numbers => tournament
  // This condition does unfortunatelly only work for the first and final table in an MTT,
  // not necessarily for other late tables (fractional bets, uneven sums).
  double sum_of_all_chips = 0.0;
  for (int i=0; i<p_tablemap->nchairs(); i++) {
	  if (p_table_state->Player(i)->active()==true) {
	    sum_of_all_chips += p_table_state->Player(i)->_balance.GetValue();
		  sum_of_all_chips += p_table_state->Player(i)->_bet.GetValue();}
  }
 write_log(Preferences()->debug_istournament(), "[CSymbolEngineIsTournament] Sum of chips at the table: %.2f\n",
    sum_of_all_chips);
  if (sum_of_all_chips != int(sum_of_all_chips)) {
    // Franctional number.
    // Looks like a cash-game.
    return false;
  }
  if ((int(sum_of_all_chips) % 100) != 0) {
    // Not a multiplicity of 100.
    // Probably not a tournament.
    return false;
  }
  if ((int(sum_of_all_chips) % p_engine_container->symbol_engine_active_dealt_playing()->nplayersactive()) != 0)    { 
    // Not a multiplicity of the active players.
    // Probably not a tournament.
    return false;
  }
  return true;
}

bool CSymbolEngineIsTournament::AntesPresent() {
	// Antes are present, if all players are betting 
	// and at least 3 have a bet smaller than SB 
	// (remember: this is for the first few hands only).
	if ((p_engine_container->symbol_engine_checks_bets_folds()->nopponentsbetting() + 1)
		  < p_engine_container->symbol_engine_active_dealt_playing()->nplayersseated()) {
		return false;
	}
	int players_with_antes = 0;
	for (int i=0; i<p_tablemap->nchairs(); i++) {
		double players_bet = p_table_state->Player(i)->_bet.GetValue();
		if ((players_bet > 0) && (players_bet < p_engine_container->symbol_engine_tablelimits()->sblind())) {
			players_with_antes++;
		}
	}
	return (players_with_antes >= 3);
}

bool CSymbolEngineIsTournament::TitleStringContainsIdentifier(
    const char *identifiers[], int number_of_identifiers) {
	CString title = p_table_state->TableTitle()->Title();
	title = title.MakeLower();
	for (int i=0; i<number_of_identifiers; i++) {
    assert(identifiers[i] != "");
		if (title.Find(identifiers[i]) != -1) 	{
			return true;
		}
	}
	return false;
}

void CSymbolEngineIsTournament::TryToDetectTournament() {
	if (_istournament != kUndefined) {
		write_log(Preferences()->debug_istournament(), "[CSymbolEngineIsTournament] Currently istournament = %s\n", Bool2CString(_istournament));
  } else {
		write_log(Preferences()->debug_istournament(), "[CSymbolEngineIsTournament] Currently istournament = unknown\n");
  }
	// Don't do anything if we are already sure.
	// Don't mix things up.
	if (_decision_locked) {
		write_log(Preferences()->debug_istournament(), "[CSymbolEngineIsTournament] decision already locked\n");
		return;
	}
  if (p_table_state->_s_limit_info.buyin() > 0) {
   write_log(Preferences()->debug_istournament(), "[CSymbolEngineIsTournament] Tournament due to positive buyin detected\n");
    _istournament = true;
    _decision_locked = true;
    return;
  }
	if (p_engine_container->symbol_engine_mtt_info()->ConnectedToAnyTournament()) {
		write_log(Preferences()->debug_istournament(), "[CSymbolEngineIsTournament] MTT tournament detected\n");
		_istournament    = true;
		_decision_locked = true;
		return;
	}
	// If we have more than 2 hands played we should be sure
	// and stick to our decision, whatever it is (probably cash-game).
	// Also checking for (elapsedauto < elapsed). i.e. at least one action
	// since connection, as handsplayed does not reset if we play multiple games.
	if ((_istournament != kUndefined)
		  && (p_handreset_detector->hands_played() > 2)
		  && (p_engine_container->symbol_engine_time()->elapsedauto() < p_engine_container->symbol_engine_time()->elapsed())) {
		write_log(Preferences()->debug_istournament(), "[CSymbolEngineIsTournament] Enough hands played; locking current value\n");
		_decision_locked = true;
		return;
	}
  // If we play at DDPoker the game is a tournament,
  // even though it can~t be detected by titlestring.
  if (p_engine_container->symbol_engine_casino()->ConnectedToDDPoker()) {
    write_log(Preferences()->debug_istournament(), "[CSymbolEngineIsTournament] DDPoker tournament\n");
    _istournament    = true;
		_decision_locked = true;
		return;
  }
  // If the title-string looks like a tournament then it is a tournament.
  // This should be checked before the size of the blinds,
  // because incorrectly detecting a cash-game as tournament
  // does less harm than vice versa (blind-locking).
  // And there was a problem if during the sit-down-phase 
  // of a tournament there were no blinds to be detected:
  // http://www.maxinmontreal.com/forums/viewtopic.php?f=294&t=17625&start=30#p125608
	if (TitleStringContainsIdentifier(k_tournament_identifiers,
      k_number_of_tournament_identifiers))	{
		write_log(Preferences()->debug_istournament(), "[CSymbolEngineIsTournament] Table title looks like a tournament\n");
		_istournament    = true;
		_decision_locked = true;
		return;
	}
	// If the blinds are "too low" then we play a cash-game.
  // Only consider this option if a game is going on (playersplaying)
  // to avoid problems with no blinds during the sit-down-phase 
  // of a tournament.
  if (p_engine_container->symbol_engine_active_dealt_playing()->nplayersplaying() < 2) {
    write_log(Preferences()->debug_istournament(), "[CSymbolEngineIsTournament] Can't consider the blinds -- too few people playing.\n");
    return;
  }
  double bigblind = p_engine_container->symbol_engine_tablelimits()->bblind();
	if ((bigblind > 0) && (bigblind < k_lowest_bigblind_ever_seen_in_tournament)) {
	  write_log(Preferences()->debug_istournament(), "[CSymbolEngineIsTournament] Blinds \"too low\"; this is a cash-game\n");
	  _istournament    = false;
		// We don't lock the decision here,
    // as the blind-values at some crap-casinos need to be scraped
    // and might be unreliable for the first hand.
    // Locking will happen automatically after N hands
    // with more reliable info.
    // http://www.maxinmontreal.com/forums/viewtopic.php?f=110&t=18266&p=130775#p130772
		return;
  }
  // If it is ManualMode, then we detect it by title-string "tourney".
  // High blinds (default) don~t make it a tournament.
  // Therefore don't continue.
  if (p_engine_container->symbol_engine_casino()->ConnectedToManualMode()) {
		write_log(Preferences()->debug_istournament(), "[CSymbolEngineIsTournament] ManualMode, but no tournament identifier\n");
    return;
  }
	// If there are antes then it is a tournament.
	if (AntesPresent())	{
		write_log(Preferences()->debug_istournament(), "[CSymbolEngineIsTournament] Game with antes; therefore tournament\n");
		_istournament    = true;
		_decision_locked = true;
		return;
	}
	// If bets and balances are "tournament-like", then it is a tournament
	if (BetsAndBalancesAreTournamentLike())	{
		write_log(Preferences()->debug_istournament(), "[CSymbolEngineIsTournament] Bets and balances look like tournament\n");
		_istournament    = true;
		_decision_locked = true;
		return;
	}
	// Not yet sure, but we have handled so far:
	// * all lower stakes cash-games
	// * all SNGs / MTTs with good title-string
	// * all SNGs and starting tables of MTTs (by bets and balances)
	// * some later tables of MTTs (by antes)
	// * some more later tables of MTTs (if no reconnection is necessary)
	// This leaves us with only 2 cases
	// * medium and high-stakes cash-games ($5/$10 and above)
	// * some (very few) later tables of MTTs
	if (bigblind > k_large_bigblind_probably_later_table_in_tournament) {
		// Probably tournament, but not really sure yet,
		// so don't lock the decision.
		write_log(Preferences()->debug_istournament(), "[CSymbolEngineIsTournament] bigblind very large; probably tournament\n");
		_istournament = true;
		return;
	}
	// Smaller bigblinds (< $500) will be considered cash-games, 
	// because they are too small for later tables of tournaments
	_istournament    = false;
	_decision_locked = true;
	write_log(Preferences()->debug_istournament(), "[CSymbolEngineIsTournament] high-stakes cash-game up to $200\\$400\n");
	// The only case that can go wrong now:
	// High-stakes cash-games ($250/$500 and above) can be recognized 
	// as tournaments. 
	// Main consequence: blinds won't be locked for the entire session,
	// but only for the current hand. Does this hurt much?
}

bool CSymbolEngineIsTournament::IsMTT() {
  if (!istournament()) return false;
  if (TitleStringContainsIdentifier(kMTTIdentifiers, kNumberOfMTTIdentifiers)) return true;
  if (p_engine_container->symbol_engine_mtt_info()->ConnectedToMTT()) return true;
  return false;
}

bool CSymbolEngineIsTournament::IsSNG() {
  return (istournament() && !IsMTT() && !IsDON());
}

bool CSymbolEngineIsTournament::IsDON() {
  if (!istournament()) return false;
  if (TitleStringContainsIdentifier(kDONIdentifiers, kNumberOfDONIdentifiers)) return true;
  return false;
}

bool CSymbolEngineIsTournament::EvaluateSymbol(const CString name, double *result, bool log /* = false */) { 
  FAST_EXIT_ON_OPENPPL_SYMBOLS(name);
  if (memcmp(name, "is", 2)!=0)  {
    // Symbol of a different symbol-engine
    return false;
  }
	if (memcmp(name, "istournament", 12)==0 && strlen(name)==12) {
		*result = istournament();
		// Valid symbol
		return true;
	}
  if (memcmp(name, "issng", 5)==0 && strlen(name)==5) {
		*result = IsSNG();
		// Valid symbol
		return true;
	}
  if (memcmp(name, "ismtt", 5)==0 && strlen(name)==5) {
		*result = IsMTT();
		// Valid symbol
		return true;
	}
  if (memcmp(name, "isdon", 5)==0 && strlen(name)==5) {
		*result = IsDON();
		// Valid symbol
		return true;
	}
  // Symbol of a different symbol-engine
	return false;
}

CString CSymbolEngineIsTournament::SymbolsProvided() {
  return "istournament issng ismtt isdon ";
}
