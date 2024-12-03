#pragma once
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

#ifdef PREFERENCES_DLL_EXPORTS
#define PREFERENCES_DLL_API __declspec(dllexport)
#else
#define PREFERENCES_DLL_API __declspec(dllimport)
#endif

#include <afxwin.h>
#include "..\..\Shared\CCritSec\CCritSec.h"

// global preferences constants
enum {
  k_AutoConnector_Connect_Never = 0,
  k_AutoConnector_Connect_Permanent = 2,
};

enum {
  k_lazy_scraping_always = 0,
  k_lazy_scraping_myturn = 1,
  k_lazy_scraping_cards = 2,
};

enum {
  k_popup_disabled = 0,
  k_popup_minimize = 1,
  k_popup_kill = 2,
};

enum {
  // Numerical values
  // This includes bools, ints and doubles
  //
  // Boolean values
  k_prefs_debug_preferences,	// Should be the very first, to get proper logging for the rest
  k_prefs_debug_autoconnector,
  k_prefs_debug_autoplayer,
  k_prefs_debug_heartbeat,
  k_prefs_debug_prwin,
  k_prefs_debug_icm,
  k_prefs_debug_occlusionchecker,
  k_prefs_debug_pokertracker,
  k_prefs_debug_rebuy,
  k_prefs_debug_replayframes,
  k_prefs_debug_scraper,
  k_prefs_debug_sessioncounter,
  k_prefs_debug_stableframescounter,
  k_prefs_debug_symbolengine,
  k_prefs_debug_symbolengine_open_ppl,
  k_prefs_debug_memory_usage,
  k_prefs_debug_parser,
  k_prefs_debug_blindlocking,
  k_prefs_debug_memorysymbols,
  k_prefs_debug_tablemap_loader,
  k_prefs_debug_filesystem_monitor,
  k_prefs_debug_handhistory,
  k_prefs_debug_hopper_messages,
  k_prefs_debug_alltherest,
  k_prefs_debug_table_positioner,
  k_prefs_debug_istournament,
  k_prefs_debug_gui,
  k_prefs_debug_table_limits,
  k_prefs_debug_lazy_scraper,
  k_prefs_debug_betsize_adjustment,
  k_prefs_debug_allin_adjustment,
  k_prefs_debug_handreset_detector,
  k_prefs_debug_engine_container,
  k_prefs_debug_enhanced_prwin,
  k_prefs_debug_dll_extension,
  k_prefs_debug_scraper_preprocessor,
  k_prefs_debug_openholdem,
  k_prefs_debug_popup_blocker,
  k_prefs_debug_formula,
  k_prefs_debug_versus,
  k_prefs_debug_auto_trace,
  k_prefs_debug_ast_priority_ordering,
  k_prefs_debug_filenames,
  k_prefs_debug_hand_and_board_expressions,
  k_prefs_debug_tokenizer,
  k_prefs_debug_tablepoints,
  k_prefs_debug_sharedmem,
  k_prefs_debug_singletons,
  k_prefs_debug_autostarter,
  k_prefs_debug_watchdog,
  k_prefs_debug_timers,
  k_prefs_debug_nutfullhouse,
  k_prefs_debug_multiplexer,
  k_prefs_debug_symbol_verification,
  k_prefs_engage_autoplayer,
  k_prefs_restore_position_and_focus,
  k_prefs_use_auto_replay,
  k_prefs_replay_record,
  k_prefs_log_delay_function,
  k_prefs_log_hopper_functions,
  k_prefs_log_icm_functions,
  k_prefs_log_ini_functions,
  k_prefs_log_prwin_functions,
  k_prefs_disable_msgbox,
  k_prefs_validator_stop_on_error,
  k_prefs_validator_use_heuristic_rules,
  k_prefs_validator_shoot_replayframe_on_error,
  k_prefs_validator_enabled,
  k_prefs_use_auto_starter,
  k_prefs_use_auto_shutdown,
  k_prefs_gui_first_visible,
  k_prefs_gui_all_minimized,
  k_prefs_gui_less,
  k_prefs_rebuy_condition_no_cards,
  k_prefs_rebuy_condition_change_in_handnumber,
  k_prefs_rebuy_condition_heuristic_check_for_occlusion,
  k_prefs_configurationcheck_input_settings,
  k_prefs_configurationcheck_font_settings,
  k_prefs_handhistory_generator_enable,
  k_prefs_simple_window_title,
  k_prefs_table_positioner_always_keep_position,
  // Int values
  k_prefs_replay_max_frames,
  k_prefs_frame_delay,
  k_prefs_click_delay,
  k_prefs_swag_delay_1,
  k_prefs_swag_delay_2,
  k_prefs_swag_delay_3,
  k_prefs_scrape_delay,
  k_prefs_chat_min_delay,
  k_prefs_chat_random_delay,
  k_prefs_log_max_logsize,
  k_prefs_formula_x,
  k_prefs_formula_y,
  k_prefs_formula_dx,
  k_prefs_formula_dy,
  k_prefs_main_x,
  k_prefs_main_y,
  k_prefs_autoconnector_when_to_connect,
  k_prefs_rebuy_minimum_time_to_next_try,
  k_prefs_lazy_scraping_when_to_scrape,
  k_prefs_table_positioner_options,
  k_prefs_scraper_zoom,
  k_prefs_popup_blocker,
  // Double values
  // Only on double left, just to check for outdated ICM-config
  k_prefs_icm_prize1,
  //
  // leave this always at the last position
  k_prefs_last_numerical_value,
};

enum {
  // String Values
  k_prefs_pt_ip_addr,
  k_prefs_pt_port,
  k_prefs_pt_dbname,
  k_prefs_pt_user,
  k_prefs_pt_pass,
  k_prefs_rebuy_script,
  k_prefs_window_class_name,
  k_prefs_mutex_name,
  k_prefs_unwanted_scrape,
  // leave this always at the last position
  k_prefs_last_CString_value,
};

class PREFERENCES_DLL_API CPreferences {
public:
  // public functions
  CPreferences();
  ~CPreferences();
public:
  // Has to be called immediately after the log-file has been opened.
  // Must not be called in the constructor (too early for log)
  void LoadPreferences();
public:
  // public accessors
  // We keep one accessor per variable,
  // because it would be inconvenient to have only few accessors
  // and feed them with some contants at countless locations.
  //
  // Autoplayer
  const int frame_delay() { return prefs_numerical_values[k_prefs_frame_delay]; }
  const int click_delay() { return prefs_numerical_values[k_prefs_click_delay]; }
  const int swag_delay_1() { return prefs_numerical_values[k_prefs_swag_delay_1]; }
  const int swag_delay_2() { return prefs_numerical_values[k_prefs_swag_delay_2]; }
  const int swag_delay_3() { return prefs_numerical_values[k_prefs_swag_delay_3]; }
  const bool engage_autoplayer() { return prefs_numerical_values[k_prefs_engage_autoplayer]; }
  const bool restore_position_and_focus() { return prefs_numerical_values[k_prefs_restore_position_and_focus]; }
  const bool use_auto_replay() { return prefs_numerical_values[k_prefs_use_auto_replay]; }
  // Scraper
  const int scrape_delay() { return prefs_numerical_values[k_prefs_scrape_delay]; }
  LPCSTR unwanted_scrape() { return NonEmptyStringValueElseDefault(k_prefs_unwanted_scrape); }
  // Replay Frames
  const int replay_record() { return prefs_numerical_values[k_prefs_replay_record]; }
  const int replay_max_frames() { return prefs_numerical_values[k_prefs_replay_max_frames]; }
  // Poker Tracker
  LPCSTR pt_ip_addr() { return NonEmptyStringValueElseDefault(k_prefs_pt_ip_addr); }
  LPCSTR pt_port()    { return NonEmptyStringValueElseDefault(k_prefs_pt_port); }
  LPCSTR pt_dbname()  { return NonEmptyStringValueElseDefault(k_prefs_pt_dbname); }
  LPCSTR pt_user()    { return NonEmptyStringValueElseDefault(k_prefs_pt_user); }
  LPCSTR pt_pass()    { return NonEmptyStringValueElseDefault(k_prefs_pt_pass); }
  //  PokerChat
  const int chat_min_delay() { return prefs_numerical_values[k_prefs_chat_min_delay]; }
  const int chat_random_delay() { return prefs_numerical_values[k_prefs_chat_random_delay]; }
  // Logging and debugging
  // Most options always enabled for beginners
  // who are "smart" enough to disable logging,
  // but then cry because the log does not show any info.
  const bool disable_msgbox() { return prefs_numerical_values[k_prefs_disable_msgbox]; }
  const int log_max_logsize() { return prefs_numerical_values[k_prefs_log_max_logsize]; }
  const bool log_delay_function() { return prefs_numerical_values[k_prefs_log_delay_function]; }
  const bool log_hopper_functions() { return prefs_numerical_values[k_prefs_log_hopper_functions]; }
  const bool log_icm_functions() { return prefs_numerical_values[k_prefs_log_icm_functions]; }
  const bool log_ini_functions() { return prefs_numerical_values[k_prefs_log_ini_functions]; }
  const bool log_prwin_functions() { return prefs_numerical_values[k_prefs_log_prwin_functions]; }
  // Debugging
  const bool	debug_autoconnector() { return prefs_numerical_values[k_prefs_debug_autoconnector]; }
  const bool	debug_autoplayer() { return prefs_numerical_values[k_prefs_debug_autoplayer]; }
  const bool	debug_heartbeat() { return prefs_numerical_values[k_prefs_debug_heartbeat]; }
  const bool	debug_prwin() { return prefs_numerical_values[k_prefs_debug_prwin]; }
  const bool	debug_icm() { return prefs_numerical_values[k_prefs_debug_icm]; }
  const bool	debug_occlusionchecker() { return prefs_numerical_values[k_prefs_debug_occlusionchecker]; }
  const bool	debug_pokertracker() { return prefs_numerical_values[k_prefs_debug_pokertracker]; }
  const bool	debug_rebuy() { return prefs_numerical_values[k_prefs_debug_rebuy]; }
  const bool	debug_replayframes() { return prefs_numerical_values[k_prefs_debug_replayframes]; }
  const bool	debug_scraper() { return prefs_numerical_values[k_prefs_debug_scraper]; }
  const bool	debug_sessioncounter() { return prefs_numerical_values[k_prefs_debug_sessioncounter]; }
  const bool	debug_stableframescounter() { return prefs_numerical_values[k_prefs_debug_stableframescounter]; }
  const bool	debug_symbolengine() { return prefs_numerical_values[k_prefs_debug_symbolengine]; }
  const bool	debug_symbolengine_open_ppl() { return prefs_numerical_values[k_prefs_debug_symbolengine_open_ppl]; }
  const bool	debug_memory_usage() { return prefs_numerical_values[k_prefs_debug_memory_usage]; }
  const bool	debug_parser() { return prefs_numerical_values[k_prefs_debug_parser]; }
  const bool	debug_blindlocking() { return prefs_numerical_values[k_prefs_debug_blindlocking]; }
  const bool	debug_memorysymbols() { return prefs_numerical_values[k_prefs_debug_memorysymbols]; }
  const bool	debug_tablemap_loader() { return prefs_numerical_values[k_prefs_debug_tablemap_loader]; }
  const bool	debug_filesystem_monitor() { return prefs_numerical_values[k_prefs_debug_filesystem_monitor]; }
  const bool	debug_handhistory() { return prefs_numerical_values[k_prefs_debug_handhistory]; }
  const bool	debug_hopper_messages() { return prefs_numerical_values[k_prefs_debug_hopper_messages]; }
  const bool	debug_alltherest() { return prefs_numerical_values[k_prefs_debug_alltherest]; }
  const bool  debug_table_positioner() { return prefs_numerical_values[k_prefs_debug_table_positioner]; }
  const bool  debug_istournament() { return prefs_numerical_values[k_prefs_debug_istournament]; }
  const bool  debug_gui() { return prefs_numerical_values[k_prefs_debug_gui]; }
  const bool  debug_table_limits() { return prefs_numerical_values[k_prefs_debug_table_limits]; }
  const bool  debug_lazy_scraper() { return prefs_numerical_values[k_prefs_debug_lazy_scraper]; }
  const bool  debug_betsize_adjustment() { return prefs_numerical_values[k_prefs_debug_betsize_adjustment]; }
  const bool  debug_allin_adjustment() { return prefs_numerical_values[k_prefs_debug_allin_adjustment]; }
  const bool  debug_handreset_detector() { return prefs_numerical_values[k_prefs_debug_handreset_detector]; }
  const bool  debug_engine_container() { return prefs_numerical_values[k_prefs_debug_engine_container]; }
  const bool  debug_enhanced_prwin() { return prefs_numerical_values[k_prefs_debug_enhanced_prwin]; }
  const bool  debug_preferences() { return prefs_numerical_values[k_prefs_debug_preferences]; }
  const bool  debug_dll_extension() { return prefs_numerical_values[k_prefs_debug_dll_extension]; }
  const bool  debug_openholdem() { return prefs_numerical_values[k_prefs_debug_openholdem]; }
  const bool  debug_popup_blocker() { return prefs_numerical_values[k_prefs_debug_popup_blocker]; }
  const bool  debug_scraper_preprocessor() { return prefs_numerical_values[k_prefs_debug_scraper_preprocessor]; }
  const bool  debug_formula() { return prefs_numerical_values[k_prefs_debug_formula]; }
  const bool  debug_versus() { return prefs_numerical_values[k_prefs_debug_versus]; }
  const bool  debug_auto_trace() { return prefs_numerical_values[k_prefs_debug_auto_trace]; }
  const bool  debug_ast_priority_ordering() { return prefs_numerical_values[k_prefs_debug_ast_priority_ordering]; }
  const bool  debug_filenames() { return prefs_numerical_values[k_prefs_debug_filenames]; }
  const bool  debug_hand_and_board_expressions() { return prefs_numerical_values[k_prefs_debug_hand_and_board_expressions]; }
  const bool  debug_tokenizer() { return prefs_numerical_values[k_prefs_debug_tokenizer]; }
  const bool  debug_tablepoints() { return prefs_numerical_values[k_prefs_debug_tablepoints]; }
  const bool  debug_sharedmem() { return prefs_numerical_values[k_prefs_debug_sharedmem]; }
  const bool  debug_singletons() { return prefs_numerical_values[k_prefs_debug_singletons]; }
  const bool  debug_autostarter() { return prefs_numerical_values[k_prefs_debug_autostarter]; }
  const bool  debug_watchdog() { return prefs_numerical_values[k_prefs_debug_watchdog]; }
  const bool  debug_timers() { return prefs_numerical_values[k_prefs_debug_timers]; }
  const bool  debug_nutfullhouse() { return prefs_numerical_values[k_prefs_debug_nutfullhouse]; }
  const bool  debug_multiplexer() { return prefs_numerical_values[k_prefs_debug_multiplexer]; }
  const bool  debug_symbol_verification() { return prefs_numerical_values[k_prefs_debug_symbol_verification]; }
  // Main window locations
  const int main_x() { return prefs_numerical_values[k_prefs_main_x]; }
  const int main_y() { return prefs_numerical_values[k_prefs_main_y]; }
  // Formula window locations
  const int formula_x() { return prefs_numerical_values[k_prefs_formula_x]; }
  const int formula_y() { return prefs_numerical_values[k_prefs_formula_y]; }
  const int formula_dx() { return prefs_numerical_values[k_prefs_formula_dx]; }
  const int formula_dy() { return prefs_numerical_values[k_prefs_formula_dy]; }
  // Validator
  const int validator_enabled() { return prefs_numerical_values[k_prefs_validator_enabled]; }
  const bool validator_stop_on_error() { return prefs_numerical_values[k_prefs_validator_stop_on_error]; }
  const bool validator_use_heuristic_rules() { return prefs_numerical_values[k_prefs_validator_use_heuristic_rules]; }
  const bool validator_shoot_replayframe_on_error() { return prefs_numerical_values[k_prefs_validator_shoot_replayframe_on_error]; }
  // Auto-starter
  const bool use_auto_starter() { return prefs_numerical_values[k_prefs_use_auto_starter]; }
  const bool use_auto_shutdown() { return prefs_numerical_values[k_prefs_use_auto_shutdown]; }
  // Auto-connector
  const int autoconnector_when_to_connect() { return prefs_numerical_values[k_prefs_autoconnector_when_to_connect]; }
  // GUI
  const bool gui_first_visible() { return prefs_numerical_values[k_prefs_gui_first_visible]; }
  const bool gui_all_minimized() { return prefs_numerical_values[k_prefs_gui_all_minimized]; }
  const bool gui_less() { return prefs_numerical_values[k_prefs_gui_less]; }
  // Rebuy
  const bool rebuy_condition_no_cards() { return prefs_numerical_values[k_prefs_rebuy_condition_no_cards]; }
  const bool rebuy_condition_change_in_handnumber() { return prefs_numerical_values[k_prefs_rebuy_condition_change_in_handnumber]; }
  const bool rebuy_condition_heuristic_check_for_occlusion() { return prefs_numerical_values[k_prefs_rebuy_condition_heuristic_check_for_occlusion]; }
  const int  rebuy_minimum_time_to_next_try() { return prefs_numerical_values[k_prefs_rebuy_minimum_time_to_next_try]; }
  LPCSTR rebuy_script() { return NonEmptyStringValueElseDefault(k_prefs_rebuy_script); }
  // Configuration check
  const bool configurationcheck_input_settings() { return prefs_numerical_values[k_prefs_configurationcheck_input_settings]; }
  const bool configurationcheck_font_settings() { return prefs_numerical_values[k_prefs_configurationcheck_font_settings]; }
  // Lazy scraping
  const int lazy_scraping_when_to_scrape() { return prefs_numerical_values[k_prefs_lazy_scraping_when_to_scrape]; }
  // Handhistory generator
  const bool handhistory_generator_enable() { return prefs_numerical_values[k_prefs_handhistory_generator_enable]; }
  // Table Positioner
  const int table_positioner_options() { return prefs_numerical_values[k_prefs_table_positioner_options]; }
  const bool table_positioner_always_keep_position() { return prefs_numerical_values[k_prefs_table_positioner_always_keep_position]; }
  // Misc
  const int scraper_zoom() { return prefs_numerical_values[k_prefs_scraper_zoom]; }
  // Popup blocker
  const int popup_blocker() { return prefs_numerical_values[k_prefs_popup_blocker]; }
  // Obscure
  LPCSTR window_class_name() { return NonEmptyStringValueElseDefault(k_prefs_window_class_name); }
  LPCSTR mutex_name() { return NonEmptyStringValueElseDefault(k_prefs_mutex_name); }
  const bool simple_window_title() { return prefs_numerical_values[k_prefs_simple_window_title]; }
public:
  // public mutators
  void SetValue(int index_of_variable, LPCSTR value);
  // double has to handle ints and bools too,
  // otherwise we get an "error c2668  ambiguous call to overloaded function"
  // http://msdn.microsoft.com/en-us/library/da60x087.aspx
  void SetValue(int index_of_variable, double value);
private:
  void CheckForOutdatedICMConfig();
private:
  CString DefaultStringValues(int index);
  CString NonEmptyStringValueElseDefault(int index);
private:
  // private variables - use public accessors and public mutators to address these
  CString	prefs_CString_values[k_prefs_last_CString_value];
  double  prefs_numerical_values[k_prefs_last_numerical_value];
private:
  // private functions and variables - not available via accessors or mutators
  void ReadPreferences();
  void InitDefaults();
  void ReadReg(const LPCTSTR registry_key, CString *registry_value);
  void ReadReg(const LPCTSTR registry_key, double *registry_value);
  void WriteReg(const LPCTSTR registry_key, const CString &registry_value);
  void WriteReg(const LPCTSTR registry_key, const double registry_value);
private:
  CCritSec m_critsec;
  const char* kPreferencesSectionInIniFile = "Preferences";;
  CString ini_filename;
};