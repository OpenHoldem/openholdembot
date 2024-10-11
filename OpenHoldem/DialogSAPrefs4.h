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

#ifndef INC_DIALOGSAPREFS4_H
#define INC_DIALOGSAPREFS4_H

#include "resource.h"
#include "afxvslistbox.h"

#include "SAPrefsDialog.h"

// CDlgSAPrefs4 dialog

class CDlgSAPrefs4 : public CSAPrefsSubDlg
{
	DECLARE_DYNAMIC(CDlgSAPrefs4)

public:
	CDlgSAPrefs4(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgSAPrefs4();

// Dialog Data
	enum { IDD = IDD_SAPREFS4 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_ScrapeDelay;
	CVSListBox m_UnwantedScrape;
	CSpinButtonCtrl m_ScrapeDelay_Spin;
	virtual BOOL OnInitDialog();

protected:
	virtual void OnOK();

public:
	afx_msg void OnDeltaposScrapedelaySpin(NMHDR *pNMHDR, LRESULT *pResult);
};


#endif //INC_DIALOGSAPREFS4_H