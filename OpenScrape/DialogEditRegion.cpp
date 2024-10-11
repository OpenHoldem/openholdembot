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


// DialogEditRegion.cpp : implementation file
//

#include "stdafx.h"
#include "DialogEditRegion.h"

// CDlgEditRegion dialog

IMPLEMENT_DYNAMIC(CDlgEditRegion, CDialog)

CDlgEditRegion::CDlgEditRegion(CWnd* pParent /*=NULL*/) : CDialog(CDlgEditRegion::IDD, pParent)
{
}

CDlgEditRegion::~CDlgEditRegion()
{
}

void CDlgEditRegion::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_NAME, m_Name);
	DDX_Control(pDX, IDC_LABEL, m_Label);
}


BEGIN_MESSAGE_MAP(CDlgEditRegion, CDialog)
	ON_BN_CLICKED(IDOK, &CDlgEditRegion::OnBnClickedOk)
END_MESSAGE_MAP()


// CDlgEditRegion message handlers

BOOL CDlgEditRegion::OnInitDialog()
{
	CString		text;
	CDialog::OnInitDialog();

	SetWindowText(titletext.GetString());
	m_Label.SetWindowTextA(labeltext.GetString());

	for (int i=0; i<strings.GetSize(); i++)  m_Name.AddString(strings[i].GetString());
	m_Name.SelectString(-1, name.GetString());
	m_Name.SetWindowPos(NULL, 0, 0, 145, 300, SWP_NOMOVE | SWP_NOZORDER);

	m_Name.SetFocus();

	return FALSE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgEditRegion::OnBnClickedOk()
{
	m_Name.GetWindowText(name);

	OnOK();
}

