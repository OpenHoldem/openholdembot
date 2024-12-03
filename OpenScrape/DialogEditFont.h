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


#pragma once
#include "afxwin.h"
#include "resource.h"

// CDlgEditFont dialog

class CDlgEditFont : public CDialog
{
	DECLARE_DYNAMIC(CDlgEditFont)

public:
	CDlgEditFont(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgEditFont();
	virtual BOOL OnInitDialog();

	enum { IDD = IDD_EDIT_FONT };
	CString			titletext;
	CString			character;
	int				group;
	bool			delete_sort_enabled;
	CArray <STablemapFont, STablemapFont>		*new_t$_recs[k_max_number_of_font_groups_in_tablemap];

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	afx_msg void OnBnClickedOk();
	afx_msg void OnLbnSelchangeCharlist();
	afx_msg void OnEnKillfocusCharacter();
	afx_msg void OnBnClickedDelete();
	afx_msg void OnBnClickedSort();

	CEdit		m_Character;
	CComboBox	m_Type;
	CListBox	m_CharList;
	CEdit		m_PixelSep;
	LOGFONT		lf_fixed;
	CFont		separation_font;
	CButton m_Delete;
	CButton m_Sort;

	DECLARE_MESSAGE_MAP()
};
