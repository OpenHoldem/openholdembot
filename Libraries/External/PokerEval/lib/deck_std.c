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
#include <ctype.h>
#include "poker_defs.h"



char StdDeck_rankChars[] = "23456789TJQKA";
char StdDeck_suitChars[] = "hdcs";

int tototo[8];


int 
StdDeck_cardToString(int cardIndex, char *outString) {
  *outString++ = StdDeck_rankChars[StdDeck_RANK(cardIndex)];
  *outString++ = StdDeck_suitChars[StdDeck_SUIT(cardIndex)];
  *outString   = '\0';

  return 2;
}


int 
StdDeck_stringToCard(char *inString, int *cardIndex) {
  char *p;
  int rank, suit;

  p = inString;
  for (rank=StdDeck_Rank_FIRST; rank <= StdDeck_Rank_LAST; rank++) 
    if (StdDeck_rankChars[rank] == toupper(*p))
      break;
  if (rank > StdDeck_Rank_LAST)
    goto noMatch;
  ++p;
  for (suit=StdDeck_Suit_FIRST; suit <= StdDeck_Suit_LAST; suit++) 
    if (StdDeck_suitChars[suit] == tolower(*p))
      break;
  if (suit > StdDeck_Suit_LAST)
    goto noMatch;
  *cardIndex = StdDeck_MAKE_CARD(rank, suit);
  return 2;

 noMatch:
  /* Didn't match anything, return failure */
  return 0;
}


int
StdDeck_maskToCards(void *cardMask, int cards[]) {
  int i, n=0;
  StdDeck_CardMask c = *((StdDeck_CardMask *) cardMask);

  for (i=StdDeck_N_CARDS-1; i >= 0; i--) 
    if (StdDeck_CardMask_CARD_IS_SET(c, i)) 
      cards[n++] = i;

  return n;
}


#if 0
int 
StdDeck_stringToMask(char *inString, StdDeck_CardMask *outMask) {
  char *p;
  int n=0, rank, suit, card;

  StdDeck_CardMask_RESET(*outMask);
  for (p=inString; p < inString + strlen(inString); p++) {
    if (*p == ' ')
      continue;
    for (rank=StdDeck_Rank_FIRST; rank <= StdDeck_Rank_LAST; rank++) 
      if (StdDeck_rankChars[rank] == toupper(*p))
        break;
    if (rank > StdDeck_Rank_LAST)
      break;
    ++p;
    for (suit=StdDeck_Suit_FIRST; suit <= StdDeck_Suit_LAST; suit++) 
      if (StdDeck_suitChars[suit] == tolower(*p))
        break;
    if (suit > StdDeck_Suit_LAST)
      break;
    ++p;
    card = StdDeck_MAKE_CARD(rank, suit);
    StdDeck_CardMask_SET(*outMask, card);
    ++n;
  }
    
  return n;
}
#endif

int
StdDeck_NumCards(void *cardMask) {
  StdDeck_CardMask c = *((StdDeck_CardMask *) cardMask);
  int i;
  int ncards = 0;
  for (i=0; i<StdDeck_N_CARDS; i++)
    if (StdDeck_CardMask_CARD_IS_SET(c, i))
      ncards++;
  return ncards;
}

Deck StdDeck = { 
  StdDeck_N_CARDS, 
  "StandardDeck", 
  StdDeck_cardToString, 
  StdDeck_stringToCard,
  StdDeck_maskToCards,
  StdDeck_NumCards
};

