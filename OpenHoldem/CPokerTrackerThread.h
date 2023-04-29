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

#ifndef INC_CPOKERTRACKERTHREAD_H
#define INC_CPOKERTRACKERTHREAD_H

#include "libpq-fe.h"
#include <map>
#include "CSpaceOptimizedGlobalObject.h"


const int k_pt_advanced_stat_update_every    =    5;
const int k_pt_min_hands_for_slower_updates  = 1000;
const int k_pt_min_name_length_to_skip_lev_dist  = 10;

struct PT_SPlayerData 
{
	char			scraped_name[kMaxLengthOfPlayername];
	char			pt_name[kMaxLengthOfPlayername];
	bool			found;
	// Stats are now in the DLL
	//double			stat[k_pt_max_number_of_supported_pokertracker_stats];
	int				skipped_updates;           
};

extern PT_SPlayerData _pt_player_data[kMaxNumberOfPlayers];

class CPokerTrackerThread : public CSpaceOptimizedGlobalObject
{
	friend class CSymbolEnginePokerTracker;
public:
	// public functions
	CPokerTrackerThread();
	~CPokerTrackerThread();
public:
	void				StartThread();
	void				StopThread();
	bool				IsConnected();
	CString				CreateConnectionString(const CString ip_address, 
	const CString port, const CString username,
	const CString password, const CString DB_name);

private:
	// private functions and variables - not available via accessors or mutators
	static void			GetStatsForChair(LPVOID pParam, int chair, int sleepTime);
	static UINT			PokertrackerThreadFunction(LPVOID pParam);
	static bool			LightSleep(int sleepTime, CPokerTrackerThread * pParent);
	void				SetStatGroups();
	bool				AllConnectionDataSpecified();
	void				Connect();
  void        Reconnect(void);
	void				Disconnect();
	bool				NameLooksLikeBadScrape(char *oh_scraped_name);
	bool				CheckIfNameExistsInDB(int chair);
	bool				CheckIfNameHasChanged(int chair);
	double			UpdateStat(const int m_chair, const int stat);
	void				ClearSeatStats(int m_chair, bool clearNameAndFound = true);
	bool				QueryName(const char * query_name, const char * scraped_name, char * best_name);
	bool				FindName(const char *scraped_name, char *best_name);
	bool				UpdateAllStats(int chair);
	int					GetStatGroup(int stat);
	int					SkipUpdateCondition(int stat, int chair);
	bool				SkipUpdateForChair(int chair);
	void				RecalcSkippedUpdates(int chr);
	void				ReportUpdateComplete(int updatedCount, int chair);
	void				SetPlayerName(int chr, bool found, const char* pt_name, const char* scraped_name);
	int					GetSkippedUpdates(int chr){return _pt_player_data[chr].skipped_updates;}
	bool				IsFound(int chair);
	const char* GetPlayerScrapedName(int chair){return _pt_player_data[chair].scraped_name;}

	CString			_conn_str;
	bool				_connected;
	PGconn *		_pgconn;

	HANDLE			_m_stop_thread;
	HANDLE			_m_wait_thread;

	CWinThread	*_pt_thread;

};

extern CPokerTrackerThread *p_pokertracker_thread;

#endif //INC_CPOKERTRACKERTHREAD_H