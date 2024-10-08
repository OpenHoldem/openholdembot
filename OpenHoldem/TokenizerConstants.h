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

#ifndef INC_TOKENIZERCONSTANTS_H
#define INC_TOKENIZERCONSTANTS_H

enum {
  // End of file
  kTokenEndOfFile = 0,
  kTokenEndOfFunction,
  // Operators
  kTokenOperatorPlus,               // +
  kTokenOperatorMinus,              // -
  kTokenOperatorMultiplication,     // *
  kTokenOperatorDivision,           // /
  kTokenOperatorUnaryMinus,         // -
  kTokenOperatorModulo,             // % (OpenHoldem)
  kTokenOperatorExponentiation,     // **
  kTokenOperatorLog,                // ln
  kTokenOperatorEquality,           // == (OpenHoldem) = (OpenPPL)
  kTokenOperatorApproximatellyEqual,// ~~ 
  kTokenOperatorSmaller,            // <
  kTokenOperatorSmallerOrEqual,     // <= 
  kTokenOperatorGreater,            // >
  kTokenOperatorGreaterOrEqual,     // >=
  kTokenOperatorNotEqual,           // !=
  kTokenOperatorLogicalAnd,         // &&
  kTokenOperatorLogicalOr,	        // ||
  kTokenOperatorLogicalNot,         // !
  kTokenOperatorLogicalXOr,         // ^^
  kTokenOperatorBinaryAnd,          // &
  kTokenOperatorBinaryOr,           // |
  kTokenOperatorBinaryNot,          // ~
  kTokenOperatorBinaryXOr,          // ^
  kTokenOperatorBitShiftLeft,       // <<
  kTokenOperatorBitShiftRight,      // >>
  kTokenOperatorBitCount,           // `	
  kTokenOperatorPercentage,         // % (OpenPPL)
  kTokenOperatorConditionalIf,      // ?
  kTokenOperatorConditionalElse,    // :
  kTokenOperatorConditionalWhen,    // WHEN
  // Brackets
  kTokenBracketOpen_1,              // (				
  kTokenBracketOpen_2,              // [
  kTokenBracketOpen_3,              // {
  kTokenBracketClose_1,             // )
  kTokenBracketClose_2,             // ]
  kTokenBracketClose_3,             // }
  kTokenOperatorSetHandWeight,		// :
  kTokenOperatorOpenEndedHandRange, // +
  kTokenOperatorHandRangeGroup,		// -
  // Numbers, Identifier, Cards
  kTokenIdentifier,
  kTokenNumber,
  kTokenCards,
  // ## as start and end of list and function-headers
  kTokenDoubleShebang,              // ##
  // OpenPPL actions
  kTokenActionBeep,
  kTokenActionFold,
  kTokenActionCheck,
  kTokenActionCall,
  kTokenActionRaise,                // Also eqauls MinRaise, Bet and MinBet
  kTokenActionRaiseTo,              
  kTokenActionRaiseBy,
  kTokenActionRaiseFourthPot,
  kTokenActionRaiseThirdPot,
  kTokenActionRaiseHalfPot,
  kTokenActionRaiseTwoThirdPot,
  kTokenActionRaiseThreeFourthPot,
  kTokenActionRaisePot,
  kTokenActionRaiseMax,
  kTokenActionReturn,
  // Shanky command "Sitout", meaning first fold, then sitout
  // Not really supported, we just fold
  kTokenShankyKeywordSitOut,
  kTokenActionUserVariableToBeSet,
  // OpenPPL keyword FORCE
  kTokenKeywordForce,
  // Shanky-style delay (unsupported)
  kTokenUnsupportedDelay,
  // Shankly style hand- and board expressions.
  // Token gets only used for importing Shanky-PPL;
  // the parse represents these expressions as identifiers
  // (hand$XYZ, board$XYZ)
  kTokenShankyKeywordHand,
  kTokenShankyKeywordBoard,
  // Shanky-style "In BigBlind" instead of "InBigBlind".
  // Valid code, as Shanky analyses a character-stream,
  // spaces stripped away ;-)
  kTokenShankyKeywordIn,
  // Special action-constants for node-types
  // Not really tokens, but handled here for consistency
  kTokenActionRaiseByBigBlinds,
  kTokenActionRaiseToBigBlinds,
  kTokenActionRaiseByPercentagedPotsize,
  // Always leave that at the very end
  kNumberOfTokens,
};

const int kNumberOfOpenPPLActions = 28;

const char* const kOpenPPLActionStrings[kNumberOfOpenPPLActions] = {
  // No longer considering about (OpenPPL 1.x)
  // * Leave
  // * Close
  // Because they will be handled by secondary OH-functions
  "Bet",
  "Call",
  "Fold",
  "Play",
  "Beep",
  "Raise",
  "RaiseTo",
  "RaiseBy",
  "Check",
  "Allin",
  "BetMin",
  "BetFourthPot",
  "BetThirdPot",
  "BetHalfPot",
  "BetTwoThirdPot",
  "BetThreeFourthPot",
  "BetPot",
  "BetMax",
  "RaiseMin",
  "RaiseFourthPot",
  "RaiseThirdPot",
  "RaiseHalfPot",
  "RaiseTwoThirdPot",
  "RaiseThreeFourthPot",
  "RaisePot",
  "RaiseMax",
  "SitOut",
  "Set",
};

const int kOpenPPLActionConstants[kNumberOfOpenPPLActions] = {
  // Duplicates are possible, because for example
  // "Bet" and "Raise" are technically both kTokenActionRaise.
  kTokenActionRaise, 
  kTokenActionCall,
  kTokenActionFold,
  kTokenActionCall,
  kTokenActionBeep,
  kTokenActionRaise,
  kTokenActionRaiseTo,
  kTokenActionRaiseBy,
  kTokenActionCheck,
  kTokenActionRaiseMax,
  kTokenActionRaise,
  kTokenActionRaiseFourthPot,
  kTokenActionRaiseThirdPot,
  kTokenActionRaiseHalfPot,
  kTokenActionRaiseTwoThirdPot,
  kTokenActionRaiseThreeFourthPot,
  kTokenActionRaisePot,
  kTokenActionRaiseMax,
  kTokenActionRaise,
  kTokenActionRaiseFourthPot,
  kTokenActionRaiseThirdPot,
  kTokenActionRaiseHalfPot,
  kTokenActionRaiseTwoThirdPot,
  kTokenActionRaiseThreeFourthPot,
  kTokenActionRaisePot,
  kTokenActionRaiseMax,
  kTokenShankyKeywordSitOut,
  kTokenActionUserVariableToBeSet,
};

inline bool TokenIsBracketOpen(int token) {
  return ((token == kTokenBracketOpen_1)
    || (token == kTokenBracketOpen_2)
	  || (token == kTokenBracketOpen_3));
}

bool TokenIsUnary(int token);

bool TokenIsBinary(int token);

inline bool TokenIsTernary(int token) {
  return ((token == kTokenOperatorConditionalIf)
    || (token == kTokenOperatorConditionalWhen));
}

inline bool TokenIsElementaryAction(int token) {
  return ((token >= kTokenActionBeep)
    && (token <= kTokenActionUserVariableToBeSet));
}

inline bool TokenIsOpenPPLAction(int token) {
  return (TokenIsElementaryAction(token));
}

inline bool TokenIsBracketClose(int token) {
  return ((token == kTokenBracketClose_1)
    || (token == kTokenBracketClose_2)
	|| (token == kTokenBracketClose_3));
}

int GetOperatorPriority(int token);

// Right assopciativ: only **, ln and conditional
inline int IsOperatorRightAssociativ(int token) {
  return ((token == kTokenOperatorExponentiation)
    || (token == kTokenOperatorLog)
    || (token == kTokenOperatorConditionalIf));
}

// For debugging output
CString TokenString(int token);

// For the generation of verbose error-messages (unexpected token in bot-logic)
CString TokenVerboseExplained(int token);

// For smart display of binary numbers in debug-tab
bool TokenEvaluatesToBinaryNumber(int token);

#endif INC_TOKENIZERCONSTANTS_H