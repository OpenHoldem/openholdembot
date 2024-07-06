/*
 * Copyright (C) 1999-2006 
 *           Michael Maurer <mjmaurer@yahoo.com>
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
/*
Macros to enumerate combinations and permutations of the remaining
cards in a deck.  Each macro accepts an argument containing code that
is executed in the inner loop of the enumeration.

------------------------------------------------------------------------
DECK_ENUMERATE_x_CARDS(deck, cards_var, action)
Visits every x-card subset of deck 'deck'.  For each subset, sets 'cards'
to the cards in the subset and invokes the code in 'var_action'.
	x = {1..7} 
        deck -- type Deck, one of {StdDeck, JokerDeck, AStudDeck}
        cards_var -- type deck##_CardMask, for use in action code, contains
        	     the current subset of cards, declared by caller
        action -- C code to run in macro inner loop


------------------------------------------------------------------------
DECK_ENUMERATE_x_CARDS_D(deck, cards_var, dead_cards, action)
Similar to DECK_ENUMERATE_x_CARDS(), but cards in 'dead_cards' are not
included in any subset.
  	dead_cards -- type deck##_CardMask, the set of cards not to deal


------------------------------------------------------------------------
DECK_MONTECARLO_N_CARDS_D(deck, cards_var, dead_cards,
                          num_cards, num_iter, action)
Similar to DECK_ENUMERATE_x_CARDS(), but instead of exhaustively enumerating
all possible subsets, this macro randomly samples the subsets (with
replacement).
	num_cards -- type int, the number of cards to store in cards_var
	num_iter -- type int, the number of samples to draw

------------------------------------------------------------------------
DECK_ENUMERATE_COMBINATIONS_D(deck, set_var, num_sets, set_sizes, 
                              dead_cards, action).
Visits every ordered set of set_sizes[i]-sized card unordered subsets of deck
'deck'.  This is intended for games such as stud or draw, where each player
will receive his own cards, and the number of received cards may differ among
players.
        set_var -- type deck##_CardMask[], for use in action code, gives 
                   the set of cards dealt to each player in this iteration,
                   declared by caller
        num_sets -- type int, dimension of set_sizes array (number of players)
        set_sizes -- type int[], number of cards to be received by each player
        dead_cards -- type deck##_CardMask, the set of cards not to deal
        action -- C code to run in macro inner loop

------------------------------------------------------------------------
DECK_ENUMERATE_PERMUTATIONS_D(deck, set_var, num_sets, set_sizes, 
                              dead_cards, action).
Visits every ordered set of set_sizes[i]-sized card ordered subsets of deck
'deck'.  This is intended for games in which each player receives his own
cards and the order of received cards is important.
        set_var -- type deck##_CardMask[], for use in action code, gives 
                   the set of cards dealt to each player in this iteration,
                   declared by caller
        num_sets -- type int, dimension of set_sizes array (number of players)
        set_sizes -- type int[], number of cards to be received by each player
        dead_cards -- type deck##_CardMask, the set of cards not to deal
        action -- C code to run in macro inner loop

------------------------------------------------------------------------
DECK_MONTECARLO_PERMUTATIONS_D(deck, set_var, num_sets, set_sizes, 
                               dead_cards, num_iter, action)
Like DECK_ENUMERATE_PERMUTATIONS_D(), but but instead of exhaustively
enumerating all possible ordered sets of subsets, this macro randomly samples
them (with replacement).
	num_iter -- type int, the number of samples to draw */

#ifndef ENUMERATE_H
#define ENUMERATE_H

#include <stdio.h>
#include <stdlib.h>
#include "pokereval_export.h"

#ifndef RANDOM
#define RANDOM rand
#endif

#define DECK_ENUMERATE_1_CARDS(deck, cards_var, action) \
do {                                                    \
  int _i1;                                              \
  deck##_CardMask _card;                                \
                                                        \
  for (_i1 = deck##_N_CARDS-1; _i1 >= 0; _i1--) {       \
    _card = deck##_MASK(_i1);                           \
    cards_var = _card;                                  \
    { action };                                         \
  };                                                    \
} while (0)

#define DECK_ENUMERATE_2_CARDS(deck, cards_var, action) \
do {                                                    \
  int _i1, _i2;                                         \
  deck##_CardMask _card1, _card2,                       \
    _n2;                                                \
                                                        \
  for (_i1 = deck##_N_CARDS-1; _i1 >= 0; _i1--) {       \
    _card1 = deck##_MASK(_i1);                          \
    for (_i2 = _i1-1; _i2 >= 0; _i2--) {                \
      _card2 = deck##_MASK(_i2);                        \
      deck##_CardMask_OR(_n2, _card1, _card2);          \
      cards_var = _n2;                                  \
      { action };                                       \
    };                                                  \
  };                                                    \
} while (0)

#define DECK_ENUMERATE_3_CARDS(deck, cards_var, action) \
do {                                                    \
  int _i1, _i2, _i3;                                    \
  deck##_CardMask _card1, _card2, _card3,               \
    _n2, _n3;                                           \
                                                        \
  for (_i1 = deck##_N_CARDS-1; _i1 >= 0; _i1--) {       \
    _card1 = deck##_MASK(_i1);                          \
    for (_i2 = _i1-1; _i2 >= 0; _i2--) {                \
      _card2 = deck##_MASK(_i2);                        \
      deck##_CardMask_OR(_n2, _card1, _card2);          \
      for (_i3 = _i2-1; _i3 >= 0; _i3--) {              \
        _card3 = deck##_MASK(_i3);                      \
	deck##_CardMask_OR(_n3, _n2, _card3);           \
        cards_var = _n3;                                \
        { action };                                     \
      };                                                \
    };                                                  \
  };                                                    \
} while (0)

#define DECK_ENUMERATE_4_CARDS(deck, cards_var, action) \
do {                                                    \
  int _i1, _i2, _i3, _i4;                               \
  deck##_CardMask _card1, _card2, _card3, _card4,       \
    _n2, _n3, _n4;                                      \
                                                        \
  for (_i1 = deck##_N_CARDS-1; _i1 >= 0; _i1--) {       \
    _card1 = deck##_MASK(_i1);                          \
    for (_i2 = _i1-1; _i2 >= 0; _i2--) {                \
      _card2 = deck##_MASK(_i2);                        \
      deck##_CardMask_OR(_n2, _card1, _card2);          \
      for (_i3 = _i2-1; _i3 >= 0; _i3--) {              \
        _card3 = deck##_MASK(_i3);                      \
	deck##_CardMask_OR(_n3, _n2, _card3);           \
        for (_i4 = _i3-1; _i4 >= 0; _i4--) {            \
          _card4 = deck##_MASK(_i4);                    \
	  deck##_CardMask_OR(_n4, _n3, _card4);         \
          cards_var = _n4;                              \
          { action };                                   \
        };                                              \
      };                                                \
    };                                                  \
  };                                                    \
} while (0)

#define DECK_ENUMERATE_5_CARDS(deck, cards_var, action)                   \
do {                                                                      \
  int _i1, _i2, _i3, _i4, _i5;                                            \
  deck##_CardMask _card1, _card2, _card3, _card4, _card5,                 \
    _n2, _n3, _n4, _n5;                                                   \
                                                                          \
  for (_i1 = deck##_N_CARDS-1; _i1 >= 0; _i1--) {                         \
    _card1 = deck##_MASK(_i1);                                            \
    for (_i2 = _i1-1; _i2 >= 0; _i2--) {                                  \
      _card2 = deck##_MASK(_i2);                                          \
      deck##_CardMask_OR(_n2, _card1, _card2);                            \
      for (_i3 = _i2-1; _i3 >= 0; _i3--) {                                \
        _card3 = deck##_MASK(_i3);                                        \
	deck##_CardMask_OR(_n3, _n2, _card3);                             \
        for (_i4 = _i3-1; _i4 >= 0; _i4--) {                              \
          _card4 = deck##_MASK(_i4);                                      \
	  deck##_CardMask_OR(_n4, _n3, _card4);                           \
          for (_i5 = _i4-1; _i5 >= 0; _i5--) {                            \
            _card5 = deck##_MASK(_i5);                                    \
            deck##_CardMask_OR(_n5, _n4, _card5);                         \
            cards_var = _n5;                                              \
            { action };                                                   \
          };                                                              \
        };                                                                \
      };                                                                  \
    };                                                                    \
  };                                                                      \
} while (0)                                                               
                                                                          
#define DECK_ENUMERATE_7_CARDS(deck, cards_var, action)                   \
do {                                                                      \
  int _i1, _i2, _i3, _i4, _i5, _i6, _i7;                                  \
  deck##_CardMask _card1, _card2, _card3, _card4, _card5, _card6, _card7, \
    _n2, _n3, _n4, _n5, _n6, _n7;                                         \
                                                                          \
  for (_i1 = deck##_N_CARDS-1; _i1 >= 0; _i1--) {                         \
    _card1 = deck##_MASK(_i1);                                            \
    for (_i2 = _i1-1; _i2 >= 0; _i2--) {                                  \
      _card2 = deck##_MASK(_i2);                                          \
      deck##_CardMask_OR(_n2, _card1, _card2);                            \
      for (_i3 = _i2-1; _i3 >= 0; _i3--) {                                \
        _card3 = deck##_MASK(_i3);                                        \
	deck##_CardMask_OR(_n3, _n2, _card3);                             \
        for (_i4 = _i3-1; _i4 >= 0; _i4--) {                              \
          _card4 = deck##_MASK(_i4);                                      \
	  deck##_CardMask_OR(_n4, _n3, _card4);                           \
          for (_i5 = _i4-1; _i5 >= 0; _i5--) {                            \
            _card5 = deck##_MASK(_i5);                                    \
            deck##_CardMask_OR(_n5, _n4, _card5);                         \
            for (_i6 = _i5-1; _i6 >= 0; _i6--) {                          \
              _card6 = deck##_MASK(_i6);                                  \
              deck##_CardMask_OR(_n6, _n5, _card6);                       \
              for (_i7 = _i6-1; _i7 >= 0; _i7--) {                        \
                _card7 = deck##_MASK(_i7);                                \
                deck##_CardMask_OR(_n7, _n6, _card7);                     \
                cards_var = _n7;                                          \
                { action };                                               \
              };                                                          \
            };                                                            \
          };                                                              \
        };                                                                \
      };                                                                  \
    };                                                                    \
  };                                                                      \
} while (0)


#define DECK_ENUMERATE_1_CARDS_D(deck, cards_var, dead_cards, action)   \
do {                                                                    \
  int _i1;                                                              \
  deck##_CardMask _card;                                                \
                                                                        \
  for (_i1 = deck##_N_CARDS-1; _i1 >= 0; _i1--) {                       \
    _card = deck##_MASK(_i1);                                           \
    if (deck##_CardMask_ANY_SET(dead_cards, _card))                     \
      continue;                                                         \
    cards_var = _card;                                                  \
    { action };                                                         \
  };                                                                    \
} while (0)

#define DECK_ENUMERATE_2_CARDS_D(deck, cards_var, dead_cards, action)   \
do {                                                                    \
  int _i1; int _i2;							\
  deck##_CardMask _card1; deck##_CardMask _card2;			\
  deck##_CardMask _n2;							\
                                                                        \
  for (_i1 = deck##_N_CARDS-1; _i1 >= 0; _i1--) {                       \
    _card1 = deck##_MASK(_i1);                                          \
    if (deck##_CardMask_ANY_SET(dead_cards, _card1))                    \
      continue;                                                         \
    for (_i2 = _i1-1; _i2 >= 0; _i2--) {                                \
      _card2 = deck##_MASK(_i2);                                        \
      if (deck##_CardMask_ANY_SET(dead_cards, _card2))                  \
        continue;                                                       \
      deck##_CardMask_OR(_n2, _card1, _card2);                          \
      cards_var = _n2;                                                  \
      { action };                                                       \
    };                                                                  \
  };                                                                    \
} while (0)                                                             
                                                                        
#define DECK_ENUMERATE_3_CARDS_D(deck, cards_var, dead_cards, action)   \
do {                                                                    \
  int _i1, _i2, _i3;                                                    \
  deck##_CardMask _card1, _card2, _card3,                               \
    _n2, _n3;                                                           \
                                                                        \
  for (_i1 = deck##_N_CARDS-1; _i1 >= 0; _i1--) {                       \
    _card1 = deck##_MASK(_i1);                                          \
    if (deck##_CardMask_ANY_SET(dead_cards, _card1))                    \
      continue;                                                         \
    for (_i2 = _i1-1; _i2 >= 0; _i2--) {                                \
      _card2 = deck##_MASK(_i2);                                        \
      if (deck##_CardMask_ANY_SET(dead_cards, _card2))                  \
        continue;                                                       \
      deck##_CardMask_OR(_n2, _card1, _card2);                          \
      for (_i3 = _i2-1; _i3 >= 0; _i3--) {                              \
        _card3 = deck##_MASK(_i3);                                      \
        if (deck##_CardMask_ANY_SET(dead_cards, _card3))                \
          continue;                                                     \
	deck##_CardMask_OR(_n3, _n2, _card3);                           \
        cards_var = _n3;                                                \
        { action };                                                     \
      };                                                                \
    };                                                                  \
  };                                                                    \
} while (0)                                                             
                                                                        
#define DECK_ENUMERATE_4_CARDS_D(deck, cards_var, dead_cards, action)   \
do {                                                                    \
  int _i1, _i2, _i3, _i4;                                               \
  deck##_CardMask _card1, _card2, _card3, _card4,                       \
    _n2, _n3, _n4;                                                      \
                                                                        \
  for (_i1 = deck##_N_CARDS-1; _i1 >= 0; _i1--) {                       \
    _card1 = deck##_MASK(_i1);                                          \
    if (deck##_CardMask_ANY_SET(dead_cards, _card1))                    \
      continue;                                                         \
    for (_i2 = _i1-1; _i2 >= 0; _i2--) {                                \
      _card2 = deck##_MASK(_i2);                                        \
      if (deck##_CardMask_ANY_SET(dead_cards, _card2))                  \
        continue;                                                       \
      deck##_CardMask_OR(_n2, _card1, _card2);                          \
      for (_i3 = _i2-1; _i3 >= 0; _i3--) {                              \
        _card3 = deck##_MASK(_i3);                                      \
        if (deck##_CardMask_ANY_SET(dead_cards, _card3))                \
          continue;                                                     \
	deck##_CardMask_OR(_n3, _n2, _card3);                           \
        for (_i4 = _i3-1; _i4 >= 0; _i4--) {                            \
          _card4 = deck##_MASK(_i4);                                    \
          if (deck##_CardMask_ANY_SET(dead_cards, _card4))              \
            continue;                                                   \
	  deck##_CardMask_OR(_n4, _n3, _card4);                         \
          cards_var = _n4;                                              \
          { action };                                                   \
        };                                                              \
      };                                                                \
    };                                                                  \
  };                                                                    \
} while (0)                                                             
                                                                        
#define DECK_ENUMERATE_5_CARDS_D(deck, cards_var, dead_cards, action)   \
do {                                                                    \
  int _i1, _i2, _i3, _i4, _i5;                                          \
  deck##_CardMask _card1, _card2, _card3, _card4, _card5,               \
    _n2, _n3, _n4, _n5;                                                 \
                                                                        \
  for (_i1 = deck##_N_CARDS-1; _i1 >= 0; _i1--) {                       \
    _card1 = deck##_MASK(_i1);                                          \
    if (deck##_CardMask_ANY_SET(dead_cards, _card1))                    \
      continue;                                                         \
    for (_i2 = _i1-1; _i2 >= 0; _i2--) {                                \
      _card2 = deck##_MASK(_i2);                                        \
      if (deck##_CardMask_ANY_SET(dead_cards, _card2))                  \
        continue;                                                       \
      deck##_CardMask_OR(_n2, _card1, _card2);                          \
      for (_i3 = _i2-1; _i3 >= 0; _i3--) {                              \
        _card3 = deck##_MASK(_i3);                                      \
        if (deck##_CardMask_ANY_SET(dead_cards, _card3))                \
          continue;                                                     \
	deck##_CardMask_OR(_n3, _n2, _card3);                           \
        for (_i4 = _i3-1; _i4 >= 0; _i4--) {                            \
          _card4 = deck##_MASK(_i4);                                    \
          if (deck##_CardMask_ANY_SET(dead_cards, _card4))              \
            continue;                                                   \
	  deck##_CardMask_OR(_n4, _n3, _card4);                         \
          for (_i5 = _i4-1; _i5 >= 0; _i5--) {                          \
            _card5 = deck##_MASK(_i5);                                  \
            if (deck##_CardMask_ANY_SET(dead_cards, _card5))            \
              continue;                                                 \
            deck##_CardMask_OR(_n5, _n4, _card5);                       \
            cards_var = _n5;                                            \
            { action };                                                 \
          };                                                            \
        };                                                              \
      };                                                                \
    };                                                                  \
  };                                                                    \
} while (0)




#define DECK_ENUMERATE_N_CARDS_D(deck, cards_var, n_cards, dead_cards,     \
                                 action)                                   \
do {                                                                       \
  int _i1; int _i2; int _i3;						\
  int _i4; int _i5; int _i6;						\
  int _i7; int _i8; int _i9;						\
  deck##_CardMask _card1; deck##_CardMask _card2; deck##_CardMask _card3; \
  deck##_CardMask _card4; deck##_CardMask _card5; deck##_CardMask _card6; \
  deck##_CardMask _card7; deck##_CardMask _card8; deck##_CardMask _card9; \
  deck##_CardMask _n1; deck##_CardMask _n2; deck##_CardMask _n3;	\
  deck##_CardMask _n4; deck##_CardMask _n5; deck##_CardMask _n6;	\
  deck##_CardMask _n7; deck##_CardMask _n8; deck##_CardMask _n9;	\
                                                                           \
  _i1 = _i2 = _i3 = _i4 = _i5 = _i6 = _i7 = _i8 = _i9 = 0;                 \
  deck##_CardMask_RESET(_card9);                                           \
  _card1 = _card2 = _card3 = _card4 = _card5 = _card6                      \
    = _card7 = _card8 = _card9;                                            \
  deck##_CardMask_RESET(_n9);                                              \
  _n1 = _n2 = _n3 = _n4 = _n5 = _n6 = _n7 = _n8 = _n9;                     \
                                                                           \
  switch (n_cards) {                                                       \
  default:                                                                 \
  case 9:                                                                  \
  case 0:                                                                  \
    break;                                                                 \
  case 8:                                                                  \
    _i2 = deck##_N_CARDS-1;                                                \
    break;                                                                 \
  case 7:                                                                  \
    _i3 = deck##_N_CARDS-1;                                                \
    break;                                                                 \
  case 6:                                                                  \
    _i4 = deck##_N_CARDS-1;                                                \
    break;                                                                 \
  case 5:                                                                  \
    _i5 = deck##_N_CARDS-1;                                                \
    break;                                                                 \
  case 4:                                                                  \
    _i6 = deck##_N_CARDS-1;                                                \
    break;                                                                 \
  case 3:                                                                  \
    _i7 = deck##_N_CARDS-1;                                                \
    break;                                                                 \
  case 2:                                                                  \
    _i8 = deck##_N_CARDS-1;                                                \
    break;                                                                 \
  case 1:                                                                  \
    _i9 = deck##_N_CARDS-1;                                                \
    break;                                                                 \
  }                                                                        \
  switch (n_cards) {                                                       \
  default:                                                                 \
    fprintf(stderr, "ENUMERATE_N_CARDS: n-cards must be in range 0..9\n"); \
    break;                                                                 \
                                                                           \
  case 9:                                                                  \
    for (_i1 = deck##_N_CARDS-1; _i1 >= 0; _i1--) {                        \
      _card1 = deck##_MASK(_i1);                                           \
      if (deck##_CardMask_ANY_SET(dead_cards, _card1))                     \
        continue;                                                          \
      _n1 = _card1;                                                        \
      for (_i2 = _i1-1; _i2 >= 0; _i2--) {                                 \
  case 8:                                                                  \
        _card2 = deck##_MASK(_i2);                                         \
        if (deck##_CardMask_ANY_SET(dead_cards, _card2))                   \
          continue;                                                        \
        deck##_CardMask_OR(_n2, _n1, _card2);                              \
        for (_i3 = _i2-1; _i3 >= 0; _i3--) {                               \
  case 7:                                                                  \
          _card3 = deck##_MASK(_i3);                                       \
          if (deck##_CardMask_ANY_SET(dead_cards, _card3))                 \
            continue;                                                      \
          deck##_CardMask_OR(_n3, _n2, _card3);                            \
          for (_i4 = _i3-1; _i4 >= 0; _i4--) {                             \
  case 6:                                                                  \
            _card4 = deck##_MASK(_i4);                                     \
            if (deck##_CardMask_ANY_SET(dead_cards, _card4))               \
              continue;                                                    \
            deck##_CardMask_OR(_n4, _n3, _card4);                          \
            for (_i5 = _i4-1; _i5 >= 0; _i5--) {                           \
  case 5:                                                                  \
              _card5 = deck##_MASK(_i5);                                   \
              if (deck##_CardMask_ANY_SET(dead_cards, _card5))             \
                continue;                                                  \
              deck##_CardMask_OR(_n5, _n4, _card5);                        \
              for (_i6 = _i5-1; _i6 >= 0; _i6--) {                         \
  case 4:                                                                  \
                _card6 = deck##_MASK(_i6);                                 \
                if (deck##_CardMask_ANY_SET(dead_cards, _card6))           \
                  continue;                                                \
                deck##_CardMask_OR(_n6, _n5, _card6);                      \
                for (_i7 = _i6-1; _i7 >= 0; _i7--) {                       \
  case 3:                                                                  \
                  _card7 = deck##_MASK(_i7);                               \
                  if (deck##_CardMask_ANY_SET(dead_cards, _card7))         \
                    continue;                                              \
                  deck##_CardMask_OR(_n7, _n6, _card7);                    \
                  for (_i8 = _i7-1; _i8 >= 0; _i8--) {                     \
  case 2:                                                                  \
                    _card8 = deck##_MASK(_i8);                             \
                    if (deck##_CardMask_ANY_SET(dead_cards, _card8))       \
                      continue;                                            \
                    deck##_CardMask_OR(_n8, _n7, _card8);                  \
                    for (_i9 = _i8-1; _i9 >= 0; _i9--) {                   \
  case 1:                                                                  \
                      _card9 = deck##_MASK(_i9);                           \
                      if (deck##_CardMask_ANY_SET(dead_cards, _card9))     \
                        continue;                                          \
                      deck##_CardMask_OR(_n9, _n8, _card9);                \
  case 0:                                                                  \
                        cards_var = _n9;                                   \
                        { action };                                        \
		    }                                                      \
		  }                                                        \
		}                                                          \
	      }                                                            \
	    }                                                              \
	  }                                                                \
	}                                                                  \
      }                                                                    \
    }                                                                      \
  }                                                                        \
} while (0)                                                                
                                                                           
                                                                           
#define DECK_ENUMERATE_N_CARDS(deck, cards_var, n_cards, action)           \
do {                                                                       \
  int _i1, _i2, _i3, _i4, _i5, _i6, _i7, _i8, _i9;                         \
  deck##_CardMask _card1, _card2, _card3, _card4, _card5,                  \
    _card6, _card7, _card8, _card9,                                        \
    _n1, _n2, _n3, _n4, _n5, _n6, _n7, _n8, _n9;                           \
                                                                           \
  _i1 = _i2 = _i3 = _i4 = _i5 = _i6 = _i7 = _i8 = _i9 = 0;                 \
  deck##_CardMask_RESET(_card9);                                           \
  _card1 = _card2 = _card3 = _card4 = _card5 = _card6                      \
    = _card7 = _card8 = _card9;                                            \
  deck##_CardMask_RESET(_n9);                                              \
  _n1 = _n2 = _n3 = _n4 = _n5 = _n6 = _n7 = _n8 = _n9;                     \
                                                                           \
  switch (n_cards) {                                                       \
  default:                                                                 \
  case 9:                                                                  \
  case 0:                                                                  \
    break;                                                                 \
  case 8:                                                                  \
    _i2 = deck##_N_CARDS-1;                                                \
    break;                                                                 \
  case 7:                                                                  \
    _i3 = deck##_N_CARDS-1;                                                \
    break;                                                                 \
  case 6:                                                                  \
    _i4 = deck##_N_CARDS-1;                                                \
    break;                                                                 \
  case 5:                                                                  \
    _i5 = deck##_N_CARDS-1;                                                \
    break;                                                                 \
  case 4:                                                                  \
    _i6 = deck##_N_CARDS-1;                                                \
    break;                                                                 \
  case 3:                                                                  \
    _i7 = deck##_N_CARDS-1;                                                \
    break;                                                                 \
  case 2:                                                                  \
    _i8 = deck##_N_CARDS-1;                                                \
    break;                                                                 \
  case 1:                                                                  \
    _i9 = deck##_N_CARDS-1;                                                \
    break;                                                                 \
  }                                                                        \
  switch (n_cards) {                                                       \
  default:                                                                 \
    fprintf(stderr, "ENUMERATE_N_CARDS: n-cards must be in range 0..9\n"); \
    break;                                                                 \
                                                                           \
  case 9:                                                                  \
    for (_i1 = deck##_N_CARDS-1; _i1 >= 0; _i1--) {                        \
      _card1 = deck##_MASK(_i1);                                           \
      _n1 = _card1;                                                        \
      for (_i2 = _i1-1; _i2 >= 0; _i2--) {                                 \
  case 8:                                                                  \
        _card2 = deck##_MASK(_i2);                                         \
        deck##_CardMask_OR(_n2, _n1, _card2);                              \
        for (_i3 = _i2-1; _i3 >= 0; _i3--) {                               \
  case 7:                                                                  \
          _card3 = deck##_MASK(_i3);                                       \
          deck##_CardMask_OR(_n3, _n2, _card3);                            \
          for (_i4 = _i3-1; _i4 >= 0; _i4--) {                             \
  case 6:                                                                  \
            _card4 = deck##_MASK(_i4);                                     \
            deck##_CardMask_OR(_n4, _n3, _card4);                          \
            for (_i5 = _i4-1; _i5 >= 0; _i5--) {                           \
  case 5:                                                                  \
              _card5 = deck##_MASK(_i5);                                   \
              deck##_CardMask_OR(_n5, _n4, _card5);                        \
              for (_i6 = _i5-1; _i6 >= 0; _i6--) {                         \
  case 4:                                                                  \
                _card6 = deck##_MASK(_i6);                                 \
                deck##_CardMask_OR(_n6, _n5, _card6);                      \
                for (_i7 = _i6-1; _i7 >= 0; _i7--) {                       \
  case 3:                                                                  \
                  _card7 = deck##_MASK(_i7);                               \
                  deck##_CardMask_OR(_n7, _n6, _card7);                    \
                  for (_i8 = _i7-1; _i8 >= 0; _i8--) {                     \
  case 2:                                                                  \
                    _card8 = deck##_MASK(_i8);                             \
                    deck##_CardMask_OR(_n8, _n7, _card8);                  \
                    for (_i9 = _i8-1; _i9 >= 0; _i9--) {                   \
  case 1:                                                                  \
                      _card9 = deck##_MASK(_i9);                           \
                      deck##_CardMask_OR(_n9, _n8, _card9);                \
  case 0:                                                                  \
                        cards_var = _n9;                                   \
                        { action };                                        \
		    }                                                      \
		  }                                                        \
		}                                                          \
	      }                                                            \
	    }                                                              \
	  }                                                                \
	}                                                                  \
      }                                                                    \
    }                                                                      \
  }                                                                        \
} while (0)

#include "combinations.h"
#define DECK_ENUMERATE_COMBINATIONS_D(deck, set_var, num_sets, set_sizes, \
                                      dead_cards, action) \
do { \
  Combinations *_combos; \
  int *_ncombo; \
  int *_curIndex; \
  int **_curElem; \
  deck##_CardMask *_curHand; \
  deck##_CardMask _avail; \
  deck##_CardMask _unavail; \
  int _lowestRoll; \
  int _couldIncr; \
  int _i, _j, _k; \
   \
  _combos = (Combinations *) malloc(num_sets * sizeof(Combinations)); \
  _ncombo = (int *) malloc(num_sets * sizeof(int)); \
  _curIndex = (int *) malloc(num_sets * sizeof(int)); \
  _curElem = (int **) malloc(num_sets * sizeof(int)); \
  _curHand = (deck##_CardMask *) malloc(num_sets * sizeof(deck##_CardMask)); \
  for (_i=0; _i<num_sets; _i++) { \
    _combos[_i] = init_combinations(deck##_N_CARDS, set_sizes[_i]); \
    _ncombo[_i] = num_combinations(_combos[_i]); \
    _curElem[_i] = (int *) malloc(set_sizes[_i] * sizeof(int)); \
  } \
  _unavail = dead_cards; \
  _lowestRoll = 0; \
  do { \
    /* for each player rolling over, find the lowest available hand */ \
    for (_i=_lowestRoll; _i<num_sets; _i++) { \
      /* for each candidate hand for player i */ \
      for (_j=0; _j<_ncombo[_i]; _j++) { \
        get_combination(_combos[_i], _j, _curElem[_i]); \
        deck##_CardMask_RESET(set_var[_i]); \
        for (_k=0; _k<set_sizes[_i]; _k++) \
          deck##_CardMask_SET(set_var[_i], _curElem[_i][_k]); \
        if (!deck##_CardMask_ANY_SET(_unavail, set_var[_i])) \
          break;	/* this hand is available for player i */ \
      } \
      if (_j == _ncombo[_i]) { printf("not enough cards\n"); exit(1); } \
      deck##_CardMask_OR(_unavail, _unavail, set_var[_i]); \
      _curIndex[_i] = _j; \
    } \
 \
    { action } \
 \
    /* Now increment the least significant player's hand, and if it overflows, \
       carry into the next player's hand.  Set lowestRoll to the most \
       significant player whose hand overflowed. */ \
    _couldIncr = 0; \
    for (_i=num_sets-1; _i>=0; _i--) { \
      deck##_CardMask_NOT(_avail, set_var[_i]); \
      deck##_CardMask_AND(_unavail, _unavail, _avail); \
      for (_j=_curIndex[_i]+1; _j<_ncombo[_i]; _j++) { \
        get_combination(_combos[_i], _j, _curElem[_i]); \
        deck##_CardMask_RESET(set_var[_i]); \
        for (_k=0; _k<set_sizes[_i]; _k++) \
          deck##_CardMask_SET(set_var[_i], _curElem[_i][_k]); \
        if (!deck##_CardMask_ANY_SET(_unavail, set_var[_i])) \
          break;	/* this hand is available for player i */ \
      } \
      if (_j < _ncombo[_i]) { \
        deck##_CardMask_OR(_unavail, _unavail, set_var[_i]); \
        _curIndex[_i] = _j; \
        _couldIncr = 1; \
        _lowestRoll = _i + 1; \
        break; \
      } \
    } \
  } while (_couldIncr); \
  for (_i=0; _i<num_sets; _i++) { \
    free_combinations(_combos[_i]); \
    free(_curElem[_i]); \
  } \
  free(_combos); \
  free(_ncombo); \
  free(_curIndex); \
  free(_curElem); \
  free(_curHand); \
} while (0)

#define DECK_ENUMERATE_PERMUTATIONS_D(deck, set_var, num_sets, set_sizes, \
                                      dead_cards, action)                 \
do {                                                                      \
  deck##_CardMask _orMask[deck##_N_CARDS+1];                              \
  int _i, _j, _t, _index, _nLiveCards, _nCards,                           \
    _liveCards[deck##_N_CARDS], _indices[deck##_N_CARDS+1];               \
  _nCards = _nLiveCards = 0;                                              \
  for (_i=0; _i < (num_sets); _i++)                                       \
    _nCards += (set_sizes)[_i];                                           \
                                                                          \
  for (_i=0; _i < deck##_N_CARDS; _i++)                                   \
    if (!(deck##_CardMask_CARD_IS_SET(dead_cards, _i)))                   \
      _liveCards[_nLiveCards++] = _i;                                     \
                                                                          \
  if (_nLiveCards < _nCards)                                              \
    fprintf(stderr, "ENUMERATE_PERMUTATIONS: not enough cards\n");        \
  else {                                                                  \
    deck##_CardMask_RESET(_orMask[0]);                                    \
    for (_i=1; _i <= _nCards; _i++) {                                     \
      _indices[_i] = _i-1;                                                \
      deck##_CardMask_OR(_orMask[_i], _orMask[_i-1],                      \
                         deck##_MASK(_liveCards[_indices[_i]]));          \
    };                                                                    \
                                                                          \
    for (;;) {                                                            \
      set_var[0] = _orMask[set_sizes[0]];                                 \
      _t = set_sizes[0];                                                  \
      for (_j=1; _j < num_sets; _j++) {                                   \
        deck##_CardMask_XOR(set_var[_j], _orMask[_t + set_sizes[_j]],     \
                            _orMask[_t]);                                 \
        _t += set_sizes[_j];                                              \
      };                                                                  \
      { action };                                                         \
                                                                          \
      _index = _nCards;                                                   \
      do {                                                                \
        ++_indices[_index];                                               \
        while (_indices[_index] >= _nLiveCards) {                         \
          --_index;                                                       \
          if (_index == 0)                                                \
            break;                                                        \
          ++_indices[_index];                                             \
        };                                                                \
        if (_index == 0)                                                  \
          break;                                                          \
      }                                                                   \
      while (deck##_CardMask_CARD_IS_SET(_orMask[_index-1],               \
                                         _liveCards[_indices[_index]]));  \
      if (_index == 0)                                                    \
        break;                                                            \
      deck##_CardMask_OR(_orMask[_index], _orMask[_index-1],              \
                         deck##_MASK(_liveCards[_indices[_index]]));      \
      for (_i=_index+1; _i <= _nCards; _i++) {                            \
        _indices[_i] = 0;                                                 \
        while (deck##_CardMask_CARD_IS_SET(_orMask[_i-1],                 \
                                           _liveCards[_indices[_i]]))     \
          ++_indices[_i];                                                 \
        deck##_CardMask_OR(_orMask[_i], _orMask[_i-1],                    \
                           deck##_MASK(_liveCards[_indices[_i]]));        \
      };                                                                  \
    };                                                                    \
  };                                                                      \
} while (0)

#define DECK_MONTECARLO_N_CARDS_D(deck, cards_var, dead_cards,	\
                                  num_cards, num_iter, action)	\
do {                                                    	\
  int _i, _j, _c;                                              	\
  								\
  for (_i=0; _i<num_iter; _i++) {				\
    deck##_CardMask _used;					\
    _used = dead_cards;						\
    deck##_CardMask_RESET(cards_var);				\
    for (_j=0; _j<num_cards; _j++) {				\
      do {							\
        _c = RANDOM() % deck##_N_CARDS;				\
      } while (deck##_CardMask_CARD_IS_SET(_used, _c));		\
      deck##_CardMask_SET(cards_var, _c);			\
      deck##_CardMask_SET(_used, _c);				\
    }								\
    { action };                                         	\
  };                                                    	\
} while (0)

#define DECK_MONTECARLO_PERMUTATIONS_D(deck, set_var, num_sets, set_sizes, \
                                       dead_cards, num_iter, action)	\
do {                                                    	\
  int _i, _j, _k, _c;                                           \
  								\
  for (_i=0; _i<num_iter; _i++) {				\
    deck##_CardMask _used;					\
    _used = dead_cards;						\
    for (_j=0; _j<num_sets; _j++) {				\
      deck##_CardMask_RESET(set_var[_j]);			\
      for (_k=0; _k<set_sizes[_j]; _k++) {			\
        do {							\
          _c = RANDOM() % deck##_N_CARDS;			\
        } while (deck##_CardMask_CARD_IS_SET(_used, _c));	\
        deck##_CardMask_SET(set_var[_j], _c);			\
        deck##_CardMask_SET(_used, _c);				\
      }								\
    }								\
    { action };                                         	\
  };                                                    	\
} while (0)

#define ENUMERATE_1_CARDS(c,a) DECK_ENUMERATE_1_CARDS(Deck, c, a)
#define ENUMERATE_2_CARDS(c,a) DECK_ENUMERATE_2_CARDS(Deck, c, a)
#define ENUMERATE_3_CARDS(c,a) DECK_ENUMERATE_3_CARDS(Deck, c, a)
#define ENUMERATE_4_CARDS(c,a) DECK_ENUMERATE_4_CARDS(Deck, c, a)
#define ENUMERATE_5_CARDS(c,a) DECK_ENUMERATE_5_CARDS(Deck, c, a)
#define ENUMERATE_7_CARDS(c,a) DECK_ENUMERATE_7_CARDS(Deck, c, a)

#define ENUMERATE_1_CARDS_D(c,d,a) DECK_ENUMERATE_1_CARDS_D(Deck, c, d, a)
#define ENUMERATE_2_CARDS_D(c,d,a) DECK_ENUMERATE_2_CARDS_D(Deck, c, d, a)
#define ENUMERATE_3_CARDS_D(c,d,a) DECK_ENUMERATE_3_CARDS_D(Deck, c, d, a)
#define ENUMERATE_4_CARDS_D(c,d,a) DECK_ENUMERATE_4_CARDS_D(Deck, c, d, a)
#define ENUMERATE_5_CARDS_D(c,d,a) DECK_ENUMERATE_5_CARDS_D(Deck, c, d, a)
#define ENUMERATE_7_CARDS_D(c,d,a) DECK_ENUMERATE_7_CARDS_D(Deck, c, d, a)

#define ENUMERATE_N_CARDS(c,n,a) DECK_ENUMERATE_N_CARDS(Deck, c, n, a)

#define ENUMERATE_N_CARDS_D(c,n,d,a) \
  DECK_ENUMERATE_N_CARDS_D(Deck, c, n, d, a)

#define ENUMERATE_PERMUTATIONS_D(s, n, ss, dc, a) \
  DECK_ENUMERATE_PERMUTATIONS(Deck, s, n, ss, dc, a)

#define MONTECARLO_N_CARDS_D(c,n,d,nc,ni,a) \
  DECK_MONTECARLO_N_CARDS_D(Deck, c, n, d, nc, ni, a)

#define MONTECARLO_PERMUTATIONS_D(s, n, ss, dc, ni, a) \
  DECK_MONTECARLO_PERMUTATIONS(Deck, s, n, ss, dc, ni, a)

#endif /* ENUMERATE_H */
