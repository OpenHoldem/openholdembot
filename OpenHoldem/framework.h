#pragma once

#ifndef _SECURE_ATL
#define _SECURE_ATL 1
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // Exclude rarely-used stuff from Windows headers
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // some CString constructors will be explicit

// turns off MFC's hiding of some common and often safely ignored warning messages
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions


#include <afxdisp.h>        // MFC Automation classes



#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC support for Internet Explorer 4 Common Controls
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>             // MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxcontrolbars.h>     // MFC support for ribbons and control barsMRU

#include <afxmt.h>			// CMutex

#include <Unknwn.h>
#include <inspectable.h>

// WinRT
#include <winrt/base.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Graphics.Capture.h>
#include <winrt/Windows.Graphics.DirectX.h>
#include <winrt/Windows.Graphics.DirectX.Direct3D11.h>
#include <windows.graphics.directx.direct3d11.interop.h>
#include <windows.graphics.capture.interop.h>
#include <windows.graphics.capture.h>
//#include <DispatcherQueue.h>

// STL
#include <atomic>
#include <memory>

// D3D
#include <d3d11.h>
#include <dxgi1_2.h>
//#include <wincodec.h>

// DWM
#include <dwmapi.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "Dwmapi.lib")
#pragma comment(lib, "windowsapp.lib") // Required for WinRT

//using namespace winrt;
using namespace winrt::Windows::Graphics::Capture;
//using namespace Windows::Graphics::DirectX;
//using namespace Windows::Graphics::DirectX::Direct3D11;

#define OPENHOLDEM_PROGRAM

// Version
#define VERSION_NUMBER			14.10
#define VERSION_TEXT				"14.0.2.0"  // change version number in OpenHoldem.rc also, when needed

// PokerEval
#include "poker_defs.h"
#include "pokereval_export.h"
#include "evx_defs.h"

// Assertions
#include <assert.h>

// Important project headers
#include "CValidator.h"
#include "FloatingPoint_Comparisions.h"
#include "NumericalFunctions.h"
#include "..\DLLs\Debug_DLL\debug.h"
#include "..\DLLs\Files_DLL\files.h"
#include "..\DLLs\Globals_DLL\globals.h"
#include "..\DLLs\Preferences_DLL\Preferences.h"
#include "..\DLLs\StringFunctions_DLL\string_functions.h"
#include "..\Shared\CCritSec\CCritSec.h"
#include "..\Shared\MagicNumbers\MagicNumbers.h"
#include "..\StructsDefines\structs_defines.h"

// To avoid some race-conditions
#define WAIT_FOR_CONDITION(condition) { while (!(condition)) { Sleep(250); } }









#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif


