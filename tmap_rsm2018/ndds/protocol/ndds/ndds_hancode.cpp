// ndds_hancode.cpp

#include "ndds_hancode_table.h"
#include "ndds_hancode.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>
/*
void* NDDSCALL _ndds_bsearch(const void* pKey,const void* pBase,
	unsigned int nElementCount,unsigned int nElementSize,
	int (NDDSCALL *pfnCompare)(const void*,const void*))
{
	assert(pKey != NULL);
	assert(pBase != NULL);
	assert(pfnCompare != NULL);

	int		nResult;
	unsigned int		nHalf;
	unsigned char*		pLow,pHi,pMid;

	pLow = (unsigned char*)pBase;
	pHi = (unsigned char*)pBase + (nElementCount - 1) * nElementSize;

	while(pLow <= pHi)
	{
		if(nHalf = (nElementCount >> 1))
		{
			pMid = pLow + ((nElementCount & 0x01) ? nHalf : (nHalf - 1)) * nElementSize;
			if(!(nResult = pfnCompare(pKey,pMid)))
			{
				return pMid;
			}
			else if(nResult < 0)
			{
				pHi = pMid - nElementSize;
				nElementCount = (nElementCount & 0x01) ? nHalf : nHalf - 1;
			}
			else
			{
				pLow = pMid + nElementSize;
				nElementCount = nHalf;
			}
		}
		else if(nElementCount)
			return (pfnCompare(pKey,pLow) ? NULL : pLow);
		else
			break;
	}

	return NULL;
}
*/
int _nddsCompareHanCode(const unsigned short* ele1, const unsigned short* ele2)
{
	return (*ele1 - *ele2);
}

unsigned short nddsKsc5601ToKssm(unsigned short ksc)
{
	unsigned int		c1, c2;
	int			nIndex;
	
	c1 = NDDS_HIBYTE(ksc);
	c2 = NDDS_LOBYTE(ksc);

	if(c1 == 0x0000)
	{
		return c2;
	}
	else if(c1 == 0x00A4)
	{
		return ((c2 >= 0x00A1) && (c2 <= 0x00D3)) ? NDDS_HANCODE_SINGLE_TABLE[c2 - 0x00A1] : 0;
	}
	else if((c1 < 0x00B0) || (c1 > 0x00C8) || (c2 < 0x00A1) || (c2 > 0x00FE))
	{
		return 0;
	}
	
	nIndex = ((c1 - 0x00B0) * 94) + (c2 - 0x00A1);
	
	return NDDS_HANCODE_TABLE[nIndex];
}

unsigned short nddsKssmToKsc5601(unsigned short ksm)
{
	unsigned short*		pcode;
	int			nIndex;

	if(!(ksm & 0x8000))
	{
		return ksm;
	}
	
	pcode = (unsigned short*)bsearch(
		&ksm,
		NDDS_HANCODE_TABLE,
		NDDS_HANCODE_TABLE_SIZE,
		sizeof(unsigned short),
		(int(*)(const void*, const void*))_nddsCompareHanCode);
	
	if(pcode != NULL)
	{
		nIndex = pcode - NDDS_HANCODE_TABLE;
		return ((0x00B0 + (nIndex / 94)) << 8) + (nIndex % 94) + 0x00A1;
	}
	
	for(nIndex = 0; nIndex < NDDS_HANCODE_SINGLE_TABLE_SIZE; nIndex++)
	{
		if(ksm == NDDS_HANCODE_SINGLE_TABLE[nIndex])
			return (nIndex + 0xA4A1);
	}
	
	return 0;
}

bool nddsKsc5601ToKssmString(char* pszKsc, char* pszKssm, int nBufferSize)
{
	unsigned short		c1, c2;
	unsigned short		kssm;
	int			nLength;
	
	assert(pszKsc != NULL);
	assert(pszKssm != NULL);
	
	nLength = strlen(pszKsc);
	if(nLength >= nBufferSize)
		return false;

	while((c1 = *(unsigned char*)pszKsc++) != NULL)
	{
		if(c1 & 0x80)
		{
			c2 = *(unsigned char*)pszKsc++;
			kssm = nddsKsc5601ToKssm((c1 << 8) | c2);
			if(kssm)
			{
				*pszKssm++ = NDDS_HIBYTE(kssm);
				*pszKssm++ = NDDS_LOBYTE(kssm);
			}
		}
		else
		{
			*pszKssm++ = (char)c1;
		}
	}
	
	*pszKssm = NULL;
	
	return true;
}

bool nddsKssmToKsc5601String(char* pszKssm, char* pszKsc, int nBufferSize)
{
	unsigned short		c1, c2;
	unsigned short		ksc;
	int			nLength;

	assert(pszKssm != NULL);
	assert(pszKsc != NULL);

	nLength = strlen(pszKssm);
	if(nLength >= nBufferSize)
		return false;
		
	while((c1 = *(unsigned char*)pszKssm++) != NULL)
	{
		if(c1 & 0x80)
		{
			c2 = *(unsigned char*)pszKssm++;
			ksc = nddsKssmToKsc5601((c1 << 8) | c2);
			if(ksc)
			{
				*pszKsc++ = NDDS_HIBYTE(ksc);
				*pszKsc++ = NDDS_LOBYTE(ksc);
			}
		}
		else
		{
			*pszKsc++ = (char)c1;
		}
	}
	
	*pszKsc = NULL;
	
	return true;
}

