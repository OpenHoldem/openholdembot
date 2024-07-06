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

// DialogSAPrefs22.cpp : implementation file
//

#include "stdafx.h"

#include "SAPrefsSubDlg.h"
#include "DialogSAPrefs22.h"



// DialogSAPrefs22 dialog

IMPLEMENT_DYNAMIC(CDlgSAPrefs22, CDialog)

CDlgSAPrefs22::CDlgSAPrefs22(CWnd* pParent /*=NULL*/)
	: CSAPrefsSubDlg(CDlgSAPrefs22::IDD, pParent)
{
}

CDlgSAPrefs22::~CDlgSAPrefs22()
{
}

void CDlgSAPrefs22::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_RADIO_POPUP_DISABLED, _popup_disabled);
	DDX_Control(pDX, IDC_RADIO_POPUP_MINIMIZE, _popup_minimize);
	DDX_Control(pDX, IDC_RADIO_POPUP_KILL, _popup_kill);
}


BEGIN_MESSAGE_MAP(CDlgSAPrefs22, CDialog)
END_MESSAGE_MAP()


// DialogSAPrefs22 message handlers

BOOL CDlgSAPrefs22::OnInitDialog()
{
	CSAPrefsSubDlg::OnInitDialog();

	if (Preferences()->popup_blocker() == k_popup_minimize)
	{
		_popup_minimize.SetCheck(true);
	}
	else if (Preferences()->popup_blocker() == k_popup_kill)
	{
		_popup_kill.SetCheck(true);
	}
	else // k_popup_disabled 
	{
		_popup_disabled.SetCheck(true);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


void CDlgSAPrefs22::OnOK()
{
	if (_popup_minimize.GetCheck() != 0)
	{
		Preferences()->SetValue(k_prefs_popup_blocker, k_popup_minimize);
	}
	else if (_popup_kill.GetCheck() != 0)
	{
		Preferences()->SetValue(k_prefs_popup_blocker, k_popup_kill);
	}
	else // _popup_disabled.GetCheck() == true
	{
		Preferences()->SetValue(k_prefs_popup_blocker, k_popup_disabled);
	}

	CSAPrefsSubDlg::OnOK();
}
