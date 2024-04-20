// NddsModule.h
#ifndef __NDDSMODULE_H__
#define __NDDSMODULE_H__

// 작성자 : 박현재
// 작성일 : 2008/05/07
// 설  명 :

#include "interface_def.h"
#include "ndds_inc.h"
#include "singleton.h"
#include "classcfg.h"
#include <list>
#ifdef _WIN32
#include "SDL.h"
#include <WinSock2.h>
#else
#include <pthread.h>
#include <semaphore.h>
#endif

//#include "uiResDump.h"

//정수 -> 빅엔디안 바이트 배열 변환 매크로
//2013.03.20 flintlock
#define BIGENDIAN_INT2BYTE(src, dest)	\
{										\
	dest[3] = (src & 0xff);				\
	dest[2] = ((src >> 8) & 0xff);		\
	dest[1] = ((src >> 16) & 0xff);		\
	dest[0] = ((src >> 24) & 0xff);		\
}

#define BIGENDIAN_SHORT2BYTE(src, dest)	\
{										\
	dest[1] = (src & 0xff);				\
	dest[0] = ((src >> 8) & 0xff);		\
}

//빅엔디안 바이트 배열 -> 정수 변환 매크로
//2013.03.20 flintlock
#define BIGENDIAN_BYTE2INT(src)	\
	((src[0] & 0xff) << 24)		\
	| ((src[1] & 0xff) << 16)	\
	| ((src[2] & 0xff) << 8)	\
	| (src[3] & 0xff)

//////////////////////////////////////////////////////////////////////////
// 비동기 모듈 동작 관련(경로탐색, 검색등의 비동기로 동작하는 모듈을 구동)

// 모듈 프로세스 종류
typedef enum
{
	E_NDDS_PROCTYPE_NONE = 0,		// 설정되지 않음
	E_NDDS_PROCTYPE_ROUTE,			// 경로 탐색
	E_NDDS_PROCTYPE_SEARCH,			// 검색
	E_NDDS_PROCTYPE_FAVORITE,		// 즐겨찾기
	E_NDDS_PROCTYPE_TRAFFICINFO,	// 교통정보
	E_NDDS_PROCTYPE_AUTH,			// 인증
    E_NDDS_PROCTYPE_NONNDDS,        // AutoComplete, R-Geocode, ComplexImage, AQI. for vNext. 2019.8.8 by hheo
}NeNddsModuleProcType;

// 경로 탐색(E_NDDS_PROCTYPE_ROUTE) 요청 데이터
typedef struct tagRouteInputProcData
{
	NeNddsModuleProcType		eType;				// 모듈 프로세스 종류

	ROUTE_INPUT	sRouteInput;
}ROUTE_INPUT_PROCDATA, *PROUTE_INPUT_PROCDATA;

// 경로 탐색(E_NDDS_PROCTYPE_ROUTE) 결과 데이터
typedef struct tagRouteOutputProcData
{
	NeNddsModuleProcType		eType;				// 모듈 프로세스 종류

	// 결과 데이터
	int						m_nCurRouteRet;		//현재 Route 결과값(UI에 리턴값으로 사용)
	int nResult;
	NstRouteCollectLogic sRouteOutput;
}ROUTE_OUTPUT_PROCDATA, *PROUTE_OUTPUT_PROCDATA;

// 검색(E_NDDS_PROCTYPE_SEARCH) 요청 데이터
typedef struct tagSearchInputProcData
{
	NeNddsModuleProcType		eType;				// 모듈 프로세스 종류
	SEARCH_INPUT			sSearchInput;		// 검색 Input 데이터
}SEARCH_INPUT_PROCDATA, *PSEARCH_INPUT_PROCDATA;

// 검색(E_NDDS_PROCTYPE_SEARCH) 결과 데이터
typedef struct tagSearchOutputProcData
{
	NeNddsModuleProcType		eType;				// 모듈 프로세스 종류
	
	// 결과 데이터
	int						m_nCurSearchRet;		//현재 Search 결과값(UI에 리턴값으로 사용)
	int			m_nTab;
	int nResult;
	char szErrorMsg[256];								// 에러 메세지(서버 일때만 사용)
}SEARCH_OUTPUT_PROCDATA, *PSEARCH_OUTPUT_PROCDATA;

// 즐겨찾기(E_NDDS_PROCTYPE_FAVORITE) 요청 데이터
typedef struct tagFavoriteInputProcData
{
	NeNddsModuleProcType		eType;				// 모듈 프로세스 종류
	FAVORITE_INPUT			sFavoriteInput;		// 즐겨찾기 Input 데이터
}FAVORITE_INPUT_PROCDATA, *PFAVORITE_INPUT_PROCDATA;

// 즐겨찾기(E_NDDS_PROCTYPE_FAVORITE) 결과 데이터
typedef struct tagFavoriteOutputProcData
{
	NeNddsModuleProcType		eType;				// 모듈 프로세스 종류

	// 결과 데이터
	int						m_nCurFavoriteRet;		//현재 Favorite 결과값(UI에 리턴값으로 사용)
	int nResult;
	char szErrorMsg[256];								// 에러 메세지(서버 일때만 사용)
}FAVORITE_OUTPUT_PROCDATA, *PFAVORITE_OUTPUT_PROCDATA;

// 즐겨찾기 등록(E_NDDS_PROCTYPE_REGIST_FAVORITE) 요청 데이터
typedef struct tagRegistFavoriteInputProcData
{
    NeNddsModuleProcType			eType;						// 모듈 프로세스 종류
    FAVORITE_NDDSPOI_REGIST_INPUT	sRegistFavoriteInput;		// 즐겨찾기 Input 데이터
}REGIST_FAVORITE_INPUT_PROCDATA, *PREGIST_FAVORITE_INPUT_PROCDATA;


// 즐겨찾기 전체 업로드(E_NDDS_PROCTYPE_UPLOAD_FAVORITE) 요청 데이터
typedef struct tagUploadFavoriteInputProcData
{
    NeNddsModuleProcType			eType;						// 모듈 프로세스 종류
    FAVORITE_NDDSPOI_UPLOAD_INPUT	sUploadFavoriteInput;		// 즐겨찾기 Input 데이터
}UPLOAD_FAVORITE_INPUT_PROCDATA, *PUPLOAD_FAVORITE_INPUT_PROCDATA;

// 즐겨찾기 멀티 삭제(E_NDDS_PROCTYPE_REMOVE_FAVORITE) 요청 데이터
typedef struct tagRemoveFavoriteInputProcData
{
    NeNddsModuleProcType			eType;						// 모듈 프로세스 종류
    FAVORITE_NDDSPOI_REMOVE_INPUT	sRemoveFavoriteInput;		// 즐겨찾기 Input 데이터
}REMOVE_FAVORITE_INPUT_PROCDATA, *PREMOVE_FAVORITE_INPUT_PROCDATA;

// 즐겨찾기 단건 수정(E_NDDS_PROCTYPE_MODIFY_FAVORITE) 요청 데이터
typedef struct tagModifyFavoriteInputProcData
{
    NeNddsModuleProcType			eType;						// 모듈 프로세스 종류
    FAVORITE_NDDSPOI_MODIFY_INPUT	sModifyFavoriteInput;		// 즐겨찾기 Input 데이터
}MODIFY_FAVORITE_INPUT_PROCDATA, *PMODIFY_FAVORITE_INPUT_PROCDATA;

// 교통정보(E_NDDS_PROCTYPE_TRAFFICINFO) 요청 데이터
typedef struct tagTrafficInfoInputProcData
{
	NeNddsModuleProcType		eType;				// 모듈 프로세스 종류

	char szRseId[20];
}TRAFFICINFO_INPUT_PROCDATA, *PTRAFFICINFO_INPUT_PROCDATA;

// 교통정보(E_NDDS_PROCTYPE_TRAFFICINFO) 결과 데이터
typedef struct tagTrafficInfoOutputProcData
{
	NeNddsModuleProcType		eType;				// 모듈 프로세스 종류

	// 결과 데이터
	int						m_nCurTrafficInfoRet;
	int nResult;
	char szErrorMsg[256];								// 에러 메세지
}TRAFFICINFO_OUTPUT_PROCDATA, *PTRAFFICINFO_OUTPUT_PROCDATA;

// 인증(E_NDDS_PROCTYPE_AUTH) 요청 데이터
typedef struct tagAuthInputProcData
{
	NeNddsModuleProcType		eType;				// 모듈 프로세스 종류
	char					szOtpCode[7];			// OTP코드
}AUTH_INPUT_PROCDATA, *PAUTH_INPUT_PROCDATA;

// 인증(E_NDDS_PROCTYPE_AUTH) 결과 데이터
typedef struct tagAuthOutputProcData
{
	NeNddsModuleProcType		eType;				// 모듈 프로세스 종류

	// 결과 데이터
	int						m_nCurAuthRet;		//현재 Auth 결과값(UI에 리턴값으로 사용)
	int nResult;
	int nValidateCode;
	int nResultCode;
	char szAccessKey[50];
	char szErrorMsg[256];								// 에러 메세지
}AUTH_OUTPUT_PROCDATA, *PAUTH_OUTPUT_PROCDATA;


// AutoComplete, R-Geocode, ComplexImage, AQI. for vNext. 2019.8.8 by hheo
// NonNdds (E_NDDS_PROCTYPE_NONNDDS) 요청 데이터
typedef struct tagNonNddsInputProcData
{
	NeNddsModuleProcType	eType;				// 모듈 프로세스 종류
	char					szUrl[MAXLEN_URL];
	char					szDomain[MAXLEN_URL];
}NONNDDS_INPUT_PROCDATA, *PNONNDDS_INPUT_PROCDATA;

// AutoComplete, R-Geocode, ComplexImage, AQI. for vNext. 2019.8.8 by hheo
// NonNdds (E_NDDS_PROCTYPE_NONNDDS) 결과 데이터
typedef struct tagNonNddsOutputProcData
{
	NeNddsModuleProcType	eType;				// 모듈 프로세스 종류
	int						nResult;
	char					*pszResult;
	char					szErrorMsg[256];
	bool					bResult;
}NONNDDS_OUTPUT_PROCDATA, *PNONNDDS_OUTPUT_PROCDATA;


//union -> struct 로 변경. 2015.7.16. mi 경로탐색 결과값 변경되는 문제 예외처리
// 모듈 프로세스 요청 데이터
//typedef union tagModuleInputProcData
typedef struct tagModuleInputProcData
{
	NeNddsModuleProcType					eProcType;					// 모듈 프로세스 종류
	int									nRouteSessionID;
	bool								bRouteSync;
	ROUTE_INPUT_PROCDATA				sRouteData;
	int									nSearchSessionID;
	bool								bSearchSync;
	SEARCH_INPUT_PROCDATA				sSearchData;
	int									nFavoriteSessionID;
	bool								bFavoriteSync;
	FAVORITE_INPUT_PROCDATA				sFavoriteData;
	int									nTrafficInfoSessionID;
	bool								bTrafficInfoSync;
	TRAFFICINFO_INPUT_PROCDATA			sTrafficInfoData;
	int									nAuthSessionID;
	bool								bAuthSync;
	AUTH_INPUT_PROCDATA					sAuthData;
    int									nNonNddsSessionID;
    bool								bNonNddsSync;
    NONNDDS_INPUT_PROCDATA				sNonNddsData;
}MODULE_INPUT_PROCDATA, *PMODULE_INPUT_PROCDATA;

// 모듈 프로세스 결과 데이터
//typedef union tagModuleOutputProcData
typedef struct tagModuleOutputProcData
{
	NeNddsModuleProcType					eProcType;					// 모듈 프로세스 종류
	ROUTE_OUTPUT_PROCDATA				sRouteData;
	SEARCH_OUTPUT_PROCDATA				sSearchData;
	FAVORITE_OUTPUT_PROCDATA			sFavoriteData;
	TRAFFICINFO_OUTPUT_PROCDATA			sTrafficInfoData;
	AUTH_OUTPUT_PROCDATA				sAuthData;
    NONNDDS_OUTPUT_PROCDATA             sNonNddsData;
}MODULE_OUTPUT_PROCDATA, *PMODULE_OUTPUT_PROCDATA;

#pragma pack(1)

// Route Collect Result
typedef struct tagNstRouteCollect
{
	int nSearchOption;
	unsigned int nCollectSize;
	unsigned char *pCollectData;
} TsRouteCollect;

typedef struct tagNstTvasFileInfo
{
	int				 nSessionID;
    NeNddsRouteType	 eRouteType;					// 경로탐색 종류
	char			 sTvasPath[3][256];
} TsTvasFileInfo;

#pragma pack()

#define _HTTP_DOWN_LOAD_PATH			"GET http://%s/Tmap/%s HTTP/1.0\r\n\r\n"

typedef std::list<TsTvasFileInfo> TsTvasFileInfoList;

class CNddsModule
{
	NOT_COPYABLE(CNddsModule)
private:
	CNddsModule(void);
	virtual ~CNddsModule(void);

public:
	//////////////////////////////////////////////////////////////////////////
	// 모듈 초기화 및 해제
	//////////////////////////////////////////////////////////////////////////

	// 공용 모듈 객체 생성 & 초기화 (기본)
	bool InitCommon(PNDDSMODULE_INIT pNddsInit);
	// 공용 모듈 해제(스레드 종료 - 최우선 순으로 호출 해야 한다.)
	void ReleaseCommonThread();

	// 프로토콜 모듈 객체 생성 & 초기화
	bool InitProtocol(PNDDSMODULE_INIT pNddsInit);
	// 프로토콜 모듈 해제
	void ReleaseProtocol();
	
	//////////////////////////////////////////////////////////////////////////
	// 공통
	//////////////////////////////////////////////////////////////////////////


	//UI 콜백 메세지를 보낸다.
	bool SendMessgeCallback(int nMsg, int nSessionID, long lParam1=0, long lParam2=0, void* lpParam=NULL);

	// 요청 스레드
#ifdef _LINUX
	static void* NddsModuleProc(void *lParam);
#else
	static int NddsModuleProc(void *lParam);
#endif

	// UI 콜백함수를 설정한다.
	void SetNddsUpdateCallback(LPCALL_NDDSUPDATE_PROC pfnProc);

	// 요청 스레드 관련 (경로탐색, 검색 관련 스레드)
#ifdef _WIN32
	SDL_mutex*					m_pModuleProcMutex;		//스레드 동기화
	SDL_sem*							m_pModuleProcSem;		//요청에 대한 동작중 체크
	SDL_cond*					m_pModuleProcCond;
	SDL_Thread*						m_pModuleProcThread;
	SDL_mutex*					m_pAsyncNddsUpdateMutex;
#else
	pthread_mutex_t					m_pModuleProcMutex;		//스레드 동기화
	sem_t							m_pModuleProcSem;		//요청에 대한 동작중 체크
	pthread_cond_t					m_pModuleProcCond;
	pthread_t						m_pModuleProcThread;
	pthread_mutex_t					m_pAsyncNddsUpdateMutex;
#endif
	bool							m_bEndModuleProcThread;
	
	MODULE_INPUT_PROCDATA				m_sModuleInputProcData;			
	MODULE_OUTPUT_PROCDATA				m_sModuleOutputProcData;			

	int m_nInitFlag;
	LPCALL_NDDSUPDATE_PROC	UpdateNdds;		//NDDS에서 UI로 업데이트 보낼 콜백함수

	bool								m_bIsRoute;

	static int AsyncNddsUpdate(eNddsProtocolType eProtocolType, int nSessionID, int nErrCode);

	void IsChangeAccesskey(int nSessionID, char *szAccessKey);
	void CheckErrorCode(int nSessionID, char *szErrorCode);

	//////////////////////////////////////////////////////////////////////////
	// 경로탐색 모듈 관련
	//////////////////////////////////////////////////////////////////////////

	int							m_nCurRouteRet;			//현재 탐색했던 결과값(UI에 리턴값으로 사용)
	unsigned int				m_nRouteRunState;
	bool						m_bStopServerRoute;		// 서버 경로 탐색 요청 중지

	int							*m_pnPreRids;				//이전경로 RID목록
	int							*m_pnPreSecs;				//이전경로 RID의 소요시간 목록
    char						*m_pszSearchCondData;		//탐색조건 데이터
    char						*m_pszGpsTraceData;         //탐색조건 데이터

	// 경로탐색
	bool API_NDDS_Route(PAPI_ROUTE_REQ pRequest, PAPI_ROUTE_RES pResult);

	// 탐색중인 경로를 취소 한다.
	bool API_NDDS_StopRoute(PAPI_STOPROUTE_REQ pRequest, PAPI_STOPROUTE_RES pResult);
    int RouteFunc(int nSessionID, eNddsRequestType eRequestType, NstRouteReqLogic *pRouteInput, int &nErrCode, NstRouteCollectLogic *pRouteOutput);
	int GetRouteCountByOption(unsigned int uRouteOption, unsigned int *pMultiOption, int *pRouteOption, int *pRouteFee, int *pRouteTraffic);
	bool IsValidePositionByRoutting(int nPosInfoCnt, NstSchPosInfo *pnsPosInfo, bool bStopPosInfoFlag, int *pnErrCode);
    int RouttingServer(int nSessionID, eNddsRequestType eRequestType, NstRouteReqLogic *pRouteInput, NstRouteCollectLogic *pRouteOutput, NstTVASFilesInfo *pFileInfo, char *szErrorMsg,int *nErrorCode);
	bool API_NDDS_GetWeatherInfo(PAPI_WEATHERINFO_REQ pRequest, PAPI_WEATHERINFO_RES pResult);
    int GetRouteResult(int nSessionID, int *nErrorCode, NeNddsRouteType	*eRouteType);
	bool GetRouteSummaryResult(int nSessionID, int *nErrorCode);
	bool API_NDDS_GetRouteSummaryInfo(PAPI_GET_ROUTESUMMARYINFO_REQ pRequest, PAPI_GET_ROUTESUMMARYINFO_RES pResult);

	bool RouteSummaryFunc(int nSessionID, eNddsRequestType eRequestType, NstRouteSummaryReqLogic *pRouteInput, int &nErrCode);
	bool RouttingSummaryServer(int nSessionID, eNddsRequestType eRequestType, NstRouteSummaryReqLogic *pRouteInput, char *szErrorMsg,int *nErrorCode);
    bool API_NDDS_SetTmapHost(const char *pszHostName, const unsigned int nPort);

    //////////////////////////////////////////////////////////////////////////
	// 검색 모듈 관련
	//////////////////////////////////////////////////////////////////////////

	// API : 검색을 수행 한다.
	bool API_NDDS_Search(PAPI_SEARCH_REQ pRequest, PAPI_SEARCH_RES pResult);

	// API : 검색된 결과 리스트를 얻는다.
	bool API_NDDS_GetSearchResultList(PAPI_SEARCH_RESULTLIST_REQ pRequest, PAPI_SEARCH_RESULTLIST_RES pResult);

	// API : POI 상세정보를 얻는다.
	bool API_NDDS_GetSearchPoiDetailInfo(PAPI_SEARCH_POIDETAILINFO_REQ pRequest, PAPI_SEARCH_POIDETAILINFO_RES pResult);

    // API : POI 최저가를 얻는다.
    bool API_NDDS_GetSearchPoiStationInfo(PAPI_SEARCH_POISTATIONINFO_REQ pRequest, PAPI_SEARCH_POISTATIONINFO_RES pResult);

    // API : ev충전소 상세정보를 얻는다.
    bool API_NDDS_GetSearchPoiEvStationInfo(PAPI_SEARCH_POIEVSTATIONINFO_REQ pRequest, PAPI_SEARCH_POIEVSTATIONINFO_RES pResult);

	// API : 가맹점 상세정보를 얻는다.
	bool API_NDDS_GetSearchPoiJoinInfo(PAPI_SEARCH_POIJOININFO_REQ pRequest, PAPI_SEARCH_POIJOININFO_RES pResult);

	// API : POI 최근길 조회.
	bool API_NDDS_GetSearchRecentDestnationList(PAPI_SEARCH_RECENTDESTNATION_REQ pRequest, PAPI_SEARCH_RECENTDESTNATION_RES pResult);

	//////////////////////////////////////////////////////////////////////////
	// 즐겨찾기 모듈 관련
	//////////////////////////////////////////////////////////////////////////

	// API : 즐겨찾기를 수행 한다.
	bool API_NDDS_Favorite(PAPI_FAVORITE_REQ pRequest, PAPI_FAVORITE_RES pResult);

	// API : 즐겨찾기 결과 리스트를 얻는다.
	bool API_NDDS_GetFavoriteResultList(PAPI_FAVORITE_RESULTLIST_REQ pRequest, PAPI_FAVORITE_RESULTLIST_RES pResult);

	// API : 즐겨찾기 집으로/회사로 정보를 얻는다.
	bool API_NDDS_GetMyFavorite(PAPI_MYFAVORITE_REQ pRequest, PAPI_MYFAVORITE_RES pResult);

	// API : 차량 전송 정보 조회 결과를 얻는다.
	bool API_NDDS_GetSendToCarInfo(PAPI_SENDTOCARINFO_REQ pRequest, PAPI_SENDTOCARINFO_RES pResult);

    // API : UserData를 얻는다.
    bool API_NDDS_GetUserData(PAPI_GET_USERDATA_REQ pRequest, PAPI_GET_USERDATA_RES pResult);

	NeNddsFavoriteType	m_eLastFavoriteType;		// 마지막 즐겨찾기 타입

	int FavoriteNddsPoi(int nSessionID, eNddsRequestType eRequestType, bool bMyFavoriteYn, char *szErrorMsg);
    int RegistFavoriteNddsPoi(int nSessionID, eNddsRequestType eRequestType, FAVORITE_NDDSPOI_REGIST_INPUT poiFavorite, char *szErrorMsg);
    int UploadFavoriteNddsPoi(int nSessionID, eNddsRequestType eRequestType, FAVORITE_NDDSPOI_UPLOAD_INPUT poiFavorite, char *szErrorMsg);
    int RemoveFavoriteNddsPoi(int nSessionID, eNddsRequestType eRequestType, DS_REMOVE_POIFAVORITE_INFO *poiFavorite, int nPoiCnt, char *szErrorMsg);
    int modifyFavoriteNddsPoi(int nSessionID, eNddsRequestType eRequestType, DS_MODIFY_POIFAVORITE_INFO poiFavorite, char *szErrorMsg);
    int FavoriteNddsRegisterSendToCarInfo(int nSessionID, eNddsRequestType eRequestType, char *szCustName, stVERTEX32 *pTp, int nPoiId, char rpFlag, char *szNavSeq, char *szErrorMsg);
	int FavoriteNddsFindSendToCarInfo(int nSessionID, eNddsRequestType eRequestType, bool bNewAddrChk, char *szErrorMsg);
	int FavoriteNddsRemoveSendToCarInfo(int nSessionID, eNddsRequestType eRequestType, char *szCustName, stVERTEX32 *pTp, char *szErrorMsg);
    int FavoriteNddsFindUserProfile(int nSessionID, eNddsRequestType eRequestType, char *szErrorMsg);
    int FavoriteNddsRegistUserProfile(int nSessionID, eNddsRequestType eRequestType, FAVORITE_NDDSREGISTUSERPROFILE_INPUT inputData, char *szErrorMsg);
    int FavoriteNddsFindCarProfile(int nSessionID, eNddsRequestType eRequestType, char *szErrorMsg);
    int FavoriteNddsRegistCarProfile(int nSessionID, eNddsRequestType eRequestType, FAVORITE_NDDSREGISTCARPROFILE_INPUT inputData, char *szErrorMsg);
    int FavoriteNddsFindUserData(int nSessionID, eNddsRequestType eRequestType, FAVORITE_NDDSFINDUSERDATA_INPUT inputData, char *szErrorMsg);

	//////////////////////////////////////////////////////////////////////////
	// 교통정보
	//////////////////////////////////////////////////////////////////////////
	// API : VMS 가이드 정보 조회
	bool API_NDDS_VmsGuideInfo(PAPI_VMSGUIDEINFO_REQ pRequest, PAPI_VMSGUIDEINFO_RES pResult);

	// API : VMS 가이드 정보 조회
	bool API_NDDS_GetVmsGuideInfo(PAPI_GET_VMSGUIDEINFO_REQ pRequest, PAPI_GET_VMSGUIDEINFO_RES pResult);

	//////////////////////////////////////////////////////////////////////////
	// 인증
	//////////////////////////////////////////////////////////////////////////
	// API : 인증
	bool API_NDDS_AuthTmapForSmartCar(PAPI_AUTHTMAPFORSMARTCAR_REQ pRequest, PAPI_AUTHTMAPFORSMARTCAR_RES pResult);
	
	// API : 인증 결과 조회
	bool API_NDDS_GetAuthResult(PAPI_GET_AUTHRESULT_REQ pRequest, PAPI_GET_AUTHRESULT_RES pResult);

	// API : AccessKey 변경
	bool API_NDDS_SetAuthAccessKey(PAPI_SET_AUTHACCESSKEY_REQ pRequest, PAPI_SET_AUTHACCESSKEY_RES pResult);

    bool API_NDDS_SetReqDataSave(PAPI_SETREQDATASAVE_REQ pRequest, PAPI_SETREQDATASAVE_RES pResult);

    bool API_NDDS_NonNdds(PAPI_NONNDDS_REQ pRequest, PAPI_NONNDDS_RES pResult);

	//////////////////////////////////////////////////////////////////////////
	// Ndds 관련
	//////////////////////////////////////////////////////////////////////////

	NstNddsWeatherInfo			m_pNddsWeatherInfo;
	NstNddsPoiResult			m_pNddsPoiResult[200];
	NstNddsPoiDetailInfo		m_pNddsPoiDetailInfo;
    NstNddsPoiStationInfo		m_pNddsPoiStationInfo;
    NstNddsPoiEvStationInfo		m_pNddsPoiEvStationInfo;
    NstNddsPoiEvStationsByPoint		m_pNddsPoiEvStationsByPoint;
	NstNddsPoiJoinInfo			m_pNddsPoiJoinInfo[8];
	NstNddsRecentDestnation		m_pNddsRecentDestnation[250];
	NstNddsAuthTmapForSmartCar  m_pNddsAuthTmapForSmartCar;
	NstNddsVmsGuideInfo			m_pNddsVmsGuideInfo;
	NstNddsMyFavorite			m_pNddsMyFavorite;
	NstNddsFavoriteResult		m_pNddsFavoriteResult[250];
	NstNddsFavoriteSendToCarInfo m_pNddsFavoriteSendToCarInfo;
    NstNddsUserData				m_pNddsUserData;
    int							m_nReadFavCnt;
	int							m_nAuthCnt;
	int							m_nTrafficInfoCnt;
	int							m_nReadDetailInfoCnt;
	int							m_nReadJoinInfoCnt;
	int							m_nReadRecentCnt;
	int							m_nReadCnt;
    NeNddsSearchType	m_eLastSearchType;		// 마지막 검색 타입

	NstNddsRouteSummaryResult	m_pNddsRouteSummaryResult[3];
	int						m_nRouteSummaryIdx;

	bool m_bNewAddrChk;
	bool m_bNewAddrChkForS2C;
	NeNddsOilType m_eOilType;
	bool m_bMyFavoriteYn;

    char                        *m_pNonNddsResult;

	char m_szAccessKey[50];

	// Ndds Tmap POI 검색
	int SearchNddsPoi(int nSessionID, eNddsRequestType eRequestType, char *szSearchName, char *szAddrName, NeNddsSearchSort  eSortType, stVERTEX32 *pwpCurrent, int nDist, NeNddsOilType eOilType, bool bNewAddrChk, char *szErrorMsg);
	// Ndds Tmap 주변지 검색
	int SearchNddsAroundPoi(int nSessionID, eNddsRequestType eRequestType, char *szSearchName, stVERTEX32 *pwpCurrent, int nDist, bool bNewAddrChk, char *szErrorMsg);
	// Ndds Tmap POI 상세정보 검색
	int SearchNddsPoiDetailInfo(int nSessionID, eNddsRequestType eRequestType, int poiId, char *navSeq, int sizeWidth, int sizeHeight, char *szErrorMsg);
	// Ndds Tmap 가맹점 상세정보 검색
	int SearchNddsPoiJoinInfo(int nSessionID, eNddsRequestType eRequestType, int poiId, char *szErrorMsg);
	// Ndds Tmap POI 최근길 조회
	int SearchNddsRecentDestnation(int nSessionID, eNddsRequestType eRequestType, char *szErrorMsg);
    // Ndds Tmap POI 최근길 전체 업로드
    int SearchNddsUploadRecentDestnation(int nSessionID, eNddsRequestType eRequestType, N_SEARCH_UPLOADRECENTDESTNATION *pUploadRecentPoi, int nUploadRecentCnt, char* szSvcUploadYn, char *szErrorMsg);
    // Ndds Tmap POI 최근길 멀티삭제
    int SearchNddsRemoveRecentDestnation(int nSessionID, eNddsRequestType eRequestType, N_SEARCH_REMOVERECENTDESTNATION *pUploadRecentPoi, int nUploadRecentCnt, char *szErrorMsg);
    // Ndds Tmap POI 지역분류별 주소명 조회
	int SearchNddsAreaNamesByStep(int nSessionID, eNddsRequestType eRequestType, char *szAddrName1, char *szAddrName2, bool bNewAddrChk, char *szErrorMsg);
    // Ndds Tmap 최저가 주유소(충전소) 조회
    int SearchNddsPoiStationInfo(int nSessionID, eNddsRequestType eRequestType, stVERTEX32 *pwpCurrent, int nDist, char *szDataKind, char *szFuelType, char *szErrorMsg);
    // Ndds Tmap ev충전소 상세검색 조회
    int SearchNddsPoiEvStationInfo(int nSessionID, eNddsRequestType eRequestType, int nPoiID, char *szErrorMsg);
    // Ndds Tmap ev충전소 상세검색 조회
    int SearchNddsPoiEvStationsByPoint(int nSessionID, eNddsRequestType eRequestType, stVERTEX32 *pwpCurrent, int nDist, char *szErrorMsg);


	// Ndds VMS 가이드 정보 조회
	int NddsVmsGuideInfo(int nSessionID, eNddsRequestType eRequestType, char *szRseId, char *szErrorMsg);

	// Ndds Tmap 단말인증(for Smart Car)
    int NddsAuthforSmartCar(int nSessionID, eNddsRequestType eRequestType, char *szOtpCode, int &nValidateCode, int &nResultCode, char *szAccessKey, int &nErrCode, char *szErrorMsg);

	int NonNddsRequest(int nSessionID, eNddsRequestType eRequestType, char *pszUrl, char *pszDomain, char *pszResult, char *szErrorMsg, bool &bResult);

	int GetNddsPoiResult(int nSessionID, bool bNewAddrChk, NeNddsOilType eOilType, int *nErrorCode);
	int GetNddsAroundPoiResult(int nSessionID, bool bNewAddrChk, int *nErrorCode);
	int GetNddsPoiDetailInfoResult(int nSessionID, int *nErrorCode);
    int GetNddsPoiStationInfoResult(int nSessionID, int *nErrorCode);

    // Ndds Tmap Ev충전소 상세조회
    int GetNddsPoiEvStationInfoResult(int nSessionID, int *nErrorCode);
    int GetNddsPoiEvStationsByPointResult(int nSessionID, int *nErrorCode);
	int GetNddsPoiJoinInfoResult(int nSessionID, int *nErrorCode);
	int GetNddsRecentDestnationResult(int nSessionID, int *nErrorCode);
	int GetNddsAreaNamesByStepResult(int nSessionID, int *nErrorCode);
    // Ndds Tmap UserData 조회 (최근길/즐겨찾기)
    int GetNddsUserDataResult(int nSessionID, int *nErrorCode);

	int GetNddsPoiFavoriteResult(int nSessionID, int *nErrorCode);
	int GetNddsRegisterSendToCarInfoResult(int nSessionID, int *nErrorCode);
	int GetNddsFindSendToCarInfoResult(int nSessionID, bool bNewAddrChk, int *nErrorCode);
	int GetNddsRemoveSendToCarInfoResult(int nSessionID, int *nErrorCode);
	
	int GetNddsVmsGuideInfoResult(int nSessionID, int *nErrorCode);

    int GetNddsAuthforSmartCarResult(int nSessionID, int &nValidateCode, int &nResultCode, char *szAccessKey, int *nErrorCode);

	int GetNonNddsResult(int nSessionID, char *pszResult, bool &bResult);

private:
	int						m_nLastIdxSession;
	TsTvasFileInfoList		m_listTvasInfo;

	DECLARE_SINGLETON(CNddsModule)

};

#endif
