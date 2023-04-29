//******************************************************************************
//
// This file is part of the OpenHoldem project
//    Source code:           https://github.com/OpenHoldem/openholdembot/
//    Forums:                http://www.maxinmontreal.com/forums/index.php
//    Licensed under GPL v3: http://www.gnu.org/licenses/gpl.html
//
//******************************************************************************
//
// Purpose: fpdb_query_definitions.cpp : Defines the entry point for the DLL application.
//
//******************************************************************************

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers
#endif

// Modify the following defines if you have to target a platform prior to the ones specified below.
// Refer to MSDN for the latest info on corresponding values for different platforms.
#ifndef WINVER				// Allow use of features specific to Windows XP or later.
#define WINVER 0x0501		// Change this to the appropriate value to target other versions of Windows.
#endif

#ifndef _WIN32_WINNT		// Allow use of features specific to Windows XP or later.                   
#define _WIN32_WINNT 0x0501	// Change this to the appropriate value to target other versions of Windows.
#endif						

#ifndef _WIN32_WINDOWS		// Allow use of features specific to Windows 98 or later.
#define _WIN32_WINDOWS 0x0410 // Change this to the appropriate value to target Windows Me or later.
#endif

#ifndef _WIN32_IE			// Allow use of features specific to IE 6.0 or later.
#define _WIN32_IE 0x0600	// Change this to the appropriate value to target other versions of IE.
#endif

// This definition will trigger the generation of a DLL-export-table 
// in the header-file "fpdb_query_definitions.h".
// Without this macro FPDB_DLL_API will cause an import-table
// for the users of this DLL.
#define FPDB_DLL_EXPORTS

#include <windows.h>
#include "fpdb_query_definitions.h"

#include <atlstr.h>
#include "..\Shared\MagicNumbers\MagicNumbers.h"
#include "..\OpenHoldem\NumericalFunctions.h"
#include "Fpdb_Queries_Version_4.h"

FPDB_DLL_API int FPDB_DLL_GetNumberOfStats() {
	return k_number_of_fpdb_stats;
}

// We create queries on the fly, 
// so that they are usable for both ring-games and tournaments 
const char* const k_holdem_id  = "1";
const char* const k_omaha_id  = "2";
const char* const k_tournament_infix = "tourney";
const char* const k_cashgame_infix = "cash";

// Values of all stats for all players
double stats[k_number_of_fpdb_stats][kMaxNumberOfPlayers];

FPDB_DLL_API CString FPDB_DLL_GetQuery(
	    int stats_index, 
      bool isomaha, 
      bool istournament,
	    int site_id, 
      CString player_name,
      double big_blind) {
	AssertRange(stats_index, 0, (k_number_of_fpdb_stats - 1));
	CString query = query_definitions[stats_index].query;
	CString site_id_as_string;
	site_id_as_string.Format("%i", site_id);
	query.Replace("%SITEID%", site_id_as_string);
	query.Replace("%SCREENNAME%", player_name);
	query.Replace("%GAMETYPE%", (istournament ? "tour" : "ring"));
	query.Replace("%GAMECAT%", (isomaha ? "omaha" : "holdem") );
	return query;
}

FPDB_DLL_API CString FPDB_DLL_Getdescription(int stats_index) { 
	AssertRange(stats_index, 0, (k_number_of_fpdb_stats - 1));
	return query_definitions[stats_index].description_for_editor; 
}

FPDB_DLL_API CString FPDB_DLL_GetBasicSymbolNameWithoutFPDBPrefix(int stats_index) {
	AssertRange(stats_index, 0, (k_number_of_fpdb_stats - 1));
	return query_definitions[stats_index].name;
}	

FPDB_DLL_API bool FPDB_DLL_IsBasicStat(int stats_index) { 
	AssertRange(stats_index, 0, (k_number_of_fpdb_stats - 1));
	return query_definitions[stats_index].stat_group == fpdb_group_basic; 
}

FPDB_DLL_API bool FPDB_DLL_IsPositionalPreflopStat(int stats_index) { 
	AssertRange(stats_index, 0, (k_number_of_fpdb_stats - 1));
	return query_definitions[stats_index].stat_group == fpdb_group_positional;
}

FPDB_DLL_API bool FPDB_DLL_IsAdvancedStat(int stats_index) { 
	AssertRange(stats_index, 0, (k_number_of_fpdb_stats - 1));
	return query_definitions[stats_index].stat_group == fpdb_group_advanced; 
}

// Not exported
CString PureSymbolName(CString symbol_name) {
	// Cut off "fpdb_" prefix for other chairs
	if (symbol_name.Left(5) == "fpdb_") {
		symbol_name = symbol_name.Right(symbol_name.GetLength() - 5);
		// Cut off chair number at the right end
		char last_character = symbol_name[symbol_name.GetLength() - 1];
		if (isdigit(last_character)) {
			symbol_name = symbol_name.Left(symbol_name.GetLength() - 1);
		}	
  }
  return symbol_name;
}

// Not exported
int GetIndex(CString symbol_name) {
	assert(symbol_name != "");
	symbol_name = PureSymbolName(symbol_name);
	// This function can (and should) probably be ofpdbimized
	// by use of CMaps (binary trees).
	for (int i=0; i<k_number_of_fpdb_stats; ++i) {
		if (symbol_name == query_definitions[i].name) {
			return i;
		}
	}
	return kUndefined;
}

FPDB_DLL_API double	FPDB_DLL_GetStat(CString symbol_name, int chair) {
	assert(symbol_name != "");
	symbol_name = PureSymbolName(symbol_name);
	AssertRange(chair, kFirstChair, kLastChair);
	int stats_index = GetIndex(symbol_name);
	if (stats_index == kUndefined) {
		return kUndefined;
	}
	return stats[stats_index][chair];
}

FPDB_DLL_API void FPDB_DLL_SetStat(int stats_index, int chair, double value) {
	AssertRange(stats_index, 0, (k_number_of_fpdb_stats - 1));
	AssertRange(chair, kFirstChair, kLastChair);
	stats[stats_index][chair] = value;
}

FPDB_DLL_API bool FPDB_DLL_IsValidSymbol(CString symbol_name) {
	return (GetIndex(symbol_name) >= 0);
}

FPDB_DLL_API void FPDB_DLL_ClearPlayerStats(int chair) {
	AssertRange(chair, kFirstChair, kLastChair);
	for (int i=0; i<k_number_of_fpdb_stats; ++i) {
		FPDB_DLL_SetStat(i, chair, kUndefined);
	}
}

FPDB_DLL_API void FPDB_DLL_ClearAllPlayerStats() {
	for (int i=0; i<kMaxNumberOfPlayers; ++i) {
		FPDB_DLL_ClearPlayerStats(i);
	}
}