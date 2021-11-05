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

// RtaWindow.cpp : implementation file
//

#include "stdafx.h"
#include "OpenHoldem.h"
#include "CTableState.h"
#include "CAutoConnector.h"
#include "CEngineContainer.h"
#include "CSymbolengineUserchair.h"
#include "RtaWindow.h"


// CRtaWindow

IMPLEMENT_DYNCREATE(CRtaWindow, CWnd)

CRtaWindow *p_rta_window = NULL;

CRtaWindow::CRtaWindow()
{
	SetHandrank(0);
	SetGto(0.0, 0.0, 0.0, 0, 0.0, 0.0, 0.0, 0.0, 0.0, FALSE);
	SetnOuts(0);
}

CRtaWindow::~CRtaWindow()
{
}

BEGIN_MESSAGE_MAP(CRtaWindow, CWnd)
	ON_WM_PAINT()
END_MESSAGE_MAP()


BOOL CRtaWindow::PreCreateWindow(CREATESTRUCT& cs) {
	if (!CWnd::PreCreateWindow(cs))
		return FALSE;

	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	// Size the window to 1/3 screen size and center it
	//cs.cy = ::GetSystemMetrics(SM_CYSCREEN) / 3;
	//cs.cx = ::GetSystemMetrics(SM_CXSCREEN) / 3;
	//cs.y = ((cs.cy * 3) - cs.cy) / 2;
	//cs.x = ((cs.cx * 3) - cs.cx) / 2;
	//cs.cy = 650;
	//cs.cx = 350;
	
	cs.dwExStyle |= WS_EX_CLIENTEDGE;
	cs.style &= ~WS_BORDER;
	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS,
		::LoadCursor(NULL, IDC_ARROW), reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1), NULL);

	return CWnd::PreCreateWindow(cs);
}

//CRtaWindow closing

void CRtaWindow::Close()
{
	this->CloseWindow();
}

// CRtaWindow initializing

void CRtaWindow::Init(CWnd * pWnd)
{
	// Register the window class.
	const char CLASS_NAME[] = "RTAClass";

	WNDCLASS wc = { };
	HINSTANCE hInstance = NULL;

	//hInstance = wc.hInstance;
	wc.lpszClassName = CLASS_NAME;

	RegisterClass(&wc);
	
	// Create the window

	this->CreateEx(
		NULL,								// Optional window styles.
		CLASS_NAME,							// Window class
		"Real Time Advisor",				// Window text
		WS_OVERLAPPED | WS_VISIBLE,         // Window style

		// Size and position
		//CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		0, 0, 350, 700,
		pWnd->m_hWnd,       // Parent window 
		NULL				// Menu
	);

	ShowWindow(TRUE);

	RECT		_client_rect;
	GetWindowRect(&_client_rect);
	SetBkMode(this->GetWindowDC()->GetSafeHdc(), TRANSPARENT);
	CPoint m_position;
	CSize m_size;
	int margin = 15;
	m_position.x = _client_rect.left + margin;
	m_position.y = _client_rect.top + margin;
	m_size.cx = 300;
	m_size.cy = 300;

	// Create CGraphObject ( 2DPie graph )
	m_pPieGraphObject = new CGraphObject();
	m_pPieGraphObject->Create(NULL, NULL, NULL, CRect(40, 20, 240, 220), this->GetWindowDC()->GetWindow(), ID_OBJECT_PIEGRAPH, NULL);
	// Create graph and set graph parameters
	m_pPieGraphObject->CreateGraph(GT_2DPIE);
	m_pPieGraphObject->SetGraphPosition(m_position);
	m_pPieGraphObject->SetGraphSize(m_size);
	m_pPieGraphObject->SetGraphBackgroundColor(RGB(255, 255, 255));
	m_pPieGraphObject->SetGraphTitle("0");
	m_pPieGraphObject->SetGraphSubtitle("Handrank");
	m_pPieGraphObject->SetGraphTitleShadow(FALSE);
	m_pPieGraphObject->SetGraphSubtitleShadow(FALSE);
	m_pPieGraphObject->SetGraphTitleColor(RGB(0, 0, 255));
	m_pPieGraphObject->SetGraphSubtitleColor(RGB(96, 96, 96));
	m_pPieGraphObject->SetGraphLegendBackgroundColor(RGB(208, 208, 208));
	// Add graph segments
	m_pPieGraphObject->Add2DPieGraphSegment(0.01, RGB(0, 255, 0), "Prwin");
	m_pPieGraphObject->Add2DPieGraphSegment(0.01, RGB(0, 0, 255), "Prtie");
	m_pPieGraphObject->Add2DPieGraphSegment(0.01, RGB(255, 0, 0), "Prlos");
	// Set graph animation
	m_pPieGraphObject->SetGraphAnimation(FALSE, AT_PIE_DRAW);

	m_position.y = (_client_rect.bottom - _client_rect.top) / 2 - margin;

	// Create CGraphObject ( 2DBar graph )
	m_pBarGraphObject = new CGraphObject();
	m_pBarGraphObject->Create(NULL, NULL, NULL, CRect(40, 20, 240, 220), this->GetWindowDC()->GetWindow(), ID_OBJECT_BARGRAPH, NULL);
	// Create graph and set graph parameters
	m_pBarGraphObject->CreateGraph(GT_2DBAR);
	m_pBarGraphObject->SetGraphPosition(m_position);
	m_pBarGraphObject->SetGraphSize(m_size);
	m_pBarGraphObject->SetGraphBackgroundColor(RGB(255, 255, 255));
	m_pBarGraphObject->SetGraphTitle("0");
	m_pBarGraphObject->SetGraphSubtitle("Outs");
	m_pBarGraphObject->SetGraphTitleShadow(FALSE);
	m_pBarGraphObject->SetGraphSubtitleShadow(FALSE);
	m_pBarGraphObject->SetGraphTitleColor(RGB(0, 0, 255));
	m_pBarGraphObject->SetGraphSubtitleColor(RGB(96, 96, 96));
	m_pBarGraphObject->SetGraphLegendBackgroundColor(RGB(208, 208, 208));
	// Add graph segments
	m_pBarGraphObject->Add2DBarGraphSegment("Odds");
	m_pBarGraphObject->Add2DBarGraphSegment("Equities");
	m_pBarGraphObject->Add2DBarGraphSeries("Out Odds", RGB(255, 0, 255));
	m_pBarGraphObject->Add2DBarGraphSeries("Implied Odds", RGB(0, 255, 255));
	m_pBarGraphObject->Add2DBarGraphSeries("Pot Odds", RGB(255, 255, 0));
	m_pBarGraphObject->Add2DBarGraphSeries("My Equity", RGB(0, 255, 0));
	m_pBarGraphObject->Add2DBarGraphSeries("Pot Equity", RGB(0, 0, 255));
	m_pBarGraphObject->Set2DBarGraphValue(1, 1, 0.01);
	m_pBarGraphObject->Set2DBarGraphValue(1, 2, 0.01);
	m_pBarGraphObject->Set2DBarGraphValue(1, 3, 0.01);
	m_pBarGraphObject->Set2DBarGraphValue(2, 4, 0.01);
	m_pBarGraphObject->Set2DBarGraphValue(2, 5, 0.01);
	// Set graph animation
	m_pBarGraphObject->SetGraphAnimation(FALSE, AT_BAR_DRAW_ALL);


	RECT current_position;
	pWnd->GetWindowRect(&current_position);
	SetWindowPos(&CWnd::wndTop, current_position.right, current_position.top, NULL, NULL, SWP_NOSIZE);
}

// CRtaWindow drawing

void CRtaWindow::Draw() {
	
	RECT current_position;
	if (p_autoconnector->IsConnectedToExistingWindow() && p_autoconnector->attached_hwnd() != NULL)
	{
		FromHandle(p_autoconnector->attached_hwnd())->GetWindowRect(&current_position);
		SetWindowPos(&CWnd::wndTop, current_position.right, current_position.top, NULL, NULL, SWP_NOSIZE);
	}

	//gto
	if (p_table_state->User()->HasKnownCards()) {

			// Handrank
			_info_handrank.Format("%i / 169", _handrank);
			m_pPieGraphObject->SetGraphTitle(_info_handrank);

			// Update pie graph segments
			m_pPieGraphObject->Update2DPieGraphSegment(1, 100 * _prwin, RGB(0, 255, 0), "Prwin");
			m_pPieGraphObject->Update2DPieGraphSegment(2, 100 * _prtie, RGB(0, 0, 255), "Prtie");
			m_pPieGraphObject->Update2DPieGraphSegment(3, 100 * _prlos, RGB(255, 0, 0), "Prlos");

			DWORD fdwMenu = theApp.m_pMainWnd->GetMenu()->GetMenuState(ID_TOOLS_ADDACTIONS, MF_BYCOMMAND);
			if (fdwMenu & MF_CHECKED) {
				_actiontrace_log_message.Replace("log$_", "Line: "); _actiontrace_log_message.Replace("_", " / ");
				m_pPieGraphObject->SetGraphInfos(_custom_log_message, LastAction().Right(LastAction().GetLength() - 2).MakeUpper(), _actiontrace_log_message);
			}			
			else {
				m_pPieGraphObject->SetGraphInfos(_custom_log_message, LastAction().Right(LastAction().GetLength() - 2).MakeUpper(), "");
			}
			// nOuts
			_info_nouts.Format("%i", _nouts);
			m_pBarGraphObject->SetGraphTitle(_info_nouts);

			// Update bar graph segments
			m_pBarGraphObject->Set2DBarGraphValue(1, 1, 100 * _outodds);
			m_pBarGraphObject->Set2DBarGraphValue(1, 2, _impliedodds);
			m_pBarGraphObject->Set2DBarGraphValue(1, 3, 100 * _potodds);
			m_pBarGraphObject->Set2DBarGraphValue(2, 4, _myequity);
			m_pBarGraphObject->Set2DBarGraphValue(2, 5, _potequity);
	}
}

CString CRtaWindow::LastAction() {
	if (p_engine_container->symbol_engine_userchair() == NULL) {
		// Very early phase of initialization
		// Can't continue here.
		return "Not playing";
	}
	if (!p_engine_container->symbol_engine_userchair()->userchair_confirmed()) {
		return "Not playing";
	}
	// Return the last saved action.
	// This value should get set exactly once after autoplayer/actions
	// to avoid multiple evaluations of the autoplayer-functions,
	// especially at different heartbeats.
	return _last_action;
}

void CRtaWindow::SetGto(double prwin, double prtie, double prlos, int nouts, double outodds, double impliedodds, double potodds, double myequity, double potequity, bool prwin_mustplay) {
	_prwin = prwin;
	_prtie = prtie;
	_prlos = prlos;
	_nouts = nouts;
	_outodds = outodds;
	_impliedodds = impliedodds;
	_potodds = potodds;
	_myequity = myequity;
	_potequity = potequity;
	_prwin_mustplay = prwin_mustplay;
}

void CRtaWindow::SetnOuts(int nouts)
{
	_nouts = nouts;
}


// CRtaWindow diagnostics

#ifdef _DEBUG
void CRtaWindow::AssertValid() const
{
	CWnd::AssertValid();
}

#ifndef _WIN32_WCE
void CRtaWindow::Dump(CDumpContext& dc) const
{
	CWnd::Dump(dc);
}
#endif
#endif //_DEBUG


// CRtaWindow message handlers
