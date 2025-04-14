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

#include "pch.h" 
#include "CParseTreeTerminalNode.h"

#include <math.h>
#include "CAutoplayerTrace.h"
#include "CEngineContainer.h"
#include "CFunctionCollection.h"
#include "CMemoryPool.h"
#include "CParserSymbolTable.h"

#include "CSymbolEngineChipAmounts.h"
#include "CSymbolEngineMemorySymbols.h"
#include "CSymbolEngineOpenPPLUserVariables.h"
#include "CSymbolEngineTableLimits.h"
#include "FloatingPoint_Comparisions.h"

#include "..\DLLs\WindowFunctions_DLL\window_functions.h"
#include "..\DLLs\StringFunctions_DLL\string_functions.h"
#include "TokenizerConstants.h"

CParseTreeTerminalNode::CParseTreeTerminalNode(int relative_line_number) : 
    CParseTreeNode(relative_line_number) {
}

CParseTreeTerminalNode::~CParseTreeTerminalNode() {
}