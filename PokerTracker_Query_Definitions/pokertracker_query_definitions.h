#pragma once
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

#ifdef POKERTRACKER_DLL_EXPORTS
#define POKERTRACKER_DLL_API extern "C" __declspec(dllexport)
#else
#define POKERTRACKER_DLL_API extern "C" __declspec(dllimport)
#endif

#ifdef USER_DLL
#define DLL_IMPLEMENTS extern "C" __declspec(dllexport)
#define EXE_IMPLEMENTS extern "C" __declspec(dllimport)
#else
#define DLL_IMPLEMENTS extern "C" __declspec(dllimport)
#define EXE_IMPLEMENTS extern "C" __declspec(dllexport)
#endif

#include "atlstr.h"

POKERTRACKER_DLL_API	int		  PT_DLL_GetNumberOfStats();
POKERTRACKER_DLL_API	CString PT_DLL_GetDescription(int stats_index);
POKERTRACKER_DLL_API	CString PT_DLL_GetBasicSymbolNameWithoutPTPrefix(int stats_index);
POKERTRACKER_DLL_API	bool	  PT_DLL_IsBasicStat(int stats_index);
POKERTRACKER_DLL_API	bool	  PT_DLL_IsPositionalPreflopStat(int stats_index);
POKERTRACKER_DLL_API	bool	  PT_DLL_IsAdvancedStat(int stats_index);
POKERTRACKER_DLL_API	double	PT_DLL_GetStat(CString symbol_name, int chair);
POKERTRACKER_DLL_API	void	  PT_DLL_SetStat(int stats_index, int chair, double value);
POKERTRACKER_DLL_API	bool	  PT_DLL_IsValidSymbol(CString symbol_name);
POKERTRACKER_DLL_API	void	  PT_DLL_ClearPlayerStats(int chair);
POKERTRACKER_DLL_API	void	  PT_DLL_ClearAllPlayerStats();
double __stdcall GetSymbol(const char* name_of_single_symbol__not_expression);
POKERTRACKER_DLL_API	CString PT_DLL_GetQuery(int stats_index, 
												bool isomaha, bool istournament, bool IsMTT, bool IsSNG, bool IsDON, bool IsTRIPLEUP, bool IsSHOOTOUT,
												bool IsFREEROLL, bool IsKNOCKOUT, bool IsREBUY, bool IsSATELITTE, bool IsSPIN,
												bool IsTURBO, bool IsSEMITURBO, bool IsSUPERTURBO, bool IsHYPERTURBO, bool IsULTRATURBO,
												int site_id, CString player_name, int table_size, double small_blind, double big_blind,
												bool isfinaltable, CString nb_hands, CString time_period);
POKERTRACKER_DLL_API CString nb_hands, time_period;

void InitializeOpenHoldemFunctionInterface();

typedef double (*pFunc)(int m_chr, int stat);
POKERTRACKER_DLL_API void pUpdateStat(pFunc pFunc);