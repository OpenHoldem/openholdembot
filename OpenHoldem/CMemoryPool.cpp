//******************************************************************************
//
// This file is part of the OpenHoldem project
//    Source code:           https://github.com/OpenHoldem/openholdembot/
//    Forums:                http://www.maxinmontreal.com/forums/index.php
//    Licensed under GPL v3: http://www.gnu.org/licenses/gpl.html
//
//******************************************************************************
//
// Purpose: Memory-pool
//   to avoid allocating countless oversized 4KB blocks
//   when we want to create only small objects.
//   http://www.maxinmontreal.com/forums/viewtopic.php?f=111&t=20996
//
//******************************************************************************
//
// Limitations:
//   * does not handle memory-requests that are larger as the default buffer
//   * does not support partial release of memory;
//     use multiple pools if needed
//   * not thread-safe; use different pools for different threads
//
//******************************************************************************

#include "stdafx.h"
#include "CMemoryPool.h"

#include "CFormulaParser.h"
#include "WindowFunctions/window_functions.h"

CMemoryPool *p_memory_pool_tablemaps = NULL;
CMemoryPool *p_memory_pool_scraper = NULL;
CMemoryPool *p_memory_pool_user_logic = NULL;
CMemoryPool *p_memory_pool_library_logic = NULL;
CMemoryPool *p_memory_pool_global = NULL;

void CreateMemoryPools() {
  assert(p_memory_pool_tablemaps == NULL);
  assert(p_memory_pool_scraper == NULL);
  assert(p_memory_pool_user_logic == NULL);
  assert(p_memory_pool_library_logic == NULL);
  assert(p_memory_pool_global == NULL);
  p_memory_pool_tablemaps = new CMemoryPool;
  p_memory_pool_scraper = new CMemoryPool;
  p_memory_pool_user_logic = new CMemoryPool;
  p_memory_pool_library_logic = new CMemoryPool;
  p_memory_pool_global = new CMemoryPool;
}

void DeleteAllMemoryPools() {
  delete p_memory_pool_tablemaps;
  delete p_memory_pool_scraper;
  delete p_memory_pool_user_logic;
  delete p_memory_pool_library_logic;
  delete p_memory_pool_global;
}

// 64 KB = 1 default block at 64-bit-Windows 
//       = 16 default blocks at 52-bit-Windows
const int kMemoryBlockSize = 65536;

CMemoryPool::CMemoryPool() {
  _all_emmory_released = true;
  _current_memory_block = NULL;
  _bytes_used_in_current_block = kMemoryBlockSize;
  _memory_blocks.RemoveAll();
}

CMemoryPool::~CMemoryPool() {
  if (!_all_emmory_released) {
    ReleaseAll();
  }
}

size_t CMemoryPool::BytesAvailableInCurrentBlock() {
  if (_current_memory_block == NULL) {
    return 0;
  }
  AssertRange(_bytes_used_in_current_block, 0, kMemoryBlockSize);
  return (kMemoryBlockSize - _bytes_used_in_current_block);
}

void* CMemoryPool::Allocate(size_t size) {
  AlignNextMemoryBlock();
  if (size > kMemoryBlockSize) {
    MessageBox_Error_Warning(
      "CMemoryPool received oversized request.\n"
      "Going to terminate.\n");    PostQuitMessage(0);
    return NULL;
  }
  if (_current_memory_block == NULL) {
    AllocateNewMemoryBlock();
  } else if (size > BytesAvailableInCurrentBlock()) {
    AllocateNewMemoryBlock();
  }
  assert(size <= BytesAvailableInCurrentBlock());
  _all_emmory_released = false;
  size_t offset_to_allocation = _bytes_used_in_current_block;
  _bytes_used_in_current_block += size;
  return ((char*)_current_memory_block + offset_to_allocation);
}

void CMemoryPool::AllocateNewMemoryBlock() {
  _current_memory_block = malloc(kMemoryBlockSize);
  if (_current_memory_block == NULL) {
    MessageBox_Error_Warning(
      "CMemoryPool received oversized request.\n"
      "Going to terminate.\n");
    PostQuitMessage(0);
    return;
  }
  _bytes_used_in_current_block = 0;
  _memory_blocks.Add(_current_memory_block);
}

void CMemoryPool::ReleaseAll() {
  for (int i = 0; i < _memory_blocks.GetCount(); ++i) {
    _current_memory_block = _memory_blocks[i];
    delete _current_memory_block;
  }
  _all_emmory_released = true;
  _current_memory_block = NULL;
  _bytes_used_in_current_block = kMemoryBlockSize;
  _memory_blocks.RemoveAll();
}

void CMemoryPool::AlignNextMemoryBlock() {
  // Align memory-addresses to multiples of 4 or 8
  const int alignment = 4;
  int mis_alignment = _bytes_used_in_current_block % alignment;
  assert(mis_alignment >= 0);
  if (mis_alignment != 0) {
    int n_padding_bytes = alignment - mis_alignment;
    assert(n_padding_bytes > 0);
    _bytes_used_in_current_block += n_padding_bytes;
    assert(_bytes_used_in_current_block % alignment == 0);
  }
}

CMemoryPool *PMemoryPoolParser() {
  assert(p_formula_parser != NULL);
  if (p_formula_parser->IsParsingReadOnlyFunctionLibrary()) {
    assert(p_memory_pool_library_logic != NULL);
    return p_memory_pool_library_logic;
  }
  assert(p_memory_pool_user_logic != NULL);
  return p_memory_pool_user_logic;
}