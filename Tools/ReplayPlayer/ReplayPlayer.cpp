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


// OHReplay.cpp : Defines the entry point for the application.
//
#include "stdafx.h"
#include "ReplayPlayer.h"

#include <atlstr.h>
#include <stdio.h>
#include "Registry.h"

//
//   FUNCTION: Main
//
int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_OHREPLAY, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_OHREPLAY));

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_OHREPLAY));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
bool InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	Registry	reg;
	int			max_x, max_y;
	HWND		hWnd;
	HMENU		sys_menu;

	hInst = hInstance; // Store instance handle in our global variable

	hWnd = CreateWindow(szWindowClass, szTitle, WS_BORDER | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
						CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

	g_hWnd = hWnd; // store hwnd in our global variable

	if (!hWnd)
		return FALSE;

	// Add items to system menu
	sys_menu = GetSystemMenu(g_hWnd, FALSE);
	AppendMenu(sys_menu, MF_SEPARATOR, 0, "");
	AppendMenu(sys_menu, MF_STRING, sys_menu_item_open, "&Open...\tCtrl-O");

	// Restore window location and size
	reg.read_reg();
	max_x = GetSystemMetrics(SM_CXSCREEN) - GetSystemMetrics(SM_CXICON);
	max_y = GetSystemMetrics(SM_CYSCREEN) - GetSystemMetrics(SM_CYICON);

	SetWindowPos(hWnd, HWND_TOP, min(reg.main_x, max_x), min(reg.main_y, max_y), 320, 240, SWP_SHOWWINDOW);
	UpdateWindow(hWnd);

	cur_frame = -1;
	cur_working_path[0] = '\0';

	return true;
}

bool DestroyWindow()
{
	Registry		reg;
	WINDOWPLACEMENT wp;

	// Save window position
	reg.read_reg();
	GetWindowPlacement(g_hWnd, &wp);

	reg.main_x = wp.rcNormalPosition.left;
	reg.main_y = wp.rcNormalPosition.top;
	reg.write_reg();

	return true;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int				wmId, wmEvent;
	PAINTSTRUCT		ps;
	HDC				hdc;

	switch (message)
	{
		case WM_COMMAND:
			wmId    = LOWORD(wParam);
			wmEvent = HIWORD(wParam);
			
			// Parse the menu selections:
			switch (wmId)
			{
				case IDM_ABOUT:
					DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
					break;

				case IDM_EXIT:
					DestroyWindow(hWnd);
					break;

				case OPENFRAME1:
				case OPENFRAME2:
					open_frame();
					break;

				case PREVFRAME:
					prev_frame();
					break;

				case NEXTFRAME:
					next_frame();
					break;

				case RESET:
					cur_frame = -1;
					cur_working_path[0] = '\0';
					InvalidateRect(g_hWnd, NULL, true);
					break;

				default:
					return DefWindowProc(hWnd, message, wParam, lParam);
			}
			break;

		case WM_PAINT:
			hdc = BeginPaint(hWnd, &ps);
			
			draw_cur_frame();

			EndPaint(hWnd, &ps);
			break;

		case WM_DESTROY:
			DestroyWindow();
			PostQuitMessage(0);
			break;

		case WM_SYSCOMMAND:
			if (wParam == sys_menu_item_open)
			{
				open_frame();
				return false;
			}
			return DefWindowProc(hWnd, message, wParam, lParam);
			break;

		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}

//
// Message handler for about box.
//
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
		case WM_INITDIALOG:
			return (INT_PTR)TRUE;

		case WM_COMMAND:
			if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
			{
				EndDialog(hDlg, LOWORD(wParam));
				return (INT_PTR)TRUE;
			}
			break;
	}

	return (INT_PTR) false;
}

//
// Draw the frame identified by cur_frame_fullpath
//
void draw_cur_frame()
{
	HDC				hdc, hdcScreen, hdcCompatible; 
	HBITMAP			hBmpFile, hBmpWindow, old_bitmap1, old_bitmap2;
	BITMAP			bmp;
	FILE			*fp;
	char			c, title[512], text[2048], szHtmFile[MAX_PATH], szBmpFile[MAX_PATH];
	int				i, width, height;
	RECT			text_rect;

	// Draw white background and instructions
	if (cur_frame == -1 || cur_working_path[0] == '\0')
	{
		SetWindowPos(g_hWnd, NULL, 0, 0, 320, 240, SWP_NOMOVE | SWP_NOZORDER);
		hdc = GetDC(g_hWnd);
		SetTextColor(hdc, RGB(220,0,0));
		sprintf_s(text, 2048, "OHReplay 12.0.3\n");
		strcat_s(text, 2048, "----------------------------------\n");
		strcat_s(text, 2048, "Open a frame: SysMenu/Open, Ctrl-O, F2\n\n");
		strcat_s(text, 2048, "Next frame: Tab\n");
		strcat_s(text, 2048, "Previous frame: Shift-Tab\n");
		strcat_s(text, 2048, "Reset: Esc\n");
		memset(&text_rect, 0, sizeof(RECT));
		DrawText(hdc, text, (int) strlen(text), &text_rect, DT_CALCRECT);
		DrawText(hdc, text, (int) strlen(text), &text_rect, NULL);

		ReleaseDC(g_hWnd, hdc);

		SetWindowText(g_hWnd, "OHReplay");
	}

	// Draw current frame
	else
	{
		// Filenames
		sprintf_s(szHtmFile, MAX_PATH, "%sframe%06d.htm", cur_working_path, cur_frame);
		sprintf_s(szBmpFile, MAX_PATH, "%sframe%06d.bmp", cur_working_path, cur_frame);

		// Open bitmap file
		hBmpFile = (HBITMAP) LoadImage(0, szBmpFile, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

		if (!hBmpFile)
			return;

		// Get bitmap size information
		if (!GetObject(hBmpFile, sizeof(bmp), &bmp))
			return;

		width = bmp.bmWidth;
		height = bmp.bmHeight;

		// Resize window
		SetWindowPos(g_hWnd, NULL, 0, 0, 
					 width + GetSystemMetrics(SM_CXDLGFRAME)*2, 
					 height + GetSystemMetrics(SM_CYDLGFRAME)*2 + GetSystemMetrics(SM_CYSIZE) + 1, 
					 SWP_NOMOVE | SWP_NOZORDER);

		// Select file bitmap into a DC
		hdc = GetDC(g_hWnd);
		hdcScreen = CreateDC("DISPLAY", NULL, NULL, NULL); 
		hdcCompatible = CreateCompatibleDC(hdcScreen); 
		old_bitmap1 = (HBITMAP) SelectObject(hdcCompatible, hBmpFile);

		// Create bitmap for window and select it to window's DC
		hBmpWindow = CreateCompatibleBitmap(hdcScreen, width, height);
		old_bitmap2 = (HBITMAP) SelectObject(hdc, hBmpWindow);

		// Copy bitmap to window
		BitBlt(hdc, 0, 0, width, height, hdcCompatible, 0, 0, SRCCOPY);
		SelectObject(hdc, old_bitmap2);
		SelectObject(hdcCompatible, old_bitmap1);

		// Clean up
		DeleteObject(hBmpWindow);
		DeleteObject(hBmpFile);
		DeleteDC(hdcCompatible);
		DeleteDC(hdcScreen);
		ReleaseDC(g_hWnd, hdc);

		// Set title text
		i=0;
		if (fopen_s(&fp, szHtmFile, "r")==0)
		{
			c = fgetc(fp);
			while (c!=EOF && c!='\r' && c!='\n')
			{
				title[i++]=c;
				c = fgetc(fp);
			}
			fclose(fp);
		}

		title[i] = '\0';
		
		sprintf_s(text, 2048, "%s [%06d]", title, cur_frame);
		SetWindowText(g_hWnd, text);
	}
}

//
// Open a frame
//
void open_frame()
{
	char			szFile[MAX_PATH], szFileTitle[MAX_PATH], szName[MAX_PATH], framenum[MAX_PATH];
	int				name_start, name_end;

	// Select frame to open
	if (select_frame(szFile, szFileTitle, &name_start, &name_end))
	{
		// Extract name and path
		memcpy(szName, szFile+name_start, name_end-name_start-1); 
		szName[name_end-name_start-1] = '\0';
		
		memcpy(cur_working_path, szFile, name_start);  // save in global var for future use
		cur_working_path[name_start] = '\0';

		// Verifying correct format of name, e.g. "frame000000.htm".
		// Renamed frames cause a crash
		// http://www.maxinmontreal.com/forums/viewtopic.php?f=156&t=12960
		// ".htm" got already truncated at the end...
		CString name_to_verify = szName;
		if ((name_to_verify.GetLength() != 11)
			|| (name_to_verify.Left(5) != "frame"))
		{
			MessageBox(0, "Incorrect filename.\n"
				"Replay-frames have to be named like \"frame012345.htm\".\n"
				"\n"
				"Ignoring this frame.\n",
				"Error", 0);
			return;
		}
		// Extract frame number
		// 11 bytzes to extract after "frame": 
		// 6 digits, 4 for ".htm" and 1 for NULL.
		const int number_of_bytes_after_frame_prefix = 11; 
		sprintf_s(framenum, number_of_bytes_after_frame_prefix, 
			"%s", szName+5);
		// Truncating ".htm".
		framenum[6] = '\0';

		cur_frame = atoi(framenum);

		// draw the frame
		draw_cur_frame();
	}
}

//
// CFileDialog to select frame to display 
//
bool select_frame(char *szFile, char *szFileTitle, int *name_start, int *name_end)
{
	OPENFILENAME	ofn;

	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = g_hWnd;
	ofn.lpstrFile = szFile;
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrFileTitle = szFileTitle;
	ofn.lpstrFileTitle[0] = '\0';
	ofn.nMaxFileTitle = MAX_PATH;
  // Supporting pure bitmaps now because too much beginners don't send complete frames.
  // This way we can use at least the image info, but not the table-title in the HTML
	ofn.lpstrFilter = "OpenHoldem Frames (.htm)\0*.htm\0Bitmaps (.bmp)\0*.bmp\0\0";
	ofn.nFilterIndex = 1;
	ofn.lpstrTitle = "Select OpenHoldem frame to OPEN";
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	if (GetOpenFileName(&ofn))
	{
		*name_start = ofn.nFileOffset;
		*name_end = ofn.nFileExtension;
		return true;
	}

	return false;
}

//
// Previous frame
//
void prev_frame()
{
	char			framenum[MAX_PATH], szHtmFile[MAX_PATH];
	WIN32_FIND_DATA	FindFileData;
	HANDLE			hFind;
	int				prev_frame=-1, max_frame=-1;

	if (cur_working_path[0] == '\0')
		return;

	// Filename
	sprintf_s(szHtmFile, MAX_PATH, "%sframe??????.htm", cur_working_path);

	// Find first file that matches wildcard
	hFind = FindFirstFile(szHtmFile, &FindFileData);

	if (hFind == INVALID_HANDLE_VALUE)
	{
		return;
	}

	else
	{
		// Scan through all matches, find highest numbered one that is less than cur_frame
		sprintf_s(framenum, MAX_PATH, "%s", FindFileData.cFileName+5);
		framenum[6] = '\0';

		if (atoi(framenum) > prev_frame && atoi(framenum)<cur_frame)
			prev_frame = atoi(framenum);

		if (atoi(framenum) > max_frame)
			max_frame = atoi(framenum);

		while (FindNextFile(hFind, &FindFileData) != 0) 
		{
			sprintf_s(framenum, MAX_PATH, "%s", FindFileData.cFileName+5);
			framenum[6] = '\0';

			if (atoi(framenum) > prev_frame && atoi(framenum)<cur_frame)
				prev_frame = atoi(framenum);

			if (atoi(framenum) > max_frame)
				max_frame = atoi(framenum);
		}
	}

	// Save new frame in a global
	if (prev_frame != -1)
		cur_frame = prev_frame;
	else
		cur_frame = max_frame;

	// Draw new frame
	draw_cur_frame();
}

//
// Next frame
//
void next_frame()
{
	char			framenum[MAX_PATH], szHtmFile[MAX_PATH];
	WIN32_FIND_DATA	FindFileData;
	HANDLE			hFind;
	int				next_frame=999999, min_frame=999999;

	if (cur_working_path[0] == '\0')
		return;

	// Filename
	sprintf_s(szHtmFile, MAX_PATH, "%s%s", cur_working_path, "frame??????.htm");

	// Find first file that matches wildcard
	hFind = FindFirstFile(szHtmFile, &FindFileData);

	if (hFind == INVALID_HANDLE_VALUE)
	{
		return;
	}

	else
	{
		// Scan through all matches, find lowest numbered one that is greater than cur_frame
		sprintf_s(framenum, MAX_PATH, "%s", FindFileData.cFileName+5);
		framenum[6] = '\0';

		if (atoi(framenum) < next_frame && atoi(framenum)>cur_frame)
			next_frame = atoi(framenum);

		if (atoi(framenum) < min_frame)
			min_frame = atoi(framenum);

		while (FindNextFile(hFind, &FindFileData) != 0) 
		{
			sprintf_s(framenum, MAX_PATH, "%s", FindFileData.cFileName+5);
			framenum[6] = '\0';

			if (atoi(framenum) < next_frame && atoi(framenum)>cur_frame)
				next_frame = atoi(framenum);

			if (atoi(framenum) < min_frame)
				min_frame = atoi(framenum);
		}
	}

	// Save new frame in a global
	if (next_frame != 999999)
		cur_frame = next_frame;
	else
		cur_frame = min_frame;

	// Draw new frame
	draw_cur_frame();
}
