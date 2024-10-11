//******************************************************************************
//
// This file is part of the OpenHoldem project
//    Source code:           https://github.com/OpenHoldem/openholdembot/
//    Forums:                http://www.maxinmontreal.com/forums/index.php
//    Licensed under GPL v3: http://www.gnu.org/licenses/gpl.html
//
//******************************************************************************
//
// Purpose: Base class for Operators and terminal nodes 
//   (identifiers, numbers)
//
//******************************************************************************

#include "stdafx.h" 
#include "CParseTreeNode.h"

#include "CFunction.h"
#include "CMemoryPool.h"

#include "TokenizerConstants.h"

CParseTreeNode::CParseTreeNode(int relative_line_number) {
  _relative_line_number = relative_line_number;
  _node_type = kUndefined;
  _first_sibbling  = NULL;
  _second_sibbling = NULL;
  _third_sibbling  = NULL;
}

CParseTreeNode::~CParseTreeNode() {
  // No longer deleting any sibblings
  // (must not be done, because they didn't get allocated explicitly).
  // CMemoryPool cares about all these small objects now.
}

TPParseTreeNode CParseTreeNode::GetRightMostSibbling() {
  if (TokenIsTernary(_node_type)) {
    return _third_sibbling;
  } else if (TokenIsBinary(_node_type)) {
    return _second_sibbling;
  } else if (TokenIsUnary(_node_type)) {
    return _first_sibbling;
  } else {
    // Not an operator
    return NULL;
  }
}

TPParseTreeNode CParseTreeNode::GetLeftMostSibbling() {
  if (TokenIsUnary(_node_type)
      || TokenIsBinary(_node_type)
      || TokenIsTernary(_node_type)) {
    return _first_sibbling;
  }
  // Not an operator
   return NULL;
}

void CParseTreeNode::SetRightMostSibbling(TPParseTreeNode sibbling){
  if (TokenIsTernary(_node_type)) {
   _third_sibbling = sibbling;
  } else if (TokenIsBinary(_node_type)) {
    _second_sibbling = sibbling;
  } else {
    // Default: first one is always present
    _first_sibbling = sibbling;
  }
}

void CParseTreeNode::SetLeftMostSibbling(TPParseTreeNode sibbling){
  _first_sibbling = sibbling;
}

bool CParseTreeNode::IsAnyKindOfWhenCondition() {
  return (_node_type == kTokenOperatorConditionalWhen);
}

bool CParseTreeNode::IsWhenConditionWithAction() {
  return (IsAnyKindOfWhenCondition() && !IsOpenEndedWhenCondition());
}

bool CParseTreeNode::IsOpenEndedWhenCondition() {
  if (!IsAnyKindOfWhenCondition()) return false;
  if ((_second_sibbling == NULL) && (_third_sibbling == NULL)) return true; // ?????
  if ((_second_sibbling != NULL) 
      && (_second_sibbling->_node_type == kTokenOperatorConditionalWhen)) {
    return true;
  }
  return false;
}

void  CParseTreeNode::VerifyBooleanOperand(double value) {
  // A very common user-mistake:
  // People doing logical operations on umbers
  // as they don't understand the precedence of operators well emough.
  // Example: .., AND NOT betrund > 1 ...
  // http://www.maxinmontreal.com/forums/viewtopic.php?f=297&t=20703
  if (value == double(true)) {
    return;
  }
  if (value == double(false)) {
    return;
  }
  write_log(k_always_log_errors, "WARNING! Logical operator %s working on numbers\n",
    TokenString(_node_type));
  write_log(k_always_log_errors, "  Function: %s\n",
    CFunction::CurrentlyEvaluatedFunction()->name());
  write_log(k_always_log_errors, "  Line:     %i\n",
    _relative_line_number);
}

// virtual 
double CParseTreeNode::Evaluate(bool log /* = false */) {
  return kUndefined;
}

// virtual 
CString CParseTreeNode::EvaluateString(bool log /* = false */) {
	return "";
}

// virtual 
CString CParseTreeNode::EvaluateToString(bool /* log = false */) {
  return "";
}

// virtual 
bool CParseTreeNode::EvaluatesToBinaryNumber() {
  return false;
}

// virtual 
CString CParseTreeNode::Serialize() {
  return "";
}

void* CParseTreeNode::operator new(size_t size) {
  assert(PMemoryPoolParser() != NULL);
  write_log(Preferences()->debug_memory_usage(), "[CParseTreeNode] Allocating %i bytes\n", size);
  return PMemoryPoolParser()->Allocate(size);
}