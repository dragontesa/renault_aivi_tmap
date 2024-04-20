#ifndef PLATFORMCONTROLLER_GEN3_H
#define PLATFORMCONTROLLER_GEN3_H

#include "platformcontroller.h"
#ifdef BUILD_APP
    #ifdef TMAP_CONNMAN_PROXY_API
    #include "connmanager_lxc.h"
    #else
    #include "connmanager_csm.h"
    #endif
#else
#include "connmanager.h"
#endif

#include "common/rgdataptr.h"
#include "platformcontroller.h"
#include "navigationdata.h"
#include "satellitedataptr.h"
#include "tmapcontroller.h"
#include "layermanager.h"
#include "navigationservicetype.h"
#include "guicontroltype.h"
#include "hardkeyfilter.h"
#ifdef USE_SYNCBLOCK
    #include "interface/syncBlock/SimpleProducer.h"
    #include "interface/syncBlock/producer.h"
#endif
#include <QtDBus>
class produceWorker;
class BoschGuiCmAiGuicontrolInterface;
class OrgBoschCmNavigationNavigationExtServiceInterface;
class NavigationServer;
class NavigationSDSService;
class NavigationDataUpdateService;
class BoschCmProfileInterface;
class ComBoschDrPositionServiceNavigationInterface;
class BoschCmAiNissanHmiZoneDataServiceHMIZoneDataInterface;
class comboschAutomotiveProxyCarConfiguration;
class ComBoschAutomotiveProxyLifeCycleManagerInterface;
class ComBoschAutomotiveProxyPrivateDataEncryptionInterface;
class ComBoschAutomotiveProxySecureDataCommunicationInterface;
#if defined(BUILD_TEST) && defined(USE_DBUS)
#define TEST_DBUS
#endif

#define TMAP_APP_ID 3
QT_BEGIN_NAMESPACE
class QQmlEngine;
class QQuickWindow;
class QQuickView;
class QThread;
QT_END_NAMESPACE

namespace SKT {

#ifdef TEST_DBUS
class DbusTest;
#endif

struct GadgetInfo {
    GadgetInfo() : instanceId(0){ }
    int instanceId;
    bool active;
};

struct StreamInfo {
    QString VideoStreamIP;
    int VideoStreamPort;
    int VideoType;
    NSMapVideoStreamDimensions VideoStreamDemenstions;
};

struct NaviAppStatue {
    NaviAppStatue() : AndroidAutoStatus(false), VideoStream(false), AlramMute(false), AudioMute(false){}
    bool AndroidAutoStatus;
    bool VideoStream;    
    bool AlramMute;
    bool AudioMute;
};

// Navigation Ext SPM central lock
class NavigationExtCentralLock;

class PlatformControllerGen3 : public PlatformController {
    Q_OBJECT
public:
    explicit PlatformControllerGen3(TmapController* controller, LayerManager* layermanager);
    virtual ~PlatformControllerGen3();

private:
    void init() override;

public:
    void startService() override;
    void finishService() override;
    QString deviceId() const override;

public:
    bool dimmingStatus() const override;
    bool networkOnline() const override;
    bool appForeground() const override;
    char reverseGearState() const override;
    char LanguageIndex() const override;
    char DateFormat() const override;
    char TimeFormat() const override;
    char ColorState() const override;
    char MexMode() const override;
    char DialPOICallStatus() const override;
    char CallServiceStatus() const override;
    uint zone3PullUpListState() const override;

    bool remount(const QString &path, bool readOnly) override;
    bool remountCommand(uint type, const QString &srcPath, const QString &destPath) override;
    bool restart() override;
    bool callNumber(const QString &number) override;
    uint carconfigurationValue(const uchar key) const override;

    // NavigationExt
    void getPublicConfigItem(const QString &publicItemName);


    // profile manager
    uint doRegisterApp(const QString &AppName) override;
    uint doSetProfileName(const QString &newName, uchar userID) override;

    QByteArray getProfileImages() const override;
    QStringList getProfileNames() const override;
    types getProfileTypes() const override;

    void setNavigationDataUpdateStatusInfo(int status, float progress) const override;
    
    public slots:
     void onProfileDataChanged(uint result) const override;
     void onProfileCopied(uint result) const override;
     void onProfileCreated(uint result) const override;
     void onProfileDeleted(uint result) const override;
     void onStatusChanged(uint profileStatus) const;


    void setZoneVisibility(uchar appId, bool visibilityState, uchar displayRegionType) const override;
    void setEditfieldText(uchar appId, const QString &searchBarText, bool isHighLightRequired, uchar highlightStartIndex, uchar highlightCount) const override;
    void setEditFieldUpdate(uchar appId, uchar cursorPosition, const QString &searchbarText) const override;

    void setKeyboard(uchar appId, uchar keyboardType, const QString &validCharacters)const override;
    void setSDSType(uint sdsType, uchar value)const override;
    #if 0
    void setZone1(uchar appId, uint zone1Type, ContentInfo zone1ContentDetails, bool updateRegion) const;
    void setZone3(uchar appId, uint zone3Type, ContentInfo zone3ContentDetails, bool updateRegion) const;
    #endif
    
    void setZone3SubTextInfo(const QStringList &zone3SubText)const;

    PopupProperty getPopupProperty() const;
    PopupProperty getToastPopupProperty() const;
    Q_INVOKABLE void showPopup(const QString &viewname) const override;
    Q_INVOKABLE void hidePopup(const QString &viewname) const override;
    Q_INVOKABLE void beepForwading(int beepType) const override;
    Q_INVOKABLE QDBusPendingCall setupZone1(uint type, const QVariant &param, bool updateForce = false);
    Q_INVOKABLE void setZone3Visible(bool);
    Q_INVOKABLE void setupKeyboard(uchar type, const QString& text="", int searchCount=0);
    Q_INVOKABLE void setZone1ForKeyboard(uchar type, const QString& text, int searchCount=0);
    Q_INVOKABLE void showVolume();
    Q_INVOKABLE void switchToApp();
    Q_INVOKABLE void switchToSettingView();
    Q_INVOKABLE void switchToSystem();
    Q_INVOKABLE void keyForwarding(uint key, uint state);
    Q_INVOKABLE void callNumberWrapper(const QString &number);
    Q_INVOKABLE void appHasNoMap();

     // deprecated, use signal 'property changed' on multi-threads
    // Q_INVOKABLE QObject *hmiZoneInterface() const;
    Q_INVOKABLE void setHmiZoneEventHandler(QObject* handler);
    Q_INVOKABLE void setSearchBarText(const QString&);
    Q_INVOKABLE QObject *getGuiControl() const;

    BoschGuiCmAiGuicontrolInterface *guiControl() { return m_guiControl; }
    OrgBoschCmNavigationNavigationExtServiceInterface *navigationExt() { return m_navigationExt; }
    NavigationServer *navigationService() { return m_navigationServer; }
    NavigationSDSService *navigationSDSService() { return m_navigationSDSService; }
    NavigationDataUpdateService *navigationDataUpdateService() { return m_navigationDataUpdateService; }
    BoschCmProfileInterface *profilemanager() {return m_profilemanager;}
    BoschCmAiNissanHmiZoneDataServiceHMIZoneDataInterface *hmiZone() { return m_hmiZoneManager; }
    comboschAutomotiveProxyCarConfiguration *carconfiguration() { return m_carconfiguration; }
    ComBoschAutomotiveProxyPrivateDataEncryptionInterface *dataencrypt() { return m_dataencrypt; }

    virtual void sendGadgetImage(const QImage& image, int type);

private slots:
    void setDimmingStatus(bool dimming);
    void setAppForeground(bool foreground);
    void setNetworkOnline(bool online);
    void setReverseGearState(char gearState);
    void setLanguageIndex(char languageIndex);
    void setDateFormat(char dateFormat);
    void setTimeFormat(char timeFormat);
    void setColorState(char colorState);
    void setMexMode(char mexMode);

    void setDialPOICallStatus(char dialPOICallStatus);
    void setCallServiceStatus(char callServiceStatus);
    void setZone3PullUpListState(uint zone3PullUpListState);
    void setUpdateLockStates(NXLockStateInfoList lockStateInfoList);
    void setDateTimeDetailInfo(NXDateTimeDetailInfo dateTimeDetailInfo);

    void setHmiState(uint hmiState);
    void setAppStateNavigation(uchar appStateNavigation);

private slots:
    void onProfileChanged(uchar id);
    // void onPlatformThreadStarted();
    void onEngineInitialized(bool success);
    void onApplicationWindowCreated(QQuickWindow* window);
    void onLayermanagerInitialized(bool success);
    void onServiceRegistered();
    // Centrol Lock
    void onSpmCentralLockStateChanged(int lockState);
    void onPlatformSpmCentralLocStateRequested(int state) override; // called from package manager

    void onProfileManagerRegistered();
    void onHMIZoneInterfaceRegistered();
    void onCarconfigurationRegistered();
    void onLifeCycleManagerRegistered();
    void onDataEncryptRegistered();
    void onSecureDataCommunicationRegistered();
    void onSendGadgetImage(QImage img, int type);

    // from ui
    void onMapStartedToRender();

    // from LifeCycleManager
    void onLifecycleRequest(uint mode, uint request_id);

    // from GuiControl
    void onInit_GUI(uint u32LayerID, uint u32SurfaceID, uint u32Width, uint u32Height);
    void onUpdateSurfaceState(uint u32CommandID, uint SurfaceState);
    void onPopupStatus(uint surfaceId, uint popupstatus);
    void onActivateExternalImageProvider(uint activate, QList<int> instanceIds, int consumerId);
    void onExternalImageProviderTouch(TouchInfo data, int instanceId);
    void onContextSwitchRequest(uint ContextSwitchRequest);
    void onFocusConfigProvider(char focusConfig);
    void onNaviGadgetFocusStatusChanged(uint instanceId, bool focus);
    void onNaviGadgetFocusInteractionSignal(FocusInteractionInfo info);

    // from NavigationExt
    void onResetToFactorySettings();
    void onNaviSignalAppStateNavigation(uchar status);
    void onPublicConfigItem(uchar status, NXConfigInfoItemList publicConfigData);
    void onNaviSignalRFAntennaStatus(uchar u8RFAntenna, uchar u8RFAntennaStatus, uchar u8ConnectionCurrent);

    // from NetworkManager
    void onNetworkStateChanged(ConnManager::State state);

    // from Navigation
    void onNavStartStatusChanged(int status);
    void onNavStatusChanged(int status);
    void onMapVersionChanged(QString value);
    void onCurrentStreetChanged(RGDataPtr mCurrentStreet);
    void onSatelliteInfoChanged(SatelliteDataPtr value);
    void onDistanceToDestination(RGDataPtr distanceToDestination);
    void onTurntoStreet(RGDataPtr turntoStreet);
    void onLaneRecommendationList(RGDataPtr laneRecommandtionList);
    void onTrafficEnforcement(RGDataPtr trafficEnforcement);
    void onArrivalInformationList(RGDataPtr arrivalInfomationList);
    void onManeuverSymbol(RGDataPtr maneuverSymbol);
    void onManeuverView(RGDataPtr maneuverView);
    void onRroundaboutExitNumber(RGDataPtr roundaboutExitNumber);
    void onNextTurntoStreet(RGDataPtr nextTurntoStreet);
    void onBarGraphInfo(RGDataPtr barGraphInfo);
    void onSpeedLimit(RGDataPtr speedLimit);
    void onPositionInformation(RGDataPtr positionInformation);
    void onNextManeuverDetails(RGDataPtr nextManeuverDetails);
    void onWorkLocation(TS_ROUTE_POS workLocation);
    void onHomeLocation(TS_ROUTE_POS homeLocation);
    void onMapRepresentation(int mapRepresentation);
    void onWaypointList(QVariant waypointList);
    void onActiveDestinationInfoChanged(QVariant activeWaypointList);
    void onIsAddressBookChanged(bool isAddressBookChanged);
    void onIsLastDestinationListChanged(bool isRecentDestinationChanged);
    void onRGDataInfoChanged(RGDataPtr value);
    void onRouteRecalculationReasonChanged(int value);
    void onRouteCalculateDurationChanged(int duration);
    void onRouteOptionChanged(int option);
    //void getMapVersion();

    void onShowMapScreenWithMapViewMode(uint viewMode);
    void onSetZoomInStep(uchar inStep);
    void onSetZoomLevel(uint zoomLevel);
    void onSetZoomOutStep(uchar outStep);
    void onShowTrafficInfoScreen();
    void onShowAdjustCurrentLocationScreen(NSPositionWGS84 pos);
    void onShowDeleteWaypointListScreen();
    void onRequestWaypointList();
    void onShowWhereAmIScreen();
    void onApplyWaypointListChange();
    void onInsertWaypoint(uint newIndex);
    void onInsertWaypointAtDefaultPosition();
    void onDeleteWaypoints(uint firstIndex, uint lastIndex);
    void onReplaceWaypoint(uint index);
    void onSetLocationWithDestinationMemoryEntry(qulonglong id);
    NSPOILocation onToLocationWithDestinationMemoryEntry(uint category, qulonglong id); // 2019-3-12, get location from destination memory entries
    void onSetLocationWithCoordinates(NSPositionWGS84 location);
    void onSetLocationWithDetails(const NSPOILocation& location);
    void onShowAndSetLocationWithDetails(const NSPOILocation& location);

    void onDestinationReachedStatusChanged(RGDataPtr rg);
    void onWaypointOperationStatusList(const QList<int>& status);

    void onAudioMuteStateChanged(bool enabled);
    void onVoiceGuidanceTypeChanged(bool active);

    // from navigation ui volume
    void onNaviVolumeTouched(bool enabled);

    void onNaviShutdownReadyFinished(bool ok);

    // from system
    void onStartGuidance();
    void onCancelRouteGuidance();
    void onRequestDetailsForSelectedLocation(const NSPOILocation& location);
    void onSetMapRepresentation(uint representation);
    void onSetVoiceGuidance(bool active);
    void onOverSpeedAlertActiveChanged(bool enabled);
    void onStartGuidanceToHomeLocation();
    void onStartGuidanceToWorkLocation();
    void onStartGuidanceToPosWGS84(NSPositionWGS84 destination);
    void onStoreDestinationInAddressBook(NSPOILocation poiLocation);
    void onStorePoiLocationToAddressBook(NSPOILocation poiLocation);
    void onStoreLocationToAddressBook(NSLocation location);
    NSDestinationMemoryDetailsList onRequestDestinationMemoryEntries(uint categoryType);
    void onRetriggerAcousticOutput();
    QString onSdsGetHouseNumberPattern(QString &maxHouseNumber, QStringList &patterns);
    void onSetRouteCriterion(uint routeCriterion);
    void onShowCalculateTour(QVariant v, uint screenTransition);
    void onShowOnMap(NSPositionWGS84 location);
    void onRequestFreeTextSearchResults(const QString &searchString,
                                        uint searchScope,
                                        uint searchType,
                                        uint category,
                                        uint destinationIndex,
                                        uint startIndex,
                                        uint numElements);
    void onShowDestinationFreeTextSearchInput(const QString& searchString);

    NSMapVideoStreamResult onStartMapVideoStream(NSMapVideoStreamDimensions mapVideoStreamDimensions, const QString &ipAddress, ushort portnumber, ushort videoType);
    NSMapVideoStreamResult onModifyMapVideoStream(uint mapVideoStreamHandle, NSMapVideoStreamDimensions mapVideoStreamDimensions, ushort videoType);
    NSMapVideoStreamResult onDeleteMapVideoStream(uint mapVideoStreamHandle);

     void onSpeakSamplePhrase(uint sampleVoicePhrase);


    //fromHMIZone13
    void onActiveNotificationType(uint activeNotification);
    void onButtonPressed(uchar appId, uint buttonId, uchar buttonState);
    void onKeyboardInputFinished(uchar appId, const QString &searchbarText);
    void onKeyboardKeyPressed(uchar appId, const QString &searchbarText, const QString &keyPressed, bool IsCtrlKey, bool IsSpeedLockActive);
    void onPullupButtonPressed(uchar appId, uchar buttonId, uchar buttonState);
    void onTogglePressed(uchar appId, uint buttonId, uchar buttonState, bool toggleValue);

    // to HMIZone13
    void onScreenIDChanged(int ScreenID);

    virtual void beforePlatformContextSwitchRequested();

    // from navigation app
    void onKeyEvent(quint32 state, quint32 nativeScanCode);
    void encorderForwarding(int encCode, int encValue);

    // from NavigationSDSService
    void onPositionStatusInfo(RGDataPtr info);
    void onPositionInfo(RGDataPtr info);
    void onSdsSearchAddress(const NSSDSAddress& requestedAddress);

    // from NavigationDataUpdateService
    void onNavigationDiagDataUpdateStatusInfo(int status, int progress);
    void onNavigationDiagDataDeleteStatusInfo(int status, int progress);

    void onCrossImageVisibleChanged(bool visible);

    void onMapVideoStatus();

    void onContextSwitchResponded(QVariant args);

    void onStopSendingGadgetImageRequested();

private:
    QQuickView* createPopupWindow(QQmlApplicationEngine* engine, const SurfaceProperty& surfaceProperty);
    void setupDBus();
    void NuanceVRTest();
    void contextSwitchResponseComplete();

private:
    TmapController* m_controller;
    SurfaceProperty m_appSurface;
    PopupProperty m_popupProperty;
    PopupProperty m_toastPopupProperty;
    BoschGuiCmAiGuicontrolInterface *m_guiControl;
    OrgBoschCmNavigationNavigationExtServiceInterface *m_navigationExt;
    NavigationExtCentralLock *m_CentralLock;
    comboschAutomotiveProxyCarConfiguration *m_carconfiguration;
    ComBoschAutomotiveProxyLifeCycleManagerInterface *m_lifeCycleManager;
    ComBoschAutomotiveProxyPrivateDataEncryptionInterface *m_dataencrypt;
    NavigationServer *m_navigationServer;
    NavigationSDSService *m_navigationSDSService;
    NavigationDataUpdateService *m_navigationDataUpdateService;
    BoschCmProfileInterface *m_profilemanager;
    BoschCmAiNissanHmiZoneDataServiceHMIZoneDataInterface *m_hmiZoneManager;
    ComBoschAutomotiveProxySecureDataCommunicationInterface* m_secureDataCom;
    QString mMapDataVersion;
    QList<TS_ROUTE_POS> m_Waypoints;
    RGDataPtr mLastRGDataPtr;
    NaviAppStatue *mNaviAppStatue;
    bool m_isStartStreamSignal;
    StreamInfo  *mVideoStreamInfo;
    int nNextManeuverIndex;
    int m_ServiceIndex;
    int m_RecommandType;
    uint m_ClusterHandleId;
    int m_BarGraphInfoTBTIndex;
    int m_BarGraphInfoTBTMaxScale;
    int m_GoPosCode;

    int m_nPrimaryMax;
    int m_nPrimaryMin;
    int m_nSecondaryMax;
    int m_nSecondaryMin;

    bool m_bMapModifyReturn;
    bool m_bNaviInit;
    bool m_bScreenOn;

#ifdef USE_SYNCBLOCK
    GadgetInfo mGadgetInfo;

    producer* m_mapgadgetXXL_producer;
    producer* m_mapgadgetXL_producer;
    producer* m_mapgadgetL_producer;
    producer* m_mapgadgetM_producer;
    producer* m_mapgadgetS_producer;

    producer* m_parkinggadget_producer;
    producer* m_oilstationgadget_producer;
    producer* m_aqigadget_producer;
    producer* m_contactgadget_producer;
    QList<int> m_tempInstanceIds;
    bool m_tempActivate;
#endif
#ifdef TEST_DBUS
    DbusTest *m_dbusTest;
#endif
    bool m_dimmingStatus;
    bool m_appForeground;
    bool m_networkOnline;

    bool m_engineInitialized;
    bool m_lmInitialized;

    char m_ReverseGearState;
    char m_LanguageIndexStatus;
    char m_DateFormatStatus;
    char m_TimeFormatStatus;
    char m_ColorStateStatus;
    char m_MexModeStatus;
    char m_dialPOICallStatus;
    char m_callServiceStatus;
    char m_focusConfig;
    uint m_zone3PullUpListState;
    NXLockStateInfoList m_lockStateInfo;

    quint32 m_ProfileAppId;


    QQuickWindow* mPopupWindow = nullptr;
    QQuickWindow* mToastWindow = nullptr;
    int mZone1Type = -1;
    QVariant mZone1Param;
    bool mBlockZoneCommand = true;
    bool mZone3Visible = false;
    int m_createdLayerCount = 0;
    bool mMapViewVisible = false;
    bool mContextSwitchResponseForComplete = false;

    uint m_lifeCycleManagerRequestId = 0;

    int m_RouteOption;

signals:
    void requestProperty_All();
    void gadgetActive(bool);
    void registApplicationWindow(QQuickWindow* window, const SurfaceProperty* prop);
    void platformUpdateLockStatesChanged(NXLockStateInfoList lockStateInfoList);
    void setPopupInfo(int, int, int, int);
    void setToastPopupInfo(int, int, int, int);
    void clientStatusGuiSenseReady();
    void setPauseMap(bool);
    void registerPopup() const;
    void spcxRgEnabled(bool);
    void contextStateAccepted();
    void contextStateCompleted();

private:
    QThread* m_Thread;
    HardKeyFilter mHardKeyFilter;
    QString  m_DeviceId;
    int m_contextSwitchRequest = -1;
    bool m_appHasNoMap = false;
    QMutex mGadgetMutex;
    bool m_gadgetStopped = false;
};


}
Q_DECLARE_METATYPE(SKT::GadgetInfo)

#endif // PLATFORMCONTROLLER_GEN3_H
