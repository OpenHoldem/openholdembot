/*********************************************************************

   SAPrefsSubDlg

   Copyright (C) 2002 Smaller Animals Software, Inc.

   This software is provided 'as-is', without any express or implied
   warranty.  In no event will the authors be held liable for any damages
   arising from the use of this software.

   Permission is granted to anyone to use this software for any purpose,
   including commercial applications, and to alter it and redistribute it
   freely, subject to the following restrictions:

   1. The origin of this software must not be misrepresented; you must not
	 claim that you wrote the original software. If you use this software
	 in a product, an acknowledgment in the product documentation would be
	 appreciated but is not required.

   2. Altered source versions must be plainly marked as such, and must not be
	 misrepresented as being the original software.

   3. This notice may not be removed or altered from any source distribution.

   http://www.smalleranimals.com
   smallest@smalleranimals.com

**********************************************************************/

// SAPrefsSubDlg.cpp: implementation of the CSAPrefsSubDlg class.
//
//////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "SAPrefsSubDlg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CSAPrefsSubDlg, CDialog)

CSAPrefsSubDlg::CSAPrefsSubDlg()
{
	ASSERT(0);
	// don't use this constructor!
}

//////////////////////////////////////////////////////////////////////

CSAPrefsSubDlg::CSAPrefsSubDlg(UINT nID, CWnd *pParent /*=NULL*/)
		: CDialog(nID)
{
	m_id = nID;
}

//////////////////////////////////////////////////////////////////////

CSAPrefsSubDlg::~CSAPrefsSubDlg()
{
}

//////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CSAPrefsSubDlg, CDialog)
	//{{AFX_MSG_MAP(CSAPrefsSubDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////

void CSAPrefsSubDlg::OnOK()
{
	EndDialog(IDOK);
}

//////////////////////////////////////////////////////////////////////

void CSAPrefsSubDlg::OnCancel()
{
	EndDialog(IDCANCEL);
}

//////////////////////////////////////////////////////////////////////

BOOL CSAPrefsSubDlg::PreTranslateMessage(MSG* pMsg)
{
	// Don't let CDialog process the Escape key.
	if ((pMsg->message == WM_KEYDOWN) && (pMsg->wParam == VK_ESCAPE))
	{
		return TRUE;
	}

	// Don't let CDialog process the Return key, if a multi-line edit has focus
	if ((pMsg->message == WM_KEYDOWN) && (pMsg->wParam == VK_RETURN))
	{
		// Special case: if control with focus is an edit control with
		// ES_WANTRETURN style, let it handle the Return key.

		TCHAR szClass[10];
		CWnd* pWndFocus = GetFocus();
		if (((pWndFocus = GetFocus()) != NULL) &&
				IsChild(pWndFocus) &&
				(pWndFocus->GetStyle() & ES_WANTRETURN) &&
				GetClassName(pWndFocus->m_hWnd, szClass, 10) &&
				(lstrcmpi(szClass, _T("EDIT")) == 0))
		{
			pWndFocus->SendMessage(WM_CHAR, pMsg->wParam, pMsg->lParam);
			return TRUE;
		}

		return FALSE;
	}

	return CDialog::PreTranslateMessage(pMsg);
}

//////////////////////////////////////////////////////////////////////
