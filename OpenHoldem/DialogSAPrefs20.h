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

#ifndef INC_DIALOGSAPREFS20_H
#define INC_DIALOGSAPREFS20_H

#include "resource.h"
#include "afxwin.h"

#include "SAPrefsDialog.h"

// CDlgSAPrefs20 dialog

class CDlgSAPrefs20 : public CSAPrefsSubDlg
{
	DECLARE_DYNAMIC(CDlgSAPrefs20)

public:
	CDlgSAPrefs20(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgSAPrefs20();

// Dialog Data
	enum { IDD = IDD_SAPREFS20 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	DECLARE_MESSAGE_MAP()
	virtual BOOL OnInitDialog();
	virtual void OnOK();
protected:
  afx_msg void OnBnClickedToggleAllDebug();
private:
  void AlignbuttonsInAlphabeticalOrder();
  void AlignButton(int button_ID);
  void ToggleButton(int button_ID);
private:
  int _buttons_aligned;
};

#endif