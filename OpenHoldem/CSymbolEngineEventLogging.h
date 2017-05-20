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

#ifndef INC_CSYMBOLENGINEEVENTLOGGING_H
#define INC_CSYMBOLENGINEEVENTLOGGING_H

#include "CVirtualSymbolEngine.h"

class CSymbolEngineEventLogging: public CVirtualSymbolEngine
{
public:
	CSymbolEngineEventLogging();
	~CSymbolEngineEventLogging();
public:
	// Mandatory reset-functions
	void InitOnStartup();
	void UpdateOnConnection();
	void UpdateOnHandreset();
	void UpdateOnNewRound();
	void UpdateOnMyTurn();
	void UpdateOnHeartbeat();
public:
	// Public accessors
	bool EvaluateSymbol(const CString name, double *result, bool log = false);
};

extern CSymbolEngineEventLogging *p_symbol_engine_event_logging;

#endif INC_CSYMBOLENGINEEVENTLOGGING_H