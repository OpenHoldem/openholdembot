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
#include "CTableState.h"
#include "CSymbolEngineRange.h"
#include "CIteratorThread.h"
#include "CBetroundCalculator.h"
#include "CEngineContainer.h"
#include "CardFunctions.h"
#include "CFunction.h"
#include "CFunctionCollection.h"
#include "COHScriptList.h"
#include "CSymbolEngineMemorySymbols.h"
#include "CParseErrors.h"
#include <regex>
#include <string>

using namespace std;

sprw1326*	prw1326;	//prwin 1326 data structure Matrix 2008-04-29
const int suit[4] = { 0, 13, 26, 39 };

CSymbolEngineRange::CSymbolEngineRange() {
	// The values of some symbol-engines depend on other engines.
	// As the engines get later called in the order of initialization
	// we assure correct ordering by checking if they are initialized.
	//
	// This engine does not use any other engines.
	prw1326 = p_iterator_thread->prw1326();
	_range_text = "";
}

CSymbolEngineRange::~CSymbolEngineRange() {
}

void CSymbolEngineRange::InitOnStartup() {
	UpdateOnConnection();
}

void CSymbolEngineRange::UpdateOnConnection() {
	UpdateOnHandreset();
}

void CSymbolEngineRange::UpdateOnHandreset() {
	ClearAll();
	UnSetLists();
	// For testing purpose only, do not uncomment in production
	//prw1326->useme = 1326;
	//prw1326->preflop = 1326;
}

void CSymbolEngineRange::UpdateOnNewRound() {
}

void CSymbolEngineRange::UpdateOnMyTurn() {
}

void CSymbolEngineRange::UpdateOnHeartbeat() {
}

void CSymbolEngineRange::ClearAll() {
	for (int i = 0; i < kMaxNumberOfPlayers; ++i) {
		_range[i] = "";
		for (int j = 0; j < k_number_of_ranks_per_deck; ++j) {
			for (int k = 0; k < k_number_of_ranks_per_deck; ++k) {
				_range_matrix[i][j][k] = false;
				_range_weight_matrix[i][j][k] = 100;
			}
		}
	}
}

// First_rank, second_rank  don't matter for suitedness here.
// Suit gets controlled by parameter 3: "suited"
void CSymbolEngineRange::Set(int first_rank, int second_rank, bool suited, int weight) {
	AssertRange(first_rank, 2, k_rank_ace);
	AssertRange(second_rank, 2, k_rank_ace);
	// Higher rank first
	if (first_rank < second_rank) {
		SwapInts(&first_rank, &second_rank);
	}
	if (suited) {
		Set(first_rank, second_rank, weight);
	}
	else {
		Set(second_rank, first_rank, weight);
	}
}

// first_rank  > second_rank -> suited
// second_rank > first_rank  -> offsuited
void CSymbolEngineRange::Set(int first_rank, int second_rank, int weight) {
	AssertRange(first_rank, 2, k_rank_ace);
	AssertRange(second_rank, 2, k_rank_ace);
	// Normating to [0..12]
	first_rank -= 2;
	second_rank -= 2;
	// Needs to be [second_rank][first_rank]
	// for correct view in handlist-editor,
	// otherwise suited/offsuited would be confused
	// and we need a single point to get it right
	_range_matrix[_chairnumber][second_rank][first_rank] = true;
	_range_weight_matrix[_chairnumber][second_rank][first_rank] = weight;
}

// first_rank  > second_rank -> suited
// second_rank > first_rank  -> offsuited
void CSymbolEngineRange::Unset(int first_rank, int second_rank, bool suited) {
	AssertRange(first_rank, 2, k_rank_ace);
	AssertRange(second_rank, 2, k_rank_ace);
	// Normating to [0..12]
	first_rank -= 2;
	second_rank -= 2;
	// Needs to be [second_rank][first_rank] if suited
	// for correct view in handlist-editor,
	// otherwise suited/offsuited would be confused
	// and we need a single point to get it right
	if (suited)
		SwapInts(&first_rank, &second_rank);
	_range_matrix[_chairnumber][first_rank][second_rank] = false;
	_range_weight_matrix[_chairnumber][first_rank][second_rank] = 100;
}

void CSymbolEngineRange::UnSetLists() {
	for (int i = 0; i < 10; i++) {
		prw1326->chair[i].limit = 0;
		prw1326->chair[i].level = 100;
		prw1326->chair[i].ignore = 0;

		for (int ia = 0; ia < 1326; ia++) {
			prw1326->chair[i].ranklo[ia] = 0;
			prw1326->chair[i].rankhi[ia] = 0;
			prw1326->chair[i].weight[ia] = 100;
		}
	}
}

void CSymbolEngineRange::UnSetChairList(int chairnumber) {
	prw1326->chair[chairnumber].limit = 0;
	prw1326->chair[chairnumber].level = 100;
	prw1326->chair[chairnumber].ignore = 0;

	for (int ia = 0; ia < 1326; ia++) {
		prw1326->chair[chairnumber].ranklo[ia] = 0;
		prw1326->chair[chairnumber].rankhi[ia] = 0;
		prw1326->chair[chairnumber].weight[ia] = 100;
	}
	_range[chairnumber] = "";
	for (int j = 0; j < k_number_of_ranks_per_deck; ++j) {
		for (int k = 0; k < k_number_of_ranks_per_deck; ++k) {
			_range_matrix[chairnumber][j][k] = false;
			_range_weight_matrix[chairnumber][j][k] = 100;
		}
	}
}

void CSymbolEngineRange::cards_removal(int chairnumber, int r1, int r2, int s1, int s2)
{
	// Normating to [0..12]
	r1 -= 2;
	r2 -= 2;
	int c1 = r1 + s1;
	int c2 = r2 + s2;
	int limit = prw1326->chair[chairnumber].limit;

	for (int p = 0; p < limit; p++)
	{
		if (prw1326->chair[chairnumber].ranklo[p] == c1 || prw1326->chair[chairnumber].rankhi[p] == c1) {
			for (int i = p; i < limit; i++)
			{
				if (i == limit - 1) {
					prw1326->chair[chairnumber].ranklo[i] = 0;
					prw1326->chair[chairnumber].rankhi[i] = 0;
					prw1326->chair[chairnumber].weight[i] = prw1326->chair[chairnumber].level;
				}
				else {
					prw1326->chair[chairnumber].ranklo[i] = prw1326->chair[chairnumber].ranklo[i + 1];
					prw1326->chair[chairnumber].rankhi[i] = prw1326->chair[chairnumber].rankhi[i + 1];
					prw1326->chair[chairnumber].weight[i] = prw1326->chair[chairnumber].weight[i + 1];
				}
			}
			limit = prw1326->chair[chairnumber].limit--;
			p--;
		}
	}

	for (int p = 0; p < limit; p++)
	{
		if (prw1326->chair[chairnumber].ranklo[p] == c2 || prw1326->chair[chairnumber].rankhi[p] == c2) {
			for (int i = p; i < limit; i++)
			{
				if (i == limit - 1) {
					prw1326->chair[chairnumber].ranklo[i] = 0;
					prw1326->chair[chairnumber].rankhi[i] = 0;
					prw1326->chair[chairnumber].weight[i] = prw1326->chair[chairnumber].level;
				}
				else {
					prw1326->chair[chairnumber].ranklo[i] = prw1326->chair[chairnumber].ranklo[i + 1];
					prw1326->chair[chairnumber].rankhi[i] = prw1326->chair[chairnumber].rankhi[i + 1];
					prw1326->chair[chairnumber].weight[i] = prw1326->chair[chairnumber].weight[i + 1];
				}
			}
			limit = prw1326->chair[chairnumber].limit--;
			p--;
		}
	}
}

void CSymbolEngineRange::del_single_hand(int chairnumber, int r1, int r2, int s1, int s2)
{
	// Normating to [0..12]
	r1 -= 2;
	r2 -= 2;
	int c1 = r1 + s1;
	int c2 = r2 + s2;
	int limit = prw1326->chair[chairnumber].limit;
	int found_num = -1;

	for (int p = 0; p < limit; p++)
	{
		if (prw1326->chair[chairnumber].ranklo[p] == c1 && prw1326->chair[chairnumber].rankhi[p] == c2 || prw1326->chair[chairnumber].ranklo[p] == c2 && prw1326->chair[chairnumber].rankhi[p] == c1)
		{
			found_num = p;
			break;
		}
	}
	for (int i = found_num; i < limit; i++)
	{
		if (i == limit - 1) {
			prw1326->chair[chairnumber].ranklo[i] = 0;
			prw1326->chair[chairnumber].rankhi[i] = 0;
			prw1326->chair[chairnumber].weight[i] = prw1326->chair[chairnumber].level;
		}
		else {
			prw1326->chair[chairnumber].ranklo[i] = prw1326->chair[chairnumber].ranklo[i + 1];
			prw1326->chair[chairnumber].rankhi[i] = prw1326->chair[chairnumber].rankhi[i + 1];
			prw1326->chair[chairnumber].weight[i] = prw1326->chair[chairnumber].weight[i + 1];
		}
	}

	prw1326->chair[chairnumber].limit--;
}

void CSymbolEngineRange::del_pocketpair_hand(int chairnumber, int r1)
{
	/*
	remove pocket pair
	*/
	for (int i = 0; i<3; i++) for (int j = i + 1; j<4; j++)
		del_single_hand(chairnumber, r1, r1, suit[i], suit[j]);
}

void CSymbolEngineRange::del_offsuited_hand(int chairnumber, int r1, int r2)
{
	/*
	remove off suited hand
	*/
	for (int i = 0; i<4; i++) for (int j = 0; j<4; j++) if (j != i)
		del_single_hand(chairnumber, r1, r2, suit[i], suit[j]);
}

void CSymbolEngineRange::del_suited_hand(int chairnumber, int r1, int r2)
{
	/*
	remove suited hand
	*/
	for (int i = 0; i<4; i++)
		del_single_hand(chairnumber, r1, r2, suit[i], suit[i]);
}

void CSymbolEngineRange::add_single_hand(int chairnumber, int r1, int r2, int s1, int s2, int weight)
{
	// Normating to [0..12]
	r1 -= 2;
	r2 -= 2;
	int c1 = r1 + s1;
	int c2 = r2 + s2;
	int lim = prw1326->chair[chairnumber].limit;

	if (r1 < r2)
	{
		prw1326->chair[chairnumber].ranklo[lim] = c1;
		prw1326->chair[chairnumber].rankhi[lim] = c2;
	}
	else
	{
		prw1326->chair[chairnumber].ranklo[lim] = c2;
		prw1326->chair[chairnumber].rankhi[lim] = c1;
	}

	prw1326->chair[chairnumber].weight[lim] = weight;
	prw1326->chair[chairnumber].limit++;
}

void CSymbolEngineRange::add_pocketpair_hand(int chairnumber, int r1, int weight)
{
	/*
	add pocket pair
	*/
	for (int i = 0; i<3; i++) for (int j = i + 1; j<4; j++)
		add_single_hand(chairnumber, r1, r1, suit[i], suit[j], weight);
}

void CSymbolEngineRange::add_offsuited_hand(int chairnumber, int r1, int r2, int weight)
{
	/*
	add off suited hand
	*/
	for (int i = 0; i<4; i++) for (int j = 0; j<4; j++) if (j != i)
		add_single_hand(chairnumber, r1, r2, suit[i], suit[j], weight);
}

void CSymbolEngineRange::add_suited_hand(int chairnumber, int r1, int r2, int weight)
{
	/*
	add suited hand
	*/
	for (int i = 0; i<4; i++)
		add_single_hand(chairnumber, r1, r2, suit[i], suit[i], weight);
}

void CSymbolEngineRange::weight_single_hand(int chairnumber, int r1, int r2, int s1, int s2, int old_weight, int new_weight) {
	/*
	changes weight for specified hand with old_weight to new_weight
	*/
	// Normating to [0..12]
	r1 -= 2;
	r2 -= 2;
	int c1 = r1 + s1;
	int c2 = r2 + s2;
	int found_num = -1;

	for (int p = 0; p < prw1326->chair[chairnumber].limit; p++)
	{
		if (prw1326->chair[chairnumber].ranklo[p] == c1 && prw1326->chair[chairnumber].rankhi[p] == c2 || prw1326->chair[chairnumber].ranklo[p] == c2 && prw1326->chair[chairnumber].rankhi[p] == c1)
		{
			found_num = p;
			break;
		}
	}

	if (prw1326->chair[chairnumber].weight[found_num] == old_weight)	prw1326->chair[chairnumber].weight[found_num] = new_weight;
}

void CSymbolEngineRange::weight_pocketpair_hand(int chairnumber, int r1, int old_weight, int new_weight)
{
	/*
	weight pocket pair
	*/

	for (int p = 0; p<3; p++) for (int k = p + 1; k<4; k++)	weight_single_hand(chairnumber, r1, r1, suit[p], suit[k], old_weight, new_weight);
}

void CSymbolEngineRange::weight_offsuited_hand(int chairnumber, int r1, int r2, int old_weight, int new_weight)
{
	/*
	weight off suit hand
	*/

	for (int p = 0; p<4; p++) for (int k = 0; k<4; k++) if (k != p)
		weight_single_hand(chairnumber, r1, r2, suit[p], suit[k], old_weight, new_weight);
}

void CSymbolEngineRange::weight_suited_hand(int chairnumber, int r1, int r2, int old_weight, int new_weight)
{
	/*
	weight suited hand
	*/

	for (int k = 0; k<4; k++) weight_single_hand(chairnumber, r1, r2, suit[k], suit[k], old_weight, new_weight);
}

void CSymbolEngineRange::weight_all_hand(int chairnumber, int old_weight, int new_weight) {
	/*
	changes all hands with old_weight to new_weight
	*/
	int max_card = prw1326->chair[chairnumber].limit;

	for (int z = 0; z < max_card; z++)
	{
		int r1 = prw1326->chair[chairnumber].ranklo[z] % 13;
		int r2 = prw1326->chair[chairnumber].rankhi[z] % 13;
		// Normating to [2..14]
		r1 += 2;
		r2 += 2;
		int s1 = prw1326->chair[chairnumber].ranklo[z] - prw1326->chair[chairnumber].ranklo[z] % 13;
		int s2 = prw1326->chair[chairnumber].rankhi[z] - prw1326->chair[chairnumber].rankhi[z] % 13;

		weight_single_hand(chairnumber, r1, r2, s1, s2, old_weight, new_weight);
	}
}

bool CSymbolEngineRange::SetRange(CString range_symbol, CString range_value) {
	_chairnumber = RightDigitCharacterToNumber(range_symbol);

	int position = 0;
	CString separator = _T(", \n\r");
	CString sep_operator = _T("<>=");
	CString token = "null";
	int command = kSetRange;
	int ioperator = kNoOperator;
	bool weight_all = false;
	double oldweight = kUndefined, newweight = kUndefined;
	double operator_limit;

	if (prw1326->useme != 1326) {
		ErrorPrw1326NotActivated();
		return false;
	}
	if (p_betround_calculator->betround() == kBetroundPreflop && prw1326->preflop != 1326) {
		ErrorPrw1326PreflopNotActivated();
		return false;
	}

	range_value = range_value.Trim();	
	CString range_lower = range_value;						// Syntax
	if (range_lower.Left(3).MakeLower() == "del")			// SET rangeX "del listxyz" (or "del AA:100 KK:94 AKs:88 ...")	
		command = KRemoveRange;
	if (range_lower.MakeLower() == "del_pp")				// SET rangeX "del_pp"
		command = kRemovePocketPair;
	if (range_lower.MakeLower() == "del_os")				// SET rangeX "del_os"
		command = kRemoveOffsuited;
	if (range_lower.MakeLower() == "del_s")					// SET rangeX "del_s"
		command = kRemoveSuited;
	if (range_lower.Left(6).MakeLower() == "weight")		// SET rangeX "weight listabc" (or "weight AA:85 KK:78 AKs:71 ...")
		command = kChangeWeight;
	if (range_lower.Left(10).MakeLower() == "weight_all")	// SET rangeX "weight_all /84 /96" (/oldweight /newweight)
		command = kChangeAllWeight;

	switch (command) {
	case kSetRange:
		UnSetChairList(_chairnumber);
		break;
	case KRemoveRange:
		range_value = range_value.Mid(3).Trim();
		break;
	case kChangeWeight:
		range_value = range_value.Mid(6).Trim();
		break;
	case kChangeAllWeight:
		CString srange_value = range_value;
		range_value = range_value.Mid(10).Trim();
		CString sweight = range_value.Tokenize("/", position).Trim();
		if (sweight == "" || !isNumber(sweight)) {
			ErrorInvalidMember(srange_value);
			return false;
		}
		oldweight = round(StringToNumber(sweight));
		sweight = range_value.Tokenize("/", position).Trim();
		if (sweight == "" || !isNumber(sweight)) {
			ErrorInvalidMember(srange_value);
			return false;
		}
		newweight = round(StringToNumber(sweight));
		range_value = _range[_chairnumber];
		range_value.Replace(Number2CString(oldweight), Number2CString(newweight));
		command = kChangeWeight;
		weight_all = true;
		break;
	}

	if (command == kSetRange) {
		CString list = range_value;
		if (range_value.FindOneOf(sep_operator) != -1) {
			int pos = 0;
			CString limit;
			list = range_value.Tokenize(sep_operator, pos).Trim();
			limit = range_value.Mid(pos).Trim();
			pos = range_value.FindOneOf(sep_operator);
			if (range_value.GetAt(pos) == '>') {
				if (range_value.GetAt(pos + 1) == '=') {
					ioperator = kOperatorSuperiorEqualTo;
					limit.Remove('=');
					pos = pos + 1;
				}
				else
					ioperator = kOperatorSuperiorTo;
			}
			else if (range_value.GetAt(pos) == '<') {
				if (range_value.GetAt(pos + 1) == '=') {
					ioperator = kOperatorInferiorEqualTo;
					limit.Remove('=');
					pos = pos + 1;
				}
				else
					ioperator = kOperatorInferiorTo;
			}
			else if (range_value.GetAt(pos) == '=')
				ioperator = kOperatorEqualTo;
			if (range_value.Mid(pos + 1).FindOneOf(sep_operator) != -1) {
				ErrorInvalidSelectRangeOperator(range_value);
				return false;
			}
			if (limit == "") {
				ErrorInvalidSelectRangeOperator(range_value);
				return false;
			}
			operator_limit = StringToNumber(limit);
			if (!isNumber(limit)) {
				if (limit.Left(2) == "f$") {
					CFunction *function = (CFunction*)p_function_collection->LookUp(limit);
					if (function == NULL) {
						// Function not found
						CString error_message;
						error_message.Format("Function %s not found or mispelled\n"
							"Please set a valid function name.", limit);
						CParseErrors::Error(error_message);
						return false;
					}
					operator_limit = function->Evaluate(limit);
				}
				else if (limit.Left(6) == "me_re_") {
					p_engine_container->symbol_engine_memory_symbols()->EvaluateSymbol(limit, &operator_limit);
					if (operator_limit < 0) {
						// Memory symbol not found
						CString error_message;
						error_message.Format("Memory symbol %s not found or mispelled\n"
							"Please set a valid memory symbol.", limit);
						CParseErrors::Error(error_message);
						return false;
					}
				}
				else {
					ErrorInvalidSelectRangeOperator(range_value);
					return false;
				}
			}

			range_value = list;
		}
	}

	if (range_value.Left(4) == "list") {
		COHScriptList *hand_list = (COHScriptList*)p_function_collection->LookUp(range_value);
		if (hand_list == NULL) {
			// List not found
			CString error_message;
			error_message.Format("List %s not found or mispelled\n"
				"Please set a valid list name.", range_value);
			CParseErrors::Error(error_message);
			return false;
		}
		range_value = hand_list->function_text();
	}

	position = 0;

	while (!token.IsEmpty())
	{
		// Get next token.
		if (command == kSetRange || command == KRemoveRange || command == kChangeWeight || command == kChangeAllWeight)
			token = range_value.Tokenize(separator, position);
		else
			token = _range[_chairnumber].Tokenize(separator, position);

		int token_start = 0;
		double weight = 100;
		CString hand, hand_start, hand_stop;
		char first_rank_string, second_rank_string, first_rank_string2, second_rank_string2;
		int first_rank, second_rank, first_rank2, second_rank2;
		char suit = '\0', suit2 = '\0';
		int length, length2;
		bool open_ended = (token.Find("+") != -1) ? true : false;
		bool hand_group = (token.Find("-") != -1) ? true : false;
		if (open_ended && hand_group) {
			ErrorInvalidMember(token);
			return false;
		}
		length = token.GetLength();
		if (length == 0)
			break;
		if (open_ended)
			token.Remove('+');
		if (hand_group) {
			hand_start = token.Tokenize("-", token_start);
			hand_stop = token.Mid(token_start);
		}
		hand = token.Tokenize(":", token_start);
		if (token.Find(":") != -1) {
			CString sweight = token.Mid(token_start);
			if (sweight == "") {
				ErrorInvalidMember(token);
				return false;
			}
			weight = round(StringToNumber(sweight));
		}
		length = hand.GetLength();
		if ((length < 2) || (length > 3) || (weight < 0) || (weight > 100)) {
			ErrorInvalidMember(token);
			return false;
		}
		if ((ioperator == kOperatorEqualTo && weight != round(operator_limit)) ||
			(ioperator == kOperatorSuperiorTo && weight <= round(operator_limit)) ||
			(ioperator == kOperatorSuperiorEqualTo && weight < round(operator_limit)) ||
			(ioperator == kOperatorInferiorTo && weight >= round(operator_limit)) ||
			(ioperator == kOperatorInferiorEqualTo && weight > round(operator_limit)))
			goto ScanForNextToken;
				
		if (hand_group) {
			length2 = hand_start.GetLength();
			if (length != length2) {
				ErrorInvalidMember(token);
				return false;
			}
			first_rank_string2 = tolower(hand_start.GetAt(0));
			second_rank_string2 = tolower(hand_start.GetAt(1));
			first_rank2 = CardRankCharacterToCardRank(first_rank_string2);
			second_rank2 = CardRankCharacterToCardRank(second_rank_string2);
			if ((first_rank2 < 0) || (second_rank2 < 0)) {
				ErrorInvalidMember(token);
				return false;
			}
		}
		first_rank_string = tolower(hand.GetAt(0));
		second_rank_string = tolower(hand.GetAt(1));
		first_rank = CardRankCharacterToCardRank(first_rank_string);
		second_rank = CardRankCharacterToCardRank(second_rank_string);
		if ((first_rank < 0) || (second_rank < 0)) {
			ErrorInvalidMember(token);
			return false;
		}
		if (length == 2) {
			if (!hand_group && (first_rank == second_rank)) {
				switch (command) {
				case kSetRange:
					if (open_ended) {
						for (int i = k_rank_ace; i >= first_rank; i--) {
							add_pocketpair_hand(_chairnumber, i, weight);
							Set(i, i, false, weight);
						}
					}
					else {
						add_pocketpair_hand(_chairnumber, first_rank, weight);
						Set(first_rank, second_rank, false, weight);
					}
					break;
				case kChangeWeight:
					if (open_ended) {
						for (int i = k_rank_ace; i >= first_rank; i--) {
							if (!weight_all) {
								if (_range_matrix[_chairnumber][i - 2][i - 2] == true)
									oldweight = _range_weight_matrix[_chairnumber][i - 2][i - 2];
								else continue;
							}
							if (_range_weight_matrix[_chairnumber][i - 2][i - 2] == oldweight) {
								weight_pocketpair_hand(_chairnumber, i, oldweight, weight);
								Set(i, i, false, weight);
							}
						}
					}
					else {
						if (!weight_all) {
							if (_range_matrix[_chairnumber][second_rank - 2][first_rank - 2] == true)
								oldweight = _range_weight_matrix[_chairnumber][second_rank - 2][first_rank - 2];
							else continue;
						}
						if (_range_weight_matrix[_chairnumber][second_rank - 2][first_rank - 2] == oldweight) {
							weight_pocketpair_hand(_chairnumber, first_rank, oldweight, weight);
							Set(first_rank, second_rank, false, weight);
						}
					}
					break;
				case KRemoveRange:
					if (open_ended) {
						for (int i = k_rank_ace; i >= first_rank; i--) {
							del_pocketpair_hand(_chairnumber, i);
							Unset(i, i);
						}
					}
					else {
						del_pocketpair_hand(_chairnumber, first_rank);
						Unset(first_rank, second_rank);
					}
					break;
				case kRemovePocketPair:
					del_pocketpair_hand(_chairnumber, first_rank);
					Unset(first_rank, second_rank);
					break;
				}
			}
			else
				if (hand_group && (first_rank == second_rank) && (first_rank2 == second_rank2)) {
					if (first_rank > first_rank2)
						SwapInts(&first_rank, &first_rank2);
					switch (command) {
					case kSetRange:
						for (int i = first_rank2; i >= first_rank; i--) {
							add_pocketpair_hand(_chairnumber, i, weight);
							Set(i, i, false, weight);
						}
						break;
					case kChangeWeight:
						for (int i = first_rank2; i >= first_rank; i--) {
							if (!weight_all) {
								if (_range_matrix[_chairnumber][i - 2][i - 2] == true)
									oldweight = _range_weight_matrix[_chairnumber][i - 2][i - 2];
								else continue;
							}
							if (_range_weight_matrix[_chairnumber][i - 2][i - 2] == oldweight) {
								weight_pocketpair_hand(_chairnumber, i, oldweight, weight);
								Set(i, i, false, weight);
							}
						}
						break;
					case KRemoveRange:
						for (int i = first_rank2; i >= first_rank; i--) {
							del_pocketpair_hand(_chairnumber, i);
							Unset(i, i);
						}
						break;
					case kRemovePocketPair:
						del_pocketpair_hand(_chairnumber, first_rank);
						Unset(first_rank, second_rank);
						break;
					}
				}
			else {
				ErrorOldStyleFormat(token);
				return false;
			}
		}
		else if (length == 3) {
			// Higher rank first
			if (hand_group) {
				if (first_rank < second_rank) {
					SwapInts(&first_rank, &second_rank);
				}
				if (first_rank2 < second_rank2) {
					SwapInts(&first_rank2, &second_rank2);
				}
				if (first_rank2 < first_rank) {
					SwapInts(&first_rank2, &first_rank);
					SwapInts(&second_rank2, &second_rank);
				}
				if (second_rank2 < second_rank)
					ErrorInvalidMember(token);
				if (first_rank2 > first_rank)
					if (!((first_rank2 - second_rank2 == first_rank - second_rank) && (first_rank2 - second_rank2 <= 3)))
						ErrorInvalidMember(token);
			}
			else if (first_rank < second_rank) {
				SwapInts(&first_rank, &second_rank);
			}
			suit = tolower(hand.GetAt(2));
			if (hand_group)
				suit2 = tolower(hand_start.GetAt(2));
			if (hand_group && suit != suit2) {
				ErrorInvalidMember(token);
				return false;
			}
			if (suit == 'o') {
				switch (command) {
				case kSetRange:
					if (open_ended) {
						for (int i = first_rank - 1; i >= second_rank; i--) {
							add_offsuited_hand(_chairnumber, first_rank, i, weight);
							Set(first_rank, i, false, weight);
						}
					}
					else if (hand_group) {
						if (first_rank2 == first_rank) {
							for (int i = second_rank2; i >= second_rank; i--) {
								add_offsuited_hand(_chairnumber, first_rank, i, weight);
								Set(first_rank, i, false, weight);
							}
						}
						if (first_rank2 > first_rank) {
							int j = second_rank2;
							for (int i = first_rank2; i >= first_rank; i--) {
								add_offsuited_hand(_chairnumber, i, j, weight);
								Set(i, j--, false, weight);
							}
						}
					}
					else {
						add_offsuited_hand(_chairnumber, first_rank, second_rank, weight);
						Set(first_rank, second_rank, false, weight);
					}
					break;
				case kChangeWeight:
					if (open_ended) {
						for (int i = first_rank - 1; i >= second_rank; i--) {
							if (!weight_all) {
								if (_range_matrix[_chairnumber][first_rank - 2][i - 2] == true)
									oldweight = _range_weight_matrix[_chairnumber][first_rank - 2][i - 2];
								else continue;
							}
							if (_range_weight_matrix[_chairnumber][first_rank - 2][i - 2] == oldweight) {
								weight_offsuited_hand(_chairnumber, first_rank, i, oldweight, weight);
								Set(first_rank, i, false, weight);
							}
						}
					}
					else if (hand_group) {
						if (first_rank2 == first_rank) {
							for (int i = second_rank2; i >= second_rank; i--) {
								if (!weight_all) {
									if (_range_matrix[_chairnumber][first_rank - 2][i - 2] == true)
										oldweight = _range_weight_matrix[_chairnumber][first_rank - 2][i - 2];
									else continue;
								}
								if (_range_weight_matrix[_chairnumber][first_rank - 2][i - 2] == oldweight) {
									weight_offsuited_hand(_chairnumber, first_rank, i, oldweight, weight);
									Set(first_rank, i, false, weight);
								}
							}
						}
						if (first_rank2 > first_rank) {
							int j = second_rank2 + 1;
							for (int i = first_rank2; i >= first_rank; i--) {
								j--;
								if (!weight_all) {
									if (_range_matrix[_chairnumber][i - 2][j - 2] == true)
										oldweight = _range_weight_matrix[_chairnumber][i - 2][j - 2];
									else continue;
								}
								if (_range_weight_matrix[_chairnumber][i - 2][j - 2] == oldweight) {
									weight_offsuited_hand(_chairnumber, i, j, oldweight, weight);
									Set(i, j, false, weight);
								}
							}
						}
					}
					else {
						if (!weight_all) {
							if (_range_matrix[_chairnumber][first_rank - 2][second_rank - 2] == true)
								oldweight = _range_weight_matrix[_chairnumber][first_rank - 2][second_rank - 2];
							else continue;
						}
						if (_range_weight_matrix[_chairnumber][first_rank - 2][second_rank - 2] == oldweight) {
							weight_offsuited_hand(_chairnumber, first_rank, second_rank, oldweight, weight);
							Set(first_rank, second_rank, false, weight);
						}
					}
					break;
				case KRemoveRange:
					if (open_ended) {
						for (int i = first_rank - 1; i >= second_rank; i--) {
							del_offsuited_hand(_chairnumber, first_rank, i);
							Unset(first_rank, i, false);
						}
					}
					else if (hand_group) {
						if (first_rank2 == first_rank) {
							for (int i = second_rank2; i >= second_rank; i--) {
								del_offsuited_hand(_chairnumber, first_rank, i);
								Unset(first_rank, i, false);
							}
						}
						if (first_rank2 > first_rank) {
							int j = second_rank2;
							for (int i = first_rank2; i >= first_rank; i--) {
								del_offsuited_hand(_chairnumber, i, j);
								Unset(i, j--, false);
							}
						}
					}
					else {
						del_offsuited_hand(_chairnumber, first_rank, second_rank);
						Unset(first_rank, second_rank, false);
					}
					break;
				case kRemoveOffsuited:
					del_offsuited_hand(_chairnumber, first_rank, second_rank);
					Unset(first_rank, second_rank, false);
					break;
				}
			}
			else if (suit == 's') {
				switch (command) {
				case kSetRange:
					if (open_ended) {
						for (int i = first_rank - 1; i >= second_rank; i--) {
							add_suited_hand(_chairnumber, first_rank, i, weight);
							Set(first_rank, i, true, weight);
						}
					}
					else if (hand_group) {
						if (first_rank2 == first_rank) {
							for (int i = second_rank2; i >= second_rank; i--) {
								add_suited_hand(_chairnumber, first_rank, i, weight);
								Set(first_rank, i, true, weight);
							}
						}
						if (first_rank2 > first_rank) {
							int j = second_rank2;
							for (int i = first_rank2; i >= first_rank; i--) {
								add_suited_hand(_chairnumber, i, j, weight);
								Set(i, j--, true, weight);
							}
						}
					}
					else {
						add_suited_hand(_chairnumber, first_rank, second_rank, weight);
						Set(first_rank, second_rank, true, weight);
					}
					break;
				case kChangeWeight:
					if (open_ended) {
						for (int i = first_rank - 1; i >= second_rank; i--) {
							if (!weight_all) {
								if (_range_matrix[_chairnumber][i - 2][first_rank - 2] == true)
									oldweight = _range_weight_matrix[_chairnumber][i - 2][first_rank - 2];
								else continue;
							}
							if (_range_weight_matrix[_chairnumber][i - 2][first_rank - 2] == oldweight) {
								weight_suited_hand(_chairnumber, first_rank, i, oldweight, weight);
								Set(first_rank, i, true, weight);
							}
						}
					}
					else if (hand_group) {
						if (first_rank2 == first_rank) {
							for (int i = second_rank2; i >= second_rank; i--) {
								if (!weight_all) {
									if (_range_matrix[_chairnumber][i - 2][first_rank - 2] == true)
										oldweight = _range_weight_matrix[_chairnumber][i - 2][first_rank - 2];
									else continue;
								}
								if (_range_weight_matrix[_chairnumber][i - 2][first_rank - 2] == oldweight) {
									weight_suited_hand(_chairnumber, first_rank, i, oldweight, weight);
									Set(first_rank, i, true, weight);
								}
							}
						}
						if (first_rank2 > first_rank) {
							int j = second_rank2 + 1;
							for (int i = first_rank2; i >= first_rank; i--) {
								j--;
								if (!weight_all) {
									if (_range_matrix[_chairnumber][j - 2][i - 2] == true)
										oldweight = _range_weight_matrix[_chairnumber][j - 2][i - 2];
									else continue;
								}
								if (_range_weight_matrix[_chairnumber][j - 2][i - 2] == oldweight) {
									weight_suited_hand(_chairnumber, i, j, oldweight, weight);
									Set(i, j, true, weight);
								}
							}
						}
					}
					else {
						if (!weight_all) {
							if (_range_matrix[_chairnumber][second_rank - 2][first_rank - 2] == true)
								oldweight = _range_weight_matrix[_chairnumber][second_rank - 2][first_rank - 2];
							else continue;
						}
						if (_range_weight_matrix[_chairnumber][second_rank - 2][first_rank - 2] == oldweight) {
							weight_suited_hand(_chairnumber, first_rank, second_rank, oldweight, weight);
							Set(first_rank, second_rank, true, weight);
						}
					}
					break;
				case KRemoveRange:
					if (open_ended) {
						for (int i = first_rank - 1; i >= second_rank; i--) {
							del_suited_hand(_chairnumber, first_rank, i);
							Unset(first_rank, i);
						}
					}
					else if (hand_group) {
						if (first_rank2 == first_rank) {
							for (int i = second_rank2; i >= second_rank; i--) {
								del_suited_hand(_chairnumber, first_rank, i);
								Unset(first_rank, i);
							}
						}
						if (first_rank2 > first_rank) {
							int j = second_rank2;
							for (int i = first_rank2; i >= first_rank; i--) {
								del_suited_hand(_chairnumber, i, j);
								Unset(i, j--);
							}
						}
					}
					else {
						del_suited_hand(_chairnumber, first_rank, second_rank);
						Unset(first_rank, second_rank);
					}
					break;
				case kRemoveSuited:
					del_suited_hand(_chairnumber, first_rank, second_rank);
					Unset(first_rank, second_rank);
					break;
				}
			}
			else {
				ErrorInvalidMember(token);
				return false;
			}
		}
		else {
			ErrorInvalidMember(token);
			return false;
		}

		// "Ugly" goto to directly jump to scan of next token
		ScanForNextToken:
			continue;
	}
	/*
	// hole cards removal
	if (command == kSetRange) {
		int	plcardrank[2] = { 0 }, plcardsuit[2] = { 0 };
		// playercards
		plcardrank[0] = p_table_state->User()->hole_cards(0)->GetOpenHoldemRank();
		plcardrank[1] = p_table_state->User()->hole_cards(1)->GetOpenHoldemRank();
		plcardsuit[0] = p_table_state->User()->hole_cards(0)->GetSuit();
		plcardsuit[1] = p_table_state->User()->hole_cards(1)->GetSuit();
		if (plcardrank[1] > plcardrank[0]) {
			SwapInts(&plcardrank[0], &plcardrank[1]);
			SwapInts(&plcardsuit[0], &plcardsuit[1]);
		}
		cards_removal(_chairnumber, plcardrank[0], plcardrank[1], suit[plcardsuit[0]], suit[plcardsuit[1]]);
	}
	*/
	if (NHandsOnRange() < 0 || NHandsOnRange() > 169) {
		ErrorInvalidFormat();
		return false;
	}
	GenerateRangeTextFromRangeMatrix();
	_range[_chairnumber] = _range_text.Trim();
	return true;
}

int CSymbolEngineRange::NHandsOnRange() {
	int result = 0;
	for (int i = 2; i <= k_rank_ace; ++i) {
		for (int j = 2; j <= k_rank_ace; ++j) {
			if (_range_matrix[_chairnumber][i][j]) {
				result++;
			}
		}
	}
	return result;
}

bool CSymbolEngineRange::IsOnRange(int first_rank, int second_rank, bool suited) {
	AssertRange(first_rank, 2, k_rank_ace);
	AssertRange(second_rank, 2, k_rank_ace);
	// Higher rank first
	if (first_rank < second_rank) {
		SwapInts(&first_rank, &second_rank);
	}
	if (suited) {
		return IsOnRange(first_rank, second_rank);
	}
	else {
		return IsOnRange(second_rank, first_rank);
	}
}

bool CSymbolEngineRange::IsOnRange(int first_rank, int second_rank) {
	AssertRange(first_rank, 2, k_rank_ace);
	AssertRange(second_rank, 2, k_rank_ace);
	// Normating to [0..12]
	first_rank -= 2;
	second_rank -= 2;
	_weight = _range_weight_matrix[_chairnumber][second_rank][first_rank];
	return _range_matrix[_chairnumber][second_rank][first_rank];
};

void CSymbolEngineRange::GenerateRangeTextFromRangeMatrix() {
	CString result;
	// Pairs
	bool any_hand_added = false;
	for (int i = k_rank_ace; i >= 2; --i) {
		if (IsOnRange(i, i)) {
			result += k_card_chars[i - 2];
			result += k_card_chars[i - 2];
			result += ":" + Number2CString(_weight);
			result += " ";
			any_hand_added = true;
		}
	}
	if (any_hand_added) {
		result += "\n";
	}
	// Suited
	any_hand_added = false;
	for (int i = k_rank_ace; i >= 2; --i) {
		for (int j = i - 1; j >= 2; --j) {
			if (IsOnRange(i, j, true)) {
				result += k_card_chars[i - 2];
				result += k_card_chars[j - 2];
				result += "s:" + Number2CString(_weight);
				result += " ";
				any_hand_added = true;
			}
		}
	}
	if (any_hand_added) {
		result += "\n";
	}
	// Offsuited
	any_hand_added = false;
	for (int i = k_rank_ace; i >= 2; --i) {
		for (int j = i - 1; j >= 2; --j) {
			if (IsOnRange(i, j, false)) {
				result += k_card_chars[i - 2];
				result += k_card_chars[j - 2];
				result += "o:" + Number2CString(_weight);
				result += " ";
				any_hand_added = true;
			}
		}
	}
	if (any_hand_added) {
		result += "\n";
	}
	_range_text = result;
}

void CSymbolEngineRange::ErrorPrw1326NotActivated() {
	CString error_message = "Prw1326 is not activated\n"
		"To enable it use: SET prw1326_useme \"true\" command";
	CParseErrors::Error(error_message);
}

void CSymbolEngineRange::ErrorPrw1326PreflopNotActivated() {
	CString error_message = "Prw1326 for Preflop is not activated\n"
		"To enable it use: SET prw1326_usepreflop \"true\" command";
	CParseErrors::Error(error_message);
}

void CSymbolEngineRange::ErrorInvalidSelectRangeOperator(CString select_range_command) {
	CString error_message;
	error_message.Format("Invalid select range operator: %s\n"
		"Valid commands look like: SET range0 \"QQ-TT:85, AJo-63o:46, 94o+:33 = 85\"\n"
		"Or SET range_dealerchair \"listXYZ > 45\" (fixed values between 0-100)\n"
		"Or SET range0 \"listXYZ <= me_re_Limit\" (0-100 memory symbol values)\n"
		"Or SET range0 \"listXYZ >= f$ABC\" (0-100 function values).",
		select_range_command);
	CParseErrors::Error(error_message);
}

void CSymbolEngineRange::ErrorInvalidMember(CString range_member) {
	CString error_message;
	error_message.Format("Invalid range command or member %s\n"
    "Valid are commands look like \"del listxyz\" or \"del AA:100 KK:94 AKs:88\"\n"
	"Or \"weight listabc\" or \"weight AA:85 KK:78 AKs:71\"\n"
	"Or \"weight_all /84 /96\" (/oldweight /newweight)\n"
	"Or \"del_pp\" or \"del_os\" or \"del_s\"\n"
    "Valid are members look like \"AA AKs ATo 22\"\n"
	"Or like \"AA, AKs ATo,22\" (with ',' separator you can mix with space)\n"
	"Or like \"AA:100 AKs:88 ATo:75 22:0\" (with 0-100 weights)\n"
	"Or mix both weighted and non-weighted entries\n"
	"Or like \"TT+:85 T6s+:33, 94o+\" (for open-ended range)\n"
	"Or like \"QQ-99:92,T7s-T3s, T7o-T3o:21\" (for handrange group)\n"
	"Or like \"KJs-86s:66, AJo-63o:63\" (for one or two gapper group)\n"
		"But you can't mix open-ended and handrange group.",
		range_member);
	CParseErrors::Error(error_message);
}

void CSymbolEngineRange::ErrorOldStyleFormat(CString token) {
	CString error_message;
	error_message.Format("Old style range format detected: %s\n"
		"Use AKs for suited hands, AKo for off-suited, AKs AKo for both.",
		token);
	CParseErrors::Error(error_message);
}

void CSymbolEngineRange::ErrorInvalidFormat() {
	CString error_message = "Invalid range format:\n"
		"Range must have between 0 to 169 entries.";
	CParseErrors::Error(error_message);
}

bool CSymbolEngineRange::EvaluateSymbol(const CString name, CString *result, bool log /* = false */) {
	FAST_EXIT_ON_OPENPPL_SYMBOLS(name);
	if (memcmp(name, "range", 5) == 0 && strlen(name) == 6) {
		*result = range(RightDigitCharacterToNumber(name));
		return true;
	}
	else if (name == kEmptyExpression_False_Zero_WhenOthersFoldForce) {
		*result = "";
		return true;
	}
	else if ((memcmp(name, kEmptyExpression_False_Zero_WhenOthersFoldForce, strlen(kEmptyExpression_False_Zero_WhenOthersFoldForce)) == 0)
		&& (strlen(name) == strlen(kEmptyExpression_False_Zero_WhenOthersFoldForce))) {
		*result = "";
		return true;
	}
	// Symbol of a different symbol-engine
	return false;
}

CString CSymbolEngineRange::SymbolsProvided() {
	CString list = "";
	list += RangeOfSymbols("range%i", kFirstChair, kLastChair);
	return list;
}
