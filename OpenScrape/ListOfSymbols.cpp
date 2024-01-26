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
#include <vector>
#include "ListOfSymbols.h"
#include "..\DLLs\StringFunctions_DLL\string_functions.h"

// declared as extern in the heqder
std::vector<CString> list_of_regions;
std::vector<CString> list_of_sizes;
std::vector<CString> list_of_symbols;
std::vector<CString> list_of_templates;

CString ListOfSymbols() {
  CString list;
  // The function RangeOfSymbols adds a space after every symbol.
  // Individual symbols that get added to the list 
  // must preovide a space at the end.
  list += RangeOfSymbols("titletext%i", 0, 9);
  list += RangeOfSymbols("!titletext%i", 0, 9);
  list += RangeOfSymbols("ttlimits%i", 0, 9);
  list += RangeOfSymbols("c0limits%i", 0, 9);
  list += RangeOfSymbols("i%ibuttonhotkey", 0, 9);
  list += RangeOfSymbols("i%ibuttondefaultlabel", 0, 9);
  list += RangeOfSymbols("i86%ibuttonclickmethod", 0, 9);
  list += RangeOfSymbols("t%itype", 0, 3);
  list += "allinconfirmationmethod ";
  list += "betpotmethod ";
  list += "betpot_2_1buttonhotkey ";
  list += "betpot_1_1buttonhotkey ";
  list += "betpot_3_4buttonhotkey ";
  list += "betpot_2_3buttonhotkey ";
  list += "betpot_1_2buttonhotkey ";
  list += "betpot_1_3buttonhotkey ";
  list += "betpot_1_4buttonhotkey ";
  list += "betsizeconfirmationmethod ";
  list += "betsizedeletionmethod ";
  list += "betsizeinterpretationmethod ";
  list += "betsizeselectionmethod ";
  list += "buttonclickmethod ";
  list += "c0limits ";
  list += "cardscrapemethod ";
  list += "chipscrapemethod ";
  list += "islobby ";
  list += "ispopup ";
  list += "nchairs ";
  list += "network ";
  list += "potmethod ";
  list += "sitename ";
  list += "titletext ";
  list += "!titletext ";
  list += "ttlimits ";
  list += "use_comma_instead_of_dot ";
  return list;
}

CString ListOfRegions() {
  CString list;
  list += RangeOfSymbols("c0cardface%i", 0, 4);
  list += RangeOfSymbols("c0cardface%inocard", 0, 4);
  list += RangeOfSymbols("c0cardface%irank", 0, 4);
  list += RangeOfSymbols("c0cardface%isuit", 0, 4);
  list += "c0handnumber ";
  list += RangeOfSymbols("c0handnumber%i", 1, 9);
  list += RangeOfSymbols("c0pot%i", 0, 4);
  list += RangeOfSymbols("c0pot%ichip00", 0, 4);
  list += RangeOfSymbols("c0pot%ichip01", 0, 4);
  list += RangeOfSymbols("c0pot%ichip10", 0, 4);
  list += "c0limits ";
  list += RangeOfSymbols("c0limits%i", 0, 9);
  list += RangeOfSymbols("i%ibutton", 0, 9);
  list += RangeOfSymbols("i%ilabel", 0, 9);
  list += RangeOfSymbols("i%istate", 0, 9);
  list += RangeOfSymbols("i86%ibutton", 0, 9);
  list += RangeOfSymbols("i86%istate", 0, 9);
  list += RangeOfSymbols("p%iactive", 0, 9);
  list += RangeOfSymbols("p%ibalance", 0, 9);
  list += RangeOfSymbols("p%ibet", 0, 9);
  list += RangeOfSymbols("p%icardback", 0, 9);
  list += RangeOfSymbols("p%icardface0", 0, 9);
  list += RangeOfSymbols("p%icardface0nocard", 0, 9);
  list += RangeOfSymbols("p%icardface0rank", 0, 9);
  list += RangeOfSymbols("p%icardface0suit", 0, 9);
  list += RangeOfSymbols("p%icardface1", 0, 9);
  list += RangeOfSymbols("p%icardface1nocard", 0, 9);
  list += RangeOfSymbols("p%icardface1rank", 0, 9);
  list += RangeOfSymbols("p%icardface1suit", 0, 9);
  list += RangeOfSymbols("p%icardface2", 0, 9);
  list += RangeOfSymbols("p%icardface2nocard", 0, 9);
  list += RangeOfSymbols("p%icardface2rank", 0, 9);
  list += RangeOfSymbols("p%icardface2suit", 0, 9);
  list += RangeOfSymbols("p%icardface3", 0, 9);
  list += RangeOfSymbols("p%icardface3nocard", 0, 9);
  list += RangeOfSymbols("p%icardface3rank", 0, 9);
  list += RangeOfSymbols("p%icardface3suit", 0, 9);
  list += RangeOfSymbols("p%icolourcode", 0, 9);
  list += RangeOfSymbols("p%idealer", 0, 9);
  list += RangeOfSymbols("p%iname", 0, 9);
  list += RangeOfSymbols("p%iseated", 0, 9);
  list += RangeOfSymbols("p%ichip00", 0, 9);
  list += RangeOfSymbols("p%ichip01", 0, 9);
  list += RangeOfSymbols("p%ichip10", 0, 9);
  list += RangeOfSymbols("tablepoint%i", 0, 9);
  list += RangeOfSymbols("u%iactive", 0, 9);
  list += RangeOfSymbols("u%ibalance", 0, 9);
  list += RangeOfSymbols("u%ibet", 0, 9);
  list += RangeOfSymbols("u%icardface0", 0, 9);
  list += RangeOfSymbols("u%icardface1", 0, 9);
  list += RangeOfSymbols("u%idealer", 0, 9);
  list += RangeOfSymbols("u%iname", 0, 9);
  list += RangeOfSymbols("u%iseated", 0, 9);
  list += "area_action_button ";
  list += "area_hopper_button ";
  list += "area_common_cards ";
  list += RangeOfSymbols("area_player_cards%i", 0, 9);
  list += "betpot_2_1button ";
  list += "betpot_1_1button ";
  list += "betpot_3_4button ";
  list += "betpot_2_3button ";
  list += "betpot_1_2button ";
  list += "betpot_1_3button ";
  list += "betpot_1_4button ";
  list += "betpot_2_1state ";
  list += "betpot_1_1state ";
  list += "betpot_3_4state ";
  list += "betpot_2_3state ";
  list += "betpot_1_2state ";
  list += "betpot_1_3state ";
  list += "betpot_1_4state ";
  list += "c0isfinaltable ";
  list += "c0smallblind ";
  list += "c0bigblind ";
  list += "c0bigbet ";
  list += "c0ante ";
  list += "i3edit ";
  list += "i3handle ";
  list += "i3slider ";
  list += "mtt_average_stack ";
  list += "mtt_largest_stack ";
  list += "mtt_my_rank ";
  list += "mtt_number_entrants ";
  list += "mtt_paid_places ";
  list += "mtt_players_remaining ";
  list += "mtt_smallest_stack ";
  return list;
}

CString ListOfTemplates() {
	CString list;
	list += "card_common_ac ";
	list += "card_common_ad ";
	list += "card_common_as ";
	list += "card_common_ah ";
	list += "card_common_kc ";
	list += "card_common_kd ";
	list += "card_common_ks ";
	list += "card_common_kh ";
	list += "card_common_qc ";
	list += "card_common_qd ";
	list += "card_common_qs ";
	list += "card_common_qh ";
	list += "card_common_jc ";
	list += "card_common_jd ";
	list += "card_common_js ";
	list += "card_common_jh ";
	list += "card_common_tc ";
	list += "card_common_td ";
	list += "card_common_ts ";
	list += "card_common_th ";
	list += "card_common_9c ";
	list += "card_common_9d ";
	list += "card_common_9s ";
	list += "card_common_9h ";
	list += "card_common_8c ";
	list += "card_common_8d ";
	list += "card_common_8s ";
	list += "card_common_8h ";
	list += "card_common_7c ";
	list += "card_common_7d ";
	list += "card_common_7s ";
	list += "card_common_7h ";
	list += "card_common_6c ";
	list += "card_common_6d ";
	list += "card_common_6s ";
	list += "card_common_6h ";
	list += "card_common_5c ";
	list += "card_common_5d ";
	list += "card_common_5s ";
	list += "card_common_5h ";
	list += "card_common_4c ";
	list += "card_common_4d ";
	list += "card_common_4s ";
	list += "card_common_4h ";
	list += "card_common_3c ";
	list += "card_common_3d ";
	list += "card_common_3s ";
	list += "card_common_3h ";
	list += "card_common_2c ";
	list += "card_common_2d ";
	list += "card_common_2s ";
	list += "card_common_2h ";
	list += "card_player_ac ";
	list += "card_player_ad ";
	list += "card_player_as ";
	list += "card_player_ah ";
	list += "card_player_kc ";
	list += "card_player_kd ";
	list += "card_player_ks ";
	list += "card_player_kh ";
	list += "card_player_qc ";
	list += "card_player_qd ";
	list += "card_player_qs ";
	list += "card_player_qh ";
	list += "card_player_jc ";
	list += "card_player_jd ";
	list += "card_player_js ";
	list += "card_player_jh ";
	list += "card_player_tc ";
	list += "card_player_td ";
	list += "card_player_ts ";
	list += "card_player_th ";
	list += "card_player_9c ";
	list += "card_player_9d ";
	list += "card_player_9s ";
	list += "card_player_9h ";
	list += "card_player_8c ";
	list += "card_player_8d ";
	list += "card_player_8s ";
	list += "card_player_8h ";
	list += "card_player_7c ";
	list += "card_player_7d ";
	list += "card_player_7s ";
	list += "card_player_7h ";
	list += "card_player_6c ";
	list += "card_player_6d ";
	list += "card_player_6s ";
	list += "card_player_6h ";
	list += "card_player_5c ";
	list += "card_player_5d ";
	list += "card_player_5s ";
	list += "card_player_5h ";
	list += "card_player_4c ";
	list += "card_player_4d ";
	list += "card_player_4s ";
	list += "card_player_4h ";
	list += "card_player_3c ";
	list += "card_player_3d ";
	list += "card_player_3s ";
	list += "card_player_3h ";
	list += "card_player_2c ";
	list += "card_player_2d ";
	list += "card_player_2s ";
	list += "card_player_2h ";
	return list;
}

std::vector<CString> SplitStringToStringVector(CString s, char delimiter) {
  // Add a delimiter to the end of input for more easy tokenization
  CString temp_input = s + delimiter;
  std::vector<CString> result;
  int length = temp_input.GetLength();
  int pos = 0;
  int last_pos = -1;
  while (pos < length) {
    if ((temp_input[pos] == delimiter) && (pos != last_pos)) {
      int length_of_substring = pos - last_pos - 1;
      if (length_of_substring <= 0) {
        ++pos;
        continue;
      }
      CString substring = temp_input.Mid(last_pos + 1, length_of_substring);
      result.push_back(substring);
      last_pos = pos;
    }
    ++pos;
  }
  return result;
}

CString ListOfSizes() {
  return "clientsizemin clientsizemax targetsize";
}

void BuildVectorsOfScraperSymbols() {
  const char kSpace = ' ';
  CString sizes = ListOfSizes();
  CString regions = ListOfRegions();
  CString symbols = ListOfSymbols();
  CString templates = ListOfTemplates();
  list_of_sizes = SplitStringToStringVector(sizes, kSpace);
  list_of_regions = SplitStringToStringVector(regions, kSpace);
  list_of_templates = SplitStringToStringVector(templates, kSpace);
}