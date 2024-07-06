#******************************************************************************
#
# This file is part of the OpenHoldem project
#   Download page:         http://code.google.com/p/openholdembot/
#   Forums:                http://www.maxinmontreal.com/forums/index.php
#   Licensed under GPL v3: http://www.gnu.org/licenses/gpl.html
#
#******************************************************************************
#
# PURPOSE: Converts Shanky-PPL to OpenPPL 7.x
#
#   for demonstration purposes and as a starting point for beginners.
#
#******************************************************************************
#
# PREREQUISITES: ActiveState Perl installed
#
# USAGE: at the DOS-prompt type
#   perl convert.pl < MYPROFILE.TXT > MYPROFILE.OHF
#
# RESULT: an OpenHoldem formula file with the changes described here:
#
#******************************************************************************
#
# LIMITATIONS: 
#   * does not delete multi-line beep and sitout commands
#   * does not support line-breaks inside hand and board expressions
#
#   But due to some very creatively formatted profiles we created anoither script
#     linebreak_correction_for_non_stqndard_formatted_profiles.pl
#
#******************************************************************************

use strict;
use warnings;
use diagnostics;

my $needs_indentation;
my $line_of_code;

print "##f\$notes##\n";
print "//\n";
print "// Converted to OpenPPL 7.x by convert.pl\n";
print "//\n";

# While there is input: process it line by line
while (<>)
{
    $line_of_code++;
	# Print comments (full line only) as is, without any processing
	if (m/^\/\//)
	{
		print;
		next;
	}
	# Ignore Option-settings
	if (m/MaxSessionHands =/ig
		|| m/MaxSessionTime = /ig
		|| m/MaxStackSize =/ig
		|| m/BathroomBreakAfter =/ig
		|| m/BathroomBreakTime =/ig
		|| m/SitOutWhenOpponentsAreLessThan =/ig
		|| m/MakePotSizedPreFlopRaisesWhen =/ig
		|| m/FoldToPre-FlopRaisesForAQAJsKQ =/ig
		|| m/FoldPost-FlopBelowTopPairToAnyBetOrRaise =/ig
		|| m/FoldToPost-FlopRaisesWithUnpairedBoardsFor =/ig
		|| m/FoldToPost-FlopRaisesWithPairedBoardsFor =/ig
		|| m/NeverBetTurnOrRiverWithHandBelow =/ig
		|| m/NeverBetFlopWithHandBelowMiddlePairWhen =/ig
		|| m/NeverBetFlopWithHandBelowMiddlePairStackSize =/ig
		|| m/NeverCallPairedBoardsWithHandBelow =/ig
		|| m/NeverCallFlushPossibleBoardsWithHandBelow =/ig
		|| m/FoldToPre-FlopRaiseWith55Or66When =/ig
		|| m/FoldToPre-FlopRaiseWith77Or88When =/ig
		|| m/FoldToPre-FlopRaiseWith99OrTTWhen =/ig
		|| m/FoldToPre-FlopRaiseWithJJOrQQWhen =/ig
		|| m/FoldToPre-FlopRaiseWithAKWhen =/ig
		|| m/FoldToPre-FlopRaiseWithAQsWhen =/ig
		|| m/FoldToPre-FlopRaiseWithAKsWhen =/ig
		|| m/FoldToPre-FlopRaiseWithKQsWhen =/ig
		|| m/SuperTightPlay =/ig
		|| m/SemiSuperTightPlay =/ig
		|| m/NoLimp =/ig
		|| m/AggressivePreFlop =/ig
		|| m/AggressivePostFlop =/ig
		|| m/AggressivePreFlopStackSize =/ig
		|| m/AggressivePostFlopStackSize =/ig
		|| m/MiningForSets =/ig
		|| m/PostFlopinBlindsOverride =/ig 
		|| m/RaiseMinOverride =/ig
		|| m/PreFlopReRaiseOverride =/ig
		|| m/MinBetOverride =/ig
		|| m/FoldOrGoAllInWhenOpponentsAfterFoldsLessThan =/ig
		|| m/FoldOrGoAllInWhenOpponentsAfterFoldsLessThanStackSize =/ig
		|| m/ApplyFoldOrAllInAfter =/ig
		|| m/FoldOrGoAllInWhenOpponentsAtTableLessThan =/ig
		|| m/FoldOrGoAllInWhenOpponentsAtTableLessThanStackSize =/ig
		|| m/AlwaysCallWhenOpponentsRaiseIsLessThan =/ig
		|| m/AlwaysCallWhenStackSizeIsLessThanTotalInvested =/ig
		|| m/Verbose =/ig
		|| m/AutoPlay =/ig
		|| m/UserDefinedGameType =/ig
		|| m/UseOcrAtFullTilt =/ig
		|| m/AutoRebuyAtUB =/ig
		|| m/AlwaysAskForTableName =/ig
		|| m/EnableFoldInTurn =/ig 
		|| m/DefaultBetSizeFlop =/ig
		|| m/DefaultBetSizeTurn =/ig
		|| m/DefaultBetSizeRiver =/ig
		|| m/MoveMouseBeforeClickingAtFT =/ig
		|| m/WindowLeft =/ig
		|| m/WindowRight =/ig
		|| m/WindowBottom =/ig
		|| m/WindowTop =/ig)
	{
		# No processing, skip it silently.
		next;
	}
	# Remove keyword custom
	if (m/^custom/ig) 
	{
		# No processing, skip it silently.
		next;
	}
	# Have a look for code-snippets of Doodle -- Doodle is copyrighted.
	# Testing 10 different lines of code
	if (m/When Have2ndTopPair  and HaveFlushDraw  and bets = 1  and raises = 0  and StackSize > 35 and \(not onecardflushpossible\)/
		|| m/When Amounttocall < 26% PotSize  and \( HavePair  or hand = A  or Overcards = 2 \)  and not \( OneCardStraightPossible  or OneCardFlushPossible \) call force/
		|| m/When opponents = 1  and HaveOverPair  and \( hand = J J  or hand = Q Q  or hand = K K  or hand = A A \)  and bets = 1  and raises = 0  and not \( PairOnBoard  or FlushPossible \)  and BetSize >= 40% PotSize  and BetSize < 55% PotSize and not \(onecardstraightpossible and raisesonflop and betsize > 16\) and not \(onecardstraightpossible and betsize > 22\) call force/
		|| m/When \( opponents >= 4  or folds >= 3 \)  and not \( bets = 1  and raises = 0  and AmountToCall < 25% PotSize \)  and not \( HaveTopPair  or HaveOverPair  or HaveTwoPair  or HaveSet  or HaveTrips  or HaveFullHouse  or HaveQuads  or HaveStraight  or HaveFlush  or HaveStraightDraw  or HaveFlushDraw \) fold force/
		|| m/When raises = 1  and calls = 0  and BetSize <= 5  and \( LastRaiserPosition = 1  or LastRaiserPosition = 2  or LastRaiserPosition = 3 \)  and \( hand = Ad  or hand = Qd \) RaisePot force/
		|| m/When bets = 1  and raises = 0  and AmountToCall > 50% PotSize  and \( board = A A  or board = K K  or board = Q Q  or board = J J  or board = T T  or board = 9 9  or board = 8 8 \) call force/
		|| m/when onecardflushpossible and haveflush and not \(havenutflush or have2ndnutflush or have3rdnutflush\) and amounttocall < 15% potsize call forcee/
		|| m/When AmountToCall < 37% PotSize  and HaveFlushDraw  and not \( FlushPossible  or HaveOverPair  or HaveTopPair  or HaveTwoPair  or HaveSet  or HaveTrips \) call force/
		|| m/When BotIsLastRaiser  and StackSize > 20  and bets = 0  and raises = 0  and not \( FlushPossible  or opponents > 2 \)  and not \( board = A  and StraightPossible \)  and not ( board = K  and StraightPossible )  and not \( board = Q J T  or board = J T 9  or board = T 9 8  or board = 9 8 7  or board = 8 7 6 \) BetPot force/
		|| m/When \( BotsLastAction = raise  or BotsLastAction = call \)  and raises = 1  and \(AmountToCall > 25% StackSize or AmountToCall > 52% potsize or AmountTocall > 8\)  and not \( opponents = 1  and OpponentIsAllIn \) fold force/)
	{
		print STDERR "Error in line $line_of_code\n";
		print STDERR "This profile seems to contain copyrighted Doodle-code.\n";
		print STDERR "Can't be included in the OpenHoldem archive.\n";
		next;
	}
	
	# Translate preflop, flop turn and river into function headers.
	#
	# Substitution is case-insensitive ("i" at the end), 
	# but we care about beautyful, readable output and "fix" the cases.
	s/^PREFLOP/\#\#f\$preflop\#\#/i;
	s/^FLOP/\#\#f\$flop\#\#/i;
	s/^TURN/\#\#f\$turn\#\#/i;
	s/^RIVER/\#\#f\$river\#\#/i;
	# The parser does not expect comments at the same line like function headers,
	# therefor we shouldd introduce a new-line in this case.
	if (m/^##/)
	{
		s/\/\//\n\/\//;
	}
	# Now replace tabs by 4 spaces to make life more easy
	s/\t/    /g;
	# Add spaces after WHEN and before FORCE
	# But only at the beginning and end of a line.
	# These words might also be used in unser-variables
	s/^when/WHEN /ig;
	s/force$/ FORCE/ig;
	# Add spaces before/after brackets
	s/\(/ \(/g;
	s/\)/\) /g;
	# Remove all superfluous blanks.
	# We will care about indentation later
	s/    / /ig;
	s/   / /ig;
	s/  / /ig;
	# Remove Shanky-style delay
	s/delay([ ]*(\d)+)//ig;
	# Sitout and Beep not supported
	if (m/SitOut FORCE/ig || m/Beep FORCE/ig) 
	{
	    if (m/WHEN/)
		{
			# Single-line-statement.
			# Just skip it.
			next;
		}
		# Multi-line-statement
		# Print a warning at stderr
		print STDERR "Error in line $line_of_code\n";
		print STDERR "Unsupported Beep or SitOut action in multi-line-statement.\n";
		print STDERR "Can't fix that automatically -- please remove it manually.\n";
		next;
	}
	# Partial hand or board expression at the end of a line. Can't be handled
	if (((m/hand(\s)*$/i) || (m/hand(\s)*=(\s)*$/i) || (m/board$/i) || (m/board(\s)*=(\s)*$/i)) 
	    # But avoid false positives (regular variables)
	    && !m/onboard$/i && !m/toboard$/i && !m/inhand/i
		# Also avoid false positives by comments
		&& !m/\/\//
		# Also avoid false positives by user-variables that end on "hand" or "board"
		&& !m/user(\w+)$/i)
	{
		print STDERR "Error in line $line_of_code\n";
		print STDERR "Partial hand or board expression at the end of a line.\n";
		print STDERR "Can't handle that automatically -- please fix it manually.\n";
		next;
	}
	# Easy to read cases for all built-in symbols.
	# E.g. "nobettingonturn" should be spelled "NoBettingOnTurn", etc.
	#
	# Some special ones first, as they are part of other symbols and could break them later.
	# e.g Opponents breaks MaxOpponentStacksize
	s/None/None/ig;
	s/Opponents/Opponents/ig;
	s/Bets/Bets/ig;
	# 5) Other symbols
	s/IsFinalTable/IsFinalTable/ig;
	s/Others/Others/ig;
	s/Random/Random/ig;
	# 1) BettingAction symbols
	s/BotCalledBeforeFlop/BotCalledBeforeFlop/ig;
	s/BotIsLastRaiser/BotIsLastRaiser/ig;
	s/BotRaisedBeforeFlop/BotRaisedBeforeFlop/ig;
	s/BotRaisedOnFlop/BotRaisedOnFlop/ig;
	s/BotRaisedOnTurn/BotRaisedOnTurn/ig;
	s/BotsActionsOnFlop/BotsActionsOnFlop/ig;
	s/BotsActionsOnThisRound/BotsActionsOnThisRound/ig;
	s/BotsActionsOnThisRoundIncludingChecks/BotsActionsOnThisRoundIncludingChecks/ig;
	s/BotsActionsPreflop/BotsActionsPreflop/ig;
	s/BotsLastAction/BotsLastAction/ig;
	s/BotsLastPreflopAction/BotsLastPreflopAction/ig;
	s/CalledOnFlop/CalledOnFlop/ig;
	s/CalledOnTurn/CalledOnTurn/ig;
	s/Calls/Calls/ig;
	s/CallsSinceLastRaise/CallsSinceLastRaise/ig;
	s/Checks/Checks/ig;
	s/Folds/Folds/ig;
	s/NoBettingOnFlop/NoBettingOnFlop/ig;
	s/NoBettingOnTurn/NoBettingOnTurn/ig;
	s/NumberOfRaisesBeforeFlop/NumberOfRaisesBeforeFlop/ig;
	s/NumberOfRaisesOnFlop/NumberOfRaisesOnFlop/ig;
	s/NumberOfRaisesOnTurn/NumberOfRaisesOnTurn/ig;
	s/OpponentCalledOnFlop/OpponentCalledOnFlop/ig;
	s/OpponentCalledOnTurn/OpponentCalledOnTurn/ig;
	s/OpponentIsAllin/OpponentIsAllin/ig;
	s/Raises/Raises/ig;
	s/RaisesBeforeFlop/RaisesBeforeFlop/ig;
	s/RaisesOnFlop/RaisesOnFlop/ig;
	s/RaisesOnTurn/RaisesOnTurn/ig;
	s/RaisesSinceLastPlay/RaisesSinceLastPlay/ig;
	# 6) Player symbols
	s/OpponentsAtTable/OpponentsAtTable/ig;
	s/OpponentsLeft/OpponentsLeft/ig;
	s/OpponentsOnFlop/OpponentsOnFlop/ig;
	s/OpponentsOnRiver/OpponentsOnRiver/ig;
	s/OpponentsOnTurn/OpponentsOnTurn/ig;
	s/OpponentsWithHigherStack/OpponentsWithHigherStack/ig;
	s/OpponentsWithLowerStack/OpponentsWithLowerStack/ig;
	s/StillToAct/StillToAct/ig;
	# 2) Betsizes and stacksizes
	s/AmountToCall/AmountToCall/ig;
	s/BetSize/BetSize/ig;
	s/BigBlindSize/BigBlindSize/ig;
	s/MaxOpponentStackSize/MaxOpponentStackSize/ig;
	s/MaxStillToActStackSize/MaxStillToActStackSize/ig;
	s/MinOpponentStackSize/MinOpponentStackSize/ig;
	s/MinStillToActStackSize/MinStillToActStackSize/ig;
	s/PotSize/PotSize/ig;
	s/StackSize/StackSize/ig;
	s/StartingStackSize/StartingStackSize/ig;
	s/TotalInvested/TotalInvested/ig;
	s/TotalInvestedThisRound/TotalInvestedThisRound/ig;
	# 3) Board symbols
	s/2ndTopFlopCardPairedOnRiver/2ndTopFlopCardPairedOnRiver/ig;
	s/2ndTopFlopCardPairedOnTurn/2ndTopFlopCardPairedOnTurn/ig;
	s/AcePresentOnFlop/AcePresentOnFlop/ig;
	s/FlushOnBoard/FlushOnBoard/ig;
	s/FlushPossible/FlushPossible/ig;
	s/FlushPossibleOnFlop/FlushPossibleOnFlop/ig;
	s/FlushPossibleOnTurn/FlushPossibleOnTurn/ig;
	s/FourCardsToWheelOnBoard/FourCardsToWheelOnBoard/ig;
	s/FourOf1SuitOnTurn/FourOfOneSuitOnTurn/ig;
	s/FourOf1SuiteOnTurn/FourOfOneSuitOnTurn/ig;
	s/FullHouseOnBoard/FullHouseOnBoard/ig;
	s/KingPresentOnFlop/KingPresentOnFlop/ig;
	s/LowCardsOnBoard/LowCardsOnBoard/ig;
	s/MoreThanOneStraightPossibleOnFlop/MoreThanOneStraightPossibleOnFlop/ig;
	s/MoreThanOneStraightPossibleOnTurn/MoreThanOneStraightPossibleOnTurn/ig;
	s/OneCardFlushPossible/OneCardFlushPossible/ig;
	s/OneCardStraightFlushPossible/OneCardStraightFlushPossible/ig;
	s/OneCardStraightPossible/OneCardStraightPossible/ig;
	s/OneCardStraightPossibleOnTurn/OneCardStraightPossibleOnTurn/ig;
	s/Only1OneCardStraightPossible/Only1OneCardStraightPossible/ig;
	s/OnlyOneStraightPossible/OnlyOneStraightPossible/ig;
	s/OvercardsOnBoard/OvercardsOnBoard/ig;
	s/PairOnBoard/PairOnBoard/ig;
	s/PairOnFlop/PairOnFlop/ig;
	s/PairOnTurn/PairOnTurn/ig;
	s/QuadsOnBoard/QuadsOnBoard/ig;
	s/QueenPresentOnFlop/QueenPresentOnFlop/ig;
	s/RankOfPairOnBoard/RankOfPairOnBoard/ig;
	s/RankOfSpareCardWhenTwoPairOnBoard/RankOfSpareCardWhenTwoPairOnBoard/ig;	
	s/RiverCardIsOvercardToBoard/RiverCardIsOvercardToBoard/ig;
	s/RunnerRunnerFlushPossibleAtRiver/RunnerRunnerFlushPossibleAtRiver/ig;
	s/SecondTopflopCardPairedOnRiver/SecondTopflopCardPairedOnRiver/ig;
	s/SecondTopflopCardPairedOnTurn/SecondTopflopCardPairedOnTurn/ig;
	s/StraightFlushOnBoard/StraightFlushOnBoard/ig;
	s/StraightFlushPossible/StraightFlushPossible/ig;
	s/StraightFlushPossibleByOthers/StraightFlushPossibleByOthers/ig;
	s/StraightOnBoard/StraightOnBoard/ig;
	s/StraightPossible/StraightPossible/ig;
	s/StraightPossibleOnFlop/StraightPossibleOnFlop/ig;
	s/StraightPossibleOnTurn/StraightPossibleOnTurn/ig;
	s/SuitsOnBoard/SuitsOnBoard/ig;
	s/SuitsOnFlop/SuitsOnFlop/ig;
	s/ThreeCardStraightOnBoard/ThreeCardStraightOnBoard/ig;
	s/TopFlopCardPairedonRiver/TopFlopCardPairedOnRiver/ig;
	s/TopFlopCardPairedonTurn/TopFlopCardPairedOnTurn/ig;
	s/TripsOnBoard/TripsOnBoard/ig;
	s/TripsOnBoardOnTurn/TripsOnBoardOnTurn/ig;
	s/TurnCardisOvercardToBoard/TurnCardIsOvercardToBoard/ig;
	s/TurnCardPaired/TurnCardPaired/ig;
	s/TwoOfOneSuitPresentOnFlop/TwoOfOneSuitPresentOnFlop/ig;
	s/TwoPairOnBoard/TwoPairOnBoard/ig;
	s/UncoordinatedFlop/UncoordinatedFlop/ig;
	s/WheelPossible/WheelPossible/ig;
	# 4) Handstrength symbols
	s/HadOverPairOnFlop/HadOverPairOnFlop/ig;
	s/HadPairOnFlop/HadPairOnFlop/ig;
	s/HadPairOnTurn/HadPairOnTurn/ig;
	s/HadTopPairOnFlop/HadTopPairOnFlop/ig;
	s/HadTopPairOnTurn/HadTopPairOnTurn/ig;
	s/HadTwoPairOnFlop/HadTwoPairOnFlop/ig;
	s/Have2ndBestKicker/Have2ndBestKicker/ig;
	s/Have2ndBestKickerOrBetter/Have2ndBestKickerOrBetter/ig;
	s/Have2ndBestOverPairOrBetter/Have2ndBestOverPairOrBetter/ig;
	s/Have2ndNutFlush/Have2ndNutFlush/ig;
	s/Have2ndNutFlushDraw/Have2ndNutFlushDraw/ig;
	s/Have2ndNutStraight/Have2ndNutStraight/ig;
	s/Have2ndOverPair/Have2ndOverPair/ig;
	s/Have2ndTopPair/Have2ndTopPair/ig;
	s/Have2ndTopSet/Have2ndTopSet/ig;
	s/Have3rdBestKicker/Have3rdBestKicker/ig;
	s/Have3rdBestKickerOrBetter/Have3rdBestKickerOrBetter/ig;
	s/Have3rdBestOverPairOrBetter/Have3rdBestOverPairOrBetter/ig;
	s/Have3rdNutFlush/Have3rdNutFlush/ig;
	s/Have3rdNutFlushDraw/Have3rdNutFlushDraw/ig;
	s/Have3rdOverPair/Have3rdOverPair/ig;
	s/Have3rdTopPair/Have3rdTopPair/ig;
	s/Have3rdTopSet/Have3rdTopSet/ig;
	s/Have4thNutFlush/Have4thNutFlush/ig;
	s/Have4thNutFlushDraw/Have4thNutFlushDraw/ig;
	s/Have4thOverPair/Have4thOverPair/ig;
	s/Have4thTopPair/Have4thTopPair/ig;
	s/Have4thTopSet/Have4thTopSet/ig;
	s/Have5thNutFlush/Have5thNutFlush/ig;
	s/Have5thNutFlushDraw/Have5thNutFlushDraw/ig;
	s/Have5thOverPair/Have5thOverPair/ig;
	s/HaveBackdoor2ndNutFlushDraw/HaveBackdoor2ndNutFlushDraw/ig;
	s/HaveBackdoor3rdNutFlushDraw/HaveBackdoor3rdNutFlushDraw/ig;
	s/HaveBackdoorFlushDraw/HaveBackdoorFlushDraw/ig;
	s/HaveBackdoorNutFlushDraw/HaveBackdoorNutFlushDraw/ig;
	s/HaveBackdoorStraightDraw/HaveBackdoorStraightDraw/ig;
	s/HaveBestKicker/HaveBestKicker/ig;
	s/HaveBestKickerOrBetter/HaveBestKickerOrBetter/ig;
	s/HaveBestOverPairOrBetter/HaveBestOverPairOrBetter/ig;
	s/HaveBottomPair/HaveBottomPair/ig;
	s/HaveBottomSet/HaveBottomSet/ig;
	s/HaveBottomTrips/HaveBottomTrips/ig;
	s/HaveBottomTwoPair/HaveBottomTwoPair/ig;
	s/HaveDoubleGutshotDraw/HaveDoubleGutshotDraw/ig;
	s/HaveFlush/HaveFlush/ig;
	s/HaveFlushDraw/HaveFlushDraw/ig;
	s/HaveFullHouse/HaveFullHouse/ig;
	s/HaveInsideNutStraightDraw/HaveInsideNutStraightDraw/ig;
	s/HaveInsideStraightDraw/HaveInsideStraightDraw/ig;
	s/HaveNothing/HaveNothing/ig;
	s/HaveNutFlush/HaveNutFlush/ig;
	s/HaveNutFlushCard/HaveNutFlushCard/ig;
	s/HaveNutFlushDraw/HaveNutFlushDraw/ig;
	s/HaveNuts/HaveNuts/ig;
	s/HaveNutStraight/HaveNutStraight/ig;
	s/HaveNutStraightDraw/HaveNutStraightDraw/ig;
	s/HaveNutStraightFlush/HaveNutStraightFlush/ig;
	s/HaveOpenEndedStraightDraw/HaveOpenEndedStraightDraw/ig;
	s/HaveOverPair/HaveOverPair/ig;
	s/HavePair/HavePair/ig;
	s/HaveQuads/HaveQuads/ig;
	s/HaveRunnerRunnerFlushAtRiver/HaveRunnerRunnerFlushAtRiver/ig;
	s/HaveSet/HaveSet/ig;
	s/HaveStraight/HaveStraight/ig;
	s/HaveStraightDraw/HaveStraightDraw/ig;
	s/HaveStraightFlush/HaveStraightFlush/ig;
	s/HaveTopNonBoardPairedPair/HaveTopNonBoardPairedPair/ig;
	s/HaveTopPair/HaveTopPair/ig;
	s/HaveTopSet/HaveTopSet/ig;
	s/HaveTopTrips/HaveTopTrips/ig;
	s/HaveTopTwoPair/HaveTopTwoPair/ig;
	s/HaveTrips/HaveTrips/ig;
	s/HaveTwoPair/HaveTwoPair/ig;
	s/HaveUnderPair/HaveUnderPair/ig;
	s/HaveUnderStraight/HaveUnderStraight/ig;
	s/KingPresentOnFlop/KingPresentOnFlop/ig;
	s/NutFullHouseOrFourOfAKind/NutFullHouseOrFourOfAKind/ig;
	s/Overcards/Overcards/ig;
	s/PairInHand/PairInHand/ig;
	s/SuitsInHand/SuitsInHand/ig;
	# 8) Position symbols
	s/FirstCallerPosition/FirstCallerPosition/ig;
	s/FirstRaiserPosition/FirstRaiserPosition/ig;
	s/LastCallerPosition/LastCallerPosition/ig;	
	s/LastRaiserPosition/LastRaiserPosition/ig;
	s/Position/Position/ig;
	s/First/First/ig;
	s/Middle/Middle/ig;
	s/Last/Last/ig;
	# Positional Symbols Correction (spaces)
	s/In SmallBlind/InSmallBlind/ig;
	s/In BigBlind/InBigBlind/ig;
	s/In Button/InButton/ig;
	s/In CutOff/InCutOff/ig;
	s/In EarlyPosition/InEarlyPosition/ig;
	s/In EarlyPosition1/InEarlyPosition1/ig;
	s/In EarlyPosition2/InEarlyPosition2/ig;
	s/In EarlyPosition3/InEarlyPosition3/ig;
	s/In LatePosition/InLatePosition/ig;
	s/In LatePosition1/InLatePosition1/ig;
	s/In LatePosition2/InLatePosition2/ig;
	s/In LatePosition3/InLatePosition3/ig;
	s/In MiddlePosition/InMiddlePosition/ig;
	s/In MiddlePosition1/InMiddlePosition1/ig;
	s/In MiddlePosition2/InMiddlePosition2/ig;
	s/In MiddlePosition3/InMiddlePosition3/ig;
	s/In TheBlinds/InTheBlinds/ig;
	s/In UTG/InUTG/ig;
	# Positional Symbols Correction (caces)
	s/InSmallBlind/InSmallBlind/ig;
	s/InBigBlind/InBigBlind/ig;
	s/InButton/InButton/ig;
	s/InCutOff/InCutOff/ig;
	s/InEarlyPosition/InEarlyPosition/ig;
	s/InEarlyPosition1/InEarlyPosition1/ig;
	s/InEarlyPosition2/InEarlyPosition2/ig;
	s/InEarlyPosition3/InEarlyPosition3/ig;
	s/InLatePosition/InLatePosition/ig;
	s/InLatePosition1/InLatePosition1/ig;
	s/InLatePosition2/InLatePosition2/ig;
	s/InLatePosition3/InLatePosition3/ig;
	s/InMiddlePosition/InMiddlePosition/ig;
	s/InMiddlePosition1/InMiddlePosition1/ig;
	s/InMiddlePosition2/InMiddlePosition2/ig;
	s/InMiddlePosition3/InMiddlePosition3/ig;
	s/InTheBlinds/InTheBlinds/ig;
	s/InUTG/InUTG/ig;
	# Fix renamed symbols (consistency)
	s/ CalledOn/ BotCalledOn/ig;
	s/\(CalledOn/\(BotCalledOn/ig;
	# Use OpenPPL-style hand and board expressions
	{
		# Match spaces, ranks and suits.
		# Rip in in pieces, then put everything together again, but without spaces.
		# Some sub-expressions might be empty, we can ignore that safely.
	    no warnings 'uninitialized';
		s/hand(\s*)=(\s*)([AKQJT]|\d)?(\s*)([cdhs])?(\s*)([AKQJT]|\d)?(\s*)([cdhs])?/hand\$$3$5$7$9 /ig;
        # Board expressions, only ranks, but optionally with 1 suits after the first card
        s/board(\s*)=(\s*)([AKQJT]|\d)?([cdhs])?(\s*)([AKQJT]|\d)?(\s*)([AKQJT]|\d)?(\s*)([AKQJT]|\d)?(\s*)([AKQJT]|\d)?/board\$$3$4$6$8$10$12 /ig;
	}
	s/ board = / board\$/ig;
	s/ suited/Suited/ig;
	# Remove spaces in card expressions
	#s/a a/AA/ig;
	#s/a k/AK/ig;
	#s/a q/AQ/ig;
	#s/a j/AJ/ig;
	#s/a t/AT/ig;
	#s/k q/KQ/ig;
	#s/k j/KJ/ig;
	#s/k t/KT/ig;
	#s/q j/QJ/ig;
	#s/q t/QT/ig;
	#s/j t/JT/ig;
	#s/a (\d)/A$1/ig;
	#s/k (\d)/K$1/ig;
	#s/q (\d)/Q$1/ig;
	#s/j (\d)/J$1/ig;
	#s/t (\d)/T$1/ig;
	#s/(\d) (\d)/$1$2/ig;
	# Adapt "Raise X" and "Bet X" to "RaiseBy" which seems to be mostly used
	s/Raise(\s*)(\d)/RaiseBy $2/ig;
	s/Bet(\s*)(\d)/RaiseBy $2/ig;
	# Temporary change RaiseMin and BetMin to Raise and Bet,
	# as OH 7.4.3 does not handle them (yet).
	s/RaiseMin/Raise/ig;
	s/BetMin/Bet/ig;
	# User Variables (at the end of a line,
	# but might also be followed by a comment
	s/( )*$//;
	s/user(\w*)$/ SET user$1/ig;
	s/user(\w*)([ ]*)\/\// SET user$1 \/\//ig;
	# Other user-variables: care about cases
	s/ user/ user/ig;
	s/\(user/\(user/ig;
	# Suited
	s/ suited/Suited/ig;
	# Proper Cases for actions
	s/RaiseMax/RaiseMax/ig;
	s/RaisePot/RaisePot/ig;
	s/RaiseHalfPot/RaiseHalfPot/ig;
	s/RaiseMin/RaiseMin/ig;
	s/Raise/Raise/ig;
	s/BetMax/BetMax/ig;
	s/BetPot/BetPot/ig;
	s/BetHalfPot/RaiseHalfPot/ig;
	s/BetMin/BetMin/ig;
	s/ Bet / Bet /ig;
	s/Call/Call/ig;
	s/Check/Check/ig;
	s/Fold/Fold/ig;
	# Proper cases for operators
	s/ AND / AND /ig;
	s/\)AND /\)AND /ig;
	s/ AND\(/ AND\(/ig;
	s/\)AND\(/\)AND\(/ig;
	s/ NOT / NOT /ig;
	s/\(NOT /\(NOT /ig;
	s/ NOT\(/ NOT\(/ig;
	s/\(NOT\(/\(NOT\(/ig;
	s/ OR / OR /ig;
	s/\)OR /\)OR /ig;
	s/ OR\(/ OR\(/ig;
	s/\)OR\(/\)OR\(/ig;
	# Change 1st, 2nd etc in symbol names
	s/1st/First/ig;
	s/2nd/Second/ig;
	s/3rd/Third/ig;
	s/4th/Fourth/ig;
	s/5th/Fifth/ig;
	# Use the approximately equal operator for bets and stacks
	# "StackSize" also covers "MaxStillToActStackSize" and "MinStillToActStackSize"
	s/BetSize = /BetSize ~~ /ig; 
	s/TotalInvested = /TotalInvested ~~ /ig;  
	s/AmountToCall = /AmountToCall ~~ /ig; 
	s/StackSize = /StackSize ~~ /ig; 
	s/PotSize = /PotSize ~~ /ig; 
	# Fix oddities in some ShankyBots
	s/suitedor/Suited OR/ig;
	s/and notr/AND NOT R/ig;
	s/ andb/AND B/ig;
	s/1AND/1 AND/ig;
	s/2AND/2 AND/ig;
	s/Raise (\d+)/RaiseBy ($1)/ig;
	s/FORCE FORCE/FORCE/ig;
	s/nothave/NOT Have/ig;
	s/notflush/NOT Flush/ig;
	s/Possibleor/Posisble OR/ig;
	s/notTrips/nOT Trips/ig;
	s/Posisble/Possible/ig;
	s/forc([ ]*)$/FORCE/ig;
	s/^(\s*)force(\s*)$//ig;   
	# Correct missing Force
	s/([ ])*$//;
	s/Fold$/Fold FORCE/ig;
	s/Check$/Check FORCE/ig;
	s/Call$/Call FORCE/ig;
	s/Raise$/Raise FORCE/ig;
	s/RaisePot$/RaisePot FORCE/ig;
	s/RaiseMax$/RaiseMax FORCE/ig;
	s/RaiseBy (\d+)(%?)$/RaiseBy $1$2 FORCE/ig;
	# Also correct missing force after action and before comment
	s/RaiseMax((\s)*\/\/)/RaiseMax FORCE \/\//ig;
	# And then correct incorrect corrections, 
	# e.g. WHEN "BotsLastAction = call" at the end of a line
	s/=((\s)*)Fold FORCE$/= Fold/ig;
	s/=((\s)*)Check FORCE$/= Check/ig;
	s/=((\s)*)Call FORCE$/= Call/ig;
	s/=((\s)*)Raise FORCE$/= Raise/ig;
	s/=((\s)*)RaisePot FORCE$/= RaisePot/ig;
	s/=((\s)*)RaiseMax FORCE$/= RaiseMax/ig;
	# Cleaning up
	# Remove extra spaces
	s/    / /;
	s/   / /;
	s/  / /;
	s/^    //;
	s/^   //;
	s/^  //;
	s/^ //;
	# Hand and board expressions can take too much input
    # affecting actions like Call and Check and operators like AND
	s/c all force/ Call FORCE/ig;
	s/ca ll force/ Call FORCE/ig;
	s/c heck force/ Check FORCE/ig;
	s/a nd / AND /ig;
	s/s uited/Suited/ig;
	# Hand and board expressions can also incorrectly take the endd f other symbols
	s/OnBoard\$/OnBoard = /ig;
	s/ToBoard\$/ToBoard = /ig;
	s/InHand\$/InHand = /ig;
	# Remove invalid SET
	# from multi-line-expressions with user-var at the end
	s/WHEN SET/WHEN /ig;
	s/AND SET/AND /ig;
	s/NOT SET/NOT /ig;
	s/OR SET/OR /ig;
	s/\(SET/\(/ig;
	s/\( SET/\(/ig;
	# Remove incorrect FORCE after uservariables that look like actions ("user_RaiseFold")
	s/user((\w)+)(\s)+FORCE/user$1/ig;
	# Again remove superfluous spaces that got introduced by cleanups
	s/  / /;
	# Care about indentation
	if (m/^\/\//) 
	{
		# Comments: keep indentation loke previous line
	} 
	elsif (m/^##/)
	{
		# Function headers
		$needs_indentation = 0;
	}
	elsif (m/^WHEN/ && !m/FORCE/ && !m/SET/)
	{
		# Open ended when condition
		$needs_indentation = 0;
	} 
	else
	{
		# When condition with action or multi-line-condition
		$needs_indentation = 1;
	}
	if ($needs_indentation)
	{
		print "    ";
	}
	# And finally write the processed line to standard output
	print;
	# Print newline if missing
	if (!m/\n$/)
	{
		print "\n";
	}
}