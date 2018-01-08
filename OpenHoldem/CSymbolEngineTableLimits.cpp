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
#include "CSymbolEngineTableLimits.h"

#include <assert.h>
#include "CBetroundCalculator.h"
#include "CBlindGuesser.h"
#include "CCasinoInterface.h"
#include "CEngineContainer.h"
#include "CPreferences.h"
#include "CScraper.h"
#include "CSymbolEngineDealerchair.h"
#include "CSymbolEngineGameType.h"
#include "CSymbolEngineIsTournament.h"
#include "CTableState.h"
#include "FloatingPoint_Comparisions.h"
#include "Median.h"
#include "..\DLLs\StringFunctions_DLL\string_functions.h"

CSymbolEngineTableLimits::CSymbolEngineTableLimits() {
	// The values of some symbol-engines depend on other engines.
	// As the engines get later called in the order of initialization
	// we assure correct ordering by checking if they are initialized.
	assert(p_engine_container->symbol_engine_dealerchair() != NULL);
	//
	// Actually this symbol-engine also depends on 
	// p_symbol_engine_is_tournament.
	// This is a circular dependency, but this does not really matter,
	// as the value of istournament() should be constant for the
	// entire session, so it does not matter, if we use old values
	// from the last heartbeat.
  //
  // Same for CSymbolEngineGameType
}

CSymbolEngineTableLimits::~CSymbolEngineTableLimits() {
}

void CSymbolEngineTableLimits::InitOnStartup() {
	UpdateOnConnection();
}

void CSymbolEngineTableLimits::UpdateOnConnection() {
	write_log(preferences.debug_table_limits(), 
    "[CSymbolEngineTableLimits] UpdateOnConnection()\n");
	number_of_saved_tablelimits = 0;
	for (int i=0; i<k_number_of_hands_to_autolock_blinds_for_cashgames; i++)	{
		tablelimits_first_N_hands_sblind[i] = 0;
		tablelimits_first_N_hands_bblind[i] = 0;
		tablelimits_first_N_hands_bbet[i]   = 0;
	}
	_ante = 0;
  tablelimit_best_guess.bbet = 0;
  tablelimit_best_guess.bblind  = 0;
  tablelimit_best_guess.sblind = 0;
  tablelimit_locked_for_complete_session.bbet = 0;
  tablelimit_locked_for_complete_session.bblind = 0;
	tablelimit_locked_for_complete_session.sblind = 0;
  blinds_locked_for_complete_session = false;
  // UpdateOnHandreset also cares about tablelimit_locked_for_current_hand
	UpdateOnHandreset();
}

void CSymbolEngineTableLimits::UpdateOnHandreset() {
	write_log(preferences.debug_table_limits(), 
    "[CSymbolEngineTableLimits] UpdateOnHandreset()\n");
	blinds_locked_for_current_hand = false;
	tablelimit_locked_for_current_hand.sblind = 0;
	tablelimit_locked_for_current_hand.bblind = 0;
	tablelimit_locked_for_current_hand.bbet   = 0;
	// We have to reset the known good values also,
	// * as the blinds could change (tournament)
	// * as they could be wrong and we would assume,
	//   that we know the blinds and don't calculate anything.
	tablelimit_best_guess.sblind = 0;
	tablelimit_best_guess.bblind = 0;
	tablelimit_best_guess.bbet   = 0;
}

void CSymbolEngineTableLimits::UpdateOnNewRound() {
}

void CSymbolEngineTableLimits::UpdateOnMyTurn() {
}

void CSymbolEngineTableLimits::UpdateOnHeartbeat() {
	write_log(preferences.debug_table_limits(), 
    "[CSymbolEngineTableLimits] UpdateOnHeartbeat()\n");
  if (TableLimitsNeedToBeComputed()) {
    CBlindGuesser _blind_guesser;
    _blind_guesser.Guess(&tablelimit_best_guess.sblind,
      &tablelimit_best_guess.bblind,
      &tablelimit_best_guess.bbet);
    if (p_table_state->_s_limit_info.ante() > 0) {
      if (p_table_state->_s_limit_info.ante() > sblind()) {
        write_log(k_always_log_errors,
          "[CSymbolEngineTableLimits] WARNING! ante larger than small blind\n");
        write_log(k_always_log_errors,
          "[CSymbolEngineTableLimits] This looks like a problem in your tablemap.\n");
        _ante = kUndefinedZero;
      } else {
        _ante = p_table_state->_s_limit_info.ante();
      }
    }
    AutoLockBlinds();
  }
}

bool CSymbolEngineTableLimits::TableLimitsNeedToBeComputed() {
  if (blinds_locked_for_current_hand 
      || blinds_locked_for_complete_session) {
    write_log(preferences.debug_table_limits(), 
      "[CSymbolEngineTableLimits] TableLimitsNeedToBeComputed() false, because locked\n");
    return false;
  }
  write_log(preferences.debug_table_limits(), 
      "[CSymbolEngineTableLimits] TableLimitsNeedToBeComputed() true, because not locked\n");
  return true;
}

void CSymbolEngineTableLimits::AutoLockBlindsForCashgamesAfterNHands() {
	if (p_engine_container->symbol_engine_istournament() == NULL)	{
		// Initialization phase
		// Null pointer possible due to circular dependency
		// Simply leave because it is too early to lock the blinds
		return;
	}
	write_log(preferences.debug_table_limits(), 
    "[CSymbolEngineTableLimits] AutoLockBlindsForCashgamesAfterNHands()\n");
	write_log(preferences.debug_table_limits(),
    "[CSymbolEngineTableLimits] blinds_locked_for_complete_session: %s\n", 
    Bool2CString(blinds_locked_for_complete_session));
	write_log(preferences.debug_table_limits(), 
    "[CSymbolEngineTableLimits] istournament: %s\n", 
    Bool2CString(p_engine_container->symbol_engine_istournament()->istournament()));
	if (blinds_locked_for_complete_session) {
    write_log(preferences.debug_table_limits(), 
      "[CSymbolEngineTableLimits] blinds_locked_for_complete_session\n");
		write_log(preferences.debug_table_limits(), 
      "[CSymbolEngineTableLimits] Leaving CSymbolEngineTableLimits::AutoLockBlindsForCashgamesAfterNHands() early\n");
		return;
  }
  if (p_engine_container->symbol_engine_istournament()->istournament())	{
		write_log(preferences.debug_table_limits(), 
      "[CSymbolEngineTableLimits] istournament\n");
		write_log(preferences.debug_table_limits(), 
      "[CSymbolEngineTableLimits] Leaving CSymbolEngineTableLimits::AutoLockBlindsForCashgamesAfterNHands() early\n");
		return;
	}
	if (number_of_saved_tablelimits == k_number_of_hands_to_autolock_blinds_for_cashgames) {
		// We simply take the median as the "correct" value.
		// This works, as long as less than half of the values are too small
		// and less than half of the values are too high.
		// Rasonable assumption, otherwise we have name serious problem anyway.
		tablelimit_locked_for_complete_session.sblind = median(tablelimits_first_N_hands_sblind, k_number_of_hands_to_autolock_blinds_for_cashgames);
		tablelimit_locked_for_complete_session.bblind = median(tablelimits_first_N_hands_bblind, k_number_of_hands_to_autolock_blinds_for_cashgames);
		tablelimit_locked_for_complete_session.bbet   = median(tablelimits_first_N_hands_bbet,   k_number_of_hands_to_autolock_blinds_for_cashgames);
		blinds_locked_for_complete_session = true;
		write_log(preferences.debug_table_limits(), 
      "[CSymbolEngineTableLimits] Locking blinds at %.2f / %.2f / %.2f\n", 
      tablelimit_locked_for_complete_session.sblind, 
			tablelimit_locked_for_complete_session.bblind, 
      tablelimit_locked_for_complete_session.bbet);
	}	else {
		write_log(preferences.debug_table_limits(), 
      "[CSymbolEngineTableLimits] Not yet enough hands to lock blinds permanent.\n");
	}
}

void CSymbolEngineTableLimits::RememberBlindsForCashgames() {
	write_log(preferences.debug_table_limits(), 
    "[CSymbolEngineTableLimits] RememberBlindsForCashgames()\n");
	if (number_of_saved_tablelimits < k_number_of_hands_to_autolock_blinds_for_cashgames)	{
		write_log(preferences.debug_table_limits(), 
      "[CSymbolEngineTableLimits] Saving blinds for hand %d\n", number_of_saved_tablelimits);
		tablelimits_first_N_hands_sblind[number_of_saved_tablelimits] = tablelimit_locked_for_current_hand.sblind;
		tablelimits_first_N_hands_bblind[number_of_saved_tablelimits] = tablelimit_locked_for_current_hand.bblind;
		tablelimits_first_N_hands_bbet[number_of_saved_tablelimits]   = tablelimit_locked_for_current_hand.bbet;
		number_of_saved_tablelimits++;
	}	else {
		write_log(preferences.debug_table_limits(), 
      "[CSymbolEngineTableLimits] Already enough saved blinds.\n");
	}
}

void CSymbolEngineTableLimits::AutoLockBlindsForCurrentHand() {
	write_log(preferences.debug_table_limits(), 
    "[CSymbolEngineTableLimits] AutoLockBlindsForCurrentHand()\n");
	blinds_locked_for_current_hand = true;
	tablelimit_locked_for_current_hand.sblind = tablelimit_best_guess.sblind;
	tablelimit_locked_for_current_hand.bblind = tablelimit_best_guess.bblind;
	tablelimit_locked_for_current_hand.bbet	  = tablelimit_best_guess.bbet;
	write_log(preferences.debug_table_limits(), 
    "[CSymbolEngineTableLimits] Locked blinds at %.2f / %.2f / %.2f\n", 
    tablelimit_locked_for_current_hand.sblind,
		tablelimit_locked_for_current_hand.bblind, 
    tablelimit_locked_for_current_hand.bbet);
	RememberBlindsForCashgames();
}

void CSymbolEngineTableLimits::AutoLockBlinds() {
	write_log(preferences.debug_table_limits(), 
    "[CSymbolEngineTableLimits] AutoLockBlinds()\n");
	write_log(preferences.debug_table_limits(), 
    "[CSymbolEngineTableLimits] blinds_locked_for_current_hand: %s\n", 
    Bool2CString(blinds_locked_for_current_hand));
	// Reasonable blinds guaranteed by the way we guess.
  // And IsMyTurn guarantees stable input
  if (!blinds_locked_for_current_hand && p_casino_interface->IsMyTurn()) {
		AutoLockBlindsForCurrentHand();
		AutoLockBlindsForCashgamesAfterNHands();
	}
}

void CSymbolEngineTableLimits::CalcTableLimits() { 
 	write_log(preferences.debug_table_limits(), "[CSymbolEngineTableLimits] CalcTableLimits()\n");
  UpdateOnHeartbeat();
}

STableLimit CSymbolEngineTableLimits::BestTableLimitsToBeUsed() {
  if (blinds_locked_for_complete_session) {
		return tablelimit_locked_for_complete_session;
	}	else if (blinds_locked_for_current_hand) {
		return tablelimit_locked_for_current_hand;
	}	else {
		return tablelimit_best_guess;
	}
}

double CSymbolEngineTableLimits::sblind() {
	return BestTableLimitsToBeUsed().sblind;
}

double CSymbolEngineTableLimits::bblind(){
	return BestTableLimitsToBeUsed().bblind;
}

double CSymbolEngineTableLimits::bigbet(){
	return BestTableLimitsToBeUsed().bbet;
}

double CSymbolEngineTableLimits::ante() {
	return _ante; 
}

double CSymbolEngineTableLimits::buyin() {
  if (p_table_state->_s_limit_info.buyin() > 0) {
    return p_table_state->_s_limit_info.buyin();
  }
  return kUndefinedZero;
}

double CSymbolEngineTableLimits::prizepool() {
	if (p_table_state->_s_limit_info.prizepool() > 0) {
		return p_table_state->_s_limit_info.prizepool();
	}
	return kUndefinedZero;
}

double CSymbolEngineTableLimits::bet(int betround) {
	assert(betround >= kBetroundPreflop);
	assert(betround <= kBetroundRiver);
  assert(p_engine_container->symbol_engine_gametype() != NULL);
  if (p_engine_container->symbol_engine_gametype()->isfl() && (betround >= kBetroundTurn)) {
    return bigbet();
  }
	return bblind();
}

double CSymbolEngineTableLimits::bet() {
	return (bet(p_betround_calculator->betround()));
}

bool CSymbolEngineTableLimits::EvaluateSymbol(const CString name, double *result, bool log /* = false */) {
  FAST_EXIT_ON_OPENPPL_SYMBOLS(name);
	if (memcmp(name, "bet", 3)==0)	{
		if (memcmp(name, "bet", 3)==0 && strlen(name)==3) {
			*result = bet();
      return true;
		}	else if (memcmp(name, "bet", 3)==0 && strlen(name)==4) {
      char betround = name[3];
      if ((betround >= '1') && (betround <= '4')) {
			  *result = bet(RightDigitCharacterToNumber(name));
        return true;
      }
		}
    // Invalid symbol
		return false;
	}
	if (memcmp(name, "bblind", 6)==0 && strlen(name)==6) {
		*result = bblind();
	}	else if (memcmp(name, "sblind", 6)==0 && strlen(name)==6)	{
		*result = sblind();
	}	else if (memcmp(name, "ante", 4)==0 && strlen(name)==4)	{
		*result = ante();
	}	else if (memcmp(name, "buyin", 5)==0 && strlen(name)==5) {
		*result = buyin();
	}	else if (memcmp(name, "prizepool", 9) == 0 && strlen(name) == 9) {
		*result = prizepool();
  }	else {
		// Symbol of a different symbol-engine
		return false;
	}
	// Valid symbol
	return true;
}

CString CSymbolEngineTableLimits::SymbolsProvided() {
  CString list = "bet bblind sblind ante buyin ";
  list += RangeOfSymbols("bet%i", kBetroundPreflop, kBetroundRiver);
  return list;
}