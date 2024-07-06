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
#include "CSymbolEnginePokerAction.h"

#include "CBetroundCalculator.h"
#include "CEngineContainer.h"
#include "CPokerTrackerLookUp.h"
#include "CSymbolEngineActiveDealtPlaying.h"
#include "CSymbolEngineChipAmounts.h"
#include "CSymbolEngineDealerchair.h"
#include "CSymbolEnginePositions.h"
#include "CSymbolEngineRaisers.h"
#include "StringFunctions/string_functions.h"

CSymbolEnginePokerAction::CSymbolEnginePokerAction() {
  // The values of some symbol-engines depend on other engines.
	// As the engines get later called in the order of initialization
	// we assure correct ordering by checking if they are initialized.
	assert(p_engine_container->symbol_engine_active_dealt_playing() != NULL);
  assert(p_engine_container->symbol_engine_chip_amounts() != NULL);
  assert(p_engine_container->symbol_engine_dealerchair() != NULL);
  assert(p_engine_container->symbol_engine_positions() != NULL);
  assert(p_engine_container->symbol_engine_raisers() != NULL);
}

CSymbolEnginePokerAction::~CSymbolEnginePokerAction() {
}

void CSymbolEnginePokerAction::InitOnStartup() {
}

void CSymbolEnginePokerAction::UpdateOnConnection() {
}

void CSymbolEnginePokerAction::UpdateOnHandreset() {
}

void CSymbolEnginePokerAction::UpdateOnNewRound() {
}

void CSymbolEnginePokerAction::UpdateOnMyTurn() {
}

void CSymbolEnginePokerAction::UpdateOnHeartbeat() {
}

const int CSymbolEnginePokerAction::PreflopPos() {
	int		sym_nplayersdealt = p_engine_container->symbol_engine_active_dealt_playing()->nplayersdealt();
	int		sym_dealposition  = p_engine_container->symbol_engine_positions()->dealposition();
	

	//SB=1 BB=2 Early=3 Middle=4 Late=5 Dealer=6
	return
		sym_nplayersdealt==10 ? (sym_dealposition==1 ? 1 :
								 sym_dealposition==2 ? 2 :
								 sym_dealposition==3 ? 3 :
								 sym_dealposition==4 ? 3 :
								 sym_dealposition==5 ? 3 :
								 sym_dealposition==6 ? 4 :
								 sym_dealposition==7 ? 4 :
								 sym_dealposition==8 ? 5 :
								 sym_dealposition==9 ? 5 :
								 sym_dealposition==10 ? 6 : 0):
		sym_nplayersdealt==9  ? (sym_dealposition==1 ? 1 :
								 sym_dealposition==2 ? 2 :
								 sym_dealposition==3 ? 3 :
								 sym_dealposition==4 ? 3 :
								 sym_dealposition==5 ? 4 :
								 sym_dealposition==6 ? 4 :
								 sym_dealposition==7 ? 5 :
								 sym_dealposition==8 ? 5 :
								 sym_dealposition==9 ? 6 : 0):
		sym_nplayersdealt==8  ? (sym_dealposition==1 ? 1 :
								 sym_dealposition==2 ? 2 :
								 sym_dealposition==3 ? 3 :
								 sym_dealposition==4 ? 4 :
								 sym_dealposition==5 ? 4 :
								 sym_dealposition==6 ? 5 :
								 sym_dealposition==7 ? 5 :
								 sym_dealposition==8 ? 6 : 0):
		sym_nplayersdealt==7  ? (sym_dealposition==1 ? 1 :
								 sym_dealposition==2 ? 2 :
								 sym_dealposition==3 ? 4 :
								 sym_dealposition==4 ? 4 :
								 sym_dealposition==5 ? 5 :
								 sym_dealposition==6 ? 5 :
								 sym_dealposition==7 ? 6 : 0):
		sym_nplayersdealt==6  ? (sym_dealposition==1 ? 1 :
								 sym_dealposition==2 ? 2 :
								 sym_dealposition==3 ? 4 :
								 sym_dealposition==4 ? 5 :
								 sym_dealposition==5 ? 5 :
								 sym_dealposition==6 ? 6 : 0):
		sym_nplayersdealt==5  ? (sym_dealposition==1 ? 1 :
								 sym_dealposition==2 ? 2 :
								 sym_dealposition==3 ? 5 :
								 sym_dealposition==4 ? 5 :
								 sym_dealposition==5 ? 6 : 0):
		sym_nplayersdealt==4  ? (sym_dealposition==1 ? 1 :
								 sym_dealposition==2 ? 2 :
								 sym_dealposition==3 ? 5 :
								 sym_dealposition==4 ? 6 : 0):
		sym_nplayersdealt==3  ? (sym_dealposition==1 ? 1 :
								 sym_dealposition==2 ? 2 :
								 sym_dealposition==3 ? 6 : 0):
		sym_nplayersdealt==2  ? (
								// reversed blinds - dealer is BB
								// Even worse code. Looks like a code-clone
								(pt_lookup.GetSiteId()==11) ?
								(sym_dealposition==1 ? 1 :
								sym_dealposition==2 ? 6 : 0 ) :
								// Normal blinds - dealposition==1 is BB
								(sym_dealposition==1 ? 2 :
								 sym_dealposition==2 ? 6 : 0)) :0;
}
const int CSymbolEnginePokerAction::PreflopRaisPos() {
	int		e = SUCCESS;
	int		sym_nplayersdealt    = p_engine_container->symbol_engine_active_dealt_playing()->nplayersdealt();
	int		sym_dealpositionrais = p_engine_container->symbol_engine_positions()->dealpositionrais();
	
	//SB=1 BB=2 Early=3 Middle=4 Late=5 Dealer=6
	return
		sym_nplayersdealt==10 ? (sym_dealpositionrais==1 ? 1 :
								sym_dealpositionrais==2 ? 2 :
								sym_dealpositionrais==3 ? 3 :
								sym_dealpositionrais==4 ? 3 :
								sym_dealpositionrais==5 ? 3 :
								sym_dealpositionrais==6 ? 4 :
								sym_dealpositionrais==7 ? 4 :
								sym_dealpositionrais==8 ? 5 :
								sym_dealpositionrais==9 ? 5 :
								sym_dealpositionrais==10 ? 6 : 0):
		sym_nplayersdealt==9  ? (sym_dealpositionrais==1 ? 1 :
								sym_dealpositionrais==2 ? 2 :
								sym_dealpositionrais==3 ? 3 :
								sym_dealpositionrais==4 ? 3 :
								sym_dealpositionrais==5 ? 4 :
								sym_dealpositionrais==6 ? 4 :
								sym_dealpositionrais==7 ? 5 :
								sym_dealpositionrais==8 ? 5 :
								sym_dealpositionrais==9 ? 6 : 0):
		sym_nplayersdealt==8  ? (sym_dealpositionrais==1 ? 1 :
								sym_dealpositionrais==2 ? 2 :
								sym_dealpositionrais==3 ? 3 :
								sym_dealpositionrais==4 ? 4 :
								sym_dealpositionrais==5 ? 4 :
								sym_dealpositionrais==6 ? 5 :
								sym_dealpositionrais==7 ? 5 :
								sym_dealpositionrais==8 ? 6 : 0):
		sym_nplayersdealt==7  ? (sym_dealpositionrais==1 ? 1 :
								sym_dealpositionrais==2 ? 2 :
								sym_dealpositionrais==3 ? 4 :
								sym_dealpositionrais==4 ? 4 :
								sym_dealpositionrais==5 ? 5 :
								sym_dealpositionrais==6 ? 5 :
								sym_dealpositionrais==7 ? 6 : 0):
		sym_nplayersdealt==6  ? (sym_dealpositionrais==1 ? 1 :
								sym_dealpositionrais==2 ? 2 :
								sym_dealpositionrais==3 ? 4 :
								sym_dealpositionrais==4 ? 5 :
								sym_dealpositionrais==5 ? 5 :
								sym_dealpositionrais==6 ? 6 : 0):
		sym_nplayersdealt==5  ? (sym_dealpositionrais==1 ? 1 :
								sym_dealpositionrais==2 ? 2 :
								sym_dealpositionrais==3 ? 5 :
								sym_dealpositionrais==4 ? 5 :
								sym_dealpositionrais==5 ? 6 : 0):
		sym_nplayersdealt==4  ? (sym_dealpositionrais==1 ? 1 :
								sym_dealpositionrais==2 ? 2 :
								sym_dealpositionrais==3 ? 5 :
								sym_dealpositionrais==4 ? 6 : 0):
		sym_nplayersdealt==3  ? (sym_dealpositionrais==1 ? 1 :
								sym_dealpositionrais==2 ? 2 :
								sym_dealpositionrais==3 ? 6 : 0):
		sym_nplayersdealt==2  ? (
								// reversed blinds - dealer is BB
								// !! Bad code. We should get rid of hard-coded constants
								// !! does also not work with new PT3 constants
								(pt_lookup.GetSiteId()==11) ?
								(sym_dealpositionrais==1 ? 1 :
								sym_dealpositionrais==2 ? 6 : 0 ) :
								// Normal blinds - dealposition==1 is BB
								(sym_dealpositionrais==1 ? 2 :
								sym_dealpositionrais==2 ? 6 : 0)) :0;
}
const int CSymbolEnginePokerAction::PostflopPos() {
	int		e = SUCCESS;
	int		sym_nplayersplaying = p_engine_container->symbol_engine_active_dealt_playing()->nplayersplaying();
	int		sym_betposition     = p_engine_container->symbol_engine_positions()->betposition();

	//first=1 early=2 middle=3 late=4 last=5
	return
		sym_nplayersplaying==10 ? (sym_betposition==1 ? 1 :
									sym_betposition==2 ? 2 :
									sym_betposition==3 ? 2 :
									sym_betposition==4 ? 2 :
									sym_betposition==5 ? 3 :
									sym_betposition==6 ? 3 :
									sym_betposition==7 ? 3 :
									sym_betposition==8 ? 4 :
									sym_betposition==9 ? 4 :
									sym_betposition==10 ? 5 : 0):
		sym_nplayersplaying==9  ? (sym_betposition==1 ? 1 :
									sym_betposition==2 ? 2 :
									sym_betposition==3 ? 2 :
									sym_betposition==4 ? 3 :
									sym_betposition==5 ? 3 :
									sym_betposition==6 ? 3 :
									sym_betposition==7 ? 4 :
									sym_betposition==8 ? 4 :
									sym_betposition==9 ? 5 : 0):
		sym_nplayersplaying==8  ? (sym_betposition==1 ? 1 :
									sym_betposition==2 ? 2 :
									sym_betposition==3 ? 2 :
									sym_betposition==4 ? 3 :
									sym_betposition==5 ? 3 :
									sym_betposition==6 ? 4 :
									sym_betposition==7 ? 4 :
									sym_betposition==8 ? 5 : 0):
		sym_nplayersplaying==7  ? (sym_betposition==1 ? 1 :
									sym_betposition==2 ? 2 :
									sym_betposition==3 ? 3 :
									sym_betposition==4 ? 3 :
									sym_betposition==5 ? 4 :
									sym_betposition==6 ? 4 :
									sym_betposition==7 ? 5 : 0):
		sym_nplayersplaying==6  ? (sym_betposition==1 ? 1 :
									sym_betposition==2 ? 2 :
									sym_betposition==3 ? 3 :
									sym_betposition==4 ? 4 :
									sym_betposition==5 ? 4 :
									sym_betposition==6 ? 5 : 0):
		sym_nplayersplaying==5  ? (sym_betposition==1 ? 1 :
									sym_betposition==2 ? 2 :
									sym_betposition==3 ? 3 :
									sym_betposition==4 ? 4 :
									sym_betposition==5 ? 5 : 0):
		sym_nplayersplaying==4  ? (sym_betposition==1 ? 1 :
									sym_betposition==2 ? 3 :
									sym_betposition==3 ? 4 :
									sym_betposition==4 ? 5 : 0):
		sym_nplayersplaying==3  ? (sym_betposition==1 ? 1 :
									sym_betposition==2 ? 4 :
									sym_betposition==3 ? 5 : 0):
		sym_nplayersplaying==2  ? (sym_betposition==1 ? 1 :
									sym_betposition==2 ? 5 : 0): 0;
}

const bool CSymbolEnginePokerAction::FirstIntoPot() {
  if (p_betround_calculator->betround() == kBetroundPreflop) 	{
		return (p_engine_container->symbol_engine_chip_amounts()->potplayer() <= p_engine_container->symbol_engine_tablelimits()->sblind() + p_engine_container->symbol_engine_tablelimits()->bblind()); 
	}	else {
		return (p_engine_container->symbol_engine_chip_amounts()->potplayer() == 0);
	}
}

const int CSymbolEnginePokerAction::BetPosition(const int chairnum) {
	int		betpos = 0;
	int		sym_dealerchair        = p_engine_container->symbol_engine_dealerchair()->dealerchair();
	int		sym_playersplayingbits = p_engine_container->symbol_engine_active_dealt_playing()->playersplayingbits();

	if (chairnum<0 || chairnum>9)
		return betpos;

	if (!(IsBitSet(sym_playersplayingbits, chairnum)))
		return betpos;

	for (int i=sym_dealerchair+1; i<=sym_dealerchair+10; i++)
	{
		if (IsBitSet(sym_playersplayingbits, (i%10)))
			betpos++;

		if (i%10==chairnum)
			break;
	}
	return betpos;
}

const int CSymbolEnginePokerAction::DealPosition(const int chairnum) {
	int		dealposchair = 0 ;
	int		sym_dealerchair      = p_engine_container->symbol_engine_dealerchair()->dealerchair();
	int		sym_playersdealtbits = p_engine_container->symbol_engine_active_dealt_playing()->playersdealtbits();

  if (chairnum < 0 || chairnum>9) {
    return dealposchair;
  }
	for (int i=sym_dealerchair+1; i<=sym_dealerchair+10; i++) {
    if (IsBitSet(sym_playersdealtbits, (i % 10))) {
      dealposchair++;
    }
    if (i % 10 == chairnum) {
      break;
    }
	}
	return (IsBitSet(sym_playersdealtbits, chairnum)) ? dealposchair : 0 ;
}

const bool CSymbolEnginePokerAction::AgchairAfter() {
	if (!p_engine_container->symbol_engine_userchair()->userchair_confirmed())
	{
		return false;
	}
	if (p_engine_container->symbol_engine_raisers()->raischair() >=0 )
	{
		return (BetPosition(p_engine_container->symbol_engine_raisers()->raischair()) > p_engine_container->symbol_engine_positions()->betposition());
	}
	else
	{
		return false;
	}
}

bool CSymbolEnginePokerAction::EvaluateSymbol(const CString name, double *result, bool log/* = false*/) {
  if (memcmp(name,"ac_", 3) != 0) {
    // Symbol of a different symbol-engine
    return false;
  }
	if (memcmp(name,"ac_agchair_after", 16) == 0)	{
    *result = AgchairAfter();
    return true;
  }
	if (memcmp(name,"ac_preflop_pos", 14) == 0)	{
    *result = PreflopPos();
    return true;
  }
	if (memcmp(name,"ac_prefloprais_pos", 18) == 0)	{
    *result = PreflopRaisPos();
    return true;
  }
	if (memcmp(name,"ac_postflop_pos", 15) == 0) {
    *result = PostflopPos();
    return true;
  }
	if (memcmp(name,"ac_first_into_pot", 17) == 0) {
    *result = FirstIntoPot();
    return true;
  }
	if (memcmp(name,"ac_betpos", 9) == 0)	{
    *result = BetPosition(RightDigitCharacterToNumber(name));
    return true;
  }
	if (memcmp(name,"ac_dealpos", 10) == 0) {
    *result = DealPosition(RightDigitCharacterToNumber(name));
    return true;
  }
  // Invalid symbol
  return false;
}


CString CSymbolEnginePokerAction::SymbolsProvided() {
  CString list_of_symbols = "ac_agchair_after "
    "ac_prefloprais_pos ac_postflop_pos ac_first_into_pot ";
  list_of_symbols += RangeOfSymbols("ac_betpos%i", kFirstChair, kLastChair);
  list_of_symbols += RangeOfSymbols("ac_dealpos%i", kFirstChair, kLastChair);
  return list_of_symbols;
}

