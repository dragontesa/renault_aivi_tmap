#include "serversearch.h"
#include "searchmanager.h"
#include "textdecoder.h"
#include "stringutil.h"
#include "NDDS_Interface.h"
#include "MNS_Interface.h"
#include <QDebug>
#include "PoiHelper.h"

#include "VSM_Map.h"
#include <QtMath>

#include "../ndds/include/common_def.h"

namespace SKT {


ServerSearch* ServerSearch::s_instance = NULL;

ServerSearch::ServerSearch()
{

}

ServerSearch::~ServerSearch()
{

}

ServerSearch* ServerSearch::sharedInstance()
{
    if (!s_instance) {
        s_instance = (ServerSearch*) new ServerSearch();
    }
    return s_instance;
}

void ServerSearch::sendFinished(SearchReplyBase *reply) {
    QMetaObject::invokeMethod(reply, "finished", Qt::QueuedConnection);
}

int ServerSearch::conv2OPIPoleCate(const char* szOilCompany, int poleKind) {
    QString reqStr = QString::fromLocal8Bit(szOilCompany);
    enum OILBRAND{
        E_DEF = -1,
        E_SK = 0,
        E_GS,
        E_SOIL,
        E_OILBANK,
    };
    OILBRAND type;
    QByteArray oilbank = TextDecoder::instance()->fromUnicode("오일뱅크");
    int ret = 0;

    if (checkStrCompare(reqStr, "SK")) {
        type = E_SK;
    } else if (checkStrCompare(reqStr, "GS")) {
        type = E_GS;
    } else if (checkStrCompare(reqStr, "S-Oil")) {
        type = E_SOIL;
    } else if (strcmp(szOilCompany, oilbank) == 0) {
        type = E_OILBANK;
    } else {
        return 0;
    }

    switch(type) {
    case E_SK:
        ret = 20101;
        break;
    case E_GS:
        ret = 20102;
        break;
    case E_SOIL:
        ret = 20103;
        break;
    case E_OILBANK:
        ret = 20104;
        break;
    default :
        ret = 0;
        break;
    }

    if(poleKind == 4 /*충전소*/ && ret!=0) {
        ret = ret + 100;
    }
    return ret;
}

bool ServerSearch::checkStrCompare(QString reqStr, const char* token) {
    int strSize = 0;
    bool ret = false;
    if(reqStr.size() == 0) return false;
    reqStr.replace(token, "");
    strSize = reqStr.size();
    if(strSize == 0) {
        ret = true;
    } else {
        ret = false;
    }
    return ret;
}

TPOI_SEARCH_LIST* ServerSearch::conv2TmapSearchList(DS_SEARCH_LIST* result, int resultCount) {

    TPOI_SEARCH_LIST* convResult = new TPOI_SEARCH_LIST[resultCount];
    VSM_Int32   retAngle = 0;

    for(int i = 0; i < resultCount ; i++) {
        memset((void*)&(convResult[i]), 0x0, sizeof(TPOI_SEARCH_LIST));
        convResult[i].nIdx          = result[i].nIdx;
        convResult[i].nPoiID        = result[i].nPoiID;
        convResult[i].nAddrCode     = result[i].nAddrCode;
        convResult[i].byRPFlag      = result[i].byRPFlag;
        convResult[i].nDist         = result[i].nDist;
        VSMMap_GetAngleWpToWp(getPosition().x, getPosition().y, result[i].tp.x, result[i].tp.y, retAngle = 0);
        convResult[i].nDeg          = 360 - retAngle; // 진북방향기준에서 각도계산값이 반시계 방향으로 되어있어, 시계방향으로 보정해줌
        convResult[i].nKind         = result[i].nKind;
        //REM convResult[i].nOPIPoleCate  = result[i].nOPIPoleCate;
        convResult[i].nOPIPoleCate  = conv2OPIPoleCate(result[i].szOilCompany, result[i].nDataKind);
        convResult[i].nDataKind     = result[i].nDataKind;
        convResult[i].nOPIPrice     = result[i].nOPIPrice;
        convResult[i].tp.x          = result[i].tp.x;
        convResult[i].tp.y          = result[i].tp.y;
        convResult[i].sGatherTime.nDay      = result[i].sGatherTime.nDay;
        convResult[i].sGatherTime.nHour     = result[i].sGatherTime.nHour;
        convResult[i].sGatherTime.nMinute   = result[i].sGatherTime.nMinute;
        convResult[i].sGatherTime.nMonth    = result[i].sGatherTime.nMonth;
        convResult[i].sGatherTime.nSecond   = result[i].sGatherTime.nSecond;
        convResult[i].sGatherTime.nWeek     = result[i].sGatherTime.nWeek;
        convResult[i].sGatherTime.nYear     = result[i].sGatherTime.nYear;
        strncpy(convResult[i].szName, result[i].szName,sizeof(convResult[i].szName)-1);
        strncpy(convResult[i].szStdName, result[i].szStdName,sizeof(convResult[i].szStdName)-1);
        strncpy(convResult[i].szAddrName, result[i].szAddrName,sizeof(convResult[i].szAddrName)-1);
        strncpy(convResult[i].szPhoneNumber, result[i].szPhoneNumber,sizeof(convResult[i].szPhoneNumber)-1);

        //reserved for ndds
        strncpy(convResult[i].szOilCompany, result[i].szOilCompany,sizeof(convResult[i].szOilCompany)-1);
        convResult[i].bHighGasoline = result[i].bHighGasoline;
        convResult[i].bLowestOilSt = result[i].bLowestOilSt;
        convResult[i].nOPIGasolinePrice = result[i].nOPIGasolinePrice;
        convResult[i].nOPIDieselPrice = result[i].nOPIDieselPrice;
        convResult[i].nOPILPGPrice = result[i].nOPILPGPrice;
        convResult[i].nOPIHiGasolinePrice = result[i].nOPIHiGasolinePrice;
        convResult[i].nOPIHiLPGPrice = result[i].nOPIHiLPGPrice;
        strncpy(convResult[i].szNavseq, result[i].szNavSeq,sizeof(convResult[i].szNavseq)-1);


        //EV
        convResult[i].evStationCnt = result[i].nEvChargerTotalCnt;
        if (convResult[i].evStationCnt > D_MAXLEN_EVCHARGER)
            convResult[i].evStationCnt = D_MAXLEN_EVCHARGER;
        for(int j=0;j<convResult[i].evStationCnt;j++)
        {
            memset(&convResult[i].chargerInfo[j], 0, sizeof(convResult[i].chargerInfo[j]));
            strncpy(convResult[i].chargerInfo[j].szBid, result[i].sChargerInfo[j].szBid, sizeof(result[i].sChargerInfo[j].szBid)-1);
            strncpy(convResult[i].chargerInfo[j].szSid, result[i].sChargerInfo[j].szSid, sizeof(result[i].sChargerInfo[j].szSid)-1);
            strncpy(convResult[i].chargerInfo[j].szCid, result[i].sChargerInfo[j].szCid, sizeof(result[i].sChargerInfo[j].szCid)-1);
            strncpy(convResult[i].chargerInfo[j].szType, result[i].sChargerInfo[j].szType, sizeof(result[i].sChargerInfo[j].szType)-1);
            convResult[i].chargerInfo[j].nStatus = result[i].sChargerInfo[j].nStatus;
        }
    }
    return convResult;
}


TPOI_DETAIL_INFO* ServerSearch::conv2TmapPoiDetailInfo(DS_SEARCH_POI_DETAIL_INFO* stDetailResult, int resultCount) {

    TPOI_DETAIL_INFO* convResult = new TPOI_DETAIL_INFO[resultCount];

    for(int i = 0; i < resultCount ; i++) {
        memset((void*)&(convResult[i]), 0x0, sizeof(TPOI_DETAIL_INFO));

        convResult->nPoiID = stDetailResult[i].nPoiID;
        strncpy(convResult[i].szName, stDetailResult[i].szName,sizeof(convResult[i].szName)-1);
        strncpy(convResult[i].szAddrName, stDetailResult[i].szAddrName ,sizeof(convResult[i].szAddrName)-1); // NDDS에서 구주소만 전달가능하므로, null로 처리
        strncpy(convResult[i].szPhoneNumber, stDetailResult[i].szPhoneNumber,sizeof(convResult[i].szPhoneNumber)-1);
        strncpy(convResult[i].szUrl, stDetailResult[i].szUrl,sizeof(convResult[i].szUrl)-1);
        convResult[i].bHoliday = stDetailResult[i].bHoliday;
        convResult[i].byPark = stDetailResult[i].byPark;
        strncpy(convResult[i].szCateName, stDetailResult[i].szCateName,sizeof(convResult[i].szCateName)-1);
        strncpy(convResult[i].szInfo, stDetailResult[i].szInfo,sizeof(convResult[i].szInfo)-1);
        memset((void*)&(convResult[i].sGatherTime), 0x0, sizeof(convResult[i].sGatherTime));
        strncpy(convResult[i].szAddInformation, stDetailResult[i].szAddInfo,sizeof(convResult[i].szAddInformation)-1);

        strncpy(convResult[i].szDayOffInfo    , stDetailResult[i].szDayOffInfo    , sizeof(convResult[i].szDayOffInfo   )-1);
        strncpy(convResult[i].szBusinessHours , stDetailResult[i].szBusinessHours , sizeof(convResult[i].szBusinessHours)-1);
        strncpy(convResult[i].szDayOffDate    , stDetailResult[i].szDayOffDate    , sizeof(convResult[i].szDayOffDate   )-1);

        convResult[i].nDataKind = stDetailResult[i].nDataKind;
        convResult[i].bHighGasoline = stDetailResult[i].bHighGasoline;
        convResult[i].nOPIGasolinePrice = stDetailResult[i].nOPIGasolinePrice;
        convResult[i].nOPIDieselPrice = stDetailResult[i].nOPIDieselPrice;
        convResult[i].nOPILPGPrice = stDetailResult[i].nOPILPGPrice;
        convResult[i].nOPIHiGasolinePrice = stDetailResult[i].nOPIHiGasolinePrice;
        convResult[i].nOPIHiLPGPrice = stDetailResult[i].nOPIHiLPGPrice;
        convResult[i].tp = stDetailResult[i].tp;

        qDebug() << "-------------------------poi detail info - s -----------------------";
        qDebug() << "stDetailResult.nPoiID" << stDetailResult[i].nPoiID;
        qDebug() << "stDetailResult.szName" << TextDecoder::instance()->fromCP949(stDetailResult[i].szName);
        qDebug() << "stDetailResult.szAddrName" << TextDecoder::instance()->fromCP949(stDetailResult[i].szAddrName);
        qDebug() << "stDetailResult.szPhoneNumber" << TextDecoder::instance()->fromCP949(stDetailResult[i].szPhoneNumber);
        qDebug() << "stDetailResult.szUrl" << TextDecoder::instance()->fromCP949(stDetailResult[i].szUrl);
        qDebug() << "stDetailResult.bHoliday" << stDetailResult[i].bHoliday;
        qDebug() << "stDetailResult.byPark" << stDetailResult[i].byPark;
        qDebug() << "stDetailResult.szCateName" << TextDecoder::instance()->fromCP949(stDetailResult[i].szCateName);
        qDebug() << "stDetailResult.szInfo" << TextDecoder::instance()->fromCP949(stDetailResult[i].szInfo);
        qDebug() << "stDetailResult.sGatherTime" << stDetailResult[i].m_GatherTime;
        qDebug() << "stDetailResult.szAddInformation" << TextDecoder::instance()->fromCP949(stDetailResult[i].szAddInfo);
        qDebug() << "stDetailResult.nDataKind " << stDetailResult[i].nDataKind;
        qDebug() << "stDetailResult.bHighGasoline" << stDetailResult[i].bHighGasoline;
        qDebug() << "stDetailResult.nOPIGasolinePrice" << stDetailResult[i].nOPIGasolinePrice;
        qDebug() << "stDetailResult.nOPIDieselPrice" << stDetailResult[i].nOPIDieselPrice;
        qDebug() << "stDetailResult.nOPILPGPrice" << stDetailResult[i].nOPILPGPrice;
        qDebug() << "stDetailResult.nOPIHiGasolinePrice" << stDetailResult[i].nOPIHiGasolinePrice;
        qDebug() << "stDetailResult.nOPIHiLPGPrice" << stDetailResult[i].nOPIHiLPGPrice;
        qDebug() << "-------------------------poi detail info - e -----------------------";
    }
    return convResult;
}

TPOI_EvDETAIL_INFO* ServerSearch::conv2TmapPoiEvDetailInfo(DS_EVSTATION_INFO* stDetailResult, int resultCount) {

    TPOI_EvDETAIL_INFO* convResult = new TPOI_EvDETAIL_INFO[resultCount];

    for(int i = 0; i < resultCount ; i++) {
        memset((void*)&(convResult[i]), 0x0, sizeof(TPOI_EvDETAIL_INFO));

        convResult->nPoiID = stDetailResult[i].poiId;
        convResult->szNavseq[0] = '0';
        if(stDetailResult[i].szStationName)
            strncpy(convResult[i].szName, stDetailResult[i].szStationName,sizeof(convResult[i].szName)-1);
        if(stDetailResult[i].szAddr)
            strncpy(convResult[i].szAddrName, stDetailResult[i].szAddr ,sizeof(convResult[i].szAddrName)-1);
        if(stDetailResult[i].szRoadAddr)
            strncpy(convResult[i].szRoadAddr, stDetailResult[i].szRoadAddr ,sizeof(convResult[i].szRoadAddr)-1);
        if(stDetailResult[i].szAddrDetail)
            strncpy(convResult[i].szAddrDetail, stDetailResult[i].szAddrDetail ,sizeof(convResult[i].szAddrDetail)-1);
        if(stDetailResult[i].szMngName)
            strncpy(convResult[i].szMngName, stDetailResult[i].szMngName ,sizeof(convResult[i].szMngName)-1);
        if(stDetailResult[i].szMngId)
            strncpy(convResult[i].szMngId, stDetailResult[i].szMngId ,sizeof(convResult[i].szMngId)-1);
        if(stDetailResult[i].szTel)
            strncpy(convResult[i].szPhoneNumber, stDetailResult[i].szTel,sizeof(convResult[i].szPhoneNumber)-1);
        convResult[i].bFreeYn = stDetailResult[i].bFreeYn;
        convResult[i].bReservYn = stDetailResult[i].bReservYn;
        if(stDetailResult[i].szUseTime)
            strncpy(convResult[i].szUseTime, stDetailResult[i].szUseTime,sizeof(convResult[i].szUseTime)-1);
        convResult[i].bPayYn = stDetailResult[i].bPayYn;
        if(stDetailResult[i].szFee)
            strncpy(convResult[i].szFee, stDetailResult[i].szFee ,sizeof(convResult[i].szFee)-1);
        if(stDetailResult[i].szUpdateDt)
            strncpy(convResult[i].szUpdateDt, stDetailResult[i].szUpdateDt,sizeof(convResult[i].szUpdateDt)-1);
        convResult[i].tp = stDetailResult[i].tp;
        convResult[i].evStationCnt = stDetailResult[i].nTotalCnt;
        if (convResult[i].evStationCnt > D_MAXLEN_EVCHARGER)
            convResult[i].evStationCnt = D_MAXLEN_EVCHARGER;
        for(int j=0;j<convResult[i].evStationCnt;j++)
        {
            memset(&convResult[i].chargerInfo[j], 0, sizeof(convResult[i].chargerInfo[j]));
            if(stDetailResult[i].chargerInfo[j].szBid)
                strncpy(convResult[i].chargerInfo[j].szBid, stDetailResult[i].chargerInfo[j].szBid, sizeof(stDetailResult[i].chargerInfo[j].szBid)-1);
            if(stDetailResult[i].chargerInfo[j].szSid)
                strncpy(convResult[i].chargerInfo[j].szSid, stDetailResult[i].chargerInfo[j].szSid, sizeof(stDetailResult[i].chargerInfo[j].szSid)-1);
            if(stDetailResult[i].chargerInfo[j].szCid)
                strncpy(convResult[i].chargerInfo[j].szCid, stDetailResult[i].chargerInfo[j].szCid, sizeof(stDetailResult[i].chargerInfo[j].szCid)-1);
            if(stDetailResult[i].chargerInfo[j].szType)
                strncpy(convResult[i].chargerInfo[j].szType, stDetailResult[i].chargerInfo[j].szType, sizeof(stDetailResult[i].chargerInfo[j].szType)-1);
            convResult[i].chargerInfo[j].nStatus = stDetailResult[i].chargerInfo[j].nStatus;
        }
        qDebug() << "-------------------------poi ev detail info - s -----------------------";
        qDebug() << "stDetailResult.poiId" << stDetailResult[i].poiId;
        qDebug() << "stDetailResult.szStationName" << TextDecoder::instance()->fromCP949(stDetailResult[i].szStationName);
        qDebug() << "stDetailResult.szAddr" << TextDecoder::instance()->fromCP949(stDetailResult[i].szAddr);
        qDebug() << "stDetailResult.szTel" << TextDecoder::instance()->fromCP949(stDetailResult[i].szTel);
        qDebug() << "stDetailResult.bFreeYn" << stDetailResult[i].bFreeYn;
        qDebug() << "stDetailResult.bReservYn" << stDetailResult[i].bReservYn;
        qDebug() << "stDetailResult.szUseTime" << TextDecoder::instance()->fromCP949(stDetailResult[i].szUseTime);
        qDebug() << "stDetailResult.bPayYn" << stDetailResult[i].bPayYn;
        qDebug() << "stDetailResult.szUpdateDt" << TextDecoder::instance()->fromCP949(stDetailResult[i].szUpdateDt);
        qDebug() << "-------------------------poi ev detail info - e -----------------------";
    }
    return convResult;
}

TPOI_STATION_INFO* ServerSearch::conv2TmapPoiStationInfo(DS_SEARCH_POI_STATION_INFO* stDetailResult, int resultCount) {

    TPOI_STATION_INFO* convResult = new TPOI_STATION_INFO[resultCount];

    for(int i = 0; i < resultCount ; i++) {
        memset((void*)&(convResult[i]), 0x0, sizeof(TPOI_STATION_INFO));

        convResult->nPoiID = stDetailResult[i].nPoiID;
        strncpy(convResult[i].szName, stDetailResult[i].szName,sizeof(convResult[i].szName)-1);
        convResult[i].nDataKind = stDetailResult[i].nDataKind;
        convResult[i].nOPIGasolinePrice = stDetailResult[i].nOPIGasolinePrice;
        convResult[i].nOPIDieselPrice = stDetailResult[i].nOPIDieselPrice;
        convResult[i].nOPILPGPrice = stDetailResult[i].nOPILPGPrice;
        convResult[i].nOPIHiGasolinePrice = stDetailResult[i].nOPIHiGasolinePrice;
        convResult[i].nOPIHiLPGPrice = stDetailResult[i].nOPIHiLPGPrice;
        convResult[i].tp = stDetailResult[i].tp;

        qDebug() << "-------------------------poi detail info - s -----------------------";
        qDebug() << "stDetailResult.nPoiID" << stDetailResult[i].nPoiID;
        qDebug() << "stDetailResult.szName" << TextDecoder::instance()->fromCP949(stDetailResult[i].szName);
        qDebug() << "stDetailResult.nDataKind " << stDetailResult[i].nDataKind;
        qDebug() << "stDetailResult.nOPIGasolinePrice" << stDetailResult[i].nOPIGasolinePrice;
        qDebug() << "stDetailResult.nOPIDieselPrice" << stDetailResult[i].nOPIDieselPrice;
        qDebug() << "stDetailResult.nOPILPGPrice" << stDetailResult[i].nOPILPGPrice;
        qDebug() << "stDetailResult.nOPIHiGasolinePrice" << stDetailResult[i].nOPIHiGasolinePrice;
        qDebug() << "stDetailResult.nOPIHiLPGPrice" << stDetailResult[i].nOPIHiLPGPrice;
        qDebug() << "-------------------------poi detail info - e -----------------------";
    }
    return convResult;
}

const TPOI_FAVORITE_INFO* ServerSearch::conv2TmapFavoriteInfo(const DS_FAVORITE_LIST* result, int resultCount)
{
    TPOI_FAVORITE_INFO* convResult = NULL;
    convResult = new TPOI_FAVORITE_INFO[resultCount];
    for (int i=0;i<resultCount;i++) {
       memset((void*)&convResult[i], 0 , sizeof(TPOI_FAVORITE_INFO));

       convResult[i].baseInfo.nIdx = result[i].nIdx;
       convResult[i].baseInfo.nPoiID = result[i].nPoiID;
       convResult[i].baseInfo.tp = result[i].tp;
       convResult[i].baseInfo.byRPFlag = result[i].byRPFlag;
       strncpy(convResult[i].baseInfo.szCustName,result[i].szCustName,D_MAXLEN_SISULNAME);
       strncpy(convResult[i].baseInfo.szNavSeq, result[i].szNavSeq, 6);
       strncpy(convResult[i].baseInfo.szAddInfo, result[i].szAddInfo, D_MAXLEN_FAVADDINFO);
       strncpy(convResult[i].baseInfo.szInsDatetime, result[i].szInsDatetime, D_MAXLEN_SVCDATE);

#if 0 // DONOT!, that takes much time for getting remote sync , by lckxx 20191108
        // get poi name and address for specified poiid
        const DS_SEARCH_POI_DETAIL_INFO* detail = getPoiDetailInfoById(result[i].nPoiID,result[i].szNavSeq);
        if (detail != NULL) {
            strncpy(convResult[i].szPoiName, detail->szName, D_MAXLEN_SISULNAME);
            strncpy(convResult[i].szAddress, detail->szAddrName, D_MAXLEN_ADDRNAME);
            delete detail;
        }
#endif
    }
    return convResult;
}


const TPOI_MYHOMEOFFICE_INFO* ServerSearch::conv2TmapMyHomeOfficeInfo(const DS_MYFAVORITE* result)
{
    TPOI_MYHOMEOFFICE_INFO* convArray = new TPOI_MYHOMEOFFICE_INFO[1];

    TPOI_MYHOMEOFFICE_INFO* convResult = &convArray[0];
    memset((void*)convResult, 0, sizeof(TPOI_MYHOMEOFFICE_INFO));
    convResult->baseInfo.Hometp = result->Hometp;
    convResult->baseInfo.nHomePoiID = result->nHomePoiID;
    convResult->baseInfo.byHomeRpFlag = result->byHomeRpFlag;
    convResult->baseInfo.Officetp     = result->Officetp;
    convResult->baseInfo.byOfficeRpFlag  = result->byOfficeRpFlag;
    strncpy(convResult->baseInfo.szHomeCustName, result->szHomeCustName, D_MAXLEN_SISULNAME);
    strncpy(convResult->baseInfo.szHomeNavSeq, result->szHomeNavSeq, 6);
    strncpy(convResult->baseInfo.szHomeAddInfo, result->szHomeAddInfo, D_MAXLEN_FAVADDINFO);
    strncpy(convResult->baseInfo.szHomeInsDatetime, result->szHomeInsDatetime, D_MAXLEN_SVCDATE);
    strncpy(convResult->baseInfo.szOfficeCustName, result->szOfficeCustName, D_MAXLEN_SISULNAME);
    strncpy(convResult->baseInfo.szOfficeNavSeq, result->szOfficeNavSeq, 6);

#if 0 // DONOT!, that takes much time for getting remote sync , by lckxx 20191108
    const DS_SEARCH_POI_DETAIL_INFO* home = getPoiDetailInfoById(result->nHomePoiID,result->szHomeNavSeq);
    if (home != NULL) {
        strncpy(convResult->szHomePoiName, home->szName, D_MAXLEN_SISULNAME);
        strncpy(convResult->szHomeAddress, home->szAddrName, D_MAXLEN_ADDRNAME);
        delete home;
    }

    const DS_SEARCH_POI_DETAIL_INFO* office = getPoiDetailInfoById(result->nOfficePoiID,result->szOfficeNavSeq);
    if (office != NULL) {
        strncpy(convResult->szOfficePoiName, office->szName, D_MAXLEN_SISULNAME);
        strncpy(convResult->szOfficeAddress, office->szAddrName, D_MAXLEN_ADDRNAME);
        delete office;
    }
#endif
    return convArray;
}

const TPOI_RECENTDESTINATION_INFO* ServerSearch::conv2TmapRecentDestinationInfo(
    const DS_SEARCH_RECENT_DESTNATION* result, 
    int resultCount)
{
    TPOI_RECENTDESTINATION_INFO* convResult = NULL;
    convResult = new TPOI_RECENTDESTINATION_INFO[resultCount];
    for (int i=0;i<resultCount;i++) {
        memset((void*)&convResult[i], 0, sizeof(TPOI_RECENTDESTINATION_INFO));

        convResult[i].baseInfo.nPoiID = result[i].nPoiID;
        convResult[i].baseInfo.tp = result[i].tp;
        convResult[i].baseInfo.byRPFlag = result[i].byRPFlag;
        strncpy(convResult[i].baseInfo.szName, result[i].szName, D_MAXLEN_SISULNAME);
        strncpy(convResult[i].baseInfo.szNavSeq, result[i].szNavSeq, 6);

#if 0 // DONOT!, that takes much time for getting remote sync , by lckxx 20191108
        // get poi name and address for specified poiid
        const DS_SEARCH_POI_DETAIL_INFO* detail = getPoiDetailInfoById(result[i].nPoiID,result[i].szNavSeq);
        if (detail != NULL) {
            strncpy(convResult[i].szPoiName, detail->szName, D_MAXLEN_SISULNAME);
            strncpy(convResult[i].szAddress, detail->szAddrName, D_MAXLEN_ADDRNAME);
            delete detail;
        }
#endif
    }

    return convResult;
}


const TPOI_SENDTOCAR_INFO* ServerSearch::conv2TmapFindSendToCarInfo(const DS_SENDTOCARINFO* result, int resultCount)
{
    TPOI_SENDTOCAR_INFO* convResult = NULL;
    convResult = new TPOI_SENDTOCAR_INFO[resultCount];
    for (int i=0;i<resultCount;i++) {
       memset((void*)&convResult[i], 0 , sizeof(TPOI_SENDTOCAR_INFO));

       //convResult[i].baseInfo.nIdx = result[i].nIdx;
       convResult[i].baseInfo.nPoiID = result[i].nPoiID;
       convResult[i].baseInfo.tp = result[i].tp;
       convResult[i].baseInfo.byRPFlag = result[i].byRPFlag;
       strncpy(convResult[i].baseInfo.szCustName,result[i].szCustName,D_MAXLEN_SENDTOCARNAME);
       strncpy(convResult[i].baseInfo.szNavSeq, result[i].szNavSeq, 6);
       //strncpy(convResult[i].baseInfo.szAddInfo, result[i].szAddInfo, D_MAXLEN_FAVADDINFO);
       strncpy(convResult[i].baseInfo.szInsDatetime, result[i].szInsDatetime, D_MAXLEN_SVCDATE);
    }
    return convResult;
}

const TPOI_SENDTOCAR_INFO* ServerSearch::conv2TmapRemoveSendToCarInfo(const DS_SENDTOCARINFO* result, int resultCount)
{
    TPOI_SENDTOCAR_INFO* convResult = NULL;
    convResult = new TPOI_SENDTOCAR_INFO[resultCount];
    for (int i=0;i<resultCount;i++) {
       memset((void*)&convResult[i], 0 , sizeof(TPOI_SENDTOCAR_INFO));

       //convResult[i].baseInfo.nIdx = result[i].nIdx;
       convResult[i].baseInfo.nPoiID = result[i].nPoiID;
       convResult[i].baseInfo.tp = result[i].tp;
       convResult[i].baseInfo.byRPFlag = result[i].byRPFlag;
       strncpy(convResult[i].baseInfo.szCustName,result[i].szCustName,D_MAXLEN_SENDTOCARNAME);
       strncpy(convResult[i].baseInfo.szNavSeq, result[i].szNavSeq, 6);
       //strncpy(convResult[i].baseInfo.szAddInfo, result[i].szAddInfo, D_MAXLEN_FAVADDINFO);
       strncpy(convResult[i].baseInfo.szInsDatetime, result[i].szInsDatetime, D_MAXLEN_SVCDATE);
    }
    return convResult;
}

void ServerSearch::doServerSearch(const QSharedPointer<SearchRequest> &req, SearchReply *reply, DS_SEARCH_REQ &sNddsSearchReq) {
    qDebug() << "[Perf][ServerSearch][doServerSearch]before";

    const int requestCount = req->count();
    const int startIndex = req->startIndex();
    //const bool bNewAddr = req->newAddr();
    setPosition(sNddsSearchReq.sSearchServerAround.tpCurrent);

    DeErrorCode eError;
    int totalCount = 0;

    int nSessionID = 0;
    qDebug() << "[Perf][ServerSearch][doServerSearch][NDDS_Search]before";
    eError = NDDS_Search(&sNddsSearchReq, true, totalCount, &nSessionID);
    qDebug() << "[Perf][ServerSearch][doServerSearch][NDDS_Search]after";
    if (eError != DE_ERROR_SUCCESS) {
        qDebug() << "NDDS_Search eError:" << eError;
        reply->m_error = true;
        reply->m_errorCause = eError;
        sendFinished(reply);
        return;
    }

    if (totalCount > 0) {
        int resultCount = requestCount;
        if (resultCount == 0) {
            resultCount = totalCount;
        }

        DS_SEARCH_LIST* result = new DS_SEARCH_LIST[resultCount];
        if (result) {
            memset(result, 0, resultCount * sizeof(DS_SEARCH_LIST));

            qDebug() << "[Perf][ServerSearch][doServerSearch][NDDS_GetSearchResultList]before";
            eError = NDDS_GetSearchResultList(sNddsSearchReq.eSearchType, startIndex, resultCount, result, &totalCount);
            qDebug() << "[Perf][ServerSearch][doServerSearch][NDDS_GetSearchResultList]after";
            if (eError == DE_ERROR_SUCCESS) {
                qDebug() << "NDDS_GetSearchResultList resultCount:" << resultCount;

                reply->m_results.reset(conv2TmapSearchList(result, resultCount));
                reply->m_resultCount = resultCount;
                reply->m_totalCount = totalCount;
                delete [] result;
            } else {
                qDebug() << "NDDS_GetSearchResultList eError:" << eError;
                delete [] result;
                reply->m_error = true;
                reply->m_errorCause = eError;
            }
        } else {
            reply->m_error = true;
            reply->m_errorCause = DE_ERROR_UNKNOWN;
        }
    } else {
        reply->m_error = false;
        // allocate the dummy data to avoid NPE
        DS_SEARCH_LIST* result = new DS_SEARCH_LIST[1];
        if (result) {
            memset(result, 0, sizeof(DS_SEARCH_LIST));
            reply->m_results.reset(conv2TmapSearchList(result, 1));
            reply->m_resultCount = 0;
            reply->m_totalCount = 0;
        }
    }

    sendFinished(reply);

    qDebug() << "[Perf][ServerSearch][doServerSearch]after";
}


DeAroundPoiType syncCategoryIndex(int nType) {
    // Local과 서버를 같은 타입으로 맞추기 위해서......(그러나 시나리오가 달라서 맞출 수 없음 ) 임시...
    // 카테고리 인덱스 (0:주유소, 1:충전소, 2:주차장, 3:은행, 4:쇼핑, 5:음식점, 6:관광, 7:의료, 8:공공기관, 9: 금융, 10: 전기차 충전소)
    DeAroundPoiType nOilType = DE_AROUNDTYPE_NONE;
    switch(nType) {
        case 0:
            nOilType = DE_AROUNDTYPE_OPI;
            break;
        case 1:
            nOilType = DE_AROUNDTYPE_LPG;
            break;
        case 2:
            nOilType = DE_AROUNDTYPE_PARK;
            break;
        case 3:
            nOilType = DE_AROUNDTYPE_BANK;
            break;
        case 4:
            nOilType = DE_AROUNDTYPE_SHOPPING;
            break;
        case 5:
            nOilType = DE_AROUNDTYPE_RESTAURANT;
            break;
        case 6:
            nOilType = DE_AROUNDTYPE_TOUR;
            break;
        case 7:
            nOilType = DE_AROUNDTYPE_HOSPITAL;
            break;
        case 8: // not used in RSM2018 (UID0.97a)
        case 9: // not used in RSM2018 (UID0.97a)
            nOilType = DE_AROUNDTYPE_NONE;
            break;
        case 10:
             nOilType = DE_AROUNDTYPE_OPI_EV;
             break;
        default:
            nOilType = DE_AROUNDTYPE_NONE;
            break;
    }
    return (DeAroundPoiType)nOilType;
}

int syncDistance(int nDist, DeSearchType searchType) {
    // Local과 서버를 같은 타입으로 맞추기 위해서....(그러나 시나리오가 달라서 맞출 수 없음 ) 임시...
    // @input : 주변 검색 거리 (0:0~2km, 1:2~5km, 2:5~7km, 3:7~10km)
    // @output : 0,1,2,3(input) =>2,5,7,10 (output)
    // NDDS5.0 문서에 5.2.2 주변지 검색에서 radius 필드는 아래와 같습니다.
    // radius		String	Y	검색반경 (0~33) 단위 1=1km default : 0 (센터위임)
    // header에 있는 주석은 무시하시고 nDist 값을 위와 같이 0~33 으로  해보시기 바랍니다.
    int convDist = 0;

    switch(searchType) {
        case DE_SEARCHTYPE_NONE :
        case DE_SEARCHTYPE_SERVER :
        case DE_SEARCHTYPE_SERVERPOIDETAILINFO :
        case DE_SEARCHTYPE_SERVERPOIJOININFO :
        case DE_SEARCHTYPE_SERVERRECENTDESTNATION :
        case DE_SEARCHTYPE_SERVERUPLOADRECENTDESTNATION :
        case DE_SEARCHTYPE_SERVERREMOVERECENTDESTNATION :
        case DE_SEARCHTYPE_SERVERADDR :
        case DE_SEARCHTYPE_SERVERPOISTATIONINFO :
        case DE_SEARCHTYPE_SERVERUSERDATA :
        case DE_SEARCHTYPE_SERVERPOIEVSTATIONINFO :
                convDist = 0; // Server search default == 0
            break;
        case DE_SEARCHTYPE_SERVERPOIEVSTATIONSBYPOINT :
                convDist = 9; // Server search default == 9 (only EV statation)
            break;
        case DE_SEARCHTYPE_SERVERAROUND: //2km
            switch(nDist) {
                case 0: //2km
                    convDist = 2;
                    break;
                case 1: //5km
                    convDist = 5;
                    break;
                case 2: //7km
                    convDist = 7;
                    break;
                case 3: //10km
                    convDist = 10;
                    break;
                default:
                    convDist = 0; // Server search default == 0
                    break;
            }
            break;
        default : {
            convDist = 0; // Server search default == 0
            break;
        }
    }

    return convDist;
}

void ServerSearch::searchByServerSisulName(const QSharedPointer<SearchRequest> &req, SearchReply *reply) {
    DS_SEARCH_REQ sNddsSearchReq;
    memset(&sNddsSearchReq, 0, sizeof(DS_SEARCH_REQ));

    DS_SEARCH_SERVER_REQ *serverSisualName = &sNddsSearchReq.sSearchServer;
    serverSisualName->eSearchType = DE_SEARCHTYPE_SERVER;
    serverSisualName->eSortType = toDeSearchSort(req->sortType());
    serverSisualName->tpCurrent.x = req->x();
    serverSisualName->tpCurrent.y = req->y();
    QByteArray searchName = TextDecoder::instance()->fromUnicode(req->keyword());
    QByteArray searchAddr = TextDecoder::instance()->fromUnicode(req->addressName());
    strncpy(serverSisualName->szSearchName, searchName.constData(), sizeof(serverSisualName->szSearchName));
    strncpy(serverSisualName->szAddrName, searchAddr.constData(), sizeof(serverSisualName->szAddrName));
    serverSisualName->nDist = syncDistance(req->distance(), serverSisualName->eSearchType);
    serverSisualName->eOilType = (DeOilType)req->oilType();
    serverSisualName->bNewAddr = req->newAddr();

    doServerSearch(req, reply, sNddsSearchReq);
}

void ServerSearch::searchByServerSisulAround(const QSharedPointer<SearchRequest> &req, SearchReply *reply) {

    DS_SEARCH_REQ sNddsSearchReq;
    memset(&sNddsSearchReq, 0, sizeof(DS_SEARCH_REQ));

    sNddsSearchReq.eSearchType = DE_SEARCHTYPE_SERVERAROUND;

    DS_SEARCH_SERVERAROUND_REQ *serverSisualAround = &sNddsSearchReq.sSearchServerAround;
    serverSisualAround->tpCurrent.x = req->x();
    serverSisualAround->tpCurrent.y = req->y();
    serverSisualAround->eAroundPoiType = (DeAroundPoiType)syncCategoryIndex(req->categoryIdx());
    serverSisualAround->nDist = syncDistance(req->distance(), sNddsSearchReq.eSearchType);
    serverSisualAround->bNewAddr = req->newAddr();

    doServerSearch(req, reply, sNddsSearchReq);
}

void ServerSearch::searchByAddressVr(const QSharedPointer<SearchRequest> &req, SearchReply *reply)
{
    DS_SEARCH_REQ sNddsSearchReq;
    memset(&sNddsSearchReq, 0, sizeof(DS_SEARCH_REQ));

    auto params = req->addressVrParams();

    QString state = params[QStringLiteral("state")].toString();
    QString place = params[QStringLiteral("place")].toString();
    QString road = params[QStringLiteral("road")].toString();
    QString houseNumber  = params[QStringLiteral("houseNumber")].toString();

    DS_SEARCH_SERVER_REQ *serverSisualName = &sNddsSearchReq.sSearchServer;
    serverSisualName->eSearchType = DE_SEARCHTYPE_SERVER;
    serverSisualName->eSortType = toDeSearchSort(req->sortType());
    serverSisualName->tpCurrent.x = req->x();
    serverSisualName->tpCurrent.y = req->y();
    QByteArray searchName = TextDecoder::instance()->fromUnicode(
                QString("%1 %2 %3 %4").arg(state).arg(place).arg(road).arg(houseNumber));
    QByteArray searchAddr = TextDecoder::instance()->fromUnicode(req->addressName());
    strncpy(serverSisualName->szSearchName, searchName.constData(), sizeof(serverSisualName->szSearchName));
    strncpy(serverSisualName->szAddrName, searchAddr.constData(), sizeof(serverSisualName->szAddrName));
    serverSisualName->nDist = syncDistance(req->distance(), serverSisualName->eSearchType);
    serverSisualName->eOilType = (DeOilType)req->oilType();
    serverSisualName->bNewAddr = req->newAddr();

    doServerSearch(req, reply, sNddsSearchReq);
}

void ServerSearch::searchByServerAddress(const QSharedPointer<SearchRequest> &req, SearchReply *reply) {

    DS_SEARCH_REQ sNddsSearchReq;
    memset(&sNddsSearchReq, 0, sizeof(DS_SEARCH_REQ));

    DS_SEARCH_SERVER_REQ *serverSisualName = &sNddsSearchReq.sSearchServer;
    serverSisualName->eSearchType = DE_SEARCHTYPE_SERVER;
    serverSisualName->eSortType = toDeSearchSort(req->sortType());
    serverSisualName->tpCurrent.x = req->x();
    serverSisualName->tpCurrent.y = req->y();
    QByteArray searchName = TextDecoder::instance()->fromUnicode(
                QString("%1 %2").arg(req->keyword()).arg(req->jibun()));
    QByteArray searchAddr = TextDecoder::instance()->fromUnicode(req->addressName());
    strncpy(serverSisualName->szSearchName, searchName.constData(), sizeof(serverSisualName->szSearchName));
    strncpy(serverSisualName->szAddrName, searchAddr.constData(), sizeof(serverSisualName->szAddrName));
    serverSisualName->nDist = syncDistance(req->distance(), (DeSearchType)serverSisualName->eSearchType);
    serverSisualName->eOilType = (DeOilType)req->oilType();
    serverSisualName->bNewAddr = req->newAddr();

    doServerSearch(req, reply, sNddsSearchReq);
}

void ServerSearch::searchByServerNextAddress(const QSharedPointer<SearchRequest> &req, SearchReply *reply) {

    DS_SEARCH_REQ sNddsSearchReq;
    memset(&sNddsSearchReq, 0, sizeof(DS_SEARCH_REQ));

    sNddsSearchReq.eSearchType = DE_SEARCHTYPE_SERVERADDR;

    DS_SEARCH_SERVERADDR_REQ *serverAddr = &sNddsSearchReq.sSearchServerAddr;
    QByteArray searchName = TextDecoder::instance()->fromUnicode(req->keyword());
    QByteArray searchAddr = TextDecoder::instance()->fromUnicode(req->addressName());
    strncpy(serverAddr->szAddrName1, searchName.constData(), sizeof(serverAddr->szAddrName1));
    strncpy(serverAddr->szAddrName2, searchAddr.constData(), sizeof(serverAddr->szAddrName2));
//    strcpy(sNddsSearchReq.sSearchServerAddr.szAddrName1, "서울"); //(NextAddress로 동작)
//    strcpy(sNddsSearchReq.sSearchServerAddr.szAddrName2, "중구");
    serverAddr->bNewAddr = req->newAddr();

    doServerSearch(req, reply, sNddsSearchReq);
}

int ServerSearch::toServer(int id) {
    //@ M&Service Local Navi Engine에서는 로컬 POI id의 맨 마지막 두 자리를 navSeq로 사용하므로
    //@ Local POI id (최대 999999를 넘는 경우) --> Server POI id 체계로 변환이 필요
    //@ Local : 8자리 (POI id 6자리 + POI Naq 2자리)
    //@ Server : 6자리 (POI id 6자리, Naq는 별도 필드로 분리하여 사용)
    if(id <= 0)
        return 0;

    if(id > 9999999) {
        return id / 100;
    } else {
        return id;
    }
}

void ServerSearch::searchByPOIdetailInfo(const QSharedPointer<SearchRequest> &req, SearchReplyPoiDetail *reply) {
    qDebug() << "[Perf][ServerSearch][searchByPOIdetailInfo]before";

    DS_SEARCH_REQ sNddsSearchReq;
    DeErrorCode eError;
    int nCnt=0;
    memset(&sNddsSearchReq, 0, sizeof(DS_SEARCH_REQ));

    DS_SEARCH_SERVERPOIDETAILINFO_REQ *sPoiDetailInfo = &sNddsSearchReq.sSearchServerPoiDetailInfo;
    sNddsSearchReq.eSearchType = DE_SEARCHTYPE_SERVERPOIDETAILINFO;

    sPoiDetailInfo->nPoiID = PoiHelper::toServer(req->PoiId());
    sPoiDetailInfo->nSizeWidth = 800;       // width, height size fixed
    sPoiDetailInfo->nSizeHeight = 480;
    if ( req->navSeq().isEmpty()) {
        strcpy(sPoiDetailInfo->navSeq, "0");
    } else {
        QByteArray searchNavSeq = TextDecoder::instance()->fromUnicode(req->navSeq());
        strncpy(sPoiDetailInfo->navSeq, searchNavSeq.constData(), sizeof(sPoiDetailInfo->navSeq)-1);
    }

    int nSessionID=0;
    eError = NDDS_Search(&sNddsSearchReq, true, nCnt, &nSessionID);
    if (eError != DE_ERROR_SUCCESS) {
        qDebug() << "NDDS_Search eError:" << eError;
        reply->m_error = true;
        reply->m_errorCause = eError;
        sendFinished(reply);
        return;
    }

    DS_SEARCH_POI_DETAIL_INFO stDetailResult;
    qDebug() << "[Perf][ServerSearch][searchByPOIdetailInfo][NDDS_GetSearchPoiDetailInfo]before";
    NDDS_GetSearchPoiDetailInfo(&stDetailResult);
    qDebug() << "[Perf][ServerSearch][searchByPOIdetailInfo][NDDS_GetSearchPoiDetailInfo]after";

    qDebug() << "-------------------------poi detail info - s -----------------------";
    qDebug() << "stDetailResult.nPoiID" << stDetailResult.nPoiID;
    qDebug() << "stDetailResult.szName" << TextDecoder::instance()->fromCP949(stDetailResult.szName);
    qDebug() << "stDetailResult.szAddrName" << TextDecoder::instance()->fromCP949(stDetailResult.szAddrName);
    qDebug() << "stDetailResult.szPhoneNumber" << TextDecoder::instance()->fromCP949(stDetailResult.szPhoneNumber);
    qDebug() << "stDetailResult.szUrl" << TextDecoder::instance()->fromCP949(stDetailResult.szUrl);
    qDebug() << "stDetailResult.bHoliday" << stDetailResult.bHoliday;
    qDebug() << "stDetailResult.byPark" << stDetailResult.byPark;
    qDebug() << "stDetailResult.szCateName" << TextDecoder::instance()->fromCP949(stDetailResult.szCateName);
    qDebug() << "stDetailResult.szInfo" << TextDecoder::instance()->fromCP949(stDetailResult.szInfo);
    qDebug() << "stDetailResult.sGatherTime" << stDetailResult.m_GatherTime;
    qDebug() << "stDetailResult.szAddInfo" << TextDecoder::instance()->fromCP949(stDetailResult.szAddInfo);
    qDebug() << "stDetailResult.nDataKind " << stDetailResult.nDataKind;
    qDebug() << "stDetailResult.bHighGasoline" << stDetailResult.bHighGasoline;
    qDebug() << "stDetailResult.nOPIGasolinePrice" << stDetailResult.nOPIGasolinePrice;
    qDebug() << "stDetailResult.nOPIDieselPrice" << stDetailResult.nOPIDieselPrice;
    qDebug() << "stDetailResult.nOPILPGPrice" << stDetailResult.nOPILPGPrice;
    qDebug() << "stDetailResult.nOPIHiGasolinePrice" << stDetailResult.nOPIHiGasolinePrice;
    qDebug() << "stDetailResult.nOPIHiLPGPrice" << stDetailResult.nOPIHiLPGPrice;
    qDebug() << "-------------------------poi detail info - e -----------------------";

    if (stDetailResult.nPoiID > 0) {
        reply->m_results.reset(conv2TmapPoiDetailInfo(&stDetailResult, 1));
        reply->m_resultCount = 1;
        qstrncpy(reply->m_results->szNavseq, stDetailResult.szNavSeq, 6);
    } else {
        qDebug() << "NDDS_GetSearchPoiDetailInfo eError:";
        reply->m_error = true;
        reply->m_errorCause = DE_ERROR_SEARCH_POIID;
    }
    sendFinished(reply);

    qDebug() << "[Perf][ServerSearch][searchByPOIdetailInfo]before";
}

void ServerSearch::searchByEvPOIdetailInfo(const QSharedPointer<SearchRequest> &req, SearchReplyEvPoiDetail *reply) {
    qDebug() << "[Perf][ServerSearch][searchByEvPOIdetailInfo]before";

    DS_SEARCH_REQ sNddsSearchReq;
    DeErrorCode eError;
    int nCnt=0;
    memset(&sNddsSearchReq, 0, sizeof(DS_SEARCH_REQ));

    DS_SEARCH_SERVERPOIEVSTATIONINFO_REQ *sPoiDetailInfo = &sNddsSearchReq.sSearchServerPoiEvStationInfo;
    sNddsSearchReq.eSearchType = DE_SEARCHTYPE_SERVERPOIEVSTATIONINFO;

    sPoiDetailInfo->nPoiID = PoiHelper::toServer(req->PoiId());

    int nSessionID=0;
    qDebug() << "[Perf][ServerSearch][searchByEvPOIdetailInfo][NDDS_Search]before";
    eError = NDDS_Search(&sNddsSearchReq, true, nCnt, &nSessionID);
    qDebug() << "[Perf][ServerSearch][searchByEvPOIdetailInfo][NDDS_Search]after";
    if (eError != DE_ERROR_SUCCESS) {
        qDebug() << "NDDS_Search eError:" << eError;
        reply->m_error = true;
        reply->m_errorCause = eError;
        sendFinished(reply);
        return;
    }

    DS_EVSTATION_INFO stDetailResult;
    qDebug() << "[Perf][ServerSearch][searchByEvPOIdetailInfo][NDDS_GetPoiEvStationInfo]before";
    eError = NDDS_GetPoiEvStationInfo(sNddsSearchReq.eSearchType, &stDetailResult);
    qDebug() << "[Perf][ServerSearch][searchByEvPOIdetailInfo][NDDS_GetPoiEvStationInfo]after";
    if (eError != DE_ERROR_SUCCESS) {
        qDebug() << "NDDS_Search eError:" << eError;
        reply->m_error = true;
        reply->m_errorCause = eError;
        sendFinished(reply);
        return;
    }

    qDebug() << "-------------------------poi ev detail info - s -----------------------";
    qDebug() << "stDetailResult.poiId" << stDetailResult.poiId;
    qDebug() << "stDetailResult.szStationName" << TextDecoder::instance()->fromCP949(stDetailResult.szStationName);
    qDebug() << "stDetailResult.szAddr" << TextDecoder::instance()->fromCP949(stDetailResult.szAddr);
    qDebug() << "stDetailResult.szTel" << TextDecoder::instance()->fromCP949(stDetailResult.szTel);
    qDebug() << "stDetailResult.bFreeYn" << stDetailResult.bFreeYn;
    qDebug() << "stDetailResult.bReservYn" << stDetailResult.bReservYn;
    qDebug() << "stDetailResult.szUseTime" << TextDecoder::instance()->fromCP949(stDetailResult.szUseTime);
    qDebug() << "stDetailResult.bPayYn" << stDetailResult.bPayYn;
    qDebug() << "stDetailResult.szUpdateDt" << TextDecoder::instance()->fromCP949(stDetailResult.szUpdateDt);
    qDebug() << "-------------------------poi ev detail info - e -----------------------";

    if (stDetailResult.poiId > 0) {
        reply->m_results.reset(conv2TmapPoiEvDetailInfo(&stDetailResult, 1));
        reply->m_resultCount = 1;
    } else {
        qDebug() << "NDDS_GetPoiEvStationInfo eError:";
        reply->m_error = true;
        reply->m_errorCause = DE_ERROR_SEARCH_POIID;
    }
    sendFinished(reply);

    qDebug() << "[Perf][ServerSearch][searchByEvPOIdetailInfo]after";
}

void ServerSearch::searchPoiStationInfo(const QSharedPointer<SearchRequest> &req, SearchReplyPoiStation *reply) {
    qDebug() << "[Perf][ServerSearch][searchPoiStationInfo]before";

    QVariantMap reqData = req->extra();

    DS_SEARCH_REQ sNddsSearchReq;
    DeErrorCode eError;
    int nCnt=0;
    memset(&sNddsSearchReq, 0, sizeof(DS_SEARCH_REQ));

    QVariantMap params = reqData["params"].value<QVariantMap>();
    int dataKind = params["dataKind"].toInt();
    int fuelKind = params["fuelKind"].toInt();
    QString fuelType = "";
    switch(fuelKind) {
        case N_eOilTypeGasoline:
            fuelType = "FT_GAS";
            break;
        case N_eOilTypeDiesel:
            fuelType = "FT_DIESEL";
            break;
        case N_eOilTypeLPG:
            fuelType = "FT_LPG";
            break;
        case N_eOilTypeEV:
           fuelType = "FT_EV";
            break;
        default:
            fuelType = "FT_GASPM";
            break;
    }

    // 최저가 주유소(충전소) 조회 샘플
    //    DeSearchType eSearchType;
    //    memset(&sNddsSearchReq, 0x00, sizeof(DS_SEARCH_REQ));
    //    eSearchType = DE_SEARCHTYPE_SERVERPOISTATIONINFO;
    //    sNddsSearchReq.eSearchType = eSearchType;
    //    sNddsSearchReq.sSearchServerPoiStationInfo.eSearchType = eSearchType;
    //    sNddsSearchReq.sSearchServerPoiStationInfo.nDist = 9;//req->distance();
    //    sprintf(sNddsSearchReq.sSearchServerPoiStationInfo.szDataKind, "%d", 3);//dataKind);
    //    sprintf(sNddsSearchReq.sSearchServerPoiStationInfo.szFuelType, "%s", "FT_GAS");//fuelType.toLocal8Bit().constData());
    //    sNddsSearchReq.sSearchServerPoiStationInfo.tp.x = 4570966;//req->x();
    //    sNddsSearchReq.sSearchServerPoiStationInfo.tp.y = 1351818;//req->y();

    DS_SEARCH_SERVERPOISTATIONINFO_REQ *sPoiStationInfo = &sNddsSearchReq.sSearchServerPoiStationInfo;
    sNddsSearchReq.eSearchType = DE_SEARCHTYPE_SERVERPOISTATIONINFO;
    sPoiStationInfo->tp.x = req->x();
    sPoiStationInfo->tp.y = req->y();
    sPoiStationInfo->nDist = syncDistance(req->distance(), sNddsSearchReq.eSearchType);
    sprintf(sPoiStationInfo->szDataKind, "%d", dataKind);
    sprintf(sPoiStationInfo->szFuelType, "%s", fuelType.toLocal8Bit().constData());

    int nSessionID=0;
    qDebug() << "[Perf][ServerSearch][searchPoiStationInfo][NDDS_Search]before";
    eError = NDDS_Search(&sNddsSearchReq, true, nCnt, &nSessionID);
    qDebug() << "[Perf][ServerSearch][searchPoiStationInfo][NDDS_Search]after";
    if (eError != DE_ERROR_SUCCESS) {
        qDebug() << "NDDS_Search eError:" << eError;
        reply->m_error = true;
        reply->m_errorCause = eError;
        sendFinished(reply);
        return;
    }


    DS_SEARCH_POI_STATION_INFO stLowestResult;
    qDebug() << "[Perf][ServerSearch][searchPoiStationInfo][NDDS_GetSearchPoiStationInfo]before";
    NDDS_GetSearchPoiStationInfo(&stLowestResult);
    qDebug() << "[Perf][ServerSearch][searchPoiStationInfo][NDDS_GetSearchPoiStationInfo]after";

    qDebug() << "-------------------------poi station info - s -----------------------";
    qDebug() << "stLowestResult.nPoiID" << stLowestResult.nPoiID;
    qDebug() << "stLowestResult.szName" << TextDecoder::instance()->fromCP949(stLowestResult.szName);
    qDebug() << "stLowestResult.nDataKind " << stLowestResult.nDataKind;
    qDebug() << "stLowestResult.nOPIGasolinePrice" << stLowestResult.nOPIGasolinePrice;
    qDebug() << "stLowestResult.nOPIDieselPrice" << stLowestResult.nOPIDieselPrice;
    qDebug() << "stLowestResult.nOPILPGPrice" << stLowestResult.nOPILPGPrice;
    qDebug() << "stLowestResult.nOPIHiGasolinePrice" << stLowestResult.nOPIHiGasolinePrice;
    qDebug() << "stLowestResult.nOPIHiLPGPrice" << stLowestResult.nOPIHiLPGPrice;
    qDebug() << "-------------------------poi detail info - e -----------------------";

    if (stLowestResult.nPoiID > 0) {
        reply->m_results.reset(conv2TmapPoiStationInfo(&stLowestResult, 1));
        reply->m_resultCount = 1;
    } else {
        qDebug() << "NDDS_GetSearchPoiStationInfo eError:";
        reply->m_error = true;
        reply->m_errorCause = DE_ERROR_SEARCH_POIID;
    }
    sendFinished(reply);

    qDebug() << "[Perf][ServerSearch][searchPoiStationInfo]after";
}

void ServerSearch::searchRecentDestination(const QSharedPointer<SearchRequest> &req, SearchRecentDestinationReply *reply)
{
    qDebug() << "[Perf][ServerSearch][searchRecentDestination]before";

    DS_SEARCH_REQ sNddsSearchReq;
    memset(&sNddsSearchReq, 0, sizeof(DS_SEARCH_REQ));
    sNddsSearchReq.eSearchType = DE_SEARCHTYPE_SERVERRECENTDESTNATION;

    const int requestCount = req->count();
    const int startIndex   = req->startIndex();

    DeErrorCode eError;
    int totalCount = 0;

    int nSessionID = 0;
    qDebug() << "[Perf][ServerSearch][searchRecentDestination][NDDS_Search]before";
    eError = NDDS_Search(&sNddsSearchReq, true, totalCount, &nSessionID);
    qDebug() << "[Perf][ServerSearch][searchRecentDestination][NDDS_Search]after";
    if (eError != DE_ERROR_SUCCESS) {
        qDebug() << "NDDS_Search for RecentDest eError:" << eError;
        reply->m_error = true;
        reply->m_errorCause = eError;
        sendFinished(reply);
        return;
    }

    if (totalCount > 0) {
        int resultCount = requestCount;
        if (resultCount == 0 || resultCount > totalCount) {
            resultCount = totalCount;
        }

        DS_SEARCH_RECENT_DESTNATION* result = new DS_SEARCH_RECENT_DESTNATION[resultCount];
        if (result) {
            memset(result, 0, resultCount * sizeof(DS_SEARCH_RECENT_DESTNATION));

            qDebug() << "[Perf][ServerSearch][searchRecentDestination][NDDS_GetSearchRecentDestnationList]before";
            int total = totalCount;
            eError = NDDS_GetSearchRecentDestnationList(startIndex, resultCount, result, &total); // this API reset total count of last parameter
            qDebug() << "[Perf][ServerSearch][searchRecentDestination][NDDS_GetSearchRecentDestnationList]after";
            if (eError == DE_ERROR_SUCCESS) {
                qDebug() << "NDDS_GetSearchRecentDestnationList resultCount:" << resultCount;
                const TPOI_RECENTDESTINATION_INFO* convResult = conv2TmapRecentDestinationInfo(result, resultCount);
                delete [] result;

                if (convResult != NULL) {
                    reply->m_results.reset(const_cast<TPOI_RECENTDESTINATION_INFO*>(convResult));
                    reply->m_resultCount = resultCount;
                    reply->m_totalCount = totalCount;
                } else {
                    reply->m_error = true;
                    reply->m_errorCause = DE_ERROR_UNKNOWN;
                }

            } else {
                qDebug() << "NDDS_GetSearchRecentDestnationList eError:" << eError;
                delete [] result;
                reply->m_error = true;
                reply->m_errorCause = eError;
            }
        } else {
            reply->m_error = true;
            reply->m_errorCause = DE_ERROR_UNKNOWN;
        }
    } else {
            reply->m_resultCount = 0;
            reply->m_totalCount = 0;
    }
    
    sendFinished(reply);

    qDebug() << "[Perf][ServerSearch][searchRecentDestination]after";
}


const DS_SEARCH_POI_DETAIL_INFO* ServerSearch::getPoiDetailInfoById(int poiid, const char* navseq)
{
    qDebug() << "[Perf][ServerSearch][getPoiDetailInfoById]before";

     DS_SEARCH_REQ sNddsSearchReq;
     DS_SEARCH_SERVERPOIDETAILINFO_REQ *detail = &sNddsSearchReq.sSearchServerPoiDetailInfo;
     memset(&sNddsSearchReq,0, sizeof(DS_SEARCH_REQ));
     sNddsSearchReq.eSearchType = DE_SEARCHTYPE_SERVERPOIDETAILINFO;
     detail->nPoiID = poiid;
     if ( navseq && navseq[0] !=0 )
         strncpy(detail->navSeq,navseq,sizeof(detail->navSeq));
     sNddsSearchReq.sSearchServerPoiDetailInfo.nSizeWidth = 800;
     sNddsSearchReq.sSearchServerPoiDetailInfo.nSizeHeight = 480;
     DeErrorCode derror;
     int cnt=0;
     qDebug() << "[Perf][ServerSearch][getPoiDetailInfoById][NDDS_Search]before";
     derror = NDDS_Search(&sNddsSearchReq, true, cnt);
     qDebug() << "[Perf][ServerSearch][getPoiDetailInfoById][NDDS_Search]after";
     if (derror == DE_ERROR_SUCCESS) {
        DS_SEARCH_POI_DETAIL_INFO * result = NULL;
        result = new DS_SEARCH_POI_DETAIL_INFO;
        derror = NDDS_GetSearchPoiDetailInfo(result);
        if (derror == DE_ERROR_SUCCESS) {
            return result;
        }
        delete result;
    }
    qDebug() << "getPoiDetailInfoById failed to get detail for the favorite poi "
    << poiid <<":"<<derror;

    qDebug() << "[Perf][ServerSearch][getPoiDetailInfoById]after";

    return NULL;
}

void ServerSearch::searchFavorites(const SearchRequestBasePtr &req, SearchFavoriteReply *reply)
{
    qDebug() << "[Perf][ServerSearch][searchFavorites]before";

    DS_FAVORITE_REQ sFavoriteReq;
    sFavoriteReq.eFavoriteType = DE_FAVORITETYPE_SERVER;
    sFavoriteReq.sFavoriteServer.bMyFavorite = false;
    const int requestCount = req->count();
    const int startIndex   = req->startIndex();
    int totalCount;
    int nSessionID = 0;
    qDebug() << "[Perf][ServerSearch][searchFavorites][NDDS_Favorite]before";
    DeErrorCode eError = NDDS_Favorite(&sFavoriteReq, true, totalCount, &nSessionID);
    qDebug() << "[Perf][ServerSearch][searchFavorites][NDDS_Favorite]after";
    if (eError != DE_ERROR_SUCCESS) {
        qDebug() << "NDDS_Favorite eError:" << eError;
        reply->m_error = true;
        reply->m_errorCause = eError;
        sendFinished(reply);
        return;
    }

    if (totalCount > 0) {
        int resultCount = requestCount;
        if (resultCount == 0 || resultCount > totalCount) {
            resultCount = totalCount;
        }

        DS_FAVORITE_LIST*   result = new DS_FAVORITE_LIST[resultCount];
        if (result) {
            memset(result, 0, resultCount * sizeof(DS_FAVORITE_LIST));

            qDebug() << "[Perf][ServerSearch][searchFavorites][NDDS_GetFavoriteResultList]before";
            int total = totalCount;
            eError = NDDS_GetFavoriteResultList(sFavoriteReq.eFavoriteType, startIndex, resultCount, result, &total);// this API reset total count of last parameter
            qDebug() << "[Perf][ServerSearch][searchFavorites][NDDS_GetFavoriteResultList]after";
            if (eError == DE_ERROR_SUCCESS && resultCount > 0) {
                qDebug() << "NDDS_GetFavoriteResultList cnt:" << resultCount;

                const TPOI_FAVORITE_INFO* convResult = conv2TmapFavoriteInfo(result, resultCount);
                delete [] result;

                if (convResult != NULL) {
                    reply->m_results.reset(const_cast<TPOI_FAVORITE_INFO*>(convResult));
                    reply->m_resultCount = resultCount;
                    reply->m_totalCount = totalCount;
                } else {
                    reply->m_error = true;
                    reply->m_errorCause = DE_ERROR_UNKNOWN;
                }
            }  else {
                qDebug() << "NDDS_GetFavoriteResultList error:" << eError;
                delete [] result;
                reply->m_error = true;
                reply->m_errorCause = eError;
            }
        } else {
            reply->m_error = true;
            reply->m_errorCause = DE_ERROR_UNKNOWN;
        }
    } else {
        reply->m_resultCount = 0;
        reply->m_totalCount = 0;
    }

    sendFinished(reply);

    qDebug() << "[Perf][ServerSearch][searchFavorites]after";
}


void ServerSearch::searchMyHomeOffice(const SearchRequestBasePtr &req, SearchMyHomeOfficeReply *reply)
{
    qDebug() << "[Perf][ServerSearch][searchMyHomeOffice]before";

    DS_FAVORITE_REQ sFavoriteReq;
    sFavoriteReq.eFavoriteType = DE_FAVORITETYPE_SERVER;
    sFavoriteReq.sFavoriteServer.bMyFavorite = true;
    const int requestCount = req->count();
    const int startIndex   = req->startIndex();
    int totalCount;
    int nSessionID = 0;
    qDebug() << "[Perf][ServerSearch][searchMyHomeOffice][NDDS_Favorite]before";
    DeErrorCode eError = NDDS_Favorite(&sFavoriteReq, true, totalCount, &nSessionID);
    qDebug() << "[Perf][ServerSearch][searchMyHomeOffice][NDDS_Favorite]after";
    if (eError != DE_ERROR_SUCCESS) {
        qDebug() << "NDDS_Favorite eError:" << eError;
        reply->m_error = true;
        reply->m_errorCause = eError;
        sendFinished(reply);
        return;
    }

    DS_MYFAVORITE result;
    memset(&result, 0, sizeof(DS_MYFAVORITE));
    qDebug() << "[Perf][ServerSearch][searchMyHomeOffice][NDDS_GetMyFavorite]before";
    eError = NDDS_GetMyFavorite(&result);
    qDebug() << "[Perf][ServerSearch][searchMyHomeOffice][NDDS_GetMyFavorite]after";
    if (eError == DE_ERROR_SUCCESS) {
        qDebug() << "NDDS_GetMyFavorite okay";
        TPOI_MYHOMEOFFICE_INFO* convResult = (TPOI_MYHOMEOFFICE_INFO*)conv2TmapMyHomeOfficeInfo(&result);
        if (convResult != NULL) {
            // get poi name and address for specified poiid
            reply->m_results.reset(convResult);
        }
        else {
            reply->m_error = true;
            reply->m_errorCause = DE_ERROR_UNKNOWN;
        }
    } else {
        qDebug() << "NDDS_GetMyFavorite error:" << eError;
        reply->m_error = true;
        reply->m_errorCause = eError;
    }

    sendFinished(reply);

    qDebug() << "[Perf][ServerSearch][searchMyHomeOffice]after";
}

void ServerSearch::searchRegisterSendToCar(const SearchRequestBasePtr &req, SearchSendToCarReply *reply)
{
    qDebug() << "[Perf][ServerSearch][searchRegisterSendToCar]before";

    QVariantMap reqData = req->extra();

    DS_FAVORITE_REQ sFavoriteReq;
    memset(&sFavoriteReq, 0x00, sizeof(sFavoriteReq));
    sFavoriteReq.eFavoriteType = DE_FAVORITETYPE_REGISTERSENDTOCARINFO;
    strcpy(sFavoriteReq.sFavoriteRegistSendToCarInfo.szCustName , reqData["alias"].toString().toLocal8Bit().constData());
    sFavoriteReq.sFavoriteRegistSendToCarInfo.tp.x = reqData["x"].toInt();
    sFavoriteReq.sFavoriteRegistSendToCarInfo.tp.y = reqData["y"].toInt();
    sFavoriteReq.sFavoriteRegistSendToCarInfo.byRPFlag = reqData["rpFlag"].toInt();
    sFavoriteReq.sFavoriteRegistSendToCarInfo.nPoiID = reqData["poiID"].toInt();
    strcpy(sFavoriteReq.sFavoriteRegistSendToCarInfo.szNavSeq, reqData["navSeq"].toString().toLocal8Bit().constData());
    const int requestCount = req->count();
    const int startIndex   = req->startIndex();
    int totalCount;
    int nSessionID = 0;
    qDebug() << "[Perf][ServerSearch][searchRegisterSendToCar][NDDS_Favorite]before";
    DeErrorCode eError = NDDS_Favorite(&sFavoriteReq, true, totalCount, &nSessionID);
    qDebug() << "[Perf][ServerSearch][searchRegisterSendToCar][NDDS_Favorite]after";
    if (eError != DE_ERROR_SUCCESS) {
        qDebug() << "NDDS_Favorite eError:" << eError;
        reply->m_error = true;
        reply->m_errorCause = eError;
        sendFinished(reply);
        return;
    }

    if (totalCount > 0) {
        int resultCount = requestCount;
        if (resultCount == 0 || resultCount > totalCount) {
            resultCount = totalCount;
        }
        reply->m_resultCount = resultCount;
        reply->m_totalCount = totalCount;
    } else {
        reply->m_resultCount = 0;
        reply->m_totalCount = 0;
    }

    sendFinished(reply);

    qDebug() << "[Perf][ServerSearch][searchRegisterSendToCar]after";
}


void ServerSearch::searchFindSendToCar(const SearchRequestBasePtr &req, SearchSendToCarReply *reply)
{
    qDebug() << "[Perf][ServerSearch][searchFindSendToCar]before";

    DS_FAVORITE_REQ sFavoriteReq;
    memset(&sFavoriteReq, 0x00, sizeof(sFavoriteReq));
    sFavoriteReq.eFavoriteType = DE_FAVORITETYPE_FINDSENDTOCARINFO;
    sFavoriteReq.sFavoriteFindSendToCarInfo.bNewAddr = false;
    const int requestCount = req->count();
    const int startIndex   = req->startIndex();
    int totalCount;
    int nSessionID = 0;
    qDebug() << "[Perf][ServerSearch][searchFindSendToCar][NDDS_Favorite]before";
    DeErrorCode eError = NDDS_Favorite(&sFavoriteReq, true, totalCount, &nSessionID);
    qDebug() << "[Perf][ServerSearch][searchFindSendToCar][NDDS_Favorite]after";
    if (eError != DE_ERROR_SUCCESS) {
        qDebug() << "NDDS_Favorite eError:" << eError;
        reply->m_error = true;
        reply->m_errorCause = eError;
        sendFinished(reply);
        return;
    }

    if (totalCount > 0) {
        int resultCount = requestCount;
        if (resultCount == 0 || resultCount > totalCount) {
            resultCount = totalCount;
        }

        DS_SENDTOCARINFO*   result = new DS_SENDTOCARINFO[resultCount];
        if (result) {
            memset(result, 0, resultCount * sizeof(DS_SENDTOCARINFO));

            qDebug() << "[Perf][ServerSearch][searchFindSendToCar][NDDS_GetSendToCarInfo]before";
            eError = NDDS_GetSendToCarInfo(result);
            qDebug() << "[Perf][ServerSearch][searchFindSendToCar][NDDS_GetSendToCarInfo]after";
            if (eError == DE_ERROR_SUCCESS && resultCount > 0) {
                qDebug() << "NDDS_GetSendToCarInfo cnt:" << resultCount;

                TPOI_SENDTOCAR_INFO* convResult = (TPOI_SENDTOCAR_INFO*)conv2TmapFindSendToCarInfo(result, resultCount);
                delete [] result;

                if (convResult != NULL) {
                   // get poi name and address for specified poiid
                    for (int i=0;i<resultCount;i++) {
                        const DS_SEARCH_POI_DETAIL_INFO* detail = getPoiDetailInfoById(result[i].nPoiID,result[i].szNavSeq);
                        if (detail != NULL) {
                            strncpy(convResult[i].szPoiName, detail->szName, D_MAXLEN_SISULNAME);
                            strncpy(convResult[i].szAddress, detail->szAddrName, D_MAXLEN_ADDRNAME);
                            delete detail;
                        }
                    }
                    reply->m_results.reset(convResult);
                    reply->m_resultCount = resultCount;
                    reply->m_totalCount = totalCount;
                } else {
                    reply->m_error = true;
                    reply->m_errorCause = DE_ERROR_UNKNOWN;
                }
            }  else {
                qDebug() << "NDDS_GetFavoriteResultList error:" << eError;
                delete [] result;
                reply->m_error = true;
                reply->m_errorCause = eError;
            }
        } else {
            reply->m_error = true;
            reply->m_errorCause = DE_ERROR_UNKNOWN;
        }
    } else {
        reply->m_resultCount = 0;
        reply->m_totalCount = 0;
    }

    sendFinished(reply);

    qDebug() << "[Perf][ServerSearch][searchFindSendToCar]after";
}

void ServerSearch::searchRemoveSendToCar(const SearchRequestBasePtr &req, SearchSendToCarReply *reply)
{
    qDebug() << "[Perf][ServerSearch][searchRemoveSendToCar]before";

    QVariantMap reqData = req->extra();

    DS_FAVORITE_REQ sFavoriteReq;
    memset(&sFavoriteReq, 0x00, sizeof(sFavoriteReq));
    sFavoriteReq.eFavoriteType = DE_FAVORITETYPE_REMOVESENDTOCAR;
    strcpy(sFavoriteReq.sFavoriteRemoveSendToCarInfo.szCustName , reqData["alias"].toByteArray().constData());
           //reqData["alias"].toString().toLocal8Bit().constData());
    sFavoriteReq.sFavoriteRemoveSendToCarInfo.tp.x = reqData["x"].toInt();
    sFavoriteReq.sFavoriteRemoveSendToCarInfo.tp.y = reqData["y"].toInt();
    //sFavoriteReq.sFavoriteRemoveSendToCarInfo.byRPFlag = reqData["RPFlag"].toInt();
    //sFavoriteReq.sFavoriteRemoveSendToCarInfo.nPoiID = reqData["PoiID"].toInt();
    //strcpy(sFavoriteReq.sFavoriteRemoveSendToCarInfo.szNavSeq, reqData["NavSeq"].toString().toLocal8Bit().constData());


    const int requestCount = req->count();
    const int startIndex   = req->startIndex();
    int totalCount;
    int nSessionID = 0;
    qDebug() << "[Perf][ServerSearch][searchRemoveSendToCar][NDDS_Favorite]before";
    DeErrorCode eError = NDDS_Favorite(&sFavoriteReq, true, totalCount, &nSessionID);
    qDebug() << "[Perf][ServerSearch][searchRemoveSendToCar][NDDS_Favorite]after";
    if (eError != DE_ERROR_SUCCESS) {
        qDebug() << "NDDS_Favorite eError:" << eError;
        reply->m_error = true;
        reply->m_errorCause = eError;
        sendFinished(reply);
        return;
    }

    if (totalCount > 0) {
        int resultCount = requestCount;
        if (resultCount == 0 || resultCount > totalCount) {
            resultCount = totalCount;
        }
        reply->m_resultCount = resultCount;
        reply->m_totalCount = totalCount;
    } else {
        reply->m_resultCount = 0;
        reply->m_totalCount = 0;
    }

    sendFinished(reply);

    qDebug() << "[Perf][ServerSearch][searchRemoveSendToCar]after";
}

DeServerSearchSort ServerSearch::toDeSearchSort(SearchRequestBase::SortType sortType) {
    DeServerSearchSort t;

    switch (sortType) {
    default:
    case SearchRequestBase::SortTypeByServerAccuracy:
        t = DE_SERVER_SORTTYPE_ACCURACY;
        break;
    case SearchRequestBase::SortTypeByDist:
    case SearchRequestBase::SortTypeByServerDist:
        t = DE_SERVER_SORTTYPE_DIST;
        break;
    }
    return t;
}


void ServerSearch::uploadAllRecentDestination( QSharedPointer<UploadAllRecentDestinationRequest> req, UploadReplyBase *reply )
{
    qDebug() << "[Perf][ServerSearch][uploadAllRecentDestination]before";

    DS_SEARCH_REQ uploadReq;
    memset(&uploadReq, 0, sizeof(DS_SEARCH_REQ));
    int cnt = req->uploadCount();
    if (cnt == 0 || req->m_data.isNull()) {
        reply->m_error = true;
        reply->m_errorCause = DE_ERROR_INVALID_PARAM;
        sendFinished(static_cast<SearchReplyBase*>(reply));
        return;
    }

    uploadReq.eSearchType = DE_SEARCHTYPE_SERVERUPLOADRECENTDESTNATION;
    uploadReq.sSearchUploadRecentDestnation.nUploadCnt = cnt;
    TPOI_UPLOAD_RECENTDESTINATION* reqdata = req->m_data.data();

    for (int i=0;i<cnt&&i<100; i++) {
        uploadReq.sSearchUploadRecentDestnation.sUploadRecent[i].tp          = reqdata[i].recentdest.tp;
        uploadReq.sSearchUploadRecentDestnation.sUploadRecent[i].nPoiID      = reqdata[i].recentdest.nPoiID;
        uploadReq.sSearchUploadRecentDestnation.sUploadRecent[i].byRPFlag    = reqdata[i].recentdest.byRPFlag;
        strcpy(uploadReq.sSearchUploadRecentDestnation.sUploadRecent[i].szName, reqdata[i].recentdest.szName);
        strcpy(uploadReq.sSearchUploadRecentDestnation.sUploadRecent[i].szNavseq, reqdata[i].recentdest.szNavseq);
        strcpy(uploadReq.sSearchUploadRecentDestnation.sUploadRecent[i].nToltalCnt, reqdata[i].recentdest.nToltalCnt);
        strcpy(uploadReq.sSearchUploadRecentDestnation.sUploadRecent[i].szSvcDate, reqdata[i].recentdest.szSvcDate);
    }

    int nResult = 0;
    int sessionId = 0;

    qDebug() << "[Perf][ServerSearch][uploadAllRecentDestination][NDDS_Search]before";
    DeErrorCode dErr = NDDS_Search( &uploadReq, true, nResult, &sessionId );
    qDebug() << "[Perf][ServerSearch][uploadAllRecentDestination][NDDS_Search]after";
    if (dErr != DE_ERROR_SUCCESS) {
        qDebug() << "NDDS_Search upload error :" << dErr;
        reply->m_error = true;
        reply->m_errorCause = dErr;
        sendFinished(static_cast<SearchReplyBase*>(reply));
        return;
    }
    else {
        reply->m_error = false;
        reply->m_errorCause = DE_ERROR_SUCCESS;
        reply->m_uploadedCount = cnt;
    }

    sendFinished(static_cast<SearchReplyBase*>(reply));

    qDebug() << "[Perf][ServerSearch][uploadAllRecentDestination]after";
}

void ServerSearch::uploadAllFavorite( QSharedPointer<UploadAllFavoriteRequest> req, UploadReplyBase* reply )
{
    qDebug() << "[Perf][ServerSearch][uploadAllFavorite]before";

    DS_FAVORITE_REQ uploadReq;
    memset(&uploadReq, 0, sizeof(DS_FAVORITE_REQ));
    int cnt = req->uploadCount();
    if (req->m_data.isNull() && req->m_UploadOption == QStringLiteral("FI") ) {
        reply->m_error = true;
        reply->m_errorCause = DE_ERROR_INVALID_PARAM;
        sendFinished(static_cast<SearchReplyBase*>(reply));
        return;
    }

    uploadReq.eFavoriteType = DE_FAVORITETYPE_SERVER_UPLOAD;
    uploadReq.sUploadFavoriteServer.bMyFavorite = (req->m_UploadOption != QStringLiteral("FI")); // home office
    uploadReq.sUploadFavoriteServer.nUploadPoiCnt = cnt;
    qstrncpy(uploadReq.sUploadFavoriteServer.szUploadOption, req->m_UploadOption.toLatin1().constData(), 7 );

    DS_UPLOAD_POIMYFAVORITE* homeoffice = &uploadReq.sUploadFavoriteServer.sUploadpoiMyFavorite;
    homeoffice->Hometp     = req->m_homeoffice.homeoffice.Hometp;
    homeoffice->nHomePoiID = req->m_homeoffice.homeoffice.nHomePoiID;
    homeoffice->byHomeRpFlag = req->m_homeoffice.homeoffice.byHomeRpFlag;
    homeoffice->Officetp     = req->m_homeoffice.homeoffice.Officetp;
    homeoffice->nOfficePoiID = req->m_homeoffice.homeoffice.nOfficePoiID;
    homeoffice->byOfficeRpFlag = req->m_homeoffice.homeoffice.byOfficeRpFlag;
    strcpy(homeoffice->szHomeCustName, req->m_homeoffice.homeoffice.szHomeCustName);
    strcpy(homeoffice->szHomeNavSeq, req->m_homeoffice.homeoffice.szHomeNavSeq);
    strcpy(homeoffice->szHomeAddInfo, req->m_homeoffice.homeoffice.szHomeAddInfo);
    strcpy(homeoffice->szOfficeCustName, req->m_homeoffice.homeoffice.szOfficeCustName);
    strcpy(homeoffice->szOfficeNavSeq,  req->m_homeoffice.homeoffice.szOfficeNavSeq);
    strcpy(homeoffice->szOfficeAddInfo, req->m_homeoffice.homeoffice.szOfficeAddInfo);

    DS_UPLOAD_POIFAVORITE* favs = uploadReq.sUploadFavoriteServer.sUploadFavoriteInfo;
    const TPOI_UPLOAD_FAVORITE*  reqfavs   = req->m_data.data();
    for (int i=0; i<cnt && i < D_MAXELN_FAVORITELIST; i++) {
        favs[i].tp    =   reqfavs[i].favorite.tp;
        favs[i].nPoiID    = reqfavs[i].favorite.nPoiID;
        favs[i].byRPFlag  = reqfavs[i].favorite.byRPFlag;
        strcpy(favs[i].szCustName, reqfavs[i].favorite.szCustName);
        strcpy(favs[i].szNavSeq, reqfavs[i].favorite.szNavSeq);
        strcpy(favs[i].szAddInfo, reqfavs[i].favorite.szAddInfo);
        strcpy(favs[i].szOrgCustName, reqfavs[i].favorite.szOrgCustName);
        strcpy(favs[i].szIconInfo, reqfavs[i].favorite.szIconInfo);
        strcpy(favs[i].szInsDatetime, reqfavs[i].favorite.szInsDatetime);
    }

    int result;
    int sessionId = 0;
    qDebug() << "[Perf][ServerSearch][uploadAllFavorite][NDDS_Favorite]before";
    DeErrorCode dErr = NDDS_Favorite(&uploadReq, true, result, &sessionId);
    qDebug() << "[Perf][ServerSearch][uploadAllFavorite][NDDS_Favorite]after";
    if (dErr != DE_ERROR_SUCCESS) {
        qDebug() << "NDDS_Favorite upload error :" << dErr;
        reply->m_error = true;
        reply->m_errorCause = dErr;
        sendFinished(static_cast<SearchReplyBase*>(reply));
        return;
    }
    else {
        reply->m_error = false;
        reply->m_errorCause = DE_ERROR_SUCCESS;
        reply->m_uploadedCount = cnt;
    }
    sendFinished(static_cast<SearchReplyBase*>(reply));

    qDebug() << "[Perf][ServerSearch][uploadAllFavorite]after";
}

void ServerSearch::registMultiFavorites(QSharedPointer<RegistMultiFavoritesRequest> req, UploadReplyBase* reply )
{
    qDebug() << "[Perf][ServerSearch][registMultiFavorites]before";

    DS_FAVORITE_REQ uploadReq;
    memset(&uploadReq, 0, sizeof(DS_FAVORITE_REQ));
    int cnt = req->uploadCount();
    if (cnt == 0 || req->m_data.isNull()) {
        reply->m_error = true;
        reply->m_errorCause = DE_ERROR_INVALID_PARAM;
        sendFinished(static_cast<SearchReplyBase*>(reply));
        return;
    }

    uploadReq.eFavoriteType = DE_FAVORITETYPE_SERVER_REGIST;
    uploadReq.sRegistFavoriteServer.nPoiRegistCnt = cnt;
    DS_REGIST_FAVORITE_LIST* favs = uploadReq.sRegistFavoriteServer.sRegistFavoriteInfo;
    TPOI_REGIST_FAVORITE* reqfavs = req->m_data.data();
    for (int i=0; i < cnt && i < D_MAXELN_FAVORITELIST; i++ ) {
        favs[i].eFavoriteType = reqfavs[i].favorite.eFavoriteType;
        favs[i].tp            = reqfavs[i].favorite.tp;
        favs[i].nPoiID        = reqfavs[i].favorite.nPoiID;
        favs[i].byRPFlag      = reqfavs[i].favorite.byRPFlag;
        strcpy(favs[i].szCustName, reqfavs[i].favorite.szCustName);
        strcpy(favs[i].szNavSeq,   reqfavs[i].favorite.szNavSeq);
        strcpy(favs[i].szAddInfo,  reqfavs[i].favorite.szAddInfo);
        strcpy(favs[i].szOrgCustName, reqfavs[i].favorite.szOrgCustName);
        strcpy(favs[i].szIconInfo, reqfavs[i].favorite.szIconInfo);
    }

    int result;
    int sessionId = 0;
    qDebug() << "[Perf][ServerSearch][registMultiFavorites][NDDS_Favorite]before";
    DeErrorCode dErr = NDDS_Favorite(&uploadReq, true, result, &sessionId);
    qDebug() << "[Perf][ServerSearch][registMultiFavorites][NDDS_Favorite]after";
    if (dErr != DE_ERROR_SUCCESS) {
        qDebug() << "NDDS_Favorite upload error :" << dErr;
        reply->m_error = true;
        reply->m_errorCause = dErr;
        sendFinished(static_cast<SearchReplyBase*>(reply));
        return;
    }
    else {
        reply->m_error = false;
        reply->m_errorCause = DE_ERROR_SUCCESS;
        reply->m_uploadedCount = cnt;
    }
    sendFinished(static_cast<SearchReplyBase*>(reply));

    qDebug() << "[Perf][ServerSearch][registMultiFavorites]after";
}


void ServerSearch::removeMultiFavorites(QSharedPointer<RemoveMultiFavoritesRequest> req, UploadReplyBase* reply )
{
    qDebug() << "[Perf][ServerSearch][removeMultiFavorites]before";

    DS_FAVORITE_REQ removeReq;
    memset(&removeReq, 0, sizeof(DS_FAVORITE_REMOVE_REQ));
    int cnt = req->uploadCount();

    if (cnt == 0 || req->m_data.isNull()) {
        reply->m_error = true;
        reply->m_errorCause = DE_ERROR_INVALID_PARAM;
        sendFinished(static_cast<SearchReplyBase*>(reply));
        return;
    }

    removeReq.eFavoriteType = DE_FAVORITETYPE_SERVER_REMOVE;
    removeReq.sRemoveFavoriteServer.nRemovePoiRecentCnt = cnt;
    DS_REMOVE_POIFAVORITE* favs = removeReq.sRemoveFavoriteServer.sRemoveFavoriteInfo;
    TPOI_REMOVE_FAVORITE* reqfavs = req->m_data.data();
    for (int i=0; i < cnt && i < D_MAXELN_FAVORITELIST; i++ ) {
        favs[i].tp            = reqfavs[i].favorite.tp;
        strcpy(favs[i].szCustName, reqfavs[i].favorite.szCustName);
    }

    int result;
    int sessionId = 0;
    DeErrorCode dErr = NDDS_Favorite(&removeReq, true, result, &sessionId);
    if (dErr != DE_ERROR_SUCCESS) {
        qDebug() << "NDDS_Favorite upload error :" << dErr;
        reply->m_error = true;
        reply->m_errorCause = dErr;
        sendFinished(static_cast<SearchReplyBase*>(reply));
        return;
    }
    else {
        reply->m_error = false;
        reply->m_errorCause = DE_ERROR_SUCCESS;
        reply->m_uploadedCount = cnt;
    }
    sendFinished(static_cast<SearchReplyBase*>(reply));

    qDebug() << "[Perf][ServerSearch][removeMultiFavorites]after";
}


void ServerSearch::modifyFavorite(QSharedPointer<ModifyFavoriteRequest> req, UploadReplyBase* reply )
{
    qDebug() << "[Perf][ServerSearch][modifyFavorite]before";

    DS_FAVORITE_REQ modifyReq;
    memset(&modifyReq, 0, sizeof(DS_FAVORITE_REQ));
    int cnt = req->uploadCount();
    if (cnt == 0 || req->m_data.isNull()) {
        reply->m_error = true;
        reply->m_errorCause = DE_ERROR_INVALID_PARAM;
        sendFinished(static_cast<SearchReplyBase*>(reply));
        return;
    }

    modifyReq.eFavoriteType = DE_FAVORITETYPE_SERVER_MODIFY;
    modifyReq.sRegistFavoriteServer.nPoiRegistCnt = cnt;
    DS_MODIFY_POIFAVORITE* fav = &modifyReq.sModifyFavoriteServer.sModifyFavoriteInfo;
    TPOI_MODIFY_FAVORITE* reqfav = req->m_data.data();
    fav->eFavoriteType = reqfav->favorite.eFavoriteType;
    fav->tp            = reqfav->favorite.tp;
    strcpy(fav->szCustName, reqfav->favorite.szCustName);

    int result;
    int sessionId = 0;
    qDebug() << "[Perf][ServerSearch][modifyFavorite][NDDS_Favorite]before";
    DeErrorCode dErr = NDDS_Favorite(&modifyReq, true, result, &sessionId);
    qDebug() << "[Perf][ServerSearch][modifyFavorite][NDDS_Favorite]after";
    if (dErr != DE_ERROR_SUCCESS) {
        qDebug() << "NDDS_Favorite upload error :" << dErr;
        reply->m_error = true;
        reply->m_errorCause = dErr;
        sendFinished(static_cast<SearchReplyBase*>(reply));
        return;
    }
    else {
        reply->m_error = false;
        reply->m_errorCause = DE_ERROR_SUCCESS;
        reply->m_uploadedCount = cnt;
    }
    sendFinished(static_cast<SearchReplyBase*>(reply));

    qDebug() << "[Perf][ServerSearch][modifyFavorite]after";
}

void ServerSearch::removeMultiRecentDestinations(QSharedPointer<RemoveMultiRecentDestinationsRequest> req, UploadReplyBase* reply)
{
    qDebug() << "[Perf][ServerSearch][removeMultiRecentDestinations]before";

    DS_SEARCH_REQ removeReq;
    memset(&removeReq, 0, sizeof(DS_SEARCH_REQ));
    int cnt = req->uploadCount();
    if (cnt == 0 || req->m_data.isNull()) {
        reply->m_error = true;
        reply->m_errorCause = DE_ERROR_INVALID_PARAM;
        sendFinished(static_cast<SearchReplyBase*>(reply));
        return;
    }

    removeReq.eSearchType = DE_SEARCHTYPE_SERVERREMOVERECENTDESTNATION;
    removeReq.sSearchRemoveRecentDestnation.nRemoveCnt = cnt;
    DS_SEARCH_REMOVE_RECENT_DESTNATION* dests = removeReq.sSearchRemoveRecentDestnation.sRemoveRecent;
    TPOI_REMOVE_RECENTDESTINATION* reqdata = req->m_data.data();

    for (int i=0;i<cnt&&i<100; i++) {
        dests[i].tp          = reqdata[i].recentdest.tp;
        strcpy(dests[i].szName, reqdata[i].recentdest.szName);
    }

    int nResult = 0;
    int sessionId = 0;

    qDebug() << "[Perf][ServerSearch][removeMultiRecentDestinations][NDDS_Search]before";
    DeErrorCode dErr = NDDS_Search( &removeReq, true, nResult, &sessionId );
    qDebug() << "[Perf][ServerSearch][removeMultiRecentDestinations][NDDS_Search]after";
    if (dErr != DE_ERROR_SUCCESS) {
        qDebug() << "NDDS_Search upload error :" << dErr;
        reply->m_error = true;
        reply->m_errorCause = dErr;
        sendFinished(static_cast<SearchReplyBase*>(reply));
        return;
    }
    else {
        reply->m_error = false;
        reply->m_errorCause = DE_ERROR_SUCCESS;
        reply->m_uploadedCount = cnt;
    }

    sendFinished(static_cast<SearchReplyBase*>(reply));

    qDebug() << "[Perf][ServerSearch][removeMultiRecentDestinations]after";
}

void ServerSearch::searchByEvAround(const QSharedPointer<SearchRequest> &req, SearchReply *reply) {
    DeErrorCode eError1;
    int nCnt=0;
    DS_SEARCH_REQ sSearchReq;
    DeSearchType eSearchType;
    memset(&sSearchReq, 0x00, sizeof(DS_SEARCH_REQ));

    eSearchType = DE_SEARCHTYPE_SERVERPOIEVSTATIONSBYPOINT;
    sSearchReq.eSearchType = eSearchType;
    sSearchReq.sSearchServerPoiEvStationsByPoint.nDist = syncDistance(req->distance(), eSearchType);
    sSearchReq.sSearchServerPoiEvStationsByPoint.tp.x = req->x();
    sSearchReq.sSearchServerPoiEvStationsByPoint.tp.y = req->y();

    eError1 = NDDS_Search(&sSearchReq, true, nCnt);

    if(eError1 != DE_ERROR_SUCCESS)
    {
        qDebug() << "NDDS_Search eError:" << eError1;
        reply->m_error = true;
        reply->m_errorCause = eError1;
        sendFinished(reply);
        return;
    }

    const int startIndex = req->startIndex();
    const int requestCount = req->count();
    if (nCnt > 0) {
        int resultCount = requestCount;
        if (resultCount == 0) {
            resultCount = nCnt;
        }

        DS_SEARCH_LIST* result = new DS_SEARCH_LIST[resultCount];
        if (result) {
            memset(result, 0, resultCount * sizeof(DS_SEARCH_LIST));

            eError1 = NDDS_GetSearchResultList(eSearchType, startIndex, resultCount, result);
            if (eError1 == DE_ERROR_SUCCESS) {
                qDebug() << "NDDS_GetSearchResultList resultCount:" << resultCount;
                for(int i=0; i<10; i++)
                {
                     qDebug() << "BYPOINT " << result[i].szStdName << ","<<result[i].tp.x << "," << result[i].tp.y;
                }

                reply->m_results.reset(conv2TmapSearchList(result, resultCount));
                reply->m_resultCount = resultCount;
                reply->m_totalCount = nCnt;
                delete [] result;
            } else {
                qDebug() << "NDDS_GetSearchResultList eError:" << eError1;
                delete [] result;
                reply->m_error = true;
                reply->m_errorCause = eError1;
            }
        } else {
            reply->m_error = true;
            reply->m_errorCause = DE_ERROR_UNKNOWN;
        }
    } else {
        reply->m_error = false;
        // allocate the dummy data to avoid NPE
        DS_SEARCH_LIST* result = new DS_SEARCH_LIST[1];
        if (result) {
            memset(result, 0, sizeof(DS_SEARCH_LIST));
            reply->m_results.reset(conv2TmapSearchList(result, 1));
            reply->m_resultCount = 0;
            reply->m_totalCount = 0;
        }
    }

    sendFinished(reply);
}


void ServerSearch::searchBySisulNamePrediction(const QSharedPointer<SearchNotNddsRequest> &req,SearchSisulNamePredictionReply *reply)
{
    DeErrorCode eError1;
    QString url = req->singleUrl();
    QString domain = req->hostDomain();
    TSisulNamePrediction *sisulNamePrediction = new TSisulNamePrediction[1];
    memset(sisulNamePrediction[0].rawdata, 0, TSisulNamePrediction::BYTES);
    int nResult = 0;

    eError1 = NDDS_NonNdds(true, url.toUtf8().constData(), domain.toUtf8().constData(), sisulNamePrediction[0].rawdata, nResult);

    if(eError1 != DE_ERROR_SUCCESS)
    {
        reply->m_error = true;
        reply->m_errorCause = eError1;
        sendFinished(reply);
        delete [] sisulNamePrediction;
        return;
    }

    reply->m_results.reset(sisulNamePrediction);
    sendFinished(reply);
}

void ServerSearch::searchByGeoCoding(const QSharedPointer<SearchNotNddsRequest> &req,SearchGeoCodingReply *reply)
{
    DeErrorCode eError1;
    QString url = req->singleUrl();
    QString domain = req->hostDomain();
    TGeoCoding *geoCode = new TGeoCoding[1];
    memset(geoCode[0].rawdata, 0, TGeoCoding::BYTES);
    int nResult = 0;

    eError1 = NDDS_NonNdds(true, url.toUtf8().constData(), domain.toUtf8().constData(), geoCode[0].rawdata, nResult);

    if(eError1 != DE_ERROR_SUCCESS)
    {
        reply->m_error = true;
        reply->m_errorCause = eError1;
        delete [] geoCode;
        sendFinished(reply);
        return;
    }

    reply->m_results.reset(geoCode);
    sendFinished(reply);
}

void ServerSearch::searchByComplexImage(const QSharedPointer<SearchNotNddsRequest> &req,SearchComplexImageReply *reply)
{
    DeErrorCode eError1;
    QString url = req->singleUrl();
    QString domain = req->hostDomain();
    TComplexImage *complexImage = new TComplexImage[1];

    memset(complexImage[0].rawdata, 0, TComplexImage::BYTES);

    eError1 = NDDS_NonNdds(true, url.toUtf8().constData(), domain.toUtf8().constData(), complexImage[0].rawdata, complexImage->size);

    if(eError1 != DE_ERROR_SUCCESS)
    {
        reply->m_error = true;
        reply->m_errorCause = eError1;
        delete [] complexImage;
        sendFinished(reply);
        return;
    }

    reply->m_results.reset(complexImage);
    sendFinished(reply);
}

void ServerSearch::searchByAQI(const QSharedPointer<SearchNotNddsRequest> &req,SearchAqiReply *reply)
{
    DeErrorCode eError1;
    QString url = req->singleUrl();
    QString domain = req->hostDomain();
    TAqi *aqi = new TAqi[1];
    memset(aqi[0].rawdata, 0, TAqi::BYTES);
    int nResult = 0;

    eError1 = NDDS_NonNdds(true, url.toUtf8().constData(), domain.toUtf8().constData(), aqi[0].rawdata, nResult);

    if(eError1 != DE_ERROR_SUCCESS)
    {
        reply->m_error = true;
        reply->m_errorCause = eError1;
        sendFinished(reply);
        delete [] aqi;
        return;
    }

    reply->m_results.reset(aqi);
    sendFinished(reply);
}



} //namespace SKT
