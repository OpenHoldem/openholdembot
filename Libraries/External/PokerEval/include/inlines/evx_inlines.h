#ifndef	__EVX_INLINES_H__
#define	__EVX_INLINES_H__

/*
 *  evx_inlines.h: The inline functions that are used by evx5/evx7 
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

static inline uint32 flush_value(StdDeck_RankMask ranks)
{
    return EvxHandVal_FLUSH | topFiveBitsTable[ranks];
}

/*
 * EvxHandVal_STRAIGHT_table has EvxHandVal_STRAIGHT or'd in
 */

static inline uint32 straight_value(StdDeck_RankMask ranks)
{
    return evxStrValueTable[ranks];
}

static inline uint32 nopair_value(StdDeck_RankMask ranks)
{
    return EvxHandVal_NOPAIR | topFiveBitsTable[ranks];
}

/*
 * three_of_a_kind_value_table is EvxHandVal_TRIPS ored with the top
 * 2 cards in the subscript.
 */

static inline uint32 trips_value(StdDeck_RankMask ranks, 
                                 StdDeck_RankMask trips)
{
    return (trips << StdDeck_Rank_COUNT) | evxTripsValueTable[trips ^ ranks];
}

static inline uint32 two_pair_value(StdDeck_RankMask ranks, 
                                    StdDeck_RankMask all_pairs)
{
    uint32 retval;

    retval = topTwoBitsTable[all_pairs];
    return EvxHandVal_TWOPAIR | (retval << StdDeck_Rank_COUNT) | topBitTable[ranks^retval];
}

/*
 * pair_value_table has EvxHandVal_ONEPAIR or'd in.
 */

static inline uint32 pair_value(StdDeck_RankMask ranks, 
                                StdDeck_RankMask all_pairs)
{
    uint32 retval;

    retval = topBitTable[all_pairs];
    return (retval << StdDeck_Rank_COUNT) | evxPairValueTable[ranks^retval];
}

#define c clubs
#define d diamonds
#define h hearts
#define s spades

#endif
