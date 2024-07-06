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

#include <math.h>
#include "string_functions.h"
#include "Debugger/debugger.h"
#include "MagicNumbers/MagicNumbers.h"

const char kUnprintableBeepChar = 0x07;
const char kCharToBeRemoved = kUnprintableBeepChar;

CString Bool2CString(bool b) {
  return (b ? "true" : "false");
}

CString Bool2CString(BOOL b) {
	return (b ? "true" : "false");
}

CString CStringRemoveLeft(CString string, int number_of_characters_to_remove) {
  int length = string.GetLength();
  int number_of_character_to_keep = length - number_of_characters_to_remove;
  return string.Right(number_of_character_to_keep);
}

CString CStringRemoveRight(CString string, int number_of_characters_to_remove) {
  int length = string.GetLength();
  int number_of_character_to_keep = length - number_of_characters_to_remove;
  return string.Left(number_of_character_to_keep);
}

int  DigitCharacterToNumber(char digit) {
  if ((digit >= '0') && (digit <= '9')) {
    return (digit - '0');
  }
  // Actually we want to return kUndefined (-1)
  // but the result gets used as an index without any additional checks,
  // so we return something "fail-safe" for the error-case.
  return kUndefinedZero;
}

CString IntToBinaryString(int number, int min_digits /* = 1 */) {
  CString result;
  assert(min_digits >= 1);
  assert(min_digits <= 32);
  if (number < 0) {
    result = "-";
    number = 0 - number;
  }
  result += "0b";
  CString digits;
  do {
    if ((number % 2) == 0) {
      digits = "0" + digits;
    }
    else {
      digits = "1" + digits;
    }
    number /= 2;
    --min_digits;
  } while ((number > 0) || (min_digits > 0));
  result += digits;
  return result;
};

CString Number2CString(double number, int default_precision /* = 2 */) {
  assert(default_precision >= 0);
  CString result;
  // Avoiding the IsInteger function used by OH,
  // as we don't want to include too much in this DLL.
  // Replacement taken from 
  // http://stackoverflow.com/questions/1521607/check-double-variable-if-it-contains-an-integer-and-not-floating-point
  if (trunc(number) == number) {
    result.Format("%d", int(number));
  } else {
    CString format_string;
    format_string.Format("%%1.%if", default_precision);
    result.Format(format_string, number);
  }
  return result;
}

inline bool IsStandardASCII(char c) {
  return ((c >= 0) && (c <= 0x7F));
}

inline bool ExtendedIsDigit(char c) {
  // Characters from the extended ASCII-set >= 0x80 (non-english currencies)
  // could cause assertions for (signed) chars inside isdigit()
  // http://www.maxinmontreal.com/forums/viewtopic.php?f=110&t=17579&start=90#p122315
  if (!IsStandardASCII(c)) return false;
  return isdigit(c) != 0;
}

// This function removes superfluous charaters from single numbers only.
// Example: "Singlemalt raised to $60"
void KeepBalanceNumbersOnly(CString *s) {
  for (int i = 0; i < s->GetLength(); ++i) {
    char next_character = s->GetAt(i);
    if (ExtendedIsDigit(next_character)) {
      continue;
    }
    if (next_character == '.') {
      continue;
    }
    // Remove (more efficiently: replace) all other characters
    s->SetAt(i, kCharToBeRemoved);
  }
  // Now remove all bad characters at once
  s->Remove(kCharToBeRemoved);
}

void RemoveLeftWhiteSpace(CString *s) {
  s->TrimLeft();
}

void RemoveMultipleWhiteSpaces(CString *s) {
  // Remove all sequences or more white spaces,
  // keep only the first white space.
  int length = s->GetLength();
  for (int i = 0; i < (length - 1); ++i) {
    switch (s->GetAt(i)) {
    case ' ':
    case '\t':
    case '\n':
    case kCharToBeRemoved:
      char next_char = s->GetAt(i + 1);
      switch (next_char) {
      case ' ':
      case '\t':
      case '\n':
        s->SetAt(i + 1, kCharToBeRemoved);
        break;
      }
      break;
    }
  }
  // Finally remove all superfluous characters
  s->Remove(kCharToBeRemoved);
}

void RemoveOHReplayFrameNumber(CString *s) {
  if (s->GetLength() < 8) {
    return;
  }
  if (RightCharacter(*s) != ']') {
    return;
  }
  if (RightCharacter(*s, 7) != '[') {
    return;
  }
  // Title-format: #################[003141]
  // Remove frame-number
  int new_length = s->GetLength() - 8;
  assert(new_length >= 0);
  s->Truncate(new_length);
}

void RemoveRightWhiteSpace(CString *s) {
  s->TrimRight();
}

void RemoveSpacesInsideNumbers(CString *s) {
  int length = s->GetLength();
  // Looking for patters like "7 500"
  // last_index is the last position where the first digit might occur.
  int last_index = length - 4;
  for (int i = 0; i < last_index; ++i) {
    if (isdigit(s->GetAt(i))
      && isspace(s->GetAt(i + 1))
      && isdigit(s->GetAt(i + 2))
      && isdigit(s->GetAt(i + 3))
      && isdigit(s->GetAt(i + 4))
      // Accessing +5 is save;
      // If things go bad it is \0
      && !isdigit(s->GetAt(i + 5))) {
      s->SetAt(i + 1, kCharToBeRemoved);
    }
  }
  // Finally remove all superfluous characters
  s->Remove(kCharToBeRemoved);
}

void ReplaceCommasInNumbersByDots(CString *s) {
  int length = s->GetLength();
  int last_index = length - 1;
  if (length > 0) {
    char first_char = s->GetAt(0);
    if ((first_char == '.') || (first_char == ',')) {
      s->SetAt(0, kCharToBeRemoved);
      s->Remove(kCharToBeRemoved);
    }
  }
  for (int i = 1; i < last_index; ++i) {
    if ((s->GetAt(i) == ',')
      && isdigit(s->GetAt(i - 1))
      && isdigit(s->GetAt(i + 1))) {
      s->SetAt(i, '.');
    }
  }
}

void ReplaceKnownNonASCIICharacters(CString *s) {
  // Some crappy casinos use other characters like "non-breaking-spaces"2
  // (extended Latin-1 character-set) instead of a space
  // http://www.maxinmontreal.com/forums/viewtopic.php?f=110&t=19407&p=141701#p141701
  // http://www.joelonsoftware.com/articles/Unicode.html
  int length = s->GetLength();
  for (int i = 0; i < length; ++i) {
    char current_char = s->GetAt(i);
    if (isascii(current_char)) {
      continue;
    }
    unsigned int char_value = unsigned int(current_char);
    switch (char_value) {
    case 0xFFFFFF80:
      // "Euro" in some unknown extended ASCII-encoding.
      // Replace it by Dollars, as Euro is extended ASCII.
      // "Proprietary Windows" according to https://de.wikipedia.org/wiki/Eurozeichen
      s->SetAt(i, '$');
      break;
    case 0xFFFFFF82:
      // French a with hat
      // http://www.maxinmontreal.com/forums/viewtopic.php?f=217&t=20167&start=150#p160146
      s->SetAt(i, 'a');
      break;
    case 0xFFFFFF88:
      // "Euro" in some unknown extended ASCII-encoding,
      // displayed as "Modifier letter circumflex accent" in latin-1
      // http://www.maxinmontreal.com/forums/viewtopic.php?f=156&t=20167&p=141946#p141916
      // Probably Western Europe encoding, also reported here (Germany):
      // http://www.maxinmontreal.com/forums/viewtopic.php?f=156&t=20167&start=90#p143386
      s->SetAt(i, '$');
      break;
    case 0xFFFFFF92:
      // Yet another encoding of "Euro",
      // probably Western Europe encoding (Germany)
      // http://www.maxinmontreal.com/forums/viewtopic.php?f=156&t=20167&start=90#p143386
      s->SetAt(i, '$');
      break;
    case 0xFFFFFFA0:
      // "Non-breakable space" in extended ASCII Latin-1 encoding
      // http://www.maxinmontreal.com/forums/viewtopic.php?f=156&t=20167&start=60#p143155
      s->SetAt(i, ' ');
      break;
    case 0xFFFFFFA2:
      // Cent symbol
      // http://www.maxinmontreal.com/forums/viewtopic.php?f=156&t=20167&start=60#p143155
      // https://www.atwebresults.com/ascii-codes.php?type=2
      s->SetAt(i, 'c');
      break;
    case 0xFFFFFFA3:
      // British pounds
      // http://www.maxinmontreal.com/forums/viewtopic.php?f=156&t=20167&p=143012#p142990
      s->SetAt(i, '$');
      break;
    case 0xFFFFFFAC:
      // Margematical not, to be replaced by a minus
      // http://www.maxinmontreal.com/forums/viewtopic.php?f=217&t=20167&start=150#p160146
      s->SetAt(i, '-');
      break;
    case 0xFFFFFFB0:
      // French n° with accent
      // http://www.maxinmontreal.com/forums/viewtopic.php?f=217&t=20167&start=150#p160117
      s->SetAt(i, '0');
      break;
    case 0xFFFFFFBD:
      // 1/2 character, replaced with space (Alt 0189)
      // http://www.maxinmontreal.com/forums/viewtopic.php?f=217&t=20167&p=158814#p156798
      s->SetAt(i, ' ');
      break;
    case 0xFFFFFFBF:
      // interrogation opening character, replaced with space (Alt 168)
      // http://www.maxinmontreal.com/forums/viewtopic.php?f=217&t=20167&p=158814#p156798
      s->SetAt(i, ' ');
      break;
    case 0xFFFFFFC1:
      // French A with accent (Alt 0193)
      // http://www.maxinmontreal.com/forums/viewtopic.php?f=217&t=20167&start=150#p156046
      s->SetAt(i, 'A');
      break;
    case 0xFFFFFFC9:
      // French E with accent (Alt 0201)
      // http://www.maxinmontreal.com/forums/viewtopic.php?f=217&t=20167&start=150#p154322
      s->SetAt(i, 'E');
      break;
    case 0xFFFFFFE1:
      // a with accent 	
      // http://www.maxinmontreal.com/forums/viewtopic.php?f=217&t=20167&p=158814#p158545
      s->SetAt(i, 'a');
      break;
    case 0xFFFFFFE2:
      // a with circumflex accent (Alt 0226)
      s->SetAt(i, 'a');
      break;
    case 0xFFFFFFE7:
      // c cedilla (Alt 0231)
      s->SetAt(i, 'c');
      break;
    case 0xFFFFFFE8:
      // e grave (Alt 0232)
      s->SetAt(i, 'e');
      break;
    case 0xFFFFFFEF:
      // i umlaut character or i acute character, (Alt 139 or Alt 161)
      // http://www.maxinmontreal.com/forums/viewtopic.php?f=217&t=20167&p=158814#p156798
      s->SetAt(i, 'i');
      break;
    case 0xFFFFFFF3:
    case 0xFFFFFFE4:
    case 0xFFFFFFE0:
    case 0xFFFFFFEB:
    case 0xFFFFFFE5:
    case 0xFFFFFFED:
    case 0xFFFFFFFB:
    case 0xFFFFFFE9:
      // From Pokerstars with love
      // they put crap at the end of the real title (########)
      // but the visible titlebar in the client-area is clean.
      // http://www.maxinmontreal.com/forums/viewtopic.php?f=156&t=20167&p=142568#p142568
      s->SetAt(i, kCharToBeRemoved);
      // and remove it at the end of the function
      break;
    case 0xFFFFFFF1:
      // Portuguese "n" with accent
      s->SetAt(i, 'n');
      break;
    }
  }
  s->Remove(kCharToBeRemoved);
}

void ReplaceOutlandischCurrencyByDollarsandCents(CString *s) {
  int length = s->GetLength();
  for (int i = 0; i < length; ++i) {
    char current_char = s->GetAt(i);
    switch (current_char) {
    case '€':
    case '£': s->SetAt(i, '$');
      break;
    case '¢': s->SetAt(i, 'c');
      break;
    }
  }
}

void RemoveSpacesInFrontOfCentMultipliers(CString *s) {
  int length = s->GetLength();
  int last_index = length - 2;
  for (int i = 0; i < last_index; ++i) {
    if (isdigit(s->GetAt(i))
      && (isspace(s->GetAt(i + 1)))
      && (s->GetAt(i + 2) == 'c')) {
      s->SetAt(i + 1, kCharToBeRemoved);
    }
  }
  // Finally remove all superfluous characters
  s->Remove(kCharToBeRemoved);
}

void RemoveExtraDotsInNumbers(CString *s) {
  bool inside_number = false;
  bool dot_inside_number_seen = false;
  int digits_seen = 0;
  int length = s->GetLength();
  int last_index = length - 1;
  // Searching backwards for dots in numbers
  for (int i = last_index; i >= 0; --i) {
    char current_char = s->GetAt(i);
    if (isdigit(current_char)) {
      inside_number = true;
      ++digits_seen;
      continue;
    }
    else if (current_char == '.') {
      // If we already saw a dot, e.g. 1.234.5
      // or if we already saw more than 2 digits, e.g. 1.234 
      // then we found an extra dot, separating 1000s or milions.
      // This extra dot has to be removed.
      if ((dot_inside_number_seen) 
        || (digits_seen > 2)) {
        s->SetAt(i, kCharToBeRemoved);
      }
      dot_inside_number_seen = true;
      continue;
    } else {
      inside_number = false;
      dot_inside_number_seen = false;
      digits_seen = 0;
    }
  }
  // Finally remove all superfluous characters
  s->Remove(kCharToBeRemoved);
}

void StringFunctionsTest() {
#ifdef _DEBUG
  CString crappy_title = "  Robostars  Buyin €5,666.777,8     Ante 250 000      Rake 25 ¢     [000017]";
  RemoveOHReplayFrameNumber(&crappy_title);
  assert(crappy_title == "  Robostars  Buyin €5,666.777,8     Ante 250 000      Rake 25 ¢     ");
  ReplaceOutlandischCurrencyByDollarsandCents(&crappy_title);
  assert(crappy_title == "  Robostars  Buyin $5,666.777,8     Ante 250 000      Rake 25 c     ");
  RemoveLeftWhiteSpace(&crappy_title);
  assert(crappy_title == "Robostars  Buyin $5,666.777,8     Ante 250 000      Rake 25 c     ");
  RemoveRightWhiteSpace(&crappy_title);
  assert(crappy_title == "Robostars  Buyin $5,666.777,8     Ante 250 000      Rake 25 c");
  RemoveMultipleWhiteSpaces(&crappy_title);
  assert(crappy_title == "Robostars Buyin $5,666.777,8 Ante 250 000 Rake 25 c");
  RemoveSpacesInsideNumbers(&crappy_title);
  assert(crappy_title == "Robostars Buyin $5,666.777,8 Ante 250000 Rake 25 c");
  RemoveSpacesInFrontOfCentMultipliers(&crappy_title);
  assert(crappy_title == "Robostars Buyin $5,666.777,8 Ante 250000 Rake 25c");
  ReplaceCommasInNumbersByDots(&crappy_title);
  assert(crappy_title == "Robostars Buyin $5.666.777.8 Ante 250000 Rake 25c");
  RemoveExtraDotsInNumbers(&crappy_title);
  assert(crappy_title == "Robostars Buyin $5666777.8 Ante 250000 Rake 25c");
#endif
}

char RightCharacter(CString string, int index_from_right /* = 0 */) {
  assert(index_from_right >= 0);
  int index_from_left = string.GetLength() - index_from_right - 1;
  return string[index_from_left];
}

int RightDigitCharacterToNumber(CString string, int index_from_right /* = 0 */) {
  assert(index_from_right >= 0);
  return DigitCharacterToNumber(RightCharacter(string, index_from_right));
}

bool StringAIsPrefixOfStringB(const char *string_a, const char *string_b) {
  if (strlen(string_a) > strlen(string_b)) {
    return false;
  }
  // Result of memcmp == 0 means: identical
  // See http://www.cplusplus.com/reference/clibrary/cstring/memcmp/
  return (memcmp(string_a, string_b, strlen(string_a)) == 0);
}

bool StringAIsPostfixOfStringB(const char *string_a, const char *string_b) {
  int length_a = strlen(string_a);
  int length_b = strlen(string_b);
  if (length_a > length_b) {
    return false;
  }
  int start_position = length_b - length_a;
  if (memcmp(string_a, (string_b + start_position), length_a) == 0) {
    return true;
  }
  return false;
}

bool StringIsExactMatch(const char *string_a, const char *string_b) {
  return ((strlen(string_a) == strlen(string_b))
    && StringAIsPrefixOfStringB(string_a, string_b));
}

double StringToMoney(const CString inStr) {
  CStringArray		 possibleValues;
  CArray<int, int> possibleValuesMultiplier;
  CString				   activeValue = "";
  int					     iValueWithCurrencySymbol = -1;
  bool				     currencySymbol = false;
#ifdef OPENHOLDEM_PROGRAM
  //!!!write_log(preferences.debug_scraper(),
    ///"[CTransform] StringToMoney %s\n", inStr);
#endif 
  if (strlen(inStr) == 0) {
    return kUndefinedZero;
  }
  if (inStr.FindOneOf("0123456789") < 0) {
    return kUndefinedZero;
  }
  const char *str = inStr.GetString();
  while (*str) {
    switch (*str) {
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
    case '.':
      activeValue += *str;
      break;
    case ',':
      break;
    case '$':
    case '€':
    case '£':
      if (activeValue.GetLength() > 0) {
        possibleValues.Add(activeValue);
        possibleValuesMultiplier.Add(1);
        activeValue.Empty();
      }
      currencySymbol = true;
      break;
    default:
      if (activeValue.GetLength() > 0) {
        int index = (int)possibleValues.Add(activeValue);
        if (currencySymbol) {
          iValueWithCurrencySymbol = index;
        }
        // Support for multiplier-symbols in bets and balances
        // k = $1.000
        // m = $1.000.000
        // c = $0.01
        // Both upper and lower cases
        // http://www.maxinmontreal.com/forums/viewtopic.php?f=117&t=18939
        if (*str == '¢' || *str == 'c' || *str == 'C') {
          possibleValuesMultiplier.Add(-100);
        }
        else if (*str == 'k' || *str == 'K') {
          possibleValuesMultiplier.Add(1000);
        }
        else if (*str == 'm' || *str == 'M') {
          possibleValuesMultiplier.Add(1000000);
        }
        else {
          possibleValuesMultiplier.Add(1);
        }
        activeValue.Empty();
      }
      break;
    }
    ++str;
  }
  if (activeValue.GetLength() > 0) {
    int index = (int)possibleValues.Add(activeValue);
    possibleValuesMultiplier.Add(1);
    if (currencySymbol) {
      iValueWithCurrencySymbol = index;
    }
  }
  double number = 0.0;
  int iValueToUse = -1;
  if (possibleValues.GetSize() == 1) {
    iValueToUse = 0;
  }
  else if (iValueWithCurrencySymbol >= 0) {
    iValueToUse = iValueWithCurrencySymbol;
  }
  else if (possibleValues.GetSize() > 1) {
    iValueToUse = 0;
  }
  if (iValueToUse >= 0) {
    number = atof(possibleValues[iValueToUse].GetString());
    if (possibleValuesMultiplier[iValueToUse] < 0) {
      number /= -possibleValuesMultiplier[iValueToUse];
    }
    else {
      number *= possibleValuesMultiplier[iValueToUse];
    }
  }
  return number;
}

void WarnAboutNonASCIICharacters(const CString *s) {
  // Some crappy casinos use other characters like "non-breaking-spaces"2
  // (extended Latin-1 character-set) instead of a space
  // Warn about them if they didn't get already replaced.
  // http://www.maxinmontreal.com/forums/viewtopic.php?f=110&t=19407&p=141701#p141701
  // http://www.joelonsoftware.com/articles/Unicode.html
  int length = s->GetLength();
  for (int i = 0; i < length; ++i) {
    char current_char = s->GetAt(i);
    if (!isascii(current_char)) {
      unsigned int char_value = unsigned int(current_char);
      int signed_char_value = int(current_char);
      CString message;
      message.Format("Unexpected character inside title or number\n"
        "Probably extended ASCII or Unicode\n"
        "Numerical value: %x, %d\n"
        "Title: %s\n"
        "Please report to the developers\n"
        "http://www.maxinmontreal.com/forums/viewtopic.php?f=156&t=20167",
        char_value, signed_char_value, s);
      ///!!!write_log(k_always_log_errors, message);
    }
  }
}

// For the symbol-engines to build a ctring with a list of symbols
// like currentbet0..currentbet9, separates by spaces
CString RangeOfSymbols(CString format_string, int first, int last) {
  CString result;
  CString next_symbol;
  assert(last >= first);
  for (int i = first; i <= last; ++i) {
    next_symbol.Format(format_string, i);
    result += next_symbol;
    result += " ";
  }
  return result;
}