//******************************************************************************
//
// This file is part of the OpenHoldem project
//    Source code:           https://github.com/OpenHoldem/openholdembot/
//    Forums:                http://www.maxinmontreal.com/forums/index.php
//    Licensed under GPL v3: http://www.gnu.org/licenses/gpl.html
//
//******************************************************************************
//
// Purpose: Detecting handresets the reliable way,
//   requireing N seen handresetmethods within 3 heartbeats
//
//******************************************************************************

#include "stdafx.h"
#include "CHandresetDetector.h"

#include "CCasinoInterface.h"
#include "CEngineContainer.h"

#include "CScraper.h"
#include "CSymbolEngineActiveDealtPlaying.h"
#include "CSymbolEngineChipAmounts.h"
#include "CSymbolEngineDealerchair.h"
#include "CSymbolEngineHistory.h"
#include "CSymbolEngineIsOmaha.h"
#include "CSymbolEngineTableLimits.h"
#include "CSymbolEngineTime.h"
#include "CSymbolEngineUserchair.h"
#include "CTableState.h"
#include "CTableTitle.h"
#include "CTablemap/CTablemap.h"

#include "StringFunctions/string_functions.h"

CHandresetDetector *p_handreset_detector = NULL;

const int kNumberOfHandresetMethods = 9;
const double kMinimumtimeBetweenTwoHeartbeats = 4.0;

CHandresetDetector::CHandresetDetector() {
  write_log(Preferences()->debug_handreset_detector(), "[CHandresetDetector] Executing constructor\n");
  _is_handreset_on_this_heartbeat = false;
	dealerchair = kUndefined;
  last_dealerchair = kUndefined;
	handnumber = "";
  last_handnumber = "";
  _hands_played = 0;
  _hands_played_headsup = 0;
  _last_potsize = kUndefinedZero;
  _potsize = kUndefinedZero;
  _community_cards = kUndefined;
  _last_community_cards = kUndefined;
  _nopponentsplaying = kUndefined;
  _last_nopponentsplaying = kUndefined;
  _bblind = kUndefinedZero;
  _last_bblind = kUndefinedZero;
  _small_blind_existed_last_hand = false;
  _showdown_cards_visible = false;
  _last_showdown_cards_visible = false;
  _buttons_visible = false;
  _last_buttons_visible = false;
  for (int i = 0; i<kMaxNumberOfCardsPerPlayer; i++) {
    playercards[i] = CARD_NOCARD;
    last_playercards[i] = CARD_NOCARD;  
  }
  for (int i = 0; i < kMaxNumberOfPlayers; ++i) {
    _balance[i] = kUndefinedZero;
    _last_balance[i] = kUndefinedZero;
  }
}

CHandresetDetector::~CHandresetDetector() {
}

CString CHandresetDetector::GetHandNumber() {
	return handnumber;
}

void CHandresetDetector::CalculateIsHandreset() {
  // We work with bit-vectors here and not simple counters,
  // because we want to make sure that at least N *different*
  // fired during the last 3 heartbeats.
	//
  // Last 2 heartbeats
  _methods_firing_the_last_three_heartbeats[2] = _methods_firing_the_last_three_heartbeats[1];
  _methods_firing_the_last_three_heartbeats[1] = _methods_firing_the_last_three_heartbeats[0];
  // This heartbeat
  _methods_firing_the_last_three_heartbeats[0] = BitVectorFiringHandresetMethods();
  // Composed result
  int total_methods_firing = _methods_firing_the_last_three_heartbeats[2]
    | _methods_firing_the_last_three_heartbeats[1]
    | _methods_firing_the_last_three_heartbeats[0];
  write_log(Preferences()->debug_handreset_detector(), "[CHandresetDetector] Methods firing last 3 heartbeat2: %s\n",
    IntToBinaryString(total_methods_firing));
  int number_of_methods_firing = bitcount(total_methods_firing);
  write_log(Preferences()->debug_handreset_detector(), "[CHandresetDetector] Number of methods firing last 3 heartbeat2: %i\n",
    number_of_methods_firing);
  if ((p_engine_container->symbol_engine_time()->elapsedhand() < kMinimumtimeBetweenTwoHeartbeats) 
      && (p_engine_container->symbol_engine_time()->elapsed() > kMinimumtimeBetweenTwoHeartbeats)) {
    // Prevent multiple fast hearbeats due to lagging casino
    // and too many handreset-events on multiple heartbeats
    // http://www.maxinmontreal.com/forums/viewtopic.php?f=156&t=19938
    write_log(Preferences()->debug_handreset_detector(), "[CHandresetDetector] No handreset; too shortly after last hanreset\n");
    _is_handreset_on_this_heartbeat = false;
    // Also clear historic data
    // We don't want to take slow animations on handreset into the future
    // http://www.maxinmontreal.com/forums/viewtopic.php?f=156&t=20352
    ClearSeenHandResets();
  } else if (number_of_methods_firing >= 2) {
    write_log(Preferences()->debug_handreset_detector(), "[CHandresetDetector] Handreset found\n");
    _is_handreset_on_this_heartbeat = true;
    UpdateHandsPlayedOnHandreset();
    ClearSeenHandResets();
  } else {
    write_log(Preferences()->debug_handreset_detector(), "[CHandresetDetector] No handreset\n");
    _is_handreset_on_this_heartbeat = false;
  }
}

void CHandresetDetector::ClearSeenHandResets() {
  // Clear data to avoid multiple fast handreset with already used methods, 
  // if casino needs several heartbeats to update table view.
  // To be cleared:
  //   * on handreset
  //   * during animations after handreset
  //     http://www.maxinmontreal.com/forums/viewtopic.php?f=156&t=20352
  _methods_firing_the_last_three_heartbeats[0] = 0;
  _methods_firing_the_last_three_heartbeats[1] = 0;
  _methods_firing_the_last_three_heartbeats[2] = 0;
}

int CHandresetDetector::BitVectorFiringHandresetMethods() {
  int handresetmethods_fired = 0;
  // Build a bit-vector of handresetmethods that fire
  // Highest (9th) bit is for first method
  handresetmethods_fired |= (IsHandresetByDealerChair() ? 1 : 0);
  handresetmethods_fired <<= 1;
  handresetmethods_fired |= (IsHandresetByUserCards() ? 1 : 0);
  handresetmethods_fired <<= 1;
  handresetmethods_fired |= (IsHandresetByHandNumber() ? 1 : 0);
  handresetmethods_fired <<= 1;
  handresetmethods_fired |= (IsHandresetByCommunityCards() ? 1 : 0);
  handresetmethods_fired <<= 1;
  handresetmethods_fired |= (IsHandresetByPotsize() ? 1 : 0);
  handresetmethods_fired <<= 1;
  handresetmethods_fired |= (IsHandresetByIncreasingNumberOfOpponentsPlaying() ? 1 : 0);
  handresetmethods_fired <<= 1;
  handresetmethods_fired |= (IsHandresetByIncreasingBalance() ? 1 : 0);
  handresetmethods_fired <<= 1;
  handresetmethods_fired |= (IsHandresetByChangingBlindLevel() ? 1 : 0);
  handresetmethods_fired <<= 1;
  handresetmethods_fired |= (IsHandresetByNewSmallBlind() ? 1 : 0 );
  handresetmethods_fired <<= 1;
  handresetmethods_fired |= (IsHandresetByOHReplayFrameNumber() ? 1 : 0);
  handresetmethods_fired <<= 1;
  handresetmethods_fired |= (IsHandresetByVisibleAntes() ? 1 : 0);
  handresetmethods_fired <<= 1;
  handresetmethods_fired |= (IsHandresetByDisappearingShowdownCards() ? 1 : 0);
  handresetmethods_fired <<= 1;
  handresetmethods_fired |= (IsHandresetByButtonsAfterFold() ? 1 : 0);

  // No shift-left after the last bit
  write_log(Preferences()->debug_handreset_detector(), "[CHandresetDetector] Methods firing this heartbeat: %s\n",
    IntToBinaryString(handresetmethods_fired, kNumberOfHandresetMethods));
  return handresetmethods_fired;
}

bool CHandresetDetector::IsHandresetByDealerChair() {
	bool ishandreset = (dealerchair != last_dealerchair)
    && IsValidDealerChair(dealerchair)
    && IsValidDealerChair(last_dealerchair);
	write_log(Preferences()->debug_handreset_detector(), "[CHandresetDetector] Handreset by dealerchair: %s\n",
		Bool2CString(ishandreset));
	return ishandreset;
}

bool CHandresetDetector::IsHandresetByUserCards() {
	bool ishandreset = (p_table_state->User()->HasKnownCards()
		&& (playercards[0] != last_playercards[0]) 
		&& (playercards[1] != last_playercards[1]));
	write_log(Preferences()->debug_handreset_detector(), "[CHandresetDetector] Handreset by cards: %s\n",
		Bool2CString(ishandreset));
	return ishandreset;
}

bool CHandresetDetector::IsHandresetByHandNumber() {
	bool ishandreset = (handnumber != last_handnumber)
    && IsValidHandNumber(handnumber)
    && IsValidHandNumber(last_handnumber);
	write_log(Preferences()->debug_handreset_detector(), "[CHandresetDetector] Handreset by handnumber: %s\n",
		Bool2CString(ishandreset));
	return ishandreset;
}

bool CHandresetDetector::IsValidHandNumber(CString handnumber) {
	int length_of_handnumber = handnumber.GetLength();
	int max_length_of_handnumber = p_tablemap->HandNumberMaxExpectedDigits();
	int min_length_of_handnumber = p_tablemap->HandNumberMinExpectedDigits(); 

	if ((min_length_of_handnumber > 0) && (max_length_of_handnumber > 0)) {
		return ((length_of_handnumber >= min_length_of_handnumber)
			&& (length_of_handnumber <= max_length_of_handnumber));
	}	else {
		// No extra requirements for handnumber: always valid
		return true;
	}
}

bool CHandresetDetector::IsValidDealerChair(int dealerchair) {
	// Dealerchair should be -1, if not found (occlusion).
	return ((dealerchair >= 0) && (dealerchair < p_tablemap->nchairs()));
}

bool CHandresetDetector::IsHandresetByCommunityCards() {
  bool ishandreset = ((_community_cards < _last_community_cards)
    && (_community_cards == 0));
  write_log(Preferences()->debug_handreset_detector(), "[CHandresetDetector] Handreset by community cards: %s\n",
		Bool2CString(ishandreset));
  return ishandreset;
}

bool CHandresetDetector::IsHandresetByPotsize() {
  // Decreasing potsize and potsize is preflop-like and it is preflop
  bool ishandreset = (_potsize < _last_potsize)
    && (_potsize >= 1 * BIG_BLIND)
    && (_potsize <  6 * BIG_BLIND)
    && (_community_cards == 0);
  write_log(Preferences()->debug_handreset_detector(), "[CHandresetDetector] Handreset by potsize: %s\n",
		Bool2CString(ishandreset));
  return ishandreset;
}

bool CHandresetDetector::IsHandresetByIncreasingNumberOfOpponentsPlaying() {
  bool ishandreset = (_nopponentsplaying > _last_nopponentsplaying)
    && (_community_cards == 0);
  write_log(Preferences()->debug_handreset_detector(), "[CHandresetDetector] Handreset by nopponentsplaying: %s\n",
		Bool2CString(ishandreset));
  return ishandreset;
}

bool CHandresetDetector::IsHandresetByIncreasingBalance() {
  bool ishandreset = false;
  for (int i=0; i<kMaxNumberOfPlayers; ++i) {
    if ((_balance[i] > _last_balance[i])
        && (_last_balance[i] > 0)) {
      ishandreset = true;
      break;
    }
  }
  write_log(Preferences()->debug_handreset_detector(), "[CHandresetDetector] Handreset by increasing balance: %s\n",
		Bool2CString(ishandreset));
  return ishandreset;
}

bool CHandresetDetector::IsHandresetByNewSmallBlind() {
  bool ishandreset = SmallBlindExists() 
    && !_small_blind_existed_last_hand
    && (_community_cards == 0);
  write_log(Preferences()->debug_handreset_detector(), "[CHandresetDetector] Handreset by new small blind: %s\n",
		Bool2CString(ishandreset));
  return ishandreset;
}

bool CHandresetDetector::IsHandresetByChangingBlindLevel() {
  bool ishandreset = (_bblind != _last_bblind)
    && (_bblind != 0)
    && (_last_bblind != 0);
  write_log(Preferences()->debug_handreset_detector(), "[CHandresetDetector] Handreset by changing blind level: %s\n",
		Bool2CString(ishandreset));
  return ishandreset;
}

bool CHandresetDetector::IsHandresetByOHReplayFrameNumber() {
  bool ishandreset = (-_ohreplay_framenumber < _last_ohreplay_framenumber);
  write_log(Preferences()->debug_handreset_detector(), "[CHandresetDetector] Handreset by decreasing framenumber of OHReplay: %s\n",
		Bool2CString(ishandreset));
  return ishandreset;
}

bool CHandresetDetector::IsHandresetByVisibleAntes() {
  bool ishandreset = (_antes_visible && !_last_antes_visible);
  write_log(Preferences()->debug_handreset_detector(), "[CHandresetDetector] Handreset by visible antes: %s\n",
    Bool2CString(ishandreset));
  return ishandreset;
}

bool CHandresetDetector::IsHandresetByDisappearingShowdownCards() {
  bool ishandreset = (_last_showdown_cards_visible && !_showdown_cards_visible);
  write_log(Preferences()->debug_handreset_detector(), "[CHandresetDetector] Handreset by disappearing showdown cards: %s\n",
    Bool2CString(ishandreset));
  return ishandreset;
}

bool CHandresetDetector::IsHandresetByButtonsAfterFold() {
  bool ishandreset = (p_engine_container->symbol_engine_history()->DidFoldThisHand()
    && _buttons_visible && !_last_buttons_visible);
  write_log(Preferences()->debug_handreset_detector(), "[CHandresetDetector] Handreset by autoplayer-buttons after fold: %s\n",
    Bool2CString(ishandreset));
  return false;
}

bool CHandresetDetector::SmallBlindExists() {
  for (int i=0; i<p_tablemap->nchairs(); ++i) {
    double players_bet = p_table_state->Player(i)->_bet.GetValue();
    if ((players_bet > 0) && (players_bet < _bblind)) {
      // Either SB or ante, first orbit preflop, hand-reset
      return true;
    }
  }
  return false;
}

void CHandresetDetector::UpdateHandsPlayedOnHandreset() {
  if (!p_engine_container->symbol_engine_userchair()->userchair_confirmed()) {
    // We want to update the handsplayed-counter only when the user
    // is at least seated, not on connection and similar events.
    // http://www.maxinmontreal.com/forums/viewtopic.php?f=214&t=20753
    return;
  }
  ++_hands_played;
  if (p_engine_container->symbol_engine_active_dealt_playing()->nopponentsdealt() == 1) {
    // Last hand (data not yet reset) was headsup
    ++_hands_played_headsup;
  }
  else {
    _hands_played_headsup = 0;
  }
}

void CHandresetDetector::GetNewSymbolValues() {
	assert(p_engine_container->symbol_engine_dealerchair() != NULL);
	if (IsValidDealerChair(p_engine_container->symbol_engine_dealerchair()->dealerchair()))	{
		dealerchair = p_engine_container->symbol_engine_dealerchair()->dealerchair();	
		write_log(Preferences()->debug_handreset_detector(), "[CHandresetDetector] Setting new dealerchair to [%i]\n", dealerchair);
	}
	if (IsValidHandNumber(p_table_state->_s_limit_info.handnumber()))	{
		write_log(Preferences()->debug_handreset_detector(), "[CHandresetDetector] Setting handnumber to [%s]\n", handnumber);
		handnumber = p_table_state->_s_limit_info.handnumber();	
	}	else {
		write_log(Preferences()->debug_handreset_detector(), "[CHandresetDetector] Setting handnumber to [%s] was skipped. Reason: [digits number not in range]\n", handnumber);
	}
	assert(p_engine_container->symbol_engine_userchair() != NULL);
	int userchair = p_engine_container->symbol_engine_userchair()->userchair();
  _potsize = p_engine_container->symbol_engine_chip_amounts()->pot();
  _community_cards = p_table_state->NumberOfCommunityCards();
  _nopponentsplaying = p_engine_container->symbol_engine_active_dealt_playing()->nopponentsplaying();
  _bblind = p_engine_container->symbol_engine_tablelimits()->bblind();
  _showdown_cards_visible = p_table_state->ShowdownCardsVisible();
  _antes_visible = p_table_state->AntesVisible();
  _buttons_visible = p_casino_interface->IsMyTurn();
	for (int i=0; i<kMaxNumberOfCardsPerPlayer; i++) {
		if ((userchair >= 0) && (userchair < p_tablemap->nchairs())) {
      playercards[i] = p_table_state->User()->hole_cards(i)->GetValue();
		} else {
			playercards[i] = CARD_UNDEFINED;
		}
	}
  for (int i=0; i<p_tablemap->nchairs(); ++i) {
    _balance[i] = p_table_state->Player(i)->_balance.GetValue();
  }
  assert(p_table_title != NULL);
  _ohreplay_framenumber = p_table_title->OHReplayFrameNumber();
}

void CHandresetDetector::StoreOldValuesForComparisonOnNextHeartbeat() {
	last_dealerchair = dealerchair;
	write_log(Preferences()->debug_handreset_detector(), "[CHandresetDetector] Setting last dealerchair to [%i]\n", dealerchair);
	last_handnumber = handnumber;
  _last_potsize = _potsize;
  _last_community_cards = _community_cards;
  _last_nopponentsplaying = _nopponentsplaying;
  _last_bblind = _bblind;
  _small_blind_existed_last_hand = SmallBlindExists();
  _last_showdown_cards_visible = _showdown_cards_visible;
  _last_antes_visible = _antes_visible;
  _last_buttons_visible = _buttons_visible;
	for (int i=0; i<NumberOfCardsPerPlayer(); i++) {
		last_playercards[i] = playercards[i];
	}
  for (int i = 0; i < p_tablemap->nchairs(); ++i) {
    _last_balance[i] = _balance[i];
  }
  _last_ohreplay_framenumber = _ohreplay_framenumber;
}

void CHandresetDetector::OnNewHeartbeat() {
	if (p_engine_container->symbol_engine_dealerchair() == NULL) {
		// Very early initialization phase
		return;
	}
	GetNewSymbolValues();
	CalculateIsHandreset();
	StoreOldValuesForComparisonOnNextHeartbeat();
}

