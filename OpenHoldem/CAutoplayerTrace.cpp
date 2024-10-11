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
#include "CAutoplayerTrace.h"

#include "CAutoplayerFunctions.h"
#include "CBetroundCalculator.h"
#include "CEngineContainer.h"
#include "CFunctionCollection.h"
#include "CHandresetDetector.h"
#include "COHScriptObject.h"
#include "CopenHoldemStatusbar.h"

#include "CScraper.h"
#include "CSymbolEngineAutoplayer.h"
#include "CSymbolEngineChipAmounts.h"
#include "CSymbolEngineHandrank.h"
#include "CSymbolEnginePokerval.h"
#include "CSymbolEnginePrwin.h"
#include "CSymbolEngineUserchair.h"
#include "CTableState.h"

#define ENT CSLock lock(m_critsec);

CAutoplayerTrace *p_autoplayer_trace = NULL;

CAutoplayerTrace::CAutoplayerTrace() {
  Clear();
}

CAutoplayerTrace::~CAutoplayerTrace() {
  Clear();
}

void CAutoplayerTrace::Clear() {
  ENT 
  _indentation = 0;
  _number_of_log_lines = 0;
  _symboltrace_collection.RemoveAll();
  _already_logged_symbols.clear();
}

bool CAutoplayerTrace::SymbolNeedsToBeLogged(CString name) {
  // DLL, as there is no caching and value might change
  if (memcmp(name, "dll$", 4) == 0) return true;
  // Memory-store and recall-commands
  if (memcmp(name, "me_", 3) == 0) return true;
  // OpenPPL-user-variables might also change (once)
  // We don't care about multiple loggings of userchair here
  if (_memicmp(name, "user", 4) == 0) return true;
  // True random numbers that don't get cached,
  // i.e. OH-script "random" and OpenPPL "Random"
  if (memcmp(name, "random", 6) == 0) return true;
  if (memcmp(name, "Random", 6) == 0) return true;
  // Values that already got logged can be ignored
  if (_already_logged_symbols[name] == true) return false;
  // Everything else needs to be logged
  return true;
}

int CAutoplayerTrace::Add(CString symbol) {
  ENT
  write_log(Preferences()->debug_auto_trace(),
    "[CAutoplayerTrace] Add (%s, ...)\n", symbol);
  // This function for symbols without value is for functions only.
  // These functions are eitherpredefined (f$), userdefined (f$)
  // or OpenPPL (upper-cases).
  // The value will be backpatched later.
  assert(COHScriptObject::IsFunction(symbol)
    || COHScriptObject::IsOpenPPLSymbol(symbol));
  CString new_message;
  new_message.Format("%s%s = ",
    Indentation(), symbol);
  _symboltrace_collection.Add(new_message);
  // Nothing to do for _already_logged_symbols here,
  // as this Add()-function is for the first-time-evaluation
  // of f$functions.
  _number_of_log_lines++;
  return (_number_of_log_lines - 1); 
}

void CAutoplayerTrace::Add(CString symbol, double value, bool undefined /* = false */) {
  ENT
  write_log(Preferences()->debug_auto_trace(),
    "[CAutoplayerTrace] Add (%s, %.3f)\n",
    symbol, value);
  if (!SymbolNeedsToBeLogged(symbol)) return;
  CString new_message;
  if (undefined) {
    // For empty functions with NULL parse-tree
    assert(value == kUndefinedZero);
    new_message.Format("%s%s = %.3f   [undefined]",
      Indentation(), symbol, value);
  } else if (COHScriptObject::IsFunction(symbol)
      || COHScriptObject::IsOpenPPLSymbol(symbol)) {
    // Function with known value a priori
    new_message.Format("%s%s = %.3f   [cached]",
      Indentation(), symbol, value);
  } else {
    // "Normal" symbol
    new_message.Format("%s%s = %.3f",
      Indentation(), symbol, value); 
  }
  _symboltrace_collection.Add(new_message);
  _already_logged_symbols[symbol] = true;
  _number_of_log_lines++;
}

void CAutoplayerTrace::Add(CString symbol, CString value, bool undefined /* = false */) {
	ENT
		write_log(Preferences()->debug_auto_trace(),
			"[CAutoplayerTrace] Add (%s, %s)\n",
			symbol, value);
	if (!SymbolNeedsToBeLogged(symbol)) return;
	CString new_message;
	if (undefined) {
		// For empty functions with NULL parse-tree
		assert(value == "");
		new_message.Format("%s%s = %s   [undefined]",
			Indentation(), symbol, value);
	}
	else if (COHScriptObject::IsFunction(symbol)
		|| COHScriptObject::IsOpenPPLSymbol(symbol)) {
		// Function with known value a priori
		new_message.Format("%s%s = %s   [cached]",
			Indentation(), symbol, value);
	}
	else {
		// "Normal" symbol
		new_message.Format("%s%s = %s",
			Indentation(), symbol, value);
	}
	_symboltrace_collection.Add(new_message);
	_already_logged_symbols[symbol] = true;
	_number_of_log_lines++;
}


void CAutoplayerTrace::BackPatchValueAndLine(
    int index, double value, int starting_line_of_function, CString path) {
  assert(index >= 0);
  assert(index < _number_of_log_lines);
  // Starting line should be > 0, but auto-generated missing 
  // vital functions like f$check can have line == 0.
  assert(starting_line_of_function >= 0);
  int executed_absolute_line = starting_line_of_function 
    + _last_evaluated_relative_line_number;
  // Already done:
  // Indentation, symbol, " = "
  CString complete_message;
  complete_message.Format("%s%.3f   [Line %d/%d, %s]", 
    _symboltrace_collection.GetAt(index),
    value,
    _last_evaluated_relative_line_number,
    executed_absolute_line,
    FilenameWithoutPath(path));
  _symboltrace_collection.SetAt(index, complete_message);
}

void CAutoplayerTrace::Indent(bool more) {
  if (more) {
    _indentation += 2;
  } else {
    _indentation -= 2;
  }
  if (_indentation < 0) {
    // This could happen if we calculate and log prwin functions
    // and the heartbeat-threat resets the autoplayer-trace inbetween.
    // Most easy way to fix it: continue gracefully
    _indentation = 0;
  }
}

CString CAutoplayerTrace::Indentation() {
  assert(_indentation >= 0);
  CString format;
  format.Format("%%%ds", _indentation);
  CString indentation;
  indentation.Format(format, "");
  return indentation;
}

void CAutoplayerTrace::Print(const char *action_taken, bool full_log_for_primary_formulas) {
  // Probably not necessary: CSLock lock(log_critsec);
  // as nothing else should happen when the autoplayer is finished
  if (full_log_for_primary_formulas) {
    LogPlayers();
    // This information is only meaningful for playing decision f$all .. f$fold
    LogBasicInfo(action_taken);
  } else {
    LogSecondaryAction(action_taken);
  }
  LogAutoPlayerTrace();
  Clear();
}

CString CAutoplayerTrace::BestAction() {
  for (int i=k_autoplayer_function_allin; i<=k_autoplayer_function_fold; ++i) {
    if (p_autoplayer_functions->GetAutoplayerFunctionValue(i)) {
      if (i == k_autoplayer_function_betsize) {
        // Special treatment for f$betsize
        // e.g. "f$betsize = 201.47"
        CString best_action;
        best_action.Format("%s = %.2f", k_standard_function_names[i],
          p_function_collection->EvaluateAutoplayerFunction(k_autoplayer_function_betsize));
        return best_action;
      }
      else {
        return k_standard_function_names[i];
      }
    }
  }
  // No action can happen if it is not our turn (best action in GUI)
  return "no action";
}

void CAutoplayerTrace::LogBasicInfo(const char *action_taken) {
  CString	comcards, temp, rank, pokerhand;
  CString	fcra_formula_status;
  int		userchair = p_engine_container->symbol_engine_userchair()->userchair();
  int		betround  = p_betround_calculator->betround();
  // Player cards
  // There always exists a user, if not then we have a fake-player. ;-)
  assert(p_table_state->User() != NULL);
  CString player_cards = p_table_state->User()->Cards();
  // Common cards
  comcards = "";
  if (betround >= kBetroundFlop) {
    for (int i=0; i<kNumberOfFlopCards; i++) {
      if (p_table_state->CommonCards(i)->IsKnownCard()) {
        comcards.Append(p_table_state->CommonCards(i)->ToString());
      }
    }
  }
  if (betround >= kBetroundTurn) {
    comcards.Append(p_table_state->TurnCard()->ToString());
  }
  if (betround >= kBetroundRiver) {
    comcards.Append(p_table_state->RiverCard()->ToString());
  }
  comcards.Append("..........");
  comcards = comcards.Left(10);
  // Always use handrank169 here
  rank.Format("%.0f", p_engine_container->symbol_engine_handrank()->handrank169());
  // poker hand
  pokerhand = p_engine_container->symbol_engine_pokerval()->HandType();
  // fcra_seen
  CString fcra_seen = p_engine_container->symbol_engine_autoplayer()->GetFCKRAString();
  // fcra formula status
  fcra_formula_status.Format("%s%s%s%s%s",
	p_function_collection->EvaluateAutoplayerFunction(k_autoplayer_function_fold)  ? "F" : ".",
	p_function_collection->EvaluateAutoplayerFunction(k_autoplayer_function_call)  ? "C" : ".",
	p_function_collection->EvaluateAutoplayerFunction(k_autoplayer_function_check) ? "K" : ".",
	p_function_collection->EvaluateAutoplayerFunction(k_autoplayer_function_raise) ? "R" : ".",
	p_function_collection->EvaluateAutoplayerFunction(k_autoplayer_function_allin) ? "A" : ".");
  // More verbose summary in the log
  // The old WinHoldem format was a complete mess
  write_log_separator(k_always_log_basic_information, "Basic Info");
  write_log(k_always_log_basic_information, "  Version:       %s\n",    VERSION_TEXT); 
  write_log(k_always_log_basic_information, "  Handnumber:    %s\n",    p_handreset_detector->GetHandNumber());
  write_log(k_always_log_basic_information, "  Chairs:        %5d\n",   p_tablemap->nchairs());
  write_log(k_always_log_basic_information, "  Userchair:     %5d\n",   userchair);
  write_log(k_always_log_basic_information, "  Holecards:     %s\n",    player_cards.GetString());
  write_log(k_always_log_basic_information, "  Community:     %s\n",    comcards.GetString());
  write_log(k_always_log_basic_information, "  Handrank:      %s\n",    rank.GetString());
  write_log(k_always_log_basic_information, "  Hand:          %s\n",    pokerhand.GetString());
  write_log(k_always_log_basic_information, "  My balance:    %9.2f\n", p_table_state->User()->_balance.GetValue());
  write_log(k_always_log_basic_information, "  My currentbet: %9.2f\n", p_table_state->User()->_bet.GetValue()); 
  write_log(k_always_log_basic_information, "  To call:       %9.2f\n", p_engine_container->symbol_engine_chip_amounts()->call());
  write_log(k_always_log_basic_information, "  Pot:           %9.2f\n", p_engine_container->symbol_engine_chip_amounts()->pot());
  write_log(k_always_log_basic_information, "  Big blind:     %9.2f\n", p_engine_container->symbol_engine_tablelimits()->bblind());
  write_log(k_always_log_basic_information, "  Big bet (FL):  %9.2f\n", p_engine_container->symbol_engine_tablelimits()->bigbet());
  write_log(k_always_log_basic_information, "  f$betsize:     %9.2f\n", p_function_collection->EvaluateAutoplayerFunction(k_autoplayer_function_betsize));
  write_log(k_always_log_basic_information, "  Formulas:      %s\n",    fcra_formula_status.GetString());
  write_log(k_always_log_basic_information, "  Buttons:       %s\n",    fcra_seen.GetString());
  // !! "Best action" is undefined if the executed action
  // is "something else" like a hopper function
  write_log(k_always_log_basic_information, "  Best action:   %s\n", BestAction().GetString());
  write_log(k_always_log_basic_information, "  Action taken:  %s\n",    action_taken);
  write_log_separator(k_always_log_basic_information, "");
  // Also show "BestAction" in the statusbar.
  // This needs to be set exactly once to avoid multiple evaluations 
  // of the autoplayer functions
  p_openholdem_statusbar->SetLastAction(BestAction());
}

void CAutoplayerTrace::LogPlayers() {
  write_log_separator(k_always_log_basic_information, "Players");
  // Logging all players at the table
  // starting at userchair (hero), so that we can easily see all raises behind him
  int	userchair = p_engine_container->symbol_engine_userchair()->userchair();
  int nchairs = p_tablemap->nchairs();
  for (int i = 0; i < nchairs; ++i) {
    int chair = (userchair + i) % nchairs;
    CString data;
    data.Format("Chair %2i  %s\n", chair, p_table_state->Player(chair)->DataDump());
   write_log(k_always_log_basic_information, data.GetBuffer());
  }
  write_log_separator(k_always_log_basic_information, "");
}

void CAutoplayerTrace::LogSecondaryAction(const char *action_taken) {
  write_log_separator(true, "Secondary Action");
  write_log(k_always_log_basic_information, "  Action taken:  %s\n", action_taken);
  write_log_separator(true, "");
}

void CAutoplayerTrace::LogAutoPlayerTrace() {
  if (_symboltrace_collection.GetSize() <= 0) {
    return;
  }
  write_log_separator(true, "Autoplayer Trace");
  for (int i=0; i<_symboltrace_collection.GetSize(); ++i) {
	  write_log_nostamp(true, "%s\n", _symboltrace_collection.GetAt(i));
  }
  write_log_separator(true, "");
}


