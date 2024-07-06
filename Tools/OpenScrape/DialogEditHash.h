#pragma once
#include "afxwin.h"
#include "resource.h"

// CDlgEditHash dialog

class CDlgEditHash : public CDialog
{
	DECLARE_MESSAGE_MAP()
	DECLARE_DYNAMIC(CDlgEditHash)

public:
	CDlgEditHash(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgEditHash();

	CString						titletext;
	CArray <CString, CString>	strings;
	CString						type;

// Dialog Data
	enum { IDD = IDD_EDIT_HASH };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	CComboBox m_Type;
};
