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
#include "CSymbolEngineWeightedRange.h"

CSymbolEngineWeightedRange::CSymbolEngineWeightedRange() {
	// The values of some symbol-engines depend on other engines.
	// As the engines get later called in the order of initialization
	// we assure correct ordering by checking if they are initialized.
	//
	// This engine does not use any other engines.
}

CSymbolEngineWeightedRange::~CSymbolEngineWeightedRange() {
}

void CSymbolEngineWeightedRange::InitOnStartup() {
	UpdateOnConnection();
}

void CSymbolEngineWeightedRange::UpdateOnConnection() {
	UpdateOnHandreset();
}

void CSymbolEngineWeightedRange::UpdateOnHandreset() {
	for (int i = 0; i<kMaxNumberOfPlayers; i++) {
		_wrange[i] = "";
	}
}

void CSymbolEngineWeightedRange::UpdateOnNewRound() {
}

void CSymbolEngineWeightedRange::UpdateOnMyTurn() {
}

void CSymbolEngineWeightedRange::UpdateOnHeartbeat() {
}

void CSymbolEngineWeightedRange::AssignWeightedRanges() {
}

bool CSymbolEngineWeightedRange::EvaluateSymbol(const CString name, CString *result, bool log /* = false */) {
  FAST_EXIT_ON_OPENPPL_SYMBOLS(name);
	if (memcmp(name, "wrange", 6)==0 && strlen(name)==7)	{
		*result = wrange(RightDigitCharacterToNumber(name));
		return true;
	}
	// Symbol of a different symbol-engine
	return false;
}

CString CSymbolEngineWeightedRange::SymbolsProvided() {
	CString list = "";
	list += RangeOfSymbols("wrange%i", kFirstChair, kLastChair);
  return list;
}
