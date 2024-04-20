#ifndef __NDDS_INTERFACE_H__
#define __NDDS_INTERFACE_H__

#include <stdio.h>
#include "NDDS_Define.h"

#ifdef __cplusplus
extern "C" {
#endif

/// NDDS Lib를 초기화 한다.
/**
	@param bSync			[IN] 초기화 동기 방식(true:동기, false:비동기-NDDS콜백(DE_MSGTYPE_NDDS_INIT)참조)
	@param pCallback		[IN] NDDS 전체 콜백 함수
	@param pInitEx			[IN] NDDS 초기화 확장 설정값
	@return					에러코드(성공:DE_ERROR_SUCCESS, 이외 실패)
**/
NDDS_API DeErrorCode NDDS_Initialize(bool bSync, LPCALL_NDDS pCallback, DS_NDDSINIT_EX *pInitEx=NULL);

/// NDDS Lib를 해제 한다.
/**
	@return
**/
NDDS_API void NDDS_Uninitialize();

/// NDDS 버전을 얻는다.
/**
	@return					버전 값
**/
NDDS_API char* NDDS_GetVersion();

/// NDDS 서버 Hostname을 변경한다.
/**
    @param pszHostname		[IN] 서버 Hostname
    @param nPort			[IN] 서버 Port
    @return					에러코드(성공:DE_ERROR_SUCCESS, 이외 실패)
**/
NDDS_API DeErrorCode NDDS_SetTmapHost(const char *pszHostName, const unsigned int nPort);

//////////////////////////////////////////////////////////////////////////
/// 경로 탐색
//////////////////////////////////////////////////////////////////////////

/// 경로 탐색을 요청 한다.
/**
	@param pRouteReq		[IN] 경로 탐색 요청 데이터
	@param bSync			[IN] 동기 설정 (비동기 방식 일때는 NDDS 콜백으로 완료가 전달된 이후 다음 경로 관련 시나리오를 진행해야 함.)
	@param nSessionID		[OUT] 세션ID (비동기 방식 일때 요청에 대한 세션ID, UI에서 세션관리용)
    @param nRouteCnt   		[OUT] 탐색성공개수
	@return					에러코드(성공:DE_ERROR_SUCCESS, 이외 실패)
**/
NDDS_API DeErrorCode NDDS_Route(DS_ROUTE_REQ *pRouteReq, bool bSync, int *nSessionID = NULL, int *nRouteCnt = NULL);

/// 경로 탐색을 중단 한다.
/**
	@param nSessionID		[IN] 세션ID(요청한 세션ID에 대해서 통신 취소. -1로 입력시 모든 통신 취소)
	@return					에러코드(성공:DE_ERROR_SUCCESS, 이외 실패)
**/
NDDS_API DeErrorCode NDDS_StopRoute(int nSessionID = -1);

/// 경로탐색후 목적지 날씨 조회
/**
	@param pResult			[OUT] 날씨정보
	@return					에러코드(성공:DE_ERROR_SUCCESS, 이외 실패)
**/
NDDS_API DeErrorCode NDDS_GetWeatherInfo(DS_WEATHERINFO_RES *pResult);

/// 경로 요약 정보
/**
	@param nSessionID		[IN] 세션ID (비동기 방식 일때 요청에 대한 세션ID, UI에서 세션관리용)
	@param nSessionID		[OUT] 경로 요약 정보
	@return					에러코드(성공:DE_ERROR_SUCCESS, 이외 실패)
**/
NDDS_API DeErrorCode NDDS_GetRouteSummaryInfo(int nSessionID, DS_ROUTESUMMARYINFO_RES *pResult);


//////////////////////////////////////////////////////////////////////////
/// 검색 관련
//////////////////////////////////////////////////////////////////////////

/// 검색(모든 검색)을 요청 한다.
/**
	@param pSearchReq		[IN] 검색 요청 데이터
	@param bSync			[IN] 동기 설정 (비동기 방식 일때는 NDDS 콜백으로 완료가 전달된 이후 결과 받아야 함)
	@param nResult			[OUT] 검색 결과 개수 (동기방식 일때만 의미 있음)
	@param nSessionID		[OUT] 세션ID (비동기 방식 일때 요청에 대한 세션ID, UI에서 세션관리용)
	@return					에러코드(성공:DE_ERROR_SUCCESS, 이외 실패)
**/
NDDS_API DeErrorCode NDDS_Search(DS_SEARCH_REQ *pSearchReq, bool bSync, int &nResult, int *nSessionID = NULL);

/// 검색 결과 리스트 얻는다.
/**
	@param eSearchType		[IN] 검색 종류 (기존 검색 했던 종류)
	@param nIdx				[IN] 결과 요청 시작 인덱스
	@param nCnt				[IN] 결과 요청 개수 (D_MAX_RESULTLIST 배수로 요청하는게 효율적임)
							[OUT] 결과 개수
	@param pResult			[OUT] 결과 리스트 (요청시 메모리 할당 필요)
	@param nTotal			[OUT] 리스트 전체 개수
	@return					에러코드(성공:DE_ERROR_SUCCESS, 이외 실패)
**/
NDDS_API DeErrorCode NDDS_GetSearchResultList(DeSearchType eSearchType, int nIdx, int &nCnt, DS_SEARCH_LIST *pResult, int *nTotal=NULL);

/// POI 상세정보를 얻는다.
/**
	@param pResult			[OUT] POI 상세정보
	@return					에러코드(성공:DE_ERROR_SUCCESS, 이외 실패)
**/
NDDS_API DeErrorCode NDDS_GetSearchPoiDetailInfo(DS_SEARCH_POI_DETAIL_INFO *pResult);


/// POI 최저가를 얻는다.
/**
    @param pResult			[OUT] POI 상세정보
    @return					에러코드(성공:DE_ERROR_SUCCESS, 이외 실패)
**/
NDDS_API DeErrorCode NDDS_GetSearchPoiStationInfo(DS_SEARCH_POI_STATION_INFO *pResult);

/// 가맹점 상세정보를 얻는다.
/**
	@param nIdx				[IN] 결과 요청 시작 인덱스
	@param nCnt				[IN] 결과 요청 개수 (D_MAX_RESULTLIST 배수로 요청하는게 효율적임)
							[OUT] 결과 개수
	@param pResult			[OUT] 가맹점 상세정보
	@param nTotal			[OUT] 전체 개수
	@return					에러코드(성공:DE_ERROR_SUCCESS, 이외 실패)
**/
NDDS_API DeErrorCode NDDS_GetSearchPoiJoinInfo(int nIdx, int &nCnt, DS_SEARCH_POI_JOIN_INFO *pResult, int *nTotal=NULL);

/// POI 최근길 조회.
/**
	@param nIdx				[IN] 결과 요청 시작 인덱스
	@param nCnt				[IN] 결과 요청 개수 (D_MAX_RESULTLIST 배수로 요청하는게 효율적임)
							[OUT] 결과 개수
	@param pResult			[OUT] 결과 리스트 (요청시 메모리 할당 필요)
	@param nTotal			[OUT] 리스트 전체 개수
	@return					에러코드(성공:DE_ERROR_SUCCESS, 이외 실패)
**/
NDDS_API DeErrorCode NDDS_GetSearchRecentDestnationList(int nIdx, int &nCnt, DS_SEARCH_RECENT_DESTNATION *pResult, int *nTotal=NULL);

/// POI 최근길 조회.
/**
    @param eSearchType		[IN] 검색 종류 (기존 검색 했던 종류)
    @param pResult			[OUT] 결과 리스트 (요청시 메모리 할당 필요)
    @return					에러코드(성공:DE_ERROR_SUCCESS, 이외 실패)
**/
NDDS_API DeErrorCode NDDS_GetUserData(DeSearchType eSearchType, DS_USER_INFO pResult);

/// POI ev충전소 상세정보 조회
/**
    @param eSearchType		[IN] 검색 종류 (기존 검색 했던 종류)
    @param pResult			[OUT] 충전소 상세정보 (요청시 메모리 할당 필요)
    @return					에러코드(성공:DE_ERROR_SUCCESS, 이외 실패)
**/
NDDS_API DeErrorCode NDDS_GetPoiEvStationInfo(DeSearchType eSearchType, DS_EVSTATION_INFO *pResult);

//////////////////////////////////////////////////////////////////////////
/// 즐겨찾기 관련
//////////////////////////////////////////////////////////////////////////

/// 즐겨찾기를 조회 한다.
/**
	@param pFavoriteReq		[IN] 즐겨찾기 요청 데이터
	@param bSync			[IN] 동기 설정 (비동기 방식 일때는 NDDS 콜백으로 완료가 전달된 이후 결과 받아야 함)
	@param nResult			[OUT] 즐겨찾기 결과 개수 (동기방식 일때만 의미 있음)
	@return					에러코드(성공:DE_ERROR_SUCCESS, 이외 실패)
**/
NDDS_API DeErrorCode NDDS_Favorite(DS_FAVORITE_REQ *pFavoriteReq, bool bSync, int &nResult, int *nSessionID = NULL);

/// 즐겨찾기 리스트 얻는다.
/**
	@param eFavoriteType	[IN] 즐겨찾기 종류
	@param nIdx				[IN] 결과 요청 시작 인덱스
	@param nCnt				[IN] 결과 요청 개수 (D_MAX_RESULTLIST 배수로 요청하는게 효율적임)
							[OUT] 결과 개수
	@param pResult			[OUT] 결과 리스트 (요청시 메모리 할당 필요)
	@param nTotal			[OUT] 리스트 전체 개수
	@return					에러코드(성공:DE_ERROR_SUCCESS, 이외 실패)
**/
NDDS_API DeErrorCode NDDS_GetFavoriteResultList(DeFavoriteType eFavoriteType, int nIdx, int &nCnt, DS_FAVORITE_LIST *pResult, int *nTotal=NULL);

/// 집으로/회사로 정보를 얻는다.(NDDS_Favorite()함수로 즐겨찾기 조회시 값이 들어옴)
/**
	@param pResult			[OUT] 집으로/회사로 정보
	@return					에러코드(성공:DE_ERROR_SUCCESS, 이외 실패)
**/
NDDS_API DeErrorCode NDDS_GetMyFavorite(DS_MYFAVORITE *pResult);

/// 차량 전송 정보를 얻는다.
/**
	@param pResult			[OUT] 위치정보
	@return					에러코드(성공:DE_ERROR_SUCCESS, 이외 실패)
**/
NDDS_API DeErrorCode NDDS_GetSendToCarInfo(DS_SENDTOCARINFO *pResult);

/// VMS 가이드 정보 조회.
/**
	@param rseId			[IN] Virtual-VMS 설치 좌표 기준 ID
	@param bSync			[IN] 동기 설정 (비동기 방식 일때는 NDDS 콜백으로 완료가 전달된 이후 다음 경로 관련 시나리오를 진행해야 함.)
	@param nSessionID		[OUT] 세션ID (비동기 방식 일때 요청에 대한 세션ID, UI에서 세션관리용)
	@return					에러코드(성공:DE_ERROR_SUCCESS, 이외 실패)
**/
NDDS_API DeErrorCode NDDS_VmsGuideInfo(char *pRseId, bool bSync, int *nSessionID = NULL);

/// VMS 가이드 정보 결과 조회
/**
	@param pResult			[OUT] 인증결과
	@return					에러코드(성공:DE_ERROR_SUCCESS, 이외 실패)
**/
NDDS_API DeErrorCode NDDS_GetVmsGuideInfo(DS_VMSGUIDEINFO *pResult);

/// 단말인증.
/**
	@param pOptCode			[IN] OTP 코드 (최대 6자, OTP 인증 할 경우 외 NULL값)
	@param bSync			[IN] 동기 설정 (비동기 방식 일때는 NDDS 콜백으로 완료가 전달된 이후 다음 시나리오를 진행해야 함.)
	@param nSessionID		[OUT] 세션ID (비동기 방식 일때 요청에 대한 세션ID, UI에서 세션관리용)
	@return					에러코드(성공:DE_ERROR_SUCCESS, 이외 실패)
**/
NDDS_API DeErrorCode NDDS_AuthTmapForSmartCar(char *pOtpCode, bool bSync, int *nSessionID = NULL);

/// 단말인증결과 조회
/**
	@param pResult			[OUT] 인증결과
	@return					에러코드(성공:DE_ERROR_SUCCESS, 이외 실패)
**/
NDDS_API DeErrorCode NDDS_GetAuthResult(DS_AUTHRESULT *pResult);

/// 인증 AccessKey 변경
/**
	@param pAccessKey		[IN] 인증후 변경된 AccessKey
	@return					에러코드(성공:DE_ERROR_SUCCESS, 이외 실패)
**/
NDDS_API DeErrorCode NDDS_SetAuthAccessKey(char *pAccessKey);

///  요청데이터 로그 저장
/**
    @param bLogSave          [IN] 로그 저장 여부
    @param pszPath           [IN] 로그 저장 위치 예) "/ivinusr/log"
    @return					에러코드(성공:DE_ERROR_SUCCESS, 이외 실패)
**/
NDDS_API DeErrorCode NDDS_SetReqDataSave(bool bLogSave, const char* pszPath);

/// AutoComplete, R-Geocode, ComplexImage, AQI. for vNext. 2019.8.8 by hheo
/**
    @param bSync			[IN] 동기 설정 (비동기 방식 일때는 NDDS 콜백으로 완료가 전달된 이후 결과 받아야 함)
    @param pszUrl			[IN] http request full url
    @param pszDomain        [IN] ssl connect domain
    @param pszResult		[OUT] http response body (json)
**/
NDDS_API DeErrorCode NDDS_NonNdds(bool bSync, const char* pszUrl, const char* pszDomain, char* pszResult, int &nResult);

/// 외부 Log 함수포인터를 연결한다.
/**
	@param pLogFunction		[IN] 외부 Log 함수의 포인터
	@author					hheo
	@date					2019.11.21
**/
NDDS_API void NDDS_SetLogFunction( void (*pLogFunction)(int,const char*) );


#ifdef __cplusplus
}
#endif

#endif // __NDDS_INTERFACE_H__
