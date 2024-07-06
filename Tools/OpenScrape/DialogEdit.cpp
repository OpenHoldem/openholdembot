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


// EditDlg.cpp : implementation file
//

#include "stdafx.h"
#include "DialogEdit.h"

// CDlgEdit dialog
IMPLEMENT_DYNAMIC(CDlgEdit, CDialog)

CDlgEdit::CDlgEdit(CWnd* pParent /*=NULL*/)	: CDialog(CDlgEdit::IDD, pParent) 
{}

CDlgEdit::~CDlgEdit() 
{}

void CDlgEdit::DoDataExchange(CDataExchange* pDX) 
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDITENTRY, m_EditEntry);
}


BEGIN_MESSAGE_MAP(CDlgEdit, CDialog)
	ON_BN_CLICKED(IDOK, &CDlgEdit::OnBnClickedOk)
END_MESSAGE_MAP()


// CDlgEdit message handlers
BOOL CDlgEdit::OnInitDialog() 
{
	CDialog::OnInitDialog();

	SetWindowText(m_titletext.GetString());
	m_EditEntry.SetWindowText(m_result.GetString());
	m_EditEntry.SetSel(MAKEWORD(0,-1));
	m_EditEntry.SetFocus();

	return FALSE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgEdit::OnBnClickedOk() 
{
	m_EditEntry.GetWindowText(m_result);
	OnOK();
}
