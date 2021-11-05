#pragma once

const int kNbOfActions = 25;

// CDlgSelectActions dialog

class CDlgSelectActions : public CDialog
{
	DECLARE_DYNAMIC(CDlgSelectActions)

public:
	CDlgSelectActions(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CDlgSelectActions();
	virtual BOOL OnInitDialog();

// Dialog Data
//#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SELECTACTIONS };
//#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedSelectAll();
	afx_msg void OnBnClickedClear();

	DECLARE_MESSAGE_MAP()
public:
	CButton m_Check[kNbOfActions+1];
	CStringArray arrActionStrings;
};
