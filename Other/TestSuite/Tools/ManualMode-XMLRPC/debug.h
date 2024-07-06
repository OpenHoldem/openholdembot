#ifndef INC_DEBUG_H
#define INC_DEBUG_H

char * get_time(char * timebuf);
char * get_now_time(char * timebuf);
void logfatal (char* fmt, ...);
BOOL CreateBMPFile(const char *szFile, HBITMAP hBMP);
int GenerateDump(EXCEPTION_POINTERS *pExceptionPointers);

extern FILE *log_fp;

#endif /* INC_DEBUG_H */
