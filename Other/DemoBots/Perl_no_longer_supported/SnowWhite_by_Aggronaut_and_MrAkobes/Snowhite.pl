###################################################################
# CONTRIBUTED TO MAXINMONTREAL ON XXXX
#  EmptyPerlBot.Pl
#  ======================
#
#  Author: aggronaut, mrakobes
#
#
#  Last change: 2011.09.16
#
#
###################################################################
#
#  USAGE:
#  ======
#
#    You need ActiveState Perl properly installed.
#      OH must be able to find "PerlEz.dll"
#      You maybe must change the PATH search path from the DOS prompt:
#      > set PATH = C:\Programs\Perl\Bin;%PATH%
#    The package Win32::API must be installed. 
#      Use Perls package ppm.
#    For details please have a look at the docu
#      in the OpenHoldem Wiki.
#      
#    To request an OH symbol or DLL symbol:
#      use the function "gws (<SYMBOL>)"  
#
#    To request a Perl symbol
#      a) inside the DLL: use "gws (<SYMBOL>)"
#      b) inside a formula: Just access the symbol :)
#   
#   Perl symbols have to begin with "pl_", NOT "pl$",
#     as the Dollar character is reserved in Perl.
#
#   Perl symbols shall be functions without arguments;
#     if you need constant symbols, just use constant functions.
#     (A function call is cheaper than the evaluation of a string,
#     which involves a compilation step.)  
#
#   You nevertheless need a classic OpenHoldem formula,
#     but you may redirect any symbol to a perl function;
#     eg:
#             ##f$call##
#             pl_call
#
##################################################################
#
#  It's better to use a strict and conservative coding style.
#    As we have limited debugging support running our bot,
#    I recommend to test as much as possible offline
#    and do at least a syntax check (from the command line)
#    > perl -w -c EmptyPerlBot.pl
#

use strict;
use warnings;
use diagnostics;

##################################################################
#
#  Including Perl modules
#

#  Set the search path for our Perl modules.
#    This path has to be absolute (as a security measure)
#    (Note: '\' is an escape character. We have to write
#    two to get a single one.)
#
use lib ("C:\\Documents and Settings\\John\\Desktop\\Pokerbot\\");


#  The module "Perl_OH_Interaction.pm" contains the technical stuff
#      to get OpenHoldem running with Perl.
#
use Debug;
use Perl_OH_Interaction;
# include this library for the min function and others
use List::Util qw(first max maxstr min minstr reduce shuffle sum);
use POSIX qw(ceil);

use constant FALSE => 0;
use constant TRUE  => 1;


##################################################################
#
#  Some constants and variables.
#
use constant PREFLOP 	=> 1;
use constant FLOP 		=> 2;
use constant TURN 		=> 3;
use constant RIVER 		=> 4;

# pokertracker stats
use constant PTT_HANDS 	=> 1;
use constant PTT_PFR 	=> 2;
use constant PTT_FLOPPCT	=> 3;

# our table image
use constant LOOSE => 0;
use constant TIGHT => 1;

##############
# 
# globals
# generally only needed to communicate/share some data among functions during the same scrape

# max players at the table
use constant MAX_PLAYERS => 9; # This may not be changed!

#  A hash to cache OpenHoldem symbols
#
#  You can later access the symbol values
#    by their name, e.g.:  $OHS{'dealerchair'}
#    
my %OHS;


#  Symbols we want to cache
#    (Note: "qw" avoids putting each symbol in quotation marks.)
#
my @OH_Symbolnames = qw( 
ac_first_into_pot
ac_postflop_pos
ac_preflop_pos
balance
balance0
balance1
balance2
balance3
balance4
balance5
balance6
balance7
balance8
balance9
bblind
bet
br
call
chairbit$Anybody
chair
$$cc0
$$cc1
$$cc2
$$cc3
$$cc4
$$cr0
$$cr1
$$cr2
$$cr3
$$cr4
$$cs0
$$cs1
$$cs2
$$cs3
$$cs4
currentbet	
currentbet0	
currentbet1	
currentbet2	
currentbet3	
currentbet4	
currentbet5
currentbet6	
currentbet7	
currentbet8	
currentbet9	
dealpositionrais
dealposition
didcall
didcallround1
didcallround2
didcallround3
didcallround4
didchec
didchecround1
didchecround2
didchecround3
didchecround4
didswag
didswaground1
didswaground2
didswaground3
didswaground4
handnumber
isflush
isfourofakind
isfullhouse
ishandup
ishiflush
ishipair
ishistraight
islist100
islist101
islist102
islist103
islist104
islist105
islist106
islist107
islist108
islist109
islist110
islist111
islopair
ismidpair
isonepair
ispair
isroyalflush
isstraight
isstraightflush
isthreeofakind
istwopair
mh_kickerbetter
mh_str_fullhouse
mh_str_straight
mh_str_flush
mh_str_onepair
mh_str_trips
mh_str_twopair
mh_nsuitedbetter
nfriendsplaying
nhands
nhandslo
nhandshi
nhandsti
nopponentsplaying
nopponentsraising
npcbits
nplayersactive
nplayersdealt
nrankedcommon
nstraight
nstraightcommon
nstraightfill
nstraightfillcommon
nsuited
nsuitedcommon
$$pc0
$$pc1
$$ps0
$$ps1
pcbits
playersplayingbits
playersplayingbits
opponentsplayingbits
pot
potcommon
ptt_rpfr
ptt_rhands
raischair
random
randomround
rankhicommon
rankhiplayer
rankloplayer
sblind
stack0
stack1
stack2
stack3
stack4
stack5
stack6
stack7
stack8
stack9
trankcommon
tsuit
tsuitcommon
);

##################################################################
#
#  Some functions
#

#  Calling OpenHoldem from Perl is not implemented
#    in an extremely efficient way.
#  Therefore it is recommended to cache the symbols     
#    you need on Perl side to request them only
#    once per cycle.
#  Updating the symbol cache should be the first action
#    in a cycle. You may put this operation e.g. 
#    into the "allin" function.
#
sub cache_Symbols
{
    #  Get your symbol values
    #send_DebugMsg('cache_Symbols');
    foreach my $Symbol (@OH_Symbolnames)
    {
	$OHS{$Symbol} = gws($Symbol); 
	# send_DebugMsg($Symbol, '=>', $OHS{$Symbol});
    }
}


sub pl_play
{
    #  Return values
    #    -2:  Leave the table
    #    -1:  Do nothing
    #     0:  Sit out
    #     1:  Play (and post blinds)
    # 
    #  Just to get it running....
    return 1;
}



#
#  AUTOLOAD handles unknown functions. 
#
#    As this should happen only for requests of unknown symbols,
#    we don`t check the expected return type and return 0.0,
#    as this should be fine for OpenHoldem.
#

sub AUTOLOAD
{
    #  Put some error handling here, if you need.
    send_DebugMsg('AUTOLOAD');
    return 0;	
}
# Returns card values in "CCs" format, useful for debugging

# FUNCTION: card_val
# receives: a card rank (a 32-bit number that represents a card, page 82 of manual 2.0)
# each card val is stored in 8 bits: 
#	bits 7-4 store the rank 1-14 (A is both 1 and 14).
#	bits 3-0 store the suit: 4 spade, 3 heart, 2 diamond, 1 club
# returns: a human readable string for the card value, e.g. Jc, As, etc.
# on error it returns X for the card rank and x for the card suit
sub card_val
{
        my $bits = $_[0];
        my $card_rank = $bits >> 4;
        my $card_suit = ( $bits << 28) >> 28;
        my $ret_str = "$card_rank";
        if ( $card_rank == 14 || $card_rank == 1) {
                $ret_str = "A";
        }
        elsif ( $card_rank == 13) {
                $ret_str = "K";
        }
        elsif ( $card_rank == 12) {
                $ret_str = "Q";
        }
        elsif ( $card_rank == 11) {
                $ret_str = "J";
        }
        elsif ( $card_rank == 10) {
                $ret_str = "T";
        }
        # invalid card rank error check
        if ( $card_rank < 2 || $card_rank > 14) {
                $ret_str = "X";
        }

        if ( $card_suit == 4) {
                $ret_str .= "s";
        }
        elsif ( $card_suit == 3) {
                $ret_str .= "h";
        }
        elsif ( $card_suit == 2) {
                $ret_str .= "d";
        }
        elsif ( $card_suit == 1) {
                $ret_str .= "c";
        }
        else {
                $ret_str .= "x";
        }

        return $ret_str;
}


# these $hand_number_saved and @saved_stacks are needed to obtain the initial player stacks
# the saved_stacks array persists through the duration of the hand
my $hand_number_saved = 0;
my @saved_stacks = ();

# FUNCTION: calculate_preflop_stacks - populates global saved_stacks array
# receives: nothing
# side effects: array of preflop stacks of players that are seated at the table is repopulated on each hand
# each player's stack is tied to the player's chair number, which is constant throughout the hand
# stack size of 0 indicates a player sitting out/not present - the one we don't care about
sub calculate_preflop_stacks
{	
	# just in case we get some crazy number read
	if ( $hand_number_saved < 0)
	{
		send_DebugMsg("read the hand number saved as $hand_number_saved");
		$hand_number_saved = 0; 
	}
	
	if ( $OHS{'br'} == PREFLOP && $OHS{'handnumber'} > $hand_number_saved)
	{
		$hand_number_saved = $OHS{'handnumber'};
		@saved_stacks = ();
						
		# Player stack consists of his balance and currentbet
		# the specific chair balance is index into the saved_stacks array
		$saved_stacks[0] = $OHS{'balance0'} + $OHS{'currentbet0'};
		$saved_stacks[1] = $OHS{'balance1'} + $OHS{'currentbet1'};
		$saved_stacks[2] = $OHS{'balance2'} + $OHS{'currentbet2'};
		$saved_stacks[3] = $OHS{'balance3'} + $OHS{'currentbet3'};	
		$saved_stacks[4] = $OHS{'balance4'} + $OHS{'currentbet4'};
		$saved_stacks[5] = $OHS{'balance5'} + $OHS{'currentbet5'};
		$saved_stacks[6] = $OHS{'balance6'} + $OHS{'currentbet6'};
		$saved_stacks[7] = $OHS{'balance7'} + $OHS{'currentbet7'};
		$saved_stacks[8] = $OHS{'balance8'} + $OHS{'currentbet8'}; 
	}
	send_DebugMsg("saved_stacks = @saved_stacks");
}

# FUNCTION: get_mychair
# returns hero's chair number adjusted for the poker client
sub get_mychair
{
	return $OHS{'chair'};
}

# FUNCTION: our chips invested
# calculates the amount chips we have bet during the hand
sub our_chips_invested
{
	my $chips_spent = 0;
	
	# we don't want to divide by zero
	if ( $saved_stacks[get_mychair()] == 0)
	{
		return -1; # need to return an error here
	}
	
	$chips_spent =  ( $saved_stacks[get_mychair()] - $OHS{'balance'}) / $saved_stacks[get_mychair()];
	send_DebugMsg("Our chips invested: $chips_spent == ");
	
	return $chips_spent;	
}

# FUNCTION: risk of ruin (if we are up against a higher stack and bust out if we lose)
sub riskofruin
{
	my $balance = $OHS{'balance'};
	if ( $balance < max(saved_stacks())) {
		send_DebugMsg("risk_of_ruin is true", "Max saved stacks =", max(saved_stacks()));
		return TRUE;
	}
}

# FUNCTION: 
sub active_positions
{	
	my $playersplayingbits = $OHS{'playersplayingbits'};
	my @active_positions = ();
	
	for ( my $i=0;$i<MAX_PLAYERS;$i++) # MAX_PLAYERS here
	{
		if ( $playersplayingbits & (1 << $i)) 
		{
			push(@active_positions, $i);
		}
	}
	
	return @active_positions;
}

# FUNCTION: my_playersplayingbits
sub my_playersplayingbits
{
	my $x = $OHS{'playersplayingbits'};
	
	return $x;
}

# FUNCTION: active_stats
# receives: type of pokertracker stat to return:
#		- ptt_hands;
#		- ptt_pfr;
#		- ptt_floppct.
# returns: array of stats
# push in reverse order so that the first one is the lowest index player in hand
# Example: active_stats(ptt_PFR) will return array of active stats of preflop raise
sub active_stats
{
	my @current_stats_array = ();
	my $playersplayingbits = my_playersplayingbits();
	
	if ( $_[0] == PTT_HANDS) {
		#Poker Tracker number of hands that are in the database for chair x
		if ( $playersplayingbits & (1 << 8)) {
			push( @current_stats_array, $OHS{'ptt_hands8'});
		}
		if ( $playersplayingbits & (1 << 7)) {
			push( @current_stats_array, $OHS{'ptt_hands7'});
		}		
		if ( $playersplayingbits & (1 << 6)) {
			push( @current_stats_array, $OHS{'ptt_hands6'});
		}
		if ( $playersplayingbits & (1 << 5)) {
			push( @current_stats_array, $OHS{'ptt_hands5'});
		}
		if ( $playersplayingbits & (1 << 4)) {
			push( @current_stats_array, $OHS{'ptt_hands4'});
		}
		if ( $playersplayingbits & (1 << 3)) {
			push( @current_stats_array, $OHS{'ptt_hands3'});
		}
		if ( $playersplayingbits & (1 << 2)) {
			push( @current_stats_array, $OHS{'ptt_hands2'});
		}
		if ( $playersplayingbits & (1 << 1)) {
			push( @current_stats_array, $OHS{'ptt_hands1'});
		}
		if ( $playersplayingbits & (1 << 0)) {
			push( @current_stats_array, $OHS{'ptt_hands0'});
		}
	}
	elsif ( $_[0] == PTT_PFR) {
		# Poker Tracker pre-flop raise percentage for chair x
		if ( $playersplayingbits & (1 << 8)) {
			push( @current_stats_array, $OHS{'ptt_pfr8'});
		}
		if ( $playersplayingbits & (1 << 7)) {
			push( @current_stats_array, $OHS{'ptt_pfr7'});
		}		
		if ( $playersplayingbits & (1 << 6)) {
			push( @current_stats_array, $OHS{'ptt_pfr6'});
		}
		if ( $playersplayingbits & (1 << 5)) {
			push( @current_stats_array, $OHS{'ptt_pfr5'});
		}
		if ( $playersplayingbits & (1 << 4)) {
			push( @current_stats_array, $OHS{'ptt_pfr4'});
		}
		if ( $playersplayingbits & (1 << 3)) {
			push( @current_stats_array, $OHS{'ptt_pfr3'});
		}
		if ( $playersplayingbits & (1 << 2)) {
			push( @current_stats_array, $OHS{'ptt_pfr2'});
		}
		if ( $playersplayingbits & (1 << 1)) {
			push( @current_stats_array, $OHS{'ptt_pfr1'});
		}
		if ( $playersplayingbits & (1 << 0)) {
			push( @current_stats_array, $OHS{'ptt_pfr0'});
		}
	}
	elsif ( $_[0] == PTT_FLOPPCT) {
		# Poker Tracker saw flop pct for chair x
		if ( $playersplayingbits & (1 << 8)) {
			push( @current_stats_array, $OHS{'ptt_floppct8'});
		}
		if ( $playersplayingbits & (1 << 7)) {
			push( @current_stats_array, $OHS{'ptt_floppct7'});
		}		
		if ( $playersplayingbits & (1 << 6)) {
			push( @current_stats_array, $OHS{'ptt_floppct6'});
		}
		if ( $playersplayingbits & (1 << 5)) {
			push( @current_stats_array, $OHS{'ptt_floppct5'});
		}
		if ( $playersplayingbits & (1 << 4)) {
			push( @current_stats_array, $OHS{'ptt_floppct4'});
		}
		if ( $playersplayingbits & (1 << 3)) {
			push( @current_stats_array, $OHS{'ptt_floppct3'});
		}
		if ( $playersplayingbits & (1 << 2)) {
			push( @current_stats_array, $OHS{'ptt_floppct2'});
		}
		if ( $playersplayingbits & (1 << 1)) {
			push( @current_stats_array, $OHS{'ptt_floppct1'});
		}
		if ( $playersplayingbits & (1 << 0)) {
			push( @current_stats_array, $OHS{'ptt_floppct0'});
		}
	}

	return @current_stats_array;
}
	
# FUNCTION: isflopdry
# for dry flop
# returns TRUE if flop is unsuited, unpaired or has a pair less than TT, no more than 2 connected
sub isflopdry
{
	#send_DebugMsg("-=== **** nsuitedcommon = 		", $OHS{'nsuitedcommon'},	"	should be == 1\n");
	#send_DebugMsg("-=== **** rankhicommon =			", $OHS{'rankhicommon'},		"	should be < 10\n");
	#send_DebugMsg("-=== **** nrankedcommon =		", $OHS{'nrankedcommon'},	"	should be < 3\n");
	#send_DebugMsg("-=== **** nstraightcommon =		", $OHS{'nstraightcommon'},	"	should be < 3\n");
	
	if ( $OHS{'nsuitedcommon'} < 3 && $OHS{'rankhicommon'} < 10 
			&& $OHS{'nrankedcommon'} < 3 && $OHS{'nstraightcommon'} < 3)
	{
		send_DebugMsg('DRY flop');
		return TRUE;
	}
	
	return FALSE;
}



# FUNCTION: monster_draw
# returns TRUE when flush draw + straight draw on unpaired board, flush draw + 1 pair on unpaired board, straight draw + 1 pair on unpaired board
sub monster_draw
{
	if 	( 
		( flush_draw() == TRUE && paired_board() == FALSE) && 
		( $OHS{'isonepair'} || $OHS{'nstraight'} == 4) 
		)
	{
		send_DebugMsg('-=0= MONSTER draw as flush draw + 1 pair or flush draw + straight draw =0=-');
		return TRUE;
	}
	
	if (
		 $OHS{'isonepair'} && $OHS{'nsuitedcommon'} < 3 && paired_board() == FALSE && 
		($OHS{'nstraight'} == 4 || $OHS{'nstraightfill'} == 1)
		)
		
	{
		send_DebugMsg('-=0= MONSTER draw 1 pair + straight draw =0=-');
		return TRUE;
	}
		
	
	return FALSE;
}

# FUNCTION: bluff_flush
# returns TRUE when board is unpaired and became 3-flush on the river
sub bluff_flush
{
	if ( $OHS{'nrankedcommon'} == 1 && $OHS{'nsuitedcommon'} == 3 && $OHS{'tsuitcommon'} == $OHS{'$$cs4'})
	{
		send_DebugMsg ("Bluff flush is TRUE, nrankedcommon is ", $OHS{'nrankedcommon'}, "nsuitedcommon is ",
		$OHS{'nsuitedcommon'}, "tsuitcommon is ", $OHS{'tsuitcommon'}, "River card suit is ", $OHS{'$$cs4'});
		return TRUE;		
	}	
	send_DebugMsg ("Bluff flush is FALSE, nrankedcommon is ", $OHS{'nrankedcommon'}, "nsuitedcommon is ",
		$OHS{'nsuitedcommon'}, "tsuitcommon is ", $OHS{'tsuitcommon'}, "River card suit is ", $OHS{'$$cs4'});		
	return FALSE;	
}

# FUNCTION: am_i_dealer
# returns TRUE if we are on the button
sub am_i_dealer
{
	if ( $OHS{'ac_preflop_pos'} == 6 || $OHS{'ac_postflop_pos'}  == 5)
	{
		#send_DebugMsg ("I am DEALER!");
		return TRUE;
	}	
	return FALSE;
}

# FUNCTION: headsup
# returns true when we are headsup
sub are_we_headsup
{
	if ( $OHS{'nplayersdealt'} == 2) {
		#send_DebugMsg("We are heads up!");
		return TRUE;
	}	
	return FALSE;
}

# FUNCTION: adjusted_dealposition
sub adjusted_dealposition
{
	my $adp = -1;
	
	# headsup is a special case, the dealer is sb, and therefore is dealposition 2 
	# note: sb is 1 in all other cases [besides headsup]
	if ( are_we_headsup() == TRUE) {
		if ( am_i_dealer() == TRUE) {
			$adp = MAX_PLAYERS;
		} else {
			$adp = MAX_PLAYERS - 1;
		}
	}
	else {
		# players_short holds the number of players missing from full table of 9
		my $players_short = MAX_PLAYERS - $OHS{'nplayersdealt'};
		# adjusted_dealposition is needed to select the appropriate hand list
		# the hand lists become wider as the deal position increases
		# when we are short-handed - we adjust hand lists up -
		# i.e. we open up (lower) starting hand requirements
		$adp = $OHS{'dealposition'} + $players_short;
	}
	
	#send_DebugMsg("	+++++=== adjusted_dealposition =", $adp);
		
	return $adp;
	
}

sub adjusted_dealpositionrais
{
	my $adpr = -1;
	
	# headsup is a special case, the dealer is sb, and therefore is dealposition 2 
	# note: sb is 1 in all other cases [besides headsup]
	# so, if the raiser is sb and he's the dealer - his position is most favorable
	# therefore it's MAX_PLAYERS
	if ( are_we_headsup() == TRUE) {
		if ( $OHS{'dealpositionrais'} == 1) { # if the raiser is the dealer
			$adpr = MAX_PLAYERS;
		} else {
			$adpr = MAX_PLAYERS - 1;
		}
	}
	else
	{
		# players_short holds the number of players missing from full table of 9
		my $players_short = MAX_PLAYERS - $OHS{'nplayersdealt'};
		# adjusted_dealposition is needed to select the appropriate hand list
		# the hand lists become wider as the deal position increases
		# when we are short-handed - we adjust hand lists up -
		# i.e. we open up (lower) starting hand requirements
		$adpr = $OHS{'dealpositionrais'} + $players_short;
	}
	
	#send_DebugMsg("	+++++=== adjusted_dealpositionrais =", $adpr);
	return $adpr;
}


# FUNCTION: c_bet
# returns TRUE when board is suitable for c-bet
sub c_bet
{
	# only cbet if raised preflop, first to act, and didn't call preflop
	if ( $OHS{'didswaground1'} && $OHS{'ac_first_into_pot'}  && !$OHS{'didcallround1'})
	{
		if ( $OHS{'nopponentsplaying'} == 1 && $OHS{'randomround'} > 0.15) 
		{
			send_DebugMsg("++++&&&&& always cbet vs 1 player &&&&&+++++");
			return TRUE;
		}
		
		if ( isflopdry() && $OHS{'nopponentsplaying'} < 4) 
		{
			send_DebugMsg("++++&&&&& cbet dry flop vs 2 or less &&&&&+++++");
			return TRUE;
		}
		
		if ( monster_draw() && $OHS{'nopponentsplaying'} > 1 ) 
		{
			send_DebugMsg("++++&&&&& cbet a monster draw vs 2 or more opponents &&&&&+++++");
			return TRUE;
		}
		if (flush_draw())
		{
			send_DebugMsg("++++&&&&& cbet a draw draw &&&&&+++++");
			return TRUE;
		}		
		if ( made_hand() && $OHS{'nopponentsplaying'} ) 
		{
			send_DebugMsg("++++&&&&& cbet any pair vs 2 when on the button and checked to &&&&&+++++");
			return TRUE;
		}
	}
	
	send_DebugMsg ( "board bad for cbetting");
	return FALSE;
}

# Take out this sub, reevaluate when heads up module is developed and tracker stats incorporated
# sub madehand_vs_lag_or_sb
# {
#	if ( $OHS{'nopponentsplaying'} == 1 && made_hand() && ! monster_hand() && !$OHS{'didswaground2'} &&
#		( $OHS{'dealposition'} == 2 && $OHS{'dealpositionrais'} == 1) 
		#	|| ( $OHS{'ptt_floppct'} > 25 || $OHS{'ptt_rpfr'} > 12)
#				&& $OHS{'call'} < 0.15 * $OHS{'balance'})
#	{
#		return TRUE;
#	}
	
#	return FALSE;
#}

# FUNCTION: float_the_turn
# returns TRUE when conditions are favorable to float the turn; FALSE otherwise
sub float_the_turn
{
	if ( $OHS{'didcallround2'})
	{
		if ( $OHS{'ac_first_into_pot'})
		{
			if ($OHS{'balance'} > 4 * $OHS{'pot'})
			{ 
				if ($OHS{'nopponentsplaying'} == 1)
				{
					if ($OHS{'randomround'} > 0.7)
					{
						send_DebugMsg ( "....... will float the turn ............");
						return TRUE;
					}
				}
			}
		}
	}
	
	send_DebugMsg ( "....... will NOT float the turn ............");
	return FALSE;
}
	
# FUNCTION: isoverpair
# returns TRUE when we hold an overpair to the board, FALSE otherwise
sub isoverpair
{
	if ( $OHS{'ispair'} && ( $OHS{'rankhiplayer'} > $OHS{'rankhicommon'})) {
		send_DebugMsg ( "holding an OVERPAIR");
		return TRUE;
	}
	
	#send_DebugMsg ( "NOT holding an OVERPAIR");
	return FALSE;
}

sub odds_to_call
{	
	if ( $OHS{'br'} == PREFLOP)
	{
		if ($OHS{'call'} <= 0.1 * $OHS{'pot'})
		{
			if ($OHS{'balance'} > 15* $OHS{'call'})
			{
				if ($OHS{'islist101'})
				{
					send_DebugMsg ("Calling pre on list 101 for less than 1/10 of the pot with stack");
					return TRUE;
				}
			}
		}
	}
	
	if ( $OHS{'br'} == FLOP)
	{
		send_DebugMsg ( "Street: flop");
		if ($OHS{'call'} <= 0.5 * $OHS{'potcommon'})
		{
			 send_DebugMsg ("Call <= 1/2 potcommon");
			 if ($OHS{'balance'} > 10 * $OHS{'potcommon'})
			{
				send_DebugMsg ("Stack > 10 * potcommon");
				return TRUE;
			}
		}
	}				
	
	
	if ($OHS{'br'} == TURN)
	{
		send_DebugMsg ("Street: turn");
		if ($OHS{'call'} <= 0.25 * $OHS{'potcommon'})
		{
			send_DebugMsg ("Call <=0.25 * potcommon");
			if ($OHS{'balance'} > 5 * $OHS{'potcommon'})
			{
				send_DebugMsg ("Stack > 5 * potcommon");
				send_DebugMsg ("Have odds to call");
				return TRUE;
			}
		}
	}
	
	if ($OHS{'call'} <= 2 * $OHS{'bblind'} ) 
	{
		send_DebugMsg ("Call <= 2 bb");
		if ($OHS{'balance'} > 15 * $OHS{'call'})
		{
			send_DebugMsg ("Stack > 15 bb");
			send_DebugMsg ("Have odds to call");				
			return TRUE;
		}
	}	
	
	send_DebugMsg ( "NO odds to call");
	return FALSE;
}
# FUNCTION: pot_equity_river
# evaluates pot equity to win or tie on the river based on nhandsti, nhandslo, nhandshi
sub pot_equity_river
{
my $per = 0;
if ($OHS{'br'} == RIVER)
	{
		$per = (0.5 * $OHS{'nhandsti'} + $OHS{'nhandslo'})/$OHS{'nhands'} ;
	}
	return $per;
	send_DebugMsg ( "Pot equity on the river:", $per);
	
}


# FUNCTION: TPTK
# returns TRUE when holding top pair with top kicker
# top pair can be two types of hands:
# - Ax with x on board as the rankhicommon;
# - AK with A on board as the rankhicommon.
sub TPTK
{
	# main condition: 
	# - not pocket pair;
	# - a hand is a pair;
	# - the highest player's card is A (14);
	# - the board is not paired.
	if ( !$OHS{'ispair'} && $OHS{'isonepair'} && $OHS{'rankhiplayer'} == 14 && $OHS{'nrankedcommon'} == 1 && 
			( $OHS{'rankloplayer'} == $OHS{'rankhicommon'} || 				# our low card is highest on board
			( $OHS{'rankhicommon'} == 14 && $OHS{'rankloplayer'} == 13) ) )	# A is paired and our low card is K
	{
		send_DebugMsg ( "****** holding TPTK *******");
		return TRUE;
	}
	
	# send_DebugMsg ( "****** NOT holding TPTK *******");
	return FALSE;
}

# FUNCTION: pot_commitment
# receives: nothing
# returns: array of currentbetx to balancex ratios of players that are still in the hand to determine if anybody is pot-committed
# (if a player has bet 40% or more of his initial balance into the pot than he is pot-committed)
# to find out whether anybody is pot-committed, call the max of this sub and if it is >= 0.4 than a player in the hand is pot-committed
# note that a player that is allin ($balancex == 0) will be assigned a value of 0.4 by this sub 
sub pot_commitment
{
	my $playersplayingbits = my_playersplayingbits();
	
	# Player balances and bets
	# specific chair balance	
	my $balance0 = $OHS{'balance0'};
	my $balance1 = $OHS{'balance1'};
	my $balance2 = $OHS{'balance2'};
	my $balance3 = $OHS{'balance3'};
	my $balance4 = $OHS{'balance4'};
	my $balance5 = $OHS{'balance5'};
	my $balance6 = $OHS{'balance6'};
	my $balance7 = $OHS{'balance7'};
	my $balance8 = $OHS{'balance8'};
	
# array for players'balances  
	my @players_balances = ();
	
	@players_balances = ( $balance0, $balance1, $balance2,
							 $balance3, $balance4, $balance5, $balance6,
							 $balance7, $balance8);

	# this array will hold the commitment to the pot and will ultimately be returned from this sub
	my @bet_to_stack_ratios_local = ();
	
	for ( my $i=0;$i<MAX_PLAYERS;$i++) 
	{
        if ( $playersplayingbits & (1 << $i)) 
		{
			my $to_push = 0;
			
			if ($players_balances[$i] == 0) 
			{
				$to_push = 0.4; # push 0.4 to indicate a committed stack (0.4 is the threshold).
			} 
			else 
			{
				# don't want to accidentally div by 0
				if ( $saved_stacks[$i] != 0)
				{
					$to_push = ( $saved_stacks[$i] - $players_balances[$i]) / $saved_stacks[$i];
				}
			}			
			push( @bet_to_stack_ratios_local, $to_push);
        }
	}	
	return @bet_to_stack_ratios_local;
}

# FUNCTION: average_hands_tracked
# to be used to identify situations when to calculate image 
# (18 tracked hands as a table average are needed to use "Image" function)
# receives: poker tracker total number of tracked hands from active_stats array
# returns: average tracked hands
sub average_hands_tracked
{
	my $total = 0;
	my $count = 0;

	# walk the array of PT hands and 
	foreach ( active_stats( PTT_HANDS)) {
		$total += $_;
		$count++;
	}

	if ( $count != 0) {
		return $total/$count;
	}

	return -1;
}

# Function our_stats, objective is to obtain our own tracker stats to be used as input in "Image" function
# receives: tracker stats from active_stats array and chairbit$abc from OH
# returns: user's (hero's) stas
# substitue "abc" in chairbit$abc symbol with our username at the poker site
sub our_stats
{ 
	my $chairbit = $OHS{'chairbit$abc'};
	my $our_ptt_pfr = 0;
	my $our_ptt_floppct = 0;
	
	if ( $_[0] == PTT_PFR) {
		# Poker Tracker pre-flop raise percentage for chair x
		if ( $chairbit & (1 << 8)) {
			$our_ptt_pfr = $OHS{'ptt_pfr8'};
		}
		elsif ( $chairbit & (1 << 7)) {
			$our_ptt_pfr = $OHS{'ptt_pfr7'};
		}		
		elsif ( $chairbit & (1 << 6)) {
			$our_ptt_pfr = $OHS{'ptt_pfr6'};
		}
		elsif ( $chairbit & (1 << 5)) {
			$our_ptt_pfr = $OHS{'ptt_pfr5'};
		}
		elsif ( $chairbit & (1 << 4)) {
			$our_ptt_pfr = $OHS{'ptt_pfr4'};
		}
		elsif ( $chairbit & (1 << 3)) {
			$our_ptt_pfr = $OHS{'ptt_pfr3'};
		}
		elsif ( $chairbit & (1 << 2)) {
			$our_ptt_pfr = $OHS{'ptt_pfr2'};
		}
		elsif ( $chairbit & (1 << 1)) {
			$our_ptt_pfr = $OHS{'ptt_pfr1'};
		}
		elsif ( $chairbit & (1 << 0)) {
			$our_ptt_pfr = $OHS{'ptt_pfr0'};
		}
		
		return $our_ptt_pfr;
	}
	
	if ( $_[0] == PTT_FLOPPCT) {
		# Poker swa flop percentage for chair x
		if ( $chairbit & (1 << 8)) {
			$our_ptt_floppct = $OHS{'ptt_floppct8'};
		}
		elsif ( $chairbit & (1 << 7)) {
			$our_ptt_floppct = $OHS{'ptt_floppct7'};
		}		
		elsif ( $chairbit & (1 << 6)) {
			$our_ptt_floppct = $OHS{'ptt_floppct6'};
		}
		elsif ( $chairbit & (1 << 5)) {
			$our_ptt_floppct = $OHS{'ptt_floppct5'};
		}
		elsif ( $chairbit & (1 << 4)) {
			$our_ptt_floppct = $OHS{'ptt_floppct4'};
		}
		elsif ( $chairbit & (1 << 3)) {
			$our_ptt_floppct = $OHS{'ptt_floppct3'};
		}
		elsif ( $chairbit & (1 << 2)) {
			$our_ptt_floppct = $OHS{'ptt_floppct2'};
		}
		elsif ( $chairbit & (1 << 1)) {
			$our_ptt_floppct = $OHS{'ptt_floppct1'};
		}
	
		elsif ( $chairbit & (1 << 0)) {
			$our_ptt_floppct = $OHS{'ptt_floppct'};
		}
		
		return $our_ptt_floppct;
	}
	
	return -1; # should never happen; means we got a bad argument from caller
}

# Function image, to determine how we are perceived by the table based on our recents stats
# the function is to be used whenever we have played at the same table for the last 18 hands or more
# (cash games, MTTs, SNG, will scarcely apply to rush tournaments)
# returns isimagetight
# if isimagetight = 1 we can loosen up our ranges and postflop betting somewhat
sub image
{ 	
	my $isimagetight = 0;
		
 	if  ( average_hands_tracked() >= 18 && our_stats( PTT_PFR) <= 9 && our_stats( PTT_FLOPPCT) < 13) 
	{ 
		$isimagetight = TIGHT; # tight is 1
	}
	else 
	{
		$isimagetight = LOOSE;	# loose is default and is 0
	}
	
	return $isimagetight;
}

# FUNCTION: sc_or_AXs_w_odds
sub sc_or_Axs_w_odds
{
	# only good for preflop
	if ( $OHS{'br'} != PREFLOP)
	{
		return FALSE;
	}
	
	if ( $OHS{'ac_preflop_pos'} >= 5) # only from late and button
	{
		#send_DebugMsg("late position");
		if (odds_to_call())  
		{
			#send_DebugMsg("	odds to call and deep enough");
			if ( $OHS{'nsuited'} == 2)
			{
				#send_DebugMsg("		we are suited");
				if ( $OHS{'rankhiplayer'} - $OHS{'rankloplayer'} <= 2 && $OHS{'rankhiplayer'} > 4)
				{
					send_DebugMsg("			connected or 1-gapper 5+");
					return TRUE;
				}
				if ( $OHS{'rankhiplayer'} == 14) {
					send_DebugMsg("			holding Axs");
					return TRUE;
				}
			}
		}
	}
	
	return FALSE;
}

# FUNCTION: small_pp_w_odds
sub small_pp_w_odds
{
	# only good for preflop
	if ( $OHS{'br'} != PREFLOP)
	{
		return FALSE;
	}
	
	if ( $OHS{'ac_preflop_pos'} >= 5) # only from late and button
	{
		#send_DebugMsg("late position");
		if ( $OHS{'nopponentsplaying'} >= 2 ) # we have 2 or more limpers/min-raisers
		{
			#send_DebugMsg("	2+ limpers/min-raisers ahead");
			if ( ( $OHS{'call'} <= 2 * $OHS{'bblind'} || odds_to_call()) && $OHS{'balance'} > 30 * $OHS{'bblind'}) 
			{
				#send_DebugMsg("		odds to call and deep enough");
				if ( $OHS{'ispair'} == TRUE && $OHS{'rankhiplayer'} < 9)
				{
					send_DebugMsg("	dealt a small pocket pair with odds to call");
					return TRUE;
				}
			}
		}
	}
	
	return FALSE;
}

# FUNCTION: flush_draw
# returns whether we have 4 cards of the same suit
# it avoids reporting a flush draw on the river as it's rather useless info
# NOTE: does NOT check for paired board
sub flush_draw
{
	if ( $OHS{'nsuited'} == 4 && $OHS{'nsuitedcommon'} < 4 && $OHS{'br'} != RIVER) # flush draw is useless on RIVER
	{
		#send_DebugMsg("holding a flush draw");
		return TRUE;
	}
	
	return FALSE;
}

# FUNCTION: paired_board
# returns TRUE if the board is paired
sub paired_board
{
	if ( $OHS{'nrankedcommon'} == 2 )
	{
		#send_DebugMsg("paired board");
		return TRUE;
	}
	
	return FALSE;
}

# FUNCTION: round_to_sb
# receives: the raise amount
# returns: the ceiling of the raise amount rounded up to sb
sub round_to_sb
{
	return ( ceil ( $_[0] / $OHS{'sblind'})) * $OHS{'sblind'}; # round UP
}

# FUNCTION: donk_bet 
# receives action in front on the flop, our action preflop, and our hand
# true or false
# to be used in how to determine a bet we are facing on the flop is a donk bet (an out-of-position bet)
# it is only useful to reraise it one-on-one
sub donk_bet
{ 
	if ( $OHS{'br'} == FLOP && $OHS{'nopponentsplaying'} < 4 && !$OHS{'ac_first_into_pot'} && 
		 $OHS{'didswaground1'} && !$OHS{'didcallround1'} && !$OHS{'didswaground2'})
	{
		send_DebugMsg("facing a donk bet");
		return TRUE;
	}
	return FALSE;
}

# FUNCTION: one pair strength, scale 0 to 5
sub onepair_str
{
	if ($OHS{'isonepair'})
	{
		if (paired_board())
		{
			if ($OHS{'nopponentsplaying'} ==1)
			{
				if ($OHS{'rankhiplayer'} > 12)
				{
					send_DebugMsg("onepair_str == 1");
					return 1;
				} 
			}
		}
		if (!paired_board())
		{
			if ($OHS{'islopair'})
			{
				send_DebugMsg("onepair_str == 2");
				return 2;
			}
			if ($OHS{'ismidpair'})
			{
				send_DebugMsg("onepair_str == 3");
				return 3;
			}
				
			if ($OHS{'mh_kickerbetter'} > 1)
			{
				if ($OHS{'rankhiplayer'} == $OHS{'rankhicommon'})
				{
					send_DebugMsg("onepair_str == 4");
					return 4;
				}
				if ($OHS{'rankloplayer'} == $OHS{'rankhicommon'})
				{
					send_DebugMsg("onepair_str == 4");
					return 4;
				}
			}
			if (isoverpair())
			{
				if ($OHS{'rankhiplayer'} <= 10)
				{
					send_DebugMsg("onepair_str == 4");
					return 4;
				}
			}
			if ($OHS{'mh_kickerbetter'} < 2)
			{
				if ($OHS{'rankhiplayer'} == $OHS{'rankhicommon'})
				{
					send_DebugMsg("onepair_str == 5");
					return 5;
				}
				if ($OHS{'rankloplayer'} == $OHS{'rankhicommon'})
				{
					send_DebugMsg("onepair_str == 5");
					return 5;
				}
			}
			if (isoverpair())
			{
				if ($OHS{'rankhiplayer'} > 10)
				{
					send_DebugMsg("onepair_str == 5");
					return 5;
				}
			}
		}
	}
	return 0;
}
# FUNCTION: two pair strength
# OH symbol mh_str_twopair is to be used in conjunction with it
# mh_str_twopair undervalues TPTK and overpair  when the board has a low pair by giving the hand a strength of only 3
# this sub corrects this deficiency by assigning TPTK and high overpair in this case a value of 4
sub twopair_str 
{	
	if ( $OHS{'istwopair'})
	{			
		if ($OHS{'npcbits'} == 1)
		{
			if ($OHS{'pcbits'} == 1)
			{
				if ($OHS{'rankhiplayer'} == 14)
				{
					send_DebugMsg("TWOPAIR: Board is twopair, I am holding an ace, twopair_str == 1");
					return 1;
				}
			}
			
			if ($OHS{'pcbits'} >= 16)
			{
				if ($OHS{'rankhiplayer'} > 12)
				{
					if ($OHS{'rankloplayer'} > 10)
					{	
						if ($OHS{'mh_kickerbetter'} < 2)
						{
							send_DebugMsg("TWOPAIR: holding top pair and best or second-best kicker, strength 4");
							return 4;
						}
					}
				}
			}
		}
		
		if ($OHS{'npcbits'} == 2)
		{
			if ( $OHS{'ispair'})
			{
				if ($OHS{'rankhiplayer'} <= 10)
				{
					if ($OHS{'rankhiplayer'} < $OHS{'rankhicommon'})
					{
						send_DebugMsg ("TWOPAIR: holding a pocket pair TT or less, board paired, overcards, strength 2");
						return 2;
					}
					
					if ($OHS{'rankhiplayer'} > $OHS{'rankhicommon'})
					{
						send_DebugMsg ("TWOPAIR: holding a pocket pair TT or less, board paired, no overcards, strength 3");
						return 3;
					}
				}
				if ($OHS{'rankhiplayer'} > 10)
				{
					if ($OHS{'rankhiplayer'} > $OHS{'rankhicommon'})
					{
						send_DebugMsg ("TWOPAIR: holding a pocket pair JJ or more, board paired, no overcards, strength 4");
						return 4;
					}
				}
			}
			if ($OHS {'nopponentsplaying'} == 1)
				{
					if ($OHS{'pcbits'} < 16)
					{
						return 4;
					}
					{
						return 5;
					}
				}
				
		}
		
		send_DebugMsg("TWOPAIR: returning a default mh_str_twopair=$OHS{'mh_str_twopair'}");
		return $OHS{'mh_str_twopair'};
	}
	
	return 0;
}

# FUNCTION: trips strength, scale 0 to 5
sub trips_str
{
	if ($OHS{'isthreeofakind'} && $OHS{'nrankedcommon'} == 3 && $OHS{'rankhiplayer'} == 14)
	{
		send_DebugMsg("TRIPS: board is trips, holding an ace, trips_str == 1");
		return 1;
	}

	if ($OHS{'isthreeofakind'} && $OHS{'nrankedcommon'} == 2 && $OHS{'trankcommon'} < 10)
	{
		send_DebugMsg("TRIPS: board is paired, less than TTT, trips_str == 2");
		return 2;
	}

	if ($OHS{'isthreeofakind'} && $OHS{'nrankedcommon'} == 2 && $OHS{'trankcommon'} >= 10)
	{
		send_DebugMsg("TRIPS: board is paired, TTT thru AAA, trips_str == 3");
		return 3;
	}

	if ($OHS{'isthreeofakind'} && $OHS{'npcbits'} == 2 && $OHS{'rankhiplayer'} < 10)
	{
		send_DebugMsg("TRIPS: a set less than TTT, trips_str == 4");
		return 4;
	}
	
	if ($OHS{'isthreeofakind'} && $OHS{'npcbits'} == 2 && $OHS{'rankhiplayer'} >= 10 )
	{
		send_DebugMsg("TRIPS: high set, TTT thru AAA, trips_str == 5");
		return 5;
	}
	
	return 0;
}

# FUNCTION: straight strength, scale 0 to 5
sub straight_str
{
	if ($OHS{'isstraight'} && $OHS{'nrankedcommon'} == 3)
	{
		send_DebugMsg("STRAIGHT: tripped board, straight_str == 1");
		return 1;
	}			
	
	if ($OHS{'isstraight'}  && $OHS{'nrankedcommon'} == 2 && $OHS{'nsuitedcommon'} == 3)
	{
		send_DebugMsg("STRAIGHT: paired board, possibly facing a flush, straight_str == 2");
		return 2;
	}

	if ($OHS{'isstraight'}  && ($OHS{'nrankedcommon'} == 2 || $OHS{'nsuitedcommon'} == 3))
	{
		send_DebugMsg("STRAIGHT: either paired or 3-flush board,  straight_str == 3");
		return 3;
	}
	
	if ($OHS{'isstraight'}  && $OHS{'nrankedcommon'} == 1 && $OHS{'nsuitedcommon'} < 3 && $OHS{'npcbits'} == 1)
	{
		send_DebugMsg("STRAIGHT: non-paired with no flush possibilities board,one hole card contributed, straight_str == 4");
		return 4;
	}

	if ($OHS{'isstraight'}  && $OHS{'nrankedcommon'} == 1 && $OHS{'nsuitedcommon'} < 3 && $OHS{'npcbits'} == 2)
	{
		send_DebugMsg("STRAIGHT: non-paired with no flush possibilities board,two hole cards contributed, straight_str == 5");
		return 5;
	}
	
	return 0;
}


			
# FUNCTION: flush strength, scale 0 to 5
sub flush_str
{
	if ($OHS{'isflush'} && $OHS{'nrankedcommon'} == 3)
	{
		send_DebugMsg("FLUSH: tripped board, flush_str == 1");
		return 1;
	}			
	
	if ($OHS{'isflush'}  && $OHS{'nrankedcommon'} == 2 && $OHS{'mh_nsuitedbetter'} > 4)
	{
		send_DebugMsg("FLUSH: paired board, number of cards that make a higher flush > 4 , flush_str == 2");
		return 2;
	}

	if ($OHS{'isflush'}  && (($OHS{'nrankedcommon'} == 2 && $OHS{'mh_nsuitedbetter'} <= 4 )||( $OHS{'nrankedcommon'} == 1 && $OHS{'mh_nsuitedbetter'} > 5  )))
	{
		send_DebugMsg("FLUSH: board paired, # cards that make a better flush <= 4 or board non-paired and # better cards is > 5, flush_str == 3");
		return 3;
	}

	if ($OHS{'isflush'}  && $OHS{'nrankedcommon'} == 1 && $OHS{'mh_nsuitedbetter'} <= 5)
	{
		send_DebugMsg("FLUSH: non-paired board, number of cards that make a higher flush <= 5 , flush_str == 4");
		return 4;
	}

	if ($OHS{'isflush'}  && $OHS{'nrankedcommon'} == 1 && $OHS{'mh_nsuitedbetter'} == 0)
	{
		send_DebugMsg("FLUSH: non-paired board, highest flush possible, flush_str == 5");
		return 5;
	}

	return 0;
}

# FUNCTION: made_hand
sub made_hand
{
	if ( onepair_str() > 1 
			|| twopair_str() > 0
			|| trips_str() > 0
			|| straight_str() > 0
			|| flush_str() > 0
			|| $OHS{'isfullhouse'} 
			|| ($OHS{'isfourofakind'} && $OHS{'nrankedcommon'} < 4)
			|| ($OHS{'isfourofakind'} && $OHS{'rankhiplayer'} > 12)
			|| $OHS{'isstraightflush'} 
			|| $OHS{'isroyalflush'})
	{
		send_DebugMsg ( "****** have a MADE HAND *******");
		return TRUE;
	}
	
	return FALSE;
}

# FUNCTION: monster_hand
# returns TRUE for high straight, high flush, 4 of a kind, straight flush, royal flush
sub monster_hand
{
	if (trips_str() > 3)
	{
		if($OHS{'nsuitedcommon'} < 3)
		{
			if($OHS{'nstraightfillcommon'} > 1)
			{
				send_DebugMsg ("Trips strength > 3 and no flush, little straight possibilities");
				return TRUE;
			}
		}
		if($OHS{'br'} == FLOP)	
		{
			send_DebugMsg ("Trip strength > 3 on the flop");
		return TRUE;
		}
		if($OHS{'br'} == TURN)
		{
			send_DebugMsg ("Trip strength > 3 on the turn");
		return TRUE;
		}
	}
	if (straight_str() == 5)
	{
		send_DebugMsg("Straight strength is 5");
	return TRUE;
	}
	if (flush_str() == 5)
	{
		send_DebugMsg("Flush strength is 5");
	return TRUE;
	}
	if (flush_str() == 4)
	{
		if($OHS{'nopponentsplaying'} < 3)
		{
			send_DebugMsg("Flush strength is 4, number of opponents < 3");
			return TRUE;
		}
	}
	
	if ($OHS{'mh_str_fullhouse'} > 1)
	{
		send_DebugMsg("Full house strength > 1, ain't folding");
		return TRUE;
	}
	
	if ($OHS{'isfourofakind'})
	{
		if ( $OHS{'nrankedcommon'} < 4)
		{
			send_DebugMsg ("Holding quads, board is not quads");
			return TRUE;
		}		
		if ( $OHS{'nrankedcommon'} == 4)
		{
			if($OHS{'rankhiplayer'} == 14)
			{
				send_DebugMsg ("Board is quads, holding an Ace, have the nuts");
				return TRUE;
			}			
			if ($OHS{'trankcommon'} == 14)
			{
				if ($OHS{'rankhiplayer'} == 13)
				{
					send_DebugMsg ("Board is quad Aces, holding a King, have the nuts");
					return TRUE;
				}
			}			
			if ($OHS{'rankhicommon'} == 14)
			{
				send_DebugMsg ("Board is quads, 1 Ace on the board, have the nuts");
				return TRUE;
			}
		}
	}	
	if ($OHS{'isstraightflush'})
	{
		send_DebugMsg ("Straightflush, ain't folding");
		return TRUE;
	}	
	if ($OHS{'isroyalflush'})
	{
		send_DebugMsg ("Royal flush");
		return TRUE;
	}	
	return FALSE;
}

# FUNCTION: bluff_the_bluffer
# bet greater than the previous bettor (0-MAX_PLAYERS)
sub bluff_the_bluffer
{
	if ( $OHS{'nopponentsraising'} == 1 && $OHS{'nopponentsplaying'} < 3 && !$OHS{'didswag'} 
#   && 	$OHS{'ptt_floppct'} > 25 
	&& $OHS{'balance'} > 100 * $OHS{'bblind'}  && $OHS{'randomround'} < 0.3 
							&& $OHS{'call'} < 0.3 * $saved_stacks[$OHS{'raischair'}])
	{
		send_DebugMsg ("bluffing the bluffer!");
		return TRUE;
	}

	return FALSE;
}

	

sub pl_alli
{
	# send_DebugMsg("<<<<<============ entered pl_alli=====================\n");
	# must call cache_Symbols here once to update the symbols once per cycle since pl_alli() is called first
	# send_DebugMsg('caching');
    cache_Symbols();
	
	# am_i_dealer();
	send_DebugMsg ("My Hand = ", card_val($OHS{'$$pc0'}), card_val($OHS{'$$pc1'}));
	send_DebugMsg ("Board = ", card_val($OHS{'$$cc0'}), card_val($OHS{'$$cc1'}), card_val($OHS{'$$cc2'}), card_val($OHS{'$$cc3'}), card_val($OHS{'$$cc4'}));
	
	calculate_preflop_stacks();
	
    my $falli = FALSE;

    if ( $OHS{'br'} == PREFLOP) 
	{
		#send_DebugMsg("---+++F++++ entered PREFLOP pl_alli ++++F++++---\n");
		
		# shove any two when balance less than 3bb
        if ( $OHS{'ac_first_into_pot'} && $OHS{'balance'} <= 3 * $OHS{'bblind'})
		{
			send_DebugMsg("ALLIN PREFLOP: shoving <3bb stack on ATC preflop");
            $falli = TRUE;
			goto falli_done;
        }
		
		if ( $OHS{'ac_first_into_pot'} && $OHS{'balance'} <= 6 * $OHS{'bblind'} && $OHS{'islist102'}) 
		{
			send_DebugMsg("ALLIN PREFLOP: shoving list102 first in pot when <=6bb");
			$falli = TRUE;
			goto falli_done;
		}
		
		if ( $OHS{'ac_first_into_pot'} && $OHS{'balance'} < 14 * $OHS{'bblind'}) 
		{
			if ( are_we_headsup() == TRUE) {
				if ( $OHS{'islist101'} ) # shove if first in pot
				{
					send_DebugMsg("ALLIN PREFLOP: shoving <14bb stack on list101 as dealer preflop headsup");
					$falli = TRUE;
					goto falli_done;
				}					 
			} 
			else 
			{
				if (    ( $OHS{'dealposition'} == 1 && $OHS{'islist101'}) || # sb is special
						( adjusted_dealposition() == 3 && $OHS{'islist103'}) ||
						( adjusted_dealposition() == 4 && $OHS{'islist104'}) ||
						( adjusted_dealposition() == 5 && $OHS{'islist105'}) ||
						( adjusted_dealposition() == 6 && $OHS{'islist106'}) ||
						( adjusted_dealposition() == 7 && $OHS{'islist107'}) ||
						( adjusted_dealposition() == 8 && $OHS{'islist108'}) ||
						( adjusted_dealposition() == 9 && $OHS{'islist109'}))
				{
					send_DebugMsg("ALLIN PREFLOP: shoving <14bb stack according to lists preflop");
					$falli = TRUE;
					goto falli_done;
				}
			}
        }
        
		if ( $OHS{'call'} > 2 * $OHS{'bblind'} && $OHS{'call'} > 0.2 * $OHS{'balance'} && $OHS{'islist100'})
        {
			send_DebugMsg("ALLIN PREFLOP: shoving when call is > 2bb and > 20% of balance and hand is list100 preflop");
            $falli = TRUE;
			goto falli_done;
        }	
		
		# reshove short stack tightly to a high raise
		# (pocket hand has to be one better
		# than the position-wise raise list for the raising chair)
        if ( $OHS{'call'} > 2 * $OHS{'bblind'} && $OHS{'balance'} < 14 * $OHS{'bblind'}) 
		{
			if ( are_we_headsup() == TRUE) 
			{
				if ( $OHS{'islist102'})
				{
					send_DebugMsg("ALLIN PREFLOP: reshoving short stack tightly to a high raise preflop headsup");
					$falli = TRUE;
					goto falli_done;
				}					 
			} 
			else {
				if (    ( $OHS{'dealpositionrais'} == 1 && $OHS{'nopponentsraising'} == 1 && $OHS{'islist102'}) ||
						( adjusted_dealpositionrais() == 3 && $OHS{'islist100'}) ||
						( adjusted_dealpositionrais() == 4 && $OHS{'islist103'}) ||
						( adjusted_dealpositionrais() == 5 && $OHS{'islist104'}) ||
						( adjusted_dealpositionrais() == 6 && $OHS{'islist105'}) ||
						( adjusted_dealpositionrais() == 7 && $OHS{'islist106'}) ||
						( adjusted_dealpositionrais() == 8 && $OHS{'islist107'}) ||
						( adjusted_dealpositionrais() == 9 && $OHS{'islist108'}))
				{
					send_DebugMsg("ALLIN PREFLOP: reshoving short stack tightly to a high raise preflop");
					$falli = TRUE;
					goto falli_done;
				}
			}
        }

		# shoving short stack on AA, KK, QQ regardless of the action in front
	
		if ( $OHS{'balance'} < 14 * $OHS{'bblind'} && $OHS{'islist110'})
        {
			send_DebugMsg("ALLIN PREFLOP: shoving when balance < 14bb and hand is list110 preflop");
            $falli = TRUE;
			goto falli_done;
        }	
        
		## reshove short stack to a limp/minraise according to raise list
        if ( $OHS{'balance'} < 14 * $OHS{'bblind'} &&
                        $OHS{'call'} <= 2 * $OHS{'bblind'} ) 
		{
			if ( are_we_headsup() == TRUE) 
			{
				if ( $OHS{'islist102'})
				{
					send_DebugMsg("ALLIN PREFLOP: reshoving short stack to a limp/minraise preflop when heads up!");
					$falli = TRUE;
					goto falli_done;
				}					 
			} 
			else # we are not headsup
			{	
				if (    ( $OHS{'dealpositionrais'} == 1 && $OHS{'islist102'}) ||
						( adjusted_dealpositionrais() == 3 && $OHS{'islist103'}) ||
						( adjusted_dealpositionrais() == 4 && $OHS{'islist104'}) ||
						( adjusted_dealpositionrais() == 5 && $OHS{'islist105'}) ||
						( adjusted_dealpositionrais() == 6 && $OHS{'islist106'}) ||
						( adjusted_dealpositionrais() == 7 && $OHS{'islist107'}) ||
						( adjusted_dealpositionrais() == 8 && $OHS{'islist108'}) ||
						( adjusted_dealpositionrais() == 9 && $OHS{'islist109'}))
				{
					send_DebugMsg("ALLIN PREFLOP: reshoving short stack preflop to a limp/minraise!");
					$falli = TRUE;
					goto falli_done;
				}
			}
        }
		
		# send_DebugMsg("---+++F++++ done PREFLOP pl_alli $falli = ", $falli, " ++++F++++---\n");
    } # end PREFLOP
	
	if ( $OHS{'br'} == FLOP) 
	{	
		# raised preflop on a monster to get it in on the flop (reflect in swag)
		if ( $OHS{'didswaground1'} && our_chips_invested()  > 0.25 * $saved_stacks[get_mychair()] && $OHS{'islist110'}) 
		{
			send_DebugMsg ("ALLIN FLOP: raised preflop on a monster to get it in on the flop - islist110");
			$falli = TRUE;
			goto falli_done;
		}
		
		# when raised from early with multiple callers, hit the flop or holding an overpair and flop is not flush
		if ( $OHS{'ac_first_into_pot'})  # first to act
		{
			send_DebugMsg(" First to act");
			if ( $OHS{'pot'} > 0.35 * $OHS{'balance'}) 
			{
				send_DebugMsg("	pot > 1/3 balance");
				if ( $OHS{'nsuitedcommon'} < 3)
				{
					send_DebugMsg("flop is not flush");
					if ( $OHS{'islist105'})
					{	send_DebugMsg ("List 105");
						if (onepair_str() > 3 || twopair_str() > 3)
						{
							send_DebugMsg ("Holding top pair, overpair, TPTK, or twopair strength > 3");
							if ($OHS{'nopponentsplaying'} > 1) 
							{
								send_DebugMsg ("vs more than 1");
								send_DEbugMsg ("ALLIN FLOP: raised pre on list105, hit the flop hard, shoving vs > 1");
								$falli = TRUE;
								goto falli_done;
							}
 						}
					}
				}
				if (trips_str() > 0)
				{
					send_DebugMsg ("trips strength > 0");
					if ( !monster_hand())
					{
						send_DebugMsg ("Not a monster though");
						if ($OHS{'nopponentsplaying'} > 2)
						{
							send_DebugMsg ("Vs more than 2");
							send_DebugMsg("ALLIN FLOP: low trips");
							$falli = TRUE;
							goto falli_done;
						}
					}
				}
			}
		}	
		
		# when a made hand is at risk of being outdrawn and there is significant action in front
		if ( !$OHS{'ac_first_into_pot'})
		{
			send_DebugMsg ("FLOP: not first to act");
			if ($OHS{'call'} > 0.35 * $OHS{'potcommon'})
			{
				send_DebugMsg ("call > 1/3 potcommon");
				if ($OHS{'call'} > 0.2 * $OHS{'balance'}) 
				{
					send_DebugMsg ("call > 1/5 of remaing stack");
					if (twopair_str() > 2||					
						trips_str() > 1 || 
						(onepair_str() == 5 && $OHS{'nopponentsraising'} < 2) ||
						(isoverpair() && $OHS{'nopponentsplaying'} < 3 && $OHS{'nopponentsraising'} < 2 ) || 
						(straight_str() && !monster_hand()) ||
						(flush_str () && !monster_hand()))
					{
						send_DebugMsg ("ALLIN FLOP: a made hand is at risk of being outdrawn and there is significant action in front");
					$falli = TRUE;
					goto falli_done;
					}
				}
			}
		}
		
		# when holding a top overpair on flop with significant action in front
		if ( $OHS{'islist110'} && $OHS{'nopponentsraising'} < 2)  # holding AA or KK, not raise/reraise in front
		{
			send_DebugMsg(" hole cards are AA or KK");
			if ( $OHS{'rankhiplayer'} >= $OHS{'rankhicommon'}) 
			{
				send_DebugMsg("	no overcards on the flop");
				if ( $OHS{'call'} > 0.35 * $OHS{'potcommon'})
				{
					send_DebugMsg("	amount to call > 1/3 potcommon");
					if ( $OHS{'call'} > 0.2 * $OHS{'balance'})
					{
						send_DebugMsg("ALLIN FLOP: more than 1/5 of our balance to call");
						$falli = TRUE;
						goto falli_done;
					}
				}
			}
		}
						
		# vs 1 or 2 opponents, hit the board as short stack or have a flush or straight draw;
		if ( ( $OHS{'balance'} < 10 * $OHS{'bblind'} && $OHS{'nopponentsplaying'} <= 2) && 
				( 	made_hand() || 
					flush_draw() == TRUE ||	# flush draw 
					$OHS{'nstraight'} >= 4 ||	# straight draw or made straight 
					$OHS{'istwopair'} ||
					$OHS{'isthreeofakind'}
				)
			)
		{
			send_DebugMsg ("ALLIN FLOP: vs 1 or 2 opps, hit the board as short stack or have a flush/straight draw");
			$falli = TRUE;
			goto falli_done;
		}

		# having raised pre, facing a donk bet, reraising with fold equity
		if ( donk_bet() && $OHS{'balance'} < 16 * $OHS{'bblind'} && $OHS{'call'} < 0.3 * $OHS{'balance'})
		{
			send_DebugMsg ("ALLIN FLOP: shoving short stack to a donk bet");
			$falli = TRUE;
			goto falli_done;
		}
		
		# holding a monster, for more than 40% of the stack
		if (monster_hand() && $OHS{'call'} >= 0.3 * $OHS{'balance'})
		{
			send_DebugMsg ("ALLIN FLOP: monster facing a bet for 30% or more of the stack");
			$falli = TRUE;
			goto falli_done;
		}
	
    } # end FLOP

    if ( $OHS{'br'} == TURN) 
	{
		if ( $OHS{'ac_first_into_pot'})  # First to act
		{
			send_DebugMsg(" First to act");
			if (made_hand()) 
			{
				send_DebugMsg("	made hand");
				if ( $OHS{'ishandup'})
				{
					send_DebugMsg("	our hand improved on the turn");
					if ($OHS{'balance'} < 1.3 * $OHS{'pot'}) 
					{
						send_DebugMsg("ALLIN TURN: first to act, hand improved,  shoving into a large pot ");
						$falli = TRUE;
						goto falli_done;
					}
				}
			}
		}
		
		# holding a monster with significant action in front
		if ( $OHS{'call'} > 0.4 * $OHS{'balance'} && 
			monster_hand())
		{
			send_DebugMsg ("ALLIN TURN: facing a bet for 40% or more of the stack and holding a monster");
			$falli = TRUE;
			goto falli_done;
		}
					
		# holding an overpair or top pair vs 1 or 2 opponents, pot-committed, on a relatively dry board
		
		if ( $OHS{'nopponentsplaying'} <= 2 && $OHS{'balance'}  < 1.4 * $OHS{'pot'})  # vs 1 or 2 opponents, big pot
		{
			send_DebugMsg(" Vs 1 or 2");
			if (onepair_str() > 3 || twopair_str() > 3) 
			{
				send_DebugMsg("	highest pair or twopair strength");
				if ( $OHS{'nsuitedcommon'} < 3)
				{
					send_DebugMsg("	less than 3 to a flush on the board");
					if ( $OHS{'nstraightfillcommon'} > 1) 
					{
						send_DebugMsg("ALLIN FLOP: hit the flop or holding an overpair, no-one has flush, little straight possibilities, shoving into a large pot ");
						$falli = TRUE;
						goto falli_done;
					}
				}
			}
		}
		
		# hand improved on the turn, first to act
						
	} # end TURN
	
    if ( $OHS{'br'} == RIVER) 
	{
		# on a monster or high set first to act with significant action on the turn	
		if ( $OHS{'ac_first_into_pot'})  # First to act
		{
			send_DebugMsg(" First to act");
			if ($OHS{'didswaground3'}) 
			{
				send_DebugMsg("	swagged on the turn");
				if ( $OHS{'balance'} < 1.9 * $OHS{'pot'})
				{
					send_DebugMsg("	balance is less than about 2x the pot");
					if ( monster_hand() || 
					( trips_str() && $OHS{'nsuitedcommon'} < 3 && $OHS{'nstraightfillcommon'} > 1)) 
					{
						send_DebugMsg("ALLIN RIVER: first to act, swagged the turn,  shoving a monster or trips into a large pot ");
						$falli = TRUE;
						goto falli_done;
					}
				}
			}
		}
		
		# representing flush heads-up on the  river 35 % of the time when villain's action indicates 
		# he does not have a drawing hand and the whole stack is not at risk
		if ( $OHS{'ac_first_into_pot'} && $OHS{'nopponentsplaying'} == 1 && bluff_flush() && !riskofruin() && 
				$OHS{'randomround'} <= 0.29 && $OHS{'didcallround1'} && ($OHS{'didcallround2'} || $OHS{'didcallround3'}))
		{
			send_DebugMsg ("ALLIN RIVER: representing flush");
			$falli = TRUE;
			goto falli_done;
		}
				
		if ( $OHS{'ac_first_into_pot'}) 
		{	send_DebugMsg ("River: first to act");
			if (made_hand())
			{
				send_DebugMsg ("made hand");
				if ($OHS{'ishandup'})
				{
					send_DebugMsg ("Hand improved");
					if ($OHS{'balance'} < $OHS{'pot'})
					{
						send_DebugMsg ("balance less than the pot");		
						send_DebugMsg ("ALLIN RIVER: first to act and hand improved on the river, for less than pot");
						$falli = TRUE;
						goto falli_done;
					}
				}
			}
		}
		
		# on a monster or high set with no flush possibilities and insignificant straight possibilities with action on the river
		if ($OHS{'pot'} > 0.7 * $OHS{'balance'})
		{
			send_DebugMsg ("Large pot");
			if ( $OHS{'didcallround4'}
			  || $OHS{'didswaground4'}
			  || $OHS{'didchecround4'})
			{
				send_DebugMsg ("got reraised on the river");
				if (monster_hand())
				{
					send_DebugMsg("ALLIN RIVER:got reraised, holding a monster, shoving into a big pot");
					$falli = TRUE;
					goto falli_done;
				}
			}
		}		
		
	} # end RIVER

falli_done:
	#send_DebugMsg("<<<<<==================== returning from pl_alli, falli=", $falli, "=====================\n");
    return $falli;
}

# FUNCTION: pl_swag
sub pl_swag
{
	my $fswag = 0;
	
	#send_DebugMsg("xxxxxxx--- entered pl_swag, br = $OHS{'br'} \n");
	
	if ( $OHS{'br'} == PREFLOP) 
	{
		if ( $OHS{'ac_first_into_pot'} && $OHS{'balance'} >= 14 * $OHS{'bblind'}) 
		{
			if ( are_we_headsup() == TRUE) 
			{
				if ( $OHS{'islist101'})
				{
					$fswag = 3 * $OHS{'bblind'};
					send_DebugMsg("SWAG PREFLOP: raising list101 when headsup! $fswag");
					goto pl_swag_done;
				}					 
			} 
			else 
			{
				if (    ( $OHS{'dealposition'} == 1 && $OHS{'islist101'}) || # sb is special
							( adjusted_dealposition() == 3 && $OHS{'islist103'}) ||
							( adjusted_dealposition() == 4 && $OHS{'islist104'}) ||
							( adjusted_dealposition() == 5 && $OHS{'islist105'}) ||
							( adjusted_dealposition() == 6 && $OHS{'islist106'}) ||
							( adjusted_dealposition() == 7 && $OHS{'islist107'}) ||
							( adjusted_dealposition() == 8 && $OHS{'islist108'}) ||
							( adjusted_dealposition() == 9 && $OHS{'islist109'}))
				{
					$fswag = 3 * $OHS{'bblind'};
					send_DebugMsg("SWAG PREFLOP: raising according to list/position: $fswag");
					goto pl_swag_done;
				}
			}
		}
		
		if ( ( $OHS{'call'} > 2 * $OHS{'bblind'}) && $OHS{'islist100'})
		{
			$fswag = round_to_sb( 3.2 * $OHS{'call'});
			send_DebugMsg("SWAG PREFLOP: call > 2bb && list100: $fswag");
			goto pl_swag_done;
		}
		
		# reraise 50% of the time tightly to a raise
		# (pocket hand has to be one better
		# than the position-wise raise list for the raising chair)
		if ( $OHS{'call'} > 2 * $OHS{'bblind'} && $OHS{'balance'} >= 14 * $OHS{'bblind'} && $OHS{'randomround'} <= 0.5) 
		{
			if ( are_we_headsup() == TRUE) 
			{
				if ( $OHS{'islist102'} || 
					 ( am_i_dealer() == TRUE && $OHS{'islist101'}) )
				{
					$fswag = round_to_sb( 2.4 * $OHS{'call'});
					send_DebugMsg("SWAG PREFLOP: re-raising tightly list102 or 101 when dealer headsup! : $fswag");
					goto pl_swag_done;
				}					 
			} 
			else 
			{
				if (    ( $OHS{'dealpositionrais'} == 1 && $OHS {'nopponentsplaying'} == 1 && !$OHS{'didswag'} && $OHS{'islist102'}) ||
						( adjusted_dealposition() == 2 && $OHS{'islist105'}) ||
						( adjusted_dealposition() == 3 && $OHS{'islist100'}) ||
						( adjusted_dealposition() == 4 && $OHS{'islist103'}) ||
						( adjusted_dealposition() == 5 && $OHS{'islist104'}) ||
						( adjusted_dealposition() == 6 && !$OHS{'didswag'} && $OHS{'islist105'}) ||
						( adjusted_dealposition() == 7 && !$OHS{'didswag'} && $OHS{'islist106'}) ||
						( adjusted_dealposition() == 8 && !$OHS{'didswag'} && $OHS{'islist107'}) ||
						( adjusted_dealposition() == 9 && !$OHS{'didswag'} && $OHS{'islist108'}))
				{
					$fswag = round_to_sb( 2.4 * $OHS{'call'});
					send_DebugMsg("SWAG PREFLOP: re-raising tightly listX - 1: $fswag");
					goto pl_swag_done;
				}
			}
		}
		
		## reraise to a limp/minraise according to raise list
		if ( $OHS{'ac_first_into_pot'} == FALSE && $OHS{'balance'} >= 14 * $OHS{'bblind'} &&
				$OHS{'call'} <= 2 * $OHS{'bblind'} ) 
		{
			if ( are_we_headsup() == TRUE)
			{
				if ( $OHS{'islist102'} == TRUE)
				{
					$fswag = round_to_sb( $OHS{'call'} * ( 1.8 + 1.8 * $OHS{'randomround'}));
					send_DebugMsg("SWAG PREFLOP: re-raising a limp/minraise list102 when headsup: $fswag");
					goto pl_swag_done;
				}
			} 
			if (    ( $OHS{'dealpositionrais'} == 1 && $OHS{'islist102'}) ||
						( adjusted_dealpositionrais() == 3 && $OHS{'islist104'}) ||
						( adjusted_dealpositionrais() == 4 && $OHS{'islist104'}) ||
						( adjusted_dealpositionrais() == 5 && $OHS{'islist105'}) ||
						( adjusted_dealpositionrais() == 6 && $OHS{'islist106'}) ||
						( adjusted_dealpositionrais() == 7 && $OHS{'islist107'}) ||
						( adjusted_dealpositionrais() == 8 && $OHS{'islist108'}) ||
						( adjusted_dealpositionrais() == 9 && $OHS{'islist109'}))
			{
					$fswag = round_to_sb( $OHS{'call'} * ( 2.5 + 1.5 * $OHS{'randomround'}));
					send_DebugMsg("SWAG PREFLOP: re-raising a limp/minraise: $fswag");
					goto pl_swag_done;
			}
			
		}
		
		# reraise big only on big hands over multiple limpers AA KK QQ in list110
		if ( $OHS{'nopponentsplaying'} > 1 && $OHS{'balance'} >= 14 * $OHS{'bblind'} &&
				$OHS{'call'} <= 2 * $OHS{'bblind'} && $OHS{'islist110'})
		{
			$fswag = round_to_sb( (0.8 + 0.6 * $OHS{'randomround'}) * $OHS{'pot'}); # XXX: i think pot is better than call here
			send_DebugMsg("SWAG PREFLOP: reraise big only on big hands over multiple limpers AA KK QQ in list110: $fswag");
			goto pl_swag_done;
		}
		# send_DebugMsg("xxxxxxx--- END fswag preflop, $fswag = ", $fswag);
	} # END PREFLOP
	
	if ( $OHS{'br'} == FLOP)  #FLOP
	{
		#send_DebugMsg("           pl_swag on the flop\n");

		$fswag = 0;
		
		if  ( c_bet() == TRUE) 						
		{
			$fswag = round_to_sb( $OHS{'potcommon'} * ( 0.6 + 0.55 * $OHS{'randomround'}));
			send_DebugMsg("SWAG FLOP: c_bet $fswag");
			goto pl_swag_done;
		}
		
		if ($OHS{'ac_first_into_pot'})
		{
			if 	((flush_draw() && $OHS{'balance'} > 18 * $OHS{'bblind'}) ||
				(( straight_str() >= 2 && straight_str() <= 3) && $OHS{'nopponentsplaying'} > 1)  ||
				(flush_str() && !monster_hand() && $OHS{'nopponentsplaying'} > 2) ||
				(( twopair_str() >= 2 && twopair_str() <= 4) && $OHS{'nopponentsplaying'} > 1) ||
				(( trips_str() >= 1 && trips_str() <= 2) && $OHS{'nopponentsplaying'} > 2) ||
				(trips_str() > 2 && ($OHS{'nsuitedcommon'} > 1 || $OHS{'nstraightcommmon'} > 1)))
			{
				$fswag = round_to_sb( $OHS{'potcommon'} * ( 0.55 + 0.45 * $OHS{'randomround'}));
				send_DebugMsg("SWAG FLOP: leading out on a medium strength made hand at risk of being outdrawn or a draw");
				send_DebugMsg ("My Hand = ", card_val($OHS{'$$pc0'}), card_val($OHS{'$$pc1'}));
				send_DebugMsg ("Board = ", card_val($OHS{'$$cc0'}), card_val($OHS{'$$cc1'}), card_val($OHS{'$$cc2'}), card_val($OHS{'$$cc3'}), card_val($OHS{'$$cc4'}));
			goto pl_swag_done;
			}
		}
		
		# leading out on draws or relatively strong made hands that can be outdrawn 

		if ($OHS{'ac_first_into_pot'})
		{
			if (onepair_str() > 3)
			{
				if ($OHS{'nsuitedcommon'} < 3)
				{
					if ($OHS{'nstraightcommon'} < 3)
					{
						if ($OHS{'nopponentsplaying'} > 1)
						{
							$fswag = round_to_sb( $OHS{'potcommon'} * ( 0.55 + 0.45 * $OHS{'randomround'}));
							send_DebugMsg("SWAG FLOP: leading out a small to mid pair");
							send_DebugMsg ("My Hand = ", card_val($OHS{'$$pc0'}), card_val($OHS{'$$pc1'}));
							send_DebugMsg ("Board = ", card_val($OHS{'$$cc0'}), card_val($OHS{'$$cc1'}), card_val($OHS{'$$cc2'}), card_val($OHS{'$$cc3'}), card_val($OHS{'$$cc4'}));
							goto pl_swag_done;
						}
					}
				}
			}
		}
		
		if ($OHS{'ac_first_into_pot'})
		{
			if (monster_draw())
			{
				$fswag = round_to_sb( $OHS{'potcommon'} * ( 0.55 + 0.45 * $OHS{'randomround'}));
				send_DebugMsg("SWAG FLOP: leading out on a medium strength made hand at risk of being outdrawn or a draw");
				send_DebugMsg ("My Hand = ", card_val($OHS{'$$pc0'}), card_val($OHS{'$$pc1'}));
				send_DebugMsg ("Board = ", card_val($OHS{'$$cc0'}), card_val($OHS{'$$cc1'}), card_val($OHS{'$$cc2'}), card_val($OHS{'$$cc3'}), card_val($OHS{'$$cc4'}));
			goto pl_swag_done;
			}
		}
		
		if ( $OHS{'ac_first_into_pot'})
		{
			if ($OHS{'nopponentsplaying'} < 3)
			{
				if (onepair_str() > 2||twopair_str() > 2)
				{
					$fswag = round_to_sb( $OHS{'potcommon'} * ( 0.55 + 0.45 * $OHS{'randomround'}));
					send_DebugMsg("SWAG FLOP: first to act from button on dry flop");
					goto pl_swag_done;
				}
			}
		}
			
		if ( $OHS{'ac_first_into_pot'})
		{
			if ($OHS{'dealposition'} == max(active_positions()))
			{
				if ($OHS{'balance'} > 18 * $OHS{'bblind'})
				{
					if (isflopdry()) 
					{
					$fswag = round_to_sb( $OHS{'potcommon'} * ( 0.55 + 0.45 * $OHS{'randomround'}));
					send_DebugMsg("SWAG FLOP: first to act from button on dry flop");
					goto pl_swag_done;
					}
				}
			}
		}
				
		if (!$OHS{'ac_first_into_pot'})
		{
			send_DebugMsg ("First to act");
			if (monster_hand())
			{
				if (!$OHS{'didcallround2'})
				{
					if (!$OHS{'didswaground2'})
					{
						if ($OHS{'call'} <= 3 * $OHS{'bblind'})
						{
						$fswag = round_to_sb( $OHS{'call'} * ( 2 + 0.8 * $OHS{'randomround'}));
						send_DebugMsg("SWAG FLOP: Reraised on a good made hand or monster draw: $fswag");
						goto pl_swag_done;
						}
					}
				}
			}
		}
				
		# sizeable reraise on good made hands for value/fold equity
		if 	( !$OHS{'ac_first_into_pot'})
		{
			if ($OHS{'balance'} > 15 * $OHS{'call'})
			{
				if (!$OHS{'didcallround2'})
				{
					if (!$OHS{'didswaground2'})
					{
						if ((isoverpair() && $OHS{'nsuitedcommon'} < 3) ||
							(isoverpair() && $OHS{'nopponentsplaying'} == 1) ||
							monster_draw()  || 
							(twopair_str() > 3 && $OHS{'nsuitedcommon'} < 3) ||
							(twopair_str() > 1 && $OHS{'nopponentsplaying'} == 1) || 
							trips_str() > 1 ||
							(trips_str() > 0 && $OHS{'nopponentsplaying'} == 1) ||
							straight_str() > 2  ||
							(straight_str() > 0 && $OHS{'nopponentsplaying'} == 1) ||
							flush_str() > 2  || 
							(flush_str() > 0 && $OHS{'nopponentsplaying'} == 1) ||
							$OHS{'isfullhouse'})
						{
							if ($OHS{'call'} < 0.3 * $OHS{'potcommon'})
							{
							$fswag = round_to_sb( $OHS{'pot'} * ( 0.6 + 0.8 * $OHS{'randomround'}));
							send_DebugMsg("SWAG FLOP: Reraised on a good made hand or monster draw: $fswag");
							goto pl_swag_done;
							}
							else 
							{ 
							$fswag = round_to_sb( $OHS{'call'} * ( 2.7 + 0.8 * $OHS{'randomround'}));
							send_DebugMsg("SWAG FLOP: Reraised on a good made hand or monster draw: $fswag");
							goto pl_swag_done;
							}
						}
					}
				}
			}
		}
				
		if (!$OHS{'ac_first_into_pot'})
		{
			if (!$OHS{'didswaground2'})
			{
				if ($OHS{'randomround'} > 0.175)
				{
					if (($OHS{'islist100'} && $OHS{'rankhiplayer'} >= $OHS{'rankhicommon'}) ||
						monster_hand() ||
						onepair_str() > 3 ||
						twopair_str() > 3 ||
						trips_str() > 1 ||
						straight_str() > 1  ||
						flush_str() > 1 )
					{		
						if ($OHS{'call'} < 0.3 * $OHS{'potcommon'})
						{
							$fswag = round_to_sb( $OHS{'potcommon'} * ( 0.6 + 0.8 * $OHS{'randomround'}));
							send_DebugMsg("SWAG FLOP: Reraised on AA or KK or monster: $fswag");
							goto pl_swag_done;
						}					
						else 
						{ 
							$fswag = round_to_sb( $OHS{'call'} * ( 2.7 + 0.8 * $OHS{'randomround'}));
							send_DebugMsg("SWAG FLOP: Reraised on AA or KK or monster: $fswag");
							goto pl_swag_done;
						}
					}
				}
			}
		}
		
		if ($OHS{'islist110'})
		{
			if (isoverpair())
			{
				if ($OHS{'call'} > 0.35 * $OHS{'potcommon'})
				{
					if ($OHS{'call'} <= 0.2 * $OHS{'balance'})
					{
						if (!$OHS{'didcallround2'})
						{
							if (!$OHS{'didswaground2'})
							{
								$fswag = round_to_sb( $OHS{'potcommon'} * ( 0.7 + 0.8 * $OHS{'randomround'}));
								send_DebugMsg("SWAG FLOP: islist110 on dry flop ", $fswag);
								goto pl_swag_done;
							}
						}
					}
				}
			}
		}
		
		# facing a donk bet
		if ( donk_bet())
		{
			if ($OHS{'balance'} >= 14 * $OHS{'bblind'})
			{
				if ($OHS{'call'} < 0.15 * $OHS{'balance'})
				{
					$fswag = round_to_sb( $OHS{'potcommon'} * ( 0.7 + 0.8 * $OHS{'randomround'}));
					send_DebugMsg("SWAG FLOP: reraising donk bet ", $fswag);
					goto pl_swag_done;				
				}
			}
		}
	}
	# END FLOP
	if ( $OHS{'br'} == TURN) 
	{
		$fswag = FALSE;	
		if ( $OHS{'ac_first_into_pot'})
		{
			if (made_hand())
			{
				if ($OHS{'ishandup'})
				{
					if ($OHS{'balance'} > $OHS{'pot'})
					{
						$fswag = round_to_sb(( 0.4 + 0.55 * $OHS{'randomround'}) * $OHS{'potcommon'});
						send_DebugMsg("SWAG TURN: first to act hand improved on the turn: $fswag");
						goto pl_swag_done;
					}
				}
			}
		}
		
		if (float_the_turn() && (made_hand()))
			{
				$fswag = round_to_sb(( 0.55 + 0.55 * OHS{'randomround'}) * $OHS{'pot'});
			send_DebugMsg("SWAG TURN: float_turn with made hand");
			goto pl_swag_done;
			}
		if ($OHS{'ac_first_into_pot'} && made_hand() && $OHS{'dealposition'} == max(active_positions()))
		{
			$fswag = round_to_sb(( 0.55 + 0.55 * OHS{'randomround'}) * $OHS{'pot'});
			send_DebugMsg("SWAG TURN: made hand in position when checked to");
			goto pl_swag_done;
		}
		
		#bet turn when flop is checked, less than 3 to a straight, less than 3 to a flush, turn card is less than a J
		
		if ( $OHS{'ac_first_into_pot'})
		{
			if ($OHS{'didchecround2'})
			{
				if (!$OHS{'didcallround2'})
				{
					if (!$OHS{'didswaground2'})
					{
						if ($OHS{'nsuitedcommon'} < 3)
						{
							if ($OHS{'nstraightcommon'} < 3)
							{
								if ($OHS{'$cr3'} < 12)
								{
									if ($OHS{'balance'} > 3* $OHS{'pot'})
									{
										$fswag = round_to_sb( $OHS{'potcommon'} * ( 0.55 + 0.65 * $OHS{'randomround'}));
										send_DebugMsg("SWAG TURN: first to act when flop is checked, non-straight, non-flushy board, turn card is less than J, balance > 3 * pot");
										goto pl_swag_done;
									}
								}
							}
						}
					}
				}
			}
		}			
		
		if ($OHS{'ac_first_into_pot'})
		{
			if ($OHS{'randomround'} > 0.25)
			{
				if ($OHS{'nopponentsplaying'} < 4)
				{
					if (monster_draw() ||
						(onepair_str() == 5 && $OHS{'nsuitedcommon'} < 3 && $OHS{'nstraightfillcommon'} > 1) ||
						twopair_str() > 3 ||
						trips_str() > 1 ||
						( straight_str() >= 3 && straight_str() <= 4)) 
					{
						$fswag = round_to_sb(( 0.55 + 0.55 * OHS{'randomround'}) * $OHS{'pot'});
						send_DebugMsg("SWAG TURN: first to act on a hand or monster draw");
						goto pl_swag_done;
					}
				}
			}
		}
		
		if (!$OHS{'ac_first_into_pot'})
		{
			if (!$OHS{'didcallround3'})
			{
				if (!$OHS{'didswaground3'})
				{
					if (monster_hand() ||
						flush_str() > 2 ||
						trips_str() > 2 ||
						straight_str() > 3 ||
						twopair_str() > 3 ||
						(onepair_str() > 4 && $OHS{'randomround'} > 0.5))		   
					{
						$fswag = round_to_sb(( 0.35 + 0.55 * OHS{'randomround'}) * $OHS{'pot'});
						send_DebugMsg("SWAG TURN: rerasing on a strong hand or monster");
						goto pl_swag_done;
					}
				}
			}
		}
		
		# reraising on list 100
		
		if ( !$OHS{'ac_first_into_pot'})
		{
			if ( $OHS{'islist100'})
			{
				if ($OHS{'rankhiplayer'} > $OHS{'rankhicommon'})
				{
					if ( $OHS{'nsuitedcommon'} < 3)
					{
						if ($OHS{'nstraightcommon'} < 3)
						{
							if (!$OHS{'didcallround3'})
							{
								if (!$OHS{'didswaground3'})
								{
									$fswag = round_to_sb(( 1.8 + 0.8 * $OHS{'randomround'}) * $OHS{'call'});
									send_DebugMsg("SWAG TURN: non-flushy, less than 3 to a straight, no Aces board, islist100: ", $fswag);
									goto pl_swag_done;
								}
							}
						}
					}
				}
			}
		}
	} #END TURN

	if ( $OHS{'br'} == RIVER) 
	{
		$fswag = FALSE;
		
		if ($OHS{'ac_first_into_pot'})
		{
			if (made_hand())
			{
				if ($OHS{'ishandup'})
				{
					if ($OHS{'balance'} > $OHS{'pot'})
					{
					$fswag = round_to_sb(( 0.4 + 0.55 * $OHS{'randomround'}) * $OHS{'potcommon'});
					send_DebugMsg("SWAG RIVER: first to act hand improved on the river: $fswag");
					goto pl_swag_done;
					}
				}
			}
		}
			
		if ($OHS{'ac_first_into_pot'})
		{
			if ($OHS{'nopponentsplaying'} == 1)
			{
				if (bluff_flush())
				{
					if (! riskofruin())
					{
						if ($OHS{'randomround'} > 0.71)
						{
							if ($OHS{'didcallround2'} || $OHS{'didcallround3'})				
								{
									$fswag = round_to_sb(( 0.55 + 0.55 * $OHS{'randomround'}) * $OHS{'pot'});
									send_DebugMsg("SWAG RIVER: bluffing the flush: $fswag");
									goto pl_swag_done;
								}
						}
					}
				}
			}
		}
		
		if ($OHS{'ac_first_into_pot'} && monster_hand() && $OHS{'randomround'} > 0.15)
		{
			$fswag = round_to_sb(( 0.28 + 0.55 * $OHS{'randomround'}) * $OHS{'pot'});
			send_DebugMsg("SWAG RIVER: monster: $fswag");
			goto pl_swag_done;
		}
		
		if ($OHS{'ac_first_into_pot'})
		{
			if (!OHS{'didcallround4'})
			{
				if (!$OHS{'didswaground4'})
				{
					if ( twopair_str() > 2 ||
					( trips_str() > 1 && $OHS{'randomround'} > 0.25) ||
					(twopair_str == 5 && $OHS{'nsuitedcommon'} < 3 && $OHS{'nstraightcommon'} < 4 && $OHS{'nopponentsplaying'} < 3))			
					{
						$fswag = round_to_sb(( 0.5 + 0.75 * $OHS{'randomround'}) * $OHS{'potcommon'});
						send_DebugMsg("SWAG RIVER: first to act and a strong hand or a monster: $fswag");
						goto pl_swag_done;
					}
				}
			}
		}
		
		# bet 4-flush
		if ( $OHS{'ac_first_into_pot'})
		{
			if ($OHS{'nsuitedcommon'} == 4)
			{
				if ($OHS{'nopponentsplaying'} < 3)
				{
					if (!$OHS{'didcallround3'} || !$OHS{'didswaground3'})
					{
						if ($OHS{'balance'} > $OHS{'potcommon'}) 
						{
							$fswag = round_to_sb((0.45 + 0.65 * $OHS{'randomround'}) * $OHS{'potcommon'});
							send_DebugMsg("SWAG RIVER: bet 4-flush: ", $fswag);
							goto pl_swag_done;
						}
					}
				}
			}
		}
		
		if ( monster_hand() && ! $OHS{'ac_first_into_pot'})
		{
			$fswag = round_to_sb(( 0.65 + 0.80 * $OHS{'randomround'}) * $OHS{'potcommon'});
			send_DebugMsg("SWAG RIVER: not first to act and monster hand: $fswag");
			goto pl_swag_done;
		}
				
	} # END RIVER
	
pl_swag_done:

	return $fswag;
}


# FUNCTION: call
sub pl_call
{
	my $fcall = 0;
	
	# call a high raise 50% tightly
	# (pocket hand has to be one better
	# than the position-wise raise list for the raising chair)
    if ( $OHS{'br'} == PREFLOP) 
	{
		if ($OHS{'call'} > 2 * $OHS{'bblind'})
		{
			if ($OHS{'balance'} >= 14 * $OHS{'bblind'} && $OHS {'randomround'} > 0.5)
			{
				if ( are_we_headsup() == TRUE) 
					{
						if ( $OHS{'islist102'} || 
							( am_i_dealer() == TRUE && $OHS{'islist101'}))
						{
							send_DebugMsg("CALL PREFLOP: calling on list102 or 101 when dealer headsup!");
							$fcall = TRUE;
							goto pl_call_done;
						}					 
					} 
					else 
					{
						if (( $OHS{'dealpositionrais'} == 1 && $OHS {'nopponentsplaying'} == 1 && $OHS{'islist102'}) ||
						( adjusted_dealposition() == 2 && $OHS{'islist105'}) ||
						( adjusted_dealposition() == 3 && $OHS{'islist100'}) ||
						( adjusted_dealposition() == 4 && $OHS{'islist103'}) ||
						( adjusted_dealposition() == 5 && $OHS{'islist104'}) ||
						( adjusted_dealposition() == 6 && $OHS{'islist105'}) ||
						( adjusted_dealposition() == 7 && $OHS{'islist106'}) ||
						( adjusted_dealposition() == 8 && $OHS{'islist107'}) ||
						( adjusted_dealposition() == 9 && $OHS{'islist108'}))
						{
							send_DebugMsg("CALL PREFLOP: calling according to list/position!");
							$fcall = TRUE;
							goto pl_call_done;
						}
					}
			}
		}
		
		if ( $OHS{'call'} <= 2 * $OHS{'bblind'} && $OHS{'balance'} >= 14 * $OHS{'bblind'}) 
		{
			if ( are_we_headsup() == TRUE) 
			{
				if ( $OHS{'islist102'} || 
					( am_i_dealer() == TRUE && $OHS{'islist101'}) )
				{
					send_DebugMsg("CALL PREFLOP: calling on list102 or 101 when dealer headsup to call <=2bb and balance >= 14bb");
					$fcall = TRUE;
					goto pl_call_done;
				}					 
			} 
			else 
			{
				if ($OHS{'islist109'} && odds_to_call())
				{
					send_DebugMsg("CALL PREFLOP: calling because to call <=2bb and balance >= 14bb");
					$fcall = TRUE;
					goto pl_call_done;
				}
			}
		}
		
		# call short stacks shoves when our stack is 30bb+ on AA, KK, QQ, JJ, TT, 99, 88, AKs, AK, AQs, AQ, AJs, AJ, ATs, AT, A9s, A8s, KQs, KJs
		if ( $OHS{'call'} <= 9 * $OHS{'bblind'} && $OHS{'balance'} >= 30 * $OHS{'bblind'} && $OHS{'islist111'} && max(pot_commitment()) >= 0.4)
		{
			send_DebugMsg("CALL PREFLOP: calling because to call <=9bb and balance >= 30bb and list111");
			$fcall = TRUE;
			goto pl_call_done;
		}
		
		if ( sc_or_Axs_w_odds() == TRUE || small_pp_w_odds() == TRUE) 
		{
			send_DebugMsg("CALL PREFLOP: odds to call on sc, Axs, or small pp in position");
			$fcall = TRUE;
			goto pl_call_done;
		}
		
		if ( ($OHS{'didswaground1'} || $OHS{'didcallround1'}) && # XXX: must use pot not potcommon as we are preflop
				($OHS{'call'} < 3 * $OHS{'bblind'} || $OHS{'call'} < 0.3 * $OHS{'pot'}|| max(pot_commitment()) >= 0.4))
		 {
			$fcall = TRUE;
			send_DebugMsg("CALL PREFLOP: got reraised, opponent committed or odds to call");
			goto pl_call_done;
		}
		
		if ( ($OHS{'didswaground1'} || $OHS{'didcallround1'}) && # calling because > 60% of chips invested
				our_chips_invested() > 0.6)
		 {
			$fcall = TRUE;
			send_DebugMsg("CALL PREFLOP: got reraised, committed");
			goto pl_call_done;
		}
		
		if ($OHS{'dealposition'} == 2 && max(pot_commitment()) == 1 && $OHS{'call'} < 2 * $OHS{'bblind'} && $OHS{'nplayersactive'} == 1)
		
		{	$fcall = TRUE;
			send_DebugMsg("CALL PREFLOP: got reraised, committed");
			goto pl_call_done;
		}
	}
	
	if ( $OHS{'br'} == FLOP) 
	{
		$fcall = 0;
		
		if  ( monster_hand() || 
				( made_hand() && $OHS{'call'} <= 0.5 * $OHS{'potcommon'} && $OHS{'call'} <= 0.1 * $OHS{'balance'}) ||
				trips_str() > 2 ||
				$OHS{'isflush'} ||
				$OHS{'isstraight'} ||
				( monster_draw() && odds_to_call() == TRUE && ( $OHS{'balance'} - $OHS{'call'}) > 0.7 * $OHS{'balance'}) ||
				( $OHS{'nopponentsplaying'} == 1  && $OHS{'islist100'}))
		{
			$fcall = TRUE;
			send_DebugMsg("CALL FLOP: monster or made hand with odds to call");
			goto pl_call_done;
		}
		
		
		if ( ( $OHS{'nstraight'} == 4 || flush_draw()) && odds_to_call())
			{
				$fcall = TRUE;
				send_DebugMsg("CALL FLOP: got reraised, holding a straight or flush draw w/odds to call and large stack");
				goto pl_call_done;
			}
			
			if ( $OHS{'randomround'} > 0.5)
			{
							
				if ( (made_hand()|| monster_draw()) && $OHS{'call'} < 0.3 * $OHS{'potcommon'})
				{
					$fcall = TRUE;
					send_DebugMsg("CALL FLOP: call < 30% of pot and random > 0.5, monster draw or made hand");
					goto pl_call_done;
				}
				
				if ( $OHS{'nopponentsplaying'} == 1 && (made_hand() || monster_draw()) && max(pot_commitment()) >= 0.4)
				{
					$fcall = TRUE;
					send_DebugMsg("CALL FLOP: opponent committed and made hand or monster draw");
					goto pl_call_done;
				}
			}
		
		
		if ( ($OHS{'didswaground2'} || $OHS{'didcallround2'}) && # calling because > 60% of chips invested
				our_chips_invested() > 0.6)
		 {
			$fcall = TRUE;
			send_DebugMsg("CALL FLOP: got reraised, committed");
			goto pl_call_done;
		}
		
	} #END FLOP
	
	if ( $OHS{'br'} == TURN) 
	{
		$fcall = 0;

		if ( (monster_draw() || flush_draw()) && odds_to_call() == TRUE)
		{
			$fcall = TRUE;
			send_DebugMsg("CALL TURN: monster draw w/odds to call and < 1/3 of stack to call");
			goto pl_call_done;
		}
		
		if ( $OHS{'call'} < 0.7 * $OHS{'potcommon'} && $OHS{'nopponentsplaying'} == 1 && 
			( isoverpair() || TPTK()) && $OHS{'nsuitedcommon'} < 3) 
		{
			$fcall = TRUE;
			send_DebugMsg("CALL TURN: < 0.7 potcommon to call vs 1, have an overpair or TPTK with 2 or less suited on board");
			goto pl_call_done;
		}
		
		if ( monster_hand())
		{
			$fcall = TRUE;
			send_DebugMsg("CALL TURN: monster hand");
			goto pl_call_done;
		}
		
		if ( $OHS{'call'} < 0.7 * $OHS{'potcommon'} && onepair_str() > 3)
		{
			$fcall = TRUE;
			send_DebugMsg("CALL TURN: to call < 70% of potcommon && one pair with strength > 3");
			goto pl_call_done;
		}
		
		if ( flush_draw() == TRUE && odds_to_call() && ( $OHS{'balance'} - $OHS{'call'}) > 0.7 * $OHS{'balance'})
		{
			$fcall = TRUE;
			send_DebugMsg("CALL TURN: to call < 30% of potcommon && have a flush draw");
			goto pl_call_done;
		}
		
		if ( twopair_str() > 3 )
		{
			$fcall = TRUE;
			send_DebugMsg("CALL TURN: two pair with strength > 2 (calling because there is a chance to improve on the river)");
			goto pl_call_done;
		}
		
		if ( trips_str() > 3)
		{
			$fcall = TRUE;
			send_DebugMsg("CALL TURN: three of a kind");
			goto pl_call_done;
		}
		
		if ( made_hand() && ($OHS{'call'} < 3 * $OHS{'bblind'} || $OHS{'call'} < 0.3 * $OHS{'potcommon'}))
		{
			$fcall = TRUE;
			send_DebugMsg("CALL TURN: raised or called in current round and 3bb or <30% of potcommon to call");
			goto pl_call_done;
		}
		
		if ( $OHS{'didswaground3'} || $OHS{'didcallround3'})
		{
			if (( monster_draw() || $OHS{'nstraight'} == 4 || flush_draw()) && odds_to_call())
			{
				$fcall = TRUE;
				send_DebugMsg("CALL TURN: got reraised, a good draw with odds to call");
				goto pl_call_done;
			}
			
			if ( (made_hand()||monster_draw()) && ($OHS{'call'} < 3 * $OHS{'bblind'} || $OHS{'call'} < 0.3 * $OHS{'potcommon'} || max(pot_commitment()) >= 0.4))
			{
				$fcall = TRUE;
				send_DebugMsg("CALL TURN: got reraised, opponent committed or odds to call, have made hand or monster draw");
				goto pl_call_done;
			}
		}
		
				
		if ( ( made_hand()|| monster_draw()) && ($OHS{'didswaground3'} || $OHS{'didcallround3'}) && # calling because > 60% of chips invested
				our_chips_invested() > 0.6)
		{
			$fcall = TRUE;
			send_DebugMsg("CALL TURN: got reraised, committed");
			goto pl_call_done;
		}
		
		# calling short stack allin when our hand has a 60% chance of winning
		if ({'call'} < 0.25 * $OHS{'potcommon'})
		{
				if ($OHS{'call'} < 0.25 * $OHS {'balance'})
			{
					if (($OHS{'nhandslo'} - $OHS{'nhandshi'}) > 75)
				{
						if (max(pot_commitment()) == 1)
					{
						$fcall = TRUE;
						send_DebugMsg("CALL TURN: made hand, opponent allin and less than 25* of pot to call");
						goto pl_call_done;
					}
				}
			}
		}		
	
		if (our_chips_invested() > 0.7)
		{
			if (($OHS{'nhandslo'} - $OHS{'nhandshi'}) > 150)
			{
				$fcall = TRUE;
				send_DebugMsg ("CALL TURN: nhandslo - nhandshi > 250 and 70 % of chips already invested");
				goto pl_call_done;
			}
		}
		
		if (our_chips_invested() > 0.9)
		{
			if (made_hand())
			{
				$fcall = TRUE;
				send_DebugMsg ("CALL TURN: nhandslo > nhandshi  and 90 % of chips already invested");
				goto pl_call_done;
			}
		}
				
		if ($OHS{'nopponentsplaying'} == 1)
		{
			if (($OHS{'nhandslo'} - $OHS{'nhandshi'}) > 100)
			{
				if ($OHS{'call'} < 0.4 * $OHS{'balance'})
				{
					if ($OHS{'call'} < $OHS{'potcommon'})
					{
					$fcall = TRUE;
					send_DebugMsg("CALL TURN: for less than half remaining stack, chance of winning > 70%");
					goto pl_call_done;
					}
				}
			}
		}
			
	} #END TURN
	
	if ( $OHS{'br'} == RIVER) 
	{
		$fcall = 0;	
		
		if ( (made_hand()|| $OHS{'rankhiplayer'} == 14) && ($OHS{'call'} < $OHS{'balance'}))
			{
				if( pot_equity_river()** $OHS{'nopponentsactive'} >= $OHS{'call'}/$OHS{'potcommon'})
					{
					$fcall = TRUE;
					send_DebugMsg("CALL RIVER: pot equity is >= amount to call");
					goto pl_call_done;
					}
			}
		
		if  ( isoverpair() || TPTK())
		{
			if ($OHS{'nsuitedcommon'} < 3)
			{
				if ($OHS{'call'} < $OHS{'pot'})
				{
					if ($OHS{'call'} < 0.6 * $OHS{'balance'})
					{
					$fcall = TRUE;
					send_DebugMsg("CALL RIVER: overpair or TPTK on non-flushy board && call < potcommon and call <60% of balance");
					goto pl_call_done;
					}
				}
			}
		}
		
		if ( monster_hand() == TRUE)
		{
			$fcall = TRUE;
			send_DebugMsg("CALL RIVER: on monster hand");
			goto pl_call_done;	
		}
		
		if ($OHS{'call'} < 0.3 * $OHS{'pot'})
		{
			if (made_hand())
			{
				if ($OHS{'call'} < 0.3 * $OHS{'balance'})
				{
					if (!$OHS{'didswaground4'} || !$OHS{'didcallround4'})
					{
						$fcall = TRUE;
						send_DebugMsg("CALL RIVER: call on made hand < 30% of pot when not reraised in current round");
						goto pl_call_done;
					}
				}
			}
		}
						
		if ( twopair_str() > 3 && !$OHS{'didswaground4'})
		{
			$fcall = TRUE;
			send_DebugMsg("CALL RIVER: two pair with with strength > 3");
			goto pl_call_done;
		}
		
		if ( trips_str() > 1 && $OHS{'call'} < $OHS{'potcommon'})
		{
			$fcall = TRUE;
			send_DebugMsg("CALL RIVER: three of a kind with less then a pot sized bet");
			goto pl_call_done;
		}
		
		if ( flush_str() && (!$OHS{'didswaground4'} || !$OHS{'didcallround4'}) && $OHS{'call'} < $OHS{'potcommon'})
		{
			$fcall = TRUE;
			send_DebugMsg("CALL RIVER: flush strength > 0 and did not get reraised in current round,  call < potcommon");
			goto pl_call_done;
		}
		
		if ( $OHS{'isstraight'} && $OHS{'nsuitedcommon'} < 4 && $OHS{'call'} < $OHS{'potcommon'})
		{
			$fcall = TRUE;
			send_DebugMsg("CALL RIVER: straight on a non-flushy board and call < potcommon");
			goto pl_call_done;
		}
		
		if ( $OHS{'isfullhouse'} && (!$OHS{'didswaground4'} || !$OHS{'didcallround4'}) && $OHS{'call'} < $OHS{'potcommon'})
		{
			$fcall = TRUE;
			send_DebugMsg("CALL RIVER: full house, did not get reraised in current round, and < pot sized bet");
			goto pl_call_done;
		}
		
						
		if (our_chips_invested() > 0.7)
		{
			if (($OHS{'nhandslo'} - $OHS{'nhandshi'}) > 200)
			{
				$fcall = TRUE;
				send_DebugMsg ("CALL RIVER: nhandslo - nhandshi > 250 and 70 % of chips already invested");
				goto pl_call_done;
			}
		}
		
		if (our_chips_invested() > 0.9)
		{
			if ($OHS{'nhandslo'} > $OHS{'nhandshi'})
			{
				$fcall = TRUE;
				send_DebugMsg ("CALL RIVER: nhandslo > nhandshi  and 90 % of chips already invested");
				goto pl_call_done;
			}
		}
			
		if ($OHS{'nhands'} == $OHS{'nhandsti'})
		{
			$fcall = TRUE;
			send_DebugMsg("CALL RIVER: the board is the nuts");
			goto pl_call_done;
		}
			
	} #END RIVER

pl_call_done:

	return $fcall;
}

# prefold in the following cases
sub pl_prefold
{
	# do not prefold heads up
	if ( are_we_headsup() == TRUE)
	{
		return FALSE;
	}
	
	if ( $OHS{'br'} == PREFLOP)
	{
		# prefold everything not in list101 when > 3bb - on 3 bb shove ATC when first to act
		if ( $OHS{'islist101'} == FALSE && !$OHS{'dealposition'} == 2 && $OHS{'balance'} >  3 * $OHS{'bb;ind'})
		{
			return TRUE;
		}
	}
	
	if ( $OHS{'br'} == FLOP)
	{
		if (!$OHS{'ac_first_into_pot'})
		{
			if ($OHS{'ispair'})
			{
				if ($OHS{'nopponentsplaying'} > 1)
				{
					if ($OHS{'rankhiplayer'} < 10)
					{
						if ($OHS{'rankhiplayer'} < $OHS{'rankhicommon'})
						{
							if (!$OHS{'isthreeofakind'} ||!monster_hand()||!monster_draw())
							{
								send_DebugMsg ("FLOP: folding small pair to action vs many opp, did not hit a set");
								return TRUE;
							}
						}
					}
				}
			}
		}
		return 0;
	}
	
	if ( $OHS{'br'} == TURN)
	{
	
	}
	
	if ( $OHS{'br'} == RIVER)
	{
	
	}
	
	# do not prefold by default
	return 0;
}

# tanked
# delay min of 10 seconds up to 15 seconds
sub tanked
{
	return 10000 + $OHS{'randomround'} * 5000;
}

# long delay
# 5-10 sec
sub long_delay
{
	return 5000 + $OHS{'randomround'} * 5000;
}

# medium delay
# 2-5 sec
sub medimum_delay
{
	return 2000 + $OHS{'randomround'} * 3000;
}

# short delay
# 1-3 sec
sub short_delay
{
	return 1000 + $OHS{'randomround'} * 2000;
}

# automatic decision
sub automatic_decision
{
	return 200 + $OHS{'randomround'} * 1328;
}

# snap call/shove with a fist pump
sub snapfistpump
{
	return 0;
}
	
# delay in the following cases
sub pl_delay
{
	# cache results of main functions
	# for performance reasons - we want to call these only once
	my $allin = pl_alli();
	my $call = pl_call();
	my $swag = pl_swag();
	
	if ( $OHS{'br'} == PREFLOP)
	{
		if ( $allin == TRUE || $swag == TRUE) 
		{
			# if headsup - fake automatic decision preflop
			if ( are_we_headsup() == TRUE)
			{
				return automatic_decision(); 
			}
			else # not headsup
			{
				# if not UTG - fake automatic decision
				if ( $OHS{'dealposition'} != 2)
				{
					return automatic_decision();
				}
				
				# we are UTG and have to think a little
				return short_delay();
			}
		}
	}
	
	if ( $OHS{'br'} == FLOP)
	{
		return automatic_decision();
	}
	
	if ( $OHS{'br'} == TURN)
	{
		if ( $call > 0 && monster_hand())
		{
			return long_delay();
		}
	}
	
	if ( $OHS{'br'} == RIVER)
	{
		if ( $call > 0 && monster_hand())
		{
			return snapfistpump();
		}
	}
	
	# pretend as automatic decision was made by default
	return automatic_decision();
}