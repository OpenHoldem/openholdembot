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

#pragma once

#ifdef MOUSE_EXPORTS
#define MOUSE_DLL_API __declspec(dllexport)
#else
#define MOUSE_DLL_API __declspec(dllimport)
#endif

enum MouseButton { MouseLeft, MouseMiddle, MouseRight };

extern "C"
{
	typedef int(*mouse_click_t)(const HWND hwnd, const RECT rect, const MouseButton button, const int clicks);
	MOUSE_DLL_API int MouseClick(const HWND hwnd, const RECT rect, const MouseButton button, const int clicks);

	typedef int(*mouse_clickdrag_t)(const HWND hwnd, const RECT rect);
	MOUSE_DLL_API int MouseClickDrag(const HWND hwnd, const RECT rect);

	typedef void (*mouse_process_message_t)(const char* message, const void* param);
	MOUSE_DLL_API void ProcessMessage(const char* message, const void* param);
}

const POINT RandomizeClickLocation(const RECT rect);
const void GetClickPoint(const int x, const int y, const int rx, const int ry, POINT *p);
const double RandomNormalScaled(const double scale, const double m, const double s);
const double RandomNormal(const double m, const double s);
