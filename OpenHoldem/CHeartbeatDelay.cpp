//******************************************************************************
//
// This file is part of the OpenHoldem project
//    Source code:           https://github.com/OpenHoldem/openholdembot/
//    Forums:                http://www.maxinmontreal.com/forums/index.php
//    Licensed under GPL v3: http://www.gnu.org/licenses/gpl.html
//
//******************************************************************************
//
// Purpose: Flexible delay for the heartbeat iteration
//
//******************************************************************************

#include "stdafx.h"
#include "CHeartbeatDelay.h"

#include "CAutoconnector.h"
#include "CCasinoInterface.h"
#include "CEngineContainer.h"
#include "CHandresetDetector.h"

#include "CSessionCounter.h"
#include "CSymbolEngineActiveDealtPlaying.h"
#include "CSymbolEngineCasino.h"
#include "CSymbolEngineChecksBetsFolds.h"
#include "CSymbolEngineIsRush.h"
#include "CSymbolEngineIsTournament.h"
#include "CSymbolEngineTime.h"
#include "CTableState.h"

CHeartbeatDelay::CHeartbeatDelay() {
}

CHeartbeatDelay::~CHeartbeatDelay(){
}

void CHeartbeatDelay::FlexibleSleep() {
  double default_heartbeat_delay = Preferences()->scrape_delay();
  double sleeping_factor = SleepingFactor();
  if (p_engine_container->symbol_engine_casino()->ConnectedToManualMode()) {
    // Don't become too laggy at ManualMode,
    // response-time to euser is more important
    // than "performance", usually single-tabling.
    sleeping_factor = MIN(sleeping_factor, 2);
  }
  double modified_heartbeat_delay = default_heartbeat_delay * sleeping_factor;
  write_log(Preferences()->debug_heartbeat(), "[HeartBeatThread] default delay   %.3f ms.\n", default_heartbeat_delay);
  write_log(Preferences()->debug_heartbeat(), "[HeartBeatThread] sleeping factor %.3f .\n", sleeping_factor);
  write_log(Preferences()->debug_heartbeat(), "[HeartBeatThread] modified delay  %.3f ms.\n", modified_heartbeat_delay);
  Sleep(modified_heartbeat_delay);
}

double CHeartbeatDelay::SleepingFactor() {
  write_log(Preferences()->debug_alltherest(), "[CHeartbeatDelay] location Johnny_A\n");
  if (!p_autoconnector->IsConnectedToAnything()) {
    // Keep heartbeat_delay as is
    // We want fast auto-connects 
    // and the auto-connector is extremely optimized.
    return 1.0;
  }	else if (!p_engine_container->symbol_engine_userchair()->userchair_confirmed()) {
    return SleepingFactorNotSeated();
  } else if (p_table_state->User()->HasKnownCards()) {
    // We hold cards -- the CRITICAL case
    // Handle it early in case of any bad information or bad logic
    // that might mix things up
    if (p_casino_interface->IsMyTurn()) {
		  // Playing and my turn
		  // Stable frames expected
		  // Shorter reaction times desired
      return 0.5; 
	  } else {
      return SleepingFactorPlayingNotMyTurn();
    }
  } else if (p_engine_container->symbol_engine_active_dealt_playing()->nopponentsseated() == 0) {
    // Userchair known, but no opponents seated
    // Might take some time until game continues
    // Quite non-critical
    return 4.0;
  } else if (p_engine_container->symbol_engine_active_dealt_playing()->nopponentsactive() == 0) {
    // Userchair known, opponent(s) seated but sitting out
    // Game might pause a bit, but opponent could sit in again
    // No action expected, but slightly more critical than the case above.
    return 3.0;
  }
  // Folded
  return SleepingFactorActiveButFolded();
}

double CHeartbeatDelay::SleepingFactorNotSeated() {
  // Not (yet) seated
  // Probably not much critical work to be done.
  if (p_handreset_detector->hands_played() > 1) { 
    if (p_engine_container->symbol_engine_time()->elapsedauto() > 90) {
      // Tournament finished or cash-game stood-up, extremely non-critical
      // We continue with very low priority just to handle f$close, etc.
      return 5.0;
    } else {
      // Tournament finished or cash-game stood-up (probably),
      // but last hand ended only recently.
      // Be more conservative
      return 2.0;
    }
  } else { 
    // if (p_handreset_detector->hands_played() <= 1) {
    // Game not yet started
    if (p_engine_container->symbol_engine_istournament()->istournament()) {
      // Tournament
      // Might take some time to fill up, non-critical
      return 3.0;
    } else { 
      // Cash-game without confirmed user-chair
      // Might start soon
      return 2.0;
    }
  }
}

double CHeartbeatDelay::SleepingFactorPlayingNotMyTurn() {
  if (p_engine_container->symbol_engine_time()->elapsedauto() < p_engine_container->symbol_engine_active_dealt_playing()->nopponentsplaying()) {
    // Short after autoplayer-action
    // Will take some time until it is our turn again.
    // Slow down a little bit, depending on the number of opponents
    int nopponentsplaying = p_engine_container->symbol_engine_active_dealt_playing()->nopponentsplaying();
    if (nopponentsplaying >= 1) {
      return (nopponentsplaying + 1);
    }
    else {
      return 2.0;
    }
  } else if ((p_table_state->User()->_bet.GetValue() > 0) && (p_engine_container->symbol_engine_checks_bets_folds()->nopponentschecking() >= 1)) {
    // At least one opponent has to decide about raise / call / fold
    // Slow down a little bit.
    return (1 + 0.5 * p_engine_container->symbol_engine_checks_bets_folds()->nopponentschecking());
  }
  // Else: my turn expected soon
  // keep default value
  return 1.0;
}

double CHeartbeatDelay::SleepingFactorActiveButFolded() {
  if (p_engine_container->symbol_engine_isrush()->isrush()) {
    // Folded
    // New hand starts soon
    // Don't change delay
    return 1.0;
  } else if (p_engine_container->symbol_engine_active_dealt_playing()->nopponentsplaying() >= 3) {
    // Folded
    // Multiway, not participating.
    // Hand will continue for some time.
    return 3.0;
  } else if (p_engine_container->symbol_engine_active_dealt_playing()->nopponentsplaying() >= 2) {
    // Folded
    // Headsup, not participating.
    if (p_engine_container->symbol_engine_checks_bets_folds()->nopponentschecking() >= 2) {
      // At least 2 players did not yet act (or check)
      // Hand will continue for some time    }
      return 2.5;
    }
    else if (BETROUND <= kBetroundTurn) {
      // Not yet river
      // Hand will probably continue for some time
      return 2.0;
    } else {
      // Heads-up at the river, at least one player betting
      // Hand might be over soon.
      return 1.5;
    }
  } else {
    // User seated, seated and active non-playing opponents
    // New hand (hand-reset) expected
    // Critical phase, don~t change anything
    return 1.0;
  }
}