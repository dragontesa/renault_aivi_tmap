// debug_win32.h

// 작성자 : 정인택
// 작성일 : 2008.03.31
// 설  명 : 디버그 inlne 함수 정의

#ifndef __DEBUG_WIN32_H
#define __DEBUG_WIN32_H

#if (_DEBUG)

#define DBGPRINT_BUFFER_SIZE		4096

inline
void __cdecl DebugPrintA(const char* format,...)
{
	char		szBuffer[DBGPRINT_BUFFER_SIZE];
	va_list		args;

	va_start(args,format);
	_vsnprintf(szBuffer,DBGPRINT_BUFFER_SIZE-1,format,args);
	OutputDebugStringA(szBuffer);
	va_end(args);
}

inline
void __cdecl DebugPrintATag(const char* tag, const char* format,...)
{
	char		szBuffer[DBGPRINT_BUFFER_SIZE];
	va_list		args;

	OutputDebugStringA(tag);

	va_start(args,format);
	_vsnprintf(szBuffer,DBGPRINT_BUFFER_SIZE-1,format,args);
	OutputDebugStringA(szBuffer);
	va_end(args);
}

inline
void __cdecl DebugPrintW(const wchar_t* format,...)
{
	wchar_t	szBuffer[DBGPRINT_BUFFER_SIZE];
	va_list		args;

	va_start(args,format);
	_vsnwprintf(szBuffer,DBGPRINT_BUFFER_SIZE-1,format,args);
	OutputDebugStringW(szBuffer);					
	va_end(args);
}

inline
void __cdecl DebugPrintWTag(const wchar_t* tag, const wchar_t* format,...)
{
	wchar_t	szBuffer[DBGPRINT_BUFFER_SIZE];
	va_list		args;

	OutputDebugStringW(tag);

	va_start(args,format);
	_vsnwprintf(szBuffer,DBGPRINT_BUFFER_SIZE-1,format,args);
	OutputDebugStringW(szBuffer);					
	va_end(args);
}

#define DebugBreak			__debugbreak//DebugBreak

#else

#define DbgPrintA(x)			((void)0)
#define DbgPrintATag(x)		((void)0)
#define DbgPrintW(x)			((void)0)
#define DbgPrintWTag(x)		((void)0)
#define DebugBreak			((void)0)

#endif

#endif // __DEBUG_WIN32_H
