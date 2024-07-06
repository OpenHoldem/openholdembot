/*
 *  Copyright 2006 Michael Maurer <mjmaurer@yahoo.com>, 
 *                 Brian Goetz <brian@quiotix.com>, 
 *                 Loic Dachary <loic@dachary.org>, 
 *                 Tim Showalter <tjs@psaux.com>
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
#include <stdio.h>
#include "poker_defs.h"


int
GenericDeck_maskToString(Deck *deck, void *cardMask, char *outString) {
  int cards[50], n, i;
  char *p;

  n = (*deck->maskToCards)(cardMask, cards);
  
  p = outString;
  for (i=0; i<n; i++) {
    if (i > 0) 
      *p++ = ' ';
    p += (*deck->cardToString)(cards[i], p);
  };
  *p = '\0';
  return (outString - p);
}


int 
GenericDeck_printMask(Deck *deck, void *cardMask) {
  char outString[150];
  int r;

  r = GenericDeck_maskToString(deck, cardMask, outString);
  printf("%s", outString);
  return r;
}


char *
GenericDeck_maskString(Deck *deck, void *cardMask) {
  static thread char outString[150];

  GenericDeck_maskToString(deck, cardMask, outString);
  return outString;
}


int
GenericDeck_numCards(Deck *deck, void *cardMask) {
  return (*deck->numCards)(cardMask);
}


char *
GenericDeck_cardString(Deck *deck, int cardIndex) {
  static thread char outString[16];

  (*deck->cardToString)(cardIndex, outString);
  return outString;
}


int
GenericDeck_printCard(Deck *deck, int cardIndex) {
  char outString[16];
  int ret;

  ret = (*deck->cardToString)(cardIndex, outString);
  if (ret) 
    printf("%s", outString);
  return ret;
}


