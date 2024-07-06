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
#include "CSymbolEngineActiveDealtPlaying.h"

#include "CCasinoInterface.h"
#include "CEngineContainer.h"

#include "CScraper.h"
#include "CStringMatch.h"
#include "CSymbolEngineDealerchair.h"
#include "CSymbolEngineUserchair.h"
#include "CSymbolEngineTableLimits.h"
#include "CTablemap/CTablemap.h"
#include "CTableState.h"


CSymbolEngineActiveDealtPlaying::CSymbolEngineActiveDealtPlaying()
{
	// The values of some symbol-engines depend on other engines.
	// As the engines get later called in the order of initialization
	// we assure correct ordering by checking if they are initialized.
	assert(p_engine_container->symbol_engine_dealerchair() != NULL);
	assert(p_engine_container->symbol_engine_tablelimits() != NULL);
	assert(p_engine_container->symbol_engine_userchair() != NULL);
}

CSymbolEngineActiveDealtPlaying::~CSymbolEngineActiveDealtPlaying()
{}

void CSymbolEngineActiveDealtPlaying::InitOnStartup() {
	UpdateOnConnection();
}

void CSymbolEngineActiveDealtPlaying::UpdateOnConnection() {
  UpdateOnHandreset();
  _maxnplayersdealt   = 0;   
}

void CSymbolEngineActiveDealtPlaying::UpdateOnHandreset() {
  _playersactivebits = 0;
  _playersplayingbits = 0;
  _playersdealtbits = 0;
  _playersseatedbits = 0;
  _playersallinbits = 0;
}

void CSymbolEngineActiveDealtPlaying::UpdateOnNewRound()
{}

void CSymbolEngineActiveDealtPlaying::UpdateOnMyTurn()
{
    CalculateActiveBits();
    CalculatePlayingBits();
    CalculateDealtBits();
    CalculateSeatedBits();
    CalculateAllinBits();
}

void CSymbolEngineActiveDealtPlaying::UpdateOnHeartbeat() {
    CalculateActiveBits();
    CalculatePlayingBits();
    CalculateDealtBits();
    CalculateSeatedBits();
    CalculateAllinBits();
}

void CSymbolEngineActiveDealtPlaying::CalculateActiveBits()
{
	_playersactivebits  = 0;
	for (int i=0; i<kMaxNumberOfPlayers; i++)
	{
		if (p_table_state->Player(i)->active())
		{
			_playersactivebits |= (1<<i);			
		}
	}
}

void CSymbolEngineActiveDealtPlaying::CalculatePlayingBits() {
	_playersplayingbits = 0;
	for (int i=0; i<kMaxNumberOfPlayers; ++i) {
        if (p_table_state->Player(i)->HasAnyCards()) {
			_playersplayingbits |= (1<<i);
		}
	}
}

void CSymbolEngineActiveDealtPlaying::CalculateSeatedBits() {
	_playersseatedbits = 0;
	for (int i=0; i<kMaxNumberOfPlayers; i++)	{
		if (p_table_state->Player(i)->seated())	{
			_playersseatedbits |= 1<<i;			
		}
	}
	AssertRange(_playersseatedbits, 0, k_bits_all_ten_players_1_111_111_111);
}

void CSymbolEngineActiveDealtPlaying::CalculateAllinBits() {
  _playersallinbits = 0;
  // First: simply check for all players with no balance
  for (int i = 0; i<kMaxNumberOfPlayers; i++) {
    if (p_table_state->Player(i)->IsAllin()) {
      _playersallinbits |= 1 << i;
    }
  }
  AssertRange(_playersseatedbits, 0, k_bits_all_ten_players_1_111_111_111);
}

int CSymbolEngineActiveDealtPlaying::userchairbit() { 
  return 1 << p_engine_container->symbol_engine_userchair()->userchair(); 
}

void CSymbolEngineActiveDealtPlaying::CalculateDealtBits() {
	int  number_of_blind_posters_found = 0;
	bool big_blind_found = false;
	bool first_non_blind_with_cards_found = false;

	for (int i=0; i<p_tablemap->nchairs(); i++) {
		int chair_to_consider = (DEALER_CHAIR + i + 1) % p_tablemap->nchairs();
		bool this_player_got_dealt = false;
    // Players with cards are always "dealt",
    // independent of the rest of following complicated logic,
    // which might fail, especially in case of GIGO.
    if (p_table_state->Player(chair_to_consider)->HasAnyCards()) {
      this_player_got_dealt = true;
    }
		// First we search the blinds only, 
		// i.e. players with a positive bet.
		// We don't consider players who are only "active",
		// i.e. players who sat out but came back.
		if ((number_of_blind_posters_found < kUsualNumberOfBlindPosters) && ! big_blind_found) {
			double bet = p_table_state->Player(chair_to_consider)->_bet.GetValue();
			if (bet > 0) {
        write_log(Preferences()->debug_symbolengine(),
          "[CSymbolEngineActiveDealtPlaying] CalculateDealtBits() chair %i is a blind poster\n",
          chair_to_consider);
				number_of_blind_posters_found++;
				this_player_got_dealt = true;
			}
			if ((bet == BIG_BLIND) || (number_of_blind_posters_found == kUsualNumberOfBlindPosters)) {
				// big blind might be allin for less than 1 bb
				// or small blind might be missing.
				// But we catch both cases, as long as not both happen
				// at the same time.
				big_blind_found = true;
			}
		}
		// Once the blinds have been found we accept every player as dealt
		// who is active. Usually these are name fast fold,
		// though there might be some people who were sitting out
		// and came back.
		// We do so, until we reach somebody who has cards.
		// After this player we look for cards only,
		// because there can be no quick folds after him.
		else if (p_table_state->Player(chair_to_consider)->HasAnyCards()) {
      // Player with cards found
      write_log(Preferences()->debug_symbolengine(),
        "[CSymbolEngineActiveDealtPlaying] CalculateDealtBits() chair %i holds cards, therefore dealt\n",
        chair_to_consider);
			this_player_got_dealt = true;
      first_non_blind_with_cards_found = true;
		} else {
      // Player has no cards
      if (first_non_blind_with_cards_found == false) {
        // Not yet anybody with cards outside the blinds found
        // Consider active players as dealt with fast folds.
			  if (p_table_state->Player(chair_to_consider)->active()) {
          write_log(Preferences()->debug_symbolengine(),
            "[CSymbolEngineActiveDealtPlaying] CalculateDealtBits() chair %i is active after the blinds, probably dealt and fast fold\n",
            chair_to_consider);
				  this_player_got_dealt = true;    
        }
			}
		}
		if (this_player_got_dealt) {
			_playersdealtbits |= 1 << chair_to_consider;
			AssertRange(_playersdealtbits, 0, k_bits_all_ten_players_1_111_111_111);
		}
	}
  write_log(Preferences()->debug_symbolengine(),
    // _playersdealtbits once as decimal, once as 4-digit hexadecimal
    "[CSymbolEngineActiveDealtPlaying] playersdealtbits = %i = %#4x\n",
    _playersdealtbits, _playersdealtbits);
  if (nplayersdealt() > _maxnplayersdealt) {
    _maxnplayersdealt = nplayersdealt();
  }
}

bool CSymbolEngineActiveDealtPlaying::EvaluateSymbol(const CString name, double *result, bool log /* = false */)
{
  FAST_EXIT_ON_OPENPPL_SYMBOLS(name);
	if (memcmp(name, "nopponents", 10)==0)
	{
		if (memcmp(name, "nopponentsseated", 16)==0 && strlen(name)==16)	
		{
			*result = nopponentsseated();
		}
		else if (memcmp(name, "nopponentsactive", 16)==0 && strlen(name)==16)	
		{
			*result = nopponentsactive();
		}
		else if (memcmp(name, "nopponentsdealt", 15)==0 && strlen(name)==15)	
		{
			*result = nopponentsdealt();
		}
		else if (memcmp(name, "nopponentsplaying", 17)==0 && strlen(name)==17)	
		{
			*result = nopponentsplaying();
		}
    else if (memcmp(name, "nopponentsallin", 15) == 0 && strlen(name) == 15)
    {
      *result = nopponentsallin();
    }
		else
		{
			// Invalid symbol
			return false;
		}
		// Valid symbol
		return true;
	}
	else if (memcmp(name, "nplayers", 8)==0)
	{
		if (memcmp(name, "nplayersseated", 14)==0 && strlen(name)==14)
		{
			*result = nplayersseated();
		}
		else if (memcmp(name, "nplayersactive", 14)==0 && strlen(name)==14)
		{
			*result = nplayersactive();
		}
		else if (memcmp(name, "nplayersdealt", 13)==0 && strlen(name)==13)	
		{
			*result = nplayersdealt();
		}
		else if (memcmp(name, "nplayersplaying", 15)==0 && strlen(name)==15)
		{
			*result = nplayersplaying();
		}
    else if (memcmp(name, "nplayersallin", 13) == 0 && strlen(name) == 13)
    {
      *result = nplayersallin();
    }
		else
		{
			// Invalid symbol
			return false;
		}
		// Valid symbol
		return true;
	}
	else if (memcmp(name, "players", 7)==0)
	{
		if (memcmp(name, "playersseatedbits", 17)==0 && strlen(name)==17)	
		{
			*result = playersseatedbits();
		}
		else if (memcmp(name, "playersactivebits", 17)==0 && strlen(name)==17)	
		{
			*result = playersactivebits();
		}
		else if (memcmp(name, "playersdealtbits", 16)==0 && strlen(name)==16)	
		{
			*result = playersdealtbits();
		}
		else if (memcmp(name, "playersplayingbits", 18)==0 && strlen(name)==18)
		{
			*result = playersplayingbits();
		}
    else if (memcmp(name, "playersallinbits", 16) == 0 && strlen(name) == 16)
    {
      *result = playersallinbits();
    }
		else
		{
			// Invalid symbol
			return false;
		}
		// Valid symbol
		return true;
	}
	else if (memcmp(name, "opponents", 9)==0)
	{
		if (memcmp(name, "opponentsseatedbits", 19)==0 && strlen(name)==19)	
		{
			*result = opponentsseatedbits();
		}
		else if (memcmp(name, "opponentsactivebits", 19)==0 && strlen(name)==19)	
		{
			*result = opponentsactivebits();
		}
		else if (memcmp(name, "opponentsdealtbits", 18)==0 && strlen(name)==18)	
		{
			*result = opponentsdealtbits();
		}
		else if (memcmp(name, "opponentsplayingbits", 20)==0 && strlen(name)==20)
		{
			*result = opponentsplayingbits();
		}
    else if (memcmp(name, "opponentsallinbits", 18) == 0 && strlen(name) == 18)
    {
      *result = opponentsallinbits();
    }
		else
		{
			// Invalid symbol
			return false;
		}
		// Valid symbol
		return true;
	} else if (memcmp(name, "maxnplayersdealt", 16)==0 && strlen(name)==16) {
    *result = _maxnplayersdealt;
    return true;
  }
	// Symbol of a different symbol-engine
	return false;
}

CString CSymbolEngineActiveDealtPlaying::SymbolsProvided() {
  return "nopponentsseated nopponentsactive nopponentsdealt nopponentsplaying nopponentsallin "
    "nplayersseated nplayersactive nplayersdealt nplayersplaying nplayersallin "
    "playersseatedbits playersactivebits playersdealtbits playersplayingbits playersallinbits "
    "opponentsseatedbits opponentsactivebits opponentsdealtbits opponentsplayingbits opponentsallinbits "
    "maxnplayersdealt ";
}