#include "pch.h"
#include "OpenPPL_ErrorsWarnings.h"

#include "CPreferences.h"
#include "MagicNumbers.h"

/*##f$OpenPPL_ERROR_THIS_SHOULD_NOT_HAPPEN__THERE_SEEMS_TO_BE_A_PROBLEM_IN_THE_LIBRARY##
##f$OpenPPL_ERROR_Blinds_Not_Recognized_Correctly__Check_Your_TableMap##
##f$OpenPPL_ERROR_Raises_Is_Negative__Trying_To_Adapt_In_A_Reasonable_Way##
##f$OpenPPL_ERROR_Suit_Of_Card_Not_Recognized_Correctly__Check_Your_TableMap##
##f$OpenPPL_ERROR_Rank_Of_Card_Not_Recognized_Correctly__Check_Your_TableMap##
##f$OpenPPL_ERROR_Betround_Out_Of_Range##
##f$OpenPPL_ERROR_BotsActionsOnThisRound_Out_Of_Range##
##f$OpenPPL_ERROR_ButtonChair_Not_Recognized__Check_Your_TableMap##
##f$OpenPPL_ERROR_CutOffChair_Not_Recognized__Check_Your_TableMap##
##f$OpenPPL_ERROR_MiddlePosition3Chair_Not_Recognized__Check_Your_TableMap##
##f$OpenPPL_ERROR_MiddlePosition2Chair_Not_Recognized__Check_Your_TableMap##
##f$OpenPPL_ERROR_MiddlePosition1Chair_Not_Recognized__Check_Your_TableMap##
##f$OpenPPL_ERROR_EarlyPosition3Chair_Not_Recognized__Check_Your_TableMap##
##f$OpenPPL_ERROR_EarlyPosition2Chair_Not_Recognized__Check_Your_TableMap##
##f$OpenPPL_ERROR_EarlyPosition1Chair_Not_Recognized__Check_Your_TableMap##
##f$OpenPPL_ERROR_BigBlindChair_Not_Recognized__Check_Your_TableMap##
##f$OpenPPL_ERROR_SmallBlindChair_Not_Recognized__Check_Your_TableMap##
##f$OpenPPL_ERROR_DealPosition1Chair_Not_Recognized__Check_Your_TableMap##
##f$OpenPPL_ERROR_We_Dont_Have_Any_Straight__No_Not_Call_This_Function##
##f$OpenPPL_ERROR_Dealposition_Out_Of_Range__Check_Your_TableMap##
##f$OpenPPL_ERROR_We_Dont_Have_Any_Straight__Do_Not_Call_This_Function##
##f$OpenPPL_ERROR_No_Opponents_Detected__Check_Your_TableMap##
##f$OpenPPL_ERROR_Game_Is_Not_HeadsUp##
##f$OpenPPL_ERROR_You_Dont_Have_Any_Kicker##
##f$OpenPPL_HINT_We_Dont_Have_A_FullHouse_At_All##
##f$OpenPPL_HINT_There_Are_No_True_Raisers##
##f$OpenPPL_HINT_Such_A_Straight_Does_Not_Exist##
##f$OpenPPL_HINT_Something_Is_Wrong__It_Seems_You_Are_Not_Even_Seated##
##f$OpenPPL_HINT_EarlyPosition1_Does_Not_Exist##
##f$OpenPPL_HINT_EarlyPosition2_Does_Not_Exist##
##f$OpenPPL_HINT_EarlyPosition3_Does_Not_Exist##
##f$OpenPPL_HINT_MiddlePosition1_Does_Not_Exist##
##f$OpenPPL_HINT_MiddlePosition2_Does_Not_Exist##
##f$OpenPPL_HINT_MiddlePosition3_Does_Not_Exist##
##f$OpenPPL_HINT_CutOff_Does_Not_Exist##
##f$OpenPPL_HINT_There_Is_No_SmallNlind_In_This_Game##
##f$OpenPPL_HINT_There_Is_No_Raiser_Therefore_No_CurrentBet##
##f$OpenPPL_HINT_There_Is_No_Pair_On_The_Board##
##f$OpenPPL_HINT_Nothing_To_Do_But_All_OK##
##f$OpenPPL_HINT_SuitsOnBoard_Undefined_For_This_Betting_Round##
##f$OpenPPL_HINT_StillToAct_Is_Only_Valid_On_First_Orbit##
##f$OpenPPL_HINT_There_Are_No_Common_Cards_Preflop##
##f$OpenPPL_HINT_All_Opponents_Allin##
##f$OpenPPL_HINT_No_Oppponent_Or_Not_Headsup##
##f$OpenPPL_HINT_No_FirstCaller##
##f$OpenPPL_HINT_No_LastCaller##
##f$OpenPPL_HINT_No_FirstRaiser##
##f$OpenPPL_HINT_No_Last_Raiser##
##f$OpenPPL_HINT_No_Last_Raiser_Or_Not_Enough_Hands##
##f$OpenPPL_HINT_We_Wont_Close_The_Table_While_Not_Sitting_Out##
*/

void OpenPPL_Error(CString message)
{
	CString complete_message;
	complete_message.Format("[OpenPPL] ERROR: %s\n", message);
	write_log(k_always_log_errors, (char*)&complete_message);
}

void OpenPPL_Warning(CString message)
{
	CString complete_message;
	complete_message.Format("[OpenPPL] Warning: %s\n", message);
	write_log(preferences.debug_symbolengine_open_ppl(), 
		(char*)&complete_message);
}

int OpenPPL_ERROR_THIS_SHOULD_NOT_HAPPEN__THERE_SEEMS_TO_BE_A_PROBLEM_IN_THE_LIBRARY()
{
	OpenPPL_Error("THIS SHOULD NOT HAPPEN. THERE SEEMS TO BE A PROBLEM IN THE LIBRARY");
	return k_undefined;
}

int OpenPPL_ERROR_Blinds_Not_Recognized_Correctly__Check_Your_TableMap()
{
	OpenPPL_Error("Blinds Not Recognized Correctly. Check Your TableMap");
	return k_undefined;
}