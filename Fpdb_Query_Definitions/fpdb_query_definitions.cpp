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
#include <vector>
#include <regex>
#include <sstream>
using namespace std;


FPDB_DLL_API CString fpdb_nb_hands = "";
FPDB_DLL_API CString fpdb_time_period = "";

pFpdbFunc fpUpdateFpdbStat = nullptr;

FPDB_DLL_API int FPDB_DLL_GetNumberOfStats() {
	return k_number_of_fpdb_stats;
}

// We create queries on the fly, 
// so that they are usable for both ring-games and tournaments 
const char* const k_holdem_id  = "1";
const char* const k_omaha_id  = "2";
const char* const k_tournament_infix = "tourney";
const char* const k_cashgame_infix = "cash";
// last nb of hands and time period constants and variables
const char* const wrong_format = "You can't mix last nb of hands and last time period filters, you must choose either one of both... e.g. 'pt_12months_500hands_your_stat_name_raischair'";
CString period_intervals[] = { "years", "year", "months", "month", "weeks", "week", "days", "day", "hours", "hour", "minutes", "minute", "seconds", "second" };
CString tourney_types[] = { "MTT", "STT", "SNG", "DON", "BOUNTY", "SHOOTOUT", "REBUY", "MATRIX", "FAST", "POF", "SATELITTE", "STEPS", "DEEP", "MULTIENTRY", "FIFTY50", "ANTEUP", "FLIPOUT", "TRIPLEUP", "LOTTERY", "RE-ENTRY", "PROGBOUNT", "TIMEDALLIN", "Normal", "Semi-Turbo", "Turbo", "Super-Turbo", "Hyper-Turbo", "Ultra-Turbo", "0 max", "2 max", "3 max", "6 max", "8 max", "9 max", "10 max" };
CString player_positions[] = { "SB", "BB", "EP", "MP", "CO", "BTN" };
CString handstring = "hands";
double player_chair_number;						   

// Values of all stats for all players
double stats[k_number_of_fpdb_stats][kMaxNumberOfPlayers];

FPDB_DLL_API CString FPDB_DLL_GetQuery(
	int stats_index, 
    bool isomaha, bool istournament, 
	bool IsMTT, bool IsSNG, bool IsDON, bool IsTRIPLEUP, bool IsSHOOTOUT,
	bool IsFREEROLL, bool IsKNOCKOUT, bool IsREBUY, bool IsSATELLITE, bool IsSPIN,
	bool IsTURBO, bool IsSEMITURBO, bool IsSUPERTURBO, bool IsHYPERTURBO, bool IsULTRATURBO,
	int site_id, CString player_name, int table_size, int nplayersseated, double small_blind, double big_blind, double ante,
	bool isfinaltable, CString fpdb_nb_hands, CString fpdb_time_period) {
	
	AssertRange(stats_index, 0, (k_number_of_fpdb_stats - 1));
	CString query = query_definitions[stats_index].query;
	CString site_id_as_string;
	site_id_as_string.Format("%i", site_id);

	CString tourney_type = "";
	CString tourney_type_alt = "";
	CString speed_type = "";
	CString strPaidPlaces = "";
	CString strEffStackSize1 = "";
	CString strEffStackSize2 = "";
	CString Mfactor;
	Mfactor.Format("%f", (big_blind / (big_blind + small_blind + (ante*nplayersseated))));
	bool IsSTT = false;
	double M_zone = GetSymbol("Mzone");
	double effM_zone = GetSymbol("EffectiveMzone");
	double TableMzone = GetSymbol("TableMzone");
	double EffStackSize = GetSymbol("EffectiveActiveStack");
	double StartingBigBlindSize = GetSymbol("StartingBigBlindSize");
	double StartingStackSize = GetSymbol("StartingStackSize");
	double PaidPlaces = GetSymbol("PaidPlaces");
	double ShortStackStage = GetSymbol("ShortStackStage");
	double STTGameStage = GetSymbol("STTGameStage");  // 1 = EarlyStage, 2 = MiddleStage, 3 = LateStage, 4 = BubbleStage, 5 = ITMStage, 6 = HUStage, and special ShortStackStage
	double MTTGameStage = GetSymbol("MTTGameStage");  // 1 = Green Zone, 2 = Yellow Zone, 3 = Orange Zone, 4 = Red Zone, 5 = Dead Zone
	double STTSPinGameStage = GetSymbol("STTSPinGameStage");  // 1 = 3-Handed, 2 = Heads-Up
	double MTTSPinGameStage = GetSymbol("MTTSPinGameStage");  // 1 = EarlyStage, 2 = MiddleStage, 3 = Heads-Up

	if (ShortStackStage) { 
		strEffStackSize1.Format("%f", EffStackSize - 0.02*EffStackSize);
		strEffStackSize2.Format("%f", EffStackSize + 0.02*EffStackSize);
	};
	strPaidPlaces.Format("%f", PaidPlaces);
	if (STTGameStage == 5) strPaidPlaces.Format("%f", PaidPlaces + 1);
	//if (STTGameStage == 6) strPaidPlaces.Format("%", PaidPlaces);

	CString player_position = "";
	if (player_chair_number == GetSymbol("smallblindchair"))		player_position = "S";
	if (player_chair_number == GetSymbol("bigblindchair"))			player_position = "B";
	if (player_chair_number == GetSymbol("cutoffchair"))			player_position = "1";
	if (player_chair_number == (GetSymbol("mp2chair") || GetSymbol("mp3chair")))				player_position = "2";
	if (player_chair_number == GetSymbol("mp1chair"))				player_position = "3";
	if (player_chair_number == GetSymbol("ep3chair"))				player_position = "4";
	if (player_chair_number == GetSymbol("ep2chair"))				player_position = "5";
	if (player_chair_number == (GetSymbol("utgchair") || GetSymbol("ep1chair")))				player_position = "6";
	if (nplayersseated != 2 && player_chair_number == GetSymbol("buttonchair"))			
																	player_position = "0";

	//if (IsMTT) tourney_type.Append("%MTT");
	if (istournament && !IsMTT) {
		//tourney_type.Append("%STT"); IsSTT = true;
		IsSTT = true;
	}
	if (table_size == 0) tourney_type.Append("TT.maxSeats = 0 AND ");
	if (table_size == 2) tourney_type.Append("TT.maxSeats = 2 AND ");
	if (table_size == 3) tourney_type.Append("TT.maxSeats = 3 AND ");
	if (table_size == 6) tourney_type.Append("TT.maxSeats = 6 AND ");
	if (table_size == 8) tourney_type.Append("TT.maxSeats = 8 AND ");
	if (table_size == 9) tourney_type.Append("TT.maxSeats = 9 AND ");
	if (table_size == 10) tourney_type.Append("TT.maxSeats = 10 AND ");
	if (IsTURBO) speed_type = "%Turbo";
	else if (IsSEMITURBO) speed_type = "%Semi-Turbo";
	else if (IsSUPERTURBO) speed_type = "%Super-Turbo";
	else if (IsHYPERTURBO) speed_type = "%Hyper-Turbo";
	else if (IsULTRATURBO) speed_type = "%Ultra-Turbo";
	else speed_type = "%Normal";
	if (IsSNG) tourney_type.Append("TT.sng = 1 AND ");
	if (IsDON) tourney_type.Append("TT.doubleOrNothing = 1 AND ");
	//if (IsTRIPLEUP) tourney_type.Append("%TRIPLEUP");
	if (IsSHOOTOUT) tourney_type.Append("TT.shoutout = 1 AND ");
	if (IsSATELLITE) tourney_type.Append("TT.satellite = 1 AND ");
	//if (IsSPIN) tourney_type.Append("%LOTTERY");
	if (IsREBUY) tourney_type.Append("TT.rebuy = 1 AND ");
	//if (IsKNOCKOUT) tourney_type_alt.Append(tourney_type);
	if (IsKNOCKOUT) tourney_type.Append("TT.knockout = 1 AND ");
	//if (IsKNOCKOUT) tourney_type_alt.Append("%PROGBOUNT");
	if (tourney_type != "") {
		tourney_type = tourney_type.Left(tourney_type.GetLength() - 5);
		tourney_type = "AND (" + tourney_type + ")";
	}
	query.Replace("%SITEID%", site_id_as_string);
	query.Replace("%SCREENNAME%", player_name);
	query.Replace("%GAMETYPE%", (istournament ? "tour" : "ring"));			  
	query.Replace("%GAMECAT%", (istournament ? "holdem" : "omaha") );
	//query.Replace("%TYPELIMIT%", (istournament ? ", tourney_blinds as TL" : ", cash_limit as TL"));
	//query.Replace("%LIMITNAME%", (istournament ? "TL.blinds_name" : "TL.limit_name"));

	/// Cash-Game  ////////////////
	if (!IsMTT && !IsSTT)
		query.Replace("%LIMIT&STAGESTATEMENT%", "HP.bigblind = G.bigblind AND");

	////////////////////////////////////////////////////////////

	///  For Spins   ////////////////////////////////////////

	if (IsSTT && IsSPIN) {
		if (STTSPinGameStage == 1)			// 3-Handed Stage
			query.Replace("%LIMIT&STAGESTATEMENT%", "HP.bigblind = G.bigblind AND %SHORTSTACKSTATEMENT% H.seats = 3 AND");
		if (STTSPinGameStage == 2)			// Heads-Up Stage
			query.Replace("%LIMIT&STAGESTATEMENT%", "HP.bigblind = G.bigblind AND %SHORTSTACKSTATEMENT% H.seats = 2 AND");
											// ShortStack Stage
		query.Replace("%SHORTSTACKSTATEMENT%", (ShortStackStage ? "(HP.effActiveStack / 100 >= " + strEffStackSize1 + " AND HP.effActiveStack / 100 <= " + strEffStackSize2 + ") AND" : ""));
	}

	if (IsMTT && IsSPIN) {
		if (StartingStackSize == 25) {
			if (MTTSPinGameStage == 1)		// Early Stage
				query.Replace("%LIMIT&STAGESTATEMENT%", "HP.bigblind = G.bigblind AND %SHORTSTACKSTATEMENT% HP.effActiveMzone > 17*" + Mfactor + " AND");
			if (MTTSPinGameStage == 2)		// Middle Stage
				query.Replace("%LIMIT&STAGESTATEMENT%", "HP.bigblind = G.bigblind AND %SHORTSTACKSTATEMENT% HP.effActiveMzone > 12*" + Mfactor + " AND HP.effActiveMzone <= 17*" + Mfactor + " AND");
		}
		if (StartingStackSize == 15) {
			if (MTTSPinGameStage == 1)		// Early Stage
				query.Replace("%LIMIT&STAGESTATEMENT%", "HP.bigblind = G.bigblind AND %SHORTSTACKSTATEMENT% HP.effActiveMzone > 10*" + Mfactor + " AND");
			if (MTTSPinGameStage == 2)		// Middle Stage
				query.Replace("%LIMIT&STAGESTATEMENT%", "HP.bigblind = G.bigblind AND %SHORTSTACKSTATEMENT% HP.effActiveMzone > 5*" + Mfactor + " AND HP.effActiveMzone <= 10*" + Mfactor + " AND");
		}
		if (MTTSPinGameStage == 3)			// Heads-Up Stage
			query.Replace("%LIMIT&STAGESTATEMENT%", "HP.bigblind = G.bigblind AND %SHORTSTACKSTATEMENT% H.seats = 2 AND");
											// ShortStack Stage
		query.Replace("%SHORTSTACKSTATEMENT%", (ShortStackStage ? "(HP.effActiveStack / 100 >= " + strEffStackSize1 + " AND HP.effActiveStack / 100 <= " + strEffStackSize2 + ") AND" : ""));
	}


	///  For STT SNGs   ////////////////////////////////////////

	CString EarlyStageStatement =  "HP.effActiveMzone >= 12*" + Mfactor + " AND \
									G.bigblind <= 3 * (select distinct first_value(gt.bigblind) over (order by hd.startTime) \
									from HandsPlayers as hdp, Hands as hd, Tourneys as tn, Gametypes as gt \
									where hdp.handId = hd.Id and hdp.seatNo = hd.heroSeat and hd.tourneyId = tn.id and hd.gametypeId = gt.id and gt.type = 'tour' and gt.category LIKE 'holdem' and hdp.bigblind = gt.bigblind)";
	CString LateStageStatement =   "(HP.effActiveMzone <= 10*" + Mfactor + " OR (G.bigblind >= 100*100 AND HP.effActiveMzone < 12*" + Mfactor + ") \
									OR (G.bigblind >= 160*100 AND (HP.tableMzone  < 12 OR HP.effActiveMzone < 10)))";

	if ((IsSTT || isfinaltable) && !IsSPIN) {
		if (STTGameStage == 1)		// Early Stage
			query.Replace("%LIMIT&STAGESTATEMENT%", "HP.bigblind = G.bigblind AND %SHORTSTACKSTATEMENT% " + EarlyStageStatement + " AND");
		if (STTGameStage == 2)		// Middle Stage
			query.Replace("%LIMIT&STAGESTATEMENT%", "HP.bigblind = G.bigblind AND %SHORTSTACKSTATEMENT% NOT(" + EarlyStageStatement + ") AND NOT(" + LateStageStatement + ") AND H.seats > " + strPaidPlaces + " AND H.seats != " + strPaidPlaces + "+1 AND H.seats != 2 AND");
		if (STTGameStage == 3)		// Late Stage
			query.Replace("%LIMIT&STAGESTATEMENT%", "HP.bigblind = G.bigblind AND %SHORTSTACKSTATEMENT% " + LateStageStatement + " AND");
		if (STTGameStage == 4) 		// Bubble Stage
			query.Replace("%LIMIT&STAGESTATEMENT%", "HP.bigblind = G.bigblind AND %SHORTSTACKSTATEMENT% H.seats = " + strPaidPlaces + " AND");
		if (STTGameStage == 5) 		// ITM Stage
			query.Replace("%LIMIT&STAGESTATEMENT%", "HP.bigblind = G.bigblind AND %SHORTSTACKSTATEMENT% H.seats <= " + strPaidPlaces + " AND");
		if (STTGameStage == 6)		// HU Stage
			query.Replace("%LIMIT&STAGESTATEMENT%", "HP.bigblind = G.bigblind AND %SHORTSTACKSTATEMENT% H.seats = 2 AND");
									// ShortStack Stage
		query.Replace("%SHORTSTACKSTATEMENT%", (ShortStackStage ? "(HP.effActiveStack / 100 >= " + strEffStackSize1 + " AND HP.effActiveStack / 100 <= " + strEffStackSize2 + ") AND" : ""));
	}

	////////////////////////////////////////////////////////////

	///  For MTTs   ////////////////////////////////////////////

	if (IsMTT &&  !isfinaltable && !IsSPIN) {
		if (MTTGameStage == 1)		// Green Zone
			query.Replace("%LIMIT&STAGESTATEMENT%", "HP.bigblind = G.bigblind AND HP.effActiveMzone >= 20 AND");
		if (MTTGameStage == 2)		// Yellow Zone
			query.Replace("%LIMIT&STAGESTATEMENT%", "HP.bigblind = G.bigblind AND (HP.effActiveMzone >= 13 AND HP.effActiveMzone < 20) AND");
		if (MTTGameStage == 3)		// Orange Zone
			query.Replace("%LIMIT&STAGESTATEMENT%", "HP.bigblind = G.bigblind AND (HP.effActiveMzone >= 6 AND HP.effActiveMzone < 13) AND");
		if (MTTGameStage == 4)		// Red Zone
			query.Replace("%LIMIT&STAGESTATEMENT%", "HP.bigblind = G.bigblind AND %SHORTSTACKSTATEMENT% (HP.effActiveMzone >= 3 AND HP.effActiveMzone < 6) AND");
		if (MTTGameStage == 5)		// Dead Zone
			query.Replace("%LIMIT&STAGESTATEMENT%", "HP.bigblind = G.bigblind AND (HP.effActiveMzone >= 0 AND HP.effActiveMzone < 3) AND");
									// Red Zone - ShortStack Stage
		query.Replace("%SHORTSTACKSTATEMENT%", (ShortStackStage ? "(HP.effActiveStack / 100 >= " + strEffStackSize1 + " AND HP.effActiveStack / 100 <= " + strEffStackSize2 + ") AND" : ""));
	}
	
	////////////////////////////////////////////////////////////

	query.Replace("%TOURNEYTYPESELECT%", (istournament && tourney_type != "" ? ", Tourneys as T, TourneysPlayers as TP, TourneyTypes as TT" : ""));
	//if (tourney_type_alt != "")
	//	query.Replace("%TOURNEYTABLETYPE%", "H.tourneyId = TP.tourneyId AND TP.tourneyId = T.id  AND T.tourneyTypeId = TT.id AND TT.speed LIKE '" + speed_type + "%' %TOURNEYTYPE% AND");
	query.Replace("%TOURNEYTABLETYPE%", (istournament && tourney_type != "" ? "H.tourneyId = TP.tourneyId AND TP.tourneyId = T.id  AND T.tourneyTypeId = TT.id AND TT.speed LIKE '" + speed_type + "%' %TOURNEYTYPE% AND" : ""));
	query.Replace("%TOURNEYTYPE%", (istournament && tourney_type != "" ? tourney_type : ""));
	//query.Replace("%TOURNEYTYPE_ALT%", (istournament && tourney_type_alt != "" ? tourney_type_alt : ""));
	//query.Replace("%PLAYERPOSITION%", (player_position != "" ? ", lookup_positions as LP" : ""));
	query.Replace("%POSITIONSTATEMENT%", (player_position != "" ? "HP.position = '" + player_position + "' AND" : ""));
	if (fpdb_nb_hands == "")
		query.Replace("LIMIT %NBHANDS% \t\t", "");
	else
		query.Replace("%NBHANDS%", fpdb_nb_hands);
	if (fpdb_time_period == "")
		query.Replace("H.startTime > date('now','-%TIMEPERIOD%') AND \t\t\t\t", "");
	else
		query.Replace("%TIMEPERIOD%", fpdb_time_period);			

	return query;
}

FPDB_DLL_API CString FPDB_DLL_GetDescription(int stats_index) { 
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
vector<CString> PureSymbolName(CString symbol_name) {
	// Cut off "fpdb_" prefix for other chairs
	if (symbol_name.Left(5) == "fpdb_") {
		fpdb_nb_hands = "";
		fpdb_time_period = "";	   
		symbol_name = symbol_name.Right(symbol_name.GetLength() - 5);
		string _symbol_name = (string)symbol_name;
		smatch symbol_match;

		
		if (regex_match(_symbol_name, symbol_match, regex("([0-9]+)" + handstring + "_([a-zA-Z0-9_]+)"))) {
			fpdb_nb_hands = symbol_match[1].str().c_str();
			_symbol_name = symbol_match[2];
		}
		for (int i = 0; i < size(period_intervals); i++) { // Look for a string from period_intervals[] in the query
			if (regex_match(_symbol_name, symbol_match, regex("([0-9]+)" + period_intervals[i] + "_([a-zA-Z0-9_]+)"))) {
				if (fpdb_nb_hands == "") {
					fpdb_time_period = symbol_match[1].str().c_str(); fpdb_time_period.AppendChar(' ');
					fpdb_time_period = fpdb_time_period + period_intervals[i];
					_symbol_name = symbol_match[2];
				}   else symbol_name = wrong_format;
				break; // Found period_intervals in the substring, break the loop here
			}
		}

		symbol_name = _symbol_name.c_str();						 
		// Cut off chair number at the right end
		char last_character = symbol_name[symbol_name.GetLength() - 1];
		if (isdigit(last_character))
			symbol_name = symbol_name.Left(symbol_name.GetLength() - 1);
		player_chair_number = atof(&last_character);
	}
	vector<CString> pureSymbol;
	pureSymbol.push_back(symbol_name);

	return pureSymbol; // first element: symbol_name (pure symbol name) - second element: fpdb_nb_hands - third element: fpdb_time_period				
}

// Not exported
int GetIndex(CString symbol_name) {
	assert(symbol_name != "");
	symbol_name = PureSymbolName(symbol_name)[0];
	// This function can (and should) probably be optimized
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
	symbol_name = PureSymbolName(symbol_name)[0];
	AssertRange(chair, kFirstChair, kLastChair);
	int stats_index = GetIndex(symbol_name);

	fpUpdateFpdbStat(chair, stats_index);

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
	for (int i = 0; i < kMaxNumberOfPlayers; ++i) {
		FPDB_DLL_ClearPlayerStats(i);
	}
}

FPDB_DLL_API void pUpdateFpdbStat(pFpdbFunc pFunc)
{
	fpUpdateFpdbStat = pFunc;
}


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

typedef double(*t_GetSymbol)(const char* name_of_single_symbol__not_expression);
t_GetSymbol p_GetSymbol = nullptr;

void ErrorPointerNotInitialized(CString function_name);

double __stdcall GetSymbol(const char* name_of_single_symbol__not_expression) {
	if (p_GetSymbol == nullptr) {
		ErrorPointerNotInitialized("GetSymbol");
		return 0.0;
	}
	return p_GetSymbol(name_of_single_symbol__not_expression);
}

BOOL APIENTRY DllMain(HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
	switch (ul_reason_for_call) {
	case DLL_PROCESS_ATTACH:
#ifdef _DEBUG
		AllocConsole();
#endif _DEBUG
		InitializeOpenHoldemFunctionInterface();
		DLLOnLoad();
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		DLLOnUnLoad();
#ifdef _DEBUG
		FreeConsole();
#endif _DEBUG
		break;
	}
	return TRUE;
}

FARPROC WINAPI LookupOpenHoldemFunction(char* function_name) {
	// https://msdn.microsoft.com/en-us/library/windows/desktop/ms683199(v=vs.85).aspx
	HMODULE openholdem_main_module = GetModuleHandle(NULL);
	if (openholdem_main_module == nullptr) {
		return nullptr;
	}
	// https://msdn.microsoft.com/en-us/library/windows/desktop/ms683212(v=vs.85).aspx
	FARPROC WINAPI function_address = GetProcAddress(openholdem_main_module, function_name);
	if (function_address == nullptr) {
		CString error_message;
		error_message.Format("Can not find %s in Openholdem.exe.\n",
			function_name);
		MessageBox(0,
			error_message,
			"DLL Error",
			MB_ICONERROR);
	}
	return function_address;
}

void InitializeOpenHoldemFunctionInterface() {
	p_GetSymbol = (t_GetSymbol)LookupOpenHoldemFunction("GetSymbol");
}

void ErrorPointerNotInitialized(CString function_name) {
	CString error_message;
	error_message.Format("OpenHoldem interface not yet initialized.\n"
		"Can't use function %s.\n",
		function_name);
	MessageBox(0,
		error_message,
		"DLL Error",
		MB_ICONERROR);
}