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

#include "stdafx.h"
#include "CFlagsToolbar.h"

#include <afxwin.h>
#include "CAutoplayer.h"
#include "CEngineContainer.h"
#include "MyCtoolbar.h"
#include "resource.h"


CFlagsToolbar *p_flags_toolbar = NULL;

#define ENT CSLock lock(m_critsec);

BEGIN_MESSAGE_MAP(CFlagsToolbar, CWnd)
END_MESSAGE_MAP()

CFlagsToolbar::CFlagsToolbar(CFrameWnd *parent_window)
{
	_parent_window = parent_window;
	CreateMainToolbar();
	CreateFlagsToolbar();
	AlignToolbars();

	bool to_be_enabled_or_not = p_autoplayer->autoplayer_engaged();
	//p_flags_toolbar->
	m_MainToolBar.GetToolBarCtrl().CheckButton(ID_MAIN_TOOLBAR_AUTOPLAYER, to_be_enabled_or_not);
	for (int i=0; i<kNumberOfFlags; i++)
	{
		SetFlag(i, false);
	}
  ResetButtonsOnDisconnect();
  ResetButtonsOnAutoplayerOff();
}

CFlagsToolbar::~CFlagsToolbar()
{}

void CFlagsToolbar::ResetButtonsOnConnect() {
	ResetButtonsOnAutoplayerOff();
	m_MainToolBar.GetToolBarCtrl().EnableButton(ID_MAIN_TOOLBAR_SHOOTFRAME, true);
  m_MainToolBar.GetToolBarCtrl().EnableButton(ID_MAIN_TOOLBAR_SCRAPER_OUTPUT, true);
}

void CFlagsToolbar::ResetButtonsOnDisconnect() {
  ResetButtonsOnAutoplayerOff();
	m_MainToolBar.GetToolBarCtrl().CheckButton(ID_MAIN_TOOLBAR_AUTOPLAYER, false);
  m_MainToolBar.GetToolBarCtrl().EnableButton(ID_MAIN_TOOLBAR_SCRAPER_OUTPUT, false);
}

void CFlagsToolbar::ResetButtonsOnAutoplayerOn() {
  m_MainToolBar.GetToolBarCtrl().EnableButton(ID_FILE_NEW, false);
	m_MainToolBar.GetToolBarCtrl().EnableButton(ID_FILE_OPEN, false);
  m_MainToolBar.GetToolBarCtrl().EnableButton(ID_EDIT_FORMULA, false);
  CheckButton(ID_MAIN_TOOLBAR_AUTOPLAYER, true);
}

void CFlagsToolbar::ResetButtonsOnAutoplayerOff() {
  m_MainToolBar.GetToolBarCtrl().EnableButton(ID_FILE_NEW, true);
	m_MainToolBar.GetToolBarCtrl().EnableButton(ID_FILE_OPEN, true);
  m_MainToolBar.GetToolBarCtrl().EnableButton(ID_EDIT_FORMULA, true);
  CheckButton(ID_MAIN_TOOLBAR_AUTOPLAYER, false);
}

bool CFlagsToolbar::GetFlag(int flag_number)
{
	ENT
	if (flag_number>=0 && flag_number<kNumberOfFlags) 
	{
		return _flags[flag_number]; 
	}
	else 
	{
		return false; 
	}
}

int CFlagsToolbar::GetFlagMax()
{
	int flag_max = -1;
	for (int i=0; i<kNumberOfFlags; i++)
	{
		if (_flags[i])
		{
			flag_max = i;
		}
	}
	return flag_max;
}

long int CFlagsToolbar::GetFlagBits()
{
	long int flag_bits = 0;
	for (int i=0; i<kNumberOfFlags; i++)
	{
		if (_flags[i])
		{
			flag_bits |= (1 << i);
		}
	}
	return flag_bits;
}


void CFlagsToolbar::SetFlag(int flag_number, BOOL new_value)
{
	ENT

	assert((void*)_tool_bar != NULL);
	// No further assertion and no error-message here, only a range-check,
	// as this function might get called by external programs
	// via window-messages.
	if (flag_number>=0 && flag_number<kNumberOfFlags) 
	{
		_flags[flag_number] = !!new_value; 
		switch (flag_number)
		{
			case 0: _tool_bar.GetToolBarCtrl().CheckButton(ID_NUMBER0, new_value); break;
			case 1: _tool_bar.GetToolBarCtrl().CheckButton(ID_NUMBER1, new_value); break;
			case 2: _tool_bar.GetToolBarCtrl().CheckButton(ID_NUMBER2, new_value); break;
			case 3: _tool_bar.GetToolBarCtrl().CheckButton(ID_NUMBER3, new_value); break;
			case 4: _tool_bar.GetToolBarCtrl().CheckButton(ID_NUMBER4, new_value); break;
			case 5: _tool_bar.GetToolBarCtrl().CheckButton(ID_NUMBER5, new_value); break;
			case 6: _tool_bar.GetToolBarCtrl().CheckButton(ID_NUMBER6, new_value); break;
			case 7: _tool_bar.GetToolBarCtrl().CheckButton(ID_NUMBER7, new_value); break;
			case 8: _tool_bar.GetToolBarCtrl().CheckButton(ID_NUMBER8, new_value); break;
			case 9: _tool_bar.GetToolBarCtrl().CheckButton(ID_NUMBER9, new_value); break;
			case 10: _tool_bar.GetToolBarCtrl().CheckButton(ID_NUMBER10, new_value); break;
			case 11: _tool_bar.GetToolBarCtrl().CheckButton(ID_NUMBER11, new_value); break;
			case 12: _tool_bar.GetToolBarCtrl().CheckButton(ID_NUMBER12, new_value); break;
			case 13: _tool_bar.GetToolBarCtrl().CheckButton(ID_NUMBER13, new_value); break;
			case 14: _tool_bar.GetToolBarCtrl().CheckButton(ID_NUMBER14, new_value); break;
			case 15: _tool_bar.GetToolBarCtrl().CheckButton(ID_NUMBER15, new_value); break;
			case 16: _tool_bar.GetToolBarCtrl().CheckButton(ID_NUMBER16, new_value); break;
			case 17: _tool_bar.GetToolBarCtrl().CheckButton(ID_NUMBER17, new_value); break;
			case 18: _tool_bar.GetToolBarCtrl().CheckButton(ID_NUMBER18, new_value); break;
			case 19: _tool_bar.GetToolBarCtrl().CheckButton(ID_NUMBER19, new_value); break;
		}
	}
}

void CFlagsToolbar::OnClickedFlags() {
	assert((void*)_tool_bar != NULL);
	SetFlag(0,  _tool_bar.GetToolBarCtrl().IsButtonChecked(ID_NUMBER0));
	SetFlag(1,  _tool_bar.GetToolBarCtrl().IsButtonChecked(ID_NUMBER1));
	SetFlag(2,  _tool_bar.GetToolBarCtrl().IsButtonChecked(ID_NUMBER2));
	SetFlag(3,  _tool_bar.GetToolBarCtrl().IsButtonChecked(ID_NUMBER3));
	SetFlag(4,  _tool_bar.GetToolBarCtrl().IsButtonChecked(ID_NUMBER4));
	SetFlag(5,  _tool_bar.GetToolBarCtrl().IsButtonChecked(ID_NUMBER5));
	SetFlag(6,  _tool_bar.GetToolBarCtrl().IsButtonChecked(ID_NUMBER6));
	SetFlag(7,  _tool_bar.GetToolBarCtrl().IsButtonChecked(ID_NUMBER7));
	SetFlag(8,  _tool_bar.GetToolBarCtrl().IsButtonChecked(ID_NUMBER8));
	SetFlag(9,  _tool_bar.GetToolBarCtrl().IsButtonChecked(ID_NUMBER9));
	SetFlag(10, _tool_bar.GetToolBarCtrl().IsButtonChecked(ID_NUMBER10));
	SetFlag(11, _tool_bar.GetToolBarCtrl().IsButtonChecked(ID_NUMBER11));
	SetFlag(12, _tool_bar.GetToolBarCtrl().IsButtonChecked(ID_NUMBER12));
	SetFlag(13, _tool_bar.GetToolBarCtrl().IsButtonChecked(ID_NUMBER13));
	SetFlag(14, _tool_bar.GetToolBarCtrl().IsButtonChecked(ID_NUMBER14));
	SetFlag(15, _tool_bar.GetToolBarCtrl().IsButtonChecked(ID_NUMBER15));
	SetFlag(16, _tool_bar.GetToolBarCtrl().IsButtonChecked(ID_NUMBER16));
	SetFlag(17, _tool_bar.GetToolBarCtrl().IsButtonChecked(ID_NUMBER17));
	SetFlag(18, _tool_bar.GetToolBarCtrl().IsButtonChecked(ID_NUMBER18));
	SetFlag(19, _tool_bar.GetToolBarCtrl().IsButtonChecked(ID_NUMBER19));
	// No longer calling p_engine_container->EvaluateAll();
  // It might cause unexpected side-effects,
  // if we evaluate multiple times during one heartbeat.
  // The effect of this button-click will have to wait
  // till the next heartbeat starts.
}

void CFlagsToolbar::CreateMainToolbar(void)
{
	TBBUTTONINFO	tbi;

	tbi.cbSize = sizeof(TBBUTTONINFO);
	tbi.dwMask = TBIF_STYLE;
	tbi.fsStyle = TBSTYLE_CHECK;

	// Main toolbar
	m_MainToolBar.CreateEx(_parent_window, NULL, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER |
						   CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);
	m_MainToolBar.LoadToolBar(IDR_MAINFRAME);
	m_MainToolBar.EnableDocking(CBRS_ALIGN_TOP);
	
	// Make formula button sticky
	m_MainToolBar.GetToolBarCtrl().SetButtonInfo(ID_MAIN_TOOLBAR_FORMULA, &tbi);

	// Make validator button sticky
	m_MainToolBar.GetToolBarCtrl().SetButtonInfo(ID_MAIN_TOOLBAR_VALIDATOR, &tbi);
	m_MainToolBar.GetToolBarCtrl().SetButtonInfo(ID_MAIN_TOOLBAR_TAGLOGFILE, &tbi);

	// Make scraper output button sticky
	m_MainToolBar.GetToolBarCtrl().SetButtonInfo(ID_MAIN_TOOLBAR_SCRAPER_OUTPUT, &tbi);

	// Make autoplayer button sticky, and start it out disabled
	m_MainToolBar.GetToolBarCtrl().SetButtonInfo(ID_MAIN_TOOLBAR_AUTOPLAYER, &tbi);
	m_MainToolBar.GetToolBarCtrl().EnableButton(ID_MAIN_TOOLBAR_AUTOPLAYER, false);

	// Start shoot replay frame button disabled
	m_MainToolBar.GetToolBarCtrl().EnableButton(ID_MAIN_TOOLBAR_SHOOTFRAME, false);

  // Title of floating main toolbar
	m_MainToolBar.SetWindowText("Main");
}

void CFlagsToolbar::CreateFlagsToolbar(void)
{
	TBBUTTONINFO	tbi;

	tbi.cbSize = sizeof(TBBUTTONINFO);
	tbi.dwMask = TBIF_STYLE;
	tbi.fsStyle = TBSTYLE_CHECK;

	// Flags toolbar
	_tool_bar.CreateEx(_parent_window, NULL, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER |
							CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);
	_tool_bar.LoadToolBar(IDR_FLAGS);

	assert((void*)_tool_bar != NULL);
	// Make flags toolbar buttons sticky
	_tool_bar.GetToolBarCtrl().SetButtonInfo(ID_NUMBER0, &tbi);
	_tool_bar.GetToolBarCtrl().SetButtonInfo(ID_NUMBER1, &tbi);
	_tool_bar.GetToolBarCtrl().SetButtonInfo(ID_NUMBER2, &tbi);
	_tool_bar.GetToolBarCtrl().SetButtonInfo(ID_NUMBER3, &tbi);
	_tool_bar.GetToolBarCtrl().SetButtonInfo(ID_NUMBER4, &tbi);
	_tool_bar.GetToolBarCtrl().SetButtonInfo(ID_NUMBER5, &tbi);
	_tool_bar.GetToolBarCtrl().SetButtonInfo(ID_NUMBER6, &tbi);
	_tool_bar.GetToolBarCtrl().SetButtonInfo(ID_NUMBER7, &tbi);
	_tool_bar.GetToolBarCtrl().SetButtonInfo(ID_NUMBER8, &tbi);
	_tool_bar.GetToolBarCtrl().SetButtonInfo(ID_NUMBER9, &tbi);
	_tool_bar.GetToolBarCtrl().SetButtonInfo(ID_NUMBER10, &tbi);
	_tool_bar.GetToolBarCtrl().SetButtonInfo(ID_NUMBER11, &tbi);
	_tool_bar.GetToolBarCtrl().SetButtonInfo(ID_NUMBER12, &tbi);
	_tool_bar.GetToolBarCtrl().SetButtonInfo(ID_NUMBER13, &tbi);
	_tool_bar.GetToolBarCtrl().SetButtonInfo(ID_NUMBER14, &tbi);
	_tool_bar.GetToolBarCtrl().SetButtonInfo(ID_NUMBER15, &tbi);
	_tool_bar.GetToolBarCtrl().SetButtonInfo(ID_NUMBER16, &tbi);
	_tool_bar.GetToolBarCtrl().SetButtonInfo(ID_NUMBER17, &tbi);
	_tool_bar.GetToolBarCtrl().SetButtonInfo(ID_NUMBER18, &tbi);
	_tool_bar.GetToolBarCtrl().SetButtonInfo(ID_NUMBER19, &tbi);
	_tool_bar.EnableDocking(CBRS_ALIGN_TOP);
	// Title of floating flags toolbar
	_tool_bar.SetWindowText("Flags");
	assert((void*)_tool_bar != NULL);
}

void CFlagsToolbar::AlignToolbars(void)
{
	// Put the main toolbar and flags toolbar on the same line
	CRect rectBar1, rectBar2;

	_parent_window->RecalcLayout(true);

	m_MainToolBar.GetWindowRect(rectBar1);
	_tool_bar.GetWindowRect(rectBar2);

	UINT uiBarWidth = rectBar2.Width();
	rectBar2.left = rectBar1.right;
	rectBar2.top = rectBar1.top;
	rectBar2.bottom = rectBar1.bottom;
	rectBar2.right = rectBar1.right + uiBarWidth;

	_parent_window->RecalcLayout();
}

void CFlagsToolbar::UnattachOHFromPokerWindow() {
}

void CFlagsToolbar::EnableButton(int button_ID, bool new_status)
{
	m_MainToolBar.GetToolBarCtrl().EnableButton(button_ID, new_status);
}

void CFlagsToolbar::CheckButton(int button_ID, bool new_status)
{
	m_MainToolBar.GetToolBarCtrl().CheckButton(button_ID, new_status);
}

bool CFlagsToolbar::IsButtonChecked(int button_ID)
{
	return !!m_MainToolBar.GetToolBarCtrl().IsButtonChecked(button_ID);
}

bool CFlagsToolbar::IsButtonEnabled(int button_ID)
{
	return !!m_MainToolBar.GetToolBarCtrl().IsButtonEnabled(button_ID);
}