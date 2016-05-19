//******************************************************************************
//
// This file is part of the OpenHoldem project
//   Download page:         http://code.google.com/p/openholdembot/
//   Forums:                http://www.maxinmontreal.com/forums/index.php
//   Licensed under GPL v3: http://www.gnu.org/licenses/gpl.html
//
//******************************************************************************
//
// Purpose:
//
//******************************************************************************

#ifndef INC_POKERTRACKERQUERYDEFINITIONS_H
#define INC_POKERTRACKERQUERYDEFINITION_H

#ifdef POKERTRACKER_DLL_EXPORTS
#define POKERTRACKER_DLL_API extern "C" __declspec(dllexport)
#else
#define POKERTRACKER_DLL_API extern "C" __declspec(dllimport)
#endif

#include "atlstr.h"

const int kNumberOfPokerTrackerPostfixes = 11;
const char* const kPokerTrackerPostfixes[kNumberOfPokerTrackerPostfixes] = {
  "_raischair",
  "_headsup",
  "_smallblind",
  "_bigblind",
  "_dealer",
  "_cutoff",
  "_user",
  "_firstraiser",
  "_firstcaller",
  "_lastcaller",
  "_avg"
};

POKERTRACKER_DLL_API	void	  PT_DLL_Initialise();
POKERTRACKER_DLL_API	int		  PT_DLL_GetNumberOfStats();
POKERTRACKER_DLL_API	int		  PT_DLL_GetNumberOfStatTypes();
POKERTRACKER_DLL_API	CString PT_DLL_GetDescription(int stats_index);
POKERTRACKER_DLL_API	CString PT_DLL_GetBasicSymbolNameWithoutPTPrefix(int stats_index);
POKERTRACKER_DLL_API	bool	  PT_DLL_IsBasicStat(int stats_index);
POKERTRACKER_DLL_API	bool	  PT_DLL_IsPositionalPreflopStat(int stats_index);
POKERTRACKER_DLL_API	bool	  PT_DLL_IsAdvancedStat(int stats_index);
POKERTRACKER_DLL_API	double	PT_DLL_GetStat(CString symbol_name, int chair);
POKERTRACKER_DLL_API	double	PT_DLL_GetStatOpp(CString symbol_name, int chair);
POKERTRACKER_DLL_API	void	  PT_DLL_SetStat(int stats_index, int chair, double value, int opp);
POKERTRACKER_DLL_API	bool	  PT_DLL_IsValidSymbol(CString symbol_name);
POKERTRACKER_DLL_API	void	  PT_DLL_ClearPlayerStats(int chair);
POKERTRACKER_DLL_API	void	  PT_DLL_ClearAllPlayerStats();
POKERTRACKER_DLL_API	CString PT_DLL_GetQuery(int stats_index, 
												bool isomaha, bool istournament,
												int site_id, CString player_name);

#endif INC_POKERTRACKERQUERYDEFINITION_H