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

#ifdef FPDB_DLL_EXPORTS
#define FPDB_DLL_API extern "C" __declspec(dllexport)
#else
#define FPDB_DLL_API extern "C" __declspec(dllimport)
#endif

#include "atlstr.h"

FPDB_DLL_API	int		  FPDB_DLL_GetNumberOfStats();
FPDB_DLL_API	CString FPDB_DLL_Getdescription(int stats_index);
FPDB_DLL_API	CString FPDB_DLL_GetBasicSymbolNameWithoutFPDBPrefix(int stats_index);
FPDB_DLL_API	bool	  FPDB_DLL_IsBasicStat(int stats_index);
FPDB_DLL_API	bool	  FPDB_DLL_IsPositionalPreflopStat(int stats_index);
FPDB_DLL_API	bool	  FPDB_DLL_IsAdvancedStat(int stats_index);
FPDB_DLL_API	double	FPDB_DLL_GetStat(CString symbol_name, int chair);
FPDB_DLL_API	void	  FPDB_DLL_SetStat(int stats_index, int chair, double value);
FPDB_DLL_API	bool	  FPDB_DLL_IsValidSymbol(CString symbol_name);
FPDB_DLL_API	void	  FPDB_DLL_ClearPlayerStats(int chair);
FPDB_DLL_API	void	  FPDB_DLL_ClearAllPlayerStats();
FPDB_DLL_API	CString FPDB_DLL_GetQuery(int stats_index, 
												bool isomaha, bool istournament,
												int site_id, CString player_name,
                        double big_blind);