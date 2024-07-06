///*****************************************************************************
//
// This file is part of the OpenHoldem project
//   Download page:         http://code.google.com/p/openholdembot/
//   Forums:                http://www.maxinmontreal.com/forums/index.php
//   Licensed under GPL v3: http://www.gnu.org/licenses/gpl.html
//
///*****************************************************************************
//
// Purpose:
//
///*****************************************************************************

// GENERAL STATS
// ICON disabled, as not supported by PT 4.
// New VPIP and PFR are NOW supported 
// We keep old way for these 2 stats with name conversion as Legacy
// Now we support an incredible number of statistics 
// As far as possible, every statistic in its name and in its description, is faithful to what is reported in PT4 that now is 100% supported
// Nothing from the old source has been eliminated 
// Only added and something renamed
// A special thank you to the whole community, which made this possible 
// We are Maxinmontreal 
// https://pt4.pokertracker.com/forums/viewtopic.php?f=59&t=49035
// http://www.maxinmontreal.com/forums/viewforum.php?f=273
// http://www.maxinmontreal.com/forums/viewtopic.php?f=273&t=20933
// http://www.maxinmontreal.com/forums/viewtopic.php?f=273&t=21561

#pragma once
#define PT4_QUERY_SUPPORT__PFR							(TRUE)	// "pfr aka pt4"
#define PT4_QUERY_SUPPORT__VPIP							(TRUE)	// "vpip aka pt4"
#define PT4_QUERY_SUPPORT__ICON							(FALSE) // "icon"
#define PT4_QUERY_SUPPORT__NB_HANDS					(TRUE)	// "hands"
#define PT4_QUERY_SUPPORT__VPIP_LEGACY			(TRUE)	// "vpip aka pt3"
#define PT4_QUERY_SUPPORT__PFR_LEGACY				(TRUE)	// "pfr aka pt3"
#define PT4_QUERY_SUPPORT__RFI							(TRUE)	// "rfi"
#define PT4_QUERY_SUPPORT__WTSD							(TRUE)	// "wtsd"
#define PT4_QUERY_SUPPORT__WSSD							(TRUE)	// "wssd"

// PREFLOP STATS
#define PT4_QUERY_SUPPORT__STEAL_ATTEMPT				  (TRUE)	// "steal_attempt"
#define PT4_QUERY_SUPPORT__FOLD_BB_TO_STEAL				(TRUE)	// "bb_fold_to_steal"
#define PT4_QUERY_SUPPORT__FOLD_SB_TO_STEAL				(TRUE)	// "sb_fold_to_steal"
#define PT4_QUERY_SUPPORT__3B_VS_STEAL					  (TRUE)	// "3bet_vs_steal"
#define PT4_QUERY_SUPPORT__BB_3B_VS_STEAL				  (TRUE)	// "bb_3bet_vs_steal"
#define PT4_QUERY_SUPPORT__SB_3B_VS_STEAL				  (TRUE)	// "sb_3bet_vs_steal"
#define PT4_QUERY_SUPPORT__PREFLOP_CALLED_RAISE		(TRUE)	// "preflop_called_raise"
#define PT4_QUERY_SUPPORT__PREFLOP_3B					    (TRUE)	// "3bet"
#define PT4_QUERY_SUPPORT__PREFLOP_FOLD_TO_3B			(TRUE)	// "fold_to_3bet"
#define PT4_QUERY_SUPPORT__PREFLOP_FOLD_TO_3B_AFTER_RAISE			(TRUE)	// "fold_to_3bet"
#define PT4_QUERY_SUPPORT__PF_FOLD_TO_RESTEAL			(TRUE)	// "fold_to_resteal"
#define PT4_QUERY_SUPPORT__PREFLOP_4B					    (TRUE)	// "4bet"
#define PT4_QUERY_SUPPORT__PREFLOP_FOLD_TO_4B			(TRUE)	// "fold_to_4bet"
#define PT4_QUERY_SUPPORT__4BET_PLUS_RATIO     		(TRUE)	// ""
#define PT4_QUERY_SUPPORT__FOLD_TO_PF_SQUEEZE     (TRUE)	// ""
#define PT4_QUERY_SUPPORT__FOLD_TO_SQUEEZE_AS_RAISER  		(TRUE)	// ""
#define PT4_QUERY_SUPPORT__FOLD_TO_SQUEEZE_AS_CALLER  		(TRUE)	// ""
#define PT4_QUERY_SUPPORT__PREFLOP_SQUEEZE     		(TRUE)	// ""
#define PT4_QUERY_SUPPORT__FOLD_TO_PF_4BET_AFTER_3BET     (TRUE)	// ""

// AF
#define PT4_QUERY_SUPPORT__FLOP_AGGRESSION_FACTOR		(TRUE)	// "flop_af"
#define PT4_QUERY_SUPPORT__TURN_AGGRESSION_FACTOR		(TRUE)	// "turn_af"
#define PT4_QUERY_SUPPORT__RIVER_AGGRESSION_FACTOR	(TRUE)	// "river_af"
#define PT4_QUERY_SUPPORT__TOTAL_AGGRESSION_FACTOR	(TRUE)	// "total_af"

// AFq
#define PT4_QUERY_SUPPORT__FLOP_AGGRESSION_FREQUENCY	(TRUE)	// "flop_afq" 
#define PT4_QUERY_SUPPORT__TURN_AGGRESSION_FREQUENCY	(TRUE)	// "turn_afq" 
#define PT4_QUERY_SUPPORT__RIVER_AGGRESSION_FREQUENCY	(TRUE)	// "river_afq" 
#define PT4_QUERY_SUPPORT__TOTAL_AGGRESSION_FREQUENCY	(TRUE)	// "total_afq" 

// Street seen
#define PT4_QUERY_SUPPORT__SEEN_FLOP_PCT			  	(TRUE)	// "flop_seen"
#define PT4_QUERY_SUPPORT__SEEN_TURN_PCT			  	(TRUE)	// "turn_seen"
#define PT4_QUERY_SUPPORT__SEEN_RIVER_PCT			  	(TRUE)	// "river_seen"

// Continuation bet // not filtred /// action for all type of pre flop game /// AKA SRP 3BET AND 3BET+
// Flop
#define PT4_QUERY_SUPPORT__FLOP_CBET					  (TRUE)	// "flop_cbet"
#define PT4_QUERY_SUPPORT__FLOP_FOLD_TO_CBET	  (TRUE)	// "flop_fold_to_cbet"
#define PT4_QUERY_SUPPORT__FLOP_RAISE_CBET		  (TRUE)	// "flop_raise_cbet"
// Turn
#define PT4_QUERY_SUPPORT__TURN_CBET					  (TRUE)	// "turn_cbet"
#define PT4_QUERY_SUPPORT__TURN_FOLD_TO_CBET	  (TRUE)	// "turn_fold_to_cbet"
// River
#define PT4_QUERY_SUPPORT__RIVER_FOLD_TO_CBET	  (TRUE)	// "river_fold_to_cbet"

// POSTFLOP Other stats
#define PT4_QUERY_SUPPORT__POSTFLOP_FOLD_TO_3B	(TRUE)	// "postflop_fold_to_3bet"

// FLOP Other stats
#define PT4_QUERY_SUPPORT__FLOP_FOLD_TO_3B			(FALSE)	// "flop_fold_to_3bet"   // 3b is a reraise, we should have a "flop_fold_to_raise" stat first.
#define PT4_QUERY_SUPPORT__FLOP_CHECK_RAISE			(TRUE)	// "flop_checkraise"
#define PT4_QUERY_SUPPORT__FLOP_DONKBET					(TRUE)	// "flop_donkbet"
#define PT4_QUERY_SUPPORT__FLOP_FLOAT     			(TRUE)	// "flop_float" (PT's stat for: bet IP flop vs missed cbet)
#define PT4_QUERY_SUPPORT__FOLD_TO_F_CHECK_RAISE     				(TRUE)	// ""
#define PT4_QUERY_SUPPORT__FLOAT_FLOP     				(TRUE)	// ""
#define PT4_QUERY_SUPPORT__CALL_F_FLOAT_BET     				(TRUE)	// ""

// TURN Other stats
#define PT4_QUERY_SUPPORT__TURN_FOLD_TO_3B			(FALSE)	// "turn_fold_to_3bet" // 3b is a reraise, we should have a "turn_fold_to_raise" stat first.
#define PT4_QUERY_SUPPORT__TURN_CHECK_RAISE			(TRUE)	// "turn_checkraise"
#define PT4_QUERY_SUPPORT__TURN_CHECK_CALL			(TRUE)	// "turn_checkcall"

// RIVER Other stats
#define PT4_QUERY_SUPPORT__RIVER_FOLD_TO_3B			(FALSE)	// "river_fold_to_3bet" // 3b is a reraise, we should have a "river_fold_to_raise" stat first.
#define PT4_QUERY_SUPPORT__RIVER_BET     				(TRUE)	// "river_bet"

// FLOP - 3BET Pots AND 3BET + Pots
#define PT4_QUERY_SUPPORT__DONK_FLOP_IN_3BET_PLUS_POT     				(TRUE)	// ""
#define PT4_QUERY_SUPPORT__FOLD_TO_F_CBET_IN_3BET_POT     				(TRUE)	// ""
#define PT4_QUERY_SUPPORT__CBET_FLOP_IN_3BET_POT     				(TRUE)	// ""
#define PT4_QUERY_SUPPORT__FLOP_CHECK_RAISE_IN_3BET_POT     				(TRUE)	// ""
#define PT4_QUERY_SUPPORT__FLOP_CHECK_RAISE_IN_3BET_PLUS_POT     				(TRUE)	// ""
#define PT4_QUERY_SUPPORT__FOLD_TO_F_CHECK_RAISE_3B_PLUS_POT     				(TRUE)	// ""
#define PT4_QUERY_SUPPORT__FLOAT_FLOP_IN_3BET_POT     				(TRUE)	// ""
#define PT4_QUERY_SUPPORT__CALL_F_FLOAT_BET_IN_3B_POT     				(TRUE)	// ""
#define PT4_QUERY_SUPPORT__FOLD_TO_F_FLOAT_BET_IN_3B_POT     				(TRUE)	// ""
#define PT4_QUERY_SUPPORT__FOLD_TO_F_FLOAT_BET_IN_3B_PLUS_POT     				(TRUE)	// ""

// Advanced RFI - Positional Pre Flop - 6 MAX AND 9 MAX now supported
#define PT4_QUERY_SUPPORT__RFI_BTN_6MAX     				(TRUE)	// ""
#define PT4_QUERY_SUPPORT__RFI_CO_6MAX     				(TRUE)	// ""
#define PT4_QUERY_SUPPORT__RFI_MP_6MAX     				(TRUE)	// ""
#define PT4_QUERY_SUPPORT__RFI_EP_6MAX     				(TRUE)	// ""
#define PT4_QUERY_SUPPORT__RFI_SB_6MAX     				(TRUE)	// ""

#define PT4_QUERY_SUPPORT__RFI_BTN_9MAX     				(TRUE)	// ""
#define PT4_QUERY_SUPPORT__RFI_CO_9MAX     				(TRUE)	// ""
#define PT4_QUERY_SUPPORT__RFI_MP_9MAX     				(TRUE)	// ""
#define PT4_QUERY_SUPPORT__RFI_MP1_9MAX     				(TRUE)	// ""
#define PT4_QUERY_SUPPORT__RFI_MP2_9MAX     				(TRUE)	// ""
#define PT4_QUERY_SUPPORT__RFI_EP_9MAX     				(TRUE)	// ""
#define PT4_QUERY_SUPPORT__RFI_EP1_9MAX     				(TRUE)	// ""
#define PT4_QUERY_SUPPORT__RFI_SB_9MAX     				(TRUE)	// ""

// Advanced OPPORTUNITIES /// 
#define PT4_QUERY_SUPPORT__CBET_FLOP_OPP_IN_3BET_POT     				(TRUE)	// ""
#define PT4_QUERY_SUPPORT__CBET_FLOP_OPP     				(TRUE)	// ""
#define PT4_QUERY_SUPPORT__FOLD_TO_3BET_AFTER_R_OPP     				(TRUE)	// ""
#define PT4_QUERY_SUPPORT__PREFLOP_SQUEEZE_OPP   				(TRUE)	// ""
#define PT4_QUERY_SUPPORT__FLOP_FOLD_TO_CBET__FLOP_RAISE_CBET_OPP     				(TRUE)	// ""
#define PT4_QUERY_SUPPORT__FLOP_DONKBET_OPP    				(TRUE)	// ""
#define PT4_QUERY_SUPPORT__FOLD_TO_F_CBET_IN_3BET_PLUS_POT_OPP     				(TRUE)	// ""
#define PT4_QUERY_SUPPORT__FOLD_TO_F_CHECK_RAISE_OPP     				(TRUE)	// ""
#define PT4_QUERY_SUPPORT__FOLD_TO_SQUEEZE_AS_RAISER_OPP     				(TRUE)	// ""

const int k_number_of_pokertracker_stats =  //GENERAL STATS
(PT4_QUERY_SUPPORT__ICON ? 1 : 0)
+ (PT4_QUERY_SUPPORT__NB_HANDS ? 1 : 0)
+ (PT4_QUERY_SUPPORT__PFR ? 1 : 0)
+ (PT4_QUERY_SUPPORT__VPIP ? 1 : 0)
+ (PT4_QUERY_SUPPORT__VPIP_LEGACY ? 1 : 0)
+ (PT4_QUERY_SUPPORT__PFR_LEGACY ? 1 : 0)
+ (PT4_QUERY_SUPPORT__RFI ? 1 : 0)
+ (PT4_QUERY_SUPPORT__WTSD ? 1 : 0)
+ (PT4_QUERY_SUPPORT__WSSD ? 1 : 0)
//PREFLOP STATS
+ (PT4_QUERY_SUPPORT__STEAL_ATTEMPT ? 1 : 0)
+ (PT4_QUERY_SUPPORT__FOLD_BB_TO_STEAL ? 1 : 0)
+ (PT4_QUERY_SUPPORT__FOLD_SB_TO_STEAL ? 1 : 0)
+ (PT4_QUERY_SUPPORT__3B_VS_STEAL ? 1 : 0)
+ (PT4_QUERY_SUPPORT__BB_3B_VS_STEAL ? 1 : 0)
+ (PT4_QUERY_SUPPORT__SB_3B_VS_STEAL ? 1 : 0)
+ (PT4_QUERY_SUPPORT__PREFLOP_CALLED_RAISE ? 1 : 0)
+ (PT4_QUERY_SUPPORT__PREFLOP_3B ? 1 : 0)
+ (PT4_QUERY_SUPPORT__PREFLOP_FOLD_TO_3B ? 1 : 0)
+ (PT4_QUERY_SUPPORT__PREFLOP_FOLD_TO_3B_AFTER_RAISE ? 1 : 0)
+ (PT4_QUERY_SUPPORT__PF_FOLD_TO_RESTEAL ? 1 : 0)
+ (PT4_QUERY_SUPPORT__PREFLOP_4B ? 1 : 0)
+ (PT4_QUERY_SUPPORT__PREFLOP_FOLD_TO_4B ? 1 : 0)
//  AF
+ (PT4_QUERY_SUPPORT__FLOP_AGGRESSION_FACTOR ? 1 : 0)
+ (PT4_QUERY_SUPPORT__TURN_AGGRESSION_FACTOR ? 1 : 0)
+ (PT4_QUERY_SUPPORT__RIVER_AGGRESSION_FACTOR ? 1 : 0)
+ (PT4_QUERY_SUPPORT__TOTAL_AGGRESSION_FACTOR ? 1 : 0)
//  AFq
+ (PT4_QUERY_SUPPORT__FLOP_AGGRESSION_FREQUENCY ? 1 : 0)
+ (PT4_QUERY_SUPPORT__TURN_AGGRESSION_FREQUENCY ? 1 : 0)
+ (PT4_QUERY_SUPPORT__RIVER_AGGRESSION_FREQUENCY ? 1 : 0)
+ (PT4_QUERY_SUPPORT__TOTAL_AGGRESSION_FREQUENCY ? 1 : 0)
// POSTFLOP stats
+ (PT4_QUERY_SUPPORT__POSTFLOP_FOLD_TO_3B ? 1 : 0)
//  Street seen
+ (PT4_QUERY_SUPPORT__SEEN_FLOP_PCT ? 1 : 0)
+ (PT4_QUERY_SUPPORT__SEEN_TURN_PCT ? 1 : 0)
+ (PT4_QUERY_SUPPORT__SEEN_RIVER_PCT ? 1 : 0)
//  Continuation bet
//  Flop
+ (PT4_QUERY_SUPPORT__FLOP_CBET ? 1 : 0)
+ (PT4_QUERY_SUPPORT__FLOP_FOLD_TO_CBET ? 1 : 0)
+ (PT4_QUERY_SUPPORT__FLOP_RAISE_CBET ? 1 : 0)
//  Turn
+ (PT4_QUERY_SUPPORT__TURN_CBET ? 1 : 0)
+ (PT4_QUERY_SUPPORT__TURN_FOLD_TO_CBET ? 1 : 0)
//  River
+ (PT4_QUERY_SUPPORT__RIVER_FOLD_TO_CBET ? 1 : 0)
//  Flop Other stats
+ (PT4_QUERY_SUPPORT__FLOP_FOLD_TO_3B ? 1 : 0)
+ (PT4_QUERY_SUPPORT__FLOP_CHECK_RAISE ? 1 : 0)
+ (PT4_QUERY_SUPPORT__FLOP_DONKBET ? 1 : 0)
+ (PT4_QUERY_SUPPORT__FLOP_FLOAT ? 1 : 0)
//  Turn Other stats
+ (PT4_QUERY_SUPPORT__TURN_FOLD_TO_3B ? 1 : 0)
+ (PT4_QUERY_SUPPORT__TURN_CHECK_RAISE ? 1 : 0)
+ (PT4_QUERY_SUPPORT__TURN_CHECK_CALL ? 1 : 0)
//  River Other stats
+ (PT4_QUERY_SUPPORT__RIVER_FOLD_TO_3B ? 1 : 0)
+ (PT4_QUERY_SUPPORT__RIVER_BET ? 1 : 0)
//  Varie
+ (PT4_QUERY_SUPPORT__4BET_PLUS_RATIO ? 1 : 0)
+ (PT4_QUERY_SUPPORT__FOLD_TO_PF_SQUEEZE ? 1 : 0)
+ (PT4_QUERY_SUPPORT__FOLD_TO_SQUEEZE_AS_RAISER ? 1 : 0)
+ (PT4_QUERY_SUPPORT__FOLD_TO_SQUEEZE_AS_CALLER ? 1 : 0)
+ (PT4_QUERY_SUPPORT__FOLD_TO_F_CHECK_RAISE ? 1 : 0)
+ (PT4_QUERY_SUPPORT__FLOAT_FLOP ? 1 : 0)
+ (PT4_QUERY_SUPPORT__CALL_F_FLOAT_BET ? 1 : 0)
+ (PT4_QUERY_SUPPORT__PREFLOP_SQUEEZE ? 1 : 0)
//  3bet and 3bet+
+ (PT4_QUERY_SUPPORT__DONK_FLOP_IN_3BET_PLUS_POT ? 1 : 0)
+ (PT4_QUERY_SUPPORT__FOLD_TO_F_CBET_IN_3BET_POT ? 1 : 0)
+ (PT4_QUERY_SUPPORT__CBET_FLOP_IN_3BET_POT ? 1 : 0)
+ (PT4_QUERY_SUPPORT__FLOP_CHECK_RAISE_IN_3BET_POT ? 1 : 0)
+ (PT4_QUERY_SUPPORT__FLOP_CHECK_RAISE_IN_3BET_PLUS_POT ? 1 : 0)
+ (PT4_QUERY_SUPPORT__FOLD_TO_F_CHECK_RAISE_3B_PLUS_POT ? 1 : 0)
+ (PT4_QUERY_SUPPORT__FLOAT_FLOP_IN_3BET_POT ? 1 : 0)
+ (PT4_QUERY_SUPPORT__CALL_F_FLOAT_BET_IN_3B_POT ? 1 : 0)
+ (PT4_QUERY_SUPPORT__FOLD_TO_F_FLOAT_BET_IN_3B_POT ? 1 : 0)
+ (PT4_QUERY_SUPPORT__FOLD_TO_F_FLOAT_BET_IN_3B_PLUS_POT ? 1 : 0)
//	Positional	
+ (PT4_QUERY_SUPPORT__RFI_BTN_6MAX ? 1 : 0)
+ (PT4_QUERY_SUPPORT__RFI_CO_6MAX ? 1 : 0)
+ (PT4_QUERY_SUPPORT__RFI_MP_6MAX ? 1 : 0)
+ (PT4_QUERY_SUPPORT__RFI_EP_6MAX ? 1 : 0)
+ (PT4_QUERY_SUPPORT__RFI_SB_6MAX ? 1 : 0)
+ (PT4_QUERY_SUPPORT__RFI_BTN_9MAX ? 1 : 0)
+ (PT4_QUERY_SUPPORT__RFI_CO_9MAX ? 1 : 0)
+ (PT4_QUERY_SUPPORT__RFI_MP_9MAX ? 1 : 0)
+ (PT4_QUERY_SUPPORT__RFI_MP1_9MAX ? 1 : 0)
+ (PT4_QUERY_SUPPORT__RFI_MP2_9MAX ? 1 : 0)
+ (PT4_QUERY_SUPPORT__RFI_EP_9MAX ? 1 : 0)
+ (PT4_QUERY_SUPPORT__RFI_EP1_9MAX ? 1 : 0)
+ (PT4_QUERY_SUPPORT__RFI_SB_9MAX ? 1 : 0)
// OPP
+ (PT4_QUERY_SUPPORT__CBET_FLOP_OPP_IN_3BET_POT ? 1 : 0)
+ (PT4_QUERY_SUPPORT__CBET_FLOP_OPP ? 1 : 0)
+ (PT4_QUERY_SUPPORT__FOLD_TO_3BET_AFTER_R_OPP ? 1 : 0)
+ (PT4_QUERY_SUPPORT__PREFLOP_SQUEEZE_OPP ? 1 : 0)
+ (PT4_QUERY_SUPPORT__FLOP_FOLD_TO_CBET__FLOP_RAISE_CBET_OPP ? 1 : 0)
+ (PT4_QUERY_SUPPORT__FLOP_DONKBET_OPP ? 1 : 0)
+ (PT4_QUERY_SUPPORT__FOLD_TO_F_CBET_IN_3BET_PLUS_POT_OPP ? 1 : 0)
+ (PT4_QUERY_SUPPORT__FOLD_TO_F_CHECK_RAISE_OPP ? 1 : 0)
+ (PT4_QUERY_SUPPORT__FOLD_TO_SQUEEZE_AS_RAISER_OPP ? 1 : 0);

// PokerTracker support
enum PT_Groups {
	pt_group_basic = 1,
	pt_group_advanced = 2,
	pt_group_positional = 3
};

enum PT_UpdateTypes {
	pt_updateType_noUpdate = 0,
	pt_updateType_updateBasic = 1,
	pt_updateType_updateAll = 2
};

enum PT_StatTypes {
	pt_statType_Ring = 0,
	pt_statType_Tourney = 1
};

struct t_QueryDefinition {
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
	// PT4 query to get icon 
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

#if PT4_QUERY_SUPPORT__PFR
	// PT4 
	{
		// name
		"pfr",
		// description_for_editor
		"Pre Flop Raise %",
		// query
		"SELECT (case when ActionOpportunities = 0 then -1 \
				 else cast(ActionCount as real) / ActionOpportunities \
				 end) as result \
		 FROM	(SELECT	sum(CASE WHEN S.cnt_p_raise > 0 THEN 1 ELSE 0 END) \
						 as ActionCount, \
				sum(CASE WHEN LA.action LIKE '__%' OR (LA.action LIKE '_' AND (S.amt_before > (CL.amt_bb + S.amt_ante)) AND (S.amt_p_raise_facing < (S.amt_before - (S.amt_blind + S.amt_ante))) AND (S.flg_p_open_opp OR S.cnt_p_face_limpers > 0 OR S.flg_p_3bet_opp OR S.flg_p_4bet_opp) ) THEN 1 ELSE 0 END) \
						 as ActionOpportunities \
				 FROM	player as P, %TYPE%_hand_player_statistics as S, lookup_actions as LA, cash_limit as CL \
				 WHERE	S.id_player = P.id_player AND \
					LA.id_action = S.id_action_p AND \
					CL.id_limit = S.id_limit AND \
					S.id_gametype = %GAMETYPE% AND \
					P.id_site = %SITEID% AND \
					P.player_name LIKE '%SCREENNAME%') foo",
	// stat_group
	pt_group_advanced
	},
#endif


#if PT4_QUERY_SUPPORT__VPIP 
		// PT4 query to get vpip 
	{
		// name
		"vpip",
		// description_for_editor
		"Poker Tracker VP$IP",
		// query
		"SELECT (case   when ActionOpportunities = 0 then -1 \
             else cast(ActionCount as real) / ActionOpportunities \
             end) as result \
       FROM   (SELECT   sum(case when flg_vpip then 1 else 0 end) \
                   as ActionCount, \
                  (sum(CASE WHEN S.id_hand > 0 THEN 1 ELSE 0 END) - sum(CASE WHEN LA_P.action = '' THEN 1 ELSE 0 END)) \
                   as ActionOpportunities \
             FROM   player as P, %TYPE%_hand_player_statistics as S, lookup_actions as LA_P \
             WHERE   S.id_player = P.id_player AND \
                                                LA_P.id_action = S.id_action_p AND  \
                  S.id_gametype = %GAMETYPE% AND \
                  P.id_site = %SITEID% AND \
                  P.player_name LIKE '%SCREENNAME%') foo",
		// stat_group
		pt_group_basic
	},
#endif

#if PT4_QUERY_SUPPORT__NB_HANDS
	// PT4 query to get number of hands in the database 
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

#if PT4_QUERY_SUPPORT__VPIP_LEGACY
	// PT4 query to get vpip 
	{
		// name
		"vpip_L",
		// description_for_editor
		"Poker Tracker VP$IP Legacy Edition",
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

#if PT4_QUERY_SUPPORT__PFR_LEGACY
	// PT4 query to get pre flop raise 
	{
		// name
		"pfr_L",
		// description_for_editor
		"Poker Tracker pre-flop raise Legacy Edition",
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
	// PT4 query to get pre-flop raise first in
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

#if PT4_QUERY_SUPPORT__RFI_BTN_6MAX
	// PT4 query to get pre-flop raise first in
	{
		// name
		"rfi_btn_6max",
		// description_for_editor
		"Poker Tracker pre-flop raise first in - Position BTN _6max",
		// query
		"SELECT(case when ActionOpportunities = 0 then - 1 \
			 else cast(ActionCount as real) / ActionOpportunities \
			      end) as result \
			FROM(SELECT   sum(case when(s.flg_p_first_raise AND s.flg_p_open_opp) then  1 else  0 end) \
			      as ActionCount, \
			sum((case when(s.flg_p_open_opp) then  1 else  0 end)) \
			      as ActionOpportunities \
			FROM   player as P, %TYPE%_hand_player_statistics as S, lookup_positions, %TYPE%_limit AS TL \
			WHERE   S.id_player = P.id_player AND \
			(lookup_positions.position = S.position  AND lookup_positions.cnt_players = S.cnt_players_lookup_position) AND \
			      lookup_positions.flg_btn AND \
			      TL.id_limit = S.id_limit AND \
			      TL.limit_name LIKE '%(6 max)' AND \
			            S.id_gametype = %GAMETYPE% AND \
						P.id_site = %SITEID% AND \
						P.player_name LIKE '%SCREENNAME%') foo",
	// stat_group
	pt_group_basic
	},
#endif

#if PT4_QUERY_SUPPORT__RFI_CO_6MAX
	// PT4 query to get pre-flop raise first in
	{
		// name
		"rfi_co_6max",
		// description_for_editor
		"Poker Tracker pre-flop raise first in - Position CO _6max",
		// query
		"SELECT(case when ActionOpportunities = 0 then - 1 \
			 else cast(ActionCount as real) / ActionOpportunities \
			      end) as result \
			FROM(SELECT   sum(case when(s.flg_p_first_raise AND s.flg_p_open_opp) then  1 else  0 end) \
			      as ActionCount, \
			sum((case when(s.flg_p_open_opp) then  1 else  0 end)) \
			      as ActionOpportunities \
			FROM   player as P, %TYPE%_hand_player_statistics as S, lookup_positions, %TYPE%_limit AS TL \
			WHERE   S.id_player = P.id_player AND \
			(lookup_positions.position = S.position  AND lookup_positions.cnt_players = S.cnt_players_lookup_position) AND \
			      lookup_positions.flg_co AND \
			      TL.id_limit = S.id_limit AND \
			      TL.limit_name LIKE '%(6 max)' AND \
			            S.id_gametype = %GAMETYPE% AND \
						P.id_site = %SITEID% AND \
						P.player_name LIKE '%SCREENNAME%') foo",
						// stat_group
	                   pt_group_basic
	},
#endif

#if PT4_QUERY_SUPPORT__RFI_MP_6MAX
	// PT4 query to get pre-flop raise first in
	{
		// name
		"rfi_mp_6max",
		// description_for_editor
		"Poker Tracker pre-flop raise first in - Position MP _6max",
		// query
		"SELECT(case when ActionOpportunities = 0 then - 1 \
			 else cast(ActionCount as real) / ActionOpportunities \
			      end) as result \
			FROM(SELECT   sum(case when(s.flg_p_first_raise AND s.flg_p_open_opp) then  1 else  0 end) \
			      as ActionCount, \
			sum((case when(s.flg_p_open_opp) then  1 else  0 end)) \
			      as ActionOpportunities \
			FROM   player as P, %TYPE%_hand_player_statistics as S, lookup_positions, %TYPE%_limit AS TL \
			WHERE   S.id_player = P.id_player AND \
			(lookup_positions.position = S.position  AND lookup_positions.cnt_players = S.cnt_players_lookup_position) AND \
			      lookup_positions.flg_mp AND \
			      TL.id_limit = S.id_limit AND \
			      TL.limit_name LIKE '%(6 max)' AND \
			            S.id_gametype = %GAMETYPE% AND \
						P.id_site = %SITEID% AND \
						P.player_name LIKE '%SCREENNAME%') foo",
						// stat_group
	                    pt_group_basic
	},
#endif

#if PT4_QUERY_SUPPORT__RFI_EP_6MAX
	// PT4 query to get pre-flop raise first in
	{
		// name
		"rfi_ep_6max",
		// description_for_editor
		"Poker Tracker pre-flop raise first in - Position EP _6max",
		// query
		"SELECT(case when ActionOpportunities = 0 then - 1 \
			 else cast(ActionCount as real) / ActionOpportunities \
			      end) as result \
			FROM(SELECT   sum(case when(s.flg_p_first_raise AND s.flg_p_open_opp) then  1 else  0 end) \
			      as ActionCount, \
			sum((case when(s.flg_p_open_opp) then  1 else  0 end)) \
			      as ActionOpportunities \
			FROM   player as P, %TYPE%_hand_player_statistics as S, lookup_positions, %TYPE%_limit AS TL \
			WHERE   S.id_player = P.id_player AND \
			(lookup_positions.position = S.position  AND lookup_positions.cnt_players = S.cnt_players_lookup_position) AND \
			      lookup_positions.flg_ep AND \
			      TL.id_limit = S.id_limit AND \
			      TL.limit_name LIKE '%(6 max)' AND \
			            S.id_gametype = %GAMETYPE% AND \
						P.id_site = %SITEID% AND \
						P.player_name LIKE '%SCREENNAME%') foo",
		      // stat_group
	          pt_group_basic
	},
#endif

#if PT4_QUERY_SUPPORT__RFI_SB_6MAX
	// PT4 query to get pre-flop raise first in
	{
		// name
		"rfi_sb_6max",
		// description_for_editor
		"Poker Tracker pre-flop raise first in - Position SB _6max",
		// query
		"SELECT(case when ActionOpportunities = 0 then - 1 \
			 else cast(ActionCount as real) / ActionOpportunities \
			      end) as result \
			FROM(SELECT   sum(case when(s.flg_p_first_raise AND s.flg_p_open_opp) then  1 else  0 end) \
			      as ActionCount, \
			sum((case when(s.flg_p_open_opp) then  1 else  0 end)) \
			      as ActionOpportunities \
			FROM   player as P, %TYPE%_hand_player_statistics as S, lookup_positions, %TYPE%_limit AS TL \
			WHERE   S.id_player = P.id_player AND \
			(lookup_positions.position = S.position  AND lookup_positions.cnt_players = S.cnt_players_lookup_position) AND \
			      lookup_positions.flg_sb AND \
			      TL.id_limit = S.id_limit AND \
			      TL.limit_name LIKE '%(6 max)' AND \
			            S.id_gametype = %GAMETYPE% AND \
						P.id_site = %SITEID% AND \
						P.player_name LIKE '%SCREENNAME%') foo",
		// stat_group
	pt_group_basic
	},
#endif

#if PT4_QUERY_SUPPORT__RFI_BTN_9MAX
	// PT4 query to get pre-flop raise first in
	{
		// name
		"rfi_btn_9max",
		// description_for_editor
		"Poker Tracker pre-flop raise first in - Position BTN _9max",
		// query
		"SELECT(case when ActionOpportunities = 0 then - 1 \
			 else cast(ActionCount as real) / ActionOpportunities \
			      end) as result \
			FROM(SELECT   sum(case when(s.flg_p_first_raise AND s.flg_p_open_opp) then  1 else  0 end) \
			      as ActionCount, \
			sum((case when(s.flg_p_open_opp) then  1 else  0 end)) \
			      as ActionOpportunities \
			FROM   player as P, %TYPE%_hand_player_statistics as S, lookup_positions, %TYPE%_limit AS TL \
			WHERE   S.id_player = P.id_player AND \
			(lookup_positions.position = S.position  AND lookup_positions.cnt_players = S.cnt_players_lookup_position) AND \
			      lookup_positions.flg_btn AND \
			      TL.id_limit = S.id_limit AND \
			      TL.limit_name NOT LIKE '%(6 max)' AND \
			            S.id_gametype = %GAMETYPE% AND \
						P.id_site = %SITEID% AND \
						P.player_name LIKE '%SCREENNAME%') foo",
						   // stat_group
	                        pt_group_basic
	},
#endif

#if PT4_QUERY_SUPPORT__RFI_CO_9MAX
	// PT4 query to get pre-flop raise first in
	{
		// name
		"rfi_co_9max",
		// description_for_editor
		"Poker Tracker pre-flop raise first in - Position CO _9max",
		// query
		"SELECT(case when ActionOpportunities = 0 then - 1 \
			 else cast(ActionCount as real) / ActionOpportunities \
			      end) as result \
			FROM(SELECT   sum(case when(s.flg_p_first_raise AND s.flg_p_open_opp) then  1 else  0 end) \
			      as ActionCount, \
			sum((case when(s.flg_p_open_opp) then  1 else  0 end)) \
			      as ActionOpportunities \
			FROM   player as P, %TYPE%_hand_player_statistics as S, lookup_positions, %TYPE%_limit AS TL \
			WHERE   S.id_player = P.id_player AND \
			(lookup_positions.position = S.position  AND lookup_positions.cnt_players = S.cnt_players_lookup_position) AND \
			      lookup_positions.flg_co AND \
			      TL.id_limit = S.id_limit AND \
			      TL.limit_name NOT LIKE '%(6 max)' AND \
			            S.id_gametype = %GAMETYPE% AND \
						P.id_site = %SITEID% AND \
						P.player_name LIKE '%SCREENNAME%') foo",
							// stat_group
	                         pt_group_basic
	},
#endif

#if PT4_QUERY_SUPPORT__RFI_MP2_9MAX
	// PT4 query to get pre-flop raise first in
	{
		// name
		"rfi_mp2_9max",
		// description_for_editor
		"Poker Tracker pre-flop raise first in - Position MP2 _9max",
		// query
		"SELECT(case when ActionOpportunities = 0 then - 1 \
			 else cast(ActionCount as real) / ActionOpportunities \
			      end) as result \
			FROM(SELECT   sum(case when(s.flg_p_first_raise AND s.flg_p_open_opp) then  1 else  0 end) \
			      as ActionCount, \
			sum((case when(s.flg_p_open_opp) then  1 else  0 end)) \
			      as ActionOpportunities \
			FROM   player as P, %TYPE%_hand_player_statistics as S, lookup_positions, %TYPE%_limit AS TL \
			WHERE   S.id_player = P.id_player AND \
			(lookup_positions.position = S.position  AND lookup_positions.cnt_players = S.cnt_players_lookup_position) AND \
			      lookup_positions.flg_mp AND \
			      TL.id_limit = S.id_limit AND \
			      TL.limit_name NOT LIKE '%(6 max)' AND \
						S.position = 2 AND \
			            S.id_gametype = %GAMETYPE% AND \
						P.id_site = %SITEID% AND \
						P.player_name LIKE '%SCREENNAME%') foo",
				  // stat_group
	              pt_group_basic
	},
#endif

#if PT4_QUERY_SUPPORT__RFI_MP1_9MAX
	// PT4 query to get pre-flop raise first in
	{
		// name
		"rfi_mp1_9max",
		// description_for_editor
		"Poker Tracker pre-flop raise first in - Position MP1 _9max",
		// query
		"SELECT(case when ActionOpportunities = 0 then - 1 \
			 else cast(ActionCount as real) / ActionOpportunities \
			      end) as result \
			FROM(SELECT   sum(case when(s.flg_p_first_raise AND s.flg_p_open_opp) then  1 else  0 end) \
			      as ActionCount, \
			sum((case when(s.flg_p_open_opp) then  1 else  0 end)) \
			      as ActionOpportunities \
			FROM   player as P, %TYPE%_hand_player_statistics as S, lookup_positions, %TYPE%_limit AS TL \
			WHERE   S.id_player = P.id_player AND \
			(lookup_positions.position = S.position  AND lookup_positions.cnt_players = S.cnt_players_lookup_position) AND \
			      lookup_positions.flg_mp AND \
			      TL.id_limit = S.id_limit AND \
			      TL.limit_name NOT LIKE '%(6 max)' AND \
						S.position = 3 AND \
			            S.id_gametype = %GAMETYPE% AND \
						P.id_site = %SITEID% AND \
						P.player_name LIKE '%SCREENNAME%') foo",
		// stat_group
	    pt_group_basic
	},
#endif

#if PT4_QUERY_SUPPORT__RFI_MP_9MAX
	// PT4 query to get pre-flop raise first in
	{
		// name
		"rfi_mp_9max",
		// description_for_editor
		"Poker Tracker pre-flop raise first in - Position MP _9max",
		// query
		"SELECT(case when ActionOpportunities = 0 then - 1 \
			 else cast(ActionCount as real) / ActionOpportunities \
			      end) as result \
			FROM(SELECT   sum(case when(s.flg_p_first_raise AND s.flg_p_open_opp) then  1 else  0 end) \
			      as ActionCount, \
			sum((case when(s.flg_p_open_opp) then  1 else  0 end)) \
			      as ActionOpportunities \
			FROM   player as P, %TYPE%_hand_player_statistics as S, lookup_positions, %TYPE%_limit AS TL \
			WHERE   S.id_player = P.id_player AND \
			(lookup_positions.position = S.position  AND lookup_positions.cnt_players = S.cnt_players_lookup_position) AND \
			      lookup_positions.flg_mp AND \
			      TL.id_limit = S.id_limit AND \
			      TL.limit_name NOT LIKE '%(6 max)' AND \
						S.position = 4 AND \
			            S.id_gametype = %GAMETYPE% AND \
						P.id_site = %SITEID% AND \
						P.player_name LIKE '%SCREENNAME%') foo",
		// stat_group
	pt_group_basic
	},
#endif

#if PT4_QUERY_SUPPORT__RFI_EP1_9MAX
	// PT4 query to get pre-flop raise first in
	{
		// name
		"rfi_ep1_9max",
		// description_for_editor
		"Poker Tracker pre-flop raise first in - Position EP1 _9max",
		// query
		"SELECT(case when ActionOpportunities = 0 then - 1 \
			 else cast(ActionCount as real) / ActionOpportunities \
			      end) as result \
			FROM(SELECT   sum(case when(s.flg_p_first_raise AND s.flg_p_open_opp) then  1 else  0 end) \
			      as ActionCount, \
			sum((case when(s.flg_p_open_opp) then  1 else  0 end)) \
			      as ActionOpportunities \
			FROM   player as P, %TYPE%_hand_player_statistics as S, lookup_positions, %TYPE%_limit AS TL \
			WHERE   S.id_player = P.id_player AND \
			(lookup_positions.position = S.position  AND lookup_positions.cnt_players = S.cnt_players_lookup_position) AND \
			      lookup_positions.flg_mp AND \
			      TL.id_limit = S.id_limit AND \
			      TL.limit_name NOT LIKE '%(6 max)' AND \
						S.position = 5 AND \
			            S.id_gametype = %GAMETYPE% AND \
						P.id_site = %SITEID% AND \
						P.player_name LIKE '%SCREENNAME%') foo",
			// stat_group
	        pt_group_basic
	},
#endif

#if PT4_QUERY_SUPPORT__RFI_EP_9MAX
	// PT4 query to get pre-flop raise first in
	{
		// name
		"rfi_ep_9max",
		// description_for_editor
		"Poker Tracker pre-flop raise first in - Position EP _9max",
		// query
		"SELECT(case when ActionOpportunities = 0 then - 1 \
			 else cast(ActionCount as real) / ActionOpportunities \
			      end) as result \
			FROM(SELECT   sum(case when(s.flg_p_first_raise AND s.flg_p_open_opp) then  1 else  0 end) \
			      as ActionCount, \
			sum((case when(s.flg_p_open_opp) then  1 else  0 end)) \
			      as ActionOpportunities \
			FROM   player as P, %TYPE%_hand_player_statistics as S, lookup_positions, %TYPE%_limit AS TL \
			WHERE   S.id_player = P.id_player AND \
			(lookup_positions.position = S.position  AND lookup_positions.cnt_players = S.cnt_players_lookup_position) AND \
			      lookup_positions.flg_mp AND \
			      TL.id_limit = S.id_limit AND \
			      TL.limit_name NOT LIKE '%(6 max)' AND \
						S.position = 6 AND \
			            S.id_gametype = %GAMETYPE% AND \
						P.id_site = %SITEID% AND \
						P.player_name LIKE '%SCREENNAME%') foo",
			// stat_group
	        pt_group_basic
	},
#endif

#if PT4_QUERY_SUPPORT__RFI_SB_9MAX
	// PT4 query to get pre-flop raise first in
	{
		// name
		"rfi_sb_9max",
		// description_for_editor
		"Poker Tracker pre-flop raise first in - Position SB _9max",
		// query
		"SELECT(case when ActionOpportunities = 0 then - 1 \
			 else cast(ActionCount as real) / ActionOpportunities \
			      end) as result \
			FROM(SELECT   sum(case when(s.flg_p_first_raise AND s.flg_p_open_opp) then  1 else  0 end) \
			      as ActionCount, \
			sum((case when(s.flg_p_open_opp) then  1 else  0 end)) \
			      as ActionOpportunities \
			FROM   player as P, %TYPE%_hand_player_statistics as S, lookup_positions, %TYPE%_limit AS TL \
			WHERE   S.id_player = P.id_player AND \
			(lookup_positions.position = S.position  AND lookup_positions.cnt_players = S.cnt_players_lookup_position) AND \
			      lookup_positions.flg_mp AND \
			      TL.id_limit = S.id_limit AND \
			      TL.limit_name NOT LIKE '%(6 max)' AND \
						S.position = 9 AND \
			            S.id_gametype = %GAMETYPE% AND \
						P.id_site = %SITEID% AND \
						P.player_name LIKE '%SCREENNAME%') foo",
			// stat_group
	        pt_group_basic
	},
#endif

#if PT4_QUERY_SUPPORT__WTSD
	// PT4 query to get Went to SD 
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
	// PT4 query to get Won $ At SD 
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
	// PT4 query to get attempted to steal 
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
	// PT4 query to get Folded Big Blind to steal 
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
	// PT4 query to get Folded Small Blind to steal 
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
	// PT4 query to get 3B vs. steal 
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
	// PT4 query to get BB 3B vs. steal 
	{
		// name
		"bb_3bet_vs_steal",
		// description_for_editor
		"Poker Tracker BB 3bet vs. steal",
		// query
		"SELECT (case when ActionOpportunities = 0 then -1 \
      else cast(ActionCount as real) / ActionOpportunities \
      end) as result \
    FROM (SELECT   sum(case when (flg_blind_b AND flg_blind_def_opp AND flg_p_3bet) then 1 else 0 end) \
      as ActionCount, \
      sum(case when (flg_blind_b AND flg_blind_def_opp AND flg_p_3bet_opp) then 1 else 0 end) \
      as ActionOpportunities \
    FROM player as P, %TYPE%_hand_player_statistics as S \
    WHERE S.id_player = P.id_player AND \
      S.id_gametype = %GAMETYPE% AND \
      P.id_site = %SITEID% AND \
      P.player_name LIKE '%SCREENNAME%') foo",
	// stat_group
	pt_group_positional
	},
#endif

#if PT4_QUERY_SUPPORT__SB_3B_VS_STEAL
	// PT4 query to get SB 3B vs. steal 
	{
		// name
		"sb_3bet_vs_steal",
		// description_for_editor
		"Poker Tracker SB 3bet vs. steal",
		// query
		"SELECT (case when ActionOpportunities = 0 then -1 \
      else cast(ActionCount as real) / ActionOpportunities \
      end) as result \
    FROM (SELECT sum(case when (flg_blind_s AND flg_blind_def_opp AND flg_p_3bet) then 1 else 0 end) \
      as ActionCount, \
      sum(case when (flg_blind_s AND flg_blind_def_opp AND flg_p_3bet_opp) then 1 else 0 end) \
      as ActionOpportunities \
    FROM   player as P, %TYPE%_hand_player_statistics as S \
    WHERE   S.id_player = P.id_player AND \
      S.id_gametype = %GAMETYPE% AND \
      P.id_site = %SITEID% AND \
      P.player_name LIKE '%SCREENNAME%') foo",
	// stat_group
	pt_group_positional
	},
#endif

#if PT4_QUERY_SUPPORT__PREFLOP_CALLED_RAISE
	// PT4 query to get pre-flop called raise 
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
	// PT4  query to get PREFLOP 3 bet 
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
	// PT4  query to get PREFLOP fold to 3 bet 
	{
		// name
		"fold_to_3bet",
		// description_for_editor
		"Poker Tracker folded while facing 3bet preflop",
		// query
		"SELECT (case when ActionOpportunities = 0 then -1 \
				 else cast(ActionCount as real) / ActionOpportunities \
				 end) as result \
		 FROM	(SELECT	sum(case when(s.enum_p_3bet_action='F') then  1 else  0 end) \
						 as ActionCount, \
						sum(case when(s.flg_p_3bet_def_opp) then  1 else  0 end) \
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

#if PT4_QUERY_SUPPORT__PREFLOP_FOLD_TO_3B_AFTER_RAISE
	// PT4  query to get PREFLOP fold to 3 bet 
	{
		// name
		"fold_to_3bet_afterR",
		// description_for_editor
		"Poker Tracker folded while facing 3bet preflop ... after raise",
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
	// PT4 query to get Fold to Resteal 
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
	// PT4 query to get Preflop 4B 
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
	// PT4 query to get Preflop fold to 4B 
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

#if PT4_QUERY_SUPPORT__PREFLOP_SQUEEZE
	// PT4 
	{
		// name
		"preflop_squeeze",
		// description_for_editor
		"Squeeze Preflop %",
		// query
		"SELECT (case when ActionOpportunities = 0 then -1 \
				 else cast(ActionCount as real) / ActionOpportunities \
				 end) as result \
		 FROM	(SELECT	sum(CASE WHEN S.flg_p_squeeze THEN 1 ELSE 0 END) \
						 as ActionCount, \
				sum(CASE WHEN S.flg_p_squeeze_opp THEN 1 ELSE 0 END) \
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

#if PT4_QUERY_SUPPORT__FLOP_AGGRESSION_FACTOR
	// PT4 query to get flop aggression factor 
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
	// PT4 query to get turn aggression factor 
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
	// PT4 query to get river aggression factor 
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
	// PT4 query to get total aggression factor without preflop 
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
	// PT4 query to get flop aggression frequency 
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
	// PT4 query to get turn aggression frequency 
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
	// PT4 query to get river aggression frequency 
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
	// PT4 query to get total aggression frequency without preflop 
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
	// PT4 query to get flops seen 
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
	// PT4 query to get turns seen 
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
	// PT4 query to get rivers seen 
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
	// PT4  query to get cbet on FLOP 
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
	// PT4  query to get FLOP fold to cbet 
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
	// PT4  query to get raise cbet on FLOP 
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
	// PT4  query to get cbet on TURN 
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
	// PT4  query to get TURN fold to cbet 
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
	// PT4  query to get RIVER fold to cbet 
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
	// PT4  query to get total fold to 3 bet without preflop
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
	// PT4  query to get FLOP fold to 3 bet 
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
	// PT4  query to get flop check-raise 
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
	// PT4  query to get FLOP donk 
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
	// PT4  query to get flop float 
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
	// PT4  query to get TURN fold to 3 bet 
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
	// PT4  query to get TURN Check-Raise 
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
	// PT4  query to get TURN Check-Call 
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
	// PT4  query to get RIVER fold to 3 bet 
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
	// PT4  query to get River bet 
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

#if PT4_QUERY_SUPPORT__4BET_PLUS_RATIO
	// PT4 
	{
		// name
		"4bet_plus_ratio",
		// description_for_editor
		"Poker Tracker 4bet + ratio",
		// query
		"SELECT (cast(sum(case when S.cnt_p_raise > 0 then 1 else 0 end) as decimal) / \
		(cast(sum(case when S.id_hand > 0 then 1 else 0 end) as decimal) - cast(sum(case when LA.action = '' then 1 else 0 end) as decimal))) * \
		(cast(sum(case when S.flg_p_first_raise and S.flg_p_4bet then 1 else 0 end) as decimal) / \
			cast(sum(case when S.flg_p_4bet_opp AND S.flg_p_first_raise then 1 else 0 end) as decimal)) as result \
			FROM	player as P, %TYPE%_hand_player_statistics as S, lookup_actions as LA \
			WHERE	S.id_player = P.id_player AND \
			LA.id_action = S.id_action_p AND \
			S.id_gametype = %GAMETYPE% AND \
			P.id_site = %SITEID% AND \
			P.player_name LIKE '%SCREENNAME%'",
	// stat_group
	pt_group_advanced
	},
#endif

#if PT4_QUERY_SUPPORT__FOLD_TO_PF_SQUEEZE
	// PT4 
	{
		// name
		"fold_to_pf_squeeze",
		// description_for_editor
		"Poker Tracker fold to pf squeeze",
		// query
		"SELECT ( cast(sum(case when S.flg_p_squeeze_def_opp AND S.enum_p_squeeze_action='F' then 1 else 0 end) as decimal) / \
		cast(sum(case when S.flg_p_squeeze_def_opp then 1 else 0 end) as decimal) ) as result \
		FROM	player as P, %TYPE%_hand_player_statistics as S \
			WHERE	S.id_player = P.id_player AND \
			S.id_gametype = %GAMETYPE% AND \
			P.id_site = %SITEID% AND \
			P.player_name LIKE '%SCREENNAME%'",
	// stat_group
	pt_group_advanced
	},
#endif

#if PT4_QUERY_SUPPORT__FOLD_TO_SQUEEZE_AS_RAISER
	// PT4 
	{
		// name
		"fold_to_squeeze_as_raiser",
		// description_for_editor
		"Poker Tracker fold to squeeze as raiser",
		// query
		"SELECT ( cast(sum(case when S.flg_p_squeeze_def_opp AND S.enum_p_squeeze_action='F' AND S.flg_p_first_raise then 1 else 0 end) as decimal) / \
		cast(sum(case when S.flg_p_squeeze_def_opp AND S.flg_p_first_raise then 1 else 0 end) as decimal) ) as result \
		FROM	player as P, %TYPE%_hand_player_statistics as S \
			WHERE	S.id_player = P.id_player AND \
			S.id_gametype = %GAMETYPE% AND \
			P.id_site = %SITEID% AND \
			P.player_name LIKE '%SCREENNAME%'",
	// stat_group
	pt_group_advanced
	},
#endif

#if PT4_QUERY_SUPPORT__FOLD_TO_SQUEEZE_AS_CALLER
	// PT4 
	{
		// name
		"fold_to_squeeze_as_caller",
		// description_for_editor
		"Poker Tracker fold to squeeze as caller",
		// query
		"SELECT ( cast(sum(case when S.flg_p_squeeze_def_opp AND S.enum_p_squeeze_action='F' AND NOT S.flg_p_first_raise then 1 else 0 end) as decimal) / \
		cast(sum(case when S.flg_p_squeeze_def_opp AND NOT S.flg_p_first_raise then 1 else 0 end) as decimal) ) as result \
		FROM	player as P, %TYPE%_hand_player_statistics as S \
			WHERE	S.id_player = P.id_player AND \
			S.id_gametype = %GAMETYPE% AND \
			P.id_site = %SITEID% AND \
			P.player_name LIKE '%SCREENNAME%'",
	// stat_group
	pt_group_advanced
	},
#endif

#if PT4_QUERY_SUPPORT__DONK_FLOP_IN_3BET_PLUS_POT
	// PT4 
	{
		// name
		"donk_flop_in_3bet_plus_pot",
		// description_for_editor
		"Flop Donk Bet in 3Bet+ Pot %",
		// query
		"SELECT (case when ActionOpportunities = 0 then -1 \
				 else cast(ActionCount as real) / ActionOpportunities \
				 end) as result \
		FROM (SELECT sum(CASE WHEN(S.flg_p_3bet_def_opp OR S.flg_p_4bet_def_opp) AND NOT(S.flg_p_3bet OR S.flg_p_4bet) AND S.flg_f_bet AND NOT(S.flg_f_cbet_opp) AND((cash_hand_summary.cnt_players > 2 AND S.val_p_raise_aggressor_pos < S.position) or (cash_hand_summary.cnt_players = 2 and S.flg_blind_b)) THEN 1 ELSE 0 END) \
				 as ActionCount, \
				 sum(CASE WHEN(S.flg_p_3bet_def_opp OR S.flg_p_4bet_def_opp) AND NOT(S.flg_p_3bet OR S.flg_p_4bet) AND(S.flg_f_open_opp) AND NOT(S.flg_f_cbet_opp) AND((cash_hand_summary.cnt_players > 2 AND S.val_p_raise_aggressor_pos < S.position) or (cash_hand_summary.cnt_players = 2 AND S.flg_blind_b)) THEN 1 ELSE 0 END) \
				 as ActionOpportunities \
			FROM	player as P, cash_hand_player_statistics as S, cash_hand_summary \
			WHERE	S.id_player = P.id_player AND \
				 cash_hand_summary.id_hand = S.id_hand AND \
				 S.id_gametype = %GAMETYPE% AND \
				 P.id_site = %SITEID% AND \
				 P.player_name LIKE '%SCREENNAME%') foo",
	// stat_group
	pt_group_advanced
	},
#endif

#if PT4_QUERY_SUPPORT__FOLD_TO_F_CBET_IN_3BET_POT
	// PT4 
	{
		// name
		"fold_to_f_cbet_in_3bet_pot",
		// description_for_editor
		"Fold to Flop Continuation Bet % in 3+Bet Pot",
		// query
		"SELECT (case when ActionOpportunities = 0 then -1 \
				 else cast(ActionCount as real) / ActionOpportunities \
				 end) as result \
		 FROM	(SELECT	sum(CASE WHEN (S.flg_p_3bet_def_opp OR S.flg_p_4bet_def_opp) AND S.enum_f_cbet_action='F' THEN 1 ELSE 0 END) \
						 as ActionCount, \
				sum(CASE WHEN (S.flg_p_3bet_def_opp OR S.flg_p_4bet_def_opp) AND S.flg_f_cbet_def_opp THEN 1 ELSE 0 END) \
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

#if PT4_QUERY_SUPPORT__CBET_FLOP_IN_3BET_POT
	// PT4 
	{
		// name
		"cbet_flop_in_3bet_pot",
		// description_for_editor
		"Flop Continuation Bet % (3Bet Pot)",
		// query
		"SELECT (case when ActionOpportunities = 0 then -1 \
				 else cast(ActionCount as real) / ActionOpportunities \
				 end) as result \
		 FROM	(SELECT	sum(CASE WHEN S.flg_f_cbet AND (S.flg_p_3bet or S.flg_p_4bet) THEN 1 ELSE 0 END) \
						 as ActionCount, \
				sum(CASE WHEN S.flg_f_cbet_opp AND (S.flg_p_3bet or S.flg_p_4bet) THEN 1 ELSE 0 END) \
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

#if PT4_QUERY_SUPPORT__FLOP_CHECK_RAISE_IN_3BET_POT
	// PT4 
	{
		// name
		"flop_check_raise_in_3bet_pot",
		// description_for_editor
		"Poker Tracker flop check-raise in 3Bet Pot %",
		// query
		"SELECT (case when ActionOpportunities = 0 then -1 \
				 else cast(ActionCount as real) / ActionOpportunities \
				 end) as result \
		 FROM	(SELECT	sum(CASE WHEN S.flg_f_check_raise AND S.flg_p_3bet THEN 1 ELSE 0 END) \
						 as ActionCount, \
				sum(CASE WHEN S.flg_f_check AND S.flg_p_3bet AND (S.cnt_f_raise > 0 OR S.cnt_f_call > 0 OR S.flg_f_fold) THEN 1 ELSE 0 END) \
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

#if PT4_QUERY_SUPPORT__FLOP_CHECK_RAISE_IN_3BET_PLUS_POT
	// PT4 
	{
		// name
		"flop_check_raise_in_3bet_plus_pot",
		// description_for_editor
		"Poker Tracker flop check-raise in 3Bet+ Pot %",
		// query
		"SELECT (case when ActionOpportunities = 0 then -1 \
				 else cast(ActionCount as real) / ActionOpportunities \
				 end) as result \
		 FROM	(SELECT	sum(CASE WHEN S.flg_f_check_raise AND (S.flg_p_3bet or S.flg_p_4bet) THEN 1 ELSE 0 END) \
						 as ActionCount, \
				sum(CASE WHEN S.flg_f_check AND (S.flg_p_3bet or S.flg_p_4bet) AND (S.cnt_f_raise > 0 OR S.cnt_f_call > 0 OR S.flg_f_fold) THEN 1 ELSE 0 END) \
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

#if PT4_QUERY_SUPPORT__FOLD_TO_F_CHECK_RAISE
	// PT4 
	{
		// name
		"fold_to_f_check_raise",
		// description_for_editor
		"Fold Flop Check Raise %",
		// query
		"SELECT (case when ActionOpportunities = 0 then -1 \
				 else cast(ActionCount as real) / ActionOpportunities \
				 end) as result \
		 FROM	(SELECT	sum(CASE WHEN substring(LA.action from 2 for 1) = 'F' AND S.flg_f_bet AND S.flg_f_face_raise AND ((cash_hand_summary.cnt_players > 2 AND substring(cash_hand_summary.str_aggressors_f from 2 for 1)::int > S.position) OR (cash_hand_summary.cnt_players = 2 AND S.flg_blind_s)) THEN 1 ELSE 0 END) \
						 as ActionCount, \
				sum(CASE WHEN S.flg_f_bet AND S.flg_f_face_raise AND ((cash_hand_summary.cnt_players > 2 AND substring(cash_hand_summary.str_aggressors_f from 2 for 1)::int > S.position) OR (cash_hand_summary.cnt_players = 2 AND S.flg_blind_s)) THEN 1 ELSE 0 END) \
						 as ActionOpportunities \
				 FROM	player as P, %TYPE%_hand_player_statistics as S, cash_hand_summary, lookup_actions as LA \
				 WHERE	S.id_player = P.id_player AND \
					cash_hand_summary.id_hand = S.id_hand AND \
					LA.id_action = S.id_action_f AND \
					S.id_gametype = %GAMETYPE% AND \
					P.id_site = %SITEID% AND \
					P.player_name LIKE '%SCREENNAME%') foo",
	// stat_group
	pt_group_advanced
	},
#endif

#if PT4_QUERY_SUPPORT__FOLD_TO_F_CHECK_RAISE_3B_PLUS_POT
	// PT4 
	{
		// name
		"fold_to_f_check_raise_3b_plus_pot",
		// description_for_editor
		"Fold Flop Check Raise % in 3Bet+ Pot",
		// query
		"SELECT (case when ActionOpportunities = 0 then -1 \
				 else cast(ActionCount as real) / ActionOpportunities \
				 end) as result \
		 FROM	(SELECT	sum(CASE WHEN substring(LA.action from 2 for 1) = 'F' AND S.flg_f_bet AND S.flg_f_face_raise AND ((cash_hand_summary.cnt_players > 2 AND substring(cash_hand_summary.str_aggressors_f from 2 for 1)::int > S.position) OR (cash_hand_summary.cnt_players = 2 AND S.flg_blind_s)) AND (S.flg_p_3bet or S.flg_p_4bet) THEN 1 ELSE 0 END) \
						 as ActionCount, \
				sum(CASE WHEN S.flg_f_bet AND S.flg_f_face_raise AND ((cash_hand_summary.cnt_players > 2 AND substring(cash_hand_summary.str_aggressors_f from 2 for 1)::int > S.position) OR (cash_hand_summary.cnt_players = 2 AND S.flg_blind_s)) AND (S.flg_p_3bet or S.flg_p_4bet) THEN 1 ELSE 0 END) \
						 as ActionOpportunities \
				 FROM	player as P, %TYPE%_hand_player_statistics as S, cash_hand_summary, lookup_actions as LA \
				 WHERE	S.id_player = P.id_player AND \
					cash_hand_summary.id_hand = S.id_hand AND \
					LA.id_action = S.id_action_f AND \
					S.id_gametype = %GAMETYPE% AND \
					P.id_site = %SITEID% AND \
					P.player_name LIKE '%SCREENNAME%') foo",
	// stat_group
	pt_group_advanced
	},
#endif

#if PT4_QUERY_SUPPORT__FLOAT_FLOP
	// PT4 
	{
		// name
		"float_flop",
		// description_for_editor
		"Flop Float %",
		// query
		"SELECT (case when ActionOpportunities = 0 then -1 \
				 else cast(ActionCount as real) / ActionOpportunities \
				 end) as result \
		 FROM	(SELECT	sum(CASE WHEN (LA.action = 'C' OR LA.action = 'CC') AND S.flg_p_face_raise AND S.flg_f_bet AND char_length(cash_hand_summary.str_aggressors_p) = 2 AND ((cash_hand_summary.cnt_players > 2 AND substring(cash_hand_summary.str_aggressors_p from 2 for 1)::int > S.position) OR (cash_hand_summary.cnt_players = 2 AND S.flg_f_has_position)) THEN 1 ELSE 0 END) \
						 as ActionCount, \
				sum(CASE WHEN (LA.action = 'C' OR LA.action = 'CC') AND S.flg_p_face_raise AND S.flg_f_open_opp AND char_length(cash_hand_summary.str_aggressors_p) = 2 AND ((cash_hand_summary.cnt_players > 2 AND substring(cash_hand_summary.str_aggressors_p from 2 for 1)::int > S.position) OR (cash_hand_summary.cnt_players = 2 AND S.flg_f_has_position)) THEN 1 ELSE 0 END) \
						 as ActionOpportunities \
				 FROM	player as P, %TYPE%_hand_player_statistics as S, cash_hand_summary, lookup_actions as LA \
				 WHERE	S.id_player = P.id_player AND \
					cash_hand_summary.id_hand = S.id_hand AND \
					LA.id_action = S.id_action_p AND \
					S.id_gametype = %GAMETYPE% AND \
					P.id_site = %SITEID% AND \
					P.player_name LIKE '%SCREENNAME%') foo",
	// stat_group
	pt_group_advanced
	},
#endif

#if PT4_QUERY_SUPPORT__FLOAT_FLOP_IN_3BET_POT
	// PT4 
	{
		// name
		"float_flop_in_3bet_pot",
		// description_for_editor
		"Flop Float in 3Bet Pot %",
		// query
		"SELECT (case when ActionOpportunities = 0 then -1 \
				 else cast(ActionCount as real) / ActionOpportunities \
				 end) as result \
		 FROM	(SELECT	sum(CASE WHEN S.flg_p_first_raise and LA.action = 'RC' and S.flg_f_has_position and S.flg_f_bet THEN 1 ELSE 0 END) \
						 as ActionCount, \
				sum(CASE WHEN S.flg_p_first_raise and LA.action = 'RC' and S.flg_f_has_position and S.flg_f_open_opp THEN 1 ELSE 0 END) \
						 as ActionOpportunities \
				 FROM	player as P, %TYPE%_hand_player_statistics as S, lookup_actions as LA \
				 WHERE	S.id_player = P.id_player AND \
					LA.id_action = S.id_action_p AND \
					S.id_gametype = %GAMETYPE% AND \
					P.id_site = %SITEID% AND \
					P.player_name LIKE '%SCREENNAME%') foo",
	// stat_group
	pt_group_advanced
	},
#endif

#if PT4_QUERY_SUPPORT__CALL_F_FLOAT_BET
	// PT4 
	{
		// name
		"call_f_float_bet",
		// description_for_editor
		"Call Flop Float Bet %",
		// query
		"SELECT (case when ActionOpportunities = 0 then -1 \
				 else cast(ActionCount as real) / ActionOpportunities \
				 end) as result \
		 FROM	(SELECT	sum(CASE WHEN not(S.flg_p_face_raise) AND LA_P.action LIKE '%R' and S.flg_f_open_opp AND S.amt_f_bet_facing > 0 AND substring(LA_F.action from 1 for 2) = 'XC' THEN 1 ELSE 0 END) \
						 as ActionCount, \
				sum(CASE WHEN not(S.flg_p_face_raise) AND LA_P.action LIKE '%R' AND S.flg_f_open_opp AND S.flg_f_check AND S.amt_f_bet_facing > 0 THEN 1 ELSE 0 END) \
						 as ActionOpportunities \
				 FROM	player as P, %TYPE%_hand_player_statistics as S, lookup_actions as LA_P, lookup_actions as LA_F \
				 WHERE	S.id_player = P.id_player AND \
					LA_P.id_action = S.id_action_p AND \
					LA_F.id_action = S.id_action_f AND \
					S.id_gametype = %GAMETYPE% AND \
					P.id_site = %SITEID% AND \
					P.player_name LIKE '%SCREENNAME%') foo",
	// stat_group
	pt_group_advanced
	},
#endif

#if PT4_QUERY_SUPPORT__CALL_F_FLOAT_BET_IN_3B_POT
	// PT4 
	{
		// name
		"call_f_float_bet_in_3b_pot",
		// description_for_editor
		"Call Flop Float Bet in 3Bet Pot %",
		// query
		"SELECT (case when ActionOpportunities = 0 then -1 \
				 else cast(ActionCount as real) / ActionOpportunities \
				 end) as result \
		 FROM	(SELECT	sum(CASE WHEN not(S.flg_p_face_raise) AND LA_P.action LIKE '%R' and S.flg_f_open_opp AND S.amt_f_bet_facing > 0 AND substring(LA_F.action from 1 for 2) = 'XC' AND S.flg_p_3bet THEN 1 ELSE 0 END) \
						 as ActionCount, \
				sum(CASE WHEN not(S.flg_p_face_raise) AND LA_P.action LIKE '%R' AND S.flg_f_open_opp AND S.flg_f_check AND S.amt_f_bet_facing > 0 AND S.flg_p_3bet THEN 1 ELSE 0 END) \
						 as ActionOpportunities \
				 FROM	player as P, %TYPE%_hand_player_statistics as S, lookup_actions as LA_P, lookup_actions as LA_F \
				 WHERE	S.id_player = P.id_player AND \
					LA_P.id_action = S.id_action_p AND \
					LA_F.id_action = S.id_action_f AND \
					S.id_gametype = %GAMETYPE% AND \
					P.id_site = %SITEID% AND \
					P.player_name LIKE '%SCREENNAME%') foo",
	// stat_group
	pt_group_advanced
	},
#endif

#if PT4_QUERY_SUPPORT__FOLD_TO_F_FLOAT_BET_IN_3B_POT
	// PT4 
	{
		// name
		"fold_to_f_float_bet_in_3b_pot",
		// description_for_editor
		"Fold to Flop Float Bet in 3Bet Pot %",
		// query
		"SELECT (case when ActionOpportunities = 0 then -1 \
				 else cast(ActionCount as real) / ActionOpportunities \
				 end) as result \
		 FROM	(SELECT	sum(CASE WHEN not(S.flg_p_face_raise) AND LA_P.action LIKE '%R' and S.flg_f_open_opp AND S.amt_f_bet_facing > 0 AND substring(LA_F.action from 1 for 2) = 'XF' AND S.flg_p_3bet THEN 1 ELSE 0 END) \
						 as ActionCount, \
				sum(CASE WHEN not(S.flg_p_face_raise) AND LA_P.action LIKE '%R' AND S.flg_f_open_opp AND S.flg_f_check AND S.amt_f_bet_facing > 0 AND S.flg_p_3bet THEN 1 ELSE 0 END) \
						 as ActionOpportunities \
				 FROM	player as P, %TYPE%_hand_player_statistics as S, lookup_actions as LA_P, lookup_actions as LA_F \
				 WHERE	S.id_player = P.id_player AND \
					LA_P.id_action = S.id_action_p AND \
					LA_F.id_action = S.id_action_f AND \
					S.id_gametype = %GAMETYPE% AND \
					P.id_site = %SITEID% AND \
					P.player_name LIKE '%SCREENNAME%') foo",
	// stat_group
	pt_group_advanced
	},
#endif

#if PT4_QUERY_SUPPORT__FOLD_TO_F_FLOAT_BET_IN_3B_PLUS_POT
	// PT4 
	{
		// name
		"fold_to_f_float_bet_in_3b_plus_pot",
		// description_for_editor
		"Fold to Flop Float Bet in 3Bet+ Pot %",
		// query
		"SELECT (case when ActionOpportunities = 0 then -1 \
				 else cast(ActionCount as real) / ActionOpportunities \
				 end) as result \
		 FROM	(SELECT	sum(CASE WHEN not(S.flg_p_face_raise) AND LA_P.action LIKE '%R' and S.flg_f_open_opp AND S.amt_f_bet_facing > 0 AND substring(LA_F.action from 1 for 2) = 'XF' AND (S.flg_p_3bet or S.flg_p_4bet) THEN 1 ELSE 0 END) \
						 as ActionCount, \
				sum(CASE WHEN not(S.flg_p_face_raise) AND LA_P.action LIKE '%R' AND S.flg_f_open_opp AND S.flg_f_check AND S.amt_f_bet_facing > 0 AND (S.flg_p_3bet or S.flg_p_4bet) THEN 1 ELSE 0 END) \
						 as ActionOpportunities \
				 FROM	player as P, %TYPE%_hand_player_statistics as S, lookup_actions as LA_P, lookup_actions as LA_F \
				 WHERE	S.id_player = P.id_player AND \
					LA_P.id_action = S.id_action_p AND \
					LA_F.id_action = S.id_action_f AND \
					S.id_gametype = %GAMETYPE% AND \
					P.id_site = %SITEID% AND \
					P.player_name LIKE '%SCREENNAME%') foo",
	// stat_group
	pt_group_advanced
	},
#endif

#if PT4_QUERY_SUPPORT__CBET_FLOP_OPP_IN_3BET_POT
	// PT4 
	{
		// name
		"cbet_flop_opp_in_3bet_pot",
		// description_for_editor
		"Cbet Flop Opportunities in 3bet + Pot",
		// query
		"SELECT sum(CASE WHEN S.flg_f_cbet_opp AND (S.flg_p_3bet or S.flg_p_4bet) THEN 1 ELSE 0 END) as result \
		 FROM	player as P, %TYPE%_hand_player_statistics as S \
				 WHERE	S.id_player = P.id_player AND \
					S.id_gametype = %GAMETYPE% AND \
					P.id_site = %SITEID% AND \
					P.player_name LIKE '%SCREENNAME%'",
	// stat_group
	pt_group_advanced
	},
#endif

#if PT4_QUERY_SUPPORT__CBET_FLOP_OPP
	// PT4 
	{
		// name
		"cbet_flop_opp",
		// description_for_editor
		"Cbet Flop Opportunities",
		// query
		"SELECT sum(CASE WHEN S.flg_f_cbet_opp THEN 1 ELSE 0 END) as result \
		FROM	player as P, %TYPE%_hand_player_statistics as S \
				 WHERE	S.id_player = P.id_player AND \
					S.id_gametype = %GAMETYPE% AND \
					P.id_site = %SITEID% AND \
					P.player_name LIKE '%SCREENNAME%'",
	// stat_group
	pt_group_advanced
	},
#endif

#if PT4_QUERY_SUPPORT__FOLD_TO_3BET_AFTER_R_OPP
	// PT4 
	{
		// name
		"fold_to_3bet_afterR_opp",
		// description_for_editor
		"Poker Tracker folded while facing 3bet preflop Opportunities after raise",
		// query
		"SELECT sum(CASE WHEN(s.flg_p_3bet_def_opp AND s.flg_p_first_raise) THEN 1 ELSE 0 END) as result \
		FROM	player as P, %TYPE%_hand_player_statistics as S \
				 WHERE	S.id_player = P.id_player AND \
					S.id_gametype = %GAMETYPE% AND \
					P.id_site = %SITEID% AND \
					P.player_name LIKE '%SCREENNAME%'",
	// stat_group
	pt_group_advanced
	},
#endif

#if PT4_QUERY_SUPPORT__PREFLOP_SQUEEZE_OPP
	// PT4 
	{
		// name
		"preflop_squeeze_opp",
		// description_for_editor
		"Squeeze Preflop % Opportunities",
		// query
		"SELECT sum(CASE WHEN S.flg_p_squeeze_opp THEN 1 ELSE 0 END) as result \
		FROM	player as P, %TYPE%_hand_player_statistics as S \
				 WHERE	S.id_player = P.id_player AND \
					S.id_gametype = %GAMETYPE% AND \
					P.id_site = %SITEID% AND \
					P.player_name LIKE '%SCREENNAME%'",
	// stat_group
	pt_group_advanced
	},
#endif

#if PT4_QUERY_SUPPORT__FLOP_FOLD_TO_CBET__FLOP_RAISE_CBET_OPP
	// PT4 
	{
		// name
		"flop_fold_to_cbet__flop_raise_cbet_opp",
		// description_for_editor
		"Opportunities for 2 stats Flop R Cbet and Flop fold to cbet",
		// query
		"SELECT sum(CASE WHEN S.flg_f_cbet_def_opp THEN 1 ELSE 0 END) as result \
		FROM	player as P, %TYPE%_hand_player_statistics as S \
				 WHERE	S.id_player = P.id_player AND \
					S.id_gametype = %GAMETYPE% AND \
					P.id_site = %SITEID% AND \
					P.player_name LIKE '%SCREENNAME%'",
	// stat_group
	pt_group_advanced
	},
#endif

#if PT4_QUERY_SUPPORT__FLOP_DONKBET_OPP
	// PT4 
	{
		// name
		"flop_donkbet_opp",
		// description_for_editor
		"Poker Tracker donk flop Opportunities",
		// query
		"SELECT sum(case when (S.flg_p_face_raise AND NOT(S.flg_p_3bet OR S.flg_p_4bet) \
			AND(S.flg_f_open_opp) AND NOT(S.flg_f_cbet_opp) AND \
			((HSum.cnt_players > 2 and S.val_p_raise_aggressor_pos < S.position) or \
			(HSum.cnt_players = 2 and S.flg_blind_b))) then 1 else 0 end) as result \
		FROM	player as P, %TYPE%_hand_player_statistics as S, cash_hand_summary as HSum \
				 WHERE	S.id_player = P.id_player AND \
					HSum.id_hand = S.id_hand AND \
					S.id_gametype = %GAMETYPE% AND \
					P.id_site = %SITEID% AND \
					P.player_name LIKE '%SCREENNAME%'",
	// stat_group
	pt_group_advanced
	},
#endif

#if PT4_QUERY_SUPPORT__FOLD_TO_F_CBET_IN_3BET_PLUS_POT_OPP
	// PT4 
	{
		// name
		"fold_to_f_cbet_in_3bet_plus_pot_opp",
		// description_for_editor
		"Fold to Flop Continuation Bet % in 3+Bet Pot Opportunities",
		// query
		"SELECT sum(CASE WHEN (S.flg_p_3bet_def_opp OR S.flg_p_4bet_def_opp) AND S.flg_f_cbet_def_opp THEN 1 ELSE 0 END) as result \
		FROM	player as P, %TYPE%_hand_player_statistics as S \
				 WHERE	S.id_player = P.id_player AND \
					S.id_gametype = %GAMETYPE% AND \
					P.id_site = %SITEID% AND \
					P.player_name LIKE '%SCREENNAME%'",
	// stat_group
	pt_group_advanced
	},
#endif

#if PT4_QUERY_SUPPORT__FOLD_TO_F_CHECK_RAISE_OPP
	// PT4 
	{
		// name
		"fold_to_f_check_raise_opp",
		// description_for_editor
		"Fold to Flop Ckeck raise Opportunities",
		// query
		"SELECT sum(CASE WHEN S.flg_f_bet AND S.flg_f_face_raise AND ((cash_hand_summary.cnt_players > 2 AND substring(cash_hand_summary.str_aggressors_f from 2 for 1)::int > S.position) OR (cash_hand_summary.cnt_players = 2 AND S.flg_blind_s)) THEN 1 ELSE 0 END) as result \
		FROM	player as P, %TYPE%_hand_player_statistics as S , cash_hand_summary \
				 WHERE	S.id_player = P.id_player AND \
					cash_hand_summary.id_hand = S.id_hand AND \
					S.id_gametype = %GAMETYPE% AND \
					P.id_site = %SITEID% AND \
					P.player_name LIKE '%SCREENNAME%'",
	// stat_group
	pt_group_advanced
	},
#endif

#if PT4_QUERY_SUPPORT__FOLD_TO_SQUEEZE_AS_RAISER_OPP
	// PT4 
	{
		// name
		"fold_to_squeeze_as_raiser_opp",
		// description_for_editor
		"Fold to Squeeze As Raiser Opportunities",
		// query
		"SELECT cast(sum(case when S.flg_p_squeeze_def_opp AND S.flg_p_first_raise then 1 else 0 end) as decimal) as result \
		FROM	player as P, %TYPE%_hand_player_statistics as S \
				 WHERE	S.id_player = P.id_player AND \
					S.id_gametype = %GAMETYPE% AND \
					P.id_site = %SITEID% AND \
					P.player_name LIKE '%SCREENNAME%'",
	// stat_group
	pt_group_advanced
	},
#endif

};