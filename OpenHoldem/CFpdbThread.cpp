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

#include "StdAfx.h"
#include "CFpdbThread.h"

#include <assert.h>
#include <process.h>
#include <comdef.h>
#include <shlobj.h>
#include <vector>
#include "CAutoConnector.h"
#include "CEngineContainer.h"
#include "CLevDistance.h"
#include "SQLiteWrapper.h"
#include "..\Fpdb_Query_Definitions\fpdb_query_definitions.h"
#include "CFpdbLookUp.h"

#include "CSymbolEngineActiveDealtPlaying.h"
#include "CSymbolEngineAutoplayer.h"
#include "CSymbolEngineIsOmaha.h"
#include "CSymbolEngineIsRush.h"
#include "CSymbolEngineIsTournament.h"
#include "CSymbolEngineFpdb.h"
#include "CSymbolEngineTableLimits.h"
#include "CSymbolEngineTime.h"
#include "CSymbolEngineUserchair.h"
#include "..\CTablemap\CTablemap.h"
#include "CTableState.h"

using namespace std;

CFpdbThread	*p_fpdb_thread = NULL;

FPDB_SPlayerData _fpdb_player_data[kMaxNumberOfPlayers];
SQLiteDB *pSQLite;


CString CFpdbThread::getFpdbDatabasePath() {
	TCHAR szPath[MAX_PATH];
	CString path;
	if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, 0, szPath))) {
		path = szPath;
		path += FPDB_DBPATH;
		if (PathFileExists(path))
			return path;
		else return "";
	}
	else return "";
}


CFpdbThread::CFpdbThread()
{
	pSQLite = new SQLiteDB();

	// Initialize variables
	_path = getFpdbDatabasePath().GetString();
	_fpdb_thread = NULL;
	_connected = false;
	_m_stop_thread = NULL;
	_m_wait_thread = NULL;
}

CFpdbThread::~CFpdbThread()
{
  // !!! Probably not needed, as we call Singleton::StopThreads()
  // when we exit an instance and here happens a freezing
	// StopThread();
}

void CFpdbThread::StartThread()
{
	if (!AllConnectionDataSpecified())
	{
		return;
	}
	if (_fpdb_thread == NULL)
	{
		// Create events
		_m_stop_thread = CreateEvent(0, TRUE, FALSE, 0);
		_m_wait_thread = CreateEvent(0, TRUE, FALSE, 0);

		_fpdb_thread = AfxBeginThread(FpdbThreadFunction, this);

		write_log(Preferences()->debug_fpdb(), "[Fpdb] Started Fpdb-thread.\n");
	}
	else
	{
		write_log(Preferences()->debug_fpdb(), "[Fpdb] Fpdb-thread already running.\n");
	}
}

void CFpdbThread::StopThread()
{
	if (_fpdb_thread)
	{
		// Trigger thread to stop
		::SetEvent(_m_stop_thread);

		// Wait until thread finished
		::WaitForSingleObject(_m_wait_thread, k_max_time_to_wait_for_thread_to_shutdown);

		_fpdb_thread = NULL;

		Disconnect();

		write_log(Preferences()->debug_fpdb(), "[Fpdb] Stopped FPDB thread.\n");
	}

	// Close handles
	if (_m_stop_thread)
	{
		::CloseHandle(_m_stop_thread);
		_m_stop_thread = NULL;
	}
	if (_m_wait_thread)
	{
		::CloseHandle(_m_wait_thread);
		_m_wait_thread = NULL;
	}
}

bool CFpdbThread::AllConnectionDataSpecified()
{
	if (FPDB_DBNAME == "" || _path == "")
		return false;
	return true;
}

void CFpdbThread::Connect(void) {
	write_log(Preferences()->debug_fpdb(), "[Fpdb] Trying to open SQLite3 DB...\n");
	if (!AllConnectionDataSpecified()) 	{
		write_log(Preferences()->debug_fpdb(), "[Fpdb] Can't connect to DB. Vital data or FPDB AppData folder missing\n");
		return;
	}

	//Connect to database
	if (pSQLite->OpenConnection(FPDB_DBNAME, _path))
	{
		write_log(Preferences()->debug_fpdb(), "[Fpdb] SQLite3 DB %s opened successfully\n", FPDB_DBNAME);

		_connected = true;
	}
	else {
		write_log(Preferences()->debug_fpdb(), "[Fpdb] ERROR opening SQLite3 DB: %s\n\n", pSQLite->GetLastError().c_str());
		
		pSQLite->CloseConnection();
		delete pSQLite;
		
		_connected = false;
	}
}

void CFpdbThread::Reconnect(void) {
  // Reconnecting after a lost network connection
  // http://www.maxinmontreal.com/forums/viewtopic.php?f=214&t=19553
  if (!pSQLite->isConnected()) {
	  if (pSQLite->OpenConnection(FPDB_DBNAME, _path))
	  {
        write_log(Preferences()->debug_fpdb(), "[Fpdb] SQLite3 DB reconnected after bad connection\n");
        _connected = true;
      } else {
        write_log(Preferences()->debug_fpdb(), "[Fpdb] ERROR reconnecting to SQLite3 DB: %s\n\n", pSQLite->GetDbStatus());
		pSQLite->CloseConnection();
		delete pSQLite;
        _connected = false;
      }
    }
}

void CFpdbThread::Disconnect(void) {
	if (pSQLite->isConnected()) {
	  pSQLite->CloseConnection();
      write_log(Preferences()->debug_fpdb(), "[Fpdb] Disconnected");
    }
	delete pSQLite;
	_connected = false;
}

bool CFpdbThread::IsConnected() {
  write_log(Preferences()->debug_fpdb(), "[Fpdb] connected: %s\n",
    Bool2CString(_connected));
  write_log(Preferences()->debug_fpdb(), "[Fpdb]PXStatus = %d (0 = CONNECTION_OK)\n",
	  pSQLite->GetDbStatus());
	return (_connected && pSQLite->GetDbStatus() == SQLITE_OK);
}

/* A scraped name is "bad" if it consists only of characters 
   like "l", "1", "i", "." and "," */
bool CFpdbThread::NameLooksLikeBadScrape(char *oh_scraped_name)
{
	int len = (int) strlen(oh_scraped_name);
	for (int i=0; i<len; i++)
	{
		if (oh_scraped_name[i]    != 'l'
			&& oh_scraped_name[i] != '1'
			&& oh_scraped_name[i] != 'i' 
			&& oh_scraped_name[i] != '.' 
			&& oh_scraped_name[i] != ',')
		{
			// Good name, good character found, no bad scrape
			return false;
		}
	}
	// Bad name, only bad characters found, bad scrape
	return true;
}

#define LAST_STATE state[(state_index-1)&0xff]

/* Returns true if found an appropriate name in the DB for chr, or false if 
   it did not found such name.*/
bool CFpdbThread::CheckIfNameExistsInDB(int chair)
{
	char		oh_scraped_name[kMaxLengthOfPlayername]; 
	char		best_name[kMaxLengthOfPlayername];

	assert(chair >= kFirstChair); 
	assert(chair <= kLastChair);
	
	memset(oh_scraped_name, 0, kMaxLengthOfPlayername);
	memset(best_name, 0, kMaxLengthOfPlayername);

	write_log(Preferences()->debug_fpdb(), "[Fpdb] CheckIfNameExistsInDB() chair = %i\n", chair);
	
  assert(p_table_state != NULL);
	if (p_table_state->Player(chair)->name() == "")	{
		write_log(Preferences()->debug_fpdb(), "[Fpdb] CheckIfNameExistsInDB() No name known for this chair\n");
		return false;
	}

	if (p_table_state->Player(chair)->name().GetLength() <= kMaxLengthOfPlayername) 	{
		strcpy_s(oh_scraped_name, kMaxLengthOfPlayername, p_table_state->Player(chair)->name());
	}

	write_log(Preferences()->debug_fpdb(), "[Fpdb] CheckIfNameExistsInDB() Scraped name: [%s]\n", oh_scraped_name);

	if (NameLooksLikeBadScrape(oh_scraped_name))
	{
		write_log(Preferences()->debug_fpdb(), "[Fpdb] CheckIfNameExistsInDB() Name looks like a bad scrape\n");
		return false;
	}
	
	// We already have the name, and it has not changed since we last checked, so do nothing
	if (_fpdb_player_data[chair].found && 0 == strcmp(_fpdb_player_data[chair].scraped_name, oh_scraped_name))
	{
		write_log(Preferences()->debug_fpdb(), "[Fpdb] CheckIfNameExistsInDB() Name is known and good\n");
		return true;
	}
	
	// We have not found the name in FPDB, go find it
	// First see if we can find the exact scraped name
	if (FindName(oh_scraped_name, best_name))
	{
		write_log(Preferences()->debug_fpdb(), "[Fpdb] CheckIfNameExistsInDB() Name found in database\n");
		SetPlayerName(chair, true, best_name, oh_scraped_name);
		return true;
	}
	else
	{
		write_log(Preferences()->debug_fpdb(), "[Fpdb] CheckIfNameExistsInDB() Name not found in database\n");
		SetPlayerName(chair, false, "", "");
		return false;
	}
}

/* Returns true, if the name looks ok, but differs from the last one. */
bool CFpdbThread::CheckIfNameHasChanged(int chair)
{
	char		oh_scraped_name[kMaxLengthOfPlayername]; 
	char		best_name[kMaxLengthOfPlayername];

	assert(chair >= kFirstChair); 
	assert(chair <= kLastChair);
	
	memset(oh_scraped_name, 0, kMaxLengthOfPlayername);
	memset(best_name, 0, kMaxLengthOfPlayername);
	
  //!!!!! bad code, copying to char-array
	if (p_table_state->Player(chair)->name() == "") return false;
	strcpy_s(oh_scraped_name, kMaxLengthOfPlayername, p_table_state->Player(chair)->name());

	if (NameLooksLikeBadScrape(oh_scraped_name))return false;
	// We already have the name, and it has not changed since we last checked, so do nothing
	if (_fpdb_player_data[chair].found 
		  && 0 == strcmp(_fpdb_player_data[chair].scraped_name, oh_scraped_name)) {
		return false;
	}
	write_log(Preferences()->debug_fpdb(), "[Fpdb] Name changed for chair [%d] [%s] -> [%s]\n",
		chair, _fpdb_player_data[chair].scraped_name, oh_scraped_name);
	return true;
}


void CFpdbThread::SetPlayerName(int chr, bool found, const char* fpdb_name, const char* scraped_name)
{
	_fpdb_player_data[chr].found = found;
	bool logResult = false;
	if (0 != memcmp(_fpdb_player_data[chr].fpdb_name, fpdb_name, kMaxLengthOfPlayername) )
	{
		memcpy(_fpdb_player_data[chr].fpdb_name, fpdb_name, kMaxLengthOfPlayername);
		logResult = true;
	}
	if (0 != memcmp(_fpdb_player_data[chr].scraped_name, scraped_name, kMaxLengthOfPlayername) )
	{
		memcpy(_fpdb_player_data[chr].scraped_name, scraped_name, kMaxLengthOfPlayername);
		logResult = true;
	}
	if (logResult)
	{
		write_log(Preferences()->debug_fpdb(), "[Fpdb] SetPlayerName[%d]: Done. fpdbname[%s] scrapedName[%s]\n", chr, _fpdb_player_data[chr].fpdb_name, _fpdb_player_data[chr].scraped_name);
	}
}


//Short Algorithm to query the DB for the best name. returns true if best name is populated.
bool CFpdbThread::FindName(const char *oh_scraped_name, char *best_name)
{

	bool result = QueryName(oh_scraped_name, oh_scraped_name, best_name);

	if (!result) {
		/*  Escalation #1: Exact name not found in DB*/
		/*  Try query with "%" in substitution of ".". So "n.me" becomes "n%me". */
		char pointlessname[kMaxLengthOfPlayername + 1];
		memset(pointlessname, 0, kMaxLengthOfPlayername+1);
		int len = (int)strlen(oh_scraped_name);
		for (int character_position=0; character_position<len; character_position++)
		{
			if (oh_scraped_name[character_position]=='.') {
				pointlessname[character_position]='%';
				} 
			else {
				pointlessname[character_position]=oh_scraped_name[character_position];
				}
		}
		pointlessname[len]='\0';
		result = QueryName(pointlessname, oh_scraped_name, best_name);
	}

	if (!result) {
		/*  Escalation #2:
		/*  Try query with "%n%a%m%e%" . */
		char likename[2 * kMaxLengthOfPlayername + 2];
		// Attention: likename is 2 times as large as a player-name,
		// plus 2 for % at the beginning and \0 at the end.
		// !! There was a buffer-overflow in the past; better get rid of fixed-sized buffers
		memset(likename, 0, 2 * kMaxLengthOfPlayername + 2);
		int len = (int)strlen(oh_scraped_name);
		likename[0]='%';
		for (int character_position=0; character_position<len; character_position++)
		{
			likename[character_position*2+1]=oh_scraped_name[character_position];
			likename[character_position*2+2]='%';
		}

		likename[len*2 + 1]='\0';
		result = QueryName(likename, oh_scraped_name, best_name);
	}

	if (!result)
	{
		/*  Escalation #3: "%n%a%m%e%" not found in DB*/
		/*  Try query "%" to get all names. */
		result = QueryName("%", oh_scraped_name, best_name);
	}
	return result;
}

double CFpdbThread::UpdateStat(int m_chr, int stat)
{
	UINT		res = NULL;
	IResult*	query_result;
	double		result = kUndefined;
	clock_t		updStart, updEnd;
	int			duration;

	int sym_elapsed = p_engine_container->symbol_engine_time()->elapsed();

	//No more unnecessary queries when we don't even have a siteid to check
	int siteid = fpdb_lookup.GetSiteId();
	if (siteid == kUndefined)
		return kUndefined;

	if (!_connected || pSQLite->GetDbStatus() != SQLITE_OK)
		return kUndefined;

	assert(m_chr >= kFirstChair);
	assert(m_chr <= kLastChair);
	assert(stat >= 0);
	assert(stat < FPDB_DLL_GetNumberOfStats());

	// get query string for the requested statistic
  CString query = FPDB_DLL_GetQuery(stat,
    p_engine_container->symbol_engine_isomaha()->isomaha(),
    p_engine_container->symbol_engine_istournament()->istournament(),
    siteid, _fpdb_player_data[m_chr].fpdb_name,
    p_engine_container->symbol_engine_tablelimits()->bblind());

	// Do the query against the FPDB database
	updStart = clock();
	try
	{
		// See if we can find the player name in the database
		write_log(Preferences()->debug_fpdb(), 
			"[Fpdb] Querying %s for m_chr %d: %s\n", 
			FPDB_DLL_GetBasicSymbolNameWithoutFPDBPrefix(stat), 
			m_chr, query);
		res = pSQLite->Execute(query);
		query_result = pSQLite->ExecuteSelect(query);
	}
	catch (_com_error &e)
	{
		write_log(Preferences()->debug_fpdb(), "[Fpdb] ERROR\n");
		write_log(Preferences()->debug_fpdb(), _T("\tCode = %08lx\n"), e.Error());
		write_log(Preferences()->debug_fpdb(), _T("\tCode meaning = %s\n"), e.ErrorMessage());
		_bstr_t bstrSource(e.Source());
		_bstr_t bstrDescrifpdbion(e.Description());
		write_log(Preferences()->debug_fpdb(), _T("\tSource = %s\n"), (LPCTSTR) bstrSource);
		write_log(Preferences()->debug_fpdb(), _T("\tDescrifpdbion = %s\n"), (LPCTSTR) bstrDescrifpdbion);
		write_log(Preferences()->debug_fpdb(), _T("\tQuery = [%s]\n"), query);
	}
	
	updEnd = clock();
	duration = (int) ((double)(updEnd - updStart) / 1000);
	if (duration >= 3)
		write_log(Preferences()->debug_fpdb(), "[Fpdb] Query time in seconds: [%d]\n", duration);

	// Check query return code
	if (res != SQLITE_OK)
	{
		switch (res)
		{
		case SQLITE_DONE:
			write_log(Preferences()->debug_fpdb(), "[Fpdb] SQLITE_DONE: %s [%s]\n", res, query);
			break;
		case SQLITE_EMPTY:
			write_log(Preferences()->debug_fpdb(), "[Fpdb] SQLITE_EMPTY: %s [%s]\n", res, query);
			break;
		case SQLITE_SCHEMA:
			write_log(Preferences()->debug_fpdb(), "[Fpdb] SQLITE_SCHEMA: %s [%s]\n", res, query);
			break;
		case SQLITE_ABORT:
			write_log(Preferences()->debug_fpdb(), "[Fpdb] SQLITE_ABORT: %s [%s]\n", res, query);
			break;
		case SQLITE_AUTH:
			write_log(Preferences()->debug_fpdb(), "[Fpdb] SQLITE_AUTH: %s [%s]\n", res, query);
			break;
		case SQLITE_BUSY:
			write_log(Preferences()->debug_fpdb(), "[Fpdb] SQLITE_BUSY: %s [%s]\n", res, query);
			break;
		case SQLITE_CANTOPEN:
			write_log(Preferences()->debug_fpdb(), "[Fpdb] SQLITE_CANTOPEN: %s [%s]\n", res, query);
			break;
		case SQLITE_LOCKED:
			write_log(Preferences()->debug_fpdb(), "[Fpdb] SQLITE_LOCKED: %s [%s]\n", res, query);
			break;
		case SQLITE_INTERNAL:
			write_log(Preferences()->debug_fpdb(), "[Fpdb] PGRES_COPY_OUT: %s [%s]\n", res, query);
			break;
		case SQLITE_CONSTRAINT:
			write_log(Preferences()->debug_fpdb(), "[Fpdb] SQLITE_CONSTRAINT: %s [%s]\n", res, query);
			break;
		case SQLITE_CORRUPT:
			write_log(Preferences()->debug_fpdb(), "[Fpdb] SQLITE_CORRUPT: %s [%s]\n", res, query);
			break;
		case SQLITE_IOERR:
			write_log(Preferences()->debug_fpdb(), "[Fpdb] SQLITE_IOERR: %s [%s]\n", res, query);
			break;
		case SQLITE_ERROR:
			write_log(Preferences()->debug_fpdb(), "[Fpdb] SQLITE_ERROR: %s [%s]\n", res, query);
			break;
		default:
			write_log(Preferences()->debug_fpdb(), "[Fpdb] GENERIC ERROR: %s [%s]\n", res, query);
			break;
		}
	}
	else
	{
		if (query_result != NULL)
		{
			query_result->Next();
			if (query_result->ColomnData(0)[0] != NULL)
				result = StringToNumber(query_result->ColomnData(0));
			else
				result = kUndefined;
			write_log(Preferences()->debug_fpdb(), 
				"[Fpdb] Query %s for m_chr %d success: %f\n", 
				FPDB_DLL_GetBasicSymbolNameWithoutFPDBPrefix(stat), 
				m_chr, result);
	
			//release Result Data
			query_result->Release();
		}

		// update cache with new values
		FPDB_DLL_SetStat(stat, m_chr, result);
	}

	return result;
}

bool CFpdbThread::QueryName(const char * query_name, const char * scraped_name, char * best_name)
{
	int				lev_dist = 0, bestLD = 0, bestLDindex = 0;
	vector<string>	names;
	bool			result = false;
	CLevDistance	myLD;
	int				siteid = 0;
	double			Levenshtein_distance_matching_factor = 0.3;

	//No more unnecessary queries when we don't even have a siteid to check
	siteid = fpdb_lookup.GetSiteId();
	if (siteid == kUndefined)  return false;

	if (!_connected || pSQLite->GetDbStatus() != SQLITE_OK)
		return false;

	if (0 == strlen(query_name))
		return false;

	CString query;
	query.Format("SELECT name FROM Players WHERE name like '%s' AND siteId=%i",
		query_name, siteid);

	//Get Data From DB
	IResult *res = pSQLite->ExecuteSelect(query);
	if (!res) {
		write_log(Preferences()->debug_fpdb(),
			"[Fpdb] Query player name fails with SQLite error : %s\n",
			pSQLite->GetLastError().c_str());
		result = false;
	}
	else
	{
		//Get Column Count
		int i = res->GetColumnCount();

		//Store Results
		while (res->Next())
		{
			for (int k = 0; k < i; k++)
				names.push_back(res->ColomnData(k));
		}

		//release Result Data
		res->Release();
	}

	// We got nothing, return false
	if (names.size() == 0)
	{
		result = false;
	}

	// If we get one tuple, we still check for Levenshtein distance if the name is short.
	// If the name is long it is probably displayed truncated so Levenshtein distance would be too high.
	if (names.size() == 1)
	{
		const char *found_name = names[0].c_str();
		lev_dist = myLD.LD(scraped_name, found_name);
		if (	strlen(found_name) >= k_fpdb_min_name_length_to_skip_lev_dist
			 || lev_dist <= (int)strlen(found_name) * Levenshtein_distance_matching_factor )
		{
			strncpy_s(best_name, kMaxLengthOfPlayername, found_name, _TRUNCATE);
			// Max length names are possibly truncated so we replace the last character by %.
			if (strlen(best_name)==(kMaxLengthOfPlayername-1)) {
				best_name[(kMaxLengthOfPlayername-2)] = '%';
			}
			result = true;
		}
		else {
			result = false;
		}
	}

	// We got more than one tuple, figure the Levenshtein distance for all of them
	// and return the best
	else if (names.size() > 1)
	{
		bestLD = 999;
		for (UINT i=0; i<names.size(); i++)
		{
			lev_dist = myLD.LD(scraped_name, names[i].c_str());

			if (lev_dist<bestLD && lev_dist<(int)strlen(names[i].c_str())*Levenshtein_distance_matching_factor )
			{
				bestLD = lev_dist;
				bestLDindex = i;
			}
		}
		if (bestLD != 999)
		{
			strncpy_s(best_name, kMaxLengthOfPlayername, names[bestLDindex].c_str(), _TRUNCATE);
			// Max length names are possibly truncated so we replace the last character by %.
			if (strlen(best_name)==(kMaxLengthOfPlayername-1)) {
				best_name[(kMaxLengthOfPlayername-2)] = '%';
			}
			result = true;
		}
		else
		{
			result = false;
		}
	}

	return result;
}


bool CFpdbThread::UpdateAllStats(int chr)
{
	if (_fpdb_player_data[chr].skipped_updates == k_fpdb_advanced_stat_update_every)
	{
		write_log(Preferences()->debug_fpdb(), "[Fpdb] UpdateAllStats: for chair [%d] is true.\n", chr);
		return true;
	}
	write_log(Preferences()->debug_fpdb(), "[Fpdb] UpdateAllStats: for chair [%d] is false (Basic only).\n", chr);
	return false;
}

void CFpdbThread::RecalcSkippedUpdates(int chr)
{
	if (_fpdb_player_data[chr].skipped_updates == k_fpdb_advanced_stat_update_every)
		_fpdb_player_data[chr].skipped_updates = 1;
	else
		++_fpdb_player_data[chr].skipped_updates;
}


int CFpdbThread::SkipUpdateCondition(int stat, int chair)
{
	if (UpdateAllStats(chair))
	{
		return false;
	}
	else if (FPDB_DLL_IsPositionalPreflopStat(stat) 
		|| FPDB_DLL_IsAdvancedStat(stat))
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool CFpdbThread::SkipUpdateForChair(int chair)
{
	int userchair = p_engine_container->symbol_engine_userchair()->userchair();
	bool confirmed = p_engine_container->symbol_engine_userchair()->userchair_confirmed();
	if (userchair == chair && confirmed)
	{
		write_log(Preferences()->debug_fpdb(), "[Fpdb] GetStatsForChair for chair [%d] had been skipped. Reason: [User sits in this chair]\n", chair);
		return true;
	}
	
	int hands = (int)FPDB_DLL_GetStat("fpdb_hands", chair);
	if (hands > k_fpdb_min_hands_for_slower_updates)
	{
		if (UpdateAllStats(chair))
			return false;
		else
		{
			write_log(Preferences()->debug_fpdb(), "[Fpdb] GetStatsForChair for chair [%d] had been skipped. Reason: [User has lots of hands]\n", chair);
			return true;
		}
	}
	return false;
}


void CFpdbThread::GetStatsForChair(LPVOID pParam, int chair, int sleepTime)
{
	CFpdbThread *pParent = static_cast<CFpdbThread*>(pParam);
	int         updatedCount = 0;

  write_log(Preferences()->debug_fpdb(), "[Fpdb] GetStatsForChair %i\n", chair);  
	if (pParent->CheckIfNameExistsInDB(chair) == false)
	{
		/* Note that checkname fail just when starting, doesn't necessarily mean that there's no user
		   in that chair, but only that the scraper failed to find one. This could be due to lobby window
		   that hides poker window behind it. We make this check once, and if we are good, the update iteration
		   is good to go. if we are not, we assume that this seat is not taken at the moment. */ 
		write_log(Preferences()->debug_fpdb(), "[Fpdb] GetStatsForChair[%d] had been skipped. Reason: [CheckName failed]\n", chair);
		return;
	}
	write_log(Preferences()->debug_fpdb(), "[Fpdb] GetStatsForChair[%d] had been started.\n", chair);
	/* Check if there's a complete update cycle skipping for that chair
	if (pParent->SkipUpdateForChair(chair))
	{
		pParent->RecalcSkippedUpdates(chair);
		return;
	}
 */
	if (!pParent->_connected)
	{
		pParent->Connect();
	}
		
	if (pParent->_connected && pSQLite->GetDbStatus() == SQLITE_OK)
	{
    write_log(Preferences()->debug_alltherest(), "[CFpdbThread] location Johnny_7\n");
		if (p_autoconnector->IsConnectedToAnything())
		{
			for (int i=0; i<FPDB_DLL_GetNumberOfStats(); i++)
			{
				/* CheckName is necessary before each update.
				   There's a short interval between any two updates, and it's possible that the player
				   had stood up during the update process. But it also possible that the poker lobby was 
				   hiding our poker window, or some popup temporarily was over it, and that's why CheckName fails.
				   Since we cannot know which one caused checkname to fail, we would continue to update, as 
				   long as we have a found name, and as long as the name did't get changed. 
				   So what we do care about, is the situation were the name got replaced by another name,
				   in that case, we stop the update for the current chair  			   */ 

				if (_fpdb_player_data[chair].found)
				{
					/* Verify therad_stop is false */ 
					if (LightSleep(0, pParent)) 
						return; 
					/* verify that name did not get changed */
					if (pParent->CheckIfNameHasChanged(chair))
					{
						/* Name got changed while we search for stats for current chair
						   Simply return.
						   Clearing stats happens by CSymbolEngineFpdb
						   on next symbol lookup   .
						*/
						write_log(Preferences()->debug_fpdb(), "[Fpdb] Name changed for chair [%d] Stopping FPDB-symbol-lookup. \n", chair);
						return;
					}
					if (pParent->SkipUpdateCondition(i, chair))
					{
						/* Updating stat i should be skipped this time */
						/* advanced/positional stats are updated every k_fpdb_advanced_stat_update_every cycles */
						write_log(Preferences()->debug_fpdb(), "[Fpdb] GetStatsForChair: Updating stats [%d] for chair [%d] had been skipped. Reason: [advanced/positional stats cycle [%d of %d]]\n", i, chair, pParent->GetSkippedUpdates(chair) , k_fpdb_advanced_stat_update_every);
					}
					else
					{
						/* Update... */
						write_log(Preferences()->debug_fpdb(), "[Fpdb] GetStatsForChair updating stats [%d] for chair [%d]...\n", i, chair);
						pParent->UpdateStat(chair, i);
						++updatedCount;
					}
					/* Sleep between two updates (even if skipped) */
					if (LightSleep(sleepTime, pParent)) 
						return;
				}
				else
				{
					/* We couldn't find any user sitting on that chair. Give message*/
					write_log(Preferences()->debug_fpdb(), "[Fpdb] GetStatsForChair for chair [%d] had been skipped. Reason: [user not found (user stood up?)]\n", chair);
					return;
				}
			}
		}
	}
	pParent->ReportUpdateComplete(updatedCount, chair);
	pParent->RecalcSkippedUpdates(chair);
}

void CFpdbThread::ReportUpdateComplete(int updatedCount, int chair)
{
	write_log(Preferences()->debug_fpdb(), "[Fpdb] Updates for chair [%d][%s] had been completed. Total [%d] updated stats\n", chair, _fpdb_player_data[chair].scraped_name, updatedCount);
}

UINT CFpdbThread::FpdbThreadFunction(LPVOID pParam) {
	CFpdbThread *pParent = static_cast<CFpdbThread*>(pParam);
	int				iteration = 0;
	clock_t		iterStart, iterEnd;
	int				iterDurationMS;

	while (::WaitForSingleObject(pParent->_m_stop_thread, 0) != WAIT_OBJECT_0) {
		iterStart = clock();
		write_log(Preferences()->debug_fpdb(), "[Fpdb] FPDBthread iteration [%d] had started\n", ++iteration);
		if (!pParent->_connected)	{
			pParent->Connect();
		} else if (pSQLite->GetDbStatus() != SQLITE_OK) {
      pParent->Reconnect();
    }
    double players = p_engine_container->symbol_engine_active_dealt_playing()->nplayersseated();
		write_log(Preferences()->debug_fpdb(), "[Fpdb] Players count is [%d]\n", (int)players);
		if (players < 2) {
      // It looks like emfpdby tables can cause long sleeping if we continue.
      // Therefore we skip processing if there are too few players
      // http://www.maxinmontreal.com/forums/viewtopic.php?f=111&t=20427
			write_log(Preferences()->debug_fpdb(), "[Fpdb] Not enough players to justify iteration...\n");
			write_log(Preferences()->debug_fpdb(), "[Fpdb] For beginners: possible tablemap-problem?\n");
			write_log(Preferences()->debug_fpdb(), "[Fpdb] Skipping all DB-lookups...\n");
			LightSleep(3000, pParent); pSQLite->GetDbStatus();
      continue;
		}
		// Avoiding division by zero and setting sleep time
		AdaptValueToMinMaxRange(&players, 1, kMaxNumberOfPlayers);
		int sleep_time;
		if (p_engine_container->symbol_engine_isrush()->isrush()) {
			sleep_time = (int) ((double)(5 * 1000) / (double)((FPDB_DLL_GetNumberOfStats() + 1) * players));
    }	else {
			sleep_time = (int) ((double)(30 * 1000) / (double)((FPDB_DLL_GetNumberOfStats() + 1) * players));
    }
		write_log(Preferences()->debug_fpdb(), "[Fpdb] sleepTime set to %d\n", sleep_time);
		LightSleep(sleep_time, pParent);
		if (pParent->_connected && pSQLite->GetDbStatus() == SQLITE_OK)	{
			for (int chair = 0; chair < p_tablemap->nchairs(); ++chair)	{
				GetStatsForChair(pParam, chair, sleep_time);
				/* Verify therad_stop is false */ 
        if (LightSleep(0, pParent)) {
          break;
        }
			}
		}
		iterEnd = clock();
		iterDurationMS = (int) ((double)(iterEnd - iterStart));
		write_log(Preferences()->debug_fpdb(), "[Fpdb] FPDBthread iteration [%d] had ended, duration time in ms: [%d]\n", ++iteration, iterDurationMS);
		if ( (iterDurationMS <= 10000) && (iterDurationMS > 0) ) {
			write_log(Preferences()->debug_fpdb(), "[Fpdb] sleeping [%d] ms because iteration was too quick.\n", 10000 - iterDurationMS);
      if (LightSleep(10000 - iterDurationMS, pParent)) {
        break;
      }
		}
	}
	// Set event
	write_log(Preferences()->debug_fpdb(), "[Fpdb] FpdbThreadFunction: outside while loop...\n");
	::SetEvent(pParent->_m_wait_thread);
	return 0;
}

/*Sleeps but wakes up on stop thread event every 250ms.
We use this function since we never want the thread to ignore the stop_thread event while it's sleeping*/
bool CFpdbThread::LightSleep(int sleepTime, CFpdbThread *pParent) {
  write_log(Preferences()->debug_fpdb(), "[Fpdb] LightSleep: called with sleepTime[%d]\n", sleepTime);
	if (sleepTime > 0)	{
		int sleepSlice = 250 ; // ms
		int slicesNumber = sleepTime / sleepSlice ;
		for (int i = 1; i <= slicesNumber; i++)	{
			Sleep(sleepSlice);
			if (::WaitForSingleObject(pParent->_m_stop_thread, 0) == WAIT_OBJECT_0)	{
				write_log(Preferences()->debug_fpdb(), "[Fpdb] LightSleep: _m_stop_thread signal received\n");
				return true;
			}
		}
		Sleep(sleepTime%sleepSlice);
	}
	if (::WaitForSingleObject(pParent->_m_stop_thread, 0) == WAIT_OBJECT_0)	{
		write_log(Preferences()->debug_fpdb(), "[Fpdb] LightSleep: _m_stop_thread signal received\n");
		return true;
	}
	return false;
}