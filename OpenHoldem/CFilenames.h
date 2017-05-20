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

#ifndef INC_CFILENAMES_H
#define INC_CFILENAMES_H

// OpenPPL-files in the botlogic-directory
const int kNumberOfOpenPPLLibraries = 24;
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
  "OpenPPL_Library_Players_and_Opponents.ohf",
  "OpenPPL_Library_Position_Symbols.ohf",
  "OpenPPL_Library_Stacksizes.ohf", //!!!!!
  "OpenPPL_Library_Technical_Betting_Action_Symbols.ohf",
  "OpenPPL_Library_Technical_Card_Symbols.ohf",
  "OpenPPL_Library_Technical_HandStrength_Symbols.ohf",
  "OpenPPL_Library_Technical_Position_Symbols.ohf",
  "OpenPPL_Library_Technical_Stacksizes.ohf",
  "OpenPPL_Library_Update_History.ohf",
  "OpenPPL_Library_Verbosity.ohf",
};

class CFilenames {
 public:
	CFilenames();
	~CFilenames();
 public:
  // Directories
  CString BotlogicDirectory();
  CString LogsDirectory();
  CString OpenHoldemDirectory();
  CString PathOfExecutable();
  CString ReplaySessionDirectory();
  CString ScraperDirectory();
  CString ToolsDirectory();
public:
  // Filenames
	CString MiniDumpFilename();
	CString ReplayBitmapFilename(int frame_number);
	CString ReplayHTMLFilename(int frame_number);
	CString ExecutableFilename();
	CString PureExecutableFilename();
public:
  // Paths
  CString IniFilePath();
  CString LogFilePath();
  CString VersusPath();
  CString CustomLibraryPath();
  CString ManualModePath();
public:
  // Others
  CString TableMapWildcard();
 public:
	void SwitchToOpenHoldemDirectory();
  bool Exists(CString filename_or_pattern);
 private:
	CString IniFilename();
  void Log(CString name, CString value);
 private:
	char _startup_path[MAX_PATH];
};

extern CFilenames *p_filenames;

#endif INC_CFILENAMES_H