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
#include "SLimitInfo.h"

CLimitInfo::CLimitInfo() {
  Reset();
}

CLimitInfo::~CLimitInfo() {
}

void CLimitInfo::Reset() {
  _handnumber = "";
  _sblind.Reset();
  _bblind.Reset();
  _bbet.Reset();
  _ante.Reset();
  _sb_bb.Reset();
  _bb_BB.Reset();
  _limit = kUndefinedZero;
  _buyin.Reset();
  _prizepool.Reset();
  _is_final_table = false;
}

double CLimitInfo::sblind() {
  if (_sblind.GetValue() > kUndefinedZero) {
    return _sblind.GetValue();
  }
  if (_limit != kGametypeFL) {
    if (_sb_bb.GetValue() > kUndefinedZero) {
      return _sb_bb.GetValue();
    }
  } else {
    // Info not available for FL games
  }
  return kUndefinedZero;
}

double CLimitInfo::bblind() {
  if (_bblind.GetValue() > kUndefinedZero) {
    return _bblind.GetValue();
  }
  if (_limit == kGametypeFL) {
    if (_sb_bb.GetValue() > kUndefinedZero) {
      return _sb_bb.GetValue();
    } else {
      return _bb_BB.GetValue();
    }
  }
  return kUndefinedZero;
}

double CLimitInfo::bbet() {
  if (_bbet.GetValue() > kUndefinedZero) {
    return _bbet.GetValue();
  }
  if (_limit == kGametypeFL) {
    if (_bb_BB.GetValue() > kUndefinedZero) {
      return _bb_BB.GetValue();
    }
  } else {
    // Info not available for NL and PL games
  }
  return kUndefinedZero;
}