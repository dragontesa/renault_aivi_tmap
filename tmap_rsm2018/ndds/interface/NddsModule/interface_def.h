// interface_def.h
#ifndef _NDDS_INTERFACE_DEF_H__
#define _NDDS_INTERFACE_DEF_H__

#include "common_def.h"
#include <string.h>
#ifdef _WIN32
#include <WinSock2.h>
#endif

#define MAX_PATH			260
//////////////////////////////////////////////////////////////////////////
// NDDS 업데이트 콜백 관련
//////////////////////////////////////////////////////////////////////////

// NDDS 업데이트 콜백 함수
typedef int (*LPCALL_NDDSUPDATE_PROC)(int nMsg, int nSessionID, long lParam1, long lParam2, void* lpParam/*lpParam는 UI메세지로 전달될때 누락된다.*/);

// NDDS 업데이트 콜백 메세지 종류
enum NeTypeNddsUpdateMsg{
	NM_NDDS_BEGIN_ = 0x01000000,	//------------------------- NDDS 콜백 메세지 시작(수정하지 마시오) 20120722 박현재

	NM_NDDS_SEARCH_SERVER,			// 서버 Search

	NM_NDDS_ENGINE_INIT,			// NDDS 초기화 완료 [lParam1:오류 코드, lParam2:NULL, lpParam:NULL]

	NM_NDDS_ROUTE_INIT,				// 경로탐색 초기화 중(Input 데이터 설정)		[lParam1:탐색종류(NeRouteType), lParam2:NULL, lpParam:NULL]
	NM_NDDS_ROUTE_START,			// 경로탐색 시작								[lParam1:탐색종류(NeRouteType), lParam2:NULL, lpParam:NULL]
	NM_NDDS_ROUTE_PROGRESS,			// 경로탐색 진행 중								[lParam1:탐색종류(NeRouteType), lParam2:진행률(%), lpParam:NULL]
	NM_NDDS_ROUTE_END,				// 경로탐색 완료							[lParam1:탐색종류(NeRouteType), lParam2:탐색성공개수, lpParam:경로탐색 완료 정보(DS_ROUTE_RESULT)]
	NM_NDDS_ROUTE_DATA,				// 경로탐색 결과 데이터						[lParam1:탐색종류(NeRouteType), lParam2:결과 데이터 크기, lpParam:결과 데이터(TVAS 포맷 바이너리)]

	NM_NDDS_SEARCH_INIT,			// 검색 초기화 중(Input 데이터 설정)		[lParam1:검색종류(NeSearchType), lParam2:NULL, lpParam:NULL]
	NM_NDDS_SEARCH_START,			// 검색 시작								[lParam1:검색종류(NeSearchType), lParam2:NULL, lpParam:NULL]
	NM_NDDS_SEARCH_PROGRESS,		// 검색 진행 중								[lParam1:검색종류(NeSearchType), lParam2:진행률(%), lpParam:NULL]
	NM_NDDS_SEARCH_STATUS,			// 검색 진행 중								[lParam1:검색종류(NeSearchType), lParam2:진행률(%), lpParam:NULL]
	NM_NDDS_SEARCH_END,				// 검색 완료								[lParam1:검색종류(NeSearchType), lParam2:검색개수, lpParam:NULL]

	NM_NDDS_FAVORITE_INIT,			// 즐겨찾기 초기화 중(Input 데이터 설정)			[lParam1:NULL, lParam2:NULL, lpParam:NULL]
	NM_NDDS_FAVORITE_START,			// 즐겨찾기 시작								[lParam1:NULL, lParam2:NULL, lpParam:NULL]
	NM_NDDS_FAVORITE_PROGRESS,		// 즐겨찾기 진행 중							[lParam1:진행률(%), lParam2:NULL, lpParam:NULL]
	NM_NDDS_FAVORITE_STATUS,		// 즐겨찾기 진행 중							[lParam1:, lParam2:, lpParam:NULL]
	NM_NDDS_FAVORITE_END,			// 즐겨찾기 완료								[lParam1:, lParam2:, lpParam:]

	NM_NDDS_TRAFFICINFO_INIT,		// 교통정보 초기화 중(Input 데이터 설정)			[lParam1:NULL, lParam2:NULL, lpParam:NULL]
	NM_NDDS_TRAFFICINFO_START,		// 교통정보 시작								[lParam1:NULL, lParam2:NULL, lpParam:NULL]
	NM_NDDS_TRAFFICINFO_PROGRESS,	// 교통정보 진행 중							[lParam1:진행률(%), lParam2:NULL, lpParam:NULL]
	NM_NDDS_TRAFFICINFO_END,		// 교통정보 완료								[lParam1:결과, lParam2:NULL, lpParam:NULL]

	NM_NDDS_AUTH_INIT,				// 인증 초기화 중(Input 데이터 설정)			[lParam1:NULL, lParam2:NULL, lpParam:NULL]
	NM_NDDS_AUTH_START,				// 인증 시작								[lParam1:NULL, lParam2:NULL, lpParam:NULL]
	NM_NDDS_AUTH_PROGRESS,			// 인증 진행 중							[lParam1:진행률(%), lParam2:NULL, lpParam:에러코드]
	NM_NDDS_AUTH_END,				// 인증 완료								[lParam1:사용자인증결과, lParam2:단말인증결과, lpParam:AccessKey]
	NM_NDDS_AUTH_CHANGE,			// 인증 Accesskey 변경					[lParam1:NULL, lParam2:NULL, lpParam:AccessKey]

    // AutoComplete, R-Geocode, ComplexImage, AQI. for vNext. 2019.8.8 by hheo
    NM_NDDS_NONNDDS_INIT,
    NM_NDDS_NONNDDS_START,
    NM_NDDS_NONNDDS_PROGRESS,
    NM_NDDS_NONNDDS_STATUS,
    NM_NDDS_NONNDDS_END,

	NM_NDDS_END_ = 0x01ffffff	//------------------------- NDDS 콜백 메세지 끝(수정하지 마시오)
};

//////////////////////////////////////////////////////////////////////////
// NDDS 초기화 관련
//////////////////////////////////////////////////////////////////////////

// 클라이언트 OS 타입
enum NeNddsOsType
{
	eNDDS_OsType_Node = 0,				// 미정의
	eNdds_OsType_Android,				// Android
	eNdds_OsType_IOS,					// iPhone
	eNdds_OsType_Windows,				// Windows PC
	eNdds_OsType_WinCE,					// Windows CE
	eNdds_OsType_WinMobile,				// Windows Mobile
	eNdds_OsType_Qnx,					// QNX
	eNdds_OsType_Linux,					// Linux
	eNdds_OsType_Etc						// 기타 OS
};

typedef enum
{
	eNDDS_CHARACTERSET_UTF8		= 0,				// UTF-8
	eNDDS_CHARACTERSET_EUCKR,						// EUC-KR
}NeCharacterSet;

//NDDS 초기화 구조체
typedef struct tagNddsModuleInit
{
	// 서버 정보
	bool		bServerCommerce;							// 서버 타입 (true:상용서버, flase:개발서버)
    char        szServerURL[50];                            // 서버URL
    int         nServerPort;                               // 서버Port
	char		szMacAddress[12+1];							// 단말기 맥어드레스(LSG 통신 일때만 의미 있음)
	NeNddsOsType	eNdds_OsType;									// 클라이언트 OS 구분
	char		szOSVersion[29+1];							// 단말 OS 버전
	char		szModelName[29+1];							// 단말 모델명	예) SHW-M250S
	char		szAppVersion[9+1];							// App 버전 (3자리) 예) 1.0.0
	char		szAppBuildNo[6+1];							// 빌드번호(6자리)
	char		szDeviceID[20+1];							// 장치ID
	char		szDeviceIDHu[20+1];							// 기기(HeadUnit)의 Device ID 예) A1:B2:C3:D4:E5:F6
	char		szDeviceModleName[24+1];					// 기기(HeadUnit)의 단말 모델명 예) K5
	char		szDeviceModelNo[24+1];						// 기기(HeadUnit)의 단말 모델 번호 예) JF, LF 등
	char		szVendor[24+1];								// 단말 제조사명 예) KIA
	char		szSerialKey[32+1];							// Whitelist일 경우 필수 예 ) RSM01SGA44JK43AE2ZQJIWMOM
	char		szAccessKey[50];
	NeCharacterSet			eNdds_CharSet;

	LPCALL_NDDSUPDATE_PROC	pfnNddsUpdate;								// NDDS 업데이트 콜백 함수
}NDDSMODULE_INIT, *PNDDSMODULE_INIT;

//////////////////////////////////////////////////////////////////////////
// NDDS 함수 정의
//////////////////////////////////////////////////////////////////////////

enum eAPI_NDDS_Function
{
	// Search 관련
	eAPI_NDDS_Search,						// 검색을 수행한다.
	eAPI_NDDS_GetSearchResultList,		// 검색된 결과 리스트를 얻는다.
	eAPI_NDDS_GetSearchPoiDetailInfo,		// POI 상세정보를 얻는다.
    eAPI_NDDS_GetSearchPoiStationInfo,		// POI 최저가를 얻는다.
    eAPI_NDDS_GetSearchPoiEvStationInfo,	// POI EV충전소 상세정보를 얻는다.
	eAPI_NDDS_GetSearchPoiJoinInfo,		// 가맹점 상세정보를 얻는다.
	eAPI_NDDS_GetSearchRecentDestnationList,		// POI 최근길 조회.

	// Favorite 관련
	eAPI_NDDS_Favorite,						// 즐겨찾기를 수행한다.
	eAPI_NDDS_GetFavoriteResultList,		// 즐겨찾기 결과 리스트를 얻는다.
	eAPI_NDDS_GetMyFavorite,				// 즐겨찾기 집으로/회사로 정보를 얻는다.
	eAPI_NDDS_GetSendToCarInfo,				// 차량 전송 정보 조회 결과를 얻는다.
    eAPI_NDDS_GetUserData,					// UserData를 얻는다.

	// 경로 탐색 및 안내 관련
	eAPI_NDDS_Route,						// 경로을 탐색 한다.
	eAPI_NDDS_GetRouteSummaryInfo,			// 경로요약정보 조회.
	eAPI_NDDS_StopRoute,					// 탐색중인 경로를 취소 한다.
	eAPI_NDDS_GetWeatherInfo,				// 경로탐색 완료후 날씨 조회.

	// 교통정보
	eAPI_NDDS_VmsGuideInfo,					// VMS 가이드 정보 조회.
	eAPI_NDDS_GetVmsGuideInfo,				// VMS 가이드 정보를 얻는다.

	// 단말인증
	eAPI_NDDS_AuthTmapForSmartCar,			// 단말인증을 한다.
	eAPI_NDDS_GetAuthResult,				// 단말인증 결과를 얻는다.
	eAPI_NDDS_SetAuthAccessKey,				// AccessKey를 변경한다.

    // 로그 저장
    eAPI_NDDS_SetReqDataSave,				// 로그 저장

    // Non-NDDS Request
    eAPI_NDDS_NonNdds,                      // AutoComplete, R-Geocode, ComplexImage, AQI. for vNext. 2019.8.8 by hheo

	eAPI_NDDS_FunctionMax			// API 최대 개수
};


/// API Request 기본 해더
typedef struct tagApiRequest
{
}API_REQUEST, *PAPI_REQUEST;
typedef struct tagApiResult
{
}API_RESULT, *PAPI_RESULT;

#define MAX_RESULTLIST	8		// 리스트 최대 개수

#pragma pack()

//////////////////////////////////////////////////////////////////////////
// Search 관련
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// 검색 Input 데이터

typedef struct tagSearchNddsPoiInput
{
	NeNddsSearchType						eSearchType;					// 검색 종류
	NeNddsSearchSort						eSortType;						// 정렬 기준
	stVERTEX32							wpCurrent;						// 현위치 좌표

	char								szSearchName[MAXLEN_POINAME+1];	// 검색어
	char								szAddrName[MAXLEN_POINAME+1];		///< 주소 필터 (필터 없을때 ""으로 설정)
	int									nDist;							///< 주변 검색 거리 (0:default 센터 위임, 1이상:300m 단위) 예) 3:900m
	NeNddsOilType						eOilType;						// 유류 종류 (서버 검색은 주유소도 포함됨)
	bool								bNewAddr;						// 주소 필터시 주소코드 구주소인지 새주소인지 여부(구주소:false, 새주소:true)
}SEARCH_NDDSPOI_INPUT, *PSEARCH_NDDSPOI_INPUT;

typedef struct tagSearchNddsAroundPoiInput
{
	NeNddsSearchType						eSearchType;					// 검색 종류
	stVERTEX32							wpCurrent;						// 현위치 좌표

	char								szSearchName[MAXLEN_POINAME+1];	// 검색어
	int									nDist;							///< 주변 검색 거리 (0:default 센터 위임, 1이상:300m 단위) 예) 3:900m
	bool								bNewAddr;						// 주소 필터시 주소코드 구주소인지 새주소인지 여부(구주소:false, 새주소:true)
}SEARCH_NDDSAROUNDPOI_INPUT, *PSEARCH_NDDSAROUNDPOI_INPUT;

typedef struct tagSearchNddsPoiDetailInfo
{
	NeNddsSearchType						eSearchType;					// 검색 종류
	int										nPoiID;							// POI ID
	char									navSeq[7];					// 입구점번호
	int										nSizeWidth;						// 화면 가로 사이즈
	int										nSizeHeight;						// 화면 세로 사이즈
}SEARCH_NDDSPOIDETAILINFO_INPUT, *PSEARCH_NDDSPOIDETAILINFO_INPUT;

typedef struct tagSearchNddsPoiJoinInfo
{
	NeNddsSearchType						eSearchType;					// 검색 종류
	int										nPoiID;							// POI ID
}SEARCH_NDDSPOIJOININFO_INPUT, *PSEARCH_NDDSPOIJOININFO_INPUT;

typedef struct tagSearchNddsRecentDestnation
{
}SEARCH_NDDSRECENTDESTNATION_INPUT, *PSEARCH_NDDSRECENTDESTNATION_INPUT;

typedef struct tagSearchNddsAreaNamesByStepInput
{
	NeNddsSearchType						eSearchType;					// 검색 종류
	char								szAddrName1[MAXLEN_POINAME+1];	// 검색어
	char								szAddrName2[MAXLEN_POINAME+1];	// 검색어
	bool								bNewAddr;						// 주소 필터시 주소코드 구주소인지 새주소인지 여부(구주소:false, 새주소:true)
}SEARCH_NDDSAREANAMESBYSTEP_INPUT, *PSEARCH_NDDSAREANAMESBYSTEP_INPUT;

typedef struct tagUploadRecentInput
{
	NeNddsSearchType						eSearchType;
    char szSvcUploadYn[7+1];
    int nUploadRecentPoiCnt;
    N_SEARCH_UPLOADRECENTDESTNATION pUploadRecentPoi[100];

}SEARCH_NDDSUPLOADRECENT_INPUT;

typedef struct tagRemoveRecentInput
{
    NeNddsSearchType						eSearchType;					// 검색 종류
    int nRemovePoiCnt;
    N_SEARCH_REMOVERECENTDESTNATION pRemoveRecentPoi[100];

}SEARCH_NDDSREMOVERECENT_INPUT;

typedef struct tagSearchNddsPoiStationInfoInput
{
    NeNddsSearchType						eSearchType;					// 검색 종류
    stVERTEX32							wpCurrent;						// 현위치 좌표

    int									nDist;							///< 주변 검색 거리 (9:default 센터 위임, 1이상:300m 단위) 예) 3:900m
    char								szDataKind[MAXLEN_DATAKIND];	// 3:주유소, 4:충전소(3:default)
    char								szFuelType[MAXLEN_FUELTYPE];	//< 연료타입 fuletype이 있으면 datakind를 무시(FT_GAS:휘발유, FT_DISEL:경유, FT_LPG:LPG, FT_GASPM:고급휘발유)
}SEARCH_NDDSPOISTATIONINFO_INPUT, *PSEARCH_NDDSPOISTATIONINFO_INPUT;

typedef struct tagSearchNddsPoiEvStationInfoInput
{
    NeNddsSearchType					eSearchType;					// 검색 종류

    int									nPoiID;							/// POI ID
}SEARCH_NDDSPOIEVSTATIONINFO_INPUT, *PSEARCH_NDDSPOIEVSTATIONINFO_INPUT;

typedef struct tagSearchNddsPoiEvStationsByPointInput
{
    NeNddsSearchType					eSearchType;					// 검색 종류
    stVERTEX32							wpCurrent;						// 현위치 좌표

    int									nDist;							///< 주변 검색 거리 (9:default 센터 위임, 1이상:300m 단위) 예) 3:900m
}SEARCH_NDDSPOIEVSTATIONSBYPOINT_INPUT, *PSEARCH_NDDSPOIEVSTATIONSBYPOINT_INPUT;

typedef union tagSearchInput
{
	NeNddsSearchType						eSearchType;					// 검색 종류
	NeNddsSearchSort						eSortType;						// 정렬 기준
	stVERTEX32							wpCurrent;						// 현위치 좌표

	SEARCH_NDDSPOI_INPUT				sSearchNddsPoi;
	SEARCH_NDDSAROUNDPOI_INPUT			sSearchNddsAroundPoi;
	SEARCH_NDDSPOIDETAILINFO_INPUT		sSearchNddsPoiDetailInfo;
	SEARCH_NDDSPOIJOININFO_INPUT		sSearchNddsPoiJoinInfo;
	SEARCH_NDDSAREANAMESBYSTEP_INPUT	sSearchNddsAreaNamesByStep;
    SEARCH_NDDSUPLOADRECENT_INPUT		sSearchNddsUploadRecentInfo;
    SEARCH_NDDSREMOVERECENT_INPUT		sSearchNddsRemoveRecentInfo;
    SEARCH_NDDSPOISTATIONINFO_INPUT     sSearchNddsPoiStationInfo;
    SEARCH_NDDSPOIEVSTATIONINFO_INPUT	sSearchNddsPoiEvStationInfo;
    SEARCH_NDDSPOIEVSTATIONSBYPOINT_INPUT	sSearchNddsPoiEvStationsByPoint;
}SEARCH_INPUT, *PSEARCH_INPUT;
//////////////////////////////////////////////////////////////////////////

/// API : 모든 검색을 수행한다.
typedef struct tagApiSearchReq : public API_REQUEST
{
	tagApiSearchReq()
	{
		memset(this, 0, sizeof(tagApiSearchReq));
	}
	bool			bSync;					// 비동기 여부
	SEARCH_INPUT	sSearchInput;			// 검색 Input 데이터
}API_SEARCH_REQ, *PAPI_SEARCH_REQ;
typedef struct tagSearchRes : public API_RESULT
{
	int nResult;
	int nSessionID;
	char szErrorMsg[256];					// 서버 일때만 존재
}API_SEARCH_RES, *PAPI_SEARCH_RES;

/// API : 검색된 결과 리스트를 얻는다.
typedef struct tagApiSearchResultListReq : public API_REQUEST
{
	NeNddsSearchType eSearchType;				// 검색 타입
	int nIdx;						// 시작 인덱스
	int nCnt;						// 개수
}API_SEARCH_RESULTLIST_REQ, *PAPI_SEARCH_RESULTLIST_REQ;
typedef struct tagSearchResultListRes : public API_RESULT
{
	int nResult;
	int nTotal;
	N_SEARCH_LIST sList[MAX_RESULTLIST];
}API_SEARCH_RESULTLIST_RES, *PAPI_SEARCH_RESULTLIST_RES;

/// API : POI 상세정보를 얻는다.
typedef struct tagApiSearchPoiDetailInfoReq : public API_REQUEST
{
}API_SEARCH_POIDETAILINFO_REQ, *PAPI_SEARCH_POIDETAILINFO_REQ;
typedef struct tagSearchPoiDetailInfoRes : public API_RESULT
{
	int nResult;
	N_SEARCH_POIDETAILINFO sPoiDetailInfo;
}API_SEARCH_POIDETAILINFO_RES, *PAPI_SEARCH_POIDETAILINFO_RES;

/// API : UserData를 얻는다.
typedef struct tagApiGetUserDataInfoReq : public API_REQUEST
{
}API_GET_USERDATA_REQ, *PAPI_GET_USERDATA_REQ;
typedef struct tagGETUSERDATAInfoRes : public API_RESULT
{
    N_GET_USERDATA	sUserData;
}API_GET_USERDATA_RES, *PAPI_GET_USERDATA_RES;

/// API : POI 최저가를 얻는다.
typedef struct tagApiSearchPoiStationInfoReq : public API_REQUEST
{
}API_SEARCH_POISTATIONINFO_REQ, *PAPI_SEARCH_POISTATIONINFO_REQ;
typedef struct tagSearchPoiStationInfoRes : public API_RESULT
{
    int nResult;
    N_SEARCH_POISTATIONINFO sPoiStationInfo;
}API_SEARCH_POISTATIONINFO_RES, *PAPI_SEARCH_POISTATIONINFO_RES;

/// API : POI ev충전소 상세정보를 얻는다.
typedef struct tagApiSearchPoiEvStationInfoReq : public API_REQUEST
{
    int nPoiID;
}API_SEARCH_POIEVSTATIONINFO_REQ, *PAPI_SEARCH_POIEVSTATIONINFO_REQ;
typedef struct tagSearchPoiEvStationInfoRes : public API_RESULT
{
    int nResult;
    N_SEARCH_POIEVSTATIONINFO sPoiEvStationInfo;
}API_SEARCH_POIEVSTATIONINFO_RES, *PAPI_SEARCH_POIEVSTATIONINFO_RES;

/// API : 가맹점 상세정보를 얻는다.
typedef struct tagApiSearchPoiJoinInfoReq : public API_REQUEST
{
	int nIdx;						// 시작 인덱스
	int nCnt;						// 개수
}API_SEARCH_POIJOININFO_REQ, *PAPI_SEARCH_POIJOININFO_REQ;
typedef struct tagSearchPoiJoinInfoRes : public API_RESULT
{
	int nResult;
	int nTotal;
	N_SEARCH_POIJOININFO sPoiJoinInfo[MAX_RESULTLIST];
}API_SEARCH_POIJOININFO_RES, *PAPI_SEARCH_POIJOININFO_RES;

/// API : POI 최근길 조회.
typedef struct tagApiSearchRecentDestnationReq : public API_REQUEST
{
	int nIdx;						// 시작 인덱스
	int nCnt;						// 개수
}API_SEARCH_RECENTDESTNATION_REQ, *PAPI_SEARCH_RECENTDESTNATION_REQ;
typedef struct tagSearchRecentDestnationRes : public API_RESULT
{
	int nResult;
	int nTotal;
	N_SEARCH_RECENTDESTNATION sRecentDestnationList[MAX_RESULTLIST];
}API_SEARCH_RECENTDESTNATION_RES, *PAPI_SEARCH_RECENTDESTNATION_RES;

//////////////////////////////////////////////////////////////////////////
// 경로 탐색 관련
//////////////////////////////////////////////////////////////////////////

//20120815 ssh
struct NstTVASFilesInfo
{
	unsigned int		m_nRouteOption[3];                  //multi-option
	char        m_sTvasPath[3][MAX_PATH+1];     //TVAS file Name (전체 경로 포함)
};


typedef struct tagRouteInput
{
	NeNddsRouteType		eRouteType;					// 경로탐색 종류

	NstRouteReqLogic	sRouteInput;
	NstRouteSummaryReqLogic sRouteSummaryInput;
}ROUTE_INPUT, *PROUTE_INPUT;

// eAPI_NDDS_Route
typedef struct tagApiRouteReq : public API_REQUEST
{
	tagApiRouteReq()
	{
		memset(this, 0, sizeof(tagApiRouteReq));
	}
	bool			bSync;					// 비동기 여부
	ROUTE_INPUT		sRouteInput;
}API_ROUTE_REQ, *PAPI_ROUTE_REQ;
typedef struct tagApiRouteRes : public API_RESULT
{
	int nRouteCnt;						// 경로 개수
	int nRouteRet;						// 경로 탐색 에러코드(동기 일때만)
	int nSessionID;
	NstRouteCollectLogic sRouteOutput;
}API_ROUTE_RES, *PAPI_ROUTE_RES;

// eAPI_NDDS_StopRoute
typedef struct tagApiStopRouteReq : public API_REQUEST
{
	int nSessionID;						// 경로탐색 취소할 세션ID
}API_STOPROUTE_REQ, *PAPI_STOPROUTE_REQ;
typedef struct tagApiStopRouteRes : public API_RESULT
{
}API_STOPROUTE_RES, *PAPI_STOPROUTE_RES;

// eAPI_NDDS_GetWeatherInfo
typedef struct tagApiWeatherInfoReq : public API_REQUEST
{
}API_WEATHERINFO_REQ, *PAPI_WEATHERINFO_REQ;
typedef struct tagApiWeatherInfoRes : public API_RESULT
{
	char szSkyCode[8];
	char szSkyName[30];
	char szRainTypeCode[2];
	char szRainSinceOnTime[7];
	char szTempC1h[6];
}API_WEATHERINFO_RES, *PAPI_WEATHERINFO_RES;

// eAPI_NDDS_GetRouteSummaryInfo
typedef struct tagApiGetRoutSummaryInfoReq : public API_REQUEST
{
	int nSessionID;						// 경로요약정보 세션ID
}API_GET_ROUTESUMMARYINFO_REQ, *PAPI_GET_ROUTESUMMARYINFO_REQ;
typedef struct tagApiGetRoutSummaryInfoRes : public API_RESULT
{
	int nTotalTime;
	int nEstimationOption;
	char szEstimationTime[16];
}API_GET_ROUTESUMMARYINFO_RES, *PAPI_GET_ROUTESUMMARYINFO_RES;

/// eAPI_GetTvasReRouteData
typedef struct tagApiGetTvasReRouteDataReq : public API_REQUEST
{
}API_GET_TVASREROUTEDATA_REQ, *PAPI_GET_TVASREROUTEDATA_REQ;
typedef struct tagApiGetTvasReRouteDataRes : public API_RESULT
{
	//탐색 조건 데이터
	int					nRouteDataSize;
	unsigned char		byRouteData[1024];
	//주변링크 정보
	bool				bAroundLink;
	unsigned char		uOldRsdCode;				// 이탈 지점 기존경로 RSD 회전 코드
	unsigned char		uRsdCode;					// 진입 RSD 회전 코드
	unsigned short		uLinkID;					// 링크 id
	unsigned int		uTileCode;					// 타일코드(Link)
	unsigned char		uDirInfo;					// 방향 (0:정방향, 1:역방향)
	unsigned int		uPosX;						// 보간점 X 좌표(경도: 정수8자리 SK정규화좌표)
	unsigned int		uPosY;						// 보간점 Y 좌표(위도: 정수8자리 SK정규화좌표)
	//톨게이트 정보
	unsigned short		uPassTollID;				// 최종통과 톨게이트 ID
}API_GET_TVASREROUTEDATA_RES, *PAPI_GET_TVASREROUTEDATA_RES;

//////////////////////////////////////////////////////////////////////////
// Favorite 관련
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// 즐겨찾기 Input 데이터

typedef struct tagFavoriteNddsPoiInput
{
	NeNddsFavoriteType					eFavoriteType;						// 즐겨찾기 타입

	bool								bMyFavoriteYn;						// POI 집으로/회사로 조회 여부(“Y:true”/”N:false”). default: “N”
}FAVORITE_NDDSPOI_INPUT, *PFAVORITE_NDDSPOI_INPUT;

/// 즐겨찾기 등록 Input 데이터
typedef struct tagFavoriteNddsPoiRegistInput
{
    NeNddsFavoriteType	eFavoriteType;
    int							nPoiCnt;
    DS_REGIST_POIFAVORITE_INFO sRegistFavoriteList[MAX_RESULTLIST];
}FAVORITE_NDDSPOI_REGIST_INPUT, *PFAVORITE_NDDSPOI_REGIST_INPUT;

/// 즐겨찾기 업로드 Input 데이터
typedef struct tagFavoriteNddsPoiUploadInput
{
    NeNddsFavoriteType			eFavoriteType;
    bool						bMyFavorite;
    char                        szUploadOption[7];
    DS_UPLOAD_MYFAVORITE_INFO	sUploadMyFavorite;
    int							nPoiCnt;
    DS_UPLOAD_POIFAVORITE_INFO	sUploadFavoriteList[MAX_RESULTLIST];
}FAVORITE_NDDSPOI_UPLOAD_INPUT, *PFAVORITE_NDDSPOI_UPLOAD_INPUT;

/// 즐겨찾기 멀티삭제 Input 데이터
typedef struct tagFavoriteNddsPoiRemoveInput
{
    NeNddsFavoriteType			eFavoriteType;
    int							nPoiCnt;
    DS_REMOVE_POIFAVORITE_INFO	sRemoveFavoriteList[MAX_RESULTLIST];
}FAVORITE_NDDSPOI_REMOVE_INPUT, *PFAVORITE_NDDSPOI_REMOVE_INPUT;

/// 즐겨찾기 단건 수정 Input 데이터
typedef struct tagFavoriteNddsPoiModifyInput
{
    NeNddsFavoriteType			eFavoriteType;
    DS_MODIFY_POIFAVORITE_INFO	sModifyFavorite;
}FAVORITE_NDDSPOI_MODIFY_INPUT, *PFAVORITE_NDDSPOI_MODIFY_INPUT;

typedef struct tagFavoriteNddsRegisterSendToCarInfoInput
{
	NeNddsFavoriteType					eFavoriteType;						// 즐겨찾기 타입

	char								szCustName[MAXLEN_SENDTOCARNAME+1];
	stVERTEX32							tp;
	int									nPoiID;							// POI ID
	char								RPFlag;							// RPFlag
	char								navSeq[7];					// 입구점번호
}FAVORITE_NDDSREGISTERSENDTOCARINFO_INPUT, *PFAVORITE_NDDSREGISTERSENDTOCARINFO_INPUT;

//user-profile 조회 Inputdata
typedef struct tagFavoriteNddsFindUserProfileInput
{
    NeNddsFavoriteType					eFavoriteType;						// 즐겨찾기 타입
}FAVORITE_NDDSFINDUSERPROFILE_INPUT, *PFAVORITE_NDDSFINDUSERPROFILE_INPUT;

//user-profile 저장 InputData
typedef struct tagFavoriteNddsRegsitUserProfileInput
{
    NeNddsFavoriteType					eFavoriteType;						// 즐겨찾기 타입
    char								szEmail[MAXLEN_USEREMAIL];			// 이메일
    char								szUserName[MAXLEN_USERNAME];		// 이름
    char								szMdn[MAXLEN_USERMDN];				// 핸드폰 번호
    char								szBirth[MAXLEN_USERBIRTH];			// 생년월일
}FAVORITE_NDDSREGISTUSERPROFILE_INPUT, *PFAVORITE_NDDSREGISTUSERPROFILE_INPUT;

//car-profile 조회 Inputdata
typedef struct tagFavoriteNddsCarUserProfileInput
{
    NeNddsFavoriteType					eFavoriteType;						// 즐겨찾기 타입
}FAVORITE_NDDSFINDCARPROFILE_INPUT, *PFAVORITE_NDDSFINDCARPROFILE_INPUT;

//car-profile 저장 InputData
typedef struct tagFavoriteNddsRegsitCarProfileInput
{
    NeNddsFavoriteType eFavoriteType;		///< 즐겨찾기 타입
    char szModel[MAXLEN_CARMODEL];			///< 차종(경차:CT_COMPACT, 승용차:CT_NORMAL, 중형승합차:CT_MIDDLE, 대형승합차:CT_LARGE, 대형화물차:CT_TRUCK, 특수화물차:CT_SPECIAL)
    char szFuel[MAXLEN_CARFUEL];			///< 유종(휘발유:FT_GAS, 경유:FT_DIESEL, LPG:FT_LPG, 고급휘발유:FT_GAS)
    char szHipassYn[MAXLEN_CARHIPASS];		///< 하이패스차량 여부 "Y","N"
    char szNumber[MAXLEN_CARNUMBER];		///< 차량번호 예)03마1004
}FAVORITE_NDDSREGISTCARPROFILE_INPUT, *PFAVORITE_NDDSREGISTCARPROFILE_INPUT;

//최근길/즐겨찾기 전체리스트 조회 Inputdata
typedef struct tagFavoriteNddsUserDataInput
{
    NeNddsFavoriteType					eFavoriteType;						// 즐겨찾기 타입
}FAVORITE_NDDSFINDUSERDATA_INPUT, *PFAVORITE_NDDSFINDUSERDATA_INPUT;


typedef struct tagFavoriteNddsFindSendToCarInfoInput
{
	NeNddsFavoriteType					eFavoriteType;						// 즐겨찾기 타입

	bool								bNewAddr;
}FAVORITE_NDDSFINDSENDTOCARINFO_INPUT, *PFAVORITE_NDDSFINDSENDTOCARINFO_INPUT;


typedef struct tagFavoriteNddsRemoveSendToCarInfoInput
{
	NeNddsFavoriteType					eFavoriteType;						// 즐겨찾기 타입

	char								szCustName[MAXLEN_SENDTOCARNAME+1];
	stVERTEX32							tp;
}FAVORITE_NDDSREMOVESENDTOCARINFO_INPUT, *PFAVORITE_NDDSREMOVESENDTOCARINFO_INPUT;

typedef union tagFavoriteInput
{
	NeNddsFavoriteType					eFavoriteType;

	FAVORITE_NDDSPOI_INPUT				sFavoriteNddsPoi;
    FAVORITE_NDDSPOI_REGIST_INPUT		sRegistFavoriteNddsPoi;	// 즐겨찾기 등록
    FAVORITE_NDDSPOI_UPLOAD_INPUT		sUploadFavoriteNddsPoi;	// 즐겨찾기 전체 업로드
    FAVORITE_NDDSPOI_REMOVE_INPUT		sRemoveFavoriteNddsPoi;	// 즐겨찾기 멀티 삭제
    FAVORITE_NDDSPOI_MODIFY_INPUT		sModifyFavoriteNddsPoi;	// 즐겨찾기 단건 수정
	FAVORITE_NDDSREGISTERSENDTOCARINFO_INPUT	sFavoriteNddsRegiSendToCarInfo;
	FAVORITE_NDDSFINDSENDTOCARINFO_INPUT		sFavoriteNddsFindSendToCarInfo;
	FAVORITE_NDDSREMOVESENDTOCARINFO_INPUT		sFavoriteNddsRemoveSendToCarInfo;
    FAVORITE_NDDSFINDUSERPROFILE_INPUT			sFavoriteNddsFindUserProfile;	//User-Profile 조회
    FAVORITE_NDDSREGISTUSERPROFILE_INPUT		sFavoriteNddsRegistUserProfile;	//User-Profile 저장
    FAVORITE_NDDSFINDCARPROFILE_INPUT			sFavoriteNddsFindCarProfile;	//Car-Profile 조회
    FAVORITE_NDDSREGISTCARPROFILE_INPUT			sFavoriteNddsRegistCarProfile;	//Car-Profile 저장
    FAVORITE_NDDSFINDUSERDATA_INPUT				sFavoriteNddsFindUserData;		//최근길/즐겨찾기 전체리스트 조회
}FAVORITE_INPUT, *PFAVORITE_INPUT;
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// 즐겨찾기
//////////////////////////////////////////////////////////////////////////
/// API : 모든 즐겨찾기를 수행한다.
typedef struct tagApiFavoritehReq : public API_REQUEST
{
	tagApiFavoritehReq()
	{
		memset(this, 0, sizeof(tagApiFavoritehReq));
	}
	bool			bSync;					// 비동기 여부
	FAVORITE_INPUT	sFavoriteInput;			// 즐겨찾기 Input 데이터
}API_FAVORITE_REQ, *PAPI_FAVORITE_REQ;
typedef struct tagFavoriteRes : public API_RESULT
{
	int nResult;
	int nSessionID;
	char szErrorMsg[256];					// 서버 일때만 존재
}API_FAVORITE_RES, *PAPI_FAVORITE_RES;

/// API : 즐겨찾기 결과 리스트를 얻는다.
typedef struct tagApiFavoriteResultListReq : public API_REQUEST
{
	NeNddsFavoriteType eFavoriteType;				// 검색 타입
	int nIdx;						// 시작 인덱스
	int nCnt;						// 개수
}API_FAVORITE_RESULTLIST_REQ, *PAPI_FAVORITE_RESULTLIST_REQ;
typedef struct tagFavoriteResultListRes : public API_RESULT
{
	int nResult;
	int nTotal;
	N_FAVORITE_LIST sList[MAX_RESULTLIST];
}API_FAVORITE_RESULTLIST_RES, *PAPI_FAVORITE_RESULTLIST_RES;

/// API : 즐겨찾기 집으로/회사로 정보를 얻는다.
typedef struct tagApiMyFavoriteReq : public API_REQUEST
{
}API_MYFAVORITE_REQ, *PAPI_MYFAVORITE_REQ;
typedef struct tagMyFavoriteRes : public API_RESULT
{
	char	szHomeCustName[MAXLEN_POINAME+1];			// 집으로 명칭 (고객이 입력한 명칭)
	stVERTEX32 Hometp;	
	int		nHomePoiId;			// 집으로 POI ID
	char	szHomeNavSeq[7];			// 집으로 입구점 일련번호
	char	byHomeRpFlag;			// 집으로 RP FLAG
	char	szHomeAddInfo[MAXLEN_FAVADDINFO+1];			// 집으로 부가정보 (행정구역명,좌표)
	char	szHomeInsDatetime[MAXLEN_SVCDATE+1];		// 집으로 입력일시 (YYYYMMDDHH24MISS)
	char	szOfficeCustName[MAXLEN_POINAME+1];		// 회사로 명칭 (고객이 입력한 명칭)
	stVERTEX32 Officetp;
	int		nOfficePoiId;			// 회사로 POI ID
	char	szOfficeNavSeq[7];			// 회사로 입구점 일련번호
	char	byOfficeRpFlag;			// 회사로 RP FLAG
	char	szOfficeAddInfo[MAXLEN_FAVADDINFO+1];		// 회사로 부가정보 (행정구역명,좌표)
	char	szOfficeInsDatetime[MAXLEN_SVCDATE+1];	// 회사로 입력일시 (YYYYMMDDHH24MISS)
}API_MYFAVORITE_RES, *PAPI_MYFAVORITE_RES;

/// API : 차량 전송 정보 조회 결과를 얻는다.
typedef struct tagApiSendToCarInfoReq : public API_REQUEST
{
}API_SENDTOCARINFO_REQ, *PAPI_SENDTOCARINFO_REQ;
typedef struct tagSendToCarInfoRes : public API_RESULT
{
	int nPoiId;			// POI ID
	char szNavSeq[7];			// 입구점 일련번호
	char szCustName[MAXLEN_SENDTOCARNAME+1];			// 명칭
	stVERTEX32 tp;
	char szAddrname[80];
	char byRPFlag;			// RP Flag
	char szTell[16];	//081028
	char szInsDatetime[MAXLEN_SVCDATE+1];		// 입력일시 (YYYYMMDDHH24MISS)
}API_SENDTOCARINFO_RES, *PAPI_SENDTOCARINFO_RES;
//////////////////////////////////////////////////////////////////////////
// 교통정보
//////////////////////////////////////////////////////////////////////////
/// eAPI_VmsGuideInfo
typedef struct tagApiVmsGuideInfoReq : public API_REQUEST
{
	bool			bSync;					// 비동기 여부
	char			szRseId[20];			// Virtual-VMS 설치 좌표 기준 ID
}API_VMSGUIDEINFO_REQ, *PAPI_VMSGUIDEINFO_REQ;

typedef struct tagVmsGuideInfoRes : public API_RESULT
{
	int nResult;
	int nSessionID;
	char szErrorMsg[256];					// 서버 일때만 존재
}API_VMSGUIDEINFO_RES, *PAPI_VMSGUIDEINFO_RES;

/// eAPI_GetVmsGuideInfo
typedef struct tagApiGetVmsGuideInfoReq : public API_REQUEST
{
}API_GET_VMSGUIDEINFO_REQ, *PAPI_GET_VMSGUIDEINFO_REQ;

typedef struct tagGetVmsGuideInfoRes : public API_RESULT
{
	char szBaseSdttm[13];
	char szDataSource[50];
	char szContentCode[4];
	char szContentName[30];
	char szContent[128];
}API_GET_VMSGUIDEINFO_RES, *PAPI_GET_VMSGUIDEINFO_RES;

//////////////////////////////////////////////////////////////////////////
// 인증
//////////////////////////////////////////////////////////////////////////
typedef struct tagApiAuthTmapForSmartCarReq : public API_REQUEST
{
	bool			bSync;					// 비동기 여부
	char			szOtpCode[7];				// OTP코드
}API_AUTHTMAPFORSMARTCAR_REQ, *PAPI_AUTHTMAPFORSMARTCAR_REQ;
typedef struct tagApiAuthTmapForSmartCarRes : public API_RESULT
{
	int nResult;
	int nSessionID;
	char szErrorMsg[256];					// 서버 일때만 존재
}API_AUTHTMAPFORSMARTCAR_RES, *PAPI_AUTHTMAPFORSMARTCAR_RES;

typedef struct tagApiGetAuthResultReq : public API_REQUEST
{
}API_GET_AUTHRESULT_REQ, *PAPI_GET_AUTHRESULT_REQ;

typedef struct tagGetAuthResultRes : public API_RESULT
{
	char szErrorCode[7];
	char szAccessKey[50];							///< AccessKey
	N_AUTH_COMMONINFO sCommInfo;
	N_AUTH_AUTHINFO sAuthInfo;
	N_AUTH_HEADUNITDEVICEAUTHINFO sHeadUnitDeviceAuthInfo;
	N_AUTH_TIDAUTHRESINFO sTidAuthResInfo;
	N_AUTH_VERSIONINFO sVersionInfo;
	N_AUTH_APPUPDATEINFO sAppUpdateInfo;
	N_AUTH_APPCONTROLINFO sAppControlInfo;
	N_AUTH_PUSHINFO sPushInfo;
	N_AUTH_USERPROFILEINFO sUserProfileInfo;
}API_GET_AUTHRESULT_RES, *PAPI_GET_AUTHRESULT_RES;

typedef struct tagApiSetAuthAccessKeyReq : public API_REQUEST
{
	char szAccessKey[50];							///< AccessKey
}API_SET_AUTHACCESSKEY_REQ, *PAPI_SET_AUTHACCESSKEY_REQ;
typedef struct tagApiSetAuthAccessKeyRes : public API_RESULT
{
	int nResult;
}API_SET_AUTHACCESSKEY_RES, *PAPI_SET_AUTHACCESSKEY_RES;

//////////////////////////////////////////////////////////////////////////
// 로그 저장
//////////////////////////////////////////////////////////////////////////
// eAPI_NDDS_SetReqDataSave
typedef struct tagApiSetReqDataSaveReq : public API_REQUEST
{
	bool bLogSave;						// 로그 저장 여부
	char szLogSavePath[256];			// 로그 저장 위치
}API_SETREQDATASAVE_REQ, *PAPI_SETREQDATASAVE_REQ;
typedef struct tagApiSetReqDataSaveRes : public API_RESULT
{
}API_SETREQDATASAVE_RES, *PAPI_SETREQDATASAVE_RES;

//////////////////////////////////////////////////////////////////////////
// AutoComplete, R-Geocode, ComplexImage, AQI. for vNext. 2019.8.8 by hheo
// eAPI_NDDS_NonNdds
//////////////////////////////////////////////////////////////////////////
typedef struct tagApiNonNddsReq : public API_REQUEST
{
	bool			bSync;					// 비동기 여부
	char			szUrl[MAXLEN_URL];		// request full url
	char			szDomain[MAXLEN_URL];	// request domain
	char			**ppszResult;
}API_NONNDDS_REQ, *PAPI_NONNDDS_REQ;
typedef struct tagApiNonNddsRes : public API_SEARCH_RES
{
	char			*pszResult;
	bool			bResult;				// 성공 여부
}API_NONNDDS_RES, *PAPI_NONNDDS_RES;

#endif // 

