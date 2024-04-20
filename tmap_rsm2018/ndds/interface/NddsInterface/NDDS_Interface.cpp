#include "nddsmodule_inc.h"
#include "NDDS_Interface.h"
#include "ndds_inc.h"
#include "common_def.h"
#include "version.h"
#include "debug.h"

#include <stdio.h>
#include <string.h>
#ifdef _WIN32
#include "SDL.h"
#include "time.h"
#else
#include <pthread.h>
#endif

#ifdef __MM2018__
#include <dlt/dlt.h>
DLT_DECLARE_CONTEXT(con_ndds);
#endif

LPCALL_NDDS NddsEngineCallback = NULL;
NDDSMODULE_INIT g_sNddsInit = {0,};	// NDDS 초기화 변수(스레드에서 사용)
DeErrorCode g_eNddsErrorInit = DE_ERROR_FAIL;	// NDDS 초기화 에러 코드(스레드에서 설정)

bool g_bNddsIsInitialized = false;

// 경로탐색 옵션 변경(UI->Engine)
unsigned int ChangeNddsRouteOption(int nRouteOption);

// NDDS 초기화 스레드
#ifdef _LINUX
static void* NddsInitProc(void *lParam);
#else
static int NddsInitProc(void *lParam);
#endif

// NDDS 콜백
int NddsLibCallback(int nMsg, int nSessionID, long lParam1, long lParam2, void* lpParam);

bool IsNddsInitialized()
{
	return g_bNddsIsInitialized;
}

#define NIF_NDDS_NOT_INITIALIZED_RETURN_FAIL if (!IsNddsInitialized()) \
							{\
								return DE_ERROR_NOT_INITIALIZED;\
							}

#define NIF_NDDS_NOT_INITIALIZED_RETURN_FAIL_NOLOG if (!IsNddsInitialized()) \
							{\
								return DE_ERROR_NOT_INITIALIZED;\
							}

NDDS_API DeErrorCode NDDS_Initialize(bool bSync, LPCALL_NDDS pCallback, DS_NDDSINIT_EX *pInitEx)
{
#ifdef __MM2018__
	DLT_REGISTER_CONTEXT(con_ndds,"CON","NDDS DLT LOG");
#endif

#ifdef _LINUX
	pthread_t pNddsInitProcThread;
#else
	SDL_Thread *pNddsInitProcThread = NULL;
#endif

	NddsEngineCallback = pCallback;

	if(pInitEx == NULL || *pInitEx->szAuthCode == NULL || pInitEx->eOSType == DE_OSTYPE_NODE ||
		*pInitEx->szModelNo == NULL || *pInitEx->szOSVersion == NULL ||
		*pInitEx->szAppVersion == NULL || *pInitEx->szAppBuildNo == NULL)
	{
		return DE_ERROR_INVALID_PARAM;
	}

#ifdef _WIN32
	if (SDL_Init(SDL_INIT_TIMER) < 0)
	{
		return DE_ERROR_INIT_SDL;
	}
#endif

	switch(pInitEx->eCharSet)
	{
	case DE_CHARACTERSET_UTF8 :			g_sNddsInit.eNdds_CharSet = eNDDS_CHARACTERSET_UTF8; break;
	case DE_CHARACTERSET_EUCKR:			g_sNddsInit.eNdds_CharSet = eNDDS_CHARACTERSET_EUCKR; break;
	default:
		g_sNddsInit.eNdds_CharSet = eNDDS_CHARACTERSET_UTF8;
		break;
	}

	g_sNddsInit.bServerCommerce = pInitEx->bServerCommerce;
    strcpy(g_sNddsInit.szServerURL, pInitEx->szServerURL);
    g_sNddsInit.nServerPort = pInitEx->nServerPort;
    //if(pInitEx->szServerURL[0] == NULL || pInitEx->nServerPort == 0)
    //    return DE_ERROR_SERVER_INFO;
	strcpy(g_sNddsInit.szMacAddress, pInitEx->szAuthCode);
	strcpy(g_sNddsInit.szModelName, pInitEx->szModelNo);
	strcpy(g_sNddsInit.szAppVersion, pInitEx->szAppVersion);
	strcpy(g_sNddsInit.szAppBuildNo, pInitEx->szAppBuildNo);
	switch(pInitEx->eOSType)
	{
	case DE_OSTYPE_ANDROID:		g_sNddsInit.eNdds_OsType = eNdds_OsType_Android; break;
	case DE_OSTYPE_IOS:			g_sNddsInit.eNdds_OsType = eNdds_OsType_IOS; break;
	case DE_OSTYPE_WINDOWS:		g_sNddsInit.eNdds_OsType = eNdds_OsType_Windows; break;
	case DE_OSTYPE_WINCE:		g_sNddsInit.eNdds_OsType = eNdds_OsType_WinCE; break;
	case DE_OSTYPE_WINMOBILE:	g_sNddsInit.eNdds_OsType = eNdds_OsType_WinMobile; break;
	case DE_OSTYPE_QNX:			g_sNddsInit.eNdds_OsType = eNdds_OsType_Qnx; break;
	case DE_OSTYPE_LINUX:		g_sNddsInit.eNdds_OsType = eNdds_OsType_Linux; break;
	case DE_OSTYPE_ETC:
	default:
		g_sNddsInit.eNdds_OsType = eNdds_OsType_Etc;
		break;
	}
	strcpy(g_sNddsInit.szOSVersion, pInitEx->szOSVersion);
	strcpy(g_sNddsInit.szDeviceID, pInitEx->szDeviceID);

	strcpy(g_sNddsInit.szDeviceIDHu, pInitEx->szDeviceIDHu);
	strcpy(g_sNddsInit.szDeviceModleName, pInitEx->szDeviceModleName);
	strcpy(g_sNddsInit.szDeviceModelNo, pInitEx->szDeviceModelNo);
	strcpy(g_sNddsInit.szVendor, pInitEx->szVendor);
	strcpy(g_sNddsInit.szSerialKey, pInitEx->szSerialKey);
	strcpy(g_sNddsInit.szAccessKey, pInitEx->szAccessKey);

	g_sNddsInit.pfnNddsUpdate = NddsLibCallback;

	// NDDS 초기화
#ifdef _LINUX
	int thr_id;
	thr_id = pthread_create(&pNddsInitProcThread, NULL, &NddsInitProc, NULL);
	pthread_setname_np(pNddsInitProcThread, "skt.tmap.ndds.init");
	if(bSync)
	{
        void* status = 0;
		pthread_join(pNddsInitProcThread, (void **)&status);
		return g_eNddsErrorInit;
	}
#else
	pNddsInitProcThread = SDL_CreateThread(NddsInitProc, "NddsInitProc", NULL);
	if(bSync)
	{
		SDL_WaitThread(pNddsInitProcThread, NULL);
		return g_eNddsErrorInit;
	}
#endif

	return DE_ERROR_SUCCESS;
}

NDDS_API void NDDS_Uninitialize()
{
	NIF_Uninitialize();

#ifdef _WIN32
	SDL_Quit();
#endif

#ifdef __MM2018__
	DLT_UNREGISTER_CONTEXT(con_ndds);
	DLT_UNREGISTER_APP();
#endif

	g_bNddsIsInitialized = false;
}

NDDS_API char* NDDS_GetVersion()
{
	return NDDS_VERSION;
}

NDDS_API DeErrorCode NDDS_SetTmapHost(const char *pszHostName, const unsigned int nPort)
{
    bool result = NIF_SetTmapHost(pszHostName, nPort);

    return result ? DE_ERROR_SUCCESS : DE_ERROR_FAIL;
}

//////////////////////////////////////////////////////////////////////////
/// 경로 탐색
//////////////////////////////////////////////////////////////////////////

NDDS_API DeErrorCode NDDS_Route(DS_ROUTE_REQ *pRouteReq, bool bSync, int *nSessionID, int *nRouteCnt)
{
	NIF_NDDS_NOT_INITIALIZED_RETURN_FAIL;

	bool bRet;
	int i, j, idx, nRoute = 0;
	API_ROUTE_REQ sRequest;
	API_ROUTE_RES sResult;
	memset(&sRequest, 0, sizeof(API_ROUTE_REQ));
	memset(&sResult, 0, sizeof(API_ROUTE_RES));

	sRequest.bSync = bSync;

	switch(pRouteReq->eRouteType)
	{
	case DE_ROUTETYPE_ADD:				sRequest.sRouteInput.eRouteType = N_ROUTETYPE_ADD; break;
	case DE_ROUTETYPE_RE_OPTION:			sRequest.sRouteInput.eRouteType = N_ROUTETYPE_RE_OPTION; break;
	case DE_ROUTETYPE_RE_OUTROUTE:		sRequest.sRouteInput.eRouteType = N_ROUTETYPE_RE_OUTROUTE; break;
	case DE_ROUTETYPE_TPEG_RE_ROUTE:		sRequest.sRouteInput.eRouteType = N_ROUTETYPE_TPEG_RE_ROUTE; break;
	case DE_ROUTETYPE_PATTERN_ARRIVAL:	sRequest.sRouteInput.eRouteType = N_ROUTETYPE_PATTERN_ARRIVAL; break;
	case DE_ROUTETYPE_PATTERN_DEPARTURE:	sRequest.sRouteInput.eRouteType = N_ROUTETYPE_PATTERN_DEPARTURE; break;
	case DE_ROUTETYPE_NORMAL:			sRequest.sRouteInput.eRouteType = N_ROUTETYPE_NORMAL; break;
	case DE_ROUTETYPE_SUMMARY:			sRequest.sRouteInput.eRouteType = N_ROUTETYPE_SUMMARY; break;
	default:
		sRequest.sRouteInput.eRouteType = N_ROUTETYPE_NORMAL; break;
	}
	
	switch(pRouteReq->eRouteType)
	{
	case DE_ROUTETYPE_ADD:
	case DE_ROUTETYPE_RE_OPTION:
	case DE_ROUTETYPE_RE_OUTROUTE:
	case DE_ROUTETYPE_TPEG_RE_ROUTE:
	case DE_ROUTETYPE_PATTERN_ARRIVAL:
	case DE_ROUTETYPE_PATTERN_DEPARTURE:
	case DE_ROUTETYPE_NORMAL:
		{
			sRequest.sRouteInput.sRouteInput.eRouteType = sRequest.sRouteInput.eRouteType;

			strcpy(sRequest.sRouteInput.sRouteInput.m_Tvas, pRouteReq->sRouteServerReq.szTvas);

            if(pRouteReq->sRouteServerReq.szGuideImageCode[0] != 0x00)
                strcpy(sRequest.sRouteInput.sRouteInput.m_GuideImageCode, pRouteReq->sRouteServerReq.szGuideImageCode);

			unsigned int uMultiOption = ChangeNddsRouteOption(pRouteReq->sRouteServerReq.nRouteOption);
			if(uMultiOption != 0)
			{
				nRoute++;
				sRequest.sRouteInput.sRouteInput.m_uMultiOption |= uMultiOption;
			}else
			{
				return DE_ERROR_ROUTE_MULTIOPTION;
			}

			if(pRouteReq->eRouteType == DE_ROUTETYPE_PATTERN_ARRIVAL || pRouteReq->eRouteType == DE_ROUTETYPE_PATTERN_DEPARTURE)
			{
				sRequest.sRouteInput.sRouteInput.m_uMultiOption |= N_MULTIROUTE_TRAFFIC_PATTERN;		// 패턴 적용
				sRequest.sRouteInput.sRouteInput.m_nEstRouteFlag = 1;
				if(pRouteReq->eRouteType == DE_ROUTETYPE_PATTERN_ARRIVAL)
				{
					sRequest.sRouteInput.sRouteInput.m_nEstRouteType = 1;
				}else
				{
					sRequest.sRouteInput.sRouteInput.m_nEstRouteType = 2;
				}
			}

			sRequest.sRouteInput.sRouteInput.m_nCoodType = 1;
			sRequest.sRouteInput.sRouteInput.m_bStGpsPosition = pRouteReq->sRouteServerReq.bStartGpsPos;
			//if(pRouteReq->sRouteServerReq.bStartGpsPos)
			//	sRequest.sRouteInput.sRouteInput.m_nDirPriority = 1;

			sRequest.sRouteInput.sRouteInput.m_nDirPriority = pRouteReq->sRouteServerReq.byDepartDirPriority;

			DBGPRINT(_T("pRouteReq->sRouteServerReq.bStartGpsPos=%d\n"), pRouteReq->sRouteServerReq.bStartGpsPos);

			if(pRouteReq->sRouteServerReq.sDataTime.nYear > 0)
			{
				sRequest.sRouteInput.sRouteInput.m_ReqTime.Year = pRouteReq->sRouteServerReq.sDataTime.nYear;
				sRequest.sRouteInput.sRouteInput.m_ReqTime.Month = pRouteReq->sRouteServerReq.sDataTime.nMonth;
				sRequest.sRouteInput.sRouteInput.m_ReqTime.Day = pRouteReq->sRouteServerReq.sDataTime.nDay;
				sRequest.sRouteInput.sRouteInput.m_ReqTime.Hour = pRouteReq->sRouteServerReq.sDataTime.nHour;
				sRequest.sRouteInput.sRouteInput.m_ReqTime.Minute = pRouteReq->sRouteServerReq.sDataTime.nMinute;
				sRequest.sRouteInput.sRouteInput.m_ReqTime.Second = pRouteReq->sRouteServerReq.sDataTime.nSecond;
				sRequest.sRouteInput.sRouteInput.m_ReqTime.Week = pRouteReq->sRouteServerReq.sDataTime.nWeek;
			}else
			{
				time_t  ltime;
				struct tm *today;
				time(&ltime);
#ifdef __MM2018__
				ltime += 9 * 60 * 60;       // Bosch System Firmware에서 localtime을 UTC로 링크해놨기 때문에, 우리가 알아서 서울 타임존을 적용해야 함. 2019.12.1 by hheo
#endif
				today = localtime(&ltime);

				sRequest.sRouteInput.sRouteInput.m_ReqTime.Year = today->tm_year + 1900;
				sRequest.sRouteInput.sRouteInput.m_ReqTime.Month = today->tm_mon + 1;
				sRequest.sRouteInput.sRouteInput.m_ReqTime.Day = today->tm_mday;
				sRequest.sRouteInput.sRouteInput.m_ReqTime.Hour = today->tm_hour;
				sRequest.sRouteInput.sRouteInput.m_ReqTime.Minute = today->tm_min;
				sRequest.sRouteInput.sRouteInput.m_ReqTime.Second = today->tm_sec;
				sRequest.sRouteInput.sRouteInput.m_ReqTime.Week = today->tm_wday;
			}
			
			sRequest.sRouteInput.sRouteInput.m_DepartSrchFlag = pRouteReq->sRouteServerReq.byDepartSrchFlag;
			sRequest.sRouteInput.sRouteInput.m_nStRoadType = pRouteReq->sRouteServerReq.eRoadType;
			sRequest.sRouteInput.sRouteInput.m_nAngle = pRouteReq->sRouteServerReq.nAngle;
			sRequest.sRouteInput.sRouteInput.m_nSpeed = pRouteReq->sRouteServerReq.nSpeed;

			if(pRouteReq->eRouteType == DE_ROUTETYPE_RE_OPTION || pRouteReq->eRouteType == DE_ROUTETYPE_RE_OUTROUTE || pRouteReq->eRouteType == DE_ROUTETYPE_TPEG_RE_ROUTE)
			{
				sRequest.sRouteInput.sRouteInput.m_bReRoute = true;

				// 재탐색 정보 설정
                if((pRouteReq->sRouteServerReq.sReRouteInfo.pszRouteData) && (pRouteReq->sRouteServerReq.sReRouteInfo.nRouteDataSize > 0))
                    sRequest.sRouteInput.sRouteInput.m_nsReRouteInfo.pszRouteData = pRouteReq->sRouteServerReq.sReRouteInfo.pszRouteData;
				if(pRouteReq->sRouteServerReq.sReRouteInfo.bAroundLink)
				{
					sRequest.sRouteInput.sRouteInput.m_nsReRouteInfo.bAroundLink = pRouteReq->sRouteServerReq.sReRouteInfo.bAroundLink;
                    sRequest.sRouteInput.sRouteInput.m_nsReRouteInfo.uRsdCode = pRouteReq->sRouteServerReq.sReRouteInfo.uRsdCode;
					sRequest.sRouteInput.sRouteInput.m_nsReRouteInfo.uOldRsdCode = pRouteReq->sRouteServerReq.sReRouteInfo.uOldRsdCode;
					sRequest.sRouteInput.sRouteInput.m_nsReRouteInfo.uLinkID = pRouteReq->sRouteServerReq.sReRouteInfo.uLinkID;
					sRequest.sRouteInput.sRouteInput.m_nsReRouteInfo.uTileCode = pRouteReq->sRouteServerReq.sReRouteInfo.uTileCode;
					sRequest.sRouteInput.sRouteInput.m_nsReRouteInfo.uDirInfo = pRouteReq->sRouteServerReq.sReRouteInfo.uDirInfo;
					sRequest.sRouteInput.sRouteInput.m_nsReRouteInfo.uPosX = pRouteReq->sRouteServerReq.sReRouteInfo.uPosX;
					sRequest.sRouteInput.sRouteInput.m_nsReRouteInfo.uPosY = pRouteReq->sRouteServerReq.sReRouteInfo.uPosY;
				}
				
				sRequest.sRouteInput.sRouteInput.m_nsReRouteInfo.uPassTollID = pRouteReq->sRouteServerReq.sReRouteInfo.uPassTollID;
				sRequest.sRouteInput.sRouteInput.m_nsReRouteInfo.nLastRid = pRouteReq->sRouteServerReq.sReRouteInfo.nLastRid;
				sRequest.sRouteInput.sRouteInput.m_nsReRouteInfo.nTcRid = pRouteReq->sRouteServerReq.sReRouteInfo.nTcRid;
				strcpy(sRequest.sRouteInput.sRouteInput.m_nsReRouteInfo.szPreMapVersion, pRouteReq->sRouteServerReq.sReRouteInfo.szPreMapVersion);
				sRequest.sRouteInput.sRouteInput.m_nsReRouteInfo.nPreRidsCnt = pRouteReq->sRouteServerReq.sReRouteInfo.nPreRidsCnt;
				sRequest.sRouteInput.sRouteInput.m_nsReRouteInfo.pnPreRids = pRouteReq->sRouteServerReq.sReRouteInfo.pnPreRids;
				sRequest.sRouteInput.sRouteInput.m_nsReRouteInfo.nPreSecsCnt = pRouteReq->sRouteServerReq.sReRouteInfo.nPreSecsCnt;
				sRequest.sRouteInput.sRouteInput.m_nsReRouteInfo.pnPreSecs = pRouteReq->sRouteServerReq.sReRouteInfo.pnPreSecs;
				strcpy(sRequest.sRouteInput.sRouteInput.m_nsReRouteInfo.szPreTollgateIds, pRouteReq->sRouteServerReq.sReRouteInfo.szPreTollgateIds);
				sRequest.sRouteInput.sRouteInput.m_nsReRouteInfo.nInitSrchLength = pRouteReq->sRouteServerReq.sReRouteInfo.nInitSrchLength;
				strcpy(sRequest.sRouteInput.sRouteInput.m_nsReRouteInfo.szInitSrchSessionId, pRouteReq->sRouteServerReq.sReRouteInfo.szInitSrchSessionId);
            } else {
                // 일반탐색 시에도 필요한 정보(TG요금 계산)
                sRequest.sRouteInput.sRouteInput.m_nsReRouteInfo.uPassTollID = pRouteReq->sRouteServerReq.sReRouteInfo.uPassTollID;

                strcpy(sRequest.sRouteInput.sRouteInput.m_nsReRouteInfo.szPreTollgateIds, pRouteReq->sRouteServerReq.sReRouteInfo.szPreTollgateIds);
            }

            if(pRouteReq->sRouteServerReq.sPassPosInfo.szPastSessionId)
                strcpy(sRequest.sRouteInput.sRouteInput.m_nsPassPosInfo.szPastSessionId, pRouteReq->sRouteServerReq.sPassPosInfo.szPastSessionId);
            sRequest.sRouteInput.sRouteInput.m_nsPassPosInfo.nPastXpos = pRouteReq->sRouteServerReq.sPassPosInfo.nPastXpos;
            sRequest.sRouteInput.sRouteInput.m_nsPassPosInfo.nPastYpos = pRouteReq->sRouteServerReq.sPassPosInfo.nPastYpos;
            if(pRouteReq->sRouteServerReq.sPassPosInfo.szPastArrivalTime)
                strcpy(sRequest.sRouteInput.sRouteInput.m_nsPassPosInfo.szPastArrivalTime, pRouteReq->sRouteServerReq.sPassPosInfo.szPastArrivalTime);

			sRequest.sRouteInput.sRouteInput.m_nPosInfoCnt = pRouteReq->sRouteServerReq.nPosCnt;
			for(i=0 ; i<pRouteReq->sRouteServerReq.nPosCnt ; i++)
			{
				// 0:출발지, 1~3:경유지, 4:목적지
				if(i != pRouteReq->sRouteServerReq.nPosCnt-1)
					idx = i;
				else
					idx = 4;

				DBGPRINT(_T("Route #%d - (%d, %d) rpflag=%02x, poiid=%d, addrcode=%d, mppcnt=%d\n"), i, pRouteReq->sRouteServerReq.sPosInfo[i].tp.x, pRouteReq->sRouteServerReq.sPosInfo[i].tp.y, pRouteReq->sRouteServerReq.sPosInfo[i].byRPFlag, pRouteReq->sRouteServerReq.sPosInfo[i].nPoiID, pRouteReq->sRouteServerReq.sPosInfo[i].nAddrCode, pRouteReq->sRouteServerReq.sPosInfo[i].nMppCnt);

				sRequest.sRouteInput.sRouteInput.m_nsPosInfo[idx].m_bPos = true;
				strcpy(sRequest.sRouteInput.sRouteInput.m_nsPosInfo[idx].m_strName, pRouteReq->sRouteServerReq.sPosInfo[i].szName);

				sRequest.sRouteInput.sRouteInput.m_nsPosInfo[idx].m_wptX = pRouteReq->sRouteServerReq.sPosInfo[i].tp.x;
				sRequest.sRouteInput.sRouteInput.m_nsPosInfo[idx].m_wptY = pRouteReq->sRouteServerReq.sPosInfo[i].tp.y;
				sRequest.sRouteInput.sRouteInput.m_nsPosInfo[idx].m_nRpFlag = pRouteReq->sRouteServerReq.sPosInfo[i].byRPFlag;
				sRequest.sRouteInput.sRouteInput.m_nsPosInfo[idx].m_nPOIID = pRouteReq->sRouteServerReq.sPosInfo[i].nPoiID;
				sRequest.sRouteInput.sRouteInput.m_nsPosInfo[idx].m_nAddrCD = pRouteReq->sRouteServerReq.sPosInfo[i].nAddrCode;
				sRequest.sRouteInput.sRouteInput.m_nsPosInfo[idx].m_nMppCnt = pRouteReq->sRouteServerReq.sPosInfo[i].nMppCnt;

				for(j=0 ; j<pRouteReq->sRouteServerReq.sPosInfo[i].nMppCnt ; j++)
				{
					sRequest.sRouteInput.sRouteInput.m_nsPosInfo[idx].m_wpMpp[j].x = pRouteReq->sRouteServerReq.sPosInfo[i].tpMpp[j].x;
					sRequest.sRouteInput.sRouteInput.m_nsPosInfo[idx].m_wpMpp[j].y = pRouteReq->sRouteServerReq.sPosInfo[i].tpMpp[j].y;
				}
				if(idx == 4)
					strcpy(sRequest.sRouteInput.sRouteInput.m_nsPosInfo[idx].m_NavSeq, pRouteReq->sRouteServerReq.sPosInfo[i].szNavSeq);
			}

            sRequest.sRouteInput.sRouteInput.m_DestSrchFlag = pRouteReq->sRouteServerReq.byDestSrchFlag;
			// 출발지 GPS 궤적 정보 적제
			if(sRequest.sRouteInput.sRouteInput.m_bStGpsPosition && pRouteReq->sRouteServerReq.sGpsTraceData.nGpsTraceDataSize > 0)
			{
				sRequest.sRouteInput.sRouteInput.m_nsGpsTrace.nGpsTraceDataSize = pRouteReq->sRouteServerReq.sGpsTraceData.nGpsTraceDataSize;
                sRequest.sRouteInput.sRouteInput.m_nsGpsTrace.pGpasTraceData = pRouteReq->sRouteServerReq.sGpsTraceData.pGpsTraceData;
			}

			// 추가 단속카메라 정보
			memcpy(sRequest.sRouteInput.sRouteInput.m_bpAddCameraType, pRouteReq->sRouteServerReq.bpAddCameraType, sizeof(pRouteReq->sRouteServerReq.bpAddCameraType));
			memcpy(sRequest.sRouteInput.sRouteInput.m_bpAddDAType, pRouteReq->sRouteServerReq.bpAddDAType, sizeof(pRouteReq->sRouteServerReq.bpAddDAType));

			switch(pRouteReq->sRouteServerReq.eVehicleClass)
			{
			case DE_VEHICLECLASS_1:				sRequest.sRouteInput.sRouteInput.nVechileType = 0; break;
			case DE_VEHICLECLASS_3:				sRequest.sRouteInput.sRouteInput.nVechileType = 2; break;
			case DE_VEHICLECLASS_4:				sRequest.sRouteInput.sRouteInput.nVechileType = 3; break;
			case DE_VEHICLECLASS_5:				sRequest.sRouteInput.sRouteInput.nVechileType = 4; break;
			case DE_VEHICLECLASS_6:				sRequest.sRouteInput.sRouteInput.nVechileType = 5; break;
			case DE_VEHICLECLASS_2:
			default:
				sRequest.sRouteInput.sRouteInput.nVechileType = 1;
				break;
			}
			
			sRequest.sRouteInput.sRouteInput.m_bHipass = pRouteReq->sRouteServerReq.bHipass;

			switch(pRouteReq->sRouteServerReq.eOilType)
			{
			case DE_OILTYPE_DIESEL:				sRequest.sRouteInput.sRouteInput.nFuelType = 1; break;
			case DE_OILTYPE_LPG:					sRequest.sRouteInput.sRouteInput.nFuelType = 2; break;
			case DE_OILTYPE_CHAdeMO:					sRequest.sRouteInput.sRouteInput.nFuelType = 3; break;
			case DE_OILTYPE_AC3:					sRequest.sRouteInput.sRouteInput.nFuelType = 4; break;
			case DE_OILTYPE_DCCOMBO:					sRequest.sRouteInput.sRouteInput.nFuelType = 5; break;
			case DE_OILTYPE_PREMIUMGASOLINE:					sRequest.sRouteInput.sRouteInput.nFuelType = 6; break;
			case DE_OILTYPE_GASOLINE:
			default:
				sRequest.sRouteInput.sRouteInput.nFuelType = 0;
				break;
			}

			strcpy(sResult.sRouteOutput.szCollectPath[0], pRouteReq->sRouteServerReq.szCollectDataPath[0]);
			strcpy(sResult.sRouteOutput.szCollectPath[1], pRouteReq->sRouteServerReq.szCollectDataPath[1]);
			strcpy(sResult.sRouteOutput.szCollectPath[2], pRouteReq->sRouteServerReq.szCollectDataPath[2]);
		}
		break;
	case DE_ROUTETYPE_SUMMARY:
		{
			sRequest.sRouteInput.sRouteSummaryInput.m_nRouteSummaryType = pRouteReq->sRouteServerSummaryReq.eRouteSummaryType;
			sRequest.sRouteInput.sRouteSummaryInput.m_nRouteServiceType = pRouteReq->sRouteServerSummaryReq.eRouteServiceType;

			if(pRouteReq->sRouteServerSummaryReq.sDataTime.nYear > 0)
			{
				sRequest.sRouteInput.sRouteSummaryInput.m_ReqTime.Year = pRouteReq->sRouteServerSummaryReq.sDataTime.nYear;
				sRequest.sRouteInput.sRouteSummaryInput.m_ReqTime.Month = pRouteReq->sRouteServerSummaryReq.sDataTime.nMonth;
				sRequest.sRouteInput.sRouteSummaryInput.m_ReqTime.Day = pRouteReq->sRouteServerSummaryReq.sDataTime.nDay;
				sRequest.sRouteInput.sRouteSummaryInput.m_ReqTime.Hour = pRouteReq->sRouteServerSummaryReq.sDataTime.nHour;
				sRequest.sRouteInput.sRouteSummaryInput.m_ReqTime.Minute = pRouteReq->sRouteServerSummaryReq.sDataTime.nMinute;
				sRequest.sRouteInput.sRouteSummaryInput.m_ReqTime.Second = pRouteReq->sRouteServerSummaryReq.sDataTime.nSecond;
				sRequest.sRouteInput.sRouteSummaryInput.m_ReqTime.Week = pRouteReq->sRouteServerSummaryReq.sDataTime.nWeek;
			}else
			{
				time_t  ltime;
				struct tm *today;
				time(&ltime);
#ifdef __MM2018__
				ltime += 9 * 60 * 60;       // Bosch System Firmware에서 localtime을 UTC로 링크해놨기 때문에, 우리가 알아서 서울 타임존을 적용해야 함. 2019.12.1 by hheo
#endif
				today = localtime(&ltime);

				sRequest.sRouteInput.sRouteSummaryInput.m_ReqTime.Year = today->tm_year + 1900;
				sRequest.sRouteInput.sRouteSummaryInput.m_ReqTime.Month = today->tm_mon + 1;
				sRequest.sRouteInput.sRouteSummaryInput.m_ReqTime.Day = today->tm_mday;
				sRequest.sRouteInput.sRouteSummaryInput.m_ReqTime.Hour = today->tm_hour;
				sRequest.sRouteInput.sRouteSummaryInput.m_ReqTime.Minute = today->tm_min;
				sRequest.sRouteInput.sRouteSummaryInput.m_ReqTime.Second = today->tm_sec;
				sRequest.sRouteInput.sRouteSummaryInput.m_ReqTime.Week = today->tm_wday;
			}
			
			sRequest.sRouteInput.sRouteSummaryInput.m_nVertexFlag = pRouteReq->sRouteServerSummaryReq.nVertexFlag;

			sRequest.sRouteInput.sRouteSummaryInput.m_DepartDirPriority = pRouteReq->sRouteServerSummaryReq.byDepartDirPriority;
			sRequest.sRouteInput.sRouteSummaryInput.m_DepartSrchFlag = pRouteReq->sRouteServerSummaryReq.byDepartSrchFlag;
			sRequest.sRouteInput.sRouteSummaryInput.m_DestSrchFlag = pRouteReq->sRouteServerSummaryReq.byDestSrchFlag;
			sRequest.sRouteInput.sRouteSummaryInput.m_nAngle = pRouteReq->sRouteServerSummaryReq.nAngle;
			sRequest.sRouteInput.sRouteSummaryInput.m_nSpeed = pRouteReq->sRouteServerSummaryReq.nSpeed;

			sRequest.sRouteInput.sRouteSummaryInput.m_nPosInfoCnt = pRouteReq->sRouteServerSummaryReq.nPosCnt;
			for(i=0 ; i<pRouteReq->sRouteServerSummaryReq.nPosCnt ; i++)
			{
				// 0:출발지, 1~3:경유지, 4:목적지
				if(i != pRouteReq->sRouteServerSummaryReq.nPosCnt-1)
					idx = i;
				else
					idx = 4;

				DBGPRINT(_T("Route #%d - (%d, %d) rpflag=%02x, poiid=%d, addrcode=%d, mppcnt=%d\n"), i, pRouteReq->sRouteServerSummaryReq.sPosInfo[i].tp.x, pRouteReq->sRouteServerSummaryReq.sPosInfo[i].tp.y, pRouteReq->sRouteServerSummaryReq.sPosInfo[i].byRPFlag, pRouteReq->sRouteServerSummaryReq.sPosInfo[i].nPoiID, pRouteReq->sRouteServerSummaryReq.sPosInfo[i].nAddrCode, pRouteReq->sRouteServerSummaryReq.sPosInfo[i].nMppCnt);

				sRequest.sRouteInput.sRouteSummaryInput.m_nsPosInfo[idx].m_bPos = true;
				strcpy(sRequest.sRouteInput.sRouteSummaryInput.m_nsPosInfo[idx].m_strName, pRouteReq->sRouteServerSummaryReq.sPosInfo[i].szName);

				sRequest.sRouteInput.sRouteSummaryInput.m_nsPosInfo[idx].m_wptX = pRouteReq->sRouteServerSummaryReq.sPosInfo[i].tp.x;
				sRequest.sRouteInput.sRouteSummaryInput.m_nsPosInfo[idx].m_wptY = pRouteReq->sRouteServerSummaryReq.sPosInfo[i].tp.y;
				sRequest.sRouteInput.sRouteSummaryInput.m_nsPosInfo[idx].m_nRpFlag = pRouteReq->sRouteServerSummaryReq.sPosInfo[i].byRPFlag;
				sRequest.sRouteInput.sRouteSummaryInput.m_nsPosInfo[idx].m_nPOIID = pRouteReq->sRouteServerSummaryReq.sPosInfo[i].nPoiID;
				sRequest.sRouteInput.sRouteSummaryInput.m_nsPosInfo[idx].m_nAddrCD = pRouteReq->sRouteServerSummaryReq.sPosInfo[i].nAddrCode;
				sRequest.sRouteInput.sRouteSummaryInput.m_nsPosInfo[idx].m_nMppCnt = pRouteReq->sRouteServerSummaryReq.sPosInfo[i].nMppCnt;

				for(j=0 ; j<pRouteReq->sRouteServerSummaryReq.sPosInfo[i].nMppCnt ; j++)
				{
					sRequest.sRouteInput.sRouteSummaryInput.m_nsPosInfo[idx].m_wpMpp[j].x = pRouteReq->sRouteServerSummaryReq.sPosInfo[i].tpMpp[j].x;
					sRequest.sRouteInput.sRouteSummaryInput.m_nsPosInfo[idx].m_wpMpp[j].y = pRouteReq->sRouteServerSummaryReq.sPosInfo[i].tpMpp[j].y;
				}
			}
		}
		break;
	}
	

	// 경로탐색 요청
	bRet = NIF_Function(eAPI_NDDS_Route, &sRequest, &sResult);

	if(nSessionID)
		*nSessionID = sResult.nSessionID;

    if(nRouteCnt)
        *nRouteCnt = sResult.nRouteCnt;

	if(!bRet)
	{
		DeErrorCode eErrorCode = DE_ERROR_FAIL;
		switch(sResult.nRouteRet)
		{
		case N_ROUTEERR_SUCCED:
			eErrorCode = DE_ERROR_SUCCESS; break;
		case N_ROUTEERR_STARTPOS:
		case N_ROUTEERR_STARTPOS2:
		case N_ROUTEERR_GOALPOS:
		case N_ROUTEERR_COORDINATE_ERROR:
			eErrorCode = DE_ERROR_ROUTE_POSITION; break;
		case N_ROUTEERR_SAMEPOINT:
			eErrorCode = DE_ERROR_ROUTE_POSITIONSAME; break;
		case N_ROUTEERR_USERCANCEL:
			eErrorCode = DE_ERROR_ROUTE_USERCANCEL; break;
		case N_ROUTEERR_TIMEOUT:
			eErrorCode = DE_ERROR_ROUTE_TIMEOUT; break;
		case N_ROUTEERR_DISTOVER:
			eErrorCode = DE_ERROR_ROUTE_DISTOVER; break;
		case N_ROUTEERR_SERVER:
			eErrorCode = DE_ERROR_ROUTE_SERVER; break;
		case N_ROUTEERR_DUPLICATION:
			eErrorCode = DE_ERROR_ROUTE_DUPLICATION; break;
		case N_ROUTEERR_FAIL:
		default:
			eErrorCode = DE_ERROR_ROUTE_FAIL; break;
		}

		return eErrorCode;
	}

	return DE_ERROR_SUCCESS;
}

NDDS_API DeErrorCode NDDS_StopRoute(int nSessionID)
{
	NIF_NDDS_NOT_INITIALIZED_RETURN_FAIL;

	bool bRet;
	API_STOPROUTE_REQ sRequest;
	memset(&sRequest, 0x00, sizeof(API_STOPROUTE_REQ));

	sRequest.nSessionID = nSessionID;

	bRet = NIF_Function(eAPI_NDDS_StopRoute, &sRequest, NULL);

	if(!bRet)
		return DE_ERROR_FAIL;

	return DE_ERROR_SUCCESS;
}

NDDS_API DeErrorCode NDDS_GetWeatherInfo(DS_WEATHERINFO_RES *pResult)
{
	NIF_NDDS_NOT_INITIALIZED_RETURN_FAIL;

	if(pResult == NULL)
		return DE_ERROR_INVALID_PARAM;

	bool bRet;
	API_WEATHERINFO_RES sResult;
	memset(&sResult, 0x00, sizeof(API_WEATHERINFO_RES));

	bRet = NIF_Function(eAPI_NDDS_GetWeatherInfo, NULL, &sResult);

	if(!bRet)
		return DE_ERROR_FAIL;

	strcpy(pResult->szSkyCode, sResult.szSkyCode);
	strcpy(pResult->szSkyName, sResult.szSkyName);
	strcpy(pResult->szRainTypeCode, sResult.szRainTypeCode);
	strcpy(pResult->szRainSinceOnTime, sResult.szRainSinceOnTime);
	strcpy(pResult->szTempC1h, sResult.szTempC1h);

	return DE_ERROR_SUCCESS;
}

NDDS_API DeErrorCode NDDS_GetRouteSummaryInfo(int nSessionID, DS_ROUTESUMMARYINFO_RES *pResult)
{
	NIF_NDDS_NOT_INITIALIZED_RETURN_FAIL;

	if(pResult == NULL)
		return DE_ERROR_INVALID_PARAM;

	bool bRet;
	API_GET_ROUTESUMMARYINFO_REQ sRequest;
	API_GET_ROUTESUMMARYINFO_RES sResult;
	memset(&sRequest, 0x00, sizeof(API_GET_ROUTESUMMARYINFO_REQ));
	memset(&sResult, 0x00, sizeof(API_GET_ROUTESUMMARYINFO_RES));
    sRequest.nSessionID = nSessionID;

	bRet = NIF_Function(eAPI_NDDS_GetRouteSummaryInfo, &sRequest, &sResult);

	if(!bRet)
		return DE_ERROR_FAIL;

	pResult->nTotalTime = sResult.nTotalTime;
	pResult->nEstimationOption = sResult.nEstimationOption;
	strcpy(pResult->szEstimationTime, sResult.szEstimationTime);

	return DE_ERROR_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////
/// 검색 관련
//////////////////////////////////////////////////////////////////////////

NDDS_API DeErrorCode NDDS_Search(DS_SEARCH_REQ *pSearchReq, bool bSync, int &nResult, int *nSessionID)
{
	NIF_NDDS_NOT_INITIALIZED_RETURN_FAIL;

	if(pSearchReq == NULL)
		return DE_ERROR_INVALID_PARAM;

	bool bRet;
	API_SEARCH_REQ sRequest;
	API_SEARCH_RES sResult;
	memset(&sRequest, 0x00, sizeof(API_SEARCH_REQ));
	memset(&sResult, 0x00, sizeof(API_SEARCH_RES));

	sRequest.bSync = (bool)bSync;

	switch(pSearchReq->eSearchType)
	{
		case DE_SEARCHTYPE_SERVER:
			{
				DS_SEARCH_SERVER_REQ &sSearchReq = pSearchReq->sSearchServer;

				if(strlen(sSearchReq.szSearchName) > MAXLEN_POINAME)
				{
					return DE_ERROR_SEARCH_MAXNAME;
				}

				SEARCH_NDDSPOI_INPUT &sSearchInput = sRequest.sSearchInput.sSearchNddsPoi;
				sSearchInput.eSearchType = N_SEARCHTYPE_NDDSPOI;
				switch(sSearchReq.eSortType)
				{
				case DE_SERVER_SORTTYPE_DIST:				sSearchInput.eSortType = N_SORTTYPE_DIST; break;
				case DE_SERVER_SORTTYPE_ACCURACY:
				default:							sSearchInput.eSortType = N_SORTTYPE_ACCURACY; break;
				}
				sSearchInput.wpCurrent.x = sSearchReq.tpCurrent.x;
				sSearchInput.wpCurrent.y = sSearchReq.tpCurrent.y;
				strcpy(sSearchInput.szSearchName, sSearchReq.szSearchName);
				strcpy(sSearchInput.szAddrName, sSearchReq.szAddrName);
				sSearchInput.nDist = sSearchReq.nDist;
				switch(sSearchReq.eOilType)
				{
				case DE_OILTYPE_GASOLINE:	sSearchInput.eOilType = N_eOilTypeGasoline; break;
				case DE_OILTYPE_DIESEL:		sSearchInput.eOilType = N_eOilTypeDiesel; break;
				case DE_OILTYPE_LPG:			sSearchInput.eOilType = N_eOilTypeLPG; break;
				case DE_OILTYPE_DCCOMBO:			sSearchInput.eOilType = N_eOilTypeEV; break;
				default:					sSearchInput.eOilType = N_eOilTypeGasoline; break;
				}
				sSearchInput.bNewAddr = sSearchReq.bNewAddr;
			}
			break;
		case DE_SEARCHTYPE_SERVERAROUND:
			{
				DS_SEARCH_SERVERAROUND_REQ &sSearchReq = pSearchReq->sSearchServerAround;

				SEARCH_NDDSAROUNDPOI_INPUT &sSearchInput = sRequest.sSearchInput.sSearchNddsAroundPoi;
				sSearchInput.eSearchType = N_SEARCHTYPE_NDDSAROUNDPOI;

				sSearchInput.wpCurrent.x = sSearchReq.tpCurrent.x;
				sSearchInput.wpCurrent.y = sSearchReq.tpCurrent.y;
				switch(sSearchReq.eAroundPoiType)
				{
				case DE_AROUNDTYPE_NONE:
					strcpy(sSearchInput.szSearchName, sSearchReq.szSearchName);
					break;
				case DE_AROUNDTYPE_OPI:
					strcpy(sSearchInput.szSearchName, "주유소");
					break;
				case DE_AROUNDTYPE_LPG:
                    strcpy(sSearchInput.szSearchName, "OILALL_OILLPG");
					break;
				case DE_AROUNDTYPE_RESTAURANT:
                    strcpy(sSearchInput.szSearchName, "FUDALL");
					break;
				case DE_AROUNDTYPE_MART:
					strcpy(sSearchInput.szSearchName, "마트/편의점");
					break;
				case DE_AROUNDTYPE_PARK:
                    strcpy(sSearchInput.szSearchName, "PARKLOT");
					break;
				case DE_AROUNDTYPE_GARAGE:
                    strcpy(sSearchInput.szSearchName, "MATNALL");
					break;
				case DE_AROUNDTYPE_BANK:
                    strcpy(sSearchInput.szSearchName, "BNKALL");
					break;
				case DE_AROUNDTYPE_HOSPITAL:
					strcpy(sSearchInput.szSearchName, "병원/약국");
					break;
				case DE_AROUNDTYPE_TRAFFIC:
					strcpy(sSearchInput.szSearchName, "교통");
					break;
				case DE_AROUNDTYPE_SHOPPING:
					strcpy(sSearchInput.szSearchName, "쇼핑");
					break;
				case DE_AROUNDTYPE_LODGMENT:
                    strcpy(sSearchInput.szSearchName, "HOTLALL");
					break;
				case DE_AROUNDTYPE_TOUR:
                    strcpy(sSearchInput.szSearchName, "TURALL");
					break;
				case DE_AROUNDTYPE_OPI_EV:
					strcpy(sSearchInput.szSearchName, "OILALL_OILEV");
					break;
				default:
					return DE_ERROR_SEARCH_MAXNAME;
				}

				sSearchInput.nDist = sSearchReq.nDist;
				sSearchInput.bNewAddr = sSearchReq.bNewAddr;
			}
			break;
		case DE_SEARCHTYPE_SERVERPOIDETAILINFO:
			{
				DS_SEARCH_SERVERPOIDETAILINFO_REQ &sSearchReq = pSearchReq->sSearchServerPoiDetailInfo;

				if(sSearchReq.nPoiID < 1)
				{
					return DE_ERROR_SEARCH_POIID;
				}

				SEARCH_NDDSPOIDETAILINFO_INPUT &sSearchInput = sRequest.sSearchInput.sSearchNddsPoiDetailInfo;
				sSearchInput.eSearchType = N_SEARCHTYPE_NDDSPOIDETAILINFO;

				sSearchInput.nPoiID = sSearchReq.nPoiID;
				strcpy(sSearchInput.navSeq, sSearchReq.navSeq);
				sSearchInput.nSizeWidth = sSearchReq.nSizeWidth;
				sSearchInput.nSizeHeight = sSearchReq.nSizeHeight;
			}
			break;
		case DE_SEARCHTYPE_SERVERPOIJOININFO:
			{
				DS_SEARCH_SERVERPOIJOININFO_REQ &sSearchReq = pSearchReq->sSearchServerPoiJoinInfo;

				if(sSearchReq.nPoiID < 1)
				{
					return DE_ERROR_SEARCH_POIID;
				}

				SEARCH_NDDSPOIJOININFO_INPUT &sSearchInput = sRequest.sSearchInput.sSearchNddsPoiJoinInfo;
				sSearchInput.eSearchType = N_SEARCHTYPE_NDDSPOIJOININFO;

				sSearchInput.nPoiID = sSearchReq.nPoiID;
			}
			break;
		case DE_SEARCHTYPE_SERVERRECENTDESTNATION:
			{
				sRequest.sSearchInput.eSearchType = N_SEARCHTYPE_NDDSRECENTDESTNATION;
			}
			break;
      case DE_SEARCHTYPE_SERVERUPLOADRECENTDESTNATION:
            {
                for(int i=0;i<pSearchReq->sSearchUploadRecentDestnation.nUploadCnt;i++)
                {
                    sRequest.sSearchInput.sSearchNddsUploadRecentInfo.pUploadRecentPoi->nPoiID = pSearchReq->sSearchUploadRecentDestnation.sUploadRecent->nPoiID;
                    strcpy(sRequest.sSearchInput.sSearchNddsUploadRecentInfo.pUploadRecentPoi[i].szName, pSearchReq->sSearchUploadRecentDestnation.sUploadRecent[i].szName);
                    sRequest.sSearchInput.sSearchNddsUploadRecentInfo.pUploadRecentPoi[i].RPFlag = pSearchReq->sSearchUploadRecentDestnation.sUploadRecent[i].byRPFlag;
                    strcpy(sRequest.sSearchInput.sSearchNddsUploadRecentInfo.pUploadRecentPoi[i].szTotalCnt, pSearchReq->sSearchUploadRecentDestnation.sUploadRecent[i].nToltalCnt);
                    strcpy(sRequest.sSearchInput.sSearchNddsUploadRecentInfo.pUploadRecentPoi[i].szNavseq, pSearchReq->sSearchUploadRecentDestnation.sUploadRecent[i].szNavseq);
                    strcpy(sRequest.sSearchInput.sSearchNddsUploadRecentInfo.pUploadRecentPoi[i].szSvcDate, pSearchReq->sSearchUploadRecentDestnation.sUploadRecent[i].szSvcDate);
                    sRequest.sSearchInput.sSearchNddsUploadRecentInfo.pUploadRecentPoi[i].tp.x =  pSearchReq->sSearchUploadRecentDestnation.sUploadRecent[i].tp.x;
                    sRequest.sSearchInput.sSearchNddsUploadRecentInfo.pUploadRecentPoi[i].tp.y =  pSearchReq->sSearchUploadRecentDestnation.sUploadRecent[i].tp.y;
                }

                sRequest.sSearchInput.sSearchNddsUploadRecentInfo.nUploadRecentPoiCnt =  pSearchReq->sSearchUploadRecentDestnation.nUploadCnt;
				strcpy(sRequest.sSearchInput.sSearchNddsUploadRecentInfo.szSvcUploadYn, pSearchReq->sSearchUploadRecentDestnation.szSvcUploadYn);
                sRequest.sSearchInput.eSearchType = N_SEARCHTYPE_NDDSUPLOADRECENTDESTNATION;
            }
            break;
        case DE_SEARCHTYPE_SERVERREMOVERECENTDESTNATION:
            {
                sRequest.sSearchInput.sSearchNddsRemoveRecentInfo.nRemovePoiCnt = pSearchReq->sSearchRemoveRecentDestnation.nRemoveCnt;

                for(int i=0;i<sRequest.sSearchInput.sSearchNddsRemoveRecentInfo.nRemovePoiCnt;i++)
                {
                    strcpy(sRequest.sSearchInput.sSearchNddsRemoveRecentInfo.pRemoveRecentPoi[i].szName, pSearchReq->sSearchRemoveRecentDestnation.sRemoveRecent[i].szName);
                    sRequest.sSearchInput.sSearchNddsRemoveRecentInfo.pRemoveRecentPoi[i].tp.x = pSearchReq->sSearchRemoveRecentDestnation.sRemoveRecent[i].tp.x;
                    sRequest.sSearchInput.sSearchNddsRemoveRecentInfo.pRemoveRecentPoi[i].tp.y = pSearchReq->sSearchRemoveRecentDestnation.sRemoveRecent[i].tp.y;
                }
                sRequest.sSearchInput.eSearchType = N_SEARCHTYPE_NDDSREMOVERECENTDESTNATION;
            }
            break;
		case DE_SEARCHTYPE_SERVERADDR:
			{
				DS_SEARCH_SERVERADDR_REQ &sSearchReq = pSearchReq->sSearchServerAddr;

				if(strlen(sSearchReq.szAddrName1) > MAXLEN_POINAME)
				{
					return DE_ERROR_SEARCH_MAXNAME;
				}

				SEARCH_NDDSAREANAMESBYSTEP_INPUT &sSearchInput = sRequest.sSearchInput.sSearchNddsAreaNamesByStep;
				sSearchInput.eSearchType = N_SEARCHTYPE_NDDSAREANAMESBYSTEP;

				strcpy(sSearchInput.szAddrName1, sSearchReq.szAddrName1);
				strcpy(sSearchInput.szAddrName2, sSearchReq.szAddrName2);
				sSearchInput.bNewAddr = sSearchReq.bNewAddr;
			}
			break;
        case DE_SEARCHTYPE_SERVERPOISTATIONINFO:
            {
                DS_SEARCH_SERVERPOISTATIONINFO_REQ &sSearchReq = pSearchReq->sSearchServerPoiStationInfo;

                SEARCH_NDDSPOISTATIONINFO_INPUT &sSearchInput = sRequest.sSearchInput.sSearchNddsPoiStationInfo;
                sSearchInput.eSearchType = N_SEARCHTYPE_NDDSPOISTATIONINFO;

                sSearchInput.wpCurrent.x = sSearchReq.tp.x;
                sSearchInput.wpCurrent.y = sSearchReq.tp.y;
                sSearchInput.nDist = sSearchReq.nDist;
                strcpy(sSearchInput.szDataKind, sSearchReq.szDataKind);
                strcpy(sSearchInput.szFuelType, sSearchReq.szFuelType);
            }
            break;
            case DE_SEARCHTYPE_SERVERPOIEVSTATIONINFO:
            {
                DS_SEARCH_SERVERPOIEVSTATIONINFO_REQ &sSearchReq = pSearchReq->sSearchServerPoiEvStationInfo;

                SEARCH_NDDSPOIEVSTATIONINFO_INPUT &sSearchInput = sRequest.sSearchInput.sSearchNddsPoiEvStationInfo;
                sSearchInput.eSearchType = N_SEARCHTYPE_NDDSPOIEVSTATIONINFO;

                sSearchInput.nPoiID = sSearchReq.nPoiID;
            }
            break;
            case DE_SEARCHTYPE_SERVERPOIEVSTATIONSBYPOINT:
            {
                DS_SEARCH_SERVERPOIEVSTATIONSBYPOINT_REQ &sSearchReq = pSearchReq->sSearchServerPoiEvStationsByPoint;

                SEARCH_NDDSPOIEVSTATIONSBYPOINT_INPUT &sSearchInput = sRequest.sSearchInput.sSearchNddsPoiEvStationsByPoint;
                sSearchInput.eSearchType = N_SEARCHTYPE_NDDSPOIEVSTATIONSBYPOINT;

                sSearchInput.wpCurrent.x = sSearchReq.tp.x;
                sSearchInput.wpCurrent.y = sSearchReq.tp.y;
                sSearchInput.nDist = sSearchReq.nDist;
            }
            break;
		default:
		{
			return DE_ERROR_SEARCH_TYPE;
		}
		break;
	}

	bRet = NIF_Function(eAPI_NDDS_Search, &sRequest, &sResult);
	if(!bRet)
	{
		return DE_ERROR_FAIL;
	}

	if(nSessionID)
		*nSessionID = sResult.nSessionID;

	if(sResult.nResult > 200)
		nResult = 200;
	else
		nResult = sResult.nResult;

	if(sResult.nResult <= 0 && *sResult.szErrorMsg)
	{
		//return DE_ERROR_SEARCH_SERVER;
	}

	return DE_ERROR_SUCCESS;
}

NDDS_API DeErrorCode NDDS_GetSearchResultList(DeSearchType eSearchType, int nIdx, int &nCnt, DS_SEARCH_LIST *pResult, int *nTotal)
{
	NIF_NDDS_NOT_INITIALIZED_RETURN_FAIL;

	if(pResult == NULL)
	{
		return DE_ERROR_NOT_REQMEMORY;
	}
	if(nIdx < 0 || nCnt <= 0)
	{
		return DE_ERROR_INVALID_PARAM;
	}

	int i, nResult, nIdxSat, nIdxMax;
	bool bRet;
	API_SEARCH_RESULTLIST_REQ sRequest;
	API_SEARCH_RESULTLIST_RES sResult;

	memset(&sRequest, 0x00, sizeof(API_SEARCH_RESULTLIST_REQ));
	memset(&sResult, 0x00, sizeof(API_SEARCH_RESULTLIST_RES));

	switch(eSearchType)
	{
	case DE_SEARCHTYPE_SERVER:
		sRequest.eSearchType = N_SEARCHTYPE_NDDSPOI; break;
	case DE_SEARCHTYPE_SERVERAROUND:
		sRequest.eSearchType = N_SEARCHTYPE_NDDSAROUNDPOI; break;
	case DE_SEARCHTYPE_SERVERADDR:
		sRequest.eSearchType = N_SEARCHTYPE_NDDSAREANAMESBYSTEP; break;
    case DE_SEARCHTYPE_SERVERPOISTATIONINFO:
        sRequest.eSearchType = N_SEARCHTYPE_NDDSPOISTATIONINFO; break;
    case DE_SEARCHTYPE_SERVERPOIEVSTATIONSBYPOINT:
        sRequest.eSearchType = N_SEARCHTYPE_NDDSPOIEVSTATIONSBYPOINT; break;
	default:
		return DE_ERROR_SEARCH_TYPE;
		break;
	}

	nResult = 0;
	nIdxSat = nIdx;
	nIdxMax = nIdx + nCnt; // 마지막 인덱스 다음
	while(1)
	{
		sRequest.nIdx = nIdxSat;
		if(nIdxSat + MAX_RESULTLIST <= nIdxMax)
			sRequest.nCnt = MAX_RESULTLIST;
		else
			sRequest.nCnt = nIdxMax - nIdxSat;

		memset(&sResult, 0x00, sizeof(API_SEARCH_RESULTLIST_RES));
		bRet = NIF_Function(eAPI_NDDS_GetSearchResultList, &sRequest, &sResult);
		if(!bRet || sResult.nResult <= 0) break;

		N_SEARCH_LIST *pList;
		for(i=0 ; i<sResult.nResult ; i++)
		{
			pList = &sResult.sList[i];
			pResult[nResult].nIdx = pList->nIdx;

			strcpy(pResult[nResult].szName, pList->szName);
			strcpy(pResult[nResult].szStdName, pList->szStdName);
			// 주소검색(아파트)에서 szName이 없는 경우가 있는데 szName이 없으면 szStdName을 szName에 복사하도록 수정
			if(pResult[nResult].szName[0] == NULL)
			{
				strcpy(pResult[nResult].szName, pList->szStdName);
			}

			strcpy(pResult[nResult].szAddrName, pList->szAddrName);
			strcpy(pResult[nResult].szMngName, pList->szMngName);
			strcpy(pResult[nResult].szPhoneNumber, pList->szPhoneNumber);
			pResult[nResult].nPoiID = pList->nPoiID;
			pResult[nResult].nAddrCode = pList->addrcode;
			pResult[nResult].byRPFlag = pList->RPFlag;
			pResult[nResult].nDist = pList->nDist;
			pResult[nResult].nDeg = pList->nDeg;
			pResult[nResult].nKind = pList->nKind;
			pResult[nResult].tp.x = pList->wp.x;
			pResult[nResult].tp.y = pList->wp.y;

			strcpy(pResult[nResult].szOilCompany, pList->szOilCompany);
            pResult[nResult].nDataKind = pList->nDataKind;
			pResult[nResult].bHighGasoline = pList->bHighGasoline;
			pResult[nResult].bLowestOilSt = pList->bLowestOilSt;
			pResult[nResult].nOPIGasolinePrice = pList->nOPIGasolinePrice;
			pResult[nResult].nOPIDieselPrice = pList->nOPIDieselPrice;
			pResult[nResult].nOPILPGPrice = pList->nOPILPGPrice;
			pResult[nResult].nOPIHiGasolinePrice = pList->nOPIHiGasolinePrice;
			pResult[nResult].nOPIHiLPGPrice = pList->nOPIHiLPGPrice;
			strcpy(pResult[nResult].szNavSeq, pList->szNavSeq);
            if(pList->nEvChargerTotalCnt > 0)
            {
                pResult[nResult].nEvChargerTotalCnt = pList->nEvChargerTotalCnt;
                for(int j=0; j<pResult[nResult].nEvChargerTotalCnt; j++)
                {
                    if(pList->sEvChargers[j].szBid)
                        strcpy(pResult[nResult].sChargerInfo[j].szBid, pList->sEvChargers[j].szBid);
                    if(pList->sEvChargers[j].szSid)
                        strcpy(pResult[nResult].sChargerInfo[j].szSid, pList->sEvChargers[j].szSid);
                    if(pList->sEvChargers[j].szCid)
                        strcpy(pResult[nResult].sChargerInfo[j].szCid, pList->sEvChargers[j].szCid);
                    if(pList->sEvChargers[j].szType)
                        strcpy(pResult[nResult].sChargerInfo[j].szType, pList->sEvChargers[j].szType);
                    pResult[nResult].sChargerInfo[j].nStatus = pList->sEvChargers[j].nStatus;
                }
            }

			nResult++;
		}
		nIdxSat += sResult.nResult;

		if(nResult >= nCnt)
			break;
	}

	nCnt = nResult;
	if(nTotal)
	{
		*nTotal = sResult.nTotal;
	}

	return DE_ERROR_SUCCESS;
}

NDDS_API DeErrorCode NDDS_GetPoiEvStationInfo(DeSearchType eSearchType, DS_EVSTATION_INFO *pResult)
{
    NIF_NDDS_NOT_INITIALIZED_RETURN_FAIL

    int nResult = 0;
    bool bRet;

    API_SEARCH_POIEVSTATIONINFO_RES sResult;
    memset(&sResult, 0x00, sizeof(API_SEARCH_POIEVSTATIONINFO_RES));

    bRet = NIF_Function(eAPI_NDDS_GetSearchPoiEvStationInfo, NULL, &sResult);

    N_SEARCH_POIEVSTATIONINFO *pEvStationChargerData;
    pEvStationChargerData = &sResult.sPoiEvStationInfo;
    pResult->poiId = pEvStationChargerData->nPoiID;
    if(pEvStationChargerData->szStationName)
        strcpy(pResult->szStationName, pEvStationChargerData->szStationName);
    if(pEvStationChargerData->szAddr)
        strcpy(pResult->szAddr, pEvStationChargerData->szAddr);
    if(pEvStationChargerData->szRoadAddr)
        strcpy(pResult->szRoadAddr, pEvStationChargerData->szRoadAddr);
    if(pEvStationChargerData->szAddrDetail)
        strcpy(pResult->szAddrDetail, pEvStationChargerData->szAddrDetail);
    if(pEvStationChargerData->szMngName)
        strcpy(pResult->szMngName, pEvStationChargerData->szMngName);
    if(pEvStationChargerData->szMngId)
        strcpy(pResult->szMngId, pEvStationChargerData->szMngId);
    if(pEvStationChargerData->szTel)
        strcpy(pResult->szTel, pEvStationChargerData->szTel);
    pResult->bFreeYn = pEvStationChargerData->bFreeYn;
    pResult->bReservYn = pEvStationChargerData->bReservYn;
    if(pEvStationChargerData->szUseTime)
        strcpy(pResult->szUseTime, pEvStationChargerData->szUseTime);
    pResult->bPayYn = pEvStationChargerData->bPayYn;
    if(pEvStationChargerData->szFee)
        strcpy(pResult->szFee, pEvStationChargerData->szFee);
    if(pEvStationChargerData->szUpdateDt)
        strcpy(pResult->szUpdateDt, pEvStationChargerData->szUpdateDt);
    pResult->tp.x = pEvStationChargerData->wp.x;
    pResult->tp.y = pEvStationChargerData->wp.y;
    pResult->nTotalCnt = pEvStationChargerData->nTotalCnt;
    int cnt = pEvStationChargerData->nTotalCnt;
    memset(&pResult->chargerInfo, 0, sizeof(DS_POI_EVSTATION_CHARGERINFO)*cnt);

    for(int i=0;i<cnt;i++)
    {
        if(pEvStationChargerData->sEvChargers[i].szBid)
            strcpy(pResult->chargerInfo[i].szBid, pEvStationChargerData->sEvChargers[i].szBid);
        if(pEvStationChargerData->sEvChargers[i].szSid)
            strcpy(pResult->chargerInfo[i].szSid, pEvStationChargerData->sEvChargers[i].szSid);
        if(pEvStationChargerData->sEvChargers[i].szCid)
            strcpy(pResult->chargerInfo[i].szCid, pEvStationChargerData->sEvChargers[i].szCid);
        if(pEvStationChargerData->sEvChargers[i].szType)
            strcpy(pResult->chargerInfo[i].szType, pEvStationChargerData->sEvChargers[i].szType);
        pResult->chargerInfo[i].nStatus = pEvStationChargerData->sEvChargers[i].nStatus;
    }

    return DE_ERROR_SUCCESS;
}

NDDS_API DeErrorCode NDDS_GetUserData(DeSearchType eSearchType, DS_USER_INFO pResult)
{
    NIF_NDDS_NOT_INITIALIZED_RETURN_FAIL

    int nResult = 0;
    bool bRet;

    API_GET_USERDATA_RES sResult;
    memset(&sResult, 0x00, sizeof(API_GET_USERDATA_RES));

    bRet = NIF_Function(eAPI_NDDS_GetUserData, NULL, &sResult);

    N_GET_USERDATA *pUserData;
    pUserData = &sResult.sUserData;

    //poiMyFavorite
    strcpy(pResult.sPoiMyFavorite.szHomeCustName, pUserData->sPoiMyFavorite.szHomeCustName);
    pResult.sPoiMyFavorite.Hometp.x = pUserData->sPoiMyFavorite.Hometp.x;
    pResult.sPoiMyFavorite.Hometp.y = pUserData->sPoiMyFavorite.Hometp.y;
    pResult.sPoiMyFavorite.nHomePoiID = pUserData->sPoiMyFavorite.nHomePoiId;
    strcpy(pResult.sPoiMyFavorite.szHomeNavSeq, pUserData->sPoiMyFavorite.szHomeNavSeq);
    strcpy(pResult.sPoiMyFavorite.szHomeLcdName, pUserData->sPoiMyFavorite.szHomeLcdName);
    strcpy(pResult.sPoiMyFavorite.szHomeMcdName, pUserData->sPoiMyFavorite.szHomeMcdName);
    strcpy(pResult.sPoiMyFavorite.szHomeScdName, pUserData->sPoiMyFavorite.szHomeScdName);
    strcpy(pResult.sPoiMyFavorite.szHomeDcdName, pUserData->sPoiMyFavorite.szHomeDcdName);
    strcpy(pResult.sPoiMyFavorite.szHomePrimaryBun, pUserData->sPoiMyFavorite.szHomePrimaryBun);
    strcpy(pResult.sPoiMyFavorite.szHomeSecondaryBun, pUserData->sPoiMyFavorite.szHomeSecondBun);
    pResult.sPoiMyFavorite.nHomeMlClass = pUserData->sPoiMyFavorite.nHomeMlClass;
    strcpy(pResult.sPoiMyFavorite.szHomeRoadName, pUserData->sPoiMyFavorite.szHomeRoadName);
    strcpy(pResult.sPoiMyFavorite.szHomeBldNo1, pUserData->sPoiMyFavorite.szHomeBldNo1);
    strcpy(pResult.sPoiMyFavorite.szHomeBldNo2, pUserData->sPoiMyFavorite.szHomeBldNo2);
    pResult.sPoiMyFavorite.byHomeRpFlag =  pUserData->sPoiMyFavorite.byHomeRpFlag;
    strcpy(pResult.sPoiMyFavorite.szHomeAddInfo, pUserData->sPoiMyFavorite.szHomeAddInfo);
    strcpy(pResult.sPoiMyFavorite.szHomeInsDatetime, pUserData->sPoiMyFavorite.szHomeInsDatetime);

    strcpy(pResult.sPoiMyFavorite.szOfficeCustName, pUserData->sPoiMyFavorite.szOfficeCustName);
    pResult.sPoiMyFavorite.Officetp.x = (pUserData->sPoiMyFavorite.Officetp.x);
    pResult.sPoiMyFavorite.Officetp.y = (pUserData->sPoiMyFavorite.Officetp.y);
    pResult.sPoiMyFavorite.nOfficePoiID = (pUserData->sPoiMyFavorite.nOfficePoiId);
    strcpy(pResult.sPoiMyFavorite.szOfficeNavSeq, pUserData->sPoiMyFavorite.szOfficeNavSeq);
    strcpy(pResult.sPoiMyFavorite.szOfficeLcdName, pUserData->sPoiMyFavorite.szOfficeLcdName);
    strcpy(pResult.sPoiMyFavorite.szOfficeMcdName, pUserData->sPoiMyFavorite.szOfficeMcdName);
    strcpy(pResult.sPoiMyFavorite.szOfficeScdName, pUserData->sPoiMyFavorite.szOfficeScdName);
    strcpy(pResult.sPoiMyFavorite.szOfficeDcdName, pUserData->sPoiMyFavorite.szOfficeDcdName);
    strcpy(pResult.sPoiMyFavorite.szOfficePrimaryBun, pUserData->sPoiMyFavorite.szOfficePrimaryBun);
    strcpy(pResult.sPoiMyFavorite.szOfficeSecondaryBun, pUserData->sPoiMyFavorite.szOfficeSecondBun);
    pResult.sPoiMyFavorite.nOfficeMlClass = pUserData->sPoiMyFavorite.nOfficeMlClass;
    strcpy(pResult.sPoiMyFavorite.szOfficeRoadName, pUserData->sPoiMyFavorite.szOfficeRoadName);
    strcpy(pResult.sPoiMyFavorite.szOfficeBldNo1, pUserData->sPoiMyFavorite.szOfficeBldNo1);
    strcpy(pResult.sPoiMyFavorite.szOfficeBldNo2, pUserData->sPoiMyFavorite.szOfficeBldNo2);
    pResult.sPoiMyFavorite.byOfficeRpFlag =  pUserData->sPoiMyFavorite.byOfficeRpFlag;
    strcpy(pResult.sPoiMyFavorite.szOfficeAddInfo, pUserData->sPoiMyFavorite.szOfficeAddInfo);
    strcpy(pResult.sPoiMyFavorite.szOfficeInsDatetime, pUserData->sPoiMyFavorite.szOfficeInsDatetime);

    //routeMyFavorite
    strcpy(pResult.sRouteMyFavorite.routeTitle, pUserData->sRouteMyFavorite.routeTitle);
    strcpy(pResult.sRouteMyFavorite.insDatetime, pUserData->sRouteMyFavorite.insDatetime);
    //start Route
    strcpy(pResult.sRouteMyFavorite.sStartRoute.szCustName, pUserData->sRouteMyFavorite.sStartRoute.szCustName);
    pResult.sRouteMyFavorite.sStartRoute.nPoiID = pUserData->sRouteMyFavorite.sStartRoute.nPoiID;
    pResult.sRouteMyFavorite.sStartRoute.byRpFlag = pUserData->sRouteMyFavorite.sStartRoute.byRpFlag;
    pResult.sRouteMyFavorite.sStartRoute.tp.x = pUserData->sRouteMyFavorite.sStartRoute.tp.x;
    pResult.sRouteMyFavorite.sStartRoute.tp.y = pUserData->sRouteMyFavorite.sStartRoute.tp.y;
    //end Route
    strcpy(pResult.sRouteMyFavorite.sEndRoute.szCustName, pUserData->sRouteMyFavorite.sEndRoute.szCustName);
    pResult.sRouteMyFavorite.sEndRoute.nPoiID = pUserData->sRouteMyFavorite.sEndRoute.nPoiID;
    pResult.sRouteMyFavorite.sEndRoute.byRpFlag = pUserData->sRouteMyFavorite.sEndRoute.byRpFlag;
    pResult.sRouteMyFavorite.sEndRoute.tp.x = pUserData->sRouteMyFavorite.sEndRoute.tp.x;
    pResult.sRouteMyFavorite.sEndRoute.tp.y = pUserData->sRouteMyFavorite.sEndRoute.tp.y;
    for(int i=0;i<3;i++)
    {
        //mid Route1
        strcpy(pResult.sRouteMyFavorite.sMidRoute[i].szCustName, pUserData->sRouteMyFavorite.sMidRoute[i].szCustName);
        pResult.sRouteMyFavorite.sMidRoute[i].nPoiID = pUserData->sRouteMyFavorite.sMidRoute[i].nPoiID;
        pResult.sRouteMyFavorite.sMidRoute[i].byRpFlag = pUserData->sRouteMyFavorite.sMidRoute[i].byRpFlag;
        pResult.sRouteMyFavorite.sMidRoute[i].tp.x = pUserData->sRouteMyFavorite.sMidRoute[i].tp.x;
        pResult.sRouteMyFavorite.sMidRoute[i].tp.y = pUserData->sRouteMyFavorite.sMidRoute[i].tp.y;
    }

    //poiRecent
    for(int i=0;i<pUserData->nPoiRecent;i++)
    {
        pResult.sRouteRecent[i].nPoiID = pUserData->sPoiRecent[i].nPoiID;
        strcpy(pResult.sRouteRecent[i].szNavSeq, pUserData->sPoiRecent[i].szNavseq);
        strcpy(pResult.sRouteRecent[i].szName, pUserData->sPoiRecent[i].szName);
        pResult.sRouteRecent[i].tp.x = pUserData->sPoiRecent[i].tp.x;
        pResult.sRouteRecent[i].tp.y = pUserData->sPoiRecent[i].tp.y;
        strcpy(pResult.sRouteRecent[i].szLcdName, pUserData->sPoiRecent[i].szLcdName);
        strcpy(pResult.sRouteRecent[i].szMcdName, pUserData->sPoiRecent[i].szMcdName);
        strcpy(pResult.sRouteRecent[i].szScdName, pUserData->sPoiRecent[i].szScdName);
        strcpy(pResult.sRouteRecent[i].szDcdName, pUserData->sPoiRecent[i].szDcdName);
        strcpy(pResult.sRouteRecent[i].szPrimaryBun, pUserData->sPoiRecent[i].szPrimaryBun);
        strcpy(pResult.sRouteRecent[i].szSecondaryBun, pUserData->sPoiRecent[i].szSecondBun);
        pResult.sRouteRecent[i].nMlClass = pUserData->sPoiRecent[i].nMlClass;
        strcpy(pResult.sRouteRecent[i].szRoadName, pUserData->sPoiRecent[i].szRoadName);
        strcpy(pResult.sRouteRecent[i].szBldNo1, pUserData->sPoiRecent[i].szBldNo1);
        strcpy(pResult.sRouteRecent[i].szBldNo2, pUserData->sPoiRecent[i].szBldNo2);
        strcpy(pResult.sRouteRecent[i].szRepClsName, pUserData->sPoiRecent[i].szRepClsName);
        strcpy(pResult.sRouteRecent[i].szClsAName, pUserData->sPoiRecent[i].szClsAName);
        strcpy(pResult.sRouteRecent[i].szClsBName, pUserData->sPoiRecent[i].szClsBName);
        strcpy(pResult.sRouteRecent[i].szClsCName, pUserData->sPoiRecent[i].szClsCName);
        strcpy(pResult.sRouteRecent[i].szClsDName, pUserData->sPoiRecent[i].szClsDName);
        pResult.sRouteRecent[i].bRpFlag = pUserData->sPoiRecent[i].byRPFlag;
        pResult.sRouteRecent[i].szTotalCnt = atoi(pUserData->sPoiRecent[i].szTotalCnt);
        strcpy(pResult.sRouteRecent[i].szSvcDate, pUserData->sPoiRecent[i].szSvcDate);
    }

    //poiFavorite
    for(int i=0;i<pUserData->nPoiFavorite;i++)
    {
		strcpy(pResult.sPoiFavorite[i].szCustName, pUserData->sPoiFavorite[i].szCustName);
        pResult.sPoiFavorite[i].tp.x = pUserData->sPoiFavorite[i].tp.x;
        pResult.sPoiFavorite[i].tp.y = pUserData->sPoiFavorite[i].tp.y;
        pResult.sPoiFavorite[i].nPoiID = pUserData->sPoiFavorite[i].nPoiID;
        sprintf(pResult.sPoiFavorite[i].szNavSeq, "%d", pUserData->sPoiFavorite[i].nPoiID);
        strcpy(pResult.sPoiFavorite[i].szLcdName, pUserData->sPoiFavorite[i].szLcdName);
        strcpy(pResult.sPoiFavorite[i].szMcdName, pUserData->sPoiFavorite[i].szMcdName);
        strcpy(pResult.sPoiFavorite[i].szDcdName, pUserData->sPoiFavorite[i].szDcdName);
        strcpy(pResult.sPoiFavorite[i].szPrimaryBun, pUserData->sPoiFavorite[i].szPrimaryBun);
        strcpy(pResult.sPoiFavorite[i].szSecondaryBun, pUserData->sPoiFavorite[i].szSecondBun);
        pResult.sPoiFavorite[i].nMlClass = pUserData->sPoiFavorite[i].nMlClass;
        strcpy(pResult.sPoiFavorite[i].szRoadName, pUserData->sPoiFavorite[i].szRoadName);
        strcpy(pResult.sPoiFavorite[i].szBldNo1, pUserData->sPoiFavorite[i].szBldNo1);
        strcpy(pResult.sPoiFavorite[i].szBldNo2, pUserData->sPoiFavorite[i].szBldNo2);
        strcpy(pResult.sPoiFavorite[i].szRepClsName, pUserData->sPoiFavorite[i].szRepClsName);
        strcpy(pResult.sPoiFavorite[i].szClsAName, pUserData->sPoiFavorite[i].szClsAName);
        strcpy(pResult.sPoiFavorite[i].szClsBName, pUserData->sPoiFavorite[i].szClsBName);
        strcpy(pResult.sPoiFavorite[i].szClsCName, pUserData->sPoiFavorite[i].szClsCName);
        strcpy(pResult.sPoiFavorite[i].szClsDName, pUserData->sPoiFavorite[i].szClsDName);
		pResult.sPoiFavorite[i].byRPFlag = pUserData->sPoiFavorite[i].byRPFlag;
        strcpy(pResult.sPoiFavorite[i].szTelNo, pUserData->sPoiFavorite[i].szTelNo);
        strcpy(pResult.sPoiFavorite[i].szAddInfo, pUserData->sPoiFavorite[i].szAddInfo);
        strcpy(pResult.sPoiFavorite[i].szOrgCustName, pUserData->sPoiFavorite[i].szOrgCustName);
		strcpy(pResult.sPoiFavorite[i].szInsDatetime, pUserData->sPoiFavorite[i].szInsDatetime);
    }

    //routeFavorite
    for(int i=0;i<pUserData->nRouteFavorite;i++)
    {
        strcpy(pResult.sRouteFavorite[i].szRouteTitle, pUserData->sRouteFavorite[i].routeTitle);
        strcpy(pResult.sRouteFavorite[i].szInsDateTime, pUserData->sRouteFavorite[i].insDatetime);
        //start Route
        strcpy(pResult.sRouteFavorite[i].sStartRoute.szCustName, pUserData->sRouteFavorite[i].sStartRoute.szCustName);
        pResult.sRouteFavorite[i].sStartRoute.nPoiID = pUserData->sRouteFavorite[i].sStartRoute.nPoiID;
        pResult.sRouteFavorite[i].sStartRoute.byRpFlag = pUserData->sRouteFavorite[i].sStartRoute.byRpFlag;
        pResult.sRouteFavorite[i].sStartRoute.tp.x = pUserData->sRouteFavorite[i].sStartRoute.tp.x;
        pResult.sRouteFavorite[i].sStartRoute.tp.y = pUserData->sRouteFavorite[i].sStartRoute.tp.y;
        //end Route
        strcpy(pResult.sRouteFavorite[i].sEndRoute.szCustName, pUserData->sRouteFavorite[i].sEndRoute.szCustName);
        pResult.sRouteFavorite[i].sEndRoute.nPoiID = pUserData->sRouteFavorite[i].sEndRoute.nPoiID;
        pResult.sRouteFavorite[i].sEndRoute.byRpFlag = pUserData->sRouteFavorite[i].sEndRoute.byRpFlag;
        pResult.sRouteFavorite[i].sEndRoute.tp.x = pUserData->sRouteFavorite[i].sEndRoute.tp.x;
        pResult.sRouteFavorite[i].sEndRoute.tp.y = pUserData->sRouteFavorite[i].sEndRoute.tp.y;

        //mid Route
        for(int j=0;j<3;j++)
        {
            strcpy(pResult.sRouteFavorite[i].sMidRoute[j].szCustName, pUserData->sRouteFavorite[i].sMidRoute[j].szCustName);
            pResult.sRouteFavorite[i].sMidRoute[j].nPoiID = pUserData->sRouteFavorite[i].sMidRoute[j].nPoiID;
            pResult.sRouteFavorite[i].sMidRoute[j].byRpFlag = pUserData->sRouteFavorite[i].sMidRoute[j].byRpFlag;
            pResult.sRouteFavorite[i].sMidRoute[j].tp.x = pUserData->sRouteFavorite[i].sMidRoute[j].tp.x;
            pResult.sRouteFavorite[i].sMidRoute[j].tp.y = pUserData->sRouteFavorite[i].sMidRoute[j].tp.y;
        }
    }

    return DE_ERROR_SUCCESS;
}

NDDS_API DeErrorCode NDDS_GetSearchPoiDetailInfo(DS_SEARCH_POI_DETAIL_INFO *pResult)
{
	NIF_NDDS_NOT_INITIALIZED_RETURN_FAIL;

	if(pResult == NULL)
	{
		return DE_ERROR_NOT_REQMEMORY;
	}

	int nResult = 0;
	bool bRet;
	API_SEARCH_POIDETAILINFO_RES sResult;

	memset(&sResult, 0x00, sizeof(API_SEARCH_POIDETAILINFO_RES));

	bRet = NIF_Function(eAPI_NDDS_GetSearchPoiDetailInfo, NULL, &sResult);
	if(!bRet || sResult.nResult <= 0) return DE_ERROR_FAIL;

	N_SEARCH_POIDETAILINFO *pPoiDetailInfo;

	pPoiDetailInfo = &sResult.sPoiDetailInfo;

	pResult->nPoiID = pPoiDetailInfo->nPoiID;
	strcpy(pResult->szNavSeq, pPoiDetailInfo->szNavSeq);
	strcpy(pResult->szName, pPoiDetailInfo->szName);
	strcpy(pResult->szAddrName, pPoiDetailInfo->szAddrName);
	pResult->tp.x = pPoiDetailInfo->tp.x;
	pResult->tp.y = pPoiDetailInfo->tp.y;
	strcpy(pResult->szPhoneNumber, pPoiDetailInfo->szPhoneNumber);
	strcpy(pResult->szUrl, pPoiDetailInfo->szUrl);
	pResult->bHoliday = pPoiDetailInfo->bHoliday;
	pResult->byPark = pPoiDetailInfo->byPark;
	strcpy(pResult->szCateName, pPoiDetailInfo->szCateName);
	strncpy(pResult->szInfo, pPoiDetailInfo->szInfo, D_MAXLEN_DETAILINFO);

    if(pPoiDetailInfo->szPrimaryBun)
        strcpy(pResult->szPrimaryBun, pPoiDetailInfo->szPrimaryBun);

    if(pPoiDetailInfo->szSecondBun)
        strcpy(pResult->szSecondaryBun, pPoiDetailInfo->szSecondBun);

    if(pPoiDetailInfo->szLcdName)
        strcpy(pResult->szLcdName, pPoiDetailInfo->szLcdName);

    if(pPoiDetailInfo->szMcdName)
        strcpy(pResult->szMcdName, pPoiDetailInfo->szMcdName);

    if(pPoiDetailInfo->szScdName)
        strcpy(pResult->szScdName, pPoiDetailInfo->szScdName);

    if(pPoiDetailInfo->szDcdName)
        strcpy(pResult->szDcdName, pPoiDetailInfo->szDcdName);

    if(pPoiDetailInfo->nMlClass)
        pResult->nMlClass = pPoiDetailInfo->nMlClass;

    if(pPoiDetailInfo->szBldAdd)
        strcpy(pResult->szBldAdd, pPoiDetailInfo->szBldAdd);

    if(pPoiDetailInfo->szRoadName)
        strcpy(pResult->szRoadName, pPoiDetailInfo->szRoadName);

    if(pPoiDetailInfo->szBldNo1)
        strcpy(pResult->szBldNo1, pPoiDetailInfo->szBldNo1);

    if(pPoiDetailInfo->szBldNo2)
        strcpy(pResult->szBldNo2, pPoiDetailInfo->szBldNo2);

    if(pPoiDetailInfo->centerTp.x)
        pResult->centerTp.x = pPoiDetailInfo->centerTp.x;

    if(pPoiDetailInfo->centerTp.y)
        pResult->centerTp.y = pPoiDetailInfo->centerTp.y;

    if(pPoiDetailInfo->bRpFlag)
        pResult->bRpFlag = pPoiDetailInfo->bRpFlag;

    if(pPoiDetailInfo->szZipCd)
        strcpy(pResult->szZipCd, pPoiDetailInfo->szZipCd);

    if(pPoiDetailInfo->szMenu1)
        strcpy(pResult->szMenu1, pPoiDetailInfo->szMenu1);

    if(pPoiDetailInfo->szMenu2)
        strcpy(pResult->szMenu2, pPoiDetailInfo->szMenu2);

    if(pPoiDetailInfo->szMenu3)
        strcpy(pResult->szMenu3, pPoiDetailInfo->szMenu3);

    if(pPoiDetailInfo->szMenu4)
        strcpy(pResult->szMenu4, pPoiDetailInfo->szMenu4);

    if(pPoiDetailInfo->szMenu5)
        strcpy(pResult->szMenu5, pPoiDetailInfo->szMenu5);

    if(pPoiDetailInfo->szTelNo)
        strcpy(pResult->szTelNo, pPoiDetailInfo->szTelNo);

    if(pPoiDetailInfo->szRoad)
        strcpy(pResult->szRoad, pPoiDetailInfo->szRoad);

    if(pPoiDetailInfo->nRecommandPoint)
        pResult->nRecommandPoint = pPoiDetailInfo->nRecommandPoint;

    if(pPoiDetailInfo->szIconType)
        strcpy(pResult->szIconType, pPoiDetailInfo->szIconType);

    if(pPoiDetailInfo->szJoinStoreYn)
        strcpy(pResult->szJoinStoreYn, pPoiDetailInfo->szJoinStoreYn);

    if(pPoiDetailInfo->szJoinViewType)
        strcpy(pResult->szJoinViewType, pPoiDetailInfo->szJoinViewType);

	if(pPoiDetailInfo->szDayOffInfo)
		strcpy(pResult->szDayOffInfo, pPoiDetailInfo->szDayOffInfo);

	if(pPoiDetailInfo->szBusinessHours)
		strcpy(pResult->szBusinessHours, pPoiDetailInfo->szBusinessHours);

	if(pPoiDetailInfo->szDayOffDate)
		strcpy(pResult->szDayOffDate, pPoiDetailInfo->szDayOffDate);

    // sangbong.woo - s
    strncpy(pResult->szAddInfo, pPoiDetailInfo->szAddInfo, D_MAXLEN_DETAILINFO);
    pResult->bHighGasoline = pPoiDetailInfo->m_HighGasoline;
    pResult->m_GatherTime = pPoiDetailInfo->m_GatherTime;
    pResult->nOPIGasolinePrice = pPoiDetailInfo->m_nOPIGasolinePrice;
    pResult->nOPIDieselPrice = pPoiDetailInfo->m_nOPIDieselPrice;
    pResult->nOPILPGPrice = pPoiDetailInfo->m_nOPILPGPrice;
    pResult->nOPIHiGasolinePrice = pPoiDetailInfo->m_nOPIHiGasolinePrice;
    pResult->nOPIHiLPGPrice = pPoiDetailInfo->m_nOPIHiLPGPrice;
    pResult->nDataKind = pPoiDetailInfo->m_dataKind;
    // sangbong.woo - e
	int i=0;
	while(i < D_MAX_POIIMAGEINFO)
	{
		if(pPoiDetailInfo->sPoiImageInfos[i].szFileSrc[0] == NULL)
		{
			break;
		}
		pResult->sPoiImageInfos[i].nSeq = pPoiDetailInfo->sPoiImageInfos[i].nSeq;
		strcpy(pResult->sPoiImageInfos[i].szFileSrc, pPoiDetailInfo->sPoiImageInfos[i].szFileSrc);
		i++;
	}

	i = 0;
	while(i < D_MAX_POIPARKINFO)
	{
		if(pPoiDetailInfo->sPoiParkInfos[i].szCpName[0] == NULL)
		{
			break;
		}
		strcpy(pResult->sPoiParkInfos[i].szCpName, pPoiDetailInfo->sPoiParkInfos[i].szCpName);
		strcpy(pResult->sPoiParkInfos[i].szParkUrl, pPoiDetailInfo->sPoiParkInfos[i].szParkUrl);
		i++;
	}

	return DE_ERROR_SUCCESS;
}

NDDS_API DeErrorCode NDDS_GetSearchPoiStationInfo(DS_SEARCH_POI_STATION_INFO *pResult)
{
    NIF_NDDS_NOT_INITIALIZED_RETURN_FAIL;

    if(pResult == NULL)
    {
        return DE_ERROR_NOT_REQMEMORY;
    }

    int nResult = 0;
    bool bRet;
    API_SEARCH_POISTATIONINFO_RES sResult;

    memset(&sResult, 0x00, sizeof(API_SEARCH_POISTATIONINFO_RES));

    bRet = NIF_Function(eAPI_NDDS_GetSearchPoiStationInfo, NULL, &sResult);
    if(!bRet || sResult.nResult <= 0) return DE_ERROR_FAIL;

    N_SEARCH_POISTATIONINFO *pPoiStationInfo;

    pPoiStationInfo = &sResult.sPoiStationInfo;

    pResult->nPoiID = pPoiStationInfo->nPoiID;
    strcpy(pResult->szName, pPoiStationInfo->szName);
    pResult->tp.x = pPoiStationInfo->tp.x;
    pResult->tp.y = pPoiStationInfo->tp.y;
    pResult->nOPIGasolinePrice = pPoiStationInfo->m_nOPIGasolinePrice;
    pResult->nOPIDieselPrice = pPoiStationInfo->m_nOPIDieselPrice;
    pResult->nOPILPGPrice = pPoiStationInfo->m_nOPILPGPrice;
    pResult->nOPIHiGasolinePrice = pPoiStationInfo->m_nOPIHiGasolinePrice;
    pResult->nOPIHiLPGPrice = pPoiStationInfo->m_nOPIHiLPGPrice;
    pResult->nDataKind = pPoiStationInfo->m_dataKind;
    return DE_ERROR_SUCCESS;
}

NDDS_API DeErrorCode NDDS_GetSearchPoiJoinInfo(int nIdx, int &nCnt, DS_SEARCH_POI_JOIN_INFO *pResult, int *nTotal)
{
	NIF_NDDS_NOT_INITIALIZED_RETURN_FAIL;

	if(pResult == NULL)
	{
		return DE_ERROR_NOT_REQMEMORY;
	}

	int i, nResult, nIdxSat, nIdxMax;
	bool bRet;
	API_SEARCH_POIJOININFO_REQ sRequest;
	API_SEARCH_POIJOININFO_RES sResult;

	memset(&sRequest, 0x00, sizeof(API_SEARCH_POIJOININFO_REQ));
	memset(&sResult, 0x00, sizeof(API_SEARCH_POIJOININFO_RES));

	nResult = 0;
	nIdxSat = nIdx;
	nIdxMax = nIdx + nCnt; // 마지막 인덱스 다음
	while(1)
	{
		sRequest.nIdx = nIdxSat;
		if(nIdxSat + MAX_RESULTLIST <= nIdxMax)
			sRequest.nCnt = MAX_RESULTLIST;
		else
			sRequest.nCnt = nIdxMax - nIdxSat;

		memset(&sResult, 0x00, sizeof(API_SEARCH_POIJOININFO_RES));
		bRet = NIF_Function(eAPI_NDDS_GetSearchPoiJoinInfo, &sRequest, &sResult);
		if(!bRet || sResult.nResult <= 0) break;

		N_SEARCH_POIJOININFO *pPoiJoinInfo;
		for(i=0 ; i<sResult.nResult ; i++)
		{
			pPoiJoinInfo = &sResult.sPoiJoinInfo[i];
			pResult[nResult].nIdx = pPoiJoinInfo->nIdx;

			pResult[nResult].nPoiID = pPoiJoinInfo->nPoiID;
			strcpy(pResult[nResult].szJoinType, pPoiJoinInfo->szJoinType);
			strcpy(pResult[nResult].szName, pPoiJoinInfo->szName);

			strcpy(pResult[nResult].szCashRate, pPoiJoinInfo->szCashRate);
			strcpy(pResult[nResult].szCardRate, pPoiJoinInfo->szCardRate);

			strcpy(pResult[nResult].szWorkTime, pPoiJoinInfo->szWorkTime);
			strcpy(pResult[nResult].szWeekendWorkTime, pPoiJoinInfo->szWeekendWorkTime);

			strcpy(pResult[nResult].szUrl, pPoiJoinInfo->szUrl);
			pResult[nResult].byPark = pPoiJoinInfo->byPark;
			strcpy(pResult[nResult].szBenefit, pPoiJoinInfo->szBenefit);
			strcpy(pResult[nResult].szUserGuide, pPoiJoinInfo->szUseGuide);
			strcpy(pResult[nResult].szInformation, pPoiJoinInfo->szInformation);
			
			pResult[nResult].nItem = pPoiJoinInfo->nItem;
			for(int j=0; j<pPoiJoinInfo->nItem; j++)
			{
				strcpy(pResult[nResult].szItemInfos[j], pPoiJoinInfo->szItemInfos[j]);
			}

			nResult++;
		}
		nIdxSat += sResult.nResult;

		if(nResult >= nCnt)
			break;
	}

	nCnt = nResult;
	if(nTotal)
	{
		*nTotal = sResult.nTotal;
	}

	return DE_ERROR_SUCCESS;
}

NDDS_API DeErrorCode NDDS_GetSearchRecentDestnationList(int nIdx, int &nCnt, DS_SEARCH_RECENT_DESTNATION *pResult, int *nTotal)
{
	NIF_NDDS_NOT_INITIALIZED_RETURN_FAIL;

	if(pResult == NULL)
	{
		return DE_ERROR_NOT_REQMEMORY;
	}
	if(nIdx < 0 || nCnt <= 0)
	{
		return DE_ERROR_INVALID_PARAM;
	}

	int i, nResult, nIdxSat, nIdxMax;
	bool bRet;
	API_SEARCH_RECENTDESTNATION_REQ sRequest;
	API_SEARCH_RECENTDESTNATION_RES sResult;

	memset(&sRequest, 0x00, sizeof(API_SEARCH_RECENTDESTNATION_REQ));
	memset(&sResult, 0x00, sizeof(API_SEARCH_RECENTDESTNATION_RES));

	nResult = 0;
	nIdxSat = nIdx;
	nIdxMax = nIdx + nCnt; // 마지막 인덱스 다음
	while(1)
	{
		sRequest.nIdx = nIdxSat;
		if(nIdxSat + MAX_RESULTLIST <= nIdxMax)
			sRequest.nCnt = MAX_RESULTLIST;
		else
			sRequest.nCnt = nIdxMax - nIdxSat;

		memset(&sResult, 0x00, sizeof(API_SEARCH_RECENTDESTNATION_RES));

		bRet = NIF_Function(eAPI_NDDS_GetSearchRecentDestnationList, &sRequest, &sResult);
		if(!bRet || sResult.nResult <= 0) return DE_ERROR_FAIL;

		for(i=0 ; i<sResult.nResult ; i++)
		{
			N_SEARCH_RECENTDESTNATION *pRecentDestnation;
			pRecentDestnation = &sResult.sRecentDestnationList[i];

			pResult[nResult].nPoiID = pRecentDestnation->nPoiID;
			strcpy(pResult[nResult].szNavSeq, pRecentDestnation->szNavSeq);
			strcpy(pResult[nResult].szName, pRecentDestnation->szName);
			pResult[nResult].tp.x = pRecentDestnation->tp.x;
			pResult[nResult].tp.y = pRecentDestnation->tp.y;
			pResult[nResult].byRPFlag = pRecentDestnation->RPFlag;
			strcpy(pResult[nResult].szSvcDate, pRecentDestnation->szSvcDate);

			nResult++;
		}
		nIdxSat += sResult.nResult;

		if(nResult >= nCnt)
			break;
	}

	nCnt = nResult;
	if(nTotal)
	{
		*nTotal = sResult.nTotal;
	}

	return DE_ERROR_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////
/// 즐겨찾기 관련
//////////////////////////////////////////////////////////////////////////

NDDS_API DeErrorCode NDDS_Favorite(DS_FAVORITE_REQ *pFavoriteReq, bool bSync, int &nResult, int *nSessionID)
{
	NIF_NDDS_NOT_INITIALIZED_RETURN_FAIL;

	if(pFavoriteReq == NULL)
		return DE_ERROR_INVALID_PARAM;

	bool bRet;
	API_FAVORITE_REQ sRequest;
	API_FAVORITE_RES sResult;
	memset(&sRequest, 0x00, sizeof(API_FAVORITE_REQ));
	memset(&sResult, 0x00, sizeof(API_FAVORITE_RES));

	sRequest.bSync = (bool)bSync;

	switch(pFavoriteReq->eFavoriteType)
	{
	case DE_FAVORITETYPE_SERVER:
		{
			DS_FAVORITE_SERVER_REQ &sFavoriteReq = pFavoriteReq->sFavoriteServer;

			FAVORITE_NDDSPOI_INPUT &sFavoriteInput = sRequest.sFavoriteInput.sFavoriteNddsPoi;
			sFavoriteInput.eFavoriteType = N_FAVORITETYPE_NDDSPOI;
			sFavoriteInput.bMyFavoriteYn = sFavoriteReq.bMyFavorite;
		}
		break;
    case DE_FAVORITETYPE_SERVER_REGIST:
        {
            DBGPRINT(_T("DE_FAVORITETYPE_SERVER_REGIST #1 \n"));
            DS_FAVORITE_REGIST_REQ &sRegistFavoriteReq = pFavoriteReq->sRegistFavoriteServer;

            FAVORITE_NDDSPOI_REGIST_INPUT &sRegistFavoriteInput = sRequest.sFavoriteInput.sRegistFavoriteNddsPoi;
            sRegistFavoriteInput.eFavoriteType = N_FAVORITETYPE_NDDSREGISTPOI;
            sRegistFavoriteInput.nPoiCnt = pFavoriteReq->sRegistFavoriteServer.nPoiRegistCnt;

            for(int i=0;i<pFavoriteReq->sRegistFavoriteServer.nPoiRegistCnt;i++)
            {
                strcpy(sRegistFavoriteInput.sRegistFavoriteList[i].szCustName, pFavoriteReq->sRegistFavoriteServer.sRegistFavoriteInfo[i].szCustName);
                sRegistFavoriteInput.sRegistFavoriteList[i].tp.x = (int)(pFavoriteReq->sRegistFavoriteServer.sRegistFavoriteInfo[i].tp.x);// / (256*2048.0) * 36000.0f + 0.5f);;
                sRegistFavoriteInput.sRegistFavoriteList[i].tp.y = (int)(pFavoriteReq->sRegistFavoriteServer.sRegistFavoriteInfo[i].tp.y);// / (256*2048.0) * 36000.0f + 0.5f);;
                strcpy(sRegistFavoriteInput.sRegistFavoriteList[i].szNavSeq, pFavoriteReq->sRegistFavoriteServer.sRegistFavoriteInfo[i].szNavSeq);
                sRegistFavoriteInput.sRegistFavoriteList[i].byRPFlag = pFavoriteReq->sRegistFavoriteServer.sRegistFavoriteInfo[i].byRPFlag;
                strcpy(sRegistFavoriteInput.sRegistFavoriteList[i].szAddInfo, pFavoriteReq->sRegistFavoriteServer.sRegistFavoriteInfo[i].szAddInfo);
                strcpy(sRegistFavoriteInput.sRegistFavoriteList[i].szOrgCustName, pFavoriteReq->sRegistFavoriteServer.sRegistFavoriteInfo[i].szOrgCustName);
                strcpy(sRegistFavoriteInput.sRegistFavoriteList[i].szIconInfo, pFavoriteReq->sRegistFavoriteServer.sRegistFavoriteInfo[i].szIconInfo);
            }
        }
        break;
    case DE_FAVORITETYPE_SERVER_UPLOAD:
        {
            DBGPRINT(_T("DE_FAVORITETYPE_SERVER_UPLOAD #1 \n"));

            FAVORITE_NDDSPOI_UPLOAD_INPUT &sUploadFavoriteInput = sRequest.sFavoriteInput.sUploadFavoriteNddsPoi;
            strcpy(sUploadFavoriteInput.szUploadOption, pFavoriteReq->sUploadFavoriteServer.szUploadOption);
            sUploadFavoriteInput.nPoiCnt = pFavoriteReq->sUploadFavoriteServer.nUploadPoiCnt;
            sUploadFavoriteInput.eFavoriteType = N_FAVORITETYPE_NDDSUPLOADPOI;

            if(pFavoriteReq->sUploadFavoriteServer.bMyFavorite)
            {
                strcpy(sUploadFavoriteInput.sUploadMyFavorite.szHomeName, pFavoriteReq->sUploadFavoriteServer.sUploadpoiMyFavorite.szHomeCustName);
                sUploadFavoriteInput.sUploadMyFavorite.Hometp.x = (int)(pFavoriteReq->sUploadFavoriteServer.sUploadpoiMyFavorite.Hometp.x);// / (256*2048.0) * 36000.0f + 0.5f);
                sUploadFavoriteInput.sUploadMyFavorite.Hometp.y = (int)(pFavoriteReq->sUploadFavoriteServer.sUploadpoiMyFavorite.Hometp.y);// / (256*2048.0) * 36000.0f + 0.5f);
                sUploadFavoriteInput.sUploadMyFavorite.nHomePoiID, pFavoriteReq->sUploadFavoriteServer.sUploadpoiMyFavorite.nHomePoiID;
                sUploadFavoriteInput.sUploadMyFavorite.byHomeRpFlag, pFavoriteReq->sUploadFavoriteServer.sUploadpoiMyFavorite.byHomeRpFlag;
                strcpy(sUploadFavoriteInput.sUploadMyFavorite.szHomeAddInfo, pFavoriteReq->sUploadFavoriteServer.sUploadpoiMyFavorite.szHomeAddInfo);
                strcpy(sUploadFavoriteInput.sUploadMyFavorite.szOfficeName, pFavoriteReq->sUploadFavoriteServer.sUploadpoiMyFavorite.szOfficeCustName);
                sUploadFavoriteInput.sUploadMyFavorite.Officetp.x = (int)(pFavoriteReq->sUploadFavoriteServer.sUploadpoiMyFavorite.Officetp.x);// / (256*2048.0) * 36000.0f + 0.5f);
                sUploadFavoriteInput.sUploadMyFavorite.Officetp.y = (int)(pFavoriteReq->sUploadFavoriteServer.sUploadpoiMyFavorite.Officetp.y);// / (256*2048.0) * 36000.0f + 0.5f);

                sUploadFavoriteInput.sUploadMyFavorite.nOfficePoiID = pFavoriteReq->sUploadFavoriteServer.sUploadpoiMyFavorite.nOfficePoiID;
                sUploadFavoriteInput.sUploadMyFavorite.byOfficeRpFlag = pFavoriteReq->sUploadFavoriteServer.sUploadpoiMyFavorite.byOfficeRpFlag;
                strcpy(sUploadFavoriteInput.sUploadMyFavorite.szOfficeAddInfo, pFavoriteReq->sUploadFavoriteServer.sUploadpoiMyFavorite.szOfficeAddInfo);
            }

            for(int i=0;i<sUploadFavoriteInput.nPoiCnt;i++)
            {
                strcpy(sUploadFavoriteInput.sUploadFavoriteList[i].szCustName, pFavoriteReq->sUploadFavoriteServer.sUploadFavoriteInfo[i].szCustName);
                sUploadFavoriteInput.sUploadFavoriteList[i].tp.x = (int)(pFavoriteReq->sUploadFavoriteServer.sUploadFavoriteInfo[i].tp.x);// / (256*2048.0) * 36000.0f + 0.5f);
                sUploadFavoriteInput.sUploadFavoriteList[i].tp.y = (int)(pFavoriteReq->sUploadFavoriteServer.sUploadFavoriteInfo[i].tp.y);// / (256*2048.0) * 36000.0f + 0.5f);
                sUploadFavoriteInput.sUploadFavoriteList[i].nPoiID = pFavoriteReq->sUploadFavoriteServer.sUploadFavoriteInfo[i].nPoiID;
                sUploadFavoriteInput.sUploadFavoriteList[i].byRPFlag = pFavoriteReq->sUploadFavoriteServer.sUploadFavoriteInfo[i].byRPFlag;

                strcpy(sUploadFavoriteInput.sUploadFavoriteList[i].szAddInfo, pFavoriteReq->sUploadFavoriteServer.sUploadFavoriteInfo[i].szAddInfo);
                strcpy(sUploadFavoriteInput.sUploadFavoriteList[i].szOrgCustName, pFavoriteReq->sUploadFavoriteServer.sUploadFavoriteInfo[i].szOrgCustName);
                strcpy(sUploadFavoriteInput.sUploadFavoriteList[i].szInsDatetime, pFavoriteReq->sUploadFavoriteServer.sUploadFavoriteInfo[i].szInsDatetime);
                strcpy(sUploadFavoriteInput.sUploadFavoriteList[i].szNavSeq, pFavoriteReq->sUploadFavoriteServer.sUploadFavoriteInfo[i].szNavSeq);
            }
        }
        break;
    case DE_FAVORITETYPE_SERVER_REMOVE:
        {
            DBGPRINT(_T("DE_FAVORITETYPE_SERVER_REMOVE #1 \n"));
            DS_FAVORITE_REMOVE_REQ &sRemoveFavoriteReq = pFavoriteReq->sRemoveFavoriteServer;

            FAVORITE_NDDSPOI_REMOVE_INPUT &sRemoveFavoriteInput = sRequest.sFavoriteInput.sRemoveFavoriteNddsPoi;
            sRemoveFavoriteInput.eFavoriteType = N_FAVORITETYPE_NDDSREMOVEPOI;
            sRemoveFavoriteInput.nPoiCnt = sRemoveFavoriteReq.nRemovePoiRecentCnt;
            for(int i=0;i<sRemoveFavoriteInput.nPoiCnt;i++){
                strcpy(sRemoveFavoriteInput.sRemoveFavoriteList[i].szCustName, sRemoveFavoriteReq.sRemoveFavoriteInfo[i].szCustName);
                sRemoveFavoriteInput.sRemoveFavoriteList[i].tp.x =  sRemoveFavoriteReq.sRemoveFavoriteInfo[i].tp.x;
                sRemoveFavoriteInput.sRemoveFavoriteList[i].tp.y =  sRemoveFavoriteReq.sRemoveFavoriteInfo[i].tp.y;
            }
        }
        break;
    case DE_FAVORITETYPE_SERVER_MODIFY:
        {
            DBGPRINT(_T("DE_FAVORITETYPE_SERVER_MODIFY #1 \n"));
            DS_FAVORITE_MODIFY_REQ &sModifyFavoriteReq = pFavoriteReq->sModifyFavoriteServer;

            FAVORITE_NDDSPOI_MODIFY_INPUT &sModifyFavoriteInput = sRequest.sFavoriteInput.sModifyFavoriteNddsPoi;
            sModifyFavoriteInput.eFavoriteType = N_FAVORITETYPE_NDDSMODIFYPOI;
            strcpy(sModifyFavoriteInput.sModifyFavorite.szCustName, sModifyFavoriteReq.sModifyFavoriteInfo.szCustName);
            strcpy(sModifyFavoriteInput.sModifyFavorite.szNewCustName, sModifyFavoriteReq.sModifyFavoriteInfo.szNewCustName);
            sModifyFavoriteInput.sModifyFavorite.tp.x =  sModifyFavoriteReq.sModifyFavoriteInfo.tp.x;
            sModifyFavoriteInput.sModifyFavorite.tp.y =  sModifyFavoriteReq.sModifyFavoriteInfo.tp.y;
            strcpy(sModifyFavoriteInput.sModifyFavorite.szIconInfo, sModifyFavoriteReq.sModifyFavoriteInfo.szIconInfo);
        }
        break;
	case DE_FAVORITETYPE_REGISTERSENDTOCARINFO:
		{
			DS_FAVORITE_REGISTERSENDTOCARINFO_REQ &sFavoriteReq = pFavoriteReq->sFavoriteRegistSendToCarInfo;

			FAVORITE_NDDSREGISTERSENDTOCARINFO_INPUT &sFavoriteInput = sRequest.sFavoriteInput.sFavoriteNddsRegiSendToCarInfo;
			sFavoriteInput.eFavoriteType = N_FAVORITETYPE_NDDSREGISTERSENDTOCARINFO;
			strcpy(sFavoriteInput.szCustName, sFavoriteReq.szCustName);
			sFavoriteInput.tp.x = sFavoriteReq.tp.x;
			sFavoriteInput.tp.y = sFavoriteReq.tp.y;
			sFavoriteInput.nPoiID = sFavoriteReq.nPoiID;
			sFavoriteInput.RPFlag = sFavoriteReq.byRPFlag;
			strcpy(sFavoriteInput.navSeq, sFavoriteReq.szNavSeq);
		}
		break;
	case DE_FAVORITETYPE_FINDSENDTOCARINFO:
		{
			DS_FAVORITE_FINDSENDTOCARINFO_REQ &sFavoriteReq = pFavoriteReq->sFavoriteFindSendToCarInfo;

			FAVORITE_NDDSFINDSENDTOCARINFO_INPUT &sFavoriteInput = sRequest.sFavoriteInput.sFavoriteNddsFindSendToCarInfo;
			sFavoriteInput.eFavoriteType = N_FAVORITETYPE_NDDSFINDSENDTOCARINFO;

			sFavoriteInput.bNewAddr = sFavoriteReq.bNewAddr;
		}
		break;
	case DE_FAVORITETYPE_REMOVESENDTOCAR:
		{
			DS_FAVORITE_REMOVESENDTOCARINFO_REQ &sFavoriteReq = pFavoriteReq->sFavoriteRemoveSendToCarInfo;

			FAVORITE_NDDSREMOVESENDTOCARINFO_INPUT &sFavoriteInput = sRequest.sFavoriteInput.sFavoriteNddsRemoveSendToCarInfo;
			sFavoriteInput.eFavoriteType = N_FAVORITETYPE_NDDSREMOVESENDTOCARINFO;
			strcpy(sFavoriteInput.szCustName, sFavoriteReq.szCustName);
			sFavoriteInput.tp.x = sFavoriteReq.tp.x;
			sFavoriteInput.tp.y = sFavoriteReq.tp.y;
        }
		break;
    case DE_FAVORITETYPE_USERPROFILE_FIND:
        {
            DBGPRINT(_T("DE_FAVORITETYPE_USERPROFILE_FIND #1 \n"));
            DS_FAVORITE_FINDUSERPROFILE_REQ &sFavoriteReq = pFavoriteReq->sFavoriteFindUserProfile;

            FAVORITE_NDDSFINDUSERPROFILE_INPUT &sFavoriteInput = sRequest.sFavoriteInput.sFavoriteNddsFindUserProfile;
            sFavoriteInput.eFavoriteType = N_FAVORITETYPE_NDDSFINDUSERPROFILE;
        }
        break;
    case DE_FAVORITETYPE_USERPROFILE_REGIST:
        {
            DBGPRINT(_T("DE_FAVORITETYPE_USERPROFILE_REGIST #1 \n"));
            DS_FAVORITE_REGISTUSERPROFILE_REQ &sFavoriteReq = pFavoriteReq->sFavoriteRegistUserProfile;

            FAVORITE_NDDSREGISTUSERPROFILE_INPUT &sFavoriteInput = sRequest.sFavoriteInput.sFavoriteNddsRegistUserProfile;
            sFavoriteInput.eFavoriteType = N_FAVORITETYPE_NDDSREGISTUSERPROFILE;
            strcpy(sFavoriteInput.szEmail, sFavoriteReq.szEmail);
            strcpy(sFavoriteInput.szUserName, sFavoriteReq.szUserName);
            strcpy(sFavoriteInput.szMdn, sFavoriteReq.szMdn);
            strcpy(sFavoriteInput.szBirth, sFavoriteReq.szBirth);
        }
        break;
    case DE_FAVORITETYPE_CARPROFILE_FIND:
        {
            DBGPRINT(_T("DE_FAVORITETYPE_CARPROFILE_FIND #1 \n"));
			DS_FAVORITE_FINDCARPROFILE_REQ &sFavoriteReq = pFavoriteReq->sFavoriteFindCarProfile;

			FAVORITE_NDDSFINDCARPROFILE_INPUT &sFavoriteInput = sRequest.sFavoriteInput.sFavoriteNddsFindCarProfile;
			sFavoriteInput.eFavoriteType = N_FAVORITETYPE_NDDSFINDCARPROFILE;
        }
        break;
    case DE_FAVORITETYPE_CARPROFILE_REGIST:
        {
            DBGPRINT(_T("DE_FAVORITETYPE_CARPROFILE_REGIST #1 \n"));
            DS_FAVORITE_REGISTCARPROFILE_REQ &sFavoriteReq = pFavoriteReq->sFavoriteRegistCarProfile;

            FAVORITE_NDDSREGISTCARPROFILE_INPUT &sFavoriteInput = sRequest.sFavoriteInput.sFavoriteNddsRegistCarProfile;
            sFavoriteInput.eFavoriteType = N_FAVORITETYPE_NDDSREGISTCARPROFILE;
            strcpy(sFavoriteInput.szModel, sFavoriteReq.szModel);
            strcpy(sFavoriteInput.szFuel, sFavoriteReq.szFuel);
            strcpy(sFavoriteInput.szHipassYn, sFavoriteReq.szHipassYn);
            strcpy(sFavoriteInput.szNumber, sFavoriteReq.szNumber);
        }
        break;
    case DE_FAVORITETYPE_USERDATA_FIND:
        {
            DBGPRINT(_T("DE_FAVORITETYPE_CARPROFILE_FIND #1 \n"));
            DS_FAVORITE_FINDCARPROFILE_REQ &sFavoriteReq = pFavoriteReq->sFavoriteFindUserData;

            FAVORITE_NDDSFINDUSERDATA_INPUT &sFavoriteInput = sRequest.sFavoriteInput.sFavoriteNddsFindUserData;
            sFavoriteInput.eFavoriteType = N_FAVORITETYPE_NDDSFINDUSERDATA;
        }
        break;
	default:
		{
			return DE_ERROR_FAVORITE_TYPE;
		}
		break;
	}

	bRet = NIF_Function(eAPI_NDDS_Favorite, &sRequest, &sResult);
	if(!bRet)
	{
		return DE_ERROR_FAIL;
	}

	if(sResult.nResult > 250)
		nResult = 250;
	else
		nResult = sResult.nResult;

	if(sResult.nResult <= 0 && *sResult.szErrorMsg)
	{
		//return DE_ERROR_FAVORITE_SERVER;
	}

	if(nSessionID)
		*nSessionID = sResult.nSessionID;

	return DE_ERROR_SUCCESS;
}

NDDS_API DeErrorCode NDDS_GetFavoriteResultList(DeFavoriteType eFavoriteType, int nIdx, int &nCnt, DS_FAVORITE_LIST *pResult, int *nTotal)
{
	NIF_NDDS_NOT_INITIALIZED_RETURN_FAIL;

	if(pResult == NULL)
	{
		return DE_ERROR_NOT_REQMEMORY;
	}
	if(nIdx < 0 || nCnt <= 0)
	{
		return DE_ERROR_INVALID_PARAM;
	}

	int i, nResult, nIdxSat, nIdxMax;
	bool bRet;
	API_FAVORITE_RESULTLIST_REQ sRequest;
	API_FAVORITE_RESULTLIST_RES sResult;

	memset(&sRequest, 0x00, sizeof(API_FAVORITE_RESULTLIST_REQ));
	memset(&sResult, 0x00, sizeof(API_FAVORITE_RESULTLIST_RES));

	switch(eFavoriteType)
	{
	case DE_FAVORITETYPE_SERVER:
		sRequest.eFavoriteType = N_FAVORITETYPE_NDDSPOI; break;
		default:
		return DE_ERROR_FAVORITE_TYPE;
		break;
	}

	nResult = 0;
	nIdxSat = nIdx;
	nIdxMax = nIdx + nCnt; // 마지막 인덱스 다음
	while(1)
	{
		sRequest.nIdx = nIdxSat;
		if(nIdxSat + MAX_RESULTLIST <= nIdxMax)
			sRequest.nCnt = MAX_RESULTLIST;
		else
			sRequest.nCnt = nIdxMax - nIdxSat;

		memset(&sResult, 0x00, sizeof(API_FAVORITE_RESULTLIST_RES));
		bRet = NIF_Function(eAPI_NDDS_GetFavoriteResultList, &sRequest, &sResult);
		if(!bRet || sResult.nResult <= 0) break;

		N_FAVORITE_LIST *pList;
		for(i=0 ; i<sResult.nResult ; i++)
		{
			pList = &sResult.sList[i];
			pResult[nResult].nIdx = pList->nIdx;

			strcpy(pResult[nResult].szCustName, pList->szCustName);

			pResult[nResult].nPoiID = pList->nPoiID;
			pResult[nResult].byRPFlag = pList->RPFlag;
			pResult[nResult].tp.x = pList->tp.x;
			pResult[nResult].tp.y = pList->tp.y;

			strcpy(pResult[nResult].szAddInfo, pList->szAddInfo);
			strcpy(pResult[nResult].szNavSeq, pList->szNavSeq);
			strcpy(pResult[nResult].szInsDatetime, pList->szInsDatetime);
			nResult++;
		}
		nIdxSat += sResult.nResult;

		if(nResult >= nCnt)
			break;
	}

	nCnt = nResult;
	if(nTotal)
	{
		*nTotal = sResult.nTotal;
	}

	return DE_ERROR_SUCCESS;
}

NDDS_API DeErrorCode NDDS_GetMyFavorite(DS_MYFAVORITE *pResult)
{
	NIF_NDDS_NOT_INITIALIZED_RETURN_FAIL;

	if(pResult == NULL)
	{
		return DE_ERROR_NOT_REQMEMORY;
	}

	bool bRet;
	API_MYFAVORITE_RES sResult;

	memset(&sResult, 0x00, sizeof(API_MYFAVORITE_RES));

	bRet = NIF_Function(eAPI_NDDS_GetMyFavorite, NULL, &sResult);
	if(!bRet) return DE_ERROR_FAIL;

	strcpy(pResult->szHomeCustName, sResult.szHomeCustName);
	pResult->nHomePoiID = sResult.nHomePoiId;
	pResult->byHomeRpFlag = sResult.byHomeRpFlag;
	pResult->Hometp.x = sResult.Hometp.x;
	pResult->Hometp.y = sResult.Hometp.y;
	strcpy(pResult->szHomeAddInfo, sResult.szHomeAddInfo);
	strcpy(pResult->szHomeNavSeq, sResult.szHomeNavSeq);
	strcpy(pResult->szHomeInsDatetime, sResult.szHomeInsDatetime);

	strcpy(pResult->szOfficeCustName, sResult.szOfficeCustName);
	pResult->nOfficePoiID = sResult.nOfficePoiId;
	pResult->byOfficeRpFlag = sResult.byOfficeRpFlag;
	pResult->Officetp.x = sResult.Officetp.x;
	pResult->Officetp.y = sResult.Officetp.y;
	strcpy(pResult->szOfficeAddInfo, sResult.szOfficeAddInfo);
	strcpy(pResult->szOfficeNavSeq, sResult.szOfficeNavSeq);
	strcpy(pResult->szOfficeInsDatetime, sResult.szOfficeInsDatetime);

	return DE_ERROR_SUCCESS;
}

NDDS_API DeErrorCode NDDS_GetSendToCarInfo(DS_SENDTOCARINFO *pResult)
{
	NIF_NDDS_NOT_INITIALIZED_RETURN_FAIL;

	if(pResult == NULL)
	{
		return DE_ERROR_NOT_REQMEMORY;
	}

	bool bRet;
	API_SENDTOCARINFO_RES sResult;

	memset(&sResult, 0x00, sizeof(API_SENDTOCARINFO_RES));

	bRet = NIF_Function(eAPI_NDDS_GetSendToCarInfo, NULL, &sResult);
	if(!bRet) return DE_ERROR_FAIL;

	strcpy(pResult->szAddrName, sResult.szAddrname);
	strcpy(pResult->szCustName, sResult.szCustName);
	strcpy(pResult->szInsDatetime, sResult.szInsDatetime);
	strcpy(pResult->szNavSeq, sResult.szNavSeq);
	strcpy(pResult->szPhoneNumber, sResult.szTell);
	pResult->nPoiID = sResult.nPoiId;
	pResult->tp.x = sResult.tp.x;
	pResult->tp.y = sResult.tp.y;
	pResult->byRPFlag = sResult.byRPFlag;
	
	return DE_ERROR_SUCCESS;
}

NDDS_API DeErrorCode NDDS_VmsGuideInfo(char *pRseId, bool bSync, int *nSessionID)
{
	NIF_NDDS_NOT_INITIALIZED_RETURN_FAIL;

	API_VMSGUIDEINFO_REQ sRequest;
	memset(&sRequest, 0x00, sizeof(API_VMSGUIDEINFO_REQ));
	API_VMSGUIDEINFO_RES sResult;
	memset(&sResult, 0x00, sizeof(API_VMSGUIDEINFO_RES));

	strcpy(sRequest.szRseId, pRseId);
	sRequest.bSync = bSync;

	bool bRet = NIF_Function(eAPI_NDDS_VmsGuideInfo, &sRequest, &sResult);
	if(!bRet) return DE_ERROR_FAIL;

	if(nSessionID)
		*nSessionID = sResult.nSessionID;

	return DE_ERROR_SUCCESS;
}

NDDS_API DeErrorCode NDDS_GetVmsGuideInfo(DS_VMSGUIDEINFO *pResult)
{
	NIF_NDDS_NOT_INITIALIZED_RETURN_FAIL;

	API_GET_VMSGUIDEINFO_RES sResult;
	memset(&sResult, 0x00, sizeof(API_GET_VMSGUIDEINFO_RES));

	bool bRet = NIF_Function(eAPI_NDDS_GetVmsGuideInfo, NULL, &sResult);
	if(!bRet) return DE_ERROR_FAIL;

	strcpy(pResult->szBaseSdttm, sResult.szBaseSdttm);
	strcpy(pResult->szDataSource, sResult.szDataSource);
	strcpy(pResult->szContentCode, sResult.szContentCode);
	strcpy(pResult->szContentName, sResult.szContentName);
	strcpy(pResult->szContent, sResult.szContent);

	return DE_ERROR_SUCCESS;
}

NDDS_API DeErrorCode NDDS_AuthTmapForSmartCar(char *pOtpCode, bool bSync, int *nSessionID)
{
	NIF_NDDS_NOT_INITIALIZED_RETURN_FAIL;

	API_AUTHTMAPFORSMARTCAR_REQ sRequest;
	memset(&sRequest, 0x00, sizeof(API_AUTHTMAPFORSMARTCAR_REQ));
	API_AUTHTMAPFORSMARTCAR_RES sResult;
	memset(&sResult, 0x00, sizeof(API_AUTHTMAPFORSMARTCAR_RES));

	sRequest.bSync = bSync;
	if(strlen(pOtpCode) > 6)
		return DE_ERROR_AUTH_OTPCODE_MAX;

	if(pOtpCode != NULL)
		strcpy(sRequest.szOtpCode, pOtpCode);

	bool bRet = NIF_Function(eAPI_NDDS_AuthTmapForSmartCar, &sRequest, &sResult);
	if(!bRet) return DE_ERROR_FAIL;

	if(nSessionID)
		*nSessionID = sResult.nSessionID;

	return DE_ERROR_SUCCESS;
}

NDDS_API DeErrorCode NDDS_GetAuthResult(DS_AUTHRESULT *pResult)
{
	NIF_NDDS_NOT_INITIALIZED_RETURN_FAIL;

	API_GET_AUTHRESULT_RES sResult;
	memset(&sResult, 0x00, sizeof(API_GET_AUTHRESULT_RES));

	bool bRet = NIF_Function(eAPI_NDDS_GetAuthResult, NULL, &sResult);
	if(!bRet) return DE_ERROR_FAIL;

	strcpy(pResult->szAccessKey, sResult.szAccessKey);

	strcpy(pResult->sCommInfo.szEuk,sResult.sCommInfo.szEuk);
	strcpy(pResult->sCommInfo.szEid,sResult.sCommInfo.szEid);

	pResult->sAuthInfo.nValidateCode = sResult.sAuthInfo.nValidateCode;
	strcpy(pResult->sAuthInfo.szValidationSubField,sResult.sAuthInfo.szValidationSubField);
	if(sResult.sAuthInfo.nExternalUserInfoCount > 6)
	{
		pResult->sAuthInfo.nExternalUserInfoCount = 6;
		ASSERT(0);
	}
	else
		pResult->sAuthInfo.nExternalUserInfoCount = sResult.sAuthInfo.nExternalUserInfoCount;
	for(int i=0; i<pResult->sAuthInfo.nExternalUserInfoCount; i++)
	{
		strcpy(pResult->sAuthInfo.sExternalUserInfos[i].szProvider, sResult.sAuthInfo.sExternalUserInfos[i].szProvider);
		strcpy(pResult->sAuthInfo.sExternalUserInfos[i].szReadableUserId, sResult.sAuthInfo.sExternalUserInfos[i].szReadableUserId);
	}

	pResult->sHeadUnitDeviceAuthInfo.nResultCode = sResult.sHeadUnitDeviceAuthInfo.nResultCode;
	pResult->sHeadUnitDeviceAuthInfo.nResultSubField = sResult.sHeadUnitDeviceAuthInfo.nResultSubField;

	pResult->sTidAuthResInfo.nResultCode = sResult.sTidAuthResInfo.nResultCode;
	pResult->sTidAuthResInfo.nResultSubField = sResult.sTidAuthResInfo.nResultSubField;
	pResult->sTidAuthResInfo.nResultMdnAuthInfo = sResult.sTidAuthResInfo.nResultMdnAuthInfo;
	if(sResult.sTidAuthResInfo.nExternalUserInfoCount > 6)
	{
		pResult->sTidAuthResInfo.nExternalUserInfoCount = 6;
		ASSERT(0);
	}
	else
		pResult->sTidAuthResInfo.nExternalUserInfoCount = sResult.sTidAuthResInfo.nExternalUserInfoCount;
	for(int i=0; i<pResult->sTidAuthResInfo.nExternalUserInfoCount; i++)
	{
		strcpy(pResult->sTidAuthResInfo.sExternalUserInfos[i].szProvider, sResult.sTidAuthResInfo.sExternalUserInfos[i].szProvider);
		strcpy(pResult->sTidAuthResInfo.sExternalUserInfos[i].szReadableUserId, sResult.sTidAuthResInfo.sExternalUserInfos[i].szReadableUserId);
	}

	strcpy(pResult->sVersionInfo.szMenuNoticeVer,sResult.sVersionInfo.szMenuNoticeVer);
	strcpy(pResult->sVersionInfo.szOpenappVer,sResult.sVersionInfo.szOpenappVer);

	strcpy(pResult->sAppUpdateInfo.szAppUpdateType,sResult.sAppUpdateInfo.szAppUpdateType);
	strcpy(pResult->sAppUpdateInfo.szAppVer,sResult.sAppUpdateInfo.szAppVer);
	strcpy(pResult->sAppUpdateInfo.szAppUpdateViewFlag,sResult.sAppUpdateInfo.szAppUpdateViewFlag);
	strcpy(pResult->sAppUpdateInfo.szAppUpdateUrl,sResult.sAppUpdateInfo.szAppUpdateUrl);
	strcpy(pResult->sAppUpdateInfo.szAppUpdateMsg,sResult.sAppUpdateInfo.szAppUpdateMsg);

	if(sResult.sAppControlInfo.nControlInfoCount > 6)
	{
		pResult->sAppControlInfo.nControlInfoCount = 6;
		ASSERT(0);
	}
	else
		pResult->sAppControlInfo.nControlInfoCount = sResult.sAppControlInfo.nControlInfoCount;
	for(int i=0; i<pResult->sAppControlInfo.nControlInfoCount; i++)
	{
		strcpy(pResult->sAppControlInfo.sControlInfoDetails[i].szServiceName, sResult.sAppControlInfo.sControlInfoDetails[i].szServiceName);
		strcpy(pResult->sAppControlInfo.sControlInfoDetails[i].szServiceYn, sResult.sAppControlInfo.sControlInfoDetails[i].szServiceYn);
		strcpy(pResult->sAppControlInfo.sControlInfoDetails[i].szServiceItems, sResult.sAppControlInfo.sControlInfoDetails[i].szServiceItems);
	}

	strcpy(pResult->sPushInfo.szPushSetYn, sResult.sPushInfo.szPushSetYn);
	strcpy(pResult->sPushInfo.szSmsYn, sResult.sPushInfo.szSmsYn);
	strcpy(pResult->sPushInfo.szMomentYn, sResult.sPushInfo.szMomentYn);

	strcpy(pResult->sUserProfileInfo.sPersonalInfo.szEmail, sResult.sUserProfileInfo.sPersonalInfo.szEmail);
	strcpy(pResult->sUserProfileInfo.sPersonalInfo.szUserName, sResult.sUserProfileInfo.sPersonalInfo.szUserName);
	strcpy(pResult->sUserProfileInfo.sPersonalInfo.szMdn, sResult.sUserProfileInfo.sPersonalInfo.szMdn);
	strcpy(pResult->sUserProfileInfo.sPersonalInfo.szBirth, sResult.sUserProfileInfo.sPersonalInfo.szEmail);
	strcpy(pResult->sUserProfileInfo.sCarInfo.szModel, sResult.sUserProfileInfo.sCarInfo.szModel);
	strcpy(pResult->sUserProfileInfo.sCarInfo.szFuel, sResult.sUserProfileInfo.sCarInfo.szFuel);
	strcpy(pResult->sUserProfileInfo.sCarInfo.szHipassYn, sResult.sUserProfileInfo.sCarInfo.szHipassYn);
	strcpy(pResult->sUserProfileInfo.sCarInfo.szNumber, sResult.sUserProfileInfo.sCarInfo.szNumber);

	return DE_ERROR_SUCCESS;
}

NDDS_API DeErrorCode NDDS_SetAuthAccessKey(char *pAccessKey)
{
	NIF_NDDS_NOT_INITIALIZED_RETURN_FAIL;

	API_SET_AUTHACCESSKEY_REQ sRequest;
	memset(&sRequest, 0x00, sizeof(API_SET_AUTHACCESSKEY_REQ));
	API_SET_AUTHACCESSKEY_RES sResult;
	memset(&sResult, 0x00, sizeof(API_SET_AUTHACCESSKEY_RES));

	strcpy(sRequest.szAccessKey, pAccessKey);

	bool bRet = NIF_Function(eAPI_NDDS_SetAuthAccessKey, &sRequest, &sResult);
	if(!bRet) return DE_ERROR_FAIL;

	return DE_ERROR_SUCCESS;
}

NDDS_API DeErrorCode NDDS_SetReqDataSave(bool bLogSave, const char* pszPath)
{
    NIF_NDDS_NOT_INITIALIZED_RETURN_FAIL;

    API_SETREQDATASAVE_REQ sRequest;
    memset(&sRequest, 0x00, sizeof(API_SETREQDATASAVE_REQ));

    sRequest.bLogSave = bLogSave;
    if(pszPath != NULL)
        strcpy(sRequest.szLogSavePath, pszPath);

    bool bRet = NIF_Function(eAPI_NDDS_SetReqDataSave, &sRequest, NULL);
    if(!bRet) return DE_ERROR_FAIL;

    return DE_ERROR_SUCCESS;
}

// AutoComplete, R-Geocode, ComplexImage, AQI. for vNext. 2019.8.8 by hheo
NDDS_API DeErrorCode NDDS_NonNdds(bool bSync, const char* pszUrl, const char* pszDomain, char* pszResult, int &nResult)
{
    NIF_NDDS_NOT_INITIALIZED_RETURN_FAIL;

    API_NONNDDS_REQ sRequest;
    memset(&sRequest, 0x00, sizeof(API_NONNDDS_REQ));

    if (pszResult == NULL)
        return DE_ERROR_OUTPARAM_IS_NULL;

    sRequest.bSync = bSync;
    if (pszUrl != NULL)
        strcpy(sRequest.szUrl, pszUrl);

    if (pszDomain != NULL)
        strcpy(sRequest.szDomain, pszDomain);

    API_NONNDDS_RES sResponse;
    memset(&sResponse, 0x00, sizeof(API_NONNDDS_RES));
    sResponse.pszResult = pszResult;

    bool bRet = NIF_Function(eAPI_NDDS_NonNdds, &sRequest, &sResponse);
    if(!bRet)
        return DE_ERROR_NONNDDS_FAIL;

    nResult = sResponse.nResult;

    return DE_ERROR_SUCCESS;
}

NDDS_API void NDDS_SetLogFunction(LPCALL_NDDS_LOG pLogFunction)
{
	SetNddsLogFunction(pLogFunction);
}

//////////////////////////////////////////////////////////////////////////
/// 내부 함수
//////////////////////////////////////////////////////////////////////////

unsigned int ChangeNddsRouteOption(int nRouteOption)
{
	unsigned int uMultiOption = 0;
	if(nRouteOption & DE_ROUTEOPTION_OPTIMUM)
	{
		uMultiOption |= N_MULTIROUTE_BASIC_OPTIMUM;	// 최적(추천)
	}
	if(nRouteOption & DE_ROUTEOPTION_EXPRESSWAY)
	{
		uMultiOption |= N_MULTIROUTE_BASIC_EXPRESSWAY;	// 고속도로우선
	}
	if(nRouteOption & DE_ROUTEOPTION_FREEWAY)
	{
		uMultiOption |= N_MULTIROUTE_FEE_MINIMAL;		// 최소 요금(무료)
	}
	if(nRouteOption & DE_ROUTEOPTION_SHORTESTWAY)
	{
		uMultiOption |= N_MULTIROUTE_BASIC_SHORTESTWAY;	// 최단
	}
	if(nRouteOption & DE_ROUTEOPTION_EASY)
	{
		uMultiOption |= N_MULTIROUTE_BASIC_EASY;			// 초보자(직진/대로우선)
	}
	if(nRouteOption & DE_ROUTEOPTION_SHORTTIME)
	{
		uMultiOption |= N_MULTIROUTE_BASIC_SHORTTIME;		// 최소시간
	}
	return uMultiOption;
}

#ifdef _LINUX
static void* NddsInitProc(void *lParam)
#else
static int NddsInitProc(void *lParam)
#endif
{
	g_eNddsErrorInit = DE_ERROR_SUCCESS;

	bool bRet;
	bRet = NIF_Initialize(&g_sNddsInit);
	if (!bRet)
	{
		g_eNddsErrorInit = DE_ERROR_INIT_NDDS;
		NddsEngineCallback(DE_MSGTYPE_NDDS_INIT, 0, g_eNddsErrorInit, 0, NULL);
		return 0;
	}

	g_bNddsIsInitialized = true;

	NddsEngineCallback(DE_MSGTYPE_NDDS_INIT, 0, DE_ERROR_SUCCESS, 0, NULL);

	return 0;
}

int NddsLibCallback(int nMsg, int nSessionID, long lParam1, long lParam2, void* lpParam)
{
	static DS_ROUTE_RESULT sRouteResult;

	if(NddsEngineCallback == NULL)
		return 0;

	void *pParam = lpParam;
	int msg = -1;
	switch(nMsg)
	{
	case NM_NDDS_ROUTE_INIT:
	case NM_NDDS_ROUTE_START:
	case NM_NDDS_ROUTE_PROGRESS:
	case NM_NDDS_ROUTE_END:
	case NM_NDDS_ROUTE_DATA:
		{
			memset(&sRouteResult, 0x00, sizeof(DS_ROUTE_RESULT));

			unsigned int uOption = lParam1;
			if(uOption&N_MULTIROUTE_BASIC_OPTIMUM)
			{
				lParam1 = DE_ROUTEOPTION_OPTIMUM;
			}
			if(uOption&N_MULTIROUTE_BASIC_EXPRESSWAY)
			{
				lParam1 |= DE_ROUTEOPTION_EXPRESSWAY;
			}
			if(uOption&N_MULTIROUTE_FEE_MINIMAL)
			{
				lParam1 |= DE_ROUTEOPTION_FREEWAY;
			}
			if(uOption&N_MULTIROUTE_BASIC_SHORTESTWAY)
			{
				lParam1 |= DE_ROUTEOPTION_SHORTESTWAY;
			}
			if(uOption&N_MULTIROUTE_BASIC_EASY)
			{
				lParam1 |= DE_ROUTEOPTION_EASY;
			}
			if(uOption&N_MULTIROUTE_BASIC_SHORTTIME)
			{
				lParam1 |= DE_ROUTEOPTION_SHORTTIME;
			}

			if(nMsg == NM_NDDS_ROUTE_INIT)
			{
				msg = DE_MSGTYPE_ROUTE_INIT;
			}else if(nMsg == NM_NDDS_ROUTE_START)
			{
				msg = DE_MSGTYPE_ROUTE_START;
			}else if(nMsg == NM_NDDS_ROUTE_END)
			{
				NeNddsRouteErrCode *eRouteError = (NeNddsRouteErrCode*)lpParam;

				switch(*eRouteError)
				{
				case N_ROUTEERR_SUCCED:
					sRouteResult.eErrorCode = DE_ERROR_SUCCESS; break;
				case N_ROUTEERR_STARTPOS:
				case N_ROUTEERR_STARTPOS2:
				case N_ROUTEERR_GOALPOS:
				case N_ROUTEERR_COORDINATE_ERROR:
					sRouteResult.eErrorCode = DE_ERROR_ROUTE_POSITION; break;
				case N_ROUTEERR_SAMEPOINT:
					sRouteResult.eErrorCode = DE_ERROR_ROUTE_POSITIONSAME; break;
				case N_ROUTEERR_USERCANCEL:
					sRouteResult.eErrorCode = DE_ERROR_ROUTE_USERCANCEL; break;
				case N_ROUTEERR_TIMEOUT:
					sRouteResult.eErrorCode = DE_ERROR_ROUTE_TIMEOUT; break;
				case N_ROUTEERR_SERVER:
					sRouteResult.eErrorCode = DE_ERROR_ROUTE_SERVER; break;
				case N_ROUTEERR_FAIL:
				case N_ROUTEERR_DISTOVER:
				default:
					sRouteResult.eErrorCode = DE_ERROR_ROUTE_FAIL; break;
				}

				msg = DE_MSGTYPE_ROUTE_END;
				pParam = &sRouteResult;

			}else if(nMsg == NM_NDDS_ROUTE_DATA)
			{
				msg = DE_MSGTYPE_ROUTE_DATA;
			}
		}
		break;

	case NM_NDDS_SEARCH_INIT:		msg = DE_MSGTYPE_SEARCH_INIT; break;
	case NM_NDDS_SEARCH_START:		msg = DE_MSGTYPE_SEARCH_START; break;
	case NM_NDDS_SEARCH_END:		msg = DE_MSGTYPE_SEARCH_END; break;

	case NM_NDDS_FAVORITE_INIT:		msg = DE_MSGTYPE_FAVORITE_INIT; break;
	case NM_NDDS_FAVORITE_START:	msg = DE_MSGTYPE_FAVORITE_START; break;
	case NM_NDDS_FAVORITE_END:		msg = DE_MSGTYPE_FAVORITE_END; break;

	case NM_NDDS_TRAFFICINFO_INIT:			msg = DE_MSGTYPE_TRAFFICINFO_INIT; break;
	case NM_NDDS_TRAFFICINFO_START:		msg = DE_MSGTYPE_TRAFFICINFO_START; break;
	case NM_NDDS_TRAFFICINFO_END:			msg = DE_MSGTYPE_TRAFFICINFO_END; break;

	case NM_NDDS_AUTH_INIT:			msg = DE_MSGTYPE_AUTH_INIT; break;
	case NM_NDDS_AUTH_START:		msg = DE_MSGTYPE_AUTH_START; break;
	case NM_NDDS_AUTH_PROGRESS:		msg = DE_MSGTYPE_AUTH_PROGRESS; break;
	case NM_NDDS_AUTH_CHANGE:		msg = DE_MSGTYPE_AUTH_CHANGE; break;
	case NM_NDDS_AUTH_END:			msg = DE_MSGTYPE_AUTH_END; break;
    case NM_NDDS_NONNDDS_END:       msg = DE_MSGTYPE_NONNDDS_END; break;
	default:
		break;
	}

	if(msg == -1)
		return 1;

	return NddsEngineCallback(msg, nSessionID, lParam1, lParam2, pParam);
}
