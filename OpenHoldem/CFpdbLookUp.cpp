#include "stdafx.h"
#include "CFpdbLookup.h"


#include "..\CTablemap\CTablemap.h"

CFpdbLookup fpdb_lookup;

CFpdbLookup::CFpdbLookup() {
	_fpdb_siteid.clear();
	// Information on the first sites supported by FPDB (obsolete):
	// http://fpdb.wikidot.com/general:features
	// All actual supported sites available on:
	// https://github.com/Mudr0x/fpdb-reloaded
	// All sitenames and networknames have to be in LOWER-CASES!
	_fpdb_siteid.insert(std::pair<CString, int> ("fulltilt", 1));
	_fpdb_siteid.insert(std::pair<CString, int> ("stars", 2));
	_fpdb_siteid.insert(std::pair<CString, int>("everleaf", 3));
	_fpdb_siteid.insert(std::pair<CString, int> ("boss", 4));
	_fpdb_siteid.insert(std::pair<CString, int> ("ongame", 5));
	_fpdb_siteid.insert(std::pair<CString, int> ("ultimate", 6));
	_fpdb_siteid.insert(std::pair<CString, int> ("betfair", 7));
	_fpdb_siteid.insert(std::pair<CString, int> ("absolute", 8));
	_fpdb_siteid.insert(std::pair<CString, int> ("party", 9));
	_fpdb_siteid.insert(std::pair<CString, int> ("pacific", 10));
	_fpdb_siteid.insert(std::pair<CString, int> ("partouche", 11));
	_fpdb_siteid.insert(std::pair<CString, int> ("merge", 12));
	_fpdb_siteid.insert(std::pair<CString, int>	("pkr", 13));
	_fpdb_siteid.insert(std::pair<CString, int> ("ipoker", 14));
	_fpdb_siteid.insert(std::pair<CString, int> ("winamax", 15));
	_fpdb_siteid.insert(std::pair<CString, int> ("everest", 16));
	_fpdb_siteid.insert(std::pair<CString, int> ("cake", 17));
	_fpdb_siteid.insert(std::pair<CString, int> ("entraction", 18));
	_fpdb_siteid.insert(std::pair<CString, int> ("betonline", 19));
	_fpdb_siteid.insert(std::pair<CString, int> ("microgaming", 20));
	_fpdb_siteid.insert(std::pair<CString, int> ("bovada", 21));
	_fpdb_siteid.insert(std::pair<CString, int> ("enet", 22));
	_fpdb_siteid.insert(std::pair<CString, int> ("swc", 23));
	_fpdb_siteid.insert(std::pair<CString, int> ("winningpoker", 24));
	_fpdb_siteid.insert(std::pair<CString, int> ("pokermaster", 25));
	_fpdb_siteid.insert(std::pair<CString, int> ("runitonce", 26));
	_fpdb_siteid.insert(std::pair<CString, int>	("ggpoker", 27));
	_fpdb_siteid.insert(std::pair<CString, int> ("kingsclub", 28));
	_fpdb_siteid.insert(std::pair<CString, int>	("pokerbros", 29));
	_fpdb_siteid.insert(std::pair<CString, int> ("unibet", 30));
	//_fpdb_siteid.insert(std::pair<CString, int> ("cereus", 800));
	//_fpdb_siteid.insert(std::pair<CString, int> ("b2b", 1000));
	//_fpdb_siteid.insert(std::pair<CString, int> ("bodog", 1300));
	//_fpdb_siteid.insert(std::pair<CString, int> ("cryptologic", 1500));
	//_fpdb_siteid.insert(std::pair<CString, int> ("wpex", 1800));
	//_fpdb_siteid.insert(std::pair<CString, int> ("tribeca", 1900));
	// 2000 not (yet) supported, whatever it is
	//_fpdb_siteid.insert(std::pair<CString, int> ("yatahay", 2400));
	//_fpdb_siteid.insert(std::pair<CString, int> ("barriere", 2600));
	// 2700 not (yet) supported, whatever it is
	//_fpdb_siteid.insert(std::pair<CString, int> ("peoples", 2800));
}

CFpdbLookup::~CFpdbLookup() {
  _fpdb_siteid.clear();
}

const int CFpdbLookup::GetSiteId()
{
	// Is s$sitename or s$network one of the supported FPDB sites? 
	// Return the proper site_id for db queries.
	// FPDB version only
	//
	// No longer requiring an exact match for manualmode,
  // but treating it like a normal casino.
  // http://www.maxinmontreal.com/forums/viewtopic.php?f=114&t=12158&p=108712#p108712

	std::map<CString, int>::const_iterator lookup, end;
	lookup = _fpdb_siteid.begin();
	end = _fpdb_siteid.end();

	CString sitename = p_tablemap->sitename();
	CString network = p_tablemap->network();

	while (lookup!=end)
	{
		CString supported_sitename = lookup->first;
		if (sitename.MakeLower().Find(supported_sitename.MakeLower()) >= 0
			|| network.CompareNoCase(supported_sitename) == 0)
		{
			write_log(Preferences()->debug_fpdb(), "[FpdbLookup] Site or network is: %s\n",
				supported_sitename);
			write_log(Preferences()->debug_fpdb(), "[FpdbLookup] SiteID is %i\n",
				lookup->second);
			return lookup->second;
		}
		lookup++;
	}
	write_log(Preferences()->debug_fpdb(), 
		"[FpdbLookup] ERROR: unsupported sitename and unsupported network.\n");
	write_log(Preferences()->debug_fpdb(), 
		"[FpdbLookup] Probably wrong data in tablemap.\n");
	return -1;
}