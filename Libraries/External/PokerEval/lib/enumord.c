/* $Id: enumord.c 1024 2006-04-22 20:49:41Z loic $ */
/*
  See comments in enumord.h.
  Michael Maurer, Jun 2002

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

#include <stdlib.h>
#include "enumord.h"

int enum_nbits[ENUM_ORDERING_MAXPLAYERS+1] = {0, 1, 2, 2, 3, 3, 3, 3};

typedef struct {
  int index;
  int handval;
} enum_rankelem_t;

static int
enum_rankelem_compare(const void *p1, const void *p2) {
  enum_rankelem_t *e1 = (enum_rankelem_t *) p1;
  enum_rankelem_t *e2 = (enum_rankelem_t *) p2;
  if (e1->handval < e2->handval)
    return -1;
  else if (e1->handval == e2->handval)
    return 0;
  else
    return 1;
}

void
enum_ordering_rank(HandVal *hands, int noqual, int nplayers, int *ranks,
                   int reverse)
{
  enum_rankelem_t elems[ENUM_ORDERING_MAXPLAYERS];
  int i;
  int currank, lastval;
  if (reverse)
    noqual = -noqual;
  for (i=0; i<nplayers; i++) {
    elems[i].index = i;
    elems[i].handval = reverse ? -(int)(hands[i]) : hands[i];
  }
  /* TODO: we may find that the large number of function calls that qsort()
     makes to enum_rankelem_compare(), even for small arrays, is too slow
     to tolerate.  After all, this function is called in the inner loop of
     a hand outcome enumeration.  Let's cross that bridge if we come to it. */
  qsort(elems, nplayers, sizeof(enum_rankelem_t), enum_rankelem_compare);
  currank = -1;
  lastval = -1;
  for (i=nplayers-1; i>=0; i--) {
    if (elems[i].handval != lastval) {
      currank = nplayers - i - 1;
      lastval = elems[i].handval;
    }
    if (elems[i].handval == noqual)
      currank = nplayers;
    ranks[elems[i].index] = currank;
  }
}

