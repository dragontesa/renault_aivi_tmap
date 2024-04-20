// NddsInterface.h
#ifndef __NDDSLIB_INTERFACE_H__
#define __NDDSLIB_INTERFACE_H__

#include "ndds_tmap_def.h"

/* Set up for C function definitions, even when using C++ */
#ifdef __cplusplus
extern "C" {
#endif

/// Ndds 초기화
/**
    @param type				[IN] NDDS 인터페이스 문자 코드
    @param pNddsTmapInit	[IN] Ndds Tmap 통신 초기화 정보
    @return					에러코드(성공:TRUE, 실패:FALSE)
**/
extern bool NDDSLIB_SetInit(eNddsCharacterSet type, NDDSTMAP_INIT_INFO *pNddsTmapInit);

/// Ndds 해제
/**
    @return
**/
extern void NDDSLIB_Release();
/// Ndds 비동기 업데이트 콜백 설정
/**
    @param pCallback		[IN] Ndds 비동기 업데이트 콜백
    @return
**/
extern bool NDDSLIB_SetAyncNddsUpdate(LPCALL_ASYNC_NDDSUPDATE_PROC pCallback);

/// NDDS 서버 Hostname을 변경한다.
/**
    @param pszHostname		[IN] 서버 Hostname
    @param nPort			[IN] 서버 Port
    @return					에러코드(성공:TRUE, 실패:FALSE)
**/
extern bool NDDSLIB_SetTmapHost(const char *pszHostName, const unsigned int nPort);

/// Ndds AccessKey 변경
/**
    @param pAccessKey		[IN] Ndds Tmap AccessKey
    @return					에러코드(성공:TRUE, 실패:FALSE)
**/
extern bool NDDSLIB_SetAuthAccessKey(char *pAccessKey);

/// 통신 프로토콜 요청
/**
    @param eProtocolType	[IN] NDDS 프로토콜 타입
    @param eRequestType		[IN] NDDS 프로토콜 요청 방식
    @param pReq				[IN] 요청 공통 헤더
    @param nSessionID		[IN] 세션 아이디 (요청 프로토콜 관리 키)
    @param eErrCode			[OUT] 에러코드
    @return					에러코드(성공:TRUE, 실패:FALSE)
**/
extern bool NDDSLIB_RequestProtocol(eNddsProtocolType eProtocolType, eNddsRequestType eRequestType, NDDS_COMMON_REQ *pReq, int nSessionID, eNddsErrorCode *eErrCode);

/// 통신 프로토콜 요청(by Json String)
/**
    @param eRequestType		[IN] NDDS 프로토콜 요청 방식
    @param pReq				[IN] 요청 공통 헤더
    @param nSessionID		[OUT] 세션 아이디 (요청 프로토콜 관리 키)
    @param eErrCode			[OUT] 에러코드
    @return					에러코드(성공:TRUE, 실패:FALSE)
**/
extern bool NDDSLIB_RequestJson(eNddsRequestType eRequestType, NDDS_COMMON_REQ *pReq, int *nSessionID, eNddsErrorCode *eErrCode);

/// 통신 상태 얻기
/**
    @param nSessionID		[IN] 세션 아이디 (요청 프로토콜 관리 키)
    @param eStatus			[OUT] NDDS 프로토콜 진행 상태
    @param nProgress		[OUT] NDDS 프로토콜 진행률(%)
    @param eErrCode			[OUT] 에러코드
    @return					에러코드(성공:TRUE, 실패:FALSE)
**/
extern bool NDDSLIB_GetStatusProtocol(int nSessionID, eNddsStatus *eStatus, int *nProgress, eNddsErrorCode *eErrCode);

/// 통신 결과 데이터(구조체) 얻기
/**
    @param nSessionID		[IN] 세션 아이디 (요청 프로토콜 관리 키)
    @param bWait			[IN] 통신 완료후 결과 얻기(TRUE)
    @param eProtocolType	[OUT] NDDS 프로토콜 타입
    @param ppRes			[OUT] 응답 공통 헤더
    @param eErrCode			[OUT] 에러코드
    @return					에러코드(성공:TRUE, 실패:FALSE)
**/
extern bool NDDSLIB_GetResultProtocol(int nSessionID, bool bWait, eNddsProtocolType *eProtocolType, NDDS_COMMON_RES **ppRes, eNddsErrorCode *eErrCode);

/// 통신 취소
/**
    @param nSessionID		[IN] 세션 아이디 (요청 프로토콜 관리 키)
    @return					에러코드(성공:TRUE, 실패:FALSE)
**/
extern bool NDDSLIB_CancelProtocol(int nSessionID);


/// 모든 통신 취소
/**
    @return
**/
extern void NDDSLIB_ClearProtocol();

/// 통신 결과 구조체 메모리 해제
/**
    @param pObject		[IN] 응답 공통 헤더
    @return
**/
extern void NDDSLIB_ReleaseObject(NDDS_COMMON_RES *pObject, eNddsErrorCode eErrCode = NDDSERR_UNKNOWN);

///  요청데이터 로그 저장
/**
    @param bLogSave          [IN] 로그 저장 여부
    @param pszPath           [IN] 로그 저장 위치
    @return					에러코드(성공:TRUE, 실패:FALSE)
**/
extern bool NDDSLIB_SetReqDataSave(bool bLogSave, char* pszPath);

/* Ends C function definitions when using C++ */
#ifdef __cplusplus
}
#endif

#endif // __NDDSLIB_INTERFACE_H__
