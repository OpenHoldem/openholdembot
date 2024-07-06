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
#include "CSymbolEngineBlinds.h"

#include "CEngineContainer.h"
#include "CScraper.h"
#include "CSymbolEngineActiveDealtPlaying.h"
#include "CSymbolEngineDealerchair.h"
#include "CSymbolEnginePositions.h"
#include "CSymbolEngineUserchair.h"
#include "CSymbolEngineTableLimits.h"
#include "CTableState.h"


CSymbolEngineBlinds::CSymbolEngineBlinds()
{
	// The values of some symbol-engines depend on other engines.
	// As the engines get later called in the order of initialization
	// we assure correct ordering by checking if they are initialized.
	assert(p_engine_container->symbol_engine_dealerchair() != NULL);
	assert(p_engine_container->symbol_engine_positions() != NULL);
	assert(p_engine_container->symbol_engine_tablelimits() != NULL);
	assert(p_engine_container->symbol_engine_userchair() != NULL);
}

CSymbolEngineBlinds::~CSymbolEngineBlinds()
{}

void CSymbolEngineBlinds::InitOnStartup()
{
	UpdateOnConnection();
}

void CSymbolEngineBlinds::UpdateOnConnection()
{
	UpdateOnHandreset();
}

void CSymbolEngineBlinds::UpdateOnHandreset()
{
	_playersblindbits = 0;
	_bblindbits = 0;
}

void CSymbolEngineBlinds::UpdateOnNewRound()
{}

bool CSymbolEngineBlinds::BlindsAreUnknown()
{
	return (_playersblindbits == 0
		||_bblindbits == 0);

}

void CSymbolEngineBlinds::UpdateOnMyTurn()
{
	// Only updating when it is my turn (stable frames)
	// and blinds are unknown
	if (BlindsAreUnknown())
	{
		CalculateBlinds();
	}
}

void CSymbolEngineBlinds::UpdateOnHeartbeat()
{}

int CSymbolEngineBlinds::opponentsblindbits() { 
  return _playersblindbits & ~p_engine_container->symbol_engine_userchair()->userchairbit(); 
}

void CSymbolEngineBlinds::CalculateBlinds()
{
	int sbchair = kUndefined;
	int bbchair = kUndefined;

	// Heads-Up
	if (p_engine_container->symbol_engine_active_dealt_playing()->playersdealtbits() == 2 && (p_engine_container->symbol_engine_active_dealt_playing()->nplayersdealt() & (1<<DEALER_CHAIR)))
	{	
		int sbchair = DEALER_CHAIR;
		_bblindbits = p_engine_container->symbol_engine_active_dealt_playing()->nplayersdealt() ^ k_exponents[sbchair];
		_playersblindbits = p_engine_container->symbol_engine_active_dealt_playing()->nplayersdealt();
	}

	else
	{
		// Is Hero SB or BB ?
		double my_bet = p_table_state->User()->_bet.GetValue();

		if (my_bet <= p_engine_container->symbol_engine_tablelimits()->sblind() && my_bet > 0)
		{
			sbchair = p_engine_container->symbol_engine_userchair()->userchair();
			_playersblindbits = k_exponents[p_engine_container->symbol_engine_userchair()->userchair()];
		}

		if (my_bet <= p_engine_container->symbol_engine_tablelimits()->bblind() && my_bet > p_engine_container->symbol_engine_tablelimits()->sblind())
		{
			bbchair = p_engine_container->symbol_engine_userchair()->userchair();
			_bblindbits = k_exponents[bbchair];
			_playersblindbits = k_exponents[p_engine_container->symbol_engine_userchair()->userchair()];
		}

		for (int i=DEALER_CHAIR+1; i<DEALER_CHAIR+p_tablemap->nchairs(); i++)
		{
			int chair = i%p_tablemap->nchairs();
			double p_bet = p_table_state->Player(chair)->_bet.GetValue();

			// search SB
			if (sbchair == kUndefined && p_bet <= p_engine_container->symbol_engine_tablelimits()->sblind() && p_bet > 0) 
			{
				sbchair = chair;
				_playersblindbits |= k_exponents[sbchair];		
			}
			// search BB
			if (bbchair == kUndefined && p_bet <= p_engine_container->symbol_engine_tablelimits()->bblind() && p_bet > p_engine_container->symbol_engine_tablelimits()->sblind() && chair != sbchair)
			{
				bbchair = chair;	
				_bblindbits = k_exponents[bbchair];
				_playersblindbits |= k_exponents[bbchair];		
				
			}
		}
		
		// SB not found correction.
		// Will only apply if we are the bb + missed action(s). most common case. 
		// Restrictions : 3 or less players were dealt or last bb is active
		if (sbchair == kUndefined && (p_engine_container->symbol_engine_active_dealt_playing()->playersdealtbits() < 3 || (bbchair == p_engine_container->symbol_engine_userchair()->userchair() && p_engine_container->symbol_engine_positions()->nchairsdealtright() == 1)))
		{
			for (int i=DEALER_CHAIR+1; i<DEALER_CHAIR+p_tablemap->nchairs(); i++)
			{
				int chair = i%p_tablemap->nchairs();
				double p_bet = p_table_state->Player(chair)->_bet.GetValue();

				// 1st caller/raiser after dealer is sb
				if (p_bet >= p_engine_container->symbol_engine_tablelimits()->bblind() && sbchair == kUndefined && chair != bbchair)
				{
					sbchair = chair;
					_playersblindbits |= k_exponents[sbchair];
				}
			}
		}
	}							
}

bool CSymbolEngineBlinds::EvaluateSymbol(const CString name, double *result, bool log /* = false */)
{
  FAST_EXIT_ON_OPENPPL_SYMBOLS(name);
	if (memcmp(name, "nopponentsblind", 15)==0 && strlen(name)==15)
	{
		*result = nopponentsblind();
	}
	else if (memcmp(name, "nplayersblind", 13)==0 && strlen(name)==13)	
	{
		*result = nplayersblind();
	}
	else if (memcmp(name, "playersblindbits", 16)==0 && strlen(name)==16)	
	{
		*result = playersblindbits();
	}
	else if (memcmp(name, "opponentsblindbits", 18)==0 && strlen(name)==18)	
	{
		*result = opponentsblindbits();
	}
	else if (memcmp(name, "bblindbits", 10)==0 && strlen(name)==10)  	
	{
		*result = bblindbits();
	}
	else
	{
		// Invalid symbol
		return false;
	}
	// Valid symbol
	return true;
}

CString CSymbolEngineBlinds::SymbolsProvided() {
  return "nopponentsblind nplayersblind playersblindbits opponentsblindbits bblindbits ";
}