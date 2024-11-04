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

#ifndef INC_CBETSLIDER_H
#define INC_CBETSLIDER_H

class CBetSlider {
 public:
  CBetSlider();
  ~CBetSlider();
 public:
  void SetHandlePosition(const POINT position);
  void ResetHandlePosition();
  bool SlideAllin();
  bool SlideBetsize(double betsize, double betsize_for_allin);
  bool GetSliderRegions();
  bool SlideIsPossible();
 private:
  POINT _position; //?????
  RECT _i3_slider;
  RECT _i3_handle;
};

#endif INC_CBETSLIDER_H
