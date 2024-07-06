/*
 *  Copyright 2006 Loic Dachary <loic@dachary.org> 
 *
 *  This package is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; version 2 dated June, 1991.
 *
 *  This package is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this package; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
 *  MA 02110-1301, USA.
 */

#ifndef __POKER_WRAPPER_H__
#define __POKER_WRAPPER_H__

#include <pokereval_export.h>

#include <deck_std.h>

extern POKEREVAL_EXPORT unsigned int wrap_StdDeck_N_CARDS();
extern POKEREVAL_EXPORT StdDeck_CardMask wrap_StdDeck_MASK(int index);
extern POKEREVAL_EXPORT unsigned int wrap_StdDeck_Rank_2();
extern POKEREVAL_EXPORT unsigned int wrap_StdDeck_Rank_3();
extern POKEREVAL_EXPORT unsigned int wrap_StdDeck_Rank_4();
extern POKEREVAL_EXPORT unsigned int wrap_StdDeck_Rank_5();
extern POKEREVAL_EXPORT unsigned int wrap_StdDeck_Rank_6();
extern POKEREVAL_EXPORT unsigned int wrap_StdDeck_Rank_7();
extern POKEREVAL_EXPORT unsigned int wrap_StdDeck_Rank_8();
extern POKEREVAL_EXPORT unsigned int wrap_StdDeck_Rank_9();
extern POKEREVAL_EXPORT unsigned int wrap_StdDeck_Rank_TEN();
extern POKEREVAL_EXPORT unsigned int wrap_StdDeck_Rank_JACK();
extern POKEREVAL_EXPORT unsigned int wrap_StdDeck_Rank_QUEEN();
extern POKEREVAL_EXPORT unsigned int wrap_StdDeck_Rank_KING();
extern POKEREVAL_EXPORT unsigned int wrap_StdDeck_Rank_ACE();
extern POKEREVAL_EXPORT unsigned int wrap_StdDeck_Rank_COUNT();
extern POKEREVAL_EXPORT unsigned int wrap_StdDeck_Rank_FIRST();
extern POKEREVAL_EXPORT unsigned int wrap_StdDeck_Rank_LAST();
extern POKEREVAL_EXPORT unsigned int wrap_StdDeck_RANK(unsigned int index);
extern POKEREVAL_EXPORT unsigned int wrap_StdDeck_SUIT(unsigned int index);
extern POKEREVAL_EXPORT unsigned int wrap_StdDeck_MAKE_CARD(unsigned int rank, unsigned int suit);
extern POKEREVAL_EXPORT unsigned int wrap_StdDeck_Suit_HEARTS();
extern POKEREVAL_EXPORT unsigned int wrap_StdDeck_Suit_DIAMONDS();
extern POKEREVAL_EXPORT unsigned int wrap_StdDeck_Suit_CLUBS();
extern POKEREVAL_EXPORT unsigned int wrap_StdDeck_Suit_SPADES();
extern POKEREVAL_EXPORT unsigned int wrap_StdDeck_Suit_FIRST();
extern POKEREVAL_EXPORT unsigned int wrap_StdDeck_Suit_LAST();
extern POKEREVAL_EXPORT unsigned int wrap_StdDeck_Suit_COUNT();
extern POKEREVAL_EXPORT unsigned int wrap_StdDeck_CardMask_SPADES(StdDeck_CardMask cm);
extern POKEREVAL_EXPORT unsigned int wrap_StdDeck_CardMask_CLUBS(StdDeck_CardMask cm);
extern POKEREVAL_EXPORT unsigned int wrap_StdDeck_CardMask_DIAMONDS(StdDeck_CardMask cm);
extern POKEREVAL_EXPORT unsigned int wrap_StdDeck_CardMask_HEARTS(StdDeck_CardMask cm);
extern POKEREVAL_EXPORT StdDeck_CardMask wrap_StdDeck_CardMask_SET_HEARTS(StdDeck_CardMask cm, unsigned int ranks);
extern POKEREVAL_EXPORT StdDeck_CardMask wrap_StdDeck_CardMask_SET_DIAMONDS(StdDeck_CardMask cm, unsigned int ranks);
extern POKEREVAL_EXPORT StdDeck_CardMask wrap_StdDeck_CardMask_SET_CLUBS(StdDeck_CardMask cm, unsigned int ranks);
extern POKEREVAL_EXPORT StdDeck_CardMask wrap_StdDeck_CardMask_SET_SPADES(StdDeck_CardMask cm, unsigned int ranks);
extern POKEREVAL_EXPORT StdDeck_CardMask wrap_StdDeck_CardMask_NOT(StdDeck_CardMask cm);
extern POKEREVAL_EXPORT StdDeck_CardMask wrap_StdDeck_CardMask_OR(StdDeck_CardMask op1, StdDeck_CardMask op2);
extern POKEREVAL_EXPORT StdDeck_CardMask wrap_StdDeck_CardMask_AND(StdDeck_CardMask op1, StdDeck_CardMask op2);
extern POKEREVAL_EXPORT StdDeck_CardMask wrap_StdDeck_CardMask_XOR(StdDeck_CardMask op1, StdDeck_CardMask op2);
extern POKEREVAL_EXPORT StdDeck_CardMask wrap_StdDeck_CardMask_SET(StdDeck_CardMask mask, unsigned int index);
extern POKEREVAL_EXPORT StdDeck_CardMask wrap_StdDeck_CardMask_UNSET(StdDeck_CardMask mask, unsigned int index);
extern POKEREVAL_EXPORT int wrap_StdDeck_CardMask_CARD_IS_SET(StdDeck_CardMask mask, unsigned int index);
extern POKEREVAL_EXPORT int wrap_StdDeck_CardMask_ANY_SET(StdDeck_CardMask mask1, StdDeck_CardMask mask2);
extern POKEREVAL_EXPORT StdDeck_CardMask wrap_StdDeck_CardMask_RESET();
extern POKEREVAL_EXPORT int wrap_StdDeck_CardMask_IS_EMPTY(StdDeck_CardMask mask);
extern POKEREVAL_EXPORT int wrap_StdDeck_CardMask_EQUAL(StdDeck_CardMask mask1, StdDeck_CardMask mask2);

#endif /* __POKER_WRAPPER_H__ */
