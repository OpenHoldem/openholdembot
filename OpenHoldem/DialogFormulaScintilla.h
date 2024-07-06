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

#ifndef INC_DIALOGFORMULASCINTILLA_H
#define INC_DIALOGFORMULASCINTILLA_H

#include "resource.h"
#include "ScintillaWnd.h"
#include "CWinMgr/WinMgr.h"
#include "CWinMgr/SizerBar.h"

// CDlgFormulaScintilla dialog
#define	MENU_UPDATE_TIMER		1
#define	DEBUG_UPDATE_TIMER		2

#define	EDIT_NEW				0
#define	EDIT_RENAME				1
#define	EDIT_DELETE				2
//separator						3
#define	EDIT_UNDO				4
#define	EDIT_REDO				5
//separator						6
#define	EDIT_FIND				7
#define	EDIT_FIND_NEXT			8
#define	EDIT_FIND_PREV			9
//separator						10
#define	EDIT_CUT				11
#define	EDIT_COPY				12
#define	EDIT_PASTE				13
#define	EDIT_DELETE_TEXT		14
//separator						15
#define	EDIT_SELECTALL			16
//separator						17
#define	EDIT_SETTINGS			19
#define	EDIT_HANDLIST			20

#define DEBUG_FDEBUG_LOGGING	0
#define DEBUG_FDEBUG_MYTURN		1

#define SCE_C_DEFAULT 0
#define SCE_C_COMMENT 1
#define SCE_C_COMMENTLINE 2
#define SCE_C_COMMENTDOC 3
#define SCE_C_NUMBER 4
#define SCE_C_WORD 5
#define SCE_C_STRING 6
#define SCE_C_CHARACTER 7
#define SCE_C_UUID 8
#define SCE_C_PREPROCESSOR 9
#define SCE_C_OPERATOR 10
#define SCE_C_IDENTIFIER 11
#define SCE_C_STRINGEOL 12
#define SCE_C_VERBATIM 13
#define SCE_C_REGEX 14
#define SCE_C_COMMENTLINEDOC 15
#define SCE_C_WORD2 16
#define SCE_C_COMMENTDOCKEYWORD 17
#define SCE_C_COMMENTDOCKEYWORDERROR 18
#define SCE_C_GLOBALCLASS 19
#define MAX_STYLE_NUM 19

// mandatory to use toolTip : "http://forums.codeguru.com/showthread.php?231227-MFC-Tree-Control-How-to-use-SetItemData()-and-GetItemData()"
struct toolTipData {
	CString* description;
};

class CScintillaInfo {
 public:
  CScintillaInfo() { _pWnd = NULL; }
  CScintillaInfo(CScintillaWnd *pWnd, const char *name) { _pWnd = pWnd; _name = name; }
  CScintillaInfo(const CScintillaInfo &in) { operator=(in); }
  CScintillaInfo &operator=(const CScintillaInfo &in) { _pWnd = in._pWnd; _name = in._name; return *this; }
 public:
  CScintillaWnd *_pWnd;
  CString _name;
};

class CDlgFormulaScintilla : public CDialog {
 public:
  CDlgFormulaScintilla(CWnd* pParent = NULL);   // standard constructor
  virtual ~CDlgFormulaScintilla();
  virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
  virtual BOOL DestroyWindow();
  void UpdateDebugAuto();

  enum { IDD = IDD_FORMULA_SCINTILLA };

  CScintillaWnd	m_EmptyScinCtrl;
  CArray<CScintillaInfo, CScintillaInfo &> m_ScinArray;
  CScintillaWnd	*m_pActiveScinCtrl;
  CScintillaWnd	*FindScintillaWindow(const char *name);

  CButton		m_ButtonAuto;
  bool			m_wrote_fdebug_header;
  bool			m_dirty;

 protected:
  virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
  virtual BOOL OnInitDialog();
  virtual void OnCancel();
  virtual void PostNcDestroy();
  afx_msg void OnBnClickedApply();
  afx_msg void OnBnClickedCalc();
  afx_msg void OnNew();
  afx_msg void OnRename();
  afx_msg void OnDelete();
  afx_msg void OnHandList();
  afx_msg void OnSize(UINT nType, int cx, int cy);
  afx_msg LRESULT OnWinMgr(WPARAM wp, LPARAM lp);
  afx_msg void OnTvnSelchangingFormulaTree(NMHDR *pNMHDR, LRESULT *pResult);
  afx_msg void OnTvnSelchangedFormulaTree(NMHDR *pNMHDR, LRESULT *pResult);
  afx_msg void OnTvnExpandedFormulaTree(NMHDR *pNMHDR, LRESULT *pResult);
  afx_msg void OnTabSelectionChange(NMHDR *pNMHDR, LRESULT *pResult);
  afx_msg void OnFunctionTabSelectionChange(NMHDR *pNMHDR, LRESULT *pResult);
  afx_msg void OnSymbolTreeTipInfo(NMHDR *pNMHDR, LRESULT *pResult);
  afx_msg void OnTreeContextMenu(NMHDR *pNMHDR, LRESULT *pResult);
  afx_msg void OnSearchUpdate();
  afx_msg void OnBnClickedAuto();
  afx_msg void OnToggleBookmark();
  afx_msg void OnNextBookmark();
  afx_msg void OnPrevBookmark();
  afx_msg BOOL OnToolTipText(UINT nID, NMHDR* pNMHDR, LRESULT* pResult); 
  void ResizeDialogForControlBars();
  afx_msg void OnEditUndo();
  afx_msg void OnEditRedo();
  afx_msg void OnEditCopy();
  afx_msg void OnEditCut();
  afx_msg void OnEditPaste();
  afx_msg void OnEditDeleteText();
  afx_msg void OnEditSelectAll();
  void SortUdfTree();
  afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
  void OnTimer(UINT_PTR nIDEvent);
  void SaveSettingsToRegistry();
  void SetEquiDistantFont(CScintillaWnd *pWnd);
  void SetStyleColors(CScintillaWnd *pWnd);

  BOOL CDlgFormulaScintilla::PreTranslateMessage(MSG* pMsg);
  CScintillaWnd *SetupScintilla(CScintillaWnd *pWnd, const char *title);
  void UpdateAllScintillaKeywords();
  void UpdateScintillaKeywords(CScintillaWnd *pWnd);
  void DeleteScintilla(CScintillaWnd *pWnd);

  // Support Functions for the Dialog
  void StopAutoButton();
  bool PromptToSave();
  void ResizeScintillaWindows();
  void SelectFunctionTab(CScintillaWnd *pCurScin);

  void PopulateSymbols();
  void PopulatePokerTrackerSymbols();
  HTREEITEM AddSymbolTitle(const char *title, const char *description=NULL, HTREEITEM parentItem=NULL);
  HTREEITEM AddSymbolSubTitle(HTREEITEM parentItem, const char *title, const char *description=NULL);
  HTREEITEM AddSymbol(HTREEITEM parentItem, const char *symbol, const char *description);
  HTREEITEM hRawItem, hCatItem;

 private:
  void PopulateFormulaTree();
  void AddFunctionToTree(HTREEITEM hParent, CString name);
  void AddStandardFunctionsToTree(HTREEITEM hParent, int first, int last);
  void DeleteFormerParentItemIfEmpty(HTREEITEM sibbling);
  void CopyTabContentsToFormulaSet();
  void ClearCalcResult();
  void CloseTabOnDelete(CString name);
 protected:
  HTREEITEM FindUDFGroupItem(const char *groupName);
  HTREEITEM FindUDFStartingItem(const char *groupName);
  HTREEITEM MoveTreeItem(HTREEITEM hItem, HTREEITEM hNewParent, const char *name, bool bSelect);
  HTREEITEM FindFormulaWithWindow(const CScintillaWnd *pWnd, HTREEITEM hFirstItem);
  void	  GetGroupName(const char *functionName, CString &groupName);
  void	  RemoveSingleItemGroups();
  void	  GroupUDFs();
  HTREEITEM hUDFItem;
  HTREEITEM hHandListItem;

  void HandleEnables(bool All);

  // Find/Replace Support Functions and Variables
  afx_msg void OnFindReplaceDlg();
  afx_msg LRESULT OnFindReplace(WPARAM wParam, LPARAM lParam);
  afx_msg void OnFindNext();
  afx_msg void OnFindPrev();
	void CloseFindReplaceDialog();

	void DoFind(bool DirDown);

  CFindReplaceDialog *m_pFRDlg;
  CString			m_FindLastSearch;
  bool			  m_FindWholeWord;
  bool			  m_FindMatchCase;

  CMenu			  m_Menu;
  CToolBar		m_toolBar;
  CStatusBar	m_wndStatusBar;
  CTreeCtrl		m_FormulaTree;
  CTreeCtrl		m_SymbolTree;
  CButton			m_ButtonCalc, m_FormulaCancel, m_FormulaApply;
  CEdit			  m_CalcResult;
  CEdit			  m_CalcRelativeLine;
  CEdit			  m_SearchEdit;
  CWinMgr			m_winMgr;				// window manager
  CSizerBar		m_winMgrSizerBar;		// sizer bar
  CTabCtrl		m_TabControl;
  CTabCtrl		m_FunctionTab;

  HACCEL			m_hEditAccelTable;
  HACCEL			m_hDialogAccelTable;

  CString			m_current_edit;			// Name of formula currently in edit window
  bool			  in_startup;
  bool			  ok_to_update_debug;

  CStringArray	m_standard_headings;
  // Tree view for function grouping
  //   * Primary autoplayer functions
  //   * Secondary autoplayer functions
  //   * Ini functions
  //   * PrWin functions
  //   * ICM Functions
  //   * Debug functions
  static const int k_number_of_standard_headings = 7;
  CStringArray		m_standard_functions[k_number_of_standard_headings];
  DECLARE_MESSAGE_MAP()

 private:
  CString ExtractCommentFromHandList(CString HandListAsString);
  void WarnAboutAutoplayerWhenApplyingFormula();
  void SetExtendedWindowTitle(CString additional_information);
 private:
  void FormerShowEnableHideCodeClone(CScintillaWnd *new_pActiveScinCtrl);
  void ReEnableLineNumbersUglyWorkaroundForHugeShankyMonkeyStyleFormulas();
 private:
  HTREEITEM _subtree_handlists;
  HTREEITEM _subtree_user_defined_functions;
 private:
  CFont _edit_font;
};

extern CDlgFormulaScintilla	*m_formulaScintillaDlg;

#endif //INC_DIALOGFORMULASCINTILLA_H