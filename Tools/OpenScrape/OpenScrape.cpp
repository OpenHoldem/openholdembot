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

// OpenScrape.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include <psapi.h>

#include "OpenScrape.h"
#include "MainFrm.h"
#include "OpenScrapeDoc.h"
#include "OpenScrapeView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

char	_startup_path[MAX_PATH];

// COpenScrapeApp

BEGIN_MESSAGE_MAP(COpenScrapeApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, &COpenScrapeApp::OnAppAbout)
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, &CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CWinApp::OnFileOpen)
  ON_MESSAGE(WM_HOTKEY, &CMainFrame::OnHotKey)
END_MESSAGE_MAP()

// COpenScrapeApp construction
COpenScrapeApp::COpenScrapeApp()
{
    // Save startup directory
    ::GetCurrentDirectory(MAX_PATH - 1, _startup_path);
}

// The one and only COpenScrapeApp object
COpenScrapeApp theApp;

// COpenScrapeApp initialization
BOOL COpenScrapeApp::InitInstance()
{
	HMODULE		hMod;
	HANDLE		hProcess;
	DWORD		curprocid, aProcesses[1024], cbNeeded, cProcesses;
	char		sCurProcessName[MAX_PATH], sProcessName[MAX_PATH];

	// Classes
	if (!p_tablemap)  p_tablemap = new CTablemap;

	// Figure out our session number - get name of current process
	curprocid = GetCurrentProcessId();
	hProcess = OpenProcess( PROCESS_QUERY_INFORMATION |
                               PROCESS_VM_READ,
                               FALSE, curprocid );
	if (hProcess != NULL) {
		if ( EnumProcessModules( hProcess, &hMod, sizeof(hMod), &cbNeeded) ) {
			GetModuleBaseName( hProcess, hMod, sCurProcessName, MAX_PATH );
		}
	}
	CloseHandle(hProcess);

	// Noew look through process list and count number of matching processes
	EnumProcesses( aProcesses, sizeof(aProcesses), &cbNeeded );
	cProcesses = cbNeeded / sizeof(DWORD);
	sessionnum = 0;
	for (int i=0; i<(int) cProcesses; i++) {
		hProcess = OpenProcess( PROCESS_QUERY_INFORMATION |
                               PROCESS_VM_READ,
                               FALSE, aProcesses[i] );
		if (hProcess != NULL) {
			if ( EnumProcessModules( hProcess, &hMod, sizeof(hMod), &cbNeeded) ) {
				GetModuleBaseName( hProcess, hMod, sProcessName, MAX_PATH );
			}
		}
		CloseHandle(hProcess);

		if (strcmp(sCurProcessName, sProcessName)==0) {
			sessionnum++;
		}
	}

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
	SetRegistryKey(_T("OpenScrape"));
	LoadStdProfileSettings(4);  // Load standard INI file options (including MRU)
	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(COpenScrapeDoc),
		RUNTIME_CLASS(CMainFrame),       // main SDI frame window
		RUNTIME_CLASS(COpenScrapeView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);
	// Enable DDE Execute open
	EnableShellOpen();
	RegisterShellFileTypes(TRUE);

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// Dispatch commands specified on the command line.  Will return FALSE if
	// app was launched with /RegServer, /Register, /Unregserver or /Unregister.
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// The one and only window has been initialized, so show and update it
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();
	// call DragAcceptFiles only if there's a suffix
	//  In an SDI app, this should occur after ProcessShellCommand
	m_pMainWnd->DragAcceptFiles();

	// Start tablemap dialog
	m_TableMapDlg = new CDlgTableMap(NULL);
	// Create a desktop-based window, no longer a dialog-based one (BillW)
	// http://www.maxinmontreal.com/forums/viewtopic.php?f=112&t=16464
	m_TableMapDlg->Create(CDlgTableMap::IDD, CWnd::GetDesktopWindow());
	m_TableMapDlg->ShowWindow(SW_SHOW);
  ArrangeWindows();
	return true;
}

void COpenScrapeApp::ArrangeWindows() {
  // https://msdn.microsoft.com/en-us/library/windows/desktop/ms633545%28v=vs.85%29.aspx
  // TM-editor top-left
  m_TableMapDlg->SetWindowPos(NULL,
   	0,     // x-position 	
 	  0,     // y_position, 	
    kSizeXForEditor,
    kSizeYForEditor,
 	  0);
  // Main window 
  this->GetMainWnd()->SetWindowPos(NULL, 
    kSizeXForEditor, // x-positioin: right to the TM-editor
    0,              // y-position
    0, 
    0, 
    SWP_NOSIZE);     // Keep current size
}

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

// App command to run the dialog
void COpenScrapeApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}


// COpenScrapeApp message handlers


int COpenScrapeApp::ExitInstance()
{
	// classes
	if (p_tablemap)  
	{ 
		delete p_tablemap; 
		p_tablemap = NULL; 
	}
	if (m_TableMapDlg) 
	{ 
		delete m_TableMapDlg;
		m_TableMapDlg = NULL;
	}

	return CWinApp::ExitInstance();
}
