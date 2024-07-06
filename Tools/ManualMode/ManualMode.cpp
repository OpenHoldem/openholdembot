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


// ManualMode.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "ManualMode.h"
#include "ManualModeDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

char	_startup_path[MAX_PATH];

// CManualModeApp

BEGIN_MESSAGE_MAP(CManualModeApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CManualModeApp construction

CManualModeApp::CManualModeApp()
{
	// Save startup directory
	::GetCurrentDirectory(MAX_PATH - 1, _startup_path);
}


// The one and only CManualModeApp object

CManualModeApp theApp;


// CManualModeApp initialization

BOOL CManualModeApp::InitInstance()
{
	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));

	/////
	// Change class name
	WNDCLASS wc;
	// Get the info for this class.
	// #32770 is the default class name for dialogs boxes.
	::GetClassInfo(AfxGetInstanceHandle(), "#32770", &wc);
	// Change the name of the class.
	wc.lpszClassName = "OpenHoldemManualMode";
	// Register this class so that MFC can use it.
	AfxRegisterClass(&wc);	

	CManualModeDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}
