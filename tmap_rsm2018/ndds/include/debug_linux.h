// debug_linux.h

// 작성자 : 정인택
// 작성일 : 2008.03.31
// 설  명 : 디버그 inlne 함수 정의

#ifndef __DEBUG_LINUX_H
#define __DEBUG_LINUX_H

#include <stdarg.h>

#ifdef __MM2018__
#include "dlt/dlt.h"
DLT_IMPORT_CONTEXT(con_ndds);
#endif

/// 로그 타입. by hheo 2019.11.21
enum TeLogType {
	E_DEBUG = 0,
	E_INFO,
	E_WARNING,
	E_CRITICAL,
	E_FATAL
};

/// 로그 출력 콜백함수 정의
/**
	@param eType		타입
	@param szMsg		내용
	@return
	@author				hheo
	@date				2019.11.21
**/
typedef void (*LPCALL_NDDS_LOG)(int, char const*);

extern LPCALL_NDDS_LOG g_pNddsLogFunction;		// for rsm2018 logger. by hheo 2019.11.21
inline void SetNddsLogFunction(LPCALL_NDDS_LOG func) { g_pNddsLogFunction = func; };

/**
	@param szMsg		[IN] 로그 내용
	@return
	@author				hheo
	@date				2019.11.21
**/
void NddsExternalLogDebug(const char* szMsg, ...);

/**
	@param eType		[IN] 로그 타입
	@param szMsg		[IN] 로그 내용
	@param ...			[IN] 가변매개변수
	@return
	@author				hheo
	@date				2019.11.21
**/
void NddsExternalLog(TeLogType eType, const char* szMsg, ...);

#define DBGPRINT_BUFFER_SIZE		4096

#if defined(_DEBUG)

inline
void DebugPrintA(const char* format,...)
{
	char		szBuffer[DBGPRINT_BUFFER_SIZE];
	va_list		args;

	va_start(args,format);
	vsnprintf(szBuffer,DBGPRINT_BUFFER_SIZE-1,format,args);

#ifdef __MM2018__
	DLT_LOG(con_ndds,DLT_LOG_INFO,DLT_STRING(szBuffer));
#else
	printf("%s\n", szBuffer);
#endif

	va_end(args);
}

inline
void DebugPrintATag(const char* tag, const char* format,...)
{
	char		szBuffer[DBGPRINT_BUFFER_SIZE];
	va_list		args;

	va_start(args,format);
	vsnprintf(szBuffer,DBGPRINT_BUFFER_SIZE-1,format,args);

#ifdef __MM2018__
	DLT_LOG(con_ndds,DLT_LOG_INFO,DLT_STRING(szBuffer));
#else
	printf("%s\n", szBuffer);
#endif
	va_end(args);
}

#if defined(__QNXNTO__) && defined(__JLR2019__)
	#define DebugBreak()			__asm__ __volatile__("	.word	0xe7ffdefe");
#else
	#define DebugBreak()			asm("int $3")
#endif

#else

#define DbgPrintA(x)			((void)0)
#define DebugBreak()			((void)0)

#endif

// 아래 함수는 기존의 로그 출력 함수 대체 용이므로 그대로 두고, 직접 호출하지 말 것. 2019.11.21 by hheo
inline void NddsExternalLogDebug(const char* szMsg, ...)
{
	char		szBuffer[DBGPRINT_BUFFER_SIZE];
	va_list		args;

	va_start(args, szMsg);
	vsnprintf(szBuffer, DBGPRINT_BUFFER_SIZE-1, szMsg, args);

	NddsExternalLog(E_DEBUG, szBuffer);

	va_end(args);
}

// mm2018에서는 아래 함수를 사용해서 로그를 출력할 것. 2019.11.21 by hheo
inline void NddsExternalLog(TeLogType eType, const char* szMsg, ...)
{
	if (g_pNddsLogFunction == NULL)
	    return;

	char		szBuffer[DBGPRINT_BUFFER_SIZE];
	va_list		args;

	va_start(args, szMsg);
	vsnprintf(szBuffer, DBGPRINT_BUFFER_SIZE-1, szMsg, args);

	g_pNddsLogFunction(eType, szBuffer);

	va_end(args);
}

#endif // __DEBUG_LINUX_H
