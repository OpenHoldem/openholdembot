//******************************************************************************
//
// This file is part of the OpenHoldem project
//    Source code:           https://github.com/OpenHoldem/openholdembot/
//    Forums:                http://www.maxinmontreal.com/forums/index.php
//    Licensed under GPL v3: http://www.gnu.org/licenses/gpl.html
//
//******************************************************************************
//
// Purpose: Symbol lookup for various symbols 
//   that are not part of a regular symbol-engine
//
//******************************************************************************

#include "stdafx.h"
#include "CSymbolEngineVariousDataLookup.h"

#include <assert.h>
#include <process.h>
#include <float.h>

#include "CAutoconnector.h"
#include "CAutoplayerTrace.h"
#include "CBetroundCalculator.h"
#include "CEngineContainer.h"
#include "CFlagsToolbar.h"
#include "CFormulaParser.h"
#include "inlines/eval.h"
#include "Chair$Symbols.h"
#include "CHandresetDetector.h"
#include "CIteratorThread.h"
#include "CPokerTrackerThread.h"

#include "CScraper.h"
#include "CSessionCounter.h"
#include "CStringMatch.h"
#include "CSymbolEngineUserchair.h"
#include "..\CTablemap\CTablemap.h"
#include "..\CTransform\CTransform.h"
#include "CTableTitle.h"
#include "CWhiteInfoBox.h"
#include "RtaWindow.h"

#include "OpenHoldem.h"
#include "..\DLLs\WindowFunctions_DLL\window_functions.h"

CSymbolEngineVariousDataLookup			*p_symbol_engine_various_data_lookup = NULL;


CSymbolEngineVariousDataLookup::CSymbolEngineVariousDataLookup() {
  // The values of some symbol-engines depend on other engines.
  // As the engines get later called in the order of initialization
  // we assure correct ordering by checking if they are initialized.
  assert(p_engine_container->symbol_engine_userchair() != NULL);
  // Other objects that we depend on
  assert(p_autoconnector != NULL);
  assert(p_betround_calculator != NULL);
  assert(p_handreset_detector != NULL);
  assert(p_formula_parser != NULL);
  assert(p_sessioncounter != NULL);
  assert(p_tablemap != NULL);
  assert(p_table_title != NULL);
  assert(p_white_info_box != NULL);
  // Objects that are part of the GUI which runs in its own thread.
  // They might or might not yet be initialized.
  // We don#t wait here, as the GUI also waits at some point.
  // We check these pointers only when really needed
  //
  // assert(p_flags_toolbar != NULL);
}

CSymbolEngineVariousDataLookup::~CSymbolEngineVariousDataLookup() {
}

void CSymbolEngineVariousDataLookup::InitOnStartup() {
}

void CSymbolEngineVariousDataLookup::UpdateOnConnection() {
}

void CSymbolEngineVariousDataLookup::UpdateOnHandreset() {
  // Reset display
  InvalidateRect(theApp.m_pMainWnd->GetSafeHwnd(), NULL, true);
}

void CSymbolEngineVariousDataLookup::UpdateOnNewRound() {
}

void CSymbolEngineVariousDataLookup::UpdateOnMyTurn() {
}

void CSymbolEngineVariousDataLookup::UpdateOnHeartbeat() {
}

bool CSymbolEngineVariousDataLookup::EvaluateSymbol(const CString name, double *result, bool log /* = false */) {
  FAST_EXIT_ON_OPENPPL_SYMBOLS(name);
  // Various symbols below
  // without any optimized lookup.
  // Betting rounds
  if (memcmp(name, "betround", 8)==0 && strlen(name)==8)	*result = p_betround_calculator->betround();
  else if (name == "currentround") *result = p_betround_calculator->betround();
  else if (name == "previousround") *result = p_betround_calculator->PreviousRound();
  // GENERAL
  else if (memcmp(name, "session", 7)==0 && strlen(name)==7)	*result = p_sessioncounter->session_id();
  else if (memcmp(name, "version", 7)==0 && strlen(name)==7)	*result = VERSION_NUMBER;
  // Handreset
  else if (memcmp(name, "handsplayed", 11)==0 && strlen(name)==11) *result = p_handreset_detector->hands_played();
  else if (memcmp(name, "handsplayed_headsup", 19)==0 && strlen(name)==19)  *result = p_handreset_detector->hands_played_headsup();
  else if (name == kEmptyExpression_False_Zero_WhenOthersFoldForce) { *result = kUndefinedZero; }
  // OH-script-messagebox
  else if (memcmp(name, "msgbox$", 7)==0 && strlen(name)>7) {
    // Don't show name messagebox if in parsing-mode
    // We might however want to show a message if we are not yet connected,
    // e.g. in f$ini_function_pn_startup
    write_log(Preferences()->debug_alltherest(), "[CSymbolEngineVariousDataLookup] location Johnny_8\n");
    if (p_formula_parser->IsParsing()) {
	    *result = 0;
    } else {
	    MessageBox_OH_Script_Messages(name);
	    *result = 0;
    }
  } else if ((memcmp(name, "log$", 4)==0) && (strlen(name)>4)) {
    if (!p_formula_parser->IsParsing()) {
      write_log(Preferences()->debug_auto_trace(), 
        "[CSymbolEngineVariousDataLookup] %s -> 0.000 [just logged]\n", name);
      if ((memcmp(name, "log$_", 5) == 0) && (strlen(name) > 5)) {
          p_white_info_box->SetActionTraceLogMessage(name);
          p_rta_window->SetActionTraceLogMessage(name);
      } else {
      p_white_info_box->SetCustomLogMessage(name);
	  p_rta_window->SetCustomLogMessage(name);
      }
    }
    // True (1) is convenient in sequences of ANDed conditions
    // http://www.maxinmontreal.com/forums/viewtopic.php?f=110&t=19421
    *result = true;
  } else if ((memcmp(name, "attached_hwnd", 13)==0) && (strlen(name)==13)) {
    *result = int(p_autoconnector->attached_hwnd());
  } else if ((memcmp(name, "islobby", 7)==0) && (strlen(name)==7)) {
    *result = p_tablemap->islobby();
  }  else if ((memcmp(name, "ispopup", 7) == 0) && (strlen(name) == 7)) {
    *result = p_tablemap->ispopup();
  } else if ((memcmp(name, "title$", 6) == 0) && (strlen(name) >= 7)) {
    CString substring = CString(name).Mid(6);
    *result = p_table_title->ContainsSubstring(substring);
  } else if ((memcmp(name, kEmptyExpression_False_Zero_WhenOthersFoldForce, strlen(kEmptyExpression_False_Zero_WhenOthersFoldForce))==0) 
      && (strlen(name)==strlen(kEmptyExpression_False_Zero_WhenOthersFoldForce))) {
    *result = kUndefinedZero;
  } else if (memcmp(name, "f", 1) == 0 && strlen(name) == 2) {
    if (p_flags_toolbar != NULL) {
      *result = p_flags_toolbar->GetFlag(RightDigitCharacterToNumber(name));
    } else {
      *result = kUndefinedZero;
    }
  } else if (memcmp(name, "f", 1) == 0 && strlen(name) == 3) {
    if (p_flags_toolbar != NULL) {
      *result = p_flags_toolbar->GetFlag(10 * RightDigitCharacterToNumber(name, 1) + RightDigitCharacterToNumber(name, 0));
    } else {
      *result = kUndefinedZero;
    }
  } else if (memcmp(name, "fmax", 4) == 0 && strlen(name) == 4) {
    if (p_flags_toolbar != NULL) {
      *result = p_flags_toolbar->GetFlagMax();
    } else {
      *result = kUndefinedZero;
    }
  } else if (memcmp(name, "flagbits", 8) == 0 && strlen(name) == 8) {
    if (p_flags_toolbar != NULL) {
      *result = p_flags_toolbar->GetFlagBits();
    } else {
      *result = kUndefinedZero;
    }
  } else {
    // Special symbol for empty expressions. Its evaluation adds something 
    // meaningful to the log when the end of an open-ended when-condition 
    // gets reached during evaluation.
    *result = kUndefined;
    return false;
  }
  return true;
}

CString CSymbolEngineVariousDataLookup::SymbolsProvided() {
  // This list includes some prefixes of symbols that can't be verified,
  // e.g. pl_ chair$, ....
  CString list = "pl_ vs$ msgbox$ log$ "
    "betround currentround previousround "
    "fmax flagbits "
    "session version islobby ispopup"
    "handsplayed handsplayed_headsup ";
  list += RangeOfSymbols("f%i", 0, 19);
  return list;
}



