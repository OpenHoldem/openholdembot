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

// DialogSAPrefs3.cpp : implementation file
//

#include "pch.h"

#include "SAPrefsSubDlg.h"
#include "DialogSAPrefs3.h"


// CDlgSAPrefs3 dialog

IMPLEMENT_DYNAMIC(CDlgSAPrefs3, CSAPrefsSubDlg)

CDlgSAPrefs3::CDlgSAPrefs3(CWnd* pParent /*=NULL*/)
		: CSAPrefsSubDlg(CDlgSAPrefs3::IDD, pParent)
{
}

CDlgSAPrefs3::~CDlgSAPrefs3()
{
}

void CDlgSAPrefs3::DoDataExchange(CDataExchange* pDX)
{
	CSAPrefsSubDlg::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CDlgSAPrefs3, CSAPrefsSubDlg)
END_MESSAGE_MAP()

// CDlgSAPrefs3 message handlers
BOOL CDlgSAPrefs3::OnInitDialog()
{
	CSAPrefsSubDlg::OnInitDialog();
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgSAPrefs3::OnOK()
{
	CSAPrefsSubDlg::OnOK();
}
