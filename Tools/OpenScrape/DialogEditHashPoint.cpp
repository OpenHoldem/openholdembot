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


// DialogEditHashPoint.cpp : implementation file
//

#include "stdafx.h"
#include "DialogEditHashPoint.h"

// CDlgEditHashPoint dialog

IMPLEMENT_DYNAMIC(CDlgEditHashPoint, CDialog)

CDlgEditHashPoint::CDlgEditHashPoint(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgEditHashPoint::IDD, pParent)
{
}

CDlgEditHashPoint::~CDlgEditHashPoint()
{
}

void CDlgEditHashPoint::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TYPE, m_Type);
	DDX_Control(pDX, IDC_X, m_X);
	DDX_Control(pDX, IDC_Y, m_Y);
}


BEGIN_MESSAGE_MAP(CDlgEditHashPoint, CDialog)
	ON_BN_CLICKED(IDOK, &CDlgEditHashPoint::OnBnClickedOk)
END_MESSAGE_MAP()


// CDlgEditHashPoint message handlers

BOOL CDlgEditHashPoint::OnInitDialog()
{
	CDialog::OnInitDialog();

	CString		text;

	CDialog::OnInitDialog();

	SetWindowText(titletext.GetString());

	m_Type.AddString("Type 0");
	m_Type.AddString("Type 1");
	m_Type.AddString("Type 2");
	m_Type.AddString("Type 3");
	m_Type.SelectString(-1, type.GetString());
	m_Type.SetWindowPos(NULL, 0, 0, 75, 300, SWP_NOMOVE | SWP_NOZORDER);

	text.Format("%d", x);
	m_X.SetWindowText(text.GetString());

	text.Format("%d", y);
	m_Y.SetWindowText(text.GetString());

	m_Type.SetFocus();


	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgEditHashPoint::OnBnClickedOk()
{
	CString		text;

	m_Type.GetWindowText(type);

	m_X.GetWindowText(text);
	x = atoi(text.GetString());

	m_Y.GetWindowText(text);
	y = atoi(text.GetString());

	OnOK();
}
