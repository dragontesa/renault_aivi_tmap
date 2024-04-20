#include "MNS_Interface.h"

MNS_ErrorCode MNS_Initialize(char *szPath, char *szExtendPath, char *appCode, bool bSync, LPCALL_MNS pCallback, TS_ENGINEINIT_EX *pInitEx)
{
    if (pCallback) {
        pCallback(E_MSGTYPE_ENGINE_INIT, 0, 0, 0);
    }

    return E_ERROR_SUCCESS;
}

void MNS_Uninitialize()
{

}

MNS_ErrorCode MNS_Route(TS_ROUTE_REQ *pRouteReq, bool bSync, TS_ROUTE_RES *pRouteRes)
{
    return E_ERROR_UNKNOWN;
}

MNS_ErrorCode MNS_IsValidePositionByRoutting(TS_ROUTE_REQ *pRouteReq)
{
    return E_ERROR_SUCCESS;
}

TMAPNAVI_API MNS_ErrorCode MNS_IsValidePositionByRouttingEx(TS_ROUTE_REQ *pRouteReq, int nLimitKM = 1000)
{
    return E_ERROR_SUCCESS;
}

MNS_ErrorCode MNS_StopRoute()
{
    return E_ERROR_SUCCESS;
}

MNS_ErrorCode MNS_PowerSearchCheck(const char *szSearchName, bool bNewAddrChk, TeSearchType &eSearchType)
{
    return E_ERROR_UNKNOWN;
}

MNS_ErrorCode MNS_Search(TS_SEARCH_REQ *pSearchReq, bool bSync, int &nResult)
{
    return E_ERROR_UNKNOWN;
}

MNS_ErrorCode MNS_GetSearchResultList(TeSearchType eSearchType, bool bNewAddr, int nIdx, int &nCnt, TS_SEARCH_LIST *pResult, int *nTotal)
{
    return E_ERROR_UNKNOWN;
}

MNS_ErrorCode MNS_GetNextAddrList(TS_POINT tp, TeAddressType eAddressType, int nAddressCode, int nRiCode, bool bFilter, int nIdx, int &nCnt, TS_RESULT_ADDR *pResult, int *nTotal, bool *bNext)
{
    return E_ERROR_UNKNOWN;
}

MNS_ErrorCode MNS_GetNextCateList(int nCateCode, bool bFilter, int nIdx, int &nCnt, TS_RESULT_CATE *pResult, int *nTotal, bool *bNext)
{
    return E_ERROR_UNKNOWN;
}
