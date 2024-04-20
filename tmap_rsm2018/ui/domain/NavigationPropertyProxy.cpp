#include "NavigationPropertyProxy.h"
#include "RequestCommand.h"
#include "Constants.h"

#include <QDebug>
#include "NavigationPropertyProtocol.h"
#include "NavigationPropertyConstants.h"
#include "navigationdata.h"
#include "VSM_Map.h"
#include "VSM_Navi.h"
#include "VSM_Navi_Audio.h"
#include "SearchConstants.h"

namespace SKT {

NavigationPropertyProxy::NavigationPropertyProxy(QObject* property)
    : mProperty(qobject_cast<NavigationProperty*>(property))
{
    // init caching Property Data
}

void NavigationPropertyProxy::setNavigationProperty(const RequestCommandPtr& req)
{
    QVariantMap reqData = req->getReqData();
    reqData.insert(CommonConstants::RequestCommandPtr,QVariant::fromValue(req));

    const int type = reqData["type"].toInt();
//    const QString propertyName = reqData["propertyName"].toString();
//    const QVariant propertyValue = reqData["propertyValue"];

//    mProperty->setProperty(propertyName.toLatin1(),propertyValue);
    // or
    switch(type) {
    case NavigationPropertyProtocol::TmapRGDataInfo:
        mProperty->setTmapRGDataInfo(reqData["propertyValue"].value<RGDataPtr>());
       break;
    case NavigationPropertyProtocol::TmapSatelliteInfo:
        mProperty->setTmapSatelliteInfo(reqData["propertyValue"].value<SatelliteDataPtr>());
       break;
    case NavigationPropertyProtocol::TmapViewLevel:{
            ViewLevelInfo value;
            qreal data = reqData["propertyValue"].value<qreal>();
            value.mLevel = int(data);
            value.mSubLevel = int(data*1000)%1000;
            mProperty->setTmapViewLevel(value);
        }
        break;
    case NavigationPropertyProtocol::TmapMapMoveMode:
        mProperty->setTmapMapMoveMode(reqData["propertyValue"].value<int>());
       break;
    case NavigationPropertyProtocol::TmapViewMode:
        mProperty->setTmapViewMode(reqData["propertyValue"].value<int>());
       break;
    case NavigationPropertyProtocol::TmapWorkLocation:
        mProperty->setTmapWorkLocation(reqData["propertyValue"].value<TS_ROUTE_POS>());
       break;
    case NavigationPropertyProtocol::TmapHomeLocation:
        mProperty->setTmapHomeLocation(reqData["propertyValue"].value<TS_ROUTE_POS>());
       break;
    //TMCRSM-2389:내비게이션의 음성안내 설정 상태를system에 전달하는 인터페이스
    case NavigationPropertyProtocol::TmapVoiceGuidanceType:
        mProperty->setTmapVoiceGuidanceType(reqData["propertyValue"].value<int>());
       break;
    case NavigationPropertyProtocol::TmapVolume:
        mProperty->setTmapVolume(reqData["propertyValue"].value<int>());
        break;
    case NavigationPropertyProtocol::TmapIsMute:
        mProperty->setTmapIsMute(reqData["propertyValue"].value<bool>());
       break;
    case NavigationPropertyProtocol::TmapNaviStatus:
       {
           auto value = reqData["propertyValue"].value<int>();
           mProperty->setTmapNaviStatus(value);
           //T_DEBUG( "TmapNaviStatus #1" ) ;
       }
       break;
    case NavigationPropertyProtocol::TmapWaypointOperationStatusChanged:
        mProperty->setWaypointOperationStatusList(reqData["propertyValue"].value<QList<int>>());
        break;
    case NavigationPropertyProtocol::TmapActiveDestinationInfo:
        mProperty->setTmapActiveDestinationInfo(reqData["propertyValue"].value<QRoutePosListPtr>());
        break;
    case NavigationPropertyProtocol::TmapWaypointList:
        mProperty->setTmapWaypointList(reqData["propertyValue"].value<QRoutePosListPtr>());
        break;
    case NavigationPropertyProtocol::TmapRouteCalculateDuration:
        mProperty->setRouteCalculateDuration(reqData["propertyValue"].toInt());
        break;
    case NavigationPropertyProtocol::TmapRouteRecalculationReason:
        mProperty->setRouteRecalculationReason(reqData["propertyValue"].toInt());
        break;
    case NavigationPropertyProtocol::TmapFavoriteChange:
        mProperty->setFavoriteList(reqData["propertyValue"].value<QRoutePosListPtr>());
       break;
    case NavigationPropertyProtocol::TmapRecentDestinationChange:
       mProperty->setRecentDestinationList(reqData["propertyValue"].value<QRoutePosListPtr>());
       break;
    case NavigationPropertyProtocol::TmapSearchWordChange:
       mProperty->setSearchWordList(reqData["propertyValue"].value<QSearchWordListPtr>());
       break;
    case NavigationPropertyProtocol::TmapUserProfileChanged:
       mProperty->setTmapUserProfileChange(reqData["propertyValue"].value<int>());
       break;
    case NavigationPropertyProtocol::TmapRouteOptionChanged:
       mProperty->setTmapRouteOptionChange(reqData["propertyValue"].value<int>());
       break;
    case NavigationPropertyProtocol::TmapScreenIDChanged:
       mProperty->setTmapScreenID(reqData["propertyValue"].value<int>());
       break;
    //TMCRSM-2421:내비게이션의 경고음 안내 설정 상태를system에 전달하는 인터페이스
    case NavigationPropertyProtocol::TmapOverSpeedAlertActive:
       mProperty->setTmapOverSpeedAlertActive(reqData["propertyValue"].value<int>());
       break;
   }
}

}
