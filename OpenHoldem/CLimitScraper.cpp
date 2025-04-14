//******************************************************************************
//
// This file is part of the OpenHoldem project
//    Source code:           https://github.com/OpenHoldem/openholdembot/
//    Forums:                http://www.maxinmontreal.com/forums/index.php
//    Licensed under GPL v3: http://www.gnu.org/licenses/gpl.html
//
//******************************************************************************
//
// Purpose: Interpreting the title-string 
//   and scraping limits, handnumber, etc. from the table
//
//******************************************************************************

#include "pch.h"
#include "CLimitScraper.h"

CLimitScraper *p_limit_scraper = NULL;

CLimitScraper::CLimitScraper() {
}

CLimitScraper::~CLimitScraper() {
}