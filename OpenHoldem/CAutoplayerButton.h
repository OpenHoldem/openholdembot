//******************************************************************************
//
// This file is part of the OpenHoldem project
//    Source code:           https://github.com/OpenHoldem/openholdembot/
//    Forums:                http://www.maxinmontreal.com/forums/index.php
//    Licensed under GPL v3: http://www.gnu.org/licenses/gpl.html
//
//******************************************************************************
//
// Purpose: Clean interface for scraper, casino-interface amd autoplayer
//
//******************************************************************************

#ifndef INC_CAUTOPLAYERBUTTON_H
#define INC_CAUTOPLAYERBUTTON_H

#include "CCasinoHotkey.h"


class CAutoplayerButton {
  friend class CScraper;
 public:
  CAutoplayerButton();
  ~CAutoplayerButton();

  void SetTechnicalName(const CString name);
  void Reset();
  bool Click();
  bool IsClickable() { return _clickable; }

  bool IsAllin()    { return _button_type == k_autoplayer_function_allin; }
  bool IsRaise()    { return _button_type == k_autoplayer_function_raise; }
  bool IsCall()     { return _button_type == k_autoplayer_function_call; }
  bool IsCheck()    { return _button_type == k_autoplayer_function_check; }
  bool IsFold()     { return _button_type == k_autoplayer_function_fold; }
  bool IsAutopost() { return _button_type == k_hopper_function_autopost; }
  bool IsSitin()    { return _button_type == k_hopper_function_sitin; }
  bool IsSitout()   { return _button_type == k_hopper_function_sitout; }
  bool IsLeave()    { return _button_type == k_hopper_function_leave; }
  bool IsRematch()  { return _button_type == k_hopper_function_rematch; }
  bool IsPrefold()  { return _button_type == k_standard_function_prefold; }

  bool IsButtonType(int autoplayer_function_code) { return _button_type == autoplayer_function_code; }

 public:
  // To be used by the scraper
  void SetClickable(bool clickable);
  // Same as above, but to be used with input like "true", "yes", "on", ...
  void SetState(const CString state);
  void SetLabel(const CString label);

  CCasinoHotkey _hotkey;

 private:
  // The button-type of iX-buttons depends on the scraped label.
  // BetPot-buttons don't have a label, they get identified by ID
  // (position in button-array).
  // There might be multiple (moving) buttons for one action,
  // named e.g allin1 and allin2, this works as we check
  // only the first N characters.
  bool IsLabelAllin();
  bool IsLabelRaise();
  bool IsLabelCall();
  bool IsLabelCheck();
  bool IsLabelFold();
  bool IsLabelAutopost();
  bool IsLabelSitin();
  bool IsLabelSitout();
  bool IsLabelLeave();
  bool IsLabelRematch();
  bool IsLabelPrefold();
  bool IsNameI86();

  void PrecomputeButtonType();

  bool _clickable;
  // label = logical name, e.g. "raise"
  CString _label;
  CString _default_label;
  int _click_method; 
  // technical name = tablemap-object, e.g. "i3button"
  CString _technical_name;
  int _button_type;
};

#endif // INC_CAUTOPLAYERBUTTON_H