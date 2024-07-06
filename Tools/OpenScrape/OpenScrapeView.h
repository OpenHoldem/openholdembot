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


// OpenScrapeView.h : interface of the COpenScrapeView class
//
#pragma once
//!  Displays Image of csino table, overlayed with regions.
/*!
  also allows movement of regions and calling of file->menu functions.
*/
class COpenScrapeView : public CView
{

protected: // create from serialization only
	COpenScrapeView();
	DECLARE_DYNCREATE(COpenScrapeView)
	DECLARE_MESSAGE_MAP()
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);

	CPen		black_pen, green_pen, red_pen, blue_pen, white_dot_pen, black_dot_pen, null_pen;
	CBrush		white_brush, gray_brush, red_brush, yellow_brush;
	bool		dragging;
	CString		dragged_region;
	int			drag_left_offset, drag_top_offset;

	HCURSOR		hCurDrawRect, hCurStandard;

// Attributes
public:
	static COpenScrapeView * GetView(); 
	COpenScrapeDoc* GetDocument() const;
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual ~COpenScrapeView();
	void blink_rect(void);

	bool		drawing_rect, drawing_started;
	CPoint		drawrect_start;
	CString		drawrect_region;

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
};

#ifndef _DEBUG  // debug version in OpenScrapeView.cpp
inline COpenScrapeDoc* COpenScrapeView::GetDocument() const
   { return reinterpret_cast<COpenScrapeDoc*>(m_pDocument); }
#endif

