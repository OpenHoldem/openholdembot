/* DO NOT protect this file from multiple inclusions */

/*
 *  tree.h: The actual decision tree that new_eval.h uses
 *
 *  Copyright (C) 1994  Clifford T. Matthews
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
 * This file is included in multiple places in our new fast hand evaluator.
 * Each time it is included, certain hand identification predicate macros
 * are defined from outside this file, and then others are defined from
 * within this file, and then, using all the predicates, a decision tree is
 * walked until we get to a terminal node, at which point we return a complete
 * hand identifying value (i.e. a 32 bit number whose most significant bits
 * can be used to identify which type of hand you have (e.g. full-house), and
 * whose remaining bits can tell you the composition of the hand (e.g. sevens
 * and fives).
 *
 * In the interests of speed, we actually have three different types of
 * predicates.  The simplest either returns 0 or non-zero based on whether
 * or not the current hand is of a given type, given all the information
 * that we know by the time the predicate is called.  NOTE:  it is generally
 * NOT good computer science to have predicates that are postion dependent
 * (e.g. you can't use the TWO_PAIR_P predicate just anywhere in the below
 * code -- it can only be used after we've determined that we don't have
 * three of a kind).  However, it is faster to do things this way, and once
 * you fully understand what we're doing below, you can see that our code is
 * correct.  Simple predicates have names of the form XXX_P, where XXX is
 * the hand class in capital letters.  Simple predicates are ALWAYS defined
 * outside of this file (i.e. all the information that they need is available
 * to us at compile time).
 *
 * This file contains four types of things:
 *
 *      Complete predicate macros definitions that define macros which detect
 *      particular classes of hands and either return 0 if such a hand isn't
 *      detected, or a complete hand identifying value.  Complete predicate
 *      macros have names of the form XXX_complete_P, where XXX is the hand
 *      class in capital letters.
 *
 *      Helper predicate macro definitions that define macros which detect
 *      particular classes of hands and either return 0 if such a hand isn't
 *      detected, or a useful number which will be used later as the
 *      evaluation proceeds.  Helper predicate macros have names of the form
 *      XXX_helper_P, where XXX is the hand class in capital letters.
 *
 *      Helper macro definitions that aren't predicates, but that provide
 *      information useful to the tree walk.  Helper macros have names of
 *      the form YYY_helper, where YYY helps describe the data that the
 *      particular helper macro returns.
 *
 *      The decision tree code that uses all three classes of predicates to
 *      walk to the identifying node and then return the proper hand
 *      identifying value.  At different places, where this file is included
 *      some of the predicates that we #define below will be overridden with
 *      an outside definition of 0, because at compile time we can tell that
 *      certain hands do not need to be checked for particular hand types.
 *
 * The decision tree walking code frequently makes use of inline-functions
 * which map some intermediate information into coplete hand identifying
 * values.  These functions are defined elsewhere and have names of the
 * form lower_case_hand_class_value.
 *      
 */

/*
 * FLUSH_helper_P returns 0 if no flush, or the top 5 ranks in the flush suit
 *      if a flush is found.  We don't yet or in EvxHandVal_FLUSH, since that is
 *      premature.  We return what we do because it is all the information
 *      that is needed to continue checking for other things (like a straight
 *      flush).
 */

#if     !defined(FLUSH_helper_P)

#define FLUSH_helper_P()                                          \
    (evxFlushCardsTable[c] | evxFlushCardsTable[d]                  \
    | evxFlushCardsTable[h] | evxFlushCardsTable[s])

#endif

/*
 * STRAIGHT_FLUSH_helper_P returns 0 or a complete hand rank, but with the
 *      VALUE set as a straight value.  A straight flush is sufficiently rare
 *      that when we actually find one, we can clean up the VALUE by xoring in
 *      xor of the correct VALUE (i.e. EvxHandVal_STFLUSH) with the
 *      incorrect value (i.e. EvxHandVal_STRAIGHT).  Since a ^ (a^b) is b, this
 *      does the right thing and we don't need a separate table, which might
 *      slow us down by interfering with the data cache.
 */

#if     !defined(STRAIGHT_FLUSH_helper_P)

#define STRAIGHT_FLUSH_helper_P(suit)                                   \
    evxStrValueTable[suit]

#endif

#undef FK_LOCALS
#if     !defined(FOUR_OF_A_KIND_complete_P)

#define FOUR_OF_A_KIND_complete_P()                                     \
(   tmpFkRetval = c & d & h & s,                                        \
    tmpFkRetval?                                                        \
	EvxHandVal_QUADS | (tmpFkRetval << StdDeck_Rank_COUNT) |        \
			  	    topBitTable[ranks ^ tmpFkRetval]    \
      : tmpFkRetval                                                     \
)

#define FK_LOCALS uint32 tmpFkRetval;
#else
#define FK_LOCALS
#endif

/*
 * THREE_OF_A_KIND returns all the ranks that have at least three distinct
 *      members.  It is not sufficient to just return the top one, because
 *      of the splenderiferous implementation of FULL_HOUSE below.
 */

#if     !defined(THREE_OF_A_KIND_helper_P)

#define THREE_OF_A_KIND_helper_P()                                      \
     ((( c&d )|( h&s )) & (( c&h )|( d&s )))

#endif

/*
 * Watch closely:  FULL_HOUSE_complete_P will only be examined after we know
 *      that we do not have four of a kind.  So if we xor all four
 *      suits together we are left with ones every place where we
 *      either have one or three members of a particular rank.  If
 *      we invert this and then and it with ranks, we now only have
 *      ones where we have exactly two members of a particular
 *      rank.  However, this is not enough, because it is possible
 *      for a full-house to consist of two three-of-a-kinds, so we
 *      have to or in three_info, which contains all of our
 *      three-of-a-kinds.  Then we need to mask off the top rank
 *      to see if we still have a pair or three-of-a-kind left
 *      over.
 */

#undef FH_LOCALS
#if     !defined(FULL_HOUSE_complete_P)

#define FULL_HOUSE_complete_P(three_info)                               \
(   tmpFhRetval = (~(c^d^h^s) & ranks)|three_info,                      \
    tmpFhTopbit = topBitTable[three_info],                              \
    tmpFhRetval ^= tmpFhTopbit,                                         \
    tmpFhRetval?                                                        \
	EvxHandVal_FULLHOUSE | (tmpFhTopbit << StdDeck_Rank_COUNT) |    \
					topBitTable[tmpFhRetval]        \
    : tmpFhRetval                                                       \
)

#define FH_LOCALS uint32 tmpFhRetval, tmpFhTopbit;
#else
#define FH_LOCALS

#endif

#define STRAIGHT_FLUSH_XOR_CORRECTION_VALUE     (EvxHandVal_STRAIGHT ^       \
						EvxHandVal_STFLUSH)

#define ALL_PAIRS_helper()      (h & (d|c|s)) | (d & (c|s)) | (c & s)

{
  FK_LOCALS
  FH_LOCALS

    flush_suit = FLUSH_helper_P();
    if (STRAIGHT_P()) {
	if (flush_suit) {
	    if ((retval = STRAIGHT_FLUSH_helper_P(flush_suit))) {
		/* straight flush */
		return retval ^ STRAIGHT_FLUSH_XOR_CORRECTION_VALUE;
	    } else {
		if ((retval = FOUR_OF_A_KIND_complete_P())) {
		    return retval;
		    /* four of a kind */
		} else {
		    if ((three_info = THREE_OF_A_KIND_helper_P()) &&
			       (retval = FULL_HOUSE_complete_P(three_info))) {
			/* full house */
			return retval;
		    } else {
			/* flush */
			return flush_value(flush_suit);
		    }
		}
	    }
	} else {
	    if ((retval = FOUR_OF_A_KIND_complete_P())) {
		return retval;
		/* four of a kind */
	    } else {
		if ((three_info = THREE_OF_A_KIND_helper_P()) &&
			       (retval = FULL_HOUSE_complete_P(three_info))) {
		    /* full house */
		    return retval;
		} else {
		    /* straight */
		    return straight_value(ranks);
		}
	    }
	}
    } else {
	if (AT_LEAST_PAIR_P()) {
	    if ((three_info = THREE_OF_A_KIND_helper_P())) {
		if ((retval = FOUR_OF_A_KIND_complete_P())) {
		    return retval;
		    /* four of a kind */
		} else {
		    if ((retval = FULL_HOUSE_complete_P(three_info))) {
			/* full house */
			return retval;
		    } else {
			if (flush_suit) {
			    /* flush */
			    return flush_value(flush_suit);
			} else {
			    /* three of a kind */
			    return trips_value(ranks, three_info);
			}
		    }
		}
	    } else {
		if (flush_suit) {
		    /* flush */
		    return flush_value(flush_suit);
		} else {
		    all_pairs = ALL_PAIRS_helper();
		    if (PAIR_P()) {
			/* pair */
			return pair_value(ranks, all_pairs);
		    } else {
			/* two pair */
			return two_pair_value(ranks, all_pairs);
		    }
		}
	    }
	} else {
	    if (flush_suit) {
		/* flush */
		return flush_value(flush_suit);
	    } else {
		/* high hand */
		return nopair_value(ranks);
	    }
	}
    }
 }
