
#include "stdafx.h"
#include "poker_defs.h"
#include "inlines/eval.h"
#include <conio.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CWinApp theApp;

using namespace std;

double calc_pokerval(HandVal hv, int n);
void do_calc(CardMask cplCards, CardMask oppCards, CardMask comCards);
unsigned int win, los, tie;

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[]) {
	int nRetCode = 0;
	CardMask		comCards, plCards, oppCards, deadCards;
	FILE			*fp;
	CString			fn;
	int				rowswritten, rowswrittentot, nhands;
	char			pcard0, pcard1, ocard0, ocard1;
	//char			*mask, pmask[100], omask[100];
	char			start=0, end=50;

	if (argc==3) {
		start = atoi(argv[1]);
		end = atoi(argv[2]);
	}

	fn.Format("versus_%d_%d.bin", start, end);
	printf("Running from %d to %d\n\n", start, end);


	// initialize MFC and print and error on failure
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0)) {
		_tprintf(_T("Fatal Error: MFC initialization failed\n"));
		nRetCode = 1;
	}
	else {

		// Enumerate through all 2652 possible player hands
		rowswrittentot = 0;
		for (pcard0=start; pcard0<=end; pcard0++) {
			rowswritten = 0;
			for (pcard1=pcard0+1; pcard1<=51; pcard1++) {
				CardMask_RESET(plCards);
				CardMask_SET(plCards, pcard0);
				CardMask_SET(plCards, pcard1);

				//pcard0 = StdDeck_MAKE_CARD(Rank_6, Suit_HEARTS);		// testing
				//pcard1 = StdDeck_MAKE_CARD(Rank_9, Suit_HEARTS);		// testing
				//CardMask_SET(plCards, pcard0);		// testing
				//CardMask_SET(plCards, pcard1);		// testing

				// Enumerate through all possible opponent hands (excludes player cards)
				nhands=0;
				for (ocard0=0; ocard0<=50; ocard0++) {
					for (ocard1=ocard0+1; ocard1<=51; ocard1++) {
						if (ocard0!=pcard0 && ocard0!=pcard1 && ocard1!=pcard0 && ocard1!=pcard1) {
							nhands++;
							CardMask_RESET(oppCards);
							CardMask_SET(oppCards, ocard0);
							CardMask_SET(oppCards, ocard1);

							// Enumerate through all possible river situations (excludes player cards and opponent cards)
							CardMask_OR(deadCards, plCards, oppCards);
							win = los = tie = 0;
							ENUMERATE_N_CARDS_D(comCards, 5, deadCards,
								{
									do_calc(plCards, oppCards, comCards);
								});

							// Save the results
							fp = fopen(fn.GetString(), "ab");
							BYTE	byte[8];
							memcpy(&byte[0], &win, sizeof(unsigned int));
							memcpy(&byte[4], &los, sizeof(unsigned int));
							fwrite((void *) &byte, sizeof(byte), 1, fp);
							fclose(fp);
							rowswritten++;
							rowswrittentot++;

							// output for debugging
							//mask = StdDeck_maskString(plCards); strcpy(pmask, mask);
							//mask = StdDeck_maskString(oppCards); strcpy(omask, mask);
							//printf("%d %d | <%s> <%s> | %d %d %d %.3f %.3f %.3f\n", 
							//	pcard0, pcard1, pmask, omask, 
							//	win, tie, los,
							//	(double) win / (double) 1712304, (double) tie / (double) 1712304, (double) los / (double) 1712304);
						}
					}
				}
				printf("%d %d enumerated %d opponent hands.\n", pcard0, pcard1, nhands);
			}
			printf("%d> Wrote %d rows.\n", pcard0, rowswritten);
		}
		printf("TOTAL ROWS WRITTEN: %d\n", rowswrittentot);
	}

	return nRetCode;
}

void do_calc(CardMask plCards, CardMask oppCards, CardMask comCards) {
	CardMask		playerEvalCards, oppEvalCards;
	HandVal			player_hv, opp_hv;
	unsigned int	player_pokval, opp_pokval;

	CardMask_OR(playerEvalCards, plCards, comCards);
	CardMask_OR(oppEvalCards, oppCards, comCards);
	player_hv = Hand_EVAL_N(playerEvalCards, 7);
	opp_hv = Hand_EVAL_N(oppEvalCards, 7);
	player_pokval = calc_pokerval(player_hv, 7);
	opp_pokval = calc_pokerval(opp_hv, 7);

	if (player_pokval > opp_pokval)  win++;
	else if (player_pokval == opp_pokval)  tie++;
	else if (player_pokval < opp_pokval)  los++;
}

double calc_pokerval(HandVal hv, int n) {
	double		pv = 0;

	double	hicard			= 0x00000001,
			onepair			= 0x01000000,
			twopair			= 0x02000000,
			threeofakind	= 0x04000000,
			straight		= 0x08000000,
			flush			= 0x10000000,
			fullhouse		= 0x20000000,
			fourofakind		= 0x40000000,
			straightflush	= 0x80000000,
			royalflush		= 0x800EDCBA,
			fiveofakind		= 0xFF000000;

	switch HandVal_HANDTYPE(hv) {
	case HandType_STFLUSH:
		pv += straightflush;
		pv += (StdDeck_RANK(HandVal_TOP_CARD(hv))+2-0)<<16;
		pv += (StdDeck_RANK(HandVal_TOP_CARD(hv))+2-1)<<12;
		pv += (StdDeck_RANK(HandVal_TOP_CARD(hv))+2-2)<<8;
		pv += (StdDeck_RANK(HandVal_TOP_CARD(hv))+2-3)<<4;
		pv += (StdDeck_RANK(HandVal_TOP_CARD(hv))+2-4)<<0;
		break;
	case HandType_QUADS:
		pv += fourofakind;
		pv += (StdDeck_RANK(HandVal_TOP_CARD(hv))+2)<<16;
		pv += (StdDeck_RANK(HandVal_TOP_CARD(hv))+2)<<12;
		pv += (StdDeck_RANK(HandVal_TOP_CARD(hv))+2)<<8;
		pv += (StdDeck_RANK(HandVal_TOP_CARD(hv))+2)<<4;
		if (n>=5) { pv += (StdDeck_RANK(HandVal_SECOND_CARD(hv))+2)<<0; }
		break;
	case HandType_FULLHOUSE:
		pv += fullhouse;
		pv += (StdDeck_RANK(HandVal_TOP_CARD(hv))+2)<<16;
		pv += (StdDeck_RANK(HandVal_TOP_CARD(hv))+2)<<12;
		pv += (StdDeck_RANK(HandVal_TOP_CARD(hv))+2)<<8;
		pv += (StdDeck_RANK(HandVal_SECOND_CARD(hv))+2)<<4;
		pv += (StdDeck_RANK(HandVal_SECOND_CARD(hv))+2)<<0;
		break;
	case HandType_FLUSH:
		pv += flush;
		pv += (StdDeck_RANK(HandVal_TOP_CARD(hv))+2)<<16;
		pv += (StdDeck_RANK(HandVal_SECOND_CARD(hv))+2)<<12;
		pv += (StdDeck_RANK(HandVal_THIRD_CARD(hv))+2)<<8;
		pv += (StdDeck_RANK(HandVal_FOURTH_CARD(hv))+2)<<4;
		pv += (StdDeck_RANK(HandVal_FIFTH_CARD(hv))+2)<<0;
		break;
	case HandType_STRAIGHT:
		pv += straight;
		pv += (StdDeck_RANK(HandVal_TOP_CARD(hv))+2-0)<<16;
		pv += (StdDeck_RANK(HandVal_TOP_CARD(hv))+2-1)<<12;
		pv += (StdDeck_RANK(HandVal_TOP_CARD(hv))+2-2)<<8;
		pv += (StdDeck_RANK(HandVal_TOP_CARD(hv))+2-3)<<4;
		pv += (StdDeck_RANK(HandVal_TOP_CARD(hv))+2-4)<<0;
		break;
	case HandType_TRIPS:
		pv += threeofakind;
		pv += (StdDeck_RANK(HandVal_TOP_CARD(hv))+2)<<16;
		pv += (StdDeck_RANK(HandVal_TOP_CARD(hv))+2)<<12;
		pv += (StdDeck_RANK(HandVal_TOP_CARD(hv))+2)<<8;
		if (n>=4) { pv += (StdDeck_RANK(HandVal_SECOND_CARD(hv))+2)<<4; }
		if (n>=5) { pv += (StdDeck_RANK(HandVal_THIRD_CARD(hv))+2)<<0; }
		break;
	case HandType_TWOPAIR:
		pv += twopair;
		pv += (StdDeck_RANK(HandVal_TOP_CARD(hv))+2)<<16;
		pv += (StdDeck_RANK(HandVal_TOP_CARD(hv))+2)<<12;
		pv += (StdDeck_RANK(HandVal_SECOND_CARD(hv))+2)<<8;
		pv += (StdDeck_RANK(HandVal_SECOND_CARD(hv))+2)<<4;
		if (n>=5) { pv += (StdDeck_RANK(HandVal_THIRD_CARD(hv))+2)<<0; }
		break;
	case HandType_ONEPAIR:
		pv += onepair;
		pv += (StdDeck_RANK(HandVal_TOP_CARD(hv))+2)<<16;
		pv += (StdDeck_RANK(HandVal_TOP_CARD(hv))+2)<<12;
		if (n>=3) { pv += (StdDeck_RANK(HandVal_SECOND_CARD(hv))+2)<<8; }
		if (n>=4) { pv += (StdDeck_RANK(HandVal_THIRD_CARD(hv))+2)<<4; }
		if (n>=5) { pv += (StdDeck_RANK(HandVal_FOURTH_CARD(hv))+2)<<0; }
		break;
	case HandType_NOPAIR:
		if (n>=1) { pv += (StdDeck_RANK(HandVal_TOP_CARD(hv))+2)<<16; }
		if (n>=2) { pv += (StdDeck_RANK(HandVal_SECOND_CARD(hv))+2)<<12; }
		if (n>=3) { pv += (StdDeck_RANK(HandVal_THIRD_CARD(hv))+2)<<8; }
		if (n>=4) { pv += (StdDeck_RANK(HandVal_FOURTH_CARD(hv))+2)<<4; }
		if (n>=5) { pv += (StdDeck_RANK(HandVal_FIFTH_CARD(hv))+2)<<0; }
		break;
	}

	return pv;
}



