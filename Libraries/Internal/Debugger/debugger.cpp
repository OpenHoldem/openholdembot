//******************************************************************************
//
// This file is part of the OpenHoldem project
//    Source code:           https://github.com/OpenHoldem/openholdembot/
//    Forums:                http://www.maxinmontreal.com/forums/index.php
//    Licensed under GPL v3: http://www.gnu.org/licenses/gpl.html
//
//******************************************************************************
//
// Purpose: Debug-DLL for OpenHoldem
//
//******************************************************************************

#ifdef _DEBUG
// visual leak detector in debug-mode
// https://vld.codeplex.com/
// needs to be included at the very beginning
// http://www.maxinmontreal.com/forums/viewtopic.php?f=338&t=22351
#include <vld.h>
// Check a new version at https://github.com/Azure/vld/releases/
//  or use OpenHoldemBot\Other\Distributives\VisualLeakDetector
// Install it
// Check Microsoft.Cpp.Win32.user in View->Other Windows->Property Manager  for  Debugger -> Debug | Win32
//  according https://github.com/KindDragon/vld/wiki/Using-Visual-Leak-Detector
//1. Select VC++ Directories
//2. Select "Include directories"
//3. Add "C:\Program Files (x86)\Visual Leak Detector\include" to the bottom of the list
//4. Select "Library directories"
//5. Add "C:\Program Files (x86)\Visual Leak Detector\lib\Win32" to the bottom of the list
#endif _DEBUG

#include "debugger.h"
#include <afxwin.h>
#include <stdarg.h>
#include <stdio.h>
#include <time.h>
#include "CCritSec/CCritSec.h"
#include "MagicNumbers/MagicNumbers.h"

FILE *log_fp = NULL;
int session_ID = kUndefined;
CString currentLogFilePath = "";
CCritSec log_critsec;  // Used to ensure only one thread at a time writes to log file
bool footer_needs_to_be_written = false;
const char *footer = "********************************************************************************\n";

void write_footer_if_necessary() {
  if (footer_needs_to_be_written == false) {
    return;
  }
  // First set footer_needs_to_be_written = false
  // otherwise we get an endless recursion
  footer_needs_to_be_written = false;
  write_log_nostamp(true, footer);
}

char *get_time(char * timebuf) {
  // returns current system time in WH format
  time_t	ltime;
  char tmptime[30];
  time(&ltime);
  ctime_s(tmptime, 26, &ltime);
  tmptime[24] = '\0';
  memcpy(timebuf, tmptime + 20, 4); //yyyy
  *(timebuf + 4) = '-';
  // mm
  if (memcmp(tmptime + 4, "Jan", 3) == 0) {
    *(timebuf + 5) = '0';
    *(timebuf + 6) = '1';
  }
  else if (memcmp(tmptime + 4, "Feb", 3) == 0) {
    *(timebuf + 5) = '0';
    *(timebuf + 6) = '2';
  }
  else if (memcmp(tmptime + 4, "Mar", 3) == 0) {
    *(timebuf + 5) = '0';
    *(timebuf + 6) = '3';
  }
  else if (memcmp(tmptime + 4, "Apr", 3) == 0) {
    *(timebuf + 5) = '0';
    *(timebuf + 6) = '4';
  }
  else if (memcmp(tmptime + 4, "May", 3) == 0) {
    *(timebuf + 5) = '0';
    *(timebuf + 6) = '5';
  }
  else if (memcmp(tmptime + 4, "Jun", 3) == 0) {
    *(timebuf + 5) = '0';
    *(timebuf + 6) = '6';
  }
  else if (memcmp(tmptime + 4, "Jul", 3) == 0) {
    *(timebuf + 5) = '0';
    *(timebuf + 6) = '7';
  }
  else if (memcmp(tmptime + 4, "Aug", 3) == 0) {
    *(timebuf + 5) = '0';
    *(timebuf + 6) = '8';
  }
  else if (memcmp(tmptime + 4, "Sep", 3) == 0) {
    *(timebuf + 5) = '0';
    *(timebuf + 6) = '9';
  }
  else if (memcmp(tmptime + 4, "Oct", 3) == 0) {
    *(timebuf + 5) = '1';
    *(timebuf + 6) = '0';
  }
  else if (memcmp(tmptime + 4, "Nov", 3) == 0) {
    *(timebuf + 5) = '1';
    *(timebuf + 6) = '1';
  }
  else if (memcmp(tmptime + 4, "Dec", 3) == 0) {
    *(timebuf + 5) = '1';
    *(timebuf + 6) = '2';
  }
  *(timebuf + 7) = '-';
  memcpy(timebuf + 8, tmptime + 8, 2); //dd
  *(timebuf + 10) = ' ';
  memcpy(timebuf + 11, tmptime + 11, 8); //HH:mm:ss
  *(timebuf + 19) = '\0';
  return timebuf;
}

char *get_now_time(char * timebuf) {
  // returns current system time as a UNIX style string
  time_t	ltime;
  time(&ltime);
  ctime_s(timebuf, 26, &ltime);
  timebuf[24] = '\0';
  return timebuf;
}

void delete_log() {
  assert(session_ID >= 0);
  // Precondition: log file must not be open
  remove(currentLogFilePath.GetString());
}

void clear_log() {
  stop_log();
  assert(session_ID > 0);
  start_log(session_ID, true, currentLogFilePath);
}

void start_log(int current_session_iD, bool delete_old_log, const char* logFilePath) {
  if (log_fp != NULL) {
    return;
  }
  assert(current_session_iD >= 0);
  session_ID = current_session_iD;
  CSLock lock(log_critsec);
  if (delete_old_log) {
    delete_log();
  }
  currentLogFilePath = CString(logFilePath);
  // Append (or create) log
  if ((log_fp = _fsopen(currentLogFilePath.GetString(), "a", _SH_DENYWR)) != 0) {
    write_log_separator(k_always_log_basic_information, "LOG FILE OPEN");
    // License information
    write_log(k_always_log_basic_information, "OpenHoldem\n");
    //!!!write_log(k_always_log_basic_information, "Version %s\n", VERSION_TEXT);
    write_log(k_always_log_basic_information, "Licensed under GPLv3\n");
    write_log(k_always_log_basic_information, "http://www.maxinmontreal.com/forums/index.php\n");
    write_log(k_always_log_basic_information, "https://github.com/OpenHoldem/openholdembot/\n");
    write_log(k_always_log_basic_information, "http://www.gnu.org/licenses/gpl.html\n");
    write_log(k_always_log_basic_information, "Disclaimer: this is an open-source project.\n");
    write_log(k_always_log_basic_information, "We are not related to any shops, stores and/or scam-artists.\n");
    write_log_separator(k_always_log_basic_information, "");
    fflush(log_fp);
  }
}

void write_log_vl(bool debug_settings_for_this_message, const char* fmt, va_list vl) {
  char		buff[10000];
  char		nowtime[26];
  write_footer_if_necessary();
  if (debug_settings_for_this_message == false) {
    return;
  }
  if (log_fp != NULL) {
    CSLock lock(log_critsec);
    vsprintf_s(buff, 10000, fmt, vl);
    get_time(nowtime);
    fprintf(log_fp, "%s > %s", nowtime, buff);
    fflush(log_fp);
  }
}

void write_log_assert(bool is_expression, const char* fmt, ...)
{
    if (is_expression) return;  //it's mean 'no assertion here', expression is as expected    
    va_list args;
    va_start(args, fmt);
    write_log_vl(!is_expression, fmt, args); //Something is not as expected
    va_end(args);
}

void write_log(bool debug_settings_for_this_message, const char* fmt, ...) {
  char		buff[10000];
  va_list		ap;
  char		nowtime[26];
  if (debug_settings_for_this_message == false) {
    return;
  }
  if (log_fp == NULL) {
    return;
  }
  write_footer_if_necessary();
  CSLock lock(log_critsec);
  va_start(ap, fmt);
  vsprintf_s(buff, 10000, fmt, ap);
  get_time(nowtime);
  fprintf(log_fp, "%s - %s", nowtime, buff);
  va_end(ap);
  fflush(log_fp);
}

void write_log_nostamp(bool debug_settings_for_this_message, const char* fmt, ...) {
  char		buff[10000];
  va_list		ap;
  write_footer_if_necessary();
  if (debug_settings_for_this_message == false) {
    return;
  }
  if (log_fp == NULL) {
    return;
  }
  CSLock lock(log_critsec);
  va_start(ap, fmt);
  vsprintf_s(buff, 10000, fmt, ap);
  fprintf(log_fp, "%s", buff);
  va_end(ap);
  fflush(log_fp);
}

void stop_log(void) {
  write_footer_if_necessary();
  if (log_fp == NULL) return;
  write_log_separator(k_always_log_basic_information, "LOG FILE CLOSED");
  fclose(log_fp);
  log_fp = NULL;
}

void write_log_separator(bool debug_settings_for_this_message, const char* header_message) {
  if ((header_message == NULL) || (strcmp(header_message, "") == 0)) {
    // Empty header, i.e. footer
    // Don't write it immediatelly to avoid multiple consecutive headers
    footer_needs_to_be_written = true;
    return;
  }
  // Write separator with header, skip potential footers
  footer_needs_to_be_written = false;
  char header[90];
  assert(strlen(footer) < 90);
  assert(strlen(header_message) < 60);
  // Copy the footer and \0 into the header
  memcpy(header, footer, strlen(footer) + 1);
  // Now copz the header-message into the header (without \0)
  memcpy((header + 10), header_message, strlen(header_message));
  header[9] = ' ';
  header[10 + strlen(header_message)] = ' ';
  write_log_nostamp(true, header);
}