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

// DialogSAPrefs1.cpp : implementation file
//

#include "pch.h"

#include "SAPrefsSubDlg.h"
#include "DialogSAPrefs17.h"


// CDlgSAPrefs17 dialog

IMPLEMENT_DYNAMIC(CDlgSAPrefs17, CSAPrefsSubDlg)

CDlgSAPrefs17::CDlgSAPrefs17(CWnd* pParent /*=NULL*/)
		: CSAPrefsSubDlg(CDlgSAPrefs17::IDD, pParent)
{
}

CDlgSAPrefs17::~CDlgSAPrefs17()
{
}

void CDlgSAPrefs17::DoDataExchange(CDataExchange* pDX)
{
	CSAPrefsSubDlg::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CONFIGURATION_INPUT_SETTINGS, m_ConfigurationInputSettings_Button);
	DDX_Control(pDX, IDC_CONFIGURATION_FONT_SETTINGS, m_ConfigurationFontSettings_Button);
}

BEGIN_MESSAGE_MAP(CDlgSAPrefs17, CSAPrefsSubDlg)
  ON_BN_CLICKED(IDC_CHECK1, &CDlgSAPrefs17::OnBnClickedCheck1)
END_MESSAGE_MAP()

// CDlgSAPrefs1 message handlers
BOOL CDlgSAPrefs17::OnInitDialog()
{
	CSAPrefsSubDlg::OnInitDialog();
	m_ConfigurationInputSettings_Button.SetCheck(Preferences()->configurationcheck_input_settings() != 0);
	m_ConfigurationFontSettings_Button.SetCheck(Preferences()->configurationcheck_font_settings() != 0);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgSAPrefs17::OnOK()
{
	Preferences()->SetValue(k_prefs_configurationcheck_input_settings, m_ConfigurationInputSettings_Button.GetCheck());
	Preferences()->SetValue(k_prefs_configurationcheck_font_settings, m_ConfigurationFontSettings_Button.GetCheck());
	CSAPrefsSubDlg::OnOK();
}

void CDlgSAPrefs17::OnBnClickedCheck1()
{}
