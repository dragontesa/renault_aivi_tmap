// Interface.cpp

#include "interface.h"
#include "NddsModule.h"

extern LPCALL_NDDS_LOG g_pNddsLogFunction = NULL;  	// for rsm2018 logger. by hheo 2019.11.21

typedef bool (*_fAPI_NDDS_FunctionType)(CNddsModule &module, void *pRequest, void* pResult, void* pID);
_fAPI_NDDS_FunctionType	g_fAPI_NDDS_Function[eAPI_NDDS_FunctionMax];

/////////////////////////////////////////////////////////////////////////////////////////
// 내부 함수 정의
/////////////////////////////////////////////////////////////////////////////////////////

bool NIF_Function_eAPI_NDDS_Search					(CNddsModule& module, void* pRequest, void* pResult, void* pID)
{
	return module.API_NDDS_Search((PAPI_SEARCH_REQ) pRequest, (PAPI_SEARCH_RES) pResult);
}
bool NIF_Function_eAPI_NDDS_GetSearchResultList		(CNddsModule& module, void* pRequest, void* pResult, void* pID)
{
	return module.API_NDDS_GetSearchResultList((PAPI_SEARCH_RESULTLIST_REQ) pRequest, (PAPI_SEARCH_RESULTLIST_RES) pResult);
}
bool NIF_Function_eAPI_NDDS_GetSearchPoiDetailInfo		(CNddsModule& module, void* pRequest, void* pResult, void* pID)
{
	return module.API_NDDS_GetSearchPoiDetailInfo((PAPI_SEARCH_POIDETAILINFO_REQ) pRequest, (PAPI_SEARCH_POIDETAILINFO_RES) pResult);
}
bool NIF_Function_eAPI_NDDS_GetSearchPoiStationInfo		(CNddsModule& module, void* pRequest, void* pResult, void* pID)
{
    return module.API_NDDS_GetSearchPoiStationInfo((PAPI_SEARCH_POISTATIONINFO_REQ) pRequest, (PAPI_SEARCH_POISTATIONINFO_RES) pResult);
}

bool NIF_Function_eAPI_NDDS_GetSearchPoiEvStationInfo		(CNddsModule& module, void* pRequest, void* pResult, void* pID)
{
    return module.API_NDDS_GetSearchPoiEvStationInfo((PAPI_SEARCH_POIEVSTATIONINFO_REQ) pRequest, (PAPI_SEARCH_POIEVSTATIONINFO_RES) pResult);
}

bool NIF_Function_eAPI_NDDS_GetSearchPoiJoinInfo		(CNddsModule& module, void* pRequest, void* pResult, void* pID)
{
	return module.API_NDDS_GetSearchPoiJoinInfo((PAPI_SEARCH_POIJOININFO_REQ) pRequest, (PAPI_SEARCH_POIJOININFO_RES) pResult);
}
bool NIF_Function_eAPI_NDDS_GetSearchRecentDestnationList		(CNddsModule& module, void* pRequest, void* pResult, void* pID)
{
	return module.API_NDDS_GetSearchRecentDestnationList((PAPI_SEARCH_RECENTDESTNATION_REQ) pRequest, (PAPI_SEARCH_RECENTDESTNATION_RES) pResult);
}
bool NIF_Function_eAPI_NDDS_Route						(CNddsModule& module, void* pRequest, void* pResult, void* pID)
{
	return module.API_NDDS_Route((PAPI_ROUTE_REQ) pRequest, (PAPI_ROUTE_RES) pResult);
}
bool NIF_Function_eAPI_NDDS_GetRouteSummaryInfo					(CNddsModule& module, void* pRequest, void* pResult, void* pID)
{
	return module.API_NDDS_GetRouteSummaryInfo((PAPI_GET_ROUTESUMMARYINFO_REQ) pRequest, (PAPI_GET_ROUTESUMMARYINFO_RES) pResult);
}
bool NIF_Function_eAPI_NDDS_StopRoute					(CNddsModule& module, void* pRequest, void* pResult, void* pID)
{
	return module.API_NDDS_StopRoute((PAPI_STOPROUTE_REQ) pRequest, (PAPI_STOPROUTE_RES) pResult);
}
bool NIF_Function_eAPI_NDDS_GetWeatherInfo					(CNddsModule& module, void* pRequest, void* pResult, void* pID)
{
	return module.API_NDDS_GetWeatherInfo((PAPI_WEATHERINFO_REQ) pRequest, (PAPI_WEATHERINFO_RES) pResult);
}
bool NIF_Function_eAPI_NDDS_Favorite					(CNddsModule& module, void* pRequest, void* pResult, void* pID)
{
	return module.API_NDDS_Favorite((PAPI_FAVORITE_REQ) pRequest, (PAPI_FAVORITE_RES) pResult);
}
bool NIF_Function_eAPI_NDDS_GetFavoriteResultList		(CNddsModule& module, void* pRequest, void* pResult, void* pID)
{
	return module.API_NDDS_GetFavoriteResultList((PAPI_FAVORITE_RESULTLIST_REQ) pRequest, (PAPI_FAVORITE_RESULTLIST_RES) pResult);
}
bool NIF_Function_eAPI_NDDS_GetMyFavorite		(CNddsModule& module, void* pRequest, void* pResult, void* pID)
{
	return module.API_NDDS_GetMyFavorite((PAPI_MYFAVORITE_REQ) pRequest, (PAPI_MYFAVORITE_RES) pResult);
}
bool NIF_Function_eAPI_NDDS_GetSendToCarInfo		(CNddsModule& module, void* pRequest, void* pResult, void* pID)
{
	return module.API_NDDS_GetSendToCarInfo((PAPI_SENDTOCARINFO_REQ) pRequest, (PAPI_SENDTOCARINFO_RES) pResult);
}
bool NIF_Function_eAPI_NDDS_VmsGuideInfo					(CNddsModule& module, void* pRequest, void* pResult, void* pID)
{
	return module.API_NDDS_VmsGuideInfo((PAPI_VMSGUIDEINFO_REQ) pRequest, (PAPI_VMSGUIDEINFO_RES) pResult);
}
bool NIF_Function_eAPI_NDDS_GetVmsGuideInfo					(CNddsModule& module, void* pRequest, void* pResult, void* pID)
{
	return module.API_NDDS_GetVmsGuideInfo((PAPI_GET_VMSGUIDEINFO_REQ) pRequest, (PAPI_GET_VMSGUIDEINFO_RES) pResult);
}
bool NIF_Function_eAPI_NDDS_AuthTmapForSmartCar					(CNddsModule& module, void* pRequest, void* pResult, void* pID)
{
	return module.API_NDDS_AuthTmapForSmartCar((PAPI_AUTHTMAPFORSMARTCAR_REQ) pRequest, (PAPI_AUTHTMAPFORSMARTCAR_RES) pResult);
}
bool NIF_Function_eAPI_NDDS_GetAuthResult					(CNddsModule& module, void* pRequest, void* pResult, void* pID)
{
	return module.API_NDDS_GetAuthResult((PAPI_GET_AUTHRESULT_REQ) pRequest, (PAPI_GET_AUTHRESULT_RES) pResult);
}
bool NIF_Function_eAPI_NDDS_SetAuthAccessKey					(CNddsModule& module, void* pRequest, void* pResult, void* pID)
{
	return module.API_NDDS_SetAuthAccessKey((PAPI_SET_AUTHACCESSKEY_REQ) pRequest, (PAPI_SET_AUTHACCESSKEY_RES) pResult);
}
bool NIF_Function_eAPI_NDDS_GetUserData			(CNddsModule& module, void* pRequest, void* pResult, void* pID)
{
    return module.API_NDDS_GetUserData((PAPI_GET_USERDATA_REQ) pRequest, (PAPI_GET_USERDATA_RES) pResult);
}
bool NIF_Function_eAPI_NDDS_SetReqDataSave 					(CNddsModule& module, void* pRequest, void* pResult, void* pID)
{
    return module.API_NDDS_SetReqDataSave((PAPI_SETREQDATASAVE_REQ) pRequest, (PAPI_SETREQDATASAVE_RES) pResult);
}
bool NIF_Function_eAPI_NDDS_NonNdds 					(CNddsModule& module, void* pRequest, void* pResult, void* pID)
{
    return module.API_NDDS_NonNdds((PAPI_NONNDDS_REQ) pRequest, (PAPI_NONNDDS_RES) pResult);
}
/////////////////////////////////////////////////////////////////////////////////////////
// 외부 인터페이스 정의
/////////////////////////////////////////////////////////////////////////////////////////

NDDSIF_ERRCODE g_eLastErrorCode = NERROR_SUCCESS;

bool NIF_Initialize(PNDDSMODULE_INIT pNddsInit)
{
	// NDDS 모듈 객체 생성
	if(CNddsModule::IsInstance())
	{
		DBGPRINT(_T("#############################(NIF_Initialize- DestroyInstance)"));
		CNddsModule::DestroyInstance();
	}
	if(!CNddsModule::CreateInstance())
	{
		g_eLastErrorCode = NERROR_NDDSMODULECREATE;
		return false;
	}

	CNddsModule &Module = CNddsModule::GetInstance();
	DBGPRINT(_T("NIF_Initialize #1\n"));

	Module.SetNddsUpdateCallback(NULL);

	memset(g_fAPI_NDDS_Function, 0x00, sizeof(_fAPI_NDDS_FunctionType));

	g_fAPI_NDDS_Function[eAPI_NDDS_Search						] = &NIF_Function_eAPI_NDDS_Search					;
	g_fAPI_NDDS_Function[eAPI_NDDS_GetSearchResultList			] = &NIF_Function_eAPI_NDDS_GetSearchResultList		;
	g_fAPI_NDDS_Function[eAPI_NDDS_GetSearchPoiDetailInfo		] = &NIF_Function_eAPI_NDDS_GetSearchPoiDetailInfo		;
    g_fAPI_NDDS_Function[eAPI_NDDS_GetSearchPoiStationInfo		] = &NIF_Function_eAPI_NDDS_GetSearchPoiStationInfo		;
    g_fAPI_NDDS_Function[eAPI_NDDS_GetSearchPoiEvStationInfo	] = &NIF_Function_eAPI_NDDS_GetSearchPoiEvStationInfo		;
	g_fAPI_NDDS_Function[eAPI_NDDS_GetSearchPoiJoinInfo			] = &NIF_Function_eAPI_NDDS_GetSearchPoiJoinInfo		;
	g_fAPI_NDDS_Function[eAPI_NDDS_GetSearchRecentDestnationList] = &NIF_Function_eAPI_NDDS_GetSearchRecentDestnationList;

	g_fAPI_NDDS_Function[eAPI_NDDS_Route						] = &NIF_Function_eAPI_NDDS_Route						;
	g_fAPI_NDDS_Function[eAPI_NDDS_GetRouteSummaryInfo			] = &NIF_Function_eAPI_NDDS_GetRouteSummaryInfo			;
	g_fAPI_NDDS_Function[eAPI_NDDS_StopRoute					] = &NIF_Function_eAPI_NDDS_StopRoute					;
	g_fAPI_NDDS_Function[eAPI_NDDS_GetWeatherInfo				] = &NIF_Function_eAPI_NDDS_GetWeatherInfo				;

	g_fAPI_NDDS_Function[eAPI_NDDS_Favorite						] = &NIF_Function_eAPI_NDDS_Favorite					;
	g_fAPI_NDDS_Function[eAPI_NDDS_GetFavoriteResultList		] = &NIF_Function_eAPI_NDDS_GetFavoriteResultList		;
	g_fAPI_NDDS_Function[eAPI_NDDS_GetMyFavorite				] = &NIF_Function_eAPI_NDDS_GetMyFavorite				;
	g_fAPI_NDDS_Function[eAPI_NDDS_GetSendToCarInfo				] = &NIF_Function_eAPI_NDDS_GetSendToCarInfo			;

	g_fAPI_NDDS_Function[eAPI_NDDS_VmsGuideInfo					] = &NIF_Function_eAPI_NDDS_VmsGuideInfo			;
	g_fAPI_NDDS_Function[eAPI_NDDS_GetVmsGuideInfo				] = &NIF_Function_eAPI_NDDS_GetVmsGuideInfo				;

	g_fAPI_NDDS_Function[eAPI_NDDS_AuthTmapForSmartCar			] = &NIF_Function_eAPI_NDDS_AuthTmapForSmartCar			;
	g_fAPI_NDDS_Function[eAPI_NDDS_GetAuthResult				] = &NIF_Function_eAPI_NDDS_GetAuthResult				;
	g_fAPI_NDDS_Function[eAPI_NDDS_SetAuthAccessKey				] = &NIF_Function_eAPI_NDDS_SetAuthAccessKey			;

    g_fAPI_NDDS_Function[eAPI_NDDS_SetReqDataSave               ] = &NIF_Function_eAPI_NDDS_SetReqDataSave  			;
    g_fAPI_NDDS_Function[eAPI_NDDS_NonNdds                      ] = &NIF_Function_eAPI_NDDS_NonNdds                     ;
	//초기화 순서 바꾸지 말것 - 박현재 2012.07.04

	//공용 모듈
	DBGPRINT(_T("InitCommon\n"));
	if(!Module.InitCommon(pNddsInit))
	{

		//assert(0);
		g_eLastErrorCode = NERROR_INIT_COMMON;
		return false;
	}		

	//통신 모듈
	DBGPRINT(_T("InitProtocol\n"));
	if(!Module.InitProtocol(pNddsInit))
	{
		//assert(0);
		g_eLastErrorCode = NERROR_INIT_TVAS_ERR;
		return false;
	}

	DBGPRINT(_T("NIF_Initialize #2\n"));

	return true;
}

bool NIF_Uninitialize()
{
	DBGPRINT(_T("NIF_Uninitialize #1\n"));

	CNddsModule &module = CNddsModule::GetInstance();

	//해제 순서 바꾸지 말것 - 박현재 2012.07.04

	module.ReleaseCommonThread();
	module.ReleaseProtocol();

#ifdef _WIN32
	if(WSACleanup() == -1)
		return FALSE;
#endif // _WIN32

	CNddsModule::DestroyInstance();
	DBGPRINT(_T("NIF_Uninitialize #2\n"));

	return true;
}

bool NIF_Function(eAPI_NDDS_Function eFunction, PAPI_REQUEST pRequest, PAPI_RESULT pResult)
{
	CNddsModule &module = CNddsModule::GetInstance();

	if( g_fAPI_NDDS_Function[eFunction] != NULL )
		return (g_fAPI_NDDS_Function[eFunction])(module, pRequest, pResult, NULL);

	return false;
}

NDDSIF_ERRCODE NIF_GetLastErrCode()
{
	return g_eLastErrorCode;
}

bool NIF_SetTmapHost(const char *pszHostName, const unsigned int nPort)
{
    CNddsModule &module = CNddsModule::GetInstance();

    return module.API_NDDS_SetTmapHost(pszHostName, nPort);
}
