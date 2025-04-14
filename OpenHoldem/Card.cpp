//******************************************************************************
//
// This file is part of the OpenHoldem project
//    Source code:           https://github.com/OpenHoldem/openholdembot/
//    Forums:                http://www.maxinmontreal.com/forums/index.php
//    Licensed under GPL v3: http://www.gnu.org/licenses/gpl.html
//
//******************************************************************************
//
// Purpose: Scraping the poker-table and providing access to the scraped data.
//  As the CScraper is low-level and quite large we created 
//  an interface SCraperAccess that provides higher-level accessors
//  like "UserHasCards()".
//  Better use that interface to access scraper-data whenever possible.
//
//******************************************************************************

#include "pch.h"
#include "Card.h"

Card::Card() {
  ClearValue();
}

Card::~Card() {
}

void Card::SetValue(int value){
  _value = value;
}

void Card::SetValue(int openholdem_rank, int openholdem_suit) {
  assert(false);
}

void Card::ClearValue(){
  _value = CARD_UNDEFINED;
}

int Card::GetValue() {
  if (_value == CARD_UNDEFINED) {
    // This value should be used only internally by the scraper, etc.,
    // but must not be returned for the GUI and escpecially for the DLL-interface
    // http://www.maxinmontreal.com/forums/viewtopic.php?f=114&t=17677&start=30
    return CARD_NOCARD;
  }
  return _value;
}

// StdDeck uses 0..12 for deuce..ace
// OpenHoldem uses 2..14
int Card::GetOpenHoldemRank() {
  if (IsKnownCard()) {
    return (GetStdDeckRank() + 2);
  } else {
    return kUndefined;
  }
}

// StdDeck uses 0..12 for deuce..ace
// OpenHoldem uses 2..14
int Card::GetStdDeckRank() {
  if (IsKnownCard()) {
    return StdDeck_RANK(_value);
  } else {
    return kUndefined;
  }
}

int Card::GetSuit() {
  if (IsKnownCard()) {
    return StdDeck_SUIT(_value);
  } else {
    return kUndefined;
  }
}

char Card::GetRankCharacter() {
  if (!IsKnownCard()) {
    return '?';
  }
  return StdDeck_rankChars[GetStdDeckRank()];
}

char Card::GetSuitCharacter(bool uppercase /* = false */) {
  if (!IsKnownCard()) {
    return '?';
  }
  char suit_character = StdDeck_suitChars[GetSuit()];
  if (uppercase) {
    return toupper(suit_character);
  }
  else {
    return suit_character;
  }
}

bool Card::IsKnownCard() {
  return (!IsCardBack()
    && !IsNoCard()
    && (_value != CARD_UNDEFINED));
}

bool Card::IsCardBack(){
  return (_value == CARD_BACK);
}

bool Card::IsNoCard() {
  return ((_value == CARD_NOCARD) || (_value == CARD_UNDEFINED));
}
   
bool Card::IsAnyCard() {
  // We do no longer check for cardbacks,
	// but for cardbacks or cards.
	// This way we can play all cards face-up at PokerAcademy.
	// http://www.maxinmontreal.com/forums/viewtopic.php?f=111&t=13384
  return (IsKnownCard() || IsCardBack());
}

CString Card::ToString() {
  return StdDeck_cardString(_value);  
}

CString Card::ToHTML() {
  // code extracted from former CReplayFrame.cpp
  CString suit, color, rank, final;
  if (IsCardBack()) {
    rank = "*";
    suit = "*";
    color = "black";
  } else if (IsNoCard()) {
    rank = "&nbsp";
    suit = "&nbsp";
    color = "black";
  } else if (StdDeck_SUIT(_value) == Suit_CLUBS) {
    suit = "&clubs;";
    color = "green";
  } else if (StdDeck_SUIT(_value) == Suit_DIAMONDS) {
    suit = "&diams;";
    color = "blue";
  } else if (StdDeck_SUIT(_value) == Suit_HEARTS) {
    suit = "&hearts;";
    color = "red";
  } else if (StdDeck_SUIT(_value) == Suit_SPADES) {
    suit = "&spades;";
    color = "black";
  } else {
    rank = "&nbsp";
    suit = "&nbsp";
    color = "black";
  }
  if (IsKnownCard()) {
    rank.Format("%c", GetRankCharacter());
  }
  final.Format("<font color=%s>%s%s</font>", color, rank, suit);
  return final;
}