#ifndef TMAPCONTROLLER_H
#define TMAPCONTROLLER_H

#include <QObject>
#include <QQmlApplicationEngine>
#include <QMutex>
#include <QImage>
#include <QQuickWindow>
#include "navigationcontroller.h"
#include "tmapproperty.h"
#include "uilayout.h"
#include "rgsimulator.h"
#include "locationdataptr.h"
#include "qctestingfeatures.h"
#ifdef USE_MIC
#include "platforms/linux-gen3/interface/mic/videostream.h"
#endif
#include "guicontroltype.h"
#include "uiconstants.h"
#include "cswfilter.h"

class QOpenGLFramebufferObject;

namespace SKT {

class PlatformController;
class PlatformConnectivity;
class SearchManager;
class RGAudioHelper;
class MarkerManager;
class PackageManager;
class SessionManager;
class GpsService;
class GpsManagerInterface;
class CipherServiceInterface;

class TmapController : public QObject, public NavigationController
{
    Q_OBJECT
    Q_INTERFACES(SKT::NavigationController)
public:
    enum ScreenId {
        Nav_Gadget_Map_XL = 35,
        Nav_Gadget_Map_L = 51,
        Nav_Gadget_Map_M,
        Nav_Gadget_Map_S,
        Nav_Gadget_Parking,
        Nav_Gadget_OilStation,
        Nav_Gadget_Map_XXL = 82,
        Nav_Gadget_Map_XXXL,
        Nav_Gadget_Contact,
        Nav_Gadget_AQI = 214, //AQI등록이슈로 아이디 확인안됨
    };

    enum MapVideoStreamStatus {
        UNAVAILABLE = 0,
        AVAILABLE = 1,
        SPI_NAVI_ACTIVE = 2,
    };

    int VIDEO_STREAM_HANDLE = 22;

    explicit TmapController(QObject *parent = 0);
    virtual ~TmapController();

    QObject* qobject() override { return this; }
    SessionManagerInterface* sessionManager() override;
    GpsManagerInterface *gpsManager() override;
    SimulatorInterface* simulatorManager() override;
    CipherServiceInterface* cipherService(bool asQaes = false) override;

    bool online() const override { return m_online; }
    bool appForeground() const override { return m_appForeground; }
    QString appVersion() const override;
    int navigationVolume() const override;
    bool currentPosition(double &longitude, double &latitude) override;
    bool currentPosition(int &wX, int &wY) override;
    SunState sunState() const override { return m_sunState; }
    WeatherState weatherState() const override { return m_weatherState; }

    QQmlApplicationEngine* qmlEngine() { return m_qmlEngine; }
    TmapProperty* naviProperty() { return m_tmapProperty; }
    RGAudioHelper* audioHelper() { return m_audioHelper; }

    bool networkOnline() const { return m_networkOnline; }

    Q_INVOKABLE QVariant platformController() const;
    Q_INVOKABLE void setupPlatformPopup();

    bool getStartVideoStream(int width, int height);


    void stopSendingGadgetImage();

protected:
    // for CSW processing
    bool eventFilter(QObject *watched, QEvent *event);    

public slots:
    void setNavigationVolume(int volume) override;
    bool muteState(int channel) const override;
    bool callNumber(const QString &number) override;

    void simulationHardKey(int);
    void simulationGadget(bool shift, int key);
    void simulationGadgetTouch(uint state, uint x, uint y, uint timeStamp, int instanceId);

    void simulationMapInCluster(int);
    void changeMapInClusterImage(const QImage& image );
    void changeIconGadgetImage(const QImage& image, int type);
    void changeGadgetImage(const QImage& image, int type);

    void onCrossImageVisibleChanged(bool visible);

    void setAppHasCswFocus(bool focus);
    void onAppLostCswFocus();
    void handlePointerEvent(int axis, int value);

private slots:
    void setOnline(bool online);
    void setAppForeground(bool foreground);
    void setNetworkOnline(bool online);
    void setSunState(SunState sunState);
    void setWeatherState(WeatherState weatherState);    
    void changeNddsServer(bool commerce);
    void changeVsmServer(int serverType);
    void changeAvNext(int serveType);

    // ndds
    void onNddsDualcenterPolicyFetched(const QString& domainName, int portNum);
    void onNddsServerPolicyFinished();


public :
    bool VSMWorldToWGS84(int x, int y, double &gx, double &gy) override;
    bool WGS84ToVSMWorld(double gx, double gy, int &x, int &y) override;
    bool getVSMWorldToWGS84(int x, int y, double &gx, double &gy) override;
    bool getWGS84ToVSMWorld(double gx, double gy, int &x, int &y) override;
    bool getMapCenterLocation(TS_ROUTE_POS &location) override;

    void changeWindowSize(const QSize& size);

    QString getDeviceId();
    int getCarconfigurationValue(uchar key );    
    void setLanguageId(int id);

public slots:
     void onDoneStartMapStream();


    // configuration
    void changeHostBaseName(const QString& hostBaseName);

    // implement NavigationController interfaces
    bool setMuteState(int channel, bool mute) override;
    bool setMapViewMode(int viewMode) override;
    bool setViewLevel(int level, int subLevel) override;
    bool setViewLevel(ViewLevelInfo level) override;
    bool setTrafficInfoScreen(bool isShow) override;
    bool moveToCurrentPosition() override;
    bool goViaPointEditScreen() override;
    bool goRouteSearchScreen() override;

    // search
    void requestFreeTextSearchResult(const QString& searchWords, int scope, int searchType, int category, int destIndex, int fromIndex, int maxCnt);
    void requestSearchAddress(const QVariant& address);
    void requestSDSSearchAddress(const QVariant& address); // sds service
    void requestLocationDetail(const TS_ROUTE_POS& location);
    void goPoiDetailScreen(const QVariant& detail);
    void showDestinationFreeTextSearchInput(const QString& searchString);


    // route
    bool requestRoute(int option, bool apply) override;
    bool requestRoute(TS_ROUTE_POS pos, int option, bool apply) override;
    bool requestRoute(QList<TS_ROUTE_POS> waypoints, int option, bool apply) override;
    bool requestRoute(QVariant wayPoints, int option, bool apply);
    bool requestReRoute(int option = -1) override;
    bool requestReRoute(TS_ROUTE_POS pos, int option = -1) override;
    bool requestReRoute(QList<TS_ROUTE_POS> waypoints, int option = -1) override;
    void requestAroundSearchParking();
    void requestAroundSearchPatrol(int type);
    bool cancelRoute() override;
    bool setWaypoint(TS_ROUTE_POS pos, int type) override;
    bool insertWaypoint(TS_ROUTE_POS pos, uint slot);
    void insertWaypoint(uint slot);
    void insertWaypointFromSelectInMap();
    void deleteWaypoints(int firstIndex, int lastIndex);    
    bool addFavoritesPosition(TS_ROUTE_POS pos) override;
    bool rePlayVoiceGuidance() override;
    void onGearStateChanged(QString state) override;
    void onLanguageChanged(QString lang) override;
    void onResetToFactorySettings() override;
    void onDateFormatChanged(QString format) override;
    void onTimeFormatChanged(QString format) override;
    void onGoToStandby() override;
    void onGoToRunning() override;
    bool setSDIAlertForMute(bool mute) override;
    void onSearchTextChanged(QString text);
    void onAutoConpleteTextChanged(QString text);
    void applyHardKey(int key, int state);
    void applyDebugKey(int key);
    void onReadyToRunGadget();
    bool isInitVSMView();
    void saveGadgetImage( bool save );

    //TMCRSM-2446:VR로부터 음성안내on/off 명령을 받는 인터페이스
    void setVoiceGuidance(bool active);
    void setAlramGuidance(bool active);

    // Hmi 
    void onHmiStateChanged(uint hmiState) override;

    // ui context
    void onMapStartedToRender() override;
    //void onFrameSwapped();
    void respondTocontextSwitchRequest(QVariant args);

signals: // to Navi
    void mapStartedToRender();

    void onlineChanged(bool online);
    void appForegroundChanged(bool foreground);
    void navigationVolumeChanged(int volume);
    void muteStateChanged(bool mute);
    void sunStateChanged(SKT::NavigationController::SunState sun);
    void weatherStateChanged(SKT::NavigationController::WeatherState weather);
    //void cswPressed(int);
    void showGadget(int, bool, bool);

    void setDaynightTimeset(const QList<QDateTime>&); // only for engineer mode

    //TMCRSM-2446:VR로부터 음성안내on/off 명령을 받는 인터페이스
    void setVoiceGuidanceOccur(bool active);
    void setAlramGuidanceOccur(bool active);

    void mapViewModeChanged(int viewMode);
    void viewLevelChanged(int level, int subLevel);
    void trafficInfoScreenChanged(bool isShow);
    void moveToCurrentPositionOccur();

    // search
    void requestFreeTextSearchResultOccur(const QString& searchWords,
                                          int scope,
                                          int searchType,
                                          int category,
                                          int destIndex,
                                          int fromIndex,
                                          int maxCnt);
    void responsedFreeTextSearchResult(const QVariant& result);
    void goPoiDetailScreenOccur(const QVariant& detail);
    void requestSearchAddressOccur(const QVariant& address);
    void requestSDSSearchAddressOccur(const QVariant& address); // sds service
    void responsedSearchAddress(const QVariant& address);
    void responsedSDSSearchAddress(const QVariant& address); // sds service
    void responsedLocationDetail(const QVariant& detail);
    void showDestinationFreeTextSearchInputOccur(const QString& searchString);

    // route
    void goViaPointEditScreenOccur();
    void goRouteSearchScreenOccur();
    void requestRouteOccur(int option, bool apply);
    void requestRouteOccur(TS_ROUTE_POS pos, int option, bool apply);
    void requestRouteOccur(QList<TS_ROUTE_POS> waypoints, int option, bool apply);
    void requestRouteOccur(QRoutePosListPtr waypoints, int option, bool apply);
    void requestReRouteOccur(int option);
    void requestReRouteOccur(TS_ROUTE_POS pos, int option);
    void requestReRouteOccur(QList<TS_ROUTE_POS> waypoints, int option);
    void requestAroundSearchParkingOccur();
    void requestAroundSearchPatrolOccur(int type);
    void cancelRouteOccur();
    void waypointChanged(TS_ROUTE_POS pos, int type);
    void waypointInserted(TS_ROUTE_POS pos, int type);
    bool waypointInsertedFromSelectInMap(int slot);
    void waypointsDeleted(int firstIndex, int lastIndex);
    void addFavoritesPositionOccur(TS_ROUTE_POS pos);
    void replaceWaypoint(uint index);
    void showCalculateTour(TS_ROUTE_POS pos, int option, bool apply);
    void showAdjustCurrentLocationScreen(TS_ROUTE_POS pos);

    void rePlayVoiceGuidanceOccur();
    void languageChanged(QString lang);
    void resetToFactorySettings();
    void dateFormatChanged(QString format);
    void timeFormatChanged(QString format);
    void goToStandbyOccur();
    void goToRunningOccur();
    void showOnMap(int worldX, int worldY, bool isKorea=true);
    
    void changeUserOccur(int id);
    void copyUserDataOccur(int sid,int tid);
    void copyUserDataDone(bool ok);
    void createUserOccur(int id);
    void createUserDone(bool ok);
    void deleteUserOccur(int id);
    void deleteUserDone(bool ok);
    void changeProfileNameOccur(const QString&);

    void sdiAlertForMuteChanged(bool mute);
    void searchTextChanged(QString text);
    void autoCompleteTextChanged(QString text);
    void dialPOICallStatusChanged(char DialPOICallStatus);
    void callServiceStatusChanged(char CallServiceStatus);
    void isHFPAvailable(bool PhoneCallStatus);    
    void languageIndexStatusChanged(char languageIndex);
    void colorStateStatusChanged(char colorStateStatus);
    void mexModeStatusChanged(char maxModeStatus);
    void applyHardKeyOccur(int key, int state);

    void gadgetTouched(uint touchState, uint xPos, uint yPos, uint timeStamp, int instanceId);

    void naviVolumeTouched(bool enabled); // show  sys voluem hmi 

    // misc
    void speakPhreaseSampleOccur(int phraseType);

    // hmi
    void hmiStateChanged(uint hmiState);
    void naviShutdownReadyFinished(bool ok);

    // rg cross image
    void crossImageVisibleChanged(bool visible);

    void naviSignalRFAntennaStatusOccur(uchar u8RFAntenna, uchar u8RFAntennaStatus, uchar u8ConnectionCurrent);

    void currentStreetChanged(RGDataPtr value);

    // ndds
    void channelReroutePolicyReceived(const QString& policy);
    void nddsServerPolicyFinished(bool success);

#ifdef QT_DEBUG
    void showMonkeyTest();
#endif

    void contextSwitchRequested(QVariant args);
    void contextSwitchResponded(QVariant args);

signals:
    void engineInitialized(bool success);
    void applicationWindowCreated(QQuickWindow* window);
    void postKeyEvent(quint32 state, quint32 nativeScanCode);
    void setupPopupWindow(QQmlEngine* qmlEngine);
    void mapDataChanged();
    void sendGadgetImage(QImage img, int type);
    void mapInClusterWindowCreated(QQuickWindow* window);
    void platformMediaMounted(const QString& mediaPath);
    void platformMediaUnmounted();
    void platformMediaRemounted(bool success);

    #ifdef TMAP_TMC_DEV_TESTING
    void setMapInClusterIpPort(QString ip, ushort port, ushort videoType, int width, int height);
    void startMapInCluster(int layout);
    void stopMapInCluster();
    void modifyMapInCluster(int width, int height);
    void resumeMapInCluster();
    void pauseMapInCluster();

    void changeMapInClusterLayout(int layout);
    #endif

    void debugKeyPressed(int key);
    void clientStatusGuiSenseReady();
    void setPauseMap(bool);
    void updateEditFieldText();

    void doneStartMapVideoStream();
    void spcxRgEnabled(bool);

    void stopSendingGadgetImageRequested();
    void contextStateAccepted();
    void contextStateCompleted();
    void platformNaviGadgetFocusStatusChanged(uint instanceId, bool focus);
    void platformNaviGadgetFocusInteractionTypeChanged(uint instanceId, uint interactionType);

private slots:
    void platformRemountMedia(int mountType, const QString& srcPath, const QString& dstPath=QString());
    void onPlatformRemountStatus(bool success);
    void onClientStatusGuiSenseReady();
    void onPlatformZone3PullUpListStateChanged(uint state);
    void onPlatformfocusConfigChanged(char type);
    void onPlatformNaviGadgetFocusStatusChanged(uint instanceId, bool focus);
    void onPlatformNaviGadgetFocusInteractionTypeChanged(uint instanceId, uint interactionType);

private:
    PlatformController* newPlatformController(TmapController *controller);
    PlatformConnectivity* newPlatformConnectivity();
    
    void initMapUpdate();
    void initEngine();
    void initUI();
    void initGuideUI();
    void setupAudio();
    void setupMapSwitcher();
    void destroyEngine();
    void onInitialized();
    QImage getGadgetImage();
    bool isGadgetRenderTarget();
    void createVrSymLink();
    void deleteVrSymLink();

private slots:
    void onVSMInitialized(bool success);
    void onRPnSearchInitialized(bool success);
    void onRPnSearchReInitialized(bool success);
    void onPlatformInitialized();
    void onPlatformUIRequested();
    void onPlatformDimmingStatusChanged(bool dimming);
    void onplatformDialPOICallStatusChanged(char DialPOICallStatus);
    void onplatformCallServiceStatusChanged(char CallServiceStatus);
    void onPlatformIsHFPAvailable(bool PhoneCallStatus);
    void onPlatformLanguageIndexStatusChanged(char languageIndex);
    void onPlatformColorStateStatusChanged(char colorState);
    void onPlatformMexModeStatusChanged(char mexMode);
    void onPlatformPrivateDataEncryptionRegistered(QObject* interface);
    void onPlatformActivateExternalImageProvider(bool activate, int instanceId);
    void onPlatformExternalImageProviderTouch(quint8 touchState, uint xPos, uint yPos, uint timeStamp, int instanceId);
    void onPlatformNaviSignalRFAntennaStatus(uchar u8RFAntenna, uchar u8RFAntennaStatus, uchar u8ConnectionCurrent);
    void onPlatformCurrentStreetChanged(RGDataPtr value);
    void updateMapData();
    void onUpdateCommitted(bool success);
    void onLocationUpdated(SKT::LocationDataPtr location);
    void setVideoStreamIpPort(QString ip, ushort port, ushort videotype, int width, int height);
    void startVideoStream(int layout);
    void stopVideoStream();
    void setVideoStream(const QImage& img);
    void destroyVideoStream();
    void resumeVideoStream();
    void pauseVideoStream();
//    void modifyVideoStream(int width, int height);
//    void onDoneStartMapStream();
    void onPlatformContextSwitchRequested(QVariant args);

public slots:
    void onCswBackKeyAccepted();

private:
    Q_DISABLE_COPY(TmapController)

    bool m_online;
    bool m_appForeground;
    bool m_networkOnline;
    SunState m_sunState;
    bool mIsShadeArea = false;
    WeatherState m_weatherState;

    PlatformController* m_platformController;
    PlatformConnectivity* m_platformConnectivity;
    QQmlApplicationEngine* m_qmlEngine;

    PackageManager* m_packageManager;
    GpsService* m_gpsService;
    RGSimulator* m_simulator;
    RGAudioHelper* m_audioHelper;
    SearchManager* m_searchManager;
    TmapProperty* m_tmapProperty;
    SessionManager* m_sessionManager;
    CipherServiceInterface* m_cipherService;
    CipherServiceInterface* m_aesCipherService;

    int m_initializeFlags;

    UILayout::Layout m_uiLayout;

    friend class DebugKeyFilter;
//    bool    m_IsDrawRender;
//    int     m_InstanceId;
#ifdef USE_MIC
    videostream* m_centerVideoStream = NULL;
    videostream* m_sideVideoStream = NULL;
#endif
    QMutex m_VideosMutex;
    int m_videoWidth = 0;
    int m_videoHeight = 0;
    bool m_videoStreamPlay;
    //test
    bool m_videoStreamResume;
    bool m_mapInCluster;
    
    int m_currentProfileId;
    bool m_readyToRunGadget = false;
    bool m_InitVSMView = false;

    int m_AvNextServerType = 2; // 0: RPD, 1: STG, 2:None
    char m_languageIndex = 0;
    char m_colorState = 0;
    char m_mexMode = 0;
    bool m_saveGadgetImage = false;
    QVariantMap m_gadgetSignal;
    //bool mRenderSceneDone = false;
    bool mAppHasCswFocus = true;
    bool mServerPolicyFetched = false;
    bool mServerDualcenterPolicySuccess = false;

    QVariantMap m_gpsRfAntennaStatus;
    CswFilter mCswFilter;
    bool mCswBackAccepted = false;
};

}

#endif // TMAPCONTROLLER_H
