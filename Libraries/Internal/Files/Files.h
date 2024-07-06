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

#pragma once
#ifndef WINVER				// Allow use of features specific to Windows XP or later.
#define WINVER 0x0501
#endif
#include <afxwin.h>

// OpenPPL-files in the botlogic-directory
const int kNumberOfOpenPPLLibraries = 25;
const CString kOpenPPLLibraries[kNumberOfOpenPPLLibraries] = {
  "OpenPPL_Library_Ante_Others_IsFinalTable.ohf",
  "OpenPPL_Library_Bets_Pots_And_Stacks.ohf",
  "OpenPPL_Library_Betting_Action_Symbols.ohf",
  "OpenPPL_Library_Board_Symbols.ohf",
  "OpenPPL_Library_ChipAmounts.ohf",
  "OpenPPL_Library_Constants.ohf",
  "OpenPPL_Library_Deprecated.ohf",
  "OpenPPL_Library_HandStrength_Symbols.ohf",
  "OpenPPL_Library_History_Symbols.ohf",
  "OpenPPL_Library_HoldEm_HandStrength_Symbols.ohf",
  "OpenPPL_Library_HoldEm_NutFullHouseOrFourOfAKind.ohf",
  "OpenPPL_Library_Omaha_Board_Symbols.ohf",
  "OpenPPL_Library_Omaha_HandStrength_Symbols.ohf",
  "OpenPPL_Library_Omaha_NutFullHouseOrFourOfAKind.ohf",
  "OpenPPL_Library_OutsCounter.ohf",
  "OpenPPL_Library_Players_and_Opponents.ohf",
  "OpenPPL_Library_Position_Symbols.ohf",
  "OpenPPL_Library_Technical_Betting_Action_Symbols.ohf",
  "OpenPPL_Library_Technical_Card_Symbols.ohf",
  "OpenPPL_Library_Technical_HandStrength_Symbols.ohf",
  "OpenPPL_Library_Technical_Position_Symbols.ohf",
  "OpenPPL_Library_Technical_Stacksizes.ohf",
  "OpenPPL_Library_Tournament_Symbols.ohf",
  "OpenPPL_Library_Update_History.ohf",
  "OpenPPL_Library_Verbosity.ohf",
};

// Directories
CString BotlogicDirectory();
CString DefaultLogicDirectory();
CString LogsDirectory();
CString OpenHoldemDirectory();
CString OpenPPLLibraryDirectory();
CString PathOfExecutable();
CString ReplaySessionDirectory(int session_ID);
CString ScraperDirectory();
CString ToolsDirectory();
// Filenames
CString ExecutableFilename();
CString IniFilename();
CString PureExecutableFilename();
CString ReplayBitmapFilename(int session_ID, int frame_number);
CString ReplayHTMLFilename(int session_ID, int frame_number);
// Paths
CString CustomLibraryPath();
CString IniFilePath();
CString LogFilePath(int session_ID);
CString ManualModePath();
CString OpenHoldemManualpath();
CString OpenPPLManualpath();
CString VersusPath();
// Others
void OpenFileInExternalSoftware(CString file_name);
CString TableMapWildcard();
void SwitchToOpenHoldemDirectory();
//!!bool FileExists(CString filename_or_pattern);
CString FilenameWithoutPath(CString path);
CString FilenameWithoutPathAndExtension(CString path);
