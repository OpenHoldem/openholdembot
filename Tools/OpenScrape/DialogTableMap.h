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
#include "afxcmn.h"
#include "afxwin.h"
#include "resource.h"
#include "StickyButton.h"

// Region grouping types
enum {BY_TYPE = 1, BY_NAME = 2};

// CDlgTableMap dialog
class CDlgTableMap : public CDialog
{
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnPaint();
	afx_msg void OnRegionChange();
	afx_msg void OnZoomChange();
	afx_msg void OnTvnSelchangedTablemapTree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposLeftSpin(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposTopSpin(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposBottomSpin(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposRightSpin(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposRadiusSpin(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedNew();
	afx_msg void OnBnClickedDelete();
	afx_msg void OnBnClickedEdit();
	afx_msg void OnBnClickedCreateHash0();
	afx_msg void OnBnClickedCreateHash1();
	afx_msg void OnBnClickedCreateHash2();
	afx_msg void OnBnClickedCreateHash3();
	afx_msg void OnBnClickedCreateImage();
	afx_msg void OnBnClickedCreateFont();
	afx_msg void OnBnClickedFontplus();
	afx_msg void OnBnClickedFontminus();
	afx_msg void OnBnClickedNudgeTaller();
	afx_msg void OnBnClickedNudgeShorter();
	afx_msg void OnBnClickedNudgeWider();
	afx_msg void OnBnClickedNudgeNarrower();
	afx_msg void OnBnClickedNudgeBigger();
	afx_msg void OnBnClickedNudgeSmaller();
	afx_msg void OnBnClickedNudgeUpleft();
	afx_msg void OnBnClickedNudgeUp();
	afx_msg void OnBnClickedNudgeUpright();
	afx_msg void OnBnClickedNudgeRight();
	afx_msg void OnBnClickedNudgeDownright();
	afx_msg void OnBnClickedNudgeDown();
	afx_msg void OnBnClickedNudgeDownleft();
	afx_msg void OnBnClickedNudgeLeft();
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg LRESULT OnStickyButtonDown(WPARAM wp, LPARAM lp);
	afx_msg LRESULT OnStickyButtonUp(WPARAM wp, LPARAM lp);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnToolTipText(UINT id, NMHDR* pTTTStruct, LRESULT* pResult);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSizing(UINT nSide, LPRECT lpRect);
	afx_msg void OnTvnKeydownTablemapTree(NMHDR *pNMHDR, LRESULT *pResult);
	HTREEITEM GetRecordTypeNode(HTREEITEM item);
	HTREEITEM GetTextSelItemAndRecordType(CString *sel_text, CString *type_text);
	void clear_bitmap_control(void);
	void draw_region_bitmap(void);
	void draw_image_bitmap(void);
	void disable_and_clear_all(void);
	void update_r$_display(bool dont_update_spinners);
	void update_t$_display();
	COLORREF get_color_under_mouse(UINT *nFlags, CPoint *point);
	CString GetGroupName(CString regionName);
	HTREEITEM FindRegionGroupItem(HTREEITEM hRegionNode, CString groupName);
	HTREEITEM MoveTreeItem(HTREEITEM hItem, HTREEITEM hNewParent, CString name, bool bSelect);
	void RemoveSingleItemGroups(void);
	void CreateHash(int hash_type);
	void SaveNodeExpansionState(CArray <bool, bool> *node_state);
	void RestoreNodeExpansionState(CArray <bool, bool> *node_state);
	int GetType(CString selected_text);
	HTREEITEM InsertGroupedRegion(CString itemText);

	CStatic				m_BitmapFrame;
	CStickyButton		m_Picker;
	CSpinButtonCtrl		m_LeftSpin, m_TopSpin, m_BottomSpin, m_RightSpin, m_RadiusSpin;
	CComboBox			m_Transform, m_Zoom, m_TrackerFontSet, m_TrackerFontNum, m_TrackerCardNum;
	CEdit				m_Alpha, m_Red, m_Green, m_Blue, m_RedAvg, m_GreenAvg, m_BlueAvg, m_Radius, m_Result, m_PixelSeparation;
	CButton				m_New, m_Delete, m_Edit, m_CreateImage, m_CreateFont, m_FontPlus, m_FontMinus;
	CButton				m_CreateHash0, m_CreateHash1, m_CreateHash2, m_CreateHash3;
	CButton				m_NudgeTaller, m_NudgeShorter, m_NudgeWider, m_NudgeNarrower, m_NudgeBigger, m_NudgeSmaller;
	CButton				m_NudgeUpLeft, m_NudgeUp, m_NudgeUpRight, m_NudgeRight, m_NudgeDownRight, m_NudgeDown;
	CButton				m_NudgeDownLeft, m_NudgeLeft;
	CPen				black_pen, green_pen, red_pen, blue_pen, white_dot_pen, null_pen;
	CBrush				white_brush, lt_gray_brush, gray_brush, red_brush, yellow_brush;
	LOGFONT				lf_fixed;
	CFont				separation_font, nudge_font;
	int					separation_font_size;
	CBitmap				picker_bitmap, drawrect_bitmap;
	HBITMAP				h_picker_bitmap, h_drawrect_bitmap;
	bool				picker_cursor;
	HCURSOR				hCurPicker, hCurStandard;
	bool				ignore_changes;
	CStatic				m_status_cards, m_status_fonts;

public:
	virtual BOOL DestroyWindow();
	CDlgTableMap(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgTableMap();
	enum { IDD = IDD_TABLEMAP };
	void create_tree(void);
	void update_display(void);
	void UpdateStatus(void);
	HTREEITEM update_tree(CString node_text);
	void GroupRegions(void);
	void UngroupRegions(void);
	void UpdateDisplayOfAllRegions();
	HTREEITEM GetTypeNode(CString type);
	HTREEITEM FindItem(CString s, HTREEITEM start);

	CTreeCtrl			m_TableMapTree;
	CEdit				m_Left, m_Top, m_Bottom, m_Right, m_xy;
	CStickyButton		m_DrawRect;
	int					region_grouping;

	DECLARE_MESSAGE_MAP()
};


