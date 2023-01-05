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

#ifndef INC_CSymbolEngineRange_H
#define INC_CSymbolEngineRange_H

#include "CVirtualSymbolEngine.h"

typedef bool TRangeMatrix[kMaxNumberOfPlayers][k_number_of_ranks_per_deck][k_number_of_ranks_per_deck];
//   2 3 4 5 6 7 8 9 T J Q K A
// 2 P
// 3   A
// 4     I     SUITED  
// 5       R
// 6         S
// 7           P
// 8             A
// 9               I
// T                 R
// J                   S 
// Q    OFFSUITED        P
// K                       A   
// A                         I

typedef int TRangeWeightMatrix[kMaxNumberOfPlayers][k_number_of_ranks_per_deck][k_number_of_ranks_per_deck];
//   2 3 4 5 6 7 8 9 T J Q K A
// 2 P
// 3   A
// 4     I     SUITED  
// 5       R
// 6         S
// 7           P
// 8             A
// 9               I
// T                 R
// J                   S 
// Q    OFFSUITED        P
// K                       A   
// A                         I


enum Commands {
	kSetRange,
	KRemoveRange,
	kRemovePocketPair,
	kRemoveOffsuited,
	kRemoveSuited,
	kChangeWeight,
	kChangeAllWeight,
	// Always leave that at the very end
	kNumberOfCommands,
};
enum Operators {
	kNoOperator,
	kOperatorEqualTo,
	kOperatorSuperiorTo,
	kOperatorSuperiorEqualTo,
	kOperatorInferiorTo,
	kOperatorInferiorEqualTo,
	// Always leave that at the very end
	kNumberOfOperators,
};

class CSymbolEngineRange : public CVirtualSymbolEngine {
 public:
	CSymbolEngineRange();
	~CSymbolEngineRange();
 public:
	// Mandatory reset-functions
	void InitOnStartup();
	void UpdateOnConnection();
	void UpdateOnHandreset();
	void UpdateOnNewRound();
	void UpdateOnMyTurn();
	void UpdateOnHeartbeat();
 public:
	// Public accessors
	bool EvaluateSymbol(const CString name, CString *result, bool log = false);
	CString SymbolsProvided();
public:
	virtual CString get_range_text() { return _range_text; }
	void ClearAll();
	void UnSetLists();
	bool SetRange(CString range_symbol, CString range_value);
	// For Range manipulation: higher card first: suited, otherwise offsuited
	void Set(int first_rank, int second_rank, bool suited, int weight = 100);
	void Set(int first_rank, int second_rank, int weight = 100);
	void Unset(int first_rank, int second_rank, bool suited = true);
public:
	int NHandsOnRange();
	bool IsOnRange(int first_rank, int second_rank, bool suited);
	bool IsOnRange(int first_rank, int second_rank);
	bool IsEmpty() { return (NHandsOnRange() <= 0); }
public:
	// To be called for range manipulation
	void GenerateRangeTextFromRangeMatrix();
private:
	void UnSetChairList(int chairnumber);
	void cards_removal(int chairnumber, int r1, int r2, int s1, int s2);
	void add_single_hand(int chairnumber, int r1, int r2, int s1, int s2, int weight);
	void add_pocketpair_hand(int chairnumber, int r1, int weight);
	void add_offsuited_hand(int chairnumber, int r1, int r2, int weight);
	void add_suited_hand(int chairnumber, int r1, int r2, int weight);
	void del_single_hand(int chairnumber, int r1, int r2, int s1, int s2);
	void del_pocketpair_hand(int chairnumber, int r1);
	void del_offsuited_hand(int chairnumber, int r1, int r2);
	void del_suited_hand(int chairnumber, int r1, int r2);
	void weight_single_hand(int chairnumber, int r1, int r2, int s1, int s2, int old_weight, int new_weight);
	void weight_pocketpair_hand(int chairnumber, int r1, int old_weight, int new_weight);
	void weight_offsuited_hand(int chairnumber, int r1, int r2, int old_weight, int new_weight);
	void weight_suited_hand(int chairnumber, int r1, int r2, int old_weight, int new_weight);
	void weight_all_hand(int chairnumber, int old_weight, int new_weight);
 private:
	 void ErrorPrw1326NotActivated();
	 void ErrorPrw1326PreflopNotActivated();
	 void ErrorInvalidSelectRangeOperator(CString select_range_command);
	 void ErrorInvalidMember(CString range_member);
	 void ErrorOldStyleFormat(CString range_member);
	 void ErrorInvalidFormat();
private:
	 CString range(const int chair) {
		 assert((chair >= 0) || (chair == kUndefined));
		 assert(chair < kMaxNumberOfPlayers);
		 if (chair == kUndefined) {
			 return "";
		 }
		 return _range[chair];
	 }
	 bool isNumber(CString s)
	 {
		 char* p;
		 strtod(s, &p);
		 return *p == 0;
	 }
 private:
	 int _chairnumber, _weight;
	 CString _range[kMaxNumberOfPlayers];
	 TRangeMatrix _range_matrix;
	 TRangeWeightMatrix _range_weight_matrix;
protected:
	CString _range_text;
};

#endif INC_CSymbolEngineRange_H
