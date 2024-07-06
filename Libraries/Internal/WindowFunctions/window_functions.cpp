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

#include "window_functions.h"
#include "Debugger/debugger.h"
#include "MagicNumbers/MagicNumbers.h"
#include "Psapi.h"

RECT desktop_dimensions;
bool desktop_dimensions_calculated = false;

// Replacement for MicroSofts CascadeWindows 
// http://msdn.microsoft.com/en-us/library/windows/desktop/ms632674(v=vs.85).aspx
// which can't be used as it resizes back to default
void CascadeSingleWindow(HWND window, int cascade_position) {
  const int kCascadedDeltaX = 30;
  const int kCascadedDeltaY = 30;
  // We don't use the position (0, 0) because it is reserved for the lobby
  int x = (cascade_position + 1) * kCascadedDeltaX;
  int y = (cascade_position + 1) * kCascadedDeltaY;
  MoveWindow(window, x, y);
}

void GetWindowSize(HWND window, int *width, int* height) {
  if (window == NULL) {
    return;
  }
  if (!IsWindow(window)) {
    return;
  }
  RECT window_rect;
  GetWindowRect(window, &window_rect);
  *width = window_rect.right - window_rect.left;
  *height = window_rect.bottom - window_rect.top;
}

void MinimizeWindow(HWND window) {
  // http://msdn.microsoft.com/en-us/library/windows/desktop/ms633548%28v=vs.85%29.aspx
  ShowWindow(window, SW_MINIMIZE);
}

void MoveWindow(HWND window, int x, int y) {
  RECT desktop_rectangle;
  SystemParametersInfo(SPI_GETWORKAREA, NULL, &desktop_rectangle, NULL);
  // Consistancy checks
  if (x < 0
    || x >= desktop_rectangle.right
    || y < 0
    || y >= desktop_rectangle.bottom) {
    // Values out of sane range, do nothing
    return;
  }
  RECT current_position;
  GetWindowRect(window, &current_position);
  int width = current_position.right - current_position.left;
  int height = current_position.bottom - current_position.top;
  const bool kRedrawWindow = true;
  // Equally named sdystem call that needs additional (superfluous) 
  // resizing-parameters.
  MoveWindow(window, x, y, width, height, kRedrawWindow);
}

void MoveWindowToTopLeft(HWND window) {
  MoveWindow(window, 0, 0);
}

// !!! to do: move tzo numerical functions (DLL)
bool isInRange(int value, int first, int last) {
  if (value < first) {
    return false;
  }
  if (value > last) {
    return false;
  }
  return true;
}

// Replacement for MicroSofts TileWindows
// http://msdn.microsoft.com/en-us/library/windows/desktop/ms633554(v=vs.85).aspx
// Unfortunatelly this fucntion had 2 disadvantages:
//   * it allows small overlaps
//   * it resizes all full-screen-windows back to default
// TileSingleWindow has to be called for all windows,
// starting with one window plus an empty list of other windows
// then next window plus list of already positioned windows
//
// Keep other windows at current place.
// Find a free space for this one.
// Move it to top left, if not possible.
void TileSingleWindow(HWND this_window, HWND *null_terminated_list_of_other_windows) {
  RECT desktop_rectangle;
  SystemParametersInfo(SPI_GETWORKAREA, NULL, &desktop_rectangle, NULL);
  int table_width, table_height;
  GetWindowSize(this_window, &table_width, &table_height);
  // We don#t have to search the whole desktop,
  // as the window has to fit on the screen,
  // right and below of the top-left coordinates.
  RECT search_area;
  search_area.left = desktop_rectangle.left;
  search_area.top = desktop_rectangle.top;
  search_area.right = desktop_rectangle.right - table_width;
  search_area.bottom = desktop_rectangle.bottom - table_height;
  // First search a free slot, the dumb and reliable way
  const int kDeltaX = 8;
  for (int x = search_area.left; x < search_area.right; x += kDeltaX) {
    for (int y = search_area.top; y < search_area.bottom; ++y) {
      // Look for a conflict with every other window
      int right_x = x + table_width - 1;
      int bottom_y = y + table_height - 1;
      assert(null_terminated_list_of_other_windows != NULL);
      // Create a copy of the pointer, we need the original later again
      HWND* list_of_windows = null_terminated_list_of_other_windows;
      while (*list_of_windows != NULL) {
        if (*list_of_windows == this_window) {
          // Window to be positioned might be in list of "other" windows
          // which we get from auto-connector. Ignore it
          ++list_of_windows;
          continue;
        }
        RECT other_window_position;
        GetWindowRect(*list_of_windows, &other_window_position);
        bool x_range_overlaps = false;
        bool y_range_overlaps = false;
        if (isInRange(x, other_window_position.left, other_window_position.right)) {
          x_range_overlaps = true;
        }
        else if (isInRange(right_x, other_window_position.left, other_window_position.right)) {
          x_range_overlaps = true;
        }
        if (isInRange(y, other_window_position.top, other_window_position.bottom)) {
          y_range_overlaps = true;
        }
        else if (isInRange(bottom_y, other_window_position.top, other_window_position.bottom)) {
          y_range_overlaps = true;
        }
        if (x_range_overlaps && y_range_overlaps) {
          goto conflict_with_any_window_continue_with_next_x_or_y;
        }
        // No conflict with this window
        // Try next one
        ++list_of_windows;
      }
      // End of list reached
      // No overlap found
      // Heureka!
      MoveWindow(this_window, x, y);
      return;
      // GOTO-label to jump out of inner loop on conflict
      conflict_with_any_window_continue_with_next_x_or_y:
      // Skip some hundred y-coordinates that are guaranteed to fail
      // Scroll down to bottom of last overlapping window
      int next_y = bottom_y + 1;
      assert(next_y > y);
      y = next_y;
    }
  }
  // Failed
  // Move to top-left (lobby-position) to reduce overlaps and damage.
  MoveWindowToTopLeft(this_window);
}

bool WinCalculateDesktopDimensions() {
  if (!GetWindowRect(GetDesktopWindow(), &desktop_dimensions)) {
    return false;
  }
  desktop_dimensions_calculated = true;
  return true;
}

bool WinIsBring(HWND window) {
  return WinBelongsToExecutable(window, "bring.exe");
}

bool WinIsDesktop(HWND window) {
  // http://msdn.microsoft.com/en-us/library/windows/desktop/ms633504%28v=vs.85%29.aspx
  return (GetDesktopWindow() == window || GetShellWindow() == window);
}

bool WinIsMaximized(HWND window) {
  return ::IsZoomed(window) != 0;
}

bool WinIsMinimized(HWND window) {
  return ::IsIconic(window) != 0;
}

bool WinIsOutOfScreen(HWND window) {
  if (!desktop_dimensions_calculated) {
    WinCalculateDesktopDimensions();
  }
  RECT rect;
  if (!GetWindowRect(window, &rect)) {
    return false;
  }
  return((rect.right < 0)
    || (rect.bottom < 0)
    || (rect.top > desktop_dimensions.bottom)
    || (rect.left > desktop_dimensions.right));
}

bool WinIsTaskbar(HWND window) {
  // Good way to find and detect the taskbar.
  // This should work with Windows XP and Windows 7
  HWND hShellWnd = ::FindWindow("Shell_TrayWnd", NULL);
  return window == hShellWnd;
}

bool WinIsProgramManager(HWND window) {
  return WinBelongsToExecutable(window, "progman.exe");
}

bool WinIsTaskManager(HWND window) {
  return WinBelongsToExecutable(window, "taskmgr.exe");
}

bool WinBelongsToExecutable(HWND window, const char* program_name) {
  if (window) {
    DWORD dwProcessId = 0;
    DWORD dwThreadId = GetWindowThreadProcessId(window, &dwProcessId);
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION |
      PROCESS_VM_READ, FALSE, dwProcessId);
    if (hProcess) {
      bool match = false;
      char nameProc[MAX_PATH];
      if (GetProcessImageFileName(hProcess, nameProc, MAX_PATH)) {
        match = strstr(nameProc, program_name) != NULL;
      }
      CloseHandle(hProcess);
      return match;
    }
  }
  return false;
}

bool WinIsZeroSized(HWND window) {
  RECT rect;
  if (!GetWindowRect(window, &rect)) {
    return false;
  }
  return ((rect.right <= rect.left)
    || (rect.bottom <= rect.top));
}

int CalculateTotalWidthForClientWidth(HWND window, int desired_client_width) {
  assert(desired_client_width > 0);
  RECT old_client_size;
  GetClientRect(window, &old_client_size);
  int old_client_width = old_client_size.right - old_client_size.left;
  RECT old_position;
  GetWindowRect(window, &old_position);
  int new_total_width = old_position.right - old_position.left
    + desired_client_width - old_client_width;
  return new_total_width;
}

int CalculateTotalHeightForClientHeight(HWND window, int desired_client_height) {
  assert(desired_client_height > 0);
  RECT old_client_size;
  GetClientRect(window, &old_client_size);
  int old_client_height = old_client_size.bottom - old_client_size.top;
  RECT old_position;
  GetWindowRect(window, &old_position);
  int new_total_height = old_position.bottom - old_position.top
    + desired_client_height - old_client_height;
  return new_total_height;
}

void ResizeToClientSize(HWND window, int new_width, int new_height) {
  assert(new_width > 0);
  assert(new_height > 0);
  assert(window != NULL);
  int new_total_width = CalculateTotalWidthForClientWidth(
    window, new_width);
  int new_total_height = CalculateTotalHeightForClientHeight(
    window, new_height);
  ResizeToTotalSize(window, new_total_width, new_total_height);
}

void ResizeToTotalSize(HWND window, int new_width, int new_height) {
  assert(new_width > 0);
  assert(new_height > 0);
  assert(window != NULL);
  RECT old_position;
  GetWindowRect(window, &old_position);
  if (old_position.left < 0) {
    old_position.left = 0;
  }
  if (old_position.top < 0) {
    old_position.top = 0;
  }
  MoveWindow(window,
    old_position.left, old_position.top,
    new_width, new_height,
    true);	// true = Redraw the table.
            // Update shared mem !!!!
}

void WinGetTitle(HWND window, char *title) {
  GetWindowText(window, title, MAX_WINDOW_TITLE);
}