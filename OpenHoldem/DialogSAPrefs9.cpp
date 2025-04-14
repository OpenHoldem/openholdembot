//******************************************************************************
//
// This file is part of the OpenHoldem project
//    Source code:           https://github.com/OpenHoldem/openholdembot/
//    Forums:                http://www.maxinmontreal.com/forums/index.php
//    Licensed under GPL v3: http://www.gnu.org/licenses/gpl.html
//
//******************************************************************************
//
// Purpose: Preferences dialog for auto-starter and auto-shutdown
//
//******************************************************************************

// DialogSAPrefs9.cpp : implementation file
//

#include "pch.h"
#include "DialogSAPrefs9.h"


#include "SAPrefsSubDlg.h"

// DialogSAPrefs9 dialog

IMPLEMENT_DYNAMIC(CDlgSAPrefs9, CDialog)

CDlgSAPrefs9::CDlgSAPrefs9(CWnd* pParent /*=NULL*/)
		: CSAPrefsSubDlg(CDlgSAPrefs9::IDD, pParent)
{
}

CDlgSAPrefs9::~CDlgSAPrefs9()
{
}

void CDlgSAPrefs9::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_USE_AUTO_STARTER, m_use_auto_starter);
  DDX_Control(pDX, IDC_USE_AUTO_SHUTDOWN, m_use_auto_shutdown);
}

BOOL CDlgSAPrefs9::OnInitDialog()
{
  CSAPrefsSubDlg::OnInitDialog();
  m_use_auto_starter.SetCheck(Preferences()->use_auto_starter() ? BST_CHECKED : BST_UNCHECKED);
  m_use_auto_shutdown.SetCheck(Preferences()->use_auto_shutdown() ? BST_CHECKED : BST_UNCHECKED);
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgSAPrefs9::OnOK()
{
  Preferences()->SetValue(k_prefs_use_auto_starter, m_use_auto_starter.GetCheck() == BST_CHECKED ? true : false);
  Preferences()->SetValue(k_prefs_use_auto_shutdown, m_use_auto_shutdown.GetCheck() == BST_CHECKED ? true : false);
	CSAPrefsSubDlg::OnOK();
}

