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
#include "CPlayer.h"

#include "CBetroundCalculator.h"
#include "CEngineContainer.h"
#include "CSymbolEngineIsOmaha.h"
#include "CSymbolEngineTableLimits.h"

// Blobal dummy to handle access to non-existing cards easily
Card dummy_card_nocard;

CPlayer::CPlayer() {
}

CPlayer::~CPlayer() {
}

void CPlayer::Reset(bool is_resetting_dealer_data) {
  set_name("");
  _balance.Reset();
  _bet.Reset();
  for (int i=0; i<kMaxNumberOfCardsPerPlayer; ++i) {
    _hole_cards[i].ClearValue();
  }
  // We can't use set_seated(false) here, as set_seated(false) 
  // will call Reset() again; endless recursion
  _seated = false;
  set_active(false);
  // we can t reset the data for the dealer 
  // even if the chair is empty
  if (!is_resetting_dealer_data) {
	  set_dealer(false);
  }
  set_colourcode(kUndefinedZero);
}

bool CPlayer::HasAnyCards() {
  // We do no longer check for cardbacks,
	// but for cardbacks or cards.
	// This way we can play all cards face-up at PokerAcademy.
	// http://www.maxinmontreal.com/forums/viewtopic.php?f=111&t=13384
  for (int i = 0; i < NumberOfCardsPerPlayer(); ++i) {
    if (!_hole_cards[i].IsAnyCard()) {
      return false;
    }
  }
  return true;
}

CString CPlayer::Cards() {
	// log new hand
  if (HasKnownCards()) {
    CString result; 
    for (int i = 0; i < NumberOfCardsPerPlayer(); ++i) {
      result += _hole_cards[i].ToString();
    }
    return result;
  }
  if (HasAnyCards()) {
    return "BACKS";
  }
  return "NONE";
}

CString CPlayer::CardsAsHTML() {
  // Returning ranks and suits (symbols, colourful) in case of cards
  // Returning "**" on case of card-backs
  // Returning non-breaking spaces in case of no cards
  CString result;
  for (int i = 0; i < NumberOfCardsPerPlayer(); ++i) {
    result += hole_cards(i)->ToHTML();
  }
  return result;
}

bool CPlayer::HasKnownCards() {
  for (int i = 0; i < NumberOfCardsPerPlayer(); ++i) {
    if (!_hole_cards[i].IsKnownCard()) {
      return false;
    }
  }
  return true;
}

Card* CPlayer::hole_cards(int index) {
  // Make sure there is no index overflop
  // and be extra careful, acce4ss may depend on user-input
  if (index < 0) {
    return &dummy_card_nocard;
  }
  if (index >= kMaxNumberOfCardsPerPlayer) {
    return &dummy_card_nocard;
  }
  return &_hole_cards[index];
}

void CPlayer::CheckPlayerCardsForConsistency() {
  if (HasKnownCards() && (_hole_cards[0].GetValue() == _hole_cards[1].GetValue())) {
    // Identical cards, something clearly went wrong.
    // Something "known", not "nocard" and not "undefined",
    // but clearly wrong (duplicate card).
    // We assume, that we see something that differs from the background,
    // probably cardbacks, that sometimes get scraped as JJsuited or 88suited
    // by bad tablemaps.
    // Above check was designed for HoldEm, and we keep it that way,
    // as we don't want to consider any error-correction if 3 of 4 cards are good.
    _hole_cards[0].SetValue(CARD_BACK);
    _hole_cards[1].SetValue(CARD_BACK);
    _hole_cards[2].SetValue(CARD_BACK);
    _hole_cards[3].SetValue(CARD_BACK);
  } 
}

bool CPlayer::IsAllin() {
  return (_seated && _active && HasAnyCards() && (_balance.GetValue() == 0));
}

bool CPlayer::PostingBothBlinds() {
  if (p_betround_calculator->betround() > kBetroundPreflop) {
    // No blind posters postflop
    // http://www.maxinmontreal.com/forums/viewtopic.php?f=156&t=19043
    return false;
  }
  if (IsAllin()) {
    // A person who is allin for SB + BB can't be new at the table, 
    // therefore not posting both blinds.
    // http://www.maxinmontreal.com/forums/viewtopic.php?f=156&t=19414
    return false;
  }
  // We have to calculate in cents here, as IsApproximatellyEqual uses rounding internally.
  // http://www.maxinmontreal.com/forums/viewtopic.php?f=156&t=18743
  double bet_in_cents = 100 * _bet.GetValue();
  double both_blinds_in_cents = 100 * (p_engine_container->symbol_engine_tablelimits()->sblind() + p_engine_container->symbol_engine_tablelimits()->bblind());
  return (_seated && _active && HasAnyCards()
    && IsApproximatellyEqual(bet_in_cents, both_blinds_in_cents));
}

bool CPlayer::PostingAnte() {
  if (p_betround_calculator->betround() > kBetroundPreflop) {
    // No ante posters postflop
    // http://www.maxinmontreal.com/forums/viewtopic.php?f=156&t=19043
    return false;
  }
  if (IsAllin()) {
    // A person who is allin for SB + BB can't be new at the table, 
    // therefore not posting antes.
    // http://www.maxinmontreal.com/forums/viewtopic.php?f=156&t=19414
    return false;
  }
  if (_bet.GetValue() <= 0) {
    return false;
  }
  if (_bet.GetValue() >= p_engine_container->symbol_engine_tablelimits()->sblind()) {
    // Assuming the ante is smaller as the small blind
    // There are exceptions, but we use this function 
    // only as one of many handreset-methods.
    return false;
  }
  return true;
}

void CPlayer::set_seated(bool is_seated, bool is_dealer) { 
  if ((is_seated == false)) {
    // Change from seated to non-seated
    // We should clear all player data in this case
    // as the lazy-scraper stops on empty seats
    // http://www.maxinmontreal.com/forums/viewtopic.php?f=156&t=20567
    // again : the scraper engine can find the dealer button in an empty seat
    // in this case we can t reset dealer postion as this will    
    // send a bad input to all engines that contribute to determine the chairs position
    // this is possible since the lazy-scraper scrape dealer position first 
    // and will finally fix a very old bug for dealer position and false blind chairs inversion
    // https://www.maxinmontreal.com/forums/viewtopic.php?t=24182&start=90
    Reset(is_dealer);
  }
  _seated = is_seated; 
}


CString CPlayer::DataDump() {
  CString result = "";
  // 20 digits, right alighned
  result.Format("%20s", name());
  result += "  ";
  // Seated, active, playing, dealer
  if (seated()) {
    result += "S";
  } else {
    result += "-";
  }
  if (active()) {
    result += "A";
  }  else {
    result += "-";
  }
  if (HasAnyCards()) {
    result += "P";
  } else {
    result += "-";
  }
  if (dealer()) {
    result += "D";
  } else {
    result += "-";
  }
  result += "  ";
  // Money: currentbet and balance
  CString money;
  money.Format("%12.2f  %12.2f", _bet.GetValue(), _balance.GetValue());
  result += money;
  return result;
}
