//******************************************************************************
//
// This file is part of the OpenHoldem project
//    Source code:           https://github.com/OpenHoldem/openholdembot/
//    Forums:                http://www.maxinmontreal.com/forums/index.php
//    Licensed under GPL v3: http://www.gnu.org/licenses/gpl.html
//
//******************************************************************************
//
// Purpose: String functions library
//
//******************************************************************************

#pragma once
#ifndef WINVER				// Allow use of features specific to Windows XP or later.
#define WINVER 0x0501
#endif

#include <afxwin.h>

CString Bool2CString(bool b);
CString Bool2CString(BOOL b);
CString CStringRemoveLeft(CString string, int number_of_characters_to_remove);
CString CStringRemoveRight(CString string, int number_of_characters_to_remove);
int     DigitCharacterToNumber(char digit);
CString IntToBinaryString(int number, int min_digits = 1);
void KeepBalanceNumbersOnly(CString *s);
CString Number2CString(double number, int default_precision = 2);
// For the symbol-engines to build a ctring with a list of symbols
// like currentbet0..currentbet9, separated by spaces
CString RangeOfSymbols(CString format_string, int first, int last);
void RemoveExtraDotsInNumbers(CString *s);
void RemoveLeftWhiteSpace(CString *s);
void RemoveMultipleWhiteSpaces(CString *s);
void RemoveOHReplayFrameNumber(CString *s);
void RemoveRightWhiteSpace(CString *s);
void RemoveSpacesInFrontOfCentMultipliers(CString *s);
void RemoveSpacesInsideNumbers(CString *s);
void ReplaceCommasInNumbersByDots(CString *s);
void ReplaceKnownNonASCIICharacters(CString *s);
void ReplaceOutlandischCurrencyByDollarsandCents(CString *s);
char RightCharacter(CString string, int index_from_right = 0);
int  RightDigitCharacterToNumber(CString string, int index_from_right = 0);
bool StringAIsPrefixOfStringB(const char *string_a, const char *string_b);
bool StringAIsPostfixOfStringB(const char *string_a, const char *string_b);
void StringFunctionsTest();
double StringToMoney(const CString inStr);
bool StringIsExactMatch(const char *string_a, const char *string_b);
void WarnAboutNonASCIICharacters(const CString *s);
