//******************************************************************************
//
// This file is part of the OpenHoldem project
//    Source code:           https://github.com/OpenHoldem/openholdembot/
//    Forums:                http://www.maxinmontreal.com/forums/index.php
//    Licensed under GPL v3: http://www.gnu.org/licenses/gpl.html
//
//******************************************************************************
//
// Purpose: Very simple user-DLL as a starting-point
//
// Required OpenHoldem version: 12.1.6
//
//******************************************************************************

// Needs to be defined here, before #include "user.h"
// to generate proper export- and inport-definitions
#define USER_DLL

#include "user.h"
#include <conio.h>
#include <windows.h>
#include <string>
#include "OpenHoldemFunctions.h"

#include "opython.h"

//******************************************************************************
//
// Place all your initalizations in the functions below
// DLLOnLoad() amd DLLOnUnLoad() get called by the DLL,
// the other functions get called by OpenHoldem
//
//******************************************************************************

void DLLOnLoad() {
	
}

void DLLOnUnLoad() {
	
}

void __stdcall DLLUpdateOnNewFormula() {
}

void __stdcall DLLUpdateOnConnection() {
}

void __stdcall DLLUpdateOnHandreset() {
}

void __stdcall DLLUpdateOnNewRound() {
}

void __stdcall DLLUpdateOnMyTurn() {
}

void __stdcall DLLUpdateOnHeartbeat() {
}


//******************************************************************************
//
// ProcessQuery()
// Handling the lookup of dll$symbols
//
//******************************************************************************

DLL_IMPLEMENTS double __stdcall ProcessQuery(const char* pquery) 
{
	if (pquery == NULL) 
	{
		return 0;
	}

	if (strncmp(pquery,"dll$test",9)==0) 
	{
		char* question = "blahdibah";
		double answer = 42.0;
		WriteLog("%s %f\n", question, answer);
		return GetSymbol("random");
	}

	if (strncmp(pquery, "dll$decision", 13) == 0)
	{
		double ret = pyDecision();
		WriteLog("pyDecision: %f\n", ret);
		return ret;
	}

	if (strncmp(pquery, "dll$scrape", 11) == 0) 
	{
		char* scraped_result;
		int result_lenght;
		scraped_result = ScrapeTableMapRegion("p0balance", result_lenght);
		if (scraped_result != nullptr) 
		{
			// The TEXT() macro supports both ASCII and Unicode.
			// For the people who use Unicode but don't understand the "error".
			// http://www.maxinmontreal.com/forums/viewtopic.php?f=174&t=19999
			// http://stackoverflow.com/questions/15498070/what-does-t-stands-for-in-a-cstring
			MessageBox(0, scraped_result, TEXT("Scraped custom region"), 0);
			LocalFree(scraped_result);
		}
	}

	return 0;
}

//******************************************************************************
//
// DLL entry point
//
//******************************************************************************

BOOL APIENTRY DllMain(HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
  switch (ul_reason_for_call)	{
		case DLL_PROCESS_ATTACH:
			AllocConsole();
			InitializeOpenHoldemFunctionInterface();
			pyInit();
			break;
		case DLL_THREAD_ATTACH:
			break;
		case DLL_THREAD_DETACH:
			break;
		case DLL_PROCESS_DETACH:
			pyDestroy();
			FreeConsole();
			break;
    }
  return TRUE;
}
