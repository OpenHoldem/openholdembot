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
#include "CPopupHandler.h"


#include "CSessionCounter.h"
#include "CSharedMem.h"
#include "WindowFunctions/window_functions.h"

// Only for testing
#undef MESSAGEBOX_BEFORE_MINIMIZING

CPopupHandler *p_popup_handler = NULL;

CPopupHandler::CPopupHandler() {
	if (Preferences()->popup_blocker() != k_popup_disabled) {
		// Initial minimization of all windows
		// This should be done exactly once, by instance 0
		if (p_sessioncounter->session_id() == 0) {
			write_log(Preferences()->debug_popup_blocker(), "[CPopupHandler] Minimizing all windows on startup (session ID 0).\n");
			MinimizeAllOnstartup();
		}
	}
}

CPopupHandler::~CPopupHandler() {
}

void CPopupHandler::MinimizeAllOnstartup() {
	write_log(Preferences()->debug_popup_blocker(), "[CPopupHandler] MinimizeAllOnstartup()\n");
	HandleAllWindows(false);
}

void CPopupHandler::HandleAllWindows() {
	write_log(Preferences()->debug_popup_blocker(), "[CPopupHandler] HandleAllWindows()\n");
	int popup_blocker_method = Preferences()->popup_blocker();
	if (popup_blocker_method == k_popup_minimize) {
		HandleAllWindows(false);
	}
	else if (popup_blocker_method == k_popup_kill) {
		HandleAllWindows(true);
	}
	// else: disabled
}

BOOL CALLBACK EnumProcPotentialPopups(HWND hwnd, LPARAM lparam)  {
	bool hard_kill = lparam != 0;
	p_popup_handler->HandlePotentialPopup(hwnd, hard_kill);
	return true;  // keep processing through entire list of windows
}

void CPopupHandler::HandleAllWindows(bool hard_kill) {
	// Use the auto-connectors list of window-candidates
	write_log(Preferences()->debug_popup_blocker(), "[CPopupHandler] HandleAllWindows()\n");
	EnumWindows(EnumProcPotentialPopups, LPARAM(hard_kill));
}

bool CPopupHandler::WinIsOpenHoldem(HWND window) {
  DWORD PID;
  if (!GetWindowThreadProcessId(window, &PID)) {
    return false;
  }
  return (p_sharedmem->IsAnyOpenHoldemProcess(PID));
}

void CPopupHandler::HandlePotentialPopup(HWND potential_popup, bool hard_kill) {
	char title[MAX_WINDOW_TITLE];
	GetWindowText(potential_popup, title, MAX_WINDOW_TITLE);
	write_log(Preferences()->debug_popup_blocker(), "[CPopupHandler] Going to handle [%d] [%s]\n",
			potential_popup, title);

	// First: ignore all harmless windows
	if (!IsWindow(potential_popup))	{
		write_log(Preferences()->debug_popup_blocker(), "[CPopupHandler] Not a window\n");
		return;
	}
	if (!IsWindowVisible(potential_popup)) {
		write_log(Preferences()->debug_popup_blocker(), "[CPopupHandler] Window is invisible\n");
		return;
	}
	if (WinIsMinimized(potential_popup)) {
		write_log(Preferences()->debug_popup_blocker(), "[CPopupHandler] Window is minimized\n");
		return;
	}
	if (WinIsZeroSized(potential_popup)) {
		write_log(Preferences()->debug_popup_blocker(), "[CPopupHandler] Window is zero-sized\n");
		return;
	}
	if (WinIsOutOfScreen(potential_popup)) {
		write_log(Preferences()->debug_popup_blocker(), "[CPopupHandler] Window is out of screen\n");
		return;
	}
	// Second: ignore the good windows
	//   and always present windows (to avoid flickering)
	// This includes:
	//   * program manager (desktop) 
	//   * task-bar with Start-button
	//   * task manager to be able to kill the popup-blocker if necessary ;-)
	if (WinIsDesktop(potential_popup)) {
		write_log(Preferences()->debug_popup_blocker(), "[CPopupHandler] Window is desktop\n");
		return;
	}
	if (WinIsTaskbar(potential_popup)) {
		write_log(Preferences()->debug_popup_blocker(), "[CPopupHandler] Window is task-bar with Start-button\n");
		return;
	}
	if (WinIsTaskManager(potential_popup)) {
		write_log(Preferences()->debug_popup_blocker(), "[CPopupHandler] Window is task-manager\n");
		return;
	}
	if (WinIsProgramManager(potential_popup))	{
		write_log(Preferences()->debug_popup_blocker(), "[CPopupHandler] Window is program manager\n");
		return;
	}
	if (p_sharedmem->PokerWindowAttached(potential_popup)) {
		write_log(Preferences()->debug_popup_blocker(), "[CPopupHandler] Window is a served poker table\n");
		return;
	}
	if (WinIsOpenHoldem(potential_popup)) {
		write_log(Preferences()->debug_popup_blocker(), "[CPopupHandler] Window belongs to OpenHoldem\n");
		return;
	}
	if (WinIsBring(potential_popup)) {
		write_log(Preferences()->debug_popup_blocker(), "[CPopupHandler] Window belongs to Bring\n");
		return;
	}
  GetWindowText(potential_popup, title, MAX_WINDOW_TITLE);
	// Minimize or kill the remaining ones
	if (hard_kill) {
		// CloseWindow():
		// http://msdn.microsoft.com/en-us/library/windows/desktop/ms632678%28v=vs.85%29.aspx
		// DestroyWindow() can't be used:
		// http://msdn.microsoft.com/en-us/library/windows/desktop/ms632682%28v=vs.85%29.aspx
		CloseWindow(potential_popup);
		write_log(Preferences()->debug_popup_blocker(), "[CPopupHandler] Killed [%d] [%s]\n",
			potential_popup, title);
	}	else {
#ifdef MESSAGEBOX_BEFORE_MINIMIZING
		message.format("going to minimize window [%i]\n[%s]",
			potential_popup, title);
		messagebox(0, message, "popup blocker", 0);
#endif
		MinimizeWindow(potential_popup);
		write_log(Preferences()->debug_popup_blocker(), "[CPopupHandler] Minimized [%d] [%s]\n",
			potential_popup, title);
	}
}
