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

#ifndef INC_DIALOGSCRAPEROUTPUT_H
#define INC_DIALOGSCRAPEROUTPUT_H

#include "CTablemap/CTablemap.h"
#include "CWinMgr/SizerBar.h"
#include "Resource.h"

// CDlgScraperOutput dialog
class CDlgScraperOutput : public CDialog {
 public:
	CDlgScraperOutput(CWnd* pParent = NULL);   // standard constructor
 public:
  // This method
  //   * checks if a window exists
  //   * destroys it
  //   * resets the pointer to NULL
  // and may be called anytime from anywhere, 
  // especially created to close the dialog on disappearing tables
  static void DestroyWindowSafely();
 public:
	virtual ~CDlgScraperOutput();
	virtual void OnCancel();
	virtual BOOL OnInitDialog();
	virtual BOOL DestroyWindow();
	void UpdateDisplay();
	void Reset();

	enum { IDD = IDD_SCRAPER_OUTPUT };

 protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	virtual void PostNcDestroy();
	afx_msg void OnLbnSelchangeRegionlist();
	afx_msg void OnCbnSelchangeZoom();
	afx_msg void OnPaint();
	void DoBitblt(HBITMAP bitmap, RMapCI r_iter);

	CListBox		m_RegionList;
	CStatic			m_ScraperBitmap;
	CComboBox		m_Zoom;
	CEdit			m_ScraperResult;
	bool			in_startup;

	DECLARE_MESSAGE_MAP()
 private:
	void AddListboxItems();
};


extern CDlgScraperOutput	*m_ScraperOutputDlg;

#endif //INC_DIALOGSCRAPEROUTPUT_H