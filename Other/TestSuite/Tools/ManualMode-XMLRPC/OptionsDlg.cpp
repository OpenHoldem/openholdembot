// OptionsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ManualMode.h"
#include "OptionsDlg.h"


// OptionsDlg dialog

IMPLEMENT_DYNAMIC(OptionsDlg, CDialog)

OptionsDlg::OptionsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(OptionsDlg::IDD, pParent)
	, m_unobstructivePopup(FALSE)
{

}

OptionsDlg::~OptionsDlg()
{
}

void OptionsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHECK1, m_unobstructivePopup);
}


BEGIN_MESSAGE_MAP(OptionsDlg, CDialog)

END_MESSAGE_MAP()


// OptionsDlg message handlers

