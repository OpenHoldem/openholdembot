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
#include "DialogSAPrefs18.h"

#include "SAPrefsSubDlg.h"


// CDlgSAPrefs18 dialog

IMPLEMENT_DYNAMIC(CDlgSAPrefs18, CSAPrefsSubDlg)

CDlgSAPrefs18::CDlgSAPrefs18(CWnd* pParent /*=NULL*/)
		: CSAPrefsSubDlg(CDlgSAPrefs18::IDD, pParent)
{
}

CDlgSAPrefs18::~CDlgSAPrefs18()
{
}

void CDlgSAPrefs18::DoDataExchange(CDataExchange* pDX)
{
	CSAPrefsSubDlg::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_RADIO_SCRAPE_ALWAYS, _scrape_always_nutton);
	DDX_Control(pDX, IDC_RADIO_SCRAPE_ON_CARDS, _scrape_when_cards_button);
	DDX_Control(pDX, IDC_RADIO_SCRAPE_ON_MYTURN, _scrape_when_my_turn_button);
}

BEGIN_MESSAGE_MAP(CDlgSAPrefs18, CSAPrefsSubDlg)
END_MESSAGE_MAP()

// CDlgSAPrefs1 message handlers
BOOL CDlgSAPrefs18::OnInitDialog()
{
	CSAPrefsSubDlg::OnInitDialog();
	if (Preferences()->lazy_scraping_when_to_scrape() == k_lazy_scraping_myturn)
	{
		_scrape_when_my_turn_button.SetCheck(true);
	}
	else if (Preferences()->lazy_scraping_when_to_scrape() == k_lazy_scraping_cards)
	{
		_scrape_when_cards_button.SetCheck(true);
	}
	else
	{
		// Default: k_lazy_scraping_always
		_scrape_always_nutton.SetCheck(true);
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgSAPrefs18::OnOK()
{

	if (_scrape_when_cards_button.GetCheck())
	{
		Preferences()->SetValue(k_prefs_lazy_scraping_when_to_scrape, k_lazy_scraping_cards);
	}
	else if (_scrape_when_my_turn_button.GetCheck())
	{
		Preferences()->SetValue(k_prefs_lazy_scraping_when_to_scrape, k_lazy_scraping_myturn);
	}
	else
	{
		// Default: k_lazy_scraping_always
		Preferences()->SetValue(k_prefs_lazy_scraping_when_to_scrape, k_lazy_scraping_always);
	}
	CSAPrefsSubDlg::OnOK();
}
