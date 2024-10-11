//******************************************************************************
//
// This file is part of the OpenHoldem project
//    Source code:           https://github.com/OpenHoldem/openholdembot/
//    Forums:                http://www.maxinmontreal.com/forums/index.php
//    Licensed under GPL v3: http://www.gnu.org/licenses/gpl.html
//
//******************************************************************************
//
// Purpose: PrWin-simulation, HoldEm only, not Omaha#include "CSymbolEngineIsOmaha.h"
//
//******************************************************************************

#include "stdafx.h"
#include "CSymbolEnginePrwin.h"

#include <assert.h>
#include <math.h>
#include "CEngineContainer.h"
#include "CFunctionCollection.h"
#include "CIteratorThread.h"

#include "CScraper.h"
#include "CSymbolEngineAutoplayer.h"
#include "CSymbolEngineIsOmaha.h"
#include "CSymbolenginePokerval.h"
#include "CSymbolEngineRange.h"
#include "CSymbolEngineDealerchair.h"
#include "CSymbolengineChairs.h"
#include "CTableState.h"

sprw1326*	pprw1326;	//prwin 1326 data structure Matrix 2008-04-29

CSymbolEnginePrwin::CSymbolEnginePrwin() {
	// The values of some symbol-engines depend on other engines.
	// As the engines get later called in the order of initialization
	// we assure correct ordering by checking if they are initialized.
  assert(p_engine_container->symbol_engine_isomaha() != NULL);
	assert(p_engine_container->symbol_engine_pokerval() != NULL);
	assert(p_engine_container->symbol_engine_tablelimits() != NULL);
	assert(p_engine_container->symbol_engine_userchair() != NULL);
  // Initializations, needed early e.g. by CSymbolEngineHandrank
  _nhandshi = 0;
  _nhandslo = 0;
  _nhandsti = 0;
  _prwinnow = 0;
  _prlosnow = 0;
  _nopponents_for_prwin = 0;
  _known_change_in_gamestate_since_last_prwin_calculation = false;
  _prwin_recalc = false;
  _prwin_clearall = false;
  pprw1326 = p_iterator_thread->prw1326();
}

CSymbolEnginePrwin::~CSymbolEnginePrwin() {
}

void CSymbolEnginePrwin::InitOnStartup() {
	UpdateOnConnection();
}

void CSymbolEnginePrwin::UpdateOnConnection() {
	UpdateOnHandreset();
}

void CSymbolEnginePrwin::UpdateOnHandreset() {
	UpdateOnNewRound();
}

void CSymbolEnginePrwin::UpdateOnNewRound() {
	_nhandshi = 0;
	_nhandslo = 0;
	_nhandsti = 0;
	_prwinnow = 0;
	_prlosnow = 0;
  _known_change_in_gamestate_since_last_prwin_calculation = true;
  _prwin_recalc = false;
  _prwin_clearall = false;
}

void CSymbolEnginePrwin::UpdateOnMyTurn() {
  if (StartOfPrWinComputationsNeeded()) {
    assert(p_iterator_thread != NULL);
    p_iterator_thread->RestartPrWinComputations();
    _known_change_in_gamestate_since_last_prwin_calculation = false;
  }
	CalculateNOpponents();
	CalculateNhands();
	_prwin_recalc = false;
	_prwin_clearall = false;
}

void CSymbolEnginePrwin::UpdateOnHeartbeat() {
}

void CSymbolEnginePrwin::UpdateAfterAutoplayerAction(int autoplayer_action_code) {
  _known_change_in_gamestate_since_last_prwin_calculation = true;
}

bool CSymbolEnginePrwin::StartOfPrWinComputationsNeeded() {
  if (_known_change_in_gamestate_since_last_prwin_calculation) {
    return true;
  }
  if (p_engine_container->symbol_engine_autoplayer()->IsFirstHeartbeatOfMyTurn()) {
    return true;
  }
  return false;
}

void CSymbolEnginePrwin::CalculateNhands() {
	CardMask		plCards = {0}, comCards = {0}, oppCards = {0}, playerEvalCards = {0}, opponentEvalCards = {0};
	HandVal			hv_player = 0, hv_opponent = 0;
	unsigned int	pl_pokval = 0, opp_pokval = 0;
	int				dummy = 0;
	int				nplCards, ncomCards;

	_nhandshi = 0;
	_nhandsti = 0;
	_nhandslo = 0;

	// player cards
	CardMask_RESET(plCards);
	nplCards = 0;
	for (int i=0; i<NumberOfCardsPerPlayer(); i++) {
    Card* card = p_table_state->User()->hole_cards(i);
    if (card->IsKnownCard()) {
      CardMask_SET(plCards, card->GetValue());
			nplCards++;
		}
	}
  // common cards
	CardMask_RESET(comCards);
	ncomCards = 0;
	for (int i=0; i<kNumberOfCommunityCards; i++) {
    Card *card = p_table_state->CommonCards(i);
    if (card->IsKnownCard()) {
      CardMask_SET(comCards, card->GetValue());
			ncomCards++;
		}
	}
  // player/common cards and pokerval
	CardMask_OR(playerEvalCards, plCards, comCards);
	hv_player = Hand_EVAL_N(playerEvalCards, nplCards+ncomCards);
	pl_pokval = p_engine_container->symbol_engine_pokerval()->CalculatePokerval(hv_player, 
		nplCards+ncomCards, &dummy, CARD_NOCARD, CARD_NOCARD);
	for (int i=0; i<(kNumberOfCardsPerDeck-1); i++) {
		for (int j=(i+1); j<kNumberOfCardsPerDeck; j++)	{
			if (!CardMask_CARD_IS_SET(plCards, i) 
				  && !CardMask_CARD_IS_SET(plCards, j) 
				  && !CardMask_CARD_IS_SET(comCards, i) 
				  && !CardMask_CARD_IS_SET(comCards, j)) {
				// opponent cards
				CardMask_RESET(oppCards);
				CardMask_SET(oppCards, i);
				CardMask_SET(oppCards, j);
        CardMask_OR(opponentEvalCards, oppCards, comCards);
				hv_opponent = Hand_EVAL_N(opponentEvalCards, 2+ncomCards);
				opp_pokval = p_engine_container->symbol_engine_pokerval()->CalculatePokerval(hv_opponent,
					(NumberOfCardsPerPlayer() + ncomCards), 
					&dummy, CARD_NOCARD, CARD_NOCARD);

				if (pl_pokval > opp_pokval)
				{
					_nhandslo++;
				}
				else if (pl_pokval < opp_pokval)
				{
					_nhandshi++;
				}
				else
				{
					_nhandsti++;
				}
			}
		}
	}
  AssertRange(_nhandshi, 0, nhands());
	AssertRange(_nhandsti, 0, nhands());
	AssertRange(_nhandslo, 0, nhands());
	assert((_nhandshi + _nhandsti + _nhandslo) == nhands());
  _prwinnow = pow(((double)_nhandslo/nhands()), _nopponents_for_prwin);
	_prlosnow = 1 - pow((((double)_nhandslo + _nhandsti)/nhands()), _nopponents_for_prwin);

	AssertRange(_prwinnow, 0, 1);
	AssertRange(_prlosnow, 0, 1);
}

void CSymbolEnginePrwin::CalculateNOpponents() {
	_nopponents_for_prwin = p_function_collection->Evaluate(
		"f$prwin_number_of_opponents", Preferences()->log_prwin_functions());
	if (_nopponents_for_prwin > MAX_OPPONENTS) {
		_nopponents_for_prwin = MAX_OPPONENTS;
	}
	if (_nopponents_for_prwin < 1)	{
		_nopponents_for_prwin = 1;
	}
}

int CSymbolEnginePrwin::GetChair(CString chair) {
	double result = 0;

	if (chair == "userchair")
		return p_engine_container->symbol_engine_userchair()->userchair();
	if (chair == "dealerchair" || chair == "buttonchair")
		return p_engine_container->symbol_engine_dealerchair()->dealerchair();
	if (p_engine_container->symbol_engine_chairs()->EvaluateSymbol(chair, &result))
		return (int)result;
	return kUndefined;
}

void CSymbolEnginePrwin::LogHandRank() {
	write_log(k_always_log_errors, "%s\n", "********* Hand Rank Info For Enhanced Prwin *******");
	write_log(k_always_log_errors, "%s%d\n", "Dealer Chair is   -- ", GetChair("dealerchair"));
	// Look at 1326 from a 169 perspective
	int lowrank;
	int hirank;
	int oldlowrank = 0;
	int oldhirank = 0;
	char lowcard;
	char hicard;
	int s1, s2;
	char ssuit;
	char oldssuit;
	char *comsuit[4]{ "h","d","c","s" };
	CString test;
	CString combos;

	for (int i = 0; i < 10; i++) {

		int handct = 0;
		test = '\0';
		combos = '\0';

		write_log(k_always_log_errors, "%s%i%s\n", "********* -- Hand Rank Info For Chair ", i, " -- *******");
		write_log(k_always_log_errors, "%s%i%s%i\n", "Hand Rank Chair ", i, " -- Combos ", pprw1326->chair[i].limit);
		write_log(k_always_log_errors, "%s%i%s%i\n", "Hand Rank Chair ", i, " -- Level  ", pprw1326->chair[i].level);

		for (int ia = 0; ia < 1326; ia++) {

			if (pprw1326->chair[i].ranklo[ia] >= 39) {
				lowrank = pprw1326->chair[i].ranklo[ia] - 39;
				s1 = 3;
			}
			else if (pprw1326->chair[i].ranklo[ia] >= 26) {
				lowrank = pprw1326->chair[i].ranklo[ia] - 26;
				s1 = 2;
			}
			else if (pprw1326->chair[i].ranklo[ia] >= 13) {
				lowrank = pprw1326->chair[i].ranklo[ia] - 13;
				s1 = 1;
			}
			else {
				lowrank = pprw1326->chair[i].ranklo[ia];
				s1 = 0;
			}

			if (pprw1326->chair[i].rankhi[ia] >= 39) {
				hirank = pprw1326->chair[i].rankhi[ia] - 39;
				s2 = 3;
			}
			else if (pprw1326->chair[i].rankhi[ia] >= 26) {
				hirank = pprw1326->chair[i].rankhi[ia] - 26;
				s2 = 2;
			}
			else if (pprw1326->chair[i].rankhi[ia] >= 13) {
				hirank = pprw1326->chair[i].rankhi[ia] - 13;
				s2 = 1;
			}
			else {
				hirank = pprw1326->chair[i].rankhi[ia];
				s2 = 0;
			}

			if (s1 == s2) { ssuit = 's'; }
			else if (hirank == lowrank) { ssuit = ' '; }
			else { ssuit = 'o'; }

			if (s1 == s2 && hirank == lowrank) { break; }

			//if (oldhirank == hirank && oldlowrank == lowrank && oldssuit == ssuit) {
			//continue;
			//} 
			if (oldhirank == hirank && oldlowrank == lowrank && oldssuit == ssuit) {
				handct = handct;
			}
			else handct = handct++;

			oldhirank = hirank;
			oldlowrank = lowrank;
			oldssuit = ssuit;
			char *cardsrank[13] = { "2","3","4","5","6","7","8","9","T","J","Q","K","A" };

			for (int idi = 0; idi < 13; idi++) {
				if (lowrank == idi) { lowcard = *cardsrank[idi]; }
				if (hirank == idi) { hicard = *cardsrank[idi]; }
			}

			//write_log(k_always_log_errors, "%s%i%s%i%s%c%c%c%s%i\n", "Hand Rank Chair ", i, " -- Hand (", handct, ") ", hicard, lowcard, ssuit, " Weight  ", pprw1326->chair[i].weight[ia]);

			write_log(k_always_log_errors, "%s%i%s%i%s%c%c%c%c%s%i\n", "Hand Rank Chair ", i, " -- Hand (", handct, ") ", hicard, *comsuit[s2], lowcard, *comsuit[s1], " -- Weight  ", pprw1326->chair[i].weight[ia]);
			test += hicard;
			test += lowcard;
			if (ssuit != ' ') {
				test += ssuit;
			}
			test += ',';

			combos += hicard;
			combos += *comsuit[s2];
			combos += lowcard;
			combos += *comsuit[s1];
			combos += ',';
		}
		//write_log(k_always_log_errors, "%s%i%s%s\n", "Hand Rank List Chair ", i, " - ", test);
		write_log(k_always_log_errors, "%s%i%s%s\n", "Hand Rank List Chair ", i, " - ", combos);
		write_log(k_always_log_errors, "%s%i%s%i\n", "Hand Rank Chair ", i, " -- Total Hand Count ", handct);
		write_log(k_always_log_errors, "%s%i%s%i\n", "Hand Rank Chair ", i, " -- Chair Limit ", pprw1326->chair[i].limit);
	}
	write_log(k_always_log_errors, "%s\n", "*************** Hand Rank Info Summary ****************");
	if (GetChair("ep1chair") >= 0) {
		if (GetChair("ep1chair") != GetChair("userchair")) {
			write_log(k_always_log_errors, "%s%i%s%d%s\n", "EP1         -- Combos  ", pprw1326->chair[GetChair("ep1chair")].limit, "  -- Percentage ", (((pprw1326->chair[GetChair("ep1chair")].limit) * 100) / 1326), "%  ");
			if (pprw1326->chair[GetChair("ep1chair")].limit == 0 || !p_table_state->Player(GetChair("ep1chair"))->HasAnyCards()) { write_log(k_always_log_errors, "%s\n", "EP1         -- Folded or Sit-Out or Not evaluated"); }
		}
	}
	else { write_log(k_always_log_errors, "%s\n", "EP1         -- not playing..."); }
	if (GetChair("ep2chair") >= 0) {
		if (GetChair("ep2chair") != GetChair("userchair")) {
			write_log(k_always_log_errors, "%s%i%s%d%s\n", "EP2         -- Combos  ", pprw1326->chair[GetChair("ep2chair")].limit, "  -- Percentage ", (((pprw1326->chair[GetChair("ep2chair")].limit) * 100) / 1326), "%  ");
			if (pprw1326->chair[GetChair("ep2chair")].limit == 0 || !p_table_state->Player(GetChair("ep2chair"))->HasAnyCards()) { write_log(k_always_log_errors, "%s\n", "EP2         -- Folded or Sit-Out or Not evaluated"); }
		}
	}
	else { write_log(k_always_log_errors, "%s\n", "EP2         -- not playing..."); }
	if (GetChair("ep3chair") >= 0) {
		if (GetChair("ep3chair") != GetChair("userchair")) {
			write_log(k_always_log_errors, "%s%i%s%d%s\n", "EP3         -- Combos  ", pprw1326->chair[GetChair("ep3chair")].limit, "  -- Percentage ", (((pprw1326->chair[GetChair("ep3chair")].limit) * 100) / 1326), "%  ");
			if (pprw1326->chair[GetChair("ep3chair")].limit == 0 || !p_table_state->Player(GetChair("ep3chair"))->HasAnyCards()) { write_log(k_always_log_errors, "%s\n", "EP3         -- Folded or Sit-Out or Not evaluated"); }
		}
	}
	else { write_log(k_always_log_errors, "%s\n", "EP3         -- not playing..."); }
	if (GetChair("mp1chair") >= 0) {
		if (GetChair("mp1chair") != GetChair("userchair")) {
			write_log(k_always_log_errors, "%s%i%s%d%s\n", "MP1         -- Combos  ", pprw1326->chair[GetChair("mp1chair")].limit, "  -- Percentage ", (((pprw1326->chair[GetChair("mp1chair")].limit) * 100) / 1326), "%  ");
			if (pprw1326->chair[GetChair("mp1chair")].limit == 0 || !p_table_state->Player(GetChair("mp1chair"))->HasAnyCards()) { write_log(k_always_log_errors, "%s\n", "MP1         -- Folded or Sit-Out or Not evaluated"); }
		}
	}
	else { write_log(k_always_log_errors, "%s\n", "MP1         -- not playing..."); }
	if (GetChair("mp2chair") >= 0) {
		if (GetChair("mp2chair") != GetChair("userchair")) {
			write_log(k_always_log_errors, "%s%i%s%d%s\n", "MP2         -- Combos  ", pprw1326->chair[GetChair("mp2chair")].limit, "  -- Percentage ", (((pprw1326->chair[GetChair("mp2chair")].limit) * 100) / 1326), "%  ");
			if (pprw1326->chair[GetChair("mp2chair")].limit == 0 || !p_table_state->Player(GetChair("mp2chair"))->HasAnyCards()) { write_log(k_always_log_errors, "%s\n", "MP2         -- Folded or Sit-Out or Not evaluated"); }
		}
	}
	else { write_log(k_always_log_errors, "%s\n", "MP2         -- not playing..."); }
	if (GetChair("mp3chair") >= 0) {
		if (GetChair("mp3chair") != GetChair("userchair")) {
			write_log(k_always_log_errors, "%s%i%s%d%s\n", "MP3         -- Combos  ", pprw1326->chair[GetChair("mp3chair")].limit, "  -- Percentage ", (((pprw1326->chair[GetChair("mp3chair")].limit) * 100) / 1326), "%  ");
			if (pprw1326->chair[GetChair("mp3chair")].limit == 0 || !p_table_state->Player(GetChair("mp3chair"))->HasAnyCards()) { write_log(k_always_log_errors, "%s\n", "MP3         -- Folded or Sit-Out or Not evaluated"); }
		}
	}
	else { write_log(k_always_log_errors, "%s\n", "MP3         -- not playing..."); }
	if (GetChair("cutoffchair") >= 0) {
		if (GetChair("cutoffchair") != GetChair("userchair")) {
			write_log(k_always_log_errors, "%s%i%s%d%s\n", "Cut Off     -- Combos  ", pprw1326->chair[GetChair("cutoffchair")].limit, "  -- Percentage ", (((pprw1326->chair[GetChair("cutoffchair")].limit) * 100) / 1326), "%  ");
			if (pprw1326->chair[GetChair("cutoffchair")].limit == 0 || !p_table_state->Player(GetChair("cutoffchair"))->HasAnyCards()) { write_log(k_always_log_errors, "%s\n", "Cut Off     -- Folded or Sit-Out or Not evaluated"); }
		}
	}
	else { write_log(k_always_log_errors, "%s\n", "Cut Off     -- not playing..."); }
	if (GetChair("dealerchair") >= 0) {
		if (GetChair("dealerchair") != GetChair("userchair")) {
			write_log(k_always_log_errors, "%s%i%s%d%s\n", "Button      -- Combos  ", pprw1326->chair[GetChair("buttonchair")].limit, "  -- Percentage ", (((pprw1326->chair[GetChair("dealerchair")].limit) * 100) / 1326), "%  ");
			if (pprw1326->chair[GetChair("dealerchair")].limit == 0 || !p_table_state->Player(GetChair("dealerchair"))->HasAnyCards()) { write_log(k_always_log_errors, "%s\n", "Button      -- Folded or Sit-Out or Not evaluated"); }
		}
	}
	else { write_log(k_always_log_errors, "%s\n", "Button      -- not playing..."); }
	if (GetChair("smallblindchair") >= 0) {
		if (GetChair("smallblindchair") != GetChair("userchair")) {
			write_log(k_always_log_errors, "%s%i%s%d%s\n", "Small Blind -- Combos  ", pprw1326->chair[GetChair("smallblindchair")].limit, "  -- Percentage ", (((pprw1326->chair[GetChair("smallblindchair")].limit) * 100) / 1326), "%  ");
			if (pprw1326->chair[GetChair("smallblindchair")].limit == 0 || !p_table_state->Player(GetChair("smallblindchair"))->HasAnyCards()) { write_log(k_always_log_errors, "%s\n", "Small Blind -- Folded or Sit-Out or Not evaluated"); }
		}
	}
	else { write_log(k_always_log_errors, "%s\n", "Small Blind -- not playing..."); }
	if (GetChair("bigblindchair") >= 0) {
		if (GetChair("bigblindchair") != GetChair("userchair")) {
			write_log(k_always_log_errors, "%s%i%s%d%s\n", "Big Blind   -- Combos  ", pprw1326->chair[GetChair("bigblindchair")].limit, "  -- Percentage ", (((pprw1326->chair[GetChair("bigblindchair")].limit) * 100) / 1326), "%  ");
			if (pprw1326->chair[GetChair("bigblindchair")].limit == 0 || !p_table_state->Player(GetChair("bigblindchair"))->HasAnyCards()) { write_log(k_always_log_errors, "%s\n", "Big Blind   -- Folded or Sit-Out or Not evaluated"); }
		}
	}
	else { write_log(k_always_log_errors, "%s\n", "Big Blind   -- not playing..."); }
	write_log(k_always_log_errors, "%s\n\n", "**************** Calculated PrWin1326 *****************");
	write_log(k_always_log_errors, "%s%.2f%s%.2f%s%.2f%s\n\n", "% Probabilities  -- Prwin: ", p_iterator_thread->prwin()*100, "%  Prtie: ", p_iterator_thread->prtie()*100, "%  Prlos: ", p_iterator_thread->prlos()*100, "%");
	write_log(k_always_log_errors, "%s%.2f%s%.2f%s\n\n", "% Equities       -- Eqwin: ", p_iterator_thread->prwin() * 100 + (p_iterator_thread->prtie() * 100)/2, "%  Eqtie: ", (p_iterator_thread->prtie() * 100)/2, "%");
	write_log(k_always_log_errors, "%s\n", "************** END Hand Rank Info Summary ***************");
}

void CSymbolEnginePrwin::UnSetLists() {
	for (int i = 0; i < 10; i++) {
		pprw1326->chair[i].limit = 0;
		pprw1326->chair[i].level = 100;
		pprw1326->chair[i].ignore = 0;

		for (int ia = 0; ia < 1326; ia++) {
			pprw1326->chair[i].ranklo[ia] = 0;
			pprw1326->chair[i].rankhi[ia] = 0;
			pprw1326->chair[i].weight[ia] = 100;
		}
	}
}

void CSymbolEnginePrwin::SetPrw1326(CString prw1326_symbol, CString prw1326_command) {
	if (prw1326_symbol == "prw1326_useme") {
		//p_engine_container->symbol_engine_range()->ClearAll();
		//UnSetLists();
		if (prw1326_command == "true") 
			pprw1326->useme = 1326;
		else pprw1326->useme = 0;
	}
	if (prw1326_symbol == "prw1326_usepreflop") {
		if (prw1326_command == "true")
			pprw1326->preflop = 1326;
		else pprw1326->preflop = 0;
	}
	if (prw1326_symbol == "prw1326_usecallback") {
		if (prw1326_command == "true")
			pprw1326->usecallback = 1326;
		else pprw1326->usecallback = 0;
	}
	if (prw1326_symbol == "prw1326_cmd") {
		if (prw1326_command == "recalc")
			_prwin_recalc = true;
		if (prw1326_command == "clearall")
			_prwin_clearall = true;
	}
}

bool CSymbolEnginePrwin::EvaluateSymbol(const CString name, double *result, bool log /* = false */) {
  FAST_EXIT_ON_OPENPPL_SYMBOLS(name);
	if (memcmp(name, "pr", 2)==0) {
    if (memcmp(name, "prwin", 5)==0 && strlen(name)==5) {
      *result = p_iterator_thread->prwin();
    } else if (memcmp(name, "prlos", 5)==0 && strlen(name)==5) {
      *result = p_iterator_thread->prlos();
    } else if (memcmp(name, "prtie", 5)==0 && strlen(name)==5) {
      *result = p_iterator_thread->prtie();
	} else if (memcmp(name, "prw1326_useme", 13) == 0 && strlen(name) == 13) {
		*result = p_iterator_thread->prw1326()->useme;
	} else if (memcmp(name, "prw1326_usepreflop", 18) == 0 && strlen(name) == 18) {
		*result = p_iterator_thread->prw1326()->preflop;
	} else if (memcmp(name, "prw1326_usecallback", 19) == 0 && strlen(name) == 19) {
		*result = p_iterator_thread->prw1326()->usecallback;
    } else if (memcmp(name, "prw1326_cmd", 11) == 0 && strlen(name) == 11) {
		if (_prwin_recalc)		// Result: 1 = recalc command - 2 = clearall command
			*result = 1;
		if (_prwin_clearall)
			*result = 2;
	}
	else if (memcmp(name, "prwinnow", 8)==0 && strlen(name)==8) {
			*result = prwinnow();
		}	else if (memcmp(name, "prlosnow", 8)==0 && strlen(name)==8)	{
			*result = prlosnow();
		} else {
      return false;
    }
    // Valid symbol
    return true;
  }	else if (memcmp(name, "nhands", 6)==0) {
		if (memcmp(name, "nhands", 6)==0 && strlen(name)==6)	{
			*result = nhands();
		}	else if (memcmp(name, "nhandshi", 8)==0 && strlen(name)==8)	{
			*result = nhandshi();
		}	else if (memcmp(name, "nhandslo", 8)==0 && strlen(name)==8)	{
			*result = nhandslo();
		}	else if (memcmp(name, "nhandsti", 8)==0 && strlen(name)==8)	{
			*result = nhandsti();
		}	else {
			// Invalid symbol
			return false;
		}
		// Valid symbol
		return true;
	}
	// Symbol of a different symbol-engine
	return false;
}

CString CSymbolEnginePrwin::SymbolsProvided() {
  return "prwinnow prlosnow "
    "prwin prlos prtie "
	"prw1326_useme prw1326_usepreflop prw1326_usecallback prw1326_cmd "
    "nhands nhandshi nhandslo nhandsti ";
}