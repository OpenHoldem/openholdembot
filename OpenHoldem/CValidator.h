//******************************************************************************
//
// This file is part of the OpenHoldem project
//   Download page:         http://code.google.com/p/openholdembot/
//   Forums:                http://www.maxinmontreal.com/forums/index.php
//   Licensed under GPL v3: http://www.gnu.org/licenses/gpl.html
//
//******************************************************************************
//
// Purpose:
//
//******************************************************************************

#ifndef INC_VALIDATOR_H
#define INC_VALIDATOR_H

#include "NumericalFunctions.h"
																																												
class CValidator
{
public:
	// public functions
	CValidator();
	~CValidator();
	void ValidateIt();
	void ValidateGameState();
	void SetEnabledManually(bool Enabled);

private:
	// private functions
	void ValidateSingleRule();
	void ValidateVersusDBOnlyIfInstalled();
  void ValidateICMOnlyIfTournament();
	CString Symbols_And_Values(const CString symbols_possibly_affected);
	double gws(const char *the_Symbol);
  bool InExcludedTests(CString testcase_id);

private:
	// private data members, not (directly) accessible
	char	*_testcase_id;
	bool	_heuristic;
	char    *_reasoning;
	bool	_precondition;
	bool	_postcondition;
	char	*_symbols_possibly_affected;
private:
	bool _enabled_manually;
	bool _no_errors_this_heartbeat;
};

extern CValidator *p_validator; 

// Extern vars (inter-module)
extern bool vali_ok1;
extern bool vali_ok2;
extern bool vali_err;

#endif

																																																						