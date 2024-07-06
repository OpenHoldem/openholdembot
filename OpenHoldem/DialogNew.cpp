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

// NewDialog.cpp : implementation file
//

#include "stdafx.h"
#include "DialogNew.h"

#include "resource.h"
#include "WindowFunctions/window_functions.h"
#include "VerifyFunctionAndListNames.h"

// CDlgNew dialog

IMPLEMENT_DYNAMIC(CDlgNew, CDialog)

CDlgNew::CDlgNew(CWnd* pParent /*=NULL*/) : CDialog(CDlgNew::IDD, pParent) 
{
}

CDlgNew::~CDlgNew() 
{
}

void CDlgNew::DoDataExchange(CDataExchange* pDX) 
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_NEWNAME_EDIT, m_NewName);
	DDX_Control(pDX, IDC_DESC, m_Desc);
}

BEGIN_MESSAGE_MAP(CDlgNew, CDialog)
	ON_BN_CLICKED(IDOK, &CDlgNew::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CDlgNew::OnBnClickedCancel)
END_MESSAGE_MAP()

// CDlgNew message handlers
BOOL CDlgNew::OnInitDialog() 
{
	CDialog::OnInitDialog();

	if (is_function == false) 
	{
		m_NewName.SetWindowText("list");
		m_Desc.SetWindowText("New list name:");
		::SetWindowText(m_hWnd, "New List");
	}
	else 
	{
		m_NewName.SetWindowText("f$");
		m_Desc.SetWindowText("New UDF name:");
		::SetWindowText(m_hWnd, "New UDF");
	}

	return TRUE;  // return TRUE unless you set the focus to a control
}

void CDlgNew::OnBnClickedOk() 
{
	char str[MAX_WINDOW_TITLE] = {0};

    m_NewName.GetWindowText(CSnewname);
	strcpy_s(str, MAX_WINDOW_TITLE, CSnewname.GetString());
    if (is_function) {
      if (!VerifyFunctionName(str)) return;
    } else {
      if (!VerifyListName(str)) return;
    }
	OnOK();
}

void CDlgNew::OnBnClickedCancel() 
{
	OnCancel();
}

