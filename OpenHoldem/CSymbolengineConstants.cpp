//*******************************************************************************
//
// This file is part of the OpenHoldem project
//   Download page:         http://code.google.com/p/openholdembot/
//   Forums:                http://www.maxinmontreal.com/forums/index.php
//   Licensed under GPL v3: http://www.gnu.org/licenses/gpl.html
//
//*******************************************************************************
//
// Purpose:Constants symbols for more verbosity
//   like ace, ten, preflop, diamonds.
//
//*******************************************************************************

#include "stdafx.h"
#include "CSymbolEngineConstants.h"
#include "MagicNumbers.h"

CSymbolEngineConstants *p_symbol_engine_constants = NULL;

CSymbolEngineConstants::CSymbolEngineConstants() {
	// The values of some symbol-engines depend on other engines.
	// As the engines get later called in the order of initialization
	// we assure correct ordering by checking if they are initialized.
	//
	// This engine does not use any other engines.
}

CSymbolEngineConstants::~CSymbolEngineConstants() {
}

void CSymbolEngineConstants::InitOnStartup() {
	UpdateOnConnection();
}

void CSymbolEngineConstants::UpdateOnConnection() {
}

void CSymbolEngineConstants::UpdateOnHandreset() {
}

void CSymbolEngineConstants::UpdateOnNewRound() {
}

void CSymbolEngineConstants::UpdateOnMyTurn() {
}

void CSymbolEngineConstants::UpdateOnHeartbeat() {
}

bool CSymbolEngineConstants::EvaluateSymbol(const CString name, double *result, bool log /* = false */) {
  FAST_EXIT_ON_OPENPPL_SYMBOLS(name);
  if (name.GetLength() > 8) {
    // Another fast exit
    // Symbol of a different symbol-engine
    return false;
  }
  // Card ranks 
  // (OpenHoldem, not StdDeck. OH counts 2..14, StdDeck counts 0..12)
	if (name == "ace") {
    *result = k_rank_ace;
		return true;
	}
  if (name == "king") {
    *result = k_rank_king;
    return true;
  }
  if (name == "queen") {
    *result = k_rank_queen;
    return true;
  }
  if (name == "jack") {
    *result = k_rank_jack;
    return true;
  }
  if (name == "ten") {
    *result = k_rank_ten;
    return true;
  }
  // Betting rounds
  if (name == "preflop") {
    *result = kBetroundPreflop;
    return true;
  }
  if (name == "flop") {
    *result = kBetroundFlop;
    return true;
  }
  if (name == "turn") {
    *result = kBetroundTurn;
    return true;
  }

  if (name == "river") {
    *result = kBetroundRiver;
    return true;
  }
  // Card suits
  if (name == "clubs") {
    *result = Suit_CLUBS;
    return true;
  }
  if (name == "diamonds") {
    *result = Suit_DIAMONDS;
    return true;
  }
  if (name == "hearts") {
    *result = Suit_HEARTS;
    return true;
  }
  if (name == "spades") {
    *result = Suit_SPADES;
    return true;
  }
  if (name == "true") {
    *result = true;
    return true;
  }
  if (name == "false") {
    *result = false;
    return true;
  }
	// Symbol of a different symbol-engine
	return false;
}

CString CSymbolEngineConstants::SymbolsProvided() {
  return "ace king queen jack ten "
    "preflop flop turn river " 
    "clubs diamonds hearts spades "
    "true false ";
}
