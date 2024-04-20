#include "MapViewMediator.h"
#include "Constants.h"
#include "RgConstants.h"
#include "RouteConstants.h"
#include "MapConstants.h"
#include "ZoneConstants.h"
#include "SimulatorConstants.h"
#include "SettingConstants.h"
#include "HiddenSettingConstants.h"
#include "RgProtocol.h"
#include "mediator/app/MapViewMode.h"
#include "ResourceResolver.h"
#include "QmlAppHelper.h"
#include <QWindow>
#include <QMouseEvent>
#include <QQuickItem>
#include "RouteManager.h"
#include "StringHelper.h"
#include "SimulationViewMediator.h"
#include "ViewRegistry.h"
#include "SettingDto.h"
#include "PreferenceVo.h"
#include "PreferenceConstants.h"
#include "SoundConstants.h"
#include "NavigationControllerConstants.h"
#include "NavigationControllerProtocol.h"
#include "rprequest.h"
#include "NavigationPropertyProtocol.h"
#include "NavigationPropertyConstants.h"
#include "mediator/rg/RgUtils.h"
#include "DebugUtils.h"
#include "ApplicationConstants.h"
#include "VSM_Map_Settings.h"
#include "VSM_Navi.h" // temporarily feat for bosh DR gps requirement
#include "QmlAppHelper.h"
#include "TestConstants.h"
#include "DebugUtils.h"
#include "SettingProxy.h"
#include "VSM_Map.h"

#ifdef BUILD_TARGET
#include "hmiZonetype.h"
#include "PlatformHelper.h"
#endif

using namespace SKT;

MapViewMediator::MapViewMediator( QObject* viewComponent )
    : AbstractViewMediator( viewComponent ),
    mBoschInfo( nullptr ),
    mMapViewMode( MapViewMode::Normal ),
    mMapViewModeReason( MapViewMode::None ),
    mSupportBreakawayReroute ( false ),
    mSupportPeriodicReroute ( false ),
    mIsEnableToPassVia( false ),
    mIsVisibleRerouteDialog( false ),
    mLastShowRerouteDialogTime(0),
    mDisplayMMFB(false)
{
    mRenderer = findChild(QStringLiteral("renderer"));
    mGpsState = findChild(QStringLiteral("gpsState"));
    mGpsMapmatchTimestamp = findChild(QStringLiteral("gpsMapmatchTimestamp"));

    connect(mRenderer, SIGNAL(viewLevelChanged(qreal)), this, SLOT(onViewLevelChanged(qreal)));
    //mMapModeSetting = 0; // Bird View(default)
    mMapModeSetting = 1; //HeadUp (MapViewQuick's default viewMode) 20.01.07 ymhong

//    QWindowList windows = QGuiApplication::allWindows();
//    QWindow* win = windows.first();
//    win->installEventFilter( this );
}

QList<int> MapViewMediator::listNotificationInterests()
{
    static QList<int> notes = {
        RgConstants::ChangedSignalState,
        RgConstants::UpdateRouteGuidance,
        RgConstants::UpdateGpsMapmatchTimestamp,
        MapConstants::ApplyMapViewMode,
        RouteConstants::DoReroute,
        RouteConstants::RequestSkipViaReroute,
        MapConstants::ResponseMapScale,
        MapConstants::UpdateMapView,
        MapConstants::UpdateMapViewPadding,
        MapConstants::ShowVolume,
        MapConstants::HideVolume,
        ZoneConstants::DoSoftKeyFunction,
        MapConstants::PauseMapView,
        MapConstants::ResumeMapView,
        MapConstants::ShowTrafficInfoOnRouteLine,
        MapConstants::ShowTrafficInfoOnRouteLineForce,
        MapConstants::ShowTrafficInfoOnMap,
        SettingConstants::ApplySetting,
        SettingConstants::ChangedSetting,
        PreferenceConstants::ApplyPreference,
        RgConstants::ShowRouteGuidanceContinuously,
        NavigationControllerConstants::ChangeOnlineStatus,
        NavigationControllerConstants::ShowTrafficInfo,
        NavigationControllerConstants::ApplyController,
        NavigationControllerConstants::UpdateSkyState,
        NavigationControllerConstants::ResponseController,
        NavigationControllerConstants::ChangeLanguage,
        NavigationControllerConstants::PauseMapView,
        MapConstants::ChangeViewMode,
        MapConstants::ChangeViewLevel,
        MapConstants::DrawMarkerAll,
        MapConstants::EnableCallout,
        NavigationControllerConstants::GoToStandby,
        ApplicationConstants::ActivatedView,
        ApplicationConstants::DeactivatedView,
        ApplicationConstants::ReplyToViewList,
        HiddenSettingConstants::ChangeGpsMapmatch,
        HiddenSettingConstants::ChangeMapmatchingFeedback,
        HiddenSettingConstants::ChangeMapmatchingDebugText,
        RgConstants::ReplyToUiState,
        RgConstants::ChangedUiState,
        RgConstants::ReplyToIsSoundPlaying,
        RgConstants::ReplyToRouteGuidanceState,
        RgConstants::ChangedRouteGuidanceState,
        SettingConstants::DialogAccepted,
        SettingConstants::DialogCanceled,
        MapConstants::RequestGlobalMapViewStatus,
        ApplicationConstants::ShowGadget,
        ApplicationConstants::UpdatedUserProfile,
        ApplicationConstants::ChangedUserProfile,
        TestConstants::UpdateMapTheme,
        ApplicationConstants::ChangeToApp,
        ApplicationConstants::ChangeToWidget,
        MapConstants::ShowRoute,
        MapConstants::ChangeGpsStatePosition,
        ApplicationConstants::UpdateLastSesstionId,
        RouteConstants::UpdateRequestRouteDebugText,
    };

    return notes;
}

QObject* MapViewMediator::retrieveBoschInfo()
{
    if ( mBoschInfo == nullptr ) {
        QMetaObject::invokeMethod( mViewComponent, "getBoschInfo" );
        mBoschInfo = findChild(QStringLiteral("boschInfo"));
    }

    return mBoschInfo;
}

bool MapViewMediator::eventFilter(QObject *target, QEvent *event )
{
    if (event->type() == QEvent::MouseButtonDblClick) {
        auto me = static_cast<QMouseEvent*>(event);
        retrieveBoschInfo();
        if ( mBoschInfo ) {
            if ( QmlAppHelper::instance()->hitTest( me->pos(), mBoschInfo ) ) {
                QMetaObject::invokeMethod( mViewComponent, "toggleBoschInfo" );
            }
        }
    }

    return QObject::eventFilter(target, event);
}

void MapViewMediator::restoreDriveMapModeAndLevel()
{
    QMetaObject::invokeMethod( mViewComponent, "restoreDriveMapModeAndLevel", Q_ARG(QVariant,mMapModeSetting) );
}

void MapViewMediator::requestRerouteByBreakaway()
{
    QVariantMap m1;
    m1.insert( QStringLiteral("soundPlayScenario"), VSM_VoiceScenarioIndex::LEAVE_REQUSET_ROUTE);
    sendNotification( SoundConstants::PlaySound, m1 );

    QVariantMap m;
    m.insert( QStringLiteral("RequestType"), RPRequest::RequestReRouteByBreakaway); // RequestReRouteByBreakaway
    m.insert(RPREQ_KEY_DestSearchCode, RouteConstants::EXPLORER_RE_DES);
    sendNotification( RouteConstants::RequestReroute, m );
}

bool MapViewMediator::onEvent( int event, const QVariant& data )
{
    switch ( event ) {
    case Mediated: {
            auto iniPath = QDir::homePath() + QChar('/') + QLatin1String("tmap_rsm_ui_dbg.ini");
            QFile file( iniPath );
            if (file.exists()) {
                QSettings settings( iniPath, QSettings::IniFormat );
                auto benchVsmDpi = settings.value(QStringLiteral("benchVsmDpi"),0).toInt();
                if (benchVsmDpi != 0) {
                    mRenderer->setProperty("dpi", QVariant::fromValue(benchVsmDpi));
                }
            }            

            //MMV 18.4.18
            sendNotification(MapConstants::RegistGlobalMapViewRenderer,
                QVariantMap{
                    {QStringLiteral("mapView"), QVariant::fromValue( mRenderer )}
                }
            );

            sendNotification(MapConstants::NotifyMapViewMode,QVariantMap{{QStringLiteral("reason"),MapViewMode::MapViewCreated}});

            createRgViews();

            requestSettingAndPreference();

            // 18.3.12 hskim for managing the position of the gps signal state display
            sendNotification( RgConstants::RequestUiState );

            TMAP_RSM_UI_DEBUG_SETTING_HANDLER("debugShowTestKeypad",[&](const QVariant& value){
                if ( value.toBool() ) {
                    QMetaObject::invokeMethod( mViewComponent, "showTestKeypad" );
                }
            });

            DebugUtils::updateMapTheme(TestConstants::D, mRenderer, TestConstants::D_CS_C);
            DebugUtils::updateMapTheme(TestConstants::D, mRenderer, TestConstants::D_RLT);
            DebugUtils::updateMapTheme(TestConstants::D, mRenderer, TestConstants::D_RLTT);

            updateLanguage();

            sendNotification(RgConstants::RequestRouteGuidanceState, getMediatorId());
        }
        break;
    case StartedToRender:
        {
            qDebug() << "[CTX]MapViewMediator StartedToRender";
            QVariantMap body;
            sendNotification(MapConstants::GlobalMapViewStartedToRender, body);

            // Mediate 에서 처리하던 것을 맵 렌더링 이후 모드 설정이 되도록 옮겨옴.
            // 카바타가 움직이지 않을 때 맵이 화면에 아직 설정되지 않은 상태에서 모드 설정이 적용이 안되는 듯 함.
            // 20.01.07 ymhong
            body[CommonConstants::Section] = SettingConstants::DirectionGuid;
            body[CommonConstants::Key] = SettingConstants::MapMode;
            sendNotification( SettingConstants::RequestSetting, body );
        }
        break;
    case RendererTouched:
    case CloseMapTarget:
    case ViewLevelChanged:
    case AreaNameChanged:
    case ObjectHitting:
    case ModalChanged:
    case ViewModeChanged:
    case MoveModeChanged:
    case RendererSizeChanged:
    case Hitting:
        {
            handleMap( event, data );
        }
        break;

    case CreateRgPopup:
    case ReRouteButtonClicked:
    case SkipViaButtonClicked:
        {
            handleRg( event, data );
        }
        break;

    case ResetMapControlViewTimer:
        {
            sendNotification(MapConstants::ResetMapViewTimer);
        }
        break;

    case VolumeButtonClicked:
        {
#if (1)
            // do nothing.
            sendNotification(NavigationControllerConstants::ShowSettingVolume, true);
#else
#ifdef BUILD_TARGET
        static int type = HOME_PAGE;
        PlatformHelper::instance()->setZone1(type, QVariant{});
        ++type;
        if (type >= ZONE1TYPES_END) {
            type = HOME_PAGE;

            static char* names[]= {
            "HOME_PAGE",
            "TITLE",
            "TITLE_PHONE",
            "SEARCH_BUTTON",
            "SEARCH_BUTTON_PHONE",
            "SEARCH_BUTTON_ABC",
            "SEARCH_BUTTON_ABC_PHONE",
            "SEARCH_BAR",
            "SEARCH_TEXT_1",
            "SEARCH_TEXT_2",
            "NAVIGATION_FEEDBACK",
            "AUDIO_FEEDBACK",
            "HOME_MENU_EDIT_1",
            "HOME_MENU_EDIT_2",
            "Zone1Type_TOGGLE",
            "VOICE_REC",
            "ONLY_CLOCK",
            "NAV_DEBUG_MENU",
            "NAV_SEARCH_BAR",
            "Zone1Type_NO_DATA",
            "ABC_SEARCHLIST_002"
            };
            qDebug() << "zone1 = " << names[type];
        }
#endif
#endif
        }
        break;

    case CreateMapModeOptionPopup:
        {
            auto jsVal = data.value<QJSValue>();
            auto viewMode = jsVal.property( QStringLiteral("viewMode") ).toString();

            QVariantMap body;
            body[CommonConstants::Url] = ViewRegistry::url(ViewRegistry::MapModeOptionView);
            body[QStringLiteral("viewMode")] = viewMode; // Driving, Directions
            QObject *popupRoot = nullptr;
#if defined(BUILD_TARGET)
            popupRoot = PlatformHelper::instance()->popupRoot();
#endif
            if (popupRoot) {
                body[CommonConstants::Parent] = QVariant::fromValue(popupRoot);
            } else {
                body[CommonConstants::ParentObjectName] = "popupWrap";
            }
            sendNotification(Constants::CreateView, body);
        }
        break;
    case CloseMapModeOptionPopup:
        {
            QVariantMap body;
            body[CommonConstants::ObjectName] = ViewRegistry::objectName(ViewRegistry::MapModeOptionView);
            sendNotification(Constants::CloseView, body);
        }
        break;

    case Activated:
        {
            qDebug() << "[MAP] MapView.Activated !!!";

            auto args = data.toMap();

            sendNotification(MapConstants::RestoreMapViewMode,QVariantMap{{QStringLiteral("reason"),MapViewMode::MapViewActivated}});
            sendNotification( ZoneConstants::RegistSoftkey, ViewRegistry::objectName( ViewRegistry::MapView ) );
            sendNotification( ZoneConstants::SelectSoftkeyMenuMode, QStringList{});            

            // 19.7.12 hskim fix [TMCRSM-3377][TMCRSM-3417]
            if (args[QStringLiteral("isGadget")] == false) {
                sendNotification( MapConstants::UpdateMapViewPadding );
            }

#if (0)
            // TMCRMS-3648
            // 가젯 모드에서 복귀할 때 MapView가 Activated되는 케이스가 있어서 처리한 것이 Side가 있음.
            // 현재는 불필요한 MapView Activated가 발생하지 않기 때문에 롤백.
            // 19.08.21 ymhong.
            auto gadgetMode = isMapForGadget();
            auto fromGadget = isReturnFromGadget();

            qDebug() << "[MAP] gadgetMode: " << gadgetMode << ", fromGadget: " << fromGadget;

            if (!gadgetMode && !fromGadget) {
                sendNotification( ZoneConstants::SelectZone1TextInput , QVariantMap{{CommonConstants::Text,CommonConstants::Empty}});
            }
#else
            sendNotification( ZoneConstants::SelectZone1TextInput , QVariantMap{{CommonConstants::Text,CommonConstants::Empty}});
#endif
            sendNotification(RgConstants::BlockReroute, false);

            QVariantMap body;
            body.clear();
            body.insert(QStringLiteral("type"),NavigationPropertyProtocol::TmapScreenIDChanged);
            body.insert(QStringLiteral("propertyValue"),0);
            sendNotification(NavigationPropertyConstants::setNavigationProperty,body);

            // request navigation volume.
            body.clear();
            body.insert(QStringLiteral("type"), NavigationControllerProtocol::GetNaviVolume );
            sendNotification( NavigationControllerConstants::RequestController, body );

            //TMCRSM-2389:내비게이션의 음성안내 설정 상태를system에 전달하는 인터페이스(request send voice guidance active)
            sendNotification( NavigationControllerConstants::SendNotiGetVoiceGuidanceActive );

            //TMCRSM-2421:내비게이션의 경고음 안내 설정 상태를system에 전달하는 인터페이스
            sendNotification( NavigationControllerConstants::SendNotiSetOverSpeedSoundAlertInfoActive);

            switch (mMapViewMode) {
            case MapViewMode::Navi:
                sendNotification(ZoneConstants::SelectSoftkeyNaviMode);
                break;
            case MapViewMode::Normal:
                sendNotification( ZoneConstants::SelectSoftkeyNormalDriveMode);// SelectSoftkeyNaviMode );
                break;
            default:
                //check !!!
                sendNotification( ZoneConstants::SelectSoftkeyNormalDriveMode);// SelectSoftkeyNaviMode );
                break;
            }

            // 속도 반응형 지도 사용하지 않을 때
            // 마지막 사용 맵뷰 레벨값 요청
            // 18.12.31 ymhong
            if (false == mUseSpeedResponseMap) {
                body[SettingConstants::PreferencesName] = PreferenceConstants::Rg;
                body[CommonConstants::Section] = SettingConstants::MapMain;
                body[CommonConstants::Key] = "viewLevel";
                sendNotification( PreferenceConstants::RequestPreference, body );
            }

            // TMCRSM-2292. 맵 모드로 복귀할 때 poi 팝업은 닫음.
            // 19.01.02. ymhong
            sendNotification(MapConstants::RemoveCalloutPopup);
            // TMCRSM-2429, 2472. 인터페이스로 경유지 편집 화면 띄울 때 경로선 사라
            sendNotification(MapConstants::ShowRoute, QVariantMap{{"show", true}});
        }
        break;
    case RegistCsw:
        sendNotification(NavigationControllerConstants::RegistCSW, data);
        break;
    case RemoveCsw:
        sendNotification(NavigationControllerConstants::RemoveCSW, data);
        break;
    case RendererOnDestruction:
        sendNotification(MapConstants::DetachMapViewFromVsmNavi,QVariant::fromValue(mRenderer));
        break;
    case TestConstants::KeypadClicked:
        qDebug() << "MapViewMediator::KeypadClicked data: " << data.toInt();
        sendNotification(HiddenSettingConstants::ApplyGpsTimeBias, data.toInt());
        break;
    case MapDragged:
        if ( mMapViewMode == MapViewMode::MapControl || mMapViewMode == MapViewMode::NormalMapControl ) {
            sendNotification(MapConstants::ResetMapViewTimer);
        }
        break;
    case MapTouched:
        //[#온라인 주소 표출] 2018.09.13 cjlee
        sendNotification(MapConstants::RendererTouched,data);
        break;
    case SpeedResponseMapDisable: {
            QVariantMap body;
            body.insert( CommonConstants::Type, NavigationControllerProtocol::ApplySpeedLevel );
            body.insert( SettingConstants::SpeedResponseMapUse, false);
            sendNotification( NavigationControllerConstants::ApplyController, body );
        }
        break;
    case SpeedResponseMapEnable: {
            QVariantMap body;
            body.insert( CommonConstants::Type, NavigationControllerProtocol::ApplySpeedLevel );
            sendNotification( NavigationControllerConstants::ApplyController, body );
        }
        break;

    case IsNaviOrAndoChanged:
        sendNotification( MapConstants::GlobalMapViewIsNaviOrAndoChanged, data );
        break;
    case IsOnlineChanged:
        sendNotification( MapConstants::GlobalMapViewIsOnlineChanged, data );
        break;
    case ShowTrafficInfoChanged:
        sendNotification( MapConstants::GlobalMapViewShowTrafficInfoChanged, data );
        break;
    case EnableTrafficInfoChanged:
        sendNotification( MapConstants::GlobalMapViewEnableTrafficInfoChanged, data );
        break;
    case ShowTrafficInfoOnMapChanged:
        sendNotification( MapConstants::GlobalMapViewShowTrafficInfoOnMapChanged, data );
        break;
    case ShowTrafficInfoOnRouteLineChanged:
        sendNotification( MapConstants::GlobalMapViewShowTrafficInfoOnRouteLineChanged, data );
        break;
    case ShowTrafficInfoOnRouteLineForceChanged:
        sendNotification( MapConstants::GlobalMapViewShowTrafficInfoOnRouteLineForceChanged, data );
        break;
    case NightModeChanged:
        sendNotification( MapConstants::GlobalMapViewNightModeChanged, data );
        break;
    case SkyStateChanged:
        sendNotification( MapConstants::GlobalMapViewSkyStateChanged, data );
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

    case RequestViewList: {
            QVariantMap body;
            body.insert( CommonConstants::ObjectName,   ViewRegistry::objectName( ViewRegistry::MapView ));
            body.insert( CommonConstants::Relay, "NavigationControllerProxy");
            sendNotification(ApplicationConstants::RequestViewList, body);
        }
        break;

    default:
        return false;
    }

    return true;
}

bool MapViewMediator::handleNotification( const mvc::INotificationPtr& note )
{
    switch ( note->getId() ) {
    case MapConstants::ResponseMapScale:
    case MapConstants::UpdateMapView:
    case MapConstants::UpdateMapViewPadding:
    case MapConstants::PauseMapView:
    case MapConstants::ResumeMapView:
    case MapConstants::ShowTrafficInfoOnMap:
    case MapConstants::ShowTrafficInfoOnRouteLine:
    case MapConstants::ShowTrafficInfoOnRouteLineForce:
    case MapConstants::ChangeViewMode:
    case MapConstants::ChangeViewLevel:
    case MapConstants::DrawMarkerAll:
    case MapConstants::ShowVolume:
    case MapConstants::HideVolume:
    case MapConstants::EnableCallout:
    case MapConstants::ShowRoute:
        {
            handleMap( note );
        }
        break;
    case RgConstants::ChangedSignalState:
    case RgConstants::UpdateRouteGuidance:
    case RouteConstants::DoReroute:
    case RgConstants::ShowRouteGuidanceContinuously:
    case RgConstants::UpdateGpsMapmatchTimestamp:
    case RouteConstants::RequestSkipViaReroute:
    case RgConstants::ChangedUiState:
    case RgConstants::ReplyToUiState:
    case RgConstants::ReplyToIsSoundPlaying:
    case RgConstants::ReplyToRouteGuidanceState:
    case RgConstants::ChangedRouteGuidanceState:
        {
            handleRg( note );
        }
        break;
    case MapConstants::ApplyMapViewMode:
        {
            handleMapViewMode( note );
        }
        break;
    case SettingConstants::ApplySetting:
    case SettingConstants::ChangedSetting:
    case PreferenceConstants::ApplyPreference:
    case SettingConstants::DialogAccepted:
    case SettingConstants::DialogCanceled:
    case ApplicationConstants::UpdatedUserProfile:
    case ApplicationConstants::ChangedUserProfile:
        {
            handleSettingAndPreference( note );
        }
        break;
    case NavigationControllerConstants::ApplyController:
    case NavigationControllerConstants::ChangeOnlineStatus:
    case NavigationControllerConstants::ShowTrafficInfo:
    case NavigationControllerConstants::GoToStandby:
    case NavigationControllerConstants::GoToRunning:
    case NavigationControllerConstants::UpdateSkyState:
    case NavigationControllerConstants::ResponseController:
    case NavigationControllerConstants::ChangeLanguage:
    case NavigationControllerConstants::PauseMapView:
        {
            handleController( note );
        }
        break;
    case ApplicationConstants::ChangeToApp:
    case ApplicationConstants::ChangeToWidget:
    case ApplicationConstants::ShowGadget:
    case ApplicationConstants::ReplyToViewList:
    case ApplicationConstants::UpdateLastSesstionId:
    case ApplicationConstants::ActivatedView:
    case ApplicationConstants::DeactivatedView: {
            handleApplication( note );
        }
        break;
    case ZoneConstants::DoSoftKeyFunction: {
            auto m = note->getBody().toMap();
            auto func = m[QStringLiteral("function")].toString();
            if (m[QStringLiteral("name")].toString() == ViewRegistry::objectName(ViewRegistry::MapView)) {
                if (func == QStringLiteral("navi")) {
                    sendNotification(NavigationControllerConstants::ResetCSW);
                    sendNotification(Constants::CreateView,QVariantMap{{CommonConstants::Url, ViewRegistry::url( ViewRegistry::NaviMenuView )}});
                }
                else if (func == QStringLiteral("option")) {
                    if ( mMapViewMode == MapViewMode::Normal ) {
                        QMetaObject::invokeMethod(mViewComponent, "showMapModeDlg", Q_ARG(QVariant, "Normal"));
                    }
                    else if ( mMapViewMode == MapViewMode::Navi ) {
                        QMetaObject::invokeMethod(mViewComponent, "showMapModeDlg", Q_ARG(QVariant, "Navi"));
                    }
                }
            }
        }
        break;
    case HiddenSettingConstants::ChangeGpsMapmatch:
    {
        auto enabled = note->getBody().toBool();
        VSMNavi_SetDrawMMStatus((VSM_Bool) enabled);
        auto visible = QQmlProperty::read(mGpsMapmatchTimestamp, QStringLiteral("visible")).toBool();
        QQmlProperty::write( mGpsMapmatchTimestamp, QStringLiteral("visible"), (visible || enabled));
        QQmlProperty::write( mGpsMapmatchTimestamp, QStringLiteral("gpsTimestampVisible"), enabled);
        break;
    }

    case HiddenSettingConstants::ChangeMapmatchingFeedback:
    {
         auto enabled = note->getBody().toBool();
         auto visible = QQmlProperty::read(mGpsMapmatchTimestamp, QStringLiteral("visible")).toBool();
         QQmlProperty::write( mGpsMapmatchTimestamp, QStringLiteral("visible"), (visible || enabled));
         QQmlProperty::write( mGpsMapmatchTimestamp, QStringLiteral("gpsMapmatchFeedbackVisible"), enabled);
    }
        break;

    case HiddenSettingConstants::ChangeMapmatchingDebugText:
    {
        auto enabled = note->getBody().toBool();
        mDisplayMMFB = enabled;
        { // clear debug-text view
            QString mmfb;
            mmfb.clear();
            QQmlProperty::write( mGpsMapmatchTimestamp, QStringLiteral("gpsMapmatchFeedbackData"), mmfb );
        }
    }
        break;

    case RouteConstants::UpdateRequestRouteDebugText:
        mRpDebugText = note->getBody().toString();
        break;

    case MapConstants::RequestGlobalMapViewStatus:
        {
            QVariantMap m;
            m["isNaviOrAndo"] = QQmlProperty::read( mViewComponent, QStringLiteral("isNaviOrAndoChanged")).toBool();
            m["isOnline"] = QQmlProperty::read( mViewComponent, QStringLiteral("isOnline")).toBool();
            m["showTrafficInfo"] = QQmlProperty::read( mViewComponent, QStringLiteral("showTrafficInfo")).toBool();
            m["enableTrafficInfo"] = QQmlProperty::read( mViewComponent, QStringLiteral("enableTrafficInfo")).toBool();
            m["showTrafficInfoOnMap"] = QQmlProperty::read( mViewComponent, QStringLiteral("showTrafficInfoOnMap")).toBool();
            m["showTrafficInfoOnRouteLine"] = QQmlProperty::read( mViewComponent, QStringLiteral("showTrafficInfoOnRouteLine")).toBool();
            m["showTrafficInfoOnRouteLineForce"] = QQmlProperty::read( mViewComponent, QStringLiteral("showTrafficInfoOnRouteLineForce")).toBool();
            m["viewLevel"] = QQmlProperty::read( mRenderer, QStringLiteral("viewLevel")).toInt();
            m["viewMode"] = QQmlProperty::read( mRenderer, QStringLiteral("viewMode")).toInt();
            m["moveMode"] = QQmlProperty::read( mRenderer, QStringLiteral("moveMode")).toInt();
            m["nightMode"] = QQmlProperty::read( mRenderer, QStringLiteral("nightMode")).toBool();
            m["skyState"] = QQmlProperty::read( mRenderer, QStringLiteral("skyState")).toInt();

            sendNotification( MapConstants::RespondToGlobalMapViewStatus, m );
        }
        break;
    case TestConstants::UpdateMapTheme:
        {
            auto m = note->getBody().toMap();
            DebugUtils::updateMapTheme(TestConstants::D, mRenderer, m["item"].toString());
        }
        break;        
    case MapConstants::ChangeGpsStatePosition:
        {            
            auto m = note->getBody().toMap();
            auto x = m["x"].toInt();
            qDebug() << "[GlobalMap]ChangeGpsStatePosition x: " << x;
            QMetaObject::invokeMethod( mViewComponent, "changeGpsStatePosition", Q_ARG(QVariant, x) );
        }
        break;

    default:
        return false;
    }

    return true;
}

// map view auto level config for speed
void MapViewMediator::onAppliedMapAutoLevelConfig( const QVariant& config )
{
    sendNotification(HiddenSettingConstants::ChangeVsmAutoLevelMap, config);
}


void MapViewMediator::onRegister()
{
    AbstractViewMediator::onRegister();
}

void MapViewMediator::updateLanguage()
{
    auto lang = QmlAppHelper::instance()->language();
    QQmlProperty::write(mViewComponent, "lang", lang);
}

bool MapViewMediator::isMapForGadget() const
{
    auto ret = QQmlProperty::read(mViewComponent, "isMapForWidget").toBool();

    return ret;
}

bool MapViewMediator::isReturnFromGadget() const
{
    auto ret = QQmlProperty::read(mViewComponent, "returnFromWidget").toBool();
    if (ret) {
        QQmlProperty::write(mViewComponent, "returnFromWidget", false);
    }
    return ret;
}
