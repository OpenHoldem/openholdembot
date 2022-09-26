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
// Purpose:
//
//******************************************************************************

#include "CSpaceOptimizedGlobalObject.h"
#include "CVirtualSymbolEngine.h"

class CSymbolEngineActiveDealtPlaying;
class CSymbolEngineAutoplayer;
class CSymbolEngineBlinds;
class CSymbolEngineCallers;
class CSymbolEngineCards;
class CSymbolEngineCasino;
class CSymbolEngineChairs;
class CSymbolEngineChecksBetsFolds;
class CSymbolEngineChipAmounts;
class CSymbolEngineColourCodes;
class CSymbolEngineConstants;
class CSymbolEngineDealerchair;
class CSymbolEngineDebug;
class CSymbolEngineEventLogging;
class CSymbolEngineFormulaSwitching;
class CSymbolEngineGameType;
class CSymbolEngineHandrank;
class CSymbolEngineHistory;
class CSymbolEngineIniFunctions;
class CSymbolEngineIsOmaha;
class CSymbolEngineIsRush;
class CSymbolEngineIsTournament;
class CSymbolEngineMemorySymbols;
class CSymbolEngineMTTInfo;
class CSymbolEngineMultiplexer;
class CSymbolEngineNutFullhouseOrFourOfAKind;
class CSymbolEngineOpenPPL;
class CSymbolEngineOpenPPLHandAndBoardExpression;
class CSymbolEngineOpenPPLUserVariables;
class CSymbolEnginePokerAction;
class CSymbolEnginePokerTracker;
class CSymbolEnginePokerval;
class CSymbolEnginePositions;
class CSymbolEnginePrwin;
class CSymbolEngineRaisers;
class CSymbolEngineRandom;
class CSymbolEngineReplayFrameController;
class CSymbolEngineTableLimits;
class CSymbolEngineTableStats;
class CSymbolEngineTime;
class CSymbolEngineUserchair;
class CSymbolEngineUserDLL;
class CSymbolEngineVariousDataLookup;
class CSymbolEngineVersus;
class CSymbolEngineWeightedRange;
class CSymbolEngineICM;

class CEngineContainer : public CSpaceOptimizedGlobalObject {
 public:
  CEngineContainer();
  ~CEngineContainer();
 public:
  // both to be called by the auto-connector
  void UpdateOnConnection();
  void UpdateOnDisconnection();
  // To be called by the autoplayer
  void UpdateAfterAutoplayerAction(int autoplayer_action_code);
 public:
  void EvaluateAll();
  bool EvaluateSymbol(const CString name, double *result, bool log = false);
  bool EvaluateSymbol(const CString name, CString *result, bool log = false);
  CString SymbolsProvided()       { BuildListOfSymbolsProvided(); return _list_of_symbols; }
 public:
  void CreateSymbolEngines();
 private:
  void BuildListOfSymbolsProvided();
 private:
  void CreateSpecialSymbolEngines();
  void AddSymbolEngine(CVirtualSymbolEngine *new_symbol_engine);
// For new string variable symbols (storing string values)
  void AddStrVarSymbolEngine(CVirtualSymbolEngine *new_symbol_engine);
  void DestroyAllSymbolEngines();
  void DestroyAllSpecialSymbolEngines();
 private:
  void InitOnStartup();
  /* probably no longer needed
  void UpdateOnHandreset();
  void UpdateOnNewRound();
  void UpdateOnMyTurn();
  void UpdateOnHeartbeat();
  */
 private:
  CArray <CVirtualSymbolEngine*, CVirtualSymbolEngine*> _symbol_engines;
// Introduction of new string variable symbols (storing string values)
// (i.e. 'wrange0..9' symbols assigning ranges to chair0..9 for prw1326
// example: SET range0 "AA:0 AKs:6 AKo:12 ..." (handrank2652 format)
  CArray <CVirtualSymbolEngine*, CVirtualSymbolEngine*> _strvar_symbol_engines;  
  bool _reset_on_connection_executed;
  CString _list_of_symbols;
public:
  CSymbolEngineActiveDealtPlaying *symbol_engine_active_dealt_playing()
    { return p_symbol_engine_active_dealt_playing; }
  CSymbolEngineAutoplayer *symbol_engine_autoplayer()
    { return p_symbol_engine_autoplayer; }
  CSymbolEngineBlinds *symbol_engine_blinds()
    { return p_symbol_engine_blinds; }
  CSymbolEngineCallers *symbol_engine_callers()
    { return p_symbol_engine_callers; }
  CSymbolEngineCards *symbol_engine_cards()
    { return p_symbol_engine_cards; }
  CSymbolEngineCasino *symbol_engine_casino()
    { return p_symbol_engine_casino; }
  CSymbolEngineChairs *symbol_engine_chairs()
    { return p_symbol_engine_chairs; }
  CSymbolEngineChecksBetsFolds *symbol_engine_checks_bets_folds()
    { return p_symbol_engine_checks_bets_folds; }
  CSymbolEngineChipAmounts *symbol_engine_chip_amounts()
    { return p_symbol_engine_chip_amounts; }
  CSymbolEngineColourCodes *symbol_engine_colourcodes()
    { return p_symbol_engine_colourcodes; }
  CSymbolEngineConstants *symbol_engine_constants()
    { return p_symbol_engine_constants; }
  CSymbolEngineDealerchair *symbol_engine_dealerchair()
    { return p_symbol_engine_dealerchair; }
  CSymbolEngineDebug *symbol_engine_debug()
    { return p_symbol_engine_debug; }
  CSymbolEngineEventLogging *symbol_engine_event_logging()
    { return p_symbol_engine_event_logging; }
  CSymbolEngineFormulaSwitching *symbol_engine_formula_switching()
    { return p_symbol_engine_formula_switching; }
  CSymbolEngineGameType *symbol_engine_gametype()
    { return p_symbol_engine_gametype; }
  CSymbolEngineHandrank *symbol_engine_handrank()
    { return p_symbol_engine_handrank; }
  CSymbolEngineHistory *symbol_engine_history()
    { return p_symbol_engine_history; }
  CSymbolEngineIniFunctions *symbol_engine_ini_functions()
    { return p_symbol_engine_ini_functions; }
  CSymbolEngineIsOmaha *symbol_engine_isomaha()
    { return p_symbol_engine_isomaha; }
  CSymbolEngineIsRush *symbol_engine_isrush()
    { return p_symbol_engine_isrush; }
  CSymbolEngineIsTournament *symbol_engine_istournament()
    { return p_symbol_engine_istournament; }
  CSymbolEngineMemorySymbols *symbol_engine_memory_symbols()
    { return p_symbol_engine_memory_symbols; }
  CSymbolEngineMTTInfo *symbol_engine_mtt_info()
    { return p_symbol_engine_mtt_info; }
  CSymbolEngineMultiplexer *symbol_engine_multiplexer()
    { return p_symbol_engine_multiplexer; }
  CSymbolEngineNutFullhouseOrFourOfAKind *symbol_engine_nutfullhouse_or_four_of_a_kind()
    { return p_symbol_engine_nutfullhouse_or_four_of_a_kind; }
  CSymbolEngineOpenPPL *symbol_engine_open_ppl()
    { return p_symbol_engine_open_ppl; }
  CSymbolEngineOpenPPLHandAndBoardExpression *symbol_engine_open_ppl_hand_and_board_expression()
    { return p_symbol_engine_open_ppl_hand_and_board_expression; }
  CSymbolEngineOpenPPLUserVariables *symbol_engine_openppl_user_variables()
    { return p_symbol_engine_openppl_user_variables; }
  CSymbolEnginePokerAction *symbol_engine_poker_action()
    { return p_symbol_engine_poker_action; }
  CSymbolEnginePokerTracker *symbol_engine_pokertracker()
    { return p_symbol_engine_pokertracker; }
  CSymbolEnginePokerval *symbol_engine_pokerval()
    { return p_symbol_engine_pokerval; }
  CSymbolEnginePositions *symbol_engine_positions()
    { return p_symbol_engine_positions; }
  CSymbolEnginePrwin *symbol_engine_prwin()
    { return p_symbol_engine_prwin; }
  CSymbolEngineRaisers *symbol_engine_raisers()
    { return p_symbol_engine_raisers; }
  CSymbolEngineRandom *symbol_engine_random()
    { return p_symbol_engine_random; }
  CSymbolEngineReplayFrameController *symbol_engine_replayframe_controller()
    { return p_symbol_engine_replayframe_controller; }
  CSymbolEngineTableLimits *symbol_engine_tablelimits() {
    return p_symbol_engine_tablelimits; }
  CSymbolEngineTableStats *symbol_engine_table_stats()
    { return p_symbol_engine_table_stats; }
  CSymbolEngineTime *symbol_engine_time()
    { return p_symbol_engine_time; }
  CSymbolEngineUserchair *symbol_engine_userchair()
    { return p_symbol_engine_userchair; }
  CSymbolEngineUserDLL *symbol_engine_user_DLL() {
    return p_symbol_engine_user_DLL; }
  CSymbolEngineVariousDataLookup *symbol_engine_various_data_lookup()
    { return p_symbol_engine_various_data_lookup; }
  CSymbolEngineVersus *symbol_engine_versus()
    { return p_symbol_engine_versus; }
  CSymbolEngineWeightedRange *symbol_engine_weighted_range()
	{ return p_symbol_engine_weighted_range; }
  CSymbolEngineICM *symbol_engine_icm()
    { return p_symbol_engine_icm; }
private:
  CSymbolEngineActiveDealtPlaying *p_symbol_engine_active_dealt_playing;
  CSymbolEngineAutoplayer *p_symbol_engine_autoplayer;
  CSymbolEngineBlinds *p_symbol_engine_blinds;
  CSymbolEngineCallers *p_symbol_engine_callers;
  CSymbolEngineCards *p_symbol_engine_cards;
  CSymbolEngineCasino *p_symbol_engine_casino;
  CSymbolEngineChairs *p_symbol_engine_chairs;
  CSymbolEngineChecksBetsFolds *p_symbol_engine_checks_bets_folds;
  CSymbolEngineChipAmounts *p_symbol_engine_chip_amounts;
  CSymbolEngineColourCodes *p_symbol_engine_colourcodes;
  CSymbolEngineConstants *p_symbol_engine_constants;
  CSymbolEngineDealerchair *p_symbol_engine_dealerchair;
  CSymbolEngineDebug *p_symbol_engine_debug;
  CSymbolEngineEventLogging *p_symbol_engine_event_logging;
  CSymbolEngineFormulaSwitching *p_symbol_engine_formula_switching;
  CSymbolEngineGameType *p_symbol_engine_gametype;
  CSymbolEngineHandrank *p_symbol_engine_handrank;
  CSymbolEngineHistory *p_symbol_engine_history;
  CSymbolEngineIniFunctions *p_symbol_engine_ini_functions;
  CSymbolEngineIsOmaha *p_symbol_engine_isomaha;
  CSymbolEngineIsRush *p_symbol_engine_isrush;
  CSymbolEngineIsTournament *p_symbol_engine_istournament;
  CSymbolEngineMemorySymbols *p_symbol_engine_memory_symbols;
  CSymbolEngineMTTInfo *p_symbol_engine_mtt_info;
  CSymbolEngineMultiplexer *p_symbol_engine_multiplexer;
  CSymbolEngineNutFullhouseOrFourOfAKind *p_symbol_engine_nutfullhouse_or_four_of_a_kind;
  CSymbolEngineOpenPPL *p_symbol_engine_open_ppl;
  CSymbolEngineOpenPPLHandAndBoardExpression *p_symbol_engine_open_ppl_hand_and_board_expression;
  CSymbolEngineOpenPPLUserVariables *p_symbol_engine_openppl_user_variables;
  CSymbolEnginePokerAction *p_symbol_engine_poker_action;
  CSymbolEnginePokerTracker *p_symbol_engine_pokertracker;
  CSymbolEnginePokerval *p_symbol_engine_pokerval;
  CSymbolEnginePositions *p_symbol_engine_positions;
  CSymbolEnginePrwin *p_symbol_engine_prwin;
  CSymbolEngineRaisers *p_symbol_engine_raisers;
  CSymbolEngineRandom *p_symbol_engine_random;
  CSymbolEngineReplayFrameController *p_symbol_engine_replayframe_controller;
  CSymbolEngineTableLimits *p_symbol_engine_tablelimits;
  CSymbolEngineTableStats *p_symbol_engine_table_stats;
  CSymbolEngineTime *p_symbol_engine_time;
  CSymbolEngineUserchair *p_symbol_engine_userchair;
  CSymbolEngineUserDLL *p_symbol_engine_user_DLL;
  CSymbolEngineVariousDataLookup *p_symbol_engine_various_data_lookup;
  CSymbolEngineVersus *p_symbol_engine_versus;
  CSymbolEngineWeightedRange *p_symbol_engine_weighted_range;
  CSymbolEngineICM *p_symbol_engine_icm;
};

extern CEngineContainer *p_engine_container;