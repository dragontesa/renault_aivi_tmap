// ndds_unicode.cpp

#include "ndds_hancode_table.h"
#include "ndds_hancode.h"
#include "ndds_unicode_table.h"
#include "ndds_unicode.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>

/*

1. KSSM -> UCS2

	UCS2 = (((cho_index * 21) + jung_index) * 28) + jong_index + 0xAC00

	cho_index : chosung table index,
	jung_index : jungsung table index,
	jong_index : jongsung table index
	0xAC00 : UCS2 '가'

2. UCS2 -> KSSM

	cho_index = (UCS-0xAC00) / (21 * 28)
	jung_index = ((UCS-0xAC00) % (21 * 28)) / 28
	jong_index = (UCS-0xAC00) % 28

	cho_index += 2;
	jung_index += 3;
	jong_index += 1;
	
	if(jung_index >= 20)		jung_index += 6;
	else if(jung_index >= 14)	jung_index += 4;
	else if(jung_index >= 8)	jung_index == 2;
	
	if(jong_index >= 18)		jong_index += 1;
		
	KSSM = (1 << 15) | (cho_index << 10) | (jung_index << 5) | jong_index

*/

wchar_t* MBCStoUCS2(wchar_t* pDst, int dstSize, char* pSrc, int srcSize, wchar_t errChar = 0x20);

unsigned short nddsKsc5601ToUcs2(unsigned short ksc)
{
	unsigned int		c1, c2;
	int			ucs = 0;
	int			nIndex;

	c1 = NDDS_HIBYTE(ksc);
	c2 = NDDS_LOBYTE(ksc);

	if(c1 == 0x0000)
	{
		return c2;
	}

	nIndex = ((c1 - 0x00A1) * 94) + (c2 - 0x00A1);
	
	if((nIndex >= 0) && (nIndex < NDDS_KSC5601_TO_UCS2_TABLE_SIZE))
	{
		ucs = NDDS_KSC5601_TO_UCS2_TABLE[nIndex];
		if(ucs == -1)
			ucs = 0;
	}
	
	return ucs;
}

unsigned short nddsUcs2ToKsc5601(unsigned short ucs)
{
	int			i;
	unsigned short		c1, c2, c3;
	unsigned short		kssm;

	c1 = NDDS_HIBYTE(ucs);
	c2 = NDDS_LOBYTE(ucs);

	if(c1 == 0x0000)
	{
		return c2;
	}
	else if((ucs >= NDDS_UCS2_TO_KSSM_CHO_TABLE[0])
		&& (ucs <= NDDS_UCS2_TO_KSSM_CHO_TABLE[NDDS_UCS2_TO_KSSM_CHO_TABLE_SIZE-1]))
	{
		for(i=0; i<NDDS_UCS2_TO_KSSM_CHO_TABLE_SIZE; i++)
		{
			if(ucs == NDDS_UCS2_TO_KSSM_CHO_TABLE[i])
			{
				i += 2;
				kssm = 0x8000 | (i << 10) | (2 << 5) | 0x01;

				return nddsKssmToKsc5601(kssm);
			}
		}

		return 0;
	}
	else if((ucs >= NDDS_UCS2_TO_KSSM_JUNG_TABLE[0])
		&& (ucs <= NDDS_UCS2_TO_KSSM_JUNG_TABLE[NDDS_UCS2_TO_KSSM_JUNG_TABLE_SIZE-1]))
	{
		for(i=0; i<NDDS_UCS2_TO_KSSM_JUNG_TABLE_SIZE; i++)
		{
			if(ucs == NDDS_UCS2_TO_KSSM_JUNG_TABLE[i])
			{
				i += 3;
				if(i >= 20)
				{
					i += 6;
				}
				else if(i >= 14)
				{
					i += 4;
				}
				else if(i >= 8)
				{
					i += 2;
				}

				kssm = 0x8000 | (1 << 10) | (i << 5) | 0x01;

				return nddsKssmToKsc5601(kssm);
			}

		}

		return 0;
	}
	
	ucs -= 0xAC00;

	c1 = ucs / (21 * 28);
	c2 = (ucs % (21 * 28)) / 28;
	c3 = ucs % 28;

	c1 += 2;
	c2 += 3;
	c3 += 1;
		
	if(c2 >= 20)
	{
		c2 += 6;
	}
	else if(c2 >= 14)
	{
		c2 += 4;
	}
	else if(c2 >= 8)
	{
		c2 += 2;
	}

	if(c3 >= 18)
		c3 += 1;

	kssm = (0x8000 | (c1 << 10) | (c2 << 5) | c3);
	
	return nddsKssmToKsc5601(kssm);
}


unsigned short ndds_strlenMBKorea(char* pszMb)
{
	unsigned short		c1;
	int			nLengthByte;
	int			nLengthChar = 0;

	assert(pszMb != NULL);

	nLengthByte = strlen(pszMb);

	while((c1 = *(unsigned char*)pszMb++) != NULL)
	{
		if(c1 & 0x80)
			pszMb++;

		nLengthChar++;	
	}

	return nLengthChar;
}

bool nddsMultiByteToWideChar(char* pszMb, wchar_t* pszWc, int nWcBufferSize)
{
#if 1
	int			nLengthChar = 0;

	assert(pszMb != NULL);
	assert(pszWc != NULL);

	nLengthChar = ndds_strlenMBKorea(pszMb);	// 한글포함 문자열길이 구함
	if(nLengthChar >= nWcBufferSize)
		return false;

	MBCStoUCS2(pszWc, nWcBufferSize, pszMb, strlen(pszMb)+1);
#else
	unsigned short		c1, c2;
	unsigned short		ksc;
	int			nLengthByte = 0;
	int			nLengthChar = 0;
	
	assert(pszMb != NULL);
	assert(pszWc != NULL);
	
	nLengthByte = strlen(pszMb);
	nLengthChar = ndds_strlenMBKorea(pszMb);	// 한글포함 문자열길이 구함
	
	if(nLengthChar >= nWcBufferSize)
		return false;

	while((c1 = *(unsigned char*)pszMb++) != NULL)
	{
		if(c1 & 0x80)
		{
			c2 = *(unsigned char*)pszMb++;
			ksc = (c1 << 8) | c2;
			*pszWc++ = nddsKsc5601ToUcs2(ksc);
		}
		else
		{
			*pszWc++ = c1;
		}		
	}
	
	*pszWc = NULL;
#endif

	return true;
}

bool nddsWideCharToMultiByte(unsigned short* pszWc, char* pszMb, int nMbBufferSize)
{
	unsigned short		ksc, ucs;
	int			nLength = 0;
	
		
	assert(pszMb != NULL);
	assert(pszWc != NULL);
	
	while(pszWc[nLength++]);
	
	if((nLength << 1) >= nMbBufferSize)
		return false;
	
	while((ucs = *pszWc++) != NULL)
	{
		ksc = nddsUcs2ToKsc5601(ucs);
		if(ksc & 0x8000)
		{
			*pszMb++ = NDDS_HIBYTE(ksc);
			*pszMb++ = NDDS_LOBYTE(ksc);
		}
		else
		{
			*pszMb++ = NDDS_LOBYTE(ksc);
		}
	}
	
	*pszMb = NULL;
	
	return true;
}

extern unsigned short MBCStoUCS2_TABLE_0[];
extern unsigned short MBCStoUCS2_TABLE_1[];
extern unsigned short MBCStoUCS2_TABLE_2[];
extern unsigned short MBCStoUCS2_TABLE_3[];

typedef struct
{
	unsigned char   minLoByte;
	unsigned char   maxLoByte;
	unsigned char   minHiByte;
	unsigned char   maxHiByte;
	unsigned short* pTable;
} N_CONVERT_BLOCK;

static N_CONVERT_BLOCK s_btable[] =
{
	{ 0x41, 0x5A, 0x81, 0xC6, MBCStoUCS2_TABLE_0 },
	{ 0x61, 0x7A, 0x81, 0xC5, MBCStoUCS2_TABLE_1 },
	{ 0x81, 0xA0, 0x81, 0xC5, MBCStoUCS2_TABLE_2 },
	{ 0xA1, 0xFE, 0x81, 0xFD, MBCStoUCS2_TABLE_3 }
};

long s_FetchMBCS(char*& pSrc, const char* pEnd)
{
	unsigned long result;

	if (*pSrc & 0x80)
	{
		if (pSrc+1 >= pEnd)
			return -1;

		result = (unsigned char)(*pSrc++);
		result = (result << 8) | (unsigned char)(*pSrc++);
	}
	else
	{
		if (pSrc >= pEnd)
			return -1;

		result = *pSrc++;
	}

	return long(result);
}

wchar_t* MBCStoUCS2(wchar_t* pDst, int dstSize, char* pSrc, int srcSize, wchar_t errChar)
{
	if ((pDst == NULL) || (dstSize <= 0) || (pSrc == NULL) || (srcSize <= 0))
		return NULL;

	char*    pSrcEnd = pSrc + srcSize;
	wchar_t* pDstEnd = pDst + dstSize;

	while (pDst < pDstEnd)
	{
		long data = s_FetchMBCS(pSrc, pSrcEnd);

		if (data >= 0)
		{
			if (data < 0x80)
			{
				*pDst++ = wchar_t(data);
			}
			else
			{
				unsigned char LoByte = data & 0xFF;
				unsigned char hiByte = (data >> 8) & 0xFF;

				*pDst = errChar;

				for (int ixTable = 0; ixTable < sizeof(s_btable) / sizeof(s_btable[0]); ixTable++)
				{
					N_CONVERT_BLOCK& table = s_btable[ixTable];

					if ((LoByte >= table.minLoByte) && (LoByte <= table.maxLoByte) &&
					    (hiByte >= table.minHiByte) && (hiByte <= table.maxHiByte))
					{
						LoByte  -= table.minLoByte;
						hiByte  -= table.minHiByte;
						*pDst++  = table.pTable[hiByte * (table.maxLoByte - table.minLoByte + 1) + LoByte];
						break;
					}
				}
			}
			continue;
		}
		break;
	}
	
	return pDst;
}

#ifdef _LINUX
#include <iconv.h>
#endif
#include <errno.h>
#include <wchar.h>
int UCS2toUTF8(unsigned short *ucs2, int count, char *dst)
{
#ifdef _LINUX
    wchar_t *wcs = (wchar_t *)ucs2;
    char *iconv_in = (char *) wcs;
    size_t iconv_in_bytes = (wcslen(wcs) + 1 /* L'\0' */) * sizeof(wchar_t);
    size_t iconv_out_bytes = ((count)+1)*2;
    size_t ret;
    iconv_t cd;

    cd = iconv_open("UTF-8", "WCHAR_T");
    if ((iconv_t) -1 == cd) {
        return -1;
    }

    ret = iconv(cd, &iconv_in, &iconv_in_bytes, &dst, &iconv_out_bytes);
    if ((size_t) -1 == ret) {
        iconv_close(cd);
        return -1;
    }

    iconv_close(cd);
#endif

    return 0;
}

int UTF8toUCS2(char *src, unsigned short *ucs2)
{
#ifdef _LINUX
	char *wcs = (char *)ucs2;
    char *iconv_in = src;
    size_t iconv_in_bytes = (strlen(iconv_in) + 1 /* L'\0' */);
    size_t iconv_out_bytes = (strlen(iconv_in)+1)*2;
    size_t ret;
    iconv_t cd;

    cd = iconv_open("UCS-2", "UTF-8");
    if ((iconv_t) -1 == cd) {
        return -1;
    }

    ret = iconv(cd, &iconv_in, &iconv_in_bytes, &wcs, &iconv_out_bytes);
    if ((size_t) -1 == ret) {
        iconv_close(cd);
        return -1;
    }

    iconv_close(cd);
#endif

    return 0;
}
