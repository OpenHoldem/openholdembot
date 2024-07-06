// ManualModeDlg.h : header file
//

#pragma once

#include "poker_defs.h"
#include "EditDlg.h"
#include "GameInfoDlg.h"
#include "OptionsDlg.h"

#include "xmlrpc.h"
#include <windows.h>


// Limits
#define LIMIT_NL		0
#define LIMIT_PL		1
#define LIMIT_FL		2

// Window Size
#define MM_WIDTH		580
#define MM_HEIGHT		360

// Startup path
extern char	startup_path[MAX_PATH];

extern bool waiting_for_action;
extern HANDLE button_events[6]; // FCKRA

enum {
	P0C0, P0C1, P1C0, P1C1, P2C0, P2C1, P3C0, P3C1, P4C0, P4C1, 
	P5C0, P5C1,	P6C0, P6C1, P7C0, P7C1, P8C0, P8C1, P9C0, P9C1,
	CC0, CC1, CC2, CC3, CC4,
	PN0, PN1, PN2, PN3, PN4, PN5, PN6, PN7, PN8, PN9,
	PB0, PB1, PB2, PB3, PB4, PB5, PB6, PB7, PB8, PB9,
	CIB,
	FB, CB, KB, RB, AB, SO, LT, PFB, APB,
	BET0, BET1, BET2, BET3, BET4, BET5, BET6, BET7, BET8, BET9,
	MACRO
};

// CManualModeDlg dialog
class CManualModeDlg : public CDialog 
{
public:
	CManualModeDlg(CWnd* pParent = NULL);	// standard constructor
	enum { IDD = IDD_MANUALMODE_DIALOG };
	virtual BOOL DestroyWindow();

	void ResetSwag();
	void SetSeated(int chair, bool s);
	void SetActive(int chair, bool a);
	void SetDealer(int chair);
	void SetBalance(int chair, double b);
	void SetBet(int chair, double b);
	void SetSBlind(double b);
	void SetBBlind(double b);
	void SetBBet(double b);
	void SetAnte(double a);
	void SetPot(double p);
	void SetGType(std::string t);
	void SetHandNumber(int n);
	void SetNetwork(std::string n);
	void SetTournament(bool t);
	void PostSB(int chair);
	void PostBB(int chair);
	void SetButton(char b, bool a);
	void SetCards(int chair, std::string c1, std::string c2);
	void SetFlopCards(std::string c1, std::string c2, std::string c3);
	void SetTurnCard(std::string c1);
	void SetRiverCard(std::string c1);
	void SetName(int chair, std::string name);
	void DoFold(int chair);
	void DoCall(int chair);
	void DoRaise(int chair, double raise = 0);
	void DoAllin(int chair);

	void ProvideEventsHandling();

	void Refresh();

	CString raise_amount;
protected:
	xmlrpc_c::clientSimple* xClient;
	std::string serverUrl;


	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	HICON		m_hIcon;
	CPen		black_pen, green_pen, red_pen, blue_pen, white_dot_pen, white_pen, null_pen;
	CBrush		white_brush, gray_brush, red_brush, yellow_brush;
	LOGFONT		lf;
	CFont		cFont, cFont_sm;

	CardMask			used_cards;
	unsigned int		card[25];  // aligns with enum above 0-19 player cards, 20-24 common cards
	bool				seated[10], active[10];
	bool				dealer[10];
	CString				playername[10];
	CString				playerbalance[10];
	CString				playerbet[10];
	double				pot;
	bool				buttonstate[10]; // fold, call, check, raise, allin, prefold
	CString				handnumber;
	CString				sblind;
	CString				bblind;
	CString				bbet;
	CString				ante;
	int					limit;
	CString				network;
	bool				istournament;

	// For tracking right click locations
	int					click_loc;
	int					click_chair;

	CBitmap				all_cards;
	CString				macro_text;

	// For tracking current state
	int					ncommoncards_last;

	CEditDlg			edit;
	CGameInfoDlg		gameinfo;
	OptionsDlg			dlgOptions;
	CEdit				swag;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

	void draw_button_indicators(void);
	void draw_specific_button_indicator(int button_num, char ch, int left, int top, int right, int bottom);
	void draw_control_indicators(void);
	void draw_seated_active_circle(int chair);
	void draw_center_info_box(void);
	void draw_card(unsigned int card, int left, int top, int shift = 0);
	void draw_dealer_button(int chair); 
	void draw_name_box(int chair); 
	void draw_balance_box(int chair); 
	void draw_player_bet(int chair);
	void clear_scrape_areas(void);
	void get_click_loc(CPoint p);
	void set_card(unsigned int c);
	void set_card(int chair, unsigned int c);
	int get_rank(char r);
	int get_suit(char s);
	void do_fold(void);
	void do_call(void);
	void do_raise(void);
	void do_allin(void);
	void do_scrape_bets_into_pot(void);
	double get_current_bet(void);
	int get_br(void);

	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnBnClickedReset();
	afx_msg void OnBnClickedMacro();
	afx_msg void OnBnClickedPminus();
	afx_msg void OnBnClickedPplus();
	afx_msg void OnBnClickedDminus();
	afx_msg void OnBnClickedDplus();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void On2Clubs();
	afx_msg void On2Diamonds();
	afx_msg void On2Hearts();
	afx_msg void On2Spades();
	afx_msg void On3Clubs();
	afx_msg void On3Diamonds();
	afx_msg void On3Hearts();
	afx_msg void On3Spades();
	afx_msg void On4Clubs();
	afx_msg void On4Diamonds();
	afx_msg void On4Hearts();
	afx_msg void On4Spades();
	afx_msg void On5Clubs();
	afx_msg void On5Diamonds();
	afx_msg void On5Hearts();
	afx_msg void On5Spades();
	afx_msg void On6Clubs();
	afx_msg void On6Diamonds();
	afx_msg void On6Hearts();
	afx_msg void On6Spades();
	afx_msg void On7Clubs();
	afx_msg void On7Diamonds();
	afx_msg void On7Hearts();
	afx_msg void On7Spades();
	afx_msg void On8Clubs();
	afx_msg void On8Diamonds();
	afx_msg void On8Hearts();
	afx_msg void On8Spades();
	afx_msg void On9Clubs();
	afx_msg void On9Diamonds();
	afx_msg void On9Hearts();
	afx_msg void On9Spades();
	afx_msg void OnTClubs();
	afx_msg void OnTDiamonds();
	afx_msg void OnTHearts();
	afx_msg void OnTSpades();
	afx_msg void OnJClubs();
	afx_msg void OnJDiamonds();
	afx_msg void OnJHearts();
	afx_msg void OnJSpades();
	afx_msg void OnQClubs();
	afx_msg void OnQDiamonds();
	afx_msg void OnQHearts();
	afx_msg void OnQSpades();
	afx_msg void OnKClubs();
	afx_msg void OnKDiamonds();
	afx_msg void OnKHearts();
	afx_msg void OnKSpades();
	afx_msg void OnAClubs();
	afx_msg void OnADiamonds();
	afx_msg void OnAHearts();
	afx_msg void OnASpades();
	afx_msg void OnCardBacks();
	afx_msg void OnNoCards();
	afx_msg void OnNoCard();
	afx_msg void OnSeatPlayer();
	afx_msg void OnSitInPlayer();
	afx_msg void OnDealerHere();
	afx_msg void OnFold();
	afx_msg void OnCall();
	afx_msg void OnRaise();
	afx_msg void OnAllin();
public:
	afx_msg void OnEnChangeSwag();
};
