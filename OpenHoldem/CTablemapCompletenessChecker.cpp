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
#include "CTablemapCompletenessChecker.h"

#include "CSessionCounter.h"
#include "../CTablemap/CTablemap.h"
#include "..\DLLs\WindowFunctions_DLL\window_functions.h"

CTablemapCompletenessChecker::CTablemapCompletenessChecker() {
}

CTablemapCompletenessChecker::~CTablemapCompletenessChecker() {
}

void CTablemapCompletenessChecker::ErrorMissingItem(CString item) {
  CString message;
  message.Format("Missing item in tablemap: %s.\n"
    "This item is absolutely necessary for correct execution.\n"
    "%s", 
    item, p_tablemap->filepath());
  MessageBox_Interactive(message, "Error", 0);
}

void CTablemapCompletenessChecker::ErrorDeprecatedItem(CString item) {
  CString message;
  message.Format("Deprecated item in tablemap: %s.\n"
    "You can safely delete this item from the tablemap.\n"
    "Due to a simplification of OpenHoldem it is no longer required.\n"
    "The reasons and potential consequences are documented in the release-notes.\n"
    "\n"
    "%s", 
    item, p_tablemap->filepath());
  MessageBox_Interactive(message, "Error", 0);
}

void CTablemapCompletenessChecker::ErrorSwagRenamed() {
  CString message;
  message.Format("\"Swagging\" has been removed from OpenHoldem\n"
    "The following tablemap-symbols got renamed:\n"
    "\n"
    "  * swagselectionmethod -> betsizeselectionmethod\n"
    "  * swagdeletionmethodmethod -> betsizedeletionmethodmethod\n"
    "  * swagconfirmationmethod -> betsizeconfirmationmethod\n"
    "  * swagtextmethod -> betsizeinterpretationmethod\n"
    "\n"
    "%s", 
    p_tablemap->filepath());
  MessageBox_Interactive(message, "Error", 0);
}

void CTablemapCompletenessChecker::ErrorMissingTablepoint() {
  CString message;
  message.Format("Missing tablepoint in tablemap.\n"
    "This tablemap uses an empty titletext for connection.\n"
    "To reduce the risk of connections to random windows\n"
    "at least r$tablepoint0 must be present.\n"
    "\n"
    "%s",
    p_tablemap->filepath());
  MessageBox_Interactive(message, "Error", 0);
}

void CTablemapCompletenessChecker::ErrorClientsizeReplaced() {
  CString message = "z$clientsize got replaced in favour\n"
    "of a more flexible and fail-safe approach.\n"
    "It needs to be replaced by\n"
    "  * z$clientsizemin\n"
    "  * z$clientsizemax\n"
    "  * z$targetsize\n";
  MessageBox_Interactive(message, "Error", 0);
}

void CTablemapCompletenessChecker::CheckItem(CString item) {
  if (!p_tablemap->ItemExists(item)) {
    ErrorMissingItem(item);
  }
}

void CTablemapCompletenessChecker::CheckSetOfItems(CString prefix, 
                                                   int last_index, 
                                                   CString postfix, 
                                                   bool mandatory) {
  // All other items are also mandatory
  // if the first optional item exists                                                   
  CString first_optional_item;
  first_optional_item.Format("%s%i%s", prefix, 0, postfix);
  if (p_tablemap->ItemExists(first_optional_item)) {
    mandatory = true;
  }
  if (!mandatory) return;
  for (int i=0; i<=last_index; ++i) {
	  CString s_chair = to_string(i).c_str();
	  if (!p_tablemap->ItemExists("area_cards_player" + s_chair)) {
		  CheckItem(prefix, i, postfix);
	  }
  }
}

void CTablemapCompletenessChecker::CheckItem(CString prefix, int infix, CString postfix) {
  CString name;
  name.Format("%s%d%s", prefix, infix, postfix);
  CheckItem(name);
}

bool CTablemapCompletenessChecker::IsNoLimitMap() {
  return (p_tablemap->ItemExists("i3edit")
    || p_tablemap->ItemExists("i3slider")
    || p_tablemap->ItemExists("betpotmethod")
    || p_tablemap->ItemExists("betsizeselectionmethod")
    || p_tablemap->ItemExists("betsizedeletionmethod")
    || p_tablemap->ItemExists("betsizeinterpretationmethod")
    || p_tablemap->ItemExists("betsizeconfirmationmethod"));
}

void CTablemapCompletenessChecker::CheckBetsOrChips() {
  bool tm_uses_stacks = p_tablemap->ItemExists("p0chip00");
  int nchairs = p_tablemap->nchairs();
  for (int i=0; i<nchairs; ++i) {
    if (tm_uses_stacks) {
      CheckItem("p", i, "chip00");
      CheckItem("p", i, "chip01");
      CheckItem("p", i, "chip10");
    } else {
      CheckItem("p", i, "bet");
    }
  }
}

void CTablemapCompletenessChecker::CheckCardFaces(CString prefix, int infix, CString postfix) {
  CString name;
  name.Format("%s%d%s", prefix, infix, postfix);
  CString rank_name = name + "rank";
  CString s_chair = to_string(infix).c_str();
  if (p_tablemap->ItemExists("area_cards_common"))
	  CheckItem("area_cards_common");
  else if (p_tablemap->ItemExists("area_cards_player" + s_chair))
	  CheckItem("area_cards_player" + s_chair);
  else if (p_tablemap->ItemExists(rank_name)) {
    // If a rank exists then the suit also needs to get scraped
    CString suit_name = name + "suit";
    CheckItem(suit_name);
  } else {
    // Default: cardface must be there
    CheckItem(name);
  }
}

void CTablemapCompletenessChecker::CheckMainPot() {
  if (p_tablemap->ItemExists("c0pot0chip00")) {
     CheckItem("c0pot0chip01");
     CheckItem("c0pot0chip10");
  } else {
    CheckItem("c0pot0");
  }
}

void CTablemapCompletenessChecker::VerifySingleC0limitsItem(CString name) {
  // Each optional r$c0limitsX-region must have 
  // a corresponding s£c0limitsX-symbol,
  // otherwise OpenHoldem can crash on evaluation
  // http://www.maxinmontreal.com/forums/viewtopic.php?f=110&t=18865
  if (!p_tablemap->ItemExists(name)) return;
  SMapCI sit = p_tablemap->s$()->find(name); 
  if (sit == p_tablemap->s$()->end()) {
    CString missing_item = "s$" + name;
    ErrorMissingItem(missing_item);
  }
}

void CTablemapCompletenessChecker::VerifyC0limits() {
  VerifySingleC0limitsItem("c0limits");
  for (int i=0; i<9; ++i) {
    CString c0limitsX;
    c0limitsX.Format("c0limits%i", i);
    VerifySingleC0limitsItem(c0limitsX);
  }
}

void CTablemapCompletenessChecker::VerifyMap() {
  // Only session 0 verifies the tablemaps
  // for better performance amd to avoid driving users crazy.
  if (p_sessioncounter->session_id() > 0) return;
  // First check for deprecated (and renamed( items.
  // This avoids confusion if we cmplain about missing renamed symbols later
  CheckForDeprecatedItems();
  // Absoluely mandatory for connection
  CheckItem("titletext");
  VerifyTitleTexts();
  // Necessary for connection: clientsizemin/max and y$targetsize (since 9.0.1)
  CheckItem("clientsizemin");
  CheckItem("clientsizemax");
  CheckItem("targetsize");
  // All the rest is only needed for tables, 
  // but not for lobby-tablemaps or popup-TMs
  if (p_tablemap->islobby()) return;
  if (p_tablemap->ispopup()) return;
  // Basic info, needed by every table
  CheckItem("nchairs");
  CheckItem("network");
  CheckItem("ttlimits");
  if (p_tablemap->sitename() == "") {
    CString error_message;
    error_message.Format("Tablemap contains no sitename.\n"
      "Sitenames are necessary to recognize duplicate TMs\n"
      "(and for other features like PokerTracker).\n\n",
      "%s", p_tablemap->filepath());
    MessageBox_Error_Warning(error_message, "Warning");
  }
  // Range-check nchairs
  int nchairs = p_tablemap->nchairs();
  int last_chair = nchairs - 1;
  if ((nchairs < 2) || (nchairs > kMaxNumberOfPlayers)) {
    CString message;
    message.Format("Tablemap item nchairs out of range\n"
      "Correct values: 2..10\n"
      "%s",
      p_tablemap->filepath());
    MessageBox_Interactive(message, "Error", 0);
  }
  // Check mandatory items for every seat
  CheckSetOfItems("p", last_chair, "active",   true);  
  CheckSetOfItems("p", last_chair, "balance",  true);
CheckSetOfItems("p", last_chair, "dealer", true);
CheckSetOfItems("p", last_chair, "name", true);
CheckSetOfItems("p", last_chair, "seated", true);
CheckSetOfItems("p", last_chair, "cardback", true);
CheckSetOfItems("p", last_chair, "cardface0nocard", true);
CheckSetOfItems("p", last_chair, "cardface1nocard", true);
if (p_tablemap->SupportsOmaha()) {
	CheckSetOfItems("p", last_chair, "cardface2nocard", true);
	CheckSetOfItems("p", last_chair, "cardface3nocard", true);
}
// Check mandatory cards faces (or rank + suit) for every seat
for (int i = 0; i < nchairs; ++i) {
  CheckCardFaces("p", i, "cardface0");
  CheckCardFaces("p", i, "cardface1");
  if (p_tablemap->SupportsOmaha()) {
    CheckCardFaces("p", i, "cardface2");
    CheckCardFaces("p", i, "cardface3");
  }
}
CheckBetsOrChips();
// Community cards
for (int i = 0; i < kNumberOfCommunityCards; ++i) {
  CheckCardFaces("c0cardface", i, "");
}
int last_communitz_card = kNumberOfCommunityCards - 1;
if (!p_tablemap->ItemExists("area_cards_common"))
	CheckSetOfItems("c0cardface", last_communitz_card, "nocard", true);
CheckMainPot();
// Action buttons
int number_of_buttons_seen = 0;
CString area_name;
bool area_found = false;
RMapCI		r_iter = p_tablemap->r$()->end();
for (int i = 0; i < k_max_area_buttons_zone; i++) {
	area_name.Format("area_buttons_zone%c", HexadecimalChar(i));
	r_iter = p_tablemap->r$()->find(area_name);
	if (r_iter != p_tablemap->r$()->end()) {
		area_found = true;
		for (int j = 0; j < k_max_action_buttons; j++) {
			if (p_tablemap->ItemExists(k_action_button_name[j]))
				++number_of_buttons_seen;
		}
	}
}
if (!area_found) {
	for (int i = 0; i < k_max_number_of_buttons; ++i) {
		CString button;
		button.Format("i%cbutton", HexadecimalChar(i));
		if (p_tablemap->ItemExists(button)) {
			++number_of_buttons_seen;
			CString button_state;
			button_state.Format("i%cstate", HexadecimalChar(i));
			CheckItem(button_state);
			CString button_label;
			button_label.Format("i%clabel", HexadecimalChar(i));
			CheckItem(button_label);
		}
	}
}
if (number_of_buttons_seen < 3) {
  CString message;
  message.Format(
    "A tablemap needs at least 3 buttons:\n"
    "  Fold, Check/Call, Bet/Raise\n"
    "At least one of them is missing.\n"
    "%s",
    p_tablemap->filepath());
  MessageBox_Interactive(message, "Error", 0);
}
// No Limit only
if (IsNoLimitMap()) {
  CheckItem("betsizeselectionmethod");
  CheckItem("betsizedeletionmethod");
  CheckItem("betsizeinterpretationmethod");
  CheckItem("betsizeconfirmationmethod");
  // i3edit ("betsizing") needs a betsizeconfirmation-button,
  // which might be something different than min-raise-button.
  if (!area_found) {
	  CheckItem("i3edit");
	  CheckItem("i3button");
	  CheckItem("i3state");
	  CheckItem("i3label");
  }
  else {
	  CheckItem("editbet");
  }
  // Slider now optional, as allinmethod (= 3) is gone.
  // CheckItem("i3slider");
  // CheckItem("i3handle");
}
// Font-types
for (int i = 0; i < k_max_number_of_font_groups_in_tablemap; ++i) {
  if (p_tablemap->FontGroupInUse(i)) {
    CheckItem("t", i, "type");
  }
}
VerifyC0limits();
// Optional uX-regions
CheckSetOfItems("u", last_chair, "name", false);
CheckSetOfItems("u", last_chair, "seated", false);
CheckSetOfItems("u", last_chair, "active", false);
CheckSetOfItems("u", last_chair, "balance", false);
CheckSetOfItems("u", last_chair, "bet", false);
CheckSetOfItems("u", last_chair, "dealer", false);
// Not checked, despite nice to have
//   * betpot-buttons
//   * i86X-buttons
//
// Not necessary, as optional
//   * r$c0isfinaltable
//   * potmethod
//   * buttonclickmethod
//   * betpotmethod
// and some others  
}

void CTablemapCompletenessChecker::VerifyTitleTexts() {
  // titletext should exist (already checked)
  if (!p_tablemap->ItemExists("titletext")) {
    return;
  }
  if (p_tablemap->GetTMSymbol("titletext") == "") {
    // Since version 11.0.1 we support empty titletexts 
    // But then at least one tablepoint is required
    // in addition to clientsizemin/max to avoid random connections.
    // http://www.maxinmontreal.com/forums/viewtopic.php?f=124&t=20382
    if (!p_tablemap->ItemExists("tablepoint0")) {
      ErrorMissingTablepoint();
    }
    for (int i = 0; i < k_max_number_of_titletexts; ++i) {
      CString title;
      title.Format("titletext%d", i);
      if (p_tablemap->ItemExists(title)) {
        CString message;
        message.Format("Tablemap contains empty titletext.\n"
          "It must not contain additional titles: %s\n"
          "%s",
          title, p_tablemap->filepath());
        MessageBox_Interactive(message, "Error", 0);
      }
    }
    return;
  }
  // titletext not empty
  // Then all other titletexts (if existent) must be non-empty too.
  for (int i = 0; i < k_max_number_of_titletexts; ++i) {
    CString title;
    title.Format("titletext%d", i);
    if (!p_tablemap->ItemExists(title)) {
      continue;
    }
    if (p_tablemap->GetTMSymbol(title) == "") {
      CString message;
      message.Format("Tablemap contains empty %s.\n"
        "Only the main titletext may be empty.\n"
        "Mixed empty and specified titles are not supported."
        "%s",
        title, p_tablemap->filepath());
      MessageBox_Interactive(message, "Error", 0);
    }
  }
}

void CTablemapCompletenessChecker::CheckForDeprecatedItems() {
  // r$c0istournament no longer supported, works automatically
  if (p_tablemap->ItemExists("c0istournament")) {
    ErrorDeprecatedItem("c0istournament");
  }
  // handresetmethod no longer supported, works automatically
  if (p_tablemap->ItemExists("handresetmethod")) {
    ErrorDeprecatedItem("handresetmethod");
  }
  // swag cofiguration symbols got renamed
  if (p_tablemap->ItemExists("swagselectionmethod")
      || p_tablemap->ItemExists("swagdeletionmethodmethod")
      || p_tablemap->ItemExists("swagconfirmationmethod")
      || p_tablemap->ItemExists("swagtextmethod")) {
    ErrorSwagRenamed();
  }
  // activemethod no longer upported since OpenHoldem 7.3.1
  // since it was based on backward-compatibilitz for a bug
  // http://www.maxinmontreal.com/forums/viewtopic.php?f=117&t=17818&start=30#p125307
  if (p_tablemap->ItemExists("activemethod")) {
    ErrorDeprecatedItem("activemethod");
  }
  // allinmethod no longer supported, works automatically
  if (p_tablemap->ItemExists("allinmethod")) {
    ErrorDeprecatedItem("allinmethod");
  }
  // clientsize removed in favour of clientsizemin/max and targetzize
  if (p_tablemap->ItemExists("clientsize")) {
    ErrorClientsizeReplaced();
  }
}