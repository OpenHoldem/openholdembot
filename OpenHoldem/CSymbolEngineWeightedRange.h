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

#include "CVirtualSymbolEngine.h"

class CSymbolEngineWeightedRange : public CVirtualSymbolEngine {
 public:
	CSymbolEngineWeightedRange();
	~CSymbolEngineWeightedRange();
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
	bool EvaluateSymbol(const CString name, CString *result, bool log = false);
	CString SymbolsProvided();
 private:
	 void AssignWeightedRanges();
	 CString wrange(const int chair) {
		 assert((chair >= 0) || (chair == kUndefined));
		 assert(chair < kMaxNumberOfPlayers);
		 if (chair == kUndefined) {
			 return "";
		 }
		 return _wrange[chair];
	 }
 private:
	 CString _wrange[kMaxNumberOfPlayers];
};