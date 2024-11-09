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

#ifndef INC_SLIMITINFO_H
#define INC_SLIMITINFO_H

#include "CScrapedMoney.h"

class CLimitInfo {
  friend class CTitleEvaluator;
 public:
  CLimitInfo();
  ~CLimitInfo();
 public:
  void Reset();
 public:
  // The functions below reurn values depending on game-state,
  // based on the info of
  //   * _sblind
  //   * _bblind
  //   * _bbet
  //   * _sb_bb
  //   * _bb_BB 
  double	sblind();
	double	bblind();
	double	bbet();
 public:
	 double	ante() {
		 if (_ante.GetValue() > kUndefinedZero) {
			 return _ante.GetValue();
		 }
		 else {
			 // Info not available for antes
		 }
		 return kUndefinedZero;
	 }
	int		  limit()          { return _limit; }
  CString	handnumber()     { return _handnumber; }
  double  buyin()         { return _buyin.GetValue(); }
  double  prizepool()         { return _prizepool.GetValue(); }
  double  prizepoolmultiplier()         { return _prizepoolmultiplier.GetValue(); }
  bool    is_final_table() { return _is_final_table; }
 protected:
  // FL / PL / NL
  int	_limit;
  // Handnumber should be a string, as
  //   * it may contain characters
  //   * its lengths my exceed the precision of CScrapedMoney
  CString	_handnumber;
  bool _is_final_table;
	CScrapedMoney	_sblind;
	CScrapedMoney	_bblind;
	CScrapedMoney	_bbet;
	CScrapedMoney	_ante;
	CScrapedMoney	_sb_bb;
	CScrapedMoney	_bb_BB;
    CScrapedMoney   _buyin;
	CScrapedMoney   _prizepool;
	CScrapedMoney   _prizepoolmultiplier;
};

#endif // INC_SLIMITINFO_H