//******************************************************************************
//
// This file is part of the OpenHoldem project
//    Source code:           https://github.com/OpenHoldem/openholdembot/
//    Forums:                http://www.maxinmontreal.com/forums/index.php
//    Licensed under GPL v3: http://www.gnu.org/licenses/gpl.html
//
//******************************************************************************
//
// Purpose: To be used with CMap-hashtables and CStrings as keys
//
//******************************************************************************

#include "pch.h"
#include "CStringHashtableTemplate.h"

// CString hashtable below by Sam NG
// http://www.codeproject.com/Articles/13458/CMap-How-to
template<> 
UINT AFXAPI HashKey<CString*> (CString* key)
{
    return (NULL == key) ? 0 : HashKey((LPCTSTR)(*key));
}

// I don't know why, but CompareElements can't work with CString*
// have to define this
typedef CString* LPCString;

template<>
BOOL AFXAPI CompareElements<LPCString, LPCString> 
     (const LPCString* pElement1, const LPCString* pElement2)
{
    if ( *pElement1 == *pElement2 ) {
        // true even if pE1==pE2==NULL
        return true;
    } else if ( NULL != *pElement1 && NULL != *pElement2 ) {
        // both are not NULL
        return **pElement1 == **pElement2;
    } else {
        // either one is NULL
        return false;
    }
}

// End of CString hashtable by Sam NG