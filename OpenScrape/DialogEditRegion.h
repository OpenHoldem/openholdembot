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
#include "afxwin.h"
#include "resource.h"

// CDlgEditRegion dialog

class CDlgEditRegion : public CDialog
{
	DECLARE_DYNAMIC(CDlgEditRegion)

public:
	CDlgEditRegion(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgEditRegion();

	CString						titletext, labeltext;
	CArray <CString, CString>	strings;
	
	CString						name;

// Dialog Data
	enum { IDD = IDD_EDIT_REGION };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CComboBox m_Name;
	CStatic m_Label;
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
};
