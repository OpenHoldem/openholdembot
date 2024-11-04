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

#include "StdAfx.h"
#include "CAutoplayer.h"

#include <complex>
#include "AllinAdjustment.h"
#include "BetpotCalculations.h"
#include "BringKeyboard.h"
#include "CAutoplayerTrace.h"
#include "CAutoconnector.h"
#include "CAutoplayerFunctions.h"
#include "CCasinoInterface.h"
#include "CEngineContainer.h"
#include "CFlagsToolbar.h"
#include "CFunctionCollection.h"
#include "CHeartbeatThread.h"
#include "CIteratorThread.h"

#include "CRebuyManagement.h"
#include "CReplayFrame.h"
#include "CScraper.h"
#include "CStableFramesCounter.h"
#include "CSymbolEngineAutoplayer.h"
#include "CSymbolEngineCasino.h"
#include "CSymbolEngineChipAmounts.h"
#include "CSymbolEngineHistory.h"
#include "CSymbolEngineUserchair.h"
#include "CTableState.h"
#include "MainFrm.h"
#include "OpenHoldem.h"
#include "PokerChat.hpp"
#include "..\DLLs\StringFunctions_DLL\string_functions.h"
#include "CMyMutex.h"

CAutoplayer	*p_autoplayer = NULL;

CAutoplayer::CAutoplayer(void) {
	// Autoplayer is not enabled at startup.
	// We can't call EngageAutoplayer() here,
	// because the toolbar does not yet exist,
	// so we can't set the autoplayer-button.
	// However the toolbar is guaranteed to initialize correctly later.
	_autoplayer_engaged = false;
	action_sequence_needs_to_be_finished = false;
  _already_executing_allin_adjustment = false;
}


CAutoplayer::~CAutoplayer(void) {
	FinishActionSequenceIfNecessary();
}

void CAutoplayer::EngageAutoPlayerUponConnectionIfNeeded() {
  write_log(Preferences()->debug_alltherest(), "[CAutoplayer] location Johnny_5\n");
  if (p_autoconnector->IsConnectedToAnything() && Preferences()->engage_autoplayer()) {
		EngageAutoplayer(true);
	}
}

void CAutoplayer::PrepareActionSequence() {
	// This function should be called at the beginning of 
	// ExecutePrimaryFunctions and ExecuteSecondaryFunctions
	// which both will start exactly one action-sequence.
	//
	// At the end of an action sequence FinishAction() has to be called
	// to restore the mouse-position.
	//
	// Getting the cursor position has to be done AFTER  we got the mutex,
	// otherwise it could happen that other applications move the mouse
	// while we wait, leading to funny jumps when we "clean up".
	// http://www.maxinmontreal.com/forums/viewtopic.php?f=111&t=15324
	GetCursorPos(&cursor_position);
	window_with_focus = GetFocus();
	// We got the mutex and everything is prepared.
	// We now assume an action-sequence will be executed.
	// This makes cleanup simpler, as we now can handle it once,
	// instead of everywhere where an action can happen.
	action_sequence_needs_to_be_finished = true;
}

void CAutoplayer::FinishActionSequenceIfNecessary() {
	if (action_sequence_needs_to_be_finished) {
    if (p_engine_container->symbol_engine_casino()->ConnectedToOHReplay() && Preferences()->use_auto_replay()) {
      // Needs to be done very early
      // before we restore the focus
      p_casino_interface->PressTabToSwitchOHReplayToNextFrame();
    }
    // avoid multiple-clicks within a short frame of time
    p_stableframescounter->UpdateOnAutoplayerAction();
    if (p_engine_container->symbol_engine_casino()->ConnectedToOfflineSimulation() || Preferences()->restore_position_and_focus()) {
      // Restore mouse position and window focus
      // Only for simulations, not for real casinos (stealth).
		  // Restoring the original state has to be done in reversed order
		  SetFocus(window_with_focus);
		  SetCursorPos(cursor_position.x, cursor_position.y);
    }
		action_sequence_needs_to_be_finished = false;
	}
}

bool CAutoplayer::TimeToHandleSecondaryFormulas() {
	// Disabled (N-1) out of N heartbeats (3 out of 4 seconds)
	// to avoid multiple fast clicking on the sitin / sitout-button.
	// Contrary to the old f$play-function we use a heartbeat-counter
	// for that logic, as with a small scrape-delay it was
	// still possible to act multiple times within the same second.
	// Scrape_delay() should always be > 0, there's a check in the GUI.
	assert(Preferences()->scrape_delay() > 0);
  // We need milli-seconds here, just like in the preferences
  // and also floating-point-division (3000.0) before we truncate to integer.
  // Otherwise we get always 0 and a constant secondary formula
  // would be executed every heartbeat and block all primary ones.
	int hearbeats_to_pause = 3000.0 / Preferences()->scrape_delay();
	if  (hearbeats_to_pause < 1) {
 		hearbeats_to_pause = 1;
 	}
	write_log(Preferences()->debug_autoplayer(), "[AutoPlayer] TimeToHandleSecondaryFormulas() heartbeats to pause: %i\n",
		hearbeats_to_pause);
	bool act_this_heartbeat = ((p_heartbeat_thread->heartbeat_counter() % hearbeats_to_pause) == 0);
	write_log(Preferences()->debug_autoplayer(), "[AutoPlayer] TimeToHandleSecondaryFormulas() act_this_heartbeat: %s\n",
		Bool2CString(act_this_heartbeat));
	return act_this_heartbeat;
}

bool CAutoplayer::DoBetPot(void) {
	bool success = false;
	// Start with 2 * potsize, continue with lower betsizes, finally 1/4 pot
	for (int i=k_autoplayer_function_betpot_2_1; i<=k_autoplayer_function_betpot_1_4; i++) {
		if (p_autoplayer_functions->GetAutoplayerFunctionValue(i)) 	{
			write_log(Preferences()->debug_autoplayer(), 
        "[AutoPlayer] Function %s true.\n", k_standard_function_names[i]);
      if (ChangeBetPotActionToAllin(i)) {
        write_log(Preferences()->debug_autoplayer(), "[AutoPlayer] Adjusting bhetpot_X_Y to allin.\n");
        return DoAllin();
      }
			if (p_tablemap->betpotmethod() == BETPOT_RAISE)	{
				success = p_casino_interface->ClickButtonSequence(i, k_autoplayer_function_raise, Preferences()->swag_delay_3());
			}	else {
				// Default: click only betpot
				success = p_casino_interface->LogicalAutoplayerButton(i)->Click();
			}
      if (!success) {
        // Backup action> try yo swag betpot_X_Y
        double betpot_amount = BetsizeForBetpot(i);
        write_log(Preferences()->debug_autoplayer(), 
          "[AutoPlayer] Betpot with buttons failed\n");
        write_log(Preferences()->debug_autoplayer(), 
          "[AutoPlayer] Trying to swag %.2f instead\n", betpot_amount);
        success = p_casino_interface->EnterBetsize(betpot_amount);
      }
		}
    if (success) {
			// Register the action
			// Treat betpot like swagging, i.e. raising a user-defined amount
      p_engine_container->UpdateAfterAutoplayerAction(k_autoplayer_function_betsize);
      p_autoplayer_trace->Print(ActionConstantNames(i), kAlwaysLogAutoplayerFunctions);
			return true;
    }
		// Else continue trying with the next betpot function
	}
	// We didn't click any betpot-button
	return false;
}

bool CAutoplayer::AnyPrimaryFormulaTrue() { 
  // Some auto-player-functions MUST exist. If not then they get auto-generated.
  // Missing all autoplayer-functions would be a bug that leads to time-outs.
  assert(p_function_collection != NULL);
  assert(p_function_collection->Exists(k_standard_function_names[k_autoplayer_function_fold]));
	for (int i=k_autoplayer_function_beep; i<=k_autoplayer_function_fold; ++i)
	{
		double function_result = p_autoplayer_functions->GetAutoplayerFunctionValue(i);
		if (i == k_autoplayer_function_betsize)
		{
			write_log(Preferences()->debug_autoplayer(), "[AutoPlayer] AnyPrimaryFormulaTrue(): [%s]: %s\n",
				k_standard_function_names[i], Number2CString(function_result));
		}
		else
		{
			write_log(Preferences()->debug_autoplayer(), "[AutoPlayer] AnyPrimaryFormulaTrue(): [%s]: %s\n",
				k_standard_function_names[i], Bool2CString(function_result));
		}
		if (function_result)
		{
			write_log(Preferences()->debug_autoplayer(), "[AutoPlayer] AnyPrimaryFormulaTrue(): yes\n");
			return true;
		}
	}
	write_log(Preferences()->debug_autoplayer(), "[AutoPlayer] AnyPrimaryFormulaTrue(): no\n");
	return false;
}

bool CAutoplayer::AnySecondaryFormulaTrue() {
  // Considering all hopper functions
  // and the functions f$prefold and f$chat.
	for (int i=k_hopper_function_sitin; i<=k_standard_function_chat; ++i)	{
		bool function_result = p_autoplayer_functions->GetAutoplayerFunctionValue(i);
		write_log(Preferences()->debug_autoplayer(), "[AutoPlayer] AnySecondaryFormulaTrue(): [%s]: %s\n",
			k_standard_function_names[i], Bool2CString(function_result));
		if (function_result) {
			write_log(Preferences()->debug_autoplayer(), "[AutoPlayer] AnySecondaryFormulaTrue(): yes\n");
			return true;
		}
	}
	write_log(Preferences()->debug_autoplayer(), "[AutoPlayer] AnySecondaryFormulaTrue(): no\n");
	return false;
}

bool CAutoplayer::ExecutePrimaryFormulasIfNecessary() {
	write_log(Preferences()->debug_autoplayer(), "[AutoPlayer] ExecutePrimaryFormulasIfNecessary()\n");
	if (!AnyPrimaryFormulaTrue())	{
		write_log(Preferences()->debug_autoplayer(), "[AutoPlayer] No primary formula true. Nothing to do\n");
		return false;
	}
	// Execute beep (if necessary) independent of all other conditions (mutex, etc.)
	// and with autoplayer-actions.
	ExecuteBeep();
  assert(p_engine_container->symbol_engine_autoplayer()->isfinalanswer());
	assert(p_engine_container->symbol_engine_autoplayer()->ismyturn());
	// Precondition: my turn and isfinalanswer
	// So we have to take an action and are able to do so.
	// This function will ALWAYS try to click a button,
	// so we can handle the preparation once at the very beginning.
	CMyMutex mutex;
  if (!mutex.IsLocked()) {
		return false;
	}
	PrepareActionSequence();
	if (p_function_collection->EvaluateAutoplayerFunction(k_autoplayer_function_allin))	{
		if (DoAllin()) {
			return true;
		}
		// Else continue with swag and betpot
	}
	if (DoBetPot())	{
		return true;
	}
	if (DoBetsize()) {
		return true;
	}
	return ExecuteRaiseCallCheckFold();
}

bool CAutoplayer::ExecuteRaiseCallCheckFold() {
	write_log(Preferences()->debug_autoplayer(), 
    "[AutoPlayer] ExecuteRaiseCallCheckFold()\n");
  // Some auto-player-functions MUST exist. If not then they get auto-generated.
  // Missing all autoplayer-functions would be a bug that leads to time-outs.
  assert(p_function_collection != NULL);
  assert(p_function_collection->Exists(k_standard_function_names[k_autoplayer_function_fold]));
	for (int i=k_autoplayer_function_raise; i<=k_autoplayer_function_fold; i++)	{
    if ((i == k_autoplayer_function_check) && p_engine_container->symbol_engine_chip_amounts()->call() > 0) {
      write_log(k_always_log_errors, 
        "[AutoPlayer] WARNING! Can't execute f$check because there is a bet to call\n");
      continue;
    }
		if (p_function_collection->Evaluate(k_standard_function_names[i])) 	{
			if (p_casino_interface->LogicalAutoplayerButton(i)->Click()) 			{				
        p_engine_container->UpdateAfterAutoplayerAction(i);
        p_autoplayer_trace->Print(ActionConstantNames(i), kAlwaysLogAutoplayerFunctions);
				return true;
			}
		}
	}
	return false;
}

bool CAutoplayer::ExecuteBeep() {
	write_log(Preferences()->debug_autoplayer(), "[AutoPlayer] ExecuteBeep (if f$beep is true)\n");
	if (p_function_collection->Evaluate(k_standard_function_names[k_autoplayer_function_beep]))	{
		// Pitch standard: 440 Hz, 1/2 second
		// http://en.wikipedia.org/wiki/A440_%28pitch_standard%29
		Beep(440, 500);
	}
	return false;
}

bool CAutoplayer::ExecuteSecondaryFormulasIfNecessary() {
	int executed_secondary_function = kUndefined;
	if (!AnySecondaryFormulaTrue())	{
		write_log(Preferences()->debug_autoplayer(), "[AutoPlayer] All secondary formulas false.\n");
		write_log(Preferences()->debug_autoplayer(), "[AutoPlayer] Nothing to do.\n");
		return false;
	}
	CMyMutex mutex;
	if (!mutex.IsLocked()) {
		return false;
	}
	PrepareActionSequence();
	// Prefold, close, rebuy and chat work require different treatment,
	// more than just clicking a simple region...
	if (p_autoplayer_functions->GetAutoplayerFunctionValue(k_standard_function_prefold)) {
		// Prefold is technically more than a simple button-click,
		// because we need to create an autoplayer-trace afterwards.
		if (DoPrefold()) {
			executed_secondary_function = k_standard_function_prefold;
		}
	}	else if (p_autoplayer_functions->GetAutoplayerFunctionValue(k_hopper_function_close))	{
		// CloseWindow is "final".
		// We don't expect any further action after that
		// and can return immediatelly.
		if (p_casino_interface->CloseWindow()) {
			executed_secondary_function = k_hopper_function_close;
		}
	}	else if (p_autoplayer_functions->GetAutoplayerFunctionValue(k_standard_function_chat)) 	{
			if (DoChat()) {
				executed_secondary_function = k_standard_function_chat;
			}
	}
	// Otherwise: handle the simple simple button-click
	// k_hopper_function_sitin,
	// k_hopper_function_sitout,
	// k_hopper_function_leave,
  // k_hopper_function_rematch,
	// k_hopper_function_autopost,
	else {
    for (int i=k_hopper_function_sitin; i<=k_hopper_function_autopost; ++i)	{
  		if (p_autoplayer_functions->GetAutoplayerFunctionValue(i))	{
        if (p_casino_interface->LogicalAutoplayerButton(i)->Click()) {
          executed_secondary_function = i;
          break;
        }
			}
		}
	}
  // Move f$rebuy to the VERY end
  // so that an (always?) positive f$rebuy-function
  // with blocked ir non-existing script
  // can't block all the other hopper-functions.
  // http://www.maxinmontreal.com/forums/viewtopic.php?f=156&t=19953
  if ((executed_secondary_function == kUndefined)
    && (p_autoplayer_functions->GetAutoplayerFunctionValue(k_hopper_function_rebuy))) {
    // This requires an external script and some time.
    // No further actions here eihter, but immediate return.
    bool result = p_rebuymanagement->TryToRebuy();
    if (result) {
      executed_secondary_function = k_hopper_function_rebuy;
    }
  }
	if (executed_secondary_function != kUndefined) {
		FinishActionSequenceIfNecessary();
    if (Preferences()->log_hopper_functions()) {
      // No update after action required here,
      // as prefold already cares about that
      // and the other actions don't need it.
      p_autoplayer_trace->Print(ActionConstantNames(executed_secondary_function), false);
    }
		return true;
	}
	action_sequence_needs_to_be_finished = false;
	return false;
}

#define ENT CSLock lock(m_critsec);
	
void CAutoplayer::EngageAutoplayer(bool to_be_enabled_or_not) { 
	ENT 
	// Set correct button state
	// We have to be careful, as during initialization the GUI does not yet exist.
	assert(p_flags_toolbar != NULL);
	p_flags_toolbar->CheckButton(ID_MAIN_TOOLBAR_AUTOPLAYER, to_be_enabled_or_not);

	if (to_be_enabled_or_not) 
	{
		if (!p_function_collection->BotLogicCorrectlyParsed())
		{
			// Invalid formula
			// Can't autoplay
			to_be_enabled_or_not = false;
    }
	}
  if (to_be_enabled_or_not) {
    p_flags_toolbar->ResetButtonsOnAutoplayerOn();
  } else {
    p_flags_toolbar->ResetButtonsOnAutoplayerOff();
  }
	// Set value at the very last to be extra safe
	// and avoid problems with multiple threads
	// despite we use synchronization ;-)
	_autoplayer_engaged = to_be_enabled_or_not;
}

#undef ENT

bool CAutoplayer::DoChat(void) {
	assert(p_function_collection->EvaluateAutoplayerFunction(k_standard_function_chat) != 0);
	if (!IsChatAllowed())	{
		write_log(Preferences()->debug_autoplayer(), "[AutoPlayer] No chat, because chat turned off.\n");
		return false;
	}
	// Converting the result of the $chat-function to a string.
	// Will be ignored, if we already have an unhandled chat message.
	RegisterChatMessage(p_function_collection->EvaluateAutoplayerFunction(k_standard_function_chat));
	if (_the_chat_message == NULL) {
		write_log(Preferences()->debug_autoplayer(), "[AutoPlayer] No chat, because wrong chat code. Please read: ""Available chat messages"" .\n");
		return false ;
	}

	return p_casino_interface->EnterChatMessage(CString(_the_chat_message));
}

bool CAutoplayer::DoAllin(void) {
	bool success = false;
	write_log(Preferences()->debug_autoplayer(), "[AutoPlayer] Starting DoAllin...\n");

	int number_of_clicks = 1; // Default is: single click with the mouse
	if (p_tablemap->buttonclickmethod() == BUTTON_DOUBLECLICK) 	{
		number_of_clicks = 2;
	}
  // Trying to go allin using these 3 methods in the following order:
  //	1) click max (or allin), then optionally raise, depending on allinconfirmationmethod
  //	2) use the slider if it exists in the TM
	//	3) swag the balance 
	if (p_tablemap->allinconfirmationmethod() != 0)	{
		// Clicking max (or allin) and then raise
    success = p_casino_interface->ClickButtonSequence(k_autoplayer_function_allin,
      k_autoplayer_function_raise, Preferences()->swag_delay_3());
	}	else {
    // Clicking only max (or allin), but not raise
		success = p_casino_interface->LogicalAutoplayerButton(k_autoplayer_function_allin)->Click();
  }
	if (!success) {
    // Try the slider
		success = p_casino_interface->UseSliderForAllin();
  }
  if (!success) {
		// Last case: try to swagging the balance
		success = p_casino_interface->EnterBetsizeForAllin();
	}
	if (success) {
		// Not really necessary to register the action,
		// as the game is over and there is no doallin-symbol,
		// but it does not hurt to register it anyway.
    p_engine_container->UpdateAfterAutoplayerAction(k_autoplayer_function_allin);
    p_autoplayer_trace->Print(ActionConstantNames(k_autoplayer_function_allin), kAlwaysLogAutoplayerFunctions);
		return true;
	}
	return false;
}

void CAutoplayer::DoAutoplayer(void) {
	write_log(Preferences()->debug_autoplayer(), "[AutoPlayer] Starting Autoplayer cadence...\n");
  CheckBringKeyboard();
  write_log(Preferences()->debug_autoplayer(), "[AutoPlayer] Number of visible buttons: %d (%s)\n", 
		p_casino_interface->NumberOfVisibleAutoplayerButtons(),
		p_engine_container->symbol_engine_autoplayer()->GetFCKRAString());
	// Care about i86X regions first, because they are usually used 
	// to handle popups which occlude the table (unstable input)
	if (p_casino_interface->HandleInterfacebuttonsI86())	{
    write_log(Preferences()->debug_autoplayer(), "[AutoPlayer] Interface buttons (popups) handled\n");
    action_sequence_needs_to_be_finished = true;
	  goto AutoPlayerCleanupAndFinalization;
  }
  // Care about sitin, sitout, leave, etc.
  if (TimeToHandleSecondaryFormulas())	{
	  p_autoplayer_functions->CalcSecondaryFormulas();	  
    if (ExecuteSecondaryFormulasIfNecessary())	{
      write_log(Preferences()->debug_autoplayer(), "[AutoPlayer] Secondary formulas executed\n");
      goto AutoPlayerCleanupAndFinalization;
    } else {
      write_log(Preferences()->debug_autoplayer(), "[AutoPlayer] No secondary formulas to be handled.\n");
    }
  } else {
    write_log(Preferences()->debug_autoplayer(), "[AutoPlayer] Not executing secondary formulas this heartbeat\n");
	}
  if (!p_engine_container->symbol_engine_userchair()->userchair_confirmed()) {
    // Since OH 4.0.5 we support autoplaying immediatelly after connection
		// without the need to know the userchair to act on secondary formulas.
		// However: for primary formulas (f$alli, f$rais, etc.)
		// knowing the userchair (combination of cards and buttons) is a must.
		write_log(Preferences()->debug_autoplayer(), "[AutoPlayer] Skipping primary formulas because userchair unknown\n");
		goto AutoPlayerCleanupAndFinalization;
  }
	write_log(Preferences()->debug_autoplayer(), "[AutoPlayer] Going to evaluate primary formulas.\n");
	if (p_engine_container->symbol_engine_autoplayer()->isfinalanswer())	{
		p_autoplayer_functions->CalcPrimaryFormulas();
		ExecutePrimaryFormulasIfNecessary();
	}	else {
		write_log(Preferences()->debug_autoplayer(), "[AutoPlayer] No final answer, therefore not executing autoplayer-logic.\n");
	}
  // Gotos are usually considered bad code.
  // Here it simplifies the control-flow.
AutoPlayerCleanupAndFinalization:  
	FinishActionSequenceIfNecessary();
	write_log(Preferences()->debug_autoplayer(), "[AutoPlayer] ...ending Autoplayer cadence.\n");
}

bool CAutoplayer::DoBetsize() { 
  double betsize = p_function_collection->EvaluateAutoplayerFunction(k_autoplayer_function_betsize);
  double betsize_for_allin = p_table_state->User()->_bet.GetValue()
	  + p_table_state->User()->_balance.GetValue();
	if (betsize > 0) 	{
    if (!_already_executing_allin_adjustment) {
      // We have to prevent a potential endless loop here>
      // swag -> adjusted allin -> swag allin -> adjusted allin ...
      if (ChangeBetsizeToAllin(betsize)) {
        _already_executing_allin_adjustment = true;
        write_log(Preferences()->debug_autoplayer(), "[AutoPlayer] Adjusting betsize to allin.\n");
        bool success = DoAllin();
        _already_executing_allin_adjustment = false;
        return success;
      }
    }
		// Try the slider
		int success = p_casino_interface->UseSliderForBetsize(betsize, betsize_for_allin);
		if (!success) {
			success = p_casino_interface->EnterBetsize(betsize);
		}
		if (success) {
      write_log(Preferences()->debug_autoplayer(), "[AutoPlayer] betsize %.2f (adjusted) entered\n",
        betsize);
      p_engine_container->UpdateAfterAutoplayerAction(k_autoplayer_function_betsize);
      p_autoplayer_trace->Print(ActionConstantNames(k_autoplayer_function_betsize), kAlwaysLogAutoplayerFunctions);
			return true;
		}
    write_log(Preferences()->debug_autoplayer(), "[AutoPlayer] Failed to enter betsize %.2f\n",
      betsize);
    return false;
	}
	write_log(Preferences()->debug_autoplayer(), "[AutoPlayer] Don't f$betsize, because f$betsize evaluates to 0.\n");
	return false;
}

bool CAutoplayer::DoPrefold(void) {
	assert(p_function_collection->EvaluateAutoplayerFunction(k_standard_function_prefold) != 0);
	if (!p_table_state->User()->HasKnownCards()) {
		write_log(Preferences()->debug_autoplayer(), "[AutoPlayer] Prefold skipped. No known cards.\n");
		write_log(Preferences()->debug_autoplayer(), "[AutoPlayer] Smells like a bad f$prefold-function.\n");
	}
	if (p_casino_interface->LogicalAutoplayerButton(k_standard_function_prefold)->Click())	{
    p_engine_container->UpdateAfterAutoplayerAction(k_autoplayer_function_fold);
		write_log(Preferences()->debug_autoplayer(), "[AutoPlayer] Prefold executed.\n");
		return true;
	}
	return false;
}

