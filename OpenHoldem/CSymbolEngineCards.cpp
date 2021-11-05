//******************************************************************************
//
// This file is part of the OpenHoldem project
//    Source code:           https://github.com/OpenHoldem/openholdembot/
//    Forums:                http://www.maxinmontreal.com/forums/index.php
//    Licensed under GPL v3: http://www.gnu.org/licenses/gpl.html
//
//******************************************************************************
//
// Purpose: Cards, ranks and suits.
//   Partially Omaha-compatible.
//   Some symbols like "ispair" are for HoldEm only.
//
//******************************************************************************

#include "stdafx.h"
#include "CSymbolEngineCards.h"

#include <assert.h>
#include "CBetroundCalculator.h"
#include "CEngineContainer.h"

#include "CScraper.h"
#include "CSymbolEngineIsOmaha.h"
#include "CSymbolEnginePokerval.h"
#include "CSymbolEngineUserchair.h"
#include "CTableState.h"
#include "inlines/eval.h"
#include "..\CTransform\CTransform.h"
//#include "CWhiteInfoBox.h"
//#include "RtaWindow.h"



CSymbolEngineCards::CSymbolEngineCards() {
	// The values of some symbol-engines depend on other engines.
	// As the engines get later called in the order of initialization
	// we assure correct ordering by checking if they are initialized.
	//
	// We use 2 times the function p_engine_container->symbol_engine_pokerval()->CalculatePokerval,
	// but luckily this does not create name symbol-dependency.
	// We leave the file ""CSymbolEnginePokerval.h" here out to avoid name circular dependency.
  assert(p_engine_container->symbol_engine_isomaha() != NULL);
	assert(p_engine_container->symbol_engine_userchair() != NULL);
	// Set up some suit masks
	CardMaskCreateCMAllCardsOfOfSuit(&heartsCards,   Suit_HEARTS);
	CardMaskCreateCMAllCardsOfOfSuit(&diamondsCards, Suit_DIAMONDS);
	CardMaskCreateCMAllCardsOfOfSuit(&spadesCards,   Suit_SPADES);
	CardMaskCreateCMAllCardsOfOfSuit(&clubsCards,    Suit_CLUBS);
}

CSymbolEngineCards::~CSymbolEngineCards() {
}

void CSymbolEngineCards::InitOnStartup() {
	UpdateOnConnection();
}

void CSymbolEngineCards::UpdateOnConnection() {
	UpdateOnHandreset();
}

void CSymbolEngineCards::UpdateOnHandreset() {
	// pocket tests
	_ispair = false;
	_issuited = false;
	_isconnector = false;
	// common cards
	_nouts = 0;
	_ncommoncardsknown = 0;
	_ncommoncardsknown = 0;
	// (un)known cards
	_ncardsknown = 0;
	_ncardsunknown = 0;
	_ncardsbetter = 0;
	// nhands
	_nhands = 0;
	_nhandshi = 0;
	_nhandslo = 0;
	_nhandsti = 0;
	// flushes straights sets
	_nsuited = 0;
	_nsuitedcommon = 0;
	_tsuit = 0;
	_tsuitcommon = 0;
	_nranked = 0;
	_nrankedcommon = 0;
	_trank = 0;
	_trankcommon = 0;
	_nstraight = 0;
	_nstraightcommon = 0;
	_nstraightflush = 0;
	_nstraightflushcommon     = 0;
	_nstraightfill            = k_cards_needed_for_straight;
	_nstraightfillcommon      = k_cards_needed_for_straight;
	_nstraightflushfill       = k_cards_needed_for_flush;
	_nstraightflushfillcommon = k_cards_needed_for_flush;
}

void CSymbolEngineCards::UpdateOnNewRound() {
}

void CSymbolEngineCards::UpdateOnMyTurn() {
}

void CSymbolEngineCards::UpdateOnHeartbeat() {
	CalcPocketTests();
	CalculateCommonCards();
	CalcFlushesStraightsSets();
	CalcUnknownCards();
}

void CSymbolEngineCards::CalcPocketTests() {
	_ispair      = false;
	_issuited    = false;
	_isconnector = false;

	if (!p_table_state->User()->HasKnownCards()) 	{
		return;
	}

  if (p_table_state->User()->hole_cards(0)->GetOpenHoldemRank()
			== p_table_state->User()->hole_cards(1)->GetOpenHoldemRank()) {
		_ispair = true;															
	}
	if (p_table_state->User()->hole_cards(0)->GetSuit()
			== p_table_state->User()->hole_cards(1)->GetSuit())	{
		_issuited = true;														
	}
	if (abs((p_table_state->User()->hole_cards(0)->GetOpenHoldemRank()
			- p_table_state->User()->hole_cards(1)->GetOpenHoldemRank())) == 1)	{
		_isconnector = true;														
	}
}

void CSymbolEngineCards::CardMaskCreateCMAllCardsOfOfSuit(
	CardMask *card_mask_of_suit, int suit)
{
	CardMask_RESET(*card_mask_of_suit);
	for (int i=Rank_2; i<=Rank_ACE; i++)
	{
		CardMask_SET(*card_mask_of_suit, StdDeck_MAKE_CARD(i, suit));
	}
}

int CSymbolEngineCards::GetNumberOfCardsForSuit(
	CardMask cards, CardMask card_mask_of_suit)
{
	CardMask suittestCards;
	CardMask_AND(suittestCards, cards, card_mask_of_suit);
	int n = StdDeck_numCards(suittestCards);
	write_log(Preferences()->debug_symbolengine(), "[CSymbolEngineCards] found %i cards of the same suit\n", n);
	return n;
}

int CSymbolEngineCards::GetDominantSuit(CardMask cards, int* max_cards_of_same_suit) {
	int n_clubs, n_diamonds, n_hearts, n_spades;
	n_clubs    = GetNumberOfCardsForSuit(cards, clubsCards);
	n_diamonds = GetNumberOfCardsForSuit(cards, diamondsCards);
	n_hearts   = GetNumberOfCardsForSuit(cards, heartsCards);
	n_spades   = GetNumberOfCardsForSuit(cards, spadesCards);
	write_log(Preferences()->debug_symbolengine(), "[CSymbolEngineCards] found %i clubs, %i diamonds, %i hearts and %i spades\n",
		n_clubs, n_diamonds, n_hearts, n_spades);
	if ((n_clubs >= n_diamonds) && (n_clubs >= n_hearts) && (n_clubs >= n_spades)) {
		*max_cards_of_same_suit = n_clubs;
		return Suit_CLUBS;
	}	else if ((n_diamonds >= n_hearts) && (n_diamonds >= n_spades)) {
		*max_cards_of_same_suit = n_diamonds;
		return Suit_DIAMONDS;
	}	else if (n_hearts >= n_spades) {
		*max_cards_of_same_suit = n_hearts;
		return Suit_HEARTS;
	}	else {
		*max_cards_of_same_suit = n_spades;
		return Suit_SPADES;
	}
}

void CSymbolEngineCards::CalcFlushesStraightsSets()
{
	int				n = 0;
	CardMask		plCards = {0}, comCards = {0};
	int				max = 0;
	unsigned int	strbits = 0;

	_tsuit   = 0;
	_nsuited = 0;
	_tsuitcommon   = 0;
	_nsuitedcommon = 0;
	_nranked = 0;
	_trank = 0;
	_nrankedcommon = 0;
	_trankcommon   = 0;
	_nstraight     = 0;
	_nstraightfill = k_cards_needed_for_straight;
	_nstraightcommon     = 0;
	_nstraightfillcommon = k_cards_needed_for_straight;
	_nstraightflush     = 0;
	_nstraightflushfill = k_cards_needed_for_straight;
	_nstraightflushcommon     = 0;
	_nstraightflushfillcommon = k_cards_needed_for_straight;
	
	// player cards
	CardMask_RESET(plCards);
	for (int i=0; i<NumberOfCardsPerPlayer(); i++)	{
    Card* card = p_table_state->User()->hole_cards(i);
		if (card->IsKnownCard())	{
			write_log(Preferences()->debug_symbolengine(), "[CSymbolEngineCards] Setting card mask player: %i\n",
				card);
      CardMask_SET(plCards, card->GetValue());
		}
	}

	// common cards
	CardMask_RESET(comCards);
	for (int i=0; i<kNumberOfCommunityCards; i++)	{
		Card *card = p_table_state->CommonCards(i);
		if (card->IsKnownCard())		{
			write_log(Preferences()->debug_symbolengine(), "[CSymbolEngineCards] Setting card mask common (and player): %i\n",
        card->GetValue());
      CardMask_SET(comCards, card->GetValue());
      CardMask_SET(plCards, card->GetValue());
		}
	}

	// nsuited, tsuit
	_tsuit = GetDominantSuit(plCards, &_nsuited);
	// nsuitedcommon, tsuitcommon
	_tsuitcommon = GetDominantSuit(comCards, &_nsuitedcommon);
	
	// nranked, trank
	max = 0;
	for (int i=12; i>=0; i--)
	{
		n = CardMask_CARD_IS_SET(plCards, i+(Rank_COUNT*0)) +
			CardMask_CARD_IS_SET(plCards, i+(Rank_COUNT*1)) +
			CardMask_CARD_IS_SET(plCards, i+(Rank_COUNT*2)) +
			CardMask_CARD_IS_SET(plCards, i+(Rank_COUNT*3));
		if (n>max && n>0)
		{
			max = n;
			_trank = i + 2;															// trank
		}
	}
	_nranked = max;																// nranked

	// nrankedcommon, trankcommon
	max = 0;
	for (int i=12; i>=0; i--)
	{
		n = CardMask_CARD_IS_SET(comCards, i+(Rank_COUNT*0)) +
			CardMask_CARD_IS_SET(comCards, i+(Rank_COUNT*1)) +
			CardMask_CARD_IS_SET(comCards, i+(Rank_COUNT*2)) +
			CardMask_CARD_IS_SET(comCards, i+(Rank_COUNT*3));
		if (n>max && n>0)
		{
			max = n;
			_trankcommon = i + 2;													// trankcommon
		}
	}
	_nrankedcommon = max;															// nrankedcommon

	// nstraight, nstraightfill
	strbits = 0;
	for (int i=0; i<Rank_COUNT; i++)
	{
		if (CardMask_CARD_IS_SET(plCards, (Rank_COUNT*0)+i) ||
			CardMask_CARD_IS_SET(plCards, (Rank_COUNT*1)+i) ||
			CardMask_CARD_IS_SET(plCards, (Rank_COUNT*2)+i) ||
			CardMask_CARD_IS_SET(plCards, (Rank_COUNT*3)+i))
		{
			strbits |= (1<<(i+2));
		}
	}
	if (CardMask_CARD_IS_SET(plCards, (Rank_COUNT*0)+Rank_ACE) ||
		CardMask_CARD_IS_SET(plCards, (Rank_COUNT*1)+Rank_ACE) ||
		CardMask_CARD_IS_SET(plCards, (Rank_COUNT*2)+Rank_ACE) ||
		CardMask_CARD_IS_SET(plCards, (Rank_COUNT*3)+Rank_ACE))
	{
		strbits |= (1<<1);
	}

	// Checking for T-low-strsight down to Ace-low-straight
	for (int i=10; i>=1; i--)
	{
		if (((strbits>>i)&0x1f) == 0x1f)
		{
			_nstraight = (_nstraight<5 ? 5 : _nstraight);
		}
		else if (((strbits>>i)&0x1e) == 0x1e ||
				 ((strbits>>i)&0x0f) == 0x0f)
		{
			_nstraight = _nstraight<4 ? 4 : _nstraight;
		}
		else if (((strbits>>i)&0x1c) == 0x1c ||
				 ((strbits>>i)&0x0e) == 0x0e ||
				 ((strbits>>i)&0x07) == 0x07)
		{
			_nstraight = _nstraight<3 ? 3 : _nstraight;
		}
		else if (((strbits>>i)&0x18) == 0x18 ||
				 ((strbits>>i)&0x0c) == 0x0c ||
				 ((strbits>>i)&0x06) == 0x06 ||
				 ((strbits>>i)&0x3) == 0x03)
		{
			_nstraight = _nstraight<2 ? 2 : _nstraight;
		}
		else 
		{
			_nstraight = _nstraight<1 ? 1 : _nstraight;					// nstraight
		}

		n = bitcount(((strbits>>i)&0x1f));
		if (5-n < _nstraightfill)
			{
				_nstraightfill = 5-n;													// nstraightfill
			}
		}

		// nstraightcommon, nstraightfillcommon
		if (_ncommoncardsknown < 1)
		{
			_nstraightfillcommon = 5;
		}
		else
		{
			strbits = 0;
			for (int i=0; i<Rank_COUNT; i++)
			{
				if (CardMask_CARD_IS_SET(comCards, (Rank_COUNT*0)+i) ||
					CardMask_CARD_IS_SET(comCards, (Rank_COUNT*1)+i) ||
					CardMask_CARD_IS_SET(comCards, (Rank_COUNT*2)+i) ||
					CardMask_CARD_IS_SET(comCards, (Rank_COUNT*3)+i))
				{
					strbits |= (1<<(i+2));
				}
			}
			if (CardMask_CARD_IS_SET(comCards, (Rank_COUNT*0)+Rank_ACE) ||
				CardMask_CARD_IS_SET(comCards, (Rank_COUNT*1)+Rank_ACE) ||
				CardMask_CARD_IS_SET(comCards, (Rank_COUNT*2)+Rank_ACE) ||
				CardMask_CARD_IS_SET(comCards, (Rank_COUNT*3)+Rank_ACE))
			{
				strbits |= (1<<1);
			}

			// Checking for T-low-strsight down to Ace-low-straight
			for (int i=10; i>=1; i--)
			{
				if (((strbits>>i)&0x1f) == 0x1f)
				{
				_nstraightcommon = _nstraightcommon<5 ? 5 : _nstraightcommon;
				}
				else if (((strbits>>i)&0x1e) == 0x1e ||
						 ((strbits>>i)&0x0f) == 0x0f)
				{
				_nstraightcommon = _nstraightcommon<4 ? 4 : _nstraightcommon;
				}
				else if (((strbits>>i)&0x1c) == 0x1c ||
						 ((strbits>>i)&0x0e) == 0x0e ||
						 ((strbits>>i)&0x07) == 0x07)
				{
				_nstraightcommon = _nstraightcommon<3 ? 3 : _nstraightcommon;
				}
				else if (((strbits>>i)&0x18) == 0x18 ||
						 ((strbits>>i)&0x0c) == 0x0c ||
						 ((strbits>>i)&0x06) == 0x06 ||
						 ((strbits>>i)&0x03) == 0x03)
				{
				_nstraightcommon = _nstraightcommon<2 ? 2 : _nstraightcommon;
				}
				else
				{
				_nstraightcommon = _nstraightcommon<1 ? 1 : _nstraightcommon; // nstraightcommon
				}

				n = bitcount(((strbits>>i)&0x1f));
			if (5-n < _nstraightfillcommon)	
				{
				_nstraightfillcommon = 5-n;										// nstraightfillcommon
				}
			}
		}

		// nstraightflush, nstraightflushfill
		for (int j=0; j<k_number_of_suits_per_deck; j++)
		{
			strbits = 0;
			for (int i=0; i<Rank_COUNT; i++)
			{
				if (CardMask_CARD_IS_SET(plCards, (Rank_COUNT*j)+i))
				{
					strbits |= (1<<(i+2));
				}
			}
			if (CardMask_CARD_IS_SET(plCards, (Rank_COUNT*j)+Rank_ACE))
			{
				strbits |= (1<<1);
			}
			// Checking for T-low-strsight down to Ace-low-straight
			for (int i=10; i>=1; i--)
			{
				if (((strbits>>i)&0x1f) == 0x1f)
				{
					_nstraightflush = _nstraightflush<5 ? 5 : _nstraightflush;
				}
				else if (((strbits>>i)&0x1e) == 0x1e ||
						 ((strbits>>i)&0x0f) == 0x0f)
				{
					_nstraightflush = _nstraightflush<4 ? 4 : _nstraightflush;
				}
				else if (((strbits>>i)&0x1c) == 0x1c ||
						 ((strbits>>i)&0x0e) == 0x0e ||
						 ((strbits>>i)&0x07) == 0x07)
				{
					_nstraightflush = _nstraightflush<3 ? 3 : _nstraightflush;
				}
				else if (((strbits>>i)&0x18) == 0x18 ||
						 ((strbits>>i)&0x0c) == 0x0c ||
						 ((strbits>>i)&0x06) == 0x06 ||
						 ((strbits>>i)&0x03) == 0x03)
				{
					_nstraightflush = _nstraightflush<2 ? 2 : _nstraightflush;
				}
				else
				{
					_nstraightflush = _nstraightflush<1 ? 1 : _nstraightflush;	
				}

				n = bitcount(((strbits>>i)&0x1f));
				if (k_cards_needed_for_flush-n < _nstraightflushfill)
				{
					_nstraightflushfill = k_cards_needed_for_flush - n;
				}
			}
		}

		// nstraightflushcommon, nstraightflushfillcommon
		if (_ncommoncardsknown < 1)
		{
			_nstraightflushfillcommon = k_cards_needed_for_straight;
		}
		else
		{
			for (int j=0; j<=3; j++)
			{
				strbits = 0;
				for (int i=0; i<Rank_COUNT; i++)
				{
					if (CardMask_CARD_IS_SET(comCards, (Rank_COUNT*j)+i))
					{
						strbits |= (1<<(i+2));
					}
				}
				if (CardMask_CARD_IS_SET(comCards, (Rank_COUNT*j)+Rank_ACE))
				{
					strbits |= (1<<1);
			}

			for (int i=10; i>=1; i--)
			{
				if (((strbits>>i)&0x1f) == 0x1f)
				{
					_nstraightflushcommon = _nstraightflushcommon<5 ? 5 : _nstraightflushcommon;
				}
				else if (((strbits>>i)&0x1e) == 0x1e ||
						 ((strbits>>i)&0x0f) == 0x0f)
				{
					_nstraightflushcommon = _nstraightflushcommon<4 ? 4 : _nstraightflushcommon;
				}
				else if (((strbits>>i)&0x1c) == 0x1c ||
						 ((strbits>>i)&0x0e) == 0x0e ||
						 ((strbits>>i)&0x07) == 0x07)
				{
					_nstraightflushcommon = _nstraightflushcommon<3 ? 3 : _nstraightflushcommon;
				}
				else if (((strbits>>i)&0x18) == 0x18 ||
						 ((strbits>>i)&0x0c) == 0x0c ||
						 ((strbits>>i)&0x06) == 0x06 ||
						 ((strbits>>i)&0x03) == 0x03)
				{
					_nstraightflushcommon = _nstraightflushcommon<2 ? 2 : _nstraightflushcommon;
				}
				else
				{
					_nstraightflushcommon = _nstraightflushcommon<1 ? 1 : _nstraightflushcommon;		// nstraightflushcommon
				}
				n = bitcount(((strbits>>i)&0x1f));
				if (5-n < _nstraightflushfillcommon )
				{
					_nstraightflushfillcommon = 5-n;							
				}
			}
		}
	}
}

void CSymbolEngineCards::CalculateCommonCards() {
	_ncommoncardsknown = 0;
	for (int i=0; i<kNumberOfCommunityCards; i++)	{
    if (p_table_state->CommonCards(i)->IsKnownCard())		{
			_ncommoncardsknown++;							
		}
	}
}

void CSymbolEngineCards::CalcUnknownCards()
{
	CardMask		stdCards = {0}, commonCards = {0};
	int				nstdCards = 0, ncommonCards = 0;
	HandVal			handval_std = 0, handval_std_plus1 = 0, handval_common_plus1 = 0;
	int				dummy = 0;

	write_log(Preferences()->debug_symbolengine(), "[CSymbolEngineCards] CalcUnknownCards()\n");
	CardMask_RESET(stdCards);
	CardMask_RESET(commonCards);

	for (int i=0; i<NumberOfCardsPerPlayer(); i++)
	{
		// player cards
    Card* card = p_table_state->User()->hole_cards(i);
		if (card->IsKnownCard())
		{
      CardMask_SET(stdCards, card->GetValue());
			nstdCards++;
		}
	}
	for (int i=0; i<kNumberOfCommunityCards; i++)
	{
		// common cards
		Card *card = p_table_state->CommonCards(i);
		if (card->IsKnownCard())
		{
      CardMask_SET(stdCards, card->GetValue());
			nstdCards++;
      CardMask_SET(commonCards, card->GetValue());
		}
	}

	_ncardsknown = nstdCards;	
	_ncardsunknown = kNumberOfCardsPerDeck - _ncardsknown;

	handval_std = Hand_EVAL_N(stdCards, nstdCards);
	_nouts = 0;
	_ncardsbetter = 0;

	if (p_engine_container->symbol_engine_userchair()->userchair_confirmed())	{
		write_log(Preferences()->debug_symbolengine(), "[CSymbolEngineCards] userchair confirmed; calculating nouts...\n");
		// iterate through every unseen card and see what happens to our handvals
		for (int i=0; i<kNumberOfCardsPerDeck; i++)	{
      if (i!=p_table_state->User()->hole_cards(0)->GetValue()  
				  && i!=p_table_state->User()->hole_cards(1)->GetValue() 
          && i!=p_table_state->CommonCards(0)->GetValue()
          && i!=p_table_state->CommonCards(1)->GetValue()
          && i!=p_table_state->CommonCards(2)->GetValue()
          && i!=p_table_state->TurnCard()->GetValue()
          && i!=p_table_state->RiverCard()->GetValue()) {
				CardMask_SET(stdCards, i);
				handval_std_plus1 = Hand_EVAL_N(stdCards, nstdCards+1);
				CardMask_UNSET(stdCards, i);

				CardMask_SET(commonCards, i);
				handval_common_plus1 = Hand_EVAL_N(commonCards, ncommonCards+1);
				CardMask_UNSET(commonCards, i);

				if (BETROUND < kBetroundRiver 
					&& HandVal_HANDTYPE(handval_std_plus1) > HandVal_HANDTYPE(handval_std) 
					&& p_engine_container->symbol_engine_pokerval()->CalculatePokerval(handval_std_plus1, nstdCards+1, &dummy, CARD_NOCARD, CARD_NOCARD) > p_engine_container->symbol_engine_pokerval()->pokerval()
					&& HandVal_HANDTYPE(handval_std_plus1) > HandVal_HANDTYPE(handval_common_plus1))
				{
					_nouts++;										
				}

				if (p_engine_container->symbol_engine_pokerval()->CalculatePokerval(handval_common_plus1, ncommonCards+1, &dummy, CARD_NOCARD, CARD_NOCARD) > p_engine_container->symbol_engine_pokerval()->pokerval())
				{
					_ncardsbetter++;							
				}
			}
		}
	}
	write_log(Preferences()->debug_symbolengine(), "[CSymbolEngineCards] nouts: %i\n", _nouts);
	//p_white_info_box->SetnOuts(_nouts);
	//p_rta_window->SetnOuts(_nouts);
	AssertRange(_ncardsknown,   0, kNumberOfCardsPerDeck);
	AssertRange(_ncardsunknown, 0, kNumberOfCardsPerDeck);
	AssertRange(_nouts,         0, kNumberOfCardsPerDeck);
	AssertRange(_ncardsbetter,  0, kNumberOfCardsPerDeck);
}

bool CSymbolEngineCards::IsHand(const char *name) {
	int	cardrank[2] = {0}, temp;
	int	suited = 0;  //0=not specified, 1=suited, 2=offsuit
	int	cardcnt = 0;
	int	plcardrank[2] = {0}; 

	if (strlen(name) <= 1) return false;
  assert(name[0] == '$');

	// passed in symbol query
	for (int i=1; i<(int) strlen(name); ++i) {
    if (name[i] >= '2' && name[i] <= '9') {
			cardrank[cardcnt++] = DigitCharacterToNumber(name[i]);
    } else if (name[i]=='T' || name[i]=='t') {
			cardrank[cardcnt++] = 10;
    } else if (name[i]=='J' || name[i]=='j') {
			cardrank[cardcnt++] = 11;
    } else if (name[i]=='Q' || name[i]=='q') {
			cardrank[cardcnt++] = 12;
    } else if (name[i]=='K' || name[i]=='k') {
			cardrank[cardcnt++] = 13;
    } else if (name[i]=='A' || name[i]=='a') {
			cardrank[cardcnt++] = 14;
    } else if (name[i]=='X' || name[i]=='x') {
			cardrank[cardcnt++] = -1;
    } else if (name[i]=='S' || name[i]=='s') {
			suited=1;
    } else if (name[i]=='O' || name[i]=='o') {
			suited=2;
    } else {
			return false;
		}
	}
  if (!p_engine_container->symbol_engine_userchair()->userchair_confirmed())
		return false;
  // sort
	if (cardrank[1] > cardrank[0]) {
		temp = cardrank[0];
		cardrank[0] = cardrank[1];
		cardrank[1] = temp;
	}
  // playercards
	plcardrank[0] = p_table_state->User()->hole_cards(0)->GetOpenHoldemRank();
	plcardrank[1] = p_table_state->User()->hole_cards(1)->GetOpenHoldemRank();
	if (plcardrank[1] > plcardrank[0])	{
		SwapInts(&plcardrank[0], &plcardrank[1]);
	}
  bool plsuited = false;
  if (p_table_state->User()->hole_cards(0)->GetSuit() == 
		  p_table_state->User()->hole_cards(1)->GetSuit()) {
		plsuited = true;
	}
	// check for non suited/offsuit match
	if (suited==1 && !plsuited)	return false;
  if (suited==2 && plsuited)	return false;
  // check for non rank match
	// two wildcards
	if (cardrank[0]==-1 && cardrank[1]==-1) return true;
  // one card passed in, or one with name wildcard
	if (cardrank[1]==0 || cardrank[1]==-1) 	{
		if (cardrank[0] != plcardrank[0] 
        && cardrank[0] != plcardrank[1]) {
			return false;
		}
	}
  // two cards passed in
	else {
		if (cardrank[0]!=-1 && cardrank[0]!=plcardrank[0]) return false;
    if (cardrank[1]!=-1 && cardrank[1]!=plcardrank[1]) return false;
	}
  return true;
}

bool CSymbolEngineCards::EvaluateSymbol(const CString name, double *result, bool log /* = false */)
{
  FAST_EXIT_ON_OPENPPL_SYMBOLS(name);
	if (memcmp(name, "$", 1)==0) {
    int right_most_digit = RightDigitCharacterToNumber(name);
		if (memcmp(name, "$$pc", 4)==0)	{
      *result = p_table_state->User()->hole_cards(right_most_digit)->GetValue();
		}	else if (memcmp(name, "$$pr", 4)==0) {
			*result = p_table_state->User()->hole_cards(right_most_digit)->GetOpenHoldemRank();
		}	else if (memcmp(name, "$$ps", 4)==0) {
			*result = p_table_state->User()->hole_cards(right_most_digit)->GetSuit();
		} else if (memcmp(name, "$$cc", 4)==0) {
      *result = p_table_state->CommonCards(right_most_digit)->GetValue();
		}	else if (memcmp(name, "$$cr", 4)==0) {
      *result = p_table_state->CommonCards(right_most_digit)->GetOpenHoldemRank();
		}	else if (memcmp(name, "$$cs", 4)==0) {
			*result = p_table_state->CommonCards(right_most_digit)->GetSuit();
		}	else if (memcmp(name, "$", 1)==0) {
			*result = IsHand(name);
		}	else {
			// Invalid symbol
			return false;
		}
		// Valid symbol
		return true;
	}
	else if (memcmp(name, "ncards", 6)==0)
	{
		if (memcmp(name, "ncardsknown", 11)==0 && strlen(name)==11)
		{
			*result = ncardsknown();
		}
		else if (memcmp(name, "ncardsunknown", 13)==0 && strlen(name)==13)	
		{
			*result = ncardsunknown();
		}
		else if (memcmp(name, "ncardsbetter", 12)==0 && strlen(name)==12)	
		{
      WarnIfSymbolIsHoldemOnly(name);
			*result = ncardsbetter();
		}
		else
		{
			// Invalid symbol
			return false;
		}
		// Valid symbol
		return true;
	}
	else if (memcmp(name, "is", 2)==0)
	{
		if (memcmp(name, "ispair", 6)==0 && strlen(name)==6)
		{
      WarnIfSymbolIsHoldemOnly(name);
			*result = ispair();
		}
		else if (memcmp(name, "issuited", 8)==0 && strlen(name)==8)
		{
      WarnIfSymbolIsHoldemOnly(name);
			*result = issuited();
		}
		else if (memcmp(name, "isconnector", 11)==0 && strlen(name)==11)
		{
      WarnIfSymbolIsHoldemOnly(name);
			*result = isconnector();
		}
		else
		{
			// Invalid symbol
			return false;
		}
		// Valid symbol
		return true;
	}
	else if (memcmp(name, "nsuited", 7)==0)
	{
		if (memcmp(name, "nsuited", 7)==0 && strlen(name)==7)	
		{
      WarnIfSymbolIsHoldemOnly(name);
			*result = nsuited();
		}
		else if (memcmp(name, "nsuitedcommon", 13)==0 && strlen(name)==13)	
		{
      WarnIfSymbolIsHoldemOnly(name);
			*result = nsuitedcommon();
		}
		else
		{
			// Invalid symbol
			return false;
		}
		// Valid symbol
		return true;
	}
	else if (memcmp(name, "tsuit", 5)==0)
	{
		if (memcmp(name, "tsuit", 5)==0 && strlen(name)==5)
		{
      WarnIfSymbolIsHoldemOnly(name);
			*result = tsuit();
		}
		else if (memcmp(name, "tsuitcommon", 11)==0 && strlen(name)==11)
		{
      WarnIfSymbolIsHoldemOnly(name);
			*result = tsuitcommon();
		}
		else
		{
			// Invalid symbol
			return false;
		}
		// Valid symbol
		return true;
	}
	else if (memcmp(name, "nstraight", 9)==0)
	{
		if (memcmp(name, "nstraight", 9)==0 && strlen(name)==9)	
		{
      WarnIfSymbolIsHoldemOnly(name);
			*result = nstraight();
		}
		else if (memcmp(name, "nstraightcommon", 15)==0 && strlen(name)==15)	
		{
			*result = nstraightcommon();
		}
		else if (memcmp(name, "nstraightfill", 13)==0 && strlen(name)==13)		
		{
      WarnIfSymbolIsHoldemOnly(name);
			*result = nstraightfill();
		}
		else if (memcmp(name, "nstraightfillcommon", 19)==0 && strlen(name)==19)	
		{
			*result = nstraightfillcommon();
		}
		else if (memcmp(name, "nstraightflush", 14)==0 && strlen(name)==14)		
		{
      WarnIfSymbolIsHoldemOnly(name);
			*result = nstraightflush();
		}
		else if (memcmp(name, "nstraightflushcommon", 20)==0 && strlen(name)==20)
		{
			*result = nstraightflushcommon();
		}
		else if (memcmp(name, "nstraightflushfill", 18)==0 && strlen(name)==18)
		{
      WarnIfSymbolIsHoldemOnly(name);
			*result = nstraightflushfill();
		}
		else if (memcmp(name, "nstraightflushfillcommon", 24)==0 && strlen(name)==24)
		{
			*result = nstraightflushfillcommon();
		}
		else
		{
			// Invalid symbol
			return false;
		}
		// Valid symbol
		return true;
	}
	else if (memcmp(name, "nranked", 7)==0)
	{
		if (memcmp(name, "nranked", 7)==0 && strlen(name)==7)	
		{
      WarnIfSymbolIsHoldemOnly(name);
			*result = nranked();
		}
		else if (memcmp(name, "nrankedcommon", 13)==0 && strlen(name)==13)
		{
      WarnIfSymbolIsHoldemOnly(name);
			*result = nrankedcommon();
		}
		else
		{
			// Invalid symbol
			return false;
		}
		// Valid symbol
		return true;
	}
	else if (memcmp(name, "trank", 5)==0)
	{
		if (memcmp(name, "trank", 5)==0 && strlen(name)==5)
		{
      WarnIfSymbolIsHoldemOnly(name);
			*result = trank();
		}
		else if (memcmp(name, "trankcommon", 11)==0 && strlen(name)==11)
		{
      WarnIfSymbolIsHoldemOnly(name);
			*result = trankcommon();
		}
		else
		{
			// Invalid symbol
			return false;
		}
		// Valid symbol
		return true;
	}
	else if (memcmp(name, "ncommoncardsknown", 17)==0 && strlen(name)==17)	
	{
		*result = p_engine_container->symbol_engine_cards()->ncommoncardsknown();
		return true;
	}
  else if (memcmp(name, "nouts", 5)==0 && strlen(name)==5)						
	{
    WarnIfSymbolIsHoldemOnly(name);
		*result = nouts();
    return true;
	}
	// Symbol of a different symbol-engine
	return false;
}

CString CSymbolEngineCards::SymbolsProvided() {
  CString list = "ncardsknown ncardsunknown ncardsbetter "
    "ispair issuited isconnector "
    "nsuited nsuitedcommon "
    "tsuit tsuitcommon "
    "nstraight nstraightcommon nstraightfill nstraightfillcommon "
    "nstraightflush nstraightflushcommon nstraightflushfill nstraightflushfillcommon "
    "nranked nrankedcommon "
    "trank trankcommon "
    "ncommoncardsknown nouts ";
  list += RangeOfSymbols("$$pc%i", 0, kMaxNumberOfCardsPerPlayer-1);
  list += RangeOfSymbols("$$pr%i", 0, kMaxNumberOfCardsPerPlayer-1);
  list += RangeOfSymbols("$$ps%i", 0, kMaxNumberOfCardsPerPlayer-1);
  list += RangeOfSymbols("$$cc%i", 0, kNumberOfCommunityCards-1);
  list += RangeOfSymbols("$$cr%i", 0, kNumberOfCommunityCards-1);
  list += RangeOfSymbols("$$cs%i", 0, kNumberOfCommunityCards-1);
  return list;
}