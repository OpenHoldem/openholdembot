#pragma once
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
// ICON disabled, as not supported by FPDB.
// New VPIP and PFR are NOW supported 
// We keep old way for these 2 stats with name conversion as Legacy
// Now we support an incredible number of statistics 
// As far as possible, every statistic in its name and in its description, is faithful to what is reported in FPDB that now is 100% supported
// Nothing from the old source has been eliminated 
// Only added and something renamed
// A special thank you to the whole community, which made this possible 
// We are Maxinmontreal 
// https://ptb4.pokertracker.com/forums/viewtopic.php?f=59&t=49035
// http://www.maxinmontreal.com/forums/viewforum.php?f=273
// http://www.maxinmontreal.com/forums/viewtopic.php?f=273&t=20933
// http://www.maxinmontreal.com/forums/viewtopic.php?f=273&t=21561

#define FPDB_QUERY_SUPPORT__PFR							(TRUE)	// "pfr aka pt4"
#define FPDB_QUERY_SUPPORT__VPIP							(TRUE)	// "vpip aka pt4"
#define FPDB_QUERY_SUPPORT__ICON							(FALSE) // "icon"
#define FPDB_QUERY_SUPPORT__NB_HANDS					(TRUE)	// "hands"
#define FPDB_QUERY_SUPPORT__VPIP_LEGACY			(TRUE)	// "vpip aka pt3"
#define FPDB_QUERY_SUPPORT__PFR_LEGACY				(TRUE)	// "pfr aka pt3"
#define FPDB_QUERY_SUPPORT__RFI							(TRUE)	// "rfi"
#define FPDB_QUERY_SUPPORT__WTSD							(TRUE)	// "wtsd"
#define FPDB_QUERY_SUPPORT__WSSD							(TRUE)	// "wssd"

// PREFLOP STATS
#define FPDB_QUERY_SUPPORT__STEAL_ATTEMPT				  (TRUE)	// "steal_attempt"
#define FPDB_QUERY_SUPPORT__FOLD_BB_TO_STEAL				(TRUE)	// "bb_fold_to_steal"
#define FPDB_QUERY_SUPPORT__FOLD_SB_TO_STEAL				(TRUE)	// "sb_fold_to_steal"
#define FPDB_QUERY_SUPPORT__3B_VS_STEAL					  (TRUE)	// "3bet_vs_steal"
#define FPDB_QUERY_SUPPORT__BB_3B_VS_STEAL				  (TRUE)	// "bb_3bet_vs_steal"
#define FPDB_QUERY_SUPPORT__SB_3B_VS_STEAL				  (TRUE)	// "sb_3bet_vs_steal"
#define FPDB_QUERY_SUPPORT__PREFLOP_CALLED_RAISE		(TRUE)	// "preflop_called_raise"
#define FPDB_QUERY_SUPPORT__PREFLOP_3B					    (TRUE)	// "3bet"
#define FPDB_QUERY_SUPPORT__PREFLOP_FOLD_TO_3B			(TRUE)	// "fold_to_3bet"
#define FPDB_QUERY_SUPPORT__PREFLOP_FOLD_TO_3B_AFTER_RAISE			(TRUE)	// "fold_to_3bet"
#define FPDB_QUERY_SUPPORT__PF_FOLD_TO_RESTEAL			(TRUE)	// "fold_to_resteal"
#define FPDB_QUERY_SUPPORT__PREFLOP_4B					    (TRUE)	// "4bet"
#define FPDB_QUERY_SUPPORT__PREFLOP_FOLD_TO_4B			(TRUE)	// "fold_to_4bet"
#define FPDB_QUERY_SUPPORT__4BET_PLUS_RATIO     		(TRUE)	// ""
#define FPDB_QUERY_SUPPORT__FOLD_TO_PF_SQUEEZE     (TRUE)	// ""
#define FPDB_QUERY_SUPPORT__FOLD_TO_SQUEEZE_AS_RAISER  		(TRUE)	// ""
#define FPDB_QUERY_SUPPORT__FOLD_TO_SQUEEZE_AS_CALLER  		(TRUE)	// ""
#define FPDB_QUERY_SUPPORT__PREFLOP_SQUEEZE     		(TRUE)	// ""
#define FPDB_QUERY_SUPPORT__FOLD_TO_PF_4BET_AFTER_3BET     (TRUE)	// ""

// AF
#define FPDB_QUERY_SUPPORT__FLOP_AGGRESSION_FACTOR		(TRUE)	// "flop_af"
#define FPDB_QUERY_SUPPORT__TURN_AGGRESSION_FACTOR		(TRUE)	// "turn_af"
#define FPDB_QUERY_SUPPORT__RIVER_AGGRESSION_FACTOR	(TRUE)	// "river_af"
#define FPDB_QUERY_SUPPORT__TOTAL_AGGRESSION_FACTOR	(TRUE)	// "total_af"

// AFq
#define FPDB_QUERY_SUPPORT__FLOP_AGGRESSION_FREQUENCY	(TRUE)	// "flop_afq" 
#define FPDB_QUERY_SUPPORT__TURN_AGGRESSION_FREQUENCY	(TRUE)	// "turn_afq" 
#define FPDB_QUERY_SUPPORT__RIVER_AGGRESSION_FREQUENCY	(TRUE)	// "river_afq" 
#define FPDB_QUERY_SUPPORT__TOTAL_AGGRESSION_FREQUENCY	(TRUE)	// "total_afq" 

// Street seen
#define FPDB_QUERY_SUPPORT__SEEN_FLOP_PCT			  	(TRUE)	// "flop_seen"
#define FPDB_QUERY_SUPPORT__SEEN_TURN_PCT			  	(TRUE)	// "turn_seen"
#define FPDB_QUERY_SUPPORT__SEEN_RIVER_PCT			  	(TRUE)	// "river_seen"

// Continuation bet // not filtred /// action for all type of pre flop game /// AKA SRP 3BET AND 3BET+
// Flop
#define FPDB_QUERY_SUPPORT__FLOP_CBET					  (TRUE)	// "flop_cbet"
#define FPDB_QUERY_SUPPORT__FLOP_FOLD_TO_CBET	  (TRUE)	// "flop_fold_to_cbet"
#define FPDB_QUERY_SUPPORT__FLOP_RAISE_CBET		  (TRUE)	// "flop_raise_cbet"
// Turn
#define FPDB_QUERY_SUPPORT__TURN_CBET					  (TRUE)	// "turn_cbet"
#define FPDB_QUERY_SUPPORT__TURN_FOLD_TO_CBET	  (TRUE)	// "turn_fold_to_cbet"
// River
#define FPDB_QUERY_SUPPORT__RIVER_FOLD_TO_CBET	  (TRUE)	// "river_fold_to_cbet"

// POSTFLOP Other stats
#define FPDB_QUERY_SUPPORT__POSTFLOP_FOLD_TO_3B	(TRUE)	// "postflop_fold_to_3bet"

// FLOP Other stats
#define FPDB_QUERY_SUPPORT__FLOP_FOLD_TO_3B			(TRUE)	// "flop_fold_to_3bet"   // 3b is a reraise, we should have a "flop_fold_to_raise" stat first.
#define FPDB_QUERY_SUPPORT__FLOP_CHECK_RAISE			(TRUE)	// "flop_checkraise"
#define FPDB_QUERY_SUPPORT__FLOP_DONKBET					(TRUE)	// "flop_donkbet"
#define FPDB_QUERY_SUPPORT__FLOP_FLOAT     			(TRUE)	// "flop_float" (FPDB's stat for: bet IP flop vs missed cbet)
#define FPDB_QUERY_SUPPORT__FOLD_TO_F_CHECK_RAISE     				(TRUE)	// ""
#define FPDB_QUERY_SUPPORT__FLOAT_FLOP     				(TRUE)	// ""
#define FPDB_QUERY_SUPPORT__CALL_F_FLOAT_BET     				(TRUE)	// ""

// TURN Other stats
#define FPDB_QUERY_SUPPORT__TURN_FOLD_TO_3B			(TRUE)	// "turn_fold_to_3bet" // 3b is a reraise, we should have a "turn_fold_to_raise" stat first.
#define FPDB_QUERY_SUPPORT__TURN_CHECK_RAISE			(TRUE)	// "turn_checkraise"
#define FPDB_QUERY_SUPPORT__TURN_CHECK_CALL			(TRUE)	// "turn_checkcall"

// RIVER Other stats
#define FPDB_QUERY_SUPPORT__RIVER_FOLD_TO_3B			(TRUE)	// "river_fold_to_3bet" // 3b is a reraise, we should have a "river_fold_to_raise" stat first.
#define FPDB_QUERY_SUPPORT__RIVER_BET     				(TRUE)	// "river_bet"

// FLOP - 3BET Pots AND 3BET + Pots
#define FPDB_QUERY_SUPPORT__DONK_FLOP_IN_3BET_PLUS_POT     				(TRUE)	// ""
#define FPDB_QUERY_SUPPORT__FOLD_TO_F_CBET_IN_3BET_POT     				(TRUE)	// ""
#define FPDB_QUERY_SUPPORT__CBET_FLOP_IN_3BET_POT     				(TRUE)	// ""
#define FPDB_QUERY_SUPPORT__FLOP_CHECK_RAISE_IN_3BET_POT     				(TRUE)	// ""
#define FPDB_QUERY_SUPPORT__FLOP_CHECK_RAISE_IN_3BET_PLUS_POT     				(TRUE)	// ""
#define FPDB_QUERY_SUPPORT__FOLD_TO_F_CHECK_RAISE_3B_PLUS_POT     				(TRUE)	// ""
#define FPDB_QUERY_SUPPORT__FLOAT_FLOP_IN_3BET_POT     				(TRUE)	// ""
#define FPDB_QUERY_SUPPORT__CALL_F_FLOAT_BET_IN_3B_POT     				(TRUE)	// ""
#define FPDB_QUERY_SUPPORT__FOLD_TO_F_FLOAT_BET_IN_3B_POT     				(TRUE)	// ""
#define FPDB_QUERY_SUPPORT__FOLD_TO_F_FLOAT_BET_IN_3B_PLUS_POT     				(TRUE)	// ""

// Advanced RFI - Positional Pre Flop - 6 MAX AND 9 MAX now supported
#define FPDB_QUERY_SUPPORT__RFI_BTN_6MAX     				(TRUE)	// ""
#define FPDB_QUERY_SUPPORT__RFI_CO_6MAX     				(TRUE)	// ""
#define FPDB_QUERY_SUPPORT__RFI_MP_6MAX     				(TRUE)	// ""
#define FPDB_QUERY_SUPPORT__RFI_EP_6MAX     				(TRUE)	// ""
#define FPDB_QUERY_SUPPORT__RFI_SB_6MAX     				(TRUE)	// ""

#define FPDB_QUERY_SUPPORT__RFI_BTN_9MAX     				(TRUE)	// ""
#define FPDB_QUERY_SUPPORT__RFI_CO_9MAX     				(TRUE)	// ""
#define FPDB_QUERY_SUPPORT__RFI_MP_9MAX     				(TRUE)	// ""
#define FPDB_QUERY_SUPPORT__RFI_MP1_9MAX     				(TRUE)	// ""
#define FPDB_QUERY_SUPPORT__RFI_MP2_9MAX     				(TRUE)	// ""
#define FPDB_QUERY_SUPPORT__RFI_EP_9MAX     				(TRUE)	// ""
#define FPDB_QUERY_SUPPORT__RFI_EP1_9MAX     				(TRUE)	// ""
#define FPDB_QUERY_SUPPORT__RFI_SB_9MAX     				(TRUE)	// ""

// Advanced OPPORTUNITIES /// 
#define FPDB_QUERY_SUPPORT__CBET_FLOP_OPP_IN_3BET_POT     				(TRUE)	// ""
#define FPDB_QUERY_SUPPORT__CBET_FLOP_OPP     				(TRUE)	// ""
#define FPDB_QUERY_SUPPORT__FOLD_TO_3BET_AFTER_R_OPP     				(TRUE)	// ""
#define FPDB_QUERY_SUPPORT__PREFLOP_SQUEEZE_OPP   				(TRUE)	// ""
#define FPDB_QUERY_SUPPORT__FLOP_FOLD_TO_CBET__FLOP_RAISE_CBET_OPP     				(TRUE)	// ""
#define FPDB_QUERY_SUPPORT__FLOP_DONKBET_OPP    				(TRUE)	// ""
#define FPDB_QUERY_SUPPORT__FOLD_TO_F_CBET_IN_3BET_PLUS_POT_OPP     				(TRUE)	// ""
#define FPDB_QUERY_SUPPORT__FOLD_TO_F_CHECK_RAISE_OPP     				(TRUE)	// ""
#define FPDB_QUERY_SUPPORT__FOLD_TO_SQUEEZE_AS_RAISER_OPP     				(TRUE)	// ""

const int k_number_of_fpdb_stats =  //GENERAL STATS
(FPDB_QUERY_SUPPORT__ICON ? 1 : 0)
+ (FPDB_QUERY_SUPPORT__NB_HANDS ? 1 : 0)
+ (FPDB_QUERY_SUPPORT__PFR ? 1 : 0)
+ (FPDB_QUERY_SUPPORT__VPIP ? 1 : 0)
+ (FPDB_QUERY_SUPPORT__VPIP_LEGACY ? 1 : 0)
+ (FPDB_QUERY_SUPPORT__PFR_LEGACY ? 1 : 0)
+ (FPDB_QUERY_SUPPORT__RFI ? 1 : 0)
+ (FPDB_QUERY_SUPPORT__WTSD ? 1 : 0)
+ (FPDB_QUERY_SUPPORT__WSSD ? 1 : 0)
//PREFLOP STATS
+ (FPDB_QUERY_SUPPORT__STEAL_ATTEMPT ? 1 : 0)
+ (FPDB_QUERY_SUPPORT__FOLD_BB_TO_STEAL ? 1 : 0)
+ (FPDB_QUERY_SUPPORT__FOLD_SB_TO_STEAL ? 1 : 0)
+ (FPDB_QUERY_SUPPORT__3B_VS_STEAL ? 1 : 0)
+ (FPDB_QUERY_SUPPORT__BB_3B_VS_STEAL ? 1 : 0)
+ (FPDB_QUERY_SUPPORT__SB_3B_VS_STEAL ? 1 : 0)
+ (FPDB_QUERY_SUPPORT__PREFLOP_CALLED_RAISE ? 1 : 0)
+ (FPDB_QUERY_SUPPORT__PREFLOP_3B ? 1 : 0)
+ (FPDB_QUERY_SUPPORT__PREFLOP_FOLD_TO_3B ? 1 : 0)
+ (FPDB_QUERY_SUPPORT__PREFLOP_FOLD_TO_3B_AFTER_RAISE ? 1 : 0)
+ (FPDB_QUERY_SUPPORT__PF_FOLD_TO_RESTEAL ? 1 : 0)
+ (FPDB_QUERY_SUPPORT__PREFLOP_4B ? 1 : 0)
+ (FPDB_QUERY_SUPPORT__PREFLOP_FOLD_TO_4B ? 1 : 0)
//  AF
+ (FPDB_QUERY_SUPPORT__FLOP_AGGRESSION_FACTOR ? 1 : 0)
+ (FPDB_QUERY_SUPPORT__TURN_AGGRESSION_FACTOR ? 1 : 0)
+ (FPDB_QUERY_SUPPORT__RIVER_AGGRESSION_FACTOR ? 1 : 0)
+ (FPDB_QUERY_SUPPORT__TOTAL_AGGRESSION_FACTOR ? 1 : 0)
//  AFq
+ (FPDB_QUERY_SUPPORT__FLOP_AGGRESSION_FREQUENCY ? 1 : 0)
+ (FPDB_QUERY_SUPPORT__TURN_AGGRESSION_FREQUENCY ? 1 : 0)
+ (FPDB_QUERY_SUPPORT__RIVER_AGGRESSION_FREQUENCY ? 1 : 0)
+ (FPDB_QUERY_SUPPORT__TOTAL_AGGRESSION_FREQUENCY ? 1 : 0)
// POSTFLOP stats
+ (FPDB_QUERY_SUPPORT__POSTFLOP_FOLD_TO_3B ? 1 : 0)
//  Street seen
+ (FPDB_QUERY_SUPPORT__SEEN_FLOP_PCT ? 1 : 0)
+ (FPDB_QUERY_SUPPORT__SEEN_TURN_PCT ? 1 : 0)
+ (FPDB_QUERY_SUPPORT__SEEN_RIVER_PCT ? 1 : 0)
//  Continuation bet
//  Flop
+ (FPDB_QUERY_SUPPORT__FLOP_CBET ? 1 : 0)
+ (FPDB_QUERY_SUPPORT__FLOP_FOLD_TO_CBET ? 1 : 0)
+ (FPDB_QUERY_SUPPORT__FLOP_RAISE_CBET ? 1 : 0)
//  Turn
+ (FPDB_QUERY_SUPPORT__TURN_CBET ? 1 : 0)
+ (FPDB_QUERY_SUPPORT__TURN_FOLD_TO_CBET ? 1 : 0)
//  River
+ (FPDB_QUERY_SUPPORT__RIVER_FOLD_TO_CBET ? 1 : 0)
//  Flop Other stats
+ (FPDB_QUERY_SUPPORT__FLOP_FOLD_TO_3B ? 1 : 0)
+ (FPDB_QUERY_SUPPORT__FLOP_CHECK_RAISE ? 1 : 0)
+ (FPDB_QUERY_SUPPORT__FLOP_DONKBET ? 1 : 0)
+ (FPDB_QUERY_SUPPORT__FLOP_FLOAT ? 1 : 0)
//  Turn Other stats
+ (FPDB_QUERY_SUPPORT__TURN_FOLD_TO_3B ? 1 : 0)
+ (FPDB_QUERY_SUPPORT__TURN_CHECK_RAISE ? 1 : 0)
+ (FPDB_QUERY_SUPPORT__TURN_CHECK_CALL ? 1 : 0)
//  River Other stats
+ (FPDB_QUERY_SUPPORT__RIVER_FOLD_TO_3B ? 1 : 0)
+ (FPDB_QUERY_SUPPORT__RIVER_BET ? 1 : 0)
//  Varie
+ (FPDB_QUERY_SUPPORT__4BET_PLUS_RATIO ? 1 : 0)
+ (FPDB_QUERY_SUPPORT__FOLD_TO_PF_SQUEEZE ? 1 : 0)
+ (FPDB_QUERY_SUPPORT__FOLD_TO_SQUEEZE_AS_RAISER ? 1 : 0)
+ (FPDB_QUERY_SUPPORT__FOLD_TO_SQUEEZE_AS_CALLER ? 1 : 0)
+ (FPDB_QUERY_SUPPORT__FOLD_TO_F_CHECK_RAISE ? 1 : 0)
+ (FPDB_QUERY_SUPPORT__FLOAT_FLOP ? 1 : 0)
+ (FPDB_QUERY_SUPPORT__CALL_F_FLOAT_BET ? 1 : 0)
+ (FPDB_QUERY_SUPPORT__PREFLOP_SQUEEZE ? 1 : 0)
//  3bet and 3bet+
+ (FPDB_QUERY_SUPPORT__DONK_FLOP_IN_3BET_PLUS_POT ? 1 : 0)
+ (FPDB_QUERY_SUPPORT__FOLD_TO_F_CBET_IN_3BET_POT ? 1 : 0)
+ (FPDB_QUERY_SUPPORT__CBET_FLOP_IN_3BET_POT ? 1 : 0)
+ (FPDB_QUERY_SUPPORT__FLOP_CHECK_RAISE_IN_3BET_POT ? 1 : 0)
+ (FPDB_QUERY_SUPPORT__FLOP_CHECK_RAISE_IN_3BET_PLUS_POT ? 1 : 0)
+ (FPDB_QUERY_SUPPORT__FOLD_TO_F_CHECK_RAISE_3B_PLUS_POT ? 1 : 0)
+ (FPDB_QUERY_SUPPORT__FLOAT_FLOP_IN_3BET_POT ? 1 : 0)
+ (FPDB_QUERY_SUPPORT__CALL_F_FLOAT_BET_IN_3B_POT ? 1 : 0)
+ (FPDB_QUERY_SUPPORT__FOLD_TO_F_FLOAT_BET_IN_3B_POT ? 1 : 0)
+ (FPDB_QUERY_SUPPORT__FOLD_TO_F_FLOAT_BET_IN_3B_PLUS_POT ? 1 : 0)
//	Positional	
+ (FPDB_QUERY_SUPPORT__RFI_BTN_6MAX ? 1 : 0)
+ (FPDB_QUERY_SUPPORT__RFI_CO_6MAX ? 1 : 0)
+ (FPDB_QUERY_SUPPORT__RFI_MP_6MAX ? 1 : 0)
+ (FPDB_QUERY_SUPPORT__RFI_EP_6MAX ? 1 : 0)
+ (FPDB_QUERY_SUPPORT__RFI_SB_6MAX ? 1 : 0)
+ (FPDB_QUERY_SUPPORT__RFI_BTN_9MAX ? 1 : 0)
+ (FPDB_QUERY_SUPPORT__RFI_CO_9MAX ? 1 : 0)
+ (FPDB_QUERY_SUPPORT__RFI_MP_9MAX ? 1 : 0)
+ (FPDB_QUERY_SUPPORT__RFI_MP1_9MAX ? 1 : 0)
+ (FPDB_QUERY_SUPPORT__RFI_MP2_9MAX ? 1 : 0)
+ (FPDB_QUERY_SUPPORT__RFI_EP_9MAX ? 1 : 0)
+ (FPDB_QUERY_SUPPORT__RFI_EP1_9MAX ? 1 : 0)
+ (FPDB_QUERY_SUPPORT__RFI_SB_9MAX ? 1 : 0)
// OPP
+ (FPDB_QUERY_SUPPORT__CBET_FLOP_OPP_IN_3BET_POT ? 1 : 0)
+ (FPDB_QUERY_SUPPORT__CBET_FLOP_OPP ? 1 : 0)
+ (FPDB_QUERY_SUPPORT__FOLD_TO_3BET_AFTER_R_OPP ? 1 : 0)
+ (FPDB_QUERY_SUPPORT__PREFLOP_SQUEEZE_OPP ? 1 : 0)
+ (FPDB_QUERY_SUPPORT__FLOP_FOLD_TO_CBET__FLOP_RAISE_CBET_OPP ? 1 : 0)
+ (FPDB_QUERY_SUPPORT__FLOP_DONKBET_OPP ? 1 : 0)
+ (FPDB_QUERY_SUPPORT__FOLD_TO_F_CBET_IN_3BET_PLUS_POT_OPP ? 1 : 0)
+ (FPDB_QUERY_SUPPORT__FOLD_TO_F_CHECK_RAISE_OPP ? 1 : 0)
+ (FPDB_QUERY_SUPPORT__FOLD_TO_SQUEEZE_AS_RAISER_OPP ? 1 : 0);

// Fpdb support
enum FPDB_Groups {
	fpdb_group_basic = 1,
	fpdb_group_advanced = 2,
	fpdb_group_positional = 3
};

enum FPDB_UpdateTypes {
	fpdb_updateType_noUpdate = 0,
	fpdb_updateType_updateBasic = 1,
	fpdb_updateType_updateAll = 2
};

enum FPDB_StatTypes {
	fpdb_statType_Ring = 0,
	fpdb_statType_Tourney = 1
};

struct t_QueryDefinition {
	CString name;
	CString description_for_editor;
	CString query;
	int stat_group;
};

// Fpdb naming conventions: 
//   http://www.fpdb.com/guides/FPDB3/tutorials/using-custom-reports-and-statistics#Naming-conventions
t_QueryDefinition query_definitions[k_number_of_fpdb_stats] =
{
#if FPDB_QUERY_SUPPORT__ICON
	// FPDB query to get icon 
	{
		// name
		"icon",
		// description_for_editor
		"FPDB auto-rate icon code",
		// query
		"SELECT comment as result \
		 FROM   Players \
		 WHERE  siteId = %SITEID% AND \
		        name LIKE '%SCREENNAME%'",
	// stat_group
	fpdb_group_basic
	},
#endif

#if FPDB_QUERY_SUPPORT__PFR
	// FPDB 
	{
		// name
		"pfr",
		// description_for_editor
		"Pre Flop Raise %",
		// query
		"SELECT	(cast(sum(ActionCount)as real) / sum(ActionOpportunities)) as result \
			FROM	(SELECT(case when street0Aggr then 1 else 0 end) \
					as ActionCount, \
					(case when street0AggrChance then 1 else 0 end) \
					as ActionOpportunities \
			FROM	HandsPlayers as HP, Hands as H, Players as P, Gametypes as G \
					%TOURNEYTYPESELECT% \
			WHERE	H.startTime > date('now','-%TIMEPERIOD%') AND \
					%LIMIT&STAGESTATEMENT% \
					%TOURNEYTABLETYPE% \
					%POSITIONSTATEMENT% \
					HP.handId = H.id AND \
					H.gametypeId = G.id AND \
					G.type = '%GAMETYPE%' AND \
					G.category Like '%GAMECAT%' AND \
					HP.playerId = P.id AND \
					P.siteId = %SITEID% AND \
					P.name Like '%SCREENNAME%' \
				ORDER BY H.startTime DESC \
				LIMIT %NBHANDS% \
			)foo; ",
	// stat_group
	fpdb_group_advanced
	},
#endif


#if FPDB_QUERY_SUPPORT__VPIP 
		// FPDB query to get vpip 
	{
		// name
		"vpip",
		// description_for_editor
		"FPDB VP$IP",
		// query
		"SELECT	(cast(sum(ActionCount)as real) / sum(ActionOpportunities)) as result \
			FROM	(SELECT(case when street0VPI then 1 else 0 end) \
					as ActionCount, \
					(case when street0VPIChance then 1 else 0 end) \
					as ActionOpportunities \
			FROM	HandsPlayers as HP, Hands as H, Players as P, Gametypes as G \
					%TOURNEYTYPESELECT% \
			WHERE	H.startTime > date('now','-%TIMEPERIOD%') AND \
					%LIMIT&STAGESTATEMENT% \
					%TOURNEYTABLETYPE% \
					%POSITIONSTATEMENT% \
					HP.handId = H.id AND \
					H.gametypeId = G.id AND \
					G.type = '%GAMETYPE%' AND \
					G.category Like '%GAMECAT%' AND \
					HP.playerId = P.id AND \
					P.siteId = %SITEID% AND \
					P.name Like '%SCREENNAME%' \
				ORDER BY H.startTime DESC \
				LIMIT %NBHANDS% \
			)foo; ",
		// stat_group
		fpdb_group_basic
	},
#endif

#if FPDB_QUERY_SUPPORT__NB_HANDS
	// FPDB query to get number of hands in the database 
	{
		// name
		"hands",
		// description_for_editor
		"FPDB number of hands that are in the database",
		// query
		"SELECT	count(*) as result \
			FROM	HandsPlayers as HP, Hands as H, Players as P, Gametypes as G \
					%TOURNEYTYPESELECT% \
			WHERE	H.startTime > date('now','-%TIMEPERIOD%') AND \
					%LIMIT&STAGESTATEMENT% \
					%TOURNEYTABLETYPE% \
					%POSITIONSTATEMENT% \
					HP.handId = H.id AND \
					H.gametypeId = G.id AND \
					G.type = '%GAMETYPE%' AND \
					G.category Like '%GAMECAT%' AND \
					HP.playerId = P.id AND \
					P.siteId = %SITEID% AND \
					P.name Like '%SCREENNAME%' \
		ORDER BY count(H.startTime) DESC \
		LIMIT %NBHANDS% \
		",
	// stat_group
	fpdb_group_basic
	},
#endif

#if FPDB_QUERY_SUPPORT__VPIP_LEGACY
	// FPDB query to get vpip 
	{
		// name
		"vpip_L",
		// description_for_editor
		"FPDB VP$IP Legacy Edition",
		// query
		"SELECT	sum(street0VPI)*1.0/sum(street0VPIChance)*1.0 as result \
		 FROM	HandsPlayers as HP, Hands as H, Players as P, Gametypes as G \
		 WHERE	HP.handId = H.id AND \
				H.gametypeId = G.id AND \
				G.type = '%GAMETYPE%' AND \
				G.category Like '%GAMECAT%' AND \
				HP.playerId = P.id AND \
				P.siteId = %SITEID% AND \
				P.name Like '%SCREENNAME%' \
		ORDER BY count(H.startTime) DESC \
		LIMIT %NBHANDS% \
		",
	// stat_group
	fpdb_group_basic
	},
#endif

#if FPDB_QUERY_SUPPORT__PFR_LEGACY
	// FPDB query to get pre flop raise 
	{
		// name
		"pfr_L",
		// description_for_editor
		"FPDB pre-flop raise Legacy Edition",
		// query
		"SELECT	sum(street0Aggr)*1.0/sum(street0AggrChance)*1.0 as result \
		 FROM	HandsPlayers as HP, Hands as H, Players as P, Gametypes as G \
		 WHERE	HP.handId = H.id AND \
				H.gametypeId = G.id AND \
				G.type = '%GAMETYPE%' AND \
				G.category Like '%GAMECAT%' AND \
				HP.playerId = P.id AND \
				P.siteId = %SITEID% AND \
				P.name Like '%SCREENNAME%' \
		ORDER BY count(H.startTime) DESC \
		LIMIT %NBHANDS% \
		",
	// stat_group
	fpdb_group_basic
	},
#endif

#if FPDB_QUERY_SUPPORT__RFI
	// FPDB query to get pre-flop raise first in
	{
		// name
		"rfi",
		// description_for_editor
		"FPDB pre-flop raise first in",
		// query
		"SELECT	(cast(sum(ActionCount)as real) / sum(ActionOpportunities)) as result \
			FROM	(SELECT(case when raisedFirstIn then 1 else 0 end) \
					as ActionCount, \
					(case when raiseFirstInChance then 1 else 0 end) \
					as ActionOpportunities \
			FROM	HandsPlayers as HP, Hands as H, Players as P, Gametypes as G \
					%TOURNEYTYPESELECT% \
			WHERE	H.startTime > date('now','-%TIMEPERIOD%') AND \
					%LIMIT&STAGESTATEMENT% \
					%TOURNEYTABLETYPE% \
					%POSITIONSTATEMENT% \
					HP.handId = H.id AND \
					H.gametypeId = G.id AND \
					G.type = '%GAMETYPE%' AND \
					G.category Like '%GAMECAT%' AND \
					HP.playerId = P.id AND \
					P.siteId = %SITEID% AND \
					P.name Like '%SCREENNAME%' \
				ORDER BY H.startTime DESC \
				LIMIT %NBHANDS% \
			)foo; ",
	// stat_group
	fpdb_group_basic
	},
#endif

#if FPDB_QUERY_SUPPORT__WTSD
	// FPDB query to get Went to SD 
	{
		// name
		"wtsd",
		// description_for_editor
		"FPDB went to showdown",
		// query
		"SELECT	(cast(sum(ActionCount)as real) / sum(ActionOpportunities)) as result \
			FROM	(SELECT(case when sawShowdown then 1 else 0 end) \
					as ActionCount, \
					(case when street1Seen then 1 else 0 end) \
					as ActionOpportunities \
			FROM	HandsPlayers as HP, Hands as H, Players as P, Gametypes as G \
					%TOURNEYTYPESELECT% \
			WHERE	H.startTime > date('now','-%TIMEPERIOD%') AND \
					%LIMIT&STAGESTATEMENT% \
					%TOURNEYTABLETYPE% \
					%POSITIONSTATEMENT% \
					HP.handId = H.id AND \
					H.gametypeId = G.id AND \
					G.type = '%GAMETYPE%' AND \
					G.category Like '%GAMECAT%' AND \
					HP.playerId = P.id AND \
					P.siteId = %SITEID% AND \
					P.name Like '%SCREENNAME%' \
				ORDER BY H.startTime DESC \
				LIMIT %NBHANDS% \
			)foo; ",
	// stat_group
	fpdb_group_basic
	},
#endif

#if FPDB_QUERY_SUPPORT__WSSD
	// FPDB query to get Won $ At SD 
	{
		// name
		"wssd",
		// description_for_editor
		"FPDB won $ at showdown",
		// query
		"SELECT	(cast(sum(ActionCount)as real) / sum(ActionOpportunities)) as result \
			FROM	(SELECT(case when HP.sawShowdown and HP.wonAtSD then 1 else 0 end) \
					as ActionCount, \
					(case when HP.sawShowdown then 1 else 0 end) \
					as ActionOpportunities \
			FROM	HandsPlayers as HP, Hands as H, Players as P, Gametypes as G \
					%TOURNEYTYPESELECT% \
			WHERE	H.startTime > date('now','-%TIMEPERIOD%') AND \
					%LIMIT&STAGESTATEMENT% \
					%TOURNEYTABLETYPE% \
					%POSITIONSTATEMENT% \
					HP.handId = H.id AND \
					H.gametypeId = G.id AND \
					G.type = '%GAMETYPE%' AND \
					G.category Like '%GAMECAT%' AND \
					HP.playerId = P.id AND \
					P.siteId = %SITEID% AND \
					P.name Like '%SCREENNAME%' \
				ORDER BY H.startTime DESC \
				LIMIT %NBHANDS% \
			)foo; ",
	// stat_group
	fpdb_group_basic
	},
#endif

#if FPDB_QUERY_SUPPORT__STEAL_ATTEMPT
	// FPDB query to get attempted to steal 
	{
		// name
		"steal_attempt",
		// description_for_editor
		"FPDB attempt to steal blinds",
		// query
		"SELECT	(cast(sum(ActionCount)as real) / sum(ActionOpportunities)) as result \
			FROM	(SELECT(case when stealDone then 1 else 0 end) \
					as ActionCount, \
					(case when stealChance then 1 else 0 end) \
					as ActionOpportunities \
			FROM	HandsPlayers as HP, Hands as H, Players as P, Gametypes as G \
					%TOURNEYTYPESELECT% \
			WHERE	H.startTime > date('now','-%TIMEPERIOD%') AND \
					%LIMIT&STAGESTATEMENT% \
					%TOURNEYTABLETYPE% \
					%POSITIONSTATEMENT% \
					HP.handId = H.id AND \
					H.gametypeId = G.id AND \
					G.type = '%GAMETYPE%' AND \
					G.category Like '%GAMECAT%' AND \
					HP.playerId = P.id AND \
					P.siteId = %SITEID% AND \
					P.name Like '%SCREENNAME%' \
				ORDER BY H.startTime DESC \
				LIMIT %NBHANDS% \
			)foo; ",
	// stat_group
	fpdb_group_advanced
	},
#endif

#if FPDB_QUERY_SUPPORT__FOLD_BB_TO_STEAL
	// FPDB query to get Folded Big Blind to steal 
	{
		// name
		"bb_fold_to_steal",
		// description_for_editor
		"FPDB folded big blind to steal",
		// query
		"SELECT	(cast(sum(ActionCount)as real) / sum(ActionOpportunities)) as result \
			FROM	(SELECT(case when foldedBbToSteal then 1 else 0 end) \
					as ActionCount, \
					(case when foldBbToStealChance then 1 else 0 end) \
					as ActionOpportunities \
			FROM	HandsPlayers as HP, Hands as H, Players as P, Gametypes as G \
					%TOURNEYTYPESELECT% \
			WHERE	H.startTime > date('now','-%TIMEPERIOD%') AND \
					%LIMIT&STAGESTATEMENT% \
					%TOURNEYTABLETYPE% \
					%POSITIONSTATEMENT% \
					HP.handId = H.id AND \
					H.gametypeId = G.id AND \
					G.type = '%GAMETYPE%' AND \
					G.category Like '%GAMECAT%' AND \
					HP.playerId = P.id AND \
					P.siteId = %SITEID% AND \
					P.name Like '%SCREENNAME%' \
				ORDER BY H.startTime DESC \
				LIMIT %NBHANDS% \
			)foo; ",
	// stat_group
	fpdb_group_positional
	},
#endif

#if FPDB_QUERY_SUPPORT__FOLD_SB_TO_STEAL
	// FPDB query to get Folded Small Blind to steal 
	{
		// name
		"sb_fold_to_steal",
		// description_for_editor
		"FPDB folded small blind to steal",
		// query
		"SELECT	(cast(sum(ActionCount)as real) / sum(ActionOpportunities)) as result \
			FROM	(SELECT(case when foldedSbToSteal then 1 else 0 end) \
					as ActionCount, \
					(case when foldSbToStealChance then 1 else 0 end) \
					as ActionOpportunities \
			FROM	HandsPlayers as HP, Hands as H, Players as P, Gametypes as G \
					%TOURNEYTYPESELECT% \
			WHERE	H.startTime > date('now','-%TIMEPERIOD%') AND \
					%LIMIT&STAGESTATEMENT% \
					%TOURNEYTABLETYPE% \
					%POSITIONSTATEMENT% \
					HP.handId = H.id AND \
					H.gametypeId = G.id AND \
					G.type = '%GAMETYPE%' AND \
					G.category Like '%GAMECAT%' AND \
					HP.playerId = P.id AND \
					P.siteId = %SITEID% AND \
					P.name Like '%SCREENNAME%' \
				ORDER BY H.startTime DESC \
				LIMIT %NBHANDS% \
			)foo; ",
	// stat_group
	fpdb_group_positional
	},
#endif

#if FPDB_QUERY_SUPPORT__3B_VS_STEAL
	// FPDB query to get 3B vs. steal 
	{
		// name
		"3bet_vs_steal",
		// description_for_editor
		"FPDB 3bet vs. steal",
		// query
		"SELECT	(cast(sum(ActionCount)as real) / sum(ActionOpportunities)) as result \
			FROM	(SELECT(case when stealChance and street0_3BDone then 1 else 0 end) \
					as ActionCount, \
					(case when stealChance and street0_3BChance then 1 else 0 end) \
					as ActionOpportunities \
			FROM	HandsPlayers as HP, Hands as H, Players as P, Gametypes as G \
					%TOURNEYTYPESELECT% \
			WHERE	H.startTime > date('now','-%TIMEPERIOD%') AND \
					%LIMIT&STAGESTATEMENT% \
					%TOURNEYTABLETYPE% \
					%POSITIONSTATEMENT% \
					HP.handId = H.id AND \
					H.gametypeId = G.id AND \
					G.type = '%GAMETYPE%' AND \
					G.category Like '%GAMECAT%' AND \
					HP.playerId = P.id AND \
					P.siteId = %SITEID% AND \
					P.name Like '%SCREENNAME%' \
				ORDER BY H.startTime DESC \
				LIMIT %NBHANDS% \
			)foo; ",
	// stat_group
	fpdb_group_positional
	},
#endif

#if FPDB_QUERY_SUPPORT__BB_3B_VS_STEAL
	// FPDB query to get BB 3B vs. steal 
	{
		// name
		"bb_3bet_vs_steal",
		// description_for_editor
		"FPDB BB 3bet vs. steal",
		// query
		"SELECT	(cast(sum(ActionCount)as real) / sum(ActionOpportunities)) as result \
			FROM	(SELECT(case foldBbToStealChance and when street0_3BDone then 1 else 0 end) \
					as ActionCount, \
					(case when foldBbToStealChance and street0_3BChance then 1 else 0 end) \
					as ActionOpportunities \
			FROM	HandsPlayers as HP, HandsActions as HA, Actions as A, Hands as H, Players as P, Gametypes as G \
					%TOURNEYTYPESELECT% \
			WHERE	H.startTime > date('now','-%TIMEPERIOD%') AND \
					%LIMIT&STAGESTATEMENT% \
					%TOURNEYTABLETYPE% \
					%POSITIONSTATEMENT% \
					HP.handId = HA.handId AND \
					HA.actionId = A.id AND \
					(A.id = 4 OR A.id = 5 OR A.id = 16) AND \
					HP.handId = H.id AND \
					H.gametypeId = G.id AND \
					G.type = '%GAMETYPE%' AND \
					G.category Like '%GAMECAT%' AND \
					HP.playerId = P.id AND \
					P.siteId = %SITEID% AND \
					P.name Like '%SCREENNAME%' \
				ORDER BY H.startTime DESC \
				LIMIT %NBHANDS% \
			)foo; ",
	// stat_group
	fpdb_group_positional
	},
#endif

#if FPDB_QUERY_SUPPORT__SB_3B_VS_STEAL
	// FPDB query to get SB 3B vs. steal 
	{
		// name
		"sb_3bet_vs_steal",
		// description_for_editor
		"FPDB SB 3bet vs. steal",
		// query
		"SELECT	(cast(sum(ActionCount)as real) / sum(ActionOpportunities)) as result \
			FROM	(SELECT(case when foldSbToStealChance and street0_3BDone then 1 else 0 end) \
					as ActionCount, \
					(case when foldSbToStealChance and street0_3BChance then 1 else 0 end) \
					as ActionOpportunities \
			FROM	HandsPlayers as HP, HandsActions as HA, Actions as A, Hands as H, Players as P, Gametypes as G \
					%TOURNEYTYPESELECT% \
			WHERE	H.startTime > date('now','-%TIMEPERIOD%') AND \
					%LIMIT&STAGESTATEMENT% \
					%TOURNEYTABLETYPE% \
					%POSITIONSTATEMENT% \
					HP.handId = HA.handId AND \
					HA.actionId = A.id AND \
					(A.id = 2 OR A.id = 3 OR A.id = 5) AND \
					HP.handId = H.id AND \
					H.gametypeId = G.id AND \
					G.type = '%GAMETYPE%' AND \
					G.category Like '%GAMECAT%' AND \
					HP.playerId = P.id AND \
					P.siteId = %SITEID% AND \
					P.name Like '%SCREENNAME%' \
				ORDER BY H.startTime DESC \
				LIMIT %NBHANDS% \
			)foo; ",
	// stat_group
	fpdb_group_positional
	},
#endif

#if FPDB_QUERY_SUPPORT__PREFLOP_CALLED_RAISE
	// FPDB query to get pre-flop called raise 
	{
		// name
		"preflop_called_raise",
		// description_for_editor
		"FPDB pre-flop called raise",
		// query
		"SELECT	(cast(sum(ActionCount)as real) / sum(ActionOpportunities)) as result \
			FROM	(SELECT(case when HP.street0CalledRaiseDone then 1 else 0 end) \
					as ActionCount, \
					(case when HP.street0CalledRaiseChance then 1 else 0 end) \
					as ActionOpportunities \
			FROM	HandsPlayers as HP, Hands as H, Players as P, Gametypes as G \
					%TOURNEYTYPESELECT% \
			WHERE	H.startTime > date('now','-%TIMEPERIOD%') AND \
					%LIMIT&STAGESTATEMENT% \
					%TOURNEYTABLETYPE% \
					%POSITIONSTATEMENT% \
					HP.handId = H.id AND \
					H.gametypeId = G.id AND \
					G.type = '%GAMETYPE%' AND \
					G.category Like '%GAMECAT%' AND \
					HP.playerId = P.id AND \
					P.siteId = %SITEID% AND \
					P.name Like '%SCREENNAME%' \
				ORDER BY H.startTime DESC \
				LIMIT %NBHANDS% \
			)foo; ",
	// stat_group
	fpdb_group_advanced
	},
#endif

#if FPDB_QUERY_SUPPORT__PREFLOP_3B
	// FPDB  query to get PREFLOP 3 bet 
	{
		// name
		"3bet",
		// description_for_editor
		"FPDB 3bet preflop",
		// query
		"SELECT	(cast(sum(ActionCount)as real) / sum(ActionOpportunities)) as result \
			FROM	(SELECT(case when street0_3BDone then 1 else 0 end) \
					as ActionCount, \
					(case when street0_3BChance then 1 else 0 end) \
					as ActionOpportunities \
			FROM	HandsPlayers as HP, Hands as H, Players as P, Gametypes as G \
					%TOURNEYTYPESELECT% \
			WHERE	H.startTime > date('now','-%TIMEPERIOD%') AND \
					%LIMIT&STAGESTATEMENT% \
					%TOURNEYTABLETYPE% \
					%POSITIONSTATEMENT% \
					HP.handId = H.id AND \
					H.gametypeId = G.id AND \
					G.type = '%GAMETYPE%' AND \
					G.category Like '%GAMECAT%' AND \
					HP.playerId = P.id AND \
					P.siteId = %SITEID% AND \
					P.name Like '%SCREENNAME%' \
				ORDER BY H.startTime DESC \
				LIMIT %NBHANDS% \
			)foo; ",
	// stat_group
	fpdb_group_advanced
	},
#endif

#if FPDB_QUERY_SUPPORT__PREFLOP_FOLD_TO_3B
	// FPDB  query to get PREFLOP fold to 3 bet 
	{
		// name
		"fold_to_3bet",
		// description_for_editor
		"FPDB folded while facing 3bet preflop",
		// query
		"SELECT	(cast(sum(ActionCount)as real) / sum(ActionOpportunities)) as result \
			FROM	(SELECT(case when HP.street0_FoldTo3BDone then 1 else 0 end) \
					as ActionCount, \
					(case when HP.street0_FoldTo3BChance then 1 else 0 end) \
					as ActionOpportunities \
			FROM	HandsPlayers as HP, Hands as H, Players as P, Gametypes as G \
					%TOURNEYTYPESELECT% \
			WHERE	H.startTime > date('now','-%TIMEPERIOD%') AND \
					%LIMIT&STAGESTATEMENT% \
					%TOURNEYTABLETYPE% \
					%POSITIONSTATEMENT% \
					HP.handId = H.id AND \
					H.gametypeId = G.id AND \
					G.type = '%GAMETYPE%' AND \
					G.category Like '%GAMECAT%' AND \
					HP.playerId = P.id AND \
					P.siteId = %SITEID% AND \
					P.name Like '%SCREENNAME%' \
				ORDER BY H.startTime DESC \
				LIMIT %NBHANDS% \
			)foo; ",
	// stat_group
	fpdb_group_advanced
	},
#endif

#if FPDB_QUERY_SUPPORT__PREFLOP_FOLD_TO_3B_AFTER_RAISE
	// FPDB  query to get PREFLOP fold to 3 bet 
	{
		// name
		"fold_to_3bet_afterR",
		// description_for_editor
		"FPDB folded while facing 3bet preflop ... after raise",
		// query
		"SELECT	(cast(sum(ActionCount)as real) / sum(ActionOpportunities)) as result \
			FROM	(SELECT(case when HP.street0_FoldTo3BDone and HP.raisedFirstIn then 1 else 0 end) \
					as ActionCount, \
					(case when HP.street0_FoldTo3BChance and HP.raisedFirstIn then 1 else 0 end) \
					as ActionOpportunities \
			FROM	HandsPlayers as HP, Hands as H, Players as P, Gametypes as G \
					%TOURNEYTYPESELECT% \
			WHERE	H.startTime > date('now','-%TIMEPERIOD%') AND \
					%LIMIT&STAGESTATEMENT% \
					%TOURNEYTABLETYPE% \
					%POSITIONSTATEMENT% \
					HP.handId = H.id AND \
					H.gametypeId = G.id AND \
					G.type = '%GAMETYPE%' AND \
					G.category Like '%GAMECAT%' AND \
					HP.playerId = P.id AND \
					P.siteId = %SITEID% AND \
					P.name Like '%SCREENNAME%' \
				ORDER BY H.startTime DESC \
				LIMIT %NBHANDS% \
			)foo; ",
	// stat_group
	fpdb_group_advanced
	},
#endif

#if FPDB_QUERY_SUPPORT__PF_FOLD_TO_RESTEAL
	// FPDB query to get Fold to Resteal 
	{
		// name
		"fold_to_resteal",
		// description_for_editor
		"FPDB Fold to 3bet after Stealing",
		// query
		"SELECT	(cast(sum(ActionCount)as real) / sum(ActionOpportunities)) as result \
			FROM	(SELECT(case when HP.stealDone and HP.street0_FoldTo3BChance and HP.street0_FoldTo3BDone then 1 else 0 end) \
					as ActionCount, \
					(case when HP.stealDone and HP.street0_FoldTo3BChance then 1 else 0 end) \
					as ActionOpportunities \
			FROM	HandsPlayers as HP, Hands as H, Players as P, Gametypes as G \
					%TOURNEYTYPESELECT% \
			WHERE	H.startTime > date('now','-%TIMEPERIOD%') AND \
					%LIMIT&STAGESTATEMENT% \
					%TOURNEYTABLETYPE% \
					%POSITIONSTATEMENT% \
					HP.handId = H.id AND \
					H.gametypeId = G.id AND \
					G.type = '%GAMETYPE%' AND \
					G.category Like '%GAMECAT%' AND \
					HP.playerId = P.id AND \
					P.siteId = %SITEID% AND \
					P.name Like '%SCREENNAME%' \
				ORDER BY H.startTime DESC \
				LIMIT %NBHANDS% \
			)foo; ",
	// stat_group
	fpdb_group_positional
	},
#endif

#if FPDB_QUERY_SUPPORT__PREFLOP_4B
	// FPDB query to get Preflop 4B 
	{
		// name
		"4bet",
		// description_for_editor
		"FPDB Preflop 4B",
		// query
		"SELECT	(cast(sum(ActionCount)as real) / sum(ActionOpportunities)) as result \
			FROM	(SELECT(case when street0_4BDone then 1 else 0 end) \
					as ActionCount, \
					(case when street0_4BChance then 1 else 0 end) \
					as ActionOpportunities \
			FROM	HandsPlayers as HP, Hands as H, Players as P, Gametypes as G \
					%TOURNEYTYPESELECT% \
			WHERE	H.startTime > date('now','-%TIMEPERIOD%') AND \
					%LIMIT&STAGESTATEMENT% \
					%TOURNEYTABLETYPE% \
					%POSITIONSTATEMENT% \
					HP.handId = H.id AND \
					H.gametypeId = G.id AND \
					G.type = '%GAMETYPE%' AND \
					G.category Like '%GAMECAT%' AND \
					HP.playerId = P.id AND \
					P.siteId = %SITEID% AND \
					P.name Like '%SCREENNAME%' \
				ORDER BY H.startTime DESC \
				LIMIT %NBHANDS% \
			)foo; ",
	// stat_group
	fpdb_group_advanced
	},
#endif

#if FPDB_QUERY_SUPPORT__PREFLOP_FOLD_TO_4B
	// FPDB query to get Preflop fold to 4B 
	{
		// name
		"fold_to_4bet",
		// description_for_editor
		"FPDB Preflop fold to 4bet",
		// query
		"SELECT	(cast(sum(ActionCount)as real) / sum(ActionOpportunities)) as result \
			FROM	(SELECT(case when street0_FoldTo4BDone then 1 else 0 end) \
					as ActionCount, \
					(case when street0_FoldTo4BChance then 1 else 0 end) \
					as ActionOpportunities \
			FROM	HandsPlayers as HP, Hands as H, Players as P, Gametypes as G \
					%TOURNEYTYPESELECT% \
			WHERE	H.startTime > date('now','-%TIMEPERIOD%') AND \
					%LIMIT&STAGESTATEMENT% \
					%TOURNEYTABLETYPE% \
					%POSITIONSTATEMENT% \
					HP.handId = H.id AND \
					H.gametypeId = G.id AND \
					G.type = '%GAMETYPE%' AND \
					G.category Like '%GAMECAT%' AND \
					HP.playerId = P.id AND \
					P.siteId = %SITEID% AND \
					P.name Like '%SCREENNAME%' \
				ORDER BY H.startTime DESC \
				LIMIT %NBHANDS% \
			)foo; ",
	// stat_group
	fpdb_group_positional
	},
#endif

#if FPDB_QUERY_SUPPORT__PREFLOP_SQUEEZE
	// FPDB 
	{
		// name
		"preflop_squeeze",
		// description_for_editor
		"Squeeze Preflop %",
		// query
		"SELECT	(cast(sum(ActionCount)as real) / sum(ActionOpportunities)) as result \
			FROM	(SELECT(case when HP.street0_SqueezeDone then 1 else 0 end) \
					as ActionCount, \
					(case when HP.street0_SqueezeChance then 1 else 0 end) \
					as ActionOpportunities \
			FROM	HandsPlayers as HP, Hands as H, Players as P, Gametypes as G \
					%TOURNEYTYPESELECT% \
			WHERE	H.startTime > date('now','-%TIMEPERIOD%') AND \
					%LIMIT&STAGESTATEMENT% \
					%TOURNEYTABLETYPE% \
					%POSITIONSTATEMENT% \
					HP.handId = H.id AND \
					H.gametypeId = G.id AND \
					G.type = '%GAMETYPE%' AND \
					G.category Like '%GAMECAT%' AND \
					HP.playerId = P.id AND \
					P.siteId = %SITEID% AND \
					P.name Like '%SCREENNAME%' \
				ORDER BY H.startTime DESC \
				LIMIT %NBHANDS% \
			)foo; ",
	// stat_group
	fpdb_group_advanced
	},
#endif

#if FPDB_QUERY_SUPPORT__FLOP_AGGRESSION_FACTOR
	// FPDB query to get flop aggression factor 
	{
		// name
		"flop_af",
		// description_for_editor
		"FPDB flop aggression",
		// query
		"SELECT	(cast(sum(AggressionCount)as real) / CallsCount) as result \
			FROM	(SELECT (case when HP.street1Aggr then 1 else 0 end) as AggressionCount, \
					HP.street1Calls as CallsCount \
			FROM	HandsPlayers as HP, Hands as H, Players as P, Gametypes as G \
					%TOURNEYTYPESELECT% \
			WHERE	H.startTime > date('now','-%TIMEPERIOD%') AND \
					%LIMIT&STAGESTATEMENT% \
					%TOURNEYTABLETYPE% \
					%POSITIONSTATEMENT% \
					HP.handId = H.id AND \
					H.gametypeId = G.id AND \
					G.type = '%GAMETYPE%' AND \
					G.category Like '%GAMECAT%' AND \
					HP.playerId = P.id AND \
					P.siteId = %SITEID% AND \
					P.name Like '%SCREENNAME%' \
				ORDER BY H.startTime DESC \
				LIMIT %NBHANDS% \
			)foo; ",
	// stat_group
	fpdb_group_advanced
	},
#endif

#if FPDB_QUERY_SUPPORT__TURN_AGGRESSION_FACTOR
	// FPDB query to get turn aggression factor 
	{
		// name
		"turn_af",
		// description_for_editor
		"FPDB turn aggression",
		// query
		"SELECT	(cast(sum(AggressionCount)as real) / CallsCount) as result \
			FROM	(SELECT (case when HP.street2Aggr then 1 else 0 end) as AggressionCount, \
					HP.street2Calls as CallsCount \
			FROM	HandsPlayers as HP, Hands as H, Players as P, Gametypes as G \
					%TOURNEYTYPESELECT% \
			WHERE	H.startTime > date('now','-%TIMEPERIOD%') AND \
					%LIMIT&STAGESTATEMENT% \
					%TOURNEYTABLETYPE% \
					%POSITIONSTATEMENT% \
					HP.handId = H.id AND \
					H.gametypeId = G.id AND \
					G.type = '%GAMETYPE%' AND \
					G.category Like '%GAMECAT%' AND \
					HP.playerId = P.id AND \
					P.siteId = %SITEID% AND \
					P.name Like '%SCREENNAME%' \
				ORDER BY H.startTime DESC \
				LIMIT %NBHANDS% \
			)foo; ",
	// stat_group
	fpdb_group_advanced
	},
#endif

#if FPDB_QUERY_SUPPORT__RIVER_AGGRESSION_FACTOR
	// FPDB query to get river aggression factor 
	{
		// name
		"river_af",
		// description_for_editor
		"FPDB river aggression",
		// query
		"SELECT	(cast(sum(AggressionCount)as real) / CallsCount) as result \
			FROM	(SELECT (case when HP.street3Aggr then 1 else 0 end) as AggressionCount, \
					HP.street3Calls as CallsCount \
			FROM	HandsPlayers as HP, Hands as H, Players as P, Gametypes as G \
					%TOURNEYTYPESELECT% \
			WHERE	H.startTime > date('now','-%TIMEPERIOD%') AND \
					%LIMIT&STAGESTATEMENT% \
					%TOURNEYTABLETYPE% \
					%POSITIONSTATEMENT% \
					HP.handId = H.id AND \
					H.gametypeId = G.id AND \
					G.type = '%GAMETYPE%' AND \
					G.category Like '%GAMECAT%' AND \
					HP.playerId = P.id AND \
					P.siteId = %SITEID% AND \
					P.name Like '%SCREENNAME%' \
				ORDER BY H.startTime DESC \
				LIMIT %NBHANDS% \
			)foo; ",
	// stat_group
	fpdb_group_advanced
	},
#endif

#if FPDB_QUERY_SUPPORT__TOTAL_AGGRESSION_FACTOR
	// FPDB query to get total aggression factor without preflop 
	{
		// name
		"total_af",
		// description_for_editor
		"FPDB total aggression excluding preflop",
		// query
		"SELECT	(cast(sum(AggressionCount)as real) / CallsCount) as result \
			FROM	(SELECT (case when HP.street1Aggr + HP.street2Aggr + HP.street3Aggr then 1 else 0 end) as AggressionCount, \
					HP.street1Calls + HP.street2Calls + HP.street3Calls as CallsCount \
			FROM	HandsPlayers as HP, Hands as H, Players as P, Gametypes as G \
					%TOURNEYTYPESELECT% \
			WHERE	H.startTime > date('now','-%TIMEPERIOD%') AND \
					%LIMIT&STAGESTATEMENT% \
					%TOURNEYTABLETYPE% \
					%POSITIONSTATEMENT% \
					HP.handId = H.id AND \
					H.gametypeId = G.id AND \
					G.type = '%GAMETYPE%' AND \
					G.category Like '%GAMECAT%' AND \
					HP.playerId = P.id AND \
					P.siteId = %SITEID% AND \
					P.name Like '%SCREENNAME%' \
				ORDER BY H.startTime DESC \
				LIMIT %NBHANDS% \
			)foo; ",
	// stat_group
	fpdb_group_advanced
	},
#endif

#if FPDB_QUERY_SUPPORT__FLOP_AGGRESSION_FREQUENCY
	// FPDB query to get flop aggression frequency 
	{
		// name
		"flop_afq",
		// description_for_editor
		"FPDB flop aggression frequency",
		// query
		"SELECT	(cast(sum(AggressionCount)as real) / sum(SeenCount)) as result \
			FROM	(SELECT (case when HP.street1Aggr then 1 else 0 end) as AggressionCount, \
					(case when HP.street1Seen then 1 else 0 end) as SeenCount \
			FROM	HandsPlayers as HP, Hands as H, Players as P, Gametypes as G \
					%TOURNEYTYPESELECT% \
			WHERE	H.startTime > date('now','-%TIMEPERIOD%') AND \
					%LIMIT&STAGESTATEMENT% \
					%TOURNEYTABLETYPE% \
					%POSITIONSTATEMENT% \
					HP.handId = H.id AND \
					H.gametypeId = G.id AND \
					G.type = '%GAMETYPE%' AND \
					G.category Like '%GAMECAT%' AND \
					HP.playerId = P.id AND \
					P.siteId = %SITEID% AND \
					P.name Like '%SCREENNAME%' \
				ORDER BY H.startTime DESC \
				LIMIT %NBHANDS% \
			)foo; ",
	// stat_group
	fpdb_group_advanced
	},
#endif

#if FPDB_QUERY_SUPPORT__TURN_AGGRESSION_FREQUENCY
	// FPDB query to get turn aggression frequency 
	{
		// name
		"turn_afq",
		// description_for_editor
		"FPDB turn aggression frequency",
		// query
		"SELECT	(cast(sum(AggressionCount)as real) / sum(SeenCount)) as result \
			FROM	(SELECT (case when HP.street2Aggr then 1 else 0 end) as AggressionCount, \
					(case when HP.street2Seen then 1 else 0 end) as SeenCount \
			FROM	HandsPlayers as HP, Hands as H, Players as P, Gametypes as G \
					%TOURNEYTYPESELECT% \
			WHERE	H.startTime > date('now','-%TIMEPERIOD%') AND \
					%LIMIT&STAGESTATEMENT% \
					%TOURNEYTABLETYPE% \
					%POSITIONSTATEMENT% \
					HP.handId = H.id AND \
					H.gametypeId = G.id AND \
					G.type = '%GAMETYPE%' AND \
					G.category Like '%GAMECAT%' AND \
					HP.playerId = P.id AND \
					P.siteId = %SITEID% AND \
					P.name Like '%SCREENNAME%' \
				ORDER BY H.startTime DESC \
				LIMIT %NBHANDS% \
			)foo; ",
	// stat_group
	fpdb_group_advanced
	},
#endif

#if FPDB_QUERY_SUPPORT__RIVER_AGGRESSION_FREQUENCY
	// FPDB query to get river aggression frequency 
	{
		// name
		"river_afq",
		// description_for_editor
		"FPDB river aggression frequency",
		// query
		"SELECT	(cast(sum(AggressionCount)as real) / sum(SeenCount)) as result \
			FROM	(SELECT (case when HP.street3Aggr then 1 else 0 end) as AggressionCount, \
					(case when HP.street3Seen then 1 else 0 end) as SeenCount \
			FROM	HandsPlayers as HP, Hands as H, Players as P, Gametypes as G \
					%TOURNEYTYPESELECT% \
			WHERE	H.startTime > date('now','-%TIMEPERIOD%') AND \
					%LIMIT&STAGESTATEMENT% \
					%TOURNEYTABLETYPE% \
					%POSITIONSTATEMENT% \
					HP.handId = H.id AND \
					H.gametypeId = G.id AND \
					G.type = '%GAMETYPE%' AND \
					G.category Like '%GAMECAT%' AND \
					HP.playerId = P.id AND \
					P.siteId = %SITEID% AND \
					P.name Like '%SCREENNAME%' \
				ORDER BY H.startTime DESC \
				LIMIT %NBHANDS% \
			)foo; ",
	// stat_group
	fpdb_group_advanced
	},
#endif

#if FPDB_QUERY_SUPPORT__TOTAL_AGGRESSION_FREQUENCY
	// FPDB query to get total aggression frequency without preflop 
	{
		// name
		"total_afq",
		// description_for_editor
		"FPDB total aggression frequency excluding preflop",
		// query:  (raises + bets) / (calls + folds + raises + bets)
		"SELECT	(cast(sum(AggressionCount)as real) / sum(SeenCount)) as result \
			FROM	(SELECT (case when HP.street1Aggr + HP.street2Aggr + HP.street3Aggr then 1 else 0 end) as AggressionCount, \
					(case when HP.street1Seen + HP.street2Seen + HP.street3Seen then 1 else 0 end) as SeenCount \
			FROM	HandsPlayers as HP, Hands as H, Players as P, Gametypes as G \
					%TOURNEYTYPESELECT% \
			WHERE	H.startTime > date('now','-%TIMEPERIOD%') AND \
					%LIMIT&STAGESTATEMENT% \
					%TOURNEYTABLETYPE% \
					%POSITIONSTATEMENT% \
					HP.handId = H.id AND \
					H.gametypeId = G.id AND \
					G.type = '%GAMETYPE%' AND \
					G.category Like '%GAMECAT%' AND \
					HP.playerId = P.id AND \
					P.siteId = %SITEID% AND \
					P.name Like '%SCREENNAME%' \
				ORDER BY H.startTime DESC \
				LIMIT %NBHANDS% \
			)foo; ",
	// stat_group
	fpdb_group_advanced
	},
#endif

#if FPDB_QUERY_SUPPORT__SEEN_FLOP_PCT
	// FPDB query to get flops seen 
	{
		// name
		"flop_seen",
		// description_for_editor
		"FPDB saw flop",
		// query
		"SELECT (case when ActionOpportunities = 0 then -1 \
				 else cast(ActionCount as real) / ActionOpportunities \
				 end) as result \
			FROM	(SELECT	sum(case when street1Seen then 1 else 0 end) \
							 as ActionCount, \
							count(*) \
							 as ActionOpportunities \
			FROM	HandsPlayers as HP, Hands as H, Players as P, Gametypes as G \
					%TOURNEYTYPESELECT% \
			WHERE	H.startTime > date('now','-%TIMEPERIOD%') AND \
					%LIMIT&STAGESTATEMENT% \
					%TOURNEYTABLETYPE% \
					%POSITIONSTATEMENT% \
					HP.handId = H.id AND \
					H.gametypeId = G.id AND \
					G.type = '%GAMETYPE%' AND \
					G.category Like '%GAMECAT%' AND \
					HP.playerId = P.id AND \
					P.siteId = %SITEID% AND \
					P.name Like '%SCREENNAME%' \
				ORDER BY count(H.startTime) DESC \
				LIMIT %NBHANDS% \
			)foo; ",
	// stat_group
	fpdb_group_advanced
	},
#endif

#if FPDB_QUERY_SUPPORT__SEEN_TURN_PCT
	// FPDB query to get turns seen 
	{
		// name
		"turn_seen",
		// description_for_editor
		"FPDB saw turn",
		// query
		"SELECT (cast(sum(ActionCount)as real) / sum(ActionOpportunities)) as result \
			FROM	(SELECT	(case when HP.street2Seen then 1 else 0 end) \
							 as ActionCount, \
							(case when HP.street1Seen then 1 else 0 end) \
							 as ActionOpportunities \
			FROM	HandsPlayers as HP, Hands as H, Players as P, Gametypes as G \
					%TOURNEYTYPESELECT% \
			WHERE	H.startTime > date('now','-%TIMEPERIOD%') AND \
					%LIMIT&STAGESTATEMENT% \
					%TOURNEYTABLETYPE% \
					%POSITIONSTATEMENT% \
					HP.handId = H.id AND \
					H.gametypeId = G.id AND \
					G.type = '%GAMETYPE%' AND \
					G.category Like '%GAMECAT%' AND \
					HP.playerId = P.id AND \
					P.siteId = %SITEID% AND \
					P.name Like '%SCREENNAME%' \
				ORDER BY H.startTime DESC \
				LIMIT %NBHANDS% \
			)foo; ",
	// stat_group
	fpdb_group_advanced
	},
#endif

#if FPDB_QUERY_SUPPORT__SEEN_RIVER_PCT
	// FPDB query to get rivers seen 
	{
		// name
		"river_seen",
		// description_for_editor
		"FPDB saw river",
		// query
		"SELECT (cast(sum(ActionCount)as real) / sum(ActionOpportunities)) as result \
			FROM	(SELECT	(case when HP.street3Seen then 1 else 0 end) \
							 as ActionCount, \
							(case when HP.street2Seen then 1 else 0 end) \
							 as ActionOpportunities \
			FROM	HandsPlayers as HP, Hands as H, Players as P, Gametypes as G \
					%TOURNEYTYPESELECT% \
			WHERE	H.startTime > date('now','-%TIMEPERIOD%') AND \
					%LIMIT&STAGESTATEMENT% \
					%TOURNEYTABLETYPE% \
					%POSITIONSTATEMENT% \
					HP.handId = H.id AND \
					H.gametypeId = G.id AND \
					G.type = '%GAMETYPE%' AND \
					G.category Like '%GAMECAT%' AND \
					HP.playerId = P.id AND \
					P.siteId = %SITEID% AND \
					P.name Like '%SCREENNAME%' \
				ORDER BY H.startTime DESC \
				LIMIT %NBHANDS% \
			)foo; ",
	// stat_group
	fpdb_group_advanced
	},
#endif

#if FPDB_QUERY_SUPPORT__FLOP_CBET
	// FPDB  query to get cbet on FLOP 
	{
		// name
		"flop_cbet",
		// description_for_editor
		"FPDB cbet",
		// query
		"SELECT (cast(sum(ActionCount)as real) / sum(ActionOpportunities)) as result \
			FROM	(SELECT	(case when HP.street1CBDone then 1 else 0 end) \
							 as ActionCount, \
							(case when HP.street1CBChance then 1 else 0 end) \
							 as ActionOpportunities \
			FROM	HandsPlayers as HP, Hands as H, Players as P, Gametypes as G \
					%TOURNEYTYPESELECT% \
			WHERE	H.startTime > date('now','-%TIMEPERIOD%') AND \
					%LIMIT&STAGESTATEMENT% \
					%TOURNEYTABLETYPE% \
					%POSITIONSTATEMENT% \
					HP.handId = H.id AND \
					H.gametypeId = G.id AND \
					G.type = '%GAMETYPE%' AND \
					G.category Like '%GAMECAT%' AND \
					HP.playerId = P.id AND \
					P.siteId = %SITEID% AND \
					P.name Like '%SCREENNAME%' \
				ORDER BY H.startTime DESC \
				LIMIT %NBHANDS% \
			)foo; ",
	// stat_group
	fpdb_group_advanced
	},
#endif

#if FPDB_QUERY_SUPPORT__FLOP_FOLD_TO_CBET
	// FPDB  query to get FLOP fold to cbet 
	{
		// name
		"flop_fold_to_cbet",
		// description_for_editor
		"FPDB folded while facing cbet flop",
		// query
		"SELECT (cast(sum(ActionCount)as real) / sum(ActionOpportunities)) as result \
			FROM	(SELECT	(case when HP.foldToStreet1CBDone then 1 else 0 end) \
							 as ActionCount, \
							(case when HP.foldToStreet1CBChance then 1 else 0 end) \
							 as ActionOpportunities \
			FROM	HandsPlayers as HP, Hands as H, Players as P, Gametypes as G \
					%TOURNEYTYPESELECT% \
			WHERE	H.startTime > date('now','-%TIMEPERIOD%') AND \
					%LIMIT&STAGESTATEMENT% \
					%TOURNEYTABLETYPE% \
					%POSITIONSTATEMENT% \
					HP.handId = H.id AND \
					H.gametypeId = G.id AND \
					G.type = '%GAMETYPE%' AND \
					G.category Like '%GAMECAT%' AND \
					HP.playerId = P.id AND \
					P.siteId = %SITEID% AND \
					P.name Like '%SCREENNAME%' \
				ORDER BY H.startTime DESC \
				LIMIT %NBHANDS% \
			)foo; ",
	// stat_group
	fpdb_group_advanced
	},
#endif

#if FPDB_QUERY_SUPPORT__FLOP_RAISE_CBET
	// FPDB  query to get raise cbet on FLOP 
	{
		// name
		"flop_raise_cbet",
		// description_for_editor
		"FPDB raise flop cbet",
		// query
		"SELECT (cast(sum(ActionCount)as real) / sum(ActionOpportunities)) as result \
			FROM	(SELECT	(case when HP.raiseToStreet1CBDone then 1 else 0 end) \
							 as ActionCount, \
							(case when HP.raiseToStreet1CBChance then 1 else 0 end) \
							 as ActionOpportunities \
			FROM	HandsPlayers as HP, Hands as H, Players as P, Gametypes as G \
					%TOURNEYTYPESELECT% \
			WHERE	H.startTime > date('now','-%TIMEPERIOD%') AND \
					%LIMIT&STAGESTATEMENT% \
					%TOURNEYTABLETYPE% \
					%POSITIONSTATEMENT% \
					HP.handId = H.id AND \
					H.gametypeId = G.id AND \
					G.type = '%GAMETYPE%' AND \
					G.category Like '%GAMECAT%' AND \
					HP.playerId = P.id AND \
					P.siteId = %SITEID% AND \
					P.name Like '%SCREENNAME%' \
				ORDER BY H.startTime DESC \
				LIMIT %NBHANDS% \
			)foo; ",
	fpdb_group_advanced
	},
#endif

#if FPDB_QUERY_SUPPORT__TURN_CBET
	// FPDB  query to get cbet on TURN 
	{
		// name
		"turn_cbet",
		// description_for_editor
		"FPDB turn cbet",
		// query
		"SELECT (cast(sum(ActionCount)as real) / sum(ActionOpportunities)) as result \
			FROM	(SELECT	(case when HP.street2CBDone then 1 else 0 end) \
							 as ActionCount, \
							(case when HP.street2CBChance then 1 else 0 end) \
							 as ActionOpportunities \
			FROM	HandsPlayers as HP, Hands as H, Players as P, Gametypes as G \
					%TOURNEYTYPESELECT% \
			WHERE	H.startTime > date('now','-%TIMEPERIOD%') AND \
					%LIMIT&STAGESTATEMENT% \
					%TOURNEYTABLETYPE% \
					%POSITIONSTATEMENT% \
					HP.handId = H.id AND \
					H.gametypeId = G.id AND \
					G.type = '%GAMETYPE%' AND \
					G.category Like '%GAMECAT%' AND \
					HP.playerId = P.id AND \
					P.siteId = %SITEID% AND \
					P.name Like '%SCREENNAME%' \
				ORDER BY H.startTime DESC \
				LIMIT %NBHANDS% \
			)foo; ",
	fpdb_group_advanced
	},
#endif

#if FPDB_QUERY_SUPPORT__TURN_FOLD_TO_CBET
	// FPDB  query to get TURN fold to cbet 
	{
		// name
		"turn_fold_to_cbet",
		// description_for_editor
		"FPDB folded while facing cbet turn",
		// query
		"SELECT (cast(sum(ActionCount)as real) / sum(ActionOpportunities)) as result \
			FROM	(SELECT	(case when HP.foldToStreet2CBDone then 1 else 0 end) \
							 as ActionCount, \
							(case when HP.foldToStreet2CBChance then 1 else 0 end) \
							 as ActionOpportunities \
			FROM	HandsPlayers as HP, Hands as H, Players as P, Gametypes as G \
					%TOURNEYTYPESELECT% \
			WHERE	H.startTime > date('now','-%TIMEPERIOD%') AND \
					%LIMIT&STAGESTATEMENT% \
					%TOURNEYTABLETYPE% \
					%POSITIONSTATEMENT% \
					HP.handId = H.id AND \
					H.gametypeId = G.id AND \
					G.type = '%GAMETYPE%' AND \
					G.category Like '%GAMECAT%' AND \
					HP.playerId = P.id AND \
					P.siteId = %SITEID% AND \
					P.name Like '%SCREENNAME%' \
				ORDER BY H.startTime DESC \
				LIMIT %NBHANDS% \
			)foo; ",
	// stat_group
	fpdb_group_advanced
	},
#endif

#if FPDB_QUERY_SUPPORT__RIVER_FOLD_TO_CBET
	// FPDB  query to get RIVER fold to cbet 
	{
		// name
		"river_fold_to_cbet",
		// description_for_editor
		"FPDB folded while facing cbet river",
		// query
		"SELECT (cast(sum(ActionCount)as real) / sum(ActionOpportunities)) as result \
			FROM	(SELECT	(case when HP.foldToStreet3CBDone then 1 else 0 end) \
							 as ActionCount, \
							(case when HP.foldToStreet3CBChance then 1 else 0 end) \
							 as ActionOpportunities \
			FROM	HandsPlayers as HP, Hands as H, Players as P, Gametypes as G \
					%TOURNEYTYPESELECT% \
			WHERE	H.startTime > date('now','-%TIMEPERIOD%') AND \
					%LIMIT&STAGESTATEMENT% \
					%TOURNEYTABLETYPE% \
					%POSITIONSTATEMENT% \
					HP.handId = H.id AND \
					H.gametypeId = G.id AND \
					G.type = '%GAMETYPE%' AND \
					G.category Like '%GAMECAT%' AND \
					HP.playerId = P.id AND \
					P.siteId = %SITEID% AND \
					P.name Like '%SCREENNAME%' \
				ORDER BY H.startTime DESC \
				LIMIT %NBHANDS% \
			)foo; ",
	// stat_group
	fpdb_group_advanced
	},
#endif

#if FPDB_QUERY_SUPPORT__POSTFLOP_FOLD_TO_3B
	// FPDB  query to get total fold to 3 bet without preflop
	{
		// name
		"postflop_fold_to_3bet",
		// description_for_editor
		"FPDB folded while facing 3bet total without preflop",
		// query
		"SELECT (cast(sum(ActionCount)as real) / sum(ActionOpportunities)) as result \
			FROM	(SELECT	(case when street1_FoldTo3BDone + street2_FoldTo3BDone + street3_FoldTo3BDone then 1 else 0 end) \
						 as ActionCount, \
						(case when street1_FoldTo3BChance + street2_FoldTo3BChance + street3_FoldTo3BChance then 1 else 0 end) \
						 as ActionOpportunities \
			FROM	HandsPlayers as HP, Hands as H, Players as P, Gametypes as G \
					%TOURNEYTYPESELECT% \
			WHERE	H.startTime > date('now','-%TIMEPERIOD%') AND \
					%LIMIT&STAGESTATEMENT% \
					%TOURNEYTABLETYPE% \
					%POSITIONSTATEMENT% \
					HP.handId = H.id AND \
					H.gametypeId = G.id AND \
					G.type = '%GAMETYPE%' AND \
					G.category Like '%GAMECAT%' AND \
					HP.playerId = P.id AND \
					P.siteId = %SITEID% AND \
					P.name Like '%SCREENNAME%' \
				ORDER BY H.startTime DESC \
				LIMIT %NBHANDS% \
			)foo; ",
	// stat_group
	fpdb_group_advanced
	},
#endif

#if FPDB_QUERY_SUPPORT__FLOP_FOLD_TO_3B
	// FPDB  query to get FLOP fold to 3 bet 
	{
		// name
		"flop_fold_to_3bet",
		// description_for_editor
		"FPDB folded while facing 3bet flop",
		// query
		"SELECT (cast(sum(ActionCount)as real) / sum(ActionOpportunities)) as result \
			FROM	(SELECT	(case when street1_FoldTo3BDone then 1 else 0 end) \
						 as ActionCount, \
						(case when street1_FoldTo3BChance then 1 else 0 end) \
						 as ActionOpportunities \
			FROM	HandsPlayers as HP, Hands as H, Players as P, Gametypes as G \
					%TOURNEYTYPESELECT% \
			WHERE	H.startTime > date('now','-%TIMEPERIOD%') AND \
					%LIMIT&STAGESTATEMENT% \
					%TOURNEYTABLETYPE% \
					%POSITIONSTATEMENT% \
					HP.handId = H.id AND \
					H.gametypeId = G.id AND \
					G.type = '%GAMETYPE%' AND \
					G.category Like '%GAMECAT%' AND \
					HP.playerId = P.id AND \
					P.siteId = %SITEID% AND \
					P.name Like '%SCREENNAME%' \
				ORDER BY H.startTime DESC \
				LIMIT %NBHANDS% \
			)foo; ",
	// stat_group
	fpdb_group_advanced
	},
#endif

#if FPDB_QUERY_SUPPORT__FLOP_CHECK_RAISE
	// FPDB  query to get flop check-raise 
	{
		// name
		"flop_checkraise",
		// description_for_editor
		"FPDB flop check-raise",
		// query
		"SELECT (cast(sum(ActionCount)as real) / sum(ActionOpportunities)) as result \
			FROM	(SELECT	(case when street1CheckRaiseDone then 1 else 0 end) \
						 as ActionCount, \
						(case when street1CheckCallRaiseChance then 1 else 0 end) \
						 as ActionOpportunities \
			FROM	HandsPlayers as HP, Hands as H, Players as P, Gametypes as G \
					%TOURNEYTYPESELECT% \
			WHERE	H.startTime > date('now','-%TIMEPERIOD%') AND \
					%LIMIT&STAGESTATEMENT% \
					%TOURNEYTABLETYPE% \
					%POSITIONSTATEMENT% \
					HP.handId = H.id AND \
					H.gametypeId = G.id AND \
					G.type = '%GAMETYPE%' AND \
					G.category Like '%GAMECAT%' AND \
					HP.playerId = P.id AND \
					P.siteId = %SITEID% AND \
					P.name Like '%SCREENNAME%' \
				ORDER BY H.startTime DESC \
				LIMIT %NBHANDS% \
			)foo; ",
	fpdb_group_advanced
	},
#endif

#if FPDB_QUERY_SUPPORT__FLOP_DONKBET
	// FPDB  query to get FLOP donk 
	{
		// name
		"flop_donkbet",
		// description_for_editor
		"FPDB donk flop",
		// query
		"SELECT (cast(sum(ActionCount)as real) / sum(ActionOpportunities)) as result \
			FROM	(SELECT	(case when (HP.otherRaisedStreet0 AND NOT(HP.street0_3BDone OR HP.street0_4BDone) \
						AND HP.street1Aggr AND NOT(HP.street1CBChance) AND \
						((H.seats > 2 and HP.street0InPosition) or \
						(H.seats = 2 and HP.position = 'B'))) then 1 else 0 end) \
						 as ActionCount, \
						(case when (HP.otherRaisedStreet0 AND NOT(HP.street0_3BDone OR HP.street0_4BDone) \
						AND (HP.street1_2BChance) AND NOT(HP.street1CBChance) AND \
						((H.seats > 2 and HP.street0InPosition) or \
						(H.seats = 2 and HP.position = 'B'))) then 1 else 0 end) \
						 as ActionOpportunities \
			FROM	HandsPlayers as HP, Hands as H, Players as P, Gametypes as G \
					%TOURNEYTYPESELECT% \
			WHERE	H.startTime > date('now','-%TIMEPERIOD%') AND \
					%LIMIT&STAGESTATEMENT% \
					%TOURNEYTABLETYPE% \
					%POSITIONSTATEMENT% \
					HP.handId = H.id AND \
					H.gametypeId = G.id AND \
					G.type = '%GAMETYPE%' AND \
					G.category Like '%GAMECAT%' AND \
					HP.playerId = P.id AND \
					P.siteId = %SITEID% AND \
					P.name Like '%SCREENNAME%' \
				ORDER BY H.startTime DESC \
				LIMIT %NBHANDS% \
			)foo; ",
	// stat_group
	fpdb_group_advanced
	},
#endif

#if FPDB_QUERY_SUPPORT__FLOP_FLOAT
	// FPDB  query to get flop float 
	{
		// name
		"flop_float",
		// description_for_editor
		"FPDB flop float",
		// query
		"SELECT (cast(sum(ActionCount)as real) / sum(ActionOpportunities)) as result \
				 FROM	(SELECT	(case when ((HP.street0Calls>0) AND HP.otherRaisedStreet0 AND HP.street1Aggr \
								AND HP.street0Aggressors = 2 \
								AND ((H.seats > 2 AND NOT HP.street0InPosition) \
									OR (H.seats = 2 AND HP.street1InPosition))) then 1 else 0 end) \
								 as ActionCount, \
								(case when ((HP.street0Calls>0) AND HP.otherRaisedStreet0 AND HP.street1_2BChance \
								AND HP.street0Aggressors=2 \
								AND((H.seats>2 AND NOT HP.street0InPosition) \
									OR(H.seats=2 AND HP.street1InPosition))) then 1 else 0 end) \
								 as ActionOpportunities \
				 FROM	HandsPlayers as HP, Hands as H, Players as P, Gametypes as G \
						%TOURNEYTYPESELECT% \
				 WHERE	H.startTime > date('now','-%TIMEPERIOD%') AND \
						%LIMIT&STAGESTATEMENT% \
						%TOURNEYTABLETYPE% \
						%POSITIONSTATEMENT% \
						HP.handId = H.id AND \
						H.gametypeId = G.id AND \
						G.type = '%GAMETYPE%' AND \
						G.category Like '%GAMECAT%' AND \
						HP.playerId = P.id AND \
						P.siteId = %SITEID% AND \
						P.name Like '%SCREENNAME%' \
					ORDER BY H.startTime DESC \
					LIMIT %NBHANDS% \
				 )foo; ",
	// stat_group
	fpdb_group_advanced
	},
#endif

#if FPDB_QUERY_SUPPORT__TURN_FOLD_TO_3B
	// FPDB  query to get TURN fold to 3 bet 
	{
		// name
		"turn_fold_to_3bet",
		// description_for_editor
		"FPDB folded while facing 3bet turn",
		// query
		"SELECT (cast(sum(ActionCount)as real) / sum(ActionOpportunities)) as result \
			FROM	(SELECT	(case when street2_FoldTo3BDone then 1 else 0 end) \
						 as ActionCount, \
						(case when street2_FoldTo3BChance then 1 else 0 end) \
						 as ActionOpportunities \
			FROM	HandsPlayers as HP, Hands as H, Players as P, Gametypes as G \
					%TOURNEYTYPESELECT% \
			WHERE	H.startTime > date('now','-%TIMEPERIOD%') AND \
					%LIMIT&STAGESTATEMENT% \
					%TOURNEYTABLETYPE% \
					%POSITIONSTATEMENT% \
					HP.handId = H.id AND \
					H.gametypeId = G.id AND \
					G.type = '%GAMETYPE%' AND \
					G.category Like '%GAMECAT%' AND \
					HP.playerId = P.id AND \
					P.siteId = %SITEID% AND \
					P.name Like '%SCREENNAME%' \
				ORDER BY H.startTime DESC \
				LIMIT %NBHANDS% \
			)foo; ",
	// stat_group
	fpdb_group_advanced
	},
#endif

#if FPDB_QUERY_SUPPORT__TURN_CHECK_RAISE
	// FPDB  query to get TURN Check-Raise 
	{
		// name
		"turn_checkraise",
		// description_for_editor
		"FPDB turn Check-Raise",
		// query
		"SELECT (cast(sum(ActionCount)as real) / sum(ActionOpportunities)) as result \
			FROM	(SELECT	(case when street2CheckRaiseDone then 1 else 0 end) \
						 as ActionCount, \
						(case when street2CheckCallRaiseChance then 1 else 0 end) \
						 as ActionOpportunities \
			FROM	HandsPlayers as HP, Hands as H, Players as P, Gametypes as G \
					%TOURNEYTYPESELECT% \
			WHERE	H.startTime > date('now','-%TIMEPERIOD%') AND \
					%LIMIT&STAGESTATEMENT% \
					%TOURNEYTABLETYPE% \
					%POSITIONSTATEMENT% \
					HP.handId = H.id AND \
					H.gametypeId = G.id AND \
					G.type = '%GAMETYPE%' AND \
					G.category Like '%GAMECAT%' AND \
					HP.playerId = P.id AND \
					P.siteId = %SITEID% AND \
					P.name Like '%SCREENNAME%' \
				ORDER BY H.startTime DESC \
				LIMIT %NBHANDS% \
			)foo; ",
	// stat_group
	fpdb_group_advanced
	},
#endif

#if FPDB_QUERY_SUPPORT__TURN_CHECK_CALL
	// FPDB  query to get TURN Check-Call 
	{
		// name
		"turn_checkcall",
		// description_for_editor
		"FPDB turn Check-Call",
		// query
		"SELECT (cast(sum(ActionCount)as real) / sum(ActionOpportunities)) as result \
			FROM	(SELECT	(case when street2CheckCallDone then 1 else 0 end) \
						 as ActionCount, \
						(case when street2CheckCallRaiseChance then 1 else 0 end) \
						 as ActionOpportunities \
			FROM	HandsPlayers as HP, Hands as H, Players as P, Gametypes as G \
					%TOURNEYTYPESELECT% \
			WHERE	H.startTime > date('now','-%TIMEPERIOD%') AND \
					%LIMIT&STAGESTATEMENT% \
					%TOURNEYTABLETYPE% \
					%POSITIONSTATEMENT% \
					HP.handId = H.id AND \
					H.gametypeId = G.id AND \
					G.type = '%GAMETYPE%' AND \
					G.category Like '%GAMECAT%' AND \
					HP.playerId = P.id AND \
					P.siteId = %SITEID% AND \
					P.name Like '%SCREENNAME%' \
				ORDER BY H.startTime DESC \
				LIMIT %NBHANDS% \
			)foo; ",
	// stat_group
	fpdb_group_advanced
	},
#endif

#if FPDB_QUERY_SUPPORT__RIVER_FOLD_TO_3B
	// FPDB  query to get RIVER fold to 3 bet 
	{
		// name
		"river_fold_to_3bet",
		// description_for_editor
		"FPDB folded while facing 3bet river",
		// query
		"SELECT (cast(sum(ActionCount)as real) / sum(ActionOpportunities)) as result \
			FROM	(SELECT	(case when street3_FoldTo3BDone then 1 else 0 end) \
						 as ActionCount, \
						(case when street3_FoldTo3BChance then 1 else 0 end) \
						 as ActionOpportunities \
			FROM	HandsPlayers as HP, Hands as H, Players as P, Gametypes as G \
					%TOURNEYTYPESELECT% \
			WHERE	H.startTime > date('now','-%TIMEPERIOD%') AND \
					%LIMIT&STAGESTATEMENT% \
					%TOURNEYTABLETYPE% \
					%POSITIONSTATEMENT% \
					HP.handId = H.id AND \
					H.gametypeId = G.id AND \
					G.type = '%GAMETYPE%' AND \
					G.category Like '%GAMECAT%' AND \
					HP.playerId = P.id AND \
					P.siteId = %SITEID% AND \
					P.name Like '%SCREENNAME%' \
				ORDER BY H.startTime DESC \
				LIMIT %NBHANDS% \
			)foo; ",
	// stat_group
	fpdb_group_advanced
	},
#endif

#if FPDB_QUERY_SUPPORT__RIVER_BET
	// FPDB  query to get River bet 
	{
		// name
		"river_bet",
		// description_for_editor
		"FPDB river bet",
		// query
		"SELECT (case when ActionOpportunities = 0 then -1 \
				 else cast(ActionCount as real) / ActionOpportunities \
				 end) as result \
			FROM	(SELECT	sum(case when HP.street3Raises>0 then 1 else 0 end) \
						 as ActionCount, \
						count(*) \
						 as ActionOpportunities \
			FROM	HandsPlayers as HP, Hands as H, Players as P, Gametypes as G \
					%TOURNEYTYPESELECT% \
			WHERE	H.startTime > date('now','-%TIMEPERIOD%') AND \
					%LIMIT&STAGESTATEMENT% \
					%TOURNEYTABLETYPE% \
					%POSITIONSTATEMENT% \
					HP.handId = H.id AND \
					H.gametypeId = G.id AND \
					G.type = '%GAMETYPE%' AND \
					G.category Like '%GAMECAT%' AND \
					HP.playerId = P.id AND \
					P.siteId = %SITEID% AND \
					P.name Like '%SCREENNAME%' \
				ORDER BY H.startTime DESC \
				LIMIT %NBHANDS% \
			)foo; ",
	// stat_group
	fpdb_group_advanced
	},
#endif

#if FPDB_QUERY_SUPPORT__4BET_PLUS_RATIO
	// FPDB 
	{
		// name
		"4bet_plus_ratio",
		// description_for_editor
		"FPDB 4bet + ratio",
		// query
		"SELECT (cast(sum(case when HP.street0Raises > 0 then 1 else 0 end) as decimal) / \
		(cast(sum(case when HP.handId > 0 then 1 else 0 end) as decimal) - cast(sum(case when HA.actionID = 17 then 1 else 0 end) as decimal))) * \
		(cast(sum(case when HP.raisedFirstIn and HP.street0_4BDone then 1 else 0 end) as decimal) / \
			cast(sum(case when HP.street0_4BChance AND HP.raisedFirstIn then 1 else 0 end) as decimal)) as result \
			FROM	HandsPlayers as HP, HandsActions as HA, Hands as H, Players as P, Gametypes as G \
					%TOURNEYTYPESELECT% \
			WHERE	H.startTime > date('now','-%TIMEPERIOD%') AND \
					%LIMIT&STAGESTATEMENT% \
					%TOURNEYTABLETYPE% \
					%POSITIONSTATEMENT% \
					HP.handId = HA.handId AND \
					HP.handId = H.id AND \
					H.gametypeId = G.id AND \
					G.type = '%GAMETYPE%' AND \
					G.category Like '%GAMECAT%' AND \
					HP.playerId = P.id AND \
					P.siteId = %SITEID% AND \
					P.name Like '%SCREENNAME%' \
				ORDER BY count(H.startTime) DESC \
				LIMIT %NBHANDS% \
				",
	// stat_group
	fpdb_group_advanced
	},
#endif

#if FPDB_QUERY_SUPPORT__FOLD_TO_PF_SQUEEZE
	// FPDB 
	{
		// name
		"fold_to_pf_squeeze",
		// description_for_editor
		"FPDB fold to pf squeeze",
		// query
		"SELECT ( cast(sum(case when HP.street0_FoldToSqueezeChance AND HP.street0_FoldToSqueezeDone then 1 else 0 end) as decimal) / \
		cast(sum(case when HP.street0_FoldToSqueezeChance then 1 else 0 end) as decimal) ) as result \
			FROM	HandsPlayers as HP, Hands as H, Players as P, Gametypes as G \
					%TOURNEYTYPESELECT% \
			WHERE	H.startTime > date('now','-%TIMEPERIOD%') AND \
					%LIMIT&STAGESTATEMENT% \
					%TOURNEYTABLETYPE% \
					%POSITIONSTATEMENT% \
					HP.handId = H.id AND \
					H.gametypeId = G.id AND \
					G.type = '%GAMETYPE%' AND \
					G.category Like '%GAMECAT%' AND \
					HP.playerId = P.id AND \
					P.siteId = %SITEID% AND \
					P.name Like '%SCREENNAME%' \
				ORDER BY count(H.startTime) DESC \
				LIMIT %NBHANDS% \
				",
	// stat_group
	fpdb_group_advanced
	},
#endif

#if FPDB_QUERY_SUPPORT__FOLD_TO_SQUEEZE_AS_RAISER
	// FPDB 
	{
		// name
		"fold_to_squeeze_as_raiser",
		// description_for_editor
		"FPDB fold to squeeze as raiser",
		// query
		"SELECT ( cast(sum(case when HP.street0_FoldToSqueezeChance AND HP.street0_FoldToSqueezeDone AND HP.raisedFirstIn then 1 else 0 end) as decimal) / \
		cast(sum(case when HP.street0_FoldToSqueezeChance AND HP.raisedFirstIn then 1 else 0 end) as decimal) ) as result \
			FROM	HandsPlayers as HP, Hands as H, Players as P, Gametypes as G \
					%TOURNEYTYPESELECT% \
			WHERE	H.startTime > date('now','-%TIMEPERIOD%') AND \
					%LIMIT&STAGESTATEMENT% \
					%TOURNEYTABLETYPE% \
					%POSITIONSTATEMENT% \
					HP.handId = H.id AND \
					H.gametypeId = G.id AND \
					G.type = '%GAMETYPE%' AND \
					G.category Like '%GAMECAT%' AND \
					HP.playerId = P.id AND \
					P.siteId = %SITEID% AND \
					P.name Like '%SCREENNAME%' \
				ORDER BY count(H.startTime) DESC \
				LIMIT %NBHANDS% \
				",
	// stat_group
	fpdb_group_advanced
	},
#endif

#if FPDB_QUERY_SUPPORT__FOLD_TO_SQUEEZE_AS_CALLER
	// FPDB 
	{
		// name
		"fold_to_squeeze_as_caller",
		// description_for_editor
		"FPDB fold to squeeze as caller",
		// query
		"SELECT ( cast(sum(case when HP.street0_FoldToSqueezeChance AND HP.street0_FoldToSqueezeDone AND NOT HP.raisedFirstIn then 1 else 0 end) as decimal) / \
		cast(sum(case when HP.street0_FoldToSqueezeChance AND NOT HP.raisedFirstIn then 1 else 0 end) as decimal) ) as result \
			FROM	HandsPlayers as HP, Hands as H, Players as P, Gametypes as G \
					%TOURNEYTYPESELECT% \
			WHERE	H.startTime > date('now','-%TIMEPERIOD%') AND \
					%LIMIT&STAGESTATEMENT% \
					%TOURNEYTABLETYPE% \
					%POSITIONSTATEMENT% \
					HP.handId = H.id AND \
					H.gametypeId = G.id AND \
					G.type = '%GAMETYPE%' AND \
					G.category Like '%GAMECAT%' AND \
					HP.playerId = P.id AND \
					P.siteId = %SITEID% AND \
					P.name Like '%SCREENNAME%' \
				ORDER BY count(H.startTime) DESC \
				LIMIT %NBHANDS% \
				",
	// stat_group
	fpdb_group_advanced
	},
#endif

#if FPDB_QUERY_SUPPORT__DONK_FLOP_IN_3BET_PLUS_POT
	// FPDB 
	{
		// name
		"donk_flop_in_3bet_plus_pot",
		// description_for_editor
		"Flop Donk Bet in 3Bet+ Pot %",
		// query
		"SELECT (cast(sum(ActionCount)as real) / sum(ActionOpportunities)) as result \
		FROM (SELECT (CASE WHEN(HP.street0_FoldTo3BChance OR HP.street0_FoldTo4BChance) AND NOT(HP.street0_3BDone OR HP.street0_4BDone) AND HP.street1Aggr AND NOT(HP.street1CBChance) AND((H.seats > 2 AND HP.street0InPosition) or (H.seats = 2 and HP.position = 'B')) THEN 1 ELSE 0 END) \
				 as ActionCount, \
				 (CASE WHEN(HP.street0_FoldTo3BChance OR HP.street0_FoldTo4BChance) AND NOT(HP.street0_3BDone OR HP.street0_4BDone) AND(HP.raiseFirstInChance) AND NOT(HP.street1CBChance) AND((H.seats > 2 AND HP.street0InPosition) or (H.seats = 2 AND HP.position = 'B')) THEN 1 ELSE 0 END) \
				 as ActionOpportunities \
			FROM		HandsPlayers as HP, Hands as H, Players as P, Gametypes as G \
						%TOURNEYTYPESELECT% \
			WHERE		H.startTime > date('now','-%TIMEPERIOD%') AND \
						%LIMIT&STAGESTATEMENT% \
						%TOURNEYTABLETYPE% \
						%POSITIONSTATEMENT% \
						HP.handId = H.id AND \
						H.gametypeId = G.id AND \
						G.type = '%GAMETYPE%' AND \
						G.category Like '%GAMECAT%' AND \
						HP.playerId = P.id AND \
						P.siteId = %SITEID% AND \
						P.name Like '%SCREENNAME%' \
					ORDER BY H.startTime DESC \
					LIMIT %NBHANDS% \
				)foo; ",
	// stat_group
	fpdb_group_advanced
	},
#endif

#if FPDB_QUERY_SUPPORT__FOLD_TO_F_CBET_IN_3BET_POT
	// FPDB 
	{
		// name
		"fold_to_f_cbet_in_3bet_pot",
		// description_for_editor
		"Fold to Flop Continuation Bet % in 3+Bet Pot",
		// query
		"SELECT (cast(sum(ActionCount)as real) / sum(ActionOpportunities)) as result \
		 FROM	(SELECT	(CASE WHEN (HP.street0_FoldTo3BChance OR HP.street0_FoldTo4BChance) AND HP.foldToStreet1CBDone THEN 1 ELSE 0 END) \
						 as ActionCount, \
				(CASE WHEN (HP.street0_FoldTo3BChance OR HP.street0_FoldTo4BChance) AND HP.foldToStreet1CBChance THEN 1 ELSE 0 END) \
						 as ActionOpportunities \
				FROM	HandsPlayers as HP, Hands as H, Players as P, Gametypes as G \
						%TOURNEYTYPESELECT% \
				WHERE	H.startTime > date('now','-%TIMEPERIOD%') AND \
						%LIMIT&STAGESTATEMENT% \
						%TOURNEYTABLETYPE% \
						%POSITIONSTATEMENT% \
						HP.handId = H.id AND \
						H.gametypeId = G.id AND \
						G.type = '%GAMETYPE%' AND \
						G.category Like '%GAMECAT%' AND \
						HP.playerId = P.id AND \
						P.siteId = %SITEID% AND \
						P.name Like '%SCREENNAME%' \
					ORDER BY H.startTime DESC \
					LIMIT %NBHANDS% \
				)foo; ",
	// stat_group
	fpdb_group_advanced
	},
#endif

#if FPDB_QUERY_SUPPORT__CBET_FLOP_IN_3BET_POT
	// FPDB 
	{
		// name
		"cbet_flop_in_3bet_pot",
		// description_for_editor
		"Flop Continuation Bet % (3Bet Pot)",
		// query
		"SELECT (cast(sum(ActionCount)as real) / sum(ActionOpportunities)) as result \
		 FROM	(SELECT	(CASE WHEN HP.street1CBDone AND (HP.street0_3BDone or HP.street0_4BDone) THEN 1 ELSE 0 END) \
						 as ActionCount, \
				(CASE WHEN HP.street1CBChance AND (HP.street0_3BDone or HP.street0_4BDone) THEN 1 ELSE 0 END) \
						 as ActionOpportunities \
				FROM	HandsPlayers as HP, Hands as H, Players as P, Gametypes as G \
						%TOURNEYTYPESELECT% \
				WHERE	H.startTime > date('now','-%TIMEPERIOD%') AND \
						%LIMIT&STAGESTATEMENT% \
						%TOURNEYTABLETYPE% \
						%POSITIONSTATEMENT% \
						HP.handId = H.id AND \
						H.gametypeId = G.id AND \
						G.type = '%GAMETYPE%' AND \
						G.category Like '%GAMECAT%' AND \
						HP.playerId = P.id AND \
						P.siteId = %SITEID% AND \
						P.name Like '%SCREENNAME%' \
					ORDER BY H.startTime DESC \
					LIMIT %NBHANDS% \
				)foo; ",
	// stat_group
	fpdb_group_advanced
	},
#endif

#if FPDB_QUERY_SUPPORT__FLOP_CHECK_RAISE_IN_3BET_POT
	// FPDB 
	{
		// name
		"flop_check_raise_in_3bet_pot",
		// description_for_editor
		"FPDB flop check-raise in 3Bet Pot %",
		// query
		"SELECT (cast(sum(ActionCount)as real) / sum(ActionOpportunities)) as result \
		 FROM	(SELECT	(CASE WHEN HP.street1CheckRaiseDone AND HP.street0_3BDone THEN 1 ELSE 0 END) \
						 as ActionCount, \
				(CASE WHEN HP.street1CheckCallRaiseChance AND HP.street0_3BDone THEN 1 ELSE 0 END) \
						 as ActionOpportunities \
				FROM	HandsPlayers as HP, Hands as H, Players as P, Gametypes as G \
						%TOURNEYTYPESELECT% \
				WHERE	H.startTime > date('now','-%TIMEPERIOD%') AND \
						%LIMIT&STAGESTATEMENT% \
						%TOURNEYTABLETYPE% \
						%POSITIONSTATEMENT% \
						HP.handId = H.id AND \
						H.gametypeId = G.id AND \
						G.type = '%GAMETYPE%' AND \
						G.category Like '%GAMECAT%' AND \
						HP.playerId = P.id AND \
						P.siteId = %SITEID% AND \
						P.name Like '%SCREENNAME%' \
					ORDER BY H.startTime DESC \
					LIMIT %NBHANDS% \
				)foo; ",
	// stat_group
	fpdb_group_advanced
	},
#endif

#if FPDB_QUERY_SUPPORT__FLOP_CHECK_RAISE_IN_3BET_PLUS_POT
	// FPDB 
	{
		// name
		"flop_check_raise_in_3bet_plus_pot",
		// description_for_editor
		"FPDB flop check-raise in 3Bet+ Pot %",
		// query
		"SELECT (cast(sum(ActionCount)as real) / sum(ActionOpportunities)) as result \
		 FROM	(SELECT	(CASE WHEN HP.street1CheckRaiseDone AND (HP.street0_3BDone or HP.street0_4BDone) THEN 1 ELSE 0 END) \
						 as ActionCount, \
				(CASE WHEN HP.street1CheckCallRaiseChance AND (HP.street0_3BDone or HP.street0_4BDone) THEN 1 ELSE 0 END) \
						 as ActionOpportunities \
				FROM	HandsPlayers as HP, Hands as H, Players as P, Gametypes as G \
						%TOURNEYTYPESELECT% \
				WHERE	H.startTime > date('now','-%TIMEPERIOD%') AND \
						%LIMIT&STAGESTATEMENT% \
						%TOURNEYTABLETYPE% \
						%POSITIONSTATEMENT% \
						HP.handId = H.id AND \
						H.gametypeId = G.id AND \
						G.type = '%GAMETYPE%' AND \
						G.category Like '%GAMECAT%' AND \
						HP.playerId = P.id AND \
						P.siteId = %SITEID% AND \
						P.name Like '%SCREENNAME%' \
					ORDER BY H.startTime DESC \
					LIMIT %NBHANDS% \
				)foo; ",
	// stat_group
	fpdb_group_advanced
	},
#endif

#if FPDB_QUERY_SUPPORT__FOLD_TO_F_CHECK_RAISE
	// FPDB 
	{
		// name
		"fold_to_f_check_raise",
		// description_for_editor
		"Fold Flop Check Raise %",
		// query
		"SELECT (cast(sum(ActionCount)as real) / sum(ActionOpportunities)) as result \
		 FROM	(SELECT	(CASE WHEN HP.street1foldToCheckRaiseDone THEN 1 ELSE 0 END) \
						 as ActionCount, \
				(CASE WHEN HP.street1foldToCheckRaiseChance THEN 1 ELSE 0 END) \
						 as ActionOpportunities \
				FROM	HandsPlayers as HP, Hands as H, Players as P, Gametypes as G \
						%TOURNEYTYPESELECT% \
				WHERE	H.startTime > date('now','-%TIMEPERIOD%') AND \
						%LIMIT&STAGESTATEMENT% \
						%TOURNEYTABLETYPE% \
						%POSITIONSTATEMENT% \
						HP.handId = H.id AND \
						H.gametypeId = G.id AND \
						G.type = '%GAMETYPE%' AND \
						G.category Like '%GAMECAT%' AND \
						HP.playerId = P.id AND \
						P.siteId = %SITEID% AND \
						P.name Like '%SCREENNAME%' \
					ORDER BY H.startTime DESC \
					LIMIT %NBHANDS% \
				)foo; ",
	// stat_group
	fpdb_group_advanced
	},
#endif

#if FPDB_QUERY_SUPPORT__FOLD_TO_F_CHECK_RAISE_3B_PLUS_POT
	// FPDB 
	{
		// name
		"fold_to_f_check_raise_3b_plus_pot",
		// description_for_editor
		"Fold Flop Check Raise % in 3Bet+ Pot",
		// query
		"SELECT (cast(sum(ActionCount)as real) / sum(ActionOpportunities)) as result \
		 FROM	(SELECT	(CASE WHEN HP.street1foldToCheckRaiseDone AND (HP.street0_3BDone or HP.street0_4BDone) THEN 1 ELSE 0 END) \
						 as ActionCount, \
				(CASE WHEN HP.street1foldToCheckRaiseChance AND (HP.street0_3BDone or HP.street0_4BDone) THEN 1 ELSE 0 END) \
						 as ActionOpportunities \
				FROM	HandsPlayers as HP, Hands as H, Players as P, Gametypes as G \
						%TOURNEYTYPESELECT% \
				WHERE	H.startTime > date('now','-%TIMEPERIOD%') AND \
						%LIMIT&STAGESTATEMENT% \
						%TOURNEYTABLETYPE% \
						%POSITIONSTATEMENT% \
						HP.handId = H.id AND \
						H.gametypeId = G.id AND \
						G.type = '%GAMETYPE%' AND \
						G.category Like '%GAMECAT%' AND \
						HP.playerId = P.id AND \
						P.siteId = %SITEID% AND \
						P.name Like '%SCREENNAME%' \
					ORDER BY H.startTime DESC \
					LIMIT %NBHANDS% \
				)foo; ",
	// stat_group
	fpdb_group_advanced
	},
#endif

#if FPDB_QUERY_SUPPORT__FLOAT_FLOP
	// FPDB 
	{
		// name
		"float_flop",
		// description_for_editor
		"Flop Float %",
		// query
		"SELECT (cast(sum(ActionCount)as real) / sum(ActionOpportunities)) as result \
		 FROM	(SELECT	(CASE WHEN HP.callToStreet1CBDone AND HP.street1InPosition AND (HP.street2Aggr or HP.street3Aggr or HP.street4Aggr) THEN 1 ELSE 0 END) \
						 as ActionCount, \
				(CASE WHEN HP.callToStreet1CBChance AND HP.street1InPosition AND (HP.street2Aggr or HP.street3Aggr or HP.street4Aggr) THEN 1 ELSE 0 END) \
						 as ActionOpportunities \
				FROM	HandsPlayers as HP, Hands as H, Players as P, Gametypes as G \
						%TOURNEYTYPESELECT% \
				WHERE	H.startTime > date('now','-%TIMEPERIOD%') AND \
						%LIMIT&STAGESTATEMENT% \
						%TOURNEYTABLETYPE% \
						%POSITIONSTATEMENT% \
						HP.handId = H.id AND \
						H.gametypeId = G.id AND \
						G.type = '%GAMETYPE%' AND \
						G.category Like '%GAMECAT%' AND \
						HP.playerId = P.id AND \
						P.siteId = %SITEID% AND \
						P.name Like '%SCREENNAME%' \
					ORDER BY H.startTime DESC \
					LIMIT %NBHANDS% \
				)foo; ",
	// stat_group
	fpdb_group_advanced
	},
#endif

#if FPDB_QUERY_SUPPORT__FLOAT_FLOP_IN_3BET_POT
	// FPDB 
	{
		// name
		"float_flop_in_3bet_pot",
		// description_for_editor
		"Flop Float in 3Bet Pot %",
		// query
		"SELECT (cast(sum(ActionCount)as real) / sum(ActionOpportunities)) as result \
		 FROM	(SELECT	(CASE WHEN HP.callToStreet1CBDone AND HP.street0_3BDone AND HP.street1InPosition AND (HP.street2Aggr or HP.street3Aggr or HP.street4Aggr) THEN 1 ELSE 0 END) \
						 as ActionCount, \
				(CASE WHEN HP.callToStreet1CBChance AND HP.street0_3BDone AND HP.street1InPosition AND (HP.street2Aggr or HP.street3Aggr or HP.street4Aggr) THEN 1 ELSE 0 END) \
						 as ActionOpportunities \
				FROM	HandsPlayers as HP, Hands as H, Players as P, Gametypes as G \
						%TOURNEYTYPESELECT% \
				WHERE	H.startTime > date('now','-%TIMEPERIOD%') AND \
						%LIMIT&STAGESTATEMENT% \
						%TOURNEYTABLETYPE% \
						%POSITIONSTATEMENT% \
						HP.handId = H.id AND \
						H.gametypeId = G.id AND \
						G.type = '%GAMETYPE%' AND \
						G.category Like '%GAMECAT%' AND \
						HP.playerId = P.id AND \
						P.siteId = %SITEID% AND \
						P.name Like '%SCREENNAME%' \
					ORDER BY H.startTime DESC \
					LIMIT %NBHANDS% \
				)foo; ",
	// stat_group
	fpdb_group_advanced
	},
#endif

#if FPDB_QUERY_SUPPORT__CALL_F_FLOAT_BET
	// FPDB 
	{
		// name
		"call_f_float_bet",
		// description_for_editor
		"Call Flop Float Bet %",
		// query
		"SELECT (cast(sum(ActionCount)as real) / sum(ActionOpportunities)) as result \
		 FROM	(SELECT	(CASE WHEN HP.street1CBDone AND HP.street1Calls>0 AND NOT HP.street1InPosition AND (HP.callToOtherRaisedStreet2 or HP.callToOtherRaisedStreet3 or HP.callToOtherRaisedStreet4) THEN 1 ELSE 0 END) \
						 as ActionCount, \
				(CASE WHEN HP.street1CBDone AND HP.street1Calls>0 AND NOT HP.street1InPosition AND (HP.otherRaisedStreet2 or HP.otherRaisedStreet3 or HP.otherRaisedStreet4) THEN 1 ELSE 0 END) \
						 as ActionOpportunities \
				FROM	HandsPlayers as HP, Hands as H, Players as P, Gametypes as G \
						%TOURNEYTYPESELECT% \
				WHERE	H.startTime > date('now','-%TIMEPERIOD%') AND \
						%LIMIT&STAGESTATEMENT% \
						%TOURNEYTABLETYPE% \
						%POSITIONSTATEMENT% \
						HP.handId = H.id AND \
						H.gametypeId = G.id AND \
						G.type = '%GAMETYPE%' AND \
						G.category Like '%GAMECAT%' AND \
						HP.playerId = P.id AND \
						P.siteId = %SITEID% AND \
						P.name Like '%SCREENNAME%' \
					ORDER BY H.startTime DESC \
					LIMIT %NBHANDS% \
				)foo; ",
	// stat_group
	fpdb_group_advanced
	},
#endif

#if FPDB_QUERY_SUPPORT__CALL_F_FLOAT_BET_IN_3B_POT
	// FPDB 
	{
		// name
		"call_f_float_bet_in_3b_pot",
		// description_for_editor
		"Call Flop Float Bet in 3Bet Pot %",
		// query
		"SELECT (cast(sum(ActionCount)as real) / sum(ActionOpportunities)) as result \
		 FROM	(SELECT	(CASE WHEN HP.street1CBDone AND HP.street0_3BDone AND HP.street1Calls>0 AND NOT HP.street1InPosition AND (HP.callToOtherRaisedStreet2 or HP.callToOtherRaisedStreet3 or HP.callToOtherRaisedStreet4) THEN 1 ELSE 0 END) \
						 as ActionCount, \
				(CASE WHEN HP.street1CBDone AND HP.street0_3BDone AND HP.street1Calls>0 AND NOT HP.street1InPosition AND (HP.otherRaisedStreet2 or HP.otherRaisedStreet3 or HP.otherRaisedStreet4) THEN 1 ELSE 0 END) \
						 as ActionOpportunities \
				FROM	HandsPlayers as HP, Hands as H, Players as P, Gametypes as G \
						%TOURNEYTYPESELECT% \
				WHERE	H.startTime > date('now','-%TIMEPERIOD%') AND \
						%LIMIT&STAGESTATEMENT% \
						%TOURNEYTABLETYPE% \
						%POSITIONSTATEMENT% \
						HP.handId = H.id AND \
						H.gametypeId = G.id AND \
						G.type = '%GAMETYPE%' AND \
						G.category Like '%GAMECAT%' AND \
						HP.playerId = P.id AND \
						P.siteId = %SITEID% AND \
						P.name Like '%SCREENNAME%' \
					ORDER BY H.startTime DESC \
					LIMIT %NBHANDS% \
				)foo; ",
	// stat_group
	fpdb_group_advanced
	},
#endif

#if FPDB_QUERY_SUPPORT__FOLD_TO_F_FLOAT_BET_IN_3B_POT
	// FPDB 
	{
		// name
		"fold_to_f_float_bet_in_3b_pot",
		// description_for_editor
		"Fold to Flop Float Bet in 3Bet Pot %",
		// query
		"SELECT (cast(sum(ActionCount)as real) / sum(ActionOpportunities)) as result \
		 FROM	(SELECT	(CASE WHEN HP.street1CBDone AND HP.street0_3BDone AND HP.street1Calls>0 AND NOT HP.street1InPosition AND (HP.foldToOtherRaisedStreet2 or HP.foldToOtherRaisedStreet3 or HP.foldToOtherRaisedStreet4) THEN 1 ELSE 0 END) \
						 as ActionCount, \
				(CASE WHEN HP.street1CBDone AND HP.street0_3BDone AND HP.street1Calls>0 AND NOT HP.street1InPosition AND (HP.otherRaisedStreet2 or HP.otherRaisedStreet3 or HP.otherRaisedStreet4) THEN 1 ELSE 0 END) \
						 as ActionOpportunities \
				FROM	HandsPlayers as HP, Hands as H, Players as P, Gametypes as G \
						%TOURNEYTYPESELECT% \
				WHERE	H.startTime > date('now','-%TIMEPERIOD%') AND \
						%LIMIT&STAGESTATEMENT% \
						%TOURNEYTABLETYPE% \
						%POSITIONSTATEMENT% \
						HP.handId = H.id AND \
						H.gametypeId = G.id AND \
						G.type = '%GAMETYPE%' AND \
						G.category Like '%GAMECAT%' AND \
						HP.playerId = P.id AND \
						P.siteId = %SITEID% AND \
						P.name Like '%SCREENNAME%' \
					ORDER BY H.startTime DESC \
					LIMIT %NBHANDS% \
				)foo; ",
	// stat_group
	fpdb_group_advanced
	},
#endif

#if FPDB_QUERY_SUPPORT__FOLD_TO_F_FLOAT_BET_IN_3B_PLUS_POT
	// FPDB 
	{
		// name
		"fold_to_f_float_bet_in_3b_plus_pot",
		// description_for_editor
		"Fold to Flop Float Bet in 3Bet+ Pot %",
		// query
		"SELECT (cast(sum(ActionCount)as real) / sum(ActionOpportunities)) as result \
		 FROM	(SELECT	(CASE WHEN HP.street1CBDone AND (HP.street0_3BDone OR HP.street0_4BDone) AND HP.street1Calls>0 AND NOT HP.street1InPosition AND (HP.foldToOtherRaisedStreet2 or HP.foldToOtherRaisedStreet3 or HP.foldToOtherRaisedStreet4) THEN 1 ELSE 0 END) \
						 as ActionCount, \
				(CASE WHEN HP.street1CBDone AND (HP.street0_3BDone OR HP.street0_4BDone) AND HP.street1Calls>0 AND NOT HP.street1InPosition AND (HP.otherRaisedStreet2 or HP.otherRaisedStreet3 or HP.otherRaisedStreet4) THEN 1 ELSE 0 END) \
						 as ActionOpportunities \
				FROM	HandsPlayers as HP, Hands as H, Players as P, Gametypes as G \
						%TOURNEYTYPESELECT% \
				WHERE	H.startTime > date('now','-%TIMEPERIOD%') AND \
						%LIMIT&STAGESTATEMENT% \
						%TOURNEYTABLETYPE% \
						%POSITIONSTATEMENT% \
						HP.handId = H.id AND \
						H.gametypeId = G.id AND \
						G.type = '%GAMETYPE%' AND \
						G.category Like '%GAMECAT%' AND \
						HP.playerId = P.id AND \
						P.siteId = %SITEID% AND \
						P.name Like '%SCREENNAME%' \
					ORDER BY H.startTime DESC \
					LIMIT %NBHANDS% \
				)foo; ",
	// stat_group
	fpdb_group_advanced
	},
#endif

#if FPDB_QUERY_SUPPORT__CBET_FLOP_OPP_IN_3BET_POT
	// FPDB 
	{
		// name
		"cbet_flop_opp_in_3bet_pot",
		// description_for_editor
		"Cbet Flop Opportunities in 3bet + Pot",
		// query
		"SELECT sum(CASE WHEN HP.street1CBChance AND (HP.street0_3BDone OR HP.street0_4BDone) THEN 1 ELSE 0 END) as result \
		 FROM	HandsPlayers as HP, Hands as H, Players as P, Gametypes as G \
						%TOURNEYTYPESELECT% \
				WHERE	H.startTime > date('now','-%TIMEPERIOD%') AND \
						%LIMIT&STAGESTATEMENT% \
						%TOURNEYTABLETYPE% \
						%POSITIONSTATEMENT% \
						HP.handId = H.id AND \
						H.gametypeId = G.id AND \
						G.type = '%GAMETYPE%' AND \
						G.category Like '%GAMECAT%' AND \
						HP.playerId = P.id AND \
						P.siteId = %SITEID% AND \
						P.name Like '%SCREENNAME%' \
					ORDER BY count(H.startTime) DESC \
					LIMIT %NBHANDS% \
				",
	// stat_group
	fpdb_group_advanced
	},
#endif

#if FPDB_QUERY_SUPPORT__CBET_FLOP_OPP
	// FPDB 
	{
		// name
		"cbet_flop_opp",
		// description_for_editor
		"Cbet Flop Opportunities",
		// query
		"SELECT sum(CASE WHEN HP.street1CBChance THEN 1 ELSE 0 END) as result \
		FROM	HandsPlayers as HP, Hands as H, Players as P, Gametypes as G \
						%TOURNEYTYPESELECT% \
				WHERE	H.startTime > date('now','-%TIMEPERIOD%') AND \
						%LIMIT&STAGESTATEMENT% \
						%TOURNEYTABLETYPE% \
						%POSITIONSTATEMENT% \
						HP.handId = H.id AND \
						H.gametypeId = G.id AND \
						G.type = '%GAMETYPE%' AND \
						G.category Like '%GAMECAT%' AND \
						HP.playerId = P.id AND \
						P.siteId = %SITEID% AND \
						P.name Like '%SCREENNAME%' \
					ORDER BY count(H.startTime) DESC \
					LIMIT %NBHANDS% \
				",
	// stat_group
	fpdb_group_advanced
	},
#endif

#if FPDB_QUERY_SUPPORT__FOLD_TO_3BET_AFTER_R_OPP
	// FPDB 
	{
		// name
		"fold_to_3bet_afterR_opp",
		// description_for_editor
		"FPDB folded while facing 3bet preflop Opportunities after raise",
		// query
		"SELECT sum(CASE WHEN(HP.street0_4BChance AND HP.raisedFirstIn) THEN 1 ELSE 0 END) as result \
		FROM	HandsPlayers as HP, Hands as H, Players as P, Gametypes as G \
						%TOURNEYTYPESELECT% \
				WHERE	H.startTime > date('now','-%TIMEPERIOD%') AND \
						%LIMIT&STAGESTATEMENT% \
						%TOURNEYTABLETYPE% \
						%POSITIONSTATEMENT% \
						HP.handId = H.id AND \
						H.gametypeId = G.id AND \
						G.type = '%GAMETYPE%' AND \
						G.category Like '%GAMECAT%' AND \
						HP.playerId = P.id AND \
						P.siteId = %SITEID% AND \
						P.name Like '%SCREENNAME%' \
					ORDER BY count(H.startTime) DESC \
					LIMIT %NBHANDS% \
				",
	// stat_group
	fpdb_group_advanced
	},
#endif

#if FPDB_QUERY_SUPPORT__PREFLOP_SQUEEZE_OPP
	// FPDB 
	{
		// name
		"preflop_squeeze_opp",
		// description_for_editor
		"Squeeze Preflop % Opportunities",
		// query
		"SELECT sum(CASE WHEN HP.street0_SqueezeChance THEN 1 ELSE 0 END) as result \
		FROM	HandsPlayers as HP, Hands as H, Players as P, Gametypes as G \
						%TOURNEYTYPESELECT% \
				WHERE	H.startTime > date('now','-%TIMEPERIOD%') AND \
						%LIMIT&STAGESTATEMENT% \
						%TOURNEYTABLETYPE% \
						%POSITIONSTATEMENT% \
						HP.handId = H.id AND \
						H.gametypeId = G.id AND \
						G.type = '%GAMETYPE%' AND \
						G.category Like '%GAMECAT%' AND \
						HP.playerId = P.id AND \
						P.siteId = %SITEID% AND \
						P.name Like '%SCREENNAME%' \
					ORDER BY count(H.startTime) DESC \
					LIMIT %NBHANDS% \
				",
	// stat_group
	fpdb_group_advanced
	},
#endif

#if FPDB_QUERY_SUPPORT__FLOP_FOLD_TO_CBET__FLOP_RAISE_CBET_OPP
	// FPDB 
	{
		// name
		"flop_fold_to_cbet__flop_raise_cbet_opp",
		// description_for_editor
		"Opportunities for 2 stats Flop R Cbet and Flop fold to cbet",
		// query
		"SELECT sum(CASE WHEN S.raiseToStreet1CBChance THEN 1 ELSE 0 END) as result \
		FROM	HandsPlayers as HP, Hands as H, Players as P, Gametypes as G \
						%TOURNEYTYPESELECT% \
				WHERE	H.startTime > date('now','-%TIMEPERIOD%') AND \
						%LIMIT&STAGESTATEMENT% \
						%TOURNEYTABLETYPE% \
						%POSITIONSTATEMENT% \
						HP.handId = H.id AND \
						H.gametypeId = G.id AND \
						G.type = '%GAMETYPE%' AND \
						G.category Like '%GAMECAT%' AND \
						HP.playerId = P.id AND \
						P.siteId = %SITEID% AND \
						P.name Like '%SCREENNAME%' \
					ORDER BY count(H.startTime) DESC \
					LIMIT %NBHANDS% \
				",
	// stat_group
	fpdb_group_advanced
	},
#endif

#if FPDB_QUERY_SUPPORT__FLOP_DONKBET_OPP
	// FPDB 
	{
		// name
		"flop_donkbet_opp",
		// description_for_editor
		"FPDB donk flop Opportunities",
		// query
		"SELECT sum(case when (HP.street0_3BChance AND NOT(HP.street0_3BDone OR HP.street0_4BDone) \
			AND(HP.street1_2BChance) AND NOT(HP.street1CBChance) AND \
			((H.seats > 2 and HP.street0InPosition) or \
			(H.seats = 2 and HP.position = 'B'))) then 1 else 0 end) as result \
		FROM	HandsPlayers as HP, Hands as H, Players as P, Gametypes as G \
						%TOURNEYTYPESELECT% \
				WHERE	H.startTime > date('now','-%TIMEPERIOD%') AND \
						%LIMIT&STAGESTATEMENT% \
						%TOURNEYTABLETYPE% \
						%POSITIONSTATEMENT% \
						HP.handId = H.id AND \
						H.gametypeId = G.id AND \
						G.type = '%GAMETYPE%' AND \
						G.category Like '%GAMECAT%' AND \
						HP.playerId = P.id AND \
						P.siteId = %SITEID% AND \
						P.name Like '%SCREENNAME%' \
					ORDER BY count(H.startTime) DESC \
					LIMIT %NBHANDS% \
				",
	// stat_group
	fpdb_group_advanced
	},
#endif

#if FPDB_QUERY_SUPPORT__FOLD_TO_F_CBET_IN_3BET_PLUS_POT_OPP
	// FPDB 
	{
		// name
		"fold_to_f_cbet_in_3bet_plus_pot_opp",
		// description_for_editor
		"Fold to Flop Continuation Bet % in 3+Bet Pot Opportunities",
		// query
		"SELECT sum(CASE WHEN HP.street0_4BChance AND HP.raiseToStreet1CBChance THEN 1 ELSE 0 END) as result \
		FROM	HandsPlayers as HP, Hands as H, Players as P, Gametypes as G \
						%TOURNEYTYPESELECT% \
				WHERE	H.startTime > date('now','-%TIMEPERIOD%') AND \
						%LIMIT&STAGESTATEMENT% \
						%TOURNEYTABLETYPE% \
						%POSITIONSTATEMENT% \
						HP.handId = H.id AND \
						H.gametypeId = G.id AND \
						G.type = '%GAMETYPE%' AND \
						G.category Like '%GAMECAT%' AND \
						HP.playerId = P.id AND \
						P.siteId = %SITEID% AND \
						P.name Like '%SCREENNAME%' \
					ORDER BY count(H.startTime) DESC \
					LIMIT %NBHANDS% \
				",
	// stat_group
	fpdb_group_advanced
	},
#endif

#if FPDB_QUERY_SUPPORT__FOLD_TO_F_CHECK_RAISE_OPP
	// FPDB 
	{
		// name
		"fold_to_f_check_raise_opp",
		// description_for_editor
		"Fold to Flop Ckeck raise Opportunities",
		// query
		"SELECT sum(CASE WHEN HP.street1foldToCheckRaiseChance THEN 1 ELSE 0 END) as result \
		FROM	HandsPlayers as HP, Hands as H, Players as P, Gametypes as G \
						%TOURNEYTYPESELECT% \
				WHERE	H.startTime > date('now','-%TIMEPERIOD%') AND \
						%LIMIT&STAGESTATEMENT% \
						%TOURNEYTABLETYPE% \
						%POSITIONSTATEMENT% \
						HP.handId = H.id AND \
						H.gametypeId = G.id AND \
						G.type = '%GAMETYPE%' AND \
						G.category Like '%GAMECAT%' AND \
						HP.playerId = P.id AND \
						P.siteId = %SITEID% AND \
						P.name Like '%SCREENNAME%' \
					ORDER BY count(H.startTime) DESC \
					LIMIT %NBHANDS% \
				",
	// stat_group
	fpdb_group_advanced
	},
#endif

#if FPDB_QUERY_SUPPORT__FOLD_TO_SQUEEZE_AS_RAISER_OPP
	// FPDB 
	{
		// name
		"fold_to_squeeze_as_raiser_opp",
		// description_for_editor
		"Fold to Squeeze As Raiser Opportunities",
		// query
		"SELECT cast(sum(case when HP.street0_FoldToSqueezeChance AND HP.raisedFirstIn then 1 else 0 end) as decimal) as result \
		FROM	HandsPlayers as HP, Hands as H, Players as P, Gametypes as G \
						%TOURNEYTYPESELECT% \
				WHERE	H.startTime > date('now','-%TIMEPERIOD%') AND \
						%LIMIT&STAGESTATEMENT% \
						%TOURNEYTABLETYPE% \
						%POSITIONSTATEMENT% \
						HP.handId = H.id AND \
						H.gametypeId = G.id AND \
						G.type = '%GAMETYPE%' AND \
						G.category Like '%GAMECAT%' AND \
						HP.playerId = P.id AND \
						P.siteId = %SITEID% AND \
						P.name Like '%SCREENNAME%' \
					ORDER BY count(H.startTime) DESC \
					LIMIT %NBHANDS% \
				",
	// stat_group
	fpdb_group_advanced
	},
#endif

};