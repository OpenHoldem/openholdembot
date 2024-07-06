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

#include "CTablemap/CTablemap.h"
#include "StringFunctions/string_functions.h"

const char kHotkeyUndefined = '\0';
char KeyArray[2];
CString	subctrl = "ctrl";
CString	subctrlf = "ctrlf";
CString	subalt = "alt";
CString	subaltf = "altf";
CString SpaceCheck = " ";
CString	Nums = "123456789";
CString HKCode='\0';
CString ERRkey='\0';
CString ERRTM[3] = {"Space is not allowed", "Single keys are not allowed", "Disallowed key combo"};

CCasinoHotkey::CCasinoHotkey() {
}

CCasinoHotkey::~CCasinoHotkey() {
}

bool CCasinoHotkey::PressHotkey() {
	char key_to_be_pressed = LookupHotkey();
	ERRkey.MakeUpper();
	if (key_to_be_pressed == kHotkeyUndefined) {
		if (HKCode.GetLength() > 10) {
			write_log(Preferences()->debug_autoplayer(),
			"[CasinoHotkeys] Something went wrong [%s, %s]\n",
			HKCode, ERRkey );
		}
		return false;
	}
	HKCode.MakeUpper();
	write_log(Preferences()->debug_autoplayer(),
		"[CasinoHotkeys] Going to press key [%s]\n",
		HKCode);

	p_casino_interface->SendHotKey(KeyArray);
	return true;
}

void CCasinoHotkey::SetName(CString name) {
	_name = name;
}

char CCasinoHotkey::LookupHotkey() {

	assert(_name != "");
	CString key_to_be_pressed = p_tablemap->GetTMSymbol(_name);
	key_to_be_pressed.MakeLower();
	if (key_to_be_pressed == "") {
		return kHotkeyUndefined;
	}
	else if (key_to_be_pressed.GetLength() == 1) {
		HKCode = ERRTM[1];
		ERRkey = key_to_be_pressed;
		return kHotkeyUndefined;
	}
	else if (key_to_be_pressed.Find(SpaceCheck) != -1) {
		HKCode = ERRTM[0];
		ERRkey = key_to_be_pressed;
		return kHotkeyUndefined;
	}
	else if (key_to_be_pressed == "ctrlright") {
		KeyArray[0] = VK_CONTROL;
		KeyArray[1] = VK_RIGHT;
		HKCode = key_to_be_pressed;
		return true;
	}
	else if (key_to_be_pressed == "ctrlleft") {
		KeyArray[0] = VK_CONTROL;
		KeyArray[1] = VK_LEFT;
		HKCode = key_to_be_pressed;
		return true;
	}
	else if (key_to_be_pressed == "ctrlup") {
		KeyArray[0] = VK_CONTROL;
		KeyArray[1] = VK_UP;
		HKCode = key_to_be_pressed;
		return true;
	}
	else if (key_to_be_pressed == "ctrldown") {
		KeyArray[0] = VK_CONTROL;
		KeyArray[1] = VK_DOWN;
		HKCode = key_to_be_pressed;
		return true;
	}
	else if (key_to_be_pressed.GetLength() == 6 && key_to_be_pressed.Find(subctrlf) != -1) {
			KeyArray[0] = VK_CONTROL;
			if (key_to_be_pressed.Find(Nums[0]) != -1) {
				KeyArray[1] = VK_F1;
			}
			else if (key_to_be_pressed.Find(Nums[1]) != -1) {
				KeyArray[1] = VK_F2;
			}
			else if (key_to_be_pressed.Find(Nums[2]) != -1) {
				KeyArray[1] = VK_F3;
			}
			else if (key_to_be_pressed.Find(Nums[3]) != -1) {
				KeyArray[1] = VK_F4;
			}
			else if (key_to_be_pressed.Find(Nums[4]) != -1) {
				KeyArray[1] = VK_F5;
			}
			else if (key_to_be_pressed.Find(Nums[5]) != -1) {
				KeyArray[1] = VK_F6;
			}
			else if (key_to_be_pressed.Find(Nums[6]) != -1) {
				KeyArray[1] = VK_F7;
			}
			else if (key_to_be_pressed.Find(Nums[7]) != -1) {
				KeyArray[1] = VK_F8;
			}
			else if (key_to_be_pressed.Find(Nums[8]) != -1) {
				KeyArray[1] = VK_F9;
			}
			else { 
				KeyArray[1] = '\0'; 
				HKCode = ERRTM[2];
				ERRkey = key_to_be_pressed;
				return kHotkeyUndefined;
			}
			HKCode = key_to_be_pressed;
			return true;
	}
	else if (key_to_be_pressed.GetLength() == 5 && key_to_be_pressed.Find(subaltf) != -1) {
			KeyArray[0] = VK_MENU;
			if (key_to_be_pressed.Find(Nums[0]) != -1) {
				KeyArray[1] = VK_F1;
			}
			else if (key_to_be_pressed.Find(Nums[1]) != -1) {
				KeyArray[1] = VK_F2;
			}
			else if (key_to_be_pressed.Find(Nums[2]) != -1) {
				KeyArray[1] = VK_F3;
			}
			else if (key_to_be_pressed.Find(Nums[3]) != -1) {
				KeyArray[1] = VK_F4;
			}
			else if (key_to_be_pressed.Find(Nums[4]) != -1) {
				KeyArray[1] = VK_F5;
			}
			else if (key_to_be_pressed.Find(Nums[5]) != -1) {
				KeyArray[1] = VK_F6;
			}
			else if (key_to_be_pressed.Find(Nums[6]) != -1) {
				KeyArray[1] = VK_F7;
			}
			else if (key_to_be_pressed.Find(Nums[7]) != -1) {
				KeyArray[1] = VK_F8;
			}
			else if (key_to_be_pressed.Find(Nums[8]) != -1) {
				KeyArray[1] = VK_F9;
			}
			else {
				KeyArray[1] = '\0';
				HKCode = ERRTM[2];
				ERRkey = key_to_be_pressed;
				return kHotkeyUndefined;
			}
			HKCode = key_to_be_pressed;
			return true;
		}
	else if (key_to_be_pressed == "f1") {
		KeyArray[0] = VK_F1;
		HKCode = key_to_be_pressed;
		return true;
	}
	else if (key_to_be_pressed == "f2") {
		KeyArray[0] = VK_F2;
		HKCode = key_to_be_pressed;
		return true;
	}
	else if (key_to_be_pressed == "f3") {
		KeyArray[0] = VK_F3;
		HKCode = key_to_be_pressed;
		return true;
	}
	else if (key_to_be_pressed == "f4") {
		KeyArray[0] = VK_F4;
		HKCode = key_to_be_pressed;
		return true;
	}
	else if (key_to_be_pressed == "f5") {
		KeyArray[0] = VK_F5;
		HKCode = key_to_be_pressed;
		return true;
	}
	else if (key_to_be_pressed == "f6") {
		KeyArray[0] = VK_F6;
		HKCode = key_to_be_pressed;
		return true;
	}
	else if (key_to_be_pressed == "f7") {
		KeyArray[0] = VK_F7;
		HKCode = key_to_be_pressed;
		return true;
	}
	else if (key_to_be_pressed == "f8") {
		KeyArray[0] = VK_F8;
		HKCode = key_to_be_pressed;
		return true;
	}
	else if (key_to_be_pressed == "f9") {
		KeyArray[0] = VK_F9;
		HKCode = key_to_be_pressed;
		return true;
	}
	else if (key_to_be_pressed == "xxxx") {
		KeyArray[0] = 0x0d;
		return true;
	}
	else if (key_to_be_pressed.GetLength() > 5) {
		HKCode = ERRTM[2];
		ERRkey = key_to_be_pressed;
		return kHotkeyUndefined;
	}
	else if (key_to_be_pressed.Find(subctrl) != -1) {
		KeyArray[0] = VK_CONTROL;
		KeyArray[1] = VkKeyScan(key_to_be_pressed[4]);
		HKCode = key_to_be_pressed;
		return true;
	}
	else if (key_to_be_pressed.GetLength() > 4) {
		HKCode = ERRTM[2];
		ERRkey = key_to_be_pressed;
		return kHotkeyUndefined;
	}
	else if (key_to_be_pressed.Find(subalt) != -1) {
		KeyArray[0] = VK_MENU;
		KeyArray[1] = VkKeyScan(key_to_be_pressed[3]);
		HKCode = key_to_be_pressed;
		return true;
	}
	else {
		HKCode = ERRTM[2];
		ERRkey = key_to_be_pressed;
		return kHotkeyUndefined;
	}
}
