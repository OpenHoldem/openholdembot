//******************************************************************************
//
// This file is part of the OpenHoldem project
//    Source code:           https://github.com/OpenHoldem/openholdembot/
//    Forums:                http://www.maxinmontreal.com/forums/index.php
//    Licensed under GPL v3: http://www.gnu.org/licenses/gpl.html
//
//******************************************************************************
//
// Purpose: Part of the modular handhistory-generator.
//   Not really a symbol-engine but it is convenient
//   to make use of the symbol-engine-concepts,
//   Also used for the new summary in the logs.
//
//******************************************************************************

#ifndef INC_CHANDHISTORYSHOWDOWN_H
#define INC_CHANDHISTORYSHOWDOWN_H

#include "CVirtualSymbolEngine.h"

class CHandHistoryShowdown: public CVirtualSymbolEngine {
 public:
	CHandHistoryShowdown();
	~CHandHistoryShowdown();
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
  CString SymbolsProvided();
 private:
   bool _job_done;
   bool _river_seen;
};

extern CHandHistoryShowdown *p_handhistory_showdown;

#endif INC_CHANDHISTORYSHOWDOWN_H