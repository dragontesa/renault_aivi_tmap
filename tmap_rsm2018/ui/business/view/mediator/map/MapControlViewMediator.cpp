#include "MapControlViewMediator.h"
#include "mediator/app/MapViewMode.h"
#include "Constants.h"
#include "RouteConstants.h"
#include "MapConstants.h"
#include "NavibookmarkConstants.h"
#include "ZoneConstants.h"
#include "SimulatorConstants.h"
#include "RouteManager.h"
#include "ViewRegistry.h"
#include "../../common/rprequest.h"
#include "NavibookmarkProtocol.h"
#include "CommonConstants.h"
#include "PreferenceConstants.h"
#include "SearchConstants.h"
#include "SettingConstants.h"
#include "NavigationPropertyConstants.h"
#include "NavigationPropertyProtocol.h"
#include "NavigationControllerConstants.h"
#include "VSM_Map_Settings.h"
#include "VSM_Map.h"

//2018.09.13 - cjlee
#include "../common/searchrequest.h"
#include "SearchConstants.h"
#include "UtilConstants.h"

#include "PlatformHelper.h"
#include "SoftkeyMenu.h"

#define USE_SOFTKEY_INDEX_MENU

namespace SKT {

#define TIMER_INTERVAL  (1000*5)
MapControlViewMediator::MapControlViewMediator(QObject *viewComponent)
    : AbstractViewMediator(viewComponent)
{
    connect(&timer, &QTimer::timeout, [this](){
        restoreMapMode();
    });
    timer.start(TIMER_INTERVAL);

    mLastDriveMode = -1;
}

MapControlViewMediator::~MapControlViewMediator()
{
    sendNotification(SimulatorConstants::HideSimulatorControl);
}

QList<int> MapControlViewMediator::listNotificationInterests()
{
    static QList<int> interests = {
        MapConstants::ResetMapViewTimer,
        MapConstants::ReturnClicked,
        MapConstants::ResponseMapPosition,
        MapConstants::ResponseMapScale,
        MapConstants::ResponseMapAreaName,

        //[#온라인 주소 표출] 2018.09.13 cjlee
        MapConstants::RendererTouched,
        SearchConstants::ResponseOnlineSearch,

        ZoneConstants::DoSoftKeyFunction,
        ZoneConstants::DoSoftkeyIndexMenuFunction,

        NavigationControllerConstants::ChangeLanguage,

        NavigationControllerConstants::ResponseOnlineStatus,
        NavigationControllerConstants::ChangeOnlineStatus
    };

    return interests;
}

bool MapControlViewMediator::onEvent(int event, const QVariant &data)
{
    switch (event) {
    case Mediated: {
            auto args = data.value<QVariantMap>();
            mRenderer = args["mapView"].value<QObject*>();

            sendNotification(MapConstants::RequestMapScale, data);
            sendNotification(NavigationControllerConstants::QueryOnlineStatus);
        }
        break;

    case Activated: {
            auto args = data.value<QVariantMap>();
            auto lastDriveMode = args["lastMapViewMode"].toInt();
            if(lastDriveMode == MapViewMode::Normal
                    || lastDriveMode == MapViewMode::Navi
                    || lastDriveMode == MapViewMode::Simul){
                sendNotification(MapConstants::ShowCarvata, true);//+
                //T_DEBUG( "#1" );
                sendNotification(MapConstants::ChangeMapViewMode,QVariantMap{{CommonConstants::Mode,MapViewMode::NormalMapControl}});
            }
            else{
                sendNotification(MapConstants::ShowCarvata, false);//+
                //T_DEBUG( "#2" );
                sendNotification(MapConstants::ChangeMapViewMode,QVariantMap{{CommonConstants::Mode,MapViewMode::MapControl}});
            }

            QVariantMap body;
            body.insert(QStringLiteral("type"),NavigationPropertyProtocol::TmapScreenIDChanged);
            body.insert(QStringLiteral("propertyValue"),1);
            sendNotification(NavigationPropertyConstants::setNavigationProperty,body);

            setupSoftkeyMenu(lastDriveMode);

            mLastDriveMode = lastDriveMode;

            if (lastDriveMode == MapViewMode::Simul) {
                sendNotification(MapConstants::ChangeAutoZoom, false);
                sendNotification(NavigationControllerConstants::CollectCSW, QVariant::fromValue(mViewComponent));
            }

            sendNotification(MapConstants::MapControlViewActivated, true);
            sendNotification(SimulatorConstants::ShowSimulatorControl);
        }//case Activated
        break;

    case Deactivated:
        sendNotification(MapConstants::MapControlViewActivated, false);
        sendNotification(ZoneConstants::SelectSoftkeyMenuMode, QStringList{});
        if (mLastDriveMode == MapViewMode::Simul) {
            sendNotification(MapConstants::ChangeAutoZoom, true);
        }
        break;

    case NewDestinationAccepted:
        sendByDestination();
        close();
        break;

    case NewDestinationCanceled:
        close();
        break;

    case ModalChanged:
        {
            sendNotification(ZoneConstants::UpdateModal,data.toMap());
        }
        break;
    case MapScaleChanged:
        break;
        #if 0 // TMCRSM-2716 (From Renault), do hold user's tilt on while controlling map, so this codes get dropped
    case MapScaleUpClicked: { //  TMCRSM-550 , it must consider if the view's 3d angle is more than 3d max angle tilt  2018.3.20
         auto level = data.toInt();
         auto handle = qvariant_cast<void*>(mRenderer->property("nativeHandle"));
         VSMCameraConfig config;
         auto success = VSMMap_Setting_GetCameraConfig(handle, config);
         success = VSMMap_SetTiltAngle(handle, config.max3dAngles[level], true);
        }
         break;
         #endif
    case MapScaleReturnButtonClicked:
        sendNotification(MapConstants::ReturnClicked, data);
        break;
    case RestoreSimulationViewButtonClicked:
        restoreMapMode();
        break;

    case ResetTimer:
        resetTimer();
        break;

    case RegistCSW:
        sendNotification(NavigationControllerConstants::RegistCSW, data);
        break;

    case SelectCSW:
        sendNotification(NavigationControllerConstants::SelectCSW, data);
        break;

    case MoveMap: //CSW 키로 지도 이동
        {            
            auto m = data.toMap();
            auto x = m["x"].toInt();
            auto y = m["y"].toInt();
            auto screenSize = 50;
            m["x"] = x * screenSize;
            m["y"] = y * screenSize;

            sendNotification(MapConstants::MoveMapByScreenPos, m);
        }
        break;

    case SaveMapViewLevel:
        {
            auto mapViewLevel = data.toInt();

            // 마지막 뷰 레벨을 저장함.
            sendNotification(PreferenceConstants::SavePreference, QVariantMap{
                 {"preferencesName",PreferenceConstants::Rg},
                 {CommonConstants::Section,SettingConstants::MapMain},
                 {CommonConstants::Key,QStringLiteral("viewLevel")},
                 {CommonConstants::Value, mapViewLevel}
             });
        }
        break;

    //[#온라인 주소 표출]
    case RequestSearch:
        requestAddress();
        break;
    }//switch(event)

    return false;
}

bool MapControlViewMediator::handleNotification(const SKT::mvc::INotificationPtr &noti)
{
    switch (noti->getId()) {
    case NavigationControllerConstants::ChangeLanguage:
        setupSoftkeyMenu(mLastDriveMode, /*registOwner*/false);
        break;

    case NavigationControllerConstants::ChangeOnlineStatus:
    case NavigationControllerConstants::ResponseOnlineStatus:
        {
            auto isOnline = noti->getBody().toBool();
            QQmlProperty::write(mViewComponent, "isOnline", isOnline);
            if (isOnline) {
                // 온라인으로 변경 시 현재 맵 위치에 대한 주소가 없을 수 있으므로 무조건 요청
                // 19.07.17 ymhong
                requestAddress();
            }
        }
        break;

    case MapConstants::ResetMapViewTimer:
        // Map drag에 의한 호출됨.
        resetTimer();

        // 마지막으로 유저가 움직인 지점을 기록. (insertWaypointAtDefaultPosition 대응)
        sendNotification(MapConstants::MoveMapByUser);
        break;

    case MapConstants::ReturnClicked:
        timer.stop();
        restoreMapMode();
        break;
    case MapConstants::ResponseMapScale:
        {
            auto level = noti->getBody().toInt();
            if (level) {
                QMetaObject::invokeMethod(mViewComponent, "updateMapScale", Q_ARG(QVariant, level));
            }
        }
        break;

    case MapConstants::ResponseMapPosition:
        {
            auto pt = noti->getBody().toPoint();
            sendNotification(MapConstants::RequestMapAreaName, QVariantMap{
                                 {CommonConstants::X, pt.x()},
                                 {CommonConstants::Y, pt.y()}
                             });
            auto address = mMapAreaName;
            showConfirmDialog(pt, address);
        }
        break;

    case MapConstants::ResponseMapAreaName:
        mMapAreaName = noti->getBody().toString();
        break;

    case ZoneConstants::DoSoftKeyFunction:
    case ZoneConstants::DoSoftkeyIndexMenuFunction:
        procSoftkey(noti->getBody());
        break;

    //[#온라인 주소 표출] 2018.09.13 cjlee
    case MapConstants::RendererTouched:
        QMetaObject::invokeMethod(mViewComponent, "__onMapTouch");
        break;
    case SearchConstants::ResponseOnlineSearch:{
            QVariantMap data = noti->getBody().value<QVariantMap>();
            if (!data.contains(SearchConstants::Result))
                return false;

            QString search = CommonConstants::Empty;
            static int totalCount = 0;
            int startIndex = 0;
            if (data.contains(SearchConstants::ReqData)) {
                QVariantMap reqData = data[SearchConstants::ReqData].value<QVariantMap>();
                auto owner = reinterpret_cast<void*>(reqData[CommonConstants::Owner].toULongLong());
                if (owner != this)
                    return false;
                if (reqData.contains(SearchConstants::Query)){
                    search = reqData[SearchConstants::Query].value<QString>();
                }

                if(startIndex == 0) {
                    if (reqData.contains(SearchConstants::Max)){
                        totalCount = reqData[SearchConstants::Max].value<int>();
                    }
                }

                if (reqData.contains(SearchConstants::Params)){
                    QVariantMap params = reqData[SearchConstants::Params].value<QVariantMap>();
                    if (params.contains(SearchConstants::StartIndex)){
                        startIndex = params[SearchConstants::StartIndex].value<int>();
                    }
                }
            }
            QString txt = data[SearchConstants::Result].value<QString>();
            QJsonDocument jsonResponse = QJsonDocument::fromJson(txt.toUtf8());
            QJsonObject jsonObject = jsonResponse.object();
            QVariantMap result = jsonObject.toVariantMap();
            QMetaObject::invokeMethod(mViewComponent, "__onResult",  Q_ARG(QVariant, QVariant::fromValue(result)) );
            sendNotification( UtilConstants::HideProgressIndicator );
        }
        break;

    }

    return false;
}

void MapControlViewMediator::close()
{
    QMetaObject::invokeMethod(mViewComponent, "close");
}

void MapControlViewMediator::resetTimer()
{
    timer.start(TIMER_INTERVAL);
}

void MapControlViewMediator::restoreMapMode()
{
    //T_DEBUG( "#1" );
	// 17.5.10 hskim 화면 별로 RestoreMapViewMode를 호출하지 않아도 됨.
	// 화면이 닫히면 MapView의 Activated 처리에서 호출하는 방식으로 통합적으로 처리.
    //sendNotification(MapConstants::RestoreMapViewMode);
    sendNotification(Constants::CloseView, QVariantMap{{CommonConstants::ObjectName, "MapControlView"}});
}

void MapControlViewMediator::procSoftkey(const QVariant &data)
{
    QVariantMap m = data.toMap();
    if (m["name"].toString() != "MapControlView")
        return;

    QString function = m["function"].toString();
    if (function == "menu") {

        sendNotification(MapConstants::RequestMapAreaName);
        if (mMapAreaName.isEmpty())
            mMapAreaName = QQmlProperty::read(mViewComponent, "address").toString();

        if (mMapAreaName.isEmpty()) {
            resetTimer();
            return;
        }

        QString menu = m["menu"].toString();
        auto index = -1;
        if (m.contains("index")) {
            index = m["index"].toInt();
            menu.clear();
        }

        if (index == SetAsDestination || menu == tr("목적지로")) {
            QVariantMap m {{"backup", true}};
            sendNotification(RouteConstants::CloseRouteCandidateView, m);

            if (mLastDriveMode == MapViewMode::Navi) {
                QVariantMap m {{"mapView",QVariant::fromValue(mRenderer)},{CommonConstants::Type, QStringLiteral("center")}};
                sendNotification(MapConstants::RequestMapPosition, m);
                // do not close.
                return;
            } else {
                sendByDestination();
                close();
            }
        } else if (index == SetAsWaypoint || menu == tr("경유지로")) {
            // 빈 경유지 슬롯에 자동 추가 되도록 slot을 -1로 설정한다.
            sendNotification(RouteConstants::SelectViaSlot, QVariantMap{{"slot", -1}});

            sendNotification(RouteConstants::BackupViaData);

            QVariantMap body;
            body["mapView"] = QVariant::fromValue(mRenderer);
            body[CommonConstants::Caller] = ViewRegistry::objectName(ViewRegistry::MapControlView);
            sendNotification(MapConstants::RequestMapPosition, body);
        } else if (index == AddToBookmark || menu == tr("즐겨찾기 추가")) {
            sendNotification(MapConstants::RequestMapPosition,
                             QVariantMap {
                                 {CommonConstants::Type, CommonConstants::Favorite},
                                 {CommonConstants::Caller, ViewRegistry::objectName(ViewRegistry::MapControlView)},
                             });
            close();
        }
    } else if (function == "back") {
        T_DEBUG( "#1" );
        timer.stop();
        sendNotification(ZoneConstants::SelectSoftkeyNormalDriveMode);
        restoreMapMode();
    }
}

void MapControlViewMediator::sendByDestination()
{
    QVariantMap body;
    body["clear"] = true;
    sendNotification(RouteConstants::SetDeparture, body);

    QVariantMap m {
        {SearchConstants::DestType, RouteConstants::EXPLORER_LONGITUDE_N_LATITUDE_DES}
    };
    sendNotification(RouteConstants::SetArrival, m);
    sendNotification(RouteConstants::RequestRouteAsDestination);

    QVariantMap body2;
    body2.insert(CommonConstants::Type,NavibookmarkProtocol::TransactionMap);
    body2.insert(SearchConstants::SearchType,NavibookmarkProtocol::MapSearch);
    sendNotification(NavibookmarkConstants::NewNavibookSession,body2);
}

void MapControlViewMediator::showConfirmDialog(const QPoint& pt, const QString &address)
{
    QVariantMap m;

    m[QStringLiteral("url")] = ViewRegistry::url(ViewRegistry::ChangeDestinationView);
    m[QStringLiteral("destX")] = pt.x();
    m[QStringLiteral("destY")] = pt.y();
    m[QStringLiteral("address")] = address;
    m[QStringLiteral("view")] = QVariant::fromValue(mViewComponent);
    PlatformHelper::instance()->setParentForSystemPopup(m);

    sendNotification(Constants::CreateView, m);
}

void MapControlViewMediator::setupSoftkeyMenu(int lastDriveMode, bool registOwner)
{
    QStringList menus;
    QVariantList menus2;

    switch (lastDriveMode) {
    case MapViewMode::Normal:
#ifdef USE_SOFTKEY_INDEX_MENU
        menus2.append(QVariant::fromValue(SoftkeyMenu{SetAsDestination, tr("목적지로")}));
        menus2.append(QVariant::fromValue(SoftkeyMenu{AddToBookmark, tr("즐겨찾기 추가")}));
#else
        menus << tr("목적지로") << tr("즐겨찾기 추가") << "noOption";
#endif
        break;
    case MapViewMode::Navi:
#ifdef USE_SOFTKEY_INDEX_MENU
        menus2.append(QVariant::fromValue(SoftkeyMenu{SetAsWaypoint, tr("경유지로")}));
        menus2.append(QVariant::fromValue(SoftkeyMenu{SetAsDestination, tr("목적지로")}));
        menus2.append(QVariant::fromValue(SoftkeyMenu{AddToBookmark, tr("즐겨찾기 추가")}));
#else
        menus << tr("경유지로") << tr("목적지로") << tr("즐겨찾기 추가") << "noOption";
#endif
        break;
    default:
        break;
    }

    if (menus.count() > 0 || menus2.count() > 0) {
        if (registOwner) {
            auto objectName = QQmlProperty::read(mViewComponent, CommonConstants::ObjectName).toString();
            sendNotification(ZoneConstants::RegistSoftkey, objectName);
        }
#ifdef USE_SOFTKEY_INDEX_MENU
        sendNotification(ZoneConstants::SelectSoftkeyIndexMenuMode, QVariantMap{
                             {"menus2", menus2},
                             {"defaultMenu", tr("목적지로")}
                         });
#else
        sendNotification(ZoneConstants::SelectSoftkeyMenuMode, menus);
#endif
    }
}

void MapControlViewMediator::requestAddress()
{
    QVariantMap body;
    body.insert( SearchConstants::SearchType, SearchRequestBase::SearchAddress);
    sendNotification(SearchConstants::RequestSearch, body);
}

}//SKT
