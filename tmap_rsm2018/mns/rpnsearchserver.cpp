#include "rpnsearchserver.h"

#include "environment.h"
#include "VSM_DataType.h"
#include "VSM.h"
#include "VSM_Map.h"
#include "VSM_Navi.h"
#include "VSM_Util.h"

#include "common/PoiHelper.h"
#include "stringutil.h"

#include <QDebug>
#include <QFile>
#include <QTextCodec>
#include <QDate>
#include <QtCore/qglobal.h>
#include <QElapsedTimer>

#include "hostnames.h"

extern bool NDDSLIB_SetTmapHost(char *pUrl);

namespace SKT {

void strcpyFromNavSeq(char* dest, int navSeq) {
    QString src = QString::number(navSeq);
    StringUtil::strlcpy(dest, src.toStdString().c_str(), 6);
}

DeOilType changeToNddsOilType(int oilType, int chargeType) {
    Q_ASSERT(0<=oilType && oilType<5);

    switch (oilType) {
    case 0: //Gasoline
        return DE_OILTYPE_GASOLINE;
    case 1: //Diesel
        return DE_OILTYPE_DIESEL;
    case 2: //LPG
        return DE_OILTYPE_LPG;
    case 3: // Elec
        switch (chargeType) {
        case 1: return DE_OILTYPE_AC3;
        case 2: return DE_OILTYPE_CHAdeMO;
        case 3: return DE_OILTYPE_DCCOMBO;
        }
    case 4: //Premium gasoline
        return DE_OILTYPE_PREMIUMGASOLINE;
    default:
        return DE_OILTYPE_GASOLINE;
    }
}

#define COPY_TS2DS(ds,ts) {\
    DS_ROUTE_POS &DS = ds; \
    TS_ROUTE_POS &TS = ts; \
    StringUtil::strlcpy(DS.szName, TS.szName, sizeof(DS.szName));\
    DS.tp.x = TS.tp.x;\
    DS.tp.y = TS.tp.y;\
    DS.nPoiID = PoiHelper::toServer(TS.nPoiID);\
    DS.nAddrCode = TS.nAddrCode;\
    DS.nMppCnt = TS.nMppCnt;\
    DS.byRPFlag = TS.byRPFlag; \
    strcpyFromNavSeq(DS.szNavSeq, PoiHelper::navSeq(TS.nPoiID));\
    for(int c = 0 ; c < TD_MPP_CNT ; c++){\
        DS.tpMpp[c].x = TS.tpMpp[c].x;\
        DS.tpMpp[c].y = TS.tpMpp[c].y;\
    }\
}
void COPY_DS2TS(const DS_ROUTE_POS& ds, TS_ROUTE_POS& ts) {
    StringUtil::strlcpy(ts.szName, ds.szName, sizeof(ts.szName));
    ts.tp.x = ds.tp.x;
    ts.tp.y = ds.tp.y;
    auto navSeq = PoiHelper::navSeqFromServer(ds.szNavSeq).toInt();
    ts.nPoiID = PoiHelper::fromServer(ds.nPoiID, navSeq);
    ts.nAddrCode = ds.nAddrCode;
    ts.nMppCnt = ds.nMppCnt;
    ts.byRPFlag = ds.byRPFlag;
    for(int c = 0 ; c < TD_MPP_CNT ; c++){
        ts.tpMpp[c].x = ds.tpMpp[c].x;
        ts.tpMpp[c].y = ds.tpMpp[c].y;
    }
}
void COPY_DS2TS_TIME(const DS_DATETIME& ds, TS_DATETIME& ts) {
    ts.nYear = ds.nYear;
    ts.nMonth = ds.nMonth;
    ts.nDay = ds.nDay;
    ts.nHour = ds.nHour;
    ts.nMinute = ds.nMinute;
    ts.nSecond = ds.nSecond;
    ts.nWeek = ds.nWeek;
}
TeRouteType DS2TS_ROUTETYPE(DeRouteType type) {
    switch (type) {
    case DE_ROUTETYPE_NORMAL:
    case DE_ROUTETYPE_ADD:
    case DE_ROUTETYPE_RE_OPTION:
    case DE_ROUTETYPE_RE_OUTROUTE:
    case DE_ROUTETYPE_TPEG_RE_ROUTE:
    case DE_ROUTETYPE_PATTERN_ARRIVAL:
    case DE_ROUTETYPE_PATTERN_DEPARTURE:
        return static_cast<TeRouteType>(type);

    case DE_ROUTETYPE_SUMMARY:
        qDebug() << "DS2TS_ROUTETYPE(unsupported type)";
        return E_ROUTETYPE_NORMAL;
    }
}
TeMnsOilType DS2TS_OILTYPE(DeOilType type) {
    switch (type) {
    case DE_OILTYPE_GASOLINE:
    case DE_OILTYPE_DIESEL:
    case DE_OILTYPE_LPG:
        return static_cast<TeMnsOilType>(type);

    case DE_OILTYPE_CHAdeMO:
    case DE_OILTYPE_AC3:
    case DE_OILTYPE_DCCOMBO:
    case DE_OILTYPE_PREMIUMGASOLINE:
        qDebug() << "DS2TS_OILTYPE(unsupported type)";
        return E_OILTYPE_GASOLINE;
    }
}

const int RPnSearchServer::cameraTypes = 0xff;
const int RPnSearchServer::dangerAreaOptions = 0xff;

RPnSearchServer::RPnSearchServer()
{
}

void RPnSearchServer::requestRouteSummary(RPRequestPtr& req){
    setVia(RPnSearch::RpRouteDeparture,req->getVia(1));
    setVia(RPnSearch::RpRouteDestination,req->getVia(5));

    memset(&req1,0,sizeof(DS_ROUTE_REQ));

    checkRouteRequestDeparture();
    QStringListPtr tvasFileName = req->getTvasFileName();
    if(tvasFileName.isNull() == true){ // will delete
        QStringList *files = new QStringList();
        files->append("summary.db");
        tvasFileName = QStringListPtr(files);
    }
    makeRouteResponse_Data(tvasFileName);
    COPY_TS2DS(req1.sRouteServerSummaryReq.sPosInfo[0],routePosition[RpRouteDeparture]);
    COPY_TS2DS(req1.sRouteServerSummaryReq.sPosInfo[1],routePosition[RpRouteDestination]);

    if((*req->getReq())[RPREQ_KEY_State] == RPRequest::RequestState_Valid){
        // 출발지가 사용자에 의해 변경된 경우.
        if (req->getDepartureReplaced()) {
            // 출발지가 자동설정일 때 주행각도 사용자 설정일 때에는 -1을 전달.
            req1.sRouteServerSummaryReq.nAngle = -1;

            // 출발지점 주행방향 설정
            // 자동설정일 때 1
            // 사용자설정일 때 0
            req1.sRouteServerSummaryReq.byDepartDirPriority = 0;
        }

        makeRouteRequest_AngleAndSpeed(req);

        // 출발지 탐색 코드
        auto departSearchFlag = req->getStartSearchCode();
        req1.sRouteServerSummaryReq.byDepartSrchFlag = departSearchFlag;

        // 목적지 탐색 코드
        auto destSearchFlag = req->getDestSearchCode();
        req1.sRouteServerSummaryReq.byDestSrchFlag = destSearchFlag;

        req1.sRouteServerSummaryReq.eRouteSummaryType = DE_ROUTESUMMARYTYPE_OPTIMUM;
        req1.sRouteServerSummaryReq.eRouteServiceType = DE_ROUTESERVICETYPE_REALTIME_DUPLEX;
        req1.sRouteServerSummaryReq.nPosCnt = 2;
        req1.sRouteServerSummaryReq.nVertexFlag = 1;
        req1.eRouteType = DE_ROUTETYPE_SUMMARY;

        int nSessionID=0;

        int resCount = 0;
#if defined(APPLY_DUAL_CHANNEL)
        DeErrorCode err = requestRouteUsingDualChannel(req, nSessionID, resCount);
#else
        DeErrorCode err = NDDS_Route(&req1, true, &nSessionID, &resCount);
#endif
        if (resCount == 1) {
            (*req->getRes())[RPREQ_KEY_ServerUrgentMode] = true;
        }
        qDebug() << "DeErrorCode err " << err;
        (*req->getRes())[RPREQ_KEY_ErrorCode] = err;

        if(err == E_ERROR_ROUTE_USERCANCEL) {
            (*req->getRes())[RPREQ_KEY_Answer] = RPREQ_Error_Cancel;
            (*req->getRes())[RPREQ_KEY_AnswerCode] = RPRequest::ResponseAnswer_Cancel;
            (*req->getRes())[RPREQ_KEY_ErrorCode] = err;
        } else if (err == E_ERROR_SUCCESS) {
            DS_ROUTESUMMARYINFO_RES res;
            err = NDDS_GetRouteSummaryInfo(nSessionID, &res);
            auto totalTime = 0;
            if (err == E_ERROR_SUCCESS) {
                totalTime = res.nTotalTime;
            }
            RouteSummaryInfo *info = new RouteSummaryInfo();
            QRouteSummaryInfo infoP = QRouteSummaryInfo(info);
            info->nTotalDist = 0;
            info->nTotalTime = totalTime;
            qDebug() << "NDDS_GetRouteSummaryInfo : " << totalTime;
            (*req->getRes())[RPREQ_KEY_RouteSummaryInfo] = QVariant::fromValue<QRouteSummaryInfo>(infoP);
        }
    }else {
        (*req->getRes())[RPREQ_KEY_Answer] = RPREQ_Error_Cancel;
        (*req->getRes())[RPREQ_KEY_AnswerCode] = RPRequest::ResponseAnswer_Cancel;
        (*req->getRes())[RPREQ_KEY_ErrorCode] = E_ERROR_UNKNOWN;
    }
}

void RPnSearchServer::requestRoute(RPRequestPtr& req){
    qDebug() << "[Perf][RPnSearchServer][requestRoute]before";
    setVia(RPnSearch::RpRouteDeparture,req->getVia(1));
    setVia(RPnSearch::RpRouteVia1,req->getVia(2));
    setVia(RPnSearch::RpRouteVia2,req->getVia(3));
    setVia(RPnSearch::RpRouteVia3,req->getVia(4));
    setVia(RPnSearch::RpRouteDestination,req->getVia(5));

    QRouteOptionListPtr rpOptions = req->getRPOptions();
    QStringListPtr tvasFileName = req->getTvasFileName();

    if(rpOptions.isNull() == true){ // will delete
        QList<TeRouteOption> *opts = new QList<TeRouteOption>();
        opts->append(E_ROUTEOPTION_OPTIMUM);
        opts->append(E_ROUTEOPTION_FREEWAY);
        opts->append(E_ROUTEOPTION_SHORTESTWAY);
        rpOptions = QRouteOptionListPtr(opts);
    }
    if(tvasFileName.isNull() == true){ // will delete
        QStringList *files = new QStringList();
        files->append("tvas_route1.db");
        files->append("tvas_route2.db");
        files->append("tvas_route3.db");
        tvasFileName = QStringListPtr(files);
    }

    requestClearData();
    makeRouteRequest_DefaultData(req);
    makeRouteResponse_Data(tvasFileName);
    makeRouteRequest_Option(rpOptions);
    makeRouteRequest_GpsTraceData(req);

    if((*req->getReq())[RPREQ_KEY_State] == RPRequest::RequestState_Valid){
        // set tvas version it is only used to be for server, 2018.4.12
        QString tvasVer = (*req->getReq())[RPREQ_KEY_TvasVersion].toString();
        if (!tvasVer.isEmpty())
            qstrncpy(req1.sRouteServerReq.szTvas,tvasVer.toLatin1().constData(),6);

        // 출발지 탐색 코드
        auto departSearchFlag = req->getStartSearchCode();
        req1.sRouteServerReq.byDepartSrchFlag = departSearchFlag;

        // 목적지 탐색 코드
        auto destSearchFlag = req->getDestSearchCode();
        req1.sRouteServerReq.byDestSrchFlag = destSearchFlag;

        // 지난 위치 정보
        auto lastGoalPos = req->getLastGoalPos();
        if (lastGoalPos.isNull() == false) {
            req1.sRouteServerReq.sPassPosInfo.nPastXpos = lastGoalPos.x();
            req1.sRouteServerReq.sPassPosInfo.nPastYpos = lastGoalPos.y();

            auto lastGoalSessionId = req->getLastGoalSessionId();
            strncpy(req1.sRouteServerReq.sPassPosInfo.szPastSessionId, lastGoalSessionId.toStdString().c_str(), 25);

            auto lastGoalTime = req->getLastGoalTime();
            strncpy(req1.sRouteServerReq.sPassPosInfo.szPastArrivalTime, lastGoalTime.toStdString().c_str(), 15);
        }

        auto m = *(req->getReq());

        auto isReroute = false;
        makeRouteRequest_RerouteDataUsingMap(m, isReroute);

        if (m.contains(RPREQ_KEY_RD4_Filename)) {
            auto filename = m[RPREQ_KEY_RD4_Filename].toString();
            QFile file(filename);
            if (file.open(QIODevice::ReadOnly)) {
                auto size = file.size();
                auto ba = file.read(size);
                file.close();

                mRD4 = ba.toBase64();
                size = mRD4.size();

                // 아직 NDDS로 해당 정보 전달하지 않음
                // 버퍼공간 보다 큰 데이터를 전달
                // 19.07.03 ymhong.
                req1.sRouteServerReq.sReRouteInfo.nRouteDataSize = size;
                req1.sRouteServerReq.sReRouteInfo.pszRouteData = mRD4.data();
            }
        }

        int nSessionID=0; //sample

        QElapsedTimer elapsedTimer;
        elapsedTimer.start();

        int resCount = 0;
#if defined(APPLY_DUAL_CHANNEL)
        DeErrorCode err = requestRouteUsingDualChannel(req, nSessionID, resCount);
#else
        DeErrorCode err = NDDS_Route(&req1, true, &nSessionID, &resCount);
#endif
        if (resCount == 1) {
            (*req->getRes())[RPREQ_KEY_ServerUrgentMode] = true;
        }
        qDebug() << "NDDS_Route() : " << elapsedTimer.elapsed() << "ms";
        qDebug() << "DeErrorCode err " << err;
        (*req->getRes())[RPREQ_KEY_ErrorCode] = err;
        (*req->getRes())["elapsed"] = elapsedTimer.elapsed();

        if(err == DE_ERROR_ROUTE_USERCANCEL
                || (*req->getReq())[RPREQ_KEY_State] != RPRequest::RequestState_Valid){
            qDebug() << "RPnSearchServer::requestRoute RequestState_Valid Not " << err;
            (*req->getRes())[RPREQ_KEY_Answer] = RPREQ_Error_Cancel;
            (*req->getRes())[RPREQ_KEY_AnswerCode] = RPRequest::ResponseAnswer_Cancel;
            (*req->getRes())[RPREQ_KEY_ErrorCode] = err;
        }else {
            if (err == E_ERROR_SUCCESS) {
                DS_WEATHERINFO_RES * weatherInfo = new DS_WEATHERINFO_RES();
                QWeatherInfo weatherInfoP = QWeatherInfo(weatherInfo);
                qDebug() << "[Perf][RPnSearchServer][requestRoute][NDDS_GetWeatherInfo]before";
                err = NDDS_GetWeatherInfo(weatherInfo);
                qDebug() << "[Perf][RPnSearchServer][requestRoute][NDDS_GetWeatherInfo]after";
                (*req->getRes())[RPREQ_KEY_WeatherInfo] = QVariant::fromValue<QWeatherInfo>(weatherInfoP);
                (*req->getRes())[RPREQ_KEY_ErrorCode] = err;
                (*req->getRes())[RPREQ_KEY_SessionId] = sessionId();// mSessionId;
            } else {
                // 서버로 경로 탐색하여 에러가 발생한 경우
                // 거리 초과에 대한 에러 코드를 별도로 내려주지 않아
                // MNS api를 이용하여 거리를 초과 하는지 체크한다.
                // 18.11.08 ymhong

                // 서버에서 경로 요청 시 DS_ROUTE_ROUTE 구조체를 사용하므로
                // TS_ROUTE_ROUTE 구조체에 맞게 정보를 채운다.
                // 구조체에 없는 데이터는 주석 처리.
                TS_ROUTE_REQ ts;
                memset(&ts, 0, sizeof(TS_ROUTE_REQ));

                for (int i=0; i<TD_POS_CNT; i++) {
                    COPY_DS2TS(req1.sRouteServerReq.sPosInfo[i], ts.sPosInfo[i]);
                }
                qstrncpy(ts.szTvas, req1.sRouteServerReq.szTvas, 6);
                ts.eRouteType = DS2TS_ROUTETYPE(req1.eRouteType);
                ts.bHipass = req1.sRouteServerReq.bHipass;
                //ts.bNVXInfoCollect = false;
                //ts.bSDICollect = false;
                ts.eOilType = DS2TS_OILTYPE(req1.sRouteServerReq.eOilType);
                ts.eVehicleClass = static_cast<TeVehicleClass>(req1.sRouteServerReq.eVehicleClass);
                //ts.nCurrentAngle = 0;
                ts.nCurrentSpeed = req1.sRouteServerReq.nSpeed;
                //ts.nLocalRPSearchCountFactor = 0;
                ts.nPosCnt = req1.sRouteServerReq.nPosCnt;
                ts.nRouteOption = req1.sRouteServerReq.nRouteOption;
                //ts.nRouteTrafficType = 0;
                COPY_DS2TS_TIME(req1.sRouteServerReq.sDataTime, ts.sDataTime);
                //ts.szSDIDataFilePath = 0;
                //ts.szSDIIndexFilePath = 0;
                //ts.uPassTollID = 0;

                qDebug() << "[Perf][RPnSearchServer][requestRoute][MNS_IsValidePositionByRoutting]before";
                auto result = MNS_IsValidePositionByRoutting(&ts);
                qDebug() << "[Perf][RPnSearchServer][requestRoute][MNS_IsValidePositionByRoutting]after";
                if (result != E_ERROR_SUCCESS) {
                    (*req->getRes())[RPREQ_KEY_ErrorCode] = E_ERROR_ROUTE_DISTOVER;
                }
            }
        }
    }else {
        qDebug() << "RPnSearchServer::requestRoute RequestState_Invalid_Cancel ";
        (*req->getRes())[RPREQ_KEY_Answer] = RPREQ_Error_Cancel;
        (*req->getRes())[RPREQ_KEY_AnswerCode] = RPRequest::ResponseAnswer_Cancel;
        (*req->getRes())[RPREQ_KEY_ErrorCode] = E_ERROR_UNKNOWN;
    }

    qDebug() << "[Perf][RPnSearchServer][requestRoute]after";
}

static void printRerouteMap(const QString &description, const QVariantMap &m)
{
    qDebug() << "printMap(" << description << ")";
    for (auto key : m.keys()) {
        qDebug() << "m[" << key << "] = " << m[key];
    }
    qDebug() << "-----------------------------------";
}

void RPnSearchServer::requestReroute(RPRequestPtr& req)
{
    qDebug() << "[Perf][RPnSearchServer][requestReroute]before";

    // TMCRSM-197. 온라인에서 재탐색 시 경유지 목적지 바뀌는 문제 수정.
    // 18.03.21 ymhong.
    if(req->getVia(1).isNull() == true)
        changeDeparture_currentPosition();
    else
        setVia(RPnSearch::RpRouteDeparture,req->getVia(1));

    setVia(RPnSearch::RpRouteVia1,req->getVia(2));
    setVia(RPnSearch::RpRouteVia2,req->getVia(3));
    setVia(RPnSearch::RpRouteVia3,req->getVia(4));
    setVia(RPnSearch::RpRouteDestination,req->getVia(5));

    QRouteOptionListPtr rpOptions = req->getRPOptions();
    if(rpOptions.isNull() == true){
        QList<TeRouteOption> *opts = new QList<TeRouteOption>();
        opts->append(E_ROUTEOPTION_OPTIMUM);
        rpOptions = QRouteOptionListPtr(opts);
    }

    QStringListPtr tvasFileName = req->getTvasFileName();
    if(tvasFileName.isNull() == true){
        QStringList *files = new QStringList();
        files->append("tvas_route.db");
        tvasFileName = QStringListPtr(files);
    }

    requestClearData();
    makeRouteRequest_DefaultData(req, true);
    makeRouteResponse_Data(tvasFileName);
    makeRouteRequest_Option(rpOptions);
    makeRouteRequest_GpsTraceData(req);

    auto m = *req->getReq();

#if (1)
    // TMCRSM-3349. 비통신->통신 전환 시 재탐색 하면 간헐적으로 Crash.
    // 재탐색 시 NDDS로 전달 되는 재탐색 관련 파라메타들을 살펴보기 위한 디버깅용 로그
    // 19.07.03 ymhong.
    printRerouteMap("[RPSERVER]", m);
#endif

    auto isReroute = true;
    makeRouteRequest_RerouteDataUsingMap(m, isReroute);

    if (m.contains(RPREQ_KEY_RD4_Filename)) {
        auto filename = m[RPREQ_KEY_RD4_Filename].toString();
        QFile file(filename);
        if (file.open(QIODevice::ReadOnly)) {
            auto size = file.size();
            auto ba = file.read(size);
            file.close();

            mRD4 = ba.toBase64();
            size = mRD4.size();

            // 아직 NDDS로 해당 정보 전달하지 않음
            // 버퍼공간 보다 큰 데이터를 전달
            // 19.07.03 ymhong.
            req1.sRouteServerReq.sReRouteInfo.nRouteDataSize = size;
            req1.sRouteServerReq.sReRouteInfo.pszRouteData = mRD4.data();
        }
    }

    // 강제 재탐색 관련 정보
    if (m.contains(RPREQ_KEY_CongestRid)) {
        req1.sRouteServerReq.sReRouteInfo.nTcRid = m[RPREQ_KEY_CongestRid].toInt();
    }

    // 목적지 탐색 코드
    auto destSearchFlag = req->getDestSearchCode();
    req1.sRouteServerReq.byDestSrchFlag = destSearchFlag;

    req1.eRouteType = DE_ROUTETYPE_RE_OUTROUTE;

    auto command = m[RPREQ_KEY_Command].toString();
    if (command == "ChangeOption") {
        req1.eRouteType = DE_ROUTETYPE_RE_OPTION;
    }

//#define CHECK_REROUTE_BUFFERSIZE

#ifdef CHECK_REROUTE_BUFFERSIZE
#define BYTEALIGN_SIZE(size)	(((size)%4 == 0)?(size):((size)+4-(size)%4))

    qDebug() << "[NDDS] calcuated reroute buffer size : start !";

    auto bufferSize = 0;
    bufferSize += req1.sRouteServerReq.sGpsTraceData.nGpsTraceDataSize;
    bufferSize += 14; // date
    bufferSize = BYTEALIGN_SIZE(bufferSize+1);
    bufferSize += sizeof(int) * 3; // maybe 3
    bufferSize = BYTEALIGN_SIZE(bufferSize);
    bufferSize += 3; // "5.4"
    bufferSize = BYTEALIGN_SIZE(bufferSize);
    bufferSize += strlen(req1.sRouteServerReq.sPosInfo[0].szName);
    bufferSize = BYTEALIGN_SIZE(bufferSize+1);
    bufferSize += strlen(req1.sRouteServerReq.sPosInfo[1].szName);
    bufferSize = BYTEALIGN_SIZE(bufferSize+1);
    auto poiId = QString::number(req1.sRouteServerReq.sPosInfo[1].nPoiID);
    bufferSize += poiId.length();
    bufferSize = BYTEALIGN_SIZE(bufferSize+1);
    auto navSeqLen = strlen(req1.sRouteServerReq.sPosInfo[1].szNavSeq);
    bufferSize += navSeqLen;
    bufferSize = BYTEALIGN_SIZE(bufferSize+1);
    auto waypoints = 0;
    if (req1.sRouteServerReq.sPosInfo[2].tp.x != 0 && req1.sRouteServerReq.sPosInfo[2].tp.y != 0) {
        waypoints++;
    }
    if (req1.sRouteServerReq.sPosInfo[3].tp.x != 0 && req1.sRouteServerReq.sPosInfo[3].tp.y != 0) {
        waypoints++;
    }
    if (req1.sRouteServerReq.sPosInfo[4].tp.x != 0 && req1.sRouteServerReq.sPosInfo[4].tp.y != 0) {
        waypoints++;
    }

    typedef struct tagNddsTmapWayPointsDto{
        int x;					// 경유지 X 좌표
        int y;					// 경유지 Y 좌표
        char *poiID;			// 경유지 POI ID
        unsigned char rpFlag;			// 경유지 RP Flag
    }NDDSTMAP_WAYPOINTS_DTO;

    bufferSize += sizeof(NDDSTMAP_WAYPOINTS_DTO) * waypoints;
    bufferSize = BYTEALIGN_SIZE(bufferSize);
    for (int i=2; i<5; i++) {
        if (req1.sRouteServerReq.sPosInfo[i].tp.x != 0 && req1.sRouteServerReq.sPosInfo[i].tp.y != 0) {
            poiId = QString::number(req1.sRouteServerReq.sPosInfo[i].nPoiID);
            bufferSize += poiId.length();
            bufferSize = BYTEALIGN_SIZE(bufferSize+1);
        }
    }
    bufferSize += 8; // preMapVersion
    bufferSize = BYTEALIGN_SIZE(bufferSize+1);
    bufferSize += strlen(req1.sRouteServerReq.sReRouteInfo.szPreTollgateIds);
    bufferSize = BYTEALIGN_SIZE(bufferSize+1);
    bufferSize += strlen(req1.sRouteServerReq.sReRouteInfo.szInitSrchSessionId);
    bufferSize = BYTEALIGN_SIZE(bufferSize+1);
    bufferSize += strlen(req1.sRouteServerReq.sPassPosInfo.szPastSessionId);
    bufferSize = BYTEALIGN_SIZE(bufferSize+1);
    bufferSize += strlen(req1.sRouteServerReq.sPassPosInfo.szPastArrivalTime);
    bufferSize = BYTEALIGN_SIZE(bufferSize+1);
    bufferSize += 6; // HHMMSS
    bufferSize = BYTEALIGN_SIZE(bufferSize);

    auto bufferOk = bufferSize < 50000;

    qDebug() << "[NDDS] calcuated reroute buffer size : " << bufferSize << " = " << (bufferOk ? "OK" : "NG");

    (*req->getRes())["bufferSize"] = bufferSize;
#endif

    int nSessionID=0;
    // set tvas version it is only used to be for server, 2018.4.12
    QString tvasVer = (*req->getReq())[RPREQ_KEY_TvasVersion].toString();
    if (!tvasVer.isEmpty())
        qstrncpy(req1.sRouteServerReq.szTvas,tvasVer.toLatin1().constData(),6);

    QElapsedTimer elapsedTimer;
    elapsedTimer.start();
    
#if defined(APPLY_DUAL_CHANNEL)
    int resCount = 0;
    DeErrorCode err = requestRouteUsingDualChannel(req, nSessionID, resCount);
#else
    qDebug() << "[Perf][RPnSearchServer][requestReroute][NDDS_Route]before";
    DeErrorCode err = NDDS_Route(&req1, true, &nSessionID);
    qDebug() << "[Perf][RPnSearchServer][requestReroute][NDDS_Route]after";
#endif
    qDebug() << "NDDS_Route(), Reroute : " << elapsedTimer.elapsed() << "ms";
    qDebug() << "DeErrorCode err " << err;
    (*req->getRes())[RPREQ_KEY_ErrorCode] = err;
    (*req->getRes())["elapsed"] = elapsedTimer.elapsed();
    if (err == E_ERROR_SUCCESS) {
        (*req->getRes())[RPREQ_KEY_SessionId] = sessionId();// mSessionId;
    }

    qDebug() << "[Perf][RPnSearchServer][requestReroute]after";
}

void RPnSearchServer::cancelRoute(RPRequestPtr& req){
    qDebug() << "[Perf][RPnSearchServer][cancelRoute][NDDS_StopRoute]before";
    DeErrorCode err = NDDS_StopRoute();
    qDebug() << "[Perf][RPnSearchServer][cancelRoute][NDDS_StopRoute]after";
    qDebug() << "RPnSearchServer::cancelRoute DeErrorCode err " << err;
    (*req->getRes())[RPREQ_KEY_ErrorCode] = err;
}

void RPnSearchServer::makeRouteRequest_DefaultData(){
    positionCount = 0;

    req1.eRouteType = DE_ROUTETYPE_NORMAL;
    req1.sRouteServerReq.nRouteOption = DE_ROUTEOPTION_OPTIMUM;
    req1.sRouteServerReq.bStartGpsPos = true;

    // sDataTime
    QDate date = QDate::currentDate();
    QTime time = QTime::currentTime();
    req1.sRouteServerReq.sDataTime.nYear =  date.year();
    req1.sRouteServerReq.sDataTime.nMonth = date.month();
    req1.sRouteServerReq.sDataTime.nWeek = date.weekNumber();
    req1.sRouteServerReq.sDataTime.nDay = date.day();
    req1.sRouteServerReq.sDataTime.nHour = time.hour();
    req1.sRouteServerReq.sDataTime.nMinute = time.minute();
    req1.sRouteServerReq.sDataTime.nSecond = time.second();

    req1.sRouteServerReq.eVehicleClass = DE_VEHICLECLASS_1;
    req1.sRouteServerReq.eOilType = DE_OILTYPE_GASOLINE;
    makeRouteRequest_Departure();
    makeRouteRequest_Via1();
    makeRouteRequest_Via2();
    makeRouteRequest_Via3();
    makeRouteRequest_Destination();
    req1.sRouteServerReq.nPosCnt = positionCount+1;

    req1.sRouteServerReq.sGpsTraceData.nGpsTraceDataSize = 0;
    req1.sRouteServerReq.sGpsTraceData.pGpsTraceData = 0;

    // bpAddCameraType
    int type = RPnSearchServer::cameraTypes;
    if(type & 0xffffffff == 0){
        req1.sRouteServerReq.bpAddCameraType[0] = true;
    } else{
        for(int c = 1 ; c < D_ADD_CAMERATYPE ; c++){
            if(type & 0x01 == 1) req1.sRouteServerReq.bpAddCameraType[c] = true;
            else req1.sRouteServerReq.bpAddCameraType[c] = false;
            type = type>>1;
        }
    }

    //bpAddDAType
    type = RPnSearchServer::dangerAreaOptions;
    if(type & 0xffffffff == 0){
        req1.sRouteServerReq.bpAddDAType[0] = true;
    } else{
        for(int c = 1 ; c < D_ADD_CAMERATYPE ; c++){
            if(type & 0x01 == 1) req1.sRouteServerReq.bpAddDAType[c] = true;
            else req1.sRouteServerReq.bpAddDAType[c] = false;
            type = type>>1;
        }
    }

    req1.sRouteServerReq.sReRouteInfo.nRouteDataSize = 0;
    req1.sRouteServerReq.sReRouteInfo.pszRouteData = nullptr;
    //
    strcpy(req1.sRouteServerReq.szGuideImageCode, "R480x540");
}

static DeRouteOption changeOptionTeToDe(TeRouteOption o)
{
    switch (o) {
    case E_ROUTEOPTION_OPTIMUM:
        return DE_ROUTEOPTION_OPTIMUM;
    case E_ROUTEOPTION_EXPRESSWAY:
        return DE_ROUTEOPTION_EXPRESSWAY;
    case E_ROUTEOPTION_EASY:
        return DE_ROUTEOPTION_EASY;
    case E_ROUTEOPTION_FREEWAY:
        return DE_ROUTEOPTION_FREEWAY;
    case E_ROUTEOPTION_SHORTESTWAY:
        return DE_ROUTEOPTION_SHORTESTWAY;
    case E_ROUTEOPTION_SHORTTIME:
        return DE_ROUTEOPTION_SHORTTIME;
    }
}
void RPnSearchServer::makeRouteRequest_Option(QRouteOptionListPtr options){
#if (0)
    for(int c = 0 ; c < options->length() ; c++){
//        req1.sRouteServerReq.nRouteOption |= options->at(c);
        req1.sRouteServerReq.nRouteOption |= DE_ROUTEOPTION_OPTIMUM;
        if(options->at(c) == E_ROUTEOPTION_EXPRESSWAY)
            req1.sRouteServerReq.nRouteOption |= DE_ROUTEOPTION_EXPRESSWAY;
        if(options->at(c) == E_ROUTEOPTION_FREEWAY)
            req1.sRouteServerReq.nRouteOption |= DE_ROUTEOPTION_FREEWAY;
        if(options->at(c) == E_ROUTEOPTION_SHORTESTWAY)
            req1.sRouteServerReq.nRouteOption |= DE_ROUTEOPTION_SHORTESTWAY;
        if(options->at(c) == E_ROUTEOPTION_EASY)
            req1.sRouteServerReq.nRouteOption |= DE_ROUTEOPTION_EASY;
        if(options->at(c) == E_ROUTEOPTION_SHORTTIME)
            req1.sRouteServerReq.nRouteOption |= DE_ROUTEOPTION_SHORTTIME;
    }
#else
    if (options->count()) {
        req1.sRouteServerReq.nRouteOption = changeOptionTeToDe(options->at(0));
        for (auto c = 1; c < options->length(); c++) {
            auto option = options->at(c);
            req1.sRouteServerReq.nRouteOption |= option;
        }
    }
#endif
}

void RPnSearchServer::makeRouteRequest_Departure(){
    COPY_TS2DS(req1.sRouteServerReq.sPosInfo[0],routePosition[RpRouteDeparture]);
}

void RPnSearchServer::makeRouteRequest_Via1(){
    if(MakeRouteRequest_CheckPosition(routePosition[RpRouteVia1])){
        ++positionCount;
        COPY_TS2DS(req1.sRouteServerReq.sPosInfo[positionCount],routePosition[RpRouteVia1]);
    }
}

void RPnSearchServer::makeRouteRequest_Via2(){
    if(MakeRouteRequest_CheckPosition(routePosition[RpRouteVia2])){
        ++positionCount;
        COPY_TS2DS(req1.sRouteServerReq.sPosInfo[positionCount],routePosition[RpRouteVia2]);
    }
}

void RPnSearchServer::makeRouteRequest_Via3(){
    if(MakeRouteRequest_CheckPosition(routePosition[RpRouteVia3])){
        ++positionCount;
        COPY_TS2DS(req1.sRouteServerReq.sPosInfo[positionCount],routePosition[RpRouteVia3]);
    }
}

void RPnSearchServer::makeRouteRequest_Destination(){
    if(MakeRouteRequest_CheckPosition(routePosition[RpRouteDestination])){
        ++positionCount;
        COPY_TS2DS(req1.sRouteServerReq.sPosInfo[positionCount],routePosition[RpRouteDestination]);
    }
}

void RPnSearchServer::makeRouteRequest_RerouteData()
{
    /*
    int		nLastRid;				//마지막 매칭 RID
    int		nTcRid;					//이전경로 정체 RID(TVAS5.2이상에서 사용됨)
    char	szPreMapVersion[24];		//이전경로 맵버전
    int		nPreRidsCnt;			//이전경로 RID목록 개수
    int		*pnPreRids;				//이전경로 RID목록
    int		nPreSecsCnt;				//이전경로 RID의 소요시간 목록 개수
    int		*pnPreSecs;				//이전경로 RID의 소요시간 목록
    char	szPreTollgateIds[32];		//이전경로 톨게이트 ID데이터
    int		nInitSrchLength;		//초기탐색 직선거리
    char	szInitSrchSessionId[8];	//초기탐색 SessionId;
    */

    // RouteSearchProxy로 옮김
    // 19.06.12 ymhong.
#if (0)
    VSM_RerouteData rerouteData;
    VSM_Bool bAroundLink;
    VSMNavi_GetTvasReRouteData(rerouteData, bAroundLink);

    VSM_RerouteRIDData rerouteRidData;
    VSMNavi_GetTvasReRouteRIDData(rerouteRidData);

    //-------------------------------------------------------------------------
    //NDDS 6.0 재탐색 정보 - Wiki에 정리된 내용들.
    //-------------------------------------------------------------------------
    req1.sRouteServerReq.sReRouteInfo.uPassTollID = rerouteData.uPassedTollID;
    req1.sRouteServerReq.sReRouteInfo.nLastRid = rerouteRidData.lastRid;
    qstrcpy(req1.sRouteServerReq.sReRouteInfo.szPreMapVersion, rerouteRidData.preMapVersion);

    mPreRids.reserve(rerouteRidData.reRids.size());
    std::copy(std::begin(rerouteRidData.reRids), std::end(rerouteRidData.reRids), std::begin(mPreRids));
    req1.sRouteServerReq.sReRouteInfo.nPreRidsCnt = rerouteRidData.reRids.size();//maybe preRids...
    req1.sRouteServerReq.sReRouteInfo.pnPreRids = mPreRids.data();

    mPreSecs.reserve(rerouteRidData.preSecs.size());
    std::copy(std::begin(rerouteRidData.preSecs), std::end(rerouteRidData.preSecs), std::begin(mPreSecs));
    req1.sRouteServerReq.sReRouteInfo.nPreSecsCnt = rerouteRidData.preSecs.size();
    req1.sRouteServerReq.sReRouteInfo.pnPreSecs = mPreSecs.data();

    qstrcpy(req1.sRouteServerReq.sReRouteInfo.szPreTollgateIds, rerouteData.tollIdList);
    req1.sRouteServerReq.sReRouteInfo.nInitSrchLength = rerouteRidData.initSrchLength;
    qstrcpy(req1.sRouteServerReq.sReRouteInfo.szInitSrchSessionId, rerouteRidData.initSrchSessionId);

    //VSMNavi_GetRouteGuidance 함수
    //VSM_stDriveInfo:stDRGInfo:congestRid 추가 예정
    //req1.sRouteServerReq.sReRouteInfo.nTcRid = congestRid;
#endif
}

void RPnSearchServer::makeRouteRequest_AngleAndSpeed(RPRequestPtr &req)
{
    auto m = (*req->getReq());
    if (m.contains(RPREQ_KEY_Angle)) {
        req1.sRouteServerReq.nAngle = m[RPREQ_KEY_Angle].toInt();
        req1.sRouteServerSummaryReq.nAngle = m[RPREQ_KEY_Angle].toInt();
    }
    if (m.contains(RPREQ_KEY_Speed)) {
        req1.sRouteServerReq.nSpeed = m[RPREQ_KEY_Speed].toInt();
        req1.sRouteServerSummaryReq.nSpeed = m[RPREQ_KEY_Speed].toInt();
    }
}

void RPnSearchServer::makeRouteRequest_OilType(RPRequestPtr &req)
{
    auto m = *req->getReq();
    auto oilType = m[QStringLiteral("OilKind")].toInt();
    auto chargeType = m[QStringLiteral("ChargeType")].toInt();
    req1.sRouteServerReq.eOilType = changeToNddsOilType(oilType, chargeType);
}

void RPnSearchServer::makeRouteRequest_DefaultData(RPRequestPtr &req, bool reroute)
{
    makeRouteRequest_AngleAndSpeed(req);
    makeRouteRequest_OilType(req);

    // hipass 옵션 설정.
    req1.sRouteServerReq.bHipass = req->getHipass();

    req1.sRouteServerReq.byDepartDirPriority = 1;

    if (reroute) {

    } else {
        // 출발지가 사용자에 의해 변경된 경우.
        if (req->getDepartureReplaced()) {
            // 출발지가 자동설정일 때 주행각도 사용자 설정일 때에는 -1을 전달.
            req1.sRouteServerReq.nAngle = -1;

            // 출발지점 주행방향 설정
            // 자동설정일 때 1
            // 사용자설정일 때 0
            req1.sRouteServerReq.byDepartDirPriority = 0;
        }
    }
}

void RPnSearchServer::makeRouteResponse_DefaultData(){
    QString TvasPath = SKT::Environment::getRoutePath();
    TvasPath.append("/");
    //TvasPath.append("routes/");
    int arraySize = sizeof(req1.sRouteServerReq.szCollectDataPath)/sizeof(req1.sRouteServerReq.szCollectDataPath[0]);
    for(int c = 0 ; c < arraySize ; c++)
    {
        QString TvasFile = QString(TvasPath);
        TvasFile.append(QString::asprintf("tvas_route%d.db",c));
        StringUtil::strlcpy(req1.sRouteServerReq.szCollectDataPath[c],TvasFile.toLatin1(),sizeof(req1.sRouteServerReq.szCollectDataPath[c]));
    }
}

void RPnSearchServer::makeRouteResponse_Data(QStringListPtr tvasFileNames){
    if(tvasFileNames->length() > 0){
        req1.sRouteServerReq.szCollectDataPath[0][0] = '\0';
        req1.sRouteServerReq.szCollectDataPath[1][0] = '\0';
        req1.sRouteServerReq.szCollectDataPath[2][0] = '\0';
    }

    QString TvasPath = SKT::Environment::getRoutePath();
    TvasPath.append("/");
    //TvasPath.append("routes/");
    for(int c = 0 ; c < tvasFileNames->length() ; c++)
    {
        QString TvasFile = QString(TvasPath);
        TvasFile.append(tvasFileNames->at(c));
        StringUtil::strlcpy(req1.sRouteServerReq.szCollectDataPath[c],TvasFile.toLatin1(),sizeof(req1.sRouteServerReq.szCollectDataPath[c]));
    }
}

void RPnSearchServer::makeRouteRequest_GpsTraceData(RPRequestPtr& req)
{
    auto m = (*req->getReq());
    if (m.contains(RPREQ_KEY_GpsTraceData)) {
        auto gpsTraceDataBase64 = m[RPREQ_KEY_GpsTraceData].toByteArray().toBase64();
        mGpsTraceData = gpsTraceDataBase64.toStdString();

        auto size = mGpsTraceData.size();

        req1.sRouteServerReq.sGpsTraceData.pGpsTraceData = const_cast<char *>(mGpsTraceData.c_str());
        req1.sRouteServerReq.sGpsTraceData.nGpsTraceDataSize = size;
    } else {
        mGpsTraceData.clear();
    }
}

void RPnSearchServer::requestClearData(){
    memset(&req1,0,sizeof(DS_ROUTE_REQ));

    checkRouteRequestDeparture();

    makeRouteRequest_DefaultData();
    makeRouteResponse_DefaultData();
}

DeErrorCode RPnSearchServer::requestRouteUsingDualChannel(RPRequestPtr &req, int &sessionId, int &resCount)
{
    // 채널 정책을 받지 못했다면 비통신 요청 하도록 처리
    if (false == mServerPolicySuccess) {
        return DE_ERROR_ROUTE_SERVER;
    }

    DeErrorCode err;

    qDebug() << "[Perf][RPnSearchServer][requestRouteUsingDualChannel]before";

    auto retry = 0;
    auto secondaryNddsDomainUsed = false;
    const quint16 nddsPort = 9443;

    for ( ; retry < 3; retry++) {
        auto domain = retry < 2 ? primaryNddsDomain() : secondaryNddsDomain();
        if (retry == 2) {
            secondaryNddsDomainUsed = true;
        }

        NDDS_SetTmapHost(domain.toStdString().c_str(), nddsPort);

        qDebug() << "[Perf][RPnSearchServer][requestRouteUsingDualChannel][NDDS_Route]before";
        err = NDDS_Route(&req1, true, &sessionId, &resCount);
        qDebug() << "[Perf][RPnSearchServer][requestRouteUsingDualChannel][NDDS_Route]after";
        if (err != DE_ERROR_ROUTE_SERVER) {
            break;
        }
    }

    if (secondaryNddsDomainUsed) {
        auto domain = primaryNddsDomain();
        NDDS_SetTmapHost(domain.toStdString().c_str(), nddsPort);
    }

    qDebug() << "[Perf][RPnSearchServer][requestRouteUsingDualChannel]after";

    return err;
}

void RPnSearchServer::makeRouteRequest_RerouteDataUsingMap(const QVariantMap &m, bool isReroute)
{
    // 재탐색 관련 NDDS fields
    // 19.06.12 ymhong.

    // 재탐색 이외에 경로 탐색 시에도 필요한 정보. (TG 요금 관련 이슈들...)
    // 19.11.08 ymhong
    if (m.contains(RPREQ_KEY_LastTollgateId)) {
        req1.sRouteServerReq.sReRouteInfo.uPassTollID = m[RPREQ_KEY_LastTollgateId].toInt();
    }

    if (m.contains(RPREQ_KEY_PreTollgateIds)) {
        auto preTollgateIds = m[RPREQ_KEY_PreTollgateIds].toString();

        memset(req1.sRouteServerReq.sReRouteInfo.szPreTollgateIds, 0, 32);
        strcpy(req1.sRouteServerReq.sReRouteInfo.szPreTollgateIds, preTollgateIds.toStdString().c_str());
    }

    // 재탐색 시 포함 되는 정보
    if (isReroute) {
        if (m.contains(RPREQ_KEY_RadiusInfoType)) {
            req1.sRouteServerReq.sReRouteInfo.bAroundLink = m[RPREQ_KEY_RadiusInfoType].toBool();
        }

        if (m.contains(RPREQ_KEY_RadiusEntryTurnType)) {
            req1.sRouteServerReq.sReRouteInfo.uRsdCode = m[RPREQ_KEY_RadiusEntryTurnType].toInt();
        }

        if (m.contains(RPREQ_KEY_RadiusLinkId)) {
            req1.sRouteServerReq.sReRouteInfo.uLinkID = m[RPREQ_KEY_RadiusLinkId].toInt();
        }

        if (m.contains(RPREQ_KEY_RadiusMeshCode)) {
            req1.sRouteServerReq.sReRouteInfo.uTileCode = m[RPREQ_KEY_RadiusMeshCode].toInt();
        }

        if (m.contains(RPREQ_KEY_RadiusLinkDirection)) {
            req1.sRouteServerReq.sReRouteInfo.uDirInfo = m[RPREQ_KEY_RadiusLinkDirection].toInt();
        }

        if (m.contains(RPREQ_KEY_RadiusLeaveTurnType)) {
            req1.sRouteServerReq.sReRouteInfo.uOldRsdCode = m[RPREQ_KEY_RadiusLeaveTurnType].toInt();
        }

        if (m.contains(RPREQ_KEY_RadiusXPos)) {
            req1.sRouteServerReq.sReRouteInfo.uPosX = m[RPREQ_KEY_RadiusXPos].toInt();
        }

        if (m.contains(RPREQ_KEY_RadiusYPos)) {
            req1.sRouteServerReq.sReRouteInfo.uPosY = m[RPREQ_KEY_RadiusYPos].toInt();
        }

        // RerouteRidData
        if (m.contains(RPREQ_KEY_LastRid)) {
            req1.sRouteServerReq.sReRouteInfo.nLastRid = m[RPREQ_KEY_LastRid].toInt();
        }

        if (m.contains(RPREQ_KEY_PreMapVersion)) {
            auto preMapVersion = m[RPREQ_KEY_PreMapVersion].toString();
            auto arrayLen = sizeof(req1.sRouteServerReq.sReRouteInfo.szPreMapVersion) /
                            sizeof(req1.sRouteServerReq.sReRouteInfo.szPreMapVersion[0]);
            memset(req1.sRouteServerReq.sReRouteInfo.szPreMapVersion, 0, arrayLen);
            strncpy(req1.sRouteServerReq.sReRouteInfo.szPreMapVersion, preMapVersion.toStdString().c_str(), 8);
        }

        if (m.contains(RPREQ_KEY_ReRids)) {
            auto reRids = m[RPREQ_KEY_ReRids].value<QVariantList>();
            mPreRids.clear();
            for (auto reRid : reRids) {
                mPreRids.push_back(reRid.toInt());
            }
            req1.sRouteServerReq.sReRouteInfo.nPreRidsCnt = mPreRids.size();
            req1.sRouteServerReq.sReRouteInfo.pnPreRids = mPreRids.data();
        }

        if (m.contains(RPREQ_KEY_PreSecs)) {
            auto preSecs = m[RPREQ_KEY_PreSecs].value<QVariantList>();
            mPreSecs.clear();
            for (auto preSec : preSecs) {
                mPreSecs.push_back(preSec.toInt());
            }
            req1.sRouteServerReq.sReRouteInfo.nPreSecsCnt = mPreSecs.size();
            req1.sRouteServerReq.sReRouteInfo.pnPreSecs = mPreSecs.data();
        }

        if (m.contains(RPREQ_KEY_InitSrchLength)) {
            req1.sRouteServerReq.sReRouteInfo.nInitSrchLength = m[RPREQ_KEY_InitSrchLength].toInt();
        }

        if (m.contains(RPREQ_KEY_InitSrchSessionId)) {
            auto initSrchSessionId = m[RPREQ_KEY_InitSrchSessionId].toString();
            auto arraySize = sizeof(req1.sRouteServerReq.sReRouteInfo.szInitSrchSessionId) /
                             sizeof(req1.sRouteServerReq.sReRouteInfo.szInitSrchSessionId[0]);
            memset(req1.sRouteServerReq.sReRouteInfo.szInitSrchSessionId, 0, arraySize);
            qstrcpy(req1.sRouteServerReq.sReRouteInfo.szInitSrchSessionId, initSrchSessionId.toStdString().c_str());
        }
    }
}

void RPnSearchServer::requestVMSInfo(RPRequestPtr& req){
    qDebug() << "[Perf][RPnSearchServer][requestVMSInfo]before";
    int rseId = req->getVMSResID();
    DeErrorCode err = DE_ERROR_UNKNOWN;
    if(rseId >= 0){
        char sResID[20] = {0,};
        sprintf(sResID,"%d",rseId);
        qDebug() << "[Perf][RPnSearchServer][requestVMSInfo][NDDS_VmsGuideInfo]before";
        err = NDDS_VmsGuideInfo(sResID,true);
        qDebug() << "[Perf][RPnSearchServer][requestVMSInfo][NDDS_VmsGuideInfo]after";
        if(err == DE_ERROR_SUCCESS){
            DS_VMSGUIDEINFO * vmsInfo = new DS_VMSGUIDEINFO();
            QVMSGuideInfoPtr VMSGuideInfoP = QVMSGuideInfoPtr(vmsInfo);
            qDebug() << "[Perf][RPnSearchServer][requestVMSInfo][NDDS_GetVmsGuideInfo]after";
            err = NDDS_GetVmsGuideInfo(vmsInfo);
            qDebug() << "[Perf][RPnSearchServer][requestVMSInfo][NDDS_GetVmsGuideInfo]after";
            if(err == DE_ERROR_SUCCESS){
                (*req->getRes())[RPREQ_KEY_VMSGuideInfo] = QVariant::fromValue<QVMSGuideInfoPtr>(VMSGuideInfoP);
            }
        }
    }
    (*req->getRes())[RPREQ_KEY_ErrorCode] = err;
    qDebug() << "[Perf][RPnSearchServer][requestVMSInfo]after";
}

void RPnSearchServer::parseSessionId(const char *p)
{
    QMutexLocker lock(&mSessionMutex);
    const int SESSION_ID_LENGTH = 24;
    mSessionId = QString::fromLocal8Bit(p, SESSION_ID_LENGTH);
}

QString RPnSearchServer::sessionId()
{
    QMutexLocker lock(&mSessionMutex);
    return mSessionId;
}

}
