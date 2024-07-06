/*
 *  eval_type.h: a fast poker hand evaluator -- evaluates only the hand type
 *  Should be small enough to stay in the L1 cache, if we're lucky.  Uses only
 *  one 8K table.  
 *
 *  Copyright (C) 1999-2002
 *           Michael Maurer <mjmaurer@yahoo.com>
 *           Brian Goetz <brian@quiotix.com>
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

#ifndef __EVAL_TYPE_H__
#define __EVAL_TYPE_H__

#include "poker_defs.h"

/* This table packs n_bits, is_straight, and (n_bits > 5) into one byte,
   as follows:
   Bit 0:   n_bits > 5
   Bit 1:   is_straight
   Bit 2-6: n_bits
*/
extern "C" uint8 nBitsAndStrTable[StdDeck_N_RANKMASKS];

static inline int 
StdDeck_StdRules_EVAL_TYPE( StdDeck_CardMask cards, int n_cards )
{
  uint32 ranks, four_mask, three_mask, two_mask, 
    n_dups, n_ranks, is_st_or_fl = 0, t, sc, sd, sh, ss;

  sc = StdDeck_CardMask_CLUBS(cards);
  sd = StdDeck_CardMask_DIAMONDS(cards);
  sh = StdDeck_CardMask_HEARTS(cards);
  ss = StdDeck_CardMask_SPADES(cards);

  ranks = sc | sd | sh | ss;
  n_ranks = nBitsAndStrTable[ranks] >> 2;
  n_dups = n_cards - n_ranks;

  if (nBitsAndStrTable[ranks] & 0x01) { /* if n_ranks > 5 */
    if (nBitsAndStrTable[ranks] & 0x02)
      is_st_or_fl = StdRules_HandType_STRAIGHT;

    t = nBitsAndStrTable[ss] | nBitsAndStrTable[sc]
      | nBitsAndStrTable[sd] | nBitsAndStrTable[sh];

    if (t & 0x01) {
      if (t & 0x02) 
        return StdRules_HandType_STFLUSH;
      else 
        is_st_or_fl = StdRules_HandType_FLUSH;
    };

    if (is_st_or_fl && n_dups < 3)
      return is_st_or_fl;
  };

  switch (n_dups) {
  case 0:
    return StdRules_HandType_NOPAIR;
    break;

  case 1:
    return StdRules_HandType_ONEPAIR;
    break;

  case 2:
    two_mask = ranks ^ (sc ^ sd ^ sh ^ ss);
    return (two_mask != 0) 
      ? StdRules_HandType_TWOPAIR 
      : StdRules_HandType_TRIPS;
    break;

  default:
    four_mask  = (sc & sd) & (sh & ss);
    if (four_mask) 
      return StdRules_HandType_QUADS;
    three_mask = (( sc&sd )|( sh&ss )) & (( sc&sh )|( sd&ss ));
    if (three_mask) 
      return StdRules_HandType_FULLHOUSE;
    else if (is_st_or_fl)
      return is_st_or_fl;
    else 
      return StdRules_HandType_TWOPAIR;

    break;
  };

}

#endif
