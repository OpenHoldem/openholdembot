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


// SelectTableDialog.cpp : implementation file
//

#include "stdafx.h"
#include "OpenScrape.h"
#include "DialogSelectTable.h"

// CDlgSelectTable dialog
IMPLEMENT_DYNAMIC(CDlgSelectTable, CDialog)

CDlgSelectTable::CDlgSelectTable(CWnd* pParent /*=NULL*/) : CDialog(CDlgSelectTable::IDD, pParent) 
{
}

CDlgSelectTable::~CDlgSelectTable() 
{
}

void CDlgSelectTable::DoDataExchange(CDataExchange* pDX) 
{
	CDialog::DoDataExchange(pDX);
	//DDX_Control(pDX, IDC_LIST_SELECT_TABLE, m_List_Select_Table);
	DDX_Control(pDX, IDC_LIST_SELECT_TABLE, m_HSLB);
	DDX_Control(pDX, IDOK, m_OK);
}


BEGIN_MESSAGE_MAP(CDlgSelectTable, CDialog)
	ON_BN_CLICKED(IDOK, &CDlgSelectTable::OnBnClickedOk)
	ON_LBN_DBLCLK(IDC_LIST_SELECT_TABLE, &CDlgSelectTable::OnLbnDblclkListSelectTable)
	ON_LBN_SELCHANGE(IDC_LIST_SELECT_TABLE, &CDlgSelectTable::OnLbnSelchangeListSelectTable)
END_MESSAGE_MAP()


// CDlgSelectTable message handlers

void CDlgSelectTable::OnBnClickedOk() 
{
	selected_item = m_HSLB.GetCurSel();
	OnOK();
}

BOOL CDlgSelectTable::OnInitDialog() 
{
	int		 N;
	CDialog::OnInitDialog();

	selected_item = -1;

	// Populate listbox
	N = (int) tlist.GetSize();
	for (int i=0; i<N; i++) {
		m_HSLB.AddString(tlist[i].title.GetString());
	}

	m_OK.EnableWindow(false);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgSelectTable::OnLbnDblclkListSelectTable() 
{
	OnBnClickedOk();
}

void CDlgSelectTable::OnLbnSelchangeListSelectTable() 
{
	m_OK.EnableWindow(true);
}
