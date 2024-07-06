//*******************************************************************************
//
// This file is part of the OpenHoldem project
//    Source code:           https://github.com/OpenHoldem/openholdembot/
//    Forums:                http://www.maxinmontreal.com/forums/index.php
//    Licensed under GPL v3: http://www.gnu.org/licenses/gpl.html
//
//*******************************************************************************
//
// Purpose:
//
//*******************************************************************************

#include "stdafx.h"
#include "Debugger/debugger.h"

// Deals with floating points, ints, hex and binary
double StringToNumber(CString number) {
  // http://www.cplusplus.com/reference/cstdlib/strtol/
  // http://stackoverflow.com/questions/10156409/convert-hex-string-char-to-int
  // http://stackoverflow.com/questions/2482211/c-converting-binary-to-decimal
  assert(number[0] != NULL);
  if (memcmp(number, "0x", 2) == 0) {
    // Hex number
    // Prefix 0x is allowed
    return strtol(number, NULL, 16);
  } else if (memcmp(number, "0b", 2) == 0) {
    // Binary number
    // Prefix 0b is not allowed: we have to remove it
    return strtol(number.Mid(2), NULL, 2);
  } else {
    // Decimal number, either int or floating point
    return atof(number);
  }
}

char HexadecimalChar(int i) {
  assert(i >= 0);
  assert(i <= 35);
  if (i < 10) {
    return ('0' + i);
  }
  else if (i <= 35) {
    return ('A' + i - 10);
  }
  else {
    return '#';
  }
}

#define TWO(c) (0x1u << (c))
#define MASK(c) (((unsigned int)(-1)) / (TWO(TWO(c)) + 1u))
#define COUNT(x,c) ((x) & MASK(c)) + (((x) >> (TWO(c))) & MASK(c))

int bitcount (unsigned int n) {
  n = COUNT(n, 0) ;
  n = COUNT(n, 1) ;
  n = COUNT(n, 2) ;
  n = COUNT(n, 3) ;
  n = COUNT(n, 4) ;
  /* n = COUNT(n, 5) ;    for 64-bit integers */
  return n;
}

int Random(int n) {
	// rang 0..randmax
	int r = rand();
	int result = double(n) * (double(r)/RAND_MAX);
	return result;
}

// Rounds value to N units
// Default unit is 1.0 (integer)
double Rounding(double value, double unit /* = 1.0 */) {
  assert(value >= 0.0);
  assert(unit >= 0.0);
  int n_units = (value + 0.5 * unit) / unit;
  assert(n_units >= 0);
  double rounded_value = n_units * unit;
  return rounded_value;
}
