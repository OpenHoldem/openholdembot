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
#include "CBetSlider.h"

#include "CAutoConnector.h"
#include "CAutoplayerTrace.h"
#include "CBetsizeInputBox.h"
#include "CCasinoInterface.h"
#include "CScraper.h"

#include "../CTablemap/CTablemap.h"
#include "OpenHoldem.h"

CBetSlider::CBetSlider() {
  ResetHandlePosition();
  // Not really (0, 0), but (-1, -1), out of the screen
  POINT	point_null = { kUndefined, kUndefined };
}

CBetSlider::~CBetSlider() {
}

void CBetSlider::SetHandlePosition(const POINT position) {
  _position.x = position.x;
  _position.y = position.y;
}

void CBetSlider::ResetHandlePosition() {
  _position.x = kUndefined;
  _position.y = kUndefined;
}

bool CBetSlider::SlideAllin() {
    if (!SlideIsPossible()) {
        write_log(Preferences()->debug_autoplayer(), "[BetSlider] ...ending DoSlider early (i3handle or i3slider are not defined in the tablemap)\n");
        return false;
    }
    write_log(Preferences()->debug_autoplayer(), "[BetSlider] Starting DoSlider...\n");
    if ((_position.x == kUndefined) || (_position.y == kUndefined)) {
        write_log(Preferences()->debug_autoplayer(), "[BetSlider] ...ending DoSlider early (handle not found - i3handle must use a transform that resolves to either 'handle' or 'true')\n");
        return false;
    }
    // Click and drag handle
    RECT drag_region;
    GetSliderRegions();
    bool is_horizontal_slider = ((_i3_slider.right - _i3_slider.left) > (_i3_slider.bottom - _i3_slider.top)) ? true : false;
    drag_region.left = _position.x + ((_i3_handle.right - _i3_handle.left) / 2);
    if (is_horizontal_slider) {
        drag_region.top = _position.y + ((_i3_handle.bottom - _i3_handle.top) / 2);
        drag_region.right = _position.x + (_i3_slider.right - _i3_slider.left);
        drag_region.bottom = drag_region.top;
    }
    else {
        drag_region.top = _position.y + (_i3_handle.bottom - _i3_handle.top) - (_i3_slider.bottom - _i3_slider.top);
        drag_region.bottom = _position.y + ((_i3_handle.bottom - _i3_handle.top) / 2);
        drag_region.right = drag_region.left;
    }

    HWND attached_window = p_autoconnector->attached_hwnd();
    bool		lost_focus = false;
    int swag_attempts = 0, betconf_attempts = 0;
    while (++swag_attempts == 1 || lost_focus) {
        lost_focus = false;
        write_log(Preferences()->debug_autoplayer(), "[BetSlider] Slider : Calling mouse.dll to jam from %d,%d to %d,%d\n", drag_region.left, drag_region.top, drag_region.right, drag_region.bottom);
        // Not really (0, 0), but (-1, -1), out of the screen
        POINT	point_null = { kUndefined, kUndefined };
        (theApp._dll_mouse_click_drag) (p_autoconnector->attached_hwnd(), drag_region, is_horizontal_slider);

        write_log(Preferences()->debug_autoplayer(), "[BetSlider] Sleeping %d ms\n.", Preferences()->swag_delay_3());
        Sleep(Preferences()->swag_delay_3());
        SetFocus(attached_window);
        SetForegroundWindow(attached_window);
        SetActiveWindow(attached_window);
        // Check for stolen , and thus misswag
        if (p_casino_interface->TableLostFocus()) {
            lost_focus = true;
        }
        if (lost_focus) {
            // Print a very verbose warning in the log
            // Experience tell that beginenrs (and veterans) need that.
            write_log(k_always_log_errors, "[BetSlider] WARNING! Betsizing swag failed because of lost focus.\n");
            write_log(k_always_log_errors, "[BetSlider] Another window popped up and receives mouse and keyboard input.\n");
            write_log(k_always_log_errors, "[BetSlider] This might be caused by bad casino, bad hopper or by user-interaction.\n");
            if (swag_attempts < 5)
                write_log(k_always_log_errors, "[BetSlider] Performing next Betsizing swag attempt: \"%i\"\n", swag_attempts + 1);
            else {
                write_log(k_always_log_errors, "[BetSlider] Reached max of \"%i\" Betsizing swag attempt.\n", swag_attempts);
                write_log(k_always_log_errors, "[BetSlider] Relaying now to backup action.\n");
                return false;
            }
        }
    }

    if (swag_attempts <= 5) {
        while (++betconf_attempts == 1 || lost_focus) {
            lost_focus = false;
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
                write_log(k_always_log_errors, "[BetSlider] WARNING! Betsizing confirmation failed because of lost focus.\n");
                write_log(k_always_log_errors, "[BetSlider] Another window popped up and receives mouse and keyboard input.\n");
                write_log(k_always_log_errors, "[BetSlider] This might be caused by bad casino, bad hopper or by user-interaction.\n");
                if (betconf_attempts < 5)
                    write_log(k_always_log_errors, "[BetSlider] Performing next Betsizing confirmation attempt: \"%i\"\n", betconf_attempts + 1);
                else {
                    write_log(k_always_log_errors, "[BetSlider] Reached max of \"%i\" Betsizing confirmation attempt.\n", betconf_attempts);
                    write_log(k_always_log_errors, "[BetSlider] Relaying now to backup action.\n");
                    return false;
                }
            }
            else {
                // Click confirmation button 
                if (!p_casino_interface->_betsize_input_box.Confirm()) {
                    write_log(k_always_log_errors, "[BetSlider] WARNING! Betsizing confirmation failed because of bad swag confirmation method, needed Click Bet method.\n");
                    write_log(k_always_log_errors, "[BetSlider] Relaying now to backup action.\n");
                    return false;
                }
                SetFocus(attached_window);
                SetForegroundWindow(attached_window);
                SetActiveWindow(attached_window);
                // Check for stolen focus, and thus misswag
                if (p_casino_interface->TableLostFocus()) {
                    lost_focus = true;
                }
                if (lost_focus) {
                    // Print a very verbose warning in the log
                    // Experience tell that beginenrs (and veterans) need that.
                    write_log(k_always_log_errors, "[BetSlider] WARNING! Betsizing confirmation failed because of lost focus.\n");
                    write_log(k_always_log_errors, "[BetSlider] Another window popped up and receives mouse and keyboard input.\n");
                    write_log(k_always_log_errors, "[BetSlider] This might be caused by bad casino, bad hopper or by user-interaction.\n");
                    if (betconf_attempts < 5)
                        write_log(k_always_log_errors, "[BetSlider] Performing next Betsizing confirmation attempt: \"%i\"\n", betconf_attempts + 1);
                    else {
                        write_log(k_always_log_errors, "[BetSlider] Reached max of \"%i\" Betsizing confirmation attempt.\n", betconf_attempts);
                        write_log(k_always_log_errors, "[BetSlider] Relaying now to backup action.\n");
                        return false;
                    }
                }
            }
        }
    }
    write_log(Preferences()->debug_autoplayer(), "[BetSlider] Jam complete: %d,%d,%d,%d\n", drag_region.left, drag_region.top, drag_region.right, drag_region.bottom);
    write_log(Preferences()->debug_autoplayer(), "[BetSlider] ...ending DoSlider.\n");
    return true;
}

bool CBetSlider::SlideBetsize(double betsize, double betsize_for_allin) {
    if (!SlideIsPossible()) {
        write_log(Preferences()->debug_autoplayer(), "[BetSlider] ...ending DoSlider early (i3handle or i3slider are not defined in the tablemap)\n");
        return false;
    }
    write_log(Preferences()->debug_autoplayer(), "[BetSlider] Starting DoSlider...\n");
    if ((_position.x == kUndefined) || (_position.y == kUndefined)) {
        write_log(Preferences()->debug_autoplayer(), "[BetSlider] ...ending DoSlider early (handle not found - i3handle must use a transform that resolves to either 'handle' or 'true')\n");
        return false;
    }
    // Click and drag handle
    RECT drag_region;
    GetSliderRegions();
    double slide_size_ratio = betsize / betsize_for_allin;
    bool is_horizontal_slider = ((_i3_slider.right - _i3_slider.left) > (_i3_slider.bottom - _i3_slider.top)) ? true : false;
    drag_region.left = _position.x + ((_i3_handle.right - _i3_handle.left) / 2);
    if (is_horizontal_slider) {
        drag_region.top = _position.y + ((_i3_handle.bottom - _i3_handle.top) / 2);
        drag_region.right = _position.x + ((_i3_slider.right - _i3_slider.left) * slide_size_ratio);
        drag_region.bottom = drag_region.top;
    }
    else {
        drag_region.top = _position.y + (_i3_handle.bottom - _i3_handle.top) - ((_i3_slider.bottom - _i3_slider.top) * slide_size_ratio);
        drag_region.bottom = _position.y + ((_i3_handle.bottom - _i3_handle.top) / 2);
        drag_region.right = drag_region.left;
    }

    HWND attached_window = p_autoconnector->attached_hwnd();
    bool		lost_focus = false;
    int swag_attempts = 0, betconf_attempts = 0;
    while (++swag_attempts == 1 || lost_focus) {
        lost_focus = false;
        write_log(Preferences()->debug_autoplayer(), "[BetSlider] Slider : Calling mouse.dll to jam from %d,%d to %d,%d\n", drag_region.left, drag_region.top, drag_region.right, drag_region.bottom);
        // Not really (0, 0), but (-1, -1), out of the screen
        POINT	point_null = { kUndefined, kUndefined };
        (theApp._dll_mouse_click_drag) (p_autoconnector->attached_hwnd(), drag_region, is_horizontal_slider);

        write_log(Preferences()->debug_autoplayer(), "[BetSlider] Sleeping %d ms\n.", Preferences()->swag_delay_3());
        Sleep(Preferences()->swag_delay_3());
        SetFocus(attached_window);
        SetForegroundWindow(attached_window);
        SetActiveWindow(attached_window);
        // Check for stolen , and thus misswag
        if (p_casino_interface->TableLostFocus()) {
            lost_focus = true;
        }
        if (lost_focus) {
            // Print a very verbose warning in the log
            // Experience tell that beginenrs (and veterans) need that.
            write_log(k_always_log_errors, "[BetSlider] WARNING! Betsizing swag failed because of lost focus.\n");
            write_log(k_always_log_errors, "[BetSlider] Another window popped up and receives mouse and keyboard input.\n");
            write_log(k_always_log_errors, "[BetSlider] This might be caused by bad casino, bad hopper or by user-interaction.\n");
            if (swag_attempts < 5)
                write_log(k_always_log_errors, "[BetSlider] Performing next Betsizing swag attempt: \"%i\"\n", swag_attempts + 1);
            else {
                write_log(k_always_log_errors, "[BetSlider] Reached max of \"%i\" Betsizing swag attempt.\n", swag_attempts);
                write_log(k_always_log_errors, "[BetSlider] Relaying now to backup action.\n");
                return false;
            }
        }
    }

    if (swag_attempts <= 5) {
        while (++betconf_attempts == 1 || lost_focus) {
            lost_focus = false;
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
                write_log(k_always_log_errors, "[BetSlider] WARNING! Betsizing confirmation failed because of lost focus.\n");
                write_log(k_always_log_errors, "[BetSlider] Another window popped up and receives mouse and keyboard input.\n");
                write_log(k_always_log_errors, "[BetSlider] This might be caused by bad casino, bad hopper or by user-interaction.\n");
                if (betconf_attempts < 5)
                    write_log(k_always_log_errors, "[BetSlider] Performing next Betsizing confirmation attempt: \"%i\"\n", betconf_attempts + 1);
                else {
                    write_log(k_always_log_errors, "[BetSlider] Reached max of \"%i\" Betsizing confirmation attempt.\n", betconf_attempts);
                    write_log(k_always_log_errors, "[BetSlider] Relaying now to backup action.\n");
                    return false;
                }
            }
            else {
                // Click confirmation button 
                if (!p_casino_interface->_betsize_input_box.Confirm()) {
                    write_log(k_always_log_errors, "[BetSlider] WARNING! Betsizing confirmation failed because of bad swag confirmation method, needed Click Bet method.\n");
                    write_log(k_always_log_errors, "[BetSlider] Relaying now to backup action.\n");
                    return false;
                }
                SetFocus(attached_window);
                SetForegroundWindow(attached_window);
                SetActiveWindow(attached_window);
                // Check for stolen focus, and thus misswag
                if (p_casino_interface->TableLostFocus()) {
                    lost_focus = true;
                }
                if (lost_focus) {
                    // Print a very verbose warning in the log
                    // Experience tell that beginenrs (and veterans) need that.
                    write_log(k_always_log_errors, "[BetSlider] WARNING! Betsizing confirmation failed because of lost focus.\n");
                    write_log(k_always_log_errors, "[BetSlider] Another window popped up and receives mouse and keyboard input.\n");
                    write_log(k_always_log_errors, "[BetSlider] This might be caused by bad casino, bad hopper or by user-interaction.\n");
                    if (betconf_attempts < 5)
                        write_log(k_always_log_errors, "[BetSlider] Performing next Betsizing confirmation attempt: \"%i\"\n", betconf_attempts + 1);
                    else {
                        write_log(k_always_log_errors, "[BetSlider] Reached max of \"%i\" Betsizing confirmation attempt.\n", betconf_attempts);
                        write_log(k_always_log_errors, "[BetSlider] Relaying now to backup action.\n");
                        return false;
                    }
                }
            }
        }
    }
    write_log(Preferences()->debug_autoplayer(), "[BetSlider] Jam complete: %d,%d,%d,%d\n", drag_region.left, drag_region.top, drag_region.right, drag_region.bottom);
    write_log(Preferences()->debug_autoplayer(), "[BetSlider] ...ending DoSlider.\n");
    return true;
}

bool CBetSlider::GetSliderRegions() {
  p_tablemap->GetTMRegion("i3slider", &_i3_slider);
  p_tablemap->GetTMRegion("i3handle", &_i3_handle);
  if ((_i3_slider.bottom < 0)
    || (_i3_slider.left < 0)
    || (_i3_slider.right < 0)
    || (_i3_slider.top < 0)) {
    return false;
  }
  if ((_i3_handle.bottom < 0)
    || (_i3_handle.left < 0)
    || (_i3_handle.right < 0)
    || (_i3_handle.top < 0)) {
    return false;
  }
  return true;
}

bool CBetSlider::SlideIsPossible() {
  // Required: betsize-confirmation-button, slider and handle
    if (!p_tablemap->ItemExists("i3slider")) {
        return false;
    }
    if (!p_tablemap->ItemExists("i3handle")) {
        return false;
    }
  if (p_tablemap->swagconfirmationmethod() == BETCONF_CLICKBET) {
      CString text;
      p_scraper->EvaluateRegion("i3handle", &text);
      // We perform now before "true" evaluation of i3handle to support casinos that need first to click Bet or Raise button first
      // before to slide the bet: https://www.maxinmontreal.com/forums/viewtopic.php?p=191837#p191837
    if (!p_casino_interface->BetsizeConfirmationButton()->IsClickable() || (text != "handle" && text != "true")) {
        p_casino_interface->BetsizeConfirmationButton()->Click();
        Sleep(Preferences()->swag_delay_3());
        p_scraper->EvaluateRegion("i3handle", &text);
        if (!p_casino_interface->BetsizeConfirmationButton()->IsClickable() || (text != "handle" && text != "true"))
            return false;
    }
  }
  if (!GetSliderRegions()) {
    return false;
  }
  return true;
}