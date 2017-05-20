//******************************************************************************
//
// This file is part of the OpenHoldem project
//    Source code:           https://github.com/OpenHoldem/openholdembot/
//    Forums:                http://www.maxinmontreal.com/forums/index.php
//    Licensed under GPL v3: http://www.gnu.org/licenses/gpl.html
//
//******************************************************************************
//
// Purpose: starting and terminating bots automatically
//
//******************************************************************************

#include "stdafx.h"
#include "COpenHoldemStarter.h"

#include "CAutoConnector.h"
#include "CFilenames.h"
#include "CPreferences.h"
#include "CSharedMem.h"
#include "CSessionCounter.h"
#include "CSymbolEngineTime.h"
#include "OpenHoldem.h"

// For connection and popup handling
const int kMinNumberOfUnoccupiedBotsNeeded =   1;
const int kSecondsToWaitBeforeTermination  = 120;
const int kSecondsToWaitBeforeNextStart    =   5;

COpenHoldemStarter::COpenHoldemStarter() {
  time(&_starting_time_of_last_instance);
}

COpenHoldemStarter::~COpenHoldemStarter() {
}

void COpenHoldemStarter::StartNewInstanceIfNeeded() {
  assert(p_sharedmem != NULL);
  if (p_sharedmem->OpenHoldemProcessID() == 0) {
    write_log(k_always_log_errors, "WARNING! Auto-starter turned off, unavailable process ID\n");
    return;
  }
  if (p_sharedmem->NUnoccupiedBots() >= kMinNumberOfUnoccupiedBotsNeeded) {
    // Enough instance available for new connections / popup handling
    write_log(preferences.debug_autostarter(), "[COpenHoldemStarter] No bots needed, enough free instances.\n");
    return;
  }
  time_t current_time;
  time(&current_time);
  if (current_time - _starting_time_of_last_instance < kSecondsToWaitBeforeNextStart) {
    // Another instance got started only recently.
    // Grant it some seconds to show up
    // and don't flood the screen with new bots.
    return;
  }
  if (p_sharedmem->LowestConnectedSessionID() != p_sessioncounter->session_id()) {
    // Only one instance should handle auto-starting.
    // This might delay auto-starting until the first connection, which is OK.
    write_log(preferences.debug_autostarter(), "[COpenHoldemStarter] Not my business to start new instances.\n");
    return;
  }
  time(&_starting_time_of_last_instance);
  //! delay until next start
  // No error-checking, as Openholdem exists (at least when we started).
  // http://msdn.microsoft.com/en-us/library/windows/desktop/bb762153%28v=vs.85%29.aspx
  write_log(preferences.debug_autostarter(), "[COpenHoldemStarter] Starting new instance [%s].\n",
    p_filenames->ExecutableFilename());
  ShellExecute(
    NULL,               // Pointer to parent window; not needed
    "open",             // "open" == "execute" for an executable
    p_filenames->ExecutableFilename(),
    NULL, 		          // Parameters
    "",                 // Working directory
    SW_SHOWNORMAL);		  // Active window, default size
}

void COpenHoldemStarter::CloseThisInstanceIfNoLongerNeeded() {
  assert(p_sharedmem != NULL);
  if (p_sharedmem->OpenHoldemProcessID() == 0) {
    write_log(k_always_log_errors, "WARNING! Auto-shutdown turned off, unavailable process ID\n");
    return;
  }
  if (p_autoconnector->IsConnectedToAnything()) {
    write_log(preferences.debug_autostarter(), "[COpenHoldemStarter] Playing, therefore still needed.\n");
    // Instance needed for playing
    return;
  }
  if (p_sharedmem->NUnoccupiedBots() <= kMinNumberOfUnoccupiedBotsNeeded) {
    write_log(preferences.debug_autostarter(), "[COpenHoldemStarter] Needed for new connections.\n");
    // Instance needed for new connections / popup handling
    return;
  }
  if (p_symbol_engine_time->elapsedauto() < kSecondsToWaitBeforeTermination) {
    // Don't shut down immediately
    // Instance might be needed soon again
    write_log(preferences.debug_autostarter(), "[COpenHoldemStarter] Not waited long enough for shutdown.\n");
    return;
  }
  if (p_sharedmem->LowestUnconnectedSessionID() != p_sessioncounter->session_id()) {
    // Only one instance should tzerminate at a time
    // to keep one instance available
    write_log(preferences.debug_autostarter(), "[COpenHoldemStarter] Not my turn to shutdown.\n");
    write_log(preferences.debug_autostarter(), "[COpenHoldemStarter] Lowest free ID: %d\n", p_sharedmem->LowestUnconnectedSessionID());
    write_log(preferences.debug_autostarter(), "[COpenHoldemStarter] My ID: %d\n", p_sessioncounter->session_id());
    return;
  }
  write_log(preferences.debug_autostarter(), "[COpenHoldemStarter] Shutting down this instance.\n");
  PostMessage(theApp.m_pMainWnd->GetSafeHwnd(), WM_QUIT, NULL, NULL);
}
