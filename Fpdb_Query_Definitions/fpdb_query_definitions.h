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

#ifdef USER_DLL
#define DLL_IMPLEMENTS extern "C" __declspec(dllexport)
#define EXE_IMPLEMENTS extern "C" __declspec(dllimport)
#else
#define DLL_IMPLEMENTS extern "C" __declspec(dllimport)
#define EXE_IMPLEMENTS extern "C" __declspec(dllexport)
#endif

#include "atlstr.h"

FPDB_DLL_API	int		  FPDB_DLL_GetNumberOfStats();
FPDB_DLL_API	CString FPDB_DLL_GetDescription(int stats_index);
FPDB_DLL_API	CString FPDB_DLL_GetBasicSymbolNameWithoutFPDBPrefix(int stats_index);
FPDB_DLL_API	bool	  FPDB_DLL_IsBasicStat(int stats_index);
FPDB_DLL_API	bool	  FPDB_DLL_IsPositionalPreflopStat(int stats_index);
FPDB_DLL_API	bool	  FPDB_DLL_IsAdvancedStat(int stats_index);
FPDB_DLL_API	double	FPDB_DLL_GetStat(CString symbol_name, int chair);
FPDB_DLL_API	void	  FPDB_DLL_SetStat(int stats_index, int chair, double value);
FPDB_DLL_API	bool	  FPDB_DLL_IsValidSymbol(CString symbol_name);
FPDB_DLL_API	void	  FPDB_DLL_ClearPlayerStats(int chair);
FPDB_DLL_API	void	  FPDB_DLL_ClearAllPlayerStats();
double __stdcall GetSymbol(const char* name_of_single_symbol__not_expression);															  
FPDB_DLL_API	CString FPDB_DLL_GetQuery(int stats_index, 
												bool isomaha, bool istournament, bool IsMTT, bool IsSNG, bool IsDON, bool IsTRIPLEUP, bool IsSHOOTOUT,
												bool IsFREEROLL, bool IsKNOCKOUT, bool IsREBUY, bool IsSATELITTE, bool IsSPIN,
												bool IsTURBO, bool IsSEMITURBO, bool IsSUPERTURBO, bool IsHYPERTURBO, bool IsULTRATURBO,
												int site_id, CString player_name, int table_size, int nplayersseated, double small_blind, double big_blind, double ante,
												bool isfinaltable, CString fpdb_nb_hands, CString fpdb_time_period);
FPDB_DLL_API CString fpdb_nb_hands, fpdb_time_period;

void InitializeOpenHoldemFunctionInterface();

typedef double (*pFpdbFunc)(int m_chr, int stat);
FPDB_DLL_API void pUpdateFpdbStat(pFpdbFunc pFunc);