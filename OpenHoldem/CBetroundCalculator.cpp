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
#include "CBetroundCalculator.h"


#include "CScraper.h"
#include "CTableState.h"

#include "StringFunctions/string_functions.h"

CBetroundCalculator *p_betround_calculator = NULL;

CBetroundCalculator::CBetroundCalculator() {
	// Betround will be indirectly used 
	// when a formula gets loaded and verified.
	// (it gets used for symbol lookups).
	// So it should have a meaningful value,
	// even before we connect to a table.
	_betround = kBetroundPreflop;
	_betround_previous_heartbeat = kUndefined;
}

CBetroundCalculator::~CBetroundCalculator()
{}

void CBetroundCalculator::OnNewHeartbeat() {
	// Save old value first before before changing _betround
	// Otherwise the values will be identical all the time.
	_betround_previous_heartbeat = _betround;
	write_log(Preferences()->debug_alltherest(), 
    "[CBetroundCalculator] _betround_previous_heartbeat = %i\n",
		_betround_previous_heartbeat);
	// Betround is a very important prerequisite
	// to determine what symbols shall be calculated.
	// So we can hardly do it with a symbol-engine and do it here
	if (!p_scraper->IsCommonAnimation()) {
    if (p_table_state->RiverCard()->IsKnownCard()) {
			_betround = kBetroundRiver;
		} else if (p_table_state->TurnCard()->IsKnownCard()) {
			_betround = kBetroundTurn;
		} else if (p_table_state->CommonCards(2)->IsKnownCard() 
			  && p_table_state->CommonCards(1)->IsKnownCard() 
			  && p_table_state->CommonCards(0)->IsKnownCard()) {
			_betround = kBetroundFlop;
		}	else {
			_betround = kBetroundPreflop;
		}
	}	else {
		// There is a common card animation going on currently
		// so lets not try to determine the betround,
		// but if it's a new hand then lets default to pre-flop
		write_log(Preferences()->debug_alltherest(), "[CBetroundCalculator] Animation going on\n");
		if (_betround_previous_heartbeat == kUndefined)	{
			_betround = kBetroundPreflop;
		}
	}
	write_log(Preferences()->debug_alltherest(), "[CBetroundCalculator] _betround = %i\n",
		_betround);
}

bool CBetroundCalculator::IsNewBetround() { 
	bool result = (_betround != _betround_previous_heartbeat);
	write_log(Preferences()->debug_alltherest(), 
    "[CBetroundCalculator] IsNewBetround() = %s\n",
		Bool2CString(result));
	return result; 
}

int CBetroundCalculator::betround() {
  // Betround is very important and gets used as an index at various places
  // so we always make sure that it is not out of range.
  assert(_betround >= kBetroundPreflop);
  assert(_betround <= kBetroundRiver);
  return _betround;
}

int CBetroundCalculator::PreviousRound() {
  if (betround() == kBetroundPreflop) {
    // Keep the first betting round, 
    // as this function might be used for indexing
    return kBetroundPreflop;
  }
  return betround() - 1;
}