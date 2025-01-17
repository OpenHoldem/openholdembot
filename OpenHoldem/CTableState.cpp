//******************************************************************************
//
// This file is part of the OpenHoldem project
//    Source code:           https://github.com/OpenHoldem/openholdembot/
//    Forums:                http://www.maxinmontreal.com/forums/index.php
//    Licensed under GPL v3: http://www.gnu.org/licenses/gpl.html
//
//******************************************************************************
//
// Purpose: Game-state object for the table
//   (common cards and players)
//   Actually it should be names CGameSate,
//   but there already is a symbol engine with that name. 
//
//******************************************************************************

#include "stdafx.h"
#include "CTableState.h"

#include "CEngineContainer.h"
#include "CSymbolEngineUserchair.h"
#include "CSymbolEngineActiveDealtPlaying.h"

CTableState *p_table_state = NULL;

CTableState::CTableState() {
  Reset();
}

CTableState::~CTableState() {
}

void CTableState::Reset() {
  _s_limit_info.Reset();
  for (int i=0; i<kNumberOfCommunityCards; ++i) {
    _common_cards[i].ClearValue();
  }
  // we made reset function for CPlayer object
  // accept 1 argument to keep the delaer char saved
  // in case the dealer is assigned to an empty seat
  // since here we destroy all data this is false by default
  for (int i=0; i<kNumberOfPlayerEntries; ++i) {
    Player(i)->Reset(false);
  }
  for (int i=0; i<kMaxNumberOfPots; ++i) {
    _pot[i].Reset();
  }
  TableTitle()->Clear();
}

CPlayer *CTableState::User() {
  if (p_engine_container->symbol_engine_userchair() == NULL) {
    return &_players[kFakeEntryForUnknownUserchair];
  }
  if (!p_engine_container->symbol_engine_userchair()->userchair_confirmed()) {
    return &_players[kFakeEntryForUnknownUserchair];
  }
  int userchair = p_engine_container->symbol_engine_userchair()->userchair();
  AssertRange(userchair, 0, kLastChair);
  return &_players[userchair];
}

CPlayer *CTableState::Player(int chair) {
  AssertRange(chair, 0, kLastChair);
  return &_players[chair];
}

double CTableState::Pot(int sidepot_index) {
  AssertRange(sidepot_index, 0, kLastPot);
  return  _pot[sidepot_index].GetValue();
}

bool CTableState::set_pot(int sidepot_index, CString new_value) {
  AssertRange(sidepot_index, 0, kLastPot);
  return _pot[sidepot_index].SetValue(new_value);
}

void CTableState::ResetPots() {
  for (int j = 0; j < kMaxNumberOfPots; j++) {
    _pot[j].Reset();
  }
}

Card *CTableState::CommonCards(int common_card_index) {
  AssertRange(common_card_index, 0, kIndexOfLastCommunityCard);
  return &_common_cards[common_card_index];
}

Card *CTableState::TurnCard() {
  return CommonCards(kIndexOfTurnCard);
}

Card *CTableState::RiverCard() {
  return CommonCards(kIndexOfRiverCard);
}

int CTableState::NumberOfCommunityCards() {
  int result = 0;
  for (int i = 0; i < kNumberOfCommunityCards; ++i) {
    if (_common_cards[i].IsKnownCard()) {
      ++result;
    } else {
      break;
    }
  }
  return result;
}

bool CTableState::ShowdownCardsVisible() {
  for (int i = 0; i < kMaxNumberOfPlayers; ++i) {
    if (&_players[i] == User()) {
      // user-cards, probably no showdown
      continue;
    }
    if (Player(i)->HasKnownCards()) {
      // Opoonent with visible cards, usually showdown.
      // Might also be a poker-simulatpr sh owing all players face-cards
      // Might also be new user-chair after table-change in MTT, ...
      // but thenwe had a handreset, all OK.
      return false;
    }
  }
  return false;
}

bool CTableState::AntesVisible() {
  int number_of_players_posting_ante = 0;
  for (int i = 0; i < kMaxNumberOfPlayers; ++i) {
    if (Player(i)->PostingAnte()) {
      ++number_of_players_posting_ante;
    }
  }
  // Be conservatibe, aboid scraping errors
  // Require more than one player with a tiny bet
  return (number_of_players_posting_ante > 1);
}

CTableTitle *CTableState::TableTitle() {
  return &_table_title;
}

