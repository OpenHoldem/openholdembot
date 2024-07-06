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

#include "registry.h"
#include "stdio.h"
#include "stdlib.h"

Registry::Registry(void) 
{
}

void Registry::read_reg(void) 
{
	HKEY		hKey;
	DWORD		dwType, cbData;
	LONG		hkResult;
	char		str[256];

	// Defaults
	main_x = main_y = 0;

	hkResult = RegOpenKeyEx(HKEY_CURRENT_USER, "Software\\OpenHoldem\\OHReplay", 0, KEY_READ, &hKey);
	if (hkResult==ERROR_SUCCESS) {

		// Main window location
		cbData = sizeof(str);
		if ( (hkResult = RegQueryValueEx(hKey, "main_x", NULL, &dwType, (LPBYTE) str, &cbData)) == ERROR_SUCCESS) {
			main_x = atoi(str);
		}
		cbData = sizeof(str);
		if ( (hkResult = RegQueryValueEx(hKey, "main_y", NULL, &dwType, (LPBYTE) str, &cbData)) == ERROR_SUCCESS) {
			main_y = atoi(str);
		}
	}

	RegCloseKey(hKey);
}

void Registry::write_reg(void) 
{
	HKEY		hKey;
	DWORD		dwDisp;
	char		str[256];

	//PokerTracker Settings
	RegCreateKeyEx(HKEY_CURRENT_USER, "Software\\OpenHoldem\\OHReplay", 0, NULL, 
	REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, &dwDisp);

	// Main window location and size
	sprintf_s(str, 256, "%d", main_x);
	RegSetValueEx(hKey, "main_x", 0, REG_SZ, (LPBYTE) str, (DWORD) strlen(str)+1);
	sprintf_s(str, 256, "%d", main_y);
	RegSetValueEx(hKey, "main_y", 0, REG_SZ, (LPBYTE) str, (DWORD) strlen(str)+1);

	RegCloseKey(hKey);
}
