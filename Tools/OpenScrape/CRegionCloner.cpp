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
#include "CRegionCloner.h"

#include <assert.h>
#include <math.h>
#include "CTablemap/CTablemap.h"
#include "CTablemap/CTablemapAccess.h"
#include "MagicNumbers/MagicNumbers.h"
#include "OpenScrape.h"

const int k_number_of_circular_cloneable_regions = 39;

// 1st string: name of 0-region
// 2nd string: prefix before number
// 3rd string: postfix after number
const CString circular_cloneable_regions[k_number_of_circular_cloneable_regions][3] =
{{"p0active",          "p", "active"},  
// we should already have pXbalance regions, so don't clone them
// {"p0balance",         "p", "balance"},
 {"p0bet",             "p", "bet"},
 {"p0cardback",        "p", "cardback"},
 {"p0cardface0",       "p", "cardface0"},
 {"p0cardface1",       "p", "cardface1"},
 {"p0cardface2",       "p", "cardface2" },
 {"p0cardface3",       "p", "cardface3" },
 {"p0cardface0nocard", "p", "cardface0nocard"},
 {"p0cardface1nocard", "p", "cardface1nocard"},
 {"p0cardface2nocard", "p", "cardface2nocard" },
 {"p0cardface3nocard", "p", "cardface3nocard" },
 {"p0cardface0rank",   "p", "cardface0rank"},
 {"p0cardface0suit",   "p", "cardface0suit"},
 {"p0cardface1rank",   "p", "cardface1rank"},
 {"p0cardface1suit",   "p", "cardface1suit"},
 {"p0cardface2rank",   "p", "cardface2rank" },
 {"p0cardface2suit",   "p", "cardface2suit" },
 {"p0cardface3rank",   "p", "cardface3rank" },
 {"p0cardface3suit",   "p", "cardface3suit" },
 {"p0dealer",          "p", "dealer",},
 {"p0name",            "p", "name"},
 {"p0colourcode",      "p", "colourcode"},
 {"p0seated",          "p", "seated"},
 // Alternative for blinking fonts, etc.
 {"u0active",          "u", "active"},  
 {"u0balance",         "u", "balance"},
 {"u0bet",             "u", "bet"},
 {"u0cardback",        "u", "cardback"},
 {"u0cardface0",       "u", "cardface0"},
 {"u0cardface1",       "u", "cardface1"},
 {"u0cardface0rank",   "u", "cardface0rank"},
 {"u0cardface0suit",   "u", "cardface0suit"},
 {"u0cardface1rank",   "u", "cardface1rank"},
 {"u0cardface1suit",   "u", "cardface1suit"},
 {"u0dealer",          "u", "dealer"},
 {"u0name",            "u", "name"},
 {"u0seated",          "u", "seated"},
 // Scraping chip-stacks
 // We handle only chip-positions 00, 01 and 10,
 // because these are mostly used.
 {"p0chip00",        "p", "chip00"},
 {"p0chip01",        "p", "chip01"},
 {"p0chip10",        "p", "chip10"}
};

CRegionCloner::CRegionCloner()
{
	if (p_tablemap_access != NULL)
	{
		p_tablemap_access = new CTablemapAccess;
	}
}

CRegionCloner::~CRegionCloner()
{
	delete(p_tablemap_access); 
}

bool CRegionCloner::TableSizeUndefined()
{
	return false; /* !!! To do ((p_tablemap_access->GetClientSizeX() <= 0)
		|| (p_tablemap_access->GetClientSizeY() <= 0)); */
}

CString CRegionCloner::CreateName(CString prefix, int number, CString postfix)
{
	const int k_max_length_of_name = 100;
	char new_symbol_name[k_max_length_of_name];
	assert(number >= 0);
	assert(number <= 9);
	assert(prefix.GetLength() + postfix.GetLength() < k_max_length_of_name); 
	sprintf_s(new_symbol_name, k_max_length_of_name, "%s%d%s", prefix.GetString(), number, postfix.GetString());
	return CString(new_symbol_name);
}

void CRegionCloner::CalculateLinearRegions(STablemapRegion first_region, int number_of_regions) {
	// Calculates the position for N regions,
	// horizontally aligned and symmetrically to the middle of the table,
	// with equal distance to each other.
	assert(number_of_regions >= 1);
	assert(number_of_regions <= k_max_number_of_regions_to_clone); 

	int width_of_region = first_region.right - first_region.left;
  int target_size_X, dummy;
  p_tablemap_access->GetClientSize("targetsize", &target_size_X, &dummy);
	int space_for_remaining_regions = target_size_X
		- first_region.left // distance to the left of first region
		- first_region.left // distance to the right of last region
		- width_of_region;
	int delta_X_to_next_region = space_for_remaining_regions
		/ (number_of_regions - 1);
	for (int i=0; i<number_of_regions; i++)	{
		// Align horizontally, don't change the first region
		linear_region_positions[i].left = first_region.left
			+ i * delta_X_to_next_region;
		linear_region_positions[i].right = first_region.right
			+ i * delta_X_to_next_region;
		// Keep all at the same Y-position
		linear_region_positions[i].top = first_region.top;
		linear_region_positions[i].bottom = first_region.bottom;
	}
}

void CRegionCloner::CalculateCircularRegions(STablemapRegion first_region, int number_of_regions)
{
	// Calculates the position for N regions,
	// about circular aligned and around the center of the table.
	assert(number_of_regions >= 1);
	assert(number_of_regions <= k_max_number_of_regions_to_clone); 

  int target_size_X;
  int target_size_Y;
  p_tablemap_access->GetClientSize("targetsize", &target_size_X, &target_size_Y);

	int center_x_of_region = 0.5 * (first_region.left + first_region.right);
	int center_y_of_region = 0.5 * (first_region.top + first_region.bottom);

	int center_x_of_table  = 0.5 * target_size_X;
	int center_y_of_table  = 0.5 * target_size_Y;

	int delta_x_between_centers = center_x_of_table - center_x_of_region;
	int delta_y_between_centers = center_y_of_table - center_y_of_region;

	int distance_to_center = sqrt(float(
		  delta_x_between_centers * delta_x_between_centers
		+ delta_y_between_centers * delta_y_between_centers));

	float angle_between_regions = (2 * M_PI) / number_of_regions;

	// starting angle for region 0.
	// 0 means: 12 o'clock position
	int angle_for_region_0 = 0;
	if (delta_x_between_centers == 0)
	{
		if (center_y_of_region > center_y_of_table)
		{
			angle_for_region_0 = 0;
		}
		else
		{
			angle_for_region_0 = M_PI;
		}
	}
	else
	{
		angle_for_region_0 = sin(float(delta_y_between_centers / delta_x_between_centers));
	}
	// Don't do any calculations for region 0.
	// We keep it anyway and don't want to ruin it even by one pixel.
	for (int i=0; i<number_of_regions; i++)
	{
		float angle_for_region_i = angle_for_region_0 + (i * angle_between_regions);
		if (angle_for_region_i > 2 * M_PI)
		{
			// normalize angle, in case we didn't start right of 12 o'clock.
			angle_for_region_i -= 2 * M_PI;
		}
		assert(angle_for_region_i >= 0);
		assert(angle_for_region_i <= 2 * M_PI);

		// distance from center of table to center of region
		// distance may be negative, so we can simply add it alter;
		int delta_x_to_region_i = sin(angle_for_region_i) * distance_to_center;
		int delta_y_to_region_i = cos(angle_for_region_i) * distance_to_center;		
		
		// absolue position of center of new region
		int center_x_of_region_1 = center_x_of_table - delta_x_to_region_i;
		int center_y_of_region_1 = center_y_of_table - delta_y_to_region_i;

		int width_of_region = first_region.right - first_region.left;
		int height_of_region = first_region.top - first_region.bottom;

		// finally calculating the corners of the new region
		circular_region_positions[i].left   = center_x_of_region_1 - (1/2 * width_of_region);
		circular_region_positions[i].bottom = center_y_of_region_1 - (1/2 * height_of_region);
		circular_region_positions[i].right  = circular_region_positions[i].left   + width_of_region;
		circular_region_positions[i].top    = circular_region_positions[i].bottom + height_of_region;
	}	
}

void CRegionCloner::CalculateCircularRegionsForFirstCloneableObject() {
	STablemapRegion region_to_be_cloned;
	// Find first cloneable region for position calculations
	for (int i = 0; i<k_number_of_circular_cloneable_regions; i++) {
		if (p_tablemap_access->GetTableMapRegion(
			  circular_cloneable_regions[i][0], // name of region
			  &region_to_be_cloned)) {
			CalculateCircularRegions(region_to_be_cloned, kMaxNumberOfPlayers);
			break;
		}
	}
}

void CRegionCloner::CalculateDistanceToFirstCloneableRegion(STablemapRegion region_to_be_cloned)
{
	// Calculates distance from current 0-region 
	// to very first cloneable region, so that we get an offset
	// amd can later allign the pther regions properly
	distance_to_first_cloneable_region.bottom =
		region_to_be_cloned.bottom - circular_region_positions[0].bottom;
	distance_to_first_cloneable_region.left =
		region_to_be_cloned.left - circular_region_positions[0].left;
	distance_to_first_cloneable_region.right =
		region_to_be_cloned.right - circular_region_positions[0].right;
	distance_to_first_cloneable_region.top =
		region_to_be_cloned.top - circular_region_positions[0].top;
}

void CRegionCloner::ApplyNextLinearRegionPosition(STablemapRegion *new_region, int index)
{
	assert(index >= 0);
	assert(index < k_max_number_of_regions_to_clone);

	new_region->bottom = linear_region_positions[index].bottom;
	new_region->top    = linear_region_positions[index].top;

	new_region->left   = linear_region_positions[index].left;
	new_region->right  = linear_region_positions[index].right;
}

void CRegionCloner::ApplyNextCircularRegionPosition(
	STablemapRegion region_to_be_cloned, STablemapRegion *new_region, int index)
{
	// We didn't calculate position 0,
	// because we want to keep it exactly as is.
	assert(index > 0);
	assert(index < k_max_number_of_regions_to_clone);

	new_region->bottom = circular_region_positions[index].bottom
		+ distance_to_first_cloneable_region.bottom;
	new_region->left = circular_region_positions[index].left
		+ distance_to_first_cloneable_region.left;
	new_region->top = circular_region_positions[index].top
		+ distance_to_first_cloneable_region.top;
	new_region->right = circular_region_positions[index].right
		+ distance_to_first_cloneable_region.right;
}

void CRegionCloner::CloneCommonCards()
{
	STablemapRegion region_to_be_cloned;
	if (!p_tablemap_access->GetTableMapRegion("region_to_be_cloned", &region_to_be_cloned))
	{
		return;
	}
	CalculateLinearRegions(region_to_be_cloned, kNumberOfCommunityCards);
	for (int i=0; i<kNumberOfCommunityCards; i++)
	{	
		STablemapRegion new_region;
		ApplyNextLinearRegionPosition(&new_region, i);

		new_region.name = CreateName("c0cardface", i, "");
		new_region.color = region_to_be_cloned.color;
		new_region.radius = region_to_be_cloned.radius;
		new_region.transform = region_to_be_cloned.transform;

		p_tablemap->r$_insert(new_region);

		theApp.m_TableMapDlg->UpdateDisplayOfAllRegions();
	}
}

void CRegionCloner::CloneCircularCloneableRegions() {
	//CalculateCircularRegionsForFirstCloneableObject();
  STablemapRegion region_to_be_cloned;
	STablemapRegion region_with_balance;
	int balance_0_top, balance_0_left, balance_0_bottom, balance_0_right;

	if (p_tablemap_access->GetTableMapRegion(
			"p0balance", // get p0balance and its coordinates to calculate offsets
			&region_to_be_cloned)) {
		balance_0_top = region_to_be_cloned.top;
		balance_0_left = region_to_be_cloned.left;
		balance_0_bottom = region_to_be_cloned.bottom;
		balance_0_right = region_to_be_cloned.right;
	} else {
		MessageBox(NULL,"You must have p0balance object at first!", "Warning",0);
		return;
	}
	for (int i=0; i<k_number_of_circular_cloneable_regions; ++i) {
		// Clone a single region;
		if (!p_tablemap_access->GetTableMapRegion(
			  circular_cloneable_regions[i][0], // name of region
			  &region_to_be_cloned)) {
			continue;
		}
		//CalculateDistanceToFirstCloneableRegion(region_to_be_cloned);

		// Start with player 1 and keep player 0 as is
		for (int p=1; p<kMaxNumberOfPlayers; p++)	{	
			STablemapRegion new_region, existing_region;
		  //	ApplyNextCircularRegionPosition(region_to_be_cloned, &new_region, p);

			new_region.name = CreateName( 
				circular_cloneable_regions[i][1],   // prefix
				p,									// player number
				circular_cloneable_regions[i][2]);  // postfix
			new_region.color = region_to_be_cloned.color;
			new_region.radius = region_to_be_cloned.radius;
			new_region.transform = region_to_be_cloned.transform;

			CString s = "";
			s.Format("p%dbalance", p);

			if (p_tablemap_access->GetTableMapRegion(
			      s, // if this region has balance then continue and clone
			      &region_with_balance) 
          && !p_tablemap_access->GetTableMapRegion(new_region.name, &existing_region)) {
				new_region.top = region_with_balance.top - ( balance_0_top - region_to_be_cloned.top );
				new_region.left = region_with_balance.left - ( balance_0_left - region_to_be_cloned.left );
				new_region.bottom = new_region.top + (region_to_be_cloned.bottom-region_to_be_cloned.top );
				new_region.right = new_region.left + ( region_to_be_cloned.right - region_to_be_cloned.left );
				p_tablemap->r$_insert(new_region);
			}
		}
	}
	theApp.m_TableMapDlg->UpdateDisplayOfAllRegions(); 
}

void CRegionCloner::CloneRegions() {
  CloneCommonCards();
	CloneCircularCloneableRegions();
}