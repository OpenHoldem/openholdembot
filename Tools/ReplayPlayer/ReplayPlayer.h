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

#include "resource.h"

// Defines
#define MAX_LOADSTRING 100


// Functions
ATOM				MyRegisterClass(HINSTANCE hInstance);
bool				InitInstance(HINSTANCE, int);
bool				DestroyWindow();
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
void				draw_cur_frame();
void				open_frame();
bool				select_frame(char *szFile, char *szFileTitle, int *name_start, int *name_end);
void				prev_frame();
void				next_frame();


// Global Variables
HINSTANCE	hInst;									// current instance
HWND		g_hWnd;									// current hwnd
TCHAR		szTitle[MAX_LOADSTRING];				// The title bar text
TCHAR		szWindowClass[MAX_LOADSTRING];			// the main window class name
const int	sys_menu_item_open = WM_USER + 1;
int			cur_frame;
char		cur_working_path[MAX_PATH];
