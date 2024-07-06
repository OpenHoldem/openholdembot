/*
 *  evx_generate.c: a program to generate the switch files for the
 *  eval_exactly_5 and eval_exactly_7 (evx) evaluators
 *
 *  Copyright (C) 1994-99    Clifford T. Matthews, Brian Goetz
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

#include "poker_defs.h"
#include <stdio.h>
#include <stdlib.h>

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#if defined(_MSC_VER)
#define HAVE_SYS_STAT_H
#endif /* defined(_MSC_VER) */

#ifdef HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

typedef enum {
  MAY_HAVE_FOUR_OF_A_KIND  = (1 << 0),
  MAY_HAVE_FLUSH           = (1 << 1),
  MUST_HAVE_STRAIGHT       = (1 << 2),
  MAY_HAVE_FULL_HOUSE      = (1 << 3),
  MAY_HAVE_THREE_OF_A_KIND = (1 << 4),
  MUST_HAVE_AT_LEAST_PAIR  = (1 << 5),
  MUST_HAVE_PAIR           = (1 << 6),
} may_have_t;

#define MAY_HAVE_COMBINATIONS 0x80
#define HAND_SIZE 5

/* This is a bit array of booleans.  For each set of hand possibilities
 * (e.g. "might have a flush or two pair, but not a full house", it
 * indicates all ranks known to have those properties.
 */
static uint8 may_have[MAY_HAVE_COMBINATIONS][StdDeck_N_RANKMASKS / 8];

static void compute_cases (void);
static void print_cases (void);

int
main (int argc, char** argv)
{
  FILE *fp;

  /* Copy the preamble to stdout. */
  fp = fopen ("evx_preamble.cfrag", "r");
  if (fp == NULL) {
      fprintf (stderr, "Unable to read evx_preamble.c\n");
      exit (-1);
    }
  puts("/* This file is machine-generated -- DO NOT EDIT! */\n");
  {
    struct stat sbuf;
    char *p;

    fstat (fileno(fp), &sbuf);
    p = malloc(sbuf.st_size + 1);
    (void)fread (p, sbuf.st_size, 1, fp);
    p[sbuf.st_size] = 0;
    printf (p, CARDS_DEALT);
  }
  fclose (fp);

  /* Compute which cases go where. */
  compute_cases ();
  
  /* Print out the cases. */
  print_cases ();

  puts ("    default:\n"
	"      abort ();\n"
	"      break;\n"
	"    }\n"
	"\n"
	"  abort ();\n"
	"  return 0;   /* Quiet the compiler. */\n"
	"}");

  return 0;
}


static void
print_cases ()
{
  unsigned possible;

  for (possible = 0; possible < MAY_HAVE_COMBINATIONS; possible++)
    {
      unsigned n;
      int found;

      /* First see if we have _any_ cases for this switch. */
      for (n = 0, found = 0; n < StdDeck_N_RANKMASKS / 8; n++)
	if (may_have[possible][n] != 0)
	  {
	    found = 1;
	    break;
	  }

      /* If we found an example, print it out. */
      if (found)
	{
	  unsigned cards;

	  /* Print out all of the case statements. */
	  for (cards = 0; cards < StdDeck_N_RANKMASKS; cards++)
	    if (may_have[possible][cards / 8] & (1 << (cards & 7)))
	      printf ("    case %u:\n", cards);

	  /* Print out all of the #defines for stuff we know. */
	  if (!(possible & MAY_HAVE_FLUSH)
		|| !(possible & MUST_HAVE_STRAIGHT))
	    puts ("#define STRAIGHT_FLUSH_helper_P(suit) 0");
	  if (!(possible & MAY_HAVE_FOUR_OF_A_KIND))
	    puts ("#define FOUR_OF_A_KIND_complete_P() 0");
	  if (!(possible & MAY_HAVE_FLUSH))
	    puts ("#define FLUSH_helper_P() 0");
	  if (!(possible & MAY_HAVE_FULL_HOUSE))
	    puts ("#define FULL_HOUSE_complete_P(three_info) 0");
	  if (!(possible & MAY_HAVE_THREE_OF_A_KIND))
	    puts ("#define THREE_OF_A_KIND_helper_P() 0");
	  
	  /* For these three, we _know_ if we do or do not have them based
	   * solely on the ranks of the cards in our hands.
	   */
	  printf ("#define STRAIGHT_P() %d\n",
		  (possible & MUST_HAVE_STRAIGHT) != 0);
	  printf ("#define AT_LEAST_PAIR_P() %d\n",
		  (possible & MUST_HAVE_AT_LEAST_PAIR) != 0);
	  printf ("#define PAIR_P() %d\n", (possible & MUST_HAVE_PAIR) != 0);

	  puts ("\n"
		"#include \"inlines/evx_action.h\"\n");

	  /* #undef everything. */
	  puts ("#undef STRAIGHT_FLUSH_helper_P\n"
		"#undef FOUR_OF_A_KIND_complete_P\n"
		"#undef STRAIGHT_P\n"
		"#undef FLUSH_helper_P\n"
		"#undef FULL_HOUSE_complete_P\n"
		"#undef THREE_OF_A_KIND_helper_P\n"
		"#undef AT_LEAST_PAIR_P\n"
		"#undef PAIR_P");

	  /* Print out the break. */
	  puts ("      break;");
	}
    }
}


static inline unsigned
num_bits (uint32 n)
{
  unsigned bits;
  for (bits = 0; n != 0; n &= n - 1)
    bits++;
  return bits;
}


static void
compute_cases ()
{
  unsigned cards;

  for (cards = 0; cards < StdDeck_N_RANKMASKS; cards++)
    {
      int i, possible_hands;
      unsigned num_unique_ranks;

      /* Count how many distinct ranks we have, and bail out if
       * this hand is impossible.
       */
      num_unique_ranks = num_bits (cards);
      if (num_unique_ranks > CARDS_DEALT
	  || num_unique_ranks < (CARDS_DEALT + 3) / 4)
/*-->*/ continue;

      possible_hands = 0;

      /* Check for a straight. */
      for (i = 0; i <= StdDeck_Rank_COUNT - HAND_SIZE; i++)
	if (((cards >> i) & ((1 << HAND_SIZE) - 1)) == ((1 << HAND_SIZE) - 1))
	  {
	    possible_hands |= MUST_HAVE_STRAIGHT;
	    break;
	  }

      if ((cards & StdRules_FIVE_STRAIGHT) == StdRules_FIVE_STRAIGHT)
	  possible_hands |= MUST_HAVE_STRAIGHT;

      /* Check for a possible flush. */
      if (num_unique_ranks >= HAND_SIZE)
	possible_hands |= MAY_HAVE_FLUSH;

      /* Check for a possible four of a kind. */
      if (num_unique_ranks <= CARDS_DEALT - 3)
	possible_hands |= MAY_HAVE_FOUR_OF_A_KIND;

      /* Check for a possible three of a kind. */
      if (num_unique_ranks <= CARDS_DEALT - 2)
	possible_hands |= MAY_HAVE_THREE_OF_A_KIND;

      /* Check for a possible pair when _no better hand is possible_. */
      if (num_unique_ranks == CARDS_DEALT - 1)
	possible_hands |= MUST_HAVE_PAIR;

      /* Check for at least a pair. */
      if (num_unique_ranks <= CARDS_DEALT - 1)
	possible_hands |= MUST_HAVE_AT_LEAST_PAIR;

      /* Check for a possible full house. */
      if (num_unique_ranks <= CARDS_DEALT - 3)
	possible_hands |= MAY_HAVE_FULL_HOUSE;

      /* Record the proper case statement for this set of cards. */
      may_have[possible_hands][cards / 8] |= 1 << (cards & 7);
    }
}
