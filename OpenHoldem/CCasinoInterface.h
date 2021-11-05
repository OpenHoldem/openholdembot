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

#ifndef INC_CCASINOINTERFACE_H
#define INC_CCASINOINTERFACE_H

#include "CAllinSlider.h"
#include "CAutoplayerButton.h"
#include "CBetsizeInputBox.h"
#include "CCasinoHotkey.h"
#include "CSpaceOptimizedGlobalObject.h"


class CCasinoInterface: public CSpaceOptimizedGlobalObject {
  friend class CAllinSlider;
  friend class CAutoplayer;
  friend class CScraper;
  friend class CSymbolEngineAutoplayer;
 public:
	CCasinoInterface();
	~CCasinoInterface();
 public:
  void Reset();
 public:
	bool ClickButtonSequence(int first_button, int second_button, int delay_in_milli_seconds);
	bool EnterBetsize(double total_betsize_in_dollars);
  bool EnterBetsizeForAllin();
	bool UseSliderForAllin();
	bool CloseWindow();
	bool EnterChatMessage(CString &message);
	int  NumberOfVisibleAutoplayerButtons();
  bool IsMyTurn();
  void PressTabToSwitchOHReplayToNextFrame();
  bool HandleInterfacebuttonsI86();
  bool TableLostFocus();
  void ClickRect(RECT rect); 
  void DoubleClickRect(RECT rect); 
  void SendKey(const char ascii_key);
 private:
	POINT p_null;
 public:
  // For logical lookup of buttons
  CAutoplayerButton* LogicalAutoplayerButton(int autoplayer_function_code);
  CAutoplayerButton* LogicalAutoplayerButton(CString ButtonLabel);
  CAutoplayerButton* BetsizeConfirmationButton();
 public:
  bool AllinOptionAvailable();
 protected:
  // To be used by the scraper
  CAutoplayerButton _technical_autoplayer_buttons[k_max_number_of_buttons];
  CAutoplayerButton _technical_betpot_buttons[k_max_betpot_buttons];
  CAutoplayerButton _technical_i86X_spam_buttons[k_max_number_of_i86X_buttons];
  CAllinSlider _allin_slider;
 protected:
  // To be used by the slider to confirm the betsize
  CBetsizeInputBox  _betsize_input_box;
 private:
  // Avoid returning NULL if a certain button can not be looked up.
  CAutoplayerButton _non_clickable_fake_button;
 private:
  CCasinoHotkey casino_hotkeys;
  int _next_i86_starting_button;
};

extern CCasinoInterface *p_casino_interface;

#endif // INC_CCasinoInterface_H