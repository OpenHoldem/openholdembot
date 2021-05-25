// WaitDialog.cpp : implementation file
//

#include "stdafx.h"
#include "OpenScrape.h"
#include "WaitDialog.h"
#include "WaitRsc.h"
#include "afxdialogex.h"


// CWaitDialog dialog

IMPLEMENT_DYNAMIC(CWaitDialog, CDialog)

CWaitDialog::CWaitDialog(BOOL* pFlag, LPCTSTR pszCaption, LPCTSTR pszText) : CDialog()
{
	m_pFlag = pFlag;

	// Disable the main window and create the dialog.
	AfxGetMainWnd()->EnableWindow(FALSE);
	Create(IDD_WAITDIALOG);

	// Initialize the dialog caption and the static text control.
	SetWindowText((pszCaption == NULL) ? "Working" : pszCaption);

	CStatic* pCtrl = (CStatic*)GetDlgItem(IDC_MSGCTRL);
	pCtrl->SetWindowText((pszText == NULL) ? "Click Cancel to cancel " \
		"the operation" : pszText);

	// Display the dialog.
	ShowWindow(SW_SHOW);
}

CWaitDialog::~CWaitDialog()
{
	Close();
}

void CWaitDialog::OnCancel()
{
	*m_pFlag = FALSE;
	Close();
}

BOOL CWaitDialog::Pump()
{
	MSG msg;

	// Retrieve and dispatch any waiting messages.
	while (::PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE)) {
		if (!AfxGetApp()->PumpMessage()) {
			::PostQuitMessage(0);
			return FALSE;
		}
	}

	// Simulate the framework's idle processing mechanism.
	LONG lIdle = 0;
	while (AfxGetApp()->OnIdle(lIdle++));
	return TRUE;
}

void CWaitDialog::SetPercentComplete(int nPercent)
{
	if (::IsWindow(m_hWnd)) {
		if (nPercent > 100)
			nPercent = 100;

		CProgressCtrl* pCtrl = (CProgressCtrl*)GetDlgItem(IDC_PROGRESSCTRL);
		pCtrl->SetPos(nPercent);
	}
}

void CWaitDialog::SetPercentCompleteAndPump(int nPercent)
{
	SetPercentComplete(nPercent);
	Pump();
}

void CWaitDialog::SetMessageText(LPCTSTR pszText)
{
	if (::IsWindow(m_hWnd)) {
		CStatic* pCtrl = (CStatic*)GetDlgItem(IDC_MSGCTRL);
		pCtrl->SetWindowText(pszText);
	}
}

void CWaitDialog::Close()
{
	if (::IsWindow(m_hWnd)) {
		AfxGetMainWnd()->EnableWindow(TRUE);
		DestroyWindow();
	}
}

bool CWaitDialog::Cancel() const
{
	return (*m_pFlag == FALSE);
}

void CWaitDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CWaitDialog, CDialog)
END_MESSAGE_MAP()


// CWaitDialog message handlers
