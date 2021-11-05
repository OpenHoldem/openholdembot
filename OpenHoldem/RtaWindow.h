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

#include "resource.h"
#include "GraphObject.h"


// CRtaWindow window

class CRtaWindow : public CWnd
{
	DECLARE_DYNCREATE(CRtaWindow)

public:
	CRtaWindow();           // protected constructor used by dynamic creation
	virtual ~CRtaWindow();

public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

	void Init(CWnd * pWnd = NULL);
	void Draw();
	void Close();

	// For log$-symbols
	void SetCustomLogMessage(CString message) { _custom_log_message = message; }
	void SetActionTraceLogMessage(CString message) { _actiontrace_log_message = message; }

	void SetLastAction(CString action) { _last_action = action; }
	void SetHandrank(int handrank) { _handrank = handrank; }
	void SetGto(double prwin, double prtie, double prlos, int nouts, double outodds, double impliedodds, double potodds, double myequity, double potequity, bool prwin_mustplay);
	void SetnOuts(int nouts);

private:
	CGraphObject* m_pPieGraphObject;
	CGraphObject* m_pBarGraphObject;
	CString LastAction();

	// Info to be displayed
	CString _last_action;
	CString	_info_handrank;
	CString	_info_gto;
	CString	_info_nouts;
	CString _custom_log_message;
	CString		_actiontrace_log_message;

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

protected:
	DECLARE_MESSAGE_MAP()
};

extern CRtaWindow *p_rta_window;
