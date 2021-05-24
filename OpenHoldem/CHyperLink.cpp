// CHyperLink.cpp : implementation file
//

#include "stdafx.h"
#include "CHyperLink.h"

#include"OpenHoldem.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif




/////////////////////////////////////////////////////////////////////////////
// CHyperLink

CHyperLink::CHyperLink()
{
	m_sLinkColor = RGB(0, 0 ,255);
	m_sHoverColor = RGB(255, 0, 0);
	m_sVisitedColor = RGB(5, 34, 143);

	m_bFire = false;
	m_bFireChild = false;
	m_bMouseOver = false;
	m_bEnableToolTip = false;
	m_bVisited =  false;
	
	//Create Tooltip
}

CHyperLink::~CHyperLink()
{
}


BEGIN_MESSAGE_MAP(CHyperLink, CStatic)
	//{{AFX_MSG_MAP(CMyHyperLink)
	ON_WM_MOUSEMOVE()
	ON_WM_SETCURSOR()
	ON_CONTROL_REFLECT(BN_CLICKED, OnClicked)


	ON_WM_CTLCOLOR_REFLECT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHyperLink message handlers

//Sets the Link Color
void CHyperLink::SetLinkColor(COLORREF sLinkColor)
{
	m_sLinkColor = sLinkColor;

}

//open the URL by Windows ShellExecute()
bool CHyperLink::GoToLinkUrl(CString csLink)
{

	HINSTANCE hInstance = (HINSTANCE)ShellExecute(NULL, _T("open"), csLink.operator LPCTSTR(), NULL, NULL, 2);

	if ((UINT)hInstance < HINSTANCE_ERROR){
		return false;
	}else
		return true;
}

//User can Active/Inactive the Tooltip already they set 
void CHyperLink::ActiveToolTip(int nFlag)
{
	if (nFlag)
		m_bEnableToolTip = true;
	else
		m_bEnableToolTip = false;
}

//change The Tooltip text
void CHyperLink::SetTootTipText(LPCSTR szToolTip)
{
	if (m_bEnableToolTip )
	{
		m_ToolTip.UpdateTipText(szToolTip,this,1001);
	}

}

//The Mouse Move Message
void CHyperLink::OnMouseMove(UINT nFlags, CPoint point) 
{
	CStatic::OnMouseMove(nFlags, point);
	if (m_bMouseOver)
	{
		CRect oRect;
		GetClientRect(&oRect);

		//check if the mouse is in the rect
		if (oRect.PtInRect(point) == false)
		{
			m_bMouseOver = false;
			//Release the Mouse capture previously take
			ReleaseCapture();
			RedrawWindow();
			return;
		}
	}else
	{
		m_bMouseOver = true;
		RedrawWindow();
		//capture the mouse
		SetCapture();
	}
}

//before Subclassing 
void CHyperLink::PreSubclassWindow() 
{

	//Enable the Static to send the Window Messages To its parent
	DWORD dwStyle = GetStyle();
	SetWindowLong(GetSafeHwnd() ,GWL_STYLE ,dwStyle | SS_NOTIFY);

	char szCurretText[MAX_PATH];
	GetWindowText(szCurretText, MAX_PATH);
	if ((szCurretText) == NULL){
		SetWindowText(m_csLinkText.operator LPCTSTR());
	}
	
	LOGFONT sLogFont;
	//GetFont()->GetLogFont(&sLogFont);
	sLogFont.lfCharSet = DEFAULT_CHARSET;
	strcpy(sLogFont.lfFaceName, "Arial");
	sLogFont.lfHeight = -14;
	sLogFont.lfWeight = FW_SEMIBOLD;
	sLogFont.lfEscapement = 0;
	//Set the Link UnderLined
	sLogFont.lfUnderline = true;
	sLogFont.lfStrikeOut = false;
	sLogFont.lfItalic = false;
	//Set the Font to  the Control
//	m_oTextFont.CreateFontIndirect(&sLogFont);
	m_oTextFont.CreateFont(16, 7, 0, 0, FW_SEMIBOLD, FALSE, TRUE, FALSE,
		DEFAULT_CHARSET, OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS, PROOF_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Arial");
	this->SetFont(&m_oTextFont, true);
	
	//Adjust the window
	//IsValidURL();

	//Set the Cursor Hand
	//WinHlp32.exe in windows folder ResourceID 106
	//is a standard window HAND cursor 
	//courtesy www.codeguru.com
	//you can use a custom Hand cursor resourse also
	// i added that  as a resourse in this project with 
	// ID - IDC_CURSOR_HAND

//	char szWindowsDir[MAX_PATH*2];
//	GetWindowsDirectory(szWindowsDir ,MAX_PATH*2);
//	strcat(szWindowsDir,"\\Winhlp32.exe");
//	HMODULE hModule = LoadLibrary(szWindowsDir);
	
//		if (hModule){
		m_hHyperCursor = AfxGetApp()->LoadStandardCursor(IDC_HAND);
		//m_hHyperCursor = ::LoadCursor(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDC_CURSOR_HAND));
		//m_hHyperCursor = ::LoadCursorFromFile(".\\res\\idc_cursor_hand.cur");
//	}

//	this->SetCursor(m_hHyperCursor);

	//free the module
//	if (hModule)
//		FreeLibrary(hModule);

	CStatic::PreSubclassWindow();
	this->SetCursor(m_hHyperCursor);
	
//	m_ToolTip.Create(this,TTS_ALWAYSTIP);
//	CRect oRect;
//	GetClientRect(&oRect);
//	m_ToolTip.AddTool(this,"",oRect,1001);
//	m_ToolTip.ShowWindow(SW_HIDE);
}

void CHyperLink::SetLinkText(CString csLinkText, BOOL display)
{
	m_csLinkText = csLinkText;
	if (display)
		this->SetWindowText(csLinkText.operator LPCTSTR());
	
}

BOOL CHyperLink::PreTranslateMessage(MSG* pMsg) 
{
//	m_ToolTip.RelayEvent(pMsg);
	return CStatic::PreTranslateMessage(pMsg);
}


BOOL CHyperLink::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{

	::SetCursor(m_hHyperCursor);
	return true;
	//return CStatic::OnSetCursor(pWnd, nHitTest, message);
}

//////////////////EVENT WILL GET HERE //////////////////////


void CHyperLink::OnClicked() 
{
	if (m_bFire) {
		Fire();
	}
	else if (m_bFireChild){
		//Fire the Event to Parent Window
		CWnd *pParent;
		pParent = GetParent();
		int nCtrlID = GetDlgCtrlID();
		::SendMessage(pParent->m_hWnd, _HYPERLINK_EVENT, (WPARAM)nCtrlID, 0);
		//::PostMessage(pParent->m_hWnd, __EVENT_ID_, (WPARAM)nCtrlID, 0);

	} else
	{
		GoToLinkUrl(m_csUrl);
	}

	m_bVisited = true;
	//redraw the control 
	this->Invalidate(true);
}

HBRUSH CHyperLink::CtlColor(CDC* pDC, UINT nCtlColor) 
{
	if (m_bMouseOver){
		if (m_bVisited)
			pDC->SetTextColor(m_sVisitedColor);
		else
			pDC->SetTextColor(m_sHoverColor);
	}else {
		if (m_bVisited)
			pDC->SetTextColor(m_sVisitedColor);
		else
			pDC->SetTextColor(m_sLinkColor);
	}
	pDC->SetBkMode(TRANSPARENT);
	return((HBRUSH)GetStockObject(NULL_BRUSH));
}

void CHyperLink::SetToolTipTextColor(COLORREF sToolTipText) {
	m_ToolTip.SetTipTextColor(sToolTipText);
}

void CHyperLink::SetToolTipBgColor(COLORREF sToolTipBgColor)
{
	m_ToolTip.SetTipBkColor(sToolTipBgColor);

}

CString CHyperLink::GetLinkText()  {
	if (m_csLinkText.IsEmpty())
		return CString("");
	return m_csLinkText;
}

void CHyperLink::SetLinkUrl(CString csUrl) {
	m_csUrl= csUrl;
}

CString CHyperLink::GetLinkUrl() {
	return m_csUrl;
}

void CHyperLink::SetVisitedColor(COLORREF sVisitedColor) {
	m_sVisitedColor = sVisitedColor ;
}

void CHyperLink::SetHoverColor(COLORREF cHoverColor) {
	m_sHoverColor = cHoverColor;
}

void CHyperLink::SetFire(bool nFlag) {
	if (nFlag)
		m_bFire = true;
	else
		m_bFire = false;
}

void CHyperLink::SetFireChild(int nFlag) {
	if (nFlag)
		m_bFireChild = true;
	else
		m_bFireChild = false;
}

BOOL CHyperLink::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
	NMHDR* pMsgHdr;
	pMsgHdr = (NMHDR*) lParam;

	switch (pMsgHdr->code){
	case NM_RCLICK:
		break;
	default:
	;
	}
	
	return CStatic::OnNotify(wParam, lParam, pResult);
}

void CHyperLink::Fire() {

	CString profile_file = theApp.GetUsedProfilePath();

	SHELLEXECUTEINFO ExecuteInfo;
	memset(&ExecuteInfo, 0, sizeof(ExecuteInfo));

	ExecuteInfo.cbSize = sizeof(ExecuteInfo);
	ExecuteInfo.fMask = 0;
	ExecuteInfo.hwnd = 0;						    // Pointer to parent window; not needed
	ExecuteInfo.lpVerb = "open";		            // "open" == "execute" for an executable
	ExecuteInfo.lpFile = ToolsDirectory() + "\\OpenPad.exe";
	ExecuteInfo.lpParameters = profile_file;		// Parameters
	ExecuteInfo.lpDirectory = ToolsDirectory();		// Working directory, location of OpenPad
	ExecuteInfo.nShow = SW_SHOWNORMAL;				// Active window, default size
	ExecuteInfo.hInstApp = 0;

	if (!ShellExecuteEx(&ExecuteInfo))
		MessageBox("Unable to find OpenPad.\nMake sure you put OpenPad on tools directory and try again.", "Error");
	
	Sleep(300);
	LRESULT copyData;  //copyDataResult has value returned by OpenPad 
	HWND hOpenPadWnd = ::FindWindow("OpenPadClass", NULL);
	
	CString abs_line_number = GetLinkText().Mid(GetLinkText().ReverseFind('/') + 2);
	if (hOpenPadWnd)
	{
		COPYDATASTRUCT cpd; // The structure mentioned above
		cpd.dwData = 0;
		cpd.cbData = abs_line_number.GetLength() + sizeof(wchar_t); // sizeof (wchar_t) refers to the length of \ 0.     
		cpd.lpData = (void*)abs_line_number.GetBuffer(cpd.cbData);
		copyData = ::SendMessage(hOpenPadWnd, WM_COPYDATA, (WPARAM)m_hWnd, (LPARAM)(LPVOID)&cpd);
		abs_line_number.ReleaseBuffer();
	}
}
