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

// CDlgEditGrHashPoints dialog

class CDlgEditGrHashPoints : public CDialog
{
public:
	CDlgEditGrHashPoints(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgEditGrHashPoints();
	virtual BOOL OnInitDialog();
	virtual BOOL DestroyWindow();

	CArray <STablemapHashPoint, STablemapHashPoint> working_hash_points[4];

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	afx_msg void OnPaint();
	afx_msg void OnCbnSelchangeHashType();
	afx_msg void OnCbnSelchangeSampleImage();
	afx_msg void OnCbnSelchangeZoomLevel();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	void update_bitmap();
	void reset_list_box(int type);

	enum { IDD = IDD_EDIT_GR_HASHPOINTS };
	CComboBox m_Hash_Type, m_Sample_Image, m_Zoom;
	CListBox m_Point_List;
	CStatic m_Sample_Bitmap, m_MousePos;

	DECLARE_MESSAGE_MAP()
};
