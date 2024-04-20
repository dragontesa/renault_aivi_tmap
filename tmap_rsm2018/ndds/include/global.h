#if !defined(__GLOBAL__HEADER__)
#define __GLOBAL__HEADER__

//////////////////////////////////////////////////////////////////////////
// 시스템 환경 Define 설정
//////////////////////////////////////////////////////////////////////////
#if defined(_WIN32)
#elif defined(__ANDROID__)
#elif defined(__IPHONEOS__)
#elif defined(__QNXNTO__)
#elif defined(_LINUX)
#else
	#pragma message("(!!! ERROR !!!)")
#endif
#pragma warning(disable : 4996)

#ifdef DEBUG
	#undef _DEBUG
	#define _DEBUG
#endif // DEBUG

#ifdef _WIN32
#ifdef _WIN32_WCE
	#ifndef _OS_WINCE
		#define _OS_WINCE
	#endif
#else
	#ifndef _OS_WIN32
		#define _OS_WIN32
	#endif
#endif
#endif

//#define __SSL_SOCKET__	// OpenSSL 적용여부

#endif // __GLOBAL__HEADER__
