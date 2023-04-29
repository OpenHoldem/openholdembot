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

#ifndef INC_CFPDBLOOKUP_H
#define INC_CFPDBLOOKUP_H

#include <map>

class CFpdbLookup
{
public:
	// public functions
	CFpdbLookup();
	~CFpdbLookup();
	const int GetSiteId();

private:
	// private functions and variables - not available via accessors or mutators
	std::map<CString, int>	_fpdb_siteid;
};

extern CFpdbLookup fpdb_lookup;

#endif INC_CFPDBLOOKUP_H