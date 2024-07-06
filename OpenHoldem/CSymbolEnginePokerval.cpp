//******************************************************************************
//
// This file is part of the OpenHoldem project
//    Source code:           https://github.com/OpenHoldem/openholdembot/
//    Forums:                http://www.maxinmontreal.com/forums/index.php
//    Licensed under GPL v3: http://www.gnu.org/licenses/gpl.html
//
//******************************************************************************
//
// Purpose: Hand-strength, HoldEm only, not Omaha.
//
//******************************************************************************

#include "stdafx.h"
#include "CSymbolEnginePokerval.h"

#include "CardFunctions.h"
#include "CBetroundCalculator.h"
#include "CEngineContainer.h"

#include "CScraper.h"
#include "CSymbolEngineCards.h"
#include "CSymbolEngineIsOmaha.h"
#include "CSymbolEngineUserchair.h"
#include "CTableState.h"
#include "CTransform/CTransform.h"
#include "PokerEval/include/inlines/eval.h"



CSymbolEnginePokerval::CSymbolEnginePokerval() {
	// The values of some symbol-engines depend on other engines.
	// As the engines get later called in the order of initialization
	// we assure correct ordering by checking if they are initialized.
	assert(p_engine_container->symbol_engine_cards() != NULL);
	assert(p_engine_container->symbol_engine_tablelimits() != NULL);
	assert(p_engine_container->symbol_engine_userchair() != NULL);
}

CSymbolEnginePokerval::~CSymbolEnginePokerval() {
}

void CSymbolEnginePokerval::InitOnStartup() {
  PrepareConstantSuitMasks();
}

void CSymbolEnginePokerval::UpdateOnConnection() {
	UpdateOnHandreset();
}

void CSymbolEnginePokerval::UpdateOnHandreset() {
	for (int i=0; i<=kNumberOfBetrounds; ++i) {
		_phandval[i] = kUndefinedZero;
		_chandval[i] = kUndefinedZero;
	}
	handval = kUndefinedZero;
}

void CSymbolEnginePokerval::UpdateOnNewRound() {
}

void CSymbolEnginePokerval::UpdateOnMyTurn() {
}

void CSymbolEnginePokerval::UpdateOnHeartbeat() {
  _hand_type       = kUndefinedZero;
	_ishistraight    = false;
	_isroyalflush    = false;
	_ishandup        = false;
	_ishandupcommon  = false;
	_ishipair        = false;
	_islopair        = false;
	_ismidpair       = false;
	_ishistraight    = false;
	_ishiflush       = false;
	_rankbitsplayer  = kUndefinedZero;
	_rankbitscommon  = kUndefinedZero;
	_rankbitspoker   = kUndefinedZero;
	_srankbitsplayer = kUndefinedZero;
	_srankbitscommon = kUndefinedZero;
	_srankbitscommonp = kUndefinedZero;
	_srankbitspoker  = kUndefinedZero;
  CalculateCardMasks();
	CalculateRankBits();
	CalcPokerValues();
}

void CSymbolEnginePokerval::PrepareConstantSuitMasks() {
  CardMask_RESET(_heartsCards);
  CardMask_RESET(_diamondsCards);
  CardMask_RESET(_spadesCards);
  CardMask_RESET(_clubsCards);
  for (int rank = Rank_2; rank <= Rank_ACE; ++rank) {
    CardMask_SET(_heartsCards, StdDeck_MAKE_CARD(rank, Suit_HEARTS));
    CardMask_SET(_diamondsCards, StdDeck_MAKE_CARD(rank, Suit_DIAMONDS));
    CardMask_SET(_spadesCards, StdDeck_MAKE_CARD(rank, Suit_SPADES));
    CardMask_SET(_clubsCards, StdDeck_MAKE_CARD(rank, Suit_CLUBS));
  }
}

void CSymbolEnginePokerval::CalculateCardMasks() {
  CardMask_RESET(_player_cards);
  if (p_table_state->User()->HasKnownCards()) {
    for (int i = 0; i<NumberOfCardsPerPlayer(); i++) {
      CardMask_SET(_player_cards, p_table_state->User()->hole_cards(i)->GetValue());
    }
  }
  CardMask_RESET(_board_cards);
  for (int i = 0; i<kNumberOfCommunityCards; i++) {
    // common cards
    Card *card = p_table_state->CommonCards(i);
    if (card->IsKnownCard()) {
      CardMask_SET(_board_cards, card->GetValue());
    }
  }
  CardMask_RESET(_all_cards);
  CardMask_OR(_all_cards, _player_cards, _board_cards);
}

int CSymbolEnginePokerval::HandEval(CardMask c) {
  int n_cards = CardCount(c);
  int hand_strength = Hand_EVAL_N(c, n_cards);
  return hand_strength;
}

int CSymbolEnginePokerval::CardCount(CardMask c) {
  int count = 0;
  for (int suit = StdDeck_Suit_FIRST; suit <= StdDeck_Suit_LAST; ++suit) {
    for (int rank = Rank_2; rank <= Rank_ACE; ++rank) {
      int card = StdDeck_MAKE_CARD(rank, suit);
      if (StdDeck_CardMask_CARD_IS_SET(c, card)) {
        ++count;
      }
    }
  }
  return count;
}

int CSymbolEnginePokerval::CardMaskToRankBits(CardMask c) {
  int rankbits = 0;
  for (int suit = StdDeck_Suit_FIRST; suit <= StdDeck_Suit_LAST; ++suit) {
    for (int rank = Rank_2; rank <= Rank_ACE; ++rank) {
      int card = StdDeck_MAKE_CARD(rank, suit);
      if (StdDeck_CardMask_CARD_IS_SET(c, card)) {
        SetRankBit(&rankbits, rank);
      }
    }
  }
  return rankbits;
}

int CSymbolEnginePokerval::SuitBits(CardMask c, int suit) {
  CardMask suited_cards;
  CardMask_RESET(suited_cards);
  switch (suit) {
  case Suit_HEARTS:
    CardMask_AND(suited_cards, c, _heartsCards);
    break;
  case Suit_DIAMONDS:
    CardMask_AND(suited_cards, c, _diamondsCards);
    break;
  case Suit_SPADES:
    CardMask_AND(suited_cards, c, _spadesCards);
    break;
  case Suit_CLUBS:
    CardMask_AND(suited_cards, c, _clubsCards);
    break;
  // Otherwise: 
  // Keep suited cards empty
  }
  return CardMaskToRankBits(suited_cards);
}

int CSymbolEnginePokerval::suitbitsplayer(int suit) {
  return SuitBits(_player_cards, suit);
}

int CSymbolEnginePokerval::suitbitscommon(int suit) {
  return SuitBits(_board_cards, suit);
}

void CSymbolEnginePokerval::CalcPokerValues() {
  handval = HandEval(_all_cards);
	_pcbits = 0;
	_pokerval = CalculatePokerval(handval, CardCount(_all_cards), &_pcbits,				
    p_table_state->User()->hole_cards(0)->GetValue(), 
    p_table_state->User()->hole_cards(1)->GetValue());

  write_log(Preferences()->debug_symbolengine(), "[CSymbolEnginePokerval] handval = %i\n", handval);
	write_log(Preferences()->debug_symbolengine(), "[CSymbolEnginePokerval] pokerval = %i\n", _pokerval);
	write_log(Preferences()->debug_symbolengine(), "[CSymbolEnginePokerval] pcbits = %i\n", _pcbits);

	_phandval[BETROUND] = _pokerval & 0xff000000; 
  if (BETROUND > kBetroundPreflop
		&& _phandval[BETROUND] > _phandval[BETROUND-1]) {
		_ishandup = true;														
	}
  CalculateHandType(); 

	///////////////////////////////////////////////////////////////////
	// pokervalplayer
	handval = HandEval(_player_cards);
  int	dummy = 0;
	_pokervalplayer = CalculatePokerval(handval, CardCount(_player_cards), &dummy, CARD_NOCARD, CARD_NOCARD);

  ///////////////////////////////////////////////////////////////////
	// pokervalcommon

	handval = HandEval(_board_cards);

	_pokervalcommon = CalculatePokerval(handval, CardCount(_board_cards), &dummy, CARD_NOCARD, CARD_NOCARD); 
  _chandval[BETROUND] = _pokervalcommon & 0xff000000; 
	if (BETROUND > kBetroundPreflop 
			&& _chandval[BETROUND] > _chandval[BETROUND-1])	{
		_ishandupcommon = true;
	}
}

void CSymbolEnginePokerval::CalculateHandType()
{
  int nCards = CardCount(_all_cards);
	_hand_type = HandVal_HANDTYPE(handval);
	write_log(Preferences()->debug_symbolengine(), "[CSymbolEnginePokerval::CalculateHandType] handval = %i\n", handval);
	write_log(Preferences()->debug_symbolengine(), "[CSymbolEnginePokerval::CalculateHandType] handtype = %i\n", _hand_type);

	if  (isstraightflush() 
		&& StdDeck_RANK(HandVal_TOP_CARD(handval)) == Rank_ACE)
	{
		_isroyalflush = true;
	}
	else if (isflush())
	{
		if (StdDeck_RANK(HandVal_TOP_CARD(handval)) == 12 && (_pcbits & 0x10))
		{
			_ishiflush = true;
		}
		// If we have name pocket King, and it's the second best card in our flush
		else if (StdDeck_RANK(HandVal_SECOND_CARD(handval)) == 11 && (_pcbits & 0x8))
		{
			_ishiflush = true;
		}
		// If we have name pocket Queen, and it's the third best card in our flush
		else if (StdDeck_RANK(HandVal_THIRD_CARD(handval)) == 10 && (_pcbits & 0x4))
		{
			_ishiflush = true;
		}
		// If we have name pocket Jack, and it's the fourth best card in our flush
		else if (StdDeck_RANK(HandVal_FOURTH_CARD(handval)) == 9 && (_pcbits & 0x2))
		{
			_ishiflush = true;
		}																	
	}
	else if (isstraight())
	{							
		if (!IsHigherStraightPossible(handval))
		{
			_ishistraight = true;
		}
	}
	else if (HandVal_HANDTYPE(handval) == HandType_ONEPAIR)
	{
		// hi lo mid pair
		// Only user cards...
		if (nCards == 2)
		{
			_ishipair = true;
		}
		// at least flop seen...
		else if (nCards >= 5)
		{
			if ((int)(StdDeck_RANK(HandVal_TOP_CARD(handval))) 
				>= OpenHoldemRankToStdDeckLibraryRank(rankhicommon()))
			{
				_ishipair = true;										
			}
			else if ((int)(StdDeck_RANK(HandVal_TOP_CARD(handval))) 
					< OpenHoldemRankToStdDeckLibraryRank(rankhicommon())
				&& (int)(StdDeck_RANK(HandVal_TOP_CARD(handval))) 
					> OpenHoldemRankToStdDeckLibraryRank(ranklocommon()))
			{
				_ismidpair = true;
			}
			else
			{
				_islopair = true;
			}
		}
	}
}

CString CSymbolEnginePokerval::HandType() {
	if (ishicard())	            return "hcard"; 
  else if (isonepair())	      return "1pair"; 
  else if (istwopair())	      return "2pair"; 
  else if (isthreeofakind())  return "3kind"; 
  else if (isstraight())      return "strai"; 
  else if (isflush())         return "flush"; 
  else if (isfullhouse())     return "fullh"; 
  else if (isfourofakind())   return "4kind";
	else if (isstraightflush()) return "strfl"; 
  else if (isroyalflush())    return "royfl"; 
  // No hand
	assert(false);
	return "err";
}

void CSymbolEnginePokerval::CalculateRankBits() {
	int	     _rank = 0, suit = 0 ;

  _rankbitsplayer   = kUndefinedZero;
	_rankbitscommon   = kUndefinedZero;
	_rankbitspoker    = kUndefinedZero;
	_srankbitsplayer  = kUndefinedZero;
	_srankbitscommon  = kUndefinedZero;
	_srankbitscommonp = kUndefinedZero;
	_srankbitspoker	  = kUndefinedZero;

  int tsuit = p_engine_container->symbol_engine_cards()->tsuit();
  int tsuitcommon = p_engine_container->symbol_engine_cards()->tsuitcommon();
  write_log(Preferences()->debug_symbolengine(), "[CSymbolEnginePokerval] CalculateHandType() tsuit = %i\n", tsuit);
  write_log(Preferences()->debug_symbolengine(), "[CSymbolEnginePokerval] CalculateHandType() tsuitcommon = %i\n", tsuitcommon);
	
  _rankbitsplayer = CardMaskToRankBits(_player_cards);
  _rankbitscommon = CardMaskToRankBits(_board_cards);
	_rankbitspoker = (1<<((_pokerval>>16)&0xf))
		| (1<<((_pokerval>>12)&0xf)) 
		| (1<<((_pokerval>>8)&0xf)) 
		| (1<<((_pokerval>>4)&0xf)) 
		| (1<<((_pokerval>>0)&0xf)); 
	// Take care about ace (low bit)
	_rankbitspoker |= ((_rankbitspoker&0x4000) ? (1<<1) : 0); 

	for (_rank=StdDeck_Rank_LAST; _rank>=StdDeck_Rank_FIRST; _rank--)	{
		if (CardMask_CARD_IS_SET(_player_cards, StdDeck_MAKE_CARD(_rank, tsuit)))	{
			SetRankBit(&_srankbitsplayer, _rank);
		}
		if (CardMask_CARD_IS_SET(_board_cards, StdDeck_MAKE_CARD(_rank, tsuitcommon))) {
			SetRankBit(&_srankbitscommon, _rank);
		}
		if (CardMask_CARD_IS_SET(_board_cards, StdDeck_MAKE_CARD(_rank, tsuit))) {
			SetRankBit(&_srankbitscommonp, _rank);
		}
	}
  _srankbitspoker =														
			(CardMask_CARD_IS_SET(_all_cards, StdDeck_MAKE_CARD(((_pokerval>>16)&0xf)-2, tsuit)) ?
			 (1<<((_pokerval>>16)&0xf)) : 0) |
			(CardMask_CARD_IS_SET(_all_cards, StdDeck_MAKE_CARD(((_pokerval>>12)&0xf)-2, tsuit)) ?
			 (1<<((_pokerval>>12)&0xf)) : 0) |
			(CardMask_CARD_IS_SET(_all_cards, StdDeck_MAKE_CARD(((_pokerval>>8)&0xf)-2, tsuit)) ?
			 (1<<((_pokerval>>8)&0xf)) : 0) |
			(CardMask_CARD_IS_SET(_all_cards, StdDeck_MAKE_CARD(((_pokerval>>4)&0xf)-2, tsuit)) ?
			 (1<<((_pokerval>>4)&0xf)) : 0) |
			(CardMask_CARD_IS_SET(_all_cards, StdDeck_MAKE_CARD(((_pokerval>>0)&0xf)-2, tsuit)) ?
		 (1<<((_pokerval>>0)&0xf)) : 0);
  // Take care about ace (low bit)
	_srankbitspoker |= ((_srankbitspoker&0x4000) ? (1<<1) : 0); 

  write_log(Preferences()->debug_symbolengine(), "[CSymbolEnginePokerval] CalculateHandType() rankbitsplayer  = %i\n", _rankbitsplayer);
  write_log(Preferences()->debug_symbolengine(), "[CSymbolEnginePokerval] CalculateHandType() rankbitscommon  = %i\n", _rankbitscommon);
	write_log(Preferences()->debug_symbolengine(), "[CSymbolEnginePokerval] CalculateHandType() rankbitspoker   = %i\n", _rankbitspoker);
	write_log(Preferences()->debug_symbolengine(), "[CSymbolEnginePokerval] CalculateHandType() srankbitsplayer = %i\n", _srankbitsplayer);
	write_log(Preferences()->debug_symbolengine(), "[CSymbolEnginePokerval] CalculateHandType() srankbitscommon = %i\n", _srankbitscommon);
	write_log(Preferences()->debug_symbolengine(), "[CSymbolEnginePokerval] CalculateHandType() srankbitspoker  = %i\n", _srankbitspoker);

	AssertRange(_rankbitsplayer,  0, k_rankbits_all_cards_111_111_111_111_110);
	AssertRange(_rankbitscommon,  0, k_rankbits_all_cards_111_111_111_111_110); 
	AssertRange(_rankbitspoker,   0, k_rankbits_all_cards_111_111_111_111_110);  
	AssertRange(_srankbitsplayer, 0, k_rankbits_all_cards_111_111_111_111_110);  
	AssertRange(_srankbitscommon, 0, k_rankbits_all_cards_111_111_111_111_110); 
	AssertRange(_srankbitspoker,  0, k_rankbits_all_cards_111_111_111_111_110); 	
}

int CSymbolEnginePokerval::GetRankHi(int rankbits) {
	for (int i=k_rank_ace; i>=2; i--)	{
		if (IsBitSet(rankbits, i)) return i;
	}
	return 0;
}

int CSymbolEnginePokerval::GetRankLo(int rankbits) {
	for (int i=2; i<=k_rank_ace; i++)	{
		if (IsBitSet(rankbits, i)) return i;
	}
	return 0;
}

void CSymbolEnginePokerval::SetRankBit(int* rankbits, int rank)
{
	*rankbits |= (1 << (rank + 2));		
	if (rank == Rank_ACE)
	{
		*rankbits |= (1<<1);
	}
}

bool CSymbolEnginePokerval::IsHigherStraightPossible(HandVal handval)
{
	assert((HandVal_HANDTYPE(handval) == HandType_STRAIGHT));
	if (StdDeck_RANK(HandVal_TOP_CARD(handval)) == StdDeck_Rank_ACE)
	{
		return false;												
	}
	// Otherwise:
	// Check for all higher 5-card-straight,
	// if only 2 or less are missing
	int n_shift = 10;
	int _rank = 0, suit = 0;
	unsigned int _rankbits_common = 0;

  _rankbits_common = CardMaskToRankBits(_board_cards);

	int current_top_end = StdDeck_RANK(HandVal_TOP_CARD(handval))+2;

	for (int k=14; k>current_top_end; k--)
	{   	
		if (bitcount((_rankbits_common>>n_shift)&0x1f) >= 3)
			return true;

		n_shift--;
	}
	return false;
}

int CSymbolEnginePokerval::CalculatePokerval(HandVal hv, int n, int *pcb, int card0, int card1)
{
	double		pv = 0.;
	int				i = 0, j = 0, k = 0, max = 0, c = 0, flush_suit = 0; //Matrix 2008-06-28
	int				bits = 0;
	CardMask	Cards = {0}, suittestCards = { 0 };

	// If we have name straight flush or flush, figure out the suit
	flush_suit = -1;
	if (HandVal_HANDTYPE(hv)==HandType_STFLUSH || HandVal_HANDTYPE(hv)==HandType_FLUSH)
	{
		// Set up some suit masks
		CardMask_RESET(Cards);
		for (int i=0; i<NumberOfCardsPerPlayer(); i++)
		{
			if (p_table_state->User()->HasKnownCards())
			{
        CardMask_SET(Cards, p_table_state->User()->hole_cards(i)->GetValue());
			}
		}

		for (int i=0; i<kNumberOfCommunityCards; i++)
		{
      Card *card = p_table_state->CommonCards(i);
      if (card->IsKnownCard()) {
        CardMask_SET(Cards, card->GetValue());
			}
		}

		max = 0;
		CardMask_AND(suittestCards, Cards, _spadesCards);
		c = StdDeck_numCards(suittestCards);
		if ( c>max && c>0)
		{
			max = c;
			flush_suit = Suit_SPADES;
		}
		CardMask_AND(suittestCards, Cards, _heartsCards);
		c = StdDeck_numCards(suittestCards);
		if ( c>max && c>0)
		{
			max = c;
			flush_suit = Suit_HEARTS;
		}
		CardMask_AND(suittestCards, Cards, _diamondsCards);
		c = StdDeck_numCards(suittestCards);
		if ( c>max && c>0)
		{
			max = c;
			flush_suit = Suit_DIAMONDS;
		}
		CardMask_AND(suittestCards, Cards, _clubsCards);
		c = StdDeck_numCards(suittestCards);
		if ( c>max && c>0)
		{
			max = c;
			flush_suit = Suit_CLUBS;
		}
	}

	// Now figure out the pokerval and pcbits
	switch HandVal_HANDTYPE(hv)
	{
	case HandType_STFLUSH:
		pv += k_pokerval_straightflush;
		pv += (HandVal_TOP_CARD(hv)+2-0)<<16;
		pv += (HandVal_TOP_CARD(hv)+2-1)<<12;
		pv += (HandVal_TOP_CARD(hv)+2-2)<<8;
		pv += (HandVal_TOP_CARD(hv)+2-3)<<4;
		pv += (HandVal_TOP_CARD(hv)+2-4)<<0;

		for (int i=0; i<kNumberOfCommunityCards; i++)
		{
			j = StdDeck_RANK(card0);	//Matrix 2008-06-28
			k = StdDeck_RANK(card1);
			if ( (j == HandVal_TOP_CARD(hv)-i &&
					StdDeck_SUIT(card0) == flush_suit)
					||
					k == HandVal_TOP_CARD(hv)-i &&
					StdDeck_SUIT(card1) == flush_suit)
			{
				bits |= (1<<(4-i));
			}
			// In straight evaluation an Ace can appear in hv as either 0x0c or 0xff. 
			// We need to do an ugly test for both cases.

			if ((j == 12) || (k == 12))  //Matrix 2008-10-14 !!KLUDGE ALERT!!
			{
				if (j == 12) 
				{
					j=-1;
				}
				if(k == 12)
				{
					k=-1;
				}
				if ((j == HandVal_TOP_CARD(hv)-i && StdDeck_SUIT(card0) == flush_suit)
					||	k == HandVal_TOP_CARD(hv)-i && StdDeck_SUIT(card1) == flush_suit)
				{
					bits |= (1<<(4-i));
				}
			}
		}
		break;

	case HandType_QUADS:
		pv += k_pokerval_fourofakind;
		pv += (HandVal_TOP_CARD(hv)+2)<<16;
		pv += (HandVal_TOP_CARD(hv)+2)<<12;
		pv += (HandVal_TOP_CARD(hv)+2)<<8;
		pv += (HandVal_TOP_CARD(hv)+2)<<4;
		if (n>=5)  pv += (HandVal_SECOND_CARD(hv)+2)<<0;

		// Either hole card is used in quads
		if ( (StdDeck_RANK(card0) == HandVal_TOP_CARD(hv)) !=
				(StdDeck_RANK(card1) == HandVal_TOP_CARD(hv)) )
		{
			bits |= (1<<4);
		}
		// Both hole cards are used in quads
		else if (StdDeck_RANK(card0) == HandVal_TOP_CARD(hv) &&
				 StdDeck_RANK(card1) == HandVal_TOP_CARD(hv))
		{
			bits |= (1<<4);
			bits |= (1<<3);
		}
		// Either hole card is used as kicker
		if (n>=5 &&
				(StdDeck_RANK(card0) == HandVal_SECOND_CARD(hv) ||
				 StdDeck_RANK(card1) == HandVal_SECOND_CARD(hv)))
		{
			bits |= (1<<0);
		}
		break;

	case HandType_FULLHOUSE:
		pv += k_pokerval_fullhouse;
		pv += (HandVal_TOP_CARD(hv)+2)<<16;
		pv += (HandVal_TOP_CARD(hv)+2)<<12;
		pv += (HandVal_TOP_CARD(hv)+2)<<8;
		pv += (HandVal_SECOND_CARD(hv)+2)<<4;
		pv += (HandVal_SECOND_CARD(hv)+2)<<0;

		// Either hole card is used in top of boat
		if ( (StdDeck_RANK(card0) == HandVal_TOP_CARD(hv)) !=
				(StdDeck_RANK(card1) == HandVal_TOP_CARD(hv)) )
		{
			bits |= (1<<4);
		}
		// Both hole cards are used in top of boat
		else if (StdDeck_RANK(card0) == HandVal_TOP_CARD(hv) &&
				 StdDeck_RANK(card1) == HandVal_TOP_CARD(hv))
		{
			bits |= (1<<4);
			bits |= (1<<3);
		}
		// Either hole card is used in bottom of boat
		if ( (StdDeck_RANK(card0) == HandVal_SECOND_CARD(hv))  !=
				(StdDeck_RANK(card1) == HandVal_SECOND_CARD(hv)) )
		{
			bits |= (1<<1);
		}
		// Both hole cards are used in bottom of boat
		else if (StdDeck_RANK(card0) == HandVal_SECOND_CARD(hv) &&
				 StdDeck_RANK(card1) == HandVal_SECOND_CARD(hv))
		{
			bits |= (1<<1);
			bits |= (1<<0);
		}
		break;

	case HandType_FLUSH:
		pv += k_pokerval_flush;
		pv += (HandVal_TOP_CARD(hv)+2)<<16;
		pv += (HandVal_SECOND_CARD(hv)+2)<<12;
		pv += (HandVal_THIRD_CARD(hv)+2)<<8;
		pv += (HandVal_FOURTH_CARD(hv)+2)<<4;
		pv += (HandVal_FIFTH_CARD(hv)+2)<<0;

		if ( (StdDeck_RANK(card0) == HandVal_TOP_CARD(hv) &&
				StdDeck_SUIT(card0) == flush_suit)
				||
				(StdDeck_RANK(card1) == HandVal_TOP_CARD(hv) &&
				 StdDeck_SUIT(card1) == flush_suit) )
		{
			bits |= (1<<4);
		}
		if ( (StdDeck_RANK(card0) == HandVal_SECOND_CARD(hv) &&
				StdDeck_SUIT(card0) == flush_suit)
				||
				(StdDeck_RANK(card1) == HandVal_SECOND_CARD(hv) &&
				 StdDeck_SUIT(card1) == flush_suit) )
		{
			bits |= (1<<3);
		}
		if ( (StdDeck_RANK(card0) == HandVal_THIRD_CARD(hv) &&
				StdDeck_SUIT(card0) == flush_suit)
				||
				(StdDeck_RANK(card1) == HandVal_THIRD_CARD(hv) &&
				 StdDeck_SUIT(card1) == flush_suit) )
		{
			bits |= (1<<2);
		}
		if ( (StdDeck_RANK(card0) == HandVal_FOURTH_CARD(hv) &&
				StdDeck_SUIT(card0) == flush_suit)
				||
				(StdDeck_RANK(card1) == HandVal_FOURTH_CARD(hv) &&
				 StdDeck_SUIT(card1) == flush_suit) )
		{
			bits |= (1<<1);
		}
		if ( (StdDeck_RANK(card0) == HandVal_FIFTH_CARD(hv) &&
				StdDeck_SUIT(card0) == flush_suit)
				||
				(StdDeck_RANK(card1) == HandVal_FIFTH_CARD(hv) &&
				 StdDeck_SUIT(card1) == flush_suit) )
		{
			bits |= (1<<0);
		}
		break;

	case HandType_STRAIGHT:
		pv += k_pokerval_straight;
		pv += (HandVal_TOP_CARD(hv)+2-0)<<16;
		pv += (HandVal_TOP_CARD(hv)+2-1)<<12;
		pv += (HandVal_TOP_CARD(hv)+2-2)<<8;
		pv += (HandVal_TOP_CARD(hv)+2-3)<<4;
		pv += (HandVal_TOP_CARD(hv)+2-4)<<0;
		for (int i=0; i<kNumberOfCommunityCards; i++)
		{
			j = StdDeck_RANK(card0);	//Matrix 2008-06-28
			k = StdDeck_RANK(card1);
			if (j == HandVal_TOP_CARD(hv)-i ||
					k == HandVal_TOP_CARD(hv)-i)
			{
				bits |= (1<<(4-i));
			}
			if((j==12)||(k==12))  //Matrix 2008-10-14 !!KLUDGE ALERT!!
			{
				if(j==12)j=-1;
				if(k==12)k=-1;
				if (j == HandVal_TOP_CARD(hv)-i ||
					k == HandVal_TOP_CARD(hv)-i)
					{
						bits |= (1<<(4-i));
					}
			}

		}
		break;

	case HandType_TRIPS:
		pv += k_pokerval_threeofakind;
		pv += (HandVal_TOP_CARD(hv)+2)<<16;
		pv += (HandVal_TOP_CARD(hv)+2)<<12;
		pv += (HandVal_TOP_CARD(hv)+2)<<8;
		if (n>=4)  pv += (HandVal_SECOND_CARD(hv)+2)<<4;
		if (n>=5)  pv += (HandVal_THIRD_CARD(hv)+2)<<0;

		// either hole card used in trips
		if ( (StdDeck_RANK(card0) == HandVal_TOP_CARD(hv)) !=
				(StdDeck_RANK(card1) == HandVal_TOP_CARD(hv)) )
		{
			bits |= (1<<4);
		}
		// both hole cards used in trips
		else if (StdDeck_RANK(card0) == HandVal_TOP_CARD(hv) &&
				 StdDeck_RANK(card1) == HandVal_TOP_CARD(hv))
		{
			bits |= (1<<4);
			bits |= (1<<3);
		}
		// either hole card is first kicker
		if (n>=4 &&
				(StdDeck_RANK(card0) == HandVal_SECOND_CARD(hv) ||
				 StdDeck_RANK(card1) == HandVal_SECOND_CARD(hv)))
		{
			bits |= (1<<1);
		}
		// either hole card is second kicker
		if (n>=5 &&
				(StdDeck_RANK(card0) == HandVal_THIRD_CARD(hv) ||
				 StdDeck_RANK(card1) == HandVal_THIRD_CARD(hv)))
		{
			bits |= (1<<1);
		}
		break;

	case HandType_TWOPAIR:
		pv += k_pokerval_twopair;
		pv += (HandVal_TOP_CARD(hv)+2)<<16;
		pv += (HandVal_TOP_CARD(hv)+2)<<12;
		pv += (HandVal_SECOND_CARD(hv)+2)<<8;
		pv += (HandVal_SECOND_CARD(hv)+2)<<4;
		if (n>=5)  pv += (HandVal_THIRD_CARD(hv)+2)<<0;

		// either hole card used in top pair
		if ( (StdDeck_RANK(card0) == HandVal_TOP_CARD(hv)) !=
				(StdDeck_RANK(card1) == HandVal_TOP_CARD(hv)) )
		{
			bits |= (1<<4);
		}
		// both hole cards used in top pair
		else if (StdDeck_RANK(card0) == HandVal_TOP_CARD(hv) &&
				 StdDeck_RANK(card1) == HandVal_TOP_CARD(hv))
		{
			bits |= (1<<4);
			bits |= (1<<3);
		}
		// either hole card used in bottom pair
		if ( (StdDeck_RANK(card0) == HandVal_SECOND_CARD(hv)) !=
				(StdDeck_RANK(card1) == HandVal_SECOND_CARD(hv)) )
		{
			bits |= (1<<2);
		}
		// both hole cards used in bottom pair
		else if (StdDeck_RANK(card0) == HandVal_SECOND_CARD(hv) &&
				 StdDeck_RANK(card1) == HandVal_SECOND_CARD(hv))
		{
			bits |= (1<<2);
			bits |= (1<<1);
		}
		// either hole card kicker
		if (n>=5 &&
				(StdDeck_RANK(card0) == HandVal_THIRD_CARD(hv) ||
				 StdDeck_RANK(card1) == HandVal_THIRD_CARD(hv)))
		{
			bits |= (1<<0);
		}
		break;

	case HandType_ONEPAIR:
		pv += k_pokerval_onepair;
		pv += (HandVal_TOP_CARD(hv)+2)<<16;
		pv += (HandVal_TOP_CARD(hv)+2)<<12;
		if (n>=3)  pv += (HandVal_SECOND_CARD(hv)+2)<<8;
		if (n>=4)  pv += (HandVal_THIRD_CARD(hv)+2)<<4;
		if (n>=5)  pv += (HandVal_FOURTH_CARD(hv)+2)<<0;

		// either hole card used in pair
		if ( (StdDeck_RANK(card0) == StdDeck_RANK(HandVal_TOP_CARD(hv))) !=
				(StdDeck_RANK(card1) == StdDeck_RANK(HandVal_TOP_CARD(hv))) )
		{
			bits |= (1<<4);
		}
		// both hole cards used in pair
		else if (StdDeck_RANK(card0) == StdDeck_RANK(HandVal_TOP_CARD(hv)) &&
				 StdDeck_RANK(card1) == StdDeck_RANK(HandVal_TOP_CARD(hv)))
		{
			bits |= (1<<4);
			bits |= (1<<3);
		}
		// either hole card used as first kicker
		if (n>=3 &&
				(StdDeck_RANK(card0) == HandVal_SECOND_CARD(hv) ||
				 StdDeck_RANK(card1) == HandVal_SECOND_CARD(hv)))
		{
			bits |= (1<<2);
		}
		// either hole card used as second kicker
		if (n>=4 &&
				(StdDeck_RANK(card0) == HandVal_THIRD_CARD(hv) ||
				 StdDeck_RANK(card1) == HandVal_THIRD_CARD(hv)))
		{
			bits |= (1<<1);
		}
		// either hole card used as third kicker
		if (n>=5 &&
				(StdDeck_RANK(card0) == HandVal_FOURTH_CARD(hv) ||
				 StdDeck_RANK(card1) == HandVal_FOURTH_CARD(hv)))
		{
			bits |= (1<<0);
		}
		break;

	case HandType_NOPAIR:
		if (n>=1)  pv += (HandVal_TOP_CARD(hv)+2)<<16;
		if (n>=2)  pv += (HandVal_SECOND_CARD(hv)+2)<<12;
		if (n>=3)  pv += (HandVal_THIRD_CARD(hv)+2)<<8;
		if (n>=4)  pv += (HandVal_FOURTH_CARD(hv)+2)<<4;
		if (n>=5)  pv += (HandVal_FIFTH_CARD(hv)+2)<<0;

		// either hole card used as top card
		if (n>=1 &&
				(StdDeck_RANK(card0) == HandVal_TOP_CARD(hv) ||
				 StdDeck_RANK(card1) == HandVal_TOP_CARD(hv) ))
		{
			bits |= (1<<4);
		}
		// either hole card used as first kicker
		if (n>=2 &&
				(StdDeck_RANK(card0) == HandVal_SECOND_CARD(hv) ||
				 StdDeck_RANK(card1) == HandVal_SECOND_CARD(hv) ))
		{
			bits |= (1<<3);
		}
		// either hole card used as second kicker
		if (n>=3 &&
				(StdDeck_RANK(card0) == HandVal_THIRD_CARD(hv) ||
				 StdDeck_RANK(card1) == HandVal_THIRD_CARD(hv) ))
		{
			bits |= (1<<2);
		}
		// either hole card used as third kicker
		if (n>=3 &&
				(StdDeck_RANK(card0) == HandVal_FOURTH_CARD(hv) ||
				 StdDeck_RANK(card1) == HandVal_FOURTH_CARD(hv) ))
		{
			bits |= (1<<1);
		}
		// either hole card used as fourth kicker
		if (n>=3 &&
				(StdDeck_RANK(card0) == HandVal_FIFTH_CARD(hv) ||
				 StdDeck_RANK(card1) == HandVal_FIFTH_CARD(hv) ))
		{
			bits |= (1<<0);
		}
		break;
	}

	*pcb = bits;
	return pv;
}

bool CSymbolEnginePokerval::EvaluateSymbol(const CString name, double *result, bool log /* = false */)
{
  FAST_EXIT_ON_OPENPPL_SYMBOLS(name);
	if (memcmp(name, "rankhi", 6) == 0)
	{
		if (memcmp(name, "rankhi", 6)==0 && strlen(name)==6)
		{
			*result = rankhi();
		}
		else if (memcmp(name, "rankhicommon", 12)==0 && strlen(name)==12)
		{
			*result = rankhicommon();
		}
		else if (memcmp(name, "rankhiplayer", 12)==0 && strlen(name)==12)
		{
			*result = rankhiplayer();
		}
		else if (memcmp(name, "rankhipoker", 11)==0 && strlen(name)==11)
		{
      WarnIfSymbolIsHoldemOnly(name);
			*result = rankhipoker();
		}
		else
		{
			// Invalid symbol
			return false;
		}
		// Valid symbol
		return true;
	}
	else if (memcmp(name, "srankhi", 7) == 0)
	{
		if (memcmp(name, "srankhi", 7)==0 && strlen(name)==7)		
		{
			*result = srankhi();
		}
		else if (memcmp(name, "srankhicommon", 13)==0 && strlen(name)==13)
		{
			*result = srankhicommon();
		}
		else if (memcmp(name, "srankhiplayer", 13)==0 && strlen(name)==13)	
		{
			*result = srankhiplayer();
		}
		else if (memcmp(name, "srankhipoker", 12)==0 && strlen(name)==12)	
		{
      WarnIfSymbolIsHoldemOnly(name);
			*result = srankhipoker();
		}
		else
		{
			// Invalid symbol
			return false;
		}
		// Valid symbol
		return true;
	}
	else if (memcmp(name, "ranklo", 6) == 0)
	{
		if (memcmp(name, "ranklo", 6)==0 && strlen(name)==6)
		{
			*result = ranklo();
		}
		else if (memcmp(name, "ranklocommon", 12)==0 && strlen(name)==12)
		{
			*result = ranklocommon();
		}
		else if (memcmp(name, "rankloplayer", 12)==0 && strlen(name)==12)
		{
			*result = rankloplayer();
		}
		else if (memcmp(name, "ranklopoker", 11)==0 && strlen(name)==11)	
		{
      WarnIfSymbolIsHoldemOnly(name);
			*result = ranklopoker();
		}
		else
		{
			// Invalid symbol
			return false;
		}
		// Valid symbol
		return true;
	}
	else if (memcmp(name, "sranklo", 7) == 0)
	{
		if (memcmp(name, "sranklo", 7)==0 && strlen(name)==7)		
		{
			*result = sranklo();
		}
		else if (memcmp(name, "sranklocommon", 13)==0 && strlen(name)==13)	
		{
			*result = sranklocommon();
		}
		else if (memcmp(name, "srankloplayer", 13)==0 && strlen(name)==13)	
		{
			*result = srankloplayer();
		}
		else if (memcmp(name, "sranklopoker", 12)==0 && strlen(name)==12)	
		{
      WarnIfSymbolIsHoldemOnly(name);
			*result = sranklopoker();
		}
		else 
		{
			// Invalid symbol
			return false;
		}
		// Valid symbol
		return true;
	}
	else if (memcmp(name, "pokerval", 8)==0)
	{
		if (memcmp(name, "pokerval", 8)==0 && strlen(name)==8)	
		{
      WarnIfSymbolIsHoldemOnly(name);
			*result = pokerval();
		}
		else if (memcmp(name, "pokervalplayer", 14)==0 && strlen(name)==14)
		{
      WarnIfSymbolIsHoldemOnly(name);
			*result = pokervalplayer();
		}
		else if (memcmp(name, "pokervalcommon", 14)==0 && strlen(name)==14)
		{
      WarnIfSymbolIsHoldemOnly(name);
			*result = pokervalcommon();
		}
		else 
		{
			// Invalid symbol
			return false;
		}
		// Valid symbol
		return true;
	}
  else if (memcmp(name, "suitbits", 8) == 0)
  {
    int right_most_digit = RightDigitCharacterToNumber(name);
    if (memcmp(name, "suitbitsplayer", 14) == 0 && strlen(name) == 15)
    {
      *result = suitbitsplayer(right_most_digit);
    }
    else if (memcmp(name, "suitbitscommon", 14) == 0 && strlen(name) == 15)
    {
      *result = suitbitscommon(right_most_digit);
    }
    else
    {
      // Invalid symbol
      return false;
    }
    // Valid symbol
    return true;
  }
	else if (memcmp(name, "rankbits", 8)==0)
	{
		if (memcmp(name, "rankbits", 8)==0 && strlen(name)==8)
		{
			*result = rankbits();
		}
		else if (memcmp(name, "rankbitscommon", 14)==0 && strlen(name)==14)
		{
			*result = rankbitscommon();
		}
		else if (memcmp(name, "rankbitsplayer", 14)==0 && strlen(name)==14)
		{
			*result = rankbitsplayer();
		}
		else if (memcmp(name, "rankbitspoker", 13)==0 && strlen(name)==13)	
		{
      WarnIfSymbolIsHoldemOnly(name);
			*result = rankbitspoker();
		}
		else 
		{
			// Invalid symbol
			return false;
		}
		// Valid symbol
		return true;
	}
	else if (memcmp(name, "srankbits", 9)==0)
	{
		if (memcmp(name, "srankbits", 9)==0 && strlen(name)==9)		
		{
			*result = srankbits();
		}
		else if (memcmp(name, "srankbitscommon", 15)==0 && strlen(name)==15)
		{
			*result = srankbitscommon();
		}
		else if (memcmp(name, "srankbitsplayer", 15)==0 && strlen(name)==15)
		{
			*result = srankbitsplayer();
		}
		else if (memcmp(name, "srankbitspoker", 14)==0 && strlen(name)==14)
		{
      WarnIfSymbolIsHoldemOnly(name);
			*result = srankbitspoker();
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
		if (memcmp(name, "ishandup", 8)==0 && strlen(name)==8)
		{
      WarnIfSymbolIsHoldemOnly(name);
			*result = ishandup(); 
		}
		else if (memcmp(name, "ishandupcommon", 14)==0 && strlen(name)==14)
		{
      WarnIfSymbolIsHoldemOnly(name);
			*result = ishandupcommon();
		}
		else if (memcmp(name, "ishicard", 8)==0 && strlen(name)==8)	
		{
      WarnIfSymbolIsHoldemOnly(name);
			*result = ishicard();
		}
		else if (memcmp(name, "isonepair", 9)==0 && strlen(name)==9)	
		{
      WarnIfSymbolIsHoldemOnly(name);
			*result = isonepair();
		}
		else if (memcmp(name, "istwopair", 9)==0 && strlen(name)==9)		
		{
      WarnIfSymbolIsHoldemOnly(name);
			*result = istwopair();
		}
		else if (memcmp(name, "isthreeofakind", 14)==0 && strlen(name)==14)
		{
      WarnIfSymbolIsHoldemOnly(name);
			*result = isthreeofakind();
		}
		else if (memcmp(name, "isstraight", 10)==0 && strlen(name)==10)	
		{
      WarnIfSymbolIsHoldemOnly(name);
			*result = isstraight();
		}
		else if (memcmp(name, "isflush", 7)==0 && strlen(name)==7)			
		{
      WarnIfSymbolIsHoldemOnly(name);
			*result = isflush();
		}
		else if (memcmp(name, "isfullhouse", 11)==0 && strlen(name)==11)	
		{
      WarnIfSymbolIsHoldemOnly(name);
			*result = isfullhouse();
		}
		else if (memcmp(name, "isfourofakind", 13)==0 && strlen(name)==13)	
		{
      WarnIfSymbolIsHoldemOnly(name);
			*result = isfourofakind();
		}
		else if (memcmp(name, "isstraightflush", 15)==0 && strlen(name)==15)
		{
      WarnIfSymbolIsHoldemOnly(name);
			*result = isstraightflush();
		}
		else if (memcmp(name, "isroyalflush", 12)==0 && strlen(name)==12)
		{
      WarnIfSymbolIsHoldemOnly(name);
			*result = isroyalflush();
		}
		else if (memcmp(name, "ishipair", 8)==0 && strlen(name)==8)	
		{
      WarnIfSymbolIsHoldemOnly(name);
			*result = ishipair();
		}
		else if (memcmp(name, "islopair", 8)==0 && strlen(name)==8)	
		{
      WarnIfSymbolIsHoldemOnly(name);
			*result = islopair();
		}
		else if (memcmp(name, "ismidpair", 9)==0 && strlen(name)==9)		
		{
      WarnIfSymbolIsHoldemOnly(name);
			*result = ismidpair();
		}
		else if (memcmp(name, "ishistraight", 12)==0 && strlen(name)==12)
		{
      WarnIfSymbolIsHoldemOnly(name);
			*result = ishistraight();
		}
		else if (memcmp(name, "ishiflush", 9)==0 && strlen(name)==9)	
		{
      WarnIfSymbolIsHoldemOnly(name);
			*result = ishiflush();
		}
		else 
		{
			// Invalid symbol
			return false;
		}
		// Valid symbol
		return true;
	}
	// Various symbols without grouping
	else if (memcmp(name, "pcbits", 6)==0 && strlen(name)==6)	
	{
    WarnIfSymbolIsHoldemOnly(name);
		*result = pcbits();
	}
	else if (memcmp(name, "npcbits", 7)==0 && strlen(name)==7)		
	{
    WarnIfSymbolIsHoldemOnly(name);
		*result = npcbits();
	}
	else if (memcmp(name, "hicard", 6)==0 && strlen(name)==6)		
	{
		*result = k_pokerval_hicard;
	}
	else if (memcmp(name, "onepair", 7)==0 && strlen(name)==7)		
	{
		*result = k_pokerval_onepair;
	}
	else if (memcmp(name, "twopair", 7)==0 && strlen(name)==7)			
	{
		*result = k_pokerval_twopair;
	}
	else if (memcmp(name, "threeofakind", 12)==0 && strlen(name)==12)
	{
		*result = k_pokerval_threeofakind;
	}
	else if (memcmp(name, "straight", 8)==0 && strlen(name)==8)	
	{
		*result = k_pokerval_straight;
	}
	else if (memcmp(name, "flush", 5)==0 && strlen(name)==5)		
	{
		*result = k_pokerval_flush;
	}
	else if (memcmp(name, "fullhouse", 9)==0 && strlen(name)==9)	
	{
		*result = k_pokerval_fullhouse;
	}
	else if (memcmp(name, "fourofakind", 11)==0 && strlen(name)==11)	
	{
		*result = k_pokerval_fourofakind;
	}
	else if (memcmp(name, "straightflush", 13)==0 && strlen(name)==13)
	{
		*result = k_pokerval_straightflush;
	}
	else if (memcmp(name, "royalflush", 10)==0 && strlen(name)==10)	
	{
		*result = k_pokerval_royalflush;
	}
	else
	{
		// Symbol of a different symbol-engine
		return false;
	}
	// Valid symbol
	return true;
}

CString CSymbolEnginePokerval::SymbolsProvided() {
  CString list_of_symbols = "rankhi rankhicommon rankhiplayer rankhipoker "
    "srankhi srankhicommon srankhiplayer srankhipoker "
    "ranklo ranklocommon rankloplayer ranklopoker "
    "sranklo sranklocommon srankloplayer sranklopoker "
    "pokerval pokervalplayer pokervalcommon "
    "rankbits rankbitscommon rankbitsplayer rankbitspoker "
    "srankbits srankbitscommon srankbitsplayer srankbitspoker "
    "ishandup ishandupcommon "
    "ishicard isonepair istwopair isthreeofakind isstraight "
    "isflush isfullhouse isfourofakind isstraightflush isroyalflush "
    "ishipair islopair ismidpair "
    "ishistraight ishiflush "
    "pcbits npcbits "
    "hicard onepair twopair threeofakind straight "
    "flush fullhouse fourofakind straightflush royalflush ";
  // More verbose naming available via the multiplexer engine
  list_of_symbols += RangeOfSymbols("suitbitsplayer%d", StdDeck_Suit_FIRST, StdDeck_Suit_LAST);
  list_of_symbols += RangeOfSymbols("suitbitscommon%d", StdDeck_Suit_FIRST, StdDeck_Suit_LAST);
  return list_of_symbols;
}
