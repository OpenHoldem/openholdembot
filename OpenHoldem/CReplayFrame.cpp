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
#include "CReplayFrame.h"

#include "CEngineContainer.h"
#include "CHeartbeatThread.h"

#include "CScraper.h"
#include "CSessionCounter.h"
#include "CSymbolEngineActiveDealtPlaying.h"
#include "CSymbolEngineAutoplayer.h"
#include "CSymbolEngineDealerchair.h"
#include "CSymbolEngineUserchair.h"
#include "CSymbolEngineTableLimits.h"
#include "CTableState.h"
#include "..\CTablemap\CTablemap.h"
#include "..\DLLs\WindowFunctions_DLL\window_functions.h"
#include "OpenHoldem.h"
#include <io.h>

//  Sanity check: enough disk-space for a replay frame?
//	We assume, 10 MB are enough
//	  - a large table of 1000x1000 pixels a 4 byte
//	  - some KB for the HTML file.
//	  - some space for other processes	
const unsigned int FREE_SPACE_NEEDED_FOR_REPLAYFRAME = 10000000;  

// We always start with 0 amd overwrite existing frames.
// That's more simple and we usually are not interested
// in old frames of bygone sessions.
int CReplayFrame::_next_replay_frame = 0;

CReplayFrame::CReplayFrame(void) {
	// if replay frame current nb is 0 (it will most often happen at start)
	// then check first available file and use that one first
	if (0 == _next_replay_frame) {
		for (int i = 0 ; i < Preferences()->replay_max_frames() ; i++) {
			CString path = ReplayHTMLFilename(p_sessioncounter->session_id(), i);
			if (ENOENT == _access_s(path.GetString(), 0)) {
				_next_replay_frame = i;
				break;
			}
		}
	}
}

CReplayFrame::~CReplayFrame(void) {
  // As we create a new CReplayFrame for each frame
  // we simply have to increment the global counter once we are finished.
	++_next_replay_frame;
  if (_next_replay_frame >= Preferences()->replay_max_frames()) {
    // Once we reach the maximum overwrite old frames
    // in cyclic order, starting again from 0.
    _next_replay_frame = 0;
  }
}

void CReplayFrame::CreateReplayFrame(void){
	FILE	*fp = NULL;
	ULARGE_INTEGER	free_bytes_for_user_on_disk = {0}, 
	total_bytes_on_disk = {0}, 
	free_bytes_total_on_disk = {0};
	//  Sanity check: Enough disk-space for replay frame?	
	GetDiskFreeSpaceEx(
		OpenHoldemDirectory(),  //  Directory on disk of interest
		&free_bytes_for_user_on_disk,  
		&total_bytes_on_disk,	
		&free_bytes_total_on_disk);
	if (free_bytes_for_user_on_disk.QuadPart < FREE_SPACE_NEEDED_FOR_REPLAYFRAME) {
		write_log(Preferences()->debug_replayframes(), "[CReplayFrame] Not enough disk-space\n");
		MessageBox_Error_Warning("Not enough disk space to create replay-frame.");
		return;
	}
	// Get exclusive access to CScraper and CSymbols variables
	// (Wait for scrape/symbol cycle to finish before saving frame)
	EnterCriticalSection(&p_heartbeat_thread->cs_update_in_progress);
	CString next_frame;
	next_frame.Format("[%06d]", _next_replay_frame);
	// Replay-frame should always be mentioned in the log for easy reference
	write_log(k_always_log_basic_information, "[CReplayFrame] Shooting frame %s\n", next_frame);
	CreateBitMapFile();
	// Create HTML file
	assert(p_sessioncounter != nullptr);
	CString path = ReplayHTMLFilename(p_sessioncounter->session_id(), _next_replay_frame);
	if (fopen_s(&fp, path.GetString(), "w")==0) {
		write_log(Preferences()->debug_replayframes(), "[CReplayFrame] Creating HTML file: %s\n", path);
		// First line has to be the "title" of the table.
		// This is no longer valid HTML, but the way Ray.E.Bornert did it
		// for WinHoldem and WinScrape.
		fprintf(fp, "%s\n", LPCSTR(p_table_state->TableTitle()->Title()));
		fprintf(fp, "<br>\n");
		// HTML header
		fprintf(fp, "<html>\n");
		fprintf(fp, "  <head>\n");
		fprintf(fp, "    <title>%s</title>\n", LPCSTR(p_table_state->TableTitle()->Title()));
		fprintf(fp, "  </head>");
		fprintf(fp, "<style>\n");
		fprintf(fp, "td {text-align:right;}\n");
		fprintf(fp, "</style>\n");
		fprintf(fp, "<body>\n");
		fprintf(fp, "<font face=courier>\n");
		// Bitmap image
		fprintf(fp, "<img src=\"frame%06d.bmp\">\n", _next_replay_frame);
		fprintf(fp, "<br>\n");
		// Title, data, frame-number, OH-version
		fprintf(fp, "%s", LPCSTR(GeneralInfo()));
		// Links forwards and backwards to the next frames
		fprintf(fp, "%s", LPCSTR(GetLinksToPrevAndNextFile()));
		fprintf(fp, "<br>\n");
		fprintf(fp, "<br>\n");
		// Header of main table for smaller data tables
		fprintf(fp, "<table>\n");
		fprintf(fp, "<tr>\n");
		fprintf(fp, "<td>\n");
		// Data tables
		fprintf(fp, "%s", LPCSTR(GetPlayerInfoAsHTML()));
		fprintf(fp, "/<td>\n");
		fprintf(fp, "<td>\n");
		fprintf(fp, "%s", LPCSTR(GetButtonStatesAsHTML()));
		fprintf(fp, "%s", LPCSTR(GetBlindInfoAsHTML()));
		fprintf(fp, "%s", LPCSTR(GetCommonCardsAsHTML()));
		fprintf(fp, "%s", LPCSTR(GetPotsAsHTML()));
		fprintf(fp, "</td>\n");
		// Footer of main table
		fprintf(fp, "</tr>\n");
		fprintf(fp, "</table>\n");
		// End of HTML
		fprintf(fp, "</body></html>\n");
		fclose(fp);
	}	
	LeaveCriticalSection(&p_heartbeat_thread->cs_update_in_progress);
}

CString CReplayFrame::GeneralInfo() {
	// Get current time
	time_t    ltime = 0;
	tm        now_time = {0};
	char			now_time_str[100] = {0};
	time(&ltime);
	localtime_s(&now_time, &ltime);
	strftime(now_time_str, 100, "%Y-%m-%d %H:%M:%S", &now_time);

	CString result;
	result += "<table border=4 cellpadding=1 cellspacing=1>\n";
	// Table title
	result += "<tr><td>\n";
	result += p_table_state->TableTitle()->Title();
	result += "</td></tr>\n";
	// Session, 
	result += "<tr><td>\n";
	result += "Session: ";
	result += Number2CString(p_sessioncounter->session_id(), 0);
	result += "</td></tr>\n";
	// Frame number 
	result += "<tr><td>\n";
	CString next_frame;
	next_frame.Format("Frame: %06d", _next_replay_frame);
	result += next_frame;
	result += "</td></tr>\n";
	// Time
	result += "<tr><td>\n";
	result += now_time_str;
	result += "</td></tr>\n";
	// Version
	result += "<tr><td>\n";
	result += "OpenHoldem ";
	result += VERSION_TEXT;
	result += "</td></tr>\n";
	// Finish table
	result += "</table>\n";
	return result;
}

CString CReplayFrame::GetPlayerInfoAsHTML() {
	CString player_info, text;
	// Table header for: SFABD (seated, active, button, dealt, playing),
	// hand, bet, balance, name
	player_info += "<table border=4 cellpadding=1 cellspacing=1>\n";
	player_info += "  <thead>\n";
	player_info += "    <tr>\n";
	player_info += "      <th>#</th>\n";
	player_info += "      <th>SABDP</th>\n";  // seated, active, button, dealt, playing
	player_info += "      <th>hand</th>\n";
	player_info += "      <th>bet</th>\n";
	player_info += "      <th>balance</th>\n";
	player_info += "      <th>name</th>\n";
	player_info += "    </tr>\n";
	player_info += "  </thead>\n";
	// Table footer for explanations
	player_info += "  <tfoot>\n";
	player_info += "    <tr\n";
	player_info += "      <td colspan=5>SABDP = seated / active / button / dealt / playing</td>\n";
	player_info += "    </tr>\n";
	player_info += "  </tfoot>\n";
	// Table body
	player_info += "  <tbody>\n";
	// One table row per player...
	for (int i=0; i<p_tablemap->nchairs(); i++) {
		player_info += "    <tr>\n";
		// Chair number
		text.Format("      <td>%d</td>\n", i);   
		player_info += text;
		// seated, active, button, dealt, playing
		text = "      <td>";
		text +=	(p_engine_container->symbol_engine_active_dealt_playing()->playersseatedbits() & (1<<i)) ? "s" : "-";
		text +=	(p_engine_container->symbol_engine_active_dealt_playing()->playersactivebits() & (1<<i)) ? "a" : "-";
		text +=	(DEALER_CHAIR == i) ? "b" : "-";
		text +=	(p_engine_container->symbol_engine_active_dealt_playing()->playersdealtbits() & (1<<i)) ? "d" : "-";
		text +=	(p_engine_container->symbol_engine_active_dealt_playing()->playersplayingbits() & (1<<i)) ? "p" : "-";
		text += "</td>\n";
		player_info += text;
		// Cards
		text.Format("      <td>%s</td>\n", p_table_state->Player(i)->CardsAsHTML());
		player_info += text;
		// Bet
		text.Format("      <td>%11.2f</td>\n", p_table_state->Player(i)->_bet.GetValue());
		player_info += text;
		// Balance
		text.Format("      <td>%11.2f</td>\n", p_table_state->Player(i)->_balance.GetValue());
		player_info += text;
		// Name
		text.Format("      <td>%-15s</td>\n", p_table_state->Player(i)->name().GetString());
		player_info += text;
		player_info += "    </tr>\n";
	}
	// End of table body
	player_info += "  </tbody>\n";
	// End of table
	player_info += "</table>\n";
	return player_info;
}

CString CReplayFrame::GetButtonStatesAsHTML() {
	CString button_states;
	// Table header for: FCKRA
	button_states += "<table align=center border=4 cellpadding=1 cellspacing=1>\n";
	button_states += "  <tr><th>FCKRA</th></tr>\n";
	button_states += "  <tr>\n";
	// Buttons
	CString fckra_seen = p_engine_container->symbol_engine_autoplayer()->GetFCKRAString();
	// Table footer
	button_states += "    <td>";
	button_states += fckra_seen.GetString();
	button_states += "</td>\n",
	button_states += "  </tr>\n";
	button_states += "</table>\n";
	return button_states;
}

CString CReplayFrame::GetBlindInfoAsHTML() {
	CString blind_info, text;
	// Table for: sb, bb, BB
	blind_info += "<table align=center border=4 cellpadding=1 cellspacing=1>\n";
	text.Format("  <tr><th>sb</th><td>%11.2f</td></tr>\n", p_engine_container->symbol_engine_tablelimits()->sblind());
	blind_info += text;
	text.Format("  <tr><th>bb</th><td>%11.2f</td></tr>\n", p_engine_container->symbol_engine_tablelimits()->bblind());
	blind_info += text;
	text.Format("  <tr><th>BB</th><td>%11.2f</td></tr>\n", p_engine_container->symbol_engine_tablelimits()->bigbet());
	blind_info += text;
	text.Format("</table>\n");
	blind_info += text;
	return blind_info;
}

CString CReplayFrame::GetCommonCardsAsHTML() {
	CString common_cards, text;
	// Table header for: common cards
	common_cards += "<table align=center border=4 cellpadding=1 cellspacing=1>\n";
	common_cards += "<tr><th>commoncard</th></tr>\n";
	common_cards += "<tr>\n";
	// Common cards
	text.Format("<td>%s%s%s%s%s</td>\n",
    p_table_state->CommonCards(0)->ToHTML(),
    p_table_state->CommonCards(1)->ToHTML(),
    p_table_state->CommonCards(2)->ToHTML(),
    p_table_state->TurnCard()->ToHTML(),
    p_table_state->RiverCard()->ToHTML());
	common_cards += text;
	// Table footer
	common_cards += "</tr>\n";
	common_cards += "</table>\n";
  return common_cards;
}

CString CReplayFrame::GetPotsAsHTML() {
	CString pots, text;
	// Table header for: pots
	pots += "<table align=center border=4 cellpadding=1 cellspacing=1>\n";
	pots += "  <tr><th>#</th><th>pot</th></tr>\n";
	pots += "  <tr>\n";
	// Main pot
	text.Format("    <td>0</td><td>%11.2f</td>\n", p_table_state->Pot(0));
	pots += text;
	pots += "  </tr>\n";
	// Side pots
	for (int i=1; i<kMaxNumberOfPots; i++) {
		if (p_table_state->Pot(i)) {
			pots += "  <tr>\n";
			text.Format("    <td>%d</td><td>%11.2f</td>\n", i, p_table_state->Pot(i));
			pots += text;
			pots += "  </tr>\n";
		}	else {
			break;
		}
	}
	pots += "</table>\n";
	return pots;
}

void CReplayFrame::CreateReplaySessionDirectoryIfNecessary() {
	assert(p_sessioncounter != nullptr);
	CString path = ReplaySessionDirectory(p_sessioncounter->session_id());
	if (GetFileAttributes(path.GetString()) == INVALID_FILE_ATTRIBUTES)	{
    write_log(Preferences()->debug_replayframes(), "[CReplayFrame] Creating replay directory %s\n", path);
		SHCreateDirectoryEx(NULL, path.GetString(), NULL);
	}
}

CString CReplayFrame::GetLinksToPrevAndNextFile() {
  CString links, text;
  text.Format("<a href=\"frame%06d.htm\">PREV</a>\n",
    _next_replay_frame - 1 >= 0 ? _next_replay_frame - 1 : Preferences()->replay_max_frames());
  links += text;
  text.Format("<a href=\"frame%06d.htm\">NEXT</a>\n",
    _next_replay_frame + 1 < Preferences()->replay_max_frames() ? _next_replay_frame + 1 : 0);
  links += text;
  return links;
}

void CReplayFrame::CreateBitMapFile() {
	CString path;
  CreateReplaySessionDirectoryIfNecessary();
  const char *szFile = ReplayBitmapFilename(p_sessioncounter->session_id(), _next_replay_frame);
  HBITMAP hBMP = p_scraper->entire_window_cur();
  // Saves the hBitmap as a bitmap.
  HDC					hdcScreen = CreateDC("DISPLAY", NULL, NULL, NULL);
  HDC					hdcCompatible = CreateCompatibleDC(hdcScreen);
  PBITMAPINFO	pbmi = NULL;
  HANDLE			hf = NULL; // file handle
  BITMAPFILEHEADER	hdr; // bitmap file-header
  PBITMAPINFOHEADER	pbih = NULL; // bitmap info-header
  LPBYTE			lpBits = NULL; // memory pointer
  DWORD				dwTotal = 0; // total count of bytes
  DWORD				cb = 0; // incremental count of bytes
  BYTE				*hp = NULL; // byte pointer
  DWORD				dwTmp = 0;
  BITMAP			bmp;

  memset(&bmp, 0, sizeof(BITMAP));
  GetObject(hBMP, sizeof(BITMAP), &bmp);
  memset(&hdr, 0, sizeof(hdr));
  int cClrBits = (WORD)(bmp.bmPlanes * bmp.bmBitsPixel);
  if (cClrBits>8) {
    // No Palette (normally)
    pbmi = (PBITMAPINFO)calloc(1, sizeof(BITMAPINFOHEADER));
  } else {
    pbmi = (PBITMAPINFO)calloc(1, sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * (1 << (min(8, cClrBits))));
    pbmi->bmiHeader.biClrUsed = (1 << cClrBits);
  }
  // Initialize the fields in the BITMAPINFO structure.
  pbmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
  // Retrieve the color table (RGBQUAD array) and the bits
  // (array of palette indices) from the DIB.
  if (!GetDIBits(hdcCompatible, hBMP, 0, bmp.bmHeight, NULL, pbmi, DIB_RGB_COLORS)) {
    goto to_return;
  }
  pbih = &(pbmi->bmiHeader);
  pbmi->bmiHeader.biCompression = BI_RGB;
  lpBits = (LPBYTE)calloc(1, pbih->biSizeImage);
  if (!lpBits) {
    goto to_return;
  }
  if (!GetDIBits(hdcCompatible, hBMP, 0, (WORD)pbih->biHeight, lpBits, pbmi, DIB_RGB_COLORS)) {
    goto to_return;
  }
  // Create the .BMP file.
  hf = CreateFile(szFile, GENERIC_READ | GENERIC_WRITE, (DWORD)0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, (HANDLE)NULL);
  if (hf == INVALID_HANDLE_VALUE) {
    goto to_return;
  }
  hdr.bfType = 0x4d42; // 0x42 = "B" 0x4d = "M"
                       // Compute the size of the entire file.
  hdr.bfSize = (DWORD)(sizeof(BITMAPFILEHEADER) + pbih->biSize + pbih->biClrUsed * sizeof(RGBQUAD) + pbih->biSizeImage);
  hdr.bfReserved1 = 0;
  hdr.bfReserved2 = 0;
  // Compute the offset to the array of color indices.
  hdr.bfOffBits = (DWORD) sizeof(BITMAPFILEHEADER) + pbih->biSize + pbih->biClrUsed * sizeof(RGBQUAD);
  // Copy the BITMAPFILEHEADER into the .BMP file.
  if (!WriteFile(hf, (LPVOID)&hdr, sizeof(BITMAPFILEHEADER), (LPDWORD)&dwTmp, NULL)) {
    goto to_return;
  }
  // Copy the BITMAPINFOHEADER and RGBQUAD array into the file.
  if (!WriteFile(hf, (LPVOID)pbih, sizeof(BITMAPINFOHEADER) + pbih->biClrUsed * sizeof(RGBQUAD), (LPDWORD)&dwTmp, (NULL))) {
    goto to_return;
  }
  // Copy the array of color indices into the .BMP file.
  dwTotal = cb = pbih->biSizeImage;
  hp = lpBits;
  if (!WriteFile(hf, (LPSTR)hp, (int)cb, (LPDWORD)&dwTmp, NULL)) {
    goto to_return;
  }
  // Close the .BMP file.
  if (!CloseHandle(hf)) {
    goto to_return;
  }
to_return:
  ;
  // Free memory.
  if (pbmi)free(pbmi);
  if (lpBits) {
    free(lpBits);
  }
  DeleteDC(hdcCompatible);
  DeleteDC(hdcScreen);
}