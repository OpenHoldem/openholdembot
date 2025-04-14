//******************************************************************************
//
// This file is part of the OpenHoldem project
//    Source code:           https://github.com/OpenHoldem/openholdembot/
//    Forums:                http://www.maxinmontreal.com/forums/index.php
//    Licensed under GPL v3: http://www.gnu.org/licenses/gpl.html
//
//******************************************************************************
//
// Purpose: Determining hand-ranges for weighted PrWin
//
//******************************************************************************

#include "pch.h"
#include "PrWinHandranges.h"



bool IsHandInWeightedRange(const int card1, const int card2, 
							const int willplay, const int wontplay, 
							const int topclip, const int mustplay)
{
	/*
	p |---|	---------
	r |   |   /      \
	o |   |  /        \
	b |   | /          \
	a |   |/            \
	b |                  \
	l -----------------------------------------------------
	e	 1	2	   3	 4	 handrank169 ->
	1=mustplay
	2=topclip
	3=willplay
	4=wontplay
	(with the current statistics available in OH only willplay and wontplay should be used.
	 mustplay and topclip are debatable constructs at the best of times 2008-01-29)
		
	Original code by Matrix (slightly confusing):
	http://code.google.com/p/openholdembot/source/browse/trunk/OpenHoldem/CIteratorThread.cpp?spec=svn666&r=666
	*/
	extern int pair2rank_offsuited[170], pair2rank_suited[170];
	int suit_1 = card1 / 13;
	int suit_2 = card2 / 13;
	int rank_1 = card1 % 13;
	int rank_2 = card2 % 13;
	int	hrank  = 0;

	if (rank_2 > rank_1) //normalise the card order
	{
		SwapInts(&rank_1, &rank_2);
	}
	int handrank_offset = rank_1 * 13 + rank_2; //offset into handrank table

	if (suit_1 == suit_2)
	{
		hrank = pair2rank_suited[handrank_offset]; //suited
	}
	else
	{
		hrank = pair2rank_offsuited[handrank_offset]; //not suited
	}

	if (hrank >= wontplay)
	{
		// No good, never play these
		return false; 
	}
	if (hrank <= mustplay)
	{ 
		// OK, assumed opponent will always play these
		return true; 
	}
	if ((hrank > topclip) && (hrank <= willplay))
	{
		// OK, in the 100% region
		return true;
	}

	// We now handle the probability slopes
	if (hrank <= topclip) //handle the good cards not likely to be one-betted with
	{
		int randomized_topclip = topclip * (1 - rand());
		return (hrank > randomized_topclip);
	}

	//now finish with poorish hands of reduced probability
	int wontplay_willplay_distance = wontplay - willplay;
	if (wontplay_willplay_distance < 3)
	{
		//protect ourselves from effect of bad wontplay
		return true; 
	}

	int randomized_willplay = willplay + rand() % wontplay_willplay_distance;

	return (hrank < randomized_willplay);
}
