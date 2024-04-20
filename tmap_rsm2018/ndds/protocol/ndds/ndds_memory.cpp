// ndds_memory.cpp

#include "ndds_memory.h"
#include <stdlib.h>
#include <assert.h>

#define NULL 0

CNddsMemory::CNddsMemory(void)
{
	m_bAlloc = false;
	m_pBuffer = NULL;
	m_nBufferSize = 0;
	m_nBufferPos = 0;
}

CNddsMemory::~CNddsMemory(void)
{
	ExitBuffer();
}

void CNddsMemory::InitBuffer(unsigned char *pBuffer, int nBuffer)
{
	m_pBuffer = pBuffer;
	m_nBufferSize = nBuffer;
	m_nBufferPos = 0;

	m_bAlloc = false;
}

void CNddsMemory::InitBuffer(int nSize)
{
	m_pBuffer = (unsigned char*)malloc(nSize);
	m_nBufferSize = nSize;
	m_nBufferPos = 0;

	m_bAlloc = true;
}

void CNddsMemory::ExitBuffer()
{
	if(m_bAlloc)
	{
		if(m_pBuffer)
		{
			free(m_pBuffer);
		}
	}
	m_pBuffer = NULL;
	m_nBufferSize = 0;
	m_nBufferPos = 0;
}

unsigned char* CNddsMemory::ResetMemory()
{
	m_nBufferPos = 0;
	return m_pBuffer;
}

unsigned char* CNddsMemory::GetMemory(bool bAlignment, int &nFreeSize)
{
	if(bAlignment)
	{
		if(m_nBufferPos%4 != 0)
			m_nBufferPos += 4 - m_nBufferPos%4; // For 4 Byte Alignment
	}

	nFreeSize = m_nBufferSize - m_nBufferPos;
	return m_pBuffer+m_nBufferPos;
}

bool CNddsMemory::UseMemory(int nSize, int &nFreeSize)
{
	if(nSize > m_nBufferSize - m_nBufferPos)
	{
		//기본 메모리를 초과했음(확장 메모리를 생성)
		assert(0);

		return false;
	}

	m_nBufferPos += nSize;
	nFreeSize = m_nBufferSize - m_nBufferPos;

	return true;
}
