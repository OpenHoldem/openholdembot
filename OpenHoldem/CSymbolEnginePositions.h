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

#ifndef INC_CSYMBOLENGINEPOSITIONS_H
#define INC_CSYMBOLENGINEPOSITIONS_H

#include "CVirtualSymbolEngine.h"
#include "CTransform/CTransform.h"

class CSymbolEnginePositions: public CVirtualSymbolEngine
{
public:
	CSymbolEnginePositions();
	~CSymbolEnginePositions();
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
	CString SymbolsProvided();;
public:
	int nchairsdealtright()		{ return _nchairsdealtright;	}
	int nchairsdealtleft()		{ return _nchairsdealtleft;		}
	int betpositionrais()		{ return _betpositionrais;		}
	int dealpositionrais()		{ return _dealpositionrais;		}
	int betposition()			{ return _betposition;			}
	int dealposition()			{ return _dealposition;			}
	int callposition()			{ return _callposition;			}
private:
	void CalculateNChairsDealtLeftRight();
	void CalculatePositionForTheRaiser();
	void CalculatePositionsForTheUserchair();
private:
	int _nchairsdealtright;
	int _nchairsdealtleft;
	int _betpositionrais;
	int _dealpositionrais;
	int _betposition;
	int _dealposition;
	int _callposition;
};

#endif INC_CSYMBOLENGINEPOSITIONS_H