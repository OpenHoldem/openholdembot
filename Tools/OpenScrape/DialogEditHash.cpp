// DialogEditHash.cpp : implementation file
//

#include "stdafx.h"
#include "DialogEditHash.h"

// CDlgEditHash dialog

IMPLEMENT_DYNAMIC(CDlgEditHash, CDialog)

CDlgEditHash::CDlgEditHash(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgEditHash::IDD, pParent)
{
	__SEH_SET_EXCEPTION_HANDLER
}

CDlgEditHash::~CDlgEditHash()
{
}

void CDlgEditHash::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TYPE, m_Type);
}


BEGIN_MESSAGE_MAP(CDlgEditHash, CDialog)
	ON_BN_CLICKED(IDOK, &CDlgEditHash::OnBnClickedOk)
END_MESSAGE_MAP()


// CDlgEditHash message handlers

BOOL CDlgEditHash::OnInitDialog()
{
	int			i;

	CDialog::OnInitDialog();

	SetWindowText(titletext.GetString());

	for (i=0; i<strings.GetSize(); i++)  m_Type.AddString(strings[i].GetString());
	m_Type.SelectString(-1, type.GetString());
	m_Type.SetWindowPos(NULL, 0, 0, 75, 300, SWP_NOMOVE | SWP_NOZORDER);

	m_Type.SetFocus();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgEditHash::OnBnClickedOk()
{
	m_Type.GetLBText(m_Type.GetCurSel(), type);

	OnOK();
}
