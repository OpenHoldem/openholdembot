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


// DialogEditFont.cpp : implementation file
//

#include "stdafx.h"
#include <math.h>

#include "OpenScrapeDoc.h"
#include "DialogEditFont.h"

// CDlgEditFont dialog

IMPLEMENT_DYNAMIC(CDlgEditFont, CDialog)

CDlgEditFont::CDlgEditFont(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgEditFont::IDD, pParent)
{
}

CDlgEditFont::~CDlgEditFont()
{
}

void CDlgEditFont::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHARACTER, m_Character);
	DDX_Control(pDX, IDC_TYPE, m_Type);
	DDX_Control(pDX, IDC_CHARLIST, m_CharList);
	DDX_Control(pDX, IDC_PIXELSEP, m_PixelSep);
	DDX_Control(pDX, IDC_DELETE, m_Delete);
	DDX_Control(pDX, IDC_SORT, m_Sort);
}


BEGIN_MESSAGE_MAP(CDlgEditFont, CDialog)
	ON_BN_CLICKED(IDOK, &CDlgEditFont::OnBnClickedOk)
	ON_LBN_SELCHANGE(IDC_CHARLIST, &CDlgEditFont::OnLbnSelchangeCharlist)
	ON_EN_KILLFOCUS(IDC_CHARACTER, &CDlgEditFont::OnEnKillfocusCharacter)
	ON_BN_CLICKED(IDC_DELETE, &CDlgEditFont::OnBnClickedDelete)
	ON_BN_CLICKED(IDC_SORT, &CDlgEditFont::OnBnClickedSort)
END_MESSAGE_MAP()


// CDlgEditFont message handlers
BOOL CDlgEditFont::OnInitDialog()
{
	CDialog::OnInitDialog();

	CString		text;

	// Font
	lf_fixed.lfWidth = 0;
	lf_fixed.lfHeight = 8;
	lf_fixed.lfEscapement = 0;
	lf_fixed.lfOrientation = 0;
	lf_fixed.lfItalic = 0;
	lf_fixed.lfUnderline = 0;
	lf_fixed.lfStrikeOut = 0;
	lf_fixed.lfCharSet = 0;
	lf_fixed.lfOutPrecision = 0;
	lf_fixed.lfClipPrecision = 0;
	lf_fixed.lfQuality = PROOF_QUALITY;
	lf_fixed.lfPitchAndFamily = 0;
	strcpy_s(lf_fixed.lfFaceName, 32, "Courier New");
	separation_font.CreateFontIndirect(&lf_fixed);
	m_PixelSep.SetFont(&separation_font);

	SetWindowText(titletext.GetString());

	m_Delete.EnableWindow(delete_sort_enabled);
	m_Sort.EnableWindow(delete_sort_enabled);

	m_Type.AddString("Type 0");
	m_Type.AddString("Type 1");
	m_Type.AddString("Type 2");
	m_Type.AddString("Type 3");
	m_Type.AddString("Type 4");
	m_Type.AddString("Type 5");
	m_Type.AddString("Type 6");
	m_Type.AddString("Type 7");
	m_Type.AddString("Type 8");
	m_Type.AddString("Type 9");
	text.Format("Type %d", group);
	m_Type.SelectString(-1, text.GetString());
	m_Type.SetWindowPos(NULL, 0, 0, 75, 300, SWP_NOMOVE | SWP_NOZORDER);
	m_Type.EnableWindow(false);

	m_Character.SetWindowText(character.GetString());

	for (int i = 0; i < new_t$_recs[group]->GetCount(); i++)
	{
		STablemapFont &fontrec = new_t$_recs[group]->ElementAt(i);
		text.Format("%c", fontrec.ch);
		m_CharList.AddString(text.GetString());
	}

	m_CharList.SetCurSel(0);
	OnLbnSelchangeCharlist();
	m_CharList.SetFocus();


	return false;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgEditFont::OnLbnSelchangeCharlist()
{
	int				x, bit, top;
	CString			separation, text, charstr, type;
	bool			character[MAX_CHAR_WIDTH][MAX_CHAR_HEIGHT] = { false };	

	if (m_CharList.GetCurSel() != LB_ERR)
	{
		STablemapFont &fontrec = new_t$_recs[group]->ElementAt(m_CharList.GetCurSel());

		// Get set bits
		bit = 0;
		for (int j=0; j<fontrec.x_count; j++)
			for (bit=0; bit<MAX_CHAR_HEIGHT; bit++)
				character[j][bit] = (fontrec.x[j] & (int) pow((double) 2, (double) bit)) != 0;

		// Find topmost line with a set pixel
		for (int j=MAX_CHAR_HEIGHT-1; j>=0; j--)
		{
			for (x=0; x<fontrec.x_count; x++)
			{
				if (character[x][j])
				{
					top = j;
					x = fontrec.x_count + 1;
					j = -1;
				}
			}
		}

		// Create string of set pixels
		separation = "";
		for (int j=top; j>=0; j--)
		{
			for (x=0; x<fontrec.x_count; x++)
			{
				if (character[x][j])
					separation.Append("X");
				else
					separation.Append(" ");
			}
			separation.Append("\r\n");
		}

		// Update controls
		m_PixelSep.SetWindowText(separation);
		type.Format("Type %d", group);
		m_Type.SelectString(-1, type);
		charstr.Format("%c", fontrec.ch);
		m_Character.SetWindowText(charstr.GetString());
	}

	else
	{
		m_PixelSep.SetWindowText("");
		m_Type.SetCurSel(-1);
		m_Character.SetWindowText("");
	}
}

void CDlgEditFont::OnBnClickedOk()
{
	COpenScrapeDoc			*pDoc = COpenScrapeDoc::GetDocument();

	m_Character.GetWindowText(character);
	if (character.GetLength() != 1)
	{
		MessageBox("Font requires exactly one character", "ERROR", MB_OK | MB_TOPMOST);
	}
	else
	{
		OnOK();
	}
}

void CDlgEditFont::OnEnKillfocusCharacter()
{
	CString					temp_character, text;
	int						cur_sel = m_CharList.GetCurSel();

	m_Character.GetWindowText(temp_character);

	STablemapFont &fontrec = new_t$_recs[group]->ElementAt(cur_sel);

	fontrec.ch = temp_character.GetString()[0];

	text.Format("%c", fontrec.ch);
	m_CharList.DeleteString(cur_sel);
	m_CharList.InsertString(cur_sel, text.GetString());
	m_CharList.SetCurSel(cur_sel);
}

void CDlgEditFont::OnBnClickedDelete()
{
	int		sel = m_CharList.GetCurSel();

	new_t$_recs[group]->RemoveAt(sel);
	m_CharList.DeleteString(sel);
	
	if (m_CharList.GetCount()>0)
		m_CharList.SetCurSel(0);

	OnLbnSelchangeCharlist();
	m_CharList.SetFocus();
}

void CDlgEditFont::OnBnClickedSort()
{
	STablemapFont	temp;
	CString			text;
	int				size = (int) new_t$_recs[group]->GetSize();

	// Bubble sort em
	for (int i=0; i<size-1; i++)
	{
		for (int j=i+1; j<size; j++)
		{
			STablemapFont &i_fontrec = new_t$_recs[group]->ElementAt(i);
			STablemapFont &j_fontrec = new_t$_recs[group]->ElementAt(j);
			if (j_fontrec.ch < i_fontrec.ch)
			{
				// hold i rec in temp
				temp.ch = i_fontrec.ch;
				temp.x_count = i_fontrec.x_count;
				for (int k=0; k<i_fontrec.x_count; k++)
					temp.x[k] = i_fontrec.x[k];
				temp.hexmash = i_fontrec.hexmash;

				// copy j rec to i rec
				i_fontrec.ch = j_fontrec.ch;
				i_fontrec.x_count = j_fontrec.x_count;
				for (int k=0; k<j_fontrec.x_count; k++)
					i_fontrec.x[k] = j_fontrec.x[k];
				i_fontrec.hexmash = j_fontrec.hexmash;

				// copy temp to j rec
				j_fontrec.ch = temp.ch;
				j_fontrec.x_count = temp.x_count;
				for (int k=0; k<temp.x_count; k++)
					j_fontrec.x[k] = temp.x[k];
				j_fontrec.hexmash = temp.hexmash;
			}
		}
	}

	// Clear and re-populate list box
	m_CharList.ResetContent();
	for (int i=0; i < size; i++)
	{
		STablemapFont &fontrec = new_t$_recs[group]->ElementAt(i);
		text.Format("%c", fontrec.ch);
		m_CharList.AddString(text.GetString());
	}

	// Select first item in list box
	m_CharList.SetCurSel(0);
	OnLbnSelchangeCharlist();
	m_CharList.SetFocus();
}
