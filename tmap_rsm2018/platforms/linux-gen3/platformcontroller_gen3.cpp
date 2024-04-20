#include <QQuickWindow>
#include <QtDBus>
#include <QDebug>
#include <QThread>
#include <QMutex>
#include <QReadWriteLock>
#include <QWaitCondition>
#include <QtConcurrent>
#include <QQmlComponent>
#include <QQmlContext>
#include <QQuickItem>
#include <QQuickView>

#include <QKeyEvent>
#include "MNS_Define.h"
#include "MNS_Interface.h"
#include "VSM.h"
#include "VSM_Navi.h"
#include "VSM_Map.h"
#include "worldpoint.h"
#include "VSM_Util.h"
#include "platformcontroller_gen3.h"
#include "NavigationExtCentralLock.h"
#include "tmapcontroller.h"
#include "layermanager.h"
#include "interface/navigationext/navigationextserviceinterface.h"
#include "interface/HMIZone13/hmiZoneDataInterface.h"
#include "interface/HMIZone13/hmiZonetype.h"
#include "guicontrolpropertyobserver.h"
#include "navigationextpropertyobserver.h"
#include "navigationprofilepropertyobserver.h"
#include "hmizonedatapropertyobserver.h"
#include "interface/guicontrol/guicontrolinterface.h"
#include "interface/server/navigation/navigationserver.h"
#include "interface/server/navigationSDS/navigationsdsservice.h"
#include "interface/server/navigationDataUpdate/navigationdataupdateservice.h"
#include "interface/deadreckoning/drpositiontype.h"
#include "interface/server/navigation/navigationservicetype.h"
#include "interface/carconfiguration/carconfigurationinterface.h"
#include "interface/lifecyclemanager/lifecyclemanagerinterface.h"
#include "interface/privateDataEncrypt/privateDatainterface.h"
#include "interface/privateDataEncrypt/privateDataEncryptiontype.h"
#include "util/textdecoder.h"
#include "interface/server/navigationDataUpdate/navigationdataupdateservicetype.h"
#include "interface/securedatacom/securedatacommunicationinterface.h"
// api it takes a screenshot on genivi3
#ifdef BUILD_TARGET
// #include <ilm/ilm_control.h>
#endif

#include "zone1helper.h"
#include "zone3helper.h"

#include "environment.h"

#ifdef USE_SYNCBLOCK
    #include "hmibase/util/DltApplication.h"
    #include "hmibase/gadget/videobuffer/VideoBufferType.h"
    #include "hmibase/util/Timeout.h"
    #include <unistd.h>
    #include <stdlib.h>
    #include <iostream>
    #include <cstring>
    #include "interface/syncBlock/SimpleProducer.h"
    #include "interface/syncBlock/include/hmibase/gadget/syncblock2/ProducerType.h"
#endif
#ifdef USE_MIC
    #include <gst/gst.h>
#endif
#ifdef TEST_DBUS
    #include "dbustest.h"
#endif

#include "hardkey.h"
#include "mapinclusterlayout.h"
#include "datetimeutil.h"

#include "updatemanagerinterface.h"

#define DEFAULT_LAYER_ID        6003
#define DEFAULT_SURFACE_ID      103

#ifdef BUILD_TARGET
    #define DEFAULT_WIDTH           800
    #define DEFAULT_HEIGHT          480
#else
    #define DEFAULT_WIDTH           480
    #define DEFAULT_HEIGHT          640
#endif

#define SERVICE_NAME            "App.Core.AppHmi_Navigation_Ext"
//#define PROFILEMANAGER_NAME     "app.core.ProfileMngr"              // lib
#define PROFILEMANAGER_NAME     "App.Core.AppHmi_Navigation_Ext"    // lxc,
#define AUTOMOTIVEPROXY_SERVICENAME    "com.bosch.AutomotiveProxy"		// AutomotiveProxy 하위 인터페이스들 간의 공용을 위해 이름 변경. 2019.1.16 by hheo
#define CARCONFIGURATION_PATH   "/com/bosch/AutomotiveProxy/CarConfiguration"

#define GUICONTROL_PATH         "/bosch/gui/cm/ai/guicontrol"
#define NAVIGATIONEXT_PATH      "/com/bosch/cm/ai/navigation/ext"
#define PROFILEMANAGER_PATH     "/com/bosch/cm/profileMngr"

//#define HMIZONE_SERVICE_NAME    "App.Core.AppHmi_Master" //lib
#define HMIZONE_SERVICE_NAME    "App.Core.AppHmi_Navigation_Ext" // lxc
#define HMIZONE_SERVICE_PATH    "/bosch/cm/ai/nissan/hmi/zoneDataService"

#define DATAENCRYPT_PATH        "/com/bosch/AutomotiveProxy/PrivateDataEncryption"
#define SECUREDATACOM_PATH      "/com/bosch/AutomotiveProxy/SecureDataCommunication"

#define CLIENT_NAME             "TMAP"

namespace SKT {

    static QReadWriteLock sync;


PlatformController* TmapController::newPlatformController(TmapController *controller) {

    return new PlatformControllerGen3(controller, new LayerManager(controller->qobject()));
}

PlatformControllerGen3::PlatformControllerGen3(TmapController* controller, LayerManager* layermanager)
    : m_controller(controller)
    , m_guiControl(0)
    , m_navigationExt(0)
    , m_navigationServer(0)
    , m_navigationSDSService(0)
    , m_navigationDataUpdateService(0)
    , m_profilemanager(0)
    , m_hmiZoneManager(0)
    , m_carconfiguration(0)
    , m_dataencrypt(0)
    , m_dimmingStatus(false)
    , m_appForeground(false)
    , m_networkOnline(false)
    , m_engineInitialized(false)
    , m_lmInitialized(false)
    #ifdef USE_SYNCBLOCK
//    , m_producer(0)
    , m_parkinggadget_producer(0)
    , m_oilstationgadget_producer(0)
    , m_aqigadget_producer(0)
    , m_contactgadget_producer(0)
    , m_Thread(nullptr)
    , m_ClusterHandleId(0)
    , m_BarGraphInfoTBTIndex(-1)
    , m_BarGraphInfoTBTMaxScale(-1)
    , m_ServiceIndex(-1)
    , m_RecommandType(0)
    , m_tempActivate(false)
    , mNaviAppStatue(0)
    , m_isStartStreamSignal(false)
    , mVideoStreamInfo(0)
    , m_bMapModifyReturn(true)
    #endif
    , m_nPrimaryMax(0)
    , m_nPrimaryMin(0)
    , m_nSecondaryMax(0)
    , m_nSecondaryMin(0)
    , m_bScreenOn(false)
    , m_bNaviInit(false)
    , nNextManeuverIndex(-999)
{
    // // register meta types
    NavigationServiceType::registerMetaTypes();
    NavigationSDSServiceType::registerMetaTypes();
    NavigationDataUpdateServiceType::registerMetaTypes();
    NavigationExtType::registerMetaTypes();
    GuiControlType::registerMetaTypes();
    DrPositionType::registerMetaTypes();
    HMIZonetype::registerMetaTypes();
    LifeCycleManagerType::registerMetaTypes();
    DataEncrypttype::registerMetaTypes();
    qRegisterMetaType<SKT::GadgetInfo>();

    Q_ASSERT(controller);

    // collabo with layer manager
    connect(this, &PlatformControllerGen3::registApplicationWindow,
            layermanager, &LayerManager::onRegisterAppWindow );
    connect(m_controller, &TmapController::setupPopupWindow,
            layermanager, &LayerManager::onSetupPopupWindow);
    connect(layermanager, &LayerManager::layerManagerInitialized,
            this, &PlatformControllerGen3::onLayermanagerInitialized);
    connect(this, &PlatformControllerGen3::setPopupInfo, layermanager, &LayerManager::onSetPopupProperty);
    connect(this, &PlatformControllerGen3::setToastPopupInfo, layermanager, &LayerManager::onSetToastPopupProperty);

    // popup surface가 제대로 등록되지 않은 경우 처리.
    connect(this, &PlatformControllerGen3::registerPopup, layermanager, &LayerManager::onSetupPopup);

    connect(m_controller, &TmapController::stopSendingGadgetImageRequested,
            this, &PlatformControllerGen3::onStopSendingGadgetImageRequested);

    connect(layermanager, &LayerManager::pointerEvent, m_controller, &TmapController::handlePointerEvent);
}

PlatformControllerGen3::~PlatformControllerGen3()
{
#ifdef TEST_DBUS
    if (m_dbusTest) {
        m_dbusTest->cleanUp();
        delete m_dbusTest;
        m_dbusTest = 0;
    }
#endif

    if (m_guiControl) {
        delete m_guiControl;
        m_guiControl = 0;
    }

    if (m_navigationExt) {
        delete m_navigationExt;
        m_navigationExt = 0;
    }

    if (m_navigationServer) {
        delete m_navigationServer;
        m_navigationServer = 0;
    }

    if (m_navigationSDSService) {
        delete m_navigationSDSService;
        m_navigationSDSService = 0;
    }

    if (m_navigationDataUpdateService) {
        delete m_navigationDataUpdateService;
        m_navigationDataUpdateService = 0;
    }

    if (m_carconfiguration) {
        delete m_carconfiguration;
        m_carconfiguration = 0;
    }

    if (m_dataencrypt) {
        delete m_dataencrypt;
        m_dataencrypt = 0;
    }

    if(mNaviAppStatue) {
        delete mNaviAppStatue;
        mNaviAppStatue = 0;
    }

    if(mVideoStreamInfo) {
        delete mVideoStreamInfo;
        mVideoStreamInfo = 0;
    }

#ifdef USE_SYNCBLOCK
    if (m_mapgadgetXXL_producer) {
        delete m_mapgadgetXXL_producer;
        m_mapgadgetXXL_producer = 0;
    }

    if (m_mapgadgetXL_producer) {
        delete m_mapgadgetXL_producer;
        m_mapgadgetXL_producer = 0;
    }
    if (m_mapgadgetL_producer) {
        delete m_mapgadgetL_producer;
        m_mapgadgetL_producer = 0;
    }
    if (m_mapgadgetM_producer) {
        delete m_mapgadgetM_producer;
        m_mapgadgetM_producer = 0;
    }
    if (m_mapgadgetS_producer) {
        delete m_mapgadgetS_producer;
        m_mapgadgetS_producer = 0;
    }

   if (m_parkinggadget_producer) {
       delete m_parkinggadget_producer;
       m_parkinggadget_producer = 0;
   }

   if (m_oilstationgadget_producer) {
       delete m_oilstationgadget_producer;
       m_oilstationgadget_producer = 0;
   }

   if (m_aqigadget_producer) {
       delete m_aqigadget_producer;
       m_aqigadget_producer = 0;
   }

   if (m_contactgadget_producer) {
       delete m_contactgadget_producer;
       m_contactgadget_producer = 0;
   }

#endif
}

void PlatformControllerGen3::init()
{

    // monitor engine init
    connect(m_controller, &TmapController::engineInitialized,
            this, &PlatformControllerGen3::onEngineInitialized);

    connect(m_controller, &TmapController::applicationWindowCreated,
            this, &PlatformControllerGen3::onApplicationWindowCreated);

    connect(m_controller, &TmapController::postKeyEvent,
            this, &PlatformControllerGen3::onKeyEvent);

    connect(m_controller, &TmapController::sendGadgetImage,
            this, &PlatformControllerGen3::onSendGadgetImage);

    connect(m_controller, &TmapController::crossImageVisibleChanged,
            this, &PlatformControllerGen3::onCrossImageVisibleChanged);

    connect(m_controller, &TmapController::mapStartedToRender,
            this, &PlatformControllerGen3::onMapStartedToRender);

    connect(m_controller, &TmapController::doneStartMapVideoStream,
            this, &PlatformControllerGen3::onMapVideoStatus);

    connect(m_controller, &TmapController::contextSwitchResponded,
            this, &PlatformControllerGen3::onContextSwitchResponded);

    qDebug() << "platform init in the thread " << QString("0x%1").arg((quintptr)thread(),QT_POINTER_SIZE * 2, 16, QChar('0'));

#ifdef TEST_DBUS
    m_dbusTest = new DbusTest(this);
#endif
    mNaviAppStatue = new NaviAppStatue();
    mVideoStreamInfo = new StreamInfo();

    m_Waypoints.clear();
    // reserve waypoints, / fixed, crash on something with this,
    m_Waypoints.append(TS_ROUTE_POS());
    m_Waypoints.append(TS_ROUTE_POS());
    m_Waypoints.append(TS_ROUTE_POS());
    m_Waypoints.append(TS_ROUTE_POS());
    m_Waypoints.append(TS_ROUTE_POS());

    m_RouteOption = RouteCriterion_ROUTE_CRITERION_FAST;

#ifdef USE_SYNCBLOCK
    m_mapgadgetXXL_producer = producer::sharedInstance();
    m_mapgadgetXL_producer = producer::sharedInstance();
    m_mapgadgetL_producer = producer::sharedInstance();
    m_mapgadgetM_producer = producer::sharedInstance();
    m_mapgadgetS_producer = producer::sharedInstance();
    m_parkinggadget_producer = producer::sharedInstance();
    m_oilstationgadget_producer = producer::sharedInstance();
    m_aqigadget_producer = producer::sharedInstance();
    m_contactgadget_producer = producer::sharedInstance();
#endif

    // set up dbus
    setupDBus();
}

void PlatformControllerGen3::startService()
{
    #if 1
    QThread* thread = new QThread();
    moveToThread(thread);
    connect(thread, &QThread::started, this, &PlatformControllerGen3::init);
//    connect(thread, &QThread::finished, this, &QObject::deleteLater);

    m_Thread = thread;

    thread->start();
    #else
    init();
    #endif
}

void PlatformControllerGen3::finishService()
{
    if (m_Thread) {
        m_Thread->quit();
        m_Thread->wait();
        m_Thread->deleteLater();
        m_Thread = nullptr;
    }
}

QString PlatformControllerGen3::deviceId() const
{
//    QReadLocker locker(sync); // it is thread safe , doesn't need to synchronize
    return m_DeviceId;
}

/**
 *
 * MUST use remountCommand instead of this function on the LXC
 */

bool PlatformControllerGen3::remount(const QString &path, bool readOnly)
{
    qDebug() << Q_FUNC_INFO;
#ifdef USE_DBUS
    if (m_navigationExt) {
        QDBusPendingReply<bool> reply = m_navigationExt->NaviMethRemountPath(readOnly ? 0 : 1, path);
        reply.waitForFinished();
        if (reply.isError()) {
            return false;
        }
        return reply.value();
    }
    return false;
#else
    Q_UNUSED(path)
    Q_UNUSED(readOnly)
    QMetaObject::invokeMethod(this, "platformRemountStatus",
                              Qt::QueuedConnection,
                              Q_ARG(bool, true));
    return true;
#endif
}

bool PlatformControllerGen3::remountCommand(uint type, const QString &srcPath, const QString &destPath)
{
    qDebug() << Q_FUNC_INFO;
#ifdef USE_DBUS
    qDebug() << "pc, remount command #1";
    if (m_navigationExt) {
        qDebug() << "pc, remount command #2";
        QDBusPendingReply<bool> reply = m_navigationExt->NaviMethSystemCommand(type, srcPath, destPath);
        reply.waitForFinished();
        if (reply.isError()) {
            qDebug() << "pc remountCommand error["
            << reply.error().type()
            << "]:"
            << reply.error().message();
            return false;
        }
        return reply.value();
    }
    return false;
#else
    Q_UNUSED(type)
    Q_UNUSED(srcPath)
    Q_UNUSED(destPath)
    QMetaObject::invokeMethod(this, "platformRemountStatus",
                              Qt::QueuedConnection,
                              Q_ARG(bool, true));
    return true;
#endif
}


bool PlatformControllerGen3::restart()
{
    qDebug() << Q_FUNC_INFO;
#ifdef USE_DBUS
//    4: Finish download the map data
//    13: User request to restart
    if (m_navigationExt) {
        QDBusPendingReply<bool> reply = m_navigationExt->NaviMethRestartSystem(13);
        reply.waitForFinished();
        if (reply.isError()) {
            qWarning() << "NaviMethRestartSystem error";
            return false;
        }
        qDebug() << "NaviMethRestartSystem return=" << reply.value();
        return reply.value();
    }
    return false;
#else
    return true;
#endif
}

bool PlatformControllerGen3::callNumber(const QString &number)
{
//    qDebug() << Q_FUNC_INFO;
//    qDebug() << "callNumbe: " << number << endl;

    bool success = false;
    QString callnum;
    if (m_navigationExt) {
        QDBusPendingReply<bool> reply = m_navigationExt->NaviMethDialPOI(number);
        QDBusPendingCallWatcher* w = new QDBusPendingCallWatcher(reply, this);
        connect(w, &QDBusPendingCallWatcher::finished, [this](QDBusPendingCallWatcher* call){
            QDBusPendingReply<bool> reply = *call;
            bool success = false;
            if (reply.isError()) {
                qWarning() << "NaviMethDialPOI error:" << reply.error().message();
            } else {
                success = reply.value();
            }
            QMetaObject::invokeMethod(this,"platformCallNumberDone", Q_ARG(bool, success));
            call->deleteLater();
        });
    }
    return success;
}

bool PlatformControllerGen3::dimmingStatus() const
{
    QReadLocker locker(&sync);
    return m_dimmingStatus;
}

bool PlatformControllerGen3::networkOnline() const
{
    // QReadLocker locker(&sync);
    return m_networkOnline;
}

bool PlatformControllerGen3::appForeground() const
{
    // QReadLocker locker(&sync);
    return m_appForeground;
}

char PlatformControllerGen3::reverseGearState() const
{
    // QReadLocker locker(&sync);
    return m_ReverseGearState;
}
char PlatformControllerGen3::LanguageIndex() const
{
    // QReadLocker locker(&sync);
    return m_LanguageIndexStatus;
}

char PlatformControllerGen3::DateFormat() const
{
    // QReadLocker locker(&sync);
    return m_DateFormatStatus;
}

char PlatformControllerGen3::TimeFormat() const
{
    // QReadLocker locker(&sync);
    return m_TimeFormatStatus;
}

char PlatformControllerGen3::ColorState() const
{
    // QReadLocker locker(&sync);
    return m_ColorStateStatus;
}

char PlatformControllerGen3::MexMode() const
{
    // QReadLocker locker(&sync);
    return m_MexModeStatus;
}

char PlatformControllerGen3::DialPOICallStatus() const
{
    // QReadLocker locker(&sync);
    return m_dialPOICallStatus;
}

char PlatformControllerGen3::CallServiceStatus() const
{
    // QReadLocker locker(&sync);
    return m_callServiceStatus;
}

uint PlatformControllerGen3::zone3PullUpListState() const
{
    return m_zone3PullUpListState;
}

//carconfiguration method
uint PlatformControllerGen3::carconfigurationValue(uchar key) const {
    if (m_carconfiguration) {
        QDBusPendingReply<QDBusVariant> reply = m_carconfiguration->GetConfigurationValue(key);     //uint
        QDBusPendingCallWatcher* w = new QDBusPendingCallWatcher(reply, const_cast<PlatformControllerGen3*>(this));
         connect(w, &QDBusPendingCallWatcher::finished, [this](QDBusPendingCallWatcher* call) {
             QDBusPendingReply<QDBusVariant> reply = *call;
            if (reply.isError()) {
                qWarning() << "carconfigurationValue error:"<<reply.error().message();
            } else {
                QDBusVariant evSet = reply.value();
                int value = evSet.variant().toInt();
                const_cast<SKT::PlatformControllerGen3*>(this)->gotitCarConfigValue(value);
            }
            call->deleteLater();
        });
    }
}

uint PlatformControllerGen3::doRegisterApp(const QString &AppName) {
    int appId;
    if (m_profilemanager) {
        QDBusPendingReply<uint, QString, bool> reply = m_profilemanager->doRegisterApp(AppName);
        QDBusPendingCallWatcher* w = new QDBusPendingCallWatcher(reply, this);
        connect(w, &QDBusPendingCallWatcher::finished, [&appId](QDBusPendingCallWatcher* call) {
             call->deleteLater();
             QDBusPendingReply<uint, QString, bool> reply = *call;
            if (call->isError()) {
               qWarning() << "doRegisterApp error:"<<call->error().message();
            }
            else {
                QString applName;
                bool reloadData;
                if (reply.count() > 0)
                   appId    = qdbus_cast<uint>(reply.argumentAt(0));
                if (reply.count() > 1)
                   applName = qdbus_cast<QString>(reply.argumentAt(1));
                if (reply.count() > 2)
                   reloadData = qdbus_cast<bool>(reply.argumentAt(2));
                qWarning() << "doRegisterApp:" << appId <<", " << applName << ", " << reloadData;
            }
        });
    }

    return appId;
}

uint PlatformControllerGen3::doSetProfileName(const QString &newName, uchar userID) {
    if (m_profilemanager) {
        QDBusPendingReply<uint> reply = m_profilemanager->do_SetProfileName(newName, userID);
        QDBusPendingCallWatcher* w = new QDBusPendingCallWatcher(reply, this);
        connect(w, &QDBusPendingCallWatcher::finished, [](QDBusPendingCallWatcher* call) {
            if (call->isError()) {
               qWarning() << "do_SetProfileName error";
            }
            call->deleteLater();
        });
    }

    return 0;
}

QByteArray PlatformControllerGen3::getProfileImages() const {
    if (m_profilemanager) {
        QDBusPendingReply<QByteArray> reply = m_profilemanager->getProfileImages();
        QDBusPendingCallWatcher* w = new QDBusPendingCallWatcher(reply, const_cast<PlatformControllerGen3*>(this));
        connect(w, &QDBusPendingCallWatcher::finished, [this](QDBusPendingCallWatcher* call){
            QDBusPendingReply<QByteArray> reply = *call;
            if (reply.isError()) {
                qWarning(sys) << "get profile images failed: " << reply.error().message();
            }
            else {
                QByteArray imges = reply.value();
                const_cast<SKT::PlatformControllerGen3*>(this)->gotitProfileImages(imges);
            }
            call->deleteLater();
        });
    }

    // this return should not be used longer, instead use proper signal
    return QByteArray();
}

QStringList PlatformControllerGen3::getProfileNames() const {
    if (m_profilemanager) {
        QDBusPendingReply<QStringList> reply = m_profilemanager->getProfileNames();
        QDBusPendingCallWatcher* w = new QDBusPendingCallWatcher(reply, const_cast<PlatformControllerGen3*>(this));
        connect(w, &QDBusPendingCallWatcher::finished, [this](QDBusPendingCallWatcher* call){
            QDBusPendingReply<QStringList> reply = *call;
            if (reply.isError()) {
                qWarning(sys) << "get profile names failed: " << reply.error().message();
            }
            else {
                QStringList names = reply.value();
                const_cast<SKT::PlatformControllerGen3*>(this)->gotitProfileNames(names);
            }
            call->deleteLater();
        });
    }

    // this return should not be used longer, instead use proper signal
    return QStringList();
}

types PlatformControllerGen3::getProfileTypes() const {
    if (m_profilemanager) {
        QDBusPendingReply<types> reply = m_profilemanager->getProfileTypes();
        QDBusPendingCallWatcher* w = new QDBusPendingCallWatcher(reply, const_cast<PlatformControllerGen3*>(this));
        connect(w, &QDBusPendingCallWatcher::finished, [this](QDBusPendingCallWatcher* call){
            QDBusPendingReply<types> reply = *call;
            if (reply.isError()) {
                qWarning(sys) << "get profile types failed: " << reply.error().message();
            }
            else {
                types typelist = reply.value();
                const_cast<SKT::PlatformControllerGen3*>(this)->gotitProfileTypes(typelist);
            }
            call->deleteLater();
        });
    }

    // this return should not be used longer, instead use proper signal
    return types();
}


//hmi start
void PlatformControllerGen3::setZoneVisibility(uchar appId, bool visibilityState, uchar displayRegionType) const {
    if (m_hmiZoneManager) {
        QDBusPendingReply<> reply = m_hmiZoneManager->setZoneVisibility(appId, visibilityState, displayRegionType);

#if (0)
        qDebug() << "#1 setZoneVisibility";
        reply.waitForFinished();
        qDebug() << "#2 setZoneVisibility";
        if (reply.isError()) {
            qWarning() << "setZoneVisibility error";
        }
#endif
    }
}

void PlatformControllerGen3::setEditfieldText(uchar appId, const QString &searchBarText, bool isHighLightRequired, uchar highlightStartIndex, uchar highlightCount) const {
    if (m_hmiZoneManager) {
        qDebug() << "PlatformControllerGen3::setEditfieldText(" << searchBarText << ")";
        QDBusPendingReply<> reply = m_hmiZoneManager->setEditfieldText(appId, searchBarText, isHighLightRequired, highlightStartIndex, highlightCount);
#if (0)
        reply.waitForFinished();
        if (reply.isError()) {
            qWarning() << "setEditfieldText error";
        }
#endif
    }
}

void PlatformControllerGen3::setEditFieldUpdate(uchar appId, uchar cursorPosition, const QString &searchbarText) const {
    if (m_hmiZoneManager) {
        qDebug() << "PlatformControllerGen3::setEditFieldUpdate(" << appId << cursorPosition << searchbarText << ")";
        QDBusPendingReply<> reply = m_hmiZoneManager->setEditFieldUpdate(appId, cursorPosition, searchbarText);
#if (0)
        reply.waitForFinished();
        if (reply.isError()) {
            qWarning() << "setEditfieldUpdate error";
        }
#endif
    }
}

void PlatformControllerGen3::setKeyboard(uchar appId, uchar keyboardType, const QString &validCharacters) const {
    if (m_hmiZoneManager) {
        QDBusPendingReply<> reply = m_hmiZoneManager->setKeyboard(appId, keyboardType, validCharacters);
        QDBusPendingCallWatcher* w = new QDBusPendingCallWatcher(reply, const_cast<PlatformControllerGen3*>(this));
        connect(w, &QDBusPendingCallWatcher::finished,[](QDBusPendingCallWatcher* call){
            if (call->isError()) {
                qWarning() << "setKeyboard failed: "<< call->error().message();
            }
            call->deleteLater();
        });
    }
}

void PlatformControllerGen3::setSDSType(uint sdsType, uchar value) const {
//    if (m_hmiZoneManager) {
//        QDBusPendingReply<> reply = m_hmiZoneManager->setSDSType(sdsType, value);
//        reply.waitForFinished();
//        if (reply.isError()) {
//            qWarning() << "setSDSType error";
//        }
//    }
}

#if 0
void PlatformControllerGen3::setZone1(uchar appId, uint zone1Type, ContentInfo zone1ContentDetails, bool updateRegion) const {
    if (m_hmiZoneManager) {
        QDBusPendingReply<> reply = m_hmiZoneManager->setZone1(appId, zone1Type, zone1ContentDetails, updateRegion);
        reply.waitForFinished();
        if (reply.isError()) {
            qWarning() << "setZone1 error";
        }
    }
}

void PlatformControllerGen3::setZone3(uchar appId, uint zone3Type, ContentInfo zone3ContentDetails, bool updateRegion) const {
    if (m_hmiZoneManager) {
        QDBusPendingReply<> reply = m_hmiZoneManager->setZone3(appId, zone3Type, zone3ContentDetails, updateRegion);
        reply.waitForFinished();
        if (reply.isError()) {
            qWarning() << "setZone3 error";
        }
    }
}
#endif

void PlatformControllerGen3::setZone3SubTextInfo(const QStringList &zone3SubText) const {
    if (m_hmiZoneManager) {
        QDBusPendingReply<> reply = m_hmiZoneManager->setZone3SubTextInfo(zone3SubText);
        QDBusPendingCallWatcher* w = new QDBusPendingCallWatcher(reply,qobject_cast<QObject*>(const_cast<PlatformControllerGen3*>(this)));
        connect(w, &QDBusPendingCallWatcher::finished, [](QDBusPendingCallWatcher* call){
             QDBusPendingReply<> reply = *call;
             if (reply.isError()) {
                 qWarning() << "setZone3SubTextInfo error";
             }
             call->deleteLater();
        });
    }
}

PopupProperty PlatformControllerGen3::getPopupProperty() const {
    uint u32LayerID = 0;
    uint u32SurfaceID = 0;
    uint u32Width = 0;
    uint u32Height = 0;
    if(m_guiControl) {
        QDBusReply<uint> reply = m_guiControl->GetPopupProperty(u32SurfaceID, u32Width, u32Height);
        u32LayerID = reply.value();
#ifdef BUILD_TARGET
        qDebug() << "PlatformControllerGen3::getPopupProperty(), layerid: " << u32LayerID << "SurfaceID: " << u32SurfaceID << "Width: " << u32Width << "Height: " << u32Height ;
#endif
    }

    PopupProperty mPopupProperty;
    mPopupProperty.m_LayerId = u32LayerID;
    mPopupProperty.m_SurfaceId = u32SurfaceID;
    mPopupProperty.m_Width = u32Width;
    mPopupProperty.m_Height = u32Height;

    return mPopupProperty;
}

PopupProperty PlatformControllerGen3::getToastPopupProperty() const {
    uint u32LayerID = 0;
    uint u32SurfaceID = 0;
    uint u32Width = 0;
    uint u32Height = 0;
    if(m_guiControl) {
        QDBusReply<uint> reply = m_guiControl->GetToastPopupProperty(u32SurfaceID, u32Width, u32Height);
        u32LayerID = reply.value();
        qDebug() << "PlatformControllerGen3::getToastPopupProperty(), layerid: " << u32LayerID << "SurfaceID: " << u32SurfaceID << "Width: " << u32Width << "Height: " << u32Height ;
    }

    PopupProperty mToastPopupProperty;
    mToastPopupProperty.m_LayerId = u32LayerID;
    mToastPopupProperty.m_SurfaceId = u32SurfaceID;
    mToastPopupProperty.m_Width = u32Width;
    mToastPopupProperty.m_Height = u32Height;

    return mToastPopupProperty;
}


void PlatformControllerGen3::showPopup(const QString &viewname) const {
    emit registerPopup();

    if(m_guiControl) {
        QDBusPendingReply<> reply = m_guiControl->ShowPopup(viewname);
        QDBusPendingCallWatcher* w = new QDBusPendingCallWatcher(reply, qobject_cast<QObject*>(const_cast<PlatformControllerGen3*>(this)));
        connect(w, &QDBusPendingCallWatcher::finished, [](QDBusPendingCallWatcher* call) {
            if (call->isError()) {
                qWarning() << "showPopup error";
            }
            call->deleteLater();
        });
    }
}

void PlatformControllerGen3::hidePopup(const QString &viewname) const {
    if(m_guiControl) {
        QDBusPendingReply<> reply = m_guiControl->HidePopup(viewname);
        QDBusPendingCallWatcher* w = new QDBusPendingCallWatcher(reply, qobject_cast<QObject*>(const_cast<PlatformControllerGen3*>(this)));
        connect(w, &QDBusPendingCallWatcher::finished, [](QDBusPendingCallWatcher *call) {
            if (call->isError()) {
                qWarning() << "hidePopup error";
            }
            call->deleteLater();
        });
    }
}
void PlatformControllerGen3::beepForwading(int type) const {
    if(m_guiControl) {
        QDBusPendingReply<> reply = m_guiControl->BeepForwarding(type);
        QDBusPendingCallWatcher* w = new QDBusPendingCallWatcher(reply, qobject_cast<QObject*>(const_cast<PlatformControllerGen3*>(this)));
        connect(w, &QDBusPendingCallWatcher::finished, [](QDBusPendingCallWatcher* call) {
            if (call->isError()) {
                qWarning() << "beepForwading error";
            }
            call->deleteLater();
        });
    }
}

//hmi end


void PlatformControllerGen3::setDimmingStatus(bool dimming)
{
    if (m_dimmingStatus != dimming) {
        m_dimmingStatus = dimming;
        emit platformDimmingStatusChanged(dimming);
    }
}

void PlatformControllerGen3::setReverseGearState(char gearState)
{
    if (m_ReverseGearState != gearState) {
        m_ReverseGearState = gearState;
        emit platformReverseGearStateChanged(gearState);
    }
}
void PlatformControllerGen3::setLanguageIndex(char languageIndex)
{
    if (m_LanguageIndexStatus != languageIndex) {
        m_LanguageIndexStatus = languageIndex;
        emit platformLanguageIndexStatusChanged(languageIndex);
    }
}
void PlatformControllerGen3::setDateFormat(char dateFormat)
{
    if (m_DateFormatStatus != dateFormat) {
        m_DateFormatStatus = dateFormat;
        emit platformDateFormatStatusChanged(dateFormat);
    }
}
void PlatformControllerGen3::setTimeFormat(char timeFormat)
{
    if (m_TimeFormatStatus != timeFormat) {
        m_TimeFormatStatus = timeFormat;
        emit platformTimeFormatStatusChanged(timeFormat);
    }
}

void PlatformControllerGen3::setColorState(char colorState)
{
    qDebug() << "!!! PlatformControllerGen3::setColorState: " << (int)colorState;

    if (m_ColorStateStatus != colorState) {
        m_ColorStateStatus = colorState;
        emit platformColorStateStatusChanged(colorState);
    }
}

void PlatformControllerGen3::setMexMode(char mexMode)
{
    qDebug() << "!!!PlatformControllerGen3::setMexMode " << (int)mexMode;

    if (m_MexModeStatus != mexMode) {
        m_MexModeStatus = mexMode;
        emit platformMexModeStatusChanged(mexMode);
    }
}

void PlatformControllerGen3::setDialPOICallStatus(char dialPOICallStatus)
{
    if (m_dialPOICallStatus != dialPOICallStatus) {
        m_dialPOICallStatus = dialPOICallStatus;
        emit platformDialPOICallStatusChanged(dialPOICallStatus);
    }
}

void PlatformControllerGen3::setCallServiceStatus(char callServiceStatus)
{
    if (m_callServiceStatus != callServiceStatus) {
        m_callServiceStatus = callServiceStatus;
        emit platformCallServiceStatusChanged(callServiceStatus);
    }
}

void PlatformControllerGen3::setZone3PullUpListState(uint zone3PullUpListState)
{
    qDebug() << "PlatformControllerGen3::setZone3PullUpListState(" << zone3PullUpListState << ")";
    if (m_zone3PullUpListState != zone3PullUpListState) {
        m_zone3PullUpListState = zone3PullUpListState;
        emit platformZone3PullUpListStateChanged(zone3PullUpListState);
    }
}

void PlatformControllerGen3::setUpdateLockStates(NXLockStateInfoList lockStateInfoList)
{
    if (m_lockStateInfo != lockStateInfoList) {
        m_lockStateInfo = lockStateInfoList;
        emit platformUpdateLockStatesChanged(lockStateInfoList);
    }
}

void PlatformControllerGen3::setDateTimeDetailInfo(NXDateTimeDetailInfo dateTimeDetailInfo)
{
    DateTimeUtil::setNaviDateTimeDetailInfo(dateTimeDetailInfo);
}

void PlatformControllerGen3::setAppForeground(bool foreground)
{
    if (m_appForeground != foreground) {
        m_appForeground = foreground;

        if (!m_appForeground) {
            m_gadgetStopped = false;
        }
        emit platformAppForegroundChanged(foreground);
        //emit setPauseMap(!foreground);
    }
}

void PlatformControllerGen3::setNetworkOnline(bool online)
{
    qDebug() << "setNetworkOnline= " << online;
    if (m_networkOnline != online) {
        m_networkOnline = online;
        emit platformNetworkOnlineChanged(online);
    }
}

void PlatformControllerGen3::setHmiState(uint hmiState)
{
    if(hmiState == 2) //ACC on == 2
    {
        if(!m_bScreenOn)
        {
            if(m_bNaviInit)
            {
                if (m_navigationServer)
                    m_navigationServer->setNavStatus(NavStatus::NavStatus_NAVSTATUS_IDLE);
            }
        }
        m_bScreenOn = true;
    }

    emit platformHmiStateChanged(hmiState);

    QMetaObject::invokeMethod(m_controller, "onHmiStateChanged", Q_ARG(uint,hmiState));
}

void PlatformControllerGen3::setAppStateNavigation(uchar appStateNavigation)
{
    onNaviSignalAppStateNavigation(appStateNavigation);
}

void PlatformControllerGen3::setupDBus()
{
#ifdef USE_DBUS
    qDebug() << "setupDBus() #1";

    QDBusConnection c = QDBusConnection::sessionBus();
    if (!c.isConnected()) {
        qWarning("dbus not connected.");
        return;
    }

     // 1. Navigation Ext Service
     // 2. GuiControl
    // If it is registered already, setup right away
    if (c.interface()->isServiceRegistered(SERVICE_NAME)) {
        onServiceRegistered();
    } else {
        qDebug() << "Waiting for service:" << SERVICE_NAME;
        QDBusServiceWatcher *serviceWatcher = new QDBusServiceWatcher(SERVICE_NAME,
                c, QDBusServiceWatcher::WatchForRegistration, this);
        connect(serviceWatcher, &QDBusServiceWatcher::serviceRegistered, this, &PlatformControllerGen3::onServiceRegistered);
    }

    // 3. Profile Manager
    QDBusConnection profileMangerconn = QDBusConnection::sessionBus();
    if (!profileMangerconn.isConnected()) {
        qWarning("dbus not connected.");
        return;
    }

    // If it is registered already, setup right away
    if (profileMangerconn.interface()->isServiceRegistered(PROFILEMANAGER_NAME)) {
        qDebug() << "pm already registered!";
        onProfileManagerRegistered();
    } else {
        qDebug() << "pm Waiting for service:" << PROFILEMANAGER_NAME;
        QDBusServiceWatcher *profileMangerWatcher = new QDBusServiceWatcher(PROFILEMANAGER_NAME,
                profileMangerconn, QDBusServiceWatcher::WatchForRegistration, this);
        connect(profileMangerWatcher, &QDBusServiceWatcher::serviceRegistered, this, &PlatformControllerGen3::onProfileManagerRegistered);
    }

    // 4. AGW CarConfiguration
    // Automotive.CarConfiguration
    QDBusConnection carConfigurationconn = QDBusConnection::sessionBus();
    if (!carConfigurationconn.isConnected()) {
        qWarning("dbus not connected.");
        return;
    }
    // If it is registered already, setup right away
    if (carConfigurationconn.interface()->isServiceRegistered(AUTOMOTIVEPROXY_SERVICENAME)) {
        qWarning("oncarconfiguration Reg#1.");
        onCarconfigurationRegistered();
    } else {
        qDebug() << "Waiting for service:" << AUTOMOTIVEPROXY_SERVICENAME;
        QDBusServiceWatcher *carConfigrationWatcher = new QDBusServiceWatcher(AUTOMOTIVEPROXY_SERVICENAME,
                carConfigurationconn, QDBusServiceWatcher::WatchForRegistration, this);
        connect(carConfigrationWatcher, &QDBusServiceWatcher::serviceRegistered, this, &PlatformControllerGen3::onCarconfigurationRegistered);
    }

    // 5. AGW LifeCycleManager
    // Automotive.LifyCycleManager
    QDBusConnection lifeCycleManagerConn = QDBusConnection::sessionBus();
    if (!lifeCycleManagerConn.isConnected()) {
        qWarning("dbus not connected.");
        return;
    }
    // If it is registered already, setup right away
    if (lifeCycleManagerConn.interface()->isServiceRegistered(AUTOMOTIVEPROXY_SERVICENAME)) {
        qWarning(sys) << "onLifeCycleManager Reg#1.";
        onLifeCycleManagerRegistered();
    } else {
        qDebug() << "Waiting for service:" << AUTOMOTIVEPROXY_SERVICENAME;
        QDBusServiceWatcher *lifeCycleManagerWatcher = new QDBusServiceWatcher(AUTOMOTIVEPROXY_SERVICENAME,
                lifeCycleManagerConn, QDBusServiceWatcher::WatchForRegistration, this);
        connect(lifeCycleManagerWatcher, &QDBusServiceWatcher::serviceRegistered, this, &PlatformControllerGen3::onLifeCycleManagerRegistered);
    }

    // 6. Zone service
    QDBusConnection HMIZoneConn = QDBusConnection::sessionBus();
    if (!HMIZoneConn.isConnected()) {
        qWarning("dbus not connected.");
        return;
    }
    // If it is registered already, setup right away
    if (HMIZoneConn.interface()->isServiceRegistered(HMIZONE_SERVICE_NAME)) {
        onHMIZoneInterfaceRegistered();
    } else {
        qDebug() << "Waiting for service:" << HMIZONE_SERVICE_NAME;
        QDBusServiceWatcher *HMIZoneWatcher = new QDBusServiceWatcher(HMIZONE_SERVICE_NAME,
                HMIZoneConn, QDBusServiceWatcher::WatchForRegistration, this);
        connect(HMIZoneWatcher, SIGNAL(serviceRegistered(QString)), this, SLOT(onHMIZoneInterfaceRegistered()));
    }
    qDebug() << "setupDBus() #2";


    // 7. Data Update Service
    // NavigationDataUpdateService도 엔진 초기화 이전에 서비스 등록을 마쳐야 하기 때문에
    // setupDBus() 함수에서 이 곳으로 옮김. 2019.2.18 by hheo
    qDebug() << "NavigationDataUpdateService...";

    bool navigationDataUpdateServiceEnabled = true;// qgetenv("TMAP_ENABLE_NAVIGATION_DATA_UPDATE_SERVICE").toInt();
    if (navigationDataUpdateServiceEnabled) {
        qDebug() << "Registering navigation Data Update service...";
        m_navigationDataUpdateService = new NavigationDataUpdateService();
        if(m_navigationDataUpdateService == NULL) {
            // if m_navigationDataUpdateService is NULL...
            qCritical() << "create NavigationDataUpdateService is FAILED!!!";
        }

        // register a service
        m_navigationDataUpdateService->registerService();

        onNavigationDiagDataUpdateStatusInfo(NavDataUpdateState_IDLE,0); // send idle for Diagnostic on startup
    }

    if (m_navigationDataUpdateService) {
        connect(m_navigationDataUpdateService, &NavigationDataUpdateService::requestDiagDataUpdateviaUSB,
                this, &PlatformControllerGen3::platformDiagDataUpdateRequested);
        connect(m_navigationDataUpdateService, &NavigationDataUpdateService::requestDiagDataDeleteMapData,
                this, &PlatformControllerGen3::platformDiagDataDeleteRequested);
    }

    // 8. DrPosition
    // 9. NavigationExtAgent

    // 10. AGW PrivateDataEncryption
    // Automotive.PrivateDataEncryption
    QDBusConnection dataEncryptconn = QDBusConnection::sessionBus();
    if (!dataEncryptconn.isConnected()) {
        qWarning("dbus not connected.");
    } else {
        // If it is registered already, setup right away
        if (dataEncryptconn.interface()->isServiceRegistered(AUTOMOTIVEPROXY_SERVICENAME)) {
            qWarning("ondataEncrypt Reg#1.");
            onDataEncryptRegistered();
        } else {
            qDebug() << "Waiting for service:" << AUTOMOTIVEPROXY_SERVICENAME;
            QDBusServiceWatcher *dataEncryptWatcher = new QDBusServiceWatcher(AUTOMOTIVEPROXY_SERVICENAME,
                    dataEncryptconn, QDBusServiceWatcher::WatchForRegistration, this);
            connect(dataEncryptWatcher, &QDBusServiceWatcher::serviceRegistered, this, &PlatformControllerGen3::onDataEncryptRegistered);
        }
    }

    // 11. AGW Secure Data Communication
    QDBusConnection secureDatacomConn = QDBusConnection::sessionBus();
    if (!secureDatacomConn.isConnected()) {
        qWarning("scuredatacom dbus not connected");
    } else {
        if (secureDatacomConn.interface()->isServiceRegistered(AUTOMOTIVEPROXY_SERVICENAME)) {
            onSecureDataCommunicationRegistered();
        } else {
            qDebug() << "Wating for sdc service";
            QDBusServiceWatcher *secureDataComWatcher = new QDBusServiceWatcher(AUTOMOTIVEPROXY_SERVICENAME,
                   secureDatacomConn, QDBusServiceWatcher::WatchForRegistration, this);

            connect(secureDataComWatcher, &QDBusServiceWatcher::serviceRegistered,
                    this, &PlatformControllerGen3::onSecureDataCommunicationRegistered);
        }
    }
#else
     m_DeviceId = QLatin1String("0030006J");
    //emit platformPrivateDataEncryptionRegistered(0);
    emit platformInitialized();
#endif

#ifndef QT_NO_DEBUG_OUTPUT
    qDebug(sys) << "[Perf][Init]after setup DBUS";
#endif
}

void PlatformControllerGen3::onProfileChanged(uchar id)
{
   qDebug() << "pm, changed to " << id << ", name:" << m_profilemanager->profileName();
   if (m_profilemanager) {
       QMetaObject::invokeMethod(m_controller,"changeProfileNameOccur", Q_ARG(const QString&, m_profilemanager->profileName()));
   }
}

void PlatformControllerGen3::onEngineInitialized(bool success)
{
    qDebug() << "onEngineInitialized...";

    m_engineInitialized = success;

    if (!success) {
        if (m_guiControl) {
            m_guiControl->UpdateClientStatus(CLIENT_NAME,
                                             ClientStatus_CLIENT_STATUS_READY); // ready
        }
#ifndef USE_DBUS
        onInit_GUI(DEFAULT_LAYER_ID, DEFAULT_SURFACE_ID, DEFAULT_WIDTH, DEFAULT_HEIGHT);
        onUpdateSurfaceState(0, HMISurfaceState_HMI_SURFACE_FG);
#endif
        return;
    }
    qDebug() << "NavigationServer...";

    bool navigationServerEnabled = true;// qgetenv("TMAP_ENABLE_NAVIGATION_SERVER").toInt();
    if (navigationServerEnabled) {
        qDebug() << "Registering navigation server...";
        m_navigationServer = new NavigationServer();

        // set properties

        // register a service
        m_navigationServer->registerService();
        m_navigationServer->setNavStatus(NavStatus_NAVSTATUS_UNKNOWN);
    }

    qDebug() << "NavigationSDSService...";

    bool navigationSDSServiceEnabled = true;// qgetenv("TMAP_ENABLE_NAVIGATION_SDS_SERVICE").toInt();
    if (navigationSDSServiceEnabled) {
        qDebug() << "Registering navigation SDS service...";
        m_navigationSDSService = new NavigationSDSService();
        if(m_navigationSDSService == NULL) {
            // if m_navigationSDSService is NULL...
            qCritical() << "create NavigationSDSService is FAILED!!!";
        }

        // register a service
        m_navigationSDSService->registerService();
    }

//    if (m_guiControl) {
//        m_guiControl->UpdateClientStatus(CLIENT_NAME,
//                                         ClientStatus_CLIENT_STATUS_READY); // ready
//    }

    // (ex) connect for DBUS
    TmapProperty* naviProperty = m_controller->naviProperty();
    connect(naviProperty, &TmapProperty::TmapNaviStartStatusChanged, this, &PlatformControllerGen3::onNavStartStatusChanged);
    connect(naviProperty, &TmapProperty::TmapMapVersionChanged, this, &PlatformControllerGen3::onMapVersionChanged);
    connect(naviProperty, &TmapProperty::TmapIsMuteChanged, this, &PlatformControllerGen3::onAudioMuteStateChanged);
    connect(naviProperty, &TmapProperty::TmapVoiceGuidanceTypeChanged, this, &PlatformControllerGen3::onVoiceGuidanceTypeChanged);
    connect(naviProperty, &TmapProperty::TmapOverSpeedAlertActiveChanged, this, &PlatformControllerGen3::onOverSpeedAlertActiveChanged);
    connect(naviProperty, &TmapProperty::TmapRGDataInfoChanged, this, &PlatformControllerGen3::onCurrentStreetChanged);
    connect(naviProperty, &TmapProperty::TmapSatelliteInfoChanged, this, &PlatformControllerGen3::onSatelliteInfoChanged);

    connect(naviProperty, &TmapProperty::TmapRGDataInfoChanged, this, &PlatformControllerGen3::onDistanceToDestination);
    connect(naviProperty, &TmapProperty::TmapRGDataInfoChanged, this, &PlatformControllerGen3::onTurntoStreet);
    connect(naviProperty, &TmapProperty::TmapRGDataInfoChanged, this, &PlatformControllerGen3::onRroundaboutExitNumber);
    connect(naviProperty, &TmapProperty::TmapRGDataInfoChanged, this, &PlatformControllerGen3::onNextTurntoStreet);
    connect(naviProperty, &TmapProperty::TmapRGDataInfoChanged, this, &PlatformControllerGen3::onBarGraphInfo);
    connect(naviProperty, &TmapProperty::TmapRGDataInfoChanged, this, &PlatformControllerGen3::onSpeedLimit);
    connect(naviProperty, &TmapProperty::TmapRGDataInfoChanged, this, &PlatformControllerGen3::onPositionInformation);
    connect(naviProperty, &TmapProperty::TmapRGDataInfoChanged, this, &PlatformControllerGen3::onNextManeuverDetails);
    connect(naviProperty, &TmapProperty::TmapRGDataInfoChanged, this, &PlatformControllerGen3::onLaneRecommendationList);
    connect(naviProperty, &TmapProperty::TmapRGDataInfoChanged, this, &PlatformControllerGen3::onArrivalInformationList);
    connect(naviProperty, &TmapProperty::TmapRGDataInfoChanged, this, &PlatformControllerGen3::onTrafficEnforcement);

    connect(naviProperty, &TmapProperty::TmapRGDataInfoChanged, this, &PlatformControllerGen3::onManeuverSymbol);
    connect(naviProperty, &TmapProperty::TmapRGDataInfoChanged, this, &PlatformControllerGen3::onManeuverView);
    connect(naviProperty, &TmapProperty::TmapRGDataInfoChanged, this, &PlatformControllerGen3::onRGDataInfoChanged);

    connect(naviProperty, &TmapProperty::TmapNaviStatusChanged, this, &PlatformControllerGen3::onNavStatusChanged);

    connect(naviProperty, &TmapProperty::TmapWorkLocationChanged, this, &PlatformControllerGen3::onWorkLocation);
    connect(naviProperty, &TmapProperty::TmapHomeLocationChanged, this, &PlatformControllerGen3::onHomeLocation);
    connect(naviProperty, &TmapProperty::TmapViewModeChanged, this, &PlatformControllerGen3::onMapRepresentation);
    connect(naviProperty, &TmapProperty::TmapWaypointListChanged, this, &PlatformControllerGen3::onWaypointList);
    connect(naviProperty, &TmapProperty::TmapActiveDestinationInfoChanged, this, &PlatformControllerGen3::onActiveDestinationInfoChanged);
    connect(naviProperty, &TmapProperty::TmapRouteOptionChanged, this, &PlatformControllerGen3::onRouteOptionChanged);

    connect(naviProperty, &TmapProperty::TmapFavoriteChanged, this, &PlatformControllerGen3::onIsAddressBookChanged);
    connect(naviProperty, &TmapProperty::TmapRecentDestinationChanged, this, &PlatformControllerGen3::onIsLastDestinationListChanged);

    connect(naviProperty, &TmapProperty::TmapRGDataInfoChanged, this, &PlatformControllerGen3::onDestinationReachedStatusChanged);
    connect(naviProperty, &TmapProperty::TmapWaypointOperationStatusChanged, this, &PlatformControllerGen3::onWaypointOperationStatusList);
    connect(naviProperty, &TmapProperty::TmapRouteCalculateDurationChanged, this, &PlatformControllerGen3::onRouteCalculateDurationChanged);
    connect(naviProperty, &TmapProperty::TmapRouteRecalculationReasonChanged, this, &PlatformControllerGen3::onRouteRecalculationReasonChanged);

    connect(naviProperty,SIGNAL(TmapScreenIDChanged(int)),this,SLOT(onScreenIDChanged(int)));

    if (m_navigationServer) {
        connect(m_navigationServer, &NavigationServer::retriggerAcousticOutput, this, &PlatformControllerGen3::onRetriggerAcousticOutput);
//        connect(m_navigationServer, &NavigationServer::showMapScreenWithMapViewMode, this, &PlatformControllerGen3::onShowMapScreenWithMapViewMode);
        connect(m_navigationServer, &NavigationServer::setZoomInStep, this, &PlatformControllerGen3::onSetZoomInStep);
        connect(m_navigationServer, &NavigationServer::setZoomLevel, this, &PlatformControllerGen3::onSetZoomLevel);
        connect(m_navigationServer, &NavigationServer::setZoomOutStep, this, &PlatformControllerGen3::onSetZoomOutStep);
//        connect(m_navigationServer, &NavigationServer::showTrafficInfoScreen, this, &PlatformControllerGen3::onShowTrafficInfoScreen);
        connect(m_navigationServer, &NavigationServer::toShowAdjustCurrentLocationScreen, this, &PlatformControllerGen3::onShowAdjustCurrentLocationScreen);
        connect(m_navigationServer, &NavigationServer::showDeleteWaypointListScreen, this, &PlatformControllerGen3::onShowDeleteWaypointListScreen);
        connect(m_navigationServer, &NavigationServer::requestWaypointList, this, &PlatformControllerGen3::onRequestWaypointList);

        connect(m_navigationServer, &NavigationServer::showWhereAmIScreen, this, &PlatformControllerGen3::onShowWhereAmIScreen);
        connect(m_navigationServer, &NavigationServer::showCarsorLocationOnMap, this, &PlatformControllerGen3::onShowWhereAmIScreen);
        connect(m_navigationServer, &NavigationServer::applyWaypointListChange, this, &PlatformControllerGen3::onApplyWaypointListChange);
        connect(m_navigationServer, &NavigationServer::cancelRouteGuidance, this, &PlatformControllerGen3::onCancelRouteGuidance);
        connect(m_navigationServer, &NavigationServer::insertWaypoint, this, &PlatformControllerGen3::onInsertWaypoint);
        connect(m_navigationServer, &NavigationServer::insertWaypointAtDefaultPosition, this, &PlatformControllerGen3::onInsertWaypointAtDefaultPosition);
        connect(m_navigationServer, &NavigationServer::deleteWaypoints, this, &PlatformControllerGen3::onDeleteWaypoints);
        connect(m_navigationServer, &NavigationServer::replaceWaypoint, this, &PlatformControllerGen3::onReplaceWaypoint);
        connect(m_navigationServer, &NavigationServer::startGuidance, this, &PlatformControllerGen3::onStartGuidance);
        connect(m_navigationServer, &NavigationServer::toSetMapRepresentation, this, &PlatformControllerGen3::onSetMapRepresentation);
       connect(m_navigationServer, &NavigationServer::toSetVoiceGuidance, this, &PlatformControllerGen3::onSetVoiceGuidance);
        connect(m_navigationServer, &NavigationServer::startGuidanceToHomeLocation, this, &PlatformControllerGen3::onStartGuidanceToHomeLocation);
        connect(m_navigationServer, &NavigationServer::startGuidanceToWorkLocation, this, &PlatformControllerGen3::onStartGuidanceToWorkLocation);
        connect(m_navigationServer, &NavigationServer::startGuidanceToPosWGS84, this, &PlatformControllerGen3::onStartGuidanceToPosWGS84);

//        connect(m_navigationServer, &NavigationServer::storeDestinationInAddressBook, this, &PlatformControllerGen3::onStoreDestinationInAddressBook);
        connect(m_navigationServer, &NavigationServer::notifySetLocationWithCoordinates, this, &PlatformControllerGen3::onSetLocationWithCoordinates);
        connect(m_navigationServer, &NavigationServer::notifySetLocationWithDetails, this, &PlatformControllerGen3::onSetLocationWithDetails);
        connect(m_navigationServer, &NavigationServer::notifyShowAndSetLocationWithDetails, this, &PlatformControllerGen3::onShowAndSetLocationWithDetails);
        #if 0  // 2019-03-12  Don't ui flow, just keep it in cache for all setLocationWith*
        connect(m_navigationServer, &NavigationServer::notifySetLocationWithDestinationMemoryEntry, this, &PlatformControllerGen3::onSetLocationWithDestinationMemoryEntry);
        #else
        connect(m_navigationServer, &NavigationServer::toLocationWithDestinationMemoryEntry, this, &PlatformControllerGen3::onToLocationWithDestinationMemoryEntry);
        #endif
        connect(m_navigationServer, &NavigationServer::toSdsGetHouseNumberPattern, this, &PlatformControllerGen3::onSdsGetHouseNumberPattern);
        connect(m_navigationServer, &NavigationServer::showCalculateTour, this, &PlatformControllerGen3::onShowCalculateTour);
        connect(m_navigationServer, &NavigationServer::toRequestDestinationMemoryEntries, this, &PlatformControllerGen3::onRequestDestinationMemoryEntries);
        connect(m_navigationServer, &NavigationServer::notifyRequestFreeTextSearchResults, this, &PlatformControllerGen3::onRequestFreeTextSearchResults);
        connect(m_navigationServer, &NavigationServer::notifyShowDestinationFreeTextSearchInput, this, &PlatformControllerGen3::onShowDestinationFreeTextSearchInput);
        connect(m_navigationServer, &NavigationServer::notifySdsSearchAddress, this, &PlatformControllerGen3::onSdsSearchAddress);
        connect(m_navigationServer, &NavigationServer::notifyRequestDetailForSelectedLocation, this, &PlatformControllerGen3::onRequestDetailsForSelectedLocation);
        connect(m_controller, &TmapController::responsedFreeTextSearchResult, m_navigationServer, &NavigationServer::handleFreeTextSearchResult);
        connect(m_controller, &TmapController::responsedSearchAddress, m_navigationServer, &NavigationServer::handleAddressSearchResult);
        connect(m_controller, &TmapController::responsedLocationDetail, m_navigationServer, &NavigationServer::handleLocationDetail);


        connect(m_navigationServer, &NavigationServer::toStartMapVideoStream, this, &PlatformControllerGen3::onStartMapVideoStream);
        connect(m_navigationServer, &NavigationServer::toModifyMapVideoStream, this, &PlatformControllerGen3::onModifyMapVideoStream);
        connect(m_navigationServer, &NavigationServer::toDeleteMapVideoStream, this, &PlatformControllerGen3::onDeleteMapVideoStream);

        connect(m_navigationServer, &NavigationServer::storePOILocationToAddressBook, this, &PlatformControllerGen3::onStorePoiLocationToAddressBook);
        connect(m_navigationServer, &NavigationServer::storeLocationToAddressBook, this, &PlatformControllerGen3::onStoreLocationToAddressBook);

        connect(m_navigationServer, &NavigationServer::showOnMap, this, &PlatformControllerGen3::onShowOnMap);
        connect(m_navigationServer, &NavigationServer::speakSamplePhrase, this, &PlatformControllerGen3::onSpeakSamplePhrase);


    }

    if (m_navigationSDSService) {
        connect(naviProperty, &TmapProperty::TmapRGDataInfoChanged, this, &PlatformControllerGen3::onPositionStatusInfo);
        connect(naviProperty, &TmapProperty::TmapRGDataInfoChanged, this, &PlatformControllerGen3::onPositionInfo);

        connect(m_navigationSDSService, &NavigationSDSService::notifySdsSearchAddress, this, &PlatformControllerGen3::onSdsSearchAddress);
        connect(m_controller, &TmapController::responsedSDSSearchAddress, m_navigationSDSService, &NavigationSDSService::handleAddressSearchResult);
        connect(m_navigationSDSService, &NavigationSDSService::notifySetLocationWithDetails, this, &PlatformControllerGen3::onSetLocationWithDetails);
    }

    if (m_guiControl) {
        m_guiControl->UpdateClientStatus(CLIENT_NAME,
                                         ClientStatus_CLIENT_STATUS_READY); // ready
    }

    connect(this, &PlatformControllerGen3::requestProperty_All, naviProperty, &TmapProperty::onTmapRequestProperty_All);

    qDebug() << "requestProperty_All #1";
    emit requestProperty_All();
    qDebug() << "requestProperty_All #2";

#ifdef TEST_DBUS
    m_dbusTest->init();
    m_dbusTest->runTest();
#endif

#ifndef USE_DBUS
    onInit_GUI(DEFAULT_LAYER_ID, DEFAULT_SURFACE_ID, DEFAULT_WIDTH, DEFAULT_HEIGHT);
    onUpdateSurfaceState(0, HMISurfaceState_HMI_SURFACE_FG);
#endif

    // monitor connman
    ConnManager *netMngr = new ConnManager(this);
    netMngr->init();
    connect(netMngr, &ConnManager::stateChanged,
            this, &PlatformControllerGen3::onNetworkStateChanged);
    setNetworkOnline(netMngr->state() >= ConnManager::Ready);

}

void PlatformControllerGen3::onApplicationWindowCreated(QQuickWindow* window)
{
#ifndef QT_NO_DEBUG_OUTPUT
    qDebug(sys) << "PlatformControllerGen3::onApplicationWindowCreated:" << window;
#endif
       // register window to ivi layer manager
    emit registApplicationWindow(window, &m_appSurface);
}

void PlatformControllerGen3::onLayermanagerInitialized(bool success)
{
    qDebug() << "PlatformControllerGen3::onLayermanagerInitialized(" << success << ") : " << m_createdLayerCount;

    if (success) {
        m_lmInitialized = true;

        if (m_guiControl) {
            m_guiControl->UpdateClientStatus(CLIENT_NAME,
                                             ClientStatus_CLIENT_STATUS_GUI_INITED);
        }

        if (m_guiControl) {
            m_guiControl->UpdateClientStatus(CLIENT_NAME,
                                             ClientStatus_CLIENT_STATUS_GUI_SENSE_READY);
        }

        // update Navi Start Status
        if (m_navigationExt) {
            m_navigationExt->NaviMethThirdPartyNaviStartState(2); // Third party navigation ready and wrapper can get information from navigation, such as dataset/software version, etc...
        }

        // update menu poi category infomation for VR, it needs to be internationalized for language
        QList<NSPoiCategoryShortcut> poicategories;
        if (m_LanguageIndexStatus == 12 ||
                m_LanguageIndexStatus == 13) {
        poicategories << NSPoiCategoryShortcut{ QStringLiteral("Fuel station"), 0,}
                      << NSPoiCategoryShortcut{ QStringLiteral("LPG station"), 1,}
                      << NSPoiCategoryShortcut{ QStringLiteral("Parking"), 2,}
                      << NSPoiCategoryShortcut{ QStringLiteral("Bank"), 3,}
                      << NSPoiCategoryShortcut{ QStringLiteral("Shopping"), 4,}
                      << NSPoiCategoryShortcut{ QStringLiteral("Restaurant"), 5,}
                      << NSPoiCategoryShortcut{ QStringLiteral("Travel"), 6,}
                      << NSPoiCategoryShortcut{ QStringLiteral("Hospital"), 7,}
                      << NSPoiCategoryShortcut{ QStringLiteral("PublicInstitution"), 8,}
                      << NSPoiCategoryShortcut{ QStringLiteral("Finance"), 9,}
                      << NSPoiCategoryShortcut{ QStringLiteral("ChargingEvSt"), 10,};
        } else {
            poicategories << NSPoiCategoryShortcut{ QStringLiteral("주유소"), 0,}
                          << NSPoiCategoryShortcut{ QStringLiteral("충전소"), 1,}
                          << NSPoiCategoryShortcut{ QStringLiteral("주차장"), 2,}
                          << NSPoiCategoryShortcut{ QStringLiteral("은행"), 3,}
                          << NSPoiCategoryShortcut{ QStringLiteral("쇼핑"), 4,}
                          << NSPoiCategoryShortcut{ QStringLiteral("음식점"), 5,}
                          << NSPoiCategoryShortcut{ QStringLiteral("관광"), 6,}
                          << NSPoiCategoryShortcut{ QStringLiteral("병원"), 7,}
                          << NSPoiCategoryShortcut{ QStringLiteral("공공기관 "), 8,}
                          << NSPoiCategoryShortcut{ QStringLiteral("금융"), 9,}
                          << NSPoiCategoryShortcut{ QStringLiteral("전기차 충전소"), 10,};
        }


        if (m_navigationServer) {
            m_navigationServer->setMenuPoiCategoryShortcuts(poicategories);
        }


        // surface가 준비되면 zone을 설정 (Bosch guide)
        // 19.05.22 ymhong.
        beforePlatformContextSwitchRequested();

        emit clientStatusGuiSenseReady();
    }

    bool surfaceForeground = qgetenv("TMAP_SET_SURFACE_FOREGROUND").toInt();
    if (surfaceForeground) {
        if (m_guiControl) {
            m_guiControl->ChangeSurfaceState(HMISurfaceState_HMI_SURFACE_FG, 0);
        }
    }

    // take care usb is already mounted before system turn on
    if (m_navigationExt) {
        NXDevMediaInfo info = m_navigationExt->naviPropertyDevMediaInfo();
        if (info.m_mediaStatus == 0) {
            emit platformMediaUnmounted();
        } else if (info.m_mediaStatus == 1) {
            emit platformMediaMounted(info.m_mediaPath);
        }
    }

    // in first time, navigation should need the active profile to synchronize all tasks base on a profile like a data or setting 
    if (m_profilemanager) {
        // in sync call, don't guarantee immediately response due to system side issue
        QMetaObject::invokeMethod(m_controller,"changeUserOccur", Q_ARG(int, m_profilemanager->activeProfile()));
        QMetaObject::invokeMethod(m_controller,"changeProfileNameOccur", Q_ARG(const QString&, m_profilemanager->profileName()));
        // QtConcurrent::run(m_profilemanager,&BoschCmProfileInterface::activeProfile);
    }
}

void PlatformControllerGen3::onKeyEvent(quint32 state, quint32 nativeScanCode)
{
 #ifdef USE_DBUS
    QEvent::Type eventType = (QEvent::Type)state;
    if (eventType == QEvent::KeyPress || eventType == QEvent::KeyRelease) {
        if (m_guiControl) {
            GCKeyInfo k;
            k.m_key = nativeScanCode;// - 8; // Qt wayland added +8
            k.m_state = (eventType == QEvent::KeyPress) ? 1 : 0;

            switch (k.m_key) {
            case HardKey::ItcJogDialBack:
                //switchToSystem();
                QMetaObject::invokeMethod(m_controller, "applyHardKey", Q_ARG(int, k.m_key), Q_ARG(int, k.m_state));
                break;

            default:
                qDebug() << "key forwarding: key=" << k.m_key
                         << "state=" << k.m_state;
                m_guiControl->KeyForwarding(k);
                break;
            }
        }
    } else if (eventType == QEvent::Wheel){
        GCKeyInfo k;
        k.m_state = HardKey::AcceptKeyState;
        auto positiveValue = nativeScanCode;
        if (positiveValue == 0) {
            k.m_key = HardKey::ItcJogDialSpinCcw;
        } else {
            k.m_key = HardKey::ItcJogDialSpinCw;
        }
        if (mHardKeyFilter.acceptable(k)) {
            QMetaObject::invokeMethod(m_controller, "applyHardKey", Q_ARG(int, k.m_key), Q_ARG(int, k.m_state));
        }
    }
#endif
}

void PlatformControllerGen3::encorderForwarding(int encCode, int encValue)
{
    m_guiControl->EncoderForwarding(encCode, encValue);
}

void PlatformControllerGen3::onPositionStatusInfo(RGDataPtr value)
{
//    qDebug() << "NavigationSDSService::: called PlatformControllerGen3::onPositionStatusInfo";
    NSPositionStatusInfo positionStatusInfo;
    positionStatusInfo.m_countryCode = "KOR";
    positionStatusInfo.m_positionWGS84.m_latitude = value->stCurrent.latitude;
    positionStatusInfo.m_positionWGS84.m_longitude = value->stCurrent.longitude;
    positionStatusInfo.m_vehicleHeading = static_cast<double>(value->stCurrent.nPosAngle);
    positionStatusInfo.m_vehicleSpeed = static_cast<double>(value->stCurrent.nPosSpeed);
    positionStatusInfo.m_stateCode = QString("KR-%1").arg(QString::number(value->stCurrent.usStateCode));

    switch(value->stCurrent.roadcate)
    {
    case 0:         positionStatusInfo.m_roadClass = 1;     break; //ROAD_CLASS_MOTORWAY    //고속국도
    case 1:         positionStatusInfo.m_roadClass = 2;     break; //ROAD_CLASS_FREEWAY     //도시 고속화 도로
    case 2:                                                                                 //국도
    case 3:         positionStatusInfo.m_roadClass = 3;     break; //ROAD_CLASS_HIGHWAY     //국가지원지방도
    case 4:         positionStatusInfo.m_roadClass = 4;     break; //ROAD_CLASS_ARTERIAL
    case 5:                                                                                 //주요도로1
    case 6:                                                                                 //주요도로2
    case 7:         positionStatusInfo.m_roadClass = 5;     break; //ROAD_CLASS_COLLECTOR   //주요도로3
    case 8:
    case 9:
    case 10:
    case 12:        positionStatusInfo.m_roadClass = 6;     break; //ROAD_CLASS_LOCAL
    default:        positionStatusInfo.m_roadClass = 0;     break; //ROAD_CLASS_NONE
    }

//    qDebug(sys) << "positionStatusInfo" << positionStatusInfo.m_positionWGS84.m_latitude << positionStatusInfo.m_positionWGS84.m_longitude
//                << positionStatusInfo.m_vehicleHeading << positionStatusInfo.m_vehicleSpeed << positionStatusInfo.m_stateCode << positionStatusInfo.m_roadClass;

    if (m_navigationSDSService != NULL) {
        m_navigationSDSService->setPositionStatusInfo(positionStatusInfo);
    }
}

void PlatformControllerGen3::onPositionInfo(RGDataPtr value)
{
//    qDebug() << "NavigationSDSService::: called PlatformControllerGen3::onPositionInfo";
    NSPositionInfo positionInfo;
    positionInfo.m_country = "대한민국";

    QString add = TextDecoder::instance()->fromCP949(value->stCurrent.szPosAddrName);
    QStringList AddList= add.split(" ");
    int listCount = add.split(" ").length();

    if(listCount > 2){
        positionInfo.m_province       = AddList[0];
        positionInfo.m_city           = AddList[1];
        positionInfo.m_cityDistrict   = AddList[2];
    }

    positionInfo.m_street = TextDecoder::instance()->fromCP949(value->stCurrent.szPosRoadName);
    positionInfo.m_streetNumber= "";

//    qDebug(sys) << "positionInfo" << positionInfo.m_country << positionInfo.m_province << positionInfo.m_city << positionInfo.m_cityDistrict << positionInfo.m_street << "[" << value->eRgStatus << "]";

    if (m_navigationSDSService != NULL) {
        m_navigationSDSService->setPositionInfo(positionInfo);
    }
}

void PlatformControllerGen3::onSdsSearchAddress(const NSSDSAddress& requestedAddress)
{
//    qDebug() << "NavigationSDSService::: called PlatformControllerGen3::onSdsCheckAddress. count:" << requestedAddress.size();

//    for (int ii = 0; ii < requestedAddress.size(); ii++) {
//        qDebug() << "[" << ii << "] type:" << requestedAddress.at(ii).m_addressElementType <<
//                    "    data:" << requestedAddress.at(ii).m_data <<
//                    "    ambi:" << requestedAddress.at(ii).m_elementOptions.m_isAmbigious;
//    }

//    // as result it update the attribute sdsAddressWithOptions.
//    NSSDSAddressWithOptions options;
//    options.m_address = requestedAddress;
//    m_navigationSDSService->setSdsAddressWithOptions(options);
    if (requestedAddress.size() == 0) return;

    QVariantMap address;
    enum {
      ElementStateFilled    = 0x00000010,
      ElementPlaceFilled    = 0x00000100,
      ElementRoadFilled     = 0x00001000,
    };
    uint fullElementType = 0;
    for (int i=0;i<requestedAddress.size(); i++) {
        NSSDSAddressElement ae = requestedAddress.at(i);
        switch (ae.m_addressElementType) {
            case SDSAddressElementType_SDS_ADDRESS_ELEMENT_TYPE_COUNTRY:
                break;
            case SDSAddressElementType_SDS_ADDRESS_ELEMENT_TYPE_STATE:
               address[QStringLiteral("state")] = ae.m_data;
               fullElementType |= ElementStateFilled;
                break;
            case SDSAddressElementType_SDS_ADDRESS_ELEMENT_TYPE_PLACE:
              fullElementType |= ElementPlaceFilled;
              address[QStringLiteral("place")] = ae.m_data;
                break;
//            case SDSAddressElementType_SDS_ADDRESS_ELEMENT_TYPE_CITYDESTRICT:
//                address[QStringLiteral("cityDestrict")] = ae.m_data;
//                break;
            case SDSAddressElementType_SDS_ADDRESS_ELEMENT_TYPE_ROAD:
               address[QStringLiteral("road")] = ae.m_data;
               fullElementType |= ElementRoadFilled;
                break;
            case SDSAddressElementType_SDS_ADDRESS_ELEMENT_TYPE_HOUSENUMBER:
                address[QStringLiteral("houseNumber")] = ae.m_data;
                break;
            default:
                break;
        }
    }

    bool searchCityCenter = (
                (fullElementType&ElementStateFilled)     &&
                (fullElementType&ElementPlaceFilled)     &&
                !(fullElementType&ElementRoadFilled)
       );

    if (searchCityCenter) {
        QString searchText;
        searchText.append(address[QStringLiteral("state")].toString());
        searchText.append(address[QStringLiteral("place")].toString());
        QString searchTextTrimmed = searchText.replace(QChar(' '),"");
        address[QStringLiteral("citycenter")] = searchCityCenter;
        address[QStringLiteral("words")] = searchTextTrimmed;
    }

    // request
    qDebug() << "pf, address search 1";
    QObject* owner = sender();
    if (owner == static_cast<QObject*>(m_navigationServer) ) {
         QMetaObject::invokeMethod(m_controller,"requestSearchAddress", Q_ARG(const QVariant&, QVariant::fromValue(address)));
         qDebug() << "pf, address search 2";
    }
    else {
         QMetaObject::invokeMethod(m_controller,"requestSDSSearchAddress", Q_ARG(const QVariant&, QVariant::fromValue(address)));
         qDebug() << "pf, address search 3";
    }

}

void PlatformControllerGen3::onNavigationDiagDataUpdateStatusInfo(int status, int progress)
{
    qDebug() << "pf, set diag up status to " << status;
    NavigationDataUpdateStatusInfo info;
    info.m_status = status;
    info.m_progressValue = progress;
    info.m_productName   = QStringLiteral("AIVI_KOR_MAP");
    info.m_regionName    = QStringLiteral("KOREA");
    if (m_navigationDataUpdateService != NULL) {
        m_navigationDataUpdateService->setNavigationDataUpdateStatusInfo(info);
    }
}


void PlatformControllerGen3::onNavigationDiagDataDeleteStatusInfo(int status, int progress)
{
    NavigationDataDeleteStatusInfo info;
    info.m_status         = status;
    info.m_progressValue  = progress;
    info.m_productName    = QStringLiteral("AIVI_KOR_MAP");
    info.m_regionName     = QStringLiteral("KOREA");
    if (m_navigationDataUpdateService != NULL) {
        m_navigationDataUpdateService->setNavigationDataDeleteStatusInfo(info);
    }
}

void PlatformControllerGen3::setZone3Visible(bool visible)
{
    mZone3Visible = visible;

    qDebug() << "PlatformControllerGen3::setZone3Visible(" << visible << ")";
    setZoneVisibility(3, visible, Zone3Visibility);
}

void PlatformControllerGen3::setupKeyboard(uchar type, const QString& text, int searchCount)
{
    if (m_hmiZoneManager) {
        if (mBlockZoneCommand)
            return;

        Zone3InfoHolder zone3InfoHolder;
        zone3InfoHolder.m_updateRegion = true;

        switch (type) {
        case 0: // Zone1 Keyboard
        case 2: // Zone2 Keyboard
        case 3: // Zone2 Numeric
            {
                auto isNumeric = (type == 3);


#define DO_NOT_SETTING_NUMERIC_KEYBOARD

#if defined(DO_NOT_SETTING_NUMERIC_KEYBOARD)
                // 숫자 키보드가 설정 되지만 입력은 들어오지 않음
                // Zone2에서 사용할 키보드가 필요하지만 Zone1용 키보드를 설정하는게 원인인 듯 함.
                // 보쉬에서 firmware를 수정하거나 제대로 된 가이드가 오기전까지
                // 숫자 키보드를 설정하지 않도록 isNumeric을 항상 false로 함.
                // 19.04.23 ymhong
                //isNumeric = false;
#endif

                auto keypadFormat = isNumeric ? NUMERIC : BASIC_QWERTY;
                auto keyboardMode = (0 == type/* || isNumeric*/) ? KEYBOARD_FREEMODE : KEYBOARD_FREEMODE_ZONE2;
                auto keypadFormatString = keypadFormat == NUMERIC ? "NUMERIC" : "BASIC_QWERTY";
                auto keyboardModeString = keyboardMode == KEYBOARD_FREEMODE ? "KEYBOARD_FREEMODE" : "KEYBOARD_FREEMODE_ZONE2";

                zone3InfoHolder.m_zone3Type = keyboardMode;

                Zone3Helper::setupForShowKeyboard(zone3InfoHolder.m_nodeInfo, text, keypadFormat);

                qDebug() << "setZone3(type: " << type << ",keypadFormat: " << keypadFormatString << ", keyboardMode: " << keyboardModeString << ")";
                m_hmiZoneManager->setZone3(TMAP_APP_ID, zone3InfoHolder);
            }
            break;
        case 1: // Hide Keyboard
            {
                zone3InfoHolder.m_zone3Type = Zone3Type_NO_DATA;//BASIC;
                Zone3Helper::setupForHideKeyboard(zone3InfoHolder.m_nodeInfo);
                m_hmiZoneManager->setZone3(TMAP_APP_ID, zone3InfoHolder);
            }
            break;
        }
    }
}

void PlatformControllerGen3::setZone1ForKeyboard(uchar type, const QString &text, int searchCount)
{
    if (m_hmiZoneManager) {
        if (mBlockZoneCommand)
            return;

        const int SEARCH_BAR_FOR_INTEGRATED_SEARCH = 4;

        qDebug() << "PlatformControllerGen3::setZone1ForKeyboard(type:" << type << ", text:" << text << ", searchCount:" << searchCount << ")";

        uint zone1Type = 0;
        ContentInfo ci1;

        if (text.isEmpty() && type != SEARCH_BAR_FOR_INTEGRATED_SEARCH) {
            zone1Type = NAV_SEARCH_BAR;
            Zone1Helper::setupForEditBox(ci1);
        } else {
            zone1Type = SEARCH_TEXT_1;
            Zone1Helper::setupForEditBoxWithCountNumber(ci1, text, searchCount);

            if (mZone1Type == SEARCH_TEXT_2 && 0 < searchCount) {
                qDebug() << "[ZONE1] setEditFieldText(" << text << ")";
                QTimer::singleShot(1, this, [=](){
                    setEditfieldText(TMAP_APP_ID, text, false, 0, 0);       // added three argument. 2019.6.24 by hheo
                });
            }
        }

        bool updateRegion = true;
        Zone1InfoHolder param;
        param.m_zone1Type = zone1Type;
        param.m_updateRegion = updateRegion;
        param.m_nodeInfo = ci1;
        m_hmiZoneManager->setZone1(TMAP_APP_ID, param);

        mZone1Type = zone1Type;
    }
}

void PlatformControllerGen3::showVolume()
{
    ContentInfo ci;
    Zone3InfoHolder param;
    param.m_zone3Type = VOLUME;
    param.m_updateRegion = true;
    param.m_nodeInfo = ci;
    m_hmiZoneManager->setZone3(TMAP_APP_ID, param);
}

void PlatformControllerGen3::switchToApp()
{
    const uint tmapViewId = 304;
    m_guiControl->SwitchToView(tmapViewId);
}

void PlatformControllerGen3::switchToSettingView()
{
    const uint tmapViewId = 1001;
    m_guiControl->SwitchToView(tmapViewId);
}

void PlatformControllerGen3::switchToSystem()
{
    const uint systemViewId = 0;
    m_guiControl->SwitchToView(systemViewId);
}

void PlatformControllerGen3::keyForwarding(uint key, uint state)
{
    GCKeyInfo info;
    info.m_key = key;
    info.m_state = state;

    qDebug() << "PlatformControllerGen3::keyForwarding(" << key << "," << state << ")";
#ifdef BUILD_TARGET
    m_guiControl->KeyForwarding(info);
#endif
}

void PlatformControllerGen3::callNumberWrapper(const QString &number)
{
    callNumber(number);
}

void PlatformControllerGen3::appHasNoMap()
{
    m_appHasNoMap = true;
}

#if DEPRECATED_MAIN_THREAD_STUFF
QObject *PlatformControllerGen3::hmiZoneInterface() const
{
    return m_hmiZoneManager;
}
#endif

void PlatformControllerGen3::setHmiZoneEventHandler(QObject* handler)
{
    QObject::connect(m_hmiZoneManager, SIGNAL(ActiveNotificationType(uint)), handler, SLOT(onActiveNotificationType(uint)));
    QObject::connect(m_hmiZoneManager, SIGNAL(buttonPressed(uchar, uint, uchar)), handler, SLOT(onButtonPressed(uchar, uint, uchar)));
    QObject::connect(m_hmiZoneManager, SIGNAL(keyboardInputFinished(uchar, const QString)), handler, SLOT(onKeyboardInputFinished(uchar, const QString)));
    QObject::connect(m_hmiZoneManager, SIGNAL(keyboardKeyPressed(uchar, const QString, const QString, bool, bool)), handler, SLOT(onKeyboardKeyPressed(uchar, const QString, const QString, bool, bool)));
    QObject::connect(m_hmiZoneManager, SIGNAL(pullupButtonPressed(uchar, uchar, uchar)), handler, SLOT(onPullupButtonPressed(uchar, uchar, uchar)));
    QObject::connect(m_hmiZoneManager, SIGNAL(togglePressed(uchar, uint, uchar, bool)), handler, SLOT(onTogglePressed(uchar, uint, uchar, bool)));
    QObject::connect(m_guiControl, SIGNAL(PopupStatus(uint, uint)), handler, SLOT(onPopupStatus(uint, uint)));
}

void PlatformControllerGen3::setSearchBarText(const QString &text)
{
    setEditfieldText(TMAP_APP_ID, text, false, 0, 0);       // added three argument. 2019.6.24 by hheo
}

QObject *PlatformControllerGen3::getGuiControl() const
{
    return m_guiControl;
}

QDBusPendingCall PlatformControllerGen3::setupZone1(uint type, const QVariant &param, bool updateForce)
{
    if (mBlockZoneCommand) { // !m_appForeground && !updateForce) {
        qDebug() << "m_appForegroud = " << m_appForeground << ", updateForce = " << updateForce;
        return QDBusPendingReply<>{};
    }

    auto m = param.toMap();
    const auto updateRegionKey = QStringLiteral("updateRegion");
    auto updateRegion = m.contains(updateRegionKey) ? m[updateRegionKey].toBool() : true;
    auto textId = m["textId"].toInt();
    auto text = m["text"].toString();
    auto iconPath = m["iconPath"].toString();

    qDebug() << "setupZone1(" << type << "), textId:" << textId << ", text:" << text << ", updateRegion:" << updateRegion;

    ContentInfo contentInfo;

    if (type == TITLE) {
        qDebug() << "setupForTitle(cilEnagle, " << text << ")";
        Zone1Helper::setupForTitle(contentInfo, text);
    } else if (type == SEARCH_TEXT_2) {
        Zone1Helper::setupForKeyboardPulldown(contentInfo, text);
        QTimer::singleShot(1, this, [=](){
            setEditfieldText(TMAP_APP_ID, text, false, 0, 0);       // added three argument. 2019.6.24 by hheo
        });
    } else {
        Zone1Helper::setupForEditBox(contentInfo);
    }

    mZone1Type = type;
    mZone1Param = param;

    Zone1InfoHolder zone1Info;
    zone1Info.m_zone1Type = type;
    zone1Info.m_updateRegion = updateRegion;
    zone1Info.m_nodeInfo = contentInfo;
    return m_hmiZoneManager->setZone1(TMAP_APP_ID, zone1Info);
}

void PlatformControllerGen3::onCarconfigurationRegistered()
{
    qDebug() << "Create dbus interfaces onCarconfigurationRegistered...rhs";
    QDBusConnection c = QDBusConnection::sessionBus();
    if (!c.isConnected()) {
        qWarning("dbus not connected.");
        return;
    }

    m_carconfiguration = new comboschAutomotiveProxyCarConfiguration(AUTOMOTIVEPROXY_SERVICENAME,
                                                       CARCONFIGURATION_PATH,
                                                       c);
    if (!m_carconfiguration->isValid()) {
        qWarning("Cannot connect to service CarConfiguration");
        return;
    }
//    Don't use it. RHS 2019.06.05
//    QDBusPendingReply<QDBusVariant> reply01 = m_carconfiguration->GetConfigurationValue(18); //QDBusVariant quint8
//    QDBusPendingCallWatcher* w = new QDBusPendingCallWatcher(reply01, this);
//    connect(w, &QDBusPendingCallWatcher::finished, [](QDBusPendingCallWatcher* call) {
//        QDBusPendingReply<QDBusVariant> reply = *call;
//        if (reply.isError())
//            qWarning() << "carcfg 01 error";
//        else {
//            QDBusVariant dv = reply.value();
//            auto va = dv.variant();
//            auto value_a = va.toInt();
//            qDebug() << "carcfg return =" << value_a; //reply01.value();
//        }
//        call->deleteLater();
//    });
}

void PlatformControllerGen3::onLifeCycleManagerRegistered()
{
#ifndef QT_NO_DEBUG_OUTPUT
    qDebug(sys) << "Create dbus interfaces onLifeCycleManagerRegistered...hh";
#endif
    QDBusConnection c = QDBusConnection::sessionBus();
    if (!c.isConnected()) {
        qWarning(sys) << "dbus not connected.";
        return;
    }

    m_lifeCycleManager = new ComBoschAutomotiveProxyLifeCycleManagerInterface(AUTOMOTIVEPROXY_SERVICENAME,
                                                       LIFECYCLEMANAGER_PATH,
                                                       c);
    if (!m_lifeCycleManager->isValid()) {
        qWarning(sys) << "Cannot connect to service LifeCycleManager";
        return;
    }

    connect(m_lifeCycleManager, &ComBoschAutomotiveProxyLifeCycleManagerInterface::LifecycleRequest, this, &PlatformControllerGen3::onLifecycleRequest);

    LCMCredentials credentials;
    QVariant t("lxc_extnav");
    credentials["Label"] = QDBusVariant(t);

    QDBusPendingReply<uint> reply = m_lifeCycleManager->Register(credentials, LCMShutdownType_NORMAL);
#if 1
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(reply, this);
    connect(watcher, &QDBusPendingCallWatcher::finished, [this](QDBusPendingCallWatcher* call){
        QDBusPendingReply<uint> reply = *call;
        m_lifeCycleManagerRequestId = reply.value();
#ifndef QT_NO_DEBUG_OUTPUT
        qDebug(sys) << m_lifeCycleManagerRequestId;
#endif

        if (reply.isError()) {
            qWarning(sys) << "LifeCycleManager::Register error :" << reply.error();
        }
        else {
#ifndef QT_NO_DEBUG_OUTPUT
            qDebug(sys) << "LifeCycleManager::Register return :" << m_lifeCycleManagerRequestId;
#endif
        }

        call->deleteLater();
    });
#else
    reply.waitForFinished();
    m_lifeCycleManagerRequestId = reply.value();
    qDebug(sys) << m_lifeCycleManagerRequestId;

    if (reply.isError())
        qWarning(sys) << "LifeCycleManager::Register error :" << reply.error();
    else
        qDebug(sys) << "LifeCycleManager::Register return :" << m_lifeCycleManagerRequestId;
#endif
}

void PlatformControllerGen3::onDataEncryptRegistered()
{
    qDebug() << "Create dbus interfaces onDataEncryptRegistered...rhs";
    QDBusConnection c = QDBusConnection::sessionBus();
    if (!c.isConnected()) {
        qWarning("dbus not connected.");
        return;
    }

    m_dataencrypt = new ComBoschAutomotiveProxyPrivateDataEncryptionInterface(AUTOMOTIVEPROXY_SERVICENAME,
                                                       DATAENCRYPT_PATH,
                                                       c);

    if (!m_dataencrypt->isValid()) {
        qWarning("Cannot connect to service DataEncrypt");
        return;
    }

    emit platformPrivateDataEncryptionRegistered(m_dataencrypt);

#ifdef TEST_DATAENCRYPT
    struct InternalUtils {
        static NSPrivateDataList s2p( const QString& s ) {
            NSPrivateDataList ret;
            QByteArray arr = s.toLocal8Bit();
            for ( auto c : arr ) {
                NSPrivateData data;
                data.m_data = c;
                ret.append( data );
            }
            return ret;
        }

        static QString p2s( const NSPrivateDataList& pd ) {
            QByteArray ba;
            for ( auto c : pd ) {
                NSPrivateData data;
                data = c;
                ba.append( data.m_data );
            }
            return QString::fromLocal8Bit( ba );
        }
    };

    QString s = "hello";

    NSPrivateDataList ay = InternalUtils::s2p(s);
    NSPrivateDataList ray;
    auto r = m_dataencrypt->EncryptData(0, ay, ray );
    if (!r.isValid()) {
        qDebug() << "PrivateDataEncryption Encryp Error: " << r.error();
    }
    else {
        auto rs = InternalUtils::p2s( ray );
        qDebug() << "PrivateDataEncryption Encrypted: " << rs;

        NSPrivateDataList dray;
        r = m_dataencrypt->DecryptData(0, ray, dray);
        if (!r.isValid()) {
            qDebug() << "PrivateDataEncryption Decrypt Error: " << r.error();
        }
        else {
            rs = InternalUtils::p2s( dray );
            qDebug() << "PrivateDataEncryption Decrypted: " << rs;
        }
    }
#endif
}

void PlatformControllerGen3::onServiceRegistered()
{
    qDebug() << "Create dbus interfaces onServiceRegistered...rhs";
    QDBusConnection c = QDBusConnection::sessionBus();

    if (!c.isConnected()) {
        qWarning("dbus not connected.");
        return;
    }

    m_navigationExt = new OrgBoschCmNavigationNavigationExtServiceInterface(SERVICE_NAME,NAVIGATIONEXT_PATH,c);

    if (!m_navigationExt->isValid()) {
        qWarning("Cannot connect to service NavigationExtService");
        return;
    }

    qDebug() << "[Perf]after setting up NavigationNavigationExtServiceInterface";

    if (m_navigationExt) {
        bool ok = false;
        QDBusReply<QString> reply = m_navigationExt->NaviMethGetDeviceID(ok);
        if (ok) {
            m_DeviceId = reply.value();
#ifndef QT_NO_DEBUG_OUTPUT
            qDebug(sys) << "navi device id:"<<m_DeviceId;
#endif
        } else {
            qWarning(sys) << "failed to get device id";
        }
    }

    if (m_navigationExt) {
        uchar colorState = m_navigationExt->naviPropertyMultiAmbienceLightingColorState();
        //qInfo() << "[MultiSense]colorState: " << colorState;
        setColorState( colorState );
    }

    // register signals
//    connect(m_navigationExt, &OrgBoschCmNavigationNavigationExtServiceInterface::ContextSwitchRequest,
//            this, &PlatformControllerGen3::onContextSwitchRequest);
        qDebug() << "onNaviSignalAppStateNavigation #1" ;
    connect(m_navigationExt, &OrgBoschCmNavigationNavigationExtServiceInterface::NaviSignalAppStateNavigation,
            this, &PlatformControllerGen3::onNaviSignalAppStateNavigation);

        qDebug() << "onNaviSignalAppStateNavigation #2" ;
    connect(m_navigationExt, &OrgBoschCmNavigationNavigationExtServiceInterface::NaviSignalRemountStatus,
            [this](uchar status) {
       emit platformRemountStatus(status == 1);
    });
    connect(m_navigationExt, &OrgBoschCmNavigationNavigationExtServiceInterface::ResetToFactorySettings,
            this, &PlatformControllerGen3::onResetToFactorySettings);
    connect(m_navigationExt, &OrgBoschCmNavigationNavigationExtServiceInterface::NaviSignalRFAntennaStatus,
            this, &PlatformControllerGen3::onNaviSignalRFAntennaStatus);

// NaviSignalIsHFPAvailable is deprecated at ExtHall_V05.02. 2019.3.22 by hheo
//    connect(m_navigationExt, &OrgBoschCmNavigationNavigationExtServiceInterface::NaviSignalIsHFPAvailable,
//            [this](uchar status) {
//       emit platformIsHFPAvailable(status == 1);
//    });

#if 0  // Disable this connection until system issue get resolved  2019-03-28
    connect(m_navigationExt, &OrgBoschCmNavigationNavigationExtServiceInterface::NaviSignalPublicConfigItem, this, &PlatformControllerGen3::onPublicConfigItem);
#endif
//    getPublicConfigItem("URL");  // for Configuration Manager Interface test. 2018.9.17 by hheo


    // initialize property observer
    NavigationExtPropertyObserver *observer = new NavigationExtPropertyObserver(m_navigationExt); // self clean up
    setDimmingStatus(observer->dimmingStatus());

    connect(observer, &NavigationExtPropertyObserver::dimmingStatusChanged, this, &PlatformControllerGen3::setDimmingStatus);
    connect(observer, &NavigationExtPropertyObserver::devMediaInfoChanged, [this](NXDevMediaInfo info){
        if (info.m_mediaStatus == 0) {
            emit platformMediaUnmounted();
        } else if (info.m_mediaStatus == 1) {
            emit platformMediaMounted(info.m_mediaPath);
        }
    });

    connect(observer, &NavigationExtPropertyObserver::reverseGearStateChanged, this, &PlatformControllerGen3::setReverseGearState);
    connect(observer, &NavigationExtPropertyObserver::languageIndexChanged, this, &PlatformControllerGen3::setLanguageIndex);
    connect(observer, &NavigationExtPropertyObserver::dateFormatChanged, this, &PlatformControllerGen3::setDateFormat);
    connect(observer, &NavigationExtPropertyObserver::timeFormatChanged, this, &PlatformControllerGen3::setTimeFormat);
    connect(observer, &NavigationExtPropertyObserver::colorStateChanged, this, &PlatformControllerGen3::setColorState);
    connect(observer, &NavigationExtPropertyObserver::mexModeChanged, this, &PlatformControllerGen3::setMexMode);
    connect(observer, &NavigationExtPropertyObserver::dialPOICallStatusChanged, this, &PlatformControllerGen3::setDialPOICallStatus);
    connect(observer, &NavigationExtPropertyObserver::callServiceStatusChanged, this, &PlatformControllerGen3::setCallServiceStatus);
    connect(observer, &NavigationExtPropertyObserver::UpdateLockStatesChanged, this, &PlatformControllerGen3::setUpdateLockStates);
    connect(observer, &NavigationExtPropertyObserver::dateTimeDetailInfoChanged, this, &PlatformControllerGen3::setDateTimeDetailInfo);
    connect(observer, &NavigationExtPropertyObserver::hmiStateChanged, this, &PlatformControllerGen3::setHmiState);
    connect(observer, &NavigationExtPropertyObserver::appStateNavigationChanged, this, &PlatformControllerGen3::setAppStateNavigation);

    connect(m_controller, &TmapController::naviShutdownReadyFinished, this, &PlatformControllerGen3::onNaviShutdownReadyFinished);


    // inform navigation start up state when dbus client of NavigationExtService ready.
    if (m_navigationExt) {
        m_navigationExt->NaviMethThirdPartyNaviStartState(1); // 1: The Client of NavigationExtService ready to communication and handle the data from system.
    }


    m_guiControl = new BoschGuiCmAiGuicontrolInterface(SERVICE_NAME,
                                                       GUICONTROL_PATH,
                                                       c);
    if (!m_guiControl->isValid()) {
        qWarning("Cannot connect to service GuiControl");
        return;
    }
    // register signals
    connect(m_guiControl, &BoschGuiCmAiGuicontrolInterface::Init_GUI, this, &PlatformControllerGen3::onInit_GUI);
    connect(m_guiControl, &BoschGuiCmAiGuicontrolInterface::UpdateSurfaceState, this, &PlatformControllerGen3::onUpdateSurfaceState);
//    connect(m_guiControl, &BoschGuiCmAiGuicontrolInterface::PopupStatus, this, &PlatformControllerGen3::onPopupStatus);
    connect(m_guiControl, &BoschGuiCmAiGuicontrolInterface::ActivateExternalImageProvider, this, &PlatformControllerGen3::onActivateExternalImageProvider);
    connect(m_guiControl, &BoschGuiCmAiGuicontrolInterface::ExternalImageProviderTouch, this, &PlatformControllerGen3::onExternalImageProviderTouch);
    connect(m_guiControl, &BoschGuiCmAiGuicontrolInterface::ContextSwitchRequest, this, &PlatformControllerGen3::onContextSwitchRequest);
    connect(m_guiControl, &BoschGuiCmAiGuicontrolInterface::NaviGadgetFocusInteractionSignal, this, &PlatformControllerGen3::onNaviGadgetFocusInteractionSignal);

    // 2019-03-13,  navi and hmi volume sync
    connect(m_controller, &TmapController::naviVolumeTouched, this, &PlatformControllerGen3::onNaviVolumeTouched);

    // initialize property observer
    GuicontrolPropertyObserver *guicontrolObserver = new GuicontrolPropertyObserver(m_guiControl);	// insert new property (NaviPropertyFocusConfig) 2019.07.09 RHS
    connect(guicontrolObserver, &GuicontrolPropertyObserver::focusConfigChanged, this, &PlatformControllerGen3::onFocusConfigProvider);
    connect(guicontrolObserver, &GuicontrolPropertyObserver::naviGadgetFocusInfoChanged, this, &PlatformControllerGen3::onNaviGadgetFocusStatusChanged);
    // UI에 현재 property를 전달.
    guicontrolObserver->emitFocusConfigChanged();

    // 강제로 시스템 프로퍼티 변경 시그널을 발생 시킨다.
    // 시스템 프로퍼티의 초기값이 타겟이 설정값으로 되어 값의 변경이 되지 않는 한 app으로 통지되지 않음.
    // 1. 언어
    // 2. 컬러
    // 3. MEX모드
    //
    // 19.07.20 ymhong
    QMetaObject::invokeMethod(observer, "forceSystemPropertyChanged");

    // Central Lock
    NavigationExtCentralLock* cl = new NavigationExtCentralLock(m_navigationExt);
    // fix TMCRSM-2534
//    connect(this, &PlatformControllerGen3::platformMediaMounted, cl, &NavigationExtCentralLock::reserveLock);
//    connect(this, &PlatformControllerGen3::platformMediaUnmounted, cl, &NavigationExtCentralLock::useLock);
    connect(cl, &NavigationExtCentralLock::centralLockStateChanged, this, &PlatformControllerGen3::onSpmCentralLockStateChanged);
    m_CentralLock = cl;

    // Mao: SKT update the startup status incorrect, you should update the start state firstly,
    // and then request the property, method calls. Please check below DBUS message which in your log file.
    m_CentralLock->useLock(); // this must be called after update Start State by NaviMethThirdPartyNaviStartState

    // platform initialized
    emit platformInitialized();
}

void PlatformControllerGen3::onSecureDataCommunicationRegistered()
{
    QDBusConnection c = QDBusConnection::sessionBus();
    if (!c.isConnected()) {
        qWarning("securedatacom dbus not connected.");
        return;
    }

    m_secureDataCom = nullptr;
    m_secureDataCom = new ComBoschAutomotiveProxySecureDataCommunicationInterface(AUTOMOTIVEPROXY_SERVICENAME,SECUREDATACOM_PATH, c);

    if (m_secureDataCom == nullptr || !m_secureDataCom->isValid()) {
        qWarning(sys) << "Cannot connect to agw-secure data communication";
        return;
    }

    // todo something

}

void PlatformControllerGen3::onSpmCentralLockStateChanged(int lockState) {
    qDebug() << "platform, CL locked:" << lockState;
    emit platformSpmCentralLockStateUpdated(lockState);
}

void PlatformControllerGen3::onPlatformSpmCentralLocStateRequested(int state)
{
    if (!m_CentralLock) return;

    switch (state) {
       case NavigationExtCentralLock::CL_Request_Use:
           m_CentralLock->useLock();
        break;
       case NavigationExtCentralLock::CL_Request_Reserve:
           m_CentralLock->reserveLock();
        break;
    case NavigationExtCentralLock::CL_Request_Lock:
           m_CentralLock->lock();
        break;
    default:
        qWarning() <<"platform, invalid lock state "<<state<<" reuqested !";
    }
}


void PlatformControllerGen3::sendGadgetImage(const QImage& gadgetImage, int type) {
    if (!m_gadgetStopped) {
        onSendGadgetImage(gadgetImage, type);
    }
}

void PlatformControllerGen3::onSendGadgetImage(QImage gadgetImage, int type) {
    QMutexLocker lock(&mGadgetMutex);
#ifdef BUILD_TARGET
    qDebug() << "onSendGadgetImage byteCount=" << gadgetImage.byteCount() << "type="<< type;
//    QDateTime system_time;
//    NXDateTimeDetailInfo dateTimeDetailInfo = m_navigationExt->naviPropertyUpdateDateTimeDetailInfo();
//    QString mMilSec = "000";
//    QString mDate;
//    QTextStream out(&mDate);
//    out << dateTimeDetailInfo.m_year
//        << "-"
//        << dateTimeDetailInfo.m_month
//        << "-"
//        << dateTimeDetailInfo.m_day
//        << " "
//        << dateTimeDetailInfo.m_hours
//        << ":"
//        << dateTimeDetailInfo.m_minutes
//        << ":"
//        << dateTimeDetailInfo.m_seconds
//        << "."
//        << mMilSec;
//    system_time = QDateTime::fromString(mDate, "yyyy-M-d H:m:s.zzz");
//    qDebug() << "onSendGadgetImage system_time=" << system_time.toString("yyyy-M-d H:m:s.zzz");

#ifdef USE_SYNCBLOCK
//        if (m_producer) {
//            qDebug() << "onSendGadgetImage m_producer->setBuffer";

////            QImage mirroedImage = gadgetImage.mirrored(0, 1);
//            m_producer->setBuffer(gadgetImage);
//        }
    switch(type)
    {
        case TmapController::ScreenId::Nav_Gadget_Map_XXL:
            {
                if (m_mapgadgetXXL_producer) {
                    T_PERF_LOG() << "onSendGadgetImage m_mapgadgetXXL_producer->setBuffer";
                    //T_PERF_LOG() << "[Perf][Gadget][PlatformController]data.bits: " << gadgetImage.constBits();
                    m_mapgadgetXXL_producer->setBuffer(gadgetImage);
                }
            }
            break;
        case TmapController::ScreenId::Nav_Gadget_Map_XL:
            {
                if (m_mapgadgetXL_producer) {
                    qDebug() << "onSendGadgetImage m_mapgadgetXL_producer->setBuffer";
                    m_mapgadgetXL_producer->setBuffer(gadgetImage);
                }
            }
            break;
        case TmapController::ScreenId::Nav_Gadget_Map_L:
        {
            if (m_mapgadgetL_producer) {
                qDebug() << "onSendGadgetImage m_mapgadgetL_producer->setBuffer";
                m_mapgadgetL_producer->setBuffer(gadgetImage);
            }
        }
        break;
            case TmapController::ScreenId::Nav_Gadget_Map_M:
        {
            if (m_mapgadgetM_producer) {
                qDebug() << "onSendGadgetImage m_mapgadgetM_producer->setBuffer";
                m_mapgadgetM_producer->setBuffer(gadgetImage);
            }
        }
        break;
        case TmapController::ScreenId::Nav_Gadget_Map_S:
            {
                if (m_mapgadgetS_producer) {
                    qDebug() << "onSendGadgetImage m_mapgadgetS_producer->setBuffer";
                    m_mapgadgetS_producer->setBuffer(gadgetImage);
                }
            }
            break;
        case TmapController::ScreenId::Nav_Gadget_Parking:
            {
                if (m_parkinggadget_producer) {
                    qDebug() << "onSendGadgetImage m_parkinggadget_producer->setBuffer";
                    //QImage mirroedImage = gadgetImage.mirrored(0, 1);
                    m_parkinggadget_producer->setBuffer(gadgetImage);
                }
            }
            break;
        case TmapController::ScreenId::Nav_Gadget_OilStation:
            {
                if (m_oilstationgadget_producer) {
                    qDebug() << "onSendGadgetImage m_oilstationgadget_producer->setBuffer";
                    //QImage mirroedImage = gadgetImage.mirrored(0, 1);
                    m_oilstationgadget_producer->setBuffer(gadgetImage);
                }
            }
            break;
        case TmapController::ScreenId::Nav_Gadget_AQI:
            {
                if (m_aqigadget_producer) {
                    qDebug() << "onSendGadgetImage m_aqigadget_producer->setBuffer";
                    //QImage mirroedImage = gadgetImage.mirrored(0, 1);
                    m_aqigadget_producer->setBuffer(gadgetImage);
                }
            }
            break;
        case TmapController::ScreenId::Nav_Gadget_Contact:
            {
                if (m_contactgadget_producer) {
                    qDebug() << "onSendGadgetImage m_contactgadget_producer->setBuffer";
                    //QImage mirroedImage = gadgetImage.mirrored(0, 1);
                    m_contactgadget_producer->setBuffer(gadgetImage);
                }
            }
            break;
        default:
            break;
    }
#endif
#endif
}

void PlatformControllerGen3::onProfileManagerRegistered() {
    qDebug() << "Create dbus interfaces onProfileManagerRegistered...rhs";
    QDBusConnection c = QDBusConnection::sessionBus();
    if (!c.isConnected()) {
        qWarning("dbus not connected.");
        return;
    }


    m_profilemanager = new BoschCmProfileInterface(PROFILEMANAGER_NAME, PROFILEMANAGER_PATH, c);

    if (!m_profilemanager->isValid()) {
        qWarning("Cannot connect to service BoschCmProfileInterface");
        return;
    } else {
        NavigationProfilePropertyObserver *profileObserver = new NavigationProfilePropertyObserver(m_profilemanager); // self clean up
        connect(profileObserver, &NavigationProfilePropertyObserver::statusChanged, this, &PlatformControllerGen3::onStatusChanged);
        connect(profileObserver, &NavigationProfilePropertyObserver::profileDataChanged, this, &PlatformControllerGen3::onProfileDataChanged);

        connect(profileObserver, &NavigationProfilePropertyObserver::activeProfileChanged, m_controller, &TmapController::changeUserOccur);
        connect(profileObserver, &NavigationProfilePropertyObserver::activeProfileChanged, this, &PlatformControllerGen3::onProfileChanged);
        connect(profileObserver, &NavigationProfilePropertyObserver::profileNameChanged, m_controller, &TmapController::changeProfileNameOccur);
        connect(profileObserver, &NavigationProfilePropertyObserver::copyProfileChanged, m_controller, &TmapController::copyUserDataOccur);
        connect(profileObserver, &NavigationProfilePropertyObserver::createProfileChanged, m_controller, &TmapController::createUserOccur);
        connect(profileObserver, &NavigationProfilePropertyObserver::deleteProfileChanged, m_controller, &TmapController::deleteUserOccur);


        connect(m_controller, &TmapController::copyUserDataDone, this, &PlatformControllerGen3::onProfileCopied);
        connect(m_controller, &TmapController::createUserDone, this, &PlatformControllerGen3::onProfileCreated);
        connect(m_controller, &TmapController::deleteUserDone, this, &PlatformControllerGen3::onProfileDeleted);

#if 0 // current there are nothing to do in navi
    connect(profileObserver, &NavigationProfilePropertyObserver::profileImageChanged, this, &PlatformControllerGen3::setProfileImageChanged);
    connect(profileObserver, &NavigationProfilePropertyObserver::profileNameChanged, this, &PlatformControllerGen3::setProfileNameChanged);
#endif


        uint appid = doRegisterApp("procnavkor04");
//        uint appid = doRegisterApp("procnavkor04", applName, reloadData);
//        qDebug() << "doRegisterApp - appid:" << appid << ", applName:" << applName << ", reloadData:" << reloadData;
        m_ProfileAppId = appid;

    }
}

void PlatformControllerGen3::onProfileCreated(uint ok) const
{
    qDebug() << "pm create profile done:" << ok;
    QDBusPendingReply<> reply = m_profilemanager->onProfileCreated(m_ProfileAppId, ok?PROFILE_CLIENT_RESULT_SUCCESS:PROFILE_FAILED);
    QDBusPendingCallWatcher* w = new QDBusPendingCallWatcher(reply, const_cast<PlatformControllerGen3*>(this));
    connect(w, &QDBusPendingCallWatcher::finished, [](QDBusPendingCallWatcher* call) {
            if (call->isError()) {
                qWarning() << "onProfileCreated error";
            }
            call->deleteLater();
        });
}

void PlatformControllerGen3::onProfileCopied(uint ok) const
{
    qDebug() << "pm copy profile done:" << ok;
    QDBusPendingReply<> reply = m_profilemanager->onProfileCopied(m_ProfileAppId, ok?PROFILE_CLIENT_RESULT_SUCCESS:PROFILE_FAILED);
    QDBusPendingCallWatcher* w = new QDBusPendingCallWatcher(reply, const_cast<PlatformControllerGen3*>(this));
    connect(w, &QDBusPendingCallWatcher::finished, [](QDBusPendingCallWatcher* call) {
            if (call->isError()) {
                qWarning() << "onProfileCopied error";
            }
            call->deleteLater();
        });
}

void PlatformControllerGen3::onProfileDeleted(uint ok) const
{
    qDebug() << "pm delete profile done:" << ok;
    QDBusPendingReply<> reply = m_profilemanager->onProfileDeleted(m_ProfileAppId, ok?PROFILE_CLIENT_RESULT_SUCCESS:PROFILE_FAILED);
    QDBusPendingCallWatcher* w = new QDBusPendingCallWatcher(reply, const_cast<PlatformControllerGen3*>(this));
    connect(w, &QDBusPendingCallWatcher::finished, [](QDBusPendingCallWatcher* call) {
            if (call->isError()) {
                qWarning() << "onProfileDeleted error";
            }
            call->deleteLater();
        });
}

void PlatformControllerGen3::onProfileDataChanged(uint reason) const
{
    qDebug() << "pm data changed: " << reason;
    // something to do

    quint32 res = PROFILE_DATA_CHANGED_RESULT_FAILED;
    switch (reason) {
        case PROFILE_DATA_CHANGED_REASON_PREPARE:
        res = PROFILE_DATA_CHANGED_RESULT_PREPARED;
        break;
        case PROFILE_DATA_CHANGED_REASON_PROFILE_CHANGED:
        res = PROFILE_DATA_CHANGED_RESULT_PROFILE_CHANGED;
        break;
        case PROFILE_DATA_CHANGED_REASON_IDLE:
        case PROFILE_DATA_CHANGED_REASON_PROFILE_RESET:
        case PROFILE_DATA_CHANGED_REASON_DOWNLOAD:
        default:
        res = PROFILE_DATA_CHANGED_RESULT_PROFILE_CHANGED;
        break;
    }

    QDBusPendingReply<> reply = m_profilemanager->onDataChanged(m_ProfileAppId, res);
    // MUST not wait response, because there is not any response from server.
        // reply.waitForFinished();
        // if (reply.isError()) {
        //     qWarning() << "pm onDataChanged error: " << reply.error().message();
        //     return;
        // }
    if (reply.isError())
            qWarning() << "pm onDataChanged error: " << reply.error().message();
}


void PlatformControllerGen3::onStatusChanged(uint profileStatus) const
{
    qDebug() << "pm status changed: " << profileStatus;
}



void PlatformControllerGen3::onHMIZoneInterfaceRegistered() {
    qDebug() << "Create dbus interfaces onHMIZoneInterfaceRegistered...rhs";
    QDBusConnection c = QDBusConnection::sessionBus();
    if (!c.isConnected()) {
        qWarning("dbus not connected.");
        return;
    }

    m_hmiZoneManager = new BoschCmAiNissanHmiZoneDataServiceHMIZoneDataInterface(HMIZONE_SERVICE_NAME, HMIZONE_SERVICE_PATH, c);


    if (!m_hmiZoneManager->isValid()) {
        qWarning("Cannot connect to service BoschCmProfileInterface");
        return;
    } else {

        qDebug() << "m_hmiZoneManager is valid" << endl ;
        connect(m_hmiZoneManager, SIGNAL(ActiveNotificationType(uint)), this, SLOT(onActiveNotificationType(uint)));
        connect(m_hmiZoneManager, SIGNAL(buttonPressed(uchar, uint, uchar)), this, SLOT(onButtonPressed(uchar, uint, uchar)));
        connect(m_hmiZoneManager, SIGNAL(keyboardInputFinished(uchar, const QString)), this, SLOT(onKeyboardInputFinished(uchar, const QString)));
        connect(m_hmiZoneManager, SIGNAL(keyboardKeyPressed(uchar, const QString&, const QString&, bool, bool)), this, SLOT(onKeyboardKeyPressed(uchar, const QString, const QString, bool, bool)));
        connect(m_hmiZoneManager, SIGNAL(pullupButtonPressed(uchar, uchar, uchar)), this, SLOT(onPullupButtonPressed(uchar, uchar, uchar)));
        connect(m_hmiZoneManager, SIGNAL(togglePressed(uchar, uint, uchar, bool)), this, SLOT(onTogglePressed(uchar, uint, uchar, bool)));
    }


    // initialize property observer
    HMIZoneDataPropertyObserver *observer = new HMIZoneDataPropertyObserver(m_hmiZoneManager); // self clean up
    setZone3PullUpListState(observer->zone3PullUpListState());

    connect(observer, &HMIZoneDataPropertyObserver::zone3PullUpListStateChanged, this, &PlatformControllerGen3::setZone3PullUpListState);
}

void PlatformControllerGen3::onActiveNotificationType(uint activeNotification) {
    qDebug() << "onActiveNotificationType(activeNotification: " << activeNotification << endl;
}

void PlatformControllerGen3::onButtonPressed(uchar appId, uint buttonId, uchar buttonState) {
    qDebug() << "onButtonPressed(appId:" << appId << ", buttonId: " << buttonId << ", buttonState: " << buttonState << endl;
}

void PlatformControllerGen3::onKeyboardInputFinished(uchar appId, const QString &searchbarText) {
    qDebug() << "onKeyboardInputFinished(appId:" << appId << ", searchbarText: " << searchbarText << endl;
}

void PlatformControllerGen3::onKeyboardKeyPressed(uchar appId, const QString &searchbarText, const QString &keyPressed, const bool IsCtrlKey, const bool IsSpeedLockActive) {
    qDebug() << "onKeyboardKeyPressed(appId:" << appId << ", searchbarText: " << searchbarText << ", keyPressed: " << keyPressed << ", IsCtrlKey: " << IsCtrlKey << ", IsSpeedLockActive: " << IsSpeedLockActive << endl;
    setSearchBarText(searchbarText);
}

void PlatformControllerGen3::onPullupButtonPressed(uchar appId, uchar buttonId, uchar buttonState) {
    qDebug() << "onPullupButtonPressed(appId:" << appId << ", buttonId: " << buttonId << ", buttonState: " << buttonState << endl;

}

void PlatformControllerGen3::onTogglePressed(uchar appId, uint buttonId, uchar buttonState, bool toggleValue) {
    qDebug() << "onTogglePressed(appId:" << appId << ", buttonId: " << buttonId << ", buttonState: " << buttonState << ", , toggleValue: " << toggleValue << endl;
}

void PlatformControllerGen3::onLifecycleRequest(uint mode, uint request_id)
{
    qDebug() << "LifeCycleRequest::: mode:" << mode << "  request_id:" << request_id << "  m_LifeCycleManagerRequestId:" << m_lifeCycleManagerRequestId;
    if (request_id == m_lifeCycleManagerRequestId)
    {
        switch (mode)
        {
        case LCMShutdownType_RUNUP:
            {
                if(m_lifeCycleManager)
                    m_lifeCycleManager->LifecycleRequestComplete(request_id);
                else
                    qDebug() << "m_lifeCycleManager is NULL!!!!!!!!!!!!!!!!!!!!!!";
            }
            break;
        case LCMShutdownType_NORMAL:
            {
                if(m_lifeCycleManager)
                    m_lifeCycleManager->LifecycleRequestComplete(request_id);
                else
                    qDebug() << "m_lifeCycleManager is NULL!!!!!!!!!!!!!!!!!!!!!!";
            }
            break;
        default:
            qDebug() << "LifeCycleRequest::: This mode(" << mode << ") is invalid value.";
        }
    }
}


void PlatformControllerGen3::onInit_GUI(uint u32LayerID, uint u32SurfaceID, uint u32Width, uint u32Height)
{
    qDebug() << "onInit_GUI: layerId=" << u32LayerID << ", surfaceId=" << u32SurfaceID
             <<", width=" << u32Width << ", height=" << u32Height;

    m_appSurface.SetWidth(u32Width);
    m_appSurface.SetHeight(u32Height);
    m_appSurface.SetLayerId(u32LayerID);
    m_appSurface.SetSurfaceId(u32SurfaceID);
    m_appSurface.SetSurfaceVisibility(false);

    m_popupProperty = getPopupProperty();
    emit setPopupInfo(m_popupProperty.m_LayerId, m_popupProperty.m_SurfaceId, m_popupProperty.m_Width, m_popupProperty.m_Height);

    m_toastPopupProperty = getToastPopupProperty();
    emit setToastPopupInfo(m_toastPopupProperty.m_LayerId, m_toastPopupProperty.m_SurfaceId, m_toastPopupProperty.m_Width, m_toastPopupProperty.m_Height);

    emit platformUIRequested();
}

void PlatformControllerGen3::onContextSwitchRequest(uint ContextSwitchRequest)
{
    qDebug() << "[CTX]onContextSwitchRequest ContextSwitchRequest: " << ContextSwitchRequest << ", m_lmInitialized: " << m_lmInitialized << " mMapViewVisible: " << mMapViewVisible << " m_guiControl: " << m_guiControl;

    if (m_guiControl) {
        m_contextSwitchRequest = ContextSwitchRequest;

        // Bosch guide에 따라 리턴하는 Status 변경.
        // 19.05.22 ymhong

        qDebug() << "[CTX]onContextSwitchRequest #1";

        if (ContextSwitchRequest == MAP_MAIN || ContextSwitchRequest == NAVIGATION_SERVICE) {
            if (m_lmInitialized) {                
                bool isComplete = false;

                if (mMapViewVisible) {                    
                    isComplete = true;
                }
                else {
                    mContextSwitchResponseForComplete = true;
                }

                qDebug() << "[CTX]onContextSwitchRequest #2 mMapViewVisible: " << mMapViewVisible << " isComplete: " << isComplete;

                if (isComplete) {
                    m_gadgetStopped = true;
                    emit platformContextSwitchRequested(QVariantMap{
                        {QStringLiteral("request"), ContextSwitchRequest}
                    });
                }
            } else {
                m_guiControl->ContextSwitchResponse(CONTEXT_STATE_REJECTED);
                qDebug() << "[CTX] m_guiControl->ContextSwitchResponse(CONTEXT_STATE_REJECTED)";
            }
        } else {
            m_guiControl->ContextSwitchResponse(CONTEXT_STATE_INVALID);
            qDebug() << "[CTX] m_guiControl->ContextSwitchResponse(CONTEXT_STATE_INVALID)";
        }
    }
}

void PlatformControllerGen3::onResetToFactorySettings()
{
    qDebug() << "onResetToFactorySettings" ;
    QMetaObject::invokeMethod(m_controller, "onResetToFactorySettings");
}

void PlatformControllerGen3::onNaviSignalRFAntennaStatus(uchar u8RFAntenna, uchar u8RFAntennaStatus, uchar u8ConnectionCurrent)
{
    qDebug() << "onNaviSignalRFAntennaStatus::" << "RFAntenna:" << u8RFAntenna << ", RFAntennaStatus:" << u8RFAntennaStatus << ", ConnectionCurrent:" << u8ConnectionCurrent;
    emit platfromNaviSignalRFAntennaStatus(u8RFAntenna, u8RFAntennaStatus, u8ConnectionCurrent);
}

void PlatformControllerGen3::onNaviSignalAppStateNavigation(uchar status)
{
    qDebug() << "onNaviSignalAppStateNavigation ::: " << status;

    if(status == 1) //SPCX Navi(KAKAO NAvi) RG Start
    {
        qDebug() << "onNaviSignalAppStateNavigation ::: #1";

        if(m_controller)
        {
            if(mNaviAppStatue != NULL){
                mNaviAppStatue->AndroidAutoStatus = true;

                if(mNaviAppStatue->VideoStream){
                    qDebug() << "onNaviSignalAppStateNavigation ::: #1 pauseMapinCluster";
                    m_controller->pauseMapInCluster();
                }

                onMapVideoStatus();
                mNaviAppStatue->AlramMute = m_controller->muteState(0);
                mNaviAppStatue->AudioMute = m_controller->muteState(1);
            }

            m_controller->setMuteState(0, true);
            m_controller->setMuteState(1, true);
        }
    }else if(status == 2) //SPCX Navi(KAKAO NAvi) RG Cancel
    {
        qDebug() << "onNaviSignalAppStateNavigation ::: #2 ";
        if(m_controller)
        {
            if(mNaviAppStatue != NULL)
            {
                mNaviAppStatue->AndroidAutoStatus = false;

                if(mNaviAppStatue->VideoStream)
                {
                    if(m_ClusterHandleId != 0)
                    {
                        qDebug() << "onNaviSignalAppStateNavigation ::: #1 resumeMapinCluster";

                       m_controller->resumeMapInCluster();
                    }else{
                        if(mVideoStreamInfo != NULL)
                        {
                            qDebug() << "onNaviSignalAppStateNavigation ::: #1 startMapinCluster";
                            int layout = MapInClusterLayout::CenteralMap;
//                            if(mVideoStreamInfo->VideoStreamDemenstions.m_width == MapInClusterLayout::SmallLayoutWidth && mVideoStreamInfo->VideoStreamDemenstions.m_height == MapInClusterLayout::SmallLayoutHeight)
//                            {
//                                layout = MapInClusterLayout::SideMap;
//                            }

                            m_ClusterHandleId = m_controller->VIDEO_STREAM_HANDLE;
                            m_controller->setMapInClusterIpPort(mVideoStreamInfo->VideoStreamIP, mVideoStreamInfo->VideoStreamPort, mVideoStreamInfo->VideoType, mVideoStreamInfo->VideoStreamDemenstions.m_width, mVideoStreamInfo->VideoStreamDemenstions.m_height); //[TI190920] make a wrapper function to emit stopMapIncluster and call it by using QMetaObject::invokeMethod
                            m_controller->startMapInCluster(layout); //[TI190920] for consistency with others, make a wrapper function that emits startMapInCluster and call it by using QMetaObject::invokeMethod
                        }
                    }
                }

                m_controller->setMuteState(0, mNaviAppStatue->AlramMute);
                m_controller->setMuteState(1, mNaviAppStatue->AudioMute);
            }
        }
    }
    emit spcxRgEnabled(status == 1);
}

void PlatformControllerGen3::onUpdateSurfaceState(uint u32CommandID, uint SurfaceState)
{
    T_PERF_LOG() << "[Perf][Platform][Surface]onUpdateSurfaceState: u32CommandID=" << u32CommandID << "SurfaceState=" << SurfaceState;

    bool foreground = SurfaceState == HMISurfaceState_HMI_SURFACE_FG;

    setAppForeground(foreground);

    if(SurfaceState == 1)
    {

    }

    if(!m_bScreenOn)
    {
        if(m_bNaviInit)
        {
            if (m_navigationServer)
                m_navigationServer->setNavStatus(NavStatus::NavStatus_NAVSTATUS_IDLE);
        }
    }
    m_bScreenOn = true;

    if (m_guiControl) {
        QDBusPendingReply<> reply = m_guiControl->UpdateSurfaceStateResp(SurfaceState,
                                                                         m_appSurface.GetSurfaceId(),
                                                                         u32CommandID);
        QDBusPendingCallWatcher* w = new QDBusPendingCallWatcher(reply, this);
        connect(w, &QDBusPendingCallWatcher::finished, [](QDBusPendingCallWatcher* call){
            if (call->isError()) {
                qWarning() << "update surface state failed: "<<call->error().message();
            }
            call->deleteLater();
        });
    }
}

void PlatformControllerGen3::onPopupStatus(uint surfaceId, uint popupstatus) {
    qDebug() << "onPopupStatus: surfaceId: " << surfaceId << ", popupstatus: " << popupstatus ;
}

void PlatformControllerGen3::onFocusConfigProvider(char focusConfig)
{
    qDebug() << "onFocusConfigProvider ::: " << (int)focusConfig;
    if (m_focusConfig != focusConfig) {
        m_focusConfig = focusConfig;
        emit platformfocusConfigChanged(focusConfig);
    }
}

void PlatformControllerGen3::onNaviGadgetFocusInteractionSignal(FocusInteractionInfo info)
{
    auto instanceId = info.instanceId;
    auto interactionType = info.interactionType;

    qDebug() << "[GEN3] onNaviGadgetFocusInteractionSignal(" << instanceId << ", " << interactionType << ")";

    emit platformNaviGadgetFocusInteractionSignal(instanceId, interactionType);
}

void PlatformControllerGen3::onNaviGadgetFocusStatusChanged(uint instanceId, bool focus)
{
    qDebug() << "[GEN3] onNaviGadgetFocusStatusChanged(" << instanceId << ", " << focus << ")";

    emit platformNaviGadgetFocusStatusChanged(instanceId, focus);
}

void PlatformControllerGen3::onActivateExternalImageProvider(uint activate, QList<int> instanceIds, int consumerId) {
    QMutexLocker lock(&mGadgetMutex);

    if(!m_bScreenOn)
    {
        if(m_bNaviInit)
        {
            if (m_navigationServer)
                m_navigationServer->setNavStatus(NavStatus::NavStatus_NAVSTATUS_IDLE);
        }
    }
    m_bScreenOn = true;

    qDebug() << "[Gadget][Platform][Gadget]onActivateExternalImageProvider " << m_controller->naviProperty()->getTmapLocalMapVersion();
    //DB 없는 경우 Signal 전달안함
    if(m_controller->naviProperty()->getTmapLocalMapVersion().isNull())
        return;

#ifdef USE_SYNCBLOCK
    if(!m_controller) {
        qDebug() << "[Gadget][Platform][Gadget]onActivateExternalImageProvider #1 activate: " << activate << " !m_controller ";
        return;
    }

    bool isRepeat = false;
    if(!m_tempInstanceIds.isEmpty())
    {
        if(m_tempInstanceIds.size() == instanceIds.size())
        {
            isRepeat = true;

            for(int i=0;i<instanceIds.size();i++){
                if(m_tempInstanceIds.at(i) != instanceIds.at(i))
                {
                    isRepeat = false;
                }
            }

            if(m_tempActivate != activate)
                isRepeat = false;
        }
    }

    m_tempInstanceIds   = instanceIds;
    m_tempActivate      = activate;

    if(isRepeat)
        return;

    bool isActivate = false;

    if(activate == 0)
        isActivate = true;

    if ( !instanceIds.isEmpty() ) {
        for ( auto instanceId : instanceIds ) {
            T_PERF_LOG() << "[Perf][Gadget][Platform][Gadget]onActivateExternalImageProvider #3 activate: " << activate << " instanceId: " << instanceId;

            if (isActivate) {
                switch (instanceId) {
                case TmapController::ScreenId::Nav_Gadget_Map_XXXL:
                    break;
                case TmapController::ScreenId::Nav_Gadget_Map_XXL:
                    m_mapgadgetXXL_producer->init(instanceId);
                    emit platformActivateExternalImageProvider(true, instanceId);
                    break;
                case TmapController::ScreenId::Nav_Gadget_Map_XL:
                    m_mapgadgetXL_producer->init(instanceId);
                    emit platformActivateExternalImageProvider(true, instanceId);
                    break;
                case TmapController::ScreenId::Nav_Gadget_Map_L:
                    m_mapgadgetL_producer->init(instanceId);
                    emit platformActivateExternalImageProvider(true, instanceId);
                    break;
                case TmapController::ScreenId::Nav_Gadget_Map_M:
                    m_mapgadgetM_producer->init(instanceId);
                    emit platformActivateExternalImageProvider(true, instanceId);
                    break;
                case TmapController::ScreenId::Nav_Gadget_Map_S:
                    m_mapgadgetS_producer->init(instanceId);
                    emit platformActivateExternalImageProvider(true, instanceId);
                    break;

                /*멀티가젯 동작 이상으로 주석처리 중..*/
                case TmapController::ScreenId::Nav_Gadget_Parking:
                    qDebug() << "[Gadget][Platform]onActivateExternalImageProvider #5 instanceId: " << instanceId;
                    m_parkinggadget_producer->init(instanceId);
                    qDebug() << "[Gadget][Platform]onActivateExternalImageProvider #6 instanceId: " << instanceId;
                    emit platformActivateExternalImageProvider(true, instanceId);
                    break;
                case TmapController::ScreenId::Nav_Gadget_OilStation:
                    qDebug() << "[Gadget][Platform]onActivateExternalImageProvider #7 instanceId: " << instanceId;
                    m_oilstationgadget_producer->init(instanceId);
                    qDebug() << "[Gadget][Platform]onActivateExternalImageProvider #8 instanceId: " << instanceId;
                    emit platformActivateExternalImageProvider(true, instanceId);
                    break;
                case TmapController::ScreenId::Nav_Gadget_Contact:
                    qDebug() << "[Gadget][Platform]onActivateExternalImageProvider #9 instanceId: " << instanceId;
                    m_contactgadget_producer->init(instanceId);
                    qDebug() << "[Gadget][Platform]onActivateExternalImageProvider #10 instanceId: " << instanceId;
                    //emit platformActivateExternalImageProvider(true, instanceId);
                    break;
                case TmapController::ScreenId::Nav_Gadget_AQI:
                    qDebug() << "[Gadget][Platform]onActivateExternalImageProvider #10 instanceId: " << instanceId;
                    m_aqigadget_producer->init(instanceId);
                    qDebug() << "[Gadget][Platform]onActivateExternalImageProvider #11 instanceId: " << instanceId;
                    emit platformActivateExternalImageProvider(true, instanceId);
                    break;
                }
            }
            else {
                emit platformActivateExternalImageProvider(false, instanceId);

//                if(m_mapgadgetXXL_producer){
//                    m_mapgadgetXXL_producer->startTermination();
//                }
//                if(m_mapgadgetXL_producer){
//                    m_mapgadgetXL_producer->startTermination();
//                }
//                if(m_mapgadgetL_producer){
//                    m_mapgadgetL_producer->startTermination();
//                }
//                if(m_mapgadgetM_producer){
//                    m_mapgadgetM_producer->startTermination();
//                }
//                if(m_mapgadgetS_producer){
//                    m_mapgadgetS_producer->startTermination();
//                }

//                if(m_parkinggadget_producer){
//                    m_parkinggadget_producer->startTermination();
//                }

//                if(m_oilstationgadget_producer){
//                    m_oilstationgadget_producer->startTermination();
//                }

//                if(m_aqigadget_producer){
//                    m_aqigadget_producer->startTermination();
//                }

//                if(m_contactgadget_producer){
//                    m_contactgadget_producer->startTermination();
//                }
            }
        }
    }

#endif
}

void PlatformControllerGen3::onExternalImageProviderTouch(TouchInfo data, int instanceId) {
    qDebug() << "::::::- onExternalImageProviderTouch: TouchInfo.data: " << data.timeStamp << ", TouchInfo.touchState: " << data.touchState
             << ", TouchInfo.xPos: " << data.xPos << ", TouchInfo.yPos:" << data.yPos;
    qDebug() << "onExternalImageProviderTouch: instanceId= " << instanceId;

    T_PERF_LOG() << "[Perf][Gadget]onExternalImageProviderTouch: instanceId= " << instanceId;

    emit platformExternalImageProviderTouch(data.touchState, data.xPos, data.yPos, data.timeStamp, instanceId);

//    switch (instanceId) {
//    case TmapController::ScreenId::Nav_Gadget_Map_XXL:
//        if (data.touchState == 2) {
//            emit platformActivateExternalImageProvider(true, instanceId);
//            if (m_guiControl) {
//                m_guiControl->ChangeSurfaceState(HMISurfaceState_HMI_SURFACE_FG, 3);
//            }
//        }
//        break;
//    case TmapController::ScreenId::Nav_Gadget_Map_XL:
//    case TmapController::ScreenId::Nav_Gadget_Map_L:
//    case TmapController::ScreenId::Nav_Gadget_Map_M:
//    case TmapController::ScreenId::Nav_Gadget_Map_S:
//    case TmapController::ScreenId::Nav_Gadget_Map_XXXL:

//    default:
//        break;
//    }
}

void PlatformControllerGen3::onNetworkStateChanged(ConnManager::State state)
{
    setNetworkOnline(state >= ConnManager::Ready);
}

void PlatformControllerGen3::onCurrentStreetChanged(RGDataPtr value)
{
    emit platformCurrentStreetChanged(value);
    QString tmpCurrentStreet = TextDecoder::instance()->fromCP949(value->stCurrent.szPosRoadName);
    QString currentStreet = m_navigationServer->currentStreet();

    if(QString::compare(currentStreet, tmpCurrentStreet, Qt::CaseInsensitive) != 0) {
        qDebug() << "onCurrentStreetChanged #2" << tmpCurrentStreet;
        if(m_navigationServer != NULL){
            m_navigationServer->setCurrentStreet(tmpCurrentStreet);
        }
    }
}

void PlatformControllerGen3::onSatelliteInfoChanged(SatelliteDataPtr value)
{
    static SatelliteData lastSatellite;
    if (value) {
        // check there is difference from last
        bool diff = false;
        if (!diff)
           diff = value->fixType != lastSatellite.fixType;
        if (!diff)
           diff = value->satellites.count() != lastSatellite.satellites.count();
        if (!diff)
           diff = value->satellitesInUse != lastSatellite.satellitesInUse;
        if (!diff)
           diff = value->speed != lastSatellite.speed;
        if (!diff)
           diff = value->longitude != lastSatellite.longitude;
        if (!diff)
           diff = value->latitude != lastSatellite.latitude;

        int hdop1 = std::abs((int)lastSatellite.HDOP);
        int hdop2 = std::abs((int)value->HDOP);

        int rel1 = (hdop1 <= 10) ? 90 :
                   (hdop1 < 50) ?  50 : 0;

        int rel2 = (hdop2 <= 10) ? 90 :
                   (hdop2 < 50) ?  50 : 0;

        if (!diff)
           diff = rel1 != rel2;

        // update the property only if it gets changes
        if (diff) {
            lastSatellite.fixType         = value->fixType;
            lastSatellite.satellites      = value->satellites;
            lastSatellite.satellitesInUse = value->satellitesInUse;
            lastSatellite.speed           = value->speed;
            lastSatellite.longitude       = value->longitude;
            lastSatellite.latitude        = value->latitude;
            lastSatellite.HDOP            = value->HDOP;

            NSSatelliteInfo mNSSatelliteInfo;
            mNSSatelliteInfo.m_gnssMode                = (value->fixType == 3)?QStringLiteral("3D Fix"):QStringLiteral("No Fix");
            mNSSatelliteInfo.m_receivedSatellites      = value->satellites.count(); //unknown params
            mNSSatelliteInfo.m_usedSatellites          = value->satellitesInUse; //unknown params
            mNSSatelliteInfo.m_visibleSatellites       = value->satellites.count(); //unknown params
            mNSSatelliteInfo.m_gnssSpeed               = value->speed;
            mNSSatelliteInfo.m_gnssSpeedReliability    = (hdop2 <= 10) ? 99 :
                                                        (hdop2 <  50) ? 50 : 0;

            mNSSatelliteInfo.m_gnssPosition.m_latitude  = value->latitude;
            mNSSatelliteInfo.m_gnssPosition.m_longitude = value->longitude;

            if(m_navigationServer != NULL){
                m_navigationServer->setSatelliteInfo(mNSSatelliteInfo);
            }
        }
    }
}

void PlatformControllerGen3::onNavStartStatusChanged(int status)
{
    if (m_navigationExt) {
        m_navigationExt->NaviMethThirdPartyNaviStartState(status);
    }
}

void PlatformControllerGen3::onNavStatusChanged(int status)
{
    uint navStatus = status;
    if (status == 1) return; // 20191101 don't provide BUSY status due to cluster issue.

    if(status == NavStatus::NavStatus_NAVSTATUS_IDLE)
    {
        m_bNaviInit = true;
        if(!m_bScreenOn) //부팅 후 화면이 켜지기 전에 IDLE상태가 되면 이슈 발생 : (status == 4 : IDLE)
           return;
    }

    qDebug() << "platform navi status: " << status << ", " << navStatus;
    if (m_navigationServer)
    {
        m_navigationServer->setNavStatus(navStatus);

        if(navStatus != NavStatus_NAVSTATUS_GUIDANCE_ACTIVE)
           m_navigationServer->setRouteCancel();
    }
}


void PlatformControllerGen3::onMapVersionChanged(QString value)
{
    // (ex)
    qDebug() << "onMapVersionChanged ";
    mMapDataVersion = value;
    if(m_navigationServer != NULL){
        m_navigationServer->setDatasetVersion(mMapDataVersion);
    }
}

void PlatformControllerGen3::onDistanceToDestination(RGDataPtr value) {
    if(m_navigationServer->navStatus() != NavStatus::NavStatus_NAVSTATUS_GUIDANCE_ACTIVE && m_navigationServer->navStatus() != NavStatus::NavStatus_NAVSTATUS_NAVDATA_UPDATING)
        return;

//    if(m_navigationServer->navStatus() != NavStatus::NavStatus_NAVSTATUS_GUIDANCE_ACTIVE && m_navigationServer->navStatus() != NavStatus::NavStatus_NAVSTATUS_NAVDATA_UPDATING)
//        return;


    //  METERS          Uint32	0
    //  KILOMETERS      Uint32	1
    //  MILES           Uint32	2

    NSDistanceToDestination mNSDistanceToDestination;
    int nDist = value->nTotalDist;
    QString sDist = QString::number(nDist);

    mNSDistanceToDestination.m_distanceToDestination = nDist;

    if(nDist > 1000)
    {
        if(nDist < 10000){
            double DistKilo = (double)(nDist/1000.0);
            sDist = QString("%1%2").arg(QString::number(DistKilo, 'f', 1)).arg("km");
        }else{
            int DistKilo = nDist/1000;
            QString sDistKilo = QString::number(DistKilo);
            sDist = QString("%1%2").arg(sDistKilo).arg("km");
        }
    }else{
        QString sDistMeter = QString::number(nDist);
        sDist = QString("%1%2").arg(sDistMeter).arg("m");
    }

    mNSDistanceToDestination.m_distanceToDestinationAsString = sDist;
    mNSDistanceToDestination.m_distanceUnit = 0;
    if(m_navigationServer != NULL){
        m_navigationServer->setDistanceToDestination(mNSDistanceToDestination);
    }
}

/** Convert SKT turn code to Bosch turn code **/
int convertLaneTurnCode(int turncode) {
    int  boschTurnCode = 0;

    switch(turncode){
        case 1: boschTurnCode = 128; //U-Turn
            break;
        case 2: boschTurnCode = 2; //Left
            break;
        case 4: boschTurnCode = 8; //Half_Left
            break;
        case 8: boschTurnCode = 1; //Straight
            break;
        case 16: boschTurnCode = 16; //Half_Right
            break;
        case 32: boschTurnCode = 4;//Right
            break;
        defualt:
            break;
    }
    return boschTurnCode;
}

int getAbit(unsigned short x, int n) { // getbit()
  return (x & (1 << n));
}

void PlatformControllerGen3::onLaneRecommendationList(RGDataPtr value) {
    if(m_navigationServer->navStatus() != NavStatus::NavStatus_NAVSTATUS_GUIDANCE_ACTIVE && m_navigationServer->navStatus() != NavStatus::NavStatus_NAVSTATUS_NAVDATA_UPDATING)
        return;

    NSLaneRecommendationList mNSLaneRecommendationList;
    NSLaneRecommendation mNSLaneRecommendation;

    if(value.data()->bLane){
        qDebug() << "==================================================";

        for(int i = 0;i<value.data()->stLane.nLaneCount;i++)
        {
            int uturn       = (getAbit((value.data()->stLane.nLaneTurnInfo[i]),0));
            int left        = (getAbit((value.data()->stLane.nLaneTurnInfo[i]),1));
            int halfLeft    = (getAbit((value.data()->stLane.nLaneTurnInfo[i]),2));
            int str         = (getAbit((value.data()->stLane.nLaneTurnInfo[i]),3));
            int halfRight   = (getAbit((value.data()->stLane.nLaneTurnInfo[i]),4));
            int right       = (getAbit((value.data()->stLane.nLaneTurnInfo[i]),5));

            int turnCode = convertLaneTurnCode(uturn) + convertLaneTurnCode(left) + convertLaneTurnCode(halfLeft) + convertLaneTurnCode(str) + convertLaneTurnCode(halfRight) + convertLaneTurnCode(right);

            int availableUturn   = (getAbit((value.data()->stLane.nLaneAvailable[i]),0));
            int availableLeft    = (getAbit((value.data()->stLane.nLaneAvailable[i]),1));
            int availableHLeft   = (getAbit((value.data()->stLane.nLaneAvailable[i]),2));
            int availableStr     = (getAbit((value.data()->stLane.nLaneAvailable[i]),3));
            int availableHRight  = (getAbit((value.data()->stLane.nLaneAvailable[i]),4));
            int availableRight   = (getAbit((value.data()->stLane.nLaneAvailable[i]),5));

            int availableConvertCode = convertLaneTurnCode(availableUturn) + convertLaneTurnCode(availableLeft) + convertLaneTurnCode(availableHLeft) + convertLaneTurnCode(availableStr) + convertLaneTurnCode(availableHRight) + convertLaneTurnCode(availableRight);

            mNSLaneRecommendation.m_combinedFollowingLaneDirection  = turnCode;
            mNSLaneRecommendation.m_guidedFollowingLaneDirection    = availableConvertCode;
            if(availableConvertCode != 0){
                mNSLaneRecommendation.m_laneStatus = 3; //3 : RECOMMAND
            }else{
                mNSLaneRecommendation.m_laneStatus = 2; //2 : NON RECOMMAND
                mNSLaneRecommendation.m_combinedFollowingLaneDirection  = 0;
                mNSLaneRecommendation.m_guidedFollowingLaneDirection    = 0;
            }

            mNSLaneRecommendationList.append(mNSLaneRecommendation);
        }
    }

    m_navigationServer->setLaneRecommendationList(mNSLaneRecommendationList);
}

enum CameraType {
    FIXED_SPEED             = 0, //ECAM 과속 카메라 (고정식)
    MOBILE_SPEED            = 1, //MCAM 과속 카메라 (이동식)
    REDLIGHT                = 2, //ESGNC 신호단속 카메라 (고정식)
    FIXED_SPEED_AND_REDLIGHT = 6, //ESSGC 신호과속 카메라 (고정식)
    BUSLANE                 = 7, //EBUSC 버스전용 카메라 (고정식)
    PARKING_VIOLATION       = 9, //EPAKC 주정차 단속 카메라 (고정식)
    AVERAGE_SPEED_ZONE      = 11, //ACAMB ACAME 구간단속 시점 구간단속 종점
    ACCIDENT_BLACKSPOT_ZONE = 15 //ACC 사고다발지역
};

void PlatformControllerGen3::onTrafficEnforcement(RGDataPtr value) {
    NSTrafficEnforcement mTrafficEnforcement;

    if(value.data()->bSDI)
    {
        mTrafficEnforcement.m_TrafficEnforcementAlertState = 1; //0:INACTIVE 1:ACTIVE 2:ERROR
        mTrafficEnforcement.m_trafficEnforcementDistanceType = 0; //0:BEGIN /1:END
        switch(value.data()->stSDI[0].nSdiType)
        {
            case 0: //   SDI_SPEED_ACCIDENT_POS // 3. 신호과속
                mTrafficEnforcement.m_trafficEnforcementCameraType = CameraType::FIXED_SPEED; //
                break;
            case 1: //   SDI_SPEED_LIMIT_POS:1. 과속 (고정식)
                mTrafficEnforcement.m_trafficEnforcementCameraType = CameraType::FIXED_SPEED;
                break;
            case 2: //   SDI_SPEED_BLOCK_START_POS:구간단속 시작
                mTrafficEnforcement.m_trafficEnforcementCameraType = CameraType::AVERAGE_SPEED_ZONE;
                mTrafficEnforcement.m_trafficEnforcementDistanceType = 0; //0:BEGIN /1:END
                break;
            case 3: //    SDI_SPEED_BLOCK_END_POS;구간단속 끝
                mTrafficEnforcement.m_trafficEnforcementCameraType = CameraType::AVERAGE_SPEED_ZONE;
                mTrafficEnforcement.m_trafficEnforcementDistanceType = 1; //0:BEGIN /1:END
                break;
            case 4: //    SDI_SPEED_BLOCK_MID_POS,구간단속중
                mTrafficEnforcement.m_trafficEnforcementCameraType = CameraType::AVERAGE_SPEED_ZONE;
                break;
            case 5: //    SDI_TAIL,							// 꼬리물기단속카메라
                break;
            case 6: //    SDI_SIGNAL_ACCIDENT_POS,			// 4. 신호 단속
                  mTrafficEnforcement.m_trafficEnforcementCameraType = CameraType::REDLIGHT;
                break;
            case 7: //    SDI_SPEED_LIMIT_DANGEROUS_AREA,		// 8. 과속 (이동식)
                mTrafficEnforcement.m_trafficEnforcementCameraType = CameraType::MOBILE_SPEED;
                break;
            case 8: //    SDI_BOX_SPEED_LIMIT_POS,			// 181. 박스형이동식(신규)
                mTrafficEnforcement.m_trafficEnforcementCameraType = CameraType::MOBILE_SPEED;
                break;
            case 9: //    SDI_BUS_LANE,						// 7. 버스전용차로구간
                mTrafficEnforcement.m_trafficEnforcementCameraType = CameraType::BUSLANE;
                break;
            case 10://    SDI_CHANGEROAD_POS,					// 17. 가변 차로 단속
                mTrafficEnforcement.m_trafficEnforcementCameraType = CameraType::BUSLANE;
                break;
            case 11://    SDI_ROAD_CONTROL_POS,				// 14. 갓길 감시 지점
                break;
            case 12://    SDI_INTRUDER_AREA,					// 15. 끼어들기 금지
                break;
            case 13://    SDI_TRAFFICINFO_COLLECT_POS,		// 6. 교통정보 수집지점
                break;
            case 14://    SDI_CCTV_AREA,						// 182. 방범용CCTV(신규)
                break;
            case 15: //    SDI_OVERLOAD_DAANGEROUS_AREA,		// 10. 과적차량 위험구간
                break;
            case 16: //    SDI_LOAD_BAD_CONTROL_POS,			// 16. 적재 불량 단속
                break;
            case 17: //    SDI_PARKING_CONTROL_POS,			// 13. 주차단속 지점
                 mTrafficEnforcement.m_trafficEnforcementCameraType = CameraType::PARKING_VIOLATION;
                 break;
            default:
                 mTrafficEnforcement.m_trafficEnforcementCameraType = -1;
            break;
        }
    }else{
        mTrafficEnforcement.m_TrafficEnforcementAlertState = 0;
        mTrafficEnforcement.m_trafficEnforcementCameraType = 0;
        mTrafficEnforcement.m_trafficEnforcementDistanceType = 0;
        mTrafficEnforcement.m_distance ="0";
        mTrafficEnforcement.m_distanceUnit = 0;
        mTrafficEnforcement.m_speedLimit = 0;
    }

    if( mTrafficEnforcement.m_trafficEnforcementCameraType != -1){

        QString sDist;

        if(value.data()->stSDI[0].nSdiDist > 1000)
        {
            if(value.data()->stSDI[0].nSdiDist < 10000)
            {
                double dist_kilo = (double)(value.data()->stSDI[0].nSdiDist/1000.0);
                sDist = QString("%1%2").arg(QString::number(dist_kilo, 'f', 1)).arg("km");
            }else{
                int dist_kilo = value.data()->stSDI[0].nSdiDist/1000;
                sDist = QString("1%2%").arg(QString::number(dist_kilo)).arg("km");
            }
        }else{
            sDist = QString("%1%2").arg(QString::number(value.data()->stSDI[0].nSdiDist)).arg("m");
        }

        mTrafficEnforcement.m_distance = sDist;
        mTrafficEnforcement.m_distanceUnit = 0;
        mTrafficEnforcement.m_speedLimit = value.data()->stSDI[0].nSdiSpeedLimit;;

        m_navigationServer->setTrafficEnforcement(mTrafficEnforcement);
    }
}

void PlatformControllerGen3::onArrivalInformationList(RGDataPtr value){
    if(m_navigationServer->navStatus() != NavStatus::NavStatus_NAVSTATUS_GUIDANCE_ACTIVE && m_navigationServer->navStatus() != NavStatus::NavStatus_NAVSTATUS_NAVDATA_UPDATING){
        return;
    }

    if(!VSMNavi_IsRoute())
        return;

    std::vector<VSM_OsTBTList> mTBTList;
    VSMNavi_GetRouteTBTList(VSM_TBTLIST_ALL, 0, value.data()->nTBTListCount, mTBTList);

    NSArrivalInformationList mNSArrivalInformationList;

    int nDist       = 0;
    int nTimeToDest  = 0;

    int nDistFromStart = 0;
    int nTimeFromStart = 0;

    nDistFromStart = value.data()->stTBTListPos.nAccDist;
    nTimeFromStart = value.data()->stTBTListPos.nAccTime;

    if(nDistFromStart > 0)
    {
        for(int i=value->stTBTListPos.nTBTIndex;i<mTBTList.size();i++)
        {
            NSArrivalInformation mNSArrivalInformation;
            NSDistanceToDestination mNSDistanceToDestination;

            bool isVIACode =false;

            qInfo() << "mTBTList.at(i).nTurnCode[" << i <<"]" << mTBTList.at(i).nTurnCode;
            //turncode 185:첫번째경유지/ 186:두번째경유지 / 187:세번째경유지 / 200 :출발지 / 201 : 목적지
            if(mTBTList.at(i).nTurnCode == 185 || mTBTList.at(i).nTurnCode == 186 || mTBTList.at(i).nTurnCode == 187 || mTBTList.at(i).nTurnCode == 201)
            {
                nDist = mTBTList.at(i).nAccDist - nDistFromStart;
                nTimeToDest = mTBTList.at(i).nAccTime - nTimeFromStart;
                isVIACode = true;
            }

            if(nDist < 0 || nTimeToDest < 0)
                continue;

            if(isVIACode)
            {
                float TotalDist;
                QString sDist;

                if(nDist > 1000){
                    if(nDist < 10000)
                    {
                        int dist_kilo = nDist/1000;
                        int dist_meter = nDist/100 - dist_kilo*10;
                        sDist = QString("%1%2%3%4").arg(QString::number(dist_kilo)).arg(".").arg(QString::number(dist_meter)).arg("km");
                    }else{
                        int dist_kilo = nDist/1000;
                        sDist = QString("%1%2").arg(QString::number(dist_kilo)).arg("km");
                    }
                }else{
                    sDist = QString("%1%2").arg(QString::number(nDist)).arg("m");
                }

                mNSDistanceToDestination.m_distanceToDestinationAsString = sDist;

                mNSDistanceToDestination.m_distanceUnit = 0;//METERS : 0 / KILOMETERS : 1 / FEET : 2 / YARD : 3
                mNSDistanceToDestination.m_distanceToDestination = nDist;
                mNSArrivalInformation.m_distanceToDestination = mNSDistanceToDestination;

                int hour = nTimeToDest/3600;
                int minute = nTimeToDest/60;
                mNSArrivalInformation.m_timeToDestinationAsString = QString("%1:%2").arg(QString::number(hour)).arg(QString::number(minute));
                mNSArrivalInformation.m_timeToDestination = nTimeToDest;

                //도착 예정시간을 구분해서 입력 필요
        //        QDateTime time = DateTimeUtil::currentUtcDateTime();
                QTime time = QTime::currentTime();
                mNSArrivalInformation.m_estimatedTimeOfArrivalAsString = time.addSecs(nTimeToDest).toString("hh:mm");
                mNSArrivalInformation.m_estimatedTimeOfArrivalHour =   time.addSecs(nTimeToDest).toString("hh").toInt();
                mNSArrivalInformation.m_estimatedTimeOfArrivalMinute =   time.addSecs(nTimeToDest).toString("mm").toInt();

                if(mTBTList.at(i).nTurnCode == 201) //201: destination
                    mNSArrivalInformation.m_estimatedTimeType = 0;
                else
                    mNSArrivalInformation.m_estimatedTimeType = 1;

                mNSArrivalInformation.m_displayTimeType = 0; //REMAININGTIME : 0 / ETA : 1
                mNSArrivalInformation.m_displayTimeFormat = 1; //UNSET : 0 / TWELVEHOURS : 1 / TWENTYFOURHOURS : 2
                mNSArrivalInformation.m_destinationIndex = 0;

                mNSArrivalInformationList.append(mNSArrivalInformation);
//                    qInfo() << "onArrivalInformationList turncode[ "<<i<<"]" << mTBTList.at(i).nTurnCode << mTBTList.at(i).nAccDist << mTBTList.at(i).nAccTime;
            }
        }
    }

    m_navigationServer->setArrivalInformationList(mNSArrivalInformationList);
}


void PlatformControllerGen3::onManeuverView(RGDataPtr value){
    if(m_navigationServer->navStatus() != NavStatus::NavStatus_NAVSTATUS_GUIDANCE_ACTIVE && m_navigationServer->navStatus() != NavStatus::NavStatus_NAVSTATUS_NAVDATA_UPDATING)
        return;

    //qDebug() << "onManeuverView";

    NSManeuverView mNSManeuverView;

    int nExtcImageShowDist = 0;

    if(value.data()->bExtcImage)
        nExtcImageShowDist = value.data()->nExtcImageShowDist;

    if(value.data()->bDirImage)
        nExtcImageShowDist = value.data()->nDirImageShowDist;

    if(value->stGuidePoint.nTBTDist == 0)
        return;

    bool isInDisplayableDist = nExtcImageShowDist >= value.data()->stGuidePoint.nTBTDist;

    if(isInDisplayableDist)
    {
        mNSManeuverView.m_intersectionMapId = value.data()->stGuidePoint.nTBTTurnType;
        if(value.data()->bExtcImage || value.data()->bDirImage){
            if(value.data()->bExtcImage)
                mNSManeuverView.m_maneuverViewType = 2; //0:NONE / 1:DEFORMED_IMAGE / 2:INTERSECTION_MAP

            if(value.data()->bDirImage)
                mNSManeuverView.m_maneuverViewType = 1; //0:NONE / 1:DEFORMED_IMAGE / 2:INTERSECTION_MAP

            m_navigationServer->setManeuverView(mNSManeuverView);
        }
    }
}

void PlatformControllerGen3::onManeuverSymbol(RGDataPtr value){
    if(m_navigationServer->navStatus() != NavStatus::NavStatus_NAVSTATUS_GUIDANCE_ACTIVE)// && m_navigationServer->navStatus() != NavStatus::NavStatus_NAVSTATUS_NAVDATA_UPDATING)
        return;

    qDebug() << "onManeuverSymbol" << value.data()->stGuidePoint.nTBTTurnType <<  value.data()->stTBTListPos.nTBTIndex;
    m_navigationServer->setManeuverSymbol(value.data()->stTBTListPos.nTBTIndex, value.data()->stGuidePoint.nTBTTurnType);
}

void PlatformControllerGen3::onTurntoStreet(RGDataPtr value) {
    if(m_navigationServer->navStatus() != NavStatus::NavStatus_NAVSTATUS_GUIDANCE_ACTIVE && m_navigationServer->navStatus() != NavStatus::NavStatus_NAVSTATUS_NAVDATA_UPDATING)
        return;

    //QString streetName = TextDecoder::instance()->fromCP949(value->stGuidePoint.szRoadName);

//    if(value->eRgStatus ==  VSM_RG_STATUS_NONE)
//    {
//       m_navigationServer->setRouteCancel();
////       qDebug() << "onTurntoStreet ::  VSM_RG_STATUS_NONE";
//    }

    if(m_navigationServer != NULL){
        NSTurnToStreet mNSTurnToStreet;
        mNSTurnToStreet.m_turnToStreet = TextDecoder::instance()->fromCP949(value->stGuidePointNext.szRoadName);
        qDebug() << "setTurntoStreet " <<mNSTurnToStreet.m_turnToStreet << mNSTurnToStreet.m_turnToStreet.size();

        if(mNSTurnToStreet.m_turnToStreet.size() == 0)
        {
            mNSTurnToStreet.m_turnToStreet = TextDecoder::instance()->fromCP949(value->stGuidePointNext.szTBTMainText);

            if(mNSTurnToStreet.m_turnToStreet.size() == 0)
            {
                if(value->eGoPosCode != VSM_PosGoal)
                {
                    mNSTurnToStreet.m_turnToStreet = TextDecoder::instance()->fromCP949(value->szGoPosName);
                }else{
                    mNSTurnToStreet.m_turnToStreet = "목적지";
                }
            }
        }

        //qDebug() << "onTurntoStreet #3 : " << mNSTurnToStreet.m_maneuverId << mNSTurnToStreet.m_turnToStreet;
        m_navigationServer->setTurntoStreet(mNSTurnToStreet);
        //m_navigationServer->setCurrentStreet(streetName);
    }
}

void PlatformControllerGen3::onRroundaboutExitNumber(RGDataPtr value) {
    if(m_navigationServer->navStatus() != NavStatus::NavStatus_NAVSTATUS_GUIDANCE_ACTIVE && m_navigationServer->navStatus() != NavStatus::NavStatus_NAVSTATUS_NAVDATA_UPDATING)
        return;

    ushort exitNumber = 0;
    //ushort exitNumber = value->stGuidePoint.nTBTTurnType;
    switch(value->stGuidePoint.nTBTTurnType){
            case IDV_RG_dir_1:                          //	131	1시방향 출구입니다.
                exitNumber = 1;
            break;
            case IDV_RG_RDir_1:                         //	131	1시방향 출구입니다.
                exitNumber = 1;
            break;
            case IDV_RG_dir_2:                          //	132	2시방향 출구입니다.
                exitNumber = 2;
            break;
            case IDV_RG_RDir_2:                         //	132	2시방향 출구입니다.
                exitNumber = 2;
            break;
            case IDV_RG_dir_3:                          //	133	3시방향 출구입니다.
                exitNumber = 3;
            break;
            case IDV_RG_RDir_3:                         //	133	3시방향 출구입니다.
                exitNumber = 3;
            break;
            case IDV_RG_dir_4:                          //	134	4시방향 출구입니다.
                exitNumber = 4;
            break;
            case IDV_RG_RDir_4:                         //	134	4시방향 출구입니다.
                exitNumber = 4;
            break;
            case IDV_RG_dir_5:                          //	135	5시방향 출구입니다.
                exitNumber = 5;
            break;
            case IDV_RG_RDir_5:                         //	135	5시방향 출구입니다.
                exitNumber = 5;
            break;
            case IDV_RG_dir_6:                          //	136	6시방향 출구입니다.
                exitNumber = 6;
            break;
            case IDV_RG_RDir_6:                         //	136	6시방향 출구입니다.
                exitNumber = 6;
            break;
            case IDV_RG_dir_7:                          //	137	7시방향 출구입니다.
                exitNumber = 7;
            break;
            case IDV_RG_RDir_7:                         //	137	7시방향 출구입니다.
                exitNumber = 7;
            break;
            case IDV_RG_dir_8:                          //	138	8시방향 출구입니다.
                exitNumber = 8;
            break;
            case IDV_RG_RDir_8:                         //	138	8시방향 출구입니다.
                exitNumber = 8;
            break;
            case IDV_RG_dir_9:                          //	139	9시방향 출구입니다.
                exitNumber = 9;
            break;
            case IDV_RG_RDir_9:                         //	139	9시방향 출구입니다.
                exitNumber = 9;
            break;
            case IDV_RG_dir_10:                         //	140	10시방향 출구입니다.
                exitNumber = 10;
            break;
            case IDV_RG_RDir_10:                        //	140	10시방향 출구입니다.
                exitNumber = 10;
            break;
            case IDV_RG_dir_11:                         //	141	11시방향 출구입니다.
                exitNumber = 11;
            break;
            case IDV_RG_RDir_11:                        //	141	11시방향 출구입니다.
                exitNumber = 11;
            break;
            case IDV_RG_dir_12:                         //	142	12시방향 출구입니다.
                exitNumber = 12;
            break;
            case IDV_RG_RDir_12:                        //	142	12시방향 출구입니다.
                exitNumber = 12;
            break;
            default:
                exitNumber = 0;
            break;
        }

    if(m_navigationServer != NULL && exitNumber != 0){
        m_navigationServer->setRoundaboutExitNumber(exitNumber);
    }
}
//void PlatformControllerGen3::onNextTurntoStreet(RGDataPtr value) {
//    QString nextTurntoStreet = TextDecoder::instance()->fromCP949(value->stGuidePointNext.szRoadName);

//    if(m_navigationServer != NULL){
//        m_navigationServer->setNextTurntoStreet(nextTurntoStreet);
//    }
//}

void PlatformControllerGen3::onNextTurntoStreet(RGDataPtr value) {
    if(m_navigationServer->navStatus() != NavStatus::NavStatus_NAVSTATUS_GUIDANCE_ACTIVE && m_navigationServer->navStatus() != NavStatus::NavStatus_NAVSTATUS_NAVDATA_UPDATING)
        return;

    if(VSMNavi_IsRoute())
    {
    //    qDebug() << "nextTurnStreet :: value->nTBTListCount :: "<< value->nTBTListCount << value->stTBTListPos.nTBTIndex;
        if(value->nTBTListCount > value->stTBTListPos.nTBTIndex+2)
        {
            std::vector<VSM_OsTBTList> tbtList;
            qInfo() << "[TMCRSM-4108][onNextTurntoStreet]tbtIndex: " << value->stTBTListPos.nTBTIndex+2;
            VSMNavi_GetRouteTBTList(VSM_TBTLIST_ALL, value->stTBTListPos.nTBTIndex+2, 1, tbtList);

            if (0 < tbtList.size()) {
                QString nextTurntoStreet = TextDecoder::instance()->fromCP949(tbtList.at(0).szRoadName);

                if(m_navigationServer != NULL){
                    m_navigationServer->setNextTurntoStreet(nextTurntoStreet);
                }
            }
        }
    }
}

void PlatformControllerGen3::onBarGraphInfo(RGDataPtr value) {
    if(m_navigationServer->navStatus() != NavStatus::NavStatus_NAVSTATUS_GUIDANCE_ACTIVE && m_navigationServer->navStatus() != NavStatus::NavStatus_NAVSTATUS_NAVDATA_UPDATING)
        return;

    NSBarGraphInfo mNSBarGraphInfo;

    bool bBarGraphShow = false;

    if(value.data()->stGuidePoint.nTBTTurnType < IDV_RG_under_in)
    {
        if(value.data()->bExtcImage || value.data()->bDirImage)
        {
            int nExtcImageShowDist = 0;

            if(value.data()->bExtcImage)
                nExtcImageShowDist = value.data()->nExtcImageShowDist;

            if(value.data()->bDirImage)
                nExtcImageShowDist = value.data()->nDirImageShowDist;

            if(value->stGuidePoint.nTBTDist == 0)
                return;

            bool isInDisplayableDist = nExtcImageShowDist >= value.data()->stGuidePoint.nTBTDist;

            if(isInDisplayableDist)
            {
                if(m_BarGraphInfoTBTIndex != value->stTBTListPos.nTBTIndex){
                    m_BarGraphInfoTBTMaxScale = value->stGuidePoint.nTBTDist;
                }

                QString sDist;
                if(m_BarGraphInfoTBTMaxScale > 1000)
                {
                    if(m_BarGraphInfoTBTMaxScale < 10000)
                    {
                        double dist_kilo = (double)(m_BarGraphInfoTBTMaxScale/1000.0);
                        sDist = QString("%1%2").arg(QString::number(dist_kilo, 'f', 1)).arg("km");
                    }else{
                        int dist_kilo = m_BarGraphInfoTBTMaxScale/1000;
                        sDist = QString("%1%2").arg(QString::number(dist_kilo)).arg("km");
                    }
                }else{
                    sDist = QString("%1%2").arg(m_BarGraphInfoTBTMaxScale).arg("m");
                }

                mNSBarGraphInfo.m_barGraphMaxScale = sDist;
                m_BarGraphInfoTBTIndex = value->stTBTListPos.nTBTIndex;

                if(m_BarGraphInfoTBTMaxScale != 0){
                    mNSBarGraphInfo.m_barGraphMinScale = "0m";
                    int scale = ((value->stGuidePoint.nTBTDist*100) / m_BarGraphInfoTBTMaxScale);
                    mNSBarGraphInfo.m_scale = scale;


                    mNSBarGraphInfo.m_valid = true;
                    mNSBarGraphInfo.m_value = value->stGuidePoint.nTBTDist;
                    mNSBarGraphInfo.m_distanceUnit = 0; //0:METERS, 1:KILOMETERS, 2: FEET, 3:YARD

                    if(m_navigationServer != NULL){
                        bBarGraphShow = true;
                        m_navigationServer->setBarGraphInfo(mNSBarGraphInfo);
                    }
                }
            }
        }
    }

    if(!bBarGraphShow)
    {
        if(m_navigationServer != NULL)
        {
            if(m_BarGraphInfoTBTIndex != -1)
            {
                mNSBarGraphInfo.m_scale = 0;
                mNSBarGraphInfo.m_barGraphMaxScale = "";
                mNSBarGraphInfo.m_barGraphMinScale = "";
                mNSBarGraphInfo.m_valid = false;
                mNSBarGraphInfo.m_value = 0;

                m_navigationServer->setBarGraphInfo(mNSBarGraphInfo);
            }
        }
    }
}


void PlatformControllerGen3::onSpeedLimit(RGDataPtr value) {

    bool isSpeedSDI = false;
    if(value->stSDI[0].nSdiType == 0 ||  //SPEED_ACCIDENT_POS = 0,    //3. 신호과속  [ 타입 : 카메라 ]
        value->stSDI[0].nSdiType == 1 || //SPEED_LIMIT_POS = 1,    // 1. 과속 (고정식)  [ 타입 : 카메라 ]
        value->stSDI[0].nSdiType == 2 || //SPEED_BLOCK_START_POS = 2,   // 구간단속 시작  [ 타입 : 카메라 ]
        value->stSDI[0].nSdiType == 3 || //SPEED_BLOCK_END_POS = 3,   // 구간단속 끝      [ 타입 : 카메라 ]
        value->stSDI[0].nSdiType == 4 || //SPEED_BLOCK_MID_POS = 4,   // 구간단속 중      [ 타입 : 카메라 ]
        value->stSDI[0].nSdiType == 6 || //SIGNAL_ACCIDENT_POS = 6,   // 4. 신호 단속      [ 타입 : 카메라
        value->stSDI[0].nSdiType == 7 || //SPEED_LIMIT_DANGEROUS_AREA = 7,  // 8. 과속 (이동식)     [ 타입 : 카메라 ]
        value->stSDI[0].nSdiType == 8){ // 181. 박스형이동식(신규)   [ 타입 : 카메라 ]

        isSpeedSDI = true;
    }

    if(isSpeedSDI)
    {
        NSSpeedLimit mNSSpeedLimit;
    //    0 = speed limit not present,
    //    1 = speed limit present
        mNSSpeedLimit.m_countryCode = "KOR";
        mNSSpeedLimit.m_present = value->bSDI;
        mNSSpeedLimit.m_speedUnit = 0; //0: KM_REP_HOUR, 1:MI_PER_HOUR
        mNSSpeedLimit.m_value = value->stSDI[0].nSdiSpeedLimit;
    //    qDebug() << "onSpeedLimit :: " << mNSSpeedLimit.m_value << mNSSpeedLimit.m_present;
        if(!value->bSDIPlus){
            if(m_navigationServer != NULL){
                m_navigationServer->setSpeedLimit(mNSSpeedLimit);
            }
        }
    }
}

void PlatformControllerGen3::onPositionInformation(RGDataPtr value) {
    NSPositionInformation mNSPositionInformation;
    mNSPositionInformation.m_altitude = value->stCurrent.altitude;
    mNSPositionInformation.m_cityDistrict = "";
    mNSPositionInformation.m_country = "대한민국";
    mNSPositionInformation.m_countryCode = "KOR";
    NSPositionWGS84 mNSPositionWGS84;
    mNSPositionWGS84.m_latitude = value->stCurrent.latitude;
    mNSPositionWGS84.m_longitude = value->stCurrent.longitude;
    mNSPositionInformation.m_positionWGS84 = mNSPositionWGS84;
    mNSPositionInformation.m_mapMatchingPositionAccuracy = value->stCurrent.mmAccuracy;

    QString add = TextDecoder::instance()->fromCP949(value->stCurrent.szPosAddrName);
    QStringList AddList= add.split(" ");
    int listCount = add.split(" ").length();

    if(listCount > 2){
        mNSPositionInformation.m_province       = AddList[0];
        mNSPositionInformation.m_city           = AddList[1];
        mNSPositionInformation.m_cityDistrict   = AddList[2];
    }
    mNSPositionInformation.m_stateCode = QString("KR-%1").arg(QString::number(value->stCurrent.usStateCode));

    mNSPositionInformation.m_street = TextDecoder::instance()->fromCP949(value->stCurrent.szPosRoadName);
    mNSPositionInformation.m_streetNumber= "";
    mNSPositionInformation.m_vehicleHeading= static_cast<double>(value->stCurrent.nPosAngle);
    mNSPositionInformation.m_vehicleSpeed = static_cast<double>(value->stCurrent.nPosSpeed);


    switch(value->stCurrent.roadcate)
    {
    case 0: //고속국도
        mNSPositionInformation.m_roadClass = 1; //ROAD_CLASS_MOTORWAY
        break;
    case 1: //도시 고속화 도로
        mNSPositionInformation.m_roadClass = 2; //ROAD_CLASS_FREEWAY
        break;
    case 2: //국도
    case 3: //국가지원지방도
        mNSPositionInformation.m_roadClass = 3; //ROAD_CLASS_HIGHWAY
        break;
    case 4: //c
        mNSPositionInformation.m_roadClass = 4; //ROAD_CLASS_ARTERIAL
        break;
    case 5: //주요도로1
    case 6: //주요도로2
    case 7: //주요도로3
        mNSPositionInformation.m_roadClass = 5; //ROAD_CLASS_COLLECTOR
        break;
    case 8:
    case 9:
    case 10:
    case 12:
        mNSPositionInformation.m_roadClass = 6; //ROAD_CLASS_LOCAL
        break;
    default:
        mNSPositionInformation.m_roadClass = 0; //ROAD_CLASS_NONE
        break;
    }
/*
    ROAD_CLASS_NONE         //0
    ROAD_CLASS_MOTORWAY     //1
    ROAD_CLASS_FREEWAY      //2
    ROAD_CLASS_HIGHWAY      //3
    ROAD_CLASS_ARTERIAL     //4
    ROAD_CLASS_COLLECTOR    //5
    ROAD_CLASS_LOCAL        //6
    ROAD_CLASS_ADDRESSABLESTREET //7
    ROAD_CLASS_TRAIL        // 8
*/

//    qDebug() << "positionInformation " << mNSPositionInformation.m_street;
    if(m_navigationServer != NULL){
        m_navigationServer->setPositionInformation(mNSPositionInformation);
    }
}

#define ADD_VALUE    0 //0~9는 10미터로 안내하기 위해
#define ADD_VOICE_DIST    10 //잠시후 안내를 미리 하기 위해

#define VOICE_DISTANCE_FOR_HIGHWAY 200
#define VOICE_DISTANCE_FOR_COMMON 120
#define VOICE_DISTANCE_FOR_IN_COMPLEX 80

//거리 안내 상수
typedef enum
{
    RG_EXPRESS_FRONT = 0,
    RG_EXPRESS_300_M,
    RG_EXPRESS_500_M,
    RG_EXPRESS_600_M,
    RG_EXPRESS_1000_M,
    RG_EXPRESS_2000_M,
    RG_NORMAL_FRONT,
    RG_NORMAL_300_M,
    RG_NORMAL_500_M,
    RG_NORMAL_600_M,
    RG_NORMAL_1000_M,
    RG_NORMAL_2000_M,
    Num_Of_DistService
} e_DistService;

bool isDynamicService(RGDataPtr value)
{
    int nDist;

    switch (value.data()->stVoicePlayInfo.curlink.nRoadCate)
    {
        case 0: //고속국도
        case 1: //도시 고속화 도로
            nDist = VOICE_DISTANCE_FOR_HIGHWAY + ADD_VOICE_DIST + 10;
            break;
        case 2: //국도
        case 3: //국가지원지방도
        case 4: //지방도
        case 5: //주요도로1
        case 6: //주요도로2
        case 7: //주요도로3
        case 8: //기타도로1
        case 9: //이면도로
        default:
            nDist = VOICE_DISTANCE_FOR_COMMON + ADD_VOICE_DIST;
            break;
        case 11: //단지내도로
            nDist = VOICE_DISTANCE_FOR_IN_COMPLEX + ADD_VOICE_DIST;
            break;
    }

    if(value.data()->stVoicePlayInfo.currentGP.nRemainderDist <= nDist)
    {
        return true;
    }
    return false;
}

void PlatformControllerGen3::onNextManeuverDetails(RGDataPtr value) {
    if(m_navigationServer->navStatus() != NavStatus::NavStatus_NAVSTATUS_GUIDANCE_ACTIVE && m_navigationServer->navStatus() != NavStatus::NavStatus_NAVSTATUS_NAVDATA_UPDATING)
        return;

    NSNextManeuverDetails mNSNextManeuverDetails;

    QString dist;
    int distUnit;
    if(value->stGuidePoint.nTBTDist > 1000)
    {
        if(value->stGuidePoint.nTBTDist < 10000)
        {
//            double dist_kilo = (double)(value->stGuidePoint.nTBTDist/1000.0);
//            dist = QString("%1").arg(QString::number(dist_kilo, 'f', 1));

            int dist_kilo = value->stGuidePoint.nTBTDist/1000;
            int dist_meter = value->stGuidePoint.nTBTDist/100 - dist_kilo*10;
            dist = QString("%1%2%3").arg(QString::number(dist_kilo)).arg(".").arg(QString::number(dist_meter));
            qDebug() << "value->stGuidePoint.nTBTDist ::::::::::::::::::::::: " << value->stGuidePoint.nTBTDist << dist_kilo;
        }else{
            int dist_kilo = value->stGuidePoint.nTBTDist/1000;
            qDebug() << "value->stGuidePoint.nTBTDist ELSE ::::::::::::::::::::::: " << value->stGuidePoint.nTBTDist <<dist_kilo;
            dist = QString::number(dist_kilo);
        }

        distUnit = 1;
    }else{
        dist = QString::number(value->stGuidePoint.nTBTDist);
        distUnit = 0;
    }

//    mNSNextManeuverDetails.m_distanceToManeuver = QString::number(value->stGuidePoint.nTBTDist);
//    mNSNextManeuverDetails.m_distanceToManeuverUnit = 0; // 0:METERS / 1:KILOMETERS / 2:FEET / 3:YARD

    mNSNextManeuverDetails.m_distanceToManeuver = dist;
    mNSNextManeuverDetails.m_distanceToManeuverUnit = distUnit; // 0:METERS / 1:KILOMETERS / 2:FEET / 3:YARD

    qDebug() << "onNextManeuverDetails " << value->stTBTListPos.nTBTIndex <<nNextManeuverIndex;

    if(value->stTBTListPos.nTBTIndex != nNextManeuverIndex){
        mNSNextManeuverDetails.m_isNewManeuver = true;
        m_ServiceIndex = -1;
        m_RecommandType = 0;
    }else{
        mNSNextManeuverDetails.m_isNewManeuver = false;
    }

//    qDebug() << "onNextManeuverDetailsm_isNewManeuver : " << mNSNextManeuverDetails.m_isNewManeuver  << nNextManeuverIndex << value->stTBTListPos.nTBTIndex;

    if(value->stTBTListPos.nTBTIndex != -1)
        nNextManeuverIndex = value->stTBTListPos.nTBTIndex;

    mNSNextManeuverDetails.m_isReRouted = false;
    int nServiceIndex = -1;

//    if ((value.data()->stVoicePlayInfo.currentGP.nTurnType != IDV_RG_start) &&
//        (value.data()->stVoicePlayInfo.currentGP.nTurnType != IDV_RG_notice_pol) &&
//        (value.data()->stVoicePlayInfo.currentGP.nTurnType != IDV_RG_notice_pol2)){
//        CSglSoundPlayer &SoundPlayer = CSglSoundPlayer::GetInstance();

//        if (SoundPlayer.IsPlaying(DEFAULT_CHANNEL) || SoundPlayer.IsPlaying(SDI_CHANNEL))
//        {
//            return;
//        }

        // 새로운 턴에 진입 후 최소 100미터는 지나야 안내 - 도로명안내 관련 조건 통일
//        if ((value.data()->stVoicePlayInfo.currentGP.nTotalDist - value.data()->stVoicePlayInfo.currentGP.nRemainderDist) < 100)
//        {
//            return;
//        }
//    }

    if(isDynamicService(value))
    {
        m_RecommandType = 3; //0:PREVIEW / 1:PRELIMINARY /2:REGULAR /3:REPEATED
    }else{

        // nRoadCate : 도로종별 코드  : (0:고속국도, 1:도시고속화도로, 2:국도, 3;국가지원지방도, 4:지방도, 5:주요도로1, 6:주요도로2, 7:주요도로3, 8:기타도로1, 9:이면도로, 10:페리항로, 11:reserved)
        if(value.data()->stVoicePlayInfo.curlink.nRoadCate < 2)//고속도로
        {
            if(value->stGuidePoint.nTBTDist > 0 && value->stGuidePoint.nTBTDist < 250){
                nServiceIndex = RG_EXPRESS_FRONT;
            }else if(value->stGuidePoint.nTBTDist >= 250 && value->stGuidePoint.nTBTDist < 350){
                 nServiceIndex = RG_EXPRESS_300_M;
            }else if(value->stGuidePoint.nTBTDist >= 450 && value->stGuidePoint.nTBTDist < 550){
                nServiceIndex = RG_EXPRESS_500_M;
            }else if(value->stGuidePoint.nTBTDist >= 950 && value->stGuidePoint.nTBTDist < 1050){
                nServiceIndex = RG_EXPRESS_1000_M;
            }else if(value->stGuidePoint.nTBTDist >= 1950 && value->stGuidePoint.nTBTDist < 2050){
                nServiceIndex = RG_EXPRESS_2000_M;
            }else if(value->stGuidePoint.nTBTDist >= 2500 && value->stGuidePoint.nTBTDist < 999999){
                nServiceIndex = Num_Of_DistService;
            }
        }else{//일반도로
            if(value->stGuidePoint.nTBTDist > 0 && value->stGuidePoint.nTBTDist < 250){
                nServiceIndex = RG_NORMAL_FRONT;
            }else if(value->stGuidePoint.nTBTDist > 250 && value->stGuidePoint.nTBTDist < 399){
                nServiceIndex = RG_NORMAL_300_M;
            }else if(value->stGuidePoint.nTBTDist > 550 && value->stGuidePoint.nTBTDist < 699){
                nServiceIndex = RG_NORMAL_600_M;
            }else if(value->stGuidePoint.nTBTDist > 950 && value->stGuidePoint.nTBTDist < 1050){
                nServiceIndex = RG_NORMAL_1000_M;
            }else if(value->stGuidePoint.nTBTDist > 1950 && value->stGuidePoint.nTBTDist < 2050){
                nServiceIndex = RG_NORMAL_2000_M;
            }else if(value->stGuidePoint.nTBTDist > 2500 && value->stGuidePoint.nTBTDist < 999999){
                nServiceIndex = Num_Of_DistService;
            }
        }

//        if(nServiceIndex != -1)
        {
            if((m_ServiceIndex == -1 || (m_ServiceIndex == nServiceIndex && nServiceIndex != -1)))
            {
                m_RecommandType = 1; //0:PREVIEW / 1:PRELIMINARY /2:REGULAR /3:REPEATED
                m_ServiceIndex = nServiceIndex;
            }else{
                m_RecommandType = 2; //0:PREVIEW / 1:PRELIMINARY /2:REGULAR /3:REPEATED
            }
        }
    }

    mNSNextManeuverDetails.m_recommendationType = m_RecommandType;
//    qDebug() << "onNextManeuverDetails : " << value.data()->stVoicePlayInfo.curlink.nRoadCate << "(" << value->stGuidePoint.nTBTDist << ")  m_ServiceIndex : " << m_ServiceIndex << "/"<< nServiceIndex <<"/ m_recommendationType : " << mNSNextManeuverDetails.m_recommendationType;

//    if(mNSNextManeuverDetails.m_recommendationType != 0)
    {
        if(m_navigationServer != NULL){
            m_navigationServer->setNextManeuverDetails(mNSNextManeuverDetails);
        }
    }
}

void PlatformControllerGen3::onRGDataInfoChanged(RGDataPtr value) {
    mLastRGDataPtr = value;
}

void PlatformControllerGen3::onRouteRecalculationReasonChanged(int value) {
    if (m_navigationServer != NULL) {
        QString reason;
        switch (value) {
        case UNKNOWN: reason = "UNKNOWN"; break;
        case NO_ROUTE_AVAILABLE: reason = "NO_ROUTE_AVAILABLE"; break;
        case NO_CALCULATION: reason = "NO_CALCULATION"; break;
        case FIRST_CALCULATION: reason = "FIRST_CALCULATION"; break;
        case OPTIMIZE_ROUTE: reason = "OPTIMIZE_ROUTE"; break;
        case REGARDING_TRAFFIC_MESSAGE: reason = "REGARDING_TRAFFIC_MESSAGE"; break;
        case DISREGARD_ROUTE: reason = "DISREGARD_ROUTE"; break;
        case REGARDING_USER_TRAFFIC_MESSAGE: reason = "REGARDING_USER_TRAFFIC_MESSAGE"; break;
        case ROUTE_OPTION_CHANGED: reason = "ROUTE_OPTION_CHANGED"; break;
        case ROUTE_CRITERION_CHANGED: reason = "ROUTE_CRITERION_CHANGED"; break;
        case ROUTE_OPTION_CRITERION_CHANGED: reason = "ROUTE_OPTION_CRITERION_CHANGED"; break;
        case ENTRY_POINT_EVALUATION: reason = "ENTRY_POINT_EVALUATION"; break;
        }
        qDebug() << "PlatformControllerGen3::onRouteRecalculationReasonChanged(" << reason << ")";
        m_navigationServer->setRouteRecalculationReason((uint)value);
    }
}

void PlatformControllerGen3::onRouteCalculateDurationChanged(int durationMillisec)
{
    qDebug() << "PlatformControllerGen3::onRouteCalculateDurationChanged(" << durationMillisec << ")";
    if (m_navigationServer != NULL) {
        uint durationSec = static_cast<uint>(durationMillisec / 1000.0);
        if (durationMillisec > 0 && durationSec == 0)  // 0초과 1미만인 경우, 강제로 1로 셋팅 (넘길 값이 초 단위라서...) 2019.1.18 by hheo
            durationSec = 1;
        NSRouteCalculationDuration duration;
        duration.m_routeCalDurationToFinalDest = durationSec;
        duration.m_routeCalDurationToNextDest = 0; // NOT support

        m_navigationServer->setRouteCalculationDuration(duration);
    }
}

void PlatformControllerGen3::onRouteOptionChanged(int option)
{
    switch (option) {
        case 1: // 최적길
           m_RouteOption = RouteCriterion_ROUTE_CRITERION_FAST;
           break;
        case 4: // 무료길
           m_RouteOption = RouteCriterion_ROUTE_CRITERION_ECONOMIC;
           break;
        case 8: // 최단거리
           m_RouteOption = RouteCriterion_ROUTE_CRITERION_SHORT;
           break;
        case 32: // 최소시간
           m_RouteOption = RouteCriterion_ROUTE_CRITERION_FAST;
           break;
        case 2: // 고속도로 우선
           m_RouteOption = RouteCriterion_ROUTE_CRITERION_SHORT;
           break;
        case 16: // 초보자경로
           m_RouteOption = RouteCriterion_ROUTE_CRITERION_SHORT;
           break;
        default:
           m_RouteOption = RouteCriterion_ROUTE_CRITERION_FAST;
           break;
    }
}

void PlatformControllerGen3::onWorkLocation(TS_ROUTE_POS value) {
    NSWorkLocation mNSWorkLocation;
    mNSWorkLocation.m_name = TextDecoder::instance()->fromCP949(value.szName);
    VSMPoint2f geo;
    VSMPoint2i world;
    world.x = value.tp.x;
    world.y = value.tp.y;
    bool ok = VSMMap_Util_ConvertWorldTo(world, VSM_COORDINATE_WGS84,geo);

    NSPositionWGS84 mNSPositionWGS84;
    mNSPositionWGS84.m_longitude    = geo.x;
    mNSPositionWGS84.m_latitude     = geo.y;
    mNSWorkLocation.m_positionWGS84 = mNSPositionWGS84;
    mNSWorkLocation.m_valid = ok;

    // this is requested from bosch in case of deletion 2019-3-8
    if (ok == false) {
        mNSWorkLocation.m_positionWGS84.m_longitude = 0;
        mNSWorkLocation.m_positionWGS84.m_latitude  = 0;
    }

   qDebug() << "onWorkLocation " << mNSWorkLocation.m_name;
    if(m_navigationServer != NULL){
        m_navigationServer->setWorkLocation(mNSWorkLocation);
    }
}

void PlatformControllerGen3::onHomeLocation(TS_ROUTE_POS value) {
    NSHomeLocation mNSHomeLocation;
    mNSHomeLocation.m_name = TextDecoder::instance()->fromCP949(value.szName);
    VSMPoint2f geo;
    VSMPoint2i world;
    world.x = value.tp.x;
    world.y = value.tp.y;
    bool ok = VSMMap_Util_ConvertWorldTo(world, VSM_COORDINATE_WGS84,geo);

    NSPositionWGS84 mNSPositionWGS84;
    mNSPositionWGS84.m_longitude    = geo.x;
    mNSPositionWGS84.m_latitude     = geo.y;
    mNSHomeLocation.m_positionWGS84 = mNSPositionWGS84;
    mNSHomeLocation.m_valid         = ok;

    // this is requested from bosch in case of deletion 2019-3-8
    if (ok == false) {
        mNSHomeLocation.m_positionWGS84.m_longitude = 0;
        mNSHomeLocation.m_positionWGS84.m_latitude  = 0;
    }

   qDebug() << "onHomeLocation " << mNSHomeLocation.m_name;
    if(m_navigationServer != NULL){
        m_navigationServer->setHomeLocation(mNSHomeLocation);
    }
}
void PlatformControllerGen3::onMapRepresentation(int value) {
//    VSM_VIEW_MODE_NORTHUP = 1,
//    VSM_VIEW_MODE_HEADUP,
//    VSM_VIEW_MODE_BIRDVIEW

    int mapRepresentation = 0;

    switch (value) {
    case VSM_VIEW_MODE_NORTHUP:
            mapRepresentation = MapRepresentation::MapRepresentation_MAP_REPRESENTATION_2D_NORTH_UP;
        break;
    case VSM_VIEW_MODE_HEADUP:
            mapRepresentation = MapRepresentation::MapRepresentation_MAP_REPRESENTATION_2D_CAR_HEADING;
        break;
    case VSM_VIEW_MODE_BIRDVIEW:
            mapRepresentation = MapRepresentation::MapRepresentation_MAP_REPRESENTATION_3D_CAR_HEADING;
        break;
    default:
        mapRepresentation = MapRepresentation::MapRepresentation_MAP_REPRESENTATION_NOT_SET;
        break;
    }

//    qDebug() << "onMapRepresentation: " << mapRepresentation;
    if(m_navigationServer != NULL){
        m_navigationServer->setMapRepresentation(mapRepresentation);
    }
}

void PlatformControllerGen3::onWaypointList(QVariant v) {
    QRoutePosListPtr value = v.value<QRoutePosListPtr>();
    NSWaypointList mNSWaypointList;

    // 2019-12-24 fixed TMCRSM-4405 number of waypoint can't over 3, start position should be not there.
    for (int i = 1; i < value->size(); i++) {
        int x = value->at(i).tp.x;
        int y = value->at(i).tp.y;
        if (x == 0 || y == 0) // ignore invalid position
            continue;

        VSMPoint2f geo;
        NSWaypointListElement mNSWaypointListElement;
        NSPositionWGS84 mLocation;
        auto ok = VSMMap_Util_ConvertWorldTo(VSMPoint2i{x, y},VSM_COORDINATE_WGS84, geo);
        if (ok) {
            mLocation.m_latitude = geo.y;
            mLocation.m_longitude = geo.x;
        } else {
            mLocation.m_latitude = 0;
            mLocation.m_longitude = 0;
        }
        mNSWaypointListElement.m_location = mLocation;
        mNSWaypointListElement.m_name = TextDecoder::instance()->fromCP949(value->at(i).szName);
        mNSWaypointListElement.m_routeCriterion = m_RouteOption; //VSM_stTmapSummaryBaseInfo 에 경로 옵션이 있음
        mNSWaypointList.append(mNSWaypointListElement);
    }

    for (int i=0;i<value->size();i++) {
        m_Waypoints[i%5] = value->at(i);
    }

    if(m_navigationServer != NULL){
        m_navigationServer->setWaypointList(mNSWaypointList);
    }
}

void PlatformControllerGen3::onActiveDestinationInfoChanged(QVariant activeWaypointList)
{
    auto value = activeWaypointList.value<QRoutePosListPtr>();
    NSDestinationInfoElements activeWaypoints;

    for (int i = 0; i < value->size(); i++) {
        auto x = value->at(i).tp.x;
        auto y = value->at(i).tp.y;
        VSMPoint2f geo;
        VSMPoint2i world;
        world.x = x;
        world.y = y;
        bool ok = VSMMap_Util_ConvertWorldTo(world, VSM_COORDINATE_WGS84, geo);

        NSDestinationInfoElement activeInfo;
        NSPositionWGS84 mLocation;
        mLocation.m_longitude  = geo.x;
        mLocation.m_latitude   =  geo.y;
        activeInfo.m_location  = mLocation;
        activeInfo.m_name      = TextDecoder::instance()->fromCP949(value->at(i).szName);
        activeInfo.m_countryCode    = QStringLiteral("KOR");
        activeInfo.m_subCountryCode = QStringLiteral("KOR");
        activeWaypoints.append(activeInfo);
    }

    if(m_navigationServer != NULL){
        m_navigationServer->setDestinationInformation(activeWaypoints);
    }
}

void PlatformControllerGen3::onIsAddressBookChanged(bool value) {
    if(m_navigationServer != NULL){
        m_navigationServer->setIsAddressBookChanged(value);
    }
}

void PlatformControllerGen3::onIsLastDestinationListChanged(bool value) {
    qDebug() << "pf,navi last dest  changed !";
//    qDebug() << "bIsLastDestinationListChanged " << bIsLastDestinationListChanged;
    if(m_navigationServer != NULL){
        m_navigationServer->setIsLastDestinationListChanged(true);
    }
}

void PlatformControllerGen3::onShowMapScreenWithMapViewMode(uint value) {
//    m_controller->setMapViewMode(value);//[TI190920] use QMetaObject::invokeMethod
}
//uint PlatformControllerGen3::onCheckNavDataCompatibility(const QString &navDataVersion, const QByteArray &compatibilityInfo) {
//    //    To do..

//    return 0;
//}

void PlatformControllerGen3::onApplyWaypointListChange() {
    qDebug() << "[INTERFACE] PlatformControllerGen3::onApplyWaypointListChange()";
    int option = 1;
    bool apply = true;
    auto p = new QList<TS_ROUTE_POS>{};
    *p = m_Waypoints;
    QRoutePosListPtr waypoints(p);

    QMetaObject::invokeMethod(m_controller, "requestRoute", Q_ARG(QVariant, QVariant::fromValue(waypoints)), Q_ARG(int, option), Q_ARG(bool, apply));
}

void PlatformControllerGen3::onCancelRouteGuidance() {
    QMetaObject::invokeMethod(m_controller, "cancelRoute");
}

//void PlatformControllerGen3::onDeleteWaypoints(uint firstIndex, uint lastIndex) {
//    //    To do..

//}

//NSPoiCategoryShortcuts PlatformControllerGen3::onGetPoiCategoryShortcuts() {
//    NSPoiCategoryShortcuts mNSPoiCategoryShortcuts;
//    NSPoiCategoryShortcut mNSPoiCategoryShortcut;
//    QString catName = "test";
//    mNSPoiCategoryShortcut.m_categoryName = catName;
//        NSImage iconImage;
//        QByteArray byteImage;
//        iconImage.m_image = byteImage;
//        iconImage.m_imageSize = 1;
//        iconImage.m_imageType = 1;
//    mNSPoiCategoryShortcut.m_icon = iconImage;
//    mNSPoiCategoryShortcut.m_poiCategoryId = 1;
//    mNSPoiCategoryShortcut.m_shortcutIndex = 1;
//    mNSPoiCategoryShortcuts.append(mNSPoiCategoryShortcut);
//    return mNSPoiCategoryShortcuts;
//}


//NSMapPOIIconCategories PlatformControllerGen3::onGetPoiIconCategoriesInMap() {
//    NSMapPOIIconCategories mNSMapPOIIconCategories;
//    mNSMapPOIIconCategories.append(1);
//    mNSMapPOIIconCategories.append(2);
//    return mNSMapPOIIconCategories;
//}


void PlatformControllerGen3::onInsertWaypoint(uint newIndex) {
#if (0)
    // 경유지 추가시 app에 넘겨주어야 할 정보
    // 1) 좌표
    // 2) POI명칭 또는 주소
    // 3) type(아마도 1? - 경유지1)
    // 19.01.15 ymhong

//    need to tmap method
    // insert new waypoint : [Kannan (pka1cob)] : If there is an active route guidance and upon setting another destination user can add the new address as a waypoint. If the waypoint list is not full then calling this method will insert the new destination to the waypoint list.The new waypoint will always be added as first entry to the list.
    TS_ROUTE_POS nearestWaypoint = m_Waypoints.at(1);
    // m_controller->setWaypoint(m_Waypoints.at(1), newIndex); // [TI190920]
    QMetaObject::invokeMethod(m_controller, "setWaypoint", Q_ARG(TS_ROUTE_POS,nearestWaypoint), Q_ARG(int, newIndex));
    // 1. needs to check the static waypoints variable whether it is thread-safe
    // 2. use QMetaObject::invokeMethod
#else
    qDebug() << "PlatformControllerGen3::onInsertWaypoint(" << newIndex << ")";

    // newIndex는 0,1,2로 전달. UI에서는 slot번호를 받으므로 +1을 해주어야 함. 0은 출발지
    // 19.08.08 ymhong.
    auto location = m_navigationServer->currentSetLocation();
    auto longitude = location.m_longitude;
    auto latitude = location.m_latitude;
    VSMPoint2f geo { longitude, latitude };
    VSMPoint2i world;
    auto ok = VSMMap_Util_ConvertToWorld(geo, VSM_COORDINATE_WGS84, world);
    if (ok) {
        auto slot = newIndex + 1;
        TS_ROUTE_POS pos;
        pos.tp.x = world.x;
        pos.tp.y = world.y;
        QMetaObject::invokeMethod(m_controller, "insertWaypoint", Q_ARG(TS_ROUTE_POS, pos), Q_ARG(uint, slot));
    } else {
        qDebug() << Q_FUNC_INFO << ": failed to convert world coord from " << geo.x << ", " << geo.y;
    }
#endif
}

void PlatformControllerGen3::onInsertWaypointAtDefaultPosition()
{
    qDebug() << "PlatformControllerGen3::onInsertWaypointAtDefaultPosition()";

    const auto defaultSlot = 0;

    onInsertWaypoint(defaultSlot);
}

void PlatformControllerGen3::onDeleteWaypoints(uint firstIndex, uint lastIndex) {
    QMetaObject::invokeMethod(m_controller, "deleteWaypoints", Q_ARG(int,firstIndex), Q_ARG(int, lastIndex));
}

void PlatformControllerGen3::onReplaceWaypoint(uint index)
{
    QMetaObject::invokeMethod(m_controller, "replaceWaypoint", Q_ARG(uint, index));
}

NSDestinationMemoryDetailsList PlatformControllerGen3::onRequestDestinationMemoryEntries(uint categoryType) {
    qDebug() << "[INTERFACE] PlatformControllerGen3::onRequestDestinationMemoryEntries(" << categoryType << ")";

    //    need more information by tp
    QRoutePosListPtr routeList;
    if (categoryType == 0) // favorite
         routeList = m_controller->naviProperty()->getFavoriteList();
    else
         routeList = m_controller->naviProperty()->getRecentDestinationList(); // [TI190920] if it cannot be helped to access naviProperty, keep recentDestinationList in naviProperty thread-safe by using QMutex

    qDebug() << "pf, reqdestmem~:" << routeList->size();
    NSDestinationMemoryDetailsList detailList;
    for (int i = 0; i < routeList->size(); i++) {
        NSDestinationMemoryDetailListElement element;
        element.m_entry.m_entryID = i;
        element.m_entry.m_name = TextDecoder::instance()->fromCP949(routeList->at(i).szName);
        VSMPoint2i pos;
        pos.x = routeList->at(i).tp.x;
        pos.y = routeList->at(i).tp.y;
        VSMPoint2f geo;
        bool ok = VSMMap_Util_ConvertWorldTo(pos,VSM_COORDINATE_WGS84, geo);
        if (!ok) {
            qWarning() << "world to wgs84 failure for [" << routeList->at(i).szName << "] " << pos.x << ", " << pos.y;
        }
        element.m_coordinates.m_longitude = geo.x;
        element.m_coordinates.m_latitude  = geo.y;
        element.m_details.m_countryCode = "KOR";
        element.m_details.m_countryName = "Korea";
        element.m_details.m_poiName     = TextDecoder::instance()->fromCP949(routeList->at(i).szName);
        detailList.push_back(element);
    }
    // 20200324 update TMCRSM-4678
    if (m_navigationServer != nullptr) {
        m_navigationServer->handleRequestDestinationMemoryEntries(QVariant::fromValue(detailList));
        m_navigationServer->setIsLastDestinationListChanged(false);
    }
    return detailList;
}

NSMapVideoStreamResult PlatformControllerGen3::onStartMapVideoStream(NSMapVideoStreamDimensions mapVideoStreamDimensions, const QString &ipAddress, ushort portnumber, ushort videoType) {
    m_isStartStreamSignal = true;

    NSMapVideoStreamResult mNSMapVideoStreamResult;
    //스트리밍 중에 start signal이 들어오면 랜더링이나, 스트리밍 다시 시도하지 않고 바로 AVAILABLE을 리턴한다.
    if(mNaviAppStatue->VideoStream)
    {
        mNSMapVideoStreamResult.m_mapVideoStreamStatus = TmapController::AVAILABLE;
        mNSMapVideoStreamResult.m_mapVideoStreamHandle = m_controller->VIDEO_STREAM_HANDLE;

        onMapVideoStatus();

        return mNSMapVideoStreamResult;
    }

    mVideoStreamInfo->VideoStreamIP = ipAddress;
    mVideoStreamInfo->VideoStreamPort = portnumber;
//  클러스터 지도 사이즈 1개로 고정
//    mVideoStreamInfo->VideoStreamDemenstions.m_width    = mapVideoStreamDimensions.m_width;
//    mVideoStreamInfo->VideoStreamDemenstions.m_height   = mapVideoStreamDimensions.m_height;
    mVideoStreamInfo->VideoStreamDemenstions.m_width    = MapInClusterLayout::LayoutSize::LargeLayoutWidth;
    mVideoStreamInfo->VideoStreamDemenstions.m_height   = MapInClusterLayout::LayoutSize::LargeLayoutHeight;

    if(mNaviAppStatue->AndroidAutoStatus)
    {
        mNSMapVideoStreamResult.m_mapVideoStreamStatus = TmapController::SPI_NAVI_ACTIVE;
        mNSMapVideoStreamResult.m_mapVideoStreamHandle = m_controller->VIDEO_STREAM_HANDLE;

        mNaviAppStatue->VideoStream = true;
        onMapVideoStatus();

        return mNSMapVideoStreamResult;
    }else{
        if(m_ClusterHandleId != 0)
        {
            m_controller->resumeMapInCluster(); //[TI190920] for consistency with others, make a wrapper function that emits startMapInCluster and call it by using QMetaObject::invokeMethod

            mNSMapVideoStreamResult.m_mapVideoStreamStatus = TmapController::AVAILABLE;
            mNSMapVideoStreamResult.m_mapVideoStreamHandle = m_controller->VIDEO_STREAM_HANDLE;

            // process other events while wait for ready to map video stream. 2019.7.23 by hheo
            QEventLoop loop;
            connect(m_controller, SIGNAL(doneStartMapVideoStream()), &loop, SLOT(quit()));
            QTimer::singleShot(5000, &loop,&QEventLoop::quit);
            loop.exec();

            mNaviAppStatue->VideoStream = true;

            //스트리밍을 성공하여도 스트리밍 준비중에 deleteMapVideoStream이 들어왔으면 UNAVAILABLE로 리턴한다.
            if(!m_isStartStreamSignal)
            {
                mNaviAppStatue->VideoStream = false;
                mNSMapVideoStreamResult.m_mapVideoStreamStatus = TmapController::UNAVAILABLE;
            }
            qDebug() << "startMapVideoStream return 1:" << mapVideoStreamDimensions.m_width << mapVideoStreamDimensions.m_height << videoType;
            onMapVideoStatus();

            return mNSMapVideoStreamResult;
        }

        if(!m_controller->naviProperty()->getTmapLocalMapVersion().isNull())
        {
            int layout = MapInClusterLayout::CenteralMap;
// 클러스터 지도 사이즈 1개로 고정
//            if(mapVideoStreamDimensions.m_width == MapInClusterLayout::SmallLayoutWidth && mapVideoStreamDimensions.m_height == MapInClusterLayout::SmallLayoutHeight)
//            {
//                layout = MapInClusterLayout::SideMap;
//            }
//            m_controller->setMapInClusterIpPort(ipAddress, portnumber, videoType, mapVideoStreamDimensions.m_width, mapVideoStreamDimensions.m_height); //[TI190920] make a wrapper function to emit stopMapIncluster and call it by using QMetaObject::invokeMethod
            m_controller->setMapInClusterIpPort(ipAddress, portnumber, videoType, MapInClusterLayout::LayoutSize::LargeLayoutWidth, MapInClusterLayout::LayoutSize::LargeLayoutHeight); //[TI190920] make a wrapper function to emit stopMapIncluster and call it by using QMetaObject::invokeMethod
            m_controller->startMapInCluster(layout); //[TI190920] for consistency with others, make a wrapper function that emits startMapInCluster and call it by using QMetaObject::invokeMethod

            mNSMapVideoStreamResult.m_mapVideoStreamStatus = TmapController::AVAILABLE;
        }else{
            mNSMapVideoStreamResult.m_mapVideoStreamStatus = TmapController::UNAVAILABLE;
        }

        mNSMapVideoStreamResult.m_mapVideoStreamHandle = m_controller->VIDEO_STREAM_HANDLE;// [TI190920] keep VIDEO_STREAM_HANDLE thread-safe by using QMutex

        m_ClusterHandleId = mNSMapVideoStreamResult.m_mapVideoStreamHandle;

        // process other events while wait for ready to map video stream. 2019.7.23 by hheo
        QEventLoop loop;
        connect(m_controller, SIGNAL(doneStartMapVideoStream()), &loop, SLOT(quit()));
        QTimer::singleShot(10000, &loop,&QEventLoop::quit);
        loop.exec();

        mNaviAppStatue->VideoStream = true;

        if(!m_isStartStreamSignal)
        {
            mNaviAppStatue->VideoStream = false;
            mNSMapVideoStreamResult.m_mapVideoStreamStatus = TmapController::UNAVAILABLE;
        }
        onMapVideoStatus();

        return mNSMapVideoStreamResult;
    }
}


NSMapVideoStreamResult PlatformControllerGen3::onModifyMapVideoStream(uint mapVideoStreamHandle, NSMapVideoStreamDimensions mapVideoStreamDimensions, ushort videoType) {
     qDebug() << ":::::::::tmcontroller,,onModifyMapVideoStream : " << mapVideoStreamHandle << "type : " << videoType << mapVideoStreamDimensions.m_width << mapVideoStreamDimensions.m_height;
    NSMapVideoStreamResult mNSMapVideoStreamResult;
//20191106 MODIFY TEST
    if(mNaviAppStatue->VideoStream){
        mNSMapVideoStreamResult.m_mapVideoStreamStatus = TmapController::AVAILABLE;
    }else{
        mNSMapVideoStreamResult.m_mapVideoStreamStatus = TmapController::UNAVAILABLE;
    }

    mNSMapVideoStreamResult.m_mapVideoStreamHandle = m_controller->VIDEO_STREAM_HANDLE;

    return mNSMapVideoStreamResult;

// 클러스터 지도 사이즈 1개로 고정
//    mVideoStreamInfo->VideoStreamDemenstions.m_width    = mapVideoStreamDimensions.m_width;
//    mVideoStreamInfo->VideoStreamDemenstions.m_height   = mapVideoStreamDimensions.m_height;

//    if(mNaviAppStatue->AndroidAutoStatus)
//    {
//        mNSMapVideoStreamResult.m_mapVideoStreamStatus = TmapController::SPI_NAVI_ACTIVE;
//        mNSMapVideoStreamResult.m_mapVideoStreamHandle = m_controller->VIDEO_STREAM_HANDLE;

//        return mNSMapVideoStreamResult;
//    }

//    mNaviAppStatue->VideoStream = false;

//    m_controller->modifyMapInCluster(mapVideoStreamDimensions.m_width, mapVideoStreamDimensions.m_height);
//    if(m_ClusterHandleId != 0)
//     {
//        int layout = MapInClusterLayout::CenteralMap;
//        if(mapVideoStreamDimensions.m_width == MapInClusterLayout::SmallLayoutWidth && mapVideoStreamDimensions.m_height == MapInClusterLayout::SmallLayoutHeight)
//        {
//            layout = MapInClusterLayout::SideMap;
//        }

//        m_controller->changeMapInClusterLayout(layout);//[TI190920] for consistency with others, make a wrapper function that emits changeMapInClusterLayout and call it by using QMetaObject::invokeMethod
//        mNSMapVideoStreamResult.m_mapVideoStreamStatus = TmapController::AVAILABLE;
//        mNSMapVideoStreamResult.m_mapVideoStreamHandle = m_controller->VIDEO_STREAM_HANDLE ;

//        // process other events while wait for ready to map video stream. 2019.7.23 by hheo
//        m_bMapModifyReturn = false;
//        QEventLoop loop;
//        connect(m_controller, SIGNAL(doneStartMapVideoStream()), &loop, SLOT(quit()));
//        QTimer::singleShot(5000, &loop,&QEventLoop::quit);
//        loop.exec();

//        mNaviAppStatue->VideoStream = true;

//        qDebug() << "modifyMapVideoStream return:" << mapVideoStreamHandle << mapVideoStreamDimensions.m_width << mapVideoStreamDimensions.m_height << videoType;
////        m_bMapModifyReturn = true;
//        return mNSMapVideoStreamResult;
//    }else{
//        mNSMapVideoStreamResult.m_mapVideoStreamStatus = TmapController::UNAVAILABLE;

//        return mNSMapVideoStreamResult;
//     }
 }

NSMapVideoStreamResult PlatformControllerGen3::onDeleteMapVideoStream(uint mapVideoStreamHandle) {
    m_isStartStreamSignal = false;

    NSMapVideoStreamResult mNSMapVideoStreamResult;
    if(!mNaviAppStatue->VideoStream)
    {
        mNSMapVideoStreamResult.m_mapVideoStreamHandle = m_controller->VIDEO_STREAM_HANDLE;
        mNSMapVideoStreamResult.m_mapVideoStreamStatus = TmapController::UNAVAILABLE;
        onMapVideoStatus();

        return mNSMapVideoStreamResult;
    }

    if(mNaviAppStatue->VideoStream)
    {
        if(m_ClusterHandleId != 0)
        {
            if(m_controller)
            {
                m_controller->pauseMapInCluster();
            }
        }

        mNaviAppStatue->VideoStream = false;
    }

    mNSMapVideoStreamResult.m_mapVideoStreamHandle = m_controller->VIDEO_STREAM_HANDLE;
    mNSMapVideoStreamResult.m_mapVideoStreamStatus = TmapController::UNAVAILABLE;
    onMapVideoStatus();

    return mNSMapVideoStreamResult;
}


void PlatformControllerGen3::onRequestDetailsForSelectedLocation(const NSPOILocation& location) {
    if (location.m_location.m_positionWGS84.m_latitude == 0 &&
            location.m_location.m_positionWGS84.m_longitude == 0) {
        qWarning() << "pf, invalid location to request detail !";
        if (m_navigationServer) {
            m_navigationServer->handleLocationDetail(QVariant());
        }
        return;
    }

    VSMPoint2f geo;
    geo.x = location.m_location.m_positionWGS84.m_longitude;
    geo.y = location.m_location.m_positionWGS84.m_latitude;
    VSMPoint2i world;
    bool ok = VSMMap_Util_ConvertToWorld(geo, VSM_COORDINATE_WGS84,world);
    if (!ok) {
        qDebug() << Q_FUNC_INFO << ": failed to convert world coord from " << geo.x << ", " << geo.y;
    }

    TS_ROUTE_POS pos;
    memset(&pos, 0, sizeof(TS_ROUTE_POS));
    pos.tp.x = world.x;
    pos.tp.y = world.y;

    qDebug() << "pf, req location detail:" << pos.szName << ", " << location.m_location.m_name << ", " << location.m_address;

    qDebug() << "[INTERFACE] PlatformControllerGen3::onRequestDetailsForSelectedLocation(" << geo.y << ", " << geo.x << ")";

    // need poi id
    // pos.nPoiID = 000000;
    QMetaObject::invokeMethod(m_controller, "requestLocationDetail", Q_ARG(const TS_ROUTE_POS&, pos));
}


void PlatformControllerGen3::onRequestFreeTextSearchResults(
    const QString &searchString,
    uint searchScope,
    uint searchType,
    uint category,
    uint destinationIndex,
    uint startIndex,
    uint numElements) {

    qDebug() << "FTS #1 " << QTime::currentTime().toString("mm:ss:zzz");
    QMetaObject::invokeMethod(m_controller, "requestFreeTextSearchResult",
    Q_ARG(const QString&, searchString)
    , Q_ARG(int, searchScope)
    , Q_ARG(int, searchType)
    , Q_ARG(int, category)
    , Q_ARG(int, destinationIndex)
    , Q_ARG(int, startIndex)
    , Q_ARG(int, numElements)
    );

    qDebug() << "FTS #2 " << QTime::currentTime().toString("mm:ss:zzz");
}

void PlatformControllerGen3::onShowDestinationFreeTextSearchInput(const QString& searchString)
{
    QMetaObject::invokeMethod(m_controller, "showDestinationFreeTextSearchInputOccur",
    Q_ARG(const QString&,searchString));
}

//NSLocationDetails PlatformControllerGen3::onRequestLocationDetailsForCoordinates(NSPositionWGS84 cityCoordinate) {
//    NSLocationDetails mNSLocationDetails;
//    return mNSLocationDetails;
//}


void PlatformControllerGen3::onRequestWaypointList() {
    QRoutePosListPtr routeList = m_controller->naviProperty()->getTmapWaypointList(); // [TI190920] keep waypointList in naviProperty thread-safe by using QMutex
    // TODO: it should call the controller  by async
    if (routeList.data() != nullptr) {
        onWaypointList(QVariant::fromValue(routeList));
    }
}


void PlatformControllerGen3::onRetriggerAcousticOutput() {
    // m_controller->rePlayVoiceGuidance();//[TI190920] use QMetaObject::invokeMethod
    QMetaObject::invokeMethod(m_controller,"rePlayVoiceGuidance");
}

//void PlatformControllerGen3::onSdsCheckAddress(NSSDSAddress requestedAddress) {

//}


QString PlatformControllerGen3::onSdsGetHouseNumberPattern(QString &maxHouseNumber, QStringList &patterns) {
    maxHouseNumber = "9999-99";
    // patterns.clear();
    return QStringLiteral("1-0"); // return is minHouseNumber
}



//void PlatformControllerGen3::onSsdsUpdateVoiceTag(NSSDSVoiceTagId voiceTagID) {

//}


//void PlatformControllerGen3::onSelectPoiCategoryShortcut(uchar shortcutIndex) {

//}



void PlatformControllerGen3::onSetLocationWithCoordinates(NSPositionWGS84 location) {
//    need more information by NSPositionWGS84(tp)
    VSMPoint2f geo;
    geo.x = location.m_longitude;
    geo.y = location.m_latitude;
    VSMPoint2i world;
    bool ok = VSMMap_Util_ConvertToWorld(geo, VSM_COORDINATE_WGS84,world);
    if (!ok) {
        qDebug() << Q_FUNC_INFO << ": failed to convert world coord from " << geo.x << ", " << geo.y;
    }

    TS_ROUTE_POS pos;
    memset(&pos, 0, sizeof(TS_ROUTE_POS));
    pos.tp.x = world.x;
    pos.tp.y = world.y;
    m_Waypoints[4] = pos;
    // 1. needs to check the static waypoints variable whether it is thread-safe -> fixed
    // 2. use QMetaObject::invokeMethod -> fixed
    QMetaObject::invokeMethod(m_controller, "setWaypoint", Q_ARG(TS_ROUTE_POS, pos), Q_ARG(int, 4));
}


 void PlatformControllerGen3::onSetLocationWithDetails(const NSPOILocation& location) {
     onSetLocationWithCoordinates(location.m_location.m_positionWGS84);
 }


void PlatformControllerGen3::onShowAndSetLocationWithDetails(const NSPOILocation& location) {
    // onSetLocationWithCoordinates(location.m_location.m_positionWGS84); // UI Proxy에서 처리

    VSMPoint2f geo;
    geo.x = location.m_location.m_positionWGS84.m_longitude;
    geo.y = location.m_location.m_positionWGS84.m_latitude;
    VSMPoint2i world;
    bool ok = VSMMap_Util_ConvertToWorld(geo, VSM_COORDINATE_WGS84,world);
    if (!ok) {
        qDebug() << Q_FUNC_INFO << ": failed to convert world coord from " << geo.x << ", " << geo.y;
    }

    qDebug() << "[INTERFACE] PlatformControllerGen3::onShowAndSetLocationWithDetails(" << geo.y << ", " << geo.x << ")";

    TS_ROUTE_POS pos;
    memset(&pos, 0, sizeof(TS_ROUTE_POS));
    pos.tp.x = world.x;
    pos.tp.y = world.y;
    m_Waypoints[4] = pos;

    QVariantMap detail;
    detail[QStringLiteral("x")] = world.x;
    detail[QStringLiteral("y")] = world.y;
    detail[QStringLiteral("name")] = location.m_location.m_name;
    detail[QStringLiteral("address")] = location.m_address;
    detail[QStringLiteral("phone")] = location.m_phoneNumber;

    QMetaObject::invokeMethod(m_controller, "goPoiDetailScreen", Q_ARG(const QVariant&, QVariant::fromValue(detail)));
}

/**
 * Deprecated since 2019-03-12
 */
void PlatformControllerGen3::onSetLocationWithDestinationMemoryEntry(qulonglong id) {
    // need to tmap method
        // 1. needs to check the static waypoints variable whether it is thread-safe -> fixed
        // 2. use QMetaObject::invokeMethod} -> fixed
    if (m_controller->naviProperty()->getRecentDestinationList()->size() > id) {
        TS_ROUTE_POS pos = m_controller->naviProperty()->getRecentDestinationList()->at(id);
        m_Waypoints[4] = pos;

        QMetaObject::invokeMethod(m_controller, "setWaypoint", Q_ARG(TS_ROUTE_POS, pos), Q_ARG(int, 4));
    }
}

/**
 * 2019-03-12  Don't ui flow, just keep it in cache for all setLocationWith*
 * use this method instead of onSetLocationWithDestinationMemoryEntry
 */
NSPOILocation PlatformControllerGen3::onToLocationWithDestinationMemoryEntry(uint category, qulonglong id)
{
   NSPOILocation location;

   // fix TMCRSM-3958, TMCRSM-3960 2019.10.10 lck
   QRoutePosListPtr routeList;
   if (category == 0) // favorite
        routeList = m_controller->naviProperty()->getFavoriteList();
   else
        routeList = m_controller->naviProperty()->getRecentDestinationList(); // [TI190920] if it cannot be helped to access naviProperty, keep recentDestinationList in naviProperty thread-safe by using QMutex

   if (routeList->size() > id) {
           TS_ROUTE_POS pos = routeList->at(id);
           VSMPoint2f geo;
           VSMPoint2i world;
           world.x = pos.tp.x;
           world.y = pos.tp.y;
           bool ok = VSMMap_Util_ConvertWorldTo(world, VSM_COORDINATE_WGS84, geo);
           if (!ok) {
               qWarning() << "failed to convert world to at: " << __LINE__;
               return location;
           }
           location.m_location.m_positionWGS84.m_longitude = geo.x;
           location.m_location.m_positionWGS84.m_latitude  = geo.y;
           QByteArray copied(pos.szName);
           location.m_location.m_name = TextDecoder::instance()->fromCP949(copied);

           // VR에서 경유지 추가 시 requestLocationDetail 두번 호출 됨
           // 19.08.09 ymhong
           QMetaObject::invokeMethod(m_controller, "requestLocationDetail", Q_ARG(const TS_ROUTE_POS&, pos));
    }

   return location;
}

void PlatformControllerGen3::onDestinationReachedStatusChanged(RGDataPtr rg) {
  if (m_navigationServer) {
    uint status = UNKNOWN2;

    if(m_GoPosCode != -1)
    {
        if(m_GoPosCode != rg->eGoPosCode)
        {
            if (VSM_PosStart < m_GoPosCode && m_GoPosCode < VSM_PosGoal)
                status = WAYPOINT;
            else if (m_GoPosCode == VSM_PosGoal)
                status = FINAL_DESTINATION;

            m_GoPosCode = rg->eGoPosCode;

            qDebug() << "onDestinationReachedStatusChanged << " << status <<"(" << m_GoPosCode << ")";
            m_navigationServer->setDestinationReachedStatus((uint)status);
        }
    }else{
        m_GoPosCode = rg->eGoPosCode;
    }
  }
}

void PlatformControllerGen3::onWaypointOperationStatusList(const QList<int>& status) {
    if (m_navigationServer) {
       NSWaypointOpertaionStatusList list;
       int index = 0;
       for (int s : status) {
#if (1)
           QString str;
           switch (s) {
           case INSERTED: str = "INSERTED"; break;
           case DELETED: str = "DELETED"; break;
           case MOVED: str = "MOVED"; break;
           case REPLACED: str = "REPLACED"; break;
           default: str = "XXXX"; break;
           }
           qDebug() << "PlatformControllerGen3::onWaypointOperationStatusList(" << index << ") : " << str;
           ++index;
#endif
           list.append((uint)s);
       }

       m_navigationServer->setWaypointOperationStatusList(list);
    }
}


void PlatformControllerGen3::onNaviVolumeTouched(bool enabled)
{
    if (m_guiControl) {
        EnavHMIProperty volHmi;
        volHmi.hmiPropertyType  = 0;
        volHmi.hmiPropertyValue = 1;
        m_guiControl->setEnavHMIProperty(volHmi);
    }
}

void PlatformControllerGen3::onNaviShutdownReadyFinished(bool ok)
{
    if (ok) {
        if (m_navigationExt)
            m_navigationExt->NaviMethThirdPartyNaviStartState(3); // Okay, navi gets ready to power off
    }
}


void PlatformControllerGen3::onSetMapRepresentation(uint representation) {
//    qDebug() << "onSetMapRepresentation " << representation;

    int mode = 0;
    switch (representation) {
    case MapRepresentation::MapRepresentation_MAP_REPRESENTATION_2D_NORTH_UP:
            mode = VSM_VIEW_MODE_NORTHUP;
        break;
    case MapRepresentation::MapRepresentation_MAP_REPRESENTATION_2D_CAR_HEADING:
            mode = VSM_VIEW_MODE_HEADUP;
        break;
    case MapRepresentation::MapRepresentation_MAP_REPRESENTATION_3D_CAR_HEADING:
            mode = VSM_VIEW_MODE_BIRDVIEW;
        break;
    default:
        mode = 0;
        break;
    }
    QMetaObject::invokeMethod(m_controller,"setMapViewMode", Q_ARG(int, mode));
}
//void PlatformControllerGen3::onSetPoiIconCategoriesInMap(NSMapPOIIconCategories mapPOIIconCategories) {

//}

void PlatformControllerGen3::onSetRouteCriterion(uint routeCriterion) {
    int option = 0;
    switch (routeCriterion) {
    case RouteCriterion::RouteCriterion_ROUTE_CRITERION_ECONOMIC:
        option = 0x0004; // 무료도로
        break;
    case RouteCriterion::RouteCriterion_ROUTE_CRITERION_FAST:
        option = 0X0020; // 최소시간
        break;
    case RouteCriterion::RouteCriterion_ROUTE_CRITERION_SHORT:
        option = 0x0008;  // 최단거리
        break;
    default:
        option = 0x0001; // 최적길
        break;
    }

    m_controller->requestReRoute(option);// [TI190920] use QMetaObject::invokeMethod
}

//void PlatformControllerGen3::onSetSXMTravelLinkSubscriptionStatus(bool status) {

//}

void PlatformControllerGen3::onAudioMuteStateChanged(bool enabled) {
    if (m_navigationServer)
        m_navigationServer->setVoiceGuidanceActive(!enabled); // Oh No!!!,  voice guidance state always is negative of mute
}

void PlatformControllerGen3::onVoiceGuidanceTypeChanged(bool active)
{
   if (m_navigationServer)
        m_navigationServer->setVoiceGuidanceActive(active);
}

void PlatformControllerGen3::onOverSpeedAlertActiveChanged(bool enabled) {
    qDebug() << "pf, over speed alert:" << enabled;
    if (m_navigationServer)
        m_navigationServer->setOverSpeedSoundAlertInfoActive(enabled);

    QMetaObject::invokeMethod(m_controller,"setAlramGuidance", Q_ARG(bool,enabled));
}

void PlatformControllerGen3::onSetVoiceGuidance(bool active) {
    // m_controller->setMuteState(active);// [TI190920] use QMetaObject::invokeMethod
#if 1//cjlee 2019.01.24
    QMetaObject::invokeMethod(m_controller,"setVoiceGuidance", Q_ARG(bool,active));
#else
    QMetaObject::invokeMethod(m_controller,"setMuteState",Q_ARG(bool,active));
#endif
}

void PlatformControllerGen3::onSetZoomInStep(uchar zoomInStep) {
    ViewLevelInfo mViewLevelInfo = m_controller->naviProperty()->getTmapViewLevel();
    mViewLevelInfo.mLevel += static_cast<int>(zoomInStep);
    // mViewLevelInfo.mSubLevel = static_cast<int>(zoomInStep);
    QMetaObject::invokeMethod(m_controller,"setViewLevel", Q_ARG(ViewLevelInfo, mViewLevelInfo));
}

void PlatformControllerGen3::onSetZoomLevel(uint zoomLevel) {

    int level = 0;
    switch (zoomLevel) {
    default:
    case ZoomLevel_ZOOM_50M:    level = 10;     break;
    case ZoomLevel_ZOOM_100M:   level = 9;      break;
    case ZoomLevel_ZOOM_200M:   level = 8;      break;
    case ZoomLevel_ZOOM_500M:   level = 7;      break;
    case ZoomLevel_ZOOM_1000M:  level = 6;      break;
    case ZoomLevel_ZOOM_2000M:  level = 5;      break;
    case ZoomLevel_ZOOM_5000M:  level = 4;      break;
    case ZoomLevel_ZOOM_10000M: level = 3;      break;
    }

    ViewLevelInfo mViewLevelInfo;
    mViewLevelInfo.mLevel = level;
    mViewLevelInfo.mSubLevel = 0;;
    QMetaObject::invokeMethod(m_controller, "setViewLevel", Q_ARG(ViewLevelInfo, mViewLevelInfo));
}

void PlatformControllerGen3::onSetZoomOutStep(uchar zoomOutStep) {
    ViewLevelInfo mViewLevelInfo = m_controller->naviProperty()->getTmapViewLevel();
    mViewLevelInfo.mLevel -= static_cast<int>(zoomOutStep);
    // mViewLevelInfo.mSubLevel = static_cast<int>(zoomOutStep);
    QMetaObject::invokeMethod(m_controller, "setViewLevel", Q_ARG(ViewLevelInfo, mViewLevelInfo));
}

void PlatformControllerGen3::onShowAdjustCurrentLocationScreen(NSPositionWGS84 pos) {
    qDebug() << "hhtest onShowAdjustCurrentLocationScreen" << pos.m_longitude << pos.m_latitude;
    VSMPoint2f geo;
    geo.x = pos.m_longitude;
    geo.y = pos.m_latitude;
    VSMPoint2i world;
    bool bRet = VSMMap_Util_ConvertToWorld(geo, VSM_COORDINATE_WGS84, world);

    TS_ROUTE_POS routePos;
    memset(&routePos, 0, sizeof(TS_ROUTE_POS));
    routePos.tp.x = world.x;
    routePos.tp.y = world.y;

    emit m_controller->showAdjustCurrentLocationScreen(routePos);
}

void PlatformControllerGen3::onShowCalculateTour(QVariant v, uint screenTransition) {
    auto tour = v.value<NSTour>();
    qDebug() << "PlatformControllerGen3::onShowCalculateTour(" << tour.size() << ", screenTransition: " << screenTransition << ")";
    if (0 < tour.size()) {
        auto item = tour.at(0);
        uint option = 0;

        switch (item.m_routeCriterion) {
        case RouteCriterion_ROUTE_CRITERION_ECONOMIC:
            option = 0x0004; // 무료도로 E_ROUTEOPTION_OPTIMUM
            break;
        case RouteCriterion_ROUTE_CRITERION_FAST:
            option = 0X0020; // 최소시간 E_ROUTEOPTION_SHORTTIME
            break;
        case RouteCriterion_ROUTE_CRITERION_SHORT:
            option = 0x0008; // 최단거리 E_ROUTEOPTION_SHORTESTWAY
            break;
        default:
            option = 0x0001; // 최적길
            break;
        }

        enum ScreenTransition {
            ROUTE_INFO_SCREEN = 1,
            MAIN_MAP_SCREEN
        };
        auto apply = screenTransition == MAIN_MAP_SCREEN;// item.m_useDefaultRouteCriterion;

        int x= 0;
        int y= 0;

        VSMPoint2f geo;
        geo.x = item.m_location.m_location.m_positionWGS84.m_longitude;
        geo.y = item.m_location.m_location.m_positionWGS84.m_latitude;
        VSMPoint2i world;
        bool bRet = VSMMap_Util_ConvertToWorld(geo, VSM_COORDINATE_WGS84,world);
        x = world.x;
        y = world.y;

        TS_ROUTE_POS pos;
        memset(&pos, 0, sizeof(TS_ROUTE_POS));
        pos.tp.x = x;
        pos.tp.y = y;
        qstrncpy(pos.szName,item.m_location.m_location.m_name.toLatin1().constData(),100);

        QMetaObject::invokeMethod(m_controller, "showCalculateTour"
            , Q_ARG(TS_ROUTE_POS, pos)
            , Q_ARG(int, option)
            , Q_ARG(bool, apply));
    }
}

//NSPositionWGS84 PlatformControllerGen3::onShowCityInputAndGetCityCoordinates(bool &isValidAddress) {

//    NSPositionWGS84 mNSPositionWGS84;
//    return mNSPositionWGS84;
//}

void PlatformControllerGen3::onShowDeleteWaypointListScreen() {
    // m_controller->goViaPointEditScreen();// [TI190920] use QMetaObject::invokeMethod
    QMetaObject::invokeMethod(m_controller, "goViaPointEditScreenOccur");
}

//void PlatformControllerGen3::onShowDestinationOverviewFromTCU(NSLocation currentPosition, NSLocation destination) {

//}

//void PlatformControllerGen3::onShowMapScreenCarsorLockMode(uint mapCameraMode) {

//}

void PlatformControllerGen3::onShowOnMap(NSPositionWGS84 location) {
    VSMPoint2f geo;
    VSMPoint2i world;
    geo.x = location.m_longitude;
    geo.y = location.m_latitude;

    bool ok = VSMMap_Util_ConvertToWorld(geo, VSM_COORDINATE_WGS84,world);
    if (!ok) {
        qDebug() << Q_FUNC_INFO << ": failed to convert world coord from " << geo.x << ", " << geo.y;
    }

    QMetaObject::invokeMethod(m_controller, "showOnMap", Q_ARG(int, world.x), Q_ARG(int, world.y), Q_ARG(bool, ok));
    qDebug() << Q_FUNC_INFO << "show on map: " << location.m_longitude <<", " << location.m_latitude;
}

//void PlatformControllerGen3::onShowPOILocationFromTCU(NSPOILocations poiLocations) {

//}

void PlatformControllerGen3::onShowTrafficInfoScreen() {
    QMetaObject::invokeMethod(m_controller,"setTrafficInfoScreen", Q_ARG(bool, true));
}

void PlatformControllerGen3::onShowWhereAmIScreen() {
    QMetaObject::invokeMethod(m_controller,"moveToCurrentPosition");
}

void PlatformControllerGen3::onStartGuidance() {
    int option = 1;
    bool apply = true;
    // m_controller->requestRoute(m_Waypoints, option, apply);// [TI190920]
    // 1. needs to check the static waypoints variable whether it is thread-safe
    // 2. use QMetaObject::invokeMethod
    QMetaObject::invokeMethod(m_controller, "requestRoute"
        , Q_ARG(TS_ROUTE_POS, m_Waypoints[4])
        , Q_ARG(int, option)
        , Q_ARG(bool, apply));
}

void PlatformControllerGen3::onStartGuidanceToHomeLocation() {

    TS_ROUTE_POS home = m_controller->naviProperty()->getTmapHomeLocation();// [TI190920] move the codes that use naviProperty into TmapController. do not use navipropery directly in PlatformControllerGen3
    int option = 1;
    bool apply = true;

    // m_controller->requestRoute(home, option, apply);// [TI190920] use QMetaObject::invokeMethod
    QMetaObject::invokeMethod(m_controller, "requestRoute"
        , Q_ARG(TS_ROUTE_POS, home)
        , Q_ARG(int, option)
        , Q_ARG(bool, apply));
}

void PlatformControllerGen3::onStartGuidanceToPosWGS84(NSPositionWGS84 destination) {
 // no scenario
    VSMPoint2f geo;
    geo.x = destination.m_longitude;
    geo.y = destination.m_latitude;
    VSMPoint2i world;
    bool bRet = VSMMap_Util_ConvertToWorld(geo, VSM_COORDINATE_WGS84,world);

    TS_ROUTE_POS pos;
    memset(&pos,0, sizeof(TS_ROUTE_POS));
    pos.tp.x  = world.x;
    pos.tp.y  = world.y;

    int option = 1; // must use 1,  0 fire -294
    bool apply = true;

    QMetaObject::invokeMethod(m_controller,"requestRoute"
        , Q_ARG(TS_ROUTE_POS, pos)
        , Q_ARG(int, option)
        , Q_ARG(bool, apply));
}

void PlatformControllerGen3::onStartGuidanceToWorkLocation() {
    TS_ROUTE_POS work = m_controller->naviProperty()->getTmapWorkLocation();// [TI190920] move the codes that use naviProperty into TmapController. do not use navipropery directly in PlatformControllerGen3
    int option = 1;
    bool apply = true;

    // m_controller->requestRoute(work, option, apply);// [TI190920] use QMetaObject::invokeMethod
    QMetaObject::invokeMethod(m_controller,"requestRoute"
        , Q_ARG(TS_ROUTE_POS, work)
        , Q_ARG(int, option)
        , Q_ARG(bool, apply));
}

void PlatformControllerGen3::onStoreDestinationInAddressBook(NSPOILocation poiLocation) {
    VSMPoint2f geo;
    geo.x = poiLocation.m_location.m_positionWGS84.m_longitude;
    geo.y = poiLocation.m_location.m_positionWGS84.m_latitude;

    VSMPoint2i world;
    bool bRet = VSMMap_Util_ConvertToWorld(geo, VSM_COORDINATE_WGS84,world);
    if (bRet) {
        auto x = world.x;
        auto y = world.y;

        TS_ROUTE_POS pos;
        memset(&pos, 0, sizeof(TS_ROUTE_POS));
        qstrncpy(pos.szName,poiLocation.m_location.m_name.toLatin1().constData(),100);

        pos.tp.x = x;
        pos.tp.y = y;
        QMetaObject::invokeMethod(m_controller,"addFavoritesPosition",Q_ARG(TS_ROUTE_POS,pos));
    } else {
        qDebug() << "PlatformControllerGen3::onStoreDestinationInAddressBook() VSMMap_Util_ConvertToWorld failed.";
        qDebug() << "logitude:" << geo.x << ", latitude:" << geo.y;
    }
}

void PlatformControllerGen3::onStorePoiLocationToAddressBook(NSPOILocation poiLocation) {
    onStoreDestinationInAddressBook(poiLocation);
}

void PlatformControllerGen3::onStoreLocationToAddressBook(NSLocation location) {
    NSPOILocation pos;
    pos.m_location = location;
    onStoreDestinationInAddressBook(pos);
}


void PlatformControllerGen3::onSpeakSamplePhrase(uint sampleVoicePhrase)
{
    QMetaObject::invokeMethod(m_controller, "speakPhreaseSampleOccur", Q_ARG(int, sampleVoicePhrase));
}


void PlatformControllerGen3::onScreenIDChanged(int ScreenID) {
//    qDebug() << "onScreenIDChanged " << ScreenID;
#ifdef BUILD_TARGET
    Content ci1;
    ci1.m_nodeIndex = 4294967295;
        NodeBg nb1;
            nb1.m_bgBasePath = "";
            nb1.m_bgColor = 0;
            nb1.m_textColor = WHITE;
            nb1.m_textEffect = NOSHADOW;
    ci1.m_bgInfo = nb1;
    ci1.m_nodeType = ROOT;
    ci1.m_iconPath = "";
    ci1.m_buttonState = NORMAL;
        textInfoList tl1;
            textInfo ti1;
                ti1.m_textId = 0;
                ti1.m_textStr = "";
                ti1.m_type = false;
        tl1.append(ti1);
    ci1.m_textData = tl1;
        RestrictionInfo ri1;
        ri1.m_speedRestrictionCategory = SRCATEGORY_INVALID;
        ri1.m_speedRestrictionBehavior = 255;
    ci1.m_speedRestriction =ri1;
    ci1.m_editfieldMode = FREEMODE;
    ci1.m_sdsType = LISTENING;
    ci1.m_floatValue = 0;
    ci1.m_nodeLayoutType = NodeLayoutType_INVALID;

    Content ci2;
    ci2.m_nodeIndex = 1;
        NodeBg nb2;
            nb2.m_bgBasePath = "";
            nb2.m_bgColor = 0;
            nb2.m_textColor = WHITE;
            nb2.m_textEffect = NOSHADOW;
    ci2.m_bgInfo = nb2;
    ci2.m_nodeType = PULLUP;
    ci2.m_iconPath = "";
    ci2.m_buttonState = NORMAL;
        textInfoList tl2;
            textInfo ti2;
                ti2.m_textId = 1058643208;
                ti2.m_textStr = "";
                ti2.m_type = true;
        tl2.append(ti2);
    ci2.m_textData = tl2;
        RestrictionInfo ri2;
        ri2.m_speedRestrictionCategory = SRCATEGORY_INVALID;
        ri2.m_speedRestrictionBehavior = 255;
    ci2.m_speedRestriction =ri2;
    ci2.m_editfieldMode = FREEMODE;
    ci2.m_sdsType = LISTENING;
    ci2.m_floatValue = 0;
    ci2.m_nodeLayoutType = NodeLayoutType_INVALID;

    Content ci3;
    ci3.m_nodeIndex = 2;
        NodeBg nb3;
            nb3.m_bgBasePath = "";
            nb3.m_bgColor = 0;
            nb3.m_textColor = WHITE;
            nb3.m_textEffect = NOSHADOW;
    ci3.m_bgInfo = nb3;
    ci3.m_nodeType = PULLUP;
    ci3.m_iconPath = "";
    ci3.m_buttonState = NORMAL;
        textInfoList tl3;
            textInfo ti3;
                ti3.m_textId = 769391245;
                ti3.m_textStr = "";
                ti3.m_type = true;
        tl3.append(ti3);
    ci3.m_textData = tl3;
        RestrictionInfo ri3;
        ri3.m_speedRestrictionCategory = SRCATEGORY_INVALID;
        ri3.m_speedRestrictionBehavior = 255;
    ci3.m_speedRestriction =ri3;
    ci3.m_editfieldMode = FREEMODE;
    ci3.m_sdsType = LISTENING;
    ci3.m_floatValue = 0;
    ci3.m_nodeLayoutType = NodeLayoutType_INVALID;

    Content ci4;
    ci4.m_nodeIndex = 3;
        NodeBg nb4;
            nb4.m_bgBasePath = "";
            nb4.m_bgColor = 0;
            nb4.m_textColor = WHITE;
            nb4.m_textEffect = NOSHADOW;
    ci4.m_bgInfo = nb4;
    ci4.m_nodeType = PULLUP;
    ci4.m_iconPath = "";
    ci4.m_buttonState = NORMAL;
        textInfoList tl4;
            textInfo ti4;
                ti4.m_textId = 1410352885;
                ti4.m_textStr = "";
                ti4.m_type = true;
        tl4.append(ti4);
    ci4.m_textData = tl4;
        RestrictionInfo ri4;
        ri4.m_speedRestrictionCategory = SRCATEGORY_INVALID;
        ri4.m_speedRestrictionBehavior = 255;
    ci4.m_speedRestriction =ri4;
    ci4.m_editfieldMode = FREEMODE;
    ci4.m_sdsType = LISTENING;
    ci4.m_floatValue = 0;
    ci4.m_nodeLayoutType = NodeLayoutType_INVALID;

    Content ci5;
    ci5.m_nodeIndex = 4;
        NodeBg nb5;
            nb5.m_bgBasePath = "";
            nb5.m_bgColor = 0;
            nb5.m_textColor = WHITE;
            nb5.m_textEffect = NOSHADOW;
    ci5.m_bgInfo = nb5;
    ci5.m_nodeType = PULLUP;
    ci5.m_iconPath = "";
    ci5.m_buttonState = NORMAL;
        textInfoList tl5;
            textInfo ti5;
            ti5.m_textId = 0;
            ti5.m_textStr = "hilant-phone로 전환";
            ti5.m_type = false;
        tl5.append(ti5);
    ci5.m_textData = tl5;
        RestrictionInfo ri5;
        ri5.m_speedRestrictionCategory = SRCATEGORY_INVALID;
        ri5.m_speedRestrictionBehavior = 255;
    ci5.m_speedRestriction =ri5;
    ci5.m_editfieldMode = FREEMODE;
    ci5.m_sdsType = LISTENING;
    ci5.m_floatValue = 0;
    ci5.m_nodeLayoutType = NodeLayoutType_INVALID;
    ContentInfo cilEnable;
    cilEnable.append(ci1);
    cilEnable.append(ci2);
    cilEnable.append(ci3);
    cilEnable.append(ci4);
    cilEnable.append(ci5);
//------------------------------------------------------------------------------------------
    Content ci6;
    ci6.m_nodeIndex = 4294967295;
        NodeBg nb6;
            nb6.m_bgBasePath = "";
            nb6.m_bgColor = 0;
            nb6.m_textColor = WHITE;
            nb6.m_textEffect = NOSHADOW;
    ci6.m_bgInfo = nb6;
    ci6.m_nodeType = ROOT;
    ci6.m_iconPath = "";
    ci6.m_buttonState = NORMAL;
        textInfoList tl6;
            textInfo ti6;
                ti6.m_textId = 0;
                ti6.m_textStr = "";
                ti6.m_type = false;
        tl6.append(ti6);
    ci6.m_textData = tl6;
        RestrictionInfo ri6;
        ri6.m_speedRestrictionCategory = SRCATEGORY_INVALID;
        ri6.m_speedRestrictionBehavior = 255;
    ci6.m_speedRestriction =ri6;
    ci6.m_editfieldMode = FREEMODE;
    ci6.m_sdsType = LISTENING;
    ci6.m_floatValue = 0;
    ci6.m_nodeLayoutType = NodeLayoutType_INVALID;


    Content ci7;
    ci7.m_nodeIndex = 1;
        NodeBg nb7;
            nb7.m_bgBasePath = "";
            nb7.m_bgColor = 0;
            nb7.m_textColor = WHITE;
            nb7.m_textEffect = NOSHADOW;
    ci7.m_bgInfo = nb7;
    ci7.m_nodeType = NodeType_DEFAULT;
    ci7.m_iconPath = "Bitmaps##RIVIResources#Icon_Zone3#Icon_Zone3_Logs";
    ci7.m_buttonState = NORMAL;
        textInfoList tl7;
            textInfo ti7;
                ti7.m_textId = 46439846;
                ti7.m_textStr = "";
                ti7.m_type = true;
        tl7.append(ti7);
    ci7.m_textData = tl7;
        RestrictionInfo ri7;
        ri7.m_speedRestrictionCategory = SRCATEGORY_INVALID;
        ri7.m_speedRestrictionBehavior = 255;
    ci7.m_speedRestriction =ri7;
    ci7.m_editfieldMode = FREEMODE;
    ci7.m_sdsType = LISTENING;
    ci7.m_floatValue = 0;
    ci7.m_nodeLayoutType = NodeLayoutType_INVALID;

    Content ci8;
    ci8.m_nodeIndex = 2;
        NodeBg nb8;
            nb8.m_bgBasePath = "";
            nb8.m_bgColor = 0;
            nb8.m_textColor = WHITE;
            nb8.m_textEffect = NOSHADOW;
    ci8.m_bgInfo = nb8;
    ci8.m_nodeType = NodeType_DEFAULT;
    ci8.m_iconPath = "Bitmaps##RIVIResources#Icon_Zone3#Icon_Zone3_Contacts";
    ci8.m_buttonState = ACTIVE;
        textInfoList tl8;
            textInfo ti8;
                ti8.m_textId = 791731198;
                ti8.m_textStr = "";
                ti8.m_type = true;
        tl8.append(ti8);
    ci8.m_textData = tl8;
        RestrictionInfo ri8;
        ri8.m_speedRestrictionCategory = SRCATEGORY_INVALID;
        ri8.m_speedRestrictionBehavior = 255;
    ci8.m_speedRestriction =ri8;
    ci8.m_editfieldMode = FREEMODE;
    ci8.m_sdsType = LISTENING;
    ci8.m_floatValue = 0;
    ci8.m_nodeLayoutType = NodeLayoutType_INVALID;

    Content ci9;
    ci9.m_nodeIndex = 3;
        NodeBg nb9;
            nb9.m_bgBasePath = "";
            nb9.m_bgColor = 0;
            nb9.m_textColor = WHITE;
            nb9.m_textEffect = NOSHADOW;
    ci9.m_bgInfo = nb9;
    ci9.m_nodeType = NodeType_DEFAULT;
    ci9.m_iconPath = "Bitmaps##RIVIResources#Icon_Zone3#Icon_Zone3_Dial";
    ci9.m_buttonState = NORMAL;
        textInfoList tl9;
            textInfo ti9;
                ti9.m_textId = 4128744778;
                ti9.m_textStr = "";
                ti9.m_type = true;
        tl9.append(ti9);
    ci9.m_textData = tl9;
        RestrictionInfo ri9;
        ri9.m_speedRestrictionCategory = SRCATEGORY_INVALID;
        ri9.m_speedRestrictionBehavior = 255;
    ci9.m_speedRestriction =ri9;
    ci9.m_editfieldMode = FREEMODE;
    ci9.m_sdsType = LISTENING;
    ci9.m_floatValue = 0;
    ci9.m_nodeLayoutType = NodeLayoutType_INVALID;


    Content ci10;
    ci10.m_nodeIndex = 4;
        NodeBg nb10;
            nb10.m_bgBasePath = "";
            nb10.m_bgColor = 0;
            nb10.m_textColor = WHITE;
            nb10.m_textEffect = NOSHADOW;
    ci10.m_bgInfo = nb10;
    ci10.m_nodeType = ROOT;
    ci10.m_iconPath = "Bitmaps##RIVIResources#Icon_Zone3#Icon_Zone3_SMS";
    ci10.m_buttonState = NORMAL;
        textInfoList tl10;
            textInfo ti10;
                ti10.m_textId = 2582383027;
                ti10.m_textStr = "";
                ti10.m_type = false;
        tl10.append(ti10);
    ci10.m_textData = tl10;
        RestrictionInfo ri10;
        ri10.m_speedRestrictionCategory = SRCATEGORY_INVALID;
        ri10.m_speedRestrictionBehavior = 255;
    ci10.m_speedRestriction =ri10;
    ci10.m_editfieldMode = FREEMODE;
    ci10.m_sdsType = LISTENING;
    ci10.m_floatValue = 0;
    ci10.m_nodeLayoutType = NodeLayoutType_INVALID;


    Content ci11;
    ci11.m_nodeIndex = 1;
        NodeBg nb11;
            nb11.m_bgBasePath = "";
            nb11.m_bgColor = 0;
            nb11.m_textColor = WHITE;
            nb11.m_textEffect = NOSHADOW;
    ci11.m_bgInfo = nb11;
    ci11.m_nodeType = PULLUP;
    ci11.m_iconPath = "";
    ci11.m_buttonState = NORMAL;
        textInfoList tl11;
            textInfo ti11;
                ti11.m_textId = 1058643208;
                ti11.m_textStr = "";
                ti11.m_type = true;
        tl11.append(ti11);
    ci11.m_textData = tl11;
        RestrictionInfo ri11;
        ri11.m_speedRestrictionCategory = SRCATEGORY_INVALID;
        ri11.m_speedRestrictionBehavior = 255;
    ci11.m_speedRestriction =ri11;
    ci11.m_editfieldMode = FREEMODE;
    ci11.m_sdsType = LISTENING;
    ci11.m_floatValue = 0;
    ci11.m_nodeLayoutType = NodeLayoutType_INVALID;

    Content ci12;
    ci12.m_nodeIndex = 2;
        NodeBg nb12;
            nb12.m_bgBasePath = "";
            nb12.m_bgColor = 0;
            nb12.m_textColor = WHITE;
            nb12.m_textEffect = NOSHADOW;
    ci12.m_bgInfo = nb12;
    ci12.m_nodeType = PULLUP;
    ci12.m_iconPath = "";
    ci12.m_buttonState = NORMAL;
        textInfoList tl12;
            textInfo ti12;
                ti12.m_textId = 769391245;
                ti12.m_textStr = "";
                ti12.m_type = true;
        tl12.append(ti12);
    ci12.m_textData = tl12;
        RestrictionInfo ri12;
        ri12.m_speedRestrictionCategory = SRCATEGORY_INVALID;
        ri12.m_speedRestrictionBehavior = 255;
    ci12.m_speedRestriction =ri12;
    ci12.m_editfieldMode = FREEMODE;
    ci12.m_sdsType = LISTENING;
    ci12.m_floatValue = 0;
    ci12.m_nodeLayoutType = NodeLayoutType_INVALID;


    Content ci13;
    ci13.m_nodeIndex = 3;
        NodeBg nb13;
            nb13.m_bgBasePath = "";
            nb13.m_bgColor = 0;
            nb13.m_textColor = WHITE;
            nb13.m_textEffect = NOSHADOW;
    ci13.m_bgInfo = nb13;
    ci13.m_nodeType = PULLUP;
    ci13.m_iconPath = "";
    ci13.m_buttonState = NORMAL;
        textInfoList tl13;
            textInfo ti13;
                ti13.m_textId = 1410352885;
                ti13.m_textStr = "";
                ti13.m_type = true;
        tl13.append(ti13);
    ci13.m_textData = tl13;
        RestrictionInfo ri13;
        ri13.m_speedRestrictionCategory = SRCATEGORY_INVALID;
        ri13.m_speedRestrictionBehavior = 255;
    ci13.m_speedRestriction =ri13;
    ci13.m_editfieldMode = FREEMODE;
    ci13.m_sdsType = LISTENING;
    ci13.m_floatValue = 0;
    ci13.m_nodeLayoutType = NodeLayoutType_INVALID;

    Content ci14;
    ci14.m_nodeIndex = 4;
        NodeBg nb14;
            nb14.m_bgBasePath = "";
            nb14.m_bgColor = 0;
            nb14.m_textColor = WHITE;
            nb14.m_textEffect = NOSHADOW;
    ci14.m_bgInfo = nb14;
    ci14.m_nodeType = PULLUP;
    ci14.m_iconPath = "";
    ci14.m_buttonState = INACTIVE;
        textInfoList tl14;
            textInfo ti14;
                ti14.m_textId = 0;
                ti14.m_textStr = "hilant-phone로 전환";
                ti14.m_type = false;
        tl14.append(ti14);
    ci14.m_textData = tl14;
        RestrictionInfo ri14;
        ri14.m_speedRestrictionCategory = SRCATEGORY_INVALID;
        ri14.m_speedRestrictionBehavior = 255;
    ci14.m_speedRestriction =ri14;
    ci14.m_editfieldMode = FREEMODE;
    ci14.m_sdsType = LISTENING;
    ci14.m_floatValue = 0;
    ci14.m_nodeLayoutType = NodeLayoutType_INVALID;
    ContentInfo cilDisable;

    cilDisable.append(ci6);
    cilDisable.append(ci7);
    cilDisable.append(ci8);
    cilDisable.append(ci9);
    cilDisable.append(ci10);
    cilDisable.append(ci11);
    cilDisable.append(ci12);
    cilDisable.append(ci13);
    cilDisable.append(ci14);

#endif
}


void PlatformControllerGen3::NuanceVRTest()
{
    qDebug() << "E_SEARCHTYPE_NUANCEVR TEST";

    QString state, city, street, citycenter, jibun;
    TS_SEARCH_REQ sSearchReq;
    MNS_ErrorCode eError;

    for (int j = 0; j < 20; j++)
    {
        memset(&sSearchReq, 0x00, sizeof(sSearchReq));
        sSearchReq.eSearchType = E_MNS_SEARCHTYPE_NUANCEVR;
        sSearchReq.sSearchNuanceVR.eSortType = E_MNS_SORTTYPE_NAME;
        sSearchReq.sSearchNuanceVR.tpCurrent.x = 66573602;
        sSearchReq.sSearchNuanceVR.tpCurrent.y = 19695617;

        switch(j)
        {
        case 0:
            state = "서울";
            city = "도봉구";
            street = "방학동";
            citycenter = "";
            jibun = 302;
            break;
        case 1:
            state = "서울";
            city = "도봉구";
            street = "";
            citycenter = "방학동";
            jibun = "302";
            break;
        case 2:
            state = "경남";
            city = "창원시";
            street = "도계로";
            citycenter = "";
            jibun = "116";
            break;
        case 3:
            state = "경기";
            city = "성남시";
            street = "내정로165번길";
            citycenter = "";
            jibun = "50";
            break;
        case 4:
            state = "경기";
            city = "성남시";
            street = "수내동";
            citycenter = "";
            jibun = "33-1";
            break;
        case 5:
            state = "경남";
            city = "";
            street = "";
            citycenter = "";
            jibun = "";
            break;
        case 6:
            state = "경남";
            city = "창원시";
            street = "";
            citycenter = "";
            jibun = "";
            break;
        case 7:
            state = "경남";
            city = "창원시";
            street = "용잠리";
            citycenter = "";
            jibun = "575";
            break;
        case 8:
            state = "경남";
            city = "창원시";
            street = "";
            citycenter = "용잠리";
            jibun = "575";
            break;
        case 9:
            state = "경남";
            city = "";
            street = "용잠리";
            citycenter = "";
            jibun = "575";
            break;
        case 10:
            state = "경남";
            city = "";
            street = "";
            citycenter = "용잠리";
            jibun = "575";
            break;
        case 11:
            state = "경남";
            city = "창원시";
            street = "동읍";
            citycenter = "용잠리";
            jibun = "575";
            break;
        case 12:
            state = "경남";
            city = "";
            street = "동읍";
            citycenter = "용잠리";
            jibun = "575";
            break;
        case 13:
            state = "서울";
            city = "중구";
            street = "을지로";
            citycenter = "";
            jibun = "100";
            break;
        case 14:
            state = "서울";
            city = "";
            street = "을지로";
            citycenter = "";
            jibun = "100";
            break;
        case 15:
            state = "";
            city = "중구";
            street = "을지로";
            citycenter = "";
            jibun = "100";
            break;
        case 16:
            state = "";
            city = "중구";
            street = "";
            citycenter = "";
            jibun = "";
            break;
        case 17:
            state = "";
            city = "중구";
            street = "";
            citycenter = "";
            jibun = "100";
            break;
        case 18:
            state = "서울";
            city = "";
            street = "";
            citycenter = "";
            jibun = "";
            break;
        case 19:
            break;
            state = "서울";
            city = "";
            street = "";
            citycenter = "";
            jibun = "100";
        default:
            qDebug() << "NVR MNS_Search end ------------";
            return;
        }

        qDebug() << "NVR" << state << city << street << citycenter << jibun;
        strcpy(sSearchReq.sSearchNuanceVR.szLocaTypeState, TextDecoder::instance()->fromUnicode(state));
        strcpy(sSearchReq.sSearchNuanceVR.szLocaTypeCity, TextDecoder::instance()->fromUnicode(city));
        strcpy(sSearchReq.sSearchNuanceVR.szLocaTypeStreet, TextDecoder::instance()->fromUnicode(street));
        strcpy(sSearchReq.sSearchNuanceVR.szLocaTypeCitycenter, TextDecoder::instance()->fromUnicode(citycenter));
        strcpy(sSearchReq.sSearchNuanceVR.szJibun, TextDecoder::instance()->fromUnicode(jibun));

        int nCnt = 10;
        qDebug() << "NVR MNS_Search start";
        eError = MNS_Search(&sSearchReq, true, nCnt);
        qDebug() << "NVR MNS_Search eError: " << eError;
        qDebug() << "NVR MNS_Search nCnt: " << nCnt;

        if (eError == E_ERROR_SUCCESS && nCnt > 0)
        {
            TS_SEARCH_LIST list[8] = {0};       // 테스트코드이니, 8개만 가져오도록 처리
            nCnt = 8;
            qDebug() << "NVR MNS_GetSearchResultList start";
            eError = MNS_GetSearchResultList(sSearchReq.eSearchType, false, 0, nCnt, list);
            if (eError == E_ERROR_SUCCESS)
            {
                qDebug() << "NVR MNS_GetSearchResultList SUCCESS";
                for (int i = 0; i < nCnt; i++)
                {
                    QString name = TextDecoder::instance()->fromCP949(list[i].szName);
                    QString stdName = TextDecoder::instance()->fromCP949(list[i].szStdName);
                    int min1 = list[i].sAddrRange.nPrimaryMin;
                    int max1 = list[i].sAddrRange.nPrimaryMax;
                    int min2 = list[i].sAddrRange.nSecondaryMin;
                    int max2 = list[i].sAddrRange.nSecondaryMax;
                    qDebug() << "NVR" << list[i].nIdx << " : " << name << ", " << stdName << ", " << min1 << "~" << max1 << ", " << min2 << "~" << max2;
                }
            }
            else
            {
                qDebug() << "NVR eError: " << eError;
            }
        }
        qDebug() << " " << endl;
    }
}

void PlatformControllerGen3::contextSwitchResponseComplete()
{
    m_guiControl->ContextSwitchResponse(CONTEXT_STATE_COMPLETED);
    qDebug() << "[CTX] m_guiControl->ContextSwitchResponse(CONTEXT_STATE_COMPLETED) #2";
    emit contextStateCompleted();

    // Navi의 Map과 가젯의 Map이 공유 되므로 항상 표시 상태이므로 mMapViewVisible을 false로 할 필요 없음
    // 19.07.15 ymhong
    //mMapViewVisible = false;

    mContextSwitchResponseForComplete = false;
}


void PlatformControllerGen3::beforePlatformContextSwitchRequested()
{
#ifdef BUILD_TARGET
    static bool isFirst = true;
    qDebug() << "beforePlatformContextSwitchRequested() #1";

    mBlockZoneCommand = false;
    if (isFirst) {
        mZone1Type = (m_appHasNoMap) ? TITLE : NAV_SEARCH_BAR;
        bool updateForce = true;
        auto reply = setupZone1(mZone1Type, mZone1Param, updateForce);
        if (reply.isFinished()) {
            qDebug() << "reply.isFinished()";
        }
        if (reply.isError()) {
            auto dbusMessage = reply.reply();
            qDebug() << dbusMessage.errorMessage();
        }

        ContentInfo ci;
        Zone3Helper::setupForHideKeyboard(ci);
        Zone3InfoHolder param;
        param.m_zone3Type = Zone3Type_NO_DATA;
        param.m_nodeInfo = ci;
        param.m_updateRegion = true;
        m_hmiZoneManager->setZone3(3, param);
    }
    qDebug() << "beforePlatformContextSwitchRequested() #2 ... ";

    isFirst = false;
#endif
}

/*
 view.setSurfaceType(QSurface::OpenGLSurface);

QSurfaceFormat format;
//QSurfaceFormat format = view.format();
format.setAlphaBufferSize(8);
format.setRenderableType(QSurfaceFormat::OpenGL);
qDebug() <&lt; format.hasAlpha();
view.setFormat(format);

QColor color;
color.setRedF(0.0);
color.setGreenF(0.0);
color.setBlueF(0.0);
color.setAlphaF(0.0);
view.setColor(color);

view.setClearBeforeRendering(true);

view.connect(view.engine(), SIGNAL(quit()), SLOT(close()));
view.setFlags(Qt::FramelessWindowHint);
*/
QQuickView* PlatformControllerGen3::createPopupWindow(QQmlApplicationEngine* engine, const SurfaceProperty& surfaceProperty)
{
    QQuickView* quickView = new QQuickView(engine, nullptr);
#ifdef BUILD_TARGET
    qDebug() << "[popup] window = " << reinterpret_cast<int>(quickView);
#endif

    auto w = surfaceProperty.GetWidth();
    auto h = surfaceProperty.GetHeight();
    quickView->setWidth(w);
    quickView->setHeight(h);
    quickView->setSurfaceType(QSurface::OpenGLSurface);

    QSurfaceFormat format;
    format.setAlphaBufferSize(8);
    format.setRenderableType(QSurfaceFormat::OpenGLES);

    quickView->setFormat(format);
    quickView->setClearBeforeRendering(true);
    quickView->setFlags(Qt::FramelessWindowHint);
    quickView->setColor(QColor(0,0,0,0.95));

    return quickView;
}

void PlatformControllerGen3::getPublicConfigItem(const QString &publicItemName)
{
#ifdef BUILD_TARGET
    if (m_navigationExt) {
         m_navigationExt->NaviMethGetPublicConfigItem(publicItemName);
    }
#endif
}

void PlatformControllerGen3::onPublicConfigItem(uchar status, NXConfigInfoItemList publicConfigData)
{
#ifdef BUILD_TARGET
    qDebug() << "bValid:" << status << "    publicConfigData count: " << publicConfigData.size();
    if (!status) return;

    for (int ii = 0; ii < publicConfigData.size(); ii++) {
        NXConfigInfoItem item = publicConfigData.at(ii);
        if (item.m_key == "Base_URL") {
            QString baseName = item.m_value;
            QMetaObject::invokeMethod(m_controller,"changeHostBaseName", Q_ARG(const QString&, baseName));
        }
        qDebug() << "[publicConfigData] key: " << publicConfigData.at(ii).m_key << "    value: " << publicConfigData.at(ii).m_value;
    }
#endif
}

/** Deprecated!!
 * Use onNavigationDiagDataUpdateStatusInfo instead of this .
 */
void PlatformControllerGen3::setNavigationDataUpdateStatusInfo(int status, float progress) const
{
    NavigationDataUpdateStatusInfo info;
    info.m_status = (uint)status;
    info.m_progressValue = (uchar)progress;
    if (m_navigationSDSService != NULL) {
        m_navigationDataUpdateService->setNavigationDataUpdateStatusInfo(info);
    }
}

void PlatformControllerGen3::onCrossImageVisibleChanged(bool visible)
{
    qDebug() << "[PlatformControllerGen3][CrossImage] visible: " << visible;
}

void PlatformControllerGen3::onMapStartedToRender()
{
    qDebug() << "[CTX]PlatformControllerGen3::onMapStartedToRender #1";

    mMapViewVisible = true;

//    if (mContextSwitchResponseForComplete) {
//        qDebug() << "[CTX]PlatformControllerGen3::onMapStartedToRender #2";
//        contextSwitchResponseComplete();
//    }

//    if (mContextSwitchResponseForComplete) {
//        emit platformContextSwitchRequested(QVariantMap{
//            {QStringLiteral("request"), MAP_MAIN}
//        });
//    }
}

void PlatformControllerGen3::onMapVideoStatus()
{
    NSMapVideoStreamResult mNSMapVideoStreamResult;

    if(!mNaviAppStatue->VideoStream){
        mNSMapVideoStreamResult.m_mapVideoStreamStatus = TmapController::UNAVAILABLE;
    }else{
        if(mNaviAppStatue->AndroidAutoStatus)
        {
            mNSMapVideoStreamResult.m_mapVideoStreamStatus = TmapController::SPI_NAVI_ACTIVE;
            qDebug() << "kdttest setMapVideoStreamStatus #3";

        }else{
            mNSMapVideoStreamResult.m_mapVideoStreamStatus = TmapController::AVAILABLE;
        }
    }
//    if(!m_isStartStreamSignal)
//    {
//        mNSMapVideoStreamResult.m_mapVideoStreamStatus = TmapController::UNAVAILABLE;
//    }

    qDebug() << "kdttest setMapVideoStreamStatus #5" <<mNSMapVideoStreamResult.m_mapVideoStreamStatus;
    mNSMapVideoStreamResult.m_mapVideoStreamHandle = m_controller->VIDEO_STREAM_HANDLE;

    if(m_navigationServer)
        m_navigationServer->setMapVideoStreamStatus(mNSMapVideoStreamResult);
}

void PlatformControllerGen3::onContextSwitchResponded(QVariant args)
{
    qDebug() << "[GadgetSwitch]PlatformControllerGen3::onContextSwitchResponded #1";

    auto m = args.toMap();
    auto response = m[QStringLiteral("response")].toInt();
    if (response == NavigationController::CONTEXT_STATE_ACCEPTED) {
        if (m_guiControl) {
            qDebug() << "[GadgetSwitch]PlatformControllerGen3::onContextSwitchResponded #2 CONTEXT_STATE_ACCEPTED";
            m_guiControl->ContextSwitchResponse(CONTEXT_STATE_ACCEPTED);
            emit contextStateAccepted();
            contextSwitchResponseComplete();
        }
    }
}

void PlatformControllerGen3::onStopSendingGadgetImageRequested()
{
    qDebug() << "[GadgetSwitch]PlatformControllerGen3::onStopSendingGadgetImageRequested";

    m_gadgetStopped = true;
}

}//SKT
