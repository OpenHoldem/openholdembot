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

// CEditDlg dialog

class CEditDlg : public CDialog {
	DECLARE_DYNAMIC(CEditDlg)

public:
	CEditDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CEditDlg();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();

	enum { IDD = IDD_EDITDLG };
	CString		m_titletext;
	CString		m_result;
	CEdit		m_EditEntry;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	WINDOWPLACEMENT *pPlacement; 
	DECLARE_MESSAGE_MAP();
};
