#include "VsmNaviProxy.h"
#include "RouteSummaryItemVo.h"
#include "RouteSummaryItemListModel.h"
#include "RouteSummaryVo.h"
#include "RouteCandidateListModel.h"
#include "VSM.h"
#include "VSM_Navi.h"
#include "VSM_Map.h"
#include "VSM_Map_Settings.h"
#include "RouteSearchProxy.h"
#include "StringHelper.h"
#include <QDebug>
// for the map cavatar side by VSM
#include "VsmNavigationProtocol.h"
#include "SettingConstants.h"
#include "SettingDto.h"
#include "SettingProxy.h"
#include "DatabaseHelper.h"
#include "SearchConstants.h"
#include "CommonConstants.h"
//

#include "RouteSummaryTurnCodeHelper.h"
#include "RouteSummaryHelper.h"
#include "../common/rprequest.h"
#include "../common/PoiHelper.h"

#include "VsmMapProxy.h"
#include "RouteGuidanceProxy.h"

#include "SettingProxy.h"
#include "SettingDto.h"

#include "../environment.h"

// TODO: Request 시 사용하는 TVAS 버전과 맞추어야 함. VSM에서 enum 정의 필요.
// #define VSM_TVAS_VERSION       6        // 5.0

#define DistanceKey     QStringLiteral("distance")
#define FeeKey          QStringLiteral("fee")
#define RemainTimeKey   QStringLiteral("remainTime")
#define SkyCodeKey      QStringLiteral("skyCode")
#define TemporatureKey  QStringLiteral("temp")

#define TVAS_VERSION_5_0    6
#define TVAS_VERSION_5_1    7
#define TVAS_VERSION_5_2    8
#define TVAS_VERSION_5_3    9
#define TVAS_VERSION_5_4    10

namespace SKT {

VsmNaviProxy::VsmNaviProxy(QObject* controller)
    : AbstractProxy("VsmNaviProxy"),
      mController(qobject_cast<NavigationController*>(controller) ),
      mOnline(false)
{
    mSelectedRoute = 0;
    mTvasVer = TVAS_VERSION_5_4;
    mTtsPath = Environment::getRGDataPath() + "/changeVolume.wav";

    connect(controller, SIGNAL(onlineChanged(bool)), this, SLOT(onlineChanged(bool)));
}

void VsmNaviProxy::setGlobalMapView(QObject* mapView)
{
    mGlobalMapView = mapView;
    mGlobalMapViewHandle = qvariant_cast<void*>(mapView->property("nativeHandle"));
}

QObject* VsmNaviProxy::chooseMapView(QObject* mapView) const
{
    return mapView != nullptr ? mapView : mGlobalMapView;
}

void* VsmNaviProxy::chooseMapViewHandle(QObject* mapView) const
{
    return mapView != nullptr ? qvariant_cast<void*>(mapView->property("nativeHandle")) : mGlobalMapViewHandle;
}

void VsmNaviProxy::attachMapView( QObject* mapView, VSM_ViewType type )
{    
    auto handle = qvariant_cast<void*>(mapView->property("nativeHandle"));
    VSMNavi_AttachMapView(handle, type);
}

void VsmNaviProxy::detachMapView( QObject* mapView )
{
    auto handle = qvariant_cast<void*>(mapView->property("nativeHandle"));
    VSMNavi_DetachMapView(handle);
}

//QPoint VsmNaviProxy::centerPosition() const
//{
//    // 17.12.8 hskim apply multi map view
//    // needs to refactor
//    auto proxy = Facade::instance()->retrieveProxy<VsmMapProxy>();
//    return proxy->centerPosition( mMapView );
//}

QPoint VsmNaviProxy::locationPoint() const
{
    VSM_Int32 wx;
    VSM_Int32 wy;
    VSM_Bool ret = VSMNavi_GetLocationPoint(wx, wy);

    if (ret == VSM_TRUE) {
        return QPoint(wx, wy);
    } else {
        return QPoint();
    }
}

QPoint VsmNaviProxy::currentPosition() const
{
    VSM_Int32 wx;
    VSM_Int32 wy;
    VSM_Bool ret = VSMNavi_GetCurrentPosition(wx, wy);

    if (ret == VSM_TRUE) {
        return QPoint(wx, wy);
    } else {
        return QPoint();
    }
}

int VsmNaviProxy::currentSpeed() const
{
    int speed;
    if (VSMNavi_GetCurrentSpeed(speed) == VSM_FALSE) {
        speed = 0;
    }
#ifdef QT_DEBUG
    // 디버깅을 위해 모의 주행이 중단없이 항상 실행할 수 있도록 처리
    // 19.06.04 ymhong
    if (mEnableStopSimulatorBySpeed == false) {
        speed = 0;
    }
#endif
    return speed;
}

void VsmNaviProxy::applySelectRoute(QObject* mapView, quint32 routeId)
{
    mSelectedRoute = routeId;
    VSMNavi_ApplySelectRoute(routeId);
}

void VsmNaviProxy::SetUserOilType(QObject* mapView)
{
    VSM_UserOilType value = static_cast<VSM_UserOilType>(mFuelType);

    auto handle = chooseMapViewHandle(mapView);
    VSMMap_Setting_SetUserOilType(handle, value);
}

void VsmNaviProxy::cancelRoute()
{
    VSMNavi_CancelRoute(VSM_FALSE);

    mLowestPricePoiID = 0;
    mGasStationInfo.clear();
    mIsSetTvas = false;
}

void VsmNaviProxy::cacheRemainDistance(int remainDistance)
{
    mRemainDistance = remainDistance;
}

void VsmNaviProxy::cacheSummary(const QVariant &v)
{
    auto m = v.toMap();
    mRemainDistance = mDistance = m[DistanceKey].toInt();
    mFee = m[FeeKey].toInt();
    mRemainTime = m[RemainTimeKey].toInt();
}

void VsmNaviProxy::fillCachedSummary(QVariantMap &m)
{
    m[DistanceKey] = mDistance;
    m[FeeKey] = mFee;
    m[RemainTimeKey] = mRemainTime;
    m["remainDistance"] = mRemainDistance;
}

void VsmNaviProxy::cacheWeather(const QVariant &v)
{
    auto m = v.toMap();

    mSkyCode = m[SkyCodeKey].toString();
    mTemp = m[TemporatureKey].toInt();
}

void VsmNaviProxy::fillCachedWeather(QVariantMap &m)
{
    m[SkyCodeKey] = mSkyCode;
    m[TemporatureKey] = mTemp;
}

inline bool isValidStation(int stationType, VSM_UserOilType oilType) {
    const int GASOLINE_STATION = 0;
    const int LPG_STATION = 1;

    switch (oilType) {
    case VSM_USER_OILTYPE_GASOLINE:
    case VSM_USER_OILTYPE_DIESEL:
    case VSM_USER_OILTYPE_PREMIUM_GASOLINE:
        if (stationType == LPG_STATION) {
            return false;
        }
        break;

    case VSM_USER_OILTYPE_LPG:
        if (stationType == GASOLINE_STATION) {
            return false;
        }
        break;

    case VSM_USER_OILTYPE_ELECTROCITY:
        return false;
    }

    return true;
}

void VsmNaviProxy::setDrawGasStationInfo(QObject *mapView)
{
    auto handle = chooseMapViewHandle(mapView);
    VSM_UserOilType oilType = static_cast<VSM_UserOilType>(mFuelType);

    if (oilType == VSM_USER_OILTYPE_ELECTROCITY) {
        // Navi SDK 에서 충전소 정보를 가져온다.
        std::vector<VSM_stEStationInfo> eStations;
        VSMNavi_GetEStationInfo(eStations);

        // Map에 충전소를 표출한다.
        std::vector<VSMEvChargingStation> charingStations;
        charingStations.reserve(eStations.size());
        for (auto& info : eStations) {
            VSMEvChargingStation e;
            memset(&e, 0, sizeof(VSMEvChargingStation));
            e.poiId = info.nPoiID;
            if (info.nAC3) e.chargingType |= VSM_EV_CHARGING_TYPE_AC_3;
            if (info.nDcCHA) e.chargingType |= VSM_EV_CHARGING_TYPE_DC_CHADEMO;
            if (info.nDcCombo) e.chargingType |= VSM_EV_CHARGING_TYPE_DC_COMBO;
            e.slowChargingSupported = info.nSlowCharge != 0;
            strncpy(e.name, info.szName, sizeof(e.name)-1);
            e.wpt.x = info.wpt.x;
            e.wpt.y = info.wpt.y;
            charingStations.push_back(e);
        }

        VSMMap_SetEvCharingStationInfo(handle, &charingStations[0], charingStations.size());
    } else {
        std::vector<VSM_stGasStationInfo> retList;
        VSMNavi_GetGasStationInfo(retList);

        mLowestPricePoiID = 0;
        for (auto info : retList) {
            if (isValidStation(info.nType, oilType)) {
                mGasStationInfo.push_back(info.nPoiID);
                if (info.nIsLowerPrice == 1)
                    mLowestPricePoiID = info.nPoiID;
            }
        }
        VSMMap_SetDrawGasStationInfo(handle, (VSM_UInt32*)(&mGasStationInfo[0]), mGasStationInfo.size(), mLowestPricePoiID, oilType);
    }
}

// TODO: JUST FOR TEST
static const VSM_Char* ROUTE_ID_TABLE[] = {
    "routeRed",
    "routeBlue",
    "routeGreen"
};

bool VsmNaviProxy::setTvasData(const QStringList &filenames, const QList<quint32> &options, bool isLocal)
{
    QStringList colors;
    if (1 == filenames.count()) {
        auto index = 0;
        auto filename = filenames.at(0);
        if (filename.endsWith("2.db")) {
            index = 1;
        } else if (filename.endsWith("3.db")) {
            index = 2;
        }
        colors.append(ROUTE_ID_TABLE[index]);
    } else {
        colors.append(ROUTE_ID_TABLE[0]);
        colors.append(ROUTE_ID_TABLE[1]);
        colors.append(ROUTE_ID_TABLE[2]);
    }
    return setTvasDataWithColorTable(filenames, options, colors, isLocal);
}

bool VsmNaviProxy::setTvasDataWithColorTable(const QStringList &filenames, const QList<quint32> &options, const QStringList &colors, bool isLocal)
{
    if (filenames.count() == options.count())
        qDebug()<<QStringLiteral("setTvasData  !!!!!!!!!!");

    VSM_TvasFileInfo tvasData[3];
    memset(tvasData, 0, sizeof(tvasData));

    int num = filenames.count();

    for (int i=0; i<num; ++i) {
        QString filename = filenames.at(i);
        strcpy(tvasData[i].filePath, filename.toLatin1());
        tvasData[i].routeOption = options.at(i);
        auto latin1 = colors[i].toLatin1();
        auto *color = latin1.constData();
        strcpy(tvasData[i].routeId, color);
    }

    VSM_Int32 err;
    VSM_Bool isLocalTvas = isLocal;
    int tvasVer = isLocal ? TVAS_VERSION_5_4 : mTvasVer;

    bool result = false;
    if (VSMNavi_SetTvasData(num, tvasVer, tvasData, VSM_FALSE, isLocalTvas, err) == VSM_TRUE) {
        if (err == 1) {
            result = true;
            mIsSetTvas = true;
        }
    }

    return result;
}

bool VsmNaviProxy::setTvasDataRerouted(bool isLocal, int routeOption)
{
    bool retVal = false;

    VSM_TvasFileInfo tvasData[1];
    memset(tvasData, 0, sizeof(tvasData));

    strcpy(tvasData[0].filePath, "tvas_route.db");
    tvasData[0].routeOption = routeOption;
    VSM_Int32 err;
    VSM_Bool isLocalTvas = isLocal;
    int tvasVer = isLocal ? TVAS_VERSION_5_4 : mTvasVer;
    qDebug()<<QStringLiteral("setTvasDataReourted  !!!!!!!!!!");
    if (VSMNavi_SetTvasData(1, tvasVer, tvasData, VSM_TRUE, isLocalTvas, err) == VSM_TRUE) {
        if (err == 1) {
            retVal = true;
        }
    }

    return retVal;
}

void VsmNaviProxy::selectRoute(int index)
{
    if (mIsSetTvas) {
        if (index < 0) {
            index = mSelectedRoute;
        } else {
            mSelectedRoute = index;
        }
        VSM_Bool ret = VSMNavi_SelectRoute(index, VSM_FALSE);
    }
}

int VsmNaviProxy::getSelectedRoute() const
{
    return mSelectedRoute;
}

bool VsmNaviProxy::resumeRoute(bool isLocal)
{
    auto index = mSelectedRoute + 1;

    VSM_TvasFileInfo tvasData[1];
    memset(tvasData, 0, sizeof(tvasData));

    tvasData[0].routeOption = 1;
    auto tvas = QString("tvas_route.db");

    strcpy(tvasData[0].filePath, tvas.toLocal8Bit());
    bool retVal = false;
    VSM_Int32 err;
    VSM_Bool isLocalTvas = isLocal;
    int tvasVer = isLocal ? TVAS_VERSION_5_4 : mTvasVer;

    if (VSMNavi_SetTvasData(1, tvasVer, tvasData, VSM_TRUE, isLocalTvas, err) == VSM_TRUE) {
        if (err == 1) {
            retVal = true;

            VSMNavi_ApplySelectRoute(0);
        }
    }
    return retVal;
}

bool VsmNaviProxy::isSetTvas() const
{
    return mIsSetTvas;
}

void VsmNaviProxy::blockReroute(bool block)
{
    VSMNavi_BlockReroute(block);
}

void VsmNaviProxy::setDrawRoute(bool draw)
{
    VSM_Bool isDraw = draw ? VSM_TRUE : VSM_FALSE;

    VSMNavi_SetDrawRoute(isDraw);
}

void VsmNaviProxy::drawRouteAll(bool visible)
{
    for (int i=0; i<3; i++) {
        VSM_Bool bDraw = visible ? VSM_TRUE : VSM_FALSE;
        VSMNavi_SetDrawRouteVisible(i, bDraw, VSM_FALSE);
    }
}

void VsmNaviProxy::setAutoZoom(bool b)
{
    VSM_Bool bAutoZoom = b ? VSM_TRUE : VSM_FALSE;

    VSMNavi_SetAutoZoom(bAutoZoom);
}

void VsmNaviProxy::setFuelType(int fuel)
{
    mFuelType = fuel;
}

int VsmNaviProxy::fuelType() const
{
    return mFuelType;
}

void VsmNaviProxy::setChargeType(int type)
{
    mChargeType = type;
}

int VsmNaviProxy::chargeType() const
{
    return mChargeType;
}

QVariant VsmNaviProxy::summary(int routeId,const QString &description)
{
    if (routeId < 0)
        routeId = mSelectedRoute;

    StringHelper *codec = StringHelper::instance();

    std::vector<VSM_stTmapSummaryBaseInfo> summaryBaseList;
    VSM_Bool result = VSMNavi_GetRouteSummaryInfo(summaryBaseList);
    if (result == VSM_FALSE) {
        // fail !
        QString error = "VSMNavi_GetRouteSummaryInfo --- fail";

        return QVariant();
    }

    int size = summaryBaseList.size();
    if (routeId < size ) {
        auto baseInfo = summaryBaseList.at(routeId);
        int fee = baseInfo.usTallFee * 10;

        //TMCRSM-770
        //로컬의 경우 roadNames[0]에 주요도로1;주요도로2
        //서버의 경우 roadNames[0], roadNames[1]에 주요도로 정보가 있음
        auto roadInfo = codec->fromCp949((const char*)baseInfo.roadNames[0]);
        auto roads = roadInfo.split(";");
        if (roads.count() == 1 && qstrlen((const char*)baseInfo.roadNames[1]) > 0) {
            roadInfo += ";";
            roadInfo += codec->fromCp949((const char*)baseInfo.roadNames[1]);
        }

        auto vo = t_new_shared<RouteSummaryVo>(description,
                                               codec->fromCp949((const char*)baseInfo.startPosName),
                                               codec->fromCp949((const char*)baseInfo.endPosName),
                                               roadInfo,
                                               QPoint(baseInfo.wptMin.x, baseInfo.wptMin.y),
                                               QPoint(baseInfo.wptMax.x, baseInfo.wptMax.y),
                                               baseInfo.uiTotalDist,
                                               baseInfo.ucWeatherCode,
                                               baseInfo.uiTotalTime,
                                               fee);//baseInfo.usTallFee);

        return QVariant::fromValue(vo);
    } else {
        return QVariant();
    }
}

QVariant VsmNaviProxy::getDetailRoute(int routeId)
{
    if (routeId < 0) {
        // 경로 안내 시작 후 경로 상세 보기를 요청한 경우
        // routeId를 전달하지 않고 이전에 선택한 mSelectedRoute를 사용하여
        // 상세 정보를 가져온다.
        routeId = mSelectedRoute;
    }

    Q_ASSERT(0 <= routeId);

    std::vector<VSM_stTmapSummaryList> summaryList;
    VSMNavi_GetSummaryList(routeId, summaryList);

    auto searchProxy = Facade::instance()->retrieveProxy<RouteSearchProxy>();

    QStringList addressList;
    for (int i=0; i<5; i++)
        addressList.append(searchProxy->viaAddress(i));

    auto isOnline = mOnline && mChannelPolicyReceived;
    auto isLocal = !isOnline;
    RouteSummaryHelper routeSummary(summaryList, addressList, isLocal);

    return routeSummary.model();
}

QVariant VsmNaviProxy::trafficGuidance() const
{
    std::vector<VSM_ROUTETRAFFIC> trafficGuide;
    VSM_Bool success = VSMNavi_GetRouteTrafficAllList(mSelectedRoute,trafficGuide);
    if (success == VSM_FALSE) {
        T_DEBUGV("VsmNaviProxy::trafficGuidance() - VSMNavi_GetRouteTrafficAllList(%d) failed.", mSelectedRoute);
        return QVariant();
    }

    QVariantList guides;

    if (trafficGuide.size() > 0) {
        for (unsigned int i=0; i<trafficGuide.size() ;i++) {
            const VSM_ROUTETRAFFIC& t = trafficGuide.at(i);
            QVariantList guide;
            guide << t.uAccDist << t.uCong;
            guides.append(QVariant::fromValue(guide));
        }
    }

    return guides;
}

void VsmNaviProxy::onlineChanged(bool online)
{
    mOnline = online;
}


QVariant VsmNaviProxy::getGasStationInfo() const
{
    std::vector<VSM_stGasStationInfo> retList;
    VSM_Bool success = VSMNavi_GetGasStationInfo(retList);
    if (success == VSM_FALSE) {
        T_DEBUGV("VsmNaviProxy::getGasStationInfo() - VSMNavi_GetGasStationInfo(%d) failed.", mSelectedRoute);
        return QVariant();
    }

    VSM_Int32   retAngle = 0;
    QVariantList items;
    int wx , wy;
    int nDist = 0;
    int j = 0;
    auto rgProxy = Facade::instance()->retrieveProxy<RouteGuidanceProxy>();
    mController->currentPosition(wx,wy);
    for (unsigned int i=0; i<retList.size() ;i++) {
        const VSM_stGasStationInfo &t = retList.at(i);
        QVariantList item;

        VSMMap_GetAngleWpToWp(wx, wy, t.wpt.x, t.wpt.y, retAngle = 0);
        
        // 주소를 얻는다.
        char address[VSM_MAX_AREANAME_LEN+1];
        memset(address, 0, sizeof(address));
        VSMMap_GetAreaName(t.wpt.x,t.wpt.y, address, VSM_MAX_AREANAME_LEN);

        nDist = static_cast<int>(t.nDistance) - rgProxy->movingDistance();
        if (nDist < 0) {
            continue;
        }

        SearchConstants::resultInit(item);
        item[SearchConstants::ResultIndex             ] = j++;
        item[SearchConstants::ResultKind              ] = t.nType;								// 주유소 형식(0 : 주유소, 1: 충전소, 2 : 주유소&충전소)
        item[SearchConstants::ResultOPIPoleCate       ] = t.nBrand;								// 주유소 브랜드 코드
        item[SearchConstants::ResultHighGasoline      ] = t.nHasPremiumOil;						// 고급 휘발유 판매 유무(0 : 미조사, 1 : yes, 2 : no)
        item[SearchConstants::ResultLowestOilSt       ] = t.nIsLowerPrice;						// 최저가 유무(0 : 최저가 아님, 1 : 최저가)
        item[SearchConstants::ResultX                 ] = t.wpt.x;                              // 주유소 좌표
        item[SearchConstants::ResultY                 ] = t.wpt.y;                              // 주유소 좌표
        item[SearchConstants::ResultName              ] = StringHelper::instance()->fromCp949((const char*)t.szName);               // 주유소 명칭
        item[SearchConstants::ResultStdName           ] = item[SearchConstants::ResultName];
        item[SearchConstants::ResultAddress           ] = StringHelper::instance()->fromCp949((const char*)address);                // 주유소 주소
        item[SearchConstants::ResultOPIGasolinePrice  ] = t.nGasolinePrice;						// 휘발유값 (이하 단위 원)
        item[SearchConstants::ResultOPIDieselPrice    ] = t.nDieselPrice;                       // 경유값
        item[SearchConstants::ResultOPILPGPrice       ] = t.nLPGPrice;							// LPG값
        item[SearchConstants::ResultOPIHiGasolinePrice] = t.nPremiumGasolinePrice;				// 고급 휘발유값
        item[SearchConstants::ResultDistance          ] = nDist;                                // 거리
        item[SearchConstants::ResultDeg               ] = 360 - retAngle;
        item[SearchConstants::ResultPOIID             ] = t.nPoiID;
        item[SearchConstants::ResultOnline            ] = mOnline && mChannelPolicyReceived;
        items.append(QVariant::fromValue(item));
    }
    return items;
}


QVariant VsmNaviProxy::getEStationInfo() const
{
    std::vector<VSM_stEStationInfo> retList;
    VSM_Bool success = VSMNavi_GetEStationInfo(retList);
    if (success == VSM_FALSE) {
        T_DEBUGV("VsmNaviProxy::getEStationInfo() - VSMNavi_GetEStationInfo(%d) failed.", mSelectedRoute);
        return QVariant();
    }

    VSM_Int32   retAngle = 0;
    QVariantList items;
    int wx , wy;
    int nDist = 0;
    int j = 0;
    auto rgProxy = Facade::instance()->retrieveProxy<RouteGuidanceProxy>();
    mController->currentPosition(wx,wy);

    for (unsigned int i=0; i<retList.size() ;i++) {
        const VSM_stEStationInfo &t = retList.at(i);
        QVariantList item;

        VSMMap_GetAngleWpToWp(wx, wy, t.wpt.x, t.wpt.y, retAngle = 0);


        // 주소를 얻는다.
        char address[VSM_MAX_AREANAME_LEN+1];
        memset(address, 0, sizeof(address));
        VSMMap_GetAreaName(t.wpt.x,t.wpt.y, address, VSM_MAX_AREANAME_LEN);

        nDist = static_cast<int>(t.nDistance) - rgProxy->movingDistance();
        if (nDist < 0) {
            continue;
        }

        SearchConstants::resultEvInit(item);
        item[SearchConstants::ResultIndex               ] = j++;
        item[SearchConstants::ResultX                   ] = t.wpt.x;                              // 충전소 좌표
        item[SearchConstants::ResultY                   ] = t.wpt.y;                              // 충전소 좌표
        item[SearchConstants::ResultDistance            ] = nDist;                                // 거리
        item[SearchConstants::ResultName                ] = StringHelper::instance()->fromCp949((const char*)t.szName);
        item[SearchConstants::ResultStdName             ] = item[SearchConstants::ResultName];
        item[SearchConstants::ResultAddress             ] = StringHelper::instance()->fromCp949((const char*)address);
        item[SearchConstants::ResultPOIID               ] = t.nPoiID;
        item[SearchConstants::ResultFastCharge          ] = (bool)(t.nDcCHA || t.nAC3 || t.nDcCombo);
        item[SearchConstants::ResultSlowCharge          ] = (bool)t.nSlowCharge;
        item[SearchConstants::ResultDCDemo              ] = (bool)t.nDcCHA;
        item[SearchConstants::ResultAC3                 ] = (bool)t.nAC3;
        item[SearchConstants::ResultDCCombo             ] = (bool)t.nDcCombo;
        item[SearchConstants::ResultStationCount        ] = 0;
        item[SearchConstants::ResultPathType            ] = t.nOnRoute;                         // 충전소가 경로상 존재 여부(0:존재함, 1:경로주변)
        item[SearchConstants::ResultDeg                 ] = 360 - retAngle;
        item[SearchConstants::ResultOnline              ] = mOnline && mChannelPolicyReceived;
        items.append(QVariant::fromValue(item));
    }
    return items;
}

void VsmNaviProxy::initTurnTable()
{
}

void VsmNaviProxy::soundPlayScenario(int sid){
    VSMNavi_SoundPlayScenario(sid);
}

void VsmNaviProxy::soundPlayVolume()
{
    // 현재의 내비게이션 음성 볼륨을 조절합니다.
    const char tts_euckr[] = {
        '\xC7', '\xF6',
        '\xC0', '\xE7',
        '\xC0', '\xC7',
        '\x20',
        '\xB3', '\xBB',
        '\xBA', '\xF1',
        '\xB0', '\xD4',
        '\xC0', '\xCC',
        '\xBC', '\xC7',
        '\x20',
        '\xC0', '\xBD',
        '\xBC', '\xBA',
        '\x20',
        '\xBA', '\xBC',
        '\xB7', '\xFD',
        '\xC0', '\xBB',
        '\x20',
        '\xC1', '\xB6',
        '\xC0', '\xFD',
        '\xC7', '\xD5',
        '\xB4', '\xCF',
        '\xB4', '\xD9',
        '\x0'
    };

    auto latin1 = mTtsPath.toLatin1();
    auto path = latin1.constData();

    qDebug() << "VSMNavi_SoundPlay(tts_euckr, " << path << ")" << mTtsPath;
    if(!VSMNavi_SoundIsPlaying())
    {
        VSM_Bool ret = VSMNavi_SoundPlay((VSM_Char*)tts_euckr, (VSM_Char*)path);
        qDebug() << "VSMNavi_SoundPlay(tts_euckr, " << path << ") -> result : " << (ret == VSM_TRUE ? "true" : "false");
    }
}

/*!
 * void VsmNaviProxy::updateCavatarIcon()
 *
 *  Update those cavatar from VSM.
 * \brief VSM might has latest version updated from remote server.
 */
void VsmNaviProxy::updateCavatarIcon(QObject* mapView)
{
    auto handle = chooseMapViewHandle(mapView);

//	std::vector<std::string> paths;
    VSM_PositionMarkPathMap paths;

    VSM_Bool success = VSM_GetPositionMarkPathList(paths);
	if (success == 0 || paths.size() == 0)
	{
		T_DEBUG("there has nothing to update cavatar!");
		return;
	}

	QList<SettingDtoPtr> cavataSet[VsmNaviPositionMarkType::POSITION_MARK_TYPE_MAX];

	// update all cavatar resources
    for (auto iter = paths.cbegin(); iter != paths.cend(); ++iter)
    {
        QString code = QString(iter->first.c_str());
        QString fath = QString(iter->second.c_str());
//		QString fath = QString::fromStdString(pth);
//		QString fname,code;
//		int lastSlash, firstDot;
//		lastSlash = fath.lastIndexOf(QChar('/'));
//		fname = fath.mid((lastSlash>=0)?lastSlash+1:0);
//		firstDot = fname.indexOf(QChar('.'));
//		code = fname.left(firstDot);

        // classify a code in each type
		for (int i=VsmNaviPositionMarkType::POSITION_MARK_TYPE_MIN;
				i<VsmNaviPositionMarkType::POSITION_MARK_TYPE_MAX;++i)
		{
			QString marktyp = VsmNaviPositionMarkType::markTypeKey(i).append(QChar('_'));
			if (code.indexOf(marktyp) != -1 )
			{
				QString section(VsmNaviPositionMarkType::sectionName(VsmNaviPositionMarkType::MarkType(i)));
				auto dto = t_new_shared<SettingDto>(0,section,code,fath);
				cavataSet[i].append(dto);
				break;
			}
		}
    }

	// save cavatars have updated into DB
	auto proxy = Facade::instance()->retrieveProxy<SettingProxy>();
	for (int i=VsmNaviPositionMarkType::POSITION_MARK_TYPE_MIN;
			i<VsmNaviPositionMarkType::POSITION_MARK_TYPE_MAX;++i)
	{
		if (cavataSet[i].size() > 0)
		{
			QString section(VsmNaviPositionMarkType::sectionName(VsmNaviPositionMarkType::MarkType(i)));
			proxy->setSettings(section,cavataSet[i]);
		}
	}

	// update current user cavatar with new updated one
	auto ret = proxy->setting(QStringLiteral("MapSubCarIcon"),QStringLiteral("Icon"));
	int userCavatar = ret->value().toInt(); // user cavatar is normal type
    setupCavatar(handle, userCavatar,cavataSet);
}

/*!
 * called from UI
 */
void VsmNaviProxy::setCavatarIcon(QObject* mapView, int userCavatar)
{
    auto handle = chooseMapViewHandle( mapView );

	// the param type point to element index in cavatar list for the normal type
//	if (type < VsmNaviPositionMarkType::POSITION_MARK_TYPE_MIN ||
//			type > VsmNaviPositionMarkType::POSITION_MARK_TYPE_MAX)
//	{
//		T_DEBUG("invalid type for the cavatar!");
//		return;
//	}
    auto db = DatabaseHelper::instance()->connectionOfCurrentUser();
    QList<SettingDtoPtr> cavataSet[VsmNaviPositionMarkType::POSITION_MARK_TYPE_MAX];
	auto settingDao = orm::TOrm::instance()->dao<SettingDto>();

	for (int i=VsmNaviPositionMarkType::POSITION_MARK_TYPE_MIN;i<VsmNaviPositionMarkType::POSITION_MARK_TYPE_MAX;++i)
	{
		QString section(VsmNaviPositionMarkType::sectionName(VsmNaviPositionMarkType::MarkType(i)));

        auto qry = settingDao->selectByField( db, CommonConstants::Section, section );
		while (qry->next())
		{
			cavataSet[i].append(qry->record());
		}
	}
	db.close();
	
    setupCavatar(handle, userCavatar,cavataSet);
}

void VsmNaviProxy::showGasStationInfo(QObject* mapView, bool show)
{
    auto handle = chooseMapViewHandle(mapView);

    VSM_UserOilType oilType = static_cast<VSM_UserOilType>(mFuelType);
    if (show) {
        if (oilType == VSM_USER_OILTYPE_ELECTROCITY) {
            setDrawGasStationInfo(mapView);
        } else {
            VSMMap_SetDrawGasStationInfo(handle, (VSM_UInt32*)(&mGasStationInfo[0]), mGasStationInfo.size(), mLowestPricePoiID, oilType);
        }
    } else {
        if (oilType == VSM_USER_OILTYPE_ELECTROCITY) {
            std::vector<VSMEvChargingStation> eStations;
            VSMMap_SetEvCharingStationInfo(handle, &eStations[0], 0);
        } else {
            VSMMap_SetDrawGasStationInfo(handle, (VSM_UInt32*)(&mGasStationInfo[0]), 0, 0, oilType);
        }
    }
}

/**
 * set Tvas version
 * this called  from ManageTestCommand when it got changed
 */
void VsmNaviProxy::setTvasVersion(int ver)
{
    mTvasVer = ver;
}

bool VsmNaviProxy::isRoute() const
{
    auto ret = VSMNavi_IsRoute();

    return ret == VSM_TRUE;
}

bool VsmNaviProxy::isSoundPlaying() const
{
    return VSMNavi_SoundIsPlaying() == VSM_TRUE;
}

void VsmNaviProxy::enableSimulatorStopBySpeed(bool b)
{
    mEnableStopSimulatorBySpeed = b;
}

void VsmNaviProxy::setChannelPolicyReceived(bool received)
{
    mChannelPolicyReceived = received;
}

void VsmNaviProxy::setupCavatar(void* mapViewHandle, int userCavatar, const QList<SettingDtoPtr>* cavataSet)
{
	if (userCavatar < 0) {
		T_DEBUG("invalid cavatar selection code !!");
		return;
	}

	// select default cavatar and weaksignal cavatar as normal
	QString defaultCavata = (userCavatar < cavataSet[VsmNaviPositionMarkType::POSITION_MARK_TYPE_CAVATAR_NORMAL].size()) ?
		cavataSet[VsmNaviPositionMarkType::POSITION_MARK_TYPE_CAVATAR_NORMAL].at(userCavatar)->value().toString() : "";
	QString defaultWeaksignal  = (userCavatar < cavataSet[VsmNaviPositionMarkType::POSITION_MARK_TYPE_CAVATAR_WEAK_SIGNAL].size()) ?
			cavataSet[VsmNaviPositionMarkType::POSITION_MARK_TYPE_CAVATAR_WEAK_SIGNAL].at(userCavatar)->value().toString() : "";
	if (defaultCavata.isEmpty() && cavataSet[VsmNaviPositionMarkType::POSITION_MARK_TYPE_CAVATAR_NORMAL].size() > 0)
		defaultCavata = cavataSet[VsmNaviPositionMarkType::POSITION_MARK_TYPE_CAVATAR_NORMAL].at(0)->value().toString();
	if (defaultWeaksignal.isEmpty()) defaultWeaksignal = defaultCavata;


    VSM_PositionMarkType postionMarkType[6] =
    {
        VSM_PositionMarkTypeTrack, // 현위치 Dot Type
        VSM_PositionMarkTypeCompass, // Head up 보이는 각도 표시 Type
        VSM_PositionMarkTypeNormal, // Cavata Type
        VSM_PositionMarkTypeWeakSignal, // 신호 없을 때 Type
        VSM_PositionMarkTypeBirdView, // Bird view Cavata Carvata 타입
        VSM_PositionMarkTypeWeakSignalBirdView // Bird View 신호 없을 때 Cavata Type
    };

	for (int i=VsmNaviPositionMarkType::POSITION_MARK_TYPE_MIN;i<VsmNaviPositionMarkType::POSITION_MARK_TYPE_MAX;++i)
	{
		QString cavataPath;
		if (i < VsmNaviPositionMarkType::POSITION_MARK_TYPE_CAVATAR_NORMAL) {
			cavataPath = (0 < cavataSet[i].size()) ? cavataSet[i].at(0)->value().toString() : defaultCavata;
		}
		else {
			// 카바타를 뷰모드별로 나뉘지 않고 설정에서 지정한 카바타의 Key 인덱스에 맞는 각 타입별 카바타 적용하고 없으면 디폴트 값 적용
			if ( userCavatar < cavataSet[VsmNaviPositionMarkType::POSITION_MARK_TYPE_CAVATAR_NORMAL].size()) {
				QString keyIndex = cavataSet[VsmNaviPositionMarkType::POSITION_MARK_TYPE_CAVATAR_NORMAL].at(userCavatar)->key().toString().right(2);
				for (int j=0;j<cavataSet[i].size();++j) {
					if (keyIndex == cavataSet[i].at(j)->key().toString().right(2)) {
						cavataPath = cavataSet[i].at(j)->value().toString();
						break;
					}
				}
			}


			if (cavataPath.isEmpty()) {
				switch (i) {
				case VsmNaviPositionMarkType::POSITION_MARK_TYPE_CAVATAR_WEAK_SIGNAL:
				case VsmNaviPositionMarkType::POSITION_MARK_TYPE_CAVATAR_WEAK_SIGNAL_BIRDVIEW:
					cavataPath = defaultWeaksignal;
					break;
				default:
					cavataPath = defaultCavata;
				}
			}
		}

        // MMV 18.4.19
        VSMMap_Setting_SetPositionMark(mapViewHandle, postionMarkType[i], cavataPath.toStdString()); // 17.12.8 hskim apply multi map view
	}
}

}//SKT


