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

#include "CPreferences.h"
#include "CScraper.h"
#include "CTableState.h"
#include "MagicNumbers.h"

CSymbolEngineDealerchair *p_symbol_engine_dealerchair = NULL;

CSymbolEngineDealerchair::CSymbolEngineDealerchair()
{
	// The values of some symbol-engines depend on other engines.
	// As the engines get later called in the order of initialization
	// we assure correct ordering by checking if they are initialized.
	//
	// This engine does not use any other engines.
}

CSymbolEngineDealerchair::~CSymbolEngineDealerchair()
{}

void CSymbolEngineDealerchair::InitOnStartup()
{
	UpdateOnConnection();
}

void CSymbolEngineDealerchair::UpdateOnConnection()
{
	_dealerchair = kUndefined;
}

void CSymbolEngineDealerchair::UpdateOnHandreset()
{}

void CSymbolEngineDealerchair::UpdateOnNewRound()
{}

void CSymbolEngineDealerchair::UpdateOnMyTurn()
{}

void CSymbolEngineDealerchair::UpdateOnHeartbeat() {
	write_log(preferences.debug_symbolengine(), "nchairs: %d\n", 
		p_tablemap->nchairs());
	for (int i=0; i<p_tablemap->nchairs(); i++)	{
		if (p_table_state->Player(i)->dealer())	{
			write_log(preferences.debug_symbolengine(), "Setting dealerchair to %d\n", i);
			_dealerchair = i;					
			break;
		}
	}
	// Otherwise: do nothing and keep the dealer as is.
	// Dealer-button probably just temporary occluded.
	// Do not reset, as this might cause a hand-reset.
}

bool CSymbolEngineDealerchair::EvaluateSymbol(const CString name, double *result, bool log /* = false */)
{
  FAST_EXIT_ON_OPENPPL_SYMBOLS(name);
	if (name == "dealerchair")	
	{
		*result = p_symbol_engine_dealerchair->dealerchair();
		return true;
	}
  if (name == "buttonchair")
  {
    *result = p_symbol_engine_dealerchair->dealerchair();
    return true;
  }
	return false;
}

CString CSymbolEngineDealerchair::SymbolsProvided() {
  return "dealerchair buttonchair ";
}
	