#include "rpnsearch.h"
#include "rpnsearchserver.h"

#include "VSM_DataType.h"
#include "VSM.h"
#include "VSM_Map.h"
#include "VSM_Navi.h"
#include "VSM_Util.h"

#include "environment.h"
#include "tmap_rsm_ui.h"

#include "tmapcontroller.h"
#include "stringutil.h"

#include <QThread>
#include <QQueue>
#include <QWaitCondition>
#include <QDebug>
#include <QFile>
#include <QTextCodec>
#include <QDate>
#include <QtCore/qglobal.h>
#include <QMutex>
#include <QElapsedTimer>
#include <sys/stat.h>
#include "hostnames.h"

namespace SKT {

#define ServerClient ((RPnSearchServer*)client)
#define LocalClient ((RPnSearch*)client)

void nddsLogHandler(int level, const char* msg)
{
#ifndef QT_NO_DEBUG_OUTPUT
    ndds_log_handler(level,msg);
#endif
}

void checkdfile(const char*);

class RequestExecutor : public QThread
{
public:
    RequestExecutor(RPnSearch * client);
    ~RequestExecutor();

    void insertRequest(RPReqObject *reqObj);
    void abort();

protected:
    void run() Q_DECL_OVERRIDE;

private:
    RPnSearch *client;
    RPReqObject *activeReq;
    QQueue<RPReqObject*> commandQueue;
    QWaitCondition waitForCommand;
    QMutex lockQueue;
    QMutex lockMethod;
    bool aborted;

    void exeMethod(RPReqObject& req);
    void execute(RPReqObject& req);
    bool immediatelyCommand(RPReqObject *req);
    void cancelQueue();
    void clearQueue();
    bool isRouteServerError(RPReqObject& req);
    bool isRouteServerSuccess(RPReqObject& req);
};

RequestExecutor::RequestExecutor(RPnSearch * client){
    this->client = client;
    activeReq = NULL;
    aborted = false;
}

RequestExecutor::~RequestExecutor(){
    clearQueue();
}

void RequestExecutor::clearQueue(){
    for(int c = 0 ; c < commandQueue.size() ;){
        RPReqObject* req = commandQueue.dequeue();
        delete req;
    }
    commandQueue.clear();
}

bool RequestExecutor::isRouteServerError(RPReqObject &reqObj)
{
    RPRequestPtr req = reqObj.request();
    QVariantMap& resData = (*req->getRes());
    if (resData.contains(RPREQ_KEY_ErrorCode)) {
        auto errorCode = resData[RPREQ_KEY_ErrorCode].toInt();
        return errorCode == DE_ERROR_ROUTE_SERVER;
    }
    return false;
}

bool RequestExecutor::isRouteServerSuccess(RPReqObject &reqObj)
{
    RPRequestPtr req = reqObj.request();
    QVariantMap& resData = (*req->getRes());
    if (resData.contains(RPREQ_KEY_ErrorCode)) {
        auto errorCode = resData[RPREQ_KEY_ErrorCode].toInt();
        return errorCode == DE_ERROR_SUCCESS;
    }
    return false;
}

void RequestExecutor::cancelQueue(){
    if(activeReq != NULL) {
        RPRequestPtr req = activeReq->request();
        (*(activeReq->request()->getReq()))[RPREQ_KEY_State] = RPRequest::RequestState_Invalid_Cancel;
    }
    for(int c = 0 ; c < commandQueue.size() ; c++){
        RPReqObject* reply = commandQueue.at(c);
        (*(reply->request()->getReq()))[RPREQ_KEY_State] = RPRequest::RequestState_Invalid_Cancel;
    }
}

void RequestExecutor::insertRequest(RPReqObject *reqObj){
    (*(reqObj->request()->getReq())).insert(RPREQ_KEY_State,RPRequest::RequestState_Valid);

    QMutexLocker locker(&lockQueue);
    if(immediatelyCommand(reqObj) == true){
        return;
    }
    commandQueue.enqueue(reqObj);
    waitForCommand.wakeAll();
}

void RequestExecutor::abort()
{
    QMutexLocker locker(&lockQueue);
    if (!aborted) {
        aborted = true;
        waitForCommand.wakeAll();
    }
}

bool RequestExecutor::immediatelyCommand(RPReqObject *reqObj){
    RPRequestPtr req = reqObj->request();

    switch(req->getRequestType()){
    case RPRequest::RequestCancel:
        qDebug() << "RPnSearch::immediatelyCommand RequestType RequestCancel";
        execute(*reqObj);
        cancelQueue();
        return true;
    }
    return false;
}

void RequestExecutor::exeMethod(RPReqObject& req){
    // TMCRSM-1833.
    // reqeust의 실행이 끝날 때 까지 queue를 잠근다.
    // 경로 탐색 중에 RequestCancel을 요청해도 실행중에는 여기서 대기하게 됨.
    // QMutexLocker locker(&lockQueue);
    execute(req);
}

void RequestExecutor::run() {
    forever {
        lockQueue.lock();
        while (!aborted && commandQueue.empty()) {
            waitForCommand.wait(&lockQueue);
        }
        if (aborted) {
            lockQueue.unlock();
            break;
        }
        activeReq = commandQueue.dequeue();
        lockQueue.unlock();

        exeMethod(*activeReq);
        activeReq = NULL;
    }
}

void RequestExecutor::execute(RPReqObject &reqObj){
    RPRequestPtr req = reqObj.request();

    // 추후 문자열 타입을 정수 타입으로 변경 해야함. // will delete
    QVariantMap& m = *(req->getReq().data());
    /*if(!m["Command"].isNull())*/{
        if(m["Command"].toString() == "경로 요청")
            req->setRequestType(RPRequest::RequestRoute);
    }

    QVariantMap& reqData = *(req->getReq());
    QVariantMap& resData = *(new QVariantMap());
    resData.insert(RPREQ_KEY_Answer,RPREQ_Error_Success);
    resData.insert(RPREQ_KEY_AnswerCode,RPRequest::ResponseAnswer_Done);
    resData.insert(RPREQ_KEY_ErrorCode,E_ERROR_SUCCESS);
    reqData.insert(RPREQ_KEY_Response,QVariant::fromValue(QVariantMapPtr(&resData)));

    if(reqData[RPREQ_KEY_State] == RPRequest::RequestState_Valid)
    {
        int requestType = req->getRequestType();
        client->setActiveRequest(req);
        switch(requestType){
        case RPRequest::RequestRouteSummary:
            if(client->canOnlineRequest(req)) {
                ServerClient->requestRouteSummary(req);
                if (!isRouteServerSuccess(reqObj)) {
                    LocalClient->requestRouteSummary(req);
                    resData[RPREQ_KEY_SwitchToLocal] = true;
                }
            } else {
                LocalClient->requestRouteSummary(req);
            }
            break;
        case RPRequest::RequestRoute:
            if (client->isOutOfDistance(req)) {
                (*req->getRes())[RPREQ_KEY_ErrorCode] = E_ERROR_ROUTE_DISTOVER;
            } else {
                if(client->canOnlineRequest(req)) {
                    ServerClient->requestRoute(req);
                    if (isRouteServerError(reqObj)) {
                        LocalClient->requestRoute(req);
                        resData[RPREQ_KEY_SwitchToLocal] = true;
                    }
                } else {
                    LocalClient->requestRoute(req);
                }
            }
            break;
        case RPRequest::RequestReRouteForce:
        case RPRequest::RequestReRouteByAuto:
        case RPRequest::RequestReRouteByBreakaway:
            if(client->canOnlineRequest(req)) {
                ServerClient->requestReroute(req);
                if (isRouteServerError(reqObj)) {
                    LocalClient->requestReroute(req);
                    resData[RPREQ_KEY_SwitchToLocal] = true;
                }
            } else {
                LocalClient->requestReroute(req);
            }
            break;

        case RPRequest::RequestCancel:
            if(client->canOnlineRequest(req)) {
                ServerClient->cancelRoute(req);
            } else {
                LocalClient->cancelRoute(req);
            }
            break;

        case RPRequest::RequestVMSInfo:
            if(client->canOnlineRequest(req)) {
                ServerClient->requestVMSInfo(req);
            } else {
                resData[RPREQ_KEY_AnswerCode] = RPRequest::ResponseAnswer_NotSupport;
            }
            break;

        default:
            resData[RPREQ_KEY_Answer] = RPREQ_Error_NotSupport;
            resData[RPREQ_KEY_AnswerCode] = RPRequest::ResponseAnswer_NotSupport;
        }
    }
    else{
        resData[RPREQ_KEY_Answer] = RPREQ_Error_Cancel;
        resData[RPREQ_KEY_AnswerCode] = RPRequest::ResponseAnswer_Cancel;
    }

    QMetaObject::invokeMethod(&reqObj, "finished", Qt::QueuedConnection);
    client->setActiveRequest(RPRequestPtr());
}

bool MakeRouteRequest_CheckPosition(TS_ROUTE_POS &data);

RPnSearch* RPnSearch::s_instance = NULL;

RPnSearch::RPnSearch()
    : m_init(false)
    , m_isSync(false)
    , m_commerce(false)
    , executor(0)
    , m_controller(0)
{

}

RPnSearch* RPnSearch::sharedInstance(void) {
    if (!s_instance) {
        s_instance = (RPnSearch*) new RPnSearchServer();
    }
    return s_instance;
}

void RPnSearch::setDeviceId(QString deviceID)
{
    m_deviceID = deviceID;
}

void RPnSearch::init(
    TmapController *controller,
    const QString &rootPath,
    const QString& serverURL,
    int serverPort,
    bool commerce,
    bool avnext) {
    if (true == m_init) {
        return;
    }

    Q_ASSERT(controller);

    MNS_ErrorCode eError;

    m_controller = controller;
    connect(m_controller, &TmapController::mapDataChanged, this, &RPnSearch::onMapDataChanged);
    connect(m_controller, SIGNAL(nddsServerPolicyFinished(bool)), this, SLOT(onChannelServerPolicyFinished(bool)));

    m_init = true;
    m_isSync = false;
    m_commerce = commerce;
    m_avnext   = avnext;
    
    positionCount = 0;
    memset(routePosition,0,sizeof(routePosition));

    QString extendPath = SKT::Environment::getRouteSearchPath();

    TS_ENGINEINIT_EX sInitEx;
    memset(&sInitEx, 0, sizeof(TS_ENGINEINIT_EX));
    strcpy(sInitEx.szAuthCode, "48ed800029ae");
    sInitEx.eOSType = E_OSTYPE_LINUX;
    strcpy(sInitEx.szOSVersion, "4.3");
    strcpy(sInitEx.szModelNo, "MM2018");
    strcpy(sInitEx.szAppVersion, "1.0.1");
    strcpy(sInitEx.szAppBuildNo, "000001");

    MNS_SetLogFunction(localeng_log_handler);   // 로그 핸들러 연동 함수. 2019.01.10 by hheo

    eError = MNS_Initialize(rootPath.toLatin1().data()
                  , extendPath.toLatin1().data()
                  , "FGACUI5D"
                  , true
                  , callbackStatus
                  , &sInitEx);
    qDebug() << "RPnSearch::init - MNS_Initialize result : " << eError;
    DS_NDDSINIT_EX sNddsInitEx;
    memset(&sNddsInitEx, 0, sizeof(DS_NDDSINIT_EX));
    sNddsInitEx.eCharSet = DE_CHARACTERSET_EUCKR;
    sNddsInitEx.bServerCommerce = commerce;
    strcpy(sNddsInitEx.szAuthCode, "48ed800029ae");
    sNddsInitEx.eOSType = DE_OSTYPE_LINUX;
    strcpy(sNddsInitEx.szOSVersion, "4.3");
    strcpy(sNddsInitEx.szModelNo, "MM2018");
    strcpy(sNddsInitEx.szAppVersion, "1.0.1");
    strcpy(sNddsInitEx.szAppBuildNo, "000001");
    strcpy(sNddsInitEx.szDeviceID, m_deviceID.toLatin1().data());

    strcpy(sNddsInitEx.szDeviceIDHu, m_deviceID.toLatin1().data());
    strcpy(sNddsInitEx.szDeviceModleName, "K5");
    strcpy(sNddsInitEx.szDeviceModelNo, "JF");
    strcpy(sNddsInitEx.szVendor, "RSM");//JPIVI
    strcpy(sNddsInitEx.szSerialKey, "RSM01SGA44JK43AE2ZQJIWMOM");

    strcpy(sNddsInitEx.szServerURL, serverURL.toLatin1().data());
    sNddsInitEx.nServerPort = serverPort;

    strcpy(sNddsInitEx.szAccessKey, ""); // accesskey for test

#ifndef Q_OS_WIN32
    NDDS_SetLogFunction(nddsLogHandler);
    NDDS_Initialize(true, callbackNddsStatus, &sNddsInitEx);
    checkdfile(SKT::Environment::getPreferencePath().toLatin1().constData());

   /*
#if 0
    // 5.2.18.	주변 전기차 충전소 조회
    DeErrorCode eError1;
    int nCnt=0;
    DS_SEARCH_REQ sSearchReq;
    DeSearchType eSearchType;
    memset(&sSearchReq, 0x00, sizeof(DS_SEARCH_REQ));

    eSearchType = DE_SEARCHTYPE_SERVERPOIEVSTATIONSBYPOINT;
    sSearchReq.eSearchType = eSearchType;
    sSearchReq.sSearchServerPoiEvStationsByPoint.nDist = 33;
    sSearchReq.sSearchServerPoiEvStationsByPoint.tp.x = 66569576;
    sSearchReq.sSearchServerPoiEvStationsByPoint.tp.y = 19687233;

    eError1 = NDDS_Search(&sSearchReq, true, nCnt);

    if(eError1 == DE_ERROR_SUCCESS)
    {
        DS_SEARCH_LIST	SearchList[200];
        memset(SearchList, 0, sizeof(SearchList));
        eError1 = NDDS_GetSearchResultList(eSearchType, 0, nCnt, SearchList);
        for(int i=0; i<200; i++)
        {
             qDebug() << "BYPOINT " << SearchList[i].szStdName << ","<<SearchList[i].tp.x << "," << SearchList[i].tp.y;
        }
    }
#endif

#if 0
   // 최저가 주유소(충전소) 조회
   DS_SEARCH_REQ sSearchReq;
   DeSearchType eSearchType;
   int nCnt = 1;
   memset(&sSearchReq, 0x00, sizeof(DS_SEARCH_REQ));
   eSearchType = DE_SEARCHTYPE_SERVERPOISTATIONINFO;
   sSearchReq.eSearchType = eSearchType;
   sSearchReq.sSearchServerPoiStationInfo.eSearchType = eSearchType;
   sSearchReq.sSearchServerPoiStationInfo.nDist = 9;
   strcpy(sSearchReq.sSearchServerPoiStationInfo.szDataKind, "3");
   strcpy(sSearchReq.sSearchServerPoiStationInfo.szFuelType, "FUELTYPE_GAS");
   sSearchReq.sSearchServerPoiStationInfo.tp.x = 4570966;
   sSearchReq.sSearchServerPoiStationInfo.tp.y = 1351818;
   DeErrorCode eNddsError = NDDS_Search(&sSearchReq, true, nCnt);

   DS_SEARCH_LIST	SearchList[10];
   memset(SearchList, 0, sizeof(SearchList));
   eNddsError = NDDS_GetSearchResultList(eSearchType, 0, nCnt, SearchList);
#endif

#if 1
    //send to car sample
    int nCnt=0;
    //차량정보 저장
    //결과는 NDDS callback Param2로 전달됨(1:성공, 0:실패)
    DS_FAVORITE_REQ sFavoriteReq;
    memset(&sFavoriteReq, 0x00, sizeof(sFavoriteReq));
    sFavoriteReq.eFavoriteType = DE_FAVORITETYPE_REGISTERSENDTOCARINFO;
    strcpy(sFavoriteReq.sFavoriteRegistSendToCarInfo.szCustName , "seoulstation");
    sFavoriteReq.sFavoriteRegistSendToCarInfo.tp.x = 66569576;
    sFavoriteReq.sFavoriteRegistSendToCarInfo.tp.y = 19687233;
    sFavoriteReq.sFavoriteRegistSendToCarInfo.byRPFlag = 4;
    sFavoriteReq.sFavoriteRegistSendToCarInfo.nPoiID = 624361;
    strcpy(sFavoriteReq.sFavoriteRegistSendToCarInfo.szNavSeq, "0");
    DeErrorCode eError1 = NDDS_Favorite(&sFavoriteReq, true, nCnt);

    //차량정보 조회
    memset(&sFavoriteReq, 0x00, sizeof(sFavoriteReq));
    sFavoriteReq.eFavoriteType = DE_FAVORITETYPE_FINDSENDTOCARINFO;
    sFavoriteReq.sFavoriteFindSendToCarInfo.bNewAddr = false;
    eError1 = NDDS_Favorite(&sFavoriteReq, true, nCnt);
    DS_SENDTOCARINFO sSend2Car={0,};
    NDDS_GetSendToCarInfo(&sSend2Car);

    //차량정보 삭제
    //결과는 NDDS callback Param2로 전달됨(1:성공, 0:실패)
    memset(&sFavoriteReq, 0x00, sizeof(sFavoriteReq));
    sFavoriteReq.eFavoriteType = DE_FAVORITETYPE_REMOVESENDTOCAR;
    strcpy(sFavoriteReq.sFavoriteRemoveSendToCarInfo.szCustName , "seoulstation");
    sFavoriteReq.sFavoriteRemoveSendToCarInfo.tp.x = 66569576;
    sFavoriteReq.sFavoriteRemoveSendToCarInfo.tp.y = 19687233;
    eError1 = NDDS_Favorite(&sFavoriteReq, true, nCnt);

    //차량정보 삭제후 재조회
    memset(&sFavoriteReq, 0x00, sizeof(sFavoriteReq));
    sFavoriteReq.eFavoriteType = DE_FAVORITETYPE_FINDSENDTOCARINFO;
    sFavoriteReq.sFavoriteFindSendToCarInfo.bNewAddr = false;
    eError1 = NDDS_Favorite(&sFavoriteReq, true, nCnt);
    sSend2Car={0,};
    NDDS_GetSendToCarInfo(&sSend2Car);
#endif

   //최근목적지, 즐겨찾기 조회 샘플 코드

   DS_SEARCH_REQ sSearchReq;
   DeSearchType eSearchType;
   memset(&sSearchReq, 0x00, sizeof(DS_SEARCH_REQ));

    DS_FAVORITE_REQ sFavoriteReq;
    sFavoriteReq.eFavoriteType = DE_FAVORITETYPE_SERVER;
    sFavoriteReq.sFavoriteServer.bMyFavorite = true;
    int nCnt;
    DeErrorCode enError = NDDS_Favorite(&sFavoriteReq, true, nCnt);

    DS_FAVORITE_LIST	FavList[250];
    enError = NDDS_GetFavoriteResultList(sFavoriteReq.eFavoriteType, 0, nCnt, FavList);

    eSearchType = DE_SEARCHTYPE_SERVERRECENTDESTNATION;
    sSearchReq.eSearchType = eSearchType;
    enError = NDDS_Search(&sSearchReq, false, nCnt);

    DS_SEARCH_LIST	SearchList[250];
    memset(SearchList, 0, sizeof(SearchList));
    enError = NDDS_GetSearchResultList(eSearchType, 0, nCnt, SearchList);

   //경로요약 샘플 코드

   //목적지1
   DS_ROUTE_REQ sRouteReq;
   memset(&sRouteReq, 0x00, sizeof(DS_ROUTE_REQ));
   DS_DATETIME stDate={0,};
   sRouteReq.eRouteType = DE_ROUTETYPE_SUMMARY;
   sRouteReq.sRouteServerSummaryReq.eRouteSummaryType = DE_ROUTESUMMARYTYPE_OPTIMUM;
   sRouteReq.sRouteServerSummaryReq.eRouteServiceType = DE_ROUTESERVICETYPE_REALTIME_DUPLEX;
   sRouteReq.sRouteServerSummaryReq.nVertexFlag = 1;
   sRouteReq.sRouteServerSummaryReq.nPosCnt = 2;
   strcpy(sRouteReq.sRouteServerSummaryReq.sPosInfo[0].szName, "T.G.IFriday");
   sRouteReq.sRouteServerSummaryReq.sPosInfo[0].tp.x = 66569576;
   sRouteReq.sRouteServerSummaryReq.sPosInfo[0].tp.y = 19687233;
   strcpy(sRouteReq.sRouteServerSummaryReq.sPosInfo[1].szName, "dest1");
   sRouteReq.sRouteServerSummaryReq.sPosInfo[1].tp.x = 66579698;// 66567541;
   sRouteReq.sRouteServerSummaryReq.sPosInfo[1].tp.y = 19621930;// 19555271;
   sRouteReq.sRouteServerSummaryReq.sPosInfo[1].nPoiID = 5934;
   sRouteReq.sRouteServerSummaryReq.sPosInfo[1].byRPFlag = 9;
   sRouteReq.sRouteServerSummaryReq.nAngle = 0;
   sRouteReq.sRouteServerSummaryReq.nSpeed = 0;
   sRouteReq.sRouteServerSummaryReq.byDepartDirPriority = 0x00;
   sRouteReq.sRouteServerSummaryReq.byDepartSrchFlag = 7;
   sRouteReq.sRouteServerSummaryReq.byDestSrchFlag = 27;
   if (NDDS_Route(&sRouteReq, false) == DE_ERROR_SUCCESS)
   {
   }

   //목적지2
   strcpy(sRouteReq.sRouteServerSummaryReq.sPosInfo[1].szName, "dest2");
   sRouteReq.sRouteServerSummaryReq.sPosInfo[1].tp.x = 66576435 ;// 66567541;
   sRouteReq.sRouteServerSummaryReq.sPosInfo[1].tp.y = 19686927;// 19555271;
   sRouteReq.sRouteServerSummaryReq.sPosInfo[1].nPoiID = 12693;
   sRouteReq.sRouteServerSummaryReq.sPosInfo[1].byRPFlag = 7;
   if (NDDS_Route(&sRouteReq, false) == DE_ERROR_SUCCESS)
   {
   }

   //목적지3
   strcpy(sRouteReq.sRouteServerSummaryReq.sPosInfo[1].szName, "dest3");
   sRouteReq.sRouteServerSummaryReq.sPosInfo[1].tp.x = 66594610;// 66567541;
   sRouteReq.sRouteServerSummaryReq.sPosInfo[1].tp.y = 19683024;// 19555271;
   sRouteReq.sRouteServerSummaryReq.sPosInfo[1].nPoiID = 54984;
   sRouteReq.sRouteServerSummaryReq.sPosInfo[1].byRPFlag = 5;
   if (NDDS_Route(&sRouteReq, false) == DE_ERROR_SUCCESS)
   {

   }

   */

   /*
   // NDDS AUTH sample code
   NDDS_AuthTmapForSmartCar("767170", true);
   DS_AUTHRESULT stAuthResult;
   NDDS_GetAuthResult(&stAuthResult);
   NDDS_SetAuthAccessKey("8GA5643_56D63358011D86BFE053CECBEBCBA713S");
   */

    // NDDS 5.2.3, 5.2.4 sample code
   /*
    DeErrorCode eError1;
    int nCnt=0;
    DS_SEARCH_REQ sSearchReq;
    DeSearchType eSearchType;
    memset(&sSearchReq, 0x00, sizeof(DS_SEARCH_REQ));


    eSearchType = DE_SEARCHTYPE_SERVERPOIDETAILINFO;
    sSearchReq.eSearchType = eSearchType;
    sSearchReq.sSearchServerPoiDetailInfo.nPoiID = 252640;
    sSearchReq.sSearchServerPoiDetailInfo.nSizeWidth = 800;
    sSearchReq.sSearchServerPoiDetailInfo.nSizeHeight = 480;
    strcpy(sSearchReq.sSearchServerPoiDetailInfo.navSeq, "0");
    eError1 = NDDS_Search(&sSearchReq, true, nCnt);

    DS_SEARCH_POI_DETAIL_INFO stDetailResult;
    NDDS_GetSearchPoiDetailInfo(&stDetailResult);

    eSearchType = DE_SEARCHTYPE_SERVERPOIJOININFO;
    sSearchReq.eSearchType = eSearchType;
    sSearchReq.sSearchServerPoiJoinInfo.nPoiID = 1585567;
    eError1 = NDDS_Search(&sSearchReq, true, nCnt);

    DS_SEARCH_POI_JOIN_INFO stJoinResult[8]={0,};
    NDDS_GetSearchPoiJoinInfo(0, nCnt, stJoinResult);
    */
#endif

   executor = new RequestExecutor(this);
   executor->start();
}

RPnSearch::~RPnSearch(){

}

void RPnSearch::destroy(){
    if (m_init) {
        if (executor) {
            executor->abort();
            executor->wait();
            delete executor;
            executor = 0;
        }
        MNS_Uninitialize();
#ifndef Q_OS_WIN32
        NDDS_Uninitialize();
#endif
        m_init = false;
    }
}

bool RPnSearch::isSync() const {
    return m_isSync;
}

RPRequestHandle *RPnSearch::createRequest(RPRequestPtr req)
{
    return new RPReqObject(req, this);
}

void RPnSearch::requestCommand(RPRequestHandle *req)
{
    RPReqObject* r = qobject_cast<RPReqObject*>(req->qobject());

    if(m_isSync == false){
        executor->insertRequest(r);
    }
    else{
    }
}

void RPnSearch::onChannelServerPolicyFinished(bool success)
{
    mServerPolicySuccess = success;
}

void RPnSearch::onMapDataChanged()
{
    QMutexLocker locker(&m_mapDataLock);

    std::string dbPath;
    VSMNavi_GetSDIDBPath(dbPath);
    m_safeDBPath = QString::fromLatin1(dbPath.c_str(), dbPath.size());

    std::string indexPath;
    VSMNavi_GetSDIIndexPath(indexPath);
    m_safeDBIndexPath = QString::fromLatin1(indexPath.c_str(), indexPath.size());
}

void RPnSearch::setActiveRequest(RPRequestPtr req){
    QMutexLocker lock(&s_mnsMutex);

    activeRequest = req;
}

void RPnSearch::changeDeparture_currentPosition(){
    VSM_Int32 x = 0, y = 0;

    if (!VSMNavi_GetCurrentPosition(x, y)) {
        double longitude, latitude;
        m_controller->currentPosition(longitude, latitude);

        VSMPoint2f geo;
        geo.x = longitude;
        geo.y = latitude;

        VSMPoint2i world;
        VSMMap_Util_ConvertToWorld(geo, VSM_COORDINATE_WGS84, world);

        x = world.x;
        y = world.y;
    }

    routePosition[RpRouteDeparture].tp.x = x;
    routePosition[RpRouteDeparture].tp.y = y;
}

void RPnSearch::checkRouteRequestDeparture(){
    if(!isValidRouteRequestPosition(routePosition[RpRouteDeparture])){
        changeDeparture_currentPosition();
    }
}

bool RPnSearch::isValidRouteRequestPosition(TS_ROUTE_POS & pos){
    if(pos.tp.x == 0 && pos.tp.y == 0)
        return false;
    return true;
}

void RPnSearch::clearRouteRequestPosition(TS_ROUTE_POS & pos){
    memset(&pos,0,sizeof(TS_ROUTE_POS));
}

QString RPnSearch::primaryNddsDomain() const
{
    if (mPrimaryNddsDomain.length()) {
        return mPrimaryNddsDomain;
    } else {
        return TmapHostNames::getHostName(TmapHostNames::HOST_NDDS_INDEX);
    }
}

QString RPnSearch::secondaryNddsDomain() const
{
    if (mSecondaryNddsDomain.length()) {
        return mSecondaryNddsDomain;
    } else {
        return TmapHostNames::getHostName(TmapHostNames::HOST_NDDS2_INDEX);
    }
}

void RPnSearch::requestRouteSummary(RPRequestPtr& req){
    setVia(RPnSearch::RpRouteDeparture,req->getVia(1));
    setVia(RPnSearch::RpRouteVia1,req->getVia(2));
    setVia(RPnSearch::RpRouteVia2,req->getVia(3));
    setVia(RPnSearch::RpRouteVia3,req->getVia(4));
    setVia(RPnSearch::RpRouteDestination,req->getVia(5));

    QRouteOptionListPtr rpOptions = req->getRPOptions();
    QStringListPtr tvasFileName = req->getTvasFileName();

    if(rpOptions.isNull() == true){
        QList<TeRouteOption> *opts = new QList<TeRouteOption>();
        opts->append(E_ROUTEOPTION_OPTIMUM);
        rpOptions = QRouteOptionListPtr(opts);
    }
    if(tvasFileName.isNull() == true){
        QStringList *files = new QStringList();
        files->append("summary.db");
        tvasFileName = QStringListPtr(files);
    }
    qDebug() << "RPnSearch::requestRouteSummary option " << rpOptions->at(0);

    requestClearData();
    makeRouteResponse_Data(tvasFileName->at(0));
    makeRouteRequest_Option(rpOptions->at(0));
    makeRouteRequest_TvasVersion(req->getTvasVersion());
    makeRouteRequest_Hipass(req->getHipass());
    makeRouteRequest_AngleAndSpeed(req);

    req1_test.nSDICollect = E_SDICOLLECT_NONE;  // 탐색 요약을 위해서는 VSMSDI가 항상 미포함 되도록 한다. - 속도를 위하여.
    if((*req->getReq())[RPREQ_KEY_State] == RPRequest::RequestState_Valid){
        MNS_ErrorCode err = MNS_Route(&req1_test, true, &res1_test);
        qDebug() << "MNS_ErrorCode err " << err;
        (*req->getRes())[RPREQ_KEY_ErrorCode] = err;
        if(err == E_ERROR_ROUTE_USERCANCEL
                || (*req->getReq())[RPREQ_KEY_State] != RPRequest::RequestState_Valid){
            qDebug() << "RPnSearch::requestRoute RequestState_Valid Not " << err;
            (*req->getRes())[RPREQ_KEY_Answer] = RPREQ_Error_Cancel;
            (*req->getRes())[RPREQ_KEY_AnswerCode] = RPRequest::ResponseAnswer_Cancel;
            (*req->getRes())[RPREQ_KEY_ErrorCode] = err;
        }
        RouteSummaryInfo *info = new RouteSummaryInfo();
        QRouteSummaryInfo infoP = QRouteSummaryInfo(info);
        info->nTotalDist = tvasParse.parsedData.header.header45.nTotalDist;
        info->nTotalTime = tvasParse.parsedData.header.header45.nTotalTime;
        (*req->getRes())[RPREQ_KEY_RouteSummaryInfo] = QVariant::fromValue<QRouteSummaryInfo>(infoP);
    }else {
        qDebug() << "RPnSearch::requestRoute RequestState_Invalid_Cancel ";
        (*req->getRes())[RPREQ_KEY_Answer] = RPREQ_Error_Cancel;
        (*req->getRes())[RPREQ_KEY_AnswerCode] = RPRequest::ResponseAnswer_Cancel;
        (*req->getRes())[RPREQ_KEY_ErrorCode] = E_ERROR_UNKNOWN;
    }
}

void RPnSearch::requestRoute(RPRequestPtr& req){
    qDebug() << "[Perf][RPnSearch][requestRoute]before";

    setVia(RPnSearch::RpRouteDeparture,req->getVia(1));
    setVia(RPnSearch::RpRouteVia1,req->getVia(2));
    setVia(RPnSearch::RpRouteVia2,req->getVia(3));
    setVia(RPnSearch::RpRouteVia3,req->getVia(4));
    setVia(RPnSearch::RpRouteDestination,req->getVia(5));

    QRouteOptionListPtr rpOptions = req->getRPOptions();
    QStringListPtr tvasFileName = req->getTvasFileName();

    auto routeCount = tvasFileName->length();
    routeCount = 1;

    for(int c = 0 ; c < routeCount ; c++){
        qDebug() << "RPnSearch::requestRoute option " << rpOptions->at(c);
        qDebug() << "RPnSearch::requestRoute tvasFileName " << tvasFileName->at(c);

        requestClearData();
        makeRouteResponse_Data(tvasFileName->at(c));
        makeRouteRequest_Option(rpOptions->at(c));
        makeRouteRequest_TvasVersion(req->getTvasVersion());
        makeRouteRequest_Hipass(req->getHipass());
        makeRouteRequest_AngleAndSpeed(req);

        req1_test.uPassTollID = req->getLastTollgateId();
        req1_test.nSDICollect = E_SDICOLLECT_NONE;  // 신규 탐색에는 VSMSDI가 미포함 되도록 한다. - 속도를 위하여.
        if(req->getRPCalcOption() == true){
            req1_test.nSDICollect = E_SDICOLLECT_ALL;

            QMutexLocker locker(&m_mapDataLock);
            StringUtil::strlcpy(req1_test.szSDIDataFilePath, m_safeDBPath.toLatin1().constData(),
                                sizeof(req1_test.szSDIDataFilePath));
            StringUtil::strlcpy(req1_test.szSDIIndexFilePath, m_safeDBIndexPath.toLatin1().constData(),
                                sizeof(req1_test.szSDIIndexFilePath));
        }
        if((*req->getReq())[RPREQ_KEY_State] == RPRequest::RequestState_Valid){
            QElapsedTimer elapsedTimer;
            elapsedTimer.start();
            qDebug() << "[Perf][RPnSearch][requestRoute][MNS_Route]before";
            MNS_ErrorCode err = MNS_Route(&req1_test, true, &res1_test);
            qDebug() << "[Perf][RPnSearch][requestRoute][MNS_Route]after";
            qDebug() << "MNS_Route() : " << elapsedTimer.elapsed() << "ms";
            qDebug() << "MNS_ErrorCode err " << err;
            (*req->getRes())[RPREQ_KEY_ErrorCode] = err;
            (*req->getRes())["elapsed"] = elapsedTimer.elapsed();
            if(err == E_ERROR_ROUTE_USERCANCEL
                    || (*req->getReq())[RPREQ_KEY_State] != RPRequest::RequestState_Valid){
                qDebug() << "RPnSearch::requestRoute RequestState_Valid Not " << err;
                (*req->getRes())[RPREQ_KEY_Answer] = RPREQ_Error_Cancel;
                (*req->getRes())[RPREQ_KEY_AnswerCode] = RPRequest::ResponseAnswer_Cancel;
                (*req->getRes())[RPREQ_KEY_ErrorCode] = err;
                break;
            }
        }else {
            qDebug() << "RPnSearch::requestRoute RequestState_Invalid_Cancel ";
            (*req->getRes())[RPREQ_KEY_Answer] = RPREQ_Error_Cancel;
            (*req->getRes())[RPREQ_KEY_AnswerCode] = RPRequest::ResponseAnswer_Cancel;
            (*req->getRes())[RPREQ_KEY_ErrorCode] = E_ERROR_UNKNOWN;
            break;
        }
    }

    qDebug() << "[Perf][RPnSearch][requestRoute]after";
}

void RPnSearch::requestReroute(RPRequestPtr& req)
{
    qDebug() << "[Perf][RPnSearch][requestReroute]before";

    setVia(RPnSearch::RpRouteDeparture,req->getVia(1));
    setVia(RPnSearch::RpRouteVia1,req->getVia(2));
    setVia(RPnSearch::RpRouteVia2,req->getVia(3));
    setVia(RPnSearch::RpRouteVia3,req->getVia(4));
    setVia(RPnSearch::RpRouteDestination,req->getVia(5));

    changeDeparture_currentPosition();

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

    for(int c = 0 ; c < rpOptions->length() && c < tvasFileName->length() ; c++){
        qDebug() << "RPnSearch::requestReroute option " << rpOptions->at(c);
        qDebug() << "RPnSearch::requestReroute tvasFileName " << tvasFileName->at(c);

        requestClearData();
        makeRouteResponse_Data(tvasFileName->at(c));
        makeRouteRequest_Option(rpOptions->at(c));
        makeRouteRequest_TvasVersion(req->getTvasVersion());
        makeRouteRequest_Hipass(req->getHipass());
        makeRouteRequest_AngleAndSpeed(req);

        const int EXPLORER_RE_USER_DES = 0x02; // 사용자 재탐색색

        // 목적지 탐색 코드
        auto destSearchFlag = req->getDestSearchCode();
        if (destSearchFlag == EXPLORER_RE_USER_DES) {
            // 사용자 재탐색(10초뒤 자동 재탐색 or 재탐색 버튼을 눌렀을 때)
        } else {
            // 경로 이탈 재탐색
        }

        req1_test.uPassTollID = req->getLastTollgateId();

        req1_test.eRouteType = E_ROUTETYPE_RE_OUTROUTE;

        auto m = *req->getReq();
        auto command = m[RPREQ_KEY_Command].toString();
        if (command == "ChangeOption") {
            req1_test.eRouteType = E_ROUTETYPE_RE_OPTION;
        }

        // E_SDICOLLECT_ALL -> E_SDICOLLECT_VSM 으로 변경함
        // 19.07.17 ymhong
        req1_test.nSDICollect = E_SDICOLLECT_VSM;  // 경로 재 탐색에는 VSMSDI가 포함 되도록 한다.

        // 아래 코드로 인해 재탐색 시 고속도로 TBT리스트에서 전기차 충전소 아이콘이 표시 안됨.
        // 리뷰 필요. 19.06.26 ymhong.
        //
        // 아래 코드 막을 경우 구간단속 카메라 표출 안 되는 문제 발생하여 다시 원복함.
        // 전기차 충전소 아이콘 문제는 근본적인 원인을 찾아야 할 것으로 보임
        // 2019.6.28 by hheo
        {
            QMutexLocker locker(&m_mapDataLock);
            StringUtil::strlcpy(req1_test.szSDIDataFilePath, m_safeDBPath.toLatin1().constData(),
                                sizeof(req1_test.szSDIDataFilePath));
            StringUtil::strlcpy(req1_test.szSDIIndexFilePath, m_safeDBIndexPath.toLatin1().constData(),
                                sizeof(req1_test.szSDIIndexFilePath));
        }
        qDebug() << "[Perf][RPnSearch][requestReroute][MNS_Route]before";
        QElapsedTimer elapsedTimer;
        elapsedTimer.start();
        MNS_ErrorCode err = MNS_Route(&req1_test, true, &res1_test);
        qDebug() << "[Perf][RPnSearch][requestReroute][MNS_Route]after";
        qDebug() << "MNS_ErrorCode err " << err;
        qDebug() << "MNS_Route(), Reroute : " << elapsedTimer.elapsed() << "ms";
        (*req->getRes())[RPREQ_KEY_ErrorCode] = err;
        if(err == E_ERROR_ROUTE_USERCANCEL)
            break;
    }

    qDebug() << "[Perf][RPnSearch][requestReroute]after";
}

void RPnSearch::cancelRoute(RPRequestPtr& req){
    qDebug() << "[Perf][RPnSearch][cancelRoute][MNS_StopRoute]before";
    MNS_ErrorCode err = MNS_StopRoute();
    qDebug() << "[Perf][RPnSearch][cancelRoute][MNS_StopRoute]after";
    qDebug() << "cancelRoute MNS_ErrorCode err " << err;
    (*req->getRes())[RPREQ_KEY_ErrorCode] = err;
}

bool RPnSearch::canOnlineRequest(RPRequestPtr &req)
{
    return req->getOnline() == RPRequest::RequestServer_Server;
}

bool RPnSearch::isOutOfDistance(RPRequestPtr req)
{
    TS_ROUTE_REQ ts;
    memset(&ts, 0, sizeof(TS_ROUTE_POS));

    auto currentPosition = 0;

    for (auto i=0; i<5; i++) {
        auto index = i+1;
        auto via = req->getVia(index);
        auto x = via->tp.x;
        auto y = via->tp.y;
        if (0 < x && 0 < y) {
            ts.sPosInfo[currentPosition].tp.x = x;
            ts.sPosInfo[currentPosition].tp.y = y;
            ++currentPosition;
        }
    }
    ts.nPosCnt = currentPosition;

    return MNS_IsValidePositionByRouttingEx(&ts, 700);
}

void RPnSearch::setPrimaryNddsDomain(const QString &domain)
{
    mPrimaryNddsDomain = domain;
}

void RPnSearch::requestClearData(){
    memset(&req1_test,0,sizeof(TS_ROUTE_REQ));
    memset(&res1_test,0,sizeof(TS_ROUTE_RES));

    checkRouteRequestDeparture();

    makeRouteRequest_DefaultData();
    makeRouteResponse_DefaultData();
}

void RPnSearch::makeRouteRequest_DefaultData(){
    positionCount = 0;

    req1_test.eRouteType = E_ROUTETYPE_NORMAL;
    req1_test.nRouteOption = E_ROUTEOPTION_OPTIMUM;
    req1_test.nRouteTrafficType = E_ROUTETRAFFICTYPE_PATTERN;
    req1_test.bDirPriority = true;
    req1_test.bNVXInfoCollect = true;

    // sDataTime
    QDate date = QDate::currentDate();
    QTime time = QTime::currentTime();
    req1_test.sDataTime.nYear =  date.year();
    req1_test.sDataTime.nMonth = date.month();
    req1_test.sDataTime.nWeek = date.weekNumber();
    req1_test.sDataTime.nDay = date.day();
    req1_test.sDataTime.nHour = time.hour();
    req1_test.sDataTime.nMinute = time.minute();
    req1_test.sDataTime.nSecond = time.second();

    req1_test.eVehicleClass = E_VEHICLECLASS_1;
    req1_test.eOilType = E_OILTYPE_GASOLINE;
    makeRouteRequest_Departure();
    makeRouteRequest_Via1();
    makeRouteRequest_Via2();
    makeRouteRequest_Via3();
    makeRouteRequest_Destination();
    req1_test.nPosCnt = positionCount+1;
}

void RPnSearch::makeRouteRequest_Option(int option){
    req1_test.nRouteOption = (TeRouteOption)option;
}

void RPnSearch::makeRouteRequest_Departure(){
    req1_test.sPosInfo[0] = routePosition[RpRouteDeparture];
}

void RPnSearch::makeRouteRequest_Via1(){
    if(MakeRouteRequest_CheckPosition(routePosition[RpRouteVia1])){
        req1_test.sPosInfo[++positionCount] = routePosition[RpRouteVia1];
    }
}

void RPnSearch::makeRouteRequest_Via2(){
    if(MakeRouteRequest_CheckPosition(routePosition[RpRouteVia2])){
        req1_test.sPosInfo[++positionCount] = routePosition[RpRouteVia2];
    }
}

void RPnSearch::makeRouteRequest_Via3(){
    if(MakeRouteRequest_CheckPosition(routePosition[RpRouteVia3])){
        req1_test.sPosInfo[++positionCount] = routePosition[RpRouteVia3];
    }
}

void RPnSearch::makeRouteRequest_Destination(){
    if(MakeRouteRequest_CheckPosition(routePosition[RpRouteDestination])){
        req1_test.sPosInfo[++positionCount] = routePosition[RpRouteDestination];
    }
}

void RPnSearch::makeRouteRequest_TvasVersion(const QString &version)
{
    if (version.length()) {
        StringUtil::strlcpy(req1_test.szTvas, version.toStdString().c_str(), sizeof(req1_test.szTvas));
    } else {
        StringUtil::strlcpy(req1_test.szTvas, "5.0", sizeof(req1_test.szTvas));
    }
}

void RPnSearch::makeRouteRequest_Hipass(bool bHipass)
{
    req1_test.bHipass = bHipass;
}

void RPnSearch::makeRouteRequest_AngleAndSpeed(RPRequestPtr &req)
{
    auto m = *req->getReq();
    if (m.contains(RPREQ_KEY_Angle)) {
        req1_test.nCurrentAngle = m[RPREQ_KEY_Angle].toInt();
    }
    if (m.contains(RPREQ_KEY_Speed)) {
        req1_test.nCurrentSpeed = m[RPREQ_KEY_Speed].toInt();
    }
}

void RPnSearch::makeRouteResponse_DefaultData(){
    QString TvasPath = SKT::Environment::getRoutePath();// getDataPath();
    TvasPath.append("/");
    TvasPath.append("tvas_route1.db");
    StringUtil::strlcpy(res1_test.szCollectDataPath, TvasPath.toLatin1(), sizeof(res1_test.szCollectDataPath));
}

void RPnSearch::makeRouteResponse_Data(QString tvasFileName){
    QString TvasPath = SKT::Environment::getRoutePath();
    TvasPath.append("/");
//    TvasPath.append("routes/");
    TvasPath.append(tvasFileName);
    StringUtil::strlcpy(res1_test.szCollectDataPath, TvasPath.toLatin1(), sizeof(res1_test.szCollectDataPath));
}

bool RPnSearch::MakeRouteRequest_CheckPosition(TS_ROUTE_POS &data)
{
    if(data.tp.x==0 && data.tp.y==0)
        return false;
    return true;
}

void RPnSearch::setVia(RpRouteItemType type, RoutePosPtr pos){
    if(type >= RPnSearch::RpRouteDeparture){
        if(pos.isNull() == false)
            routePosition[type] = *pos;
    }
}

RoutePosPtr RPnSearch::getVia(RpRouteItemType type){
    if(type >= RPnSearch::RpRouteDeparture){
        return RoutePosPtr(new TS_ROUTE_POS(routePosition[type]));
    }
}

QMutex RPnSearch::s_mnsMutex;
QMutex RPnSearch::s_nddsMutex;

int RPnSearch::callbackStatus(int nMsg, long lParam1, long lParam2, void *lpParam){
    RPnSearch* pInstance = RPnSearch::sharedInstance();

    QMutexLocker lock(&s_mnsMutex);

    switch(nMsg) {
    case E_MSGTYPE_ENGINE_INIT:
       emit pInstance->Init(lParam1 == 0);
    break;
    case E_MSGTYPE_ROUTE_INIT:

    break;
    case E_MSGTYPE_ROUTE_START:

    break;
    case E_MSGTYPE_ROUTE_END:
        ((TS_ROUTE_RESULT*)lpParam)->eErrorCode;
    break;
    case E_MSGTYPE_ROUTE_DATA:{
        if(pInstance->activeRequest
                && pInstance->activeRequest->getRequestType() == RPRequest::RequestRouteSummary)
        {
            char* pBuffer =(char*)lpParam;
            int nSize = (int)lParam2;
            if (pBuffer == 0x0) break; // fix, segfault to null pointer
            pInstance->tvasParse.parse(pBuffer,nSize);
        }
        break;
    }
    default:
        break;
    }

    return 1;
}

int RPnSearch::callbackNddsStatus(int nMsg, int nSessionID, long lParam1, long lParam2, void *lpParam){
    RPnSearch* pInstance = RPnSearch::sharedInstance();

    QMutexLocker lock(&s_nddsMutex);

    emit pInstance->nddsMessageReceived(nMsg, lParam1, lParam2, lpParam);

    switch(nMsg) {
    case DE_MSGTYPE_NDDS_INIT:
        break;
    case DE_MSGTYPE_ROUTE_INIT:
        break;
    case DE_MSGTYPE_ROUTE_START:
        break;
    case DE_MSGTYPE_ROUTE_END:
        break;
    case DE_MSGTYPE_ROUTE_DATA:
        {
            ((RPnSearchServer*)pInstance)->parseSessionId((const char *)lpParam);
        }
        break;
    case E_MSGTYPE_SEARCH_INIT:     //< 검색 초기화 중(Input 데이터 설정) [ lParam1:검색종류(TeSearchType) ]
        break;
    case E_MSGTYPE_SEARCH_START:	//< 검색 시작	[ lParam1:검색종류(TeSearchType) ]
        break;
    case  E_MSGTYPE_SEARCH_END:     //< 검색 종료
        break;
    case DE_MSGTYPE_FAVORITE_INIT:
        break;
    case DE_MSGTYPE_FAVORITE_START:
        break;
    case DE_MSGTYPE_FAVORITE_END:
        break;
    case DE_MSGTYPE_TRAFFICINFO_INIT:
        break;
    case DE_MSGTYPE_TRAFFICINFO_START:
        break;
    case DE_MSGTYPE_TRAFFICINFO_END:
        break;
    case DE_MSGTYPE_AUTH_INIT:
        break;
    case DE_MSGTYPE_AUTH_START:
        break;
    case DE_MSGTYPE_AUTH_PROGRESS:
        {
            char szErrcode[7]={0,};
            strcpy(szErrcode, (char*)lpParam);
        }
        break;
    case DE_MSGTYPE_AUTH_CHANGE:
        {
            char szAccesskey[50]={0,};
            strcpy(szAccesskey, (char*)lpParam);
            qDebug() << "[Perf][RPnSearch][callbackNddsStatus][NDDS_SetAuthAccessKey]before";
            NDDS_SetAuthAccessKey(szAccesskey);
            qDebug() << "[Perf][RPnSearch][callbackNddsStatus][NDDS_SetAuthAccessKey]after";
        }
        break;
    case DE_MSGTYPE_AUTH_END:
        break;
    case DE_MSGTYPE_NONNDDS_END:
        break;
    default:
        break;
    }

    return 1;
}

void checkdfile(const char* szPath)
{
    if (szPath == NULL || strlen(szPath) <= 0)
        return;

    char szFullPath[512] = {0};
    sprintf(szFullPath, "%s/nddsd", szPath);
    strcat(szFullPath, "file");

    int nResult;
    struct stat stFileInfo;

    if ((nResult = stat(szFullPath, &stFileInfo)) == -1) {
        return;
    }

    if (S_ISREG(stFileInfo.st_mode) == false) {
        return;
    }

    NDDS_SetReqDataSave(true, SKT::Environment::getLogPath().toLatin1().constData());
}

}
