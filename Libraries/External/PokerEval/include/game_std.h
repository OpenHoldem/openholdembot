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
#ifndef GAME_STD_H
#define GAME_STD_H

#define DECK_STANDARD
#define RULES_STANDARD

#include "deck_std.h"
#include "rules_std.h"

#undef Hand_EVAL_N
#undef Hand_EVAL_LOW
#undef Hand_EVAL_LOW8

#define Hand_EVAL_N     StdDeck_StdRules_EVAL_N
#define Hand_EVAL_TYPE  StdDeck_StdRules_EVAL_TYPE
#define Hand_EVAL_X5    StdDeck_StdRules_EVAL_X5
#define Hand_EVAL_X7    StdDeck_StdRules_EVAL_X7

#define Hand_EVAL_LOW   StdDeck_Lowball_EVAL
#define Hand_EVAL_LOW8  StdDeck_Lowball8_EVAL

#undef  DECK_STANDARD
#undef  RULES_STANDARD

#endif /* GAME_STD_H */
