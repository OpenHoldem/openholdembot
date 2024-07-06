//***************************;***************************************************
//
// This file is part of the OpenHoldem project
//    Source code:           https://github.com/OpenHoldem/openholdembot/
//    Forums:                http://www.maxinmontreal.com/forums/index.php
//    Licensed under GPL v3: http://www.gnu.org/licenses/gpl.html
//
//******************************************************************************
//
// Purpose: Priority-ordering of operator-nodes in the parse-tree.
//   OpenHoldem creates parse-trees in a very straight-forward way:
//
// The (naive) parse-tree for "2*3 + 4*5"
//
//     *
//    / \
//   2   +
//      / \
//     3   *
//        / \
//       4   5
//
// However if we evaluate depth-first this results in (2 * (3 + ( 4 * 5))) = 46
// Now we need to modifiy the parse-tree, low-priority operators to the top:
//  
//         +
//       /   \
//     *      *
//    / \    / \
//   2   3  4   5
//
//
// Evaluating this tree depth-first will result in (2 * 3) + (4 * 5),
// first the sibblings, then the parent node (plus-operator).
//
//******************************************************************************

#include "stdafx.h"
#include "CParseTreeRotator.h"

#include "CFunction.h"
#include "CParseTreeNode.h"
#include "CParseTreeOperatorNode.h"

#include "WindowFunctions/window_functions.h"
#include "TokenizerConstants.h"

// undef for no extra debug output
#undef DEBUG_SHOW_SERIALIZATION_AFTER_ROTATION_COMPLETELY_FINISHED

CParseTreeRotator::CParseTreeRotator() {
}

CParseTreeRotator::~CParseTreeRotator() {
}

void CParseTreeRotator::Rotate(CFunction *function) {
  write_log(Preferences()->debug_ast_priority_ordering(),
    "[CParseTreeRotator] starting rotation\n");
  if (function == NULL) {
    return;
  }
  write_log(Preferences()->debug_ast_priority_ordering(),
    "[CParseTreeRotator] finished rotation\n");
  Rotate(function->_parse_tree_node, &function->_parse_tree_node);
  VerifyCorrectRotation(function->_parse_tree_node);
#ifdef DEBUG_SHOW_SERIALIZATION_AFTER_ROTATION_COMPLETELY_FINISHED
  MessageBox_Interactive(function->Serialize(), function->name(), 0);
  if (Preferences()->debug_ast_priority_ordering()) {
    CString serialized_function = function->Serialize();
    if (serialized_function.GetLength() < 2000) {
      // Extremely large functions overflow the print-buffer.
      // Therefore we skip everything that looks "large".
      write_log(Preferences()->debug_ast_priority_ordering(),
        "[CParseTreeRotator] function [%s] after rotation: %s\n",
        function->name(), serialized_function);
    }
  }
#endif
}

void CParseTreeRotator::Rotate(TPParseTreeNode parse_tree_node,
                               TPParseTreeNode *pointer_to_parent_pointer_for_back_patching) {
  write_log(Preferences()->debug_ast_priority_ordering(),
    "[CParseTreeRotator] rotating node %x\n", parse_tree_node);
  if (parse_tree_node == NULL) {
    return;
  }
  // We must rotate the sibblings first, bottom-up
  Rotate(parse_tree_node->_first_sibbling,  &parse_tree_node->_first_sibbling);
  Rotate(parse_tree_node->_second_sibbling, &parse_tree_node->_second_sibbling);
  if (!parse_tree_node->IsOpenEndedWhenCondition()) {
    // Actually the parse-tree is a directed acyclic graph.
    // as the next open-ended when-condition in a sequence
    // can be reached in 2 ways:
    //   * by the previous sequence of when-conditions (implicit continue)
    //   * by the false-edge of the previous open-ended when-condition
    // If we naively roate every sibbling of a sequence of open-ended
    // when conditions, then every addition of an OEWC doubles the effort.
    // Therefore we have to skip the edge to the next OEWC.
    Rotate(parse_tree_node->_third_sibbling,  &parse_tree_node->_third_sibbling);
  }
  // Then we can rotate our node
  if (NeedsLeftRotation(parse_tree_node)) {
    RotateLeft(parse_tree_node, pointer_to_parent_pointer_for_back_patching);
  }
  if (NeedsRightRotation(parse_tree_node)) {
    RotateRight(parse_tree_node, pointer_to_parent_pointer_for_back_patching);
  }
}

void CParseTreeRotator::RotateLeftAsLongAsNecessary(TPParseTreeNode parse_tree_node,
    TPParseTreeNode *pointer_to_parent_pointer_for_back_patching) {
  // Initial naive parse-tree
  //
  //     **
  //    / \
  //   2   *
  //      / \
  //     3   +
  //        / \
  //       4   5
  //
  // After first rotation  
  //         *
  //       /   \
  //     **     +
  //    / \    / \
  //   2   3  4   5
  //
  // Now we need one more rotation of the (*) node.
  // Previouslt we did a naive Rotate(), 
  // but this causes cubic effort, as all sub-trees
  // get rotated recursively again.
  // As all subtrees are already correctly ordered
  // we only need one (or more) left-rotations
  // of this node again.
  //          + 
  //         / \
  //        *   5
  //       / \
  //     **   4
  //    / \    
  //   2   3 
  //
  write_log(Preferences()->debug_ast_priority_ordering(),
    "[CParseTreeRotator] rotating node to left as long as necessary %x\n", parse_tree_node);
  if (parse_tree_node == NULL) {
    return;
  }
  // All sibblings already rotated, bottom-up
  if (NeedsLeftRotation(parse_tree_node)) {
    RotateLeft(parse_tree_node, pointer_to_parent_pointer_for_back_patching);
  }
}

bool CParseTreeRotator::NeedsLeftRotation(TPParseTreeNode parse_tree_node) {
  if (parse_tree_node == NULL) {
    return false;
  }
  TPParseTreeNode right_most_sibbling = parse_tree_node->GetRightMostSibbling();
  if (right_most_sibbling == NULL) {
    return false;
  }
  int sibblings_type = right_most_sibbling->_node_type;
  if ((sibblings_type == kTokenIdentifier) 
      || (sibblings_type == kTokenNumber)) {
    // Nothing to do for identifiers and numbers
    // We can~t rotate them to the position of an operator node.
    return false;
  } 
  if (TokenIsBracketOpen(sibblings_type)) {
    // Nothing to do for bracket-expressions as operands.
    // They are "unary" and the parser took care about correct recognition
    return false;
  }
  if (TokenIsUnary(sibblings_type)) {
    // Nothing to do for unary expressions as right-hand-operands
    // e.g. "2 + !false": we clearly can't move the negation to the top.
    return false;
  }
  if (sibblings_type == kTokenOperatorConditionalWhen) {
    // Nothing to do for when-conditions
    // We already got them right when constructing the parse-tree
    return false;
  }
  int sibblings_priority = GetOperatorPriority(sibblings_type);
  if (sibblings_priority == kUndefined) {
    // Something that is not an operator
    return false;
  }
  int my_priority = GetOperatorPriority(parse_tree_node->_node_type);
  if (sibblings_priority < my_priority) {
    return true;
  }
  if ((sibblings_priority == my_priority) 
      && !IsOperatorRightAssociativ(parse_tree_node->_node_type)
      && !TokenIsUnary(right_most_sibbling->_node_type)) {
    // No rotation for unary sibbling of equal priority
    // Otherwise we get an endless recusion for expressions like "NOT NOT NOT true".
    return true;
  }
  return false;
}

void CParseTreeRotator::RotateLeft(TPParseTreeNode parse_tree_node,
                                   TPParseTreeNode *pointer_to_parent_pointer_for_back_patching) {
  assert(parse_tree_node != NULL);
  TPParseTreeNode right_sibbling = parse_tree_node->GetRightMostSibbling();
  assert(right_sibbling != NULL);
  TPParseTreeNode right_left_sibbling = right_sibbling->GetLeftMostSibbling();
  parse_tree_node->SetRightMostSibbling(right_left_sibbling);
  right_sibbling->SetLeftMostSibbling(parse_tree_node);
  // Make parent pointer point to right_sibbling
  // instead of parse_tree_node
  TPParseTreeNode new_parent_node = right_sibbling;
  *pointer_to_parent_pointer_for_back_patching = right_sibbling;
  TPParseTreeNode new_left_sibbling = parse_tree_node;
  TPParseTreeNode *new_pointer_to_parent_pointer_for_back_patching = &new_parent_node->_first_sibbling; //&right_sibbling;
  RotateLeftAsLongAsNecessary(new_left_sibbling/*right_sibbling*/, new_pointer_to_parent_pointer_for_back_patching);
}

bool CParseTreeRotator::NeedsRightRotation(TPParseTreeNode parse_tree_node) {
  return false;
}

void CParseTreeRotator::RotateRight(TPParseTreeNode parse_tree_node,
                                    TPParseTreeNode *pointer_to_parent_pointer_for_back_patching) {
}

void CParseTreeRotator::VerifyCorrectRotation(TPParseTreeNode parse_tree_node) {
#ifndef _DEBUG
  // Nothing to do in release-mode
  return;
#endif
  // Nothing to do for empty trees
  if (parse_tree_node == NULL) return;
  // Sibblings first
  VerifyCorrectRotation(parse_tree_node->_first_sibbling);
  VerifyCorrectRotation(parse_tree_node->_second_sibbling);
  if (!parse_tree_node->IsOpenEndedWhenCondition()) {
    // Again skip nodes that can be reached on multiple paths
    // to avoid exponential effort.
    VerifyCorrectRotation(parse_tree_node->_third_sibbling);
  }
  // Finally this node
  assert(!NeedsLeftRotation(parse_tree_node));
  assert(!NeedsRightRotation(parse_tree_node));
}

