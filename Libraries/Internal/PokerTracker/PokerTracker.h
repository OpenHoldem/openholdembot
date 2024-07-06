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

#pragma once
#ifndef WINVER				// Allow use of features specific to Windows XP or later.
#define WINVER 0x0501
#endif

#include <afxwin.h>

int		  PT_DLL_GetNumberOfStats();
CString	  PT_DLL_GetDescription(int stats_index);
CString   PT_DLL_GetBasicSymbolNameWithoutPTPrefix(int stats_index);
bool	  PT_DLL_IsBasicStat(int stats_index);
bool	  PT_DLL_IsPositionalPreflopStat(int stats_index);
bool	  PT_DLL_IsAdvancedStat(int stats_index);
double	  PT_DLL_GetStat(CString symbol_name, int chair);
void	  PT_DLL_SetStat(int stats_index, int chair, double value);
bool	  PT_DLL_IsValidSymbol(CString symbol_name);
void	  PT_DLL_ClearPlayerStats(int chair);
void	  PT_DLL_ClearAllPlayerStats();
CString   PT_DLL_GetQuery(int stats_index, 
						bool isomaha, bool istournament,
						int site_id, CString player_name,
						double big_blind);
