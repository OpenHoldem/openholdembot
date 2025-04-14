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

#include "pch.h"
#include "BetpotCalculations.h"

#include "CEngineContainer.h"

#include "CSymbolEngineChipAmounts.h"
#include "CSymbolEngineUserchair.h"
#include "CTableState.h"

double BetPotFactor(int betpot_action_code) {
  assert(betpot_action_code >= k_autoplayer_function_betpot_2_1);
  assert(betpot_action_code <= k_autoplayer_function_betpot_1_4);
  double betpot_factor = 1.0;
  switch (betpot_action_code) {
    case k_autoplayer_function_betpot_2_1:
      betpot_factor = 2.0;
      break;
    case k_autoplayer_function_betpot_1_1:
      betpot_factor = 1.0;
      break;
    case k_autoplayer_function_betpot_3_4:
      betpot_factor = 0.75;
      break;
    case k_autoplayer_function_betpot_2_3:
      betpot_factor = 0.667;
      break;
    case k_autoplayer_function_betpot_1_2:
      betpot_factor = 0.5;
      break;
    case k_autoplayer_function_betpot_1_3:
      betpot_factor = 0.333;
      break;
    case k_autoplayer_function_betpot_1_4:
      betpot_factor = 0.25;
      break;
    default:
      betpot_factor = 1.0;
  }
  write_log(Preferences()->debug_autoplayer(), "[AutoPlayer] betpot-factor: %.3f\n",
    betpot_factor);
  return betpot_factor;
}

double BetsizeForBetpot(int betpot_action_code) {
  assert(betpot_action_code >= k_autoplayer_function_betpot_2_1);
  assert(betpot_action_code <= k_autoplayer_function_betpot_1_4);
  double pot_after_i_call = p_engine_container->symbol_engine_chip_amounts()->pot()
    + p_engine_container->symbol_engine_chip_amounts()->call();
  double additional_money_into_pot = BetPotFactor(betpot_action_code) 
    * pot_after_i_call;
  double final_betsize = p_table_state->User()->_bet.GetValue()
    + p_engine_container->symbol_engine_chip_amounts()->call()
    + additional_money_into_pot;
  assert(final_betsize > 0);
  write_log(Preferences()->debug_autoplayer(), "[AutoPlayer] Betsize for betpot-action:  %.3f\n",
    final_betsize);
  return final_betsize;
}