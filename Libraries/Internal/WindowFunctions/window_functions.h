//******************************************************************************
//
// This file is part of the OpenHoldem project
//    Source code:           https://github.com/OpenHoldem/openholdembot/
//    Forums:                http://www.maxinmontreal.com/forums/index.php
//    Licensed under GPL v3: http://www.gnu.org/licenses/gpl.html
//
//******************************************************************************
//
// Purpose: Window function library
//   a) quite independent of the rest
//   b) might be used by ManualMode too 
//      (ResizeToClientsize is needed for support 
//      of titlebars of different sizes (Windows10))
//
//******************************************************************************

#pragma once
#ifndef WINVER				// Allow use of features specific to Windows XP or later.
#define WINVER 0x0501
#endif

#include <afxwin.h>

int CalculateTotalHeightForClientHeight(HWND window, int desired_client_height);
int CalculateTotalWidthForClientWidth(HWND window, int desired_client_width);
void CascadeSingleWindow(HWND window, int cascade_position);
void GetWindowSize(HWND window, int *width, int* height);
void MinimizeWindow(HWND window);
void MoveWindow(HWND window, int x, int y);
void MoveWindowToTopLeft(HWND window);
void ResizeToClientSize(HWND window, int new_width, int new_height);
void ResizeToTotalSize(HWND window, int new_width, int new_height);
void TileSingleWindow(HWND this_window, HWND *null_terminated_list_of_other_windows);
bool WinBelongsToExecutable(HWND window, const char* program_name);
void WinGetTitle(HWND window, char *title);
bool WinIsDesktop(HWND window);
bool WinIsMaximized(HWND window);
bool WinIsMinimized(HWND window);
bool WinIsBring(HWND window);
bool WinIsOutOfScreen(HWND window);
bool WinIsTaskbar(HWND window);
bool WinIsProgramManager(HWND window);
bool WinIsTaskManager(HWND window);
bool WinIsZeroSized(HWND window);

// Errors and warnings.
// Get displayed (or not) depending on settings
void MessageBox_Error_Warning(const char* Message, const char*  Title = "Error");
// Interactive messages.
// Get always displayed, even if they are not critical.
// They are either a result of the users action 
// or we need request some direct input.
int MessageBox_Interactive(const char*  Message, const char*  Title, int Flags);
// MessageBox for the msgbox$MESSAGE-command of OH-script
void MessageBox_OH_Script_Messages(const char*  message);
