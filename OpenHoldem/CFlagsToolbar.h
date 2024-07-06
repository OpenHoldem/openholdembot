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

#ifndef INC_CFLAGSTOOLBAR_H
#define INC_CFLAGSTOOLBAR_H

#include "MyCtoolbar.h"

class CFlagsToolbar: public CWnd {
 public:
	CFlagsToolbar(CFrameWnd *parent_window);
	~CFlagsToolbar();
 public:
	bool GetFlag(const int i); 
	int  GetFlagMax();
	long int  GetFlagBits();
  void SetFlag(int flag_number, BOOL new_value);
 public:
	void ResetButtonsOnConnect();
	void ResetButtonsOnDisconnect();
  void ResetButtonsOnAutoplayerOn();
  void ResetButtonsOnAutoplayerOff();
	void UnattachOHFromPokerWindow();
 public:
	void EnableButton(int button_ID, bool new_status);
	void CheckButton(int button_ID, bool new_status);
	bool IsButtonChecked(int button_ID);
	bool IsButtonEnabled(int button_ID);
 public:
	CMyToolBar _tool_bar;
 public:
	afx_msg void OnClickedFlags();
	DECLARE_MESSAGE_MAP()
 private:
	void CreateMainToolbar();
	void CreateFlagsToolbar();
	void AlignToolbars();
 private:
	CMyToolBar m_MainToolBar;
	bool       _flags[kNumberOfFlags];
	CFrameWnd  *_parent_window;
	CCritSec   m_critsec;
};

extern CFlagsToolbar *p_flags_toolbar;

#endif INC_CFLAGSTOOLBAR_H