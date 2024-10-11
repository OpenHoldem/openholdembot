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

#include "stdafx.h"
#include "CTableState.h"
#include "CSymbolEngineRange.h"
#include "CIteratorThread.h"
#include "CBetroundCalculator.h"
#include "CEngineContainer.h"
#include "CSymbolEngineMultiplexer.h"
#include "CardFunctions.h"
#include "CFunction.h"
#include "CFunctionCollection.h"
#include "COHScriptList.h"
#include "CSymbolEnginePrwin.h"
#include "CSymbolEngineMemorySymbols.h"
#include "CParseErrors.h"
#include <regex>
#include <string>

using namespace std;

sprw1326*	prw1326;	//prwin 1326 data structure Matrix 2008-04-29
const int suit[4] = { 0, 13, 26, 39 };

CString handrank169_table[kMaxNumberOfPlayers] = {
	{ "AA,KK,QQ,JJ,TT,99,88,AKs,77,AQs,AKo,AJs,AQo,ATs,66,AJo,KQs,ATo,A9s,KJs,A8s,KTs,KQo,55,A7s,A9o,KJo,QJs,K9s,KTo,A8o,A6s,QTs,A5s,A4s,A7o,QJo,K8s,A3s,K9o,44,Q9s,JTs,QTo,A6o,K7s,A5o,A2s,K6s,A4o,K8o,Q8s,J9s,A3o,K5s,Q9o,JTo,K7o,A2o,K4s,33,Q7s,K6o,T9s,J8s,K3s,Q8o,Q6s,J9o,K5o,K2s,Q5s,T8s,J7s,K4o,Q7o,T9o,Q4s,J8o,K3o,22,Q6o,Q3s,98s,T7s,J6s,K2o,Q2s,Q5o,J5s,T8o,J7o,Q4o,97s,J4s,T6s,J3s,Q3o,98o,T7o,J6o,87s,J2s,96s,Q2o,J5o,T5s,T4s,97o,J4o,T6o,86s,95s,T3s,J3o,76s,87o,T2s,96o,J2o,85s,T5o,94s,T4o,75s,93s,86o,65s,95o,T3o,84s,92s,76o,T2o,74s,85o,54s,64s,83s,94o,75o,82s,93o,73s,65o,53s,63s,84o,92o,43s,74o,72s,54o,64o,52s,62s,83o,82o,42s,73o,53o,63o,32s,43o,72o,52o,62o,42o,32o" },
	{ "AA,KK,QQ,JJ,TT,99,AKs,88,AQs,AKo,AJs,77,KQs,ATs,AQo,KJs,AJo,KTs,KQo,A9s,QJs,ATo,66,A8s,KJo,QTs,K9s,JTs,A7s,KTo,QJo,A9o,55,A5s,A6s,Q9s,QTo,A8o,K8s,A4s,J9s,K9o,A3s,JTo,K7s,A7o,T9s,Q8s,A2s,K6s,44,Q9o,A5o,A6o,J8s,K5s,K8o,T8s,A4o,J9o,Q7s,K4s,98s,A3o,K7o,T9o,Q6s,K3s,J7s,Q8o,A2o,33,K6o,Q5s,K2s,T7s,J8o,97s,87s,Q4s,K5o,T8o,J6s,Q3s,Q7o,98o,K4o,T6s,J5s,Q2s,96s,Q6o,76s,86s,J7o,22,K3o,J4s,T7o,Q5o,J3s,K2o,T5s,97o,87o,65s,J2s,95s,75s,Q4o,T4s,85s,J6o,T3s,Q3o,54s,T6o,J5o,T2s,96o,64s,86o,76o,Q2o,94s,74s,84s,J4o,93s,53s,J3o,92s,T5o,63s,65o,73s,43s,95o,75o,83s,J2o,85o,T4o,82s,52s,T3o,54o,62s,42s,64o,72s,T2o,74o,94o,84o,32s,93o,53o,92o,63o,43o,73o,83o,82o,52o,62o,42o,72o,32o" },
	{ "AA,KK,QQ,JJ,TT,99,AKs,AQs,AKo,AJs,KQs,88,ATs,AQo,KJs,QJs,KTs,KQo,AJo,77,QTs,A9s,ATo,JTs,KJo,A8s,K9s,QJo,66,KTo,A7s,Q9s,QTo,J9s,A5s,T9s,A9o,A6s,JTo,K8s,A4s,55,A3s,K7s,Q8s,A8o,K9o,J8s,A2s,T8s,K6s,98s,Q9o,A7o,K5s,J9o,T9o,Q7s,A5o,44,K4s,A6o,K8o,J7s,Q6s,T7s,97s,87s,K3s,A4o,Q5s,K2s,Q8o,K7o,A3o,J8o,Q4s,T8o,J6s,76s,A2o,98o,33,K6o,T6s,86s,96s,Q3s,J5s,Q2s,K5o,J4s,65s,Q7o,75s,J7o,J3s,85s,K4o,T7o,T5s,95s,97o,87o,22,Q6o,J2s,54s,T4s,K3o,64s,Q5o,T3s,K2o,74s,84s,T2s,76o,Q4o,94s,53s,J6o,T6o,86o,96o,93s,Q3o,63s,43s,J5o,92s,73s,Q2o,65o,83s,J4o,52s,75o,82s,85o,42s,J3o,T5o,62s,95o,54o,72s,T4o,J2o,32s,64o,T3o,74o,84o,T2o,53o,94o,93o,63o,43o,92o,73o,83o,52o,82o,42o,62o,72o,32o" },
	{ "AA,KK,QQ,JJ,TT,AKs,AQs,99,KQs,AKo,AJs,KJs,ATs,AQo,88,QJs,KTs,KQo,QTs,AJo,JTs,A9s,KJo,77,ATo,K9s,A8s,QJo,Q9s,KTo,A7s,J9s,T9s,QTo,A5s,66,JTo,A6s,K8s,A4s,A9o,A3s,Q8s,K7s,J8s,T8s,A2s,98s,K9o,55,K6s,A8o,Q9o,K5s,T9o,Q7s,J9o,K4s,T7s,J7s,87s,A7o,97s,Q6s,K3s,44,A5o,K8o,K2s,Q5s,A6o,76s,A4o,86s,Q4s,Q8o,J6s,T8o,T6s,96s,J8o,A3o,K7o,Q3s,98o,33,J5s,65s,Q2s,A2o,75s,J4s,K6o,85s,54s,J3s,22,T5s,95s,J2s,64s,K5o,Q7o,87o,T4s,T7o,J7o,97o,74s,K4o,T3s,53s,Q6o,84s,T2s,K3o,94s,43s,63s,76o,Q5o,93s,K2o,86o,92s,73s,96o,52s,Q4o,J6o,T6o,83s,42s,65o,82s,Q3o,J5o,62s,75o,32s,Q2o,72s,J4o,54o,85o,95o,T5o,J3o,64o,J2o,T4o,74o,53o,T3o,84o,T2o,94o,43o,63o,93o,73o,92o,52o,83o,42o,82o,62o,32o,72o" },
	{ "AA,KK,QQ,JJ,AKs,TT,AQs,KQs,AKo,AJs,99,KJs,ATs,QJs,AQo,KTs,KQo,QTs,88,JTs,AJo,A9s,KJo,K9s,A8s,QJo,ATo,77,Q9s,T9s,J9s,KTo,A7s,A5s,QTo,JTo,K8s,A4s,A6s,66,A3s,Q8s,T8s,K7s,J8s,98s,A2s,A9o,K6s,K9o,55,K5s,87s,Q7s,A8o,Q9o,T7s,97s,K4s,T9o,J7s,J9o,K3s,Q6s,44,K2s,76s,Q5s,A7o,86s,A5o,Q4s,96s,T6s,33,K8o,J6s,65s,Q3s,A6o,A4o,T8o,J5s,Q8o,Q2s,75s,J8o,98o,54s,22,A3o,K7o,J4s,85s,J3s,64s,95s,A2o,T5s,J2s,K6o,T4s,53s,74s,87o,T3s,97o,43s,T7o,T2s,84s,K5o,Q7o,63s,J7o,94s,K4o,93s,52s,73s,Q6o,76o,92s,K3o,42s,86o,83s,K2o,Q5o,82s,62s,32s,96o,65o,T6o,Q4o,J6o,72s,75o,Q3o,54o,J5o,Q2o,85o,J4o,64o,95o,J3o,T5o,53o,J2o,74o,T4o,43o,T3o,84o,63o,T2o,94o,52o,93o,73o,42o,92o,83o,62o,32o,82o,72o" },
	{ "AA,KK,QQ,JJ,AKs,AQs,TT,KQs,AJs,AKo,KJs,ATs,QJs,99,KTs,AQo,QTs,JTs,KQo,88,A9s,AJo,K9s,KJo,A8s,Q9s,QJo,T9s,J9s,ATo,77,A7s,KTo,A5s,A4s,QTo,K8s,A6s,JTo,A3s,66,T8s,Q8s,A2s,J8s,K7s,98s,K6s,55,A9o,87s,K5s,97s,Q7s,K4s,T7s,K9o,J7s,K3s,T9o,44,Q6s,Q9o,J9o,K2s,76s,A8o,86s,Q5s,33,65s,Q4s,96s,22,T6s,Q3s,A7o,J6s,75s,54s,Q2s,A5o,J5s,K8o,85s,64s,A4o,T8o,J4s,A6o,98o,Q8o,J8o,J3s,53s,A3o,95s,J2s,T5s,74s,K7o,T4s,43s,A2o,T3s,84s,63s,T2s,87o,K6o,52s,94s,97o,93s,73s,42s,T7o,K5o,92s,Q7o,J7o,32s,76o,83s,62s,K4o,82s,86o,Q6o,K3o,72s,K2o,65o,96o,Q5o,54o,T6o,75o,Q4o,J6o,Q3o,Q2o,85o,64o,J5o,53o,J4o,95o,J3o,74o,T5o,43o,J2o,T4o,63o,T3o,84o,52o,T2o,94o,42o,73o,93o,92o,32o,83o,62o,82o,72o" },
	{ "AA,KK,QQ,JJ,AKs,AQs,KQs,TT,AJs,AKo,KJs,ATs,QJs,KTs,99,QTs,AQo,JTs,KQo,A9s,88,AJo,K9s,A8s,T9s,KJo,Q9s,J9s,QJo,77,A7s,A5s,ATo,A4s,KTo,A6s,K8s,A3s,JTo,QTo,66,T8s,A2s,Q8s,98s,J8s,K7s,K6s,55,87s,K5s,97s,44,K4s,T7s,Q7s,J7s,K3s,A9o,76s,33,K2s,Q6s,22,K9o,86s,T9o,65s,Q5s,J9o,Q9o,96s,54s,Q4s,A8o,75s,T6s,Q3s,J6s,Q2s,64s,85s,J5s,53s,A7o,J4s,A5o,J3s,95s,74s,T8o,J2s,43s,A4o,K8o,T5s,98o,J8o,T4s,63s,A6o,A3o,Q8o,T3s,84s,T2s,52s,A2o,K7o,42s,87o,94s,73s,93s,92s,32s,97o,K6o,62s,83s,T7o,82s,76o,K5o,J7o,72s,Q7o,K4o,86o,65o,K3o,Q6o,K2o,54o,96o,75o,Q5o,T6o,Q4o,64o,J6o,Q3o,85o,53o,Q2o,J5o,74o,43o,J4o,95o,J3o,J2o,63o,T5o,52o,T4o,84o,T3o,42o,T2o,73o,94o,32o,93o,62o,92o,83o,82o,72o" },
	{ "AA,KK,QQ,AKs,JJ,AQs,KQs,AJs,TT,AKo,KJs,ATs,QJs,KTs,QTs,99,JTs,AQo,KQo,A9s,88,K9s,T9s,AJo,A8s,J9s,Q9s,KJo,77,A5s,A7s,QJo,A4s,A3s,A6s,ATo,K8s,66,A2s,T8s,KTo,98s,Q8s,J8s,JTo,QTo,K7s,55,87s,K6s,44,97s,33,22,K5s,T7s,K4s,76s,Q7s,K3s,J7s,K2s,86s,65s,Q6s,54s,A9o,Q5s,T9o,96s,75s,Q4s,K9o,J9o,Q3s,64s,T6s,Q9o,Q2s,J6s,85s,53s,A8o,J5s,J4s,74s,43s,J3s,95s,J2s,63s,T5s,A5o,A7o,T8o,T4s,98o,T3s,84s,52s,A4o,T2s,42s,K8o,A3o,J8o,A6o,73s,Q8o,94s,32s,93s,A2o,87o,92s,62s,K7o,83s,97o,82s,76o,72s,K6o,T7o,65o,K5o,86o,J7o,54o,Q7o,K4o,K3o,75o,K2o,Q6o,96o,64o,Q5o,T6o,53o,Q4o,85o,J6o,Q3o,Q2o,43o,74o,J5o,J4o,95o,63o,J3o,J2o,52o,T5o,84o,42o,T4o,T3o,32o,T2o,73o,94o,62o,93o,92o,83o,82o,72o" },
	{ "AA,KK,QQ,AKs,JJ,AQs,KQs,AJs,TT,KJs,AKo,ATs,QJs,KTs,QTs,JTs,99,AQo,A9s,KQo,88,K9s,T9s,A8s,J9s,Q9s,77,AJo,A5s,A7s,A4s,KJo,A3s,66,A6s,QJo,K8s,A2s,T8s,98s,J8s,ATo,Q8s,55,K7s,JTo,KTo,44,33,22,QTo,87s,K6s,97s,K5s,76s,T7s,K4s,K3s,Q7s,K2s,J7s,86s,65s,54s,Q6s,75s,Q5s,96s,Q4s,Q3s,64s,T9o,Q2s,A9o,T6s,53s,J6s,85s,J9o,K9o,43s,J5s,Q9o,74s,J4s,J3s,J2s,95s,63s,A8o,T5s,52s,42s,T4s,T3s,84s,98o,T2s,A5o,T8o,A7o,73s,32s,A4o,94s,93s,62s,A3o,K8o,J8o,92s,A6o,87o,Q8o,83s,A2o,82s,97o,72s,K7o,76o,T7o,65o,K6o,86o,54o,K5o,J7o,Q7o,75o,K4o,K3o,96o,64o,K2o,53o,Q6o,85o,T6o,Q5o,43o,Q4o,Q3o,74o,Q2o,J6o,63o,J5o,95o,52o,J4o,42o,J3o,J2o,84o,T5o,32o,T4o,T3o,73o,T2o,62o,94o,93o,92o,83o,82o,72o" }
};
CString handrank_2way = "AA,KK,QQ,JJ,TT,99,88,AKs,77,AQs,AJs,AKo,ATs,AQo,AJo,KQs,66,A9s,ATo,KJs,A8s,KTs,KQo,A7s,A9o,KJo,55,QJs,K9s,A5s,A6s,A8o,KTo,QTs,A4s,A7o,K8s,A3s,QJo,K9o,A5o,A6o,Q9s,K7s,JTs,A2s,QTo,44,A4o,K6s,K8o,Q8s,A3o,K5s,J9s,Q9o,JTo,K7o,A2o,K4s,Q7s,K6o,K3s,T9s,J8s,33,Q6s,Q8o,K5o,J9o,K2s,Q5s,T8s,K4o,J7s,Q4s,Q7o,T9o,J8o,K3o,Q6o,Q3s,98s,T7s,J6s,K2o,22,Q2s,Q5o,J5s,T8o,J7o,Q4o,97s,J4s,T6s,J3s,Q3o,98o,87s,T7o,J6o,96s,J2s,Q2o,T5s,J5o,T4s,97o,86s,J4o,T6o,95s,T3s,76s,J3o,87o,T2s,85s,96o,J2o,T5o,94s,75s,T4o,93s,86o,65s,84s,95o,T3o,92s,76o,74s,T2o,54s,85o,64s,83s,94o,75o,82s,73s,93o,65o,53s,63s,84o,92o,43s,74o,72s,54o,64o,52s,62s,83o,42s,82o,73o,53o,63o,32s,43o,72o,52o,62o,42o,32o";
CString handrank_3way = "AA,KK,QQ,JJ,TT,99,AKs,88,AQs,AKo,AJs,KQs,ATs,AQo,77,KJs,AJo,KTs,A9s,KQo,ATo,QJs,66,A8s,QTs,KJo,K9s,A7s,JTs,KTo,A9o,QJo,A5s,A6s,Q9s,A4s,55,A8o,QTo,K8s,A3s,J9s,K9o,K7s,A7o,JTo,T9s,A2s,Q8s,K6s,A5o,A6o,Q9o,J8s,K5s,44,A4o,K8o,T8s,K4s,Q7s,J9o,A3o,98s,K7o,Q6s,K3s,T9o,J7s,Q8o,A2o,K2s,K6o,Q5s,T7s,J8o,97s,Q4s,33,K5o,87s,T8o,J6s,Q3s,Q7o,K4o,J5s,T6s,98o,Q2s,Q6o,96s,K3o,86s,J4s,J7o,76s,Q5o,T7o,J3s,K2o,22,T5s,97o,J2s,87o,Q4o,65s,95s,T4s,75s,85s,J6o,Q3o,T3s,T6o,J5o,54s,T2s,Q2o,96o,64s,94s,86o,76o,74s,84s,J4o,93s,J3o,53s,92s,T5o,63s,65o,95o,J2o,43s,75o,73s,T4o,83s,85o,82s,T3o,52s,54o,62s,T2o,42s,64o,72s,94o,74o,84o,32s,93o,53o,92o,63o,43o,73o,83o,82o,52o,62o,42o,72o,32o";
CString handrank_4way = "AA,KK,QQ,JJ,TT,AKs,99,AQs,AKo,AJs,KQs,88,ATs,AQo,KJs,KTs,QJs,AJo,KQo,QTs,A9s,77,ATo,JTs,KJo,A8s,K9s,QJo,A7s,KTo,Q9s,A5s,66,A6s,QTo,J9s,A9o,T9s,A4s,K8s,JTo,K7s,A8o,A3s,Q8s,K9o,A2s,K6s,J8s,T8s,A7o,55,Q9o,98s,K5s,Q7s,J9o,A5o,T9o,A6o,K4s,K8o,Q6s,J7s,T7s,A4o,97s,K3s,87s,Q5s,K7o,44,Q8o,A3o,K2s,J8o,Q4s,T8o,J6s,K6o,A2o,T6s,98o,76s,86s,96s,Q3s,J5s,K5o,Q7o,Q2s,J4s,33,65s,J7o,T7o,K4o,75s,T5s,Q6o,J3s,95s,87o,85s,97o,T4s,K3o,J2s,54s,Q5o,64s,T3s,22,K2o,74s,76o,T2s,Q4o,J6o,84s,94s,86o,T6o,96o,53s,93s,Q3o,J5o,63s,43s,92s,73s,65o,Q2o,J4o,83s,75o,52s,85o,82s,T5o,95o,J3o,62s,54o,42s,T4o,J2o,72s,64o,T3o,32s,74o,84o,T2o,94o,53o,93o,63o,43o,92o,73o,83o,52o,82o,42o,62o,72o,32o";
CString handrank_KS = "AA,KK,AKs,QQ,AKo,JJ,AQs,TT,AQo,99,AJs,88,ATs,AJo,77,66,ATo,A9s,55,A8s,KQs,44,A9o,A7s,KJs,A5s,A8o,A6s,A4s,33,KTs,A7o,A3s,KQo,A2s,A5o,A6o,A4o,KJo,QJs,A3o,22,K9s,A2o,KTo,QTs,K8s,K7s,JTs,K9o,K6s,QJo,Q9s,K5s,K8o,K4s,QTo,K7o,K3s,K2s,Q8s,K6o,J9s,K5o,Q9o,JTo,K4o,Q7s,T9s,Q6s,K3o,J8s,Q5s,K2o,Q8o,Q4s,J9o,Q3s,T8s,J7s,Q7o,Q2s,Q6o,98s,Q5o,J8o,T9o,J6s,T7s,J5s,Q4o,J4s,J7o,Q3o,97s,T8o,J3s,T6s,Q2o,J2s,87s,J6o,98o,T7o,96s,J5o,T5s,T4s,86s,J4o,T6o,97o,T3s,76s,95s,J3o,T2s,87o,85s,96o,T5o,J2o,75s,94s,T4o,65s,86o,93s,84s,95o,T3o,76o,92s,74s,54s,T2o,85o,64s,83s,94o,75o,82s,73s,93o,65o,53s,63s,84o,92o,43s,74o,72s,54o,64o,52s,62s,83o,42s,82o,73o,53o,63o,32s,43o,72o,52o,62o,42o,32o";
CString handrank_PPT = "AA,KK,QQ,JJ,AKs,TT,AKo,AQs,99,AJs,AQo,88,ATs,KQs,AJo,77,KJs,QJs,KTs,KQo,A9s,ATo,66,A8s,QTs,JTs,KJo,A7s,A5s,K9s,A4s,A6s,55,Q9s,A3s,J9s,KTo,QJo,A9o,T9s,K8s,A2s,K7s,44,A8o,QTo,Q8s,JTo,J8s,K6s,98s,T8s,K5s,A7o,K4s,K9o,A5o,33,K3s,A4o,Q9o,87s,Q7s,T7s,Q6s,K2s,J7s,A6o,97s,Q5s,A3o,J9o,T9o,22,K8o,A2o,Q4s,76s,K7o,86s,96s,J6s,J5s,K6o,Q3s,Q2s,T6s,65s,K5o,75s,Q8o,54s,J8o,J4s,T8o,98o,85s,95s,K4o,J3s,64s,T4s,T5s,87o,Q7o,K3o,J2s,74s,97o,J7o,53s,Q6o,T3s,K2o,94s,T7o,84s,43s,63s,Q5o,86o,T2s,93s,76o,Q4o,92s,96o,73s,J6o,Q3o,52s,65o,J5o,T6o,82s,42s,83s,Q2o,75o,54o,J4o,62s,85o,32s,95o,72s,J3o,T5o,T4o,64o,J2o,53o,74o,84o,T3o,43o,94o,T2o,93o,63o,92o,73o,52o,42o,83o,82o,62o,32o,72o";
CString handrank_SMG = "AA,KK,QQ,JJ,AKs,TT,AQs,AJs,AKo,KQs,99,ATs,AQo,KJs,QJs,JTs,88,AJo,KTs,KQo,QTs,J9s,T9s,98s,77,A9s,A8s,A7s,KJo,A5s,A6s,A4s,A3s,QJo,Q9s,A2s,JTo,T8s,97s,87s,76s,65s,66,ATo,55,K9s,KTo,QTo,J8s,86s,75s,54s,K8s,K7s,44,K6s,Q8s,K5s,K4s,K3s,33,J9o,K2s,T9o,T7s,22,98o,64s,53s,43s,A9o,K9o,Q9o,J7s,J8o,T8o,96s,87o,85s,76o,74s,65o,54o,42s,32s,A8o,A7o,A5o,A6o,A4o,K8o,A3o,K7o,A2o,Q7s,K6o,Q6s,Q8o,K5o,Q5s,K4o,Q4s,Q7o,K3o,Q6o,Q3s,J6s,K2o,Q2s,Q5o,J5s,J7o,Q4o,J4s,T6s,J3s,Q3o,T7o,J6o,J2s,Q2o,T5s,J5o,T4s,97o,J4o,T6o,95s,T3s,J3o,T2s,96o,J2o,T5o,94s,T4o,93s,86o,84s,95o,T3o,92s,T2o,85o,83s,94o,75o,82s,73s,93o,63s,84o,92o,74o,72s,64o,52s,62s,83o,82o,73o,53o,63o,43o,72o,52o,62o,42o,32o";
CString handrank_SMGA = "AA,KK,QQ,JJ,AKs,TT,AQs,AJs,AKo,KQs,99,ATs,AQo,KJs,KTs,QJs,88,AJo,A9s,ATo,A8s,KQo,QTs,JTs,A7s,KJo,K9s,A5s,Q9s,T9s,77,A6s,KTo,A4s,QJo,J9s,K8s,A3s,A2s,QTo,Q8s,JTo,66,K7s,J8s,T8s,98s,A9o,A8o,K6s,K5s,55,A7o,K9o,A5o,A6o,44,A4o,K8o,A3o,Q9o,K7o,A2o,K4s,Q7s,K6o,K3s,33,Q6s,Q8o,K5o,J9o,K2s,Q5s,K4o,J7s,Q4s,Q7o,T9o,J8o,K3o,Q6o,Q3s,T7s,J6s,K2o,22,Q2s,Q5o,J5s,T8o,J7o,Q4o,97s,J4s,T6s,J3s,Q3o,98o,87s,T7o,J6o,96s,J2s,Q2o,T5s,J5o,T4s,97o,86s,J4o,T6o,95s,T3s,76s,J3o,87o,T2s,85s,96o,J2o,T5o,94s,75s,T4o,93s,86o,65s,84s,95o,T3o,92s,76o,74s,T2o,54s,85o,64s,83s,94o,75o,82s,73s,93o,65o,53s,63s,84o,92o,43s,74o,72s,54o,64o,52s,62s,83o,42s,82o,73o,53o,63o,32s,43o,72o,52o,62o,42o,32o";
CString handrank_SMGO = "AA,KK,QQ,JJ,TT,99,88,AKs,77,AQs,AJs,AKo,ATs,AQo,AJo,KQs,66,A9s,ATo,KJs,A8s,KTs,KQo,A7s,A9o,KJo,55,QJs,K9s,A5s,A6s,KTo,QTs,A4s,K8s,A3s,QJo,K9o,Q9s,K7s,JTs,A2s,QTo,44,K6s,Q8s,K5s,J9s,Q9o,JTo,K4s,K3s,T9s,J8s,33,J9o,K2s,T8s,J7s,T9o,J8o,98s,T7s,22,T8o,97s,98o,87s,96s,86s,76s,87o,85s,75s,65s,76o,74s,54s,64s,65o,53s,43s,54o,42s,32s,A8o,A7o,A5o,A6o,A4o,K8o,A3o,K7o,A2o,Q7s,K6o,Q6s,Q8o,K5o,Q5s,K4o,Q4s,Q7o,K3o,Q6o,Q3s,J6s,K2o,Q2s,Q5o,J5s,J7o,Q4o,J4s,T6s,J3s,Q3o,T7o,J6o,J2s,Q2o,T5s,J5o,T4s,97o,J4o,T6o,95s,T3s,J3o,T2s,96o,J2o,T5o,94s,T4o,93s,86o,84s,95o,T3o,92s,T2o,85o,83s,94o,75o,82s,73s,93o,63s,84o,92o,74o,72s,64o,52s,62s,83o,82o,73o,53o,63o,43o,72o,52o,62o,42o,32o";
CString handrank_SMGAO = "AA,KK,QQ,JJ,TT,99,88,AKs,77,AQs,AJs,AKo,ATs,AQo,AJo,KQs,66,A9s,ATo,KJs,A8s,KTs,KQo,A7s,A9o,KJo,QJs,K9s,A5s,A6s,A8o,KTo,QTs,A4s,K8s,A3s,QJo,Q9s,K7s,JTs,A2s,QTo,K6s,Q8s,K5s,J9s,JTo,T9s,J8s,T8s,98s,55,A7o,K9o,A5o,A6o,44,A4o,K8o,A3o,Q9o,K7o,A2o,K4s,Q7s,K6o,K3s,33,Q6s,Q8o,K5o,J9o,K2s,Q5s,K4o,J7s,Q4s,Q7o,T9o,J8o,K3o,Q6o,Q3s,T7s,J6s,K2o,22,Q2s,Q5o,J5s,T8o,J7o,Q4o,97s,J4s,T6s,J3s,Q3o,98o,87s,T7o,J6o,96s,J2s,Q2o,T5s,J5o,T4s,97o,86s,J4o,T6o,95s,T3s,76s,J3o,87o,T2s,85s,96o,J2o,T5o,94s,75s,T4o,93s,86o,65s,84s,95o,T3o,92s,76o,74s,T2o,54s,85o,64s,83s,94o,75o,82s,73s,93o,65o,53s,63s,84o,92o,43s,74o,72s,54o,64o,52s,62s,83o,42s,82o,73o,53o,63o,32s,43o,72o,52o,62o,42o,32o";
CString handrank_BC = "AA,KK,AKs,QQ,AKo,JJ,TT,AQs,99,AQo,88,AJs,77,ATs,AJo,66,KQs,ATo,A9s,KJs,A8s,55,KQo,A9o,A7s,A8o,KTs,A6s,44,A5s,A7o,QJs,A4s,KJo,A3s,A2s,K9s,KTo,33,QTs,A5o,A6o,QJo,K8s,JTs,A4o,K9o,A3o,Q9s,A2o,K7s,QTo,22,K6s,K8o,Q8s,K5s,J9s,K7o,JTo,Q9o,K4s,K3s,T9s,K6o,Q7s,J8s,K2s,Q8o,Q6s,K5o,J9o,T8s,Q5s,K4o,J7s,K3o,Q4s,T9o,98s,Q7o,Q3s,K2o,J3s,J8o,Q6o,T7s,Q2s,J6s,Q5o,J5s,97s,87s,T8o,J7o,Q4o,T6s,J4s,Q3o,98o,96s,86s,T7o,J6o,Q2o,J2s,T5s,76s,J5o,T4s,97o,J4o,95s,T3s,65s,87o,85s,75s,54s,T6o,J3o,T2s,94s,96o,64s,84s,74s,86o,76o,53s,J2o,T5o,T4o,93s,95o,92s,43s,63s,83s,65o,73s,85o,75o,82s,52s,T3o,T2o,94o,54o,42s,62s,72s,64o,32s,84o,74o,53o,43o,93o,92o,63o,83o,82o,73o,52o,42o,72o,62o,32o";
CString handrank_BJ = "AA,AKs,AKo,KK,A4s,A5s,QQ,AQs,JJ,TT,AJs,KQs,AQo,ATs,KJs,QJs,KTs,99,QTs,88,AJo,JTs,66,77,55,A9s,KQo,K9s,44,T9s,A8s,J9s,Q9s,KJo,33,ATo,A7s,K8s,A3s,T8s,98s,22,A6s,Q8s,QJo,J8s,K7s,A2s,A9o,A8o,87s,A7o,QTo,JTo,KTo,A5o,K6s,T7s,97s,76s,A4o,A6o,K5s,J7s,86s,A3o,A2o,65s,Q6s,T9o,96s,Q7s,K4s,K9o,J9o,75s,54s,Q9o,T6s,K3s,Q5s,98o,K2s,J6s,85s,K8o,K7o,T8o,64s,K6o,Q4s,J5s,K5o,Q3s,J4s,95s,87o,Q8o,K4o,J8o,K3o,K2o,Q2s,74s,J3s,T5s,T4s,97o,Q7o,Q6o,76o,Q5o,T7o,J7o,J2s,Q4o,84s,Q3o,T3s,86o,J6o,J5o,T2s,94s,J4o,T6o,J3o,96o,J2o,93s,T5o,T4o,95o,T3o,92s,T2o,85o,83s,75o,82s,94o,93o,65o,73s,53s,63s,84o,43s,74o,54o,72s,64o,52s,92o,62s,83o,42s,82o,73o,53o,63o,32s,43o,72o,52o,62o,42o,32o,Q2o";
CString handrank_DD = "AA,AKs,AKo,KK,QQ,AQs,AQo,JJ,TT,AJs,AJo,99,KQs,KQo,88,ATs,ATo,77,KJs,KJo,66,A9s,A9o,KTs,55,KTo,A8s,A8o,QJs,QJo,44,K9s,K9o,33,A7s,QTs,A7o,QTo,22,K8s,K8o,A6s,A6o,Q9s,Q9o,K7s,JTs,K7o,JTo,A5s,A5o,Q8s,Q8o,K6s,J9s,K6o,J9o,A4s,A4o,Q7s,Q7o,K5s,J8s,K5o,J8o,A3s,A3o,T9s,Q6s,T9o,Q6o,K4s,K4o,J7s,J7o,A2s,A2o,Q5s,T8s,Q5o,T8o,K3s,K3o,J6s,J6o,Q4s,T7s,Q4o,T7o,K2s,98s,K2o,J5s,98o,J5o,Q3s,T6s,Q3o,T6o,J4s,97s,97o,J4o,Q2s,Q2o,T5s,T5o,J3s,96s,J3o,96o,87s,T4s,87o,T4o,J2s,95s,J2o,95o,86s,T3s,86o,T3o,94s,94o,T2s,85s,T2o,85o,76s,93s,76o,93o,84s,84o,75s,92s,75o,92o,83s,83o,74s,74o,65s,82s,65o,82o,73s,73o,64s,64o,72s,72o,63s,63o,54s,54o,62s,62o,53s,53o,52s,52o,43s,43o,42s,42o,32s,32o";
CString handrank_DN = "AA,KK,QQ,JJ,TT,AKs,AKo,99,AQs,AQo,AJs,AJo,KQs,KQo,ATs,ATo,88,KJs,KJo,A9s,A9o,KTs,KTo,QJs,QJo,A8s,A8o,77,K9s,K9o,QTs,QTo,A7s,A7o,K8s,K8o,Q9s,Q9o,JTs,JTo,A6s,A6o,K7s,K7o,Q8s,Q8o,J9s,J9o,A5s,A5o,K6s,K6o,Q7s,Q7o,66,J8s,J8o,T9s,T9o,A4s,A4o,K5s,K5o,Q6s,Q6o,J7s,J7o,T8s,T8o,A3s,A3o,K4s,K4o,Q5s,Q5o,J6s,J6o,T7s,T7o,98s,98o,A2s,A2o,K3s,K3o,Q4s,Q4o,J5s,J5o,55,T6s,T6o,97s,97o,K2s,K2o,Q3s,Q3o,J4s,J4o,T5s,T5o,96s,96o,87s,87o,Q2s,Q2o,J3s,J3o,T4s,T4o,95s,95o,86s,86o,44,J2s,J2o,T3s,T3o,94s,94o,85s,85o,76s,76o,T2s,T2o,93s,93o,84s,84o,75s,75o,92s,92o,83s,83o,74s,74o,65s,65o,82s,82o,73s,73o,64s,64o,72s,72o,63s,63o,54s,54o,33,62s,62o,53s,53o,52s,52o,43s,43o,42s,42o,32s,32o,22";
CString handrank_DO = "AA,KK,QQ,JJ,TT,99,88,AKs,AKo,77,AQs,AQo,AJs,AJo,KQs,KQo,ATs,ATo,KJs,KJo,66,A9s,KTs,A9o,QJs,KTo,QJo,A8s,A8o,K9s,QTs,K9o,QTo,A7s,A7o,K8s,Q9s,JTs,K8o,Q9o,JTo,A6s,A6o,K7s,Q8s,J9s,K7o,Q8o,J9o,55,A5s,A5o,K6s,K6o,Q7s,J8s,T9s,Q7o,T9o,J8o,A4s,A4o,K5s,Q6s,K5o,J7s,T8s,Q6o,T8o,J7o,A3s,A3o,K4s,Q5s,K4o,98s,J6s,T7s,Q5o,98o,T7o,J6o,A2s,A2o,K3s,Q4s,K3o,J5s,Q4o,97s,T6s,J5o,97o,T6o,44,K2s,Q3s,K2o,J4s,Q3o,87s,96s,T5s,J4o,87o,96o,T5o,Q2s,J3s,Q2o,T4s,86s,95s,J3o,T4o,86o,95o,J2s,T3s,76s,85s,J2o,94s,T3o,76o,85o,94o,T2s,75s,93s,84s,T2o,75o,93o,84o,65s,92s,74s,83s,65o,92o,74o,83o,33,64s,82s,73s,64o,82o,73o,54s,63s,54o,72s,63o,72o,53s,62s,53o,62o,43s,52s,43o,52o,42s,42o,22,32s,32o";
CString handrank_HG = "AA,KK,QQ,AKs,AKo,JJ,TT,99,88,77,AQs,AQo,AJs,ATs,A9s,A8s,KQs,KQo,A7s,A5s,A6s,A4s,A3s,A2s,QJs,JTs,T9s,98s,87s,76s,65s,AJo,66,ATo,KJs,KTs,A9o,KJo,55,K9s,A8o,KTo,QTs,A7o,K8s,QJo,K9o,A5o,A6o,Q9s,K7s,QTo,44,A4o,K6s,K8o,Q8s,A3o,K5s,J9s,Q9o,JTo,K7o,A2o,K4s,Q7s,K6o,K3s,J8s,33,Q6s,Q8o,K5o,J9o,K2s,Q5s,T8s,K4o,J7s,Q4s,Q7o,T9o,J8o,K3o,Q6o,Q3s,T7s,J6s,K2o,22,Q2s,Q5o,J5s,T8o,J7o,Q4o,97s,J4s,T6s,J3s,Q3o,98o,T7o,J6o,96s,J2s,Q2o,T5s,J5o,T4s,97o,86s,J4o,T6o,95s,T3s,J3o,87o,T2s,85s,96o,J2o,T5o,94s,75s,T4o,93s,86o,84s,95o,T3o,92s,76o,74s,T2o,54s,85o,64s,83s,94o,75o,82s,73s,93o,65o,53s,63s,84o,92o,43s,74o,72s,54o,64o,52s,62s,83o,42s,82o,73o,53o,63o,32s,43o,72o,52o,62o,42o,32o";
CString handrank_HC = "AA,AKs,AKo,AQs,AQo,AJs,AJo,ATs,ATo,A9s,A9o,A8s,A8o,A7s,A7o,A6s,A6o,A5s,A5o,A4s,A4o,A3s,A3o,A2s,A2o,KK,KQs,KQo,KJs,KJo,KTs,KTo,K9s,K9o,K8s,K8o,K7s,K7o,K6s,K6o,K5s,K5o,K4s,K4o,K3s,K3o,K2s,K2o,QQ,QJs,QJo,QTs,QTo,Q9s,Q9o,Q8s,Q8o,Q7s,Q7o,Q6s,Q6o,Q5s,Q5o,Q4s,Q4o,Q3s,Q3o,Q2s,Q2o,JJ,JTs,JTo,J9s,J9o,J8s,J8o,J7s,J7o,J6s,J6o,J5s,J5o,J4s,J4o,J3s,J3o,J2s,J2o,TT,T9s,T9o,T8s,T8o,T7s,T7o,T6s,T6o,T5s,T5o,T4s,T4o,T3s,T3o,T2s,T2o,99,98s,98o,97s,97o,96s,96o,95s,95o,94s,94o,93s,93o,92s,92o,88,87s,87o,86s,86o,85s,85o,84s,84o,83s,83o,82s,82o,77,76s,76o,75s,75o,74s,74o,73s,73o,72s,72o,66,65s,65o,64s,64o,63s,63o,62s,62o,55,54s,54o,53s,53o,52s,52o,44,43s,43o,42s,42o,33,32s,32o,22";
CString handrank_HCPF = "AA,KK,QQ,JJ,TT,99,88,77,66,55,44,33,22,AKs,AKo,AQs,AQo,AJs,AJo,ATs,ATo,A9s,A9o,A8s,A8o,A7s,A7o,A6s,A6o,A5s,A5o,A4s,A4o,A3s,A3o,A2s,A2o,KQs,KQo,KJs,KJo,KTs,KTo,K9s,K9o,K8s,K8o,K7s,K7o,K6s,K6o,K5s,K5o,K4s,K4o,K3s,K3o,K2s,K2o,QJs,QJo,QTs,QTo,Q9s,Q9o,Q8s,Q8o,Q7s,Q7o,Q6s,Q6o,Q5s,Q5o,Q4s,Q4o,Q3s,Q3o,Q2s,Q2o,JTs,JTo,J9s,J9o,J8s,J8o,J7s,J7o,J6s,J6o,J5s,J5o,J4s,J4o,J3s,J3o,J2s,J2o,T9s,T9o,T8s,T8o,T7s,T7o,T6s,T6o,T5s,T5o,T4s,T4o,T3s,T3o,T2s,T2o,98s,98o,97s,97o,96s,96o,95s,95o,94s,94o,93s,93o,92s,92o,87s,87o,86s,86o,85s,85o,84s,84o,83s,83o,82s,82o,76s,76o,75s,75o,74s,74o,73s,73o,72s,72o,65s,65o,64s,64o,63s,63o,62s,62o,54s,54o,53s,53o,52s,52o,43s,43o,42s,42o,32s,32o";
CString handrank_NF = "AA,KK,QQ,AKs,JJ,AQs,KQs,AJs,KJs,TT,AKo,ATs,QJs,KTs,QTs,JTs,99,AQo,A9s,KQo,88,K9s,T9s,A8s,Q9s,J9s,AJo,A5s,77,A7s,KJo,A4s,A3s,A6s,QJo,66,K8s,T8s,A2s,98s,J8s,ATo,Q8s,K7s,KTo,55,JTo,87s,QTo,44,22,33,K6s,97s,K5s,76s,T7s,K4s,K2s,K3s,Q7s,86s,65s,J7s,54s,Q6s,75s,96s,Q5s,64s,Q4s,Q3s,T9o,T6s,Q2s,A9o,53s,85s,J6s,J9o,K9o,J5s,Q9o,43s,74s,J4s,J3s,95s,J2s,63s,A8o,52s,T5s,84s,T4s,T3s,42s,T2s,98o,T8o,A5o,A7o,73s,A4o,32s,94s,93s,J8o,A3o,62s,92s,K8o,A6o,87o,Q8o,83s,A2o,82s,97o,72s,76o,K7o,65o,T7o,K6o,86o,54o,K5o,J7o,75o,Q7o,K4o,K3o,96o,K2o,64o,Q6o,53o,85o,T6o,Q5o,43o,Q4o,Q3o,74o,Q2o,J6o,63o,J5o,95o,52o,J4o,J3o,42o,J2o,84o,T5o,T4o,32o,T3o,73o,T2o,62o,94o,93o,92o,83o,82o,72o";
CString handrank_RPS = "AA,KK,QQ,JJ,AKs,AQs,TT,AKo,AJs,KQs,99,ATs,AQo,KJs,88,QJs,KTs,AJo,A9s,QTs,KQo,77,JTs,A8s,K9s,ATo,A5s,A7s,66,KJo,A4s,Q9s,T9s,J9s,A6s,QJo,55,A3s,KTo,K8s,A2s,98s,T8s,K7s,Q8s,87s,QTo,76s,JTo,44,A9o,J8s,K6s,97s,K5s,K4s,T7s,Q7s,K9o,65s,86s,A8o,J7s,33,K2s,Q9o,J9o,T9o,54s,Q6s,K3s,96s,75s,22,64s,T8o,Q5s,A7o,T7o,Q4s,J8o,98o,97o,J6s,85o,T6s,76o,Q8o,J5s,T6o,Q3s,75o,J4s,74s,K8o,86o,53s,K7o,85s,63s,T3o,63o,K6o,J6o,96o,92o,72o,52o,A6o,T2o,95s,84o,62o,T5s,95o,A5o,Q7o,T5o,87o,83o,65o,Q2s,94o,74o,A4o,T4o,82o,64o,42o,J7o,93o,73o,53o,A3o,Q5o,J2o,84s,Q4o,K5o,J5o,43s,Q3o,43o,K4o,J4o,T4s,54o,Q6o,Q2o,J3s,J3o,T3s,J2s,92s,52s,K2o,T2s,62s,32o,82s,42s,93s,73s,K3o,72s,A2o,83s,94s,32s";
CString handrank_SG = "AA,KK,QQ,JJ,TT,99,88,AKs,AQs,77,AJs,AKo,ATs,AQo,AJo,A9s,KQs,66,ATo,A8s,KJs,A7s,A9o,KTs,KQo,A6s,A8o,KJo,55,QJs,K9s,A5s,A7o,KTo,QTs,A4s,K8s,A6o,A3s,QJo,K9o,A5o,Q9s,K7s,JTs,A2s,QTo,44,A4o,K6s,K8o,Q8s,A3o,K5s,J9s,Q9o,K7o,Q7s,JTo,A2o,K4s,K6o,J8s,Q6s,Q8o,K3s,T9s,33,K5o,J9o,Q5s,J7s,Q7o,K2s,T8s,K4o,Q4s,J8o,Q6o,J6s,T9o,K3o,Q3s,T7s,Q5o,J5s,J7o,98s,K2o,22,Q2s,T8o,Q4o,J4s,T6s,J6o,97s,J3s,Q3o,T7o,T5s,J5o,98o,96s,J2s,Q2o,T4s,J4o,T6o,87s,97o,95s,T3s,J3o,T5o,86s,T2s,96o,J2o,94s,T4o,87o,85s,93s,95o,T3o,76s,86o,84s,92s,T2o,94o,75s,85o,83s,93o,76o,74s,82s,84o,92o,65s,75o,73s,83o,64s,74o,72s,82o,65o,63s,73o,54s,64o,62s,72o,53s,63o,54o,52s,62o,43s,53o,42s,52o,43o,32s,42o,32o";
CString handrank_SCG = "AA,KK,QQ,JJ,TT,99,88,AKs,77,AQs,AJs,AKo,ATs,AQo,AJo,KQs,66,KQo,55,44,33,22,A9s,ATo,KJs,QJs,JTs,A8s,KTs,A7s,KJo,A6s,KTo,QTs,QJo,QTo,JTo,T9s,98s,A9o,A5s,A8o,A4s,A7o,A3s,A5o,A6o,Q9s,A2s,J9s,T8s,97s,87s,76s,65s,K9s,K8s,K9o,K7s,A4o,K6s,K8o,Q8s,A3o,K5s,Q9o,K7o,A2o,K4s,Q7s,K6o,K3s,J8s,Q6s,Q8o,K5o,J9o,K2s,Q5s,K4o,J7s,Q4s,Q7o,T9o,J8o,K3o,Q6o,Q3s,T7s,J6s,K2o,Q2s,Q5o,J5s,T8o,J7o,Q4o,J4s,T6s,J3s,Q3o,98o,T7o,J6o,96s,J2s,Q2o,T5s,J5o,T4s,97o,86s,J4o,T6o,95s,T3s,J3o,87o,T2s,85s,96o,J2o,T5o,94s,75s,T4o,93s,86o,84s,95o,T3o,92s,76o,74s,T2o,54s,85o,64s,83s,94o,75o,82s,73s,93o,65o,53s,63s,84o,92o,43s,74o,72s,54o,64o,52s,62s,83o,42s,82o,73o,53o,63o,32s,43o,72o,52o,62o,42o,32o";
CString handrank_TG = "AA,KK,QQ,JJ,AKs,TT,AQs,AJs,AKo,KQs,99,ATs,AQo,KJs,KTs,QJs,88,AJo,A9s,ATo,A8s,KQo,QTs,JTs,77,A7s,KJo,K9s,KTo,QJo,Q9s,QTo,J9s,JTo,T9s,66,55,A5s,A6s,A4s,A3s,A2s,44,33,22,A9o,A8o,A7o,K8s,K9o,A5o,A6o,K7s,A4o,K6s,K8o,Q8s,A3o,K5s,Q9o,K7o,A2o,K4s,Q7s,K6o,K3s,J8s,Q6s,Q8o,K5o,J9o,K2s,Q5s,T8s,K4o,J7s,Q4s,Q7o,T9o,J8o,K3o,Q6o,Q3s,98s,T7s,J6s,K2o,Q2s,Q5o,J5s,T8o,J7o,Q4o,97s,J4s,T6s,J3s,Q3o,98o,87s,T7o,J6o,96s,J2s,Q2o,T5s,J5o,T4s,97o,86s,J4o,T6o,95s,T3s,76s,J3o,87o,T2s,85s,96o,J2o,T5o,94s,75s,T4o,93s,86o,65s,84s,95o,T3o,92s,76o,74s,T2o,54s,85o,64s,83s,94o,75o,82s,73s,93o,65o,53s,63s,84o,92o,43s,74o,72s,54o,64o,52s,62s,83o,42s,82o,73o,53o,63o,32s,43o,72o,52o,62o,42o,32o";
CString handrank_TGO = "AA,KK,QQ,JJ,TT,99,88,AKs,77,AQs,AJs,AKo,ATs,AQo,AJo,KQs,66,A9s,ATo,KJs,A8s,KTs,KQo,A7s,KJo,55,QJs,K9s,A5s,A6s,KTo,QTs,A4s,A3s,QJo,Q9s,JTs,A2s,QTo,44,J9s,JTo,T9s,33,22,A9o,A8o,A7o,K8s,K9o,A5o,A6o,K7s,A4o,K6s,K8o,Q8s,A3o,K5s,Q9o,K7o,A2o,K4s,Q7s,K6o,K3s,J8s,Q6s,Q8o,K5o,J9o,K2s,Q5s,T8s,K4o,J7s,Q4s,Q7o,T9o,J8o,K3o,Q6o,Q3s,98s,T7s,J6s,K2o,Q2s,Q5o,J5s,T8o,J7o,Q4o,97s,J4s,T6s,J3s,Q3o,98o,87s,T7o,J6o,96s,J2s,Q2o,T5s,J5o,T4s,97o,86s,J4o,T6o,95s,T3s,76s,J3o,87o,T2s,85s,96o,J2o,T5o,94s,75s,T4o,93s,86o,65s,84s,95o,T3o,92s,76o,74s,T2o,54s,85o,64s,83s,94o,75o,82s,73s,93o,65o,53s,63s,84o,92o,43s,74o,72s,54o,64o,52s,62s,83o,42s,82o,73o,53o,63o,32s,43o,72o,52o,62o,42o,32o";


CSymbolEngineRange::CSymbolEngineRange() {
	// The values of some symbol-engines depend on other engines.
	// As the engines get later called in the order of initialization
	// we assure correct ordering by checking if they are initialized.
	//
	// This engine does not use any other engines.
	prw1326 = p_iterator_thread->prw1326();
	_range_text = "";
}

CSymbolEngineRange::~CSymbolEngineRange() {
}

void CSymbolEngineRange::InitOnStartup() {
	UpdateOnConnection();
}

void CSymbolEngineRange::UpdateOnConnection() {
	UpdateOnHandreset();
}

void CSymbolEngineRange::UpdateOnHandreset() {
	ClearAll();
	UnSetLists();
	// For testing purpose only, do not uncomment in production
	//prw1326->useme = 1326;
	//prw1326->preflop = 1326;
}

void CSymbolEngineRange::UpdateOnNewRound() {
}

void CSymbolEngineRange::UpdateOnMyTurn() {
}

void CSymbolEngineRange::UpdateOnHeartbeat() {
}

void CSymbolEngineRange::ClearAll() {
	for (int i = 0; i < kMaxNumberOfPlayers; ++i) {
		_range[i] = "";
		for (int j = 0; j < k_number_of_ranks_per_deck; ++j) {
			for (int k = 0; k < k_number_of_ranks_per_deck; ++k) {
				_range_matrix[i][j][k] = false;
				_range_weight_matrix[i][j][k] = 100;
			}
		}
	}
}

// First_rank, second_rank  don't matter for suitedness here.
// Suit gets controlled by parameter 3: "suited"
void CSymbolEngineRange::Set(int first_rank, int second_rank, bool suited, int weight) {
	AssertRange(first_rank, 2, k_rank_ace);
	AssertRange(second_rank, 2, k_rank_ace);
	// Higher rank first
	if (first_rank < second_rank) {
		SwapInts(&first_rank, &second_rank);
	}
	if (suited) {
		Set(first_rank, second_rank, weight);
	}
	else {
		Set(second_rank, first_rank, weight);
	}
}

// first_rank  > second_rank -> suited
// second_rank > first_rank  -> offsuited
void CSymbolEngineRange::Set(int first_rank, int second_rank, int weight) {
	AssertRange(first_rank, 2, k_rank_ace);
	AssertRange(second_rank, 2, k_rank_ace);
	// Normating to [0..12]
	first_rank -= 2;
	second_rank -= 2;
	// Needs to be [second_rank][first_rank]
	// for correct view in handlist-editor,
	// otherwise suited/offsuited would be confused
	// and we need a single point to get it right
	_range_matrix[_chairnumber][second_rank][first_rank] = true;
	_range_weight_matrix[_chairnumber][second_rank][first_rank] = weight;
}

// first_rank  > second_rank -> suited
// second_rank > first_rank  -> offsuited
void CSymbolEngineRange::Unset(int first_rank, int second_rank, bool suited) {
	AssertRange(first_rank, 2, k_rank_ace);
	AssertRange(second_rank, 2, k_rank_ace);
	// Normating to [0..12]
	first_rank -= 2;
	second_rank -= 2;
	// Needs to be [second_rank][first_rank] if suited
	// for correct view in handlist-editor,
	// otherwise suited/offsuited would be confused
	// and we need a single point to get it right
	if (suited)
		SwapInts(&first_rank, &second_rank);
	_range_matrix[_chairnumber][first_rank][second_rank] = false;
	_range_weight_matrix[_chairnumber][first_rank][second_rank] = 100;
}

void CSymbolEngineRange::UnSetLists() {
	for (int i = 0; i < 10; i++) {
		prw1326->chair[i].limit = 0;
		prw1326->chair[i].level = 100;
		prw1326->chair[i].ignore = 0;

		for (int ia = 0; ia < 1326; ia++) {
			prw1326->chair[i].ranklo[ia] = 0;
			prw1326->chair[i].rankhi[ia] = 0;
			prw1326->chair[i].weight[ia] = 100;
		}
	}
}

void CSymbolEngineRange::UnSetChairList(int chairnumber) {
	prw1326->chair[chairnumber].limit = 0;
	prw1326->chair[chairnumber].level = 100;
	prw1326->chair[chairnumber].ignore = 0;

	for (int ia = 0; ia < 1326; ia++) {
		prw1326->chair[chairnumber].ranklo[ia] = 0;
		prw1326->chair[chairnumber].rankhi[ia] = 0;
		prw1326->chair[chairnumber].weight[ia] = 100;
	}
	_range[chairnumber] = "";
	for (int j = 0; j < k_number_of_ranks_per_deck; ++j) {
		for (int k = 0; k < k_number_of_ranks_per_deck; ++k) {
			_range_matrix[chairnumber][j][k] = false;
			_range_weight_matrix[chairnumber][j][k] = 100;
		}
	}
}

void CSymbolEngineRange::cards_removal(int chairnumber, int r1, int r2, int s1, int s2)
{
	// Normating to [0..12]
	r1 -= 2;
	r2 -= 2;
	int c1 = r1 + s1;
	int c2 = r2 + s2;
	int limit = prw1326->chair[chairnumber].limit;

	for (int p = 0; p < limit; p++)
	{
		if (prw1326->chair[chairnumber].ranklo[p] == c1 || prw1326->chair[chairnumber].rankhi[p] == c1) {
			for (int i = p; i < limit; i++)
			{
				if (i == limit - 1) {
					prw1326->chair[chairnumber].ranklo[i] = 0;
					prw1326->chair[chairnumber].rankhi[i] = 0;
					prw1326->chair[chairnumber].weight[i] = prw1326->chair[chairnumber].level;
				}
				else {
					prw1326->chair[chairnumber].ranklo[i] = prw1326->chair[chairnumber].ranklo[i + 1];
					prw1326->chair[chairnumber].rankhi[i] = prw1326->chair[chairnumber].rankhi[i + 1];
					prw1326->chair[chairnumber].weight[i] = prw1326->chair[chairnumber].weight[i + 1];
				}
			}
			limit = prw1326->chair[chairnumber].limit--;
			p--;
		}
	}

	for (int p = 0; p < limit; p++)
	{
		if (prw1326->chair[chairnumber].ranklo[p] == c2 || prw1326->chair[chairnumber].rankhi[p] == c2) {
			for (int i = p; i < limit; i++)
			{
				if (i == limit - 1) {
					prw1326->chair[chairnumber].ranklo[i] = 0;
					prw1326->chair[chairnumber].rankhi[i] = 0;
					prw1326->chair[chairnumber].weight[i] = prw1326->chair[chairnumber].level;
				}
				else {
					prw1326->chair[chairnumber].ranklo[i] = prw1326->chair[chairnumber].ranklo[i + 1];
					prw1326->chair[chairnumber].rankhi[i] = prw1326->chair[chairnumber].rankhi[i + 1];
					prw1326->chair[chairnumber].weight[i] = prw1326->chair[chairnumber].weight[i + 1];
				}
			}
			limit = prw1326->chair[chairnumber].limit--;
			p--;
		}
	}
}

void CSymbolEngineRange::del_single_hand(int chairnumber, int r1, int r2, int s1, int s2)
{
	// Normating to [0..12]
	r1 -= 2;
	r2 -= 2;
	int c1 = r1 + s1;
	int c2 = r2 + s2;
	int limit = prw1326->chair[chairnumber].limit;
	int found_num = -1;

	for (int p = 0; p < limit; p++)
	{
		if (prw1326->chair[chairnumber].ranklo[p] == c1 && prw1326->chair[chairnumber].rankhi[p] == c2 || prw1326->chair[chairnumber].ranklo[p] == c2 && prw1326->chair[chairnumber].rankhi[p] == c1)
		{
			found_num = p;
			break;
		}
	}
	for (int i = found_num; i < limit; i++)
	{
		if (i == limit - 1) {
			prw1326->chair[chairnumber].ranklo[i] = 0;
			prw1326->chair[chairnumber].rankhi[i] = 0;
			prw1326->chair[chairnumber].weight[i] = prw1326->chair[chairnumber].level;
		}
		else {
			prw1326->chair[chairnumber].ranklo[i] = prw1326->chair[chairnumber].ranklo[i + 1];
			prw1326->chair[chairnumber].rankhi[i] = prw1326->chair[chairnumber].rankhi[i + 1];
			prw1326->chair[chairnumber].weight[i] = prw1326->chair[chairnumber].weight[i + 1];
		}
	}

	prw1326->chair[chairnumber].limit--;
}

void CSymbolEngineRange::del_pocketpair_hand(int chairnumber, int r1)
{
	/*
	remove pocket pair
	*/
	for (int i = 0; i<3; i++) for (int j = i + 1; j<4; j++)
		del_single_hand(chairnumber, r1, r1, suit[i], suit[j]);
}

void CSymbolEngineRange::del_offsuited_hand(int chairnumber, int r1, int r2)
{
	/*
	remove off suited hand
	*/
	for (int i = 0; i<4; i++) for (int j = 0; j<4; j++) if (j != i)
		del_single_hand(chairnumber, r1, r2, suit[i], suit[j]);
}

void CSymbolEngineRange::del_suited_hand(int chairnumber, int r1, int r2)
{
	/*
	remove suited hand
	*/
	for (int i = 0; i<4; i++)
		del_single_hand(chairnumber, r1, r2, suit[i], suit[i]);
}

void CSymbolEngineRange::add_single_hand(int chairnumber, int r1, int r2, int s1, int s2, int weight)
{
	// Normating to [0..12]
	r1 -= 2;
	r2 -= 2;
	int c1 = r1 + s1;
	int c2 = r2 + s2;
	int lim = prw1326->chair[chairnumber].limit;

	if (r1 < r2)
	{
		prw1326->chair[chairnumber].ranklo[lim] = c1;
		prw1326->chair[chairnumber].rankhi[lim] = c2;
	}
	else
	{
		prw1326->chair[chairnumber].ranklo[lim] = c2;
		prw1326->chair[chairnumber].rankhi[lim] = c1;
	}

	prw1326->chair[chairnumber].weight[lim] = weight;
	prw1326->chair[chairnumber].limit++;
}

void CSymbolEngineRange::add_pocketpair_hand(int chairnumber, int r1, int weight)
{
	/*
	add pocket pair
	*/
	for (int i = 0; i<3; i++) for (int j = i + 1; j<4; j++)
		add_single_hand(chairnumber, r1, r1, suit[i], suit[j], weight);
}

void CSymbolEngineRange::add_offsuited_hand(int chairnumber, int r1, int r2, int weight)
{
	/*
	add off suited hand
	*/
	for (int i = 0; i<4; i++) for (int j = 0; j<4; j++) if (j != i)
		add_single_hand(chairnumber, r1, r2, suit[i], suit[j], weight);
}

void CSymbolEngineRange::add_suited_hand(int chairnumber, int r1, int r2, int weight)
{
	/*
	add suited hand
	*/
	for (int i = 0; i<4; i++)
		add_single_hand(chairnumber, r1, r2, suit[i], suit[i], weight);
}

void CSymbolEngineRange::weight_single_hand(int chairnumber, int r1, int r2, int s1, int s2, int old_weight, int new_weight) {
	/*
	changes weight for specified hand with old_weight to new_weight
	*/
	// Normating to [0..12]
	r1 -= 2;
	r2 -= 2;
	int c1 = r1 + s1;
	int c2 = r2 + s2;
	int found_num = -1;

	for (int p = 0; p < prw1326->chair[chairnumber].limit; p++)
	{
		if (prw1326->chair[chairnumber].ranklo[p] == c1 && prw1326->chair[chairnumber].rankhi[p] == c2 || prw1326->chair[chairnumber].ranklo[p] == c2 && prw1326->chair[chairnumber].rankhi[p] == c1)
		{
			found_num = p;
			break;
		}
	}

	if (prw1326->chair[chairnumber].weight[found_num] == old_weight)	prw1326->chair[chairnumber].weight[found_num] = new_weight;
}

void CSymbolEngineRange::weight_pocketpair_hand(int chairnumber, int r1, int old_weight, int new_weight)
{
	/*
	weight pocket pair
	*/

	for (int p = 0; p<3; p++) for (int k = p + 1; k<4; k++)	weight_single_hand(chairnumber, r1, r1, suit[p], suit[k], old_weight, new_weight);
}

void CSymbolEngineRange::weight_offsuited_hand(int chairnumber, int r1, int r2, int old_weight, int new_weight)
{
	/*
	weight off suit hand
	*/

	for (int p = 0; p<4; p++) for (int k = 0; k<4; k++) if (k != p)
		weight_single_hand(chairnumber, r1, r2, suit[p], suit[k], old_weight, new_weight);
}

void CSymbolEngineRange::weight_suited_hand(int chairnumber, int r1, int r2, int old_weight, int new_weight)
{
	/*
	weight suited hand
	*/

	for (int k = 0; k<4; k++) weight_single_hand(chairnumber, r1, r2, suit[k], suit[k], old_weight, new_weight);
}

void CSymbolEngineRange::weight_all_hand(int chairnumber, int old_weight, int new_weight) {
	/*
	changes all hands with old_weight to new_weight
	*/
	int max_card = prw1326->chair[chairnumber].limit;

	for (int z = 0; z < max_card; z++)
	{
		int r1 = prw1326->chair[chairnumber].ranklo[z] % 13;
		int r2 = prw1326->chair[chairnumber].rankhi[z] % 13;
		// Normating to [2..14]
		r1 += 2;
		r2 += 2;
		int s1 = prw1326->chair[chairnumber].ranklo[z] - prw1326->chair[chairnumber].ranklo[z] % 13;
		int s2 = prw1326->chair[chairnumber].rankhi[z] - prw1326->chair[chairnumber].rankhi[z] % 13;

		weight_single_hand(chairnumber, r1, r2, s1, s2, old_weight, new_weight);
	}
}

bool CSymbolEngineRange::SetRange(CString range_symbol, CString range_value) {
	_chairnumber = RightDigitCharacterToNumber(range_symbol);

	int position = 0;
	CString separator = _T(", \n\r");
	CString sep_operator = _T("<>=");
	CString token = "null";
	int command = kSetRange;
	int ioperator = kNoOperator;
	bool weight_all = false;
	double globalweight = 100;
	double oldweight = kUndefined, newweight = kUndefined;
	CString str_globalweight, str_range_value, str_weight, str_limit1, str_limit2, handrank;;
	int limit1 = kUndefined, limit2 = kUndefined;
	bool twolimits = false, bglobalweight = false;
	double operator_limit;
	int handcount = 0;

	if (prw1326->useme != 1326) {
		ErrorPrw1326NotActivated();
		return false;
	}
	if (p_betround_calculator->betround() == kBetroundPreflop && prw1326->preflop != 1326) {
		ErrorPrw1326PreflopNotActivated();
		return false;
	}

	int nopponents = p_engine_container->symbol_engine_prwin()->nopponents_for_prwin();
	if (nopponents < 1) nopponents = 1;
	CString handrank169 = handrank169_table[nopponents - 1];

	range_value = range_value.Trim();	
	CString range_lower = range_value;						// Syntax
	if (range_lower.Left(1).MakeLower() == "%")				// SET rangeX "%20" or "%10-40" or "%20/Y" (where Y is a user defined "list" or internal specific handranking, see appendixes)
		command = kSetPercentileRange;
	if (range_lower.Left(3).MakeLower() == "del")			// SET rangeX "del listxyz" (or "del AA:100 KK:94 AKs:88 ...")	
		command = KRemoveRange;
	if (range_lower.MakeLower() == "del_pp")				// SET rangeX "del_pp"
		command = kRemovePocketPair;
	if (range_lower.MakeLower() == "del_os")				// SET rangeX "del_os"
		command = kRemoveOffsuited;
	if (range_lower.MakeLower() == "del_s")					// SET rangeX "del_s"
		command = kRemoveSuited;
	if (range_lower.Left(6).MakeLower() == "weight")		// SET rangeX "weight listabc" (or "weight AA:85 KK:78 AKs:71 ...")
		command = kChangeWeight;
	if (range_lower.Left(10).MakeLower() == "weight_all")	// SET rangeX "weight_all /84 /96" (/oldweight /newweight)
		command = kChangeAllWeight;

	switch (command) {
	case kSetRange:
		UnSetChairList(_chairnumber);
		break;
	case KRemoveRange:
		range_value = range_value.Mid(3).Trim();
		break;
	case kChangeWeight:
		range_value = range_value.Mid(6).Trim();
		break;
	case kChangeAllWeight:
		str_range_value = range_value;
		range_value = range_value.Mid(10).Trim();
		str_weight = range_value.Tokenize("/", position).Trim();
		if (str_weight == "" || !isNumber(str_weight)) {
			ErrorInvalidMember(str_range_value);
			return false;
		}
		oldweight = round(StringToNumber(str_weight));
		str_weight = range_value.Tokenize("/", position).Trim();
		if (str_weight == "" || !isNumber(str_weight)) {
			ErrorInvalidMember(str_range_value);
			return false;
		}
		newweight = round(StringToNumber(str_weight));
		range_value = _range[_chairnumber];
		range_value.Replace(Number2CString(oldweight), Number2CString(newweight));
		command = kChangeWeight;
		weight_all = true;
		break;
	case kSetPercentileRange:
		UnSetChairList(_chairnumber);
		int pos = 0;
		str_range_value = range_value;
		range_value = range_value.Mid(1).Trim();
		if (range_value.GetAt(0) == '/' || range_value.GetAt(0) == ':') {
			ErrorInvalidMember(str_range_value);
			return false;
		}
		str_limit1 = range_value.Tokenize("-", position).Trim();
		if (str_limit1.Find("/") != -1) {
			handrank = str_limit1;
			str_limit1 = str_limit1.Tokenize("/", pos).Trim();
			handrank = handrank.Mid(pos).Trim();
			if (handrank.Find(":") != -1) {
				pos = 0;
				bglobalweight = true;
				str_globalweight = handrank;
				if (handrank.GetAt(pos) == ':') {
					ErrorInvalidMember(str_range_value);
					return false;
				}
				handrank = handrank.Tokenize(":", pos).Trim();
				str_globalweight = str_globalweight.Mid(pos).Trim();
				if (str_globalweight == "" || !isNumber(str_globalweight)) {
					ErrorInvalidMember(str_range_value);
					return false;
				}
				globalweight = round(StringToNumber(str_globalweight));
				if ((globalweight < 0) || (globalweight > 100)) {
					ErrorInvalidMember(str_range_value);
					return false;
				}
			}
		}
		if (str_limit1.Find(":") != -1) {
			pos = 0;
			bglobalweight = true;
			str_globalweight = str_limit1;
			if (str_limit1.GetAt(pos) == ':') {
				ErrorInvalidMember(str_range_value);
				return false;
			}
			str_limit1 = str_limit1.Tokenize(":", pos).Trim();
			str_globalweight = str_globalweight.Mid(pos).Trim();
			if (str_globalweight == "" || !isNumber(str_globalweight)) {
				ErrorInvalidMember(str_range_value);
				return false;
			}
			globalweight = round(StringToNumber(str_globalweight));
			if ((globalweight < 0) || (globalweight > 100)) {
				ErrorInvalidMember(str_range_value);
				return false;
			}
		}
		if (str_limit1.Left(2) == "f$") {
			CFunction *function = (CFunction*)p_function_collection->LookUp(str_limit1);
			if (function == NULL) {
				// Function not found
				CString error_message;
				error_message.Format("Function %s not found or mispelled\n"
					"Please set a valid function name.", str_limit1);
				CParseErrors::Error(error_message);
				return false;
			}
			limit1 = round(169 * function->Evaluate(str_limit1) / 100);
		}
		else if (str_limit1.Left(6) == "me_re_") {
			p_engine_container->symbol_engine_memory_symbols()->EvaluateSymbol(str_limit1, &operator_limit);
			if (operator_limit < 0) {
				// Memory symbol not found
				CString error_message;
				error_message.Format("Memory symbol %s not found or mispelled\n"
					"Please set a valid memory symbol.", str_limit1);
				CParseErrors::Error(error_message);
				return false;
			}
			limit1 = round(169 * operator_limit / 100);
		}
		else if (str_limit1 == "" || !isNumber(str_limit1)) {
			ErrorInvalidMember(str_range_value);
			return false;
		}
		if (range_value.Find("-") != -1)  twolimits = true;
		str_limit2 = range_value.Tokenize("-", position).Trim();
		if (str_limit2.Find("/") != -1) {
			pos = 0;
			handrank = str_limit2;
			str_limit2 = str_limit2.Tokenize("/", pos).Trim();
			if (str_limit2.Left(2) == "f$") {
				CFunction *function = (CFunction*)p_function_collection->LookUp(str_limit2);
				if (function == NULL) {
					// Function not found
					CString error_message;
					error_message.Format("Function %s not found or mispelled\n"
						"Please set a valid function name.", str_limit2);
					CParseErrors::Error(error_message);
					return false;
				}
				limit2 = round(169 * function->Evaluate(str_limit2) / 100);
			}
			else if (str_limit2.Left(6) == "me_re_") {
				p_engine_container->symbol_engine_memory_symbols()->EvaluateSymbol(str_limit2, &operator_limit);
				if (operator_limit < 0) {
					// Memory symbol not found
					CString error_message;
					error_message.Format("Memory symbol %s not found or mispelled\n"
						"Please set a valid memory symbol.", str_limit2);
					CParseErrors::Error(error_message);
					return false;
				}
				limit2 = round(169 * operator_limit / 100);
			}
			else if (twolimits && (str_limit2 == "" || !isNumber(str_limit2))) {
				ErrorInvalidMember(str_range_value);
				return false;
			}
			handrank = handrank.Mid(pos).Trim();
			if (handrank.Find(":") != -1) {
				pos = 0;
				bglobalweight = true;
				str_globalweight = handrank;
				if (handrank.GetAt(pos) == ':') {
					ErrorInvalidMember(str_range_value);
					return false;
				}
				handrank = handrank.Tokenize(":", pos).Trim();
				str_globalweight = str_globalweight.Mid(pos).Trim();
				if (str_globalweight == "" || !isNumber(str_globalweight)) {
					ErrorInvalidMember(str_range_value);
					return false;
				}
				globalweight = round(StringToNumber(str_globalweight));
				if ((globalweight < 0) || (globalweight > 100)) {
					ErrorInvalidMember(str_range_value);
					return false;
				}
			}
		}
		if (str_limit2.Find(":") != -1) {
			pos = 0;
			bglobalweight = true;
			str_globalweight = str_limit2;
			if (str_limit2.GetAt(pos) == ':') {
				ErrorInvalidMember(str_range_value);
				return false;
			}
			str_limit2 = str_limit2.Tokenize(":", pos).Trim();
			if (str_limit2.Left(2) == "f$") {
				CFunction *function = (CFunction*)p_function_collection->LookUp(str_limit2);
				if (function == NULL) {
					// Function not found
					CString error_message;
					error_message.Format("Function %s not found or mispelled\n"
						"Please set a valid function name.", str_limit2);
					CParseErrors::Error(error_message);
					return false;
				}
				limit2 = round(169 * function->Evaluate(str_limit2) / 100);
			}
			else if (str_limit2.Left(6) == "me_re_") {
				p_engine_container->symbol_engine_memory_symbols()->EvaluateSymbol(str_limit2, &operator_limit);
				if (operator_limit < 0) {
					// Memory symbol not found
					CString error_message;
					error_message.Format("Memory symbol %s not found or mispelled\n"
						"Please set a valid memory symbol.", str_limit2);
					CParseErrors::Error(error_message);
					return false;
				}
				limit2 = round(169 * operator_limit / 100);
			}
			else if (twolimits && (str_limit2 == "" || !isNumber(str_limit2))) {
				ErrorInvalidMember(str_range_value);
				return false;
			}
			str_globalweight = str_globalweight.Mid(pos).Trim();
			if (str_globalweight == "" || !isNumber(str_globalweight)) {
				ErrorInvalidMember(str_range_value);
				return false;
			}
			globalweight = round(StringToNumber(str_globalweight));
			if ((globalweight < 0) || (globalweight > 100)) {
				ErrorInvalidMember(str_range_value);
				return false;
			}
		}
		if (str_limit2.Left(2) == "f$") {
			CFunction *function = (CFunction*)p_function_collection->LookUp(str_limit2);
			if (function == NULL) {
				// Function not found
				CString error_message;
				error_message.Format("Function %s not found or mispelled\n"
					"Please set a valid function name.", str_limit2);
				CParseErrors::Error(error_message);
				return false;
			}
			limit2 = round(169 * function->Evaluate(str_limit2) / 100);
		}
		else if (str_limit2.Left(6) == "me_re_") {
			p_engine_container->symbol_engine_memory_symbols()->EvaluateSymbol(str_limit2, &operator_limit);
			if (operator_limit < 0) {
				// Memory symbol not found
				CString error_message;
				error_message.Format("Memory symbol %s not found or mispelled\n"
					"Please set a valid memory symbol.", str_limit2);
				CParseErrors::Error(error_message);
				return false;
			}
			limit2 = round(169 * operator_limit / 100);
		}
		else if (twolimits && (str_limit2 == "" || !isNumber(str_limit2))) {
			ErrorInvalidMember(str_range_value);
			return false;
		}
		if (limit1 == kUndefined)
			limit1 = round(169*StringToNumber(str_limit1)/100);
		if (twolimits && limit2 == kUndefined) 
			limit2 = round(169*StringToNumber(str_limit2)/100);
		if (twolimits && limit1 > limit2)
			SwapInts(&limit1, &limit2);
		
		if (handrank == "")
			range_value = handrank169;
		else if (handrank == "1")
			range_value = handrank_2way;
		else if (handrank == "2")
			range_value = handrank_3way;
		else if (handrank == "3")
			range_value = handrank_4way;
		else if (handrank == "KS")
			range_value = handrank_KS;
		else if (handrank == "PPT")
			range_value = handrank_PPT;
		else if (handrank == "SMG")
			range_value = handrank_SMG;
		else if (handrank == "SMGA")
			range_value = handrank_SMGA;
		else if (handrank == "SMGO")
			range_value = handrank_SMGO;
		else if (handrank == "SMGAO")
			range_value = handrank_SMGAO;
		else if (handrank == "BC")
			range_value = handrank_BC;
		else if (handrank == "BJ")
			range_value = handrank_BJ;
		else if (handrank == "DD")
			range_value = handrank_DD;
		else if (handrank == "DN")
			range_value = handrank_DN;
		else if (handrank == "DO")
			range_value = handrank_DO;
		else if (handrank == "HG")
			range_value = handrank_HG;
		else if (handrank == "HC")
			range_value = handrank_HC;
		else if (handrank == "HCPF")
			range_value = handrank_HCPF;
		else if (handrank == "NF")
			range_value = handrank_NF;
		else if (handrank == "RPS")
			range_value = handrank_RPS;
		else if (handrank == "SG")
			range_value = handrank_SG;
		else if (handrank == "SCG")
			range_value = handrank_SCG;
		else if (handrank == "TG")
			range_value = handrank_TG;
		else if (handrank == "TGO")
			range_value = handrank_TGO;
		else if (handrank.Left(4) == "list") {
			COHScriptList *hand_list = (COHScriptList*)p_function_collection->LookUp(handrank);
			if (hand_list == NULL) {
				// List not found
				CString error_message;
				error_message.Format("List %s not found or mispelled\n"
					"Please set a valid list name.", handrank);
				CParseErrors::Error(error_message);
				return false;
			}
			if (range_value.Find(":") != -1)  bglobalweight = true;
			range_value = hand_list->function_text();
		}
		else {
			ErrorInvalidMember(str_range_value);
			return false;
		}

		break;
	}

	if (command == kSetRange) {
		CString list = range_value;
		if (range_value.FindOneOf(sep_operator) != -1) {
			int pos = 0;
			CString limit;
			list = range_value.Tokenize(sep_operator, pos).Trim();
			limit = range_value.Mid(pos).Trim();
			pos = range_value.FindOneOf(sep_operator);
			if (range_value.GetAt(pos) == '>') {
				if (range_value.GetAt(pos + 1) == '=') {
					ioperator = kOperatorSuperiorEqualTo;
					limit.Remove('=');
					pos = pos + 1;
				}
				else
					ioperator = kOperatorSuperiorTo;
			}
			else if (range_value.GetAt(pos) == '<') {
				if (range_value.GetAt(pos + 1) == '=') {
					ioperator = kOperatorInferiorEqualTo;
					limit.Remove('=');
					pos = pos + 1;
				}
				else
					ioperator = kOperatorInferiorTo;
			}
			else if (range_value.GetAt(pos) == '=')
				ioperator = kOperatorEqualTo;
			if (range_value.Mid(pos + 1).FindOneOf(sep_operator) != -1) {
				ErrorInvalidSelectRangeOperator(range_value);
				return false;
			}
			if (limit == "") {
				ErrorInvalidSelectRangeOperator(range_value);
				return false;
			}
			operator_limit = StringToNumber(limit);
			if (!isNumber(limit)) {
				if (limit.Left(2) == "f$") {
					CFunction *function = (CFunction*)p_function_collection->LookUp(limit);
					if (function == NULL) {
						// Function not found
						CString error_message;
						error_message.Format("Function %s not found or mispelled\n"
							"Please set a valid function name.", limit);
						CParseErrors::Error(error_message);
						return false;
					}
					operator_limit = function->Evaluate(limit);
				}
				else if (limit.Left(6) == "me_re_") {
					p_engine_container->symbol_engine_memory_symbols()->EvaluateSymbol(limit, &operator_limit);
					if (operator_limit < 0) {
						// Memory symbol not found
						CString error_message;
						error_message.Format("Memory symbol %s not found or mispelled\n"
							"Please set a valid memory symbol.", limit);
						CParseErrors::Error(error_message);
						return false;
					}
				}
				else {
					ErrorInvalidSelectRangeOperator(range_value);
					return false;
				}
			}

			range_value = list;
		}
	}

	if (range_value.Left(4) == "list") {
		COHScriptList *hand_list = (COHScriptList*)p_function_collection->LookUp(range_value);
		if (hand_list == NULL) {
			// List not found
			CString error_message;
			error_message.Format("List %s not found or mispelled\n"
				"Please set a valid list name.", range_value);
			CParseErrors::Error(error_message);
			return false;
		}
		range_value = hand_list->function_text();
	}
	if (range_value.Left(5) == "range") {
		range_value = p_engine_container->symbol_engine_multiplexer()->MultiplexedSymbolName(range_value);
		if (!p_engine_container->symbol_engine_multiplexer()->IsValidPostfix()) {
			// Range symbol not found
			CString error_message;
			error_message.Format("Range symbol %s not found or mispelled\n"
				"Please set a valid range symbol.", range_value);
			CParseErrors::Error(error_message);
			return false;
		}
		CString result;
		EvaluateSymbol(range_value, &result);
		range_value = result;
	}


	position = 0;

	while (!token.IsEmpty())
	{
		handcount++;
		// Get next token.
		if (command == kSetRange || command == kSetPercentileRange || command == KRemoveRange || command == kChangeWeight || command == kChangeAllWeight)
			token = range_value.Tokenize(separator, position);
		else
			token = _range[_chairnumber].Tokenize(separator, position);

		int token_start = 0;
		double weight = 100;
		CString hand, hand_start, hand_stop;
		char first_rank_string, second_rank_string, first_rank_string2, second_rank_string2;
		int first_rank, second_rank, first_rank2, second_rank2;
		char suit = '\0', suit2 = '\0';
		int length, length2;
		bool open_ended = (token.Find("+") != -1) ? true : false;
		bool hand_group = (token.Find("-") != -1) ? true : false;
		if (open_ended && hand_group) {
			ErrorInvalidMember(token);
			return false;
		}
		length = token.GetLength();
		if (length == 0)
			break;
		if (open_ended)
			token.Remove('+');
		if (hand_group) {
			hand_start = token.Tokenize("-", token_start);
			hand_stop = token.Mid(token_start);
		}
		hand = token.Tokenize(":", token_start);
		if (token.Find(":") != -1) {
			CString str_weight = token.Mid(token_start);
			if (str_weight == "" || !isNumber(str_weight)) {
				ErrorInvalidMember(token);
				return false;
			}
			weight = round(StringToNumber(str_weight));
		}
		if (command == kSetPercentileRange && bglobalweight)
			weight = globalweight;
		length = hand.GetLength();
		if ((length < 2) || (length > 3) || (weight < 0) || (weight > 100)) {
			ErrorInvalidMember(token);
			return false;
		}
		if ((ioperator == kOperatorEqualTo && weight != round(operator_limit)) ||
			(ioperator == kOperatorSuperiorTo && weight <= round(operator_limit)) ||
			(ioperator == kOperatorSuperiorEqualTo && weight < round(operator_limit)) ||
			(ioperator == kOperatorInferiorTo && weight >= round(operator_limit)) ||
			(ioperator == kOperatorInferiorEqualTo && weight > round(operator_limit)))
			goto ScanForNextToken;
		if (command == kSetPercentileRange) {
			if (twolimits && (handcount < limit1 || handcount > limit2))
				goto ScanForNextToken;
			else if (!twolimits && handcount > limit1)
				goto ScanForNextToken;
		}
				
		if (hand_group) {
			length2 = hand_start.GetLength();
			if (length != length2) {
				ErrorInvalidMember(token);
				return false;
			}
			first_rank_string2 = tolower(hand_start.GetAt(0));
			second_rank_string2 = tolower(hand_start.GetAt(1));
			first_rank2 = CardRankCharacterToCardRank(first_rank_string2);
			second_rank2 = CardRankCharacterToCardRank(second_rank_string2);
			if ((first_rank2 < 0) || (second_rank2 < 0)) {
				ErrorInvalidMember(token);
				return false;
			}
		}
		first_rank_string = tolower(hand.GetAt(0));
		second_rank_string = tolower(hand.GetAt(1));
		first_rank = CardRankCharacterToCardRank(first_rank_string);
		second_rank = CardRankCharacterToCardRank(second_rank_string);
		if ((first_rank < 0) || (second_rank < 0)) {
			ErrorInvalidMember(token);
			return false;
		}
		if (length == 2) {
			if (!hand_group && (first_rank == second_rank)) {
				switch (command) {
				case kSetRange:
					if (open_ended) {
						for (int i = k_rank_ace; i >= first_rank; i--) {
							add_pocketpair_hand(_chairnumber, i, weight);
							Set(i, i, false, weight);
						}
					}
					else {
						add_pocketpair_hand(_chairnumber, first_rank, weight);
						Set(first_rank, second_rank, false, weight);
					}
					break;
				case kSetPercentileRange:
					if (open_ended) {
						for (int i = k_rank_ace; i >= first_rank; i--) {
							add_pocketpair_hand(_chairnumber, i, weight);
							Set(i, i, false, weight);
						}
					}
					else {
						add_pocketpair_hand(_chairnumber, first_rank, weight);
						Set(first_rank, second_rank, false, weight);
					}
					break;
				case kChangeWeight:
					if (open_ended) {
						for (int i = k_rank_ace; i >= first_rank; i--) {
							if (!weight_all) {
								if (_range_matrix[_chairnumber][i - 2][i - 2] == true)
									oldweight = _range_weight_matrix[_chairnumber][i - 2][i - 2];
								else continue;
							}
							if (_range_weight_matrix[_chairnumber][i - 2][i - 2] == oldweight) {
								weight_pocketpair_hand(_chairnumber, i, oldweight, weight);
								Set(i, i, false, weight);
							}
						}
					}
					else {
						if (!weight_all) {
							if (_range_matrix[_chairnumber][second_rank - 2][first_rank - 2] == true)
								oldweight = _range_weight_matrix[_chairnumber][second_rank - 2][first_rank - 2];
							else continue;
						}
						if (_range_weight_matrix[_chairnumber][second_rank - 2][first_rank - 2] == oldweight) {
							weight_pocketpair_hand(_chairnumber, first_rank, oldweight, weight);
							Set(first_rank, second_rank, false, weight);
						}
					}
					break;
				case KRemoveRange:
					if (open_ended) {
						for (int i = k_rank_ace; i >= first_rank; i--) {
							del_pocketpair_hand(_chairnumber, i);
							Unset(i, i);
						}
					}
					else {
						del_pocketpair_hand(_chairnumber, first_rank);
						Unset(first_rank, second_rank);
					}
					break;
				case kRemovePocketPair:
					del_pocketpair_hand(_chairnumber, first_rank);
					Unset(first_rank, second_rank);
					break;
				}
			}
			else
				if (hand_group && (first_rank == second_rank) && (first_rank2 == second_rank2)) {
					if (first_rank > first_rank2)
						SwapInts(&first_rank, &first_rank2);
					switch (command) {
					case kSetRange:
						for (int i = first_rank2; i >= first_rank; i--) {
							add_pocketpair_hand(_chairnumber, i, weight);
							Set(i, i, false, weight);
						}
						break;
					case kSetPercentileRange:
						for (int i = first_rank2; i >= first_rank; i--) {
							add_pocketpair_hand(_chairnumber, i, weight);
							Set(i, i, false, weight);
						}
						break;
					case kChangeWeight:
						for (int i = first_rank2; i >= first_rank; i--) {
							if (!weight_all) {
								if (_range_matrix[_chairnumber][i - 2][i - 2] == true)
									oldweight = _range_weight_matrix[_chairnumber][i - 2][i - 2];
								else continue;
							}
							if (_range_weight_matrix[_chairnumber][i - 2][i - 2] == oldweight) {
								weight_pocketpair_hand(_chairnumber, i, oldweight, weight);
								Set(i, i, false, weight);
							}
						}
						break;
					case KRemoveRange:
						for (int i = first_rank2; i >= first_rank; i--) {
							del_pocketpair_hand(_chairnumber, i);
							Unset(i, i);
						}
						break;
					case kRemovePocketPair:
						del_pocketpair_hand(_chairnumber, first_rank);
						Unset(first_rank, second_rank);
						break;
					}
				}
			else {
				ErrorOldStyleFormat(token);
				return false;
			}
		}
		else if (length == 3) {
			// Higher rank first
			if (hand_group) {
				if (first_rank < second_rank) {
					SwapInts(&first_rank, &second_rank);
				}
				if (first_rank2 < second_rank2) {
					SwapInts(&first_rank2, &second_rank2);
				}
				if (first_rank2 < first_rank) {
					SwapInts(&first_rank2, &first_rank);
					SwapInts(&second_rank2, &second_rank);
				}
				if (second_rank2 < second_rank)
					ErrorInvalidMember(token);
				if (first_rank2 > first_rank)
					if (!((first_rank2 - second_rank2 == first_rank - second_rank) && (first_rank2 - second_rank2 <= 3)))
						ErrorInvalidMember(token);
			}
			else if (first_rank < second_rank) {
				SwapInts(&first_rank, &second_rank);
			}
			suit = tolower(hand.GetAt(2));
			if (hand_group)
				suit2 = tolower(hand_start.GetAt(2));
			if (hand_group && suit != suit2) {
				ErrorInvalidMember(token);
				return false;
			}
			if (suit == 'o') {
				switch (command) {
				case kSetRange:
					if (open_ended) {
						for (int i = first_rank - 1; i >= second_rank; i--) {
							add_offsuited_hand(_chairnumber, first_rank, i, weight);
							Set(first_rank, i, false, weight);
						}
					}
					else if (hand_group) {
						if (first_rank2 == first_rank) {
							for (int i = second_rank2; i >= second_rank; i--) {
								add_offsuited_hand(_chairnumber, first_rank, i, weight);
								Set(first_rank, i, false, weight);
							}
						}
						if (first_rank2 > first_rank) {
							int j = second_rank2;
							for (int i = first_rank2; i >= first_rank; i--) {
								add_offsuited_hand(_chairnumber, i, j, weight);
								Set(i, j--, false, weight);
							}
						}
					}
					else {
						add_offsuited_hand(_chairnumber, first_rank, second_rank, weight);
						Set(first_rank, second_rank, false, weight);
					}
					break;
				case kSetPercentileRange:
					if (open_ended) {
						for (int i = first_rank - 1; i >= second_rank; i--) {
							add_offsuited_hand(_chairnumber, first_rank, i, weight);
							Set(first_rank, i, false, weight);
						}
					}
					else if (hand_group) {
						if (first_rank2 == first_rank) {
							for (int i = second_rank2; i >= second_rank; i--) {
								add_offsuited_hand(_chairnumber, first_rank, i, weight);
								Set(first_rank, i, false, weight);
							}
						}
						if (first_rank2 > first_rank) {
							int j = second_rank2;
							for (int i = first_rank2; i >= first_rank; i--) {
								add_offsuited_hand(_chairnumber, i, j, weight);
								Set(i, j--, false, weight);
							}
						}
					}
					else {
						add_offsuited_hand(_chairnumber, first_rank, second_rank, weight);
						Set(first_rank, second_rank, false, weight);
					}
					break;
				case kChangeWeight:
					if (open_ended) {
						for (int i = first_rank - 1; i >= second_rank; i--) {
							if (!weight_all) {
								if (_range_matrix[_chairnumber][first_rank - 2][i - 2] == true)
									oldweight = _range_weight_matrix[_chairnumber][first_rank - 2][i - 2];
								else continue;
							}
							if (_range_weight_matrix[_chairnumber][first_rank - 2][i - 2] == oldweight) {
								weight_offsuited_hand(_chairnumber, first_rank, i, oldweight, weight);
								Set(first_rank, i, false, weight);
							}
						}
					}
					else if (hand_group) {
						if (first_rank2 == first_rank) {
							for (int i = second_rank2; i >= second_rank; i--) {
								if (!weight_all) {
									if (_range_matrix[_chairnumber][first_rank - 2][i - 2] == true)
										oldweight = _range_weight_matrix[_chairnumber][first_rank - 2][i - 2];
									else continue;
								}
								if (_range_weight_matrix[_chairnumber][first_rank - 2][i - 2] == oldweight) {
									weight_offsuited_hand(_chairnumber, first_rank, i, oldweight, weight);
									Set(first_rank, i, false, weight);
								}
							}
						}
						if (first_rank2 > first_rank) {
							int j = second_rank2 + 1;
							for (int i = first_rank2; i >= first_rank; i--) {
								j--;
								if (!weight_all) {
									if (_range_matrix[_chairnumber][i - 2][j - 2] == true)
										oldweight = _range_weight_matrix[_chairnumber][i - 2][j - 2];
									else continue;
								}
								if (_range_weight_matrix[_chairnumber][i - 2][j - 2] == oldweight) {
									weight_offsuited_hand(_chairnumber, i, j, oldweight, weight);
									Set(i, j, false, weight);
								}
							}
						}
					}
					else {
						if (!weight_all) {
							if (_range_matrix[_chairnumber][first_rank - 2][second_rank - 2] == true)
								oldweight = _range_weight_matrix[_chairnumber][first_rank - 2][second_rank - 2];
							else continue;
						}
						if (_range_weight_matrix[_chairnumber][first_rank - 2][second_rank - 2] == oldweight) {
							weight_offsuited_hand(_chairnumber, first_rank, second_rank, oldweight, weight);
							Set(first_rank, second_rank, false, weight);
						}
					}
					break;
				case KRemoveRange:
					if (open_ended) {
						for (int i = first_rank - 1; i >= second_rank; i--) {
							del_offsuited_hand(_chairnumber, first_rank, i);
							Unset(first_rank, i, false);
						}
					}
					else if (hand_group) {
						if (first_rank2 == first_rank) {
							for (int i = second_rank2; i >= second_rank; i--) {
								del_offsuited_hand(_chairnumber, first_rank, i);
								Unset(first_rank, i, false);
							}
						}
						if (first_rank2 > first_rank) {
							int j = second_rank2;
							for (int i = first_rank2; i >= first_rank; i--) {
								del_offsuited_hand(_chairnumber, i, j);
								Unset(i, j--, false);
							}
						}
					}
					else {
						del_offsuited_hand(_chairnumber, first_rank, second_rank);
						Unset(first_rank, second_rank, false);
					}
					break;
				case kRemoveOffsuited:
					del_offsuited_hand(_chairnumber, first_rank, second_rank);
					Unset(first_rank, second_rank, false);
					break;
				}
			}
			else if (suit == 's') {
				switch (command) {
				case kSetRange:
					if (open_ended) {
						for (int i = first_rank - 1; i >= second_rank; i--) {
							add_suited_hand(_chairnumber, first_rank, i, weight);
							Set(first_rank, i, true, weight);
						}
					}
					else if (hand_group) {
						if (first_rank2 == first_rank) {
							for (int i = second_rank2; i >= second_rank; i--) {
								add_suited_hand(_chairnumber, first_rank, i, weight);
								Set(first_rank, i, true, weight);
							}
						}
						if (first_rank2 > first_rank) {
							int j = second_rank2;
							for (int i = first_rank2; i >= first_rank; i--) {
								add_suited_hand(_chairnumber, i, j, weight);
								Set(i, j--, true, weight);
							}
						}
					}
					else {
						add_suited_hand(_chairnumber, first_rank, second_rank, weight);
						Set(first_rank, second_rank, true, weight);
					}
					break;
				case kSetPercentileRange:
					if (open_ended) {
						for (int i = first_rank - 1; i >= second_rank; i--) {
							add_suited_hand(_chairnumber, first_rank, i, weight);
							Set(first_rank, i, true, weight);
						}
					}
					else if (hand_group) {
						if (first_rank2 == first_rank) {
							for (int i = second_rank2; i >= second_rank; i--) {
								add_suited_hand(_chairnumber, first_rank, i, weight);
								Set(first_rank, i, true, weight);
							}
						}
						if (first_rank2 > first_rank) {
							int j = second_rank2;
							for (int i = first_rank2; i >= first_rank; i--) {
								add_suited_hand(_chairnumber, i, j, weight);
								Set(i, j--, true, weight);
							}
						}
					}
					else {
						add_suited_hand(_chairnumber, first_rank, second_rank, weight);
						Set(first_rank, second_rank, true, weight);
					}
					break;
				case kChangeWeight:
					if (open_ended) {
						for (int i = first_rank - 1; i >= second_rank; i--) {
							if (!weight_all) {
								if (_range_matrix[_chairnumber][i - 2][first_rank - 2] == true)
									oldweight = _range_weight_matrix[_chairnumber][i - 2][first_rank - 2];
								else continue;
							}
							if (_range_weight_matrix[_chairnumber][i - 2][first_rank - 2] == oldweight) {
								weight_suited_hand(_chairnumber, first_rank, i, oldweight, weight);
								Set(first_rank, i, true, weight);
							}
						}
					}
					else if (hand_group) {
						if (first_rank2 == first_rank) {
							for (int i = second_rank2; i >= second_rank; i--) {
								if (!weight_all) {
									if (_range_matrix[_chairnumber][i - 2][first_rank - 2] == true)
										oldweight = _range_weight_matrix[_chairnumber][i - 2][first_rank - 2];
									else continue;
								}
								if (_range_weight_matrix[_chairnumber][i - 2][first_rank - 2] == oldweight) {
									weight_suited_hand(_chairnumber, first_rank, i, oldweight, weight);
									Set(first_rank, i, true, weight);
								}
							}
						}
						if (first_rank2 > first_rank) {
							int j = second_rank2 + 1;
							for (int i = first_rank2; i >= first_rank; i--) {
								j--;
								if (!weight_all) {
									if (_range_matrix[_chairnumber][j - 2][i - 2] == true)
										oldweight = _range_weight_matrix[_chairnumber][j - 2][i - 2];
									else continue;
								}
								if (_range_weight_matrix[_chairnumber][j - 2][i - 2] == oldweight) {
									weight_suited_hand(_chairnumber, i, j, oldweight, weight);
									Set(i, j, true, weight);
								}
							}
						}
					}
					else {
						if (!weight_all) {
							if (_range_matrix[_chairnumber][second_rank - 2][first_rank - 2] == true)
								oldweight = _range_weight_matrix[_chairnumber][second_rank - 2][first_rank - 2];
							else continue;
						}
						if (_range_weight_matrix[_chairnumber][second_rank - 2][first_rank - 2] == oldweight) {
							weight_suited_hand(_chairnumber, first_rank, second_rank, oldweight, weight);
							Set(first_rank, second_rank, true, weight);
						}
					}
					break;
				case KRemoveRange:
					if (open_ended) {
						for (int i = first_rank - 1; i >= second_rank; i--) {
							del_suited_hand(_chairnumber, first_rank, i);
							Unset(first_rank, i);
						}
					}
					else if (hand_group) {
						if (first_rank2 == first_rank) {
							for (int i = second_rank2; i >= second_rank; i--) {
								del_suited_hand(_chairnumber, first_rank, i);
								Unset(first_rank, i);
							}
						}
						if (first_rank2 > first_rank) {
							int j = second_rank2;
							for (int i = first_rank2; i >= first_rank; i--) {
								del_suited_hand(_chairnumber, i, j);
								Unset(i, j--);
							}
						}
					}
					else {
						del_suited_hand(_chairnumber, first_rank, second_rank);
						Unset(first_rank, second_rank);
					}
					break;
				case kRemoveSuited:
					del_suited_hand(_chairnumber, first_rank, second_rank);
					Unset(first_rank, second_rank);
					break;
				}
			}
			else {
				ErrorInvalidMember(token);
				return false;
			}
		}
		else {
			ErrorInvalidMember(token);
			return false;
		}

		// "Ugly" goto to directly jump to scan of next token
		ScanForNextToken:
			continue;
	}
	/*
	// hole cards removal
	if (command == kSetRange) {
		int	plcardrank[2] = { 0 }, plcardsuit[2] = { 0 };
		// playercards
		plcardrank[0] = p_table_state->User()->hole_cards(0)->GetOpenHoldemRank();
		plcardrank[1] = p_table_state->User()->hole_cards(1)->GetOpenHoldemRank();
		plcardsuit[0] = p_table_state->User()->hole_cards(0)->GetSuit();
		plcardsuit[1] = p_table_state->User()->hole_cards(1)->GetSuit();
		if (plcardrank[1] > plcardrank[0]) {
			SwapInts(&plcardrank[0], &plcardrank[1]);
			SwapInts(&plcardsuit[0], &plcardsuit[1]);
		}
		cards_removal(_chairnumber, plcardrank[0], plcardrank[1], suit[plcardsuit[0]], suit[plcardsuit[1]]);
	}
	*/
	if (NHandsOnRange() < 0 || NHandsOnRange() > 169) {
		ErrorInvalidFormat();
		return false;
	}
	GenerateRangeTextFromRangeMatrix();
	_range[_chairnumber] = _range_text.Trim();
	return true;
}

int CSymbolEngineRange::NHandsOnRange() {
	int result = 0;
	for (int i = 2; i <= k_rank_ace; ++i) {
		for (int j = 2; j <= k_rank_ace; ++j) {
			if (_range_matrix[_chairnumber][i][j]) {
				result++;
			}
		}
	}
	return result;
}

bool CSymbolEngineRange::IsOnRange(int first_rank, int second_rank, bool suited) {
	AssertRange(first_rank, 2, k_rank_ace);
	AssertRange(second_rank, 2, k_rank_ace);
	// Higher rank first
	if (first_rank < second_rank) {
		SwapInts(&first_rank, &second_rank);
	}
	if (suited) {
		return IsOnRange(first_rank, second_rank);
	}
	else {
		return IsOnRange(second_rank, first_rank);
	}
}

bool CSymbolEngineRange::IsOnRange(int first_rank, int second_rank) {
	AssertRange(first_rank, 2, k_rank_ace);
	AssertRange(second_rank, 2, k_rank_ace);
	// Normating to [0..12]
	first_rank -= 2;
	second_rank -= 2;
	_weight = _range_weight_matrix[_chairnumber][second_rank][first_rank];
	return _range_matrix[_chairnumber][second_rank][first_rank];
};

void CSymbolEngineRange::GenerateRangeTextFromRangeMatrix() {
	CString result;
	vector<vector<int>>range_matrix_ordered;
	// Pairs
	for (int i = k_rank_ace; i >= 2; --i) {
		if (IsOnRange(i, i))
			range_matrix_ordered.push_back({ i - 2, i - 2, 0, _weight });
	}
	// Suited
	for (int i = k_rank_ace; i >= 2; --i) {
		for (int j = i - 1; j >= 2; --j) {
			if (IsOnRange(i, j, true))
				range_matrix_ordered.push_back({ i - 2, j - 2, 1, _weight });
		}
	}
	// Offsuited
	for (int i = k_rank_ace; i >= 2; --i) {
		for (int j = i - 1; j >= 2; --j) {
			if (IsOnRange(i, j, false))
				range_matrix_ordered.push_back({ i - 2, j - 2, 2, _weight });
		}
	}
	const int columnId = 3;
	sort(range_matrix_ordered.rbegin(), range_matrix_ordered.rend(), [&columnId](const vector<int> &item1, const vector<int> &item2)
	{
		return item1[columnId] < item2[columnId];
	});
	int prevweight = range_matrix_ordered[0][3];
	int sz = range_matrix_ordered.size();
	for (int i = 0; i < sz; i++) {
		if (prevweight != range_matrix_ordered[i][3]) {
			prevweight = range_matrix_ordered[i][3];
			result += "\n";
		}
		if (range_matrix_ordered[i][2] == 0) // Pairs
			result += CString(k_card_chars[range_matrix_ordered[i][0]]) + CString(k_card_chars[range_matrix_ordered[i][1]]) + ":" + Number2CString(range_matrix_ordered[i][3]) + ' ';
		if (range_matrix_ordered[i][2] == 1) // Suited
			result += CString(k_card_chars[range_matrix_ordered[i][0]]) + CString(k_card_chars[range_matrix_ordered[i][1]]) + "s:" + Number2CString(range_matrix_ordered[i][3]) + ' ';
		if (range_matrix_ordered[i][2] == 2) // Offsuited
			result += CString(k_card_chars[range_matrix_ordered[i][0]]) + CString(k_card_chars[range_matrix_ordered[i][1]]) + "o:" + Number2CString(range_matrix_ordered[i][3]) + ' ';
	}
	_range_text = result;
}

void CSymbolEngineRange::ErrorPrw1326NotActivated() {
	CString error_message = "Prw1326 is not activated\n"
		"To enable it use: SET prw1326_useme \"true\" command";
	CParseErrors::Error(error_message);
}

void CSymbolEngineRange::ErrorPrw1326PreflopNotActivated() {
	CString error_message = "Prw1326 for Preflop is not activated\n"
		"To enable it use: SET prw1326_usepreflop \"true\" command";
	CParseErrors::Error(error_message);
}

void CSymbolEngineRange::ErrorInvalidSelectRangeOperator(CString select_range_command) {
	CString error_message;
	error_message.Format("Invalid select range operator: %s\n"
		"Valid commands look like: SET range0 \"QQ-TT:85, AJo-63o:46, 94o+:33 = 85\"\n"
		"Or SET range_dealerchair \"listXYZ > 45\" (fixed values between 0-100)\n"
		"Or SET range0 \"listXYZ <= me_re_Limit\" (0-100 memory symbol values)\n"
		"Or SET range0 \"range_mpchair >= f$ABC\" (0-100 function values).",
		select_range_command);
	CParseErrors::Error(error_message);
}

void CSymbolEngineRange::ErrorInvalidMember(CString range_member) {
	CString error_message;
	error_message.Format("Invalid range command or member %s\n"
    "Valid are commands look like \"del listxyz\" or \"del AA:100 KK:94 AKs:88\"\n"
	"Or \"weight listabc\" or \"weight AA:85 KK:78 AKs:71\"\n"
	"Or \"weight_all /84 /96\" (/oldweight /newweight)\n"
	"Or \"del_pp\" or \"del_os\" or \"del_s\"\n"
    "Valid are members look like \"listxyz\" or \"range_mpchair\"\n"
	"Or like \"AA, AKs ATo,22\" (with ',' separator you can mix with space)\n"
	"Or like \"AA:100 AKs:88 ATo:75 22:0\" (with 0-100 weights)\n"
	"Or mix both weighted and non-weighted entries\n"
	"Or like \"%%20\" or like \"%%10-40:85\" or like \"%%20-50/1\" (for percentile)\n"
	"Or like \"TT+:85 T6s+:33, 94o+\" (for open-ended range)\n"
	"Or like \"QQ-99:92,T7s-T3s, T7o-T3o:21\" (for handrange group)\n"
	"Or like \"KJs-86s:66, AJo-63o:63\" (for one or two gapper group)\n"
	"But you can't mix open-ended and handrange group.",
		range_member);
	CParseErrors::Error(error_message);
}

void CSymbolEngineRange::ErrorOldStyleFormat(CString token) {
	CString error_message;
	error_message.Format("Old style range format detected: %s\n"
		"Use AKs for suited hands, AKo for off-suited, AKs AKo for both.",
		token);
	CParseErrors::Error(error_message);
}

void CSymbolEngineRange::ErrorInvalidFormat() {
	CString error_message = "Invalid range format:\n"
		"Range must have between 0 to 169 entries.";
	CParseErrors::Error(error_message);
}

bool CSymbolEngineRange::EvaluateSymbol(const CString name, CString *result, bool log /* = false */) {
	FAST_EXIT_ON_OPENPPL_SYMBOLS(name);
	if (memcmp(name, "range", 5) == 0 && strlen(name) == 6) {
		*result = range(RightDigitCharacterToNumber(name));
		return true;
	}
	else if (name == kEmptyExpression_False_Zero_WhenOthersFoldForce) {
		*result = "";
		return true;
	}
	else if ((memcmp(name, kEmptyExpression_False_Zero_WhenOthersFoldForce, strlen(kEmptyExpression_False_Zero_WhenOthersFoldForce)) == 0)
		&& (strlen(name) == strlen(kEmptyExpression_False_Zero_WhenOthersFoldForce))) {
		*result = "";
		return true;
	}
	// Symbol of a different symbol-engine
	return false;
}

CString CSymbolEngineRange::SymbolsProvided() {
	CString list = "";
	list += RangeOfSymbols("range%i", kFirstChair, kLastChair);
	return list;
}
