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

// DialogSAPrefs6.cpp : implementation file
//

#include "stdafx.h"

#include "CPokerTrackerThread.h"

#include "DialogSAPrefs6.h"
#include "OpenHoldem.h"
#include "MainFrm.h"
#include "WindowFunctions/window_functions.h"
#include "SAPrefsSubDlg.h"


// CDlgSAPrefs6 dialog

IMPLEMENT_DYNAMIC(CDlgSAPrefs6, CSAPrefsSubDlg)

CDlgSAPrefs6::CDlgSAPrefs6(CWnd* pParent /*=NULL*/)
		: CSAPrefsSubDlg(CDlgSAPrefs6::IDD, pParent)
{
}

CDlgSAPrefs6::~CDlgSAPrefs6()
{
}

void CDlgSAPrefs6::DoDataExchange(CDataExchange* pDX)
{
	CSAPrefsSubDlg::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PT_IP, m_pt_ip);
	DDX_Control(pDX, IDC_PT_PORT, m_pt_port);
	DDX_Control(pDX, IDC_PT_USER, m_pt_user);
	DDX_Control(pDX, IDC_PT_PASS, m_pt_pass);
	DDX_Control(pDX, IDC_PT_DBNAME, m_pt_dbname);
}

BEGIN_MESSAGE_MAP(CDlgSAPrefs6, CSAPrefsSubDlg)
	ON_BN_CLICKED(IDC_PT_TEST, &CDlgSAPrefs6::OnBnClickedPtTest)
END_MESSAGE_MAP()

// CDlgSAPrefs6 message handlers
BOOL CDlgSAPrefs6::OnInitDialog()
{
	CString		text = "";

	CSAPrefsSubDlg::OnInitDialog();

	m_pt_ip.SetWindowText(Preferences()->pt_ip_addr());
	m_pt_port.SetWindowText(Preferences()->pt_port());
	m_pt_user.SetWindowText(Preferences()->pt_user());
	m_pt_pass.SetWindowText(Preferences()->pt_pass());
	m_pt_dbname.SetWindowText(Preferences()->pt_dbname());

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgSAPrefs6::OnOK()
{
	CString			text = "";

	m_pt_ip.GetWindowText(text);
	Preferences()->SetValue(k_prefs_pt_ip_addr, text);

	m_pt_port.GetWindowText(text);
	Preferences()->SetValue(k_prefs_pt_port, text);

	m_pt_user.GetWindowText(text);
	Preferences()->SetValue(k_prefs_pt_user, text);

	m_pt_pass.GetWindowText(text);
	Preferences()->SetValue(k_prefs_pt_pass, text);

	m_pt_dbname.GetWindowText(text);
	Preferences()->SetValue(k_prefs_pt_dbname, text);

	CSAPrefsSubDlg::OnOK();
}

void CDlgSAPrefs6::OnBnClickedPtTest() {
	CString			conn_str = "", ip_addr = "", port = "", user = "", pass = "", dbname = "", e = "";

	m_pt_ip.GetWindowText(ip_addr);
	m_pt_port.GetWindowText(port);
	m_pt_user.GetWindowText(user);
	m_pt_pass.GetWindowText(pass);
	m_pt_dbname.GetWindowText(dbname);

	if (dbname.IsEmpty()) {
		// http://www.postgresql.org/docs/8.1/static/libpq.html
		write_log(Preferences()->debug_pokertracker(), "[PokerTracker] Test: PostgreSQL DB Name not set ! Default : It will be set to '%s'\n", user);
		MessageBox_Interactive("PostgreSQL DB Name not set !\nBy Default : the same as the Username",
					   "Warning", MB_OK);
	}

	conn_str = p_pokertracker_thread->CreateConnectionString(ip_addr, 
		port, user, pass, dbname);

	// Set busy cursor
	PMainframe()->set_wait_cursor(true);
	PMainframe()->BeginWaitCursor();

	// Test the connection parameters
	PGconn	*pgconn = PQconnectdb(conn_str.GetString());

	// Unset busy cursor
	PMainframe()->EndWaitCursor();
	PMainframe()->set_wait_cursor(false);

	if (PQstatus(pgconn) == CONNECTION_OK) {
		write_log(Preferences()->debug_pokertracker(), "[PokerTracker] Test: PostgreSQL DB opened successfully <%s/%s/%s>\n", ip_addr, port, dbname);
		if (PQisthreadsafe()) {
			write_log(Preferences()->debug_pokertracker(), "[PokerTracker] Test: PostgreSQL library is thread safe.\n\n");
			MessageBox_Interactive("PostgreSQL DB opened successfully", "Success", MB_OK);
		} else {
			write_log(Preferences()->debug_pokertracker(), "[PokerTracker] Test: PostgreSQL library is *NOT* thread safe!  This is a problem!\n\n");
			MessageBox_Interactive("PostgreSQL DB opened successfully, but\nPostgreSQL library is *NOT* thread safe!\nThis is a problem!",
					   "Success (partial)", MB_OK);
		}
		PQfinish(pgconn);
	} else {
		write_log(Preferences()->debug_pokertracker(), "[PokerTracker] Test: ERROR opening PostgreSQL DB: %s\n\n", PQerrorMessage(pgconn));
		e = "ERROR opening PostgreSQL DB:\n";
		e += PQerrorMessage(pgconn);
		e += "\nConn string:";
		e += conn_str;

		MessageBox_Interactive(e.GetString(), "ERROR", MB_OK);

		PQfinish(pgconn);
	}
}
