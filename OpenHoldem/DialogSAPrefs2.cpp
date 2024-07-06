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

// DialogSAPrefs2.cpp : implementation file
//

#include "stdafx.h"

#include "SAPrefsSubDlg.h"
#include "DialogSAPrefs2.h"

#include "WindowFunctions/window_functions.h"


// CDlgSAPrefs2 dialog

IMPLEMENT_DYNAMIC(CDlgSAPrefs2, CSAPrefsSubDlg)

CDlgSAPrefs2::CDlgSAPrefs2(CWnd* pParent /*=NULL*/)
		: CSAPrefsSubDlg(CDlgSAPrefs2::IDD, pParent)
{
}

CDlgSAPrefs2::~CDlgSAPrefs2()
{
}

void CDlgSAPrefs2::DoDataExchange(CDataExchange* pDX)
{
	CSAPrefsSubDlg::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_FRAMEDELAY, m_FrameDelay);
	DDX_Control(pDX, IDC_FRAMEDELAY_SPIN, m_FrameDelay_Spin);
	DDX_Control(pDX, IDC_CLICKDELAY, m_ClickDelay);
	DDX_Control(pDX, IDC_CLICKDELAY_SPIN, m_ClickDelay_Spin);
	DDX_Control(pDX, IDC_SWAGDELAY1, m_SwagDelay1);
	DDX_Control(pDX, IDC_SWAGDELAY1_SPIN, m_SwagDelay1_Spin);
	DDX_Control(pDX, IDC_SWAGDELAY2, m_SwagDelay2);
	DDX_Control(pDX, IDC_SWAGDELAY2_SPIN, m_SwagDelay2_Spin);
	DDX_Control(pDX, IDC_SWAGDELAY3, m_SwagDelay3);
	DDX_Control(pDX, IDC_SWAGDELAY3_SPIN, m_SwagDelay3_Spin);
	DDX_Control(pDX, IDC_AUTOPLAYER_UPON_CONNECTION, m_Autoplayer_Upon_Connection);
	DDX_Control(pDX, IDC_RESTORE_POSITION_AND_FOCUS, m_Restore_Position_And_Focus);
	DDX_Control(pDX, IDC_USE_AUTO_REPLAY, m_Use_Auto_Replay);
}

BEGIN_MESSAGE_MAP(CDlgSAPrefs2, CSAPrefsSubDlg)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SWAGDELAY1_SPIN, &CDlgSAPrefs2::OnDeltaposSwagdelay1Spin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SWAGDELAY2_SPIN, &CDlgSAPrefs2::OnDeltaposSwagdelay2Spin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SWAGDELAY3_SPIN, &CDlgSAPrefs2::OnDeltaposSwagdelay3Spin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_CLICKDELAY_SPIN, &CDlgSAPrefs2::OnDeltaposClickdelaySpin)
END_MESSAGE_MAP()

// CDlgSAPrefs2 message handlers
BOOL CDlgSAPrefs2::OnInitDialog()
{
	CSAPrefsSubDlg::OnInitDialog();
	CString		text = "";

	text.Format("%d", Preferences()->frame_delay());
	m_FrameDelay.SetWindowText(text);
	m_FrameDelay_Spin.SetRange(0, 9);
	m_FrameDelay_Spin.SetPos(Preferences()->frame_delay());
	m_FrameDelay_Spin.SetBuddy(&m_FrameDelay);

	text.Format("%d", Preferences()->click_delay());
	m_ClickDelay.SetWindowText(text);
	m_ClickDelay_Spin.SetRange(0, 5000);
	m_ClickDelay_Spin.SetPos(Preferences()->click_delay());
	m_ClickDelay_Spin.SetBuddy(&m_ClickDelay);

	text.Format("%d", Preferences()->swag_delay_1());
	m_SwagDelay1.SetWindowText(text);
	m_SwagDelay1_Spin.SetRange(0, 5000);
	m_SwagDelay1_Spin.SetPos(Preferences()->swag_delay_1());
	m_SwagDelay1_Spin.SetBuddy(&m_SwagDelay1);

	text.Format("%d", Preferences()->swag_delay_2());
	m_SwagDelay2.SetWindowText(text);
	m_SwagDelay2_Spin.SetRange(0, 5000);
	m_SwagDelay2_Spin.SetPos(Preferences()->swag_delay_2());
	m_SwagDelay2_Spin.SetBuddy(&m_SwagDelay2);

	text.Format("%d", Preferences()->swag_delay_3());
	m_SwagDelay3.SetWindowText(text);
	m_SwagDelay3_Spin.SetRange(0, 5000);
	m_SwagDelay3_Spin.SetPos(Preferences()->swag_delay_3());
	m_SwagDelay3_Spin.SetBuddy(&m_SwagDelay3);

	m_Autoplayer_Upon_Connection.SetCheck(Preferences()->engage_autoplayer() ? BST_CHECKED : BST_UNCHECKED);
	m_Restore_Position_And_Focus.SetCheck(Preferences()->restore_position_and_focus() ? BST_CHECKED : BST_UNCHECKED);
	m_Use_Auto_Replay.SetCheck(Preferences()->use_auto_replay() ? BST_CHECKED : BST_UNCHECKED);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgSAPrefs2::OnOK()
{
	CString			text = "";

	m_FrameDelay.GetWindowText(text);
	if (strtoul(text.GetString(), 0, 10)>MAX_FRAMEDELAY)
	{
		MessageBox_Interactive("Invalid Frame Delay", "ERROR", MB_OK);
		return;
	}
	Preferences()->SetValue(k_prefs_frame_delay, strtoul(text.GetString(), 0, 10));

	m_ClickDelay.GetWindowText(text);
	if (strtoul(text.GetString(), 0, 10)>MAX_CLICKDELAY)
	{
		MessageBox_Error_Warning("Invalid Click Delay");
		return;
	}
	Preferences()->SetValue(k_prefs_click_delay, strtoul(text.GetString(), 0, 10));

	m_SwagDelay1.GetWindowText(text);
	if (strtoul(text.GetString(), 0, 10)>MAX_SWAGDELAY1)
	{
		MessageBox_Interactive("Invalid Betsize Delay (Select to Delete)", "ERROR", MB_OK);
		return;
	}
	Preferences()->SetValue(k_prefs_swag_delay_1, strtoul(text.GetString(), 0, 10));

	m_SwagDelay2.GetWindowText(text);
	if (strtoul(text.GetString(), 0, 10)>MAX_SWAGDELAY2)
	{
		MessageBox_Interactive("Invalid Betsize Delay (Delete to Entry)", "ERROR", MB_OK);
		return;
	}
	Preferences()->SetValue(k_prefs_swag_delay_2, strtoul(text.GetString(), 0, 10));

	m_SwagDelay3.GetWindowText(text);
	if (strtoul(text.GetString(), 0, 10)>MAX_SWAGDELAY3) 
	{
		MessageBox_Interactive("Invalid Betsize Delay (Entry to Confirm)", "ERROR", MB_OK);
		return;
	}
	Preferences()->SetValue(k_prefs_swag_delay_3, strtoul(text.GetString(), 0, 10));

	Preferences()->SetValue(k_prefs_engage_autoplayer, m_Autoplayer_Upon_Connection.GetCheck()==BST_CHECKED ? true : false);
	Preferences()->SetValue(k_prefs_restore_position_and_focus, m_Restore_Position_And_Focus.GetCheck()==BST_CHECKED ? true : false);
	Preferences()->SetValue(k_prefs_use_auto_replay, m_Use_Auto_Replay.GetCheck()==BST_CHECKED ? true : false);
	
	CSAPrefsSubDlg::OnOK();
}

void CDlgSAPrefs2::OnDeltaposSwagdelay1Spin(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	pNMUpDown->iDelta*=100;
	*pResult = 0;
}

void CDlgSAPrefs2::OnDeltaposSwagdelay2Spin(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	pNMUpDown->iDelta*=100;
	*pResult = 0;
}

void CDlgSAPrefs2::OnDeltaposSwagdelay3Spin(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	pNMUpDown->iDelta*=100;
	*pResult = 0;
}

void CDlgSAPrefs2::OnDeltaposClickdelaySpin(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	*pResult = 0;
}