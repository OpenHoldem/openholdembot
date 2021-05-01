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

// OpenHoldem.cpp : Defines the class behaviors for the application.
//
#include "stdafx.h"
#include "OpenHoldem.h"
#include <psapi.h>
#include <windows.h>
#include "..\CTablemap\CTablemap.h"
#include "..\CTablemap\CTableMapAccess.h"
#include "CAutoConnector.h"
#include "CFormulaParser.h"
#include "CHeartbeatThread.h"
#include "CIteratorThread.h"
#include "COpenHoldemHopperCommunication.h"
#include "COpenHoldemTitle.h"
#include "CSessionCounter.h"
#include "DialogFormulaScintilla.h"
#include "MainFrm.h"
#include "..\DLLs\WindowFunctions_DLL\window_functions.h"
#include "OpenHoldemDoc.h"
#include "OpenHoldemView.h"
#include "Singletons.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// Supports MRU
AFX_STATIC_DATA const TCHAR _afxFileSection[] = _T("Recent File List");
AFX_STATIC_DATA const TCHAR _afxFileEntry[] = _T("File%d");
AFX_STATIC_DATA const TCHAR _afxPreviewSection[] = _T("Settings");
AFX_STATIC_DATA const TCHAR _afxPreviewEntry[] = _T("PreviewPages");

// COpenHoldemApp
extern bool Scintilla_RegisterClasses(void *hInstance);
extern bool Scintilla_ReleaseResources();

BEGIN_MESSAGE_MAP(COpenHoldemApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, &COpenHoldemApp::OnAppAbout)
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, &CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CWinApp::OnFileOpen)
	ON_COMMAND(ID_FINISH_INITIALIZATION, &COpenHoldemApp::FinishInitialization)
END_MESSAGE_MAP()

// COpenHoldemApp construction

COpenHoldemApp::COpenHoldemApp() {
}

// COpenHoldemApp destruction
COpenHoldemApp::~COpenHoldemApp() {
}

// The one and only COpenHoldemApp object
COpenHoldemApp theApp;


// COpenHoldemApp initialization
BOOL COpenHoldemApp::InitInstance() {
  // InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
  //
  // This code should probably be called at the VERY beginning,
  // especially to support UNICODE-filenames on Win7/8,
  // which might be as early as the ini-file.
  // http://www.maxinmontreal.com/forums/viewtopic.php?f=110&t=17579&p=122399#p122398
  // http://stackoverflow.com/questions/6633515/mfc-app-assert-fail-at-crecentfilelistadd-on-command-line-fileopen
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls); 
	// Since OH 4.0.0 we always use an ini-file,
	// the one and only in our OH-directory,
	// no matter how it is named.
	// For the technical details please see:
	// http://msdn.microsoft.com/de-de/library/xykfyy20(v=vs.80).aspx

	Scintilla_RegisterClasses(AfxGetInstanceHandle());

	// Initialize richedit2 library
	AfxInitRichEdit2();

	// Change class name of Dialog
	WNDCLASS wc;
	GetClassInfo(AfxGetInstanceHandle(), "#32770", &wc);

	wc.lpszClassName = "OpenHoldemFormula";
	wc.hIcon = AfxGetApp()->LoadIcon(IDI_ICON1);
	RegisterClass(&wc);
  CWinApp::InitInstance();

 	// Initialize OLE libraries
	// Mandatory to call those initialisations. 
	// This will also help win7/8 compatibility 
	// those line are automatically inserted if you create a new MFC project with VS2010
	// http://stackoverflow.com/questions/6633515/mfc-app-assert-fail-at-crecentfilelistadd-on-command-line-fileopen
  // http://www.maxinmontreal.com/forums/viewtopic.php?f=110&t=17579&start=150#p122418
	if (!AfxOleInit())
		return FALSE;
	AfxEnableControlContainer();
  
	// Classes
  // First we have to read the pre4ferences,
  // as start_log() needs to know if the old log has to be deleted...
  free((void*)m_pszProfileName);
  m_pszProfileName = _strdup(IniFilePath().GetString());
  Preferences()->LoadPreferences();
	if (!p_sessioncounter) p_sessioncounter = new CSessionCounter();
	// Start logging immediatelly after the loading the preferences
	// and initializing the sessioncounter, which is necessary for 
	// the filename of the log (oh_0.log, etc).
  /*fn.Format("%s\\oh%d.log", _startup_path, theApp.sessionnum);
  struct stat file_stats = { 0 };
  if (stat(fn.GetString(), &file_stats) == 0) {
    unsigned long int max_file_size = 1E06 * Preferences()->log_max_logsize();
    size_t file_size = file_stats.st_size;
    if (file_size > max_file_size) {
      delete_log();
    }*/
	start_log(p_sessioncounter->session_id(), false); //!!!!!
  // ...then re-Load the preferences immediately after creation 
  // of the log-file again, as We might want to log the preferences too,
  // which was not yet possible some lines above.
  // http://www.maxinmontreal.com/forums/viewtopic.php?f=124&t=20281&p=142334#p142334
  Preferences()->LoadPreferences();
	InstantiateAllSingletons();
  write_log(Preferences()->debug_openholdem(), "[OpenHoldem] Going to load mouse.DLL\n");
	// mouse.dll - failure in load is fatal
	_mouse_dll = LoadLibrary("mouse.dll");
	if (_mouse_dll == NULL)	{
		CString		t = "";
		t.Format("Unable to load mouse.dll, error: %d\n\nExiting.", GetLastError());
		MessageBox_Error_Warning(t, "OpenHoldem mouse.dll ERROR");
		return false;
	}	else {
		_dll_mouse_process_message = (mouse_process_message_t) GetProcAddress(_mouse_dll, "ProcessMessage");
		_dll_mouse_click = (mouse_click_t) GetProcAddress(_mouse_dll, "MouseClick");
		_dll_mouse_click_drag = (mouse_clickdrag_t) GetProcAddress(_mouse_dll, "MouseClickDrag");
		if (_dll_mouse_process_message==NULL || _dll_mouse_click==NULL || _dll_mouse_click_drag==NULL) {
			CString		t = "";
			t.Format("Unable to find all symbols in mouse.dll");
			MessageBox_Error_Warning(t, "OpenHoldem mouse.dll ERROR");
			FreeLibrary(_mouse_dll);
			_mouse_dll = NULL;
			return false;
		}
	write_log(Preferences()->debug_openholdem(), "[OpenHoldem] Going to load keyboard.DLL\n");}
	// keyboard.dll - failure in load is fatal
	_keyboard_dll = LoadLibrary("keyboard.dll");
	if (_keyboard_dll==NULL) {
		CString		t = "";
		t.Format("Unable to load keyboard.dll, error: %d\n\nExiting.", GetLastError());
		MessageBox_Error_Warning(t, "OpenHoldem keyboard.dll ERROR");
		return false;
	}	else {
		_dll_keyboard_process_message = (keyboard_process_message_t) GetProcAddress(_keyboard_dll, "ProcessMessage");
		_dll_keyboard_sendstring = (keyboard_sendstring_t) GetProcAddress(_keyboard_dll, "SendString");
		_dll_keyboard_sendkey = (keyboard_sendkey_t) GetProcAddress(_keyboard_dll, "SendKey");
		if (_dll_keyboard_process_message==NULL || _dll_keyboard_sendstring==NULL || _dll_keyboard_sendkey==NULL)	{
			CString		t = "";
			t.Format("Unable to find all symbols in keyboard.dll");
			MessageBox_Error_Warning(t, "OpenHoldem keyboard.dll ERROR");
			FreeLibrary(_keyboard_dll);
			_keyboard_dll = NULL;
			return false;
		}
	}
	LoadLastRecentlyUsedFileList();
	// Register the application's document templates.  Document templates
	// serve as the connection between documents, frame windows and views
	CSingleDocTemplate* pDocTemplate;
	// Document template and doc/view
  // https://msdn.microsoft.com/en-us/library/hts9a4xz.aspx
	// https://msdn.microsoft.com/en-us/library/d1e9fe7d.aspx
	write_log(Preferences()->debug_openholdem(), "[OpenHoldem] Going to create CSingleDocTemplate()\n");
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(COpenHoldemDoc),
		RUNTIME_CLASS(CMainFrame),	   // main SDI frame window
		RUNTIME_CLASS(COpenHoldemView));
	if (!pDocTemplate) {
		write_log(Preferences()->debug_openholdem(), "[OpenHoldem] Creating CSingleDocTemplate() failed\n");
		return FALSE;
	}
	write_log(Preferences()->debug_openholdem(), "[OpenHoldem] Going to AddDocTemplate()\n");
	AddDocTemplate(pDocTemplate);

	write_log(Preferences()->debug_openholdem(), "[OpenHoldem] Going to EnableShellOpen()\n");
	EnableShellOpen();
	write_log(Preferences()->debug_openholdem(), "[OpenHoldem] Going to RegisterShellFileTypes(false)\n");
	RegisterShellFileTypes(false);
  write_log(Preferences()->debug_openholdem(), "[OpenHoldem] Going to InitializeThreads()\n");
  InitializeThreads();
  write_log(Preferences()->debug_openholdem(), "[OpenHoldem] Going to OpenLastRecentlyUsedFile()\n");
  p_formula_parser->ParseDefaultLibraries(); 
	OpenLastRecentlyUsedFile();
	write_log(Preferences()->debug_openholdem(), "[OpenHoldem] m_pMainWnd = %i\n",
		m_pMainWnd);
	write_log(Preferences()->debug_openholdem(), "[OpenHoldem] Posting message that finishes initialization later\n");
	FinishInitialization();
	write_log(Preferences()->debug_openholdem(), "[OpenHoldem] InitInstance done\n");
	return TRUE;
}

void COpenHoldemApp::FinishInitialization() {
	write_log(Preferences()->debug_openholdem(), "[OpenHoldem] FinishInitialization()\n");
	write_log(Preferences()->debug_openholdem(), "[OpenHoldem] m_pMainWnd = %i\n",
		m_pMainWnd);
	assert(p_openholdem_title != NULL);
	p_openholdem_title->UpdateTitle();
	// The one and only window has been initialized, so show and update it
	if (Preferences()->gui_first_visible() && (p_sessioncounter->session_id() == 0)) {
    m_pMainWnd->ShowWindow(SW_SHOW);
	}	else {
    m_pMainWnd->ShowWindow(SW_MINIMIZE);
	}
	m_pMainWnd->UpdateWindow();
	// call DragAcceptFiles only if there's a suffix
	// In an SDI app, this should occur after ProcessShellCommand
	// Enable drag/drop open
	m_pMainWnd->DragAcceptFiles();
	// Bring main window to front
	m_pMainWnd->SetWindowPos(&CWnd::wndTop, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	m_pMainWnd->SetActiveWindow();
	m_pMainWnd->SetFocus();
	m_pMainWnd->SetForegroundWindow();
}

int COpenHoldemApp::ExitInstance() {
  // timers and threads are already stopped 
  // by CMainFrame::DestroyWindow().
  // Now we cancontinue with singletons.
	DeleteAllSingletons();
	Scintilla_ReleaseResources();
  stop_log();
	return CWinApp::ExitInstance();
}

// CDlgAbout dialog used for App About
class CDlgAbout : public CDialog 
{
public:
	CDlgAbout();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};


CDlgAbout::CDlgAbout() : CDialog(CDlgAbout::IDD) {
}


void CDlgAbout::DoDataExchange(CDataExchange* pDX) {
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CDlgAbout, CDialog)
END_MESSAGE_MAP()

// App command to run the dialog
void COpenHoldemApp::OnAppAbout() {
	CDlgAbout aboutDlg;
	aboutDlg.DoModal();
}

void COpenHoldemApp::LoadLastRecentlyUsedFileList() {
	// Added due to inability to get standard LoadStdProfileSettings working properly
	ASSERT_VALID(this);
	ASSERT(m_pRecentFileList == NULL);

	write_log(Preferences()->debug_openholdem(), "[OpenHoldem] Going to load file history\n");
	if (kNumberOfLastRecentlyUsedFilesInFileMenu > 0) 
	{
		// create file MRU since nMaxMRU not zero
		m_pRecentFileList = new CRecentFileList(0, _afxFileSection, _afxFileEntry, 
			kNumberOfLastRecentlyUsedFilesInFileMenu);
		m_pRecentFileList->ReadList();
	}
	// 0 by default means not set
	m_nNumPreviewPages = GetProfileInt(_afxPreviewSection, _afxPreviewEntry, 0);
}

void COpenHoldemApp::StoreLastRecentlyUsedFileList() {
	m_pRecentFileList->WriteList();
}

CString COpenHoldemApp::GetUsedProfilePath() {
	return GetProfileString(_afxFileSection, "File1");
}

void COpenHoldemApp::OpenLastRecentlyUsedFile() {
	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);
	write_log(Preferences()->debug_openholdem(), "[OpenHoldem] Going to open last recently used file\n");
	// Open the most recently used file. (First on the MRU list.) Get the last
	// file from the registry. We need not account for cmdInfo.m_bRunAutomated and
	// cmdInfo.m_bRunEmbedded as they are processed before we get here.
	if (cmdInfo.m_nShellCommand == CCommandLineInfo::FileNew)	{
		CString sLastPath(GetProfileString(_afxFileSection, "File1"));
    if (!sLastPath.IsEmpty())	{
			write_log(Preferences()->debug_openholdem(), "[OpenHoldem] Last path: %s\n", 
				sLastPath);
			CFile f;
			// If file is there, set to open!
			if (f.Open(sLastPath, CFile::modeRead | CFile::shareDenyWrite))	{
				cmdInfo.m_nShellCommand = CCommandLineInfo::FileOpen;
				cmdInfo.m_strFileName = sLastPath;
				f.Close();
				// Check if Debug Action Traces Profile is used
				if (sLastPath.Right(1) == "d")
					action_trace_profile = TRUE;
			}
		}
	}
	write_log(Preferences()->debug_openholdem(), "[OpenHoldem] Going to dispatch command-line\n");
	// Dispatch commands specified on the command line.  Will fail if
	// app was launched with /RegServer, /Register, /Unregserver or /Unregister.	
	if (!ProcessShellCommand(cmdInfo)) {
		write_log(Preferences()->debug_openholdem(), "[OpenHoldem] Dispatching command-line failed\n");
	}
}

void COpenHoldemApp::InitializeThreads() {
  // Heartbeat thread cares about everything: connecting, scraping, playing
  write_log(Preferences()->debug_openholdem(), "[OpenHoldem] Going to start heartbeat thread\n");
  assert(p_heartbeat_thread == NULL);
  p_heartbeat_thread = new CHeartbeatThread();
  assert(p_heartbeat_thread != NULL);
  p_heartbeat_thread->StartThread();
  // Iterator thread
  p_iterator_thread = new CIteratorThread();
  assert(p_iterator_thread != NULL);
}