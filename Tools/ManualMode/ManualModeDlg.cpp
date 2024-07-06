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


// ManualModeDlg.cpp : implementation file
//

#include "stdafx.h"
#include <cmath>

#include "ManualModeDlg.h"

#include "MagicNumbers/MagicNumbers.h"
#include "WindowFunctions/window_functions.h"
#include "ManualMode.h"
#include "PokerEval/include/poker_defs.h"
#include "registry.h"
#include "EditDlg.h"
#include "GameInfoDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// Startup path
char	startup_path[MAX_PATH];

// Colors
// (more color codes here: http://htmlhelp.com/cgi-bin/color.cgi)
#undef CARDSIZEX
#undef CARDSIZEY
#undef COLOR_GRAY
#define CARDSIZEX		36
#define	CARDSIZEY		50
#define COLOR_GRAY		RGB(182,182,182)

// Table layouts
int		cc[kNumberOfCommunityCards][2] = 
	{ {-(CARDSIZEX*2 + 3*2 + CARDSIZEX/2), -(CARDSIZEY/2)},	// absolutes
	  {-(CARDSIZEX*1 + 3*1 + CARDSIZEX/2), -(CARDSIZEY/2)}, 
	  {-(CARDSIZEX*0 + 3*0 + CARDSIZEX/2), -(CARDSIZEY/2)}, 
	  {+(CARDSIZEX*0 + 3*1 + CARDSIZEX/2), -(CARDSIZEY/2)}, 
	  {+(CARDSIZEX*1 + 3*2 + CARDSIZEX/2), -(CARDSIZEY/2)} 
	}; 

// Player locations as a percentage of width/height
// [chairnum][x/y]
double pc[kMaxNumberOfPlayers][2] = 
	{ {.68,.11}, {.83,.21}, {.93,.47}, {.83,.73}, {.68,.83}, 
	  {.32,.83}, {.17,.73}, {.07,.47}, {.17,.21}, {.32,.11} 
	};
	
// Player bet locations relative to player locations above
// numbers are in pixel units
// [chairnum][x/y]
// Chairs 2, 3, 6, 7 need special care to avoid overlap
// by Omaha-cards
int pcbet[kMaxNumberOfPlayers][2] = 
	{ {-40,+53}, {-40,+37}, {-70,+0}, {-70,-20}, {-40,-40}, 
	  {+40,-40}, {+50,-20}, {+70,+0}, {+40,+37}, {+40,+53} 
	};

// Dealer button locations relative to player locations above
// numbers are in pixel units
// [chairnum][x/y]
int dbutn[kMaxNumberOfPlayers][2] = 
	{ {-60,+70}, {-60,+54}, {-60,+17}, {-60,-37}, {-60,-57}, 
	  {+60,-57}, {+60,-37}, {+60,+17}, {+60,+54}, {+60,+70} 
	};


CString NumberToFormattedString(double number)
{
	CString result = "";
	if (fabs(number - int(number)) >= 0.01)
	{
		// Floating point with 2 digits
		result.Format("%.2f", number);
	}
	else
	{
		// Integer without digits
		result.Format("%.0f", number);
	}
	return result;
}


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog 
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD) 
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX) 
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CManualModeDlg dialog

CManualModeDlg::CManualModeDlg(CWnd* pParent /*=NULL*/) : CDialog(CManualModeDlg::IDD, pParent) {
  Registry reg;
  reg.read_reg();
  macro_text = reg.macro;
	// Set exception handler
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	black_pen.CreatePen(PS_SOLID, 1, COLOR_BLACK);
	green_pen.CreatePen(PS_SOLID, 1, COLOR_LIME);
	red_pen.CreatePen(PS_SOLID, 1, COLOR_RED);
	blue_pen.CreatePen(PS_SOLID, 1, COLOR_BLUE);
	white_pen.CreatePen(PS_SOLID, 1, COLOR_WHITE);
	white_dot_pen.CreatePen(PS_DOT, 1, COLOR_WHITE);
	null_pen.CreatePen(PS_NULL, 0, COLOR_BLACK);
	white_brush.CreateSolidBrush(COLOR_WHITE);
	gray_brush.CreateSolidBrush(COLOR_GRAY);
	red_brush.CreateSolidBrush(COLOR_RED);
	yellow_brush.CreateSolidBrush(COLOR_YELLOW);
	// big font
	lf.lfHeight = -12;
	lf.lfWeight = FW_NORMAL;
	lf.lfWidth = 0;
	lf.lfEscapement = 0;
	lf.lfOrientation = 0;
	lf.lfItalic = 0;
	lf.lfUnderline = 0;
	lf.lfStrikeOut = 0;
	lf.lfCharSet = 0;
	lf.lfOutPrecision = 0;
	lf.lfClipPrecision = 0;
	lf.lfQuality = PROOF_QUALITY;
	lf.lfPitchAndFamily = 0;
	strcpy_s(lf.lfFaceName, 32, "Verdana");
	cFont.CreateFontIndirect(&lf);
	// small font
	strcpy_s(lf.lfFaceName, 32, "Arial");
	lf.lfHeight = -9;
	cFont_sm.CreateFontIndirect(&lf);
	all_cards.LoadBitmap(IDB_CARDS);
	// Save startup directory
	::GetCurrentDirectory(sizeof(startup_path) - 1, startup_path);
}

void CManualModeDlg::DoDataExchange(CDataExchange* pDX) 
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SWAG, swag);
}

BOOL CManualModeDlg::DestroyWindow() {
	all_cards.DeleteObject();
	cFont.DeleteObject();
	return CDialog::DestroyWindow();
}

BEGIN_MESSAGE_MAP(CManualModeDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_2_CLUBS, &CManualModeDlg::On2Clubs)
	ON_COMMAND(ID_2_DIAMONDS, &CManualModeDlg::On2Diamonds)
	ON_COMMAND(ID_2_HEARTS, &CManualModeDlg::On2Hearts)
	ON_COMMAND(ID_2_SPADES, &CManualModeDlg::On2Spades)
	ON_COMMAND(ID_3_CLUBS, &CManualModeDlg::On3Clubs)
	ON_COMMAND(ID_3_DIAMONDS, &CManualModeDlg::On3Diamonds)
	ON_COMMAND(ID_3_HEARTS, &CManualModeDlg::On3Hearts)
	ON_COMMAND(ID_3_SPADES, &CManualModeDlg::On3Spades)
	ON_COMMAND(ID_4_CLUBS, &CManualModeDlg::On4Clubs)
	ON_COMMAND(ID_4_DIAMONDS, &CManualModeDlg::On4Diamonds)
	ON_COMMAND(ID_4_HEARTS, &CManualModeDlg::On4Hearts)
	ON_COMMAND(ID_4_SPADES, &CManualModeDlg::On4Spades)
	ON_COMMAND(ID_5_CLUBS, &CManualModeDlg::On5Clubs)
	ON_COMMAND(ID_5_DIAMONDS, &CManualModeDlg::On5Diamonds)
	ON_COMMAND(ID_5_HEARTS, &CManualModeDlg::On5Hearts)
	ON_COMMAND(ID_5_SPADES, &CManualModeDlg::On5Spades)
	ON_COMMAND(ID_6_CLUBS, &CManualModeDlg::On6Clubs)
	ON_COMMAND(ID_6_DIAMONDS, &CManualModeDlg::On6Diamonds)
	ON_COMMAND(ID_6_HEARTS, &CManualModeDlg::On6Hearts)
	ON_COMMAND(ID_6_SPADES, &CManualModeDlg::On6Spades)
	ON_COMMAND(ID_7_CLUBS, &CManualModeDlg::On7Clubs)
	ON_COMMAND(ID_7_DIAMONDS, &CManualModeDlg::On7Diamonds)
	ON_COMMAND(ID_7_HEARTS, &CManualModeDlg::On7Hearts)
	ON_COMMAND(ID_7_SPADES, &CManualModeDlg::On7Spades)
	ON_COMMAND(ID_8_CLUBS, &CManualModeDlg::On8Clubs)
	ON_COMMAND(ID_8_DIAMONDS, &CManualModeDlg::On8Diamonds)
	ON_COMMAND(ID_8_HEARTS, &CManualModeDlg::On8Hearts)
	ON_COMMAND(ID_8_SPADES, &CManualModeDlg::On8Spades)
	ON_COMMAND(ID_9_CLUBS, &CManualModeDlg::On9Clubs)
	ON_COMMAND(ID_9_DIAMONDS, &CManualModeDlg::On9Diamonds)
	ON_COMMAND(ID_9_HEARTS, &CManualModeDlg::On9Hearts)
	ON_COMMAND(ID_9_SPADES, &CManualModeDlg::On9Spades)
	ON_COMMAND(ID_T_CLUBS, &CManualModeDlg::OnTClubs)
	ON_COMMAND(ID_T_DIAMONDS, &CManualModeDlg::OnTDiamonds)
	ON_COMMAND(ID_T_HEARTS, &CManualModeDlg::OnTHearts)
	ON_COMMAND(ID_T_SPADES, &CManualModeDlg::OnTSpades)
	ON_COMMAND(ID_J_CLUBS, &CManualModeDlg::OnJClubs)
	ON_COMMAND(ID_J_DIAMONDS, &CManualModeDlg::OnJDiamonds)
	ON_COMMAND(ID_J_HEARTS, &CManualModeDlg::OnJHearts)
	ON_COMMAND(ID_J_SPADES, &CManualModeDlg::OnJSpades)
	ON_COMMAND(ID_Q_CLUBS, &CManualModeDlg::OnQClubs)
	ON_COMMAND(ID_Q_DIAMONDS, &CManualModeDlg::OnQDiamonds)
	ON_COMMAND(ID_Q_HEARTS, &CManualModeDlg::OnQHearts)
	ON_COMMAND(ID_Q_SPADES, &CManualModeDlg::OnQSpades)
	ON_COMMAND(ID_K_CLUBS, &CManualModeDlg::OnKClubs)
	ON_COMMAND(ID_K_DIAMONDS, &CManualModeDlg::OnKDiamonds)
	ON_COMMAND(ID_K_HEARTS, &CManualModeDlg::OnKHearts)
	ON_COMMAND(ID_K_SPADES, &CManualModeDlg::OnKSpades)
	ON_COMMAND(ID_A_CLUBS, &CManualModeDlg::OnAClubs)
	ON_COMMAND(ID_A_DIAMONDS, &CManualModeDlg::OnADiamonds)
	ON_COMMAND(ID_A_HEARTS, &CManualModeDlg::OnAHearts)
	ON_COMMAND(ID_A_SPADES, &CManualModeDlg::OnASpades)
	ON_COMMAND(ID_CARDBACKS, &CManualModeDlg::OnCardBacks)
	ON_COMMAND(ID_NOCARDS, &CManualModeDlg::OnNoCards)
	ON_COMMAND(ID_NOCARD, &CManualModeDlg::OnNoCard)
	ON_COMMAND(ID_SEATPLAYER, &CManualModeDlg::OnSeatPlayer)
	ON_COMMAND(ID_SITIN, &CManualModeDlg::OnSitInPlayer)
	ON_COMMAND(ID_DEALERHERE, &CManualModeDlg::OnDealerHere)
	ON_COMMAND(ID_FOLD, &CManualModeDlg::OnFold)
	ON_COMMAND(ID_CALL, &CManualModeDlg::OnCall)
	ON_COMMAND(ID_RAISE, &CManualModeDlg::OnRaise)
	ON_COMMAND(ID_ALLIN, &CManualModeDlg::OnAllin)
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONUP()
	ON_BN_CLICKED(IDC_RESET, &CManualModeDlg::OnBnClickedReset)
	ON_BN_CLICKED(IDC_PMINUS, &CManualModeDlg::OnBnClickedPminus)
	ON_BN_CLICKED(IDC_PPLUS, &CManualModeDlg::OnBnClickedPplus)
	ON_BN_CLICKED(IDC_MACRO, &CManualModeDlg::OnBnClickedMacro)
	ON_BN_CLICKED(IDC_DMINUS, &CManualModeDlg::OnBnClickedDminus)
	ON_BN_CLICKED(IDC_DPLUS, &CManualModeDlg::OnBnClickedDplus)
	ON_EN_CHANGE(IDC_SWAG, &CManualModeDlg::OnEnChangeSwag)
END_MESSAGE_MAP()

// CManualModeDlg message handlers
void CManualModeDlg::clear_scrape_areas(void) 
{
	for (int i=0; i<kNumberOfCommunityCards; i++) 
	{ 
		card[CC0+i] = CARD_NOCARD;
	}
	for (int i=0; i<kMaxNumberOfPlayers; i++) 
	{ 
    SetPlayerCard(i, 0, CARD_NOCARD);
    SetPlayerCard(i, 1, CARD_NOCARD);
    SetPlayerCard(i, 2, CARD_NOCARD);
    SetPlayerCard(i, 3, CARD_NOCARD);
		seated[i] = active[i] = dealer[i] = false;
		playerbet[i] = "0";
		playerbalance[i] = "1000";
		buttonstate[i] = false;
	}
	pot = 0;
	dealer[0] = true;

	playername[0] = "Player0";
	playername[1] = "Player1";
	playername[2] = "Player2";
	playername[3] = "Player3";
	playername[4] = "Player4";
	playername[5] = "Player5";
	playername[6] = "Player6";
	playername[7] = "Player7";
	playername[8] = "Player8";
	playername[9] = "Player9";

	//istournament = false;

	//handnumber = "1";
	//sblind = "5";
	//bblind = "10";
	//bbet = "20";
	//ante = "0";
	//limit = LIMIT_FL;

	ncommoncards_last = 0;

	CardMask_RESET(used_cards);
}

BOOL CManualModeDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);

		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// Restore window location and size
	SetWindowPos(NULL, 0, 0, MM_WIDTH, MM_HEIGHT, SWP_NOCOPYBITS);
  ResizeToClientSize(this->GetSafeHwnd(), 574, 335);
	
	clear_scrape_areas();
	istournament = false;
  isomaha = false;
	handnumber = "1";
	sblind = "5";
	bblind = "10";
	bbet = "20";
	ante = "0";
	limit = LIMIT_NL;
	network = "Stars";

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CManualModeDlg::OnSysCommand(UINT nID, LPARAM lParam) 
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)	
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else 
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

int CManualModeDlg::PlayerCardLeft(int chair, int index) {
  RECT cr;
  GetClientRect(&cr);
  const int x_offset = CARDSIZEX - 14;
  int x_position = cr.right * pc[chair][0] - CARDSIZEX - 2 + index * x_offset;
  if (IsOmaha()) {
    // Original positions were designed for HoldEm
    // Shift everything one card to the left 
    // for better centralization and less overlap
    x_position -= x_offset;
  }
  if (chair == 7) {
    // Chair 7 (9 o'clock position on the very left)
    // needs to be shifted, otherwise one card would be invisible
    x_position += x_offset;
  }
  return x_position;
}

int CManualModeDlg::PlayerCardTop(int chair) {
  RECT cr;
  GetClientRect(&cr);
  return cr.bottom * pc[chair][1] - CARDSIZEY / 2 - 5;
}

bool CManualModeDlg::IsOmaha() {
  return isomaha;
}

int CManualModeDlg::NumberOfCardsPerPlayer() {
  if (IsOmaha()) {
    return kNumberOfCardsPerPlayerOmaha;
  }
  return kNumberOfCardsPerPlayerHoldEm;
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CManualModeDlg::OnPaint() {
  CPaintDC dc(this); // device context for painting
	if (IsIconic()) {
		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else 
	{
		CDialog::OnPaint();

		RECT		cr;
		// Get size of current client window
		GetClientRect(&cr);

		// Set background color (light gray)
		CBrush* pOldBrush = dc.SelectObject(&gray_brush);
		dc.PatBlt(cr.left, cr.top, cr.right-cr.left, cr.bottom-cr.top, PATCOPY);
		dc.SelectObject(pOldBrush);

    // Draw collection of player info
    // Very early to avoid overwriting of other info
    // if we play 4-card-HoldEm, which needs more space
    for (int i = 0; i < kMaxNumberOfPlayers; ++i)
    {
      // We want to draw chair 6 first and 5 last
      // due to some overlap of cards for Omaha
      int chair = (6 + i) % kMaxNumberOfPlayers;
      // Draw active circle
      if (seated[chair])
        draw_seated_active_circle(chair);

      // Draw player cards      
      for (int j = 0; j < NumberOfCardsPerPlayer(); ++j) {
        draw_card(GetPlayerCard(chair, j), PlayerCardLeft(chair, j), PlayerCardTop(chair));
      }
      // Draw dealer button
      if (dealer[chair])
        draw_dealer_button(chair);

      // Draw name and balance boxes
      draw_name_box(chair);
      draw_balance_box(chair);

      // Draw player bet
      draw_player_bet(chair);
    }

		// Draw button state indicators
		draw_button_indicators();

		// Draw button state indicators
		draw_control_indicators();

		// Draw center info box
		draw_center_info_box();

		// Set visibility of swag text edit
		if (limit == LIMIT_FL)
			swag.ShowWindow(false);
		else
			swag.ShowWindow(true);

		int shift = 0;
		if (card[CC0+0] == CARD_NOCARD || card[CC0+1] == CARD_NOCARD || card[CC0+2] == CARD_NOCARD)
			shift = 10;
		else if (card[CC0+3] == CARD_NOCARD && card[CC0+4] != CARD_NOCARD)
			shift = 10;

		// Draw common cards
		for (int i=0; i<kNumberOfCommunityCards; i++) 
		{
			draw_card(card[CC0+i], cr.right/2 + cc[i][0] , cr.bottom/2 + cc[i][1], shift);
		}
		// Draw title text
		CString t;
		if (istournament || atof(ante) > 0.001) {
			t.Format("OpenHoldem Poker - %s%s - blinds %s/%s - ante %s",
				(limit == LIMIT_NL ? "No Limit" : limit == LIMIT_PL ? "Pot Limit" : limit == LIMIT_FL ? "Fixed Limit" : "?L"), 
				(istournament ? " tourney" : ""), 
				sblind, bblind, ante);
		}
		else {
			t.Format("OpenHoldem Poker - %s - blinds %s/%s",
				(limit == LIMIT_NL ? "No Limit" : limit == LIMIT_PL ? "Pot Limit" : limit == LIMIT_FL ? "Fixed Limit" : "?L"), 
				sblind, bblind);
		}
		SetWindowText(t);
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CManualModeDlg::OnQueryDragIcon() 
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CManualModeDlg::draw_button_indicators(void) 
{
	bool		fold_drawn, call_drawn, check_drawn, raise_drawn, allin_drawn;
	RECT		cr;
    
	// Get size of current client window
	GetClientRect(&cr);

	fold_drawn = call_drawn = check_drawn = raise_drawn = allin_drawn = false;
	if (buttonstate[0]) 
	{
		draw_specific_button_indicator(0, 'F', cr.right-84, cr.bottom-16, cr.right-70, cr.bottom-2);
		fold_drawn = true;
	}
	if (buttonstate[1]) 
	{
		draw_specific_button_indicator(1, 'C', cr.right-67, cr.bottom-16, cr.right-53, cr.bottom-2);
		call_drawn = true;
	}
	if (buttonstate[2]) 
	{
		draw_specific_button_indicator(2, 'K', cr.right-50, cr.bottom-16, cr.right-36, cr.bottom-2);
		check_drawn = true;
	}
	if (buttonstate[3]) 
	{
		draw_specific_button_indicator(3, 'R', cr.right-33, cr.bottom-16, cr.right-19, cr.bottom-2);
		raise_drawn = true;
	}
	if (buttonstate[4]) 
	{
		draw_specific_button_indicator(4, 'A', cr.right-16, cr.bottom-16, cr.right-2, cr.bottom-2);
		allin_drawn = true;
	}

	if (!fold_drawn) 
		draw_specific_button_indicator(-1, 'F', cr.right-84, cr.bottom-16, cr.right-70, cr.bottom-2);

	if (!call_drawn) 
		draw_specific_button_indicator(-1, 'C', cr.right-67, cr.bottom-16, cr.right-53, cr.bottom-2);

	if (!check_drawn)
		draw_specific_button_indicator(-1, 'K', cr.right-50, cr.bottom-16, cr.right-36, cr.bottom-2);

	if (!raise_drawn)
		draw_specific_button_indicator(-1, 'R', cr.right-33, cr.bottom-16, cr.right-19, cr.bottom-2);

	if (!allin_drawn)
		draw_specific_button_indicator(-1, 'A', cr.right-16, cr.bottom-16, cr.right-2, cr.bottom-2);
}

void CManualModeDlg::draw_specific_button_indicator(int button_num, char ch, int left, int top, int right, int bottom) 
{
	CPen		*pTempPen, oldpen;
	CBrush		*pTempBrush, oldbrush;	
	RECT		rect;
	CFont		*oldfont;
	CString		t;
	CDC			*pDC = GetDC();

	// Set font basics
	oldfont = pDC->SelectObject(&cFont);

	// Background color
	pDC->SetBkColor(COLOR_GRAY);

	if (button_num == -1) 
	{
		pTempPen = (CPen*)pDC->SelectObject(&white_dot_pen);
		pTempBrush = (CBrush*)pDC->SelectObject(&gray_brush);
		pDC->SetTextColor(COLOR_WHITE);
	}
	else 
	{
		if (buttonstate[button_num]) 
		{
			if (ch=='F') 
			{
				pTempPen = (CPen*)pDC->SelectObject(&red_pen);
				pTempBrush = (CBrush*)pDC->SelectObject(&white_brush);
				pDC->SetTextColor(COLOR_RED);
			}
			else if (ch=='C') 
			{
				pTempPen = (CPen*)pDC->SelectObject(&blue_pen);
				pTempBrush = (CBrush*)pDC->SelectObject(&white_brush);
				pDC->SetTextColor(COLOR_BLUE);
			}
			else if (ch=='K') 
			{
				pTempPen = (CPen*)pDC->SelectObject(&blue_pen);
				pTempBrush = (CBrush*)pDC->SelectObject(&white_brush);
				pDC->SetTextColor(COLOR_BLUE);
			}
			else if (ch=='R') 
			{
				pTempPen = (CPen*)pDC->SelectObject(&green_pen);
				pTempBrush = (CBrush*)pDC->SelectObject(&white_brush);
				pDC->SetTextColor(COLOR_LIME);
			}
			else if (ch=='A') 
			{
				pTempPen = (CPen*)pDC->SelectObject(&black_pen);
				pTempBrush = (CBrush*)pDC->SelectObject(&white_brush);
				pDC->SetTextColor(COLOR_BLACK);
			}
		}
		else 
		{
			pTempPen = (CPen*)pDC->SelectObject(&white_dot_pen);
			pTempBrush = (CBrush*)pDC->SelectObject(&gray_brush);
			pDC->SetTextColor(COLOR_WHITE);
		}
	}

	oldpen.FromHandle((HPEN)pTempPen);					// Save old pen
	oldbrush.FromHandle((HBRUSH)pTempBrush);			// Save old brush

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
	ReleaseDC(pDC);
}

void CManualModeDlg::draw_control_indicators(void)
{
	CPen		*pTempPen, oldpen;
	CBrush		*pTempBrush, oldbrush;	
	CFont		*oldfont;
	CString		t;
	CDC			*pDC = GetDC();
	RECT		cr, rect, textrect;

	// Get size of current client window
	GetClientRect(&cr);

	// Background color
	pDC->SetBkColor(COLOR_GRAY);

	//
	// Sit Out button
	//
	pTempPen = (CPen*)pDC->SelectObject(&black_pen);
	pTempBrush = (CBrush*)pDC->SelectObject(&white_brush);
	pDC->SetTextColor(COLOR_BLACK);
	pDC->SetBkMode(OPAQUE);
	pDC->Rectangle(cr.left+2, cr.bottom-48, cr.left+12, cr.bottom-38);

	// Place X if needed
	if (buttonstate[5])
	{
		t = "X";
		rect.left = cr.left + 2;
		rect.top = cr.bottom - 48;
		rect.right = cr.left + 12;
		rect.bottom = cr.bottom - 38;
		pDC->SetBkMode(TRANSPARENT);
		oldfont = pDC->SelectObject(&cFont);
		pDC->DrawText(t.GetString(), t.GetLength(), &rect, DT_CENTER | DT_SINGLELINE | DT_VCENTER);
		pDC->SelectObject(oldfont);
	}

	// Calc rectangle size for label text
	t = "Sit Out";
	textrect.left = 0;
	textrect.top = 0;
	textrect.right = 0;
	textrect.bottom = 0;

	oldfont = pDC->SelectObject(&cFont_sm);

	pDC->DrawText(t.GetString(), t.GetLength(), &textrect, DT_CALCRECT);

	// Draw the label text
	rect.left = 14 + textrect.left;
	rect.top = cr.bottom - 48;
	rect.right = 14 + textrect.right;
	rect.bottom = cr.bottom - 48 + textrect.bottom;

	pDC->SetBkMode(TRANSPARENT);
	pDC->DrawText(t.GetString(), t.GetLength(), &rect, DT_CENTER | DT_SINGLELINE | DT_VCENTER);

	pDC->SelectObject(oldfont);


	//
	// Leave table button
	//
	pTempPen = (CPen*)pDC->SelectObject(&black_pen);
	pTempBrush = (CBrush*)pDC->SelectObject(&white_brush);
	pDC->SetTextColor(COLOR_BLACK);
	pDC->SetBkMode(OPAQUE);
	pDC->Rectangle(cr.left+2, cr.bottom-36, cr.left+12, cr.bottom-26);

	// Place X if needed
	if (buttonstate[6])
	{
		t = "X";
		rect.left = cr.left + 2;
		rect.top = cr.bottom - 36;
		rect.right = cr.left + 12;
		rect.bottom = cr.bottom - 26;
		pDC->SetBkMode(TRANSPARENT);
		oldfont = pDC->SelectObject(&cFont);
		pDC->DrawText(t.GetString(), t.GetLength(), &rect, DT_CENTER | DT_SINGLELINE | DT_VCENTER);
		pDC->SelectObject(oldfont);
	}

	// Calc rectangle size for label text
	t = "Leave Table";
	textrect.left = 0;
	textrect.top = 0;
	textrect.right = 0;
	textrect.bottom = 0;

	oldfont = pDC->SelectObject(&cFont_sm);

	pDC->DrawText(t.GetString(), t.GetLength(), &textrect, DT_CALCRECT);

	// Draw the label text
	rect.left = 14 + textrect.left;
	rect.top = cr.bottom - 36;
	rect.right = 14 + textrect.right;
	rect.bottom = cr.bottom - 36 + textrect.bottom;

	pDC->SetBkMode(TRANSPARENT);
	pDC->DrawText(t.GetString(), t.GetLength(), &rect, DT_CENTER | DT_SINGLELINE | DT_VCENTER);

	pDC->SelectObject(oldfont);


	//
	// Pre-Fold button
	//
	pTempPen = (CPen*)pDC->SelectObject(&black_pen);
	pTempBrush = (CBrush*)pDC->SelectObject(&white_brush);
	pDC->SetTextColor(COLOR_BLACK);
	pDC->SetBkMode(OPAQUE);
	pDC->Rectangle(cr.left+2, cr.bottom-24, cr.left+12, cr.bottom-14);

	// Place X if needed
	if (buttonstate[7])
	{
		t = "X";
		rect.left = cr.left + 2;
		rect.top = cr.bottom - 24;
		rect.right = cr.left + 12;
		rect.bottom = cr.bottom - 14;
		pDC->SetBkMode(TRANSPARENT);
		oldfont = pDC->SelectObject(&cFont);
		pDC->DrawText(t.GetString(), t.GetLength(), &rect, DT_CENTER | DT_SINGLELINE | DT_VCENTER);
		pDC->SelectObject(oldfont);
	}

	// Calc rectangle size for label text
	t = "Fold to any bet (prefold)";
	textrect.left = 0;
	textrect.top = 0;
	textrect.right = 0;
	textrect.bottom = 0;

	oldfont = pDC->SelectObject(&cFont_sm);

	pDC->DrawText(t.GetString(), t.GetLength(), &textrect, DT_CALCRECT);

	// Draw the label text
	rect.left = 14 + textrect.left;
	rect.top = cr.bottom - 24;
	rect.right = 14 + textrect.right;
	rect.bottom = cr.bottom - 24 + textrect.bottom;

	pDC->SetBkMode(TRANSPARENT);
	pDC->DrawText(t.GetString(), t.GetLength(), &rect, DT_CENTER | DT_SINGLELINE | DT_VCENTER);

	pDC->SelectObject(oldfont);


	//
	// Auto post blinds button
	//
	pTempPen = (CPen*)pDC->SelectObject(&black_pen);
	pTempBrush = (CBrush*)pDC->SelectObject(&white_brush);
	pDC->SetTextColor(COLOR_BLACK);
	pDC->SetBkMode(OPAQUE);
	pDC->Rectangle(cr.left+2, cr.bottom-12, cr.left+12, cr.bottom-2);

	// Place X if needed
	if (buttonstate[8])
	{
		t = "X";
		rect.left = cr.left + 2;
		rect.top = cr.bottom - 12;
		rect.right = cr.left + 12;
		rect.bottom = cr.bottom - 2;
		pDC->SetBkMode(TRANSPARENT);
		oldfont = pDC->SelectObject(&cFont);
		pDC->DrawText(t.GetString(), t.GetLength(), &rect, DT_CENTER | DT_SINGLELINE | DT_VCENTER);
		pDC->SelectObject(oldfont);
	}

	// Calc rectangle size for label text
	t = "Automatically post blinds";
	textrect.left = 0;
	textrect.top = 0;
	textrect.right = 0;
	textrect.bottom = 0;

	oldfont = pDC->SelectObject(&cFont_sm);

	pDC->DrawText(t.GetString(), t.GetLength(), &textrect, DT_CALCRECT);

	// Draw the label text
	rect.left = 14 + textrect.left;
	rect.top = cr.bottom - 12;
	rect.right = 14 + textrect.right;
	rect.bottom = cr.bottom - 12 + textrect.bottom;

	pDC->SetBkMode(TRANSPARENT);
	pDC->DrawText(t.GetString(), t.GetLength(), &rect, DT_CENTER | DT_SINGLELINE | DT_VCENTER);

	pDC->SelectObject(oldfont);


	// Restore original pen, brush and font
	pDC->SelectObject(oldpen);
	pDC->SelectObject(oldbrush);
	pDC->SelectObject(oldfont);
	ReleaseDC(pDC);
}

void CManualModeDlg::draw_center_info_box(void) 
{
	CPen		*pTempPen, oldpen;
	CBrush		*pTempBrush, oldbrush;	
	RECT		rect;
	CString		t, s;
	CFont		*oldfont;
	RECT		cr;
	int			left, top, right, bottom;
	CDC			*pDC = GetDC();
	
	// Get size of current client window
	GetClientRect(&cr);

	// Figure placement of box
	left = cr.right/2-65;
	top = 4;
	right = cr.right/2+45;
	bottom = 78;

	pTempPen = (CPen*)pDC->SelectObject(&black_pen);
	oldpen.FromHandle((HPEN)pTempPen);					// Save old pen
	pTempBrush = (CBrush*)pDC->SelectObject(&white_brush);
	oldbrush.FromHandle((HBRUSH)pTempBrush);			// Save old brush

	pDC->SetBkMode(OPAQUE);
	pDC->Rectangle(left, top, right, bottom);

	// Set font basics
	oldfont = pDC->SelectObject(&cFont);
	pDC->SetTextColor(COLOR_BLACK);

	// Set rectangle
	rect.left = left;
	rect.top = top;
	rect.right = right;
	rect.bottom = bottom;

	t = "";
	// handnumber
	s.Format(" Hand #: %s\n", handnumber);
	t.Append(s);

	// blinds/type
	s.Format(" %s%s %s/%s/%s\n", 
			(limit == LIMIT_NL ? "NL" : limit == LIMIT_PL ? "PL" : limit == LIMIT_FL ? "FL" : "?L"), 
			(istournament ? "T" : ""), 
			sblind, bblind, bbet);
	t.Append(s);

	// ante
	s.Format(" Ante: %s\n", ante);
	t.Append(s);

	// Pot
	s.Format(" Pot: %s\n", NumberToFormattedString(pot));
	t.Append(s);

	s.Format(" Net: %s\n", network);
	t.Append(s);

	// Draw it
	pDC->SetBkMode(TRANSPARENT);
	pDC->DrawText(t.GetString(), t.GetLength(), &rect, NULL);

	// Restore original pen, brush and font
	pDC->SelectObject(oldpen);
	pDC->SelectObject(oldbrush);
	pDC->SelectObject(oldfont);
	ReleaseDC(pDC);
}

void CManualModeDlg::draw_seated_active_circle(int chair) 
{
	CPen		*pTempPen, oldpen;
	CBrush		*pTempBrush, oldbrush;	
	RECT		cr;
	int			left, top, right, bottom;
	CDC			*pDC = GetDC();
	
	// Get size of current client window
	GetClientRect(&cr);

	// Background color
	pDC->SetBkColor(COLOR_GRAY);

	// Figure placement of circle
	left = cr.right * pc[chair][0] - CARDSIZEX - 4;
	top = cr.bottom * pc[chair][1] - CARDSIZEX - 3;
	right = cr.right * pc[chair][0] + CARDSIZEX + 3;
	bottom = cr.bottom * pc[chair][1] + CARDSIZEX + 3;

	pTempPen = (CPen*)pDC->SelectObject(&black_pen);
	oldpen.FromHandle((HPEN)pTempPen);					// Save old pen

	if (active[chair]) 
	{
		pTempBrush = (CBrush*)pDC->SelectObject(&white_brush);
	}
	else 
	{
		pTempBrush = (CBrush*)pDC->SelectObject(&gray_brush);
	}
	oldbrush.FromHandle((HBRUSH)pTempBrush);			// Save old brush

	pDC->Ellipse(left, top, right, bottom);

	// Restore original pen and brush
	pDC->SelectObject(oldpen);
	pDC->SelectObject(oldbrush);
	ReleaseDC(pDC);
}

void CManualModeDlg::draw_dealer_button(int chair) 
{
	CPen		*pTempPen, oldpen;
	CBrush		*pTempBrush, oldbrush;	
	RECT		cr;
	int			left, top, right, bottom;
	CDC			*pDC = GetDC();
	
	// Get size of current client window
	GetClientRect(&cr);

	// Background color
	pDC->SetBkColor(COLOR_GRAY);

	// Figure placement of circle
	left = cr.right * pc[chair][0] + dbutn[chair][0] - 8;
	top = cr.bottom * pc[chair][1] + dbutn[chair][1] - 8;
	right = cr.right * pc[chair][0] + dbutn[chair][0] + 8;
	bottom = cr.bottom * pc[chair][1] + dbutn[chair][1] + 8;

	pTempPen = (CPen*)pDC->SelectObject(&black_pen);
	oldpen.FromHandle((HPEN)pTempPen);					// Save old pen
	pTempBrush = (CBrush*)pDC->SelectObject(&red_brush);
	oldbrush.FromHandle((HBRUSH)pTempBrush);			// Save old brush

	pDC->Ellipse(left, top, right, bottom);

	// Restore original pen and brush
	pDC->SelectObject(oldpen);
	pDC->SelectObject(oldbrush);
	ReleaseDC(pDC);
}

void CManualModeDlg::draw_card(unsigned int card, int left, int top, int shift) 
{
	CPen		*pTempPen, oldpen;
	CBrush		*pTempBrush, oldbrush;	
	CDC			*pDC = GetDC();
	CDC			compatDC;
	CBitmap		one_card;
	int			suitoffset, rankoffset;

	// Setup DCs
	one_card.CreateCompatibleBitmap(pDC, CARDSIZEX, CARDSIZEY);
	compatDC.CreateCompatibleDC(pDC);
	compatDC.SelectObject(all_cards);
	pDC->SelectObject(one_card);

	// CARD BACK
	if (card == CARD_BACK) 
	{
		pDC->BitBlt(left, top, CARDSIZEX, CARDSIZEY, &compatDC, 0, CARDSIZEY*4, SRCCOPY);
	}

	// NO CARD
	else if (card == CARD_NOCARD) 
	{
		pDC->SetBkColor(COLOR_GRAY);

		pTempPen = (CPen*)pDC->SelectObject(&white_dot_pen);
		oldpen.FromHandle((HPEN)pTempPen);					// Save old pen
		pTempBrush = (CBrush*)pDC->SelectObject(&gray_brush);
		oldbrush.FromHandle((HBRUSH)pTempBrush);			// Save old brush

		pDC->SetBkMode(OPAQUE);
		pDC->RoundRect(left, top, left+CARDSIZEX, top+CARDSIZEY, 5, 5);

		pDC->SelectObject(oldpen);
		pDC->SelectObject(oldbrush);	
	}

	// NORMAL CARD
	else 
	{
		rankoffset = (12-StdDeck_RANK(card)) * CARDSIZEX;
		suitoffset = StdDeck_SUIT(card) == Suit_SPADES ? CARDSIZEY*0 :
					 StdDeck_SUIT(card) == Suit_HEARTS ? CARDSIZEY*1 :
					 StdDeck_SUIT(card) == Suit_DIAMONDS ? CARDSIZEY*2 :
					 StdDeck_SUIT(card) == Suit_CLUBS ? CARDSIZEY*3 : 0;
		pDC->BitBlt(left, top + shift, CARDSIZEX, CARDSIZEY, &compatDC, rankoffset, suitoffset, SRCCOPY);
	}

	one_card.DeleteObject();
	compatDC.DeleteDC();
	ReleaseDC(pDC);
}


void CManualModeDlg::draw_name_box(int chair) 
{
	CPen		*pTempPen, oldpen;
	CBrush		*pTempBrush, oldbrush;	
	RECT		textrect, drawrect;
	CFont		*oldfont;
	RECT		cr;
	int			left, top, right, bottom;
	CDC			*pDC = GetDC();

	// Get size of current client window
	GetClientRect(&cr);

	// Background color
	pDC->SetBkColor(COLOR_GRAY);

	// Set font basics
	oldfont = pDC->SelectObject(&cFont);
	pDC->SetTextColor(COLOR_BLACK);

	// Figure placement of box
	left = cr.right * pc[chair][0] - 36;
	top = cr.bottom * pc[chair][1] + 15;
	right = cr.right * pc[chair][0] + 35;
	bottom = cr.bottom * pc[chair][1] + 30;

	if (seated[chair] || active[chair]) 
	{
		pTempPen = (CPen*)pDC->SelectObject(&black_pen);
		oldpen.FromHandle((HPEN)pTempPen);					// Save old pen
		pTempBrush = (CBrush*)pDC->SelectObject(&white_brush);
		oldbrush.FromHandle((HBRUSH)pTempBrush);			// Save old brush

		// Calc rectangle size for text
		textrect.left = 0;
		textrect.top = 0;
		textrect.right = 0;
		textrect.bottom = 0;
		pDC->DrawText(playername[chair].GetString(), playername[chair].GetLength(), &textrect, DT_CALCRECT);

		// Figure out placement of rectangle
		drawrect.left = left < (left+(right-left)/2)-textrect.right/2-3 ? left : (left+(right-left)/2)-textrect.right/2-3;
		drawrect.top = top;
		drawrect.right = right > (left+(right-left)/2)+textrect.right/2+3 ? right : (left+(right-left)/2)+textrect.right/2+3;
		drawrect.bottom = bottom;

		// Draw it		
		pDC->SetBkMode(OPAQUE);
		pDC->Rectangle(drawrect.left, drawrect.top, drawrect.right, drawrect.bottom+2);
		pDC->SetBkMode(TRANSPARENT);
		pDC->DrawText(playername[chair].GetString(), playername[chair].GetLength(), &drawrect, 
			DT_CENTER | DT_SINGLELINE | DT_VCENTER);		
	}
	else 
	{
		pTempPen = (CPen*)pDC->SelectObject(&white_dot_pen);
		oldpen.FromHandle((HPEN)pTempPen);					// Save old pen
		pTempBrush = (CBrush*)pDC->SelectObject(&gray_brush);
		oldbrush.FromHandle((HBRUSH)pTempBrush);			// Save old brush

		pDC->SetBkMode(OPAQUE);
		pDC->Rectangle(left, top, right, bottom+2);
	}

	// Restore original pen and brush
	pDC->SelectObject(oldpen);
	pDC->SelectObject(oldbrush);
	pDC->SelectObject(oldfont);
	ReleaseDC(pDC);
}

void CManualModeDlg::draw_balance_box(int chair) 
{
	CPen		*pTempPen, oldpen;
	CBrush		*pTempBrush, oldbrush;	
	RECT		textrect, drawrect;
	CFont		*oldfont;
	CString		t;
	RECT		cr;
	int			left, top, right, bottom;
	CDC			*pDC = GetDC();

	// Get size of current client window
	GetClientRect(&cr);

	// Background color
	pDC->SetBkColor(COLOR_GRAY);

	// Set font basics
	oldfont = pDC->SelectObject(&cFont);
	pDC->SetTextColor(COLOR_BLACK);

	if (seated[chair] || active[chair]) 
	{
		pTempPen = (CPen*)pDC->SelectObject(&black_pen);
		oldpen.FromHandle((HPEN)pTempPen);					// Save old pen
		pTempBrush = (CBrush*)pDC->SelectObject(&white_brush);
		oldbrush.FromHandle((HBRUSH)pTempBrush);			// Save old brush
		t = playerbalance[chair];
	}
	else 
	{
		pTempPen = (CPen*)pDC->SelectObject(&white_dot_pen);
		oldpen.FromHandle((HPEN)pTempPen);					// Save old pen
		pTempBrush = (CBrush*)pDC->SelectObject(&gray_brush);
		oldbrush.FromHandle((HBRUSH)pTempBrush);			// Save old brush
		t = "";
	}

	// Figure placement of box
	left = cr.right * pc[chair][0] - 26;
	top = cr.bottom * pc[chair][1] + 31;
	right = cr.right * pc[chair][0] + 25;
	bottom = cr.bottom * pc[chair][1] + 46;

	// Calc rectangle size for text
	textrect.left = 0;
	textrect.top = 0;
	textrect.right = 0;
	textrect.bottom = 0;
	pDC->DrawText(t.GetString(), t.GetLength(), &textrect, DT_CALCRECT);

	// Figure out placement of rectangle
	drawrect.left = left < (left+(right-left)/2)-textrect.right/2-3 ? left : (left+(right-left)/2)-textrect.right/2-3;
	drawrect.top = top;
	drawrect.right = right > (left+(right-left)/2)+textrect.right/2+3 ? right : (left+(right-left)/2)+textrect.right/2+3;
	drawrect.bottom = bottom;

	// Draw it		
	pDC->SetBkMode(OPAQUE);
	pDC->Rectangle(drawrect.left, drawrect.top, drawrect.right, drawrect.bottom);
	pDC->SetBkMode(TRANSPARENT);
	pDC->DrawText(t.GetString(), t.GetLength(), &drawrect, DT_CENTER | DT_SINGLELINE | DT_VCENTER);

	// Restore original pen and brush
	pDC->SelectObject(oldpen);
	pDC->SelectObject(oldbrush);
	pDC->SelectObject(oldfont);
	ReleaseDC(pDC);
}

void CManualModeDlg::draw_player_bet(int chair) 
{
	CPen		*pTempPen, oldpen;
	CBrush		*pTempBrush, oldbrush;	
	RECT		textrect, drawrect;
	CFont		*oldfont;
	CString		t;
	RECT		cr;
	int			xcenter, ycenter, xadj, yadj;
	CDC			*pDC = GetDC();
	
	// Get size of current client window
	GetClientRect(&cr);

	// Background color
	pDC->SetBkColor(COLOR_GRAY);

	// Figure placement
	xcenter = cr.right * pc[chair][0];
	ycenter = cr.bottom * pc[chair][1];
	xadj = pcbet[chair][0];
	yadj = pcbet[chair][1];
					
	// Set font basics
	oldfont = pDC->SelectObject(&cFont);
	pDC->SetTextColor(COLOR_WHITE);

	// Draw box
	if (active[chair]) 
	{
		pTempPen = (CPen*)pDC->SelectObject(&white_pen);
		oldpen.FromHandle((HPEN)pTempPen);					// Save old pen
		t = playerbet[chair];
	}
	else 
	{
		pTempPen = (CPen*)pDC->SelectObject(&white_dot_pen);
		oldpen.FromHandle((HPEN)pTempPen);					// Save old pen
		t = "";
	}
	pTempBrush = (CBrush*)pDC->SelectObject(&gray_brush);
	oldbrush.FromHandle((HBRUSH)pTempBrush);			// Save old brush

	// Calc rectangle size for text
	textrect.left = 0;
	textrect.top = 0;
	textrect.right = 0;
	textrect.bottom = 0;
	pDC->DrawText(t.GetString(), t.GetLength(), &textrect, DT_CALCRECT);
	if (textrect.right<40) textrect.right = 40;
	if (textrect.bottom<16) textrect.bottom = 16;

	// Figure out placement of rectangle
	if (xadj<0) 
	{
		drawrect.left = xcenter + xadj - textrect.right;
		drawrect.top = ycenter + yadj - textrect.bottom/2;
		drawrect.right = xcenter + xadj;
		drawrect.bottom = ycenter + yadj + textrect.bottom/2;
	}
	else if (xadj>0) 
	{
		drawrect.left = xcenter + xadj;
		drawrect.top = ycenter + yadj - textrect.bottom/2;
		drawrect.right = xcenter + xadj + textrect.right;
		drawrect.bottom = ycenter + yadj + textrect.bottom/2;
	}
	else 
	{  // xadj==0
		drawrect.left = xcenter + xadj - textrect.right/2;
		drawrect.top = ycenter + yadj - textrect.bottom/2;
		drawrect.right = xcenter + xadj + textrect.right/2;
		drawrect.bottom = ycenter + yadj + textrect.bottom/2;
	}

	//Draw it
	pDC->SetBkMode(OPAQUE);
	pDC->Rectangle(drawrect.left-2, drawrect.top, drawrect.right+2, drawrect.bottom);
	pDC->SetBkMode(TRANSPARENT);
	pDC->DrawText(t.GetString(), t.GetLength(), &drawrect, DT_CENTER | DT_SINGLELINE | DT_VCENTER);

	pDC->SelectObject(oldpen);
	pDC->SelectObject(oldbrush);
	pDC->SelectObject(oldfont);
	ReleaseDC(pDC);
}

void CManualModeDlg::OnLButtonUp(UINT nFlags, CPoint point) 
{
	// Find out where we clicked
	get_click_loc(point);

	// Clicked on a button indicator
	if  ( click_loc>=FB && click_loc<=APB ) 
	{
		HandleButtonClick(click_loc);
	}

	// Clicked on the center info box
	else if ( click_loc==CIB ) 
	{
	}

	// Clicked on a player's name
	else if ( click_loc>=PN0 && click_loc<=PN9 ) 
	{
	}

	// Clicked on a player's balance
	else if ( click_loc>=PB0 && click_loc<=PB9 ) 
	{
	}

	// Clicked on a player's bet
	else if ( click_loc>=BET0 && click_loc<=BET9 ) 
	{
		// ctrl-shift click = allin
		if (nFlags & MK_SHIFT & MK_CONTROL) 
		{
			do_allin();
			InvalidateRect(NULL, true);
		}

		// shift click = raise
		else if (nFlags & MK_SHIFT) 
		{
			do_raise();
			InvalidateRect(NULL, true);
		}

		// plain click = call
		else if (atof(playerbet[click_chair]) != get_current_bet()) 
		{
			do_call();
			InvalidateRect(NULL, true);
		}
	}

	// Clicked on a common card
	else if ( click_loc>=CC0 && click_loc<=CC4 ) 
	{
	}

	// Clicked on a player card
	else if ( click_loc>=P0C0 && click_loc<=P9C3 ) 
	{
	}

	CDialog::OnLButtonUp(nFlags, point);
}

void CManualModeDlg::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	// Find out where we clicked
	get_click_loc(point);

	// Clicked on a button indicator
	if  ( click_loc>=FB && click_loc<=APB ) 
	{
	}

	// Clicked on the center info box
	else if ( click_loc==CIB ) 
	{
	}

	// Clicked on a player's name
	else if ( click_loc>=PN0 && click_loc<=PN9 ) 
	{
	}

	// Clicked on a player's balance
	else if ( click_loc>=PB0 && click_loc<=PB9 ) 
	{
	}

	// Clicked on a player's bet
	else if ( click_loc>=BET0 && click_loc<=BET9 ) 
	{
	}

	// Clicked on a common card
	else if ( click_loc>=CC0 && click_loc<=CC4 ) 
	{
	}

	// Clicked on a player card
	else if ( click_loc>=P0C0 && click_loc<=P9C3 ) 
	{
		// Sit in, sit down, set card backs
		seated[click_chair] = true;
		active[click_chair] = true;
		OnCardBacks();
	}

	CDialog::OnLButtonDblClk(nFlags, point);

}

void CManualModeDlg::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	CMenu			contextMenu, *tracker, *subrank; 
	MENUITEMINFO	mii;
	CPoint			point_adj;
	Registry		reg;

	// Find out where we clicked
	point_adj = point;
	ScreenToClient(&point_adj);
	get_click_loc(point_adj);

	//////////////////////////////////////////////////////////////
	// If we clicked on the macro button, pop up the edit dialog
	if  ( click_loc==MACRO ) 
	{
		edit.m_titletext.Format("Edit macro");
		edit.m_result = macro_text;
		if (edit.DoModal() == IDOK) 
		{
			macro_text = edit.m_result;
			reg.read_reg();
			reg.macro = macro_text;
			reg.write_reg();
		}
	}

	//////////////////////////////////////////////////////////////
	// If we clicked on a button indicator, toggle it
	else if ( click_loc>=FB && click_loc<=APB ) 
	{
		buttonstate[click_loc-FB] = !buttonstate[click_loc-FB];
		InvalidateRect(NULL, true);
	}

	//////////////////////////////////////////////////////////////
	// If we clicked on the center info box, popup dialog
	else if ( click_loc==CIB ) 
	{
		gameinfo.m_sblind = sblind;
		gameinfo.m_bblind = bblind;
		gameinfo.m_bigbet = bbet;
		gameinfo.m_ante = ante;
		gameinfo.m_gametype = limit;
		gameinfo.m_network = network;
		gameinfo.m_tournament = istournament;
    gameinfo.m_is_omaha = isomaha;
		if (gameinfo.DoModal() == IDOK) 
		{
			sblind = gameinfo.m_sblind;
			bblind = gameinfo.m_bblind;
			bbet = gameinfo.m_bigbet;
			ante = gameinfo.m_ante;
			limit = gameinfo.m_gametype;
			network = gameinfo.m_network;
			istournament = gameinfo.m_tournament;
      isomaha = gameinfo.m_is_omaha;
      InvalidateRect(NULL, true);
		}
	}

	//////////////////////////////////////////////////////////////
	// If we clicked on a player's name, popup dialog
	else if ( click_loc>=PN0 && click_loc<=PN9 ) 
	{
		edit.m_titletext.Format("New name for seat %d", click_chair);
		edit.m_result = playername[click_chair];
		if (edit.DoModal() == IDOK) 
		{
			playername[click_chair] = edit.m_result;
			InvalidateRect(NULL, true);
		}
	}

	//////////////////////////////////////////////////////////////
	// If we clicked on a player's balance, popup dialog
	else if ( click_loc>=PB0 && click_loc<=PB9 ) 
	{
		edit.m_titletext.Format("New balance for seat %d", click_chair);
		edit.m_result = playerbalance[click_chair];
		if (edit.DoModal() == IDOK) 
		{
			playerbalance[click_chair] = edit.m_result;
			InvalidateRect(NULL, true);
		}
	}

	//////////////////////////////////////////////////////////////
	// If we clicked on a player's bet, popup dialog
	else if ( click_loc>=BET0 && click_loc<=BET9 ) 
	{
		edit.m_titletext.Format("New bet for seat %d", click_chair);
		edit.m_result = playerbet[click_chair];
		if (edit.DoModal() == IDOK) 
		{
			double new_balance = atof(playerbalance[click_chair].GetString()) + 
								 atof(playerbet[click_chair].GetString()) - 
								 atof(edit.m_result.GetString());

			playerbalance[click_chair] = NumberToFormattedString(new_balance);
			
			playerbet[click_chair] = edit.m_result;
			InvalidateRect(NULL, true);
		}
	}

	//////////////////////////////////////////////////////////////
	// If we clicked on a common card popup the card menu
	else if ( click_loc>=CC0 && click_loc<=CC4 ) 
	{
		
		// Load the popup menu
		contextMenu.LoadMenu(IDR_MENU_CARD_SELECTION);

		// Get a pointer to the first item of the menu
		tracker = contextMenu.GetSubMenu(0);

		// Set state of each menu item based on used_cards
		memset(&mii, 0, sizeof(MENUITEMINFO));
		mii.cbSize = sizeof(MENUITEMINFO);
		mii.fMask = MIIM_STATE;
		for (int i=0; i<k_number_of_ranks_per_deck; i++) 
		{
			subrank = tracker->GetSubMenu(i);
      
      mii.fState = CardMask_CARD_IS_SET(used_cards, StdDeck_MAKE_CARD(i, Suit_CLUBS)) ? MFS_DISABLED : MFS_ENABLED;
			subrank->SetMenuItemInfo(0, &mii, true);

			mii.fState = CardMask_CARD_IS_SET(used_cards, StdDeck_MAKE_CARD(i, Suit_DIAMONDS)) ? MFS_DISABLED : MFS_ENABLED;
			subrank->SetMenuItemInfo(1, &mii, true);

			mii.fState = CardMask_CARD_IS_SET(used_cards, StdDeck_MAKE_CARD(i, Suit_HEARTS)) ? MFS_DISABLED : MFS_ENABLED;
			subrank->SetMenuItemInfo(2, &mii, true);

			mii.fState = CardMask_CARD_IS_SET(used_cards, StdDeck_MAKE_CARD(i, Suit_SPADES)) ? MFS_DISABLED : MFS_ENABLED;
			subrank->SetMenuItemInfo(3, &mii, true);

			mii.fState = CardMask_CARD_IS_SET(used_cards, StdDeck_MAKE_CARD(i, Suit_CLUBS)) &&
						 CardMask_CARD_IS_SET(used_cards, StdDeck_MAKE_CARD(i, Suit_DIAMONDS)) &&
						 CardMask_CARD_IS_SET(used_cards, StdDeck_MAKE_CARD(i, Suit_HEARTS)) &&
						 CardMask_CARD_IS_SET(used_cards, StdDeck_MAKE_CARD(i, Suit_SPADES)) ? MFS_DISABLED : MFS_ENABLED;
			tracker->SetMenuItemInfo(i, &mii, true);
		}

		// Set state of no cards item
		mii.fState = card[click_loc]!=CARD_NOCARD ? MFS_ENABLED : MFS_DISABLED;
		tracker->SetMenuItemInfo(14, &mii, true);

		// Display the context menu, unobstructive
		tracker->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON, 
			point.x + MM_WIDTH - point_adj.x, 
			point.y, this);
	}

	//////////////////////////////////////////////////////////////
	// If we clicked on a player card popup the card/seat menu
	else if ( click_loc>=P0C0 && click_loc<=P9C3 ) 
	{
		
		// Load the popup menu
		contextMenu.LoadMenu(IDR_MENU_CARD_AND_SEAT_SELECTION);

		// Get a pointer to the first item of the menu
		tracker = contextMenu.GetSubMenu(0);

		// Set state and text of seat item
		memset(&mii, 0, sizeof(MENUITEMINFO));
		mii.cbSize = sizeof(MENUITEMINFO);
		mii.fMask = MIIM_STRING;
		mii.dwTypeData = seated[click_chair] ? "&Stand up" : "&Sit down";
		tracker->SetMenuItemInfo(0, &mii, true);

		// Set state and text of sit item
		memset(&mii, 0, sizeof(MENUITEMINFO));
		mii.cbSize = sizeof(MENUITEMINFO);
		mii.fMask = MIIM_STRING | MIIM_STATE;
		mii.fState = seated[click_chair] ? MFS_ENABLED : MFS_DISABLED;
		mii.dwTypeData = active[click_chair] ? "S&it out" : "S&it in";
		tracker->SetMenuItemInfo(1, &mii, true);

		// Set state of dealer item
		memset(&mii, 0, sizeof(MENUITEMINFO));
		mii.cbSize = sizeof(MENUITEMINFO);
		mii.fMask = MIIM_STATE;
		mii.fState = dealer[click_chair] ? MFS_DISABLED : MFS_ENABLED;
		tracker->SetMenuItemInfo(2, &mii, true);

		// Set state of 2-A menu items based on used_cards
		memset(&mii, 0, sizeof(MENUITEMINFO));
		mii.cbSize = sizeof(MENUITEMINFO);
		mii.fMask = MIIM_STATE;
		for (int i=0; i<k_number_of_ranks_per_deck; i++) 
		{
			subrank = tracker->GetSubMenu(i+4);

			mii.fState = CardMask_CARD_IS_SET(used_cards, StdDeck_MAKE_CARD(i, Suit_CLUBS)) ? MFS_DISABLED : MFS_ENABLED;
			subrank->SetMenuItemInfo(0, &mii, true);

			mii.fState = CardMask_CARD_IS_SET(used_cards, StdDeck_MAKE_CARD(i, Suit_DIAMONDS)) ? MFS_DISABLED : MFS_ENABLED;
			subrank->SetMenuItemInfo(1, &mii, true);

			mii.fState = CardMask_CARD_IS_SET(used_cards, StdDeck_MAKE_CARD(i, Suit_HEARTS)) ? MFS_DISABLED : MFS_ENABLED;
			subrank->SetMenuItemInfo(2, &mii, true);

			mii.fState = CardMask_CARD_IS_SET(used_cards, StdDeck_MAKE_CARD(i, Suit_SPADES)) ? MFS_DISABLED : MFS_ENABLED;
			subrank->SetMenuItemInfo(3, &mii, true);

			mii.fState = (CardMask_CARD_IS_SET(used_cards, StdDeck_MAKE_CARD(i, Suit_CLUBS)) &&
						  CardMask_CARD_IS_SET(used_cards, StdDeck_MAKE_CARD(i, Suit_DIAMONDS)) &&
						  CardMask_CARD_IS_SET(used_cards, StdDeck_MAKE_CARD(i, Suit_HEARTS)) &&
						  CardMask_CARD_IS_SET(used_cards, StdDeck_MAKE_CARD(i, Suit_SPADES)) ) ||
						  !active[click_chair] ? MFS_DISABLED : MFS_ENABLED;
			tracker->SetMenuItemInfo(i+4, &mii, true);
		}

		// Set state of cardbacks item
		mii.fState = active[click_chair] ? MFS_ENABLED : MFS_DISABLED;
		tracker->SetMenuItemInfo(18, &mii, true);

		// Set state of no cards item
		mii.fState = active[click_chair] &&
					 (GetPlayerCard(click_chair, 0) !=CARD_NOCARD 
             || GetPlayerCard(click_chair, 1) != CARD_NOCARD
             || GetPlayerCard(click_chair, 2) != CARD_NOCARD
             || GetPlayerCard(click_chair, 3) != CARD_NOCARD)
					 ? MFS_ENABLED : MFS_DISABLED;
		tracker->SetMenuItemInfo(19, &mii, true);

		// Display the context menu
		tracker->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON, 
			point.x + MM_WIDTH - point_adj.x,
			point.y, this);
	}
}

void CManualModeDlg::get_click_loc(CPoint p) 
{
	RECT		cr;
	int			xcenter, ycenter, xadj, yadj;
	int			left, top, right, bottom;
	RECT		textrect, drawrect;
	CDC			*pDC = GetDC();
	CString		t;

	GetClientRect(&cr);

	// see if we clicked on the macro button
	if (p.x >= 233 && p.x <= 233+51 &&
		p.y >= 307 && p.y <= 307+23) 
	{
		click_loc = MACRO;
		click_chair = -1;
		return;
	}

	// see if we clicked on a common card
	for (int i=0; i<kNumberOfCommunityCards; i++) 
	{
		if (p.x >= cr.right/2 + cc[i][0] && 
			p.x <= cr.right/2 + cc[i][0] + CARDSIZEX &&
			p.y >= cr.bottom/2 + cc[i][1] &&
			p.y <= cr.bottom/2 + cc[i][1] + CARDSIZEY) 
		{
			click_loc = CC0+i;
			click_chair = -1;
			return;
		}
	}

	// See if we clicked on a player's name
	for (int i=0; i<kMaxNumberOfPlayers; i++) 
	{
		// Figure placement of box
		left = cr.right * pc[i][0] - 36;
		top = cr.bottom * pc[i][1] + 15;
		right = cr.right * pc[i][0] + 35;
		bottom = cr.bottom * pc[i][1] + 30;

		// Calc rectangle size for text
		textrect.left = 0;
		textrect.top = 0;
		textrect.right = 0;
		textrect.bottom = 0;
		pDC->DrawText(playername[i].GetString(), playername[i].GetLength(), &textrect, DT_CALCRECT);

		// Figure out placement of rectangle
		drawrect.left = left < (left+(right-left)/2)-textrect.right/2-3 ? left : (left+(right-left)/2)-textrect.right/2-3;
		drawrect.top = top;
		drawrect.right = right > (left+(right-left)/2)+textrect.right/2+3 ? right : (left+(right-left)/2)+textrect.right/2+3;
		drawrect.bottom = bottom;

		if (p.x >= drawrect.left && p.x <= drawrect.right &&
			p.y >= drawrect.top && p.y <= drawrect.bottom &&
			seated[i]) 
		{
			click_loc = PN0+i;
			click_chair = i;
			return;
		}
	}

	// See if we clicked on a player's balance
	for (int i=0; i<kMaxNumberOfPlayers; i++) 
	{
		t = "";
		if (seated[i] || active[i]) 
		{
			t = playerbalance[i];
		}

		// Figure placement of box
		left = cr.right * pc[i][0] - 26;
		top = cr.bottom * pc[i][1] + 31;
		right = cr.right * pc[i][0] + 25;
		bottom = cr.bottom * pc[i][1] + 46;

		// Calc rectangle size for text
		textrect.left = 0;
		textrect.top = 0;
		textrect.right = 0;
		textrect.bottom = 0;
		pDC->DrawText(t.GetString(), t.GetLength(), &textrect, DT_CALCRECT);
	
		// Figure out placement of rectangle
		drawrect.left = left < (left+(right-left)/2)-textrect.right/2-3 ? left : (left+(right-left)/2)-textrect.right/2-3;
		drawrect.top = top;
		drawrect.right = right > (left+(right-left)/2)+textrect.right/2+3 ? right : (left+(right-left)/2)+textrect.right/2+3;
		drawrect.bottom = bottom;
		
		if (p.x >= drawrect.left && p.x <= drawrect.right &&
			p.y >= drawrect.top && p.y <= drawrect.bottom &&
			seated[i]) 
		{
			click_loc = PB0+i;
			click_chair = i;
			return;
		}
	}

	// see if we clicked on a player's bet
	for (int i=0; i<kMaxNumberOfPlayers; i++) 
	{
		t = "";
		if (active[i]) 
		{
			t = playerbet[i];
		}

		xcenter = cr.right * pc[i][0];
		ycenter = cr.bottom * pc[i][1];
		xadj = pcbet[i][0];
		yadj = pcbet[i][1];
		
		// Calc rectangle size for text
		textrect.left = 0;
		textrect.top = 0;
		textrect.right = 0;
		textrect.bottom = 0;
		pDC->DrawText(t.GetString(), t.GetLength(), &textrect, DT_CALCRECT);
		if (textrect.right<40) textrect.right = 40;
		if (textrect.bottom<16) textrect.bottom = 16;

		// Figure out placement of rectangle
		if (xadj<0) 
		{
			drawrect.left = xcenter + xadj - textrect.right;
			drawrect.top = ycenter + yadj - textrect.bottom/2;
			drawrect.right = xcenter + xadj;
			drawrect.bottom = ycenter + yadj + textrect.bottom/2;
		}
		else if (xadj>0) 
		{
			drawrect.left = xcenter + xadj;
			drawrect.top = ycenter + yadj - textrect.bottom/2;
			drawrect.right = xcenter + xadj + textrect.right;
			drawrect.bottom = ycenter + yadj + textrect.bottom/2;
		}
		else 
		{  // xadj==0
			drawrect.left = xcenter + xadj - textrect.right/2;
			drawrect.top = ycenter + yadj - textrect.bottom/2;
			drawrect.right = xcenter + xadj + textrect.right/2;
			drawrect.bottom = ycenter + yadj + textrect.bottom/2;
		}

		if (p.x >= drawrect.left-2 && p.x <= drawrect.right+2 &&
			p.y >= drawrect.top && p.y <= drawrect.bottom &&
			active[i]) 
		{
			click_loc = BET0+i;
			click_chair = i;
			return;
		}

	}

  // see if we clicked on the center info box
  if (p.x >= cr.right / 2 - 65 && p.x <= cr.right / 2 + 45 &&
    p.y >= 4 && p.y <= 78)
  {
    click_loc = CIB;
    click_chair = -1;
    return;
  }

	// See if we clicked on a player's card
  // We draw players and cards in increasing order
  // Due to space restrictions cards might overlap (for Omaha).
  // Therefore we search inreversed order 
  // to get the top-most card and avoid confusion.
  for (int i = kMaxNumberOfPlayers - 1; i >= 0; --i) {
    for (int j = kMaxNumberOfCardsPerPlayer - 1; j >= 0; --j)
    {
      if (!IsOmaha() && (j >= kNumberOfCardsPerPlayerHoldEm)) {
        // Skip cards 0 and 3 if we are playing Hold'em
        // It is annoying if we accidentaly set an invisible card
        // or if a visible card is not everywhere clickable
        continue;
      }
      int left = PlayerCardLeft(i, j);
      int top = PlayerCardTop(i);
      int right = left + CARDSIZEX - 1;
      int bottom = top + CARDSIZEY - 1;
      if (p.x >= left &&
        p.x <= right &&
        p.y >= top &&
        p.y <= bottom)
      {
        click_loc = P0C0 + i * kNumberOfCardsPerPlayerOmaha + j;
        click_chair = i;
        return;
      }
    }
	}

	// see if we clicked on a button
	if (p.x >= cr.right-84 && p.x <= cr.right-70 &&
		p.y >= cr.bottom-16 && p.y <= cr.bottom-2) 
	{
		click_loc = FB;
		click_chair = -1;
		return;
	}
	if (p.x >= cr.right-67 && p.x <= cr.right-53 &&
		p.y >= cr.bottom-16 && p.y <= cr.bottom-2) 
	{
		click_loc = CB;
		click_chair = -1;
		return;
	}
	if (p.x >= cr.right-50 && p.x <= cr.right-36 &&
		p.y >= cr.bottom-16 && p.y <= cr.bottom-2) 
	{
		click_loc = KB;
		click_chair = -1;
		return;
	}
	if (p.x >= cr.right-33 && p.x <= cr.right-19 &&
		p.y >= cr.bottom-16 && p.y <= cr.bottom-2) 
	{
		click_loc = RB;
		click_chair = -1;
		return;
	}
	if (p.x >= cr.right-16 && p.x <= cr.right-2 &&
		p.y >= cr.bottom-16 && p.y <= cr.bottom-2) 
	{
		click_loc = AB;
		click_chair = -1;
		return;
	}

	if (p.x >= cr.left+2 && p.x <= cr.left+12 &&
		p.y >= cr.bottom-48 && p.y <= cr.bottom-38) 
	{
		click_loc = SO;
		click_chair = -1;
		return;
	}
	if (p.x >= cr.left+2 && p.x <= cr.left+12 &&
		p.y >= cr.bottom-36 && p.y <= cr.bottom-26) 
	{
		click_loc = LT;
		click_chair = -1;
		return;
	}
	if (p.x >= cr.left+2 && p.x <= cr.left+12 &&
		p.y >= cr.bottom-24 && p.y <= cr.bottom-14) 
	{
		click_loc = PFB;
		click_chair = -1;
		return;
	}
	if (p.x >= cr.left+2 && p.x <= cr.left+12 &&
		p.y >= cr.bottom-12 && p.y <= cr.bottom-2) 
	{
		click_loc = APB;
		click_chair = -1;
		return;
	}

	click_loc = -1;
	click_chair = -1;
	return;
}

void CManualModeDlg::On2Clubs()		{ set_card(StdDeck_MAKE_CARD(Rank_2, Suit_CLUBS)); }
void CManualModeDlg::On2Diamonds()	{ set_card(StdDeck_MAKE_CARD(Rank_2, Suit_DIAMONDS)); }
void CManualModeDlg::On2Hearts()	{ set_card(StdDeck_MAKE_CARD(Rank_2, Suit_HEARTS)); }
void CManualModeDlg::On2Spades()	{ set_card(StdDeck_MAKE_CARD(Rank_2, Suit_SPADES)); }
void CManualModeDlg::On3Clubs()		{ set_card(StdDeck_MAKE_CARD(Rank_3, Suit_CLUBS)); }
void CManualModeDlg::On3Diamonds()	{ set_card(StdDeck_MAKE_CARD(Rank_3, Suit_DIAMONDS)); }
void CManualModeDlg::On3Hearts()	{ set_card(StdDeck_MAKE_CARD(Rank_3, Suit_HEARTS)); }
void CManualModeDlg::On3Spades()	{ set_card(StdDeck_MAKE_CARD(Rank_3, Suit_SPADES)); }
void CManualModeDlg::On4Clubs()		{ set_card(StdDeck_MAKE_CARD(Rank_4, Suit_CLUBS)); }
void CManualModeDlg::On4Diamonds()	{ set_card(StdDeck_MAKE_CARD(Rank_4, Suit_DIAMONDS)); }
void CManualModeDlg::On4Hearts()	{ set_card(StdDeck_MAKE_CARD(Rank_4, Suit_HEARTS)); }
void CManualModeDlg::On4Spades()	{ set_card(StdDeck_MAKE_CARD(Rank_4, Suit_SPADES)); }
void CManualModeDlg::On5Clubs()		{ set_card(StdDeck_MAKE_CARD(Rank_5, Suit_CLUBS)); }
void CManualModeDlg::On5Diamonds()	{ set_card(StdDeck_MAKE_CARD(Rank_5, Suit_DIAMONDS)); }
void CManualModeDlg::On5Hearts()	{ set_card(StdDeck_MAKE_CARD(Rank_5, Suit_HEARTS)); }
void CManualModeDlg::On5Spades()	{ set_card(StdDeck_MAKE_CARD(Rank_5, Suit_SPADES)); }
void CManualModeDlg::On6Clubs()		{ set_card(StdDeck_MAKE_CARD(Rank_6, Suit_CLUBS)); }
void CManualModeDlg::On6Diamonds()	{ set_card(StdDeck_MAKE_CARD(Rank_6, Suit_DIAMONDS)); }
void CManualModeDlg::On6Hearts()	{ set_card(StdDeck_MAKE_CARD(Rank_6, Suit_HEARTS)); }
void CManualModeDlg::On6Spades()	{ set_card(StdDeck_MAKE_CARD(Rank_6, Suit_SPADES)); }
void CManualModeDlg::On7Clubs()		{ set_card(StdDeck_MAKE_CARD(Rank_7, Suit_CLUBS)); }
void CManualModeDlg::On7Diamonds()	{ set_card(StdDeck_MAKE_CARD(Rank_7, Suit_DIAMONDS)); }
void CManualModeDlg::On7Hearts()	{ set_card(StdDeck_MAKE_CARD(Rank_7, Suit_HEARTS)); }
void CManualModeDlg::On7Spades()	{ set_card(StdDeck_MAKE_CARD(Rank_7, Suit_SPADES)); }
void CManualModeDlg::On8Clubs()		{ set_card(StdDeck_MAKE_CARD(Rank_8, Suit_CLUBS)); }
void CManualModeDlg::On8Diamonds()	{ set_card(StdDeck_MAKE_CARD(Rank_8, Suit_DIAMONDS)); }
void CManualModeDlg::On8Hearts()	{ set_card(StdDeck_MAKE_CARD(Rank_8, Suit_HEARTS)); }
void CManualModeDlg::On8Spades()	{ set_card(StdDeck_MAKE_CARD(Rank_8, Suit_SPADES)); }
void CManualModeDlg::On9Clubs()		{ set_card(StdDeck_MAKE_CARD(Rank_9, Suit_CLUBS)); }
void CManualModeDlg::On9Diamonds()	{ set_card(StdDeck_MAKE_CARD(Rank_9, Suit_DIAMONDS)); }
void CManualModeDlg::On9Hearts()	{ set_card(StdDeck_MAKE_CARD(Rank_9, Suit_HEARTS)); }
void CManualModeDlg::On9Spades()	{ set_card(StdDeck_MAKE_CARD(Rank_9, Suit_SPADES)); }
void CManualModeDlg::OnTClubs()		{ set_card(StdDeck_MAKE_CARD(Rank_TEN, Suit_CLUBS)); }
void CManualModeDlg::OnTDiamonds()	{ set_card(StdDeck_MAKE_CARD(Rank_TEN, Suit_DIAMONDS)); }
void CManualModeDlg::OnTHearts()	{ set_card(StdDeck_MAKE_CARD(Rank_TEN, Suit_HEARTS)); }
void CManualModeDlg::OnTSpades()	{ set_card(StdDeck_MAKE_CARD(Rank_TEN, Suit_SPADES)); }
void CManualModeDlg::OnJClubs()		{ set_card(StdDeck_MAKE_CARD(Rank_JACK, Suit_CLUBS)); }
void CManualModeDlg::OnJDiamonds()	{ set_card(StdDeck_MAKE_CARD(Rank_JACK, Suit_DIAMONDS)); }
void CManualModeDlg::OnJHearts()	{ set_card(StdDeck_MAKE_CARD(Rank_JACK, Suit_HEARTS)); }
void CManualModeDlg::OnJSpades()	{ set_card(StdDeck_MAKE_CARD(Rank_JACK, Suit_SPADES)); }
void CManualModeDlg::OnQClubs()		{ set_card(StdDeck_MAKE_CARD(Rank_QUEEN, Suit_CLUBS)); }
void CManualModeDlg::OnQDiamonds()	{ set_card(StdDeck_MAKE_CARD(Rank_QUEEN, Suit_DIAMONDS)); }
void CManualModeDlg::OnQHearts()	{ set_card(StdDeck_MAKE_CARD(Rank_QUEEN, Suit_HEARTS)); }
void CManualModeDlg::OnQSpades()	{ set_card(StdDeck_MAKE_CARD(Rank_QUEEN, Suit_SPADES)); }
void CManualModeDlg::OnKClubs()		{ set_card(StdDeck_MAKE_CARD(Rank_KING, Suit_CLUBS)); }
void CManualModeDlg::OnKDiamonds()	{ set_card(StdDeck_MAKE_CARD(Rank_KING, Suit_DIAMONDS)); }
void CManualModeDlg::OnKHearts()	{ set_card(StdDeck_MAKE_CARD(Rank_KING, Suit_HEARTS)); }
void CManualModeDlg::OnKSpades()	{ set_card(StdDeck_MAKE_CARD(Rank_KING, Suit_SPADES)); }
void CManualModeDlg::OnAClubs()		{ set_card(StdDeck_MAKE_CARD(Rank_ACE, Suit_CLUBS)); }
void CManualModeDlg::OnADiamonds()	{ set_card(StdDeck_MAKE_CARD(Rank_ACE, Suit_DIAMONDS)); }
void CManualModeDlg::OnAHearts()	{ set_card(StdDeck_MAKE_CARD(Rank_ACE, Suit_HEARTS)); }
void CManualModeDlg::OnASpades()	{ set_card(StdDeck_MAKE_CARD(Rank_ACE, Suit_SPADES)); }

void CManualModeDlg::OnCardBacks() 
{
  for (int j = 0; j < NumberOfCardsPerPlayer(); ++j) {
    if (GetPlayerCard(click_chair, j != CARD_NOCARD
      && GetPlayerCard(click_chair, j) != CARD_BACK))
    {
      CardMask_UNSET(used_cards, GetPlayerCard(click_chair, j));
      SetPlayerCard(click_chair, j, CARD_BACK);
    }
  }
  InvalidateRect(NULL, true);
}

void CManualModeDlg::OnNoCards() 
{
  for (int j = 0; j < NumberOfCardsPerPlayer(); ++j) {
    if (GetPlayerCard(click_chair, j != CARD_NOCARD
      && GetPlayerCard(click_chair, j) != CARD_BACK))
    {
      CardMask_UNSET(used_cards, GetPlayerCard(click_chair, j));
      SetPlayerCard(click_chair, j, CARD_NOCARD);
    }
  }
	InvalidateRect(NULL, true);
}

void CManualModeDlg::OnNoCard() 
{
	if (card[click_loc]!=CARD_NOCARD) 
	{
		CardMask_UNSET(used_cards, card[click_loc]);
	}
	card[click_loc] = CARD_NOCARD; 
	InvalidateRect(NULL, true);
}

void CManualModeDlg::set_card(unsigned int c) 
{
	if (card[click_loc]!=CARD_NOCARD && card[click_loc]!=CARD_BACK) 
	{
		CardMask_UNSET(used_cards, card[click_loc]);
	}
	card[click_loc] = c; 
	CardMask_SET(used_cards, c);

	if (click_loc>=CC0 && click_loc<=CC4) 
	{
		do_scrape_bets_into_pot();
	}

	InvalidateRect(NULL, true);
}

void CManualModeDlg::OnSeatPlayer() 
{ 
	seated[click_chair] = !seated[click_chair];
	if (!seated[click_chair]) active[click_chair] = false;
	InvalidateRect(NULL, true);
}

void CManualModeDlg::OnSitInPlayer() 
{ 
	active[click_chair] = !active[click_chair];
	InvalidateRect(NULL, true);
}

void CManualModeDlg::OnDealerHere() { 
  SetDealerPosition(click_chair);
}

void CManualModeDlg::SetDealerPosition(int chair) {
  for (int i = 0; i<kMaxNumberOfPlayers; i++) {
    dealer[i] = false;
  }
  dealer[chair] = true;
  InvalidateRect(NULL, true);
}

void CManualModeDlg::OnFold() 
{ 
  do_fold();
  InvalidateRect(NULL, true);
}

void CManualModeDlg::OnCall() 
{ 
	if (atof(playerbet[click_chair]) != get_current_bet()) 
	{
		do_call();
		InvalidateRect(NULL, true);
	}
}

void CManualModeDlg::OnRaise() 
{ 
	do_raise();
	InvalidateRect(NULL, true);
}

void CManualModeDlg::OnAllin() 
{ 
	do_allin();
	InvalidateRect(NULL, true);
}

void CManualModeDlg::OnBnClickedReset() 
{
	clear_scrape_areas();
	handnumber.Format("%i",atoi(handnumber) + 1);
	InvalidateRect(NULL, true);
}

void CManualModeDlg::OnBnClickedPminus() 
{
	// Removing players counter-clockwise,
	// starting with highest numbers
	for (int i=(kMaxNumberOfPlayers - 1); i>=0; i--) 
	{
		if (seated[i] == true) 
		{
			seated[i] = false;
			active[i] = false;
			InvalidateRect(NULL, true);
			return;
		}
	}
}

void CManualModeDlg::OnBnClickedPplus() 
{
	for (int i=0; i<kMaxNumberOfPlayers; i++) 
	{
		if (seated[i] == false) 
		{
			seated[i] = true;
			active[i] = true;
			InvalidateRect(NULL, true);
			return;
		}
	}
}

void CManualModeDlg::OnBnClickedMacro() {
  // First clear the table view and give OH time to scrape.
  // Just clicking "macro" over and over again might not trigger handresets.
  OnBnClickedReset();
  // https://stackoverflow.com/questions/2325894/difference-between-invalidaterect-and-redrawwindow
  UpdateWindow();
  Sleep(1500);
  //
	int	chair = kUndefined;
  int cards_seen = 0;
  int com_card=0;
  unsigned int c;

  CardMask_RESET(used_cards);
	for (int i=0; i<macro_text.GetLength(); i++) {
    if (chair > 9) break;
    char next_char = macro_text.GetAt(i); 
		if (next_char == 'R') {
			clear_scrape_areas();
      continue;
    }
    switch (next_char) {
      case 'N': // Button
      case 'b': // Small blind
      case 'B': // Big blind 
      case 'P': // Player
        ++chair;
        seated[chair] = true;
        active[chair] = true;
        SetPlayerCard(chair, 0, CARD_BACK);
        SetPlayerCard(chair, 1, CARD_BACK);
        SetPlayerCard(chair, 2, CARD_BACK);
        SetPlayerCard(chair, 3, CARD_BACK);
        break;
    }
		switch (next_char) {
      case 'N': // Button
        SetDealerPosition(chair);
        break;
      case 'p': // Not seated
        ++chair;
			  seated[chair] = true;
			  active[chair] = false;
        SetPlayerCard(chair, 0, CARD_NOCARD);
        SetPlayerCard(chair, 1, CARD_NOCARD);
        SetPlayerCard(chair, 2, CARD_NOCARD);
        SetPlayerCard(chair, 3, CARD_NOCARD);
        break;
    }
    if (chair >= 10) {
      break;
    }
    // Setting bet and balance
    double total_balance = atof(playerbalance[chair]) + atof(playerbet[chair]);
    double sblind_value = atof(sblind);
    double bblind_value = atof(bblind);
    switch (next_char) {
      case 'N':
      case 'P':
        playerbalance[chair] = NumberToFormattedString(total_balance);
        playerbet[chair] = NumberToFormattedString(0);
        break;
      case 'p':
        playerbalance[chair] = NumberToFormattedString(0);
        playerbet[chair] = NumberToFormattedString(0);
        break;
      case 'B':
        playerbalance[chair] = NumberToFormattedString(total_balance - bblind_value);
        playerbet[chair] = NumberToFormattedString(bblind_value);
        break;
      case 'b':
        playerbalance[chair] = NumberToFormattedString(total_balance - sblind_value);
        playerbet[chair] = NumberToFormattedString(sblind_value);
        break;
    }
    // Cards
		if ((macro_text.Mid(i,1) >= "2" && macro_text.Mid(i,1) <= "9") ||
				 macro_text.Mid(i,1).MakeLower() == "t" ||
				 macro_text.Mid(i,1).MakeLower() == "j" ||
				 macro_text.Mid(i,1).MakeLower() == "q" ||
				 macro_text.Mid(i,1).MakeLower() == "k" ||
				 macro_text.Mid(i,1).MakeLower() == "a") 
		{
      c = CARD_NOCARD;
			if (macro_text.Mid(i+1,1).MakeLower() == "c") 
			{
				c = StdDeck_MAKE_CARD(get_rank(macro_text.Mid(i,1).MakeLower().GetString()[0]), Suit_CLUBS);
				i++;
			}
			else if (macro_text.Mid(i+1,1).MakeLower() == "d") 
			{
				c = StdDeck_MAKE_CARD(get_rank(macro_text.Mid(i,1).MakeLower().GetString()[0]), Suit_DIAMONDS);
				i++;
			}
			else if (macro_text.Mid(i+1,1).MakeLower() == "h") 
			{
				c = StdDeck_MAKE_CARD(get_rank(macro_text.Mid(i,1).MakeLower().GetString()[0]), Suit_HEARTS);
				i++;
			}
			else if (macro_text.Mid(i+1,1).MakeLower() == "s") 
			{
				c = StdDeck_MAKE_CARD(get_rank(macro_text.Mid(i,1).MakeLower().GetString()[0]), Suit_SPADES);
				i++;
			}
      if (c != CARD_NOCARD) {
        // First N cards get assigned to current chair,
        if ((cards_seen >= 0) && (cards_seen < NumberOfCardsPerPlayer())) {
          SetPlayerCard(chair, cards_seen, c);
        }
				// Next 5 cards get set to common
				else {
					card[CC0+(com_card%5)] = c; 					
					com_card++;
				}
        // Mark card as used
        CardMask_SET(used_cards, c);
        cards_seen++;
			}
		}
	}
	InvalidateRect(NULL, true);
}

int CManualModeDlg::get_rank(char c) 
{
	if (c=='2') return Rank_2;
	if (c=='3') return Rank_3;
	if (c=='4') return Rank_4;
	if (c=='5') return Rank_5;
	if (c=='6') return Rank_6;
	if (c=='7') return Rank_7;
	if (c=='8') return Rank_8;
	if (c=='9') return Rank_9;
	if (c=='t') return Rank_TEN;
	if (c=='j') return Rank_JACK;
	if (c=='q') return Rank_QUEEN;
	if (c=='k') return Rank_KING;
	if (c=='a') return Rank_ACE;

	return Rank_2;
}

void CManualModeDlg::OnBnClickedDminus() 
{
	for (int i=0; i<kMaxNumberOfPlayers; i++) 
	{
		if (dealer[i] == true) 
		{
			dealer[i] = false;
			if (--i < 0) 
			{ 
				i = kMaxNumberOfPlayers - 1; 
			}

			dealer[i] = true;
			InvalidateRect(NULL, true);
			return;
		}
	}
}

void CManualModeDlg::OnBnClickedDplus() 
{
	CString s;
	for (int i=0; i<kMaxNumberOfPlayers; i++) 
	{
		if (dealer[i] == true) 
		{
			dealer[i] = false;
			if (++i >= kMaxNumberOfPlayers)
			{ 
				i = 0; 
			}

			dealer[i] = true;
			InvalidateRect(NULL, true);
			return;
		}
	}
}

void CManualModeDlg::SetPlayerCard(int chair, int index, int card_value) {
  int total_index = chair * kNumberOfCardsPerPlayerOmaha + index;
  card[total_index] = card_value;
}

int CManualModeDlg::GetPlayerCard(int chair, int index) {
  int total_index = chair * kNumberOfCardsPerPlayerOmaha + index;
  return card[total_index];
}

void CManualModeDlg::do_fold(void)
{
  if (click_chair < 0) {
    return;
  }
  for (int j = 0; j < NumberOfCardsPerPlayer(); ++j) {
    if (GetPlayerCard(click_chair, j != CARD_NOCARD
      && GetPlayerCard(click_chair, j) != CARD_BACK))
    {
      CardMask_UNSET(used_cards, GetPlayerCard(click_chair, j));
      SetPlayerCard(click_chair, j, CARD_NOCARD);
    }
  }
}

void CManualModeDlg::do_call(void) 
{
	double		diff;

	diff = get_current_bet() - atof(playerbet[click_chair]);
	if (diff > atof(playerbalance[click_chair])) 
	{
		diff = atof(playerbalance[click_chair]);
	}

	playerbet[click_chair] = NumberToFormattedString(diff+atof(playerbet[click_chair]));

	double new_balance = atof(playerbalance[click_chair].GetString()) - diff;
	playerbalance[click_chair] = NumberToFormattedString(new_balance);
}

void CManualModeDlg::do_raise(void) 
{
	double diff, raise_amt;

	raise_amt = get_br()>=3 && limit==LIMIT_FL ? atof(bblind)*2 :atof(bblind);

	diff = get_current_bet() + raise_amt - atof(playerbet[click_chair]);
	if (diff > atof(playerbalance[click_chair])) 
	{
		diff = atof(playerbalance[click_chair]);
	}

	playerbet[click_chair] = NumberToFormattedString(diff + atof(playerbet[click_chair]));

	double new_balance = atof(playerbalance[click_chair].GetString()) - diff;
	playerbalance[click_chair] = NumberToFormattedString(new_balance);
}

void CManualModeDlg::do_allin(void) 
{
	// Caution: if we go allin, we have to add the balance to the bet, not replace it!
	// Only problem: all numbers are defined as strings
	double playerbet_as_number = atof(playerbet[click_chair]);
	double playerbalance_as_number = atof(playerbalance[click_chair]);
	double new_playerbet = playerbet_as_number + playerbalance_as_number;
	playerbet[click_chair] = NumberToFormattedString(new_playerbet);
	playerbalance[click_chair] = "0";
}

void CManualModeDlg::do_scrape_bets_into_pot(void) 
{
	int		ncommoncards = 0;

	for (int i=0; i<kNumberOfCommunityCards; i++) 
	{
		if (card[CC0+i]!=CARD_NOCARD && card[CC0+i]!=CARD_BACK) 
		{
			ncommoncards += 1;
		}
	}

	if (ncommoncards_last == 0 && ncommoncards>0) 
	{
		for (int i=0; i<kMaxNumberOfPlayers; i++) 
		{
			pot += atof(playerbet[i]);
			playerbet[i] = "0";
		}
	}
	else if (ncommoncards_last == 3 && ncommoncards>3) 
	{
		for (int i=0; i<kMaxNumberOfPlayers; i++) 
		{
			pot += atof(playerbet[i]);
			playerbet[i] = "0";
		}
	}
	else if (ncommoncards_last == 4 && ncommoncards>4) 
	{
		for (int i=0; i<kMaxNumberOfPlayers; i++) 
		{
			pot += atof(playerbet[i]);
			playerbet[i] = "0";
		}
	}

	ncommoncards_last = ncommoncards;
}


double CManualModeDlg::get_current_bet(void) 
{
	double	curbet=0;

	for (int i=0; i<kMaxNumberOfPlayers; i++) 
	{
		if (atof(playerbet[i]) > curbet) 
		{
			curbet = atof(playerbet[i]);
		}
	}

	if (get_br()==1) 
	{
		if (curbet==0)  curbet = atof(sblind);
		else if (curbet==atof(sblind))  curbet = atof(bblind);
	}

	return curbet;
}

int CManualModeDlg::get_br(void) 
{
	int		br;
	int		ncommoncards = 0;

	for (int i=0; i<kNumberOfCommunityCards; i++) 
	{
		if (card[CC0+i]!=CARD_NOCARD && card[CC0+i]!=CARD_BACK) 
		{
			ncommoncards += 1;
		}
	}

	if (ncommoncards<=2) br=1;
	else if (ncommoncards==3) br=2;
	else if (ncommoncards==4) br=3;
	else if (ncommoncards==5) br=4;

	return br;
}
void CManualModeDlg::OnEnChangeSwag()
{
	CString tmp;
	swag.GetWindowText(tmp);
	raise_amount = atof(tmp);
}

/*****************************************************
*
* Buttons, betsizes, reacting to actions
*
*****************************************************/

int CManualModeDlg::Userchair()
{
	// Simplified logic:
	// First chair (clockwise) that has cards
	for (int i=kFirstChair; i<=kLastChair; i++)
	{
		if ((GetPlayerCard(i, 0) != CARD_BACK)
			&& (GetPlayerCard(i, 0) != CARD_NOCARD))
		{
			return i;
		}
	}
	return kNotFound;
}

bool CManualModeDlg::UserMaybeBigBlind()
{
	return (playerbet[Userchair()] == bblind);
}

bool CManualModeDlg::IsPreflop() {
  for (int i=0; i<kNumberOfCommunityCards; i++) { 
		if (card[CC0+i] != CARD_NOCARD)	{
			return false;
		}
	}
  return true;
}

bool CManualModeDlg::PreflopUnraised() {
  if (!IsPreflop()) return false;
	for (int i=kFirstChair; i<=kLastChair; i++)	{
		if (atof(playerbet[i]) > atof(bblind)) return false;
	}
	return true;
}

bool CManualModeDlg::MyTurnPossible()
{
	// Simplified logic
	// My turn is possible, if
	//   * userchair known
	//   * somebody else has a higher bet
	//   * nobody did act (all bets 0)
	int userchair = Userchair();
	if ((userchair < kFirstChair) || (userchair > kLastChair))
	{
		return false;
	}
	bool somebody_betting = false;
	bool somebody_raising = false;
	for (int i=kFirstChair; i<=kLastChair; i++)
	{
		if (atof(playerbet[i]) > 0)
		{
			somebody_betting = true;
		}
		if (atof(playerbet[i]) > MyCurrentBet())
		{
			somebody_raising = true;
		}
	}
	return (!somebody_betting 
		|| somebody_raising 
		|| (PreflopUnraised() && UserMaybeBigBlind()));
}

double CManualModeDlg::MyBalance()
{
	int userchair = Userchair();
	if ((userchair < kFirstChair) || (userchair > kLastChair))
	{
		return 0;
	}
	return atof(playerbalance[userchair]);
}

double CManualModeDlg::MyTotalBalance()
{
	return MyBalance() + MyCurrentBet();
}

double CManualModeDlg::MyCurrentBet()
{
	int userchair = Userchair();
	if ((userchair < kFirstChair) || (userchair > kLastChair))
	{
		return 0;
	}
	return atof(playerbet[userchair]);
}

void CManualModeDlg::DisableAllActionButtons()
{
	for (int i=0; i<5; i++)
	{
		buttonstate[i] = false;
	}
	InvalidateRect(NULL, true);
}

void CManualModeDlg::SetAllPossibleButtons()
{
	int userchair = Userchair();
	if (userchair < 0 || !MyTurnPossible() || MyBalance() <= 0)
	{
		MessageBox("Not your turn, Sir.\n"
			"Please set up a valid situation first\n"
			"(e.g. click \"Macro\").\n",
			"Error", 0);
		DisableAllActionButtons();
		return;
	}
	// Fold always possible
	buttonstate[0] = true;
	// Check
	if ((!IsPreflop() && (get_current_bet() <= 0))
		|| (PreflopUnraised() && UserMaybeBigBlind()))
	{
		buttonstate[2] = true;
	}
	else
	{
		// Call
		// There is always either check or call possible.
		buttonstate[1] = true;
	}
	// raise
	if (MyTotalBalance() > get_current_bet())
	{
		buttonstate[3] = true;
	}
	// allin
	if (limit == LIMIT_NL || limit == LIMIT_PL)
	{
		// Simplified for PL
		buttonstate[4] = true;
	}
	InvalidateRect(NULL, true);
}

void CManualModeDlg::HandleButtonClick(int click_loc)
{
	int button_ID = click_loc - FB;
	if ((click_loc==SO) || (click_loc==LT)
		|| (click_loc==PFB) || (click_loc==APB))
	{
		// Simple button like sitout and auto-post
		// Just toggle its state
		buttonstate[button_ID] = !buttonstate[button_ID];
		InvalidateRect(NULL, true);
		return;
	}
	// Otherwise: FB, CB, KB, RB, AB,
	if (buttonstate[button_ID] == false)
	{
		// clicked on a non high-lighted action-button
		// User wants to set buttons
		// Do it automatically
		SetAllPossibleButtons();
		return;
	}
	// Otherwise: highlighted action button clicked
	// We act and buttons disappear
	DisableAllActionButtons();
	// We simply set the clickchair and then continue 
	// as if the user was handling the popup-menu
	click_chair = Userchair();
	switch (click_loc)
	{
	case FB: do_fold();
		break;
	case CB: do_call();
		break;
	case KB: // Nothing to do for check
		break;
	case RB: if (raise_amount > 0)
			{
				ExecuteSwag();
			}
			else
			{
				do_raise();
			}
		break;
	case AB: do_allin();
		break;
	}

}

void CManualModeDlg::ExecuteSwag()
{
	if (raise_amount < MyCurrentBet())
	{
		MessageBox("Dear Sir, no chance to take money out of the pot.\n"
			"The RoboStars Security Team watches you.", "Error", 0);	
	}
	else if (raise_amount >= MyTotalBalance())
	{
		do_allin();
	}
	else if (raise_amount < get_current_bet())
	{	
		MessageBox("Dear Sir, didn't you want to top the previous raise a bit?", "Error", 0);
	}
	else
	{
		// Valid normal swag
		// Userchair should be valid, otherwise we had no chance to click buttons
		int userchair = Userchair();
		double remaining_balance = MyTotalBalance() - raise_amount;
		playerbet[userchair] = NumberToFormattedString(raise_amount);
		playerbalance[userchair] = NumberToFormattedString(remaining_balance);
	}
	// Clear swag
	raise_amount = 0;
	swag.SetWindowTextA("");
	InvalidateRect(NULL, true);
}