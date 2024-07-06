/*
 * Copyright (C) 2004-2006
 *           Michael Maurer <mjmaurer@yahoo.com>
 *           Brian Goetz <brian@quiotix.com>
 *           Loic Dachary <loic@dachary.org>
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
/*
   Note that this file has two #if .. #endif sections -- one for 
   StdDeck macros to prevent double-inclusion, and one to define the 
   generic Deck_ macros if DECK_STANDARD is defined 
*/

#ifndef __DECK_STD_H__
#define __DECK_STD_H__

#include "pokereval_export.h"

#define StdDeck_N_CARDS      52
#define StdDeck_MASK(index)  (StdDeck_cardMasksTable[index])

#define StdDeck_Rank_2      0
#define StdDeck_Rank_3      1
#define StdDeck_Rank_4      2
#define StdDeck_Rank_5      3
#define StdDeck_Rank_6      4
#define StdDeck_Rank_7      5
#define StdDeck_Rank_8      6
#define StdDeck_Rank_9      7
#define StdDeck_Rank_TEN    8
#define StdDeck_Rank_JACK   9
#define StdDeck_Rank_QUEEN  10
#define StdDeck_Rank_KING   11
#define StdDeck_Rank_ACE    12
#define StdDeck_Rank_COUNT  13
#define StdDeck_Rank_FIRST  StdDeck_Rank_2
#define StdDeck_Rank_LAST   StdDeck_Rank_ACE
#define StdDeck_N_RANKMASKS (1 << StdDeck_Rank_COUNT)

#define StdDeck_RANK(index)  ((index) % StdDeck_Rank_COUNT)
#define StdDeck_SUIT(index)  ((index) / StdDeck_Rank_COUNT)
#define StdDeck_MAKE_CARD(rank, suit) ((suit * StdDeck_Rank_COUNT) + rank)

#define StdDeck_Suit_HEARTS   0
#define StdDeck_Suit_DIAMONDS 1
#define StdDeck_Suit_CLUBS    2
#define StdDeck_Suit_SPADES   3
#define StdDeck_Suit_FIRST    StdDeck_Suit_HEARTS
#define StdDeck_Suit_LAST     StdDeck_Suit_SPADES
#define StdDeck_Suit_COUNT    4

typedef uint32 StdDeck_RankMask;

typedef union {
#ifdef USE_INT64
  uint64  cards_n;
#else
  struct {
    uint32 n1, n2;
  } cards_nn;
#endif
  struct {
    /* There are multiple ways to define these fields.  We could pack the
       13-bit fields together, we could define 13-bit fields with 3-bit
       padding between them, or we can define them as 16-bit fields.  
       Which is best will depend on the particular characteristics of the
       processor.  
       If we are on a big-endian processor, the padding should precede the
       field; on a little-endian processor, the padding should come after
       the field.  
       I found that uint32 was faster than uint16 on a lot of processors --
       the 16-bit instructions can be slow. 
    */
#ifdef WORDS_BIGENDIAN
    uint32         : 3;
    uint32 spades  :13;
    uint32         : 3;
    uint32 clubs   :13;
    uint32         : 3;
    uint32 diamonds:13;
    uint32         : 3;
    uint32 hearts  :13;
#else
    uint32 spades  :13;
    uint32         : 3;
    uint32 clubs   :13;
    uint32         : 3;
    uint32 diamonds:13;
    uint32         : 3;
    uint32 hearts  :13;
    uint32         : 3;
#endif
  } cards;
} StdDeck_CardMask;

#define StdDeck_CardMask_SPADES(cm)   ((cm).cards.spades)
#define StdDeck_CardMask_CLUBS(cm)    ((cm).cards.clubs)
#define StdDeck_CardMask_DIAMONDS(cm) ((cm).cards.diamonds)
#define StdDeck_CardMask_HEARTS(cm)   ((cm).cards.hearts)

#define StdDeck_CardMask_SET_SPADES(cm, ranks)   ((cm).cards.spades=(ranks))
#define StdDeck_CardMask_SET_CLUBS(cm, ranks)    ((cm).cards.clubs=(ranks))
#define StdDeck_CardMask_SET_DIAMONDS(cm, ranks) ((cm).cards.diamonds=(ranks))
#define StdDeck_CardMask_SET_HEARTS(cm, ranks)   ((cm).cards.hearts=(ranks))

#ifdef USE_INT64
#define StdDeck_CardMask_OP(result, op1, op2, OP) \
  LongLong_OP((result).cards_n, (op1).cards_n, (op2).cards_n, OP);
#else
#define StdDeck_CardMask_OP(result, op1, op2, OP) \
  do {                                                                  \
    (result.cards_nn.n1) = (op1.cards_nn.n1) OP (op2.cards_nn.n1);      \
    (result.cards_nn.n2) = (op1.cards_nn.n2) OP (op2.cards_nn.n2);      \
} while (0)
#endif

#ifdef USE_INT64
#define StdDeck_CardMask_NOT(result, op1)                               \
  do {                                                                  \
    (result).cards_n = ~(op1).cards_n;                                  \
  } while (0)
#else
#define StdDeck_CardMask_NOT(result, op1)                               \
  do {                                                                  \
    (result).cards_nn.n1 = ~(op1).cards_nn.n1;                          \
    (result).cards_nn.n2 = ~(op1).cards_nn.n2;                          \
  } while (0)
#endif

#define StdDeck_CardMask_OR(result, op1, op2) \
  StdDeck_CardMask_OP(result, op1, op2, |)

#define StdDeck_CardMask_AND(result, op1, op2) \
  StdDeck_CardMask_OP(result, op1, op2, &)

#define StdDeck_CardMask_XOR(result, op1, op2) \
  StdDeck_CardMask_OP(result, op1, op2, ^)

#define StdDeck_CardMask_SET(mask, index)       \
do {                                            \
  StdDeck_CardMask _t1 = StdDeck_MASK(index);           \
  StdDeck_CardMask_OR((mask), (mask), _t1);             \
} while (0)

#define StdDeck_CardMask_UNSET(mask, index)       	\
do {                                            	\
  StdDeck_CardMask _t1 = StdDeck_MASK(index);           \
  StdDeck_CardMask_NOT(_t1, _t1);	             	\
  StdDeck_CardMask_AND((mask), (mask), _t1);            \
} while (0)

#ifdef USE_INT64                                                          
#define StdDeck_CardMask_CARD_IS_SET(mask, index)                       \
  (( (mask).cards_n & (StdDeck_MASK(index).cards_n)) != 0 )                 
#else                                                                   
/* TODO: this looks wrong; should be ((mask.n1 & i.n1 != 0) || (i.n1 == 0)) */
#define StdDeck_CardMask_CARD_IS_SET(mask, index)                       \
  ((( (mask).cards_nn.n1 & (StdDeck_MASK(index).cards_nn.n1)) != 0 )    \
   || (( (mask).cards_nn.n2 & (StdDeck_MASK(index).cards_nn.n2)) != 0 ))   
#endif

#ifdef USE_INT64                                                          
#define StdDeck_CardMask_ANY_SET(mask1, mask2)                          \
  (( (mask1).cards_n & (mask2).cards_n) != 0 )                 
#else                                                                   
#define StdDeck_CardMask_ANY_SET(mask1, mask2)                           \
  ((( (mask1).cards_nn.n1 & (mask2).cards_nn.n1) != 0 )                 \
   || (( (mask1).cards_nn.n2 & (mask2).cards_nn.n2) != 0 ))           
#endif

#ifdef USE_INT64
#define StdDeck_CardMask_RESET(mask) \
  do { (mask).cards_n = 0; } while (0)
#else
#define StdDeck_CardMask_RESET(mask) \
  do { (mask).cards_nn.n1 = (mask).cards_nn.n2 = 0; } while (0)
#endif

#ifdef USE_INT64
#define StdDeck_CardMask_IS_EMPTY(mask) \
  ((mask).cards_n == 0)
#else
#define StdDeck_CardMask_IS_EMPTY(mask) \
  ((mask).cards_nn.n1 == 0 && (mask).cards_nn.n2 == 0)
#endif

#ifdef USE_INT64
#define StdDeck_CardMask_EQUAL(mask1, mask2) \
  ((mask1).cards_n == (mask2).cards_n)
#else
#define StdDeck_CardMask_EQUAL(mask1, mask2) \
  ((mask1).cards_nn.n1 == (mask2).cards_nn.n1 && (mask1).cards_nn.n2 == (mask2).cards_nn.n2)
#endif

/*
#ifdef __cplusplus
extern "C" {
#endif
*/
extern POKEREVAL_EXPORT uint8             nBitsTable[StdDeck_N_RANKMASKS];
extern POKEREVAL_EXPORT uint32     topFiveCardsTable[StdDeck_N_RANKMASKS];
extern POKEREVAL_EXPORT uint32       topFiveBitTable[StdDeck_N_RANKMASKS];
extern POKEREVAL_EXPORT uint8           topCardTable[StdDeck_N_RANKMASKS];
extern POKEREVAL_EXPORT uint32           topBitTable[StdDeck_N_RANKMASKS];
extern POKEREVAL_EXPORT uint32       topTwoBitsTable[StdDeck_N_RANKMASKS];
extern POKEREVAL_EXPORT uint32      topFiveBitsTable[StdDeck_N_RANKMASKS];
extern POKEREVAL_EXPORT uint8          straightTable[StdDeck_N_RANKMASKS];
/*
#ifdef __cplusplus
}
#endif
*/
extern POKEREVAL_EXPORT uint32  bottomFiveCardsTable[StdDeck_N_RANKMASKS];
extern POKEREVAL_EXPORT uint32  bottomFiveJokerTable[StdDeck_N_RANKMASKS];
extern POKEREVAL_EXPORT uint8        bottomCardTable[StdDeck_N_RANKMASKS];

extern POKEREVAL_EXPORT StdDeck_CardMask StdDeck_cardMasksTable[StdDeck_N_CARDS];

extern POKEREVAL_EXPORT char StdDeck_rankChars[StdDeck_Rank_LAST+1];
extern POKEREVAL_EXPORT char StdDeck_suitChars[StdDeck_Suit_LAST+1];

extern POKEREVAL_EXPORT int StdDeck_cardToString(int cardIndex, char *outString);
extern POKEREVAL_EXPORT int StdDeck_stringToCard(char *inString, int *outCard);

#define StdDeck_cardString(i) GenericDeck_cardString(&StdDeck, (i))
#define StdDeck_printCard(i)  GenericDeck_printCard(&StdDeck, (i))
#define StdDeck_printMask(m)  GenericDeck_printMask(&StdDeck, ((void *) &(m)))
#define StdDeck_maskString(m) GenericDeck_maskString(&StdDeck, ((void *) &(m)))
#define StdDeck_numCards(m) GenericDeck_numCards(&StdDeck, ((void *) &(m)))
#define StdDeck_maskToString(m, s) GenericDeck_maskToString(&StdDeck, ((void *) &(m)), (s))

extern POKEREVAL_EXPORT Deck StdDeck;


#endif




#ifdef DECK_STANDARD

#if defined(Deck_N_CARDS)
#include "deck_undef.h"
#endif

#define Deck_N_CARDS      StdDeck_N_CARDS
#define Deck_MASK         StdDeck_MASK
#define Deck_RANK         StdDeck_RANK
#define Deck_SUIT         StdDeck_SUIT

#define Rank_2            StdDeck_Rank_2 
#define Rank_3            StdDeck_Rank_3
#define Rank_4            StdDeck_Rank_4 
#define Rank_5            StdDeck_Rank_5 
#define Rank_6            StdDeck_Rank_6 
#define Rank_7            StdDeck_Rank_7 
#define Rank_8            StdDeck_Rank_8 
#define Rank_9            StdDeck_Rank_9 
#define Rank_TEN          StdDeck_Rank_TEN
#define Rank_JACK         StdDeck_Rank_JACK
#define Rank_QUEEN        StdDeck_Rank_QUEEN
#define Rank_KING         StdDeck_Rank_KING
#define Rank_ACE          StdDeck_Rank_ACE
#define Rank_FIRST        StdDeck_Rank_FIRST 
#define Rank_COUNT        StdDeck_Rank_COUNT

#define Suit_HEARTS       StdDeck_Suit_HEARTS
#define Suit_DIAMONDS     StdDeck_Suit_DIAMONDS
#define Suit_CLUBS        StdDeck_Suit_CLUBS
#define Suit_SPADES       StdDeck_Suit_SPADES
#define Suit_FIRST        StdDeck_Suit_FIRST
#define Suit_COUNT        StdDeck_Suit_COUNT

#define CardMask               StdDeck_CardMask 
#define CardMask_NOT           StdDeck_CardMask_NOT
#define CardMask_OR            StdDeck_CardMask_OR
#define CardMask_XOR           StdDeck_CardMask_XOR
#define CardMask_AND           StdDeck_CardMask_AND
#define CardMask_SET           StdDeck_CardMask_SET
#define CardMask_CARD_IS_SET   StdDeck_CardMask_CARD_IS_SET
#define CardMask_ANY_SET       StdDeck_CardMask_ANY_SET
#define CardMask_RESET         StdDeck_CardMask_RESET
#define CardMask_UNSET         StdDeck_CardMask_UNSET

#define CardMask_SPADES        StdDeck_CardMask_SPADES
#define CardMask_HEARTS        StdDeck_CardMask_HEARTS
#define CardMask_CLUBS         StdDeck_CardMask_CLUBS
#define CardMask_DIAMONDS      StdDeck_CardMask_DIAMONDS

#define CardMask_SET_SPADES    StdDeck_CardMask_SET_SPADES
#define CardMask_SET_HEARTS    StdDeck_CardMask_SET_HEARTS
#define CardMask_SET_CLUBS     StdDeck_CardMask_SET_CLUBS
#define CardMask_SET_DIAMONDS  StdDeck_CardMask_SET_DIAMONDS

#define CurDeck StdDeck

#endif 
