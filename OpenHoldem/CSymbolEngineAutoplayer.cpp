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
#include "CSymbolEngineAutoplayer.h"

#include "CAutoconnector.h"
#include "CAutoplayerFunctions.h"
#include "CCasinoInterface.h"
#include "CEngineContainer.h"
#include "CFunctionCollection.h"
#include "CIteratorThread.h"

#include "CScraper.h"  
#include "CStableFramesCounter.h"
#include "CSymbolengineDebug.h"
#include "CSymbolEngineTime.h"
#include "CSymbolEngineUserchair.h"
#include "CTableState.h"



CSymbolEngineAutoplayer::CSymbolEngineAutoplayer() {
	// The values of some symbol-engines depend on other engines.
	// As the engines get later called in the order of initialization
	// we assure correct ordering by checking if they are initialized.
  assert(p_engine_container->symbol_engine_tablelimits() != NULL);
  assert(p_engine_container->symbol_engine_time() != NULL);
  assert(p_engine_container->symbol_engine_userchair() != NULL);
}

CSymbolEngineAutoplayer::~CSymbolEngineAutoplayer() {
}

void CSymbolEngineAutoplayer::InitOnStartup() {
	_myturnbits    = 0;
	_issittingin   = false;
	_isautopost    = false;
	_isfinalanswer = false;
}


void CSymbolEngineAutoplayer::UpdateOnConnection() {
	_myturnbits      = 0;
	_issittingin     = false;
	_isautopost      = false;
	_isfinalanswer   = false;
	_last_myturnbits = 0;
}

void CSymbolEngineAutoplayer::UpdateOnHandreset() {
}

void CSymbolEngineAutoplayer::UpdateOnNewRound() {
}

void CSymbolEngineAutoplayer::UpdateOnMyTurn() {
}

void CSymbolEngineAutoplayer::UpdateOnHeartbeat() {
	_last_myturnbits = _myturnbits != 0;
	_myturnbits      = 0;
	_issittingin     = false;
	_isautopost      = false;
	_isfinalanswer   = false;
	CalculateMyTurnBits();
	CalculateSitInState();
	CalculateFinalAnswer();
}

void CSymbolEngineAutoplayer::CalculateMyTurnBits() {
	write_log(Preferences()->debug_symbolengine(), "[CSymbolEngineAutoplayer] myturnbits reset: %i\n", _myturnbits);
	for (int i=0; i<k_max_number_of_buttons; i++) {
		if (p_casino_interface->_technical_autoplayer_buttons[i].IsClickable()) {
      // myturnbits  
      // Since OH 7.7.2 in the form FCKRA 
      // like the butons in the GUI (F =lowest bit) 
      if (p_casino_interface->_technical_autoplayer_buttons[i].IsFold()) {
				_myturnbits |= kMyTurnBitsFold;
			}	else if (p_casino_interface->_technical_autoplayer_buttons[i].IsCall()) {
				_myturnbits |= kMyTurnBitsCall;
			}	else if (p_casino_interface->_technical_autoplayer_buttons[i].IsCheck()) {
				_myturnbits |= kMyTurnBitsCheck;
      }	else if (p_casino_interface->_technical_autoplayer_buttons[i].IsRaise()) {
				_myturnbits |= kMyTurnBitsRaise;
			}	else if (p_casino_interface->_technical_autoplayer_buttons[i].IsAllin()) {
				_myturnbits |= kMyTurnBitsAllin;
			}	else if (p_casino_interface->_technical_autoplayer_buttons[i].IsAutopost()) {
				_isautopost = true;
			}
		}
	}
	write_log(Preferences()->debug_symbolengine(), "[CSymbolEngineAutoplayer] myturnbits now: %i\n", _myturnbits);
}

void CSymbolEngineAutoplayer::CalculateSitInState() {
  for (int i=0; i<k_max_number_of_buttons; ++i) {
    if (p_casino_interface->_technical_autoplayer_buttons[i].IsSitin()) {
	    // Sitin-button found
      // We are sitting in if that button can NOT be clicked
	    _issittingin = !p_casino_interface->_technical_autoplayer_buttons[i].IsClickable();
	    return;
    } else if (p_casino_interface->_technical_autoplayer_buttons[i].IsSitout()) {
	    // Sitout-button found
      // We are sitting in if that button CAN be clicked
	    _issittingin = (p_casino_interface->_technical_autoplayer_buttons[i].IsClickable());
	    return;
    }
  }
}

bool CSymbolEngineAutoplayer::isfinaltable() {
  return p_table_state->_s_limit_info.is_final_table();
}

void CSymbolEngineAutoplayer::CalculateFinalAnswer() {
	// [IMPERFECT CODE] Updates stable-frames-counter as a side-effect
	// and should therefore only get called once per heartbeat.
	_isfinalanswer = true;
	// check factors that affect isFinalAnswer status
	if (p_iterator_thread->IteratorThreadWorking())	{
		write_log(Preferences()->debug_autoplayer(), "[AutoPlayer] Not Final Answer because iterator_thread still running\n");
		_isfinalanswer = false;
	}
	// Change from only requiring one visible button (OpenHoldem 2008-04-03)
	else if (p_casino_interface->NumberOfVisibleAutoplayerButtons() < k_min_buttons_needed_for_my_turn)	{
		write_log(Preferences()->debug_autoplayer(), "[AutoPlayer] Not Final Answer because too few buttons visible\n");
		write_log(Preferences()->debug_autoplayer(), "[AutoPlayer] Buttons visible: %i\n", p_casino_interface->NumberOfVisibleAutoplayerButtons());
		write_log(Preferences()->debug_autoplayer(), "[AutoPlayer] Either not your turn or problem with the tablemap\n");
		_isfinalanswer = false;
	}
  // if we are not playing (occluded?) 2008-03-25 Matrix
	else if (!p_table_state->User()->HasKnownCards())	{
		write_log(Preferences()->debug_autoplayer(), "[AutoPlayer] Not Final Answer because the user is \"not playing\"\n");
		write_log(Preferences()->debug_autoplayer(), "[AutoPlayer] Chair %d (locked) has no cards\n", p_engine_container->symbol_engine_userchair()->userchair());
		write_log(Preferences()->debug_autoplayer(), "[AutoPlayer] Possibly a tablemap-problem\n");
		_isfinalanswer = false;
	}
	//  Avoiding unnecessary calls to p_stableframescounter->UpdateNumberOfStableFrames(),
	if (_isfinalanswer)	{
		p_stableframescounter->UpdateNumberOfStableFrames();
	}
  write_log(Preferences()->debug_autoplayer(), "[AutoPlayer] Number of stable frames: % d\n", p_stableframescounter->NumberOfStableFrames());
  CString delay_function = k_standard_function_names[k_standard_function_delay];
  double desired_delay_in_milli_seconds = p_function_collection->Evaluate(delay_function, Preferences()->log_delay_function());
  p_engine_container->symbol_engine_debug()->SetValue(1, desired_delay_in_milli_seconds);
  double milli_seconds_since_my_turn = p_engine_container->symbol_engine_time()->elapsedmyturn() * 1000;
  p_engine_container->symbol_engine_debug()->SetValue(2, milli_seconds_since_my_turn);
  if (milli_seconds_since_my_turn < desired_delay_in_milli_seconds) {
    write_log(Preferences()->debug_autoplayer(), "[AutoPlayer] Not isfinalanswer because of f$delay\n");
    _isfinalanswer = false;
  }
	// If we don't have enough stable frames, or have not waited f$delay milliseconds, then return.
	if (p_stableframescounter->NumberOfStableFrames() < Preferences()->frame_delay()) {
		write_log(Preferences()->debug_autoplayer(), "[AutoPlayer] Not Final Answer because we don't have enough stable frames, or have not waited f$delay (=%.0f ms)\n", 
       p_function_collection->Evaluate(delay_function, Preferences()->log_delay_function()));
		_isfinalanswer = false;
	}
  p_engine_container->symbol_engine_debug()->SetValue(3, _isfinalanswer);
}

CString CSymbolEngineAutoplayer::GetFCKRAString()
{
	// Buttons visible (Fold, Call, Check, Raise, Allin)
	CString fckra_seen;
	fckra_seen.Format("%s%s%s%s%s",
    // According to the docu
    // myturnbits  
    // a bit-vector that tells you what buttons are visible 
    // bits 43210 correspond to buttons KARCF 
    // (check alli rais call fold). 
    // Bit 4 (check) was added in OpenHoldem 2.0, 
    // that’s why it is “out of order"
		(_myturnbits & kMyTurnBitsFold  ? "F" : "."),
    (_myturnbits & kMyTurnBitsCall  ? "C" : "."),
		(_myturnbits & kMyTurnBitsCheck ? "K" : "."),
		(_myturnbits & kMyTurnBitsRaise ? "R" : "."),
		(_myturnbits & kMyTurnBitsAllin ? "A" : "."));
	return fckra_seen;
}

bool CSymbolEngineAutoplayer::IsFirstHeartbeatOfMyTurn()
{
	return(ismyturn()
		&& (_last_myturnbits == 0));
}

bool CSymbolEngineAutoplayer::EvaluateSymbol(const CString name, double *result, bool log /* = false */)
{
  FAST_EXIT_ON_OPENPPL_SYMBOLS(name);
	if (memcmp(name, "is", 2)==0)
	{
		if (memcmp(name, "isfinaltable", 12)==0 && strlen(name)==12)	
		{
			*result = isfinaltable();
		}
		else if (memcmp(name, "ismyturn", 8)==0 && strlen(name)==8)		
		{
			*result = ismyturn();
		}
		else if (memcmp(name, "issittingin", 11)==0 && strlen(name)==11)	
		{
			*result = issittingin();
		}
		else if (memcmp(name, "issittingout", 12)==0 && strlen(name)==12)
		{
			*result = issittingout();
		}
		else if (memcmp(name, "isautopost", 10)==0 && strlen(name)==10)	
		{
			*result = isautopost();
		}
		else if (memcmp(name, "isfinalanswer", 13)==0 && strlen(name)==13)	
		{
			*result = isfinalanswer();
		}
		else
		{
			// Invalid symbol
			return false;
		}
		// Valid symbol
		return true;
	}
	else if (memcmp(name, "myturnbits", 10)==0 && strlen(name)==10)
	{
		*result = myturnbits();
		// Valid symbol
		return true;
	}

	// Symbol of a different symbol-engine
	return false;
}

CString CSymbolEngineAutoplayer::SymbolsProvided() {
  return "isfinaltable ismyturn issittingin issittingout isautopost "
    "isfinalanswer myturnbits ";
}