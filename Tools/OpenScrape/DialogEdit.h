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

// CDlgEdit dialog
class CDlgEdit : public CDialog 
{
	DECLARE_DYNAMIC(CDlgEdit)

public:
	CDlgEdit(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgEdit();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();

	enum { IDD = IDD_EDITDLG };
	CString		m_titletext;
  CString   m_titletext_preprocessed;
	CString		m_result;
	CEdit		  m_EditEntry;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};
