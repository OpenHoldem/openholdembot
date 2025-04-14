//******************************************************************************
//
// This file is part of the OpenHoldem project
//    Source code:           https://github.com/OpenHoldem/openholdembot/
//    Forums:                http://www.maxinmontreal.com/forums/index.php
//    Licensed under GPL v3: http://www.gnu.org/licenses/gpl.html
//
//******************************************************************************
//
// Purpose: Monitorinf the scraper directory for
//   * changed files
//   * new files
//   * renamed files
//   and providing a method to check for changes.
//   The autoconnector will then automatically 
//   reload all tablemaps on the next attempt to connect.
//
//******************************************************************************

#include "pch.h"
#include "CFileSystemMonitor.h"


#include "OpenHoldem.h"
#include "Shlwapi.h"

// Class monitors the scraper directory for
// * changed files
// * new files
// * renamed files
// and provides a method to check for changes.
// The autoconnector will then automatically 
// reload all tablemaps on the next attempt to connect.
//
// http://msdn.microsoft.com/en-us/library/system.io.filesystemwatcher.aspx
// http://msdn.microsoft.com/en-us/library/windows/desktop/aa364417(v=vs.85).aspx
// http://msdn.microsoft.com/en-us/library/aa364417%28VS.85%29.aspx
// http://msdn.microsoft.com/en-us/library/aa365261(v=vs.85).aspx

CFileSystemMonitor *p_filesystem_monitor = NULL;

CFileSystemMonitor::CFileSystemMonitor() {
	write_log(Preferences()->debug_filesystem_monitor(), "[CFileSystemMonitor] executing constructor.)\n");
	absolute_path_to_scraper_directory = ScraperDirectory();
	write_log(Preferences()->debug_filesystem_monitor(), "[CFileSystemMonitor] Scraper folder: %s\n", 
		absolute_path_to_scraper_directory);
	// Create directory in case it does not exist (mainly debug sessions).
	CreateDirectory(absolute_path_to_scraper_directory, NULL);
	InitMonitor();
}

CFileSystemMonitor::~CFileSystemMonitor() {
	if (!_scraper_directory_on_a_network_drive) {
		CloseHandle(dwChangeHandle);
	}
}

void CFileSystemMonitor::InitMonitor() {
  _scraper_directory_on_a_network_drive = false;
  // http://msdn.microsoft.com/de-de/library/windows/desktop/bb773640%28v=vs.85%29.aspx
  if (PathIsNetworkPath(absolute_path_to_scraper_directory)) {
    // Network mapped drives are unsupported
    // and can lead to crashes.
    // http://www.maxinmontreal.com/forums/viewtopic.php?f=114&t=17677&p=122925#p122925
    write_log(Preferences()->debug_filesystem_monitor(), "[CFileSystemMonitor] Unsupported network mapped drive\n");
    _scraper_directory_on_a_network_drive = true;
    return;
  }
	int changes_to_monitor = FILE_NOTIFY_CHANGE_FILE_NAME
		| FILE_NOTIFY_CHANGE_SIZE
		| FILE_NOTIFY_CHANGE_LAST_WRITE;
  // Since OpenHoldem 13.0 we support a nested scraper-directory,
  // even with some "official" maps for real-world-casinos.
  const bool watch_subtree = TRUE;
	dwChangeHandle = FindFirstChangeNotification(
		absolute_path_to_scraper_directory,
    watch_subtree,	
		changes_to_monitor);
	if ((dwChangeHandle == INVALID_HANDLE_VALUE) || (dwChangeHandle == NULL)) {
		write_log(Preferences()->debug_filesystem_monitor(), "[CFileSystemMonitor] InitMonitor() failed.\n");
		write_log(Preferences()->debug_filesystem_monitor(), "[CFileSystemMonitor] Going to terminate...\n");
		ExitProcess(GetLastError()); 
	}
}

bool CFileSystemMonitor::AnyChanges() {
  if (_scraper_directory_on_a_network_drive) {
    // Network mapped drives are unsupported
    write_log(Preferences()->debug_filesystem_monitor(), "[CFileSystemMonitor] Unsupported network mapped drive\n");
    write_log(Preferences()->debug_filesystem_monitor(), "[CFileSystemMonitor] Treating as \"no changes\"\n");
    return false;
  }
	// http://msdn.microsoft.com/en-us/library/windows/desktop/ms687025(v=vs.85).aspx
	dwWaitStatus = WaitForMultipleObjects(
		1,                // number of handles
		&dwChangeHandle,  // handle
		FALSE,            // bWaitAll - does not make a difference here
		0);               // time to wait
	if (dwWaitStatus == WAIT_OBJECT_0) {
		write_log(Preferences()->debug_filesystem_monitor(), "[CFileSystemMonitor] Scraper directoy changed.\n");
		// Resetting change handle for next query
		// http://msdn.microsoft.com/en-us/library/windows/desktop/aa365261%28v=vs.85%29.aspx
		FindNextChangeNotification(dwChangeHandle);
		return true;
	}
	write_log(Preferences()->debug_filesystem_monitor(), "[CFileSystemMonitor] No changes in scraper directoy.\n");
	return false;
}