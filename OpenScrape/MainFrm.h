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


// MainFrm.h : interface of the CMainFrame class
//
#pragma once
#include "OpenScrapeDoc.h"

#define		BLINKER_TIMER				1

class CMainFrame : public CFrameWnd {
 public:
  LRESULT OnHotKey(WPARAM wParam, LPARAM lParam);
 protected: // create from serialization only
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnViewRefresh();
	afx_msg void OnViewPrev();
	afx_msg void OnViewNext();
	afx_msg void OnToolsCloneRegions();
	afx_msg void OnToolsCollectFonts();
	afx_msg void OnEditUpdatehashes();
	afx_msg void OnEditDuplicateregion();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnGroupregionsBytype();
	afx_msg void OnGroupregionsByname();
	afx_msg void OnUpdateViewCurrentwindowsize(CCmdUI *pCmdUI);
	afx_msg void OnUpdateEditDuplicateregion(CCmdUI *pCmdUI);
	afx_msg void OnUpdateGroupregionsBytype(CCmdUI *pCmdUI);
	afx_msg void OnUpdateGroupregionsByname(CCmdUI *pCmdUI);
	void SaveBmpPbits(void);
	DECLARE_MESSAGE_MAP()

	CStatusBar		m_wndStatusBar;
	CToolBar		m_wndToolBar;
	CComboBox		m_Transform;
 public:
	virtual BOOL DestroyWindow();
	afx_msg void OnViewConnecttowindow();
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	int					region_grouping;
	virtual ~CMainFrame();
 private:
	bool CreateToolbar();
	bool CreateStatusBar();
	void ForceRedraw();
	void ResizeWindow(COpenScrapeDoc *pDoc);
	void BringOpenScrapeBackToFront();
	void SetTablemapSizeIfUnknown(int size_x, int size_y);
  void CheckIfOHReplayRunning();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
};

// used by EnumProcTopLevelWindowList function
extern CArray <STableList, STableList>		g_tlist; 

BOOL CALLBACK EnumProcTopLevelWindowList(HWND hwnd, LPARAM lparam);
