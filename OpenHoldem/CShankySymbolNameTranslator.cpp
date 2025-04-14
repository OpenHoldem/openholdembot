//******************************************************************************
//
// This file is part of the OpenHoldem project
//    Source code:           https://github.com/OpenHoldem/openholdembot/
//    Forums:                http://www.maxinmontreal.com/forums/index.php
//    Licensed under GPL v3: http://www.gnu.org/licenses/gpl.html
//
//******************************************************************************
//
// Purpose: Adapting symbol names when importing Shanky PPL,
//   also caring about proper cases, because Shanky PPL is case-insensitive.
//
//******************************************************************************

#include "pch.h"
#include "CShankySymbolNameTranslator.h"

CShankySymbolNameTranslator::CShankySymbolNameTranslator() {
  InitSymbolTable();
}

CShankySymbolNameTranslator::~CShankySymbolNameTranslator() {
}

void CShankySymbolNameTranslator::InitSymbolTable() {
  _shanky_symbol_table.clear();
  // On the left: Shanky-PPL-symbol in lowercases
  // Attention! Naming can differ slightly.
  // On the right: OpenPPL-symbol with upper-cases,
  //
  // First: numerical functions
  _shanky_symbol_table["amounttocall"] = "AmountToCall";
  _shanky_symbol_table["bets"] = "Bets";
  _shanky_symbol_table["betsize"] = "BetSize";
  _shanky_symbol_table["bigblindsize"] = "BigBlindSize";
  _shanky_symbol_table["botsactionsonthisround"] = "BotsActionsOnThisRound";
  _shanky_symbol_table["botsactionsonflop"] = "BotsActionsOnFlop";
  _shanky_symbol_table["botsactionspreflop"] = "BotsActionsPreflop";
  _shanky_symbol_table["botslastaction"] = "BotsLastAction";
  _shanky_symbol_table["botslastpreflopaction"] = "BotsLastPreflopAction";
  _shanky_symbol_table["calls"] = "Calls";
  _shanky_symbol_table["callssincelastraise"] = "CallsSinceLastRaise";
  _shanky_symbol_table["checks"] = "Checks";
  _shanky_symbol_table["firstcallerposition"] = "FirstCallerPosition";
  _shanky_symbol_table["firstraiserposition"] = "FirstRaiserPosition";
  _shanky_symbol_table["folds"] = "Folds";
  _shanky_symbol_table["lastcallerposition"] = "LastCallerPosition";
  _shanky_symbol_table["lastraiserposition"] = "LastRaiserPosition";
  _shanky_symbol_table["lowcardsinhand"] = "LowCardsInHand";
  _shanky_symbol_table["lowcardsonboard"] = "LowCardsOnBoard";
  _shanky_symbol_table["maxcurrentopponentstacksize"] = "MaxOpponentsLeftStackSize";
  _shanky_symbol_table["maxopponentstacksize"] = "MaxOpponentStackSize";
  _shanky_symbol_table["minopponentstacksize"] = "MinOpponentStackSize";
  _shanky_symbol_table["maxstilltoactstacksize"] = "MaxStillToActStackSize";
  _shanky_symbol_table["minstilltoactstacksize"] = "MinStillToActStackSize";
  _shanky_symbol_table["numberofraisesbeforeflop"] = "NumberOfRaisesBeforeFlop";
  _shanky_symbol_table["numberofraisesonflop"] = "NumberOfRaisesOnFlop";
  _shanky_symbol_table["numberofraisesonturn"] = "NumberOfRaisesOnTurn";
  // NutFullHouseOrFourOfAKind is special, as it returnx a different value for "no full house"
  // But we created a Shanky-compatible version
  _shanky_symbol_table["nutfullhouseorfourofakind"] = "NutFullHouseOrFourOfAKind_ShankyCompatible";
  _shanky_symbol_table["opponents"] = "Opponents";
  _shanky_symbol_table["opponentsattable"] = "OpponentsAtTable";
  _shanky_symbol_table["opponentsleft"] = "OpponentsLeft";
  _shanky_symbol_table["opponentsonflop"] = "OpponentsOnFlop";
  _shanky_symbol_table["opponentswithhigherstack"] = "OpponentsWithHigherStack";
  _shanky_symbol_table["opponentswithlowerstack"] = "OpponentsWithLowerStack";
  _shanky_symbol_table["overcards"] = "Overcards";
  _shanky_symbol_table["overcardsonboard"] = "OvercardsOnBoard";
  _shanky_symbol_table["position"] = "Position";
  _shanky_symbol_table["potsize"] = "PotSize";
  _shanky_symbol_table["raises"] = "Raises";
  _shanky_symbol_table["raisessincelastplay"] = "RaisesSinceLastPlay";
  _shanky_symbol_table["random"] = "Random";
  _shanky_symbol_table["stacksize"] = "StackSize";
  _shanky_symbol_table["startingstacksize"] = "StartingStackSize";
  _shanky_symbol_table["stilltoact"] = "StillToAct";
  _shanky_symbol_table["suitsinhand"] = "SuitsInHand";
  _shanky_symbol_table["suitsonboard"] = "SuitsOnBoard";
  _shanky_symbol_table["suitsonflop"] = "SuitsOnFlop";
  _shanky_symbol_table["totalinvested"] = "TotalInvested";
  // Boolean functions
  _shanky_symbol_table["acepresentonflop"] = "AcePresentOnFlop";
  _shanky_symbol_table["botcalledbeforeflop"] = "BotCalledBeforeFlop";
  _shanky_symbol_table["botislastraiser"] = "BotIsLastRaiser";
  _shanky_symbol_table["botraisedbeforeflop"] = "BotRaisedBeforeFlop";
  _shanky_symbol_table["botraisedonflop"] = "BotRaisedOnFlop";
  _shanky_symbol_table["botraisedonturn"] = "BotRaisedOnTurn";
  _shanky_symbol_table["calledonflop"] = "BotCalledOnFlop";
  _shanky_symbol_table["calledonturn"] = "BotCalledOnTurn";
  _shanky_symbol_table["doublesuited"] = "DoubleSuited";
  _shanky_symbol_table["flushonboard"] = "FlushOnBoard";
  _shanky_symbol_table["flushpossible"] = "FlushPossible";
  _shanky_symbol_table["flushpossibleonflop"] = "FlushPossibleOnFlop";
  _shanky_symbol_table["flushpossibleonturn"] = "FlushPossibleOnTurn";
  _shanky_symbol_table["fourcardstraightinhand"] = "FourCardStraightInHand";
  _shanky_symbol_table["fourcardstowheelonboard"] = "FourCardsToWheelOnBoard";
  _shanky_symbol_table["fourcardswith1gapinhand"] = "FourCardsWith1GapInHand";
  _shanky_symbol_table["fourof1suitonturn"] = "FourOfOneSuitOnTurn";
  _shanky_symbol_table["fullhouseonboard"] = "FullHouseOnBoard";
  _shanky_symbol_table["hadpaironflop"] = "HadPairOnFlop";
  _shanky_symbol_table["hadpaironturn"] = "HadPairOnTurn";
  _shanky_symbol_table["hadtoppaironflop"] = "HadTopPairOnFlop";
  _shanky_symbol_table["hadtoppaironturn"] = "HadTopPairOnTurn";
  _shanky_symbol_table["hadtwopaironflop"] = "HadTwoPairOnFlop";
  _shanky_symbol_table["hadoverpaironflop"] = "HadOverPairOnFlop";
  _shanky_symbol_table["have10outstraightdraw"] = "Have10OutStraightDraw";
  _shanky_symbol_table["have12outstraightdraw"] = "Have12OutStraightDraw";
  _shanky_symbol_table["have16outstraightdraw"] = "Have16OutStraightDraw";
  _shanky_symbol_table["have2ndbestkicker"] = "HaveSecondBestKicker";
  _shanky_symbol_table["have2ndbestkickerorbetter"] = "HaveSecondBestKickerOrBetter";
  _shanky_symbol_table["have2ndbestoverpairorbetter"] = "HaveSecondBestOverPairOrBetter";
  _shanky_symbol_table["have2ndnutflush"] = "HaveSecondNutFlush";
  _shanky_symbol_table["have2ndnutflushdraw"] = "HaveSecondNutFlushDraw";
  _shanky_symbol_table["have2ndnutlow"] = "HaveSecondNutLow";
  _shanky_symbol_table["have2ndnutlowdraw"] = "HaveSecondNutLowDraw";
  _shanky_symbol_table["have2ndnutstraight"] = "HaveSecondNutStraight";
  _shanky_symbol_table["have2ndoverpair"] = "HaveSecondOverPair";
  _shanky_symbol_table["have2ndtoppair"] = "HaveSecondTopPair";
  _shanky_symbol_table["have2ndtopset"] = "HaveSecondTopSet";
  _shanky_symbol_table["have3rdbestkicker"] = "HaveThirdBestKicker";
  _shanky_symbol_table["have3rdbestkickerorbetter"] = "HaveThirdBestKickerOrBetter";
  _shanky_symbol_table["have3rdbestoverpairorbetter"] = "HaveThirdBestOverPairOrBetter";
  _shanky_symbol_table["have3rdnutflush"] = "HaveThirdNutFlush";
  _shanky_symbol_table["have3rdnutflushdraw"] = "HaveThirdNutFlushDraw";
  _shanky_symbol_table["have3rdnutlow"] = "HaveThirdNutLow";
  _shanky_symbol_table["have3rdnutlowdraw"] = "HaveThirdNutLowDraw";
  _shanky_symbol_table["have3rdoverpair"] = "HaveThirdOverPair";
  _shanky_symbol_table["have3rdtoppair"] = "HaveThirdTopPair";
  _shanky_symbol_table["have3rdtopset"] = "HaveThirdTopSet";
  _shanky_symbol_table["have4thnutflush"] = "HaveFourthNutFlush";
  _shanky_symbol_table["have4thnutflushdraw"] = "HaveFourthNutFlushDraw";
  _shanky_symbol_table["have4thnutlow"] = "HaveFourthNutLow";
  _shanky_symbol_table["have4thnutlowdraw"] = "HaveFourthNutLowDraw";
  _shanky_symbol_table["have4thoverpair"] = "HaveFourthOverPair";
  _shanky_symbol_table["have4thtoppair"] = "HaveFourthTopPair";
  _shanky_symbol_table["have4thtopset"] = "HaveFourthTopSet";
  _shanky_symbol_table["have5thnutflush"] = "HaveFifthNutFlush";
  _shanky_symbol_table["have5thnutflushdraw"] = "HaveFifthNutFlushDraw";
  _shanky_symbol_table["have5thoverpair"] = "HaveFifthOverPair";
  _shanky_symbol_table["havebackdoor2ndnutflushdraw"] = "HaveBackdoorSecondNutFlushDraw";
  _shanky_symbol_table["havebackdoor3rdnutflushdraw"] = "HaveBackdoorThirdNutFlushDraw";
  _shanky_symbol_table["havebackdoorflushdraw"] = "HaveBackdoorFlushDraw";
  _shanky_symbol_table["havebackdoornutflushdraw"] = "HaveBackdoorNutFlushDraw";
  _shanky_symbol_table["havebestkicker"] = "HaveBestKicker";
  _shanky_symbol_table["havebestkickerorbetter"] = "HaveBestKickerOrBetter";
  _shanky_symbol_table["havebestoverpairorbetter"] = "HaveBestOverPairOrBetter";
  _shanky_symbol_table["havebottompair"] = "HaveBottomPair";
  _shanky_symbol_table["havebottomset"] = "HaveBottomSet";
  _shanky_symbol_table["havebottomtwopair"] = "HaveBottomTwoPair";
  _shanky_symbol_table["haveflush"] = "HaveFlush";
  _shanky_symbol_table["haveflushdraw"] = "HaveFlushDraw";
  _shanky_symbol_table["havefullhouse"] = "HaveFullHouse";
  _shanky_symbol_table["haveinsidenutstraightdraw"] = "HaveInsideNutStraightDraw";
  _shanky_symbol_table["haveinsidestraightdraw"] = "HaveInsideStraightDraw";
  _shanky_symbol_table["havelow"] = "HaveLow";
  _shanky_symbol_table["havenothing"] = "HaveNothing";
  _shanky_symbol_table["havenutflush"] = "HaveNutFlush";
  _shanky_symbol_table["havenutflushcard"] = "HaveNutFlushCard";
  _shanky_symbol_table["havenutflushdraw"] = "HaveNutFlushDraw";
  _shanky_symbol_table["havenutlow"] = "HaveNutLow";
  _shanky_symbol_table["havenutlowbackdoordraw"] = "HaveNutLowBackdoorDraw";
  _shanky_symbol_table["havenutlowdraw"] = "HaveNutLowDraw";
  _shanky_symbol_table["havenutlowdrawwithbackup"] = "HaveNutLowDrawWithBackup";
  _shanky_symbol_table["havenutlowwithbackup"] = "HaveNutLowWithBackup";
  _shanky_symbol_table["havenuts"] = "HaveNuts";
  _shanky_symbol_table["havenutstraight"] = "HaveNutStraight";
  _shanky_symbol_table["havenutstraightdraw"] = "HaveNutStraightDraw";
  _shanky_symbol_table["havenutstraightflush"] = "HaveNutStraightFlush";
  _shanky_symbol_table["haveoverpair"] = "HaveOverPair";
  _shanky_symbol_table["havepair"] = "HavePair";
  _shanky_symbol_table["havequads"] = "HaveQuads";
  _shanky_symbol_table["haveset"] = "HaveSet";
  _shanky_symbol_table["havestraight"] = "HaveStraight";
  _shanky_symbol_table["havestraightdraw"] = "HaveStraightDraw";
  _shanky_symbol_table["havestraightflush"] = "HaveStraightFlush";
  _shanky_symbol_table["havetopnonboardpairedpair"] = "HaveTopNonBoardPairedPair";
  _shanky_symbol_table["havetoppair"] = "HaveTopPair";
  _shanky_symbol_table["havetopset"] = "HaveTopSet";
  _shanky_symbol_table["havetoptwopair"] = "HaveTopTwoPair";
  _shanky_symbol_table["havetrips"] = "HaveTrips";
  _shanky_symbol_table["havetwopair"] = "HaveTwoPair";
  _shanky_symbol_table["haveunderpair"] = "HaveUnderPair";
  _shanky_symbol_table["haveunderstraight"] = "HaveUnderStraight";
  _shanky_symbol_table["inbigblind"] = "InBigBlind";
  _shanky_symbol_table["inbutton"] = "InButton";
  _shanky_symbol_table["insmallblind"] = "InSmallBlind";
  _shanky_symbol_table["isfinaltable"] = "IsFinalTable";
  _shanky_symbol_table["kingpresentonflop"] = "KingPresentOnFlop";
  _shanky_symbol_table["lowpossible"] = "LowPossible";
  _shanky_symbol_table["lowpossibleonflop"] = "LowPossibleOnFlop";
  _shanky_symbol_table["lowpossibleonturn"] = "LowPossibleOnTurn";
  _shanky_symbol_table["morethanonestraightpossibleonflop"] = "MoreThanOneStraightPossibleOnFlop";
  _shanky_symbol_table["morethanonestraightpossibleonturn"] = "MoreThanOneStraightPossibleOnRiver";
  _shanky_symbol_table["nobettingonflop"] = "NoBettingOnFlop";
  _shanky_symbol_table["nobettingonturn"] = "NoBettingOnTurn";
  _shanky_symbol_table["onecardflushpossible"] = "OneCardFlushPossible";
  _shanky_symbol_table["onecardstraightpossible"] = "OneCardStraightPossible";
  _shanky_symbol_table["onecardstraightflushpossible"] = "OneCardStraightFlushPossible";
  _shanky_symbol_table["onecardstraightpossibleonturn"] = "OneCardStraightPossibleOnTurn";
  _shanky_symbol_table["only1onecardstraightpossible"] = "Only1OneCardStraightPossible";
  _shanky_symbol_table["onlyonestraightpossible"] = "OnlyOneStraightPossible";
  _shanky_symbol_table["opponentcalledonflop"] = "OpponentCalledOnFlop";
  _shanky_symbol_table["opponentcalledonturn"] = "OpponentCalledOnTurn";
  _shanky_symbol_table["opponentisallin"] = "OpponentIsAllin";
  _shanky_symbol_table["others"] = "Others";
  _shanky_symbol_table["pairinhand"] = "PairInHand";
  _shanky_symbol_table["paironboard"] = "PairOnBoard";
  _shanky_symbol_table["paironflop"] = "PairOnFlop";
  _shanky_symbol_table["paironturn"] = "PairOnTurn";
  _shanky_symbol_table["quadsonboard"] = "QuadsOnBoard";
  _shanky_symbol_table["queenpresentonflop"] = "QueenPresentOnFlop";
  _shanky_symbol_table["raisesbeforeflop"] = "RaisesBeforeFlop";
  _shanky_symbol_table["raisesonflop"] = "RaisesOnFlop";
  _shanky_symbol_table["raisesonturn"] = "RaisesOnTurn";
  _shanky_symbol_table["rivercardisovercardtoboard"] = "RiverCardIsOvercardToBoard";
  _shanky_symbol_table["secondtopflopcardpairedonriver"] = "SecondTopFlopCardPairedOnRiver";
  _shanky_symbol_table["secondtopflopcardpairedonturn"] = "SecondTopFlopCardPairedOnTurn";
  _shanky_symbol_table["stackunknown"] = "StackUnknown";
  _shanky_symbol_table["straightflushpossible"] = "StraightFlushPossible";
  _shanky_symbol_table["straightflushpossiblebyothers"] = "StraightFlushPossibleByOthers";
  _shanky_symbol_table["straightonboard"] = "StraightOnBoard";
  _shanky_symbol_table["straightpossible"] = "StraightPossible";
  _shanky_symbol_table["straightpossibleonflop"] = "StraightPossibleOnFlop";
  _shanky_symbol_table["straightpossibleonturn"] = "StraightPossibleOnTurn";
  _shanky_symbol_table["threecardstraightinhand"] = "ThreeCardStraightInHand";
  _shanky_symbol_table["threecardstraightonboard"] = "ThreeCardStraightOnBoard";
  _shanky_symbol_table["threecardswith1gapinhand"] = "ThreeCardsWith1gapInHand";
  _shanky_symbol_table["topflopcardpairedonriver"] = "TopFlopCardPairedOnRiver";
  _shanky_symbol_table["topflopcardpairedonturn"] = "TopFlopCardPairedOnTurn";
  _shanky_symbol_table["tripsinhand"] = "TripsInHand";
  _shanky_symbol_table["tripsonboard"] = "TripsOnBoard";
  _shanky_symbol_table["tripsonboardonturn"] = "TripsOnBoardOnTurn";
  _shanky_symbol_table["turncardpaired"] = "TurnCardPaired";
  _shanky_symbol_table["turncardisovercardtoboard"] = "TurnCardIsOvercardToBoard";
  _shanky_symbol_table["twoofonesuitpresentonflop"] = "TwoOfOneSuitPresentOnFlop";
  _shanky_symbol_table["twopairinhand"] = "TwoPairInHand";
  _shanky_symbol_table["twopaironboard"] = "TwoPairOnBoard";
  _shanky_symbol_table["uncoordinatedflop"] = "UncoordinatedFlop";
  _shanky_symbol_table["wheelpossible"] = "WheelPossible";
  // Action constants
  _shanky_symbol_table["beep"] = "Beep";
  _shanky_symbol_table["betmax"] = "BetMax";
  _shanky_symbol_table["betpot"] = "BetPot";
  _shanky_symbol_table["bethalfpot"] = "BetHalfPot";
  _shanky_symbol_table["betmin"] = "BetMin";
  _shanky_symbol_table["bet"] = "Bet";
  _shanky_symbol_table["raisemax"] = "RaiseMax";
  _shanky_symbol_table["raisepot"] = "RaisePot";
  _shanky_symbol_table["raisehalfpot"] = "RaiseHalfPot";
  _shanky_symbol_table["raisemin"] = "RaiseMin";
  _shanky_symbol_table["raise"] = "Raise";
  _shanky_symbol_table["call"] = "Call";
  _shanky_symbol_table["check"] = "Check";
  _shanky_symbol_table["fold"] = "Fold";
  _shanky_symbol_table["none"] = "None";
  // Shankys command "SitOut" means: first fold, then sitout.
  // We don#t support this behavious, that's why we fold
  _shanky_symbol_table["sitout"] = "Fold";
  // Position constants
  _shanky_symbol_table["first"] = "First";
  _shanky_symbol_table["middle"] = "Middle";
  _shanky_symbol_table["last"] = "Last";
}

CString CShankySymbolNameTranslator::Translate(CString symbol) {
  CString key = symbol;
  CString lookup_result = _shanky_symbol_table[key.MakeLower()];
  if (lookup_result != "") {
    return lookup_result;
  }
  // return symbol without modifications
  return symbol;
}