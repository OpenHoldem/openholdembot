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
#include "CLazyScraper.h"

#include "CAutoplayer.h"
#include "CCasinoInterface.h"
#include "CEngineContainer.h"
#include "CHandresetDetector.h"

#include "CScraper.h"
#include "CSymbolEngineActiveDealtPlaying.h"
#include "CSymbolEngineHistory.h"
#include "CSymbolEngineIsTournament.h"
#include "CSymbolEngineGameType.h"
#include "CSymbolEngineTime.h"
#include "CSymbolEngineUserchair.h"
#include "CTableState.h"


CLazyScraper *p_lazyscraper = NULL;

CLazyScraper::CLazyScraper() {
  _is_identical_scrape = false;
}

CLazyScraper::~CLazyScraper() {
}

// Some ideas:
// ===========
//
// Things we need to scrape every heartbeat,
// depending on handreset-method:
//   * handnumber
//   * action-buttons (standard, no betpot)
//   * cards of the userchair
//
// Things we need to scrape if the userchair is not yet known:
//   * action-buttons (standard, no betpot)
//   * all players cards
//
// Things we need to scrape on our turn
//   * betpot-buttons
//   * slider
// (only if the game-type is NL or PL)
//
// Once per every heartbeat (cash-game):
//   * scrape "seated" for all chairs
//   * scrape "active" for all seated chairs
//   * scrape cards for all active chairs
//   * scrape bets and balances for all active chairs
// (but we might skip this if we have folded 
// and the hand-history-generator is disabled)
//
// Tournaments:
//   * scrape bets and balances for all chairs when our turn because of ICM
//
// Name-scraping
// Preflop up to our first action only, because:
//   * once it is our turn we have stable frames
//   * later name-changes (new players) don't affect the game
//   * potential occlusion later can no longer affect PT
//   * improved CPU-usage as name-scraping needs about 40%
//
// DLLs and/or HH-gen might need all data all the time.
// If in doubt be conservative.

void CLazyScraper::DoScrape() {
	if (p_scraper->IsIdenticalScrape())	{
		_is_identical_scrape = true;
    return;
	}
  _is_identical_scrape = false;
	p_scraper->ScrapeLimits();
	if (NeedDealerChair()) { 
		p_scraper->ScrapeDealer();
	}
	if (NeedUsersCards())	{
		assert(p_engine_container->symbol_engine_userchair()->userchair_confirmed());
		p_scraper->ScrapePlayerCards(p_engine_container->symbol_engine_userchair()->userchair());
	}
	p_scraper->ScrapeSeatedActive();
	if (NeedAllPlayersCards()) {
		p_scraper->ScrapeAllPlayerCards(); 
	}
	if (NeedCommunityCards())	{
		p_scraper->ScrapeCommonCards();
	}

	// Scrape buttons ////
	bool area_found = false;
	CString area_name;
	RMapCI		r_iter = p_tablemap->r$()->end();

	// Reinitialize all buttons
	for (int i = 0; i<k_max_number_of_buttons; i++) {
		p_casino_interface->_technical_autoplayer_buttons[i].SetState("");
		p_casino_interface->_technical_autoplayer_buttons[i].SetLabel("");
	}
	for (int i = 0; i<k_max_betpot_buttons; i++) {
		p_casino_interface->_technical_betpot_buttons[i].SetState("");
		p_casino_interface->_technical_betpot_buttons[i].SetLabel("");
	}
	for (int i = 0; i<k_max_number_of_i86X_buttons; i++) {
		p_casino_interface->_technical_i86X_spam_buttons[i].SetState("");
		p_casino_interface->_technical_i86X_spam_buttons[i].SetLabel("");
	}

	// Search and scrape templates first if available
	for (int i = 0; i < k_max_area_buttons_zone; i++) {
		area_name.Format("area_buttons_zone%c", HexadecimalChar(i));
		r_iter = p_tablemap->r$()->find(area_name);
		if (r_iter != p_tablemap->r$()->end()) {
			int r_width = r_iter->second.right - r_iter->second.left;
			int r_height = r_iter->second.bottom - r_iter->second.top;
			if (r_width > 0 && r_height > 0) {
				area_found = true;
				if (NeedActionbuttons())
					p_scraper->ScrapeButtons(area_name, "action");
				if (NeedBetpotButtons())
					p_scraper->ScrapeButtons(area_name, "betpot");
				if (NeedInterfaceButtons())
					p_scraper->ScrapeButtons(area_name, "spam");
			}
		}
	}
	// Else usual button scrape
	if (!area_found) {
		if (NeedFoldButton())	{
			// For fast detection of my turn
			// Currently included in NeedActionbuttons()
		// No extra-scrape of fold-button for improved reaction time
		}
		if (NeedActionbuttons()) {
			p_scraper->ScrapeActionButtons();
			p_scraper->ScrapeActionButtonLabels();
		}
		if (NeedInterfaceButtons())	{
			p_scraper->ScrapeInterfaceButtons();
		}
		if (NeedBetpotButtons()) {
			p_scraper->ScrapeBetpotButtons();
		}
	}
	// End Scrape buttons ////


	if (NeedSlider())	{
		p_scraper->ScrapeSlider();
	}
	// Swagbox AKA i3edit does not need to be scraped
	// The CasinoInterface checks the existence and uses this region automatically
	if (NeedBetsAndBalances()) {
		p_scraper->ScrapeBetsAndBalances();
		p_scraper->ScrapePots();
	}
	if (NeedAllPlayerNames())	{
		p_scraper->ClearAllPlayerNames();
		ScrapeUnknownPlayerNames();
	}
	if (NeedUnknownPlayerNames())	{
		ScrapeUnknownPlayerNames();
	}
  if (NeedColourCodes()) {
    p_scraper->ScrapeColourCodes();
  }
  if (NeedMTTRegions()) {
    p_scraper->ScrapeMTTRegions();
  }
}

bool CLazyScraper::NeedDealerChair() {
	return true;
}

bool CLazyScraper::NeedHandNumber() {
	return true;
}

bool CLazyScraper::NeedUsersCards() {
	return (p_engine_container->symbol_engine_userchair()->userchair_confirmed());
}

bool CLazyScraper::NeedAllPlayersCards() {
	// User playing
	// HH-gen
	// DLL?
	return true;
}

bool CLazyScraper::NeedFoldButton() {
	// To detect my turn
	return true;
}

bool CLazyScraper::NeedActionbuttons() {
	// If MyTurn
	return true;
}

bool CLazyScraper::NeedInterfaceButtons() {
	return true;
}

bool CLazyScraper::NeedBetpotButtons() {
	return (p_casino_interface->IsMyTurn()
		&& (p_engine_container->symbol_engine_gametype()->isnl() || p_engine_container->symbol_engine_gametype()->ispl()));
}

bool CLazyScraper::NeedSlider() {
	return NeedBetpotButtons(); 
}

bool CLazyScraper::NeedBetsAndBalances() {
	return true;
}

bool CLazyScraper::NeedAllPlayerNames() {
	// Scraping names is the most costly part and mostly needed for PokerTracker.
	// It is enough if we do this until our turn, because
	// * at our turn we have stable frames
	// * new players after our turn can't affect the current hand
  if (p_engine_container->symbol_engine_history()->DidActThisHand()) {
    return false;
  }
  // We can also stop scraping names if we see new cards 
  // after a hand-reset because then a poterntial new player
  // can no longer join the game.
  if ((p_engine_container->symbol_engine_time()->elapsedhand() > 2)
      && (p_engine_container->symbol_engine_active_dealt_playing()->nplayersdealt() >= 2)) {
    return false;
  }
  return true;
}

bool CLazyScraper::NeedUnknownPlayerNames() {
	// Always true
	// If a new player sits down or we missed an existing player
	// due to occlusion we should always scrape that name.
	return true;
}

bool CLazyScraper::NeedCommunityCards() {
	return true;
}

void CLazyScraper::ScrapeUnknownPlayerNames() {
	for (int i=0; i<p_tablemap->nchairs(); i++) {
		if (p_table_state->Player(i)->seated()
			  && (p_table_state->Player(i)->name() == "")) {
			p_scraper->ScrapeName(i);
		}
	}
}

bool CLazyScraper::NeedColourCodes() {
  // Scrape colour-codes at the beginning of a session 
  // and at my turn -- that's enough.
  return (p_casino_interface->IsMyTurn()
    || (p_handreset_detector->hands_played() <= 1));
}

bool CLazyScraper::NeedMTTRegions() {
  // return when it is our turn
  // or if we have played less than 3 hands (for possible mtt detect)
  return (p_casino_interface->IsMyTurn()
	  || p_handreset_detector->hands_played() < 3);
}