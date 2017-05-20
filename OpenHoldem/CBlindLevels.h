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

#ifndef INC_CBLINDLEVELS_H
#define INC_CBLINDLEVELS_H

class CBlindLevels {
 public:
  CBlindLevels();
  ~CBlindLevels();
 public:
  bool BestMatchingBlindLevel(double *sblind, double *bblind, double *bbet);
  // Especially used for guessing the small-blind,
  // which usually is 50% or sometimes 40% of the big-blind
  double GetNextSmallerOrEqualBlindOnList(double guessed_blind);
 private:
  bool BlindsMatchBlindLevelPerfectly(const int level,
                             const double sblind, 
                             const double bblind, 
                             const double bbet);
  bool BlindsMatchBlindLevelPartially(const int level,
                             const double sblind, 
                             const double bblind, 
                             const double bbet);
};

#endif INC_CBLINDLEVELS_H