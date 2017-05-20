	//***************************************************************************** 
//
// This file is part of the OpenHoldem project
//    Source code:           https://github.com/OpenHoldem/openholdembot/
//    Forums:                http://www.maxinmontreal.com/forums/index.php
//    Licensed under GPL v3: http://www.gnu.org/licenses/gpl.html
//
//***************************************************************************** 
//
// Purpose: This symbol-engine does not provide any symbols,
//   but triggers only the calculation of history/symbols, etc.
//   All OpenPPL-symbols are functions that get handled by the function-collection.
//
//***************************************************************************** 

#ifndef INC_CSYMBOLENGINE_OPENPPL_HANDSTRENGTH_H
#define INC_CSYMBOLENGINE_OPENPPL_HANDSTRENGTH_H

#include "CVirtualSymbolEngine.h"

class CSymbolEngineOpenPPL: public CVirtualSymbolEngine
{
 public:
	CSymbolEngineOpenPPL();
	~CSymbolEngineOpenPPL();
 public:
	// Mandatory reset-functions
	void InitOnStartup();
	void UpdateOnConnection();
	void UpdateOnHandreset();
	void UpdateOnNewRound();
	void UpdateOnMyTurn();
	void UpdateOnHeartbeat();
  void UpdateAfterAutoplayerAction(int autoplayer_action_code);
 public:
	// Public accessors
	bool EvaluateSymbol(const CString name, double *result, bool log = false);
	CString IdentifiersProvided();;
 public:
  void VerifyExistenceOfOpenPPLInitializationInLibrary();
 private:
  void CheckExistenceOfOpenPPLFunction(CString function);
};

extern CSymbolEngineOpenPPL *p_symbol_engine_open_ppl;

#endif INC_CSYMBOLENGINE_OPENPPL_HANDSTRENGTH_H