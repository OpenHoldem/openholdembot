//******************************************************************************
//
// This file is part of the OpenHoldem project
//    Source code:           https://github.com/OpenHoldem/openholdembot/
//    Forums:                http://www.maxinmontreal.com/forums/index.php
//    Licensed under GPL v3: http://www.gnu.org/licenses/gpl.html
//
//******************************************************************************
//
// Purpose: PrWin-simulation, HoldEm only, not Omaha
//
//******************************************************************************

#include "stdafx.h"
#include "CIteratorThread.h"

#include <process.h>
#include "CBetroundCalculator.h"
#include "CEngineContainer.h"
#include "CFunctionCollection.h"
#include "COpenHoldemStatusbar.h"
#include "CWhiteInfoBox.h"
#include "RtaWindow.h"

#include "CScraper.h"
#include "CSymbolEngineActiveDealtPlaying.h"
#include "CSymbolEngineAutoplayer.h"
#include "CSymbolEngineBlinds.h"
#include "CSymbolEngineChipAmounts.h"
#include "CSymbolEngineHistory.h"
#include "CSymbolEngineIsOmaha.h"
#include "CSymbolEnginePokerval.h"
#include "CSymbolEnginePrwin.h"
#include "CTableState.h"
#include "CValidator.h"
#include "inlines/eval.h"


#include "PrWinHandranges.h"
#include "mtrand.h"

CIteratorThread	*p_iterator_thread = NULL;
HANDLE CIteratorThread::_m_stop_thread; //!!

// Static variables
int CIteratorThread::_iterations_calculated;
int CIteratorThread::_iterations_required;
int CIteratorThread::_nopponents;
//int CIteratorThread::_generated_numbers[1326];
double CIteratorThread::_prwin;
double CIteratorThread::_prtie;
double CIteratorThread::_prlos;
double CIteratorThread::_nouts;
double CIteratorThread::_outodds;
double CIteratorThread::_impliedodds;
double CIteratorThread::_potodds;
double CIteratorThread::_myequity;
double CIteratorThread::_potequity;
double CIteratorThread::_prwin_mustplay;
int CIteratorThread::_total_weight[kMaxNumberOfPlayers];

// weighted prwin lookup tables for non-suited and suited cards
int pair2rank_offsuited[170] = {0}, pair2rank_suited[170] = {0};
// int willplay = 0, wontplay = 0, topclip = 0, mustplay = 0
sprw1326	_prw1326;	//prwin 1326 data structure Matrix 2008-04-29


// handrank table used to prime weighted prwin lookup table.
// This is the Sklansky-Chubukov hand rankings.
// left in this form for ease of developer modification.
// converted at startup to the tables actually used by prwin calculation
char *prwin_handrank_table_169[kNumberOfStartingHands] =
{
	"AA ","KK ","AKs","QQ ","AK ","JJ ","AQs","TT ","AQ ","99 ",
	"AJs","88 ","ATs","AJ ","77 ","66 ","AT ","A9s","55 ","A8s",
	"KQs","44 ","A9 ","A7s","KJs","A5s","A8 ","A6s","A4s","33 ",
	"KTs","A7 ","A3s","KQ ","A2s","A5 ","A6 ","A4 ","KJ ","QJs",
	"A3 ","22 ","K9s","A2 ","KT ","QTs","K8s","K7s","JTs","K9 ",
	"K6s","QJ ","Q9s","K5s","K8 ","K4s","QT ","K7 ","K3s","K2s",
	"Q8s","K6 ","J9s","K5 ","Q9 ","JT ","K4 ","Q7s","T9s","Q6s",
	"K3 ","J8s","Q5s","K2 ","Q8 ","Q4s","J9 ","Q3s","T8s","J7s",
	"Q7 ","Q2s","Q6 ","98s","Q5 ","J8 ","T9 ","J6s","T7s","J5s",
	"Q4 ","J4s","J7 ","Q3 ","97s","T8 ","J3s","T6s","Q2 ","J2s",
	"87s","J6 ","98 ","T7 ","96s","J5 ","T5s","T4s","86s","J4 ",
	"T6 ","97 ","T3s","76s","95s","J3 ","T2s","87 ","85s","96 ",
	"T5 ","J2 ","75s","94s","T4 ","65s","86 ","93s","84s","95 ",
	"T3 ","76 ","92s","74s","54s","T2 ","85 ","64s","83s","94 ",
	"75 ","82s","73s","93 ","65 ","53s","63s","84 ","92 ","43s",
	"74 ","72s","54 ","64 ","52s","62s","83 ","42s","82 ","73 ",
	"53 ","63 ","32s","43 ","72 ","52 ","62 ","42 ","32 "
};

// Make some variables global
// because they need to be accessed and shared 
// by both class functions and static iterator-functions
// and because the former "solution" of passing class pointers 
// to static iterator-functions was no real option either. ;-(
int				   deck[kNumberOfCardsPerDeck];
CardMask		 usedCards,temp_usedCards;
// The PrWin-calculations support HoldEm only, not Omaha
unsigned int ocard[MAX_OPPONENTS*kNumberOfCardsPerPlayerHoldEm];
CardMask     addlcomCards;
CardMask     evalCards = {0}, opp_evalCards = {0};
int          _willplay, _wontplay, _mustplay, _topclip;
int          _nplCards, _ncomCards;
double       _win, _tie, _los;

CIteratorThread::CIteratorThread() {
	write_log(Preferences()->debug_prwin(), "[PrWinThread] Iterator Thread starting.\n");

	// Create events
	_m_stop_thread = CreateEvent(0, TRUE, FALSE, 0);
	_m_wait_thread = CreateEvent(0, TRUE, FALSE, 0);

	// Initialize variables
	InitHandranktTableForPrwin();
	AfxBeginThread(IteratorThreadFunction, this);

	write_log(Preferences()->debug_prwin(), "[PrWinThread] Iterator Thread started.\n");
}

CIteratorThread::~CIteratorThread() {
	write_log(Preferences()->debug_prwin(), "[PrWinThread] Iterator Thread ending...\n");
  assert(p_iterator_thread != NULL);
	if (_m_stop_thread)
	{
		// Trigger thread to die
		::SetEvent(_m_stop_thread);

		// Wait until thread finished
		::WaitForSingleObject(_m_wait_thread, k_max_time_to_wait_for_thread_to_shutdown);

		// Close handles
		::CloseHandle(_m_stop_thread);
		::CloseHandle(_m_wait_thread);
	}
	p_iterator_thread = NULL;

	write_log(Preferences()->debug_prwin(), "[PrWinThread] Iterator Thread ended.\n");
}

void CIteratorThread::RestartPrWinComputations() {
  if (p_engine_container->symbol_engine_isomaha()->isomaha()) {
    write_log(Preferences()->debug_prwin(), "[PrWinThread] Not restarting prwin computations, PrWin is Hold'em only\n");
    return;
  }
	write_log(Preferences()->debug_prwin(), "[PrWinThread] Restarting prwin computations.\n");
  assert(p_iterator_thread != NULL);
  assert(IteratorThreadWorking() == false);
  InitIteratorLoop();
  ResetIteratorVars();
	ResetGlobalVariables();
}

void CIteratorThread::AdjustPrwinVariablesIfNecessary() {
	// Cut off from IteratorThreadFunction
	// Also moved outside of the loop.
	// Correct the protection aganst low f$willplay/f$wontplay - Matrix 2008-12-22
  _nopponents = p_engine_container->symbol_engine_prwin()->nopponents_for_prwin();
	if (_willplay && (_willplay < 2 * _nopponents + 1))
	{
		write_log(Preferences()->debug_prwin(), "[PrWinThread] Adjusting willplay (too low)\n");
		_willplay = 2 * _nopponents + 1; //too low a value can give lockup
	}
	if (_wontplay < _willplay)
	{
		write_log(Preferences()->debug_prwin(), "[PrWinThread] Adjusting wontplay (too low)\n");
		_wontplay = _willplay; //wontplay cannot safely be less than willplay
	}
}

UINT CIteratorThread::IteratorThreadFunction(LPVOID pParam) {
	CIteratorThread *pParent = static_cast<CIteratorThread*>(pParam);
  // Loop-variables j, k get used inside and outside loops.
	// It is a bit messy, nearly impossible to fix it.
	// At least the outer loops ("f$prwin_number_of_iterations" and "i") could be improved.
	unsigned int	pl_pokval = 0, opp_pokval = 0, opp_pokvalmax = 0;
	HandVal		pl_hv = 0, opp_hv = 0;
	int				dummy = 0, enhanced_dealing_return=0;
	bool			hand_lost;

	ResetGlobalVariables();
	// Seed the RNG
	srand((unsigned)GetTickCount());
  while (true) {
    // Check event for thread stop signal once per main iterator loop
    // (and additionally once every 1000 iterations later)
    // !! can crash here on termination; pParent killed?
		//if(::WaitForSingleObject(pParent->_m_stop_thread, 0) == WAIT_OBJECT_0) {
    //
    // "invalid handle was specified"
    if (WaitForSingleObject(CIteratorThread::_m_stop_thread, 0) == WAIT_OBJECT_0) {
			// Set event
			::SetEvent(pParent->_m_wait_thread);
			AfxEndThread(0);
		}
    Sleep(250);
    if (IteratorThreadComplete()) {
      // No longer anything to do
      continue;
    }
	  // At this point, the iterator thread starts working on a new hand
    _iterations_calculated = 1;  // Set to 1 so it's clear the thread is busy
    _prwin = _prlos = _prtie = 0;
    if (!p_engine_container->symbol_engine_autoplayer()->ismyturn()) {
      // Not my turn;
      // Nothing to simulate
      write_log(Preferences()->debug_prwin(), "[PrWinThread] Waiting (Not my turn).\n");
      continue;
    }
    if (IteratorThreadComplete()) {
      // No longer anything to do
      continue;
    }
    _nopponents = p_engine_container->symbol_engine_prwin()->nopponents_for_prwin();
    if (_nopponents <= 0) {
      // Wait until _nopponents is valid
      write_log(Preferences()->debug_prwin(), "[PrWinThread] Waiting (_nopponents invalid).\n");
      continue;
    }
    write_log(Preferences()->debug_prwin(), "[PrWinThread] Start of main loop.\n");
    // "f$prwin_number_of_iterations" has to be declared outside of the loop,
    // as we check afterwards, if the loop terminated successfully.
    AdjustPrwinVariablesIfNecessary();
    LARGE_INTEGER frequency;        // ticks per second
    LARGE_INTEGER t1, t2;           // ticks
    double elapsedTime = 0;
    QueryPerformanceFrequency(&frequency); // get ticks per second
    CalculateTotalWeights();
  	//
	  // Main iterator loop
	  //
	  for (_iterations_calculated=1; _iterations_calculated < _iterations_required; ++_iterations_calculated) {
		  // Check event for thread stop signal once every 1000 iterations
      if ((_iterations_calculated % 1000 == 0)
          && (_iterations_calculated > 0)
          && (::WaitForSingleObject(pParent->_m_stop_thread, 0) == WAIT_OBJECT_0)) {
			  // Set event
			  ::SetEvent(pParent->_m_wait_thread);
			  AfxEndThread(0);
		  }
      CardMask_OR(usedCards, pParent->_plCards, pParent->_comCards);
		  if (UseEnhancedPrWin())	{
			  QueryPerformanceCounter(&t1);	// start timer		  
			  enhanced_dealing_return = EnhancedDealingAlgorithm();
			  if (enhanced_dealing_return < 0) {
				  _prwin = enhanced_dealing_return;
				  _prtie = enhanced_dealing_return;
				  _prlos = enhanced_dealing_return;
				  _iterations_calculated = _iterations_required;
				  write_log(Preferences()->debug_prwin(), "[PrWinThread] Chair's %i range consists of dead cards only.\n",enhanced_dealing_return);
				  break;
			  }
			  QueryPerformanceCounter(&t2); // stop timer
			  elapsedTime = elapsedTime + (t2.QuadPart - t1.QuadPart) * 1000.0 / frequency.QuadPart; // compute the elapsed time in millisec
		  }	else { 
			  QueryPerformanceCounter(&t1);	// start timer
			  StandardDealingAlgorithm(_nopponents);
			  QueryPerformanceCounter(&t2); // stop timer
			  elapsedTime = elapsedTime + (t2.QuadPart - t1.QuadPart) * 1000.0 / frequency.QuadPart; // compute the elapsed time in millisec
		  }
      // Get my handval/pokerval
		  CardMask_OR(evalCards, pParent->_plCards, pParent->_comCards);
		  CardMask_OR(evalCards, evalCards, addlcomCards);
		  pl_hv = Hand_EVAL_N(evalCards, 7);
		  pl_pokval = p_engine_container->symbol_engine_pokerval()->CalculatePokerval(pl_hv, 7, &dummy, CARD_NOCARD, CARD_NOCARD);//??
      // Scan through opponents' handvals/pokervals
		  // - if we find one better than ours, then we are done, increment los
		  // - for win/tie, we need to wait until we scan them all
		  opp_pokvalmax = 0;
		  hand_lost = false;
		  for (int i=0; i<_nopponents; i++) {
			  CardMask_RESET(opp_evalCards);
			  CardMask_OR(opp_evalCards, pParent->_comCards, addlcomCards);
			  CardMask_SET(opp_evalCards, ocard[i*2]);
			  CardMask_SET(opp_evalCards, ocard[(i*2)+1]);
			  opp_hv = Hand_EVAL_N(opp_evalCards, 7);
			  opp_pokval = p_engine_container->symbol_engine_pokerval()->CalculatePokerval(opp_hv, 7, &dummy, CARD_NOCARD, CARD_NOCARD);
				write_log(Preferences()->debug_prwin(), "[PrWinThread] PlayerPV: %i OppPV: %i\n",
				pl_pokval, opp_pokval);
			  if (opp_pokval > pl_pokval) {
				  write_log(Preferences()->debug_prwin(), "[PrWinThread] Lost\n");
				  _los++;
				  hand_lost = true;
				  break;
			  }	else {
				  if (opp_pokval > opp_pokvalmax)	
				  {
					  opp_pokvalmax = opp_pokval;
				  }
			  }
		  }
		  if (!hand_lost)	{
			  if (pl_pokval > opp_pokvalmax) {
          write_log(Preferences()->debug_prwin(), "[PrWinThread] Won\n");
				  _win++;
			  }	else {
          write_log(Preferences()->debug_prwin(), "[PrWinThread] Tie\n");
				  _tie++;
			  }
		  }
		  UpdateIteratorVarsForDisplay();
	  }
	  write_log(Preferences()->debug_prwin(), "[PrWinThread] End of main loop.\n");
    if (!IteratorThreadComplete()) {
      // Computation stopped with some kind of error.
      // Reset vars to avoid bogus data
		  ResetIteratorVars();
	  }

	if(enhanced_dealing_return == 0)
	{
		UpdateIteratorVarsForDisplay(); //calculates _prwin, _prtie, _prlos
	}

	  if(UseEnhancedPrWin())
	  {			
			write_log(Preferences()->debug_prwin(), "EnhancedDealingAlgorithm elapsed time in millisec: %.3f Iterations: %d prwin: %.3f prtie: %.3f prlos: %.3f vanilla.limit: %i \n",
				elapsedTime,_iterations_calculated, _prwin, _prtie, _prlos, _prw1326.vanilla_chair.limit );
	  }
	  else
	  {
			write_log(Preferences()->debug_prwin(), "StandardDealingAlgorithm elapsed time in millisec: %.3f Iterations: %d prwin: %.3f prtie: %.3f prlos: %.3f\n",
				elapsedTime,_iterations_calculated, _prwin, _prtie, _prlos);
	  }

    ::SetEvent(pParent->_m_wait_thread);
  }
	return 0;
}

void CIteratorThread::UpdateIteratorVarsForDisplay() {
	// Update display once every 1000 iterations
	if (IteratorThreadComplete()
		|| ((_iterations_calculated % 1000 == 0) && (_iterations_calculated >= 1000)))
	{
		_prwin = _win / (double) _iterations_calculated;
		_prtie = _tie / (double) _iterations_calculated;
		_prlos = _los / (double) _iterations_calculated;
		write_log(Preferences()->debug_prwin(), "[PrWinThread] Progress: %d %.3f %.3f %.3f\n", 
			_iterations_calculated, _prwin, _prtie, _prlos);

		if (p_engine_container->EvaluateSymbol("f$nOuts", &_nouts)) {
			write_log(Preferences()->debug_prwin(), "[nOuts] Value: %i\n", (int)_nouts);
		}
		else _nouts = 0;

		if (p_engine_container->EvaluateSymbol("f$OutOdds", &_outodds)) {
			write_log(Preferences()->debug_prwin(), "[OutOdds] Value: %.3f\n", _outodds);
		}
		else _outodds = 0.0;

		if (p_engine_container->EvaluateSymbol("f$ImpliedOdds", &_impliedodds)) {
			write_log(Preferences()->debug_prwin(), "[ImpliedOdds] Value: %.3f\n", _impliedodds);
		}
		else _impliedodds = 0.0;
		
		if (p_engine_container->EvaluateSymbol("f$PotOdds", &_potodds)) {
			write_log(Preferences()->debug_prwin(), "[PotOdds] Value: %.3f\n", _potodds);
		}
		else _potodds = 0.0;

		if (p_engine_container->EvaluateSymbol("f$MyEquity", &_myequity)) {
			write_log(Preferences()->debug_prwin(), "[MyEquity] Value: %.3f\n", _myequity);
		}
		else _myequity = 0.0;

		if (p_engine_container->EvaluateSymbol("f$PotEquity", &_potequity)) {
			write_log(Preferences()->debug_prwin(), "[PotEquity] Value: %.3f\n", _potequity);
		}
		else _potequity = 0.0;

		if (p_engine_container->EvaluateSymbol("f$prwin_mustplay", &_prwin_mustplay)) {
			write_log(Preferences()->debug_prwin(), "[Prwin_Mustplay] Value: %s\n", (bool)_prwin_mustplay);
		}
		else _prwin_mustplay = FALSE;

    p_openholdem_statusbar->SetPrWin(_prwin, _prtie, _prlos);
	p_white_info_box->SetGto(_prwin, _prtie, _prlos, (int)_nouts, _outodds, _impliedodds, _potodds, _myequity, _potequity, (bool)_prwin_mustplay);
	p_rta_window->SetGto(_prwin, _prtie, _prlos, (int)_nouts, _outodds, _impliedodds, _potodds, _myequity, _potequity, (bool)_prwin_mustplay);
	}
}

void CIteratorThread::ResetIteratorVars() {
	_prwin = 0.0;
	_prtie = 0.0;
	_prlos = 0.0;
	_nouts = 0.0;
	_outodds = 0.0;
	_impliedodds = 0.0;
	_potodds = 0.0;
  _iterations_calculated = 0;
  memset(_total_weight, 0, sizeof(_total_weight));  
}

void CIteratorThread::ResetGlobalVariables() {
	CardMask_RESET(usedCards);
	CardMask_RESET(temp_usedCards);
	CardMask_RESET(addlcomCards);
	CardMask_RESET(evalCards);
	CardMask_RESET(opp_evalCards);
	for (int i=0; i<kNumberOfCardsPerDeck; i++) {
		deck[i] = 0;
	}
	for (int i=0; i<MAX_OPPONENTS; i++) {
		ocard[2*i] = 0;
		ocard[2*i + 1] = 0;
	}
}

void CIteratorThread::CalculateTotalWeights()
{
	if (!UseEnhancedPrWin()) return;

	int userchair = p_engine_container->symbol_engine_userchair()->userchair();
	int playersplayingbits = p_engine_container->symbol_engine_active_dealt_playing()->playersplayingbits();

	for(int eachChair=0; eachChair < kMaxNumberOfPlayers; eachChair++) //calculate total weights for all playing opponents
	{
		if (eachChair == userchair) continue; //skip our own chair!
		if (!(playersplayingbits & (1<<eachChair))) continue; //skip inactive chairs 
		for(int eachPossibleHand=0; eachPossibleHand < _prw1326.chair[eachChair].limit; eachPossibleHand++)
		{
			_total_weight[eachChair] += _prw1326.chair[eachChair].weight[eachPossibleHand];
		}			
	}	

}

void CIteratorThread::InitNumberOfIterations() {
	_iterations_required = p_function_collection->Evaluate(
		k_standard_function_names[k_prwin_number_of_iterations], Preferences()->log_prwin_functions());
	_iterations_required = _iterations_required / 5;   // added to increase calculation speed
}

void CIteratorThread::InitIteratorLoop() {
	write_log(Preferences()->debug_prwin(), "[PrWinThread] Initializing iterator loop\n");

	// Set starting status and parameters
	InitNumberOfIterations();
	// player cards
	CardMask_RESET(_plCards);
	CardMask_RESET(_comCards);
	_nplCards = _ncomCards = 0;

	// Counters
	_win = _tie = _los = 0;

  int userchair = p_engine_container->symbol_engine_userchair()->userchair();
  if (userchair == kUndefined) return;

	// setup masks
  AssertRange(userchair, 0, kMaxChairNumber);
	for (int i=0; i<NumberOfCardsPerPlayer(); i++) {
    Card *card = p_table_state->User()->hole_cards(i);
    if (card->IsKnownCard()) {
      CardMask_SET(_plCards, card->GetValue());
			_nplCards++;
		}
	}
	for (int i=0; i<kNumberOfCommunityCards; i++) {
    Card *card = p_table_state->CommonCards(i);
    if (card->IsKnownCard()) {
      CardMask_SET(_comCards, card->GetValue());
			_ncomCards++;
		}
	}

	//Weighted prwin only for nopponents <=13
  _topclip = p_function_collection->Evaluate("f$prwin_topclip", Preferences()->log_prwin_functions());
  _mustplay = p_function_collection->Evaluate("f$prwin_mustplay", Preferences()->log_prwin_functions());
	_willplay = p_function_collection->Evaluate("f$prwin_willplay", Preferences()->log_prwin_functions());
	_wontplay = p_function_collection->Evaluate("f$prwin_wontplay", Preferences()->log_prwin_functions());
	// Call prw1326 callback if needed
	if (_prw1326.useme==1326 
		  && _prw1326.usecallback==1326 
		  && (p_betround_calculator->betround()!= kBetroundPreflop
			  || _prw1326.preflop==1326) ){
		_prw1326.prw_callback(); //Matrix 2008-05-09
	}
}

void CIteratorThread::InitHandranktTableForPrwin() {
	int		vndx = 0;
	char	*ptr = NULL;
	memset(_total_weight, 0, sizeof(_total_weight));

	//Initialise the handrank tables used by prwin
	vndx=0; //used to provide an offset into the vanilla table
	for (int i=0; i<kNumberOfStartingHands; i++)
	{
		//normal weighted prwin table
		ptr = prwin_handrank_table_169[i];
		int j=(strchr(k_card_chars,*ptr)-k_card_chars)*13 + (strchr(k_card_chars,*(ptr+1))-k_card_chars);
		if (*(ptr+2)=='s')
		{
			pair2rank_suited[j]= i+1;
		}
		else 
		{
			pair2rank_offsuited[j] = i+1;
		}
		//prw1326 vanilla table
		j=strchr(k_card_chars,*ptr)-k_card_chars;
		int k=strchr(k_card_chars,*(ptr+1))-k_card_chars;
		for(;;)
		{
			//I originally had an algorithm to do this, but it was obscure and impenetrable
			//so now I have switched to the clumsy but simple approach.
			if(j==k)//pair
			{
				_prw1326.vanilla_chair.rankhi[vndx]=j;	//h
				_prw1326.vanilla_chair.rankhi[vndx+1]=j;	//h
				_prw1326.vanilla_chair.rankhi[vndx+2]=j; //h
				_prw1326.vanilla_chair.rankhi[vndx+3]=j+13; //d
				_prw1326.vanilla_chair.rankhi[vndx+4]=j+13; //d
				_prw1326.vanilla_chair.rankhi[vndx+5]=j+26; //c
				_prw1326.vanilla_chair.ranklo[vndx]=k+13;	//d
				_prw1326.vanilla_chair.ranklo[vndx+1]=k+26;	//c
				_prw1326.vanilla_chair.ranklo[vndx+2]=k+39;	//s
				_prw1326.vanilla_chair.ranklo[vndx+3]=k+26;	//c	
				_prw1326.vanilla_chair.ranklo[vndx+4]=k+39;	//s
				_prw1326.vanilla_chair.ranklo[vndx+5]=k+39;	//s
				vndx+=6;
				break;
			}
		
			if (*(ptr+2)=='s') //suited
			{
				_prw1326.vanilla_chair.rankhi[vndx]=j;		//h
				_prw1326.vanilla_chair.rankhi[vndx+1]=j+13;	//d
				_prw1326.vanilla_chair.rankhi[vndx+2]=j+26;	//c
				_prw1326.vanilla_chair.rankhi[vndx+3]=j+39;	//s
				_prw1326.vanilla_chair.ranklo[vndx]=k;		//h
				_prw1326.vanilla_chair.ranklo[vndx+1]=k+13;	//d
				_prw1326.vanilla_chair.ranklo[vndx+2]=k+26;	//c
				_prw1326.vanilla_chair.ranklo[vndx+3]=k+39;	//s
				vndx+=4;
				break;
			}
		
			//only unsuited non-pairs left
			_prw1326.vanilla_chair.rankhi[vndx]=j;		//h
			_prw1326.vanilla_chair.rankhi[vndx+1]=j;		//h
			_prw1326.vanilla_chair.rankhi[vndx+2]=j;		//h
			_prw1326.vanilla_chair.rankhi[vndx+3]=j+13;	//d
			_prw1326.vanilla_chair.rankhi[vndx+4]=j+13;	//d
			_prw1326.vanilla_chair.rankhi[vndx+5]=j+13;	//d
			_prw1326.vanilla_chair.rankhi[vndx+6]=j+26;	//c
			_prw1326.vanilla_chair.rankhi[vndx+7]=j+26;	//c
			_prw1326.vanilla_chair.rankhi[vndx+8]=j+26;	//c
			_prw1326.vanilla_chair.rankhi[vndx+9]=j+39;	//s
			_prw1326.vanilla_chair.rankhi[vndx+10]=j+39;	//s
			_prw1326.vanilla_chair.rankhi[vndx+11]=j+39; //s 
			_prw1326.vanilla_chair.ranklo[vndx]=k+13;	//d
			_prw1326.vanilla_chair.ranklo[vndx+1]=k+26;	//c
			_prw1326.vanilla_chair.ranklo[vndx+2]=k+39;	//s
			_prw1326.vanilla_chair.ranklo[vndx+3]=k;		//h
			_prw1326.vanilla_chair.ranklo[vndx+4]=k+26;	//c
			_prw1326.vanilla_chair.ranklo[vndx+5]=k+39;	//s
			_prw1326.vanilla_chair.ranklo[vndx+6]=k;		//h
			_prw1326.vanilla_chair.ranklo[vndx+7]=k+13;	//d
			_prw1326.vanilla_chair.ranklo[vndx+8]=k+39;	//s
			_prw1326.vanilla_chair.ranklo[vndx+9]=k;		//h
			_prw1326.vanilla_chair.ranklo[vndx+10]=k+13;	//d
			_prw1326.vanilla_chair.ranklo[vndx+11]=k+26;	//c
			vndx+=12;
			break;
		}
	}

	_prw1326.vanilla_chair.level=1024;
	_prw1326.vanilla_chair.limit=820; //cut off a little early, since 820-884 very improbable

	const int k_first_third_1326_range  =  442;
	const int k_second_third_1326_range =  884;
	const int k_third_third_1326_range  = 1326;

	// now assign a weight table. Assume upper third fully probable, next third reducing
	// probability, lowest third not played.
	for(int i=0; i<k_first_third_1326_range; i++)
	{
		_prw1326.vanilla_chair.weight[i]=_prw1326.vanilla_chair.level;
	}
	for(int i=k_first_third_1326_range; i<k_second_third_1326_range; i++)
	{
		_prw1326.vanilla_chair.weight[i]=_prw1326.vanilla_chair.level*(884-i)/442;
	}
	for(int i=k_second_third_1326_range; i<k_third_third_1326_range; i++)
	{
		_prw1326.vanilla_chair.weight[i]=0;
	}

	CloneVanillaChairToAllOtherChairs();
}

void CIteratorThread::CloneVanillaChairToAllOtherChairs() {
	// finally copy the vanilla to all user chairs so that someone who just turns on prw1326
	// experimentally does not cause a crash
  // http://www.maxinmontreal.com/forums/viewtopic.php?f=124&t=19012&hilit=%2Avanilla%2A
	write_log(Preferences()->debug_prwin(), "[PrWinThread] CIteratorThread::CloneVanillaChairToAllOtherChairs \n");
	for(int i=0; i<kMaxNumberOfPlayers; ++i) {
		_prw1326.chair[i] = _prw1326.vanilla_chair;
	}
}

void CIteratorThread::StandardDealingAlgorithm(int nopponents)
{
	// Normal prwin opponent card selection
	// Random ranges for everybody
	// Number of opponents might be adapted to get more "reasonable" results
	write_log(Preferences()->debug_prwin(), "[PrWinThread] Using standard prwin.\n");

	// if f$prwin_number_of_opponents<=13 then deal with random replacement algorithm, otherwise deal with swap algorithm
	if (nopponents <= 13)
	{
		StandardDealingAlgorithmForUpTo13Opponents(nopponents);
	}
	else
	{
		SwapDealingAlgorithmForMoreThan13Opponents(nopponents);
	}
}

void CIteratorThread::SwapDealingAlgorithmForMoreThan13Opponents(int nopponents)
{
	write_log(Preferences()->debug_prwin(), "[PrWinThread] Useing swap-algorithm, as f$prwin_number_of_opponents > 13\n");
	// swap alogorithm
	// weighted prwin not implemented for this case
	int numberOfCards = kNumberOfCardsPerDeck;
	for (int i=0; i<numberOfCards; i++)
	{
		deck[i] = i;
	}

	while (numberOfCards >= 1)
	{
		int selected_card = rand() % numberOfCards;
		numberOfCards--;
		if (selected_card != numberOfCards)
		{
			SwapInts(&deck[selected_card], &deck[numberOfCards]);
		}
	}

	// opponent cards
	int x = 0;
	for (int i=0; 
		i<nopponents*NumberOfCardsPerPlayer(); 
		i++)
	{
		while (CardMask_CARD_IS_SET(usedCards, deck[x]) 
			&& (x < kNumberOfCardsPerDeck)) 
		{
			x++;
		}
		ocard[i] = deck[x++];
	}

	// additional common cards
	CardMask_RESET(addlcomCards);
	for (int i=0; i<(kNumberOfCommunityCards - _ncomCards); i++)
	{
		while (CardMask_CARD_IS_SET(usedCards, deck[x]) 
			&& (x < kNumberOfCardsPerDeck)) 
		{
			x++;
		}
		CardMask_SET(addlcomCards, deck[x++]);
	}
}

int CIteratorThread::GetRandomCard()
{
	// We use bit-operations for truncation
	// That is better as modulo (52) which introdues a bias
	const int k_low_6_bits = 0x3F;
	// The search-loop is guaranteed to terminate,
	// as there are 52-cards in the deck, 
	// but at most 25 cards in use (10*2 player-cards + 5 community-cards).
	while (true)
	{
		int card = rand() & k_low_6_bits;
		if ((card < kNumberOfCardsPerDeck)
			&& !CardMask_CARD_IS_SET(usedCards, card))
		{
			// We found a good card
			return card;
		}
	}
}

void CIteratorThread::StandardDealingAlgorithmForUpTo13Opponents(int nopponents)
{
	unsigned int	card = 0;

	write_log(Preferences()->debug_prwin(), "[PrWinThread] Using random algorithm, as f$prwin_number_of_opponents <= 13\n");
	// random replacement algorithm
	// opponent cards
	if (nopponents < 1) {
		write_log(Preferences()->debug_prwin(), "[PrWinThread] No opponents. Auto-adapting to 1.\n");
    nopponents = 1;
	}
	for (int i=0; 
		i<nopponents*NumberOfCardsPerPlayer(); 
		i+=NumberOfCardsPerPlayer())
	{
		temp_usedCards=usedCards;
		do
		{
			usedCards = temp_usedCards; //reset the card mask to clear settings from failed card assignments

			for (int j=0; j<NumberOfCardsPerPlayer(); j++)
			{
				card = GetRandomCard();
				CardMask_SET(usedCards, card);
				ocard[i+j] = card;
			}

			if (!_willplay)
			{
				write_log(Preferences()->debug_prwin(), "[PrWinThread] Weighting disabled. Willplay is 0.\n");
				break; //0 disables weighting
			}

			//put break for i=0 and opponent unraised BB case (cannot assume anything about his cards)
			//In round 1 we should really do an analysis of chairs to find out how many have still to
			//place a bet. Not implemented since accuracy of prwin pre-flop is less critical.
			//
      // bblimp and special handling for checking players removed in OH 9.2.8
      // as it was broken and we consider it useless / even harmful.
      // http://www.maxinmontreal.com/forums/viewtopic.php?f=156&t=19064
		} while (!IsHandInWeightedRange(ocard[i], ocard[i+1],
			_willplay, _wontplay, 
			_topclip, _mustplay));
	}
	// additional common cards
	CardMask_RESET(addlcomCards);
	for (int i=0; i<(kNumberOfCommunityCards - _ncomCards); i++)
	{
		card = GetRandomCard();
		CardMask_SET(usedCards, card);
		CardMask_SET(addlcomCards, card);
	}
}

int CIteratorThread::EnhancedDealingAlgorithm() {
	write_log(Preferences()->debug_prwin(), "[PrWinThread] Using ZeeZooLaa's enhanced prwin.\n");
	unsigned int	card = 0, deadHandsCounter = 0;
	int k = 0; //k is used as an index into ocard[]
	int userchair = p_engine_container->symbol_engine_userchair()->userchair();
	int playersplayingbits = p_engine_container->symbol_engine_active_dealt_playing()->playersplayingbits();
	int chairWeight;
	bool deadHands[k_number_of_pocketcard_combinations_without_order];

	for(int eachChair=0; eachChair < kMaxNumberOfPlayers; eachChair++) { // loop through playing opponents
		if (eachChair == userchair) continue; //skip our own chair!
		if (!(playersplayingbits & (1<<eachChair))) continue; //skip inactive chairs 
		chairWeight = _total_weight[eachChair];
		if (_prw1326.chair[eachChair].ignore || chairWeight <= 0	)	{
			card = GetRandomCard();
			CardMask_SET(usedCards, card);
			ocard[k++] = card;

			card = GetRandomCard();
			CardMask_SET(usedCards, card);
			ocard[k++] = card;

			continue;
		} // end of special non-weighted cases

		memset(deadHands,false,sizeof(deadHands));
		deadHandsCounter = 0;

		bool random_weighted_hand_was_found = false;
		while(!random_weighted_hand_was_found) {
			int random_weight = RNG::Instance()->under(chairWeight);	//find random_weight which is between [0..chairWeight)
			for (int eachPossibleHand=0; eachPossibleHand < _prw1326.chair[eachChair].limit; eachPossibleHand++) {	//find random weighted hand			
				if (!deadHands[eachPossibleHand] && random_weight < _prw1326.chair[eachChair].weight[eachPossibleHand]) { //random hand found.
					if(CardMask_CARD_IS_SET(usedCards, _prw1326.chair[eachChair].rankhi[eachPossibleHand] ) 
              || CardMask_CARD_IS_SET(usedCards, _prw1326.chair[eachChair].ranklo[eachPossibleHand] )) {
						//hand contains dead card
						deadHands[eachPossibleHand] = true;
						deadHandsCounter++;
						chairWeight -= _prw1326.chair[eachChair].weight[eachPossibleHand];
						if(deadHandsCounter == _prw1326.chair[eachChair].limit || chairWeight <= 0) {
							//all range consists only of dead cards
							//failed to satisfy the specified range, user possibly needs to expand the range of corresponding chair
							if(eachChair == 0) return -10;
							return 0-eachChair; 
						}
						break; //generate new random_weight
					}
					//hand not dead, use it
					ocard[k++] = _prw1326.chair[eachChair].rankhi[eachPossibleHand];
					ocard[k++] = _prw1326.chair[eachChair].ranklo[eachPossibleHand];

					CardMask_SET(usedCards, ocard[k-2]);
					CardMask_SET(usedCards, ocard[k-1]);
					random_weighted_hand_was_found = true;
					break;
				}
				//keep decreasing the random_weight until it becomes less then _prw1326.chair[eachPlayer].weight
				if(!deadHands[eachPossibleHand]) {
					random_weight -= _prw1326.chair[eachChair].weight[eachPossibleHand];
				}
			}//end of eachPossibleHand
		}//end of random_weighted_hand_was_found
	} //end of eachPlayer

	// additional common cards
	CardMask_RESET(addlcomCards);
	for (int i=0; i<(kNumberOfCommunityCards - _ncomCards); i++) {
		card = GetRandomCard();
		CardMask_SET(usedCards, card);
		CardMask_SET(addlcomCards, card);
	}
  return 0; //success
}

bool CIteratorThread::UseEnhancedPrWin() {
	return (_prw1326.useme==1326 
		&& (p_betround_calculator->betround() >= kBetroundFlop 
			|| _prw1326.preflop==1326));
}