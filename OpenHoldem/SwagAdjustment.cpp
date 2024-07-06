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
#include "SwagAdjustment.h"

#include "BetpotCalculations.h"
#include "CEngineContainer.h"
#include "CFunctionCollection.h"

#include "CSymbolEngineChipAmounts.h"
#include "CSymbolengineGameType.h"
#include "CSymbolEngineIsTournament.h"
#include "CSymbolEngineUserchair.h"
#include "CSymbolEngineTableLimits.h"
#include "CTablemap/CTablemap.h"
#include "CTableState.h"


#include "CSymbolEngineCasino.h"

double MinimumBetsizeDueToPreviousRaise() {
	double minimums_swag_amount = (p_engine_container->symbol_engine_chip_amounts()->call() 
		+ p_engine_container->symbol_engine_chip_amounts()->sraiprev());
	// If there are no bets and no raises the min-bet is 1 big-blind
	if (minimums_swag_amount <= 0)	{
		if (p_engine_container->symbol_engine_tablelimits()->bblind() > 0) 	{
			write_log(Preferences()->debug_betsize_adjustment(), "[BetsizeAdjustment] MinimumBetsizeDueToPreviousRaise: set to 1 big-blind\n");
			minimums_swag_amount = p_engine_container->symbol_engine_tablelimits()->bblind();
		}	else {
			write_log(Preferences()->debug_betsize_adjustment(), "[BetsizeAdjustment] MinimumBetsizeDueToPreviousRaise: SERIOUS TABLEMAP-PROBLEM; no bets; big-blind unknown; setting it to 0.02\n");
			// Setting it to the absolute minimum
			minimums_swag_amount = 0.02;
		}
	}
	write_log(Preferences()->debug_betsize_adjustment(), "[BetsizeAdjustment] MinimumBetsizeDueToPreviousRaise: %.2f\n", minimums_swag_amount);
	assert(minimums_swag_amount > 0);
	return minimums_swag_amount;
}

double MaximumPossibleBetsizeBecauseOfBalance() {
	assert(p_engine_container->symbol_engine_userchair() != NULL);
  assert(p_engine_container->symbol_engine_userchair()->userchair_confirmed());
	double maximum_betsize = p_table_state->User()->_bet.GetValue()
		+ p_table_state->User()->_balance.GetValue();
	if (maximum_betsize <= 0) {
    // http://www.maxinmontreal.com/forums/viewtopic.php?f=110&t=17915#p124550
    write_log(k_always_log_errors, "[BetsizeAdjustment] Invalid balance and bet. Sum <= 0.\n");
    maximum_betsize = kUndefinedZero;
  }
	write_log(Preferences()->debug_betsize_adjustment(), "[BetsizeAdjustment] MaximumPossibleBetsizeBecauseOfBalance %.2f\n",
		maximum_betsize);
	return maximum_betsize;
}

double SwagAmountAjustedToCasino(double amount_to_raise_to) {
	int userchair = p_engine_container->symbol_engine_userchair()->userchair();
	assert(amount_to_raise_to >= 0);
	double swag_amount_ajusted_to_casino = amount_to_raise_to;
	// WinHoldems f$srai should return, what we want to add to (call + currentbet)
	// so we have to calculate our adjustment the other way.
	// http://www.winholdem.net/help/help-formula.html
	// http://forum.winholdem.net/wbb/viewtopic.php?t=1849
	if (p_tablemap->swagtextmethod() == 2) {
		// Old adjustment: call, so currentbet is too much
		swag_amount_ajusted_to_casino = amount_to_raise_to - p_table_state->User()->_bet.GetValue();
	}	else if (p_tablemap->swagtextmethod() == 3)	{
		// Old adjustment: call + currentbet.
		// Everything fine, nothing to do.
	}	else {
		// Default: swagtextmethod == 1
		// Old adjustment: 0, currentbet and call are too much.
		swag_amount_ajusted_to_casino = amount_to_raise_to 
			- p_table_state->User()->_bet.GetValue()
			- p_engine_container->symbol_engine_chip_amounts()->call();
	}
	write_log(Preferences()->debug_betsize_adjustment(), "[BetsizeAdjustment] SwagAmountAjustedToCasino %.2f\n",
		swag_amount_ajusted_to_casino);
	return swag_amount_ajusted_to_casino;
}

bool BetSizeIsAllin(double amount_to_raise_to_in_dollars_and_cents) {
	double maximum_betsize = MaximumPossibleBetsizeBecauseOfBalance();
	write_log(Preferences()->debug_betsize_adjustment(), "[BetsizeAdjustment] BetSizeIsAllin() desired betsize: %.2f\n",
		amount_to_raise_to_in_dollars_and_cents);
	write_log(Preferences()->debug_betsize_adjustment(), "[BetsizeAdjustment] BetSizeIsAllin() maximum betsize: %.2f\n",
		maximum_betsize);
	return (amount_to_raise_to_in_dollars_and_cents >= maximum_betsize);
}

double RoundedBetsizeForTournaments(double amount_to_raise_to_in_dollars_and_cents) {
	if (!p_engine_container->symbol_engine_istournament()->istournament()) {
		write_log(Preferences()->debug_betsize_adjustment(), 
			"[BetsizeAdjustment] Game-type is cash-game. No rounding necessary.\n");
		return amount_to_raise_to_in_dollars_and_cents;
	}
	write_log(Preferences()->debug_betsize_adjustment(), 
			"[BetsizeAdjustment] This is a tournament. Rounding to full dollars or to allin\n");
	if (BetSizeIsAllin(amount_to_raise_to_in_dollars_and_cents)) {
		write_log(Preferences()->debug_betsize_adjustment(), 
			"[BetsizeAdjustment] Tournament, but no rounding because we will go allin.\n");
		return amount_to_raise_to_in_dollars_and_cents;
	}	else {
		int rounded_amount = int(amount_to_raise_to_in_dollars_and_cents);
		write_log(Preferences()->debug_betsize_adjustment(), 
			"[BetsizeAdjustment] Rounded dollars for tournaments: %.2f\n", rounded_amount);
		return rounded_amount;
	}
}

double MaximumBetsizeForGameType() {
	double maximum_betsize = 0;
	if (p_engine_container->symbol_engine_gametype()->ispl())	{
		write_log(Preferences()->debug_betsize_adjustment(),
			"[BetsizeAdjustment] Game-type is Pot Limit.\n");
		maximum_betsize = BetsizeForBetpot(k_autoplayer_function_betpot_1_1);
    write_log(Preferences()->debug_betsize_adjustment(), 
      "[BetsizeAdjustment] MaximumBetsizeForPotLimit: %.2f\n", maximum_betsize);
	}	else if (p_engine_container->symbol_engine_gametype()->isfl()) {
		write_log(Preferences()->debug_betsize_adjustment(), 
			"[BetsizeAdjustment] Game-type is Fixed Limit. No betsizing supported.\n");
		maximum_betsize = 0;
	}	else {
		int userchair = p_engine_container->symbol_engine_userchair()->userchair();
		write_log(Preferences()->debug_betsize_adjustment(), 
			"[BetsizeAdjustment] Game-type is No Limit. Betsize restricted only by users balance.\n");
		maximum_betsize = MaximumPossibleBetsizeBecauseOfBalance();
	}
	write_log(Preferences()->debug_betsize_adjustment(), 
		"[BetsizeAdjustment] MaximumBetsizeForGameType(): %.2f\n", maximum_betsize);
	assert(maximum_betsize >= 0);
	return maximum_betsize;	
}

double RoundToBeautifulBetsize(const double amount_to_raise_to) {
  assert(amount_to_raise_to >= p_engine_container->symbol_engine_tablelimits()->bblind());
  // Don't round allin betsizes.
  if (BetSizeIsAllin(amount_to_raise_to) ) {
    write_log(Preferences()->debug_betsize_adjustment(),
      "[BetsizeAdjustment] RoundToBeautifulBetsize: no rounding, because betsize is allin.\n");
    return amount_to_raise_to;
  }
  double result;
  // Don't round very small betsizes.
  // There might be aa reason for it in very small pots.
  if (amount_to_raise_to < (2.6 * p_engine_container->symbol_engine_tablelimits()->bblind())) {
    write_log(Preferences()->debug_betsize_adjustment(),
      "[BetsizeAdjustment] RoundToBeautifulBetsize: no rounding to multiples of SB/BB, because too small betsize\n");
    result = amount_to_raise_to;
  } else if (amount_to_raise_to == Rounding(amount_to_raise_to, p_engine_container->symbol_engine_tablelimits()->bblind())) {
    // Don't round betsizes that are alreadz a multiple of big-blind.
    // This case deserves specdial attention, because big-blind
    // might be not a multiple of smallblind, e.g $2/$5
    // http://www.maxinmontreal.com/forums/viewtopic.php?f=120&t=18592
    result = amount_to_raise_to;
    write_log(Preferences()->debug_betsize_adjustment(),
      "[BetsizeAdjustment] RoundToBeautifulBetsize: no rounding, because betsize is a multiple of big-blind\n");
    // But don~t return anzthing here.
    // Continue with rounding gor large numbers (later)
  } else if (amount_to_raise_to < (10 * p_engine_container->symbol_engine_tablelimits()->sblind())) {
    // Small numbers
    // Round to multiples of SB
    result = Rounding(amount_to_raise_to, p_engine_container->symbol_engine_tablelimits()->sblind());
    write_log(Preferences()->debug_betsize_adjustment(),
      "[BetsizeAdjustment] Rounding to multiples of SB %.2f\n", result);
  } else {
    // "Large" numbers
    // Round to multiples of BB
    result = Rounding(amount_to_raise_to, p_engine_container->symbol_engine_tablelimits()->bblind());
    write_log(Preferences()->debug_betsize_adjustment(),
      "[BetsizeAdjustment] Rounding to multiples of BB %.2f\n", result);
  }
  // whole numbers (larg numbers)
  if (result > 25000) {
    result = Rounding(result, 1000);
    write_log(Preferences()->debug_betsize_adjustment(),
      "[BetsizeAdjustment] Rounding to multiples of $1000 %.2f\n", result);
  } else if (result > 2500) {
    result = Rounding(result, 100);
    write_log(Preferences()->debug_betsize_adjustment(),
      "[BetsizeAdjustment] Rounding to multiples of $100 %.2f\n", result);
  } else if (result > 250) {
    result = Rounding(result, 10);
    write_log(Preferences()->debug_betsize_adjustment(),
      "[BetsizeAdjustment] Rounding to multiples of $10 %.2f\n", result);
  } else if (result > 20) {
    result = Rounding(result, 1);
    write_log(Preferences()->debug_betsize_adjustment(),
      "[BetsizeAdjustment] Rounding to multiples of $1 %.2f\n", result);
  }  
  // Whole numbers (small),
  // but only if we want to bet more than 0.90.
  // Avoid rounding to 0.
  else if ((result > 0.90) && (abs(result - int(result)) < 0.10)) {
    result = Rounding(result, 1);
    write_log(Preferences()->debug_betsize_adjustment(),
      "[BetsizeAdjustment] Rounding a little bit to nearest $1 %.2f\n", result);
  }
  write_log(Preferences()->debug_betsize_adjustment(),
    "[BetsizeAdjustment] RoundToBeautifulBetsize: %.2f rounded to %.2f\n",
    amount_to_raise_to, result);
  assert(result > 0);
  return result;
}

double MaximumBetsizeDueToMaxOppStack() {
	double maximum = p_engine_container->symbol_engine_chip_amounts()->call() + p_table_state->User()->_bet.GetValue() + p_engine_container->symbol_engine_chip_amounts()->MaxActiveOpponentStack();
	write_log(Preferences()->debug_betsize_adjustment(), "[BetsizeAdjustment] MaximumBetsizeDueToMaxOppStack: %.2f\n", maximum);
	assert(maximum > 0);
	return maximum;
}

double AdjustedBetsize(double amount_to_raise_to) {
	double original_amount_to_raise_to = amount_to_raise_to;
  write_log(Preferences()->debug_betsize_adjustment(),
    "[BetsizeAdjustment] Rounding to beautiful numbers\n");
  // Rounding to beautiful numbers but only if enabled
  amount_to_raise_to = RoundToBeautifulBetsize(amount_to_raise_to);
  double minimum;
  if (p_engine_container->symbol_engine_casino()->ConnectedToDDPoker()) {
    write_log(Preferences()->debug_betsize_adjustment(),
      "[BetsizeAdjustment] Special adjustment for DDPoker needed.\n");
	  minimum = min(MinimumBetsizeDueToPreviousRaise(), MaximumBetsizeDueToMaxOppStack() );
  } else {
	  minimum = MinimumBetsizeDueToPreviousRaise();
  }
	AdaptValueToMinMaxRange(&amount_to_raise_to, minimum, amount_to_raise_to);
	AdaptValueToMinMaxRange(&amount_to_raise_to, amount_to_raise_to, MaximumBetsizeForGameType());
	AdaptValueToMinMaxRange(&amount_to_raise_to, amount_to_raise_to, MaximumPossibleBetsizeBecauseOfBalance());
  // Rounding to beautiful numbers (here full dollars)
  AdaptValueToMinMaxRange(&amount_to_raise_to, amount_to_raise_to, RoundedBetsizeForTournaments(amount_to_raise_to));
  // Special handling for DDPoker
  // http://www.maxinmontreal.com/forums/viewtopic.php?f=120&t=19185&hilit=ddpoker
  if(p_engine_container->symbol_engine_casino()->ConnectedToDDPoker()){
    //!!?? Duplicate to ZeeZooLaas code above?
		AdaptValueToMinMaxRange(&amount_to_raise_to, amount_to_raise_to, MaximumBetsizeDueToMaxOppStack());
	}
  AdaptValueToMinMaxRange(&amount_to_raise_to, 0, SwagAmountAjustedToCasino(amount_to_raise_to));
	return amount_to_raise_to; 
}
