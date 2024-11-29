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
#include "CSymbolEngineFpdb.h"
#include "CSymbolEngineActiveDealtPlaying.h"

#include "CEngineContainer.h"
#include "CFormulaParser.h"
#include "CParseErrors.h"
#include "CParseTreeTerminalNodeEndOfFunction.h"
#include "SQLiteWrapper.h"
#include "..\Fpdb_Query_Definitions\fpdb_query_definitions.h"
#include "CFpdbThread.h"

#include "CSymbolEngineCallers.h"
#include "CSymbolEngineChairs.h"
#include "CSymbolengineDealerchair.h"
#include "CSymbolEngineIsRush.h"
#include "CSymbolEngineRaisers.h"
#include "CSymbolEngineUserchair.h"
#include "..\DLLs\WindowFunctions_DLL\window_functions.h"
#include "..\DLLs\StringFunctions_DLL\string_functions.h"

SQLiteDB *pSQLiteDB;

CSymbolEngineFpdb::CSymbolEngineFpdb()
{
	// The values of some symbol-engines depend on other engines.
	// As the engines get later called in the order of initialization
	// we assure correct ordering by checking if they are initialized.
	assert(p_engine_container->symbol_engine_isrush() != NULL); 
	assert(p_engine_container->symbol_engine_raisers() != NULL);
	assert(p_engine_container->symbol_engine_userchair() != NULL);
	assert(p_engine_container->symbol_engine_active_dealt_playing() != NULL);
}

CSymbolEngineFpdb::~CSymbolEngineFpdb() {
}

void CSymbolEngineFpdb::InitOnStartup() {
	check_for_identity_of_players_executed_this_heartbeat = false;
	ClearAllStats();
}

void CSymbolEngineFpdb::UpdateOnConnection() {
	ClearAllStats();
	if (!p_fpdb_thread->_connected) {
		p_fpdb_thread->Connect();
	}
	else if (pSQLiteDB->GetDbStatus() != SQLITE_OK) {
		p_fpdb_thread->Reconnect();
  }
//	p_fpdb_thread->StartThread();	// Now we stop getting stats every hearthbeat as before but only on demand with our function pointer fpUpdateStat from PT DLL
									// as it is very resource intensive and keeping both update stats on demand and on every heartbeat can cause OH freeze or a memory heap corruption				 
}

void CSymbolEngineFpdb::UpdateOnHandreset() {
  if (p_engine_container->symbol_engine_isrush()->isrush())
    ClearAllStats();
}

void CSymbolEngineFpdb::UpdateOnNewRound()
{}

void CSymbolEngineFpdb::UpdateOnMyTurn()
{}

void CSymbolEngineFpdb::UpdateOnHeartbeat()
{
	check_for_identity_of_players_executed_this_heartbeat = false;
}

void CSymbolEngineFpdb::WarnAboutInvalidFPDBSymbol(CString s) {
	CString error_message;
  error_message.Format("Invalid Fpdb-symbol: %s\n", s);
	CParseErrors::MessageBox_Formula_Error(error_message, "Error");
}

bool CSymbolEngineFpdb::IsOldStyleFPDBSymbol(CString s) {
  if (s.Left(4) == "fpdb") {
    return false;
  }
  if (s.Left(5) != "fpdb_") {
    return true;
  } 
  if (s.Left(7) == "fpdb_r_") {
    return true;
  }
  return false;
}

void CSymbolEngineFpdb::CheckForChangedPlayersOncePerHeartbeatAndSymbolLookup() {
	if (check_for_identity_of_players_executed_this_heartbeat)
	{
		return;
	}
	check_for_identity_of_players_executed_this_heartbeat = true;
	ClearAllStatsOfChangedPlayers();
}

void CSymbolEngineFpdb::ClearSeatStats(int chair, bool clearNameAndFound) {
	assert(chair >= kFirstChair); 
	assert(chair <= kLastChair);
	write_log(Preferences()->debug_fpdb(), "[CSymbolEngineFpdb] ClearSeatStats() for chair %i\n", chair);
	FPDB_DLL_ClearPlayerStats(chair);
	if (clearNameAndFound) {
		_fpdb_player_data[chair].found = false;
		memset(_fpdb_player_data[chair].fpdb_name, 0, kMaxLengthOfPlayername);
		memset(_fpdb_player_data[chair].scraped_name, 0, kMaxLengthOfPlayername);
	}
	_fpdb_player_data[chair].skipped_updates = k_fpdb_advanced_stat_update_every;
}

void CSymbolEngineFpdb::ClearAllStatsOfChangedPlayers() {
	write_log(Preferences()->debug_fpdb(), "[CSymbolEngineFpdb] Executing ClearAllStatsOfChangedPlayers()\n");
	for (int i=0; i<kMaxNumberOfPlayers; i++)
	{
		if (p_fpdb_thread->CheckIfNameHasChanged(i))
		{
			ClearSeatStats(i, true);
		}
	}
}

void CSymbolEngineFpdb::ClearAllStats()
{
	for (int i=0; i<kMaxNumberOfPlayers; i++)
	{
		ClearSeatStats(i, true);
	}
}

int CSymbolEngineFpdb::PlayerIcon(const int chair) {
  assert(chair >= 0);
  assert(chair <= kLastChair);
  return FPDB_DLL_GetStat("icon", chair);
}

bool CSymbolEngineFpdb::EvaluateSymbol(const CString name, double *result, bool log /* = false */) {
  FAST_EXIT_ON_OPENPPL_SYMBOLS(name);
	if (memcmp(name, "fpdb_", 5) != 0) {
		// Symbol of a different symbol-engine
		return false;
	}
	CheckForChangedPlayersOncePerHeartbeatAndSymbolLookup();
	if (IsOldStyleFPDBSymbol(name))	{
		CString error_message;
		error_message.Format(
			"Old style Fpdb symbol detected: %s.\n"
			"\n"
			"Fpdb symbol start with \"fpdb_\".\n"
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
			"ERROR: Invalid Fpdb Symbol");
		*result = kUndefined;
		return true;
	}
	if (!FPDB_DLL_IsValidSymbol(CString(name)))	{
		// Invalid Fpdb symbol
		WarnAboutInvalidFPDBSymbol(name);
		*result = kUndefined;
		return true;
	}
	int chair = 0;
	if (!p_fpdb_thread->IsConnected()) 	{
		if (!p_engine_container->symbol_engine_userchair()->userchair_confirmed() || p_formula_parser->IsParsing()) {
			// We are not yet seated or formula is getting parsed.
			// Symbol-lookup happens, because of Formula-validation.
			// Not a problem, if we do not yet have a DB-connection.
			// Don't throw a warning here.
      write_log(Preferences()->debug_fpdb(), "[Fpdb] Not yet seated or formula parsing.\n");
		} else {
			// We are seated and playing, use a FPDB-symbol,
      // but are noit connected to a database
      if (CParseTreeTerminalNodeEndOfFunction::evaluating_defailt_logic()) {
        // Not a problem
        // The default-bot uses FPDB-symbols,
        // but also plays well against unknown.
        // Lots of people will use it without a DB
      } else {
        // Serious problem: FPDB-usage in user-code
        MessageBox_Error_Warning("Not connected to Fpdb database.\n"
          "Can't use Fpdb symbols.");
      }
		}
		*result = kUndefined;
		return true;
	}	else {
		CString last_character = name.Right(1);
    if (!isdigit(last_character[0])) {
      CString error_message;
      error_message.Format("Invalid Fpdb Symbol: %s",
        name);
      CParseErrors::MessageBox_Formula_Error(error_message, "ERROR");
		  *result = kUndefined;
      return false;
    }
		chair = atoi(last_character);
	}


  // Catch undefined chair (e.g. fpdb_r_-symbol without raisee)
  if (chair < 0) {
    *result = kUndefined;
    return true;
  }
	AssertRange(chair, kFirstChair, kLastChair);
	*result = FPDB_DLL_GetStat(name, chair); 
	return true;
}

CString CSymbolEngineFpdb::SymbolsProvided() {
  CString list;
  for (int i=0; i<FPDB_DLL_GetNumberOfStats(); ++i) {
    CString basic_symbol_name = FPDB_DLL_GetBasicSymbolNameWithoutFPDBPrefix(i);
    // Add symbol for raise-chair
    CString new_symbol = "fpdb_" + basic_symbol_name + "_raischair";
    list.AppendFormat(" %s", new_symbol);
    // Add symbol for headsup-chair...
    new_symbol = "fpdb_" + basic_symbol_name + "_headsup";
	  list.AppendFormat(" %s", new_symbol);
    // ... and all similar symbols
    new_symbol = "fpdb_" + basic_symbol_name + "_smallblind";
	  list.AppendFormat(" %s", new_symbol);
    new_symbol = "fpdb_" + basic_symbol_name + "_bigblind";
	  list.AppendFormat(" %s", new_symbol);
    new_symbol = "fpdb_" + basic_symbol_name + "_cutoff";
	  list.AppendFormat(" %s", new_symbol);
    new_symbol = "fpdb_" + basic_symbol_name + "_firstcaller";
	  list.AppendFormat(" %s", new_symbol);
    new_symbol = "fpdb_" + basic_symbol_name + "_lastcaller";
	  list.AppendFormat(" %s", new_symbol);
    new_symbol = "fpdb_" + basic_symbol_name + "_firstraiser";
	  list.AppendFormat(" %s", new_symbol);
    new_symbol = "fpdb_" + basic_symbol_name + "_dealer";
	  list.AppendFormat(" %s", new_symbol);
    new_symbol = "fpdb_" + basic_symbol_name + "_user";
	  list.AppendFormat(" %s", new_symbol);

    // Add symbols for all chairs, indexed by trailing numbers
    for (int j=0; j<kMaxNumberOfPlayers; j++) {
	    new_symbol.Format("fpdb_%s%i", basic_symbol_name, j); 
	    list.AppendFormat(" %s", new_symbol);
    }
  }
  return list;
}
