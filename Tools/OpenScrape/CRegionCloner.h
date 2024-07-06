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


#ifndef INC_C_REGIONCLONER_H
#define INC_C_REGIONCLONER_H

#include "CTablemap/CTablemap.h"
#include "MagicNumbers/MagicNumbers.h"

class CRegionCloner
{
public:
	CRegionCloner();
	~CRegionCloner();
	void CloneRegions();
private:
	bool TableSizeUndefined();
	void CloneCommonCards();
	void CloneCircularCloneableRegions();
	CString CreateName(CString prefix, int number, CString postfix);
	void CalculateLinearRegions(STablemapRegion first_region, int number_of_regions);
	void CalculateCircularRegions(STablemapRegion first_region, int unmber_of_regions);
	void CalculateCircularRegionsForFirstCloneableObject();
	void CalculateDistanceToFirstCloneableRegion(STablemapRegion region_to_be_cloned);
	void ApplyNextLinearRegionPosition(STablemapRegion *new_region, int index);
	void ApplyNextCircularRegionPosition(STablemapRegion region_to_be_cloned, STablemapRegion *new_region, int index);
private:
	RECT linear_region_positions[k_max_number_of_regions_to_clone];
	RECT circular_region_positions[k_max_number_of_regions_to_clone];
	RECT distance_to_first_cloneable_region;
};

#endif // INC_C_REGIONCLONER_H