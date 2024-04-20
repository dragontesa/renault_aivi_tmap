// NddsModule.cpp

#include "NddsModule.h"
#include "ndds_inc.h"
#include <string.h>
#include <wchar.h>
#include <math.h>
#define MaxNddsSchResultBufCnt 200
#define WORLDCOORD_PER_DEG	(256 * 2048.0) // WGS84 좌표에 WORLDCOORD_PER_DEG를 곱하면 World좌표가 된다.
#define BYTEALIGN_SIZE(size)	(((size)%4 == 0)?(size):((size)+4-(size)%4))
#define MaxRouteSummaryCnt	3
// move to common_def.h
//#define SEARCH_WAITTING_TIME 10000
//#define RP_WAITTING_TIME 10000

bool NddsErrorCheck(eNddsErrorCode eErrCode, char *szErrorMsg, char *szServerMsg)
{
	if(eErrCode != NDDSERR_SUCCESS)
	{
		switch(eErrCode)
		{
		case NDDSERR_RES_FAIL:
			break;
		case NDDSERR_USERCANCEL:
			strcpy(szServerMsg, "사용자 취소");
			break;
		default:
			strcpy(szServerMsg, "서버 접속 실패");
			return false;
			break;
		}

		if(szErrorMsg)
		{
			strcpy(szServerMsg, szErrorMsg);
			return false;
		}
		return false;
	}
	return true;
}

CNddsModule *g_this_nddsmodule = NULL;

IMPLEMENT_SINGLETON(CNddsModule)

CNddsModule::CNddsModule(void)
{
	g_this_nddsmodule = this;

	m_nInitFlag = 0;

	UpdateNdds = NULL;

	// 경로탐색 관련 데이터
	//m_bIsRoute = false;
	m_pnPreRids = NULL;				//이전경로 RID목록
	m_pnPreSecs = NULL;				//이전경로 RID의 소요시간 목록
    m_pszSearchCondData = NULL;
    m_pszGpsTraceData = NULL;
	m_bStopServerRoute = false;
	m_nRouteSummaryIdx = 0;
	memset(m_pNddsRouteSummaryResult, 0x00, sizeof(m_pNddsRouteSummaryResult));

	// 검색 관련 데이터
	memset(m_pNddsPoiResult, 0x00, sizeof(m_pNddsPoiResult));
	memset(&m_pNddsPoiDetailInfo, 0x00, sizeof(NstNddsPoiDetailInfo));
	memset(&m_pNddsPoiJoinInfo, 0x00, sizeof(NstNddsPoiJoinInfo)*8);
	memset(&m_pNddsRecentDestnation, 0x00, sizeof(m_pNddsRecentDestnation));
	memset(&m_pNddsAuthTmapForSmartCar, 0x00, sizeof(m_pNddsAuthTmapForSmartCar));
	memset(&m_pNddsVmsGuideInfo, 0x00, sizeof(m_pNddsVmsGuideInfo));
	memset(&m_pNddsFavoriteResult, 0x00, sizeof(m_pNddsFavoriteResult));
	memset(&m_pNddsMyFavorite, 0x00, sizeof(m_pNddsMyFavorite));
	memset(&m_pNddsFavoriteSendToCarInfo, 0x00, sizeof(m_pNddsFavoriteSendToCarInfo));
    memset(&m_pNddsUserData, 0x00, sizeof(m_pNddsUserData));
	m_bNewAddrChk = false;
	m_eOilType = N_eOilTypeGasoline;
	m_bMyFavoriteYn = false;
	m_bNewAddrChkForS2C = false;
    m_pNonNddsResult = nullptr;

	memset(m_szAccessKey, 0x00, sizeof(m_szAccessKey));

	m_bIsRoute = false;
#ifdef _WIN32
	m_pModuleProcMutex = NULL;
	m_pModuleProcSem = NULL;
	m_pModuleProcCond = NULL;
	m_pModuleProcThread = NULL;
	m_pAsyncNddsUpdateMutex = NULL;
#else
	m_pModuleProcMutex = PTHREAD_MUTEX_INITIALIZER;
	m_pModuleProcThread = NULL;
	m_pAsyncNddsUpdateMutex = PTHREAD_MUTEX_INITIALIZER;
#endif
	m_bEndModuleProcThread = false;
	m_nLastIdxSession = 0;
}

CNddsModule::~CNddsModule(void)
{
	if(m_pnPreRids != NULL)
	{
		free(m_pnPreRids);
	}

	if(m_pnPreSecs != NULL)
	{
		free(m_pnPreSecs);
	}

    if(m_pszSearchCondData != NULL)
    {
        free(m_pszSearchCondData);
    }

    if(m_pszGpsTraceData != NULL)
    {
        free(m_pszGpsTraceData);
    }
}

//////////////////////////////////////
// 모듈 초기화 및 해제
//////////////////////////////////////

bool CNddsModule::InitCommon(PNDDSMODULE_INIT pNddsInit)
{
	// 초기화 플래그를 유지한다.
	UpdateNdds = pNddsInit->pfnNddsUpdate;

	// 모듈 프로세스 스레드 초기화
#ifdef _LINUX
	if (pthread_mutex_init(&m_pModuleProcMutex, NULL) != 0)
	{
		DBGPRINT("\n mutex init failed\n");
	}

	int state = sem_init(&m_pModuleProcSem,0,1);
	if(state != 0)
		return false;

	pthread_cond_init(&m_pModuleProcCond, NULL);

	int thr_id;
	thr_id = pthread_create(&m_pModuleProcThread, NULL, &NddsModuleProc, this);
	pthread_setname_np(m_pModuleProcThread, "skt.tmap.ndds.proc");
	if(m_pModuleProcThread == NULL)
	{
		//assert(0);
		return false;
	}

	if (pthread_mutex_init(&m_pAsyncNddsUpdateMutex, NULL) != 0)
	{
		DBGPRINT("\n mutex init failed2\n");
	}
#else
	m_pModuleProcMutex = SDL_CreateMutex();
	m_pModuleProcSem = SDL_CreateSemaphore(1);
	m_pModuleProcCond = SDL_CreateCond();

	m_pModuleProcThread = SDL_CreateThread(NddsModuleProc, "NddsModuleProc", this);
	if(m_pModuleProcThread == NULL)
	{
		return FALSE;
	}

	m_pAsyncNddsUpdateMutex = SDL_CreateMutex();

	unsigned short		wVersion = MAKEWORD(2,2);
	WSADATA		wsaData;

	if(WSAStartup(wVersion,&wsaData) != 0)
	{
		return FALSE;
	}
#endif // _WIN32

	return true;
}

void CNddsModule::ReleaseCommonThread()
{
	//스레드 종료할수 있도록 프래그 설정
	m_bEndModuleProcThread = true;

	//메인 메세지큐의 메세지를 처리하는 스레드를 종료한다.
#ifdef _LINUX
	if(m_pModuleProcThread)
	{
		//경로 탐색 중지 플래그 설정한다.

		//검색 중지 플래그 설정한다. (추후)

		//스레드 CondWait에서 빠져 나오도록 시그널 발생
		pthread_mutex_lock(&m_pModuleProcMutex);
		pthread_cond_signal(&m_pModuleProcCond);
		pthread_mutex_unlock(&m_pModuleProcMutex);

		//스레드가 종료 될때 까지 대기
        void* status = 0;
        pthread_join(m_pModuleProcThread, &status);
		m_pModuleProcThread = NULL;
	}

	//if(m_pModuleProcCond)
	{
		pthread_cond_destroy(&m_pModuleProcCond);
	}
	//if(m_pModuleProcSem)
	{
		sem_destroy(&m_pModuleProcSem);
	}
	//if(m_pModuleProcMutex)
	{
		pthread_mutex_destroy(&m_pModuleProcMutex);
		//m_pModuleProcMutex = NULL;
	}
	//if(m_pAsyncNddsUpdateMutex)
	{
		pthread_mutex_destroy(&m_pAsyncNddsUpdateMutex);
		//m_pAsyncNddsUpdateMutex = NULL;
	}
#else
	if(m_pModuleProcThread)
	{
		//검색 중지 플래그 설정한다. (추후)

		//스레드 CondWait에서 빠져 나오도록 시그널 발생
		SDL_LockMutex(m_pModuleProcMutex);
		SDL_CondSignal(m_pModuleProcCond);
		SDL_UnlockMutex(m_pModuleProcMutex);

		//스레드가 종료 될때 까지 대기
		SDL_WaitThread(m_pModuleProcThread, NULL);
		m_pModuleProcThread = NULL;
	}

	if(m_pModuleProcCond)
	{
		SDL_DestroyCond(m_pModuleProcCond);
		m_pModuleProcCond = NULL;
	}
	if(m_pModuleProcSem)
	{
		SDL_DestroySemaphore(m_pModuleProcSem);
		m_pModuleProcSem = NULL;
	}
	if(m_pModuleProcMutex)
	{
		SDL_DestroyMutex(m_pModuleProcMutex);
		m_pModuleProcMutex = NULL;
	}
	if(m_pAsyncNddsUpdateMutex)
	{
		SDL_DestroyMutex(m_pAsyncNddsUpdateMutex);
		m_pAsyncNddsUpdateMutex = NULL;
	}
#endif
}

bool CNddsModule::InitProtocol(PNDDSMODULE_INIT pNddsInit)
{
	NDDSTMAP_INIT_INFO sNddsTmapInit;
	memset(&sNddsTmapInit, 0x00, sizeof(NDDSTMAP_INIT_INFO));

    if(strlen(pNddsInit->szServerURL) > 0)
    {
        sNddsTmapInit.nddsServerType = NDDSSERVER_NONE;
        strcpy(sNddsTmapInit.szServerURL, pNddsInit->szServerURL);
        sNddsTmapInit.nServerPort = pNddsInit->nServerPort;
    }
    else
    {
        if(pNddsInit->bServerCommerce)
            sNddsTmapInit.nddsServerType = NDDSSERVER_COMMERCE;
        else
            sNddsTmapInit.nddsServerType = NDDSSERVER_DEVELOPMENT;
    }

	// 		if(*pNddsInit->szMdn)
	// 			strcpy(sNddsTmapInit.mdn, pNddsInit->szMdn);
	if(*pNddsInit->szMacAddress)
		strcpy(sNddsTmapInit.macAddress, pNddsInit->szMacAddress);

	switch(pNddsInit->eNdds_OsType)
	{
	case eNdds_OsType_Android:	strcpy(sNddsTmapInit.osType, "AND");	break;
	case eNdds_OsType_IOS:		strcpy(sNddsTmapInit.osType, "IOS");	break;
	case eNdds_OsType_Windows:
	case eNdds_OsType_WinCE:
	case eNdds_OsType_WinMobile:
		strcpy(sNddsTmapInit.osType, "WIN");	break;
	default:
		strcpy(sNddsTmapInit.osType, "ETC");	break;
	}

	strcpy(sNddsTmapInit.osVersion, pNddsInit->szOSVersion);

	strcpy(sNddsTmapInit.appVersion, pNddsInit->szAppVersion);
	strcpy(sNddsTmapInit.modelNo, pNddsInit->szModelName);
	strcpy(sNddsTmapInit.buildNo, pNddsInit->szAppBuildNo);

#if 0
	if(pNddsInit->nWidth <= 480)
		strcpy(sNddsTmapInit.resolution, "HVGA");
	else if(pNddsInit->nWidth <= 800)
		strcpy(sNddsTmapInit.resolution, "WVGA");
	else
	{
		if(pNddsInit->nHeight <= 600)
			strcpy(sNddsTmapInit.resolution, "WSVGA");
		else
			strcpy(sNddsTmapInit.resolution, "XGA");
	}
#endif

	strcpy(sNddsTmapInit.resolution, "WVGA");
	strcpy(sNddsTmapInit.carrier, "SKTelecom");
	strcpy(sNddsTmapInit.usingInfo, "MAIN");
	sNddsTmapInit.width = 800;//pNddsInit->nWidth;
	sNddsTmapInit.height = 480;//pNddsInit->nHeight;
	strcpy(sNddsTmapInit.deviceId, pNddsInit->szDeviceID);

	strcpy(sNddsTmapInit.deviceIdHu, pNddsInit->szDeviceIDHu);
	strcpy(sNddsTmapInit.deviceModleName, pNddsInit->szDeviceModleName);
	strcpy(sNddsTmapInit.deviceModelNo, pNddsInit->szDeviceModelNo);
	strcpy(sNddsTmapInit.vendor, pNddsInit->szVendor);
	strcpy(sNddsTmapInit.serialKey, pNddsInit->szSerialKey);
	
	strcpy(sNddsTmapInit.accessKey, pNddsInit->szAccessKey);
	strcpy(m_szAccessKey, pNddsInit->szAccessKey);

	if(pNddsInit->eNdds_CharSet == eNDDS_CHARACTERSET_UTF8)
		NDDSLIB_SetInit(NDDSCHARACTERSET_UTF8, &sNddsTmapInit);
	else
		NDDSLIB_SetInit(NDDSCHARACTERSET_EUCKR, &sNddsTmapInit);

	NDDSLIB_SetAyncNddsUpdate(AsyncNddsUpdate);

	return true;
}

void CNddsModule::ReleaseProtocol()
{
	NDDSLIB_Release();
}
//////////////////////////////////////
// 공통
//////////////////////////////////////

bool CNddsModule::SendMessgeCallback(int nMsg, int nSessionID, long lParam1, long lParam2, void* lpParam)
{
	if(UpdateNdds)
	{
		return UpdateNdds(nMsg, nSessionID, lParam1, lParam2, lpParam);
	}
	return false;
}

#ifdef _LINUX
void* CNddsModule::NddsModuleProc(void *lParam)
#else
int CNddsModule::NddsModuleProc(void *lParam)
#endif
{
	CNddsModule *pNddsModule = (CNddsModule*)lParam;

	NeNddsModuleProcType			&eProcType = pNddsModule->m_sModuleInputProcData.eProcType;
	int								&nRouteSessionID = pNddsModule->m_sModuleInputProcData.nRouteSessionID;
	bool							&bRouteSync	= pNddsModule->m_sModuleInputProcData.bRouteSync;
	ROUTE_INPUT_PROCDATA			&sRouteInputData = pNddsModule->m_sModuleInputProcData.sRouteData;
	ROUTE_OUTPUT_PROCDATA			&sRouteOutputData = pNddsModule->m_sModuleOutputProcData.sRouteData;
	int								&nSearchSessionID = pNddsModule->m_sModuleInputProcData.nSearchSessionID;
	bool							&bSearchSync = pNddsModule->m_sModuleInputProcData.bSearchSync;
	SEARCH_INPUT_PROCDATA			&sSearchInputData = pNddsModule->m_sModuleInputProcData.sSearchData;
	SEARCH_OUTPUT_PROCDATA			&sSearchOutputData = pNddsModule->m_sModuleOutputProcData.sSearchData;
	int								&nFavoriteSessionID = pNddsModule->m_sModuleInputProcData.nFavoriteSessionID;
	bool							&bFavoriteSync = pNddsModule->m_sModuleInputProcData.bFavoriteSync;
	FAVORITE_INPUT_PROCDATA			&sFavoriteInputData = pNddsModule->m_sModuleInputProcData.sFavoriteData;
	FAVORITE_OUTPUT_PROCDATA		&sFavoriteOutputData = pNddsModule->m_sModuleOutputProcData.sFavoriteData;
	int								&nTrafficInfoSessionID = pNddsModule->m_sModuleInputProcData.nTrafficInfoSessionID;
	bool							&bTrafficInfoSync = pNddsModule->m_sModuleInputProcData.bTrafficInfoSync;
	TRAFFICINFO_INPUT_PROCDATA		&sTrafficInfoInputData = pNddsModule->m_sModuleInputProcData.sTrafficInfoData;
	TRAFFICINFO_OUTPUT_PROCDATA		&sTrafficInfoOutputData = pNddsModule->m_sModuleOutputProcData.sTrafficInfoData;
	int								&nAuthSessionID = pNddsModule->m_sModuleInputProcData.nAuthSessionID;
	bool							&bAuthSync = pNddsModule->m_sModuleInputProcData.bAuthSync;
	AUTH_INPUT_PROCDATA				&sAuthInputData = pNddsModule->m_sModuleInputProcData.sAuthData;
	AUTH_OUTPUT_PROCDATA			&sAuthOutputData = pNddsModule->m_sModuleOutputProcData.sAuthData;
    int								&nNonNddsSessionID = pNddsModule->m_sModuleInputProcData.nNonNddsSessionID;
    bool							&bNonNddsSync = pNddsModule->m_sModuleInputProcData.bNonNddsSync;
    NONNDDS_INPUT_PROCDATA			&sNonNddsInputData = pNddsModule->m_sModuleInputProcData.sNonNddsData;
    NONNDDS_OUTPUT_PROCDATA			&sNonNddsOutputData = pNddsModule->m_sModuleOutputProcData.sNonNddsData;

	SEARCH_NDDSPOI_INPUT			&sSearchNddsPoi = sSearchInputData.sSearchInput.sSearchNddsPoi;
	SEARCH_NDDSAROUNDPOI_INPUT		&sSearchNddsAroundPoi = sSearchInputData.sSearchInput.sSearchNddsAroundPoi;
	SEARCH_NDDSPOIDETAILINFO_INPUT	&sSearchNddsPoiDetailInfo = sSearchInputData.sSearchInput.sSearchNddsPoiDetailInfo;
	SEARCH_NDDSPOIJOININFO_INPUT	&sSearchNddsPoiJoinInfo = sSearchInputData.sSearchInput.sSearchNddsPoiJoinInfo;
	SEARCH_NDDSAREANAMESBYSTEP_INPUT &sSearchNddsAreaNamesByStep = sSearchInputData.sSearchInput.sSearchNddsAreaNamesByStep;
    SEARCH_NDDSPOISTATIONINFO_INPUT &sSearchNddsPoiStationInfo = sSearchInputData.sSearchInput.sSearchNddsPoiStationInfo;
    SEARCH_NDDSPOIEVSTATIONINFO_INPUT &sSearchNddsPoiEvStationInfo = sSearchInputData.sSearchInput.sSearchNddsPoiEvStationInfo;
    SEARCH_NDDSPOIEVSTATIONSBYPOINT_INPUT &sSearchNddsPoiEvStationsByPoint = sSearchInputData.sSearchInput.sSearchNddsPoiEvStationsByPoint;

	FAVORITE_NDDSPOI_INPUT			&sFavoriteNddsPoi = sFavoriteInputData.sFavoriteInput.sFavoriteNddsPoi;
    SEARCH_NDDSUPLOADRECENT_INPUT	&sSearchNddsUploadRecentPoi = sSearchInputData.sSearchInput.sSearchNddsUploadRecentInfo;
    SEARCH_NDDSREMOVERECENT_INPUT	&sSearchNddsRemoveRecentPoi = sSearchInputData.sSearchInput.sSearchNddsRemoveRecentInfo;
    FAVORITE_NDDSREGISTERSENDTOCARINFO_INPUT		&sFavoriteNddsRegisterSendToCarInfo = sFavoriteInputData.sFavoriteInput.sFavoriteNddsRegiSendToCarInfo;
	FAVORITE_NDDSFINDSENDTOCARINFO_INPUT			&sFavoriteNddsFindSendToCarInfo = sFavoriteInputData.sFavoriteInput.sFavoriteNddsFindSendToCarInfo;
	FAVORITE_NDDSREMOVESENDTOCARINFO_INPUT			&sFavoriteNddsRemoveSendToCarInfo = sFavoriteInputData.sFavoriteInput.sFavoriteNddsRemoveSendToCarInfo;
	
	eNddsRequestType eRequestType = NDDSREQUEST_SERIALIZE;

	//DBGPRINT(_T("MUTEX[%d] - NddsModuleProc::LockMutex #1\n"), GetTickCount(), GetTickCount());
#ifdef _LINUX
	pthread_mutex_lock(&pNddsModule->m_pModuleProcMutex);
#else
	SDL_LockMutex(pNddsModule->m_pModuleProcMutex);
#endif
	//DBGPRINT(_T("MUTEX[%d] - NddsModuleProc::LockMutex #2\n"), GetTickCount(), GetTickCount());

	while(1)
	{
		// 스레드 종료 설정되어 있으면 루프를 종료 한다.
		if(pNddsModule->m_bEndModuleProcThread)
			break;

		// 요청이 올때까지 대기한다.
		//DBGPRINT(_T("MUTEX[%d] - NddsModuleProc::CondWait #1\n"), GetTickCount(), GetTickCount());
#ifdef _LINUX
		pthread_cond_wait(&pNddsModule->m_pModuleProcCond, &pNddsModule->m_pModuleProcMutex);
#else
		SDL_CondWait(pNddsModule->m_pModuleProcCond, pNddsModule->m_pModuleProcMutex);
#endif
		//DBGPRINT(_T("MUTEX[%d] - NddsModuleProc::CondWait #2\n"), GetTickCount(), GetTickCount());

		// 스레드 종료 설정되어 있으면 루프를 종료 한다.
		if(pNddsModule->m_bEndModuleProcThread)
			break;

		//DBGPRINT(_T("############################# NddsModuleProc #1\n"));

		NddsExternalLog(E_DEBUG, "nddsmodule :: eProcType = %d", eProcType);

		switch(eProcType)
		{
		case E_NDDS_PROCTYPE_ROUTE:			//경로 탐색
			{
				pNddsModule->m_bIsRoute = true;

				//DBGPRINT(_T("############################# E_PROCTYPE_ROUTE [NM_NDDS_ROUTE_START SEND] #1\n"));

				NddsExternalLog(E_DEBUG, "nddsmodule :: SendMessageCallback NM_NDDS_ROUTE_START before nRouteSessionID = %d, eRouteType = %d", nRouteSessionID, (long)sRouteInputData.sRouteInput.eRouteType);
				while (pNddsModule->SendMessgeCallback(NM_NDDS_ROUTE_START, nRouteSessionID, (long)(sRouteInputData.sRouteInput.eRouteType)) != true)
				{
					//NM_NDDS_ROUTE_START 메세지는 반드시 성공 해야 한다.
					delay(10);
				}
				NddsExternalLog(E_DEBUG, "nddsmodule :: SendMessageCallback NM_NDDS_ROUTE_START after");

				//DBGPRINT(_T("############################# E_PROCTYPE_ROUTE [NM_NDDS_ROUTE_START] #1\n"));

				if( sRouteInputData.sRouteInput.sRouteInput.m_bReRoute != true )
					pNddsModule->m_nRouteRunState = NM_NDDS_ROUTE_START;
				
				if(bRouteSync)
				{
					eRequestType = NDDSREQUEST_SERIALIZE;
				}
				else
				{
					eRequestType = NDDSREQUEST_ATONCE;
				}

				switch(sRouteInputData.sRouteInput.eRouteType)
				{
				case N_ROUTETYPE_ADD:
				case N_ROUTETYPE_RE_OPTION:
				case N_ROUTETYPE_RE_OUTROUTE:
				case N_ROUTETYPE_TPEG_RE_ROUTE:
				case N_ROUTETYPE_PATTERN_ARRIVAL:
				case N_ROUTETYPE_PATTERN_DEPARTURE:
				case N_ROUTETYPE_NORMAL:
					{
						NddsExternalLog(E_DEBUG, "nddsmodule :: RouteFunc before :: nRouteSessionID = %d, eRequestType = %d, m_nCurRouteRet = %d", nRouteSessionID, eRequestType, sRouteOutputData.m_nCurRouteRet);
						sRouteOutputData.nResult = pNddsModule->RouteFunc(nRouteSessionID, eRequestType, &sRouteInputData.sRouteInput.sRouteInput, sRouteOutputData.m_nCurRouteRet, &sRouteOutputData.sRouteOutput);
						NddsExternalLog(E_DEBUG, "nddsmodule :: RouteFunc after :: nRouteSessionID = %d, eRequestType = %d, m_nCurRouteRet = %d, nResult = %d", nRouteSessionID, eRequestType, sRouteOutputData.m_nCurRouteRet, sRouteOutputData.nResult);
					}
					break;
				case N_ROUTETYPE_SUMMARY:
					{
						sRouteOutputData.nResult = pNddsModule->RouteSummaryFunc(nRouteSessionID, eRequestType, &sRouteInputData.sRouteInput.sRouteSummaryInput, sRouteOutputData.m_nCurRouteRet);
					}
					break;
				default:
					break;
				}
				
				if(eRequestType == NDDSREQUEST_SERIALIZE)
				{
					pNddsModule->m_nRouteRunState = NM_NDDS_ROUTE_END;

					//DBGPRINT(_T("############################# E_PROCTYPE_ROUTE [NM_NDDS_ROUTE_END SEND] #1\n"));

					NddsExternalLog(E_DEBUG, "nddsmodule :: SendMessgeCallback NM_NDDS_ROUTE_END before :: nRouteSessionID = %d, eRouteType = %d, m_nCurRouteRet = %d", nRouteSessionID, (long)(sRouteInputData.sRouteInput.eRouteType), sRouteOutputData.m_nCurRouteRet);
					while(pNddsModule->SendMessgeCallback(NM_NDDS_ROUTE_END, nRouteSessionID, (long)(sRouteInputData.sRouteInput.eRouteType), sRouteOutputData.nResult, &sRouteOutputData.m_nCurRouteRet) != true)
					{
						//NM_NDDS_ROUTE_END 메세지는 반드시 성공 해야 한다.
						// 메세지가 누락될 경우 단말이 멈출 수 있다.
						delay(10);
					}
					NddsExternalLog(E_DEBUG, "nddsmodule :: SendMessgeCallback NM_NDDS_ROUTE_END after");
					//DBGPRINT(_T("############################# E_PROCTYPE_ROUTE [NM_NDDS_ROUTE_END] #1\n"));

					pNddsModule->m_nRouteRunState = NM_NDDS_ROUTE_DATA;

					//DBGPRINT(_T("############################# E_PROCTYPE_ROUTE [NM_NDDS_ROUTE_DATA SEND] #1\n"));
                    NddsExternalLog(E_DEBUG, "nddsmodule :: SendMessgeCallback NM_NDDS_ROUTE_DATA before :: nRouteSessionID = %d, eRouteType = %d, szSessionID = %s", nRouteSessionID, (long)(sRouteInputData.sRouteInput.eRouteType), sRouteOutputData.sRouteOutput.szSessionID);
                    while(pNddsModule->SendMessgeCallback(NM_NDDS_ROUTE_DATA, nRouteSessionID, (long)(sRouteInputData.sRouteInput.eRouteType), NULL, sRouteOutputData.sRouteOutput.szSessionID) != true)
					{					
						delay(10);
					}
					NddsExternalLog(E_DEBUG, "nddsmodule :: SendMessgeCallback NM_NDDS_ROUTE_DATA after");
					//DBGPRINT(_T("############################# E_PROCTYPE_ROUTE [NM_NDDS_ROUTE_DATA] #1\n"));

					if(sRouteOutputData.sRouteOutput.pCollectData[0] != NULL)
					{
						free(sRouteOutputData.sRouteOutput.pCollectData[0]);
						sRouteOutputData.sRouteOutput.pCollectData[0] = NULL;
					}
					if(sRouteOutputData.sRouteOutput.pCollectData[1] != NULL)
					{
						free(sRouteOutputData.sRouteOutput.pCollectData[1]);
						sRouteOutputData.sRouteOutput.pCollectData[1] = NULL;
					}
					if(sRouteOutputData.sRouteOutput.pCollectData[2] != NULL)
					{
						free(sRouteOutputData.sRouteOutput.pCollectData[2]);
						sRouteOutputData.sRouteOutput.pCollectData[2] = NULL;
					}
				}

				pNddsModule->m_bIsRoute = false;
			}
			break;
		case E_NDDS_PROCTYPE_SEARCH:			//검색
			{			
				pNddsModule->SendMessgeCallback(NM_NDDS_SEARCH_START, nSearchSessionID, (long)(sSearchInputData.sSearchInput.eSearchType));

				if(bSearchSync)
				{
					eRequestType = NDDSREQUEST_SERIALIZE;
				}
				else
				{
					eRequestType = NDDSREQUEST_ATONCE;
				}

				switch(sSearchInputData.sSearchInput.eSearchType)
				{
				case N_SEARCHTYPE_NDDSPOI:
					{
						sSearchOutputData.nResult = pNddsModule->SearchNddsPoi(nSearchSessionID, eRequestType, sSearchNddsPoi.szSearchName, sSearchNddsPoi.szAddrName, sSearchNddsPoi.eSortType, &sSearchNddsPoi.wpCurrent, sSearchNddsPoi.nDist, sSearchNddsPoi.eOilType, sSearchNddsPoi.bNewAddr, sSearchOutputData.szErrorMsg);
					}
					break;
				case N_SEARCHTYPE_NDDSAROUNDPOI:
					{
						sSearchOutputData.nResult = pNddsModule->SearchNddsAroundPoi(nSearchSessionID, eRequestType, sSearchNddsAroundPoi.szSearchName, &sSearchNddsAroundPoi.wpCurrent, sSearchNddsAroundPoi.nDist, sSearchNddsPoi.bNewAddr, sSearchOutputData.szErrorMsg);
					}
					break;
				case N_SEARCHTYPE_NDDSPOIDETAILINFO:
					{
						sSearchOutputData.nResult = pNddsModule->SearchNddsPoiDetailInfo(nSearchSessionID, eRequestType, sSearchNddsPoiDetailInfo.nPoiID, sSearchNddsPoiDetailInfo.navSeq, sSearchNddsPoiDetailInfo.nSizeWidth, sSearchNddsPoiDetailInfo.nSizeHeight, sSearchOutputData.szErrorMsg);
					}
                    break;
				case N_SEARCHTYPE_NDDSPOIJOININFO:
					{
						sSearchOutputData.nResult = pNddsModule->SearchNddsPoiJoinInfo(nSearchSessionID, eRequestType, sSearchNddsPoiJoinInfo.nPoiID, sSearchOutputData.szErrorMsg);
					}
					break;
				case N_SEARCHTYPE_NDDSRECENTDESTNATION:
					{
						sSearchOutputData.nResult = pNddsModule->SearchNddsRecentDestnation(nSearchSessionID, eRequestType, sSearchOutputData.szErrorMsg);
					}
					break;
                case N_SEARCHTYPE_NDDSUPLOADRECENTDESTNATION:
                    {
						sSearchOutputData.nResult = pNddsModule->SearchNddsUploadRecentDestnation(nSearchSessionID, eRequestType, sSearchNddsUploadRecentPoi.pUploadRecentPoi, sSearchNddsUploadRecentPoi.nUploadRecentPoiCnt, sSearchNddsUploadRecentPoi.szSvcUploadYn, sSearchOutputData.szErrorMsg);
                    }
                    break;
                case N_SEARCHTYPE_NDDSREMOVERECENTDESTNATION:
                    {
                        sSearchOutputData.nResult = pNddsModule->SearchNddsRemoveRecentDestnation(nSearchSessionID, eRequestType, sSearchNddsRemoveRecentPoi.pRemoveRecentPoi, sSearchNddsRemoveRecentPoi.nRemovePoiCnt, sSearchOutputData.szErrorMsg);
                    }
                    break;
				case N_SEARCHTYPE_NDDSAREANAMESBYSTEP:
					{
						sSearchOutputData.nResult = pNddsModule->SearchNddsAreaNamesByStep(nSearchSessionID, eRequestType, sSearchNddsAreaNamesByStep.szAddrName1, sSearchNddsAreaNamesByStep.szAddrName2, sSearchNddsAreaNamesByStep.bNewAddr, sSearchOutputData.szErrorMsg);
					}
					break;
                case N_SEARCHTYPE_NDDSPOISTATIONINFO:
                    {
                        sSearchOutputData.nResult = pNddsModule->SearchNddsPoiStationInfo(nSearchSessionID, eRequestType, &sSearchNddsPoiStationInfo.wpCurrent, sSearchNddsPoiStationInfo.nDist, sSearchNddsPoiStationInfo.szDataKind, sSearchNddsPoiStationInfo.szFuelType, sSearchOutputData.szErrorMsg);
                    }
                    break;
                case N_SEARCHTYPE_NDDSPOIEVSTATIONINFO:
                    {
                        sSearchOutputData.nResult = pNddsModule->SearchNddsPoiEvStationInfo(nSearchSessionID, eRequestType, sSearchNddsPoiEvStationInfo.nPoiID, sSearchOutputData.szErrorMsg);
                    }
                    break;
                case N_SEARCHTYPE_NDDSPOIEVSTATIONSBYPOINT:
                    {
                        sSearchOutputData.nResult = pNddsModule->SearchNddsPoiEvStationsByPoint(nSearchSessionID, eRequestType, &sSearchNddsPoiEvStationsByPoint.wpCurrent, sSearchNddsPoiEvStationsByPoint.nDist, sSearchOutputData.szErrorMsg);
                    }
                    break;
				default:
					//assert(0);
					break;
				}

				if(eRequestType == NDDSREQUEST_SERIALIZE)
				{
					pNddsModule->SendMessgeCallback(NM_NDDS_SEARCH_STATUS, nSearchSessionID, (long)&sSearchInputData.sSearchInput);

					//DBGPRINT(_T("Result = %d\n"), sSearchOutputData.nResult);
					pNddsModule->SendMessgeCallback(NM_NDDS_SEARCH_END, nSearchSessionID, (long)(sSearchInputData.sSearchInput.eSearchType), sSearchOutputData.nResult);
				}
			}
			break;
		case E_NDDS_PROCTYPE_FAVORITE:			//즐겨찾기
			{
                pNddsModule->SendMessgeCallback(NM_NDDS_FAVORITE_START, nFavoriteSessionID, (long)(sFavoriteInputData.sFavoriteInput.eFavoriteType));

                if(bFavoriteSync)
                {
                    eRequestType = NDDSREQUEST_SERIALIZE;
                }
                else
                {
                    eRequestType = NDDSREQUEST_ATONCE;
                }

            switch(sFavoriteInputData.sFavoriteInput.eFavoriteType)
            {
                case N_FAVORITETYPE_NDDSPOI:					// 즐겨찾기 조회
                {
                    sFavoriteOutputData.nResult = pNddsModule->FavoriteNddsPoi(nFavoriteSessionID, eRequestType, sFavoriteNddsPoi.bMyFavoriteYn, sFavoriteOutputData.szErrorMsg);
                }
                break;
                case N_FAVORITETYPE_NDDSREGISTPOI:			// 즐겨찾기 등록
                {
                    sFavoriteOutputData.nResult = pNddsModule->RegistFavoriteNddsPoi(nFavoriteSessionID, eRequestType, sFavoriteInputData.sFavoriteInput.sRegistFavoriteNddsPoi, sFavoriteOutputData.szErrorMsg);
                }
                break;
                case N_FAVORITETYPE_NDDSUPLOADPOI:		// 즐겨찾기 업로드
                {
                    sFavoriteOutputData.nResult = pNddsModule->UploadFavoriteNddsPoi(nFavoriteSessionID, eRequestType, sFavoriteInputData.sFavoriteInput.sUploadFavoriteNddsPoi, sFavoriteOutputData.szErrorMsg);
                }
                break;
                case N_FAVORITETYPE_NDDSREMOVEPOI:	// 즐겨찾기 멀티 삭제
                {
                    sFavoriteOutputData.nResult = pNddsModule->RemoveFavoriteNddsPoi(nFavoriteSessionID, eRequestType, sFavoriteInputData.sFavoriteInput.sRemoveFavoriteNddsPoi.sRemoveFavoriteList, sFavoriteInputData.sFavoriteInput.sRemoveFavoriteNddsPoi.nPoiCnt, sFavoriteOutputData.szErrorMsg);
                }
                break;
                case N_FAVORITETYPE_NDDSMODIFYPOI: // 즐겨찾기 단건 수정
                {
                    sFavoriteOutputData.nResult = pNddsModule->modifyFavoriteNddsPoi(nFavoriteSessionID, eRequestType, sFavoriteInputData.sFavoriteInput.sModifyFavoriteNddsPoi.sModifyFavorite, sFavoriteOutputData.szErrorMsg);
                }
                break;
                case N_FAVORITETYPE_NDDSREGISTERSENDTOCARINFO:
					{
						sFavoriteOutputData.nResult = pNddsModule->FavoriteNddsRegisterSendToCarInfo(nFavoriteSessionID, eRequestType, sFavoriteNddsRegisterSendToCarInfo.szCustName, &sFavoriteNddsRegisterSendToCarInfo.tp, sFavoriteNddsRegisterSendToCarInfo.nPoiID, sFavoriteNddsRegisterSendToCarInfo.RPFlag, sFavoriteNddsRegisterSendToCarInfo.navSeq, sFavoriteOutputData.szErrorMsg);
					}
					break;
				case N_FAVORITETYPE_NDDSFINDSENDTOCARINFO:
					{
						sFavoriteOutputData.nResult = pNddsModule->FavoriteNddsFindSendToCarInfo(nFavoriteSessionID, eRequestType, sFavoriteNddsFindSendToCarInfo.bNewAddr, sFavoriteOutputData.szErrorMsg);
					}
					break;
				case N_FAVORITETYPE_NDDSREMOVESENDTOCARINFO:
					{
						sFavoriteOutputData.nResult = pNddsModule->FavoriteNddsRemoveSendToCarInfo(nFavoriteSessionID, eRequestType, sFavoriteNddsRemoveSendToCarInfo.szCustName, &sFavoriteNddsRemoveSendToCarInfo.tp, sFavoriteOutputData.szErrorMsg);
					}
					break;
                case N_FAVORITETYPE_NDDSFINDUSERPROFILE:
                    {
                        sFavoriteOutputData.nResult = pNddsModule->FavoriteNddsFindUserProfile(nFavoriteSessionID, eRequestType, sFavoriteOutputData.szErrorMsg);
                    }
                    break;
                case N_FAVORITETYPE_NDDSREGISTUSERPROFILE:
                    {
                        sFavoriteOutputData.nResult = pNddsModule->FavoriteNddsRegistUserProfile(nFavoriteSessionID, eRequestType, sFavoriteInputData.sFavoriteInput.sFavoriteNddsRegistUserProfile, sFavoriteOutputData.szErrorMsg);
                    }
                    break;
                case N_FAVORITETYPE_NDDSFINDCARPROFILE:
                    {
                        sFavoriteOutputData.nResult = pNddsModule->FavoriteNddsFindCarProfile(nFavoriteSessionID, eRequestType, sFavoriteOutputData.szErrorMsg);
                    }
                    break;
                case N_FAVORITETYPE_NDDSREGISTCARPROFILE:
                    {
                        sFavoriteOutputData.nResult = pNddsModule->FavoriteNddsRegistCarProfile(nFavoriteSessionID, eRequestType, sFavoriteInputData.sFavoriteInput.sFavoriteNddsRegistCarProfile, sFavoriteOutputData.szErrorMsg);
                    }
                    break;
                case N_FAVORITETYPE_NDDSFINDUSERDATA:
                    {
                       sFavoriteOutputData.nResult = pNddsModule->FavoriteNddsFindUserData(nFavoriteSessionID, eRequestType, sFavoriteInputData.sFavoriteInput.sFavoriteNddsFindUserData, sFavoriteOutputData.szErrorMsg);
                    }
                    break;
                default:
					//assert(0);
					break;
				}

				pNddsModule->SendMessgeCallback(NM_NDDS_FAVORITE_STATUS, nFavoriteSessionID, (long)&sFavoriteInputData.sFavoriteInput);
				pNddsModule->SendMessgeCallback(NM_NDDS_FAVORITE_END, nFavoriteSessionID, (long)(sFavoriteInputData.sFavoriteInput.eFavoriteType), sFavoriteOutputData.nResult);
			}
			break;
		case E_NDDS_PROCTYPE_TRAFFICINFO:			//교통정보
			{			
				pNddsModule->SendMessgeCallback(NM_NDDS_TRAFFICINFO_START, nTrafficInfoSessionID);

				if(bTrafficInfoSync)
				{
					eRequestType = NDDSREQUEST_SERIALIZE;
				}
				else
				{
					eRequestType = NDDSREQUEST_ATONCE;
				}

				sTrafficInfoOutputData.nResult = pNddsModule->NddsVmsGuideInfo(nTrafficInfoSessionID, eRequestType, sTrafficInfoInputData.szRseId, sTrafficInfoOutputData.szErrorMsg);

				if(eRequestType == NDDSREQUEST_SERIALIZE)
				{
					pNddsModule->SendMessgeCallback(NM_NDDS_TRAFFICINFO_END, nTrafficInfoSessionID, (long)(sTrafficInfoOutputData.nResult));
				}
			}
			break;
		case E_NDDS_PROCTYPE_AUTH:			//인증
			{			
				pNddsModule->SendMessgeCallback(NM_NDDS_AUTH_START, nAuthSessionID);

				if(bAuthSync)
				{
					eRequestType = NDDSREQUEST_SERIALIZE;
				}
				else
				{
					eRequestType = NDDSREQUEST_ATONCE;
				}

                int nErrCode = NDDSERR_UNKNOWN;
                sAuthOutputData.nResult = pNddsModule->NddsAuthforSmartCar(nAuthSessionID, eRequestType, sAuthInputData.szOtpCode, sAuthOutputData.nValidateCode, sAuthOutputData.nResultCode, sAuthOutputData.szAccessKey, nErrCode, sAuthOutputData.szErrorMsg);
		
				if(eRequestType == NDDSREQUEST_SERIALIZE)
				{
                    pNddsModule->SendMessgeCallback(NM_NDDS_AUTH_END, nAuthSessionID, (long)(nErrCode), (long)(sAuthOutputData.nResult), sAuthOutputData.szAccessKey);
				}
			}
			break;
        case E_NDDS_PROCTYPE_NONNDDS:           // AutoComplete, R-Geocode, ComplexImage, AQI. for vNext. 2019.8.8 by hheo
            {
                pNddsModule->SendMessgeCallback(NM_NDDS_NONNDDS_START, nNonNddsSessionID, 0);

                if(bNonNddsSync)
                {
                    eRequestType = NDDSREQUEST_SERIALIZE;
                }
                else
                {
                    eRequestType = NDDSREQUEST_ATONCE;
                }

                sNonNddsOutputData.nResult = pNddsModule->NonNddsRequest(nNonNddsSessionID, eRequestType, sNonNddsInputData.szUrl, sNonNddsInputData.szDomain, sNonNddsOutputData.pszResult, sNonNddsOutputData.szErrorMsg, sNonNddsOutputData.bResult);

                if(eRequestType == NDDSREQUEST_SERIALIZE)
                {
                    pNddsModule->SendMessgeCallback(NM_NDDS_NONNDDS_STATUS, nNonNddsSessionID, 0);

                    //DBGPRINT(_T("Result = %d\n"), sNonNddsOutputData.nResult);
                    pNddsModule->SendMessgeCallback(NM_NDDS_NONNDDS_END, nNonNddsSessionID, 0, sNonNddsOutputData.nResult);
                }
            }
            break;
        default:
			//assert(0);
			break;
		}

#ifdef _LINUX
		sem_post(&pNddsModule->m_pModuleProcSem);
#else
		SDL_SemPost(pNddsModule->m_pModuleProcSem);
#endif

		//DBGPRINT(_T("############################# NddsModuleProc #2\n"));
	}

	//DBGPRINT(_T("MUTEX[%d] - NddsModuleProc::UnlockMutex #1\n"), GetTickCount(), GetTickCount());
#ifdef _LINUX
	pthread_mutex_unlock(&pNddsModule->m_pModuleProcMutex);
#else
	SDL_UnlockMutex(pNddsModule->m_pModuleProcMutex);
#endif
	//DBGPRINT(_T("MUTEX[%d] - NddsModuleProc::UnlockMutex #2\n"), GetTickCount(), GetTickCount());
	return 0;
}


void CNddsModule::SetNddsUpdateCallback(LPCALL_NDDSUPDATE_PROC pfnProc)
{
	UpdateNdds = pfnProc;
}

int CNddsModule::AsyncNddsUpdate(eNddsProtocolType eProtocolType, int nSessionID, int nErrCode)
{
#ifdef _LINUX
	pthread_mutex_lock(&g_this_nddsmodule->m_pAsyncNddsUpdateMutex);
#else
	SDL_LockMutex(g_this_nddsmodule->m_pAsyncNddsUpdateMutex);
#endif
	int nNddsErrorCode = -1;
	printf("=================================> sessionid = %d\n", nSessionID);
	if(eProtocolType == NDDSTYPE_TMAP_PLANNINGROUTE || eProtocolType == NDDSTYPE_TMAP_ROUTESUMMARYINFO)
	{
		nNddsErrorCode = N_ROUTEERR_SUCCED;

		if((eNddsErrorCode)nErrCode != NDDSERR_SUCCESS)
		{
			switch((eNddsErrorCode)nErrCode){
			case NDDSERR_USERCANCEL:
				nNddsErrorCode = N_ROUTEERR_USERCANCEL;
				break;
			case NDDSERR_TIMEOUT:
				nNddsErrorCode = N_ROUTEERR_TIMEOUT;
				break;
			default:
				nNddsErrorCode = N_ROUTEERR_SERVER;
				break;
			}
		}
	}

	switch(eProtocolType)
	{
	case NDDSTYPE_JSON_STRING:
		break;
	case NDDSTYPE_TMAP_PLANNINGROUTE:
		{
            int nRouteCnt = 0;
            NeNddsRouteType eRouteType = N_ROUTETYPE_NORMAL;
			if((eNddsErrorCode)nErrCode == NDDSERR_SUCCESS)
                nRouteCnt = g_this_nddsmodule->GetRouteResult(nSessionID, &nNddsErrorCode, &eRouteType);
            g_this_nddsmodule->SendMessgeCallback(NM_NDDS_ROUTE_END, nSessionID, eRouteType, nRouteCnt, &nNddsErrorCode);
		}
		break;
	case NDDSTYPE_TMAP_ROUTESUMMARYINFO:
		{
			if((eNddsErrorCode)nErrCode == NDDSERR_SUCCESS)
				g_this_nddsmodule->GetRouteSummaryResult(nSessionID, &nNddsErrorCode);
			g_this_nddsmodule->SendMessgeCallback(NM_NDDS_ROUTE_END, nSessionID, NULL, NULL, &nNddsErrorCode);
		}
		break;
	case NDDSTYPE_TMAP_FINDPOIS:
		{
			int nCnt = 0;
			if((eNddsErrorCode)nErrCode == NDDSERR_SUCCESS)
				nCnt = g_this_nddsmodule->GetNddsPoiResult(nSessionID, g_this_nddsmodule->m_bNewAddrChk, g_this_nddsmodule->m_eOilType, &nNddsErrorCode);
			g_this_nddsmodule->SendMessgeCallback(NM_NDDS_SEARCH_END, nSessionID, N_SEARCHTYPE_NDDSPOI, nCnt);
		}
		break;
	case NDDSTYPE_TMAP_FINDAROUNDPOIS:
		{
			int nCnt = 0;
			if((eNddsErrorCode)nErrCode == NDDSERR_SUCCESS)
				nCnt = g_this_nddsmodule->GetNddsAroundPoiResult(nSessionID, g_this_nddsmodule->m_bNewAddrChk, &nNddsErrorCode);
			g_this_nddsmodule->SendMessgeCallback(NM_NDDS_SEARCH_END, nSessionID, N_SEARCHTYPE_NDDSAROUNDPOI, nCnt);
		}
		break;
	case NDDSTYPE_TMAP_FINDPOIDETAILINFO:
		{
			int nCnt = 0;
			if((eNddsErrorCode)nErrCode == NDDSERR_SUCCESS)
				nCnt = g_this_nddsmodule->GetNddsPoiDetailInfoResult(nSessionID, &nNddsErrorCode);
			g_this_nddsmodule->SendMessgeCallback(NM_NDDS_SEARCH_END, nSessionID, N_SEARCHTYPE_NDDSPOIDETAILINFO, nCnt);
		}
		break;
    case NDDSTYPE_TMAP_FINDPOISTATIONINFO:
        {
            int nCnt = 0;
            if((eNddsErrorCode)nErrCode == NDDSERR_SUCCESS)
                nCnt = g_this_nddsmodule->GetNddsPoiStationInfoResult(nSessionID, &nNddsErrorCode);
            g_this_nddsmodule->SendMessgeCallback(NM_NDDS_SEARCH_END, nSessionID, N_SEARCHTYPE_NDDSPOISTATIONINFO, nCnt);
        }
        break;
    case NDDSTYPE_TMAP_FINDPOIEVSTATIONINFO:
        {
            int nCnt = 0;
            if((eNddsErrorCode)nErrCode == NDDSERR_SUCCESS)
                nCnt = g_this_nddsmodule->GetNddsPoiEvStationInfoResult(nSessionID, &nNddsErrorCode);
            g_this_nddsmodule->SendMessgeCallback(NM_NDDS_SEARCH_END, nSessionID, N_SEARCHTYPE_NDDSPOIEVSTATIONINFO, nCnt);
        }
        break;
    case NDDSTYPE_TMAP_FINDPOIEVSTATIONSBYPOINT:
        {
            int nCnt = 0;
            if((eNddsErrorCode)nErrCode == NDDSERR_SUCCESS)
                nCnt = g_this_nddsmodule->GetNddsPoiEvStationsByPointResult(nSessionID, &nNddsErrorCode);
            g_this_nddsmodule->SendMessgeCallback(NM_NDDS_SEARCH_END, nSessionID, N_SEARCHTYPE_NDDSPOIEVSTATIONSBYPOINT, nCnt);
        }
        break;
    case NDDSTYPE_TMAP_UPLOADPOIFAVORITE:
        {
            int nCnt = 0;
            g_this_nddsmodule->SendMessgeCallback(NM_NDDS_FAVORITE_END, nSessionID, N_FAVORITETYPE_NDDSPOI, nCnt);
        }
        break;
    case NDDSTYPE_TMAP_REMOVEPOIFAVORITE:
        {
            int nCnt = 0;
            g_this_nddsmodule->SendMessgeCallback(NM_NDDS_FAVORITE_END, nSessionID, N_FAVORITETYPE_NDDSPOI, nCnt);
        }
        break;
    case NDDSTYPE_TMAP_MODIFYPOIFAVORITE:
        {
            int nCnt = 0;
            g_this_nddsmodule->SendMessgeCallback(NM_NDDS_FAVORITE_END, nSessionID, N_FAVORITETYPE_NDDSPOI, nCnt);
        }
        break;
    case NDDSTYPE_TMAP_FINDUSERPROFILE:
        {
            int nCnt = 0;
            g_this_nddsmodule->SendMessgeCallback(NM_NDDS_FAVORITE_END, nSessionID, N_FAVORITETYPE_NDDSPOI, nCnt);
        }
        break;
    case NDDSTYPE_TMAP_REGISTUSERPROFILE:
        {
            int nCnt = 0;
            g_this_nddsmodule->SendMessgeCallback(NM_NDDS_FAVORITE_END, nSessionID, N_FAVORITETYPE_NDDSPOI, nCnt);
        }
        break;
    case NDDSTYPE_TMAP_FINDCARPROFILE:
        {
            int nCnt = 0;
            g_this_nddsmodule->SendMessgeCallback(NM_NDDS_FAVORITE_END, nSessionID, N_FAVORITETYPE_NDDSPOI, nCnt);
        }
        break;
    case NDDSTYPE_TMAP_REGISTCARPROFILE:
        {
            int nCnt = 0;
            g_this_nddsmodule->SendMessgeCallback(NM_NDDS_FAVORITE_END, nSessionID, N_FAVORITETYPE_NDDSPOI, nCnt);
        }
        break;
	case NDDSTYPE_TMAP_FINDPOIJOININFO:
		{
			int nCnt = 0;
			if((eNddsErrorCode)nErrCode == NDDSERR_SUCCESS)
				nCnt = g_this_nddsmodule->GetNddsPoiJoinInfoResult(nSessionID, &nNddsErrorCode);
			g_this_nddsmodule->SendMessgeCallback(NM_NDDS_SEARCH_END, nSessionID, N_SEARCHTYPE_NDDSPOIJOININFO, nCnt);
		}
		break;
	case NDDSTYPE_TMAP_FINDRECENTDESTNATION:
		{
			int nCnt = 0;
			if((eNddsErrorCode)nErrCode == NDDSERR_SUCCESS)
				nCnt = g_this_nddsmodule->GetNddsRecentDestnationResult(nSessionID, &nNddsErrorCode);
			g_this_nddsmodule->SendMessgeCallback(NM_NDDS_SEARCH_END, nSessionID, N_SEARCHTYPE_NDDSRECENTDESTNATION, nCnt);
		}
		break;
	case NDDSTYPE_TMAP_FINDAREANAMESBYSTEP:
		{
			int nCnt = 0;
			if((eNddsErrorCode)nErrCode == NDDSERR_SUCCESS)
				nCnt = g_this_nddsmodule->GetNddsAreaNamesByStepResult(nSessionID, &nNddsErrorCode);
			g_this_nddsmodule->SendMessgeCallback(NM_NDDS_SEARCH_END, nSessionID, N_SEARCHTYPE_NDDSAREANAMESBYSTEP, nCnt);
		}
		break;
	case NDDSTYPE_TMAP_FINDPOIFAVORITE:
		{
			int nCnt = 0;
			if((eNddsErrorCode)nErrCode == NDDSERR_SUCCESS)
				nCnt = g_this_nddsmodule->GetNddsPoiFavoriteResult(nSessionID, &nNddsErrorCode);
			g_this_nddsmodule->SendMessgeCallback(NM_NDDS_FAVORITE_END, nSessionID, N_FAVORITETYPE_NDDSPOI, nCnt);
		}
		break;
	case NDDSTYPE_TMAP_REGISTERSENDTOCARINFO:
		{
			int nCnt = 0;
			if((eNddsErrorCode)nErrCode == NDDSERR_SUCCESS)
				nCnt = g_this_nddsmodule->GetNddsRegisterSendToCarInfoResult(nSessionID, &nNddsErrorCode);
			g_this_nddsmodule->SendMessgeCallback(NM_NDDS_FAVORITE_END, nSessionID, N_FAVORITETYPE_NDDSREGISTERSENDTOCARINFO, nCnt);
		}
		break;
	case NDDSTYPE_TMAP_FINDSENDTOCARINFO:
		{
			int nCnt = 0;
			if((eNddsErrorCode)nErrCode == NDDSERR_SUCCESS)
				nCnt = g_this_nddsmodule->GetNddsFindSendToCarInfoResult(nSessionID, g_this_nddsmodule->m_bNewAddrChkForS2C, &nNddsErrorCode);
			g_this_nddsmodule->SendMessgeCallback(NM_NDDS_FAVORITE_END, nSessionID, N_FAVORITETYPE_NDDSFINDSENDTOCARINFO, nCnt);
		}
		break;
	case NDDSTYPE_TMAP_REMOVESENDTOCARINFO:
		{
			int nCnt = 0;
			if((eNddsErrorCode)nErrCode == NDDSERR_SUCCESS)
				nCnt = g_this_nddsmodule->GetNddsRemoveSendToCarInfoResult(nSessionID, &nNddsErrorCode);
			g_this_nddsmodule->SendMessgeCallback(NM_NDDS_FAVORITE_END, nSessionID, N_FAVORITETYPE_NDDSREMOVESENDTOCARINFO, nCnt);
		}
		break;
	case NDDSTYPE_TMAP_VMSGUIDEINFO:
		{
			int nCnt = 0;
			if((eNddsErrorCode)nErrCode == NDDSERR_SUCCESS)
				nCnt = g_this_nddsmodule->GetNddsVmsGuideInfoResult(nSessionID, &nNddsErrorCode);
			g_this_nddsmodule->SendMessgeCallback(NM_NDDS_TRAFFICINFO_END, nSessionID, NULL, nCnt);
		}
		break;
	case NDDSTYPE_TMAP_AUTHFORSMARTCAR:
		{
			int nValidateCode=0;
			int nResultCode=0;
			char szAccessKey[50]={0,};
			int nCnt = 0;
			if((eNddsErrorCode)nErrCode == NDDSERR_SUCCESS)
                nCnt = g_this_nddsmodule->GetNddsAuthforSmartCarResult(nSessionID, nValidateCode, nResultCode, szAccessKey, (int*)&nNddsErrorCode);
			g_this_nddsmodule->SendMessgeCallback(NM_NDDS_AUTH_END, nSessionID, (long)(nValidateCode), (long)(nResultCode), szAccessKey);
		}
		break;


	}

	//DBGPRINT(_T("############################# AsyncNddsUpdate #1 eProtocolType=%d, nSessionID=%d, nNddsErrorCode=%d\n"), eProtocolType, nSessionID, nNddsErrorCode);

#ifdef _LINUX
	pthread_mutex_unlock(&g_this_nddsmodule->m_pAsyncNddsUpdateMutex);
#else
	SDL_UnlockMutex(g_this_nddsmodule->m_pAsyncNddsUpdateMutex);
#endif

	return true;
} 

void CNddsModule::IsChangeAccesskey(int nSessionID, char *szAccessKey)
{
	if(szAccessKey == NULL)
		return;
		
	if(strlen(szAccessKey) > 0)
	{
        if(strcmp(szAccessKey, m_szAccessKey) != 0)
			SendMessgeCallback(NM_NDDS_AUTH_CHANGE, nSessionID, NULL, NULL, szAccessKey);
	}
}

void CNddsModule::CheckErrorCode(int nSessionID, char *szCheckCode)
{
	NddsExternalLog(E_DEBUG, "nddsmodule :: CheckErrorCode 1 :: nSessionID = %d, szCheckCode = %x", nSessionID, (void*)szCheckCode);
	if(szCheckCode == NULL)
		return;
		
	NddsExternalLog(E_DEBUG, "nddsmodule :: CheckErrorCode 2 :: nSessionID = %d, szCheckCode = %s", nSessionID, szCheckCode);
	if(strcmp(szCheckCode, "000000") != 0)
	{
		NddsExternalLog(E_DEBUG, "nddsmodule :: CheckErrorCode 2-1 :: nSessionID = %d, szCheckCode = %s", nSessionID, szCheckCode);
		SendMessgeCallback(NM_NDDS_AUTH_PROGRESS, nSessionID, NULL, NULL, szCheckCode);
		NddsExternalLog(E_DEBUG, "nddsmodule :: CheckErrorCode 2-2 :: nSessionID = %d, szCheckCode = %s", nSessionID, szCheckCode);
	}

	NddsExternalLog(E_DEBUG, "nddsmodule :: CheckErrorCode 3 :: finish");
}

//////////////////////////////////////
// 경로탐색 API
//////////////////////////////////////

bool CNddsModule::API_NDDS_Route(PAPI_ROUTE_REQ pRequest, PAPI_ROUTE_RES pResult)
{
	//DBGPRINT(_T("############################# Route #1\n"));

	// 모듈 프로세스 스레드에 요청 중이거나 동작중인지 체크 하여 있으면 실패 처리한다. (동시 요청 안됨)
#ifdef _LINUX
	if(sem_wait(&m_pModuleProcSem) != 0)
#else
	if(SDL_SemWait(m_pModuleProcSem) == SDL_MUTEX_TIMEDOUT)
#endif
	{
		//DBGPRINT(_T("############################# Route #false\n"));
		pResult->nRouteRet = N_ROUTEERR_DUPLICATION;
		return false;
	}

	//DBGPRINT(_T("MUTEX[%d] - API_NDDS_Route::LockMutex #1\n"), GetTickCount());
#ifdef _LINUX
	pthread_mutex_lock(&m_pModuleProcMutex);
#else
	SDL_LockMutex(m_pModuleProcMutex);
#endif

	int nSessionID = ++m_nLastIdxSession;

	SendMessgeCallback(NM_NDDS_ROUTE_INIT, nSessionID, (long)(pRequest->sRouteInput.sRouteInput.m_uMultiOption));
	//////////////////////////////////////
	//Search Input 데이터를 설정한다.
	//Search Outnput 데이터를 설정한다.

	m_sModuleInputProcData.eProcType = E_NDDS_PROCTYPE_ROUTE;
	m_sModuleInputProcData.nRouteSessionID = nSessionID;
	m_sModuleInputProcData.bRouteSync = pRequest->bSync;

	memcpy(&m_sModuleInputProcData.sRouteData.sRouteInput, &pRequest->sRouteInput, sizeof(ROUTE_INPUT));
	memcpy(&m_sModuleOutputProcData.sRouteData.sRouteOutput, &pResult->sRouteOutput, sizeof(NstRouteCollectLogic));

	////////////////////////////////
	// 스레드를 동작시킨다.
#ifdef _LINUX
	pthread_cond_signal(&m_pModuleProcCond);
	pthread_mutex_unlock(&m_pModuleProcMutex);
#else
	SDL_CondSignal(m_pModuleProcCond);
	SDL_UnlockMutex(m_pModuleProcMutex);
#endif
	delay(15);	//스레드 스위칭

	if(pRequest->bSync)
	{
		// 스레드 처리가 끝날때 까지 대기한다.	--> 이 부분 추후 수정 필요 있어보임. 수정하면 위의 delay가 필요 없어질 수도..??
#ifdef _LINUX
		pthread_mutex_lock(&m_pModuleProcMutex);
		pthread_mutex_unlock(&m_pModuleProcMutex);
#else
		SDL_LockMutex(m_pModuleProcMutex);
		SDL_UnlockMutex(m_pModuleProcMutex);
#endif
		pResult->nRouteRet = m_sModuleOutputProcData.sRouteData.m_nCurRouteRet;
		pResult->nRouteCnt = m_sModuleOutputProcData.sRouteData.nResult;

		if(pResult->nRouteCnt <= 0 || pResult->nRouteRet != N_ROUTEERR_SUCCED) {
			NddsExternalLog(E_DEBUG, "hhtest pResult->nRouteCnt = %d, pResult->nRouteRet = %d\n", pResult->nRouteCnt, pResult->nRouteRet);
			return false;
		}
	}

	pResult->nSessionID = nSessionID;
	//DBGPRINT(_T("############################# Route #true\n"));

	return true;
}

bool CNddsModule::API_NDDS_StopRoute(PAPI_STOPROUTE_REQ pRequest, PAPI_STOPROUTE_RES pResult)
{
#if 0
	m_bStopServerRoute = true;
	return true;
#else
	return NDDSLIB_CancelProtocol(pRequest->nSessionID);
#endif
}

bool CNddsModule::API_NDDS_SetTmapHost(const char *pszHostName, const unsigned int nPort)
{
    return NDDSLIB_SetTmapHost(pszHostName, nPort);
}

bool CNddsModule::API_NDDS_GetWeatherInfo(PAPI_WEATHERINFO_REQ pRequest, PAPI_WEATHERINFO_RES pResult)
{
	strcpy(pResult->szSkyCode, m_pNddsWeatherInfo.m_SkyCode);
	strcpy(pResult->szSkyName, m_pNddsWeatherInfo.m_SkyName);
	strcpy(pResult->szRainTypeCode, m_pNddsWeatherInfo.m_RainTypeCode);
	strcpy(pResult->szRainSinceOnTime, m_pNddsWeatherInfo.m_RainSinceOnTime);
	strcpy(pResult->szTempC1h, m_pNddsWeatherInfo.m_TempC1h);

	return true;
}

bool CNddsModule::API_NDDS_GetRouteSummaryInfo(PAPI_GET_ROUTESUMMARYINFO_REQ pRequest, PAPI_GET_ROUTESUMMARYINFO_RES pResult)
{
	bool bResult = false;
	for(int i=0; i<3 ;i++)
	{
        if(pRequest->nSessionID == m_pNddsRouteSummaryResult[i].m_SessionID)
		{
			pResult->nTotalTime = m_pNddsRouteSummaryResult[i].m_TotalTime;
			pResult->nEstimationOption = m_pNddsRouteSummaryResult[i].m_EstimationOption;
			strcpy(pResult->szEstimationTime, m_pNddsRouteSummaryResult[i].m_EstimationTime);
			bResult = true;
			break;
		}
	}

	return bResult;
}

const int Meter_PER_World_X[] = {
	2124, 2124, 2124, 2124, 2124, 2124, 2124, 2124, 2123, 2123, // 0~9
	2123, 2123, 2123, 2123, 2123, 2123, 2123, 2123, 2123, 2123, // 1~1.9
	2122, 2122, 2122, 2122, 2122, 2122, 2122, 2121, 2121, 2121, // 2~2.9
	2121, 2121, 2120, 2120, 2120, 2120, 2120, 2119, 2119, 2119, // 3~3.9
	2119, 2118, 2118, 2118, 2117, 2117, 2117, 2117, 2116, 2116, // 4~4.9
	2116, 2115, 2115, 2115, 2114, 2114, 2114, 2113, 2113, 2113, // 5~5.9
	2112, 2112, 2111, 2111, 2111, 2110, 2110, 2109, 2109, 2108, // 6~6.9
	2108, 2108, 2107, 2107, 2106, 2106, 2105, 2105, 2104, 2104, // 7~7.9
	2103, 2103, 2102, 2102, 2101, 2100, 2100, 2099, 2099, 2098, // 8~8.9
	2098, 2097, 2097, 2096, 2095, 2095, 2094, 2094, 2093, 2092, // 9~9.9
	2092, 2091, 2090, 2090, 2089, 2088, 2088, 2087, 2086, 2086, // 10~10.9
	2085, 2084, 2083, 2083, 2082, 2081, 2081, 2080, 2079, 2078, // 11~11.9
	2078, 2077, 2076, 2075, 2074, 2074, 2073, 2072, 2071, 2070, // 12~12.9
	2070, 2069, 2068, 2067, 2066, 2065, 2064, 2064, 2063, 2062, // 13~13.9
	2061, 2060, 2059, 2058, 2057, 2056, 2055, 2055, 2054, 2053, // 14~14.9
	2052, 2051, 2050, 2049, 2048, 2047, 2046, 2045, 2044, 2043, // 15~15.9
	2042, 2041, 2040, 2039, 2038, 2037, 2036, 2035, 2034, 2032, // 16~16.9
	2031, 2030, 2029, 2028, 2027, 2026, 2025, 2024, 2023, 2021, // 17~17.9
	2020, 2019, 2018, 2017, 2016, 2015, 2013, 2012, 2011, 2010, // 18~18.9
	2009, 2007, 2006, 2005, 2004, 2002, 2001, 2000, 1999, 1998, // 19~19.9
	1996, 1995, 1994, 1992, 1991, 1990, 1989, 1987, 1986, 1985, // 20~20.9
	1983, 1982, 1981, 1979, 1978, 1977, 1975, 1974, 1973, 1971, // 21~21.9
	1970, 1968, 1967, 1966, 1964, 1963, 1961, 1960, 1959, 1957, // 22~22.9
	1956, 1954, 1953, 1951, 1950, 1948, 1947, 1945, 1944, 1942, // 23~23.9
	1941, 1939, 1938, 1936, 1935, 1933, 1932, 1930, 1929, 1927, // 24~24.9
	1926, 1924, 1923, 1921, 1919, 1918, 1916, 1915, 1913, 1911, // 25~25.9
	1910, 1908, 1907, 1905, 1903, 1902, 1900, 1898, 1897, 1895, // 26~26.9
	1893, 1892, 1890, 1888, 1887, 1885, 1883, 1881, 1880, 1878, // 27~27.9
	1876, 1875, 1873, 1871, 1869, 1868, 1866, 1864, 1862, 1860, // 28~28.9
	1859, 1857, 1855, 1853, 1851, 1850, 1848, 1846, 1844, 1842, // 29~29.9
	1840, 1839, 1837, 1835, 1833, 1831, 1829, 1827, 1825, 1824, // 30~30.9
	1822, 1820, 1818, 1816, 1814, 1812, 1810, 1808, 1806, 1804, // 31~31.9
	1802, 1800, 1798, 1796, 1795, 1793, 1791, 1789, 1787, 1785, // 32~32.9
	1783, 1781, 1778, 1776, 1774, 1772, 1770, 1768, 1766, 1764, // 33~33.9
	1762, 1760, 1758, 1756, 1754, 1752, 1750, 1748, 1745, 1743, // 34~34.9
	1741, 1739, 1737, 1735, 1733, 1731, 1728, 1726, 1724, 1722, // 35~35.9
	1720, 1718, 1715, 1713, 1711, 1709, 1707, 1704, 1702, 1700, // 36~36.9
	1698, 1696, 1693, 1691, 1689, 1687, 1684, 1682, 1680, 1678, // 37~37.9
	1675, 1673, 1671, 1668, 1666, 1664, 1661, 1659, 1657, 1655, // 38~38.9
	1652, 1650, 1648, 1645, 1643, 1641, 1638, 1636, 1633, 1631, // 39~39.9
	1629, 1626, 1624, 1622, 1619, 1617, 1614, 1612, 1610, 1607, // 40~40.9
	1605, 1602, 1600, 1597, 1595, 1592, 1590, 1588, 1585, 1583, // 41~41.9
	1580, 1578, 1575, 1573, 1570, 1568, 1565, 1563, 1560, 1558, // 42~42.9
	1555, 1553, 1550, 1548, 1545, 1542, 1540, 1537, 1535, 1532, // 43~43.9
	1530, 1527, 1525, 1522, 1519, 1517, 1514, 1512, 1509, 1506, // 44~44.9
	1504, 1501, 1498, 1496, 1493, 1491, 1488, 1485, 1483, 1480, // 45~45.9
	1477, 1475, 1472, 1469, 1467, 1464, 1461, 1459, 1456, 1453, // 46~46.9
	1450, 1448, 1445, 1442, 1440, 1437, 1434, 1431, 1429, 1426, // 47~47.9
	1423, 1420, 1418, 1415, 1412, 1409, 1407, 1404, 1401, 1398, // 48~48.9
	1395, 1393, 1390, 1387, 1384, 1381, 1379, 1376, 1373, 1370, // 49~49.9
	1367, 1364, 1362, 1359, 1356, 1353, 1350, 1347, 1344, 1342, // 50~50.9
	1339, 1336, 1333, 1330, 1327, 1324, 1321, 1318, 1316, 1313, // 51~51.9
	1310, 1307, 1304, 1301, 1298, 1295, 1292, 1289, 1286, 1283, // 52~52.9
	1280, 1277, 1274, 1271, 1268, 1265, 1262, 1259, 1257, 1254, // 53~53.9
	1251, 1248, 1245, 1242, 1238, 1235, 1232, 1229, 1226, 1223, // 54~54.9
	1220, 1217, 1214, 1211, 1208, 1205, 1202, 1199, 1196, 1193, // 55~55.9
	1190, 1187, 1184, 1181, 1177, 1174, 1171, 1168, 1165, 1162, // 56~56.9
	1159, 1156, 1153, 1150, 1146, 1143, 1140, 1137, 1134, 1131, // 57~57.9
	1128, 1124, 1121, 1118, 1115, 1112, 1109, 1106, 1102, 1099, // 58~58.9
	1096, 1093, 1090, 1086, 1083, 1080, 1077, 1074, 1070, 1067, // 59~59.9
	1064, 1061, 1058, 1054, 1051, 1048, 1045, 1041, 1038, 1035, // 60~60.9
	1032, 1029, 1025, 1022, 1019, 1015, 1012, 1009, 1006, 1002, // 61~61.9
	999,  996,  993,  989,  986,  983,  979,  976,  973,  970, // 62~62.9
	966,  963,  960,  956,  953,  950,  946,  943,  940,  936, // 63~63.9
	933,  930,  926,  923,  920,  916,  913,  910,  906,  903, // 64~64.9
	900,  896,  893,  889,  886,  883,  879,  876,  873,  869, // 65~65.9
	866,  862,  859,  856,  852,  849,  845,  842,  839,  835, // 66~66.9
	832,  828,  825,  821,  818,  815,  811,  808,  804,  801, // 67~67.9
	797,  794,  791,  787,  784,  780,  777,  773,  770,  766, // 68~68.9
	763,  759,  756,  752,  749,  746,  742,  739,  735,  732, // 69~69.9
	728,  725,  721,  718,  714,  711,  707,  704,  700,  697, // 70~70.9
	693,  690,  686,  683,  679,  676,  672,  668,  665,  661, // 71~71.9
	658,  654,  651,  647,  644,  640,  637,  633,  630,  626, // 72~72.9
	622,  619,  615,  612,  608,  605,  601,  598,  594,  590, // 73~73.9
	587,  583,  580,  576,  573,  569,  565,  562,  558,  555, // 74~74.9
	551,  547,  544,  540,  537,  533,  530,  526,  522,  519, // 75~75.9
	515,  511,  508,  504,  501,  497,  493,  490,  486,  483, // 76~76.9
	479,  475,  472,  468,  464,  461,  457,  454,  450,  446, // 77~77.9
	443,  439,  435,  432,  428,  425,  421,  417,  414,  410, // 78~78.9
	406,  403,  399,  395,  392,  388,  384,  381,  377,  373, // 79~79.9
	370,  366,  362,  359,  355,  351,  348,  344,  340,  337, // 80~80.9
	333,  329,  326,  322,  318,  315,  311,  307,  304,  300, // 81~81.9
	296,  293,  289,  285,  282,  278,  274,  271,  267,  263, // 82~82.9
	260,  256,  252,  248,  245,  241,  237,  234,  230,  226, // 83~83.9
	223,  219,  215,  211,  208,  204,  200,  197,  193,  189, // 84~84.9
	186,  182,  178,  174,  171,  167,  163,  160,  156,  152, // 85~85.9
	149,  145,  141,  137,  134,  130,  126,  123,  119,  115, // 86~86.9
	111,  108,  104,  100,   97,   93,   89,   85,   82,   78, // 87~87.9
	74,   71,   67,   63,   59,   56,   52,   48,   45,   41, // 88~88.9
	37,   33,   30,   26,   22,   19,   15,   11,    7,    4 // 89~89.9
};

// 위도별 METER/WORLDY 값 데이터
const int Meter_PER_World_Y[] = {
	2109, 2109, 2110, 2110, 2110, 2110, 2110, 2110, 2110, 2110, //0~9
	2110, 2110, 2110, 2110, 2111, 2111, 2111, 2111, 2111, 2112, //10~19
	2112, 2112, 2112, 2113, 2113, 2113, 2113, 2114, 2114, 2114, //20~29
	2114, 2115, 2115, 2115, 2116, 2116, 2116, 2117, 2117, 2117, //30~39
	2118, 2118, 2118, 2119, 2119, 2119, 2120, 2120, 2121, 2121, //40~49
	2121, 2122, 2122, 2122, 2123, 2123, 2123, 2124, 2124, 2124, //50~59
	2124, 2125, 2125, 2125, 2126, 2126, 2126, 2126, 2127, 2127, //60~69
	2127, 2127, 2128, 2128, 2128, 2128, 2128, 2129, 2129, 2129, //70~79
	2129, 2129, 2129, 2129, 2129, 2129, 2129, 2129, 2130, 2130  //80~89
};

#define WORLDCOORD_PER_DEG_INT	524288 // WGS84 좌표에 WORLDCOORD_PER_DEG를 곱하면 World좌표가 된다.

int GetMeterPerWorldX(int StartY, int EndY)
{
	int nIdx;
	int nValue = StartY + EndY;
	static int nSize = sizeof(Meter_PER_World_X) / sizeof(Meter_PER_World_X[0]) - 1;

	// 	nIdx = (int)((nValue / WORLDCOORD_PER_DEG * 5.0) + 0.5);
	nIdx = nValue / WORLDCOORD_PER_DEG_INT * 5;

	if (nIdx > nSize)
		nIdx = nSize;

	return	Meter_PER_World_X[nIdx];
}

int GetMeterPerWorldY(int StartY, int EndY)
{
	int nIdx;
	int nValue = StartY + EndY;
	static int nSize = sizeof(Meter_PER_World_Y) / sizeof(Meter_PER_World_Y[0]) - 1;

	// 	nIdx = (int)((nValue / WORLDCOORD_PER_DEG / 2.0) + 0.5);
	nIdx = nValue / WORLDCOORD_PER_DEG_INT / 2;

	if (nIdx > nSize)
		nIdx = nSize;

	return Meter_PER_World_Y[nIdx];
}

inline static int GetRoughDist(stVERTEX32* ptOrg, stVERTEX32* ptDst)
	{
		int r, dd, s;
		float	x, y;
		int dx, dy;
		int x1, y1;		// startPosition
		int x2, y2;		// endPosition

		x1 = (int)(ptOrg->x * (GetMeterPerWorldX(ptOrg->y, ptDst->y) / NOFP_DEG));
		y1 = (int)(ptOrg->y * (GetMeterPerWorldY(ptOrg->y, ptDst->y) / NOFP_DEG));
		x2 = (int)(ptDst->x * (GetMeterPerWorldX(ptOrg->y, ptDst->y) / NOFP_DEG));
		y2 = (int)(ptDst->y * (GetMeterPerWorldY(ptOrg->y, ptDst->y) / NOFP_DEG));


		dx = x2 - x1;
		dy = y2 - y1;

		if (dx < 0) dx = -dx;
		if (dy < 0) dy = -dy;
		if (dx < dy) {
			s = dx;
			dx = dy;
			dy = s;
		}

		if (dy == 0) return dx;

		if (dx > 32767 || dy > 32767) {
			x = (float)dx;
			y = (float)dy;

			return (int)sqrt(x*x+y*y);
		}

		//initial hypotenuse guess (from Gems)
		r = dx+(dy>>1);
		//assert(r != 0);
		dd = (dx*dx)+(dy*dy);

		//converge 3 times
		r = ((dd/r)+r)>>1;
		r = ((dd/r)+r)>>1;

		return r;
	};

bool CNddsModule::IsValidePositionByRoutting(int nPosInfoCnt, NstSchPosInfo *pnsPosInfo, bool bStopPosInfoFlag, int *pnErrCode)
{
	//////////////////////////////////////
	// 각각의 좌표가 50m 이내에 존재하는지
	// 각각의 좌표의 총거리가 700Km 이상 되는지 체크

	int		nDist = 0, nEndIndex = 0;
	stVERTEX32	ptStart, ptEnd;

	// 출발지를 GPS로 설정시 맵매칭데이터가 0,0인 경우가 있어서 예외처리함
	if(pnsPosInfo[0].m_bPos && (pnsPosInfo[0].m_wptX <= 0 || pnsPosInfo[0].m_wptY <= 0))
	{
		*pnErrCode = N_ROUTEERR_STARTPOS2;
		return false;
	}

	for (int i = 1 ; i < nPosInfoCnt ; i++)
	{
		ptStart.x = pnsPosInfo[i-1].m_wptX;
		ptStart.y = pnsPosInfo[i-1].m_wptY;

		nEndIndex = (i == nPosInfoCnt-1) ? 4 : i;	// 목적지
		ptEnd.x		= pnsPosInfo[nEndIndex].m_wptX;
		ptEnd.y		= pnsPosInfo[nEndIndex].m_wptY;

		//assert(pnsPosInfo[i-1].m_bPos  && pnsPosInfo[nEndIndex].m_bPos);

		if(!(pnsPosInfo[i-1].m_bPos  && pnsPosInfo[nEndIndex].m_bPos))
		{
			*pnErrCode = N_ROUTEERR_COORDINATE_ERROR;
			return false;
		}

		nDist = GetRoughDist(&ptStart, &ptEnd);

		// 정차지점사용 flag == 0이고, 탐색 경로가 50m 이하
		if (!bStopPosInfoFlag && nDist < 50)
		{
			*pnErrCode = N_ROUTEERR_SAMEPOINT;
			return false;
		}
	}

	return true;
}

int CNddsModule::RouteFunc(int nSessionID, eNddsRequestType eRequestType, NstRouteReqLogic *pRouteInput, int &nErrCode, NstRouteCollectLogic *pRouteOutput)
{
	m_bStopServerRoute = false;

	bool bAddRoute = false;
	int i;
	int nRouteSuccesCount	= 0;

	nErrCode		= N_ROUTEERR_FAIL;

	if(pRouteInput->eRouteType == N_ROUTETYPE_ADD)
	{
		bAddRoute = true;
	}

	// 경로 지점 유효성 체크
	if (!IsValidePositionByRoutting(pRouteInput->m_nPosInfoCnt, pRouteInput->m_nsPosInfo, pRouteInput->m_bStopPosInfoFlag, &nErrCode))
	{
		NddsExternalLog(E_DEBUG, "nddsmodule :: IsValidePositionByRoutting Failed!!!! nErrCode = %d", nErrCode);
        return 0;
	}

	//////////////////////////////////////
	// 서버 탐색

	char szErrorMsg[256];
	int nNddsErrorCode = NDDSERR_SUCCESS;
	NstTVASFilesInfo sFileInfo;
			
	memset(&sFileInfo, 0x00, sizeof(NstTVASFilesInfo));
	
	NddsExternalLog(E_DEBUG, "nddsmodule :: RouttingServer before");
    nRouteSuccesCount = RouttingServer(nSessionID, eRequestType, pRouteInput, pRouteOutput, &sFileInfo, szErrorMsg, &nNddsErrorCode );
	NddsExternalLog(E_DEBUG, "nddsmodule :: RouttingServer after :: nRouteSuccesCount = %d, nNddsErrorCode = %d", nRouteSuccesCount, nNddsErrorCode);
	if (nRouteSuccesCount <= 0)
	{
        // NDDS 에러 코드 => NeRouteErrCode 변환
        // USER CANCEL 은 별도 처리 UI에서 예외 처리 해야함.minha
        printf("RouttingServer error %d\n",nNddsErrorCode); fflush(stdout);
        switch(nNddsErrorCode){
            case NDDSERR_USERCANCEL:
                nErrCode = N_ROUTEERR_USERCANCEL;
                break;
            default:
                nErrCode = N_ROUTEERR_SERVER;
                break;
        }
        return 0;
	}

	nErrCode		= N_ROUTEERR_SUCCED;

    return nRouteSuccesCount;
}

bool CNddsModule::RouteSummaryFunc(int nSessionID, eNddsRequestType eRequestType, NstRouteSummaryReqLogic *pRouteSummaryInput, int &nErrCode)
{
	m_bStopServerRoute = false;

	bool bAddRoute = false;
	int i;
	int nOldRouteSuccesCount = 0;
	int nRouteSuccesCount	= 0;
	bool bSuccess[4];
	memset(bSuccess, 0x00, sizeof(bool)*4);

	nErrCode		= N_ROUTEERR_FAIL;

	// 경로 지점 유효성 체크
	if (!IsValidePositionByRoutting(pRouteSummaryInput->m_nPosInfoCnt, pRouteSummaryInput->m_nsPosInfo, 0, &nErrCode))
	{
		return false;
	}

	//////////////////////////////////////
	// 서버 탐색

	char szErrorMsg[256];
	int nNddsErrorCode;

	if(RouttingSummaryServer(nSessionID, eRequestType, pRouteSummaryInput, szErrorMsg, &nNddsErrorCode ))
	{
		nRouteSuccesCount++;
	}
	else
	{
		// NDDS 에러 코드 => NeRouteErrCode 변환
		// USER CANCEL 은 별도 처리 UI에서 예외 처리 해야함.minha
		switch(nNddsErrorCode){
			case NDDSERR_USERCANCEL:
				nErrCode = N_ROUTEERR_USERCANCEL;
				break;
			case NDDSERR_TIMEOUT:
				nErrCode = N_ROUTEERR_TIMEOUT;
				break;
			default:
				nErrCode = N_ROUTEERR_SERVER;
				break;
		}
	}

	if (nRouteSuccesCount <= 0)
	{
		//assert(0);
		return false;
	}

	nErrCode		= N_ROUTEERR_SUCCED;

	return true;
}

#define ROUTE_BUFFER	50000
int CNddsModule::RouttingServer(int nSessionID, eNddsRequestType eRequestType, NstRouteReqLogic *pRouteInput, NstRouteCollectLogic *pRouteOutput, NstTVASFilesInfo *pFileInfo, char *szErrorMsg,int *nErrorCode)
{
	NddsExternalLog(E_DEBUG, "nddsmodule :: in RouttingServer 1");
	eNddsErrorCode eErrCode = NDDSERR_UNKNOWN;

	ENDDS_FAREWEIGHTOPTS fareWeightOpts = LOGICAPPLIED;

    unsigned char pBuffer[ROUTE_BUFFER] = {};
	int nPos = 0, i;

	NDDSTMAP_PLANNINGROUTE_REQ sRequest;
	memset(&sRequest, 0x00, sizeof(NDDSTMAP_PLANNINGROUTE_REQ));

	NddsExternalLog(E_DEBUG, "nddsmodule :: in RouttingServer 2 :: eRouteType = %d", pRouteInput->eRouteType);
	switch(pRouteInput->eRouteType)
	{
	case N_ROUTETYPE_NORMAL:
	case N_ROUTETYPE_ADD:
		sRequest.ReSearchType = REQ_ROUTE;	break;
	case N_ROUTETYPE_RE_OPTION:
	case N_ROUTETYPE_RE_OUTROUTE:
	case N_ROUTETYPE_TPEG_RE_ROUTE:
		sRequest.ReSearchType = REQ_REROUTE;	break;
	case N_ROUTETYPE_PATTERN_ARRIVAL:
	case N_ROUTETYPE_PATTERN_DEPARTURE:
		{
			sRequest.ReSearchType = REQ_ROUTE;
		}
		break;
	default:
        return 0;
		break;
	}

	NddsExternalLog(E_DEBUG, "nddsmodule :: in RouttingServer 3");
	// GPS 궤적 설정
	sRequest.nGpsTraceDataSize = 0;
	sRequest.gpsTraceData = NULL;
	if(pRouteInput->m_bStGpsPosition)
    {
		NddsExternalLog(E_DEBUG, "nddsmodule :: in RouttingServer 3-1");
        if(pRouteInput->m_nsGpsTrace.nGpsTraceDataSize > 0)
        {
			NddsExternalLog(E_DEBUG, "nddsmodule :: in RouttingServer 3-2");
            if(m_pszGpsTraceData != NULL)
            {
				NddsExternalLog(E_DEBUG, "nddsmodule :: in RouttingServer 3-3");
                free(m_pszGpsTraceData);
                m_pszGpsTraceData = NULL;
            }

			NddsExternalLog(E_DEBUG, "nddsmodule :: in RouttingServer 3-4");
            m_pszGpsTraceData = (char*)malloc(sizeof(char)*(pRouteInput->m_nsGpsTrace.nGpsTraceDataSize+1));
            memset(m_pszGpsTraceData, 0x00, pRouteInput->m_nsGpsTrace.nGpsTraceDataSize+1);
            strncpy(m_pszGpsTraceData, pRouteInput->m_nsGpsTrace.pGpasTraceData, pRouteInput->m_nsGpsTrace.nGpsTraceDataSize);
            sRequest.gpsTraceData = m_pszGpsTraceData;
            sRequest.nGpsTraceDataSize = pRouteInput->m_nsGpsTrace.nGpsTraceDataSize;
        }
	}

	////////////
	// RouteCommonInfo
	NddsExternalLog(E_DEBUG, "nddsmodule :: in RouttingServer 4");
	sRequest.common.requestTime = (char*)(pBuffer + nPos);
	nPos += sprintf(sRequest.common.requestTime, "%04d%02d%02d%02d%02d%02d", pRouteInput->m_ReqTime.Year, pRouteInput->m_ReqTime.Month, pRouteInput->m_ReqTime.Day, pRouteInput->m_ReqTime.Hour, pRouteInput->m_ReqTime.Minute, pRouteInput->m_ReqTime.Second);
	nPos = BYTEALIGN_SIZE(nPos+1);

	int nRoutePlanType = 0;
	sRequest.common.countRoutePlanTypes = 0;
	sRequest.common.routePlanTypes = (ENDDS_FIRSTGUIDEOPTION*)(pBuffer + nPos);
	if(pRouteInput->m_uMultiOption & N_MULTIROUTE_BASIC_OPTIMUM)
	{
		sRequest.common.routePlanTypes[nRoutePlanType] = TRAFFIC_RECOMMEND;
		nRoutePlanType++;
		sRequest.common.countRoutePlanTypes = nRoutePlanType;
	}
	if(pRouteInput->m_uMultiOption & N_MULTIROUTE_BASIC_EXPRESSWAY)
	{
		sRequest.common.routePlanTypes[nRoutePlanType] = TRAFFIC_HIGHWAY;
		nRoutePlanType++;
		sRequest.common.countRoutePlanTypes = nRoutePlanType;
	}
	if(pRouteInput->m_uMultiOption & N_MULTIROUTE_FEE_MINIMAL)
	{
		sRequest.common.routePlanTypes[nRoutePlanType] = TRAFFIC_FREE;
		nRoutePlanType++;
		sRequest.common.countRoutePlanTypes = nRoutePlanType;
	}
	if(pRouteInput->m_uMultiOption & N_MULTIROUTE_BASIC_SHORTESTWAY)
	{
		sRequest.common.routePlanTypes[nRoutePlanType] = SHORTEST_CHARGEDANDFREE;
		nRoutePlanType++;
		sRequest.common.countRoutePlanTypes = nRoutePlanType;
	}
	if(pRouteInput->m_uMultiOption & N_MULTIROUTE_BASIC_EASY)
	{
		sRequest.common.routePlanTypes[nRoutePlanType] = TRAFFIC_FIRSTTIME;
		nRoutePlanType++;
		sRequest.common.countRoutePlanTypes = nRoutePlanType;
	}
	if(pRouteInput->m_uMultiOption & N_MULTIROUTE_BASIC_SHORTTIME)
	{
		sRequest.common.routePlanTypes[nRoutePlanType] = TRAFFIC_MINTIME;
		nRoutePlanType++;
		sRequest.common.countRoutePlanTypes = nRoutePlanType;
	}

	NddsExternalLog(E_DEBUG, "nddsmodule :: in RouttingServer 5");
	nPos += sizeof(ENDDS_FIRSTGUIDEOPTION)*nRoutePlanType;
	nPos = BYTEALIGN_SIZE(nPos);

	sRequest.tvas = (char*)(pBuffer + nPos);
    if(pRouteInput->m_Tvas[0] == NULL)
		nPos += sprintf(sRequest.tvas, "%s", "5.0");
	else
		nPos += sprintf(sRequest.tvas, "%s", pRouteInput->m_Tvas);

	nPos = BYTEALIGN_SIZE(nPos+1);

	////////////
	// DepartRoadType

	switch(pRouteInput->m_nStRoadType)
	{
	case 1:	sRequest.departure.departRoadType = ROADTYPE_HIGHWAY; break;
	case 2:	sRequest.departure.departRoadType = ROADTYPE_CARWAY; break;
	case 3:	sRequest.departure.departRoadType = ROADTYPE_OVERPASS; break;
	case 4:	sRequest.departure.departRoadType = ROADTYPE_UNDERPASS; break;
	case 5: sRequest.departure.departRoadType = ROADTYPE_GENERAL; break;
	case 6: sRequest.departure.departRoadType = ROADTYPE_NEARWAY; break;
	case 7: sRequest.departure.departRoadType = ROADTYPE_NOTSELECTED; break;
	default:
		 sRequest.departure.departRoadType = ROADTYPE_NONE; break;
	}

	NddsExternalLog(E_DEBUG, "nddsmodule :: in RouttingServer 6");
	sRequest.departure.departName = (char*)(pBuffer + nPos);
	nPos += sprintf(sRequest.departure.departName, "%s", pRouteInput->m_nsPosInfo[0].m_strName);
	nPos = BYTEALIGN_SIZE(nPos+1);
	
	sRequest.departure.departXPos = (int)(pRouteInput->m_nsPosInfo[0].m_wptX / (256*2048.0) * 36000.0f + 0.5f);
	sRequest.departure.departYPos = (int)(pRouteInput->m_nsPosInfo[0].m_wptY / (256*2048.0) * 36000.0f + 0.5f);
	sRequest.departure.angle = pRouteInput->m_nAngle;
	sRequest.departure.speed = pRouteInput->m_nSpeed;
	sRequest.departure.departDirPriority = (unsigned char)pRouteInput->m_nDirPriority;
	//sRequest.departure.departSrchFlag = pRouteInput->m_bStGpsPosition?0x00:0x07;
	sRequest.departure.departSrchFlag = pRouteInput->m_DepartSrchFlag;

	/////////////
	// RouteDestinationInfo
    sRequest.destination.destSearchFlag = pRouteInput->m_DestSrchFlag;


	sRequest.destination.destName = (char*)(pBuffer + nPos);
	nPos += sprintf(sRequest.destination.destName, "%s", pRouteInput->m_nsPosInfo[4].m_strName);
	nPos = BYTEALIGN_SIZE(nPos+1);

	sRequest.destination.destXPos = (int)(pRouteInput->m_nsPosInfo[4].m_wptX / (256*2048.0) * 36000.0f + 0.5f);
	sRequest.destination.destYPos = (int)(pRouteInput->m_nsPosInfo[4].m_wptY / (256*2048.0) * 36000.0f + 0.5f);
	if(pRouteInput->m_nsPosInfo[4].m_nPOIID > 0)
	{
		sRequest.destination.destPoiId = (char*)(pBuffer + nPos);
		nPos += sprintf(sRequest.destination.destPoiId, "%d", pRouteInput->m_nsPosInfo[4].m_nPOIID);
		nPos = BYTEALIGN_SIZE(nPos+1);
	}

	NddsExternalLog(E_DEBUG, "nddsmodule :: in RouttingServer 7");
	//로컬에서는 RpFlag를 문자로 0~9, A~G까지 사용하지만 NDDS서버에서는 아스키코드의 10진수 변환값 0~16까지 사용함
	if(pRouteInput->m_nsPosInfo[4].m_nRpFlag >= '0' && pRouteInput->m_nsPosInfo[4].m_nRpFlag <= '9')
	{
		//0~9
		sRequest.destination.destRpFlag = pRouteInput->m_nsPosInfo[4].m_nRpFlag - 48;
	}
	else if(pRouteInput->m_nsPosInfo[4].m_nRpFlag >= 'A' && pRouteInput->m_nsPosInfo[4].m_nRpFlag <= 'G')
	{
		//10~16
		sRequest.destination.destRpFlag = (pRouteInput->m_nsPosInfo[4].m_nRpFlag - 65) + 10;
	}
    else if(pRouteInput->m_nsPosInfo[4].m_nRpFlag >= 'H' && pRouteInput->m_nsPosInfo[4].m_nRpFlag <= 'J')
    {   //방향성 고려 근접링크로 16으로 통일
          pRouteInput->m_nsPosInfo[4].m_nRpFlag = 'G';
          sRequest.destination.destRpFlag = (pRouteInput->m_nsPosInfo[4].m_nRpFlag - 65) + 10;
    }
    else if(pRouteInput->m_nsPosInfo[4].m_nRpFlag >= 'K' && pRouteInput->m_nsPosInfo[4].m_nRpFlag <= 'N')
    {
        //유사 RpFlag로 변환
        sRequest.destination.destRpFlag = (pRouteInput->m_nsPosInfo[4].m_nRpFlag - 65) + 10 - 8;
    }
    else if(pRouteInput->m_nsPosInfo[4].m_nRpFlag >= 'a' && pRouteInput->m_nsPosInfo[4].m_nRpFlag <= 'g')
    {
        //10~16
        sRequest.destination.destRpFlag = (pRouteInput->m_nsPosInfo[4].m_nRpFlag - 97) + 10;
    }
    else if(pRouteInput->m_nsPosInfo[4].m_nRpFlag >= 'h' && pRouteInput->m_nsPosInfo[4].m_nRpFlag <= 'j')
    {   //방향성 고려 근접링크로 16으로 통일
        pRouteInput->m_nsPosInfo[4].m_nRpFlag = 'g';
        sRequest.destination.destRpFlag = (pRouteInput->m_nsPosInfo[4].m_nRpFlag - 97) + 10;
    }
    else if(pRouteInput->m_nsPosInfo[4].m_nRpFlag >= 'k' && pRouteInput->m_nsPosInfo[4].m_nRpFlag <= 'n')
    {
        //유사 RpFlag로 변환
        sRequest.destination.destRpFlag = (pRouteInput->m_nsPosInfo[4].m_nRpFlag - 97) + 10 - 8;
    }
	else
	{
		//서버 명칭검색에서는 RpFlag가 0~16으로 와서 그냥 넣어줌
		sRequest.destination.destRpFlag = pRouteInput->m_nsPosInfo[4].m_nRpFlag;
	}
	
	sRequest.destination.navSeq = (char*)(pBuffer + nPos);
	nPos += sprintf(sRequest.destination.navSeq, "%s", pRouteInput->m_nsPosInfo[4].m_NavSeq);
	nPos = BYTEALIGN_SIZE(nPos+1);

	NddsExternalLog(E_DEBUG, "nddsmodule :: in RouttingServer 8");
	/////////////
	// RouteAdditionalInfo

	switch(pRouteInput->nVechileType)
	{
	case 1:	sRequest.additionalInfo.tollCarType = CARTYPE_MEDIUMVAN; break;
	case 2:	sRequest.additionalInfo.tollCarType = CARTYPE_LARGEVAN; break;
	case 3:	sRequest.additionalInfo.tollCarType = CARTYPE_LARGETRUCK; break;
	case 4:	sRequest.additionalInfo.tollCarType = CARTYPE_SPECIALTRUCK; break;
	case 5:	sRequest.additionalInfo.tollCarType = CARTYPE_SMALLCAR; break;
	case 0:
	default:
			sRequest.additionalInfo.tollCarType = CARTYPE_CAR; break;
	}

	switch(pRouteInput->nFuelType)
	{
	case 1:	sRequest.additionalInfo.carOilType = OIL_DIESEL; break;
	case 2:	sRequest.additionalInfo.carOilType = OIL_LPG; break;
	case 3:	sRequest.additionalInfo.carOilType = OIL_CHAdeMO; break;
	case 4:	sRequest.additionalInfo.carOilType = OIL_AC3; break;
	case 5:	sRequest.additionalInfo.carOilType = OIL_DCCombo; break;
	case 6:	sRequest.additionalInfo.carOilType = OIL_PremiumGasoline; break;
	case 0:
	default:
		sRequest.additionalInfo.carOilType = OIL_GASOLINE; break;
	}
	
	NddsExternalLog(E_DEBUG, "nddsmodule :: in RouttingServer 9");
	sRequest.additionalInfo.countAddCameraTypes = 0;
	memset(&sRequest.additionalInfo.addCameraTypes, 0x00, sizeof(sRequest.additionalInfo.addCameraTypes));
	if(pRouteInput->m_bpAddCameraType[1])
	{
		sRequest.additionalInfo.addCameraTypes[sRequest.additionalInfo.countAddCameraTypes] = CAMERA_BUS;
		sRequest.additionalInfo.countAddCameraTypes++;
	}

	if(pRouteInput->m_bpAddCameraType[2])
	{
		sRequest.additionalInfo.addCameraTypes[sRequest.additionalInfo.countAddCameraTypes] = CAMERA_SIGNALSPEEDING;
		sRequest.additionalInfo.countAddCameraTypes++;
	}

	if(pRouteInput->m_bpAddCameraType[3])
	{
		sRequest.additionalInfo.addCameraTypes[sRequest.additionalInfo.countAddCameraTypes] = CAMERA_MOVING;
		sRequest.additionalInfo.countAddCameraTypes++;
	}

	if(pRouteInput->m_bpAddCameraType[4])
	{
		sRequest.additionalInfo.addCameraTypes[sRequest.additionalInfo.countAddCameraTypes] = CAMERA_SHOULDERCONTROL;
		sRequest.additionalInfo.countAddCameraTypes++;
	}
/*
	if(pRouteInput->m_bpAddCameraType[5])
	{
		sRequest.additionalInfo.addCameraTypes[sRequest.additionalInfo.countAddCameraTypes] = CAMERA_CHANGEABLEROAD;
		sRequest.additionalInfo.countAddCameraTypes++;
	}

	if(pRouteInput->m_bpAddCameraType[6])
	{
		sRequest.additionalInfo.addCameraTypes[sRequest.additionalInfo.countAddCameraTypes] = CAMERA_OVERLOADED;
		sRequest.additionalInfo.countAddCameraTypes++;
	}

	if(pRouteInput->m_bpAddCameraType[7])
	{
		sRequest.additionalInfo.addCameraTypes[sRequest.additionalInfo.countAddCameraTypes] = CAMERA_SECURITY;
		sRequest.additionalInfo.countAddCameraTypes++;
	}
*/
	if(pRouteInput->m_bpAddCameraType[8])
	{
		sRequest.additionalInfo.addCameraTypes[sRequest.additionalInfo.countAddCameraTypes] = CAMERA_TRAFFIC;
		sRequest.additionalInfo.countAddCameraTypes++;
	}

	sRequest.additionalInfo.countFareWeightOpts = 1;
	sRequest.additionalInfo.fareWeightOpts = &fareWeightOpts;
	sRequest.additionalInfo.controlRouteReqFlag = pRouteInput->m_nControlRouteReqFlag;
	sRequest.additionalInfo.dynamicRoutePlanSrchFlag = pRouteInput->m_nEstRouteFlag;
	sRequest.additionalInfo.hipassFlag = pRouteInput->m_bHipass;

	////////////////////
	// List<RouteWayPointInfo> wayPoints;

	int wayPointsSize = pRouteInput->m_nPosInfoCnt - 2;

	sRequest.countWayPoints = wayPointsSize;
	NddsExternalLog(E_DEBUG, "nddsmodule :: in RouttingServer 10");
	if(wayPointsSize > 0)
	{
		NddsExternalLog(E_DEBUG, "nddsmodule :: in RouttingServer 10-1");
		sRequest.wayPoints = (NDDSTMAP_WAYPOINTS_DTO*)(pBuffer + nPos);
		nPos += sizeof(NDDSTMAP_WAYPOINTS_DTO)*wayPointsSize;
		nPos = BYTEALIGN_SIZE(nPos);

		NddsExternalLog(E_DEBUG, "nddsmodule :: in RouttingServer 10-2");
		for(i = 0 ; i < wayPointsSize ; i++)
		{
			sRequest.wayPoints[i].x = (int)(pRouteInput->m_nsPosInfo[i+1].m_wptX / (256*2048.0) * 36000.0f + 0.5f);
			sRequest.wayPoints[i].y = (int)(pRouteInput->m_nsPosInfo[i+1].m_wptY / (256*2048.0) * 36000.0f + 0.5f);
			if(pRouteInput->m_nsPosInfo[i+1].m_nPOIID > 0)
			{
				sRequest.wayPoints[i].poiID = (char*)(pBuffer + nPos);
				nPos += sprintf(sRequest.wayPoints[i].poiID, "%d", pRouteInput->m_nsPosInfo[i+1].m_nPOIID);
				nPos = BYTEALIGN_SIZE(nPos+1);
			}
			//로컬에서는 RpFlag를 문자로 0~9, A~G까지 사용하지만 NDDS서버에서는 아스키코드의 10진수 변환값 0~16까지 사용함
			if(pRouteInput->m_nsPosInfo[i+1].m_nRpFlag >= '0' && pRouteInput->m_nsPosInfo[i+1].m_nRpFlag <= '9')
			{
				//0~9
				sRequest.wayPoints[i].rpFlag = pRouteInput->m_nsPosInfo[i+1].m_nRpFlag - 48;
			}
			else if(pRouteInput->m_nsPosInfo[i+1].m_nRpFlag >= 'A' && pRouteInput->m_nsPosInfo[i+1].m_nRpFlag <= 'G')
			{
				//10~16
				sRequest.wayPoints[i].rpFlag = (pRouteInput->m_nsPosInfo[i+1].m_nRpFlag - 65) + 10;
			}
            else if(pRouteInput->m_nsPosInfo[i+1].m_nRpFlag >= 'H' && pRouteInput->m_nsPosInfo[i+1].m_nRpFlag <= 'J')
            {   //방향성 고려 근접링크로 16으로 통일
                pRouteInput->m_nsPosInfo[i+1].m_nRpFlag = 'G';
                sRequest.wayPoints[i].rpFlag = (pRouteInput->m_nsPosInfo[i+1].m_nRpFlag - 65) + 10;
            }
            else if(pRouteInput->m_nsPosInfo[i+1].m_nRpFlag >= 'K' && pRouteInput->m_nsPosInfo[i+1].m_nRpFlag <= 'N')
            {
                //유사 RpFlag로 변환
                 sRequest.wayPoints[i].rpFlag = (pRouteInput->m_nsPosInfo[i+1].m_nRpFlag - 65) + 10 - 8;
            }
            else if(pRouteInput->m_nsPosInfo[i+1].m_nRpFlag >= 'a' && pRouteInput->m_nsPosInfo[i+1].m_nRpFlag <= 'g')
            {
                //10~16
                sRequest.wayPoints[i].rpFlag = (pRouteInput->m_nsPosInfo[i+1].m_nRpFlag - 97) + 10;
            }            
            else if(pRouteInput->m_nsPosInfo[4].m_nRpFlag >= 'h' && pRouteInput->m_nsPosInfo[4].m_nRpFlag <= 'j')
            {   //방향성 고려 근접링크로 16으로 통일
                pRouteInput->m_nsPosInfo[i+1].m_nRpFlag = 'g';
                sRequest.wayPoints[i].rpFlag = (pRouteInput->m_nsPosInfo[i+1].m_nRpFlag - 97) + 10;
            }
            else if(pRouteInput->m_nsPosInfo[i+1].m_nRpFlag >= 'k' && pRouteInput->m_nsPosInfo[i+1].m_nRpFlag <= 'n')
            {
                //유사 RpFlag로 변환
                sRequest.wayPoints[i].rpFlag = (pRouteInput->m_nsPosInfo[i+1].m_nRpFlag - 97) + 10 - 8;
            }
			else
			{
				//서버 명칭검색에서는 RpFlag가 0~16으로 와서 그냥 넣어줌
				sRequest.wayPoints[i].rpFlag = pRouteInput->m_nsPosInfo[i+1].m_nRpFlag;
			}
        }
		NddsExternalLog(E_DEBUG, "nddsmodule :: in RouttingServer 10-3");
	}

	// tvasReSearch
	NddsExternalLog(E_DEBUG, "nddsmodule :: in RouttingServer 11");
	if(sRequest.ReSearchType == REQ_REROUTE)
	{
		NddsExternalLog(E_DEBUG, "nddsmodule :: in RouttingServer 11-1");
		int nData = pRouteInput->m_nsReRouteInfo.nRouteDataSize;
		if(nData > 0)
		{
			NddsExternalLog(E_DEBUG, "nddsmodule :: in RouttingServer 11-2");
            if(m_pszSearchCondData != NULL)
            {
				NddsExternalLog(E_DEBUG, "nddsmodule :: in RouttingServer 11-3");
                free(m_pszSearchCondData);
                m_pszSearchCondData = NULL;
            }

			NddsExternalLog(E_DEBUG, "nddsmodule :: in RouttingServer 11-4");
            m_pszSearchCondData = (char*)malloc(sizeof(char)*(nData+1));
            memset(m_pszSearchCondData, 0x00, nData+1);
            strncpy(m_pszSearchCondData, pRouteInput->m_nsReRouteInfo.pszRouteData, nData);
            sRequest.tvasReSearch.searchCondData = m_pszSearchCondData;
		}
		
		sRequest.tvasReSearch.lastTollgateId = pRouteInput->m_nsReRouteInfo.uPassTollID;
		sRequest.tvasReSearch.radiusInfoType = 0;

		NddsExternalLog(E_DEBUG, "nddsmodule :: in RouttingServer 11-5");
		if(pRouteInput->m_nsReRouteInfo.bAroundLink)
		{
			NddsExternalLog(E_DEBUG, "nddsmodule :: in RouttingServer 11-6");
			sRequest.tvasReSearch.radiusInfoType = 1;
			sRequest.tvasReSearch.radiusEntryTurnType = pRouteInput->m_nsReRouteInfo.uRsdCode;
			sRequest.tvasReSearch.radiusLinkId = pRouteInput->m_nsReRouteInfo.uLinkID;
			sRequest.tvasReSearch.radiusMeshCode = pRouteInput->m_nsReRouteInfo.uTileCode;
			sRequest.tvasReSearch.radiusLinkDirection = pRouteInput->m_nsReRouteInfo.uDirInfo;
			sRequest.tvasReSearch.radiusLeaveTurnType = pRouteInput->m_nsReRouteInfo.uOldRsdCode;
			sRequest.tvasReSearch.radiusXPos = pRouteInput->m_nsReRouteInfo.uPosX;
			sRequest.tvasReSearch.radiusYPos = pRouteInput->m_nsReRouteInfo.uPosY;
		}

		sRequest.tvasReSearch.lastRid = pRouteInput->m_nsReRouteInfo.nLastRid;
		sRequest.tvasReSearch.tcRid = pRouteInput->m_nsReRouteInfo.nTcRid;

		sRequest.tvasReSearch.preMapVersion = (char*)(pBuffer + nPos);
		nPos += sprintf(sRequest.tvasReSearch.preMapVersion, "%s", pRouteInput->m_nsReRouteInfo.szPreMapVersion);
		nPos = BYTEALIGN_SIZE(nPos+1);

		NddsExternalLog(E_DEBUG, "nddsmodule :: in RouttingServer 11-7");
		if(pRouteInput->m_nsReRouteInfo.pnPreRids != NULL)
		{
			NddsExternalLog(E_DEBUG, "nddsmodule :: in RouttingServer 11-8");
			if(m_pnPreRids != NULL)
			{
				NddsExternalLog(E_DEBUG, "nddsmodule :: in RouttingServer 11-9");
				free(m_pnPreRids);
                m_pnPreRids = NULL;
			}
			
			NddsExternalLog(E_DEBUG, "nddsmodule :: in RouttingServer 11-10");
			m_pnPreRids = (int*)malloc(sizeof(int)*pRouteInput->m_nsReRouteInfo.nPreRidsCnt);
			for(int i=0;i<pRouteInput->m_nsReRouteInfo.nPreRidsCnt;i++)
			{
				m_pnPreRids[i] = pRouteInput->m_nsReRouteInfo.pnPreRids[i]; 
			}
			sRequest.tvasReSearch.preRidsCnt = pRouteInput->m_nsReRouteInfo.nPreRidsCnt;
			sRequest.tvasReSearch.preRids = m_pnPreRids;
		}

		NddsExternalLog(E_DEBUG, "nddsmodule :: in RouttingServer 11-11");
		if(pRouteInput->m_nsReRouteInfo.pnPreSecs != NULL)
		{
			NddsExternalLog(E_DEBUG, "nddsmodule :: in RouttingServer 11-12");
			if(m_pnPreSecs != NULL)
			{
				free(m_pnPreSecs);
                m_pnPreSecs = NULL;
			}

			NddsExternalLog(E_DEBUG, "nddsmodule :: in RouttingServer 11-14");
			m_pnPreSecs = (int*)malloc(sizeof(int)*pRouteInput->m_nsReRouteInfo.nPreSecsCnt);

			for(int i=0;i<pRouteInput->m_nsReRouteInfo.nPreSecsCnt;i++)
			{
				m_pnPreSecs[i] = pRouteInput->m_nsReRouteInfo.pnPreSecs[i]; 
			}
			sRequest.tvasReSearch.preSecsCnt = pRouteInput->m_nsReRouteInfo.nPreSecsCnt;
			sRequest.tvasReSearch.preSecs = m_pnPreSecs;
		}

		NddsExternalLog(E_DEBUG, "nddsmodule :: in RouttingServer 11-15");
		if(pRouteInput->m_nsReRouteInfo.szPreTollgateIds != NULL)
		{
			NddsExternalLog(E_DEBUG, "nddsmodule :: in RouttingServer 11-16");
			sRequest.tvasReSearch.preTollgateIds = (char*)(pBuffer + nPos);
			nPos += sprintf(sRequest.tvasReSearch.preTollgateIds, "%s", pRouteInput->m_nsReRouteInfo.szPreTollgateIds);
			nPos = BYTEALIGN_SIZE(nPos+1);
		}

		sRequest.tvasReSearch.initSrchLength = pRouteInput->m_nsReRouteInfo.nInitSrchLength;

		NddsExternalLog(E_DEBUG, "nddsmodule :: in RouttingServer 11-17");
		if(pRouteInput->m_nsReRouteInfo.szInitSrchSessionId != NULL)
		{
			NddsExternalLog(E_DEBUG, "nddsmodule :: in RouttingServer 11-18");
			sRequest.tvasReSearch.initSrchSessionId = (char*)(pBuffer + nPos);
			nPos += sprintf(sRequest.tvasReSearch.initSrchSessionId, "%s", pRouteInput->m_nsReRouteInfo.szInitSrchSessionId);
			nPos = BYTEALIGN_SIZE(nPos+1);
		}
    } else {
        // 일반탐색 시에도 필요한 정보(TG요금 계산)
        sRequest.tvasReSearch.lastTollgateId = pRouteInput->m_nsReRouteInfo.uPassTollID;
        if(pRouteInput->m_nsReRouteInfo.szPreTollgateIds != NULL)
        {
            sRequest.tvasReSearch.preTollgateIds = (char*)(pBuffer + nPos);
            nPos += sprintf(sRequest.tvasReSearch.preTollgateIds, "%s", pRouteInput->m_nsReRouteInfo.szPreTollgateIds);
            nPos = BYTEALIGN_SIZE(nPos+1);
        }
    }
	NddsExternalLog(E_DEBUG, "nddsmodule :: in RouttingServer 12");
    if(pRouteInput->m_nsPassPosInfo.szPastSessionId != NULL)
    {
        sRequest.pastPositionInfoDto.pastSessionId = (char*)(pBuffer + nPos);
        nPos += sprintf(sRequest.pastPositionInfoDto.pastSessionId, "%s", pRouteInput->m_nsPassPosInfo.szPastSessionId);
        nPos = BYTEALIGN_SIZE(nPos+1);
    }

    sRequest.pastPositionInfoDto.pastXpos = (int)(pRouteInput->m_nsPassPosInfo.nPastXpos / (256*2048.0) * 36000.0f + 0.5f);
    sRequest.pastPositionInfoDto.pastYpos = (int)(pRouteInput->m_nsPassPosInfo.nPastYpos / (256*2048.0) * 36000.0f + 0.5f);

    if(pRouteInput->m_nsPassPosInfo.szPastArrivalTime != NULL)
    {
        sRequest.pastPositionInfoDto.pastArrivalTime = (char*)(pBuffer + nPos);
        nPos += sprintf(sRequest.pastPositionInfoDto.pastArrivalTime, "%s", pRouteInput->m_nsPassPosInfo.szPastArrivalTime);
        nPos = BYTEALIGN_SIZE(nPos+1);
    }

	if(pRouteInput->m_nEstRouteType == 1)
		sRequest.serviceFlag = ARRIVALTIMEESTIMATION;
	else if(pRouteInput->m_nEstRouteType == 2)
		sRequest.serviceFlag = DEPARTURETIMEESTIMATION;
	else 
		sRequest.serviceFlag = REALTIME;

	if(pRouteInput->m_nEstRouteType != 0)
	{
		sRequest.patternTime = (char*)(pBuffer + nPos);
		nPos += sprintf(sRequest.patternTime, "%02d%02d%02d", pRouteInput->m_ReqTime.Hour, pRouteInput->m_ReqTime.Minute, pRouteInput->m_ReqTime.Second);
		nPos = BYTEALIGN_SIZE(nPos);
	}
	sRequest.patternWeek = (ENDDS_PATTERNWEEK)(pRouteInput->m_ReqTime.Week+1);

    if(pRouteInput->m_GuideImageCode[0] != 0x00)
        strcpy(sRequest.guideImgResolutionCode, pRouteInput->m_GuideImageCode);

    sRequest.countdangerAreaOptions = 0;
    memset(&sRequest.dangerAreaOptions, 0x00, sizeof(sRequest.dangerAreaOptions));
	if(pRouteInput->m_bpAddDAType[1])
	{
		sRequest.dangerAreaOptions[sRequest.countdangerAreaOptions] = OCCURFREQUENTLY;
		sRequest.countdangerAreaOptions++;
	}

	if(pRouteInput->m_bpAddDAType[2])
	{
		sRequest.dangerAreaOptions[sRequest.countdangerAreaOptions] = SHARPCURVESECTION;
		sRequest.countdangerAreaOptions++;
	}
/*
	if(pRouteInput->m_bpAddDAType[3])
	{
		sRequest.dangerAreaOptions[sRequest.countdangerAreaOptions] = STEEPSLOPE;
		sRequest.countdangerAreaOptions++;
	}

	if(pRouteInput->m_bpAddDAType[4])
	{
		sRequest.dangerAreaOptions[sRequest.countdangerAreaOptions] = ROADKILL;
		sRequest.countdangerAreaOptions++;
	}
*/
	if(pRouteInput->m_bpAddDAType[5])
	{
		sRequest.dangerAreaOptions[sRequest.countdangerAreaOptions] = MISTAREA;
		sRequest.countdangerAreaOptions++;
	}
/*
	if(pRouteInput->m_bpAddDAType[6])//낙석 사용안함
	{
		sRequest.dangerAreaOptions[sRequest.countdangerAreaOptions] = MISTAREA;
		sRequest.countdangerAreaOptions++;
	}
*/

	if(pRouteInput->m_bpAddDAType[7])
	{
		sRequest.dangerAreaOptions[sRequest.countdangerAreaOptions] = SCHOOLZONE;
		sRequest.countdangerAreaOptions++;
	}

	if(pRouteInput->m_bpAddDAType[8])
	{
		sRequest.dangerAreaOptions[sRequest.countdangerAreaOptions] = TRAINCROSSING;
		sRequest.countdangerAreaOptions++;
	}

	sRequest.firstGuideOption = sRequest.common.routePlanTypes[0];
	sRequest.routePlanAroundRange = 1;

	//////////////////////////////////////

	NddsExternalLog(E_DEBUG, "nddsmodule :: in RouttingServer 13");
	// 사용자 취소 로직 추후 추가 예정
	if(m_bStopServerRoute)
	{
		NddsExternalLog(E_DEBUG, "nddsmodule :: in RouttingServer 13-1");
		strcpy(szErrorMsg, "사용자 취소");
		*nErrorCode = NDDSERR_USERCANCEL;
        return 0;
	}

	NddsExternalLog(E_DEBUG, "nddsmodule :: in RouttingServer 14 :: NDDSLIB_RequestProtocol before :: eRequestType = %d, nSessionID = %d", eRequestType, nSessionID);
	if(!NDDSLIB_RequestProtocol(NDDSTYPE_TMAP_PLANNINGROUTE, eRequestType, (NDDS_COMMON_REQ*)(&sRequest), nSessionID, &eErrCode))
	{
		NddsExternalLog(E_DEBUG, "nddsmodule :: in RouttingServer 14-1 :: NDDSLIB_RequestProtocol eErrCode = %d", eErrCode);
		if(eErrCode == NDDSERR_USERCANCEL)
			strcpy(szErrorMsg, "사용자 취소");
		else
			strcpy(szErrorMsg, "데이터 생성 실패");

		*nErrorCode = eErrCode;
        return 0;
	}
	NddsExternalLog(E_DEBUG, "nddsmodule :: in RouttingServer 15 :: NDDSLIB_RequestProtocol after");

    if(eRequestType == NDDSREQUEST_ATONCE)
    {
		TsTvasFileInfo stTvsFileInfo={0,};
		stTvsFileInfo.nSessionID = nSessionID;
        stTvsFileInfo.eRouteType = pRouteInput->eRouteType;
		memcpy(stTvsFileInfo.sTvasPath, pRouteOutput->szCollectPath,sizeof(pRouteOutput->szCollectPath));
		m_listTvasInfo.push_back(stTvsFileInfo);
        return 9;
	}

	eNddsStatus eStatus = NDDSSTATUS_WAIT;
	int nProgress = 0;
    //DBGPRINT("Wait ");
	unsigned long reqTime = GetTickCount();
	NddsExternalLog(E_DEBUG, "nddsmodule :: in RouttingServer 16 :: reqTime = %d", reqTime);
	while(eStatus != NDDSSTATUS_COMPLETE && eStatus != NDDSSTATUS_CANCELED)
	{
		NddsExternalLog(E_DEBUG, "nddsmodule :: in RouttingServer 16-1 :: eStatus = %d", eStatus);
        //DBGPRINT(".");
		unsigned long thisTime = GetTickCount();
		unsigned long wattingTime = thisTime - reqTime;

        if(wattingTime > RP_WAITTING_TIME)
		{
			NddsExternalLog(E_DEBUG, "nddsmodule :: in RouttingServer 16-2 :: reqTime = %d, wattingTime = %d", reqTime, wattingTime);
			NDDSLIB_CancelProtocol(nSessionID);
			strcpy(szErrorMsg, "서버 응답 없음");
			*nErrorCode = NDDSERR_TIMEOUT;
            return 0;
		}

		// 사용자 취소 로직 추후 추가 예정
		if(m_bStopServerRoute)
		{
			NddsExternalLog(E_DEBUG, "nddsmodule :: in RouttingServer 16-3 :: reqTime = %d, wattingTime = %d", reqTime, wattingTime);
			NDDSLIB_CancelProtocol(nSessionID);
			strcpy(szErrorMsg, "사용자 취소");
			*nErrorCode = NDDSERR_USERCANCEL;
            return 0;
		}
		delay(100);

		NddsExternalLog(E_DEBUG, "nddsmodule :: in RouttingServer 16-4 :: eStatus = %d", eStatus);
		NDDSLIB_GetStatusProtocol(nSessionID, &eStatus, &nProgress, &eErrCode);
		NddsExternalLog(E_DEBUG, "nddsmodule :: in RouttingServer 16-5 :: eStatus = %d, eErrCode = %d", eStatus, eErrCode);
	}
    //DBGPRINT(_T("\n"));

	NDDS_COMMON_RES *pRes = NULL;
	NDDSTMAP_PLANNINGROUTE_RES *pResStruct = NULL;
	eNddsProtocolType eResProtocolType = NDDSTYPE_NONE;
	NddsExternalLog(E_DEBUG, "nddsmodule :: in RouttingServer 17 :: NDDSLIB_GetResultProtocol before :: eStatus = %d", eStatus);
	NDDSLIB_GetResultProtocol(nSessionID, true, &eResProtocolType, &pRes, &eErrCode);
	NddsExternalLog(E_DEBUG, "nddsmodule :: in RouttingServer 18 :: NDDSLIB_GetResultProtocol after :: eErrCode = %d", eErrCode);
	pResStruct = (NDDSTMAP_PLANNINGROUTE_RES*)pRes;

   if(pResStruct != NULL)
	{
		IsChangeAccesskey(nSessionID, pResStruct->header.accessKey);
		NddsExternalLog(E_DEBUG, "nddsmodule :: in RouttingServer 18-1 :: CheckErrorCode before :: header.errorCode = %d", pResStruct->header.errorCode);
		CheckErrorCode(nSessionID, pResStruct->header.errorCode);
		NddsExternalLog(E_DEBUG, "nddsmodule :: in RouttingServer 18-2 :: CheckErrorCode after :: header.errorCode = %d", pResStruct->header.errorCode);
	}

   NddsExternalLog(E_DEBUG, "nddsmodule :: in RouttingServer 19 :: NddsErrorCheck before :: eErrCode = %d", eErrCode);
	if(!NddsErrorCheck(eErrCode, pResStruct?pResStruct->header.errorMessage:NULL, szErrorMsg))
	{
		NddsExternalLog(E_DEBUG, "nddsmodule :: in RouttingServer 19-1 :: NDDSLIB_ReleaseObject before :: eErrCode = %d", eErrCode);
        NDDSLIB_ReleaseObject(pRes, eErrCode);
		NddsExternalLog(E_DEBUG, "nddsmodule :: in RouttingServer 19-1 :: NDDSLIB_ReleaseObject after :: eErrCode = %d", eErrCode);
		*nErrorCode = eErrCode;
		return 0;
	}

	NddsExternalLog(E_DEBUG, "nddsmodule :: in RouttingServer 20 :: pResStruct NULL Check before :: eErrCode = %d", eErrCode);
	if (pResStruct == NULL)
	{
		NddsExternalLog(E_DEBUG, "nddsmodule :: in RouttingServer 20-1 :: pResStruct is NULL :: eErrCode = %d", eErrCode);
		*nErrorCode = NDDSERR_RESULT_NULL;
		NddsExternalLog(E_DEBUG, "nddsmodule :: in RouttingServer 20-2 :: pResStruct is NULL :: *nErrorCode = %d", *nErrorCode);
        return 0;
	}

	int nRouteCnt = 0;

    for(i=0; i<pResStruct->routeCount; i++)
	{
		switch(pResStruct->routePlanTypes[i])
		{
		case 0:
			pFileInfo->m_nRouteOption[i] = 0x0001;
			break;
		case 1:
			pFileInfo->m_nRouteOption[i] = 0x0002;
			break;
		case 4:
			pFileInfo->m_nRouteOption[i] = 0x0010;
			break;
		case 10:
		default:
			pFileInfo->m_nRouteOption[i] = 0x0400;
			break;
		}

		pRouteOutput->nSearchOption[i] = pFileInfo->m_nRouteOption[i];
        strncpy(pRouteOutput->szSessionID, pResStruct->header.sessionId, 24);
		if(strlen(pRouteOutput->szCollectPath[i]) > 0)
		{
			strcpy(pFileInfo->m_sTvasPath[i], pRouteOutput->szCollectPath[i]);

			FILE *fpTvas = NULL;
			fpTvas = fopen(pFileInfo->m_sTvasPath[i], "wb"); // WRITE
			fwrite(pResStruct->tvases[i], pResStruct->tvasSize[i], 1, fpTvas);
			fclose(fpTvas);
			fpTvas = NULL;
#if 0
			
			{
				fpTvas = fopen(pFileInfo->m_sTvasPath[i], "rb");
				int size=0;
				fread(&size, 4, 1, fpTvas);
				char ver = 0;
				fread(&ver, 1, 1, fpTvas);
                if(((strcmp(pRouteInput->m_Tvas, "5.3") == 0) && (ver != 83))
                    || ((strcmp(pRouteInput->m_Tvas, "5.2") == 0) && (ver != 82))
                    || ((strcmp(pRouteInput->m_Tvas, "5.1") == 0) && (ver != 81))
                    || ((strcmp(pRouteInput->m_Tvas, "5.0") == 0) && (ver != 80))
					)
				{
					fclose(fpTvas);
					fpTvas = NULL;

					char szPath[256]={0,};
                    char szSessionID[25]={0,};
                    strncmp(szSessionID, pResStruct->header.sessionId, 24);
                    sprintf(szPath, "%s%s_%d_%s", pFileInfo->m_sTvasPath[i], pResStruct->header.sessionId,i,pRouteInput->m_Tvas);
					fpTvas = fopen(szPath, "wb"); // WRITE
					fwrite(pResStruct->tvases[i], pResStruct->tvasSize[i], 1, fpTvas);
					fclose(fpTvas);
					fpTvas = NULL;
				}

				if(fpTvas != NULL)
				{
					fclose(fpTvas);
					fpTvas = NULL;
				}
			}
#endif
            nRouteCnt++;
		}

		pRouteOutput->nCollectSize[i] = pResStruct->tvasSize[i];
		pRouteOutput->pCollectData[i] = (unsigned char *) malloc(pResStruct->tvasSize[i]);
		memcpy(pRouteOutput->pCollectData[i], pResStruct->tvases[i], pResStruct->tvasSize[i]);
	}

	strcpy(m_pNddsWeatherInfo.m_SkyCode, pResStruct->destination.skyCode);
	strcpy(m_pNddsWeatherInfo.m_SkyName, pResStruct->destination.skyName);
	strcpy(m_pNddsWeatherInfo.m_RainTypeCode, pResStruct->destination.rainTypeCode);
	strcpy(m_pNddsWeatherInfo.m_RainSinceOnTime, pResStruct->destination.rainSinceOnTime);
	strcpy(m_pNddsWeatherInfo.m_TempC1h, pResStruct->destination.tempC1h);

    NDDSLIB_ReleaseObject(pRes);

	NddsExternalLog(E_DEBUG, "nddsmodule :: in RouttingServer 21 :: finish :: nRouteCnt = %d", nRouteCnt);
    return nRouteCnt;
}

bool CNddsModule::RouttingSummaryServer(int nSessionID, eNddsRequestType eRequestType, NstRouteSummaryReqLogic *pRouteSummaryInput, char *szErrorMsg,int *nErrorCode)
{
	eNddsErrorCode eErrCode = NDDSERR_UNKNOWN;

	ENDDS_FAREWEIGHTOPTS fareWeightOpts = LOGICAPPLIED;

	unsigned char pBuffer[ROUTE_BUFFER] = {0,};
	int nPos = 0, i;

	NDDSTMAP_ROUTESUMMARYINFO_REQ sRequest;
	memset(&sRequest, 0x00, sizeof(NDDSTMAP_ROUTESUMMARYINFO_REQ));

	sRequest.firstGuideOption = pRouteSummaryInput->m_nRouteSummaryType;
	sRequest.serviceFlag = pRouteSummaryInput->m_nRouteServiceType;

	sRequest.commingTime = (char*)(pBuffer + nPos);
	nPos += sprintf(sRequest.commingTime, "%04d%02d%02d%02d%02d%02d", pRouteSummaryInput->m_ReqTime.Year, pRouteSummaryInput->m_ReqTime.Month, pRouteSummaryInput->m_ReqTime.Day, pRouteSummaryInput->m_ReqTime.Hour, pRouteSummaryInput->m_ReqTime.Minute, pRouteSummaryInput->m_ReqTime.Second);
	nPos = BYTEALIGN_SIZE(nPos+1);

	sRequest.vertexFlag = pRouteSummaryInput->m_nVertexFlag;

	sRequest.departName = (char*)(pBuffer + nPos);
	nPos += sprintf(sRequest.departName, "%s", pRouteSummaryInput->m_nsPosInfo[0].m_strName);
	nPos = BYTEALIGN_SIZE(nPos+1);
	
	sRequest.departXPos = (int)(pRouteSummaryInput->m_nsPosInfo[0].m_wptX / (256*2048.0) * 36000.0f + 0.5f);
	sRequest.departYPos = (int)(pRouteSummaryInput->m_nsPosInfo[0].m_wptY / (256*2048.0) * 36000.0f + 0.5f);
	sRequest.angle = pRouteSummaryInput->m_nAngle;
	sRequest.speed = pRouteSummaryInput->m_nSpeed;
	sRequest.departDirPriority = pRouteSummaryInput->m_DepartDirPriority;
	sRequest.departSrchFlag = pRouteSummaryInput->m_DepartSrchFlag;

	/////////////
	// RouteDestinationInfo
    sRequest.destSearchFlag = pRouteSummaryInput->m_DestSrchFlag;

	sRequest.destName = (char*)(pBuffer + nPos);
	nPos += sprintf(sRequest.destName, "%s", pRouteSummaryInput->m_nsPosInfo[4].m_strName);
	nPos = BYTEALIGN_SIZE(nPos+1);

	sRequest.destXPos = (int)(pRouteSummaryInput->m_nsPosInfo[4].m_wptX / (256*2048.0) * 36000.0f + 0.5f);
	sRequest.destYPos = (int)(pRouteSummaryInput->m_nsPosInfo[4].m_wptY / (256*2048.0) * 36000.0f + 0.5f);
	if(pRouteSummaryInput->m_nsPosInfo[4].m_nPOIID > 0)
	{
		sRequest.destPoiId = (char*)(pBuffer + nPos);
		nPos += sprintf(sRequest.destPoiId, "%d", pRouteSummaryInput->m_nsPosInfo[4].m_nPOIID);
		nPos = BYTEALIGN_SIZE(nPos+1);
	}

    //로컬에서는 RpFlag를 문자로 0~9, A~G까지 사용하지만 NDDS서버에서는 아스키코드의 10진수 변환값 0~16까지 사용함
    if(pRouteSummaryInput->m_nsPosInfo[4].m_nRpFlag >= '0' && pRouteSummaryInput->m_nsPosInfo[4].m_nRpFlag <= '9')
    {
        //0~9
        sRequest.destRpFlag = pRouteSummaryInput->m_nsPosInfo[4].m_nRpFlag - 48;
    }
    else if(pRouteSummaryInput->m_nsPosInfo[4].m_nRpFlag >= 'A' && pRouteSummaryInput->m_nsPosInfo[4].m_nRpFlag <= 'G')
    {
        //10~16
        sRequest.destRpFlag = (pRouteSummaryInput->m_nsPosInfo[4].m_nRpFlag - 65) + 10;
    }
    else if(pRouteSummaryInput->m_nsPosInfo[4].m_nRpFlag >= 'H' && pRouteSummaryInput->m_nsPosInfo[4].m_nRpFlag <= 'J')
    {   //방향성 고려 근접링크로 16으로 통일
          pRouteSummaryInput->m_nsPosInfo[4].m_nRpFlag = 'G';
          sRequest.destRpFlag = (pRouteSummaryInput->m_nsPosInfo[4].m_nRpFlag - 65) + 10;
    }
    else if(pRouteSummaryInput->m_nsPosInfo[4].m_nRpFlag >= 'K' && pRouteSummaryInput->m_nsPosInfo[4].m_nRpFlag <= 'N')
    {
        //유사 RpFlag로 변환
        sRequest.destRpFlag = (pRouteSummaryInput->m_nsPosInfo[4].m_nRpFlag - 65) + 10 - 8;
    }
    else if(pRouteSummaryInput->m_nsPosInfo[4].m_nRpFlag >= 'a' && pRouteSummaryInput->m_nsPosInfo[4].m_nRpFlag <= 'g')
    {
        //10~16
        sRequest.destRpFlag = (pRouteSummaryInput->m_nsPosInfo[4].m_nRpFlag - 97) + 10;
    }
    else if(pRouteSummaryInput->m_nsPosInfo[4].m_nRpFlag >= 'h' && pRouteSummaryInput->m_nsPosInfo[4].m_nRpFlag <= 'j')
    {   //방향성 고려 근접링크로 16으로 통일
        pRouteSummaryInput->m_nsPosInfo[4].m_nRpFlag = 'g';
        sRequest.destRpFlag = (pRouteSummaryInput->m_nsPosInfo[4].m_nRpFlag - 97) + 10;
    }
    else if(pRouteSummaryInput->m_nsPosInfo[4].m_nRpFlag >= 'k' && pRouteSummaryInput->m_nsPosInfo[4].m_nRpFlag <= 'n')
    {
        //유사 RpFlag로 변환
        sRequest.destRpFlag = (pRouteSummaryInput->m_nsPosInfo[4].m_nRpFlag - 97) + 10 - 8;
    }
    else
    {
        //서버 명칭검색에서는 RpFlag가 0~16으로 와서 그냥 넣어줌
        sRequest.destRpFlag = pRouteSummaryInput->m_nsPosInfo[4].m_nRpFlag;
    }


	////////////////////
	// List<RouteWayPointInfo> wayPoints;

	int wayPointsSize = pRouteSummaryInput->m_nPosInfoCnt - 2;

	sRequest.nWayPoints = wayPointsSize;
	if(wayPointsSize > 0)
	{
		sRequest.wayPoints = (NDDSTMAP_WAYPOINTS_DTO*)(pBuffer + nPos);
		nPos += sizeof(NDDSTMAP_WAYPOINTS_DTO)*wayPointsSize;
		nPos = BYTEALIGN_SIZE(nPos);

		for(i = 0 ; i < wayPointsSize ; i++)
		{
			sRequest.wayPoints[i].x = (int)(pRouteSummaryInput->m_nsPosInfo[i+1].m_wptX / (256*2048.0) * 36000.0f + 0.5f);
			sRequest.wayPoints[i].y = (int)(pRouteSummaryInput->m_nsPosInfo[i+1].m_wptY / (256*2048.0) * 36000.0f + 0.5f);
			if(pRouteSummaryInput->m_nsPosInfo[i+1].m_nPOIID > 0)
			{
				sRequest.wayPoints[i].poiID = (char*)(pBuffer + nPos);
				nPos += sprintf(sRequest.wayPoints[i].poiID, "%d", pRouteSummaryInput->m_nsPosInfo[i+1].m_nPOIID);
				nPos = BYTEALIGN_SIZE(nPos+1);
			}
			//로컬에서는 RpFlag를 문자로 0~9, A~G까지 사용하지만 NDDS서버에서는 아스키코드의 10진수 변환값 0~16까지 사용함
			if(pRouteSummaryInput->m_nsPosInfo[i+1].m_nRpFlag >= '0' && pRouteSummaryInput->m_nsPosInfo[i+1].m_nRpFlag <= '9')
			{
				//0~9
				sRequest.wayPoints[i].rpFlag = pRouteSummaryInput->m_nsPosInfo[i+1].m_nRpFlag - 48;
			}
			else if(pRouteSummaryInput->m_nsPosInfo[i+1].m_nRpFlag >= 'A' && pRouteSummaryInput->m_nsPosInfo[i+1].m_nRpFlag <= 'G')
			{
				//10~16
				sRequest.wayPoints[i].rpFlag = (pRouteSummaryInput->m_nsPosInfo[i+1].m_nRpFlag - 65) + 10;
			}
			else
			{
				//서버 명칭검색에서는 RpFlag가 0~16으로 와서 그냥 넣어줌
				sRequest.wayPoints[i].rpFlag = pRouteSummaryInput->m_nsPosInfo[i+1].m_nRpFlag;
			}
		}
	}

	// 사용자 취소 로직 추후 추가 예정
	if(m_bStopServerRoute)
	{
		strcpy(szErrorMsg, "사용자 취소");
		*nErrorCode = NDDSERR_USERCANCEL;
		return false;
	}

	if(!NDDSLIB_RequestProtocol(NDDSTYPE_TMAP_ROUTESUMMARYINFO, eRequestType, (NDDS_COMMON_REQ*)(&sRequest), nSessionID, &eErrCode))
	{
		if(eErrCode == NDDSERR_USERCANCEL)
			strcpy(szErrorMsg, "사용자 취소");
		else
			strcpy(szErrorMsg, "데이터 생성 실패");

		*nErrorCode = eErrCode;
		return false;
	}

	if(eRequestType == NDDSREQUEST_ATONCE)
	{
		return true;
	}

	eNddsStatus eStatus = NDDSSTATUS_WAIT;
	int nProgress = 0;
	DBGPRINT(_T("Wait "));
	unsigned long reqTime = GetTickCount();
	while(eStatus != NDDSSTATUS_COMPLETE && eStatus != NDDSSTATUS_CANCELED)
	{
		DBGPRINT(_T("."));
		unsigned long thisTime = GetTickCount();
		unsigned long wattingTime = thisTime - reqTime;

        if(wattingTime > RP_WAITTING_TIME)
		{
			NDDSLIB_CancelProtocol(nSessionID);
			strcpy(szErrorMsg, "서버 응답 없음");
			*nErrorCode = NDDSERR_TIMEOUT;
			return false;
		}

		// 사용자 취소 로직 추후 추가 예정
		if(m_bStopServerRoute)
		{
			NDDSLIB_CancelProtocol(nSessionID);
			strcpy(szErrorMsg, "사용자 취소");
			*nErrorCode = NDDSERR_USERCANCEL;
			return false;
		}
		delay(100);

		NDDSLIB_GetStatusProtocol(nSessionID, &eStatus, &nProgress, &eErrCode);
	}
	DBGPRINT(_T("\n"));

	NDDS_COMMON_RES *pRes = NULL;
	NDDSTMAP_ROUTESUMMARYINFO_RES *pResStruct = NULL;
	eNddsProtocolType eResProtocolType = NDDSTYPE_NONE;
	NDDSLIB_GetResultProtocol(nSessionID, true, &eResProtocolType, &pRes, &eErrCode);
	pResStruct = (NDDSTMAP_ROUTESUMMARYINFO_RES*)pRes;

	if(pResStruct != NULL)
	{
		IsChangeAccesskey(nSessionID, pResStruct->header.accessKey);
		CheckErrorCode(nSessionID, pResStruct->header.errorCode);
	}

	if(!NddsErrorCheck(eErrCode, pResStruct?pResStruct->header.errorMessage:NULL, szErrorMsg))
	{
        NDDSLIB_ReleaseObject(pRes);
		*nErrorCode = eErrCode;
		return false;
	}

	if (pResStruct == NULL)
	{
		*nErrorCode = NDDSERR_RESULT_NULL;
		return false;
	}

	bool bResult = false;
	if(pResStruct != NULL)
	{
		if(m_nRouteSummaryIdx > 2)
		{
			m_nRouteSummaryIdx = 0;
		}

		int nIdx = m_nRouteSummaryIdx;

		m_pNddsRouteSummaryResult[nIdx].m_SessionID = nSessionID;
		m_pNddsRouteSummaryResult[nIdx].m_TotalTime = pResStruct->routeList[0].totalTime;
		m_pNddsRouteSummaryResult[nIdx].m_EstimationOption = pResStruct->routeList[0].estimationOption;
		if(pResStruct->routeList[0].estimationTime)
			strcpy(m_pNddsRouteSummaryResult[nIdx].m_EstimationTime, pResStruct->routeList[0].estimationTime);

		m_nRouteSummaryIdx++;

		bResult = true;
	}

    NDDSLIB_ReleaseObject(pRes);

	return bResult;
}

int CNddsModule::GetRouteResult(int nSessionID ,int *nErrorCode, NeNddsRouteType *eRouteType)
{
	eNddsErrorCode eErrCode = NDDSERR_UNKNOWN;
	char szErrorMsg[256] = {0,};

	NDDS_COMMON_RES *pRes = NULL;
	NDDSTMAP_PLANNINGROUTE_RES *pResStruct = NULL;
	eNddsProtocolType eResProtocolType = NDDSTYPE_NONE;
	NDDSLIB_GetResultProtocol(nSessionID, false, &eResProtocolType, &pRes, &eErrCode);
	pResStruct = (NDDSTMAP_PLANNINGROUTE_RES*)pRes;

	if(pResStruct != NULL)
	{
		IsChangeAccesskey(nSessionID, pResStruct->header.accessKey);
		CheckErrorCode(nSessionID, pResStruct->header.errorCode);
	}

	if(!NddsErrorCheck(eErrCode, pResStruct?pResStruct->header.errorMessage:NULL, szErrorMsg))
	{
        NDDSLIB_ReleaseObject(pRes, eErrCode);
		*nErrorCode = eErrCode;
		return false;
	}

	if (pResStruct == NULL)
	{
		*nErrorCode = NDDSERR_RESULT_NULL;
		return false;
	}

	int nRouteCnt = 0;

	std::list<TsTvasFileInfo>::iterator itor;

	for(itor = m_listTvasInfo.begin() ; itor != m_listTvasInfo.end() ; itor++)
	{
		if(nSessionID == (itor)->nSessionID)
		{
            *eRouteType = (itor)->eRouteType;
            for(int i=0; i<pResStruct->routeCount; i++)
			{
				if(strlen((itor)->sTvasPath[i]) > 0)
				{
					FILE *fpTvas = NULL;
					fpTvas = fopen((itor)->sTvasPath[i], "wb"); // WRITE
					fwrite(pResStruct->tvases[i], pResStruct->tvasSize[i], 1, fpTvas);
					fclose(fpTvas);
					fpTvas = NULL;
                    nRouteCnt++;
				}
			}

			m_listTvasInfo.erase(itor);
			break;
		}
	}

	strcpy(m_pNddsWeatherInfo.m_SkyCode, pResStruct->destination.skyCode);
	strcpy(m_pNddsWeatherInfo.m_SkyName, pResStruct->destination.skyName);
	strcpy(m_pNddsWeatherInfo.m_RainTypeCode, pResStruct->destination.rainTypeCode);
	strcpy(m_pNddsWeatherInfo.m_RainSinceOnTime, pResStruct->destination.rainSinceOnTime);
	strcpy(m_pNddsWeatherInfo.m_TempC1h, pResStruct->destination.tempC1h);

    NDDSLIB_ReleaseObject(pRes, eErrCode);

    return nRouteCnt;
}

bool CNddsModule::GetRouteSummaryResult(int nSessionID ,int *nErrorCode)
{
	eNddsErrorCode			eErrCode = NDDSERR_UNKNOWN;
	char szErrorMsg[256] = {0,};

	NDDS_COMMON_RES *pRes = NULL;
	NDDSTMAP_ROUTESUMMARYINFO_RES *pResStruct = NULL;
	eNddsProtocolType eResProtocolType = NDDSTYPE_NONE;
	NDDSLIB_GetResultProtocol(nSessionID, false, &eResProtocolType, &pRes, &eErrCode);
	pResStruct = (NDDSTMAP_ROUTESUMMARYINFO_RES*)pRes;

	if(pResStruct != NULL)
	{
		IsChangeAccesskey(nSessionID, pResStruct->header.accessKey);
		CheckErrorCode(nSessionID, pResStruct->header.errorCode);
	}

	if(!NddsErrorCheck(eErrCode, pResStruct?pResStruct->header.errorMessage:NULL, szErrorMsg))
	{
        NDDSLIB_ReleaseObject(pRes, eErrCode);
		*nErrorCode = eErrCode;
		return false;
	}

	if (pResStruct == NULL)
	{
		*nErrorCode = NDDSERR_RESULT_NULL;
		return false;
	}

	bool bResult = false;
	if(pResStruct != NULL)
	{
		if(m_nRouteSummaryIdx > (MaxRouteSummaryCnt-1))
		{
			m_nRouteSummaryIdx = 0;
		}

		int nIdx = m_nRouteSummaryIdx;

		m_pNddsRouteSummaryResult[nIdx].m_SessionID = nSessionID;
		m_pNddsRouteSummaryResult[nIdx].m_TotalTime = pResStruct->routeList[0].totalTime;
		m_pNddsRouteSummaryResult[nIdx].m_EstimationOption = pResStruct->routeList[0].estimationOption;
		if(pResStruct->routeList[0].estimationTime)
			strcpy(m_pNddsRouteSummaryResult[nIdx].m_EstimationTime, pResStruct->routeList[0].estimationTime);

		m_nRouteSummaryIdx++;

		bResult = true;
	}

    NDDSLIB_ReleaseObject(pRes);

	return bResult;
}
//////////////////////////////////////
//////////////////////////////////////





//////////////////////////////////////
// Search API
//////////////////////////////////////

bool CNddsModule::API_NDDS_Search(PAPI_SEARCH_REQ pRequest, PAPI_SEARCH_RES pResult)
{
	//DBGPRINT(_T("############################# Search #1\n"));

	// 모듈 프로세스 스레드에 요청 중이거나 동작중인지 체크 하여 있으면 검색이면 중지후 요청하고 나머지는 실패 처리한다.
#ifdef _LINUX
	if(sem_wait(&m_pModuleProcSem) != 0)
#else
	if(SDL_SemWait(m_pModuleProcSem) == SDL_MUTEX_TIMEDOUT)
#endif
	{
//		if(m_sModuleInputProcData.eProcType != E_PROCTYPE_SEARCH)
		{
			pResult->nResult = N_SEARCHERR_DUPLICATION;
			return false;
		}

		//DBGPRINT(_T("############################# Search #1 - Stop\n"));

		switch(m_sModuleInputProcData.sSearchData.sSearchInput.eSearchType)
		{
		default:
			//assert(0);
			break;
		}
		delay(1); //스레드 스위칭(기존 스레드 처리가 끝날 수 있게 스위칭)
	}

	//DBGPRINT(_T("MUTEX[%d] - API_NDDS_Search::LockMutex #1\n"), GetTickCount());
#ifdef _LINUX
	pthread_mutex_lock(&m_pModuleProcMutex);
#else
	SDL_LockMutex(m_pModuleProcMutex);
#endif

	int nSessionID = ++m_nLastIdxSession;

	SendMessgeCallback(NM_NDDS_SEARCH_INIT, nSessionID, (long)(pRequest->sSearchInput.eSearchType));
	//DBGPRINT(_T("MUTEX[%d] - API_NDDS_Search::LockMutex #2\n"), GetTickCount());
	//DBGPRINT(_T("############################# Search #2\n"));

	//////////////////////////////////////
	//Search Input 데이터를 설정한다.

	m_sModuleInputProcData.eProcType = E_NDDS_PROCTYPE_SEARCH;
	m_sModuleInputProcData.nSearchSessionID = nSessionID;
	m_sModuleInputProcData.bSearchSync = pRequest->bSync;
	memcpy(&m_sModuleInputProcData.sSearchData.sSearchInput, &pRequest->sSearchInput, sizeof(SEARCH_INPUT));

	////////////////////////////////
	// 스레드를 동작시킨다.
#ifdef _LINUX
	pthread_cond_signal(&m_pModuleProcCond);
	pthread_mutex_unlock(&m_pModuleProcMutex);
#else
	SDL_CondSignal(m_pModuleProcCond);
	SDL_UnlockMutex(m_pModuleProcMutex);
#endif

	delay(15);	//스레드 스위칭

	if(pRequest->bSync)
	{
		// 스레드 처리가 끝날때 까지 대기한다.
#ifdef _LINUX
		pthread_mutex_lock(&m_pModuleProcMutex);
		pthread_mutex_unlock(&m_pModuleProcMutex);
#else
		SDL_LockMutex(m_pModuleProcMutex);
		SDL_UnlockMutex(m_pModuleProcMutex);
#endif
		pResult->nResult = m_sModuleOutputProcData.sSearchData.nResult;
		if(m_sModuleInputProcData.sSearchData.sSearchInput.eSearchType == N_SEARCHTYPE_NDDSPOI
			|| m_sModuleInputProcData.sSearchData.sSearchInput.eSearchType == N_SEARCHTYPE_NDDSAREANAMESBYSTEP)
		{
			strcpy(pResult->szErrorMsg, m_sModuleOutputProcData.sSearchData.szErrorMsg);
		}
	}

	//DBGPRINT(_T("############################# Search #true\n"));

	pResult->nSessionID = nSessionID;

	return true;
}

bool CNddsModule::API_NDDS_GetSearchResultList(PAPI_SEARCH_RESULTLIST_REQ pRequest, PAPI_SEARCH_RESULTLIST_RES pResult)
{
	int i, nIdx;
	static stVERTEX32 wpOld = {0,0};
	
	pResult->nResult = 0;
	pResult->nTotal = m_nReadCnt;

	if(pRequest->eSearchType != m_eLastSearchType)
	{
		//assert(0);
		//return false;
	}

	if(pRequest->nIdx >= m_nReadCnt)
		return false;
	if(pRequest->nIdx + pRequest->nCnt > m_nReadCnt)
		pRequest->nCnt = m_nReadCnt - pRequest->nIdx;

	memset(pResult->sList, 0, sizeof(N_SEARCH_LIST)*MAX_RESULTLIST);

	switch(pRequest->eSearchType)
	{
	case N_SEARCHTYPE_NDDSPOI:
		{
			for(i=0 ; i<pRequest->nCnt ; i++)
			{
				nIdx = pRequest->nIdx + i;
				pResult->sList[i].nIdx = nIdx;

				strcpy(pResult->sList[i].szName, m_pNddsPoiResult[nIdx].Aliasname);
				strcpy(pResult->sList[i].szStdName, m_pNddsPoiResult[nIdx].m_name);

				strcpy(pResult->sList[i].szAddrName,  m_pNddsPoiResult[nIdx].m_addrname);
				strcpy(pResult->sList[i].szPhoneNumber, m_pNddsPoiResult[nIdx].m_strTell);
				pResult->sList[i].nPoiID = m_pNddsPoiResult[nIdx].m_PoiID;
				pResult->sList[i].RPFlag = m_pNddsPoiResult[nIdx].m_RPFlag;
				pResult->sList[i].nDist = m_pNddsPoiResult[nIdx].m_Dist;
				pResult->sList[i].wp.x = (int)(m_pNddsPoiResult[nIdx].m_X /36000.0 * WORLDCOORD_PER_DEG + 0.5);
				pResult->sList[i].wp.y = (int)(m_pNddsPoiResult[nIdx].m_Y /36000.0 * WORLDCOORD_PER_DEG + 0.5);

				pResult->sList[i].nOPIPrice = m_pNddsPoiResult[nIdx].m_nOPIPrice;
				pResult->sList[i].uGatherTime = m_pNddsPoiResult[nIdx].m_GatherTime;
				strcpy(pResult->sList[i].szNavSeq, m_pNddsPoiResult[nIdx].m_NavSeq);

                pResult->sList[i].nKind = m_pNddsPoiResult[nIdx].m_dataKind;

				pResult->nResult++;
			}
		}
		break;
	case N_SEARCHTYPE_NDDSAROUNDPOI:
		{
			for(i=0 ; i<pRequest->nCnt ; i++)
			{
				nIdx = pRequest->nIdx + i;
				pResult->sList[i].nIdx = nIdx;

				strcpy(pResult->sList[i].szName, m_pNddsPoiResult[nIdx].Aliasname);
				strcpy(pResult->sList[i].szStdName, m_pNddsPoiResult[nIdx].m_name);

				strcpy(pResult->sList[i].szAddrName,  m_pNddsPoiResult[nIdx].m_addrname);
				strcpy(pResult->sList[i].szPhoneNumber, m_pNddsPoiResult[nIdx].m_strTell);
				pResult->sList[i].nPoiID = m_pNddsPoiResult[nIdx].m_PoiID;
				pResult->sList[i].RPFlag = m_pNddsPoiResult[nIdx].m_RPFlag;
				pResult->sList[i].nDist = m_pNddsPoiResult[nIdx].m_Dist;
				pResult->sList[i].wp.x = (int)(m_pNddsPoiResult[nIdx].m_X /36000.0 * WORLDCOORD_PER_DEG + 0.5);
				pResult->sList[i].wp.y = (int)(m_pNddsPoiResult[nIdx].m_Y /36000.0 * WORLDCOORD_PER_DEG + 0.5);

				pResult->sList[i].nOPIPrice = m_pNddsPoiResult[nIdx].m_nOPIPrice;
				pResult->sList[i].uGatherTime = m_pNddsPoiResult[nIdx].m_GatherTime;

				strcpy(pResult->sList[i].szOilCompany, m_pNddsPoiResult[nIdx].m_strOilCompany);
                pResult->sList[i].nDataKind = m_pNddsPoiResult[nIdx].m_dataKind;
				pResult->sList[i].bHighGasoline = m_pNddsPoiResult[nIdx].m_HighGasoline;
				pResult->sList[i].bLowestOilSt = m_pNddsPoiResult[nIdx].m_LowestOilSt;
				pResult->sList[i].nOPIGasolinePrice = m_pNddsPoiResult[nIdx].m_nOPIGasolinePrice;
				pResult->sList[i].nOPIDieselPrice = m_pNddsPoiResult[nIdx].m_nOPIDieselPrice;
				pResult->sList[i].nOPILPGPrice = m_pNddsPoiResult[nIdx].m_nOPILPGPrice;
				pResult->sList[i].nOPIHiGasolinePrice = m_pNddsPoiResult[nIdx].m_nOPIHiGasolinePrice;
				pResult->sList[i].nOPIHiLPGPrice = m_pNddsPoiResult[nIdx].m_nOPIHiLPGPrice;
				strcpy(pResult->sList[i].szNavSeq, m_pNddsPoiResult[nIdx].m_NavSeq);

				pResult->nResult++;
			}
		}
		break;
	case N_SEARCHTYPE_NDDSAREANAMESBYSTEP:
		{
			for(i=0 ; i<pRequest->nCnt ; i++)
			{
				nIdx = pRequest->nIdx + i;
				pResult->sList[i].nIdx = nIdx;

				strcpy(pResult->sList[i].szName, m_pNddsPoiResult[nIdx].Aliasname);
				strcpy(pResult->sList[i].szStdName, m_pNddsPoiResult[nIdx].m_name);

				strcpy(pResult->sList[i].szAddrName,  m_pNddsPoiResult[nIdx].m_addrname);
				strcpy(pResult->sList[i].szPhoneNumber, m_pNddsPoiResult[nIdx].m_strTell);
				pResult->sList[i].nPoiID = m_pNddsPoiResult[nIdx].m_PoiID;
				pResult->sList[i].RPFlag = m_pNddsPoiResult[nIdx].m_RPFlag;
				pResult->sList[i].nDist = m_pNddsPoiResult[nIdx].m_Dist;
				pResult->sList[i].wp.x = (int)(m_pNddsPoiResult[nIdx].m_X /36000.0 * WORLDCOORD_PER_DEG + 0.5);
				pResult->sList[i].wp.y = (int)(m_pNddsPoiResult[nIdx].m_Y /36000.0 * WORLDCOORD_PER_DEG + 0.5);

				pResult->sList[i].nOPIPrice = m_pNddsPoiResult[nIdx].m_nOPIPrice;
				pResult->sList[i].uGatherTime = m_pNddsPoiResult[nIdx].m_GatherTime;

				pResult->nResult++;
			}
		}
		break;
    case N_SEARCHTYPE_NDDSPOISTATIONINFO:
        {
            for(i=0 ; i<pRequest->nCnt ; i++)
            {
                nIdx = pRequest->nIdx + i;
                pResult->sList[i].nIdx = nIdx;

                strcpy(pResult->sList[i].szStdName, m_pNddsPoiResult[nIdx].m_name);

                pResult->sList[i].nPoiID = m_pNddsPoiResult[nIdx].m_PoiID;

                pResult->sList[i].nOPIGasolinePrice = m_pNddsPoiResult[nIdx].m_nOPIGasolinePrice;
                pResult->sList[i].nOPIDieselPrice = m_pNddsPoiResult[nIdx].m_nOPIDieselPrice;
                pResult->sList[i].nOPILPGPrice = m_pNddsPoiResult[nIdx].m_nOPILPGPrice;
                pResult->sList[i].nDataKind = m_pNddsPoiResult[nIdx].m_dataKind;

                pResult->nResult++;
            }
        }
        break;
    case N_SEARCHTYPE_NDDSPOIEVSTATIONSBYPOINT:
        {
            for(i=0 ; i<pRequest->nCnt ; i++)
            {
                nIdx = pRequest->nIdx + i;
                pResult->sList[i].nIdx = nIdx;

                strcpy(pResult->sList[i].szStdName, m_pNddsPoiEvStationsByPoint.evStations[nIdx].szStationName);
                strcpy(pResult->sList[i].szAddrName, m_pNddsPoiEvStationsByPoint.evStations[nIdx].szAddr);
                strcpy(pResult->sList[i].szMngName, m_pNddsPoiEvStationsByPoint.evStations[nIdx].szMngName);
                pResult->sList[i].nPoiID = m_pNddsPoiEvStationsByPoint.evStations[nIdx].nPoiID;
                pResult->sList[i].wp.x = (int)(m_pNddsPoiEvStationsByPoint.evStations[nIdx].wp.x /36000.0 * WORLDCOORD_PER_DEG + 0.5);
                pResult->sList[i].wp.y = (int)(m_pNddsPoiEvStationsByPoint.evStations[nIdx].wp.y /36000.0 * WORLDCOORD_PER_DEG + 0.5);
                pResult->sList[i].nDist = m_pNddsPoiEvStationsByPoint.evStations[nIdx].nDist;
                pResult->sList[i].nEvChargerTotalCnt = m_pNddsPoiEvStationsByPoint.evStations[nIdx].nTotalCnt;
                if(pResult->sList[i].nEvChargerTotalCnt > 0)
                {
                    for(int j=0; j<pResult->sList[i].nEvChargerTotalCnt;j++)
                    {
                        if(m_pNddsPoiEvStationsByPoint.evStations[nIdx].sEvChargers[j].szBid)
                            strcpy(pResult->sList[i].sEvChargers[j].szBid, m_pNddsPoiEvStationsByPoint.evStations[nIdx].sEvChargers[j].szBid);
                        if(m_pNddsPoiEvStationsByPoint.evStations[nIdx].sEvChargers[j].szSid)
                            strcpy(pResult->sList[i].sEvChargers[j].szSid, m_pNddsPoiEvStationsByPoint.evStations[nIdx].sEvChargers[j].szSid);
                        if(m_pNddsPoiEvStationsByPoint.evStations[nIdx].sEvChargers[j].szCid)
                            strcpy(pResult->sList[i].sEvChargers[j].szCid, m_pNddsPoiEvStationsByPoint.evStations[nIdx].sEvChargers[j].szCid);
                        if(m_pNddsPoiEvStationsByPoint.evStations[nIdx].sEvChargers[j].szType)
                            strcpy(pResult->sList[i].sEvChargers[j].szType, m_pNddsPoiEvStationsByPoint.evStations[nIdx].sEvChargers[j].szType);
                        pResult->sList[i].sEvChargers[j].nStatus = m_pNddsPoiEvStationsByPoint.evStations[nIdx].sEvChargers[j].nStatus;
                    }
                }

                pResult->nResult++;
            }
        }
        break;
	default:
		break;
	}

	return true;
}

bool CNddsModule::API_NDDS_GetSearchPoiDetailInfo(PAPI_SEARCH_POIDETAILINFO_REQ pRequest, PAPI_SEARCH_POIDETAILINFO_RES pResult)
{
	int i=0;

	pResult->nResult = 0;

	if(N_SEARCHTYPE_NDDSPOIDETAILINFO != m_eLastSearchType)
	{
		//assert(0);
		//return false;
	}

	memset(&pResult->sPoiDetailInfo, 0, sizeof(N_SEARCH_POIDETAILINFO));
	
	pResult->sPoiDetailInfo.nPoiID = m_pNddsPoiDetailInfo.m_PoiID;
	strcpy(pResult->sPoiDetailInfo.szNavSeq, m_pNddsPoiDetailInfo.m_NavSeq);
	strcpy(pResult->sPoiDetailInfo.szName, m_pNddsPoiDetailInfo.m_Name);
	strcpy(pResult->sPoiDetailInfo.szAddrName,  m_pNddsPoiDetailInfo.m_AddrName);
	pResult->sPoiDetailInfo.tp.x = (int)(m_pNddsPoiDetailInfo.m_X /36000.0 * WORLDCOORD_PER_DEG + 0.5);
	pResult->sPoiDetailInfo.tp.y = (int)(m_pNddsPoiDetailInfo.m_Y /36000.0 * WORLDCOORD_PER_DEG + 0.5);

	strcpy(pResult->sPoiDetailInfo.szPhoneNumber, m_pNddsPoiDetailInfo.m_PhoneNumber);
	strcpy(pResult->sPoiDetailInfo.szUrl, m_pNddsPoiDetailInfo.m_Url);
	pResult->sPoiDetailInfo.bHoliday = m_pNddsPoiDetailInfo.m_Holiday;
	pResult->sPoiDetailInfo.byPark = m_pNddsPoiDetailInfo.m_Park;
	strcpy(pResult->sPoiDetailInfo.szCateName, m_pNddsPoiDetailInfo.m_CateName);
	strcpy(pResult->sPoiDetailInfo.szInfo, m_pNddsPoiDetailInfo.m_Info);

    if(m_pNddsPoiDetailInfo.m_PrimaryBun)
        strcpy(pResult->sPoiDetailInfo.szPrimaryBun, m_pNddsPoiDetailInfo.m_PrimaryBun);

    if(m_pNddsPoiDetailInfo.m_SecondaryBun)
        strcpy(pResult->sPoiDetailInfo.szSecondBun, m_pNddsPoiDetailInfo.m_SecondaryBun);

    if(m_pNddsPoiDetailInfo.m_LcdName)
        strcpy(pResult->sPoiDetailInfo.szLcdName, m_pNddsPoiDetailInfo.m_LcdName);

    if(m_pNddsPoiDetailInfo.m_McdName)
        strcpy(pResult->sPoiDetailInfo.szMcdName, m_pNddsPoiDetailInfo.m_McdName);

    if(m_pNddsPoiDetailInfo.m_ScdName)
        strcpy(pResult->sPoiDetailInfo.szScdName, m_pNddsPoiDetailInfo.m_ScdName);

    if(m_pNddsPoiDetailInfo.m_DcdName)
        strcpy(pResult->sPoiDetailInfo.szDcdName, m_pNddsPoiDetailInfo.m_DcdName);

    if(m_pNddsPoiDetailInfo.m_MlClass)
        pResult->sPoiDetailInfo.nMlClass = m_pNddsPoiDetailInfo.m_MlClass;

    if(m_pNddsPoiDetailInfo.m_BldAdd)
        strcpy(pResult->sPoiDetailInfo.szBldAdd, m_pNddsPoiDetailInfo.m_BldAdd);

    if(m_pNddsPoiDetailInfo.m_RoadName)
        strcpy(pResult->sPoiDetailInfo.szRoadName, m_pNddsPoiDetailInfo.m_RoadName);

    if(m_pNddsPoiDetailInfo.m_BldNo1)
        strcpy(pResult->sPoiDetailInfo.szBldNo1, m_pNddsPoiDetailInfo.m_BldNo1);

    if(m_pNddsPoiDetailInfo.m_BldNo2)
        strcpy(pResult->sPoiDetailInfo.szBldNo2, m_pNddsPoiDetailInfo.m_BldNo2);

    if(m_pNddsPoiDetailInfo.m_centerX)
        pResult->sPoiDetailInfo.centerTp.x =m_pNddsPoiDetailInfo.m_centerX;

    if(m_pNddsPoiDetailInfo.m_centerY)
        pResult->sPoiDetailInfo.centerTp.y = m_pNddsPoiDetailInfo.m_centerY;

    if(m_pNddsPoiDetailInfo.m_RpFlag)
        m_pNddsPoiDetailInfo.m_RpFlag = m_pNddsPoiDetailInfo.m_RpFlag;

    if(m_pNddsPoiDetailInfo.m_ZipCd)
        strcpy(pResult->sPoiDetailInfo.szZipCd, m_pNddsPoiDetailInfo.m_ZipCd);

    if(m_pNddsPoiDetailInfo.m_Menu1)
        strcpy(pResult->sPoiDetailInfo.szMenu1, m_pNddsPoiDetailInfo.m_Menu1);

    if(m_pNddsPoiDetailInfo.m_Menu2)
        strcpy(pResult->sPoiDetailInfo.szMenu2, m_pNddsPoiDetailInfo.m_Menu2);

    if(m_pNddsPoiDetailInfo.m_Menu3)
        strcpy(pResult->sPoiDetailInfo.szMenu3, m_pNddsPoiDetailInfo.m_Menu3);

    if(m_pNddsPoiDetailInfo.m_Menu4)
        strcpy(pResult->sPoiDetailInfo.szMenu4, m_pNddsPoiDetailInfo.m_Menu4);

    if(m_pNddsPoiDetailInfo.m_Menu5)
        strcpy(pResult->sPoiDetailInfo.szMenu5, m_pNddsPoiDetailInfo.m_Menu5);

    if(m_pNddsPoiDetailInfo.m_TelNo)
        strcpy(pResult->sPoiDetailInfo.szTelNo, m_pNddsPoiDetailInfo.m_TelNo);

    if(m_pNddsPoiDetailInfo.m_Road)
        strcpy(pResult->sPoiDetailInfo.szRoad, m_pNddsPoiDetailInfo.m_Road);

    if(m_pNddsPoiDetailInfo.m_RecommandPoint)
        pResult->sPoiDetailInfo.nRecommandPoint = m_pNddsPoiDetailInfo.m_RecommandPoint;

    if(m_pNddsPoiDetailInfo.m_IconType)
        strcpy(pResult->sPoiDetailInfo.szIconType, m_pNddsPoiDetailInfo.m_IconType);

    if(m_pNddsPoiDetailInfo.m_JoinStoreYn)
        strcpy(pResult->sPoiDetailInfo.szJoinStoreYn, m_pNddsPoiDetailInfo.m_JoinStoreYn);

    if(m_pNddsPoiDetailInfo.m_JoinViewType)
        strcpy(pResult->sPoiDetailInfo.szJoinViewType, m_pNddsPoiDetailInfo.m_JoinViewType);

	if(m_pNddsPoiDetailInfo.m_DayOffInfo)
		strcpy(pResult->sPoiDetailInfo.szDayOffInfo, m_pNddsPoiDetailInfo.m_DayOffInfo);

	if(m_pNddsPoiDetailInfo.m_BusinessHours)
		strcpy(pResult->sPoiDetailInfo.szBusinessHours, m_pNddsPoiDetailInfo.m_BusinessHours);

	if(m_pNddsPoiDetailInfo.m_DayOffDate)
		strcpy(pResult->sPoiDetailInfo.szDayOffDate, m_pNddsPoiDetailInfo.m_DayOffDate);

    // sangbong.woo - s
    if(m_pNddsPoiDetailInfo.m_AddInfo)
        strcpy(pResult->sPoiDetailInfo.szAddInfo, m_pNddsPoiDetailInfo.m_AddInfo);

    pResult->sPoiDetailInfo.m_HighGasoline = m_pNddsPoiDetailInfo.m_HighGasoline;
    pResult->sPoiDetailInfo.m_GatherTime = m_pNddsPoiDetailInfo.m_GatherTime;
    pResult->sPoiDetailInfo.m_nOPIGasolinePrice = m_pNddsPoiDetailInfo.m_nOPIGasolinePrice;
    pResult->sPoiDetailInfo.m_nOPIDieselPrice = m_pNddsPoiDetailInfo.m_nOPIDieselPrice;
    pResult->sPoiDetailInfo.m_nOPILPGPrice = m_pNddsPoiDetailInfo.m_nOPILPGPrice;
    pResult->sPoiDetailInfo.m_nOPIHiGasolinePrice = m_pNddsPoiDetailInfo.m_nOPIHiGasolinePrice;
    pResult->sPoiDetailInfo.m_nOPIHiLPGPrice = m_pNddsPoiDetailInfo.m_nOPIHiLPGPrice;
    pResult->sPoiDetailInfo.m_dataKind = m_pNddsPoiDetailInfo.m_dataKind;
    // sangbong.woo - e

	while(i < MAX_POIIMAGEINFO)
	{
		if(m_pNddsPoiDetailInfo.m_PoiImageInfos[i].m_FileSrc[0] == NULL)
		{
			break;
		}
		pResult->sPoiDetailInfo.sPoiImageInfos[i].nSeq = m_pNddsPoiDetailInfo.m_PoiImageInfos[i].m_Seq;
		strcpy(pResult->sPoiDetailInfo.sPoiImageInfos[i].szFileSrc, m_pNddsPoiDetailInfo.m_PoiImageInfos[i].m_FileSrc);
		i++;
	}

	i = 0;
	while(i < MAX_POIPARKINFO)
	{
		if(m_pNddsPoiDetailInfo.m_PoiParkInfos[i].m_CpName[0] == NULL)
		{
			break;
		}
		strcpy(pResult->sPoiDetailInfo.sPoiParkInfos[i].szCpName, m_pNddsPoiDetailInfo.m_PoiParkInfos[i].m_CpName);
		strcpy(pResult->sPoiDetailInfo.sPoiParkInfos[i].szParkUrl, m_pNddsPoiDetailInfo.m_PoiParkInfos[i].m_ParkUrl);
		i++;
	}

	pResult->nResult++;

	return true;
}

bool CNddsModule::API_NDDS_GetSearchPoiStationInfo(PAPI_SEARCH_POISTATIONINFO_REQ pRequest, PAPI_SEARCH_POISTATIONINFO_RES pResult)
{
    int i=0;

    pResult->nResult = 0;

    if(N_SEARCHTYPE_NDDSPOISTATIONINFO != m_eLastSearchType)
    {
        //assert(0);
        //return false;
    }

    memset(&pResult->sPoiStationInfo, 0, sizeof(N_SEARCH_POISTATIONINFO));

    pResult->sPoiStationInfo.nPoiID = m_pNddsPoiStationInfo.m_PoiID;
    strcpy(pResult->sPoiStationInfo.szName, m_pNddsPoiStationInfo.m_Name);
    pResult->sPoiStationInfo.tp.x = (int)(m_pNddsPoiStationInfo.m_X /36000.0 * WORLDCOORD_PER_DEG + 0.5);
    pResult->sPoiStationInfo.tp.y = (int)(m_pNddsPoiStationInfo.m_Y /36000.0 * WORLDCOORD_PER_DEG + 0.5);

    pResult->sPoiStationInfo.m_nOPIGasolinePrice = m_pNddsPoiStationInfo.m_nOPIGasolinePrice;
    pResult->sPoiStationInfo.m_nOPIDieselPrice = m_pNddsPoiStationInfo.m_nOPIDieselPrice;
    pResult->sPoiStationInfo.m_nOPILPGPrice = m_pNddsPoiStationInfo.m_nOPILPGPrice;
    pResult->sPoiStationInfo.m_nOPIHiGasolinePrice = m_pNddsPoiStationInfo.m_nOPIHiGasolinePrice;
    pResult->sPoiStationInfo.m_nOPIHiLPGPrice = m_pNddsPoiStationInfo.m_nOPIHiLPGPrice;
    pResult->sPoiStationInfo.m_dataKind = m_pNddsPoiStationInfo.m_dataKind;
    pResult->nResult++;

    return true;
}

bool CNddsModule::API_NDDS_GetSearchPoiEvStationInfo(PAPI_SEARCH_POIEVSTATIONINFO_REQ pRequest, PAPI_SEARCH_POIEVSTATIONINFO_RES pResult)
{
    int i=0;

    pResult->nResult = 0;

    if(N_SEARCHTYPE_NDDSPOIEVSTATIONINFO != m_eLastSearchType)
    {
        //assert(0);
        //return false;
    }

    memset(&pResult->sPoiEvStationInfo, 0, sizeof(N_SEARCH_POIEVSTATIONINFO));

    pResult->sPoiEvStationInfo.nPoiID = m_pNddsPoiEvStationInfo.nPoiID;
    if(m_pNddsPoiEvStationInfo.szStationName)
        strcpy(pResult->sPoiEvStationInfo.szStationName, m_pNddsPoiEvStationInfo.szStationName);
    if(m_pNddsPoiEvStationInfo.szAddr)
        strcpy(pResult->sPoiEvStationInfo.szAddr, m_pNddsPoiEvStationInfo.szAddr);

    if(m_pNddsPoiEvStationInfo.szRoadAddr)
        strcpy(pResult->sPoiEvStationInfo.szRoadAddr, m_pNddsPoiEvStationInfo.szRoadAddr);
    if(m_pNddsPoiEvStationInfo.szAddrDetail)
        strcpy(pResult->sPoiEvStationInfo.szAddrDetail, m_pNddsPoiEvStationInfo.szAddrDetail);
    if(m_pNddsPoiEvStationInfo.szMngName)
        strcpy(pResult->sPoiEvStationInfo.szMngName, m_pNddsPoiEvStationInfo.szMngName);
    if(m_pNddsPoiEvStationInfo.szMngId)
        strcpy(pResult->sPoiEvStationInfo.szMngId, m_pNddsPoiEvStationInfo.szMngId);
    if(m_pNddsPoiEvStationInfo.szTel)
        strcpy(pResult->sPoiEvStationInfo.szTel, m_pNddsPoiEvStationInfo.szTel);
    pResult->sPoiEvStationInfo.bFreeYn = m_pNddsPoiEvStationInfo.bFreeYn;
    if(m_pNddsPoiEvStationInfo.szFee)
        strcpy(pResult->sPoiEvStationInfo.szFee, m_pNddsPoiEvStationInfo.szFee);
    pResult->sPoiEvStationInfo.bReservYn = m_pNddsPoiEvStationInfo.bReservYn;
    if(m_pNddsPoiEvStationInfo.szUseTime)
        strcpy(pResult->sPoiEvStationInfo.szUseTime, m_pNddsPoiEvStationInfo.szUseTime);
    pResult->sPoiEvStationInfo.bPayYn = m_pNddsPoiEvStationInfo.bPayYn;
    if(m_pNddsPoiEvStationInfo.szUpdateDt)
        strcpy(pResult->sPoiEvStationInfo.szUpdateDt, m_pNddsPoiEvStationInfo.szUpdateDt);
    pResult->sPoiEvStationInfo.wp.x = m_pNddsPoiEvStationInfo.wp.x;
    pResult->sPoiEvStationInfo.wp.y = m_pNddsPoiEvStationInfo.wp.y;

    pResult->sPoiEvStationInfo.nTotalCnt = m_pNddsPoiEvStationInfo.nTotalCnt;
    for(int i=0;i<pResult->sPoiEvStationInfo.nTotalCnt;i++)
    {
        if(m_pNddsPoiEvStationInfo.sEvChargers[i].szBid)
            strcpy(pResult->sPoiEvStationInfo.sEvChargers[i].szBid, m_pNddsPoiEvStationInfo.sEvChargers[i].szBid);
        if(m_pNddsPoiEvStationInfo.sEvChargers[i].szSid)
            strcpy(pResult->sPoiEvStationInfo.sEvChargers[i].szSid, m_pNddsPoiEvStationInfo.sEvChargers[i].szSid);
        if(m_pNddsPoiEvStationInfo.sEvChargers[i].szCid)
            strcpy(pResult->sPoiEvStationInfo.sEvChargers[i].szCid, m_pNddsPoiEvStationInfo.sEvChargers[i].szCid);
        if(m_pNddsPoiEvStationInfo.sEvChargers[i].szType)
            strcpy(pResult->sPoiEvStationInfo.sEvChargers[i].szType, m_pNddsPoiEvStationInfo.sEvChargers[i].szType);
        pResult->sPoiEvStationInfo.sEvChargers[i].nStatus = m_pNddsPoiEvStationInfo.sEvChargers[i].nStatus;
    }

    return true;
}

bool CNddsModule::API_NDDS_GetSearchPoiJoinInfo(PAPI_SEARCH_POIJOININFO_REQ pRequest, PAPI_SEARCH_POIJOININFO_RES pResult)
{
	int i, nIdx;

	pResult->nResult = 0;
	pResult->nTotal = m_nReadJoinInfoCnt;

	if(N_SEARCHTYPE_NDDSPOIJOININFO != m_eLastSearchType)
	{
		//assert(0);
		//return false;
	}

	if(pRequest->nIdx >= m_nReadJoinInfoCnt)
		return false;
	if(pRequest->nIdx + pRequest->nCnt > m_nReadJoinInfoCnt)
		pRequest->nCnt = m_nReadJoinInfoCnt - pRequest->nIdx;

	memset(pResult->sPoiJoinInfo, 0, sizeof(N_SEARCH_POIJOININFO)*MAX_RESULTLIST);

	for(i=0 ; i<pRequest->nCnt ; i++)
	{
		nIdx = pRequest->nIdx + i;
		pResult->sPoiJoinInfo[i].nIdx = nIdx;

		pResult->sPoiJoinInfo[i].nPoiID = m_pNddsPoiJoinInfo[i].m_PoiID;
		strcpy(pResult->sPoiJoinInfo[i].szJoinType, m_pNddsPoiJoinInfo[i].m_JoinType);
		strcpy(pResult->sPoiJoinInfo[i].szName, m_pNddsPoiJoinInfo[i].m_Name);
		
		strcpy(pResult->sPoiJoinInfo[i].szCashRate, m_pNddsPoiJoinInfo[i].m_CashRate);
		strcpy(pResult->sPoiJoinInfo[i].szCardRate, m_pNddsPoiJoinInfo[i].m_CardRate);

		strcpy(pResult->sPoiJoinInfo[i].szWorkTime, m_pNddsPoiJoinInfo[i].m_WorkTime);
		strcpy(pResult->sPoiJoinInfo[i].szWeekendWorkTime, m_pNddsPoiJoinInfo[i].m_WeekendWorkTime);

		strcpy(pResult->sPoiJoinInfo[i].szUrl, m_pNddsPoiJoinInfo[i].m_Url);

		pResult->sPoiJoinInfo[i].byPark = m_pNddsPoiJoinInfo[i].m_Park;

		strcpy(pResult->sPoiJoinInfo[i].szBenefit, m_pNddsPoiJoinInfo[i].m_Benefit);
		strcpy(pResult->sPoiJoinInfo[i].szUseGuide, m_pNddsPoiJoinInfo[i].m_UseGuide);
		strcpy(pResult->sPoiJoinInfo[i].szInformation, m_pNddsPoiJoinInfo[i].m_Information);

		pResult->sPoiJoinInfo[i].nItem = m_pNddsPoiJoinInfo[i].m_Item;
		for(int j = 0; j < pResult->sPoiJoinInfo[i].nItem; j++)
		{
			strcpy(pResult->sPoiJoinInfo[i].szItemInfos[j], m_pNddsPoiJoinInfo[i].m_ItemInfos[j]);
		}

		pResult->nResult++;
	}

	return true;
}

bool CNddsModule::API_NDDS_GetSearchRecentDestnationList(PAPI_SEARCH_RECENTDESTNATION_REQ pRequest, PAPI_SEARCH_RECENTDESTNATION_RES pResult)
{
	int i, nIdx = 0;

	pResult->nResult = 0;

	if(N_SEARCHTYPE_NDDSRECENTDESTNATION != m_eLastSearchType)
	{
		//assert(0);
		//return false;
	}

	if(pRequest->nIdx >= m_nReadRecentCnt)
		return false;
	if(pRequest->nIdx + pRequest->nCnt > m_nReadRecentCnt)
		pRequest->nCnt = m_nReadRecentCnt - pRequest->nIdx;

	memset(pResult->sRecentDestnationList, 0, sizeof(N_SEARCH_RECENTDESTNATION)*MAX_RESULTLIST);

	for(i=0 ; i<pRequest->nCnt ; i++)
	{
		nIdx = pRequest->nIdx + i;
		pResult->sRecentDestnationList[i].nIdx = nIdx;

		pResult->sRecentDestnationList[i].nPoiID = m_pNddsRecentDestnation[nIdx].m_PoiID;

		strcpy(pResult->sRecentDestnationList[i].szNavSeq, m_pNddsRecentDestnation[nIdx].m_NavSeq);

		strcpy(pResult->sRecentDestnationList[i].szName, m_pNddsRecentDestnation[nIdx].m_Name);

		pResult->sRecentDestnationList[i].tp.x = (int)(m_pNddsRecentDestnation[nIdx].m_X /36000.0 * WORLDCOORD_PER_DEG + 0.5);
		pResult->sRecentDestnationList[i].tp.y = (int)(m_pNddsRecentDestnation[nIdx].m_Y /36000.0 * WORLDCOORD_PER_DEG + 0.5);

		pResult->sRecentDestnationList[i].RPFlag = m_pNddsRecentDestnation[nIdx].m_RPFlag;

		strcpy(pResult->sRecentDestnationList[i].szSvcDate, m_pNddsRecentDestnation[nIdx].m_SvcDate);

		pResult->nResult++;
	}

	return true;
}


int CNddsModule::SearchNddsPoi(int nSessionID, eNddsRequestType eRequestType, char *szSearchName, char *szAddrName, NeNddsSearchSort  eSortType, stVERTEX32 *pwpCurrent, int nDist, NeNddsOilType eOilType, bool bNewAddrChk, char *szErrorMsg)
{
	eNddsErrorCode eErrCode = NDDSERR_UNKNOWN;

	m_eLastSearchType = N_SEARCHTYPE_NDDSPOI;

	memset(m_pNddsPoiResult, 0x00, sizeof(NstNddsPoiResult)*MaxNddsSchResultBufCnt);
	m_nReadCnt = 0;
	*szErrorMsg = NULL;

	NDDSTMAP_FINDPOIS_REQ sRequest;
	memset(&sRequest, 0x00, sizeof(NDDSTMAP_FINDPOIS_REQ));

	char szSearchTypCd[2], szX[10], szY[10], szRadius[10];
	unsigned int x, y;

	if(eSortType == N_SORTTYPE_DIST)
		strcpy(szSearchTypCd, "R");
	else
		strcpy(szSearchTypCd, "A");

	x = (unsigned int)(pwpCurrent->x / (256*2048.0) * 36000.0f + 0.5f);
	y = (unsigned int)(pwpCurrent->y / (256*2048.0) * 36000.0f + 0.5f);
	sprintf(szX, "%u", x);
	sprintf(szY, "%u", y);
	sprintf(szRadius, "%d", nDist);

	sRequest.searchTypCd = szSearchTypCd;
	sRequest.reqCnt = 200;		// 서버에서 전달되는 최대값
	sRequest.reqSeq = 1;
	sRequest.name = szSearchName;
	sRequest.areaName = szAddrName;
	sRequest.noorX = szX;
	sRequest.noorY = szY;
	sRequest.radius = szRadius;

    if(!NDDSLIB_RequestProtocol(NDDSTYPE_TMAP_FINDPOIS, eRequestType, (NDDS_COMMON_REQ*)(&sRequest), nSessionID, &eErrCode))
	{
		if(eErrCode == NDDSERR_USERCANCEL)
			strcpy(szErrorMsg, "사용자 취소");
		else
			strcpy(szErrorMsg, "데이터 생성 실패");

		return 0;
    }

	if(eRequestType == NDDSREQUEST_ATONCE)
	{
		m_bNewAddrChk = bNewAddrChk;
		m_eOilType = eOilType;
		return true;
	}

	eNddsStatus eStatus = NDDSSTATUS_WAIT;
	int nProgress = 0;
	DBGPRINT(_T("Wait "));
    unsigned long reqTime = GetTickCount();
	while(eStatus != NDDSSTATUS_COMPLETE && eStatus != NDDSSTATUS_CANCELED)
	{
        unsigned long thisTime = GetTickCount();
        unsigned long wattingTime = thisTime - reqTime;

        if(wattingTime > SEARCH_WAITTING_TIME)
        {
            strcpy(szErrorMsg, "서버 응답 없음");
            eErrCode = NDDSERR_TIMEOUT;
            return false;
        }

		DBGPRINT(_T("."));
		delay(100);
		NDDSLIB_GetStatusProtocol(nSessionID, &eStatus, &nProgress, &eErrCode);
	}
	DBGPRINT(_T("\n"));

	int nNddsErrorCode = -1;

	return GetNddsPoiResult(nSessionID, bNewAddrChk, eOilType, &nNddsErrorCode);
}

int CNddsModule::GetNddsPoiResult(int nSessionID, bool bNewAddrChk, NeNddsOilType eOilType,int *nErrorCode)
{
	eNddsErrorCode			eErrCode = NDDSERR_UNKNOWN;
	char szErrorMsg[256] = {0,};

	NDDS_COMMON_RES *pRes = NULL;
	NDDSTMAP_FINDPOIS_RES *pResStruct = NULL;
	eNddsProtocolType eResProtocolType = NDDSTYPE_NONE;
	NDDSLIB_GetResultProtocol(nSessionID, false, &eResProtocolType, &pRes, &eErrCode);
	pResStruct = (NDDSTMAP_FINDPOIS_RES*)pRes;

	if(pResStruct != NULL)
	{
		IsChangeAccesskey(nSessionID, pResStruct->header.accessKey);
		CheckErrorCode(nSessionID, pResStruct->header.errorCode);
	}

	if(!NddsErrorCheck(eErrCode, pResStruct?pResStruct->header.errorMessage:NULL, szErrorMsg))
	{
        NDDSLIB_ReleaseObject(pRes, eErrCode);
		return 0;
	}

	if (pResStruct == NULL)
	{
		*nErrorCode = NDDSERR_RESULT_NULL;
		return false;
	}

	NDDSTMAP_POISEARCHES_DTO *pPoi;
	m_nReadCnt = pResStruct->listCnt;
	for(int i=0 ; i<m_nReadCnt ; i++)
	{
		pPoi = &pResStruct->poiSearches[i];

		if(pPoi->orgName)
			strcpy(m_pNddsPoiResult[i].Aliasname, pPoi->orgName);

		strcpy(m_pNddsPoiResult[i].m_name, pPoi->name);

		if(bNewAddrChk && pPoi->roadName)
		{
//			if(pPoi->bldNo1)
//			{
//                if(pPoi->bldNo2 != NULL && pPoi->bldNo2[0] != '0')
//                    sprintf(m_pNddsPoiResult[i].m_addrname, "%s %s %s %s-%s", pPoi->lcdName, pPoi->mcdName, pPoi->roadName, pPoi->bldNo1, pPoi->bldNo2);
//                else
//                    sprintf(m_pNddsPoiResult[i].m_addrname, "%s %s %s %s", pPoi->lcdName, pPoi->mcdName, pPoi->roadName, pPoi->bldNo1);
//			}
//            else
			if(pPoi->mcdName)
                sprintf(m_pNddsPoiResult[i].m_addrname, "%s %s %s", pPoi->lcdName, pPoi->mcdName, pPoi->roadName);
			else
				sprintf(m_pNddsPoiResult[i].m_addrname, "%s %s", pPoi->lcdName, pPoi->roadName);
		}else
		{
            if(pPoi->primary)
            {
				if(pPoi->mcdName)
					sprintf(m_pNddsPoiResult[i].m_addrname, "%s %s %s", pPoi->lcdName, pPoi->mcdName, pPoi->scdName);
				else
					sprintf(m_pNddsPoiResult[i].m_addrname, "%s %s", pPoi->lcdName, pPoi->scdName);
//              strcpy(m_pNddsPoiResult[i].m_addrname, pPoi->addr);
//				if(pPoi->secondary)
//				{
//                    if(pPoi->secondary[0] != '0') //NDDS데이터에 0이 들어있어 예외처리
//                        sprintf(m_pNddsPoiResult[i].m_addrname, "%s %s-%s", pPoi->addr, pPoi->primary, pPoi->secondary);
//                    else
//						sprintf(m_pNddsPoiResult[i].m_addrname, "%s %s", pPoi->addr, pPoi->primary);
//				}
//				else
//					sprintf(m_pNddsPoiResult[i].m_addrname, "%s %s", pPoi->addr, pPoi->primary);

				if(pPoi->mlClass && pPoi->mlClass[0] == '2')
					strcat(m_pNddsPoiResult[i].m_addrname, " 산");
			}
			else
			{
				if(pPoi->mcdName)
					sprintf(m_pNddsPoiResult[i].m_addrname, "%s %s %s", pPoi->lcdName, pPoi->mcdName, pPoi->scdName);//strcpy(m_pNddsPoiResult[i].m_addrname, pPoi->addr);
				else
					sprintf(m_pNddsPoiResult[i].m_addrname, "%s %s", pPoi->lcdName, pPoi->scdName);//strcpy(m_pNddsPoiResult[i].m_addrname, pPoi->addr);
			}
		}


        if(pPoi->navX1)
            m_pNddsPoiResult[i].m_X = atoi(pPoi->navX1);
        if(pPoi->navY1)
            m_pNddsPoiResult[i].m_Y = atoi(pPoi->navY1);
		if(pPoi->poiId)
			m_pNddsPoiResult[i].m_PoiID = atoi(pPoi->poiId);
		if(pPoi->radius)
			m_pNddsPoiResult[i].m_Dist = (int)(atof(pPoi->radius) * 1000.0);
		if(pPoi->phoneNum)
			strcpy(m_pNddsPoiResult[i].m_strTell, pPoi->phoneNum);
		m_pNddsPoiResult[i].m_RPFlag = pPoi->rpFlag;

		if(pPoi->dataKind)
		{
			if(pPoi->oilBaseSdt)
				m_pNddsPoiResult[i].m_GatherTime = atoi(pPoi->oilBaseSdt);

			if(pPoi->dataKind[0] == '3' && eOilType != N_eOilTypeLPG)
			{
				if(eOilType == N_eOilTypeGasoline)
					m_pNddsPoiResult[i].m_nOPIPrice = pPoi->hhPrice;
				else
					m_pNddsPoiResult[i].m_nOPIPrice = pPoi->ggPrice;
			}else if(pPoi->dataKind[0] == '4' && eOilType == N_eOilTypeLPG)
			{
				m_pNddsPoiResult[i].m_nOPIPrice = pPoi->llPrice;
			}else
			{
				m_pNddsPoiResult[i].m_nOPIPrice = 0;
			}
		}

        if(pPoi->dataKind)
           m_pNddsPoiResult[i].m_dataKind = atoi(pPoi->dataKind);

		if(pPoi->navSeq)
			strcpy(m_pNddsPoiResult[i].m_NavSeq, pPoi->navSeq);
	}

    NDDSLIB_ReleaseObject(pRes);

	return m_nReadCnt;
}

int CNddsModule::SearchNddsAroundPoi(int nSessionID, eNddsRequestType eRequestType, char *szSearchName, stVERTEX32 *pwpCurrent, int nDist, bool bNewAddrChk, char *szErrorMsg)
{ 
	eNddsErrorCode eErrCode = NDDSERR_UNKNOWN;

	m_eLastSearchType = N_SEARCHTYPE_NDDSAROUNDPOI;

	memset(m_pNddsPoiResult, 0x00, sizeof(NstNddsPoiResult)*MaxNddsSchResultBufCnt);
	m_nReadCnt = 0;
	*szErrorMsg = NULL;

	NDDSTMAP_FINDAROUNDPOIS_REQ sRequest;
	memset(&sRequest, 0x00, sizeof(NDDSTMAP_FINDAROUNDPOIS_REQ));

	char szX[10], szY[10], szRadius[10];
	unsigned int x, y;

	x = (unsigned int)(pwpCurrent->x / (256*2048.0) * 36000.0f + 0.5f);
	y = (unsigned int)(pwpCurrent->y / (256*2048.0) * 36000.0f + 0.5f);
	sprintf(szX, "%u", x);
	sprintf(szY, "%u", y);
	sprintf(szRadius, "%d", nDist);

	sRequest.reqCnt = 200;		// 서버에서 전달되는 최대값
	sRequest.reqSeq = 1;
	sRequest.name = szSearchName;
	sRequest.noorX = szX;
	sRequest.noorY = szY;
	sRequest.radius = szRadius;

	if(!NDDSLIB_RequestProtocol(NDDSTYPE_TMAP_FINDAROUNDPOIS, eRequestType, (NDDS_COMMON_REQ*)(&sRequest), nSessionID, &eErrCode))
	{
		if(eErrCode == NDDSERR_USERCANCEL)
			strcpy(szErrorMsg, "사용자 취소");
		else
			strcpy(szErrorMsg, "데이터 생성 실패");

		return 0;
	}

	if(eRequestType == NDDSREQUEST_ATONCE)
	{
		m_bNewAddrChk = bNewAddrChk;
		return true;
	}

    eNddsStatus eStatus = NDDSSTATUS_WAIT;
    int nProgress = 0;
    DBGPRINT(_T("Wait "));
    unsigned long reqTime = GetTickCount();
    while(eStatus != NDDSSTATUS_COMPLETE && eStatus != NDDSSTATUS_CANCELED)
    {
        unsigned long thisTime = GetTickCount();
        unsigned long wattingTime = thisTime - reqTime;

        if(wattingTime > SEARCH_WAITTING_TIME)
        {
            strcpy(szErrorMsg, "서버 응답 없음");
            eErrCode = NDDSERR_TIMEOUT;
            return false;
        }

        DBGPRINT(_T("."));
        delay(100);
        NDDSLIB_GetStatusProtocol(nSessionID, &eStatus, &nProgress, &eErrCode);
    }
	DBGPRINT(_T("\n"));

	int nNddsErrorCode = -1;

	return GetNddsAroundPoiResult(nSessionID, bNewAddrChk, &nNddsErrorCode);
}

int CNddsModule::GetNddsAroundPoiResult(int nSessionID, bool bNewAddrChk, int *nErrorCode)
{
	eNddsErrorCode eErrCode = NDDSERR_UNKNOWN;
	char szErrorMsg[256] = {0,};

	NDDS_COMMON_RES *pRes = NULL;
	NDDSTMAP_FINDAROUNDPOIS_RES *pResStruct = NULL;
	eNddsProtocolType eResProtocolType = NDDSTYPE_NONE;
	NDDSLIB_GetResultProtocol(nSessionID, false, &eResProtocolType, &pRes, &eErrCode);
	pResStruct = (NDDSTMAP_FINDAROUNDPOIS_RES*)pRes;

	if(pResStruct != NULL)
	{
		IsChangeAccesskey(nSessionID, pResStruct->header.accessKey);
		CheckErrorCode(nSessionID, pResStruct->header.errorCode);
	}

	if(!NddsErrorCheck(eErrCode, pResStruct?pResStruct->header.errorMessage:NULL, szErrorMsg))
	{
        NDDSLIB_ReleaseObject(pRes, eErrCode);
		return 0;
	}

	if (pResStruct == NULL)
	{
		*nErrorCode = NDDSERR_RESULT_NULL;
		return false;
	}

	NDDSTMAP_POISEARCHES_DTO *pPoi;
	m_nReadCnt = pResStruct->listCnt;
	for(int i=0 ; i<m_nReadCnt ; i++)
	{
		pPoi = &pResStruct->poiSearches[i];

		if(pPoi->orgName)
			strcpy(m_pNddsPoiResult[i].Aliasname, pPoi->orgName);

		strcpy(m_pNddsPoiResult[i].m_name, pPoi->name);

		if(bNewAddrChk && pPoi->roadName)
		{
			if(pPoi->bldNo1)
			{
				if(pPoi->bldNo2)
				{
					if(pPoi->mcdName)
						sprintf(m_pNddsPoiResult[i].m_addrname, "%s %s %s %s-%s", pPoi->lcdName, pPoi->mcdName, pPoi->roadName, pPoi->bldNo1, pPoi->bldNo2);
					else
						sprintf(m_pNddsPoiResult[i].m_addrname, "%s %s %s-%s", pPoi->lcdName, pPoi->roadName, pPoi->bldNo1, pPoi->bldNo2);
				}
				else
				{
					if(pPoi->mcdName)
						sprintf(m_pNddsPoiResult[i].m_addrname, "%s %s %s %s", pPoi->lcdName, pPoi->mcdName, pPoi->roadName, pPoi->bldNo1);
					else
						sprintf(m_pNddsPoiResult[i].m_addrname, "%s %s %s", pPoi->lcdName, pPoi->roadName, pPoi->bldNo1);
				}
			}
			else
			{
				if(pPoi->mcdName)
					sprintf(m_pNddsPoiResult[i].m_addrname, "%s %s %s", pPoi->lcdName, pPoi->mcdName, pPoi->roadName);
				else
					sprintf(m_pNddsPoiResult[i].m_addrname, "%s %s", pPoi->lcdName, pPoi->roadName);
			}
		}else
		{
			if(pPoi->primary)
			{
				if(pPoi->secondary)
					sprintf(m_pNddsPoiResult[i].m_addrname, "%s %s-%s", pPoi->addr, pPoi->primary, pPoi->secondary);
				else
					sprintf(m_pNddsPoiResult[i].m_addrname, "%s %s", pPoi->addr, pPoi->primary);

				if(pPoi->mlClass && pPoi->mlClass[0] == '2')
					strcat(m_pNddsPoiResult[i].m_addrname, " 산");
			}
			else
				strcpy(m_pNddsPoiResult[i].m_addrname, pPoi->addr);
		}

        if(pPoi->navX1)
            m_pNddsPoiResult[i].m_X = atoi(pPoi->navX1);
        if(pPoi->navY1)
            m_pNddsPoiResult[i].m_Y = atoi(pPoi->navY1);
		if(pPoi->poiId)
			m_pNddsPoiResult[i].m_PoiID = atoi(pPoi->poiId);
		if(pPoi->radius)
			m_pNddsPoiResult[i].m_Dist = (int)(atof(pPoi->radius) * 1000.0);
		if(pPoi->phoneNum)
			strcpy(m_pNddsPoiResult[i].m_strTell, pPoi->phoneNum);
		m_pNddsPoiResult[i].m_RPFlag = pPoi->rpFlag;

		if(pPoi->dataKind)
		{
			if(pPoi->stId)
				strcpy(m_pNddsPoiResult[i].m_strOilCompany, pPoi->stId);

            if(pPoi->highHhSale)
            {
                if(pPoi->highHhSale[0] == '1')
                    m_pNddsPoiResult[i].m_HighGasoline = true;
            }

            if(pPoi->minOilYn)
            {
                if(pPoi->minOilYn[0] == '1')
                    m_pNddsPoiResult[i].m_LowestOilSt = true;
            }

			if(pPoi->oilBaseSdt)
				m_pNddsPoiResult[i].m_GatherTime = atoi(pPoi->oilBaseSdt);

            if(pPoi->hhPrice)
                m_pNddsPoiResult[i].m_nOPIGasolinePrice = pPoi->hhPrice;
            if(pPoi->ggPrice)
                m_pNddsPoiResult[i].m_nOPIDieselPrice = pPoi->ggPrice;
            if(pPoi->llPrice)
                m_pNddsPoiResult[i].m_nOPILPGPrice = pPoi->llPrice;
            if(pPoi->highHhPrice)
                m_pNddsPoiResult[i].m_nOPIHiGasolinePrice = pPoi->highHhPrice;
            if(pPoi->highGgPrice)
                m_pNddsPoiResult[i].m_nOPIHiLPGPrice = pPoi->highGgPrice;
		}

		if(pPoi->navSeq)
		{
			if(strlen(pPoi->navSeq) < 7)
				strcpy(m_pNddsPoiResult[i].m_NavSeq, pPoi->navSeq);
			else
				DBGPRINT(_T("WARNING : navSeq %d\n"), strlen(pPoi->navSeq));
		}
	}

    NDDSLIB_ReleaseObject(pRes);

	return m_nReadCnt;
}

int CNddsModule::SearchNddsPoiDetailInfo(int nSessionID, eNddsRequestType eRequestType, int poiId, char *navSeq, int sizeWidth, int sizeHeight, char *szErrorMsg)
{
	eNddsErrorCode eErrCode = NDDSERR_UNKNOWN;

	m_eLastSearchType = N_SEARCHTYPE_NDDSPOIDETAILINFO;

	memset(&m_pNddsPoiDetailInfo, 0x00, sizeof(NstNddsPoiDetailInfo));
	m_nReadDetailInfoCnt = 0;
	*szErrorMsg = NULL;

	NDDSTMAP_FINDPOIDETAILINFO_REQ sRequest;
	memset(&sRequest, 0x00, sizeof(NDDSTMAP_FINDPOIDETAILINFO_REQ));

	char szPoiId[15] = {0,};
	char szWidth[5] = {0,};
	char szHeight[5] = {0,};

	sprintf(szPoiId, "%d", poiId);
	sprintf(szWidth, "%d", sizeWidth);
	sprintf(szHeight, "%d", sizeHeight);

	sRequest.poiId = szPoiId;
	sRequest.navSeq = navSeq;
	sRequest.sizeWidth = szWidth;
	sRequest.sizeHeight = szHeight;
	
	if(!NDDSLIB_RequestProtocol(NDDSTYPE_TMAP_FINDPOIDETAILINFO, eRequestType, (NDDS_COMMON_REQ*)(&sRequest), nSessionID, &eErrCode))
	{
		if(eErrCode == NDDSERR_USERCANCEL)
			strcpy(szErrorMsg, "사용자 취소");
		else
			strcpy(szErrorMsg, "데이터 생성 실패");

		return 0;
	}

	if(eRequestType == NDDSREQUEST_ATONCE)
	{
		return true;
    }

    eNddsStatus eStatus = NDDSSTATUS_WAIT;
    int nProgress = 0;
    DBGPRINT(_T("Wait "));
    unsigned long reqTime = GetTickCount();
    while(eStatus != NDDSSTATUS_COMPLETE && eStatus != NDDSSTATUS_CANCELED)
    {
        unsigned long thisTime = GetTickCount();
        unsigned long wattingTime = thisTime - reqTime;

        if(wattingTime > SEARCH_WAITTING_TIME)
        {
            strcpy(szErrorMsg, "서버 응답 없음");
            eErrCode = NDDSERR_TIMEOUT;
            return false;
        }

        DBGPRINT(_T("."));
        delay(100);
        NDDSLIB_GetStatusProtocol(nSessionID, &eStatus, &nProgress, &eErrCode);
    }

	int nNddsErrorCode = -1;

	return GetNddsPoiDetailInfoResult(nSessionID, &nNddsErrorCode);
}

int CNddsModule::GetNddsPoiDetailInfoResult(int nSessionID, int *nErrorCode)
{
	eNddsErrorCode eErrCode = NDDSERR_UNKNOWN;
	char szErrorMsg[256] = {0,};

	NDDS_COMMON_RES *pRes = NULL;
	NDDSTMAP_FINDPOIDETAILINFO_RES *pResStruct = NULL;
	eNddsProtocolType eResProtocolType = NDDSTYPE_NONE;
	NDDSLIB_GetResultProtocol(nSessionID, false, &eResProtocolType, &pRes, &eErrCode);
	pResStruct = (NDDSTMAP_FINDPOIDETAILINFO_RES*)pRes;

	if(pResStruct != NULL)
	{
		IsChangeAccesskey(nSessionID, pResStruct->header.accessKey);
		CheckErrorCode(nSessionID, pResStruct->header.errorCode);
	}

	if(!NddsErrorCheck(eErrCode, pResStruct?pResStruct->header.errorMessage:NULL, szErrorMsg))
	{
        NDDSLIB_ReleaseObject(pRes, eErrCode);
		return 0;
	}

	if (pResStruct == NULL)
	{
		*nErrorCode = NDDSERR_RESULT_NULL;
		return false;
	}

	if(pResStruct != NULL)
	{
        if(pResStruct->poiId)
            m_pNddsPoiDetailInfo.m_PoiID = atoi(pResStruct->poiId);
		if(pResStruct->navSeq)
			strcpy(m_pNddsPoiDetailInfo.m_NavSeq, pResStruct->navSeq);
		strcpy(m_pNddsPoiDetailInfo.m_Name, pResStruct->name);
		if(pResStruct->primaryBun)
		{
            strcpy(m_pNddsPoiDetailInfo.m_AddrName, pResStruct->addr);
//			if(pResStruct->secondaryBun)
//				sprintf(m_pNddsPoiDetailInfo.m_AddrName, "%s %s-%s", pResStruct->addr, pResStruct->primaryBun, pResStruct->secondaryBun);
//			else
//				sprintf(m_pNddsPoiDetailInfo.m_AddrName, "%s %s", pResStruct->addr, pResStruct->primaryBun);

			if(pResStruct->mlClass && pResStruct->mlClass[0] == '2')
				strcat(m_pNddsPoiDetailInfo.m_AddrName, " 산");
		}
		else
			strcpy(m_pNddsPoiDetailInfo.m_AddrName, pResStruct->addr);

        if(pResStruct->navX1)
            m_pNddsPoiDetailInfo.m_X = atoi(pResStruct->navX1);
        if(pResStruct->navY1)
            m_pNddsPoiDetailInfo.m_Y = atoi(pResStruct->navY1);
		if(pResStruct->telNo)
			strcpy(m_pNddsPoiDetailInfo.m_PhoneNumber, pResStruct->telNo);
		if(pResStruct->http)
			strcpy(m_pNddsPoiDetailInfo.m_Url, pResStruct->http);
		if(pResStruct->holidayN)
		{
			if(pResStruct->holidayN[0] == '1')
				m_pNddsPoiDetailInfo.m_Holiday = true;
			else
				m_pNddsPoiDetailInfo.m_Holiday = false;
		}

		if(pResStruct->parkYn)
		{
			if(pResStruct->parkYn[0] == '0')
				m_pNddsPoiDetailInfo.m_Park = '0';
			else if(pResStruct->parkYn[0] == '1')
				m_pNddsPoiDetailInfo.m_Park = '1';
			else
				m_pNddsPoiDetailInfo.m_Park = '2';
		}

		if(pResStruct->cateNm)
			strcpy(m_pNddsPoiDetailInfo.m_CateName, pResStruct->cateNm);
			
        // sangbong.woo - s
        if(pResStruct->addInfo)
            strcpy(m_pNddsPoiDetailInfo.m_AddInfo, pResStruct->addInfo);

        if(pResStruct->highHhSale)
        {
            if(pResStruct->highHhSale[0] == 'Y')
                m_pNddsPoiDetailInfo.m_HighGasoline = true;
        }

        if(pResStruct->oilBaseSdt)
            m_pNddsPoiDetailInfo.m_GatherTime = atoi(pResStruct->oilBaseSdt);

        if(pResStruct->hhPrice)
            m_pNddsPoiDetailInfo.m_nOPIGasolinePrice = pResStruct->hhPrice;
        if(pResStruct->ggPrice)
            m_pNddsPoiDetailInfo.m_nOPIDieselPrice = pResStruct->ggPrice;
        if(pResStruct->llPrice)
            m_pNddsPoiDetailInfo.m_nOPILPGPrice = pResStruct->llPrice;
        if(pResStruct->highHhPrice)
            m_pNddsPoiDetailInfo.m_nOPIHiGasolinePrice = pResStruct->highHhPrice;
        if(pResStruct->highGgPrice)
            m_pNddsPoiDetailInfo.m_nOPIHiLPGPrice = pResStruct->highGgPrice;

        if(pResStruct->viewId)
            m_pNddsPoiDetailInfo.m_dataKind = atoi(pResStruct->viewId);
        // sangbong.woo - e
		
        if(pResStruct->primaryBun)
            strcpy(m_pNddsPoiDetailInfo.m_PrimaryBun, pResStruct->primaryBun);

        if(pResStruct->secondaryBun)
            strcpy(m_pNddsPoiDetailInfo.m_SecondaryBun, pResStruct->secondaryBun);

        if(pResStruct->lcdName)
            strcpy(m_pNddsPoiDetailInfo.m_LcdName, pResStruct->lcdName);

        if(pResStruct->mcdName)
            strcpy(m_pNddsPoiDetailInfo.m_McdName, pResStruct->mcdName);

        if(pResStruct->scdName)
            strcpy(m_pNddsPoiDetailInfo.m_ScdName, pResStruct->scdName);

        if(pResStruct->dcdName)
            strcpy(m_pNddsPoiDetailInfo.m_DcdName, pResStruct->dcdName);

        if(pResStruct->mlClass)
            m_pNddsPoiDetailInfo.m_MlClass = atoi(pResStruct->mlClass);

        if(pResStruct->bldAddr)
            strcpy(m_pNddsPoiDetailInfo.m_BldAdd, pResStruct->bldAddr);

        if(pResStruct->roadName)
            strcpy(m_pNddsPoiDetailInfo.m_RoadName, pResStruct->roadName);

        if(pResStruct->bldNo1)
            strcpy(m_pNddsPoiDetailInfo.m_BldNo1, pResStruct->bldNo1);

        if(pResStruct->bldNo2)
            strcpy(m_pNddsPoiDetailInfo.m_BldNo2, pResStruct->bldNo2);

        if(pResStruct->centerX)
            m_pNddsPoiDetailInfo.m_centerX = atoi(pResStruct->centerX);

        if(pResStruct->centerY)
            m_pNddsPoiDetailInfo.m_centerY = atoi(pResStruct->centerY);

        if(pResStruct->rpFlag)
            m_pNddsPoiDetailInfo.m_RpFlag = pResStruct->rpFlag;

        if(pResStruct->zipCd)
            strcpy(m_pNddsPoiDetailInfo.m_ZipCd, pResStruct->zipCd);

        if(pResStruct->menu1)
            strcpy(m_pNddsPoiDetailInfo.m_Menu1, pResStruct->menu1);

        if(pResStruct->menu2)
            strcpy(m_pNddsPoiDetailInfo.m_Menu2, pResStruct->menu2);

        if(pResStruct->menu3)
            strcpy(m_pNddsPoiDetailInfo.m_Menu3, pResStruct->menu3);

        if(pResStruct->menu4)
            strcpy(m_pNddsPoiDetailInfo.m_Menu4, pResStruct->menu4);

        if(pResStruct->menu5)
            strcpy(m_pNddsPoiDetailInfo.m_Menu5, pResStruct->menu5);

        if(pResStruct->telNo)
            strcpy(m_pNddsPoiDetailInfo.m_TelNo, pResStruct->telNo);

        if(pResStruct->road)
            strcpy(m_pNddsPoiDetailInfo.m_Road, pResStruct->road);

        if(pResStruct->recommPoint)
            m_pNddsPoiDetailInfo.m_RecommandPoint = atoi(pResStruct->recommPoint);

        if(pResStruct->iconType)
            strcpy(m_pNddsPoiDetailInfo.m_IconType, pResStruct->iconType);

        if(pResStruct->joinStoreYn)
            strcpy(m_pNddsPoiDetailInfo.m_JoinStoreYn, pResStruct->joinStoreYn);

        if(pResStruct->joinViewType)
            strcpy(m_pNddsPoiDetailInfo.m_JoinViewType, pResStruct->joinViewType);

		if(pResStruct->dayOffInfo)
			strcpy(m_pNddsPoiDetailInfo.m_DayOffInfo, pResStruct->dayOffInfo);

		if(pResStruct->businessHours)
			strcpy(m_pNddsPoiDetailInfo.m_BusinessHours, pResStruct->businessHours);

		if(pResStruct->dayOffDate)
			strcpy(m_pNddsPoiDetailInfo.m_DayOffDate, pResStruct->dayOffDate);

		if(pResStruct->infomation)
		{
			if(strlen(pResStruct->infomation) > MAXLEN_DETAILINFO)
			{
				DBGPRINT(_T("WARNING : POI DETAIL INFO MAXLEN_DETAILINFO %d\n"), strlen(pResStruct->infomation));
				strncpy(m_pNddsPoiDetailInfo.m_Info, pResStruct->infomation, MAXLEN_DETAILINFO);
			}
			else
				strcpy(m_pNddsPoiDetailInfo.m_Info, pResStruct->infomation);
		}

		int i=0;
		for(i=0; i<pResStruct->nPoiImageInfos; i++)
		{
			if(i > MAX_POIIMAGEINFO)
			{
				DBGPRINT(_T("WARNING : POI DETAIL INFO MAX_POI_IMAGE_INFO %d\n"), pResStruct->nPoiImageInfos);
				break;
			}
			m_pNddsPoiDetailInfo.m_PoiImageInfos[i].m_Seq = pResStruct->poiImageInfos[i].seq;
			strcpy(m_pNddsPoiDetailInfo.m_PoiImageInfos[i].m_FileSrc, pResStruct->poiImageInfos[i].fileSrc);
		}

		for(i=0; i<pResStruct->nPoiParkInfos; i++)
		{
			if(pResStruct->nPoiParkInfos > MAX_POIPARKINFO)
			{
				DBGPRINT(_T("WARNING : POI DETAIL INFO MAX_POI_PARK_INFO %d\n"), pResStruct->nPoiParkInfos);
				break;
			}
			strcpy(m_pNddsPoiDetailInfo.m_PoiParkInfos[i].m_CpName, pResStruct->poiParkInfos[i].cpName);
			strcpy(m_pNddsPoiDetailInfo.m_PoiParkInfos[i].m_ParkUrl, pResStruct->poiParkInfos[i].mobileLinkUrl);
		}

		m_nReadDetailInfoCnt = 1;
	}

    NDDSLIB_ReleaseObject(pRes);

	return m_nReadDetailInfoCnt;
}

int CNddsModule::SearchNddsPoiJoinInfo(int nSessionID, eNddsRequestType eRequestType, int poiId, char *szErrorMsg)
{
	eNddsErrorCode eErrCode = NDDSERR_UNKNOWN;

	m_eLastSearchType = N_SEARCHTYPE_NDDSPOIJOININFO;

	memset(&m_pNddsPoiJoinInfo, 0x00, sizeof(NstNddsPoiJoinInfo));
	m_nReadJoinInfoCnt = 0;
	*szErrorMsg = NULL;

	NDDSTMAP_FINDPOIJOININFO_REQ sRequest;
	memset(&sRequest, 0x00, sizeof(NDDSTMAP_FINDPOIJOININFO_REQ));

	char szPoiId[15] = {0,};

	sprintf(szPoiId, "%d", poiId);

	sRequest.poiId = szPoiId;

	if(!NDDSLIB_RequestProtocol(NDDSTYPE_TMAP_FINDPOIJOININFO, eRequestType, (NDDS_COMMON_REQ*)(&sRequest), nSessionID, &eErrCode))
	{
		if(eErrCode == NDDSERR_USERCANCEL)
			strcpy(szErrorMsg, "사용자 취소");
		else
			strcpy(szErrorMsg, "데이터 생성 실패");

		return 0;
	}

	if(eRequestType == NDDSREQUEST_ATONCE)
	{
		return true;
	}

    eNddsStatus eStatus = NDDSSTATUS_WAIT;
    int nProgress = 0;
    DBGPRINT(_T("Wait "));
    unsigned long reqTime = GetTickCount();
    while(eStatus != NDDSSTATUS_COMPLETE && eStatus != NDDSSTATUS_CANCELED)
    {
        unsigned long thisTime = GetTickCount();
        unsigned long wattingTime = thisTime - reqTime;

        if(wattingTime > SEARCH_WAITTING_TIME)
        {
            strcpy(szErrorMsg, "서버 응답 없음");
            eErrCode = NDDSERR_TIMEOUT;
            return false;
        }

        DBGPRINT(_T("."));
        delay(100);
        NDDSLIB_GetStatusProtocol(nSessionID, &eStatus, &nProgress, &eErrCode);
    }


	int nNddsErrorCode = -1;

	return GetNddsPoiJoinInfoResult(nSessionID, &nNddsErrorCode);
}


int CNddsModule::GetNddsUserDataResult(int nSessionID, int *nErrorCode)
{
    eNddsErrorCode eErrCode = NDDSERR_UNKNOWN;
    char szErrorMsg[256] = {0,};

	NDDS_COMMON_RES *pRes = NULL;
    NDDSTMAP_FINDUSERDATA_RES *pResStruct = NULL;
    eNddsProtocolType eResProtocolType = NDDSTYPE_NONE;
    NDDSLIB_GetResultProtocol(nSessionID, false, &eResProtocolType, &pRes, &eErrCode);
    pResStruct = (NDDSTMAP_FINDUSERDATA_RES*)pRes;

	if(pResStruct != NULL)
	{
        IsChangeAccesskey(nSessionID, pResStruct->header.accessKey);
		CheckErrorCode(nSessionID, pResStruct->header.errorCode);
	}

    if(!NddsErrorCheck(eErrCode, pResStruct?pResStruct->header.errorMessage:NULL, szErrorMsg))
    {
        NDDSLIB_ReleaseObject(pRes, eErrCode);
        return 0;
    }

	if (pResStruct == NULL)
	{
		*nErrorCode = NDDSERR_RESULT_NULL;
		return false;
	}

    if(pResStruct != NULL)
    {
        if(pResStruct->poiMyFavoriteDto.homePoiId)
            m_pNddsUserData.sPoiMyFavorite.m_HomePoiId = atoi(pResStruct->poiMyFavoriteDto.homePoiId);

        //poiMyFavorite
        strcpy(m_pNddsUserData.sPoiMyFavorite.m_HomeCustName, pResStruct->poiMyFavoriteDto.homeCustName);
        if(pResStruct->poiMyFavoriteDto.homeNoorX)
            m_pNddsUserData.sPoiMyFavorite.m_HomeX = atoi(pResStruct->poiMyFavoriteDto.homeNoorX);
        if(pResStruct->poiMyFavoriteDto.homeNoorY)
            m_pNddsUserData.sPoiMyFavorite.m_HomeY = atoi(pResStruct->poiMyFavoriteDto.homeNoorY);
        if(pResStruct->poiMyFavoriteDto.homePoiId)
            m_pNddsUserData.sPoiMyFavorite.m_HomePoiId = atoi(pResStruct->poiMyFavoriteDto.homePoiId);
        strcpy(m_pNddsUserData.sPoiMyFavorite.m_HomeNavSeq ,pResStruct->poiMyFavoriteDto.homeNavSeq);
        strcpy(m_pNddsUserData.sPoiMyFavorite.m_HomeLcdName, pResStruct->poiMyFavoriteDto.homeLcdName);
        if(pResStruct->poiMyFavoriteDto.homeMcdName)
            strcpy(m_pNddsUserData.sPoiMyFavorite.m_HomeMcdName, pResStruct->poiMyFavoriteDto.homeMcdName);
        strcpy(m_pNddsUserData.sPoiMyFavorite.m_HomeScdName, pResStruct->poiMyFavoriteDto.homeScdName);
        strcpy(m_pNddsUserData.sPoiMyFavorite.m_HomeDcdName, pResStruct->poiMyFavoriteDto.homeDcdName);
        strcpy(m_pNddsUserData.sPoiMyFavorite.m_HomePrimaryBun, pResStruct->poiMyFavoriteDto.homePrimaryBun);
        strcpy(m_pNddsUserData.sPoiMyFavorite.m_HomeSecondBun, pResStruct->poiMyFavoriteDto.homeSecondaryBun);
        if(pResStruct->poiMyFavoriteDto.homeMlClass)
            m_pNddsUserData.sPoiMyFavorite.m_HomeMlClass = atoi(pResStruct->poiMyFavoriteDto.homeMlClass);
        strcpy(m_pNddsUserData.sPoiMyFavorite.m_HomeRoadName, pResStruct->poiMyFavoriteDto.homeRoadName);
        strcpy(m_pNddsUserData.sPoiMyFavorite.m_HomeBldNo1, pResStruct->poiMyFavoriteDto.homeBldNo1);
        strcpy(m_pNddsUserData.sPoiMyFavorite.m_HomeBldNo2, pResStruct->poiMyFavoriteDto.homeBldNo2);
        m_pNddsUserData.sPoiMyFavorite.m_HomeRpFlag = pResStruct->poiMyFavoriteDto.homeRpFlag;
        strcpy(m_pNddsUserData.sPoiMyFavorite.m_HomeAddInfo, pResStruct->poiMyFavoriteDto.homeAddInfo);
        strcpy(m_pNddsUserData.sPoiMyFavorite.m_HomeInsDatetime, pResStruct->poiMyFavoriteDto.homeInsDatetime);

        strcpy(m_pNddsUserData.sPoiMyFavorite.m_OfficeCustName, pResStruct->poiMyFavoriteDto.officeCustName);
        if(pResStruct->poiMyFavoriteDto.officeNoorX)
            m_pNddsUserData.sPoiMyFavorite.m_OfficeX = atoi(pResStruct->poiMyFavoriteDto.officeNoorX);
        if(pResStruct->poiMyFavoriteDto.officeNoorY)
            m_pNddsUserData.sPoiMyFavorite.m_OfficeY = atoi(pResStruct->poiMyFavoriteDto.officeNoorY);
        if(pResStruct->poiMyFavoriteDto.officePoiId)
            m_pNddsUserData.sPoiMyFavorite.m_OfficePoiId = atoi(pResStruct->poiMyFavoriteDto.officePoiId);
        strcpy(m_pNddsUserData.sPoiMyFavorite.m_OfficeNavSeq ,pResStruct->poiMyFavoriteDto.officeNavSeq);
        strcpy(m_pNddsUserData.sPoiMyFavorite.m_OfficeLcdName, pResStruct->poiMyFavoriteDto.officeLcdName);
        if(pResStruct->poiMyFavoriteDto.officeMcdName)
            strcpy(m_pNddsUserData.sPoiMyFavorite.m_OfficeMcdName, pResStruct->poiMyFavoriteDto.officeMcdName);
        strcpy(m_pNddsUserData.sPoiMyFavorite.m_OfficeScdName, pResStruct->poiMyFavoriteDto.officeScdName);
        strcpy(m_pNddsUserData.sPoiMyFavorite.m_OfficeDcdName, pResStruct->poiMyFavoriteDto.officeDcdName);
        strcpy(m_pNddsUserData.sPoiMyFavorite.m_OfficePrimaryBun, pResStruct->poiMyFavoriteDto.officePrimaryBun);
        strcpy(m_pNddsUserData.sPoiMyFavorite.m_OfficeSecondBun, pResStruct->poiMyFavoriteDto.officeSecondaryBun);
        if(pResStruct->poiMyFavoriteDto.officeMlClass)
            m_pNddsUserData.sPoiMyFavorite.m_OfficeMlClass = atoi(pResStruct->poiMyFavoriteDto.officeMlClass);
        strcpy(m_pNddsUserData.sPoiMyFavorite.m_OfficeRoadName, pResStruct->poiMyFavoriteDto.officeRoadName);
        strcpy(m_pNddsUserData.sPoiMyFavorite.m_OfficeBldNo1, pResStruct->poiMyFavoriteDto.officeBldNo1);
        strcpy(m_pNddsUserData.sPoiMyFavorite.m_OfficeBldNo2, pResStruct->poiMyFavoriteDto.officeBldNo2);
        m_pNddsUserData.sPoiMyFavorite.m_OfficeRpFlag = pResStruct->poiMyFavoriteDto.officeRpFlag;
        strcpy(m_pNddsUserData.sPoiMyFavorite.m_OfficeAddInfo, pResStruct->poiMyFavoriteDto.officeAddInfo);
        strcpy(m_pNddsUserData.sPoiMyFavorite.m_OfficeInsDatetime, pResStruct->poiMyFavoriteDto.officeInsDatetime);

        //routeMyFavorite
        strcpy(m_pNddsUserData.sRouteMyFavorite.routeTitle, pResStruct->routeMyFavoriteDto.routeTitle);
        strcpy(m_pNddsUserData.sRouteMyFavorite.insDatetime, pResStruct->routeMyFavoriteDto.insDatetime);
        //start Route
        strcpy(m_pNddsUserData.sRouteMyFavorite.sStartRoute.szCustName, pResStruct->routeMyFavoriteDto.startRouteDto.name);
        if(pResStruct->routeMyFavoriteDto.startRouteDto.poiId)
            m_pNddsUserData.sRouteMyFavorite.sStartRoute.nPoiID = atoi(pResStruct->routeMyFavoriteDto.startRouteDto.poiId);
        m_pNddsUserData.sRouteMyFavorite.sStartRoute.byRpFlag = pResStruct->routeMyFavoriteDto.startRouteDto.rpFlag;
        if(pResStruct->routeMyFavoriteDto.startRouteDto.navX)
            m_pNddsUserData.sRouteMyFavorite.sStartRoute.tp.x = atoi(pResStruct->routeMyFavoriteDto.startRouteDto.navX);
        if(pResStruct->routeMyFavoriteDto.startRouteDto.navY)
            m_pNddsUserData.sRouteMyFavorite.sStartRoute.tp.y = atoi(pResStruct->routeMyFavoriteDto.startRouteDto.navY);
        //end Route
        strcpy(m_pNddsUserData.sRouteMyFavorite.sEndRoute.szCustName, pResStruct->routeMyFavoriteDto.endRouteDto.name);
        if(pResStruct->routeMyFavoriteDto.endRouteDto.poiId)
            m_pNddsUserData.sRouteMyFavorite.sEndRoute.nPoiID = atoi(pResStruct->routeMyFavoriteDto.endRouteDto.poiId);
        m_pNddsUserData.sRouteMyFavorite.sEndRoute.byRpFlag = pResStruct->routeMyFavoriteDto.endRouteDto.rpFlag;
        if(pResStruct->routeMyFavoriteDto.endRouteDto.navX)
            m_pNddsUserData.sRouteMyFavorite.sEndRoute.tp.x = atoi(pResStruct->routeMyFavoriteDto.endRouteDto.navX);
        if(pResStruct->routeMyFavoriteDto.endRouteDto.navY)
            m_pNddsUserData.sRouteMyFavorite.sEndRoute.tp.y = atoi(pResStruct->routeMyFavoriteDto.endRouteDto.navY);

        //mid Route
        for(int i=0;i<pResStruct->routeMyFavoriteDto.nMidList;i++)
        {
            strcpy(m_pNddsUserData.sRouteMyFavorite.sMidRoute[i].szCustName, pResStruct->routeMyFavoriteDto.midRouteDtos[i].name);
            if(pResStruct->routeMyFavoriteDto.midRouteDtos[i].poiId)
                m_pNddsUserData.sRouteMyFavorite.sMidRoute[i].nPoiID = atoi(pResStruct->routeMyFavoriteDto.midRouteDtos[i].poiId);
            m_pNddsUserData.sRouteMyFavorite.sMidRoute[i].byRpFlag = pResStruct->routeMyFavoriteDto.midRouteDtos[i].rpFlag;
            if(pResStruct->routeMyFavoriteDto.midRouteDtos[i].navX)
               m_pNddsUserData.sRouteMyFavorite.sMidRoute[i].tp.x = atoi(pResStruct->routeMyFavoriteDto.midRouteDtos[i].navX);
            if(pResStruct->routeMyFavoriteDto.midRouteDtos[i].navY)
                m_pNddsUserData.sRouteMyFavorite.sMidRoute[i].tp.y = atoi(pResStruct->routeMyFavoriteDto.midRouteDtos[i].navY);
        }

        //poiRecent
        for(int i=0;i<pResStruct->nPoiRecent;i++)
        {
            if(pResStruct->poiRecentDtos[i].poiId)
                m_pNddsUserData.sPoiRecent[i].nPoiID = atoi(pResStruct->poiRecentDtos[i].poiId);
            strcpy(m_pNddsUserData.sPoiRecent[i].szNavSeq, pResStruct->poiRecentDtos[i].navSeq);
            strcpy(m_pNddsUserData.sPoiRecent[i].szCustName, pResStruct->poiRecentDtos[i].navSeq);
            m_pNddsUserData.sPoiRecent[i].tp.x;
            m_pNddsUserData.sPoiRecent[i].tp.y;
            strcpy(m_pNddsUserData.sPoiRecent[i].szLcdName, pResStruct->poiRecentDtos[i].lcdName);
            if(pResStruct->poiRecentDtos[i].mcdName)
                strcpy(m_pNddsUserData.sPoiRecent[i].szMcdName, pResStruct->poiRecentDtos[i].mcdName);
            strcpy(m_pNddsUserData.sPoiRecent[i].szScdName, pResStruct->poiRecentDtos[i].scdName);
            strcpy(m_pNddsUserData.sPoiRecent[i].szDcdName, pResStruct->poiRecentDtos[i].dcdName);;
            strcpy(m_pNddsUserData.sPoiRecent[i].szPrimaryBun, pResStruct->poiRecentDtos[i].primaryBun);
            strcpy(m_pNddsUserData.sPoiRecent[i].szSecondBun, pResStruct->poiRecentDtos[i].secondaryBun);
            m_pNddsUserData.sPoiRecent[i].nMlClass;
            strcpy(m_pNddsUserData.sPoiRecent[i].szRoadName, pResStruct->poiRecentDtos[i].roadName);
            strcpy(m_pNddsUserData.sPoiRecent[i].szBldNo1, pResStruct->poiRecentDtos[i].bldNo1);
            strcpy(m_pNddsUserData.sPoiRecent[i].szBldNo2, pResStruct->poiRecentDtos[i].bldNo2);
            strcpy(m_pNddsUserData.sPoiRecent[i].szRepClsName, pResStruct->poiRecentDtos[i].repClsName);
            strcpy(m_pNddsUserData.sPoiRecent[i].szClsAName, pResStruct->poiRecentDtos[i].clsAName);
            strcpy(m_pNddsUserData.sPoiRecent[i].szClsBName, pResStruct->poiRecentDtos[i].clsBName);
            strcpy(m_pNddsUserData.sPoiRecent[i].szClsCName, pResStruct->poiRecentDtos[i].clsCName);
            strcpy(m_pNddsUserData.sPoiRecent[i].szClsDName, pResStruct->poiRecentDtos[i].clsDName);
            m_pNddsUserData.sPoiRecent[i].byRPFlag = pResStruct->poiRecentDtos[i].rpFlag;
            strcpy(m_pNddsUserData.sPoiRecent[i].szTotalCnt, pResStruct->poiRecentDtos[i].totalCnt);
            strcpy(m_pNddsUserData.sPoiRecent[i].szSvcDate, pResStruct->poiRecentDtos[i].svcDate);
        }

        //poiFavorite
        for(int i=0;i<pResStruct->nPoiFavorite;i++)
        {
            strcpy(m_pNddsUserData.sPoiFavorite[i].szName, pResStruct->poiFavoriteDtos[i].custName);
            if(pResStruct->poiFavoriteDtos[i].noorX)
                m_pNddsUserData.sPoiFavorite[i].tp.x = atoi(pResStruct->poiFavoriteDtos[i].noorX);
            if(pResStruct->poiFavoriteDtos[i].noorY)
                m_pNddsUserData.sPoiFavorite[i].tp.y = atoi(pResStruct->poiFavoriteDtos[i].noorY);
            if(pResStruct->poiFavoriteDtos[i].poiId)
                m_pNddsUserData.sPoiFavorite[i].nPoiID = atoi(pResStruct->poiFavoriteDtos[i].poiId);
            strcpy(m_pNddsUserData.sPoiFavorite[i].szNavseq, pResStruct->poiFavoriteDtos[i].navSeq);
            strcpy(m_pNddsUserData.sPoiFavorite[i].szLcdName, pResStruct->poiFavoriteDtos[i].lcdName);
            if(pResStruct->poiFavoriteDtos[i].mcdName)
                strcpy(m_pNddsUserData.sPoiFavorite[i].szMcdName, pResStruct->poiFavoriteDtos[i].mcdName);
            strcpy(m_pNddsUserData.sPoiFavorite[i].szScdName, pResStruct->poiFavoriteDtos[i].scdName);
            strcpy(m_pNddsUserData.sPoiFavorite[i].szDcdName, pResStruct->poiFavoriteDtos[i].dcdName);
            strcpy(m_pNddsUserData.sPoiFavorite[i].szPrimaryBun, pResStruct->poiFavoriteDtos[i].primaryBun);
            strcpy(m_pNddsUserData.sPoiFavorite[i].szSecondBun, pResStruct->poiFavoriteDtos[i].secondaryBun);
            if(pResStruct->poiFavoriteDtos[i].mlClass)
                m_pNddsUserData.sPoiFavorite[i].nMlClass = atoi(pResStruct->poiFavoriteDtos[i].mlClass);
            strcpy(m_pNddsUserData.sPoiFavorite[i].szRoadName, pResStruct->poiFavoriteDtos[i].roadName);
            strcpy(m_pNddsUserData.sPoiFavorite[i].szBldNo1, pResStruct->poiFavoriteDtos[i].bldNo1);
            strcpy(m_pNddsUserData.sPoiFavorite[i].szBldNo2, pResStruct->poiFavoriteDtos[i].bldNo2);
            strcpy(m_pNddsUserData.sPoiFavorite[i].szRepClsName, pResStruct->poiFavoriteDtos[i].repClsName);
            strcpy(m_pNddsUserData.sPoiFavorite[i].szClsAName, pResStruct->poiFavoriteDtos[i].clsAName);
            strcpy(m_pNddsUserData.sPoiFavorite[i].szClsBName, pResStruct->poiFavoriteDtos[i].clsBName);
            strcpy(m_pNddsUserData.sPoiFavorite[i].szClsCName, pResStruct->poiFavoriteDtos[i].clsCName);
            strcpy(m_pNddsUserData.sPoiFavorite[i].szClsDName, pResStruct->poiFavoriteDtos[i].clsDName);
            m_pNddsUserData.sPoiFavorite[i].byRPFlag = pResStruct->poiFavoriteDtos[i].rpFlag;
            strcpy(m_pNddsUserData.sPoiFavorite[i].szTelNo, pResStruct->poiFavoriteDtos[i].telNo);
            strcpy(m_pNddsUserData.sPoiFavorite[i].szAddInfo, pResStruct->poiFavoriteDtos[i].addInfo);
            strcpy(m_pNddsUserData.sPoiFavorite[i].szOrgCustName, pResStruct->poiFavoriteDtos[i].orgCustName);
            strcpy(m_pNddsUserData.sPoiFavorite[i].szInsDatetime, pResStruct->poiFavoriteDtos[i].insDatetime);
        }

        //routeFavorite
        for(int i=0;i<pResStruct->nRouteFavorite;i++)
        {
            strcpy(m_pNddsUserData.sRouteFavorite[i].routeTitle, pResStruct->routeFavoriteDtos[i].routeTitle);
            strcpy(m_pNddsUserData.sRouteFavorite[i].insDatetime, pResStruct->routeFavoriteDtos[i].insDatetime);
            //start Route
            strcpy(m_pNddsUserData.sRouteFavorite[i].sStartRoute.szCustName, pResStruct->routeFavoriteDtos[i].startRouteDto.name);
            if(pResStruct->routeFavoriteDtos[i].startRouteDto.poiId)
                m_pNddsUserData.sRouteFavorite[i].sStartRoute.nPoiID = atoi(pResStruct->routeFavoriteDtos[i].startRouteDto.poiId);

            m_pNddsUserData.sRouteFavorite[i].sStartRoute.byRpFlag = pResStruct->routeFavoriteDtos[i].startRouteDto.rpFlag;
            if(pResStruct->routeFavoriteDtos[i].startRouteDto.navX)
                m_pNddsUserData.sRouteFavorite[i].sStartRoute.tp.x = atoi(pResStruct->routeFavoriteDtos[i].startRouteDto.navX);
            if(pResStruct->routeFavoriteDtos[i].startRouteDto.navY)
               m_pNddsUserData.sRouteFavorite[i].sStartRoute.tp.y = atoi(pResStruct->routeFavoriteDtos[i].startRouteDto.navY);
            //end Route
            strcpy(m_pNddsUserData.sRouteFavorite[i].sEndRoute.szCustName, pResStruct->routeFavoriteDtos[i].endRouteDto.name);
            if(pResStruct->routeFavoriteDtos[i].endRouteDto.poiId)
                m_pNddsUserData.sRouteFavorite[i].sEndRoute.nPoiID = atoi(pResStruct->routeFavoriteDtos[i].endRouteDto.poiId);
            m_pNddsUserData.sRouteFavorite[i].sEndRoute.byRpFlag = pResStruct->routeFavoriteDtos[i].endRouteDto.rpFlag;
            if(pResStruct->routeFavoriteDtos[i].endRouteDto.navX)
                m_pNddsUserData.sRouteFavorite[i].sEndRoute.tp.x = atoi(pResStruct->routeFavoriteDtos[i].endRouteDto.navX);
            if(pResStruct->routeFavoriteDtos[i].endRouteDto.navY)
                m_pNddsUserData.sRouteFavorite[i].sEndRoute.tp.y = atoi(pResStruct->routeFavoriteDtos[i].endRouteDto.navY);

            //mid Route
            for(int j=0;j<3;j++)
            {
                strcpy(m_pNddsUserData.sRouteFavorite[i].sMidRoute[j].szCustName, pResStruct->routeFavoriteDtos[i].midRouteDtos[j].name);
                if(pResStruct->routeFavoriteDtos[i].midRouteDtos[j].poiId)
                    m_pNddsUserData.sRouteFavorite[i].sMidRoute[j].nPoiID = atoi(pResStruct->routeFavoriteDtos[i].midRouteDtos[j].poiId);
                m_pNddsUserData.sRouteFavorite[i].sMidRoute[j].byRpFlag = pResStruct->routeFavoriteDtos[i].midRouteDtos[j].rpFlag;
                if(pResStruct->routeFavoriteDtos[i].midRouteDtos[j].navX)
                    m_pNddsUserData.sRouteFavorite[i].sMidRoute[j].tp.x = atoi(pResStruct->routeFavoriteDtos[i].midRouteDtos[j].navX);
                if(pResStruct->routeFavoriteDtos[i].midRouteDtos[j].navY)
                    m_pNddsUserData.sRouteFavorite[i].sMidRoute[j].tp.y = atoi(pResStruct->routeFavoriteDtos[i].midRouteDtos[j].navY);
            }
        }
    }

    return 0;
}

int CNddsModule::SearchNddsRemoveRecentDestnation(int nSessionID, eNddsRequestType eRequestType, N_SEARCH_REMOVERECENTDESTNATION *pRemoveRecentPoi, int nRemoveRecentCnt, char *szErrorMsg)
{
    DBGPRINT(_T("SearchNddsRemoveRecentDestnation \n"));

    eNddsErrorCode eErrCode = NDDSERR_UNKNOWN;
    m_eLastSearchType = N_SEARCHTYPE_NDDSREMOVERECENTDESTNATION;

    NDDSTMAP_REMOVERECENTDESTNATION_REQ sRequest;
    memset(&sRequest, 0x00, sizeof(NDDSTMAP_REMOVERECENTDESTNATION_REQ));
    sRequest.poiRecentRemoves = (NDDSTMAP_POIRECENTREMOVE_DTO *) malloc(sizeof(NDDSTMAP_POIRECENTREMOVE_DTO )*nRemoveRecentCnt);
    memset(sRequest.poiRecentRemoves, 0x00, sizeof(NDDSTMAP_POIRECENTREMOVE_DTO)*nRemoveRecentCnt);

    unsigned char pBuffer[50000] = {0,};
    int nPos = 0;

    sRequest.nRemovePoiRecentCnt = nRemoveRecentCnt;

    for(int i=0;i<nRemoveRecentCnt;i++)
    {
        sRequest.poiRecentRemoves[i].custName = (char*)(pBuffer + nPos);
        strcpy(sRequest.poiRecentRemoves[i].custName,	pRemoveRecentPoi[i].szName);
        nPos += strlen(sRequest.poiRecentRemoves[i].custName)+1;

        sRequest.poiRecentRemoves[i].noorX = (char*)(pBuffer + nPos);
        sprintf(sRequest.poiRecentRemoves[i].noorX, "%d", (int)(pRemoveRecentPoi[i].tp.x / (256*2048.0) * 36000.0f + 0.5f));
        nPos += strlen(sRequest.poiRecentRemoves[i].noorX)+1;

        sRequest.poiRecentRemoves[i].noorY = (char*)(pBuffer + nPos);
        sprintf(sRequest.poiRecentRemoves[i].noorY, "%d", (int)(pRemoveRecentPoi[i].tp.y / (256*2048.0) * 36000.0f + 0.5f));
        nPos += strlen(sRequest.poiRecentRemoves[i].noorX)+1;
    }

    if(!NDDSLIB_RequestProtocol(NDDSTYPE_TMAP_REMOVERECENTDESTNATION, eRequestType, (NDDS_COMMON_REQ*)(&sRequest), nSessionID, &eErrCode))
    {
        if(eErrCode == NDDSERR_USERCANCEL)
            strcpy(szErrorMsg, "사용자 취소");
        else
            strcpy(szErrorMsg, "데이터 생성 실패");

        return 0;
    }

    if(eRequestType == NDDSREQUEST_ATONCE)
    {
        return true;
    }

    eNddsStatus eStatus = NDDSSTATUS_WAIT;
    int nProgress = 0;
    DBGPRINT(_T("Wait "));
    unsigned long reqTime = GetTickCount();
    while(eStatus != NDDSSTATUS_COMPLETE && eStatus != NDDSSTATUS_CANCELED)
    {
        unsigned long thisTime = GetTickCount();
        unsigned long wattingTime = thisTime - reqTime;

        if(wattingTime > SEARCH_WAITTING_TIME)
        {
            strcpy(szErrorMsg, "서버 응답 없음");
            eErrCode = NDDSERR_TIMEOUT;
            return false;
        }

        DBGPRINT(_T("."));
        delay(100);
        NDDSLIB_GetStatusProtocol(nSessionID, &eStatus, &nProgress, &eErrCode);
    }

	NDDS_COMMON_RES *pRes = NULL;
    NDDSTMAP_REMOVERECENTDESTNATION_RES *pResStruct = NULL;
    eNddsProtocolType eResProtocolType = NDDSTYPE_NONE;
    NDDSLIB_GetResultProtocol(nSessionID, true, &eResProtocolType, &pRes, &eErrCode);
    pResStruct = (NDDSTMAP_REMOVERECENTDESTNATION_RES*)pRes;

	if(pResStruct != NULL)
	{
		IsChangeAccesskey(nSessionID, pResStruct->header.accessKey);
		CheckErrorCode(nSessionID, pResStruct->header.errorCode);
	}
    if(!NddsErrorCheck(eErrCode, pResStruct?pResStruct->header.errorMessage:NULL, szErrorMsg))
    {
        NDDSLIB_ReleaseObject(pRes, eErrCode);
        return 0;
    }

	if (pResStruct == NULL)
	{
        eErrCode = NDDSERR_RESULT_NULL;
		return false;
	}

    NDDSLIB_ReleaseObject(pRes);

    DBGPRINT(_T("SearchNddsRemoveRecentDestnation \n"));
    return 0;
}

int CNddsModule::SearchNddsUploadRecentDestnation(int nSessionID, eNddsRequestType eRequestType, N_SEARCH_UPLOADRECENTDESTNATION *pUploadRecentPoi, int nUploadRecentCnt, char* szSvcUploadYn, char *szErrorMsg)
{
    DBGPRINT(_T("SearchNddsUploadRecentDestnation \n"));
    DBGPRINT(_T("SearchNddsUploadRecentDestnation #2 \n"));

    eNddsErrorCode eErrCode = NDDSERR_UNKNOWN;
    m_eLastSearchType = N_SEARCHTYPE_NDDSUPLOADRECENTDESTNATION;

    NDDSTMAP_UPLOADRECENTDESTNATION_REQ sRequest;
    memset(&sRequest, 0x00, sizeof(NDDSTMAP_UPLOADRECENTDESTNATION_REQ));

    unsigned char pBuffer[50000] = {0,};
    int nPos = 0;

    sRequest.nPoiRecentUploads = nUploadRecentCnt;

	sRequest.svcUploadYn = (char*)(pBuffer + nPos);
	nPos += sprintf(sRequest.svcUploadYn,	"%s", szSvcUploadYn);
	nPos = BYTEALIGN_SIZE(nPos+1);

	sRequest.poiRecentUploads = (NDDSTMAP_POIRECENTUPLOAD_DTO*)(pBuffer + nPos);
	nPos += sizeof(NDDSTMAP_POIRECENTUPLOAD_DTO)*nUploadRecentCnt;
	nPos = BYTEALIGN_SIZE(nPos);

	for(int i=0;i<nUploadRecentCnt;i++)
	{
		sRequest.poiRecentUploads[i].custName = (char*)(pBuffer + nPos);
		nPos += sprintf(sRequest.poiRecentUploads[i].custName,	"%s", pUploadRecentPoi[i].szName);
		nPos = BYTEALIGN_SIZE(nPos+1);

		sRequest.poiRecentUploads[i].navSeq = (char*)(pBuffer + nPos);
		nPos += sprintf(sRequest.poiRecentUploads[i].navSeq,	"%s", pUploadRecentPoi[i].szNavseq);
		nPos = BYTEALIGN_SIZE(nPos+1);

		sRequest.poiRecentUploads[i].noorX = (char*)(pBuffer + nPos);
		int x = (int)(pUploadRecentPoi[i].tp.x / (256*2048.0) * 36000.0f + 0.5f);
		nPos += sprintf(sRequest.poiRecentUploads[i].noorX, "%d", x);
		nPos = BYTEALIGN_SIZE(nPos+1);

		sRequest.poiRecentUploads[i].noorY = (char*)(pBuffer + nPos);
		int y = (int)(pUploadRecentPoi[i].tp.y / (256*2048.0) * 36000.0f + 0.5f);
		nPos += sprintf(sRequest.poiRecentUploads[i].noorY, "%d", y);
		nPos = BYTEALIGN_SIZE(nPos+1);

		sRequest.poiRecentUploads[i].rpFlag = pUploadRecentPoi[i].RPFlag;

		sRequest.poiRecentUploads[i].poiId = (char*)(pBuffer + nPos);
		nPos += sprintf(sRequest.poiRecentUploads[i].poiId,	"%d", pUploadRecentPoi[i].nPoiID);
		nPos = BYTEALIGN_SIZE(nPos+1);

		sRequest.poiRecentUploads[i].svcDate = (char*)(pBuffer + nPos);
		nPos += sprintf(sRequest.poiRecentUploads[i].svcDate,	"%s", pUploadRecentPoi[i].szSvcDate);
		nPos = BYTEALIGN_SIZE(nPos+1);

		sRequest.poiRecentUploads[i].totalCnt = (char*)(pBuffer + nPos);
		nPos += sprintf(sRequest.poiRecentUploads[i].totalCnt,	"%s", pUploadRecentPoi[i].szTotalCnt);
		nPos = BYTEALIGN_SIZE(nPos+1);
	}

    if(!NDDSLIB_RequestProtocol(NDDSTYPE_TMAP_UPLOADRECENTDESTNATION, eRequestType, (NDDS_COMMON_REQ*)(&sRequest), nSessionID, &eErrCode))
    {
        if(eErrCode == NDDSERR_USERCANCEL)
            strcpy(szErrorMsg, "사용자 취소");
        else
            strcpy(szErrorMsg, "데이터 생성 실패");

        return 0;
    }

    if(eRequestType == NDDSREQUEST_ATONCE)
    {
        return true;
    }

    eNddsStatus eStatus = NDDSSTATUS_WAIT;
    int nProgress = 0;
    DBGPRINT(_T("Wait "));
    unsigned long reqTime = GetTickCount();
    while(eStatus != NDDSSTATUS_COMPLETE && eStatus != NDDSSTATUS_CANCELED)
    {
        unsigned long thisTime = GetTickCount();
        unsigned long wattingTime = thisTime - reqTime;

        if(wattingTime > SEARCH_WAITTING_TIME)
        {
            strcpy(szErrorMsg, "서버 응답 없음");
            eErrCode = NDDSERR_TIMEOUT;
            return false;
        }

        DBGPRINT(_T("."));
        delay(100);
        NDDSLIB_GetStatusProtocol(nSessionID, &eStatus, &nProgress, &eErrCode);
    }

	NDDS_COMMON_RES *pRes = NULL;
    NDDSTMAP_UPLOADRECENTDESTNATION_RES *pResStruct = NULL;
    eNddsProtocolType eResProtocolType = NDDSTYPE_NONE;
    NDDSLIB_GetResultProtocol(nSessionID, true, &eResProtocolType, &pRes, &eErrCode);
    pResStruct = (NDDSTMAP_UPLOADRECENTDESTNATION_RES*)pRes;

	if(pResStruct != NULL)
	{
		IsChangeAccesskey(nSessionID, pResStruct->header.accessKey);
		CheckErrorCode(nSessionID, pResStruct->header.errorCode);
	}

    if(!NddsErrorCheck(eErrCode, pResStruct?pResStruct->header.errorMessage:NULL, szErrorMsg))
    {
        NDDSLIB_ReleaseObject(pRes, eErrCode);
        return 0;
    }

	if (pResStruct == NULL)
	{
        eErrCode = NDDSERR_RESULT_NULL;
		return false;
	}

    NDDSLIB_ReleaseObject(pRes);


    return 0;
}

int CNddsModule::GetNddsPoiJoinInfoResult(int nSessionID, int *nErrorCode)
{
	eNddsErrorCode eErrCode = NDDSERR_UNKNOWN;
	char szErrorMsg[256] = {0,};

	NDDS_COMMON_RES *pRes = NULL;
	NDDSTMAP_FINDPOIJOININFO_RES *pResStruct = NULL;
	eNddsProtocolType eResProtocolType = NDDSTYPE_NONE;
	NDDSLIB_GetResultProtocol(nSessionID, false, &eResProtocolType, &pRes, &eErrCode);
	pResStruct = (NDDSTMAP_FINDPOIJOININFO_RES*)pRes;

	if(pResStruct != NULL)
	{
		IsChangeAccesskey(nSessionID, pResStruct->header.accessKey);
		CheckErrorCode(nSessionID, pResStruct->header.errorCode);
	}

	if(!NddsErrorCheck(eErrCode, pResStruct?pResStruct->header.errorMessage:NULL, szErrorMsg))
	{
        NDDSLIB_ReleaseObject(pRes, eErrCode);
		return 0;
	}

	if (pResStruct == NULL)
	{
		*nErrorCode = NDDSERR_RESULT_NULL;
		return false;
	}

	if(pResStruct != NULL)
	{
		m_nReadJoinInfoCnt = pResStruct->nPoiJoinInfoViews;
		for(int i=0 ; i<m_nReadJoinInfoCnt ; i++)
		{
			if(i > 8)
			{
				DBGPRINT(_T("WARNING : POI JOIN INFO CNT %d\n"), m_nReadJoinInfoCnt);
				m_nReadJoinInfoCnt = 8;
				break;
			}

            if(pResStruct->poiJoinInfoViews[i].poiId)
                m_pNddsPoiJoinInfo[i].m_PoiID = atoi(pResStruct->poiJoinInfoViews[i].poiId);
			if(pResStruct->poiJoinInfoViews[i].joinType)
				strcpy(m_pNddsPoiJoinInfo[i].m_JoinType, pResStruct->poiJoinInfoViews[i].joinType);
			if(pResStruct->poiJoinInfoViews[i].poiName)
				strcpy(m_pNddsPoiJoinInfo[i].m_Name, pResStruct->poiJoinInfoViews[i].poiName);

			if(pResStruct->poiJoinInfoViews[i].cashRate)
				strcpy(m_pNddsPoiJoinInfo[i].m_CashRate, pResStruct->poiJoinInfoViews[i].cashRate);
			if(pResStruct->poiJoinInfoViews[i].cardRate)
				strcpy(m_pNddsPoiJoinInfo[i].m_CardRate, pResStruct->poiJoinInfoViews[i].cardRate);

			if(pResStruct->poiJoinInfoViews[i].workTime)
				strcpy(m_pNddsPoiJoinInfo[i].m_WorkTime, pResStruct->poiJoinInfoViews[i].workTime);
			if(pResStruct->poiJoinInfoViews[i].weekendWorkTime)
				strcpy(m_pNddsPoiJoinInfo[i].m_WeekendWorkTime, pResStruct->poiJoinInfoViews[i].weekendWorkTime);

			if(pResStruct->poiJoinInfoViews[i].http)
				strcpy(m_pNddsPoiJoinInfo[i].m_Url, pResStruct->poiJoinInfoViews[i].http);

			if(pResStruct->poiJoinInfoViews[i].parkYn)
			{
				if(pResStruct->poiJoinInfoViews[i].parkYn[0] == '0')
					m_pNddsPoiJoinInfo[i].m_Park = '0';
				else if(pResStruct->poiJoinInfoViews[i].parkYn[0] == '1')
					m_pNddsPoiJoinInfo[i].m_Park = '1';
				else
					m_pNddsPoiJoinInfo[i].m_Park = '2';
			}

			if(pResStruct->poiJoinInfoViews[i].benefit)
				strcpy(m_pNddsPoiJoinInfo[i].m_Benefit, pResStruct->poiJoinInfoViews[i].benefit);

			if(pResStruct->poiJoinInfoViews[i].useGuide)
				strcpy(m_pNddsPoiJoinInfo[i].m_UseGuide, pResStruct->poiJoinInfoViews[i].useGuide);

			if(pResStruct->poiJoinInfoViews[i].information)
			{
				if(strlen(pResStruct->poiJoinInfoViews[i].information) > MAXLEN_JOININFO)
				{
					DBGPRINT(_T("WARNING : POI JOIN INFO MAXLEN_JOININFO %d\n"), strlen(pResStruct->poiJoinInfoViews[i].information));
					strncpy(m_pNddsPoiJoinInfo[i].m_Information, pResStruct->poiJoinInfoViews[i].information, MAXLEN_JOININFO);
				}
				else
					strcpy(m_pNddsPoiJoinInfo[i].m_Information, pResStruct->poiJoinInfoViews[i].information);
			}

			int j=0;
			for(j=0; j<pResStruct->poiJoinInfoViews[i].nItem; j++)
			{
				if(i > MAX_POIJOININFO)
				{
					DBGPRINT(_T("WARNING : POI JOIN INFO MAX_POI_JOIN_INFO %d\n"), pResStruct->poiJoinInfoViews[i].nItem);
					break;
				}
				if(pResStruct->poiJoinInfoViews[i].itemInfos[j])
					strcpy(m_pNddsPoiJoinInfo[i].m_ItemInfos[j], pResStruct->poiJoinInfoViews[i].itemInfos[j]);

				pResStruct->poiJoinInfoViews[i].nItem++;
			}
		}
	}

    NDDSLIB_ReleaseObject(pRes);

	return m_nReadJoinInfoCnt;
}

int CNddsModule::SearchNddsRecentDestnation(int nSessionID, eNddsRequestType eRequestType, char *szErrorMsg)
{
	eNddsErrorCode eErrCode = NDDSERR_UNKNOWN;

	m_eLastSearchType = N_SEARCHTYPE_NDDSRECENTDESTNATION;

	memset(m_pNddsRecentDestnation, 0x00, sizeof(m_pNddsRecentDestnation));
	m_nReadRecentCnt = 0;
	*szErrorMsg = NULL;

	NDDSTMAP_FINDRECENTDESTNATION_REQ sRequest;
	memset(&sRequest, 0x00, sizeof(NDDSTMAP_FINDRECENTDESTNATION_REQ));

	if(!NDDSLIB_RequestProtocol(NDDSTYPE_TMAP_FINDRECENTDESTNATION, eRequestType, (NDDS_COMMON_REQ*)(&sRequest), nSessionID, &eErrCode))
	{
		if(eErrCode == NDDSERR_USERCANCEL)
			strcpy(szErrorMsg, "사용자 취소");
		else
			strcpy(szErrorMsg, "데이터 생성 실패");

		return 0;
	}

	if(eRequestType == NDDSREQUEST_ATONCE)
	{
		return true;
	}

    eNddsStatus eStatus = NDDSSTATUS_WAIT;
    int nProgress = 0;
    DBGPRINT(_T("Wait "));
    unsigned long reqTime = GetTickCount();
    while(eStatus != NDDSSTATUS_COMPLETE && eStatus != NDDSSTATUS_CANCELED)
    {
        unsigned long thisTime = GetTickCount();
        unsigned long wattingTime = thisTime - reqTime;

        if(wattingTime > SEARCH_WAITTING_TIME)
        {
            strcpy(szErrorMsg, "서버 응답 없음");
            eErrCode = NDDSERR_TIMEOUT;
            return false;
        }

        DBGPRINT(_T("."));
        delay(100);
        NDDSLIB_GetStatusProtocol(nSessionID, &eStatus, &nProgress, &eErrCode);
    }

	int nNddsErrorCode = -1;

	return GetNddsRecentDestnationResult(nSessionID, &nNddsErrorCode);
}

int CNddsModule::GetNddsRecentDestnationResult(int nSessionID, int *nErrorCode)
{
	eNddsErrorCode eErrCode = NDDSERR_UNKNOWN;
	char szErrorMsg[256] = {0,};

	NDDS_COMMON_RES *pRes = NULL;
	NDDSTMAP_FINDRECENTDESTNATION_RES *pResStruct = NULL;
	eNddsProtocolType eResProtocolType = NDDSTYPE_NONE;
	NDDSLIB_GetResultProtocol(nSessionID, false, &eResProtocolType, &pRes, &eErrCode);
	pResStruct = (NDDSTMAP_FINDRECENTDESTNATION_RES*)pRes;

	if(pResStruct != NULL)
	{
		IsChangeAccesskey(nSessionID, pResStruct->header.accessKey);
		CheckErrorCode(nSessionID, pResStruct->header.errorCode);
	}

	if(!NddsErrorCheck(eErrCode, pResStruct?pResStruct->header.errorMessage:NULL, szErrorMsg))
	{
        NDDSLIB_ReleaseObject(pRes, eErrCode);
		return 0;
	}

	if (pResStruct == NULL)
	{
		*nErrorCode = NDDSERR_RESULT_NULL;
		return false;
	}

	if(pResStruct->nPoiRecents > 0)
	{
		memset(m_pNddsRecentDestnation, 0x00, sizeof(m_pNddsRecentDestnation));

		for(int i=0; i<pResStruct->nPoiRecents; i++)
		{
            if(pResStruct->poiRecents[i].poiId)
                m_pNddsRecentDestnation[i].m_PoiID = atoi(pResStruct->poiRecents[i].poiId);

			if(pResStruct->poiRecents[i].navSeq)
				strcpy(m_pNddsRecentDestnation[i].m_NavSeq, pResStruct->poiRecents[i].navSeq);

			strcpy(m_pNddsRecentDestnation[i].m_Name, pResStruct->poiRecents[i].custName);

            if(pResStruct->poiRecents[i].noorX)
                m_pNddsRecentDestnation[i].m_X = atoi(pResStruct->poiRecents[i].noorX);
            if(pResStruct->poiRecents[i].noorY)
                m_pNddsRecentDestnation[i].m_Y = atoi(pResStruct->poiRecents[i].noorY);

			if(pResStruct->poiRecents[i].rpFlag)
				m_pNddsRecentDestnation[i].m_RPFlag = pResStruct->poiRecents[i].rpFlag;

			if(pResStruct->poiRecents[i].svcDate)
				strcpy(m_pNddsRecentDestnation[i].m_SvcDate, pResStruct->poiRecents[i].svcDate);
		}

		m_nReadRecentCnt = pResStruct->nPoiRecents;
	}

    NDDSLIB_ReleaseObject(pRes);

	return m_nReadRecentCnt;
}

int CNddsModule::SearchNddsAreaNamesByStep(int nSessionID, eNddsRequestType eRequestType, char *szAddrName1, char *szAddrName2, bool bNewAddrChk, char *szErrorMsg)
{
	eNddsErrorCode eErrCode = NDDSERR_UNKNOWN;

	m_eLastSearchType = N_SEARCHTYPE_NDDSAREANAMESBYSTEP;

	memset(m_pNddsPoiResult, 0x00, sizeof(NstNddsPoiResult)*MaxNddsSchResultBufCnt);
	m_nReadCnt = 0;
	*szErrorMsg = NULL;

	NDDSTMAP_FINDAREANAMESBYSTEP_REQ sRequest;
	memset(&sRequest, 0x00, sizeof(NDDSTMAP_FINDAREANAMESBYSTEP_REQ));

	char szAddrName[20] = {0,};

	sRequest.reqCnt = 200;		// 서버에서 전달되는 최대값
	sRequest.reqSeq = 1;
	sRequest.largeCodeName = szAddrName1;//szAddrName;	//[N] 주소 대분류 명, 예) 서울
	sRequest.middleCodeName = szAddrName2;//szSearchName; //[N] 주소 중분류 명, 예) 중구
	if( bNewAddrChk )						//[O] 주소옵션(“Addr”: 읍면동검색, “Road”: 도로명검색) middleCodeName(주소중분류명)이 있을경우 필수
		sRequest.addressFlag = "Road";	
	else
		sRequest.addressFlag = "Addr";	

	if(!NDDSLIB_RequestProtocol(NDDSTYPE_TMAP_FINDAREANAMESBYSTEP, eRequestType, (NDDS_COMMON_REQ*)(&sRequest), nSessionID, &eErrCode))
	{
		if(eErrCode == NDDSERR_USERCANCEL)
			strcpy(szErrorMsg, "사용자 취소");
		else
			strcpy(szErrorMsg, "데이터 생성 실패");

		return 0;
	}

	if(eRequestType == NDDSREQUEST_ATONCE)
	{
		return true;
	}

    eNddsStatus eStatus = NDDSSTATUS_WAIT;
    int nProgress = 0;
    DBGPRINT(_T("Wait "));
    unsigned long reqTime = GetTickCount();
    while(eStatus != NDDSSTATUS_COMPLETE && eStatus != NDDSSTATUS_CANCELED)
    {
        unsigned long thisTime = GetTickCount();
        unsigned long wattingTime = thisTime - reqTime;

        if(wattingTime > SEARCH_WAITTING_TIME)
        {
            strcpy(szErrorMsg, "서버 응답 없음");
            eErrCode = NDDSERR_TIMEOUT;
            return false;
        }

        DBGPRINT(_T("."));
        delay(100);
        NDDSLIB_GetStatusProtocol(nSessionID, &eStatus, &nProgress, &eErrCode);
    }

    int nNddsErrorCode = -1;

	return GetNddsAreaNamesByStepResult(nSessionID, &nNddsErrorCode);
}

int CNddsModule::GetNddsAreaNamesByStepResult(int nSessionID, int *nErrorCode)
{
	eNddsErrorCode eErrCode = NDDSERR_UNKNOWN;
	char szErrorMsg[256] = {0,};

	NDDS_COMMON_RES *pRes = NULL;
	NDDSTMAP_FINDAREANAMESBYSTEP_RES *pResStruct = NULL;
	eNddsProtocolType eResProtocolType = NDDSTYPE_NONE;
	NDDSLIB_GetResultProtocol(nSessionID, false, &eResProtocolType, &pRes, &eErrCode);
	pResStruct = (NDDSTMAP_FINDAREANAMESBYSTEP_RES*)pRes;

	if(pResStruct != NULL)
	{
		IsChangeAccesskey(nSessionID, pResStruct->header.accessKey);
		CheckErrorCode(nSessionID, pResStruct->header.errorCode);
	}

	if(!NddsErrorCheck(eErrCode, pResStruct?pResStruct->header.errorMessage:NULL, szErrorMsg))
	{
        NDDSLIB_ReleaseObject(pRes, eErrCode);
		return 0;
	}

	if (pResStruct == NULL)
	{
		*nErrorCode = NDDSERR_RESULT_NULL;
		return false;
	}

	m_nReadCnt = pResStruct->listCnt;
	for(int i=0 ; i<m_nReadCnt ; i++)
	{
		strcpy(m_pNddsPoiResult[i].m_name, &pResStruct->poiAreaCodes[i].areaName[0]);
	}

    NDDSLIB_ReleaseObject(pRes);

	return m_nReadCnt;
}


int CNddsModule::SearchNddsPoiStationInfo(int nSessionID, eNddsRequestType eRequestType, stVERTEX32 *pwpCurrent, int nDist, char *szDataKind, char *szFuelType, char *szErrorMsg)
{
    eNddsErrorCode eErrCode = NDDSERR_UNKNOWN;

    m_eLastSearchType = N_SEARCHTYPE_NDDSPOISTATIONINFO;

    memset(m_pNddsPoiResult, 0x00, sizeof(NstNddsPoiResult)*MaxNddsSchResultBufCnt);
    *szErrorMsg = NULL;

    NDDSTMAP_FINDPOISTATIONINFO_REQ sRequest;
    memset(&sRequest, 0x00, sizeof(NDDSTMAP_FINDPOISTATIONINFO_REQ));

    char szAddrName[20] = {0,};

    char szX[10], szY[10], szRadius[10];
    unsigned int x, y;

    x = (unsigned int)(pwpCurrent->x);
    y = (unsigned int)(pwpCurrent->y);
    sprintf(szX, "%u", x);
    sprintf(szY, "%u", y);
    sprintf(szRadius, "%d", nDist);

    sRequest.noorX = szX;
    sRequest.noorY = szY;
    sRequest.radius = szRadius;
    sRequest.dataKind = szDataKind;
    sRequest.fuelType = szFuelType;

    if(!NDDSLIB_RequestProtocol(NDDSTYPE_TMAP_FINDPOISTATIONINFO, eRequestType, (NDDS_COMMON_REQ*)(&sRequest), nSessionID, &eErrCode))
    {
        if(eErrCode == NDDSERR_USERCANCEL)
            strcpy(szErrorMsg, "사용자 취소");
        else
            strcpy(szErrorMsg, "데이터 생성 실패");
        return 0;
    }

    if(eRequestType == NDDSREQUEST_ATONCE)
    {
        return true;
    }

    eNddsStatus eStatus = NDDSSTATUS_WAIT;
    int nProgress = 0;
    DBGPRINT(_T("Wait "));
    unsigned long reqTime = GetTickCount();
    while(eStatus != NDDSSTATUS_COMPLETE && eStatus != NDDSSTATUS_CANCELED)
    {
        unsigned long thisTime = GetTickCount();
        unsigned long wattingTime = thisTime - reqTime;

        if(wattingTime > SEARCH_WAITTING_TIME)
        {
            strcpy(szErrorMsg, "서버 응답 없음");
            eErrCode = NDDSERR_TIMEOUT;
            return false;
        }

        DBGPRINT(_T("."));
        delay(100);
        NDDSLIB_GetStatusProtocol(nSessionID, &eStatus, &nProgress, &eErrCode);
    }

    int nNddsErrorCode = -1;
    return GetNddsPoiStationInfoResult(nSessionID, &nNddsErrorCode);
}

int CNddsModule::GetNddsPoiStationInfoResult(int nSessionID, int *nErrorCode)
{
    eNddsErrorCode eErrCode = NDDSERR_UNKNOWN;
    char szErrorMsg[256] = {0,};

	NDDS_COMMON_RES *pRes = NULL;
    NDDSTMAP_FINDPOISTATIONINFO_RES *pResStruct = NULL;
    eNddsProtocolType eResProtocolType = NDDSTYPE_NONE;
    NDDSLIB_GetResultProtocol(nSessionID, false, &eResProtocolType, &pRes, &eErrCode);
    pResStruct = (NDDSTMAP_FINDPOISTATIONINFO_RES*)pRes;

	if(pResStruct != NULL)
	{
		IsChangeAccesskey(nSessionID, pResStruct->header.accessKey);
		CheckErrorCode(nSessionID, pResStruct->header.errorCode);
	}

    if(!NddsErrorCheck(eErrCode, pResStruct?pResStruct->header.errorMessage:NULL, szErrorMsg))
    {
        NDDSLIB_ReleaseObject(pRes, eErrCode);
        return 0;
    }

	if (pResStruct == NULL)
	{
		*nErrorCode = NDDSERR_RESULT_NULL;
		return false;
	}

    if(pResStruct != NULL)
    {
        m_pNddsPoiStationInfo.m_PoiID = atoi(pResStruct->poiId);
        strcpy(m_pNddsPoiStationInfo.m_Name, pResStruct->name);
        m_pNddsPoiStationInfo.m_nOPIGasolinePrice = pResStruct->hhPrice;
        m_pNddsPoiStationInfo.m_nOPIDieselPrice = pResStruct->ggPrice;
        m_pNddsPoiStationInfo.m_nOPILPGPrice = pResStruct->llPrice;
        m_pNddsPoiStationInfo.m_nOPIHiGasolinePrice = pResStruct->highHhPrice;
        m_pNddsPoiStationInfo.m_dataKind = atoi(pResStruct->dataKind);

        m_nReadCnt = 1;
    }

    NDDSLIB_ReleaseObject(pRes);

    return m_nReadCnt;
}

int CNddsModule::SearchNddsPoiEvStationInfo(int nSessionID, eNddsRequestType eRequestType, int nPoiID, char *szErrorMsg)
{
    eNddsErrorCode eErrCode = NDDSERR_UNKNOWN;
    m_eLastSearchType = N_SEARCHTYPE_NDDSPOIEVSTATIONINFO;

    memset(m_pNddsPoiResult, 0x00, sizeof(NstNddsPoiResult)*MaxNddsSchResultBufCnt);
    *szErrorMsg = NULL;

    NDDSTMAP_FINDPOIEVSTATIONINFO_REQ sRequest;
    memset(&sRequest, 0x00, sizeof(NDDSTMAP_FINDPOIEVSTATIONINFO_REQ));
    char poiId[10];
    sprintf(poiId, "%u", nPoiID);
    sRequest.poiId = poiId;

    if(!NDDSLIB_RequestProtocol(NDDSTYPE_TMAP_FINDPOIEVSTATIONINFO, eRequestType, (NDDS_COMMON_REQ*)(&sRequest), nSessionID, &eErrCode))
    {
        if(eErrCode == NDDSERR_USERCANCEL)
            strcpy(szErrorMsg, "사용자 취소");
        else
            strcpy(szErrorMsg, "데이터 생성 실패");

        return 0;
    }

    if(eRequestType == NDDSREQUEST_ATONCE)
    {
        return true;
    }

    eNddsStatus eStatus = NDDSSTATUS_WAIT;
    int nProgress = 0;
    DBGPRINT(_T("Wait "));
    while(eStatus != NDDSSTATUS_COMPLETE && eStatus != NDDSSTATUS_CANCELED)
    {
        DBGPRINT(_T("."));
        delay(100);
        NDDSLIB_GetStatusProtocol(nSessionID, &eStatus, &nProgress, &eErrCode);
    }
    DBGPRINT(_T("\n"));

    int nNddsErrorCode = -1;
    return GetNddsPoiEvStationInfoResult(nSessionID, &nNddsErrorCode);
}

int CNddsModule::SearchNddsPoiEvStationsByPoint(int nSessionID, eNddsRequestType eRequestType, stVERTEX32 *pwpCurrent, int nDist, char *szErrorMsg)
{
    eNddsErrorCode eErrCode = NDDSERR_UNKNOWN;
    m_eLastSearchType = N_SEARCHTYPE_NDDSPOIEVSTATIONSBYPOINT;

    memset(m_pNddsPoiResult, 0x00, sizeof(NstNddsPoiResult)*MaxNddsSchResultBufCnt);
    *szErrorMsg = NULL;

    NDDSTMAP_FINDPOIEVSTATIONSBYPOINT_REQ sRequest;
    memset(&sRequest, 0x00, sizeof(NDDSTMAP_FINDPOIEVSTATIONSBYPOINT_REQ));

    char szX[10]={}, szY[10]={}, szRadius[10]={};
    unsigned int x, y;

    x = (unsigned int)(pwpCurrent->x / (256*2048.0) * 36000.0f + 0.5f);
    y = (unsigned int)(pwpCurrent->y / (256*2048.0) * 36000.0f + 0.5f);
    sprintf(szX, "%u", x);
    sprintf(szY, "%u", y);
    sprintf(szRadius, "%d", nDist);

    sRequest.noorX = szX;
    sRequest.noorY = szY;
    sRequest.radius = szRadius;

    if(!NDDSLIB_RequestProtocol(NDDSTYPE_TMAP_FINDPOIEVSTATIONSBYPOINT, eRequestType, (NDDS_COMMON_REQ*)(&sRequest), nSessionID, &eErrCode))
    {
        if(eErrCode == NDDSERR_USERCANCEL)
            strcpy(szErrorMsg, "사용자 취소");
        else
            strcpy(szErrorMsg, "데이터 생성 실패");

        return 0;
    }

    if(eRequestType == NDDSREQUEST_ATONCE)
    {
        return true;
    }

    eNddsStatus eStatus = NDDSSTATUS_WAIT;
    int nProgress = 0;
    DBGPRINT(_T("Wait "));
    while(eStatus != NDDSSTATUS_COMPLETE && eStatus != NDDSSTATUS_CANCELED)
    {
        DBGPRINT(_T("."));
        delay(100);
        NDDSLIB_GetStatusProtocol(nSessionID, &eStatus, &nProgress, &eErrCode);
    }
    DBGPRINT(_T("\n"));

    int nNddsErrorCode = -1;
    return GetNddsPoiEvStationsByPointResult(nSessionID, &nNddsErrorCode);
}

int CNddsModule::GetNddsPoiEvStationInfoResult(int nSessionID, int *nErrorCode)
{
    eNddsErrorCode eErrCode = NDDSERR_UNKNOWN;
    char szErrorMsg[256] = {0,};

	NDDS_COMMON_RES *pRes = NULL;
    NDDSTMAP_FINDPOIEVSTATIONINFO_RES *pResStruct = NULL;
    eNddsProtocolType eResProtocolType = NDDSTYPE_NONE;
    NDDSLIB_GetResultProtocol(nSessionID, false, &eResProtocolType, &pRes, &eErrCode);
    pResStruct = (NDDSTMAP_FINDPOIEVSTATIONINFO_RES*)pRes;

    memset(&m_pNddsPoiEvStationInfo, 0x00, sizeof(NstNddsPoiEvStationInfo));

	if(pResStruct != NULL)
    {
        IsChangeAccesskey(nSessionID, pResStruct->header.accessKey);
        CheckErrorCode(nSessionID, pResStruct->header.errorCode);
    }

    if(!NddsErrorCheck(eErrCode, pResStruct?pResStruct->header.errorMessage:NULL, szErrorMsg))
    {
        NDDSLIB_ReleaseObject(pRes, eErrCode);
        return 0;
    }

	if (pResStruct == NULL)
	{
		*nErrorCode = NDDSERR_RESULT_NULL;
		return false;
	}

    if(pResStruct != NULL)
    {
        m_pNddsPoiEvStationInfo.nPoiID = atoi(pResStruct->poiId);
        if (pResStruct->stationName)
            strncpy(m_pNddsPoiEvStationInfo.szStationName, pResStruct->stationName,sizeof(m_pNddsPoiEvStationInfo.szStationName)-1);
        if (pResStruct->addr)
            strncpy(m_pNddsPoiEvStationInfo.szAddr, pResStruct->addr, sizeof(m_pNddsPoiEvStationInfo.szAddr)-1);
        if (pResStruct->tel)
            strncpy(m_pNddsPoiEvStationInfo.szTel, pResStruct->tel, sizeof(m_pNddsPoiEvStationInfo.szTel)-1);
        if(pResStruct->freeYn && atoi(pResStruct->freeYn) == 1){
            m_pNddsPoiEvStationInfo.bFreeYn = true;
        }else{
            m_pNddsPoiEvStationInfo.bFreeYn = false;
        }

        if(pResStruct->reservYn && atoi(pResStruct->reservYn) == 1){
            m_pNddsPoiEvStationInfo.bReservYn = true;
        }else{
            m_pNddsPoiEvStationInfo.bReservYn = false;
        }

        if (pResStruct->useTime)
            strncpy(m_pNddsPoiEvStationInfo.szUseTime, pResStruct->useTime, sizeof(m_pNddsPoiEvStationInfo.szUseTime)-1);
        if(pResStruct->payYn && atoi(pResStruct->payYn) == 1){
            m_pNddsPoiEvStationInfo.bPayYn = true;
        }else{
            m_pNddsPoiEvStationInfo.bPayYn = false;
        }

        if (pResStruct->updateDt)
            strncpy(m_pNddsPoiEvStationInfo.szUpdateDt, pResStruct->updateDt, sizeof(m_pNddsPoiEvStationInfo.szUpdateDt)-1);
        m_pNddsPoiEvStationInfo.wp.x = atoi(pResStruct->noorX);
        m_pNddsPoiEvStationInfo.wp.y = atoi(pResStruct->noorY);

        if (pResStruct->totalCnt> MAXLEN_EVSTATION_CHARGERS)
            pResStruct->totalCnt = MAXLEN_EVSTATION_CHARGERS;

        m_pNddsPoiEvStationInfo.nTotalCnt = pResStruct->totalCnt;
        for(int i=0;i<m_pNddsPoiEvStationInfo.nTotalCnt;i++)
        {
            if(!(pResStruct->chargerInfo+i))
                break;
            strncpy(m_pNddsPoiEvStationInfo.sEvChargers[i].szBid, pResStruct->chargerInfo[i].bid, sizeof(m_pNddsPoiEvStationInfo.sEvChargers[i].szBid)-1);
            strncpy(m_pNddsPoiEvStationInfo.sEvChargers[i].szSid, pResStruct->chargerInfo[i].sid, sizeof(m_pNddsPoiEvStationInfo.sEvChargers[i].szSid)-1);
            strncpy(m_pNddsPoiEvStationInfo.sEvChargers[i].szCid, pResStruct->chargerInfo[i].cid, sizeof(m_pNddsPoiEvStationInfo.sEvChargers[i].szCid)-1);
            strncpy(m_pNddsPoiEvStationInfo.sEvChargers[i].szType, pResStruct->chargerInfo[i].type, sizeof(m_pNddsPoiEvStationInfo.sEvChargers[i].szType)-1);
            m_pNddsPoiEvStationInfo.sEvChargers[i].nStatus= atoi(pResStruct->chargerInfo[i].status);
        }
        m_nReadCnt = 1;
    }

    NDDSLIB_ReleaseObject(pRes);

    return m_nReadCnt;
}

int CNddsModule::GetNddsPoiEvStationsByPointResult(int nSessionID, int *nErrorCode)
{
    eNddsErrorCode eErrCode = NDDSERR_UNKNOWN;
    char szErrorMsg[256] = {0,};

	NDDS_COMMON_RES *pRes = NULL;
    NDDSTMAP_FINDPOIEVSTATIONSBYPOINT_RES *pResStruct = NULL;
    eNddsProtocolType eResProtocolType = NDDSTYPE_NONE;
    NDDSLIB_GetResultProtocol(nSessionID, false, &eResProtocolType, &pRes, &eErrCode);
    pResStruct = (NDDSTMAP_FINDPOIEVSTATIONSBYPOINT_RES*)pRes;

    memset(&m_pNddsPoiEvStationsByPoint, 0x00, sizeof(NstNddsPoiEvStationsByPoint));

	if(pResStruct != NULL)
    {
        IsChangeAccesskey(nSessionID, pResStruct->header.accessKey);
        CheckErrorCode(nSessionID, pResStruct->header.errorCode);
    }

    if(!NddsErrorCheck(eErrCode, pResStruct?pResStruct->header.errorMessage:NULL, szErrorMsg))
    {
        NDDSLIB_ReleaseObject(pRes, eErrCode);
        return 0;
    }

	if (pResStruct == NULL)
	{
		*nErrorCode = NDDSERR_RESULT_NULL;
		return false;
	}

    if(pResStruct != NULL)
    {
        m_pNddsPoiEvStationsByPoint.nTotalCnt = pResStruct->totalCnt;
        for(int i=0;i<m_pNddsPoiEvStationsByPoint.nTotalCnt;i++)
        {
            if(i>=200)
            {
                break;
            }
            m_pNddsPoiEvStationsByPoint.evStations[i].nPoiID = atoi(pResStruct->evstations[i].poiId);
            strncpy(m_pNddsPoiEvStationsByPoint.evStations[i].szStationName, pResStruct->evstations[i].stationName, sizeof(m_pNddsPoiEvStationsByPoint.evStations[i].szStationName)-1);
            strncpy(m_pNddsPoiEvStationsByPoint.evStations[i].szAddr, pResStruct->evstations[i].addr, sizeof(m_pNddsPoiEvStationsByPoint.evStations[i].szAddr)-1);
            if(pResStruct->evstations[i].mngName)
                strncpy(m_pNddsPoiEvStationsByPoint.evStations[i].szMngName, pResStruct->evstations[i].mngName, sizeof(m_pNddsPoiEvStationsByPoint.evStations[i].szMngName)-1);
            m_pNddsPoiEvStationsByPoint.evStations[i].wp.x = atoi(pResStruct->evstations[i].noorX);
            m_pNddsPoiEvStationsByPoint.evStations[i].wp.y = atoi(pResStruct->evstations[i].noorY);
            if(pResStruct->evstations[i].radius)
                 m_pNddsPoiEvStationsByPoint.evStations[i].nDist = (int)(atof(pResStruct->evstations[i].radius) * 1000.0);

            if(pResStruct->evstations[i].totalCnt > 0)
            {
                if(pResStruct->evstations[i].totalCnt > MAXLEN_EVSTATION_CHARGERS)
                {
                    printf("More than 25 evStation %d\n", pResStruct->evstations[i].totalCnt); fflush(stdout);
                    m_pNddsPoiEvStationsByPoint.evStations[i].nTotalCnt = MAXLEN_EVSTATION_CHARGERS;
                }
                else
                {
                    m_pNddsPoiEvStationsByPoint.evStations[i].nTotalCnt = pResStruct->evstations[i].totalCnt;
                }

                for(int j=0; j<m_pNddsPoiEvStationsByPoint.evStations[i].nTotalCnt; j++)
                {
                    if(pResStruct->evstations[i].evChargers[j].bid)
                        strncpy(m_pNddsPoiEvStationsByPoint.evStations[i].sEvChargers[j].szBid, pResStruct->evstations[i].evChargers[j].bid, sizeof(m_pNddsPoiEvStationsByPoint.evStations[i].sEvChargers[j].szBid)-1);
                    if(pResStruct->evstations[i].evChargers[j].sid)
                        strncpy(m_pNddsPoiEvStationsByPoint.evStations[i].sEvChargers[j].szSid, pResStruct->evstations[i].evChargers[j].sid, sizeof(m_pNddsPoiEvStationsByPoint.evStations[i].sEvChargers[j].szSid)-1);
                    if(pResStruct->evstations[i].evChargers[j].cid)
                        strncpy(m_pNddsPoiEvStationsByPoint.evStations[i].sEvChargers[j].szCid, pResStruct->evstations[i].evChargers[j].cid, sizeof(m_pNddsPoiEvStationsByPoint.evStations[i].sEvChargers[j].szCid)-1);
                    if(pResStruct->evstations[i].evChargers[j].type)
                        strncpy(m_pNddsPoiEvStationsByPoint.evStations[i].sEvChargers[j].szType, pResStruct->evstations[i].evChargers[j].type, sizeof(m_pNddsPoiEvStationsByPoint.evStations[i].sEvChargers[j].szType)-1);
                    if(pResStruct->evstations[i].evChargers[j].status)
                        m_pNddsPoiEvStationsByPoint.evStations[i].sEvChargers[j].nStatus= atoi(pResStruct->evstations[i].evChargers[j].status);
                }

            }
        }

        m_nReadCnt = m_pNddsPoiEvStationsByPoint.nTotalCnt;
    }
    else
    {
        m_nReadCnt = 0;
    }

    NDDSLIB_ReleaseObject(pRes);

    return m_nReadCnt;
}

//////////////////////////////////////
// Favorite API
//////////////////////////////////////

bool CNddsModule::API_NDDS_Favorite(PAPI_FAVORITE_REQ pRequest, PAPI_FAVORITE_RES pResult)
{
	//DBGPRINT(_T("############################# Favorite #1\n"));

	// 모듈 프로세스 스레드에 요청 중이거나 동작중인지 체크 하여 있으면 검색이면 중지후 요청하고 나머지는 실패 처리한다.
#ifdef _LINUX
	if(sem_wait(&m_pModuleProcSem) != 0)
#else
	if(SDL_SemWait(m_pModuleProcSem) == SDL_MUTEX_TIMEDOUT)
#endif
	{
		//		if(m_sModuleInputProcData.eProcType != E_PROCTYPE_FAVORITE)
		{
			//pResult->nResult = N_FAVORITEERR_DUPLICATION;
			return false;
		}

		//DBGPRINT(_T("############################# Search #1 - Stop\n"));

		switch(m_sModuleInputProcData.sFavoriteData.sFavoriteInput.eFavoriteType)
		{
		default:
			//assert(0);
			break;
		}
		delay(1); //스레드 스위칭(기존 스레드 처리가 끝날 수 있게 스위칭)
	}

	//DBGPRINT(_T("MUTEX[%d] - API_NDDS_Search::LockMutex #1\n"), GetTickCount());
#ifdef _LINUX
	pthread_mutex_lock(&m_pModuleProcMutex);
#else
	SDL_LockMutex(m_pModuleProcMutex);
#endif
	//DBGPRINT(_T("MUTEX[%d] - API_NDDS_Search::LockMutex #2\n"), GetTickCount());
	//DBGPRINT(_T("############################# Search #2\n"));

	int nSessionID = ++m_nLastIdxSession;

	SendMessgeCallback(NM_NDDS_FAVORITE_INIT, nSessionID, (long)(pRequest->sFavoriteInput.eFavoriteType));

	//////////////////////////////////////
	//Search Input 데이터를 설정한다.

	m_sModuleInputProcData.eProcType = E_NDDS_PROCTYPE_FAVORITE;
	m_sModuleInputProcData.nFavoriteSessionID = nSessionID;
	m_sModuleInputProcData.bFavoriteSync = pRequest->bSync;
	memcpy(&m_sModuleInputProcData.sFavoriteData.sFavoriteInput, &pRequest->sFavoriteInput, sizeof(FAVORITE_INPUT));

	////////////////////////////////
	// 스레드를 동작시킨다.
#ifdef _LINUX
	pthread_cond_signal(&m_pModuleProcCond);
	pthread_mutex_unlock(&m_pModuleProcMutex);
#else
	SDL_CondSignal(m_pModuleProcCond);
	SDL_UnlockMutex(m_pModuleProcMutex);
#endif

	delay(1);	//스레드 스위칭

	if(pRequest->bSync)
	{
		// 스레드 처리가 끝날때 까지 대기한다.
#ifdef _LINUX
		pthread_mutex_lock(&m_pModuleProcMutex);
		pthread_mutex_unlock(&m_pModuleProcMutex);
#else
		SDL_LockMutex(m_pModuleProcMutex);
		SDL_UnlockMutex(m_pModuleProcMutex);
#endif
		pResult->nResult = m_sModuleOutputProcData.sFavoriteData.nResult;
		strcpy(pResult->szErrorMsg, m_sModuleOutputProcData.sFavoriteData.szErrorMsg);
	}

	//DBGPRINT(_T("############################# Favorite #true\n"));
	pResult->nSessionID = nSessionID;

	return true;
}

//즐겨찾기 단건 수정
int CNddsModule::modifyFavoriteNddsPoi(int nSessionID, eNddsRequestType eRequestType, DS_MODIFY_POIFAVORITE_INFO poiFavorite, char *szErrorMsg)
{
    eNddsErrorCode eErrCode = NDDSERR_UNKNOWN;

    m_eLastFavoriteType = N_FAVORITETYPE_NDDSMODIFYPOI;

    NDDSTMAP_MODIFYPOIFAVORITE_REQ sRequest;
    memset(&sRequest, 0x00, sizeof(NDDSTMAP_MODIFYPOIFAVORITE_REQ));


    unsigned char pBuffer[30000] = {0,};
    int nPos = 0;
    sRequest.poiFavorites.custName = (char*)(pBuffer + nPos);
    strcpy(sRequest.poiFavorites.custName,	poiFavorite.szCustName);
    nPos += strlen(sRequest.poiFavorites.custName)+1;

    sRequest.poiFavorites.newCustName = (char*)(pBuffer + nPos);
    strcpy(sRequest.poiFavorites.newCustName,	poiFavorite.szNewCustName);
    nPos += strlen(sRequest.poiFavorites.newCustName)+1;

    sRequest.poiFavorites.iconInfo = (char*)(pBuffer + nPos);
    strcpy(sRequest.poiFavorites.iconInfo,	poiFavorite.szIconInfo);
    nPos += strlen(sRequest.poiFavorites.iconInfo)+1;

    sRequest.poiFavorites.noorX = (char*)(pBuffer + nPos);
    sprintf(sRequest.poiFavorites.noorX, "%d", (int)(poiFavorite.tp.x / (256*2048.0) * 36000.0f + 0.5f));
    nPos += strlen(sRequest.poiFavorites.noorX)+1;

    sRequest.poiFavorites.noorY = (char*)(pBuffer + nPos);
    sprintf(sRequest.poiFavorites.noorY, "%d", (int)(poiFavorite.tp.y / (256*2048.0) * 36000.0f + 0.5f));


    if(!NDDSLIB_RequestProtocol(NDDSTYPE_TMAP_MODIFYPOIFAVORITE, eRequestType, (NDDS_COMMON_REQ*)(&sRequest), nSessionID, &eErrCode))
    {
        if(eErrCode == NDDSERR_USERCANCEL)
            strcpy(szErrorMsg, "사용자 취소");
        else
            strcpy(szErrorMsg, "데이터 생성 실패");

        return 0;
    }

    if(eRequestType == NDDSREQUEST_ATONCE)
    {
        return true;
    }
    eNddsStatus eStatus = NDDSSTATUS_WAIT;
    int nProgress = 0;
    DBGPRINT(_T("Wait "));
    unsigned long reqTime = GetTickCount();
    while(eStatus != NDDSSTATUS_COMPLETE && eStatus != NDDSSTATUS_CANCELED)
    {
        unsigned long thisTime = GetTickCount();
        unsigned long wattingTime = thisTime - reqTime;

        if(wattingTime > SEARCH_WAITTING_TIME)
        {
            strcpy(szErrorMsg, "서버 응답 없음");
            eErrCode = NDDSERR_TIMEOUT;
            return false;
        }

        DBGPRINT(_T("."));
        delay(100);
        NDDSLIB_GetStatusProtocol(nSessionID, &eStatus, &nProgress, &eErrCode);
    }

	NDDS_COMMON_RES *pRes = NULL;
    NDDSTMAP_MODIFYPOIFAVORITE_RES *pResStruct = NULL;
    eNddsProtocolType eResProtocolType = NDDSTYPE_NONE;
    NDDSLIB_GetResultProtocol(nSessionID, true, &eResProtocolType, &pRes, &eErrCode);
    pResStruct = (NDDSTMAP_MODIFYPOIFAVORITE_RES*)pRes;

	if(pResStruct != NULL)
	{
		IsChangeAccesskey(nSessionID, pResStruct->header.accessKey);
		CheckErrorCode(nSessionID, pResStruct->header.errorCode);
	}

    if(!NddsErrorCheck(eErrCode, pResStruct?pResStruct->header.errorMessage:NULL, szErrorMsg))
    {
        NDDSLIB_ReleaseObject(pRes, eErrCode);
        return 0;
    }

	if (pResStruct == NULL)
	{
        eErrCode = NDDSERR_RESULT_NULL;
		return false;
	}

    NDDSLIB_ReleaseObject(pRes);

    DBGPRINT(_T("############################# ModifyFavoriteNddsPoi #true \n"));
    return 0;
}

//즐겨찾기 멀티 삭제
int CNddsModule::RemoveFavoriteNddsPoi(int nSessionID, eNddsRequestType eRequestType, DS_REMOVE_POIFAVORITE_INFO *poiFavorite, int nPoiCnt, char *szErrorMsg)
{
    eNddsErrorCode eErrCode = NDDSERR_UNKNOWN;

    m_eLastFavoriteType = N_FAVORITETYPE_NDDSREMOVEPOI;

    NDDSTMAP_REMOVEPOIFAVORITE_REQ sRequest;
    memset(&sRequest, 0x00, sizeof(NDDSTMAP_REMOVEPOIFAVORITE_REQ));

    unsigned char pBuffer[50000] = {0,};
    int nPos = 0;
    sRequest.poiFavorites = (NDDSTMAP_POIREMOVEPOIFAVORITE_DTO *) malloc(sizeof(NDDSTMAP_POIREMOVEPOIFAVORITE_DTO )*nPoiCnt);
    memset(sRequest.poiFavorites, 0x00, sizeof(NDDSTMAP_POIREMOVEPOIFAVORITE_DTO)*nPoiCnt);

    sRequest.nPoiFavorites = nPoiCnt;

    for(int i=0;i<nPoiCnt;i++)
    {
        sRequest.poiFavorites[i].custName = (char*)(pBuffer + nPos);
        strcpy(sRequest.poiFavorites[i].custName,	poiFavorite[i].szCustName);
        nPos += strlen(sRequest.poiFavorites[i].custName)+1;

        sRequest.poiFavorites[i].noorX = (char*)(pBuffer + nPos);
        sprintf(sRequest.poiFavorites[i].noorX, "%d", (int)(poiFavorite[i].tp.x / (256*2048.0) * 36000.0f + 0.5f));
        nPos += strlen(sRequest.poiFavorites[i].noorX)+1;

        sRequest.poiFavorites[i].noorY = (char*)(pBuffer + nPos);
        sprintf(sRequest.poiFavorites[i].noorY, "%d", (int)(poiFavorite[i].tp.y / (256*2048.0) * 36000.0f + 0.5f));
        nPos += strlen(sRequest.poiFavorites[i].noorY)+1;
    }

    if(!NDDSLIB_RequestProtocol(NDDSTYPE_TMAP_REMOVEPOIFAVORITE, eRequestType, (NDDS_COMMON_REQ*)(&sRequest), nSessionID, &eErrCode))
    {
        if(eErrCode == NDDSERR_USERCANCEL)
            strcpy(szErrorMsg, "사용자 취소");
        else
            strcpy(szErrorMsg, "데이터 생성 실패");

        return 0;
    }

    if(eRequestType == NDDSREQUEST_ATONCE)
    {
        return true;
    }

    eNddsStatus eStatus = NDDSSTATUS_WAIT;
    int nProgress = 0;
    DBGPRINT(_T("Wait "));
    unsigned long reqTime = GetTickCount();
    while(eStatus != NDDSSTATUS_COMPLETE && eStatus != NDDSSTATUS_CANCELED)
    {
        unsigned long thisTime = GetTickCount();
        unsigned long wattingTime = thisTime - reqTime;

        if(wattingTime > SEARCH_WAITTING_TIME)
        {
            strcpy(szErrorMsg, "서버 응답 없음");
            eErrCode = NDDSERR_TIMEOUT;
            return false;
        }

        DBGPRINT(_T("."));
        delay(100);
        NDDSLIB_GetStatusProtocol(nSessionID, &eStatus, &nProgress, &eErrCode);
    }

	NDDS_COMMON_RES *pRes = NULL;
    NDDSTMAP_REMOVEPOIFAVORITE_RES *pResStruct = NULL;
    eNddsProtocolType eResProtocolType = NDDSTYPE_NONE;
    NDDSLIB_GetResultProtocol(nSessionID, true, &eResProtocolType, &pRes, &eErrCode);
    pResStruct = (NDDSTMAP_REMOVEPOIFAVORITE_RES*)pRes;

	if(pResStruct != NULL)
	{
		IsChangeAccesskey(nSessionID, pResStruct->header.accessKey);
		CheckErrorCode(nSessionID, pResStruct->header.errorCode);
	}

    if(!NddsErrorCheck(eErrCode, pResStruct?pResStruct->header.errorMessage:NULL, szErrorMsg))
    {
        NDDSLIB_ReleaseObject(pRes, eErrCode);
        return 0;
    }

	if (pResStruct == NULL)
	{
        eErrCode = NDDSERR_RESULT_NULL;
		return false;
	}

    NDDSLIB_ReleaseObject(pRes);
    DBGPRINT(_T("############################# RemoveFavoriteNddsPoi #true \n"));
    return 0;
}

//즐겨찾기 업로드
int CNddsModule::UploadFavoriteNddsPoi(int nSessionID, eNddsRequestType eRequestType, FAVORITE_NDDSPOI_UPLOAD_INPUT poiFavoriteInput, char *szErrorMsg)
{
    eNddsErrorCode eErrCode = NDDSERR_UNKNOWN;

    m_eLastFavoriteType = N_FAVORITETYPE_NDDSUPLOADPOI;

    NDDSTMAP_REGISTALLPOIFAVORITE_REQ sRequest;
    memset(&sRequest, 0x00, sizeof(NDDSTMAP_REGISTALLPOIFAVORITE_REQ));
    unsigned char pBuffer[50000] = {0,};
    int nPos = 0;

    sRequest.uploadOption = (char*)(pBuffer + nPos);
    strcpy(sRequest.uploadOption, poiFavoriteInput.szUploadOption);
    nPos += strlen(sRequest.uploadOption)+1;

    if(poiFavoriteInput.bMyFavorite)
    {
        sRequest.poiMyFavorite.homeCustName = (char*)(pBuffer + nPos);
        strcpy(sRequest.poiMyFavorite.homeCustName,	poiFavoriteInput.sUploadMyFavorite.szHomeName);
        nPos += strlen(sRequest.poiMyFavorite.homeCustName)+1;

        sRequest.poiMyFavorite.homeNoorX = (char*)(pBuffer + nPos);
        sprintf(sRequest.poiMyFavorite.homeNoorX, "%d", (int)(poiFavoriteInput.sUploadMyFavorite.Hometp.x)); // / (256*2048.0) * 36000.0f + 0.5f));
        nPos += strlen(sRequest.poiMyFavorite.homeNoorX)+1;

        sRequest.poiMyFavorite.homeNoorY = (char*)(pBuffer + nPos);
        sprintf(sRequest.poiMyFavorite.homeNoorY, "%d", (int)(poiFavoriteInput.sUploadMyFavorite.Hometp.y)); // / (256*2048.0) * 36000.0f + 0.5f));
        nPos += strlen(sRequest.poiMyFavorite.homeNoorY)+1;

        sRequest.poiMyFavorite.homePoiId = (char*)(pBuffer + nPos);
        sprintf(sRequest.poiMyFavorite.homePoiId, "%d", poiFavoriteInput.sUploadMyFavorite.nHomePoiID);
        nPos += strlen(sRequest.poiMyFavorite.homePoiId)+1;

        sRequest.poiMyFavorite.homeNavSeq = (char*)(pBuffer + nPos);
        strcpy(sRequest.poiMyFavorite.homeNavSeq,	poiFavoriteInput.sUploadMyFavorite.szHomeNavSeq);
        nPos += strlen(sRequest.poiMyFavorite.homeNavSeq)+1;

        sRequest.poiMyFavorite.homeRpFlag =	poiFavoriteInput.sUploadMyFavorite.byHomeRpFlag;

        sRequest.poiMyFavorite.homeAddInfo = (char*)(pBuffer + nPos);
        strcpy(sRequest.poiMyFavorite.homeAddInfo,	poiFavoriteInput.sUploadMyFavorite.szHomeAddInfo);
        nPos += strlen(sRequest.poiMyFavorite.homeAddInfo)+1;
    }

    sRequest.poiFavorites = (NDDSTMAP_POIUPLOADFAVORITE_DTO *) malloc(sizeof(NDDSTMAP_POIUPLOADFAVORITE_DTO )*poiFavoriteInput.nPoiCnt);
    memset(sRequest.poiFavorites, 0x00, sizeof(NDDSTMAP_POIUPLOADFAVORITE_DTO)*poiFavoriteInput.nPoiCnt);

    sRequest.nPoiFavorites = poiFavoriteInput.nPoiCnt;
    for(int i=0;i<sRequest.nPoiFavorites;i++)
    {
        sRequest.poiFavorites[i].custName = (char*)(pBuffer + nPos);
        strcpy(sRequest.poiFavorites[i].custName,	poiFavoriteInput.sUploadFavoriteList[i].szCustName);
        nPos += strlen(sRequest.poiFavorites[i].custName)+1;

        sRequest.poiFavorites[i].noorX = (char*)(pBuffer + nPos);
        sprintf(sRequest.poiFavorites[i].noorX, "%d", (int)(poiFavoriteInput.sUploadFavoriteList[i].tp.x));// / (256*2048.0) * 36000.0f + 0.5f));
        nPos += strlen(sRequest.poiFavorites[i].noorX)+1;

        sRequest.poiFavorites[i].noorY = (char*)(pBuffer + nPos);
        sprintf(sRequest.poiFavorites[i].noorY, "%d", (int)(poiFavoriteInput.sUploadFavoriteList[i].tp.y));// / (256*2048.0) * 36000.0f + 0.5f));
        nPos += strlen(sRequest.poiFavorites[i].noorY)+1;

        sRequest.poiFavorites[i].poiId = (char*)(pBuffer + nPos);
        sprintf(sRequest.poiFavorites[i].poiId, "%d", poiFavoriteInput.sUploadFavoriteList[i].nPoiID);
        nPos += strlen(sRequest.poiFavorites[i].poiId)+1;

        sRequest.poiFavorites[i].rpFlag = 	poiFavoriteInput.sUploadFavoriteList[i].byRPFlag;

        sRequest.poiFavorites[i].addInfo = (char*)(pBuffer + nPos);
        strcpy(sRequest.poiFavorites[i].addInfo,	poiFavoriteInput.sUploadFavoriteList[i].szAddInfo);
        nPos += strlen(sRequest.poiFavorites[i].addInfo)+1;

        sRequest.poiFavorites[i].orgCustName = (char*)(pBuffer + nPos);
        strcpy(sRequest.poiFavorites[i].orgCustName,	poiFavoriteInput.sUploadFavoriteList[i].szOrgCustName);
        nPos += strlen(sRequest.poiFavorites[i].orgCustName)+1;

        sRequest.poiFavorites[i].iconInfo = (char*)(pBuffer + nPos);
        strcpy(sRequest.poiFavorites[i].iconInfo,	poiFavoriteInput.sUploadFavoriteList[i].szIconInfo);
        nPos += strlen(sRequest.poiFavorites[i].iconInfo)+1;
    }

    if(!NDDSLIB_RequestProtocol(NDDSTYPE_TMAP_UPLOADPOIFAVORITE, eRequestType, (NDDS_COMMON_REQ*)(&sRequest), nSessionID, &eErrCode))
    {
        if(eErrCode == NDDSERR_USERCANCEL)
            strcpy(szErrorMsg, "사용자 취소");
        else
            strcpy(szErrorMsg, "데이터 생성 실패");

        return 0;
    }

    if(eRequestType == NDDSREQUEST_ATONCE)
    {
        return true;
    }

    eNddsStatus eStatus = NDDSSTATUS_WAIT;
    int nProgress = 0;
    DBGPRINT(_T("Wait "));
    unsigned long reqTime = GetTickCount();
    while(eStatus != NDDSSTATUS_COMPLETE && eStatus != NDDSSTATUS_CANCELED)
    {
        unsigned long thisTime = GetTickCount();
        unsigned long wattingTime = thisTime - reqTime;

        if(wattingTime > SEARCH_WAITTING_TIME)
        {
            strcpy(szErrorMsg, "서버 응답 없음");
            eErrCode = NDDSERR_TIMEOUT;
            return false;
        }

        DBGPRINT(_T("."));
        delay(100);
        NDDSLIB_GetStatusProtocol(nSessionID, &eStatus, &nProgress, &eErrCode);
    }

	NDDS_COMMON_RES *pRes = NULL;
    NDDSTMAP_REGISTALLPOIFAVORITE_RES *pResStruct = NULL;
    eNddsProtocolType eResProtocolType = NDDSTYPE_NONE;
    NDDSLIB_GetResultProtocol(nSessionID, true, &eResProtocolType, &pRes, &eErrCode);
    pResStruct = (NDDSTMAP_REGISTALLPOIFAVORITE_RES*)pRes;

	if(pResStruct != NULL)
	{
		IsChangeAccesskey(nSessionID, pResStruct->header.accessKey);
		CheckErrorCode(nSessionID, pResStruct->header.errorCode);
	}

    if(!NddsErrorCheck(eErrCode, pResStruct?pResStruct->header.errorMessage:NULL, szErrorMsg))
    {
        NDDSLIB_ReleaseObject(pRes, eErrCode);
        return 0;
    }

	if (pResStruct == NULL)
	{
        eErrCode = NDDSERR_RESULT_NULL;
		return false;
	}

    NDDSLIB_ReleaseObject(pRes);
    DBGPRINT(_T("############################# UploadFavoriteNddsPoi #true \n"));
    return 0;
}

//즐겨찾기 등록
int CNddsModule::RegistFavoriteNddsPoi(int nSessionID, eNddsRequestType eRequestType, FAVORITE_NDDSPOI_REGIST_INPUT poiFavoriteInput, char *szErrorMsg){
    eNddsErrorCode eErrCode = NDDSERR_UNKNOWN;

    m_eLastFavoriteType = N_FAVORITETYPE_NDDSREGISTPOI;

    NDDSTMAP_REGISTPOIFAVORITEMULTI_REQ sRequest;
    memset(&sRequest, 0x00, sizeof(NDDSTMAP_REGISTPOIFAVORITEMULTI_REQ));

    unsigned char pBuffer[50000] = {0,};
    int nPos = 0;

    sRequest.poiFavorites = (NDDSTMAP_POIREGISTPOIFAVORITE_DTO *) malloc(sizeof(NDDSTMAP_POIREGISTPOIFAVORITE_DTO )*poiFavoriteInput.nPoiCnt);
    memset(sRequest.poiFavorites, 0x00, sizeof(NDDSTMAP_POIREGISTPOIFAVORITE_DTO)*poiFavoriteInput.nPoiCnt);

    sRequest.nPoiFavorites = poiFavoriteInput.nPoiCnt;
    for(int i=0;i<sRequest.nPoiFavorites;i++)
    {
        sRequest.poiFavorites[i].custName = (char*)(pBuffer + nPos);
        strcpy(sRequest.poiFavorites[i].custName,	poiFavoriteInput.sRegistFavoriteList[i].szCustName);
        nPos += strlen(sRequest.poiFavorites[i].custName)+1;

        sRequest.poiFavorites[i].noorX = (char*)(pBuffer + nPos);
        sprintf(sRequest.poiFavorites[i].noorX, "%d", (int)(poiFavoriteInput.sRegistFavoriteList[i].tp.x / (256*2048.0) * 36000.0f + 0.5f));
        nPos += strlen(sRequest.poiFavorites[i].noorX)+1;

        sRequest.poiFavorites[i].noorY = (char*)(pBuffer + nPos);
        sprintf(sRequest.poiFavorites[i].noorY, "%d", (int)(poiFavoriteInput.sRegistFavoriteList[i].tp.y / (256*2048.0) * 36000.0f + 0.5f));
        nPos += strlen(sRequest.poiFavorites[i].noorY)+1;

        sRequest.poiFavorites[i].poiId = (char*)(pBuffer + nPos);
        sprintf(sRequest.poiFavorites[i].poiId, "%d", poiFavoriteInput.sRegistFavoriteList[i].nPoiID);
        nPos += strlen(sRequest.poiFavorites[i].poiId)+1;

        sRequest.poiFavorites[i].navSeq = (char*)(pBuffer + nPos);
        strcpy(sRequest.poiFavorites[i].navSeq, poiFavoriteInput.sRegistFavoriteList[i].szNavSeq);
        nPos += strlen(sRequest.poiFavorites[i].navSeq)+1;

        sRequest.poiFavorites[i].rpFlag = poiFavoriteInput.sRegistFavoriteList[i].byRPFlag;

        sRequest.poiFavorites[i].addInfo = (char*)(pBuffer + nPos);
        strcpy(sRequest.poiFavorites[i].addInfo, poiFavoriteInput.sRegistFavoriteList[i].szAddInfo);
        nPos += strlen(sRequest.poiFavorites[i].addInfo)+1;

        sRequest.poiFavorites[i].orgCustName = (char*)(pBuffer + nPos);
        strcpy(sRequest.poiFavorites[i].orgCustName, poiFavoriteInput.sRegistFavoriteList[i].szOrgCustName);
        nPos += strlen(sRequest.poiFavorites[i].orgCustName)+1;

        sRequest.poiFavorites[i].iconInfo = (char*)(pBuffer + nPos);
        strcpy(sRequest.poiFavorites[i].iconInfo, poiFavoriteInput.sRegistFavoriteList[i].szIconInfo);
        nPos += strlen(sRequest.poiFavorites[i].iconInfo)+1;
    }

    if(!NDDSLIB_RequestProtocol(NDDSTYPE_TMAP_REGISTPOIFAVORITEMULTI, eRequestType, (NDDS_COMMON_REQ*)(&sRequest), nSessionID, &eErrCode))
    {
        if(eErrCode == NDDSERR_USERCANCEL)
            strcpy(szErrorMsg, "사용자 취소");
        else
            strcpy(szErrorMsg, "데이터 생성 실패");

        return 0;
    }

    if(eRequestType == NDDSREQUEST_ATONCE)
    {
        return true;
    }

    eNddsStatus eStatus = NDDSSTATUS_WAIT;
    int nProgress = 0;
    DBGPRINT(_T("Wait "));
    unsigned long reqTime = GetTickCount();
    while(eStatus != NDDSSTATUS_COMPLETE && eStatus != NDDSSTATUS_CANCELED)
    {
        unsigned long thisTime = GetTickCount();
        unsigned long wattingTime = thisTime - reqTime;

        if(wattingTime > SEARCH_WAITTING_TIME)
        {
            strcpy(szErrorMsg, "서버 응답 없음");
            eErrCode = NDDSERR_TIMEOUT;
            return false;
        }

        DBGPRINT(_T("."));
        delay(100);
        NDDSLIB_GetStatusProtocol(nSessionID, &eStatus, &nProgress, &eErrCode);
    }

	NDDS_COMMON_RES *pRes = NULL;
    NDDSTMAP_REGISTPOIFAVORITEMULTI_RES *pResStruct = NULL;
    eNddsProtocolType eResProtocolType = NDDSTYPE_NONE;
    NDDSLIB_GetResultProtocol(nSessionID, true, &eResProtocolType, &pRes, &eErrCode);
    pResStruct = (NDDSTMAP_REGISTPOIFAVORITEMULTI_RES*)pRes;

	if(pResStruct != NULL)
	{
		IsChangeAccesskey(nSessionID, pResStruct->header.accessKey);
		CheckErrorCode(nSessionID, pResStruct->header.errorCode);
	}

    if(!NddsErrorCheck(eErrCode, pResStruct?pResStruct->header.errorMessage:NULL, szErrorMsg))
    {
        NDDSLIB_ReleaseObject(pRes, eErrCode);
        return 0;
    }

	if (pResStruct == NULL)
	{
        eErrCode = NDDSERR_RESULT_NULL;
		return false;
	}

    NDDSLIB_ReleaseObject(pRes);
    DBGPRINT(_T("############################# RegistFavoriteNddsPoi #true \n"));
    return 0;
}

int CNddsModule::FavoriteNddsPoi(int nSessionID, eNddsRequestType eRequestType, bool bMyFavoriteYn, char *szErrorMsg)
{
	eNddsErrorCode eErrCode = NDDSERR_UNKNOWN;

	m_eLastFavoriteType = N_FAVORITETYPE_NDDSPOI;

	memset(m_pNddsFavoriteResult, 0x00, sizeof(m_pNddsFavoriteResult));
	m_nReadFavCnt = 0;
	*szErrorMsg = NULL;

	NDDSTMAP_FINDPOIFAVORITE_REQ sRequest;
	memset(&sRequest, 0x00, sizeof(NDDSTMAP_FINDPOIFAVORITE_REQ));

	char szMyFavoriteYn[2];

	if(bMyFavoriteYn)
		strcpy(szMyFavoriteYn, "Y");
	else
		strcpy(szMyFavoriteYn, "N");

	sRequest.myFavoriteYn = szMyFavoriteYn;

	if(!NDDSLIB_RequestProtocol(NDDSTYPE_TMAP_FINDPOIFAVORITE, eRequestType, (NDDS_COMMON_REQ*)(&sRequest), nSessionID, &eErrCode))
	{
		if(eErrCode == NDDSERR_USERCANCEL)
			strcpy(szErrorMsg, "사용자 취소");
		else
			strcpy(szErrorMsg, "데이터 생성 실패");

		return 0;
	}

	if(eRequestType == NDDSREQUEST_ATONCE)
	{
		m_bMyFavoriteYn = bMyFavoriteYn;
		return true;
	}

    eNddsStatus eStatus = NDDSSTATUS_WAIT;
    int nProgress = 0;
    DBGPRINT(_T("Wait "));
    unsigned long reqTime = GetTickCount();
    while(eStatus != NDDSSTATUS_COMPLETE && eStatus != NDDSSTATUS_CANCELED)
    {
        unsigned long thisTime = GetTickCount();
        unsigned long wattingTime = thisTime - reqTime;

        if(wattingTime > SEARCH_WAITTING_TIME)
        {
            strcpy(szErrorMsg, "서버 응답 없음");
            eErrCode = NDDSERR_TIMEOUT;
            return false;
        }

        DBGPRINT(_T("."));
        delay(100);
        NDDSLIB_GetStatusProtocol(nSessionID, &eStatus, &nProgress, &eErrCode);
    }

	int nNddsErrorCode = -1;

	return GetNddsPoiFavoriteResult(nSessionID, &nNddsErrorCode);
}

int CNddsModule::GetNddsPoiFavoriteResult(int nSessionID, int *nErrorCode)
{
	eNddsErrorCode eErrCode = NDDSERR_UNKNOWN;
	char szErrorMsg[256] = {0,};

	NDDS_COMMON_RES *pRes = NULL;
	NDDSTMAP_FINDPOIFAVORITE_RES *pResStruct = NULL;
	eNddsProtocolType eResProtocolType = NDDSTYPE_NONE;
	NDDSLIB_GetResultProtocol(nSessionID, false, &eResProtocolType, &pRes, &eErrCode);
	pResStruct = (NDDSTMAP_FINDPOIFAVORITE_RES*)pRes;

	if(pResStruct != NULL)
	{
		IsChangeAccesskey(nSessionID, pResStruct->header.accessKey);
		CheckErrorCode(nSessionID, pResStruct->header.errorCode);
	}

	if(!NddsErrorCheck(eErrCode, pResStruct?pResStruct->header.errorMessage:NULL, szErrorMsg))
	{
        NDDSLIB_ReleaseObject(pRes, eErrCode);
		return 0;
	}

	if (pResStruct == NULL)
	{
		*nErrorCode = NDDSERR_RESULT_NULL;
		return false;
	}

	if(m_bMyFavoriteYn)
	{
		NDDSTMAP_POIMYFAVORITE_DTO *pPoiMyFavorite = &pResStruct->poiMyFavorite;

		if(pPoiMyFavorite->homeCustName)
			strcpy(m_pNddsMyFavorite.m_HomeCustName, pPoiMyFavorite->homeCustName);
		if(pPoiMyFavorite->homePoiId)
			m_pNddsMyFavorite.m_HomePoiId = atoi(pPoiMyFavorite->homePoiId);
		if(pPoiMyFavorite->homeNoorX)
			m_pNddsMyFavorite.m_HomeX = atoi(pPoiMyFavorite->homeNoorX);
		if(pPoiMyFavorite->homeNoorY)
			m_pNddsMyFavorite.m_HomeY = atoi(pPoiMyFavorite->homeNoorY);
		if(pPoiMyFavorite->homeNavSeq)
			strcpy(m_pNddsMyFavorite.m_HomeNavSeq, pPoiMyFavorite->homeNavSeq);
		if(pPoiMyFavorite->homeRpFlag)
			m_pNddsMyFavorite.m_HomeRpFlag = pPoiMyFavorite->homeRpFlag;
		if(pPoiMyFavorite->homeAddInfo)
			strcpy(m_pNddsMyFavorite.m_HomeAddInfo, pPoiMyFavorite->homeAddInfo);
		if(pPoiMyFavorite->homeInsDatetime)
			strcpy(m_pNddsMyFavorite.m_HomeInsDatetime, pPoiMyFavorite->homeInsDatetime);

		if(pPoiMyFavorite->officeCustName)
			strcpy(m_pNddsMyFavorite.m_OfficeCustName, pPoiMyFavorite->officeCustName);
		if(pPoiMyFavorite->officePoiId)
			m_pNddsMyFavorite.m_OfficePoiId = atoi(pPoiMyFavorite->officePoiId);
		if(pPoiMyFavorite->officeNoorX)
			m_pNddsMyFavorite.m_OfficeX = atoi(pPoiMyFavorite->officeNoorX);
		if(pPoiMyFavorite->officeNoorY)
			m_pNddsMyFavorite.m_OfficeY = atoi(pPoiMyFavorite->officeNoorY);
		if(pPoiMyFavorite->officeNavSeq)
			strcpy(m_pNddsMyFavorite.m_OfficeNavSeq, pPoiMyFavorite->officeNavSeq);
		if(pPoiMyFavorite->officeRpFlag)
			m_pNddsMyFavorite.m_OfficeRpFlag = pPoiMyFavorite->officeRpFlag;
		if(pPoiMyFavorite->officeAddInfo)
			strcpy(m_pNddsMyFavorite.m_OfficeAddInfo, pPoiMyFavorite->officeAddInfo);
		if(pPoiMyFavorite->officeInsDatetime)
			strcpy(m_pNddsMyFavorite.m_OfficeInsDatetime, pPoiMyFavorite->officeInsDatetime);
	}

	NDDSTMAP_POIFAVORITES_DTO *pPoiFavorite;

	m_nReadFavCnt = pResStruct->nPoiFavorites;
	for(int i=0 ; i<m_nReadFavCnt ; i++)
	{
		pPoiFavorite = &pResStruct->poiFavorites[i];

		if(pPoiFavorite->custName)
			strcpy(m_pNddsFavoriteResult[i].m_CustName, pPoiFavorite->custName);
		if(pPoiFavorite->poiId)
			m_pNddsFavoriteResult[i].m_PoiId = atoi(pPoiFavorite->poiId);
		if(pPoiFavorite->noorX)
			m_pNddsFavoriteResult[i].m_X = atoi(pPoiFavorite->noorX);
		if(pPoiFavorite->noorY)
			m_pNddsFavoriteResult[i].m_Y = atoi(pPoiFavorite->noorY);
		if(pPoiFavorite->navSeq)
			strcpy(m_pNddsFavoriteResult[i].m_NavSeq, pPoiFavorite->navSeq);
		if(pPoiFavorite->rpFlag)
			m_pNddsFavoriteResult[i].m_RPFlag = pPoiFavorite->rpFlag;
		if(pPoiFavorite->addInfo)
			strcpy(m_pNddsFavoriteResult[i].m_AddInfo, pPoiFavorite->addInfo);
		if(pPoiFavorite->insDatetime)
			strcpy(m_pNddsFavoriteResult[i].m_InsDatetime, pPoiFavorite->insDatetime);		
	}

    NDDSLIB_ReleaseObject(pRes);

	return m_nReadFavCnt;
}

bool CNddsModule::API_NDDS_GetFavoriteResultList(PAPI_FAVORITE_RESULTLIST_REQ pRequest, PAPI_FAVORITE_RESULTLIST_RES pResult)
{
	int i, nIdx;
	static stVERTEX32 wpOld = {0,0};

	pResult->nResult = 0;
	pResult->nTotal = m_nReadFavCnt;

	if(pRequest->eFavoriteType != m_eLastFavoriteType)
	{
		//assert(0);
		return false;
	}

	if(pRequest->nIdx >= m_nReadFavCnt)
		return false;
	if(pRequest->nIdx + pRequest->nCnt > m_nReadFavCnt)
		pRequest->nCnt = m_nReadFavCnt - pRequest->nIdx;

	memset(pResult->sList, 0, sizeof(N_FAVORITE_LIST)*MAX_RESULTLIST);

	switch(pRequest->eFavoriteType)
	{
	case N_FAVORITETYPE_NDDSPOI:
		{
			for(i=0 ; i<pRequest->nCnt ; i++)
			{
				nIdx = pRequest->nIdx + i;
				pResult->sList[i].nIdx = nIdx;

				strcpy(pResult->sList[i].szCustName, m_pNddsFavoriteResult[nIdx].m_CustName);
				strcpy(pResult->sList[i].szNavSeq,  m_pNddsFavoriteResult[nIdx].m_NavSeq);
				strcpy(pResult->sList[i].szAddInfo, m_pNddsFavoriteResult[nIdx].m_AddInfo);
				pResult->sList[i].nPoiID = m_pNddsFavoriteResult[nIdx].m_PoiId;
				pResult->sList[i].RPFlag = m_pNddsFavoriteResult[nIdx].m_RPFlag;
				pResult->sList[i].tp.x = (int)(m_pNddsFavoriteResult[nIdx].m_X /36000.0 * WORLDCOORD_PER_DEG + 0.5);
				pResult->sList[i].tp.y = (int)(m_pNddsFavoriteResult[nIdx].m_Y /36000.0 * WORLDCOORD_PER_DEG + 0.5);
				strcpy(pResult->sList[i].szInsDatetime, m_pNddsFavoriteResult[nIdx].m_InsDatetime);

				pResult->nResult++;
			}
		}
		break;
	default:
		break;
	}

	return true;
}

bool CNddsModule::API_NDDS_GetMyFavorite(PAPI_MYFAVORITE_REQ pRequest, PAPI_MYFAVORITE_RES pResult)
{
	memset(pResult, 0, sizeof(pResult));

	strcpy(pResult->szHomeCustName, m_pNddsMyFavorite.m_HomeCustName);
	strcpy(pResult->szHomeNavSeq,  m_pNddsMyFavorite.m_HomeNavSeq);
	strcpy(pResult->szHomeAddInfo, m_pNddsMyFavorite.m_HomeAddInfo);
	pResult->nHomePoiId = m_pNddsMyFavorite.m_HomePoiId;
	pResult->byHomeRpFlag = m_pNddsMyFavorite.m_HomeRpFlag;
	pResult->Hometp.x = (int)(m_pNddsMyFavorite.m_HomeX /36000.0 * WORLDCOORD_PER_DEG + 0.5);
	pResult->Hometp.y = (int)(m_pNddsMyFavorite.m_HomeY /36000.0 * WORLDCOORD_PER_DEG + 0.5);
	strcpy(pResult->szHomeInsDatetime, m_pNddsMyFavorite.m_HomeInsDatetime);
	strcpy(pResult->szOfficeCustName, m_pNddsMyFavorite.m_OfficeCustName);
	strcpy(pResult->szOfficeNavSeq,  m_pNddsMyFavorite.m_OfficeNavSeq);
	strcpy(pResult->szOfficeAddInfo, m_pNddsMyFavorite.m_OfficeAddInfo);
	pResult->nOfficePoiId = m_pNddsMyFavorite.m_OfficePoiId;
	pResult->byOfficeRpFlag = m_pNddsMyFavorite.m_OfficeRpFlag;
	pResult->Officetp.x = (int)(m_pNddsMyFavorite.m_OfficeX /36000.0 * WORLDCOORD_PER_DEG + 0.5);
	pResult->Officetp.y = (int)(m_pNddsMyFavorite.m_OfficeY /36000.0 * WORLDCOORD_PER_DEG + 0.5);
	strcpy(pResult->szOfficeInsDatetime, m_pNddsMyFavorite.m_OfficeInsDatetime);

	return true;
}

bool CNddsModule::API_NDDS_GetSendToCarInfo(PAPI_SENDTOCARINFO_REQ pRequest, PAPI_SENDTOCARINFO_RES pResult)
{
	memset(pResult, 0, sizeof(pResult));

	strcpy(pResult->szAddrname, m_pNddsFavoriteSendToCarInfo.m_addrname);
	strcpy(pResult->szCustName,  m_pNddsFavoriteSendToCarInfo.m_CustName);
	pResult->nPoiId = m_pNddsFavoriteSendToCarInfo.m_PoiId;
	strcpy(pResult->szInsDatetime, m_pNddsFavoriteSendToCarInfo.m_InsDatetime);
	strcpy(pResult->szNavSeq, m_pNddsFavoriteSendToCarInfo.m_NavSeq);
	pResult->byRPFlag = m_pNddsFavoriteSendToCarInfo.m_RPFlag;
	strcpy(pResult->szTell, m_pNddsFavoriteSendToCarInfo.m_strTell);
	pResult->tp.x = (int)(m_pNddsFavoriteSendToCarInfo.m_X /36000.0 * WORLDCOORD_PER_DEG + 0.5);
	pResult->tp.y = (int)(m_pNddsFavoriteSendToCarInfo.m_Y /36000.0 * WORLDCOORD_PER_DEG + 0.5);

	return true;
}

int CNddsModule::FavoriteNddsRegisterSendToCarInfo(int nSessionID, eNddsRequestType eRequestType, char *szCustName, stVERTEX32 *pTp, int nPoiId, char rpFlag, char *szNavSeq, char *szErrorMsg)
{
	eNddsErrorCode eErrCode = NDDSERR_UNKNOWN;

	m_eLastFavoriteType = N_FAVORITETYPE_NDDSREGISTERSENDTOCARINFO;

	*szErrorMsg = NULL;

	NDDSTMAP_REGISTERSENDTOCARINFO_REQ sRequest;
	memset(&sRequest, 0x00, sizeof(NDDSTMAP_REGISTERSENDTOCARINFO_REQ));

	char sCustName[101]={0,};
	char sNavSeq[7]={0,};
	char szX[10]={0,}, szY[10]={0,}, szPoiId[15]={0,};
	unsigned int x, y;

	strcpy(sCustName, szCustName);
	strcpy(sNavSeq, szNavSeq);
	x = (unsigned int)(pTp->x / (256*2048.0) * 36000.0f + 0.5f);
	y = (unsigned int)(pTp->y / (256*2048.0) * 36000.0f + 0.5f);
	sprintf(szX, "%u", x);
	sprintf(szY, "%u", y);
	sprintf(szPoiId, "%d", nPoiId);

	sRequest.custName = sCustName;
	sRequest.noorX = szX;
	sRequest.noorY = szY;
	sRequest.poiId = szPoiId;
	sRequest.rpFlag = rpFlag;
	sRequest.navSeq = sNavSeq;

	if(!NDDSLIB_RequestProtocol(NDDSTYPE_TMAP_REGISTERSENDTOCARINFO, eRequestType, (NDDS_COMMON_REQ*)(&sRequest), nSessionID, &eErrCode))
	{
		if(eErrCode == NDDSERR_USERCANCEL)
			strcpy(szErrorMsg, "사용자 취소");
		else
			strcpy(szErrorMsg, "데이터 생성 실패");

		return 0;
	}

	if(eRequestType == NDDSREQUEST_ATONCE)
	{
		return true;
	}

    eNddsStatus eStatus = NDDSSTATUS_WAIT;
    int nProgress = 0;
    DBGPRINT(_T("Wait "));
    unsigned long reqTime = GetTickCount();
    while(eStatus != NDDSSTATUS_COMPLETE && eStatus != NDDSSTATUS_CANCELED)
    {
        unsigned long thisTime = GetTickCount();
        unsigned long wattingTime = thisTime - reqTime;

        if(wattingTime > SEARCH_WAITTING_TIME)
        {
            strcpy(szErrorMsg, "서버 응답 없음");
            eErrCode = NDDSERR_TIMEOUT;
            return false;
        }

        DBGPRINT(_T("."));
        delay(100);
        NDDSLIB_GetStatusProtocol(nSessionID, &eStatus, &nProgress, &eErrCode);
    }

	int nNddsErrorCode = -1;

	return GetNddsRegisterSendToCarInfoResult(nSessionID, &nNddsErrorCode);
}

int CNddsModule::GetNddsRegisterSendToCarInfoResult(int nSessionID, int *nErrorCode)
{
	eNddsErrorCode eErrCode = NDDSERR_UNKNOWN;
	char szErrorMsg[256] = {0,};

	NDDS_COMMON_RES *pRes = NULL;
	NDDSTMAP_REGISTERSENDTOCARINFO_RES *pResStruct = NULL;
	eNddsProtocolType eResProtocolType = NDDSTYPE_NONE;
	NDDSLIB_GetResultProtocol(nSessionID, false, &eResProtocolType, &pRes, &eErrCode);
	pResStruct = (NDDSTMAP_REGISTERSENDTOCARINFO_RES*)pRes;

	if(pResStruct != NULL)
	{
		IsChangeAccesskey(nSessionID, pResStruct->header.accessKey);
		CheckErrorCode(nSessionID, pResStruct->header.errorCode);
	}

	if(!NddsErrorCheck(eErrCode, pResStruct?pResStruct->header.errorMessage:NULL, szErrorMsg))
	{
        NDDSLIB_ReleaseObject(pRes, eErrCode);
		return 0;
	}

	if (pResStruct == NULL)
	{
		*nErrorCode = NDDSERR_RESULT_NULL;
		return false;
	}

    NDDSLIB_ReleaseObject(pRes);

	return true;
}

int CNddsModule::FavoriteNddsFindSendToCarInfo(int nSessionID, eNddsRequestType eRequestType, bool bNewAddrChk, char *szErrorMsg)
{
	eNddsErrorCode eErrCode = NDDSERR_UNKNOWN;

	m_eLastFavoriteType = N_FAVORITETYPE_NDDSFINDSENDTOCARINFO;

	memset(m_pNddsFavoriteResult, 0x00, sizeof(m_pNddsFavoriteResult));
	m_nReadFavCnt = 0;
	*szErrorMsg = NULL;

	NDDSTMAP_FINDSENDTOCARINFO_REQ sRequest;
	memset(&sRequest, 0x00, sizeof(NDDSTMAP_FINDSENDTOCARINFO_REQ));

	if(!NDDSLIB_RequestProtocol(NDDSTYPE_TMAP_FINDSENDTOCARINFO, eRequestType, (NDDS_COMMON_REQ*)(&sRequest), nSessionID, &eErrCode))
	{
		if(eErrCode == NDDSERR_USERCANCEL)
			strcpy(szErrorMsg, "사용자 취소");
		else
			strcpy(szErrorMsg, "데이터 생성 실패");

		return 0;
	}

	if(eRequestType == NDDSREQUEST_ATONCE)
	{
		m_bNewAddrChkForS2C = bNewAddrChk;
		return true;
	}

    eNddsStatus eStatus = NDDSSTATUS_WAIT;
    int nProgress = 0;
    DBGPRINT(_T("Wait "));
    unsigned long reqTime = GetTickCount();
    while(eStatus != NDDSSTATUS_COMPLETE && eStatus != NDDSSTATUS_CANCELED)
    {
        unsigned long thisTime = GetTickCount();
        unsigned long wattingTime = thisTime - reqTime;

        if(wattingTime > SEARCH_WAITTING_TIME)
        {
            strcpy(szErrorMsg, "서버 응답 없음");
            eErrCode = NDDSERR_TIMEOUT;
            return false;
        }

        DBGPRINT(_T("."));
        delay(100);
        NDDSLIB_GetStatusProtocol(nSessionID, &eStatus, &nProgress, &eErrCode);
    }

	int nNddsErrorCode = -1;

	return GetNddsFindSendToCarInfoResult(nSessionID, bNewAddrChk, &nNddsErrorCode);
}

int CNddsModule::GetNddsFindSendToCarInfoResult(int nSessionID, bool bNewAddrChk, int *nErrorCode)
{
	eNddsErrorCode eErrCode = NDDSERR_UNKNOWN;
	char szErrorMsg[256] = {0,};

	NDDS_COMMON_RES *pRes = NULL;
	NDDSTMAP_FINDSENDTOCARINFO_RES *pResStruct = NULL;
	eNddsProtocolType eResProtocolType = NDDSTYPE_NONE;
	NDDSLIB_GetResultProtocol(nSessionID, false, &eResProtocolType, &pRes, &eErrCode);
	pResStruct = (NDDSTMAP_FINDSENDTOCARINFO_RES*)pRes;

	if(pResStruct != NULL)
	{
		IsChangeAccesskey(nSessionID, pResStruct->header.accessKey);
		CheckErrorCode(nSessionID, pResStruct->header.errorCode);
	}

	if(!NddsErrorCheck(eErrCode, pResStruct?pResStruct->header.errorMessage:NULL, szErrorMsg))
	{
        NDDSLIB_ReleaseObject(pRes, eErrCode);
		return 0;
	}

	if (pResStruct == NULL)
	{
		*nErrorCode = NDDSERR_RESULT_NULL;
		return false;
	}

	if(pResStruct != NULL)
	{
		if(pResStruct->custName)
			strcpy(m_pNddsFavoriteSendToCarInfo.m_CustName, pResStruct->custName);

		if(bNewAddrChk && pResStruct->roadName)
		{
			if(pResStruct->bldNo1)
			{
				if(pResStruct->bldNo2)
				{
					if(pResStruct->mcdName)
						sprintf(m_pNddsFavoriteSendToCarInfo.m_addrname, "%s %s %s %s-%s", pResStruct->lcdName, pResStruct->mcdName, pResStruct->roadName, pResStruct->bldNo1, pResStruct->bldNo2);
					else
						sprintf(m_pNddsFavoriteSendToCarInfo.m_addrname, "%s %s %s-%s", pResStruct->lcdName, pResStruct->roadName, pResStruct->bldNo1, pResStruct->bldNo2);
				}
				else
				{
					if(pResStruct->mcdName)
						sprintf(m_pNddsFavoriteSendToCarInfo.m_addrname, "%s %s %s %s", pResStruct->lcdName, pResStruct->mcdName, pResStruct->roadName, pResStruct->bldNo1);
					else
						sprintf(m_pNddsFavoriteSendToCarInfo.m_addrname, "%s %s %s", pResStruct->lcdName, pResStruct->roadName, pResStruct->bldNo1);
				}
			}
			else
			{
				if(pResStruct->mcdName)
					sprintf(m_pNddsFavoriteSendToCarInfo.m_addrname, "%s %s %s", pResStruct->lcdName, pResStruct->mcdName, pResStruct->roadName);
				else
					sprintf(m_pNddsFavoriteSendToCarInfo.m_addrname, "%s %s", pResStruct->lcdName, pResStruct->roadName);
			}
		}else
		{
			if(pResStruct->primaryBun)
			{
				if(pResStruct->secondaryBun)
				{
					if(pResStruct->secondaryBun[0] != '0') //NDDS데이터에 0이 들어있어 예외처리
					{
						if(pResStruct->dcdName)
						{
							if(pResStruct->mcdName)
								sprintf(m_pNddsFavoriteSendToCarInfo.m_addrname, "%s %s %s %s %s-%s", pResStruct->lcdName, pResStruct->mcdName, pResStruct->scdName, pResStruct->dcdName, pResStruct->primaryBun, pResStruct->secondaryBun);
							else
								sprintf(m_pNddsFavoriteSendToCarInfo.m_addrname, "%s %s %s %s-%s", pResStruct->lcdName, pResStruct->scdName, pResStruct->dcdName, pResStruct->primaryBun, pResStruct->secondaryBun);
						}
						else
						{
							if(pResStruct->mcdName)
								sprintf(m_pNddsFavoriteSendToCarInfo.m_addrname, "%s %s %s %s-%s", pResStruct->lcdName, pResStruct->mcdName, pResStruct->scdName, pResStruct->primaryBun, pResStruct->secondaryBun);
							else
								sprintf(m_pNddsFavoriteSendToCarInfo.m_addrname, "%s %s %s-%s", pResStruct->lcdName, pResStruct->scdName, pResStruct->primaryBun, pResStruct->secondaryBun);
						}
					}
					else
					{
						if(pResStruct->dcdName)
						{
							if(pResStruct->mcdName)
								sprintf(m_pNddsFavoriteSendToCarInfo.m_addrname, "%s %s %s %s %s", pResStruct->lcdName, pResStruct->mcdName, pResStruct->scdName, pResStruct->dcdName, pResStruct->primaryBun);
							else
								sprintf(m_pNddsFavoriteSendToCarInfo.m_addrname, "%s %s %s %s", pResStruct->lcdName, pResStruct->scdName, pResStruct->dcdName, pResStruct->primaryBun);
						}
						else
						{
							if(pResStruct->mcdName)
								sprintf(m_pNddsFavoriteSendToCarInfo.m_addrname, "%s %s %s %s", pResStruct->lcdName, pResStruct->mcdName, pResStruct->scdName, pResStruct->primaryBun);
							else
								sprintf(m_pNddsFavoriteSendToCarInfo.m_addrname, "%s %s %s", pResStruct->lcdName, pResStruct->scdName, pResStruct->primaryBun);
						}
					}
				}
				else
				{
					if(pResStruct->dcdName)
					{
						if(pResStruct->mcdName)
							sprintf(m_pNddsFavoriteSendToCarInfo.m_addrname, "%s %s %s %s %s", pResStruct->lcdName, pResStruct->mcdName, pResStruct->scdName, pResStruct->dcdName, pResStruct->primaryBun);
						else
							sprintf(m_pNddsFavoriteSendToCarInfo.m_addrname, "%s %s %s %s", pResStruct->lcdName, pResStruct->scdName, pResStruct->dcdName, pResStruct->primaryBun);
					}
					else
					{
						if(pResStruct->mcdName)
							sprintf(m_pNddsFavoriteSendToCarInfo.m_addrname, "%s %s %s %s", pResStruct->lcdName, pResStruct->mcdName, pResStruct->scdName, pResStruct->primaryBun);
						else
							sprintf(m_pNddsFavoriteSendToCarInfo.m_addrname, "%s %s %s", pResStruct->lcdName, pResStruct->scdName, pResStruct->primaryBun);
					}
				}

				if(pResStruct->mlClass && pResStruct->mlClass[0] == '2')
					strcat(m_pNddsFavoriteSendToCarInfo.m_addrname, " 산");
			}
		}


		m_pNddsFavoriteSendToCarInfo.m_X = atoi(pResStruct->noorX);
		m_pNddsFavoriteSendToCarInfo.m_Y = atoi(pResStruct->noorY);
		if(pResStruct->poiId)
			m_pNddsFavoriteSendToCarInfo.m_PoiId = atoi(pResStruct->poiId);

		if(pResStruct->telNo)
			strcpy(m_pNddsFavoriteSendToCarInfo.m_strTell, pResStruct->telNo);
		m_pNddsFavoriteSendToCarInfo.m_RPFlag = pResStruct->rpFlag;

		if(pResStruct->navSeq)
			strcpy(m_pNddsFavoriteSendToCarInfo.m_NavSeq, pResStruct->navSeq);
	}

    NDDSLIB_ReleaseObject(pRes);

	return true;
}

int CNddsModule::FavoriteNddsRemoveSendToCarInfo(int nSessionID, eNddsRequestType eRequestType, char *szCustName, stVERTEX32 *pTp, char *szErrorMsg)
{
	eNddsErrorCode eErrCode = NDDSERR_UNKNOWN;

	m_eLastFavoriteType = N_FAVORITETYPE_NDDSREMOVESENDTOCARINFO;

	memset(m_pNddsFavoriteResult, 0x00, sizeof(m_pNddsFavoriteResult));
	m_nReadFavCnt = 0;
	*szErrorMsg = NULL;

	NDDSTMAP_REMOVESENDTOCARINFO_REQ sRequest;
	memset(&sRequest, 0x00, sizeof(NDDSTMAP_REMOVESENDTOCARINFO_REQ));

	char sCustName[101];
	char szX[10], szY[10];
	unsigned int x, y;

	strcpy(sCustName, szCustName);;
	x = (unsigned int)(pTp->x / (256*2048.0) * 36000.0f + 0.5f);
	y = (unsigned int)(pTp->y / (256*2048.0) * 36000.0f + 0.5f);
	sprintf(szX, "%u", x);
	sprintf(szY, "%u", y);

	sRequest.custName = sCustName;
	sRequest.noorX = szX;
	sRequest.noorY = szY;

	if(!NDDSLIB_RequestProtocol(NDDSTYPE_TMAP_REMOVESENDTOCARINFO, eRequestType, (NDDS_COMMON_REQ*)(&sRequest), nSessionID, &eErrCode))
	{
		if(eErrCode == NDDSERR_USERCANCEL)
			strcpy(szErrorMsg, "사용자 취소");
		else
			strcpy(szErrorMsg, "데이터 생성 실패");

		return 0;
	}

	if(eRequestType == NDDSREQUEST_ATONCE)
	{
		return true;
	}

    eNddsStatus eStatus = NDDSSTATUS_WAIT;
    int nProgress = 0;
    DBGPRINT(_T("Wait "));
    unsigned long reqTime = GetTickCount();
    while(eStatus != NDDSSTATUS_COMPLETE && eStatus != NDDSSTATUS_CANCELED)
    {
        unsigned long thisTime = GetTickCount();
        unsigned long wattingTime = thisTime - reqTime;

        if(wattingTime > SEARCH_WAITTING_TIME)
        {
            strcpy(szErrorMsg, "서버 응답 없음");
            eErrCode = NDDSERR_TIMEOUT;
            return false;
        }

        DBGPRINT(_T("."));
        delay(100);
        NDDSLIB_GetStatusProtocol(nSessionID, &eStatus, &nProgress, &eErrCode);
    }

	int nNddsErrorCode = -1;

	return GetNddsRemoveSendToCarInfoResult(nSessionID, &nNddsErrorCode);
}

int CNddsModule::GetNddsRemoveSendToCarInfoResult(int nSessionID, int *nErrorCode)
{
	eNddsErrorCode eErrCode = NDDSERR_UNKNOWN;
	char szErrorMsg[256] = {0,};

	NDDS_COMMON_RES *pRes = NULL;
	NDDSTMAP_FINDSENDTOCARINFO_RES *pResStruct = NULL;
	eNddsProtocolType eResProtocolType = NDDSTYPE_NONE;
	NDDSLIB_GetResultProtocol(nSessionID, false, &eResProtocolType, &pRes, &eErrCode);
	pResStruct = (NDDSTMAP_FINDSENDTOCARINFO_RES*)pRes;

	if(pResStruct != NULL)
	{
		IsChangeAccesskey(nSessionID, pResStruct->header.accessKey);
		CheckErrorCode(nSessionID, pResStruct->header.errorCode);
	}

	if(!NddsErrorCheck(eErrCode, pResStruct?pResStruct->header.errorMessage:NULL, szErrorMsg))
	{
        NDDSLIB_ReleaseObject(pRes, eErrCode);
		return 0;
	}

	if (pResStruct == NULL)
	{
		*nErrorCode = NDDSERR_RESULT_NULL;
		return false;
	}

    NDDSLIB_ReleaseObject(pRes);

	memset(&m_pNddsFavoriteSendToCarInfo, 0x00, sizeof(m_pNddsFavoriteSendToCarInfo));

	return true;
}


int CNddsModule::FavoriteNddsRegistUserProfile(int nSessionID, eNddsRequestType eRequestType, FAVORITE_NDDSREGISTUSERPROFILE_INPUT inputData, char *szErrorMsg)
{
    eNddsErrorCode eErrCode = NDDSERR_UNKNOWN;

    m_eLastFavoriteType = N_FAVORITETYPE_NDDSREGISTUSERPROFILE;

    memset(m_pNddsFavoriteResult, 0x00, sizeof(m_pNddsFavoriteResult));

    *szErrorMsg = NULL;

    NDDSTMAP_REGISTUSERPROFILE_REQ sRequest;
    memset(&sRequest, 0x00, sizeof(NDDSTMAP_REGISTUSERPROFILE_REQ));
    char sEmail[64]={0,};
    char sUserName[32]={0,};
    char sMdn[16]={0,};
    char sBirth[16]={0,};

    strcpy(sEmail, inputData.szEmail);
    strcpy(sUserName, inputData.szUserName);
    strcpy(sMdn, inputData.szMdn);
    strcpy(sBirth, inputData.szBirth);

    sRequest.userlInfo.email = sEmail;
    sRequest.userlInfo.userName = sUserName;
    sRequest.userlInfo.mdn = sMdn;
    sRequest.userlInfo.birth = sBirth;

    if(!NDDSLIB_RequestProtocol(NDDSTYPE_TMAP_REGISTUSERPROFILE, eRequestType, (NDDS_COMMON_REQ*)(&sRequest), nSessionID, &eErrCode))
    {
        if(eErrCode == NDDSERR_USERCANCEL)
            strcpy(szErrorMsg, "사용자 취소");
        else
            strcpy(szErrorMsg, "데이터 생성 실패");

        return 0;
    }

    if(eRequestType == NDDSREQUEST_ATONCE)
    {
        return true;
    }

    eNddsStatus eStatus = NDDSSTATUS_WAIT;
    int nProgress = 0;
    DBGPRINT(_T("Wait "));
    unsigned long reqTime = GetTickCount();
    while(eStatus != NDDSSTATUS_COMPLETE && eStatus != NDDSSTATUS_CANCELED)
    {
        unsigned long thisTime = GetTickCount();
        unsigned long wattingTime = thisTime - reqTime;

        if(wattingTime > SEARCH_WAITTING_TIME)
        {
            strcpy(szErrorMsg, "서버 응답 없음");
            eErrCode = NDDSERR_TIMEOUT;
            return false;
        }

        DBGPRINT(_T("."));
        delay(100);
        NDDSLIB_GetStatusProtocol(nSessionID, &eStatus, &nProgress, &eErrCode);
    }

    int nNddsErrorCode = -1;

    return 0;
}

int CNddsModule::FavoriteNddsFindUserProfile(int nSessionID, eNddsRequestType eRequestType, char *szErrorMsg)
{
    eNddsErrorCode eErrCode = NDDSERR_UNKNOWN;

    m_eLastFavoriteType = N_FAVORITETYPE_NDDSFINDUSERPROFILE;

    memset(m_pNddsFavoriteResult, 0x00, sizeof(m_pNddsFavoriteResult));
    m_nReadFavCnt = 0;
    *szErrorMsg = NULL;

    NDDSTMAP_FINDUSERPROFILE_REQ sRequest;
    memset(&sRequest, 0x00, sizeof(NDDSTMAP_FINDUSERPROFILE_REQ));

    if(!NDDSLIB_RequestProtocol(NDDSTYPE_TMAP_FINDUSERPROFILE, eRequestType, (NDDS_COMMON_REQ*)(&sRequest), nSessionID, &eErrCode))
    {
        if(eErrCode == NDDSERR_USERCANCEL)
            strcpy(szErrorMsg, "사용자 취소");
        else
            strcpy(szErrorMsg, "데이터 생성 실패");

        return 0;
    }

    if(eRequestType == NDDSREQUEST_ATONCE)
    {
        return true;
    }

    eNddsStatus eStatus = NDDSSTATUS_WAIT;
    int nProgress = 0;
    DBGPRINT(_T("Wait "));
    unsigned long reqTime = GetTickCount();
    while(eStatus != NDDSSTATUS_COMPLETE && eStatus != NDDSSTATUS_CANCELED)
    {
        unsigned long thisTime = GetTickCount();
        unsigned long wattingTime = thisTime - reqTime;

        if(wattingTime > SEARCH_WAITTING_TIME)
        {
            strcpy(szErrorMsg, "서버 응답 없음");
            eErrCode = NDDSERR_TIMEOUT;
            return false;
        }

        DBGPRINT(_T("."));
        delay(100);
        NDDSLIB_GetStatusProtocol(nSessionID, &eStatus, &nProgress, &eErrCode);
    }

    int nNddsErrorCode = -1;

    return 0;
}

int CNddsModule::FavoriteNddsFindCarProfile(int nSessionID, eNddsRequestType eRequestType, char *szErrorMsg)
{
    eNddsErrorCode eErrCode = NDDSERR_UNKNOWN;

    m_eLastFavoriteType = N_FAVORITETYPE_NDDSFINDCARPROFILE;

    memset(m_pNddsFavoriteResult, 0x00, sizeof(m_pNddsFavoriteResult));
    m_nReadFavCnt = 0;
    *szErrorMsg = NULL;

    NDDSTMAP_FINDCARPROFILE_REQ sRequest;
    memset(&sRequest, 0x00, sizeof(NDDSTMAP_FINDCARPROFILE_REQ));

    if(!NDDSLIB_RequestProtocol(NDDSTYPE_TMAP_FINDCARPROFILE, eRequestType, (NDDS_COMMON_REQ*)(&sRequest), nSessionID, &eErrCode))
    {
        if(eErrCode == NDDSERR_USERCANCEL)
            strcpy(szErrorMsg, "사용자 취소");
        else
            strcpy(szErrorMsg, "데이터 생성 실패");

        return 0;
    }

    if(eRequestType == NDDSREQUEST_ATONCE)
    {
        return true;
    }

    eNddsStatus eStatus = NDDSSTATUS_WAIT;
    int nProgress = 0;
    DBGPRINT(_T("Wait "));
    unsigned long reqTime = GetTickCount();
    while(eStatus != NDDSSTATUS_COMPLETE && eStatus != NDDSSTATUS_CANCELED)
    {
        unsigned long thisTime = GetTickCount();
        unsigned long wattingTime = thisTime - reqTime;

        if(wattingTime > SEARCH_WAITTING_TIME)
        {
            strcpy(szErrorMsg, "서버 응답 없음");
            eErrCode = NDDSERR_TIMEOUT;
            return false;
        }

        DBGPRINT(_T("."));
        delay(100);
        NDDSLIB_GetStatusProtocol(nSessionID, &eStatus, &nProgress, &eErrCode);
    }

    int nNddsErrorCode = -1;

    return 0;
}

int CNddsModule::FavoriteNddsFindUserData(int nSessionID, eNddsRequestType eRequestType, FAVORITE_NDDSFINDUSERDATA_INPUT inputData, char *szErrorMsg)
{
    eNddsErrorCode eErrCode = NDDSERR_UNKNOWN;

    m_eLastFavoriteType = N_FAVORITETYPE_NDDSFINDUSERDATA;

    memset(m_pNddsFavoriteResult, 0x00, sizeof(m_pNddsFavoriteResult));
    *szErrorMsg = NULL;

    NDDSTMAP_FINDUSERDATA_REQ sRequest;
    memset(&sRequest, 0x00, sizeof(NDDSTMAP_FINDUSERDATA_REQ));

    if(!NDDSLIB_RequestProtocol(NDDSTYPE_TMAP_FINDUSERDATA, eRequestType, (NDDS_COMMON_REQ*)(&sRequest), nSessionID, &eErrCode))
    {
        if(eErrCode == NDDSERR_USERCANCEL)
            strcpy(szErrorMsg, "사용자 취소");
        else
            strcpy(szErrorMsg, "데이터 생성 실패");

        return 0;
    }

    if(eRequestType == NDDSREQUEST_ATONCE)
    {
        return true;
    }

    eNddsStatus eStatus = NDDSSTATUS_WAIT;
    int nProgress = 0;
    DBGPRINT(_T("Wait "));
    unsigned long reqTime = GetTickCount();
    while(eStatus != NDDSSTATUS_COMPLETE && eStatus != NDDSSTATUS_CANCELED)
    {
        unsigned long thisTime = GetTickCount();
        unsigned long wattingTime = thisTime - reqTime;

        if(wattingTime > SEARCH_WAITTING_TIME)
        {
            strcpy(szErrorMsg, "서버 응답 없음");
            eErrCode = NDDSERR_TIMEOUT;
            return false;
        }

        DBGPRINT(_T("."));
        delay(100);
        NDDSLIB_GetStatusProtocol(nSessionID, &eStatus, &nProgress, &eErrCode);
    }

    int nNddsErrorCode = -1;

    return GetNddsUserDataResult(nSessionID, &nNddsErrorCode);
}

int CNddsModule::FavoriteNddsRegistCarProfile(int nSessionID, eNddsRequestType eRequestType, FAVORITE_NDDSREGISTCARPROFILE_INPUT inputData, char *szErrorMsg)
{
    eNddsErrorCode eErrCode = NDDSERR_UNKNOWN;

    m_eLastFavoriteType = N_FAVORITETYPE_NDDSREGISTCARPROFILE;

    memset(m_pNddsFavoriteResult, 0x00, sizeof(m_pNddsFavoriteResult));
    m_nReadFavCnt = 0;
    *szErrorMsg = NULL;

    NDDSTMAP_REGISTCARPROFILE_REQ sRequest;
    memset(&sRequest, 0x00, sizeof(NDDSTMAP_REGISTCARPROFILE_REQ));

    char szModel[20]={0,};
    char szFuel[10]={0,};
    char szHipassYn[2]={0,};
    char szNumber[12]={0,};

    strcpy(szModel, inputData.szModel);
    strcpy(szFuel, inputData.szFuel);
    strcpy(szHipassYn, inputData.szHipassYn);
    strcpy(szNumber, inputData.szNumber);

    sRequest.carlInfo.model = szModel;
    sRequest.carlInfo.fuel = szFuel;
    sRequest.carlInfo.hipassYn = szHipassYn;
    sRequest.carlInfo.number = szNumber;

    if(!NDDSLIB_RequestProtocol(NDDSTYPE_TMAP_REGISTCARPROFILE, eRequestType, (NDDS_COMMON_REQ*)(&sRequest), nSessionID, &eErrCode))
    {
        if(eErrCode == NDDSERR_USERCANCEL)
            strcpy(szErrorMsg, "사용자 취소");
        else
            strcpy(szErrorMsg, "데이터 생성 실패");

        return 0;
    }

    if(eRequestType == NDDSREQUEST_ATONCE)
    {
        return true;
    }

    eNddsStatus eStatus = NDDSSTATUS_WAIT;
    int nProgress = 0;
    DBGPRINT(_T("Wait "));
    unsigned long reqTime = GetTickCount();
    while(eStatus != NDDSSTATUS_COMPLETE && eStatus != NDDSSTATUS_CANCELED)
    {
        unsigned long thisTime = GetTickCount();
        unsigned long wattingTime = thisTime - reqTime;

        if(wattingTime > SEARCH_WAITTING_TIME)
        {
            strcpy(szErrorMsg, "서버 응답 없음");
            eErrCode = NDDSERR_TIMEOUT;
            return false;
        }

        DBGPRINT(_T("."));
        delay(100);
        NDDSLIB_GetStatusProtocol(nSessionID, &eStatus, &nProgress, &eErrCode);
    }

    int nNddsErrorCode = -1;

    return 0;
}

bool CNddsModule::API_NDDS_VmsGuideInfo(PAPI_VMSGUIDEINFO_REQ pRequest, PAPI_VMSGUIDEINFO_RES pResult)
{
#ifdef _LINUX
	if(sem_wait(&m_pModuleProcSem) != 0)
#else
	if(SDL_SemWait(m_pModuleProcSem) == SDL_MUTEX_TIMEDOUT)
#endif
	{
		return false;
		delay(1); //스레드 스위칭(기존 스레드 처리가 끝날 수 있게 스위칭)
	}

#ifdef _LINUX
	pthread_mutex_lock(&m_pModuleProcMutex);
#else
	SDL_LockMutex(m_pModuleProcMutex);
#endif

	int nSessionID = ++m_nLastIdxSession;

	SendMessgeCallback(NM_NDDS_TRAFFICINFO_INIT, nSessionID);

	m_sModuleInputProcData.eProcType = E_NDDS_PROCTYPE_TRAFFICINFO;
	m_sModuleInputProcData.nTrafficInfoSessionID = nSessionID;
	m_sModuleInputProcData.bTrafficInfoSync = pRequest->bSync;
	memcpy(&m_sModuleInputProcData.sTrafficInfoData.szRseId, &pRequest->szRseId, sizeof(pRequest->szRseId));

	////////////////////////////////
	// 스레드를 동작시킨다.
#ifdef _LINUX
	pthread_cond_signal(&m_pModuleProcCond);
	pthread_mutex_unlock(&m_pModuleProcMutex);
#else
	SDL_CondSignal(m_pModuleProcCond);
	SDL_UnlockMutex(m_pModuleProcMutex);
#endif

	delay(1);	//스레드 스위칭

	if(pRequest->bSync)
	{
		// 스레드 처리가 끝날때 까지 대기한다.
#ifdef _LINUX
		pthread_mutex_lock(&m_pModuleProcMutex);
		pthread_mutex_unlock(&m_pModuleProcMutex);
#else
		SDL_LockMutex(m_pModuleProcMutex);
		SDL_UnlockMutex(m_pModuleProcMutex);
#endif

		pResult->nResult = m_sModuleOutputProcData.sTrafficInfoData.nResult;
		strcpy(pResult->szErrorMsg, m_sModuleOutputProcData.sTrafficInfoData.szErrorMsg);
	}

	pResult->nSessionID = nSessionID;

	return true;
}

bool CNddsModule::API_NDDS_GetVmsGuideInfo(PAPI_GET_VMSGUIDEINFO_REQ pRequest, PAPI_GET_VMSGUIDEINFO_RES pResult)
{
	memset(pResult, 0x00, sizeof(pResult));

	strcpy(pResult->szBaseSdttm, m_pNddsVmsGuideInfo.m_BaseSdttm);
	strcpy(pResult->szDataSource, m_pNddsVmsGuideInfo.m_DataSource);
	strcpy(pResult->szContentCode, m_pNddsVmsGuideInfo.m_ContentCode);
	strcpy(pResult->szContentName, m_pNddsVmsGuideInfo.m_ContentName);
	strcpy(pResult->szContent, m_pNddsVmsGuideInfo.m_Content);

	return true;
}

int CNddsModule::NddsVmsGuideInfo(int nSessionID, eNddsRequestType eRequestType, char *szRseId, char *szErrorMsg)
{
	eNddsErrorCode eErrCode = NDDSERR_UNKNOWN;

	memset(&m_pNddsVmsGuideInfo, 0x00, sizeof(m_pNddsVmsGuideInfo));
	m_nTrafficInfoCnt = 0;
	*szErrorMsg = NULL;

	NDDSTMAP_VMSGUIDEINFO_REQ sRequest;
	memset(&sRequest, 0x00, sizeof(NDDSTMAP_VMSGUIDEINFO_REQ));

	sRequest.rseId = szRseId;

	if(!NDDSLIB_RequestProtocol(NDDSTYPE_TMAP_VMSGUIDEINFO, eRequestType, (NDDS_COMMON_REQ*)(&sRequest), nSessionID, &eErrCode))
	{
		if(eErrCode == NDDSERR_USERCANCEL)
			strcpy(szErrorMsg, "사용자 취소");
		else
			strcpy(szErrorMsg, "데이터 생성 실패");

		return 0;
	}

	if(eRequestType == NDDSREQUEST_ATONCE)
	{
		return true;
	}

    eNddsStatus eStatus = NDDSSTATUS_WAIT;
    int nProgress = 0;
    DBGPRINT(_T("Wait "));
    unsigned long reqTime = GetTickCount();
    while(eStatus != NDDSSTATUS_COMPLETE && eStatus != NDDSSTATUS_CANCELED)
    {
        unsigned long thisTime = GetTickCount();
        unsigned long wattingTime = thisTime - reqTime;

        if(wattingTime > SEARCH_WAITTING_TIME)
        {
            strcpy(szErrorMsg, "서버 응답 없음");
            eErrCode = NDDSERR_TIMEOUT;
            return false;
        }

        DBGPRINT(_T("."));
        delay(100);
        NDDSLIB_GetStatusProtocol(nSessionID, &eStatus, &nProgress, &eErrCode);
    }

	int nNddsErrorCode = -1;

	return GetNddsVmsGuideInfoResult(nSessionID, &nNddsErrorCode);
}

int CNddsModule::GetNddsVmsGuideInfoResult(int nSessionID, int *nErrorCode)
{
	eNddsErrorCode eErrCode = NDDSERR_UNKNOWN;
	char szErrorMsg[256] = {0,};

	NDDS_COMMON_RES *pRes = NULL;
	NDDSTMAP_VMSGUIDEINFO_RES *pResStruct = NULL;
	eNddsProtocolType eResProtocolType = NDDSTYPE_NONE;
	NDDSLIB_GetResultProtocol(nSessionID, false, &eResProtocolType, &pRes, &eErrCode);
	pResStruct = (NDDSTMAP_VMSGUIDEINFO_RES*)pRes;

	if(pResStruct != NULL)
	{
		IsChangeAccesskey(nSessionID, pResStruct->header.accessKey);
		CheckErrorCode(nSessionID, pResStruct->header.errorCode);
	}

	if(!NddsErrorCheck(eErrCode, pResStruct?pResStruct->header.errorMessage:NULL, szErrorMsg))
	{
        NDDSLIB_ReleaseObject(pRes, eErrCode);
		return 0;
	}

	if (pResStruct == NULL)
	{
		*nErrorCode = NDDSERR_RESULT_NULL;
		return false;
	}

	if(eErrCode == NDDSERR_SUCCESS)
	{
		if(pResStruct->baseSdttm)
			strcpy(m_pNddsVmsGuideInfo.m_BaseSdttm, pResStruct->baseSdttm);
		if(pResStruct->dataSource)
			strcpy(m_pNddsVmsGuideInfo.m_DataSource, pResStruct->dataSource);
		if(pResStruct->contentCode)
			strcpy(m_pNddsVmsGuideInfo.m_ContentCode, pResStruct->contentCode);
		if(pResStruct->contentName)
			strcpy(m_pNddsVmsGuideInfo.m_ContentName, pResStruct->contentName);
		if(pResStruct->content)
			strcpy(m_pNddsVmsGuideInfo.m_Content, pResStruct->content);

		m_nTrafficInfoCnt = 1;
	}

    NDDSLIB_ReleaseObject(pRes);

	return m_nTrafficInfoCnt;
}

bool CNddsModule::API_NDDS_AuthTmapForSmartCar(PAPI_AUTHTMAPFORSMARTCAR_REQ pRequest, PAPI_AUTHTMAPFORSMARTCAR_RES pResult)
{
	//DBGPRINT(_T("############################# Auth #1\n"));

	// 모듈 프로세스 스레드에 요청 중이거나 동작중인지 체크 하여 있으면 검색이면 중지후 요청하고 나머지는 실패 처리한다.
#ifdef _LINUX
	if(sem_wait(&m_pModuleProcSem) != 0)
#else
	if(SDL_SemWait(m_pModuleProcSem) == SDL_MUTEX_TIMEDOUT)
#endif
	{
		return false;
	}

	//DBGPRINT(_T("MUTEX[%d] - API_NDDS_Auth::LockMutex #1\n"), GetTickCount());
#ifdef _LINUX
	pthread_mutex_lock(&m_pModuleProcMutex);
#else
	SDL_LockMutex(m_pModuleProcMutex);
#endif
	//DBGPRINT(_T("MUTEX[%d] - API_NDDS_Auth::LockMutex #2\n"), GetTickCount());
	//DBGPRINT(_T("############################# Auth #2\n"));

	int nSessionID = ++m_nLastIdxSession;

	SendMessgeCallback(NM_NDDS_AUTH_INIT, nSessionID);

	//////////////////////////////////////
	//Search Input 데이터를 설정한다.

	m_sModuleInputProcData.eProcType = E_NDDS_PROCTYPE_AUTH;
	m_sModuleInputProcData.nAuthSessionID = nSessionID;
	m_sModuleInputProcData.bAuthSync = pRequest->bSync;
	memcpy(&m_sModuleInputProcData.sAuthData.szOtpCode, &pRequest->szOtpCode, sizeof(pRequest->szOtpCode));
	////////////////////////////////
	// 스레드를 동작시킨다.
#ifdef _LINUX
	pthread_cond_signal(&m_pModuleProcCond);
	pthread_mutex_unlock(&m_pModuleProcMutex);
#else
	SDL_CondSignal(m_pModuleProcCond);
	SDL_UnlockMutex(m_pModuleProcMutex);
#endif

	delay(15);	//스레드 스위칭

	if(pRequest->bSync)
	{
		// 스레드 처리가 끝날때 까지 대기한다.
#ifdef _LINUX
		pthread_mutex_lock(&m_pModuleProcMutex);
		pthread_mutex_unlock(&m_pModuleProcMutex);
#else
		SDL_LockMutex(m_pModuleProcMutex);
		SDL_UnlockMutex(m_pModuleProcMutex);
#endif

		pResult->nResult = m_sModuleOutputProcData.sAuthData.nResult;
		strcpy(pResult->szErrorMsg, m_sModuleOutputProcData.sAuthData.szErrorMsg);
	}

	//DBGPRINT(_T("############################# Auth #true\n"));

	pResult->nSessionID = nSessionID;

	return true;
}

bool CNddsModule::API_NDDS_GetAuthResult(PAPI_GET_AUTHRESULT_REQ pRequest, PAPI_GET_AUTHRESULT_RES pResult)
{
	memset(pResult, 0x00, sizeof(pResult));

	strcpy(pResult->szErrorCode, m_pNddsAuthTmapForSmartCar.m_ErrorCode);
	strcpy(pResult->szAccessKey, m_pNddsAuthTmapForSmartCar.m_AccessKey);

	strcpy(pResult->sCommInfo.szEuk,m_pNddsAuthTmapForSmartCar.m_CommonInfo.m_Euk);
	strcpy(pResult->sCommInfo.szEid,m_pNddsAuthTmapForSmartCar.m_CommonInfo.m_Eid);

	pResult->sAuthInfo.nValidateCode = m_pNddsAuthTmapForSmartCar.m_AuthInfo.m_ValidateCode;
	strcpy(pResult->sAuthInfo.szValidationSubField,m_pNddsAuthTmapForSmartCar.m_AuthInfo.m_ValidationSubField);
	//strcpy(pResult->sAuthInfo.szValidateGroup,m_pNddsAuthTmapForSmartCar.m_AuthInfo.m_ValidateGroup);
	if(m_pNddsAuthTmapForSmartCar.m_AuthInfo.m_ExternalUserInfoCount > 6)
	{
		pResult->sAuthInfo.nExternalUserInfoCount = 6;
		ASSERT(0);
	}
	else
		pResult->sAuthInfo.nExternalUserInfoCount = m_pNddsAuthTmapForSmartCar.m_AuthInfo.m_ExternalUserInfoCount;
	for(int i=0; i<pResult->sAuthInfo.nExternalUserInfoCount; i++)
	{
		strcpy(pResult->sAuthInfo.sExternalUserInfos[i].szProvider, m_pNddsAuthTmapForSmartCar.m_AuthInfo.m_ExternalUserInfos[i].m_Provider);
		strcpy(pResult->sAuthInfo.sExternalUserInfos[i].szReadableUserId, m_pNddsAuthTmapForSmartCar.m_AuthInfo.m_ExternalUserInfos[i].m_ReadableUserId);
	}

	pResult->sHeadUnitDeviceAuthInfo.nResultCode = m_pNddsAuthTmapForSmartCar.m_HeadUnitDeviceAuthInfo.m_ResultCode;
	pResult->sHeadUnitDeviceAuthInfo.nResultSubField = m_pNddsAuthTmapForSmartCar.m_HeadUnitDeviceAuthInfo.m_ResultSubField;

	pResult->sTidAuthResInfo.nResultCode = m_pNddsAuthTmapForSmartCar.m_TidAuthResInfo.m_ResultCode;
	pResult->sTidAuthResInfo.nResultSubField = m_pNddsAuthTmapForSmartCar.m_TidAuthResInfo.m_ResultSubField;
	pResult->sTidAuthResInfo.nResultMdnAuthInfo = m_pNddsAuthTmapForSmartCar.m_TidAuthResInfo.m_ResultMdnAuthInfo;
	if(m_pNddsAuthTmapForSmartCar.m_TidAuthResInfo.m_ExternalUserInfoCount > 6)
	{
		pResult->sTidAuthResInfo.nExternalUserInfoCount = 6;
		ASSERT(0);
	}
	else
		pResult->sTidAuthResInfo.nExternalUserInfoCount = m_pNddsAuthTmapForSmartCar.m_TidAuthResInfo.m_ExternalUserInfoCount;
	for(int i=0; i<pResult->sTidAuthResInfo.nExternalUserInfoCount; i++)
	{
		strcpy(pResult->sTidAuthResInfo.sExternalUserInfos[i].szProvider, m_pNddsAuthTmapForSmartCar.m_TidAuthResInfo.m_ExternalUserInfos[i].m_Provider);
		strcpy(pResult->sTidAuthResInfo.sExternalUserInfos[i].szReadableUserId, m_pNddsAuthTmapForSmartCar.m_TidAuthResInfo.m_ExternalUserInfos[i].m_ReadableUserId);
	}

	strcpy(pResult->sVersionInfo.szMenuNoticeVer,m_pNddsAuthTmapForSmartCar.m_VersionInfo.m_MenuNoticeVer);
	strcpy(pResult->sVersionInfo.szOpenappVer,m_pNddsAuthTmapForSmartCar.m_VersionInfo.m_OpenappVer);

	strcpy(pResult->sAppUpdateInfo.szAppUpdateType,m_pNddsAuthTmapForSmartCar.m_AppUpdateInfo.m_AppUpdateType);
	strcpy(pResult->sAppUpdateInfo.szAppVer,m_pNddsAuthTmapForSmartCar.m_AppUpdateInfo.m_AppVer);
	strcpy(pResult->sAppUpdateInfo.szAppUpdateViewFlag,m_pNddsAuthTmapForSmartCar.m_AppUpdateInfo.m_AppUpdateViewFlag);
	strcpy(pResult->sAppUpdateInfo.szAppUpdateUrl,m_pNddsAuthTmapForSmartCar.m_AppUpdateInfo.m_AppUpdateUrl);
	strcpy(pResult->sAppUpdateInfo.szAppUpdateMsg,m_pNddsAuthTmapForSmartCar.m_AppUpdateInfo.m_AppUpdateMsg);
	 
	if(m_pNddsAuthTmapForSmartCar.m_AppControlInfo.m_ControlInfoCount > 6)
	{
		pResult->sAppControlInfo.nControlInfoCount = 6;
		ASSERT(0);
	}
	else
		pResult->sAppControlInfo.nControlInfoCount = m_pNddsAuthTmapForSmartCar.m_AppControlInfo.m_ControlInfoCount;
	for(int i=0; i<pResult->sAppControlInfo.nControlInfoCount; i++)
	{
		strcpy(pResult->sAppControlInfo.sControlInfoDetails[i].szServiceName, m_pNddsAuthTmapForSmartCar.m_AppControlInfo.m_ControlInfoDetails[i].m_ServiceName);
		strcpy(pResult->sAppControlInfo.sControlInfoDetails[i].szServiceYn, m_pNddsAuthTmapForSmartCar.m_AppControlInfo.m_ControlInfoDetails[i].m_ServiceYn);
		strcpy(pResult->sAppControlInfo.sControlInfoDetails[i].szServiceItems, m_pNddsAuthTmapForSmartCar.m_AppControlInfo.m_ControlInfoDetails[i].m_ServiceItems);
	}

	strcpy(pResult->sPushInfo.szPushSetYn, m_pNddsAuthTmapForSmartCar.m_PushInfo.m_PushSetYn);
	strcpy(pResult->sPushInfo.szSmsYn, m_pNddsAuthTmapForSmartCar.m_PushInfo.m_SmsYn);
	strcpy(pResult->sPushInfo.szMomentYn, m_pNddsAuthTmapForSmartCar.m_PushInfo.m_MomentYn);

	strcpy(pResult->sUserProfileInfo.sPersonalInfo.szEmail, m_pNddsAuthTmapForSmartCar.m_UserProfileInfo.m_PersonalInfo.m_Email);
	strcpy(pResult->sUserProfileInfo.sPersonalInfo.szUserName, m_pNddsAuthTmapForSmartCar.m_UserProfileInfo.m_PersonalInfo.m_UserName);
	strcpy(pResult->sUserProfileInfo.sPersonalInfo.szMdn, m_pNddsAuthTmapForSmartCar.m_UserProfileInfo.m_PersonalInfo.m_Mdn);
	strcpy(pResult->sUserProfileInfo.sPersonalInfo.szBirth, m_pNddsAuthTmapForSmartCar.m_UserProfileInfo.m_PersonalInfo.m_Email);
	strcpy(pResult->sUserProfileInfo.sCarInfo.szModel, m_pNddsAuthTmapForSmartCar.m_UserProfileInfo.m_CarInfo.m_Model);
	strcpy(pResult->sUserProfileInfo.sCarInfo.szFuel, m_pNddsAuthTmapForSmartCar.m_UserProfileInfo.m_CarInfo.m_Fuel);
	strcpy(pResult->sUserProfileInfo.sCarInfo.szHipassYn, m_pNddsAuthTmapForSmartCar.m_UserProfileInfo.m_CarInfo.m_HipassYn);
	strcpy(pResult->sUserProfileInfo.sCarInfo.szNumber, m_pNddsAuthTmapForSmartCar.m_UserProfileInfo.m_CarInfo.m_Number);

	return true;
}


bool CNddsModule::API_NDDS_GetUserData(PAPI_GET_USERDATA_REQ pRequest, PAPI_GET_USERDATA_RES pResult)
{
    memset(pResult, 0x00, sizeof(pResult));
    //poiMyFavorite
    strcpy(pResult->sUserData.sPoiMyFavorite.szHomeCustName, m_pNddsUserData.sPoiMyFavorite.m_HomeCustName);
    pResult->sUserData.sPoiMyFavorite.Hometp.x = m_pNddsUserData.sPoiMyFavorite.m_HomeX;
    pResult->sUserData.sPoiMyFavorite.Hometp.y = m_pNddsUserData.sPoiMyFavorite.m_HomeY;
    pResult->sUserData.sPoiMyFavorite.nHomePoiId = m_pNddsUserData.sPoiMyFavorite.m_HomePoiId;
    strcpy(pResult->sUserData.sPoiMyFavorite.szHomeNavSeq, m_pNddsUserData.sPoiMyFavorite.m_HomeNavSeq);
    strcpy(pResult->sUserData.sPoiMyFavorite.szHomeLcdName, m_pNddsUserData.sPoiMyFavorite.m_HomeLcdName);
    strcpy(pResult->sUserData.sPoiMyFavorite.szHomeMcdName, m_pNddsUserData.sPoiMyFavorite.m_HomeMcdName);
    strcpy(pResult->sUserData.sPoiMyFavorite.szHomeScdName, m_pNddsUserData.sPoiMyFavorite.m_HomeScdName);
    strcpy(pResult->sUserData.sPoiMyFavorite.szHomeDcdName, m_pNddsUserData.sPoiMyFavorite.m_HomeDcdName);
    strcpy(pResult->sUserData.sPoiMyFavorite.szHomePrimaryBun, m_pNddsUserData.sPoiMyFavorite.m_HomePrimaryBun);
    strcpy(pResult->sUserData.sPoiMyFavorite.szHomeSecondBun, m_pNddsUserData.sPoiMyFavorite.m_HomeSecondBun);
    pResult->sUserData.sPoiMyFavorite.nHomeMlClass = m_pNddsUserData.sPoiMyFavorite.m_HomeMlClass;
    strcpy(pResult->sUserData.sPoiMyFavorite.szHomeRoadName, m_pNddsUserData.sPoiMyFavorite.m_HomeRoadName);
    strcpy(pResult->sUserData.sPoiMyFavorite.szHomeBldNo1, m_pNddsUserData.sPoiMyFavorite.m_HomeBldNo1);
    strcpy(pResult->sUserData.sPoiMyFavorite.szHomeBldNo2, m_pNddsUserData.sPoiMyFavorite.m_HomeBldNo2);
    pResult->sUserData.sPoiMyFavorite.byHomeRpFlag =  m_pNddsUserData.sPoiMyFavorite.m_HomeRpFlag;
    strcpy(pResult->sUserData.sPoiMyFavorite.szHomeAddInfo, m_pNddsUserData.sPoiMyFavorite.m_HomeAddInfo);
    strcpy(pResult->sUserData.sPoiMyFavorite.szHomeInsDatetime, m_pNddsUserData.sPoiMyFavorite.m_HomeInsDatetime);

    strcpy(pResult->sUserData.sPoiMyFavorite.szOfficeCustName, m_pNddsUserData.sPoiMyFavorite.m_OfficeCustName);
    pResult->sUserData.sPoiMyFavorite.Officetp.x = (m_pNddsUserData.sPoiMyFavorite.m_OfficeX);
    pResult->sUserData.sPoiMyFavorite.Officetp.y = (m_pNddsUserData.sPoiMyFavorite.m_OfficeY);
    pResult->sUserData.sPoiMyFavorite.nOfficePoiId = (m_pNddsUserData.sPoiMyFavorite.m_OfficePoiId);
    strcpy(pResult->sUserData.sPoiMyFavorite.szOfficeNavSeq, m_pNddsUserData.sPoiMyFavorite.m_OfficeNavSeq);
    strcpy(pResult->sUserData.sPoiMyFavorite.szOfficeLcdName, m_pNddsUserData.sPoiMyFavorite.m_OfficeLcdName);
    strcpy(pResult->sUserData.sPoiMyFavorite.szOfficeMcdName, m_pNddsUserData.sPoiMyFavorite.m_OfficeMcdName);
    strcpy(pResult->sUserData.sPoiMyFavorite.szOfficeScdName, m_pNddsUserData.sPoiMyFavorite.m_OfficeScdName);
    strcpy(pResult->sUserData.sPoiMyFavorite.szOfficeDcdName, m_pNddsUserData.sPoiMyFavorite.m_OfficeDcdName);
    strcpy(pResult->sUserData.sPoiMyFavorite.szOfficePrimaryBun, m_pNddsUserData.sPoiMyFavorite.m_OfficePrimaryBun);
    strcpy(pResult->sUserData.sPoiMyFavorite.szOfficeSecondBun, m_pNddsUserData.sPoiMyFavorite.m_OfficeSecondBun);
    pResult->sUserData.sPoiMyFavorite.nOfficeMlClass = m_pNddsUserData.sPoiMyFavorite.m_OfficeMlClass;
    strcpy(pResult->sUserData.sPoiMyFavorite.szOfficeRoadName, m_pNddsUserData.sPoiMyFavorite.m_OfficeRoadName);
    strcpy(pResult->sUserData.sPoiMyFavorite.szOfficeBldNo1, m_pNddsUserData.sPoiMyFavorite.m_OfficeBldNo1);
    strcpy(pResult->sUserData.sPoiMyFavorite.szOfficeBldNo2, m_pNddsUserData.sPoiMyFavorite.m_OfficeBldNo2);
    pResult->sUserData.sPoiMyFavorite.byOfficeRpFlag =  m_pNddsUserData.sPoiMyFavorite.m_OfficeRpFlag;
    strcpy(pResult->sUserData.sPoiMyFavorite.szOfficeAddInfo, m_pNddsUserData.sPoiMyFavorite.m_OfficeAddInfo);
    strcpy(pResult->sUserData.sPoiMyFavorite.szOfficeInsDatetime, m_pNddsUserData.sPoiMyFavorite.m_OfficeInsDatetime);

    //routeMyFavorite
    strcpy(pResult->sUserData.sRouteMyFavorite.routeTitle, m_pNddsUserData.sRouteMyFavorite.routeTitle);
    strcpy(pResult->sUserData.sRouteMyFavorite.insDatetime, m_pNddsUserData.sRouteMyFavorite.insDatetime);
    //start Route
    strcpy(pResult->sUserData.sRouteMyFavorite.sStartRoute.szCustName, m_pNddsUserData.sRouteMyFavorite.sStartRoute.szCustName);
    pResult->sUserData.sRouteMyFavorite.sStartRoute.nPoiID = m_pNddsUserData.sRouteMyFavorite.sStartRoute.nPoiID;
    pResult->sUserData.sRouteMyFavorite.sStartRoute.byRpFlag = m_pNddsUserData.sRouteMyFavorite.sStartRoute.byRpFlag;
    pResult->sUserData.sRouteMyFavorite.sStartRoute.tp.x = m_pNddsUserData.sRouteMyFavorite.sStartRoute.tp.x;
    pResult->sUserData.sRouteMyFavorite.sStartRoute.tp.y = m_pNddsUserData.sRouteMyFavorite.sStartRoute.tp.y;
    //end Route
    strcpy(pResult->sUserData.sRouteMyFavorite.sEndRoute.szCustName, m_pNddsUserData.sRouteMyFavorite.sEndRoute.szCustName);
    pResult->sUserData.sRouteMyFavorite.sEndRoute.nPoiID = m_pNddsUserData.sRouteMyFavorite.sEndRoute.nPoiID;
    pResult->sUserData.sRouteMyFavorite.sEndRoute.byRpFlag = m_pNddsUserData.sRouteMyFavorite.sEndRoute.byRpFlag;
    pResult->sUserData.sRouteMyFavorite.sEndRoute.tp.x = m_pNddsUserData.sRouteMyFavorite.sEndRoute.tp.x;
    pResult->sUserData.sRouteMyFavorite.sEndRoute.tp.y = m_pNddsUserData.sRouteMyFavorite.sEndRoute.tp.y;
    for(int i=0;i<3;i++)
    {
        //mid Route1
        strcpy(pResult->sUserData.sRouteMyFavorite.sMidRoute[i].szCustName, m_pNddsUserData.sRouteMyFavorite.sMidRoute[i].szCustName);
        pResult->sUserData.sRouteMyFavorite.sMidRoute[i].nPoiID = m_pNddsUserData.sRouteMyFavorite.sMidRoute[i].nPoiID;
        pResult->sUserData.sRouteMyFavorite.sMidRoute[i].byRpFlag = m_pNddsUserData.sRouteMyFavorite.sMidRoute[i].byRpFlag;
        pResult->sUserData.sRouteMyFavorite.sMidRoute[i].tp.x = m_pNddsUserData.sRouteMyFavorite.sMidRoute[i].tp.x;
        pResult->sUserData.sRouteMyFavorite.sMidRoute[i].tp.y = m_pNddsUserData.sRouteMyFavorite.sMidRoute[i].tp.y;
    }

    //poiRecent
    for(int i=0;i<m_pNddsUserData.nPoiRecent;i++)
    {
        pResult->sUserData.sPoiRecent[i].nPoiID = m_pNddsUserData.sPoiRecent[i].nPoiID;
        strcpy(pResult->sUserData.sPoiRecent[i].szNavseq, m_pNddsUserData.sPoiRecent[i].szNavSeq);
        strcpy(pResult->sUserData.sPoiRecent[i].szName, m_pNddsUserData.sPoiRecent[i].szCustName);
        pResult->sUserData.sPoiRecent[i].tp.x = m_pNddsUserData.sPoiRecent[i].tp.x;
        pResult->sUserData.sPoiRecent[i].tp.y = m_pNddsUserData.sPoiRecent[i].tp.y;
        strcpy(pResult->sUserData.sPoiRecent[i].szLcdName, m_pNddsUserData.sPoiRecent[i].szLcdName);
        strcpy(pResult->sUserData.sPoiRecent[i].szMcdName, m_pNddsUserData.sPoiRecent[i].szMcdName);
        strcpy(pResult->sUserData.sPoiRecent[i].szScdName, m_pNddsUserData.sPoiRecent[i].szScdName);
        strcpy(pResult->sUserData.sPoiRecent[i].szDcdName, m_pNddsUserData.sPoiRecent[i].szDcdName);
        strcpy(pResult->sUserData.sPoiRecent[i].szPrimaryBun, m_pNddsUserData.sPoiRecent[i].szPrimaryBun);
        strcpy(pResult->sUserData.sPoiRecent[i].szSecondBun, m_pNddsUserData.sPoiRecent[i].szSecondBun);
        pResult->sUserData.sPoiRecent[i].nMlClass = m_pNddsUserData.sPoiRecent[i].nMlClass;
        strcpy(pResult->sUserData.sPoiRecent[i].szRoadName, m_pNddsUserData.sPoiRecent[i].szRoadName);
        strcpy(pResult->sUserData.sPoiRecent[i].szBldNo1, m_pNddsUserData.sPoiRecent[i].szBldNo1);
        strcpy(pResult->sUserData.sPoiRecent[i].szBldNo2, m_pNddsUserData.sPoiRecent[i].szBldNo2);
        strcpy(pResult->sUserData.sPoiRecent[i].szRepClsName, m_pNddsUserData.sPoiRecent[i].szRepClsName);
        strcpy(pResult->sUserData.sPoiRecent[i].szClsAName, m_pNddsUserData.sPoiRecent[i].szClsAName);
        strcpy(pResult->sUserData.sPoiRecent[i].szClsBName, m_pNddsUserData.sPoiRecent[i].szClsBName);
        strcpy(pResult->sUserData.sPoiRecent[i].szClsCName, m_pNddsUserData.sPoiRecent[i].szClsCName);
        strcpy(pResult->sUserData.sPoiRecent[i].szClsDName, m_pNddsUserData.sPoiRecent[i].szClsDName);
        pResult->sUserData.sPoiRecent[i].byRPFlag = m_pNddsUserData.sPoiRecent[i].byRPFlag;
        strcpy(pResult->sUserData.sPoiRecent[i].szTotalCnt, m_pNddsUserData.sPoiRecent[i].szTotalCnt);
        strcpy(pResult->sUserData.sPoiRecent[i].szSvcDate, m_pNddsUserData.sPoiRecent[i].szSvcDate);
    }

    //poiFavorite
    for(int i=0;i<m_pNddsUserData.nPoiFavorite;i++)
    {
        strcpy(pResult->sUserData.sPoiFavorite[i].szCustName, m_pNddsUserData.sPoiFavorite[i].szName);
        pResult->sUserData.sPoiFavorite[i].tp.x = m_pNddsUserData.sPoiFavorite[i].tp.x;
        pResult->sUserData.sPoiFavorite[i].tp.y = m_pNddsUserData.sPoiFavorite[i].tp.y;
        pResult->sUserData.sPoiFavorite[i].nPoiID = m_pNddsUserData.sPoiFavorite[i].nPoiID;
        sprintf(pResult->sUserData.sPoiFavorite[i].szNavSeq, "%d", m_pNddsUserData.sPoiFavorite[i].nPoiID);
        strcpy(pResult->sUserData.sPoiFavorite[i].szLcdName, m_pNddsUserData.sPoiFavorite[i].szLcdName);
        strcpy(pResult->sUserData.sPoiFavorite[i].szMcdName, m_pNddsUserData.sPoiFavorite[i].szMcdName);
        strcpy(pResult->sUserData.sPoiFavorite[i].szDcdName, m_pNddsUserData.sPoiFavorite[i].szDcdName);
        strcpy(pResult->sUserData.sPoiFavorite[i].szPrimaryBun, m_pNddsUserData.sPoiFavorite[i].szPrimaryBun);
        strcpy(pResult->sUserData.sPoiFavorite[i].szSecondBun, m_pNddsUserData.sPoiFavorite[i].szSecondBun);
        pResult->sUserData.sPoiFavorite[i].nMlClass = m_pNddsUserData.sPoiFavorite[i].nMlClass;
        strcpy(pResult->sUserData.sPoiFavorite[i].szRoadName, m_pNddsUserData.sPoiFavorite[i].szRoadName);
        strcpy(pResult->sUserData.sPoiFavorite[i].szBldNo1, m_pNddsUserData.sPoiFavorite[i].szBldNo1);
        strcpy(pResult->sUserData.sPoiFavorite[i].szBldNo2, m_pNddsUserData.sPoiFavorite[i].szBldNo2);
        strcpy(pResult->sUserData.sPoiFavorite[i].szRepClsName, m_pNddsUserData.sPoiFavorite[i].szRepClsName);
        strcpy(pResult->sUserData.sPoiFavorite[i].szClsAName, m_pNddsUserData.sPoiFavorite[i].szClsAName);
        strcpy(pResult->sUserData.sPoiFavorite[i].szClsBName, m_pNddsUserData.sPoiFavorite[i].szClsBName);
        strcpy(pResult->sUserData.sPoiFavorite[i].szClsCName, m_pNddsUserData.sPoiFavorite[i].szClsCName);
        strcpy(pResult->sUserData.sPoiFavorite[i].szClsDName, m_pNddsUserData.sPoiFavorite[i].szClsDName);
        pResult->sUserData.sPoiFavorite[i].byRPFlag = m_pNddsUserData.sPoiFavorite[i].byRPFlag;
        strcpy(pResult->sUserData.sPoiFavorite[i].szTelNo, m_pNddsUserData.sPoiFavorite[i].szTelNo);
        strcpy(pResult->sUserData.sPoiFavorite[i].szAddInfo, m_pNddsUserData.sPoiFavorite[i].szAddInfo);
        strcpy(pResult->sUserData.sPoiFavorite[i].szOrgCustName, m_pNddsUserData.sPoiFavorite[i].szOrgCustName);
        strcpy(pResult->sUserData.sPoiFavorite[i].szInsDatetime, m_pNddsUserData.sPoiFavorite[i].szInsDatetime);
    }

    //routeFavorite
    for(int i=0;i<m_pNddsUserData.nRouteFavorite;i++)
    {
        strcpy(pResult->sUserData.sRouteFavorite[i].routeTitle, m_pNddsUserData.sRouteFavorite[i].routeTitle);
        strcpy(pResult->sUserData.sRouteFavorite[i].insDatetime, m_pNddsUserData.sRouteFavorite[i].insDatetime);
        //start Route
        strcpy(pResult->sUserData.sRouteFavorite[i].sStartRoute.szCustName, m_pNddsUserData.sRouteFavorite[i].sStartRoute.szCustName);
        pResult->sUserData.sRouteFavorite[i].sStartRoute.nPoiID = m_pNddsUserData.sRouteFavorite[i].sStartRoute.nPoiID;
        pResult->sUserData.sRouteFavorite[i].sStartRoute.byRpFlag = m_pNddsUserData.sRouteFavorite[i].sStartRoute.byRpFlag;
        pResult->sUserData.sRouteFavorite[i].sStartRoute.tp.x = m_pNddsUserData.sRouteFavorite[i].sStartRoute.tp.x;
        pResult->sUserData.sRouteFavorite[i].sStartRoute.tp.y = m_pNddsUserData.sRouteFavorite[i].sStartRoute.tp.y;
        //end Route
        strcpy(pResult->sUserData.sRouteFavorite[i].sEndRoute.szCustName, m_pNddsUserData.sRouteFavorite[i].sEndRoute.szCustName);
        pResult->sUserData.sRouteFavorite[i].sEndRoute.nPoiID = m_pNddsUserData.sRouteFavorite[i].sEndRoute.nPoiID;
        pResult->sUserData.sRouteFavorite[i].sEndRoute.byRpFlag = m_pNddsUserData.sRouteFavorite[i].sEndRoute.byRpFlag;
        pResult->sUserData.sRouteFavorite[i].sEndRoute.tp.x = m_pNddsUserData.sRouteFavorite[i].sEndRoute.tp.x;
        pResult->sUserData.sRouteFavorite[i].sEndRoute.tp.y = m_pNddsUserData.sRouteFavorite[i].sEndRoute.tp.y;

        //mid Route
        for(int j=0;j<3;j++)
        {
            strcpy(pResult->sUserData.sRouteFavorite[i].sMidRoute[j].szCustName, m_pNddsUserData.sRouteFavorite[i].sMidRoute[j].szCustName);
            pResult->sUserData.sRouteFavorite[i].sMidRoute[j].nPoiID = m_pNddsUserData.sRouteFavorite[i].sMidRoute[j].nPoiID;
            pResult->sUserData.sRouteFavorite[i].sMidRoute[j].byRpFlag = m_pNddsUserData.sRouteFavorite[i].sMidRoute[j].byRpFlag;
            pResult->sUserData.sRouteFavorite[i].sMidRoute[j].tp.x = m_pNddsUserData.sRouteFavorite[i].sMidRoute[j].tp.x;
            pResult->sUserData.sRouteFavorite[i].sMidRoute[j].tp.y = m_pNddsUserData.sRouteFavorite[i].sMidRoute[j].tp.y;
        }
    }

    return true;
}

bool CNddsModule::API_NDDS_SetReqDataSave(PAPI_SETREQDATASAVE_REQ pRequest, PAPI_SETREQDATASAVE_RES pResult)
{
    return NDDSLIB_SetReqDataSave(pRequest->bLogSave, pRequest->szLogSavePath);
}

bool CNddsModule::API_NDDS_SetAuthAccessKey(PAPI_SET_AUTHACCESSKEY_REQ pRequest, PAPI_SET_AUTHACCESSKEY_RES pResult)
{
	memset(m_szAccessKey, 0x00, sizeof(m_szAccessKey));
	strcpy(m_szAccessKey, pRequest->szAccessKey);

	return NDDSLIB_SetAuthAccessKey(pRequest->szAccessKey);
}

int CNddsModule::NddsAuthforSmartCar(int nSessionID, eNddsRequestType eRequestType, char *szOtpCode, int &nValidateCode, int &nResultCode, char *szAccessKey, int &nErrCode, char *szErrorMsg)
{
	eNddsErrorCode eErrCode = NDDSERR_UNKNOWN;

	memset(&m_pNddsAuthTmapForSmartCar, 0x00, sizeof(m_pNddsAuthTmapForSmartCar));
	m_nAuthCnt = 0;
	*szErrorMsg = NULL;

	NDDSTMAP_AUTHFORSMARTCAR_REQ sRequest;
	memset(&sRequest, 0x00, sizeof(NDDSTMAP_AUTHFORSMARTCAR_REQ));
	sRequest.otpCode = szOtpCode;

	if(!NDDSLIB_RequestProtocol(NDDSTYPE_TMAP_AUTHFORSMARTCAR, eRequestType, (NDDS_COMMON_REQ*)(&sRequest), nSessionID, &eErrCode))
	{
        nErrCode = (int) eErrCode;

		if(eErrCode == NDDSERR_USERCANCEL)
			strcpy(szErrorMsg, "사용자 취소");
		else
			strcpy(szErrorMsg, "데이터 생성 실패");

		return 0;
	}

    nErrCode = (int) eErrCode;

	if(eRequestType == NDDSREQUEST_ATONCE)
	{
		return true;
	}

    eNddsStatus eStatus = NDDSSTATUS_WAIT;
    int nProgress = 0;
    DBGPRINT(_T("Wait "));
    unsigned long reqTime = GetTickCount();
    while(eStatus != NDDSSTATUS_COMPLETE && eStatus != NDDSSTATUS_CANCELED)
    {
        unsigned long thisTime = GetTickCount();
        unsigned long wattingTime = thisTime - reqTime;

        if(wattingTime > SEARCH_WAITTING_TIME)
        {
            strcpy(szErrorMsg, "서버 응답 없음");
            eErrCode = NDDSERR_TIMEOUT;
            return false;
        }

        DBGPRINT(_T("."));
        delay(100);
        NDDSLIB_GetStatusProtocol(nSessionID, &eStatus, &nProgress, &eErrCode);
    }

    //int nNddsErrorCode = -1;

    return GetNddsAuthforSmartCarResult(nSessionID, nValidateCode, nResultCode, szAccessKey, &nErrCode);
}

bool CNddsModule::API_NDDS_NonNdds(PAPI_NONNDDS_REQ pRequest, PAPI_NONNDDS_RES pResult)
{
    //DBGPRINT(_T("############################# Search #1\n"));

    // 모듈 프로세스 스레드에 요청 중이거나 동작중인지 체크 하여 있으면 검색이면 중지후 요청하고 나머지는 실패 처리한다.
#ifdef _LINUX
    if(sem_wait(&m_pModuleProcSem) != 0)
#else
    if(SDL_SemWait(m_pModuleProcSem) == SDL_MUTEX_TIMEDOUT)
#endif
    {
        if(m_sModuleInputProcData.eProcType != E_NDDS_PROCTYPE_NONNDDS)
        {
            pResult->nResult = N_NONNDDSERR_DUPLICATION;
            return false;
        }

        //DBGPRINT(_T("############################# Search #1 - Stop\n"));
        delay(1); //스레드 스위칭(기존 스레드 처리가 끝날 수 있게 스위칭)
    }

    //DBGPRINT(_T("MUTEX[%d] - API_NDDS_Search::LockMutex #1\n"), GetTickCount());
#ifdef _LINUX
    pthread_mutex_lock(&m_pModuleProcMutex);
#else
    SDL_LockMutex(m_pModuleProcMutex);
#endif

    int nSessionID = ++m_nLastIdxSession;

    SendMessgeCallback(NM_NDDS_NONNDDS_INIT, nSessionID);
    //DBGPRINT(_T("MUTEX[%d] - API_NDDS_Search::LockMutex #2\n"), GetTickCount());
    //DBGPRINT(_T("############################# Search #2\n"));

    //////////////////////////////////////
    //Search Input 데이터를 설정한다.
    m_sModuleInputProcData.eProcType = E_NDDS_PROCTYPE_NONNDDS;
    m_sModuleInputProcData.nNonNddsSessionID = nSessionID;
    m_sModuleInputProcData.bNonNddsSync = pRequest->bSync;
    strcpy(m_sModuleInputProcData.sNonNddsData.szUrl, pRequest->szUrl);
    strcpy(m_sModuleInputProcData.sNonNddsData.szDomain, pRequest->szDomain);
    m_sModuleOutputProcData.sNonNddsData.pszResult = pResult->pszResult;

    ////////////////////////////////
    // 스레드를 동작시킨다.
#ifdef _LINUX
    pthread_cond_signal(&m_pModuleProcCond);
    pthread_mutex_unlock(&m_pModuleProcMutex);
#else
    SDL_CondSignal(m_pModuleProcCond);
    SDL_UnlockMutex(m_pModuleProcMutex);
#endif

    delay(15);	//스레드 스위칭

    if(pRequest->bSync)
    {
        // 스레드 처리가 끝날때 까지 대기한다.
#ifdef _LINUX
        pthread_mutex_lock(&m_pModuleProcMutex);
        pthread_mutex_unlock(&m_pModuleProcMutex);
#else
        SDL_LockMutex(m_pModuleProcMutex);
        SDL_UnlockMutex(m_pModuleProcMutex);
#endif
        pResult->nResult = m_sModuleOutputProcData.sNonNddsData.nResult;
        strcpy(pResult->szErrorMsg, m_sModuleOutputProcData.sNonNddsData.szErrorMsg);
        pResult->bResult = m_sModuleOutputProcData.sNonNddsData.bResult;
    }

    //DBGPRINT(_T("############################# Search #true\n"));

    pResult->nSessionID = nSessionID;

    return pResult->bResult;
}

int CNddsModule::GetNddsAuthforSmartCarResult(int nSessionID, int &nValidateCode, int &nResultCode, char *szAccessKey, int *nErrorCode)
{
	eNddsErrorCode eErrCode = NDDSERR_UNKNOWN;
	char szErrorMsg[256] = {0,};

	NDDS_COMMON_RES *pRes = NULL;
	NDDSTMAP_AUTHFORSMARTCAR_RES *pResStruct = NULL;
	eNddsProtocolType eResProtocolType = NDDSTYPE_NONE;
	NDDSLIB_GetResultProtocol(nSessionID, false, &eResProtocolType, &pRes, &eErrCode);
	pResStruct = (NDDSTMAP_AUTHFORSMARTCAR_RES*)pRes;

    *nErrorCode     = (int) eErrCode;

	if(pResStruct != NULL)
	{
		IsChangeAccesskey(nSessionID, pResStruct->header.accessKey);
		CheckErrorCode(nSessionID, pResStruct->header.errorCode);
	}

	if(!NddsErrorCheck(eErrCode, pResStruct?pResStruct->header.errorMessage:NULL, szErrorMsg))
	{
        NDDSLIB_ReleaseObject(pRes, eErrCode);
		return 0;
	}

	if (pResStruct == NULL)
	{
		*nErrorCode = NDDSERR_RESULT_NULL;
		return false;
	}

	if(eErrCode == NDDSERR_SUCCESS)
	{
		if(pResStruct->header.accessKey)
		{
			strcpy(m_pNddsAuthTmapForSmartCar.m_AccessKey, pResStruct->header.accessKey);
			strcpy(szAccessKey, pResStruct->header.accessKey);
		}

		if(pResStruct->header.errorCode)
		{
			strcpy(m_pNddsAuthTmapForSmartCar.m_ErrorCode, pResStruct->header.errorCode);
		}

		if(pResStruct->commonInfo->euk)
			strcpy(m_pNddsAuthTmapForSmartCar.m_CommonInfo.m_Euk,pResStruct->commonInfo->euk);
		if(pResStruct->commonInfo->eid)
			strcpy(m_pNddsAuthTmapForSmartCar.m_CommonInfo.m_Eid,pResStruct->commonInfo->eid);

		m_pNddsAuthTmapForSmartCar.m_AuthInfo.m_ValidateCode = pResStruct->authInfo->validateCode;
		nValidateCode = m_pNddsAuthTmapForSmartCar.m_AuthInfo.m_ValidateCode;
		if(pResStruct->authInfo->validationSubField)
			strcpy(m_pNddsAuthTmapForSmartCar.m_AuthInfo.m_ValidationSubField,pResStruct->authInfo->validationSubField);
		//strcpy(pResult->sAuthInfo.szValidateGroup,m_pNddsAuthTmapForSmartCar.m_AuthInfo.m_ValidateGroup);
		if(pResStruct->authInfo->externalUserInfoCount > 6)
		{
			m_pNddsAuthTmapForSmartCar.m_AuthInfo.m_ExternalUserInfoCount = 6;
			ASSERT(0);
		}
		else
			m_pNddsAuthTmapForSmartCar.m_AuthInfo.m_ExternalUserInfoCount = pResStruct->authInfo->externalUserInfoCount;
		for(int i=0; i<m_pNddsAuthTmapForSmartCar.m_AuthInfo.m_ExternalUserInfoCount; i++)
		{
			if(pResStruct->authInfo->externalUserInfos[i].provider)
				strcpy(m_pNddsAuthTmapForSmartCar.m_AuthInfo.m_ExternalUserInfos[i].m_Provider, pResStruct->authInfo->externalUserInfos[i].provider);
			if(pResStruct->authInfo->externalUserInfos[i].readableUserId)
				strcpy(m_pNddsAuthTmapForSmartCar.m_AuthInfo.m_ExternalUserInfos[i].m_ReadableUserId, pResStruct->authInfo->externalUserInfos[i].readableUserId);
		}

		m_pNddsAuthTmapForSmartCar.m_HeadUnitDeviceAuthInfo.m_ResultCode = pResStruct->headUnitDeviceAuthInfo->resultCode;
		nResultCode = m_pNddsAuthTmapForSmartCar.m_HeadUnitDeviceAuthInfo.m_ResultCode;
		m_pNddsAuthTmapForSmartCar.m_HeadUnitDeviceAuthInfo.m_ResultSubField = pResStruct->headUnitDeviceAuthInfo->resultSubField;

		m_pNddsAuthTmapForSmartCar.m_TidAuthResInfo.m_ResultCode = pResStruct->tidAuthResInfo->resultCode;
		m_pNddsAuthTmapForSmartCar.m_TidAuthResInfo.m_ResultSubField = pResStruct->tidAuthResInfo->resultSubField;
		m_pNddsAuthTmapForSmartCar.m_TidAuthResInfo.m_ResultMdnAuthInfo = pResStruct->tidAuthResInfo->resultMdnAuthInfo;
		if(pResStruct->tidAuthResInfo->externalUserInfoCount > 6)
		{
			m_pNddsAuthTmapForSmartCar.m_TidAuthResInfo.m_ExternalUserInfoCount = 6;
			ASSERT(0);
		}
		else
			m_pNddsAuthTmapForSmartCar.m_TidAuthResInfo.m_ExternalUserInfoCount = pResStruct->tidAuthResInfo->externalUserInfoCount;
		for(int i=0; i<m_pNddsAuthTmapForSmartCar.m_TidAuthResInfo.m_ExternalUserInfoCount; i++)
		{
			if(pResStruct->tidAuthResInfo->externalUserInfos[i].provider)
				strcpy(m_pNddsAuthTmapForSmartCar.m_TidAuthResInfo.m_ExternalUserInfos[i].m_Provider, pResStruct->tidAuthResInfo->externalUserInfos[i].provider);
			if(pResStruct->tidAuthResInfo->externalUserInfos[i].readableUserId)
				strcpy(m_pNddsAuthTmapForSmartCar.m_TidAuthResInfo.m_ExternalUserInfos[i].m_ReadableUserId, pResStruct->tidAuthResInfo->externalUserInfos[i].readableUserId);
		}

		if(pResStruct->versionInfo->menuNoticeVer)
			strcpy(m_pNddsAuthTmapForSmartCar.m_VersionInfo.m_MenuNoticeVer,pResStruct->versionInfo->menuNoticeVer);
		if(pResStruct->versionInfo->openappVer)
			strcpy(m_pNddsAuthTmapForSmartCar.m_VersionInfo.m_OpenappVer,pResStruct->versionInfo->openappVer);

		if(pResStruct->appUpdateInfo->appUpdateType)
			strcpy(m_pNddsAuthTmapForSmartCar.m_AppUpdateInfo.m_AppUpdateType,pResStruct->appUpdateInfo->appUpdateType);
		if(pResStruct->appUpdateInfo->appVer)
			strcpy(m_pNddsAuthTmapForSmartCar.m_AppUpdateInfo.m_AppVer,pResStruct->appUpdateInfo->appVer);
		if(pResStruct->appUpdateInfo->appUpdateViewFlag)
			strcpy(m_pNddsAuthTmapForSmartCar.m_AppUpdateInfo.m_AppUpdateViewFlag,pResStruct->appUpdateInfo->appUpdateViewFlag);
		if(pResStruct->appUpdateInfo->appUpdateUrl)
			strcpy(m_pNddsAuthTmapForSmartCar.m_AppUpdateInfo.m_AppUpdateUrl,pResStruct->appUpdateInfo->appUpdateUrl);
		if(pResStruct->appUpdateInfo->appUpdateMsg)
			strcpy(m_pNddsAuthTmapForSmartCar.m_AppUpdateInfo.m_AppUpdateMsg,pResStruct->appUpdateInfo->appUpdateMsg);

		if(pResStruct->appControlInfo->controlInfoCount > 6)
		{
			m_pNddsAuthTmapForSmartCar.m_AppControlInfo.m_ControlInfoCount = 6;
			ASSERT(0);
		}
		else
			m_pNddsAuthTmapForSmartCar.m_AppControlInfo.m_ControlInfoCount = pResStruct->appControlInfo->controlInfoCount;
		for(int i=0; i<m_pNddsAuthTmapForSmartCar.m_AppControlInfo.m_ControlInfoCount; i++)
		{
			if(pResStruct->appControlInfo->controlInfoDetails[i].serviceName)
				strcpy(m_pNddsAuthTmapForSmartCar.m_AppControlInfo.m_ControlInfoDetails[i].m_ServiceName, pResStruct->appControlInfo->controlInfoDetails[i].serviceName);
			if(pResStruct->appControlInfo->controlInfoDetails[i].serviceYn)
				strcpy(m_pNddsAuthTmapForSmartCar.m_AppControlInfo.m_ControlInfoDetails[i].m_ServiceYn, pResStruct->appControlInfo->controlInfoDetails[i].serviceYn);
			if(pResStruct->appControlInfo->controlInfoDetails[i].serviceItems)
				strcpy(m_pNddsAuthTmapForSmartCar.m_AppControlInfo.m_ControlInfoDetails[i].m_ServiceItems, pResStruct->appControlInfo->controlInfoDetails[i].serviceItems);
		}

		if(pResStruct->pushInfo->pushSetYn)
			strcpy(m_pNddsAuthTmapForSmartCar.m_PushInfo.m_PushSetYn, pResStruct->pushInfo->pushSetYn);
		if(pResStruct->pushInfo->smsYn)
			strcpy(m_pNddsAuthTmapForSmartCar.m_PushInfo.m_SmsYn, pResStruct->pushInfo->smsYn);
		if(pResStruct->pushInfo->momentYn)
			strcpy(m_pNddsAuthTmapForSmartCar.m_PushInfo.m_MomentYn, pResStruct->pushInfo->momentYn);

		if(pResStruct->userProfileInfo->personalInfo->email)
			strcpy(m_pNddsAuthTmapForSmartCar.m_UserProfileInfo.m_PersonalInfo.m_Email, pResStruct->userProfileInfo->personalInfo->email);
		if(pResStruct->userProfileInfo->personalInfo->userName)
			strcpy(m_pNddsAuthTmapForSmartCar.m_UserProfileInfo.m_PersonalInfo.m_UserName, pResStruct->userProfileInfo->personalInfo->userName);
		if(pResStruct->userProfileInfo->personalInfo->mdn)
			strcpy(m_pNddsAuthTmapForSmartCar.m_UserProfileInfo.m_PersonalInfo.m_Mdn, pResStruct->userProfileInfo->personalInfo->mdn);
		if(pResStruct->userProfileInfo->personalInfo->email)
			strcpy(m_pNddsAuthTmapForSmartCar.m_UserProfileInfo.m_PersonalInfo.m_Birth, pResStruct->userProfileInfo->personalInfo->email);
		if(pResStruct->userProfileInfo->carInfo->model)
			strcpy(m_pNddsAuthTmapForSmartCar.m_UserProfileInfo.m_CarInfo.m_Model, pResStruct->userProfileInfo->carInfo->model);
		if(pResStruct->userProfileInfo->carInfo->fuel)
			strcpy(m_pNddsAuthTmapForSmartCar.m_UserProfileInfo.m_CarInfo.m_Fuel, pResStruct->userProfileInfo->carInfo->fuel);
		if(pResStruct->userProfileInfo->carInfo->hipassYn)
			strcpy(m_pNddsAuthTmapForSmartCar.m_UserProfileInfo.m_CarInfo.m_HipassYn, pResStruct->userProfileInfo->carInfo->hipassYn);
		if(pResStruct->userProfileInfo->carInfo->number)
			strcpy(m_pNddsAuthTmapForSmartCar.m_UserProfileInfo.m_CarInfo.m_Number, pResStruct->userProfileInfo->carInfo->number);

		if(m_pNddsAuthTmapForSmartCar.m_AuthInfo.m_ValidateCode == 2000 && m_pNddsAuthTmapForSmartCar.m_HeadUnitDeviceAuthInfo.m_ResultCode == 2000)
			m_nAuthCnt = 1;
	}

    NDDSLIB_ReleaseObject(pRes);

	return m_nAuthCnt;
}

int CNddsModule::NonNddsRequest(int nSessionID, eNddsRequestType eRequestType, char *pszUrl, char *pszDomain, char *pszResult, char *szErrorMsg, bool &bResult)
{
    eNddsErrorCode eErrCode = NDDSERR_UNKNOWN;

    m_eLastSearchType = N_SEARCHTYPE_NONE;
    m_nReadCnt = 0;
    *szErrorMsg = NULL;
    bResult = true;

    NDDSTMAP_NONNDDS_REQ sRequest;
    memset(&sRequest, 0x00, sizeof(NDDSTMAP_NONNDDS_REQ));
    sRequest.pszUrl = pszUrl;
    sRequest.pszDomain = pszDomain;
    sRequest.ppszResult = &pszResult;

    if(!NDDSLIB_RequestProtocol(NDDSTYPE_TMAP_NONNDDS, eRequestType, (NDDS_COMMON_REQ*)(&sRequest), nSessionID, &eErrCode))
    {
        if(eErrCode == NDDSERR_USERCANCEL) {
            strcpy(szErrorMsg, "사용자 취소");
        } else {
            strcpy(szErrorMsg, "데이터 생성 실패");
        }

        bResult = false;
        return 0;
    }

    if(eRequestType == NDDSREQUEST_ATONCE)
    {
        return 1;
    }

    eNddsStatus eStatus = NDDSSTATUS_WAIT;
    int nProgress = 0;
    DBGPRINT(_T("Wait "));
    unsigned long reqTime = GetTickCount();
    while(eStatus != NDDSSTATUS_COMPLETE && eStatus != NDDSSTATUS_CANCELED)
    {
        unsigned long thisTime = GetTickCount();
        unsigned long wattingTime = thisTime - reqTime;

        if(wattingTime > SEARCH_WAITTING_TIME)
        {
            strcpy(szErrorMsg, "서버 응답 없음");
            eErrCode = NDDSERR_TIMEOUT;
            bResult = false;
            return 0;
        }

        DBGPRINT(_T("."));
        delay(100);
        NDDSLIB_GetStatusProtocol(nSessionID, &eStatus, &nProgress, &eErrCode);
    }
    DBGPRINT(_T("\n"));

    int nNddsErrorCode = -1;

    return GetNonNddsResult(nSessionID, pszResult, bResult);
}

int CNddsModule::GetNonNddsResult(int nSessionID, char *pszResult, bool &bResult)
{
    eNddsErrorCode eErrCode = NDDSERR_UNKNOWN;

	NDDS_COMMON_RES *pRes = NULL;
    NDDSTMAP_NONNDDS_RES *pResStruct = NULL;
    eNddsProtocolType eResProtocolType = NDDSTYPE_TMAP_NONNDDS;
    NDDSLIB_GetResultProtocol(nSessionID, false, &eResProtocolType, &pRes, &eErrCode);
    pResStruct = (NDDSTMAP_NONNDDS_RES*)pRes;

    if (pResStruct == NULL)
    {
		NddsExternalLog(E_DEBUG, "nddsmodule :: in GetNonNddsResult 18-0-1 :: pResStruct is NULL :: eErrCode = %d", eErrCode);
        eErrCode = NDDSERR_RESULT_NULL;
        bResult = false;
        NddsExternalLog(E_DEBUG, "nddsmodule :: in GetNonNddsResult 18-0-2 :: pResStruct is NULL :: eErrCode = %d", eErrCode);
        return 0;
    }

    if(eErrCode == NDDSERR_SUCCESS)
    {
//        if(pResStruct->pszResult)
//            strcpy(pszResult, pResStruct->pszResult);
        if (pResStruct->pszResult) {
            memcpy(pszResult, pResStruct->pszResult, pResStruct->nResult);
        }
    }
    else
    {
        bResult = false;
    }

    NDDSLIB_ReleaseObject(pRes);

    return pResStruct->nResult;
}
