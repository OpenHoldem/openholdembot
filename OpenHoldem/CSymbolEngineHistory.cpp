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
#include "CSymbolEngineHistory.h"

#include "CBetroundCalculator.h"
#include "CEngineContainer.h"

#include "CSymbolEngineActiveDealtPlaying.h"
#include "CSymbolEngineChipAmounts.h"
#include "CSymbolEngineTableLimits.h"
#include "CSymbolEngineUserchair.h"
#include "CSymbolEngineChecksBetsFolds.h"
#include "CTablemap/CTablemap.h"
#include "FloatingPoint_Comparisions.h"


const char* const k_hist_sym_strings[24]/*!!!k_hist_sym_count]*/ = {
	// CHIP AMOUNTS (21)
	"balance",
	"balance0", "balance1", "balance2", "balance3", "balance4",
	"balance5", "balance6", "balance7", "balance8", "balance9",
	"stack0", "stack1", "stack2", "stack3", "stack4",
	"stack5", "stack6", "stack7", "stack8", "stack9",
	// FLUSHES SETS STRAIGHTS (16)
	/*"nsuited", 	"nsuitedcommon", "tsuit", "tsuitcommon", "nranked",
	"nrankedcommon", "trank", "trankcommon", "nstraight", "nstraightcommon",
	"nstraightfill", "nstraightfillcommon", "nstraightflush",
	"nstraightflushcommon", "nstraightflushfill", "nstraightflushfillcommon",
	*/
	// PROBABILITIES (3)
	"prwin", "prlos", "prtie",
	//POKER VALUES (5)
	/*"pokerval", "pokervalplayer", "pokervalcommon", "pcbits", "npcbits",
	*/
	//HAND TESTS (12)
	/*"ishandup", "ishandupcommon", "ishicard", "isonepair", "istwopair", "isthreeofakind",
	"isstraight", "isflush", "isfullhouse", "isfourofakind", "isstraightflush", "isroyalflush",
	*/
	//POCKET/COMMON TESTS (5)
	/*"ishipair", "islopair", "ismidpair", "ishistraight", "ishiflush",
	*/
	//(UN)KNOWN CARDS (1)
	/*"ncardsbetter",
	*/
	//NHANDS (6)
	/*"nhands", "nhandshi", "nhandslo", "nhandsti", "prwinnow", "prlosnow",
	*/
	//RANK BITS (8)
	/*"rankbits", "rankbitscommon", "rankbitsplayer", "rankbitspoker", "srankbits",
	"srankbitscommon", "srankbitsplayer", "srankbitspoker",
	*/
	//RANK HI (8)
	/*"rankhi", "rankhicommon", "rankhiplayer", "rankhipoker", "srankhi", "srankhicommon",
	"srankhiplayer", "srankhipoker",
	*/
	//RANK LO (8)
	/*"ranklo", "ranklocommon", "rankloplayer", "ranklopoker", "sranklo", "sranklocommon",
	"srankloplayer", "sranklopoker",
	*/
};

CSymbolEngineHistory::CSymbolEngineHistory() {
	// The values of some symbol-engines depend on other engines.
	// As the engines get later called in the order of initialization
	// we assure correct ordering by checking if they are initialized.
	assert(p_engine_container->symbol_engine_active_dealt_playing() != NULL);
	assert(p_engine_container->symbol_engine_chip_amounts() != NULL);
	assert(p_engine_container->symbol_engine_tablelimits() != NULL);
	assert(p_engine_container->symbol_engine_userchair() != NULL);
	// Making sure that _hist_sym_count is correct,
	// to avoid array overflows later if we remove symbols
	// without adapting the counter.
	// Last index is (_hist_sym_count - 1).
	//!!!assert(k_hist_sym_strings[k_hist_sym_count - 1] != NULL);
}

CSymbolEngineHistory::~CSymbolEngineHistory() {}

void CSymbolEngineHistory::InitOnStartup() {
	UpdateOnConnection();
}

void CSymbolEngineHistory::UpdateOnConnection() {
	UpdateOnHandreset();
}

void CSymbolEngineHistory::UpdateOnHandreset() {
	write_log(Preferences()->debug_symbolengine(),
		"[SymbolEngineHistory] Update on handreset\n");
	_prevaction = k_prevaction_undefined;
	// Element 0 is unused
	for (int i = 0; i<(kNumberOfBetrounds + 1); ++i) {
		_nplayersround[i] = 0;
		_nbetsround[i] = 0;
		for (int j = 0; j <= k_autoplayer_function_fold; ++j) {
			_autoplayer_actions[i][j] = 0;
		}
		// History-smbols
		for (int j = 0; j<24 /*!!!k_hist_sym_count*/; ++j) {
			_hist_sym[j][i] = 0.0;
		}
	}
}

void CSymbolEngineHistory::UpdateOnNewRound() {
}

void CSymbolEngineHistory::UpdateOnMyTurn() {
	// Always update history when it is my turn
	// That's when we have stable frames.
	CalculateHistory();
}

void CSymbolEngineHistory::UpdateOnHeartbeat() {
	// Updating here only if the user is no longer playing
	// At random heartbeats we don#t have stable frames;
	// this can lead to ugly misscrapes, for example
	// river-card already painted, but bets of the turn still visible.
	// http://www.maxinmontreal.com/forums/viewtopic.php?f=217&t=20825
	// Try to avoid such situations!
	if (!p_table_state->User()->HasAnyCards()) {
		CalculateHistory();
		return;
	}
	// Also calculating history when we acted on this round.
	// This seems to be necess<ry to catch nbetsroundX if we are the last raiser
	// and seems to be safe (not affected by the problem above)
	// http://www.maxinmontreal.com/forums/viewtopic.php?f=111&t=21226
	if (DidAct()) {
		CalculateHistory();
		return;
	}
}

void CSymbolEngineHistory::UpdateAfterAutoplayerAction(int autoplayer_action_code) {
	AssertRange(autoplayer_action_code, k_autoplayer_function_beep,
		k_autoplayer_function_fold);
	// Nothing to do of the "action" was "beep".
	if (autoplayer_action_code == k_autoplayer_function_beep) {
		return;
	}
	// Special handling for check/call
	// Some people have problems scraping check and call correctly,
	// as usually only one of these buttons is visible
	// and they often share the same place.
	// Others might decide to "call" if it is free to call, etc.
	// Therefore we set "_prevaction", "didchec", "didcall" here 
	// depending on the amount to call.
	if ((autoplayer_action_code == k_autoplayer_function_call)
		|| (autoplayer_action_code == k_autoplayer_function_check)) {
		if (IsSmallerOrEqual(p_engine_container->symbol_engine_chip_amounts()->call(), 0.0)) {
			// It was free to check
			_autoplayer_actions[BETROUND][k_autoplayer_function_check]++;
			SetPrevaction(k_autoplayer_function_check);
		}
		else {
			// There was name positive amount to call
			_autoplayer_actions[BETROUND][k_autoplayer_function_call]++;
			SetPrevaction(k_autoplayer_function_call);
		}
	}
	else {
		_autoplayer_actions[BETROUND][autoplayer_action_code]++; //!!! betpot? Todo: set _didswag
		SetPrevaction(autoplayer_action_code);
	}
}

// Attention: SetPrevaction takes an OH-autoplayer-constant as input,
// but needs to translate it to an old-style Winholdem-prevaction-constant
void CSymbolEngineHistory::SetPrevaction(int autoplayer_action_code) {
	switch (autoplayer_action_code) {
	case k_autoplayer_function_allin:
		_prevaction = k_prevaction_allin;
		break;
	case k_autoplayer_function_betpot_2_1:
	case k_autoplayer_function_betpot_1_1:
	case k_autoplayer_function_betpot_3_4:
	case k_autoplayer_function_betpot_2_3:
	case k_autoplayer_function_betpot_1_2:
	case k_autoplayer_function_betpot_1_3:
	case k_autoplayer_function_betpot_1_4:
		_prevaction = k_prevaction_betsize;
		break;
	case k_autoplayer_function_betsize:
		_prevaction = k_prevaction_betsize;
		break;
	case k_autoplayer_function_raise:
		_prevaction = k_prevaction_raise;
		break;
	case k_autoplayer_function_call:
		_prevaction = k_prevaction_call;
		break;
	case k_autoplayer_function_check:
		_prevaction = k_prevaction_check;
		break;
	case k_autoplayer_function_fold:
		_prevaction = k_prevaction_fold;
		break;
	default:
		_prevaction = k_prevaction_undefined;
		break;
	};
}

void CSymbolEngineHistory::CalculateHistory() {
	// Collect symbol if ismyturn.
	// Per definition we need to get the value at last myturn in betround N.
	write_log(Preferences()->debug_symbolengine(),
		"[SymbolEngineHistory] Update on my turn\n");
	int	betround = p_betround_calculator->betround();
	for (int i = 0; i<24/*!!!k_hist_sym_count*/; ++i) {
		double result;
		p_engine_container->EvaluateSymbol(k_hist_sym_strings[i], &result);
		_hist_sym[i][betround] = result;
	}

	if (_nplayersround[BETROUND] == 0) {
		_nplayersround[BETROUND] =
			p_engine_container->symbol_engine_active_dealt_playing()->nplayersplaying() +
			p_engine_container->symbol_engine_checks_bets_folds()->nopponentsfoldedround();
	}
	double maxbet = 0.0;
	for (int i = 0; i<p_tablemap->nchairs(); i++) {
		// Be careful: in some cases it might be that name user folds,
		// but "Fold" gets displayed where formerly his bet got displayed.
		// This may lead to ugly mis-scrapes, that's why he have to check
		// if the user is still playing.
		// (http://www.maxinmontreal.com/forums/viewtopic.php?f=111&t=10929)		
		if (IsBitSet(p_engine_container->symbol_engine_active_dealt_playing()->playersplayingbits(), i)) {
			double current_players_bet = p_table_state->Player(i)->_bet.GetValue();
			maxbet = MAX(maxbet, current_players_bet);
		}
	}
	double bet = MAX(p_engine_container->symbol_engine_tablelimits()->bet(), p_engine_container->symbol_engine_tablelimits()->bblind());
	if (bet > 0) {
		maxbet /= bet;
		_nbetsround[BETROUND] = MAX(_nbetsround[BETROUND], maxbet);
	}
	else {
		write_log(Preferences()->debug_symbolengine(), "[SymbolEngineHistory] CSymbolEngineHistory::CalculateHistory() Skipping calculation of nbetsround due to unknown min-bet\n");
	}
}

double CSymbolEngineHistory::HistorySymbol(const CString sym, const int round) {
	write_log(Preferences()->debug_symbolengine(),
		"[SymbolEngineHistory] Looking up %s for round %i\n", sym, round);
	for (int i = 0; i<24/*!!!k_hist_sym_count*/; i++) {
		if (memcmp(sym, k_hist_sym_strings[i], strlen(sym)) == 0 && strlen(sym) == strlen(k_hist_sym_strings[i])) {
			return _hist_sym[i][round];
		}
	}
	return kUndefinedZero;
}

bool CSymbolEngineHistory::EvaluateSymbol(const CString name, double *result, bool log /* = false */) {
	FAST_EXIT_ON_OPENPPL_SYMBOLS(name);
	if (memcmp(name, "did", 3) == 0) {
		if (memcmp(name, "didchec", 7) == 0 && strlen(name) == 7) {
			*result = didchec(p_betround_calculator->betround());
		}
		else if (memcmp(name, "didcall", 7) == 0 && strlen(name) == 7) {
			*result = didcall(p_betround_calculator->betround());
		}
		else if (memcmp(name, "didrais", 7) == 0 && strlen(name) == 7) {
			*result = didrais(p_betround_calculator->betround());
		}
		else if (memcmp(name, "didbetsize", 10) == 0 && strlen(name) == 10) {
			*result = didswag(p_betround_calculator->betround());
		}
		else if (memcmp(name, "didfold", 7) == 0 && strlen(name) == 7) {
			*result = didfold(p_betround_calculator->betround());
		}
		else if (memcmp(name, "didalli", 7) == 0 && strlen(name) == 7) {
			*result = didalli(p_betround_calculator->betround());
		}
		else if (memcmp(name, "didchecround", 12) == 0 && strlen(name) == 13) {
			*result = didchec(RightDigitCharacterToNumber(name));
		}
		else if (memcmp(name, "didcallround", 12) == 0 && strlen(name) == 13) {
			*result = didcall(RightDigitCharacterToNumber(name));
		}
		else if (memcmp(name, "didraisround", 12) == 0 && strlen(name) == 13) {
			*result = didrais(RightDigitCharacterToNumber(name));
		}
		else if (memcmp(name, "didbetsizeround", 15) == 0 && strlen(name) == 16) {
			*result = didswag(RightDigitCharacterToNumber(name));
		}
		else if (memcmp(name, "didfoldround", 12) == 0 && strlen(name) == 13) {
			*result = didfold(RightDigitCharacterToNumber(name));
		}
		else if (memcmp(name, "didalliround", 12) == 0 && strlen(name) == 13) {
			*result = didalli(RightDigitCharacterToNumber(name));
		}
		else {
			// Invalid symbol
			return false;
		}
		// Valid symbol
		return true;
	}
	else if (memcmp(name, "nplayersround", 13) == 0) {
		if (memcmp(name, "nplayersround", 13) == 0 && strlen(name) == 13) {
			// For current betting round
			*result = nplayersround(p_betround_calculator->betround());
		}
		else if (memcmp(name, "nplayersround", 13) == 0 && strlen(name) == 14) {
			*result = nplayersround(RightDigitCharacterToNumber(name));
		}
		else {
			// Invalid symbol
			return false;
		}
		// Valid symbol
		return true;
	}
	else if (memcmp(name, "prevaction", 10) == 0 && strlen(name) == 10) {
		*result = prevaction();
	}
	else if (memcmp(name, "nbetsround", 10) == 0 && strlen(name) == 10) {
		*result = nbetsround(p_betround_calculator->betround());
	}
	else if (memcmp(name, "nbetsround", 10) == 0 && strlen(name) == 11) {
		*result = nbetsround(RightDigitCharacterToNumber(name));
	}
	else if (memcmp(name, "hi_", 3) == 0) {
		// History symbols
		int	round = RightDigitCharacterToNumber(name);
		CString pure_name_without_betround = name.Mid(3);
		int length = pure_name_without_betround.GetLength();
		pure_name_without_betround.Truncate(length - 1);
		*result = HistorySymbol(pure_name_without_betround, round);
		return true;
	}
	else {
		// Symbol of a different symbol-engine
		return false;
	}
	// Valid symbol
	return true;
}

bool CSymbolEngineHistory::DidAct() {
	// Extra pre-caution for preflop, in case of failed hand-reset,
	// including another extra fail-safe for unknown big-blind
	if ((BETROUND == kBetroundPreflop)
		&& p_engine_container->symbol_engine_userchair()->userchair_confirmed()
		&& ((p_table_state->User()->_bet.GetValue() < p_engine_container->symbol_engine_tablelimits()->bblind())
			|| (p_table_state->User()->_bet.GetValue() == 0))) {
		return false;
	}
	// Otherwise: return "normal" value, depending on didbetsize, didrais, ...
	return DidAct(BETROUND);
}

bool CSymbolEngineHistory::DidAct(int betround) {
	if (!p_engine_container->symbol_engine_userchair()->userchair_confirmed()) {
		return false;
	}
	// Considering fold or allin too. It's unneccessary for bot logic, but usefull for lazy scraping.
	if (didchec(betround) || didcall(betround)
		|| didswag(betround) || didrais(betround)
		|| didfold(betround) || didalli(betround)) {
		return true;
	}
	// Considering my bet, in case the user acts manually
	// or we cinnect in the middle of a hand.
	// Ignoring limped pots preflop
	double currentbet = p_table_state->User()->_bet.GetValue();
	if (currentbet > p_engine_container->symbol_engine_tablelimits()->bblind()) {
		return true;
	}
	if ((p_betround_calculator->betround() > kBetroundPreflop)
		&& (currentbet > 0)) {
		return true;
	}
	return false;
}

bool CSymbolEngineHistory::DidFoldThisHand() {
	return (didfold(kBetroundPreflop)
		|| didfold(kBetroundFlop)
		|| didfold(kBetroundTurn)
		|| didfold(kBetroundRiver));
}

CString CSymbolEngineHistory::SymbolsProvided() {
	CString list = "didchec didcall didrais didbetsize didfold didalli "
		"nplayersround nbetsround prevaction ";
	list += RangeOfSymbols("didchecround%i", kBetroundPreflop, kBetroundRiver);
	list += RangeOfSymbols("didcallround%i", kBetroundPreflop, kBetroundRiver);
	list += RangeOfSymbols("didraisround%i", kBetroundPreflop, kBetroundRiver);
	list += RangeOfSymbols("didbetsizeround%i", kBetroundPreflop, kBetroundRiver);
	list += RangeOfSymbols("nplayersround%i", kBetroundPreflop, kBetroundRiver);
	list += RangeOfSymbols("nbetsround%i", kBetroundPreflop, kBetroundRiver);
	for (int i = 0; i<24; /*!!!k_hist_sym_count*/ ++i) {
		CString new_history_symbol;
		new_history_symbol.Format("hi_%s%%i", k_hist_sym_strings[i]);
		list += RangeOfSymbols(new_history_symbol, kBetroundPreflop, kBetroundRiver);
	}
	return list;
}