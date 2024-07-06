//*******************************************************************************
//
// This file is part of the OpenHoldem project
//    Source code:           https://github.com/OpenHoldem/openholdembot/
//    Forums:                http://www.maxinmontreal.com/forums/index.php
//    Licensed under GPL v3: http://www.gnu.org/licenses/gpl.html
//
//*******************************************************************************
//
// Purpose:
//
//*******************************************************************************

#include "window_functions.h"
#include "Debugger/debugger.h"

const int k_messagebox_standard_flags = MB_OK | MB_TOPMOST;
const int k_messagebox_error_flags = k_messagebox_standard_flags | MB_ICONWARNING;

void MessageBox_Error_Warning(const char*  Message, const char*  Title /* = "Error" */) {
  // Only OpenHoldem supports this setting,
  // but not OpenScrape or other potential applications
  /*!!!!!if (preferences.disable_msgbox()) {
    return;
  }*/
  MessageBox(0, Message, Title, k_messagebox_error_flags);
}

int MessageBox_Interactive(const char* Message, const char* Title, int Flags) {
  return MessageBox(0, Message, Title, (Flags | k_messagebox_standard_flags));
}

// MessageBox for the msgbox$MESSAGE-command of OH-script
// Returns 0 as a result
void MessageBox_OH_Script_Messages(const char* message) {
  // Preprocess message
  CString CS_message(message);
  assert(CS_message.Left(7) == "msgbox$");
  CS_message.Replace("msgbox$", "");
  CS_message.Replace("_B", " ");
  CS_message.Replace("_C", ",");
  CS_message.Replace("_D", ".");
  CS_message.Replace("_N", "\n");
  // At the very last: underscores (to avoid incorrect future replacements)
  CS_message.Replace("_U", "_");
  MessageBox_Error_Warning(CS_message, "OH-Script Message");
}