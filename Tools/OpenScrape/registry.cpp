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
#include "DialogTableMap.h"

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
	tablemap_x  = 0;
	tablemap_y  = 0;
	tablemap_dx = 600;
	tablemap_dy = 750;

	main_x  = tablemap_dx;
	main_y  = 0;
	main_dx = 400;
	main_dy = 300;

	grhash_x = grhash_y = 0;
	grhash_dx = 380;
	grhash_dy = 330;

	region_grouping = BY_TYPE; // None

	hkResult = RegOpenKeyEx(HKEY_CURRENT_USER, "Software\\OpenHoldem\\OpenScrape", 0, KEY_READ, &hKey);
	if (hkResult==ERROR_SUCCESS) {
		// No longer any need to get...   
		//   * Main window location and size
		//   * tablemap window location and size
		// The defaults are fine.
		//
		// graphical hash point window location and size
		cbData = sizeof(str);
		if ( (hkResult = RegQueryValueEx(hKey, "grhash_x", NULL, &dwType, (LPBYTE) str, &cbData)) == ERROR_SUCCESS)
			grhash_x = atoi(str);

		cbData = sizeof(str);
		if ( (hkResult = RegQueryValueEx(hKey, "grhash_y", NULL, &dwType, (LPBYTE) str, &cbData)) == ERROR_SUCCESS)
			grhash_y = atoi(str);

		cbData = sizeof(str);
		if ( (hkResult = RegQueryValueEx(hKey, "grhash_dx", NULL, &dwType, (LPBYTE) str, &cbData)) == ERROR_SUCCESS)
			grhash_dx = atoi(str);

		cbData = sizeof(str);
		if ( (hkResult = RegQueryValueEx(hKey, "grhash_dy", NULL, &dwType, (LPBYTE) str, &cbData)) == ERROR_SUCCESS)
			grhash_dy = atoi(str);

		// Region grouping
		cbData = sizeof(str);
		if ( (hkResult = RegQueryValueEx(hKey, "region_grouping", NULL, &dwType, (LPBYTE) str, &cbData)) == ERROR_SUCCESS)
			region_grouping = atoi(str);
	}

	RegCloseKey(hKey);
}

void Registry::write_reg(void) 
{
	HKEY		hKey;
	DWORD		dwDisp;
	char		str[256];

	//PokerTracker Settings
	RegCreateKeyEx(HKEY_CURRENT_USER, "Software\\OpenHoldem\\OpenScrape", 0, NULL, 
	REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, &dwDisp);

	// graphical hash point window location and size
	sprintf_s(str, 256, "%d", grhash_x);
	RegSetValueEx(hKey, "grhash_x", 0, REG_SZ, (LPBYTE) str, (DWORD) strlen(str)+1);
	sprintf_s(str, 256, "%d", grhash_y);
	RegSetValueEx(hKey, "grhash_y", 0, REG_SZ, (LPBYTE) str, (DWORD) strlen(str)+1);
	sprintf_s(str, 256, "%d", grhash_dx);
	RegSetValueEx(hKey, "grhash_dx", 0, REG_SZ, (LPBYTE) str, (DWORD) strlen(str)+1);
	sprintf_s(str, 256, "%d", grhash_dy);
	RegSetValueEx(hKey, "grhash_dy", 0, REG_SZ, (LPBYTE) str, (DWORD) strlen(str)+1);

	// Region grouping
	sprintf_s(str, 256, "%d", region_grouping);
	RegSetValueEx(hKey, "region_grouping", 0, REG_SZ, (LPBYTE) str, (DWORD) strlen(str)+1);

	RegCloseKey(hKey);
}
