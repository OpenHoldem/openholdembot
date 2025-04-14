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

	CStatusBar	m_wndStatusBar;
	CToolBar		m_wndToolBar;
 public:
	virtual BOOL DestroyWindow();
	afx_msg void OnViewConnecttowindow();
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual ~CMainFrame();
 private:
	bool CreateToolbar();
	bool CreateStatusBar();
	void ForceRedraw();
	void ResizeWindow(COpenScrapeDoc *pDoc);
	void BringOpenScrapeBackToFront();
	void SetTablemapSizeIfUnknown(int size_x, int size_y);
	void CheckIfOHReplayRunning();
	void CaptureWindow();
  BOOL SaveHBITMAPToFile(HBITMAP hBitmap, LPCTSTR lpszFileName);
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	bool CopyWindowClientRectToTexture(
		ID3D11Device* device,
		ID3D11DeviceContext* context,
		ID3D11Texture2D* sourceTexture,
		HWND hwnd,
		ID3D11Texture2D** outTexture)
	{
		if (!device || !context || !sourceTexture || !hwnd) return false;

		// Step 1: Get client rect and convert to screen coordinates
		RECT clientRect;
		RECT winRect;
		if (!::GetClientRect(hwnd, &clientRect))
			return false;
		if (!::GetWindowRect(hwnd, &winRect))
			return false;

		POINT topLeft = { clientRect.left, clientRect.top };
		::ClientToScreen(hwnd, &topLeft);

		int width = clientRect.right - clientRect.left;
		int height = clientRect.bottom - clientRect.top;

		// Step 2: Create destination texture
		D3D11_TEXTURE2D_DESC srcDesc;
		sourceTexture->GetDesc(&srcDesc);

		D3D11_TEXTURE2D_DESC dstDesc = srcDesc;
		dstDesc.Width = width;
		dstDesc.Height = height;
		dstDesc.BindFlags = 0;
		dstDesc.MiscFlags = 0;
		dstDesc.Usage = D3D11_USAGE_STAGING;
		dstDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;

		ID3D11Texture2D* destTexture = nullptr;
		HRESULT hr = device->CreateTexture2D(&dstDesc, nullptr, &destTexture);
		if (FAILED(hr)) return false;

		// Step 3: Copy region from source to destination
		D3D11_BOX srcBox;
		srcBox.left = 0;         // adjust if sourceTexture isn't full screen
		srcBox.top = topLeft.y - winRect.top;
		srcBox.front = 0;
		srcBox.right = srcBox.left + width;
		srcBox.bottom = srcBox.top + height;
		srcBox.back = 1;

		context->CopySubresourceRegion(destTexture, 0, 0, 0, 0, sourceTexture, 0, &srcBox);

		*outTexture = destTexture; // return the new texture
		return true;
	}
};

// used by EnumProcTopLevelWindowList function
extern CArray <STableList, STableList>		g_tlist; 

BOOL CALLBACK EnumProcTopLevelWindowList(HWND hwnd, LPARAM lparam);
