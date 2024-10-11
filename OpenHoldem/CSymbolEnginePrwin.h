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

#ifndef INC_CSYMBOLENGINEPRWIN_H
#define INC_CSYMBOLENGINEPRWIN_H

#include "CVirtualSymbolEngine.h"
#include "assert.h"
#include "CSymbolEngineUserChair.h"
#include "CSymbolEngineTableLimits.h"


class CSymbolEnginePrwin: public CVirtualSymbolEngine
{
public:
	CSymbolEnginePrwin();
	~CSymbolEnginePrwin();
public:
	// Mandatory reset-functions
	void InitOnStartup();
	void UpdateOnConnection();
	void UpdateOnHandreset();
	void UpdateOnNewRound();
	void UpdateOnMyTurn();
	void UpdateOnHeartbeat();
  void UpdateAfterAutoplayerAction(int autoplayer_action_code);
public:
	// Public accessors
	bool EvaluateSymbol(const CString name, double *result, bool log = false);
	CString SymbolsProvided();
	void SetPrw1326(CString prw1326_symbol, CString prw1326_command);
	void UnSetLists();
	void LogHandRank();
public:
	int nhands()		  { return (_nhandshi + _nhandslo + _nhandsti); }
	int nhandshi()		{ return _nhandshi; }
	int nhandslo()		{ return _nhandslo; }
	int nhandsti()		{ return _nhandsti; }
	double prwinnow()	{ return _prwinnow; }
	double prlosnow()	{ return _prlosnow; }
public:
	int nopponents_for_prwin()	{ return _nopponents_for_prwin; }
private:
	void CalculateNOpponents();
	void CalculateNhands();
  bool StartOfPrWinComputationsNeeded();
  int GetChair(CString chair);
private:
	int _nhandshi;
	int _nhandslo;
	int _nhandsti;
	double _prwinnow;
	double _prlosnow;
private:
	int _nopponents_for_prwin;
  bool _known_change_in_gamestate_since_last_prwin_calculation;
  bool _prwin_recalc, _prwin_clearall;
};

#endif INC_CSYMBOLENGINEPRWIN_H
