//******************************************************************************
//
// This file is part of the OpenHoldem project
//    Source code:           https://github.com/OpenHoldem/openholdembot/
//    Forums:                http://www.maxinmontreal.com/forums/index.php
//    Licensed under GPL v3: http://www.gnu.org/licenses/gpl.html
//
//******************************************************************************
//
// Purpose: File-handling for OpenHoldem
//
//******************************************************************************

#include "files.h"
#include "Debugger/debugger.h"

const char* k_default_ini_filename = "OpenHoldem.ini";

CString OpenHoldemDirectory() {
  // Get the  directory that contains openHoldem for future use.
  // This is NOT GetCurrentDirectory(), which returns the working-directorz,
  // i.e. the working directorz of the process that started OpenHoldem.
  //
  // If we don't get this path right and the user stars OpenHoldem
  // from a different directorz with a script we won't find our files.
  // http://stackoverflow.com/questions/143174/how-do-i-get-the-directory-that-a-program-is-running-from
  // http://stackoverflow.com/questions/4517425/how-to-get-program-path
  char path[MAX_PATH];
  ::GetModuleFileName(NULL, path, _MAX_PATH);
  // Remove executable filename from path
  // http://pic.dhe.ibm.com/infocenter/zvm/v6r2/index.jsp?topic=%2Fcom.ibm.zvm.v620.edclv%2Fstrrchr.htm
  char *last_backslash_in_path = strrchr(path, '\\');
  if (last_backslash_in_path != NULL) {
    ++last_backslash_in_path;
    *last_backslash_in_path = '\0';
  }
  CString result(path);
  return result;
}

CString BotlogicDirectory() {
  assert(OpenHoldemDirectory() != "");
  CString bot_logic_dir = CString(OpenHoldemDirectory()) + "Logic\\";
  return bot_logic_dir;
}

CString DefaultLogicDirectory() {
  return BotlogicDirectory() + "Default\\";
}

CString IniFilePath() {
  // We need the complete path here,
  // otherwise the file would be expected in the default location,
  // i.e. the Windows-directory.
  CString result = OpenHoldemDirectory() + CString("\\") + IniFilename();
  return result;
}

CString IniFilename() {
  WIN32_FIND_DATA find_file_data;
  HANDLE h_find;
  CString wildcard = OpenHoldemDirectory() + "\\*.ini";
  CString ini_filename;

  // Try to find "first" ini-file.
  // No matter how it is named -- it is the right one.
  // http://msdn.microsoft.com/en-us/library/windows/desktop/aa364418(v=vs.85).aspx
  h_find = FindFirstFile(wildcard, &find_file_data);
  if (h_find == INVALID_HANDLE_VALUE) {
    // No ini-file found.
    // Use default one (for saving).
    FindClose(h_find);

    return k_default_ini_filename;
  }
  ini_filename = find_file_data.cFileName;
  // Check that no more ini-files exist (potential problems).
  // http://msdn.microsoft.com/en-us/library/windows/desktop/aa364428(v=vs.85).aspx
  if (FindNextFile(h_find, &find_file_data)) {
    FindClose(h_find);
    /*!!!MessageBox_Error_Warning(
      "More than one ini-file in OpenHoldem-directory.\n"
      "Don't know which one to use.\n"
      "\n"
      "Going to terminate...");*/
    PostQuitMessage(-1);
    // Previously we returned "a_result_to_make_the_compiler_happy.ini"
    // believing the result was meaning-less and would never be used.
    // However PostQuitMessage(-1) uses a message-queue,
    // so it may take some time, and OH will continue a little bit.
    // Therefore we take the last filename now in order to 
    // not create yet another one with a funny name. ;-)
    // http://www.maxinmontreal.com/forums/viewtopic.php?f=156&t=16229
    return ini_filename;
  }
  FindClose(h_find);
  // Exactly one ini-file found
  return ini_filename;
}

void SwitchToOpenHoldemDirectory() {
  assert(OpenHoldemDirectory() != "");
  SetCurrentDirectory(OpenHoldemDirectory());
}

CString OpenPPLLibraryDirectory() {
  return BotlogicDirectory() + "OpenPPL\\";
}

void OpenFileInExternalSoftware(CString file_name) {
  long long int RetValue = long long int(ShellExecute(
    NULL, 
    "open", 
    file_name, 
    NULL, 
    "", 
    SW_SHOWNORMAL));
  if (RetValue <= 32) {
    CString error_message;
    error_message.Format("Could not open file \n%s\n",
      file_name);
    MessageBox(0, error_message, "Error", 0);
  }
}

CString ManualDirectory() {
  assert(OpenHoldemDirectory() != "");
  CString manual_directory = OpenHoldemDirectory() + "Documents\\";
  return manual_directory;
}

CString OpenHoldemManualpath() {
  CString manual_path = ManualDirectory() + "OpenHoldem.wiki.bat";
  return manual_path;
}

CString OpenPPLManualpath() {
  CString manual_path = ManualDirectory() + "OpenPPL.wiki.bat";
  return manual_path;
}

CString TableMapWildcard() {
  assert(OpenHoldemDirectory() != "");
  CString wildcard;
  wildcard.Format("%sTables\\*.tm", OpenHoldemDirectory());
  return wildcard;
}

CString ScraperDirectory() {
  assert(OpenHoldemDirectory() != "");
  CString scraper_dir = CString(OpenHoldemDirectory()) + "Tables\\";
  return scraper_dir;
}

CString ToolsDirectory() {
  assert(OpenHoldemDirectory() != "");
  CString tools_dir = CString(OpenHoldemDirectory()) + "Tools\\";
  return tools_dir;
}

CString ReplaySessionDirectory(int session_ID) {
  assert(session_ID >= 0);
  assert(OpenHoldemDirectory() != "");
  CString path;
  path.Format("%sReplay\\session_%lu\\", OpenHoldemDirectory(), session_ID);
  return path;
}

CString ReplayBitmapFilename(int session_ID, int frame_number) {
  assert(frame_number >= 0);
  CString path;
  path.Format("%sframe%06d.bmp", ReplaySessionDirectory(session_ID), frame_number);
  return path;
}

CString ReplayHTMLFilename(int session_ID, int frame_number) {
  assert(frame_number >= 0);
  CString path;
  path.Format("%sframe%06d.htm", ReplaySessionDirectory(session_ID), frame_number);
  return path;
}

CString LogsDirectory() {
  assert(OpenHoldemDirectory() != "");
  CString path;
  path.Format("%sLogs\\", OpenHoldemDirectory());
  // Create logs-directoy if necessary
  CreateDirectory(path, NULL);
  return path;
};

CString LogFilePath(int session_ID) {
  assert(session_ID >= 0);
  CString path;
  path.Format("%soh_%lu.log", LogsDirectory(), session_ID);
  return path;
}

CString PathOfExecutable() {
  // http://msdn.microsoft.com/en-us/library/windows/desktop/ms683197(v=vs.85).aspx
  // http://stackoverflow.com/questions/124886/how-to-get-the-application-executable-name-in-windows-c-win32-or-c-cli
  TCHAR exe_path[MAX_PATH + 1];
  GetModuleFileName(0, exe_path, MAX_PATH + 1);
  return exe_path;
}

CString ExecutableFilename() {
  return FilenameWithoutPath(PathOfExecutable());
}

CString PureExecutableFilename() {
  return FilenameWithoutPathAndExtension(ExecutableFilename());
}

CString VersusPath() {
  CString result = OpenHoldemDirectory() + "versus.bin";
  return result;
}

CString CustomLibraryPath() {
  CString result = BotlogicDirectory() + "\\custom.ohf";
  return result;
}

CString ManualModePath() {
  CString result = ToolsDirectory() + "\\ManualMode.exe";
  return result;
}

bool Exists(CString filename_or_pattern) {
  // https://msdn.microsoft.com/en-us/library/windows/desktop/aa364418%28v=vs.85%29.aspx
  WIN32_FIND_DATA FindFileData;
  HANDLE hFind;
  hFind = FindFirstFile(filename_or_pattern, &FindFileData);
  if (hFind == INVALID_HANDLE_VALUE) return false;
  FindClose(hFind);
  return true;
}

CString FilenameWithoutPath(CString path) {
  int pos = path.ReverseFind('\\');
  if (pos > 0) {
    assert(pos < path.GetLength() - 1);
    return path.Mid(pos + 1);
  }
  return path;
}

CString FilenameWithoutPathAndExtension(CString path) {
  CString filename = FilenameWithoutPath(path);
  int pos = filename.ReverseFind('.');
  CString result;
  if (pos > 0) {
    result = filename.Left(pos);
  }
  else {
    result = filename;
  }
  return result;
}
