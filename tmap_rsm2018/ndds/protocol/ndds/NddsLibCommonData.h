// NddsCommonData.h
#ifndef __NDDSLIB_COMMONDATA_H__
#define __NDDSLIB_COMMONDATA_H__

#include "NddsLibConst.h"
#include "ndds_tmap_def.h"
#include "NddsLibHttpHeader.h"
#include "ndds_memorypool.h"
#include "debug.h"
#include <cstddef>

#define COMM_BUFFER_SIZE		1000000		// 프로토콜 생성 버퍼 크기

#define VNEXT_DOMAIN    "avnext.renault.com"

class CNddsLibCommonData
{
	NOT_COPYABLE(CNddsLibCommonData)

private:
	CNddsLibCommonData(void);
	virtual ~CNddsLibCommonData(void);

public:
	void SetTmapInit(NDDSTMAP_INIT_INFO *pInit);
    void setTmapHost(const char *pszHostName, const unsigned int nPort);
	void SetAuthAccessKey(char *pAccessKey);
	void SetTmapTigSoqInfo(char *pInfo);

	inline unsigned char *GetMemory(int nSize)
	{
		std::size_t size = nSize;
		return (unsigned char*)m_pMemoryPool->GetMemory(size);
	};

	inline void FreeMemory(unsigned char *pBuffer)
	{
		m_pMemoryPool->FreeMemory((void*)pBuffer);
	};

	CNddsMemoryPool			*m_pMemoryPool;

	//////////////////////////////////////////////////////////////////////////
	// Ndds Tmap 관련
	char					m_szTmapHost[50];					// Ndds Tmap 서버 IP
	unsigned int					m_nTmapPort;						// Ndds Tmap 서버 포트
    bool                    m_bUseDefaultPort;                  // for vnext
	NDDS_HTTP_HEADERDATA			m_sTmapHttpHeader;					// Tmap Http 해더
	NDDS_COMMONHEADER_REQ	m_sTmapNddsCommonHeader;			// Tmap Ndds 공통 해더

	eNddsCharacterSet		m_eCharacterSet;					// 인터페이스 문자셋

	char	m_prevResponseServiceID[15]; // 길안내 재탐색 시 사용.

    bool m_bLogSave;
    char m_szLogPath[256];

	DECLARE_SINGLETON(CNddsLibCommonData)
};

#endif // __NDDSLIB_COMMONDATA_H__
