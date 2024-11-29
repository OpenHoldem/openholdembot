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

#ifndef INC_STRUCTS_DEFINES_H
#define INC_STRUCTS_DEFINES_H

#include "..\pokereval\include\poker_defs.h"
#include "..\Shared\MagicNumbers\MagicNumbers.h"

//Allow keywords as macros.  Needed for vs2013
#define _ALLOW_KEYWORD_MACROS

// thread timeout
#define THREAD_WAIT					3000

// numeric constants
#define M_E			 2.7182818284590452354
#define M_LOG2E	 1.4426950408889634074
#define M_LOG10E 0.43429448190325182765
#define M_LN2		 0.69314718055994530942
#define M_LN10	 2.30258509299404568402
#define M_PI		 3.14159265358979323846
#define M_SQRT2	 1.41421356237309504880

// Preferences
#define MAX_OPPONENTS		22	
#define MAX_FRAMEDELAY		9
#define MAX_CLICKDELAY		5000
#define MAX_SWAGDELAY1		5000	
#define MAX_SWAGDELAY2		5000	
#define MAX_SWAGDELAY3		5000	
#define MAX_ALLINDELAY		5000	
#define MAX_SCRAPEDELAY		5000
#define MIN_SCRAPEDELAY		100
#define MAX_TOKENNUMBER		15
#define MAX_TOKENSIZE		25

// Chipstacks
#define MAX_CHIPS_PER_STACK	20
#define MAX_CHIP_STACKS		10

// Button click options
#define BETPOT_DEFAULT	0
#define BETPOT_RAISE	1

// For drawing
// Card sizes
#define CARDSIZEX		26
#define	CARDSIZEY		52

// Bitmap display
#define BITMAP_WIDTH		344
#define BITMAP_HEIGHT		164

// For scraper engine
#define NOTHING_CHANGED		  0
#define	COM_CARD_CHANGED	  (1<<1)
#define PL_CARD_CHANGED		  (1<<2)
#define	SEATED_CHANGED		  (1<<3)
#define	ACTIVE_CHANGED		  (1<<4)
#define	DEALER_CHANGED		  (1<<5)
#define	NAME_CHANGED		    (1<<6)
#define	BALANCE_CHANGED		  (1<<7)
#define	PLAYERBET_CHANGED	  (1<<8)
#define	BUTTONSTATE_CHANGED	(1<<9)
#define BUTTONLABEL_CHANGED	(1<<10)
#define	POT_CHANGED			    (1<<11)
#define	LIMITS_CHANGED		  (1<<12)
#define	HANDNUMBER_CHANGED  (1<<13)
#define	SOMETHING_CHANGED   0x1FFF  

// Tablemap/scraper limits
#define MAX_CHAR_WIDTH		200
#define	MAX_CHAR_HEIGHT		150
#define MAX_HASH_WIDTH		200
#define MAX_HASH_HEIGHT		150
#define MAX_IMAGE_WIDTH		200
#define MAX_IMAGE_HEIGHT	150
#define MAX_SINGLE_CHAR_WIDTH	31
#define MAX_SINGLE_CHAR_HEIGHT	32

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

enum FileType { FTnone=1, FTlist, FTfunc };

struct STableList {
	HWND		hwnd;
  int     tablemap_index;
#ifndef   OPENHOLDEM_PROGRAM
  RECT    attached_rect;
  RECT    crect;
  CString title;
#endif
};

//prwin 1326 chair structure    Matrix 2008-04-29
struct sprw1326_chair {
	int			level;			// indicates significance level for 'always consider'
	int			limit;			// max index into significance array - used for computational efficiency
	int			ignore;			// if non-zero no weighting will be applied to this chair
	int			rankhi[k_number_of_pocketcard_combinations_without_order];	// hi card number in pocket cards
	int			ranklo[k_number_of_pocketcard_combinations_without_order];	// lo card number in pocket cards
	int			weight[k_number_of_pocketcard_combinations_without_order];	// the significance value for this hand
	double		scratch;		// for future reference
};

//prwin 1326 structure			Matrix 2008-04-29
struct sprw1326
{
	int			useme;				// unless set to 1326 the normal OH prwin will be used
	int			preflop;			// unless set to 1326 the normal OH prwin will be used pre-flop
	int			usecallback;		// unless set to 1326 the callback function will not be invoked
	double	(*prw_callback)(); // if enabled will be invoked before the prwin calculation pass
	double	scratch;			// for future reference
	sprw1326_chair	vanilla_chair;	// will be precalculated by OH at startup - convenience values
	sprw1326_chair  chair[kMaxNumberOfPlayers];  // structures for each chair
};

#endif /* INC_STRUCTS_DEFINES_H */
