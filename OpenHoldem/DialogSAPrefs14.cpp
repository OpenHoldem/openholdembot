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

// DialogSAPrefs14.cpp : implementation file
//

#include "stdafx.h"

#include "SAPrefsSubDlg.h"
#include "DialogSAPrefs14.h"



// DialogSAPrefs14 dialog

IMPLEMENT_DYNAMIC(CDlgSAPrefs14, CDialog)

CDlgSAPrefs14::CDlgSAPrefs14(CWnd* pParent /*=NULL*/)
	: CSAPrefsSubDlg(CDlgSAPrefs14::IDD, pParent)
{
}

CDlgSAPrefs14::~CDlgSAPrefs14()
{
}

void CDlgSAPrefs14::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_RADIO_AUTOCONNECTOR_3, _autoconnector_connect_never);
	DDX_Control(pDX, IDC_RADIO_AUTOCONNECTOR_5, _autoconnector_connect_permanent);
}


BEGIN_MESSAGE_MAP(CDlgSAPrefs14, CDialog)
END_MESSAGE_MAP()


// DialogSAPrefs14 message handlers

BOOL CDlgSAPrefs14::OnInitDialog()
{
	CString		text = "";

	CSAPrefsSubDlg::OnInitDialog();

	if (Preferences()->autoconnector_when_to_connect() == k_AutoConnector_Connect_Never)
	{
		_autoconnector_connect_never.SetCheck(true);
	}
	else // k_AutoConnector_Connect_Permanent
	{
		_autoconnector_connect_permanent.SetCheck(true);
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


void CDlgSAPrefs14::OnOK() {
	if (_autoconnector_connect_never.GetCheck() != 0) {
		Preferences()->SetValue(k_prefs_autoconnector_when_to_connect, k_AutoConnector_Connect_Never);
	}	else { // _autoconnector_connect_permanent.GetCheck() == BST_CHECKED 
		Preferences()->SetValue(k_prefs_autoconnector_when_to_connect, k_AutoConnector_Connect_Permanent);
	}
	CSAPrefsSubDlg::OnOK();
}
