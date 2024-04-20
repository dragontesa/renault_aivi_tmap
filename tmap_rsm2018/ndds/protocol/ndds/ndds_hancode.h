// ndds_hancode.h

#ifndef __NDDS_HANCODE_H
#define __NDDS_HANCODE_H

#define NDDS_HIBYTE(w)			(((w) >> 8) & 0xFF)
#define NDDS_LOBYTE(w)			((w) & 0xFF)
#define NDDS_HIWORD(w)			(((w) >> 16) & 0xFFFF)
#define NDDS_LOWORD(w)			((w) & 0xFFFF)

#ifndef NDDSCALL
#if defined(_WIN32)
	#define NDDSCALL	__cdecl
#elif defined(__cplusplus)
	#define NDDSCALL
#endif
#endif // NDDSCALL

#define NULL 0

unsigned short nddsKsc5601ToKssm(unsigned short ksc);
unsigned short nddsKssmToKsc5601(unsigned short ksm);
bool nddsKsc5601ToKssmString(char* pszKsc, char* pszKssm, int nBufferSize);
bool nddsKssmToKsc5601String(char* pszKssm, char* pszKsc, int nBufferSize);

#endif // __NDDS_HANCODE_H
