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

#ifndef INC_STDAFX_H
#define INC_STDAFX_H

// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently,
// but are changed infrequently

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers
#endif

// Modify the following defines if you have to target a platform prior to the ones specified below.
// Refer to MSDN for the latest info on corresponding values for different platforms.
#ifndef WINVER				  // Allow use of features specific to Windows XP or later.
#define WINVER 0x0501		// Change this to the appropriate value to target other versions of Windows.
#endif

// Turn some warnings OFF, because we consider them harmless
// NO NEED TO DO IT

// Turn some warnings to ERRORS, because we consider them helpful
//
// https://www.google.de/url?sa=t&rct=j&q=&esrc=s&source=web&cd=2&cad=rja&uact=8&ved=0ahUKEwjqmYHZnMHMAhVHiywKHaB5DWUQFggiMAE&url=https%3A%2F%2Fmsdn.microsoft.com%2Fen-us%2Flibrary%2Faa233834%28v%3Dvs.60%29.aspx&usg=AFQjCNGv_QKRcNSaYPR7j5rYxWxvP7j3Nw&sig2=gvKmaviDIOuyH1_Di0bpWA
// identifier1' has C-linkage specified, but returns UDT 'identifier2' which is incompatible with C
// This warning gets caused by the PT-query-definitions-DLL
// which returns CString instead of char* (inconvenient memory-management and string building).
// This is OK, as long as we call the DL from C / C++.
#pragma warning(error:4190)
//
// https://msdn.microsoft.com/en-us/library/b6801kcy%28v=vs.100%29.aspx
// forcing value to bool 'true' or 'false' (performance warning)
#pragma warning(error:4800) 
//
// https://msdn.microsoft.com/en-us/library/aa231853%28v=vs.60%29.aspx
// macro redefinition
#pragma warning(error:4005)   
// https://msdn.microsoft.com/en-us/library/aa231864%28v=vs.60%29.aspx
// signed/unsigned mismatch
#pragma warning(error:4018) 
// https://msdn.microsoft.com/en-us/library/aa733780%28v=vs.60%29.aspx
// 'extended-attribute' : ignored on left of 'type' when no variable is declared
#pragma warning(error:4091)   
// https://msdn.microsoft.com/en-us/library/aa733790%28v=vs.60%29.aspx
// unreferenced local variable
#pragma warning(error:4101)
// https://msdn.microsoft.com/en-us/library/aa733807%28v=vs.60%29.aspx
// pragma not supported during fast compile
#pragma warning(error:4118)   
// https://msdn.microsoft.com/en-us/library/ew69e79d.aspx
// http://www.maxinmontreal.com/forums/viewtopic.php?f=110&t=19656
// unrecognized character escape sequence
#pragma warning(error:4129)   
// http://msdn.microsoft.com/en-US/library/23k5d385%28v=VS.80%29.aspx
// local declaration of <variable> hides previous declaration at <line> of <file>
#pragma warning(error:4229)		
// https://blogs.msdn.microsoft.com/vcblog/2015/06/22/format-specifiers-checking/
// Format specifiers checking (various errors)
#pragma warning(error:4477)		
// https://msdn.microsoft.com/en-us/library/k64a6he5.aspx
// expression has no effect; expected expression with side-effect
#pragma warning(error:4555)
// https://msdn.microsoft.com/en-us/library/axhfhh6x.aspx
// uninitialized local variable - Stack Overflow
#pragma warning(error:4700)
// https://msdn.microsoft.com/pl-pl/library/c26da40e%28v=vs.71%29.aspx
// unreachable code
//#pragma warning(error:4702)
// https://msdn.microsoft.com/en-us/library/aa734012%28v=vs.60%29.aspx
// not all control paths return a value
#pragma warning(error:4715)   
// https://msdn.microsoft.com/en-us/library/aa266195%28v=vs.60%29.aspx
// unsafe mix of type X and type 'bool' in operation
#pragma warning(error:4805)		
// http://msdn.microsoft.com/en-us/library/2c8f766e(v=vs.80).aspx
// Local declaration of <variable> hides declaration of same name in outer scope.
#pragma warning(error:6246)	

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif

// *************************************************************========================================

#include <afxtempl.h>
#include <afxpriv.h>
#include <afxdlgs.h>

#include <afxadv.h>			// MFC support for MRU

#include <afxmt.h>			// CMutex

#include <windows.h>
#include <stdlib.h>

#define OPENHOLDEM_PROGRAM

// Version
#define VERSION_NUMBER			14.10
#define VERSION_TEXT			"14.0.2.0"  // change version number in OpenHoldem.rc also, when needed

// PokerEval
#include "PokerEval/include/poker_defs.h"
#include "PokerEval/include/pokereval_export.h"
#include "PokerEval/include/evx_defs.h"

// Important project headers
#include "CValidator.h"
#include "FloatingPoint_Comparisions.h"
#include "NumericalFunctions.h"
#include "Debugger/debugger.h"
#include "Files/files.h"
#include "Globals/globals.h"
#include "Preferences/Preferences.h"
#include "StringFunctions/string_functions.h"
#include "CCritSec/CCritSec.h"
#include "MagicNumbers/MagicNumbers.h"
#include "StructsDefines/structs_defines.h"

// To avoid some race-conditions
#define WAIT_FOR_CONDITION(condition) { while (!(condition)) { Sleep(250); } }

#endif //INC_STDAFX_H