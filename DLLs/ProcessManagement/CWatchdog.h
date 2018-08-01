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
// Purpose: Watching for crashed processes and cleaning up shared memory
//   * so that another instance can connect to this table
//   * because the OpenHoldem starter needs the number of running processes
//
//******************************************************************************

#include "LibDef.h"
#include "..\MemoryManagement_DLL\CSpaceOptimizedGlobalObject.h"

class PROCESS_MANAGEMEMT__DLL_API CWatchdog: public CSpaceOptimizedGlobalObject {
public:
  CWatchdog();
  ~CWatchdog();
public:
  void HandleCrashedAndFrozenProcesses();
  void MarkThisInstanceAsAlive();
private:
  void WatchForCrashedProcesses();
  void WatchForFrozenProcesses();
private:
  void MarkInstanceAsAlive(int session_ID);
  void MarkInstanceAsDead(int session_ID);
  void MarkThisInstanceAsDead();
};