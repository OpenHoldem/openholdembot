/*
 *  eval.h: a fast poker hand evaluator
 *
 *  Copyright (C) 1993-99 Brian Goetz, Keith Miyake, Clifford T. Matthews
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

#ifndef __EVAL_H__
#define __EVAL_H__

#include <assert.h>

/*
 * When run over seven cards, here are the distribution of hands:
 *        high hand: 23294460
 *             pair: 58627800
 *         two pair: 31433400
 *  three of a kind: 6461620
 *         straight: 6180020
 *            flush: 4047644
 *       full house: 3473184
 *   four of a kind: 224848
 *   straight flush: 41584
 *
 */

#define SC sc
#define SD sd
#define SH sh
#define SS ss

/*
 * is_straight used to check for a straight by masking the ranks with four
 * copies of itself, each shifted one bit with respect to the
 * previous one.  So any sequence of five adjacent bits will still
 * be non-zero, but any gap will result in a zero value.  There's
 * a nice side-effect of leaving the top most bit set so we can use
 * it to set top_card.
 * Now we use a precomputed lookup table.  
 *
 */

#if 0
/* Keith's is-straight, which is still pretty good and uses one less table. */
    if ( (ranks2  = ranks & (ranks << 1)) &&
	 (ranks2 &=         (ranks << 2)) &&
	 (ranks2 &=         (ranks << 3)) &&
	 (ranks2 &=         (ranks << 4)) ) {
        retval.eval_t.hand     = StdRules_HandType_STRAIGHT;
        retval.eval_t.top_card = topCardTable[ranks2];
    } else if ((ranks & StdDeck_FIVE_STRAIGHT) ==  StdDeck_FIVE_STRAIGHT) {
        retval.eval_t.hand     = StdRules_HandType_STRAIGHT;
        retval.eval_t.top_card = StdDeck_Ranks_5;
    }
#endif


static inline HandVal 
StdDeck_StdRules_EVAL_N( StdDeck_CardMask cards, int n_cards )
{
  HandVal retval;
  uint32 ranks, four_mask, three_mask, two_mask, 
    n_dups, n_ranks;
  uint32 sc, sd, sh, ss;

  ss = StdDeck_CardMask_SPADES(cards);
  sc = StdDeck_CardMask_CLUBS(cards);
  sd = StdDeck_CardMask_DIAMONDS(cards);
  sh = StdDeck_CardMask_HEARTS(cards);

  retval = 0;
  ranks = SC | SD | SH | SS;
  n_ranks = nBitsTable[ranks];
  n_dups = n_cards - n_ranks;

  /* Check for straight, flush, or straight flush, and return if we can
     determine immediately that this is the best possible hand 
  */
  if (n_ranks >= 5) {
    if (nBitsTable[SS] >= 5) {
      if (straightTable[SS]) 
        return HandVal_HANDTYPE_VALUE(StdRules_HandType_STFLUSH)
          + HandVal_TOP_CARD_VALUE(straightTable[SS]);
      else
        retval = HandVal_HANDTYPE_VALUE(StdRules_HandType_FLUSH) 
          + topFiveCardsTable[SS];
    } 
    else if (nBitsTable[SC] >= 5) {
      if (straightTable[SC]) 
        return HandVal_HANDTYPE_VALUE(StdRules_HandType_STFLUSH)
          + HandVal_TOP_CARD_VALUE(straightTable[SC]);
      else 
        retval = HandVal_HANDTYPE_VALUE(StdRules_HandType_FLUSH) 
          + topFiveCardsTable[SC];
    } 
    else if (nBitsTable[SD] >= 5) {
      if (straightTable[SD]) 
        return HandVal_HANDTYPE_VALUE(StdRules_HandType_STFLUSH)
          + HandVal_TOP_CARD_VALUE(straightTable[SD]);
      else 
        retval = HandVal_HANDTYPE_VALUE(StdRules_HandType_FLUSH) 
          + topFiveCardsTable[SD];
    } 
    else if (nBitsTable[SH] >= 5) {
      if (straightTable[SH]) 
        return HandVal_HANDTYPE_VALUE(StdRules_HandType_STFLUSH)
          + HandVal_TOP_CARD_VALUE(straightTable[SH]);
      else 
        retval = HandVal_HANDTYPE_VALUE(StdRules_HandType_FLUSH) 
          + topFiveCardsTable[SH];
    } 
    else {
      int st;
      st = straightTable[ranks];
      if (st) 
        retval = HandVal_HANDTYPE_VALUE(StdRules_HandType_STRAIGHT)
			+ HandVal_TOP_CARD_VALUE(st);
    };

    /* Another win -- if there can't be a FH/Quads (n_dups < 3), 
       which is true most of the time when there is a made hand, then if we've
       found a five card hand, just return.  This skips the whole process of
       computing two_mask/three_mask/etc.
    */
    if (retval && n_dups < 3)
      return retval;
  };

  /*
   * By the time we're here, either: 
     1) there's no five-card hand possible (flush or straight), or
     2) there's a flush or straight, but we know that there are enough
        duplicates to make a full house / quads possible.  
   */
  switch (n_dups)
    {
    case 0:
      /* It's a no-pair hand */
      return HandVal_HANDTYPE_VALUE(StdRules_HandType_NOPAIR)
        + topFiveCardsTable[ranks];
      break;
      
    case 1: {
      /* It's a one-pair hand */
      uint32 t, kickers;

      two_mask   = ranks ^ (SC ^ SD ^ SH ^ SS);

      retval = HandVal_HANDTYPE_VALUE(StdRules_HandType_ONEPAIR)
        + HandVal_TOP_CARD_VALUE(topCardTable[two_mask]);
      t = ranks ^ two_mask;      /* Only one bit set in two_mask */
      /* Get the top five cards in what is left, drop all but the top three 
       * cards, and shift them by one to get the three desired kickers */
      kickers = (topFiveCardsTable[t] >> HandVal_CARD_WIDTH)
        & ~HandVal_FIFTH_CARD_MASK;
      retval += kickers;

      return retval;
    }
    break;
      
    case 2: 
      /* Either two pair or trips */

      two_mask   = ranks ^ (SC ^ SD ^ SH ^ SS);
      if (two_mask) { 
        uint32 t;

        t = ranks ^ two_mask; /* Exactly two bits set in two_mask */
        retval = HandVal_HANDTYPE_VALUE(StdRules_HandType_TWOPAIR)
          + (topFiveCardsTable[two_mask]
             & (HandVal_TOP_CARD_MASK | HandVal_SECOND_CARD_MASK))
          + HandVal_THIRD_CARD_VALUE(topCardTable[t]);

        return retval;
      }
      else {
        int t, second;
        
        three_mask = (( SC&SD )|( SH&SS )) & (( SC&SH )|( SD&SS ));
        
        retval = HandVal_HANDTYPE_VALUE(StdRules_HandType_TRIPS)
          + HandVal_TOP_CARD_VALUE(topCardTable[three_mask]);

        t = ranks ^ three_mask; /* Only one bit set in three_mask */
        second = topCardTable[t];
        retval += HandVal_SECOND_CARD_VALUE(second);
        t ^= (1 << second);
        retval += HandVal_THIRD_CARD_VALUE(topCardTable[t]);
        return retval;
      }
      break;
      
    default:
      /* Possible quads, fullhouse, straight or flush, or two pair */
      four_mask  = SH & SD & SC & SS;
      if (four_mask) {
        int tc;

        tc = topCardTable[four_mask];
        retval = HandVal_HANDTYPE_VALUE(StdRules_HandType_QUADS)
          + HandVal_TOP_CARD_VALUE(tc)
          + HandVal_SECOND_CARD_VALUE(topCardTable[ranks ^ (1 << tc)]);
        return retval;
      };

      /* Technically, three_mask as defined below is really the set of
         bits which are set in three or four of the suits, but since
         we've already eliminated quads, this is OK */
      /* Similarly, two_mask is really two_or_four_mask, but since we've
         already eliminated quads, we can use this shortcut */

      two_mask   = ranks ^ (SC ^ SD ^ SH ^ SS);
      if (nBitsTable[two_mask] != n_dups) {
        /* Must be some trips then, which really means there is a 
           full house since n_dups >= 3 */
        int tc, t;

        three_mask = (( SC&SD )|( SH&SS )) & (( SC&SH )|( SD&SS ));
        retval  = HandVal_HANDTYPE_VALUE(StdRules_HandType_FULLHOUSE);
        tc = topCardTable[three_mask];
        retval += HandVal_TOP_CARD_VALUE(tc);
        t = (two_mask | three_mask) ^ (1 << tc);
        retval += HandVal_SECOND_CARD_VALUE(topCardTable[t]);
        return retval;
      };

      if (retval) /* flush and straight */
        return retval;
      else {
        /* Must be two pair */
        int top, second;
          
        retval = HandVal_HANDTYPE_VALUE(StdRules_HandType_TWOPAIR);
        top = topCardTable[two_mask];
        retval += HandVal_TOP_CARD_VALUE(top);
        second = topCardTable[two_mask ^ (1 << top)];
        retval += HandVal_SECOND_CARD_VALUE(second);
        retval += HandVal_THIRD_CARD_VALUE(topCardTable[ranks ^ (1 << top) 
                                                        ^ (1 << second)]);
        return retval;
      };

      break;
    };
}

#undef SC
#undef SH
#undef SD
#undef SS

#endif

