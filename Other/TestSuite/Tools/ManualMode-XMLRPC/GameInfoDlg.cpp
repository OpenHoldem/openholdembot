// DialogLockBlinds.cpp : implementation file
//

#include "stdafx.h"
#include "GameInfoDlg.h"
#include "debug.h"
#include "ManualModeDlg.h"

// CGameInfoDlg dialog
CGameInfoDlg::CGameInfoDlg(CWnd* pParent /*=NULL*/)	: CDialog(CGameInfoDlg::IDD, pParent) 
{
	SetUnhandledExceptionFilter(MyUnHandledExceptionFilter);

	pPlacement = NULL;
}

CGameInfoDlg::~CGameInfoDlg() 
{
	if(pPlacement != NULL)
		free((WINDOWPLACEMENT *) pPlacement);
}

void CGameInfoDlg::DoDataExchange(CDataExchange* pDX) 
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SMALL_BLIND, m_SmallBlind);
	DDX_Control(pDX, IDC_BIG_BLIND, m_BigBlind);
	DDX_Control(pDX, IDC_BIG_BET, m_BigBet);
	DDX_Control(pDX, IDC_ANTE, m_Ante);
	DDX_Control(pDX, IDC_GAME_TYPE, m_GameType);
	DDX_Control(pDX, IDC_NETWORK, m_Network);
	DDX_Control(pDX, IDC_TOURNAMENT, m_Tournament);
}


BEGIN_MESSAGE_MAP(CGameInfoDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CGameInfoDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CGameInfoDlg::OnBnClickedCancel)
END_MESSAGE_MAP()


// CGameInfoDlg message handlers

BOOL CGameInfoDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	CString str;

	m_SmallBlind.SetWindowText(m_sblind);
	m_BigBlind.SetWindowText(m_bblind);
	m_BigBet.SetWindowText(m_bigbet);
	m_Ante.SetWindowText(m_ante);
	m_GameType.AddString("Fixed Limit");
	m_GameType.AddString("Pot Limit");
	m_GameType.AddString("No Limit");
	m_GameType.SetWindowPos(NULL, 81, 121, 138, 75, SWP_SHOWWINDOW);
	m_Network.AddString("Stars");
	m_Network.AddString("Party");
	m_Network.AddString("Ultimate");
	m_Network.AddString("Absolute");
	m_Network.AddString("Microgaming");
	m_Network.AddString("Ongame");
	m_Network.AddString("Cryptologic");
	m_Network.AddString("Pacific");
	m_Network.AddString("FullTilt");
	m_Network.AddString("B2B");
	m_Network.AddString("Tribeca");
	m_Network.AddString("Worldpex");
	m_Network.AddString("iPoker");
	m_Network.AddString("Tain");
	m_Network.AddString("Bodog");
	m_Network.AddString("Everest");
	m_Network.AddString("Boss");
	m_Network.AddString("Betfair");

	m_Network.SelectString(0, m_network);

	if (m_gametype == LIMIT_NL)
		m_GameType.SelectString(0, "No Limit");
	else if (m_gametype == LIMIT_PL)
		m_GameType.SelectString(0, "Pot Limit");
	else
		m_GameType.SelectString(0, "Fixed Limit");

	if (m_tournament)
		m_Tournament.SetCheck(BST_CHECKED);
	else
		m_Tournament.SetCheck(BST_UNCHECKED);

	m_SmallBlind.SetFocus();
	m_SmallBlind.SetSel(MAKEWORD(0, -1));

	if (pPlacement != NULL) SetWindowPlacement(pPlacement);

	return false;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CGameInfoDlg::OnBnClickedOk() 
{
	CString str;

	m_SmallBlind.GetWindowText(m_sblind);
	m_BigBlind.GetWindowText(m_bblind);
	m_BigBet.GetWindowText(m_bigbet);
	m_Ante.GetWindowText(m_ante);
	// The constants here are not identical with the gametype constants
	// we use elsewhere. The constants used here simply depend on the order 
	// of choices in the drop-down-box.
	if (m_GameType.GetCurSel() == 0) {
		m_gametype = LIMIT_FL;
	}
	else if (m_GameType.GetCurSel() == 1) {
		m_gametype = LIMIT_PL;
	}
	else if (m_GameType.GetCurSel() == 2) {
		m_gametype = LIMIT_NL;
	}
	else 
	{
		// This should not happem, but the user can edit the input
		m_gametype = LIMIT_FL;
	}

	m_Network.GetLBText(m_Network.GetCurSel(), m_network);

	int network_selection = m_Network.GetCurSel();
	if(network_selection != CB_ERR)
	{
		m_Network.GetLBText(network_selection, m_network);
	}
	
	m_tournament = m_Tournament.GetCheck() & BST_CHECKED;

	if (pPlacement == NULL) 
		pPlacement = (WINDOWPLACEMENT*) malloc(sizeof(WINDOWPLACEMENT));
	GetWindowPlacement(pPlacement);

	OnOK();
}

void CGameInfoDlg::OnBnClickedCancel()
{
	if (pPlacement == NULL) 
		pPlacement = (WINDOWPLACEMENT*) malloc(sizeof(WINDOWPLACEMENT));
	GetWindowPlacement(pPlacement);

	OnCancel();
}
