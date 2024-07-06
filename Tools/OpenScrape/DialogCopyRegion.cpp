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


// DialogCopyRegion.cpp : implementation file
//

#include "stdafx.h"
#include "DialogCopyRegion.h"


// CDlgCopyRegion dialog

IMPLEMENT_DYNAMIC(CDlgCopyRegion, CDialog)

CDlgCopyRegion::CDlgCopyRegion(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgCopyRegion::IDD, pParent)
{

}

CDlgCopyRegion::~CDlgCopyRegion()
{
}

void CDlgCopyRegion::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SOURCE, m_Source);
	DDX_Control(pDX, IDC_REGIONLIST, m_RegionList);
}


BEGIN_MESSAGE_MAP(CDlgCopyRegion, CDialog)
	ON_BN_CLICKED(IDOK, &CDlgCopyRegion::OnBnClickedOk)
END_MESSAGE_MAP()


// CDlgCopyRegion message handlers

BOOL CDlgCopyRegion::OnInitDialog()
{
	CDialog::OnInitDialog();

	CString t = "Copy " + source_region + " to:";
	m_Source.SetWindowText(t.GetString());

	for (int i=0; i<candidates.GetSize(); i++)  
		m_RegionList.AddString(candidates[i].GetString());

	m_RegionList.SetFocus();

	return FALSE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgCopyRegion::OnBnClickedOk()
{
	selected.RemoveAll();

	CArray<int,int> list_sel_indices;
	list_sel_indices.SetSize(m_RegionList.GetSelCount());
	m_RegionList.GetSelItems(m_RegionList.GetSelCount(), list_sel_indices.GetData()); 

	for (int i=0; i<m_RegionList.GetSelCount(); i++)
	{
		CString s;
		m_RegionList.GetText(list_sel_indices[i], s);
		selected.Add(s);
	}

	OnOK();
}