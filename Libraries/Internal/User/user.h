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
// DO NOT CHANGE ANYTHING IN THIS FILE!
//   
// This Header defines an interface
// Functions and data-types must exactly match.
//
//******************************************************************************

#pragma once
// Import and export directives
// for use by this DLL and by OpenHoldem
#ifdef USER_EXPORTS
#define DLL_IMPLEMENTS extern "C" __declspec(dllexport)
#define EXE_IMPLEMENTS extern "C" __declspec(dllimport)
#else
#define DLL_IMPLEMENTS extern "C" __declspec(dllimport)
#define EXE_IMPLEMENTS extern "C" __declspec(dllexport)
#endif

// Functions implemented and exported by the DLL,
// imported by OpenHoldem
DLL_IMPLEMENTS double __stdcall ProcessQuery(const char* pquery);
DLL_IMPLEMENTS void __stdcall DLLUpdateOnNewFormula();
DLL_IMPLEMENTS void __stdcall DLLUpdateOnConnection();
DLL_IMPLEMENTS void __stdcall DLLUpdateOnHandreset();
DLL_IMPLEMENTS void __stdcall DLLUpdateOnNewRound();
DLL_IMPLEMENTS void __stdcall DLLUpdateOnMyTurn();
DLL_IMPLEMENTS void __stdcall DLLUpdateOnHeartbeat();

// Please refer to the file OpenHoldemFunctions.h
// for functions that get exported by OpenHoldem
// and are available at the DLL-level.
