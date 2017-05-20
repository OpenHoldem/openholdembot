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
#include "CHandHistoryAction.h"

#include "CPreferences.h"
#include "CScraper.h"
#include "MagicNumbers.h"

CHandHistoryAction *p_handhistory_action = NULL;

CHandHistoryAction::CHandHistoryAction() {
	// The values of some symbol-engines depend on other engines.
	// As the engines get later called in the order of initialization
	// we assure correct ordering by checking if they are initialized.
	//
	// This engine does not use any other engines.
}

CHandHistoryAction::~CHandHistoryAction() {
}

void CHandHistoryAction::InitOnStartup() {
}

void CHandHistoryAction::UpdateOnConnection() {
}

void CHandHistoryAction::UpdateOnHandreset() {
}

void CHandHistoryAction::UpdateOnNewRound() {
}

void CHandHistoryAction::UpdateOnMyTurn() {
}

void CHandHistoryAction::UpdateOnHeartbeat() {
}

bool CHandHistoryAction::EvaluateSymbol(const CString name, double *result, bool log /* = false */) {
  // No symbols provided
	return false;
}

CString CHandHistoryAction::SymbolsProvided() {
  // No symbols provided
  return " ";
}
	