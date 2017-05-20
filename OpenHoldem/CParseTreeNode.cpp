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
#include "CPreferences.h"
#include "TokenizerConstants.h"

CParseTreeNode::CParseTreeNode(int relative_line_number) {
  _relative_line_number = relative_line_number;
  _node_type = kUndefined;
  _first_sibbling  = NULL;
  _second_sibbling = NULL;
  _third_sibbling  = NULL;
}

CParseTreeNode::~CParseTreeNode() {
  // OpenHoldem could crash when deleting(reloading, parsing, applying) a formula.
  //   Open - ended when - conditions are usually reachable on two paths :
  // a) from the OEWC above, if this OEWC was false
  // b) from the sequence of when - conditions with actions above
  // if none of these actions could be taken.
  //   However when deleting a parse - tree each node must be deleted exactly once.
  //   Care has been taken for OEWCs, but the identification of OEWCs
  //   failed on a half - way destructed parse - tree.
  //   So some OEWCs could be deleted twice which caused an access violation.
  //   Thanks to JohnNash, Kkfc, Dolar and Salem for reporting the problem
  //   and to Sagan, Gezs, BadHabit and Darkmund for providing log - files and crashing formulas.
  //
  // Therefore we have to check the open-ended-ness once at the very beginning
  // and remeber the result.
  bool is_open_ended_when_condition = IsOpenEndedWhenCondition();
#ifdef _DEBUG
  write_log(preferences.debug_formula(),
    "[CParseTreeNode] Deleting parse tree node %i\n", this);
  if (_node_type == kTokenOperatorConditionalWhen) {
    write_log(preferences.debug_formula(),
      "[CParseTreeNode] WHEN condition: %s\n", _first_sibbling->Serialize());
    write_log(preferences.debug_formula(),
      "[CParseTreeNode] Open ended: %s\n", Bool2CString(IsOpenEndedWhenCondition()));
  }
  write_log(preferences.debug_formula(),
    "[CParseTreeNode] 1st sibbling %i::%i\n", this, _first_sibbling);
  write_log(preferences.debug_formula(),
    "[CParseTreeNode] 2nd sibbling %i::%i\n", this, _second_sibbling);
  write_log(preferences.debug_formula(),
    "[CParseTreeNode] 3rd sibbling %i::%i\n", this, _third_sibbling);
#endif
  if (_first_sibbling != NULL) {
#ifdef _DEBUG
   write_log(preferences.debug_formula(),
     "[CParseTreeNode] Deleting 1st sibbling %i::%i\n", this, _first_sibbling);
#endif
    delete _first_sibbling;
    _first_sibbling = NULL;
  }
  if (_second_sibbling != NULL) {
#ifdef _DEBUG
    write_log(preferences.debug_formula(),
      "[CParseTreeNode] Deleting 2nd sibbling %i::%i\n", this, _second_sibbling);
#endif
    delete _second_sibbling;
    _second_sibbling = NULL;
  }
  if ((_third_sibbling != NULL) && !is_open_ended_when_condition) {
    // Be carefull with open-ended when-conditions.
    // They create graphs, no longer pure trees/
    // The 3rd node is reachable on two paths.
#ifdef _DEBUG
    write_log(preferences.debug_formula(),
      "[CParseTreeNode] Open ended: %s\n", Bool2CString(IsOpenEndedWhenCondition()));
    write_log(preferences.debug_formula(),
      "[CParseTreeNode] Node type %i\n", _node_type);
    write_log(preferences.debug_formula(),
      "[CParseTreeNode] Deleting 3rd sibbling %i::%i\n", this, _third_sibbling);
#endif
    delete _third_sibbling;
    _third_sibbling = NULL;
  }
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
  // www!!!!!
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
