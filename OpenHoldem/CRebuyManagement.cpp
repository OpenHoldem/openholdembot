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
#include <atlstr.h>
#include <time.h>
#include "CAutoconnector.h"
#include "CAutoplayerFunctions.h"
#include "CEngineContainer.h"
#include "CFunctionCollection.h"
#include "CHandresetDetector.h"
#include "COcclusionCheck.h"

#include "CRebuyManagement.h"
#include "CScraper.h"
#include "CSymbolEngineChipAmounts.h"
#include "CSymbolEngineUserchair.h"
#include "CTablemap/CTablemap.h"
#include "CSymbolEngineTableLimits.h"
#include "CTableState.h"
#include "WindowFunctions/window_functions.h"

CRebuyManagement *p_rebuymanagement = NULL;

CRebuyManagement::CRebuyManagement() {
	// Init time of last rebuy in a reasonable way at startup.
	time(&RebuyLastTime);
	PreviousRebuyHandNumber = "";
}

CRebuyManagement::~CRebuyManagement() {
}


bool CRebuyManagement::MinimumDelayElapsed() {
	unsigned int MinimumTimeDifference = Preferences()->rebuy_minimum_time_to_next_try();
	// Make sure, we don't try to rebuy too often in a short time
	time(&CurrentTime);
	double RebuyTimeDifference = difftime(CurrentTime, RebuyLastTime);
  write_log(Preferences()->debug_rebuy(), "[CRebuyManagement] Time since last rebuy %.2f\n", RebuyTimeDifference);
	if (RebuyTimeDifference < MinimumTimeDifference) {
		write_log(Preferences()->debug_rebuy(), "[CRebuyManagement] MinimumDelayElapsed(): false\n");
		return false;
	}
	return true;
}

bool CRebuyManagement::ChangeInHandNumber() {
  if (!Preferences()->rebuy_condition_change_in_handnumber()) {
    return true;
  }
  write_log(Preferences()->debug_rebuy(), "[CRebuyManagement] handnumber %s\n", PreviousRebuyHandNumber);
  write_log(Preferences()->debug_rebuy(), "[CRebuyManagement] handnumber %s\n", p_handreset_detector->GetHandNumber());
  if (p_handreset_detector->GetHandNumber() > PreviousRebuyHandNumber) {
    write_log(Preferences()->debug_rebuy(), "[CRebuyManagement] ChangeInHandNumber(): true\n");
		return true;
	}
	write_log(Preferences()->debug_rebuy(), "[CRebuyManagement] ChangeInHandNumber(): false\n");
	return false;
}

bool CRebuyManagement::NoCards(){
	if (!Preferences()->rebuy_condition_no_cards()) {
		return true;
	}
	int UserChair = p_engine_container->symbol_engine_userchair()->userchair();
	if ((UserChair < 0) || (UserChair > 9)) {
		// "No cards", but not even seated.
		// We should never get into that situation,
		// as the autoplayer can't get enabled without a userchair.
		// If all goes wrong, the rebuy-script has to handle that case.
    write_log(Preferences()->debug_rebuy(), "[CRebuyManagement] No_Cards: true, as userchair not detected\n");
		return true;
	}
	if (!p_table_state->User()->HasKnownCards()) 	{
    write_log(Preferences()->debug_rebuy(), "[CRebuyManagement] No_Cards: true.\n");
		return true;
	}
	// We hold cards. No good time to rebuy,
	// if we play at a real casino.
	write_log(Preferences()->debug_rebuy(), "[CRebuyManagement] No_Cards: false: we hold cards.\n");	
	return false;
}

bool CRebuyManagement::OcclusionCheck() {
	if (!Preferences()->rebuy_condition_heuristic_check_for_occlusion()) {
		return true;
	}	else if (p_occlusioncheck->UserBalanceOccluded()) {
		write_log(Preferences()->debug_rebuy(), "[CRebuyManagement] OcclusionCheck: false (occluded)\n");
		return false;
	}
  write_log(Preferences()->debug_rebuy(), "[CRebuyManagement] OcclusionCheck: true (not occluded)\n");
	return true;
}

bool CRebuyManagement::RebuyPossible() {
	if (MinimumDelayElapsed()
      && ChangeInHandNumber()
		  && NoCards()
		  && OcclusionCheck()) {
		write_log(Preferences()->debug_rebuy(), "[CRebuyManagement] RebuyPossible: true\n");
		return true;
	}	else {
		write_log(Preferences()->debug_rebuy(), "[CRebuyManagement] RebuyPossible: false\n");
		return false;
	}
}

bool CRebuyManagement::TryToRebuy() {
	write_log(Preferences()->debug_rebuy(), "[CRebuyManagement] TryToRebuy()\n");
	if (RebuyPossible()) {
		RebuyLastTime = CurrentTime;		
		PreviousRebuyHandNumber = p_handreset_detector->GetHandNumber();
		ExecuteRebuyScript();
    return true;
	}
  return false;
}	

void CRebuyManagement::ExecuteRebuyScript() {
	// Call the external rebuy script.
	//
	// CAUTION! DO NOT USE THIS FUNCTION DIRECTLY!
	// It has to be protected by a mutex.
	// We assume, the autoplayer does that.
	//
	// Build command-line-options for rebuy-script
	write_log(Preferences()->debug_rebuy(), "[CRebuyManagement] ExecuteRebuyScript\n");
	CString Casino;
	if (p_tablemap->s$()->find("sitename") != p_tablemap->s$()->end()) 	{
		Casino = p_tablemap->s$()->find("sitename")->second.text.GetString();
	}	else {
		Casino = "Undefined";
	}
	HWND WindowHandleOfThePokerTable = p_autoconnector->attached_hwnd();
	double SmallBlind = p_engine_container->symbol_engine_tablelimits()->sblind();
	double BigBlind = p_engine_container->symbol_engine_tablelimits()->bblind();
	double BigBet = p_engine_container->symbol_engine_tablelimits()->bigbet();
	int UserChair = p_engine_container->symbol_engine_userchair()->userchair();
	double Balance = p_table_state->User()->_balance.GetValue();
	double TargetAmount = p_function_collection->Evaluate(k_standard_function_names[k_hopper_function_rebuy]);
	CString RebuyScript = Preferences()->rebuy_script();
	CString CommandLine;
	CommandLine.Format(CString("%s %s %u %d %.2f %.2f %.2f %.2f %.2f"), 
		RebuyScript, Casino, WindowHandleOfThePokerTable, 
		UserChair, Balance, SmallBlind, BigBlind, BigBet, TargetAmount);
	// For some docu on "CreateProcess" see:
	// http://pheatt.emporia.edu/courses/2005/cs260s05/hand39/hand39.htm
	// http://msdn.microsoft.com/en-us/library/aa908775.aspx
	// http://www.codeproject.com/KB/threads/CreateProcessEx.aspx
	STARTUPINFO StartupInfo;
    PROCESS_INFORMATION ProcessInfo; 
	memset(&StartupInfo, 0, sizeof(StartupInfo));
	memset(&ProcessInfo, 0, sizeof(ProcessInfo));
	StartupInfo.cb = sizeof(StartupInfo); 
  write_log(Preferences()->debug_rebuy(), "[CRebuyManagement] RebuyScript %s\n", RebuyScript);
  write_log(Preferences()->debug_rebuy(), "[CRebuyManagement] Casino %s\n", Casino);
  write_log(Preferences()->debug_rebuy(), "[CRebuyManagement] Window %u\n", WindowHandleOfThePokerTable);
  write_log(Preferences()->debug_rebuy(), "[CRebuyManagement] Userchair %d\n", UserChair);
  write_log(Preferences()->debug_rebuy(), "[CRebuyManagement] Balance %.2f\n", Balance);
  write_log(Preferences()->debug_rebuy(), "[CRebuyManagement] Smallblind  %.2f\n", SmallBlind);
  write_log(Preferences()->debug_rebuy(), "[CRebuyManagement] Bigblind  %.2f\n", BigBlind);
  write_log(Preferences()->debug_rebuy(), "[CRebuyManagement] Bigbet  %.2f\n", BigBet);
  write_log(Preferences()->debug_rebuy(), "[CRebuyManagement] Target-amount  %.2f\n", TargetAmount);
	write_log(Preferences()->debug_rebuy(), "[CRebuyManagement] command line: %s\n", CommandLine);
	if(CreateProcess(NULL, CommandLine.GetBuffer(), NULL, 
		  false, 0, NULL, NULL, 0, &StartupInfo, &ProcessInfo)) {
		// Docu for WaitForSingleObject:
		// http://msdn.microsoft.com/en-us/library/ms687032(VS.85).aspx
		// It seems, none of the exitcodes is relevant for us.
		//
		// Wait for termination of the rebuy-script, if necessary forever,
		// as we can't release the (autoplayer)mutex as long as the script is running.
		int ExitCode = WaitForSingleObject(ProcessInfo.hProcess, INFINITE);
	}	else {
		CString ErrorMessage = CString("Could not execute rebuy-script: ") + CString(RebuyScript) + "\n";
		write_log(Preferences()->debug_rebuy(), ErrorMessage.GetBuffer());
		MessageBox_Error_Warning(ErrorMessage.GetBuffer());
	}
}