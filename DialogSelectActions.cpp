// DialogSelectActions.cpp : implementation file
//

#include "stdafx.h"
#include "OpenHoldem.h"
#include "DialogSelectActions.h"


// CDlgSelectActions dialog

IMPLEMENT_DYNAMIC(CDlgSelectActions, CDialog)

CDlgSelectActions::CDlgSelectActions(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_SELECTACTIONS, pParent)
{

}

CDlgSelectActions::~CDlgSelectActions()
{
}

void CDlgSelectActions::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHECK0, m_Check[0]);
	DDX_Control(pDX, IDC_CHECK1, m_Check[1]);
	DDX_Control(pDX, IDC_CHECK2, m_Check[2]);
	DDX_Control(pDX, IDC_CHECK3, m_Check[3]);
	DDX_Control(pDX, IDC_CHECK4, m_Check[4]);
	DDX_Control(pDX, IDC_CHECK5, m_Check[5]);
	DDX_Control(pDX, IDC_CHECK6, m_Check[6]);
	DDX_Control(pDX, IDC_CHECK7, m_Check[7]);
	DDX_Control(pDX, IDC_CHECK8, m_Check[8]);
	DDX_Control(pDX, IDC_CHECK9, m_Check[9]);
	DDX_Control(pDX, IDC_CHECK10, m_Check[10]);
	DDX_Control(pDX, IDC_CHECK11, m_Check[11]);
	DDX_Control(pDX, IDC_CHECK12, m_Check[12]);
	DDX_Control(pDX, IDC_CHECK13, m_Check[13]);
	DDX_Control(pDX, IDC_CHECK14, m_Check[14]);
	DDX_Control(pDX, IDC_CHECK15, m_Check[15]);
	DDX_Control(pDX, IDC_CHECK16, m_Check[16]);
	DDX_Control(pDX, IDC_CHECK17, m_Check[17]);
	DDX_Control(pDX, IDC_CHECK18, m_Check[18]);
	DDX_Control(pDX, IDC_CHECK19, m_Check[19]);
	DDX_Control(pDX, IDC_CHECK20, m_Check[20]);
	DDX_Control(pDX, IDC_CHECK21, m_Check[21]);
	DDX_Control(pDX, IDC_CHECK22, m_Check[22]);
	DDX_Control(pDX, IDC_CHECK23, m_Check[23]);
	DDX_Control(pDX, IDC_CHECK24, m_Check[24]);
}


BEGIN_MESSAGE_MAP(CDlgSelectActions, CDialog)
	ON_BN_CLICKED(IDOK, &CDlgSelectActions::OnBnClickedOk)
	ON_BN_CLICKED(IDSELECTALL, &CDlgSelectActions::OnBnClickedSelectAll)
	ON_BN_CLICKED(IDCLEAR, &CDlgSelectActions::OnBnClickedClear)
END_MESSAGE_MAP()

// CDlgSelectActions message handlers
BOOL CDlgSelectActions::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set checked status of each box
	OnBnClickedSelectAll();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgSelectActions::OnBnClickedOk()
{
	const char* const strActionStrings[kNbOfActions] = {
		" RaiseTo "," RaiseBy "," Call FORCE"," Fold FORCE"," RaiseMax FORCE"," Check FORCE"," Raise FORCE",
		" BetPot FORCE"," RaisePot FORCE"," BetMax FORCE"," BetMin FORCE"," RaiseMin FORCE"," Allin FORCE",
		" BetThreeFourthPot FORCE"," RaiseThreeFourthPot FORCE"," BetTwoThirdPot FORCE"," RaiseTwoThirdPot FORCE",
		" BetHalfPot FORCE"," RaiseHalfPot FORCE"," BetThirdPot FORCE"," RaiseThirdPot FORCE"," BetFourthPot FORCE",
		" RaiseFourthPot FORCE"," Bet FORCE"," Beep FORCE"
	};
	for (int i = 0; i < kNbOfActions; i++) {
		if (m_Check[i].GetCheck() & BST_CHECKED)
			arrActionStrings.Add(strActionStrings[i]);
	}
	CDialog::OnOK();
}

void CDlgSelectActions::OnBnClickedSelectAll()
{
	// Set checked status of each box
	for (int i = 0; i < kNbOfActions; i++) {
		m_Check[i].SetCheck(BST_CHECKED);
	}
}

void CDlgSelectActions::OnBnClickedClear()
{
	// Set unchecked status of each box
	for (int i = 0; i < kNbOfActions; i++) {
		m_Check[i].SetCheck(BST_UNCHECKED);
	}
}
