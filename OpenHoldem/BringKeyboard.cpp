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
#include "BringKeyBoard.h"

#include "CAutoConnector.h"
#include "CEngineContainer.h"

#include "CSymbolEngineCasino.h"
#include "CSymbolEngineAutoplayer.h"
#include "MainFrm.h"
#include "OpenHoldem.h"
#include "CMyMutex.h"

void CheckBringKeyboard(void) {
	HMENU			bringsysmenu = NULL;
	MENUITEMINFO mii;
	int				input_count = 0, i = 0;
	INPUT			input[100] = {0};
	char			temp[256] = {0};
	CString		c_text = "";
	int				keybd_item_pos = 0;
	int				e = SUCCESS;

	if (!p_engine_container->symbol_engine_casino()->ConnectedToBring()) 	{
		write_log(Preferences()->debug_autoplayer(), "[BringKeyBoard] Not connected to bring, therefore no bring-keyboard to be enabled.\n");
		return;
	}

	write_log(Preferences()->debug_autoplayer(), "[BringKeyBoard] Connected to bring.\n");
	write_log(Preferences()->debug_autoplayer(), "[BringKeyBoard] Enabling bring-keyboard if necessary.\n");

	// Init locals
	memset(&mii, 0, sizeof(MENUITEMINFO));

	// Find position of "Keyboard" item on system menu
	bringsysmenu = GetSystemMenu(p_autoconnector->attached_hwnd(), false);

	mii.cbSize = sizeof(MENUITEMINFO);
	mii.fMask = MIIM_STRING;
	mii.fType = MFT_STRING;
	mii.dwTypeData = temp;
	keybd_item_pos = kUndefined;
	for (int i=GetMenuItemCount(bringsysmenu)-1; i>=0; --i)	{
		mii.cch = 256;
	
		// Get the text of this menu item
		GetMenuItemInfo(bringsysmenu, i, true, &mii);
		c_text = temp;

		// See if this is the "keyboard" menu item
		if (c_text.MakeLower().Find("keyboard") != -1) {
			keybd_item_pos = i;
			continue;
		}
	}

	// Get state of keyboard menu item
	if (keybd_item_pos == kUndefined) {
		return;
  }
	mii.cbSize = sizeof(MENUITEMINFO);
	mii.fMask = MIIM_STATE;
	GetMenuItemInfo(bringsysmenu, keybd_item_pos, true, &mii);
	
	if (!(mii.fState&MFS_CHECKED)) 	{
		HWND			hwnd_focus;
		POINT			cur_pos = {0};

		input_count = 0;
		// Alt key down
		ZeroMemory(&input[input_count],sizeof(INPUT));
		input[input_count].type = INPUT_KEYBOARD;
		input[input_count].ki.wVk = VK_MENU;
		input_count++;

		// Space bar down
		ZeroMemory(&input[input_count],sizeof(INPUT));
		input[input_count].type = INPUT_KEYBOARD;
		input[input_count].ki.wVk = VK_SPACE;
		input_count++;

		// Space bar up
		ZeroMemory(&input[input_count],sizeof(INPUT));
		input[input_count].type = INPUT_KEYBOARD;
		input[input_count].ki.wVk = VK_SPACE;
		input[input_count].ki.dwFlags = KEYEVENTF_KEYUP;
		input_count++;

		// Alt key up
		ZeroMemory(&input[input_count],sizeof(INPUT));
		input[input_count].type = INPUT_KEYBOARD;
		input[input_count].ki.wVk = VK_MENU;
		input[input_count].ki.dwFlags = KEYEVENTF_KEYUP;
		input_count++;

		CMyMutex mutex;
		if (!mutex.IsLocked()) return;

		hwnd_focus = GetFocus();
		GetCursorPos(&cur_pos);

		SetFocus(p_autoconnector->attached_hwnd());
		SetForegroundWindow(p_autoconnector->attached_hwnd());
		SetActiveWindow(p_autoconnector->attached_hwnd());
		SendInput(input_count, input, sizeof(INPUT));

		Sleep(200);
		input_count = 0;
		// K down
		ZeroMemory(&input[input_count],sizeof(INPUT));
		input[input_count].type = INPUT_KEYBOARD;
		input[input_count].ki.wVk = 'K';
		input_count++;

		// K up
		ZeroMemory(&input[input_count],sizeof(INPUT));
		input[input_count].type = INPUT_KEYBOARD;
		input[input_count].ki.wVk = 'K';
		input[input_count].ki.dwFlags = KEYEVENTF_KEYUP;
		input_count++;

		SetFocus(p_autoconnector->attached_hwnd());
		SetForegroundWindow(p_autoconnector->attached_hwnd());
		SetActiveWindow(p_autoconnector->attached_hwnd());
		SendInput(input_count, input, sizeof(INPUT));

		SetActiveWindow(hwnd_focus);
		SetForegroundWindow(hwnd_focus);
		SetFocus(hwnd_focus);

		SetCursorPos(cur_pos.x, cur_pos.y);
	}
}
