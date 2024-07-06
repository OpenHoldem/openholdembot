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
#include "EditDlg.h"


// CEditDlg dialog

IMPLEMENT_DYNAMIC(CEditDlg, CDialog)

CEditDlg::CEditDlg(CWnd* pParent /*=NULL*/)	: CDialog(CEditDlg::IDD, pParent) {
	pPlacement = NULL;
}

CEditDlg::~CEditDlg() {
	if(pPlacement != NULL)
		free((WINDOWPLACEMENT *) pPlacement);
}

void CEditDlg::DoDataExchange(CDataExchange* pDX) {
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDITENTRY, m_EditEntry);
}


BEGIN_MESSAGE_MAP(CEditDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CEditDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CEditDlg::OnBnClickedCancel)
END_MESSAGE_MAP()


// CEditDlg message handlers

BOOL CEditDlg::OnInitDialog() {
	CDialog::OnInitDialog();

	SetWindowText(m_titletext);
	m_EditEntry.SetWindowText(m_result);
	m_EditEntry.SetSel(MAKEWORD(0,-1));
	m_EditEntry.SetFocus();
	if (pPlacement != NULL) SetWindowPlacement(pPlacement);

	return FALSE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CEditDlg::OnBnClickedOk() {
	 m_EditEntry.GetWindowText(m_result);

	if (pPlacement == NULL) 
		pPlacement = (WINDOWPLACEMENT*) malloc(sizeof(WINDOWPLACEMENT));
	GetWindowPlacement(pPlacement);

	OnOK();
}

void CEditDlg::OnBnClickedCancel()
{
	if (pPlacement == NULL) 
		pPlacement = (WINDOWPLACEMENT*) malloc(sizeof(WINDOWPLACEMENT));
	GetWindowPlacement(pPlacement);

	OnCancel();
}
