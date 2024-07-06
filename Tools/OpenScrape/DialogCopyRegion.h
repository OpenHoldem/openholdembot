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

// CDlgCopyRegion dialog

class CDlgCopyRegion : public CDialog
{
	DECLARE_DYNAMIC(CDlgCopyRegion)

public:
	CDlgCopyRegion(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgCopyRegion();

	CString						source_region;
	CArray <CString, CString>	candidates;
	CArray <CString, CString>	selected;

// Dialog Data
	enum { IDD = IDD_COPY_REGION };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CStatic m_Source;

	virtual BOOL OnInitDialog();
	CListBox m_RegionList;
	afx_msg void OnBnClickedOk();
};
