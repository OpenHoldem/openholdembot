#ifndef INC_MAINFRM_H
#define INC_MAINFRM_H

#include "..\CTablemap\CTablemap.h"
#include "..\Shared\CCritSec\CCritSec.h"

#define HWND_CHECK_TIMER				1
#define ENABLE_BUTTONS_TIMER    2
#define UPDATE_STATUS_BAR_TIMER 3

class CMainFrame : public CFrameWnd 
{
protected: // create from serialization only
	DECLARE_DYNCREATE(CMainFrame)
	DECLARE_MESSAGE_MAP()

	CMainFrame();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnFileOpen();
	afx_msg void OnEditFormula();
	afx_msg void OnEditForceuserchair();
	afx_msg void OnEditViewLog();
	afx_msg void OnEditTagLog();
  afx_msg void OnEditClearLog();
	afx_msg void OnScraperOutput();
	afx_msg void OnViewShootreplayframe();
  afx_msg void OnManualMode();
	afx_msg void OnEditPreferences();
	afx_msg void OnFileLoadTableMap();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnUpdateStatus(CCmdUI *pCmdUI);
	afx_msg void OnAutoplayer();
	afx_msg void OnValidator();
	afx_msg void OnClickedFlags();

	afx_msg void OnUpdateMenuFileNew(CCmdUI* pCmdUI);
	afx_msg void OnUpdateMenuFileOpen(CCmdUI* pCmdUI);
  afx_msg void OnUpdateMenuFileEdit(CCmdUI* pCmdUI);
	afx_msg void OnUpdateMenuFileLoadProfile(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDllLoadspecificfile(CCmdUI *pCmdUI);
	afx_msg void OnUpdateViewMainToolbar(CCmdUI *pCmdUI);
	afx_msg void OnUpdateViewStatusbar(CCmdUI *pCmdUI);
	afx_msg void OnUpdateViewShootreplayframe(CCmdUI *pCmdUI);
  afx_msg void OnUpdateViewScraperOutput(CCmdUI *pCmdUI);
	afx_msg void OnUpdateEditForceuserchair(CCmdUI *pCmdUI);

	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnHelpProblemSolver();
public:
	afx_msg void OnHelp();
	afx_msg void OnHelpOpenPPL();
	afx_msg void OnHelpForums();
public:
  void KillTimers();
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual ~CMainFrame();
	virtual BOOL DestroyWindow();
	CString	_exec_filename;
public:
	// public accessors
	const bool wait_cursor() { return _wait_cursor; }
public:
	void DisableButtonsOnConnect();
	void EnableButtonsOnDisconnect();
	void UnattachOHFromPokerWindow();
public:
	void ResetDisplay();
public:
#define ENT CSLock lock(m_critsec);
	// public mutators
	void set_wait_cursor(const bool b) { ENT _wait_cursor = b; }
#undef ENT

private:
	// private variables - use public accessors and public mutators to address these
	bool			_wait_cursor;	 // Used if we need to display a wait cursor anywhere

private:
	// private functions and variables - not available via accessors or mutators
	int CreateStatusBar(void);

	RECT			_prev_att_rect, _prev_wrect;

	CCritSec		m_critsec;
};

// used by EnumProcTopLevelWindowList function
extern CArray <STableList, STableList>		g_tlist; 

BOOL CALLBACK EnumProcTopLevelWindowList(HWND hwnd, LPARAM lparam);

// Accessor function because p_mainframe never gets initialized.
CMainFrame* PMainframe();

#endif /* INC_MAINFRM_H */
