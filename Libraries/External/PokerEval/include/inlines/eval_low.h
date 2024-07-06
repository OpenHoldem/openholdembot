/*
 * Copyright (C) 1999-2002
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
/* TODO -- review and fix code for hands with >5 cards, in particular
 hands like KK77442, KKK7733, 777KK33, 333KK77, 4444KKK. */

#ifndef __EVAL_LOW_H__
#define __EVAL_LOW_H__

#include <assert.h>
#include "handval_low.h"

/*
 * Lowball evaluator.  Assumes that n_cards >= 5
 */

static inline uint32
_bottomNCards(uint32 cards, int howMany) {
  int i;
  uint32 retval, t;

  retval = 0;
  for (i=0; i<howMany; i++) {
    t = bottomCardTable[cards];
    retval += t << (i*LowHandVal_CARD_WIDTH);
    cards ^= (1 << t);
  }

  return retval;
}


static inline LowHandVal 
StdDeck_Lowball_EVAL(StdDeck_CardMask cards, int n_cards) {
  uint32 ranks, dups, trips, ss, sh, sd, sc, t, tt;

  ss = StdDeck_CardMask_SPADES(cards);
  sc = StdDeck_CardMask_CLUBS(cards);
  sd = StdDeck_CardMask_DIAMONDS(cards);
  sh = StdDeck_CardMask_HEARTS(cards);

  ss = Lowball_ROTATE_RANKS(ss);
  sc = Lowball_ROTATE_RANKS(sc);
  sd = Lowball_ROTATE_RANKS(sd);
  sh = Lowball_ROTATE_RANKS(sh);

  ranks = sc | ss | sd | sh;
  if (nBitsTable[ranks] >= 5) 
    return LowHandVal_HANDTYPE_VALUE(StdRules_HandType_NOPAIR) 
      + bottomFiveCardsTable[ranks];
  else {
    dups = (sc&sd) | (sh & (sc|sd)) | (ss & (sh|sc|sd));
    t = bottomCardTable[dups];

    switch (nBitsTable[ranks]) {
    case 4:
      return LowHandVal_HANDTYPE_VALUE(StdRules_HandType_ONEPAIR) 
        + LowHandVal_TOP_CARD_VALUE(t)
        + (_bottomNCards(ranks ^ (1 << t), 3) << LowHandVal_CARD_WIDTH);
      break;

    case 3:
      if (nBitsTable[dups] == 2) {
        /* TODO: does this assume 5-card hand?  e.g., not quads plus pair */
        tt = bottomCardTable[dups ^ (1 << t)];
        return LowHandVal_HANDTYPE_VALUE(StdRules_HandType_TWOPAIR)
          + LowHandVal_TOP_CARD_VALUE(tt) 
          + LowHandVal_SECOND_CARD_VALUE(t) 
          + ((_bottomNCards(ranks ^ (1 << t) ^ (1 << tt), 1)) 
             << (2*LowHandVal_CARD_WIDTH));
      }
      else {
        /* TODO: does this assume 5-card hand?  e.g., not full house plus pair */
        t = bottomCardTable[dups];
        return LowHandVal_HANDTYPE_VALUE(StdRules_HandType_TRIPS) 
          + LowHandVal_TOP_CARD_VALUE(t)
          + (_bottomNCards(ranks ^ (1 << t), 2) 
             << (2*LowHandVal_CARD_WIDTH));
      }
      break;

    case 2:
      if (nBitsTable[dups] == 2) {
        /* TODO: does this assume 5-card hand?  e.g., not two trips */
        trips = dups & (sc ^ ss ^ sd ^ sh);
        t = bottomCardTable[trips];
        return LowHandVal_HANDTYPE_VALUE(StdRules_HandType_FULLHOUSE)
          + LowHandVal_TOP_CARD_VALUE(t) 
          + LowHandVal_SECOND_CARD_VALUE(bottomCardTable[ranks ^ (1 << t)]);
      }
      else {
        return LowHandVal_HANDTYPE_VALUE(StdRules_HandType_QUADS)
          + LowHandVal_TOP_CARD_VALUE(t) 
          + LowHandVal_SECOND_CARD_VALUE(bottomCardTable[ranks ^ (1 << t)]);
      };
      break;
    };
  };

  assert(!"Logic error in eval_low");
  return 0;
}

#endif
