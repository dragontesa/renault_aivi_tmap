#include "NDDS_Interface.h"

DeErrorCode NDDS_Initialize(bool bSync, LPCALL_NDDS pCallback, DS_NDDSINIT_EX *pInitEx)
{
    if (pCallback) {
        pCallback(DE_MSGTYPE_NDDS_INIT, 0, DE_ERROR_SUCCESS, 0, 0);
    }
    return DE_ERROR_SUCCESS;
}

void NDDS_Uninitialize()
{

}

char* NDDS_GetVersion()
{
    return "";
}

DeErrorCode NDDS_Route(DS_ROUTE_REQ *pRouteReq, bool bSync, int *nSessionID)
{
    return DE_ERROR_UNKNOWN;
}

DeErrorCode NDDS_StopRoute(int nSessionID)
{
    return DE_ERROR_SUCCESS;
}

DeErrorCode NDDS_GetWeatherInfo(DS_WEATHERINFO_RES *pResult)
{
    return DE_ERROR_UNKNOWN;
}

DeErrorCode NDDS_GetRouteSummaryInfo(int nSessionID, DS_ROUTESUMMARYINFO_RES *pResult)
{
    return DE_ERROR_UNKNOWN;
}

DeErrorCode NDDS_Search(DS_SEARCH_REQ *pSearchReq, bool bSync, int &nResult, int *nSessionID)
{
    return DE_ERROR_UNKNOWN;
}

DeErrorCode NDDS_GetSearchResultList(DeSearchType eSearchType, int nIdx, int &nCnt, DS_SEARCH_LIST *pResult, int *nTotal)
{
    return DE_ERROR_UNKNOWN;
}

DeErrorCode NDDS_GetSearchPoiDetailInfo(DS_SEARCH_POI_DETAIL_INFO *pResult)
{
    return DE_ERROR_UNKNOWN;
}

DeErrorCode NDDS_GetSearchPoiStationInfo(DS_SEARCH_POI_STATION_INFO *pResult)
{
    return DE_ERROR_UNKNOWN;
}


DeErrorCode NDDS_GetSearchPoiJoinInfo(int nIdx, int &nCnt, DS_SEARCH_POI_JOIN_INFO *pResult, int *nTotal)
{
    return DE_ERROR_UNKNOWN;
}

DeErrorCode NDDS_VmsGuideInfo(char *pRseId, bool bSync, int *nSessionID)
{
    return DE_ERROR_UNKNOWN;
}

DeErrorCode NDDS_GetVmsGuideInfo(DS_VMSGUIDEINFO *pResult)
{
    return DE_ERROR_UNKNOWN;
}

DeErrorCode NDDS_AuthTmapForSmartCar(char *pOtpCode, bool bSync, int *nSessionID)
{
    return DE_ERROR_UNKNOWN;
}

DeErrorCode NDDS_GetAuthResult(DS_AUTHRESULT *pResult)
{
    return DE_ERROR_UNKNOWN;
}

DeErrorCode NDDS_SetAuthAccessKey(char *pAccessKey)
{
    return DE_ERROR_UNKNOWN;
}

