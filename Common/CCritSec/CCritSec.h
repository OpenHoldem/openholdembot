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

#ifndef INC_CCRITSEC_H
#define INC_CCRITSEC_H

class CCritSec
{
public:
    CCritSec()
        { ::InitializeCriticalSectionAndSpinCount(&m_rep, 4000); }
    ~CCritSec()
        { ::DeleteCriticalSection(&m_rep); }
    
    void Enter()
        { ::EnterCriticalSection(&m_rep); }
    void Leave()
        { ::LeaveCriticalSection(&m_rep); }
    
private:
    // copy ops are private to prevent copying
    CCritSec(const CCritSec&);
    CCritSec& operator=(const CCritSec&);
    
    CRITICAL_SECTION m_rep;
};

class CSLock
{
public:
    CSLock(CCritSec& a_section)
        : m_section(a_section) { m_section.Enter(); }
    ~CSLock()
        { m_section.Leave(); }
    
private:
    // copy ops are private to prevent copying
    CSLock(const CSLock&);
    CSLock& operator=(const CSLock&);
    
    CCritSec& m_section;
};

#endif //INC_CCRITSEC_H