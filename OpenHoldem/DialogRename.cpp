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

// RenameDialog.cpp : implementation file
//

#include "stdafx.h"
#include "DialogRename.h"

#include "COHScriptObject.h"
#include "WindowFunctions/window_functions.h"
#include "OpenHoldem.h"
#include "VerifyFunctionAndListNames.h"

// CDlgRename dialog

IMPLEMENT_DYNAMIC(CDlgRename, CDialog)

CDlgRename::CDlgRename(CWnd* pParent /*=NULL*/) : CDialog(CDlgRename::IDD, pParent) 
{
}

CDlgRename::~CDlgRename() 
{
}

void CDlgRename::DoDataExchange(CDataExchange* pDX) 
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_OLDNAME_EDIT, m_OldName);
	DDX_Control(pDX, IDC_NEWNAME_EDIT, m_NewName);
}

BEGIN_MESSAGE_MAP(CDlgRename, CDialog)
	ON_BN_CLICKED(IDOK, &CDlgRename::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CDlgRename::OnBnClickedCancel)
END_MESSAGE_MAP()

// CDlgRename message handlers
BOOL CDlgRename::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_OldName.SetWindowText(CSoldname);
	m_NewName.SetWindowText(CSoldname);

	return TRUE;  // return TRUE unless you set the focus to a control
}

void CDlgRename::OnBnClickedOk() 
{
	char oldstr[MAX_WINDOW_TITLE] = {0}, newstr[MAX_WINDOW_TITLE] = {0};

	m_NewName.GetWindowText(CSnewname);
	strcpy_s(oldstr, MAX_WINDOW_TITLE, CSoldname.GetString());
	strcpy_s(newstr, MAX_WINDOW_TITLE, CSnewname.GetString());

    if (COHScriptObject::IsFunction(oldstr)) {
      if (!VerifyFunctionName(newstr)) return;
    } else {
      if (!VerifyListName(newstr)) return;
    }
	OnOK();
}

void CDlgRename::OnBnClickedCancel() 
{
	OnCancel();
}

