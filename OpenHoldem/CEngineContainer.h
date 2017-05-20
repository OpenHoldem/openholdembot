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

#ifndef INC_CENGINECONTAINER_H
#define INC_CENGINECONTAINER_H

#include "CVirtualSymbolEngine.h"

const int k_max_number_of_symbol_engines = 100;

class CEngineContainer {
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
  CString SymbolsProvided()       { BuildListOfSymbolsProvided(); return _list_of_symbols; }
 private:
  void BuildListOfSymbolsProvided();
 private:
  void CreateSpecialSymbolEngines();
  void AddSymbolEngine(CVirtualSymbolEngine *new_symbol_engine);
  void CreateSymbolEngines();
  void DestroyAllSymbolEngines();
  void DestroyAllSpecialSymbolEngines();
 private:
  void InitOnStartup();
  void UpdateOnHandreset();
  void UpdateOnNewRound();
  void UpdateOnMyTurn();
  void UpdateOnHeartbeat();
 private:
  CVirtualSymbolEngine *_symbol_engines[k_max_number_of_symbol_engines]; 
  int _number_of_symbol_engines_loaded;
  bool _reset_on_connection_executed;
  CString _list_of_symbols;
};

extern CEngineContainer *p_engine_container;

#endif INC_CENGINECONTAINER_H