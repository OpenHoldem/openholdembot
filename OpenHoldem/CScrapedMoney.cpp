//******************************************************************************
//
// This file is part of the OpenHoldem project
//    Source code:           https://github.com/OpenHoldem/openholdembot/
//    Forums:                http://www.maxinmontreal.com/forums/index.php
//    Licensed under GPL v3: http://www.gnu.org/licenses/gpl.html
//
//******************************************************************************
//
// Purpose: class for bets, balances and pots,
//  which also handles proprocessing of special number formats, ...
//    http://www.maxinmontreal.com/forums/viewtopic.php?f=117&t=20164
//    http://www.maxinmontreal.com/forums/viewtopic.php?f=156&t=19216
//    http://www.maxinmontreal.com/forums/viewtopic.php?f=156&t=19658
//    http://www.maxinmontreal.com/forums/viewtopic.php?f=156&t=20096
//    http://www.maxinmontreal.com/forums/viewtopic.php?f=156&t=20085
//
//******************************************************************************

#include "stdafx.h"
#include "CScrapedMoney.h"
#include "..\DLLs\StringFunctions_DLL\string_functions.h"

CScrapedMoney::CScrapedMoney() {
	Reset();
}

CScrapedMoney::~CScrapedMoney() {
}

bool CScrapedMoney::SetValue(CString scraped_value) {
	if (scraped_value == "") {
		return false;
	}
	ReplaceKnownNonASCIICharacters(&scraped_value);
	WarnAboutNonASCIICharacters(&scraped_value);
	RemoveLeftWhiteSpace(&scraped_value);
	RemoveRightWhiteSpace(&scraped_value);
	RemoveMultipleWhiteSpaces(&scraped_value);
	RemoveSpacesInsideNumbers(&scraped_value);
	ReplaceOutlandischCurrencyByDollarsandCents(&scraped_value);
	RemoveSpacesInFrontOfCentMultipliers(&scraped_value);
	ReplaceCommasInNumbersByDots(&scraped_value);
	RemoveExtraDotsInNumbers(&scraped_value);
	//!!!KeepBalanceNumbersOnly(&scraped_value);
	if (scraped_value == "") {
		// Empty data (e.g. in the c0sblind) must not evaluated
		// otherwise we might overwrite known good data (e.g. from ttlimits)
		return false;
	}
	// Evaluate unauthorized chars: parenthesis and other brackets
	// Mainly for DDPoker
	if (scraped_value.Find("(") != -1 && scraped_value.Find(")") != -1) {
		return false;
	}
	if (scraped_value.Find("{") != -1 && scraped_value.Find("}") != -1) {
		return false;
	}
	if (scraped_value.Find("[") != -1 && scraped_value.Find("]") != -1) {
		return false;
	}
	if (scraped_value.Find("<") != -1 && scraped_value.Find(">") != -1) {
		return false;
	}
	if (!(scraped_value.Find("0123456789"))) {
		// Again: evaluate only meaningful input
		return false;
	}
	double result = StringToMoney(scraped_value);
	if (result >= 0.0) {
		return SetValue(result);
	}
	return false;
}

bool CScrapedMoney::SetValue(double new_value) {
	_value = new_value;
	return true;
}

void CScrapedMoney::Reset() {
	SetValue(0.0);
}