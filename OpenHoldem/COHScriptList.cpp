#include "stdafx.h"
#include "COHScriptList.h"

#include "CardFunctions.h"
#include "CEngineContainer.h"
#include "CFormulaParser.h"
#include "CMemoryPool.h"
#include "CParseErrors.h"

#include "CSymbolEngineCards.h"
#include "CSymbolEnginePokerval.h"
#include "CTableState.h"


COHScriptList::COHScriptList(
    CString new_name,
    CString new_function_text) : COHScriptObject(new_name, new_function_text, kNoSourceFileForThisCode, kUndefinedZero) {
  COHScriptList(new_name, new_function_text, kNoSourceFileForThisCode, kUndefinedZero);
}

COHScriptList::COHScriptList(
    CString new_name, 
    CString new_function_text,
    CString file_path,
    int absolute_line) : COHScriptObject(new_name, new_function_text, file_path, absolute_line) {
  Clear();
}

COHScriptList::~COHScriptList() {
}

void COHScriptList::Clear() {
  for (int i=0; i<k_number_of_ranks_per_deck; ++i) {
    for (int j=0; j<k_number_of_ranks_per_deck; ++j) {
      _handlist_matrix[i][j] = false;
	  _handlist_weight_matrix[i][j] = 0;
    }
  }
}

// First_rank, second_rank  don't matter for suitedness here.
// Suit gets controlled by parameter 3: "suited"
void COHScriptList::Set(int first_rank, int second_rank, bool suited, int weight) {
  AssertRange(first_rank, 2, k_rank_ace);
  AssertRange(second_rank, 2, k_rank_ace);
  // Higher rank first
  if (first_rank < second_rank) {
    SwapInts(&first_rank, &second_rank);
  }
  if (suited) {
    Set(first_rank, second_rank, weight);
  } else {
    Set(second_rank, first_rank, weight);
  }
}

// first_rank  > second_rank -> suited
// second_rank > first_rank  -> offsuited
void COHScriptList::Set(int first_rank, int second_rank, int weight) {
  AssertRange(first_rank, 2, k_rank_ace);
  AssertRange(second_rank, 2, k_rank_ace);
  // Normating to [0..12]
  first_rank -= 2;
  second_rank -= 2;
  // Needs to be [second_rank][first_rank]
  // for correct view in handlist-editor,
  // otherwise suited/offsuited would be confused
  // and we need a single point to get it right
  _handlist_matrix[second_rank][first_rank] = true;
  _handlist_weight_matrix[second_rank][first_rank] = weight;
}

// first_rank  > second_rank -> suited
// second_rank > first_rank  -> offsuited
void COHScriptList::Remove(int first_rank, int second_rank) {
  AssertRange(first_rank, 2, k_rank_ace);
  AssertRange(second_rank, 2, k_rank_ace);
  // Normating to [0..12]
  first_rank -= 2;
  // Needs to be [second_rank][first_rank]
  // for correct view in handlist-editor,
  // otherwise suited/offsuited would be confused
  // and we need a single point to get it right
  second_rank -= 2;
  _handlist_matrix[second_rank][first_rank] = false;
  _handlist_weight_matrix[second_rank][first_rank] = 0;
}

bool COHScriptList::IsOnList(int first_rank, int second_rank, bool suited) {
  AssertRange(first_rank, 2, k_rank_ace);
  AssertRange(second_rank, 2, k_rank_ace);
  // Higher rank first
  if (first_rank < second_rank) {
    SwapInts(&first_rank, &second_rank);
  }
  if (suited) {
    return IsOnList(first_rank, second_rank);
  } else {
    return IsOnList(second_rank, first_rank);
  }
}

bool COHScriptList::IsOnList(int first_rank, int second_rank) {
  AssertRange(first_rank, 2, k_rank_ace);
  AssertRange(second_rank, 2, k_rank_ace);
  // Normating to [0..12]
  first_rank -= 2;
  second_rank -= 2;
  if (_handlist_weight_matrix[second_rank][first_rank] >= rand() % 101)
	  return _handlist_matrix[second_rank][first_rank];
  else
	  return false;
};

bool COHScriptList::Set(CString list_member) {
	int token_start = 0;
	int weight = 100;
	CString hand, hand_start, hand_stop;
	char first_rank_string, second_rank_string, first_rank_string2, second_rank_string2;
	int first_rank, second_rank, first_rank2, second_rank2;
	char suit = '\0', suit2 = '\0';
	int length, length2;
	bool open_ended = (list_member.Find("+") != -1) ? true : false;
	bool hand_group = (list_member.Find("-") != -1) ? true : false;
	if (open_ended && hand_group) {
		ErrorInvalidMember(list_member);
		return false;
	}
	if (open_ended)
		list_member.Remove('+');
	if (hand_group) {
		hand_start = list_member.Tokenize("-", token_start);
		hand_stop = list_member.Mid(token_start);
	}
	hand = list_member.Tokenize(":", token_start);
	if (list_member.Find(":") != -1)
		weight = StringToNumber(list_member.Mid(token_start));
	length = hand.GetLength();
  if ((length < 2) || (length > 3) || (weight < 0) || (weight > 100)) {
    ErrorInvalidMember(list_member);
    return false;
  }
  if (hand_group) {
	  length2 = hand_start.GetLength();
	  if (length != length2) {
		  ErrorInvalidMember(list_member);
		  return false;
	  }
	  first_rank_string2 = tolower(hand_start.GetAt(0));
	  second_rank_string2 = tolower(hand_start.GetAt(1));
	  first_rank2 = CardRankCharacterToCardRank(first_rank_string2);
	  second_rank2 = CardRankCharacterToCardRank(second_rank_string2);
	  if ((first_rank2 < 0) || (second_rank2 < 0)) {
		  ErrorInvalidMember(list_member);
		  return false;
	  }
  }
  first_rank_string = tolower(hand.GetAt(0));
  second_rank_string = tolower(hand.GetAt(1));
  first_rank = CardRankCharacterToCardRank(first_rank_string);
  second_rank = CardRankCharacterToCardRank(second_rank_string);
  if ((first_rank < 0) || (second_rank < 0)) {
    ErrorInvalidMember(list_member);
    return false;
  }
  if (length == 2) {
    if (!hand_group && (first_rank_string == second_rank_string)) {
      Set(first_rank, second_rank, false, weight);
	  if (open_ended) {
		  for (int i = first_rank + 1; i <= k_rank_ace; i ++)
			  Set(i, i, false, weight);
	  }
	  return true;
    }
	if (hand_group && (first_rank_string == second_rank_string) && (first_rank_string2 == second_rank_string2)) {
		if (first_rank > first_rank2)
			SwapInts(&first_rank, &first_rank2);
		for (int i = first_rank; i <= first_rank2; i++)
			Set(i, i, false, weight);
		return true;
	}
    ErrorOldStyleFormat(list_member);
    return false;
  }
  assert(length == 3);
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
		  ErrorInvalidMember(list_member);
	  if (first_rank2 > first_rank)
		  if(!((first_rank2 - second_rank2 == first_rank - second_rank) && (first_rank2 - second_rank2 <= 3)))
			  ErrorInvalidMember(list_member);
  }
  else if (first_rank < second_rank) {
	  SwapInts(&first_rank, &second_rank);
  }
  suit = tolower(hand.GetAt(2));
  if (hand_group)
	  suit2 = tolower(hand_start.GetAt(2));
  if (hand_group && suit != suit2) {
	  ErrorInvalidMember(list_member);
	  return false;
  }
  if (suit == 's') {
    Set(first_rank, second_rank, true, weight);
	if (open_ended) {
		for (int i = second_rank + 1; i < first_rank; i++)
			Set(first_rank, i, true, weight);
	}
	if (hand_group) {
		if (first_rank2 == first_rank) {
			for (int i = second_rank + 1; i <= second_rank2; i++)
				Set(first_rank, i, true, weight);
		}
		if (first_rank2 > first_rank) {
			int j = second_rank + 1;
			for (int i = first_rank + 1; i <= first_rank2; i++)
					Set(i, j++, true, weight);
		}
	}
  } else if (suit == 'o') {
    Set(first_rank, second_rank, false, weight);
	if (open_ended) {
		for (int i = second_rank + 1; i < first_rank; i++)
			Set(first_rank, i, false, weight);
	}
	if (hand_group) {
		if (first_rank2 == first_rank) {
			for (int i = second_rank + 1; i <= second_rank2; i++)
				Set(first_rank, i, false, weight);
		}
		if (first_rank2 > first_rank) {
			int j = second_rank + 1;
			for (int i = first_rank + 1; i <= first_rank2; i++)
					Set(i, j++, false, weight);
		}
	}
  } else {
    ErrorInvalidMember(list_member);
    return false;
  }
  return true;
}

int COHScriptList::NHandsOnList() {
  int result = 0;
  for (int i=2; i<=k_rank_ace; ++i) {
    for (int j=2; j<=k_rank_ace; ++j) {
      if (_handlist_matrix[i][j]) {
        result++;
      }
    }
  }
  return result;
}

void COHScriptList::ErrorInvalidMember(CString list_member) {
  CString error_message;
  error_message.Format("Invalid list member %s\n"
    "Valid are entries look like \"AA AKs ATo 22\"\n"
	"Or like \"AA, AKs ATo,22\" (with ',' separator you can mix with space)\n"
	"Or like \"AA:100 AKs:88 ATo:75 22:0\" (with 0-100 weights)\n"
	"Or mix both weighted and non-weighted entries\n"
	"Or like \"TT+:85 T6s+:33, 94o+\" (for open-ended range)\n"
	"Or like \"QQ-99:92,T7s-T3s, T7o-T3o:21\" (for handrange group)\n"
	"Or like \"KJs-86s:66, AJo-63o:63\" (for one or two gapper group)\n"
	"But you can't mix open-ended and handrange group.",
    list_member);
  CParseErrors::Error(error_message);
}

void COHScriptList::ErrorOldStyleFormat(CString list_member) {
  CString error_message;
  error_message.Format("Old style list format detected: %s\n"
    "Use AKs for suited hands, AKo for off-suited, AKs AKo for both.",
    list_member);
  CParseErrors::Error(error_message);
}

double COHScriptList::Evaluate(bool log /* = false */) {
  write_log(Preferences()->debug_formula(), 
    "[COHScriptList] Evaluating list %s\n", _name); 
  if (!p_table_state->User()->HasKnownCards()) return false;
  return IsOnList(p_engine_container->symbol_engine_pokerval()->rankhiplayer(),
    p_engine_container->symbol_engine_pokerval()->rankloplayer(),
    p_engine_container->symbol_engine_cards()->issuited());
}

void COHScriptList::Parse() {
  if (NeedsToBeParsed()) {
    write_log(Preferences()->debug_formula() || Preferences()->debug_parser(),
      "[CFunction] Parsing %s\n", _name);
    p_formula_parser->ParseFormula(this);
    MarkAsParsed();
  }
  else {
    write_log(Preferences()->debug_formula() || Preferences()->debug_parser(),
      "[COHScriptList] No need to parse %s\n", _name);
  }
}

CString COHScriptList::function_text() {
  return _function_text;
}

void COHScriptList::GenerateFunctionTextFromHandlistMatrix() {
  CString result;
  // Pairs
  bool any_hand_added = false;
  for (int i=k_rank_ace; i>=2; --i) {
    if (IsOnList(i, i)) {
      result += k_card_chars[i-2];
      result += k_card_chars[i-2];
      result += " ";
      any_hand_added = true;
    }
  }
  if (any_hand_added) {
    result += "\n";
  }
  // Suited
  any_hand_added = false;
  for (int i=k_rank_ace; i>=2; --i) {
    for (int j=i-1; j>=2; --j) {
      if (IsOnList(i, j, true)) {
        result += k_card_chars[i-2];
        result += k_card_chars[j-2];
        result += "s ";
        any_hand_added = true;
      }
    }
  }
  if (any_hand_added) {
    result += "\n";
  }
  // Offsuited
  any_hand_added = false;
  for (int i=k_rank_ace; i>=2; --i) {
    for (int j=i-1; j>=2; --j) {
      if (IsOnList(i, j, false)) {
        result += k_card_chars[i-2];
        result += k_card_chars[j-2];
        result += "o ";
        any_hand_added = true;
      }
    }
  }
  if (any_hand_added) {
    result += "\n";
  }
  _function_text = result;
} 