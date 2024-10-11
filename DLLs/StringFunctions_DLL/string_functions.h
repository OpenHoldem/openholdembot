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

#ifndef INC_STRING_FUNCTIONS_H
#define INC_STRING_FUNCTIONS_H

#ifdef STRING_FUNCTIONS_EXPORTS
#define STRING_FUNCTIONS_API __declspec(dllexport)
#else
#define STRING_FUNCTIONS_API __declspec(dllimport)
#endif

#include <atlstr.h>

STRING_FUNCTIONS_API CString Bool2CString(bool b);
STRING_FUNCTIONS_API CString CStringRemoveLeft(CString string, int number_of_characters_to_remove);
STRING_FUNCTIONS_API CString CStringRemoveRight(CString string, int number_of_characters_to_remove);
STRING_FUNCTIONS_API int     DigitCharacterToNumber(char digit);
STRING_FUNCTIONS_API CString IntToBinaryString(int number, int min_digits = 1);
STRING_FUNCTIONS_API void KeepBalanceNumbersOnly(CString *s);
STRING_FUNCTIONS_API CString Number2CString(double number, int default_precision = 2);
// For the symbol-engines to build a ctring with a list of symbols
// like currentbet0..currentbet9, separated by spaces
STRING_FUNCTIONS_API CString RangeOfSymbols(CString format_string, int first, int last);
STRING_FUNCTIONS_API void RemoveExtraDotsInNumbers(CString *s);
STRING_FUNCTIONS_API void RemoveHtmlTagsAndContexts(CString* s);
STRING_FUNCTIONS_API void RemoveLeftWhiteSpace(CString *s);
STRING_FUNCTIONS_API void RemoveMultipleWhiteSpaces(CString *s);
STRING_FUNCTIONS_API void RemoveOHReplayFrameNumber(CString *s);
STRING_FUNCTIONS_API void RemoveRightWhiteSpace(CString *s);
STRING_FUNCTIONS_API void RemoveSpacesInFrontOfCentMultipliers(CString *s);
STRING_FUNCTIONS_API void RemoveSpacesInsideNumbers(CString *s);
STRING_FUNCTIONS_API void ReplaceCommasInNumbersByDots(CString *s);
STRING_FUNCTIONS_API void ReplaceKnownNonASCIICharacters(CString *s);
STRING_FUNCTIONS_API void ReplaceOutlandischCurrencyByDollarsandCents(CString *s);
STRING_FUNCTIONS_API char RightCharacter(CString string, int index_from_right = 0);
STRING_FUNCTIONS_API int  RightDigitCharacterToNumber(CString string, int index_from_right = 0);
STRING_FUNCTIONS_API bool StringAIsPrefixOfStringB(const char *string_a, const char *string_b);
STRING_FUNCTIONS_API bool StringAIsPostfixOfStringB(const char *string_a, const char *string_b);
STRING_FUNCTIONS_API void StringFunctionsTest();
STRING_FUNCTIONS_API double StringToMoney(const CString inStr);
STRING_FUNCTIONS_API bool StringIsExactMatch(const char *string_a, const char *string_b);
STRING_FUNCTIONS_API void WarnAboutNonASCIICharacters(const CString *s);

#endif // INC_STRING_FUNCTIONS_H 