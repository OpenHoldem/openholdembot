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

#ifndef INC_CPARSETREENODE_H
#define INC_CPARSETREENODE_H

class CParseTreeNode;

typedef CParseTreeNode *TPParseTreeNode;

class CParseTreeNode {
  friend class CParseTreeOperatorNode;
  friend class CParseTreeTerminalNode;
  friend class CParseTreeTerminalNodeBetsizeAction;
  friend class CParseTreeTerminalNodeFixedAction;
  friend class CParseTreeTerminalNodeIdentifier;
  friend class CParseTreeTerminalNodeNumber;
  friend class CParseTreeTerminalNodeUserVariable;
  friend class CFormulaParser;
  friend class CParseTreeRotator;
 public:
  CParseTreeNode(int relative_line_number);
  virtual ~CParseTreeNode();
 public:
  virtual double Evaluate(bool log = false);
  virtual CString EvaluateString(bool log = false);
  virtual CString EvaluateToString(bool log = false);
  virtual bool EvaluatesToBinaryNumber();
 public:
  // For debugging output
  virtual CString Serialize();
  // Optimized memory-allocation with memory-pools
  void* operator new(size_t size);
 protected:
  TPParseTreeNode GetRightMostSibbling();
  TPParseTreeNode GetLeftMostSibbling();
  void SetRightMostSibbling(TPParseTreeNode sibbling);
  void SetLeftMostSibbling(TPParseTreeNode sibbling);
 protected:
  bool IsAnyKindOfWhenCondition();
  bool IsWhenConditionWithAction();
  bool IsOpenEndedWhenCondition();
 protected:
  void VerifyBooleanOperand(double value);
 protected:
  int _node_type;
 protected:
  // Strictly speaking not all nodes meed sibblings.
  // Operator nodes need them, terminal nodes don't.
  // However the parse-tree-rotator that cares about priority-ordering
  // doesn't distinguish the various derived node-types.
  // ATM we do it the "easy" way.
  //
  // Sibblings: pointers to the operands of expressions
  // First: always present, as long as non-terminal-node
  TPParseTreeNode _first_sibbling;	
  // Second: for binary and ternary operators
  // Holds the 2nd operand for binary expressions
  // Holds the "then"-part of ternary-expressions
  // Holds the "then"-part of WHEN-conditions
  // Holds the continuation to the next when-condition after user-vartiables
  TPParseTreeNode _second_sibbling;	
  // Third: for ternary operators only
  // Holds the "else"-part of ternary-expressions
  // Holds the next when-condition in when-condition-sequences
  TPParseTreeNode _third_sibbling;	
 private:
  // Line number relative to current function
  int _relative_line_number;
};

#endif INC_CPARSETREENODE_H