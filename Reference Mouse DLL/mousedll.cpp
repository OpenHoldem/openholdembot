//******************************************************************************
//
// This file is part of the OpenHoldem project
//    Source code:           https://github.com/OpenHoldem/openholdembot/
//    Forums:                http://www.maxinmontreal.com/forums/index.php
//    Licensed under GPL v3: http://www.gnu.org/licenses/gpl.html
//
//******************************************************************************
//
// Purpose: mousedll.cpp : Defines the entry point for the DLL application.
//
//******************************************************************************


#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers
#endif

// Modify the following defines if you have to target a platform prior to the ones specified below.
// Refer to MSDN for the latest info on corresponding values for different platforms.
#ifndef WINVER				// Allow use of features specific to Windows XP or later.
#define WINVER 0x0501		// Change this to the appropriate value to target other versions of Windows.
#endif

#ifndef _WIN32_WINNT		// Allow use of features specific to Windows XP or later.                   
#define _WIN32_WINNT 0x0501	// Change this to the appropriate value to target other versions of Windows.
#endif						

#ifndef _WIN32_WINDOWS		// Allow use of features specific to Windows 98 or later.
#define _WIN32_WINDOWS 0x0410 // Change this to the appropriate value to target Windows Me or later.
#endif

#ifndef _WIN32_IE			// Allow use of features specific to IE 6.0 or later.
#define _WIN32_IE 0x0600	// Change this to the appropriate value to target other versions of IE.
#endif

#include <windows.h>
#include <math.h>
#include "mousedll.h"

MOUSEDLL_API int MouseClick(const HWND hwnd, const RECT rect, const MouseButton button, const int clicks)
{
	INPUT			input[100] = {0};

	POINT pt = RandomizeClickLocation(rect);
	double fScreenWidth = ::GetSystemMetrics( SM_CXSCREEN )-1;
	double fScreenHeight = ::GetSystemMetrics( SM_CYSCREEN )-1;

	// Translate click point to screen/mouse coords
	ClientToScreen(hwnd, &pt);
	double fx = pt.x*(65535.0f/fScreenWidth);
	double fy = pt.y*(65535.0f/fScreenHeight);

	// Set up the input structure
	for (int i = 0; i<clicks*2; i+=2)
	{
		ZeroMemory(&input[i],sizeof(INPUT));
		input[i].type = INPUT_MOUSE;
		input[i].mi.dx = (LONG) fx;
		input[i].mi.dy = (LONG) fy;

		switch (button)
		{
		case MouseLeft:
			input[i].mi.dwFlags = MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE | MOUSEEVENTF_LEFTDOWN;
			break;
		case MouseMiddle:
			input[i].mi.dwFlags = MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE | MOUSEEVENTF_MIDDLEDOWN;
			break;
		case MouseRight:
			input[i].mi.dwFlags = MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE | MOUSEEVENTF_RIGHTDOWN;
			break;
		}
		
		ZeroMemory(&input[i+1],sizeof(INPUT));
		input[i+1].type = INPUT_MOUSE;
		input[i+1].mi.dx = (LONG) fx;
		input[i+1].mi.dy = (LONG) fy;

		switch (button)
		{
		case MouseLeft:
			input[i+1].mi.dwFlags = MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE | MOUSEEVENTF_LEFTUP;
			break;
		case MouseMiddle:
			input[i+1].mi.dwFlags = MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE | MOUSEEVENTF_MIDDLEUP;
			break;
		case MouseRight:
			input[i+1].mi.dwFlags = MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE | MOUSEEVENTF_RIGHTUP;
			break;
		}
	}

	// Set focus to target window
	SetFocus(hwnd);
	SetForegroundWindow(hwnd);
	SetActiveWindow(hwnd);

	// Send input
	Sleep(100);
	SendInput(clicks*2, input, sizeof(INPUT));
	Sleep(100);
	return (int) true;
}

MOUSEDLL_API int MouseClickDrag(const HWND hwnd, const RECT rect, bool is_horizontal_drag) {
	INPUT			input[3];
	POINT			pt;
	double		fx, fy;

	double fScreenWidth = ::GetSystemMetrics( SM_CXSCREEN )-1;
	double fScreenHeight = ::GetSystemMetrics( SM_CYSCREEN )-1;

	if (is_horizontal_drag) {
		// Set up the input structure
		// left click, drag to right, un-left click
		pt.x = rect.left;
		pt.y = rect.top + (rect.bottom - rect.top) / 2;
		ClientToScreen(hwnd, &pt);
		fx = pt.x * (65535.0f / fScreenWidth);
		fy = pt.y * (65535.0f / fScreenHeight);

		ZeroMemory(&input[0], sizeof(INPUT));
		input[0].type = INPUT_MOUSE;
		input[0].mi.dx = (LONG)fx;
		input[0].mi.dy = (LONG)fy;
		input[0].mi.dwFlags = MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE | MOUSEEVENTF_LEFTDOWN;

		pt.x = rect.right;
		pt.y = rect.top + (rect.bottom - rect.top) / 2;
		ClientToScreen(hwnd, &pt);
		fx = pt.x * (65535.0f / fScreenWidth);
		fy = pt.y * (65535.0f / fScreenHeight);

		ZeroMemory(&input[1], sizeof(INPUT));
		input[1].type = INPUT_MOUSE;
		input[1].mi.dx = (LONG)fx;
		input[1].mi.dy = (LONG)fy;
		input[1].mi.dwFlags = MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE;
	}
	else {
		// Set up the input structure
		// left click, drag to up, un-left click
		pt.x = rect.left + (rect.left - rect.right) / 2;
		pt.y = rect.bottom;
		ClientToScreen(hwnd, &pt);
		fx = pt.x * (65535.0f / fScreenWidth);
		fy = pt.y * (65535.0f / fScreenHeight);

		ZeroMemory(&input[0], sizeof(INPUT));
		input[0].type = INPUT_MOUSE;
		input[0].mi.dx = (LONG)fx;
		input[0].mi.dy = (LONG)fy;
		input[0].mi.dwFlags = MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE | MOUSEEVENTF_LEFTDOWN;

		pt.x = rect.left + (rect.left - rect.right) / 2;
		pt.y = rect.top;
		ClientToScreen(hwnd, &pt);
		fx = pt.x * (65535.0f / fScreenWidth);
		fy = pt.y * (65535.0f / fScreenHeight);

		ZeroMemory(&input[1], sizeof(INPUT));
		input[1].type = INPUT_MOUSE;
		input[1].mi.dx = (LONG)fx;
		input[1].mi.dy = (LONG)fy;
		input[1].mi.dwFlags = MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE;
	}

	ZeroMemory(&input[2],sizeof(INPUT));
	input[2].type = INPUT_MOUSE;
	input[2].mi.dwFlags = MOUSEEVENTF_LEFTUP;

	// Set focus to target window
	SetFocus(hwnd);
	SetForegroundWindow(hwnd);
	SetActiveWindow(hwnd);
	// Send input
	SendInput(3, input, sizeof(INPUT));
	return (int) true;
}

MOUSEDLL_API void ProcessMessage(const char *message, const void *param)
{
	if (message==NULL)  return;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
    return true;
}

const POINT RandomizeClickLocation(const RECT rect) 
{
	POINT p = {0};

	// uniform random distribution, yuck!
	//p.x = ((double) rand() / (double) RAND_MAX) * (rect.right-rect.left) + rect.left;
	//p.y = ((double) rand() / (double) RAND_MAX) * (rect.bottom-rect.top) + rect.top;

	// normal random distribution - much better!
	GetClickPoint(rect.left + (rect.right-rect.left)/2, 
				  rect.top + (rect.bottom-rect.top)/2, 
				  (rect.right-rect.left)/2, 
				  (rect.bottom-rect.top)/2, 
				  &p);

	return p;
}

const void GetClickPoint(const int x, const int y, const int rx, const int ry, POINT *p) 
{
	p->x = x + (int) (RandomNormalScaled(2*rx, 0, 1) + 0.5) - (rx);
	p->y = y + (int) (RandomNormalScaled(2*ry, 0, 1) + 0.5) - (ry);
}

// random number - 0 -> scale, with normal distribution
// ignore results outside 3.5 stds from the mean
const double RandomNormalScaled(const double scale, const double m, const double s) 
{
	double res = -99;
	while (res < -3.5 || res > 3.5) res = RandomNormal(m, s);
	return (res / 3.5*s + 1) * (scale / 2.0);
}

const double RandomNormal(const double m, const double s) 
{
	/* mean m, standard deviation s */
	double x1 = 0., x2 = 0., w = 0., y1 = 0., y2 = 0.;

	do {
		x1 = 2.0 * ((double) rand()/(double) RAND_MAX) - 1.0;
		x2 = 2.0 * ((double) rand()/(double) RAND_MAX) - 1.0;
		w = x1 * x1 + x2 * x2;
	} while ( w >= 1.0 );

	w = sqrt( (-2.0 * log( w ) ) / w );
	y1 = x1 * w;
	y2 = x2 * w;

	return( m + y1 * s ); 

} 
