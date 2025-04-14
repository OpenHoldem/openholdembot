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
#include "DialogSAPrefs21.h"

#include "SAPrefsSubDlg.h"


// CDlgSAPrefs21 dialog

IMPLEMENT_DYNAMIC(CDlgSAPrefs21, CSAPrefsSubDlg)

CDlgSAPrefs21::CDlgSAPrefs21(CWnd* pParent /*=NULL*/)
		: CSAPrefsSubDlg(CDlgSAPrefs21::IDD, pParent)
{
}

CDlgSAPrefs21::~CDlgSAPrefs21()
{
}

void CDlgSAPrefs21::DoDataExchange(CDataExchange* pDX)
{
	CSAPrefsSubDlg::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_RADIO_POSITION_NEVER, _position_tables_never_button);
	DDX_Control(pDX, IDC_RADIO_POSITION_TILED, _position_tables_tiled_button);
	DDX_Control(pDX, IDC_RADIO_POSITION_CASCADED, _position_tables_cascaded_button);
	DDX_Control(pDX, IDC_ALWAYS_KEEP_POSITION, _position_always_keep_position);
}

BEGIN_MESSAGE_MAP(CDlgSAPrefs21, CSAPrefsSubDlg)
END_MESSAGE_MAP()

// CDlgSAPrefs1 message handlers
BOOL CDlgSAPrefs21::OnInitDialog()
{
	CSAPrefsSubDlg::OnInitDialog();
	if (Preferences()->table_positioner_options() == k_position_tables_tiled)
	{
		_position_tables_tiled_button.SetCheck(true);
	}
	else if (Preferences()->table_positioner_options() == k_position_tables_cascaded)
	{
		_position_tables_cascaded_button.SetCheck(true);
	}
	else
	{
		// Default: k_lazy_scraping_always
		_position_tables_never_button.SetCheck(true);
	}
	_position_always_keep_position.SetCheck(Preferences()->table_positioner_always_keep_position());

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgSAPrefs21::OnOK()
{

	if (_position_tables_tiled_button.GetCheck())
	{
		Preferences()->SetValue(k_prefs_table_positioner_options, k_position_tables_tiled);
	}
	else if (_position_tables_cascaded_button.GetCheck())
	{
		Preferences()->SetValue(k_prefs_table_positioner_options, k_position_tables_cascaded);
	}
	else
	{
		// Default: k_lazy_scraping_never
		Preferences()->SetValue(k_prefs_table_positioner_options, k_position_tables_never);
	}
	Preferences()->SetValue(k_prefs_table_positioner_always_keep_position, _position_always_keep_position.GetCheck());

	CSAPrefsSubDlg::OnOK();
}
