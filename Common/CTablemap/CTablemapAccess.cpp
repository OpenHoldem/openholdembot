//*******************************************************************************
//
// This file is part of the OpenHoldem project
//    Source code:           https://github.com/OpenHoldem/openholdembot/
//    Forums:                http://www.maxinmontreal.com/forums/index.php
//    Licensed under GPL v3: http://www.gnu.org/licenses/gpl.html
//
//*******************************************************************************
//
// Purpose:
//
//*******************************************************************************


#include "stdafx.h"
#include "CTableMapAccess.h"

#include "CTablemap/CTablemap.h"


CTablemapAccess *p_tablemap_access = NULL;


CTablemapAccess::CTablemapAccess()
{
}

CTablemapAccess::~CTablemapAccess()
{
}

bool CTablemapAccess::GetButtonRect(CString button_name, RECT *_rect)
{
	/*
		r$ : Inserts button details into RECT parameter
	*/

	return GetTableMapRect(button_name, _rect);
}

bool CTablemapAccess::GetTableMapRect(CString rect_name, RECT *_rect)
{
	/*
		r$ : Finds tablemap regions (i3_edit, i3_slider, i3_handle, iXbutton, i86Xbutton)
		and inserts details into RECT parameter
	*/

	RMapCI wanted_region = p_tablemap->r$()->find(rect_name);

	if (wanted_region != p_tablemap->r$()->end())
	{
		_rect->left   = wanted_region->second.left;
		_rect->top    = wanted_region->second.top;
		_rect->right  = wanted_region->second.right;
		_rect->bottom = wanted_region->second.bottom;

		return true;
	}

	return false;
}

bool CTablemapAccess::GetTableMapRegion(CString region_name, STablemapRegion *_region)
{
	/*
		r$ : Finds tablemap regions (i3_edit, i3_slider, i3_handle, iXbutton, i86Xbutton)
		and inserts details into region parameter
	*/

	RMapCI wanted_region = p_tablemap->r$()->find(region_name);

	if (wanted_region != p_tablemap->r$()->end())
	{
		_region->left      = wanted_region->second.left;
		_region->top       = wanted_region->second.top;
		_region->right     = wanted_region->second.right;
		_region->bottom    = wanted_region->second.bottom;
		_region->color     = wanted_region->second.color;
		_region->radius    = wanted_region->second.radius;
		_region->transform = wanted_region->second.transform;
		_region->name      = wanted_region->second.name;
		_region->cur_bmp   = wanted_region->second.cur_bmp;
		_region->last_bmp  = wanted_region->second.last_bmp;

		return true;
	}

	return false;
}

bool CTablemapAccess::SetTitleText(CString title_name, CString &destination)
{
	/*
		s$ : Extract client titletext
		required by Autoconnector
	*/

	destination = "";
	SMapCI s_iter = p_tablemap->s$()->find(title_name);

	if (s_iter != p_tablemap->s$()->end())
	{
		destination = s_iter->second.text;

		return true;
	}

	return false;
}

bool CTablemapAccess::GetClientSize(CString size_name, int *width, int *height) {
	/*
		z$ : Extract client size
		required by Autoconnector
	*/
  ZMapCI z_iter = p_tablemap->z$()->find(size_name);
  if (z_iter != p_tablemap->z$()->end()) {
		*width = z_iter->second.width;
		*height = z_iter->second.height;
    return true;
	}
  *width = 0; 
  *height = 0;
	return false;
}

unsigned int CTablemapAccess::GetClientSize(CString size_name, dim dimension)
{
	/*
		z$ : Extract client size
	*/

	ZMapCI z_iter = p_tablemap->z$()->find(size_name);

	if (z_iter != p_tablemap->z$()->end())
	{
		if (dimension == width)
			return z_iter->second.width;
		if (dimension == height)
			return z_iter->second.height;
	}

	// 0 is the default formerly used by the auto-connector
	return 0;
}

