#pragma once
//******************************************************************************
//
// This file is part of the OpenHoldem project
//    Source code:           https://github.com/OpenHoldem/openholdembot/
//    Forums:                http://www.maxinmontreal.com/forums/index.php
//    Licensed under GPL v3: http://www.gnu.org/licenses/gpl.html
//
//******************************************************************************
//
// Purpose: Constants for OpenHoldem
//
//******************************************************************************

// ATTENTION!
//   Do only use elementary data-types here.
//   Complex data-types like objects (especially CStrings)
//   will cause problems during initialization and termination,
//   if they get accessed but are not yet initialized or no longer valid.
//   http://www.maxinmontreal.com/forums/viewtopic.php?f=111&t=11724
//   (Same problem like the singletons in OpenHoldem.cpp)

#include "assert.h"

// Constants that are used in the DLL-interface
// are defined there: user.h

// Used for bit-calculations: 2^N
const int k_exponents[11] = {1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024};
const int k_bits_all_ten_players_1_111_111_111 = 0x03FF;

// Undefined (-1)
const int kNotFound      = -1;
const int kUndefined     = -1;
const int kUndefinedZero =  0;
const int kNoAbsoluteLineNumberExists = 1;

// Cards
const int CARD_BACK		   = 0xff;
const int CARD_NOCARD	   = 0xfe;
const int CARD_UNDEFINED = 0xfd;

// CString compare results
// http://msdn.microsoft.com/en-us/library/aa314313(v=vs.60).aspx
// http://www.cplusplus.com/reference/cstring/strcmp/
const int kStringsEqual = 0;

// Betting rounds
// Some data-structures use an additional value for the current round.
// This is IMO (THF) superfluous and  dangerous, but lots of work
// and even more dangerous to change.
const int kNumberOfBetrounds = 4;
const int kBetroundCurrent	 = 0;
const int kBetroundPreflop	 = 1;
const int kBetroundFlop	     = 2;
const int kBetroundTurn	     = 3;
const int kBetroundRiver     = 4;

// Aztoplayer FCKRA-bits
const int kMyTurnBitsFold = 0x01;
const int kMyTurnBitsCall = 0x02;
const int kMyTurnBitsCheck = 0x04;
const int kMyTurnBitsRaise = 0x08;
const int kMyTurnBitsAllin = 0x10;

// Players
const int kMaxNumberOfPlayers         = 10;
const int kMaxLengthOfPlayername      = 32;
const int kUsualNumberOfBlindPosters  =  2;
const int kFirstChair                 =  0;
const int kLastChair                  = (kMaxNumberOfPlayers - 1);
const int kMaxNumberOfOpponentsAtFullRingTable = (kMaxNumberOfPlayers - 1);

// Chairs
const int kMinChairNumber = 0;
const int kMaxChairNumber = kMaxNumberOfPlayers - 1;

// Number of cards
const int kNumberOfCommunityCards       =   5;
const int kIndexOfLastCommunityCard     = kNumberOfCommunityCards - 1;
const int kIndexOfRiverCard             = kIndexOfLastCommunityCard;
const int kIndexOfTurnCard              = kIndexOfRiverCard - 1;
const int kNumberOfFlopCards            =   3;  
const int kNumberOfCardsPerPlayerHoldEm =   2;
const int kNumberOfCardsPerPlayerOmaha  =   4;
const int kMaxNumberOfCardsPerPlayer    = kNumberOfCardsPerPlayerOmaha;
const int kNumberOfCardsPerDeck         =  52;
const int kNumberOfStartingHands        = 169;

// poker constants
// The type has to be unsigned int, 
// because straightflush and royalflush excide the precision of (signed) int.
// For the same reason the bitwise operations in the symbol-engine
// have to be executed with unsigned ints too.
const unsigned int k_pokerval_hicard        = 0x00000001;
const unsigned int k_pokerval_onepair       = 0x01000000;
const unsigned int k_pokerval_twopair       = 0x02000000;
const unsigned int k_pokerval_threeofakind  = 0x04000000;
const unsigned int k_pokerval_straight      = 0x08000000;
const unsigned int k_pokerval_flush         = 0x10000000;
const unsigned int k_pokerval_fullhouse     = 0x20000000;
const unsigned int k_pokerval_fourofakind   = 0x40000000;
const unsigned int k_pokerval_straightflush = 0x80000000;
const unsigned int k_pokerval_royalflush    = 0x800EDCBA;

// Game type
const int kGametypeUnknown = -1;
const int kGametypeNL      =  0;
const int kGametypePL    	 =  1;
const int kGametypeFL      =  2;

// Session IDS (max number of tables)
static const int MAX_SESSION_IDS = 25;

// buttons and other scraper constants
const int k_max_number_of_buttons = 36;      // Indexed as 0..9, A..Z
const int k_max_number_of_i86X_buttons = 10;
const int k_max_betpot_buttons = 7;				   // 2/1, 1/1, 3/4, 2/3, 1/2, 1/3, 1/4
const int k_min_buttons_needed_for_my_turn = 2;
const int k_my_turn_bits_fold_call_raise = 0x07;

// button codes
const int k_button_i3        = 3;
const int k_button_fold      = 10;
const int k_button_allin     = 11;
const int k_button_raise     = 12;
const int k_button_call      = 13;
const int k_button_check     = 14;
const int k_button_sitin     = 20;
const int k_button_sitout    = 21;
const int k_button_leave     = 22;
const int k_button_i86       = 86; 
const int k_button_rematch   = 23;
const int k_button_prefold   = 24;
const int k_button_autopost  = 25;
const int k_button_undefined = -1;

const int k_double_click_delay = 100; // ms

// Tablemap constants
#define RGB_MASK	0x00FFFFFF
const int k_max_number_of_font_groups_in_tablemap = 10;
const int k_max_number_of_hash_groups_in_tablemap = 4;
const int k_max_number_of_titletexts = 10;
const int k_max_number_of_tablepoints = 10;

// GUI
// Width just like ManualMode
// to avoid resizing on attach top / attach bottom. ;-)
const int kMainSizeX = 580;
const int kMainSizeY = 400;

// File menu
const int kNumberOfLastRecentlyUsedFilesInFileMenu = 4;

// Number of (side)pots
const int kMaxNumberOfPots = 10;
const int kLastPot = kMaxNumberOfPots - 1;

// Flags
const int kNumberOfFlags = 20;

// Suits and ranks
const int k_number_of_suits_per_deck  =  4;
const int k_number_of_ranks_per_deck  = 13;
const int k_rank_ace                  = 14; 
const int k_rank_king                 = k_rank_ace - 1;
const int k_rank_queen                = k_rank_ace - 2;
const int k_rank_jack                 = k_rank_ace - 3; 
const int k_rank_ten                  = k_rank_ace - 4;
const int k_rank_ace_low              =  1;
const int k_rankbits_all_cards_111_111_111_111_110 = 0x7FFE;

// straingts and flushes
const int k_cards_needed_for_flush    =  5;
const int k_cards_needed_for_straight =  5;

// Regions for the region cloner of OpenScrape
// may be used for players, pots, community-cards, etc.
// So we take the maximum.
const int k_max_number_of_regions_to_clone = 10;

// Memory symbols
const int k_max_number_of_memory_symbols = 2048;
const int k_max_length_of_memory_symbols = 64;

// Table stats
const int k_time_frame_for_table_stats_in_seconds = 15 * 60;

// debug tab
const int k_precision_for_debug_tab      = 3;
const int k_integer_places_for_debug_tab = 6;

// handnumber
const int k_max_length_of_handnumber = 256;

// Hand history generator
const double k_hand_history_rake = 0.05;

//  File accessable?
//  (<unistd.h> is not contained in MSCVPP)
const int F_OK = 0;

// thread timeout
#define THREAD_WAIT					3000
const int k_max_time_to_wait_for_thread_to_shutdown = 60000; // milli-seconds
// "This must not happen."
// It is better to have a named constant then to write "assert(false);".
const bool kThisMustNotHappen = false;

const int k_mutex_lock_time = 500;

enum StandardFunctionConstants {
	// Primary autoplayer-functions
	k_autoplayer_function_beep,
	k_autoplayer_function_allin,
	k_autoplayer_function_betpot_2_1,
	k_autoplayer_function_betpot_1_1,
	k_autoplayer_function_betpot_3_4,
	k_autoplayer_function_betpot_2_3,
	k_autoplayer_function_betpot_1_2,
	k_autoplayer_function_betpot_1_3,
	k_autoplayer_function_betpot_1_4,
	k_autoplayer_function_betsize,
	k_autoplayer_function_raise,
	k_autoplayer_function_call,
	k_autoplayer_function_check,
	k_autoplayer_function_fold,
	// hopper-functions
	k_hopper_function_sitin,
	k_hopper_function_sitout,
	k_hopper_function_leave,
  k_hopper_function_rematch,
	k_hopper_function_autopost,
	k_hopper_function_close,
	k_hopper_function_rebuy,
  k_hopper_function_select_formula_file,
	// Other functions
  k_standard_function_prefold,
  k_standard_function_chat,
	k_standard_function_delay,
  k_standard_function_allin_on_betsize_balance_ratio,
  k_standard_function_shoot_replay_frame,
	// Ini-functions	
	k_init_on_startup,
	k_init_on_connection,
	k_init_on_handreset,
	k_init_on_new_round,
	k_init_on_my_turn,
	k_init_on_heartbeat,
	// Prwin functions
	k_prwin_number_of_opponents,
	k_prwin_number_of_iterations,
	k_prwin_topclip,
	k_prwin_mustplay,
	k_prwin_willplay,
	k_prwin_wontplay,
  // ICM functions
  k_icm_prize1,
  k_icm_prize2,
  k_icm_prize3,
  k_icm_prize4,
  k_icm_prize5,
  k_icm_prize6,
  k_icm_prize7,
  k_icm_prize8,
  k_icm_prize9,
	// Attention! Always keep this at last place.
	k_number_of_standard_functions
};

const int k_max_length_betpot_button_name = 17;

static const char k_betpot_button_name[k_max_betpot_buttons][k_max_length_betpot_button_name] = {
	"betpot_2_1",
	"betpot_1_1",
	"betpot_3_4",
	"betpot_2_3",
	"betpot_1_2",
	"betpot_1_3",
	"betpot_1_4"
};

//  Standard function names
const int k_max_length_of_standard_function_names = 32 + 1;
static const char k_standard_function_names[k_number_of_standard_functions][k_max_length_of_standard_function_names] = {
	// Primary autoplayer-functions
	"f$beep",
	"f$allin",
	"f$betpot_2_1",
	"f$betpot_1_1",
	"f$betpot_3_4",
	"f$betpot_2_3",
	"f$betpot_1_2",
	"f$betpot_1_3",
	"f$betpot_1_4",
	"f$betsize",
	"f$raise",
	"f$call",
	"f$check",
	"f$fold",
	// Secondary autoplayer-functions
	"f$sitin",
	"f$sitout",
	"f$leave",
  "f$rematch",
	"f$autopost",
	"f$close",
	"f$rebuy",
  "f$select_formula_file",
	// Other functions
  "f$prefold",
  "f$chat",
	"f$delay",
  "f$allin_on_betsize_balance_ratio",
  "f$shoot_replay_frame",
	// Ini-functions	
	"f$ini_function_on_startup",
	"f$ini_function_on_connection",
	"f$ini_function_on_handreset",
	"f$ini_function_on_new_round",
	"f$ini_function_on_my_turn",
	"f$ini_function_on_heartbeat",
	// Prwin functions
	"f$prwin_number_of_opponents",
	"f$prwin_number_of_iterations",
	"f$prwin_topclip",
	"f$prwin_mustplay",
	"f$prwin_willplay",
	"f$prwin_wontplay",
  // ICM functions
  "f$icm_prize1",
  "f$icm_prize2",
  "f$icm_prize3",
  "f$icm_prize4",
  "f$icm_prize5",
  "f$icm_prize6",
  "f$icm_prize7",
  "f$icm_prize8",
  "f$icm_prize9",
};

const int k_max_length_of_OpenPPL_function_names = 9 + 1;
static const char k_OpenPPL_function_names[kNumberOfBetrounds + 1][k_max_length_of_OpenPPL_function_names] = {
  "",       // 0 unused
  "f$preflop",
  "f$flop",
  "f$turn",
  "f$river"
};

const int k_max_length_of_Shanky_style_betrounds = 7 + 1;
static const char k_Shanky_style_betrounds[kNumberOfBetrounds + 1][k_max_length_of_Shanky_style_betrounds] = {
  "",       // 0 unused
  "preflop",
  "flop",
  "turn",
  "river"
};

// Special symbol for empty expressions. Its evaluation adds something 
// meaningful to the log when the end of an open-ended when-condition 
// gets reached during evaluation.
// Needs to be lower-cases because it is built-in.
static const char* kEmptyExpression_False_Zero_WhenOthersFoldForce =
  "empty_expression__false__zero__when_others_fold_force";
  
enum table_positioner_options {
	k_position_tables_never,
	k_position_tables_tiled,
	k_position_tables_cascaded
};

enum shoot_replay_frames_options {
  // Default: never shoot replay-frames
  // Keeping the old (formerly hard-coded) numerical values 
  // to stay compatile to old config-files.
  kShootReplyFramesNever = 0,
  kShootReplyFramesOnMyTurn = 3,
  kShootReplyFramesOnEveryChangeWhilePlaying = 4,
  kShootReplyFramesOnEveryChange = 5,
};

// poker tracker
const double k_pokertracker_stat_undefined = -1.0;

// numeric constants
#define M_E			2.7182818284590452354
#define M_LOG2E		1.4426950408889634074
#define M_LOG10E	0.43429448190325182765
#define M_LN2		0.69314718055994530942
#define M_LN10		2.30258509299404568402
#define M_PI		3.14159265358979323846
#define M_SQRT2		1.41421356237309504880

// Error Codes for taqblemaps
// Former parser codes have been removed
// as the parser got completely reimplemented
#define SUCCESS						    0
#define ERR_EOF						    1
#define ERR_SYNTAX					  2
#define ERR_VERSION					  3
#define ERR_NOTMASTER				  4
#define ERR_HASH_COLL				  5
#define ERR_REGION_SIZE				6
#define ERR_UNK_LN_TYPE				7
#define ERR_INV_HASH_TYPE			8
#define ERR_INCOMPLETEMASTER  9

const char* const k_tablemap_errors_and_parse_errors_explained[10] = {
  "success", 
  "unexpected end of file", 
  "invalid syntax", 
  "incorrect version",
	"not a master file", 
  "hash collision", 
  "invalid region size", 
  "unknown line type",
	"invalid hash type", 
  "incomplete master file"};

// Scraper error codes
#define ERR_FIELD_TOO_LARGE			   -1
#define ERR_NOTHING_TO_SCRAPE		   -2
#define ERR_NO_HASH_MATCH			     -3
#define ERR_TEXT_SCRAPE_NOMATCH		 -4
#define ERR_INVALID_TRANSFORM_TYPE -5
#define ERR_NO_IMAGE_MATCH			   -6
#define	ERR_GOOD_SCRAPE_GENERAL		  1

// Profile version codes
#define VER_OPENHOLDEM_1		".ohdb1"  // no longer valid, due to changing of hash keys for types 1-3
#define VER_OPENHOLDEM_2		".ohdb2"  // legacy version, still supported, although openscrape table maps are preferred
#define VER_OPENSCRAPE_1		".osdb1"
#define VER_OPENSCRAPE_2		".osdb2"

// Preferences
#define MAX_OPPONENTS		    22	
#define MAX_FRAMEDELAY		   9	
#define MAX_SWAGDELAY1		5000	
#define MAX_SWAGDELAY2		5000	
#define MAX_SWAGDELAY3		5000	
#define MAX_ALLINDELAY		5000	
#define MAX_SCRAPEDELAY		5000
#define MIN_SCRAPEDELAY		 100

// Chipstacks
#define MAX_CHIPS_PER_STACK	20
#define MAX_CHIP_STACKS		10

// Tablemap Strings
// Strings with the same textual value have to evaluate 
// to the same numerical value, as they get later processed
// by a single function GetTMSymbol(name, default)
// Examples: TEXTDEL_NOTHING = TEXTSEL_NOTHING = BETCONF_NOTHING = 5

// SWAG options - text selectioin
const int TEXTSEL_SINGLECLICK =	1;
const int TEXTSEL_DOUBLECLICK =	2;
const int TEXTSEL_TRIPLECLICK =	3;  
const int TEXTSEL_CLICKDRAG	  =	4;
const int TEXTSEL_NOTHING	  =	5;

// SWAG options - text deletion
const int TEXTDEL_DELETE	 = 1;
const int TEXTDEL_BACKSPACE	 = 2;
const int TEXTDEL_NOTHING	 = TEXTSEL_NOTHING;

// SWAG options - bet confirmation
const int BETCONF_ENTER		 = 1;
const int BETCONF_CLICKBET = 2;
const int BETCONF_NOTHING	 = TEXTSEL_NOTHING;

// Button click options
const int BUTTON_SINGLECLICK = TEXTSEL_SINGLECLICK;
const int BUTTON_DOUBLECLICK = TEXTSEL_DOUBLECLICK;
const int BUTTON_NOTHING     = TEXTSEL_NOTHING;

// Autoplayer prevaction
#define PREVACT_FOLD		-1
#define PREVACT_CHEC		0
#define PREVACT_CALL		1
#define PREVACT_RAIS		2
#define PREVACT_SWAG		3
#define PREVACT_ALLI		4

// For drawing
// Card sizes
#define CARDSIZEX		26
#define	CARDSIZEY		52
// Colors ("the 16 VGA colours")
// https://en.wikipedia.org/wiki/List_of_software_palettes#Microsoft_Windows_default_16-color_palette
// https://en.wikipedia.org/wiki/Web_colors
#define	COLOR_WHITE		RGB(0xFF, 0xFF, 0xFF)
#define COLOR_SILVER	RGB(0xC0, 0xC0, 0xC0)
#define COLOR_GRAY		RGB(0x80, 0x80, 0x80)
#define	COLOR_BLACK		RGB(0x00, 0x00, 0x00)
#define	COLOR_RED		  RGB(0xFF, 0x00, 0x00)
#define	COLOR_MAROON  RGB(0x80, 0x00, 0x00)
#define COLOR_YELLOW	RGB(0xFF, 0xFF, 0x00)
#define	COLOR_OLIVE   RGB(0x80, 0x80, 0x00)
#define	COLOR_LIME    RGB(0x00, 0xFF, 0x00)
#define	COLOR_GREEN		RGB(0x00, 0x80, 0x00)
#define	COLOR_AQUA    RGB(0x00, 0xFF, 0xFF)
#define	COLOR_TEAL    RGB(0x00, 0x80, 0x80)
#define	COLOR_BLUE		RGB(0x00, 0x00, 0xFF)
#define	COLOR_NAVY		RGB(0x00, 0x00, 0x80)
#define	COLOR_FUCHSIA RGB(0xFF, 0x00, 0xFF)
#define	COLOR_PURPLE  RGB(0x80, 0x00, 0x80)

// Bitmap display
#define BITMAP_WIDTH		344
#define BITMAP_HEIGHT		164

// For scraper engine
#define NOTHING_CHANGED		0
#define	COM_CARD_CHANGED	(1<<1)
#define PL_CARD_CHANGED		(1<<2)
#define	SEATED_CHANGED		(1<<3)
#define	ACTIVE_CHANGED		(1<<4)
#define	DEALER_CHANGED		(1<<5)
#define	NAME_CHANGED		(1<<6)
#define	BALANCE_CHANGED		(1<<7)
#define	PLAYERBET_CHANGED	(1<<8)
#define	BUTTONSTATE_CHANGED	(1<<9)
#define BUTTONLABEL_CHANGED	(1<<10)
#define	POT_CHANGED			(1<<11)
#define	LIMITS_CHANGED		(1<<12)

// Tablemap/scraper limits
#define MAX_CHAR_WIDTH		200
#define	MAX_CHAR_HEIGHT		150
#define MAX_HASH_WIDTH		200
#define MAX_HASH_HEIGHT		150
#define MAX_IMAGE_WIDTH		200
#define MAX_IMAGE_HEIGHT	150
#define MAX_SINGLE_CHAR_WIDTH	31
#define MAX_SINGLE_CHAR_HEIGHT	32

#define MAX_WINDOW_TITLE	512

// Old WinHoldem prevaction-constants
// http://www.maxinmontreal.com/wiki/index.php5?title=OpenHoldem:EndUserDocumentation:Symbols#History
// (-1=fold 0=chec 1=call 2=rais 3=swag 4=alli)
enum ActionConstant {
	k_prevaction_undefined = -2,
	k_prevaction_fold      = -1,
	k_prevaction_check     = 0,	// new in OpenHoldem
	k_prevaction_call      = 1,
	k_prevaction_raise     = 2, // min-raise
	k_prevaction_betsize   = 3,	// "stated wager", i.e. using f$betsize
	k_prevaction_allin     = 4,
	k_prevaction_jam       = 5  // not really an action, but required for correctly logging the slider.
};

// for rank to card translation
// Suits
const static char *k_card_chars = "23456789TJQKA";
const int k_number_of_different_cardranks = 13;

// Index for current betting round in arrays
// we prefer to store values for preflop..river in 1..4
// and use index 0 for the current round.
// The old code did it differently (4 for the current round)
const int k_storage_index_for_current_round = 0;

// PrWin
const int k_number_of_pocketcard_combinations_without_order = 1326;

// For assertions of codes, that should never be executed
const bool k_ThisMustNotHappen = false;

// For logging
const bool kAlwaysLogAutoplayerFunctions = true;
const bool k_always_log_basic_information = true;
const bool kAlwaysLogDLLMessages = true;
const bool k_always_log_errors = true;
const bool kAlwaysLogOpenPPLInitialization = true;
const int  kMaxLogSymbolsForWhiteBox = 1;

// For string handling
const int kOneCharacterExtraForTerminatingNull = 1;

// Function to access the name of the action constants.
// As fold is negative we can no longer use the constants 
// as indices for an array.
static const char* ActionConstantNames(int autoplayer_function_code) {
  // names of action-constants for use in the autoplayer-log.
  // Formerly 4 digits (WinHoldem-style), now more sane.
  // Also considering all secondary formulas.
  if ((autoplayer_function_code >= k_autoplayer_function_beep)
    && (autoplayer_function_code <= k_standard_function_chat)) {
    return k_standard_function_names[autoplayer_function_code];
  }
  return "UNDEFINED";
}