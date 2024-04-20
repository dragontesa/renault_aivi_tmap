#include "platformcontroller_qnx.h"
#include "tmapcontroller.h"
#include <QDebug>
#include<QThread>

namespace SKT {

#define NAVISERVICE_REQUEST_ID           0x2000012

PlatformController* TmapController::newPlatformController(TmapController *controller) {
    return new PlatformControllerQnx(controller);
}

#ifdef PLATFORM_QNX
NAVI_GEO_COORDINATE_T & operator<<(NAVI_GEO_COORDINATE_T & dest, NAVI_GEO_COORDINATE & src) {
    dest.altitude = src.altitude;
    dest.latitude = src.latitude;
    dest.longitude = src.longitude;
    return dest;
}

NAVI_GEO_COORDINATE & operator<<(NAVI_GEO_COORDINATE & dest, NAVI_GEO_COORDINATE_T & src) {
    dest.altitude = src.altitude;
    dest.latitude = src.latitude;
    dest.longitude = src.longitude;
    return dest;
}

NAVI_ADDRESS & operator<<(NAVI_ADDRESS & dest, const NAVI_ADDRESS_T & src) {
    dest.addressCode = src.addressCode;
    dest.addressString = src.addressString;
    dest.blockNumber = src.blockNumber;
    dest.city = src.city;
    dest.country = src.country;
    dest.countryCode = src.countryCode;
    dest.county = src.county;
    dest.district = src.district;
    dest.houseNumber = src.houseNumber;
    dest.province = src.province;
    dest.state = src.state;
    dest.stateCode = src.stateCode;
    dest.street = src.street;
    dest.telephone = src.telephone;
    dest.zipCode = src.zipCode;
    return dest;
}

NAVI_ADDRESS_T & operator<<(NAVI_ADDRESS_T & dest, const NAVI_ADDRESS & src)
{
    dest.addressCode = src.addressCode;
    dest.addressString = src.addressString;
    dest.blockNumber = src.blockNumber;
    dest.city = src.city;
    dest.country = src.country;
    dest.countryCode = src.countryCode;
    dest.county = src.county;
    dest.district = src.district;
    dest.houseNumber = src.houseNumber;
    dest.province = src.province;
    dest.state = src.state;
    dest.stateCode = src.stateCode;
    dest.street = src.street;
    dest.telephone = src.telephone;
    dest.zipCode = src.zipCode;
    return dest;
}

NAVI_LOCATION_ADDRESS_COORDINATE & operator<<(NAVI_LOCATION_ADDRESS_COORDINATE & dest, const NAVI_LOCATION_ADDRESS_COORDINATE_T & src)
{
    dest.address<<src.address;
    dest.geoCoordinates.altitude = src.geoCoordinates.altitude;
    dest.geoCoordinates.latitude = src.geoCoordinates.latitude;
    dest.geoCoordinates.longitude = src.geoCoordinates.longitude;
    dest.locationName = src.locationName;
    return dest;
}

NAVI_LOCATION_ADDRESS_COORDINATE_T & operator<<(NAVI_LOCATION_ADDRESS_COORDINATE_T & dest, const NAVI_LOCATION_ADDRESS_COORDINATE & src) {
    dest.address<<src.address;
    dest.geoCoordinates.altitude = src.geoCoordinates.altitude;
    dest.geoCoordinates.latitude = src.geoCoordinates.latitude;
    dest.geoCoordinates.longitude = src.geoCoordinates.longitude;
    dest.locationName = src.locationName;
    return dest;
}

NAVI_ETA_TTA_DISTANCE_T & operator<<(NAVI_ETA_TTA_DISTANCE_T & dest, const NAVI_ETA_TTA_DISTANCE & src) {
    dest.distance = src.distance;
    dest.eta = src.eta;
    dest.tta = src.tta;
    return dest;
}

QSharedPointer <NAVI_LOCATION_ADDRESS_COORDINATE> operator<<(QSharedPointer <NAVI_LOCATION_ADDRESS_COORDINATE> dest, const NAVI_LOCATION_ADDRESS_COORDINATE_T& src) {
    dest->address<<src.address;
    dest->geoCoordinates.altitude = src.geoCoordinates.altitude;
    dest->geoCoordinates.latitude = src.geoCoordinates.latitude;
    dest->geoCoordinates.longitude = src.geoCoordinates.longitude;
    dest->locationName = src.locationName;
    return dest;
}

PlatformControllerQnx::PlatformControllerQnx(TmapController* controller)
    : BaseNaviService(MSG_BUFF_SIZE), m_controller(controller)
#else
PlatformControllerQnx::PlatformControllerQnx(TmapController* controller)
    : m_controller(controller)
#endif
{
    // monitor engine init
    connect(m_controller, &TmapController::engineInitialized,
            this, &PlatformControllerQnx::onEngineInitialized);
    connect(m_controller, &TmapController::applicationWindowCreated,
            this, &PlatformControllerQnx::onApplicationWindowCreated);

#ifdef PLATFORM_QNX
    m_pNaviProvider = NULL;
    m_pNaviDeploy = NULL;
    m_AppService = NULL;
    m_LocationService = NULL;
    m_KeyboardClient = NULL;
    m_PopupService = NULL;
    m_ResService = NULL;
    m_SystemSetting = NULL;
    m_ClusterService = NULL;
    m_UserprofileService = NULL;
    m_CanService = NULL;
    m_CcfService = NULL;
    m_BtService = NULL;
    m_DcmService = NULL;
    m_TjpService = NULL;
    m_SystemService = NULL;
    m_UsbDetectionManager = NULL;
    m_PartitionCtrl = NULL;
    m_ConnectivityService = NULL;
    m_AuthService = NULL;
    m_WindowService = NULL;
    m_VrService = NULL;
    m_NotificationService = NULL;

    bIsLPM = false;
#endif
}

PlatformControllerQnx::~PlatformControllerQnx()
{
#ifdef PLATFORM_QNX
    if(m_AppService != NULL)
        delete m_AppService;
    if(m_LocationService != NULL)
    {
        m_LocationService->SetLocationCallback(NULL);
        delete m_LocationService;
    }
    if(m_KeyboardClient != NULL)
        delete m_KeyboardClient;
    if(m_PopupService != NULL)
        delete m_PopupService;
    if(m_ResService != NULL)
        delete m_ResService;
    if(m_SystemSetting != NULL)
        delete m_SystemSetting;
    if(m_ClusterService != NULL)
        delete  m_ClusterService;
    if(m_UserprofileService != NULL)
        delete m_UserprofileService;
    if(m_CanService != NULL)
        delete m_CanService;
    if(m_CcfService != NULL)
        delete m_CcfService;
    if(m_BtService != NULL)
        delete m_BtService;
    if(m_DcmService != NULL)
        delete m_DcmService;
    if(m_TjpService != NULL)
        delete m_TjpService;
    if(m_SystemService != NULL)
        delete m_SystemService;
    if(m_UsbDetectionManager != NULL)
        delete m_UsbDetectionManager;
    if(m_PartitionCtrl != NULL)
        delete m_PartitionCtrl;
    if(m_ConnectivityService != NULL)
        delete m_ConnectivityService;
    if(m_AuthService != NULL)
        delete m_AuthService;
    if(m_WindowService != NULL)
        delete m_WindowService;
    if(m_VrService != NULL)
        delete m_VrService;
    if(m_NotificationService != NULL)
        delete m_NotificationService;

#ifdef USBSTATUS_QNXAPI
    QThread* thread = m_UsbStatus->thread();
    if (thread->isRunning()) {
        thread->quit();
        thread->wait();
    }

    if(m_UsbStatus != NULL)
        delete m_UsbStatus;
#endif
#endif
}

void PlatformControllerQnx::init()
{
    QMetaObject::invokeMethod(this, "platformInitialized",
                              Qt::QueuedConnection);
}

void PlatformControllerQnx::startService()
{
    QThread* thread = new QThread();
    thread->setObjectName("skt.tmap.PlatformControllerQnx");
    moveToThread(thread);
    connect(thread, &QThread::started, this, &PlatformControllerQnx::init);
//    connect(thread, &QThread::finished, this, &QObject::deleteLater);

    m_Thread = thread;
    thread->start();
//    init();
}

void PlatformControllerQnx::finishService()
{
    if (!m_Thread.isNull()) {
        m_Thread->quit();
        m_Thread->wait();
        m_Thread->deleteLater();
    }
}

QString PlatformControllerQnx::deviceId() const
{
    //return QString();
    return QLatin1String("0030006J");
}

bool PlatformControllerQnx::remount(const QString &path, bool readOnly)
{
    QMetaObject::invokeMethod(this, "platformRemountStatus",
                              Qt::QueuedConnection,
                              Q_ARG(bool, true));
    return true;
}

bool PlatformControllerQnx::restart()
{
    // 18.4.25 knlee - Modified to temporarily use when updating maps on a PC.
    // 18.8.22 khkim - Implemented to restart application, but not tested yet.

    qDebug("[PlatformControllerQnx] restart");
#ifdef PLATFORM_QNX
    if(isLPMMode())
        return false;

    if (m_SystemService == NULL)
        return false;

    //m_SystemService->requestRebootApplication();
    return true;
#endif
    return true;
}

bool PlatformControllerQnx::callNumber(const QString &number)
{
#ifdef PLATFORM_QNX
    if(isLPMMode())
        return false;

    if (m_AppService == NULL)
        return false;

    m_AppService->requestToGoApplication_CallPOI("", number.toStdString().c_str());
    return true;
#endif
    return false;
}

bool PlatformControllerQnx::dimmingStatus() const
{
    return false;
}

bool PlatformControllerQnx::networkOnline() const
{
    return false;
}

bool PlatformControllerQnx::appForeground() const
{
    return true;
}

char PlatformControllerQnx::reverseGearState() const
{
    return 0;
}

char PlatformControllerQnx::LanguageIndex() const
{
    return 0;
}

char PlatformControllerQnx::DateFormat() const
{
    return 0;
}

char PlatformControllerQnx::TimeFormat() const
{
    return 0;
}

uint PlatformControllerQnx::changeToProfile(const uchar userID) const
{
    return 0;
}

uint PlatformControllerQnx::copyProfile(const uchar sourceUserID, uchar destUserID) const
{
    return 0;
}

uint PlatformControllerQnx::createProfile(const QString &newName, uchar imageID) const
{
    return 0;
}

uint PlatformControllerQnx::createProfile(const QString &newName, uchar imageID, uchar &newUserID) const
{
    return 0;
}

uint PlatformControllerQnx::deleteProfile(uchar userID) const
{
    return 0;
}

uint PlatformControllerQnx::importProfileImage(const QString &sourcePath, ushort width, ushort height, ushort slotIdx, bool bSetForCurrentProfile) const
{
    return 0;
}

uint PlatformControllerQnx::doRegisterApp(const QString &AppName)
{
    return 0;
}

uint PlatformControllerQnx::doRegisterApp(const QString &AppName, QString &ApplName, bool &reloadData) const
{
    return 0;
}

uint PlatformControllerQnx::doSetProfileImage(uchar imageID, uchar userID) const
{
    return 0;
}

uint PlatformControllerQnx::doSetProfileName(const QString &newName) const
{
    return 0;
}

uint PlatformControllerQnx::do_SetProfileName(const QString &newName, uchar userID)
{
    return 0;
}

QByteArray PlatformControllerQnx::getProfileImages() const
{
    return 0;
}

QStringList PlatformControllerQnx::getProfileNames() const
{
    return QStringList();
}

uint PlatformControllerQnx::importProfileImage(const QString &sourcePath, ushort width, ushort height) const
{
    return 0;
}

void PlatformControllerQnx::onDataChanged(uint ApplicationPID, uint Result) const
{

}

void PlatformControllerQnx::onProfileCopied(uint ApplicationPID, uint Result) const
{

}

void PlatformControllerQnx::onProfileCreated(uint ApplicationPID, uint Result) const
{

}

void PlatformControllerQnx::onProfileDeleted(uint ApplicationPID, uint Result) const
{

}

uint PlatformControllerQnx::removeUserImage(ushort slotIdx) const
{
    return 0;
}

uint PlatformControllerQnx::setProfileImage(uchar imageID) const
{
    return 0;
}

void PlatformControllerQnx::displayZone3OnScroll(bool visibilityState) const
{

}

void PlatformControllerQnx::setEditfieldText(uchar appId, const QString &searchBarText, bool isHighLightRequired, uchar highlightStartIndex, uchar highlightCount) const
{

}

void PlatformControllerQnx::setKeyboard(uchar appId, uchar keyboardType, const QString &validCharacters) const
{

}

void PlatformControllerQnx::setSDSType(uint sdsType, uchar value) const
{

}

void PlatformControllerQnx::setZone3SubTextInfo(const QStringList &zone3SubText) const
{

}

void PlatformControllerQnx::hidePopup(const QString &viewname) const
{

}

void PlatformControllerQnx::showPopup(const QString &viewname) const
{

}

bool PlatformControllerQnx::onResponseCmdGoBack(int requestID, REQ_GO_BACK_RESULT eResult)
{
#ifdef PLATFORM_QNX
    if(isLPMMode())
        return false;

    if(m_AppService == NULL)
        return false;

    m_AppService->onResponseCmdGoBack(requestID, eResult);

    return true;
#endif
}

bool PlatformControllerQnx::onResponseReadyToSuspendSystemResult(int requestID, REQ_RESULT_MSG eResult)
{
#ifdef PLATFORM_QNX
#if 0
    if(m_AppService == NULL)
        return false;

    m_AppService->onResponseReadyToSuspendSystemResult(requestID, eResult);
#else
    if(m_SystemService == NULL)
        return false;

    m_SystemService->system_responseReadyToSuspend();
#endif

    return true;
#endif
}

bool PlatformControllerQnx::onResponseSuspendSystemResult(int requestID, REQ_RESULT_MSG eResult)
{
#ifdef PLATFORM_QNX
#if 0
    if(m_AppService == NULL)
        return false;

    m_AppService->onResponseSuspendSystemResult(requestID, eResult);
#else
    if(m_SystemService == NULL)
        return false;

    m_SystemService->system_responseSuspend();
#endif
    return true;
#endif
}

bool PlatformControllerQnx::onResponseReadyToResumeSystemResult(int requestID, REQ_RESULT_MSG eResult)
{
#ifdef PLATFORM_QNX
#if 0
    if(m_AppService == NULL)
        return false;

    m_AppService->onResponseReadyToResumeSystemResult(requestID, eResult);
#else
    if(m_SystemService == NULL)
        return false;

    bIsLPM = false;

    kpiBeginMeasurement(KPI_LPM_SEND_TO_CAR_DESTINATION_AVAILABLE);
    kpiBeginMeasurement(KPI_LPM_TRAFFIC_INFO_RESUMED);

    m_SystemService->system_responseReadyToResume();
#endif
    return true;
#endif
}

bool PlatformControllerQnx::onResponseResumeSystemResult(int requestID, REQ_RESULT_MSG eResult)
{
#ifdef PLATFORM_QNX
#if 0
    if(m_AppService == NULL)
        return false;

    m_AppService->onResponseResumeSystemResult(requestID, eResult);
#else
    if(m_SystemService == NULL)
        return false;

    m_SystemService->system_responseResume();
#endif

    return true;
#endif
}

bool PlatformControllerQnx::requestWaitingReadyToSuspend()
{
#ifdef PLATFORM_QNX

    if(m_SystemService == NULL)
        return false;

    m_SystemService->system_requestWaitingReadyToSuspend();

    return true;
#endif
}

bool PlatformControllerQnx::requestShowSystemComponent(int component)
{
#ifdef PLATFORM_QNX
    if(isLPMMode())
        return false;

    if(m_AppService == NULL)
        return false;

    if(component & SYSTEM_COMPONENT_SIDEPANEL_LEFT)
        m_AppService->requestShowSystemComponent(E_SYSTEM_COMPONENT_SIDEPANEL_LEFT);
    if(component & SYSTEM_COMPONENT_SIDEPANEL_RIGHT)
        m_AppService->requestShowSystemComponent(E_SYSTEM_COMPONENT_SIDEPANEL_RIGHT);
    if(component & SYSTEM_COMPONENT_SIDEPANEL_BOTH_LEFT_AND_RIGHT)
        m_AppService->requestShowSystemComponent(E_SYSTEM_COMPONENT_SIDEPANEL_BOTH_LEFT_AND_RIGHT);
    if(component & SYSTEM_COMPONENT_SIDEPANEL_VIEW)
        m_AppService->requestShowSystemComponent(E_SYSTEM_COMPONENT_SIDEPANEL_VIEW);
    if(component & SYSTEM_COMPONENT_SIDEPANEL_BACKBUTTON)
        m_AppService->requestShowSystemComponent(E_SYSTEM_COMPONENT_SIDEPANEL_BACKBUTTON);
    if(component & SYSTEM_COMPONENT_SYSTEM_KEYBOARD)
        m_AppService->requestShowSystemComponent(E_SYSTEM_COMPONENT_SYSTEM_KEYBOARD);
    if(component & SYSTEM_COMPONENT_SYSTEM_POPUP)
        m_AppService->requestShowSystemComponent(E_SYSTEM_COMPONENT_SYSTEM_POPUP);

    return true;
#endif
}

bool PlatformControllerQnx::requestHideSystemComponent(int component)
{
#ifdef PLATFORM_QNX
    if(isLPMMode())
        return false;

    if(m_AppService == NULL)
        return false;

    if(component & SYSTEM_COMPONENT_SIDEPANEL_LEFT)
        m_AppService->requestHideSystemComponent(E_SYSTEM_COMPONENT_SIDEPANEL_LEFT);
    if(component & SYSTEM_COMPONENT_SIDEPANEL_RIGHT)
        m_AppService->requestHideSystemComponent(E_SYSTEM_COMPONENT_SIDEPANEL_RIGHT);
    if(component & SYSTEM_COMPONENT_SIDEPANEL_BOTH_LEFT_AND_RIGHT)
        m_AppService->requestHideSystemComponent(E_SYSTEM_COMPONENT_SIDEPANEL_BOTH_LEFT_AND_RIGHT);
    if(component & SYSTEM_COMPONENT_SIDEPANEL_VIEW)
        m_AppService->requestHideSystemComponent(E_SYSTEM_COMPONENT_SIDEPANEL_VIEW);
    if(component & SYSTEM_COMPONENT_SIDEPANEL_BACKBUTTON)
        m_AppService->requestHideSystemComponent(E_SYSTEM_COMPONENT_SIDEPANEL_BACKBUTTON);
    if(component & SYSTEM_COMPONENT_SYSTEM_KEYBOARD)
        m_AppService->requestHideSystemComponent(E_SYSTEM_COMPONENT_SYSTEM_KEYBOARD);
    if(component & SYSTEM_COMPONENT_SYSTEM_POPUP)
        m_AppService->requestHideSystemComponent(E_SYSTEM_COMPONENT_SYSTEM_POPUP);

    return true;
#endif
}

bool PlatformControllerQnx::requestGoNavi(SHOW_OPT eOpt)
{
#ifdef PLATFORM_QNX
    if(isLPMMode())
        return false;

    if(m_AppService == NULL)
        return false;

    m_AppService->requestToGoApplication_Navi(eOpt);

    return true;
#endif
}

bool PlatformControllerQnx::requestGoShared()
{
#ifdef PLATFORM_QNX
    if(isLPMMode())
        return false;

    if(m_AppService == NULL)
        return false;

    m_AppService->requestToGoApplication_Shared();

    return true;
#endif
}

bool PlatformControllerQnx::getConnectivityStatusSync(bool &bConnected)
{
#ifdef PLATFORM_QNX
    if(isLPMMode())
        return false;

    if(m_ConnectivityService == NULL)
        return false;

    qDebug("[PlatformControllerQnx] getConnectivityStatusSync");

    return m_ConnectivityService->getConnectivityStatusSync(bConnected);
#endif
}

bool PlatformControllerQnx::InsertKeyboardSuggestionData(uint32_t userInputID, char* userInputText, KEYBOARD_CANDIDATE_DATA_TYPE iconType, QVariantList &data, int nTotalCount)
{
#ifdef PLATFORM_QNX
    if(isLPMMode())
        return false;

    if(m_KeyboardClient == NULL)
        return false;

    m_KeyboardClient->InsertKeyboardSuggestionData(userInputID, userInputText, iconType, data, nTotalCount);

    return true;
#endif
}

bool PlatformControllerQnx::InsertKeyboardSuggestionListData(char* userInputText, uint32_t startIndex, uint32_t endIndex, bool isRecent, QStringList &data)
{
#ifdef PLATFORM_QNX
    if(isLPMMode())
        return false;

    if(m_KeyboardClient == NULL)
        return false;

    m_KeyboardClient->InsertKeyboardSuggestionListData(userInputText, startIndex, endIndex, isRecent, data);

    return true;
#endif
}

bool PlatformControllerQnx::requestShowKeyboard(KEYBOARD_TYPE eKeyboardType, char* preFilledText, char* defaultText, bool isCapitalized, uint32_t maxLength)
{
#ifdef PLATFORM_QNX
    if(isLPMMode())
        return false;

    if(m_KeyboardClient == NULL)
        return false;

    qDebug("[PlatformControllerQnx] requestShowKeyboard  %d", eKeyboardType);

    m_KeyboardClient->requestShowKeyboard(eKeyboardType, preFilledText, defaultText, isCapitalized, maxLength);

    return true;
#endif
}

bool PlatformControllerQnx::requestShowKeyboard2(KEYBOARD_TYPE eKeyboardType, KEYBOARD_INIT_TYPE eInitType, char* defaultText, char* suggestionListCtx, unsigned int count, bool isCapitalized, unsigned int maxLength)
{
#ifdef PLATFORM_QNX
    if(isLPMMode())
        return false;

    if(m_KeyboardClient == NULL)
        return false;

    qDebug("[PlatformControllerQnx] requestShowKeyboard2  %d", eKeyboardType);

    m_KeyboardClient->requestShowKeyboard2(eKeyboardType, eInitType, defaultText, suggestionListCtx, count, isCapitalized, maxLength);

    return true;
#endif
}

bool PlatformControllerQnx::requestHideKeyboard()
{
#ifdef PLATFORM_QNX
    if(isLPMMode())
        return false;

    if(m_KeyboardClient == NULL)
        return false;

    qDebug("[PlatformControllerQnx] requestHideKeyboard");
    m_KeyboardClient->requestHideKeyboard();

    return true;
#endif
}

bool PlatformControllerQnx::requestShowPopup(POPUP_TYPE ePopupType, POPUP_SIDE_TYPE ePopupSideType, POPUP_SIZE ePopupSize, NAVI_POPUP_INFO *pPopupInfo, uint32_t priority)
{
#ifdef PLATFORM_QNX
    if(isLPMMode())
        return false;

    if(m_PopupService == NULL)
        return false;

    m_PopupService->requestShowPopup(ePopupType, ePopupSideType, ePopupSize, pPopupInfo, priority);

    return true;
#endif
}

bool PlatformControllerQnx::requestHidePopup(uint32_t handle)
{
#ifdef PLATFORM_QNX
    if(isLPMMode())
        return false;

    if(m_PopupService == NULL)
        return false;

    m_PopupService->requestHidePopup(handle);

    return true;
#endif
}

bool PlatformControllerQnx::requestShowSpecialPopup(SPECIAL_POPUP_TYPE eSpecialPopupType, NAVI_POPUP_INFO *pPopupInfo, uint32_t priority)
{
#ifdef PLATFORM_QNX
    if(isLPMMode())
        return false;

    if(m_PopupService == NULL)
        return false;

    m_PopupService->requestShowSpecialPopup(eSpecialPopupType, pPopupInfo, priority);

    return true;
#endif
}

bool PlatformControllerQnx::requestSetTurnByTurnInformation(char* roadName, JUNCTION_TYPE junction, int distance, int exitNumber)
{
#ifdef PLATFORM_QNX
    if(isLPMMode())
        return false;

    if(m_ClusterService == NULL)
        return false;

    m_ClusterService->requestSetTurnByTurnInformation(roadName, junction, distance, exitNumber);

    return true;
#endif
}

bool PlatformControllerQnx::requestSetDistance(int distance)
{
#ifdef PLATFORM_QNX
    if(isLPMMode())
        return false;

    if(m_ClusterService == NULL)
        return false;

    m_ClusterService->requestSetDistance(distance);

    return true;
#endif
}

void PlatformControllerQnx::requestSetNavigationLayout(NAVIGATION_TYPE naviType, NAVI_VIDEO_INFORMATION layout)
{
#ifdef PLATFORM_QNX
    if(isLPMMode())
        return;

    if(m_ClusterService == NULL)
        return;

    m_ClusterService->requestSetNavigationLayout(naviType, layout);
#endif
}

void PlatformControllerQnx::requestGetClusterType()
{
#ifdef PLATFORM_QNX
    if(isLPMMode())
        return;

    if(m_ClusterService == NULL)
        return;

    m_ClusterService->requestGetClusterType();
#endif
}

void PlatformControllerQnx::requestGetHudActivation()
{
#ifdef PLATFORM_QNX
    if(isLPMMode())
        return;

    if(m_ClusterService == NULL)
        return;

    m_ClusterService->requestGetHudActivation();
#endif
}

void PlatformControllerQnx::requestGetClusterVideoType()
{
#ifdef PLATFORM_QNX
    if(isLPMMode())
        return;

    if(m_ClusterService == NULL)
        return;

    m_ClusterService->requestGetClusterVideoType();
#endif
}

void PlatformControllerQnx::requestSetHUDLayout(const HUD_VIEW_MODE &viewMode, const NAVI_VIDEO_INFORMATION& videoLayout)
{
#ifdef PLATFORM_QNX
    if(isLPMMode())
        return;

    if(m_ClusterService == NULL)
        return;

    m_ClusterService->requestSetHUDLayout(viewMode, videoLayout);
#endif
}

void PlatformControllerQnx::notifyInitCcf()
{
#ifdef PLATFORM_QNX

    if (m_CcfService == NULL) {
        qWarning("[PlatformController] m_CcfService is NULL.");
        return;
    }

    emit PlatformRequestGetFuelType(m_CcfService->getFuelType());
    emit PlatformRequestGetVIN(m_CcfService->getVIN());
    emit PlatformRequestGetHybridType(m_CcfService->getHybridType());
    qDebug("[PlatformController] notifyInitCcf() fueltype=%d, VIN=%s, vehicle-type=%d, hybrid-type=%d, navi-system=%d",
           m_CcfService->getFuelType(), m_CcfService->getVIN(), m_CcfService->getVehicleType(), m_CcfService->getHybridType(), m_CcfService->getNaviSystem());
#endif
}

bool PlatformControllerQnx::requestGetAuthToken(std::string& reqURL, std::string& resToken, int& resHttpCode)
{
#ifdef PLATFORM_QNX
    if(isLPMMode())
        return false;

    if(m_AuthService == NULL)
        return false;

    m_AuthService->requestGetAuthToken(reqURL, resToken, resHttpCode);

    return true;
#endif
}

bool PlatformControllerQnx::requestCreateSessionHandle()
{
#ifdef PLATFORM_QNX
    if(isLPMMode())
        return false;

    if(m_WindowService == NULL)
        return false;

    m_WindowService->requestCreateSessionHandle();

    return true;
#endif
}

bool PlatformControllerQnx::requestFlushContext(int handle)
{
#ifdef PLATFORM_QNX
    if(isLPMMode())
        return false;

    if(m_WindowService == NULL)
        return false;

    m_WindowService->requestFlushContext(handle);

    return true;
#endif
}

bool PlatformControllerQnx::requestSetWindowPosition(int handle, char* windowName, int posX, int posY)
{
#ifdef PLATFORM_QNX
    if(isLPMMode())
        return false;

    if(m_WindowService == NULL)
        return false;

    m_WindowService->requestSetWindowPosition(handle, windowName, posX, posY);

    return true;
#endif
}

bool PlatformControllerQnx::requestCreateWindowGroup(char* winGroupName, int width, int height)
{
#ifdef PLATFORM_QNX
    if(isLPMMode())
        return false;

    if(m_WindowService == NULL)
        return false;

    m_WindowService->requestCreateWindowGroup(winGroupName, width, height);

    return true;
#endif
}

bool PlatformControllerQnx::requestRegisterGroup(int handle, char* windowName, char* winGroupName)
{
#ifdef PLATFORM_QNX
    if(isLPMMode())
        return false;

    if(m_WindowService == NULL)
        return false;

    m_WindowService->requestRegisterGroup(handle, windowName, winGroupName);

    return true;
#endif
}

bool PlatformControllerQnx::requestSetWindowSize(int handle, char* windowName, int width, int height)
{
#ifdef PLATFORM_QNX
    if(isLPMMode())
        return false;

    if(m_WindowService == NULL)
        return false;

    m_WindowService->requestSetWindowSize(handle, windowName, width, height);

    return true;
#endif
}

bool PlatformControllerQnx::requestSetWindowVisible(int handle, char* windowName, int visible)
{
#ifdef PLATFORM_QNX
    if(isLPMMode())
        return false;

    if(m_WindowService == NULL)
        return false;

    m_WindowService->requestSetWindowVisible(handle, windowName, visible);

    return true;
#endif
}

std::list<std::string> PlatformControllerQnx::getCreatedWindowList()
{
#ifdef PLATFORM_QNX
    std::list<std::string> list;
    if(isLPMMode())
        return list;

    if(m_WindowService == NULL)
        return list;

    return m_WindowService->getCreatedWindowList();
#endif
}

// Notification Service
bool PlatformControllerQnx::requestAddNotification(NOTIFY_TYPE eNotifyType, char* notifyContext)
{
#ifdef PLATFORM_QNX
    if(isLPMMode())
        return false;

    if(m_NotificationService == NULL)
        return false;

    m_NotificationService->requestAddNotification(eNotifyType, notifyContext);

    return true;
#endif
}

bool PlatformControllerQnx::requestRemoveNotification(unsigned int handle)
{
#ifdef PLATFORM_QNX
    if(isLPMMode())
        return false;

    if(m_NotificationService == NULL)
        return false;

    m_NotificationService->requestRemoveNotification(handle);

    return true;
#endif
}

bool PlatformControllerQnx::requestUpdateNotification(unsigned int handle, char* notifyContext)
{
#ifdef PLATFORM_QNX
    if(isLPMMode())
        return false;

    if(m_NotificationService == NULL)
        return false;

    m_NotificationService->requestUpdateNotification(handle, notifyContext);

    return true;
#endif
}

bool PlatformControllerQnx::requestDecreaseNumberOfGroupNotification(unsigned int handle, unsigned int number)
{
#ifdef PLATFORM_QNX
    if(isLPMMode())
        return false;

    if(m_NotificationService == NULL)
        return false;

    m_NotificationService->requestDecreaseNumberOfGroupNotification(handle, number);

    return true;
#endif
}

bool PlatformControllerQnx::requestResource(int channel)
{
#ifdef PLATFORM_QNX
    if(isLPMMode())
        return false;

    if(m_ResService == NULL)
        return false;

    m_ResService->requestResource(channel);
    return true;
#endif
}

bool PlatformControllerQnx::withdrawResource(int channel, int sessionId)
{
#ifdef PLATFORM_QNX
    if(isLPMMode())
        return false;

    if(m_ResService == NULL)
        return false;

    m_ResService->withdrawResource(channel, sessionId);

    return true;
#endif
}

#ifdef PLATFORM_QNX
ResService* PlatformControllerQnx::getResource()
{
    return m_ResService;
}
#endif

bool PlatformControllerQnx::isLPMMode()
{
    if(bIsLPM)
        qDebug("[PlatformControllerQnx] isLPMMode=%d", bIsLPM);

    return bIsLPM;
}

void PlatformControllerQnx::onEngineInitialized(bool success)
{
    Q_UNUSED(success)

    emit platformUIRequested();
    emit platformAppForegroundChanged(true);
}

void PlatformControllerQnx::onApplicationWindowCreated()
{
#ifdef PLATFORM_QNX
    m_pNaviDeploy = NaviDeploy::getInstance(MSG_BUFF_SIZE);
    //LOGI() << "[NaviService] registerDeploy(m_pNaviDeploy)";
    registerDeploy(m_pNaviDeploy, true);

    m_pNaviProvider = NaviServiceProvider::getInstance();

    m_WrapperApp = WrapperApp::getInstance(this, NAVI_APP_NAME);
    m_AppService = new AppService(m_WrapperApp);
    //m_WrapperApp = m_AppService->GetService();
    connect(m_AppService, &AppService::RegisterApplicationResult, this, &PlatformControllerQnx::onRegisterApplicationResult);
    connect(m_AppService, &AppService::ForegroundStateChanged, this, &PlatformControllerQnx::onForegroundStateChanged);
    connect(m_AppService, &AppService::ShowSystemComponentResult, this, &PlatformControllerQnx::onShowSystemComponentResult);
    connect(m_AppService, &AppService::HideSystemComponentResult, this, &PlatformControllerQnx::onHideSystemComponentResult);
    connect(m_AppService, &AppService::CmdShowApplication, this, &PlatformControllerQnx::onCmdShowApplication);
    connect(m_AppService, &AppService::CmdHideApplication, this, &PlatformControllerQnx::onCmdHideApplication);
    connect(m_AppService, &AppService::CmdResumeApplication, this, &PlatformControllerQnx::onCmdResumeApplication);
    connect(m_AppService, &AppService::CmdGoBack, this, &PlatformControllerQnx::onCmdGoBack);
    connect(m_AppService, &AppService::ButtonEvent, this, &PlatformControllerQnx::onButtonEvent);
    connect(m_AppService, &AppService::RotaryEvent, this, &PlatformControllerQnx::onRotaryEvent);
    connect(m_AppService, &AppService::SystemPopupVisibilityChanged, this, &PlatformControllerQnx::onSystemPopupVisibilityChanged);
    connect(m_AppService, &AppService::CmdTerminateApplication, this, &PlatformControllerQnx::onCmdTerminateApplication);

    if(m_WrapperApp != NULL) {
        m_AppService->requestToRegisterApplication();
    }

    //notifyInitValues(NULL);
    //
    //BaseService::run(true);

    if(m_locationcb != NULL)
    {
        m_WrapperLocation = WrapperLocation::getInstance(this, NAVI_APP_NAME);
        m_LocationService = new LocationService(m_WrapperLocation);
        //m_WrapperLocation = m_LocationService->GetService();
    }

    m_WrapperRes = WrapperResource::getInstance(this, NAVI_APP_NAME);
    m_ResService = new ResService(m_WrapperRes);
    //m_WrapperRes = m_ResService->GetService();
    connect(m_ResService, &ResService::ResponseRequestResourceAudio, this, &PlatformControllerQnx::onResponseRequestResourceAudio);
    connect(m_ResService, &ResService::ResponseWithdrawResourceAudio, this, &PlatformControllerQnx::onResponseWithdrawResourceAudio);
    connect(m_ResService, &ResService::ResponseRequestResourceAlarm, this, &PlatformControllerQnx::onResponseRequestResourceAlarm);
    connect(m_ResService, &ResService::ResponseWithdrawResourceAlarm, this, &PlatformControllerQnx::onResponseWithdrawResourceAlarm);

    m_WrapperPopup = WrapperPopup::getInstance(this, NAVI_APP_NAME);
    m_PopupService = new PopupService(m_WrapperPopup);
    //m_WrapperPopup = m_PopupService->GetService();
    connect(m_PopupService, &PopupService::ResponseShowPopup, this, &PlatformControllerQnx::onResponseShowPopup);
    connect(m_PopupService, &PopupService::ResponseHidePopup, this, &PlatformControllerQnx::onResponseHidePopup);
    connect(m_PopupService, &PopupService::NotifyPopupResult, this, &PlatformControllerQnx::onNotifyPopupResult);
    connect(m_PopupService, &PopupService::ResponseUpdatePopup, this, &PlatformControllerQnx::onResponseUpdatePopup);
    connect(m_PopupService, &PopupService::NotifyStatusChanged, this, &PlatformControllerQnx::onNotifyStatusChanged);

    m_WrapperSystemsetting = WrapperSystemsetting::getInstance(this, NAVI_APP_NAME);
    m_SystemSetting = new SystemSetting(m_WrapperSystemsetting);
    //m_WrapperSystemsetting = m_SystemSetting->GetService();
    connect(m_SystemSetting, &SystemSetting::SystemLanguageUpdate, this, &PlatformControllerQnx::onSystemLanguageUpdate);
    connect(m_SystemSetting, &SystemSetting::SystemUIThemeUpdate, this, &PlatformControllerQnx::onSystemUIThemeUpdate);
    connect(m_SystemSetting, &SystemSetting::NaviMapUIThemeUpdate, this, &PlatformControllerQnx::onNaviMapUIThemeUpdate);

    m_WrapperCan = WrapperCan::getInstance(this, NAVI_APP_NAME);
    m_CanService = new CanService(m_WrapperCan);
    //m_WrapperCan = m_CanService->GetService();
    connect(m_CanService, &CanService::DistanceToEmpty, this, &PlatformControllerQnx::onDistanceToEmpty);
    connect(m_CanService, &CanService::LowFuelLevel, this, &PlatformControllerQnx::onLowFuelLevel);
    connect(m_CanService, &CanService::FuelGaugePosition, this, &PlatformControllerQnx::onFuelGaugePosition);
    connect(m_CanService, &CanService::HVBatteryUsableSOCDisp, this, &PlatformControllerQnx::onHVBatteryUsableSOCDisp);
    connect(m_CanService, &CanService::HVBattLevelColourDisp, this, &PlatformControllerQnx::onHVBattLevelColourDisp);

    //m_CanService->subscribeSignal();

    m_WrapperCluster = WrapperCluster::getInstance(this, NAVI_APP_NAME);
    m_ClusterService = new ClusterService(m_WrapperCluster);
    //m_WrapperCluster = m_ClusterService->GetService();
    connect(m_ClusterService, &ClusterService::ResponseSetTurnByTurnInformation, this, &PlatformControllerQnx::onResponseSetTurnByTurnInformation);
    connect(m_ClusterService, &ClusterService::ResponseSetDistance, this, &PlatformControllerQnx::onResponseSetDistance);
    connect(m_ClusterService, &ClusterService::NotifyViewLayoutChangeFromCluster, this, &PlatformControllerQnx::onNotifyViewLayoutChangeFromCluster);
    connect(m_ClusterService, &ClusterService::ResponseSetNavigationLayout, this, &PlatformControllerQnx::onResponseSetNavigationLayout);
    connect(m_ClusterService, &ClusterService::ResponseGetClusterType, this, &PlatformControllerQnx::onResponseGetClusterType);
    connect(m_ClusterService, &ClusterService::ResponseGetHUDActivation, this, &PlatformControllerQnx::onResponseGetHUDActivation);
    connect(m_ClusterService, &ClusterService::ResponseGetClusterVideoType, this, &PlatformControllerQnx::onResponseGetClusterVideoType);
    connect(m_ClusterService, &ClusterService::ResponseSetHUDLayout, this, &PlatformControllerQnx::onResponseSetHUDLayout);
    connect(m_ClusterService, &ClusterService::NotifyHudViewModeChangeFromCluster, this, &PlatformControllerQnx::onNotifyHudViewModeChangeFromCluster);

    m_WrapperUserprofile = WrapperUserprofile::getInstance(this, NAVI_APP_NAME);
    m_UserprofileService = new UserprofileService(m_WrapperUserprofile);
    //m_WrapperUserprofile = m_UserprofileService->GetService();
    connect(m_UserprofileService, &UserprofileService::ResponseGetUserLoginStatus, this, &PlatformControllerQnx::onResponseGetUserLoginStatus);
    connect(m_UserprofileService, &UserprofileService::BrProfileExpired, this, &PlatformControllerQnx::onBrProfileExpired);
    connect(m_UserprofileService, &UserprofileService::BrUpdateLoginStatus, this, &PlatformControllerQnx::onBrUpdateLoginStatus);

    m_WrapperCcf = WrapperCcf::getInstance(this, NAVI_APP_NAME);
    m_CcfService = new CcfService(m_WrapperCcf);
    //m_WrapperCcf = m_CcfService->GetService();
    //connect(m_CcfService, &CcfService::ResponseGetFuelType, this, &PlatformControllerQnx::onResponseGetFuelType);
    //connect(m_CcfService, &CcfService::ResponseGetVIN, this, &PlatformControllerQnx::onResponseGetVIN);


    m_WrapperKeyboard = WrapperKeyboard::getInstance(this, NAVI_APP_NAME);
    m_KeyboardClient = new KeyboardClient(m_WrapperKeyboard);
    //m_WrapperKeyboard = m_KeyboardClient->GetService();
    connect(m_KeyboardClient, &KeyboardClient::ResponseShowKeyboard, this, &PlatformControllerQnx::onResponseShowKeyboard);
    connect(m_KeyboardClient, &KeyboardClient::ResponseHideKeyboard, this, &PlatformControllerQnx::onResponseHideKeyboard);
    connect(m_KeyboardClient, &KeyboardClient::NotifyKeyboardResult, this, &PlatformControllerQnx::onNotifyKeyboardResult);
    connect(m_KeyboardClient, &KeyboardClient::RequestCandidateInfo, this, &PlatformControllerQnx::onRequestCandidateInfo);
    connect(m_KeyboardClient, &KeyboardClient::RequestCandidateList, this, &PlatformControllerQnx::onRequestCandidateList);
    connect(m_KeyboardClient, &KeyboardClient::NotifyKeyboardError, this, &PlatformControllerQnx::onNotifyKeyboardError);

    m_WrapperBt = WrapperBluetooth::getInstance(this, NAVI_APP_NAME);
    m_BtService = new BtService(m_WrapperBt);
    //m_WrapperBt = m_BtService->GetService();
    connect(m_BtService, &BtService::PhoneAvailableChanged, this, &PlatformControllerQnx::onPhoneAvailableChanged);

    m_WrapperDcm = WrapperDcm::getInstance(this, NAVI_APP_NAME);
    m_DcmService = new DcmService(m_WrapperDcm);
    //m_WrapperDcm = m_DcmService->GetService();
    connect(m_DcmService, &DcmService::DCMFatigueStateToNavi, this, &PlatformControllerQnx::onDCMFatigueStateToNavi);
    connect(m_DcmService, &DcmService::RequestToFindRestAreas, this, &PlatformControllerQnx::onRequestToFindRestAreas);
    connect(m_DcmService, &DcmService::ReqToDisplayRestAreas, this, &PlatformControllerQnx::onReqToDisplayRestAreas);

    m_WrapperTjp = WrapperTjp::getInstance(this, NAVI_APP_NAME);
    m_TjpService = new TjpService(m_WrapperTjp);
    //m_WrapperTjp = m_TjpService->GetService();
    connect(m_TjpService, &TjpService::TJPStatusChange, this, &PlatformControllerQnx::onTJPStatusChange);

    m_WrapperSystem = WrapperSystem::getInstance("hmi.navi", this, NAVI_APP_NAME);
    m_SystemService = new SystemService(m_WrapperSystem);
    //m_WrapperSystem = m_SystemService->GetService();
    connect(m_SystemService, &SystemService::ReadyToSuspend, this, &PlatformControllerQnx::onCmdReadyToSuspendSystem);
    connect(m_SystemService, &SystemService::ReadyToResume, this, &PlatformControllerQnx::onCmdReadyToResumeSystem);
    connect(m_SystemService, &SystemService::WaitingReadyToSuspend, this, &PlatformControllerQnx::onWaitingReadyToSuspend);

    m_WrapperUsbMgr = WrapperUsbMgr::getInstance();
    m_UsbDetectionManager = new UsbDetectionManager(m_WrapperUsbMgr);
    //m_WrapperUsbMgr = m_UsbDetectionManager->GetService();
    //connect(m_UsbDetectionManager, &UsbDetectionManager::DeviceListChanged, this &PlatformControllerQnx::onDeviceListChanged);
    connect(m_UsbDetectionManager, &UsbDetectionManager::DeviceChanged, this, &PlatformControllerQnx::onDeviceChanged);

    /*
    m_WrapperPartitionCtrl = WrapperPartitionCtrl::getInstance();
    m_PartitionCtrl = new PartitionCtrl(m_WrapperPartitionCtrl);
    //m_WrapperPartitionCtrl = m_PartitionCtrl->GetService();
    connect(m_PartitionCtrl, &PartitionCtrl::ResponsePartitionOptionChange, this, &PlatformControllerQnx::onResponsePartitionOptionChange);
    */

    connectedPartitionOptionChange();

    m_WrapperConnectivity = WrapperConnectivity::getInstance(this, NAVI_APP_NAME);
    m_ConnectivityService = new ConnectivityService(m_WrapperConnectivity);
    //m_WrapperConnectivity = m_ConnectivityService->GetService();
    connect(m_ConnectivityService, &ConnectivityService::ResponseConnectivityStatus, this, &PlatformControllerQnx::onResponseConnectivityStatus);

    m_WrapperAuth = WrapperAuth::getInstance();
    m_AuthService = new AuthService(m_WrapperAuth);
    //m_WrapperAuth = m_AuthService->GetService();
    connect(m_AuthService, &AuthService::VehicleTokenRefreshed, this, &PlatformControllerQnx::onVehicleTokenRefreshed);

    m_WrapperWindow = WrapperWindow::getInstance(this, NAVI_APP_NAME);
    m_WindowService = new WindowService(m_WrapperWindow);
    //m_WrapperWindow = m_WindowService->GetService();
    connect(m_WindowService, &WindowService::CreateSessionHandle, this, &PlatformControllerQnx::onCreateSessionHandle);
    connect(m_WindowService, &WindowService::FlushContext, this, &PlatformControllerQnx::onFlushContext);
    connect(m_WindowService, &WindowService::SetWindowPosition, this, &PlatformControllerQnx::onSetWindowPosition);
    connect(m_WindowService, &WindowService::CreateWindowGroup, this, &PlatformControllerQnx::onCreateWindowGroup);
    connect(m_WindowService, &WindowService::RegisterGroup, this, &PlatformControllerQnx::onRegisterGroup);
    connect(m_WindowService, &WindowService::SetWindowSize, this, &PlatformControllerQnx::onSetWindowSize);
    connect(m_WindowService, &WindowService::SetWindowVisible, this, &PlatformControllerQnx::onSetWindowVisible);


    m_WrapperVr = WrapperVr::getInstance(this, NAVI_APP_NAME);
    m_VrService = new VrService(m_WrapperVr);
    connect(m_VrService, &VrService::ShowSpeechDialogRequest, this, &PlatformControllerQnx::onShowSpeechDialogRequest);
    connect(m_VrService, &VrService::HideSpeechDialogRequest, this, &PlatformControllerQnx::onHideSpeechDialogRequest);
    connect(m_VrService, &VrService::ShowSpeechPicklistRequest, this, &PlatformControllerQnx::onShowSpeechPicklistRequest);
    connect(m_VrService, &VrService::PicklistScrollRequest, this, &PlatformControllerQnx::onPicklistScrollRequest);
    connect(m_VrService, &VrService::SelectedPicklistItemRequest, this, &PlatformControllerQnx::onSelectedPicklistItemRequest);
    connect(m_VrService, &VrService::GetSelectedPhonenumberRequest, this, &PlatformControllerQnx::onGetSelectedPhonenumberRequest);


    //m_WrapperVr = m_VrService->GetService();

    m_WrapperNotification = WrapperNotification::getInstance(this, NAVI_APP_NAME);
    m_NotificationService = new NotificationService(m_WrapperNotification);
    connect(m_NotificationService, &NotificationService::ResponseAddNotification, this, &PlatformControllerQnx::onResponseAddNotification);
    connect(m_NotificationService, &NotificationService::ResponseRemoveNotification, this, &PlatformControllerQnx::onResponseRemoveNotification);
    connect(m_NotificationService, &NotificationService::ResponseUpdateNotification, this, &PlatformControllerQnx::onResponseUpdateNotification);
    connect(m_NotificationService, &NotificationService::ResponseDecreaseNumberOfGroupNotification, this, &PlatformControllerQnx::onResponseDecreaseNumberOfGroupNotification);
    connect(m_NotificationService, &NotificationService::ResponseClickButton, this, &PlatformControllerQnx::onResponseClickButton);
    connect(m_NotificationService, &NotificationService::ListNotificationChange, this, &PlatformControllerQnx::onListNotificationChange);
    connect(m_NotificationService, &NotificationService::FeedbackRemoveNotificationTimeout, this, &PlatformControllerQnx::onFeedbackRemoveNotificationTimeout);
    connect(m_NotificationService, &NotificationService::NotifyRemoveNotiCardByUserAction, this, &PlatformControllerQnx::onNotifyRemoveNotiCardByUserAction);

#ifdef USBSTATUS_QNXAPI
    m_UsbStatus = new UsbStatus();
    QThread* thread = new QThread();
    m_UsbStatus->moveToThread(thread);

    connect(thread, &QThread::started, m_UsbStatus, &UsbStatus::Start);
    connect(m_UsbStatus, &UsbStatus::finished, thread, &QThread::quit);
    connect(thread, &QThread::finished, thread, &QThread::quit);
    connect(m_UsbStatus, &UsbStatus::UsbStatusNoti, this, &PlatformControllerQnx::onUsbStatusNoti);

    thread->start();
#endif

    m_SystemService->notifyStartupDone();

    BaseService::run(true);

    notifyInitCcf();

    onNotifyNaviTimezoneInfo(32400, false, "KST");

    if((m_locationcb != NULL) && (m_LocationService != NULL))
        m_LocationService->SetLocationCallback(m_locationcb);

    char* langcode = getSystemLanguage();

    emit PlatformSystemLanguageUpdate(langcode);

    emit PlatformClusterSetting();
    publishRouteStatusIGM(CAN_ROUTE_STATUS_SYSTEM_START);

#endif
}

#ifdef PLATFORM_QNX
void PlatformControllerQnx::connectedPartitionOptionChange(){

    if(m_WrapperPartitionCtrl == nullptr){
        m_WrapperPartitionCtrl = WrapperPartitionCtrl::getInstance();
        m_PartitionCtrl = new PartitionCtrl(m_WrapperPartitionCtrl);
        //m_WrapperPartitionCtrl = m_PartitionCtrl->GetService();
        connect(m_PartitionCtrl, &PartitionCtrl::ResponsePartitionOptionChange, this, &PlatformControllerQnx::onResponsePartitionOptionChange);
    }
}
#endif




void PlatformControllerQnx::setLocationCallback(qnx_location_callback callback)
{
    m_locationcb = callback;
}

void PlatformControllerQnx::onRegisterApplicationResult(REQ_RESULT_MSG eResult)
{
#ifdef PLATFORM_QNX
    qWarning("PlatformControllerQnx::onRegisterApplicationResult %d", eResult);

//    if(REQ_RESULT_MSG_OK == eResult)
//    {
//        if((m_locationcb != NULL) && (m_LocationService != NULL))
//            m_LocationService->SetLocationCallback(m_locationcb);
//    }
#endif
}

void PlatformControllerQnx::onForegroundStateChanged(bool foreground)
{
    emit platformAppForegroundChanged(foreground);
}

void PlatformControllerQnx::onCmdGoBack(int requestID)
{
    emit PlatformCmdGoBack(requestID);
}

bool PlatformControllerQnx::onHandleMessages(const char* cmd, const void* data, const int& size)
{
    qWarning() << "[PlatformControllerQnx] onHandleMessage() called.";

    if(isLPMMode())
        return false;

#ifdef PLATFORM_QNX
    qWarning("--> Cmd %s from %s, path %s", cmd, getFromName(), getFromPath());
#endif
    return true;
}


bool PlatformControllerQnx::onClientConnected(const char* name, const char* path, const char* ver)
{
    qWarning("[PlatformControllerQnx] onClientConnected. name(%s), path(%s), ver(%s)", name, path, ver);

    if(isLPMMode())
        return false;

    notifyInitValues(path);

    return true;
}

bool PlatformControllerQnx::onClientDisconnected(const char* name, const char* path)
{
    qWarning("[PlatformControllerQnx] onClientDisconnected. name(%s), path(%s)", name, path);

    if(isLPMMode())
        return false;

    notifyInitValues(path);

    return true;

}

bool PlatformControllerQnx::onServiceConnected(const char* name, const char* path, const char* ver)
{
    qWarning("[PlatformControllerQnx] onServiceConnected. name(%s), path(%s), ver(%s)", name, path, ver);

    if(isLPMMode())
        return false;
#if 0
    if((name == NULL) || (path == NULL) || (ver == NULL))
    {
        qWarning("PlatformControllerQnx::onServiceConnected >> Invalid Service Info");
    }
    else
    {
        if(0 == strcmp(path, MQ_SYSTEM_WINDOW_SERVICE_PATH))
        {
            m_WindowService->requestCreateSessionHandle();
            qWarning("PlatformControllerQnx::requestCreateSessionHandle");
        }
    }
#endif
    notifyInitValues(path);

#ifdef PLATFORM_QNX
    if(0 == strcmp(path, MQ_SPEECH_SPEECH_SERVICE_PATH))
    {
        qInfo("m_WrapperVr->speechLocationChangedNotification()");
        m_WrapperVr->speechLocationChangedNotification();
    }
    else if (0 == strcmp(path, MQ_SYSTEM_USERPROFILE_SERVICE_PATH))
    {
        m_UserprofileService->requestGetUserLoginStatus();
    }
#endif

    return true;

}

bool PlatformControllerQnx::onServiceDisconnected(const char* name, const char* path)
{
    qWarning("[PlatformControllerQnx] onServiceDisconnected. name(%s), path(%s)", name, path);

    if(isLPMMode())
        return false;

    notifyInitValues(path);

    return true;

}

void PlatformControllerQnx::notifyInitValues(const char* path)
{
    qWarning() << "[PlatformControllerQnx] notifyInitValues called()";
}

#ifdef PLATFORM_QNX // System Setting
char* PlatformControllerQnx::getSystemLanguage()
{
    qDebug("[PlatformControllerQnx] getSystemLanguage");
    if(isLPMMode())
        return NULL;

    std::string lang(m_SystemSetting->getSystemLanguage().code);
    return  const_cast<char*>(lang.c_str());
}

UI_THEME PlatformControllerQnx::getSystemUITheme()
{
    qDebug("[PlatformControllerQnx] getSystemUITheme");
    if(isLPMMode())
        return UI_THEME_INVALID;

    return (UI_THEME)m_SystemSetting->getSystemUITheme();
}

UI_THEME PlatformControllerQnx::getNaviMapUITheme()
{
    qDebug("[PlatformControllerQnx] getNaviMapUITheme");
    if(isLPMMode())
        return UI_THEME_INVALID;

    return (UI_THEME)m_SystemSetting->getNaviMapUITheme();
}
#endif

#ifdef PLATFORM_QNX // Navi Setting
void PlatformControllerQnx::getGeneralSettings(const int32_t nReqID)
{
    qDebug("[PlatformControllerQnx] getGeneralSettings");

    emit PlatformGetGeneralSettings(nReqID);
}

void PlatformControllerQnx::onGetGeneralSettings(int nReqID, const NAVI_GENERAL_SETTINGS& settings, const NAVI_GENERAL_DISABLED_SETTINGS& disabledsettings)
{
    qDebug("[PlatformControllerQnx] onGetGeneralSettings");

    NAVI_GENERAL_SETTINGS_T setting;
    NAVI_GENERAL_DISABLED_SETTINGS_T disabledsetting;

    setting.showQibla = (E_NAVI_ONOFF_STATUS)settings.showQibla; // default: NAVI_ONOFF_STATUS_OFF
    setting.addressFormat = (E_MAP_ADRESS_TYPE)settings.addressFormat; // default: MAP_ADRESS_TYPE_ROADNAME
    setting.muteGuidance = (E_NAVI_ONOFF_STATUS)settings.muteGuidance;   // default: NAVI_ONOFF_STATUS_OFF
    setting.notificationVisualCueSafetyZone = (E_NAVI_ONOFF_STATUS)settings.notificationVisualCueSafetyZone; // default: NAVI_ONOFF_STATUS_ON
    setting.notificationPlaySoundSafetyZone = (E_NAVI_ONOFF_STATUS)settings.notificationPlaySoundSafetyZone; // default: NAVI_ONOFF_STATUS_ON
    setting.notificationVisualCueSpeedCamera = (E_NAVI_ONOFF_STATUS)settings.notificationVisualCueSpeedCamera;   // default: NAVI_ONOFF_STATUS_ON
    setting.notificationPlaySoundSpeedCamera = (E_NAVI_ONOFF_STATUS)settings.notificationPlaySoundSpeedCamera;   // default: NAVI_ONOFF_STATUS_ON
    setting.announcementDistance = (E_NAVI_GENERAL_ANNOUNCEMENT_DISTANCE)settings.announcementDistance;   // default: NAVI_ONOFF_STATUS_ON
    setting.notificationLowFuel = (E_NAVI_ONOFF_STATUS)settings.notificationLowFuel; // default: NAVI_ONOFF_STATUS_ON
    setting.editNumberPlate = settings.editNumberPlate;
    setting.textSize = (E_MAP_TEXT_SIZE)settings.textSize;   // default: MAP_TEXT_SIZE_MEDIUM_NORMAL

    if(settings.alertTypeList.items.size() > 0)
    {
        setting.alertTypeList.items.reserve(settings.alertTypeList.items.size());

        for (std::vector<NAVI_GENERAL_ALERT_TYPE_ITEM>::const_iterator it = settings.alertTypeList.items.begin(); it!=settings.alertTypeList.items.end(); ++it)
        {
            NAVI_GENERAL_ALERT_TYPE_ITEM item = *it;
            NAVI_GENERAL_ALERT_TYPE_ITEM_T stAlertItem;
            stAlertItem.id = item.id;
            stAlertItem.caption = item.caption;
            stAlertItem.checked = item.checked;

            setting.alertTypeList.items.push_back(stAlertItem);
        }
    }

    if(settings.warningTypeList.items.size() > 0)
    {
        setting.warningTypeList.items.reserve(settings.warningTypeList.items.size());

        for (std::vector<NAVI_GENERAL_WARNING_TYPE_ITEM>::const_iterator it = settings.warningTypeList.items.begin(); it!=settings.warningTypeList.items.end(); ++it)
        {
            NAVI_GENERAL_WARNING_TYPE_ITEM item = *it;
            NAVI_GENERAL_WARNING_TYPE_ITEM_T stWarningItem;
            stWarningItem.id = item.id;
            stWarningItem.caption = item.caption;
            stWarningItem.checked = item.checked;

            setting.warningTypeList.items.push_back(stWarningItem);
        }
    }

    if(disabledsettings.disabledSettings.size() > 0)
    {
        disabledsetting.disabledSettings.reserve(disabledsettings.disabledSettings.size());

        for (std::vector<SUPPORTED_SETTING_ENUM>::const_iterator it = disabledsettings.disabledSettings.begin(); it!=disabledsettings.disabledSettings.end(); ++it)
        {
            disabledsetting.disabledSettings.push_back((NAVI_SUPPORTED_SETTING_ENUM)*it);
        }
    }

    m_pNaviDeploy->onGetGeneralSettings(nReqID, setting, disabledsetting);
}

void PlatformControllerQnx::setGeneralShowQibla(const int32_t nReqID, const E_NAVI_ONOFF_STATUS& status)
{
    qDebug("[PlatformControllerQnx] setGeneralShowQibla status=%d", status);

    //emit PlatformSetGeneralShowQibla(nReqID, (NAVI_ONOFF_STATUS)status);
}

void PlatformControllerQnx::onRespondGeneralShowQibla(const int32_t nReqID, const NAVI_ONOFF_STATUS &status)
{
    qDebug("[PlatformControllerQnx] onRespondGeneralShowQibla status=%d", status);

    //m_pNaviDeploy->onRespondGeneralShowQibla(nReqID, (E_NAVI_ONOFF_STATUS)status, NAVI_SETTINGS_RESPOND_SET);
}

void PlatformControllerQnx::setGeneralAddressFormat(const int32_t nReqID, const E_MAP_ADRESS_TYPE& type)
{
    qDebug("[PlatformControllerQnx] setGeneralAddressFormat type=%d", type);

    emit PlatformSetGeneralAddressFormat(nReqID, (MAP_ADRESS_TYPE)type);
}

void PlatformControllerQnx::onRespondGeneralAddressFormat(int nReqID, const MAP_ADRESS_TYPE& type)
{
    qDebug("[PlatformControllerQnx] onRespondGeneralAddressFormat type=%d", type);

    if(isLPMMode())
        return;

    m_pNaviDeploy->onRespondGeneralAddressFormat(nReqID, (E_MAP_ADRESS_TYPE)type, NAVI_SETTINGS_RESPOND_SET);
}

void PlatformControllerQnx::setGeneralMuteGuidance(const int32_t nReqID, const E_NAVI_ONOFF_STATUS& mode)
{
    qDebug("[PlatformControllerQnx] setGeneralMuteGuidance mode=%d", mode);

    emit PlatformSetGeneralMuteGuidance(nReqID, (NAVI_ONOFF_STATUS)mode);
}

void PlatformControllerQnx::onRespondGeneralMuteGuidance(int nReqID, const NAVI_ONOFF_STATUS& mode, const NAVI_SETTINGS_RES_ENUM respond)
{
    qDebug("[PlatformControllerQnx] onRespondGeneralMuteGuidance mode=%d", mode);

    if(isLPMMode())
        return;

    m_pNaviDeploy->onRespondGeneralMuteGuidance(nReqID, (E_NAVI_ONOFF_STATUS)mode, (NAVI_SETTINGS_RESPOND_ENUM)respond);
}

void PlatformControllerQnx::setGeneralNotificationVisualCueSafetyZone(const int32_t nReqID, const E_NAVI_ONOFF_STATUS& status)
{
    qDebug("[PlatformControllerQnx] setGeneralNotificationVisualCueSafetyZone status=%d", status);

    emit PlatformSetGeneralNotificationVisualCueSafetyZone(nReqID, (NAVI_ONOFF_STATUS)status);
}

void PlatformControllerQnx::onRespondGeneralNotificationVisualCueSafetyZone(int nReqID, const NAVI_ONOFF_STATUS& status)
{
    qDebug("[PlatformControllerQnx] onRespondGeneralNotificationVisualCueSafetyZone status=%d", status);

    if(isLPMMode())
        return;

    m_pNaviDeploy->onRespondGeneralNotificationVisualCueSafetyZone(nReqID, (E_NAVI_ONOFF_STATUS)status, NAVI_SETTINGS_RESPOND_SET);
}

void PlatformControllerQnx::setGeneralNotificationPlaySoundSafetyZone(const int32_t nReqID, const E_NAVI_ONOFF_STATUS& status)
{
    qDebug("[PlatformControllerQnx] setGeneralNotificationPlaySoundSafetyZone status=%d", status);

    emit PlatformSetGeneralNotificationPlaySoundSafetyZone(nReqID, (NAVI_ONOFF_STATUS)status);
}

void PlatformControllerQnx::setGeneralAlertTypeList(const int32_t nReqID, const NAVI_GENERAL_ALERT_TYPE_LIST_T& list)
{
    qDebug("[PlatformControllerQnx] setGeneralAlertTypeList");

    NAVI_GENERAL_ALERT_TYPE_LIST alertList;
    if(list.items.size() > 0)
    {
        alertList.items.reserve(list.items.size());

        for (std::vector<NAVI_GENERAL_ALERT_TYPE_ITEM_T>::const_iterator it = list.items.begin(); it!=list.items.end(); ++it)
        {
            NAVI_GENERAL_ALERT_TYPE_ITEM_T item = *it;
            NAVI_GENERAL_ALERT_TYPE_ITEM stAlertItem;
            stAlertItem.id = item.id;
            stAlertItem.caption = item.caption;
            stAlertItem.checked = item.checked;

            alertList.items.push_back(stAlertItem);
        }
    }

    emit PlatformSetGeneralAlertTypeList(nReqID, alertList);
}

void PlatformControllerQnx::onRespondGeneralAlertTypeList(int nReqID, NAVI_GENERAL_ALERT_TYPE_LIST& list, NAVI_SETTINGS_RES_ENUM respond)
{
    qDebug("[PlatformControllerQnx] onRespondGeneralAlertTypeList");
    if(isLPMMode())
        return;

    NAVI_GENERAL_ALERT_TYPE_LIST_T alertList;
    if(list.items.size() > 0)
    {
        alertList.items.reserve(list.items.size());

        for (std::vector<NAVI_GENERAL_ALERT_TYPE_ITEM>::const_iterator it = list.items.begin(); it!=list.items.end(); ++it)
        {
            NAVI_GENERAL_ALERT_TYPE_ITEM item = *it;
            NAVI_GENERAL_ALERT_TYPE_ITEM_T stAlertItem;
            stAlertItem.id = item.id;
            stAlertItem.caption = item.caption;
            stAlertItem.checked = item.checked;

            alertList.items.push_back(stAlertItem);
        }
    }


    m_pNaviDeploy->onRespondGeneralAlertTypeList(nReqID, alertList, (NAVI_SETTINGS_RESPOND_ENUM)respond);
}

void PlatformControllerQnx::setGeneralWarningTypeList(const int32_t nReqID, const NAVI_GENERAL_WARNING_TYPE_LIST_T& list)
{
    qDebug("[PlatformControllerQnx] setGeneralWarningTypeList");

    NAVI_GENERAL_WARNING_TYPE_LIST warningList;
    if(list.items.size() > 0)
    {
        warningList.items.reserve(list.items.size());

        for (std::vector<NAVI_GENERAL_WARNING_TYPE_ITEM_T>::const_iterator it = list.items.begin(); it!=list.items.end(); ++it)
        {
            NAVI_GENERAL_WARNING_TYPE_ITEM_T item = *it;
            NAVI_GENERAL_WARNING_TYPE_ITEM stWarningItem;
            stWarningItem.id = item.id;
            stWarningItem.caption = item.caption;
            stWarningItem.checked = item.checked;

            warningList.items.push_back(stWarningItem);
        }
    }

    emit PlatformSetGeneralWarningTypeList(nReqID, warningList);
}

void PlatformControllerQnx::onRespondGeneralWarningTypeList(int nReqID, NAVI_GENERAL_WARNING_TYPE_LIST& list, NAVI_SETTINGS_RES_ENUM respond)
{
    qDebug("[PlatformControllerQnx] onRespondGeneralWarningTypeList");
    if(isLPMMode())
        return;

    NAVI_GENERAL_WARNING_TYPE_LIST_T warningList;
    if(list.items.size() > 0)
    {
        warningList.items.reserve(list.items.size());

        for (std::vector<NAVI_POI_ITEM>::const_iterator it = list.items.begin(); it!=list.items.end(); ++it)
        {
            NAVI_GENERAL_WARNING_TYPE_ITEM item = *it;
            NAVI_GENERAL_WARNING_TYPE_ITEM_T stWarningItem;
            stWarningItem.id = item.id;
            stWarningItem.caption = item.caption;
            stWarningItem.checked = item.checked;

            warningList.items.push_back(stWarningItem);
        }
    }


    m_pNaviDeploy->onRespondGeneralWarningTypeList(nReqID, warningList, (NAVI_SETTINGS_RESPOND_ENUM)respond);
}

void PlatformControllerQnx::onRespondGeneralNotificationPlaySoundSafetyZone(int nReqID, const NAVI_ONOFF_STATUS& status)
{
    qDebug("[PlatformControllerQnx] onRespondGeneralNotificationPlaySoundSafetyZone status=%d", status);

    if(isLPMMode())
        return;

    m_pNaviDeploy->onRespondGeneralNotificationPlaySoundSafetyZone(nReqID, (E_NAVI_ONOFF_STATUS)status, NAVI_SETTINGS_RESPOND_SET);
}

void PlatformControllerQnx::setGeneralNotificationVisualCueSpeedCamera(const int32_t nReqID, const E_NAVI_ONOFF_STATUS& status)
{
    qDebug("[PlatformControllerQnx] setGeneralNotificationVisualCueSpeedCamera status=%d", status);

    emit PlatformSetGeneralNotificationVisualCueSpeedCamera(nReqID, (NAVI_ONOFF_STATUS)status);
}

void PlatformControllerQnx::onRespondGeneralNotificationVisualCueSpeedCamera(int nReqID, const NAVI_ONOFF_STATUS& status)
{
    qDebug("[PlatformControllerQnx] onRespondGeneralNotificationVisualCueSpeedCamera status=%d", status);

    if(isLPMMode())
        return;

    m_pNaviDeploy->onRespondGeneralNotificationVisualCueSpeedCamera(nReqID, (E_NAVI_ONOFF_STATUS)status, NAVI_SETTINGS_RESPOND_SET);
}

void PlatformControllerQnx::setGeneralNotificationPlaySoundSpeedCamera(const int32_t nReqID, const E_NAVI_ONOFF_STATUS& status)
{
    qDebug("[PlatformControllerQnx] setGeneralNotificationPlaySoundSpeedCamera status=%d", status);

    emit PlatformSetGeneralNotificationPlaySoundSpeedCamera(nReqID, (NAVI_ONOFF_STATUS)status);
}

void PlatformControllerQnx::onRespondGeneralNotificationPlaySoundSpeedCamera(int nReqID, const NAVI_ONOFF_STATUS& status)
{
    qDebug("[PlatformControllerQnx] onRespondGeneralNotificationPlaySoundSpeedCamera status=%d", status);

    if(isLPMMode())
        return;

    m_pNaviDeploy->onRespondGeneralNotificationPlaySoundSpeedCamera(nReqID, (E_NAVI_ONOFF_STATUS)status, NAVI_SETTINGS_RESPOND_SET);
}

void PlatformControllerQnx::setGeneralNotificationLowFuel(const int32_t nReqID, const E_NAVI_ONOFF_STATUS& status)
{
    qDebug("[PlatformControllerQnx] setGeneralNotificationLowFuel status=%d", status);

    emit PlatformSetGeneralNotificationLowFuel(nReqID, (NAVI_ONOFF_STATUS)status);
}

void PlatformControllerQnx::onRespondGeneralNotificationLowFuel(int nReqID, const NAVI_ONOFF_STATUS& status)
{
    qDebug("[PlatformControllerQnx] onRespondGeneralNotificationLowFuel status=%d", status);

    if(isLPMMode())
        return;

    m_pNaviDeploy->onRespondGeneralNotificationLowFuel(nReqID, (E_NAVI_ONOFF_STATUS)status, NAVI_SETTINGS_RESPOND_SET);
}

void PlatformControllerQnx::setGeneralEditNumberPlate(const int32_t nReqID, const std::string& numberPlate)
{
    qDebug("[PlatformControllerQnx] setGeneralEditNumberPlate numberPlate=%s", numberPlate);

    emit PlatformSetGeneralEditNumberPlate(nReqID, numberPlate);
}

void PlatformControllerQnx::onRespondGeneralEditNumberPlate(int nReqID, const std::string& numberPlate)
{
    qDebug("[jPlatformControllerQnx] onRespondGeneralEditNumberPlate numberPlate=%s", numberPlate);

    if(isLPMMode())
        return;

    m_pNaviDeploy->onRespondGeneralEditNumberPlate(nReqID, numberPlate, NAVI_SETTINGS_RESPOND_SET);
}

void PlatformControllerQnx::setGeneralTextSize(const int32_t nReqID, const E_MAP_TEXT_SIZE& size)
{
    qDebug("[PlatformControllerQnx] setGeneralTextSize size=%d", size);
    kpiBeginMeasurement(KPI_SETTINGS_UPDATE);
    emit PlatformSetGeneralTextSize(nReqID, (MAP_TEXT_SIZE)size);
}

void PlatformControllerQnx::onRespondGeneralTextSize(int nReqID, const MAP_TEXT_SIZE& size)
{
    qDebug("[PlatformControllerQnx] onRespondGeneralTextSize size=%d", size);

    kpiFinishMeasurement(KPI_SETTINGS_UPDATE,"Tmap Settings update");

    if(isLPMMode())
        return;

    m_pNaviDeploy->onRespondGeneralTextSize(nReqID, (E_MAP_TEXT_SIZE)size, NAVI_SETTINGS_RESPOND_SET);
}

void PlatformControllerQnx::getHybridSettings(const int32_t nReqID)
{
    qDebug("[PlatformControllerQnx] getHybridSettings");
}

void PlatformControllerQnx::onGetHybridSettings(int nReqID, NAVI_HYBRID_SETTINGS& settings, NAVI_HYBRID_DISABLED_SETTINGS& disabledSettings)
{
    qDebug("[PlatformControllerQnx] onGetHybridSettings");

    //m_pNaviDeploy->onRespondHybridChargeSpeed();
}

void PlatformControllerQnx::setHybridShowChargePoint(const int32_t nReqID, const E_NAVI_ONOFF_STATUS& status)
{
    qDebug("[PlatformControllerQnx] setHybridShowChargePoint status=%d", status);
}

void PlatformControllerQnx::setHybridShowAvailableChargeStation(const int32_t nReqID, const E_NAVI_ONOFF_STATUS& status)
{
    qDebug("[PlatformControllerQnx] setHybridShowAvailableChargeStation status=%d", status);
}
void PlatformControllerQnx::onRespondHybridShowAvailableChargeStation(int nReqID, NAVI_ONOFF_STATUS status, NAVI_SETTINGS_RES_ENUM respond)
{
    qDebug("[PlatformControllerQnx] onRespondHybridShowAvailableChargeStation");
}

void PlatformControllerQnx::setHybridShowOpenChargeStation(const int32_t nReqID, const E_NAVI_ONOFF_STATUS& status)
{
    qDebug("[PlatformControllerQnx] setHybridShowOpenChargeStation");
}
void PlatformControllerQnx::onRespondHybridShowOpenChargeStation(int nReqID, NAVI_ONOFF_STATUS status, NAVI_SETTINGS_RES_ENUM respond)
{
    qDebug("[PlatformControllerQnx] onRespondHybridShowOpenChargeStation");
}

void PlatformControllerQnx::setHybridConnectorTypeList(const int32_t nReqID, const NAVI_HYBRID_CONNECTOR_TYPE_LIST_T& list)
{
    qDebug("[PlatformControllerQnx] setHybridConnectorTypeList");
}
void PlatformControllerQnx::onRespondHybridConnectorTypeList(int nReqID, NAVI_HYBRID_CONNECTOR_TYPE_LIST& list, NAVI_SETTINGS_RES_ENUM respond)
{
    qDebug("[PlatformControllerQnx] onRespondHybridConnectorTypeList");
}

void PlatformControllerQnx::setHybridChargeSpeed(const int32_t nReqID, const NAVI_HYBRID_CHARGE_SPEED_LIST_T& list)
{
    qDebug("[PlatformControllerQnx] setHybridChargeSpeed");
}
void PlatformControllerQnx::onRespondHybridChargeSpeed(int nReqID, NAVI_HYBRID_CHARGE_SPEED_LIST& list, NAVI_SETTINGS_RES_ENUM respond)
{
    qDebug("[PlatformControllerQnx] onRespondHybridChargeSpeed");
}
void PlatformControllerQnx::setHybridProvider(const int32_t nReqID, const NAVI_HYBRID_PROVIDER_LIST_T& list)
{
    qDebug("[PlatformControllerQnx] setHybridProvider");
}

void PlatformControllerQnx::setHybridPaymentTypeList(const int32_t nReqID, const NAVI_HYBRID_PAYMENT_TYPE_LIST_T& list)
{
    qDebug("[PlatformControllerQnx] setHybridPaymentTypeList");
}

void PlatformControllerQnx::getMapSettings(const int32_t nReqID)
{
    qDebug("[PlatformControllerQnx] getMapSettings");

    emit PlatformGetMapSettings(nReqID);
}

void PlatformControllerQnx::onGetMapSettings(int nReqID, const NAVI_MAP_SETTINGS& settings, const NAVI_MAP_DISABLED_SETTINGS& disabledsettings)
{
    qDebug("[PlatformControllerQnx] onGetMapSettings");

    if(isLPMMode())
        return;

    NAVI_MAP_SETTINGS_T setting;
    NAVI_MAP_DISABLED_SETTINGS_T disabledsetting;

    setting.satellite = (E_NAVI_ONOFF_STATUS)settings.satellite;
    setting.trafficOnMap = (E_NAVI_ONOFF_STATUS)settings.trafficOnMap;
    //setting.trafficOnTargetRoad = (E_TRAFFIC_ROAD)settings.trafficOnTargetRoad;
    if(settings.trafficOnTargetRoad.items.size() > 0)
    {
        setting.trafficOnTargetRoad.items.reserve(settings.trafficOnTargetRoad.items.size());

        for (std::vector<NAVI_MAP_TRAFFIC_ROAD_ITEM>::const_iterator it = settings.trafficOnTargetRoad.items.begin(); it!=settings.trafficOnTargetRoad.items.end(); ++it)
        {
            NAVI_MAP_TRAFFIC_ROAD_ITEM item = *it;
            NAVI_MAP_TRAFFIC_ROAD_ITEM_T stPoiItem;
            stPoiItem.id = item.id;
            stPoiItem.caption = item.caption;
            stPoiItem.checked = item.checked;

            setting.trafficOnTargetRoad.items.push_back(stPoiItem);
        }
    }

    //setting.trafficFlow = (E_TRAFFIC_FLOW)settings.trafficFlow;
    if(settings.trafficFlow.items.size() > 0)
    {
        setting.trafficFlow.items.reserve(settings.trafficFlow.items.size());

        for (std::vector<NAVI_MAP_TRAFFIC_ROAD_ITEM>::const_iterator it = settings.trafficFlow.items.begin(); it!=settings.trafficFlow.items.end(); ++it)
        {
            NAVI_MAP_TRAFFIC_FLOW_ITEM item = *it;
            NAVI_MAP_TRAFFIC_FLOW_ITEM_T stPoiItem;
            stPoiItem.id = item.id;
            stPoiItem.caption = item.caption;
            stPoiItem.checked = item.checked;

            setting.trafficFlow.items.push_back(stPoiItem);
        }
    }
    setting.trafficAccident = (E_NAVI_ONOFF_STATUS)settings.trafficAccident;
    setting.trafficParking = (E_NAVI_ONOFF_STATUS)settings.trafficParking;
    setting.trafficSAPA = (E_NAVI_ONOFF_STATUS)settings.trafficSAPA;
    setting.recentDestination = (E_NAVI_ONOFF_STATUS)settings.recentDestination;
    setting.favourites = (E_NAVI_ONOFF_STATUS)settings.favourites;
    setting.autoZoom = (E_NAVI_ONOFF_STATUS)settings.autoZoom;
    setting.skybox = (E_NAVI_ONOFF_STATUS)settings.skybox;
    setting.highwayMode = (E_NAVI_ONOFF_STATUS)settings.highwayMode;
    setting.viewMode = (E_MAP_VIEW_MODE)settings.viewMode;
    setting.dynamicView = (E_MAP_DYNAMIC_VIEW_MODE)settings.dynamicView;
    setting.threeDBuilding = (E_NAVI_ONOFF_STATUS)settings.threeDBuilding;
    setting.POIDisplyPreference = (E_NAVI_ONOFF_STATUS)settings.POIDisplyPreference;
    setting.displaySavedPOIIcon = (E_NAVI_ONOFF_STATUS)settings.displaySavedPOIIcon;
    if(settings.POIDisplayOnMapList.items.size() > 0)
    {
        setting.POIDisplayOnMapList.items.reserve(settings.POIDisplayOnMapList.items.size());

        for (std::vector<NAVI_POI_ITEM>::const_iterator it = settings.POIDisplayOnMapList.items.begin(); it!=settings.POIDisplayOnMapList.items.end(); ++it)
        {
            NAVI_POI_ITEM item = *it;
            NAVI_POI_ITEM_T stPoiItem;
            stPoiItem.id = item.id;
            stPoiItem.caption = item.caption;
            stPoiItem.checked = item.checked;

            setting.POIDisplayOnMapList.items.push_back(stPoiItem);
        }
    }

    if(disabledsettings.disabledSettings.size() > 0)
    {
        disabledsetting.disabledSettings.reserve(disabledsettings.disabledSettings.size());

        for (std::vector<SUPPORTED_SETTING_ENUM>::const_iterator it = disabledsettings.disabledSettings.begin(); it!=disabledsettings.disabledSettings.end(); ++it)
        {
            disabledsetting.disabledSettings.push_back((NAVI_SUPPORTED_SETTING_ENUM)*it);
        }
    }

    m_pNaviDeploy->onGetMapSettings(nReqID, setting, disabledsetting);
}

void PlatformControllerQnx::setMapTrafficOnTargetRoad(const int32_t nReqID, const NAVI_MAP_TRAFFIC_ROAD_LIST_T& roadList)
{
    qDebug("[PlatformControllerQnx] setMapTrafficOnTargetRoad");

    NAVI_MAP_TRAFFIC_ROAD_LIST road = {};
    if(roadList.items.size() > 0)
    {
        road.items.reserve(roadList.items.size());

        for (std::vector<NAVI_MAP_TRAFFIC_ROAD_ITEM_T>::const_iterator it = roadList.items.begin(); it!=roadList.items.end(); ++it)
        {
            NAVI_MAP_TRAFFIC_ROAD_ITEM_T item = *it;
            NAVI_MAP_TRAFFIC_ROAD_ITEM stPoiItem;
            stPoiItem.id = item.id;
            stPoiItem.caption = item.caption;
            stPoiItem.checked = item.checked;

            road.items.push_back(stPoiItem);
        }
    }

    emit PlatformSetMapTrafficOnTargetRoad(nReqID, road);
}

void PlatformControllerQnx::onRespondMapTrafficOnTargetRoad(int nReqID, NAVI_MAP_TRAFFIC_ROAD_LIST& road, NAVI_SETTINGS_RES_ENUM respond)
{
    if(isLPMMode())
        return;

    NAVI_MAP_TRAFFIC_ROAD_LIST_T roadList = {};
    if(road.items.size() > 0)
    {
        roadList.items.reserve(road.items.size());

        for (std::vector<NAVI_MAP_TRAFFIC_ROAD_ITEM>::iterator it = road.items.begin(); it!=road.items.end(); ++it)
        {
            NAVI_MAP_TRAFFIC_ROAD_ITEM item = *it;
            NAVI_MAP_TRAFFIC_ROAD_ITEM_T stPoiItem;
            stPoiItem.id = item.id;
            stPoiItem.caption = item.caption;
            stPoiItem.checked = item.checked;

            roadList.items.push_back(stPoiItem);
        }
    }

    m_pNaviDeploy->onRespondMapTrafficOnTargetRoad(nReqID, roadList, (NAVI_SETTINGS_RESPOND_ENUM)respond);
}

void PlatformControllerQnx::setMapRecentDestination(const int32_t nReqID, const E_NAVI_ONOFF_STATUS& status)
{
    qDebug("[PlatformControllerQnx] setMapRecentDestination status=%d", status);

    emit PlatformSetMapRecentDestination(nReqID, (NAVI_ONOFF_STATUS)status);
}

void PlatformControllerQnx::onRespondMapRecentDestination(int nReqID, NAVI_ONOFF_STATUS status, NAVI_SETTINGS_RES_ENUM respond)
{
    if(isLPMMode())
        return;

    m_pNaviDeploy->onRespondMapRecentDestination(nReqID, (E_NAVI_ONOFF_STATUS)status, (NAVI_SETTINGS_RESPOND_ENUM)respond);
}

void PlatformControllerQnx::setMapFavourites(const int32_t nReqID, const E_NAVI_ONOFF_STATUS& status)
{
    qDebug("[PlatformControllerQnx] setMapFavourites status=%d", status);

    emit PlatformSetMapFavourites(nReqID, (NAVI_ONOFF_STATUS)status);
}

void PlatformControllerQnx::onRespondMapFavourites(int nReqID, NAVI_ONOFF_STATUS status, NAVI_SETTINGS_RES_ENUM respond)
{
    if(isLPMMode())
        return;

    m_pNaviDeploy->onRespondMapFavourites(nReqID, (E_NAVI_ONOFF_STATUS)status, (NAVI_SETTINGS_RESPOND_ENUM)respond);
}

void PlatformControllerQnx::setMapPOIDisplayOnMapList(const int32_t nReqID, const NAVI_POI_LIST_T& list)
{
    qDebug("[PlatformControllerQnx] setMapPOIDisplayOnMapList");

    NAVI_POI_LIST poiList;
    if(list.items.size() > 0)
    {
        poiList.items.reserve(list.items.size());

        for (std::vector<NAVI_POI_ITEM_T>::const_iterator it = list.items.begin(); it!=list.items.end(); ++it)
        {
            NAVI_POI_ITEM_T item = *it;
            NAVI_POI_ITEM stPoiItem;
            stPoiItem.id = item.id;
            stPoiItem.caption = item.caption;
            stPoiItem.checked = item.checked;

            poiList.items.push_back(stPoiItem);
        }
    }

    poiList.maxSelectableItems = list.maxSelectableItems;

    emit PlatformSetMapPOIDisplayOnMapList(nReqID, poiList);
}

void PlatformControllerQnx::onRespondMapPOIDisplayOnMapList(int nReqID, NAVI_POI_LIST& list, NAVI_SETTINGS_RES_ENUM respond)
{
    if(isLPMMode())
        return;

    NAVI_POI_LIST_T poiList;
    if(list.items.size() > 0)
    {
        poiList.items.reserve(list.items.size());

        for (std::vector<NAVI_POI_ITEM>::iterator it = list.items.begin(); it!=list.items.end(); ++it)
        {
            NAVI_POI_ITEM item = *it;
            NAVI_POI_ITEM_T stPoiItem;
            stPoiItem.id = item.id;
            stPoiItem.caption = item.caption;
            stPoiItem.checked = item.checked;

            poiList.items.push_back(stPoiItem);
        }
    }

    poiList.maxSelectableItems = list.maxSelectableItems;

    m_pNaviDeploy->onRespondMapPOIDisplayOnMapList(nReqID, poiList, (NAVI_SETTINGS_RESPOND_ENUM)respond);
}

void PlatformControllerQnx::getRGSettings(const int32_t nReqID)
{
    qDebug("[PlatformControllerQnx] getRGSettings");

    emit PlatformGetRGSettings(nReqID);
}
void PlatformControllerQnx::onGetRGSettings(int nReqID, NAVI_RG_SETTINGS& settings, const NAVI_RG_DISABLED_SETTINGS& disabledsettings)
{
    qDebug("[PlatformControllerQnx] onGetRGSettings");

    if(isLPMMode())
        return;

    NAVI_RG_SETTINGS_T stSettings={};
    NAVI_RG_DISABLED_SETTINGS_T disabledsetting;

    stSettings.dynamicRouting = (E_NAVI_ONOFF_STATUS)settings.dynamicRouting;
    stSettings.routingMode = (E_ROUTE_ROUTING_MODE)settings.routingMode;
   
    if(settings.routePreferenceList.items.size() > 0)
    {
        stSettings.routePreferenceList.items.reserve(settings.routePreferenceList.items.size());

        for (std::vector<NAVI_ROUTE_PREFERENCE_ITEM>::iterator it = settings.routePreferenceList.items.begin(); it!=settings.routePreferenceList.items.end(); ++it)
        {
            NAVI_ROUTE_PREFERENCE_ITEM item = *it;
            NAVI_ROUTE_PREFERENCE_ITEM_T stPoiItem;
            stPoiItem.id = item.id;
            stPoiItem.caption = item.caption;
            stPoiItem.checked = item.checked;

            stSettings.routePreferenceList.items.push_back(stPoiItem);
        }
    }

    stSettings.commuteLearnRoute = (E_NAVI_ONOFF_STATUS)settings.commuteLearnRoute;
    stSettings.commuteAutoStart = (E_NAVI_ONOFF_STATUS)settings.commuteAutoStart;
    stSettings.commuteFewerCommand = (E_NAVI_ONOFF_STATUS)settings.commuteFewerCommand;

    if(settings.suggestionAlongList.items.size() > 0)
    {
        stSettings.suggestionAlongList.items.reserve(settings.suggestionAlongList.items.size());

        for (std::vector<NAVI_ROUTE_SUGGESTION_ITEM>::iterator it = settings.suggestionAlongList.items.begin(); it!=settings.suggestionAlongList.items.end(); ++it)
        {
            NAVI_ROUTE_SUGGESTION_ITEM item = *it;
            NAVI_ROUTE_SUGGESTION_ITEM_T stPoiItem;
            stPoiItem.id = item.id;
            stPoiItem.caption = item.caption;
            stPoiItem.checked = item.checked;

            stSettings.suggestionAlongList.items.push_back(stPoiItem);
        }
    }

    stSettings.roadSignBoard = (E_NAVI_ONOFF_STATUS)settings.roadSignBoard;
    stSettings.railroadCrossing = (E_NAVI_ONOFF_STATUS)settings.railroadCrossing;
    stSettings.intersectionView = (E_NAVI_ONOFF_STATUS)settings.intersectionView;

    stSettings.avoidPlateNumRestrict = (E_NAVI_ONOFF_STATUS)settings.avoidPlateNumRestrict;
    stSettings.arrivalTimeMode = (E_GUIDE_ARRIVAL_TIME)settings.arrivalTimeMode;
    stSettings.arrivalTimeTo = (E_GUIDE_ESTIMATED_TIME)settings.arrivalTimeTo;

    if(disabledsettings.disabledSettings.size() > 0)
    {
        disabledsetting.disabledSettings.reserve(disabledsettings.disabledSettings.size());

        for (std::vector<SUPPORTED_SETTING_ENUM>::const_iterator it = disabledsettings.disabledSettings.begin(); it!=disabledsettings.disabledSettings.end(); ++it)
        {
            disabledsetting.disabledSettings.push_back((NAVI_SUPPORTED_SETTING_ENUM)*it);
        }
    }

    m_pNaviDeploy->onGetRGSettings(nReqID, stSettings, disabledsetting);
}

void PlatformControllerQnx::setRGDynamicRouting(const int32_t nReqID, const E_NAVI_ONOFF_STATUS& status)
{
    qDebug("[PlatformControllerQnx] setRGDynamicRouting status=%d", status);
}
void PlatformControllerQnx::onRespondRGDynamicRouting(int nReqID, NAVI_ONOFF_STATUS status, NAVI_SETTINGS_RES_ENUM respond)
{

}

void PlatformControllerQnx::setRGRouteAutoRecalculate(const int32_t nReqID, const E_NAVI_ONOFF_STATUS& status)
{
    qDebug("[PlatformControllerQnx] setRGRouteAutoRecalculate status=%d", status);

    emit PlatformSetRGRouteAutoRecalculate(nReqID, (NAVI_ONOFF_STATUS)status);
}
void PlatformControllerQnx::onRespondRGRouteAutoRecalculate(int nReqID, NAVI_ONOFF_STATUS status, NAVI_SETTINGS_RES_ENUM respond)
{
    qDebug("[PlatformControllerQnx] onRespondRGRouteAutoRecalculate status=%d, res=%d", status, respond);

    if(isLPMMode())
        return;

    m_pNaviDeploy->onRespondRGRouteAutoRecalculate(nReqID, (E_NAVI_ONOFF_STATUS)status, (NAVI_SETTINGS_RESPOND_ENUM)respond);
}

void PlatformControllerQnx::setRGRoutingMode(const int32_t nReqID, const E_ROUTE_ROUTING_MODE& mode)
{
    qDebug("[PlatformControllerQnx] setRGRoutingMode");
/*
    NAVI_ROUTE_ROUTING_MODE stList = {};
    stList.menuType = (NAVI_MENU_TYPE)list.menuType;

    if(list.routingModeCheckbox.items.size() > 0)
    {
        stList.routingModeCheckbox.items.reserve(list.routingModeCheckbox.items.size());

        for (std::vector<NAVI_ROUTE_ROUTING_MODE_ITEM_T>::const_iterator it = list.routingModeCheckbox.items.begin(); it!=list.routingModeCheckbox.items.end(); ++it)
        {
            NAVI_ROUTE_ROUTING_MODE_ITEM_T item = *it;
            NAVI_ROUTE_ROUTING_MODE_ITEM stListItem;
            stListItem.id = item.id;
            stListItem.caption = item.caption;
            stListItem.checked = item.checked;

            stList.routingModeCheckbox.items.push_back(stListItem);
        }
    }
*/
    emit PlatformSetRGRoutingMode(nReqID, (ROUTE_ROUTING_MODE)mode);
}
void PlatformControllerQnx::onRespondRGRoutingMode(int nReqID, ROUTE_ROUTING_MODE mode, NAVI_SETTINGS_RES_ENUM respond)
{
    qDebug("[PlatformControllerQnx] onRespondRGRoutingMode mode=%d, res=%d", mode, respond);

    if(isLPMMode())
        return;

    m_pNaviDeploy->onRespondRGRoutingMode(nReqID, (E_ROUTE_ROUTING_MODE)mode, (NAVI_SETTINGS_RESPOND_ENUM)respond);
}

void PlatformControllerQnx::setRGRoutePreferenceList(const int32_t nReqID, const NAVI_ROUTE_PREFERENCE_LIST_T& list)
{
    qDebug("[PlatformControllerQnx] setRGRoutePreferenceList");

    NAVI_ROUTE_PREFERENCE_LIST stList = {};

    if(list.items.size() > 0)
    {
        stList.items.reserve(list.items.size());

        for (std::vector<NAVI_ROUTE_PREFERENCE_ITEM_T>::const_iterator it = list.items.begin(); it!=list.items.end(); ++it)
        {
            NAVI_ROUTE_PREFERENCE_ITEM_T item = *it;
            NAVI_ROUTE_PREFERENCE_ITEM stListItem;
            stListItem.id = item.id;
            stListItem.caption = item.caption;
            stListItem.checked = item.checked;

            stList.items.push_back(stListItem);
        }
    }

    emit PlatformSetRGRoutePreferenceList(nReqID, stList);
}

void PlatformControllerQnx::onRespondRGRoutePreferenceList(int nReqID, NAVI_ROUTE_PREFERENCE_LIST& list, NAVI_SETTINGS_RES_ENUM respond)
{
    qDebug()<<"[PlatformControllerQnx] onRespondRGRoutePreferenceList respond"<<respond;

    if(isLPMMode())
        return;

    NAVI_ROUTE_PREFERENCE_LIST_T stList={};

    if(list.items.size() > 0)
    {
        list.items.reserve(stList.items.size());

        for (std::vector<NAVI_ROUTE_PREFERENCE_ITEM>::iterator it = list.items.begin(); it!=list.items.end(); ++it)
        {
            NAVI_ROUTE_PREFERENCE_ITEM item = *it;
            NAVI_ROUTE_PREFERENCE_ITEM_T stListItem;
            stListItem.id = item.id;
            stListItem.caption = item.caption;
            stListItem.checked = item.checked;

            stList.items.push_back(stListItem);
        }
    }

    m_pNaviDeploy->onRespondRGRoutePreferenceList(nReqID, stList, (NAVI_SETTINGS_RESPOND_ENUM)respond);
}

void PlatformControllerQnx::setRGCommuteLearnRoute(const int32_t nReqID, const E_NAVI_ONOFF_STATUS& status)
{
    qDebug("[PlatformControllerQnx] setRGCommuteLearnRoute status=%d", status);
}
void PlatformControllerQnx::onRespondRGCommuteLearnRoute(int nReqID, NAVI_ONOFF_STATUS status, NAVI_SETTINGS_RES_ENUM respond)
{

}

void PlatformControllerQnx::setRGCommuteAutoStart(const int32_t nReqID, const E_NAVI_ONOFF_STATUS& status)
{
    qDebug("[PlatformControllerQnx] setRGCommuteAutoStart status=%d", status);
}
void PlatformControllerQnx::onRespondRGCommuteAutoStart(int nReqID, NAVI_ONOFF_STATUS status, NAVI_SETTINGS_RES_ENUM respond)
{

}

void PlatformControllerQnx::setRGCommuteFewerCommand(const int32_t nReqID, const E_NAVI_ONOFF_STATUS& status)
{
    qDebug("[PlatformControllerQnx] setRGCommuteFewerCommand status=%d", status);
}
void PlatformControllerQnx::onRespondRGCommuteFewerCommand(int nReqID, NAVI_ONOFF_STATUS status, NAVI_SETTINGS_RES_ENUM respond)
{

}

void PlatformControllerQnx::requestRGDeleteDrivingHistory(const int32_t nReqID)
{
    qDebug("[PlatformControllerQnx] requestRGDeleteDrivingHistory");
}
void PlatformControllerQnx::onRespondRGDeleteDrivingHistory(int nReqID, NAVI_REQUEST_RESULT result, NAVI_SETTINGS_RES_ENUM respond)
{

}

void PlatformControllerQnx::setRGSuggestionAlongList(const int32_t nReqID, const NAVI_ROUTE_SUGGESTION_LIST_T& list)
{
    qDebug("[PlatformControllerQnx] setRGSuggestionAlongList");
}

void PlatformControllerQnx::onRespondRGSuggestionAlongList(int nReqID, NAVI_ROUTE_SUGGESTION_LIST& list, NAVI_SETTINGS_RES_ENUM respond)
{

}

void PlatformControllerQnx::setRGRoadSignBoard(const int32_t nReqID, const E_NAVI_ONOFF_STATUS& status)
{
    qDebug("[PlatformControllerQnx] setRGRoadSignBoard status=%d", status);
}
void PlatformControllerQnx::onRespondRGRoadSignBoard(int nReqID, NAVI_ONOFF_STATUS status, NAVI_SETTINGS_RES_ENUM respond)
{

}

void PlatformControllerQnx::setRGRailroadCrossing(const int32_t nReqID, const E_NAVI_ONOFF_STATUS& status)
{
    qDebug("[PlatformControllerQnx] setRGRailroadCrossing status=%d", status);
}
void PlatformControllerQnx::onRespondRGRailroadCrossing(int nReqID, NAVI_ONOFF_STATUS status, NAVI_SETTINGS_RES_ENUM respond)
{

}

void PlatformControllerQnx::setRGIntersectionView(const int32_t nReqID, const E_NAVI_ONOFF_STATUS& status)
{
    qDebug("[PlatformControllerQnx] setRGIntersectionView status=%d", status);
}
void PlatformControllerQnx::onRespondRGIntersectionView(int nReqID, NAVI_ONOFF_STATUS status, NAVI_SETTINGS_RES_ENUM respond)
{

}

void PlatformControllerQnx::setRGAvoidPlateNumRestrict(const int32_t nReqID, const E_NAVI_ONOFF_STATUS& status)
{
    qDebug("[PlatformControllerQnx] setRGAvoidPlateNumRestrict status=%d", status);
}
void PlatformControllerQnx::onRespondRGAvoidPlateNumRestrict(int nReqID, NAVI_ONOFF_STATUS status, NAVI_SETTINGS_RES_ENUM respond)
{

}

void PlatformControllerQnx::setRGArrivalTimeMode(const int32_t nReqID, const E_GUIDE_ARRIVAL_TIME& mode)
{
    qDebug("[PlatformControllerQnx] setRGArrivalTimeMode mode=%d", mode);
}
void PlatformControllerQnx::onRespondRGArrivalTimeMode(int nReqID, GUIDE_ARRIVAL_TIME mode, NAVI_SETTINGS_RES_ENUM respond)
{

}

void PlatformControllerQnx::setRGArrivalTimeTo(const int32_t nReqID, const E_GUIDE_ESTIMATED_TIME& mode)
{
    qDebug("[PlatformControllerQnx] setRGArrivalTimeTo mode=%d", mode);
}
void PlatformControllerQnx::onRespondRGArrivalTimeTo(int nReqID, GUIDE_ESTIMATED_TIME to, NAVI_SETTINGS_RES_ENUM respond)
{

}

void PlatformControllerQnx::getVICSSettings(const int32_t nReqID)
{
    qDebug("[PlatformControllerQnx] getVICSSettings");
}

void PlatformControllerQnx::setVICSTrafficStationChoice(const int32_t nReqID, const E_NAVI_AUTOMANUAL_STATUS& status)
{
    qDebug("[PlatformControllerQnx] setVICSTrafficStationChoice status=%d", status);
}

void PlatformControllerQnx::setVICSProbeInformation(const int32_t nReqID, const E_NAVI_ONOFF_STATUS& status)
{
    qDebug("[PlatformControllerQnx] setVICSProbeInformation status=%d", status);
}

void PlatformControllerQnx::setVICSETCSoundNotification(const int32_t nReqID, const E_NAVI_ONOFF_STATUS& status)
{
    qDebug("[PlatformControllerQnx] setVICSETCSoundNotification status=%d", status);
}

void PlatformControllerQnx::setVICSDSSSFromBeacon(const int32_t nReqID, const E_NAVI_ONOFF_STATUS& status)
{
    qDebug("[PlatformControllerQnx] setVICSDSSSFromBeacon status=%d", status);
}

void PlatformControllerQnx::getClusterSettings(const int32_t nReqID)
{
    qDebug("[PlatformControllerQnx] getClusterSettings");
}

void PlatformControllerQnx::setClusterJunctionViewPicture(const int32_t nReqID, const E_NAVI_ONOFF_STATUS& status)
{
    qDebug("[PlatformControllerQnx] setClusterJunctionViewPicture status=%d", status);
}

void PlatformControllerQnx::setClusterJunctionViewOnMain(const int32_t nReqID, const E_NAVI_ONOFF_STATUS& status)
{
    qDebug("[PlatformControllerQnx] setClusterJunctionViewOnMain status=%d", status);
}

void PlatformControllerQnx::setClusterLaneAssistancePicture(const int32_t nReqID, const E_NAVI_ONOFF_STATUS& status)
{
    qDebug("[PlatformControllerQnx] setClusterLaneAssistancePicture status=%d", status);
}

void PlatformControllerQnx::setClusterLaneAssistanceOnMain(const int32_t nReqID, const E_NAVI_ONOFF_STATUS& status)
{
    qDebug("[PlatformControllerQnx] setClusterLaneAssistanceOnMain status=%d", status);
}

void PlatformControllerQnx::setClusterAbstractRouteOverview(const int32_t nReqID, const E_NAVI_ONOFF_STATUS& status)
{
    qDebug("[PlatformControllerQnx] setClusterAbstractRouteOverview status=%d", status);
}

void PlatformControllerQnx::setClusterNextWaypoint(const int32_t nReqID, const E_NAVI_ONOFF_STATUS& status)
{
    qDebug("[PlatformControllerQnx] setClusterNextWaypoint status=%d", status);
}

void PlatformControllerQnx::setClusterRoutingInformation(const int32_t nReqID, const E_NAVI_ONOFF_STATUS& status)
{
    qDebug("[PlatformControllerQnx] setClusterRoutingInformation status=%d", status);
}

void PlatformControllerQnx::setClusterSatelite(const int32_t nReqID, const E_NAVI_ONOFF_STATUS& status)
{
    qDebug("[PlatformControllerQnx] setClusterSatelite status=%d", status);
}

void PlatformControllerQnx::setClusterMirrorInfoPanel(const int32_t nReqID, const E_NAVI_ONOFF_STATUS& status)
{
    qDebug("[PlatformControllerQnx] setClusterMirrorInfoPanel status=%d", status);
}

void PlatformControllerQnx::getVoiceSettings(const int32_t nReqID)
{
    qDebug("[PlatformControllerQnx] getVoiceSettings");

    emit PlatformGetVoiceSettings(nReqID);
}

void PlatformControllerQnx::onGetVoiceSettings(int nReqID, NAVI_VOICE_SETTINGS& settings, const NAVI_VOICE_DISABLED_SETTINGS& disabledsettings)
{
    qDebug("[PlatformControllerQnx] onGetVoiceSettings");

    if(isLPMMode())
        return;

    NAVI_VOICE_SETTINGS_T setting = {};
    NAVI_VOICE_DISABLED_SETTINGS_T disabledsetting;

    if(settings.announcementFrequencyList.items.size() > 0)
    {
        setting.announcementFrequencyList.items.reserve(settings.announcementFrequencyList.items.size());

        for (std::vector<GUIDE_VOICE_FREQUENCY>::iterator it = settings.announcementFrequencyList.items.begin(); it!=settings.announcementFrequencyList.items.end(); ++it)
        {
            GUIDE_VOICE_FREQUENCY item = *it;
            E_GUIDE_VOICE_FREQUENCY stListItem;
            stListItem = (E_GUIDE_VOICE_FREQUENCY)item;

            setting.announcementFrequencyList.items.push_back(stListItem);
        }
    }
    setting.announcementFrequencyList.selectedFrequency = (E_GUIDE_VOICE_FREQUENCY)settings.announcementFrequencyList.selectedFrequency;
    setting.naturalLanguageGuidance = (E_NAVI_ONOFF_STATUS)settings.naturalLanguageGuidance;
    setting.announcementBeep = (E_NAVI_ONOFF_STATUS)settings.announcementBeep;
    setting.reroutingAnnouncement = (E_NAVI_ONOFF_STATUS)settings.reroutingAnnouncement;

    if(disabledsettings.disabledSettings.size() > 0)
    {
        disabledsetting.disabledSettings.reserve(disabledsettings.disabledSettings.size());

        for (std::vector<SUPPORTED_SETTING_ENUM>::const_iterator it = disabledsettings.disabledSettings.begin(); it!=disabledsettings.disabledSettings.end(); ++it)
        {
            disabledsetting.disabledSettings.push_back((NAVI_SUPPORTED_SETTING_ENUM)*it);
        }
    }

    m_pNaviDeploy->onGetVoiceSettings(nReqID,setting, disabledsetting);
}

void PlatformControllerQnx::setVoiceAnnouncementFrequencyList(const int32_t nReqID, const E_GUIDE_VOICE_FREQUENCY& selectedFrequency)
{
    qDebug("[PlatformControllerQnx] setVoiceAnnouncementFrequencyList ");
}

void PlatformControllerQnx::setVoiceNaturalLanguageGuidance(const int32_t nReqID, const E_NAVI_ONOFF_STATUS& status)
{
    qDebug("[PlatformControllerQnx] setVoiceNaturalLanguageGuidance status=%d", status);
}

void PlatformControllerQnx::setGeneralAnnouncementDistance(const int32_t nReqID, const E_NAVI_GENERAL_ANNOUNCEMENT_DISTANCE& distance)
{
    qDebug("[PlatformControllerQnx] setGeneralAnnouncementDistance");

    emit PlatformSetGeneralAnnouncementDistance(nReqID, (NAVI_GENERAL_ANNOUNCEMENT_DISTANCE)distance);
}

void PlatformControllerQnx::onRespondGeneralAnnouncementDistance(int nReqID, NAVI_GENERAL_ANNOUNCEMENT_DISTANCE distance, NAVI_SETTINGS_RES_ENUM respond)
{
    qDebug("[PlatformControllerQnx] onRespondGeneralAnnouncementDistance");

    if(isLPMMode())
        return;

    m_pNaviDeploy->onRespondGeneralAnnouncementDistance(nReqID, (E_NAVI_GENERAL_ANNOUNCEMENT_DISTANCE)distance, (NAVI_SETTINGS_RESPOND_ENUM)respond);
}

void PlatformControllerQnx::setVoiceAnnouncementBeep(const int32_t nReqID, const E_NAVI_ONOFF_STATUS& status)
{
    qDebug("[PlatformControllerQnx] setVoiceAnnouncementBeep status=%d", status);
}

void PlatformControllerQnx::setVoiceReroutingAnnouncement(const int32_t nReqID, const E_NAVI_ONOFF_STATUS& status)
{
    qDebug("[PlatformControllerQnx] setVoiceReroutingAnnouncement status=%d", status);
}

void PlatformControllerQnx::getAboutSettings(const int32_t nReqID)
{
    qDebug("[PlatformControllerQnx] getAboutSettings");

    emit PlatformGetAboutSettings(nReqID);
}
void PlatformControllerQnx::onGetAboutSettings(int nReqID, NAVI_ABOUT_SETTINGS& settings, const NAVI_ABOUT_DISABLED_SETTINGS& disabledsettings)
{
    qDebug("[PlatformControllerQnx] onGetAboutSettings");

    if(isLPMMode())
        return;

    NAVI_ABOUT_SETTINGS_T setting={};
    NAVI_ABOUT_DISABLED_SETTINGS_T disabledsetting;
    //setting.vendorType = (E_NAVI_ABOUT_VENDOR_TYPE)settings.vendorType;
    setting.restAboutPage.productName.productName = settings.restAboutPage.productName.productName;
    setting.restAboutPage.appVersion.appVersion = settings.restAboutPage.appVersion.appVersion;
    setting.restAboutPage.GPSInfo.GPSInfo = settings.restAboutPage.GPSInfo.GPSInfo;
    setting.restAboutPage.attribution.attribution = settings.restAboutPage.attribution.attribution;
    setting.restAboutPage.legal.legal = settings.restAboutPage.legal.legal;
    setting.restAboutPage.termsOfService.termsOfService = settings.restAboutPage.termsOfService.termsOfService;
    setting.restAboutPage.privacyPolicy.privacyPolicy = settings.restAboutPage.privacyPolicy.privacyPolicy;
    setting.restAboutPage.privacySupplement.privacySupplement = settings.restAboutPage.privacySupplement.privacySupplement;
    setting.restAboutPage.OSSNotices.OSSNotices = settings.restAboutPage.OSSNotices.OSSNotices;
    setting.restAboutPage.mapInfo.status = settings.restAboutPage.mapInfo.status;
    setting.restAboutPage.mapInfo.version = settings.restAboutPage.mapInfo.version;

    if(disabledsettings.disabledSettings.size() > 0)
    {
        disabledsetting.disabledSettings.reserve(disabledsettings.disabledSettings.size());

        for (std::vector<SUPPORTED_SETTING_ENUM>::const_iterator it = disabledsettings.disabledSettings.begin(); it!=disabledsettings.disabledSettings.end(); ++it)
        {
            disabledsetting.disabledSettings.push_back((NAVI_SUPPORTED_SETTING_ENUM)*it);
        }
    }

    m_pNaviDeploy->onGetAboutSettings(nReqID, setting, disabledsetting);
}

void PlatformControllerQnx::setAboutGPSInfo(const int32_t nReqID, const NAVI_ABOUT_GPS_INFO_T& info)
{
    qDebug("[PlatformControllerQnx] setAboutGPSInfo");

    NAVI_ABOUT_GPS_INFO stInfo = {};

    stInfo.GPSInfo = info.GPSInfo;

    emit PlatformSetAboutGPSInfo(nReqID, stInfo);
}
void PlatformControllerQnx::onRespondAboutGPSInfo(int nReqID, NAVI_ABOUT_GPS_INFO& info, NAVI_SETTINGS_RES_ENUM respond)
{
    qDebug("[PlatformControllerQnx] onRespondAboutGPSInfo");

    if(isLPMMode())
        return;

    NAVI_ABOUT_GPS_INFO_T stInfo = {};

    stInfo.GPSInfo = info.GPSInfo;

    //m_pNaviDeploy->onRespondAboutGPSInfo(nReqID, stInfo, (NAVI_SETTINGS_RESPOND_ENUM)respond);
}

void PlatformControllerQnx::getAccountSettings(const int32_t nReqID)
{
    qDebug("[PlatformControllerQnx] getAccountSettings");

    emit PlatformGetAccountSettings(nReqID);
}
void PlatformControllerQnx::onGetAccountSettings(int nReqID, NAVI_ACCOUNT_SETTINGS& settings, const NAVI_ACCOUNT_DISABLED_SETTINGS& disabledsettings)
{
    qDebug("[PlatformControllerQnx] onGetAccountSettings");

    if(isLPMMode())
        return;

    NAVI_ACCOUNT_SETTINGS_T setting={};
    NAVI_ACCOUNT_DISABLED_SETTINGS_T disabledsetting;

    if(disabledsettings.disabledSettings.size() > 0)
    {
        disabledsetting.disabledSettings.reserve(disabledsettings.disabledSettings.size());

        for (std::vector<SUPPORTED_SETTING_ENUM>::const_iterator it = disabledsettings.disabledSettings.begin(); it!=disabledsettings.disabledSettings.end(); ++it)
        {
            disabledsetting.disabledSettings.push_back((NAVI_SUPPORTED_SETTING_ENUM)*it);
        }
    }

    m_pNaviDeploy->onGetAccountSettings(nReqID, setting, disabledsetting);
}

void PlatformControllerQnx::requestAccountClearUserPreferences(const int32_t nReqID)
{
    qDebug("[PlatformControllerQnx] requestAccountClearUserPreferences");

    emit PlatformRequestAccountClearUserPreferences(nReqID);
}
void PlatformControllerQnx::onRespondAccountClearUserPreferences(int nReqID, NAVI_REQUEST_RESULT result, NAVI_SETTINGS_RES_ENUM respond)
{
    qDebug("[PlatformControllerQnx] onRespondAccountClearUserPreferences");

    if(isLPMMode())
        return;

    m_pNaviDeploy->onRespondAccountClearUserPreferences(nReqID, (E_NAVI_REQUEST_RESULT)result, (NAVI_SETTINGS_RESPOND_ENUM)respond);
}

void PlatformControllerQnx::setMapAutoZoom(const int32_t nReqID, const E_NAVI_ONOFF_STATUS& status)
{
    qDebug("[PlatformControllerQnx] setMapAutoZoom status=%d", status);

    emit PlatformSetMapAutoZoom(nReqID, (NAVI_ONOFF_STATUS)status);
}

void PlatformControllerQnx::onRespondMapAutoZoom(int nReqID, NAVI_ONOFF_STATUS status, NAVI_SETTINGS_RES_ENUM respond)
{
    qDebug("[PlatformControllerQnx] onRespondMapAutoZoom status=%d", status);

    if(isLPMMode())
        return;

    m_pNaviDeploy->onRespondMapAutoZoom(nReqID, (E_NAVI_ONOFF_STATUS)status, (NAVI_SETTINGS_RESPOND_ENUM)respond);
}

void PlatformControllerQnx::notifySetMapAutoZoom(const NAVI_ONOFF_STATUS& status)
{
    qDebug("[PlatformControllerQnx] notifySetMapAutoZoom status=%d", status);

    m_pNaviDeploy->onRespondMapAutoZoom(NAVISERVICE_REQUEST_ID, (E_NAVI_ONOFF_STATUS)status, NAVI_SETTINGS_RESPOND_NOTIFY);
}

void PlatformControllerQnx::setMap3DBuilding(const int32_t nReqID, const E_NAVI_ONOFF_STATUS &status)
{
    qDebug("[PlatformControllerQnx] setMap3DBuilding status=%d", status);

    emit PlatformSetMap3DBuilding(nReqID, (NAVI_ONOFF_STATUS)status);
}

void PlatformControllerQnx::onRespondMap3DBuilding(int nReqID, NAVI_ONOFF_STATUS status, NAVI_SETTINGS_RES_ENUM respond)
{
    if(isLPMMode())
        return;

    m_pNaviDeploy->onRespondMap3DBuilding(nReqID, (E_NAVI_ONOFF_STATUS)status, (NAVI_SETTINGS_RESPOND_ENUM)respond);
}

void PlatformControllerQnx::setMapViewMode(const int32_t nReqID, const E_MAP_VIEW_MODE &mode)
{
    qDebug("[PlatformControllerQnx] setMapViewMode mode=%d", mode);

    emit PlatformSetMapViewMode(nReqID, (MAP_VIEW_MODE)mode);
}

void PlatformControllerQnx::onRespondMapViewMode(int nReqID, MAP_VIEW_MODE mode, NAVI_SETTINGS_RES_ENUM respond)
{
    if(isLPMMode())
        return;

     m_pNaviDeploy->onRespondMapViewMode(nReqID, (E_MAP_VIEW_MODE)mode, (NAVI_SETTINGS_RESPOND_ENUM)respond);
}

void PlatformControllerQnx::setMapTrafficFlow(const int32_t nReqID, const NAVI_MAP_TRAFFIC_FLOW_LIST_T& flowList)
{
    //qDebug("[PlatformControllerQnx] setMapTrafficFlow flow=%d", flow);

    //emit PlatformSetMapTrafficFlow(nReqID, (TRAFFIC_FLOW)flow);
}

void PlatformControllerQnx::onRespondMapTrafficFlow(int nReqID, TRAFFIC_FLOW flow, NAVI_SETTINGS_RES_ENUM respond)
{
    if(isLPMMode())
        return;

    //m_pNaviDeploy->onRespondMapTrafficFlow(nReqID, (E_TRAFFIC_FLOW)flow, (NAVI_SETTINGS_RESPOND_ENUM)respond);
}

void PlatformControllerQnx::setMapTrafficAccident(const int32_t nReqID, const E_NAVI_ONOFF_STATUS &status)
{
    qDebug("[PlatformControllerQnx] setMapTrafficAccident status=%d", status);

    emit PlatformSetMapTrafficAccident(nReqID, (NAVI_ONOFF_STATUS)status);
}

void PlatformControllerQnx::onRespondMapTrafficAccident(int nReqID, NAVI_ONOFF_STATUS status, NAVI_SETTINGS_RES_ENUM respond)
{
    if(isLPMMode())
        return;

    m_pNaviDeploy->onRespondMapTrafficAccident(nReqID, (E_NAVI_ONOFF_STATUS)status, (NAVI_SETTINGS_RESPOND_ENUM)respond);
}

void PlatformControllerQnx::setMapTrafficParking(const int32_t nReqID, const E_NAVI_ONOFF_STATUS &status)
{
    qDebug("[PlatformControllerQnx] setMapTrafficParking status=%d", status);

    emit PlatformSetMapTrafficParking(nReqID, (NAVI_ONOFF_STATUS)status);
}

void PlatformControllerQnx::onRespondMapTrafficParking(int nReqID, NAVI_ONOFF_STATUS status, NAVI_SETTINGS_RES_ENUM respond)
{
    if(isLPMMode())
        return;

    m_pNaviDeploy->onRespondMapTrafficParking(nReqID, (E_NAVI_ONOFF_STATUS)status, (NAVI_SETTINGS_RESPOND_ENUM)respond);
}

void PlatformControllerQnx::setMapTrafficSAPA(const int32_t nReqID, const E_NAVI_ONOFF_STATUS &status)
{
    qDebug("[PlatformControllerQnx] setMapTrafficSAPA status=%d", status);

    emit PlatformSetMapTrafficSAPA(nReqID, (NAVI_ONOFF_STATUS)status);
}

void PlatformControllerQnx::onRespondMapTrafficSAPA(int nReqID, NAVI_ONOFF_STATUS status, NAVI_SETTINGS_RES_ENUM respond)
{
    if(isLPMMode())
        return;

    m_pNaviDeploy->onRespondMapTrafficSAPA(nReqID, (E_NAVI_ONOFF_STATUS)status, (NAVI_SETTINGS_RESPOND_ENUM)respond);
}

void PlatformControllerQnx::setMapSatellite(const int32_t nReqID, const E_NAVI_ONOFF_STATUS &status)
{
    qDebug("[PlatformControllerQnx] setMapSatellite status=%d", status);

    emit PlatformSetMapSatellite(nReqID, (NAVI_ONOFF_STATUS)status);
}

void PlatformControllerQnx::onRespondMapSatellite(int nReqID, NAVI_ONOFF_STATUS status, NAVI_SETTINGS_RES_ENUM respond)
{
    if(isLPMMode())
        return;

    m_pNaviDeploy->onRespondMapSatellite(nReqID, (E_NAVI_ONOFF_STATUS)status, (NAVI_SETTINGS_RESPOND_ENUM)respond);
}


void PlatformControllerQnx::setMapTrafficOnMap(const int32_t nReqID, const E_NAVI_ONOFF_STATUS &status)
{
    qDebug("[PlatformControllerQnx] setMapTrafficOnMap status=%d", status);

   emit PlatformSetMapTrafficOnMap(nReqID, (NAVI_ONOFF_STATUS)status);
}

void PlatformControllerQnx::onRespondMapTrafficOnMap(int nReqID, NAVI_ONOFF_STATUS status, NAVI_SETTINGS_RES_ENUM respond)
{
    if(isLPMMode())
        return;

    m_pNaviDeploy->onRespondMapTrafficOnMap(nReqID, (E_NAVI_ONOFF_STATUS)status, (NAVI_SETTINGS_RESPOND_ENUM)respond);
}

void PlatformControllerQnx::setMapPOIDisplyPreference(const int32_t nReqID, const E_NAVI_ONOFF_STATUS& status)
{
    qDebug("[PlatformControllerQnx] setMapPOIDisplyPreference status=%d", status);

    emit PlatformSetMapPOIDisplyPreference(nReqID, (NAVI_ONOFF_STATUS)status);
}

void PlatformControllerQnx::onRespondMapPOIDisplyPreference(int nReqID, NAVI_ONOFF_STATUS status, NAVI_SETTINGS_RES_ENUM respond)
{
    if(isLPMMode())
        return;

    m_pNaviDeploy->onRespondMapPOIDisplyPreference(nReqID, (E_NAVI_ONOFF_STATUS)status, (NAVI_SETTINGS_RESPOND_ENUM)respond);
}

void PlatformControllerQnx::setMapDisplaySavedPOIIcon(const int32_t nReqID, const E_NAVI_ONOFF_STATUS &status)
{
    qDebug("[PlatformControllerQnx] setMapDisplaySavedPOIIcon status=%d", status);

    emit PlatformSetMapDisplaySavedPOIIcon(nReqID, (NAVI_ONOFF_STATUS)status);
}

void PlatformControllerQnx::onRespondMapDisplaySavedPOIIcon(int nReqID, NAVI_ONOFF_STATUS status, NAVI_SETTINGS_RES_ENUM respond)
{
    if(isLPMMode())
        return;

    m_pNaviDeploy->onRespondMapDisplaySavedPOIIcon(nReqID, (E_NAVI_ONOFF_STATUS)status, (NAVI_SETTINGS_RESPOND_ENUM)respond);
}

void PlatformControllerQnx::setMapDynamicView(const int32_t nReqID, const E_MAP_DYNAMIC_VIEW_MODE &mode)
{
    qDebug("[PlatformControllerQnx] setMapDynamicView mode=%d", mode);

    emit PlatformSetMapDynamicView(nReqID, (MAP_DYNAMIC_VIEW_MODE)mode);
}

void PlatformControllerQnx::onRespondMapDynamicView(int nReqID, MAP_DYNAMIC_VIEW_MODE mode, NAVI_SETTINGS_RES_ENUM respond)
{
    if(isLPMMode())
        return;

    m_pNaviDeploy->onRespondMapDynamicView(nReqID, (E_MAP_DYNAMIC_VIEW_MODE)mode, (NAVI_SETTINGS_RESPOND_ENUM)respond);
}

void PlatformControllerQnx::onResponseSetMapCarIcon(int icon)
{
    qWarning() <<"PlatformControllerQnx::onResponseSetMapCarIcon()";
    if(isLPMMode())
        return;

    //m_pNaviDeploy->onsetMapCarIcon(icon);
}

void PlatformControllerQnx::setMapHighwayMode(const int32_t nReqID, const E_NAVI_ONOFF_STATUS& status)
{
    qDebug("[PlatformControllerQnx] setMapHighwayMode");

    emit PlatformSetMapHighwayMode(nReqID, (NAVI_ONOFF_STATUS)status);
}

void PlatformControllerQnx::onRespondMapHighwayMode(int nReqID, NAVI_ONOFF_STATUS status, NAVI_SETTINGS_RES_ENUM respond)
{
    qWarning() <<"PlatformControllerQnx::onRespondMapHighwayMode()";
    if(isLPMMode())
        return;

     m_pNaviDeploy->onRespondMapHighwayMode(nReqID, (E_NAVI_ONOFF_STATUS)status, (NAVI_SETTINGS_RESPOND_ENUM)respond);

}

void PlatformControllerQnx::setMapSkybox(const int32_t nReqID, const E_NAVI_ONOFF_STATUS& status)
{
    qWarning() <<"PlatformControllerQnx::setMapSkybox()";

    emit PlatformSetMapSkybox(nReqID, (NAVI_ONOFF_STATUS)status);
}

void PlatformControllerQnx::onRespondMapSkybox(int nReqID, NAVI_ONOFF_STATUS status, NAVI_SETTINGS_RES_ENUM respond)
{
    qWarning() <<"PlatformControllerQnx::onRespondMapSkybox()";
    if(isLPMMode())
        return;

    m_pNaviDeploy->onRespondMapSkybox(nReqID, (E_NAVI_ONOFF_STATUS)status, (NAVI_SETTINGS_RESPOND_ENUM)respond);
}

void PlatformControllerQnx::onResponseNaviVersionInfo(char* info)
{
    qWarning() <<"PlatformControllerQnx::onRequestNaviVersionInfo()";
}

void PlatformControllerQnx::onResponseMapUpdateLatestStatus(bool status)
{
    qWarning() <<"PlatformControllerQnx::onRequestMapUpdateLatestStatus()";
}

void PlatformControllerQnx::onResponseMapUpdate(NAVI_REQUEST_RESULT result)
{
    qWarning() <<"PlatformControllerQnx::onRequestMapUpdate()";
}

void PlatformControllerQnx::onResponseMobileTmapSyncStatus(bool status)
{
    qWarning() <<"PlatformControllerQnx::onRequestMobileTmapSyncStatus()";
}

void PlatformControllerQnx::onResponseSyncMobileTmap(NAVI_REQUEST_RESULT result)
{
    qWarning() <<"PlatformControllerQnx::onRequestSyncMobileTmap()";
}

void PlatformControllerQnx::setNaviTraceTrackingMode(const int32_t nReqID)
{
    qDebug("[PlatformControllerQnx] setNaviTraceTrackingMode");
}

void PlatformControllerQnx::onResponseCancelMobileTmapSync(NAVI_REQUEST_RESULT result)
{
    qWarning() <<"PlatformControllerQnx::onRequestCancelMobileTmapSync()";
}

void PlatformControllerQnx::onResponseNaviFactoryReset(NAVI_REQUEST_RESULT result)
{
    qWarning() <<"PlatformControllerQnx::onRequestNaviFactoryReset()";
}


void PlatformControllerQnx::onResponseSetRouteAlternative(NAVI_ROUTE_ALTERNATIVE_LIST* list)
{

}

void PlatformControllerQnx::onResponseSetRouteOutOfRouteRecalculation(NAVI_ONOFF_STATUS status)
{
}

void PlatformControllerQnx::onResponseSetWarningSoundWhenNaviVolumeZero(NAVI_ONOFF_STATUS status)
{
}

#endif

#ifdef PLATFORM_QNX // Common APIs
void PlatformControllerQnx::requestToTest(const int& id, const bool& bFlag, const char* msg)
{
    qWarning() << "[PlatformControllerQnx] requestToTest called()";
    if(isLPMMode())
        return;

    m_pNaviDeploy->onTestCompleted(msg, id, bFlag);
}

void PlatformControllerQnx::getNaviGuideStatus(const int32_t &nReqID)
{
    qDebug("[PlatformControllerQnx] getNaviGuideStatus %d", nReqID);

    emit PlatformGetNaviGuideStatus(nReqID);
}

void PlatformControllerQnx::onResponseGetNaviGuideStatus(int nReqID, NAVI_REQUEST_RESULT result, bool bGuidanceRunning)
{
    qDebug("[PlatformControllerQnx] onResponseGetNaviGuideStatus result=%d, rgrunning=%d", result, bGuidanceRunning);
    if(isLPMMode())
        return;

    m_pNaviDeploy->ongetNaviGuideStatus(nReqID, (E_NAVI_REQUEST_RESULT)result, bGuidanceRunning);
}

void PlatformControllerQnx::getNaviMuteStatus(const int32_t &nReqID)
{
    qDebug("[PlatformControllerQnx] getNaviMuteStatus %d", nReqID);

    emit PlatformGetNaviMuteStatus(nReqID);
}

void PlatformControllerQnx::onResponseGetNaviMuteStatus(int nReqID, NAVI_REQUEST_RESULT result, bool bMuteStatus)
{
    qDebug("[PlatformControllerQnx] onResponseGetNaviMuteStatus result=%d, mute=%d", result, bMuteStatus);
    if(isLPMMode())
        return;

    m_pNaviDeploy->ongetNaviMuteStatus(nReqID, (E_NAVI_REQUEST_RESULT)result, bMuteStatus);
}

void PlatformControllerQnx::getNaviServiceAvailable(const int32_t &nReqID)
{
    qDebug("[PlatformControllerQnx] getNaviServiceAvailable %d", nReqID);

    emit PlatformGetNaviServiceAvailable(nReqID);
}

void PlatformControllerQnx::onResponseGetNaviServiceAvailable(int nReqID, NAVI_REQUEST_RESULT result, bool bNaviAvailable)
{
    qDebug("[PlatformControllerQnx] onResponseGetNaviServiceAvailable result=%d, avail=%d", result, bNaviAvailable);
    if(isLPMMode())
        return;

    m_pNaviDeploy->ongetNaviServiceAvailable(nReqID, (E_NAVI_REQUEST_RESULT)result, bNaviAvailable);
}

void PlatformControllerQnx::getNaviCommuteStatus(const int32_t& nReqID)
{
    qDebug("[PlatformControllerQnx] getNaviCommuteStatus %d", nReqID);

    emit PlatformGetNaviCommuteStatus(nReqID);
}

void PlatformControllerQnx::onResponseGetNaviCommuteStatus(int nReqID, NAVI_REQUEST_RESULT result, NAVI_COMMUTE_STATUS status)
{
    qDebug("[PlatformControllerQnx] onResponseGetNaviCommuteStatus status(%d)", status);
    if(isLPMMode())
        return;
    m_pNaviDeploy->ongetNaviCommuteStatus(nReqID, (E_NAVI_REQUEST_RESULT)result, (E_NAVI_COMMUTE_STATUS)status);
}

void PlatformControllerQnx::getNaviFavoritesStatus(const int32_t& nReqID)
{
    qDebug("[PlatformControllerQnx] getNaviFavoritesStatus %d", nReqID);

    emit PlatformGetNaviFavoritesStatus(nReqID);
}

void PlatformControllerQnx::onResponseGetNaviFavoritesStatus(int nReqID, NAVI_REQUEST_RESULT result, bool bHomeSetStatus, bool bWorkSetStatus)
{
    qDebug("[PlatformControllerQnx] onResponseGetNaviFavoritesStatus bHomeSetStatus(%d), bWorkSetStatus(%d)", bHomeSetStatus, bWorkSetStatus);
    if(isLPMMode())
        return;

    m_pNaviDeploy->ongetNaviFavoritesStatus(nReqID, (E_NAVI_REQUEST_RESULT)result, bHomeSetStatus, bWorkSetStatus);
}

void PlatformControllerQnx::getNaviTimezoneInfo(const int32_t nReqID)
{
    qDebug("[PlatformControllerQnx] getNaviTimezoneInfo");

    emit PlatformGetNaviTimezoneInfo(nReqID);
}

void PlatformControllerQnx::onNotifyNaviTimezoneInfo(int utcOffset, bool bDst, std::string timezoneName)
{
    qDebug("[PlatformControllerQnx] onNotifyNaviTimezoneInfo");
    if(isLPMMode())
        return;

    m_pNaviDeploy->onNotifyNaviTimezoneInfo(utcOffset, bDst, timezoneName);
}

void PlatformControllerQnx::onResponseGetNaviTimezoneInfo(int nReqID, NAVI_REQUEST_RESULT result, int utcOffset, bool bDst, std::string timezoneName)
{
    qDebug("[PlatformControllerQnx] onResponseGetNaviTimezoneInfo result=%d", result);
    if(isLPMMode())
        return;

    m_pNaviDeploy->ongetNaviTimezoneInfo(nReqID, (E_NAVI_REQUEST_RESULT)result, utcOffset, bDst, timezoneName);
}

void PlatformControllerQnx::getNaviSharedUrl(const int32_t nReqID, const E_NAVI_POSITION_ROUTE_TYPE& type)
{
    qDebug("[PlatformControllerQnx] getNaviSharedUrl");

    emit PlatformGetNaviSharedUrl(nReqID, (NAVI_POSITION_ROUTE_TYPE)type);
}

void PlatformControllerQnx::onResponseGetNaviSharedUrl(int nReqID, NAVI_REQUEST_RESULT result, NAVI_POSITION_ROUTE_TYPE type, std::string url)
{
    qDebug("[PlatformControllerQnx] onResponseGetNaviSharedUrl result=%d, url=%s", result, url.c_str());
    if(isLPMMode())
        return;

    m_pNaviDeploy->ongetNaviSharedUrl(nReqID, (E_NAVI_REQUEST_RESULT)result, (E_NAVI_POSITION_ROUTE_TYPE)type, url);
}

void PlatformControllerQnx::unsubscribeNavStatus(const int32_t& nReqID)
{
    qDebug("[PlatformControllerQnx] unsubscribeNavStatus");

    emit PlatformUnsubscribeNavStatus(nReqID);
}

void PlatformControllerQnx::onResponseUnsubscribeNavStatus(int nReqID, NAVI_REQUEST_RESULT result)
{
    qDebug("[PlatformControllerQnx] onResponseUnsubscribeNavStatus result=%d", result);
    if(isLPMMode())
        return;

    m_pNaviDeploy->onunsubscribeNavStatus(nReqID, (E_NAVI_REQUEST_RESULT)result);
}

void PlatformControllerQnx::subscribeNavStatus(const int32_t& nReqID)
{
    qDebug("[PlatformControllerQnx] subscribeNavStatus");

    emit PlatformSubscribeNavStatus(nReqID);
}

void PlatformControllerQnx::onResponseSubscribeNavStatus(int nReqID, NAVI_REQUEST_RESULT result)
{
    qDebug("[PlatformControllerQnx] onResponseSubscribeNavStatus result=%d", result);
    if(isLPMMode())
        return;

    m_pNaviDeploy->onsubscribeNavStatus(nReqID, (E_NAVI_REQUEST_RESULT)result);
}

void PlatformControllerQnx::getRouteInfo(const int32_t& nReqID, const int& maxPoints, const int& waypointFrequency, const bool& frequencyType, const int& startPoint, const int& endPoint)
{
    qDebug("[PlatformControllerQnx] getRouteInfo");

    emit PlatformGetRouteInfo(nReqID, maxPoints, waypointFrequency, frequencyType, startPoint, endPoint);
}

void PlatformControllerQnx::onResponseGetRouteInfo(int nReqID, NAVI_REQUEST_RESULT result, std::vector<NAVI_POINT_ALONG_ROUTE> routeInfo)
{
    qDebug("[PlatformControllerQnx] onResponseGetRouteInfo result=%d", result);
    if(isLPMMode())
        return;

    std::vector<NAVI_POINT_ALONG_ROUTE_T> routeInfoList;
    routeInfoList.reserve(routeInfo.size());

    for (std::vector<NAVI_POINT_ALONG_ROUTE>::iterator it = routeInfo.begin(); it!=routeInfo.end(); ++it)
    {
        NAVI_POINT_ALONG_ROUTE item = *it;
        NAVI_POINT_ALONG_ROUTE_T stRouteInfo;
        stRouteInfo.geoCoordinates.latitude = item.geoCoordinates.latitude;
        stRouteInfo.geoCoordinates.longitude = item.geoCoordinates.longitude;
        stRouteInfo.geoCoordinates.altitude = item.geoCoordinates.altitude;
        stRouteInfo.timeDistance.eta = item.timeDistance.eta;
        stRouteInfo.timeDistance.tta = item.timeDistance.tta;
        stRouteInfo.timeDistance.distance = item.timeDistance.distance;
        stRouteInfo.heading = item.heading;
        stRouteInfo.id = item.id;

        routeInfoList.push_back(stRouteInfo);
    }

    m_pNaviDeploy->ongetRouteInfo(nReqID, (E_NAVI_REQUEST_RESULT)result, routeInfoList);
}

void PlatformControllerQnx::subscribeRouteRecalculation(const int32_t& nReqID)
{
    qDebug("[PlatformControllerQnx] subscribeRouteRecalculation");

    emit PlatformSubscribeRouteRecalculation(nReqID);
}

void PlatformControllerQnx::onResponseSubscribeRouteRecalculation(int nReqID, NAVI_REQUEST_RESULT result)
{
    qDebug("[PlatformControllerQnx] onResponseSubscribeRouteRecalculation result=%d", result);
    if(isLPMMode())
        return;

    m_pNaviDeploy->onsubscribeRouteRecalculation(nReqID, (E_NAVI_REQUEST_RESULT)result);
}

void PlatformControllerQnx::unsubscribeRouteRecalculation(const int32_t& nReqID)
{
    qDebug("[PlatformControllerQnx] unsubscribeRouteRecalculation");

    emit PlatformUnsubscribeRouteRecalculation(nReqID);
}

void PlatformControllerQnx::onResponseUnsubscribeRouteRecalculation(int nReqID, NAVI_REQUEST_RESULT result)
{
    qDebug("[PlatformControllerQnx] onResponseUnsubscribeRouteRecalculation result=%d", result);
    if(isLPMMode())
        return;

    m_pNaviDeploy->onunsubscribeRouteRecalculation(nReqID, (E_NAVI_REQUEST_RESULT)result);
}

void PlatformControllerQnx::removeIconsOnMap(const int32_t& nReqID, const std::vector<std::string>& icons)
{
    qDebug("[PlatformControllerQnx] removeIconsOnMap");

    emit PlatformRemoveIconsOnMap(nReqID, icons);
}

void PlatformControllerQnx::onResponseRemoveIconsOnMap(int nReqID, NAVI_REQUEST_RESULT result)
{
    qDebug("[PlatformControllerQnx] onResponseRemoveIconsOnMap result=%d", result);
    if(isLPMMode())
        return;

    m_pNaviDeploy->onremoveIconsOnMapp(nReqID, (E_NAVI_REQUEST_RESULT)result);
}

void PlatformControllerQnx::addIconsOnMap(const int32_t &nReqID, const std::vector<NAVI_ICON_INFO_T> &icons)
{
    qDebug("[PlatformControllerQnx] addIconsOnMap");

    std::vector<NAVI_ICON_INFO> iconList;
    iconList.reserve(icons.size());

    for (std::vector<NAVI_ICON_INFO_T>::const_iterator it = icons.begin(); it!=icons.end(); ++it)
    {
        NAVI_ICON_INFO_T item = *it;
        NAVI_ICON_INFO stIcon;

        stIcon.icon_id = item.icon_id;
        stIcon.cord.latitude = item.cord.latitude;
        stIcon.cord.longitude = item.cord.longitude;
        stIcon.cord.altitude = item.cord.altitude;
        stIcon.icon_path = item.icon_path;

        iconList.push_back(stIcon);
    }

    emit PlatformAddIconOnMap(nReqID, iconList);
}

void PlatformControllerQnx::onResponseAddIconOnMap(int nReqID, NAVI_REQUEST_RESULT result)
{
    qDebug("[PlatformControllerQnx] onResponseAddIconOnMap result=%d", result);
    if(isLPMMode())
        return;

    m_pNaviDeploy->onaddIconsOnMap(nReqID, (E_NAVI_REQUEST_RESULT)result);
}

void PlatformControllerQnx::updatePointsAlongRoute(const int32_t& nReqID, const std::vector<std::string>& pointAlongRouteID)
{
    qDebug("[PlatformControllerQnx] updatePointsAlongRoute");
}

void PlatformControllerQnx::onResponseUpdateRouteWaypoints(int nReqID, NAVI_REQUEST_RESULT result, std::vector<NAVI_COMMUTE_DESTINATION_INFO> routeInfo)
{
    qDebug("[PlatformControllerQnx] onResponseUpdateRouteWaypoints result=%d", result);
    if(isLPMMode())
        return;
}

void PlatformControllerQnx::getNaviSupportFunctionList(const int32_t& nReqID)
{
    qDebug("[PlatformControllerQnx] getNaviSupportFunctionList");

    emit PlatformGetNaviSupportFunctionList(nReqID);
}

void PlatformControllerQnx::onResponseGetNaviSupportFunctionList(int nReqID, NAVI_REQUEST_RESULT result, NAVI_SUPPORTED_FEATURE* supportedFeature)
{
    qDebug("[PlatformControllerQnx] onResponseGetNaviSupportFunctionList result=%d", result);
    if(isLPMMode())
        return;

    NAVI_SUPPORTED_FEATURE_T stNaviSupportedFeature={};
    stNaviSupportedFeature.nNum = supportedFeature->nNum;
    stNaviSupportedFeature.shareRouteUrl = supportedFeature->shareRouteUrl;
    stNaviSupportedFeature.shareLocationUrl = supportedFeature->shareLocationUrl;

    m_pNaviDeploy->ongetNaviSupportFunctionList(nReqID, (E_NAVI_REQUEST_RESULT)result, stNaviSupportedFeature);
}
#endif

#ifdef PLATFORM_QNX // Connected Service(WebApp)
void PlatformControllerQnx::addToFavourites(const int32_t& nReqID, const std::string& title, const NAVI_LOCATION_ADDRESS_COORDINATE_T& locationData, const E_NAVI_FAVORITES_TYPE& favoritesType)
{
    qDebug("[PlatformControllerQnx] addToFavourites : %d", nReqID);

    QSharedPointer <NAVI_LOCATION_ADDRESS_COORDINATE> pLocationData(new NAVI_LOCATION_ADDRESS_COORDINATE);
    pLocationData<<locationData;

    NAVI_FAVORITES_TYPE favorites_type = NAVI_FAVORITES_TYPE_GENERAL;
    if (favoritesType == E_NAVI_FAVORITES_TYPE_GENERAL)     favorites_type = NAVI_FAVORITES_TYPE_GENERAL;
    else if (favoritesType == E_NAVI_FAVORITES_TYPE_HOME)   favorites_type = NAVI_FAVORITES_TYPE_HOME;
    else if (favoritesType == E_NAVI_FAVORITES_TYPE_WORK)   favorites_type = NAVI_FAVORITES_TYPE_WORK;

    emit PlatformAddToFavourites(nReqID, title, pLocationData.data(), favorites_type);
}

void PlatformControllerQnx::onResponseAddToFavourites(int nReqID, NAVI_REQUEST_RESULT result)
{
    qDebug("[PlatformControllerQnx] onResponseAddToFavourites nReqID=%d, result=%d", nReqID, result);
    if(isLPMMode())
        return;

    m_pNaviDeploy->onaddToFavourites(nReqID, (E_NAVI_REQUEST_RESULT)result);
}

void PlatformControllerQnx::addWaypoint(const int32_t& nReqID, const std::string& referenceWaypointID, const NAVI_LOCATION_ADDRESS_COORDINATE_T& locationData)
{
    qDebug("[PlatformControllerQnx] addWaypoint %d", nReqID);

    QSharedPointer <NAVI_LOCATION_ADDRESS_COORDINATE> pLocationData(new NAVI_LOCATION_ADDRESS_COORDINATE);
    pLocationData<<locationData;
    int wayPointId = -1;
    if (!QString::fromStdString(referenceWaypointID).compare(""))
        wayPointId = QString::fromStdString(referenceWaypointID).toInt();
    emit PlatformAddWaypoint(nReqID, wayPointId, pLocationData.data());
}

void PlatformControllerQnx::onResponseAddWaypoint(int nReqID, NAVI_REQUEST_RESULT result)
{
    qDebug("[PlatformControllerQnx] onResponseAddWaypoint nReqID=%d, result=%d", nReqID, result);
    if(isLPMMode())
        return;

    m_pNaviDeploy->onaddWaypoint(nReqID, (E_NAVI_REQUEST_RESULT)result);
}

void PlatformControllerQnx::getAddressFromGeo(const int32_t& nReqID, const NAVI_GEO_COORDINATE_T& geoCoordinates)
{
    qDebug("[PlatformControllerQnx] getAddressFromGeo %d", nReqID);

    QSharedPointer <NAVI_GEO_COORDINATE> pGeoCoordinates(new NAVI_GEO_COORDINATE);
    memcpy(pGeoCoordinates.data(), &geoCoordinates, sizeof(NAVI_GEO_COORDINATE));
    emit PlatformGetAddressFromGeo(nReqID, pGeoCoordinates.data());
}

void PlatformControllerQnx::onResponseGetAddressFromGeo(int nReqID, NAVI_REQUEST_RESULT result, NAVI_ADDRESS* address)
{
    qDebug("[PlatformControllerQnx] onResponseGetAddressFromGeo %d", nReqID);
    if(isLPMMode())
        return;

    E_NAVI_REQUEST_RESULT eResult = E_NAVI_REQUEST_RESULT_FAIL;
    std::vector<NAVI_ADDRESS_T> addressList;
    NAVI_ADDRESS_T stAddress = {};
    if(NAVI_REQUEST_RESULT_DONE == result)
    {
        stAddress.addressString = address->addressString;
        stAddress.addressCode = address->addressCode;
        stAddress.countryCode = address->countryCode;
        stAddress.country = address->country;
        stAddress.state = address->state;
        stAddress.stateCode = address->stateCode;
        stAddress.city = address->city;
        stAddress.county = address->county;
        stAddress.province = address->province;
        stAddress.zipCode = address->zipCode;
        stAddress.district = address->district;
        stAddress.street = address->street;
        stAddress.blockNumber = address->blockNumber;
        stAddress.houseNumber = address->houseNumber;
        stAddress.telephone = address->telephone;

        addressList.reserve(1);
        addressList.push_back(stAddress);

        eResult = E_NAVI_REQUEST_RESULT_DONE;
    }

    qDebug("[PlatformControllerQnx] onResponseGetAddressFromGeo nReqID=%d, addr=%s", nReqID, address->addressString.c_str());
    m_pNaviDeploy->ongetAddressFromGeo((int)nReqID, eResult, addressList);
}

void PlatformControllerQnx::getCurrentLocation(const int32_t& nReqID)
{
    qDebug("[PlatformControllerQnx] getCurrentLocation %d", nReqID);

    emit PlatformGetCurrentLocation(nReqID);
}

void PlatformControllerQnx::onReponseGetCurrentLocation(int nReqID, NAVI_REQUEST_RESULT result, NAVI_LOCATION_ADDRESS_COORDINATE* locationData)
{
    qDebug("[PlatformControllerQnx] onReponseGetCurrentLocation() nReqID=%d, result=%d", nReqID, result);
    if(isLPMMode())
        return;

    E_NAVI_REQUEST_RESULT eResult = E_NAVI_REQUEST_RESULT_FAIL;
    NAVI_LOCATION_ADDRESS_COORDINATE_T stLocationData = {};
    if(NAVI_REQUEST_RESULT_DONE == result)
    {
        stLocationData.locationName = locationData->locationName;
        stLocationData.address.addressString = locationData->address.addressString;
        stLocationData.address.addressCode = locationData->address.addressCode;
        stLocationData.address.countryCode = locationData->address.countryCode;
        stLocationData.address.country = locationData->address.country;
        stLocationData.address.state = locationData->address.state;
        stLocationData.address.stateCode = locationData->address.stateCode;
        stLocationData.address.city = locationData->address.city;
        stLocationData.address.county = locationData->address.county;
        stLocationData.address.province = locationData->address.province;
        stLocationData.address.zipCode = locationData->address.zipCode;
        stLocationData.address.district = locationData->address.district;
        stLocationData.address.street = locationData->address.street;
        stLocationData.address.blockNumber = locationData->address.blockNumber;
        stLocationData.address.houseNumber = locationData->address.houseNumber;
        stLocationData.address.telephone = locationData->address.telephone;
        stLocationData.geoCoordinates.latitude = locationData->geoCoordinates.latitude;
        stLocationData.geoCoordinates.longitude = locationData->geoCoordinates.longitude;
        stLocationData.geoCoordinates.altitude = locationData->geoCoordinates.altitude;

        eResult = E_NAVI_REQUEST_RESULT_DONE;
    }

    m_pNaviDeploy->ongetCurrentLocation(nReqID, eResult, stLocationData);
}

void PlatformControllerQnx::getDestination(const int32_t& nReqID)
{
    qDebug("[PlatformControllerQnx] getDestination %d", nReqID);

    emit PlatformGetDestination(nReqID);
}

void PlatformControllerQnx::onResponseGetDestination(int nReqID, NAVI_REQUEST_RESULT result, NAVI_WAYPOINT_INFO* destination)
{
    qDebug("[PlatformControllerQnx] onResponseGetDestination() nReqID=%d, result=%d", nReqID, result);
    if(isLPMMode())
        return;

    E_NAVI_REQUEST_RESULT eResult = E_NAVI_REQUEST_RESULT_FAIL;
    NAVI_WAYPOINT_INFO_T stWayPointInfo = {};
    if(NAVI_REQUEST_RESULT_DONE == result)
    {
        stWayPointInfo.locationData.locationName = destination->locationData.locationName;
        stWayPointInfo.locationData.address.addressString = destination->locationData.address.addressString;
        stWayPointInfo.locationData.address.addressCode = destination->locationData.address.addressCode;
        stWayPointInfo.locationData.address.countryCode = destination->locationData.address.countryCode;
        stWayPointInfo.locationData.address.country = destination->locationData.address.country;
        stWayPointInfo.locationData.address.state = destination->locationData.address.state;
        stWayPointInfo.locationData.address.stateCode = destination->locationData.address.stateCode;
        stWayPointInfo.locationData.address.city = destination->locationData.address.city;
        stWayPointInfo.locationData.address.county = destination->locationData.address.county;
        stWayPointInfo.locationData.address.province = destination->locationData.address.province;
        stWayPointInfo.locationData.address.zipCode = destination->locationData.address.zipCode;
        stWayPointInfo.locationData.address.district = destination->locationData.address.district;
        stWayPointInfo.locationData.address.street = destination->locationData.address.street;
        stWayPointInfo.locationData.address.blockNumber = destination->locationData.address.blockNumber;
        stWayPointInfo.locationData.address.houseNumber = destination->locationData.address.houseNumber;
        stWayPointInfo.locationData.address.telephone = destination->locationData.address.telephone;
        stWayPointInfo.locationData.geoCoordinates.latitude = destination->locationData.geoCoordinates.latitude;
        stWayPointInfo.locationData.geoCoordinates.longitude = destination->locationData.geoCoordinates.longitude;
        stWayPointInfo.locationData.geoCoordinates.altitude = destination->locationData.geoCoordinates.altitude;
        stWayPointInfo.timeDistance.eta = destination->timeDistance.eta;
        stWayPointInfo.timeDistance.tta = destination->timeDistance.tta;
        stWayPointInfo.timeDistance.distance = destination->timeDistance.distance;

        eResult = E_NAVI_REQUEST_RESULT_DONE;
    }
    m_pNaviDeploy->ongetDestination(nReqID, eResult, stWayPointInfo);
}

void PlatformControllerQnx::getGeoFromAddress(const int32_t& nReqID, const NAVI_ADDRESS_T& adress)
{
    qDebug("[PlatformControllerQnx] getGeoFromAddress nReqID=%d", nReqID);

    NAVI_ADDRESS naviAddress;
    naviAddress<<adress;

    emit PlatformGetGeoFromAddress(nReqID, naviAddress);
}

void PlatformControllerQnx::onResponstGetGeoFromAddress(int nReqID, NAVI_REQUEST_RESULT result, std::vector<NAVI_LOCATION_ADDRESS_COORDINATE> geoAddressCoordinates)
{
    qDebug("[PlatformControllerQnx] onResponstGetGeoFromAddress() nReqID=%d, result=%d", nReqID, result);
    if(isLPMMode())
        return;
    std::vector<NAVI_LOCATION_ADDRESS_COORDINATE_T> geoAddressCoordinateList;
    NAVI_LOCATION_ADDRESS_COORDINATE_T locationAddr;
    foreach (NAVI_LOCATION_ADDRESS_COORDINATE location, geoAddressCoordinates) {
        locationAddr<<location;
        geoAddressCoordinateList.push_back(locationAddr);
    }
    m_pNaviDeploy->ongetGeoFromAddress(nReqID, (E_NAVI_REQUEST_RESULT)result, geoAddressCoordinateList);
}

void PlatformControllerQnx::getSavedLocations(const int32_t& nReqID)
{
    qDebug("[PlatformControllerQnx] getSavedLocations %d", nReqID);

    emit PlatformGetSavedLocations(nReqID);
}

void PlatformControllerQnx::onResponseGetSavedLocations(int nReqID, NAVI_REQUEST_RESULT result, std::vector<NAVI_SAVED_LOCATION_INFO> savedLocatonList)
{
    qDebug("[PlatformControllerQnx] onResponseGetSavedLocations() nReqID=%d, result=%d", nReqID, result);
    if(isLPMMode())
        return;
    std::vector<NAVI_SAVED_LOCATION_INFO_T> stLocationAddressCoordnateList;
    E_NAVI_REQUEST_RESULT eResult = E_NAVI_REQUEST_RESULT_FAIL;

    if(NAVI_REQUEST_RESULT_DONE == result)
    {
        foreach(NAVI_SAVED_LOCATION_INFO elem, savedLocatonList)
        {
            NAVI_SAVED_LOCATION_INFO_T locationInfo;
            memcpy(&locationInfo.location.geoCoordinates, &elem.location.geoCoordinates, sizeof(NAVI_GEO_COORDINATE_T));
            locationInfo.location.address<<elem.location.address;
            locationInfo.location.locationName = elem.location.locationName;
            locationInfo.type = (E_NAVI_FAVORITES_TYPE)elem.type;
            stLocationAddressCoordnateList.push_back(locationInfo);
        }

        eResult = E_NAVI_REQUEST_RESULT_DONE;
    }

    m_pNaviDeploy->ongetSavedLocations(nReqID, eResult, stLocationAddressCoordnateList);
}

void PlatformControllerQnx::getItinerary(const int32_t& nReqID)
{
    qDebug("[PlatformControllerQnx] getItinerary %d", nReqID);

    emit PlatformGetItinerary(nReqID);
}

void PlatformControllerQnx::onResponseGetItinerary(int nReqID, NAVI_REQUEST_RESULT result, NAVI_WAYPOINT_INFO_LIST* waypointList)
{
    qDebug("[PlatformControllerQnx] onResponseGetItinerary() nReqID=%d, result=%d", nReqID, result);
    if(isLPMMode())
        return;

    std::vector<NAVI_WAYPOINT_INFO_T> stWayPointInfo;
    NAVI_WAYPOINT_INFO_T stWayPoint;
    E_NAVI_REQUEST_RESULT eResult = E_NAVI_REQUEST_RESULT_FAIL;

    if(NAVI_REQUEST_RESULT_DONE == result)
    {
        if(waypointList != NULL)
        {
            if(waypointList->count <= 5)
            {
                stWayPointInfo.reserve(waypointList->count);
                for(int i=0; i<waypointList->count; i++)
                {
                    memset(&stWayPoint, 0x00, sizeof(stWayPoint));
                    stWayPoint.locationData.locationName = waypointList->data[i].locationData.locationName;
                    stWayPoint.locationData.address.addressString = waypointList->data[i].locationData.address.addressString;
                    stWayPoint.locationData.address.addressCode = waypointList->data[i].locationData.address.addressCode;
                    stWayPoint.locationData.address.countryCode = waypointList->data[i].locationData.address.countryCode;
                    stWayPoint.locationData.address.country = waypointList->data[i].locationData.address.country;
                    stWayPoint.locationData.address.state = waypointList->data[i].locationData.address.state;
                    stWayPoint.locationData.address.stateCode = waypointList->data[i].locationData.address.stateCode;
                    stWayPoint.locationData.address.city = waypointList->data[i].locationData.address.city;
                    stWayPoint.locationData.address.county = waypointList->data[i].locationData.address.county;
                    stWayPoint.locationData.address.province = waypointList->data[i].locationData.address.province;
                    stWayPoint.locationData.address.zipCode = waypointList->data[i].locationData.address.zipCode;
                    stWayPoint.locationData.address.district = waypointList->data[i].locationData.address.district;
                    stWayPoint.locationData.address.street = waypointList->data[i].locationData.address.street;
                    stWayPoint.locationData.address.blockNumber = waypointList->data[i].locationData.address.blockNumber;
                    stWayPoint.locationData.address.houseNumber = waypointList->data[i].locationData.address.houseNumber;
                    stWayPoint.locationData.address.telephone = waypointList->data[i].locationData.address.telephone;
                    stWayPoint.locationData.geoCoordinates.latitude = waypointList->data[i].locationData.geoCoordinates.latitude;
                    stWayPoint.locationData.geoCoordinates.longitude = waypointList->data[i].locationData.geoCoordinates.longitude;
                    stWayPoint.locationData.geoCoordinates.altitude = waypointList->data[i].locationData.geoCoordinates.altitude;
                    stWayPoint.timeDistance.eta = waypointList->data[i].timeDistance.eta;
                    stWayPoint.timeDistance.tta = waypointList->data[i].timeDistance.tta;
                    stWayPoint.timeDistance.distance = waypointList->data[i].timeDistance.distance;

                    stWayPointInfo.push_back(stWayPoint);
                }
            }
        }

        eResult = E_NAVI_REQUEST_RESULT_DONE;
    }

    m_pNaviDeploy->ongetItinerary(nReqID, eResult, stWayPointInfo);
}

void PlatformControllerQnx::getNaviSearchFeatures(const int32_t& nReqID)
{
    qDebug("[PlatformControllerQnx] getNaviSearchFeatures %d", nReqID);

    emit PlatformGetNaviSearchFeatures(nReqID);
}

void PlatformControllerQnx::onReponsegetNaviSearchFeatures(int nReqID, NAVI_REQUEST_RESULT result, std::vector<NAVI_SEARCH_CATEGORIES> categoriesNameAndID)
{
    qDebug("[PlatformControllerQnx] onReponsegetNaviSearchFeatures() nReqID=%d, result=%d", nReqID, result);
    if(isLPMMode())
        return;

    E_NAVI_REQUEST_RESULT eResult = E_NAVI_REQUEST_RESULT_FAIL;
    std::vector<NAVI_SEARCH_CATEGORIES_T> searchCategories;
    NAVI_SEARCH_CATEGORIES_T searchCategory;
    if(NAVI_REQUEST_RESULT_DONE == result)
    {
        eResult = E_NAVI_REQUEST_RESULT_DONE;
    }

    foreach (NAVI_SEARCH_CATEGORIES elem, categoriesNameAndID) {
        searchCategory.id = elem.id;
        searchCategory.name = elem.id;
        searchCategories.push_back(searchCategory);
    }
    m_pNaviDeploy->ongetNaviSearchFeatures(nReqID, eResult, searchCategories);
}

void PlatformControllerQnx::getNaviSupportedFeatures(const int32_t& nReqID)
{
    qDebug("[PlatformControllerQnx] getNaviSupportedFeatures %d", nReqID);

    emit PlatformGetNaviSupportedFeatures(nReqID);
}

void PlatformControllerQnx::onResponseGetNaviSupportedFeatures(int nReqID, NAVI_REQUEST_RESULT result, std::vector<std::string> supportedAPIsJson)
{
    qDebug("[PlatformControllerQnx] onResponseGetNaviSupportedFeatures() nReqID=%d, result=%d", nReqID, result);
    if(isLPMMode())
        return;

    E_NAVI_REQUEST_RESULT eResult = E_NAVI_REQUEST_RESULT_FAIL;
    if(NAVI_REQUEST_RESULT_DONE == result)
    {
        eResult = E_NAVI_REQUEST_RESULT_DONE;
    }

    m_pNaviDeploy->ongetNaviSupportedFeatures(nReqID, eResult, supportedAPIsJson);
}

void PlatformControllerQnx::getPOISAroundLocation(const int32_t& nReqID, const std::vector<std::string>& searchCategoryIDs, const NAVI_GEO_COORDINATE_T& locationData, const std::string& keyword)
{
    qDebug("[PlatformControllerQnx] getPOISAroundLocation %d", nReqID);

    NAVI_GEO_COORDINATE naviGeoCoordinate;
    memcpy(&naviGeoCoordinate,&locationData,sizeof(NAVI_GEO_COORDINATE_T));
    emit PlatformGetPOISAroundLocation(nReqID, searchCategoryIDs, naviGeoCoordinate, keyword);
}

void PlatformControllerQnx::onResponseGetPOISAroundLocation(int nReqID, NAVI_REQUEST_RESULT result, std::vector<NAVI_LOCATION_ADDRESS_COORDINATE> searchResults)
{
    qDebug("[PlatformControllerQnx] onResponseGetPOISAroundLocation() nReqID=%d, result=%d", nReqID, result);
    if(isLPMMode())
        return;
    std::vector<NAVI_LOCATION_ADDRESS_COORDINATE_T> stLocationAddressCoordnateList;
    E_NAVI_REQUEST_RESULT eResult = E_NAVI_REQUEST_RESULT_FAIL;
    if(NAVI_REQUEST_RESULT_DONE == result)
    {
        NAVI_LOCATION_ADDRESS_COORDINATE_T locationAssress;
        foreach (NAVI_LOCATION_ADDRESS_COORDINATE elem, searchResults) {
            locationAssress.address<<elem.address;
            locationAssress.locationName = elem.locationName;
            memcpy(&locationAssress.geoCoordinates, &elem.geoCoordinates, sizeof(NAVI_GEO_COORDINATE_T));
            stLocationAddressCoordnateList.push_back(locationAssress);
        }
        eResult = E_NAVI_REQUEST_RESULT_DONE;
    }
    m_pNaviDeploy->ongetPOISAroundLocation(nReqID, eResult, stLocationAddressCoordnateList);
}

void PlatformControllerQnx::getRecentDestinationList(const int32_t& nReqID)
{
    qDebug("[PlatformControllerQnx] getRecentDestinationList %d", nReqID);

    emit PlatformGetRecentDestinationList(nReqID);
}

void PlatformControllerQnx::onResponseGetRecentDestinationList(int nReqID, NAVI_REQUEST_RESULT result, std::vector<NAVI_WAYPOINT_INFO> recentDestinations)
{
    qDebug("[PlatformControllerQnx] onResponseGetRecentDestinationList() nReqID=%d, result=%d", nReqID, result);
    if(isLPMMode())
        return;

    std::vector<NAVI_WAYPOINT_INFO_T> stLocationAddressCoordnateList;
    NAVI_WAYPOINT_INFO_T stLocationAddressCoordnate;
    E_NAVI_REQUEST_RESULT eResult = E_NAVI_REQUEST_RESULT_FAIL;

    if(NAVI_REQUEST_RESULT_DONE == result)
    {
        foreach (NAVI_WAYPOINT_INFO elem, recentDestinations) {
            stLocationAddressCoordnate.locationData<<elem.locationData;
            stLocationAddressCoordnate.timeDistance<<elem.timeDistance;
            stLocationAddressCoordnate.waypointId = elem.waypointId;
            stLocationAddressCoordnateList.push_back(stLocationAddressCoordnate);
        }

        eResult = E_NAVI_REQUEST_RESULT_DONE;
    }
    m_pNaviDeploy->ongetRecentDestinationList(nReqID, eResult, stLocationAddressCoordnateList);
}

void PlatformControllerQnx::getWaypointData(const int32_t& nReqID, const std::string& waypointIndex)
{
    qDebug("[PlatformControllerQnx] getWaypointData %d", nReqID);
    emit PlatformGetWaypointData(nReqID, QString::fromStdString(waypointIndex).toInt());
}

void PlatformControllerQnx::onResponseGetWaypointData(int nReqID, NAVI_REQUEST_RESULT result, NAVI_WAYPOINT_INFO* waypoint)
{
    qDebug("[PlatformControllerQnx] onResponseGetWaypointData() nReqID=%d, result=%d", nReqID, result);
    if(isLPMMode())
        return;

    E_NAVI_REQUEST_RESULT eResult = E_NAVI_REQUEST_RESULT_FAIL;
    NAVI_WAYPOINT_INFO_T stWayPointInfo = {};
    if(NAVI_REQUEST_RESULT_DONE == result)
    {
        if(waypoint != NULL)
        {
            stWayPointInfo.locationData.locationName = waypoint->locationData.locationName;
            stWayPointInfo.locationData.address.addressString = waypoint->locationData.address.addressString;
            stWayPointInfo.locationData.address.addressCode = waypoint->locationData.address.addressCode;
            stWayPointInfo.locationData.address.countryCode = waypoint->locationData.address.countryCode;
            stWayPointInfo.locationData.address.country = waypoint->locationData.address.country;
            stWayPointInfo.locationData.address.state = waypoint->locationData.address.state;
            stWayPointInfo.locationData.address.stateCode = waypoint->locationData.address.stateCode;
            stWayPointInfo.locationData.address.city = waypoint->locationData.address.city;
            stWayPointInfo.locationData.address.county = waypoint->locationData.address.county;
            stWayPointInfo.locationData.address.province = waypoint->locationData.address.province;
            stWayPointInfo.locationData.address.zipCode = waypoint->locationData.address.zipCode;
            stWayPointInfo.locationData.address.district = waypoint->locationData.address.district;
            stWayPointInfo.locationData.address.street = waypoint->locationData.address.street;
            stWayPointInfo.locationData.address.blockNumber = waypoint->locationData.address.blockNumber;
            stWayPointInfo.locationData.address.houseNumber = waypoint->locationData.address.houseNumber;
            stWayPointInfo.locationData.address.telephone = waypoint->locationData.address.telephone;
            stWayPointInfo.locationData.geoCoordinates.latitude = waypoint->locationData.geoCoordinates.latitude;
            stWayPointInfo.locationData.geoCoordinates.longitude = waypoint->locationData.geoCoordinates.longitude;
            stWayPointInfo.locationData.geoCoordinates.altitude = waypoint->locationData.geoCoordinates.altitude;
            stWayPointInfo.timeDistance.eta = waypoint->timeDistance.eta;
            stWayPointInfo.timeDistance.tta = waypoint->timeDistance.tta;
            stWayPointInfo.timeDistance.distance = waypoint->timeDistance.distance;
        }

        eResult = E_NAVI_REQUEST_RESULT_DONE;
    }

    m_pNaviDeploy->ongetWaypointData(nReqID, eResult, stWayPointInfo);
}

void PlatformControllerQnx::setDestinationList(const int32_t& nReqID, const std::vector<NAVI_LOCATION_ADDRESS_COORDINATE_T>& destinationList, const bool& skipConfirmation)
{
    qDebug("[PlatformControllerQnx] setDestinationList %d", nReqID);

    QVector<NAVI_LOCATION_ADDRESS_COORDINATE> locationAddrCoorList;
    NAVI_LOCATION_ADDRESS_COORDINATE locationAddrCoor;
    foreach (NAVI_LOCATION_ADDRESS_COORDINATE_T destination, destinationList) {
        locationAddrCoor<<destination;
        locationAddrCoorList.push_back(locationAddrCoor);
    }
    emit PlatformSetDestinationList(nReqID, locationAddrCoorList);
}

void PlatformControllerQnx::onResponseSetDestinationList(int nReqID, NAVI_REQUEST_RESULT result)
{
    qDebug("[PlatformControllerQnx] onResponseSetDestinationList nReqID=%d, result=%d", nReqID, result);
    if(isLPMMode())
        return;

    m_pNaviDeploy->onsetDestinationList(nReqID, (E_NAVI_REQUEST_RESULT)result);
}

void PlatformControllerQnx::showLocationOnMap(const int32_t& nReqID, const std::vector<NAVI_GEO_COORDINATE_T>& locationList)
{
    qDebug("[PlatformControllerQnx] showLocationOnMap %d", nReqID);

    QVector<NAVI_GEO_COORDINATE> geoCoordinateList;
    NAVI_GEO_COORDINATE geoCoordinate;
    foreach (NAVI_GEO_COORDINATE_T elem, locationList) {
        memcpy(&geoCoordinate, &elem, sizeof(NAVI_GEO_COORDINATE_T));
        geoCoordinateList.push_back(geoCoordinate);
    }
    emit PlatformShowLocationOnMap(nReqID, geoCoordinateList);
}

void PlatformControllerQnx::onResponseShowLocationOnMap(int nReqID, NAVI_REQUEST_RESULT result)
{
    if(isLPMMode())
        return;

    E_NAVI_REQUEST_RESULT eResult = E_NAVI_REQUEST_RESULT_FAIL;

    if(NAVI_REQUEST_RESULT_DONE == result)
        eResult = E_NAVI_REQUEST_RESULT_DONE;

    m_pNaviDeploy->onshowLocationOnMap(nReqID, eResult);
}

void PlatformControllerQnx::subscribeDistanceETAChanged(const int32_t& nReqID)
{
    qDebug("[PlatformControllerQnx] subscribeDistanceETAChanged %d", nReqID);


    emit PlatformSubscribeDistanceETAChanged(nReqID);
}

void PlatformControllerQnx::onResponseSubscribeDistanceETAChanged(int nReqID, NAVI_REQUEST_RESULT result)
{
    if(isLPMMode())
        return;

    E_NAVI_REQUEST_RESULT eResult = E_NAVI_REQUEST_RESULT_FAIL;

    if(NAVI_REQUEST_RESULT_DONE == result)
        eResult = E_NAVI_REQUEST_RESULT_DONE;

    m_pNaviDeploy->onsubscribeDistanceETAChanged(nReqID, eResult);
}

void PlatformControllerQnx::subscribeRouteChanged(const int32_t& nReqID)
{
    qDebug("[PlatformControllerQnx] subscribeRouteChanged %d", nReqID);

    emit PlatformSubscribeRouteChanged(nReqID);
}

void PlatformControllerQnx::onResponseSubscribeRouteChanged(int nReqID, NAVI_REQUEST_RESULT result)
{
    E_NAVI_REQUEST_RESULT eResult = E_NAVI_REQUEST_RESULT_FAIL;

    if(NAVI_REQUEST_RESULT_DONE == result)
        eResult = E_NAVI_REQUEST_RESULT_DONE;

    m_pNaviDeploy->onsubscribeRouteChanged(nReqID, eResult);
}

void PlatformControllerQnx::unSubscribeDistanceETAChanged(const int32_t& nReqID)
{
    qDebug("[PlatformControllerQnx] unSubscribeDistanceChanged %d", nReqID);

    emit PlatformUnSubscribeDistanceETAChanged(nReqID);
}

void PlatformControllerQnx::onResponseUnSubscribeDistanceETAChanged(int nReqID, NAVI_REQUEST_RESULT result)
{
    qDebug("[PlatformControllerQnx] onResponseUnSubscribeDistanceETAChanged result = %d", result);
    if(isLPMMode())
        return;

    E_NAVI_REQUEST_RESULT eResult = E_NAVI_REQUEST_RESULT_FAIL;

    if(NAVI_REQUEST_RESULT_DONE == result)
        eResult = E_NAVI_REQUEST_RESULT_DONE;

    m_pNaviDeploy->onunSubscribeDistanceETAChanged(nReqID, eResult);

}

void PlatformControllerQnx::unSubscribeRouteChanged(const int32_t& nReqID)
{
    qDebug("[PlatformControllerQnx] unSubscribeRouteChanged %d", nReqID);

    emit PlatformUnSubscribeRouteChanged(nReqID);
//    int nResult = emit PlatformUnSubscribeRouteChanged(nReqID);

//    if(NAVI_REQUEST_RESULT_DONE == (NAVI_REQUEST_RESULT)nResult)
//    {
//        m_pNaviDeploy->onunSubscribeRouteChanged(nReqID, E_NAVI_REQUEST_RESULT_DONE);
//    }
//    else
//    {
//        m_pNaviDeploy->onunSubscribeRouteChanged(nReqID, E_NAVI_REQUEST_RESULT_FAIL);
//    }
}

void PlatformControllerQnx::onResponseUnSubscribeRouteChanged(int nReqID, NAVI_REQUEST_RESULT result)
{
    if(isLPMMode())
        return;

    E_NAVI_REQUEST_RESULT eResult = E_NAVI_REQUEST_RESULT_FAIL;

    if(NAVI_REQUEST_RESULT_DONE == result)
        eResult = E_NAVI_REQUEST_RESULT_DONE;

    m_pNaviDeploy->onunSubscribeRouteChanged(nReqID, eResult);
}

void PlatformControllerQnx::getMapSurface(const int32_t& nReqID, const NAVI_GET_MAP_SURFACE_T& options, const std::string& folderPath)
{
    qDebug()<<"PlatformControllerQnx::getMapSurface nReqID:"<<nReqID<<", options:"<<options.mapType<<", folderPath:"<<QString::fromStdString(folderPath);
    NAVI_GET_MAP_SURFACE option;
    option.b3DPerspective = options.b3DPerspective;
    option.mapType = (NAVI_MAP_TYPE)options.mapType;
    option.nightMode = options.nightMode;
    option.pixelHeight = options.pixelHeight;
    option.pixelWidth = options.pixelWidth;
    option.scale = options.scale;
    option.trafficEnabled = options.trafficEnabled;

    foreach (NAVI_ICON_INFO_T iconInfo, options.LocationsIcons) {
        NAVI_ICON_INFO icons;
        icons.cord<<iconInfo.cord;
        icons.icon_id = iconInfo.icon_id;
        icons.icon_path = iconInfo.icon_path;
        option.LocationsIcons.push_back(icons);
    }
    emit PlatformGetMapSurface(nReqID, option, folderPath);
}

void PlatformControllerQnx::onResponseGetMapSurface(int nReqID, NAVI_REQUEST_RESULT result, std::string filename)
{
    if(isLPMMode())
        return;

    if(NAVI_REQUEST_RESULT_DONE == (NAVI_REQUEST_RESULT)result)
    {
        m_pNaviDeploy->ongetMapSurface(nReqID, E_NAVI_REQUEST_RESULT_DONE, filename);
    }
    else
    {
       m_pNaviDeploy->ongetMapSurface(nReqID, E_NAVI_REQUEST_RESULT_FAIL, filename);
    }
}

void PlatformControllerQnx::calculateRouteInfo(const int32_t& nReqID, const NAVI_LOCATION_ADDRESS_COORDINATE_T& locationData)
{
    qWarning("PlatformControllerQnx::calculateRouteInfo %s", locationData.locationName.c_str());
    QSharedPointer <NAVI_LOCATION_ADDRESS_COORDINATE> plocationData(new NAVI_LOCATION_ADDRESS_COORDINATE);
    plocationData<<locationData;
    emit PlatformCalculateRouteInfo(nReqID, plocationData.data());
}

void PlatformControllerQnx::onResponseCalculateRouteInfo(int nReqID, NAVI_REQUEST_RESULT result, NAVI_WAYPOINT_INFO * routeInfo)
{
    if(isLPMMode())
        return;

    NAVI_WAYPOINT_INFO_T stWayPointInfo;

    if(NAVI_REQUEST_RESULT_DONE == (NAVI_REQUEST_RESULT)result && routeInfo != NULL)
    {
        stWayPointInfo.locationData<<routeInfo->locationData;
        stWayPointInfo.timeDistance<<routeInfo->timeDistance;
        stWayPointInfo.waypointId = routeInfo->waypointId;
        m_pNaviDeploy->oncalculateRouteInfo(nReqID, E_NAVI_REQUEST_RESULT_DONE, stWayPointInfo);
    }
    else
    {
        m_pNaviDeploy->oncalculateRouteInfo(nReqID, E_NAVI_REQUEST_RESULT_FAIL, stWayPointInfo);
    }
}
#endif

#ifdef PLATFORM_QNX // EB
void PlatformControllerQnx::onrequestSetNaviRouteToDestination(const int32_t& nReqID, const E_NAVI_REQUEST_RESULT& result)
{
    qDebug("[PlatformControllerQnx] onrequestSetNaviRouteToDestination result=%d", result);

    // This is a listener of requestSetNaviRouteToDestination.
    // When this listener is called, release shared memory.
    emit PlatformRequestSetNaviRouteToDestination();
}

void PlatformControllerQnx::onrequestNaviGuideStatusChanged(const int32_t& nReqID, const E_NAVI_REQUEST_RESULT& result)
{
    qDebug("[PlatformControllerQnx] onrequestNaviGuiideStatusChanged result=%d", result);

    // If result is DONE/OK, it should be implemented the following steps.
    // Next, translate route infor to OpenLR format
    // Write the route info to shared memory
    // Call requestSetNaviRouteToDestination
    // m_pNaviDeploy->requestSetNaviRouteToDestination();
    //
    // If not, nothing to do.
    if (E_NAVI_REQUEST_RESULT_DONE == result) {
        emit PlatformOnRequestNaviGuideStatusChanged();
    }
}

void PlatformControllerQnx::onrequestSetDatabaseUsable(const int32_t& nReqID, const E_NAVI_REQUEST_RESULT& result)
{
    qDebug("[PlatformControllerQnx] onrequestSetDatabaseUsable result=%d", result);

    emit PlatformResponseSetDatabaseUsable((NAVI_REQUEST_RESULT)result);
}

void PlatformControllerQnx::requestNaviGuideStatusChanged(bool bGuidanceRunning)
{
    qDebug("[PlatformControllerQnx] requestNaviGuideStatusChanged nReqID=%d, running=%d", NAVISERVICE_REQUEST_ID, bGuidanceRunning);

    if(isLPMMode())
        return;

    m_pNaviDeploy->requestNaviGuideStatusChanged(NAVISERVICE_REQUEST_ID, bGuidanceRunning);
}

void PlatformControllerQnx::requestSetNaviRouteToDestination()
{
    qDebug("[PlatformControllerQnx] requestSetNaviRouteToDestination nReqID=%d", NAVISERVICE_REQUEST_ID);

    if(isLPMMode())
        return;

    m_pNaviDeploy->requestSetNaviRouteToDestination(NAVISERVICE_REQUEST_ID);
}

void PlatformControllerQnx::requestSetDatabaseUsable(bool bBool)
{
    qDebug("[PlatformControllerQnx] requestSetDatabaseUsable nReqID=%d, bBool=%d", NAVISERVICE_REQUEST_ID, bBool);

    if(isLPMMode())
        return;

    m_pNaviDeploy->requestSetDatabaseUsable(NAVISERVICE_REQUEST_ID, bBool);
}

void PlatformControllerQnx::requestPartitionOptionChange(char* path, MountOption option)
{
    if(isLPMMode())
        return;

    if(m_PartitionCtrl == NULL)
        return;

    qDebug("[PlatformControllerQnx] requestPartitionOptionChange");
    m_PartitionCtrl->partitionCtrl_requestPartitionOptionChange(path, option);
}
#endif

#ifdef PLATFORM_QNX // Android Auto, CarPlay
void PlatformControllerQnx::setDestinationToHome(const int32_t& nReqID)
{
    qDebug("[PlatformControllerQnx] setDestinationToHome nReqID=%d", nReqID);

    emit PlatformSetDestinationToHome(nReqID);
}

void PlatformControllerQnx::onResponseSetDestinationToHome(int nReqID, NAVI_REQUEST_RESULT result)
{
    qDebug("[PlatformControllerQnx] onResponseSetDestinationToHome nReqID=%d, result=%d", nReqID, result);
    if(isLPMMode())
        return;

    m_pNaviDeploy->onsetDestinationToHome(nReqID, (E_NAVI_REQUEST_RESULT)result);
}

void PlatformControllerQnx::getCommuteSuggestions(const int32_t& nReqID)
{
    qDebug("[PlatformControllerQnx] getCommuteSuggestions");

    emit PlatformGetCommuteSuggestions(nReqID);
}

void PlatformControllerQnx::onResponseGetCommuteSuggestions(int nReqID, NAVI_REQUEST_RESULT result, std::vector<NAVI_COMMUTE_INFO> commuteList)
{
    qDebug("[PlatformControllerQnx] onResponseGetCommuteSuggestions");
    if(isLPMMode())
        return;

    std::vector<NAVI_COMMUTE_INFO_T> baseCommuteList;
    if(NAVI_REQUEST_RESULT_DONE == result)
    {
        baseCommuteList.reserve(commuteList.size());

        for (std::vector<NAVI_COMMUTE_INFO>::iterator it = commuteList.begin(); it!=commuteList.end(); ++it) {
            NAVI_COMMUTE_INFO item = *it;
            NAVI_COMMUTE_INFO_T commuteItem;
            memset(&commuteItem, 0x00, sizeof(NAVI_COMMUTE_INFO_T));
            commuteItem.listItemID  = item.listItemID;
            commuteItem.name        = item.name;
            commuteItem.ett         = item.ett;
            baseCommuteList.push_back(commuteItem);
        }
    }

    m_pNaviDeploy->ongetCommuteSuggestions(nReqID, (E_NAVI_REQUEST_RESULT)result, baseCommuteList);
}

void PlatformControllerQnx::setGoToNaviSearch(const int32_t& nReqID)
{
    qDebug("[PlatformControllerQnx] setGoToNaviSearch");

    emit PlatformSetGoToNaviSearch(nReqID);
}

void PlatformControllerQnx::onResponseSetGoToNaviSearch(int nReqID, const NAVI_REQUEST_RESULT result)
{
    qDebug("[PlatformControllerQnx] onResponseSetGoToNaviSearch");
    if(isLPMMode())
        return;

    m_pNaviDeploy->onsetGoToNaviSearch(nReqID, (E_NAVI_REQUEST_RESULT)result);
}

void PlatformControllerQnx::setCancelRoute(const int32_t& nReqID)
{
    qDebug("[PlatformControllerQnx] setCancelRoute");

    emit PlatformSetCancelRoute(nReqID);
}

void PlatformControllerQnx::onResponseSetCancelRoute(int nReqID, NAVI_REQUEST_RESULT result)
{
    qDebug("[PlatformControllerQnx] onResponseSetCancelRoute %d", result);
    if(isLPMMode())
        return;

    m_pNaviDeploy->onsetCancelRoute(nReqID, (E_NAVI_REQUEST_RESULT)result);
}

void PlatformControllerQnx::setGoToSetFavorites(const int32_t& nReqID)
{
    qDebug("[PlatformControllerQnx] setGoToSetFavoirtes nReqID=%d", nReqID);

    emit PlatformSetGoToSetFavorites(nReqID);
}

void PlatformControllerQnx::onResponseSetGoToSetFavorites(int nReqID, NAVI_REQUEST_RESULT result)
{
    qDebug("[PlatformControllerQnx] onResponseSetGoToSetFavorites nReqID=%d, result=%d", nReqID, result);
    if(isLPMMode())
        return;

    m_pNaviDeploy->onsetGoToSetFavorites(nReqID, (E_NAVI_REQUEST_RESULT)result);
}

void PlatformControllerQnx::setCommuteDestination(const int32_t& nReqID, const std::string& listItemID)
{
    qDebug("[PlatformControllerQnx] setCommuteDestination nReqID=%d, listItemID=%s", nReqID, listItemID);

    emit PlatformSetCommuteDestination(nReqID, listItemID);
}

void PlatformControllerQnx::onResponseSetCommuteDestination(int nReqID, NAVI_REQUEST_RESULT result)
{
    qDebug("[PlatformControllerQnx] onResponseSetCommuteDestination nReqID=%d, result=%d", nReqID, result);
    if(isLPMMode())
        return;

    if (result == NAVI_REQUEST_RESULT_DONE)
        requestGoNavi(SHOW_OPT_ADD);

    m_pNaviDeploy->onsetCommuteDestination(nReqID, (E_NAVI_REQUEST_RESULT)result);
}

void PlatformControllerQnx::setNaviMuteStatus(const int32_t& nReqID, const bool& bMuteStatus)
{
    qDebug("[PlatformControllerQnx] setNaviMuteStatus");

    emit PlatformSetNaviMuteStatus(nReqID, bMuteStatus);
}

void PlatformControllerQnx::onResponseSetNaviMuteStatus(int nReqID, NAVI_REQUEST_RESULT result, bool bMuteStatus)
{
    qDebug("[PlatformControllerQnx] onResponseSetNaviMuteStatus result=%d, mute=%d", result, bMuteStatus);
    if(isLPMMode())
        return;

    m_pNaviDeploy->onsetNaviMuteStatus(nReqID, (E_NAVI_REQUEST_RESULT)result, bMuteStatus);
}

void PlatformControllerQnx::onNotifyNaviGuideStatusChanged(bool bGuidanceRunning)
{
    qDebug("[PlatformControllerQnx] onNotifyNaviGuideStatusChanged rgrunning=%d", bGuidanceRunning);
    if(isLPMMode())
        return;

    m_pNaviDeploy->onNotifyNaviGuideStatusChanged(bGuidanceRunning);
}

void PlatformControllerQnx::onNotifyNaviTBTInfoChanged(NAVI_TBT_TYPE turnIcon, std::string roadname, int distance, int roundaboutExitNum, int countDownBar)
{
    qDebug("[PlatformControllerQnx] onNotifyNaviTBTInfoChanged turnicon=%d, roadname=%s, distance=%d", turnIcon, roadname, distance);
    if(isLPMMode())
        return;

    m_pNaviDeploy->onNotifyNaviTBTInfoChanged((E_NAVI_TBT_TYPE)turnIcon, roadname, distance, 0, 0);
}

void PlatformControllerQnx::onNotifyNextDestinationInfoChanged(NAVI_ETA_TTA_DISTANCE etaDistance, std::string title)
{
    qDebug("[PlatformControllerQnx] onNotifyNextDestinationInfoChanged %d, %s", etaDistance.distance, title);
    if(isLPMMode())
        return;

    NAVI_ETA_TTA_DISTANCE_T distance;
    distance.eta = etaDistance.eta;
    distance.tta = etaDistance.tta;
    distance.distance = etaDistance.distance;

    m_pNaviDeploy->onNotifyNextDestinationInfoChanged(distance, title);
}

void PlatformControllerQnx::onNotifyRouteProgressBarInfoChanged(std::vector<NAVI_HOMETILE_PROGRESS_SECTION> progressSectionList)
{
    qDebug("[PlatformControllerQnx] onNotifyRouteProgressBarInfoChanged");
    if(isLPMMode())
        return;

    std::vector<NAVI_HOMETILE_PROGRESS_SECTION_T> progressList;
    progressList.reserve(progressSectionList.size());

    for (std::vector<NAVI_HOMETILE_PROGRESS_SECTION>::iterator it = progressSectionList.begin(); it!=progressSectionList.end(); ++it) {
        NAVI_HOMETILE_PROGRESS_SECTION item = *it;
        NAVI_HOMETILE_PROGRESS_SECTION_T progress;

        progress.sectionType = (E_NAVI_HOMETILE_PROGRESS_TYPE)item.sectionType;
        progress.startPoint = item.startPoint;   //0~1000 total 1000
        progress.endPoint = item.endPoint;   //0~1000 total 1000

        progressList.push_back(progress);
    }
    m_pNaviDeploy->onNotifyRouteProgressBarInfoChanged(progressList);
}

void PlatformControllerQnx::onNotifyNaviMuteStatusChanged(bool bNaviMute)
{
    qDebug("[PlatformControllerQnx] onNotifyNaviMuteStatusChanged %d", bNaviMute);
    if(isLPMMode())
        return;

    m_pNaviDeploy->onNotifyNaviMuteStatusChanged(bNaviMute);
}

void PlatformControllerQnx::onNotifyNaviServiceAvailableChanged(bool bNaviAvailable)
{
    qDebug("[PlatformControllerQnx] onNotifyNaviServiceAvailableChanged %d", bNaviAvailable);
    if(isLPMMode())
        return;

    m_pNaviDeploy->onNotifyNaviServiceAvailableChanged(bNaviAvailable);
}

void PlatformControllerQnx::onNotifyNaviCommuteStatus(NAVI_COMMUTE_STATUS commuteStatus, bool bCommuteRunning)
{
    qDebug("[PlatformControllerQnx] onNotifyNaviCommuteStatus count(%d), running(%d)", commuteStatus, bCommuteRunning);
    if(isLPMMode())
        return;

    m_pNaviDeploy->onNotifyNaviCommuteStatus((E_NAVI_COMMUTE_STATUS)commuteStatus, bCommuteRunning);
}

void PlatformControllerQnx::onNotifyNaviCommuteInfoChanged(std::vector<NAVI_HOMETILE_POPOVER_ITEM> commuteList)
{
    qDebug("[PlatformControllerQnx] onNotifyNaviCommuteInfoChanged");
    if(isLPMMode())
        return;
}

void PlatformControllerQnx::onNotifyNaviFavoritesStatusChnaged(bool bHomeSetStatus, bool bWorkSetStatus)
{
    qDebug("[PlatformControllerQnx] onNotifyNaviFavoritesStatusChnaged home(%d), work(%d)", bHomeSetStatus, bWorkSetStatus);
    if(isLPMMode())
        return;

    m_pNaviDeploy->onNotifyNaviFavoritesStatusChnaged(bHomeSetStatus, bWorkSetStatus);
}

void PlatformControllerQnx::onNotifyDistanceETAChanged(std::vector<NAVI_WAYPOINT_INFO> waypointList)
{
    qDebug("[PlatformControllerQnx] onNotifyDistanceETAChanged");
    if(isLPMMode())
        return;

    std::vector<NAVI_WAYPOINT_INFO_T> wayList;
    wayList.reserve(waypointList.size());

    for (std::vector<NAVI_WAYPOINT_INFO>::iterator it = waypointList.begin(); it!=waypointList.end(); ++it) {
        NAVI_WAYPOINT_INFO item = *it;
        NAVI_WAYPOINT_INFO_T waypoint;
        waypoint.timeDistance.eta = item.timeDistance.eta;
        waypoint.timeDistance.tta = item.timeDistance.tta;
        waypoint.timeDistance.distance = item.timeDistance.distance;
        waypoint.locationData.locationName = item.locationData.locationName;
        waypoint.locationData.address.addressString = item.locationData.address.addressString;
        waypoint.locationData.address.addressCode = item.locationData.address.addressCode;
        waypoint.locationData.address.countryCode = item.locationData.address.countryCode;
        waypoint.locationData.address.country = item.locationData.address.country;
        waypoint.locationData.address.state = item.locationData.address.state;
        waypoint.locationData.address.stateCode = item.locationData.address.stateCode;
        waypoint.locationData.address.city = item.locationData.address.city;
        waypoint.locationData.address.county = item.locationData.address.country;
        waypoint.locationData.address.province = item.locationData.address.province;
        waypoint.locationData.address.zipCode = item.locationData.address.zipCode;
        waypoint.locationData.address.district = item.locationData.address.district;
        waypoint.locationData.address.street = item.locationData.address.street;
        waypoint.locationData.address.blockNumber = item.locationData.address.blockNumber;
        waypoint.locationData.address.houseNumber = item.locationData.address.houseNumber;
        waypoint.locationData.address.telephone = item.locationData.address.telephone;
        waypoint.locationData.geoCoordinates.latitude = item.locationData.geoCoordinates.latitude;
        waypoint.locationData.geoCoordinates.longitude = item.locationData.geoCoordinates.longitude;
        waypoint.locationData.geoCoordinates.altitude = item.locationData.geoCoordinates.altitude;
        wayList.push_back(waypoint);
    }
    m_pNaviDeploy->onNotifyDistanceETAChanged(wayList);
}

void PlatformControllerQnx::onNotifyNaviRouteChanged(int nReqID, NAVI_REQUEST_RESULT result, std::vector<NAVI_WAYPOINT_INFO> waypointList)
{
    qDebug("[PlatformControllerQnx] onNotifyNaviRouteChanged");
    if(isLPMMode())
        return;

    std::vector<NAVI_WAYPOINT_INFO_T> wayList;
    wayList.reserve(waypointList.size());

    for (std::vector<NAVI_WAYPOINT_INFO>::iterator it = waypointList.begin(); it!=waypointList.end(); ++it) {
        NAVI_WAYPOINT_INFO item = *it;
        NAVI_WAYPOINT_INFO_T waypoint;
        waypoint.timeDistance.eta = item.timeDistance.eta;
        waypoint.timeDistance.tta = item.timeDistance.tta;
        waypoint.timeDistance.distance = item.timeDistance.distance;
        waypoint.locationData.locationName = item.locationData.locationName;
        waypoint.locationData.address.addressString = item.locationData.address.addressString;
        waypoint.locationData.address.addressCode = item.locationData.address.addressCode;
        waypoint.locationData.address.countryCode = item.locationData.address.countryCode;
        waypoint.locationData.address.country = item.locationData.address.country;
        waypoint.locationData.address.state = item.locationData.address.state;
        waypoint.locationData.address.stateCode = item.locationData.address.stateCode;
        waypoint.locationData.address.city = item.locationData.address.city;
        waypoint.locationData.address.county = item.locationData.address.country;
        waypoint.locationData.address.province = item.locationData.address.province;
        waypoint.locationData.address.zipCode = item.locationData.address.zipCode;
        waypoint.locationData.address.district = item.locationData.address.district;
        waypoint.locationData.address.street = item.locationData.address.street;
        waypoint.locationData.address.blockNumber = item.locationData.address.blockNumber;
        waypoint.locationData.address.houseNumber = item.locationData.address.houseNumber;
        waypoint.locationData.address.telephone = item.locationData.address.telephone;
        waypoint.locationData.geoCoordinates.latitude = item.locationData.geoCoordinates.latitude;
        waypoint.locationData.geoCoordinates.longitude = item.locationData.geoCoordinates.longitude;
        waypoint.locationData.geoCoordinates.altitude = item.locationData.geoCoordinates.altitude;
        wayList.push_back(waypoint);
    }
    m_pNaviDeploy->onNotifyNaviRouteChanged((E_NAVI_REQUEST_RESULT)result);
}

void PlatformControllerQnx::onNotifyNaviMapAdasDataNoUpdate()
{
    qDebug("[PlatformControllerQnx] onNotifyNaviMapAdasDataNoUpdate");
    if(isLPMMode())
        return;

    m_pNaviDeploy->onNotifyNaviMapAdasDataNoUpdate();
}

void PlatformControllerQnx::onNotifyNaviMapAdasDataUpdate()
{
    qDebug("[PlatformControllerQnx] onNotifyNaviMapAdasDataUpdate");
    if(isLPMMode())
        return;

    m_pNaviDeploy->onNotifyNaviMapAdasDataUpdate();
}

void PlatformControllerQnx::onNotifyNaviMapAdasDataFinishInstallData()
{
    qDebug("[PlatformControllerQnx] onNotifyNaviMapAdasDataFinishInstallData");
    if(isLPMMode())
        return;

    m_pNaviDeploy->onNotifyNaviMapAdasDataFinishInstallData();
}

void PlatformControllerQnx::onNotifyNaviCurrentRoadInfoChanged(std::string currentRoad, std::string currentTown)
{
    if(isLPMMode())
        return;

    m_pNaviDeploy->onNotifyNaviCurrentRoadInfoChanged(currentRoad, currentTown);
}

void PlatformControllerQnx::onNotifyNaviCameraInfoChanged(std::string cameraTitle, int distance, bool bVisible)
{
//    qDebug("[PlatformControllerQnx] onNotifyNaviCameraInfoChanged : %s, %d", cameraTitle, bVisible);

    if(isLPMMode())
        return;

    m_pNaviDeploy->onNotifyNaviCameraInfoChanged(cameraTitle, distance, bVisible);
}

void PlatformControllerQnx::onNotifyPopOverItemInfoChanged(NAVI_HOMETILE_POPOVER_TYPE popOverType, std::vector<NAVI_HOMETILE_POPOVER_ITEM> popOverItemList)
{
    qDebug("[PlatformControllerQnx] onNotifyPopOverItemInfoChanged : %d, %d", popOverType, popOverItemList.size());

    if(isLPMMode())
        return;
}

#endif

#ifdef PLATFORM_QNX // VR APIs
void PlatformControllerQnx::getAddressInfoRequest(const int32_t nReqID)
{
    qDebug("[PlatformControllerQnx] getAddressInfoRequest");

    if(isLPMMode())
        return;

    ESpeechCallStatus aCallStatus = eSpeechCallSuccess;
    ESpeechAddressInfo aAddressInfo = eStateAddressCityFirst;

    qDebug("[PlatformControllerQnx] getAddressInfoResponse(%d, %d, %d)", nReqID, aCallStatus, aAddressInfo);
    m_pNaviDeploy->getAddressInfoResponse(nReqID, aCallStatus, aAddressInfo);
}

void PlatformControllerQnx::getAlphabetRequest(const int32_t nReqID)
{
    qDebug("[PlatformControllerQnx] getAlphabetRequest");

    if(isLPMMode())
        return;

    ESpeechCallStatus aCallStatus = eSpeechCallSuccess;
    ESpeechAlphabet aAlphaEnum = eAlphabetOthers;

    qDebug("[PlatformControllerQnx] getAlphabetResponse(%d, %d, %d)", nReqID, aCallStatus, aAlphaEnum);
    m_pNaviDeploy->getAlphabetResponse(nReqID, aCallStatus, aAlphaEnum);
}

void PlatformControllerQnx::getSelectedCharacterRequest(const int32_t nReqID)
{
    qDebug("[PlatformControllerQnx] getSelectedCharacterRequest");

    if(isLPMMode())
        return;

    emit PlatformGetSelectedCharacter(nReqID);

    //<.. for vr testing
    ESpeechCallStatus aCallStatus = eSpeechCallSuccess;
    std::string aSelectedChar = "";

    qDebug("[PlatformControllerQnx] getSelectedCharacterResponse(%d, %d, %s)", nReqID, aCallStatus, aSelectedChar.c_str());
    m_pNaviDeploy->getSelectedCharacterResponse(nReqID, aCallStatus, aSelectedChar);
    //..>
}

void PlatformControllerQnx::onGetSelectedCharacter(int nReqID, NAVI_REQUEST_RESULT result, std::string aSelectedChar)
{
    qDebug("[PlatformControllerQnx] onGetSelectedCharacter reqID=%d, result=%d, string=%s",
           nReqID, result, aSelectedChar.c_str());

    ESpeechCallStatus aCallStatus = eSpeechCallSuccess;
    if (NAVI_REQUEST_RESULT_DONE != result) {
        aCallStatus = eSpeechCallFailure;
    }

    m_pNaviDeploy->getSelectedCharacterResponse(nReqID, aCallStatus, aSelectedChar);
}

void PlatformControllerQnx::getCountryInfoRequest(const int32_t nReqID)
{
#ifdef PLATFORM_QNX
    kpiFinishMeasurement(KPI_LANGUAGE_SWITCH, "Tmap LangSwitch");
#endif
    qDebug("[PlatformControllerQnx] getCountryInfoRequest");

    if(isLPMMode())
        return;

    ESpeechCallStatus aCallStatus = eSpeechCallSuccess;
    bool aHasCountries = false;

    qDebug("[PlatformControllerQnx] getCountryInfoResponse(%d, %d, %d)", nReqID, aCallStatus, aHasCountries);
    m_pNaviDeploy->getCountryInfoResponse(nReqID, aCallStatus, aHasCountries);
}

void PlatformControllerQnx::getDestinationExistsCheckRequest(const int32_t nReqID)
{
    qDebug("[PlatformControllerQnx] getDestinationExistsCheck");

    emit PlatformGetDestinationExistsCheckRequest(nReqID);
}

void PlatformControllerQnx::onGetDestinationExistsCheckResponse(int nReqID, NAVI_REQUEST_RESULT result, bool aDestinationExists)
{
    ESpeechCallStatus aCallStatus = eSpeechCallSuccess;

    if (result != NAVI_REQUEST_RESULT_DONE) {
        aCallStatus = eSpeechCallFailure;
    }

    qDebug("[PlatformControllerQnx] getDestinationExistsCheckResponse(%d, %d, %d)", nReqID, aCallStatus, aDestinationExists);
    m_pNaviDeploy->getDestinationExistsCheckResponse(nReqID, aCallStatus, aDestinationExists);
}

void PlatformControllerQnx::getSpeechAddressUIDListRequest(const int32_t nReqID, const EAddrType aAddrType)
{
    qDebug("[PlatformControllerQnx] getSpeechAddressUIDListRequest %d", aAddrType);
    if(isLPMMode())
        return;

    ESpeechCallStatus aCallStatus = eSpeechCallSuccess;
    uint32_t aNoOfEntries = 0;
    std::string aFilename = "";

    switch(aAddrType) {
    case eAddrTypeHome:                ///< type is Home
        emit PlatformGetSpeechAddressUIDListHome(nReqID);
        return;
        break;
    case eAddrTypePrevDest:            ///< type is a previous destination
        emit PlatformGetSpeechAddressUIDListPrevDest(nReqID);
        return;
        break;
    case eAddrTypeLocationInput:       ///< type is a location input
    case eAddrTypePhoneNumber:         ///< type is a phone number
    case eAddrTypePOI:                 ///< type is a POI
    case eAddrTypeAny:                 ///< type is anything else
    case eAddrTypeMax:                 ///< last element indicator
    default:
        break;
    }

    //<.. for vr testing
    qDebug("[PlatformControllerQnx] getSpeechAddressUIDListResponse(%d, %d, %d, %s)",
           nReqID, aCallStatus, aNoOfEntries, aFilename.c_str());
    m_pNaviDeploy->getSpeechAddressUIDListResponse(nReqID, aCallStatus, aNoOfEntries, aFilename);
    //..>
}

void PlatformControllerQnx::onGetSpeechAddressUIDList(int nReqID, NAVI_REQUEST_RESULT result, uint32_t aNoOfEntries, string aFileName)
{
    qDebug("[PlatformControllerQnx] onGetSpeechAddressUIDList reqID=%d, result=%d, count=%d, file=%s",
           nReqID, result, aNoOfEntries, aFileName.c_str());

    ESpeechCallStatus aCallStatus = eSpeechCallSuccess;
    if (NAVI_REQUEST_RESULT_DONE != result) {
        aCallStatus = eSpeechCallFailure;
    }

    m_pNaviDeploy->getSpeechAddressUIDListResponse(nReqID, aCallStatus, aNoOfEntries, aFileName);
}

void PlatformControllerQnx::setSpeechContextAsDestinationRequest(const int32_t nReqID, const EAddrType aAddrType)
{
    qDebug("[PlatformControllerQnx] setSpeechContextAsDestinationRequest %d", aAddrType);
    if(isLPMMode())
        return;

    ESpeechCallStatus aCallStatus = eSpeechCallSuccess;

    switch(aAddrType) {
    case eAddrTypeHome:
        emit PlatformSetHomeAsDestination(nReqID);
        return;
        break;
    case eAddrTypePrevDest:
        emit PlatformSetPrevDestAsDestination(nReqID);
        break;
    case eAddrTypePOI:
        emit PlatformSetPoiAsDestination(nReqID);
        break;
    default:
        break;
    }

    //<.. for vr testing
//    qDebug("[PlatformControllerQnx] setSpeechContextAsDestinationResponse(%d, %d)", nReqID, aCallStatus);
//    m_pNaviDeploy->setSpeechContextAsDestinationResponse(nReqID, aCallStatus);
    //..>
}

void PlatformControllerQnx::onSetHomeAsDestination(int nReqID, NAVI_REQUEST_RESULT result)
{
    qDebug("[PlatformControllerQnx] onSetHomeAsDestination reqID=%d, result=%d", nReqID, result);

    ESpeechCallStatus aCallStatus = eSpeechCallSuccess;
    if (NAVI_REQUEST_RESULT_DONE != result) {
        aCallStatus = eSpeechCallFailure;
    }

    qDebug("[PlatformControllerQnx] setSpeechContextAsDestinationResponse(%d, %d)", nReqID, aCallStatus);
    m_pNaviDeploy->setSpeechContextAsDestinationResponse(nReqID, aCallStatus);
}

void PlatformControllerQnx::onSetPrevDestAsDestination(int nReqID, NAVI_REQUEST_RESULT result)
{
    qDebug("[PlatformControllerQnx] onSetPrevDestAsDestination reqID=%d, result=%d", nReqID, result);

    ESpeechCallStatus aCallStatus = eSpeechCallSuccess;
    if (NAVI_REQUEST_RESULT_DONE != result) {
        aCallStatus = eSpeechCallFailure;
    }

    qDebug("[PlatformControllerQnx] setSpeechContextAsDestinationResponse(%d, %d)", nReqID, aCallStatus);
    m_pNaviDeploy->setSpeechContextAsDestinationResponse(nReqID, aCallStatus);
}

void PlatformControllerQnx::onSetPoiAsDestination(int nReqID, NAVI_REQUEST_RESULT result)
{
    qDebug();

    ESpeechCallStatus aCallStatus = eSpeechCallSuccess;
    if (NAVI_REQUEST_RESULT_DONE != result) {
        aCallStatus = eSpeechCallFailure;
    }

    qDebug("[PlatformControllerQnx] setSpeechContextAsDestinationResponse(%d, %d)", nReqID, aCallStatus);
    m_pNaviDeploy->setSpeechContextAsDestinationResponse(nReqID, aCallStatus);
}

void PlatformControllerQnx::isSpeechAddrUniqueRequest(const int32_t nReqID, const EAddrType& aContext)
{
    qDebug("[PlatformControllerQnx] isSpeechAddrUniqueRequest %d", aContext);
    if(isLPMMode())
        return;

    ESpeechCallStatus aCallStatus = eSpeechCallSuccess;
    bool aIsUnique = false;

    switch(aContext) {
    case eAddrTypeHome:
        aIsUnique = true;
        break;
    case eAddrTypePrevDest:
        aIsUnique = true;
        break;
    case eAddrTypePOI:
        aIsUnique = true;
        break;
    default:
        break;
    }

    qDebug("[PlatformControllerQnx] isSpeechAddrUniqueResponse(%d, %d, %d)", nReqID, aCallStatus, aIsUnique);
    m_pNaviDeploy->isSpeechAddrUniqueResponse(nReqID, aCallStatus, aIsUnique);
}

void PlatformControllerQnx::getDBIDFileRequest(const int32_t nReqID, const std::vector<std::string> aStrDBIDList, const std::string aStrLang, const bool aWithPhonetics)
{
    qDebug("[PlatformControllerQnx] getDBIDFileRequest reqID=%d, strlist.size=%d, lang=%s, phone=%d",
           nReqID, (int)aStrDBIDList.size(), aStrLang.c_str(), aWithPhonetics);

    std::vector<std::string> aStrList = aStrDBIDList;
    for (std::vector<std::string>::iterator it = aStrList.begin(); it != aStrList.end(); ++it)
    {
        std::string item = *it;
        qDebug("  -> aStrDBID=%s", item.c_str());
    }

    if(isLPMMode())
        return;

    emit PlatformGetDBIDFileRequest(nReqID, aStrDBIDList);

}

void PlatformControllerQnx::onGetDBIDFileResponse(int nReqID, NAVI_REQUEST_RESULT result, uint aNoOfEntries, string aFileName)
{
    qDebug("[PlatformControllerQnx] onGetDBIDFileResponse reqID=%d, result=%d, count=%d, file=%s", nReqID, result, aNoOfEntries, aFileName.c_str());

    ESpeechCallStatus aCallStatus = eSpeechCallSuccess;
    if (NAVI_REQUEST_RESULT_DONE != result) {
        aCallStatus = eSpeechCallFailure;
    }

    m_pNaviDeploy->getDBIDFileResponse(nReqID, aCallStatus, aNoOfEntries, aFileName);
}

void PlatformControllerQnx::getEcoFileRequest(const int32_t nReqID, const SEcoFile& aFileInfo)
{
    qDebug("[PlatformControllerQnx] getEcoFileRequest eAddrElemType=%d, eInputMode=%d, strLanguage=%s, aAddrData.size()=%d",
           aFileInfo.m_eReqAddrElemType, aFileInfo.m_eInputMode, aFileInfo.m_strReqLanguage.c_str(), (int)aFileInfo.m_aAddrData.size());

    std::vector<SAddressData> aAddrData = aFileInfo.m_aAddrData;
    for (std::vector<SAddressData>::iterator it = aAddrData.begin(); it != aAddrData.end(); ++it)
    {
        SAddressData item = *it;
        qDebug("  -> m_eAddrElemType=%d, m_strAddrElemVal=%s", item.m_eAddrElemType, item.m_strAddrElemVal.c_str());
    }

    if(isLPMMode())
        return;

    ESpeechCallStatus aCallStatus = eSpeechCallSuccess;

    qDebug("[PlatformControllerQnx] getEcoFileResponse(%d, %d)", nReqID, aCallStatus);
    m_pNaviDeploy->getEcoFileResponse(nReqID, aCallStatus);
}

void PlatformControllerQnx::getEcoFileInfoRequest(const int32_t nReqID, const SEcoFileInfo& aFileInfo)
{
    qDebug("[PlatformControllerQnx] getEcoFileInfoRequest eAddrElemType=%d, eInputMode=%d, aAddrData.size()=%d",
           aFileInfo.m_eReqAddrElemType, aFileInfo.m_eInputMode, (int)aFileInfo.m_aAddrData.size());

    std::vector<SAddressData> aAddrData = aFileInfo.m_aAddrData;
    for (std::vector<SAddressData>::iterator it = aAddrData.begin(); it != aAddrData.end(); ++it)
    {
        SAddressData item = *it;
        qDebug("  -> m_eAddrElemType=%d, m_strAddrElemVal=%s", item.m_eAddrElemType, item.m_strAddrElemVal.c_str());
    }

    if(isLPMMode())
        return;

    ESpeechCallStatus aCallStatus = eSpeechCallSuccess;
    SEcoFileInfoRet aRetData;
    memset(&aRetData, 0x00, sizeof(SEcoFileInfoRet));
    aRetData.m_strLanguageList.push_back("ko_KR");
    aRetData.m_eFileType    = eFileTypeOffline;
    aRetData.m_nNoOfEntries = 0;
    aRetData.m_bAvail       = false;

    m_nCurrEcoElemType = -1;
    switch(aFileInfo.m_eReqAddrElemType)
    {
    case eAddrElemCity:                    ///< map to Speech: LOCATION_TYPE_CITY
    case eAddrElemCountry:                 ///< map to Speech: LOCATION_TYPE_COUNTRY
    case eAddrElemHouseNo:                 ///< map to Speech: LOCATION_TYPE_HOUSENR
    case eAddrElemIntersection:            ///< map to Speech: LOCATION_TYPE_INTERSECTION
    case eAddrElemNoRefine:                ///< map to Speech: LOCATION_TYPE_NO_REFINEMENT
    case eAddrElemOneshotCollapsed:        ///< map to Speech: LOCATION_TYPE_ONESHOT_COLLAPSED
    case eAddrElemOneshotUnique:           ///< map to Speech: LOCATION_TYPE_ONESHOT_UNIQUE
        break;
    case eAddrElemPoiCat:                  ///< map to Speech: LOCATION_TYPE_POI_BRAND_AND_CATEGORY
        m_nCurrEcoElemType = eAddrElemPoiCat;
        aRetData.m_nNoOfEntries = 1;        ///< number of elements in eco file
        aRetData.m_bAvail       = true;    ///< file available true|false
        break;
    case eAddrElemRefine:                  ///< map to Speech: LOCATION_TYPE_REFINEMENT
    case eAddrElemState:                   ///< map to Speech: LOCATION_TYPE_STATE
    case eAddrElemStreet:                  ///< map to Speech: LOCATION_TYPE_STREET
    case eAddrElemZip:                     ///< map to Speech: LOCATION_TYPE_POSTALCODE
    case eAddrElemOneshot:                 ///< map to Speech: LOCATION_TYPE_ONESHOT
        break;
    case eAddrElemRegionsCop:              ///< map to Speech: LOCATION_TYPE_REGIONS_COP_FILE
        m_nCurrEcoElemType = eAddrElemRegionsCop;
        aRetData.m_nNoOfEntries = 1;        ///< number of elements in eco file
        aRetData.m_bAvail       = true;    ///< file available true|false
        break;
    case eAddrElemPoiName:                 ///< map to Speech: LOCATION_TYPE_POI_NAME
        break;
    default:
        break;
    }

    qDebug("[PlatformControllerQnx] getEcoFileInfoResponse(%d, %d, aRetData) entries=%d, avail=%d",
           nReqID, aCallStatus, aRetData.m_nNoOfEntries, aRetData.m_bAvail);
    m_pNaviDeploy->getEcoFileInfoResponse(nReqID, aCallStatus, aRetData);
}

void PlatformControllerQnx::getEcoFileStatusRequest(const int32_t nReqID)
{
    qDebug("[PlatformControllerQnx] getEcoFileStatusRequest");

    if(isLPMMode())
        return;

    ESpeechCallStatus aCallStatus = eSpeechCallSuccess;
    EFileStatus eFileStatus = eFileStatusAvail;
    std::string aFilename = "";
    if (m_nCurrEcoElemType == eAddrElemPoiCat) {
        aFilename = "/ivimap/SKT/map_root/asr_KOR/contexts/KOK/P_fi_dataset_KOR_field.odl.fcf";
    }
    else if (m_nCurrEcoElemType == eAddrElemRegionsCop) {
        //aFilename = "/ivimap/SKT/map_root/asr_KOR/contexts/KOK/45[n]_fi_inc_country_KR_field.odl.fcf";
        QString copPath = QStringLiteral("/ivinusr/tmp/VDE_REGION.cop");

        if (QFile::exists(copPath))  {
            QFile::remove(copPath);
        }

        QFile copfile(copPath);
        if (copfile.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&copfile);
            out << QStringLiteral("#ECO V1.3 UTF-8;\n")
                << QStringLiteral("#ECOScheme COP1 V1.6\n")
                << QStringLiteral("#EDBTableUpdateMode AddEntryMode\n")
                << QStringLiteral("#EDBDatabaseID KR\n\n")
                << QStringLiteral("#Column1 Entry_ID\n")
                << QStringLiteral("#Column2 Entry_Ortho\n")
                << QStringLiteral("#Column3 Entry_Pho\n")
                << QStringLiteral("#Column4 Context_Location\n")
                << QStringLiteral("#Column5 Backward_Context_Location\n\n")
                << QStringLiteral("#Cardin(Entry_ID) 1\n")
                << QStringLiteral("#Format(Entry_ID) Ident\n")
                << QStringLiteral("#NumIDFormat(Entry_ID) Decimal\n")
                << QStringLiteral("#NumIDBinLength(Entry_ID) 64\n")
                << QStringLiteral("#LangIdScheme(Entry_Pho) Nuance_LLC\n")
                << QStringLiteral("#PhoneticAlphabet(Entry_Pho) LH_Plus\n")
                << QStringLiteral("#PhoneticLangSpecs(Entry_Pho) KOR 0.0\n\n")
//                << QStringLiteral("1000000;\"대한민국\";\"\";\"\";\"/ivimap/SKT/map_root/asr_KOR/contexts/KOK/45[n]_fi_inc_country_KR_field.odl.fcf\"\n");
                << QStringLiteral("0;\"대한민국\";\"\";\"/ivimap/SKT/map_root/asr_KOR/contexts/KOK/45[n]_fi_inc_country_KR_field.odl.fcf\";\"\"\n");
                copfile.close();
        }
        aFilename = copPath.toStdString();
    }

    qDebug("[PlatformControllerQnx] getEcoFileStatusResponse(%d, %d, %s, %d)", nReqID, aCallStatus, aFilename.c_str(), eFileStatus);
    m_pNaviDeploy->getEcoFileStatusResponse(nReqID, aCallStatus, aFilename, eFileStatus);
}

void PlatformControllerQnx::waitForEcoFileRequest(const int32_t nReqID)
{
    qDebug("[PlatformControllerQnx] waitForEcoFileRequest");

    if(isLPMMode())
        return;

    ESpeechCallStatus aCallStatus = eSpeechCallSuccess;
    std::string aFilename = "";

    qDebug("[PlatformControllerQnx] waitForEcoFileResponse(%d, %d, %s)", nReqID, aCallStatus, aFilename.c_str());
    m_pNaviDeploy->waitForEcoFileResponse(nReqID, aCallStatus, aFilename);
}

void PlatformControllerQnx::getGuidanceStatusRequest(const int32_t nReqID)
{
    qDebug("[PlatformControllerQnx] getGuidanceStatusRequest");

    emit PlatformGetGuidanceStatus(nReqID);
}

void PlatformControllerQnx::onResponseGetGuidanceStatus(int nReqID, NAVI_REQUEST_RESULT result, bool bGuidance)
{
    qDebug("[PlatformControllerQnx] onResponseGetGuidanceStatus result=%d, bGuidance=%d", result, bGuidance);

    if(isLPMMode())
        return;

    ESpeechCallStatus aCallStatus = eSpeechCallSuccess;
    EGuidanceStatus aGuidanceStatus = eGuidanceStatusInactive;

    if (bGuidance == true) {
        aGuidanceStatus = eGuidanceStatusActive;
    }

    qDebug("[PlatformControllerQnx] getGuidanceStatusResponse(%d, %d, %d)", nReqID, aCallStatus, aGuidanceStatus);
    m_pNaviDeploy->getGuidanceStatusResponse(nReqID, aCallStatus, aGuidanceStatus);
}

void PlatformControllerQnx::routeGuidanceStopRequest(const int32_t nReqID)
{
    qDebug("[PlatformControllerQnx] routeGuidanceStopRequest");

    emit PlatformRouteGuidanceStop(nReqID);
}

void PlatformControllerQnx::onResponseRouteGuidanceStop(int nReqID, NAVI_REQUEST_RESULT result)
{
    qDebug("[PlatformControllerQnx] onResponseRouteGuidanceStop result=%d", result);

    if(isLPMMode())
        return;

    ESpeechCallStatus aCallStatus = eSpeechCallSuccess;

    if (result != NAVI_REQUEST_RESULT_DONE) {
        aCallStatus = eSpeechCallFailure;
    }

    qDebug("[PlatformControllerQnx] routeGuidanceStopResponse(%d, %d)", nReqID, aCallStatus);
    m_pNaviDeploy->routeGuidanceStopResponse(nReqID, aCallStatus);
}

void PlatformControllerQnx::getAvailAddressDataRequest(const int32_t nReqID, const SGetAvailAddressData& aLocGetData)
{
    qDebug("[PlatformControllerQnx] getAvailAddressDataRequest lang=%s, loc_type=%d", aLocGetData.m_strLang.c_str(), aLocGetData.m_eAddrLocType);

    if(isLPMMode())
        return;

    ESpeechCallStatus aCallStatus = eSpeechCallSuccess;
    std::vector<SAvailAddressData> aRetData;

    if (aLocGetData.m_eAddrLocType == eAddrLocTypeAny) {
        ;
    }
    else if(aLocGetData.m_eAddrLocType == eAddrLocTypeCCP) {

        SAvailAddressData country;
        country.m_eAddrElemType = eAddrElemCountry;
        country.m_strOrtho = "대한민국";
        country.m_strPhonetic = "";
        country.m_strVal = "1000000";
        aRetData.push_back(country);

//        SAvailAddressData city;
//        city.m_eAddrElemType = eAddrElemCity;
//        city.m_strOrtho = "서울";
//        city.m_strPhonetic = "";
//        city.m_strVal = "4000064";
//        aRetData.push_back(city);
    }

    qDebug("[PlatformControllerQnx] getAvailAddressDataResponse(%d, %d, aRetData)", nReqID, aCallStatus);
    m_pNaviDeploy->getAvailAddressDataResponse(nReqID, aCallStatus, aRetData);
}

void PlatformControllerQnx::setAvailAddressDataRequest(const int32_t nReqID, const std::vector<SAddressData>& aAddrData)
{
    qDebug("[PlatformControllerQnx] setAvailAddressDataRequest aAddrData.size()=%d", aAddrData.size());

    std::vector<SAddressData> data = aAddrData;
    for (std::vector<SAddressData>::iterator it = data.begin(); it != data.end(); ++it)
    {
        SAddressData item = *it;
        qDebug("  -> m_eAddrElemType=%d, m_strAddrElemVal=%s(%d)",
               item.m_eAddrElemType, item.m_strAddrElemVal.c_str(), item.m_strAddrElemVal.length());
    }

    if(isLPMMode())
        return;

    ESpeechCallStatus aCallStatus = eSpeechCallSuccess;
    SAddressRetData aRetData;
    memset(&aRetData, 0x00, sizeof(SAddressRetData));
    aRetData.m_eAddrElemStatus = eAddrElemStatusNotAmbiguous;   ///< ambiguous, invalid, not ambiguous
    aRetData.m_eAddrRefineElemType = eAddrElemCity; ///< type of first ambiguous element
    aRetData.m_nNoOfElemes = 0;                   ///< number of elements in refinement file
    aRetData.m_strRefineFile = "";         ///< Filename of file containing elements to refine, incl. abs path

    qDebug("[PlatformControllerQnx] setAvaillAddressDataResponse(%d, %d, aRetData)", nReqID, aCallStatus);
    m_pNaviDeploy->setAvailAddressDataResponse(nReqID, aCallStatus, aRetData);
}

void PlatformControllerQnx::getNextPossibleAddressDataRequest(const int32_t nReqID, const std::vector<SAddressData>& aAddrData)
{
    qDebug("[PlatformControllerQnx] getNextPossibleAddressDataRequest aAddrData.size()=%d", aAddrData.size());

    std::vector<SAddressData> data = aAddrData;
    for (std::vector<SAddressData>::iterator it = data.begin(); it != data.end(); ++it)
    {
        SAddressData item = *it;
        qDebug("  -> m_eAddrElemType=%d, m_strAddrElemVal=%s(%d)",
               item.m_eAddrElemType, item.m_strAddrElemVal.c_str(), item.m_strAddrElemVal.length());
    }

    if(isLPMMode())
        return;

    ESpeechCallStatus aCallStatus = eSpeechCallSuccess;
    std::vector<EAddrElemType> aRetData;

    qDebug("[PlatformControllerQnx] getNextPossibleAddressDataResponse(%d, %d, aRetData)", nReqID, aCallStatus);
    m_pNaviDeploy->getNextPossibleAddressDataResponse(nReqID, aCallStatus, aRetData);
}

void PlatformControllerQnx::getSelectedHouseNoRequest(const int32_t nReqID)
{
    qDebug("[PlatformControllerQnx] getSelectedHouseNoRequest reqID=%d", nReqID);

    if(isLPMMode())
        return;

    emit PlatformGetSelectedHouseNo(nReqID);

    //<.. for vr testing
    ESpeechCallStatus aCallStatus = eSpeechCallSuccess;
    std::string aSelectedNo = "";

    qDebug("[PlatformControllerQnx] getSelectedHouseNoResponse(%d, %d, %s)", nReqID, aCallStatus, aSelectedNo.c_str());
    m_pNaviDeploy->getSelectedHouseNoResponse(nReqID, aCallStatus, aSelectedNo);
    //..>
}

void PlatformControllerQnx::onGetSelectedHouseNo(int nReqID, NAVI_REQUEST_RESULT result, std::string aSelectedNo)
{
    qDebug("[PlatformControllerQnx] onGetSelectedHouseNo reqID=%d, result=%d, number=%s", nReqID, result, aSelectedNo.c_str());

    ESpeechCallStatus aCallStatus = eSpeechCallSuccess;
    if (NAVI_REQUEST_RESULT_DONE != result) {
        aCallStatus = eSpeechCallFailure;
    }

    m_pNaviDeploy->getSelectedHouseNoResponse(nReqID, aCallStatus, aSelectedNo);
}

void PlatformControllerQnx::setSelectedPhonenumberRequest(const int32_t nReqID, const std::string aStrPhonenumer)
{
    qDebug("[PlatformControllerQnx] setSelectedPhonenumberRequest reqID=%d, phoneno=%s", nReqID, aStrPhonenumer.c_str());

    if(isLPMMode())
        return;

    emit PlatformSetSelectedPhonenumber(nReqID, aStrPhonenumer);

    //<.. for vr testing
    ESpeechCallStatus aCallStatus = eSpeechCallSuccess;
    bool aIsNavigable = false;

    qDebug("[PlatformControllerQnx] setSelectedPhonenumberResponse(%d, %d, %d)", nReqID, aCallStatus, aIsNavigable);
    m_pNaviDeploy->setSelectedPhonenumberResponse(nReqID, aCallStatus, aIsNavigable);
    //..>
}

void PlatformControllerQnx::onSetSelectedPhonenumber(int nReqID, NAVI_REQUEST_RESULT result, bool aIsNavigable)
{
    qDebug("[PlatformControllerQnx] onSetSelectedPhonenumber reqID=%d, result=%d, navigable=%d",
           nReqID, result, aIsNavigable);

    ESpeechCallStatus aCallStatus = eSpeechCallSuccess;
    if (NAVI_REQUEST_RESULT_DONE != result) {
        aCallStatus = eSpeechCallFailure;
    }

    m_pNaviDeploy->setSelectedPhonenumberResponse(nReqID, aCallStatus, aIsNavigable);
}

void PlatformControllerQnx::setPoiSearchAreaRequest(const int32_t nReqID, const ESearchArea& aSearchArea)
{
    qDebug("[PlatformControllerQnx] setPoiSearchAreaRequest reqId=%d, searchArea=%d", nReqID, aSearchArea);

    if(isLPMMode())
        return;

    emit PlatformSetPoiSearchArea(nReqID, (NAVI_ESearchArea)aSearchArea);
}

void PlatformControllerQnx::onSetPoiSearchArea(int nReqID, NAVI_REQUEST_RESULT result)
{
    qDebug("[PlatformControllerQnx] onSetPoiSearchArea reqID=%d, result=%d", nReqID, result);

    ESpeechCallStatus aCallStatus = eSpeechCallSuccess;
    if (NAVI_REQUEST_RESULT_DONE != result) {
        aCallStatus = eSpeechCallFailure;
    }

    m_pNaviDeploy->setPoiSearchAreaResponse(nReqID, aCallStatus);
}

void PlatformControllerQnx::getPOICategorySelectedRequest(const int32_t nReqID)
{
    qDebug("[PlatformControllerQnx] getPOICategorySelectedRequest");

    if(isLPMMode())
        return;

    ESpeechCallStatus aCallStatus = eSpeechCallSuccess;
    std::string aPOICategoryID = "";

    qDebug("[PlatformControllerQnx] getPOICategorySelectedResponse(%d, %d, %s)", nReqID, aCallStatus, aPOICategoryID);
    m_pNaviDeploy->getPOICategorySelectedResponse(nReqID, aCallStatus, aPOICategoryID);
}

void PlatformControllerQnx::setPoiRequest(const int32_t nReqID, const std::string& aStrPoiValue)
{
    qDebug("[PlatformControllerQnx] setPoiRequest reqId=%d, strPoi=%s(%d)", nReqID, aStrPoiValue.c_str(), aStrPoiValue.length());
    if(isLPMMode())
        return;

    if (aStrPoiValue.length() == 0 || aStrPoiValue == "") {
        onSetPoiResponse(nReqID, NAVI_REQUEST_RESULT_FAIL, 0, "");
    }
    else {
        emit PlatformSetPoiRequest(nReqID, aStrPoiValue);
    }

    //<.. for vr testing
//    ESpeechCallStatus aCallStatus = eSpeechCallSuccess;
//    EAddrElemStatus aPoiStatus = eAddrElemStatusInvalid;
//    int aNoOfEntries = 0;
//    std::string aStrFilename = "";

//    qDebug("[PlatformControllerQnx] setPoiResponse(%d, %d, %d, %d, %s)", nReqID, aCallStatus, aPoiStatus, aNoOfEntries, aStrFilename);
//    m_pNaviDeploy->setPoiResponse(nReqID, aCallStatus, aPoiStatus, aNoOfEntries, aStrFilename);
    //..>
}

void PlatformControllerQnx::onSetPoiResponse(int nReqID, NAVI_REQUEST_RESULT result, uint aNoOfEntries, string aFileName)
{
    qDebug("[PlatformControllerQnx] onSetPoiResponse reqId=%d, result=%d, count=%d, file=%s", nReqID, result, aNoOfEntries, aFileName.c_str());

    ESpeechCallStatus aCallStatus = eSpeechCallSuccess;
    if (NAVI_REQUEST_RESULT_DONE != result) {
        aCallStatus = eSpeechCallFailure;
    }

    EAddrElemStatus aPoiStatus = eAddrElemStatusInvalid;
    if (aNoOfEntries == 0) {
        aPoiStatus = eAddrElemStatusNotFound;
    }
    else if (aNoOfEntries == 1) {
        aPoiStatus = eAddrElemStatusNotAmbiguous;
    }
    else {
        aPoiStatus = eAddrElemStatusAmbiguous;
    }

    qDebug("[PlatformControllerQnx] setPoiResponse(%d, %d, %d, %d, %s)", nReqID, aCallStatus, aPoiStatus, aNoOfEntries, aFileName);
    m_pNaviDeploy->setPoiResponse(nReqID, aCallStatus, aPoiStatus, aNoOfEntries, aFileName);
}

void PlatformControllerQnx::setPrevDestContextRequest(const int32_t nReqID, const int32_t aContextId)
{
    qDebug("[PlatformControllerQnx] setPrevDestContextRequest");
    if(isLPMMode())
        return;

    ESpeechCallStatus aCallStatus = eSpeechCallSuccess;

    qDebug("[PlatformControllerQnx] setPrevDestContextResponse(%d, %d)", nReqID, aCallStatus);
    m_pNaviDeploy->setPrevDestContextResponse(nReqID, aCallStatus);
}

void PlatformControllerQnx::getCheckRouteExistsRequest(const int32_t nReqID)
{
    qDebug("[PlatformControllerQnx] getCheckRouteExistsRequest");

    emit PlatformGetCheckRouteExistsRequest(nReqID);
}

void PlatformControllerQnx::onGetCheckRouteExistsResponse(int nReqID, NAVI_REQUEST_RESULT result, bool aRouteExists)
{

    ESpeechCallStatus aCallStatus = eSpeechCallSuccess;

    if (result != NAVI_REQUEST_RESULT_DONE) {
        aCallStatus = eSpeechCallFailure;
    }

    qDebug("[PlatformControllerQnx] getCheckRouteExistsResponse(%d, %d, %d)", nReqID, aCallStatus, aRouteExists);
    m_pNaviDeploy->getCheckRouteExistsResponse(nReqID, aCallStatus, aRouteExists);
}

void PlatformControllerQnx::speechDialogSelectNotification(const NAVI_ESpeechSelectId selectId) {
    qDebug()<<"PlatformControllerQnx::speechDialogSelectNotification selectId:"<<selectId;
    m_VrService->speechDialogSelectNotification(selectId);
}

void PlatformControllerQnx::speechPicklistSelectNotification(const int selectId) {
    qDebug()<<"PlatformControllerQnx::speechPicklistSelectNotification selectId:"<<selectId;
    m_VrService->speechPicklistSelectNotification(selectId);
}
void PlatformControllerQnx::onResponseShowSpeechDialog(const int32_t nReqID, const NAVI_ESpeechDialogStatus aCallStatus) {
    m_VrService->showSpeechDialogResponse(nReqID, aCallStatus);
}
void PlatformControllerQnx::onResponseShowSpeechPicklist(const int32_t nReqID, const NAVI_ESpeechDialogStatus aCallStatus) {
    m_VrService->showSpeechPicklistResponse(nReqID, aCallStatus);
}

void PlatformControllerQnx::onResponseHideSpeechDialog(const int32_t nReqID, const NAVI_ESpeechDialogStatus aCallStatus) {
    m_VrService->hideSpeechDialogResponse(nReqID, aCallStatus);
}

void PlatformControllerQnx::onPicklistScrollResponse(const int32_t nReqID, const NAVI_ESpeechDialogStatus& aCallStatus) {
    m_VrService->picklistScrollResponse(nReqID, aCallStatus);
}

void PlatformControllerQnx::onSelectedPicklistItemResponse(const int32_t nReqID, const NAVI_ESpeechDialogStatus& aCallStatus, const uint32_t aSelItem) {
    m_VrService->selectedPicklistItemResponse(nReqID, aCallStatus, aSelItem);
}
void PlatformControllerQnx::onGetSelectedPhonenumberResponse(const int32_t nReqID, const NAVI_ESpeechDialogStatus& aCallStatus, const std::string& aPhonenumber) {
    m_VrService->getSelectedPhonenumberResponse(nReqID, aCallStatus, aPhonenumber);
}

void PlatformControllerQnx::getNaviStatusRequest(const int32_t nReqID)
{
    qDebug("[PlatformControllerQnx] getNaviStatusRequest");
    if(isLPMMode())
        return;

    ESpeechCallStatus aCallStatus = eSpeechCallSuccess;
    ENaviStatus aNaviStatus = eStatusAvailable;

    qDebug("[PlatformControllerQnx] getNaviStatusResponse(%d, %d, %d)", nReqID, aCallStatus, aNaviStatus);
    m_pNaviDeploy->getNaviStatusResponse(nReqID, aCallStatus, aNaviStatus);
}

void PlatformControllerQnx::getStateInfoRequest(const int32_t nReqID)
{
    qDebug("[PlatformControllerQnx] getStateInfoRequest");

    if(isLPMMode())
        return;

    ESpeechCallStatus aCallStatus = eSpeechCallSuccess;
    ESpeechStateInfo aStateInfo = eStateInfoNone;

    qDebug("[PlatformControllerQnx] getStateInfoResponse(%d, %d, %d)", nReqID, aCallStatus, aStateInfo);
    m_pNaviDeploy->getStateInfoResponse(nReqID, aCallStatus, aStateInfo);
}

void PlatformControllerQnx::setSpeechContextAsWaypointRequest(const int32_t nReqID, const EAddrType aAddrType)
{
    qDebug("[PlatformControllerQnx] setSpeechContextAsWaypointRequest %d", aAddrType);
    if(isLPMMode())
        return;

    ESpeechCallStatus aCallStatus = eSpeechCallSuccess;

    switch(aAddrType) {
    case eAddrTypeHome:
        emit PlatformSetHomeAsWaypoint(nReqID);
        break;
    case eAddrTypePrevDest:
        emit PlatformSetPrevDestAsWaypoint(nReqID);
        break;
    default:
        qDebug("[PlatformControllerQnx] setSpeechContextAsWaypointResponse(%d, %d)", nReqID, aCallStatus);
        m_pNaviDeploy->setSpeechContextAsWaypointResponse(nReqID, aCallStatus);
        break;
    }

}

void PlatformControllerQnx::onSetHomeAsWaypoint(int nReqID, NAVI_REQUEST_RESULT result)
{
    qDebug("[PlatformControllerQnx] onSetHomeAsWaypoint reqID=%d, result=%d", nReqID, result);

    ESpeechCallStatus aCallStatus = eSpeechCallSuccess;
    if (NAVI_REQUEST_RESULT_DONE != result) {
        aCallStatus = eSpeechCallFailure;
    }

    m_pNaviDeploy->setSpeechContextAsWaypointResponse(nReqID, aCallStatus);
}

void PlatformControllerQnx::onSetPrevDestAsWaypoint(int nReqID, NAVI_REQUEST_RESULT result)
{
    qDebug("[PlatformControllerQnx] onSetPrevDestAsWaypoint reqID=%d, result=%d", nReqID, result);

    ESpeechCallStatus aCallStatus = eSpeechCallSuccess;
    if (NAVI_REQUEST_RESULT_DONE != result) {
        aCallStatus = eSpeechCallFailure;
    }

    m_pNaviDeploy->setSpeechContextAsWaypointResponse(nReqID, aCallStatus);
}

void PlatformControllerQnx::onSetPoiAsWaypoint(int nReqID, NAVI_REQUEST_RESULT result)
{
    qDebug("[PlatformControllerQnx] onSetPoiAsWaypoint reqID=%d, result=%d", nReqID, result);

    ESpeechCallStatus aCallStatus = eSpeechCallSuccess;
    if (NAVI_REQUEST_RESULT_DONE != result) {
        aCallStatus = eSpeechCallFailure;
    }

    m_pNaviDeploy->setSpeechContextAsWaypointResponse(nReqID, aCallStatus);
}

void PlatformControllerQnx::naviWaypointsFullRequest(const int32_t nReqID)
{
    qDebug("[PlatformControllerQnx] naviWaypointsFullRequest");

    emit PlatformNaviWaypointsFullRequest(nReqID);
}

void PlatformControllerQnx::onNaviWaypointsFullResponse(int nReqID, bool aNaviWaypointFull)
{
    ESpeechCallStatus aCallStatus = eSpeechCallSuccess;

    qDebug("[PlatformControllerQnx] naviWaypoinsFullResponse(%d, %d, %d)", nReqID, aCallStatus, aNaviWaypointFull);
    m_pNaviDeploy->naviWaypointsFullResponse(nReqID, aCallStatus, aNaviWaypointFull);
}

// Home-tile
void PlatformControllerQnx::getTrafficInfoList(const int32_t& nReqID)
{
    qDebug("[PlatformControllerQnx] getTrafficInfoList nReqID=%d", nReqID);

    emit PlatformGetTrafficInfoList(nReqID);
}

void PlatformControllerQnx::onResponseGetTrafficInfoList(int nReqID, NAVI_REQUEST_RESULT result, std::vector<NAVI_TRAFFIC_INFO> srcList)
{
    qDebug("[PlatformControllerQnx] onResponseGetTrafficInfoList nReqID=%d, result=%d", nReqID, result);
    if(isLPMMode())
        return;

    std::vector<NAVI_TRAFFIC_INFO_T> trafficInfoList;

    trafficInfoList.reserve(srcList.size());

    for (std::vector<NAVI_TRAFFIC_INFO>::iterator it = srcList.begin(); it!=srcList.end(); ++it)
    {
        NAVI_TRAFFIC_INFO item = *it;
        NAVI_TRAFFIC_INFO_T infoItem;
        memset(&infoItem, 0x00, sizeof(NAVI_TRAFFIC_INFO_T));
        infoItem.listItemID = item.listItemID;
        infoItem.IncidentType = (E_NAVI_HOMETILE_TRAFFIC_INCIDENT_TYPE)item.IncidentType;
        infoItem.roadName = item.roadName;
        infoItem.delayTime = item.delayTime;
        infoItem.distance = item.distance;
        infoItem.locationData << item.locationData;
        trafficInfoList.push_back(infoItem);
    }

    m_pNaviDeploy->ongetTrafficInfoList(nReqID, (E_NAVI_REQUEST_RESULT)result, trafficInfoList);
}

void PlatformControllerQnx::getFuelStationList(const int32_t& nReqID)
{
    qDebug("[PlatformControllerQnx] getFuelStationList nReqID=%d", nReqID);

    emit PlatformGetFuelStationList(nReqID);
}

void PlatformControllerQnx::onResponseGetFuelStationList(int nReqID, NAVI_REQUEST_RESULT result, std::vector<NAVI_POI_INFO> srcList)
{
    qDebug("[PlatformControllerQnx] onResponseGetFuelStationList nReqID=%d, result=%d", nReqID, result);
    if(isLPMMode())
        return;

    std::vector<NAVI_POI_INFO_T> fuelStationList;

    fuelStationList.reserve(srcList.size());

    for (std::vector<NAVI_POI_INFO>::iterator it = srcList.begin(); it!=srcList.end(); ++it)
    {
        NAVI_POI_INFO item = *it;
        NAVI_POI_INFO_T infoItem;
        memset(&infoItem, 0x00, sizeof(NAVI_POI_INFO_T));
        infoItem.listItemID = item.listItemID;
        infoItem.Name = item.Name;
        infoItem.distance = item.distance;
        infoItem.type = (E_NAVI_POI_TYPE)item.type;
        infoItem.locationData << item.locationData;
        fuelStationList.push_back(infoItem);
    }

    m_pNaviDeploy->ongetFuelStationList(nReqID, (E_NAVI_REQUEST_RESULT)result, fuelStationList);
}

void PlatformControllerQnx::getRestAreaList(const int32_t& nReqID)
{
    qDebug("[PlatformControllerQnx] getRestAreaList nReqID=%d", nReqID);

    emit PlatformGetRestAreaList(nReqID);
}

void PlatformControllerQnx::onResponseGetRestAreaList(int nReqID, NAVI_REQUEST_RESULT result, std::vector<NAVI_POI_INFO> srcList)
{
    qDebug("[PlatformControllerQnx] onResponseGetRestAreaList nReqID=%d, result=%d", nReqID, result);
    if(isLPMMode())
        return;

    std::vector<NAVI_POI_INFO_T> restAreaList;

    restAreaList.reserve(srcList.size());

    for (std::vector<NAVI_POI_INFO>::iterator it = srcList.begin(); it!=srcList.end(); ++it)
    {
        NAVI_POI_INFO item = *it;
        NAVI_POI_INFO_T infoItem;
        memset(&infoItem, 0x00, sizeof(NAVI_POI_INFO_T));
        infoItem.listItemID = item.listItemID;
        infoItem.Name = item.Name;
        infoItem.distance = item.distance;
        infoItem.type = (E_NAVI_POI_TYPE)item.type;
        infoItem.locationData << item.locationData;
        restAreaList.push_back(infoItem);
    }

    m_pNaviDeploy->ongetRestAreaList(nReqID, (E_NAVI_REQUEST_RESULT)result, restAreaList);
}

void PlatformControllerQnx::showPOIonMap(const int32_t& nReqID, const NAVI_POI_INFO_T& poiInfo)
{
    qDebug("[PlatformControllerQnx] showPOIonMap nReqID=%d", nReqID);

    NAVI_POI_INFO info;
    info.listItemID = poiInfo.listItemID;
    info.Name = poiInfo.Name;
    info.distance = poiInfo.distance;
    info.type = (NAVI_POI_TYPE)poiInfo.type;
    info.locationData << poiInfo.locationData;

    emit PlatformShowPOIonMap(nReqID,info);
}

void PlatformControllerQnx::onResponseShowPOIonMap(int nReqID, NAVI_REQUEST_RESULT result)
{
    qDebug("[PlatformControllerQnx] onResponseShowPOIonMap nReqID=%d", nReqID);
    m_pNaviDeploy->onshowPOIonMap(nReqID, (E_NAVI_REQUEST_RESULT)result);
}

#endif

#ifdef PLATFORM_QNX //CAN Service
bool PlatformControllerQnx::publishRouteStatusIGM(const CAN_ROUTE_STATUS_IGM& status) {
    m_CanService->publishRouteStatusIGM((const E_CAN_ROUTE_STATUS_IGM&)status);
}
#endif

void PlatformControllerQnx::onShowSystemComponentResult(int requestID, REQ_RESULT_MSG result)
{
    emit PlatformShowSystemComponentResult(requestID, result);
}

void PlatformControllerQnx::onHideSystemComponentResult(int requestID, REQ_RESULT_MSG result)
{
    emit PlatformHideSystemComponentResult(requestID, result);
}

void PlatformControllerQnx::onCmdShowApplication(int requestID, char* strAppRole, char* strIntent)
{
    qDebug("[PlatformControllerQnx] onCmdShowApplication nReqID=%d, role=%s, intent=%s",
           requestID, strAppRole, strIntent);
    emit PlatformCmdShowApplication(requestID, strAppRole, strIntent);
}

void PlatformControllerQnx::onCmdHideApplication(int requestID, char* strAppRole, char* strIntent)
{
    qDebug("[PlatformControllerQnx] onCmdHideApplication nReqID=%d, role=%s, intent=%s",
           requestID, strAppRole, strIntent);
    emit PlatformCmdHideApplication(requestID, strAppRole, strIntent);
}

void PlatformControllerQnx::onCmdResumeApplication(int requestID, char* strAppRole, char* strIntent)
{
    qDebug("[PlatformControllerQnx] onCmdResumeApplication nReqID=%d, role=%s, intent=%s",
           requestID, strAppRole, strIntent);
    emit PlatformCmdResumeApplication(requestID, strAppRole, strIntent);
}

void PlatformControllerQnx::onPhoneAvailableChanged(bool available)
{
    qDebug("[PlatformControllerQnx] onPhoneAvailableChanged %d", available);

    emit PlatformPhoneAvailableChanged(available);
}

void PlatformControllerQnx::onCmdReadyToSuspendSystem(int requestID)
{
    bIsLPM = true;
    emit PlatformCmdReadyToSuspendSystem(requestID);
}

void PlatformControllerQnx::onCmdReadyToResumeSystem(int requestID)
{
    emit PlatformCmdReadyToResumeSystem(requestID);
}

void PlatformControllerQnx::onWaitingReadyToSuspend(WAITING_SUSPEND_RES eResult)
{
    emit PlatformWaitingReadyToSuspend(eResult);
}

void PlatformControllerQnx::onResponseShowKeyboard(KEYBOARD_RESPONSE result)
{
    qDebug("[PlatformControllerQnx] onResponseShowKeyboard %d", result);

    emit PlatformResponseShowKeyboard(result);
}

void PlatformControllerQnx::onResponseHideKeyboard(KEYBOARD_RESPONSE result)
{
    qDebug("[PlatformControllerQnx] onResponseHideKeyboard %d", result);

    emit PlatformResponseHideKeyboard(result);
}

void PlatformControllerQnx::onNotifyKeyboardResult(KEYBOARD_RESULT eResult, char* userInputText)
{
    qDebug("[PlatformControllerQnx] onNotifyKeyboardResult %d text:%s", eResult , userInputText );

    emit PlatformNotifyKeyboardResult(eResult, userInputText);
}

void PlatformControllerQnx::onRequestCandidateInfo(unsigned int inputID, char *userInputText)
{
    emit PlatformRequestCandidateInfo(inputID, userInputText);
}

void PlatformControllerQnx::onRequestCandidateList(char* userInputText, unsigned int startIndex, unsigned int endIndex)
{
    emit PlatformRequestCandidateList(userInputText, startIndex, endIndex);
}

void PlatformControllerQnx::onNotifyKeyboardError(KEYBOARD_ERROR eError)
{
    emit PlatformNotifyKeyboardError(eError);
}

void PlatformControllerQnx::onResponseShowPopup(unsigned int handle, POPUP_RESPONSE eResponse)
{
    emit PlatformResponseShowPopup(handle, eResponse);
}

void PlatformControllerQnx::onResponseHidePopup(unsigned int handle, POPUP_RESPONSE eResponse)
{
    emit PlatformResponseHidePopup(handle, eResponse);
}

void PlatformControllerQnx::onNotifyPopupResult(unsigned int handle, POPUP_RESULT eResult, bool isChecked)
{
    emit PlatformNotifyPopupResult(handle, eResult, isChecked);
}

void PlatformControllerQnx::onResponseUpdatePopup(unsigned int handle, POPUP_RESPONSE eResponse)
{
    emit PlatformResponseUpdatePopup(handle, eResponse);
}

void PlatformControllerQnx::onNotifyStatusChanged(unsigned int handle, POPUP_DISPLAY_STATUS ePopupDisplayStatus)
{
    emit PlatformNotifyStatusChanged(handle, ePopupDisplayStatus);
}

void PlatformControllerQnx::onButtonEvent(SWITCH eSwitchType, BUTTON eButton, ACTION eAction)
{
    emit PlatformButtonEvent(eSwitchType, eButton, eAction);
}

void PlatformControllerQnx::onRotaryEvent(ROTARY eRotary, ROTATION eRotation, int step)
{
    emit PlatformRotaryEvent(eRotary, eRotation, step);
}

void PlatformControllerQnx::onSystemPopupVisibilityChanged(bool isVisibleSystemPopup)
{
    emit PlatformSystemPopupVisibilityChanged(isVisibleSystemPopup);
}

void PlatformControllerQnx::onCmdTerminateApplication()
{
    emit PlatformCmdTerminateApplication();
}

void PlatformControllerQnx::onResponseRequestResourceAudio(int result, int sessionId, int sessionState, const char *pDevice)
{
    emit ResponseRequestResourceAudio(result, sessionId, sessionState, pDevice);
}

void PlatformControllerQnx::onResponseWithdrawResourceAudio(int result, int sessionId)
{
    emit ResponseWithdrawResourceAudio(result, sessionId);
}

void PlatformControllerQnx::onResponseRequestResourceAlarm(int result, int sessionId, int sessionState, const char *pDevice)
{
    emit ResponseRequestResourceAlarm(result, sessionId, sessionState, pDevice);
}

void PlatformControllerQnx::onResponseWithdrawResourceAlarm(int result, int sessionId)
{
    emit ResponseWithdrawResourceAlarm(result, sessionId);
}

void PlatformControllerQnx::onSystemLanguageUpdate(const char *langcode)
{
    qDebug("[PlatformControllerQnx] onSystemLanguageUpdate code=%s", langcode);

    emit PlatformSystemLanguageUpdate(langcode);
}

void PlatformControllerQnx::onSystemUIThemeUpdate(UI_THEME theme)
{
    qDebug("[PlatformControllerQnx] onSystemUIThemeUpdate %d", (int)theme);

    emit PlatformSystemUIThemeUpdate(theme);
}

void PlatformControllerQnx::onNaviMapUIThemeUpdate(UI_THEME theme)
{
    qDebug("[PlatformControllerQnx] onNaviMapUIThemeUpdate %d", (int)theme);

    emit PlatformNaviMapUIThemeUpdate(theme);
}

void PlatformControllerQnx::onResponseSetTurnByTurnInformation(REQUEST_RESULT result)
{
    qDebug("[PlatformControllerQnx] onResponseSetTurnByTurnInformation result=%d", result);

    emit PlatformResponseSetTurnByTurnInformation(result);
}

void PlatformControllerQnx::onResponseSetDistance(REQUEST_RESULT result)
{
    qDebug("[PlatformControllerQnx] onResponseSetDistance result=%d", result);

    emit PlatformResponseSetDistance(result);
}

void PlatformControllerQnx::onNotifyViewLayoutChangeFromCluster(CLUSTER_VIEW_MODE mode)
{
    qDebug("[PlatformControllerQnx] onNotifyViewLayoutChangeFromCluster mode=%d", mode);

    emit PlatformNotifyViewLayoutChangeFromCluster(mode);
}

void PlatformControllerQnx::onResponseSetNavigationLayout(REQUEST_RESULT result)
{
    qDebug("[PlatformControllerQnx] onResponseSetNavigationLayout result=%d", result);

    emit PlatformResponseSetNavigationLayout(result);
}

void PlatformControllerQnx::onResponseGetClusterType(CLUSTER_TYPE type)
{
    qDebug("[PlatformControllerQnx] onResponseGetClusterType type=%d", type);

    emit PlatformResponseGetClusterType(type);
}

void PlatformControllerQnx::onResponseGetHUDActivation(HUD_ACTIVATION activation)
{
    qDebug("[PlatformControllerQnx] onResponseGetHUDActivation activation=%d", activation);

    emit PlatformResponseGetHUDActivation(activation);
}

void PlatformControllerQnx::onResponseGetClusterVideoType(CLUSTER_HUD_INFORMATION videoTypeInformation)
{
    qDebug("[PlatformControllerQnx] onResponseGetClusterVideoType");

#ifdef PLATFORM_QNX
    //kpiColdBootEvent(KPI_CB_NAVIGATION_AVAILABLE);
    kpiColdBootEvent(KPI_CB_CSDK_NAVI_RECOMMENDED_DESTINATIONS_AVAILABLE);
    kpiFinishMeasurement(KPI_NAVIGATION_ROUTE_ENTRY_INPUT, "Tmap Cold Start");
    kpiBeginMeasurement(KPI_LPM_SEND_TO_CAR_DESTINATION_AVAILABLE);
#endif
    emit PlatformResponseGetClusterVideoType(videoTypeInformation);
}

void PlatformControllerQnx::onResponseSetHUDLayout(REQUEST_RESULT result)
{
    qDebug("[PlatformControllerQnx] onResponseSetHUDLayout result=%d", result);

    emit PlatformResponseSetHUDLayout(result);
}

void PlatformControllerQnx::onNotifyHudViewModeChangeFromCluster(HUD_VIEW_MODE mode)
{
    qDebug("[PlatformControllerQnx] onNotifyHudViewModeChangeFromCluster mode=%d", mode);

    emit PlatformNotifyHudViewModeChangeFromCluster(mode);
}

void PlatformControllerQnx::onResponseGetUserLoginStatus(int profileId, bool loginStatus, const char* tokenInfo)
{
    qDebug("[PlatformControllerQnx] onResponseGetUserLoginStatus profileId=%d, login=%d, token=%s", profileId, loginStatus, tokenInfo);

    emit PlatformResponseGetUserLoginStatus(profileId, loginStatus, tokenInfo);
}

void PlatformControllerQnx::onBrProfileExpired(const char* aliasName)
{
    qDebug("[PlatformControllerQnx] onBrProfileExpired alias=%s", aliasName);

    emit PlatformBrProfileExpired(aliasName);
}

void PlatformControllerQnx::onBrUpdateLoginStatus(bool loginStatus, int profileId, const char* aliasName, const char* tokenInfo)
{
    qDebug("[PlatformControllerQnx] onBrUpdateLoginStatus login=%d, profileId=%d, alias=%s, token=%s", loginStatus, profileId, aliasName, tokenInfo);

    emit PlatformBrUpdateLoginStatus(loginStatus, profileId, aliasName, tokenInfo);
}

void PlatformControllerQnx::onResponseGetFuelType(CCF_FUEL_TYPE type)
{
    qDebug("[PlatformControllerQnx] onResponseGetFuelType %d", type);

    emit PlatformRequestGetFuelType(type);
}

void PlatformControllerQnx::onResponseGetVIN(char *vin)
{
    qDebug("[PlatformControllerQnx] onResponseGetVIN %s", vin);

    emit PlatformRequestGetVIN(vin);
}

void PlatformControllerQnx::onDCMFatigueStateToNavi(NAVI_DCM_FATIGUE state)
{
    qDebug("[PlatformControllerQnx] onDCMFatigueStateToNavi state=%d", (int)state);

    emit PlatformDCMFatigueStateToNavi(state);
}

void PlatformControllerQnx::onRequestToFindRestAreas(NAVI_REQ_TO_FIND_REST_AREAS area)
{
    qDebug("[PlatformControllerQnx] onRequestToFindRestAreas area=%d", area);

    if(isLPMMode())
        return;

    if (NAVI_REQUEST_FIND_REST_AREAS == area) {
        emit PlatformRequestToFindRestAreas(area);
    }
    else {
#ifdef PLATFORM_QNX
        m_DcmService->setRestAreasAvailability(NAVI_REST_AREA_NONE);
#endif
    }
}

void PlatformControllerQnx::onReqToDisplayRestAreas(NAVI_REQ_TO_DISPLAY_REST_AREAS area)
{
    qDebug("[PlatformControllerQnx] onReqToDisplayRestAreas area=%d", area);

    emit PlatformReqToDisplayRestAreas(area);
}

void PlatformControllerQnx::setRestAreasAvailability(NAVI_REST_AREA_AVAIL value)
{
    qDebug("[PlatformControllerQnx] setRestAreasAvailability value=%d", value);

    if(isLPMMode())
        return;
#ifdef PLATFORM_QNX
    m_DcmService->setRestAreasAvailability(value);
#endif
}

void PlatformControllerQnx::setRestArRouteGuideStatus(NAVI_REST_AREA_ROUTE value)
{
    qDebug("[PlatformControllerQnx] setRestArRouteGuideStatus value=%d", value);

    if(isLPMMode())
        return;
#ifdef PLATFORM_QNX
    m_DcmService->setRestArRouteGuideStatus(value);
#endif
}

void PlatformControllerQnx::setRestAreaResponse(NAVI_REST_AREA_RESPONSE value)
{
    qDebug("[PlatformControllerQnx] setRestAreaResponse value=%d", value);

    if(isLPMMode())
        return;
#ifdef PLATFORM_QNX
    m_DcmService->setRestAreaResponse(value);
#endif
}

bool PlatformControllerQnx::setRouteInfo(NAVI_ROUTE_INFO_T routeInfo)
{
    qDebug("[PlatformControllerQnx] setRouteInfo");

    if(isLPMMode())
        return false;

#ifdef PLATFORM_QNX
    if(m_pNaviProvider == NULL)
        return false;

    ROUTE_INFO_T route;
    route.nSize = routeInfo.nSize;
    memcpy(&route.payload, &routeInfo.payload, routeInfo.nSize);

    return m_pNaviProvider->setRouteInfo(route);
#else
    return false;
#endif
}

//CAN Service

void PlatformControllerQnx::onDistanceToEmpty(int dist)
{
    qDebug("[PlatformControllerQnx] onDistanceToEmpty dist=%d", dist);

    emit PlatformDistanceToEmpty(dist);
}

void PlatformControllerQnx::onLowFuelLevel(bool lowfuel)
{
    qDebug("[PlatformControllerQnx] onLowFuelLevel lowfuel=%d", lowfuel);

    emit PlatformLowFuelLevel(lowfuel);
}

void PlatformControllerQnx::onFuelGaugePosition(int fuelLevel)
{
    qDebug("[PlatformControllerQnx] onFuelGaugePosition fuelLevel=%d", fuelLevel);

    emit PlatformFuelGaugePosition(fuelLevel);
}

void PlatformControllerQnx::onHVBatteryUsableSOCDisp(int usableState)
{
    qDebug("[PlatformControllerQnx] onHVBatteryUsableSOCDisp usableState=%d", usableState);

    emit PlatformHVBatteryUsableSOCDisp(usableState);
}

void PlatformControllerQnx::onHVBattLevelColourDisp(int colourSet)
{
    qDebug("[PlatformControllerQnx] onHVBattLevelColourDisp colourSet=%d", colourSet);

    emit PlatformHVBattLevelColourDisp(colourSet);
}

void PlatformControllerQnx::onTJPStatusChange(NAVI_TJP_STATUS_T tjpStatusInfo)
{
    qDebug("[PlatformControllerQnx] onTJPStatusChange tjpStatus=%d", (int)tjpStatusInfo.tjpStatus);

    emit PlatformTJPStatusChange(tjpStatusInfo);
}

#ifdef USBSTATUS_QNXAPI
void PlatformControllerQnx::onUsbStatusNoti(USB_STATUS eUsbStatus, char* szPath)
{
    qDebug("[PlatformControllerQnx] onUsbStatusNoti status:%d, path:%s", eUsbStatus, szPath);

    if(isLPMMode())
        return;

    if(USB_STATUS_CONNECT == eUsbStatus)
    {
        QString sPath(szPath);
        emit platformMediaMounted(sPath);
    }
    else
        emit platformMediaUnmounted();
}
#endif

void PlatformControllerQnx::onDeviceListChanged(std::list<SKT::DeviceInfo> list)
{
    emit PlatformDeviceListChanged(list);
}

void PlatformControllerQnx::onDeviceChanged(std::string MountPath, EventType type, MountStatus status)
{
    if(ET_ATTACHED == type && MS_NOERROR == status)
    {
        QString sPath = QString::fromStdString(MountPath);

        emit platformMediaMounted(sPath);
    }
    else
        emit platformMediaUnmounted();
}

void PlatformControllerQnx::onResponsePartitionOptionChange(MountResult result)
{
    emit PlatformResponsePartitionOptionChange(result);
}

void PlatformControllerQnx::onResponseConnectivityStatus(bool bConnected)
{
    emit PlatformResponseConnectivityStatus(bConnected);
}

void PlatformControllerQnx::onVehicleTokenRefreshed(char* token, char* vin)
{
    emit PlatformVehicleTokenRefreshed(token, vin);
}

void PlatformControllerQnx::onCreateSessionHandle(int handle)
{
    emit PlatformCreateSessionHandle(handle);
}

void PlatformControllerQnx::onFlushContext(WINDOW_RESULT result)
{
    emit PlatformFlushContext(result);
}

void PlatformControllerQnx::onSetWindowPosition(WINDOW_RESULT result)
{
    emit PlatformSetWindowPosition(result);
}

void PlatformControllerQnx::onCreateWindowGroup(WINDOW_RESULT result)
{
    emit PlatformCreateWindowGroup(result);
}

void PlatformControllerQnx::onRegisterGroup(WINDOW_RESULT result)
{
    emit PlatformRegisterGroup(result);
}

void PlatformControllerQnx::onSetWindowSize(WINDOW_RESULT result)
{
    emit PlatformSetWindowSize(result);
}

void PlatformControllerQnx::onSetWindowVisible(WINDOW_RESULT result)
{
    emit PlatformSetWindowVisible(result);
}

//VR Services
void PlatformControllerQnx::onShowSpeechDialogRequest(const int nReqID, const NAVI_SSpeechDialogData& aDialogData)
{
    qDebug("[VrService] PlatformControllerQnx::onShowSpeechDialogRequest()");
    emit PlatfromShowSpeechDialogRequest(nReqID, aDialogData);
}

void PlatformControllerQnx::onHideSpeechDialogRequest(const int nReqID) {
    qDebug("[VrService] PlatformControllerQnx::onHideSpeechDialogRequest()");
    emit PlatfromHideSpeechDialogRequest(nReqID);
}

void PlatformControllerQnx::onShowSpeechPicklistRequest(const int nReqID, const NAVI_SSpeechPicklistData aDialogData) {
    qDebug("[VrService] PlatformControllerQnx::onShowSpeechPicklistRequest()");
    emit PlatfromShowSpeechPicklistRequest(nReqID, aDialogData);
}

void PlatformControllerQnx::onPicklistScrollRequest(const int nReqID, const NAVI_EScrollDir& aScrollDir)
{
    emit PlatformPicklistScrollRequest(nReqID, aScrollDir);
}

void PlatformControllerQnx::onSelectedPicklistItemRequest(const int nReqID, const unsigned int aListItem)
{
    emit PlatformSelectedPicklistItemRequest(nReqID, aListItem);
}

void PlatformControllerQnx::onGetSelectedPhonenumberRequest(const int nReqID)
{
    emit PlatformGetSelectedPhonenumberRequest(nReqID);
}

void PlatformControllerQnx::onResponseAddNotification(unsigned int handle, NOTIFY_RESPONSE eResponse)
{

}

void PlatformControllerQnx::onResponseRemoveNotification(unsigned int handle, NOTIFY_RESPONSE eResponse)
{

}

void PlatformControllerQnx::onResponseUpdateNotification(unsigned int handle, NOTIFY_RESPONSE eResponse)
{

}

void PlatformControllerQnx::onResponseDecreaseNumberOfGroupNotification(unsigned int handle, NOTIFY_RESPONSE eResponse)
{

}

void PlatformControllerQnx::onResponseClickButton(unsigned int handle, char* btnName)
{

}

void PlatformControllerQnx::onListNotificationChange(unsigned int numberNotification, bool bHighPriority)
{

}

void PlatformControllerQnx::onFeedbackRemoveNotificationTimeout(unsigned int handle, NOTIFY_RESPONSE eResponse)
{

}

void PlatformControllerQnx::onNotifyRemoveNotiCardByUserAction(unsigned int handle, NOTIFY_RESPONSE eResponse)
{

}

void PlatformControllerQnx::changeMapmathingFeedback(bool enabled)
{

}

#ifdef PLATFORM_QNX
void PlatformControllerQnx::updateMmfb(LOCSRV_MMFB &mmfb)
{
    if(isLPMMode())
        return;

    if(m_LocationService)
        m_LocationService->SendMMFB(mmfb);
}
#endif

void PlatformControllerQnx::requestGetPhoneAvailable()
{
#ifdef PLATFORM_QNX

    if(isLPMMode())
        return;

    m_BtService->getPhoneAvailable();
#endif
}

void PlatformControllerQnx::requestGetUSBDeviceList()
{
#ifdef PLATFORM_QNX

    if(isLPMMode())
        return;

    m_UsbDetectionManager->usbmgr_getDeviceList();
#endif
}

}
