//******************************************************************************
//
// This file is part of the OpenHoldem project
//    Source code:           https://github.com/OpenHoldem/openholdembot/
//    Forums:                http://www.maxinmontreal.com/forums/index.php
//    Licensed under GPL v3: http://www.gnu.org/licenses/gpl.html
//
//******************************************************************************
//
// Purpose: Finding best matches to partially known blind-levels
//   * some casinos don't display blinds at all
//   * others display garbage like 0/0
//   * others display the first level in tournaments, 
//     then freeze and overpaint the titlebar with something 
//     that looks like a title
//   * some display sb/bb, some display bb/BB 
//     and others use the same title-format for both, depending on game-type
//   * ...
//   There are no easy solutions unfortunately if you want to make OH work 
//   at every casino out there.
//
//******************************************************************************

#include "stdafx.h"
#include "CBlindLevels.h"

#include "CPreferences.h"

// Small-blind, big-blind, big-bet
const int kNumberOfValuesPerLevel =   3; 
const int kNumberOfBlindLevels    = 207;
const int kLastBlindLevel         = kNumberOfBlindLevels - 1;

// http://www.maxinmontreal.com/forums/viewtopic.php?f=117&t=17380&start=60&p=125232&view=show#p125232
const double kBlindLevels[kNumberOfBlindLevels][kNumberOfValuesPerLevel] =
  {// Below levels for 
   //   * real-money online cash-games
   //   * real online tournaments 
   //   * other sane-looking blind-levels
   // in increasing order.
   // Very special blind-levels for offline-casinos
   // out of order at the very end.
   {        0.01,         0.02,         0.04},
   {        0.01,         0.02,         0.05},
   {        0.02,         0.04,         0.08},
   {        0.02,         0.05,         0.10},
   {        0.03,         0.06,         0.12},
   {        0.04,         0.08,         0.16},
   {        0.05,         0.10,         0.20},
   {        0.05,         0.10,         0.25},
   {        0.06,         0.12,         0.24},
   {        0.06,         0.12,         0.25},
   {        0.07,         0.14,         0.28},
   {        0.08,         0.16,         0.32},
   {        0.10,         0.20,         0.40},
   {        0.10,         0.25,         0.50},
   {        0.12,         0.25,         0.50},
   {        0.15,         0.25,         0.50},
   {        0.15,         0.30,         0.60},
   {        0.25,         0.50,         1.00},
   {        0.50,         1.00,         2.00},
   {        1.00,         2.00,         4.00},
   {        2.00,         4.00,         8.00},
   {        2.50,         5.00,        10.00},
   {        3.00,         6.00,        12.00},
   {        4.00,         8.00,        16.00},
   {        5.00,        10.00,        20.00},
   {       10.00,        20.00,        40.00},
   {       15.00,        25.00,        50.00},
   {       15.00,        30.00,        60.00},
   {       20.00,        40.00,        80.00},
   {       25.00,        50.00,       100.00},
   {       30.00,        60.00,       120.00},
   {       35.00,        70.00,       140.00},
   {       40.00,        80.00,       160.00},
   {       45.00,        90.00,       180.00},
   {       50.00,       100.00,       200.00},
   {       55.00,       110.00,       220.00},
   {       60.00,       120.00,       240.00},
   {       65.00,       130.00,       260.00},
   {       70.00,       140.00,       280.00},
   {       75.00,       150.00,       300.00},
   {       80.00,       160.00,       320.00},
   {       85.00,       170.00,       340.00},
   {       90.00,       180.00,       360.00},
   {       95.00,       190.00,       380.00},
   {      100.00,       200.00,       400.00},
   {      105.00,       210.00,       420.00},
   {      110.00,       220.00,       440.00},
   {      120.00,       240.00,       480.00},
   {      125.00,       250.00,       500.00},
   {      130.00,       260.00,       520.00},
   {      140.00,       280.00,       560.00},
   {      150.00,       300.00,       600.00},
   {      160.00,       320.00,       640.00},
   {      170.00,       340.00,       680.00},
   {      175.00,       350.00,       700.00},
   {      180.00,       360.00,       720.00},
   {      190.00,       380.00,       760.00},
   {      200.00,       400.00,       800.00},
   {      225.00,       450.00,       900.00},
   {      250.00,       500.00,      1000.00},
   {      300.00,       600.00,      1200.00},
   {      350.00,       700.00,      1400.00},
   {      400.00,       800.00,      1600.00},
   {      450.00,       900.00,      1800.00},
   {      500.00,      1000.00,      2000.00},
   {      550.00,      1100.00,      2200.00},
   {      600.00,      1200.00,      2400.00},
   {      625.00,      1250.00,      2500.00},
   {      650.00,      1300.00,      2600.00},
   {      700.00,      1400.00,      2800.00},
   {      750.00,      1500.00,      3000.00},
   {      800.00,      1600.00,      3200.00},
   {      850.00,      1700.00,      3400.00},
   {      900.00,      1800.00,      3600.00},
   {      950.00,      1900.00,      3800.00},
   {     1000.00,      2000.00,      4000.00},      
   {     1100.00,      2200.00,      4400.00},      
   {     1200.00,      2400.00,      4800.00},      
   {     1250.00,      2500.00,      5000.00},      
   {     1300.00,      2600.00,      5200.00},      
   {     1350.00,      2700.00,      5400.00},      
   {     1400.00,      2800.00,      5600.00},      
   {     1450.00,      2900.00,      5800.00},      
   {     1500.00,      3000.00,      6000.00},      
   {     1600.00,      3200.00,      6400.00},      
   {     1700.00,      3400.00,      6800.00},      
   {     1750.00,      3500.00,      7000.00},      
   {     1800.00,      3600.00,      7200.00},      
   {     1900.00,      3800.00,      7600.00},      
   {     2000.00,      4000.00,      8000.00},      
   {     2100.00,      4200.00,      8400.00},      
   {     2200.00,      4400.00,      8800.00},      
   {     2300.00,      4600.00,      9200.00},      
   {     2400.00,      4800.00,      9600.00},      
   {     2500.00,      5000.00,     10000.00},      
   {     2600.00,      5200.00,     10400.00},      
   {     2700.00,      5400.00,     10800.00},      
   {     2800.00,      5600.00,     11200.00},      
   {     2900.00,      5800.00,     11600.00},      
   {     3000.00,      6000.00,     12000.00},      
   {     3500.00,      7000.00,     14000.00},      
   {     4000.00,      8000.00,     16000.00},      
   {     5000.00,     10000.00,     20000.00},
   {     6000.00,     12000.00,     24000.00},
   {     6500.00,     13000.00,     26000.00},
   {     7000.00,     14000.00,     28000.00},
   {     7500.00,     15000.00,     30000.00},
   {     8000.00,     16000.00,     32000.00},
   {     8500.00,     17000.00,     34000.00},
   {    10000.00,     20000.00,     40000.00},
   {    12000.00,     24000.00,     48000.00},
   {    12500.00,     25000.00,     50000.00},
   {    14000.00,     28000.00,     56000.00},
   {    15000.00,     30000.00,     60000.00},
   {    16000.00,     32000.00,     64000.00},
   {    17500.00,     35000.00,     70000.00},
   {    18000.00,     36000.00,     72000.00},
   {    20000.00,     40000.00,     80000.00},
   {    22500.00,     45000.00,     90000.00},
   {    25000.00,     50000.00,    100000.00},
   {    27500.00,     55000.00,    110000.00},
   {    30000.00,     60000.00,    120000.00},
   {    35000.00,     70000.00,    140000.00},
   {    40000.00,     80000.00,    160000.00},
   {    45000.00,     90000.00,    180000.00},
   {    50000.00,    100000.00,    200000.00},   
   {    55000.00,    110000.00,    220000.00},   
   {    60000.00,    120000.00,    240000.00},   
   {    62500.00,    125000.00,    250000.00},   
   {    65000.00,    130000.00,    260000.00},   
   {    70000.00,    140000.00,    280000.00},   
   {    75000.00,    150000.00,    300000.00},   
   {    80000.00,    160000.00,    320000.00},   
   {    90000.00,    180000.00,    360000.00},   
   {   100000.00,    200000.00,    400000.00},   
   {   105000.00,    210000.00,    420000.00},   
   {   120000.00,    240000.00,    480000.00},   
   {   125000.00,    250000.00,    500000.00},   
   {   140000.00,    280000.00,    560000.00},   
   {   150000.00,    300000.00,    600000.00},   
   {   160000.00,    320000.00,    640000.00},   
   {   170000.00,    340000.00,    680000.00},   
   {   175000.00,    350000.00,    700000.00},   
   {   180000.00,    360000.00,    720000.00},   
   {   200000.00,    400000.00,    800000.00},   
   {   250000.00,    500000.00,   1000000.00},   
   {   300000.00,    600000.00,   1200000.00},   
   {   350000.00,    700000.00,   1400000.00},   
   {   400000.00,    800000.00,   1600000.00},   
   {   450000.00,    900000.00,   1800000.00},   
   {   500000.00,   1000000.00,   2000000.00},
   {   550000.00,   1100000.00,   2200000.00},
   {   600000.00,   1200000.00,   2400000.00},
   {   650000.00,   1300000.00,   2600000.00},
   {   700000.00,   1400000.00,   2800000.00},
   {   750000.00,   1500000.00,   3000000.00},
   {   800000.00,   1600000.00,   3200000.00},
   {   850000.00,   1700000.00,   3400000.00},
   {   900000.00,   1800000.00,   3600000.00},
   {   950000.00,   1900000.00,   3800000.00},
   {  1000000.00,   2000000.00,   4000000.00},
   {  1250000.00,   2500000.00,   5000000.00},
   {  1500000.00,   3000000.00,   6000000.00},
   {  1750000.00,   3500000.00,   7000000.00},
   {  1800000.00,   3600000.00,   7200000.00},
   {  2000000.00,   4000000.00,   8000000.00},
   {  2250000.00,   4500000.00,   9000000.00},
   {  2500000.00,   5000000.00,  10000000.00},
   {  2750000.00,   5500000.00,  11000000.00},
   {  3000000.00,   6000000.00,  12000000.00},
   {  3500000.00,   7000000.00,  14000000.00},
   {  4000000.00,   8000000.00,  16000000.00},
   {  4500000.00,   9000000.00,  18000000.00},
   {  5000000.00,  10000000.00,  20000000.00},
   {  6000000.00,  12000000.00,  24000000.00},
   {  7500000.00,  15000000.00,  30000000.00},
   {  9000000.00,  18000000.00,  36000000.00},
   { 10000000.00,  20000000.00,  40000000.00},
   { 10500000.00,  21000000.00,  42000000.00},
   { 12000000.00,  24000000.00,  48000000.00},
   { 12500000.00,  25000000.00,  50000000.00},
   { 14000000.00,  28000000.00,  56000000.00},
   { 16000000.00,  32000000.00,  64000000.00},
   { 18000000.00,  36000000.00,  72000000.00},
   { 20000000.00,  40000000.00,  80000000.00},
   { 22500000.00,  45000000.00,  90000000.00},
   { 25000000.00,  50000000.00, 100000000.00},
   { 27500000.00,  55000000.00, 110000000.00},
   { 30000000.00,  60000000.00, 120000000.00},
   { 40000000.00,  80000000.00, 160000000.00},
   { 50000000.00, 100000000.00, 200000000.00},
   // Non-standard blind-levels of a Georgian casino
   // http://www.maxinmontreal.com/forums/viewtopic.php?f=156&t=19516
   // We put them out of order to avoid problems
   // with completion.of partially known blinds
   // of other real casinos with more common levels.
   { 0.10,         0.10,         0.10 },
   { 0.25,         0.25,         0.25 },
   { 0.50,         0.50,         0.50 },
   { 1.00,         1.00,         1.00 },
   { 2.00,         2.00,         2.00 },
   { 5.00,         5.00,         5.00 },
   { 10.00,        10.00,        10.00 },
   // Finally special blind-levels like 1/1 and 1000/3000
   // from offline-simulators like DDPoker.
   {        1.00,         1.00,         2.00},
   {        2.00,         3.00,         6.00},
   {        3.00,         5.00,        10.00},
   {       10.00,        15.00,        30.00},
   {       15.00,        25.00,        50.00},
   {       25.00,        25.00,        50.00},
   {       50.00,        75.00,       150.00},
   {     2000.00,      5000.00,     10000.00}, 
   {     1000.00,      3000.00,      6000.00},
};

CBlindLevels::CBlindLevels () {
}

CBlindLevels::~CBlindLevels () {
}

bool CBlindLevels::BlindsMatchBlindLevelPerfectly(
    const int level,
    const double sblind, 
    const double bblind, 
    const double bbet) {
  if ((sblind <= 0) && (bblind <= 0) && (bbet <= 0))      return false;
  if ((sblind > 0) && (sblind != kBlindLevels[level][0])) return false;                                      
  if ((bblind > 0) && (bblind != kBlindLevels[level][1])) return false;  
  if ((bbet   > 0) && (bbet   != kBlindLevels[level][2])) return false;  
  write_log(preferences.debug_table_limits(), 
    "[CBlindLevels] Perfect match found\n");
  return true;
}

bool CBlindLevels::BlindsMatchBlindLevelPartially(
    const int level,
    const double sblind, 
    const double bblind, 
    const double bbet) {
  bool success = false;
  if ((sblind > 0) && (sblind == kBlindLevels[level][0])) {
    success = true;
  } else if ((bblind > 0) && (bblind == kBlindLevels[level][1])) {
    success = true;
  } else if ((bbet   > 0) && (bbet   == kBlindLevels[level][2]))  {
    success = true;
  } 
  if (success) {
    write_log(preferences.debug_table_limits(), 
      "[CBlindLevels] Partial match found\n");
    return true;
  }
  return false;
}

// Input:  either known level or kUndefined (-1) or kUndefinedZero
// Output: guessed levels
// Return: true if success, otherwise false
bool CBlindLevels::BestMatchingBlindLevel(double *sblind, double *bblind, double *bbet) {
  write_log(preferences.debug_table_limits(), 
    "[CBlindLevels] Trying to find best matching blind-level for %.2f / %.2f / %.2f\n",
    *sblind, *bblind, *bbet);
  // Complete fail first: nothing to guess
  if ((*sblind <= 0) && (*bblind <= 0) && (*bbet <= 0)) {
    write_log(preferences.debug_table_limits(), 
      "[CBlindLevels] No match because of bad input\n");
    return false;
  }
  // Guessing...
  for (int i=0; i<kNumberOfBlindLevels; ++i) {
    if (BlindsMatchBlindLevelPerfectly(i, *sblind, *bblind, *bbet)) {
      *sblind = kBlindLevels[i][0];
      *bblind = kBlindLevels[i][1];
      *bbet   = kBlindLevels[i][2];
      write_log(preferences.debug_table_limits(), 
        "[CBlindLevels] Blinds recognized as %.2f / %.2f / %.2f\n", 
        *sblind, *bblind, *bbet);
      return true; 
    }
  }
  for (int i=0; i<kNumberOfBlindLevels; ++i) {
    if (BlindsMatchBlindLevelPartially(i, *sblind, *bblind, *bbet)) {
      *sblind = kBlindLevels[i][0];
      *bblind = kBlindLevels[i][1];
      *bbet   = kBlindLevels[i][2];
      write_log(preferences.debug_table_limits(), 
        "[CBlindLevels] Blinds recognized as %.2f / %.2f / %.2f\n", 
        *sblind, *bblind, *bbet);
      return true; 
    }
  }
  // Nothing found
  // Keep everything as is.
  // Then another module can take these values and guess
  write_log(preferences.debug_table_limits(), 
      "[CBlindLevels] No match. Another module shall guess,\n");
  return false;
}

// Especially used for guessing the small-blind,
// which usually is 50% or sometimes 40% of the big-blind
double CBlindLevels::GetNextSmallerOrEqualBlindOnList(double guessed_blind) {
  // Highest bblind first
  if ((kBlindLevels[kLastBlindLevel][0] > 0)
      && (kBlindLevels[kLastBlindLevel][1] > 0)
      && (guessed_blind >= kBlindLevels[kLastBlindLevel][0]) 
      && (guessed_blind <= kBlindLevels[kLastBlindLevel][1])) {
    write_log(preferences.debug_table_limits(), 
      "[CBlindLevels] Best match for %.2f -> %.2f\n",
      guessed_blind, kBlindLevels[kLastBlindLevel][1]);
    return kBlindLevels[kLastBlindLevel][1];
  }
  // Then all small blinds downwards
  for (int i=kLastBlindLevel; i>=0; ++i) {
    if ((kBlindLevels[i][0] > 0)
        && (guessed_blind <= kBlindLevels[i][0])) {
      write_log(preferences.debug_table_limits(), 
        "[CBlindLevels] Best match for %.2f -> %.2f\n",
        guessed_blind, kBlindLevels[i][0]);
      return kBlindLevels[i][0];
    }
  }
  // Failure (bad input): return smallest blinds
  return kBlindLevels[0][0];
}