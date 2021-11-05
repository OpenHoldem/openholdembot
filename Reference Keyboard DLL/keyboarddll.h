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

#ifndef INC_KEYBOARDDLL_H
#define INC_KEYBOARDDLL_H

#ifdef KEYBOARDDLL_EXPORTS
#define KEYBOARDDLL_API __declspec(dllexport)
#else
#define KEYBOARDDLL_API __declspec(dllimport)
#endif

typedef int (*keyboard_sendstring_t)(const HWND hwnd, const RECT rect, const CString s, const bool use_comma);
KEYBOARDDLL_API int SendString(const HWND hwnd, const RECT rect, const CString s, const bool use_comma);

typedef int (*keyboard_sendkey_t)(const HWND hwnd, const RECT rect, UINT vkey);
KEYBOARDDLL_API int SendKey(const HWND hwnd, const RECT rect, UINT vkey);

typedef void (*keyboard_process_message_t)(const char *message, const void *param);
KEYBOARDDLL_API void ProcessMessage(const char *message, const void *param);

const POINT RandomizeClickLocation(const RECT rect);
const void GetClickPoint(const int x, const int y, const int rx, const int ry, POINT *p);
const double RandomNormalScaled(const double scale, const double m, const double s);
const double RandomNormal(const double m, const double s);

#endif //INC_KEYBOARDDLL_H