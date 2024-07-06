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
#ifndef __HANDVAL_H__
#define __HANDVAL_H__

/* 
 * The HandVal type used to be defined as a union of a 32-bit integer with
 * a mess of bit fields for the hand type, top card, etc.  This had two major
 * problems: 
 * 1.  Unions seem to cripple the optimizing ability of even smart compilers;
 * 2.  C's support for bit fields suck, and it required us to know the 
 *     endian-ness of the system to define the union.
 *
 * We've replaced it with a 32-bit quantity, and defined symbolically the
 * various shifts and masks for the various fields.  The 32-bit value can
 * still be compared numerically against another hand value to see which wins;
 * the only difference is slightly more complexity in building the value.
 * Fortunately, this is most often hidden in the evaluator, so the driver
 * programs often don't need to know anything about it.
 */

typedef uint32 HandVal;

#define HandVal_HANDTYPE_SHIFT    24
#define HandVal_HANDTYPE_MASK     0x0F000000
#define HandVal_CARDS_SHIFT       0
#define HandVal_CARDS_MASK        0x000FFFFF
#define HandVal_TOP_CARD_SHIFT    16
#define HandVal_TOP_CARD_MASK     0x000F0000
#define HandVal_SECOND_CARD_SHIFT 12
#define HandVal_SECOND_CARD_MASK  0x0000F000
#define HandVal_THIRD_CARD_SHIFT  8
#define HandVal_THIRD_CARD_MASK   0x00000F00
#define HandVal_FOURTH_CARD_SHIFT 4
#define HandVal_FOURTH_CARD_MASK  0x000000F0
#define HandVal_FIFTH_CARD_SHIFT  0
#define HandVal_FIFTH_CARD_MASK   0x0000000F
#define HandVal_CARD_WIDTH        4
#define HandVal_CARD_MASK         0x0F

#define HandVal_HANDTYPE(hv)   ((hv) >> HandVal_HANDTYPE_SHIFT)
#define HandVal_CARDS(hv)      ((hv) & HandVal_CARDS_MASK)
#define HandVal_TOP_CARD(hv) \
  (((hv) >> HandVal_TOP_CARD_SHIFT) & HandVal_CARD_MASK)
#define HandVal_SECOND_CARD(hv) \
  (((hv) >> HandVal_SECOND_CARD_SHIFT) & HandVal_CARD_MASK)
#define HandVal_THIRD_CARD(hv) \
  (((hv) >> HandVal_THIRD_CARD_SHIFT) & HandVal_CARD_MASK)
#define HandVal_FOURTH_CARD(hv) \
  (((hv) >> HandVal_FOURTH_CARD_SHIFT) & HandVal_CARD_MASK)
#define HandVal_FIFTH_CARD(hv) \
  (((hv) >> HandVal_FIFTH_CARD_SHIFT) & HandVal_CARD_MASK)

#define HandVal_HANDTYPE_VALUE(ht)   ((ht) << HandVal_HANDTYPE_SHIFT)
#define HandVal_TOP_CARD_VALUE(c)    ((c) << HandVal_TOP_CARD_SHIFT)
#define HandVal_SECOND_CARD_VALUE(c) ((c) << HandVal_SECOND_CARD_SHIFT)
#define HandVal_THIRD_CARD_VALUE(c)  ((c) << HandVal_THIRD_CARD_SHIFT)
#define HandVal_FOURTH_CARD_VALUE(c) ((c) << HandVal_FOURTH_CARD_SHIFT)
#define HandVal_FIFTH_CARD_VALUE(c)  ((c) << HandVal_FIFTH_CARD_SHIFT)

#define HandVal_NOTHING 0

#endif
