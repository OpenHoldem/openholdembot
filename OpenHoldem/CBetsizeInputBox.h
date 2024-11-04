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

#ifndef INC_CBETSIZEINPUTBOX_H
#define INC_CBETSIZEINPUTBOX_H

class CBetsizeInputBox {
  friend class CBetSlider;
 public:
  CBetsizeInputBox();
  ~CBetsizeInputBox();
 public:
  bool EnterBetsize(double total_betsize_in_dollars);
  // Depends on complete tablemap
  // and maybe visible betsize-confirmation-button
  bool IsReadyToBeUsed();
 protected:
  // To be used by the allin-slider
  void Confirm();
 private:
  bool GetI3EditRegion();
 private:
  void SelectText();
  void Clear();
 private:
  // For future use
  bool VerifyEnteredBetsize();
 private:
  RECT _i3_edit_region;
  POINT p_null;
};

#endif INC_CBETSIZEINPUTBOX_H