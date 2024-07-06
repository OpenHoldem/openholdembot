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

#include "CConfigurationCheck.h"

#include "WindowFunctions/window_functions.h"


CConfigurationCheck *p_configurationcheck = 0;

// A full list of keyboard layout codes can be found here:
// http://msdn.microsoft.com/en-us/goglobal/bb895996.aspx
const TCHAR k_KeyboardLayout_UK_US_English[KL_NAMELENGTH] = "00000409";

CConfigurationCheck::CConfigurationCheck()
{
  CheckEnabledConfigurationChecks(false);
}

CConfigurationCheck::~CConfigurationCheck()
{}

void CConfigurationCheck::ForceAllConfigurationChercks() {
  CheckEnabledConfigurationChecks(true);
}

void CConfigurationCheck::CheckEnabledConfigurationChecks(bool force_all)
{
	// Check really critical settings in any case.
	// OpenHoldem will never work, if these are not right.
	CheckColourDepth();
	CheckForSwapMouseBtns();
	// OpenHoldem may or may not work, if these are not right.
	if (Preferences()->configurationcheck_input_settings() || force_all)
	{
		CheckInputSettings();
	}
	// No longer checking CheckForClassicalTheme();
  // as now (9.1.3) all themes (probably) are supported
  // out of the box with the same tablemaps.
	if (Preferences()->configurationcheck_font_settings() || force_all)
	{
		CheckForFontSmoothing();
	}
  // !! Might be reused for MSVCRT 2010
	//CheckForMissingMSVCRT();
}

HKEY CConfigurationCheck::GetHive(CString mhive)
{
	HKEY hive;

	if (mhive == "HKCU")
		hive = HKEY_CURRENT_USER;

	if (mhive == "HKLM")
		hive = HKEY_LOCAL_MACHINE;

	return hive;
}

bool CConfigurationCheck::OpenKey(CString mhive, CString registry_path)
{
	HKEY hive, hKey;
	hive = GetHive(mhive);

	if (RegOpenKeyEx(hive, registry_path, 0, KEY_QUERY_VALUE, &hKey) == ERROR_SUCCESS)
	{
		RegCloseKey(hKey);
		return true;
	}

	RegCloseKey(hKey);
	return false;
}


CString CConfigurationCheck::GetValue(CString mhive, int type, CString registry_path, CString key_name)
{
	DWORD dwData, nBytes, dwType;
	CString value;
	HKEY hive, pKey;
	hive = GetHive(mhive);

	// Open the Key
	if(RegOpenKeyEx(hive,registry_path,0,KEY_QUERY_VALUE,&pKey)==ERROR_SUCCESS)
	{
		switch(type)
		{
			//REG_DWORD
			case 0 :

				if(RegQueryValueEx(pKey,key_name,NULL,&dwType,(LPBYTE)&dwData,&nBytes)==ERROR_SUCCESS)
				{
					value.Format("%d", dwData);
					RegCloseKey(pKey);
					return value;
				}

			//REG_SZ, REG_EXPAND_SZ
			case 1 :

				if(RegQueryValueEx(pKey,key_name,NULL,&dwType,0,&nBytes)==ERROR_SUCCESS)
				{
					// Allocate string buffer
					LPTSTR buffer = value.GetBuffer(nBytes/sizeof(TCHAR));
					// Read string
					VERIFY(RegQueryValueEx(pKey,key_name,NULL,0,(LPBYTE)buffer,&nBytes)==ERROR_SUCCESS);
					// Release string buffer
					value.ReleaseBuffer();
					//printf("Get SZ Value OK\n");
					RegCloseKey(pKey);
					return value;
				}

			default :
				return "0";
		}
	}
	else
	{
		return "0";
	}
}

void CConfigurationCheck::CheckColourDepth()
{
	CWindowDC dc(NULL);
	int nBitsPerPixel = dc.GetDeviceCaps(PLANES) * dc.GetDeviceCaps(BITSPIXEL);
	if (nBitsPerPixel < 24)
		MessageBox_Error_Warning("It appears that your Display settings are not configured according to OpenHoldem specifications.\n"
				"24 bit color or higher is needed to reliably extract information from the poker client.\n\n"
				"For more info, look at the manual and the user forums",
				"Caution: Color Depth Too Low");
}

void CConfigurationCheck::CheckInputSettings()
{
	TCHAR KeyboardLayout[KL_NAMELENGTH];
	bool Success = GetKeyboardLayoutName((LPSTR)&KeyboardLayout) != 0;

	if (Success && (_tcscmp(KeyboardLayout, k_KeyboardLayout_UK_US_English) != 0))
	{
		MessageBox_Error_Warning("You seem to have non-english keyboard settings.\n"
				"Keyboard settings affect especially the decimal point in numbers\n"
				"and therefore the scraper-engine and the auto-player.\n"
				"If you continue, OpenHoldem may or may not work as expected.\n"
				"If you are an experienced user with a working setup\n"
				"you may turn this warning off.\n"
				"If you are new to OpenHoldem or encounter problems\n"
				"you should fix your keyboard settings\n",
				"Caution: Improper keyboard settings");
	}
}

void CConfigurationCheck::CheckForSwapMouseBtns()
{
	CString p_szKeySwapBtns = "Control Panel\\Mouse\\";
	CString p_szNameSwapBtns = "SwapMouseButtons";

	bool SwapBtns = false;

	if (atoi(GetValue("HKCU", 1, p_szKeySwapBtns, p_szNameSwapBtns)) == 1)
	{
		SwapBtns = true;
	}

	if (SwapBtns == true)
	{
		MessageBox_Error_Warning("It appears that your Mouse settings are not configured according to OpenHoldem specifications.\n\n"
			
			"The autoplayer has to generate mouse-events via a very low-level system-interface,\n"
			"and windows might/will swap these hardware-events for their inverse logical responses.\n\n"
			"This means Casinos clients will no longer interpret mouse-events correctly !\n"
			"Please Disable the - Swap Mouse Buttons - option in :\n\n"
			"Control Panel-> Mouse.\n\n"
			"and restart your user session in order to proceed\n",
			"Caution: Swap Mouse Buttons Activated");
	}
}

void CConfigurationCheck::CheckForFontSmoothing()
{
	CString p_szKeyFontSmoothing = "Control Panel\\Desktop\\";
	CString p_szNameFontSmooth = "FontSmoothing";
	CString p_szNameFontSmoothType = "FontSmoothingType";

	bool font_smoothing = true;

	if (atoi(GetValue("HKCU", 1, p_szKeyFontSmoothing, p_szNameFontSmooth)) == 0)
	{
		if (atoi(GetValue("HKCU", 0, p_szKeyFontSmoothing, p_szNameFontSmoothType)) == 0)
		{
			font_smoothing = false;
		}
	}

	if (font_smoothing == true)
	{
		if (atoi(GetValue("HKCU", 1, p_szKeyFontSmoothing, p_szNameFontSmooth)) == 2)
		{
			int FontSmoothType = atoi(GetValue("HKCU", 0, p_szKeyFontSmoothing, p_szNameFontSmoothType));

			if (FontSmoothType == 2)
			{
				MessageBox_Error_Warning("It appears that Cleartype font smoothing is enabled.\n"
							"\n"
							"In order for OpenHoldem to reliably\n"
							"extract information from the poker client\n"
							"you should disable Font Smoothing.",
							"Caution: Font smoothing is enabled");
			}

			else if (FontSmoothType == 1)
			{
				MessageBox_Error_Warning("It appears that Standard font smoothing is enabled.\n"
							"\n"
							"In order for OpenHoldem to reliably\n"
							"extract information from the poker client\n"
							"you should disable Font Smoothing.",
							"Caution: Font smoothing is enabled");
			}
		}
	}
}

/*void CConfigurationCheck::CheckForMissingMSVCRT()
{
	CString p_szKeyCRT = "SOFTWARE\\Microsoft\\DevDiv\\VC\\Servicing\\10.0\\RED\\1033\\";
	CString p_szNameCRT = "Install";

	bool installed = false;

	if (atoi(GetValue("HKLM", 0, p_szKeyCRT, p_szNameCRT)) == 1)
	{
		installed = true;
	}

	if (installed == false)
	{
		MessageBox_Error_Warning("Unable to detect\n"
			"Microsoft Visual C++ 2005 redistributable runtime library.\n"
			"\n"
			"This library is necessary for Perl users.\n"
			"if you don't use Perl you may turn this warning off\n"
			"by not loading the perl interpreter by default.",
			"Caution: MSVCRT 10.0 missing");
	}
} */
