#include "stdafx.h"
#include "debug.h"
#include "ManualMode.h"
#include "ManualModeDlg.h"
#include "..\..\dbghelp\dbghelp.h"

FILE *log_fp = NULL;

char * get_time(char * timebuf) 
{
	// returns current system time in WH format
	time_t	ltime;
	char tmptime[30];
	
	time( &ltime );
	ctime_s(tmptime, 26, &ltime);
	tmptime[24]='\0';

	memcpy(timebuf, tmptime+20, 4); //yyyy
	*(timebuf+4) = '-';

	// mm
	if (memcmp(tmptime+4, "Jan", 3)==0)  { *(timebuf+5) = '0'; *(timebuf+6) = '1'; }
	else if (memcmp(tmptime+4, "Feb", 3)==0)  { *(timebuf+5) = '0'; *(timebuf+6) = '2'; }
	else if (memcmp(tmptime+4, "Mar", 3)==0)  { *(timebuf+5) = '0'; *(timebuf+6) = '3'; }
	else if (memcmp(tmptime+4, "Apr", 3)==0)  { *(timebuf+5) = '0'; *(timebuf+6) = '4'; }
	else if (memcmp(tmptime+4, "May", 3)==0)  { *(timebuf+5) = '0'; *(timebuf+6) = '5'; }
	else if (memcmp(tmptime+4, "Jun", 3)==0)  { *(timebuf+5) = '0'; *(timebuf+6) = '6'; }
	else if (memcmp(tmptime+4, "Jul", 3)==0)  { *(timebuf+5) = '0'; *(timebuf+6) = '7'; }
	else if (memcmp(tmptime+4, "Aug", 3)==0)  { *(timebuf+5) = '0'; *(timebuf+6) = '8'; }
	else if (memcmp(tmptime+4, "Sep", 3)==0)  { *(timebuf+5) = '0'; *(timebuf+6) = '9'; }
	else if (memcmp(tmptime+4, "Oct", 3)==0)  { *(timebuf+5) = '1'; *(timebuf+6) = '0'; }
	else if (memcmp(tmptime+4, "Nov", 3)==0)  { *(timebuf+5) = '1'; *(timebuf+6) = '1'; }
	else if (memcmp(tmptime+4, "Dec", 3)==0)  { *(timebuf+5) = '1'; *(timebuf+6) = '2'; }

	*(timebuf+7) = '-';
	memcpy(timebuf+8, tmptime+8, 2); //dd
	*(timebuf+10) = ' ';
	memcpy(timebuf+11, tmptime+11, 8); //HH:mm:ss
	*(timebuf+19) = '\0';

	return timebuf;
}

char * get_now_time(char * timebuf) 
{
	// returns current system time as a UNIX style string
	time_t	ltime;

	time( &ltime );
	ctime_s(timebuf, 26, &ltime);
	timebuf[24]='\0';
	return timebuf;
}

void logfatal (char* fmt, ...) 
{
	char		buff[10000] ;
	va_list		ap;
	char		fatallogpath[MAX_PATH];
	FILE		*fatallog;
	char		nowtime[26];

	sprintf_s(fatallogpath, MAX_PATH, "%s\\fatal error.log", startup_path);
	if (fopen_s(&fatallog, fatallogpath, "a")==0)
	{
		va_start(ap, fmt);
		vsprintf_s(buff, 10000, fmt, ap);
		fprintf(fatallog, "%s> %s", get_now_time(nowtime), buff);
		
		va_end(ap);
		fclose(fatallog);
	}
}

LONG WINAPI MyUnHandledExceptionFilter(EXCEPTION_POINTERS *pExceptionPointers)
{
	char flpath[MAX_PATH];
    char msg[1000];

    /*
	logfatal("########################################################################\n");
	logfatal("FATAL ERROR  (See above for call stack)\n");
	logfatal("########################################################################\n");
	logfatal("Address: %x\n", lpExceptionInfo->ExceptionRecord->ExceptionAddress);
	logfatal("Code: %x\n", lpExceptionInfo->ExceptionRecord->ExceptionCode);
	logfatal("Flags: %x\n", lpExceptionInfo->ExceptionRecord->ExceptionFlags);
	logfatal("Information: %x\n", lpExceptionInfo->ExceptionRecord->ExceptionInformation);
	logfatal("Record: %x\n", lpExceptionInfo->ExceptionRecord->ExceptionRecord);
	logfatal("Num Params: %x\n", lpExceptionInfo->ExceptionRecord->NumberParameters);
	switch (lpExceptionInfo->ExceptionRecord->ExceptionCode) {
	case EXCEPTION_ACCESS_VIOLATION:
		logfatal("Desc: EXCEPTION_ACCESS_VIOLATION The thread tried to read from or write to a virtual address\n");
		logfatal("      for which it does not have the appropriate access.\n");
		break;
	case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
		logfatal("Desc: EXCEPTION_ARRAY_BOUNDS_EXCEEDED The thread tried to access an array element that is out\n");
		logfatal("      of bounds and the underlying hardware supports bounds checking.\n");
		break;
	case EXCEPTION_BREAKPOINT:
		logfatal("Desc: EXCEPTION_BREAKPOINT A breakpoint was encountered.\n");
		break;
	case EXCEPTION_DATATYPE_MISALIGNMENT:
		logfatal("Desc: EXCEPTION_DATATYPE_MISALIGNMENT The thread tried to read or write data that is misaligned on\n");
		logfatal("      hardware that does not provide alignment. For example, 16-bit values must be aligned on 2-byte\n");
		logfatal("      boundaries; 32-bit values on 4-byte boundaries, and so on.\n");
		break;
	case EXCEPTION_FLT_DENORMAL_OPERAND:
		logfatal("Desc: EXCEPTION_FLT_DENORMAL_OPERAND One of the operands in a floating-point operation is denormal. A\n");
		logfatal("      denormal value is one that is too small to represent as a standard floating-point value.\n");
		break;
	case EXCEPTION_FLT_DIVIDE_BY_ZERO:
		logfatal("Desc: EXCEPTION_FLT_DIVIDE_BY_ZERO The thread tried to divide a floating-point value by a floating-point\n");
		logfatal("      divisor of zero.\n");
		break;
	case EXCEPTION_FLT_INEXACT_RESULT:
		logfatal("Desc: EXCEPTION_FLT_INEXACT_RESULT The result of a floating-point operation cannot be represented exactly\n");
		logfatal("      as a decimal fraction.\n");
		break;
	case EXCEPTION_FLT_INVALID_OPERATION:
		logfatal("Desc: EXCEPTION_FLT_INVALID_OPERATION This exception represents any floating-point exception not included\n");
		logfatal("      in this list.\n");
		break;
	case EXCEPTION_FLT_OVERFLOW:
		logfatal("Desc: EXCEPTION_FLT_OVERFLOW The exponent of a floating-point operation is greater than the magnitude\n");
		logfatal("      allowed by the corresponding type.\n");
		break;
	case EXCEPTION_FLT_STACK_CHECK:
		logfatal("Desc: EXCEPTION_FLT_STACK_CHECK The stack overflowed or underflowed as the result of a floating-point\n");
		logfatal("      operation.\n");
		break;
	case EXCEPTION_FLT_UNDERFLOW:
		logfatal("Desc: EXCEPTION_FLT_UNDERFLOW The exponent of a floating-point operation is less than the magnitude\n");
		logfatal("      allowed by the corresponding type.\n");
		break;
	case EXCEPTION_ILLEGAL_INSTRUCTION:
		logfatal("Desc: EXCEPTION_ILLEGAL_INSTRUCTION The thread tried to execute an invalid instruction.\n");
		break;
	case EXCEPTION_IN_PAGE_ERROR:
		logfatal("Desc: EXCEPTION_IN_PAGE_ERROR The thread tried to access a page that was not present, and the system\n");
		logfatal("      was unable to load the page. For example, this exception might occur if a network connection is lost while running a program over the network.\n");
		break;
	case EXCEPTION_INT_DIVIDE_BY_ZERO:
		logfatal("Desc: EXCEPTION_INT_DIVIDE_BY_ZERO The thread tried to divide an integer value by an integer divisor\n");
		logfatal("      of zero.\n");
		break;
	case EXCEPTION_INT_OVERFLOW:
		logfatal("Desc: EXCEPTION_INT_OVERFLOW The result of an integer operation caused a carry out of the most significant\n");
		logfatal("      bit of the result.\n");
		break;
	case EXCEPTION_INVALID_DISPOSITION:
		logfatal("Desc: EXCEPTION_INVALID_DISPOSITION An exception handler returned an invalid disposition to the exception\n");
		logfatal("      dispatcher. Programmers using a high-level language such as C should never encounter this exception.\n");
		break;
	case EXCEPTION_NONCONTINUABLE_EXCEPTION:
		logfatal("Desc: EXCEPTION_NONCONTINUABLE_EXCEPTION The thread tried to continue execution after a noncontinuable\n");
		logfatal("      exception occurred.\n");
		break;
	case EXCEPTION_PRIV_INSTRUCTION:
		logfatal("Desc: EXCEPTION_PRIV_INSTRUCTION The thread tried to execute an instruction whose operation is not allowed\n");
		logfatal("      in the current machine mode.\n");
		break;
	case EXCEPTION_SINGLE_STEP:
		logfatal("Desc: EXCEPTION_SINGLE_STEP A trace trap or other single-instruction mechanism signaled that one\n");
		logfatal("      instruction has been executed.\n");
		break;
	case EXCEPTION_STACK_OVERFLOW:
		logfatal("Desc: EXCEPTION_STACK_OVERFLOW The thread used up its stack.\n");
		break;
	default:
		logfatal("Desc: Unknown.\n");
		break;
	}
	logfatal("########################################################################\n");
	logfatal("########################################################################\n\n\n");
	*/

	// Create a minidump
	GenerateDump(pExceptionPointers);

    sprintf_s(flpath, MAX_PATH, "%s\\fatal error.log", _startup_path);
    strcpy_s(msg, 1000, "OpenHoldem is about to crash.\n");
    strcat_s(msg, 1000, "A minidump has been created in your\n");
	strcat_s(msg, 1000, "OpenHoldem startup directory.\n");
    strcat_s(msg, 1000, "\n\nOpenHoldem will shut down when you click OK.");
    MessageBox(NULL, msg, "FATAL ERROR", MB_OK | MB_ICONEXCLAMATION | MB_TOPMOST);

    return EXCEPTION_EXECUTE_HANDLER;
}

BOOL CreateBMPFile(const char *szFile, HBITMAP hBMP) 
{
	// Saves the hBitmap as a bitmap.
	HDC					hdcScreen = CreateDC("DISPLAY", NULL, NULL, NULL); 
	HDC					hdcCompatible = CreateCompatibleDC(hdcScreen); 
	PBITMAPINFO			pbmi=NULL;
	BOOL				bret=FALSE;
	HANDLE				hf=NULL; // file handle
	BITMAPFILEHEADER	hdr; // bitmap file-header
	PBITMAPINFOHEADER	pbih=NULL; // bitmap info-header
	LPBYTE				lpBits=NULL; // memory pointer
	DWORD				dwTotal=0; // total count of bytes
	DWORD				cb=0; // incremental count of bytes
	BYTE				*hp=NULL; // byte pointer
	DWORD				dwTmp=0;
	BITMAP				bmp;

	memset(&bmp,0,sizeof(BITMAP));
	GetObject(hBMP,sizeof(BITMAP),&bmp);
	memset(&hdr,0,sizeof(hdr));
	{
		int cClrBits = (WORD)(bmp.bmPlanes * bmp.bmBitsPixel);
		if (cClrBits>8) {
			// No Palette (normally)
			pbmi = (PBITMAPINFO) calloc(1, sizeof(BITMAPINFOHEADER));
		}
		else {
			pbmi = (PBITMAPINFO) calloc(1, sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * (1<<(min(8,cClrBits))));
			pbmi->bmiHeader.biClrUsed = (1<<cClrBits);
		}

		// Initialize the fields in the BITMAPINFO structure.
		pbmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);

		// Retrieve the color table (RGBQUAD array) and the bits
		// (array of palette indices) from the DIB.
		if (!GetDIBits(hdcCompatible, hBMP, 0, bmp.bmHeight, NULL, pbmi, DIB_RGB_COLORS)) {
			goto to_return;
		}
	}
	pbih = &(pbmi->bmiHeader);
	pbmi->bmiHeader.biCompression=BI_RGB;
	lpBits = (LPBYTE) calloc(1, pbih->biSizeImage);

	if (!lpBits) { goto to_return; }

	if (!GetDIBits(hdcCompatible, hBMP, 0, (WORD) pbih->biHeight, lpBits, pbmi, DIB_RGB_COLORS)) {
		goto to_return;
	}

	// Create the .BMP file.
	hf = CreateFile(szFile, GENERIC_READ | GENERIC_WRITE, (DWORD) 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, (HANDLE) NULL);
	if (hf == INVALID_HANDLE_VALUE) {
		goto to_return;
	}
	hdr.bfType = 0x4d42; // 0x42 = "B" 0x4d = "M"
	// Compute the size of the entire file.
	hdr.bfSize = (DWORD) (sizeof(BITMAPFILEHEADER) + pbih->biSize + pbih->biClrUsed*sizeof(RGBQUAD) + pbih->biSizeImage);
	hdr.bfReserved1 = 0;
	hdr.bfReserved2 = 0;

	// Compute the offset to the array of color indices.
	hdr.bfOffBits = (DWORD) sizeof(BITMAPFILEHEADER) + pbih->biSize + pbih->biClrUsed*sizeof (RGBQUAD);

	// Copy the BITMAPFILEHEADER into the .BMP file.
	if (!WriteFile(hf, (LPVOID) &hdr, sizeof(BITMAPFILEHEADER),	(LPDWORD) &dwTmp, NULL)) {
		goto to_return;
	}

	// Copy the BITMAPINFOHEADER and RGBQUAD array into the file.
	if (!WriteFile(hf, (LPVOID) pbih, sizeof(BITMAPINFOHEADER) + pbih->biClrUsed * sizeof (RGBQUAD), (LPDWORD) &dwTmp, ( NULL))) {
		goto to_return;
	}

	// Copy the array of color indices into the .BMP file.
	dwTotal = cb = pbih->biSizeImage;
	hp = lpBits;
	if (!WriteFile(hf, (LPSTR) hp, (int) cb, (LPDWORD) &dwTmp,NULL)) { goto to_return; }

	// Close the .BMP file.
	if (!CloseHandle(hf)) { goto to_return; }
	bret=TRUE;


	to_return:;
	// Free memory.
	if(pbmi)free(pbmi);
	if(lpBits)free(lpBits);
	DeleteDC(hdcCompatible);
	DeleteDC(hdcScreen);

	return bret;
}

int GenerateDump(EXCEPTION_POINTERS *pExceptionPointers)
{
    bool		bMiniDumpSuccessful;
    char		szFileName[MAX_PATH]; 
    DWORD		dwBufferSize = MAX_PATH;
    HANDLE		hDumpFile;
    SYSTEMTIME	stLocalTime;
    MINIDUMP_EXCEPTION_INFORMATION	ExpParam;

    GetLocalTime(&stLocalTime);

    sprintf_s(szFileName, MAX_PATH, "%s\\%s-%s-%04d%02d%02d-%02d%02d%02d-%ld-%ld.dmp", _startup_path, 
			"ManualMode", VERSION_TEXT, stLocalTime.wYear, stLocalTime.wMonth, stLocalTime.wDay, 
			stLocalTime.wHour, stLocalTime.wMinute, stLocalTime.wSecond, GetCurrentProcessId(), 
			GetCurrentThreadId());

    hDumpFile = CreateFile(szFileName, GENERIC_READ|GENERIC_WRITE, FILE_SHARE_WRITE|FILE_SHARE_READ, 
						   0, CREATE_ALWAYS, 0, 0);
   
    ExpParam.ThreadId = GetCurrentThreadId();
    ExpParam.ExceptionPointers = pExceptionPointers;
    ExpParam.ClientPointers = TRUE;

	MINIDUMP_TYPE	mdt = (MINIDUMP_TYPE) (MiniDumpWithPrivateReadWriteMemory | 
										   MiniDumpWithDataSegs | 
										   MiniDumpWithHandleData |
										   MiniDumpWithFullMemoryInfo | 
										   MiniDumpWithThreadInfo | 
										   MiniDumpWithUnloadedModules);
   
    bMiniDumpSuccessful = MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hDumpFile,
											mdt, &ExpParam, NULL, NULL);

    return EXCEPTION_EXECUTE_HANDLER;
}

