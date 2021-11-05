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

#include "stdafx.h"
#include "Singletons.h"

#include "CCasinoInterface.h"
#include "CAutoConnector.h"
#include "CAutoplayer.h"
#include "CAutoplayerFunctions.h"
#include "CAutoplayerTrace.h"
#include "CConfigurationCheck.h"
#include "CDebugTab.h"
#include "CEngineContainer.h"
#include "CFileSystemMonitor.h"
#include "CFormulaParser.h"
#include "CHandresetDetector.h"
#include "CHeartbeatThread.h"
#include "CIteratorThread.h"
#include "CLazyScraper.h"
#include "CMemoryPool.h"
#include "COcclusioncheck.h"
#include "COpenHoldemTitle.h"
#include "CParserSymbolTable.h"
#include "CPokerTrackerThread.h"
#include "CPopupHandler.h"

#include "CRebuyManagement.h"
#include "CScraper.h"
#include "CSessionCounter.h"
#include "CSharedMem.h"
#include "CStableFramesCounter.h"
#include "CStringMatch.h"
#include "CSymbolEngineTableLimits.h"
#include "CTableMaploader.h"
#include "CTablePositioner.h"
#include "CTableState.h"
#include "CTableTitle.h"
#include "CTitleEvaluator.h"
#include "CValidator.h"
#include "CVersionInfo.h"
#include "CWatchdog.h"
#include "CWhiteInfoBox.h"
#include "RtaWindow.h"

void InstantiateAllSingletons() {
  // Instantiation of all singletons, except session-counter.
  // session-counter has to be done do earlier, as it is needed 
  // to create the log-file, which might be needed before this point.
  // This function gets executed exactly once at startup.
  // So the global class-pointers have to be NULL.
  write_log(Preferences()->debug_singletons(), "[Singletons] Going to create m emory pools\n");
  CreateMemoryPools();
  write_log(Preferences()->debug_singletons(), "[Singletons] Going to create CWatchdog\n");
  assert(!p_watchdog);
  p_watchdog = new CWatchdog;
  write_log(Preferences()->debug_singletons(), "[Singletons] Going to create CTableTitle\n");
  assert(!p_table_title);
  p_table_title = new CTableTitle;
  write_log(Preferences()->debug_singletons(), "[Singletons] Going to create CStringMatch\n");
  assert(!p_string_match);
  p_string_match = new CStringMatch;
  write_log(Preferences()->debug_singletons(), "[Singletons] Going to create CAutoplayerTrace\n");
  assert(!p_autoplayer_trace);
  p_autoplayer_trace = new CAutoplayerTrace;
  write_log(Preferences()->debug_singletons(), "[Singletons] Going to create CTableState\n");
  assert(!p_table_state);
  p_table_state = new CTableState;
  write_log(Preferences()->debug_singletons(), "[Singletons] Going to create CHandresetDetector\n");
  assert(!p_handreset_detector);
  p_handreset_detector = new CHandresetDetector;
  write_log(Preferences()->debug_singletons(), "[Singletons] Going to create CConfigurationCheck\n");
  assert(!p_configurationcheck);
  p_configurationcheck = new CConfigurationCheck;
  write_log(Preferences()->debug_singletons(), "[Singletons] Going to create CAutoplayerFunctions\n");
  assert(!p_autoplayer_functions);
  p_autoplayer_functions = new CAutoplayerFunctions;
  write_log(Preferences()->debug_singletons(), "[Singletons] Going to create CSharedMem\n");
  assert(!p_sharedmem);
  p_sharedmem = new CSharedMem;
  write_log(Preferences()->debug_singletons(), "[Singletons] Going to create CStableFramesCounter\n");
  assert(!p_stableframescounter);
  p_stableframescounter = new CStableFramesCounter;                                                                                                                                                         
  write_log(Preferences()->debug_singletons(), "[Singletons] Going to create CTitleEvaluator\n");
  assert(!p_title_evaluator);
  p_title_evaluator = new CTitleEvaluator;
  write_log(Preferences()->debug_singletons(), "[Singletons] Going to create CScraper\n");
  assert(!p_scraper); 
  p_scraper = new CScraper;
  write_log(Preferences()->debug_singletons(), "[Singletons] Going to create CLazyScraper\n");
  assert(!p_lazyscraper);
  p_lazyscraper = new CLazyScraper;
  write_log(Preferences()->debug_singletons(), "[Singletons] Going to create CTablemap\n");
  assert(!p_tablemap);
  p_tablemap = new CTablemap;
  write_log(Preferences()->debug_singletons(), "[Singletons] Going to create CTablemapAccess\n");
  assert(!p_tablemap_access);
  p_tablemap_access = new CTablemapAccess;
  write_log(Preferences()->debug_singletons(), "[Singletons] Going to create CParserSymbolTable\n");
  assert(!p_parser_symbol_table);
  p_parser_symbol_table = new CParserSymbolTable;
  write_log(Preferences()->debug_singletons(), "[Singletons] Going to create CFormulaParser\n");
  assert(!p_formula_parser);
  p_formula_parser = new CFormulaParser;
  write_log(Preferences()->debug_singletons(), "[Singletons] Going to create CDebugTab\n");
  assert(p_debug_tab == NULL);
  p_debug_tab = new CDebugTab;
  write_log(Preferences()->debug_singletons(), "[Singletons] Going to create CAutoplayer\n");
  assert(!p_autoplayer);
  p_autoplayer = new CAutoplayer();
  write_log(Preferences()->debug_singletons(), "[Singletons] Going to create CPokerTrackerThread\n");
  assert(!p_pokertracker_thread);
  p_pokertracker_thread = new CPokerTrackerThread;
  write_log(Preferences()->debug_singletons(), "[Singletons] Going to create CValidator\n");
  assert(!p_validator);
  p_validator = new CValidator;
  write_log(Preferences()->debug_singletons(), "[Singletons] Going to create CTablePositioner\n");
  assert(!p_table_positioner);
  p_table_positioner = new CTablePositioner;
  write_log(Preferences()->debug_singletons(), "[Singletons] Going to create CFileSystemMonitor\n");
  assert(!p_filesystem_monitor);
  p_filesystem_monitor = new CFileSystemMonitor;
  write_log(Preferences()->debug_singletons(), "[Singletons] Going to create CTableMapLoader\n");
  assert(!p_tablemap_loader);
  p_tablemap_loader = new CTableMapLoader;
  write_log(Preferences()->debug_singletons(), "[Singletons] Going to create CVersionInfo\n");
  assert(!p_version_info);
  p_version_info = new CVersionInfo;
  write_log(Preferences()->debug_singletons(), "[Singletons] Going to create CPopupHandler\n");
  assert(!p_popup_handler);
  p_popup_handler = new CPopupHandler;
  write_log(Preferences()->debug_singletons(), "[Singletons] Going to create CAutoConnector\n");
  assert(!p_autoconnector);
  p_autoconnector = new CAutoConnector;
  write_log(Preferences()->debug_singletons(), "[Singletons] Going to create COpenHoldemTitle\n");
  assert(!p_openholdem_title);
  p_openholdem_title = new COpenHoldemTitle;
  write_log(Preferences()->debug_singletons(), "[Singletons] Going to create CWhiteInfoBox\n");
  assert(!p_white_info_box);
  p_white_info_box = new CWhiteInfoBox;
  write_log(Preferences()->debug_singletons(), "[Singletons] Going to create CRtaWindow\n");
  assert(!p_rta_window);
  p_rta_window = new CRtaWindow;
  write_log(Preferences()->debug_singletons(), "[Singletons] Going to create CEngineContainer\n");
  assert(!p_engine_container);
  p_engine_container = new CEngineContainer;
  p_engine_container->CreateSymbolEngines();
  write_log(Preferences()->debug_singletons(), "[Singletons] Going to create CRebuyManagement\n");
  assert(!p_rebuymanagement); 
  p_rebuymanagement = new CRebuyManagement;
  write_log(Preferences()->debug_singletons(), "[Singletons] Going to create COcclusionCheck\n");
  assert(!p_occlusioncheck);
  p_occlusioncheck = new COcclusionCheck;
  write_log(Preferences()->debug_singletons(), "[Singletons] Going to create CCasinoInterface\n");
  assert(!p_casino_interface);
  p_casino_interface = new CCasinoInterface;
  write_log(Preferences()->debug_singletons(), "[Singletons] All singletons created.\n");
}

bool all_threads_stopped = false;

// To be executed first,
// as these threads might access some variables.
void StopThreads() {
	write_log(Preferences()->debug_singletons(), "[Singletons] StopThreads()\n");
	if (p_heartbeat_thread)	{
		write_log(Preferences()->debug_singletons(), "[Singletons] Deleting heartbeat-thread\n");
		delete p_heartbeat_thread;
		p_heartbeat_thread = NULL;
	}
	if (p_iterator_thread) {
		write_log(Preferences()->debug_singletons(), "[Singletons] Deleting iterator-thread\n");
		delete p_iterator_thread;
		p_iterator_thread = NULL;
	}
	if (p_pokertracker_thread) { 
		write_log(Preferences()->debug_singletons(), "[Singletons] Deleting PokerTracker-thread\n");
		p_pokertracker_thread->~CPokerTrackerThread(); 
		p_pokertracker_thread = NULL; 
	}
	all_threads_stopped = true;
}

#define DELETE_AND_CLEAR(object_pointer) if (object_pointer)	{ delete object_pointer; object_pointer = NULL; }

void DeleteAllSingletons() {
  // First all timers and threads have to be stopped, 
  // then all singletons can (have to) be deleted.
  // StopThreads gets called by CMainFrame::DestroyWindow()
  // DeleteAllSingletons() by COpenHoldemApp::ExitInstance()
  // Correct order should be guaranteed, because of
  //   "At the time ExitInstance is called, the main window no longer exists"
  //   (http://computer-programming-forum.com/82-mfc/7ad0828fdb127d7b.htm)
  // AnyWay: we make sure...
  if (!all_threads_stopped) {
    // Explicit check to check potential pronlems 
    // that don't happen in debug-mode.
    write_log(Preferences()->debug_singletons(), 
      "[Singletons] ERROR: threads not stopped.\n");
    assert(all_threads_stopped);
  }
  // Global instances.
  // Releasing in reverse order should be good,
  // but we have to be careful, as sometimes we do some work in the destructors,
  // that depends on other classes, e.g. the destructor of the autoconnector
  // needs its session_id (CSessionCounter).
  //
  write_log(Preferences()->debug_singletons(), "[Singletons] Deleting casino interface\n");
  DELETE_AND_CLEAR(p_casino_interface)
  write_log(Preferences()->debug_singletons(), "[Singletons] Deleting occlusion check\n");
  DELETE_AND_CLEAR(p_occlusioncheck)
  write_log(Preferences()->debug_singletons(), "[Singletons] Deleting rebuy management\n");
  DELETE_AND_CLEAR(p_rebuymanagement)
  write_log(Preferences()->debug_singletons(), "[Singletons] Deleting engine container\n");
  DELETE_AND_CLEAR(p_engine_container)
  write_log(Preferences()->debug_singletons(), "[Singletons] Deleting white info box\n");
  DELETE_AND_CLEAR(p_white_info_box)
	  write_log(Preferences()->debug_singletons(), "[Singletons] Deleting rta window\n");
  DELETE_AND_CLEAR(p_rta_window)
  write_log(Preferences()->debug_singletons(), "[Singletons] Deleting OpenHoldem title\n");
  DELETE_AND_CLEAR(p_openholdem_title)
  write_log(Preferences()->debug_singletons(), "[Singletons] Deleting popup handler\n");
  DELETE_AND_CLEAR(p_popup_handler)
  write_log(Preferences()->debug_singletons(), "[Singletons] Deleting autoconnector\n");
  DELETE_AND_CLEAR(p_autoconnector)
  write_log(Preferences()->debug_singletons(), "[Singletons] Deleting version_info\n");
  DELETE_AND_CLEAR(p_version_info)
  write_log(Preferences()->debug_singletons(), "[Singletons] Deleting tablemap loader\n");
  DELETE_AND_CLEAR(p_tablemap_loader)
  write_log(Preferences()->debug_singletons(), "[Singletons] Deleting 01\n");
  DELETE_AND_CLEAR(p_filesystem_monitor)
  write_log(Preferences()->debug_singletons(), "[Singletons] Deleting 02\n");
  DELETE_AND_CLEAR(p_table_positioner)
  write_log(Preferences()->debug_singletons(), "[Singletons] Deleting 03\n");
  DELETE_AND_CLEAR(p_validator)
  write_log(Preferences()->debug_singletons(), "[Singletons] Deleting 07\n");
  DELETE_AND_CLEAR(p_autoplayer)
  write_log(Preferences()->debug_singletons(), "[Singletons] Deleting 08\n");
  DELETE_AND_CLEAR(p_formula_parser)
  write_log(Preferences()->debug_singletons(), "[Singletons] Deleting 09\n");
  DELETE_AND_CLEAR(p_parser_symbol_table)
  write_log(Preferences()->debug_singletons(), "[Singletons] Deleting 10\n");
  //!!!DELETE_AND_CLEAR(p_debug_tab)
  write_log(Preferences()->debug_singletons(), "[Singletons] Deleting 11\n");
  DELETE_AND_CLEAR(p_tablemap_access)
  write_log(Preferences()->debug_singletons(), "[Singletons] Deleting 12\n");
  DELETE_AND_CLEAR(p_tablemap)
  write_log(Preferences()->debug_singletons(), "[Singletons] Deleting 13\n");
  DELETE_AND_CLEAR(p_lazyscraper)
  write_log(Preferences()->debug_singletons(), "[Singletons] Deleting 14\n");
  DELETE_AND_CLEAR(p_scraper)
  write_log(Preferences()->debug_singletons(), "[Singletons] Deleting 15\n");
  DELETE_AND_CLEAR(p_title_evaluator)
  write_log(Preferences()->debug_singletons(), "[Singletons] Deleting 16\n");
  DELETE_AND_CLEAR(p_stableframescounter)
  write_log(Preferences()->debug_singletons(), "[Singletons] Deleting 17\n");
  DELETE_AND_CLEAR(p_sharedmem)
  write_log(Preferences()->debug_singletons(), "[Singletons] Deleting 19\n");
  DELETE_AND_CLEAR(p_autoplayer_functions)
  write_log(Preferences()->debug_singletons(), "[Singletons] Deleting 20\n");
  DELETE_AND_CLEAR(p_configurationcheck)
  write_log(Preferences()->debug_singletons(), "[Singletons] Deleting 21\n");
  DELETE_AND_CLEAR(p_handreset_detector)
  write_log(Preferences()->debug_singletons(), "[Singletons] Deleting 22\n");
  DELETE_AND_CLEAR(p_autoplayer_trace)
  write_log(Preferences()->debug_singletons(), "[Singletons] Deleting 23\n");
  DELETE_AND_CLEAR(p_table_state)
  write_log(Preferences()->debug_singletons(), "[Singletons] Deleting 24\n");
  DELETE_AND_CLEAR(p_string_match)
  write_log(Preferences()->debug_singletons(), "[Singletons] Deleting 26\n");
  DELETE_AND_CLEAR(p_table_title)
  write_log(Preferences()->debug_singletons(), "[Singletons] Deleting 28\n");
  DELETE_AND_CLEAR(p_watchdog)
  // Session counter at the very end,
  // as it lots of other modules depend on it,
  // but it doesn't depend on anything else.
  write_log(Preferences()->debug_singletons(), "[Singletons] Deleting CSessionCounter\n");
  DELETE_AND_CLEAR(p_sessioncounter)
  write_log(Preferences()->debug_singletons(), "[Singletons] Deleting memory pools\n");
  DeleteAllMemoryPools();
  write_log(Preferences()->debug_singletons(), "[Singletons] All singletons successfully deleted\n");
}
  
  


