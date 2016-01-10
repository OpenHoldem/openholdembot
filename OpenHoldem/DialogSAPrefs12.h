//*******************************************************************************
//
// This file is part of the OpenHoldem project
//   Download page:         http://code.google.com/p/openholdembot/
//   Forums:                http://www.maxinmontreal.com/forums/index.php
//   Licensed under GPL v3: http://www.gnu.org/licenses/gpl.html
//
//*******************************************************************************
//
// Purpose:
//
//*******************************************************************************

#ifndef INC_DIALOGSAPREFS12_H
#define INC_DIALOGSAPREFS12_H

#include "resource.h"
#include "afxwin.h"

#include "SAPrefsDialog.h"

// CDlgSAPrefs12 dialog

class CDlgSAPrefs12 : public CSAPrefsSubDlg
{
	DECLARE_DYNAMIC(CDlgSAPrefs12)

public:
	CDlgSAPrefs12(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgSAPrefs12();

// Dialog Data
	enum { IDD = IDD_SAPREFS12 };
	CEdit	_excluded_tests_edit;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	virtual BOOL OnInitDialog();
	virtual void OnOK();   
};

#endif