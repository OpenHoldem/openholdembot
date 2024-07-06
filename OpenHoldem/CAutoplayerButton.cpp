//******************************************************************************
//
// This file is part of the OpenHoldem project
//    Source code:           https://github.com/OpenHoldem/openholdembot/
//    Forums:                http://www.maxinmontreal.com/forums/index.php
//    Licensed under GPL v3: http://www.gnu.org/licenses/gpl.html
//
//******************************************************************************
//
// Purpose: Clean interface for scraper, casino-interface amd autoplayer
//
//******************************************************************************

#include "stdafx.h"
#include "CAutoplayerButton.h"

#include "CCasinoHotkey.h"
#include "CCasinoInterface.h"

#include "CStringMatch.h"
#include "CTablemap/CTablemap.h"


CAutoplayerButton::CAutoplayerButton() {
  Reset();
}

CAutoplayerButton::~CAutoplayerButton() {
}

void CAutoplayerButton::SetTechnicalName(const CString name) {
  _technical_name = name;
  CString hotkey_name = name + "hotkey";
  _hotkey.SetName(hotkey_name);

  _default_label = p_tablemap->GetTMSymbol(_technical_name + "defaultlabel").MakeLower();

  if (p_tablemap->GetTMSymbol(_technical_name + "clickmethod").MakeLower() == "double") {
    _click_method = BUTTON_DOUBLECLICK;
  } else if (p_tablemap->GetTMSymbol(_technical_name + "clickmethod").MakeLower() == "nothing") {
    _click_method = BUTTON_NOTHING;
  } else {
    _click_method = BUTTON_SINGLECLICK;
  }
}

void CAutoplayerButton::Reset() {
  _label = "";
  _technical_name = "";
  _button_type = kUndefined;
  SetClickable(false);
}

bool CAutoplayerButton::Click() {
  if (_clickable) {
    // Try to send a hotkey first, if specified in tablemap
    if (_hotkey.PressHotkey()) {
      write_log(Preferences()->debug_autoplayer(), "[CasinoInterface] Pressed hotkey for button button %s\n", _label);
      return true;
    }
    // Lookup the region
    RECT button_region;
    p_tablemap->GetTMRegion(_technical_name, &button_region);
    if (BUTTON_NOTHING == _click_method) {
      write_log(Preferences()->debug_autoplayer(), "[CAutoplayerButton] Doing nothing on this button [%s] [%s]\n", _label, _technical_name);
    } else if (BUTTON_DOUBLECLICK == _click_method) {
      // double click the button if needed
      p_casino_interface->DoubleClickRect(button_region);
      write_log(Preferences()->debug_autoplayer(), "[CAutoplayerButton] Clicked button [%s] [%s]\n", _label, _technical_name);
    } else {
      // Otherwise: click the button the normal way
      p_casino_interface->ClickRect(button_region);
      write_log(Preferences()->debug_autoplayer(), "[CAutoplayerButton] Clicked button [%s] [%s]\n", _label, _technical_name);
    }
    return true;
  } else {
    write_log(Preferences()->debug_autoplayer(), "[CAutoplayerButton] Could not click button %s. Either undefined or not visible.\n", _label);
    return false;
  }
}

void CAutoplayerButton::SetClickable(bool clickable) {
  _clickable = clickable;
}

void CAutoplayerButton::SetState(const CString state) {
  CString button_state_lower_case = state;
  button_state_lower_case.MakeLower();
  if (button_state_lower_case.Left(4) == "true"
      || button_state_lower_case.Left(2) == "on"
      || button_state_lower_case.Left(3) == "yes"
      || button_state_lower_case.Left(7) == "checked"
      || button_state_lower_case.Left(3) == "lit") {
    SetClickable(true);
  } else {
    SetClickable(false);
  }
}

void CAutoplayerButton::SetLabel(const CString label) {
  _label = label;
  PrecomputeButtonType();
}

bool CAutoplayerButton::IsLabelAllin() {
  return p_string_match->IsStringAllin(_label);
}

bool CAutoplayerButton::IsLabelRaise() {
  CString s_lower_case = _label.MakeLower();
  s_lower_case = s_lower_case.Left(5);
  return (s_lower_case == "raise"
    || s_lower_case == "ra1se"
    || s_lower_case == "ralse"
    || s_lower_case.Left(3) == "bet"
    // Last occurence of swag for backward compatibility
    || s_lower_case.Left(4) == "swag");
}

bool CAutoplayerButton::IsLabelCall() {
  CString s_lower_case = _label.MakeLower();
  s_lower_case = s_lower_case.Left(4);
  return (s_lower_case == "call" || s_lower_case == "caii" || s_lower_case == "ca11");
}

bool CAutoplayerButton::IsLabelCheck() {
  CString s_lower_case = _label.MakeLower();
  s_lower_case = s_lower_case.Left(5);
  return (s_lower_case == "check" || s_lower_case == "cheok");
}

bool CAutoplayerButton::IsLabelFold() {
  CString s_lower_case = _label.MakeLower();
  s_lower_case = s_lower_case.Left(4);
  return (s_lower_case == "fold" || s_lower_case == "fo1d" || s_lower_case == "foid");
}

bool CAutoplayerButton::IsLabelAutopost() {
  CString s_lower_case = _label;
  s_lower_case.Remove(' ');
  s_lower_case.Remove('-');
  s_lower_case.MakeLower();
  s_lower_case = s_lower_case.Left(8);
  return (s_lower_case == "autopost" || s_lower_case == "aut0p0st");
}

bool CAutoplayerButton::IsLabelSitin() {
  CString s_lower_case = _label;
  s_lower_case.MakeLower();
  s_lower_case.Remove(' ');
  s_lower_case.Remove('-');
  s_lower_case = s_lower_case.Left(5);
  return (s_lower_case == "sitin" || s_lower_case == "s1t1n");
}

bool CAutoplayerButton::IsLabelSitout() {
  CString s_lower_case = _label;
  s_lower_case.MakeLower();
  s_lower_case.Remove(' ');
  s_lower_case.Remove('-');
  s_lower_case = s_lower_case.Left(6);
  return (s_lower_case == "sitout" || s_lower_case == "s1tout" || s_lower_case == "sit0ut" || s_lower_case == "s1t0ut");
}

bool CAutoplayerButton::IsLabelLeave() {
  return (_label.MakeLower().Left(5) == "leave");
}

bool CAutoplayerButton::IsLabelRematch() {
  return (_label.MakeLower().Left(7) == "rematch");
}

bool CAutoplayerButton::IsLabelPrefold() {
  return (_label.MakeLower().Left(7) == "prefold");
}

bool CAutoplayerButton::IsNameI86() {
  return (_technical_name.Left(3).MakeLower() == "i86");
}


// We precompute the button-type from the label, because their was a raise-condition
// when COpenHoldemView::UpdateDisplay(), triggered by a timer,
// accessed the button-labels (non-elementary data)
// which could at the same time be changed by the scraper (part of the heart-beat-).
// We could have added mutexes, but were not sure about performance,
// so we switched to an atomic data-type without pointers instead.
void CAutoplayerButton::PrecomputeButtonType() {
  if (IsLabelAllin()) {
    _button_type = k_autoplayer_function_allin;
  } else if (IsLabelRaise()) {
    _button_type = k_autoplayer_function_raise;
  } else if (IsLabelCall()) {
    _button_type = k_autoplayer_function_call;
  } else if (IsLabelCheck()) {
    _button_type = k_autoplayer_function_check;
  } else if (IsLabelFold()) {
    _button_type = k_autoplayer_function_fold;
  } else if (IsLabelAutopost()) {
    _button_type = k_hopper_function_autopost;
  } else if (IsLabelSitin()) {
    _button_type = k_hopper_function_sitin;
  } else if (IsLabelSitout()) {
    _button_type = k_hopper_function_sitout;
  } else if (IsLabelLeave()) {
    _button_type = k_hopper_function_leave;
  } else if (IsLabelRematch()) {
    _button_type = k_hopper_function_rematch;
  } else if (IsLabelPrefold()) {
    _button_type = k_standard_function_prefold;
  } else if (IsNameI86()) {
    _button_type = k_button_i86;
  } else {
    /* No or wrongly scraped value, apply default label, if any */
    if (_default_label == "allin" || _default_label == "max") {
      _button_type = k_autoplayer_function_allin;
    } else if (_default_label == "raise" || _default_label == "bet") {
      _button_type = k_autoplayer_function_raise;
    } else if (_default_label == "call") {
      _button_type = k_autoplayer_function_call;
    } else if (_default_label == "check") {
      _button_type = k_autoplayer_function_check;
    } else if (_default_label == "fold") {
      _button_type = k_autoplayer_function_fold;
    } else if (_default_label == "autopost") {
      _button_type = k_hopper_function_autopost;
    } else if (_default_label == "sitin") {
      _button_type = k_hopper_function_sitin;
    } else if (_default_label == "sitout") {
      _button_type = k_hopper_function_sitout;
    } else if (_default_label == "leave") {
      _button_type = k_hopper_function_leave;
    } else if (_default_label == "rematch") {
      _button_type = k_hopper_function_rematch;
    } else if (_default_label == "prefold") {
      _button_type = k_standard_function_prefold;
    } else {
      _button_type = kUndefined;
      write_log(Preferences()->debug_autoplayer(), "[CasinoInterface] WARNING! Unknown button type [%s] [%s]\n", _label, _default_label);
    }
  }
}