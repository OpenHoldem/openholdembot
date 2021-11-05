//******************************************************************************
//
// This file is part of the OpenHoldem project
//    Source code:           https://github.com/OpenHoldem/openholdembot/
//    Forums:                http://www.maxinmontreal.com/forums/index.php
//    Licensed under GPL v3: http://www.gnu.org/licenses/gpl.html
//
//******************************************************************************
//
// Purpose: Hotkeys as alternative for autoplayer-buttons
//
//******************************************************************************

#include "stdafx.h"
#include "CCasinoHotkey.h"

#include "CCasinoInterface.h"

#include "..\CTablemap\CTablemap.h"
#include "..\DLLs\StringFunctions_DLL\string_functions.h"

const char kHotkeyUndefined = '\0';

CCasinoHotkey::CCasinoHotkey() {
}

CCasinoHotkey::~CCasinoHotkey() {
}

bool CCasinoHotkey::PressHotkey() {
  char key_to_be_pressed = LookupHotkey();
  if (key_to_be_pressed == kHotkeyUndefined) {
    return false;
  }
  if (isalnum(key_to_be_pressed)) {
    write_log(Preferences()->debug_autoplayer(),
      "[CasinoHotkeys] Going to press key [%c]\n",
      key_to_be_pressed);
  } else {
    write_log(Preferences()->debug_autoplayer(),
      "[CasinoHotkeys] Going to press non-alpha-numeric key [%x]\n",
      key_to_be_pressed);
  }
  p_casino_interface->SendKey(key_to_be_pressed);
  return true;
}

void CCasinoHotkey::SetName(CString name) {
  _name = name;
}

char CCasinoHotkey::LookupHotkey() {
  assert(_name != "");
  CString key_to_be_pressed = p_tablemap->GetTMSymbol(_name);
  if (key_to_be_pressed == "") {
    return kHotkeyUndefined;
  }
  key_to_be_pressed.MakeLower();
  // Analyze hotkey-combination
  if (key_to_be_pressed.GetLength() == 1) {
    char first_char = key_to_be_pressed[0];
    if (!isalnum(first_char)) {
      // We expect a..z, A..Z, 0..9
      return kHotkeyUndefined;
    }
    return first_char;
  } else if (key_to_be_pressed.GetLength() == 1) {
    if (key_to_be_pressed.MakeLower() == "space") {
      return ' ';
    } else if (key_to_be_pressed == "enter") {
      // Enter = carriage return = 0x0D
      // https://en.wikipedia.org/wiki/ASCII
      return char(0x0d);
    } else if (key_to_be_pressed == "return") {
      // Same as "enter"
      return char(0x0d);
    } else if (key_to_be_pressed == "right") {
      return VK_RIGHT;
    } else if (key_to_be_pressed == "left") {
      return VK_LEFT;
    } else if (key_to_be_pressed  == "up") {
      return VK_UP;
    } else if (key_to_be_pressed == "down") {
      return VK_DOWN;
    }
  }
  return kHotkeyUndefined;
}

