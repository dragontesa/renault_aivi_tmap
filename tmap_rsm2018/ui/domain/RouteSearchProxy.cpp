#include "RouteSearchProxy.h"

#include <QPoint>
#include <QFile>
#include <QtEndian>

#include "RequestCommand.h"
#include "Constants.h"
#include "RouteConstants.h"
#include "RgConstants.h"
#include "ZoneConstants.h"
#include "RpnSearchHelper.h"
#include "PreferenceConstants.h"
#include "../common/rprequest.h"
#include "SearchConstants.h"
#include "SettingConstants.h"
#include "navigationproperty.h"
#include "NavigationPropertyConstants.h"
#include "NavigationPropertyProtocol.h"
#include "ToastsConstants.h"
#include "UtilConstants.h"
#include "rprequest.h"
#include "TRandomUtils.h"
#include "../mns/rpnsearch.h"
#include "../platforms/linux-gen3/interface/server/navigation/navigationservicetype.h"
#include "VSM_Navi.h"
#include "VSM_Util.h"

namespace SKT {

RouteSearchProxy::RouteSearchProxy()
{
    connect(&mRouteManager, SIGNAL(updated(int)), this, SLOT(sendViaInfo()));
    connect(&mRouteManager, SIGNAL(removeMarker(int)), this, SLOT(removeMarker(int)));
    mRerouteByContinue = false;
    mOnline = false;
    mByVr = false;
    mViaSlot = -1;
    mApplyTimer = true;
    mTvasVer = 10; // 5.4
    mRouteOption = 0;
}

void RouteSearchProxy::setGlobalMapView(QObject* mapView)
{
    mGlobalMapView = mapView;
    mGlobalMapViewHandle = qvariant_cast<void*>(mapView->property("nativeHandle"));
}

QObject* RouteSearchProxy::chooseMapView(QObject* mapView) const
{
    return mapView != nullptr ? mapView : mGlobalMapView;
}

void* RouteSearchProxy::chooseMapViewHandle(QObject* mapView) const
{
    return mapView != nullptr ? qvariant_cast<void*>(mapView->property("nativeHandle")) : mGlobalMapViewHandle;
}

void RouteSearchProxy::attachMapView(QObject* mapView)
{
    if (mapView != nullptr) {
        mRouteManager.attachMapView(mapView);
    }
}

void RouteSearchProxy::detachMapView(QObject* mapView)
{
    mRouteManager.detachMapView(mapView);
}

void RouteSearchProxy::requestCommand(const RequestCommandPtr& req)
{
    T_DEBUG( "#1" );

    auto rpnSearch = RpnSearchHelper::instance();
    auto isOnline = isRpServerOnline();

    QVariantMap reqData = req->getReqData();
    if (!reqData.contains(RPREQ_KEY_Online)) {
        reqData[RPREQ_KEY_Online] = isOnline;//mOnline;
    }
    if (!reqData.contains(RPREQ_KEY_Hipass)) {
        reqData[RPREQ_KEY_Hipass] = mHipass;
    }

    reqData[QStringLiteral("TvasVer")] =
        isOnline ? getTvasVersionString(mTvasVer)
                : QStringLiteral("5.4");

    if (isOnline) {
        auto rand = TRandomUtils::randInt(0, 1);
        auto isPrimary = rand == 0;
        reqData[RPREQ_KEY_PrimaryServer] = isPrimary;
    }

    reqData.insert(CommonConstants::RequestCommandPtr,QVariant::fromValue(req));

    QVariantMapPtr reqDataPtr(new QVariantMap(reqData));
    RPRequestPtr reqPtr(new RPRequest(reqDataPtr));
    RPRequestHandle* rpReq = rpnSearch->createRequest(reqPtr);
    connect(rpReq->qobject(), SIGNAL(finished()), this, SLOT(Response()),Qt::QueuedConnection);
    rpnSearch->requestCommand(rpReq);

    auto reqType = reqData["RequestType"].toInt();

    switch (reqType) {
    case RPRequest::RequestRouteSummary:
    case RPRequest::RequestCancel:
        break;
    case RPRequest::RequestRoute:
        qDebug() << "???";
        break;
    case RPRequest::RequestReRouteForce:
    case RPRequest::RequestReRouteByAuto:
    case RPRequest::RequestReRouteByBreakaway:
        sendNotification(RouteConstants::RerouteCommandRequested);
        break;
    }

    debugLog(req);
}

void RouteSearchProxy::setOnline(bool online)
{
    mOnline = online;
}

bool RouteSearchProxy::isOnline() const
{
    return mOnline;
}

void RouteSearchProxy::setChannelPolicyReceived(bool received)
{
    mChannelPolicyReceived = received;
}

bool RouteSearchProxy::isChannelPolicyReceived() const
{
    return mChannelPolicyReceived;
}

void RouteSearchProxy::setHipass(bool hipass)
{
    mHipass = hipass;
}

bool RouteSearchProxy::isRpServerOnline() const
{
    return mOnline && mChannelPolicyReceived;
}

int RouteSearchProxy::addVia(const RouteManager::RouteItem &item)
{
    if (!mRouteManager.checkVia(mViaSlot, item.toPoint())) {
        return -1;
    }

    int slot;

    if (mViaSlot < 0) {
        // 경로 안내 중 맵을 터치하거나 검색 등을 통해 경유지로 선택 시 자동으로 빈 경유지에 등록 되도록 처리
        mRouteManager.relocation();
        RouteManager::RouteItem copy = item;
        copy.status = INSERTED;
        slot = mRouteManager.addVia(copy);

        sendWaypointOperationStatus(copy.status);
    } else {
        // 경유지 설정 화면에서 선택된 슬롯에 등록 되도록 처리
        auto slot2 = static_cast<RouteItemType>(mViaSlot);
        auto prev = mRouteManager.getItem(slot2);
        RouteManager::RouteItem copy = item;
        copy.status = prev.isNull() ? INSERTED : REPLACED;
        mRouteManager.setItem(slot2, copy);
        slot = mViaSlot;

        sendWaypointOperationStatus(copy.status);
    }

    return slot;
}

bool RouteSearchProxy::insertVia(const RouteManager::RouteItem &item)
{
    if (mViaSlot < 0) {
        return false;
    } else {
        RouteManager::RouteItem copy = item;
        copy.status = INSERTED;
        return mRouteManager.insertItem(static_cast<RouteItemType>(mViaSlot), item);
    }
}

void RouteSearchProxy::deleteVia(int begin, int end)
{
    if (begin < RouteItem_Start) {
        return;
    }
    if (end < begin) {
        return;
    }
    if (end <= RouteItem_End) {
        RouteManager::RouteItem empty;
        for ( auto i=begin; i<=end; i++ ) {
            empty.status = DELETED;
            mRouteManager.setItem(static_cast<RouteItemType>(i), empty);
        }
    }
}

bool RouteSearchProxy::hasEmptyVia() const
{
    return mRouteManager.hasEmptyVia();
}

void RouteSearchProxy::clearVia()
{
    mStartSearchCode = RouteConstants::EXPLORER_GPS_START;
    mRouteManager.clear();//status checked
}

int RouteSearchProxy::viaSlot() const
{
    return mViaSlot;
}

void RouteSearchProxy::setViaSlot(int slot)
{
    mViaSlot = slot;
}

void RouteSearchProxy::setStartSearchCode(int code)
{
    mStartSearchCode = code;
}

void RouteSearchProxy::moveDown(QObject* mapView, int index)
{
    int another = mRouteManager.moveDown(index);//status checked (RouteManager의 swap에서 처리)
    swapViaMarker(chooseMapView(mapView), index, another);
}

void RouteSearchProxy::moveUp(QObject* mapView, int index)
{
    int another = mRouteManager.moveUp(index);//status checked (RouteManager의 swap에서 처리)
    swapViaMarker(chooseMapView(mapView), index, another);
}

void RouteSearchProxy::removeVia(QObject* mapView, int type)
{
    mRouteManager.remove(type);//status checked

    RouteManager::sharedInstance()->removeViaMarker(chooseMapView(mapView), type);
}

void RouteSearchProxy::skipNextVia(QObject* mapView)
{
    for(int c = RouteItem_Via1 ; c <= RouteItem_Via3 ; c++ ){
        QPoint pos = viaWorldPos(c);
        if( pos.x() == 0 && pos.y() == 0 ) continue;
        removeVia(mapView, c);//status checked
        break;
    }

    // TMCRSM-3394. 경유지 통과 후 새로운 경유지 정보를 DB에 갱신해야 함
    // 19.07.04 ymhong.
    auto vl = mRouteManager.getViaStringList();
    saveRgPreference(RgConstants::Termination, RgConstants::Via, vl);

    // 경유지를 지나간 후 최근 목적지 > 상세 > 경유지로 > 뒤로가기
    // 를 하면 지나간 경유지가 부활함
    // 이를 방지하기 위해 경유지 통과로 경유지가 삭제 된 경우 백업 리스트를 동기화 함
    // 19.10.17 ymhong
    backupVia();
}

void RouteSearchProxy::fillViaInfo(QVariantMap &m, bool skipDeparture)
{
    int viaId = 1;
    auto *codec = QTextCodec::codecForName("EUC-KR");
    bool startPosReplaced = false;
    int destSearchCode = 0xff;

    for (int i=0; i<RouteItem_Num; ++i) {
        auto item = mRouteManager.getItem(static_cast<RouteItemType>(i));
        // 출발지가 자동으로 선택된 것이 아닌 사용자 선택에 의해 교체되었는지 체크(NDDS 사양)
        if (0 == i) {
            startPosReplaced = item.status == REPLACED;
            m[RPREQ_KEY_DepartureReplaced] = startPosReplaced;
        }
        if ( 0 < i || !skipDeparture) {
            TS_ROUTE_POS *pos = new TS_ROUTE_POS();
            memset(pos, 0, sizeof(TS_ROUTE_POS));

            auto pt = item.toPoint();
            if (0 == i && pt.isNull()) {
                pt = mCurrentPosition;
            }
            pos->tp.x = pt.x();
            pos->tp.y = pt.y();
            pos->nPoiID = item.poiId;
            pos->byRPFlag = item.rpFlag;

            if (!item.name.isEmpty()) {
                auto euckr = codec->fromUnicode(item.name);
                qstrncpy(pos->szName, euckr.data(), TD_POSNAME_LEN);
            } else {
                auto euckr = codec->fromUnicode(item.address);
                qstrncpy(pos->szName, euckr.data(), TD_POSNAME_LEN);
            }

            QString via = QString("Via%1").arg(viaId);
            m.insert(via, QVariant::fromValue(RoutePosPtr(pos)));
        }
        if (item.searchFlag != 0xff) {
            destSearchCode = item.searchFlag;
        }
        ++viaId;
    }

    // 출발지 탐색 구분 코드
    if (startPosReplaced) {
        m[RPREQ_KEY_StartSearchCode] = mStartSearchCode;
    } else {
        m[RPREQ_KEY_StartSearchCode] = RouteConstants::EXPLORER_GPS_START;
    }

    if (!m.contains(RPREQ_KEY_DestSearchCode)) {
        m[RPREQ_KEY_DestSearchCode] = destSearchCode;
    }
}

void RouteSearchProxy::fillDepartureInfo(QVariantMap &m, bool reroute)
{
    auto item = mRouteManager.getItem(RouteItem_Start);
    if (item.status != REPLACED || reroute) {
        VSM_Int32 x;
        VSM_Int32 y;
        if (VSMNavi_GetCurrentPosition(x, y)) {
            TS_ROUTE_POS *pos = new TS_ROUTE_POS();
            memset(pos, 0, sizeof(TS_ROUTE_POS));
            pos->tp.x = x;
            pos->tp.y = y;
            m["Via1"] = QVariant::fromValue(RoutePosPtr(pos));
        }
    }
}

void RouteSearchProxy::setVia(int type, const RouteManager::RouteItem &item)
{
    auto slot = static_cast<RouteItemType>(type);
    auto copy = item;
    if (copy.isNull()) {
        copy.status = DELETED;
    } else {
        auto prev = mRouteManager.getItem(slot);
        copy.status = prev.isNull() ? INSERTED : REPLACED;
    }
    mRouteManager.setItem(slot, copy);
}

void RouteSearchProxy::setViaAddress(int type, const QString &address)
{
    // empty
}

void RouteSearchProxy::replaceVia(int type, int x, int y)
{
    RouteManager::RouteItem item;
    item.wx = x;
    item.wy = y;
    item.status = REPLACED;
    mRouteManager.setItem(static_cast<RouteItemType>(type), item);
}

QString RouteSearchProxy::viaAddress(int type) const
{
    Q_ASSERT(0 <= type && type <5);

    switch (type) {
    case 0:
        return mRouteManager.startName();
    case 1:
    case 2:
    case 3:
        return mRouteManager.viaName(type-1);
    case 4:
        return mRouteManager.endName();

    default:
        return QString();
    }
}

QPoint RouteSearchProxy::viaWorldPos(int type) const
{
    return mRouteManager.worldPos(static_cast<RouteItemType>(type));
}

QString RouteSearchProxy::targetAddress() const
{
    return mRouteManager.targetAddress();
}

QPoint RouteSearchProxy::targetPos() const
{
    return mRouteManager.targetWorldPos();
}

RouteManager::RouteItem RouteSearchProxy::targetItem() const
{
    return mRouteManager.targetItem();
}

void RouteSearchProxy::showVia(QObject* mapView)
{
    for (int i=0; i<5; i++) {
        QPoint pos = mRouteManager.worldPos(static_cast<RouteItemType>(i));

        if (!pos.isNull()) {
            RouteManager::sharedInstance()->addViaMarker(chooseMapView(mapView), i, pos);
        }
    }
}

void RouteSearchProxy::hideVia(QObject* mapView)
{
    RouteManager::sharedInstance()->clearViaMarker(chooseMapView(mapView));
}

void RouteSearchProxy::backupVia()
{
    mRouteManager.backup();
}

void RouteSearchProxy::restoreVia()
{
    mRouteManager.restore();
}

void RouteSearchProxy::Response()
{
    RPRequestHandle* req = qobject_cast<RPRequestHandle*>(sender());
    if (!req) {
        return;
    }

    disconnect(req->qobject(), SIGNAL(finished()), this, SLOT(Response()));
    req->qobject()->deleteLater();

    RPRequestPtr rpReq = req->request();

    QVariantMapPtr resDataPrt = rpReq->getRes();
    QVariantMap& resData = *(resDataPrt.data());
    if(resData["Answer"].toString() != "Later")
    {
        QVariantMap &reqData = *rpReq->getReq();
        RequestCommandPtr req = reqData[CommonConstants::RequestCommandPtr].value<RequestCommandPtr>();
        req->setResData(resData);
        onRequestRouteFinished( req );
    }
}

void RouteSearchProxy::sendViaInfo()
{
    QVariantMap m;
    for (int i=0; i<5; ++i) {
        auto addr = viaAddress(i);
        auto pos = viaWorldPos(i);

        m[QString("Via%1").arg(i+1)] = addr;//viaAddress(i);
        m[QString("Via%1Addr").arg(i+1)] = pos;//viaWorldPos(i);
    }

    sendNotification(RouteConstants::UpdateViaInfo, m);
}

void RouteSearchProxy::removeMarker(int index)
{
    RouteManager::sharedInstance()->removeViaMarker(chooseMapView(nullptr), index);
}

void RouteSearchProxy::onRequestRouteFinished( const RequestCommandPtr& req)
{
    auto note = t_new_shared<mvc::TNotification>( this, RouteConstants::UpdateRoute,
        QVariant::fromValue( req ) );
    req->notifyObserver( note );
}

void RouteSearchProxy::swapViaMarker(QObject* mapView, int index0, int index1)
{
    QPoint pt;

    pt = mRouteManager.worldPos(static_cast<RouteItemType>(index0));
    RouteManager::sharedInstance()->removeViaMarker(chooseMapView(mapView), index0);
    RouteManager::sharedInstance()->addViaMarker(chooseMapView(mapView), index0, pt);

    pt = mRouteManager.worldPos(static_cast<RouteItemType>(index1));
    RouteManager::sharedInstance()->removeViaMarker(chooseMapView(mapView), index1);
    RouteManager::sharedInstance()->addViaMarker(chooseMapView(mapView), index1, pt);
}

void printVia(int index, QVariant v)
{
    int x = 0;
    int y = 0;
    int rpFlag = 0;
    if (v.isValid()) {
        auto via = v.value<RoutePosPtr>();
        x = via->tp.x;
        y = via->tp.y;
        rpFlag = via->byRPFlag;
    }

    // index가 0인 경우 Via1이지만 현재 GPS좌표로 요청한다.
    if (index == 0) {
        VSMNavi_GetCurrentPosition(x, y);

        index = 1;
    }

    if (x && y) {
#if (0)
        VSMPoint2i world{x, y};
        VSMPoint2f bessel;

        VSMMap_Util_ConvertWorldTo(world, VSM_COORDINATE_WGS84, bessel);

        QString bx; bx.setNum(bessel.x, 'g', 12);
        QString by; by.setNum(bessel.y, 'g', 12);
        auto str = by + ", " + bx;
        qDebug() << "[RpSearch]" << QString("Via%1").arg(index) << x << "," << y << " -> " << str << ", " << rpFlag;
#else
        qDebug() << "[RpSearch]" << QString("Via%1").arg(index) << x << "," << y;
#endif
    }
}

static QString departSearchCodeStr(int departSearchCode) {
    switch (departSearchCode) {
    case RouteConstants::EXPLORER_GPS_START:
        return "EXPLORER_GPS_START";
    case RouteConstants::EXPLORER_RECENT_START:
        return "EXPLORER_RECENT_START";
    case RouteConstants::EXPLORER_BOOKMARK_START:
        return "EXPLORER_BOOKMARK_START";
    case RouteConstants::EXPLORER_MAP_START:
        return "EXPLORER_MAP_START";
    case RouteConstants::EXPLORER_NAME_START:
        return "EXPLORER_NAME_START";
    case RouteConstants::EXPLORER_ADDRESS_START:
        return "EXPLORER_ADDRESS_START";
    default:
        return "";
    }
}

static QString destSearchCodeStr(int destSearchCode) {

    switch (destSearchCode) {
    case RouteConstants::EXPLORER_FIRST_DES:
        return "EXPLORER_FIRST_DES";
    case RouteConstants::EXPLORER_RE_DES:
        return "EXPLORER_RE_DES";
    case RouteConstants::EXPLORER_RE_USER_DES:
        return "EXPLORER_RE_USER_DES";
    case RouteConstants::EXPLORER_ROUNDINFO_DES:
        return "EXPLORER_ROUNDINFO_DES";
    case RouteConstants::EXPLORER_RECENT_DES:
        return "EXPLORER_RECENT_DES";
    case RouteConstants::EXPLORER_BOOKMARK_DES:
        return "EXPLORER_BOOKMARK_DES";
    case RouteConstants::EXPLORER_LONGITUDE_N_LATITUDE_DES:
        return "EXPLORER_LONGITUDE_N_LATITUDE_DES";
    case RouteConstants::EXPLORER_NAME_DES:
        return "EXPLORER_NAME_DES";
    case RouteConstants::EXPLORER_ADDRESS_DES:
        return "EXPLORER_ADDRESS_DES";
    case RouteConstants::EXPLORER_LEISURE_N_LIFE_DES:
        return "EXPLORER_LEISURE_N_LIFE_DES";
    case RouteConstants::EXPLORER_BOOKED_ROAD_DES:
        return "EXPLORER_BOOKED_ROAD_DES";
    case RouteConstants::EXPLORER_LOCAL_BUSINESS_TYPE_DES:
        return "EXPLORER_LOCAL_BUSINESS_TYPE_DES";
    case RouteConstants::EXPLORER_TELEPHONE_NUM_DES:
        return "EXPLORER_TELEPHONE_NUM_DES";
    case RouteConstants::EXPLORER_LOCAL_NAME_DES:
        return "EXPLORER_LOCAL_NAME_DES";
    case RouteConstants::EXPLORER_VOICE_ROAD_GUIDE_DES:
        return "[DEPRECATED] EXPLORER_VOICE_ROAD_GUIDE_DES";
    case RouteConstants::EXPLORER_AFTER_MOVING_MAP_DES:
        return "EXPLORER_AFTER_MOVING_MAP_DES";
    case RouteConstants::EXPLORER_MY_POSTION_DES:
        return "EXPLORER_MY_POSTION_DES";
    case RouteConstants::EXPLORER_RECEIVE_POSTION_DES:
        return "EXPLORER_RECEIVE_POSTION_DES";
    case RouteConstants::EXPLORER_SEND_POSTION_DES:
        return "EXPLORER_SEND_POSTION_DES";
    case RouteConstants::EXPLORER_OTHER_DES:
        return "[DEPRECATED] EXPLORER_OTHER_DES";
    case RouteConstants::EXPLORER_REOTHER_DES:
        return "[DEPRECATED] EXPLORER_REOTHER_DES";
    case RouteConstants::EXPLORER_REOTHER_USER_DES:
        return "[DEPRECATED] EXPLORER_REOTHER_USER_DES";
    case RouteConstants::EXPLORER_NEW_ADDRESS_DES:
        return "EXPLORER_NEW_ADDRESS_DES";
    case RouteConstants::EXPLORER_MOTEL_N_RESTURANT_DES:
        return "EXPLORER_MOTEL_N_RESTURANT_DES";
    case RouteConstants::EXPLORER_REROUTE_PERIOD_OUTSIDE_DES:
        return "EXPLORER_REROUTE_PERIOD_OUTSIDE_DES";
    case RouteConstants::EXPLORER_REROUTE_PERIOD_INSIDE_DES:
        return "EXPLORER_REROUTE_PERIOD_INSIDE_DES";
    case RouteConstants::EXPLORER_TSEARCH_DES:
        return "EXPLORER_TSEARCH_DES";
    case RouteConstants::EXPLORER_MPOST_DES:
        return "EXPLORER_MPOST_DES";
    case RouteConstants::EXPLORER_CHANGE_ROUTE_OPTION_DES:
        return "EXPLORER_CHANGE_ROUTE_OPTION_DES";
    case RouteConstants::EXPLORER_NEAR_LINK_RE_DES:
        return "EXPLORER_NEAR_LINK_RE_DES";
    case RouteConstants::EXPLORER_ROUTE_BOOKMARK_DES:
        return "EXPLORER_ROUTE_BOOKMARK_DES";
    case RouteConstants::EXPLORER_HOME_DES:
        return "EXPLORER_HOME_DES";
    case RouteConstants::EXPLORER_OFFICE_DES:
        return "EXPLORER_OFFICE_DES";
    case RouteConstants::EXPLORER_ROUTE_FREQUENT_DES:
        return "EXPLORER_ROUTE_FREQUENT_DES";
    case RouteConstants::EXPLORER_FORCED_RE_SEARCH_DES:
        return "EXPLORER_FORCED_RE_SEARCH_DES";
    default:
        return "";
    }
}

static QString getViaString(int index, int wx, int wy)
{
    VSMPoint2i world{wx, wy};
    VSMPoint2f bessel;

    VSMMap_Util_ConvertWorldTo(world, VSM_COORDINATE_WGS84, bessel);

    QString bx; bx.setNum(bessel.x, 'g', 10);
    QString by; by.setNum(bessel.y, 'g', 10);

    return QString("%1, %2").arg(bx).arg(by);
}

static QString getViaString(int index, const QVariant& v)
{
    if (v.isValid()) {
        auto via = v.value<RoutePosPtr>();
        auto x = via->tp.x;
        auto y = via->tp.y;
        if (0<x && 0<y) {
            return getViaString(index, x, y);
        }
    }
    return QString{};
}

static QString getDepartureString()
{
    VSM_Int32 x, y;

    VSMNavi_GetCurrentPosition(x, y);

    return getViaString(1, x, y);
}

void RouteSearchProxy::debugLog(const RequestCommandPtr& req)
{
    QString debugText; //MMFB 화면에서 보기 위한 용도.

    static QString searchTypeStr[] = {
        "RequestCancel",
        "RequestRoute",
        "RequestRouteSummary",
        "RequestReRouteForce",
        "RequestReRouteByAuto",
        "RequestReRouteByBreakaway"
    };
    auto m = req->getReqData();
    auto reqType = m["RequestType"].toInt();
    auto via1 = m["Via1"];
    auto via2 = m["Via2"];
    auto via3 = m["Via3"];
    auto via4 = m["Via4"];
    auto via5 = m["Via5"];
    auto searchType = m["RouteSearch"].toString();
    auto startSearchCode = m[RPREQ_KEY_StartSearchCode].toInt();
    auto destSearchCode = m[RPREQ_KEY_DestSearchCode].toInt();
    auto angle = m[RPREQ_KEY_Angle].toInt();
    auto speed = m[RPREQ_KEY_Speed].toInt();
    auto isOnline = m[RPREQ_KEY_Online].toBool();

    if (mUpdateRpDebugText) {
        auto isReroute = (reqType == RPRequest::RequestReRouteByAuto) ||
                         (reqType == RPRequest::RequestReRouteByBreakaway) ||
                         (reqType == RPRequest::RequestReRouteForce);

        debugText += QString("%1, ").arg(angle);
        if (isReroute) {
            debugText += getDepartureString();
        } else {
            debugText += getViaString(1, via1);
        }
        if (m.contains(RPREQ_KEY_LastTollgateId)) {
            auto lastTollgateId = m[RPREQ_KEY_LastTollgateId].toInt();
            debugText += "\nLastTollID:";
            debugText += QString::number(lastTollgateId);
        }
        sendNotification(RouteConstants::UpdateRequestRouteDebugText, debugText);
    }

    qDebug() << "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@";
    qDebug() << "[RP]" << "RequestType :" << searchTypeStr[reqType];
    qDebug() << "[RP]" << "isOnline :" << isOnline;
    qDebug() << "[RP]" << "searchType :" << searchType;
    qDebug() << "[RP]" << "startSearchCode :" << departSearchCodeStr(startSearchCode);
    qDebug() << "[RP]" << "destSearchCode :" << destSearchCodeStr(destSearchCode);
    qDebug() << "[RP]" << "angle :" << angle;
    qDebug() << "[RP]" << "speed :" << speed;
    switch (reqType) {
    case RPRequest::RequestCancel:
    case RPRequest::RequestNone:
        break;

    case RPRequest::RequestRoute:
    case RPRequest::RequestRouteSummary:
    case RPRequest::RequestReRouteForce:
    case RPRequest::RequestReRouteByAuto:
    case RPRequest::RequestReRouteByBreakaway:
        if (startSearchCode == RouteConstants::EXPLORER_GPS_START) {
            printVia(0, via1);
        } else {
            printVia(1, via1);
        }
        printVia(2, via2);
        printVia(3, via3);
        printVia(4, via4);
        printVia(5, via5);
        break;
    }
    qDebug() << "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@";
}

QString RouteSearchProxy::getTvasVersionString(int ver) const
{
    switch (ver) {
    case 6: return "5.0";
    case 7: return "5.1";
    case 8: return "5.2";
    case 9: return "5.3";
    case 10:return "5.4";
    default:
        return "5.4";
    }
}

void RouteSearchProxy::saveRgPreference(const QString &section, const QString &key, const QVariant &value)
{
    sendNotification(PreferenceConstants::SavePreference, QVariantMap{
         {"preferencesName",PreferenceConstants::Rg},
         {CommonConstants::Section, section},
         {CommonConstants::Key, key},
         {CommonConstants::Value, value}
     });
}

void RouteSearchProxy::addSearchMarker(QObject* mapView, int id, int focusIdx, const QString& text, int x, int y)
{
    RouteManager::sharedInstance()->addSearchMarker(chooseMapView(mapView), id, focusIdx, text, x, y);
}

void RouteSearchProxy::removeSearchMarker(QObject* mapView, int id, int x, int y)
{
    RouteManager::sharedInstance()->removeSearchMarker(chooseMapView(mapView), id,  x, y);
}

void RouteSearchProxy::updatePoi()
{
    auto poi = mRouteManager.targetAddress();

    sendNotification(ZoneConstants::UpdatePoi, poi);
}

bool RouteSearchProxy::hasVia() const
{
    return mRouteManager.hasVia();
}

bool RouteSearchProxy::hasVia(int index) const
{
    return mRouteManager.hasVia(index);
}

void RouteSearchProxy::setRerouteByContinue(bool byContinue)
{
    mRerouteByContinue = byContinue;
}

void RouteSearchProxy::setRouteOption(int routeOption)
{
    if (mRouteOption != routeOption) {
        mRouteOption = routeOption;

        QVariantMap m;
        m["propertyValue"] = routeOption;
        m["type"] = NavigationPropertyProtocol::TmapRouteOptionChanged;

        sendNotification(NavigationPropertyConstants::setNavigationProperty, m);
    }
}

bool RouteSearchProxy::isRerouteByContinue() const
{
    return mRerouteByContinue;
}

int RouteSearchProxy::routeOption() const
{
    return mRouteOption;
}

void RouteSearchProxy::setTvasValid(bool ok)
{
    mIsValidTvas = ok;
}

bool RouteSearchProxy::isTvasValid() const
{
    return mIsValidTvas;
}

void RouteSearchProxy::rerouteByOption(int option)
{
    QList<TeRouteOption> options;
    options << TeRouteOption(option);

    QVariantMap m;
    QStringList *filenames = new QStringList();
    QList<TeRouteOption> *routeOption = new QList<TeRouteOption>();

    filenames->append(QStringLiteral("tvas_route.db"));
    routeOption->append(TeRouteOption(option));

    auto isOnline = isRpServerOnline();

    m.insert(RPREQ_KEY_TvasFileName, QVariant::fromValue(QStringListPtr(filenames)));
    m.insert(RPREQ_KEY_RPOptions, QVariant::fromValue(QRouteOptionListPtr(routeOption)));
    m.insert(RPREQ_KEY_Command, "ChangeOption");
    m.insert(RPREQ_KEY_DestSearchCode, RouteConstants::EXPLORER_CHANGE_ROUTE_OPTION_DES);
    m.insert(RPREQ_KEY_Online, isOnline);//mOnline);

    // 현재 위치를 기준으로 경로 탐색을 위해서는 경로 타입이 Reroute이어야 함.
    // 그렇지 않으면 현재 위치를 고려하지 않고 처음 설정한 경로와 경유지로 경로 탐색을 하게 되어
    // 화면에 파란색 주행선이 표시되지 않음.
    // 17.07.26 ymhong.
    m.insert(RPREQ_KEY_RequestType, RPRequest::RequestReRouteForce);

    sendNotification(SearchConstants::RequestCommand, m);

    setRouteOption(option);
    showRerouteToast(option);
    sendNotification(UtilConstants::ShowProgressIndicator, QVariantMap{{"reroute", true}});
}

void RouteSearchProxy::showRerouteToast(int option)
{
    QString message;

    switch (option) {
    case E_ROUTEOPTION_OPTIMUM: // 최적
        //message = QObject::tr("T map 최적경로로 경로를 재탐색 합니다.");
        message = "T map 최적경로로 경로를 재탐색 합니다.";
        break;
    case E_ROUTEOPTION_SHORTTIME: // 최소
        message= QObject::tr("최소시간으로 경로를 재탐색 합니다.");
        break;
    case E_ROUTEOPTION_FREEWAY: // 무료
        message= QObject::tr("무료도로로 경로를 재탐색 합니다.");
        break;
    case E_ROUTEOPTION_SHORTESTWAY: // 최단
        message= QObject::tr("최단경로로 경로를 재탐색 합니다.");
        break;
    case E_ROUTEOPTION_EXPRESSWAY: // 고속도로 우선
        message= QObject::tr("고속도로 우선으로 경로를 재탐색 합니다.");
        break;
    case E_ROUTEOPTION_EASY: // 초보자
        message= QObject::tr("초보자 경로로 경로를 재탐색 합니다.");
        break;

    default:
        break;
    }

    QVariantMap m;
    m[CommonConstants::Position] = ToastsConstants::ToastUp;
    m[CommonConstants::Message] = message;
    sendNotification(ToastsConstants::ShowToast, m);
}

bool RouteSearchProxy::isOfflineOption(int option) const
{
    switch (option) {
    case E_ROUTEOPTION_OPTIMUM:
    case E_ROUTEOPTION_SHORTESTWAY:
    case E_ROUTEOPTION_FREEWAY:
        return true;

    default:
        return false;
    }
}

// 주어진 경유지 리스트와 옵션으로 경로 안내를 요청함.
void RouteSearchProxy::requestRoute(QRoutePosListPtr wayPoints, int option, bool apply)
{
    if (wayPoints->size() == 0) {
        return;
    } else if (wayPoints->size() == 1) {
        sendNotification(RouteConstants::SetDeparture, QVariantMap{{"clear", true}});
        auto pos = wayPoints->at(0);
        sendNotification(RouteConstants::SetArrival, QVariantMap{
                             {CommonConstants::X, pos.tp.x},
                             {CommonConstants::Y, pos.tp.y}
                         });
    } else {
        sendNotification(RouteConstants::SetVia, QVariantMap{
                             {"points", QVariant::fromValue(wayPoints)}
                         });
    }

    QList<TeRouteOption> options;
    options << TeRouteOption(option);

    QVariantMap m;

    if (apply) {
        QStringList *filenames = new QStringList();
        QList<TeRouteOption> *routeOption = new QList<TeRouteOption>();

        filenames->append(QStringLiteral("tvas_route.db"));
        routeOption->append(TeRouteOption(option));

        auto isOnline = isRpServerOnline();

        m.insert(RPREQ_KEY_TvasFileName, QVariant::fromValue(QStringListPtr(filenames)));
        m.insert(RPREQ_KEY_RPOptions, QVariant::fromValue(QRouteOptionListPtr(routeOption)));
        m.insert(RPREQ_KEY_Command, tr("경로 요청"));
        m.insert(RPREQ_KEY_RequestType, RPRequest::RequestRoute);
        m.insert(RPREQ_KEY_Apply, apply);
        m.insert(RPREQ_KEY_Online, isOnline);
        sendNotification(SearchConstants::RequestCommand, m);
    } else {
        m["routeOption"] = option;
        m["apply"] = true;
        sendNotification(RgConstants::StartRouteGuidanceByVr, m);
    }

    // 경로 요약 화면에서 경유지 편집 화면으로 이동 후 다시 경로 탐색을 했을 때 처리는?
    sendWaypointProperties();
}

bool RouteSearchProxy::byVr() const
{
    return mByVr;
}

void RouteSearchProxy::setByVr(bool byVr)
{
    mByVr = byVr;
}

bool RouteSearchProxy::applyTimer() const
{
    return mApplyTimer;
}

void RouteSearchProxy::setApplyTimer(bool apply)
{
    mApplyTimer = apply;
}

void RouteSearchProxy::updateViaCurrentPosition()
{
    auto item = mRouteManager.getItem(RouteItem_Start);
    item.wx = mCurrentPosition.x();
    item.wy = mCurrentPosition.y();
    item.address = mCurrentAddress;
    mRouteManager.setItem(RouteItem_Start, item);
}

void RouteSearchProxy::setCurrentPosition(int x, int y)
{
    mCurrentPosition = QPoint { x, y };
}

void RouteSearchProxy::setCurrentAddress(const QString& address)
{
    mCurrentAddress = address;
}

void RouteSearchProxy::setLastUserSelectPosition(int x, int y, const QString &address)
{
    mLastUserSelectPosition = QPoint(x, y);
    mLastUserSelectAddress = address;
    if (mLastUserSelectPosition.isNull()) {
        mLastUserSelectAddress.clear();
    }
}

void RouteSearchProxy::insertWaypoint(int slot, const QPoint &pt)
{
    if (canInsert(slot)) {
        mRouteManager.backup();
        RouteManager::RouteItem item;
        item.wx = pt.x();
        item.wy = pt.y();
        setViaSlot(slot);
        insertVia(item);

        auto result = mRouteManager.checkDepartureAndViaDistance(mCurrentPosition);
        if (CheckViaResultOk == result) {
            sendWaypointProperties();
        } else {
            mRouteManager.restore();
        }
    } else {
        sendNotification(ToastsConstants::ShowToast, QVariantMap{
                             {CommonConstants::Position, ToastsConstants::ToastUp},
                             {CommonConstants::Message, tr("더 이상 경유지 추가가 불가능합니다.")}
                         });
    }
}

void RouteSearchProxy::insertWaypointFromSelectInMap(int slot)
{
    if (!mLastUserSelectPosition.isNull()) {
        RouteManager::RouteItem item;
        item.wx = mLastUserSelectPosition.x();
        item.wy = mLastUserSelectPosition.y();
        item.address = mLastUserSelectAddress;
        // TMCRSM-2675.
        // The insertWaypoint interface is removed  and replaced with insertWaypointAtDefaultPosition .
        // When  insertWaypoint was used , SDS will  always send parameter ‘newIndex’ as 1
        // for  adding waypoint as first entry to the list.
        // The new interface ‘InsertWaypointAtDefaultPosition ‘  is not having  any parameter and
        // expecting navi to add the current location to waypoint list .
        // In Renault , current location should add as first entry in the list

        if (canInsert(slot)) {
            mRouteManager.backup();
            setViaSlot(slot);
            insertVia(item);
            if (mRouteManager.checkDepartureAndViaDistance(mCurrentPosition)) {
                sendWaypointProperties();
            } else {
                mRouteManager.restore();
            }
        } else {
            sendNotification(ToastsConstants::ShowToast, QVariantMap{
                                 {CommonConstants::Position, ToastsConstants::ToastUp},
                                 {CommonConstants::Message, tr("더 이상 경유지 추가가 불가능합니다.")}
                             });
        }
    }
}

bool RouteSearchProxy::canInsert(int slot) const
{
    return mRouteManager.canInsert(slot);
}

/**
 * set Tvas version
 * this called  from ManageTestCommand when it got changed
 */
void RouteSearchProxy::setTvasVersion(int ver)
{
    mTvasVer = ver;
}

CheckViaResult RouteSearchProxy::checkViaIsNear()
{
    return mRouteManager.checkDepartureAndViaDistance(mCurrentPosition);
}

bool RouteSearchProxy::isArrivalBelowDeparture() const
{
    auto departure = viaWorldPos(RouteItem_Start);
    auto arrival = targetPos();

    return arrival.y() < departure.y();
}

void RouteSearchProxy::relocationViaInfo()
{
    mRouteManager.relocation();
}

void RouteSearchProxy::fillRerouteData(QVariantMap &m)
{
    VSM_RerouteData rerouteData;
    memset(&rerouteData, 0, sizeof(VSM_RerouteData));

    VSM_Bool bAroundLink;
    auto result = VSMNavi_GetTvasReRouteData(rerouteData, bAroundLink);
    if (result == VSM_TRUE) {
        //마지막 톨게이트 ID
        auto lastTollgateId = rerouteData.uPassedTollID;
        m[RPREQ_KEY_LastTollgateId] = lastTollgateId;

        //이전경로 톨게이트 ID데이터
        QString preTollgateIds = QString::fromLatin1(rerouteData.tollIdList);
        m[RPREQ_KEY_PreTollgateIds] = preTollgateIds;

        // 경로주변링크 정보 유무
        auto radiusInfoType = (bAroundLink == VSM_TRUE);
        m[RPREQ_KEY_RadiusInfoType] = radiusInfoType;

        // 경로주변링크 진입 RSD 회전 코드
        auto radiusEntryTurnType = rerouteData.uRsdCode;
        m[RPREQ_KEY_RadiusEntryTurnType] = radiusEntryTurnType;

        // 경로 주변 링크ID
        auto radiusLinkId = rerouteData.uLinkID;
        m[RPREQ_KEY_RadiusLinkId] = radiusLinkId;

        // 경로주변 타일코드
        auto radiusMeshCode = rerouteData.uTileCode;
        m[RPREQ_KEY_RadiusMeshCode] = radiusMeshCode;

        // 경로주변 링크방향
        auto radiusLinkDirection = rerouteData.uDirInfo;
        m[RPREQ_KEY_RadiusLinkDirection] = radiusLinkDirection;

        // 이탈지점 기존경로의 RSD회전코드
        auto radiusLeaveTurnType = rerouteData.uOldRsdCode;
        m[RPREQ_KEY_RadiusLeaveTurnType] = radiusLeaveTurnType;

        // 경로주변링크 해당보관점 X좌표
        auto radiusXPos = rerouteData.uPosX;
        m[RPREQ_KEY_RadiusXPos] = radiusXPos;

        // 경로주변링크 해당보간점 Y좌표
        auto radiusYPos = rerouteData.uPosY;
        m[RPREQ_KEY_RadiusYPos] = radiusYPos;

        auto rd4Path = QString::fromLocal8Bit(rerouteData.szRouteDataPath);
        if (rd4Path.size()) {
            m[RPREQ_KEY_RD4_Filename] = rd4Path;
        }
    }

    VSM_RerouteRIDData rerouteRidData;
    memset(&rerouteRidData, 0, sizeof(VSM_RerouteRIDData));

    result = VSMNavi_GetTvasReRouteRIDData(rerouteRidData);
    if (VSM_TRUE == result) {
        // 마지막 매칭 RID
        m[RPREQ_KEY_LastRid] = rerouteRidData.lastRid;

        // 이전경로 맵버전
        char mapVersion[8+1];
        strncpy(mapVersion, rerouteRidData.preMapVersion, 8);
        mapVersion[8] = '\0';
        m[RPREQ_KEY_PreMapVersion] = mapVersion;// QString::fromLatin1(rerouteRidData.preMapVersion);

        // 이전경로 RID목록
        QVariantList reRids;
        for (auto reRid : rerouteRidData.reRids) {
            reRids.push_back(reRid);
        }
        m[RPREQ_KEY_ReRids] = reRids;

        // 이전경로 RID의 소요시간 목록
        QVariantList preSecs;
        for (auto preSec : rerouteRidData.preSecs) {
            preSecs.push_back(preSec);
        }
        m[RPREQ_KEY_PreSecs] = preSecs;

        // 초기탐색 직선거리
        m[RPREQ_KEY_InitSrchLength] = rerouteRidData.initSrchLength;

        // 초기탐색 SessionId
        char buf[24+1];
        memset(buf, 0, 24+1);
        strncpy(buf, rerouteRidData.initSrchSessionId, 24);
        QString sessionId = buf;

        m[RPREQ_KEY_InitSrchSessionId] = sessionId;//rerouteRidData.initSrchSessionId);
    }
}

void RouteSearchProxy::fillLocalRerouteData(QVariantMap &m)
{
    VSM_Int32 lastTollId;
    auto result = VSMNavi_GetLocalReRouteTollID(lastTollId);
    if (VSM_TRUE == result) {
        m[RPREQ_KEY_LastTollgateId] = lastTollId;
    }
}

//#define TEST_GPS_TRACE_DATA

#ifdef TEST_GPS_TRACE_DATA
//#define TEST_GPS_TRACE_DATA_ENCODING

#ifndef TEST_GPS_TRACE_DATA_ENCODING
void testGpsTraceData()
{
    auto stream = QByteArray::fromBase64("H8HqyQAAACgAEQAAAAYCuQaDAM4l7x/B6sn/////AAAAKAECuQajAM4lyABpAAAAAAAAArkGWgDOJfgfwerI/////wAAACgBArkGfQDOJdAAaQAAAAAAAAK5BjcAziX/H8Hqx/////8AAAAoAQK5Bl0AziXYAGkAAAAAAAACuQYIAM4mCR/B6sb/////AAAAKAECuQY5AM4l4ABpAAAAAAAAArj+rwDOJygfwerF/////wAAACgBArj+6QDOJzMAWgAAAAAAAAK49+MAziZGH8HqxP////8AAAAyAQK49+YAziZfAA4AAAAAAAACuPfdAM4mMR/B6sP/////AAAAMgECuPfQAM4mTwAMAAAAAAAAArj31wDOJhsfwerC/////wAAADIBArj3xgDOJjwACwAAAAAAAAK499EAziYIH8Hqwf////8AAAAyAQK4970AziYqAAoAAAAAAAACuPfLAM4l8R/B6sD/////AAAAMgACuPfLAM4l8QALAAAAAAAAArj3xQDOJdgfweq//////wAAADIBArj3rADOJfkACwAAAAAAAAK4978AziW/H8Hqvv////8AAAAyAQK496gAziXkAAsAAAAAAAACuPe6AM4lmh/B6r3/////AAAAMgECuPefAM4lwwAMAAAAAAAAArj3swDOJX8fweq8/////wAAADIBArj3lwDOJaUACwAAAAAAAAK496gAziVhH8Hqu/////8AAAPeAAK496gAziVhAAkAAAAAAAACuPeXAM4lBx/B6rj/////AAAD3gACuPeXAM4lBwAJAAAAAAAAArj3cwDOJM0fweq1/////wAAA94AArj3cwDOJM0APAAAAAAAABKzfscAAAAAH8HqvBKzJ8wAAAAAH8HqvRKzi5YAAAAAH8HqwRKzkikAAAAAH8HqxRKzki4AAAAAH8HqxRKziY0AAAAAH8Hqxg==");
    QDataStream dataStream(stream);

    quint32 uTime;
    dataStream >> uTime;

    auto dateTime = QDateTime::fromTime_t(uTime);
    dateTime = dateTime.addYears(2003-1970);

    qint8 sateliteCnt;
    dataStream >> sateliteCnt;
    qint8 maMethod;
    dataStream >> maMethod;
    qint16 hdop;
    dataStream >> hdop;
    qint16 gpsTraceDataSize;
    dataStream >> gpsTraceDataSize;
    qint16 vertexTraceDataSize;
    dataStream >> vertexTraceDataSize;
    qint16 linkCnt;
    dataStream >> linkCnt;

    for (auto i=0; i<5; i++) {
        quint32 uPosX;
        dataStream >> uPosX;
        quint32 uPosY;
        dataStream >> uPosY;
        dataStream >> uTime;
        quint16 uAngle;
        dataStream >> uAngle;
        quint16 uSpeed;
        dataStream >> uSpeed;

        //quint8	sateliteCnt;					//위성수
        dataStream >> sateliteCnt;
        //quint8	maMethod;					//측위방법 (0x01:측위불량, 0x02:2차원측위, 0x03:3차원측위)
        dataStream >> maMethod;
        //quint16	hdop;					//HDOP (0.1 당위로 정수화) 예) 1.2->12 으로 표시
        dataStream >> hdop;
        quint8	uMapMatchingCode;				//맵매칭 코드 (0없음 , 1성공, 2실패)
        dataStream >> uMapMatchingCode;
        quint32	uMatPosX;						//맵매칭 X 좌표(경도: 정수8자리 SK정규화좌표)
        dataStream >> uMatPosX;
        quint32	uMatPosY;						//맵매칭 Y 좌표(위도: 정수8자리 SK정규화좌표)
        dataStream >> uMatPosY;
        quint16	uMatAngle;
        dataStream >> uMatAngle;
        quint16 activityType;
        dataStream >> activityType;
        quint32 altitude;
        dataStream >> altitude;

        if (activityType == 0) {
            activityType++;
        }
    }
}
#endif
#endif

void RouteSearchProxy::fillGpsTraceData(bool isPeriodicReroute, QVariantMap &m)
{
    auto isGpsStartCode = mStartSearchCode == RouteConstants::EXPLORER_GPS_START;
    if (false == isGpsStartCode) {
        return;
    }

    auto isOnline = isRpServerOnline();
    // NDDS사양이기 때문에 온라인에서만 전달한다.
    // 19.07.10 ymhong
    if (isOnline) {
        std::vector<VSM_TVASGPS_DATASET> gpsTraceData;
        QByteArray stream;
        if (VSM_TRUE != VSMNavi_GetGPSTraceData(isPeriodicReroute, gpsTraceData)) {
        }
        std::vector<VSM_TVASVERTEX_DATASET> vertexTraceData;
        if (VSM_TRUE != VSMNavi_GetVertexTraceData(vertexTraceData)) {
        }
        std::vector<VSM_TVAS_LINK_DATASET> linkTraceData;
        if (VSM_TRUE != VSMNavi_GetLinkTraceData(linkTraceData)) {
        }

        if (gpsTraceData.size() > 0) {
            auto trace0 = gpsTraceData.at(0);

            quint16 gpsTraceSize = gpsTraceData.size();
            quint16 vertexTraceSize = vertexTraceData.size();
            quint16 linkTraceSize = linkTraceData.size();

            QDataStream dataStream(&stream, QIODevice::WriteOnly);
            dataStream << trace0.uTime;
            dataStream << trace0.sateliteCnt;
            dataStream << trace0.maMethod;
            dataStream << trace0.hdop;
            dataStream << gpsTraceSize;
            dataStream << vertexTraceSize;
            dataStream << linkTraceSize;

            quint16 activityType = 0;
            quint32 altitude = 0;

            for (auto g : gpsTraceData) {
                dataStream << g.uPosX;
                dataStream << g.uPosY;
                dataStream << g.uTime;
                dataStream << g.uAngle;
                dataStream << g.uSpeed;
                dataStream << g.sateliteCnt;
                dataStream << g.maMethod;
                dataStream << g.hdop;
                dataStream << g.uMapMatchingCode;
                dataStream << g.uMatPosX;
                dataStream << g.uMatPosY;
                dataStream << g.uMatAngle;
                dataStream << activityType;
                dataStream << altitude;
            }

            for (auto v : vertexTraceData) {
                dataStream << v.uTileCode;
                dataStream << v.uPosX;
                dataStream << v.uPosY;
                dataStream << v.uDistance;
                dataStream << v.uTime;
            }

            for (auto l : linkTraceData) {
                quint8 dir = (l.dir == 0) ? 0 : 1; //l.dir -> VSM_UInt32. NDDS규격서에는 byte임.

                dataStream << l.meshCode;
                dataStream << l.linkId;
                dataStream << dir;
                dataStream << l.uTime;
            }

#ifdef QT_DEBUG
#ifdef TEST_GPS_TRACE_DATA
#ifdef TEST_GPS_TRACE_DATA_ENCODING
            auto base64 = stream.toBase64();
            QFile file("test64.txt");
            if (file.open(QFile::WriteOnly)) {
                file.write(base64);
                file.close();
            }
#else
            testGpsTraceData();
#endif //TEST_GPS_TRACE_DATA_ENCODING
#endif //TEST_GPS_TRACE_DATA
#endif //QT_DEBUG

            m[RPREQ_KEY_GpsTraceData] = stream;
        }
    }
}

void RouteSearchProxy::fillLastGoalData(QVariantMap &m)
{
    const QString dateTimeFormat = "yyyyMMddhhmmss";
    if (mLastGoalPos.isNull() == false) {
        m[RPREQ_KEY_LastGoalSessionId] = mLastGoalSessionId;
        m[RPREQ_KEY_LastGoalTime] = mLastGoalTime.toString(dateTimeFormat);
        m[RPREQ_KEY_LastGoalPos] = mLastGoalPos;
    } else {
#ifdef QT_DEBUG
        // 전달이 잘 되는지 테스트용.
//        m[RPREQ_KEY_LastGoalSessionId] = "123456789012345678901234";
//        m[RPREQ_KEY_LastGoalTime] = QDateTime::currentDateTime().toString(dateTimeFormat);
//        m[RPREQ_KEY_LastGoalPos] = QPoint(1, 2);
#endif
    }
}

void RouteSearchProxy::fillAngleAndSpeed(QVariantMap &m)
{
    VSM_Int32 angle = 0;
    VSMNavi_GetCurrentAngle(angle);

    VSM_Int32 speed = 0;
    VSMNavi_GetCurrentSpeed(speed);

    m[RPREQ_KEY_Angle] = angle;
    m[RPREQ_KEY_Speed] = speed;
}

void RouteSearchProxy::setSessionId(const QString &sessionId)
{
    if (mSessionId != sessionId) {
        mSessionId = sessionId;
    }
}

QString RouteSearchProxy::sessionId() const
{
    return mSessionId;
}

void RouteSearchProxy::saveLastGoalData(const QString &sessionId, const QDateTime &currentTime, const QPoint goalPos)
{
    qDebug() << "[Last] saveLastGoalInformation(sessionId: " << sessionId << ", currentTime: " << currentTime << ", goal: " << goalPos << ")";

    if (goalPos.isNull()) {
        mLastGoalPos = mRouteManager.targetItem().toPoint();
    } else {
        mLastGoalPos = goalPos;
    }
    mLastGoalSessionId = sessionId;
    mLastGoalTime = currentTime;
    mSessionId.clear();

    QVariantMap m;
    m[RgConstants::PastPosX] = mLastGoalPos.x();
    m[RgConstants::PastPosY] = mLastGoalPos.y();
    m[RgConstants::PastDate] = mLastGoalTime.toString("yyyyMMddhhmmss");
    m[RgConstants::PastSessionId] = mLastGoalSessionId;
    saveRgPreference(RgConstants::PastPos, RgConstants::PastPosData, m);
}

void RouteSearchProxy::resetLastGoalData()
{
    qDebug() << "[Last] resetLastGoalInformation";
    mLastGoalPos = QPoint{};
    mLastGoalSessionId.clear();
    mSessionId.clear();

    QVariantMap m;
    m[RgConstants::PastPosX] = 0;
    m[RgConstants::PastPosY] = 0;
    m[RgConstants::PastDate] = "";
    m[RgConstants::PastSessionId] = "";
    saveRgPreference(RgConstants::PastPos, RgConstants::PastPosData, m);
}

void RouteSearchProxy::setUpdateRpDebugText(bool b)
{
    mUpdateRpDebugText = b;
}

void RouteSearchProxy::setRpContinue(bool b)
{
    mIsRpContinue = b;
}

bool RouteSearchProxy::isRpContinue() const
{
    return mIsRpContinue;
}

void RouteSearchProxy::setNddsPrimaryDomain(const QString &domain)
{
    auto *rpnSearch = RPnSearch::sharedInstance();
    rpnSearch->setPrimaryNddsDomain(domain);
}

void RouteSearchProxy::saveViaToPreference()
{
    auto l = mRouteManager.getViaStringList();
    saveRgPreference(RgConstants::Termination, RgConstants::Via, l);

    auto address = mRouteManager.targetAddress();
    saveRgPreference(RgConstants::Termination, RgConstants::Address, address);
}

void RouteSearchProxy::restoreViaFromPreference(const QStringList &via, bool skipDeparture)
{
    mRouteManager.restoreFromStringList(via, skipDeparture);
    if (skipDeparture) {
        RouteManager::RouteItem item;
        item.address = mCurrentAddress;
        item.wx = mCurrentPosition.x();
        item.wy = mCurrentPosition.y();
        item.status = INSERTED;
        mRouteManager.setItem(RouteItem_Start, item);
    }
}

void RouteSearchProxy::sendWaypointProperties()
{
    sendWaypointList();
    sendActiveDestinationInfo();
}

void RouteSearchProxy::sendWaypointList()
{
    auto *viaList = new QList<TS_ROUTE_POS>();

    for (int i=0; i<RouteItem_Num; i++) {
        TS_ROUTE_POS pos;
        memset(&pos, 0, sizeof(TS_ROUTE_POS));

        auto item = mRouteManager.getItem(static_cast<RouteItemType>(i));
        item.setTsRoutePos(&pos);
        viaList->append(pos);
    }

    auto routePosListPtr = QRoutePosListPtr(viaList);

    QVariantMap m;
    m["propertyValue"] = QVariant::fromValue(routePosListPtr);
    m["type"] = NavigationPropertyProtocol::TmapWaypointList;

    sendNotification(NavigationPropertyConstants::setNavigationProperty, m);
}

void RouteSearchProxy::sendWaypointOperationStatus(int status)
{
    sendWaypointOperationStatus(QList<int>{status});
}

void RouteSearchProxy::sendWaypointOperationStatus(QList<int> status)
{
    QVariantMap m;
    m["type"] = NavigationPropertyProtocol::TmapWaypointOperationStatusChanged;
    m["propertyValue"] = QVariant::fromValue(status);

    sendNotification(NavigationPropertyConstants::setNavigationProperty, m);
}

void RouteSearchProxy::sendActiveDestinationInfo()
{
    auto *viaList = new QList<TS_ROUTE_POS>();

    for (int i=1; i<RouteItem_Num; i++) {
        TS_ROUTE_POS pos;
        memset(&pos, 0, sizeof(TS_ROUTE_POS));

        auto item = mRouteManager.getItem(static_cast<RouteItemType>(i));
        if ( !item.isNull() ) {
            item.setTsRoutePos(&pos);
            viaList->append(pos);
        }
    }

    auto routePosListPtr = QRoutePosListPtr(viaList);

    if (viaList->size()) {
        QVariantMap m;
        m["propertyValue"] = QVariant::fromValue(routePosListPtr);
        m["type"] = NavigationPropertyProtocol::TmapActiveDestinationInfo;

        sendNotification(NavigationPropertyConstants::setNavigationProperty, m);
    }
}

}
