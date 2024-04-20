// Interface.h
#ifndef _NDDS_INTERFACE_H__
#define _NDDS_INTERFACE_H__

// 작성자 : 박현재
// 작성일 : 2012/03/15
// 설  명 : NDDS API


// NDDS 인터페이스 에러코드
enum NDDSIF_ERRCODE{
	NERROR_SUCCESS = 0,
	NERROR_NDDSMODULECREATE = -1000,
	NERROR_INIT_COMMON,
	NERROR_INIT_TVAS_ERR
};

#include "interface_def.h"

/// NDDS를 초기화 한다.
/**
	@param pNddsInit	[IN] 초기화 변수
	@return
**/
extern bool NIF_Initialize(PNDDSMODULE_INIT pNddsInit);


/// NDDS를 해제 한다.
/**
	@return
**/
extern bool NIF_Uninitialize();

/// NDDS 요청 함수
/**
	@param eAPI_NDDS_Function		[IN] 요청 프로토콜 타입
	@param pRequest				[IN] 요청 데이터
	@param pResult				[OUT] 결과 데이터
	@return						true:성공, false:실패
**/
extern bool NIF_Function(eAPI_NDDS_Function eFunction, PAPI_REQUEST pRequest, PAPI_RESULT pResult);

/// 마지막 에러 코드 얻는 함수
/**
	@return						true:성공, false:실패
**/
extern NDDSIF_ERRCODE NIF_GetLastErrCode();

/// NDDS 서버 Hostname을 변경한다.
/**
    @param pszHostname		[IN] 서버 Hostname
    @param nPort			[IN] 서버 Port
    @return					에러코드(성공:DE_ERROR_SUCCESS, 이외 실패)
**/
extern bool NIF_SetTmapHost(const char *pszHostName, const unsigned int nPort);

#endif // 
