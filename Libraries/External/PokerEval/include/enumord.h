/*
 * Copyright (C) 2004-2006
 *           Michael Maurer <mjmaurer@yahoo.com>
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
/* $Id: enumord.h 1773 2006-10-12 11:34:41Z loic $ */
/*
  Definitions for computing a histogram of final hand orderings.

  A final hand ordering is defined by the relative rank of each player's hand
  after all cards are dealt.  For example, in a high-only game with 3 players,
  the possible final hand orderings for players A, B, and C are (where
  comma denotes tie):
        A B C
        A C B
        B A C
        B C A
        C A B
        C B A
        A B,C
        B A,C
        C A,B
        A,B C
        A,C B
        B,C A
        A,B,C
  Another way to reprsent this is to assign a player to a column and write
  the relative hand rank (1=best) in that column.  Then the possibilities
  above are written (in the same order as above) as
        A B C
        -----
        1 2 3
        1 3 2
        2 1 3
        2 3 1
        3 1 2
        3 2 1
        1 2 2
        2 1 2
        2 2 1
        1 1 2
        1 2 1
        2 1 1
        1 1 1
  Our encoding here is based on the second form.

  For high/low games, we track the full combination of outcomes.  In a
  2-person game with a qualifier for low, the possible outcomes are:
         HI         LO
        A  B       A  B
        1  1       1  1		tie for high; tie for low
        1  1       1  2		tie for high; A wins low
        1  1       1 NQ		tie for high; A wins low, B doesn't qualify low
        1  1       2  1		tie for high; B wins low
        1  1      NQ  1		etc.
        1  1      NQ NQ
        1  2       1  1
        1  2       1  2
        1  2       1 NQ
        1  2       2  1
        1  2      NQ  1
        1  2      NQ NQ
        2  1       1  1
        2  1       1  2
        2  1       1 NQ
        2  1       2  1
        2  1      NQ  1
        2  1      NQ NQ

  For ease of programming, we are somewhat wasteful of memory when storing the
  histogram of outcomes.  We allocate an array such that hist[i] is the number
  of times that outcome i occurs.  The integer i encodes the ordering of N
  players' hand ranks by using N bit fields, each bit field just large enough
  to represent the numbers [0..N].  The value 0 in bit field K indicates that
  player K has the strongest hand (possibly tying another player); the value
  N-1 indicates player K has the weakest hand; the value N indicates a
  non-qualifying hand (such as a 9-low in 8-or-better).  Note that not all
  integers i correspond to valid rank orderings, both due the fact that for
  some values of N the bit field can encode values greater than N, and due to
  the fact that some rank orderings, like a 3-way tie for 3rd place, are
  impossible.  That is the wasteful part; a hash table would solve it.

  For high-low games, we use two such sets of bit fields, packed adjacent to
  one another in a single integer.  The high-order bit fields correspond to
  the high hand; the low-order bit fields correspond to the low hand.

  Michael Maurer, Jun 2002
*/

#ifndef ENUMORD_H
#define ENUMORD_H

#include "poker_defs.h"

/* largest integer N such that N * ENUM_ORDERING_NBITS(N) < 32 */
#define ENUM_ORDERING_MAXPLAYERS 7

/* largest integer N such that 2 * N * ENUM_ORDERING_NBITS(N) < 32 */
#define ENUM_ORDERING_MAXPLAYERS_HILO 5

typedef enum {
  enum_ordering_mode_none = 0,
  enum_ordering_mode_hi,
  enum_ordering_mode_lo,
  enum_ordering_mode_hilo
} enum_ordering_mode_t;

typedef struct {
  enum_ordering_mode_t mode;
  unsigned int nplayers;
  unsigned int nentries; /* equal to ENUM_ORDERING_NENTRIES(nplayers)
                            or ENUM_ORDERING_NENTRIES_HILO(nplayers),
                            depending on mode */
  /* hist[i] is the number of outcomes in which the ordering of players'
     relative hand values corresponds to ordering i.  We encode each ordering
     as an integer i such that ENUM_ORDERING_DECODE_PLAYER_K(i, n, k) gives
     the relative hand rank of player k out of n players.  Rank is encoded as
     an integer in [0,n] where 0=strongest, n-1=weakest, n=non-qualifying. */
  unsigned int *hist;	/* has nenetries elements */
} enum_ordering_t;

extern POKEREVAL_EXPORT int enum_nbits[ENUM_ORDERING_MAXPLAYERS+1];
extern POKEREVAL_EXPORT void enum_ordering_rank(HandVal *hands, int noqual,
                               int nplayers, int *ranks, int reverse);

/* the bit field size for one player's relative hand rank */
#define ENUM_ORDERING_NBITS(nplayers) \
  ((nplayers < 0 || nplayers >= sizeof(enum_nbits)/sizeof(enum_nbits[0])) \
   ? -1 : enum_nbits[nplayers])

/* the number of elements in the hist[] array */
#define ENUM_ORDERING_NENTRIES(nplayers) \
  (((nplayers > ENUM_ORDERING_MAXPLAYERS) || \
    ENUM_ORDERING_NBITS(nplayers) < 0) \
   ? -1 : (1 << (nplayers * ENUM_ORDERING_NBITS(nplayers))))

/* the number of elements in the hist[] array in high/low games */
#define ENUM_ORDERING_NENTRIES_HILO(nplayers) \
  (((nplayers > ENUM_ORDERING_MAXPLAYERS_HILO) || \
    ENUM_ORDERING_NBITS(nplayers) < 0) \
   ? -1 : (1 << (2 * nplayers * ENUM_ORDERING_NBITS(nplayers))))

/* Compute the integer encoding of a given array of relative hand rankings.
   ranks[k] is relative hand rank of player k's hand; rank=0 is best,
   rank=nplayers-1 is worst, rank=nplayers is non-qualifying.  Caller must
   ensure that 0 <= ranks[k] <= nplayers.  */
#define ENUM_ORDERING_ENCODE(encoding, nplayers, ranks) \
do { \
  int _k; \
  int _nbits = ENUM_ORDERING_NBITS(nplayers); \
  (encoding) = 0; \
  for (_k=0; _k<(nplayers); _k++) \
    (encoding) = ((encoding) << _nbits) | ((ranks)[_k]); \
} while (0)
  
/* Compute the integer encoding of a given array of relative hand rankings
   for high/low games. */
#define ENUM_ORDERING_ENCODE_HILO(encoding, nplayers, hiranks, loranks) \
do { \
  int _k; \
  int _nbits = ENUM_ORDERING_NBITS(nplayers); \
  (encoding) = 0; \
  for (_k=0; _k<(nplayers); _k++) \
    (encoding) = ((encoding) << _nbits) | ((hiranks)[_k]); \
  for (_k=0; _k<(nplayers); _k++) \
    (encoding) = ((encoding) << _nbits) | ((loranks)[_k]); \
} while (0)
  
/* the number of bits to the start of the bit field for player k */
#define ENUM_ORDERING_SHIFT_K(nplayers, k) \
   (((nplayers) - (k) - 1) * ENUM_ORDERING_NBITS(nplayers))

/* the number of bits to the start of the bit field for player k's high
 hand, in a high/low game */
#define ENUM_ORDERING_SHIFT_HILO_K_HI(nplayers, k) \
   ((2*(nplayers) - (k) - 1) * ENUM_ORDERING_NBITS(nplayers))

/* the number of bits to the start of the bit field for player k's low
 hand, in a high/low game */
#define ENUM_ORDERING_SHIFT_HILO_K_LO(nplayers, k) \
   (((nplayers) - (k) - 1) * ENUM_ORDERING_NBITS(nplayers))

/* a bit mask covering the bit field for player k */
#define ENUM_ORDERING_MASK_K(nplayers, k) \
  ((~(~0 << ENUM_ORDERING_NBITS(nplayers))) << \
   ENUM_ORDERING_SHIFT_K((nplayers), (k)))

/* a bit mask covering the bit field for player k's high hand, in a high/low
   game */
#define ENUM_ORDERING_MASK_HILO_K_HI(nplayers, k) \
  ((~(~0 << ENUM_ORDERING_NBITS(nplayers))) << \
   ENUM_ORDERING_SHIFT_HILO_K_HI((nplayers), (k)))

/* a bit mask covering the bit field for player k's low hand, in a high/low
   game */
#define ENUM_ORDERING_MASK_HILO_K_LO(nplayers, k) \
  ((~(~0 << ENUM_ORDERING_NBITS(nplayers))) << \
   ENUM_ORDERING_SHIFT_HILO_K_LO((nplayers), (k)))

/* decodes the integer encoding to yield the relative rank of
   player k's hand */
#define ENUM_ORDERING_DECODE_K(encoding, nplayers, k) \
  (((encoding) & ENUM_ORDERING_MASK_K((nplayers), (k))) >> \
   ENUM_ORDERING_SHIFT_K((nplayers), (k)))
   
/* decodes the integer encoding to yield the relative rank of
   player k's high hand in a high/low game */
#define ENUM_ORDERING_DECODE_HILO_K_HI(encoding, nplayers, k) \
  (((encoding) & ENUM_ORDERING_MASK_HILO_K_HI((nplayers), (k))) >> \
   ENUM_ORDERING_SHIFT_HILO_K_HI((nplayers), (k)))
   
/* decodes the integer encoding to yield the relative rank of
   player k's low hand in a high/low game */
#define ENUM_ORDERING_DECODE_HILO_K_LO(encoding, nplayers, k) \
  (((encoding) & ENUM_ORDERING_MASK_HILO_K_LO((nplayers), (k))) >> \
   ENUM_ORDERING_SHIFT_HILO_K_LO((nplayers), (k)))
   
/* given hands[], assigns ranks[k] such that a rank of 0 indicates that
   hands[k] is the highest hand value in hands[], a rank of 1 indicates
   that hands[k] is the next highest value, etc., and a rank of nplayers
   indicates that hands[k] is equal to noqual (the not-qualifying hand) */
#define ENUM_ORDERING_RANK_HI(hands, noqual, nplayers, ranks) \
  enum_ordering_rank((hands), (noqual), (nplayers), (ranks), 0)

/* given hands[], assigns ranks[k] such that a rank of 0 indicates that
   hands[k] is the lowest hand value in hands[], a rank of 1 indicates
   that hands[k] is the next lowest value, etc., and a rank of nplayers
   indicates that hands[k] is equal to noqual (the not-qualifying hand) */
#define ENUM_ORDERING_RANK_LO(hands, noqual, nplayers, ranks) \
  enum_ordering_rank((hands), (noqual), (nplayers), (ranks), 1)

/* increments the histogram bin value corresponding to the ranks[] array */
#define ENUM_ORDERING_INCREMENT(ordering, nplayers, ranks) \
do { \
  int _encoding; \
  ENUM_ORDERING_ENCODE(_encoding, (nplayers), (ranks)); \
  (ordering)->hist[_encoding]++; \
} while (0)

/* increments the histogram bin value corresponding to the ranks[] array,
 for high/low games*/
#define ENUM_ORDERING_INCREMENT_HILO(ordering, nplayers, hiranks, loranks) \
do { \
  int _encoding; \
  ENUM_ORDERING_ENCODE_HILO(_encoding, (nplayers), (hiranks), (loranks)); \
  (ordering)->hist[_encoding]++; \
} while (0)

#endif
