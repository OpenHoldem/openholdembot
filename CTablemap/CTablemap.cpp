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

#include "StdAfx.h"
#include "CTablemap.h"

#include <assert.h>
#include <Math.h>
#include "../CTransform/CTransform.h"
#include "..\DLLs\StringFunctions_DLL\string_functions.h"
#include "..\DLLs\WindowFunctions_DLL\window_functions.h"

#ifdef OPENHOLDEM_PROGRAM
#include "../DLLs/Debug_DLL/debug.h"
#include "../DLLs/Preferences_DLL/Preferences.h"
#endif

#ifdef OPENSCRAPE_PROGRAM
#include "../OpenScrape/debug.h"
#include "..\DLLs\StringFunctions_DLL\string_functions.h"
#endif

CTablemap			*p_tablemap = NULL;

CTablemap::CTablemap(void) {
  ClearTablemap();
}

CTablemap::~CTablemap(void) {
	ClearTablemap();
}

bool CTablemap::SupportsOmaha() {
  // Checkling for more cards than necessary for HoldEm
  // Checking player0 which exists in every tablemap.
  if (ItemExists("p0cardface2")) {
    // Additional card-faces supported
    return true;
  }
  if (ItemExists("p0cardface2rank")) {
    // Additional card-ranks (and suits) supported
    return true;
  }
  return false;
}

int CTablemap::GetTMSymbol(CString name, int default)
{
	// Tablemap Strings
	// Strings with the same textual value have to evaluate 
	// to the same numerical value, as they get later processed
	// by a single function GetTMSymbol(name, default)
	// Examples: TEXTDEL_NOTHING = TEXTSEL_NOTHING = BETCONF_NOTHING = 5
	assert(TEXTSEL_SINGLECLICK == BUTTON_SINGLECLICK);
	assert(TEXTSEL_DOUBLECLICK == BUTTON_DOUBLECLICK);
	assert(TEXTSEL_NOTHING == TEXTDEL_NOTHING);
	assert(TEXTSEL_NOTHING == BETCONF_NOTHING);

	SMapCI it = _s$.find(name); 
	if (it==_s$.end())
	{
		// not found
		return default;
	}
  //!!!!!! Here sometimes problem, it seems to point into nirvana
  // maybe fixed in 11.1.0
#ifdef OPENHOLDEM_PROGRAM
  write_log(Preferences()->debug_alltherest(), "[CTablemap] location Johnny_S\n");
#endif
	CString value = it->second.text.GetString();
	CString s = value.MakeLower();
#ifdef OPENHOLDEM_PROGRAM
  write_log(Preferences()->debug_alltherest(), "[CTablemap] location Johnny_T\n");
#endif
	// "sgl click" and "single", "dbl click" and "double"
	// are inconsistent naming, but can now be used interchangeably
	// as the named constants have the same value
	if (s == "sgl click") return TEXTSEL_SINGLECLICK;
	else if (s == "dbl click") return TEXTSEL_DOUBLECLICK;
	else if (s == "triple click") return TEXTSEL_TRIPLECLICK;
	else if (s == "click drag") return TEXTSEL_CLICKDRAG;
	else if (s == "nothing") return TEXTSEL_NOTHING;
	else if (s == "delete") return TEXTDEL_DELETE;
	else if (s == "backspace") return TEXTDEL_BACKSPACE; 
	else if (s == "nothing") return TEXTDEL_NOTHING;
	else if (s == "enter") return BETCONF_ENTER;
	else if (s == "click bet") return BETCONF_CLICKBET; 
	else if (s == "single") return BUTTON_SINGLECLICK; 
	else if (s == "double") return BUTTON_DOUBLECLICK; 
	else if (s == "raise") return BETPOT_RAISE; 
	else if (s == "true") return true;
	else if (s == "yes") return true; 
	else if (s != "")
	{
		// Assume it is a number
		int n = strtoul(value, NULL, 10);
		return n;
	}
	else return default;
}

CString CTablemap::GetTMSymbol(CString name)
{
	SMapCI it = _s$.find(name); 
	if (it==_s$.end()) return "";
	return it->second.text.GetString();
}

void CTablemap::GetTMRegion(const CString name, RECT *region) {
  RMapCI it = _r$.find(name);
  if (it == _r$.end()) {
#ifdef OPENHOLDEM_PROGRAM
   write_log(Preferences()->debug_tablemap_loader(), "[CTablemap] Could not find region %s\n", name);
#endif
    region->bottom = kUndefined;
    region->left = kUndefined;
    region->right = kUndefined;
    region->top = kUndefined;
  } else {
    region->bottom = it->second.bottom;
    region->left = it->second.left;
    region->right = it->second.right;
    region->top = it->second.top;
  }
}

#define ENT CSLock lock(m_critsec);

const bool CTablemap::i$_insert(const STablemapImage s) 
{ 
#ifdef OPENHOLDEM_PROGRAM
	write_log(Preferences()->debug_tablemap_loader(), "[CTablemap] i$_insert\n");
#endif
	ENT 
	uint32_t index = CreateI$Index(s.name,s.width,s.height,s.pixel);
#ifdef OPENHOLDEM_PROGRAM
	write_log(Preferences()->debug_tablemap_loader(), "[CTablemap] Index %i\n", index);
#endif
	std::pair<IMapI, bool> r=_i$.insert(IPair(index, s)); 
#ifdef OPENHOLDEM_PROGRAM
	write_log(Preferences()->debug_tablemap_loader(), "[CTablemap] Image inserted\n");
	write_log(Preferences()->debug_tablemap_loader(), "[CTablemap] Success: %i\n", r.second);
#endif
	return r.second; 
}

void CTablemap::ClearIMap()
{
#ifdef OPENHOLDEM_PROGRAM
	write_log(Preferences()->debug_tablemap_loader(), "[CTablemap] ClearIMap\n");
#endif

	IMap::iterator iter;
	// For each image in the map: delete.
	for(iter = _i$.begin(); iter != _i$.end(); iter++ ) 
	{
		// First value in the map is some index, internally created in i$_insert()
		// Second value in the map is our image.
		delete iter->second.image;
	}
	// Remove the contents of the map.
	_i$.clear();
#ifdef OPENHOLDEM_PROGRAM
	write_log(Preferences()->debug_tablemap_loader(), "[CTablemap] ClearIMap finished\n");
#endif
}

void CTablemap::ClearTablemap() {
#ifdef OPENHOLDEM_PROGRAM
	write_log(Preferences()->debug_tablemap_loader(), "[CTablemap] ClearTablemap\n");
#endif

	CSLock lock(m_critsec);
	_valid = false;
	_filepath = "";
	_filename = "";
  _nchairs = kUndefinedZero;
	_z$.clear();
	_s$.clear();
	_r$.clear();
	_tpl$.clear();
	for (int i = 0; i < k_max_number_of_font_groups_in_tablemap; i++) {
		_t$[i].clear();
  }
	for (int i = 0; i < k_max_number_of_hash_groups_in_tablemap; i++) {
		_p$[i].clear();
  }
  for (int i = 0; i < k_max_number_of_hash_groups_in_tablemap; i++) {
		_h$[i].clear();
  }
  // Clear _i$-map and its contents
	ClearIMap();
}

void CTablemap::WarnAboutGeneralTableMapError(int line, int error_code)
{
	CString error;
	error.Format("Error code: %d  line: %d\n", error_code, line);
  switch (error_code) {
  case ERR_EOF:
    error += "Unexpected end of file";
    break;
  case ERR_SYNTAX:					  
    error += "Syntax error";
    break;
  case ERR_VERSION:					  
    error += "Unsupported file version";
    break;
  case ERR_NOTMASTER:				  
    error += "Not a master file";
    break;
  case ERR_HASH_COLL:				  
    error += "Hash collision";
    break;
  case ERR_REGION_SIZE:				
    error += "Invalid region size";
    break;
  case ERR_UNK_LN_TYPE:				
    error += "Unknown line type";
    break;
  case ERR_INV_HASH_TYPE:			
    error += "Invalid hash type";
    break;
  case ERR_INCOMPLETEMASTER:  
    error += "Incomplete master file";
    break;
  }
	MessageBox_Error_Warning(error, "Table map load error");
}

bool CTablemap::ItemExists(CString name) {
  // We check only strings, sizes and regions
  ZMapCI zit = _z$.find(name); 
  if (zit !=_z$.end()) return true;
  SMapCI sit = _s$.find(name); 
  if (sit !=_s$.end()) return true;
  //TMapCI tit = _t$.find(name); 
  //if (tit !=_t$.end()) return true;
  //PMapCI pit = _p$.find(name); 
  //if (pit !=_p$.end()) return true;
  //HMapCI hit = _h$.find(name); 
  //if (hit !=_h$.end()) return true;
  //IMapCI iit = _i$.find(name); 
  //if (iit !=_i$.end()) return true;
  RMapCI rit = _r$.find(name); 
  if (rit !=_r$.end()) return true;
  TPLMapCI tplit = _tpl$.find(name);
  if (tplit != _tpl$.end()) return true;
  return false;
}

bool CTablemap::FontGroupInUse(int font_index) {
  assert(font_index >= 0);
  assert(font_index < k_max_number_of_font_groups_in_tablemap);
  TMap font_group = _t$[font_index];
  return (!font_group.empty());
}

int CTablemap::LoadTablemap(const CString _fname) {
#ifdef OPENHOLDEM_PROGRAM
	write_log(Preferences()->debug_tablemap_loader(), "[CTablemap] Loadtablemap: %s\n", _fname);
#endif
	CString		strLine = "", strLineType = "", token = "", s = "", e = "", hexval = "", t = "";
	CString		MaxFontGroup = "", MaxHashGroup = "";
	int			  pos = 0, x = 0, y = 0;
	MaxFontGroup.Format("%d", k_max_number_of_font_groups_in_tablemap);
	MaxHashGroup.Format("%d", k_max_number_of_hash_groups_in_tablemap);
	// temp
	STablemapSize      hold_size;
	STablemapSymbol    hold_symbol;
	STablemapRegion	   hold_region;
	STablemapFont			 hold_font;
	STablemapHashPoint hold_hash_point;
	STablemapHashValue hold_hash_value;
	STablemapImage		 hold_image;
	STablemapTemplate		 hold_template;
	// Clean up the global.profile structure
	ClearTablemap();
	CSLock lock(m_critsec);
  assert(_fname != "");
	_filename = _fname;
	_filepath = _fname;
	// Open the selected file
	CFile cfFile(_filename, CFile::modeRead | CFile::shareDenyNone);
	// Load its contents into a CArchive
	CArchive ar (&cfFile, CArchive::load);
	// Read the first line of the CArchive into strLine
	strLine = "";
	int linenum = 1;
	// Failed, so quit
	if (!ar.ReadString(strLine))
	{
		WarnAboutGeneralTableMapError(linenum, ERR_EOF);
		return ERR_EOF;
	}
	// skip any blank lines
	while (strLine.GetLength() == 0) 
	{
		linenum++;
		// Failed, so quit
		if (!ar.ReadString(strLine)) 
		{
			WarnAboutGeneralTableMapError(linenum, ERR_EOF);
			return ERR_EOF;
		}
	}
	//
	// Validate file version (always first line).
	// It should always be version 2.
	//
	if ((memcmp(strLine.GetString(), VER_OPENHOLDEM_2, strlen(VER_OPENHOLDEM_2)) != 0 
		&& memcmp(strLine.GetString(), VER_OPENSCRAPE_2, strlen(VER_OPENSCRAPE_2)) != 0))
	{
		CString error_message;
		error_message.Format("This is a version 1 table map.\n"
			"\n"
			"Version 2.0.0 and higher of OpenHoldem use a new format (version 2).\n"  
			"This table map is highly unlikely to work correctly until it has been\n"
			"opened in OpenScrape version 2.0.0 or higher, and adjustments\n"
			"have been made to autoplayer settings and region sizes.\n"
			"\n"
			"Filename: %s"
			"\n"
			"To avoid costly mis-scrapes and crashes OpenHoldem will terminate now.\n",
			_filename);
		MessageBox_Error_Warning(error_message, "Table map load error");
		PostQuitMessage(1);
		return ERR_VERSION;
	}
  // Repeat while there are lines in the file left to process
	do {
		// Trim the line
		strLine.Trim();
		// If the line is empty, skip it
		if (strLine.GetLength() == 0)
			continue;
		// Extract the line type
		pos=0;
		strLineType = strLine.Tokenize(" \t", pos);
		// Skip comment lines
		if (strLineType.Left(2) == "//")
			continue;
		// Skip version lines
		if (strLineType == VER_OPENHOLDEM_1 ||
				strLineType == VER_OPENHOLDEM_2 ||
				strLineType == VER_OPENSCRAPE_1 ||
				strLineType == VER_OPENSCRAPE_2) 
		{
			continue;
		}

		hold_template.left = 0;
		hold_template.top = 0;
		hold_template.right = 0;
		hold_template.bottom = 0;
		hold_template.width = 0;
		hold_template.height = 0;
		hold_template.use_default = true;
		hold_template.match_mode = -1;
		hold_template.created = false;

		// Handle z$ lines (sizes)
		if (strLineType.Left(2) == "z$") 
		{
			// name
			hold_size.name = strLineType.Mid(2);
			// width
			token = strLine.Tokenize(" \t", pos);
			if (token.GetLength()==0)
			{
				WarnAboutGeneralTableMapError(linenum, ERR_SYNTAX);
				return ERR_SYNTAX;
			}
			hold_size.width = atol(token.GetString());
			// height
			token = strLine.Tokenize(" \t", pos);
			if (token.GetLength()==0)
			{
				WarnAboutGeneralTableMapError(linenum, ERR_SYNTAX);
				return ERR_SYNTAX;
			}
			hold_size.height = atol(token.GetString());
			if (!z$_insert(hold_size))
			{
				ZMapCI z_iter = _z$.find(hold_size.name);
				if (z_iter != _z$.end())
				{
					t.Format("'%s' skipped, as this size record already exists.\nYou have to fix that tablemap.", strLine);
					MessageBox_Error_Warning(t.GetString(), "ERROR adding size record");			
				}
				else
				{
					MessageBox_Error_Warning(strLine, "ERROR adding size record");			
				}
			}
		}
		// Handle s$ lines (symbols/string)
		else if (strLineType.Left(2) == "s$") 
		{
			// name
			hold_symbol.name = strLineType.Mid(2);
			// text
			hold_symbol.text = strLine.Mid(strLineType.GetLength());
			hold_symbol.text.Trim();
			if ((hold_symbol.text.GetLength() == 0) && (hold_symbol.name != "titletext"))
			{
        // Symbols must be non-empty.
        // With the single exception of an empty main title-tedt,
        // supported since version 11.0.1
        // http://www.maxinmontreal.com/forums/viewtopic.php?f=124&t=20382
				WarnAboutGeneralTableMapError(linenum, ERR_SYNTAX);
				return ERR_SYNTAX;
			}
			// Skip _s$hXtype lines
			if (strLineType.Left(3) == "s$h" && strLineType.Mid(4,4) == "type")
			{
			}
			else
			{
				// Assure, sitename and network are lowercases.
				if ((strLineType.Left(10) == "s$sitename") || (strLineType.Left(9) == "s$network"))
				{
					CString temp = hold_symbol.text.MakeLower();
					hold_symbol.text = temp;
				}

				if (!s$_insert(hold_symbol))
				{
					SMapCI s_iter = _s$.find(hold_symbol.name);
					if (s_iter != _s$.end())
					{
						t.Format("'%s' skipped, as this string/symbol record already exists.\nYou have to fix that tablemap.", strLine);
						MessageBox_Error_Warning(t.GetString(), "ERROR adding string/symbol record");			
					}
					else
					{
						MessageBox_Error_Warning(strLine, "ERROR adding string/symbol record");			
					}
				}
			}
		}

		// Handle r$ lines (regions)
		else if (strLineType.Left(2) == "r$") 
		{
			// name
			hold_region.name = strLineType.Mid(2);

			// left
			token = strLine.Tokenize(" \t", pos);
			if (token.GetLength()==0)
			{
				WarnAboutGeneralTableMapError(linenum, ERR_SYNTAX);
				return ERR_SYNTAX;
			}

			hold_region.left = atol(token.GetString());

			// top
			token = strLine.Tokenize(" \t", pos);
			if (token.GetLength()==0)
			{
				WarnAboutGeneralTableMapError(linenum, ERR_SYNTAX);
				return ERR_SYNTAX;
			}

			hold_region.top = atol(token.GetString());

			// right
			token = strLine.Tokenize(" \t", pos);
			if (token.GetLength()==0)
			{
				WarnAboutGeneralTableMapError(linenum, ERR_SYNTAX);
				return ERR_SYNTAX;
			}

			hold_region.right = atol(token.GetString());

			// bottom
			token = strLine.Tokenize(" \t", pos);
			if (token.GetLength()==0)
			{
				WarnAboutGeneralTableMapError(linenum, ERR_SYNTAX);
				return ERR_SYNTAX;
			}

			hold_region.bottom = atol(token.GetString());

			// color
			token = strLine.Tokenize(" \t", pos);
			if (token.GetLength()==0)
			{
				WarnAboutGeneralTableMapError(linenum, ERR_SYNTAX);
				return ERR_SYNTAX;
			}

			hold_region.color = strtoul(token.GetString(), 0, 16);

			// radius
			token = strLine.Tokenize(" \t", pos);
			if (token.GetLength()==0)
			{
				WarnAboutGeneralTableMapError(linenum, ERR_SYNTAX);
				return ERR_SYNTAX;
			}

			hold_region.radius = atol(token.GetString());

			// transform
			hold_region.transform = strLine.Tokenize(" \t", pos);
			if (hold_region.transform.GetLength() == 0)
			{
				WarnAboutGeneralTableMapError(linenum, ERR_SYNTAX);
				return ERR_SYNTAX;
			}

			// pass OCR fields for old TM format
			token = strLine.Tokenize(" \t", pos);
			if (token.GetLength() == 0)
			{
				hold_region.use_default = true;
				hold_region.threshold = -1;
				hold_region.use_cropping = false;
				hold_region.crop_size = -1;
				hold_region.box_color = -1;

				goto EndRegion;
				//WarnAboutGeneralTableMapError(linenum, ERR_SYNTAX);
				//return ERR_SYNTAX;
			}

			// OCR image processing
			hold_region.use_default = atol(token.GetString());

			// threshold
			token = strLine.Tokenize(" \t", pos);
			if (token.GetLength() == 0)
			{
				//WarnAboutGeneralTableMapError(linenum, ERR_SYNTAX);
				//return ERR_SYNTAX;
			}

			hold_region.threshold = atol(token.GetString());

			// use cropping
			token = strLine.Tokenize(" \t", pos);
			if (token.GetLength() == 0)
			{
				//WarnAboutGeneralTableMapError(linenum, ERR_SYNTAX);
				//return ERR_SYNTAX;
			}

			hold_region.use_cropping = atol(token.GetString());

			// crop size
			token = strLine.Tokenize(" \t", pos);
			if (token.GetLength() == 0)
			{
				//WarnAboutGeneralTableMapError(linenum, ERR_SYNTAX);
				//return ERR_SYNTAX;
			}

			hold_region.crop_size = atol(token.GetString());

			// box color
			token = strLine.Tokenize(" \t", pos);
			if (token.GetLength() == 0)
			{
				//WarnAboutGeneralTableMapError(linenum, ERR_SYNTAX);
				//return ERR_SYNTAX;
			}

			hold_region.box_color = atol(token.GetString());

			// flags
			//token = strLine.Tokenize(" \t", pos);
			//if (token.GetLength()==0) { return ERR_SYNTAX; }
			//hold_region.flags = atol(token.GetString());

	EndRegion:
			if (!r$_insert(hold_region))
			{
				RMapCI r_iter = _r$.find(hold_region.name);
				if (r_iter != _r$.end())
				{
					t.Format("'%s' skipped, as this region record already exists.\nYou have to fix that tablemap.", strLine);
					MessageBox_Error_Warning(t.GetString(), "ERROR adding region record");			
				}
				else
				{
					MessageBox_Error_Warning(strLine, "ERROR adding region record");			
				}
			}
		}

		// Handle t$ lines (fonts)
		else if (strLineType.Left(2) == "t$" ||
				 (strLineType.Left(1) == 't' &&
				  strLineType[1] >= '0' &&
				  CString(strLineType[1]) < MaxFontGroup &&
				  strLineType[2] == '$')) 
		{

			int font_group = 0;

			// New style t$ records (font groups 0..k_max_number_of_font_groups_in_tablemap)
			t = strLineType[3];
			hold_font.ch = t[0];
			font_group = DigitCharacterToNumber(strLineType[1]);

			if (font_group < 0 || font_group >= k_max_number_of_font_groups_in_tablemap) {
				MessageBox_Error_Warning(strLine, "Invalid font group\nFont groups have to be in the range [0..k_max_number_of_font_groups_in_tablemap]");
				return ERR_SYNTAX;
			}

			int i = 0;
			hold_font.hexmash = "";
			while ((token = strLine.Tokenize(" \t", pos))!="" && i<=30 && pos!=-1) 
			{
				hold_font.x[i++] = strtoul(token, 0, 16);
				hold_font.hexmash.Append(token);
			}
			hold_font.x_count = i;

			// Add the new t$ record to the internal array
			if (!t$_insert(font_group, hold_font))
			{
				TMapCI t_iter = _t$[font_group].find(hold_font.hexmash);
				if (t_iter != _t$[font_group].end())
				{
					t.Format("'%s' skipped, as this character already exists in group %d as '%c'.\nYou have to fix that tablemap.",
						strLine, font_group, t_iter->second.ch);
					MessageBox_Error_Warning(t.GetString(), "ERROR adding font record");			
				}
				else
				{
					MessageBox_Error_Warning(strLine, "ERROR adding font record");			
				}
			}
		}

		// Handle p$ lines (hash points)
		else if (strLineType.Left(1) == 'p' &&
				 strLineType[1] >= '0' &&
				 CString(strLineType[1]) < MaxHashGroup &&
				 strLineType[2] == '$') 
		{
			// number
			int hashpoint_group = DigitCharacterToNumber(strLineType[1]);
			if (hashpoint_group < 0 || hashpoint_group >= k_max_number_of_hash_groups_in_tablemap)
			{

				MessageBox_Error_Warning(strLine, "Invalid hash point group\nHash point groups have to be in the range [0..k_max_number_of_hashpoint_groups_in_tablemap]");
				return ERR_SYNTAX;
			}

			// x
			token = strLine.Tokenize(" \t", pos);
			if (token.GetLength()==0)
			{
				WarnAboutGeneralTableMapError(linenum, ERR_SYNTAX);
				return ERR_SYNTAX;
			}

			hold_hash_point.x = atol(token.GetString());

			// y
			if (token.GetLength()==0)
			{
				WarnAboutGeneralTableMapError(linenum, ERR_SYNTAX);
				return ERR_SYNTAX;
			}

			token = strLine.Tokenize(" \t", pos);
			hold_hash_point.y = atol(token.GetString());

			// Add the new p$ record to the internal array
			if (!p$_insert(hashpoint_group, hold_hash_point))
			{
				PMapCI p_iter = _p$[hashpoint_group].find(((hold_hash_point.x&0xffff)<<16) | (hold_hash_point.y&0xffff));
				if (p_iter != _p$[hashpoint_group].end())
				{
					t.Format("'%s' skipped, as hash point (%d, %d) already exists in group %d.\nYou have to fix that tablemap.", 
						strLine, hold_hash_point.x, hold_hash_point.y, hashpoint_group);
					MessageBox_Error_Warning(t.GetString(), "ERROR adding hash point record");			
				}
				else
				{
					MessageBox_Error_Warning(strLine, "ERROR adding hash point record");			
				}
			}
		}

		// Handle h$ lines (hash values)
		else if (strLineType.Left(1) == 'h' &&
				 strLineType[1] >= '0' &&
				 CString(strLineType[1]) < MaxHashGroup &&
				 strLineType[2] == '$') 
		{
			// number
			int hash_group = DigitCharacterToNumber(strLineType[1]);
			if (hash_group < 0 || hash_group >= k_max_number_of_hash_groups_in_tablemap)
			{
				MessageBox_Error_Warning(strLine, "Invalid hash group\nHash groups have to be in the range [0..k_max_number_of_hash_groups_in_tablemap]");
				return ERR_SYNTAX;
			}

			// name
			hold_hash_value.name = strLineType.Mid(3);

			// value
			token = strLine.Tokenize(" \t", pos);
			if (token.GetLength()==0)
			{
				WarnAboutGeneralTableMapError(linenum, ERR_SYNTAX);
				return ERR_SYNTAX;
			}

			hold_hash_value.hash = strtoul(token.GetString(), 0, 16);

			// Add the new h$ record to the internal array
			if (!h$_insert(hash_group, hold_hash_value))
			{
				HMapCI h_iter = _h$[hash_group].find(hold_hash_value.hash);
				if (h_iter != _h$[hash_group].end())
				{
					t.Format("'%s' skipped, as hash %08x already exists in group %d.\nYou have to fix that tablemap.", 
						strLine, hold_hash_value.hash, hash_group);
					MessageBox_Error_Warning(t.GetString(), "ERROR adding hash record");			
				}
				else
				{
					MessageBox_Error_Warning(strLine, "ERROR adding hash record");			
				}

			}
		}

		// Handle i$ lines (images)
		else if (strLineType.Left(2) == "i$") 
		{
			// name
			hold_image.name = strLineType.Mid(2);

			// width
			token = strLine.Tokenize(" \t", pos);
			if (token.GetLength()==0)
			{
				WarnAboutGeneralTableMapError(linenum, ERR_SYNTAX);
				return ERR_SYNTAX;
			}

			hold_image.width = atol(token.GetString());

			// height
			token = strLine.Tokenize(" \t", pos);
			if (token.GetLength()==0)
			{
				WarnAboutGeneralTableMapError(linenum, ERR_SYNTAX);
				return ERR_SYNTAX;
			}

			hold_image.height = atol(token.GetString());

			// Check size of region
			if (hold_image.width * hold_image.height > MAX_IMAGE_WIDTH*MAX_IMAGE_HEIGHT)
			{
				WarnAboutGeneralTableMapError(linenum, ERR_REGION_SIZE);
				return ERR_REGION_SIZE;
			}

			// Allocate space for "RGBAImage"
			t = hold_image.name + ".ppm";
			hold_image.image = new RGBAImage(hold_image.width, hold_image.height, t.GetString());

			// read next "height" lines
			for (y=0; y < hold_image.height; y++) 
			{
				linenum++;
				if (!ar.ReadString(strLine)) 
				{
					WarnAboutGeneralTableMapError(linenum, ERR_SYNTAX);
					return ERR_SYNTAX;
				}
				// scan across "width" of line to get values
				for (x=0; x < hold_image.width; x++) 
				{
					// unreverse bgra to abgr
					hexval = strLine.Mid(x*8+6, 2) + strLine.Mid(x*8, 6);
					hold_image.pixel[y*hold_image.width + x] = strtoul(hexval, 0, 16);
					BYTE alpha = (hold_image.pixel[y*hold_image.width + x] >> 24) &0xff;
					BYTE blue =  (hold_image.pixel[y*hold_image.width + x] >> 16) &0xff;
					BYTE green = (hold_image.pixel[y*hold_image.width + x] >>  8) &0xff;
					BYTE red =   (hold_image.pixel[y*hold_image.width + x] >>  0) &0xff;
					hold_image.image->Set(red, green, blue, alpha, y*hold_image.width + x);
				}
			}

#ifdef OPENHOLDEM_PROGRAM
	write_log(Preferences()->debug_tablemap_loader(), "[CTablemap] Trying to insert image\n");
#endif
			// Add the new i$ record to the internal array
			if (!i$_insert(hold_image))
			{
#ifdef OPENHOLDEM_PROGRAM
	write_log(Preferences()->debug_tablemap_loader(), "[CTablemap] Looking up image\n");
#endif
				IMapCI i_iter = _i$.find(CreateI$Index(hold_image.name, hold_image.width, hold_image.height, hold_image.pixel));
#ifdef OPENHOLDEM_PROGRAM
	write_log(Preferences()->debug_tablemap_loader(), "[CTablemap] Looked up image\n");
#endif
				if (i_iter != _i$.end())
				{
					t.Format("'%s' skipped, as image already exists as '%s', with identical width, height and pixels.\nYou have to fix that tablemap.", 
						strLineType, i_iter->second.name);
					MessageBox_Error_Warning(t.GetString(), "ERROR adding image record");			
				}
				else
				{
					MessageBox_Error_Warning(strLine, "ERROR adding image record");			
				}
			}
			// Delete images later, when they are no longer needed,
			// probably best in the destructor, otherwise we get a memory-leak.
			// We can't delete them here, as we added a referece some line above: i$_insert(hold_image).
			// hold_image.image->~RGBAImage();
		}

		// Handle tpl$ lines (templates)
		else if (strLineType.Left(4) == "tpl$")
		{
			// name
			hold_template.name = strLineType.Mid(4);

			// width
			token = strLine.Tokenize(" \t", pos);
			if (token.GetLength() == 0)
			{
				WarnAboutGeneralTableMapError(linenum, ERR_SYNTAX);
				return ERR_SYNTAX;
			}

			hold_template.width = atol(token.GetString());

			// height
			token = strLine.Tokenize(" \t", pos);
			if (token.GetLength() == 0)
			{
				WarnAboutGeneralTableMapError(linenum, ERR_SYNTAX);
				return ERR_SYNTAX;
			}

			hold_template.height = atol(token.GetString());

			// Check size of region
			if (hold_template.width * hold_template.height > MAX_IMAGE_WIDTH*MAX_IMAGE_HEIGHT)
			{
				WarnAboutGeneralTableMapError(linenum, ERR_REGION_SIZE);
				return ERR_REGION_SIZE;
			}

			// use default
			token = strLine.Tokenize(" \t", pos);
			if (token.GetLength() == 0)
			{
				WarnAboutGeneralTableMapError(linenum, ERR_SYNTAX);
				return ERR_SYNTAX;
			}
			hold_template.use_default = atol(token.GetString());

			// match mode
			token = strLine.Tokenize(" \t", pos);
			if (token.GetLength() == 0)
			{
				WarnAboutGeneralTableMapError(linenum, ERR_SYNTAX);
				return ERR_SYNTAX;
			}
			hold_template.match_mode = atol(token.GetString());

			// created
			token = strLine.Tokenize(" \t", pos);
			if (token.GetLength() == 0)
			{
				WarnAboutGeneralTableMapError(linenum, ERR_SYNTAX);
				return ERR_SYNTAX;
			}
			hold_template.created = atol(token.GetString());

			// Allocate space for "RGBAImage"
			t = hold_template.name + ".ppm";
			hold_template.image = new RGBAImage(hold_template.width, hold_template.height, t.GetString());

			// read next "height" lines
			for (y = 0; y < hold_template.height; y++)
			{
				linenum++;
				if (!ar.ReadString(strLine))
				{
					WarnAboutGeneralTableMapError(linenum, ERR_SYNTAX);
					return ERR_SYNTAX;
				}
				// scan across "width" of line to get values
				for (x = 0; x < hold_template.width; x++)
				{
					// unreverse bgra to abgr
					hexval = strLine.Mid(x * 8 + 6, 2) + strLine.Mid(x * 8, 6);
					hold_template.pixel[y*hold_template.width + x] = strtoul(hexval, 0, 16);
					BYTE alpha = (hold_template.pixel[y*hold_template.width + x] >> 24) & 0xff;
					BYTE blue = (hold_template.pixel[y*hold_template.width + x] >> 16) & 0xff;
					BYTE green = (hold_template.pixel[y*hold_template.width + x] >> 8) & 0xff;
					BYTE red = (hold_template.pixel[y*hold_template.width + x] >> 0) & 0xff;
					hold_template.image->Set(red, green, blue, alpha, y*hold_template.width + x);
				}
			}

			// Add the new tpl$ record to the internal array
			if (!tpl$_insert(hold_template))
			{
				TPLMapCI tpl_iter = _tpl$.find(hold_template.name);
				if (tpl_iter != _tpl$.end())
				{
					t.Format("'%s' skipped, as this template record already exists.\nYou have to fix that tablemap.", strLine);
					MessageBox_Error_Warning(t.GetString(), "ERROR adding template record");
				}
				else
				{
					MessageBox_Error_Warning(strLine, "ERROR adding template record");
				}
			}
		}

		// Unknown line type
		else 
		{
			CString error;
			error.Format("Unknown Line Type.\n"
				"Some line in your tablemap is completely wrong\n"
				"and can't be processed at all:\n%s", strLine);
			MessageBox_Error_Warning(error, "TABLEMAP ERROR");
			return ERR_UNK_LN_TYPE;
		}

		linenum++;
	} while (ar.ReadString(strLine));
  InitNChairs();
	_valid = true;
	return SUCCESS;
}

void CTablemap::WriteSectionHeader(CArchive& ar, CString header)
{
	ar.WriteString("//\r\n");
	CString header_comment;
	header_comment.Format("// %s\r\n", header);
	ar.WriteString(header_comment);
	ar.WriteString("//\r\n");
	ar.WriteString("\r\n");
}

int CTablemap::SaveTablemap(CArchive& ar, const char *version_text)
{
#ifdef OPENHOLDEM_PROGRAM
	write_log(Preferences()->debug_tablemap_loader(), "[CTablemap] SaveTablemap\n");
#endif

	CString		s = "", t = "", text = "";
	char		nowtime[26] = {0};

	// Version
	s.Format("%s\r\n", VER_OPENSCRAPE_2); ar.WriteString(s);
	ar.WriteString("\r\n");

	// Comment header
	s.Format("// OpenScrape %s\r\n", version_text); ar.WriteString(s);
	ar.WriteString("\r\n");

	// Date/Time
  //!!!s.Format("// %s\r\n", get_time(nowtime)); ar.WriteString(s);
	ar.WriteString("// 32 bits per pixel\r\n");
	ar.WriteString("\r\n");

	// sizes
	WriteSectionHeader(ar, "sizes");	
	for (ZMapCI z_iter=_z$.begin(); z_iter!=_z$.end(); z_iter++)
	{
		s.Format("z$%-16s %d  %d\r\n", z_iter->second.name, z_iter->second.width, z_iter->second.height); 
		ar.WriteString(s);
	}
	ar.WriteString("\r\n");

	// strings
	WriteSectionHeader(ar, "strings");
	for (SMapCI s_iter=_s$.begin(); s_iter!=_s$.end(); s_iter++)
	{
		s.Format("s$%-25s %s\r\n", s_iter->second.name.GetString(), s_iter->second.text.GetString());
		ar.WriteString(s);
	}
	ar.WriteString("\r\n");

	// regions
	WriteSectionHeader(ar, "regions");
	for (RMapCI r_iter=_r$.begin(); r_iter!=_r$.end(); r_iter++)
	{
		s.Format("r$%-18s %3d %3d %3d %3d %8x %4d %s %d %3d %d %3d %d\r\n", r_iter->second.name.GetString(),
			r_iter->second.left, r_iter->second.top, r_iter->second.right, r_iter->second.bottom,
			r_iter->second.color, r_iter->second.radius, r_iter->second.transform, r_iter->second.use_default,
			r_iter->second.threshold, r_iter->second.use_cropping, r_iter->second.crop_size, r_iter->second.box_color);
		ar.WriteString(s);
	}
	ar.WriteString("\r\n");

	// fonts
	WriteSectionHeader(ar, "fonts");
	for (int i = 0; i < k_max_number_of_font_groups_in_tablemap; i++)
	{
		for (TMapCI t_iter =_t$[i].begin(); t_iter != _t$[i].end(); t_iter++)
		{
			s.Format("t%d$%c", i, t_iter->second.ch);
			for (int j=0; j<t_iter->second.x_count; j++) 
			{ 
				t.Format(" %x", t_iter->second.x[j]);
				s.Append(t);
			}
			s.Append("\r\n");
			ar.WriteString(s);
		}
	}
	ar.WriteString("\r\n");

	// points
	WriteSectionHeader(ar, "points");
	for (int i = 0; i < k_max_number_of_hash_groups_in_tablemap; i++)
	{
		for (PMapCI p_iter=_p$[i].begin(); p_iter!=_p$[i].end(); p_iter++)
		{
			s.Format("p%d$%4d %4d\r\n", i, p_iter->second.x, p_iter->second.y);  
			ar.WriteString(s);
		}
	}
	ar.WriteString("\r\n");

	// hash
	WriteSectionHeader(ar, "hash");
	for (int i = 0; i < k_max_number_of_hash_groups_in_tablemap; i++)
	{
		for (HMapCI h_iter=_h$[i].begin(); h_iter!=_h$[i].end(); h_iter++)
		{
			s.Format("h%d$%-18s %08x\r\n", i, h_iter->second.name.GetString(), h_iter->second.hash);
			ar.WriteString(s);
		}
	}
	ar.WriteString("\r\n");

	// images
	WriteSectionHeader(ar, "images");
	for (IMapCI i_iter=_i$.begin(); i_iter!=_i$.end(); i_iter++)
	{
		int width = i_iter->second.width;
		int height = i_iter->second.height;

		s.Format("i$%-16s %-3d %-3d\r\n", i_iter->second.name, width, height);
		ar.WriteString(s);
		for (int y=0; y<height; y++)
		{
			s = "";
			for (int x=0; x<width; x++)
			{
				text.Format("%08x", i_iter->second.pixel[y*width + x]);
				s.Append(text.Mid(2, 6));
				s.Append(text.Mid(0,2));
			}
			s.Append("\r\n");
			ar.WriteString(s);
		}
	}
	ar.WriteString("\r\n");

	// templates
	WriteSectionHeader(ar, "templates");
	for (TPLMapCI tpl_iter = _tpl$.begin(); tpl_iter != _tpl$.end(); tpl_iter++)
	{
		int width = tpl_iter->second.width;
		int height = tpl_iter->second.height;

		s.Format("tpl$%-16s  %3d %3d %d %d %d\r\n", 
			tpl_iter->second.name, tpl_iter->second.width, tpl_iter->second.height,
			tpl_iter->second.use_default, tpl_iter->second.match_mode, tpl_iter->second.created);
		ar.WriteString(s);
		for (int y = 0; y<height; y++)
		{
			s = "";
			for (int x = 0; x<width; x++)
			{
				text.Format("%08x", tpl_iter->second.pixel[y*width + x]);
				s.Append(text.Mid(2, 6));
				s.Append(text.Mid(0, 2));
			}
			s.Append("\r\n");
			ar.WriteString(s);
		}
	}
	ar.WriteString("\r\n");

	return SUCCESS;	
}

void CTablemap::ErrorHashCollision(CString name1, CString name2) {
  CString message;
  message.Format("%s%s%s%s%s%s%s",
    "Hash collision:\n\n", 
		name1, " and ", name2, "\n\n",
    "This looks like a naming conflict.\n",
    "Please remove or rename the duplicate items.\n");
  MessageBox_Error_Warning(message, "Hash collision");
}

int CTablemap::UpdateHashes(const HWND hwnd, const char *startup_path)
{
#ifdef OPENHOLDEM_PROGRAM
	write_log(Preferences()->debug_tablemap_loader(), "[CTablemap] UpdateHashes\n");
#endif	

	CString					s = "";
	uint32_t				pixels[MAX_HASH_WIDTH*MAX_HASH_HEIGHT] = {0}, filtered_pix[MAX_HASH_WIDTH*MAX_HASH_HEIGHT] = {0};
	int						pixcount = 0;
	bool					all_i$_found = false, this_i$_found = false;
	FILE					*fp = NULL;
	char					timebuf[30] = {0};
	CString					logpath = "";
	STablemapHashValue		hold_hash_value;
	STablemapHashValue		h$_record;
	CArray <STablemapHashValue, STablemapHashValue>	new_hashes[k_max_number_of_hash_groups_in_tablemap];
	CArray <STablemapHashValue, STablemapHashValue>	unmatched_h$_records[k_max_number_of_hash_groups_in_tablemap];

	// Get number of records of each type in this table map
	if (_i$.begin()==_i$.end()) 
	{
		MessageBox_Error_Warning("No images found - cannot create hashes.", "Table Map Error");
		return ERR_NOTMASTER;
	}

	// Loop through all the hash (h$) records, and check for a corresponding image (i$) record
	// Log missing records and display message if we can't find them all
	all_i$_found = true;
	for (int i = 0; i < k_max_number_of_hash_groups_in_tablemap; i++)
	{
		unmatched_h$_records[i].RemoveAll();
	}
	for (int i = 0; i < k_max_number_of_hash_groups_in_tablemap; i++)
	{
		for (HMapCI h_iter=_h$[i].begin(); h_iter!=_h$[i].end(); h_iter++)
		{
			this_i$_found = false;
			for (IMapCI i_iter=_i$.begin(); i_iter!=_i$.end(); i_iter++) 
			{
				if (i_iter->second.name == h_iter->second.name) 
					this_i$_found = true;
			}

			if (!this_i$_found) 
			{
				all_i$_found = false;
				h$_record.name = h_iter->second.name;
				h$_record.hash = h_iter->second.hash;
				unmatched_h$_records[i].Add(h$_record);
			}
		}
	}

	if (!all_i$_found) 
	{
		logpath.Format("%s\\hash creation log.txt", startup_path);
		if (fopen_s(&fp, logpath.GetString(), "a")==0)
		{
			//!!!get_now_time(timebuf);
			fprintf(fp, "<%s>\nCreating _hashes\n", timebuf);
			fprintf(fp, "Hashes with no matching image:\n");

			for (int i = 0; i < k_max_number_of_hash_groups_in_tablemap; i++)
			{
				for (int j=0; j<unmatched_h$_records[i].GetCount(); j++) 
				{
					fprintf(fp, "\t%3d. h$%s\n", j+1, unmatched_h$_records[i].GetAt(j).name.GetString());
				}
			}

			fprintf(fp, "=======================================================\n\n");
			fclose(fp);
		}

		MessageBox_Error_Warning("Could not complete hash creation, due to missing images.\n"\
				   "Please see the \"hash creation log.txt\" file for details.", 
				   "Hash Creation Error");
	}

	// Init new hash array
	for (int i = 0; i < k_max_number_of_hash_groups_in_tablemap; i++)
		new_hashes[i].RemoveAll();

	// Loop through each of the image records and create hashes
	for (IMapCI i_iter=_i$.begin(); i_iter!=_i$.end(); i_iter++) 
	{
		// Loop through the h$ records to find all the hashes that we have to create for this image record
		for (int i = 0; i < k_max_number_of_hash_groups_in_tablemap; i++)
		{
			for (HMapCI h_iter=_h$[i].begin(); h_iter!=_h$[i].end(); h_iter++) 
			{
				if (i_iter->second.name == h_iter->second.name) 
				{
					// create hash, based on type
					hold_hash_value.name = i_iter->second.name;

					// all pixel hash
					if (i == 0) 
					{
						// only create hash based on rgb values - ignore alpha
						for (int j=0; j < (int) (i_iter->second.width * i_iter->second.height); j++)
							filtered_pix[j] = i_iter->second.pixel[j] & RGB_MASK;

						hold_hash_value.hash = hashword(&filtered_pix[0], i_iter->second.width * i_iter->second.height, HASH_SEED_0);
					}

					// selected pixel hash
					else if (i >= 1 && i < k_max_number_of_hash_groups_in_tablemap) 
					{
						pixcount = 0;
						for (PMapCI p_iter=_p$[i].begin(); p_iter!=_p$[i].end(); p_iter++)
						{
							if (p_iter->second.x <= i_iter->second.width &&
								p_iter->second.y <= i_iter->second.height) 
							{
								// only create hash based on rgb values - ignore alpha
								pixels[pixcount++] = 
									i_iter->second.pixel[(p_iter->second.y * i_iter->second.width) + p_iter->second.x] & RGB_MASK;
							}
						}

						if (i==1)
								hold_hash_value.hash = hashword(&pixels[0], pixcount, HASH_SEED_1);
						if (i==2)
								hold_hash_value.hash = hashword(&pixels[0], pixcount, HASH_SEED_2);
						if (i==3)
								hold_hash_value.hash = hashword(&pixels[0], pixcount, HASH_SEED_3);

					}

					// Check for hash collision
					for (int j=0; j<(int) new_hashes[i].GetSize(); j++) 
					{
						if (hold_hash_value.hash == new_hashes[i].GetAt(j).hash) 
						{
							ErrorHashCollision(i_iter->second.name.GetString(), 
								new_hashes[i].GetAt(j).name.GetString()); 
							return ERR_HASH_COLL;
						}
					}

					// Store this new hash in the temporary holding CArray
					new_hashes[i].Add(hold_hash_value);
				}
			}  // for (HMapCI h_iter=_h$.begin(); h_iter!=_h$.end(); h_iter++)
		} // for (int i=0; i<=3; i++)
	} // for (IMapCI i_iter=_i$.begin(); i_iter!=_i$.end(); i_iter++)


	// Copy the temporary new hash CArray to the internal std::map
	for (int i = 0; i < k_max_number_of_hash_groups_in_tablemap; i++)
		h$_clear(i);

	for (int i = 0; i < k_max_number_of_hash_groups_in_tablemap; i++)
	{
		for (int j=0; j<(int) new_hashes[i].GetSize(); j++)
		{
			if (!h$_insert(i, new_hashes[i].GetAt(j)))
			{
        CString e;
				e.Format("Hash record: %d %s %08x", i, new_hashes[i].GetAt(j).name, new_hashes[i].GetAt(j).hash);
				MessageBox_Error_Warning(e, "ERROR adding hash value record: " + hold_hash_value.name);	
			}
		}
	}

	if (!all_i$_found)
	{
		WarnAboutGeneralTableMapError(0, ERR_INCOMPLETEMASTER);	
		return ERR_INCOMPLETEMASTER;
	}

	return SUCCESS;
}

// Used by OpenScrape to calculate initial hash when first creating it with "Calculate Hash" button
uint32_t CTablemap::CalculateHashValue(IMapCI i_iter, const int type)
{
#ifdef OPENHOLDEM_PROGRAM
	write_log(Preferences()->debug_tablemap_loader(), "[CTablemap] CalculateHashValue\n");
#endif

	uint32_t pixels[MAX_HASH_WIDTH*MAX_HASH_HEIGHT] = {0}, filtered_pix[MAX_HASH_WIDTH*MAX_HASH_HEIGHT] = {0};

	// all pixel hash
	if (type == 0) 
	{
		// only create hash based on rgb values - ignore alpha
		for (int j=0; j < (int) (i_iter->second.width * i_iter->second.height); j++)
			filtered_pix[j] = i_iter->second.pixel[j] & RGB_MASK;

		return hashword(&filtered_pix[0], i_iter->second.width * i_iter->second.height, HASH_SEED_0);
	}

	// selected pixel hash
	else if (type >= 1 && type < k_max_number_of_hash_groups_in_tablemap) 
	{
		int pixcount = 0;
		for (PMapCI p_iter=_p$[type].begin(); p_iter!=_p$[type].end(); p_iter++)
		{
			if (p_iter->second.x <= i_iter->second.width &&
				p_iter->second.y <= i_iter->second.height) 
			{
				// only create hash based on rgb values - ignore alpha
				pixels[pixcount++] = 
					i_iter->second.pixel[(p_iter->second.y * i_iter->second.width) + p_iter->second.x] & RGB_MASK;
			}
		}

		if (type==1)
				return hashword(&pixels[0], pixcount, HASH_SEED_1);
		if (type==2)
				return hashword(&pixels[0], pixcount, HASH_SEED_2);
		if (type==3)
				return hashword(&pixels[0], pixcount, HASH_SEED_3);

	}

	return 0;
}

// Creates the 32bit hash for an image record using name and pixels
uint32_t CTablemap::CreateI$Index(const CString name, const int width, const int height, const uint32_t *pixels)
{
#ifdef OPENHOLDEM_PROGRAM
	write_log(Preferences()->debug_tablemap_loader(), "[CTablemap] CreateI$Index\n");
	write_log(Preferences()->debug_tablemap_loader(), "[CTablemap] Name %s\n", name);
	write_log(Preferences()->debug_tablemap_loader(), "[CTablemap] Width %i\n", width);
	write_log(Preferences()->debug_tablemap_loader(), "[CTablemap] Height %i\n", height);
#endif

	assert(width <= MAX_HASH_WIDTH);
	assert(height <= MAX_HASH_HEIGHT);

	uint32_t *uints = new uint32_t[MAX_HASH_WIDTH*MAX_HASH_HEIGHT + name.GetLength()];
#ifdef OPENHOLDEM_PROGRAM
	write_log(Preferences()->debug_tablemap_loader(), "[CTablemap] Buffer created %i\n",
		uints);
#endif

	int c = 0;
#ifdef OPENHOLDEM_PROGRAM
	write_log(Preferences()->debug_tablemap_loader(), "[CTablemap] Putting the name into the buffer\n");
#endif
	// Put the ascii value of each letter into a uint32_t
	for (int i=0; i<name.GetLength(); i++)
		uints[c++] = name[i];

#ifdef OPENHOLDEM_PROGRAM
	write_log(Preferences()->debug_tablemap_loader(), "[CTablemap] Putting the pixels into the buffer\n");
#endif
	// Now the pixels
	for (int i=0; i<(int) (height * width); i++)
		uints[c++] = pixels[i];

#ifdef OPENHOLDEM_PROGRAM
	write_log(Preferences()->debug_tablemap_loader(), "[CTablemap] Calculating the hash value\n");
#endif
	uint32_t index = hashword(&uints[0], height * width + name.GetLength(), 0x71e9ff36);

#ifdef OPENHOLDEM_PROGRAM
	write_log(Preferences()->debug_tablemap_loader(), "[CTablemap] Releasing the buffer %i\n",
		uints);
#endif
	delete []uints;
#ifdef OPENHOLDEM_PROGRAM
	write_log(Preferences()->debug_tablemap_loader(), "[CTablemap] Buffer released successfully\n");
#endif

	return index;
}

void CTablemap::InitNChairs() {
  // Previously this lookup-code was in the accessor nchairs()
  // but sometimes we got a crash related to string-operations
  // (3rd-party-code) somewhere deep inside GetTMSymbol().
  // As we didn't find the reason we now avoid these string operations#
  // and pre-compute nchairs once.
  int n = GetTMSymbol("nchairs", kMaxNumberOfPlayers);
  if (n >= 2 && n <= kMaxNumberOfPlayers) {
    _nchairs = n;
  } else {
    _nchairs = kMaxNumberOfPlayers;
  }
}

 