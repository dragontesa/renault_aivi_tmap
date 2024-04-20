#include "simplesearch.h"
#include "searchmanager.h"
#include "searchmanagertest.h"
#include "textdecoder.h"
#include "NDDS_Interface.h"
#include "MNS_Interface.h"
// test comment
#include <QDebug>

#include "VSM_Map.h"
#include <QtMath>
#include <QElapsedTimer>
#include <unistd.h>


namespace SKT {

#define TD_MAXLEN_KEYWORD   40

SimpleSearch* SimpleSearch::s_instance = NULL;

SimpleSearch::SimpleSearch()
{

}

SimpleSearch::~SimpleSearch()
{

}

SimpleSearch* SimpleSearch::sharedInstance()
{
    if (!s_instance) {
        s_instance = (SimpleSearch*) new SimpleSearch();
    }
    return s_instance;
}

void SimpleSearch::sendFinished(SearchReplyBase *reply) {
    QMetaObject::invokeMethod(reply, "finished", Qt::QueuedConnection);
}

TPOI_SEARCH_LIST* SimpleSearch::conv2TmapSearchList(TS_SEARCH_LIST* result, int resultCount) {

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
        convResult[i].nOPIPoleCate  = result[i].nOPIPoleCate;
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

        convResult[i].nPrimaryMax           = result[i].sAddrRange.nPrimaryMax;
        convResult[i].nPrimaryMin           = result[i].sAddrRange.nPrimaryMin;
        convResult[i].nSecondaryMax         = result[i].sAddrRange.nSecondaryMax;
        convResult[i].nSecondaryMin         = result[i].sAddrRange.nSecondaryMin;
//        qDebug() << "hhtest conv2TmapSearchList]" << result[i].sAddrRange.nPrimaryMin << "-" << result[i].sAddrRange.nSecondaryMin << " ~ " << result[i].sAddrRange.nPrimaryMax << "-" << result[i].sAddrRange.nSecondaryMax;

        //reserved for ndds
        memset(convResult[i].szOilCompany, 0x0, sizeof(convResult[i].szOilCompany)-1);
        convResult[i].nDataKind = 0;
        convResult[i].bHighGasoline = false;
        convResult[i].bLowestOilSt = false;
        convResult[i].nOPIGasolinePrice = 0;
        convResult[i].nOPIDieselPrice = 0;
        convResult[i].nOPILPGPrice = 0;
        convResult[i].nOPIHiGasolinePrice = 0;
        convResult[i].nOPIHiLPGPrice = 0;
        memset(convResult[i].szNavseq, 0x0, sizeof(convResult[i].szNavseq)-1);
    }
    return convResult;
}

void SimpleSearch::doSearchByRicode(const QSharedPointer<SearchRequest> &req, SearchReply *reply, TS_SEARCH_REQ &sSearchReq) {
    qDebug() << "[Perf][SimpleSearch][doSearchByRicode]before";

    const int requestCount = 0;
    const int startIndex = req->startIndex();
    const bool bNewAddr = req->newAddr();
    QVector<TS_SEARCH_LIST> result;

    MNS_ErrorCode eError;
    int totalCount = 0;

    int riCounter = 0;
    while (riCounter < 100) {
        sSearchReq.sSearchAddrJibun.nAddrRiCode = riCounter;
        qDebug() << "[Perf][SimpleSearch][doSearchByRicode][MNS_Search]before";
        eError = MNS_Search(&sSearchReq, true, totalCount);
        qDebug() << "[Perf][SimpleSearch][doSearchByRicode][MNS_Search]after";
        if (eError != E_ERROR_SUCCESS) {
            riCounter++;
            continue;
        }

        int resultCount = requestCount;
        if (resultCount == 0) {
            resultCount = totalCount;
        }

        if (totalCount > 0) {
            TS_SEARCH_LIST* resultTmp = new TS_SEARCH_LIST[totalCount];
            memset(resultTmp, 0, resultCount * sizeof(TS_SEARCH_LIST));
            qDebug() << "[Perf][SimpleSearch][doSearchByRicode][MNS_GetSearchResultList]before";
            eError = MNS_GetSearchResultList(sSearchReq.eSearchType, bNewAddr, startIndex, resultCount, resultTmp, &totalCount);
            qDebug() << "[Perf][SimpleSearch][doSearchByRicode][MNS_GetSearchResultList]after";
            if (eError == E_ERROR_SUCCESS) {
                for(int i = 0; i < totalCount; i++) {
                    TS_SEARCH_LIST resultunit;
                    memcpy(&resultunit, &resultTmp[i], sizeof(TS_SEARCH_LIST));
                    result.append(resultunit);
                }
            }
        }
        riCounter++;
    }
    reply->m_results.reset(conv2TmapSearchList(result.data(), result.size()));
    reply->m_resultCount = result.size();
    reply->m_totalCount = result.size();
    result.clear();
    sendFinished(reply);

    qDebug() << "[Perf][SimpleSearch][doSearchByRicode]after";
}

void SimpleSearch::doSearch(const QSharedPointer<SearchRequest> &req, SearchReply *reply, TS_SEARCH_REQ &sSearchReq) {
    qDebug() << "[Perf][SimpleSearch][doSearch]before";

    const int reSearchCount = 5;
    const int requestCount = req->count();
    const int startIndex = req->startIndex();
    const bool bNewAddr = req->newAddr();
    setPosition(sSearchReq.sSearchSisulAround.tpCurrent);

    MNS_ErrorCode eError;
    int totalCount = 0;

    qDebug() << "[Perf][SimpleSearch][doSearch][MNS_Search]before";
    eError = MNS_Search(&sSearchReq, true, totalCount);
    qDebug() << "[Perf][SimpleSearch][doSearch][MNS_Search]after";
    if (eError != E_ERROR_SUCCESS) {
        qDebug() << "MNS_Search eError:" << eError;
        reply->m_error = true;
        reply->m_errorCause = eError;
        sendFinished(reply);
        return;
    }

    qDebug() << "[SimpleSearch::doSearch] #1 totalCount: " << totalCount;

    if (totalCount > 0) {
        int resultCount = requestCount;
        if (resultCount == 0) {
            resultCount = totalCount;
        }

        TS_SEARCH_LIST* result = new TS_SEARCH_LIST[resultCount];
        if (result) {
            memset(result, 0, resultCount * sizeof(TS_SEARCH_LIST));


            for(int i=0; i<reSearchCount; i++) {
                qDebug() << "[Perf][SimpleSearch][doSearch][MNS_GetSearchResultList]before";
                eError = MNS_GetSearchResultList(sSearchReq.eSearchType, bNewAddr, startIndex, resultCount, result, &totalCount);
                qDebug() << "[Perf][SimpleSearch][doSearch][MNS_GetSearchResultList]after";
                if (eError == E_ERROR_SUCCESS) {
                    qDebug() << "MNS_GetSearchResultList resultCount:" << resultCount;

                    QElapsedTimer elapsedTimer;
                    elapsedTimer.start();

                    bool isReSearch = false;
                    for(int j=0;j<resultCount;j++) {
                        if (sSearchReq.eSearchType == E_MNS_SEARCHTYPE_SISULCATE) {
                            if (sSearchReq.sSearchSisulCate.nCateCode != result[j].nKind ) {
                                isReSearch = true;
                                break;
                            }
                        }
                        if(result[j].nPoiID == 0) {
                            isReSearch = true;
                            break;
                        }
                    }
                    if (isReSearch && i < (reSearchCount-1)) {
                        int gap = (100 - elapsedTimer.elapsed());
                        if (gap > 0){
                            usleep(gap * 1000);
                        }
                        continue;
                    }

                    reply->m_results.reset(conv2TmapSearchList(result, resultCount));
                    reply->m_resultCount = resultCount;
                    reply->m_totalCount = totalCount;
                    delete [] result;
                } else {
                    qDebug() << "MNS_GetSearchResultList eError:" << eError;
                    delete [] result;
                    reply->m_error = true;
                    reply->m_errorCause = eError;
                }
                break;
            }
        } else {
            reply->m_error = true;
            reply->m_errorCause = E_ERROR_UNKNOWN;
        }
    } else {
        // allocate the dummy data to avoid NPE
        TS_SEARCH_LIST* result = new TS_SEARCH_LIST[1];
        if (result) {
            memset(result, 0, sizeof(TS_SEARCH_LIST));
            reply->m_results.reset(conv2TmapSearchList(result, 1));
            reply->m_resultCount = 0;
            reply->m_totalCount = 0;
            delete [] result;
        }
    }

    sendFinished(reply);
#if DEBUG_GETNEXTLIST
    SKT::SearchManagerTest::sharedInstance()->requestSearch_getNextAddressList(bNewAddr);
#endif

    qDebug() << "[Perf][SimpleSearch][doSearch]after";
}

void SimpleSearch::searchBySisulName(const QSharedPointer<SearchRequest> &req, SearchReply *reply) {
    qDebug() << "[Perf][SimpleSearch][searchBySisulName]before";

    const bool bNewAddr = req->newAddr();

    TS_SEARCH_REQ sSearchReq;
    memset(&sSearchReq, 0, sizeof(TS_SEARCH_REQ));

    QString trimmedString = req->keyword();
    trimmedString = trimmedString.trimmed();
    trimmedString = trimmedString.replace(" ", "");
    trimmedString = trimmedString.simplified();
    QByteArray searchName = TextDecoder::instance()->fromUnicode(trimmedString);

    sSearchReq.eSearchType =  (req->isPrediction() == true)?E_MNS_SEARCHTYPE_PREDICTION:E_MNS_SEARCHTYPE_SISULNAME;
    TeSearchType eSearchType = sSearchReq.eSearchType;
    MNS_ErrorCode eError = E_ERROR_SUCCESS;
    if (!req->isPrediction()) {
        qDebug() << "[Perf][SimpleSearch][searchBySisulName][MNS_PowerSearchCheck]before";
        eError = MNS_PowerSearchCheck(searchName.constData(), bNewAddr, eSearchType);
        qDebug() << "[Perf][SimpleSearch][searchBySisulName][MNS_PowerSearchCheck]after";
        if ((eError != E_ERROR_SUCCESS) || (trimmedString.size() > (TD_MAXLEN_KEYWORD)) ) {
            qDebug() << "SimpleSearch::searchBySisulName : string count " << trimmedString.size();
            qDebug() << "SimpleSearch::searchBySisulName : [error code] " << eError;
            reply->m_error = true;
            reply->m_errorCause = eError;
            sendFinished(reply);
            return;
        }
    }

    sSearchReq.eSearchType = eSearchType;

    switch(eSearchType) {
    case E_MNS_SEARCHTYPE_SISULNAME:
    case E_MNS_SEARCHTYPE_PREDICTION:
        {
            TS_SEARCH_SISULNAME_REQ *sisualName = &sSearchReq.sSearchSisulName;
            sisualName->eSortType = toTeSearchSort(req->sortType());
            sisualName->tpCurrent.x = req->x();
            sisualName->tpCurrent.y = req->y();
            sisualName->eSisualNameType = E_MNS_SISUALNAMETYPE_ALL;
            sisualName->nCateCode = 0;
            sisualName->nAddrCode[0] = req->addrCode();
            sisualName->nAddrCode[1] = 0;
            sisualName->nAddrCode[2] = 0;
            sisualName->bNewAddr = req->newAddr();
            sSearchReq.eSearchType = (req->isPrediction() == true)?E_MNS_SEARCHTYPE_PREDICTION:E_MNS_SEARCHTYPE_SISULNAME;
            strncpy(sisualName->szSearchName, searchName.constData(),
                    sizeof(sisualName->szSearchName)-1);
        }
        break;
    case E_MNS_SEARCHTYPE_PHONENUMBER:
        {
            TS_SEARCH_PHONENUMBER_REQ *sPhoneNumber = &sSearchReq.sSearchPhoneNumber;
            sPhoneNumber->eSortType = toTeSearchSort(req->sortType());
            sPhoneNumber->tpCurrent.x = req->x();
            sPhoneNumber->tpCurrent.y = req->y();
            strncpy(sPhoneNumber->szPhoneNumber, searchName.constData(),
                    sizeof(sPhoneNumber->szPhoneNumber)-1);
        }
        break;
    case E_MNS_SEARCHTYPE_POWERJIBUN:
        {
            TS_SEARCH_POWERJIBUN_REQ *sPowerOldJibun = &sSearchReq.sSearchPowerJibun;
            sPowerOldJibun->eSortType = toTeSearchSort(req->sortType());
            sPowerOldJibun->tpCurrent.x = req->x();
            sPowerOldJibun->tpCurrent.y = req->y();
            strncpy(sPowerOldJibun->szAddrName, searchName.constData(),
                    sizeof(sPowerOldJibun->szAddrName)-1);
            // - s : 결과 없을 시, 구주소->신주소 두 번 검색
            int totalCount = 0;
            qDebug() << "[Perf][SimpleSearch][searchBySisulName][MNS_Search]before";
            eError = MNS_Search(&sSearchReq, true, totalCount);
            qDebug() << "[Perf][SimpleSearch][searchBySisulName][MNS_Search]after";
            if (totalCount > 0 && eError == E_ERROR_SUCCESS) {
                break;
            }
            qDebug() << "[Perf][SimpleSearch][searchBySisulName][E_MNS_SEARCHTYPE_POWERJIBUN][MNS_PowerSearchCheck]before";
            eError = MNS_PowerSearchCheck(searchName.constData(), !bNewAddr, eSearchType);
            qDebug() << "[Perf][SimpleSearch][searchBySisulName][E_MNS_SEARCHTYPE_POWERJIBUN][MNS_PowerSearchCheck]after";

            TS_SEARCH_POWERNEWJIBUN_REQ *sPowerNewJibun = &sSearchReq.sSearchPowerNewJibun;
            sPowerNewJibun->eSearchType = eSearchType;
            sPowerNewJibun->eSortType = toTeSearchSort(req->sortType());
            sPowerNewJibun->tpCurrent.x = req->x();
            sPowerNewJibun->tpCurrent.y = req->y();
            strncpy(sPowerNewJibun->szAddrName, searchName.constData(),
                    sizeof(sPowerNewJibun->szAddrName)-1);
            // - e
        }
        break;
    case E_MNS_SEARCHTYPE_POWERNEWJIBUN:
        {
            TS_SEARCH_POWERNEWJIBUN_REQ *sPowerNewJibun = &sSearchReq.sSearchPowerNewJibun;
            sPowerNewJibun->eSortType = toTeSearchSort(req->sortType());
            sPowerNewJibun->tpCurrent.x = req->x();
            sPowerNewJibun->tpCurrent.y = req->y();
            strncpy(sPowerNewJibun->szAddrName, searchName.constData(),
                    sizeof(sPowerNewJibun->szAddrName)-1);
            // - s  : 결과 없을 시, 신주소->구주소 두 번 검색
            int totalCount = 0;
            qDebug() << "[Perf][SimpleSearch][searchBySisulName][E_MNS_SEARCHTYPE_POWERNEWJIBUN][MNS_Search]before";
            eError = MNS_Search(&sSearchReq, true, totalCount);
            qDebug() << "[Perf][SimpleSearch][searchBySisulName][E_MNS_SEARCHTYPE_POWERNEWJIBUN][MNS_Search]after";
            if (totalCount > 0 && eError == E_ERROR_SUCCESS) {
                break;
            }
            qDebug() << "[Perf][SimpleSearch][searchBySisulName][E_MNS_SEARCHTYPE_POWERNEWJIBUN][MNS_PowerSearchCheck]before";
            eError = MNS_PowerSearchCheck(searchName.constData(), !bNewAddr, eSearchType);
            qDebug() << "[Perf][SimpleSearch][searchBySisulName][E_MNS_SEARCHTYPE_POWERNEWJIBUN][MNS_PowerSearchCheck]after";

            TS_SEARCH_POWERJIBUN_REQ *sPowerOldJibun = &sSearchReq.sSearchPowerJibun;
            sPowerOldJibun->eSearchType = eSearchType;
            sPowerOldJibun->eSortType = toTeSearchSort(req->sortType());
            sPowerOldJibun->tpCurrent.x = req->x();
            sPowerOldJibun->tpCurrent.y = req->y();
            strncpy(sPowerOldJibun->szAddrName, searchName.constData(),
                    sizeof(sPowerOldJibun->szAddrName)-1);
            // - e
        }
        break;
    default:
        {
            TS_SEARCH_LIST* result = new TS_SEARCH_LIST[1];
            memset(result, 0, sizeof(TS_SEARCH_LIST));
            reply->m_results.reset(conv2TmapSearchList(result, 1));
            reply->m_resultCount = 0;
            reply->m_totalCount = 0;
            sendFinished(reply);
            return;
        }
    }

    doSearch(req, reply, sSearchReq);

    qDebug() << "[Perf][SimpleSearch][searchBySisulName]after";
}

void SimpleSearch::searchBySisulAround(const QSharedPointer<SearchRequest> &req, SearchReply *reply) {

    TS_SEARCH_REQ sSearchReq;
    memset(&sSearchReq, 0, sizeof(TS_SEARCH_REQ));

    sSearchReq.eSearchType = E_MNS_SEARCHTYPE_SISULAROUND;

    TS_SEARCH_SISULAROUND_REQ *sisualAround = &sSearchReq.sSearchSisulAround;
    sisualAround->eSortType = toTeSearchSort(req->sortType());
    sisualAround->tpCurrent.x = req->x();
    sisualAround->tpCurrent.y = req->y();
    sisualAround->nIdxCate = req->categoryIdx();
    sisualAround->nDist = req->distance();

    doSearch(req, reply, sSearchReq);
}

void SimpleSearch::searchByCategory(const QSharedPointer<SearchRequest> &req, SearchReply *reply) {
    qDebug() << "[Perf][SimpleSearch][searchByCategory]before";

    TS_SEARCH_REQ sSearchReq;
    memset(&sSearchReq, 0, sizeof(TS_SEARCH_REQ));

    sSearchReq.eSearchType = E_MNS_SEARCHTYPE_SISULCATE;

    TS_SEARCH_SISULCATE_REQ *sisualCategory = &sSearchReq.sSearchSisulCate;
    sisualCategory->eSortType = toTeSearchSort(req->sortType());
    sisualCategory->tpCurrent.x = req->x();
    sisualCategory->tpCurrent.y = req->y();
    sisualCategory->nCateCode = req->categoryCode();
    sisualCategory->nAddrCode = req->addrCode();
    sisualCategory->bNewAddr = req->newAddr();

    int demo_cnt_tmp = 2;
    TS_RESULT_CATE sCateList[demo_cnt_tmp];
    int nCnt = demo_cnt_tmp;
    int nTotal = demo_cnt_tmp;
    bool bNext = false;
    qDebug() << "[Perf][SimpleSearch][searchByCategory][MNS_GetNextCateList]before";
    MNS_GetNextCateList(sisualCategory->nCateCode, 0, 0, nCnt, sCateList, &nTotal, &bNext);
    qDebug() << "[Perf][SimpleSearch][searchByCategory][MNS_GetNextCateList]after";

    doSearch(req, reply, sSearchReq);

    qDebug() << "[Perf][SimpleSearch][searchByCategory]after";
}


void SimpleSearch::searchByPOIdetailInfo(const QSharedPointer<SearchRequest> &req, SearchReplyPoiDetail *reply) {

    // allocate the dummy data to avoid NPE
    TPOI_DETAIL_INFO* result = new TPOI_DETAIL_INFO[1];
    memset(result, 0, sizeof(TPOI_DETAIL_INFO));
    reply->m_results.reset(result);
    reply->m_resultCount = 0;
    sendFinished(reply);
}

void SimpleSearch::searchByEvPOIdetailInfo(const QSharedPointer<SearchRequest> &req, SearchReplyEvPoiDetail *reply) {

    qDebug() << "[Perf][SimpleSearch][searchByEvPOIdetailInfo]before";
#if 1 //New
    MNS_ErrorCode eError = E_ERROR_UNKNOWN;
    TPOI_EvDETAIL_INFO* result = new TPOI_EvDETAIL_INFO[1];
    memset(result, 0, sizeof(TPOI_EvDETAIL_INFO));
    TS_RESULT_EVSTATIONNEW evResult;
    memset(&evResult, 0, sizeof(evResult));
    int poiId = req->PoiId()/100;//기존 전기차 poi ID에서 입구점 빠진 poi ID
    qDebug() << "[Perf][SimpleSearch][searchByEvPOIdetailInfo][MNS_GetEvStationNew]before";
    eError = MNS_GetEvStationNew(poiId, &evResult );
    qDebug() << "[Perf][SimpleSearch][searchByEvPOIdetailInfo][MNS_GetEvStationNew]after";
    if (eError == E_ERROR_SUCCESS) {
        result->nPoiID = evResult.PoiID;
        strncpy(&result->szName[0], &evResult.PoiName[0], sizeof(evResult.PoiName));
        strncpy(&result->szAddrName[0], &evResult.PoiAddr[0], sizeof(evResult.PoiAddr));
        strncpy(&result->szRoadAddr[0], &evResult.PoiRoadAddr[0], sizeof(result->szRoadAddr)-1);        
        strncpy(&result->szPhoneNumber[0], &evResult.TelNum[0], sizeof(result->szPhoneNumber)-1);
        strncpy(&result->szPoidAddr[0],   &evResult.PoidAddr[0], sizeof(evResult.PoidAddr)-1);
        strncpy(&result->szPoidRoadAddr[0], &evResult.PoiDRoadAddr[0], sizeof(evResult.PoiDRoadAddr)-1);

        result->bFreeYn = evResult.ParkingFee;
        strncpy(&result->szUseTime[0], &evResult.UseTime[0], sizeof(result->szUseTime)-1);
        result->bPayYn = evResult.ChargeFee;
        strncpy(&result->szFee[0], &evResult.ChargeCost[0], sizeof(result->szFee)-1);
        int chargerMaxCount = sizeof(result->chargerInfo) /sizeof(result->chargerInfo[0]);
        if (evResult.ChargeCnt >= chargerMaxCount ) {
           evResult.ChargeCnt = chargerMaxCount;
        }
        result->evStationCnt = evResult.ChargeCnt;
        for(int i=0; i<result->evStationCnt; i++) {
            auto bytes = QString("0%1").arg(evResult.blandID).toLocal8Bit();
            char *bid = bytes.data();
            strncpy(&result->chargerInfo[i].szBid[0], bid, sizeof(result->chargerInfo[i].szBid)-1);
            int chargeType = evResult.ChargeType[i];

            bytes = QString("0%1").arg(chargeType).toLocal8Bit();
            char *type = bytes.data();
            strncpy(&result->chargerInfo[i].szType[0], type, sizeof(result->chargerInfo[i].szType)-1);
        }
    }
    reply->m_results.reset(result);
    reply->m_resultCount = 1;
    sendFinished(reply);
#else
    MNS_ErrorCode eError = E_ERROR_UNKNOWN;
    TPOI_EvDETAIL_INFO* result = new TPOI_EvDETAIL_INFO[1];
    memset(result, 0, sizeof(TPOI_EvDETAIL_INFO));
    TS_RESULT_EVSTATION evResult;
    memset(&evResult, 0, sizeof(evResult));
    int poiId = req->PoiId();
    eError = MNS_GetEvStation(poiId, &evResult );
    if (eError == E_ERROR_SUCCESS) {
        result->nPoiID = evResult.PoiID;
        result->szNavseq[0] = '0';
        result->bSlowChage = evResult.bSlowChage;
        result->bDCDemo = evResult.bDCDemo;
        result->bAC3 = evResult.bAC3;
        result->bDCCombo = evResult.bDCCombo;
    }
    reply->m_results.reset(result);
    reply->m_resultCount = 1;
    sendFinished(reply);
#endif

    qDebug() << "[Perf][SimpleSearch][searchByEvPOIdetailInfo]after";
}

void SimpleSearch::searchPoiStationInfo(const QSharedPointer<SearchRequest> &req, SearchReplyPoiStation *reply) {

    // allocate the dummy data to avoid NPE
    TPOI_STATION_INFO* result = new TPOI_STATION_INFO[1];
    memset(result, 0, sizeof(TPOI_STATION_INFO));
    reply->m_results.reset(result);
    reply->m_resultCount = 0;
    sendFinished(reply);
}


void SimpleSearch::searchByAddress(const QSharedPointer<SearchRequest> &req, SearchReply *reply) {
    qDebug() << "[Perf][SimpleSearch][searchByAddress]before";

    const bool bNewAddr = req->newAddr();

    TS_SEARCH_REQ sSearchReq;
    memset(&sSearchReq, 0, sizeof(TS_SEARCH_REQ));

    QByteArray searchName = TextDecoder::instance()->fromUnicode(req->keyword());
    QByteArray searchJibun = TextDecoder::instance()->fromUnicode(req->jibun());
    TeSearchType eSearchType;
    MNS_ErrorCode eError;
    if( req->startAddrIndex() >0 && strlen(searchJibun.data())>0 && strlen(searchName.data())>0 ) {
        eSearchType = req->newAddr()?E_MNS_SEARCHTYPE_NEWADDR:E_MNS_SEARCHTYPE_ADDRJIBUN;
    } else {
        qDebug() << "[Perf][SimpleSearch][searchByAddress][MNS_PowerSearchCheck]before";
        eError = MNS_PowerSearchCheck(searchName.constData(), bNewAddr, eSearchType);
        qDebug() << "[Perf][SimpleSearch][searchByAddress][MNS_PowerSearchCheck]after";
        if (eError != E_ERROR_SUCCESS) {
            reply->m_error = true;
            reply->m_errorCause = eError;
            sendFinished(reply);
            qDebug() << "[Perf][SimpleSearch][searchByAddress]after";
            return;
        }
    }

    sSearchReq.eSearchType = eSearchType;
    if (bNewAddr && eSearchType == E_MNS_SEARCHTYPE_POWERNEWJIBUN) {
        TS_SEARCH_POWERNEWJIBUN_REQ *sPowerNewJibun = &sSearchReq.sSearchPowerNewJibun;
        sPowerNewJibun->eSortType = toTeSearchSort(req->sortType());
        sPowerNewJibun->tpCurrent.x = req->x();
        sPowerNewJibun->tpCurrent.y = req->y();
        strncpy(sPowerNewJibun->szAddrName, searchName.constData(),
                sizeof(sPowerNewJibun->szAddrName)-1);

        doSearch(req, reply, sSearchReq);
    } else if (!bNewAddr && eSearchType == E_MNS_SEARCHTYPE_POWERJIBUN) {
        TS_SEARCH_POWERJIBUN_REQ *sPowerOldJibun = &sSearchReq.sSearchPowerJibun;
        sPowerOldJibun->eSortType = toTeSearchSort(req->sortType());
        sPowerOldJibun->tpCurrent.x = req->x();
        sPowerOldJibun->tpCurrent.y = req->y();
        strncpy(sPowerOldJibun->szAddrName, searchName.constData(),
                sizeof(sPowerOldJibun->szAddrName)-1);

        doSearch(req, reply, sSearchReq);
    } else if (bNewAddr && eSearchType == E_MNS_SEARCHTYPE_NEWADDR) {
        TS_SEARCH_NEWADDR_REQ *sNewAddr = &sSearchReq.sSearchNewAddr;
        sNewAddr->eSortType = toTeSearchSort(req->sortType());
        sNewAddr->tpCurrent.x = req->x();
        sNewAddr->tpCurrent.y = req->y();
        sNewAddr->nAddrCode = req->addrCode();
        sNewAddr->nNewAddrStIdx = req->startAddrIndex();
        sNewAddr->nNewAddrCnt = req->startAddrCnt();
        memset(sNewAddr->szJinun, 0x0, sizeof(sNewAddr->szJinun));
        strncpy(sNewAddr->szJinun, searchJibun.constData(),
                sizeof(sNewAddr->szJinun)-1);
        memset(sNewAddr->szAddrName, 0x0, sizeof(sNewAddr->szAddrName));
        strncpy(sNewAddr->szAddrName, searchName.constData(),
                sizeof(sNewAddr->szAddrName)-1);

        doSearch(req, reply, sSearchReq);
    } else if (!bNewAddr && eSearchType == E_MNS_SEARCHTYPE_ADDRJIBUN) {
        TS_SEARCH_ADDRJIBUN_REQ *sAddrJibun = &sSearchReq.sSearchAddrJibun;
        sAddrJibun->eSortType = toTeSearchSort(req->sortType());
        sAddrJibun->tpCurrent.x = req->x();
        sAddrJibun->tpCurrent.y = req->y();
        sAddrJibun->nAddrCode = req->addrCode();
        sAddrJibun->nAddrRiCode = req->riCode();
        sAddrJibun->nJibunStIdx = req->startAddrIndex();
        sAddrJibun->nJibunCnt = req->startAddrCnt();
        sAddrJibun->cSanFlag = req->sanFlag();
        memset(sAddrJibun->szJinun, 0x0, sizeof(sAddrJibun->szJinun));
        strncpy(sAddrJibun->szJinun, searchJibun.constData(),
                sizeof(sAddrJibun->szJinun)-1);

        if(req->riCode() > 0) {
            doSearchByRicode(req, reply, sSearchReq);
        } else {
            doSearch(req, reply, sSearchReq);
        }
    } else {
        TS_SEARCH_LIST* result = new TS_SEARCH_LIST[1];
        memset(result, 0, sizeof(TS_SEARCH_LIST));
        reply->m_results.reset(conv2TmapSearchList(result, 1));
        reply->m_resultCount = 0;
        reply->m_totalCount = 0;
        sendFinished(reply);
    }

    qDebug() << "[Perf][SimpleSearch][searchByAddress]after";
}

void SimpleSearch::searchByAddressVr(const QSharedPointer<SearchRequest> &req, SearchReply *reply)
{
    qDebug() << "[SimpleSearch::searchByAddressVr] #1";

    TS_SEARCH_REQ sSearchReq;
    memset(&sSearchReq, 0, sizeof(TS_SEARCH_REQ));

    sSearchReq.eSearchType = E_MNS_SEARCHTYPE_NUANCEVR;
    sSearchReq.sSearchNuanceVR.eSortType = E_MNS_SORTTYPE_NAME;
    sSearchReq.sSearchNuanceVR.tpCurrent.x = req->x();
    sSearchReq.sSearchNuanceVR.tpCurrent.y = req->y();

    auto params = req->addressVrParams();

    QString state = params[QStringLiteral("state")].toString();
    QString place = params[QStringLiteral("place")].toString();
    QString road = params[QStringLiteral("road")].toString();
    QString houseNumber  = params[QStringLiteral("houseNumber")].toString();

    // state 명칭 변환 (예> 검색어가 "서울특별시", "서울시"로 들어올 경우 "서울"로 변환)
    QRegExp exp1("자치");
    QRegExp exp2("특별");
    QRegExp exp3("광역");
    QRegExp exp4("시");
    QRegExp exp5(".북도");
    QRegExp exp6(".남도");
    QRegExp exp7("도");
    state.replace(exp1, "");
    state.replace(exp2, "");
    state.replace(exp3, "");
    state.replace(exp4, "");
    state.replace(exp5, "북");
    state.replace(exp6, "남");
    state.replace(exp7, "");

    strcpy(sSearchReq.sSearchNuanceVR.szLocaTypeState, TextDecoder::instance()->fromUnicode(state));
    strcpy(sSearchReq.sSearchNuanceVR.szLocaTypeCity, TextDecoder::instance()->fromUnicode(place));

    bool isEupMyeonRi = false;
    auto toks = road.split(QChar(' '), QString::SkipEmptyParts);
    if (toks.length() > 1 ) {
        auto first = toks[0].trimmed();
        if ( first.length() > 0 ) {
            QChar lastChar = first[first.size() - 1];
            if (lastChar == QChar(L'읍') || lastChar == QChar(L'면')) {
                auto second = toks[1].trimmed();
                lastChar = second[second.size() - 1];
                if (lastChar == QChar(L'리')) {
                    isEupMyeonRi = true;
                    strcpy(sSearchReq.sSearchNuanceVR.szLocaTypeStreet, TextDecoder::instance()->fromUnicode(first));
                    strcpy(sSearchReq.sSearchNuanceVR.szLocaTypeCitycenter, TextDecoder::instance()->fromUnicode(second));
                }
            }
        }
    }

    if (!isEupMyeonRi) {
        strcpy(sSearchReq.sSearchNuanceVR.szLocaTypeStreet, TextDecoder::instance()->fromUnicode(road));
    }

    strcpy(sSearchReq.sSearchNuanceVR.szJibun, TextDecoder::instance()->fromUnicode(houseNumber));

    qDebug() << "[SimpleSearch::searchByAddressVr] #2";

    qDebug() << "[SimpleSearch::searchByAddressVr] eSearchType: " << sSearchReq.eSearchType;
    qDebug() << "[SimpleSearch::searchByAddressVr] sSearchNuanceVR.eSortType: " << sSearchReq.sSearchNuanceVR.eSortType;
    qDebug() << "[SimpleSearch::searchByAddressVr] sSearchNuanceVR.tpCurrent.x: " << sSearchReq.sSearchNuanceVR.tpCurrent.x;
    qDebug() << "[SimpleSearch::searchByAddressVr] sSearchNuanceVR.tpCurrent.y: " << sSearchReq.sSearchNuanceVR.tpCurrent.y;
    qDebug() << "[SimpleSearch::searchByAddressVr] sSearchNuanceVR.szLocaTypeState: " << TextDecoder::instance()->fromCP949(sSearchReq.sSearchNuanceVR.szLocaTypeState);
    qDebug() << "[SimpleSearch::searchByAddressVr] sSearchNuanceVR.szLocaTypeCity: " << TextDecoder::instance()->fromCP949(sSearchReq.sSearchNuanceVR.szLocaTypeCity);
    qDebug() << "[SimpleSearch::searchByAddressVr] sSearchNuanceVR.szLocaTypeStreet: " << TextDecoder::instance()->fromCP949(sSearchReq.sSearchNuanceVR.szLocaTypeStreet);
    qDebug() << "[SimpleSearch::searchByAddressVr] sSearchNuanceVR.szLocaTypeCitycenter: " << TextDecoder::instance()->fromCP949(sSearchReq.sSearchNuanceVR.szLocaTypeCitycenter);
    qDebug() << "[SimpleSearch::searchByAddressVr] sSearchNuanceVR.szJibun: " << TextDecoder::instance()->fromCP949(sSearchReq.sSearchNuanceVR.szJibun);

    qDebug() << "[SimpleSearch::searchByAddressVr] #3";

    doSearch(req, reply, sSearchReq);
}

void SimpleSearch::searchNextAddress(const QSharedPointer<SearchNextAddrRequest> &addrReq, SearchNextAddrReply *reply) {
    qDebug() << "[Perf][SimpleSearch][searchNextAddress]before";

    const bool bNewAddr = addrReq->newAddr();
    TeAddressType eAddressType = (bNewAddr) ? TeAddressType::E_ADDRESS_NEW : TeAddressType::E_ADDRESS_OLD;

    TS_POINT tp;
    tp.x = addrReq->x();
    tp.y = addrReq->y();
    const int nRiCode = addrReq->riCode();
    const int nIdx = addrReq->startIndex();
    const int nRequestCnt = addrReq->count();
    const int nAddressCode = addrReq->addressCode();
    bool bFilter = false;
    int nCnt = 0;
    int nTotal = 0;
    bool bNext;
    MNS_ErrorCode eError;

    TS_RESULT_ADDR pResultDepth1[20];    // 최초 사용할 메모리
    memset(pResultDepth1, 0, sizeof(pResultDepth1));
    nCnt = 20;
    qDebug() << "[Perf][SimpleSearch][searchNextAddress][MNS_GetNextAddrList]before";
    eError = MNS_GetNextAddrList(tp, eAddressType, nAddressCode, nRiCode, bFilter, nIdx, nCnt, pResultDepth1, &nTotal, &bNext);
    qDebug() << "[Perf][SimpleSearch][searchNextAddress][MNS_GetNextAddrList]after";
    if (eError != E_ERROR_SUCCESS) {
        reply->m_error = true;
        reply->m_errorCause = eError;
        sendFinished(reply);
        qDebug() << "[Perf][SimpleSearch][searchNextAddress]after";
        return;
    }

    if (nTotal > 0) {
        int resultCount = nRequestCnt;
        if (resultCount == 0) {
            resultCount = nTotal;
        }
        int nlast_totalCnt = resultCount;

        TS_RESULT_ADDR *item = new TS_RESULT_ADDR[resultCount];
        if (item) {
            memset(item, 0x0, sizeof(TS_RESULT_ADDR)*resultCount);

            qDebug() << "[Perf][SimpleSearch][searchNextAddress][MNS_GetNextAddrList]#1 before";
            eError = MNS_GetNextAddrList(tp, eAddressType, nAddressCode, nRiCode, bFilter, nIdx, resultCount, item, &nTotal, &bNext);
            qDebug() << "[Perf][SimpleSearch][searchNextAddress][MNS_GetNextAddrList]#1 after";
            if (eError == E_ERROR_SUCCESS) {
                // E_ADDRESS_OLD 인 경우, 추가탐색으로 startIdx, startCnt를 얻어야 함 by MNS
                int last_resultCount = 2;
                TS_RESULT_ADDR lastItem[last_resultCount];
                int last_nTotal = 0;
                bool last_bNext = false;
                memset(lastItem, 0x0, sizeof(TS_RESULT_ADDR)*last_resultCount);
                if(E_ADDRESS_OLD == eAddressType && &item[1] != NULL) {
                    qDebug() << "[Perf][SimpleSearch][searchNextAddress][MNS_GetNextAddrList]#2 before";
                    eError = MNS_GetNextAddrList(tp, eAddressType, item[1].nAddrCode, item[1].nAddrRiCode
                            , false, 0, last_resultCount, lastItem, &last_nTotal, &last_bNext);
                    qDebug() << "[Perf][SimpleSearch][searchNextAddress][MNS_GetNextAddrList]#2 after";
                }
                if(eError == E_ERROR_SUCCESS) {
                    if(E_ADDRESS_OLD == eAddressType) {
                        //1. 동 하위 nJibunCnt, nJibunStIdx를 추가로 획득
                        if(lastItem[1].nAddrRiCode == 0 && last_nTotal < 0 && last_bNext == false && last_resultCount == 1) {
                            for( int j = 1 ; j < nlast_totalCnt; j++) {
                                int temp_Cnt = 2;
                                qDebug() << "[Perf][SimpleSearch][searchNextAddress][MNS_GetNextAddrList]#3 before";
                                MNS_GetNextAddrList(tp, eAddressType, item[j].nAddrCode, item[j].nAddrRiCode
                                                    , false, 0, temp_Cnt, lastItem, &last_nTotal, &last_bNext);
                                qDebug() << "[Perf][SimpleSearch][searchNextAddress][MNS_GetNextAddrList]#3 after";
                                item[j].nJibunCnt = lastItem[0].nJibunCnt;
                                item[j].nJibunStIdx = lastItem[0].nJibunStIdx;
                                item[j].nAddrRiCode = lastItem[0].nAddrRiCode;
                            }
                        }
                        //2. 면 하위 nJibunCnt, nJibunStIdx를 추가로 획득 (nAddrRiCode를 이용하여, 한 단계 더 검색해야 함)
                        else if (lastItem[1].nAddrRiCode >= 1 && last_bNext == true ) {
                            for( int j = 1 ; j < nlast_totalCnt; j++) {
                                int temp_Cnt = 2;
                                // item[j].nAddrCode 이용하여, lastItem[1].nAddrRiCode를 얻어내고,
                                // item[j].nAddrCode와 lastItem[1].nAddrRiCode를 이용하여, lastItem[0].nJibunStIdx와 lastItem[0].nJibunCnt를 얻어낸다.
                                // nJibunStIdx와 nJibunCnt가 있어야, 해당 주소(리주소)에 해당하는 번지를 모두 검색할 수 있기 때문이다.
                                qDebug() << "[Perf][SimpleSearch][searchNextAddress][MNS_GetNextAddrList]#4 before";
                                MNS_GetNextAddrList(tp, eAddressType, item[j].nAddrCode, item[j].nAddrRiCode
                                                    , false, 0, temp_Cnt, lastItem, &last_nTotal, &last_bNext);
                                qDebug() << "[Perf][SimpleSearch][searchNextAddress][MNS_GetNextAddrList]#4 after";
                                MNS_GetNextAddrList(tp, eAddressType, item[j].nAddrCode, lastItem[1].nAddrRiCode
                                                    , false, 0, temp_Cnt, lastItem, &last_nTotal, &last_bNext);
                                qDebug() << "[Perf][SimpleSearch][searchNextAddress][MNS_GetNextAddrList]#5 after";
                                item[j].nJibunCnt = lastItem[0].nJibunCnt;
                                item[j].nJibunStIdx = lastItem[0].nJibunStIdx;
                                item[j].nAddrRiCode = lastItem[0].nAddrRiCode;
                            }
                        }
                    }
                }
                reply->m_results.reset(item);
                reply->m_resultCount = resultCount;
                reply->m_totalCount = nTotal;
            } else {
                reply->m_error = true;
                reply->m_errorCause = eError;
            }
        }
    } else {
        TS_RESULT_ADDR *item = new TS_RESULT_ADDR[1];
        memset(item, 0, sizeof(TS_RESULT_ADDR));
        reply->m_results.reset(item);
        reply->m_resultCount = 0;
        reply->m_totalCount = 0;
    }

    sendFinished(reply);
#if DEBUG_GETNEXTLIST
    SKT::SearchManagerTest::sharedInstance()->requestSearch_getNextAddressList(bNewAddr);
    SKT::SearchManagerTest::sharedInstance()->requestSearch_getNextCateList();
#endif

    qDebug() << "[Perf][SimpleSearch][searchNextAddress]after";
}

void SimpleSearch::searchNextCategory(const QSharedPointer<SearchNextCateRequest> &addrReq, SearchNextCateReply *reply) {
    qDebug() << "[Perf][SimpleSearch][searchNextCategory]before";

    const int nIdx = addrReq->startIndex();
    const int nRequestCnt = addrReq->count();
    const int nCateCode = addrReq->categoryCode();
    bool bFilter = false;
    int nCnt = 0;
    int nTotal = 0;
    bool bNext;
    MNS_ErrorCode eError;

    TS_RESULT_CATE pResultDepth1[20];    // 최초 사용할 메모리
    memset(pResultDepth1, 0, sizeof(pResultDepth1));
    nCnt = 20;

    qDebug() << "[Perf][SimpleSearch][searchNextCategory][MNS_GetNextCateList]#1 before";
    eError = MNS_GetNextCateList(nCateCode, bFilter, nIdx, nCnt, pResultDepth1, &nTotal, &bNext);
    qDebug() << "[Perf][SimpleSearch][searchNextCategory][MNS_GetNextCateList]#1 after";
    if (eError != E_ERROR_SUCCESS) {
        reply->m_error = true;
        reply->m_errorCause = eError;
        sendFinished(reply);
        qDebug() << "[Perf][SimpleSearch][searchNextCategory]after";
        return;
    }

    if (nTotal > 0) {
        int resultCount = nRequestCnt;
        if (resultCount == 0) {
            resultCount = nTotal;
        }

        TS_RESULT_CATE *item = new TS_RESULT_CATE[resultCount];
        if (item) {
            memset(item, 0x0, sizeof(TS_RESULT_CATE)*resultCount);
            qDebug() << "[Perf][SimpleSearch][searchNextCategory][MNS_GetNextCateList]#2 before";
            eError = MNS_GetNextCateList(nCateCode, bFilter, nIdx, resultCount, item, &nTotal, &bNext);
            qDebug() << "[Perf][SimpleSearch][searchNextCategory][MNS_GetNextCateList]#2 after";
            if (eError == E_ERROR_SUCCESS) {
                reply->m_results.reset(item);
                reply->m_resultCount = resultCount;
                reply->m_totalCount = nTotal;
            } else {
                reply->m_error = true;
                reply->m_errorCause = eError;
            }
        }
    } else {
        TS_RESULT_CATE *item = new TS_RESULT_CATE[1];
        memset(item, 0, sizeof(TS_RESULT_CATE));
        reply->m_results.reset(item);
        reply->m_resultCount = 0;
        reply->m_totalCount = 0;
    }

    sendFinished(reply);
#if DEBUG_GETNEXTLIST
    SKT::SearchManagerTest::sharedInstance()->requestSearch_getNextCateList();
#endif

    qDebug() << "[Perf][SimpleSearch][searchNextCategory]after";
}

TeSearchSort SimpleSearch::toTeSearchSort(SearchRequestBase::SortType sortType) {
    TeSearchSort t;

    switch (sortType) {
    default:
    case SearchRequestBase::SortTypeNone:
        t = E_MNS_SORTTYPE_NONE;
        break;
    case SearchRequestBase::SortTypeByName:
        t = E_MNS_SORTTYPE_NAME;
        break;
    case SearchRequestBase::SortTypeByDist:
        t = E_MNS_SORTTYPE_DIST;
        break;
    case SearchRequestBase::SortTypeByPrice:
        t = E_MNS_SORTTYPE_PRICE;
        break;
    }
    return t;
}


} // namespace SKT
