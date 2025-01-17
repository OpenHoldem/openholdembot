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

#include "stdafx.h"
#include "CSymbolEngineDealerchair.h"

#include "CEngineContainer.h"

#include "CScraper.h"
#include "CSymbolEngineTableLimits.h"
#include "CTableState.h"


CSymbolEngineDealerchair::CSymbolEngineDealerchair() {
	// The values of some symbol-engines depend on other engines.
	// As the engines get later called in the order of initialization
	// we assure correct ordering by checking if they are initialized.
	//
	// This engine does not use any other engines.
  //
  // This engine also might use CSymbolEngineTableLimits, but
  //   * only very rarely if we have to detect the dealer by the SB-chair
  //   * the small-blind is (more or less( constant
  //   * that symbol-engine gets initialized later
  // so we omit the check for initialization here.
}

CSymbolEngineDealerchair::~CSymbolEngineDealerchair()
{}

void CSymbolEngineDealerchair::InitOnStartup() {
	UpdateOnConnection();
}

void CSymbolEngineDealerchair::UpdateOnConnection() {
	_dealerchair = kUndefined;
}

void CSymbolEngineDealerchair::UpdateOnHandreset() {
}

void CSymbolEngineDealerchair::UpdateOnNewRound() {
}

void CSymbolEngineDealerchair::UpdateOnMyTurn() {
}

void CSymbolEngineDealerchair::UpdateOnHeartbeat() {
	write_log(Preferences()->debug_symbolengine(), "nchairs: %d\n", 
		p_tablemap->nchairs());
	for (int i=0; i<p_tablemap->nchairs(); i++)	{
		if (p_table_state->Player(i)->dealer())	{
			write_log(Preferences()->debug_symbolengine(), "Setting dealerchair to %d\n", i);
			_dealerchair = i;					
			return;
		}
	}
  // No dealer-button found.
  // Maybe temporary occluded, maybe completely bad tablemap
  // http://www.maxinmontreal.com/forums/viewtopic.php?f=117&t=21454
  // Try to find the smallblind
  int smallblindchair = SmallBlindChair();
  if (smallblindchair >= 0) {
    _dealerchair = RightHandActiveChair(smallblindchair);
	write_log(k_always_log_errors, "[CSymbolengineDealerchair] No dealer-button found - Probably occluded or bad TM - setting to: %i\n",
		_dealerchair);
	write_log(k_always_log_errors, "[CSymbolengineDealerchair] Standard search for dealer failed - Please get in contact with the development team\n");
  }
	// Otherwise: do nothing and keep the dealer as is.
	// Do not reset, as this might cause a hand-reset.
}

int CSymbolEngineDealerchair::SmallBlindChair() {
  for (int i = 0; i < p_tablemap->nchairs(); i++) {
    if (p_table_state->Player(i)->_bet.GetValue() == p_engine_container->symbol_engine_tablelimits()->sblind()) {
      return i;
    }
  }
  return kUndefined;
}

int CSymbolEngineDealerchair::RightHandActiveChair(int chair) {
  for (int i = (p_tablemap->nchairs() - 1); i >= 0; --i) {
    int next_chair = chair + i;
    next_chair %= p_tablemap->nchairs();
    if (p_table_state->Player(next_chair)->active()) {
      return next_chair;
    }
  }
  return kUndefined;
}

bool CSymbolEngineDealerchair::EvaluateSymbol(const CString name, double *result, bool log /* = false */) {
  FAST_EXIT_ON_OPENPPL_SYMBOLS(name);
	if (name == "dealerchair")	{
		*result = p_engine_container->symbol_engine_dealerchair()->dealerchair();
		return true;
	}
  if (name == "buttonchair") {
    *result = p_engine_container->symbol_engine_dealerchair()->dealerchair();
    return true;
  }
	return false;
}

CString CSymbolEngineDealerchair::SymbolsProvided() {
  return "dealerchair buttonchair ";
}
	
