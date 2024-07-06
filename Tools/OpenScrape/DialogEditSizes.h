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


// CDlgEditSizes dialog

class CDlgEditSizes : public CDialog
{
	DECLARE_MESSAGE_MAP()
	DECLARE_DYNAMIC(CDlgEditSizes)

public:
	CDlgEditSizes(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgEditSizes();
	CString						titletext;
	CArray <CString, CString>	strings;
	
	CString						name;
	int							width, height;

// Dialog Data
	enum { IDD = IDD_EDIT_SIZES };

protected:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	CComboBox m_Name;
	CEdit m_Width;
	CEdit m_Height;
};
