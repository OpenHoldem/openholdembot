//*******************************************************************************
//
// This file is part of the OpenHoldem project
//   Download page:         http://code.google.com/p/openholdembot/
//   Forums:                http://www.maxinmontreal.com/forums/index.php
//   Licensed under GPL v3: http://www.gnu.org/licenses/gpl.html
//
//*******************************************************************************
//
// Purpose: Calculating the OpenPL-symbol "nutfullhouseorfourofakind"
//   by relative "simple" iterations, as the functions in the OpenPPL-library
//   proved to be way to complicated and error-prone. 
//
//*******************************************************************************

#include "stdafx.h"
#include "CSymbolengineNutFullhouseOrFourOfAKind.h"

#include "PokerEval/include/inlines/eval.h"
#include "Card.h"
#include "CBetroundCalculator.h"
#include "CEngineContainer.h"

#include "CSymbolEngineCards.h"
#include "CSymbolEngineIsOmaha.h"
#include "CSymbolEngineUserchair.h"
#include "CTableState.h"



CSymbolEngineNutFullhouseOrFourOfAKind::CSymbolEngineNutFullhouseOrFourOfAKind() {
  // The values of some symbol-engines depend on other engines.
  // As the engines get later called in the order of initialization
  // we assure correct ordering by checking if they are initialized.
  //
  // This engine does not use any other engines.
  assert(p_engine_container->symbol_engine_cards() != NULL);
  assert(p_engine_container->symbol_engine_userchair() != NULL);
  // Also depending on CSymbolEngineIsOmaha.h, which returns "constant" values
}

CSymbolEngineNutFullhouseOrFourOfAKind::~CSymbolEngineNutFullhouseOrFourOfAKind() {
}

void CSymbolEngineNutFullhouseOrFourOfAKind::InitOnStartup() {
  UpdateOnConnection();
}

void CSymbolEngineNutFullhouseOrFourOfAKind::UpdateOnConnection() {
  UpdateOnHandreset();
}

void CSymbolEngineNutFullhouseOrFourOfAKind::UpdateOnHandreset() {
  _nutfullhouseorfourofakind = kNutFullhouseOrFourOfAKind_NoFullHouseAtAll;
  _nutfullhouseorfourofakind_total_count = 0;
}

void CSymbolEngineNutFullhouseOrFourOfAKind::UpdateOnNewRound() {
  CalculateNutFullhouseOrFourOfAKind();
}

void CSymbolEngineNutFullhouseOrFourOfAKind::UpdateOnMyTurn() {
  CalculateNutFullhouseOrFourOfAKind();
}

void CSymbolEngineNutFullhouseOrFourOfAKind::UpdateOnHeartbeat() {
}

bool CSymbolEngineNutFullhouseOrFourOfAKind::IsFullhouseOrFourOfAKind(int hand_val) {
  int hand_type = HandVal_HANDTYPE(hand_val);
  if (hand_type == HandType_FULLHOUSE) {
    return true;
  }
  if (hand_type == HandType_QUADS) {
    return true;
  }
  return false;
}

int CSymbolEngineNutFullhouseOrFourOfAKind::Handstrength(CardMask cards, int n_cards) {
  int hand_strength = Hand_EVAL_N(cards, n_cards);
  int hand_type = HandVal_HANDTYPE(hand_strength);
  if (hand_type != HandType_QUADS) {
    // Unmodified hand-strength of our 5-card-hand (non-quads).
  }
  else if (p_engine_container->symbol_engine_cards()->nrankedcommon() == 4) {
    // Quads on board, kickers do count.
    // Keep hand-strength unmodified.
  }
  else {
    // Quads with at least one hole-card.
    // We want to count this type of quads only once
    // and not all 12 kickers individuaklly.
    // Remove kicker from hand_strength.
    // Bitmasks can be found in handval.h
    const int kBitmaskQuadsWithoutKicker = HandVal_HANDTYPE_MASK | HandVal_TOP_CARD_MASK;
    hand_strength &= kBitmaskQuadsWithoutKicker;
  }
  return hand_strength;
}

int CSymbolEngineNutFullhouseOrFourOfAKind::FindUnusedCard(CardMask used_cards, int desired_rank) {
  for (int suit = StdDeck_Suit_FIRST; suit <= StdDeck_Suit_LAST; ++suit) {
    int card = StdDeck_MAKE_CARD(desired_rank, suit);
    if (!CardMask_CARD_IS_SET(used_cards, card)) {
      return card;
    }
  }
  return kUndefined;
}

void CSymbolEngineNutFullhouseOrFourOfAKind::CalculateNutFullhouseOrFourOfAKind() {
  _nutfullhouseorfourofakind = kNutFullhouseOrFourOfAKind_NoFullHouseAtAll;
  _nutfullhouseorfourofakind_total_count = 0;
  _list_all_fullhouses.clear();
  _list_better_fullhouses.clear();
  if (p_betround_calculator->betround() < kBetroundFlop) {
    // Preflop or something else
    // No fullhouse possible
    write_log(Preferences()->debug_nutfullhouse(),
      "[CSymbolEngineNutFullhouseOrFourOfAKind] Not yet flop. no fullhouse possible\n");
    return;
  }
  int userchair = p_engine_container->symbol_engine_userchair()->userchair();
  if (userchair == kUndefined) {
    // Can happen if we are not seated 
    // or if we connect in a potential showdown-situation (river without buttons)
    return;
  }
  int handstrength_player = 0;
  CardMask player_cards;
  CardMask board_cards;
  CardMask_RESET(player_cards);
  CardMask_RESET(board_cards);
  int n_player_cards = 0;
  int n_board_cards = 0;
  // setup masks
  AssertRange(userchair, 0, kMaxChairNumber);
  for (int i = 0; i < NumberOfCardsPerPlayer(); i++) {
    Card *card = p_table_state->User()->hole_cards(i);
    if (card->IsKnownCard()) {
      CardMask_SET(player_cards, card->GetValue());
      ++n_player_cards;
    }
  }
  for (int i = 0; i < kNumberOfCommunityCards; i++) {
    Card *card = p_table_state->CommonCards(i);
    if (card->IsKnownCard()) {
      CardMask_SET(board_cards, card->GetValue());
      ++n_board_cards;
    }
  }
  // Evaluate heros hand
  // No need to evaluate the board separately
  CardMask all_cards;
  CardMask_RESET(all_cards);
  CardMask_OR(all_cards, player_cards, board_cards);
  int n_all_cards = n_player_cards + n_board_cards;
  handstrength_player = Handstrength(all_cards, n_all_cards);
  write_log(Preferences()->debug_nutfullhouse(),
    "[CSymbolEngineNutFullhouseOrFourOfAKind] Player strength: %x\n", 
    handstrength_player);
  if (IsFullhouseOrFourOfAKind(handstrength_player)) {
    // We hold the best hand ATM, as far as we know
    _nutfullhouseorfourofakind = kNutFullhouseOrFourOfAKind_WeHaveTheNuts;
    write_log(Preferences()->debug_nutfullhouse(),
      "[CSymbolEngineNutFullhouseOrFourOfAKind] Player has fullhouse or quads\n");
    // Remeber the fullhouse or quads 
    // in case this hand can't be found by the enumaeration
    // because no opponent can hold a hand of the same strength.
    _list_all_fullhouses[handstrength_player] = true;
  } else {
    write_log(Preferences()->debug_nutfullhouse(),
      "[CSymbolEngineNutFullhouseOrFourOfAKind] Player has NOT fullhouse or quads\n");
    assert(_nutfullhouseorfourofakind == kNutFullhouseOrFourOfAKind_NoFullHouseAtAll);
    return;
  }
  // Enumerate all possible better opponent holdings. 
  // We have to enumerate bottom up if we want to count all holdings 
  // exactly once if we want to avoid ugly tricks like remembering
  // and sorting.
  CardMask used_cards;
  CardMask opponent_cards;
  int handstrength_enumerated = 0;
  for (int first_rank = Rank_2; first_rank <= Rank_ACE; ++first_rank) {
    CardMask_RESET(opponent_cards);
    CardMask_RESET(used_cards);
    CardMask_OR(used_cards, board_cards, player_cards);
    int first_player_card = FindUnusedCard(used_cards, first_rank);
    if (first_player_card == kUndefined) {
      // All cards of this rank already used
      continue;
    }
    CardMask_SET(opponent_cards, first_player_card);
    CardMask_SET(used_cards, first_player_card);
    for (int second_rank = Rank_2; second_rank <= first_rank; ++second_rank) {
      int second_player_card = FindUnusedCard(used_cards, second_rank);
      if (second_player_card == kUndefined) {
        // All cards of this rank already used
        continue;
      }
      CardMask_SET(opponent_cards, second_player_card);
      CardMask eval_cards;
      CardMask_RESET(eval_cards);
      CardMask_OR(eval_cards, board_cards, opponent_cards);
      int n_eval_cards = n_board_cards + 2;
      handstrength_enumerated = Handstrength(eval_cards, n_eval_cards);
      // Output cards used for evaluation
      write_log(Preferences()->debug_nutfullhouse(),
        "[CSymbolEngineNutFullhouseOrFourOfAKind] Evaluating %s\n",
        StdDeck_maskString(eval_cards));
      // Remove card for next iteration
      CardMask_UNSET(opponent_cards, second_player_card);
      CardMask_UNSET(used_cards, second_player_card);
      if (!IsFullhouseOrFourOfAKind(handstrength_enumerated)) {
        write_log(Preferences()->debug_nutfullhouse(),
          "[CSymbolEngineNutFullhouseOrFourOfAKind] NOT fullhouse or quads\n");
        continue;
      }
      write_log(Preferences()->debug_nutfullhouse(),
        "[CSymbolEngineNutFullhouseOrFourOfAKind] Hand strength %x\n", 
        handstrength_enumerated);
      _list_all_fullhouses[handstrength_enumerated] = true;
      if (handstrength_enumerated > handstrength_player) {
        // A valid fullhouse pr four of a kind that beats us
        write_log(Preferences()->debug_nutfullhouse(),
          "[CSymbolEngineNutFullhouseOrFourOfAKind] Fullhouse (or quads) better than ours\n");
        _list_better_fullhouses[handstrength_enumerated] = true;
      }
    }
  }
  // Return 1 if we hold the nuts, i.e. 0 better fullhouses / quads
  _nutfullhouseorfourofakind = _list_better_fullhouses.size() + 1;
  _nutfullhouseorfourofakind_total_count = _list_all_fullhouses.size();
  write_log(Preferences()->debug_nutfullhouse(),
    "[CSymbolEngineNutFullhouseOrFourOfAKind] nutfullhouseorfourofakind: %i\n",
    _nutfullhouseorfourofakind);
  write_log(Preferences()->debug_nutfullhouse(),
    "[CSymbolEngineNutFullhouseOrFourOfAKind] total count: %i\n",
    _nutfullhouseorfourofakind_total_count);
}

bool CSymbolEngineNutFullhouseOrFourOfAKind::EvaluateSymbol(const CString name, double *result, bool log /* = false */) {
  FAST_EXIT_ON_OPENPPL_SYMBOLS(name);
	if (memcmp(name, "nutfullhouseorfourofakind", 25)==0 && strlen(name)==25)	{
		*result = _nutfullhouseorfourofakind;
		return true;
	}
  if (memcmp(name, "nutfullhouseorfourofakind_ntotal", 32) == 0 && strlen(name) == 32) {
    *result = _nutfullhouseorfourofakind_total_count;
    return true;
  }
	// Symbol of a different symbol-engine
	return false;
}

CString CSymbolEngineNutFullhouseOrFourOfAKind::SymbolsProvided() {
  return "nutfullhouseorfourofakind nutfullhouseorfourofakind_ntotal";
}
