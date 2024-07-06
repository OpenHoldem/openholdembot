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
#include "CSymbolEngineGameType.h"

#include "CCasinoInterface.h"
#include "CEngineContainer.h"

#include "CScraper.h"
#include "CSymbolEngineIsTournament.h"
#include "CTableState.h"

CSymbolEngineGameType	*p_symbol_engine_gametype = NULL;

CSymbolEngineGameType::CSymbolEngineGameType() {
	// The values of some symbol-engines depend on other engines.
	// As the engines get later called in the order of initialization
	// we assure correct ordering by checking if they are initialized.
	assert(p_engine_container->symbol_engine_istournament() != NULL);
  InitOnStartup();
}

CSymbolEngineGameType::~CSymbolEngineGameType() {
}

void CSymbolEngineGameType::InitOnStartup() {
	UpdateOnConnection();
}

void CSymbolEngineGameType::UpdateOnConnection() {
	_gametype = kGametypeFL; 
}

void CSymbolEngineGameType::UpdateOnHandreset() {
}

void CSymbolEngineGameType::UpdateOnNewRound() {
}

void CSymbolEngineGameType::UpdateOnMyTurn() {
}

void CSymbolEngineGameType::UpdateOnHeartbeat() {
  int scraped_limit = p_table_state->_s_limit_info.limit();
  switch (scraped_limit) {
    case kGametypeNL:
    case kGametypePL:
    case kGametypeFL:
      _gametype = scraped_limit;
      return;
  }
  // No relia ble undo from scraper
  // Let's do some guess-work, based on GUI-elements
  //
  // No reset to FL, because we want to remember previous guesses,
  if (isnl()) {
    // Known type. Can't go back to FL/PL.
    return;
  }
  if (p_casino_interface->AllinOptionAvailable()) {
    _gametype = kGametypeNL;
  } else if (isfl()) {
    // FL or unknown ATM.
    // Might also be PL, but there is no good way to detect.
  }
}

CString CSymbolEngineGameType::GetGameTypeAsString() {
	CString result = "";
	if (isnl())	{
		result = "NL";
	}	else if (ispl()) {
		result = "PL";
	}	else if (isfl()) {
		result = "FL";
	}	else {
    // Default: assume fixed limit.
    // That's better than former "unknown".
		result = "FL";
	}
	if (p_engine_container->symbol_engine_istournament()->istournament()) {
		result += "T";
	}
	return result;
}

bool CSymbolEngineGameType::EvaluateSymbol(const CString name, double *result, bool log /* = false */) {
  FAST_EXIT_ON_OPENPPL_SYMBOLS(name);
	if (memcmp(name, "is", 2)==0)	{
		if (memcmp(name, "isnl", 4)==0 && strlen(name)==4) {
			*result = isnl();
		}	else if (memcmp(name, "ispl", 4)==0 && strlen(name)==4)	{
			*result = ispl();
		}	else if (memcmp(name, "isfl", 4)==0 && strlen(name)==4)	{
			*result = isfl();
		}	else {
			// Invalid symbol
			return false;
		}
		// Valid symbol
		return true;
	}	else if (memcmp(name, "lim", 3)==0 && strlen(name)==3) {
		*result = gametype();
    return true;
	}	
	// Symbol of a different symbol-engine
	return false;
}

CString CSymbolEngineGameType::SymbolsProvided() {
  CString list = "isnl ispl isfl lim ";
  return list;
}