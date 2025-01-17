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

#ifndef INC_CPLAYER_H
#define INC_CPLAYER_H

#include "Card.h"
#include "CScrapedMoney.h"

class CPlayer {
 public:
  CPlayer();
  ~CPlayer();
 public:
  void Reset(bool is_resetting_dealer_data); 
 public:
  bool HasAnyCards();
  bool HasKnownCards();
 public:
  void CheckPlayerCardsForConsistency();
  CString Cards();
  CString CardsAsHTML();
 public:
  bool IsAllin();
  // Having to post both blinds at once happens at some casinos
  // http://www.maxinmontreal.com/forums/viewtopic.php?f=297&t=18948
  // http://www.maxinmontreal.com/forums/viewtopic.php?f=297&t=18948
  // This does not count as raising.
  bool PostingBothBlinds();
  bool PostingAnte();
 public:
  // Public accessors
  CString name()       { return _name; }
  int     colourcode() { return _colourcode; }
 public:
  // Public accessors
  Card* hole_cards(int index);
 public:
  // Public accessors
  bool seated() { return _seated; }
  bool active() { return _active; }
  bool dealer() { return _dealer; }
 public:
  // Public setters (must only be used by CScraper)
  void set_name(CString new_name)         { _name = new_name; }
  void set_colourcode(int new_colourcode) { _colourcode = new_colourcode; }
 public:
  // Public setters (must only be used by CScraper)
  void set_seated(bool is_seated, bool is_dealer);
  void set_active(bool is_active) { _active = is_active; }
  void set_dealer(bool is_dealer) { _dealer = is_dealer; }
 public:
  // For the log-file
  CString DataDump();
 public:
  double stack() { return (_balance.GetValue() + _bet.GetValue()); }
 public:
  CScrapedMoney _balance;
  CScrapedMoney _bet;
 private:
  CString _name;
  Card    _hole_cards[kMaxNumberOfCardsPerPlayer];
  int     _colourcode;
 private:
  bool _seated;
  bool _active;
  bool _dealer;
};

#endif INC_CPLAYER_H
