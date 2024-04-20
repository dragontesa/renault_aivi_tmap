// debug.h

// 작성자 : 정인택
// 작성일 : 2007.12.27
// 설  명 : 디버그 매크로 정의

#ifndef __DEBUG_H
#define __DEBUG_H

#if defined(_WIN32)
	#include "debug_win32.h"
#else
	#include "debug_linux.h"
#endif

#ifndef _T
#ifdef _UNICODE
#define _T(x)				L##x
#else
#define _T(x)				x
#endif // _UNICODE
#endif // _T

#if defined(_DEBUG) || defined(__DEBUG__) || defined(_NATIVE_DEBUG_PRINT)
        #ifdef _UNICODE
            #define DBGPRINT		DebugPrintW
			#define DBGPRINT_TAG	DebugPrintWTag
            #define DBGPRINT_EX		DebugPrintExW
        #else
            #ifdef __MM2018__
                #define DBGPRINT		NddsExternalLogDebug
                #define DBGPRINT_TAG	NddsExternalLogDebug
                #define DBGPRINT_EX		NddsExternalLogDebug
            #else
                #define DBGPRINT		DebugPrintA
                #define DBGPRINT_TAG	DebugPrintATag
                #define DBGPRINT_EX		DebugPrintExA
            #endif
        #endif // _UNICODE
#ifdef _WIN32
		#define DBGBREAK			DebugBreak
#else
        #define DBGBREAK()			DebugBreak()
#endif
		#if defined(__ANDROID__)
			#define ASSERT(expr)			\
			{									\
				if(!(expr))						\
					DBGPRINT("ASSERT:%s(%d)[%s]", __FILE__, __LINE__,#expr);	\
			}
		#else
			#define ASSERT(expr)		\
			{								\
				if(!(expr))					\
					DBGBREAK();			\
			}
		#endif
#else

	#define DBGPRINT(...)		((void)0)
	#define DBGPRINT_TAG(...)	((void)0)
	#define DBGPRINT_EX(...)	((void)0)
	#define DBGBREAK()			((void)0)
	#define SETDBGPRINTID(...)	((void)0)
	#define GETDBGPRINTID()		((void)0)

	#if defined(__ANDROID__)
		#define ASSERT(expr)			\
		{									\
			if(!(expr))						\
			DBGPRINT("ASSERT:%s(%d)[%s]", __FILE__, __LINE__,#expr);	\
		}
	#else
		#define ASSERT(x)			((void)0)
	#endif

#endif

// 배열 범위 초과 매크로 추가
#ifndef INDEX_CHECK_RETURN
#define INDEX_CHECK_RETURN(nIndex, nMaxCount)	\
	if( nIndex >= nMaxCount || nIndex < 0 ){	\
	ASSERT(0);								\
	return;										\
	}											\

#endif

#ifndef INDEX_CHECK_RETURN_BOOL
#define INDEX_CHECK_RETURN_BOOL(nIndex, nMaxCount)	\
	if( nIndex >= nMaxCount || nIndex < 0 ){	\
	ASSERT(0);							\
	return FALSE;							\
	}											\

#endif

#ifndef INDEX_CHECK_BREAK
#define INDEX_CHECK_BREAK(nIndex, nMaxCount)	\
	if( nIndex >= nMaxCount || nIndex < 0 ){	\
	ASSERT(0);							\
	break;									\
	}											\

#endif

#ifndef INDEX_CHECK_CONTINUE
#define INDEX_CHECK_CONTINUE(nIndex, nMaxCount)	\
	if( nIndex >= nMaxCount || nIndex < 0 ){	\
	ASSERT(0);							\
	continue;									\
	}											\

#endif

#ifndef INDEX_CHECK_BLOCK_START
#define INDEX_CHECK_BLOCK_START(nIndex, nMaxCount) \
	if( nIndex < nMaxCount && nIndex > -1 ){	\

#endif

#ifndef INDEX_CHECK_BLOCK_END
#define INDEX_CHECK_BLOCK_END() \
	}	\

#endif

#endif // __DEBUG_H
