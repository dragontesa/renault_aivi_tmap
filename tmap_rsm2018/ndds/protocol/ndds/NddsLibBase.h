// CNddsLibBase.h
#ifndef __NDDSLIB_BASE_H__
#define __NDDSLIB_BASE_H__

#include "ndds_tmap_def.h"
#include "NddsLibHttpHeader.h"
#include "NddsLibConst.h"
#include "ndds_memory.h"
#include <stdio.h>
#include <sys/types.h>
#ifdef _WIN32
#include "SDL.h"
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif

class CNddsLibBase;

// 통신 관리 데이터
typedef struct tagNddsProtocolSessionData
{
	int					nSessionID;			// 세션 아이디 (요청 프로토콜 관리 키)
	unsigned long		nReqTime;

	NDDS_HTTP_HEADERDATA			sHttpHeader;		// 
	NDDS_COMMONHEADER_REQ	sCommonHeader;		// 

	eNddsProtocolType		eProtocolType;		// 요청 프로토콜 타입
	eNddsRequestType		eRequestType;		// 요청 방식
	char					*pRequestData;		// 요청 데이터 (Json)
	int					nRequestData;		// 요청 데이터 크기

	eNddsStatus				eStatus;			// 진행 상태
	int					nProgress;			// 진행률 (%)
	eNddsErrorCode			eErrCode;			// 에러코드 (통신 완료 상태 일때 만 의미 있음)
	NDDS_COMMON_RES			*pResultStruct;		// 결과 데이터

	CNddsLibBase				*pNdds;
#ifdef _LINUX
	pthread_t				pThread;			// 스레드
#else
	SDL_Thread*				pThread;			// 스레드
#endif
}NDDS_PROTOCOL_SESSION_DATA, *PNDDS_PROTOCOL_SESSION_DATA;

#define COMM_RECIVE_BUFFER		SSL3_RT_MAX_PLAIN_LENGTH		// 통신 버퍼 크기(1회 전송 크기). vNext 적용하면서 늘림. 4k -> 16k
#define COMM_DEFAULT_TIMEOUT	10			// 통신 타입아웃
#define INT_SIZE				4
#define BYTE_SIZE				1


#define JSON_ALLOC_BLOCK(pBuffer, type, nSize, memory)					\
{																		\
	pBuffer = (type)memory.pUse;										\
	memory.nUse += nSize;												\
	if(memory.nUse%4 != 0) memory.nUse += 4 - memory.nUse%4;			\
	memory.pUse = memory.pMemory + memory.nUse;							\
	memset(pBuffer, 0x00, nSize);									\
}

#define READ_BYTE_DATALEN(org,des, size)	\
{											\
	memcpy(des, org, size);				\
	org += size;							\
}

#define READ_BYTE_DATA(org,des)		\
{									\
	des = *org;						\
	org += 1;						\
}

#define READ_INTEGER_DATA(org,des)	\
{									\
	unsigned char * pOrg = org;				\
	unsigned char * pDes = (unsigned char *) &des;	\
	pDes[0] = pOrg[3];				\
	pDes[1] = pOrg[2];				\
	pDes[2] = pOrg[1];				\
	pDes[3] = pOrg[0];				\
	org += 4;						\
}

#define JUST_READ_INTEGER_DATA(org,des)	\
{									\
	unsigned char * pOrg = org;				\
	unsigned char * pDes = (unsigned char *) &des;	\
	pDes[0] = pOrg[3];				\
	pDes[1] = pOrg[2];				\
	pDes[2] = pOrg[1];				\
	pDes[3] = pOrg[0];				\
}

#ifdef _LINUX
#define ALLOC_STR_UTF8_COPY(bConverting, org, des, len, memory)										\
{																									\
	if(bConverting)																					\
	{																								\
		wchar_t *pwTemp = (wchar_t*)memory.pUse;														\
		nddsMultiByteToWideChar((char*)org, pwTemp, memory.nMemory - memory.nUse);					\
		char *pTemp = (char*)malloc((wcslen(pwTemp)+1)*2);											\
		UCS2toUTF8((unsigned short *)pwTemp, wcslen(pwTemp), pTemp);								\
		memcpy(memory.pUse, pTemp, strlen(pTemp)+1);												\
		des = (char*)memory.pUse;																	\
		memory.nUse += strlen((char*)des) + 1;													\
		if(memory.nUse%4 != 0) memory.nUse += 4 - memory.nUse%4;									\
		memory.pUse = memory.pMemory + memory.nUse;													\
		free(pTemp);																				\
	}else																							\
	{																								\
		memcpy(memory.pUse, org, strlen((char*)org)+1);										\
		des = (char*)memory.pUse;																	\
		memory.nUse += strlen((char*)des) + 1;													\
		if(memory.nUse%4 != 0) memory.nUse += 4 - memory.nUse%4;									\
		memory.pUse = memory.pMemory + memory.nUse;													\
	}																								\
}
#else
#define ALLOC_STR_UTF8_COPY(bConverting, org, des, len, memory)										\
{																									\
	if(bConverting)																					\
	{																								\
		wchar_t *pwTemp = (wchar_t*)memory.pUse;														\
		nddsMultiByteToWideChar((char*)org, pwTemp, memory.nMemory - memory.nUse);					\
		char *pTemp = SDL_iconv_string("UTF-8", "UCS-2-INTERNAL", (char*)pwTemp, (wcslen(pwTemp)+1)*2); \
		memcpy(memory.pUse, pTemp, strlen(pTemp)+1);												\
		des = (char*)memory.pUse;																	\
		memory.nUse += strlen((char*)des) + 1;													\
		if(memory.nUse%4 != 0) memory.nUse += 4 - memory.nUse%4;									\
		memory.pUse = memory.pMemory + memory.nUse;													\
		SDL_free(pTemp);																				\
	}else																							\
	{																								\
		memcpy(memory.pUse, org, strlen((char*)org)+1);										\
		des = (char*)memory.pUse;																	\
		memory.nUse += strlen((char*)des) + 1;													\
		if(memory.nUse%4 != 0) memory.nUse += 4 - memory.nUse%4;									\
		memory.pUse = memory.pMemory + memory.nUse;													\
	}																								\
}
#endif

#define ALLOC_STR_COPY(org,des,len,memory)								\
{																		\
	des = (char *)memory.pUse;											\
	memory.nUse += len + 1;												\
	memset(des,0x00,len + 1);										\
	if(memory.nUse%4 != 0) memory.nUse += 4 - memory.nUse%4;			\
	memory.pUse = memory.pMemory + memory.nUse;							\
	memcpy(des, org,len);											\
}

#define ALLOC_MEM_COPY(org,des,len,memory)								\
{																		\
	des = memory.pUse;													\
	memory.nUse += len + 1;												\
	memset(des,0x00,len + 1);										\
	if(memory.nUse%4 != 0) memory.nUse += 4 - memory.nUse%4;			\
	memory.pUse = memory.pMemory + memory.nUse;							\
	memcpy(des, org,len);											\
}

#define JSON_ALLOC_STRING(element, name, szData, memory)					\
{																			\
	std::string strValue = element[name].asString();						\
	if(!strValue.empty())													\
	{																		\
		szData = (char*)memory.pUse;										\
		memory.nUse += strValue.length() + 1;								\
		if(memory.nUse%4 != 0) memory.nUse += 4 - memory.nUse%4;			\
		memory.pUse = memory.pMemory + memory.nUse;							\
		strcpy(szData, strValue.c_str());								\
	}																		\
}

#ifdef _LINUX
#define JSON_ALLOC_MULITBYTE_STRING(bConverting, element, name, szData, memory)							\
{																										\
	std::string strValue = element[name].asString();													\
	if(!strValue.empty())																				\
	{																									\
		if(bConverting)																					\
		{																								\
			char *pTemp = (char*)malloc((strValue.length()+1)*2);										\
			UTF8toUCS2((char *)strValue.c_str(), (unsigned short *)pTemp);														\
            nddsWideCharToMultiByte((unsigned short *)pTemp, (char*)memory.pUse, memory.nMemory - memory.nUse);	\
			szData = (char*)memory.pUse;																\
			memory.nUse += strlen(szData) + 1;														\
			if(memory.nUse%4 != 0) memory.nUse += 4 - memory.nUse%4;									\
			memory.pUse = memory.pMemory + memory.nUse;													\
			free(pTemp);																				\
		}else																							\
		{																								\
			szData = (char*)memory.pUse;																\
			memory.nUse += strValue.length() + 1;														\
			if(memory.nUse%4 != 0) memory.nUse += 4 - memory.nUse%4;									\
			memory.pUse = memory.pMemory + memory.nUse;													\
			strcpy(szData, strValue.c_str());														\
		}																								\
	}																									\
}
#else
#define JSON_ALLOC_MULITBYTE_STRING(bConverting, element, name, szData, memory)							\
{																										\
	std::string strValue = element[name].asString();													\
	if(!strValue.empty())																				\
	{																									\
		if(bConverting)																					\
		{																								\
			char *pTemp = SDL_iconv_string("UCS-2-INTERNAL", "UTF-8",strValue.c_str(), (strValue.length()+1)*2); \
			nddsWideCharToMultiByte((unsigned short *)pTemp, (char*)memory.pUse, memory.nMemory - memory.nUse);	\
			szData = (char*)memory.pUse;																\
			memory.nUse += strlen(szData) + 1;														\
			if(memory.nUse%4 != 0) memory.nUse += 4 - memory.nUse%4;									\
			memory.pUse = memory.pMemory + memory.nUse;													\
			SDL_free(pTemp);																				\
		}else																							\
		{																								\
			szData = (char*)memory.pUse;																\
			memory.nUse += strValue.length() + 1;														\
			if(memory.nUse%4 != 0) memory.nUse += 4 - memory.nUse%4;									\
			memory.pUse = memory.pMemory + memory.nUse;													\
			strcpy(szData, strValue.c_str());														\
		}																								\
	}																									\
}
#endif

#define JSON_ALLOC_BASE64(element, name, type, szData, outputSize,memory)										\
{																												\
	std::string strValue = element[name].asString();															\
	if(!strValue.empty())																						\
	{																											\
		szData = (type)memory.pUse;																				\
		outputSize = CNddsBase64::NddsDecode_Base64((char*)strValue.c_str(), szData, memory.nMemory - memory.nUse);	\
		memory.nUse += outputSize;																				\
		if(memory.nUse%4 != 0) memory.nUse += 4 - memory.nUse%4;												\
		memory.pUse = memory.pMemory + memory.nUse;																\
	}																											\
}


#define JSON_ALLOC_ARRAYSTRING(element, szData, memory)							\
{																				\
	for(int i = 0;i<(int)element.size();i++)										\
	{																			\
		std::string strValue = element[i].asString();							\
		if(!strValue.empty())													\
		{																		\
			szData[i] = (char*)memory.pUse;										\
			memory.nUse += strValue.length() + 1;								\
			if(memory.nUse%4 != 0) memory.nUse += 4 - memory.nUse%4;			\
			memory.pUse = memory.pMemory + memory.nUse;							\
			strcpy(szData[i], strValue.c_str());							\
		}																		\
	}																			\
}


class CNddsLibBase
{
public:
	CNddsLibBase();
	virtual ~CNddsLibBase();

	/// 통신 객체 초기화 한다.
	/**
		@param pSessionData		[IN] 통신 세션 관리 구조체
		@return
	**/
	void Init(NDDS_PROTOCOL_SESSION_DATA *pSessionData);

	/// 마지막 에러코드를 얻는다.
	/**
		@return					에러코드(eNddsErrorCode)
	**/
	eNddsErrorCode GetLastErrorCode() {return m_eLastErrCode;};

	/// NDDS 서버 요청
	/**
		@return					성공여부
	**/
    bool RequestServer();
    bool RequestNonNddsServer();

	//////////////////////////////////////////////////////////////////////////
	// Override 함수

public:

	/// 통신 객체 초기화 한다.
	/**
		@param pReqStruct		[IN] 통신 요청 구조체
		@param pSessionData		[IN] 통신 세션 관리 구조체
		@return
	**/
	virtual bool MakeRequestData(NDDS_COMMON_REQ *pReqStruct, NDDS_PROTOCOL_SESSION_DATA *pSessionData) {return false;};

protected:


	/// 공통 해더를 Json에 추가 한다.
	/**
		@param root				[OUT] Json 객체
		@param pSessionData		[IN] 공통 해더 구조체
		@return
	**/
	virtual bool AddCommonHeader(NDDSLIB_Json::Value &root, NDDS_COMMONHEADER_REQ *pCommonHeader, bool bSeedKey) {return false;};

	
	//////////////////////////////////////////////////////////////////////////
	// 프로토콜 데이터 생성 및 파싱
	//////////////////////////////////////////////////////////////////////////
protected:


	int MakeServiceCloseRequestData(unsigned char **pReqData, char *pServiceID);			// 서비스 종료 생성

	virtual bool ParserJson(eNddsProtocolType eProtocolType,char *jsonData,int jsonLength, NDDS_COMMON_RES **ppResStruct, NDDS_MEMORYSTATUS &memory) {return false;};
	virtual bool ParserBinary(eNddsProtocolType eProtocolType,unsigned char *binaryBuf,int bufSize,NDDS_COMMON_RES **ppResStruct,NDDS_MEMORYSTATUS &memory) {return false;};
    virtual bool ParserNonNdds(eNddsProtocolType eProtocolType, char *resData, int contentLength, NDDS_COMMON_RES **ppResStruct, NDDS_MEMORYSTATUS &memory) {return false;};

	virtual bool CheckServiceClose(NDDS_PROTOCOL_SESSION_DATA *pSessionData) {return false;};
	bool MakeErrorRes(NDDS_MEMORYSTATUS &memory, NDDS_ERROR_RES **ppRes);

	bool ParserResult(NDDS_COMMONHEADER_RES common_header, unsigned char *orgContentData,int nContentSize,NDDS_MEMORYSTATUS &memory, NDDS_JSON_RES **ppRes);

	NDDSTMAP_POISEARCHES_DTO* ParserPois(NDDSLIB_Json::Value element,NDDS_MEMORYSTATUS &memory,int listCnt);

	eNddsErrorCode m_eLastErrCode;
//	HttpHeader m_HttpHeader;
	NDDS_HTTP_HEADERDATA *m_pHttpHeaderData;
	NDDS_COMMONHEADER_REQ *m_pNDDSCommonHeaderInfo;

	NDDS_PROTOCOL_SESSION_DATA *m_pSessionData;


protected:

	CNddsMemory		*m_pMemory;
    char			m_szHost[50];								// 서버 IP
    unsigned int	m_nPort;									// 서버 포트
    bool            m_bUseECMEngine;                            // is for vnext
    bool            m_bUseDefaultPort;
};

#endif // __NDDSLIB_BASE_H__
