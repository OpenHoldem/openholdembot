#include "pch.h"
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
    }
  }
}

// First_rank, second_rank  don't matter for suitedness here.
// Suit gets controlled by parameter 3: "suited"
void COHScriptList::Set(int first_rank, int second_rank, bool suited) {
  AssertRange(first_rank, 2, k_rank_ace);
  AssertRange(second_rank, 2, k_rank_ace);
  // Higher rank first
  if (first_rank < second_rank) {
    SwapInts(&first_rank, &second_rank);
  }
  if (suited) {
    Set(first_rank, second_rank);
  } else {
    Set(second_rank, first_rank);
  }
}

// first_rank  > second_rank -> suited
// second_rank > first_rank  -> offsuited
void COHScriptList::Set(int first_rank, int second_rank) {
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
  return _handlist_matrix[second_rank][first_rank];
};

bool COHScriptList::Set(CString list_member) {
  int length = list_member.GetLength();
  if ((length < 2) || (length > 3)) {
    ErrorInvalidMember(list_member);
    return false;
  }
  char first_rank_string = tolower(list_member.GetAt(0));
  char second_rank_string = tolower(list_member.GetAt(1));
  int first_rank = CardRankCharacterToCardRank(first_rank_string);
  int second_rank = CardRankCharacterToCardRank(second_rank_string);
  if ((first_rank < 0) || (second_rank < 0)) {
    ErrorInvalidMember(list_member);
    return false;
  }
  if (length == 2) {
    if (first_rank_string == second_rank_string) {
      Set(first_rank, second_rank, false);
      return true;
    }
    ErrorOldStyleFormat(list_member);
    return false;
  }
  assert(length == 3);
  char suit = tolower(list_member.GetAt(2));
  if (suit == 's') {
    Set(first_rank, second_rank, true);
  } else if (suit == 'o') {
    Set(first_rank, second_rank, false);
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
    "Valid are entries look like \"AA AKs ATo 22",
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