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

#include "stdafx.h"
#include "TokenizerConstants.h"

#include <assert.h>

const bool kTokenIsUnary[kNumberOfTokens] = {
  // kTokenEndOfFile = 0,
  false,
  // kTokenEndOfFunction,
  false,
  // kTokenOperatorPlus,               
  false,
  // kTokenOperatorMinus,              
  false,
  // kTokenOperatorMultiplication,     
  false,
  // kTokenOperatorDivision,           
  false,
  // kTokenOperatorUnaryMinus
  true,
  // kTokenOperatorModulo,             
  false,
  // kTokenOperatorExponentiation,     
  false,
  // kTokenOperatorLog,                
  true,
  // kTokenOperatorEquality,           
  false,
  // kTokenOperatorApproximatellyEqual,
  false,
  // kTokenOperatorSmaller,            
  false,
  // kTokenOperatorSmallerOrEqual,     
  false,
  // kTokenOperatorGreater,           
  false,
  // kTokenOperatorGreaterOrEqual,     
  false,
  // kTokenOperatorNotEqual,           
  false,
  // kTokenOperatorLogicalAnd,        
  false,
  // kTokenOperatorLogicalOr,	    
  false,
  // kTokenOperatorLogicalNot,        
  true,
  // kTokenOperatorLogicalXOr,       
  false,
  // kTokenOperatorBinaryAnd,          
  false,
  // kTokenOperatorBinaryOr,           
  false,
  // kTokenOperatorBinaryNot,          
  true,
  // kTokenOperatorBinaryXOr,          
  false,
  // kTokenOperatorBitShiftLeft,      
  false,
  // kTokenOperatorBitShiftRight,     
  false,
  // kTokenOperatorBitCount,
  true,
  // kTokenOperatorPercentage,         
  false,
  // kTokenOperatorConditionalIf,      
  false,
  // kTokenOperatorConditionalThen,    
  false,
  // kTokenOperatorConditionalWhen,    
  false,
  // kTokenBracketOpen_1,              			
  true,
  // kTokenBracketOpen_2,              
  true,
  // kTokenBracketOpen_3,              
  true,
  // kTokenBracketClose_1,            
  false,
  // kTokenBracketClose_2,            
  false,
  // kTokenBracketClose_3,           
  false,
  // kTokenIdentifier,
  false,
  // kTokenNumber,
  false,
  // kTokenCards,
  false,
  // kTokenDoubleShebang,
  false,
};

const bool kTokenIsBinary[kNumberOfTokens] = {
  // kTokenEndOfFile = 0,
  false,
  // kTokenEndOfFunction,
  false,
  // kTokenOperatorPlus,               
  true,
  // kTokenOperatorMinus, 
  true,
  // kTokenOperatorMultiplication,     
  true,
  // kTokenOperatorDivision,           
  true,
  // kTokenOperatorUnaryMinus,
  false,
  // kTokenOperatorModulo,             
  true,
  // kTokenOperatorExponentiation,     
  true,
  // kTokenOperatorLog,                
  false,
  // kTokenOperatorEquality,           
  true,
  // kTokenOperatorApproximatellyEqual,
  true,
  // kTokenOperatorSmaller,            
  true,
  // kTokenOperatorSmallerOrEqual,     
  true,
  // kTokenOperatorGreater,           
  true,
  // kTokenOperatorGreaterOrEqual,     
  true,
  // kTokenOperatorNotEqual,           
  true,
  // kTokenOperatorLogicalAnd,        
  true,
  // kTokenOperatorLogicalOr,	    
  true,
  // kTokenOperatorLogicalNot,        
  false,
  // kTokenOperatorLogicalXOr,       
  true,
  // kTokenOperatorBinaryAnd,          
  true,
  // kTokenOperatorBinaryOr,           
  true,
  // kTokenOperatorBinaryNot,          
  false,
  // kTokenOperatorBinaryXOr,          
  true,
  // kTokenOperatorBitShiftLeft,      
  true,
  // kTokenOperatorBitShiftRight,     
  true,
  // kTokenOperatorBitCount,
  false,
  // kTokenOperatorPercentage,         
  true,
  // kTokenOperatorConditionalIf,      
  false,
  // kTokenOperatorConditionalThen,    
  false,
  // kTokenOperatorConditionalWhen,    
  false, 
  // kTokenBracketOpen_1,              			
  false,
  // kTokenBracketOpen_2,              
  false,
  // kTokenBracketOpen_3,              
  false,
  // kTokenBracketClose_1,            
  false,
  // kTokenBracketClose_2,            
  false,
  // kTokenBracketClose_3,           
  false,
  // kTokenIdentifier,
  false,
  // kTokenNumber,
  false,
  // kTokenCards,
  false,
  // kTokenDoubleShebang,
  false,
};

bool TokenIsUnary(int token) {
  assert(token >=0);
  assert(token < kNumberOfTokens);
  return kTokenIsUnary[token];
}

bool TokenIsBinary(int token) {
  assert(token >= 0);
  assert(token < kNumberOfTokens);
  return kTokenIsBinary[token];
}

CString TokenString(int token) {
  switch (token)  {
	case kTokenEndOfFile: return "End of file";
	case kTokenEndOfFunction: return "End of function";
	case kTokenOperatorPlus: return "+";               
	case kTokenOperatorMinus: return "-";
	case kTokenOperatorMultiplication: return "*";    
	case kTokenOperatorDivision: return "/";
  case kTokenOperatorUnaryMinus: return "-";
	case kTokenOperatorModulo: return "modulo";	// because % can also mean percentage
	case kTokenOperatorExponentiation: return "**";
	case kTokenOperatorLog: return "ln";
	case kTokenOperatorEquality: return "=="; 
	case kTokenOperatorApproximatellyEqual: return "~~";
	case kTokenOperatorSmaller: return "<";     
	case kTokenOperatorSmallerOrEqual: return "<=";
	case kTokenOperatorGreater: return ">";     
	case kTokenOperatorGreaterOrEqual: return ">=";
	case kTokenOperatorNotEqual: return "!=";           
	case kTokenOperatorLogicalAnd: return "AND";        
	case kTokenOperatorLogicalOr: return "OR";	    
	case kTokenOperatorLogicalNot: return "NOT";        
	case kTokenOperatorLogicalXOr: return "XOR"; 
	case kTokenOperatorBinaryAnd: return "BitAnd";
	case kTokenOperatorBinaryOr: return "BitOr";           
	case kTokenOperatorBinaryNot: return "BitNot";          
	case kTokenOperatorBinaryXOr: return "BitXOr";    
	case kTokenOperatorBitShiftLeft: return "<<";      
	case kTokenOperatorBitShiftRight: return ">>";
  case kTokenOperatorBitCount: return "BitCount";
	case kTokenOperatorPercentage: return "percentage"; // because % can also mean modulo      
	case kTokenOperatorConditionalIf: return "?";     
  case kTokenOperatorConditionalElse: return ":";
	case kTokenOperatorConditionalWhen: return "WHEN";
	case kTokenBracketOpen_1: return "(";              			
	case kTokenBracketOpen_2: return "[";              
	case kTokenBracketOpen_3: return "{";              
	case kTokenBracketClose_1: return ")";            
	case kTokenBracketClose_2: return "]";            
	case kTokenBracketClose_3: return "}";
	case kTokenIdentifier: return "identifier";
	case kTokenNumber: return "number";
	case kTokenCards: return "cards";
	case kTokenDoubleShebang: return "##";
  case kTokenActionBeep: return "Beep";
  case kTokenActionFold: return "Fold";
  case kTokenActionCheck: return "Check";
  case kTokenActionCall: return "Call";
  case kTokenActionRaise: return "Raise";
  case kTokenActionRaiseTo: return "RaiseTo";
  case kTokenActionRaiseBy: return "RaiseBy";
  // case kTokenActionRaiseMin: return "RaiseMin";
  case kTokenActionRaiseFourthPot: return "RaiseFourthPot";
  case kTokenActionRaiseThirdPot: return "RaiseThirdPot";
  case kTokenActionRaiseHalfPot: return "RaiseHalfPot";
  case kTokenActionRaiseTwoThirdPot: return "RaiseTwoThirdPot";
  case kTokenActionRaiseThreeFourthPot: return "RaiseThreeFourthPot";
  case kTokenActionRaisePot: return "RaisePot";
  case kTokenActionRaiseMax: return "RaiseMax";
  case kTokenActionReturn: return "Return";
  case kTokenShankyKeywordSitOut: return "SitOut";
  case kTokenActionUserVariableToBeSet: return "Set";
  case kTokenKeywordForce: return "Force";
  case kTokenUnsupportedDelay: return "Delay";
  case kTokenShankyKeywordHand: return "hand";
  case kTokenShankyKeywordBoard: return "board";
  case kTokenShankyKeywordIn: return "in";
  case kTokenActionRaiseByBigBlinds: return "RaiseByBigBlinds";
  case kTokenActionRaiseToBigBlinds: return "RaiseToBigBlinds";
  case kTokenActionRaiseByPercentagedPotsize: return "RaiseByPercentagedPotsize";
  case kTokenOperatorSetHandWeight: return ":";
	default:
		assert(kThisMustNotHappen);
    CString error_message;
    error_message.Format("Error: unexpected token ID in TokenString(%i)", token);
		return error_message;
  }
}

CString TokenVerboseExplained(int token) {
  assert(token >= 0);
  assert(token < kNumberOfTokens);
  switch (token) {
  case kTokenEndOfFile: return "end of file";
  case kTokenEndOfFunction: return "end of function";
  case kTokenOperatorPlus: return "binary operator +";
  case kTokenOperatorMinus: return "binary operator -";
  case kTokenOperatorMultiplication: return "binary operator *";
  case kTokenOperatorDivision: return "binary operator /";
  case kTokenOperatorUnaryMinus: return "unary operator -";
  case kTokenOperatorModulo: return "binary operator modulo";	// because % can also mean percentage
  case kTokenOperatorExponentiation: return "binary operator exponentiation";
  case kTokenOperatorLog: return "unary operator ln";
  case kTokenOperatorEquality: return "binary operator ==";
  case kTokenOperatorApproximatellyEqual: return "binary operator ~~";
  case kTokenOperatorSmaller: return "binary operator <";
  case kTokenOperatorSmallerOrEqual: return "binary operator <=";
  case kTokenOperatorGreater: return "binary operator >";
  case kTokenOperatorGreaterOrEqual: return "binary operator >=";
  case kTokenOperatorNotEqual: return "binary operator !=";
  case kTokenOperatorLogicalAnd: return "binary operator AND";
  case kTokenOperatorLogicalOr: return "binary operator OR";
  case kTokenOperatorLogicalNot: return "binary operator NOT";
  case kTokenOperatorLogicalXOr: return "binary operator XOR";
  case kTokenOperatorBinaryAnd: return "binary operator BitAnd";
  case kTokenOperatorBinaryOr: return "binary operator BitOr";
  case kTokenOperatorBinaryNot: return "binary operator BitNot";
  case kTokenOperatorBinaryXOr: return "binary operator BitXOr";
  case kTokenOperatorBitShiftLeft: return "binary operator <<";
  case kTokenOperatorBitShiftRight: return "binary operator >>";
  case kTokenOperatorBitCount: return "unary operator BitCount";
  case kTokenOperatorPercentage: return "binary operator percentage"; // because % can also mean modulo      
  case kTokenOperatorConditionalIf: return "conditional operator ?";
  case kTokenOperatorConditionalElse: return "conditional operator :";
  case kTokenOperatorConditionalWhen: return "start of WHEN condition";
  case kTokenBracketOpen_1: return "opening bracket (";
  case kTokenBracketOpen_2: return "opening bracket [";
  case kTokenBracketOpen_3: return "opening bracket {";
  case kTokenBracketClose_1: return "closing bracket )";
  case kTokenBracketClose_2: return "closing bracket]";
  case kTokenBracketClose_3: return "closing bracket";
  case kTokenIdentifier: return "identifier";
  case kTokenNumber: return "number";
  case kTokenCards: return "cards";
  case kTokenDoubleShebang: return "##";
  case kTokenActionBeep: return "action Beep";
  case kTokenActionFold: return "action Fold";
  case kTokenActionCheck: return "action Check";
  case kTokenActionCall: return "action Call";
  case kTokenActionRaise: return "action Raise";
  case kTokenActionRaiseTo: return "action RaiseTo";
  case kTokenActionRaiseBy: return "action RaiseBy";
    // case kTokenActionRaiseMin: return "RaiseMin";
  case kTokenActionRaiseFourthPot: return "action RaiseFourthPot";
  case kTokenActionRaiseThirdPot: return "action RaiseThirdPot";
  case kTokenActionRaiseHalfPot: return "action RaiseHalfPot";
  case kTokenActionRaiseTwoThirdPot: return "action RaiseTwoThirdPot";
  case kTokenActionRaiseThreeFourthPot: return "action RaiseThreeFourthPot";
  case kTokenActionRaisePot: return "action RaisePot";
  case kTokenActionRaiseMax: return "action RaiseMax";
  case kTokenActionReturn: return "keyword Return";
  case kTokenShankyKeywordSitOut: return "action SitOut";
  case kTokenActionUserVariableToBeSet: return "action Set";
  case kTokenKeywordForce: return "keyword Force";
  case kTokenUnsupportedDelay: return "unsupported Shanky-style Delay";
  case kTokenShankyKeywordHand: return "Shanky-style \"hand\"-expression";
  case kTokenShankyKeywordBoard: return "Shanky-style \"board\"-expression";
  case kTokenShankyKeywordIn: return "keyword In (part of Shanky-style position)";
  case kTokenActionRaiseByBigBlinds: return "RaiseByBigBlinds";
  case kTokenActionRaiseToBigBlinds: return "RaiseToBigBlinds";
  case kTokenActionRaiseByPercentagedPotsize: return "RaiseByPercentagedPotsize";
  case kTokenOperatorSetHandWeight: return "set handweight operator :";
  default:
    assert(kThisMustNotHappen);
    CString error_message;
    error_message.Format("Error: unexpected token ID in TokenVerboseExplained((%i)", token);
    return error_message;
  }
}

const int kOperatorPriority[kNumberOfTokens] = {
  // kTokenEndOfFile
  kUndefined,
  // kTokenEndOfFunction,
  kUndefined,
  // kTokenOperatorPlus,    
  11,
  // kTokenOperatorMinus,              
  11,
  // kTokenOperatorMultiplication, 
  12,
  // kTokenOperatorDivision,           
  12,
  // kTokenOperatorUnaryMinus,
  13,
  // kTokenOperatorModulo,             
  12,
  // kTokenOperatorExponentiation,    
  14,
  // kTokenOperatorLog,                
  14,
  // kTokenOperatorEquality,           
  8,
  // kTokenOperatorApproximatellyEqual,
  8,
  // kTokenOperatorSmaller,            
  9,
  // kTokenOperatorSmallerOrEqual,     
  9,
  // kTokenOperatorGreater,           
  9,
  // kTokenOperatorGreaterOrEqual,     
  9,
  // kTokenOperatorNotEqual,           
  8,
  // kTokenOperatorLogicalAnd,        
  4,
  // kTokenOperatorLogicalOr,	    
  2,
  // kTokenOperatorLogicalNot,        
  13,
  // kTokenOperatorLogicalXOr,       
  3,
  // kTokenOperatorBinaryAnd,
  7,
  // kTokenOperatorBinaryOr,           
  5,
  // kTokenOperatorBinaryNot,          
  13,
  // kTokenOperatorBinaryXOr,          
  6,
  // kTokenOperatorBitShiftLeft,      
  10,
  // kTokenOperatorBitShiftRight,     
  10,
  // kTokenOperatorBitCount, 
  13,
  // kTokenOperatorPercentage, 
  12,   // Like multiplication
  // kTokenOperatorConditionalIf,      
  1,
  // kTokenOperatorConditionalThen,    
  kUndefined,
  // kTokenOperatorConditionalWhen,    
  1,    // Like conditional if
  // kTokenBracketOpen_1, 
  //   Brackets have LOWEST priority as everything inside brackets 
  //   gets evaluated first.
  0,
  // kTokenBracketOpen_2,              
  0,
  // kTokenBracketOpen_3,              
  0,
  // kTokenBracketClose_1,            
  kUndefined,
  // kTokenBracketClose_2,            
  kUndefined,
  // kTokenBracketClose_3,           
  kUndefined,
  // kTokenIdentifier,
  kUndefined,
  // kTokenNumber,
  kUndefined,
  // kTokenCards,
  kUndefined,
  // kTokenDoubleShebang,
  kUndefined,
};

int GetOperatorPriority(int token) {
  assert(token >= 0);
  assert(token < kNumberOfTokens);
  return kOperatorPriority[token];
}

const bool kTokenEvaluatesToBinaryNumber[kNumberOfTokens] = {
  // kTokenEndOfFile = 0,
  false,
  // kTokenEndOfFunction,
  false,
  // kTokenOperatorPlus,               
  false,
  // kTokenOperatorMinus,              
  false,
  // kTokenOperatorMultiplication,     
  false,
  // kTokenOperatorDivision,           
  false,
  // kTokenOperatorUnaryMinus
  false,
  // kTokenOperatorModulo,             
  false,
  // kTokenOperatorExponentiation,     
  false,
  // kTokenOperatorLog,                
  false,
  // kTokenOperatorEquality,           
  false,
  // kTokenOperatorApproximatellyEqual,
  false,
  // kTokenOperatorSmaller,            
  false,
  // kTokenOperatorSmallerOrEqual,     
  false,
  // kTokenOperatorGreater,           
  false,
  // kTokenOperatorGreaterOrEqual,     
  false,
  // kTokenOperatorNotEqual,           
  false,
  // kTokenOperatorLogicalAnd,        
  false,
  // kTokenOperatorLogicalOr,	    
  false,
  // kTokenOperatorLogicalNot,        
  false,
  // kTokenOperatorLogicalXOr,       
  false,
  // kTokenOperatorBinaryAnd,          
  true,
  // kTokenOperatorBinaryOr,           
  true,
  // kTokenOperatorBinaryNot,          
  true,
  // kTokenOperatorBinaryXOr,          
  true,
  // kTokenOperatorBitShiftLeft,      
  true,
  // kTokenOperatorBitShiftRight,     
  true,
  // kTokenOperatorBitCount,
  true,
  // kTokenOperatorPercentage,         
  false,
  // kTokenOperatorConditionalIf,      
  false,
  // kTokenOperatorConditionalThen,    
  false,
  // kTokenOperatorConditionalWhen,    
  false,
  // kTokenBracketOpen_1,              			
  false,
  // kTokenBracketOpen_2,              
  false,
  // kTokenBracketOpen_3,              
  false,
  // kTokenBracketClose_1,            
  false,
  // kTokenBracketClose_2,            
  false,
  // kTokenBracketClose_3,           
  false,
  // kTokenIdentifier,
  false,
  // kTokenNumber,
  false,
  // kTokenCards,
  false,
  // kTokenDoubleShebang,
  false,
};

bool TokenEvaluatesToBinaryNumber(int token) {
  assert(token >= 0);
  assert(token < kNumberOfTokens);
  return kTokenEvaluatesToBinaryNumber[token];
}

/*
kTokenEndOfFile = 0,
	kTokenEndOfFunction,
	kTokenOperatorPlus,               
	kTokenOperatorMinus,              
	kTokenOperatorMultiplication,     
	kTokenOperatorDivision,           
  kTokenOperatorUnaryMinus,
	kTokenOperatorModulo,             
	kTokenOperatorExponentiation,     
	kTokenOperatorLog,                
	kTokenOperatorEquality,           
	kTokenOperatorApproximatellyEqual,
	kTokenOperatorSmaller,            
	kTokenOperatorSmallerOrEqual,     
	kTokenOperatorGreater,           
	kTokenOperatorGreaterOrEqual,     
	kTokenOperatorNotEqual,           
	kTokenOperatorLogicalAnd,        
	kTokenOperatorLogicalOr,	    
	kTokenOperatorLogicalNot,        
	kTokenOperatorLogicalXOr,       
	kTokenOperatorBinaryAnd,          
	kTokenOperatorBinaryOr,           
	kTokenOperatorBinaryNot,          
	kTokenOperatorBinaryXOr,          
	kTokenOperatorBitShiftLeft,      
	kTokenOperatorBitShiftRight,     
  kTokenOperatorBitCount,
	kTokenOperatorPercentage,         
	kTokenOperatorConditionalIf,      
	kTokenOperatorConditionalThen,    
	kTokenOperatorConditionalWhen,    
	kTokenBracketOpen_1,              			
	kTokenBracketOpen_2,              
	kTokenBracketOpen_3,              
	kTokenBracketClose_1,            
	kTokenBracketClose_2,            
	kTokenBracketClose_3,           
	kTokenIdentifier,
	kTokenNumber,
	kTokenCards,
	kTokenDoubleShebang,
	*/