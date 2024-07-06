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

#include "StdAfx.h"
#include "CTableMaploader.h"

#include "CAutoConnector.h"
#include "CFileSystemMonitor.h"

#include "CTablemapCompletenessChecker.h"
#include "CTablePointChecker.h"
#include "CTablemap/CTablemapAccess.h"
#include "Files/Files.h"
#include "WindowFunctions/window_functions.h"
#include "OpenHoldem.h"

CTableMapLoader *p_tablemap_loader = NULL;

std::map<int, t_tablemap_connection_data> tablemap_connection_data;

CTableMapLoader::CTableMapLoader() {
	write_log(Preferences()->debug_tablemap_loader(), "[CTablemapLoader] CTableMapLoader()\n");
	tablemaps_in_scraper_folder_already_parsed = false;
	// Parse all tablemaps once on startup.
	// We want to avoid heavy workload in the connect()-function.
	ParseAllTableMapsToLoadConnectionData();
	CheckForDuplicatedTablemaps(); 
}

CTableMapLoader::~CTableMapLoader() {
  tablemap_connection_data.clear();
}

void CTableMapLoader::ParseAllTableMapsToLoadConnectionData(CString scraper_directory) {
	write_log(Preferences()->debug_tablemap_loader(), "[CTablemapLoader] ParseAllTableMapsToLoadConnectionData: %s\n", scraper_directory);
  CString wildcard;
  wildcard.Format("%s\\*", scraper_directory);
  CFileFind	hFile;
	BOOL bFound = hFile.FindFile(wildcard);
	while (bFound) {
		bFound = hFile.FindNextFile();
    // Formerly there has been a check /hFile.GetFilePath() != p_tablemap->filepath()) 
    // but if we want to reload, then everything
    write_log(Preferences()->debug_tablemap_loader(), "[CTablemapLoader] Looking at %s\n",
      hFile.GetFilePath());
    if (hFile.IsDots()) {
      write_log(Preferences()->debug_tablemap_loader(), "[CTablemapLoader] Skipping dots\n");
      // Ignore link to current directory and to parent-directory
      continue;
    } else if (hFile.IsDirectory()) {
      // Traverse sub-directories recursively
      write_log(Preferences()->debug_tablemap_loader(), "[CTablemapLoader] Diving into directory\n");
      ParseAllTableMapsToLoadConnectionData(hFile.GetFilePath());
      continue;
    } else if (hFile.GetFilePath().Right(3).MakeLower() != ".tm") {
      // Not a tablemap
      write_log(Preferences()->debug_tablemap_loader(), "[CTablemapLoader] Not a tablemap\n");
      continue;
    }
    write_log(Preferences()->debug_tablemap_loader(), "[CTablemapLoader] Loading file\n");
		int ret = p_tablemap->LoadTablemap(hFile.GetFilePath().GetString());
		if (ret == SUCCESS)	{
			ExtractConnectionDataFromCurrentTablemap(p_tablemap);
      CTablemapCompletenessChecker tablemap_completeness_checker;
      tablemap_completeness_checker.VerifyMap();
			write_log(Preferences()->debug_tablemap_loader(), "[CTablemapLoader] Number of TMs loaded: %d\n", _number_of_tablemaps_loaded);
		}
	}
}

void CTableMapLoader::ParseAllTableMapsToLoadConnectionData() {
	write_log(Preferences()->debug_tablemap_loader(), "[CTablemapLoader] ParseAllTableMapsToLoadConnectionData\n");
  tablemap_connection_data.clear();
  _number_of_tablemaps_loaded = 0;
	ParseAllTableMapsToLoadConnectionData(ScraperDirectory());
	tablemaps_in_scraper_folder_already_parsed = true;
}

bool CTableMapLoader::tablemap_connection_dataAlreadyStored(CString TablemapFilePath) {
  assert(TablemapFilePath != "");
	for (int i=0; i<_number_of_tablemaps_loaded; i++)	{
    assert(tablemap_connection_data[i].FilePath != "");
		if (tablemap_connection_data[i].FilePath == TablemapFilePath)	{
			write_log(Preferences()->debug_tablemap_loader(), "[CTablemapLoader] tablemap_connection_dataAlreadyStored [%s] [true]\n", TablemapFilePath);
			return true;
		}
	}
	write_log(Preferences()->debug_tablemap_loader(), "[CTablemapLoader] tablemap_connection_dataAlreadyStored [%s] [false]\n", TablemapFilePath);
	return false;
}

void CTableMapLoader::CheckForDuplicatedTablemaps() {
	CString error_message = "";
	for (int i=0; i<_number_of_tablemaps_loaded; i++)	{
		for (int j=i+1; j<_number_of_tablemaps_loaded; j++)	{
			if	((tablemap_connection_data[i].SiteName == tablemap_connection_data[j].SiteName)
				&& (tablemap_connection_data[i].TitleText == tablemap_connection_data[j].TitleText))
			{
				write_log(Preferences()->debug_tablemap_loader(), "[CTablemapLoader] tablemap_connection_dataDuplicated [%s, %s]  [true]\n", 
					tablemap_connection_data[i].SiteName, tablemap_connection_data[i].TitleText);
				error_message.Format("It seems you have multiple versions of the same map in your scraper folder.\n\n"\
					"SiteName = %s\n"\
					"TitleText = %s\n\n"\
					"This will cause problems as the autoconnector won't be able to decide which one to use.\n"\
					"Please remove the superfluous maps from the scraper folder.\n", 
					tablemap_connection_data[i].SiteName, tablemap_connection_data[i].TitleText);
				MessageBox_Error_Warning((LPCTSTR) error_message, 
					"Warning! Duplicate SiteName and TitleText");
			}
		}
	}
}

void CTableMapLoader::ExtractConnectionDataFromCurrentTablemap(CTablemap *cmap) {
	write_log(Preferences()->debug_tablemap_loader(), "[CTablemapLoader] ExtractConnectionDataFromCurrentTablemap(): %s\n", cmap->filepath());
	write_log(Preferences()->debug_tablemap_loader(), "[CTablemapLoader] number_of_tablemaps_loaded: %d\n", _number_of_tablemaps_loaded);
	// Avoiding to store the data twice, e.g. when we load a known TM manually
	if (tablemap_connection_dataAlreadyStored(cmap->filepath())) 	{
		write_log(Preferences()->debug_tablemap_loader(), "[CTablemapLoader] ExtractConnectionDataFromCurrentTablemap(): already stored; early exit\n");
		return;
	}
	tablemap_connection_data[_number_of_tablemaps_loaded].FilePath = cmap->filepath();
	tablemap_connection_data[_number_of_tablemaps_loaded].SiteName = cmap->sitename();
	tablemap_connection_data[_number_of_tablemaps_loaded].TitleText = cmap->titletext();
	// Get clientsize info through TM-access-class
	p_tablemap_access->GetClientSize("clientsizemin", &tablemap_connection_data[_number_of_tablemaps_loaded].ClientSizeMinX, &tablemap_connection_data[_number_of_tablemaps_loaded].ClientSizeMinY);
	p_tablemap_access->GetClientSize("clientsizemax", &tablemap_connection_data[_number_of_tablemaps_loaded].ClientSizeMaxX, &tablemap_connection_data[_number_of_tablemaps_loaded].ClientSizeMaxY);
	// Extract title text information
	p_tablemap_access->SetTitleText("titletext", tablemap_connection_data[_number_of_tablemaps_loaded].TitleText);
	// Extract negative title texs
	p_tablemap_access->SetTitleText("!titletext", tablemap_connection_data[_number_of_tablemaps_loaded].NegativeTitleText);
	CString s = "";
	for (int i=0; i<k_max_number_of_titletexts; i++) {
		s.Format("titletext%d", i);
		p_tablemap_access->SetTitleText(s, tablemap_connection_data[_number_of_tablemaps_loaded].TitleText_0_9[i]);
		s.Format("!titletext%d", i);
		p_tablemap_access->SetTitleText(s, tablemap_connection_data[_number_of_tablemaps_loaded].NegativeTitleText_0_9[i]);		
	}
	// Extract tablepoints r$tablepoint0..9
	s = "";
	int tpcount = 0;
	for (int i=0; i<k_max_number_of_tablepoints; ++i) {
		s.Format("tablepoint%d", i);
		if (p_tablemap_access->GetTableMapRegion(s, &tablemap_connection_data[_number_of_tablemaps_loaded].TablePoint[i])) {
		  ++tpcount;
		} else {
      break;
    }
	}
	tablemap_connection_data[_number_of_tablemaps_loaded].TablePointCount = tpcount;
	++_number_of_tablemaps_loaded;
}

void CTableMapLoader::ReloadAllTablemapsIfChanged() {
  if (p_filesystem_monitor->AnyChanges()) {
    // Since OpenHoldem 4.0.0 there is no longer any possibility 
    // for manual connection; i.e. no possibility for re-connection.
    // Therefore the file-system-monitor does now dis-connect
    // if it finds a new/changed tablemap and the auto-connector-thread 
    // will thereafter reload the TMs and connect again.
    //
    // Note: This solution might lose some game-history (reset),
    // but that is perfectly acceptable for development
    // and hot-plugging of TMs won't happen in production.
    write_log(Preferences()->debug_tablemap_loader(), "[CTablemapLoader] Tablemaps changed; going to reload.\n");
    p_autoconnector->Disconnect("tablemaps changed and need to be reloaded");
    ParseAllTableMapsToLoadConnectionData();
  }
  else {
    write_log(Preferences()->debug_tablemap_loader(), "[CTablemapLoader] All tablemaps unchanged; nothing to do.\n");
  }
}

CString CTableMapLoader::GetTablemapPathToLoad(int tablemap_index) {
  assert(tablemap_index >= 0);
  assert(tablemap_index < _number_of_tablemaps_loaded);
  return tablemap_connection_data[tablemap_index].FilePath;
}

// Forward declarations for the next function
// !! To be moved into an extra module one day
bool TablemapMatchesWindowBySize(int MapIndex, HWND window);
bool TablemapMatchesWindowByTitle(int MapIndex, HWND window);
bool TablemapMatchesWindowByNegativeTitles(int MapIndex, HWND window);
bool TablemapMatchesWindowByTablepoints(int MapIndex, HWND window);

// This function has to be global and can't be part of the class,
// as it has to be called by the callback-function 
// BOOL CALLBACK EnumProcTopLevelWindowList(HWND windowwnd, LPARAM lparam) 
bool Check_TM_Against_Single_Window(int MapIndex, HWND window) { 
	write_log(Preferences()->debug_tablemap_loader(), "[CTablemapLoader] Check_TM_Against_Single_Window(..)\n");
  write_log(Preferences()->debug_tablemap_loader(), "[CTablemapLoader] Checking map [%d] and window [%d]\n", 
    MapIndex, window);
  if (!TablemapMatchesWindowBySize(MapIndex, window)) {
    write_log(Preferences()->debug_tablemap_loader(), "[CTablemapLoader] Size does not match\n");
    return false;
  }
  if (!TablemapMatchesWindowByTitle(MapIndex, window)) {
    write_log(Preferences()->debug_tablemap_loader(), "[CTablemapLoader] Title does not match\n");
    return false;
  }
  if (!TablemapMatchesWindowByNegativeTitles(MapIndex, window)) {
    write_log(Preferences()->debug_tablemap_loader(), "[CTablemapLoader] Megative title does not match\n");
    return false;
  }
  if (!TablemapMatchesWindowByTablepoints(MapIndex, window)) {
    write_log(Preferences()->debug_tablemap_loader(), "[CTablemapLoader] Tablepoint does not match\n");
    return false;
  }
  write_log(Preferences()->debug_tablemap_loader(), "[CTablemapLoader] Tablemap [%d] matches window [%d]\n", 
    MapIndex, window);
  return true;
}

bool TablemapMatchesWindowBySize(int MapIndex, HWND window) {
  RECT crect;
  GetClientRect(window, &crect);
  if (crect.right < tablemap_connection_data[MapIndex].ClientSizeMinX) return false;
  if (crect.right > tablemap_connection_data[MapIndex].ClientSizeMaxX) return false;
  if (crect.bottom < tablemap_connection_data[MapIndex].ClientSizeMinY) return false;
  if (crect.bottom > tablemap_connection_data[MapIndex].ClientSizeMaxY) return false;
  write_log(Preferences()->debug_tablemap_loader(), "[CTablemapLoader] Good size: (%dpx, %dpx) in clientsizemin/max\n",
    crect.right, crect.bottom,
    tablemap_connection_data[MapIndex].ClientSizeMinX, tablemap_connection_data[MapIndex].ClientSizeMinY);
  return true;
}

bool TablemapMatchesWindowByTitle(int MapIndex, HWND window) {
  // Any good match counts as matching
  char text[MAX_WINDOW_TITLE] = { 0 };
  GetWindowText(window, text, sizeof(text));
  CString title = text;
  if ((tablemap_connection_data[MapIndex].TitleText != "")
    && title.Find(tablemap_connection_data[MapIndex].TitleText) != -1) {
    // Matching non-empty title-text
    write_log(Preferences()->debug_tablemap_loader(), "[CTablemapLoader] Title [%s] matches.\n", title);
    return true;
  }
  if ((tablemap_connection_data[MapIndex].TitleText == "")
    && (title == "")) {
    // Matching empty title-text (since version 11.0.1)
    // Exact match, as partial matching makes no sense
    // This also requires tablepoints to make sure that we don't randomly
    // connect to crappy ghost-windows. The existence of tablepoints
    // gets verified by the TablemapcompletenessChecker.
    // http://www.maxinmontreal.com/forums/viewtopic.php?f=124&t=20382
    write_log(Preferences()->debug_tablemap_loader(), "[CTablemapLoader] Empty title [%s] matches.\n", title);
    return true;
  }
  // titletext din't match
  // Check for titletext0..titletext9
  for (int i = 0; i<k_max_number_of_titletexts; i++) {
    if ((tablemap_connection_data[MapIndex].TitleText_0_9[i] != "")
      && (title.Find(tablemap_connection_data[MapIndex].TitleText_0_9[i]) != -1)) {
      write_log(Preferences()->debug_tablemap_loader(), "[CTablemapLoader] Title [%s] matches.\n", title);
      return true;
    }
  }
  write_log(Preferences()->debug_tablemap_loader(), "[CTablemapLoader] Title [%s] does not match.\n", title);
  return false;
}

bool TablemapMatchesWindowByNegativeTitles(int MapIndex, HWND window) {
  // Any bad match counts as not matching
  char text[MAX_WINDOW_TITLE] = { 0 };
  GetWindowText(window, text, sizeof(text));
  CString title = text;
  if ((tablemap_connection_data[MapIndex].NegativeTitleText != "")
    && title.Find(tablemap_connection_data[MapIndex].NegativeTitleText) != -1) {
    write_log(Preferences()->debug_tablemap_loader(), "[CTablemapLoader] Failure. Title [%s] matches negative text.\n", title);
    return false;
  }
  // titletext din't match
  // Check for titletext0..titletext9
  for (int i = 0; i<k_max_number_of_titletexts; i++) {
    if ((tablemap_connection_data[MapIndex].NegativeTitleText_0_9[i] != "")
      && (title.Find(tablemap_connection_data[MapIndex].NegativeTitleText_0_9[i]) != -1)) {
      write_log(Preferences()->debug_tablemap_loader(), "[CTablemapLoader] Failure. Title [%s] matches negative text.\n", title);
      return false;
    }
  }
  write_log(Preferences()->debug_tablemap_loader(), "[CTablemapLoader] Success. Title [%s] does not match negative titles.\n", title);
  return true;
}

bool TablemapMatchesWindowByTablepoints(int MapIndex, HWND window) {
  // All tablepoints (if present must match
  CTablepointChecker tablepoint_checker;
  if (!tablepoint_checker.CheckTablepoints(window, MapIndex)) {
    write_log(Preferences()->debug_tablemap_loader(), "[CTablemapLoader] Not all tablepoints match.\n");
    return false;
  }
  write_log(Preferences()->debug_tablemap_loader(), "[CTablemapLoader] Window ia a match\n");
  return true;
}