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
#include "CMyMutex.h"


CMyMutex::CMyMutex() : _mutex(false, Preferences()->mutex_name()) {
  // We want a long timeout to let OHs instances act in FIFO order.
	// But we can't wait forever because events can happen. Popups, table timeout... 
  // FIFO order is actually not guaranteed, but seems to work by chance in Win XP:
  // http://www.maxinmontreal.com/forums/viewtopic.php?f=114&t=18860
	if (_mutex.Lock(5000)) {
    write_log(Preferences()->debug_autoplayer(), "[CMyMutex] successfully locked\n");
	  _locked = TRUE;
  }	else {
    write_log(Preferences()->debug_autoplayer(), "[CMyMutex] Timeout. Locking failed\n");
	  _locked = FALSE;
  }
}


CMyMutex::~CMyMutex(void) {
	if (_locked == TRUE) {   
    write_log(Preferences()->debug_autoplayer(), "[CMyMutex] Locked: %s\n", Bool2CString(_locked));  
		_mutex.Unlock();
  }
}

bool CMyMutex::IsLocked() {
	return _locked == TRUE;
}
