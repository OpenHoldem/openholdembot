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

#ifndef INC_CITERATORTHREAD_H
#define INC_CITERATORTHREAD_H

#include "inlines/eval.h"
#include "CSpaceOptimizedGlobalObject.h"

extern sprw1326	_prw1326;	// prwin 1326 data structure Matrix 2008-04-29

class CIteratorThread /*!!!!!: public CSpaceOptimizedGlobalObject */{
 public:
	// public functions
	CIteratorThread();
	~CIteratorThread();
 public:
  // Public accessors()
  double prwin()   { return _prwin; }
  double prtie()   { return _prtie; }
  double prlos()   { return _prlos; }
  double outs() { return _nouts; }
  double outodds() { return _outodds; }
  double impliedodds() { return _impliedodds; }
  double potodds() { return _potodds; }
  double myequity() { return _myequity; }
  double potequity() { return _potequity; }
  double prwin_mustplay() { return _prwin_mustplay; }
 public:
  static bool IteratorThreadComplete() { return _iterations_calculated >= _iterations_required; }
  bool IteratorThreadWorking()    { return ((_iterations_calculated > 0) && (_iterations_calculated < _iterations_required)); }
  int  IteratorThreadProgress()   { return _iterations_calculated; }
 public:
	void set_prw1326_useme(const int i)	{ _prw1326.useme = i;}
	const	sprw1326 *prw1326()	          { return &_prw1326; }
 public:
  // For the DLL "cmd$recalc"
  void RestartPrWinComputations();
#undef ENT
 private:
	// private functions and variables - not available via accessors or mutators
	static UINT IteratorThreadFunction(LPVOID pParam);
	static void AdjustPrwinVariablesIfNecessary();
	static void StandardDealingAlgorithm(int nopponents);
	static void SwapDealingAlgorithmForMoreThan13Opponents(int nopponents);
	static void StandardDealingAlgorithmForUpTo13Opponents(int nopponents);
	static int EnhancedDealingAlgorithm();
	static bool UseEnhancedPrWin();
	static int  GetRandomCard();	
 private:
	static void UpdateIteratorVarsForDisplay();
	static void ResetIteratorVars();
	static void ResetGlobalVariables();
	static void CalculateTotalWeights();
 private:
	void InitIteratorLoop();
	void InitHandranktTableForPrwin();
	void CloneVanillaChairToAllOtherChairs();
	void InitNumberOfIterations();
 private:
	// variables for iterator loop
	CardMask		_plCards, _comCards;
	HANDLE			_m_wait_thread;
  static HANDLE			_m_stop_thread; //!!!!!
 private:
  static int _iterations_calculated;
  static int _iterations_required;
  static int _total_weight[kMaxNumberOfPlayers];
  static int _nopponents;;
 private:
	 static double _prwin, _prtie, _prlos, _nouts, _outodds, _impliedodds, _potodds, _myequity, _potequity, _prwin_mustplay;	 
};

extern CIteratorThread *p_iterator_thread;

#endif //INC_CITERATORTHREAD_H
