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

// OpenHoldemView.cpp : implementation of the COpenHoldemView class
//

#include "stdafx.h"
#include "OpenHoldemView.h"

#include "CAutoplayerTrace.h"
#include "CEngineContainer.h"
#include "CHandresetDetector.h"
#include "CHeartbeatThread.h"

#include "CCasinoInterface.h"
#include "CScraper.h"
#include "CStringMatch.h"
#include "CSymbolengineAutoplayer.h"
#include "CSymbolengineChipAmounts.h"
#include "CSymbolengineColourCodes.h"
#include "CSymbolengineGameType.h"
#include "CSymbolEngineIsOmaha.h"
#include "CSymbolEngineIsTournament.h"
#include "CSymbolEngineTableLimits.h"
#include "..\CTablemap\CTablemap.h"
#include "CTableState.h"
#include "CWhiteInfoBox.h"
#include "RtaWindow.h"

#include "OpenHoldem.h"
#include "OpenHoldemDoc.h"

DWORD dwMenu;

// Table layouts
int		cc[kNumberOfCommunityCards][2] =
{
	{-(CARDSIZEX * 2 + 3 * 2 + CARDSIZEX / 2), -(CARDSIZEY / 2)},
	{-(CARDSIZEX * 1 + 3 * 1 + CARDSIZEX / 2), -(CARDSIZEY / 2)},
	{-(CARDSIZEX * 0 + 3 * 0 + CARDSIZEX / 2), -(CARDSIZEY / 2)},
	{+(CARDSIZEX * 0 + 3 * 1 + CARDSIZEX / 2), -(CARDSIZEY / 2)},
	{+(CARDSIZEX * 1 + 3 * 2 + CARDSIZEX / 2), -(CARDSIZEY / 2)}
};

// Player locations as a percentage of width/height
// [nplayers][chairnum][x/y]
const int kNumberOfScreenDimensions = 2;
double	pc[kMaxNumberOfPlayers + 1][kMaxNumberOfPlayers][kNumberOfScreenDimensions] = {
	// 0 chairs
	  { {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0} },
	  // 1 chair
		{ {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0} },
		// 2 chairs
		  { {.95,.47}, {.05,.47}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0} },
		  // 3 chairs
			{ {.95,.47}, {.50,.83}, {.05,.47}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0} },
			// 4 chairs
			  { {.89,.25}, {.89,.69}, {.11,.69}, {.11,.25}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0} },
			  // 5 chairs
				{ {.89,.25}, {.89,.69}, {.50,.83}, {.11,.69}, {.11,.25}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0} },
				// 6 chairs
				  { {.72,.11}, {.95,.47}, {.72,.83}, {.28,.83}, {.05,.47}, {.28,.11}, {0,0}, {0,0}, {0,0}, {0,0} },
				  // 7 chairs
					{ {.72,.11}, {.95,.47}, {.72,.83}, {.50,.83}, {.28,.83}, {.05,.47}, {.28,.11}, {0,0}, {0,0}, {0,0} },
					// 8 chairs
					  { {.72,.11}, {.89,.25}, {.89,.69}, {.72,.83}, {.28,.83}, {.11,.69}, {.11,.25}, {.28,.11}, {0,0}, {0,0} },
					  // 9 chairs
						{ {.72,.11}, {.89,.25}, {.89,.69}, {.72,.83}, {.50,.83}, {.28,.83}, {.11,.69}, {.11,.25}, {.28,.11}, {0,0} },
						// 10 chairs
						  { {.72,.11}, {.85,.21}, {.95,.47}, {.85,.73}, {.72,.83}, {.28,.83}, {.15,.73}, {.05,.47}, {.15,.21}, {.28,.11} }
};

// Player bet locations relative to player locations above
// numbers are in pixel units
// [nplayers][chairnum][x/y]
int pcbet[11][10][2] =
{
	{ {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0} },	// 0 players
	{ {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0} },	// 1 player
	{ {-40,+0}, {+40,+0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0} },	// 2 players
	{ {-40,+0}, {+0,-45}, {+40,+0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0} }, // 3 players
	{ {-40,+37}, {-40,-20}, {+40,-20}, {+40,+37}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0} }, // 4 players
	{ {-40,+37}, {-40,-20}, {+0,-45}, {+40,-20}, {+40,+37},  {0,0}, {0,0}, {0,0}, {0,0}, {0,0} }, // 5 players
	{ {-40,+60}, {-40,+0}, {-40,-40}, {+40,-40}, {+40,+0}, {+40,+60}, {0,0}, {0,0}, {0,0}, {0,0} },	// 6 players
	{ {-40,+40}, {-40,+0}, {-40,+40}, {+0,-45}, {+40,-40}, {+40,+0}, {+40,+40}, {0,0}, {0,0}, {0,0} }, // 7 players
	{ {-40,+40}, {-40,+37}, {-40,-20}, {-40,-40}, {+40,-40}, {+40,-20}, {+40,+37}, {+40,+40}, {0,0}, {0,0} }, // 8 players
	{ {-40,+58}, {-40,+37}, {-40,-20}, {-40,-40}, {+0,-45}, {+40,-40}, {+40,-20}, {+40,+37}, {+40,+58}, {0,0} }, // 9 players
	{ {-40,+40}, {-40,+37}, {-40,+0}, {-40,-20}, {-40,-40}, {+40,-40}, {+40,-20}, {+40,+0}, {+40,+37}, {+40,+40} } // 10 players
};

// COpenHoldemView
IMPLEMENT_DYNCREATE(COpenHoldemView, CView)

BEGIN_MESSAGE_MAP(COpenHoldemView, CView)
	ON_WM_ERASEBKGND()
	ON_WM_TIMER()
END_MESSAGE_MAP()

// COpenHoldemView construction/destruction
COpenHoldemView::COpenHoldemView() {
	_black_pen.CreatePen(PS_SOLID, 1, COLOR_BLACK);
	_green_pen.CreatePen(PS_SOLID, 1, COLOR_GREEN);
	_red_pen.CreatePen(PS_SOLID, 1, COLOR_RED);
	_blue_pen.CreatePen(PS_SOLID, 1, COLOR_BLUE);
	_white_dot_pen.CreatePen(PS_DOT, 1, COLOR_WHITE);
	_null_pen.CreatePen(PS_NULL, 0, COLOR_BLACK);

	_white_brush.CreateSolidBrush(COLOR_WHITE);
	_gray_brush.CreateSolidBrush(COLOR_GRAY);
	_red_brush.CreateSolidBrush(COLOR_RED);
	_yellow_brush.CreateSolidBrush(COLOR_YELLOW);

	_logfont.lfWidth = 0;
	_logfont.lfEscapement = 0;
	_logfont.lfOrientation = 0;
	_logfont.lfItalic = 0;
	_logfont.lfUnderline = 0;
	_logfont.lfStrikeOut = 0;
	_logfont.lfCharSet = 0;
	_logfont.lfOutPrecision = 0;
	_logfont.lfClipPrecision = 0;
	_logfont.lfQuality = PROOF_QUALITY;
	_logfont.lfPitchAndFamily = 0;
	strcpy_s(_logfont.lfFaceName, 32, "Times New Roman");

	_handnumber_last = "";
	_sblind_last = _bblind_last = _lim_last = _ante_last = _pot_last = 0.;
	_iterator_thread_progress_last = 0;

	for (int i = 0; i < kNumberOfCommunityCards; i++)
		_card_common_last[i] = CARD_UNDEFINED;

	for (int i = 0; i < kMaxNumberOfPlayers; i++)
	{
		_seated_last[i] = false;
		_active_last[i] = false;
		_playername_last[i] = "";
		_dealer_last[i] = false;
		_playerbalance_last[i] = _playerbet_last[i] = 0.;
		for (int j = 0; j < kMaxNumberOfCardsPerPlayer; j++) {
			_card_player_last[i][j] = CARD_NOCARD;
		}
	}

	_istournament_last = false;
}

COpenHoldemView::~COpenHoldemView() {
	_black_pen.DeleteObject();
	_green_pen.DeleteObject();
	_red_pen.DeleteObject();
	_blue_pen.DeleteObject();
	_white_dot_pen.DeleteObject();
	_null_pen.DeleteObject();
	_white_brush.DeleteObject();
	_gray_brush.DeleteObject();
	_red_brush.DeleteObject();
	_yellow_brush.DeleteObject();
}

BOOL COpenHoldemView::PreCreateWindow(CREATESTRUCT& cs) {
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	/* Size the window to 1/3 screen size and center it
	cs.cy = ::GetSystemMetrics(SM_CYSCREEN) / 3;
	cs.cx = ::GetSystemMetrics(SM_CXSCREEN) / 3;
	cs.y = ((cs.cy * 3) - cs.cy) / 2;
	cs.x = ((cs.cx * 3) - cs.cx) / 2; */

	return CView::PreCreateWindow(cs);
}

void COpenHoldemView::OnInitialUpdate() {
	CView::OnInitialUpdate();

	// Timer to check for display updates
	SetTimer(DISPLAY_UPDATE_TIMER, 103, 0);
}

// COpenHoldemView drawing
void COpenHoldemView::OnDraw(CDC* pDC) {
	UpdateDisplay(true);
	write_log(Preferences()->debug_alltherest(), "[GUI] location Johnny_1\n");
}

void COpenHoldemView::OnTimer(UINT_PTR nIDEvent) {
	write_log(Preferences()->debug_timers(), "[GUI] COpenHoldemView::OnTimer()\n");
	if (nIDEvent == DISPLAY_UPDATE_TIMER)
	{
		// Only do this if we are not in the middle of a scraper/symbol update
		if (TryEnterCriticalSection(&p_heartbeat_thread->cs_update_in_progress))
		{
			UpdateDisplay(false);
			write_log(Preferences()->debug_alltherest(), "[GUI] location Johnny_2\n");
			LeaveCriticalSection(&p_heartbeat_thread->cs_update_in_progress);
		}
	}
	// Otherwise: continue with parent class
	CView::OnTimer(nIDEvent);
}

void COpenHoldemView::UpdateDisplay(const bool update_all) {
	bool		update_it = false;
	CDC			*pDC = GetDC();

	CString sym_handnumber = p_handreset_detector->GetHandNumber();
	double  sym_bblind = p_engine_container->symbol_engine_tablelimits()->bblind();
	double  sym_sblind = p_engine_container->symbol_engine_tablelimits()->sblind();
	double  sym_ante = p_engine_container->symbol_engine_tablelimits()->ante();
	int     sym_lim = p_engine_container->symbol_engine_gametype()->gametype();
	bool    sym_istournament = p_engine_container->symbol_engine_istournament()->istournament();
	double  sym_pot = p_engine_container->symbol_engine_chip_amounts()->pot();

	// Get size of current client window
	GetClientRect(&_client_rect);

	// Set background color (light gray)
	if (update_all)
	{
		CBrush backBrush(COLOR_GRAY);
		CBrush* pOldBrush = pDC->SelectObject(&backBrush);
		pDC->PatBlt(_client_rect.left, _client_rect.top, _client_rect.right - _client_rect.left, _client_rect.bottom - _client_rect.top, PATCOPY);
		pDC->SelectObject(pOldBrush);
	}

	// Draw center info box
	update_it = false;
	if (_handnumber_last != sym_handnumber)
	{
		_handnumber_last = sym_handnumber;
		update_it = true;
	}
	if (_sblind_last != sym_sblind)
	{
		_sblind_last = sym_sblind;
		update_it = true;
	}
	if (_bblind_last != sym_bblind)
	{
		_bblind_last = sym_bblind;
		update_it = true;
	}
	if (_lim_last != sym_lim)
	{
		_lim_last = sym_lim;
		update_it = true;
	}
	if (_istournament_last != sym_istournament)
	{
		_istournament_last = sym_istournament;
		update_it = true;
	}
	if (_ante_last != sym_ante != 0)
	{
		_ante_last = sym_ante;
		update_it = true;
	}
	if (_pot_last != sym_pot)
	{
		_pot_last = sym_pot;
		update_it = true;
	}

	if ((p_engine_container->symbol_engine_autoplayer()->ismyturn()) || update_it || update_all)
	{
		dwMenu = theApp.m_pMainWnd->GetMenu()->GetMenuState(ID_VIEW_RTA, MF_BYCOMMAND);
		if (dwMenu & MF_CHECKED)
		{
			assert(p_rta_window != NULL);
			p_rta_window->Draw();
		}
		assert(p_white_info_box != NULL);
		p_white_info_box->Draw(_client_rect, _logfont, pDC,
			&_black_pen, &_white_brush);
		ReleaseDC(pDC);
	}

	// Draw button state indicators
	DrawButtonIndicators();

	// Draw common cards
	for (int i = 0; i < kNumberOfCommunityCards; i++)
	{
		Card *p_card = p_table_state->CommonCards(i);
		int card_value = p_table_state->CommonCards(i)->GetValue();
		if (_card_common_last[i] != card_value || update_all)
		{
			_card_common_last[i] = card_value;
			write_log(Preferences()->debug_gui(), "[GUI] COpenHoldemView::UpdateDisplay() Drawing common card %i: [%s]\n",
				i, p_card->ToString());
			DrawCard(p_card,
				_client_rect.right / 2 + cc[i][0], _client_rect.bottom / 3.5 + cc[i][1],
				_client_rect.right / 2 + cc[i][0] + CARDSIZEX, _client_rect.bottom / 3.5 + cc[i][1] + CARDSIZEY,
				false);
		}
	}
	// Draw collection of player info
	for (int i = 0; i < p_tablemap->nchairs(); i++)
	{
		write_log(Preferences()->debug_gui(), "[GUI] COpenHoldemView::UpdateDisplay() checking changes for chair %i\n", i);
		// Figure out if we need to redraw this seat
		update_it = false;
		if (_seated_last[i] != p_table_state->Player(i)->seated()
			|| _active_last[i] != p_table_state->Player(i)->active()) {
			_seated_last[i] = p_table_state->Player(i)->seated();
			_active_last[i] = p_table_state->Player(i)->active();
			update_it = true;
		}
		if (_card_player_last[i][0] != p_table_state->Player(i)->hole_cards(0)->GetValue()
			|| _card_player_last[i][1] != p_table_state->Player(i)->hole_cards(1)->GetValue()
			|| _card_player_last[i][2] != p_table_state->Player(i)->hole_cards(2)->GetValue()
			|| _card_player_last[i][3] != p_table_state->Player(i)->hole_cards(3)->GetValue()) {
			_card_player_last[i][0] = p_table_state->Player(i)->hole_cards(0)->GetValue();
			_card_player_last[i][1] = p_table_state->Player(i)->hole_cards(1)->GetValue();
			_card_player_last[i][2] = p_table_state->Player(i)->hole_cards(2)->GetValue();
			_card_player_last[i][3] = p_table_state->Player(i)->hole_cards(3)->GetValue();
			update_it = true;
		}
		if (_dealer_last[i] != p_table_state->Player(i)->dealer()) {
			_dealer_last[i] = p_table_state->Player(i)->dealer();
			update_it = true;
		}
		if (_playername_last[i] != p_table_state->Player(i)->name()) {
			_playername_last[i] = p_table_state->Player(i)->name();
			update_it = true;
		}
		if (_playerbalance_last[i] != p_table_state->Player(i)->_balance.GetValue()) {
			_playerbalance_last[i] = p_table_state->Player(i)->_balance.GetValue();
			update_it = true;
		}
		if (_playerbet_last[i] != p_table_state->Player(i)->_bet.GetValue())
		{
			_playerbet_last[i] = p_table_state->Player(i)->_bet.GetValue();
			update_it = true;
		}

		if (update_it || update_all) {
			write_log(Preferences()->debug_gui(), "[GUI] COpenHoldemView::UpdateDisplay() updating chair %i\n", i);
			// Draw active circle
			if (p_table_state->Player(i)->seated()) {
				DrawSeatedActiveCircle(i);
				// Draw cards first, because we want the name 
				// to occlude the cards and not the other way.
				DrawPlayerCards(i);
				DrawNameBox(i);
				DrawBalanceBox(i);
				// !! Disabled, as it gets rarely used
				// and doesn't fit well to the Omaha cards.
				// DrawColourCodes(i);
			}
			// Drawing a bet, even if no player seated.
			// The player might have left the table, 
			// but depending on casinos potmethod a bet might still be there.
			DrawPlayerBet(i);
		}
		// Draw dealer button
		// At some casinos the dealer can be at an empty seat.
		// Therefore we draw the dealer-button anyway, inependent of "seated" and "active".
		// Draw it at the very last, as we want to have it at the top of the cards.
		if (p_table_state->Player(i)->dealer()) {
			DrawDealerButton(i);
		}
	}
	write_log(Preferences()->debug_gui(), "[GUI] COpenHoldemView::UpdateDisplay() Update finished\n");
	ReleaseDC(pDC);
	write_log(Preferences()->debug_gui(), "[GUI] COpenHoldemView::UpdateDisplay() DC released\n");
}

void COpenHoldemView::DrawButtonIndicators(void) {
	bool fold_drawn, call_drawn, check_drawn, raise_drawn, allin_drawn;
	bool autopost_drawn, sitin_drawn, sitout_drawn, leave_drawn, prefold_drawn;

	autopost_drawn = sitin_drawn = sitout_drawn = leave_drawn = prefold_drawn = false;
	fold_drawn = call_drawn = check_drawn = raise_drawn = allin_drawn = false;

	// Draw "on" buttons
	assert(p_casino_interface != NULL);
	if (p_casino_interface->LogicalAutoplayerButton(k_autoplayer_function_fold)->IsClickable()) {
		DrawSpecificButtonIndicator('F', true, _client_rect.right - 84, _client_rect.bottom - 16, _client_rect.right - 70, _client_rect.bottom - 2);
		fold_drawn = true;
	}
	if (p_casino_interface->LogicalAutoplayerButton(k_autoplayer_function_call)->IsClickable()) {
		DrawSpecificButtonIndicator('C', true, _client_rect.right - 67, _client_rect.bottom - 16, _client_rect.right - 53, _client_rect.bottom - 2);
		call_drawn = true;
	}
	if (p_casino_interface->LogicalAutoplayerButton(k_autoplayer_function_check)->IsClickable()) {
		DrawSpecificButtonIndicator('K', true, _client_rect.right - 50, _client_rect.bottom - 16, _client_rect.right - 36, _client_rect.bottom - 2);
		check_drawn = true;
	}
	if (p_casino_interface->LogicalAutoplayerButton(k_autoplayer_function_raise)->IsClickable()) {
		DrawSpecificButtonIndicator('R', true, _client_rect.right - 33, _client_rect.bottom - 16, _client_rect.right - 19, _client_rect.bottom - 2);
		raise_drawn = true;
	}
	if (p_casino_interface->LogicalAutoplayerButton(k_autoplayer_function_allin)->IsClickable()) {
		DrawSpecificButtonIndicator('A', true, _client_rect.right - 16, _client_rect.bottom - 16, _client_rect.right - 2, _client_rect.bottom - 2);
		allin_drawn = true;
	}
	if (p_casino_interface->LogicalAutoplayerButton(k_hopper_function_autopost)->IsClickable()) {
		DrawSpecificButtonIndicator('T', true, _client_rect.left + 2, _client_rect.bottom - 16, _client_rect.left + 16, _client_rect.bottom - 2);
		autopost_drawn = true;
	}
	if (p_casino_interface->LogicalAutoplayerButton(k_hopper_function_sitin)->IsClickable()) {
		DrawSpecificButtonIndicator('I', true, _client_rect.left + 19, _client_rect.bottom - 16, _client_rect.left + 33, _client_rect.bottom - 2);
		sitin_drawn = true;
	}
	if (p_casino_interface->LogicalAutoplayerButton(k_hopper_function_sitout)->IsClickable()) {
		DrawSpecificButtonIndicator('O', true, _client_rect.left + 36, _client_rect.bottom - 16, _client_rect.left + 50, _client_rect.bottom - 2);
		sitout_drawn = true;
	}
	if (p_casino_interface->LogicalAutoplayerButton(k_hopper_function_leave)->IsClickable()) {
		DrawSpecificButtonIndicator('L', true, _client_rect.left + 53, _client_rect.bottom - 16, _client_rect.left + 67, _client_rect.bottom - 2);
		leave_drawn = true;
	}
	if (p_casino_interface->LogicalAutoplayerButton(k_standard_function_prefold)->IsClickable()) {
		DrawSpecificButtonIndicator('P', true, _client_rect.left + 70, _client_rect.bottom - 16, _client_rect.left + 84, _client_rect.bottom - 2);
		prefold_drawn = true;
	}
	// !! To do: rematch

	  // Draw "off" buttons
	if (!fold_drawn) {
		DrawSpecificButtonIndicator('F', false, _client_rect.right - 84, _client_rect.bottom - 16, _client_rect.right - 70, _client_rect.bottom - 2);
	}
	if (!call_drawn) {
		DrawSpecificButtonIndicator('C', false, _client_rect.right - 67, _client_rect.bottom - 16, _client_rect.right - 53, _client_rect.bottom - 2);
	}
	if (!check_drawn) {
		DrawSpecificButtonIndicator('K', false, _client_rect.right - 50, _client_rect.bottom - 16, _client_rect.right - 36, _client_rect.bottom - 2);
	}
	if (!raise_drawn) {
		DrawSpecificButtonIndicator('R', false, _client_rect.right - 33, _client_rect.bottom - 16, _client_rect.right - 19, _client_rect.bottom - 2);
	}
	if (!allin_drawn) {
		DrawSpecificButtonIndicator('A', false, _client_rect.right - 16, _client_rect.bottom - 16, _client_rect.right - 2, _client_rect.bottom - 2);
	}
	if (!autopost_drawn) {
		DrawSpecificButtonIndicator('T', false, _client_rect.left + 2, _client_rect.bottom - 16, _client_rect.left + 16, _client_rect.bottom - 2);
	}
	if (!sitin_drawn) {
		DrawSpecificButtonIndicator('I', false, _client_rect.left + 19, _client_rect.bottom - 16, _client_rect.left + 33, _client_rect.bottom - 2);
	}
	if (!sitout_drawn) {
		DrawSpecificButtonIndicator('O', false, _client_rect.left + 36, _client_rect.bottom - 16, _client_rect.left + 50, _client_rect.bottom - 2);
	}
	if (!leave_drawn) {
		DrawSpecificButtonIndicator('L', false, _client_rect.left + 53, _client_rect.bottom - 16, _client_rect.left + 67, _client_rect.bottom - 2);
	}
	if (!prefold_drawn) {
		DrawSpecificButtonIndicator('P', false, _client_rect.left + 70, _client_rect.bottom - 16, _client_rect.left + 84, _client_rect.bottom - 2);
	}
}

void COpenHoldemView::DrawSpecificButtonIndicator(const char ch, const bool on_off, const int left,
	const int top, const int right, const int bottom) {
	CPen		*pTempPen = NULL, oldpen;
	CBrush	*pTempBrush = NULL, oldbrush;
	RECT		rect = { 0 };
	CFont		*oldfont = NULL, cFont;
	CString	t = "";
	CDC			*pDC = GetDC();

	// Set font basics
	_logfont.lfHeight = -12;
	_logfont.lfWeight = FW_NORMAL;
	cFont.CreateFontIndirect(&_logfont);
	oldfont = pDC->SelectObject(&cFont);

	// Background color
	pDC->SetBkColor(COLOR_GRAY);

	if (on_off == false) {
		pTempPen = (CPen*)pDC->SelectObject(&_white_dot_pen);
		pTempBrush = (CBrush*)pDC->SelectObject(&_gray_brush);
		pDC->SetTextColor(COLOR_WHITE);
	}
	else {
		if (ch == 'F') {
			pTempPen = (CPen*)pDC->SelectObject(&_red_pen);
			pTempBrush = (CBrush*)pDC->SelectObject(&_white_brush);
			pDC->SetTextColor(COLOR_RED);
		}
		else if (ch == 'C') {
			pTempPen = (CPen*)pDC->SelectObject(&_blue_pen);
			pTempBrush = (CBrush*)pDC->SelectObject(&_white_brush);
			pDC->SetTextColor(COLOR_BLUE);
		}
		else if (ch == 'K') {
			pTempPen = (CPen*)pDC->SelectObject(&_blue_pen);
			pTempBrush = (CBrush*)pDC->SelectObject(&_white_brush);
			pDC->SetTextColor(COLOR_BLUE);
		}
		else if (ch == 'R') {
			pTempPen = (CPen*)pDC->SelectObject(&_green_pen);
			pTempBrush = (CBrush*)pDC->SelectObject(&_white_brush);
			pDC->SetTextColor(COLOR_GREEN);
		}
		else if (ch == 'A') {
			pTempPen = (CPen*)pDC->SelectObject(&_black_pen);
			pTempBrush = (CBrush*)pDC->SelectObject(&_white_brush);
			pDC->SetTextColor(COLOR_BLACK);
		}
		else if (ch == 'T') {
			pTempPen = (CPen*)pDC->SelectObject(&_black_pen);
			pTempBrush = (CBrush*)pDC->SelectObject(&_white_brush);
			pDC->SetTextColor(COLOR_BLUE);
		}
		else if (ch == 'I') {
			pTempPen = (CPen*)pDC->SelectObject(&_black_pen);
			pTempBrush = (CBrush*)pDC->SelectObject(&_white_brush);
			pDC->SetTextColor(COLOR_GREEN);
		}
		else if (ch == 'O') {
			pTempPen = (CPen*)pDC->SelectObject(&_black_pen);
			pTempBrush = (CBrush*)pDC->SelectObject(&_white_brush);
			pDC->SetTextColor(COLOR_YELLOW);
		}
		else if (ch == 'L') {
			pTempPen = (CPen*)pDC->SelectObject(&_black_pen);
			pTempBrush = (CBrush*)pDC->SelectObject(&_white_brush);
			pDC->SetTextColor(COLOR_RED);
		}
		else if (ch == 'P') {
			pTempPen = (CPen*)pDC->SelectObject(&_black_pen);
			pTempBrush = (CBrush*)pDC->SelectObject(&_white_brush);
			pDC->SetTextColor(COLOR_BLACK);
		}
		else {
			// Formerly unreachable code,
			// can now only happen if we add new buttons,
			// but don't handle them here properly.
			pTempPen = (CPen*)pDC->SelectObject(&_white_dot_pen);
			pTempBrush = (CBrush*)pDC->SelectObject(&_gray_brush);
			pDC->SetTextColor(COLOR_WHITE);
		}
	}
	oldpen.FromHandle((HPEN)pTempPen);					// Save old pen
	oldbrush.FromHandle((HBRUSH)pTempBrush);		// Save old brush
	pDC->SetBkMode(OPAQUE);
	pDC->Rectangle(left, top, right, bottom);
	// Set rectangle
	rect.left = left;
	rect.top = top;
	rect.right = right;
	rect.bottom = bottom;
	t.Format("%c", ch);
	pDC->SetBkMode(TRANSPARENT);
	pDC->DrawText(t.GetString(), t.GetLength(), &rect, DT_CENTER | DT_SINGLELINE | DT_VCENTER);
	// Restore original pen and brush
	pDC->SelectObject(oldpen);
	pDC->SelectObject(oldbrush);
	pDC->SelectObject(oldfont);
	cFont.DeleteObject();
	ReleaseDC(pDC);
}

void COpenHoldemView::DrawSeatedActiveCircle(const int chair) {
	CPen		*pTempPen = NULL, oldpen;
	CBrush	*pTempBrush = NULL, oldbrush;
	int			left = 0, top = 0, right = 0, bottom = 0;
	CDC			*pDC = GetDC();

	// Background color
	pDC->SetBkColor(COLOR_GRAY);

	// Figure placement of circle
	left = _client_rect.right * pc[p_tablemap->nchairs()][chair][0] - CARDSIZEX - 6;
	top = _client_rect.bottom * pc[p_tablemap->nchairs()][chair][1] - CARDSIZEX - 5;
	right = _client_rect.right * pc[p_tablemap->nchairs()][chair][0] + CARDSIZEX + 5;
	bottom = _client_rect.bottom * pc[p_tablemap->nchairs()][chair][1] + CARDSIZEX + 5;

	pTempPen = (CPen*)pDC->SelectObject(&_black_pen);
	oldpen.FromHandle((HPEN)pTempPen);					// Save old pen

	if (p_table_state->Player(chair)->active()) {
		pTempBrush = (CBrush*)pDC->SelectObject(&_white_brush);
	}
	else {
		pTempBrush = (CBrush*)pDC->SelectObject(&_gray_brush);
	}
	oldbrush.FromHandle((HBRUSH)pTempBrush);			// Save old brush

	pDC->Ellipse(left, top, right, bottom);

	// Restore original pen and brush
	pDC->SelectObject(oldpen);
	pDC->SelectObject(oldbrush);
	ReleaseDC(pDC);
}

void COpenHoldemView::DrawDealerButton(const int chair) {
	CPen		*pTempPen = NULL, oldpen;
	CBrush	*pTempBrush = NULL, oldbrush;
	int			left = 0, top = 0, right = 0, bottom = 0;
	CDC			*pDC = GetDC();

	// Background color
	pDC->SetBkColor(COLOR_GRAY);

	// Figure placement of circle
	left = _client_rect.right * pc[p_tablemap->nchairs()][chair][0] - 8;
	top = _client_rect.bottom * pc[p_tablemap->nchairs()][chair][1] - 8;
	right = _client_rect.right * pc[p_tablemap->nchairs()][chair][0] + 8;
	bottom = _client_rect.bottom * pc[p_tablemap->nchairs()][chair][1] + 8;

	pTempPen = (CPen*)pDC->SelectObject(&_black_pen);
	oldpen.FromHandle((HPEN)pTempPen);					// Save old pen
	pTempBrush = (CBrush*)pDC->SelectObject(&_red_brush);
	oldbrush.FromHandle((HBRUSH)pTempBrush);			// Save old brush

	pDC->Ellipse(left, top, right, bottom);

	// Restore original pen and brush
	pDC->SelectObject(oldpen);
	pDC->SelectObject(oldbrush);
	ReleaseDC(pDC);
}

void COpenHoldemView::DrawCard(Card *card, const int left, const int top,
	const int right, const int bottom, const bool pl_card) {
	CPen		*pTempPen = NULL, oldpen;
	CBrush	*pTempBrush = NULL, oldbrush;
	RECT		rrect = { 0 }, srect = { 0 };
	CFont		*oldfont = NULL, cFont;
	CDC			*pDC = GetDC();

	// Set font basics
	_logfont.lfHeight = -24;
	_logfont.lfWeight = FW_BOLD;
	cFont.CreateFontIndirect(&_logfont);
	oldfont = pDC->SelectObject(&cFont);

	// Background color
	pDC->SetBkColor(COLOR_GRAY);

	// CARD BACK
	if (card->IsCardBack())
	{
		pTempPen = (CPen*)pDC->SelectObject(&_black_pen);
		oldpen.FromHandle((HPEN)pTempPen);					// Save old pen
		pTempBrush = (CBrush*)pDC->SelectObject(&_yellow_brush);
		oldbrush.FromHandle((HBRUSH)pTempBrush);			// Save old brush

		pDC->SetBkMode(OPAQUE);
		pDC->RoundRect(left, top, right, bottom, 5, 5);

		// Restore original pen and brush
		pDC->SelectObject(oldpen);
		pDC->SelectObject(oldbrush);
	}

	// NO CARD
	else if (card->IsNoCard())
	{
		pTempPen = (CPen*)pDC->SelectObject(&_white_dot_pen);
		oldpen.FromHandle((HPEN)pTempPen);					// Save old pen
		pTempBrush = (CBrush*)pDC->SelectObject(&_gray_brush);
		oldbrush.FromHandle((HBRUSH)pTempBrush);			// Save old brush

		pDC->SetBkMode(OPAQUE);
		pDC->RoundRect(left, top, right, bottom, 5, 5);

		// Restore original pen and brush
		pDC->SelectObject(oldpen);
		pDC->SelectObject(oldbrush);
	}

	// NORMAL CARD
	else
	{
		// Rect for rank and suit location
		rrect.left = left;
		rrect.top = top;
		rrect.right = right;
		rrect.bottom = top + ((bottom - top) / 2);
		srect.left = left;
		srect.top = top + ((bottom - top) / 2) + 1 - (pl_card ? 7 : 0);
		srect.right = right;
		srect.bottom = bottom - (pl_card ? 7 : 0);

		// Draw card rect and suit
		pTempBrush = (CBrush*)pDC->SelectObject(&_white_brush);
		oldbrush.FromHandle((HBRUSH)pTempBrush);			// Save old brush

		// Set colors
		switch (card->GetSuit())
		{
		case Suit_CLUBS:
			pDC->SetTextColor(COLOR_GREEN);
			pTempPen = (CPen*)pDC->SelectObject(&_green_pen);
			break;
		case Suit_DIAMONDS:
			pDC->SetTextColor(COLOR_BLUE);
			pTempPen = (CPen*)pDC->SelectObject(&_blue_pen);
			break;
		case Suit_HEARTS:
			pDC->SetTextColor(COLOR_RED);
			pTempPen = (CPen*)pDC->SelectObject(&_red_pen);
			break;
		case Suit_SPADES:
			pDC->SetTextColor(COLOR_BLACK);
			pTempPen = (CPen*)pDC->SelectObject(&_black_pen);
			break;
		default:
			pTempPen = (CPen*)pDC->GetCurrentPen();
			break;
		}
		oldpen.FromHandle((HPEN)pTempPen);					// Save old pen

		// Draw card and suit
		pDC->SetBkMode(OPAQUE);
		pDC->RoundRect(left, top, right, bottom, 5, 5);
		pDC->SetBkMode(TRANSPARENT);
		// Draw card rank and suit
		pDC->DrawText(CString(card->GetSuitCharacter(true)),
			-1, &srect, DT_CENTER | DT_SINGLELINE | DT_VCENTER);
		pDC->DrawText(CString(card->GetRankCharacter()),
			-1, &rrect, DT_CENTER | DT_SINGLELINE | DT_VCENTER);

		// Restore original pen and brush
		pDC->SelectObject(oldpen);
		pDC->SelectObject(oldbrush);
	}

	pDC->SelectObject(oldfont);
	cFont.DeleteObject();
	ReleaseDC(pDC);
}

void COpenHoldemView::DrawNameBox(const int chair) {
	CPen		*pTempPen = NULL, oldpen;
	CBrush		*pTempBrush = NULL, oldbrush;
	RECT		textrect = { 0 }, drawrect = { 0 };
	CFont		*oldfont = NULL, cFont;
	int			left = 0, top = 0, right = 0, bottom = 0;
	CDC			*pDC = GetDC();
	static RECT	name_rect_last[10] = { 0 };
	// Background color
	pDC->SetBkColor(COLOR_GRAY);
	// Figure placement of box
	left = _client_rect.right * pc[p_tablemap->nchairs()][chair][0] - 36;
	top = _client_rect.bottom * pc[p_tablemap->nchairs()][chair][1] + 15;
	right = _client_rect.right * pc[p_tablemap->nchairs()][chair][0] + 35;
	bottom = _client_rect.bottom * pc[p_tablemap->nchairs()][chair][1] + 30;
	// Set font basics
	_logfont.lfHeight = -12;
	_logfont.lfWeight = FW_NORMAL;
	cFont.CreateFontIndirect(&_logfont);
	oldfont = pDC->SelectObject(&cFont);
	pDC->SetTextColor(COLOR_BLACK);
	if (p_table_state->Player(chair)->seated() || p_table_state->Player(chair)->active()) {
		pTempPen = (CPen*)pDC->SelectObject(&_black_pen);
		oldpen.FromHandle((HPEN)pTempPen);					// Save old pen
		pTempBrush = (CBrush*)pDC->SelectObject(&_white_brush);
		oldbrush.FromHandle((HBRUSH)pTempBrush);			// Save old brush
	// Calc rectangle size for text
		textrect.left = 0;
		textrect.top = 0;
		textrect.right = 0;
		textrect.bottom = 0;
		pDC->DrawText(p_table_state->Player(chair)->name().GetString(),
			p_table_state->Player(chair)->name().GetLength(), &textrect, DT_CALCRECT);
		// Figure out placement of rectangle
		drawrect.left = left < (left + (right - left) / 2) - textrect.right / 2 - 3 ? left : (left + (right - left) / 2) - textrect.right / 2 - 3;
		drawrect.top = top;
		drawrect.right = right > (left + (right - left) / 2) + textrect.right / 2 + 3 ? right : (left + (right - left) / 2) + textrect.right / 2 + 3;
		drawrect.bottom = bottom;
		// Invalidate everything if the name has decreased in width
		if (name_rect_last[chair].right - name_rect_last[chair].left != drawrect.right - drawrect.left) {
			InvalidateRect(NULL, true);
		}
		// Draw it
		pDC->SetBkMode(OPAQUE);
		pDC->Rectangle(drawrect.left, drawrect.top, drawrect.right, drawrect.bottom);
		pDC->SetBkMode(TRANSPARENT);
		pDC->DrawText(p_table_state->Player(chair)->name().GetString(),
			p_table_state->Player(chair)->name().GetLength(), &drawrect,
			DT_CENTER | DT_SINGLELINE | DT_VCENTER);
		name_rect_last[chair].left = drawrect.left;
		name_rect_last[chair].top = drawrect.top;
		name_rect_last[chair].right = drawrect.right;
		name_rect_last[chair].bottom = drawrect.bottom;
	}
	else {
		pTempPen = (CPen*)pDC->SelectObject(&_white_dot_pen);
		oldpen.FromHandle((HPEN)pTempPen);					// Save old pen
		pTempBrush = (CBrush*)pDC->SelectObject(&_gray_brush);
		oldbrush.FromHandle((HBRUSH)pTempBrush);			// Save old brush
		pDC->SetBkMode(OPAQUE);
		pDC->Rectangle(left, top, right, bottom);
	}
	// Restore original pen and brush
	pDC->SelectObject(oldpen);
	pDC->SelectObject(oldbrush);
	pDC->SelectObject(oldfont);
	cFont.DeleteObject();
	ReleaseDC(pDC);
}

void COpenHoldemView::DrawBalanceBox(const int chair) {
	CPen		*pTempPen = NULL, oldpen;
	CBrush	*pTempBrush = NULL, oldbrush;
	RECT		textrect = { 0 }, drawrect = { 0 };
	CFont		*oldfont = NULL, cFont;
	CString	t = "";
	int			left = 0, top = 0, right = 0, bottom = 0;
	CDC			*pDC = GetDC();
	static RECT	balance_rect_last[10] = { 0 };
	// Background color
	pDC->SetBkColor(COLOR_GRAY);
	// Figure placement of box
	left = _client_rect.right * pc[p_tablemap->nchairs()][chair][0] - 26;
	top = _client_rect.bottom * pc[p_tablemap->nchairs()][chair][1] + 30;
	right = _client_rect.right * pc[p_tablemap->nchairs()][chair][0] + 25;
	bottom = _client_rect.bottom * pc[p_tablemap->nchairs()][chair][1] + 45;
	// Set font basics
	_logfont.lfHeight = -12;
	_logfont.lfWeight = FW_NORMAL;
	cFont.CreateFontIndirect(&_logfont);
	oldfont = pDC->SelectObject(&cFont);
	pDC->SetTextColor(COLOR_BLACK);
	if (p_table_state->Player(chair)->seated() || p_table_state->Player(chair)->active()) {
		pTempPen = (CPen*)pDC->SelectObject(&_black_pen);
		oldpen.FromHandle((HPEN)pTempPen);					// Save old pen
		pTempBrush = (CBrush*)pDC->SelectObject(&_white_brush);
		oldbrush.FromHandle((HBRUSH)pTempBrush);			// Save old brush

		// Format Text
		if (p_table_state->Player(chair)->active()) {
			t = Number2CString(p_table_state->Player(chair)->_balance.GetValue());
		}
		else {
			t.Format("Out (%s)", Number2CString(p_table_state->Player(chair)->_balance.GetValue()));
		}
	}
	else {
		pTempPen = (CPen*)pDC->SelectObject(&_white_dot_pen);
		oldpen.FromHandle((HPEN)pTempPen);					// Save old pen
		pTempBrush = (CBrush*)pDC->SelectObject(&_gray_brush);
		oldbrush.FromHandle((HBRUSH)pTempBrush);			// Save old brush
		t = "";
	}
	// Calc rectangle size for text
	textrect.left = 0;
	textrect.top = 0;
	textrect.right = 0;
	textrect.bottom = 0;
	pDC->DrawText(t.GetString(), t.GetLength(), &textrect, DT_CALCRECT);
	// Figure out placement of rectangle
	drawrect.left = left < (left + (right - left) / 2) - textrect.right / 2 - 3 ? left : (left + (right - left) / 2) - textrect.right / 2 - 3;
	drawrect.top = top;
	drawrect.right = right > (left + (right - left) / 2) + textrect.right / 2 + 3 ? right : (left + (right - left) / 2) + textrect.right / 2 + 3;
	drawrect.bottom = bottom;
	// Invalidate everything if the balance has decreased in width
	if (balance_rect_last[chair].right - balance_rect_last[chair].left != drawrect.right - drawrect.left) {
		InvalidateRect(NULL, true);
	}
	// Draw it
	pDC->SetBkMode(OPAQUE);
	pDC->Rectangle(drawrect.left, drawrect.top, drawrect.right, drawrect.bottom);
	pDC->SetBkMode(TRANSPARENT);
	pDC->DrawText(t.GetString(), t.GetLength(), &drawrect, DT_CENTER | DT_SINGLELINE | DT_VCENTER);
	balance_rect_last[chair].left = drawrect.left;
	balance_rect_last[chair].top = drawrect.top;
	balance_rect_last[chair].right = drawrect.right;
	balance_rect_last[chair].bottom = drawrect.bottom;

	// Restore original pen and brush
	pDC->SelectObject(oldpen);
	pDC->SelectObject(oldbrush);
	pDC->SelectObject(oldfont);
	cFont.DeleteObject();
	ReleaseDC(pDC);
}

void COpenHoldemView::DrawPlayerBet(const int chair) {
	CPen		*pTempPen = NULL, oldpen;
	CBrush	*pTempBrush = NULL, oldbrush;
	RECT		textrect = { 0 }, drawrect = { 0 };
	CFont		*oldfont = NULL, cFont;
	CString	t = "";
	static  RECT	bet_rect_last[10] = { 0 };
	int		  xcenter = 0, ycenter = 0, xadj = 0, yadj = 0;
	CDC		 *pDC = GetDC();
	// Draw background colored rectangle over position of previous bet to erase it
	pTempPen = (CPen*)pDC->SelectObject(&_null_pen);
	oldpen.FromHandle((HPEN)pTempPen);					// Save old pen
	pTempBrush = (CBrush*)pDC->SelectObject(&_gray_brush);
	oldbrush.FromHandle((HBRUSH)pTempBrush);			// Save old brush
	pDC->Rectangle(bet_rect_last[chair].left, bet_rect_last[chair].top,
		bet_rect_last[chair].right, bet_rect_last[chair].bottom);
	pDC->SelectObject(oldpen);
	pDC->SelectObject(oldbrush);
	// Background color
	pDC->SetBkColor(COLOR_GRAY);
	// Figure placement
	xcenter = _client_rect.right * pc[p_tablemap->nchairs()][chair][0];
	ycenter = _client_rect.bottom * pc[p_tablemap->nchairs()][chair][1];
	xadj = pcbet[p_tablemap->nchairs()][chair][0];
	yadj = pcbet[p_tablemap->nchairs()][chair][1];
	// Set font basics
	_logfont.lfHeight = -12;
	_logfont.lfWeight = FW_NORMAL;
	cFont.CreateFontIndirect(&_logfont);
	oldfont = pDC->SelectObject(&cFont);
	pDC->SetTextColor(COLOR_BLACK);
	// Format text
	if (p_table_state->Player(chair)->_bet.GetValue() != 0)
	{
		t = Number2CString(p_table_state->Player(chair)->_bet.GetValue());
	}
	else {
		t = "";
	}
	// Calc rectangle size for text
	textrect.left = 0;
	textrect.top = 0;
	textrect.right = 0;
	textrect.bottom = 0;
	pDC->DrawText(t.GetString(), t.GetLength(), &textrect, DT_CALCRECT);
	// Figure out placement of rectangle
	if (xadj < 0) {
		drawrect.left = xcenter + xadj - textrect.right;
		drawrect.top = ycenter + yadj - textrect.bottom / 2;
		drawrect.right = xcenter + xadj;
		drawrect.bottom = ycenter + yadj + textrect.bottom / 2;
	}
	else if (xadj > 0) {
		drawrect.left = xcenter + xadj;
		drawrect.top = ycenter + yadj - textrect.bottom / 2;
		drawrect.right = xcenter + xadj + textrect.right;
		drawrect.bottom = ycenter + yadj + textrect.bottom / 2;
	}
	else	// xadj==0
	{
		drawrect.left = xcenter + xadj - textrect.right / 2;
		drawrect.top = ycenter + yadj - textrect.bottom / 2;
		drawrect.right = xcenter + xadj + textrect.right / 2;
		drawrect.bottom = ycenter + yadj + textrect.bottom / 2;
	}
	// Draw it
	pDC->SetBkMode(OPAQUE);
	pDC->DrawText(t.GetString(), t.GetLength(), &drawrect, DT_CENTER | DT_SINGLELINE | DT_VCENTER);
	bet_rect_last[chair].left = drawrect.left;
	bet_rect_last[chair].top = drawrect.top;
	bet_rect_last[chair].right = drawrect.right;
	bet_rect_last[chair].bottom = drawrect.bottom;

	cFont.DeleteObject();
	ReleaseDC(pDC);
}

void COpenHoldemView::DrawPlayerCards(const int chair) {
	if (!p_table_state->Player(chair)->active()) {
		// Forget about inactive players, they have no cards.
		// Don't draw them to point out the mistake faster
		// for newbies with bad tablemaps.
		return;
	}
	// Get size of current client window
	GetClientRect(&_client_rect);
	// Calculate fixed y-position
	int pos_y_top = _client_rect.bottom * pc[p_tablemap->nchairs()][chair][1] - CARDSIZEY / 2;
	int pos_y_bottom = pos_y_top + CARDSIZEY - 1;
	// x-offset between two cards, with some overlap due ti space restrictions
	int x_offset_to_next_card = CARDSIZEX - 10;
	// Calculate starting position for first card
	int first_pos_x_right = _client_rect.right * pc[p_tablemap->nchairs()][chair][0] + 7;
	if (p_engine_container->symbol_engine_isomaha()->isomaha()) {
		// Original positions were designed for HoldEm.
		// If we play Omaha, then move everything one card to the left
		// for better centralization.
		first_pos_x_right -= x_offset_to_next_card;
	}
	int number_of_cards = NumberOfCardsPerPlayer();
	write_log(Preferences()->debug_alltherest(), "[GUI] Drawing %d player cards for chair %d\n",
		number_of_cards, chair);
	for (int i = 0; i < number_of_cards; ++i) {
		Card *player_card_N = p_table_state->Player(chair)->hole_cards(i);
		int pos_x_right = first_pos_x_right + i * x_offset_to_next_card;
		int pos_x_left = pos_x_right - CARDSIZEX;
		DrawCard(player_card_N, pos_x_left, pos_y_top, pos_x_right, pos_y_bottom, true);
	}
}

void COpenHoldemView::DrawColourCodes(const int chair) {
	if (p_engine_container->symbol_engine_colourcodes() == 0) {
		// Not yet initialized
		return;
	}
	int right = 0;
	// Figure placement of box
	int bottom = _client_rect.bottom * pc[p_tablemap->nchairs()][chair][1] + 16;
	int top = bottom - 10;
	if (chair >= (p_tablemap->nchairs() / 2)) {
		right = _client_rect.right * pc[p_tablemap->nchairs()][chair][0] - 20;
	}
	else {
		right = _client_rect.right * pc[p_tablemap->nchairs()][chair][0] + 31;
	}
	int left = right - 10;
	// Draw it
	CDC	*pDC = GetDC();
	CPen		*pTempPen = NULL, oldpen;
	CBrush	*pTempBrush = NULL, oldbrush;
	pTempPen = (CPen*)pDC->SelectObject(&_black_pen);
	oldpen.FromHandle((HPEN)pTempPen);					// Save old pen
	COLORREF colour_code = p_engine_container->symbol_engine_colourcodes()->ColourCodeToDisplay(chair);
	CBrush mybrush(colour_code);
	pTempBrush = (CBrush*)pDC->SelectObject(mybrush);
	oldbrush.FromHandle((HBRUSH)pTempBrush);			// Save old brush
	pDC->Rectangle(left, top, right, bottom);
	// Restore original pen and brush
	pDC->SelectObject(oldpen);
	pDC->SelectObject(oldbrush);
	ReleaseDC(pDC);
}
