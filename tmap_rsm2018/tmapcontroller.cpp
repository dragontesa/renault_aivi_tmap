#include <QQmlContext>
#include <QQuickWindow>
#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QSettings>
#include <QTimeZone>
#include <QOpenGLFramebufferObject>
#include <QImage>
#include <QOpenGLContext>
#include <QResource>
#include <curl/curl.h>

#include "tmapcontroller.h"
#include "platformcontroller.h"
#include "platform_connectivity.h"
#include "tmapproperty.h"
#include "build.h"

#include "rgsimulator.h"
#include "gpsservice.h"
#include "environment.h"
#include "map.h"
#include "mapviewquick.h"
#include "routesearch.h"
#include "rpnsearch.h"
#include "worldpoint.h"
#include "rgaudiohelper.h"
#include "searchmanager.h"
#include "addressmanager.h"
//#include "markermanager.h"
#include "fpscalculator.h"
#include "mapswitcher.h"
#include "packagemanager.h"
#include "sessionmanager.h"
#include "cipherservice.h"
#include "daynightdetector.h"
#include "debugkeyfilter.h"
#include "cswfilter.h"

#include "uilayout.h"
#include "uiconstants.h"
#include "tmap_rsm_ui.h"

#include "VSM_Map.h"
#include "VSM_Navi.h"
#include "VSM_Navi_Audio.h"
#include "VSM_Util.h"
#include "MNS_Define.h"
#include "qctestingfeatures.h"

#include "hardkey.h"
#include "datetimeutil.h"
#include "hostnames.h"
#include "util/textdecoder.h"
#include "util/opensslthreadlock.h"
#include "sslcontext.h"
#include "mns/serverpolicyfetch.h"
//#include <sys/time.h>
#include "ui/util/PlatformHelper.h"

#if TMAP_UI_LAYOUT == 1920720
#define DEFAULT_UI_LAYOUT       SKT::UILayout::L1920x720
#elif TMAP_UI_LAYOUT == 8201024
#define DEFAULT_UI_LAYOUT       SKT::UILayout::L820x1024
#else
#define DEFAULT_UI_LAYOUT       SKT::UILayout::L820x1024
#endif

#define PRINT_FPS_INTERVAL_MS   200

#define INIT_FLAG_VSM           (1)
#define INIT_FLAG_MNS           (1<<1)
#define INIT_FLAG_VSM_SUCCESS   (1<<2)
#define INIT_FLAG_MNS_SUCCESS   (1<<3)
#define INIT_FLAG_ALL           (INIT_FLAG_VSM|INIT_FLAG_MNS)
#define INIT_FLAG_ALL_SUCCESS   (INIT_FLAG_VSM_SUCCESS|INIT_FLAG_MNS_SUCCESS)

//#define USE_FBO_FOR_GADGET

static const bool FEATURE_DAYNIGHT_BY_GPSTIME = true;

namespace SKT {

// TODO:
static NavigationController::SunState sunStateConvert(DayNightDetector::TimeType t)
{
    switch (t) {
    case DayNightDetector::DAWN:
        return NavigationController::SunDawn;
    case DayNightDetector::DAY:
        return NavigationController::SunDay;
    case DayNightDetector::TWILIGHT:
        return NavigationController::SunTwilight;
    case DayNightDetector::NIGHT:
        return NavigationController::SunNight;
    default:
        return NavigationController::SunDay;
    }
}

TmapController::TmapController(QObject *parent)
    : QObject(parent)
    , m_online(false)
    , m_appForeground(false)
    , m_networkOnline(false)
    , m_sunState(SunDay)
    , m_weatherState(WeatherSunny)
    , m_audioHelper(0)
    , m_searchManager(0)
    , m_initializeFlags(0)
    , m_uiLayout(DEFAULT_UI_LAYOUT)
    , m_mapInCluster(false)
    , m_videoStreamPlay(false)
    //, m_mapInClusterWindow(0)
    , m_currentProfileId(-1)
//    , m_IsDrawRender(false)
//    , m_InstanceId(-1)
    , m_cipherService(0)
    , m_aesCipherService(0)
{
    qRegisterMetaType<VSM::WorldPoint>();
    qRegisterMetaType<SKT::NavigationController::SunState>();
    qRegisterMetaType<SKT::NavigationController::WeatherState>();
    qRegisterMetaType<SKT::QRoutePosListPtr>();
    qRegisterMetaType<SKT::QPOICateInfoListPtr>();
    qRegisterMetaType<SKT::QSearchWordListPtr>();
    qRegisterMetaType<SKT::ViewLevelInfo>("ViewLevelInfo");
    qRegisterMetaType<TS_ROUTE_POS>();
    qRegisterMetaType<VSM_stDriveInfo>();
    qRegisterMetaType<SKT::RGDataPtr>("RGDataPtr");
    qRegisterMetaType<SKT::SatelliteData>();
    qRegisterMetaType<SKT::SatelliteDataPtr>("SatelliteDataPtr");

    qmlRegisterType<VSM::MapViewQuick>("SceneGraphRendering", 1, 0, "Renderer");

    // initialize openssl
    m_platformConnectivity = newPlatformConnectivity();
    m_platformConnectivity->init();

    // initialize libcurl
    curl_global_init(CURL_GLOBAL_ALL);

    opensslthreadlock::setup();

    // setup host deployment
    TmapHostNames::setupHostDeployment();
    qDebug() << "host deployment : " << (int)TmapHostNames::getHostDeploymentId();
    int  vsmServer = (int)TmapHostNames::getHostType(TmapHostNames::HOST_STREAMMAP_INDEX);
    qDebug() << "vsm server type:" << vsmServer
    << ", server: " << TmapHostNames::getHostName(TmapHostNames::HOST_STREAMMAP_INDEX);

#if defined(QT_DEBUG) && !defined(BUILD_TARGET)
    // install key event filter for debugging
    QCoreApplication::instance()->installEventFilter(new DebugKeyFilter(this, this));
#else
    QCoreApplication::instance()->installEventFilter(this);;
#endif

    m_tmapProperty = new TmapProperty(this);

    m_qmlEngine = new QQmlApplicationEngine();

    m_gpsService = new GpsService();
    if (FEATURE_DAYNIGHT_BY_GPSTIME) {
        connect(m_gpsService, &GpsService::locationUpdated, this, &TmapController::onLocationUpdated);
    }

    m_simulator = new RGSimulator();
    m_sessionManager = new SessionManager();

    m_qmlEngine->rootContext()->setContextProperty("simulator", m_simulator);

    m_platformController = newPlatformController(this);
    connect(m_platformController, &PlatformController::platformInitialized, this, &TmapController::onPlatformInitialized);
    connect(m_platformController, &PlatformController::platformUIRequested, this, &TmapController::onPlatformUIRequested);
    connect(m_platformController, &PlatformController::platformDimmingStatusChanged, this, &TmapController::onPlatformDimmingStatusChanged);
    connect(m_platformController, &PlatformController::platformAppForegroundChanged, this, &TmapController::setAppForeground);
    connect(m_platformController, &PlatformController::platformNetworkOnlineChanged, this, &TmapController::setNetworkOnline);
    connect(m_platformController, &PlatformController::platformDialPOICallStatusChanged, this, &TmapController::onplatformDialPOICallStatusChanged);
    connect(m_platformController, &PlatformController::platformCallServiceStatusChanged, this, &TmapController::onplatformCallServiceStatusChanged);
    connect(m_platformController, &PlatformController::platformIsHFPAvailable , this, &TmapController::onPlatformIsHFPAvailable );
    connect(m_platformController, &PlatformController::platformMediaMounted, this, &TmapController::platformMediaMounted);
    connect(m_platformController, &PlatformController::platformMediaUnmounted, this, &TmapController::platformMediaUnmounted);
    connect(m_platformController, &PlatformController::platformRemountStatus, this, &TmapController::onPlatformRemountStatus);
    connect(m_platformController, &PlatformController::platformLanguageIndexStatusChanged, this, &TmapController::onPlatformLanguageIndexStatusChanged);
    connect(m_platformController, &PlatformController::platformColorStateStatusChanged, this, &TmapController::onPlatformColorStateStatusChanged);
    connect(m_platformController, &PlatformController::platformMexModeStatusChanged, this, &TmapController::onPlatformMexModeStatusChanged);
    connect(m_platformController, &PlatformController::platformPrivateDataEncryptionRegistered, this, &TmapController::onPlatformPrivateDataEncryptionRegistered);
    //connect(m_platformController, &PlatformController::platformActivateExternalImageProvider, this, &TmapController::onPlatformActivateExternalImageProvider);
    connect(m_platformController, &PlatformController::platformActivateExternalImageProvider, this, &TmapController::onPlatformActivateExternalImageProvider);
    connect(m_platformController, &PlatformController::platformExternalImageProviderTouch, this, &TmapController::onPlatformExternalImageProviderTouch);
    connect(m_platformController, SIGNAL(clientStatusGuiSenseReady()), this, SLOT(onClientStatusGuiSenseReady()));
    connect(m_platformController, SIGNAL(setPauseMap(bool)), this, SIGNAL(setPauseMap(bool)));
    connect(m_platformController, &PlatformController::platformZone3PullUpListStateChanged, this, &TmapController::onPlatformZone3PullUpListStateChanged);
    connect(m_platformController, &PlatformController::platfromNaviSignalRFAntennaStatus , this, &TmapController::onPlatformNaviSignalRFAntennaStatus );
    connect(m_platformController, &PlatformController::platformCurrentStreetChanged, this, &TmapController::onPlatformCurrentStreetChanged);
    connect(m_platformController, &PlatformController::platformfocusConfigChanged, this, &TmapController::onPlatformfocusConfigChanged);
    connect(m_platformController, &PlatformController::platformNaviGadgetFocusStatusChanged, this, &TmapController::onPlatformNaviGadgetFocusStatusChanged);
    connect(m_platformController, &PlatformController::platformNaviGadgetFocusInteractionSignal, this, &TmapController::onPlatformNaviGadgetFocusInteractionTypeChanged);
    connect(m_platformController, SIGNAL(spcxRgEnabled(bool)), this, SIGNAL(spcxRgEnabled(bool)));
    connect(m_platformController, &PlatformController::platformContextSwitchRequested, this, &TmapController::onPlatformContextSwitchRequested);
    connect(m_platformController, SIGNAL(contextStateAccepted()), this, SIGNAL(contextStateAccepted()));
    connect(m_platformController, SIGNAL(contextStateCompleted()), this, SIGNAL(contextStateCompleted()));

    m_packageManager = new PackageManager(m_platformController);

    #if 1
    QMetaObject::invokeMethod(m_platformController, "startService", Qt::QueuedConnection);
    #else // Deprecated which call directly to the platform.
    m_platformController->init();
    #endif
}

TmapController::~TmapController()
{
    if (m_simulator) {
        m_simulator->stop();
    }

    if (m_gpsService) {
        m_gpsService->stopUpdates();
    }

    if (m_qmlEngine) {
        delete m_qmlEngine;
        m_qmlEngine = NULL;
    }

    RsmUi_Terminate();

    if (m_searchManager) {
        delete m_searchManager;
        m_searchManager = NULL;
    }

    if (m_simulator) {
        delete m_simulator;
        m_simulator = NULL;
    }

    if (m_gpsService) {
        delete m_gpsService;
        m_gpsService = NULL;
    }

    if (m_tmapProperty){
        delete m_tmapProperty;
        m_tmapProperty = NULL;
    }

    if (m_sessionManager) {
        delete m_sessionManager;
        m_sessionManager = 0;
    }

    if (m_cipherService) {
        delete m_cipherService;
        m_cipherService = 0;
    }

    if (m_aesCipherService) {
        delete m_aesCipherService;
        m_aesCipherService = 0;
    }

    if (m_packageManager) {
        delete m_packageManager;
        m_packageManager = 0;
    }

    if (m_platformController) {

        QMetaObject::invokeMethod(m_platformController, "finishService", Qt::QueuedConnection);
        m_platformController->deleteLater();
        m_platformController = NULL;
    }

    destroyEngine();

    if (m_audioHelper) {
        delete m_audioHelper;
        m_audioHelper = NULL;
    }

    // clean up libcurl
    curl_global_cleanup();

    if (m_platformConnectivity) {
        m_platformConnectivity->release();
        delete m_platformConnectivity;
        m_platformConnectivity = NULL;
    }

    opensslthreadlock::cleanup();
}

bool TmapController::eventFilter(QObject *watched, QEvent *event)
{
    auto type = event->type();

    if (type == QEvent::KeyPress || type == QEvent::KeyRelease) {
        auto keyEvent = dynamic_cast<QKeyEvent*>(event);
        auto keyCode = keyEvent->nativeScanCode()-8; // Qt wayland added +8
        auto pressed = type == QEvent::KeyPress;

        // PTT키를 계속 누르고 있을 때 연속으로 forwarding되지 않도록 처리.
        // 19.08.06 ymhong.
        if (keyEvent->isAutoRepeat()) {
            return true;
        }

        qDebug() << "[EVENT] key pressed : " << pressed << ", key : " << keyCode;

#if (0)
        if (mCswFilter.processKey(keyCode, pressed)) {
            qDebug() << "[EVENT] tmapcontroller forward key : " << keyCode;
            return true;
        } else {
            if (mCswFilter.isBackKey(keyCode)) {
                // navi에서 back 전달???
                auto keyState = pressed ? 1 : 0;
                applyHardKey(keyCode, keyState);
            }
            return true;
        }
#else
        // SWID-HMIProxy-V1.4
        // 19.09.06 ymhong
        if (mCswFilter.isBackKey(keyCode)) {
            mCswBackAccepted = false;

            // navi에서 back 전달???
            auto keyState = pressed ? 1 : 0;
            applyHardKey(keyCode, keyState);

            // UI에서 CSW Back키가 처리되지 않은 경우 시스템으로 forwarding 해야 함.
            // 20.01.02 ymhong
            if (mCswBackAccepted) {
                qDebug() << "[CSW] tmapcontroller return true;";
                return true;
            } else {
                qDebug() << "[CSW] tmapcontroller forward key(1) : " << keyCode << ", " << keyState;

                PlatformHelper::instance()->keyForwarding(keyCode, keyState);
                return true;
            }
        } else {
            if (mCswFilter.processKey(keyCode, pressed)) {
                qDebug() << "[CSW] tmapcontroller forward key(2) : " << keyCode;
            }
            return true;
        }
#endif
    } else if (type == QEvent::Wheel) {
        auto*wheelEvent = dynamic_cast<QWheelEvent*>(event);
        auto angleDelta = wheelEvent->angleDelta();
        qDebug() << "[EVENT] Wheel : angleDelta -> " << angleDelta;
        int encCode = HardKey::HARDKEYCODE_CSW_ROTARY_ENCODER_BUTTON;
        int encValue = angleDelta.y() / 12;
        mCswFilter.encoderForwarding(encCode, encValue);
        return true;
    }

    // 이벤트를 처리한 경우 아래 함수가 호출 되면 안됨.
    return QObject::eventFilter(watched, event);
}

QString TmapController::appVersion() const
{
    return Build::versionString();
}

int TmapController::navigationVolume() const
{
    if (m_audioHelper) {
        return m_audioHelper->navigationVolume();
    }
    return 0;
}

void TmapController::setNavigationVolume(int volume)
{
    if (m_audioHelper) {
        m_audioHelper->setNavigationVolume(volume);
    }
}

bool TmapController::muteState(int chnanel) const
{
    if(m_audioHelper){
        return m_audioHelper->getMute(chnanel);
    }

    return false;
}

bool TmapController::setMuteState(int channel, bool mute)
{
    qDebug() << "onSetVoiceGuidance :: #4 ["<< channel <<"]" << mute << ")";

    // setMute 내에서 실행하는 mutex lock과 muteChanged에 연결된 콜체인에서 호출하는 mutex lock이,
    // 또 다른 쓰레드와 서로 교착상태에 빠지게 만드는 현상이 있어서 m_audioHelper->setMute() 내에서 호출하던 muteChanged를 setMute의 위로 빼냄. 2019.12.10 by hheo
    if (channel == 1) emit m_audioHelper->muteChanged(mute); // fixed, navi service isVoiceGuidanceActive property has not updating

    if (m_audioHelper) {
        m_audioHelper->setMute(channel, mute);
        return true;
    }
    return false;
}

void TmapController::changeHostBaseName(const QString& hostBaseName)
{
    TmapHostNames::exchangeBaseName(hostBaseName.toLatin1().constData());
    // it needs to reboot
}

void TmapController::setNetworkOnline(bool online)
{
    if (m_networkOnline != online) {
        qDebug() << "setNetworkOnline__@tmapcontroller" << online;
        m_networkOnline = online;

        if (online && (mServerPolicyFetched == false || mServerDualcenterPolicySuccess == false)
           ) {
            mServerPolicyFetched = true; // navi request the policy only once.
            // fetch ndds server policy
            PolicyFetcher* fetcher = ServerPolicyFetchCreator().create();
            connect(fetcher, &PolicyFetcher::fetchedDualcenterPolicy, this, &TmapController::onNddsDualcenterPolicyFetched);
            connect(fetcher, &PolicyFetcher::fetchedReroutePolicy, this, &TmapController::channelReroutePolicyReceived);
            connect(fetcher, &PolicyFetcher::finished, this, &TmapController::onNddsServerPolicyFinished);
            connect(fetcher, &PolicyFetcher::finished, fetcher, &PolicyFetcher::free);
//            connect(this, &TmapController::onlineChanged, fetcher, &PolicyFetcher::finished);
            connect(this, &TmapController::destroyed, fetcher, &PolicyFetcher::free);

            fetcher->fetch();
        }

        setOnline(online);
        VSM::MapSwitcher *switcher = VSM::Map::sharedInstance()->mapSwitcher();
        if (online) {
            // when the map data is invalid, map switcher is not initilized.
            if ( switcher ) {
                switcher->checkOnlineMapAvailable();
            }
        } else {
            // setOnline(false);
            // why network state should be dependent on the vsm's online state?
            //  I think the role have to be separeted, because any application's network state must be same with system's one
            // and there could be another jobs based on the network.
            // 20188.10.18 by lckxx
            switcher->switchTo(VSM::MapSwitcher::MapTypeOffline);
        }
    }
}

void TmapController::setSunState(NavigationController::SunState s)
{
    if (m_sunState != s) {
        qDebug() << "setSunType:" << s;
        m_sunState = s;
        emit sunStateChanged(s);
    }
}

void TmapController::setWeatherState(NavigationController::WeatherState w)
{
    if (m_weatherState != w) {
        qDebug() << "setWeatherType:" << w;
        m_weatherState = w;
        emit weatherStateChanged(w);
    }
}

void TmapController::simulationHardKey(int key)
{
    int hk;

    switch (key) {
    case Qt::Key_Up:
        hk = HardKey::ItcJogDialUp;
        break;
    case Qt::Key_Down:
        hk = HardKey::ItcJogDialDown;
        break;
    case Qt::Key_Left:
        hk = HardKey::ItcJogDialLeft;
        break;
    case Qt::Key_Right:
        hk = HardKey::ItcJogDialRight;
        break;
    case Qt::Key_Q:
        hk = HardKey::ItcJogDialSpinCcw;
        break;
    case Qt::Key_E:
        hk = HardKey::ItcJogDialSpinCw;
        break;
    case Qt::Key_Return:
        hk = HardKey::ItcJogDialSelect;
        break;
    case Qt::Key_Backspace:
        hk = HardKey::ItcJogDialBack;
        break;
    case Qt::Key_Backslash:
        hk = HardKey::ItcJogDialOption;
        break;

    default:
        return;
    }

    emit applyHardKeyOccur(hk, HardKey::AcceptKeyState);
}

bool TmapController::isInitVSMView()
{
    return m_InitVSMView;
}


void TmapController::setOnline(bool online)
{
    if (m_online != online) {
        qDebug() << "setOnline:" << online;
        m_online = online;
        emit onlineChanged(online);
    }
}

void TmapController::setAppForeground(bool foreground)
{
    if (m_appForeground != foreground) {
        qDebug() << "setAppForeground" << foreground;
        m_appForeground = foreground;
        emit appForegroundChanged(foreground);
    }
}

bool TmapController::currentPosition(double &longitude, double &latitude)
{
    double lastLongitude = 0, lastLatitude = 0;
    m_gpsService->lastPosition(lastLongitude, lastLatitude);

    longitude = lastLongitude;
    latitude = lastLatitude;

    return true;
}

bool TmapController::currentPosition(int &wX, int &wY)
{
    double lastLongitude = 0, lastLatitude = 0;
    m_gpsService->lastPosition(lastLongitude, lastLatitude);

    VSMPoint2f geo;
    VSMPoint2i world;
    geo.x = lastLongitude;
    geo.y = lastLatitude;

    if (VSMMap_Util_ConvertToWorld(geo, VSM_COORDINATE_WGS84, world) == VSM_TRUE) {
        wX = world.x;
        wY = world.y;
        return true;
    }

    return false;
}

QVariant TmapController::platformController() const
{
    auto p = dynamic_cast<QObject*>(m_platformController);

    return QVariant::fromValue(p);
}

//#define TEST_MAPINCLUSTER

void TmapController::setupPlatformPopup()
{
    qDebug() << "TmapController::setupPlatformPopup()";

#ifdef TEST_MAPINCLUSTER
    simulationMapInCluster(Qt::Key_F8);
#else
//    QQuickWindow *window = qobject_cast<QQuickWindow*>(m_qmlEngine->rootObjects().first());
    // QMetaObject::invokeMethod(m_platformController, "setupPopupWindow", Q_ARG(QQmlEngine*, m_qmlEngine));

    emit setupPopupWindow(m_qmlEngine);
#endif
}

bool TmapController::callNumber(const QString &number)
{
    connect(m_platformController, &PlatformController::platformCallNumberDone, [this](bool success) {
        QMetaObject::invokeMethod(this,"responsedCallNumber", Q_ARG(bool,success));
    });
    bool success = false;

    QMetaObject::invokeMethod(m_platformController,"callNumberWrapper", Qt::QueuedConnection, Q_ARG(const QString&,number) );

    // return m_platformController->callNumber(number); // Deprecated which call directly to the platform.

    return success;
}

QString TmapController::getDeviceId()
{
    return m_platformController->deviceId();
}

int TmapController::getCarconfigurationValue(uchar key) {
    int value=0;
    QMetaObject::invokeMethod(m_platformController, "carconfigurationValue", Qt::QueuedConnection, Q_ARG(uchar, key));
    QEventLoop loop;
    connect(m_platformController, &PlatformController::gotitCarConfigValue,
    [&value, &loop](uint result){
        value = result;
        loop.exit(0);
    });

    QTimer::singleShot(4000, &loop, &QEventLoop::quit);
    loop.exec();

    return value;
}

SessionManagerInterface* TmapController::sessionManager()
{
    return m_sessionManager;
}

GpsManagerInterface *TmapController::gpsManager()
{
    return m_gpsService;
}

SimulatorInterface *TmapController::simulatorManager()
{
    return m_simulator;
}

CipherServiceInterface* TmapController::cipherService(bool asQaes)
{
    CipherServiceInterface* ret = m_cipherService;

    if ( asQaes ) {
        if ( !m_aesCipherService ) {
            m_aesCipherService = new AesCipherService();
        }
        ret = m_aesCipherService;
    }

    return ret;
}

void TmapController::onInitialized()
{
    if ((m_initializeFlags & INIT_FLAG_ALL_SUCCESS) != INIT_FLAG_ALL_SUCCESS) {
        // clear initialize flags
        m_initializeFlags = 0;

        // release engine
        destroyEngine();
        emit engineInitialized(false);
        m_tmapProperty->setTmapNaviStartStatus(0);
        return;
    }

    VSM_SetTileDiskCachMode(VSM_TILE_DB_MODE_EMBEDDED);


    // initialize session manager --> 모든 온라인 서비스 관련은 듀얼센터 서버정책 반영 후 수행하도록 수정 - 2019.09.05 by lckxx
                   // --> 2019-11-25 듀얼센터 서버 정책 반영 시 인증키 무효(210501)로 인한 자동 로그 아웃되며,
                   // 세션매니저를 통해 새로운 인증키 갱신이 이루어져야 하므로 듀얼센터와 상관없이 세션관리를 수행하도록 한다.
    m_sessionManager->init();
    //m_sessionManager->setActiveProfile(1);  // TODO: set active profile
//    connect(this, &TmapController::changeUserOccur, m_sessionManager, &SessionManager::changeSessionByProfileId);
    #if 0 // must hold session key, it is not included in user data, 2018.2.8
    connect(this, &TmapController::copyUserDataOccur, m_sessionManager, [&](int sid, int tid) {
        Q_UNUSED(sid)
        m_sessionManager->clearSessionByProfileId(tid);
    });
    #endif
    connect(this, &TmapController::deleteUserOccur, m_sessionManager, &SessionManager::clearSessionByProfileId);
    connect(this, &TmapController::resetToFactorySettings, m_sessionManager, &SessionManager::clearAllSession);

    connect(this, &TmapController::startMapInCluster, &TmapController::startVideoStream);
    connect(this, &TmapController::setMapInClusterIpPort, &TmapController::setVideoStreamIpPort);
    connect(this, &TmapController::stopMapInCluster, &TmapController::stopVideoStream);
//    connect(this, &TmapController::modifyMapInCluster, &TmapController::modifyVideoStream);

    connect(this, &TmapController::resumeMapInCluster, &TmapController::resumeVideoStream);
    connect(this, &TmapController::pauseMapInCluster, &TmapController::pauseVideoStream);
    setupMapSwitcher();

    // set initial car position
    int wX, wY;
    if (currentPosition(wX, wY)) {
        VSMNavi_InputPostion(wX, wY, VSM_FALSE);
    }

    // initialize audio
    setupAudio();

    if (FEATURE_DAYNIGHT_BY_GPSTIME) {
        qDebug() << "TmapController::onInitialized #1";
        double lon, lat;
        if (currentPosition(lon, lat)) {
            qDebug() << "TmapController::onInitialized #2";

            // for initial night mode, system time is used because gps time is not received yet.
            QDateTime utc = DateTimeUtil::currentUtcDateTime();
            QDateTime timeset[4];
            SunState s = sunStateConvert(DayNightDetector::detect(utc, lon, lat, timeset));
            qDebug() << "TmapController::setSunState #1";
            setSunState(s);
        }
    } else {
        qDebug() << "TmapController::setSunState #2";
        setSunState(m_platformController->dimmingStatus() ? SunNight : SunDay);
    }

    qDebug() << "m_gpsService->startUpdates() #1";
    m_gpsService->startUpdates();
    qDebug() << "m_gpsService->startUpdates() #2";

    emit engineInitialized(true);
}

void TmapController::onVSMInitialized(bool success)
{
    m_initializeFlags |= INIT_FLAG_VSM;

    if (success)
        m_initializeFlags |= INIT_FLAG_VSM_SUCCESS;
    else
        qWarning() << "failed to initialize VSM";

    if ((m_initializeFlags & INIT_FLAG_ALL) == INIT_FLAG_ALL) {
#ifndef QT_NO_DEBUG_OUTPUT
        qDebug(sys) << "[Perf][Init]after VSM init";
#endif
        onInitialized();
    }
}

void TmapController::onRPnSearchInitialized(bool success)
{
    m_initializeFlags |= INIT_FLAG_MNS;

    if (success)
        m_initializeFlags |= INIT_FLAG_MNS_SUCCESS;
    else
        qWarning() << "failed to initialize MNS";

    if ((m_initializeFlags & INIT_FLAG_ALL) == INIT_FLAG_ALL)
        onInitialized();
}

void TmapController::onRPnSearchReInitialized(bool success)
{
    m_initializeFlags |= INIT_FLAG_MNS;

    if (success)
        m_initializeFlags |= INIT_FLAG_MNS_SUCCESS;
    else {
        m_initializeFlags &= ~INIT_FLAG_MNS_SUCCESS;
        qWarning() << "failed to initialize MNS";
    }
}

void TmapController::onPlatformInitialized()
{
    // we would need to verify connectivity state
    initMapUpdate();
}

void TmapController::onPlatformUIRequested()
{
    if ((m_initializeFlags & INIT_FLAG_ALL_SUCCESS) != INIT_FLAG_ALL_SUCCESS) {
        initGuideUI();
    } else {
        initUI();
    }

    QList<QObject*> rootObjects = m_qmlEngine->rootObjects();
    QQuickWindow *window = 0;
    if (!rootObjects.isEmpty()) {
         window = qobject_cast<QQuickWindow*>(rootObjects.first());
    }

    if (window) {
        // print fps
        bool logFps = qgetenv("TMAP_ENABLE_FPS").toInt();
        if (logFps) {
            static qint64 sec = QDateTime::currentMSecsSinceEpoch();
            FpsCalculator* fps = new FpsCalculator(this);
            connect(window, &QQuickWindow::frameSwapped, fps, &FpsCalculator::recalculateFPS, Qt::DirectConnection);
            connect(fps, &FpsCalculator::fpsChanged, [&](int fps) {
                qint64 now = QDateTime::currentMSecsSinceEpoch();
                if (now - sec >= PRINT_FPS_INTERVAL_MS) {
                    //T_PERF_LOG() << "[Perf]current fps :" << fps;
                    sec = now;
                }
            });
        }
        //connect(window, &QQuickWindow::frameSwapped, this, &TmapController::onFrameSwapped);

        emit applicationWindowCreated(window);
#ifndef QT_NO_DEBUG_OUTPUT
        qDebug(sys) << "after emit applicationWindowCreated:" << window;
#endif

#ifdef QT_DEBUG
        // Ubuntu에서는 아래 시그널을 받지 못하기 때문에 applicationWindowCreated 이후에 호출해 준다.
        // 아래 시그널을 받은 후 경로 이어하기 또는 최근 목적지 그리드뷰를 표출하기 때문
        // 19.05.24 ymhong
        emit clientStatusGuiSenseReady();
#endif
    } else {
        m_tmapProperty->setTmapNaviStartStatus(0); // set start status with failed to start
        qWarning("Application Window is not created!");
    }
}

void TmapController::onPlatformDimmingStatusChanged(bool dimming)
{
    if (!FEATURE_DAYNIGHT_BY_GPSTIME) {
        qDebug() << "TmapController::setSunState #3";
        setSunState(dimming ? SunNight : SunDay);
    }
}

void TmapController::onplatformDialPOICallStatusChanged(char DialPOICallStatus) {
    emit dialPOICallStatusChanged(DialPOICallStatus);
}

void TmapController::onplatformCallServiceStatusChanged(char CallServiceStatus) {
    emit callServiceStatusChanged(CallServiceStatus);
}

void TmapController::onPlatformIsHFPAvailable(bool PhoneCallStatus) {
    emit isHFPAvailable(PhoneCallStatus);
}

void TmapController::updateMapData()
{
    VSM_Char mapVersion[32];
    if (VSMMap_GetMapVersion(mapVersion, sizeof(mapVersion))) {
        QString mver(mapVersion);
        m_tmapProperty->setTmapMapVersion(mver);
    }

    qDebug() << "mapVersion:" << m_tmapProperty->getTmapMapVersion();

    VSM_Int32 sdiVersion = 0;
    if(VSMNavi_GetSDIDBVersion(sdiVersion)){
        QString sdiver = QString::number(sdiVersion);
        m_tmapProperty->setTmapSDIVersion(sdiver);
    }
    qDebug() << "sdiVersion:" << m_tmapProperty->getTmapSDIVersion();

    emit mapDataChanged();
}

void TmapController::onUpdateCommitted(bool success)
{
    Q_UNUSED(success)

    disconnect(m_packageManager, &PackageManager::updateCommitted, this, 0);

    deleteVrSymLink();
    initEngine();
}

void TmapController::onLocationUpdated(LocationDataPtr location)
{
    static int lastUpdate = 0;

    // check whether location data is valid or not.
    if (location->dataStatus() != LocationData::ACTIVE) {
        return;
    }

    QDateTime dateTime = location->dateTime();

    qDebug() << "[Time][TmapController]utc: " << dateTime << " local: " << DateTimeUtil::toLocalDateTime(dateTime);
    //qDebug() << "[GPS][TmapController]gpsMode: " << location->gpsMode() << " accuracy: " << location->accuracy();

    if (!dateTime.isValid()) {
        return;
    }

    QTime now = dateTime.time();
    int min = now.hour() * 60 + now.minute();

    QDateTime timeset[4];
    SunState s = m_sunState;

    if (location->gpsMode() == LocationData::PHONE_GPS) {
#ifdef SWITCH_TO_NIGHTMODE_WHEN_NO_GPS
        bool isShadeAreaNow = location->accuracy() <= -99;
        if (mIsShadeArea) {
            if (!isShadeAreaNow) {
                mIsShadeArea = false;
                s = sunStateConvert(DayNightDetector::detect(dateTime,
                  location->longitude(),
                  location->latitude(),
                  timeset
                  ));
            }
        }
        else {
            if (isShadeAreaNow) {
                s = NavigationController::SunNight;
                mIsShadeArea = true;
            }
        }
#endif
    }
    else { // DR_GPS
        // detect every minute
        if (lastUpdate != min) {
            SunState ts = sunStateConvert(DayNightDetector::detect(dateTime,
              location->longitude(),
              location->latitude(),
              timeset
              ));
#ifdef SWITCH_TO_NIGHTMODE_WHEN_NO_GPS
            if (!mIsShadeArea) {
                s = ts;
            }
#else
            s = ts;
#endif
            lastUpdate = min;
        }
    }

    setSunState(s);

    if ( timeset[0].isValid() ) {
        timeset[0] = DateTimeUtil::toLocalDateTime(timeset[0]);
        timeset[1] = DateTimeUtil::toLocalDateTime(timeset[1]);
        timeset[2] = DateTimeUtil::toLocalDateTime(timeset[2]);
        timeset[3] = DateTimeUtil::toLocalDateTime(timeset[3]);

        QList<QDateTime> sets;
        sets << timeset[0] << timeset[1] << timeset[2] << timeset[3];

        emit setDaynightTimeset(sets); // sunrise,sunset, civStart, civEnd
    }
}

void TmapController::initMapUpdate()
{
    if (m_packageManager->checkUpdatePackageInstalled()) {
        connect(m_packageManager, &PackageManager::updateCommitted,
                this, &TmapController::onUpdateCommitted,
                Qt::QueuedConnection);

        m_packageManager->commitUpdatePackage();
        return;
    }

    initEngine();
    if(m_gpsService)
    {
        m_gpsService->InitNaviEngine();
    }
}

void TmapController::initEngine()
{
    createVrSymLink();

    if (!m_packageManager->loadInstalledPackage()) {
        m_tmapProperty->setErrorMap(PackageContainer::error);
        emit engineInitialized(false);
        return;
    }

    m_tmapProperty->setTmapLocalMapVersion(m_packageManager->mapVersion());
    m_tmapProperty->setTmapLocalNetworkVersion(m_packageManager->localNetworkVersion());
    m_tmapProperty->setTmapLocalPoiVersion(m_packageManager->poiVersion());

    SKT::RPnSearch* pRPnSearch = SKT::RPnSearch::sharedInstance();
    connect(pRPnSearch, &SKT::RPnSearch::Init, this, &TmapController::onRPnSearchInitialized, Qt::QueuedConnection);
    pRPnSearch->setDeviceId(getDeviceId());
    QString nddsUrl(TmapHostNames::getHostName(TmapHostNames::HOST_NDDS_INDEX));
    int nddsPort     = TmapHostNames::getHostPort(TmapHostNames::HOST_NDDS_INDEX);
    int nddsType     = (int)TmapHostNames::getHostType(TmapHostNames::HOST_NDDS_INDEX);
    bool commerce    = nddsType == TmapHostNames::HOST_PRD || nddsType == TmapHostNames::HOST_AVNEXT_PRD;
    pRPnSearch->init(this, m_packageManager->localNetworkPath(), nddsUrl, nddsPort, commerce);

    VSM::Map* map = VSM::Map::sharedInstance();
    connect(map, &VSM::Map::initialized, this, &TmapController::onVSMInitialized, Qt::QueuedConnection);

    auto screenSize = SKT::UILayout::layoutSize(m_uiLayout);
    auto iniPath = QDir::homePath() + QChar('/') + QLatin1String("tmap_rsm_ui_dbg.ini");
    QFile file( iniPath );
    if (file.exists()) {
        QSettings settings( iniPath, QSettings::IniFormat );
        auto resolution = settings.value(QStringLiteral("resolution")).toString();
        if (resolution == QLatin1String("1920x720")) {
            m_uiLayout = SKT::UILayout::L1920x720;
            screenSize = SKT::UILayout::layoutSize(m_uiLayout);
        }
        else if (resolution == QLatin1String("820x1024")) {
            m_uiLayout = SKT::UILayout::L820x1024;
            screenSize = SKT::UILayout::layoutSize(m_uiLayout);
        }
        else if (resolution == QLatin1String("800x480")) {
            m_uiLayout = SKT::UILayout::L800x480;
            screenSize = SKT::UILayout::layoutSize(m_uiLayout);
        }
    }

    // read VSM app params
    int  vsmServer = (int)TmapHostNames::getHostType(TmapHostNames::HOST_STREAMMAP_INDEX);
    qDebug() << "vsm server type:"<<vsmServer;
    QString vsmConfigId;
    QString vsmConfigVer;
    QString vsmIniPath = SKT::Environment::getPreferencePath() + QDir::separator() + QStringLiteral("vsm_app.ini");
    QFile vsmIniFile( vsmIniPath );
    qputenv("MAPVIEW_GL_MULTISAMPLES",QByteArray().setNum(-1));
    if (vsmIniFile.exists()) {
        QSettings vsmIni(vsmIniPath, QSettings::IniFormat);
        vsmIni.beginGroup(QStringLiteral("VsmConfig"));
        auto configId = vsmIni.value(QStringLiteral("AppConfigId"));
        if (configId.isValid())
            vsmConfigId = configId.toString();

        auto configVer = vsmIni.value(QStringLiteral("AppConfigVer"));
        if (configVer.isValid())
            vsmConfigVer = configVer.toString();

        // set fps from ini
        #ifdef MAPVIEW_FPS_TEST
        vsmIni.endGroup();
        vsmIni.beginGroup(QStringLiteral("VsmTest"));
        int useFps = vsmIni.value(QStringLiteral("UseFps")).toInt();
        if (useFps > 0)
            qputenv(QStringLiteral("TMAP_ENABLE_FPS").toLatin1().constData(), QByteArray().setNum(1));

        if (vsmIni.contains(QStringLiteral("GLAntiAliasing"))) {
           int ms = vsmIni.value(QStringLiteral("GLAntiAliasing")).toInt() % 4;
            qputenv("MAPVIEW_GL_MULTISAMPLES",QByteArray().setNum(ms));
        }

        int useFst = vsmIni.value(QStringLiteral("UseFst")).toInt();
        if (useFst > 0)
            qputenv(QStringLiteral("TMAP_ENABLE_FST").toLatin1().constData(), QByteArray().setNum(1));

       #endif
    }

    // initialize VSM engine
    map->init(screenSize.width(), screenSize.height(),
              m_packageManager->mapPath(),
              SKT::Environment::getOnlineMapPath() + QDir::separator() + QLatin1String("vsm"),
              SKT::Environment::getRoutePath(),//DataPath(), // route cache folder get moved to online_streaming due to small disk capcity by lckxx 2018.10.01
              VSM_CONFIGURATION_EMBEDDED,
              vsmServer,
              vsmConfigId,
              vsmConfigVer);
}

void TmapController::destroyEngine()
{
    SKT::RPnSearch::sharedInstance()->destroy();
    VSM::Map::sharedInstance()->destroy();
    //MarkerManager::destroyInstance();
}

void TmapController::setupAudio()
{
    // register audio handler
    m_audioHelper = new RGAudioHelper(m_platformController);
    VSMNavi_SetAudioInterface(m_audioHelper);
    VSMNavi_InitEngineSound();

    // relay signals
    connect(m_audioHelper, &RGAudioHelper::navigationVolumeChanged, this, &TmapController::navigationVolumeChanged);
    connect(m_audioHelper, &RGAudioHelper::muteChanged, this, &TmapController::muteStateChanged);

    // relay signals for TmapProperty
    connect(m_audioHelper, &RGAudioHelper::navigationVolumeChanged, m_tmapProperty, &TmapProperty::setTmapVolume);
    connect(m_audioHelper, &RGAudioHelper::muteChanged, m_tmapProperty, &TmapProperty::setTmapIsMute);
}

void TmapController::setupMapSwitcher()
{
    VSM::MapSwitcher *switcher = VSM::Map::sharedInstance()->mapSwitcher();

    QResource r(":/assets/vsm_public.pem");
    if (r.isValid()) {
        switcher->setPublicKey(reinterpret_cast<const char*>(r.data()), r.size());
    }

    // attach handler for map switching
    connect(switcher, &VSM::MapSwitcher::onlineMapChecked, [=] (bool success) {
        if (success && m_networkOnline) {
            switcher->switchTo(VSM::MapSwitcher::MapTypeOnline);
            // setOnline(true);
            // why should network state be dependent on the vsm's online state?
            //  I think the role have to be separeted, because any application's network state must be same with system's one
            // and there could be another jobs based on the network.
            // 20188.10.18 by lckxx
        } else {
            switcher->switchTo(VSM::MapSwitcher::MapTypeOffline);
            // setOnline(false);
        }
    });

    // update version whenever map is switched
    connect(switcher, &VSM::MapSwitcher::mapTypeChanged, this, &TmapController::updateMapData);

    updateMapData();
}

void TmapController::initUI()
{
    qDebug() << "[MultiLang]TmapController::initUI index: " << (int)m_languageIndex;

    // RSM UI
    SKT::RPnSearch* pRPnSearch = SKT::RPnSearch::sharedInstance();
    VSM::AddressManager *addressManager = VSM::Map::sharedInstance()->addressManager();
    m_searchManager = new SearchManager();

    QString crossImagePath = m_packageManager->crossImagePath();

    QMap<QString,QMessageLogger::CategoryFunction> logCats;
    logCats["sys"] = sys;
    logCats["ui"] = ui;

    // a test for received gadget signals before ui module initialization
//    QVariantMap gadgetSignal;
//    gadgetSignal["isValid"] = true;
//    gadgetSignal["gadgetType"] = (int)UIConstants::GadgetXXL;
//    gadgetSignal["gadgetActivate"] = true;
//    m_gadgetSignal = gadgetSignal;

    RsmUi_Initialize(
        m_qmlEngine,
        logCats,
        sys_log_handler,
        QVariantMap{
            {"layout", m_uiLayout},
            {"resDirPath", Environment::getResPath()},
            {"dbDirPath", Environment::getDatabasePath()},
            {"crossImagePath", crossImagePath},
            {"gpsService", QVariant::fromValue(m_gpsService)},
            {"simulator", QVariant::fromValue(m_simulator)},
            {"rpnSearch", QVariant::fromValue(pRPnSearch)},
            {"addressManager", QVariant::fromValue(addressManager)},
            {"searchManager", QVariant::fromValue(m_searchManager)},
            {"controller", QVariant::fromValue(this)},
            {"navigationProperty", QVariant::fromValue(m_tmapProperty)},
            //{"markerManager", QVariant::fromValue(MarkerManager::sharedInstance())},
            {"preferenceDirPath", Environment::getPreferencePath()},
            {"updateManager", QVariant::fromValue(m_packageManager)},
            {"isEngineInit", QVariant::fromValue(true)},
            {"languageIndex", QVariant::fromValue(m_languageIndex)},
            {"colorState", QVariant::fromValue(m_colorState)},
            {"mexMode", QVariant::fromValue(m_mexMode)},
            {"gadgetSignal", QVariant::fromValue(m_gadgetSignal)},
            {"gpsRfSignal", QVariant::fromValue( m_gpsRfAntennaStatus)},
        }
    );
}

void TmapController::initGuideUI()
{
//    // RSM UI  // rpn should not be ran on engine init failure by lckxx 2017.10.18, artf261697
//    SKT::RPnSearch* pRPnSearch = SKT::RPnSearch::sharedInstance();
//    VSM::AddressManager *addressManager = VSM::Map::sharedInstance()->addressManager();
//    m_searchManager = new SearchManager();

    QString crossImagePath = m_packageManager->crossImagePath();

    QMap<QString,QMessageLogger::CategoryFunction> logCats;
    logCats["sys"] = sys;
    logCats["ui"] = ui;

    RsmUi_Initialize(
        m_qmlEngine,
        logCats,
        sys_log_handler,
        QVariantMap{
            {"layout", m_uiLayout},
            {"resDirPath", Environment::getResPath()},
            {"dbDirPath", Environment::getDatabasePath()},
            {"crossImagePath", crossImagePath},
            {"gpsService", QVariant::fromValue(m_gpsService)},
            {"simulator", QVariant::fromValue(m_simulator)},
//                {"rpnSearch", QVariant::fromValue(pRPnSearch)},
//                {"addressManager", QVariant::fromValue(addressManager)},
//                {"searchManager", QVariant::fromValue(m_searchManager)},
            {"controller", QVariant::fromValue(this)},
            {"navigationProperty", QVariant::fromValue(m_tmapProperty)},
            //{"markerManager", QVariant::fromValue(MarkerManager::sharedInstance())},
            {"preferenceDirPath", Environment::getPreferencePath()},
            {"updateManager", QVariant::fromValue(m_packageManager)},
            {"languageIndex", QVariant::fromValue(m_languageIndex)},
            {"isEngineInit", QVariant::fromValue(false)},
        }
    );
}

bool TmapController::setMapViewMode(int viewMode){
    emit mapViewModeChanged(viewMode);
    return true;
}
bool TmapController::setViewLevel(int level, int subLevel){
    emit viewLevelChanged(level,subLevel);
    return true;
}
bool TmapController::setViewLevel(ViewLevelInfo level){
    emit viewLevelChanged(level.mLevel,level.mSubLevel);
    return true;
}
bool TmapController::setTrafficInfoScreen(bool isShow){
    emit trafficInfoScreenChanged(isShow);
    return true;
}
bool TmapController::moveToCurrentPosition(){
    emit moveToCurrentPositionOccur();
    return true;
}
bool TmapController::goViaPointEditScreen(){
    emit goViaPointEditScreenOccur();
    return true;
}
bool TmapController::goRouteSearchScreen(){
    emit goRouteSearchScreenOccur();
    return true;
}

void TmapController::requestFreeTextSearchResult(
    const QString& searchWords,
     int scope,
     int searchType,
     int category,
     int destIndex,
     int fromIndex,
     int maxCnt ) {
    emit requestFreeTextSearchResultOccur(searchWords, scope, searchType, category, destIndex, fromIndex, maxCnt);
}

void TmapController::requestSearchAddress(const QVariant& address)
{
    emit requestSearchAddressOccur(address);
}

void TmapController::requestSDSSearchAddress(const QVariant& address)
{
    emit requestSDSSearchAddressOccur(address);
}

void TmapController::requestLocationDetail(const TS_ROUTE_POS& location)
{
    // get just address using by Address API
    VSM::AddressRequest req(location.tp.x, location.tp.y);
    VSM::AddressReply *reply = VSM::Map::sharedInstance()->addressManager()->post(req);
    if (reply) {
        connect(reply, &VSM::AddressReply::finished, [this,reply]() {
            QString address;
            if (!reply->error()) {
                address = reply->address();
                qDebug() << "location detail by xy: "<< address;
            }
            emit responsedLocationDetail(QVariantMap({{QStringLiteral("address"),address}}));
            reply->deleteLater();
        });
    }
    else {
        emit responsedLocationDetail(QVariant());
    }
}

void TmapController::goPoiDetailScreen(const QVariant& detail)
{
// get just address using by Address API
    QVariantMap locationInfo = detail.toMap();
    int tpX = locationInfo[QStringLiteral("x")].toInt();
    int tpY = locationInfo[QStringLiteral("y")].toInt();
    // QString poiName = locationInfo[QStringLiteral("name")].toString();
    QString address = locationInfo[QStringLiteral("address")].toString();
    // QString phone = locationInfo[QStringLiteral("phone")].toString();

    if (address.isEmpty()) {
        VSM::AddressRequest req(tpX, tpY);
        VSM::AddressReply *reply = VSM::Map::sharedInstance()->addressManager()->post(req);
        if (reply) {
            connect(reply, &VSM::AddressReply::finished, [this,reply,locationInfo]() {
                QVariantMap details = locationInfo;
                QString address;
                if (!reply->error()) {
                    address = reply->address();
                }
                details[QStringLiteral("address")] = address;
                qDebug() << "go location detail by xy: "<< address;
                emit goPoiDetailScreenOccur(QVariant::fromValue(details));
                reply->deleteLater();
            });
        }
        else {
            emit goPoiDetailScreenOccur(QVariant::fromValue(locationInfo));
        }
    } else {
        emit goPoiDetailScreenOccur(QVariant::fromValue(locationInfo));
    }
}

void TmapController::showDestinationFreeTextSearchInput(const QString& searchString)
{
    emit showDestinationFreeTextSearchInputOccur(searchString);
}

bool TmapController::requestRoute(int option, bool apply){
    emit requestRouteOccur(option, apply);
    return true;
}
bool TmapController::requestRoute(TS_ROUTE_POS pos, int option, bool apply){
    emit requestRouteOccur(pos, option, apply);
    return true;
}
bool TmapController::requestRoute(QList<TS_ROUTE_POS> waypoints, int option, bool apply){
    emit requestRouteOccur(waypoints, option, apply);
    return true;
}
bool TmapController::requestRoute(QVariant waypoints, int option, bool apply)
{
    emit requestRouteOccur(waypoints.value<QRoutePosListPtr>(), option, apply);
    return true;
}

bool TmapController::requestReRoute(int option){
    emit requestReRouteOccur(option);
    return true;
}
bool TmapController::requestReRoute(TS_ROUTE_POS pos, int option){
    emit requestReRouteOccur(pos,option);
    return true;
}
bool TmapController::requestReRoute(QList<TS_ROUTE_POS> waypoints, int option){
    emit requestReRouteOccur(waypoints,option);
    return true;
}

void TmapController::requestAroundSearchParking()
{
    emit requestAroundSearchParkingOccur();
}

void TmapController::requestAroundSearchPatrol(int type)
{
    emit requestAroundSearchPatrolOccur(type);
}

bool TmapController::cancelRoute(){
    emit cancelRouteOccur();
    return true;
}
bool TmapController::setWaypoint(TS_ROUTE_POS pos, int type){
    emit waypointChanged(pos,type);
    return true;
}

bool TmapController::insertWaypoint(TS_ROUTE_POS pos, uint type)
{
    qDebug() << "[INTERFACE] TmapController::insertWaypoint(" << pos.tp.x << "," << pos.tp.y << ", slot:" << type << ")";
    emit waypointInserted(pos, type);
    return true;
}

void TmapController::insertWaypoint(uint slot)
{
    qDebug() << "[INTERFACE] TmapController::insertWaypoint(" << slot << ")";
    emit waypointInsertedFromSelectInMap(slot);
}

void TmapController::insertWaypointFromSelectInMap()
{
    const auto defaultSlot = 1;
    emit waypointInsertedFromSelectInMap(defaultSlot);
}

void TmapController::deleteWaypoints(int firstIndex, int lastIndex) {
    emit waypointsDeleted(firstIndex, lastIndex);
}

bool TmapController::addFavoritesPosition(TS_ROUTE_POS pos){
    emit addFavoritesPositionOccur(pos);
    return true;
}
bool TmapController::rePlayVoiceGuidance(){
    // RGAudio 호출
    if (m_audioHelper) {
        return m_audioHelper->playLastAnnouncement();
    }
    return false;
}
void TmapController::onGearStateChanged(QString state){
    // VSM 호출
}
void TmapController::onLanguageChanged(QString lang){
    emit languageChanged(lang);
}
void TmapController::onResetToFactorySettings(){
    emit resetToFactorySettings();
}
void TmapController::onDateFormatChanged(QString format){
    emit dateFormatChanged(format);
}
void TmapController::onTimeFormatChanged(QString format){
    emit timeFormatChanged(format);
}
void TmapController::onGoToStandby(){
    emit goToStandbyOccur();
}
void TmapController::onGoToRunning(){
    emit goToRunningOccur();
}

bool TmapController::setSDIAlertForMute(bool mute){
    emit sdiAlertForMuteChanged(mute);
    return true;
}
void TmapController::onSearchTextChanged(QString text)
{
    emit searchTextChanged(text);
}
void TmapController::onAutoConpleteTextChanged(QString text)
{
    emit autoCompleteTextChanged(text);
}

void TmapController::applyHardKey(int key, int state)
{
    emit applyHardKeyOccur(key, state);
}

void TmapController::applyDebugKey(int key)
{
    emit debugKeyPressed(key);
}

bool TmapController::VSMWorldToWGS84(int x, int y, double &gx, double &gy){
    return getVSMWorldToWGS84(x,y,gx,gy);
}
bool TmapController::WGS84ToVSMWorld(double gx, double gy, int &x, int &y){
    return getWGS84ToVSMWorld(gx,gy,x,y);
}

bool TmapController::getVSMWorldToWGS84(int x, int y, double &gx, double &gy){
    VSMPoint2f geo;
    VSMPoint2i world;
    world.x = x;
    world.y = y;
    bool bRet = VSMMap_Util_ConvertWorldTo(world, VSM_COORDINATE_WGS84,geo);
    gx = geo.x;
    gy = geo.y;
    return bRet;
}
bool TmapController::getWGS84ToVSMWorld(double gx, double gy, int &x, int &y){
    VSMPoint2f geo;
    geo.x = gx;
    geo.y = gy;
    VSMPoint2i world;
    bool bRet = VSMMap_Util_ConvertToWorld(geo, VSM_COORDINATE_WGS84,world);
    x = world.x;
    y = world.y;
    return bRet;
}
bool TmapController::getMapCenterLocation(TS_ROUTE_POS &location){
    VSM_Int32 x, y;

    VSMMap_GetMapCenter(VSM_NULL, x,y);
    location.tp.x = x;
    location.tp.y = y;
    VSMMap_GetAreaName(x,y,(VSM_Char*)(location.szName), sizeof(location.szName));
    return true;
}

void TmapController::changeWindowSize(const QSize& size)
{
    QObject *rootObject = m_qmlEngine->rootObjects().first();
    QQuickWindow *window = qobject_cast<QQuickWindow*>(rootObject);
    if(window && (window->width() != size.width() || window->height() != size.height() ))
    {
        window->setWidth(size.width());
        window->setHeight(size.height());
    }
}

void TmapController::changeNddsServer(bool commerce)
{
    // customize hybrid deployment if AVNEXT proxy is in used
    const char* deployment[TmapHostNames::HOST_MAX];
     // get current deployment
     TmapHostNames::getHostDeployment(deployment,TmapHostNames::getHostDeploymentId());
     // customize host only for NDDS
     if (commerce) {
         deployment[TmapHostNames::HOST_NDDS_INDEX] = TmapHostNames::NDDS_PRD_SERVER;
         deployment[TmapHostNames::HOST_NDDS2_INDEX] = TmapHostNames::NDDS_PRD_SERVER2;
     }
     else {
         deployment[TmapHostNames::HOST_NDDS_INDEX] = TmapHostNames::NDDS_DEV_SERVER;
         deployment[TmapHostNames::HOST_NDDS2_INDEX] = TmapHostNames::NDDS_DEV_SERVER;
     }

     // update custom deployment
     TmapHostNames::setHostDeployment(deployment);
#if (0)
     SKT::RPnSearch* rpnSearch = SKT::RPnSearch::sharedInstance();
     rpnSearch->destroy(); // blocking until got completion
     disconnect(rpnSearch,&SKT::RPnSearch::Init,0,0);
     connect(rpnSearch, &SKT::RPnSearch::Init, this, &TmapController::onRPnSearchReInitialized, Qt::QueuedConnection);
     QString nddsUrl(TmapHostNames::getHostName(TmapHostNames::HOST_NDDS_INDEX));
     int nddsPort = TmapHostNames::getHostPort(TmapHostNames::HOST_NDDS_INDEX);
     rpnSearch->init(this, m_packageManager->localNetworkPath(),nddsUrl, nddsPort, commerce);
#endif
}

void TmapController::changeVsmServer(int serverType)
{
      // customize hybrid deployment if AVNEXT proxy is in used
     const char* deployment[TmapHostNames::HOST_MAX];
     // get current deployment
     TmapHostNames::getHostDeployment(deployment,TmapHostNames::getHostDeploymentId());
     // customize host only for NDDS
     if (serverType == 1) { // commercial
         deployment[TmapHostNames::HOST_STREAMMAP_INDEX] = TmapHostNames::STREAM_MAP_PRD_SERVER;
         deployment[TmapHostNames::HOST_RESOURCE_INDEX] = TmapHostNames::RESOURCE_PRD_SERVER;
     }
     else if (serverType == 2 ) { // development
         deployment[TmapHostNames::HOST_STREAMMAP_INDEX] = TmapHostNames::STREAM_MAP_DEV_SERVER;
         deployment[TmapHostNames::HOST_RESOURCE_INDEX] = TmapHostNames::RESOURCE_DEV_SERVER;
     }
     else { // staging
         deployment[TmapHostNames::HOST_STREAMMAP_INDEX] = TmapHostNames::STREAM_MAP_OLD_SERVER;
         deployment[TmapHostNames::HOST_RESOURCE_INDEX] = TmapHostNames::RESOURCE_DEV_SERVER;
     }

     // update custom deployment
     TmapHostNames::setHostDeployment(deployment);

    // app must restart to get effect
}

void TmapController::changeAvNext(int serverType)
{
    m_AvNextServerType = serverType;

    TmapHostNames::eHostDeploymentId deployId = TmapHostNames::getHostDeploymentId();
    // change deployment
    if (serverType == 0)  { // AVNEXT PRD PROXY
        // set deployment id
        deployId = TmapHostNames::HOST_DEPLOY_RELEASE;

    }
    else if (serverType == 1) { // AVNEXT STG PROXY
        deployId = TmapHostNames::HOST_DEPLOY_QC_AVNEXT;
    }
    else { // OFF AVNEXT
          //REM TmapHostNames::resettHostDeployment(); // by sangbong.woo, chogang.lee
          deployId =  TmapHostNames::HOST_DEPLOY_QC;
    }

    // get deployment from preset
    const char* deployment[TmapHostNames::HOST_MAX];
    TmapHostNames::getHostDeployment(deployment,deployId);

    // set deployment as custom
    TmapHostNames::setHostDeployment(deployment);
}


void TmapController::onNddsDualcenterPolicyFetched(const QString& domainName, int portNum)
{
    TmapHostNames::eHostDeploymentId deployId = TmapHostNames::getHostDeploymentId();
    const char* deployment[TmapHostNames::HOST_MAX];
    TmapHostNames::getHostDeployment(deployment,deployId);

    // current ndds servers
    const char* primary   = TmapHostNames::getHostName(TmapHostNames::HOST_NDDS_INDEX);
    const char* secondary = TmapHostNames::getHostName(TmapHostNames::HOST_NDDS2_INDEX);

    bool policyChanged = false;
    // change primary by dualcenter policy
    if (domainName == secondary) {
        // swap primary and secondary
        deployment[TmapHostNames::HOST_NDDS_INDEX]  =  secondary;
        deployment[TmapHostNames::HOST_NDDS2_INDEX] =  primary;
        // set host deployment
        TmapHostNames::setHostDeployment(deployment);
        policyChanged = true;
    }
    else {
        if (strstr(primary, "avnext") != 0) { // assume vnext
            if (domainName.contains("ntmap2") && strstr(secondary,"skt2")) {
                deployment[TmapHostNames::HOST_NDDS_INDEX]  =  secondary;
                deployment[TmapHostNames::HOST_NDDS2_INDEX] =  primary;
                // set host deployment
                TmapHostNames::setHostDeployment(deployment);
                policyChanged = true;
            }
            else if (domainName.contains("ntmap.") && strstr(secondary,"skt.")) {
                deployment[TmapHostNames::HOST_NDDS_INDEX]  =  secondary;
                deployment[TmapHostNames::HOST_NDDS2_INDEX] =  primary;
                // set host deployment
                TmapHostNames::setHostDeployment(deployment);
                policyChanged = true;
            }
        }
    }

    // apply the server to ndds
    if (policyChanged) {
        const char* nddsHost = TmapHostNames::getHostName(TmapHostNames::HOST_NDDS_INDEX);
        int         nddsPort = TmapHostNames::getHostPort(TmapHostNames::HOST_NDDS_INDEX);
        DeErrorCode success = NDDS_SetTmapHost(nddsHost,nddsPort);
        if (success != DE_ERROR_SUCCESS) {
            qDebug() << "failed to set ndds server:"<<success;
        }
    }

    mServerDualcenterPolicySuccess = true; // navigation should run into connectivity

}

void TmapController::onNddsServerPolicyFinished()
{
    if (mServerDualcenterPolicySuccess == false) {
        // notify that navigation should run into local
        qDebug() << "[CHANNEL] ndds server policy failed";
    } else {
        qDebug() << "[CHANNEL] ndds server policy succeeded";
    }
    emit nddsServerPolicyFinished(mServerDualcenterPolicySuccess);
}


void TmapController::platformRemountMedia(int mountType, const QString& srcPath, const QString& dstPath)
{
    #if 1
        connect(m_platformController, &PlatformController::platformRemountDone, [this](bool success) {
            emit platformMediaRemounted(success);
            disconnect(m_platformController, &PlatformController::platformRemountDone,0,0);
        });
        bool success = false;
        QMetaObject::invokeMethod(m_platformController, "remountCommand",
         Qt::QueuedConnection
         , Q_RETURN_ARG(bool, success)
         , Q_ARG(uint, mountType)
         , Q_ARG(const QString&, srcPath)
         , Q_ARG(const QString&, dstPath));

    #else // deprecated which call directaly to platform
    bool success = m_platformController->remountCommand(mountType, srcPath, dstPath);
    emit platformMediaRemounted(success);
    #endif
}

void TmapController::onPlatformRemountStatus(bool success)
{
    qDebug() << "tmcontroller,, platform remount status: " << success;
}

void TmapController::onClientStatusGuiSenseReady()
{
    emit clientStatusGuiSenseReady();
}

void TmapController::onPlatformZone3PullUpListStateChanged(uint state)
{
    // Third Party navigation should call "setEditFieldText"
    // when option button was pressed which trigger this zone3

    qDebug() << "TmapController::onPlatformZone3PullUpListStateChanged(" << state << ")";

    if (state == 0) { // Closed
        emit updateEditFieldText();
    }
}

void TmapController::onPlatformfocusConfigChanged(char type)
{
    if (type == 0) {
        qDebug() << "mCswFilter.setType(CswFilter::IGNORE_KEYBOARD)";
        mCswFilter.setType(CswFilter::IGNORE_KEYBOARD);
    } else {
        qDebug() << "mCswFilter.setType(CswFilter::FORWARD_KEYBOARD)";
        mCswFilter.setType(CswFilter::FORWARD_KEYBOARD);
    }
}

void TmapController::onPlatformNaviGadgetFocusStatusChanged(uint instanceId, bool focus)
{
    emit platformNaviGadgetFocusStatusChanged(instanceId, focus);
}

void TmapController::onPlatformNaviGadgetFocusInteractionTypeChanged(uint instanceId, uint interactionType)
{
    emit platformNaviGadgetFocusInteractionTypeChanged(instanceId, interactionType);
}

void TmapController::setLanguageId(int id)
{
    onPlatformLanguageIndexStatusChanged(id);
}

void TmapController::onPlatformLanguageIndexStatusChanged(char languageIndex)
{
    qDebug() << "[MultiLang]TmapController::onPlatformLanguageIndexStatusChanged index: " << (int)languageIndex;

    m_languageIndex = languageIndex;

    emit languageIndexStatusChanged(languageIndex);
}

void TmapController::onPlatformColorStateStatusChanged(char colorState)
{
    qDebug() << "TmapController::onPlatformColorStateStatusChanged  colorState:" << (int)colorState;

    m_InitVSMView = true;
    m_colorState = colorState;

    emit colorStateStatusChanged(colorState);
}

void TmapController::onPlatformMexModeStatusChanged(char mexMode)
{
    qDebug() << "TmapController::onPlatformMexModeStatusChanged  maxMode:" << (int)mexMode;

    m_mexMode = mexMode;
    emit mexModeStatusChanged(mexMode);
}

void TmapController::onPlatformPrivateDataEncryptionRegistered(QObject* interface)
{
    m_cipherService = new CipherService(dynamic_cast<ComBoschAutomotiveProxyPrivateDataEncryptionInterface*>(interface));
    //m_cipherService = new AesCipherService();
}

//TMCRSM-2446:VR로부터 음성안내on/off 명령을 받는 인터페이스
void TmapController::setVoiceGuidance(bool active)
{
    emit setVoiceGuidanceOccur(active);
}

void TmapController::setAlramGuidance(bool active)
{
    emit setAlramGuidanceOccur(active);
}

void TmapController::onHmiStateChanged(uint hmiState)
{
    emit hmiStateChanged(hmiState);
}

void TmapController::onCrossImageVisibleChanged(bool visible)
{
    emit crossImageVisibleChanged(visible);
}

void TmapController::setAppHasCswFocus(bool focus)
{
    mAppHasCswFocus = focus;
}

void TmapController::onAppLostCswFocus()
{
    // 아직 설정하지 않음.
    // 이걸 설정하면 Csw key, wheel 이벤트 처리 못함.
    // 19.05.28 ymhong.

    //setAppHasCswFocus(false);
}

void TmapController::handlePointerEvent(int axis, int value)
{
    if (axis == 3) {
        mCswFilter.encoderForwarding(HardKey::HARDKEYCODE_CSW_ROTARY_ENCODER_BUTTON, value);
    } else if (axis == 9) {
        mCswFilter.encoderForwarding(HardKey::HARDKEYCODE_SWC_ROTARY_ENCODER_BUTTON, value);
    }
}

void TmapController::onMapStartedToRender()
{
    qDebug() << "[CTX]TmapController::onMapStartedToRender";
    //mRenderSceneDone = true;
    emit mapStartedToRender();
}

//void TmapController::onFrameSwapped()
//{
//    // RenderScene이 호출 되고 frame swapped까지 되어야 화면에 맵이 표시.
//    // 19.05.27 ymhong.
//    if (mRenderSceneDone) {
//        emit mapStartedToRender();
//        mRenderSceneDone = false;
//    }
//}

void TmapController::onPlatformNaviSignalRFAntennaStatus(uchar u8RFAntenna, uchar u8RFAntennaStatus, uchar u8ConnectionCurrent)
{
    qDebug() << "[RFAntenna]" << "TmapController";
    m_gpsRfAntennaStatus["signaled"] = true;
    m_gpsRfAntennaStatus["u8RFAntenna"] = u8RFAntenna;
    m_gpsRfAntennaStatus["u8RFAntennaStatus"] = u8RFAntennaStatus;
    m_gpsRfAntennaStatus["u8ConnectionCurrent"] = u8ConnectionCurrent;

    emit naviSignalRFAntennaStatusOccur( u8RFAntenna, u8RFAntennaStatus, u8ConnectionCurrent );
}

void TmapController::onPlatformCurrentStreetChanged(RGDataPtr value)
{
    emit currentStreetChanged(value);
}

void TmapController::createVrSymLink() {
    QString vrPath = Environment::getVrPath();
    QString vrFile = Environment::getVrSymLink();
    QFile fileVr;
    fileVr.setFileName(vrFile);
    if (fileVr.exists()) {
        return;
    }
    QDir dirVr(vrPath);
    if (dirVr.exists()) {
        QFile::link(vrPath, vrFile);
    }
}

void TmapController::deleteVrSymLink() {
    QFile fileVr;
    fileVr.setFileName(Environment::getVrSymLink());
    if (fileVr.exists()) {
        fileVr.remove();
    }
}

void TmapController::onPlatformContextSwitchRequested(QVariant args)
{
    qDebug() << "[GadgetSwitch]TmapController::onPlatformContextSwitchRequest";

    emit contextSwitchRequested(args);
}

void TmapController::onCswBackKeyAccepted()
{
    qDebug() << "[CSW] TmapController::onCswBackAccepted()";

    mCswBackAccepted = true;
}

void TmapController::respondTocontextSwitchRequest(QVariant args)
{
    emit contextSwitchResponded(args);
}


}

