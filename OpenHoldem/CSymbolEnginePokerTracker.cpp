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

#include "stdafx.h"
#include "CSymbolEnginePokerTracker.h"
#include "CSymbolEngineActiveDealtPlaying.h"

#include "CEngineContainer.h"
#include "CFormulaParser.h"
#include "CParseErrors.h"
#include "CParseTreeTerminalNodeEndOfFunction.h"
#include "PokerTracker/PokerTracker.h"
#include "CPokerTrackerThread.h"

#include "CSymbolEngineCallers.h"
#include "CSymbolEngineChairs.h"
#include "CSymbolengineDealerchair.h"
#include "CSymbolEngineIsRush.h"
#include "CSymbolEngineRaisers.h"
#include "CSymbolEngineUserchair.h"
#include "WindowFunctions/window_functions.h"
#include "StringFunctions/string_functions.h"

CSymbolEnginePokerTracker::CSymbolEnginePokerTracker()
{
	// The values of some symbol-engines depend on other engines.
	// As the engines get later called in the order of initialization
	// we assure correct ordering by checking if they are initialized.
	assert(p_engine_container->symbol_engine_isrush() != NULL); 
	assert(p_engine_container->symbol_engine_raisers() != NULL);
	assert(p_engine_container->symbol_engine_userchair() != NULL);
	assert(p_engine_container->symbol_engine_active_dealt_playing() != NULL);
}

CSymbolEnginePokerTracker::~CSymbolEnginePokerTracker() {
}

void CSymbolEnginePokerTracker::InitOnStartup() {
	check_for_identity_of_players_executed_this_heartbeat = false;
	ClearAllStats();
}

void CSymbolEnginePokerTracker::UpdateOnConnection() {
	ClearAllStats();
	p_pokertracker_thread->StartThread();
}

void CSymbolEnginePokerTracker::UpdateOnHandreset() {
  if (p_engine_container->symbol_engine_isrush()->isrush())
    ClearAllStats();
}

void CSymbolEnginePokerTracker::UpdateOnNewRound()
{}

void CSymbolEnginePokerTracker::UpdateOnMyTurn()
{}

void CSymbolEnginePokerTracker::UpdateOnHeartbeat()
{
	check_for_identity_of_players_executed_this_heartbeat = false;
}

void CSymbolEnginePokerTracker::WarnAboutInvalidPTSymbol(CString s) {
	CString error_message;
  error_message.Format("Invalid PokerTracker-symbol: %s\n", s);
	CParseErrors::MessageBox_Formula_Error(error_message, "Error");
}

bool CSymbolEnginePokerTracker::IsOldStylePTSymbol(CString s) {
  if (s.Left(2) == "pt") {
    return false;
  }
  if (s.Left(3) != "pt_") {
    return true;
  } 
  if (s.Left(5) == "pt_r_") {
    return true;
  }
  return false;
}

void CSymbolEnginePokerTracker::CheckForChangedPlayersOncePerHeartbeatAndSymbolLookup() {
	if (check_for_identity_of_players_executed_this_heartbeat)
	{
		return;
	}
	check_for_identity_of_players_executed_this_heartbeat = true;
	ClearAllStatsOfChangedPlayers();
}

void CSymbolEnginePokerTracker::ClearSeatStats(int chair, bool clearNameAndFound) {
	assert(chair >= kFirstChair); 
	assert(chair <= kLastChair);
	write_log(Preferences()->debug_pokertracker(), "[CSymbolEnginePokerTracker] ClearSeatStats() for chair %i\n", chair);
	PT_DLL_ClearPlayerStats(chair);
	if (clearNameAndFound) {
		_player_data[chair].found = false;
		memset(_player_data[chair].pt_name, 0, kMaxLengthOfPlayername);
		memset(_player_data[chair].scraped_name, 0, kMaxLengthOfPlayername);
	}
	_player_data[chair].skipped_updates = k_advanced_stat_update_every;
}

void CSymbolEnginePokerTracker::ClearAllStatsOfChangedPlayers() {
	write_log(Preferences()->debug_pokertracker(), "[CSymbolEnginePokerTracker] Executing ClearAllStatsOfChangedPlayers()\n");
	for (int i=0; i<kMaxNumberOfPlayers; i++)
	{
		if (p_pokertracker_thread->CheckIfNameHasChanged(i))
		{
			ClearSeatStats(i, true);
		}
	}
}

void CSymbolEnginePokerTracker::ClearAllStats()
{
	for (int i=0; i<kMaxNumberOfPlayers; i++)
	{
		ClearSeatStats(i, true);
	}
}

int CSymbolEnginePokerTracker::PlayerIcon(const int chair) {
  assert(chair >= 0);
  assert(chair <= kLastChair);
  return PT_DLL_GetStat("icon", chair);
}

bool CSymbolEnginePokerTracker::EvaluateSymbol(const CString name, double *result, bool log /* = false */) {
  FAST_EXIT_ON_OPENPPL_SYMBOLS(name);
	if (memcmp(name, "pt_", 3) != 0) {
		// Symbol of a different symbol-engine
		return false;
	}
	CheckForChangedPlayersOncePerHeartbeatAndSymbolLookup();
	if (IsOldStylePTSymbol(name))	{
		CString error_message;
		error_message.Format(
			"Old style PokerTracker symbol detected: %s.\n"
			"\n"
			"PokerTracker symbol start with \"pt_\".\n"
      "Possible postfixes:\n"
      "  * chair number (0..9)\n"
      "  * _raischair\n"
      "  * _headsupchair\n"
      "  * _smallblindchair\n"
      "  * _bigblindchair\n"
      "  * _dealerchair\n"
      "  * _cutoffchair\n"
      "  * _userchair\n"
      "  * _firstraiserchair\n"
      "  * _firstcallerchair\n"
      "  * _lastcallerchair\n", name);
		CParseErrors::MessageBox_Formula_Error(
			error_message,			 
			"ERROR: Invalid PokerTracker Symbol");
		*result = kUndefined;
		return true;
	}
	if (!PT_DLL_IsValidSymbol(CString(name)))	{
		// Invalid PokerTracker symbol
		WarnAboutInvalidPTSymbol(name);
		*result = kUndefined;
		return true;
	}
	int chair = 0;
	if (!p_pokertracker_thread->IsConnected()) 	{
		if (!p_engine_container->symbol_engine_userchair()->userchair_confirmed() || p_formula_parser->IsParsing()) {
			// We are not yet seated or formula is getting parsed.
			// Symbol-lookup happens, because of Formula-validation.
			// Not a problem, if we do not yet have a DB-connection.
			// Don't throw a warning here.
      write_log(Preferences()->debug_pokertracker(), "[PokerTracker] Not yet seated or formula parsing.\n");
		} else {
			// We are seated and playing, use a PT-symbol,
      // but are noit connected to a database
      if (CParseTreeTerminalNodeEndOfFunction::evaluating_defailt_logic()) {
        // Not a problem
        // The default-bot uses PT-symbols,
        // but also plays well against unknown.
        // Lots of people will use it without a DB
      } else {
        // Serious problem: PT-usage in user-code
        MessageBox_Error_Warning("Not connected to PokerTracker database.\n"
          "Can't use PokerTracker symbols.");
      }
		}
		*result = kUndefined;
		return true;
	}	else {
		CString last_character = name.Right(1);
    if (!isdigit(last_character[0])) {
      CString error_message;
      error_message.Format("Invalid PokerTracker Symbol: %s",
        name);
      CParseErrors::MessageBox_Formula_Error(error_message, "ERROR");
		  *result = kUndefined;
      return false;
    }
		chair = atoi(last_character);
	}


  // Catch undefined chair (e.g. pt_r_-symbol without raisee)
  if (chair < 0) {
    *result = kUndefined;
    return true;
  }
	AssertRange(chair, kFirstChair, kLastChair);
	*result = PT_DLL_GetStat(name, chair); 
	return true;
}

CString CSymbolEnginePokerTracker::SymbolsProvided() {
  CString list;
  for (int i=0; i<PT_DLL_GetNumberOfStats(); ++i) {
    CString basic_symbol_name = PT_DLL_GetBasicSymbolNameWithoutPTPrefix(i);
    // Add symbol for raise-chair
    CString new_symbol = "pt_" + basic_symbol_name + "_raischair";
    list.AppendFormat(" %s", new_symbol);
    // Add symbol for headsup-chair...
    new_symbol = "pt_" + basic_symbol_name + "_headsup";
	  list.AppendFormat(" %s", new_symbol);
    // ... and all similar symbols
    new_symbol = "pt_" + basic_symbol_name + "_smallblind";
	  list.AppendFormat(" %s", new_symbol);
    new_symbol = "pt_" + basic_symbol_name + "_bigblind";
	  list.AppendFormat(" %s", new_symbol);
    new_symbol = "pt_" + basic_symbol_name + "_cutoff";
	  list.AppendFormat(" %s", new_symbol);
    new_symbol = "pt_" + basic_symbol_name + "_firstcaller";
	  list.AppendFormat(" %s", new_symbol);
    new_symbol = "pt_" + basic_symbol_name + "_lastcaller";
	  list.AppendFormat(" %s", new_symbol);
    new_symbol = "pt_" + basic_symbol_name + "_firstraiser";
	  list.AppendFormat(" %s", new_symbol);
    new_symbol = "pt_" + basic_symbol_name + "_dealer";
	  list.AppendFormat(" %s", new_symbol);
    new_symbol = "pt_" + basic_symbol_name + "_user";
	  list.AppendFormat(" %s", new_symbol);

    // Add symbols for all chairs, indexed by trailing numbers
    for (int j=0; j<kMaxNumberOfPlayers; j++) {
	    new_symbol.Format("pt_%s%i", basic_symbol_name, j); 
	    list.AppendFormat(" %s", new_symbol);
    }
  }
  return list;
}
