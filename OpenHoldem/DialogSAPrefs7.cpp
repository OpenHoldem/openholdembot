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

// DialogSAPrefs7.cpp : implementation file
//

#include "pch.h"

#include "SAPrefsSubDlg.h"
#include "DialogSAPrefs7.h"



// CDlgSAPrefs7 dialog

IMPLEMENT_DYNAMIC(CDlgSAPrefs7, CDialog)

CDlgSAPrefs7::CDlgSAPrefs7(CWnd* pParent /*=NULL*/)
		: CSAPrefsSubDlg(CDlgSAPrefs7::IDD, pParent)
{
}

CDlgSAPrefs7::~CDlgSAPrefs7()
{
}

void CDlgSAPrefs7::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ICM_1, m_ICM1);
	DDX_Control(pDX, IDC_ICM_2, m_ICM2);
	DDX_Control(pDX, IDC_ICM_3, m_ICM3);
	DDX_Control(pDX, IDC_ICM_4, m_ICM4);
	DDX_Control(pDX, IDC_ICM_5, m_ICM5);
  DDX_Control(pDX, IDC_ICM_6, m_ICM6);
  DDX_Control(pDX, IDC_ICM_7, m_ICM7);
  DDX_Control(pDX, IDC_ICM_8, m_ICM8);
  DDX_Control(pDX, IDC_ICM_9, m_ICM9);
}

BEGIN_MESSAGE_MAP(CDlgSAPrefs7, CDialog)
END_MESSAGE_MAP()

// CDlgSAPrefs7 message handlers
BOOL CDlgSAPrefs7::OnInitDialog()
{
  return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgSAPrefs7::OnOK()
{
	CSAPrefsSubDlg::OnOK();
}
