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

// DialogSAPrefs10.cpp : implementation file
//

#include "stdafx.h"

#include "SAPrefsSubDlg.h"
#include "DialogSAPrefs10.h"

#include "WindowFunctions/window_functions.h"

#define MAX_DELAY 1000000

// DialogSAPrefs10 dialog

IMPLEMENT_DYNAMIC(CDlgSAPrefs10, CDialog)

CDlgSAPrefs10::CDlgSAPrefs10(CWnd* pParent /*=NULL*/)
		: CSAPrefsSubDlg(CDlgSAPrefs10::IDD, pParent)
{
}

CDlgSAPrefs10::~CDlgSAPrefs10()
{
}

void CDlgSAPrefs10::DoDataExchange(CDataExchange* pDX)
{
	CSAPrefsSubDlg::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MINIMUM_DELAY, m_MinimumDelay);
	DDX_Control(pDX, IDC_MINIMUM_DELAY_SPIN, m_MinimumDelay_Spin);
	DDX_Control(pDX, IDC_RANDOM_DELAY, m_RandomDelay);
	DDX_Control(pDX, IDC_RANDOM_DELAY_SPIN, m_RandomDelay_Spin);
}

// DialogSAPrefs10 message handlers
BOOL CDlgSAPrefs10::OnInitDialog()
{
	CSAPrefsSubDlg::OnInitDialog();
	CString		text = "";

	text.Format("%d", Preferences()->chat_min_delay());
	m_MinimumDelay.SetWindowText(text);
	m_MinimumDelay_Spin.SetRange(0, (short) MAX_DELAY);
	m_MinimumDelay_Spin.SetPos(Preferences()->chat_min_delay());
	m_MinimumDelay_Spin.SetBuddy(&m_MinimumDelay);

	text.Format("%d", Preferences()->chat_random_delay());
	m_RandomDelay.SetWindowText(text);
	m_RandomDelay_Spin.SetRange(0, (short) MAX_DELAY);
	m_RandomDelay_Spin.SetPos(Preferences()->chat_random_delay());
	m_RandomDelay_Spin.SetBuddy(&m_RandomDelay);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgSAPrefs10::OnOK()
{
	CString			text = "";

	m_MinimumDelay.GetWindowText(text);
	if (strtoul(text.GetString(), 0, 10)<0 || strtoul(text.GetString(), 0, 10)>MAX_DELAY) {
		MessageBox_Interactive("Invalid minimum Chat Delay", "ERROR", MB_OK);
		return;
	}
	Preferences()->SetValue(k_prefs_chat_min_delay, strtoul(text.GetString(), 0, 10));

	m_RandomDelay.GetWindowText(text);
	if (strtoul(text.GetString(), 0, 10)<0 || strtoul(text.GetString(), 0, 10)>MAX_DELAY) {
		MessageBox_Interactive("Invalid random Chat Delay", "ERROR", MB_OK);
		return;
	}
	Preferences()->SetValue(k_prefs_chat_random_delay, strtoul(text.GetString(), 0, 10));

	CSAPrefsSubDlg::OnOK();
}


