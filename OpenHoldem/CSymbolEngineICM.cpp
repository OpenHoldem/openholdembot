//******************************************************************************
//
// This file is part of the OpenHoldem project
//    Source code:           https://github.com/OpenHoldem/openholdembot/
//    Forums:                http://www.maxinmontreal.com/forums/index.php
//    Licensed under GPL v3: http://www.gnu.org/licenses/gpl.html
//
//******************************************************************************
//
// Purpose:
//
//******************************************************************************

/*
	   Author	:	spud
	   Date		:	2007-MAR-08
	  Notation: P(i,n)=probability that player i finishes in nth place
	  P(w|z)=probability that w occurs given that z occurs
	  I=i's stack
	  T=SUM[I]
	  P(i,1)=I/T
	  P(i,n)=SUM(j!=i)[P(j,1)*P(i,n|j,1)]
	  ICM assumes that P(i,n|j,1) = (P(i,n-1) in a game where j has been removed)
*/

#include "stdafx.h"
#include "CSymbolEngineICM.h"
#include "CEngineContainer.h"
#include "CFunctionCollection.h"

#include "CScraper.h"
#include "CSymbolEngineTableLimits.h"
#include "CTablemap/CTablemap.h"
#include "CSymbolEngineActiveDealtPlaying.h"
#include "CSymbolEngineBlinds.h"
#include "CSymbolEngineChipAmounts.h"
#include "CSymbolEngineDealerchair.h"
#include "CSymbolEngineUserchair.h"
#include "CSymbolEnginePokerAction.h"
#include "CTableState.h"


#include "WindowFunctions/window_functions.h"

CSymbolEngineICM::CSymbolEngineICM() {
  // The values of some symbol-engines depend on other engines.
  // As the engines get later called in the order of initialization
  // we assure correct ordering by checking if they are initialized.
  assert(p_engine_container->symbol_engine_active_dealt_playing() != NULL);
  assert(p_engine_container->symbol_engine_blinds() != NULL);
  assert(p_engine_container->symbol_engine_chip_amounts() != NULL);
  assert(p_engine_container->symbol_engine_dealerchair() != NULL);
  assert(p_engine_container->symbol_engine_userchair() != NULL);
}

CSymbolEngineICM::~CSymbolEngineICM() {
}

void CSymbolEngineICM::InitOnStartup() {
}

void CSymbolEngineICM::UpdateOnConnection() {
}

void CSymbolEngineICM::UpdateOnHandreset() {
}

void CSymbolEngineICM::UpdateOnNewRound() {
}

void CSymbolEngineICM::UpdateOnMyTurn() {
}

void CSymbolEngineICM::UpdateOnHeartbeat() {
}

double P(int i, int n, double *s, int N)
{
	double p = 0.;

	if (n==1)
	{
		double I = s[i];
		double T = 0;

		for (int k=0; k < N; k++) T+=s[k];

		p = (T > 0.) ? I / T : 0.;
	}
	else
	{
		for (int j=0; j < N; j++)
		{
			if (j != i)
			{
				double *ss = (double *) calloc(N-1,sizeof(double));
				int ii = (i > j) ? i-1 : i;
				memcpy(ss, s, j * sizeof(double));
				memcpy(ss + j, s + (j + 1), (N - j -1) * sizeof(double));

				p += P(j, 1, s, N) * P(ii, n-1, ss, N-1);

				free(ss);
			}
		}
	}
	return p;
}

int CSymbolEngineICM::GetChairFromDealPos(const char* name)
{
	int	sym_nplayersseated =	p_engine_container->symbol_engine_active_dealt_playing()->nplayersseated();
	int	sym_dealerchair =		p_engine_container->symbol_engine_dealerchair()->dealerchair();
	int nchairs =               p_tablemap->nchairs();
	int	chair = -1;

		if (strcmp(name,"SB")==0)
		{
			if (sym_nplayersseated==2)
			{
				chair = sym_dealerchair;
			}
			else if (sym_nplayersseated>=3)
			{
			for(int i =0; i<=nchairs; i++){
				if(p_engine_container->symbol_engine_poker_action()->DealPosition(i)==1)
					chair = i;
			}
		    }
		}
		else if (strcmp(name,"BB")==0)
		{
			if (sym_nplayersseated==2)
			{
			for(int i =0; i<=nchairs; i++){
				if(p_engine_container->symbol_engine_poker_action()->DealPosition(i)==1)
					chair = i;
			}
		    }
			else if (sym_nplayersseated>=3)
			{
			for(int i =0; i<=nchairs; i++){
				if(p_engine_container->symbol_engine_poker_action()->DealPosition(i)==2)
					chair = i;
			}
			}
		}
		
		else if ((strcmp(name,"UTG")==0) && (sym_nplayersseated >=10)){
			for(int i =0; i<=nchairs; i++){
				if(p_engine_container->symbol_engine_poker_action()->DealPosition(i)==(sym_nplayersseated-7))
					chair = i;
			}
		}
		else if ((strcmp(name,"UTG1")==0) && (sym_nplayersseated >=9))
		{
			for(int i =0; i<=nchairs; i++){
				if(p_engine_container->symbol_engine_poker_action()->DealPosition(i)==(sym_nplayersseated-6))
					chair = i;
			}
		}
		else if ((strcmp(name,"UTG2")==0) && (sym_nplayersseated >=8))
		{
			for(int i =0; i<=nchairs; i++){
				if(p_engine_container->symbol_engine_poker_action()->DealPosition(i)==(sym_nplayersseated-5))
					chair = i;
			}
		}
		else if ((strcmp(name,"UTG3")==0) && (sym_nplayersseated >=7))
		{
			for(int i =0; i<=nchairs; i++){
				if(p_engine_container->symbol_engine_poker_action()->DealPosition(i)==(sym_nplayersseated-4))
					chair = i;
			}
		}
		else if ((strcmp(name,"UTG4")==0) && (sym_nplayersseated >=6))
		{
			for(int i =0; i<=nchairs; i++){
				if(p_engine_container->symbol_engine_poker_action()->DealPosition(i)==(sym_nplayersseated-3))
					chair = i;
			}
		}
		else if ((strcmp(name,"UTG5")==0) && (sym_nplayersseated >=5))
		{
			for(int i =0; i<=nchairs; i++){
				if(p_engine_container->symbol_engine_poker_action()->DealPosition(i)==(sym_nplayersseated-2))
					chair = i;
			}
		}
		else if ((strcmp(name,"UTG6")==0) && (sym_nplayersseated >=4))
		{
			for(int i =0; i<=nchairs; i++){
				if(p_engine_container->symbol_engine_poker_action()->DealPosition(i)==(sym_nplayersseated-1))
					chair = i;
			}
		}
		else if (strcmp(name,"D")==0)
		{
                 chair = sym_dealerchair;
		}
		else if ((strcmp(name,"CO")==0) && (sym_nplayersseated >=4))
		{
			for(int i =0; i<=nchairs; i++){
				if(p_engine_container->symbol_engine_poker_action()->DealPosition(i)==(sym_nplayersseated-1))
					chair = i;
			}
		}

	return chair;

	}




double CSymbolEngineICM::EquityICM(double *stacks, double *prizes, int playerNB, int player)
{
	double ICM = 0.0;

	int			sym_opponentsseatedbits = p_engine_container->symbol_engine_active_dealt_playing()->opponentsseatedbits();

	for (int i=0; i<playerNB; ++i) {
    write_log(Preferences()->debug_icm(),
      "[CSymbolEngineICM] player %d  stack = %1.2f \n", i, stacks[i]);
	}

	// Degenerate case when the player's stack is zero.  Place
	// him after all opponents who had stacks at the start of this
	// hand.  This will not tie-break if two players bust on the
	// same hand.

	if (stacks[player] == 0.)
	{
		int place = 0;

		for (int i = 0; i < kMaxNumberOfPlayers; i++)
		{
			if (IsBitSet(sym_opponentsseatedbits, i))
			{
				place++;
			}
		}

		return prizes[place];

	}

	int i = 0;
	while (i < playerNB && prizes[i] > 0.) {
		double p = P(player, i + 1, stacks, playerNB);
		write_log(Preferences()->debug_icm(),
      "[CSymbolEngineICM] prob place %d = %1.4f \n", i + 1, p);
		ICM += prizes[i] * p;
		i++;
	}

	return ICM;
}

bool CSymbolEngineICM::EvaluateSymbol(const CString name, double *result, bool log /* = false */) {
  // Fast exit on other symbols
  int	sym_userchair = p_engine_container->symbol_engine_userchair()->userchair();
  if (memcmp(name, "icm", 3) != 0) return false;
	if (sym_userchair == kUndefined) {
		*result = 0.0;
    return true;
	}

  double		prizes[kMaxNumberOfPlayers] = {0};
  double		stacks[kMaxNumberOfPlayers] = {0};

  int number_of_icm_prizes = k_icm_prize9 - k_icm_prize1 + 1;
  double sum_of_prizes = 0.0;
  for (int i=0; i<number_of_icm_prizes; ++i) {
    int function_name_index = k_icm_prize1 + i;
    prizes[i] = p_function_collection->Evaluate(
      k_standard_function_names[function_name_index], Preferences()->log_icm_functions());
    sum_of_prizes += prizes[i];
  }
  // Sanity check: sum of prizes should be ~1.00
  if ((sum_of_prizes <= 0) || (sum_of_prizes > 1)) {
    CString error_message;
    error_message.Format("Incorrect f$icm_prizeX-functions.\n"
      "The sum of all values should be in the range (0..1].\n"
      "Current value: %.3f", sum_of_prizes);
    MessageBox_Error_Warning(error_message);
    *result = 0.0;
    return true;
  }

	int		sym_opponentsplayingbits = p_engine_container->symbol_engine_active_dealt_playing()->opponentsplayingbits();
	int		sym_nopponentsplaying = p_engine_container->symbol_engine_active_dealt_playing()->nopponentsplaying();
	int		sym_nplayersseated = p_engine_container->symbol_engine_active_dealt_playing()->nplayersseated();
	int		sym_playersseatedbits = p_engine_container->symbol_engine_active_dealt_playing()->playersseatedbits();
	double	sym_pot = p_engine_container->symbol_engine_chip_amounts()->pot();
	double	sym_call = p_engine_container->symbol_engine_chip_amounts()->call();
	double	sym_currentbet[kMaxNumberOfPlayers]={0};

	for (int i = 0; i < kMaxNumberOfPlayers; i++)
	{
		if (IsBitSet(sym_playersseatedbits, i))
		{
			stacks[i] = p_table_state->Player(i)->_balance.GetValue();
			sym_currentbet[i] = p_table_state->Player(i)->_bet.GetValue();
		}
	}

	if (strncmp(name,"icm_fold",8)==0)
	{
		double to_split = p_engine_container->symbol_engine_chip_amounts()->potcommon();

		for (int i = 0; i < kMaxNumberOfPlayers; i++)
		{
			if (IsBitSet(sym_opponentsplayingbits, i))
				stacks[i] += sym_currentbet[i];
			else
				to_split += sym_currentbet[i];
		}

		double win = to_split / sym_nopponentsplaying;

		for (int i = 0; i < kMaxNumberOfPlayers; i++)
		{
			if (IsBitSet(sym_opponentsplayingbits, i))
				stacks[i] += win;
		}
	}

	else if (strncmp(name,"icm_callwin",11)==0)
	{
		double call = sym_call;

		if (stacks[sym_userchair] < call)
		{
			double myTotalBet = sym_currentbet[sym_userchair] + stacks[sym_userchair];

			for (int i = 0; i < kMaxNumberOfPlayers; i++)
			{
				if (IsBitSet(sym_opponentsplayingbits, i) && 
					myTotalBet < sym_currentbet[i])
				{
					double extra = sym_currentbet[i] - myTotalBet;

					stacks[i] += extra;
					sym_pot -= extra;
				}
			}
		}
		stacks[sym_userchair] += sym_pot;
	}

	else if (strncmp(name,"icm_calllose",12)==0)
	{
		double mycall = min(sym_call,stacks[sym_userchair]);
		double win = (sym_pot + mycall) / sym_nopponentsplaying;

		stacks[sym_userchair] -= mycall;

		for (int i = 0; i < kMaxNumberOfPlayers; i++)
		{
			if (IsBitSet(sym_opponentsplayingbits, i))
			{
				stacks[i] += win;
			}
		}
	}

	else if (strncmp(name,"icm_calltie",11)==0)
	{
		double win = 0.;

		if (stacks[sym_userchair] < sym_call)
		{
			double myTotalBet = sym_currentbet[sym_userchair] + stacks[sym_userchair];

			for (int i = 0; i < kMaxNumberOfPlayers; i++)
			{
				if (IsBitSet(sym_opponentsplayingbits, i) 
					&& myTotalBet < sym_currentbet[i])
				{
					double extra = sym_currentbet[i] - myTotalBet;

					stacks[i] += extra;
					sym_pot -= extra;
				}
			}
		}
		sym_pot += min(sym_call, stacks[sym_userchair]);
		win = sym_pot / (sym_nopponentsplaying +1);
		stacks[sym_userchair] += win;
		for (int i = 0; i < kMaxNumberOfPlayers; i++)
		{
			if (IsBitSet(sym_opponentsplayingbits, i))
			{
				stacks[i] += win;
			}
		}
	}

	else if (strncmp(name,"icm_alliwin",11)==0)
	{
		if (isdigit(name[11]))
		{
			//assume callers are n smallest stacks
			bool callers[kMaxNumberOfPlayers] = {0};
			int ncallers = min(RightDigitCharacterToNumber(name), sym_nopponentsplaying);
      
      for (int i = 0; i < ncallers; i++)
			{
				int jsmallest = -1;
				double smalleststack = DBL_MAX;

				for (int j = 0; j < kMaxNumberOfPlayers; j++)
				{
					if (IsBitSet(sym_opponentsplayingbits, j))
					{
						if (!callers[j] && (sym_currentbet[j] + stacks[j]) < smalleststack)
						{
							smalleststack = sym_currentbet[j] + stacks[j];
							jsmallest = j;
						}
					}
				}
				if (jsmallest > -1)
				{
					double oppCurrentBet = sym_currentbet[jsmallest];
					double myTotalBet = sym_currentbet[sym_userchair] + stacks[sym_userchair];
					double extra = ((oppCurrentBet + stacks[jsmallest]) > myTotalBet) ? myTotalBet - oppCurrentBet : stacks[jsmallest];
					stacks[jsmallest] -= extra;
					callers[jsmallest] = true;
					sym_pot += extra;
				}
			}
			stacks[sym_userchair] += sym_pot;
		}
		else
		{
			//assume only one particular caller
			int oppChair = GetChairFromDealPos(name.Mid(11));
			if (oppChair == kUndefined || oppChair == sym_userchair) {
				*result = kUndefined;
				return true;
			}
			if (oppChair >= 0)
			{
				double oppCurrentBet = sym_currentbet[oppChair];
				double myTotalBet = sym_currentbet[sym_userchair] + stacks[sym_userchair];
				double extra = ((oppCurrentBet + stacks[oppChair]) > myTotalBet) ? myTotalBet - oppCurrentBet : stacks[oppChair];

				stacks[oppChair] -= extra;
				stacks[sym_userchair]+= (sym_pot + extra);
			}
		}
	}

	else if (strncmp(name,"icm_allilose",12)==0)
	{
		if (isdigit(name[12]))
		{
			//assume callers are n biggest stacks
			int ncallers = min(RightDigitCharacterToNumber(name), sym_nopponentsplaying);

			if (ncallers > 0)
			{
				bool callers[kMaxNumberOfPlayers] = {0};
				int *biggest =(int *) calloc(ncallers, sizeof(int));
				double *sidepots = (double *)calloc(ncallers, sizeof(double));
				double mybet = 0.;

				memset(sidepots,0, ncallers * sizeof(double));

				for (int i = 0; i < ncallers; i++)
				{
					int jbiggest = -1;
					double biggeststack = 0.;

					for (int j = 0; j < kMaxNumberOfPlayers; j++)
					{
						if (IsBitSet(sym_opponentsplayingbits, j))
						{
							if (!callers[j] && (sym_currentbet[j] + stacks[j]) > biggeststack)
							{
								biggeststack = sym_currentbet[j] + stacks[j];
								jbiggest = j;
							}
						}
					}
					biggest[i] = jbiggest;
					if (jbiggest > -1)
					{
						callers[jbiggest] = true;
						stacks[jbiggest] += sym_currentbet[jbiggest];
						sym_pot -= sym_currentbet[jbiggest];
					}
				}
				stacks[sym_userchair] += sym_currentbet[sym_userchair];
				sym_pot -= sym_currentbet[sym_userchair];
				for (int i = (ncallers -1); i>=0; i--)
				{
					ASSERT(biggest[i] >= 0);
					sidepots[i] = min(stacks[biggest[i]], stacks[sym_userchair]) - mybet;
					mybet += sidepots[i];
				}
				stacks[sym_userchair] -= mybet;
				sidepots[ncallers - 1] += sym_pot;
				for (int i = 0; i<ncallers; i++)
				{
					double win = sidepots[i] / (i + 1);

					for (int j = 0; j<=i; j++)
					{
						ASSERT(biggest[j] >= 0);
						stacks[biggest[j]] += win;
					}
				}
				free(biggest);
				free(sidepots);
			}
		}
		else
		{
			//assume only one particular caller
			int oppChair = GetChairFromDealPos(name.Mid(12));
			if (oppChair == kUndefined || oppChair == sym_userchair) {
				*result = kUndefined;
				return true;
			}
			if (oppChair >= 0)
			{
				double oppTotalBet = sym_currentbet[oppChair] + stacks[oppChair];
				double myTotalBet = sym_currentbet[sym_userchair] + stacks[sym_userchair];
				double extra = (oppTotalBet > myTotalBet) ? stacks[sym_userchair] : oppTotalBet - sym_currentbet[sym_userchair];

				stacks[oppChair] += (sym_pot + extra);
				stacks[sym_userchair] -= extra;
			}
		}
	}

	else if (strncmp(name,"icm_allitie",11)==0)
	{
		//assume only one particular caller
		int oppChair = GetChairFromDealPos(name.Mid(11));
		if (oppChair == kUndefined || oppChair == sym_userchair) {
			*result = kUndefined;
			return true;
		}
		if (oppChair >= 0)
		{
			stacks[oppChair] += sym_pot / 2;
			stacks[sym_userchair] += sym_pot / 2;
		}
	}

	else if(strcmp(name,"icm")==0)
	{
		for (int i = 0; i < kMaxNumberOfPlayers; i++)
		{
			if (IsBitSet(sym_playersseatedbits, i))
			{
				stacks[i] = p_engine_container->symbol_engine_chip_amounts()->stacks_at_hand_start(i);
			}
	   }
	}

	else
	{
		// Not a valid symbol
		return false;
	} 

	*result = EquityICM(stacks, prizes, kMaxNumberOfPlayers, sym_userchair);
	return true;
}

CString CSymbolEngineICM::SymbolsProvided() {
  //RangeOfSymbols(CString format_string, int first, int last);
  CString result = "icm icm_fold icm_callwin icm_calllose icm_calltie ";
  // Winning against 0..9 opponents
  result += RangeOfSymbols("icm_alliwin%d", 0, 9);
  // Losing against 1..9 opponents
  result += RangeOfSymbols("icm_allilose%d", 0, 9);
  // Wining allin against a particular opponent
  result += "icm_alliwinSB icm_alliwinBB icm_alliwinUTG icm_alliwinUTG1 "
    "icm_alliwinUTG2 icm_alliwinUTG3 icm_alliwinUTG4 icm_alliwinUTG5 "
    "icm_alliwinUTG6 icm_alliwinCO icm_alliwinD "
    // Tieing allin against a particular opponent
    "icm_allitieSB icm_allitieBB icm_allitieUTG icm_allitieUTG1 "
    "icm_allitieUTG2 icm_allitieUTG3 icm_allitieUTG4 icm_allitieUTG5 "
    "icm_allitieUTG6 icm_allitieCO icm_allitieD "
    // Losing allin against a particular opponent
    "icm_alliloseSB icm_alliloseBB icm_alliloseUTG icm_alliloseUTG1 "
    "icm_alliloseUTG2 icm_alliloseUTG3 icm_alliloseUTG4 icm_alliloseUTG5 "
    "icm_alliloseUTG6 icm_alliloseCO icm_alliloseD ";
  return result;
}