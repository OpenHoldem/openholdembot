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
#include "CSymbolEngineRandom.h"

#include "MagicNumbers.h"
#include "NumericalFunctions.h"

CSymbolEngineRandom *p_symbol_engine_random = NULL;

CSymbolEngineRandom::CSymbolEngineRandom() {
	// The values of some symbol-engines depend on other engines.
	// As the engines get later called in the order of initialization
	// we assure correct ordering by checking if they are initialized.
	//
	// This engine does not use any other engines.
}

CSymbolEngineRandom::~CSymbolEngineRandom() {
}

void CSymbolEngineRandom::InitOnStartup() {
	UpdateOnConnection();
}

void CSymbolEngineRandom::UpdateOnConnection() {
	UpdateOnHandreset();
}

void CSymbolEngineRandom::UpdateOnHandreset() {
	_randomhand      = Random_0_1();
	_randomheartbeat = kUndefined;
	UpdateOnNewRound();
}

void CSymbolEngineRandom::UpdateOnNewRound() {
	_randomround = Random_0_1();
}

void CSymbolEngineRandom::UpdateOnMyTurn() {
}

void CSymbolEngineRandom::UpdateOnHeartbeat() {
	_randomheartbeat = Random_0_1();
}

double CSymbolEngineRandom::Random_0_1() {
	double result = ((double) rand() / (double) RAND_MAX);
	AssertRange(result, 0, 1);
	return result;
}

bool CSymbolEngineRandom::EvaluateSymbol(const CString name, double *result, bool log /* = false */) {
  if (memcmp(name, "Random", 6) == 0 && strlen(name) == 6) {
    // The OpenPPL-symbol "Random" is no longer implemented in the library
    // but as a built-in symbol to prevent symbol-caching.
    // http://www.maxinmontreal.com/forums/viewtopic.php?f=111&t=19611
    *result = Random(100);
    return true;
  }
  FAST_EXIT_ON_OPENPPL_SYMBOLS(name);
	if (memcmp(name, "random", 6) == 0)	{
		if (memcmp(name, "randomheartbeat", 15)==0 && strlen(name)==15)	{
			*result = randomheartbeat();
		}	else if (memcmp(name, "randomhand", 10)==0 && strlen(name)==10) {
			*result = randomhand();
		}	else if (memcmp(name, "randomround", 11)==0 && strlen(name)==11) {
			*result = randomround();
		}	else if (memcmp(name, "random", 6) == 0 && strlen(name)==6)	{
			*result = random();
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

CString CSymbolEngineRandom::SymbolsProvided() {
  return "Random "
    "random "
    "randomheartbeat randomhand randomround ";
}