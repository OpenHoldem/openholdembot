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

#include "CSpaceOptimizedGlobalObject.h"
#include "CHyperLink.h"

class CWhiteInfoBox : public CSpaceOptimizedGlobalObject {
protected:
	virtual int DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
//	DECLARE_MESSAGE_MAP()
 public:
  CWhiteInfoBox();
  ~CWhiteInfoBox();
 public:
	LRESULT OnChildFire(WPARAM wparam,LPARAM lparam);
  void Draw(RECT client_rect, LOGFONT logfont, CDC *pDC,
    CPen *black_pen, CBrush *white_brush);
  // For log$-symbols
  void SetCustomLogMessage(CString message) { _custom_log_message = message; }

  void SetLastAction(CString action) { _last_action = action; }
  void SetHandrank(int handrank) { _handrank = handrank; }
  void SetGto(double prwin, double prtie, double prlos, int nouts, double outodds, double impliedodds, double potodds, double myequity, double potequity, bool prwin_mustplay);
  void SetnOuts(int nouts);

 private:
  CString LastAction();
  CString InfoText();
  CString GtoText();
  // Info to be displayed
  CString _last_action;
  CString	_info_handrank;
  CString	_info_gto;;
  CString	_info_nouts;
 private:
  CHyperLink	m_Link;
  CRect			mLinkRect;
  CString		_custom_log_message;

  int _handrank;
  double _prwin;
  double _prtie;
  double _prlos;
  int _nouts;
  double _outodds;
  double _impliedodds;
  double _potodds;
  double _myequity;
  double _potequity;
  bool _prwin_mustplay;
};

extern CWhiteInfoBox *p_white_info_box;