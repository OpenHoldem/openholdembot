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

// DialogSAPrefs20.cpp : implementation file
//

#include "stdafx.h"
#include "DialogSAPrefs20.h"


#include "SAPrefsSubDlg.h"

// CDlgSAPrefs20 dialog

IMPLEMENT_DYNAMIC(CDlgSAPrefs20, CDialog)

CDlgSAPrefs20::CDlgSAPrefs20(CWnd* pParent /*=NULL*/)
	: CSAPrefsSubDlg(CDlgSAPrefs20::IDD, pParent) {
}

CDlgSAPrefs20::~CDlgSAPrefs20() {
}

void CDlgSAPrefs20::DoDataExchange(CDataExchange* pDX) {
	CDialog::DoDataExchange(pDX);
	// No need for any DDX.
	// We do it the good old Win95-way. :)
}

BEGIN_MESSAGE_MAP(CDlgSAPrefs20, CDialog)
  ON_BN_CLICKED(IDC_TOGGLE_ALL_DEBUG, &CDlgSAPrefs20::OnBnClickedToggleAllDebug)
END_MESSAGE_MAP()

// Button position
const int kPositionFirstX         =   4;
const int kPositionFirstY         =  23;
const int kPositionDeltaX         = 155;
const int kPositionDeltaY         =  22;
const int kButtonWidthX           = 150;
const int kButtonWidthY           =  10;
const int kButtonWidthToggleAllY  = 20;
const int kMaxButtonsInYDirection = 23;

// List of button IS,
// alphabetically sorted by button text
const int kNumberOfCheckboxes = 54;
const int kCheckboxIDs[kNumberOfCheckboxes] = {
  IDC_DEBUG_ALLIN_ADJUSTMENT,
  IDC_DEBUG_AST_PRIORITY_ORDERING,
  IDC_DEBUG_AUTOCONNECTOR,
  IDC_DEBUG_AUTOPLAYER,
  IDC_DEBUG_AUTO_TRACE,
  IDC_DEBUG_AUTOSTARTER,
  IDC_DEBUG_BETSIZE_ADJUSTMENT,
  IDC_DEBUG_BLINDLOCKING,
  IDC_DEBUG_BOARD_EXPRESSIONS,
  IDC_DEBUG_DLL_EXTENSION,
  IDC_DEBUG_ENGINE_CONTAINER,
  IDC_DEBUG_ENHANCED_PRWIN,
  IDC_DEBUG_FILENAMES,
  IDC_DEBUG_FORMULA,
  IDC_DEBUG_GUI,
  IDC_DEBUG_HANDHISTORY,
  IDC_DEBUG_HANDRESET_DETECTOR,
  IDC_DEBUG_HEARTBEAT,
  IDC_DEBUG_HOPPER_MESSAGES,
  IDC_DEBUG_ICM,
  IDC_DEBUG_ISTOURNAMENT,
  IDC_DEBUG_LAZY_SCRAPER,
  IDC_DEBUG_MEMORY_SYMBOLS,
  IDC_DEBUG_MEMORY_USAGE,
  IDC_DEBUG_MULTIPLEXER,
  IDC_DEBUG_NUTFULLHOUSE,
  IDC_DEBUG_OCCLUSSION,
  IDC_DEBUG_OPENHOLDEM,
  IDC_DEBUG_PARSER,
  IDC_DEBUG_POKERTRACKER,
  IDC_DEBUG_POPUP_BLOCKER,
  IDC_DEBUG_PREFERENCES,
  IDC_DEBUG_PRWIN,
  IDC_DEBUG_REBUY,
  IDC_DEBUG_REPLAYFRAMES,
  IDC_DEBUG_SCRAPER,
  // Called "scraper directory monitor"
  IDC_DEBUG_FILESYSTEM_MONITOR,
  IDC_DEBUG_SCRAPER_PREPROCESSOR,
  IDC_DEBUG_SESSIONCOUNTER,
  IDC_DEBUG_SHAREDMEM,
  IDC_DEBUG_SINGLETONS,
  IDC_DEBUG_STABLEFRAMESCOUNTER,
  IDC_DEBUG_SYMBOLENGINE,
  IDC_DEBUG_SYMBOLENGINE_OPENPPL,
  IDC_DEBUG_SYMBOL_VERIFICATION,
  IDC_DEBUG_TABLE_LIMITS,
  IDC_DEBUG_TABLEMAP_LOADER,
  IDC_DEBUG_TABLEPOINTS,
  IDC_DEBUG_TABLE_POSITIONER,
  IDC_DEBUG_TIMERS,
  IDC_DEBUG_TOKENIZER,
  IDC_DEBUG_VERSUS,
  IDC_DEBUG_WATCHDOG,
  // "all the rest" at the very end.
  // It contains all temp-debug.
  IDC_DEBUG_ALLTHEREST
};

void CDlgSAPrefs20::AlignButton(int button_ID) {
  assert(_buttons_aligned >= 0);
  assert(_buttons_aligned < (kNumberOfCheckboxes + 2));
  int x_slot = _buttons_aligned / kMaxButtonsInYDirection;
  int y_slot = _buttons_aligned % kMaxButtonsInYDirection;
  int x_position = kPositionFirstX + x_slot * kPositionDeltaX;
  int y_position = kPositionFirstY + y_slot * kPositionDeltaY;
  CWnd* button = GetDlgItem(button_ID);
  assert(button != NULL);
  if (button_ID == IDC_TOGGLE_ALL_DEBUG) {
    // Some extra space for this special button (no checkbox)
    ++_buttons_aligned;
    button->SetWindowPos(NULL, x_position, y_position, kButtonWidthX, kButtonWidthToggleAllY, SWP_SHOWWINDOW);
  } else {
    button->SetWindowPos(NULL, x_position, y_position, kButtonWidthX, kButtonWidthY, SWP_SHOWWINDOW);
  }
  ++_buttons_aligned;
}

void CDlgSAPrefs20::AlignbuttonsInAlphabeticalOrder() {
  _buttons_aligned = 0;
  for (int i = 0; i < kNumberOfCheckboxes; ++i) {
    AlignButton(kCheckboxIDs[i]);
  }
  AlignButton(IDC_TOGGLE_ALL_DEBUG);
}

void CDlgSAPrefs20::OnBnClickedToggleAllDebug() {
  for (int i = 0; i < kNumberOfCheckboxes; ++i) {
    ToggleButton(kCheckboxIDs[i]);
  }
}

void CDlgSAPrefs20::ToggleButton(int button_ID) {
  bool checked = IsDlgButtonChecked(button_ID);
  if (checked) {
    CheckDlgButton(button_ID, MF_UNCHECKED);
  } else {
    CheckDlgButton(button_ID, MF_CHECKED);
  }
}

// CDlgSAPrefs20 message handlers
BOOL CDlgSAPrefs20::OnInitDialog() {	
  AlignbuttonsInAlphabeticalOrder();
	CheckDlgButton(IDC_DEBUG_AUTOCONNECTOR , Preferences()->debug_autoconnector() ? MF_CHECKED : MF_UNCHECKED);
	CheckDlgButton(IDC_DEBUG_AUTOPLAYER, Preferences()->debug_autoplayer() ? MF_CHECKED : MF_UNCHECKED);
	CheckDlgButton(IDC_DEBUG_HEARTBEAT, Preferences()->debug_heartbeat() ? MF_CHECKED : MF_UNCHECKED);
	CheckDlgButton(IDC_DEBUG_PRWIN, Preferences()->debug_prwin() ? MF_CHECKED : MF_UNCHECKED);
	CheckDlgButton(IDC_DEBUG_ICM, Preferences()->debug_icm() ? MF_CHECKED : MF_UNCHECKED);
	CheckDlgButton(IDC_DEBUG_OCCLUSSION, Preferences()->debug_occlusionchecker() ? MF_CHECKED : MF_UNCHECKED);
	CheckDlgButton(IDC_DEBUG_POKERTRACKER, Preferences()->debug_pokertracker() ? MF_CHECKED : MF_UNCHECKED);
	CheckDlgButton(IDC_DEBUG_REBUY, Preferences()->debug_rebuy() ? MF_CHECKED : MF_UNCHECKED);
	CheckDlgButton(IDC_DEBUG_REPLAYFRAMES, Preferences()->debug_replayframes() ? MF_CHECKED : MF_UNCHECKED);
	CheckDlgButton(IDC_DEBUG_SCRAPER, Preferences()->debug_scraper() ? MF_CHECKED : MF_UNCHECKED);
	CheckDlgButton(IDC_DEBUG_SESSIONCOUNTER, Preferences()->debug_sessioncounter() ? MF_CHECKED : MF_UNCHECKED);
	CheckDlgButton(IDC_DEBUG_STABLEFRAMESCOUNTER, Preferences()->debug_stableframescounter() ? MF_CHECKED : MF_UNCHECKED);
	CheckDlgButton(IDC_DEBUG_SYMBOLENGINE, Preferences()->debug_symbolengine() ? MF_CHECKED : MF_UNCHECKED);
	CheckDlgButton(IDC_DEBUG_SYMBOLENGINE_OPENPPL, Preferences()->debug_symbolengine_open_ppl() ? MF_CHECKED : MF_UNCHECKED);
  CheckDlgButton(IDC_DEBUG_MEMORY_USAGE, Preferences()->debug_memory_usage() ? MF_CHECKED : MF_UNCHECKED);
  CheckDlgButton(IDC_DEBUG_PARSER, Preferences()->debug_parser() ? MF_CHECKED : MF_UNCHECKED);
	CheckDlgButton(IDC_DEBUG_BLINDLOCKING, Preferences()->debug_blindlocking() ? MF_CHECKED : MF_UNCHECKED);
	CheckDlgButton(IDC_DEBUG_MEMORY_SYMBOLS, Preferences()->debug_memorysymbols() ? MF_CHECKED : MF_UNCHECKED);
	CheckDlgButton(IDC_DEBUG_TABLEMAP_LOADER, Preferences()->debug_tablemap_loader() ? MF_CHECKED : MF_UNCHECKED);
	CheckDlgButton(IDC_DEBUG_FILESYSTEM_MONITOR, Preferences()->debug_filesystem_monitor() ? MF_CHECKED : MF_UNCHECKED);
	CheckDlgButton(IDC_DEBUG_TABLE_POSITIONER, Preferences()->debug_table_positioner() ? MF_CHECKED : MF_UNCHECKED);
	CheckDlgButton(IDC_DEBUG_ISTOURNAMENT, Preferences()->debug_istournament() ? MF_CHECKED : MF_UNCHECKED);
	CheckDlgButton(IDC_DEBUG_GUI, Preferences()->debug_gui() ? MF_CHECKED : MF_UNCHECKED);
	CheckDlgButton(IDC_DEBUG_TABLE_LIMITS, Preferences()->debug_table_limits() ? MF_CHECKED : MF_UNCHECKED);
	CheckDlgButton(IDC_DEBUG_LAZY_SCRAPER, Preferences()->debug_lazy_scraper() ? MF_CHECKED : MF_UNCHECKED);
	CheckDlgButton(IDC_DEBUG_BETSIZE_ADJUSTMENT, Preferences()->debug_betsize_adjustment() ? MF_CHECKED : MF_UNCHECKED);
  CheckDlgButton(IDC_DEBUG_ALLIN_ADJUSTMENT, Preferences()->debug_allin_adjustment() ? MF_CHECKED : MF_UNCHECKED);
	CheckDlgButton(IDC_DEBUG_HANDRESET_DETECTOR, Preferences()->debug_handreset_detector() ? MF_CHECKED : MF_UNCHECKED);
	CheckDlgButton(IDC_DEBUG_ENGINE_CONTAINER, Preferences()->debug_engine_container() ? MF_CHECKED : MF_UNCHECKED);
	CheckDlgButton(IDC_DEBUG_ENHANCED_PRWIN, Preferences()->debug_enhanced_prwin() ? MF_CHECKED : MF_UNCHECKED);
	CheckDlgButton(IDC_DEBUG_HANDHISTORY, Preferences()->debug_handhistory() ? MF_CHECKED : MF_UNCHECKED);
	CheckDlgButton(IDC_DEBUG_PREFERENCES, Preferences()->debug_preferences() ? MF_CHECKED : MF_UNCHECKED);
	CheckDlgButton(IDC_DEBUG_DLL_EXTENSION, Preferences()->debug_dll_extension() ? MF_CHECKED : MF_UNCHECKED);
	CheckDlgButton(IDC_DEBUG_SCRAPER_PREPROCESSOR, Preferences()->debug_scraper_preprocessor() ? MF_CHECKED : MF_UNCHECKED);
	CheckDlgButton(IDC_DEBUG_HOPPER_MESSAGES, Preferences()->debug_hopper_messages() ? MF_CHECKED : MF_UNCHECKED);
	CheckDlgButton(IDC_DEBUG_OPENHOLDEM, Preferences()->debug_openholdem() ? MF_CHECKED : MF_UNCHECKED);
  CheckDlgButton(IDC_DEBUG_POPUP_BLOCKER, Preferences()->debug_popup_blocker() ? MF_CHECKED : MF_UNCHECKED);
  CheckDlgButton(IDC_DEBUG_FORMULA, Preferences()->debug_formula() ? MF_CHECKED : MF_UNCHECKED);
  CheckDlgButton(IDC_DEBUG_ALLTHEREST, Preferences()->debug_alltherest() ? MF_CHECKED : MF_UNCHECKED);
  CheckDlgButton(IDC_DEBUG_VERSUS, Preferences()->debug_versus() ? MF_CHECKED : MF_UNCHECKED);
  CheckDlgButton(IDC_DEBUG_AUTO_TRACE, Preferences()->debug_auto_trace() ? MF_CHECKED : MF_UNCHECKED);
  CheckDlgButton(IDC_DEBUG_FILENAMES, Preferences()->debug_filenames() ? MF_CHECKED : MF_UNCHECKED);
  CheckDlgButton(IDC_DEBUG_AST_PRIORITY_ORDERING, Preferences()->debug_ast_priority_ordering() ? MF_CHECKED : MF_UNCHECKED);
  CheckDlgButton(IDC_DEBUG_BOARD_EXPRESSIONS, Preferences()->debug_hand_and_board_expressions() ? MF_CHECKED : MF_UNCHECKED);
  CheckDlgButton(IDC_DEBUG_TOKENIZER, Preferences()->debug_tokenizer() ? MF_CHECKED : MF_UNCHECKED);
  CheckDlgButton(IDC_DEBUG_TABLEPOINTS, Preferences()->debug_tablepoints() ? MF_CHECKED : MF_UNCHECKED);
  CheckDlgButton(IDC_DEBUG_SHAREDMEM, Preferences()->debug_sharedmem() ? MF_CHECKED : MF_UNCHECKED);
  CheckDlgButton(IDC_DEBUG_SINGLETONS, Preferences()->debug_singletons() ? MF_CHECKED : MF_UNCHECKED);
  CheckDlgButton(IDC_DEBUG_AUTOSTARTER, Preferences()->debug_autostarter() ? MF_CHECKED : MF_UNCHECKED);
  CheckDlgButton(IDC_DEBUG_WATCHDOG, Preferences()->debug_watchdog() ? MF_CHECKED : MF_UNCHECKED);
  CheckDlgButton(IDC_DEBUG_TIMERS, Preferences()->debug_timers() ? MF_CHECKED : MF_UNCHECKED);
  CheckDlgButton(IDC_DEBUG_NUTFULLHOUSE, Preferences()->debug_nutfullhouse() ? MF_CHECKED : MF_UNCHECKED);
  CheckDlgButton(IDC_DEBUG_MULTIPLEXER, Preferences()->debug_multiplexer() ? MF_CHECKED : MF_UNCHECKED);
  CheckDlgButton(IDC_DEBUG_SYMBOL_VERIFICATION, Preferences()->debug_symbol_verification() ? MF_CHECKED : MF_UNCHECKED);
  return TRUE;  // return TRUE unless you set the focus to a control
  // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgSAPrefs20::OnOK() {
	Preferences()->SetValue(k_prefs_debug_autoconnector, IsDlgButtonChecked(IDC_DEBUG_AUTOCONNECTOR));
	Preferences()->SetValue(k_prefs_debug_autoplayer, IsDlgButtonChecked(IDC_DEBUG_AUTOPLAYER));
	Preferences()->SetValue(k_prefs_debug_heartbeat, IsDlgButtonChecked(IDC_DEBUG_HEARTBEAT));
	Preferences()->SetValue(k_prefs_debug_prwin, IsDlgButtonChecked(IDC_DEBUG_PRWIN));
	Preferences()->SetValue(k_prefs_debug_icm, IsDlgButtonChecked(IDC_DEBUG_ICM));
	Preferences()->SetValue(k_prefs_debug_occlusionchecker, IsDlgButtonChecked(IDC_DEBUG_OCCLUSSION));
	Preferences()->SetValue(k_prefs_debug_pokertracker, IsDlgButtonChecked(IDC_DEBUG_POKERTRACKER));
	Preferences()->SetValue(k_prefs_debug_rebuy, IsDlgButtonChecked(IDC_DEBUG_REBUY));
	Preferences()->SetValue(k_prefs_debug_replayframes, IsDlgButtonChecked(IDC_DEBUG_REPLAYFRAMES));
	Preferences()->SetValue(k_prefs_debug_scraper, IsDlgButtonChecked(IDC_DEBUG_SCRAPER));
	Preferences()->SetValue(k_prefs_debug_sessioncounter, IsDlgButtonChecked(IDC_DEBUG_SESSIONCOUNTER));
	Preferences()->SetValue(k_prefs_debug_stableframescounter, IsDlgButtonChecked(IDC_DEBUG_STABLEFRAMESCOUNTER));
	Preferences()->SetValue(k_prefs_debug_symbolengine, IsDlgButtonChecked(IDC_DEBUG_SYMBOLENGINE));
	Preferences()->SetValue(k_prefs_debug_symbolengine_open_ppl, IsDlgButtonChecked(IDC_DEBUG_SYMBOLENGINE_OPENPPL));
  Preferences()->SetValue(k_prefs_debug_memory_usage, IsDlgButtonChecked(IDC_DEBUG_MEMORY_USAGE));
  Preferences()->SetValue(k_prefs_debug_parser, IsDlgButtonChecked(IDC_DEBUG_PARSER));
	Preferences()->SetValue(k_prefs_debug_blindlocking, IsDlgButtonChecked(IDC_DEBUG_BLINDLOCKING));
	Preferences()->SetValue(k_prefs_debug_memorysymbols, IsDlgButtonChecked(IDC_DEBUG_MEMORY_SYMBOLS));
	Preferences()->SetValue(k_prefs_debug_tablemap_loader, IsDlgButtonChecked(IDC_DEBUG_TABLEMAP_LOADER));
	Preferences()->SetValue(k_prefs_debug_filesystem_monitor, IsDlgButtonChecked(IDC_DEBUG_FILESYSTEM_MONITOR));
	Preferences()->SetValue(k_prefs_debug_table_positioner, IsDlgButtonChecked(IDC_DEBUG_TABLE_POSITIONER));
	Preferences()->SetValue(k_prefs_debug_istournament, IsDlgButtonChecked(IDC_DEBUG_ISTOURNAMENT));
	Preferences()->SetValue(k_prefs_debug_gui, IsDlgButtonChecked(IDC_DEBUG_GUI));
	Preferences()->SetValue(k_prefs_debug_table_limits, IsDlgButtonChecked(IDC_DEBUG_TABLE_LIMITS));
	Preferences()->SetValue(k_prefs_debug_lazy_scraper, IsDlgButtonChecked(IDC_DEBUG_LAZY_SCRAPER));
	Preferences()->SetValue(k_prefs_debug_betsize_adjustment, IsDlgButtonChecked(IDC_DEBUG_BETSIZE_ADJUSTMENT));
  Preferences()->SetValue(k_prefs_debug_allin_adjustment, IsDlgButtonChecked(IDC_DEBUG_ALLIN_ADJUSTMENT));
	Preferences()->SetValue(k_prefs_debug_handreset_detector, IsDlgButtonChecked(IDC_DEBUG_HANDRESET_DETECTOR));
	Preferences()->SetValue(k_prefs_debug_engine_container, IsDlgButtonChecked(IDC_DEBUG_ENGINE_CONTAINER));
	Preferences()->SetValue(k_prefs_debug_enhanced_prwin, IsDlgButtonChecked(IDC_DEBUG_ENHANCED_PRWIN));
	Preferences()->SetValue(k_prefs_debug_handhistory, IsDlgButtonChecked(IDC_DEBUG_HANDHISTORY));
	Preferences()->SetValue(k_prefs_debug_preferences, IsDlgButtonChecked(IDC_DEBUG_PREFERENCES));
	Preferences()->SetValue(k_prefs_debug_dll_extension, IsDlgButtonChecked(IDC_DEBUG_DLL_EXTENSION));
	Preferences()->SetValue(k_prefs_debug_scraper_preprocessor, IsDlgButtonChecked(IDC_DEBUG_SCRAPER_PREPROCESSOR));
	Preferences()->SetValue(k_prefs_debug_hopper_messages, IsDlgButtonChecked(IDC_DEBUG_HOPPER_MESSAGES));
	Preferences()->SetValue(k_prefs_debug_openholdem, IsDlgButtonChecked(IDC_DEBUG_OPENHOLDEM));
	Preferences()->SetValue(k_prefs_debug_popup_blocker, IsDlgButtonChecked(IDC_DEBUG_POPUP_BLOCKER));
  Preferences()->SetValue(k_prefs_debug_formula, IsDlgButtonChecked(IDC_DEBUG_FORMULA));
	Preferences()->SetValue(k_prefs_debug_alltherest, IsDlgButtonChecked(IDC_DEBUG_ALLTHEREST));
  Preferences()->SetValue(k_prefs_debug_versus, IsDlgButtonChecked(IDC_DEBUG_VERSUS));
  Preferences()->SetValue(k_prefs_debug_auto_trace, IsDlgButtonChecked(IDC_DEBUG_AUTO_TRACE));
  Preferences()->SetValue(k_prefs_debug_filenames, IsDlgButtonChecked(IDC_DEBUG_FILENAMES));
  Preferences()->SetValue(k_prefs_debug_ast_priority_ordering, IsDlgButtonChecked(IDC_DEBUG_AST_PRIORITY_ORDERING));
  Preferences()->SetValue(k_prefs_debug_hand_and_board_expressions, IsDlgButtonChecked(IDC_DEBUG_BOARD_EXPRESSIONS));
  Preferences()->SetValue(k_prefs_debug_tokenizer, IsDlgButtonChecked(IDC_DEBUG_TOKENIZER));
  Preferences()->SetValue(k_prefs_debug_tablepoints, IsDlgButtonChecked(IDC_DEBUG_TABLEPOINTS));
  Preferences()->SetValue(k_prefs_debug_sharedmem, IsDlgButtonChecked(IDC_DEBUG_SHAREDMEM));
  Preferences()->SetValue(k_prefs_debug_singletons, IsDlgButtonChecked(IDC_DEBUG_SINGLETONS));
  Preferences()->SetValue(k_prefs_debug_autostarter, IsDlgButtonChecked(IDC_DEBUG_AUTOSTARTER));
  Preferences()->SetValue(k_prefs_debug_watchdog, IsDlgButtonChecked(IDC_DEBUG_WATCHDOG));
  Preferences()->SetValue(k_prefs_debug_timers, IsDlgButtonChecked(IDC_DEBUG_TIMERS));
  Preferences()->SetValue(k_prefs_debug_nutfullhouse, IsDlgButtonChecked(IDC_DEBUG_NUTFULLHOUSE));
  Preferences()->SetValue(k_prefs_debug_multiplexer, IsDlgButtonChecked(IDC_DEBUG_MULTIPLEXER));
  Preferences()->SetValue(k_prefs_debug_symbol_verification, IsDlgButtonChecked(IDC_DEBUG_SYMBOL_VERIFICATION));
	CSAPrefsSubDlg::OnOK();
}
