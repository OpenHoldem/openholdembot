#include "stdafx.h"
#include "CPokerTrackerLookup.h"


#include "CTablemap/CTablemap.h"

CPokerTrackerLookup pt_lookup;

CPokerTrackerLookup::CPokerTrackerLookup() {
	_pt4_siteid.clear();
	// Documentation about PT3 sited_IDs:
	// http://www.pokertracker.com/forums/viewtopic.php?f=18&t=20169&p=95629
	// All sitenames and networknames have to be in LOWER-CASES!
	_pt4_siteid.insert(std::pair<CString, int> ("stars", 100));
	_pt4_siteid.insert(std::pair<CString, int> ("party", 200));
	_pt4_siteid.insert(std::pair<CString, int> ("fulltilt", 300));
	_pt4_siteid.insert(std::pair<CString, int> ("ipoker", 400));
	_pt4_siteid.insert(std::pair<CString, int> ("everest", 500));
	_pt4_siteid.insert(std::pair<CString, int> ("ongame", 600));
	_pt4_siteid.insert(std::pair<CString, int> ("boss", 700));
	_pt4_siteid.insert(std::pair<CString, int> ("cereus", 800));
	_pt4_siteid.insert(std::pair<CString, int> ("pacific", 900));
	_pt4_siteid.insert(std::pair<CString, int> ("b2b", 1000));
	_pt4_siteid.insert(std::pair<CString, int> ("microgaming", 1100));
	_pt4_siteid.insert(std::pair<CString, int> ("cake", 1200));
	_pt4_siteid.insert(std::pair<CString, int> ("bodog", 1300));
	_pt4_siteid.insert(std::pair<CString, int> ("betfair", 1400));
	_pt4_siteid.insert(std::pair<CString, int> ("cryptologic", 1500));
	_pt4_siteid.insert(std::pair<CString, int> ("ultimate", 1600));
	_pt4_siteid.insert(std::pair<CString, int> ("absolute", 1700));
	_pt4_siteid.insert(std::pair<CString, int> ("wpex", 1800));
	_pt4_siteid.insert(std::pair<CString, int> ("tribeca", 1900));
	// 2000 not (yet) supported, whatever it is
	_pt4_siteid.insert(std::pair<CString, int> ("merge", 2100));
	_pt4_siteid.insert(std::pair<CString, int> ("winamax", 2200));
	_pt4_siteid.insert(std::pair<CString, int> ("everleaf", 2300));
	_pt4_siteid.insert(std::pair<CString, int> ("yatahay", 2400));
	_pt4_siteid.insert(std::pair<CString, int> ("enet", 2500));
	_pt4_siteid.insert(std::pair<CString, int> ("barriere", 2600));
	// 2700 not (yet) supported, whatever it is
	_pt4_siteid.insert(std::pair<CString, int> ("peoples", 2800));
}

CPokerTrackerLookup::~CPokerTrackerLookup() {
  _pt4_siteid.clear();
}

const int CPokerTrackerLookup::GetSiteId()
{
	// Is s$sitename or s$network one of the supported PT sites? 
	// Return the proper site_id for db queries.
	// PT version 4 only
	//
	// No longer requiring an exact match for manualmode,
  // but treating it like a normal casino.
  // http://www.maxinmontreal.com/forums/viewtopic.php?f=114&t=12158&p=108712#p108712

	std::map<CString, int>::const_iterator lookup, end;
	lookup = _pt4_siteid.begin();
	end = _pt4_siteid.end();

	CString sitename = p_tablemap->sitename();
	CString network = p_tablemap->network();

	while (lookup!=end)
	{
		CString supported_sitename = lookup->first;
		if (sitename.MakeLower().Find(supported_sitename.MakeLower()) >= 0
			|| network.CompareNoCase(supported_sitename) == 0)
		{
			write_log(Preferences()->debug_pokertracker(), "[PokerTrackerLookup] Site or network is: %s\n",
				supported_sitename);
			write_log(Preferences()->debug_pokertracker(), "[PokerTrackerLookup] SiteID is %i\n",
				lookup->second);
			return lookup->second;
		}
		lookup++;
	}
	write_log(Preferences()->debug_pokertracker(), 
		"[PokerTrackerLookup] ERROR: unsupported sitename and unsupported network.\n");
	write_log(Preferences()->debug_pokertracker(), 
		"[PokerTrackerLookup] Probably wrong data in tablemap.\n");
	return -1;
}