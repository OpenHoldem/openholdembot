/*
 * Copyright (C) 1999-2006 
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
#ifndef __EVX_DEFS__
#define __EVX_DEFS__

#include "pokereval_export.h"

typedef uint32 EvxHandVal;

#define EvxHandVal_TYPE_SHIFT     (2 * StdDeck_Rank_COUNT)
#define EvxHandVal_SIGCARDS_SHIFT StdDeck_Rank_COUNT
#define EvxHandVal_KICKERS_SHIFT  0
#define EvxHandVal_RANK_MASK      (0x1FFF)

#define EvxHandVal_HANDTYPE_VALUE(ht) ((ht) << EvxHandVal_TYPE_SHIFT)
#define EvxHandVal_HANDTYPE(ehv) ((ehv) >> EvxHandVal_TYPE_SHIFT)

enum {
  EvxHandVal_NOPAIR    = EvxHandVal_HANDTYPE_VALUE(StdRules_HandType_NOPAIR), 
  EvxHandVal_ONEPAIR   = EvxHandVal_HANDTYPE_VALUE(StdRules_HandType_ONEPAIR), 
  EvxHandVal_TWOPAIR   = EvxHandVal_HANDTYPE_VALUE(StdRules_HandType_TWOPAIR), 
  EvxHandVal_TRIPS     = EvxHandVal_HANDTYPE_VALUE(StdRules_HandType_TRIPS), 
  EvxHandVal_STRAIGHT  = EvxHandVal_HANDTYPE_VALUE(StdRules_HandType_STRAIGHT), 
  EvxHandVal_FLUSH     = EvxHandVal_HANDTYPE_VALUE(StdRules_HandType_FLUSH), 
  EvxHandVal_FULLHOUSE = EvxHandVal_HANDTYPE_VALUE(StdRules_HandType_FULLHOUSE), 
  EvxHandVal_QUADS     = EvxHandVal_HANDTYPE_VALUE(StdRules_HandType_QUADS), 
  EvxHandVal_STFLUSH   = EvxHandVal_HANDTYPE_VALUE(StdRules_HandType_STFLUSH),
};

extern POKEREVAL_EXPORT uint32 evxPairValueTable[StdDeck_N_RANKMASKS];
extern POKEREVAL_EXPORT uint32 evxTripsValueTable[StdDeck_N_RANKMASKS];
extern POKEREVAL_EXPORT uint32 evxStrValueTable[StdDeck_N_RANKMASKS];
extern POKEREVAL_EXPORT uint32 evxFlushCardsTable[StdDeck_N_RANKMASKS];

extern POKEREVAL_EXPORT HandVal EvxHandVal_toHandVal(EvxHandVal ehv);

#endif
