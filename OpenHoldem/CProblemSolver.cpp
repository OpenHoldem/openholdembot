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

#include <stdafx.h>
#include "CProblemSolver.h"

#include "CAutoConnector.h"
#include "CCasinoInterface.h"
#include "CConfigurationCheck.h"
#include "CEngineContainer.h"

#include "CSymbolEngineActiveDealtPlaying.h"
#include "CSymbolEnginePokerval.h"
#include "CSymbolEngineTime.h"
#include "CSymbolEngineUserchair.h"
#include "CTableMapLoader.h"
#include "CTableState.h"
#include "WindowFunctions/window_functions.h"

CProblemSolver::CProblemSolver()
{}

CProblemSolver::~CProblemSolver()
{}

const char *k_title_string = "Problem Solver";

bool CProblemSolver::NoTableMapsInScraperFolder()
{
	return (p_tablemap_loader->NumberOfTableMapsLoaded() == 0);
}

bool CProblemSolver::NotConnected()
{
  write_log(Preferences()->debug_alltherest(), "[CProblemSolver] location Johnny_9\n");
	return (p_autoconnector->IsConnectedToAnything() == false);
}

bool CProblemSolver::UserChairUnknown()
{
	return (p_engine_container->symbol_engine_userchair()->userchair_confirmed() == false);
}

bool CProblemSolver::NoOpponents()
{
	return (p_engine_container->symbol_engine_active_dealt_playing()->nopponentsplaying() == 0);
}

bool CProblemSolver::AutoPlayerDidActAtLeastOnce()
{
	// We compare the time since connection to the time since last action.
	return (p_engine_container->symbol_engine_time()->elapsed() != p_engine_container->symbol_engine_time()->elapsedauto());
}

bool CProblemSolver::NoCardsVisible()
{
	return (!p_table_state->User()->HasKnownCards());
}

bool CProblemSolver::NotEnoughButtonsVisible() {
	// We need at least 2 visible buttons to play
	return (p_casino_interface->NumberOfVisibleAutoplayerButtons() < 2);
}


void CProblemSolver::TryToDetectBeginnersProblems()
{
  p_configurationcheck->ForceAllConfigurationChercks();
	if (NoTableMapsInScraperFolder())
	{
		MessageBox_Interactive(
			"BAD NEWS:\n"
			"    - No tablemaps in scraper folder.\n"
			"\n"
			"You need a tablemap for every casino and game-type.",
			k_title_string, 0);

	}
	else if (NotConnected())
	{
		MessageBox_Interactive(
			"GOOD NEWS:\n"
			"    - At least one tablemap in scraper folder.\n"
			"\n"
			"BAD NEWS:\n"
			"    - You are not connected to a table.\n"
			"\n"
			" To connect to a table three conditions must be met:\n"
			"    - You need a tablemap for this casino and game-type.\n"
			"    - The table must match z$clientsizemin and z$clientsizemax.\n"
      "         OpenScrape -> Menu -> View\n"
			"    - The s$titletext must match.\n"
			"If OpenHoldem does not connect, then you have to fix your tablemap.",
			k_title_string, 0);
	}
	else if (NoOpponents())
	{
		MessageBox_Interactive(
			"GOOD NEWS:\n"
			"    - At least one tablemap in scraper folder.\n."
			"    - You are connected to a table.\n"
			"\n"
			"BAD NEWS:\n"
			"    - There seem to be no opponents.\n"
			"\n"
			"Please revisit your tablemap, especially:\n"
			"    - Seated regions\n"
			"    - Active regions\n"
			"    - Cardbacks\n",
			k_title_string, 0);
	}
	else if (UserChairUnknown())
	{
		MessageBox_Interactive(
			"GOOD NEWS:\n"
			"    - At least one tablemap in scraper folder.\n"
			"    - You are connected to a table.\n"
			"    - At least one opponent got recognized.\n"
			"\n"
			"BAD NEWS:\n"
			"    - You are not seated or userchair not (yet) recognized.\n"
			"\n"
			"To detect the userchair three conditions must be met:\n"
			"    - Player must be active (not sitting out).\n"
			"    - Cards visible.\n"
			"    -  Buttons visible at the same time.\n"
			"Please revisit your tablemap.",
			k_title_string, 0);
	}
	else if (AutoPlayerDidActAtLeastOnce())
	{
		// This is the GOOD case!
		MessageBox_Interactive(
			"REALLY GOOD NEWS:\n"
			"    - At least one tablemap in scraper folder.\n"
			"    - You are connected to a table.\n"
			"    - At least one opponent got recognized.\n"
			"    - Your chair got recognized.\n"
			"    - The autoplayer did act at least once.\n"
			"\n"
			"This means that your map and your settings\n"
			"are at least not completely wrong.\n" 
			"The rest is fine-tuning,\n"
			"we won't deal with that here.\n"
			"\n"
			"Good luck, dear friend.\n",
			k_title_string, 0);
	}
	// Cards and buttons get handled after the good case,
	// because cards and buttons are not always visible,
	// even if everything is ok.
	else if (NoCardsVisible())
	{
		MessageBox_Interactive(
			"GOOD NEWS:\n"
			"    - At least one tablemap in scraper folder\n."
			"    - You are connected to a table.\n"
			"    - At least one opponent got recognized.\n"
			"    - Your chair got recognized.\n"
			"\n"
			"BAD NEWS:\n"
			"    - You don't have any cards\n"
			"\n"       
			"Please revisit your tablemap\n"
			"to make sure that cards get scraped correctly.",
			k_title_string, 0);
	}
	else if (NotEnoughButtonsVisible())
	{
		MessageBox_Interactive(
			"GOOD NEWS:\n"
			"    - At least one tablemap in scraper folder\n."
			"    - You are connected to a table.\n"
			"    - At least one opponent got recognized.\n"
			"    - Your chair got recognized.\n"
			"    - Your cards got recognized.\n"
			"\n"
			"BAD NEWS:\n"
			"    - Not enough buttons detected.\n"
			"\n"       
			"Please revisit your tablemap\n"
			"to make sure that all buttons get scraped correctly.",     
			k_title_string, 0);
	}
	else
	{
		// This should not happen.
		// No error detected, but autoplayer did not yet act.
		MessageBox_Interactive(
			"GOOD NEWS:\n"
			"    - At least one tablemap in scraper folder\n."
			"    - You are connected to a table\n"
			"    - At least one opponent got recognized.\n"
			"    - Your chair got recognized\n"
			"    - Your cards got recognized\n"
			"    - Buttons got detected\n"
			"\n"
			"BAD NEWS:\n"
			"    - The autoplayer did not (yet) act,\n"        
			"\n"
			"The autoplayer should act within some seconds,\n"
			"as long as the autoplayer is engaged (default).\n",
			k_title_string, 0);
	}
}