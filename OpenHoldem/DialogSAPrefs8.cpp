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

// DialogSAPrefs8.cpp : implementation file
//

#include "pch.h"

#include "SAPrefsSubDlg.h"
#include "DialogSAPrefs8.h"



// CDlgSAPrefs8 dialog

IMPLEMENT_DYNAMIC(CDlgSAPrefs8, CDialog)

CDlgSAPrefs8::CDlgSAPrefs8(CWnd* pParent /*=NULL*/)
		: CSAPrefsSubDlg(CDlgSAPrefs8::IDD, pParent)
{
}

CDlgSAPrefs8::~CDlgSAPrefs8()
{
}

void CDlgSAPrefs8::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_RECORD_NEVER, m_RecordNever);
	DDX_Control(pDX, IDC_RECORD_MY_TURN, m_RecordMyTurn);
  DDX_Control(pDX, IDC_RECORD_EVERY_CHANGE_PLAYING, m_RecordEveryChangePlaying);
  DDX_Control(pDX, IDC_RECORD_EVERY_CHANGE, m_RecordEveryChange);
	DDX_Control(pDX, IDC_MAXFRAMES, m_MaxFrames);
	DDX_Control(pDX, IDC_MAXFRAMES_SPIN, m_MaxFramesSpin);
}

// CDlgSAPrefs8 message handlers
BOOL CDlgSAPrefs8::OnInitDialog() {
	CString text = "";
  CSAPrefsSubDlg::OnInitDialog();
  // Starting with 3 to avoid confusion with old values (on off)
  // and defaulting to off
	switch (Preferences()->replay_record()) {
    case kShootReplyFramesOnMyTurn:
      m_RecordMyTurn.SetCheck(BST_CHECKED);
      break;
     case kShootReplyFramesOnEveryChangeWhilePlaying:
      m_RecordEveryChangePlaying.SetCheck(BST_CHECKED);
      break;
    case kShootReplyFramesOnEveryChange:
      m_RecordEveryChange.SetCheck(BST_CHECKED);
      break;
    default:
      // kShootReplyFramesNever
      m_RecordNever.SetCheck(BST_CHECKED);
      break;
  }
	text.Format("%d", Preferences()->replay_max_frames());
	m_MaxFrames.SetWindowText(text.GetString());
	m_MaxFramesSpin.SetRange(1, 1000);
	m_MaxFramesSpin.SetPos(Preferences()->replay_max_frames());
	m_MaxFramesSpin.SetBuddy(&m_MaxFrames);
  return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgSAPrefs8::OnOK() {
	CString			text = "";
  if (m_RecordMyTurn.GetCheck() == BST_CHECKED){
		Preferences()->SetValue(k_prefs_replay_record, 
      kShootReplyFramesOnMyTurn);
	} else if (m_RecordEveryChangePlaying.GetCheck() == BST_CHECKED){
		Preferences()->SetValue(k_prefs_replay_record, 
      kShootReplyFramesOnEveryChangeWhilePlaying);
	} else if (m_RecordEveryChange.GetCheck() == BST_CHECKED){
		Preferences()->SetValue(k_prefs_replay_record, 
      kShootReplyFramesOnEveryChange);
	} else {
    // default off
    Preferences()->SetValue(k_prefs_replay_record, 
      kShootReplyFramesNever);
  }
  m_MaxFrames.GetWindowText(text);
	Preferences()->SetValue(k_prefs_replay_max_frames, atoi(text.GetString()));
  CSAPrefsSubDlg::OnOK();
}