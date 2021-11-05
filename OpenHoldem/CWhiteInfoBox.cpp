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

#include "stdafx.h"
#include "CWhiteInfoBox.h"

#include "OpenHoldem.h"
#include "CEngineContainer.h"
#include "CHandresetDetector.h"
#include "CSymbolEngineChipAmounts.h"
#include "CSymbolEngineGameType.h"
#include "CSymbolengineTableLimits.h"
#include "CSymbolengineUserchair.h"
#include "CTableState.h"
#include "..\DLLs\StringFunctions_DLL\string_functions.h"

CWhiteInfoBox *p_white_info_box = NULL;
BOOL init = TRUE;

int CWhiteInfoBox::DoDataExchange(CDataExchange* pDX)
{
	//CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CControlContainerDlg)
	DDX_Control(pDX, IDC_LINK, m_Link);
	//}}AFX_DATA_MAP
	return DoDataExchange(pDX);
}

//BEGIN_MESSAGE_MAP(CWhiteInfoBox, CSpaceOptimizedGlobalObject)
	//{{AFX_MSG_MAP(CWhiteInfoBox)
	//Message Trap - to trap message from the CHyperlink control
//	ON_MESSAGE(_HYPERLINK_EVENT, OnChildFire)
	//}}AFX_MSG_MAP
//END_MESSAGE_MAP()


CWhiteInfoBox::CWhiteInfoBox() {
	SetHandrank(0);
	SetGto(0.0, 0.0, 0.0, 0, 0.0, 0.0, 0.0, 0.0, 0.0, FALSE);
	SetnOuts(0);
}

CWhiteInfoBox::~CWhiteInfoBox() {
}

void CWhiteInfoBox::Draw(RECT client_rect, LOGFONT logfont, CDC *pDC,
                         CPen *black_pen, CBrush *white_brush) {
	CPen		*pTempPen = NULL, oldpen;
	CBrush	*pTempBrush = NULL, oldbrush;
	RECT		rect = {0};
	CFont		*oldfont = NULL, cFont;
	int			left = 0, top = 0, right = 0, bottom = 0;
		
	// "White box" in the OpenHoldem-GUI with basic important info
	const int k_basic_height = 2;				// pixels
	const int k_extra_height_per_line = 16;	// pixels
	const int k_number_of_default_lines = 9;	// hand-number, game-type, ante, pot, gto
	int height = k_basic_height 
		+ k_extra_height_per_line * k_number_of_default_lines;
	if (kMaxLogSymbolsForWhiteBox > 0)	{
		// Extra lines for symbol-logging
		height += k_extra_height_per_line * kMaxLogSymbolsForWhiteBox;
	}
  // Figure placement of box
	left = client_rect.right/2-130;
	top = 175;
	right = client_rect.right/2+130;
	bottom = top+height;

	pTempPen = (CPen*)pDC->SelectObject(&black_pen);
	oldpen.FromHandle((HPEN)pTempPen);					// Save old pen
	pTempBrush = (CBrush*)pDC->SelectObject(&white_brush);
	oldbrush.FromHandle((HBRUSH)pTempBrush);			// Save old brush

	pDC->SetBkMode(OPAQUE);
	pDC->Rectangle(left, top, right, bottom);

	// Set rectangle
	rect.left = left;
	rect.top = top+3;
	rect.right = right;
	rect.bottom = bottom;

	// Set font basics
	logfont.lfHeight = -14;
	logfont.lfWeight = FW_NORMAL;
	cFont.CreateFontIndirect(&logfont);
	oldfont = pDC->SelectObject(&cFont);
	pDC->SetTextColor(COLOR_BLACK);

	// Set Hyperlink Static control
	if (init) {
		mLinkRect = CRect(left + 9, top + k_extra_height_per_line * 3.3, left + 9, top + k_extra_height_per_line * 3.5 - logfont.lfHeight);
		m_Link.Create(NULL, WS_CHILD | WS_OVERLAPPED | WS_CLIPSIBLINGS | WS_VISIBLE,
			mLinkRect, pDC->GetWindow());
		m_Link.SetFire(TRUE);
		init = FALSE;
	}

  CString info_txt = InfoText();
	// Draw it
	pDC->SetBkMode(TRANSPARENT);
	pDC->DrawText(info_txt.GetString(), info_txt.GetLength(), &rect, NULL);

	// Restore original pen, brush and font
	pDC->SelectObject(oldpen);
	pDC->SelectObject(oldbrush);
	pDC->SelectObject(oldfont);
	cFont.DeleteObject();

	// Set font basics
	logfont.lfHeight = -14;
	logfont.lfWeight = FW_BOLD;
	cFont.CreateFontIndirect(&logfont);
	oldfont = pDC->SelectObject(&cFont);
	pDC->SetTextColor(COLOR_MAROON);

	info_txt = GtoText();
	// Draw it
	pDC->SetBkMode(TRANSPARENT);
	pDC->DrawText(info_txt.GetString(), info_txt.GetLength(), &rect, NULL);

	// Restore original pen, brush and font
	pDC->SelectObject(oldpen);
	pDC->SelectObject(oldbrush);
	pDC->SelectObject(oldfont);
	cFont.DeleteObject();
}

CString CWhiteInfoBox::InfoText() {
  double sym_bblind		= p_engine_container->symbol_engine_tablelimits()->bblind();
	double sym_sblind		= p_engine_container->symbol_engine_tablelimits()->sblind();
	double sym_ante			= p_engine_container->symbol_engine_tablelimits()->ante();
	int sym_lim				  = p_engine_container->symbol_engine_gametype()->gametype();
	CString sym_handnumber = p_handreset_detector->GetHandNumber();
	double sym_pot			= p_engine_container->symbol_engine_chip_amounts()->pot();
  CString result, s;
	// handnumber
	if (sym_handnumber != "") {
		s.Format("  Hand #: %s\n", sym_handnumber);
	}	else 	{
		s.Format("  Hand #: -\n");
	}
	result.Append(s);

  // blinds, game-type
	CString format_string;
  if (IsInteger(sym_sblind) && IsInteger(sym_bblind)) {
    // Display as integer numbers
		format_string = "  %s %.0f/%.0f/%.0f\n";
  }
  else {
		// Fractional nunbers: use 2.00 digits  
		format_string = "  %s %.2f/%.2f/%.2f\n";
	}
	s.Format(format_string,
		p_engine_container->symbol_engine_gametype()->GetGameTypeAsString(),
		sym_sblind, sym_bblind, p_engine_container->symbol_engine_tablelimits()->bigbet());
	result.Append(s);

	// ante + pot
	if (sym_ante != 0) {
		s.Format("  Ante: %s  Pot: %s\n", Number2CString(sym_ante), Number2CString(sym_pot));
		result.Append(s);
	}
	else {
	// Pot
	s.Format("  Pot: %s\n", Number2CString(sym_pot));
	result.Append(s);
	}

	// logged symbols
	if (kMaxLogSymbolsForWhiteBox > 0) {
		DWORD fdwMenu = theApp.m_pMainWnd->GetMenu()->GetMenuState(ID_TOOLS_ADDACTIONS, MF_BYCOMMAND);
		if (fdwMenu & MF_CHECKED)
		{
				_actiontrace_log_message.Replace("log$_", "Line: "); _actiontrace_log_message.Replace("_", " / ");
				WINDOWPLACEMENT wndpl;
				m_Link.GetWindowPlacement(&wndpl);
				wndpl.rcNormalPosition.right = mLinkRect.left + _actiontrace_log_message.GetLength() * 8;
				m_Link.SetWindowPlacement(&wndpl);
				m_Link.SetLinkText(_actiontrace_log_message);
				//m_Link.SetWindowText(_actiontrace_log_message);

				LRESULT copyData;  //copyDataResult has value returned by OpenPad 
				HWND hOpenPadWnd = ::FindWindow("OpenPadClass", NULL);

				CString abs_line_number = _actiontrace_log_message.Mid(_actiontrace_log_message.ReverseFind('/') + 2);
				if (hOpenPadWnd)
				{
					COPYDATASTRUCT cpd; // The structure mentioned above
					cpd.dwData = 0;
					cpd.cbData = abs_line_number.GetLength() + sizeof(wchar_t); // sizeof (wchar_t) refers to the length of \ 0.     
					cpd.lpData = (void*)abs_line_number.GetBuffer(cpd.cbData);
					copyData = ::SendMessage(hOpenPadWnd, WM_COPYDATA, (WPARAM)AfxGetMainWnd()->GetSafeHwnd(), (LPARAM)(LPVOID)&cpd);
					abs_line_number.ReleaseBuffer();
				}
				result.Append("                                    " + _custom_log_message);
		}
		else result.Append("  " + _custom_log_message);
	}

  return result;
}

CString CWhiteInfoBox::GtoText() {
	CString result;

	//gto
	if (p_table_state->User()->HasKnownCards()) {
		_info_handrank.Format("\n\n\n\n  %s\n  Rank:  %i/169  ", LastAction().Right(LastAction().GetLength()-2).MakeUpper(), _handrank);
		// PrWin: percentages instead of probabilities
		_info_gto.Format("  PrWin:  %3.1f/%3.1f/%3.1f\n  Outs:  %i    Out Odds:  %3.1f\n  Implied Odds:  %3.1f    PotOdds:  %3.1f\n  My Equity:  %3.1f    Pot Equity:  %3.1f",
			100 * _prwin, 100 * _prtie, 100 * _prlos, _nouts, 100 * _outodds, _impliedodds, 100 * _potodds, _myequity, _potequity);
	}
	result.Append(_info_handrank);
	result.Append(_info_gto);

	return result;
}

CString CWhiteInfoBox::LastAction() {
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

void CWhiteInfoBox::SetGto(double prwin, double prtie, double prlos, int nouts, double outodds, double impliedodds, double potodds, double myequity, double potequity, bool prwin_mustplay) {
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

void CWhiteInfoBox::SetnOuts(int nouts)
{
	_nouts = nouts;
}

///////// Event From the child catch here//////////////////////
///////// WAPARAM contains the ID of the control from which the event 
//generates

LRESULT CWhiteInfoBox::OnChildFire(WPARAM wparam, LPARAM lparam)
{
	CString csTmp;
	csTmp.Format(" Child Fire Me ====>>with resource ID(#%d)", (int)wparam);

//	MessageBox(csTmp.GetBuffer(MAX_PATH));
	csTmp.ReleaseBuffer();
	return lparam;
}