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

// DialogSAPrefs13.cpp : implementation file
//

#include "pch.h"

#include "SAPrefsSubDlg.h"
#include "DialogSAPrefs13.h"
#include "COpenHoldemTitle.h"



// CDlgSAPrefs13 dialog

IMPLEMENT_DYNAMIC(CDlgSAPrefs13, CDialog)

CDlgSAPrefs13::CDlgSAPrefs13(CWnd* pParent /*=NULL*/)
	: CSAPrefsSubDlg(CDlgSAPrefs13::IDD, pParent)
{

}

CDlgSAPrefs13::~CDlgSAPrefs13()
{
}

void CDlgSAPrefs13::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_OMF_CLASS_NAME, _class_name_edit);
	DDX_Control(pDX, IDC_OMF_MUTEX_NAME, _class_mutex_edit);
	//DDX_Control(pDX, IDC_SIMPLE_TITLE, _simple_window_title);
}


BEGIN_MESSAGE_MAP(CDlgSAPrefs13, CDialog)
END_MESSAGE_MAP()


// CDlgSAPrefs13 message handlers
BOOL CDlgSAPrefs13::OnInitDialog()
{
	CSAPrefsSubDlg::OnInitDialog();

	_class_name_edit.SetWindowText(Preferences()->window_class_name());
	_class_mutex_edit.SetWindowText(Preferences()->mutex_name());
	bool use_simple_title = Preferences()->simple_window_title();
	CheckDlgButton(IDC_SIMPLE_TITLE, use_simple_title ? MF_CHECKED : MF_UNCHECKED);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgSAPrefs13::OnOK()
{
	CString temp;

	_class_name_edit.GetWindowText(temp);
	Preferences()->SetValue(k_prefs_window_class_name, temp);
	_class_mutex_edit.GetWindowText(temp);
	Preferences()->SetValue(k_prefs_mutex_name, temp);
	Preferences()->SetValue(k_prefs_simple_window_title, IsDlgButtonChecked(IDC_SIMPLE_TITLE));

	p_openholdem_title->UpdateTitle();
	CSAPrefsSubDlg::OnOK();
}