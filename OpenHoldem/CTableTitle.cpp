//******************************************************************************
//
// This file is part of the OpenHoldem project
//    Source code:           https://github.com/OpenHoldem/openholdembot/
//    Forums:                http://www.maxinmontreal.com/forums/index.php
//    Licensed under GPL v3: http://www.gnu.org/licenses/gpl.html
//
//******************************************************************************
//
// Purpose: small data container for the table-title 
//  which also handles proprocessing of special number formats, ...
//    http://www.maxinmontreal.com/forums/viewtopic.php?f=117&t=20164
//    http://www.maxinmontreal.com/forums/viewtopic.php?f=156&t=19216
//    http://www.maxinmontreal.com/forums/viewtopic.php?f=156&t=19658
//    http://www.maxinmontreal.com/forums/viewtopic.php?f=156&t=20096
//    http://www.maxinmontreal.com/forums/viewtopic.php?f=156&t=20085
//
//******************************************************************************

#include "stdafx.h"
#include "CTableTitle.h"

#ifdef OPENHOLDEM_PROGRAM
#include "CAutoconnector.h"
#endif
#include "..\DLLs\StringFunctions_DLL\string_functions.h"

CTableTitle *p_table_title = NULL;
int CTableTitle::_ohreplay_framenumber;

CTableTitle::CTableTitle() {
#ifdef _DEBUG
  StringFunctionsTest();
#endif
  Clear();
}

CTableTitle::~CTableTitle() {
}

void CTableTitle::UpdateTitle() {
#ifdef OPENHOLDEM_PROGRAM
  if (!p_autoconnector->IsConnectedToAnything()) {
    return;
  }
	char title[MAX_WINDOW_TITLE];
	GetWindowText(p_autoconnector->attached_hwnd(), title, MAX_WINDOW_TITLE-1);
  SetTitle(title);
#endif 
}

void CTableTitle::Clear() {
  SetTitle("");
  _previous_title = "";
  _ohreplay_framenumber = kUndefinedZero;
}

void CTableTitle::SetTitle(CString new_title) {
  _previous_title = _title;
  _title = new_title;
  _preprocessed_title = PreprocessTitle(_title);
}

CString CTableTitle::Title() {
  return _title;
}

CString CTableTitle::PreprocessedTitle() {
  return _preprocessed_title;
}

CString CTableTitle::PreprocessTitle(CString title) {
  // There are some annoying title formats that cause problems
  // for normal title processing. We try to transform these
  // mal-formed title-strings to good ones:
  // * spaces between numbers and cent multipliers at 888
  //   http://www.maxinmontreal.com/forums/viewtopic.php?f=156&t=19216
  // * spaces inside large numbers at WPN
  //   http://www.maxinmontreal.com/forums/viewtopic.php?f=120&t=20096&hilit=space%2A
  // * spaces at the beginning of the title
  //   http://www.maxinmontreal.com/forums/viewtopic.php?f=117&t=20085
  // One more problem:
  // * extra dots in mubers at Microgaming
  //   http://www.maxinmontreal.com/forums/viewtopic.php?f=156&t=19658
  // but the latter one mainly affects balances. 
  CString result = title;
  ExtractOHReplayFrameNumber(&result);
  RemoveOHReplayFrameNumber(&result);
  ReplaceKnownNonASCIICharacters(&result);
  WarnAboutNonASCIICharacters(&result);
  RemoveLeftWhiteSpace(&result);
  RemoveRightWhiteSpace(&result);
  RemoveMultipleWhiteSpaces(&result);
  RemoveSpacesInsideNumbers(&result);
  ReplaceOutlandischCurrencyByDollarsandCents(&result);
  RemoveSpacesInFrontOfCentMultipliers(&result);
  ReplaceCommasInNumbersByDots(&result);
  RemoveExtraDotsInNumbers(&result);
  RemoveHtmlTagsAndContexts(&result);
  return result;
}

bool CTableTitle::TitleChangedSinceLastHeartbeat() {
  return (_title != _previous_title);
}

bool CTableTitle::ContainsSubstring(CString substring) {
  return (_title.Find(substring) >= 0);
}

void CTableTitle::ExtractOHReplayFrameNumber(const CString *s) {
  if (s->GetLength() < 8) {
    return;
  }
  if (RightCharacter(*s) != ']') {
    return;
  }
  if (RightCharacter(*s, 7) != '[') {
    return;
  }
  // Title-format: #################[003141]
  // Extract frame-number
  int length = s->GetLength();
  int start_pos = length - 7;
  CString framenumber = s->Mid(start_pos);
  framenumber.Truncate(6);
  _ohreplay_framenumber = atoi(framenumber);
}