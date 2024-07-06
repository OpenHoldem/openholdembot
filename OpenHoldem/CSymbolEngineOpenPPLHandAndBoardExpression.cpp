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
#include "CSymbolEngineOpenPPLHandAndBoardExpression.h"

#include "CardFunctions.h"
#include "CEngineContainer.h"
#include "CFormulaParser.h"
#include "CParseErrors.h"

#include "CSymbolEngineCards.h"
#include "CSymbolEngineIsOmaha.h"
#include "CSymbolEnginePokerval.h"
#include "CTableState.h"
#include "StringFunctions/string_functions.h"

const int prime_coded_card_ranks[k_rank_ace+1] = {
	1,	// rank = 0, unused, exists due to C++-definition
	1,	// rank = 1 = Ace-low, unused (here)
	2,  // rank = 2
	3,  // rank = 3
	5,  // rank = 4
	7,  // rank = 5
	11, // rank = 6
	13, // rank = 7
	17, // rank = 8
	19, // rank = 9
	23, // rank = Ten
	29, // rank = Jack
	31, // rank = Queen
	37, // rank = King
	41, // rank = Ace-high
};

CSymbolEngineOpenPPLHandAndBoardExpression::CSymbolEngineOpenPPLHandAndBoardExpression() {
	// The values of some symbol-engines depend on other engines.
	// As the engines get later called in the order of initialization
	// we assure correct ordering by checking if they are initialized.
	assert(p_engine_container->symbol_engine_cards() != NULL);
  assert(p_engine_container->symbol_engine_isomaha() != NULL);
	assert(p_engine_container->symbol_engine_pokerval() != NULL);
}

CSymbolEngineOpenPPLHandAndBoardExpression::~CSymbolEngineOpenPPLHandAndBoardExpression()
{}

void CSymbolEngineOpenPPLHandAndBoardExpression::InitOnStartup()
{}

void CSymbolEngineOpenPPLHandAndBoardExpression::UpdateOnConnection()
{}

void CSymbolEngineOpenPPLHandAndBoardExpression::UpdateOnHandreset()
{}

void CSymbolEngineOpenPPLHandAndBoardExpression::UpdateOnNewRound()
{}

void CSymbolEngineOpenPPLHandAndBoardExpression::UpdateOnMyTurn() {
}

void CSymbolEngineOpenPPLHandAndBoardExpression::UpdateOnHeartbeat() {
  if (p_engine_container->symbol_engine_isomaha()->isomaha()) {
    _prime_coded_hole_cards = PrimeCodedRanks(
      p_table_state->User()->hole_cards(0)->GetOpenHoldemRank(),
      p_table_state->User()->hole_cards(1)->GetOpenHoldemRank(),
      p_table_state->User()->hole_cards(2)->GetOpenHoldemRank(),
      p_table_state->User()->hole_cards(3)->GetOpenHoldemRank());
  } else {
    _prime_coded_hole_cards = PrimeCodedRanks(
      p_table_state->User()->hole_cards(0)->GetOpenHoldemRank(),
      p_table_state->User()->hole_cards(1)->GetOpenHoldemRank());
  }
	_prime_coded_board_cards = PrimeCodedRanks(
    p_table_state->CommonCards(0)->GetOpenHoldemRank(),
    p_table_state->CommonCards(1)->GetOpenHoldemRank(),
    p_table_state->CommonCards(2)->GetOpenHoldemRank(),
    p_table_state->TurnCard()->GetOpenHoldemRank(),
    p_table_state->RiverCard()->GetOpenHoldemRank());
  write_log(Preferences()->debug_hand_and_board_expressions(), 
		"[CSymbolEngineOpenPPLHandAndBoardExpression] _prime_coded_hole_cards = %i\n",
		_prime_coded_hole_cards);
	write_log(Preferences()->debug_hand_and_board_expressions(), 
		"[CSymbolEngineOpenPPLHandAndBoardExpression] _prime_coded_board_cards = %i\n",
		_prime_coded_board_cards);
}

void CSymbolEngineOpenPPLHandAndBoardExpression::CheckForProbablyMistakenSpadesInsteadOfSuited(CString expression) {
  if (!p_formula_parser->IsParsing()) {
    // We want this check andf warning only once at parse-time
    return;
  }
  // This function must only be called if the prefix is hand$ or board$
  assert(expression.GetLength() >= CString("hand$").GetLength());
  // Now we are also sure, that we have enough input for the remaining operations...
  char last_character = RightCharacter(expression);
  if (tolower(last_character) != 's') {
    // No "spades" at the end
    return;
  }
  // The next character should be a card-rank-character 2..A,
  // but we don't check, as it is unreliable user-input.
  char third_last_character = RightCharacter(expression, 2);
  if (!IsCardRankCharacter(third_last_character)) {
    // We either found another suit or a "$" (or something else)
    return;
  }
  // Now we know: 2 ranks at the end + 1 suit
  // Something like AKs (spades)
  // If the user really intended this he could write KsA.
  // AKs probably got confused with AKsuited.
  CString guessed_correct_expression = expression + "uited";
  CString message;
  message.Format("Hand- or board-expression \"%s\" detected.\n"
    "\n"
    "Suit \"s\" (spades) located at the very end.\n"
    "Did you mean \"%s\" instead?\n"
	"Please disambiguate this expression.\n"
	"Either use \"suited\" or reorder the spades-card.\n",
    expression, guessed_correct_expression);
  CParseErrors::Error(message);
}

bool CSymbolEngineOpenPPLHandAndBoardExpression::EvaluateSymbol(const CString name, double *result, bool log /* = false */) {
	// First check, if hand$ or board$ and/or Suited
	// At the same time remove the unnecessary parts of the expression
	if (memcmp(name, "hand$", 5) == 0) {
    CheckForProbablyMistakenSpadesInsteadOfSuited(name);
		is_hand_expression  = true;
		is_board_expression = false;
		hand_or_board_expression = name;
		write_log(Preferences()->debug_hand_and_board_expressions(), 
			"[CSymbolEngineOpenPPLHandAndBoardExpression] Evaluating %s\n",
			hand_or_board_expression);
		hand_or_board_expression = CStringRemoveLeft(hand_or_board_expression, 5);
		prime_coded_available_ranks = _prime_coded_hole_cards;
	}	else if	(memcmp(name, "board$", 6) == 0) {
    CheckForProbablyMistakenSpadesInsteadOfSuited(name);
		is_hand_expression  = false;
		is_board_expression = true;
		hand_or_board_expression = name;
		write_log(Preferences()->debug_hand_and_board_expressions(), 
			"[CSymbolEngineOpenPPLHandAndBoardExpression] Evaluating %s\n",
			hand_or_board_expression);
		hand_or_board_expression = CStringRemoveLeft(hand_or_board_expression, 6);
		prime_coded_available_ranks = _prime_coded_board_cards;
	}	else {
		// Quick exit on other symbols
		return false;
	}
  write_log(Preferences()->debug_hand_and_board_expressions(), 
    "[CSymbolEngineOpenPPLHandAndBoardExpression] Encoded available ranks: %i\n",
    prime_coded_available_ranks);
	bool is_suited_expression = false;
	assert(is_hand_expression || is_board_expression);
	if (hand_or_board_expression.Right(6).MakeLower() == "suited") {
		write_log(Preferences()->debug_hand_and_board_expressions(), 
			"[CSymbolEngineOpenPPLHandAndBoardExpression] Suited expression\n");
		is_suited_expression = true;
		hand_or_board_expression = CStringRemoveRight(
			hand_or_board_expression, 6);
	}
	// Checking ranks with potential multiplicity > 1, not caring about suits.
	// We do this at the very beginning, as this is a very quick test
	// and most real-world-use-cases will be false, so we get a fast exit.
	int prime_coded_search_expression = PrimeCodedRanks(hand_or_board_expression);
  write_log(Preferences()->debug_hand_and_board_expressions(), 
    "[CSymbolEngineOpenPPLHandAndBoardExpression] Encoded searched ranks: %i\n",
    prime_coded_available_ranks);
	if ((prime_coded_available_ranks % prime_coded_search_expression) != 0)	{
		// Division without reminder not possible.
		// Therefore different primes in the search-expression
		// Therefore ranks that do not fit available ranks.
		write_log(Preferences()->debug_hand_and_board_expressions(), 
			"[CSymbolEngineOpenPPLHandAndBoardExpression] No match, because ranks do not fit\n");
		*result = false;
		return true;
	}
	// This was super-elegant, but unfortunatelly there can be 
	// SUITED expressions, which we can only solve with srankbits.
	// Ranks in the expression (to be searched)
	if (is_suited_expression)	{
		int rankbits_to_be_searched = CardStringToRankbits(((char*)hand_or_board_expression.GetString()));
		write_log(Preferences()->debug_hand_and_board_expressions(), 
			"[CSymbolEngineOpenPPLHandAndBoardExpression] rank bits for %s = %i\n",
			hand_or_board_expression.GetString(), rankbits_to_be_searched);
		if (is_hand_expression)	{
			// Suited hand-expression
			// Ranks already checked, there are only 2, this simplifies things
			if (!p_engine_container->symbol_engine_cards()->issuited()) {
        write_log(Preferences()->debug_hand_and_board_expressions(), 
			    "[CSymbolEngineOpenPPLHandAndBoardExpression] No match, because off-suited hole-cards\n");
				// No suited ranks available
				*result = false;
				return true;
			}
		}	else {
			// Suited board-expression
			int rankbits_available = p_engine_container->symbol_engine_pokerval()->srankbitscommon();
			// Check ranks in expression against available ranks
			if ((rankbits_to_be_searched & rankbits_available) != rankbits_to_be_searched)
			{
				write_log(Preferences()->debug_hand_and_board_expressions(),
					"[CSymbolEngineOpenPPLHandAndBoardExpression] No match, because suited rankbits do not fit\n");
				*result = false;
				return true;
			}
		}
	}
	// Third case: cards with individual suits
	int length = hand_or_board_expression.GetLength();
	for (int i=0; i<(length-1); i++) 	{
		if (IsCardRankCharacter(hand_or_board_expression[i])
			  && IsCardSuitCharacter(hand_or_board_expression[i+1])) {
			CString card_with_specific_suit =
				CString(hand_or_board_expression[i])
				+ CString(hand_or_board_expression[i+1]);
			int icard_with_specific_suit = CardStringToCardNumber(
				((char*)card_with_specific_suit.GetString()));

			// Check if card is on the board or in the hand
			if (is_hand_expression) {
        // Code below works for both HoldEm and Omaha
        // Undefined cards are OK
				if (!IsCardInCollection(icard_with_specific_suit,
					  p_table_state->User()->hole_cards(0)->GetValue(),
            p_table_state->User()->hole_cards(1)->GetValue(),
            p_table_state->User()->hole_cards(2)->GetValue(),
            p_table_state->User()->hole_cards(3)->GetValue())) {
					write_log(Preferences()->debug_hand_and_board_expressions(),
						"[CSymbolEngineOpenPPLHandAndBoardExpression] No match, concrete hole cards do not fit\n");
					*result = false;
					return true;
				}
			}	else {
				if (!IsCardInCollection(icard_with_specific_suit,
            p_table_state->CommonCards(0)->GetValue(),
            p_table_state->CommonCards(1)->GetValue(),
            p_table_state->CommonCards(2)->GetValue(),
            p_table_state->TurnCard()->GetValue(),
            p_table_state->RiverCard()->GetValue())) 	{
					write_log(Preferences()->debug_hand_and_board_expressions(),
						"[CSymbolEngineOpenPPLHandAndBoardExpression] No match, concrete board cards do not fit\n");
					*result = false;
					return true;
				}
			}
		}	
	}
	write_log(Preferences()->debug_hand_and_board_expressions(),
		"[CSymbolEngineOpenPPLHandAndBoardExpression] successful match\n");
	*result = true;
	return true;
}

int CSymbolEngineOpenPPLHandAndBoardExpression::PrimeCodedRanks(int rank_0,
	int rank_1, int opt_rank_2, int opt_rank_3, int opt_rank_4) {
	int result = 1;
	int ranks[kNumberOfCommunityCards];
  write_log(Preferences()->debug_hand_and_board_expressions(),
    "[CSymbolEngineOpenPPLHandAndBoardExpression] Given ranks = %i, %i, %i, %i, %i\n",
    rank_0, rank_1, opt_rank_2, opt_rank_3, opt_rank_4);
	ranks[0] = rank_0;
	ranks[1] = rank_1;
	ranks[2] = opt_rank_2;
	ranks[3] = opt_rank_3;
	ranks[4] = opt_rank_4;
	for (int i=0; i<kNumberOfCommunityCards; i++) {
    int rank = ranks[i];
		assert((rank >= 0) || (rank == kUndefined));
		assert(rank <= k_rank_ace);
    write_log(Preferences()->debug_hand_and_board_expressions(),
      "[CSymbolEngineOpenPPLHandAndBoardExpression] card %i rank = %i\n",
      i, rank);
    if (rank >= 1) {
      result *= prime_coded_card_ranks[rank];
    } else {
      // Undefined card 
      // Ignore (which is a multiplication with 1)
    }
	}
  write_log(Preferences()->debug_hand_and_board_expressions(),
    "[CSymbolEngineOpenPPLHandAndBoardExpression] Encoded ranks = %i\n",
    result);
	return result;
}

int CSymbolEngineOpenPPLHandAndBoardExpression::PrimeCodedRanks(CString card_expression) {
	int result = 1;
	int length = card_expression.GetLength();
  for (int i = 0; i < length; i++) {
    char next_character = card_expression[i];
    if (!IsCardRankCharacter(next_character)) {
      // The expression might contain suits,
      // so simply continue on unexpected characters
      continue;
    }
    int rank = CardRankCharacterToCardRank(next_character);
    if (rank == kUndefined) {
      // Hand expression might fior example contain "10" instead of "T"
      // http://www.maxinmontreal.com/forums/viewtopic.php?f=156&t=19774
      CString message;
      message.Format("Invalid character \"%c\" in hand- or board-expression.\n", next_character);
      CParseErrors::Error(message);
      break;
    } else {
      result *= prime_coded_card_ranks[rank];
    }
	}
	return result;
}

CString CSymbolEngineOpenPPLHandAndBoardExpression::SymbolsProvided() {
  return "hand$ board$";
}