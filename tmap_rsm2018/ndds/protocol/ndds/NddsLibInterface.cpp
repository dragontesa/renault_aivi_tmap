// NddsInterfce.cpp

//#include "version.h"
#include "NddsLibInterface.h"
#include "NddsLibModule.h"
#include "debug.h"

bool NDDSLIB_SetInit(eNddsCharacterSet type, NDDSTMAP_INIT_INFO *pNddsTmapInit)
{
    //////////////////////////////////////////////////////////////////////////
    // NDDS 초기화
    if(!CNddsLibModule::CreateInstance())
    {
        return false;
    }

    CNddsLibModule &ndds = CNddsLibModule::GetInstance();
    ndds.Initialize(type, pNddsTmapInit);

    return true;
}

bool NDDSLIB_SetTmapHost(const char *pszHostName, const unsigned int nPort)
{
    if(!CNddsLibModule::CreateInstance())
    {
        return false;
    }

    CNddsLibModule &ndds = CNddsLibModule::GetInstance();
    ndds.setTmapHost(pszHostName, nPort);

    return true;
}

void NDDSLIB_Release()
{
    if(CNddsLibModule::IsInstance())
    {
        CNddsLibModule::DestroyInstance();
    }
}

bool NDDSLIB_SetAyncNddsUpdate(LPCALL_ASYNC_NDDSUPDATE_PROC pCallback)
{
    if(!CNddsLibModule::CreateInstance())
    {
        return false;
    }

    CNddsLibModule &ndds = CNddsLibModule::GetInstance();
    ndds.SetASyncNddsUpdate(pCallback);

    return true;
}

bool NDDSLIB_SetAuthAccessKey(char *pAccessKey)
{
    //////////////////////////////////////////////////////////////////////////
    // NDDS 초기화
    if(!CNddsLibModule::CreateInstance())
    {
        return false;
    }

    CNddsLibModule &ndds = CNddsLibModule::GetInstance();
    ndds.SetAuthAccessKey(pAccessKey);

    return true;
}

bool NDDSLIB_RequestProtocol(eNddsProtocolType eProtocolType, eNddsRequestType eRequestType, NDDS_COMMON_REQ *pReq, int nSessionID, eNddsErrorCode *eErrCode)
{
    bool bRet = false;
    CNddsLibModule &ndds = CNddsLibModule::GetInstance();

    NddsExternalLog(E_DEBUG, "nddslibinterface :: RequestProtocol before :: eProtocolType = %d, eRequestType = %d, nSessionID = %d", eProtocolType, eRequestType, nSessionID);
    bRet = ndds.RequestProtocol(eProtocolType, eRequestType, pReq, nSessionID, eErrCode);
	NddsExternalLog(E_DEBUG, "nddslibinterface :: RequestProtocol after ::  eProtocolType = %d, eRequestType = %d, nSessionID = %d, eErrCode = %d", eProtocolType, eRequestType, nSessionID, *eErrCode);

    return bRet;
}

// 통신 프로토콜 요청(by Json String)
bool NDDSLIB_RequestJson(eNddsRequestType eRequestType, NDDS_COMMON_REQ *pReq, int *nSessionID, eNddsErrorCode *eErrCode)
{
    bool bRet = false;
    CNddsLibModule &ndds = CNddsLibModule::GetInstance();

    bRet = ndds.RequestJson(eRequestType, pReq, nSessionID, eErrCode);

    return bRet;
}

bool NDDSLIB_GetStatusProtocol(int nSessionID, eNddsStatus *eStatus, int *nProgress, eNddsErrorCode *eErrCode)
{
    bool bRet = false;
    CNddsLibModule &ndds = CNddsLibModule::GetInstance();

    bRet = ndds.GetStatusProtocol(nSessionID, eStatus, nProgress, eErrCode);

    return bRet;
}

bool NDDSLIB_GetResultProtocol(int nSessionID, bool bWait, eNddsProtocolType *eProtocolType, NDDS_COMMON_RES **ppRes, eNddsErrorCode *eErrCode)
{
    bool bRet = false;
    CNddsLibModule &ndds = CNddsLibModule::GetInstance();

    NDDSTMAP_COMMON_RES *pResultData = NULL;

    if(bWait)
    {
        eNddsStatus eStatus;
        int nProgress;
        eNddsErrorCode eErrorCode;

        while(NDDSLIB_GetStatusProtocol(nSessionID, &eStatus, &nProgress, &eErrorCode))
        {
//			DBGPRINT(_T("[%d] %u, %d, %u\n"), nSessionID, eStatus, nProgress, eErrorCode);
            if(eStatus == NDDSSTATUS_COMPLETE)
            {
                bRet = ndds.GetResultProtocol(nSessionID, eProtocolType, ppRes, eErrCode);
                break;
            }
            if(eStatus == NDDSSTATUS_CANCELED)
                break;

            delay(50);
        }

    }else
    {
        bRet = ndds.GetResultProtocol(nSessionID, eProtocolType, ppRes, eErrCode);
    }

    return bRet;
}

bool NDDSLIB_CancelProtocol(int nSessionID)
{
    bool bRet = false;
    if(nSessionID == -1)
        return bRet;

    CNddsLibModule &ndds = CNddsLibModule::GetInstance();

    bRet = ndds.CancelProtocol(nSessionID);
    return bRet;
}

void NDDSLIB_ClearProtocol()
{
    CNddsLibModule &ndds = CNddsLibModule::GetInstance();
    ndds.ClearProtocol();
}

void NDDSLIB_SetSoqInfo(char* pInfo)
{
    CNddsLibModule &ndds = CNddsLibModule::GetInstance();
    ndds.SetTmapSoqInfo(pInfo);
}

void NDDSLIB_ReleaseObject(NDDS_COMMON_RES *pObject, eNddsErrorCode eErrCode)
{
    if(pObject != NULL && pObject->memory.pMemory != NULL && pObject->memory.nMemory > 0)
    {
        free(pObject->memory.pMemory);
        memset(&pObject->memory, 0x00, sizeof(NDDS_MEMORYSTATUS));
        memset(&pObject->header, 0x00, sizeof(NDDS_COMMONHEADER_RES));
    }
    else if(eErrCode == NDDSERR_RES_BODY && pObject->memory.pMemory != nullptr)
    {
        free(pObject->memory.pMemory);
        pObject->memory.pMemory = nullptr;
    }
}

bool NDDSLIB_SetReqDataSave(bool bLogSave, char* pszPath)
{
    //////////////////////////////////////////////////////////////////////////
    // NDDS 요청 데이터 저장
    if(!CNddsLibModule::CreateInstance())
    {
        return false;
    }

    CNddsLibModule &ndds = CNddsLibModule::GetInstance();
    ndds.SetReqDataSave(bLogSave, pszPath);

    return true;
}
