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

#ifndef INC_CSYMBOLENGINEACTIVEDEALTPLAYING_H
#define INC_CSYMBOLENGINEACTIVEDEALTPLAYING_H

#include "CVirtualSymbolEngine.h"
#include "CSymbolEngineUserchair.h"
#include "NumericalFunctions.h"

class CSymbolEngineActiveDealtPlaying: public CVirtualSymbolEngine {
 public:
	CSymbolEngineActiveDealtPlaying();
	~CSymbolEngineActiveDealtPlaying();
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
	bool EvaluateSymbol(const CString name, double *result, bool log = false);
	int playersactivebits()		{ return _playersactivebits & playersseatedbits(); }
	int opponentsactivebits()	{ return (playersactivebits() & ~userchairbit());}
	int nplayersactive()		  { return bitcount(playersactivebits()); }
	int nopponentsactive()		{ return bitcount(opponentsactivebits()); }
 public:
	int playersplayingbits()	  { return _playersplayingbits & playersseatedbits(); }
	int opponentsplayingbits()	{ return (playersplayingbits() & ~userchairbit());}
	int nplayersplaying()		    { return bitcount(playersplayingbits()); }
	int nopponentsplaying()	  	{ return bitcount(opponentsplayingbits()); }
public:
	int playersdealtbits()		{ return _playersdealtbits; }
	int opponentsdealtbits()	{ return (_playersdealtbits & ~userchairbit());}
	int nplayersdealt()			  { return bitcount(playersdealtbits()); }
	int nopponentsdealt()		  { return bitcount(opponentsdealtbits()); }
 public:
	int playersseatedbits()		{ return _playersseatedbits; }
	int opponentsseatedbits()	{ return (_playersseatedbits & ~userchairbit());}
	int nplayersseated()		  { return bitcount(playersseatedbits()); }
	int nopponentsseated()		{ return bitcount(opponentsseatedbits()); }
 public:
	// Especially useful for the case when we are only interested in opponents
	// and calculate that value from players, subtracting the userchair.
	int userchairbit()				{ return 1 << USER_CHAIR; }
 public:
   int maxnplayersdealt()   { return _maxnplayersdealt; }
 public:
  CString SymbolsProvided();
 private:
	void CalculateActiveBits();
	void CalculatePlayingBits();
	void CalculateDealtBits();
	void CalculateSeatedBits();
private:
	int _playersactivebits;
	int _playersplayingbits;
	int _playersdealtbits;
	int _playersseatedbits;
  int _maxnplayersdealt;
};

extern CSymbolEngineActiveDealtPlaying *p_symbol_engine_active_dealt_playing;

#endif INC_CSYMBOLENGINEACTIVEDEALTPLAYING_H