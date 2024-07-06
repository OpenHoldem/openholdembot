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
#include "DialogSAPrefs11.h"

#include <limits.h>
#include "SAPrefsSubDlg.h"

#include "WindowFunctions/window_functions.h"

#define MAX_MAX_LOG 1000000

// DialogSAPrefs11 dialog


IMPLEMENT_DYNAMIC(CDlgSAPrefs11, CDialog)

CDlgSAPrefs11::CDlgSAPrefs11(CWnd* pParent /*=NULL*/)
		: CSAPrefsSubDlg(CDlgSAPrefs11::IDD, pParent) {
}

CDlgSAPrefs11::~CDlgSAPrefs11() {
}

void CDlgSAPrefs11::DoDataExchange(CDataExchange* pDX) {
	CSAPrefsSubDlg::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LOG_PRWIN_FUNCTIONS, m_LogPrwinFunctions);
	DDX_Control(pDX, IDC_LOG_HOPPER_FUNCTIONS, m_LogHopperFunctions);
	DDX_Control(pDX, IDC_LOG_INI_FUNCTIONS, m_LogIniFunctions);
	DDX_Control(pDX, IDC_LOG_ICM_FUNCTIONS, m_LogICMFunctions);
  DDX_Control(pDX, IDC_LOG_DELAY_FUNCTION, m_LogDelayFunction);

	DDX_Control(pDX, IDC_MAXIMUM_LOGSIZE, m_MaximumLogSize);
	DDX_Control(pDX, IDC_MAXIMUM_LOGSIZE_SPIN, m_MaximumLogSize_Spin);
}

BOOL CDlgSAPrefs11::OnInitDialog() {
	CSAPrefsSubDlg::OnInitDialog();
	CString		text = "";

	m_LogHopperFunctions.SetCheck(Preferences()->log_hopper_functions() ? BST_CHECKED : BST_UNCHECKED);
	m_LogICMFunctions.SetCheck(Preferences()->log_icm_functions() ? BST_CHECKED : BST_UNCHECKED);
	m_LogPrwinFunctions.SetCheck(Preferences()->log_prwin_functions() ? BST_CHECKED : BST_UNCHECKED);
  m_LogIniFunctions.SetCheck(Preferences()->log_ini_functions() ? BST_CHECKED : BST_UNCHECKED);
  m_LogDelayFunction.SetCheck(Preferences()->log_delay_function() ? BST_CHECKED : BST_UNCHECKED);

	text.Format("%d", Preferences()->log_max_logsize());
	text.Format("%d", Preferences()->log_max_logsize());
	m_MaximumLogSize.SetWindowText(text);
	m_MaximumLogSize_Spin.SetRange(0, 999);
	m_MaximumLogSize_Spin.SetPos(Preferences()->log_max_logsize());
	m_MaximumLogSize_Spin.SetBuddy(&m_MaximumLogSize);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

// DialogSAPrefs11 message handlers
void CDlgSAPrefs11::OnOK() {
	CString			text = "";

	Preferences()->SetValue(k_prefs_log_hopper_functions, m_LogHopperFunctions.GetCheck()==BST_CHECKED ? true : false);
	Preferences()->SetValue(k_prefs_log_icm_functions, m_LogICMFunctions.GetCheck()==BST_CHECKED ? true : false);
  Preferences()->SetValue(k_prefs_log_prwin_functions, m_LogPrwinFunctions.GetCheck()==BST_CHECKED ? true : false);
	Preferences()->SetValue(k_prefs_log_ini_functions, m_LogIniFunctions.GetCheck()==BST_CHECKED ? true : false);
  Preferences()->SetValue(k_prefs_log_delay_function, m_LogDelayFunction.GetCheck() == BST_CHECKED ? true : false);

	m_MaximumLogSize.GetWindowText(text);
	Preferences()->SetValue(k_prefs_log_max_logsize, strtoul(text.GetString(), NULL, 10));
	
	CSAPrefsSubDlg::OnOK();
}

