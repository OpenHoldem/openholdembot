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

// CDlgSAPrefs19.cpp : implementation file
//

#include "pch.h"
#include "DialogSAPrefs19.h"

#include "SAPrefsSubDlg.h"



// CDlgSAPrefs19 dialog

IMPLEMENT_DYNAMIC(CDlgSAPrefs19, CDialog)

CDlgSAPrefs19::CDlgSAPrefs19(CWnd* pParent /*=NULL*/)
	: CSAPrefsSubDlg(CDlgSAPrefs19::IDD, pParent)
{

}

CDlgSAPrefs19::~CDlgSAPrefs19()
{
}

void CDlgSAPrefs19::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_HANDHISTORY_GENERATOR_ENABLE, _handhistory_generator_enable_Button);
}


BEGIN_MESSAGE_MAP(CDlgSAPrefs19, CDialog)
END_MESSAGE_MAP()


// CDlgSAPrefs19 message handlers
BOOL CDlgSAPrefs19::OnInitDialog()
{
	CSAPrefsSubDlg::OnInitDialog();

	_handhistory_generator_enable_Button.SetCheck(Preferences()->handhistory_generator_enable());

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgSAPrefs19::OnOK()
{
	Preferences()->SetValue(k_prefs_handhistory_generator_enable, _handhistory_generator_enable_Button.GetCheck() == BST_CHECKED ? true : false);

	CSAPrefsSubDlg::OnOK();
}