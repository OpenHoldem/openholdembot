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
#include "CSessionCounter.h"

#include "WindowFunctions/window_functions.h"

// Global instantiation of CSessionCounter class
CSessionCounter *p_sessioncounter = NULL;

CSessionCounter::CSessionCounter()
{	
	_session_id = -1;

	// Try to get a _session_id that is not in use.
	for (int i=0; i<MAX_SESSION_IDS; i++)
	{
		// Name the mutexes A..Y
		CString mutex_name = CString(Preferences()->mutex_name()) + "_" + CString(char('A' + i));
		hMutex = CreateMutex(0, FALSE, mutex_name);
		if (GetLastError() != ERROR_ALREADY_EXISTS)
		{
			_session_id = i;
			return;
		}
		else
		{
			CloseHandle(hMutex);
		}
	}

	// Otherwise: We failed. No ID available.
	MessageBox_Error_Warning("Could not grab a session ID.\n"
		"Too many instances of OpenHoldem\n",
		"SessionCounter Error");
	PostQuitMessage(-1);
}

CSessionCounter::~CSessionCounter()
{
	// Release the mutex for our _session_id
	CloseHandle(hMutex);
}

