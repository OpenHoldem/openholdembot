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
#include "CBetsizeInputBox.h"

#include "CAutoConnector.h"
#include "CAutoplayer.h"
#include "CAutoplayerTrace.h"
#include "CCasinoInterface.h"
#include "CEngineContainer.h"
#include "CFunctionCollection.h"
#include "CAutoOcr.h"
#include "CScraper.h"

#include "SwagAdjustment.h"
#include "CSymbolEngineHistory.h"
#include "..\CTablemap\CTablemap.h"
#include "OpenHoldem.h"
#include "..\DLLs\WindowFunctions_DLL\window_functions.h"
#include "CMyMutex.h"

CBetsizeInputBox::CBetsizeInputBox() {
	// dummy point for mouse and keyboard DLL
	p_null.x = kUndefined;
	p_null.y = kUndefined;
}

CBetsizeInputBox::~CBetsizeInputBox() {
}

bool CBetsizeInputBox::EnterBetsize(double total_betsize_in_dollars) {
	POINT	  cur_pos = { 0 };
	bool		lost_focus = false;
	int swag_attempts = 0, betconf_attempts = 0;
	// Not really (0, 0), but (-1, -1), out of the screen
	POINT	point_null = { kUndefined, kUndefined };
	CString	swag_amt;
	HWND attached_window = p_autoconnector->attached_hwnd();
	HWND foreground_window = GetForegroundWindow();
	CString foreground_title(" ", MAX_WINDOW_TITLE);
	WinGetTitle(foreground_window, foreground_title.GetBuffer());
	write_log(Preferences()->debug_autoplayer(), "[CBetsizeInputBox] Foreground window: \"%s\"\n", foreground_title);
	write_log(Preferences()->debug_autoplayer(), "[CBetsizeInputBox] Starting DoBetsize...\n");
	if (!GetI3EditRegion()) {
		write_log(k_always_log_errors, "[CBetsizeInputBox] WARNING! i3edit undefined or out of range\n");
		return false;
	}
	// In some cases only call and fold are possible.
	// Then a betsize should be skipped.
	// We detect this situation by missing min-raise button.
	// No backup-action here:
	// OH-script doesn't provide that and OPPL eill do that automatically.
	// http://www.maxinmontreal.com/forums/viewtopic.php?f=117&t=18125
	if (!p_casino_interface->LogicalAutoplayerButton(k_autoplayer_function_raise)->IsClickable()) {
		write_log(Preferences()->debug_autoplayer(), "[CBetsizeInputBox] ...ending DoBetsize early (no (min-)raise possible).\n");
		return false;
	}
	// swag regions are hard coded as #3 for now, due to legacy WH standard
	if (!IsReadyToBeUsed()) {
		write_log(Preferences()->debug_autoplayer(), "[CBetsizeInputBox] ...ending DoBetsize early (no edit field or no i3button).\n");
		return false;
	}
	while (++swag_attempts == 1 || lost_focus) {
		lost_focus = false;
		SelectText();
		// First sleep(), THEN check for stolen focus, then act
		//  NOT the other way: http://www.maxinmontreal.com/forums/viewtopic.php?f=120&t=14791
		write_log(Preferences()->debug_autoplayer(), "[CBetsizeInputBox] Sleeping %dms.\n", Preferences()->swag_delay_1());
		Sleep(Preferences()->swag_delay_1()); // Select to delete autoplayer delay (default 400ms)
		SetFocus(attached_window);
		SetForegroundWindow(attached_window);
		SetActiveWindow(attached_window);
		// Check for stolen , and thus misswag
		if (p_casino_interface->TableLostFocus()) {
			lost_focus = true;
		}
		else {
			Clear();
			write_log(Preferences()->debug_autoplayer(), "[CBetsizeInputBox] Sleeping %dms.\n", Preferences()->swag_delay_2());
			Sleep(Preferences()->swag_delay_2()); // Delete to entry autoplayer delay (default 400ms)
			SetFocus(attached_window);
			SetForegroundWindow(attached_window);
			SetActiveWindow(attached_window);
			// Check for stolen focus, and thus misswag
			if (p_casino_interface->TableLostFocus()) {
				lost_focus = true;
			}
			else {
				// SWAG AMOUNT ENTRY
				double swag_adjusted = AdjustedBetsize(total_betsize_in_dollars);
				swag_amt = Number2CString(swag_adjusted);
				// Also adapt f$betsize for correct logging later-on
				// http://www.maxinmontreal.com/forums/viewtopic.php?f=156&t=18648
				p_function_collection->SetAutoplayerFunctionValue(k_autoplayer_function_betsize, swag_adjusted);
				write_log(Preferences()->debug_autoplayer(), "[CBetsizeInputBox] betsize (not adjusted): %.2f\n", total_betsize_in_dollars);
				write_log(Preferences()->debug_autoplayer(), "[CBetsizeInputBox] calling keyboard.dll to enter betsize (adjusted): %s %d,%d %d,%d\n",
					swag_amt, _i3_edit_region.left, _i3_edit_region.top, _i3_edit_region.right, _i3_edit_region.bottom);
				bool use_comma_instead_of_dot = p_tablemap->use_comma_instead_of_dot();
				(theApp._dll_keyboard_sendstring) (p_autoconnector->attached_hwnd(), _i3_edit_region,
					swag_amt, use_comma_instead_of_dot);
				// Check for stolen focus, and thus misswag
				if (p_casino_interface->TableLostFocus()) {
					lost_focus = true;
				}
			}
		}
		if (lost_focus) {
			// Print a very verbose warning in the log
			// Experience tell that beginenrs (and veterans) need that.
			write_log(k_always_log_errors, "[CBetsizeInputBox] WARNING! Betsizing swag failed because of lost focus.\n");
			write_log(k_always_log_errors, "[CBetsizeInputBox] Another window popped up and receives mouse and keyboard input.\n");
			write_log(k_always_log_errors, "[CBetsizeInputBox] This might be caused by bad casino, bad hopper or by user-interaction.\n");
			if (swag_attempts < 5)
				write_log(k_always_log_errors, "[CBetsizeInputBox] Performing next Betsizing swag attempt: \"%i\"\n", swag_attempts + 1);
			else {
				write_log(k_always_log_errors, "[CBetsizeInputBox] Reached max of \"%i\" Betsizing swag attempt.\n", swag_attempts);
				write_log(k_always_log_errors, "[CBetsizeInputBox] Relaying now to backup action.\n");
				break;
			}
		}
	}
	if (swag_attempts <= 5) {
		while (++betconf_attempts == 1 || lost_focus) {
			lost_focus = false;
			write_log(Preferences()->debug_autoplayer(), "[CBetsizeInputBox] Sleeping %dms.\n", Preferences()->swag_delay_3());
			Sleep(Preferences()->swag_delay_3()); // Entry to confirm autoplayer delay (default 700ms)
			SetFocus(attached_window);
			SetForegroundWindow(attached_window);
			SetActiveWindow(attached_window);
			// Check for stolen focus, and thus misswag
			if (p_casino_interface->TableLostFocus()) {
				lost_focus = true;
			}
			// BET CONFIRMATION ACTION
			if (lost_focus) {
				// Print a very verbose warning in the log
				// Experience tell that beginenrs (and veterans) need that.
				write_log(k_always_log_errors, "[CBetsizeInputBox] WARNING! Betsizing confirmation failed because of lost focus.\n");
				write_log(k_always_log_errors, "[CBetsizeInputBox] Another window popped up and receives mouse and keyboard input.\n");
				write_log(k_always_log_errors, "[CBetsizeInputBox] This might be caused by bad casino, bad hopper or by user-interaction.\n");
				if (betconf_attempts < 5)
					write_log(k_always_log_errors, "[CBetsizeInputBox] Performing next Betsizing confirmation attempt: \"%i\"\n", betconf_attempts + 1);
				else {
					write_log(k_always_log_errors, "[CBetsizeInputBox] Reached max of \"%i\" Betsizing confirmation attempt.\n", betconf_attempts);
					write_log(k_always_log_errors, "[CBetsizeInputBox] Relaying now to backup action.\n");
					break;
				}
			}
			else {
				if (p_tablemap->swagconfirmationmethod() == BETCONF_ENTER) {
					write_log(Preferences()->debug_autoplayer(), "[CBetsizeInputBox] Confirmation; calling keyboard.dll to press 'Enter'\n");
					p_casino_interface->SendKey(VK_RETURN);
					// Check for stolen focus, and thus misswag
					if (p_casino_interface->TableLostFocus()) {
						lost_focus = true;
					}
				}
				else if (p_tablemap->swagconfirmationmethod() == BETCONF_CLICKBET
					&& p_casino_interface->LogicalAutoplayerButton(k_autoplayer_function_raise)->IsClickable()) {
					write_log(Preferences()->debug_autoplayer(), "[CBetsizeInputBox] Bet Confirmation: Using raise button\n");
					if (p_tablemap->buttonclickmethod() == BUTTON_DOUBLECLICK) {
						p_casino_interface->ClickButtonSequence(k_autoplayer_function_raise,
							k_autoplayer_function_raise,
							k_double_click_delay);
						// Check for stolen focus, and thus misswag
						if (p_casino_interface->TableLostFocus()) {
							lost_focus = true;
						}
					}
					else {
						p_casino_interface->LogicalAutoplayerButton(k_autoplayer_function_raise)->Click();
						// Check for stolen focus, and thus misswag
						if (p_casino_interface->TableLostFocus()) {
							lost_focus = true;
						}
					}
				}
				else if (p_tablemap->swagconfirmationmethod() == BETCONF_NOTHING) {
				}
				else {
					write_log(Preferences()->debug_autoplayer(), "[CBetsizeInputBox] ...ending DoBetsize early (invalid betsizeconfirmationmethod).\n");
					write_log(Preferences()->debug_autoplayer(), "[CBetsizeInputBox] Valid options are: \"enter\", \"click bet\", \"nothing\"\n");
					return false;
				}
				if (lost_focus) {
					// Print a very verbose warning in the log
					// Experience tell that beginenrs (and veterans) need that.
					write_log(k_always_log_errors, "[CBetsizeInputBox] WARNING! Betsizing confirmation failed because of lost focus.\n");
					write_log(k_always_log_errors, "[CBetsizeInputBox] Another window popped up and receives mouse and keyboard input.\n");
					write_log(k_always_log_errors, "[CBetsizeInputBox] This might be caused by bad casino, bad hopper or by user-interaction.\n");
					if (betconf_attempts < 5)
						write_log(k_always_log_errors, "[CBetsizeInputBox] Performing next Betsizing confirmation attempt: \"%i\"\n", betconf_attempts + 1);
					else {
						write_log(k_always_log_errors, "[CBetsizeInputBox] Reached max of \"%i\" Betsizing confirmation attempt.\n", betconf_attempts);
						write_log(k_always_log_errors, "[CBetsizeInputBox] Relaying now to backup action.\n");
						break;
					}
				}
			}
		}
	}
	int betround = p_betround_calculator->betround();
	write_log(Preferences()->debug_autoplayer(), "[CBetsizeInputBox] ...ending DoBetsize, 'didbetsize' now: %d\n",
		p_engine_container->symbol_engine_history()->didswag(betround));
	return (!lost_focus);
}

bool CBetsizeInputBox::GetI3EditRegion() {
  CString	area_name;
  bool area_found = false;
  RMapCI	r_iter = p_tablemap->r$()->end();
  RECT zero_rect = RECT{ 0 };

  for (int i = 0; i < k_max_area_buttons_zone; i++) {
	  area_name.Format("area_buttons_zone%c", HexadecimalChar(i));
	  r_iter = p_tablemap->r$()->find(area_name);
	  if (r_iter != p_tablemap->r$()->end()) {
		  int r_width = r_iter->second.right - r_iter->second.left;
		  int r_height = r_iter->second.bottom - r_iter->second.top;
		  if (r_width > 0 && r_height > 0) {
			  area_found = true;
			  p_auto_ocr->GetDetectTemplateResult(r_iter->second.name, "editbet", &_i3_edit_region);
			  if (!EqualRect(&_i3_edit_region, &zero_rect))
				  return true;
		  }
	  }
  }

  if (!area_found) {
	  p_tablemap->GetTMRegion("i3edit", &_i3_edit_region);
	  if ((_i3_edit_region.bottom < 0)
		  || (_i3_edit_region.left < 0)
		  || (_i3_edit_region.right < 0)
		  || (_i3_edit_region.top < 0)) {
		  return false;
	  }
  }
  return true;
}

bool CBetsizeInputBox::IsReadyToBeUsed() {
  if (!p_tablemap->ItemExists("i3edit")) {
    return false;
  }
  if (!p_tablemap->ItemExists("i3state")) {
	  return false;
  }
  if (p_tablemap->swagconfirmationmethod() == BETCONF_CLICKBET) {
	  CString text;
	  p_scraper->EvaluateRegion("i3state", &text);
	  // We perform now before "true" evaluation of i3state to support casinos that need first to click Bet or Raise button first
	  // before to enter the bet: https://www.maxinmontreal.com/forums/viewtopic.php?p=191837#p191837
	  if (!p_casino_interface->BetsizeConfirmationButton()->IsClickable() || (text != "on" && text != "yes" && text != "checked" && text != "true" && text != "lit")) {
		  p_casino_interface->BetsizeConfirmationButton()->Click();
		  Sleep(Preferences()->swag_delay_3());
		  p_scraper->EvaluateRegion("i3state", &text);
		  if (!p_casino_interface->BetsizeConfirmationButton()->IsClickable() || (text != "on" && text != "yes" && text != "checked" && text != "true" && text != "lit"))
			  return false;
	  }
  }
  if (!GetI3EditRegion()) {
    return false;
  }
  return true;
}

void CBetsizeInputBox::SelectText() {
  if (p_tablemap->swagselectionmethod() == TEXTSEL_SINGLECLICK) {
   write_log(Preferences()->debug_autoplayer(), "[CBetsizeInputBox] Text selection; calling mouse.dll to single click: %d,%d %d,%d\n",
      _i3_edit_region.left, _i3_edit_region.top, _i3_edit_region.right, _i3_edit_region.bottom);
    (theApp._dll_mouse_click) (p_autoconnector->attached_hwnd(), _i3_edit_region, MouseLeft, 1);
  } else if (p_tablemap->swagselectionmethod() == TEXTSEL_DOUBLECLICK) {
   write_log(Preferences()->debug_autoplayer(), "[CBetsizeInputBox] Text selection; calling mouse.dll to double click: %d,%d %d,%d\n",
      _i3_edit_region.left, _i3_edit_region.top, _i3_edit_region.right, _i3_edit_region.bottom);
    (theApp._dll_mouse_click) (p_autoconnector->attached_hwnd(), _i3_edit_region, MouseLeft, 2);
  } else if (p_tablemap->swagselectionmethod() == TEXTSEL_TRIPLECLICK) {
   write_log(Preferences()->debug_autoplayer(), "[CBetsizeInputBox] Text selection; calling mouse.dll to triple click: %d,%d %d,%d\n",
      _i3_edit_region.left, _i3_edit_region.top, _i3_edit_region.right, _i3_edit_region.bottom);
    (theApp._dll_mouse_click) (p_autoconnector->attached_hwnd(), _i3_edit_region, MouseLeft, 3);
  } else if (p_tablemap->swagselectionmethod() == TEXTSEL_CLICKDRAG) {
   write_log(Preferences()->debug_autoplayer(), "[CBetsizeInputBox] Text selection; calling mouse.dll to click drag: %d,%d %d,%d\n",
      _i3_edit_region.left, _i3_edit_region.top, _i3_edit_region.right, _i3_edit_region.bottom);
    (theApp._dll_mouse_click_drag) (p_autoconnector->attached_hwnd(), _i3_edit_region, true);
  } else if (p_tablemap->swagselectionmethod() == TEXTSEL_NOTHING) {
    // Nothing to do
  } else {
   write_log(Preferences()->debug_autoplayer(), "[CBetsizeInputBox] ...ending DoBetsize early (invalid betsizeselectionmethod).\n");
    return;
  }
}

void CBetsizeInputBox::Clear() {
	if (p_tablemap->swagdeletionmethod() == TEXTDEL_DELETE) {
		write_log(Preferences()->debug_autoplayer(), "[CBetsizeInputBox] Text deletion; calling keyboard.dll to press 'delete'\n");
		p_casino_interface->SendKey(VK_DELETE);
	}
	else if (p_tablemap->swagdeletionmethod() == TEXTDEL_BACKSPACE) {
		write_log(Preferences()->debug_autoplayer(), "[CBetsizeInputBox] Text deletion; calling keyboard.dll to press 'backspace'\n");
		p_casino_interface->SendKey(VK_BACK);
	}
	else if (p_tablemap->swagdeletionmethod() == TEXTDEL_NOTHING) {
		// Nothing to do to delete the text
		// Once selected it will be overwritten.
	}
}

bool CBetsizeInputBox::Confirm() {
	if (p_tablemap->swagconfirmationmethod() == BETCONF_CLICKBET
		&& p_casino_interface->LogicalAutoplayerButton(k_autoplayer_function_raise)->IsClickable()) {
		write_log(Preferences()->debug_autoplayer(), "[CBetsizeInputBox] Bet Confirmation: Using raise button\n");
		if (p_tablemap->buttonclickmethod() == BUTTON_DOUBLECLICK) {
			p_casino_interface->ClickButtonSequence(k_autoplayer_function_raise,
				k_autoplayer_function_raise,
				k_double_click_delay);
		}
		else {
			p_casino_interface->LogicalAutoplayerButton(k_autoplayer_function_raise)->Click();
		}
		return true;
	}
	return false;
}

bool CBetsizeInputBox::VerifyEnteredBetsize() {
	// For future use
	return true;
}
