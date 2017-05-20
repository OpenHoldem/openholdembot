//******************************************************************************
//
// This file is part of the OpenHoldem project
//    Source code:           https://github.com/OpenHoldem/openholdembot/
//    Forums:                http://www.maxinmontreal.com/forums/index.php
//    Licensed under GPL v3: http://www.gnu.org/licenses/gpl.html
//
//******************************************************************************
//
// Purpose: Not really a symbol-engine, 
//   but a good way to get important events logged.
//
//******************************************************************************

#include "stdafx.h"
#include "CSymbolEngineEventLogging.h"

#include "CAutoconnector.h"
#include "CAutoplayerTrace.h"
#include "CHandresetDetector.h"
#include "CScraper.h"
#include "CSymbolEngineDealerchair.h"
#include "CSymbolEngineUserchair.h"
#include "CTableState.h"
#include "CTableTitle.h"
#include "CVersionInfo.h"

CSymbolEngineEventLogging *p_symbol_engine_event_logging = NULL;

CSymbolEngineEventLogging::CSymbolEngineEventLogging()
{}

CSymbolEngineEventLogging::~CSymbolEngineEventLogging()
{}

void CSymbolEngineEventLogging::InitOnStartup()
{}

void CSymbolEngineEventLogging::UpdateOnConnection() {
  p_autoplayer_trace->Clear();
}

void CSymbolEngineEventLogging::UpdateOnHandreset() {
  // Log a new connection, plus the version-info
  // (because of all the guys who report "bugs" of outdated versions)
  write_log(k_always_log_basic_information,
    "\n"
    "==============================================\n"
    "%s"    // Version info already contains a newline
    "Table: %s\n"
    "==============================================\n"
    "HAND RESET\n"
    "==============================================\n",
    p_version_info->GetVersionInfo(), 
    p_table_title->Title());
}

void CSymbolEngineEventLogging::UpdateOnNewRound()
{}

void CSymbolEngineEventLogging::UpdateOnMyTurn()
{}

void CSymbolEngineEventLogging::UpdateOnHeartbeat() {
  p_autoplayer_trace->Clear();
}

bool CSymbolEngineEventLogging::EvaluateSymbol(const CString name, double *result, bool log /* = false */) {
	// We don't provide any symbols
	return false;
}
