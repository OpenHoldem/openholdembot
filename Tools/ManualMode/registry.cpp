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

#define MM_SUBKEY  "Software\\OpenHoldem\\ManualMode"

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
	macro = "RNbBPpPAdKsP";

	// Manual Mode Settings
	hkResult = RegOpenKeyEx(HKEY_CURRENT_USER, MM_SUBKEY, 0, KEY_READ, &hKey);
	if (hkResult==ERROR_SUCCESS) {
    // Macro
		cbData = sizeof(str);
		if ( (hkResult = RegQueryValueEx(hKey, "macro", NULL, &dwType, (LPBYTE) str, &cbData)) == ERROR_SUCCESS) {
			macro = str;
		}
	}

	RegCloseKey(hKey);
}

void Registry::write_reg(void) 
{
	HKEY		hKey;
	DWORD		dwDisp;
	char		str[256];

	// Settings
	RegCreateKeyEx(HKEY_CURRENT_USER, MM_SUBKEY, 0, NULL, 
	REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, &dwDisp);
	// Macro
	sprintf_s(str, 256, "%s", macro.GetString());
	RegSetValueEx(hKey, "macro", 0, REG_SZ, (LPBYTE) str, (DWORD) strlen(str)+1);
	RegCloseKey(hKey);
}
