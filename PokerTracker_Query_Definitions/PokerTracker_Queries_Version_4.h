//******************************************************************************
//
// This file is part of the OpenHoldem project
//   Download page:         http://code.google.com/p/openholdembot/
//   Forums:                http://www.maxinmontreal.com/forums/index.php
//   Licensed under GPL v3: http://www.gnu.org/licenses/gpl.html
//
//******************************************************************************
//
// Purpose:
//
//******************************************************************************

#ifndef INC_POKERTRACKER_QUERIES_VERSION_4_H
#define INC_POKERTRACKER_QUERIES_VERSION_4_H
// GENERAL STATS
// ICON disabled, as not supported bz PT 4.
// https://pt4.pokertracker.com/forums/viewtopic.php?f=59&t=49035
#define PT4_QUERY_SUPPORT__ICON							(FALSE) // "icon"
#define PT4_QUERY_SUPPORT__NB_HANDS						(TRUE)	// "hands"
#define PT4_QUERY_SUPPORT__VPIP							(TRUE)	// "vpip"
#define PT4_QUERY_SUPPORT__PFR							(TRUE)	// "pfr"
#define PT4_QUERY_SUPPORT__RFI							(TRUE)	// "rfi"
#define PT4_QUERY_SUPPORT__WTSD							(TRUE)	// "wtsd"
#define PT4_QUERY_SUPPORT__WSSD							(TRUE)	// "wssd"

// PREFLOP STATS
#define PT4_QUERY_SUPPORT__STEAL_ATTEMPT				(TRUE)	// "steal_attempt"
#define PT4_QUERY_SUPPORT__FOLD_BB_TO_STEAL				(TRUE)	// "bb_fold_to_steal"
#define PT4_QUERY_SUPPORT__FOLD_SB_TO_STEAL				(TRUE)	// "sb_fold_to_steal"
#define PT4_QUERY_SUPPORT__3B_VS_STEAL					(TRUE)	// "3bet_vs_steal"
#define PT4_QUERY_SUPPORT__BB_3B_VS_STEAL				(TRUE)	// "bb_3bet_vs_steal"
#define PT4_QUERY_SUPPORT__SB_3B_VS_STEAL				(TRUE)	// "sb_3bet_vs_steal"
#define PT4_QUERY_SUPPORT__PREFLOP_CALLED_RAISE			(TRUE)	// "preflop_called_raise"
#define PT4_QUERY_SUPPORT__PREFLOP_3B					(TRUE)	// "3bet"
#define PT4_QUERY_SUPPORT__PREFLOP_FOLD_TO_3B			(TRUE)	// "fold_to_3bet"
#define PT4_QUERY_SUPPORT__PF_FOLD_TO_RESTEAL			(TRUE)	// "fold_to_resteal"
#define PT4_QUERY_SUPPORT__PREFLOP_4B					(TRUE)	// "4bet"
#define PT4_QUERY_SUPPORT__PREFLOP_FOLD_TO_4B			(TRUE)	// "fold_to_4bet"

// AF
#define PT4_QUERY_SUPPORT__FLOP_AGGRESSION_FACTOR		(TRUE)	// "flop_af"
#define PT4_QUERY_SUPPORT__TURN_AGGRESSION_FACTOR		(TRUE)	// "turn_af"
#define PT4_QUERY_SUPPORT__RIVER_AGGRESSION_FACTOR		(TRUE)	// "river_af"
#define PT4_QUERY_SUPPORT__TOTAL_AGGRESSION_FACTOR		(TRUE)	// "total_af"

// AFq
#define PT4_QUERY_SUPPORT__FLOP_AGGRESSION_FREQUENCY	(TRUE)	// "flop_afq" 
#define PT4_QUERY_SUPPORT__TURN_AGGRESSION_FREQUENCY	(TRUE)	// "turn_afq" 
#define PT4_QUERY_SUPPORT__RIVER_AGGRESSION_FREQUENCY	(TRUE)	// "river_afq" 
#define PT4_QUERY_SUPPORT__TOTAL_AGGRESSION_FREQUENCY	(TRUE)	// "total_afq" 

// Street seen
#define PT4_QUERY_SUPPORT__SEEN_FLOP_PCT			  	(TRUE)	// "flop_seen"
#define PT4_QUERY_SUPPORT__SEEN_TURN_PCT			  	(TRUE)	// "turn_seen"
#define PT4_QUERY_SUPPORT__SEEN_RIVER_PCT			  	(TRUE)	// "river_seen"

// Continuation bet
// Flop
#define PT4_QUERY_SUPPORT__FLOP_CBET					(TRUE)	// "flop_cbet"
#define PT4_QUERY_SUPPORT__FLOP_FOLD_TO_CBET			(TRUE)	// "flop_fold_to_cbet"
#define PT4_QUERY_SUPPORT__FLOP_RAISE_CBET				(TRUE)	// "flop_raise_cbet"
// Turn
#define PT4_QUERY_SUPPORT__TURN_CBET					(TRUE)	// "turn_cbet"
#define PT4_QUERY_SUPPORT__TURN_FOLD_TO_CBET			(TRUE)	// "turn_fold_to_cbet"
// River
#define PT4_QUERY_SUPPORT__RIVER_FOLD_TO_CBET			(TRUE)	// "river_fold_to_cbet"

// POSTFLOP Other stats
#define PT4_QUERY_SUPPORT__POSTFLOP_FOLD_TO_3B			(TRUE)	// "postflop_fold_to_3bet"

// FLOP Other stats
#define PT4_QUERY_SUPPORT__FLOP_FOLD_TO_3B				(FALSE)	// "flop_fold_to_3bet"   // 3b is a reraise, we should have a "flop_fold_to_raise" stat first.
#define PT4_QUERY_SUPPORT__FLOP_CHECK_RAISE				(TRUE)	// "flop_checkraise"
#define PT4_QUERY_SUPPORT__FLOP_DONKBET					(TRUE)	// "flop_donkbet"
#define PT4_QUERY_SUPPORT__FLOP_FLOAT     				(TRUE)	// "flop_float" (PT's stat for: bet IP flop vs missed cbet)

// TURN Other stats
#define PT4_QUERY_SUPPORT__TURN_FOLD_TO_3B				(FALSE)	// "turn_fold_to_3bet" // 3b is a reraise, we should have a "turn_fold_to_raise" stat first.
#define PT4_QUERY_SUPPORT__TURN_CHECK_RAISE				(TRUE)	// "turn_checkraise"
#define PT4_QUERY_SUPPORT__TURN_CHECK_CALL				(TRUE)	// "turn_checkcall"

// RIVER Other stats
#define PT4_QUERY_SUPPORT__RIVER_FOLD_TO_3B				(FALSE)	// "river_fold_to_3bet" // 3b is a reraise, we should have a "river_fold_to_raise" stat first.
#define PT4_QUERY_SUPPORT__RIVER_BET     				(TRUE)	// "river_bet"

// GENERAL by position
#define PT4_QUERY_SUPPORT__SB_RFI						(TRUE)	// "sb_rfi"
#define PT4_QUERY_SUPPORT__BTN_RFI						(TRUE)	// "btn_rfi"
#define PT4_QUERY_SUPPORT__CO_RFI						(TRUE)	// "co_rfi"
#define PT4_QUERY_SUPPORT__MP3_RFI						(TRUE)	// "mp_rfi"
#define PT4_QUERY_SUPPORT__MP2_RFI						(TRUE)	// "ep_rfi"


const int k_number_of_pokertracker_stats =  //GENERAL STATS
											(PT4_QUERY_SUPPORT__ICON ? 1 : 0) +
											(PT4_QUERY_SUPPORT__NB_HANDS ? 1 : 0) + 
											(PT4_QUERY_SUPPORT__VPIP ? 1 : 0) + 
											(PT4_QUERY_SUPPORT__PFR ? 1 : 0) + 
											(PT4_QUERY_SUPPORT__RFI ? 1 : 0) + 
											(PT4_QUERY_SUPPORT__WTSD ? 1 : 0) + 
											(PT4_QUERY_SUPPORT__WSSD ? 1 : 0) +

											//PREFLOP STATS
											(PT4_QUERY_SUPPORT__STEAL_ATTEMPT ? 1 : 0) + 
											(PT4_QUERY_SUPPORT__FOLD_BB_TO_STEAL ? 1 : 0) + 
											(PT4_QUERY_SUPPORT__FOLD_SB_TO_STEAL ? 1 : 0) + 
											(PT4_QUERY_SUPPORT__3B_VS_STEAL ? 1 : 0) + 
											(PT4_QUERY_SUPPORT__BB_3B_VS_STEAL ? 1 : 0) + 
											(PT4_QUERY_SUPPORT__SB_3B_VS_STEAL ? 1 : 0) + 
											(PT4_QUERY_SUPPORT__PREFLOP_CALLED_RAISE ? 1 : 0) + 
											(PT4_QUERY_SUPPORT__PREFLOP_3B ? 1 : 0) + 
											(PT4_QUERY_SUPPORT__PREFLOP_FOLD_TO_3B ? 1 : 0) + 
											(PT4_QUERY_SUPPORT__PF_FOLD_TO_RESTEAL ? 1 : 0) + 
											(PT4_QUERY_SUPPORT__PREFLOP_4B ? 1 : 0) + 
											(PT4_QUERY_SUPPORT__PREFLOP_FOLD_TO_4B ? 1 : 0) + 

											//  AF
											(PT4_QUERY_SUPPORT__FLOP_AGGRESSION_FACTOR ? 1 : 0) + 
											(PT4_QUERY_SUPPORT__TURN_AGGRESSION_FACTOR ? 1 : 0) + 
											(PT4_QUERY_SUPPORT__RIVER_AGGRESSION_FACTOR ? 1 : 0) + 
											(PT4_QUERY_SUPPORT__TOTAL_AGGRESSION_FACTOR ? 1 : 0) +

											//  AFq
											(PT4_QUERY_SUPPORT__FLOP_AGGRESSION_FREQUENCY ? 1 : 0) + 
											(PT4_QUERY_SUPPORT__TURN_AGGRESSION_FREQUENCY ? 1 : 0) + 
											(PT4_QUERY_SUPPORT__RIVER_AGGRESSION_FREQUENCY ? 1 : 0) + 
											(PT4_QUERY_SUPPORT__TOTAL_AGGRESSION_FREQUENCY ? 1 : 0) +

											// POSTFLOP stats
											(PT4_QUERY_SUPPORT__POSTFLOP_FOLD_TO_3B ? 1 : 0) +

											//  Street seen
											(PT4_QUERY_SUPPORT__SEEN_FLOP_PCT ? 1 : 0) + 
											(PT4_QUERY_SUPPORT__SEEN_TURN_PCT ? 1 : 0) + 
											(PT4_QUERY_SUPPORT__SEEN_RIVER_PCT ? 1 : 0) +

											//  Continuation bet
											//  Flop
											(PT4_QUERY_SUPPORT__FLOP_CBET ? 1 : 0) + 
											(PT4_QUERY_SUPPORT__FLOP_FOLD_TO_CBET ? 1 : 0) + 
											(PT4_QUERY_SUPPORT__FLOP_RAISE_CBET ? 1 : 0) + 
											//  Turn
											(PT4_QUERY_SUPPORT__TURN_CBET ? 1 : 0) + 
											(PT4_QUERY_SUPPORT__TURN_FOLD_TO_CBET ? 1 : 0) + 
											//  River
											(PT4_QUERY_SUPPORT__RIVER_FOLD_TO_CBET ? 1 : 0)+

											//  Flop Other stats
											(PT4_QUERY_SUPPORT__FLOP_FOLD_TO_3B ? 1 : 0) + 
											(PT4_QUERY_SUPPORT__FLOP_CHECK_RAISE ? 1 : 0) + 
											(PT4_QUERY_SUPPORT__FLOP_DONKBET ? 1 : 0) + 
											(PT4_QUERY_SUPPORT__FLOP_FLOAT ? 1 : 0) +

											//  Turn Other stats
											(PT4_QUERY_SUPPORT__TURN_FOLD_TO_3B ? 1 : 0) + 
											(PT4_QUERY_SUPPORT__TURN_CHECK_RAISE ? 1 : 0) + 
											(PT4_QUERY_SUPPORT__TURN_CHECK_CALL ? 1 : 0) + 

											//  River Other stats
											(PT4_QUERY_SUPPORT__RIVER_FOLD_TO_3B ? 1 : 0) + 
											(PT4_QUERY_SUPPORT__RIVER_BET ? 1 : 0) +
											(PT4_QUERY_SUPPORT__SB_RFI ? 1 : 0) +
											(PT4_QUERY_SUPPORT__BTN_RFI ? 1 : 0) +
											(PT4_QUERY_SUPPORT__CO_RFI ? 1 : 0) +
											(PT4_QUERY_SUPPORT__MP3_RFI ? 1 : 0) +
											(PT4_QUERY_SUPPORT__MP2_RFI ? 1 : 0);


// PokerTracker support
enum PT_Groups
{
	pt_group_basic = 1,
	pt_group_advanced = 2,
	pt_group_positional = 3
};

enum PT_UpdateTypes
{
	pt_updateType_noUpdate = 0,
	pt_updateType_updateBasic = 1,
	pt_updateType_updateAll = 2
};

enum PT_StatTypes
{
	pt_statType_Ring = 0,
	pt_statType_Tourney = 1
};

typedef struct t_QueryDefinition
{
	CString name;
	CString description_for_editor;
	CString query;
	int stat_group;
};

// PokerTracker naming conventions: 
//   http://www.pokertracker.com/guides/PT3/tutorials/using-custom-reports-and-statistics#Naming-conventions

t_QueryDefinition query_definitions[k_number_of_pokertracker_stats] =
{

#if PT4_QUERY_SUPPORT__ICON
	/* PT4 query to get icon */
	{
		// name
		"icon",
		// description_for_editor
		"Poker Tracker auto-rate icon code",
		// query
		"SELECT val_icon as result \
		 FROM   player \
		 WHERE  id_site = %SITEID% AND \
		        player_name LIKE '%SCREENNAME%'",
		// stat_group
		pt_group_basic
	},
#endif

#if PT4_QUERY_SUPPORT__NB_HANDS
	/* PT4 query to get number of hands in the database */
	{
		// name
		"hands",	
		// description_for_editor
		"Poker Tracker number of hands that are in the database",
		// query
		"SELECT count(*) as result \
		 FROM   player as P, %TYPE%_hand_player_statistics as S \
		 WHERE  S.id_player = P.id_player AND \
				S.id_gametype = %GAMETYPE% AND \
		        P.id_site = %SITEID% AND \
		        P.player_name LIKE '%SCREENNAME%'",
		// stat_group
		pt_group_basic
	},
#endif

#if PT4_QUERY_SUPPORT__VPIP
	/* PT4 query to get vpip */
	{
		// name
		"vpip",			
		// description_for_editor
		"Poker Tracker VP$IP",
		// query
		"SELECT (case	when ActionOpportunities = 0 then -1 \
				 else cast(ActionCount as real) / ActionOpportunities \
				 end) as result \
		 FROM	(SELECT	sum(case when flg_vpip then 1 else 0 end) \
						 as ActionCount, \
						count(*) \
						 as ActionOpportunities \
				 FROM	player as P, %TYPE%_hand_player_statistics as S \
				 WHERE	S.id_player = P.id_player AND \
						S.id_gametype = %GAMETYPE% AND \
						P.id_site = %SITEID% AND \
						P.player_name LIKE '%SCREENNAME%') foo",
		// stat_group
		pt_group_basic
	},
#endif

#if PT4_QUERY_SUPPORT__PFR
	/* PT4 query to get pre flop raise */
	{
		// name
		"pfr",			
		// description_for_editor
		"Poker Tracker pre-flop raise ",
		// query
		"SELECT (case when ActionOpportunities = 0 then -1 \
				 else cast(ActionCount as real) / ActionOpportunities \
				 end) as result \
		 FROM	(SELECT	sum(case when cnt_p_raise > 0 then 1 else 0 end) \
						 as ActionCount, \
						count(*) \
						 as ActionOpportunities \
				 FROM   player as P, %TYPE%_hand_player_statistics as S \
				 WHERE  S.id_player = P.id_player AND \
						S.id_gametype = %GAMETYPE% AND \
						P.id_site = %SITEID% AND \
						P.player_name LIKE '%SCREENNAME%') foo",
		// stat_group
		pt_group_basic
	},
#endif

#if PT4_QUERY_SUPPORT__RFI
	/* PT4 query to get pre-flop raise first in*/
	{
		// name
		"rfi",		
		// description_for_editor
		"Poker Tracker pre-flop raise first in",
		// query
		"SELECT (case when ActionOpportunities = 0 then -1 \
				 else cast(ActionCount as real) / ActionOpportunities \
				 end) as result \
		 FROM	(SELECT	sum(case when(s.flg_p_first_raise AND s.flg_p_open_opp) then  1 else  0 end) \
						 as ActionCount, \
						sum((case when(s.flg_p_open_opp) then  1 else  0 end)) \
						 as ActionOpportunities \
				 FROM	player as P, %TYPE%_hand_player_statistics as S \
				 WHERE	S.id_player = P.id_player AND \
						S.id_gametype = %GAMETYPE% AND \
						P.id_site = %SITEID% AND \
						P.player_name LIKE '%SCREENNAME%') foo",
		// stat_group
		pt_group_basic
	},
#endif

#if PT4_QUERY_SUPPORT__WTSD
	/* PT4 query to get Went to SD */
	{
		// name
		"wtsd",			
		// description_for_editor
		"Poker Tracker went to showdown",
		// query
		"SELECT (case when ActionOpportunities = 0 then -1 \
				 else cast(ActionCount as real) / ActionOpportunities \
				 end) as result \
		 FROM	(SELECT	sum(case when flg_showdown then 1 else 0 end) \
						 as ActionCount, \
						sum(case when flg_f_saw then 1 else 0 end) \
						 as ActionOpportunities \
				 FROM	player as P, %TYPE%_hand_player_statistics as S \
				 WHERE	S.id_player = P.id_player AND \
						S.id_gametype = %GAMETYPE% AND \
						P.id_site = %SITEID% AND \
						P.player_name LIKE '%SCREENNAME%') foo",
		// stat_group
		pt_group_basic
	},
#endif

#if PT4_QUERY_SUPPORT__WSSD
	/* PT4 query to get Won $ At SD */
	{
		// name
		"wssd",			
		// description_for_editor
		"Poker Tracker won $ at showdown",
		// query
		"SELECT (case when ActionOpportunities = 0 then -1 \
				 else cast(ActionCount as real) / ActionOpportunities \
				 end) as result \
		 FROM	(SELECT	sum (case when (s.flg_showdown AND s.flg_won_hand) then  1 else  0 end) \
						 as ActionCount, \
						sum((case when(s.flg_showdown) then  1 else  0 end)) \
						 as ActionOpportunities \
				 FROM	player as P, %TYPE%_hand_player_statistics as S \
				 WHERE	S.id_player = P.id_player AND \
						S.id_gametype = %GAMETYPE% AND \
						P.id_site = %SITEID% AND \
						P.player_name LIKE '%SCREENNAME%') foo",
		// stat_group
		pt_group_basic
	},
#endif

#if PT4_QUERY_SUPPORT__STEAL_ATTEMPT
	/* PT4 query to get attempted to steal */
	{
		// name
		"steal_attempt",	
		// description_for_editor
		"Poker Tracker attempt to steal blinds",
		// query
		"SELECT (case when ActionOpportunities = 0 then -1 \
				 else cast(ActionCount as real) / ActionOpportunities \
				 end) as result \
		 FROM	(SELECT	sum(case when flg_steal_att then 1 else 0 end) \
						 as ActionCount, \
						sum(case when flg_steal_opp then 1 else 0 end) \
						 as ActionOpportunities \
				 FROM	player as P, %TYPE%_hand_player_statistics as S \
				 WHERE	S.id_player = P.id_player AND \
						S.id_gametype = %GAMETYPE% AND \
						P.id_site = %SITEID% AND \
						P.player_name LIKE '%SCREENNAME%') foo",
		// stat_group
		pt_group_advanced
	},
#endif

#if PT4_QUERY_SUPPORT__FOLD_BB_TO_STEAL
	/* PT4 query to get Folded Big Blind to steal */
	{
		// name
		"bb_fold_to_steal",		
		// description_for_editor
		"Poker Tracker folded big blind to steal",
		// query
		"SELECT (case when ActionOpportunities = 0 then -1 \
				 else cast(ActionCount as real) / ActionOpportunities \
				 end) as result \
		 FROM	(SELECT	sum(case when flg_bb_steal_fold then 1 else 0 end) \
						 as ActionCount, \
						sum(case when flg_blind_def_opp AND flg_blind_b then 1 else 0 end) \
						 as ActionOpportunities \
				 FROM	player as P, %TYPE%_hand_player_statistics as S \
				 WHERE	S.id_player = P.id_player AND \
						S.id_gametype = %GAMETYPE% AND \
						P.id_site = %SITEID% AND \
						P.player_name LIKE '%SCREENNAME%') foo",
		// stat_group
		pt_group_positional
	},
#endif

#if PT4_QUERY_SUPPORT__FOLD_SB_TO_STEAL
	/* PT4 query to get Folded Small Blind to steal */
	{
		// name
		"sb_fold_to_steal",
		// description_for_editor
		"Poker Tracker folded small blind to steal",
		// query
		"SELECT (case when ActionOpportunities = 0 then -1 \
				 else cast(ActionCount as real) / ActionOpportunities \
				 end) as result \
		 FROM	(SELECT	sum(case when flg_sb_steal_fold then 1 else 0 end) \
						 as ActionCount, \
						sum(case when flg_blind_def_opp AND flg_blind_s then 1 else 0 end) \
						 as ActionOpportunities \
				 FROM	player as P, %TYPE%_hand_player_statistics as S \
				 WHERE	S.id_player = P.id_player AND \
						S.id_gametype = %GAMETYPE% AND \
						P.id_site = %SITEID% AND \
						P.player_name LIKE '%SCREENNAME%') foo",
		// stat_group
		pt_group_positional
	},
#endif

#if PT4_QUERY_SUPPORT__3B_VS_STEAL
	/* PT4 query to get 3B vs. steal */
	{
		// name
		"3bet_vs_steal",
		// description_for_editor
		"Poker Tracker 3bet vs. steal",
		// query
		"SELECT (case when ActionOpportunities = 0 then -1 \
				 else cast(ActionCount as real) / ActionOpportunities \
				 end) as result \
		 FROM	(SELECT	sum(case when (flg_blind_def_opp AND flg_p_3bet) then 1 else 0 end) \
						 as ActionCount, \
						sum((case when (flg_blind_def_opp AND flg_p_3bet_opp) then  1 else  0 end)) \
						 as ActionOpportunities \
				 FROM	player as P, %TYPE%_hand_player_statistics as S \
				 WHERE	S.id_player = P.id_player AND \
						S.id_gametype = %GAMETYPE% AND \
						P.id_site = %SITEID% AND \
						P.player_name LIKE '%SCREENNAME%') foo",
		// stat_group
		pt_group_positional
	},
#endif

#if PT4_QUERY_SUPPORT__BB_3B_VS_STEAL
	/* PT4 query to get BB 3B vs. steal */
	{
		// name
		"bb_3bet_vs_steal",
		// description_for_editor
		"Poker Tracker BB 3bet vs. steal",
		// query
		"SELECT (case when ActionOpportunities = 0 then -1 \
				 else cast(ActionCount as real) / ActionOpportunities \
				 end) as result \
		 FROM	(SELECT	sum(case when ((flg_blind_b=TRUE) AND (flg_blind_def_opp=TRUE) AND (cnt_p_raise>0)) then 1 else 0 end) \
						 as ActionCount, \
						sum(case when ((flg_blind_b=TRUE) AND (flg_blind_def_opp=TRUE)) then 1 else 0 end) \
						 as ActionOpportunities \
				 FROM	player as P, %TYPE%_hand_player_statistics as S \
				 WHERE	S.id_player = P.id_player AND \
						S.id_gametype = %GAMETYPE% AND \
						P.id_site = %SITEID% AND \
						P.player_name LIKE '%SCREENNAME%') foo",
      // stat_group
      pt_group_positional
	},
#endif

#if PT4_QUERY_SUPPORT__SB_3B_VS_STEAL
	/* PT4 query to get SB 3B vs. steal */
	{
		// name
		"sb_3bet_vs_steal",
		// description_for_editor
		"Poker Tracker SB 3bet vs. steal",
		// query
		"SELECT (case when ActionOpportunities = 0 then -1 \
				 else cast(ActionCount as real) / ActionOpportunities \
				 end) as result \
		 FROM	(SELECT	sum(case when ((flg_blind_s=TRUE) AND (flg_blind_def_opp=TRUE) AND (cnt_p_raise>0)) then 1 else 0 end) \
						 as ActionCount, \
						sum(case when ((flg_blind_s=TRUE) AND (flg_blind_def_opp=TRUE)) then 1 else 0 end) \
						 as ActionOpportunities \
				 FROM	player as P, %TYPE%_hand_player_statistics as S \
				 WHERE	S.id_player = P.id_player AND \
						S.id_gametype = %GAMETYPE% AND \
						P.id_site = %SITEID% AND \
						P.player_name LIKE '%SCREENNAME%') foo",
		// stat_group
		pt_group_positional
	},
#endif

#if PT4_QUERY_SUPPORT__PREFLOP_CALLED_RAISE
	/* PT4 query to get pre-flop called raise */
	{
		// name
		"preflop_called_raise",		
		// description_for_editor
		"Poker Tracker pre-flop called raise",
		// query
		"SELECT (case when ActionOpportunities = 0 then -1 \
				 else cast(ActionCount as real) / ActionOpportunities \
				 end) as result \
		 FROM	(SELECT	sum(case when ((s.amt_p_2bet_facing > 0 AND NOT(s.flg_p_limp) AND NOT s.flg_p_3bet_def_opp AND NOT flg_p_4bet_def_opp AND cnt_p_call>0) or \
						(s.enum_p_3bet_action='C') or (s.enum_p_4bet_action='C')) then 1 else 0 end) \
						 as ActionCount, \
						sum(case when s.flg_p_3bet_def_opp or s.flg_p_4bet_def_opp or s.amt_p_2bet_facing > 0 then 1 else 0 end) \
						 as ActionOpportunities \
				 FROM	player as P, %TYPE%_hand_player_statistics as S \
				 WHERE	S.id_player = P.id_player AND \
				 		S.flg_p_face_raise AND \
						S.id_gametype = %GAMETYPE% AND \
						P.id_site = %SITEID% AND \
						P.player_name LIKE '%SCREENNAME%') foo",
		// stat_group
		pt_group_advanced
	},
#endif

#if PT4_QUERY_SUPPORT__PREFLOP_3B
	/* PT4  query to get PREFLOP 3 bet */
	{
		// name
		"3bet",	
		// description_for_editor
		"Poker Tracker 3bet preflop",
		// query
		"SELECT (case when ActionOpportunities = 0 then -1 \
				 else cast(ActionCount as real) / ActionOpportunities \
				 end) as result \
		 FROM	(SELECT	sum(case when flg_p_3bet then 1 else 0 end) \
						 as ActionCount, \
						sum(case when flg_p_3bet_opp then 1 else 0 end) \
						 as ActionOpportunities \
				 FROM	player as P, %TYPE%_hand_player_statistics as S \
				 WHERE	S.id_player = P.id_player AND \
						S.id_gametype = %GAMETYPE% AND \
						P.id_site = %SITEID% AND \
						P.player_name LIKE '%SCREENNAME%') foo",
		// stat_group
		pt_group_advanced
	},
#endif

#if PT4_QUERY_SUPPORT__PREFLOP_FOLD_TO_3B
	/* PT4  query to get PREFLOP fold to 3 bet */
	{
		// name
		"fold_to_3bet",	
		// description_for_editor
		"Poker Tracker folded while facing 3bet preflop",
		// query
		"SELECT (case when ActionOpportunities = 0 then -1 \
				 else cast(ActionCount as real) / ActionOpportunities \
				 end) as result \
		 FROM	(SELECT	sum(case when(s.enum_p_3bet_action='F' AND s.flg_p_first_raise) then  1 else  0 end) \
						 as ActionCount, \
						sum(case when(s.flg_p_3bet_def_opp AND s.flg_p_first_raise) then  1 else  0 end) \
						 as ActionOpportunities \
				 FROM	player as P, %TYPE%_hand_player_statistics as S \
				 WHERE	S.id_player = P.id_player AND \
						S.id_gametype = %GAMETYPE% AND \
						P.id_site = %SITEID% AND \
						P.player_name LIKE '%SCREENNAME%') foo",
		// stat_group
		pt_group_advanced
	},
#endif

#if PT4_QUERY_SUPPORT__PF_FOLD_TO_RESTEAL
	/* PT4 query to get Fold to Resteal */
	{
		// name
		"fold_to_resteal",
		// description_for_editor
		"Poker Tracker Fold to 3bet after Stealing",
		// query
		"SELECT (case when ActionOpportunities = 0 then -1 \
				 else cast(ActionCount as real) / ActionOpportunities \
				 end) as result \
		 FROM	(SELECT	sum(case when(s.flg_steal_att AND s.flg_p_3bet_def_opp AND s.enum_p_3bet_action='F') then  1 else  0 end) \
						 as ActionCount, \
						sum((case when(s.flg_steal_att AND s.flg_p_3bet_def_opp ) then  1 else  0 end)) \
						 as ActionOpportunities \
				 FROM	player as P, %TYPE%_hand_player_statistics as S \
				 WHERE	S.id_player = P.id_player AND \
						S.id_gametype = %GAMETYPE% AND \
						P.id_site = %SITEID% AND \
						P.player_name LIKE '%SCREENNAME%') foo",
		// stat_group
		pt_group_positional
	},
#endif

#if PT4_QUERY_SUPPORT__PREFLOP_4B
	/* PT4 query to get Preflop 4B */
	{
		// name
		"4bet",
		// description_for_editor
		"Poker Tracker Preflop 4B",
		// query
		"SELECT (case when ActionOpportunities = 0 then -1 \
				 else cast(ActionCount as real) / ActionOpportunities \
				 end) as result \
		 FROM	(SELECT	sum(case when flg_p_4bet then 1 else 0 end) \
						 as ActionCount, \
						sum(case when flg_p_4bet_opp then 1 else 0 end) \
						 as ActionOpportunities \
				 FROM	player as P, %TYPE%_hand_player_statistics as S \
				 WHERE	S.id_player = P.id_player AND \
						S.id_gametype = %GAMETYPE% AND \
						P.id_site = %SITEID% AND \
						P.player_name LIKE '%SCREENNAME%') foo",
		// stat_group
		pt_group_advanced
	},
#endif

#if PT4_QUERY_SUPPORT__PREFLOP_FOLD_TO_4B
	/* PT4 query to get Preflop fold to 4B */
	{
		// name
		"fold_to_4bet",
		// description_for_editor
		"Poker Tracker Preflop fold to 4bet",
		// query
		"SELECT (case when ActionOpportunities = 0 then -1 \
				 else cast(ActionCount as real) / ActionOpportunities \
				 end) as result \
		 FROM	(SELECT	sum(case when (enum_p_4bet_action='F') then 1 else 0 end) \
						 as ActionCount, \
						sum(case when flg_p_4bet_def_opp then 1 else 0 end) \
						 as ActionOpportunities \
				 FROM	player as P, %TYPE%_hand_player_statistics as S \
				 WHERE	S.id_player = P.id_player AND \
						S.id_gametype = %GAMETYPE% AND \
						P.id_site = %SITEID% AND \
						P.player_name LIKE '%SCREENNAME%') foo",
		// stat_group
		pt_group_positional
	},
#endif

#if PT4_QUERY_SUPPORT__FLOP_AGGRESSION_FACTOR
	/* PT4 query to get flop aggression factor */
	{
		// name
		"flop_af",
		// description_for_editor
		"Poker Tracker flop aggression",
		// query
		"SELECT (case when CallsCount = 0 then -1 \
				 else cast(AggressionCount as real) / CallsCount \
				 end) as result \
		 FROM	(SELECT	sum(S.cnt_f_raise) + sum(case when S.flg_f_bet then 1 else 0 end) \
						 as AggressionCount, \
						sum(S.cnt_f_call) \
						 as CallsCount \
				 FROM	player as P, %TYPE%_hand_player_statistics as S \
				 WHERE	S.id_player = P.id_player AND \
						S.id_gametype = %GAMETYPE% AND \
						P.id_site = %SITEID% AND \
						P.player_name LIKE '%SCREENNAME%') foo",
		// stat_group
		pt_group_advanced
	},
#endif

#if PT4_QUERY_SUPPORT__TURN_AGGRESSION_FACTOR
	/* PT4 query to get turn aggression factor */
	{
		// name
		"turn_af",
		// description_for_editor
		"Poker Tracker turn aggression",
		// query
		"SELECT (case when CallsCount = 0 then -1 \
				 else cast(AggressionCount as real) / CallsCount \
				 end) as result \
		 FROM	(SELECT	sum(S.cnt_t_raise) + sum(case when S.flg_t_bet then 1 else 0 end) \
						 as AggressionCount, \
						sum(S.cnt_t_call) \
						 as CallsCount \
				 FROM	player as P, %TYPE%_hand_player_statistics as S \
				 WHERE	S.id_player = P.id_player AND \
						S.id_gametype = %GAMETYPE% AND \
						P.id_site = %SITEID% AND \
						P.player_name LIKE '%SCREENNAME%') foo",
		// stat_group
		pt_group_advanced
	},
#endif

#if PT4_QUERY_SUPPORT__RIVER_AGGRESSION_FACTOR
	/* PT4 query to get river aggression factor */
	{
		// name
		"river_af",			
		// description_for_editor
		"Poker Tracker river aggression",
		// query
		"SELECT (case when CallsCount = 0 then -1 \
				 else cast(AggressionCount as real) / CallsCount \
				 end) as result \
		 FROM	(SELECT	sum(S.cnt_r_raise) + sum(case when S.flg_r_bet then 1 else 0 end) \
						 as AggressionCount, \
						sum(S.cnt_r_call) \
						 as CallsCount \
				 FROM	player as P, %TYPE%_hand_player_statistics as S \
				 WHERE	S.id_player = P.id_player AND \
						S.id_gametype = %GAMETYPE% AND \
						P.id_site = %SITEID% AND \
						P.player_name LIKE '%SCREENNAME%') foo",
		// stat_group
		pt_group_advanced
	},
#endif

#if PT4_QUERY_SUPPORT__TOTAL_AGGRESSION_FACTOR
	/* PT4 query to get total aggression factor without preflop */
	{
		// name
		"total_af",	
		// description_for_editor
		"Poker Tracker total aggression excluding preflop",
		// query
		"SELECT (case when CallsCount = 0 then -1 \
				else cast(AggressionCount as real) / CallsCount \
				end) as result \
		 FROM (SELECT	 sum(cnt_f_raise) + sum(case when flg_f_bet then 1 else 0 end) \
					   + sum(cnt_t_raise) + sum(case when flg_t_bet then 1 else 0 end) \
                       + sum(cnt_r_raise) + sum(case when flg_r_bet then 1 else 0 end) \
						  as AggressionCount, \
						 sum(cnt_f_call) \
                       + sum(cnt_t_call) \
                       + sum(cnt_r_call) \
						  as CallsCount \
			   FROM   player as P, %TYPE%_hand_player_statistics as S \
			   WHERE  S.id_player = P.id_player AND \
					  S.id_gametype = %GAMETYPE% AND \
		              P.id_site = %SITEID% AND \
		              P.player_name LIKE '%SCREENNAME%') foo",
		// stat_group
		pt_group_advanced
	},
#endif

#if PT4_QUERY_SUPPORT__FLOP_AGGRESSION_FREQUENCY
	/* PT4 query to get flop aggression frequency */
	{
		// name
		"flop_afq",
		// description_for_editor
		"Poker Tracker flop aggression frequency",
		// query
		"SELECT (case when (AggressionCount + PassiveCount) = 0 then -1 \
				else cast(AggressionCount as real) / (AggressionCount + PassiveCount) \
				end) as result \
		 FROM (SELECT	sum(cnt_f_raise) + sum(case when flg_f_bet then 1 else 0 end) \
						 as AggressionCount, \
						sum(cnt_f_call) + sum(case when flg_f_fold then 1 else 0 end) \
						 as PassiveCount \
			   FROM   player as P, %TYPE%_hand_player_statistics as S \
			   WHERE  S.id_player = P.id_player AND \
					  S.id_gametype = %GAMETYPE% AND \
		              P.id_site = %SITEID% AND \
		              P.player_name LIKE '%SCREENNAME%') AS foo",
		// stat_group
		pt_group_advanced
	},
#endif

#if PT4_QUERY_SUPPORT__TURN_AGGRESSION_FREQUENCY
	/* PT4 query to get turn aggression frequency */
	{
		// name
		"turn_afq",
		// description_for_editor
		"Poker Tracker turn aggression frequency",
		// query
		"SELECT (case when (AggressionCount + PassiveCount) = 0 then -1 \
				else cast(AggressionCount as real) / (AggressionCount + PassiveCount) \
				end) as result \
		 FROM (SELECT	sum(cnt_t_raise) + sum(case when flg_t_bet then 1 else 0 end) \
						 as AggressionCount, \
						sum(cnt_t_call) + sum(case when flg_t_fold then 1 else 0 end) \
						 as PassiveCount \
			   FROM   player as P, %TYPE%_hand_player_statistics as S \
			   WHERE  S.id_player = P.id_player AND \
					  S.id_gametype = %GAMETYPE% AND \
		              P.id_site = %SITEID% AND \
		              P.player_name LIKE '%SCREENNAME%') AS foo",
		// stat_group
		pt_group_advanced
	},
#endif

#if PT4_QUERY_SUPPORT__RIVER_AGGRESSION_FREQUENCY
	/* PT4 query to get river aggression frequency */
	{
		// name
		"river_afq",			
		// description_for_editor
		"Poker Tracker river aggression frequency",
		// query
		"SELECT (case when (AggressionCount + PassiveCount) = 0 then -1 \
				else cast(AggressionCount as real) / (AggressionCount + PassiveCount) \
				end) as result \
		 FROM (SELECT	sum(cnt_r_raise) + sum(case when flg_r_bet then 1 else 0 end) \
						 as AggressionCount, \
						sum(cnt_r_call) + sum(case when flg_r_fold then 1 else 0 end) \
						 as PassiveCount \
			   FROM   player as P, %TYPE%_hand_player_statistics as S \
			   WHERE  S.id_player = P.id_player AND \
					  S.id_gametype = %GAMETYPE% AND \
		              P.id_site = %SITEID% AND \
		              P.player_name LIKE '%SCREENNAME%') AS foo",
		// stat_group
		pt_group_advanced
	},
#endif

#if PT4_QUERY_SUPPORT__TOTAL_AGGRESSION_FREQUENCY
	/* PT4 query to get total aggression frequency without preflop */
	{
		// name
		"total_afq",	
		// description_for_editor
		"Poker Tracker total aggression frequency excluding preflop",
		// query:  (raises + bets) / (calls + folds + raises + bets)
		"SELECT (case when (AggressionCount + PassiveCount) = 0 then -1 \
				else cast(AggressionCount as real) / (AggressionCount + PassiveCount) \
				end) as result \
		 FROM (SELECT	  sum(cnt_f_raise) + sum(case when flg_f_bet then 1 else 0 end) \
						+ sum(cnt_t_raise) + sum(case when flg_t_bet then 1 else 0 end) \
						+ sum(cnt_r_raise) + sum(case when flg_r_bet then 1 else 0 end) \
						   as AggressionCount, \
						  sum(cnt_f_call) + sum(case when flg_f_fold then 1 else 0 end) \
                        + sum(cnt_t_call) + sum(case when flg_t_fold then 1 else 0 end) \
                        + sum(cnt_r_call) + sum(case when flg_r_fold then 1 else 0 end) \
						   as PassiveCount \
			   FROM   player as P, %TYPE%_hand_player_statistics as S \
			   WHERE  S.id_player = P.id_player AND \
					  S.id_gametype = %GAMETYPE% AND \
		              P.id_site = %SITEID% AND \
		              P.player_name LIKE '%SCREENNAME%') AS foo",
		// stat_group
		pt_group_advanced
	},
#endif

#if PT4_QUERY_SUPPORT__SEEN_FLOP_PCT
	/* PT4 query to get flops seen */
	{
		// name
		"flop_seen",		
		// description_for_editor
		"Poker Tracker saw flop",
		// query
		"SELECT (case when ActionOpportunities = 0 then -1 \
				 else cast(ActionCount as real) / ActionOpportunities \
				 end) as result \
		 FROM	(SELECT	sum(case when flg_f_saw then 1 else 0 end) \
						 as ActionCount, \
						count(*) \
						 as ActionOpportunities \
				 FROM	player as P, %TYPE%_hand_player_statistics as S \
				 WHERE	S.id_player = P.id_player AND \
						S.id_gametype = %GAMETYPE% AND \
						P.id_site = %SITEID% AND \
						P.player_name LIKE '%SCREENNAME%') foo",
		// stat_group
		pt_group_advanced
	},
#endif

#if PT4_QUERY_SUPPORT__SEEN_TURN_PCT
	/* PT4 query to get turns seen */
	{
		// name
		"turn_seen",		
		// description_for_editor
		"Poker Tracker saw turn",
		// query
		"SELECT (case when ActionOpportunities = 0 then -1 \
				 else cast(ActionCount as real) / ActionOpportunities \
				 end) as result \
		 FROM	(SELECT	sum(case when s.flg_t_saw then 1 else 0 end) \
						 as ActionCount, \
						sum((case when (s.flg_f_saw) then 1 else 0 end)) \
						 as ActionOpportunities \
				 FROM	player as P, %TYPE%_hand_player_statistics as S \
				 WHERE	S.id_player = P.id_player AND \
						S.id_gametype = %GAMETYPE% AND \
						P.id_site = %SITEID% AND \
						P.player_name LIKE '%SCREENNAME%') foo",
		// stat_group
		pt_group_advanced
	},
#endif

#if PT4_QUERY_SUPPORT__SEEN_RIVER_PCT
	/* PT4 query to get rivers seen */
	{
		// name
		"river_seen", 
		// description_for_editor
		"Poker Tracker saw river",
		// query
		"SELECT (case when ActionOpportunities = 0 then -1 \
				 else cast(ActionCount as real) / ActionOpportunities \
				 end) as result \
		 FROM	(SELECT	sum(case when s.flg_r_saw then 1 else 0 end) \
						 as ActionCount, \
						sum((case when (s.flg_t_saw) then 1 else 0 end)) \
						 as ActionOpportunities \
				 FROM	player as P, %TYPE%_hand_player_statistics as S \
				 WHERE	S.id_player = P.id_player AND \
						S.id_gametype = %GAMETYPE% AND \
						P.id_site = %SITEID% AND \
						P.player_name LIKE '%SCREENNAME%') foo",
		// stat_group
		pt_group_advanced
	},
#endif

#if PT4_QUERY_SUPPORT__FLOP_CBET
	/* PT4  query to get cbet on FLOP */
	{
		// name
		"flop_cbet",		
		// description_for_editor
		"Poker Tracker cbet",
		// query
		"SELECT (case when ActionOpportunities = 0 then -1 \
				 else cast(ActionCount as real) / ActionOpportunities \
				 end) as result \
		 FROM	(SELECT	sum(case when flg_f_cbet then 1 else 0 end) \
						 as ActionCount, \
						sum(case when flg_f_cbet_opp then 1 else 0 end) \
						 as ActionOpportunities \
				 FROM	player as P, %TYPE%_hand_player_statistics as S \
				 WHERE	S.id_player = P.id_player AND \
						S.id_gametype = %GAMETYPE% AND \
						P.id_site = %SITEID% AND \
						P.player_name LIKE '%SCREENNAME%') foo",
		// stat_group
		pt_group_advanced
	},
#endif

#if PT4_QUERY_SUPPORT__FLOP_FOLD_TO_CBET
	/* PT4  query to get FLOP fold to cbet */
	{
		// name
		"flop_fold_to_cbet",	
		// description_for_editor
		"Poker Tracker folded while facing cbet flop",
		// query
		"SELECT (case when ActionOpportunities = 0 then -1 \
				 else cast(ActionCount as real) / ActionOpportunities \
				 end) as result \
		 FROM	(SELECT	sum(case when enum_f_cbet_action = 'F' then 1 else 0 end) \
						 as ActionCount, \
						sum(case when flg_f_cbet_def_opp then 1 else 0 end) \
						 as ActionOpportunities \
				 FROM	player as P, %TYPE%_hand_player_statistics as S \
				 WHERE	S.id_player = P.id_player AND \
						S.id_gametype = %GAMETYPE% AND \
						P.id_site = %SITEID% AND \
						P.player_name LIKE '%SCREENNAME%') foo",
		// stat_group
		pt_group_advanced
	},
#endif

#if PT4_QUERY_SUPPORT__FLOP_RAISE_CBET
	/* PT4  query to get raise cbet on FLOP */
	{
		// name
		"flop_raise_cbet",		
		// description_for_editor
		"Poker Tracker raise flop cbet",
		// query
		"SELECT (case when ActionOpportunities = 0 then -1 \
				 else cast(ActionCount as real) / ActionOpportunities \
				 end) as result \
		 FROM	(SELECT	sum(case when S.enum_f_cbet_action = 'R' then 1 else 0 end) \
						 as ActionCount, \
						sum(case when S.flg_f_cbet_def_opp = 't' then 1 else 0 end) \
						 as ActionOpportunities \
				 FROM	player as P, %TYPE%_hand_player_statistics as S \
				 WHERE	S.id_player = P.id_player AND \
						S.id_gametype = %GAMETYPE% AND \
						P.id_site = %SITEID% AND \
						P.player_name LIKE '%SCREENNAME%') foo",
		pt_group_advanced
	},
#endif

#if PT4_QUERY_SUPPORT__TURN_CBET
	/* PT4  query to get cbet on TURN */
	{
		// name
		"turn_cbet",		
		// description_for_editor
		"Poker Tracker turn cbet",
		// query
		"SELECT (case when ActionOpportunities = 0 then -1 \
				 else cast(ActionCount as real) / ActionOpportunities \
				 end) as result \
		 FROM	(SELECT	sum(case when S.flg_t_cbet = 't' then 1 else 0 end) \
						 as ActionCount, \
						sum(case when flg_t_cbet_opp then 1 else 0 end) \
						 as ActionOpportunities \
				 FROM	player as P, %TYPE%_hand_player_statistics as S \
				 WHERE	S.id_player = P.id_player AND \
						S.id_gametype = %GAMETYPE% AND \
						P.id_site = %SITEID% AND \
						P.player_name LIKE '%SCREENNAME%') foo",
		pt_group_advanced
	},
#endif

#if PT4_QUERY_SUPPORT__TURN_FOLD_TO_CBET
	/* PT4  query to get TURN fold to cbet */
	{
		// name
		"turn_fold_to_cbet",
		// description_for_editor
		"Poker Tracker folded while facing cbet turn",
		// query
		"SELECT (case when ActionOpportunities = 0 then -1 \
				 else cast(ActionCount as real) / ActionOpportunities \
				 end) as result \
		 FROM	(SELECT	sum(case when enum_t_cbet_action = 'F' then 1 else 0 end) \
						 as ActionCount, \
						sum(case when flg_t_cbet_def_opp then 1 else 0 end) \
						 as ActionOpportunities \
				 FROM	player as P, %TYPE%_hand_player_statistics as S \
				 WHERE	S.id_player = P.id_player AND \
						S.id_gametype = %GAMETYPE% AND \
						P.id_site = %SITEID% AND \
						P.player_name LIKE '%SCREENNAME%') foo",
		// stat_group
		pt_group_advanced
	},
#endif

#if PT4_QUERY_SUPPORT__RIVER_FOLD_TO_CBET
	/* PT4  query to get RIVER fold to cbet */
	{
		// name
		"river_fold_to_cbet",
		// description_for_editor
		"Poker Tracker folded while facing cbet river",
		// query
		"SELECT (case when ActionOpportunities = 0 then -1 \
				 else cast(ActionCount as real) / ActionOpportunities \
				 end) as result \
		 FROM	(SELECT	sum(case when enum_r_cbet_action = 'F' then 1 else 0 end) \
						 as ActionCount, \
						sum(case when flg_r_cbet_def_opp then 1 else 0 end) \
						 as ActionOpportunities \
				 FROM	player as P, %TYPE%_hand_player_statistics as S \
				 WHERE	S.id_player = P.id_player AND \
						S.id_gametype = %GAMETYPE% AND \
						P.id_site = %SITEID% AND \
						P.player_name LIKE '%SCREENNAME%') foo",
		// stat_group
		pt_group_advanced
	},
#endif

#if PT4_QUERY_SUPPORT__POSTFLOP_FOLD_TO_3B
	/* PT4  query to get total fold to 3 bet without preflop*/
	{
		// name
		"postflop_fold_to_3bet",		
		// description_for_editor
		"Poker Tracker folded while facing 3bet total without preflop",
		// query
		"SELECT (case when ActionOpportunities = 0 then -1 \
				 else cast(ActionCount as real) / ActionOpportunities \
				 end) as result \
		 FROM	(SELECT	sum(case when enum_f_3bet_action = 'F' or enum_t_3bet_action ='F' or enum_r_3bet_action = 'F' then 1 else 0 end) \
						 as ActionCount, \
						sum(case when flg_f_3bet_def_opp or flg_t_3bet_def_opp or flg_r_3bet_def_opp then 1 else 0 end) \
						 as ActionOpportunities \
				 FROM	player as P, %TYPE%_hand_player_statistics as S \
				 WHERE	S.id_player = P.id_player AND \
						S.id_gametype = %GAMETYPE% AND \
						P.id_site = %SITEID% AND \
						P.player_name LIKE '%SCREENNAME%') foo",
		// stat_group
		pt_group_advanced
	},
#endif

#if PT4_QUERY_SUPPORT__FLOP_FOLD_TO_3B
	/* PT4  query to get FLOP fold to 3 bet */
	{
		// name
		"flop_fold_to_3bet",
		// description_for_editor
		"Poker Tracker folded while facing 3bet flop",
		// query
		"SELECT (case when ActionOpportunities = 0 then -1 \
				 else cast(ActionCount as real) / ActionOpportunities \
				 end) as result \
		 FROM	(SELECT	sum(case when enum_f_3bet_action = 'F' then 1 else 0 end) \
						 as ActionCount, \
						sum(case when flg_f_3bet_def_opp then 1 else 0 end) \
						 as ActionOpportunities \
				 FROM	player as P, %TYPE%_hand_player_statistics as S \
				 WHERE	S.id_player = P.id_player AND \
						S.id_gametype = %GAMETYPE% AND \
						P.id_site = %SITEID% AND \
						P.player_name LIKE '%SCREENNAME%') foo",
		// stat_group
		pt_group_advanced
	},
#endif

#if PT4_QUERY_SUPPORT__FLOP_CHECK_RAISE
	/* PT4  query to get flop check-raise */
	{
		// name
		"flop_checkraise",		
		// description_for_editor
		"Poker Tracker flop check-raise",
		// query
		"SELECT (case when ActionOpportunities = 0 then -1 \
				 else cast(ActionCount as real) / ActionOpportunities \
				 end) as result \
		 FROM	(SELECT	sum(case when (flg_f_check_raise) then 1 else 0 end) \
						 as ActionCount, \
						sum (case when (flg_f_check=TRUE AND (cnt_f_raise>0 OR cnt_f_call>0 OR flg_f_fold)) then 1 else 0 end) \
						 as ActionOpportunities \
				 FROM	player as P, %TYPE%_hand_player_statistics as S \
				 WHERE	S.id_player = P.id_player AND \
						S.id_gametype = %GAMETYPE% AND \
						P.id_site = %SITEID% AND \
						P.player_name LIKE '%SCREENNAME%') foo",
		pt_group_advanced
	},
#endif

#if PT4_QUERY_SUPPORT__FLOP_DONKBET
	/* PT4  query to get FLOP donk */
	{
		// name
		"flop_donkbet",
		// description_for_editor
		"Poker Tracker donk flop",
		// query
		"SELECT (case when ActionOpportunities = 0 then -1 \
				 else cast(ActionCount as real) / ActionOpportunities \
				 end) as result \
		 FROM	(SELECT	sum(case when (S.flg_p_face_raise AND NOT(S.flg_p_3bet OR S.flg_p_4bet) \
						AND S.flg_f_bet AND NOT(S.flg_f_cbet_opp) AND \
						((HSum.cnt_players > 2 and S.val_p_raise_aggressor_pos < S.position) or \
						(HSum.cnt_players = 2 and S.flg_blind_b))) then 1 else 0 end) \
						 as ActionCount, \
						sum(case when (S.flg_p_face_raise AND NOT(S.flg_p_3bet OR S.flg_p_4bet) \
						AND (S.flg_f_open_opp) AND NOT(S.flg_f_cbet_opp) AND \
						((HSum.cnt_players > 2 and S.val_p_raise_aggressor_pos < S.position) or \
						(HSum.cnt_players = 2 and S.flg_blind_b))) then 1 else 0 end) \
						 as ActionOpportunities \
				 FROM	player as P, %TYPE%_hand_summary as HSum, %TYPE%_hand_player_statistics as S \
				 WHERE	S.id_player = P.id_player AND \
						S.id_gametype = %GAMETYPE% AND \
						HSum.id_hand = S.id_hand AND \
						P.id_site = %SITEID% AND \
						P.player_name LIKE '%SCREENNAME%') foo",
		// stat_group
		pt_group_advanced
	},
#endif

#if PT4_QUERY_SUPPORT__FLOP_FLOAT
	/* PT4  query to get flop float */
	{
		// name
		"flop_float",
		// description_for_editor
		"Poker Tracker flop float",
		// query
		"SELECT (case when ActionOpportunities = 0 then -1 \
				 else cast(ActionCount as real) / ActionOpportunities \
				 end) as result \
				 FROM	(SELECT	sum(case when ((cnt_p_call>0) AND S.flg_p_face_raise AND S.flg_f_bet \
								AND char_length(HSum.str_aggressors_p) = 2 \
								AND ((HSum.cnt_players > 2 AND S.val_p_raise_aggressor_pos > S.position) \
									OR (HSum.cnt_players = 2 AND S.flg_f_has_position))) then 1 else 0 end) \
								 as ActionCount, \
								sum(case when ((cnt_p_call>0) AND S.flg_p_face_raise AND S.flg_f_open_opp \
								AND char_length(HSum.str_aggressors_p)=2 \
								AND((HSum.cnt_players>2 AND S.val_p_raise_aggressor_pos > S.position) \
									OR(HSum.cnt_players=2 AND S.flg_f_has_position))) then 1 else 0 end) \
								 as ActionOpportunities \
				 FROM	player as P, %TYPE%_hand_summary as HSum, %TYPE%_hand_player_statistics as S \
				 WHERE	S.id_player = P.id_player AND \
						S.id_gametype = %GAMETYPE% AND \
						HSum.id_hand = S.id_hand AND \
						P.id_site = %SITEID% AND \
						P.player_name LIKE '%SCREENNAME%') foo",
		// stat_group
		pt_group_advanced
	},
#endif

#if PT4_QUERY_SUPPORT__TURN_FOLD_TO_3B
	/* PT4  query to get TURN fold to 3 bet */
	{
		// name
		"turn_fold_to_3bet",	
		// description_for_editor
		"Poker Tracker folded while facing 3bet turn",
		// query
		"SELECT (case when ActionOpportunities = 0 then -1 \
				 else cast(ActionCount as real) / ActionOpportunities \
				 end) as result \
		 FROM	(SELECT	sum(case when enum_t_3bet_action = 'F' then 1 else 0 end) \
						 as ActionCount, \
						sum(case when flg_t_3bet_def_opp then 1 else 0 end) \
						 as ActionOpportunities \
				 FROM	player as P, %TYPE%_hand_player_statistics as S \
				 WHERE	S.id_player = P.id_player AND \
						S.id_gametype = %GAMETYPE% AND \
						P.id_site = %SITEID% AND \
						P.player_name LIKE '%SCREENNAME%') foo",
		// stat_group
		pt_group_advanced
	},
#endif

#if PT4_QUERY_SUPPORT__TURN_CHECK_RAISE
	/* PT4  query to get TURN Check-Raise */
	{
		// name
		"turn_checkraise",	
		// description_for_editor
		"Poker Tracker turn Check-Raise",
		// query
		"SELECT (case when ActionOpportunities = 0 then -1 \
				 else cast(ActionCount as real) / ActionOpportunities \
				 end) as result \
		 FROM	(SELECT	sum(case when (flg_t_check_raise) then 1 else 0 end) \
						 as ActionCount, \
						sum(case when (flg_t_check=TRUE AND (cnt_t_raise>0 OR cnt_t_call>0 OR flg_t_fold)) then 1 else 0 end) \
						 as ActionOpportunities \
				 FROM	player as P, %TYPE%_hand_player_statistics as S \
				 WHERE	S.id_player = P.id_player AND \
						S.id_gametype = %GAMETYPE% AND \
						P.id_site = %SITEID% AND \
						P.player_name LIKE '%SCREENNAME%') foo",
		// stat_group
		pt_group_advanced
	},
#endif

#if PT4_QUERY_SUPPORT__TURN_CHECK_CALL
	/* PT4  query to get TURN Check-Call */
	{
		// name
		"turn_checkcall",	
		// description_for_editor
		"Poker Tracker turn Check-Call",
		// query
		"SELECT (case when ActionOpportunities = 0 then -1 \
				 else cast(ActionCount as real) / ActionOpportunities \
				 end) as result \
		 FROM	(SELECT	sum(case when (S.flg_t_check AND S.cnt_t_call>0) then 1 else 0 end) \
						 as ActionCount, \
						sum(case when (S.flg_t_check= 't' AND (S.cnt_t_raise>0 OR S.cnt_t_call>0 OR S.flg_t_fold= 't')) then 1 else 0 end) \
						 as ActionOpportunities \
				 FROM	player as P, %TYPE%_hand_player_statistics as S \
				 WHERE	S.id_player = P.id_player AND \
						S.id_gametype = %GAMETYPE% AND \
						P.id_site = %SITEID% AND \
						P.player_name LIKE '%SCREENNAME%') foo",
		// stat_group
		pt_group_advanced
	},
#endif

#if PT4_QUERY_SUPPORT__RIVER_FOLD_TO_3B
	/* PT4  query to get RIVER fold to 3 bet */
	{
		// name
		"river_fold_to_3bet",	
		// description_for_editor
		"Poker Tracker folded while facing 3bet river",
		// query
		"SELECT (case when ActionOpportunities = 0 then -1 \
				 else cast(ActionCount as real) / ActionOpportunities \
				 end) as result \
		 FROM	(SELECT	sum(case when enum_r_3bet_action = 'F' then 1 else 0 end) \
						 as ActionCount, \
						sum(case when flg_r_3bet_def_opp then 1 else 0 end) \
						 as ActionOpportunities \
				 FROM	player as P, %TYPE%_hand_player_statistics as S \
				 WHERE	S.id_player = P.id_player AND \
						S.id_gametype = %GAMETYPE% AND \
						P.id_site = %SITEID% AND \
						P.player_name LIKE '%SCREENNAME%') foo",
		// stat_group
		pt_group_advanced
	},
#endif

#if PT4_QUERY_SUPPORT__RIVER_BET
	/* PT4  query to get River bet */
	{
		// name
		"river_bet",
		// description_for_editor
		"Poker Tracker river bet",
		// query
		"SELECT (case when ActionOpportunities = 0 then -1 \
				 else cast(ActionCount as real) / ActionOpportunities \
				 end) as result \
		 FROM	(SELECT	sum(case when flg_r_bet then 1 else 0 end) \
						 as ActionCount, \
						sum(case when (flg_r_bet or flg_r_check) then 1 else 0 end) \
						 as ActionOpportunities \
				 FROM	player as P, %TYPE%_hand_player_statistics as S \
				 WHERE	S.id_player = P.id_player AND \
						S.id_gametype = %GAMETYPE% AND \
						P.id_site = %SITEID% AND \
						P.player_name LIKE '%SCREENNAME%') foo",
		// stat_group
		pt_group_advanced
	},
#endif

#if PT4_QUERY_SUPPORT__SB_RFI
	/* PT4  query to get SmallBlind raise first in */
	{
		//name
		"sb_rfi",
		// description_for_editor
		"Poker Tracker small blind raise first in",
		//query
		"SELECT (case when  (sum((case when(s.flg_p_open_opp and s.position=9) then  1 else  0 end))) = 0 then -1\ else cast(sum(case when(s.flg_p_first_raise AND \
		           s.flg_p_open_opp and s.position=9) then  1 else  0 end)As Real)/  \
		           (sum((case when(s.flg_p_open_opp and s.position=9) then  1 else  0 end))) end) as result \
		FROM   player as P, %TYPE%_hand_player_statistics as S \
		         WHERE  S.id_player = P.id_player AND \
		                S.id_gametype = %GAMETYPE% AND \
		                P.id_site = %SITEID% AND \
		                P.player_name LIKE '%SCREENNAME%'",
		// stat_group
		pt_group_advanced
	},
#endif

#if PT4_QUERY_SUPPORT__BTN_RFI
	/* PT4  query to get Button raise first in */
	{
		//name
		"btn_rfi",
		// description_for_editor
		"Poker Tracker button raise first in",
		//query
		"SELECT (case when  (sum((case when(s.flg_p_open_opp and s.position=0) then  1 else  0 end))) = 0 then -1\ else cast(sum(case when(s.flg_p_first_raise AND \
		           s.flg_p_open_opp and s.position=0) then  1 else  0 end)As Real)/  \
		           (sum((case when(s.flg_p_open_opp and s.position=0) then  1 else  0 end))) end) as result \
		FROM   player as P, %TYPE%_hand_player_statistics as S \
		         WHERE  S.id_player = P.id_player AND \
		                S.id_gametype = %GAMETYPE% AND \
		                P.id_site = %SITEID% AND \
		                P.player_name LIKE '%SCREENNAME%'",
		// stat_group
		pt_group_advanced
	},
#endif

#if PT4_QUERY_SUPPORT__CO_RFI
	/* PT4  query to get Cut-Off raise first in */
	{
		//name
		"co_rfi",
		// description_for_editor
		"Poker Tracker cut off raise first in",
		//query
		"SELECT (case when  (sum((case when(s.flg_p_open_opp and s.position=1) then  1 else  0 end))) = 0 then -1\ else cast(sum(case when(s.flg_p_first_raise AND \
		           s.flg_p_open_opp and s.position=1) then  1 else  0 end)As Real)/  \
		           (sum((case when(s.flg_p_open_opp and s.position=1) then  1 else  0 end))) end) as result \
		FROM   player as P, %TYPE%_hand_player_statistics as S \
		         WHERE  S.id_player = P.id_player AND \
		                S.id_gametype = %GAMETYPE% AND \
		                P.id_site = %SITEID% AND \
		                P.player_name LIKE '%SCREENNAME%'",
		// stat_group
		pt_group_advanced
	},
#endif

#if PT4_QUERY_SUPPORT__MP3_RFI
	/* PT4  query to get Middle Position 3 raise first in */
	{
		//name
		"mp3_rfi",
		// description_for_editor
		"Poker Tracker middle position 3 raise first in",
		//query
		"SELECT (case when  (sum((case when(s.flg_p_open_opp and s.position=2) then  1 else  0 end))) = 0 then -1\ else cast(sum(case when(s.flg_p_first_raise AND \
		           s.flg_p_open_opp and s.position=2) then  1 else  0 end)As Real)/  \
		           (sum((case when(s.flg_p_open_opp and s.position=2) then  1 else  0 end))) end) as result \
		FROM   player as P, %TYPE%_hand_player_statistics as S \
		         WHERE  S.id_player = P.id_player AND \
		                S.id_gametype = %GAMETYPE% AND \
		                P.id_site = %SITEID% AND \
		                P.player_name LIKE '%SCREENNAME%'",
		// stat_group
		pt_group_advanced
	},
#endif

#if PT4_QUERY_SUPPORT__MP2_RFI
	/* PT4  query to get Middle Position 2 raise first in */
	{
		//name
		"mp2_rfi",
		// description_for_editor
		"Poker Tracker middle position 2 raise first in",
		//query
		"SELECT (case when  (sum((case when(s.flg_p_open_opp and s.position=3) then  1 else  0 end))) = 0 then -1\ else cast(sum(case when(s.flg_p_first_raise AND \
		           s.flg_p_open_opp and s.position=3) then  1 else  0 end)As Real)/  \
		           (sum((case when(s.flg_p_open_opp and s.position=3) then  1 else  0 end))) end) as result \
		FROM   player as P, %TYPE%_hand_player_statistics as S \
		         WHERE  S.id_player = P.id_player AND \
		                S.id_gametype = %GAMETYPE% AND \
		                P.id_site = %SITEID% AND \
		                P.player_name LIKE '%SCREENNAME%'",
		// stat_group
		pt_group_advanced
	},
#endif
};

#endif // INC_POKERTRACKER_QUERIES_VERSION_3_H