//******************************************************************************
//
// This file is part of the OpenHoldem project
//    Source code:           https://github.com/OpenHoldem/openholdembot/
//    Forums:                http://www.maxinmontreal.com/forums/index.php
//    Licensed under GPL v3: http://www.gnu.org/licenses/gpl.html
//
//******************************************************************************
//
// Purpose: Functions that get exported by OpenHoldem.
//   They are available to the DLL once the interface has been 
//   properly initialized (a warning will be shown if not)
//
//******************************************************************************
//
// DO NOT CHANGE ANYTHING IN THIS FILE!
//   
// This Header defines an interface
// Functions and data-types must exactly match.
//
// DO NOT DELETE THE  ERROR-HANDLING
// Using non-initialized pointers will crash OpenHoldem.
// If you get an error-message at runtime
// then change the timing of your initializations.
// E.g. use DLLOnLoad to initialize global variables.
//
//******************************************************************************

#include "OpenHoldemFunctions.h"
#include "atlstr.h"

//******************************************************************************
//
// Function signatures and pointers
//
//******************************************************************************

typedef double(*t_GetSymbol)(const char* name_of_single_symbol__not_expression);
typedef void*(*t_GetPrw1326)();
typedef char*(*t_GetHandnumber)();
typedef char*(*t_GetPlayerName)(int chair);
typedef char*(*t_GetTableTitle)();
typedef void(*t_ParseHandList)(const char* name_of_list, const char* list_body);
typedef char*(*t_ScrapeTableMapRegion)(char* p_region, int& p_returned_lengh);
typedef void(*t_SendChatMessage)(char *message);
typedef void(*t_WriteLog)(char* format, va_list args);

t_GetSymbol p_GetSymbol = nullptr;
t_GetPrw1326 p_GetPrw1326 = nullptr;
t_GetHandnumber p_GetHandnumber = nullptr;
t_GetPlayerName p_GetPlayerName = nullptr;
t_GetTableTitle p_GetTableTitle = nullptr;
t_ParseHandList p_ParseHandList = nullptr;
t_ScrapeTableMapRegion p_ScrapeTableMapRegion = nullptr;
t_SendChatMessage p_SendChatMessage = nullptr;
t_WriteLog p_WriteLog = nullptr;

//******************************************************************************
//
// OpenHoldem functions, to be used by the DLL
//
//******************************************************************************

void ErrorPointerNotInitialized(CString function_name);

double __stdcall GetSymbol(const char* name_of_single_symbol__not_expression) {
  if (p_GetSymbol == nullptr) {
    ErrorPointerNotInitialized("GetSymbol");
	  return 0.0;
  }
  return p_GetSymbol(name_of_single_symbol__not_expression);
}

void* __stdcall GetPrw1326() {
  if (p_GetPrw1326 == nullptr) {
    ErrorPointerNotInitialized("GetPrw1326");
    return nullptr;
  }
  return p_GetPrw1326();
}

char* __stdcall GetHandnumber() {
  if (p_GetHandnumber == nullptr) {
    ErrorPointerNotInitialized("GetHandnumber");
    return "";
  }
  return p_GetHandnumber();
}

char* __stdcall GetPlayerName(int chair) {
  if (p_GetPlayerName == nullptr) {
    ErrorPointerNotInitialized("GetPlayerName");
    return "";
  }
  return p_GetPlayerName(chair);
}

char* __stdcall GetTableTitle() {
  if (p_GetTableTitle == nullptr) {
    ErrorPointerNotInitialized("GetTableTitle");
    return "";
  }
  return p_GetTableTitle();
}

void __stdcall ParseHandList(const char* name_of_list, const char* list_body) {
  if (p_ParseHandList == nullptr) {
    ErrorPointerNotInitialized("ParseHandList");
    return;
  }
  p_ParseHandList(name_of_list, list_body);
}

char* __stdcall ScrapeTableMapRegion(char* p_region, int& p_returned_lengh) {
  if (p_ScrapeTableMapRegion == nullptr) {
    ErrorPointerNotInitialized("ScrapeTableMapRegion");
    return "";
  }
  return p_ScrapeTableMapRegion(p_region, p_returned_lengh);
}

void __stdcall SendChatMessage(char *message) {
  if (p_SendChatMessage == nullptr) {
    ErrorPointerNotInitialized("SendChatMessage");
    return;
  }
  p_SendChatMessage(message);
}

void WriteLog(char* format, ...) {
  if (p_WriteLog == nullptr) {
    ErrorPointerNotInitialized("WriteLog");
    return;
  }
  // Docu about ellipsis and variadic macro:
  // http://msdn.microsoft.com/en-us/library/ms177415(v=vs.80).aspx
  // http://stackoverflow.com/questions/1327854/how-to-convert-a-variable-argument-function-into-a-macro
  va_list args;
  va_start(args, format);
  p_WriteLog(format, args);
  va_end(args);
}

//******************************************************************************
//
// Initialization of the interface
//
//******************************************************************************

FARPROC WINAPI LookupOpenHoldemFunction(char* function_name) {
  // https://msdn.microsoft.com/en-us/library/windows/desktop/ms683199(v=vs.85).aspx
  HMODULE openholdem_main_module = GetModuleHandle(NULL);
  if (openholdem_main_module == nullptr) {
    return nullptr;
  }
  // https://msdn.microsoft.com/en-us/library/windows/desktop/ms683212(v=vs.85).aspx
  const auto function_address = GetProcAddress(openholdem_main_module, function_name);
  if (function_address == nullptr) {
    CString error_message;
    error_message.Format("Can not find %s in Openholdem.exe.\n",
      function_name);
    MessageBox(0,
      error_message,
      "DLL Error",
      MB_ICONERROR);
  }
  return function_address;
}

void InitializeOpenHoldemFunctionInterface() {
  p_GetSymbol = (t_GetSymbol)LookupOpenHoldemFunction("GetSymbol");
  p_GetPrw1326 = (t_GetPrw1326)LookupOpenHoldemFunction("GetPrw1326");
  p_GetHandnumber = (t_GetHandnumber)LookupOpenHoldemFunction("GetHandnumber");
  p_GetPlayerName = (t_GetPlayerName)LookupOpenHoldemFunction("GetPlayerName");
  p_GetTableTitle = (t_GetTableTitle) LookupOpenHoldemFunction("GetTableTitle");
  p_ParseHandList = (t_ParseHandList)LookupOpenHoldemFunction("ParseHandList");
  p_ScrapeTableMapRegion = (t_ScrapeTableMapRegion)LookupOpenHoldemFunction("ScrapeTableMapRegion");
  p_SendChatMessage = (t_SendChatMessage)LookupOpenHoldemFunction("SendChatMessage");
  p_WriteLog = (t_WriteLog)LookupOpenHoldemFunction("WriteLog");
}

void ErrorPointerNotInitialized(CString function_name) {
  CString error_message;
  error_message.Format("OpenHoldem interface not yet initialized.\n"
    "Can't use function %s.\n",
    function_name);
  MessageBox(0,
    error_message,
    "User library error",
    MB_ICONERROR);
}