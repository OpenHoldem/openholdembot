/*
 * Copyright (C) 2002-2006 
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

/* Public declarations for combinations.c */
/* Michael Maurer, Jun 2002 */

#ifndef COMBINATIONS_H
#define COMBINATIONS_H

#include "pokereval_export.h"

typedef void *Combinations;

extern POKEREVAL_EXPORT void free_combinations(Combinations c);
extern POKEREVAL_EXPORT Combinations init_combinations(int nuniv, int nelem);
extern POKEREVAL_EXPORT int num_combinations(Combinations c);
extern POKEREVAL_EXPORT void get_combination(Combinations c, int cnum, int *elems);

#endif
