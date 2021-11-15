#pragma once
//******************************************************************************
//
// This file is part of the OpenHoldem project
//    Source code:           https://github.com/OpenHoldem/openholdembot/
//    Forums:                http://www.maxinmontreal.com/forums/index.php
//    Licensed under GPL v3: http://www.gnu.org/licenses/gpl.html
//
//******************************************************************************
//
// Purpose: preferences dialog for GUI
//
//******************************************************************************

#include "resource.h"
#include "afxwin.h"
#include "SAPrefsDialog.h"

// CDlgSAPrefs15 dialog
class CDlgSAPrefs15 : public CSAPrefsSubDlg {
	DECLARE_DYNAMIC(CDlgSAPrefs15)
 public:
	CDlgSAPrefs15(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgSAPrefs15();
// Dialog Data
	enum { IDD = IDD_SAPREFS15 };
 protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL	OnInitDialog();
	DECLARE_MESSAGE_MAP()
	virtual void OnOK();
 private:
	afx_msg void OnBnClickedDisableMsgbox();
 public:
	CButton		_gui_first_visible;
	CButton		_gui_all_minimized;
	CButton		_gui_less;
	CButton		m_disable_msgbox;
	CButton		m_amounts_in_bb;
};