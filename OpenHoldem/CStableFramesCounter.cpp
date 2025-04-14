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

// !!! Todo: turn it into a symbol-engine,
// !!! including reset on autoplayer-action

#include "pch.h"
#include "CStableFramesCounter.h"

#include "CEngineContainer.h"

#include "CScraper.h"
#include "CSymbolEngineAutoplayer.h"
#include "CSymbolEngineIsOmaha.h"
#include "CTableState.h"


CStableFramesCounter *p_stableframescounter = NULL;

CStableFramesCounter::CStableFramesCounter() {
	// Initialize private variables
  for (int i = 0; i < kNumberOfCommunityCards; ++i) {
    _card_common_last[i] = CARD_NOCARD;
  }
  for (int i = 0; i < kMaxNumberOfPlayers; ++i) {
    for (int j = 0; j < kMaxNumberOfCardsPerPlayer; ++j) {
      _card_player_last[i][j] = CARD_NOCARD;
    }
  }
	memset(&_dealer_last[0],         0, sizeof(_dealer_last[0])*kMaxNumberOfPlayers);
	memset(&_playerbalance_last[0],  0, sizeof(_playerbalance_last[0])*kMaxNumberOfPlayers);
	memset(&_playerbet_last[0],      0, sizeof(_playerbet_last[0])*kMaxNumberOfPlayers);
	_myturnbitslast = 0;
	Reset();
}

CStableFramesCounter::~CStableFramesCounter() {
}

void CStableFramesCounter::Reset() {
	write_log(Preferences()->debug_stableframescounter(), "[CStableFramesCounter] Reset\n");
	_NumberOfStableFrames = 0;
	_isReset = true;
}

void CStableFramesCounter::UpdateOnAutoplayerAction() {
  Reset();
}

void CStableFramesCounter::SaveCurrentState() {
	/* 
		Saves the current state for future reference
	*/
  _myturnbitslast = p_engine_container->symbol_engine_autoplayer()->myturnbits();
  for (int i=0; i<kNumberOfCommunityCards; i++) {
    _card_common_last[i] = p_table_state->CommonCards(i)->GetValue();
  }
  for (int i=0; i<kMaxNumberOfPlayers; i++) {
    for (int j = 0; j < kMaxNumberOfCardsPerPlayer; ++j) {
      _card_player_last[i][j] = p_table_state->Player(i)->hole_cards(j)->GetValue();
    }
		_dealer_last[i]         = p_table_state->Player(i)->dealer();
		_playerbalance_last[i]  = p_table_state->Player(i)->_balance.GetValue();
		_playerbet_last[i]      = p_table_state->Player(i)->_bet.GetValue();
	}
}

// This function does automatically detect a changed game-state,
// so there is no longer any need to do a "Reset()" after 
// every autoplayer-action.
int CStableFramesCounter::UpdateNumberOfStableFrames() {
	write_log(Preferences()->debug_stableframescounter(), "[CStableFramesCounter] Update number of stable frames\n");
  if (_isReset) {
		// Counter got reset, e.g. after an autoplayer-action.
		write_log(Preferences()->debug_stableframescounter(), "[CStableFramesCounter] Counter got reset: no stable frames yet\n");
    // Remember current values as last known ones.
		SaveCurrentState();
		// Nothing to compare - now we have 0 identical frames.
		_isReset = false;
		_NumberOfStableFrames = 0;
    return _NumberOfStableFrames; 
	}
	// These items need to be the same to count as a identical frame:
	// - up and down cards
	// - button position
	// - playerbets
	// - playerbalances
	// - button states
	bool same_scrape = true;
  if (_myturnbitslast != p_engine_container->symbol_engine_autoplayer()->myturnbits()) {
		same_scrape = false;
  }
	for (int i=0; i<kNumberOfCommunityCards; i++) {
		if(!same_scrape) break;
    if (p_table_state->CommonCards(i)->GetValue() != _card_common_last[i]) {
			same_scrape = false;
			write_log(Preferences()->debug_stableframescounter(), "[CStableFramesCounter] Community-cards don't match\n");
		}
	}
  for (int i = 0; i < kMaxNumberOfPlayers; i++) {
    if (!same_scrape) {
      break;
    }
    write_log(Preferences()->debug_stableframescounter(), "[CStableFramesCounter] Checking player: %d\n", i);
    if (p_table_state->Player(i)->dealer() != _dealer_last[i]) {
			same_scrape = false;
			write_log(Preferences()->debug_stableframescounter(), "[CStableFramesCounter] Dealer%d-position does not match\n", i);
		}	else if (p_table_state->Player(i)->_balance.GetValue() != _playerbalance_last[i])	{
			same_scrape = false;
			write_log(Preferences()->debug_stableframescounter(), "[CStableFramesCounter] Player%d-balance does not match\n", i);
		}	else if (p_table_state->Player(i)->_bet.GetValue()	 != _playerbet_last[i]) {
			same_scrape = false;
			write_log(Preferences()->debug_stableframescounter(), "[CStableFramesCounter] Player%d-bet does not match\n", i);
    } else {
      for (int j = 0; j < kMaxNumberOfCardsPerPlayer; ++j) {
        if (p_table_state->Player(i)->hole_cards(j)->GetValue() != _card_player_last[i][j]) {
          same_scrape = false;
          write_log(Preferences()->debug_stableframescounter(), "[CStableFramesCounter] Player%d-cards don't match\n", i);
          break;
        }
      }
    }
	}
  if (same_scrape) {
		write_log(Preferences()->debug_stableframescounter(), "[CStableFramesCounter] Increasing number of stable frames\n");
		_NumberOfStableFrames++;
	}	else {
		// Unstable frame
		write_log(Preferences()->debug_stableframescounter(), "[CStableFramesCounter] Gamestate unstable: resetting counter\n");
		Reset();
	}
  write_log(Preferences()->debug_stableframescounter(), "[CStableFramesCounter] Number of stable frames: %d\n", _NumberOfStableFrames);
  return _NumberOfStableFrames;
}