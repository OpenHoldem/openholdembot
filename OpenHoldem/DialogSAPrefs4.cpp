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

// DialogSAPrefs4.cpp : implementation file
//

#include "stdafx.h"
#include <regex>
using namespace std;

#include "SAPrefsSubDlg.h"
#include "DialogSAPrefs4.h"

#include "..\DLLs\WindowFunctions_DLL\window_functions.h"

// CDlgSAPrefs4 dialog

IMPLEMENT_DYNAMIC(CDlgSAPrefs4, CSAPrefsSubDlg)

CDlgSAPrefs4::CDlgSAPrefs4(CWnd* pParent /*=NULL*/)
		: CSAPrefsSubDlg(CDlgSAPrefs4::IDD, pParent)
{
}

CDlgSAPrefs4::~CDlgSAPrefs4()
{
}

void CDlgSAPrefs4::DoDataExchange(CDataExchange* pDX)
{
	CSAPrefsSubDlg::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SCRAPEDELAY, m_ScrapeDelay);
	DDX_Control(pDX, IDC_SCRAPEDELAY_SPIN, m_ScrapeDelay_Spin);
	DDX_Control(pDX, IDC_UNWANTEDSCRAPE, m_UnwantedScrape);
}

BEGIN_MESSAGE_MAP(CDlgSAPrefs4, CSAPrefsSubDlg)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SCRAPEDELAY_SPIN, &CDlgSAPrefs4::OnDeltaposScrapedelaySpin)
END_MESSAGE_MAP()

// CDlgSAPrefs4 message handlers
BOOL CDlgSAPrefs4::OnInitDialog()
{
	CString		delaytext = "";
	CString Separator = _T("|~|");
	int Position = 0;
	CString Token = "null";

	CSAPrefsSubDlg::OnInitDialog();

	delaytext.Format("%d", Preferences()->scrape_delay());
	m_ScrapeDelay.SetWindowText(delaytext);
	m_ScrapeDelay_Spin.SetRange(100, 5000);
	m_ScrapeDelay_Spin.SetPos(Preferences()->scrape_delay());
	m_ScrapeDelay_Spin.SetBuddy(&m_ScrapeDelay);

	while (!Token.IsEmpty())
	{
		// Get next token.
		Token = Preferences()->unwanted_scrape().Tokenize(Separator, Position);
		if (!Token.IsEmpty()) {
			m_UnwantedScrape.AddItem(Token);
		}
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgSAPrefs4::OnOK()
{
	CString		delaytext = "", unwantedtext = "";
	CString		Separator = _T("|~|");
	string		msgTokenNumberLimit = "You have reached the " + to_string(MAX_TOKENNUMBER) + " maximum accepted entries.";
	string		msgTokenSizeLimit = "Entries must not exceed limit of " + to_string(MAX_TOKENSIZE) + " characters.";
	CString		msgInvalidRegexExp = "Invalid Regex expression(s) in your entries.";


	m_ScrapeDelay.GetWindowText(delaytext);
	if (strtoul(delaytext.GetString(), 0, 10)<MIN_SCRAPEDELAY || strtoul(delaytext.GetString(), 0, 10)>MAX_SCRAPEDELAY) {
		MessageBox_Interactive("Invalid Scrape Delay", "ERROR", MB_OK);
		return;
	}
	Preferences()->SetValue(k_prefs_scrape_delay, strtoul(delaytext.GetString(), 0, 10));

	int count = m_UnwantedScrape.GetCount();
	if (count > MAX_TOKENNUMBER) {
		MessageBox_Interactive(msgTokenNumberLimit.c_str(), "Error", MB_OK);
		return;
	}
	for (int i = 0; i < count; i++) {
		if (m_UnwantedScrape.GetItemText(i).GetLength() > MAX_TOKENSIZE) {
			MessageBox_Interactive(msgTokenSizeLimit.c_str(), "Error", MB_OK);
			return;
		}
		if (m_UnwantedScrape.GetItemText(i).Find("\\(" || "\\)" || "\\[" || "\\]" || "\\{" || "\\}")) {
			try {
				regex(m_UnwantedScrape.GetItemText(i));
			}
			catch (exception e) {
				MessageBox_Interactive(msgInvalidRegexExp, "Error", MB_OK);
				return;
			}
		}
		unwantedtext.Append(m_UnwantedScrape.GetItemText(i) + Separator);
	}	
	Preferences()->SetValue(k_prefs_unwanted_scrape, unwantedtext);
	
	CSAPrefsSubDlg::OnOK();
}

void CDlgSAPrefs4::OnDeltaposScrapedelaySpin(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	pNMUpDown->iDelta*=100;
	*pResult = 0;
}
