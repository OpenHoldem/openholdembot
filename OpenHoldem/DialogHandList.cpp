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

// DialogHandList.cpp : implementation file
//

#include "pch.h"
#include "DialogHandList.h"


// CDlgHandList dialog
CDlgHandList::CDlgHandList(CWnd* pParent /*=NULL*/)
		: CDialog(CDlgHandList::IDD, pParent)
{
	nhands = 0;
}

CDlgHandList::~CDlgHandList()
{
}

void CDlgHandList::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_HANDLIST_NAME, m_HandList_Name);
	DDX_Control(pDX, IDC_CHECK1, m_Check[k_rank_ace][k_rank_ace]);
	DDX_Control(pDX, IDC_CHECK2, m_Check[k_rank_ace][k_rank_king]);
	DDX_Control(pDX, IDC_CHECK3, m_Check[k_rank_ace][k_rank_queen]);
	DDX_Control(pDX, IDC_CHECK4, m_Check[k_rank_ace][k_rank_jack]);
	DDX_Control(pDX, IDC_CHECK5, m_Check[k_rank_ace][10]);
	DDX_Control(pDX, IDC_CHECK6, m_Check[k_rank_ace][9]);
	DDX_Control(pDX, IDC_CHECK7, m_Check[k_rank_ace][8]);
	DDX_Control(pDX, IDC_CHECK8, m_Check[k_rank_ace][7]);
	DDX_Control(pDX, IDC_CHECK9, m_Check[k_rank_ace][6]);
	DDX_Control(pDX, IDC_CHECK10, m_Check[k_rank_ace][5]);
	DDX_Control(pDX, IDC_CHECK11, m_Check[k_rank_ace][4]);
	DDX_Control(pDX, IDC_CHECK12, m_Check[k_rank_ace][3]);
	DDX_Control(pDX, IDC_CHECK13, m_Check[k_rank_ace][2]);
	DDX_Control(pDX, IDC_CHECK14, m_Check[k_rank_king][k_rank_ace]);
	DDX_Control(pDX, IDC_CHECK15, m_Check[k_rank_king][k_rank_king]);
	DDX_Control(pDX, IDC_CHECK16, m_Check[k_rank_king][k_rank_queen]);
	DDX_Control(pDX, IDC_CHECK17, m_Check[k_rank_king][k_rank_jack]);
	DDX_Control(pDX, IDC_CHECK18, m_Check[k_rank_king][10]);
	DDX_Control(pDX, IDC_CHECK19, m_Check[k_rank_king][9]);
	DDX_Control(pDX, IDC_CHECK20, m_Check[k_rank_king][8]);
	DDX_Control(pDX, IDC_CHECK21, m_Check[k_rank_king][7]);
	DDX_Control(pDX, IDC_CHECK22, m_Check[k_rank_king][6]);
	DDX_Control(pDX, IDC_CHECK23, m_Check[k_rank_king][5]);
	DDX_Control(pDX, IDC_CHECK24, m_Check[k_rank_king][4]);
	DDX_Control(pDX, IDC_CHECK25, m_Check[k_rank_king][3]);
	DDX_Control(pDX, IDC_CHECK26, m_Check[k_rank_king][2]);
	DDX_Control(pDX, IDC_CHECK27, m_Check[k_rank_queen][k_rank_ace]);
	DDX_Control(pDX, IDC_CHECK28, m_Check[k_rank_queen][k_rank_king]);
	DDX_Control(pDX, IDC_CHECK29, m_Check[k_rank_queen][k_rank_queen]);
	DDX_Control(pDX, IDC_CHECK30, m_Check[k_rank_queen][k_rank_jack]);
	DDX_Control(pDX, IDC_CHECK31, m_Check[k_rank_queen][10]);
	DDX_Control(pDX, IDC_CHECK32, m_Check[k_rank_queen][9]);
	DDX_Control(pDX, IDC_CHECK33, m_Check[k_rank_queen][8]);
	DDX_Control(pDX, IDC_CHECK34, m_Check[k_rank_queen][7]);
	DDX_Control(pDX, IDC_CHECK35, m_Check[k_rank_queen][6]);
	DDX_Control(pDX, IDC_CHECK36, m_Check[k_rank_queen][5]);
	DDX_Control(pDX, IDC_CHECK37, m_Check[k_rank_queen][4]);
	DDX_Control(pDX, IDC_CHECK38, m_Check[k_rank_queen][3]);
	DDX_Control(pDX, IDC_CHECK39, m_Check[k_rank_queen][2]);
	DDX_Control(pDX, IDC_CHECK40, m_Check[k_rank_jack][k_rank_ace]);
	DDX_Control(pDX, IDC_CHECK41, m_Check[k_rank_jack][k_rank_king]);
	DDX_Control(pDX, IDC_CHECK42, m_Check[k_rank_jack][k_rank_queen]);
	DDX_Control(pDX, IDC_CHECK43, m_Check[k_rank_jack][k_rank_jack]);
	DDX_Control(pDX, IDC_CHECK44, m_Check[k_rank_jack][10]);
	DDX_Control(pDX, IDC_CHECK45, m_Check[k_rank_jack][9]);
	DDX_Control(pDX, IDC_CHECK46, m_Check[k_rank_jack][8]);
	DDX_Control(pDX, IDC_CHECK47, m_Check[k_rank_jack][7]);
	DDX_Control(pDX, IDC_CHECK48, m_Check[k_rank_jack][6]);
	DDX_Control(pDX, IDC_CHECK49, m_Check[k_rank_jack][5]);
	DDX_Control(pDX, IDC_CHECK50, m_Check[k_rank_jack][4]);
	DDX_Control(pDX, IDC_CHECK51, m_Check[k_rank_jack][3]);
	DDX_Control(pDX, IDC_CHECK52, m_Check[k_rank_jack][2]);
	DDX_Control(pDX, IDC_CHECK53, m_Check[10][k_rank_ace]);
	DDX_Control(pDX, IDC_CHECK54, m_Check[10][k_rank_king]);
	DDX_Control(pDX, IDC_CHECK55, m_Check[10][k_rank_queen]);
	DDX_Control(pDX, IDC_CHECK56, m_Check[10][k_rank_jack]);
	DDX_Control(pDX, IDC_CHECK57, m_Check[10][10]);
	DDX_Control(pDX, IDC_CHECK58, m_Check[10][9]);
	DDX_Control(pDX, IDC_CHECK59, m_Check[10][8]);
	DDX_Control(pDX, IDC_CHECK60, m_Check[10][7]);
	DDX_Control(pDX, IDC_CHECK61, m_Check[10][6]);
	DDX_Control(pDX, IDC_CHECK62, m_Check[10][5]);
	DDX_Control(pDX, IDC_CHECK63, m_Check[10][4]);
	DDX_Control(pDX, IDC_CHECK64, m_Check[10][3]);
	DDX_Control(pDX, IDC_CHECK65, m_Check[10][2]);
	DDX_Control(pDX, IDC_CHECK66, m_Check[9][k_rank_ace]);
	DDX_Control(pDX, IDC_CHECK67, m_Check[9][k_rank_king]);
	DDX_Control(pDX, IDC_CHECK68, m_Check[9][k_rank_queen]);
	DDX_Control(pDX, IDC_CHECK69, m_Check[9][k_rank_jack]);
	DDX_Control(pDX, IDC_CHECK70, m_Check[9][10]);
	DDX_Control(pDX, IDC_CHECK71, m_Check[9][9]);
	DDX_Control(pDX, IDC_CHECK72, m_Check[9][8]);
	DDX_Control(pDX, IDC_CHECK73, m_Check[9][7]);
	DDX_Control(pDX, IDC_CHECK74, m_Check[9][6]);
	DDX_Control(pDX, IDC_CHECK75, m_Check[9][5]);
	DDX_Control(pDX, IDC_CHECK76, m_Check[9][4]);
	DDX_Control(pDX, IDC_CHECK77, m_Check[9][3]);
	DDX_Control(pDX, IDC_CHECK78, m_Check[9][2]);
	DDX_Control(pDX, IDC_CHECK79, m_Check[8][k_rank_ace]);
	DDX_Control(pDX, IDC_CHECK80, m_Check[8][k_rank_king]);
	DDX_Control(pDX, IDC_CHECK81, m_Check[8][k_rank_queen]);
	DDX_Control(pDX, IDC_CHECK82, m_Check[8][k_rank_jack]);
	DDX_Control(pDX, IDC_CHECK83, m_Check[8][10]);
	DDX_Control(pDX, IDC_CHECK84, m_Check[8][9]);
	DDX_Control(pDX, IDC_CHECK85, m_Check[8][8]);
	DDX_Control(pDX, IDC_CHECK86, m_Check[8][7]);
	DDX_Control(pDX, IDC_CHECK87, m_Check[8][6]);
	DDX_Control(pDX, IDC_CHECK88, m_Check[8][5]);
	DDX_Control(pDX, IDC_CHECK89, m_Check[8][4]);
	DDX_Control(pDX, IDC_CHECK90, m_Check[8][3]);
	DDX_Control(pDX, IDC_CHECK91, m_Check[8][2]);
	DDX_Control(pDX, IDC_CHECK92, m_Check[7][k_rank_ace]);
	DDX_Control(pDX, IDC_CHECK93, m_Check[7][k_rank_king]);
	DDX_Control(pDX, IDC_CHECK94, m_Check[7][k_rank_queen]);
	DDX_Control(pDX, IDC_CHECK95, m_Check[7][k_rank_jack]);
	DDX_Control(pDX, IDC_CHECK96, m_Check[7][10]);
	DDX_Control(pDX, IDC_CHECK97, m_Check[7][9]);
	DDX_Control(pDX, IDC_CHECK98, m_Check[7][8]);
	DDX_Control(pDX, IDC_CHECK99, m_Check[7][7]);
	DDX_Control(pDX, IDC_CHECK100, m_Check[7][6]);
	DDX_Control(pDX, IDC_CHECK101, m_Check[7][5]);
	DDX_Control(pDX, IDC_CHECK102, m_Check[7][4]);
	DDX_Control(pDX, IDC_CHECK103, m_Check[7][3]);
	DDX_Control(pDX, IDC_CHECK104, m_Check[7][2]);
	DDX_Control(pDX, IDC_CHECK105, m_Check[6][k_rank_ace]);
	DDX_Control(pDX, IDC_CHECK106, m_Check[6][k_rank_king]);
	DDX_Control(pDX, IDC_CHECK107, m_Check[6][k_rank_queen]);
	DDX_Control(pDX, IDC_CHECK108, m_Check[6][k_rank_jack]);
	DDX_Control(pDX, IDC_CHECK109, m_Check[6][10]);
	DDX_Control(pDX, IDC_CHECK110, m_Check[6][9]);
	DDX_Control(pDX, IDC_CHECK111, m_Check[6][8]);
	DDX_Control(pDX, IDC_CHECK112, m_Check[6][7]);
	DDX_Control(pDX, IDC_CHECK113, m_Check[6][6]);
	DDX_Control(pDX, IDC_CHECK114, m_Check[6][5]);
	DDX_Control(pDX, IDC_CHECK115, m_Check[6][4]);
	DDX_Control(pDX, IDC_CHECK116, m_Check[6][3]);
	DDX_Control(pDX, IDC_CHECK117, m_Check[6][2]);
	DDX_Control(pDX, IDC_CHECK118, m_Check[5][k_rank_ace]);
	DDX_Control(pDX, IDC_CHECK119, m_Check[5][k_rank_king]);
	DDX_Control(pDX, IDC_CHECK120, m_Check[5][k_rank_queen]);
	DDX_Control(pDX, IDC_CHECK121, m_Check[5][k_rank_jack]);
	DDX_Control(pDX, IDC_CHECK122, m_Check[5][10]);
	DDX_Control(pDX, IDC_CHECK123, m_Check[5][9]);
	DDX_Control(pDX, IDC_CHECK124, m_Check[5][8]);
	DDX_Control(pDX, IDC_CHECK125, m_Check[5][7]);
	DDX_Control(pDX, IDC_CHECK126, m_Check[5][6]);
	DDX_Control(pDX, IDC_CHECK127, m_Check[5][5]);
	DDX_Control(pDX, IDC_CHECK128, m_Check[5][4]);
	DDX_Control(pDX, IDC_CHECK129, m_Check[5][3]);
	DDX_Control(pDX, IDC_CHECK130, m_Check[5][2]);
	DDX_Control(pDX, IDC_CHECK131, m_Check[4][k_rank_ace]);
	DDX_Control(pDX, IDC_CHECK132, m_Check[4][k_rank_king]);
	DDX_Control(pDX, IDC_CHECK133, m_Check[4][k_rank_queen]);
	DDX_Control(pDX, IDC_CHECK134, m_Check[4][k_rank_jack]);
	DDX_Control(pDX, IDC_CHECK135, m_Check[4][10]);
	DDX_Control(pDX, IDC_CHECK136, m_Check[4][9]);
	DDX_Control(pDX, IDC_CHECK137, m_Check[4][8]);
	DDX_Control(pDX, IDC_CHECK138, m_Check[4][7]);
	DDX_Control(pDX, IDC_CHECK139, m_Check[4][6]);
	DDX_Control(pDX, IDC_CHECK140, m_Check[4][5]);
	DDX_Control(pDX, IDC_CHECK141, m_Check[4][4]);
	DDX_Control(pDX, IDC_CHECK142, m_Check[4][3]);
	DDX_Control(pDX, IDC_CHECK143, m_Check[4][2]);
	DDX_Control(pDX, IDC_CHECK144, m_Check[3][k_rank_ace]);
	DDX_Control(pDX, IDC_CHECK145, m_Check[3][k_rank_king]);
	DDX_Control(pDX, IDC_CHECK146, m_Check[3][k_rank_queen]);
	DDX_Control(pDX, IDC_CHECK147, m_Check[3][k_rank_jack]);
	DDX_Control(pDX, IDC_CHECK148, m_Check[3][10]);
	DDX_Control(pDX, IDC_CHECK149, m_Check[3][9]);
	DDX_Control(pDX, IDC_CHECK150, m_Check[3][8]);
	DDX_Control(pDX, IDC_CHECK151, m_Check[3][7]);
	DDX_Control(pDX, IDC_CHECK152, m_Check[3][6]);
	DDX_Control(pDX, IDC_CHECK153, m_Check[3][5]);
	DDX_Control(pDX, IDC_CHECK154, m_Check[3][4]);
	DDX_Control(pDX, IDC_CHECK155, m_Check[3][3]);
	DDX_Control(pDX, IDC_CHECK156, m_Check[3][2]);
	DDX_Control(pDX, IDC_CHECK157, m_Check[2][k_rank_ace]);
	DDX_Control(pDX, IDC_CHECK158, m_Check[2][k_rank_king]);
	DDX_Control(pDX, IDC_CHECK159, m_Check[2][k_rank_queen]);
	DDX_Control(pDX, IDC_CHECK160, m_Check[2][k_rank_jack]);
	DDX_Control(pDX, IDC_CHECK161, m_Check[2][10]);
	DDX_Control(pDX, IDC_CHECK162, m_Check[2][9]);
	DDX_Control(pDX, IDC_CHECK163, m_Check[2][8]);
	DDX_Control(pDX, IDC_CHECK164, m_Check[2][7]);
	DDX_Control(pDX, IDC_CHECK165, m_Check[2][6]);
	DDX_Control(pDX, IDC_CHECK166, m_Check[2][5]);
	DDX_Control(pDX, IDC_CHECK167, m_Check[2][4]);
	DDX_Control(pDX, IDC_CHECK168, m_Check[2][3]);
	DDX_Control(pDX, IDC_CHECK169, m_Check[2][2]);
	DDX_Control(pDX, IDC_COMMENT, m_CommentST);
}

BEGIN_MESSAGE_MAP(CDlgHandList, CDialog)
	ON_BN_CLICKED(IDOK, &CDlgHandList::OnBnClickedOk)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_CHECK1, IDC_CHECK169, OnCheckClick)
	ON_BN_CLICKED(IDANYPP, &CDlgHandList::OnBnClickedPP)
	ON_BN_CLICKED(IDBROADWAYS, &CDlgHandList::OnBnClickedBroadways)
	ON_BN_CLICKED(IDALLSUITED, &CDlgHandList::OnBnClickedSuited)
	ON_BN_CLICKED(IDCLEAR, &CDlgHandList::OnBnClickedClear)
END_MESSAGE_MAP()

// CDlgHandList message handlers
BOOL CDlgHandList::OnInitDialog()
{
	CDialog::OnInitDialog();

	CString		s = "";

	// Set title and static text control
  assert(p_list != NULL);
	s.Format("Hand List Editor - %s", p_list->name());
	SetWindowText(s.GetString());
	s.Format("Hand list: %s", p_list->name());
	m_HandList_Name.SetWindowText(s.GetString());

	// Set checked/unchecked status of each box
  assert(p_list != NULL);
	for (int i=2; i<=k_rank_ace; i++) {
		for (int j=2; j<=k_rank_ace; j++) {
			m_Check[i][j].SetCheck(p_list->IsOnList(i, j) ? BST_CHECKED : BST_UNCHECKED);
    }
	}
	OnCheckClick(0);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgHandList::OnCheckClick(UINT controlID)
{
	nhands = 0;
	for (int i=2; i<=k_rank_ace; i++) {
		for (int j=2; j<=k_rank_ace; j++) {
      if (m_Check[i][j].GetCheck() & BST_CHECKED) {
				// pairs	
				if (i==j) nhands += 6;
				// unsuited
				else if (i<j) nhands += 12;
				// suited
				else nhands += 4;
			}
		}
	}
	CString comment = "";
	comment.Format("%d/1326=%.2f%%", nhands, ((double)nhands/1326.0)*100.0);
	m_CommentST.SetWindowText(comment);
}

void CDlgHandList::OnBnClickedOk()
{
	OnCheckClick(0);
  for (int i=2; i<=k_rank_ace; i++) {
		for (int j=2; j<=k_rank_ace; j++) {
      if (m_Check[i][j].GetCheck() & BST_CHECKED) {
        p_list->Set(i, j);
      } else {
        p_list->Remove(i, j);
      }
    }
  }
  p_list->GenerateFunctionTextFromHandlistMatrix();
	OnOK();
}

void CDlgHandList::OnBnClickedPP()
{
	// Set checked status for all PP
	for (int i=2; i<=k_rank_ace; i++) {
		m_Check[i][i].SetCheck(BST_CHECKED);
	}
	OnCheckClick(0);
}

void CDlgHandList::OnBnClickedBroadways()
{
	// Set checked status for broadways
	for (int i=10; i<=k_rank_ace; i++) {
		for (int j=10; j<=k_rank_ace; j++) {
			m_Check[i][j].SetCheck(BST_CHECKED);
		}
	}
	OnCheckClick(0);
}

void CDlgHandList::OnBnClickedSuited()
{
	// Set checked status for suited cards
	for (int i=2; i<=k_rank_ace; i++) {
		for (int j=2; j<i; j++) {
			m_Check[i][j].SetCheck(BST_CHECKED);
		}
	}
	OnCheckClick(0);
}

void CDlgHandList::OnBnClickedClear()
{
	// Set unchecked status for each box
	for (int i=2; i<=k_rank_ace; i++) {
		for (int j=2; j<=k_rank_ace; j++) {
			p_list->Remove(i, j);
			m_Check[i][j].SetCheck(BST_UNCHECKED);
		}
	}
	OnCheckClick(0);
}

