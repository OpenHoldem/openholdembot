//******************************************************************************
//
// This file is part of the OpenHoldem project
//    Source code:           https://github.com/OpenHoldem/openholdembot/
//    Forums:                http://www.maxinmontreal.com/forums/index.php
//    Licensed under GPL v3: http://www.gnu.org/licenses/gpl.html
//
//******************************************************************************
//
// Purpose: base class for global objects 
//   that use memory-pools
//
//******************************************************************************

#pragma once

#include "CMemoryPool.h"
#include "Debugger/debugger.h"

class CSpaceOptimizedGlobalObject {
 public:
  CSpaceOptimizedGlobalObject()  {}
  ~CSpaceOptimizedGlobalObject() {}
 public:
  void* operator new(size_t size) {
    assert(p_memory_pool_global != NULL);
    return p_memory_pool_global->Allocate(size);
  }
 public:
  void operator delete(void* ptr) {}
};