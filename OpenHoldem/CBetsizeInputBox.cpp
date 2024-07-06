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
#include "CAutoplayerTrace.h"
#include "CCasinoInterface.h"
#include "CEngineContainer.h"
#include "CFunctionCollection.h"

#include "SwagAdjustment.h"
#include "CSymbolEngineHistory.h"
#include "CTablemap/CTablemap.h"
#include "OpenHoldem.h"
#include "WindowFunctions/window_functions.h"

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
  // Not really (0, 0), but (-1, -1), out of the screen
  POINT	point_null = { kUndefined, kUndefined };
  CString	swag_amt;

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
  SelectText();
  // First sleep(), THEN check for stolen focus, then act
  //  NOT the other way: http://www.maxinmontreal.com/forums/viewtopic.php?f=120&t=14791
  write_log(Preferences()->debug_autoplayer(), "[CBetsizeInputBox] Sleeping %dms.\n", Preferences()->swag_delay_1());
  Sleep(Preferences()->swag_delay_1());
  // Check for stolen , and thus misswag
  if (p_casino_interface->TableLostFocus()) {
    lost_focus = true;
  }
  Clear();
  write_log(Preferences()->debug_autoplayer(), "[CBetsizeInputBox] Sleeping %dms.\n", Preferences()->swag_delay_2());
  Sleep(Preferences()->swag_delay_2());
  // Check for stolen focus, and thus misswag
  if (p_casino_interface->TableLostFocus()) {
    lost_focus = true;
  }
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
  write_log(Preferences()->debug_autoplayer(), "[CBetsizeInputBox] Sleeping %dms.\n", Preferences()->swag_delay_3());
  Sleep(Preferences()->swag_delay_3());
  // Check for stolen focus, and thus misswag
  if (p_casino_interface->TableLostFocus()) {
    lost_focus = true;
  }
  // BET CONFIRMATION ACTION
  if (lost_focus) {
    // Print a very verbose warning in the log
    // Experience tell that beginenrs (and veterans) need that.
    write_log(k_always_log_errors, "[CBetsizeInputBox] WARNING! Betsizing failed because of lost focus.\n");
    write_log(k_always_log_errors, "[CBetsizeInputBox] Another window popped up and receives mouse and keyboard input.\n");
     write_log(k_always_log_errors, "[CBetsizeInputBox] This might be caused by bad casino, bad hopper or by user-interaction.\n");
  } else {
    if (p_tablemap->swagconfirmationmethod() == BETCONF_ENTER) {
      write_log(Preferences()->debug_autoplayer(), "[CBetsizeInputBox] Confirmation; calling keyboard.dll to press 'Enter'\n");
      p_casino_interface->SendKey(VK_RETURN);
    } else if (p_tablemap->swagconfirmationmethod() == BETCONF_CLICKBET
      && p_casino_interface->LogicalAutoplayerButton(k_autoplayer_function_raise)->IsClickable()) {
      write_log(Preferences()->debug_autoplayer(), "[CBetsizeInputBox] Bet Confirmation: Using raise button\n");
      if (p_tablemap->buttonclickmethod() == BUTTON_DOUBLECLICK) {
        p_casino_interface->ClickButtonSequence(k_autoplayer_function_raise,
          k_autoplayer_function_raise,
          k_double_click_delay);
      } else {
        p_casino_interface->LogicalAutoplayerButton(k_autoplayer_function_raise)->Click();
      }
    } else if (p_tablemap->swagconfirmationmethod() == BETCONF_NOTHING) {
    } else {
      write_log(Preferences()->debug_autoplayer(), "[CBetsizeInputBox] ...ending DoBetsize early (invalid betsizeconfirmationmethod).\n");
      write_log(Preferences()->debug_autoplayer(), "[CBetsizeInputBox] Valid options are: \"enter\", \"click bet\", \"nothing\"\n");
      return false;
    }
  }
  int betround = p_betround_calculator->betround();
  write_log(Preferences()->debug_autoplayer(), "[CBetsizeInputBox] ...ending DoBetsize, 'didbetsize' now: %d\n",
    p_engine_container->symbol_engine_history()->didswag(betround));
  return (!lost_focus); 
}

bool CBetsizeInputBox::GetI3EditRegion() {
  p_tablemap->GetTMRegion("i3edit", &_i3_edit_region);
  if ((_i3_edit_region.bottom < 0)
      || (_i3_edit_region.left < 0)
      || (_i3_edit_region.right < 0)
      || (_i3_edit_region.top < 0)) {
    return false;
  }
  return true;
}

bool CBetsizeInputBox::IsReadyToBeUsed() {
  if (p_tablemap->swagconfirmationmethod() == BETCONF_CLICKBET) {
    if (!p_casino_interface->BetsizeConfirmationButton()->IsClickable()) {
      return false;
    }
  }
  if (!p_tablemap->ItemExists("i3edit")) {
    return false;
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
    (theApp._dll_mouse_click_drag) (p_autoconnector->attached_hwnd(), _i3_edit_region);
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

void CBetsizeInputBox::Confirm() {
}

bool CBetsizeInputBox::VerifyEnteredBetsize() {
  // For future use
  return true;
}
