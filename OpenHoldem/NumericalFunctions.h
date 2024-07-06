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

#ifndef INC_NUMERICALFUNCTIONS_H
#define INC_NUMERICALFUNCTIONS_H

// Deals with floating points, ints, hex and binary
double StringToNumber(CString number);

// Turns an integer into a hex-char 0..9, A..F
// Works up to 35: 0..9, A..Z
// Used for indexing more than 10 autoplayer buttons
char HexadecimalChar(int i);

inline void SwapDoubles(double *first, double *second) {
  double temp = *first;
  *first = *second;
  *second = temp;
}

inline void SwapInts(int *first, int *second){
  int temp = *first;
  *first = *second;
  *second = temp;
}

inline bool IsInteger(double k) { 
  return (double(int(k)) == k); 
}

inline int RankOfCard(int card) {
  // Suit is stored in bits 0..3
  // Rank is stored in bits 4..7
  return ((card >>4) & 0x0F);
}

// Making this a macro, because we want source-code information
// about the call-site if an assertion fires, and not
// "numeri...tions.h  Line 45"
#define AssertRange(/*double*/ value, /*double*/ lower_bound, /*double*/ upper_bound) \
    { assert((value) >= (lower_bound)); assert((value) <= (upper_bound)); }

#define RETURN_DEFAULT_IF_OUT_OF_RANGE(value, upper_bound, _default)	if (((value) < 0) || ((value) > (upper_bound))) return (_default);
#define RETURN_IF_OUT_OF_RANGE(value, upper_bound)						if (((value) < 0) || ((value) > (upper_bound))) return;

inline void SetBit(int *bit_vector, int n_th_bit) {
  *bit_vector |= (1 << n_th_bit);
}

inline bool IsBitSet(int bit_vector, int n_th_bit) {
  return ((bit_vector >> n_th_bit) & 1);
}

int	bitcount(unsigned bit_vector);

inline void AdaptValueToMinMaxRange(double *value, double min, double max) {
  if (*value < min) {
    *value = min;
  } else if (*value > max) {
    *value = max;
  }
}

// returns a random number in the range 0..n-1
int Random(int n);

// Rounds value to N units
// Default unit is 1.0 (integer)
double Rounding(double value, double unit = 1.0);

#define MAX(x, y)		(((x) > (y)) ? (x) : (y))
#define MIN(x, y)		(((x) < (y)) ? (x) : (y))

#endif INC_NUMERICALFUNCTIONS_H