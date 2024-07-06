//******************************************************************************
//
// This file is part of the OpenHoldem project
//    Source code:           https://github.com/OpenHoldem/openholdembot/
//    Forums:                http://www.maxinmontreal.com/forums/index.php
//    Licensed under GPL v3: http://www.gnu.org/licenses/gpl.html
//
//******************************************************************************
//
// Purpose: Storing and loading options from/ro ini-files
//
//******************************************************************************

#include "Preferences.h"
#include "MagicNumbers/MagicNumbers.h"
#include "Debugger/debugger.h"
#include "Files/Files.h"
#include "StringFunctions/string_functions.h"
#include "WindowFunctions/window_functions.h"

//
// Constructor and destructor
//
CPreferences::CPreferences() {
  ini_filename = IniFilePath();
}

CPreferences::~CPreferences() {
}

// registry keys 
const char* k_registry_keys_for_numerical_values[k_prefs_last_numerical_value + 1] = {
  // bools
  "debug_preferences",
  "debug_autoconnector",
  "debug_autoplayer",
  "debug_heartbeat",
  "debug_prwin",
  "debug_icm",
  "debug_occlusionchecker",
  "debug_pokertracker",
  "debug_rebuy",
  "debug_replayframes",
  "debug_scraper",
  "debug_sessioncounter",
  "debug_stableframescounter",
  "debug_symbolengine",
  "debug_symbolengine_open_ppl",
  "debug_memory_usage",
  "debug_parser",
  "debug_blindlocking",
  "debug_memorysymbols",
  "debug_tablemap_loader",
  "debug_filesystem_monitor",
  "debug_handhistory",
  "debug_hopper_messages",
  "debug_alltherest",
  "debug_table_positioner",
  "debug_istournament",
  "debug_gui",
  "debug_table_limits",
  "debug_lazy_scraper",
  "debug_betsize_adjustment",
  "debug_allin_adjustment",
  "debug_handreset_detector",
  "debug_engine_container",
  "debug_dll_extension",
  "debug_scraper_preprocessor",
  "debug_openholdem",
  "debug_popup_blocker",
  "debug_formula",
  "debug_versus",
  "debug_auto_trace",
  "debug_ast_priority_ordering",
  "debug_filenames",
  "debug_hand_and_board_expressions",
  "debug_tokenizer",
  "debug_tablepoints",
  "debug_sharedmem",
  "debug_singletons",
  "debug_autostarter",
  "debug_watchdog",
  "debug_timers",
  "debug_nutfullhouse",
  "debug_multiplexer",
  "debug_symbol_verification",
  "engage_autoplayer",
  "restore_position_and_focus",
  "use_auto_replay",
  "replay_record",
  "log_delay_function",
  "log_hopper_functions",
  "log_icm_functions",
  "log_ini_functions",
  "log_prwin_functions",
  "disable_msgbox",
  "validator_stop_on_error",
  "validator_use_heuristic_rules",
  "validator_shoot_replayframe_on_error",
  "validator_enabled",
  "use_auto_starter",
  "use_auto_shutdown",
  "gui_first_visible",
  "gui_all_minimized",
  "gui_less",
  "rebuy_condition_no_cards",
  "rebuy_condition_change_in_handnumber",
  "rebuy_condition_heuristic_check_for_occlusion",
  "configurationcheck_input_settings",
  "configurationcheck_font_settings",
  "handhistory_generator_enable",
  "simple_window_title",
  "table_positioner_always_keep_position",
  // ints
  "replay_max_frames",
  "frame_delay",
  "click_delay",
  "betsize_delay_1",
  "betsize_delay_2",
  "betsize_delay_3",
  "scrape_delay",
  "chat_min_delay",
  "chat_random_delay",
  "log_max_logsize",
  "formula_x",
  "formula_y",
  "formula_dx",
  "formula_dy",
  "main_x",
  "main_y",
  "autoconnector_when_to_connect",
  "rebuy_minimum_time_to_next_try",
  "lazy_scraping_when_to_scrape",
  "table_positioner_options",
  "scraper_zoom",
  "popup_blocker",
  // doubles
  // Only one double left, just to check for outdated ICM-config
  "icm_prize1",
  //
  // Finally an empty string
  // This correspondents to k_prefs_last_numerical_value (unused)
  // It will cause an assertion if used
  // and therefore help us to detect missing values.
  "",
};

const char* k_registry_keys_for_CStrings[k_prefs_last_CString_value + 1] = {
  "pt_ip_addr",
  "pt_port",
  "pt_dbname",
  "pt_user",
  "pt_pass",
  "rebuy_script",
  "window_class_name",
  "mutex_name",
  // Finally an empty string
  // This correspondents to k_prefs_last_numerical_value (unused)
  // It will cause an assertion if used
  // and therefore help us to detect missing values.
  "",
};

void CPreferences::LoadPreferences() {
#ifdef _DEBUG
    write_log(true, "[Debug] Loading preferences is started\n");
#endif
  InitDefaults();
  ReadPreferences();
#ifdef _DEBUG
  write_log(true, "[Debug] Loading preferences is finished\n");
#endif
}


void CPreferences::CheckForOutdatedICMConfig() {
  if (prefs_numerical_values[k_prefs_icm_prize1] != kUndefinedZero) {
    MessageBox_Error_Warning(
      "Outdated ICM-configuration\n"
      "\n"
      "OpenHoldem does no longer use the ini-file for that\n"
      "but f$icm_prize1 .. f$icm_prize9 functions\n"
      "\n"
      "Now you can adapt the payout-structure at runtime\n"
      "depending on casino, game-type, number of players...");
  }
}

//
// Private method to initialize the default values
//
void CPreferences::InitDefaults(void) {
  // General initialization
  // that fits most values and purposes
  for (int i = 0; i<k_prefs_last_numerical_value; i++) {
    prefs_numerical_values[i] = 0.0;	// 0 for ints and false for bools
  }
  for (int i = 0; i<k_prefs_last_CString_value; i++) {
    prefs_CString_values[i] = "";
  }
  // Initialization of data that requires special values
  // Bools
  prefs_numerical_values[k_prefs_engage_autoplayer] = true;
  prefs_numerical_values[k_prefs_gui_first_visible] = true;
  prefs_numerical_values[k_prefs_restore_position_and_focus] = false;
  prefs_numerical_values[k_prefs_use_auto_replay] = true;
  prefs_numerical_values[k_prefs_use_auto_starter] = true;
  prefs_numerical_values[k_prefs_use_auto_shutdown] = true;
  prefs_numerical_values[k_prefs_debug_preferences] = true; // To assure that we log at least the initialization of this value
  prefs_numerical_values[k_prefs_rebuy_condition_no_cards] = true;
  prefs_numerical_values[k_prefs_rebuy_condition_change_in_handnumber] = true;
  prefs_numerical_values[k_prefs_rebuy_condition_heuristic_check_for_occlusion] = true;
  prefs_numerical_values[k_prefs_configurationcheck_font_settings] = true;
  prefs_numerical_values[k_prefs_configurationcheck_input_settings] = false;
  // Ints
  prefs_numerical_values[k_prefs_formula_dx] = 640;
  prefs_numerical_values[k_prefs_formula_dy] = 400;
  prefs_numerical_values[k_prefs_frame_delay] = 2;
  prefs_numerical_values[k_prefs_click_delay] = 250;
  prefs_numerical_values[k_prefs_swag_delay_1] = 400;
  prefs_numerical_values[k_prefs_swag_delay_2] = 400;
  prefs_numerical_values[k_prefs_swag_delay_3] = 700;
  prefs_numerical_values[k_prefs_scrape_delay] = 750;
  prefs_numerical_values[k_prefs_scraper_zoom] = 2;			// 4x
  prefs_numerical_values[k_prefs_replay_max_frames] = 100;
  prefs_numerical_values[k_prefs_chat_min_delay] = 600;		//  seconds
  prefs_numerical_values[k_prefs_chat_random_delay] = 3000;	//  seconds;
  prefs_numerical_values[k_prefs_log_max_logsize] = 10;		// MB		
  prefs_numerical_values[k_prefs_autoconnector_when_to_connect] = k_AutoConnector_Connect_Permanent;
  prefs_numerical_values[k_prefs_rebuy_minimum_time_to_next_try] = 30;
  prefs_numerical_values[k_prefs_lazy_scraping_when_to_scrape] = k_lazy_scraping_always;
  prefs_numerical_values[k_prefs_table_positioner_options] = k_position_tables_cascaded;
  // Doubles
  prefs_numerical_values[k_prefs_icm_prize1] = kUndefinedZero;
  // CString
  for (int i = 0; i < k_prefs_last_CString_value; ++i) {
    prefs_CString_values[i] = DefaultStringValues(i);
  }
}

CString CPreferences::DefaultStringValues(int index) {
  switch (index) {
  case k_prefs_pt_ip_addr:
    return "127.0.0.1";
  case k_prefs_pt_port:
    return "5432";
  case k_prefs_pt_dbname:
    return "noname";
  case k_prefs_pt_user:
    return "pokertracker";
  case k_prefs_pt_pass:
    return "*** top secret ***";
  case k_prefs_rebuy_script:
    return "Tools\\RebuyDemo.exe";
  case k_prefs_window_class_name:
    return "OpenHoldem";
  case k_prefs_mutex_name:
    return "OHAntiColl";
  default:
    assert(kThisMustNotHappen);
    return "Error in CPreferences::DefaultStringValues()";
  }
}

CString CPreferences::NonEmptyStringValueElseDefault(int index) {
  assert(index >= 0);
  assert(index < k_prefs_last_CString_value);
  if (prefs_CString_values[index] == "") {
    // Empty values are dangerous
    // An empty window_class_name will even crash MFC
    // http://www.maxinmontreal.com/forums/viewtopic.php?f=257&t=21306
	  // Other empty values don't make much sense either.
    // Therefore we restore the default here.
    prefs_CString_values[index] = DefaultStringValues(index);
  }
  return prefs_CString_values[index];
}

//
// Private methods to access the registry
//

void CPreferences::ReadPreferences() {
  // Then read the registry values and overwrite the defaults if defined
  for (int i = 0; i<k_prefs_last_numerical_value; i++) {
    assert(k_registry_keys_for_numerical_values[i] != "");
    /*write_log(debug_preferences(), "[CPreferences] Reading %2i. (numerical) %s\n",
      i, k_registry_keys_for_numerical_values[i]);*/
    ReadReg(k_registry_keys_for_numerical_values[i], &prefs_numerical_values[i]);
  }
  for (int i = 0; i<k_prefs_last_CString_value; i++) {
    assert(k_registry_keys_for_CStrings[i] != "");
    /*write_log(debug_preferences(), "[CPreferences] Reading %2i. (textual) %s\n",
      i, k_registry_keys_for_CStrings[i]);*/
    ReadReg(k_registry_keys_for_CStrings[i], &prefs_CString_values[i]);
  }
}

void CPreferences::ReadReg(const LPCTSTR registry_key, CString *registry_value) {
  const int kSizeOgInputBuffer = 1000;
  char input_buffer[kSizeOgInputBuffer];
  // https://msdn.microsoft.com/en-us/library/windows/desktop/ms724353(v=vs.85).aspx
  // The parameters are
  //   * ini-file-section
  //   * ini-file-key
  //   * default
  //   * result-buffer
  //   * size of result-buffer
  //   * filename
  GetPrivateProfileString(
    kPreferencesSectionInIniFile,
    registry_key,
    "",
    input_buffer,
    kSizeOgInputBuffer,
    ini_filename);
  if (input_buffer[0] != '\n') {
    *registry_value = CString(input_buffer);
  }
  /*write_log(debug_preferences(), "[CPreferences] %s = %s\n",
    registry_key, registry_value->GetString());*/
  //MessageBox(0, registry_key, *registry_value, 0);
}

void CPreferences::ReadReg(const LPCTSTR registry_key, double *registry_value) {
  CString value;
  ReadReg(registry_key, &value);
  if (!value.IsEmpty()) {
    *registry_value = atof(value);
  }
  /*write_log(debug_preferences(), "[CPreferences] %s = %s\n",
    registry_key, Number2CString(*registry_value));*/
}

void CPreferences::WriteReg(const LPCTSTR registry_key, const CString &registry_value) {
  // https://msdn.microsoft.com/en-us/library/windows/desktop/ms725501(v=vs.85).aspx
  // Parameters
  //   * ini-file-section
  //   * ini-file-key
  //   * value
  //   * filename
  WritePrivateProfileString(
    kPreferencesSectionInIniFile,
    registry_key,
    registry_value,
    ini_filename);
}

void CPreferences::WriteReg(const LPCTSTR registry_key, const double registry_value) {
  CString str;
  str.Format("%.2f", registry_value);
  WriteReg(registry_key, str);
}

#define ENT CSLock lock(m_critsec);

void CPreferences::SetValue(int index_of_variable, LPCSTR value) {
  ENT
  //!!!!!AssertRange(index_of_variable, 0, k_prefs_last_CString_value);
  prefs_CString_values[index_of_variable] = value;
  WriteReg(k_registry_keys_for_CStrings[index_of_variable], value);
  /*write_log(debug_preferences(), "[CPreferences] %s = %s\n",
    k_registry_keys_for_CStrings[index_of_variable], value);*/
}

void CPreferences::SetValue(int index_of_variable, double value) {
  ENT
  //!!!!!AssertRange(index_of_variable, 0, k_prefs_last_numerical_value);
  prefs_numerical_values[index_of_variable] = value;
  /*//!!!!!if (IsInteger(value)) {
    WriteReg(k_registry_keys_for_numerical_values[index_of_variable], int(value));
  }
  else */{
    WriteReg(k_registry_keys_for_numerical_values[index_of_variable], value);
  }
  /*write_log(debug_preferences(), "[CPreferences] %s = %s\n",
    k_registry_keys_for_numerical_values[index_of_variable], Number2CString(value));*/
}
