// ndds_unicode.h

#ifndef __NDDS_UNICODE_H
#define __NDDS_UNICODE_H

unsigned short nddsKsc5601ToUcs2(unsigned short ksc);
unsigned short nddsUcs2ToKsc5601(unsigned short ucs);
bool nddsMultiByteToWideChar(char* pszMb,wchar_t* pszWc,int nWcBufferSize);
bool nddsWideCharToMultiByte(unsigned short* pszWc,char* pszMb,int nMbBufferSize);

unsigned short ndds_strlenMBKorea(char* pszMb);

int UCS2toUTF8(unsigned short *ucs2, int count, char *dst);
int UTF8toUCS2(char *src, unsigned short *ucs2);

#endif // __NDDS_UNICODE_H
