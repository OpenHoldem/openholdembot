//******************************************************************************
//
// This file is part of the OpenHoldem project
//    Source code:           https://github.com/OpenHoldem/openholdembot/
//    Forums:                http://www.maxinmontreal.com/forums/index.php
//    Licensed under GPL v3: http://www.gnu.org/licenses/gpl.html
//
//******************************************************************************
//
// Purpose: pokertracker_query_definitions.cpp : Defines the entry point for the DLL application.
//
//******************************************************************************

#include "PokerTracker.h"
#include "Debugger/debugger.h"

#include "MagicNumbers/MagicNumbers.h"
#include "OpenHoldem/NumericalFunctions.h"
#include "PokerTracker_Queries_Version_4.h"

int PT_DLL_GetNumberOfStats() {
	return k_number_of_pokertracker_stats;
}

// We create queries on the fly, 
// so that they are usable for both ring-games and tournaments 
const char* const k_holdem_id  = "1";
const char* const k_omaha_id  = "2";
const char* const k_tournament_infix = "tourney";
const char* const k_cashgame_infix = "cash";

// Values of all stats for all players
double stats[k_number_of_pokertracker_stats][kMaxNumberOfPlayers];

CString PT_DLL_GetQuery(
	    int stats_index, 
      bool isomaha, 
      bool istournament,
	    int site_id, 
      CString player_name,
      double big_blind) {
	AssertRange(stats_index, 0, (k_number_of_pokertracker_stats - 1));
	CString query = query_definitions[stats_index].query;
	CString site_id_as_string;
	site_id_as_string.Format("%i", site_id);
	query.Replace("%SITEID%", site_id_as_string);
	query.Replace("%SCREENNAME%", player_name);
	query.Replace("%GAMETYPE%", (isomaha ? k_omaha_id : k_holdem_id) );
	query.Replace("%TYPE%", (istournament ? k_tournament_infix : k_cashgame_infix));
	return query;
}

CString PT_DLL_GetDescription(int stats_index) { 
	AssertRange(stats_index, 0, (k_number_of_pokertracker_stats - 1));
	return query_definitions[stats_index].description_for_editor; 
}

CString PT_DLL_GetBasicSymbolNameWithoutPTPrefix(int stats_index) {
	AssertRange(stats_index, 0, (k_number_of_pokertracker_stats - 1));
	return query_definitions[stats_index].name;
}	

bool PT_DLL_IsBasicStat(int stats_index) { 
	AssertRange(stats_index, 0, (k_number_of_pokertracker_stats - 1));
	return query_definitions[stats_index].stat_group == pt_group_basic; 
}

bool PT_DLL_IsPositionalPreflopStat(int stats_index) { 
	AssertRange(stats_index, 0, (k_number_of_pokertracker_stats - 1));
	return query_definitions[stats_index].stat_group == pt_group_positional;
}

bool PT_DLL_IsAdvancedStat(int stats_index) { 
	AssertRange(stats_index, 0, (k_number_of_pokertracker_stats - 1));
	return query_definitions[stats_index].stat_group == pt_group_advanced; 
}

// Not exported
CString PureSymbolName(CString symbol_name) {
	// Cut off "pt_" prefix for other chairs
	if (symbol_name.Left(3) == "pt_") {
		symbol_name = symbol_name.Right(symbol_name.GetLength() - 3);
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
	// This function can (and should) probably be optimized
	// by use of CMaps (binary trees).
	for (int i=0; i<k_number_of_pokertracker_stats; ++i) {
		if (symbol_name == query_definitions[i].name) {
			return i;
		}
	}
	return kUndefined;
}

double	PT_DLL_GetStat(CString symbol_name, int chair) {
	assert(symbol_name != "");
	symbol_name = PureSymbolName(symbol_name);
	AssertRange(chair, kFirstChair, kLastChair);
	int stats_index = GetIndex(symbol_name);
	if (stats_index == kUndefined) {
		return kUndefined;
	}
	return stats[stats_index][chair];
}

void PT_DLL_SetStat(int stats_index, int chair, double value) {
	AssertRange(stats_index, 0, (k_number_of_pokertracker_stats - 1));
	AssertRange(chair, kFirstChair, kLastChair);
	stats[stats_index][chair] = value;
}

bool PT_DLL_IsValidSymbol(CString symbol_name) {
	return (GetIndex(symbol_name) >= 0);
}

void PT_DLL_ClearPlayerStats(int chair) {
	AssertRange(chair, kFirstChair, kLastChair);
	for (int i=0; i<k_number_of_pokertracker_stats; ++i) {
		PT_DLL_SetStat(i, chair, kUndefined);
	}
}

void PT_DLL_ClearAllPlayerStats() {
	for (int i=0; i<kMaxNumberOfPlayers; ++i) {
		PT_DLL_ClearPlayerStats(i);
	}
}