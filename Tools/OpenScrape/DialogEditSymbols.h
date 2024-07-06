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


// CDlgEditSymbols dialog

class CDlgEditSymbols : public CDialog
{
	DECLARE_MESSAGE_MAP()
	DECLARE_DYNAMIC(CDlgEditSymbols)

public:
	CDlgEditSymbols(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgEditSymbols();

	CString						titletext;
	CArray <CString, CString>	strings;
	CString						titlebartext, name, value;

// Dialog Data
	enum { IDD = IDD_EDIT_SYMBOLS };

protected:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedParsebutton();
	afx_msg void OnCbnSelchangeName();
	afx_msg void OnEnKillfocusValue();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	void SetDefaultValues();

	CComboBox m_Name, m_Value;
	CEdit m_Titletext;
  CEdit m_Titletext_preprocessed;
	CEdit m_ParseResults;
	CButton m_ParseButton;
};
