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

#pragma once
#include "resource.h"

// CDlgNew dialog

class CDlgNew : public CDialog
{
	DECLARE_DYNAMIC(CDlgNew)

public:
	CDlgNew(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgNew();

// Dialog Data
	enum { IDD = IDD_NEW };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	CEdit m_NewName;
	CString CSnewname;
	bool is_function; // otherwise list
	virtual BOOL OnInitDialog();
	CStatic m_Desc;
};
