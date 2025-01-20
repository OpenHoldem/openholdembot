//******************************************************************************
//
// This file is part of the OpenHoldem project
//    Source code:           https://github.com/OpenHoldem/openholdembot/
//    Forums:                http://www.maxinmontreal.com/forums/index.php
//    Licensed under GPL v3: http://www.gnu.org/licenses/gpl.html
//
//******************************************************************************
//
// Purpose: Reading the poker-table.
//  State-less class for future multi-table support.
//  All data is now in the CTable'state container.
//
//******************************************************************************

#include "StdAfx.h"
#include <string>
#include <regex>
using namespace std;
#include "CScraper.h"

#include "Bitmaps.h" 
#include "CardFunctions.h"
#include "CAutoconnector.h"
#include "CCasinoInterface.h"
#include "CEngineContainer.h"
#include "CAutoOcr.h"

#include "CStringMatch.h"
#include "CSymbolEngineActiveDealtPlaying.h"
#include "CSymbolEngineAutoplayer.h"
#include "CSymbolEngineCasino.h"
#include "CSymbolEngineDebug.h"
#include "CSymbolEngineHistory.h"
#include "CSymbolEngineIsOmaha.h"
#include "CSymbolEngineMTTInfo.h"
#include "CSymbolEngineUserchair.h"
#include "CSymbolEngineTableLimits.h"
#include "CTableState.h"
#include "CTableTitle.h"
#include "CTitleEvaluator.h"
#include "..\CTransform\CTransform.h"
#include "..\CTransform\hash\lookup3.h"

#include "MainFrm.h"
#include "OpenHoldem.h"

CScraper *p_scraper = NULL;

#define __HDC_HEADER 		HBITMAP		old_bitmap = NULL; \
	HDC				hdc = GetDC(p_autoconnector->attached_hwnd()); \
	HDC				hdcScreen = CreateDC("DISPLAY", NULL, NULL, NULL); \
	HDC				hdcCompatible = CreateCompatibleDC(hdcScreen); \
  ++_leaking_GDI_objects;

#define __HDC_FOOTER_ATTENTION_HAS_TO_BE_CALLED_ON_EVERY_FUNCTION_EXIT_OTHERWISE_MEMORY_LEAK \
  DeleteDC(hdcCompatible); \
	DeleteDC(hdcScreen); \
	ReleaseDC(p_autoconnector->attached_hwnd(), hdc); \
  --_leaking_GDI_objects;



CScraper::CScraper(void) {
	p_table_state->Reset();
  _leaking_GDI_objects = 0;
  total_region_counter = 0;
  identical_region_counter = 0;
}

CScraper::~CScraper(void) {
	p_table_state->Reset();
  if (_leaking_GDI_objects != 0 ) {
    write_log(k_always_log_errors, "[CScraper] ERROR! Leaking GDI objects: %i\n",
      _leaking_GDI_objects);
    write_log(k_always_log_errors, "[CScraper] Please get in contact with the development team\n");
  }
  assert(_leaking_GDI_objects == 0);
  write_log(true, "[CScraper] Total regions scraped %i\n",
    total_region_counter);
  write_log(true, "[CScraper] Identical regions scraped %i\n",
    identical_region_counter);
}

bool CScraper::ProcessRegion(RMapCI r_iter) {
  write_log(Preferences()->debug_scraper(),
    "[CScraper] ProcessRegion %s (%i, %i, %i, %i)\n",
    r_iter->first, r_iter->second.left, r_iter->second.top,
    r_iter->second.right, r_iter->second.bottom);
  write_log(Preferences()->debug_scraper(),
    "[CScraper] ProcessRegion color %i radius %i transform %s\n",
    r_iter->second.color, r_iter->second.radius, r_iter->second.transform);
	__HDC_HEADER
	// Get "current" bitmap
	old_bitmap = (HBITMAP) SelectObject(hdcCompatible, r_iter->second.cur_bmp);
	/*if (r_iter->second.transform[0] == 'A') {
		BitBlt(hdcCompatible, 0, 0, r_iter->second.right - r_iter->second.left + 7,
			r_iter->second.bottom - r_iter->second.top + 7,
			hdc, r_iter->second.left - 3, r_iter->second.top - 3, SRCCOPY);
	}
	else {*/
		BitBlt(hdcCompatible, 0, 0, r_iter->second.right - r_iter->second.left + 1, 
									r_iter->second.bottom - r_iter->second.top + 1, 
									hdc, r_iter->second.left, r_iter->second.top, SRCCOPY);
	//}
	SelectObject(hdcCompatible, old_bitmap);
	//SaveHBITMAPToFile(r_iter->second.cur_bmp, "output.bmp");

	// If the bitmaps are different, then continue on
	if (!BitmapsAreEqual(r_iter->second.last_bmp, r_iter->second.cur_bmp)) {
    // Copy into "last" bitmap
		old_bitmap = (HBITMAP) SelectObject(hdcCompatible, r_iter->second.last_bmp);
		/*if (r_iter->second.transform[0] == 'A') {
			BitBlt(hdcCompatible, 0, 0, r_iter->second.right - r_iter->second.left + 7,
				r_iter->second.bottom - r_iter->second.top + 7,
				hdc, r_iter->second.left - 3, r_iter->second.top - 3, SRCCOPY);
		}
		else {*/
			BitBlt(hdcCompatible, 0, 0, r_iter->second.right - r_iter->second.left + 1,
				r_iter->second.bottom - r_iter->second.top + 1,
				hdc, r_iter->second.left, r_iter->second.top, SRCCOPY);
		//}
		SelectObject(hdcCompatible, old_bitmap);  
		__HDC_FOOTER_ATTENTION_HAS_TO_BE_CALLED_ON_EVERY_FUNCTION_EXIT_OTHERWISE_MEMORY_LEAK
		return true;
	}
	__HDC_FOOTER_ATTENTION_HAS_TO_BE_CALLED_ON_EVERY_FUNCTION_EXIT_OTHERWISE_MEMORY_LEAK
	return false;
}

bool CScraper::EvaluateRegion(CString name, CString *result) {
  __HDC_HEADER
  write_log(Preferences()->debug_scraper(),
    "[CScraper] EvaluateRegion %s\n", name);
	CTransform	trans;
	RMapCI		r_iter = p_tablemap->r$()->find(name.GetString());
	if (r_iter != p_tablemap->r$()->end()) {
    // Potential for optimization here
    ++total_region_counter;
		if (ProcessRegion(r_iter)) {
      ++identical_region_counter;
      write_log(Preferences()->debug_scraper(),
        "[CScraper] Region %s identical\n", name);
    } else {
      write_log(Preferences()->debug_scraper(),
        "[CScraper] Region %s NOT identical\n", name);
    }
		old_bitmap = (HBITMAP) SelectObject(hdcCompatible, r_iter->second.cur_bmp);
		if (r_iter->second.transform[0] == 'A') {
			int w = r_iter->second.right - r_iter->second.left + 1;
			int h = r_iter->second.bottom - r_iter->second.top + 1;
			Mat input(h, w, CV_8UC4);
			BITMAPINFOHEADER bi = { sizeof(bi), w, -h, 1, 32, BI_RGB };
			GetDIBits(hdc, r_iter->second.cur_bmp, 0, h, input.data, (BITMAPINFO*)&bi, DIB_RGB_COLORS);
			//imshow("Output", input);
			//waitKey();
			*result = p_auto_ocr->get_ocr_result(input, r_iter).GetString();
		}
		else
			trans.DoTransform(r_iter, hdcCompatible, result);
		SelectObject(hdcCompatible, old_bitmap);
		write_log(Preferences()->debug_scraper(), "[CScraper] EvaluateRegion(), [%s] -> [%s]\n", 
			name, *result);
    __HDC_FOOTER_ATTENTION_HAS_TO_BE_CALLED_ON_EVERY_FUNCTION_EXIT_OTHERWISE_MEMORY_LEAK
		return true;
	}
	// Region does not exist
  *result = "";
	__HDC_FOOTER_ATTENTION_HAS_TO_BE_CALLED_ON_EVERY_FUNCTION_EXIT_OTHERWISE_MEMORY_LEAK
	return false;
}

// Result will be changed to true, if "true" or something similar is found
// Result will be changed to falsee, if "false" or something similar is found
// Otherwise unchanged (keep default / allow multiple evaluations)
void CScraper::EvaluateTrueFalseRegion(bool *result, const CString name) {
  CString text_result;
	if (EvaluateRegion(name, &text_result))	{
    write_log(Preferences()->debug_scraper(), "[CScraper] %s result %s\n", 
      name, text_result.GetString());
    if (text_result == "true") {
      *result = true;
    } else if (text_result == "false") {
      *result = false;
    }
	}
}

void CScraper::ScrapeButtons(CString area_name, CString needed_buttons) {
	RECT button_region;
	CString result;
	RMapCI		r_iter = p_tablemap->r$()->end();

	r_iter = p_tablemap->r$()->find(area_name);
	if (r_iter != p_tablemap->r$()->end()) {
		int r_width = r_iter->second.right - r_iter->second.left;
		int r_height = r_iter->second.bottom - r_iter->second.top;
		if (r_width > 0 && r_height > 0) {
			// Action buttons
			if (needed_buttons == "action") {
				for (int i = 0; i < k_max_action_buttons; i++) {
					result = p_auto_ocr->GetDetectTemplateResult(r_iter->second.name, k_action_button_name[i], &button_region);
					if (result == "true") {
						p_casino_interface->_technical_autoplayer_buttons[i].SetState(result);
						p_casino_interface->_technical_autoplayer_buttons[i].SetLabel(k_action_button_name[i]);
					}
					if ((i == 5) && p_engine_container->symbol_engine_casino()->ConnectedToManualMode()) {
						// Ugly WinHoldem convention
						// When using ManualMode, grab i5state for PT network
						p_tablemap->set_network(result);
					}
				}
				// Handle of All-in button special cases
				// https://www.maxinmontreal.com/forums/viewtopic.php?p=186236#p186236
				if (p_casino_interface->_technical_autoplayer_buttons[2].IsClickable()) {
					// if (no check and call button) then "allin" is call
					if (!p_casino_interface->_technical_autoplayer_buttons[5].IsClickable() &&
						!p_casino_interface->_technical_autoplayer_buttons[6].IsClickable())
						p_casino_interface->_technical_autoplayer_buttons[2].SetLabel("call");
					// if (call button and no raise button) then "allin" is raise
					else if (p_casino_interface->_technical_autoplayer_buttons[5].IsClickable() &&
						!p_casino_interface->_technical_autoplayer_buttons[3].IsClickable() &&
						!p_casino_interface->_technical_autoplayer_buttons[4].IsClickable())
						p_casino_interface->_technical_autoplayer_buttons[2].SetLabel("raise");
				}
			}
			// betpot buttons
			if (needed_buttons == "betpot") {
				for (int i = 0; i < k_max_betpot_buttons; i++) {
					result = p_auto_ocr->GetDetectTemplateResult(r_iter->second.name, k_betpot_button_name[i], &button_region);
					if (result == "true") {
						p_casino_interface->_technical_betpot_buttons[i].SetState(result);
						//p_casino_interface->_technical_betpot_buttons[i].SetLabel(k_betpot_button_name[i]);
					}
				}
			}
			// Interface buttons (i86)
			if (needed_buttons == "spam") {
				for (int i = 0; i < k_max_number_of_i86X_buttons; i++) {
					// i86X-buttons
					CString button_name;
					button_name.Format("spam%d", i);
					result = p_auto_ocr->GetDetectTemplateResult(r_iter->second.name, button_name, &button_region);
					if (result == "true") {
						p_casino_interface->_technical_i86X_spam_buttons[i].SetState(result);
						//p_casino_interface->_technical_i86X_spam_buttons[i].SetLabel(button_name);
					}
				}
			}
		}
	}
}

void CScraper::ScrapeInterfaceButtons() {
	CString result;
	// i86X-buttons
	CString button_name;
	for (int i = 0; i<k_max_number_of_i86X_buttons; i++) {
		button_name.Format("i86%dstate", i);
		if (EvaluateRegion(button_name, &result)) {
			p_casino_interface->_technical_i86X_spam_buttons[i].SetState(result);
		}
	}
}

void CScraper::ScrapeActionButtons() {
	CString button_name;
	CString result;
	for (int i = 0; i<k_max_number_of_buttons; ++i) {
		button_name.Format("i%cstate", HexadecimalChar(i));
		if (EvaluateRegion(button_name, &result)) {
			p_casino_interface->_technical_autoplayer_buttons[i].SetState(result);
		}
		if ((i == 5) && p_engine_container->symbol_engine_casino()->ConnectedToManualMode()) {
			// Ugly WinHoldem convention
			// When using ManualMode, grab i5state for PT network
			p_tablemap->set_network(result);
		}
	}
}

void CScraper::ScrapeActionButtonLabels() {
	CString label;
	CString result;
	// Every button needs a label
	// No longer using any WinHoldem defaults
	for (int i = 0; i<k_max_number_of_buttons; ++i) {
		p_casino_interface->_technical_autoplayer_buttons[i].SetLabel("");
		label.Format("i%clabel", HexadecimalChar(i));
		if (EvaluateRegion(label, &result)) {
			p_casino_interface->_technical_autoplayer_buttons[i].SetLabel(result);
		}
	}
}

void CScraper::ScrapeBetpotButtons() {
	CString button_name;
	CString result;
	for (int i = 0; i<k_max_betpot_buttons; i++) {
		button_name.Format("%sstate", k_betpot_button_name[i]);
		if (EvaluateRegion(button_name, &result)) {
			p_casino_interface->_technical_betpot_buttons[i].SetState(result);
		}
	}
}

void CScraper::ScrapeSeatedActive() {
	for (int i=0; i<p_tablemap->nchairs(); i++)	{
    p_table_state->Player(i)->set_active(false);
    // Me must NOT set_seated(false) here,
    // as that would reset all player data.
    // http://www.maxinmontreal.com/forums/viewtopic.php?f=156&t=20567
		ScrapeSeated(i);
		if (p_table_state->Player(i)->seated()) {
			ScrapeActive(i);
		}
	}
}

void CScraper::ScrapeBetsAndBalances() {
	for (int i=0; i<p_tablemap->nchairs(); i++)
	{
		// We have to scrape "every" player,
    //   * as people might bet-fold-standup.
    //   * as people might be missing in tournament, but we use ICM
		// Improvement: 
		//   * scrape everybody up to my first action (then we know who was dealt)
		//   * after that we scrape only dealt players
		//   * and also players who have cards (fresh sitdown and hand-reset, former playersdealt is wrong)
		if ((!p_engine_container->symbol_engine_history()->DidActThisHand())
			|| IsBitSet(p_engine_container->symbol_engine_active_dealt_playing()->playersdealtbits(), i)
      || p_table_state->Player(i)->HasAnyCards())
		{
			ScrapeBet(i);
			ScrapeBalance(i);
		}
	}
}

void CScraper::ScrapeSeated(int chair) {
	CString seated;
	CString result;
	// if chair is not seated we call a reset for this chair in CPlayer  
	// by setting this cahir to false we call set_seated first 
	// that will call reset - All ok up the empty chair is the dealer 
	// we must not reset the dealer data even if the seat is empty 
	// this is possible since the lazy-sceaper look for dealer first 
	// we chack here if the chair is dealer and we pass this value
	// to the CPlayer object and that s all we need
	// https://www.maxinmontreal.com/forums/viewtopic.php?t=24182&start=90
	bool this_chair_is_dealer = p_table_state->Player(chair)->dealer();
	// We must NOT set_seated(false) here,
	// as that would reset all player data.
	// http://www.maxinmontreal.com/forums/viewtopic.php?f=156&t=20567
	// http://www.maxinmontreal.com/forums/viewtopic.php?p=191043
	seated.Format("p%dseated", chair);
	if (EvaluateRegion(seated, &result)) {
		if ((result != "") && (p_string_match->IsStringSeated(result))) {
			p_table_state->Player(chair)->set_seated(true, this_chair_is_dealer);
			return;
		}
	}

	// try u region next uXseated,
	// but only if we didn't get a positive result from the p region
	seated.Format("u%dseated", chair);
	if (EvaluateRegion(seated, &result)) {
		if ((result != "") && (p_string_match->IsStringSeated(result))) {
			p_table_state->Player(chair)->set_seated(true, this_chair_is_dealer);
			return;
		}
	}
	// Failed. Not seated
	p_table_state->Player(chair)->set_seated(false, this_chair_is_dealer);
}

void CScraper::ScrapeDealer() {
	// The dealer might sit in any chair, even empty ones in some cases
	// That's why we scrape all chairs
	CString dealer;
	CString result;
	for (int i=0; i<p_tablemap->nchairs(); i++)	{
		p_table_state->Player(i)->set_dealer(false);
	}
	for (int i=0; i<p_tablemap->nchairs(); i++)	{
		dealer.Format("p%ddealer", i);
		if (EvaluateRegion(dealer, &result)) {
			if (p_string_match->IsStringDealer(result))	{
				p_table_state->Player(i)->set_dealer(true);
				return;
			}
		}
		// Now search for uXdealer
		dealer.Format("u%ddealer", i);
		if (EvaluateRegion(dealer, &result)) {
			if (p_string_match->IsStringDealer(result))	{
				p_table_state->Player(i)->set_dealer(true);
				return;
			}
		}
	}
}

void CScraper::ScrapeActive(int chair) {
	CString active;
	CString result;
	p_table_state->Player(chair)->set_active(false);
  // try p region first pXactive
	active.Format("p%dactive", chair);
	if (EvaluateRegion(active, &result)) {
		p_table_state->Player(chair)->set_active(p_string_match->IsStringActive(result));
	}
	if (p_table_state->Player(chair)->active()) {
		return;
	}
	active.Format("u%dactive", chair);
	if (EvaluateRegion(active, &result)) {
		p_table_state->Player(chair)->set_active(p_string_match->IsStringActive(result));
	}
}

void CScraper::ScrapeColourCodes() {
  CString result;
  CString region;
  for (int i=0; i<p_tablemap->nchairs(); i++) {
    region.Format("p%icolourcode", i);
    if (EvaluateRegion(region, &result)) {
      p_table_state->Player(i)->set_colourcode(atoi(result));
    } else {
      p_table_state->Player(i)->set_colourcode(kUndefinedZero);
    }
  }
}

void CScraper::ScrapeSlider() {
	__HDC_HEADER
	RMapCI handleCI, slider;
	POINT handle_xy;
  // find handle
	handleCI = p_tablemap->r$()->find("i3handle");
	slider = p_tablemap->r$()->find("i3slider");
  if (handleCI!=p_tablemap->r$()->end() 
      && slider!=p_tablemap->r$()->end() 
      && p_casino_interface->BetsizeConfirmationButton()->IsClickable()) {
		p_casino_interface->_bet_slider.ResetHandlePosition();
		// Skipping now "true" evaluation of i3handle to support casinos that need first to click Bet or Raise button first
		// before to slide the bet: https://www.maxinmontreal.com/forums/viewtopic.php?p=191837#p191837
		handle_xy.x = handleCI->second.left;
		handle_xy.y = handleCI->second.top;
        p_casino_interface->_bet_slider.SetHandlePosition(handle_xy);
		write_log(Preferences()->debug_scraper(), "[CScraper] i3handle, result %d,%d\n", handle_xy.x, handle_xy.y);
        __HDC_FOOTER_ATTENTION_HAS_TO_BE_CALLED_ON_EVERY_FUNCTION_EXIT_OTHERWISE_MEMORY_LEAK
				return;
	}
	write_log(Preferences()->debug_scraper(), "[CScraper] i3handle, cannot find handle in the slider region...\n");
  __HDC_FOOTER_ATTENTION_HAS_TO_BE_CALLED_ON_EVERY_FUNCTION_EXIT_OTHERWISE_MEMORY_LEAK
}

int CScraper::CardString2CardNumber(CString card) {
	int result;
	if (StdDeck_stringToCard((char*) card.GetString(), &result)) {
    AssertRange(result, 0, 255);
	  return result;
  } else {
    return CARD_UNDEFINED;
  }
}

int CScraper::ScrapeCardface(CString base_name) {
  CString card_str;
  // Here name = base_name
	if (EvaluateRegion(base_name, &card_str)) {
		if (card_str != "") {
			return CardString2CardNumber(card_str);
		}
	}
  return CARD_UNDEFINED;
}

int CScraper::ScrapeCardByRankAndSuit(CString base_name) {
  CString rank = base_name + "rank";
	CString suit = base_name + "suit";
	CString rank_result, suit_result;
	// Scrape suit first (usually very fast colour-transform)
	if (EvaluateRegion(suit, &suit_result))	{
		// If a suit could not be recognized we don't need to scrape the rank at all
		// which is often an expensive fuzzy font in this case.
		if (IsSuitString(suit_result)) {
			EvaluateRegion(rank, &rank_result);
			if (IsRankString(rank_result))
			{
        if (rank_result == "10") {
          rank_result = "T";
        }
				CString card_str = rank_result + suit_result;
				return CardString2CardNumber(card_str);
			}
		}
	}
  return CARD_UNDEFINED;
}

int CScraper::ScrapeCardback(CString base_name) {
  if (base_name[0] == 'p')	{
	  CString cardback = base_name.Left(2) + "cardback";
	  CString cardback_result;
	  if (EvaluateRegion(cardback, &cardback_result)) {
	    if ((cardback_result == "cardback")
	        || (cardback_result == "true")) {
		    return CARD_BACK;
	    }
	  }
  }
  return CARD_UNDEFINED;
}

int CScraper::ScrapeNoCard(CString base_name){
  CString card_no_card = base_name + "nocard";
  CString no_card_result;
  if (EvaluateRegion(card_no_card, &no_card_result) 
		  && (no_card_result == "true"))	{
    write_log(Preferences()->debug_scraper(), "[CScraper] ScrapeNoCard(%s) -> true\n",
      card_no_card);
    return CARD_NOCARD;
  }
  write_log(Preferences()->debug_scraper(), "[CScraper] ScrapeNoCard(%s) -> false\n",
      card_no_card);
  return CARD_UNDEFINED;
}

// Cares about "everything"
//   * cardfaces
//   * ranks and suits
//   * cardbacks
int CScraper::ScrapeCard(CString name) {
  int result = CARD_UNDEFINED;
  if (p_tablemap->cardscrapemethod() == 1) {
    // Some casinos display additional cardbacks, 
    // even if a player has card-faces
    // For these casinos we have to scrape the faces first
    // http://www.maxinmontreal.com/forums/viewtopic.php?f=111&t=18539
    // This order of scraping (faces, backs, nocard)
    // always works, but has suboptimal performance
    result = ScrapeCardface(name);
    if (result != CARD_UNDEFINED) {
	  return result;
	}
	// Nextz: try to scrape suits and ranks individually
    result = ScrapeCardByRankAndSuit(name);
    if (result != CARD_UNDEFINED) {
      return result;
    }
  }
  // First: in case of player cards try to scrape card-backs
  // This has to be the very first one,
  // because some casinos use different locations for cardbacks and cards
  // http://www.maxinmontreal.com/forums/viewtopic.php?f=117&t=17960
  result = ScrapeCardback(name);
  if (result == CARD_BACK) {
    return CARD_BACK;
  }
  // Then try to scrape "no card"
  result = ScrapeNoCard(name);
  if (result == CARD_NOCARD) {
    return CARD_NOCARD;
  }
  if (p_tablemap->cardscrapemethod() != 1) {
    // If not already done so scrape card-faces
    // This order of scraping (backs, nocard, faces)
    // works for most casinos and has a very good performance
    result = ScrapeCardface(name);
    if (result != CARD_UNDEFINED) {
      return result;
    }
	// Again: try to scrape suits and ranks individually
    result = ScrapeCardByRankAndSuit(name);
    if (result != CARD_UNDEFINED) {
      return result;
    }
  }
  // Otherwise: in case of playercards try to scrape uXcardfaceY
  CString uname = name;
  if (name[0] == 'p')	{
    uname.SetAt(0, 'u');
	result = ScrapeCardface(uname);
    if (result != CARD_UNDEFINED) {
      return result;
    }
  }
  // Nothing found
  write_log(k_always_log_errors, 
    "[CScraper] WARNING ScrapeCard(%s) found nothing\n", name);
  write_log(k_always_log_errors, 
    "[CScraper] Not nocard, no cards and no cardbacks.\n");
  write_log(k_always_log_errors,
    "[CScraper] Defaulting to nocard\n");
  write_log(k_always_log_errors,
     "[CScraper] Please revisit your cards, especially nocard-regions.\n");
  // For some time we tried to be smart and returned
  //   * CARD_BACK for players
  //   * CARD_NOCARD for board-cards
  // in case of scraping-errors, as card-backs are more easy 
  // to get wrong than nocard (usually a simple colour-transform 
  // with background-colour and negative radius), 
  // but it looks as if this "smart" error-handling 
  // was the reason for wrong deal-positions for some beginners 
  // with bad tablemaps.
  // So we are back to simplicity.
  // http://www.maxinmontreal.com/forums/viewtopic.php?f=120&t=19109
  return CARD_NOCARD;
}

void CScraper::ScrapePlayerCards(int chair) {
	CString card_name, area_name;
	int card = CARD_UNDEFINED;
	int number_of_cards_to_be_scraped = kNumberOfCardsPerPlayerHoldEm;
	if (p_tablemap->SupportsOmaha()) {
		number_of_cards_to_be_scraped = kNumberOfCardsPerPlayerOmaha;
	}
	area_name.Format("area_cards_player%c", HexadecimalChar(chair));
	RMapCI		r_iter = p_tablemap->r$()->find(area_name);
	RMapCI		r_iter2 = p_tablemap->r$()->find("area_cards_common");
	vector<CString> result;
	if (r_iter != p_tablemap->r$()->end()) {
		int r_width = r_iter->second.right - r_iter->second.left;
		int r_height = r_iter->second.bottom - r_iter->second.top;
		if (r_width > 0 && r_height > 0)
			result = p_auto_ocr->GetDetectTemplatesResult(r_iter->second.name);
		int i = 0;
		for (auto & element : result) {
			int card = CardString2CardNumber(element);
			p_table_state->Player(chair)->hole_cards(i)->SetValue(card);
			i++;
		}
	}
	else {
		for (int i = 0; i < number_of_cards_to_be_scraped; i++) {
			card_name.Format("p%dcardface%d", chair, i);
			if ((i > 0)
				&& ((card == CARD_UNDEFINED) || (card == CARD_BACK) || (card == CARD_NOCARD))) {
				// Stop scraping if we find missing cards or cardbacks
			}
			else {
				card = ScrapeCard(card_name);
			}
			p_table_state->Player(chair)->hole_cards(i)->SetValue(card);
		}
	}
	p_table_state->Player(chair)->CheckPlayerCardsForConsistency();
}

void CScraper::ScrapeCommonCards() {
	RMapCI		r_iter = p_tablemap->r$()->find("area_cards_common");
	vector<CString> result;
	if (r_iter != p_tablemap->r$()->end()) {
		int r_width = r_iter->second.right - r_iter->second.left;
		int r_height = r_iter->second.bottom - r_iter->second.top;
		if (r_width > 0 && r_height > 0)
			result = p_auto_ocr->GetDetectTemplatesResult(r_iter->second.name);
		// Clear common cards first
		for (int i = 0; i < kNumberOfCommunityCards; i++) {
			p_table_state->CommonCards(i)->ClearValue();
		}
		// Populate common cards after, if there is some.
		int i = 0;
		for (auto & element : result) {
			int card = CardString2CardNumber(element);
			p_table_state->CommonCards(i)->SetValue(card);
			i++;
		}
	}
	else {
		CString card_name;
		for (int i = 0; i < kNumberOfCommunityCards; i++) {
			card_name.Format("c0cardface%d", i);
			int card = ScrapeCard(card_name);
			p_table_state->CommonCards(i)->SetValue(card);
		}
	}
}
	
// returns true if common cards are in the middle of an animation
bool CScraper::IsCommonAnimation(void) {
	int	flop_card_count = 0;

	// Count all the flop cards
	for (int i=0; i<kNumberOfFlopCards; i++) {
    if (p_table_state->CommonCards(i)->IsKnownCard()) {
			flop_card_count++;
		}
	}

	// If there are some flop cards present,
	// but not all 3 then there is an animation going on
	if (flop_card_count > 0 && flop_card_count < kNumberOfFlopCards) {
		return true;
	}
	// If the turn card is present,
	// but not all 3 flop cards are present then there is an animation going on
	else if (p_table_state->TurnCard()->IsKnownCard()
      && flop_card_count != kNumberOfFlopCards) {
		return true;
	}
	// If the river card is present,
	// but the turn card isn't
	// OR not all 3 flop cards are present then there is an animation going on
	else if (p_table_state->RiverCard()->IsKnownCard() 
      && (!p_table_state->TurnCard()->IsKnownCard() || flop_card_count != kNumberOfFlopCards)) {
		return true;
	}
	return false;
}

void CScraper::ClearAllPlayerNames() {
	for (int i=0; i<kMaxNumberOfPlayers; i++) {
    p_table_state->Player(i)->set_name("");
	}
}

void CScraper::ScrapeName(int chair) {
	RETURN_IF_OUT_OF_RANGE (chair, p_tablemap->LastChair())

	CString				result;
	CString				s = "";
	CString Separator = _T("|~|");
	CString Token, temp;
	CString bregex = "(^[\"\\(\\)\\[\\]<>{}#*-]*?)";
	CString eregex = "([c$��\"\\(\\)\\[\\]<>{}#*-]*?)([0-9.,]*)([c$��\"\\(\\)\\[\\]<>{}#*-]*?$)";
	int Position;

	// Player name uXname
	s.Format("u%dname", chair);
	EvaluateRegion(s, &result);
	write_log(Preferences()->debug_scraper(), "[CScraper] u%dname, result %s\n", chair, result.GetString());
	if (result != "") {
		Position = 0;
		Token = "null";
		temp = result;
		temp.MakeLower();
		while (!Token.IsEmpty())
		{
			// Get next token.
			Token = CString(Preferences()->unwanted_scrape()).Tokenize(Separator, Position);
			Token.MakeLower();
			//
			// Handle User RegEx and all exact user token matches
			if (regex_match((string)temp, regex(Token))) { return; }
			//
			//Handle all user tokens with bregex and eregex added
			if (regex_match((string)temp, regex(bregex + Token + eregex))) { return; }			
		}
		p_table_state->Player(chair)->set_name(result);
		return;
	}
	// Player name pXname
	s.Format("p%dname", chair);
	EvaluateRegion(s, &result);
	write_log(Preferences()->debug_scraper(), "[CScraper] p%dname, result %s\n", chair, result.GetString());
	if (result != "") {
		Position = 0;
		Token = "null";
		temp = result;
		temp.MakeLower();
		while (!Token.IsEmpty())
		{
			// Get next token.
			Token = CString(Preferences()->unwanted_scrape()).Tokenize(Separator, Position);
			Token.MakeLower();
			//
			// Handle User RegEx and all exact user token matches
			if (regex_match((string)temp, regex(Token))) { return; }
			//
			//Handle all user tokens with bregex and eregex added
			if (regex_match((string)temp, regex(bregex + Token + eregex))) { return; }
		}
		p_table_state->Player(chair)->set_name(result);
    return;
	}
}

CString CScraper::ScrapeUPBalance(int chair, char scrape_u_else_p) {
  CString	name;
  CString text;
  assert((scrape_u_else_p == 'u') || (scrape_u_else_p == 'p'));
  name.Format("%c%dbalance", scrape_u_else_p, chair);
  if (EvaluateRegion(name, &text)) {
		if (p_string_match->IsStringAllin(text)) { 
      write_log(Preferences()->debug_scraper(), "[CScraper] %s, result ALLIN", name);
       return Number2CString(0.0);
		}	else if (	text.MakeLower().Find("out")!=-1
				||	text.MakeLower().Find("inactive")!=-1
				||	text.MakeLower().Find("away")!=-1 ) {
			p_table_state->Player(chair)->set_active(false);
			write_log(Preferences()->debug_scraper(), "[CScraper] %s, result OUT/INACTIVE/AWAY\n", name);
      return Number2CString(kUndefinedZero);
		}	else {
      return text;
		}
	}
  // Number2CString(kUndefined) returns "-1",
  // which probably got converted to 1 by StringToMoney.
  // That's why we return an empty string "" again
  // to support all the people who don't scrape "nothing" as 0 or allin.
  // http://www.maxinmontreal.com/forums/viewtopic.php?f=124&t=20277
  return "";
}

void CScraper::ScrapeBalance(int chair) {
	RETURN_IF_OUT_OF_RANGE (chair, p_tablemap->LastChair())
  // Scrape uXbalance and pXbalance
  CString balance = ScrapeUPBalance(chair, 'p');
  if (p_table_state->Player(chair)->_balance.SetValue(balance)) {
    return;
  }
  balance = ScrapeUPBalance(chair, 'u');
  if (p_table_state->Player(chair)->_balance.SetValue(balance)) {
    return;
  }
}

void CScraper::ScrapeBet(int chair) {
  RETURN_IF_OUT_OF_RANGE (chair, p_tablemap->LastChair())

	__HDC_HEADER;
	CString				text = "";
	CString				s = "", t="";

	p_table_state->Player(chair)->_bet.Reset();
  // Player bet pXbet
  s.Format("p%dbet", chair);
  CString result;
  EvaluateRegion(s, &result);
	if (p_table_state->Player(chair)->_bet.SetValue(result)) {
		__HDC_FOOTER_ATTENTION_HAS_TO_BE_CALLED_ON_EVERY_FUNCTION_EXIT_OTHERWISE_MEMORY_LEAK
		return;
	}
  // uXbet
	s.Format("u%dbet", chair);
  result = "";
  EvaluateRegion(s, &result);
  if (p_table_state->Player(chair)->_bet.SetValue(result)) {
    __HDC_FOOTER_ATTENTION_HAS_TO_BE_CALLED_ON_EVERY_FUNCTION_EXIT_OTHERWISE_MEMORY_LEAK
      return;
  }
	// pXchip00
	s.Format("p%dchip00", chair);
	RMapCI r_iter = p_tablemap->r$()->find(s.GetString());
	if (r_iter != p_tablemap->r$()->end() && p_table_state->Player(chair)->_bet.GetValue() == 0) 	{
		old_bitmap = (HBITMAP) SelectObject(hdcCompatible, _entire_window_cur);
		double chipscrape_res = DoChipScrape(r_iter);
		SelectObject(hdcCompatible, old_bitmap);

		t.Format("%.2f", chipscrape_res);
		p_table_state->Player(chair)->_bet.SetValue(t.GetString());
		write_log(Preferences()->debug_scraper(), "[CScraper] p%dchipXY, result %f\n", 
      chair, p_table_state->Player(chair)->_bet.GetValue());
	}
	__HDC_FOOTER_ATTENTION_HAS_TO_BE_CALLED_ON_EVERY_FUNCTION_EXIT_OTHERWISE_MEMORY_LEAK
}

void CScraper::ScrapeAllPlayerCards() {
	for (int i=0; i<kMaxNumberOfPlayers; i++){
		for (int j=0; j<NumberOfCardsPerPlayer(); j++) {
			p_table_state->Player(i)->hole_cards(j)->ClearValue();
		}
	}
	write_log(Preferences()->debug_scraper(), "[CScraper] ScrapeAllPlayerCards()\n");
	for (int i=0; i<p_tablemap->nchairs(); i++) {
		write_log(Preferences()->debug_scraper(), "[CScraper] Calling ScrapePlayerCards, chair %d.\n", i);
		ScrapePlayerCards(i);
	}
}

void CScraper::ScrapePots() {
	__HDC_HEADER
	CString			text = "";
	CTransform	trans;
	CString			s = "", t="";
	RMapCI			r_iter = p_tablemap->r$()->end();

  p_table_state->ResetPots();
	for (int j=0; j<kMaxNumberOfPots; j++) {
		// r$c0potX
		s.Format("c0pot%d", j);
    CString result;
    EvaluateRegion(s, &result);
    if (p_table_state->set_pot(j, result)) {
      continue;
    }
		// r$c0potXchip00_index
		s.Format("c0pot%dchip00", j);
		r_iter = p_tablemap->r$()->find(s.GetString());
		if (r_iter != p_tablemap->r$()->end() && p_table_state->Pot(j) == 0) {
			ProcessRegion(r_iter);
			//old_bitmap = (HBITMAP) SelectObject(hdcCompatible, r_iter->second.cur_bmp);
			//trans.DoTransform(r_iter, hdcCompatible, &text);
			//SelectObject(hdcCompatible, old_bitmap);
      old_bitmap = (HBITMAP) SelectObject(hdcCompatible, _entire_window_cur);
			double chipscrape_res = DoChipScrape(r_iter);
			SelectObject(hdcCompatible, old_bitmap);
			t.Format("%.2f", chipscrape_res);
			p_table_state->set_pot(j, t.GetString());
			write_log(Preferences()->debug_scraper(), 
        "[CScraper] c0pot%dchipXY, result %f\n", j, p_table_state->Pot(j));

			// update the bitmap for second chip position in the first stack
			s.Format("c0pot%dchip01", j);
			r_iter = p_tablemap->r$()->find(s.GetString());
			if (r_iter != p_tablemap->r$()->end()) {
				ProcessRegion(r_iter);
      }
			// update the bitmap for first chip position in the second stack
			s.Format("c0pot%dchip10", j);
			r_iter = p_tablemap->r$()->find(s.GetString());
			if (r_iter != p_tablemap->r$()->end())
				ProcessRegion(r_iter);
		}
	}
	__HDC_FOOTER_ATTENTION_HAS_TO_BE_CALLED_ON_EVERY_FUNCTION_EXIT_OTHERWISE_MEMORY_LEAK
}

void CScraper::ScrapeMTTRegions() {
  assert(p_engine_container->symbol_engine_mtt_info() != NULL);
	CString result;
	if (EvaluateRegion("mtt_number_entrants", &result)) {	
		p_engine_container->symbol_engine_mtt_info()->set_mtt_number_entrants(result);
	}
	if (EvaluateRegion("mtt_players_remaining", &result)) {
		p_engine_container->symbol_engine_mtt_info()->set_mtt_players_remaining(result);
	}
	if (EvaluateRegion("mtt_my_rank", &result)) {
		p_engine_container->symbol_engine_mtt_info()->set_mtt_my_rank(result);
	}
	if (EvaluateRegion("mtt_paid_places", &result)) {
		p_engine_container->symbol_engine_mtt_info()->set_mtt_paid_places(result);
	}
	if (EvaluateRegion("mtt_largest_stack", &result)) {
		p_engine_container->symbol_engine_mtt_info()->set_mtt_largest_stack(result);
	}
	if (EvaluateRegion("mtt_average_stack", &result)) {
		p_engine_container->symbol_engine_mtt_info()->set_mtt_average_stack(result);
	}
	if (EvaluateRegion("mtt_smallest_stack", &result)) {
		p_engine_container->symbol_engine_mtt_info()->set_mtt_smallest_stack(result);
	}
}

void CScraper::ScrapeLimits() {
  assert(p_title_evaluator != NULL);
  p_title_evaluator->ClearAllDataOncePerHeartbeat();
  p_title_evaluator->EvaluateScrapedHandNumbers();
  p_title_evaluator->EvaluateTitleText();
  p_title_evaluator->EvaluateScrapedTitleTexts();
  p_title_evaluator->EvaluateScrapedGameInfo(); 
}

BOOL CScraper::SaveHBITMAPToFile(HBITMAP hBitmap, LPCTSTR lpszFileName)
{
	HDC hDC;
	int iBits;
	WORD wBitCount;
	DWORD dwPaletteSize = 0, dwBmBitsSize = 0, dwDIBSize = 0, dwWritten = 0;
	BITMAP Bitmap0;
	BITMAPFILEHEADER bmfHdr;
	BITMAPINFOHEADER bi;
	LPBITMAPINFOHEADER lpbi;
	HANDLE fh, hDib, hPal, hOldPal2 = NULL;
	hDC = CreateDC(TEXT("DISPLAY"), NULL, NULL, NULL);
	iBits = GetDeviceCaps(hDC, BITSPIXEL) * GetDeviceCaps(hDC, PLANES);
	DeleteDC(hDC);
	if (iBits <= 1)
		wBitCount = 1;
	else if (iBits <= 4)
		wBitCount = 4;
	else if (iBits <= 8)
		wBitCount = 8;
	else
		wBitCount = 24;
	GetObject(hBitmap, sizeof(Bitmap0), (LPSTR)&Bitmap0);
	bi.biSize = sizeof(BITMAPINFOHEADER);
	bi.biWidth = Bitmap0.bmWidth;
	bi.biHeight = -Bitmap0.bmHeight;
	bi.biPlanes = 1;
	bi.biBitCount = wBitCount;
	bi.biCompression = BI_RGB;
	bi.biSizeImage = 0;
	bi.biXPelsPerMeter = 0;
	bi.biYPelsPerMeter = 0;
	bi.biClrImportant = 0;
	bi.biClrUsed = 256;
	dwBmBitsSize = ((Bitmap0.bmWidth * wBitCount + 31) & ~31) / 8
		* Bitmap0.bmHeight;
	hDib = GlobalAlloc(GHND, dwBmBitsSize + dwPaletteSize + sizeof(BITMAPINFOHEADER));
	lpbi = (LPBITMAPINFOHEADER)GlobalLock(hDib);
	*lpbi = bi;

	hPal = GetStockObject(DEFAULT_PALETTE);
	if (hPal)
	{
		hDC = GetDC(NULL);
		hOldPal2 = SelectPalette(hDC, (HPALETTE)hPal, FALSE);
		RealizePalette(hDC);
	}


	GetDIBits(hDC, hBitmap, 0, (UINT)Bitmap0.bmHeight, (LPSTR)lpbi + sizeof(BITMAPINFOHEADER)
		+ dwPaletteSize, (BITMAPINFO *)lpbi, DIB_RGB_COLORS);

	if (hOldPal2)
	{
		SelectPalette(hDC, (HPALETTE)hOldPal2, TRUE);
		RealizePalette(hDC);
		ReleaseDC(NULL, hDC);
	}

	fh = CreateFile(lpszFileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);

	if (fh == INVALID_HANDLE_VALUE)
		return FALSE;

	bmfHdr.bfType = 0x4D42; // "BM"
	dwDIBSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + dwPaletteSize + dwBmBitsSize;
	bmfHdr.bfSize = dwDIBSize;
	bmfHdr.bfReserved1 = 0;
	bmfHdr.bfReserved2 = 0;
	bmfHdr.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + (DWORD)sizeof(BITMAPINFOHEADER) + dwPaletteSize;

	WriteFile(fh, (LPSTR)&bmfHdr, sizeof(BITMAPFILEHEADER), &dwWritten, NULL);

	WriteFile(fh, (LPSTR)lpbi, dwDIBSize, &dwWritten, NULL);
	GlobalUnlock(hDib);
	GlobalFree(hDib);
	CloseHandle(fh);
	return TRUE;
}

void CScraper::CreateBitmaps(void) {
	HDC				hdcScreen = CreateDC("DISPLAY", NULL, NULL, NULL);

	// Whole window
	RECT			cr = {0};
	GetClientRect(p_autoconnector->attached_hwnd(), &cr);
	_entire_window_last = CreateCompatibleBitmap(hdcScreen, cr.right, cr.bottom);
	_entire_window_cur = CreateCompatibleBitmap(hdcScreen, cr.right, cr.bottom);

	// r$regions
	for (RMapI r_iter=p_tablemap->set_r$()->begin(); r_iter!=p_tablemap->set_r$()->end(); r_iter++)
	{
		int w = r_iter->second.right - r_iter->second.left + 1;
		int h = r_iter->second.bottom - r_iter->second.top + 1;
		/*if (r_iter->second.transform[0] != 'A') {
			r_iter->second.last_bmp = CreateCompatibleBitmap(hdcScreen, w, h);
			r_iter->second.cur_bmp = CreateCompatibleBitmap(hdcScreen, w, h);
		}
		else {*/
			//w = w + 6;
			//h = h + 6;
			r_iter->second.last_bmp = CreateCompatibleBitmap(hdcScreen, w, h);
			r_iter->second.cur_bmp = CreateCompatibleBitmap(hdcScreen, w, h);
		//}
	}

	DeleteDC(hdcScreen);
}

void CScraper::DeleteBitmaps(void) {
	// Whole window
	DeleteObject(_entire_window_last);
  delete_entire_window_cur();

	// Common cards
	for (RMapI r_iter=p_tablemap->set_r$()->begin(); r_iter!=p_tablemap->set_r$()->end(); r_iter++)
	{
		DeleteObject(r_iter->second.last_bmp); r_iter->second.last_bmp=NULL;
		DeleteObject(r_iter->second.cur_bmp); r_iter->second.cur_bmp=NULL;
	}
}

// This is the chip scrape routine
const double CScraper::DoChipScrape(RMapCI r_iter) {
	HDC				hdc = GetDC(p_autoconnector->attached_hwnd());

	int				j = 0, stackindex = 0, chipindex = 0;
	int				hash_type = 0, pixcount = 0, chipwidth = 0, chipheight = 0;
	int				top = 0, bottom = 0, left = 0, right = 0;
	bool			stop_loop = false;
	uint32_t		*uresult = NULL, hash = 0, pix[MAX_HASH_WIDTH*MAX_HASH_HEIGHT] = {0};
	double			result = 0;
	CString			resstring = "";

	CString			type = "";
	int				vertcount = 0, horizcount = 0;
	RMapCI			r_start = p_tablemap->r$()->end();
	RMapCI			r_vert[10];
	RMapCI			r_horiz[10];
	CString			s = "";

	// Initialize arrays
	for (int j=0; j<10; j++)
	{
		r_vert[j] = p_tablemap->r$()->end();
		r_horiz[j] = p_tablemap->r$()->end();
	}

	// Check for bad parameters
	if (r_iter == p_tablemap->r$()->end())
	{
		ReleaseDC(p_autoconnector->attached_hwnd(), hdc);
		return 0.;
	}

	// figure out if we are dealing with a pot or playerbet here
	if (r_iter->second.name.Mid(0,5)=="c0pot" && r_iter->second.name.Mid(6,4)=="chip")
		type = r_iter->second.name.Mid(0,10);

	else if (r_iter->second.name.Mid(0,1)=="p" && r_iter->second.name.Mid(2,4)=="chip")
		type = r_iter->second.name.Mid(0,6);

	else
	{
		ReleaseDC(p_autoconnector->attached_hwnd(), hdc);
		return 0.;
	}

	// find start, vert stride, and horiz stride regions
	s.Format("%s00", type.GetString());
	r_start = p_tablemap->r$()->find(s.GetString());
	if (r_start == p_tablemap->r$()->end())
	{
		ReleaseDC(p_autoconnector->attached_hwnd(), hdc);
		return 0.;
	}

	for (int j = 1; j<=9; j++)
	{
		s.Format("%s0%d", type.GetString(), j);
		r_vert[j] = p_tablemap->r$()->find(s.GetString());
		if (r_vert[j] != p_tablemap->r$()->end())
			vertcount++;

		s.Format("%s%d0", type.GetString(), j);
		r_horiz[j] = p_tablemap->r$()->find(s.GetString());
		if (r_horiz[j] != p_tablemap->r$()->end())
			horizcount++;
	}

	hash_type = RightDigitCharacterToNumber(r_start->second.transform);

	// Bitblt the attached windows bitmap into a HDC
	HDC hdcScreen = CreateDC("DISPLAY", NULL, NULL, NULL);
	HDC hdcCompat = CreateCompatibleDC(hdcScreen);
	RECT rect;
	GetClientRect(p_autoconnector->attached_hwnd(), &rect);
	HBITMAP attached_bitmap = CreateCompatibleBitmap(hdcScreen, rect.right, rect.bottom);
	HBITMAP	old_bitmap = (HBITMAP) SelectObject(hdcCompat, attached_bitmap);
	BitBlt(hdcCompat, 0, 0, rect.right, rect.bottom, hdc, 0, 0, SRCCOPY);
	
	// Get chipscrapemethod option from tablemap, if specified
	CString res = p_tablemap->chipscrapemethod();
	CString cs_method = res.MakeLower();
	int cs_method_x = 0, cs_method_y = 0;
	if (cs_method!="" && cs_method!="all" && cs_method.Find("x")!=-1)
	{
		cs_method_x = strtoul(cs_method.Left(cs_method.Find("x")), NULL, 10);
		cs_method_y = strtoul(cs_method.Mid(cs_method.Find("x")+1), NULL, 10);
	}

	stop_loop = false;
	// loop through horizontal stacks
	for (stackindex=0; stackindex<MAX_CHIP_STACKS && !stop_loop; stackindex++)
	{
		// loop through vertical chips
		for (chipindex=0; chipindex<MAX_CHIPS_PER_STACK && !stop_loop; chipindex++)
		{

			// figure out left, right, top, bottom of next chip to be scraped
			if (vertcount==1)
			{
				top = r_start->second.top + chipindex*(r_vert[1]->second.top - r_start->second.top);
				bottom = r_start->second.bottom + chipindex*(r_vert[1]->second.bottom - r_start->second.bottom);
			}
			else
			{
				if (r_vert[chipindex+1] == p_tablemap->r$()->end())
				{
					stop_loop = true;
				}
				else
				{
					top = r_vert[chipindex+1]->second.top;
					bottom = r_vert[chipindex+1]->second.bottom;
				}
			}

			if (horizcount==1)
			{
				left = r_start->second.left + stackindex*(r_horiz[1]->second.left - r_start->second.left);
				right = r_start->second.right + stackindex*(r_horiz[1]->second.right - r_start->second.right);
			}
			else
			{
				if (r_horiz[stackindex+1] == p_tablemap->r$()->end())
				{
					stop_loop = true;
				}
				else
				{
					left = r_horiz[stackindex+1]->second.left;
					right = r_horiz[stackindex+1]->second.right;
				}
			}

			if (!stop_loop)
			{
				chipwidth = right - left + 1;
				chipheight = bottom - top + 1;

				// calculate hash
				if (hash_type>=1 && hash_type<=3)
				{
					pixcount = 0;
					for (PMapCI p_iter=p_tablemap->p$(hash_type)->begin(); p_iter!=p_tablemap->p$(hash_type)->end(); p_iter++)
					{
							int x = p_iter->second.x;
							int y = p_iter->second.y;

							if (x<chipwidth && y<chipheight)
								pix[pixcount++] = GetPixel(hdcCompat, left + x, top + y);
					}

					if (hash_type==1) hash = hashword(&pix[0], pixcount, HASH_SEED_1);
					else if (hash_type==2) hash = hashword(&pix[0], pixcount, HASH_SEED_2);
					else if (hash_type==3) hash = hashword(&pix[0], pixcount, HASH_SEED_3);
				}

				// lookup hash in h$ records
				HMapCI h_iter = p_tablemap->h$(hash_type)->find(hash);

				// no hash match
				if (h_iter == p_tablemap->h$(hash_type)->end())
				{
					// See if we should stop horiz or vert loops on a non-match
					if (cs_method == "")
					{
						// Stop horizontal scrape loop if chipindex==0 AND a non-match
						if (chipindex==0)
							stackindex = MAX_CHIP_STACKS+1;

						// stop vertical scrape loop on a non-match
						chipindex = MAX_CHIPS_PER_STACK+1;
					}
				}

				// hash match found
				else
				{
					resstring = h_iter->second.name;
					resstring.Remove(',');
					resstring.Remove('$');
					result += atof(resstring.GetString());
				}
			}

			// See if we should stop chip loop due to chipscrapemethod
			if (cs_method!="" && cs_method!="all" && chipindex>=cs_method_y)
				chipindex = MAX_CHIPS_PER_STACK+1;
		}

		// See if we should stop stack loop due to chipscrapemethod
		if (cs_method!="" && cs_method!="all" && stackindex>=cs_method_x)
			stackindex = MAX_CHIP_STACKS+1;

	}

	SelectObject(hdcCompat, old_bitmap);
	DeleteObject(attached_bitmap);
	DeleteDC(hdcCompat);
	DeleteDC(hdcScreen);

	ReleaseDC(p_autoconnector->attached_hwnd(), hdc);
	return result;
}

bool CScraper::IsExtendedNumberic(CString text) {
  bool currently_unused = false;
  assert(currently_unused);
  return false;
}

bool CScraper::IsIdenticalScrape() {
  __HDC_HEADER

	// Get bitmap of whole window
	RECT		cr = {0};
	GetClientRect(p_autoconnector->attached_hwnd(), &cr);

	old_bitmap = (HBITMAP) SelectObject(hdcCompatible, _entire_window_cur);
	BitBlt(hdcCompatible, 0, 0, cr.right, cr.bottom, hdc, cr.left, cr.top, SRCCOPY);
	SelectObject(hdcCompatible, old_bitmap);

  p_table_state->TableTitle()->UpdateTitle();
	
	// If the bitmaps are the same, then return now
	// !! How often does this happen?
	// !! How costly is the comparison?
	if (BitmapsAreEqual(_entire_window_last, _entire_window_cur) 
      && !p_table_state->TableTitle()->TitleChangedSinceLastHeartbeat()) 	{
		DeleteDC(hdcCompatible);
		DeleteDC(hdcScreen);
		ReleaseDC(p_autoconnector->attached_hwnd(), hdc);
		write_log(Preferences()->debug_scraper(), "[CScraper] IsIdenticalScrape() true\n");
    __HDC_FOOTER_ATTENTION_HAS_TO_BE_CALLED_ON_EVERY_FUNCTION_EXIT_OTHERWISE_MEMORY_LEAK
		return true;
	}
	// Copy into "last" bitmap
	old_bitmap = (HBITMAP) SelectObject(hdcCompatible, _entire_window_last);
	BitBlt(hdcCompatible, 0, 0, cr.right-cr.left+1, cr.bottom-cr.top+1, hdc, cr.left, cr.top, SRCCOPY);
	SelectObject(hdc, old_bitmap);

	__HDC_FOOTER_ATTENTION_HAS_TO_BE_CALLED_ON_EVERY_FUNCTION_EXIT_OTHERWISE_MEMORY_LEAK
	write_log(Preferences()->debug_scraper(), "[CScraper] IsIdenticalScrape() false\n");
	return false;
}

#undef __HDC_HEADER 
#undef __HDC_FOOTER_ATTENTION_HAS_TO_BE_CALLED_ON_EVERY_FUNCTION_EXIT_OTHERWISE_MEMORY_LEAK