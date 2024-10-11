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
#include "CAutoplayerFunctions.h"

#include <assert.h>
#include "CBetroundCalculator.h"
#include "CEngineContainer.h"
#include "CFunctionCollection.h"
#include "CSymbolEngineAutoplayer.h"
#include "CSymbolEngineChipAmounts.h"
#include "CSymbolEngineOpenPPL.h"
#include "CSymbolEngineTableLimits.h"
#include "CSymbolEngineUserchair.h"


#include "..\DLLs\StringFunctions_DLL\string_functions.h"

// TODO make it a symbol-engine?
// Call it!

CAutoplayerFunctions *p_autoplayer_functions = NULL;

CAutoplayerFunctions::CAutoplayerFunctions() {
}

double CAutoplayerFunctions::GetAutoplayerFunctionValue(const int function_code) {
	assert(function_code >= 0);
	assert(function_code < k_number_of_standard_functions);
  return p_function_collection->Evaluate(k_standard_function_names[function_code]);
}

void CAutoplayerFunctions::CalcPrimaryFormulas() {
  write_log(Preferences()->debug_formula(), "[CAutoplayerFunctions] CalcPrimaryFormulas()\n");
  assert(p_engine_container->symbol_engine_autoplayer()->isfinalanswer());
  if (p_function_collection->IsOpenPPLProfile()) {
    CalcPrimaryFormulasOpenPPL();
    CalculateOpenPPLBackupActions();
    return;
  }
  // Otherwiese: OH-script
  assert(!p_function_collection->IsOpenPPLProfile());
  CalcPrimaryFormulasOHScript();
}

void CAutoplayerFunctions::CalcPrimaryFormulasOHScript() {
  write_log(Preferences()->debug_formula(), "[CAutoplayerFunctions] CalcPrimaryFormulasOHScript()\n");
	for (int i=k_autoplayer_function_beep; i<=k_autoplayer_function_fold; i++) {
		double result = p_function_collection->Evaluate(k_standard_function_names[i], kAlwaysLogAutoplayerFunctions);
		write_log(Preferences()->debug_formula(), "[CAutoplayerFunctions] Primary formulas; %s: %f\n", 
			k_standard_function_names[i], result);
	}
}

void CAutoplayerFunctions::CalcPrimaryFormulasOpenPPL() {
 write_log(Preferences()->debug_formula(), "[CAutoplayerFunctions] CalcPrimaryFormulasOpenPPL()\n");
  // Now do the main evaluation
  int betround = p_betround_calculator->betround();
	if (betround < kBetroundPreflop || betround > kBetroundRiver) {
    write_log(Preferences()->debug_formula(), 
      "[CAutoplayerFunctions] Betround out of range. Can't calculate OpenPPL-decision\n");
    return;
  }
  double decision = p_function_collection->Evaluate(k_OpenPPL_function_names[betround], 
    kAlwaysLogAutoplayerFunctions);
  write_log(Preferences()->debug_formula(), 
    "[CAutoplayerFunctions] Decision (non-translated) = %.2f\n", decision);
  TranslateOpenPPLDecisionToAutoplayerFunctions(decision);
}

void CAutoplayerFunctions::CheckIfDecisionMatchesElementaryAction(int decision, int action) {
  if (action == k_autoplayer_function_betsize) {
    // Not an elementary action
    return;
  }
  CString action_name;
  // Translate aqutoplayer-constant to OPPL-action-name
  switch (action) {
    case k_autoplayer_function_beep:
      action_name = "Beep";
      break;
    case k_autoplayer_function_allin:
      action_name = "RaiseMax";
      break;
    case k_autoplayer_function_betpot_2_1:
      action_name = "RaiseTwoPot";
      break;
    case k_autoplayer_function_betpot_1_1:
      action_name = "RaisePot";
      break;
    case k_autoplayer_function_betpot_3_4:
      action_name = "RaiseThreeFourthPot";
      break;
    case k_autoplayer_function_betpot_2_3:
      action_name = "RaiseTwoThirdPot";
      break;
    case k_autoplayer_function_betpot_1_2:
      action_name = "RaiseHalfPot";
      break;
    case k_autoplayer_function_betpot_1_3:
      action_name = "RaiseThirdPot";
      break;
    case k_autoplayer_function_betpot_1_4:
      action_name = "RaiseFourthPot";
      break;
    // k_autoplayer_function_betsize already handled above
    // Not an elementary action -> return
    case k_autoplayer_function_raise:
      action_name = "Raise";
      break;
    case k_autoplayer_function_call:
      action_name = "Call";
      break;
    case k_autoplayer_function_check:
      action_name = "Check";
      break;
    case k_autoplayer_function_fold:
      action_name = "Fold";
      break;
    default:
      assert (kThisMustNotHappen);
  }
  double open_ppl_action_code = p_function_collection->Evaluate(action_name);
  write_log(Preferences()->debug_symbolengine_open_ppl(),
    "[CAutoplayerFunctions] Checking if decision %.3f matches action %s (%.3f)\n",
    decision, action_name, open_ppl_action_code);
  if (decision == open_ppl_action_code) {
    write_log(Preferences()->debug_symbolengine_open_ppl(),
    "[CAutoplayerFunctions] Decision matches action\n");
    p_function_collection->SetAutoplayerFunctionValue(action, true);
  }
}

void CAutoplayerFunctions::TranslateOpenPPLDecisionToAutoplayerFunctions(double decision) {
  write_log(Preferences()->debug_formula(), "[CAutoplayerFunctions] TranslateOpenPPLDecisionToAutoplayerFunctions()\n");
  // Positive values mean:  betsizes (in big-blinds)
  // Small negative values: percentaged potsized bets
  // Large negative values: action constants
  if (decision > 0) {
    // OpenHoldem uses f$betsize in dollars
    assert(p_engine_container->symbol_engine_tablelimits() != NULL);
    double betsize = decision * p_engine_container->symbol_engine_tablelimits()->bblind();
    p_function_collection->SetAutoplayerFunctionValue(k_autoplayer_function_betsize, 
      betsize);
  } else if (IsPercentagePotsizeBet(decision)) {
    double betsize = BetSizeForPercentagedPotsizeBet(decision);
    p_function_collection->SetAutoplayerFunctionValue(k_autoplayer_function_betsize, 
      betsize);
  } else if (decision < -1000) {
    // Large negative values: action constants
    for (int action = k_autoplayer_function_beep; action <= k_autoplayer_function_fold; ++action) {
      CheckIfDecisionMatchesElementaryAction(decision, action);
    }
  } else {
    // This can only be undefined == 0.0
    assert(decision == kUndefinedZero);
    write_log(Preferences()->debug_symbolengine_open_ppl(),
      "[CAutoplayerFunctions] OpenPPL-decision undefined. Defaulting to check/fold.\n");
    p_function_collection->SetAutoplayerFunctionValue(k_autoplayer_function_check, true);
    p_function_collection->SetAutoplayerFunctionValue(k_autoplayer_function_fold, true);
  }
}

void CAutoplayerFunctions::CalculateSingleOpenPPLBackupAction(
    int potential_action, int potential_backup) {
  double action_value = p_function_collection->EvaluateAutoplayerFunction(
    potential_action);
  write_log(Preferences()->debug_formula(), 
    "[CAutoplayerFunctions] %s -> %.3f\n",
    k_standard_function_names[potential_action], 
    action_value);
  if (action_value) {
    write_log(Preferences()->debug_formula(), 
      "[CAutoplayerFunctions] Backup action added: %s -> %s\n",
      k_standard_function_names[potential_action], 
      k_standard_function_names[potential_backup]);
    p_function_collection->SetAutoplayerFunctionValue(
      potential_backup, true);
  }
}

bool CAutoplayerFunctions::IsFoldAllinSituation() {
  // Determine this situation by button-states
  // and not by easily misscraped bets and balances.
  // Fold and allin-button must be visible.
  // Raise. call and check must not.
  CString fckra = p_engine_container->symbol_engine_autoplayer()->GetFCKRAString();
  write_log(Preferences()->debug_formula(), 
    "[CAutoplayerFunctions] Buttons seen: %s\n", fckra);
  if (fckra == "F...A") {
    write_log(Preferences()->debug_formula(), 
      "[CAutoplayerFunctions] Fold / allin situation\n");
    return true;
  }
  return false;
}

void CAutoplayerFunctions::CalculateOpenPPLBackupActions() {
  write_log(Preferences()->debug_formula(), 
    "[CAutoplayerFunctions] CalculateOpenPPLBackupActions()\n");
  // Beep is a stand-alone action
  // No backup, can't be combined with pother actions
  // (contrary to f$beep in OH-script).
  if (p_function_collection->EvaluateAutoplayerFunction(
      k_autoplayer_function_beep)) {
    // Turn check/fold off
    p_function_collection->SetAutoplayerFunctionValue(
      k_autoplayer_function_check, false);
    p_function_collection->SetAutoplayerFunctionValue(
      k_autoplayer_function_fold, false);
    return;
  }
  // Allin -> BetPot
  CalculateSingleOpenPPLBackupAction(
    k_autoplayer_function_allin, k_autoplayer_function_betpot_1_1);
  // BetPot -> 3/4 BetPot
  CalculateSingleOpenPPLBackupAction(
    k_autoplayer_function_betpot_1_1, k_autoplayer_function_betpot_3_4);
  // 3/4 BetPot -> 2/3 BetPot
  CalculateSingleOpenPPLBackupAction(
    k_autoplayer_function_betpot_3_4, k_autoplayer_function_betpot_2_3);
  // 2/3 BetPot -> 1/2 BetPot
  CalculateSingleOpenPPLBackupAction(
    k_autoplayer_function_betpot_2_3, k_autoplayer_function_betpot_1_2);
  // 1/2 BetPot -> 1/3 BetPot
  CalculateSingleOpenPPLBackupAction(
    k_autoplayer_function_betpot_1_2, k_autoplayer_function_betpot_1_3);
  // 1/3 BetPot -> 1/4 BetPot
  CalculateSingleOpenPPLBackupAction(
    k_autoplayer_function_betpot_1_3, k_autoplayer_function_betpot_1_4);
  // 1/4 BetPot -> Raise
  CalculateSingleOpenPPLBackupAction(
    k_autoplayer_function_betpot_1_4, k_autoplayer_function_raise);
  // Also: f$betsize -> Raise
  CalculateSingleOpenPPLBackupAction(
    k_autoplayer_function_betsize, k_autoplayer_function_raise);
  // Raise -> Call
  CalculateSingleOpenPPLBackupAction(
    k_autoplayer_function_raise, k_autoplayer_function_call);
  // Call -> Check
  CalculateSingleOpenPPLBackupAction(
    k_autoplayer_function_call, k_autoplayer_function_check);
  // Call -> Allin, in case we only can fold / (call) allin
  if (IsFoldAllinSituation()) {
    CalculateSingleOpenPPLBackupAction(
      k_autoplayer_function_call, k_autoplayer_function_allin);
  }
  // Check -> Fold
  CalculateSingleOpenPPLBackupAction(
    k_autoplayer_function_check, k_autoplayer_function_fold);
  /* Fold -> Check
  CalculateSingleOpenPPLBackupAction(
	  k_autoplayer_function_fold, k_autoplayer_function_check);*/
}

bool CAutoplayerFunctions::IsPercentagePotsizeBet(double decision) {
  // Small negative values: percentaged potsized bets
  // According to the old library...
  return ((decision < 0) && (decision >= -1000));
}

double CAutoplayerFunctions::BetSizeForPercentagedPotsizeBet(double decision) {
  double percentage_0_100 = -100 * decision;
  write_log(Preferences()->debug_formula(), 
    "[CAutoplayerFunctions] Calculate f$betsize for %.2f percent potsize\n",
    percentage_0_100);
  assert(p_engine_container->symbol_engine_chip_amounts() != NULL);
  assert(p_engine_container->symbol_engine_userchair() != NULL);
  assert(p_engine_container->symbol_engine_userchair()->userchair_confirmed());
  double betsize = p_table_state->User()->_bet.GetValue() 
    + p_engine_container->symbol_engine_chip_amounts()->call() 
    + (-1 * decision) * (p_engine_container->symbol_engine_chip_amounts()->pot() + p_engine_container->symbol_engine_chip_amounts()->call());
    write_log(Preferences()->debug_formula(), 
      "[CAutoplayerFunctions] f$betsize is %s\n",
    Number2CString(betsize, 2));
  return betsize;
}

void CAutoplayerFunctions::CalcSecondaryFormulas(void) {
  // Not considering k_standard_function_shoot_replay_frame here,
  // as this function gets handled by CSymbolEngineReplayFrameController
	for (int i=k_hopper_function_sitin; i<= k_standard_function_allin_on_betsize_balance_ratio; ++i) {
    double result = p_function_collection->Evaluate(k_standard_function_names[i], Preferences()->log_hopper_functions());
		write_log(Preferences()->debug_formula(), "[CAutoplayerFunctions] Secondary formulas; %s: %f\n", 
			  k_standard_function_names[i], result);
	}
}



