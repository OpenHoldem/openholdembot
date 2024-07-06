/* @doc
 * @module ScintillaWnd.h | Definition of a Scintilla syntax coloring edit control for MFC<nl>
 * This program is an example how to use the excellent scintilla edit control of Neil Hodgson.<nl>
 * See www.scintilla.org for details<nl>
 * Author: Horst Brückner, hb@ec-logic.com<nl>
 * Environment: VisualC++ Version 6, static Build of Scintilla, SciLexer.dll as Lexer<nl>
 */

#ifndef __SCINTILLAWND_H__
#define __SCINTILLAWND_H__

#include "Scintilla/include/Scintilla.h"
#include "Scintilla/include/SciLexer.h"

////////////////////////////////////
// @class CScintillaWnd | Class of a GCL Scintilla syntax coloring edit control for MFC
// @base public | CWnd
//
class CScintillaWnd : public CWnd  
{
public:
// public constructor - destructor
// empty Constructor
	CScintillaWnd();
// destructor
	virtual ~CScintillaWnd();

public:
// public macro members
// return linenumber display flag
   bool GetDisplayLinenumbers (){return m_bLinenumbers;};
// return selection display flag
   bool GetDisplaySelection (){return m_bSelection;};
// return folding margin display flag
   bool GetDisplayFolding (){return m_bFolding;};
// set search flags
   virtual void SetSearchflags (int nSearchflags){m_nSearchflags = nSearchflags;};
   

public:
// public function members
// register a window class to use scintilla in a dialog
   static bool Register(CWinApp *app, UINT id);
// try to load Lexer DLL
   static HMODULE LoadScintillaDll (LPCSTR szLexerDll = NULL);
// create window
   virtual bool Create (LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);
// Set Text to the Scintilla control
   virtual void SetText (LPCSTR szText);
// Get Text from the Scintilla control
   virtual void GetText (CString &strText);
// Get Text from the Scintilla control
   virtual LPSTR GetText();
// calcluate number of chars needed for linenumberdisplay
   virtual int GetLinenumberChars ();
// calcluate number of pixels for linenumber display
   virtual int GetLinenumberWidth ();
// set display of linenumbers on/off
   virtual void SetDisplayLinenumbers(bool bFlag = true);
// set display of selection/bookmark margin on/off
   virtual void SetDisplaySelection(bool bFlag = true);
// set display of source folding margin on/off
   virtual void SetDisplayFolding(bool bFlag = true);
// cut selection to clipboard
   virtual void Cut();
// copy selection to clipboard
   virtual void Copy();
// paste from clipboard
   virtual void Paste();
// clear selection
   virtual void Clear();
// undo last change
   virtual void Undo();
// redo last change
   virtual void Redo();
// check if we can undo
   virtual bool CanUndo();
// check if we can redo
   virtual bool CanRedo();
// check if we have something to paste from clipboard
   virtual bool CanPaste();
// select complete text
   virtual void SelectAll();
// return the current line number
   virtual long GetCurrentLine();
// return the current column number
   virtual long GetCurrentColumn();
// return the position of the anchor
   virtual long GetAnchor();
// return the current character position within the file
   virtual long GetCurrentPosition();
// return the current style number at the current character position
   virtual int GetCurrentStyle();
// return the current folding level at the current character position
   virtual int GetCurrentFoldinglevel();
// set the fontname for a style number
   virtual void SetFontname(int nStyle, LPCSTR szFontname);
// set the fontname height in points for a style number
   virtual void SetFontheight(int nStyle, int nHeight);
// set the foregroundcolor for a style number
   virtual void SetForeground(int nStyle, COLORREF crForeground);
// set the backgroundcolor for a style number
   virtual void SetBackground(int nStyle, COLORREF crBackground);
// set given style to bold
   virtual void SetBold(int nStyle, bool bFlag);
// set given style to bold
   virtual void SetItalic(int nStyle, bool bFlag);
// set given style to bold
   virtual void SetUnderline(int nStyle, bool bFlag);
// get flag if we are in overstrike mode
   virtual bool GetOverstrike();
// set flag for overstrike mode
   virtual void SetOverstrike(bool bOverstrike);
// init the view with reasonable defaults
   virtual void Init();
// called whenever the text is changed - we can update any indicators and do brace matching
   virtual void UpdateUI();
// do default folding 
   virtual void DoDefaultFolding(int nMargin, long lPosition);
// refresh lexer and display after new settings
   virtual void Refresh();
// add a bookmark = marker type 0
   virtual void AddBookmark(long lLine);
// delete a bookmark = marker type 0
   virtual void DeleteBookmark(long lLine);
// check for bookmark = marker type 0
   virtual bool HasBookmark(long lLine);
// Find next bookmark
   virtual void FindNextBookmark();
// Find previous bookmark
   virtual void FindPreviousBookmark();
// goto line
   virtual void GotoLine(long lLine);
// goto position
   virtual void GotoPosition(long lPos);
// search forward for a given text
   virtual bool SearchForward(LPSTR szText);
// search backward for a given text
   virtual bool SearchBackward(LPSTR szText);
// replace a text found by SearchForward or SearchBackward
   virtual void ReplaceSearchedText(LPCSTR szText);
// Set your own lexer
   virtual void SetLexer(int nLexer);
// return start position of selection
   virtual long GetSelectionStart();
// return end position of selection
   virtual long GetSelectionEnd();
// get selected text
   virtual CString GetSelectedText();
// replace all in buffer or selection
   virtual int ReplaceAll(LPCSTR szFind, LPCSTR szReplace, bool bSelection = true);


protected:
// protected data members
// flag if we should display line numbers
   bool m_bLinenumbers;
// flag if we should display selection and bookmark margin
   bool m_bSelection;
// flag if we should display folding margin
   bool m_bFolding;
// search flags
   int  m_nSearchflags;

};
#define STR_SCINTILLAWND _T("Scintilla")

#endif // !__SCINTILLAWND_H__
