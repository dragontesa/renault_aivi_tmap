#ifndef NAVIGATIONCONTROLLERPROXY_H
#define NAVIGATIONCONTROLLERPROXY_H

#include "AbstractProxy.h"
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QRunnable>
#include "../common/navigationcontroller.h"
#include "../common/navigationproperty.h"
#include "../common/satellitedataptr.h"
#include "../common/nmeatype.h"
#include "VSM_Navi.h"
#include "VSM_Map.h"
#include "VSM_Map_Settings.h"
#include "ProfileManager.h"
#include "OffscreenRenderer.h"
#include "RgProtocol.h"

namespace SKT {

T_SHAREABLE( NavigationControllerProxy );
T_SHAREABLE( RequestCommand );

class NavigationControllerProxy : public AbstractProxy
{
    Q_OBJECT
public:
    enum NightMode {
        Auto,   //자동
        Day,    //주간
        Night   //야간
    };

    enum MapMode {
        Far,    //지도 더 멀리 보기
        Normal, //지도 기본 보기(추천)
        Near    //지도 더 자세히 보기
    };

    enum FontSize {
         SMALL, //작게
         NORMAL,//보통
         LARGE  //크게
    };


    enum CarConfReqType {
        VIHICLE_MODEL = 1,
        VIHICLE_FUEL = 18,
        VIHICLE_PETROL = 19,
    };

    enum CarConfResFuelType {
        PETROL,
        DIESEL,
        HYBRID,
        HYBRIDPETROL,
        ELECTRICITY,
    };

    NavigationControllerProxy(QObject* controller, QObject* property, char languageIndex, QVariantMap gadgetSignal, QVariantMap gpsRfSignal, QVariantMap extraData);
    NavigationController* getControllerHandle();

    QString activeUserProfileName() const;
    QString lastUserProfileName() const;
    void addMapView( QObject* mapView );
    void removeMapView( QObject* mapView );

    void applyCommand(const RequestCommandPtr& req);
    void requestCommand(const RequestCommandPtr& req);

    int getNightMode();    
    bool getAppVersion(QVariantMap data);
    bool getGPSInfo();
    bool getNaviVolume();
    bool getNaviMute();
    QList<void*> getMapviewHandle() const;
    const QList<QVariantMap>* getVoiceList() const;
    int getSunState();
    int getWeatherState();
    bool getDateFormat();
    bool getTimeFormat();
    bool getSafeDrivingSoundAtMute();

    void showSysVolumeHMI(bool enabled);

    bool applySpeedLevel();
    bool applySpeedLevel(bool sppeedZoomLevel);
    bool applyVoiceGuidance();
    bool applyFontScale();
    bool applyVoice();
    bool applyNaviVolume(int volume);
    bool applyNaviMute(bool mute);
    bool applyNightMode(bool automatically = false);
    bool applyCameraGuidance(bool send = true);
    bool applyAttentionGuidance(bool send = true);
    bool applyRoadViolation(bool send = true);
    bool applySafeDrivingSoundAtMute(bool send = true);
    bool applyAutoFuelType(int type);
    bool applyVoiceGuidanceVocalPoint(bool send = true);

    void requestSetting();
    void requestSetting(const QString& section, const QString& key);

    void sendNotiNaviVolume(int volume);
    void sendNotiNaviMute(bool mute);

    void sendNotiGetVoiceGuidanceActive();
    void sendNotiSetVoiceGuidanceActive(bool active);

    void sendNotiSetOverSpeedSoundAlertInfoActive(bool active);

    void notifyUserDataModified(int dataType, const QVariant& data=QVariant());

    bool callPhoneNumber(QString phoneNumber);

    void clearSession();
    void clearAllSession();

    void initVoiceList();

    void changeLanguage(const QString& lang);
    bool getOnlineState() const;

    // it only used on engineer mode
    void changeNddsServer(bool commerce);
    void changeVsmServer(int serverType);
    void changeAvNextServer(int serverType);
    void enableSunstateTimeset(bool enabled);

    void setPlatformController(QObject*);
    void showPopup(const QString& viewName);
    void hidePopup(const QString& viewName);
    void beep(int id);
    void setZone1(const QVariant& v);
    void setZone3(const QVariant& v);
    void setKeyboard(const QVariant& v);

    QString mountedMediaPath() const;
    void    remountMedia(bool readonly);

    void registerMapInClusterRenderer(OffscreenRenderer* renderer);
    bool registerGadgetRenderer(OffscreenRenderer* renderer, QObject* imageProvider, QQuickWindow* gadgetDebugWindow);
    void registerOffscreenRenderer(OffscreenRenderer* renderer);

    void mapViewInitialized();

    void showGadget(int type, bool show, bool asWindow);
    void saveGadgetImage(bool save);

    void setAlertForMute(bool mute);

    void fileGpsSetPos( double percent );
    void fileGpsRewind();
    void fileGpsFastForward();

    void requestToViewList(int viewId);
    void sendReplyToViewList(const QList<int> data, const int findId);

    void switchToAppIfNecessary();

    void onCrossImageVisibleChanged(bool visible);

    void onMapStartedToRender();
    void setAppLostCswFocus();
    void reserveAppSwitchJob(const QVariantMap &m);
    void reserveSystemSwitchJob(const QVariantMap &m);
    void runReservedJob(const QVariantMap &m);
    void runReservedAppSwitchJob();
    void runReservedSystemSwitchJob();

    bool isSpcxRgEnabled() const;

    void respondToContextSwitchRequest(QVariant args);
    bool isChannelServerPolicyFinished() const;
    bool isChannelServerPolicyDone() const;

    void setIsRg(bool b);

    void emitCswBackKeyAccepted();

private slots:
    void setSunState(SKT::NavigationController::SunState sunState);
    void setWeatherState(SKT::NavigationController::WeatherState weatherState);
    void setNightMode(bool nightMode);
    void navigationVolumeChanged(int volume);
    void setMuteState(bool mute);
    void navigationAppeared(bool appeared);
    void onlineChanged(bool online);
    //void processCsw(int);

    // it only used on engineer mode
    void updatedDaynightTimeset(const QList<QDateTime>&);

    //TMCRSM-2446:VR로부터 음성안내on/off 명령을 받는 인터페이스
    void onSetVoiceGuidanceOccur(bool active);
    void onSetAlramGuidanceOccur(bool active);

    //TMCRSM-2452:VR로부터 좌표값으로 상세보기 화면으로 이동 명령을 받는 인터페이스
    void onGoPoiDetailScreenOccur(const QVariant& detail);

    void onShowGadget(int type, bool show, bool asWindow);
    void onGadgetActivated(uint,QList<int>,int);
    void onGadgetTouched(uint touchState, uint xPos, uint yPos, uint timeStamp, int instanceId, bool byInterface = false);
    void onGadgetLongTouched();
    void onGadgetFocused(uint instanceId, bool focus);
    void onGadgetFocusInteraction(uint instanceId, int interactionType);
    void onMapViewModeChanged(int mode);
    void onViewLevelChanged(int level,int sub_level);
    void onTrafficInfoScreenChanged(bool isShow);
    void onMoveToCurrentPosition();
    void onGoViaPointEditScreen();
    void onShowOnMap(int worldX, int worldY, bool isKorea=true);
    void onRequestRoute(int option, bool apply);
    void onRequestRoute(TS_ROUTE_POS pos, int option, bool apply);
    void onRequestRoute(QList<TS_ROUTE_POS> wayPoints, int option, bool apply);
    void onRequestRoute(QRoutePosListPtr wayPoints, int option, bool apply);
    void onRequestReRoute(int option);
    void onRequestReRoute(TS_ROUTE_POS pos, int option);
    void onRequestReRoute(QList<TS_ROUTE_POS> wayPoint, int option);
    void onShowCalculateTour(TS_ROUTE_POS pos, int option, bool apply);
    void requestAroundSearchParking();
    void requestAroundSearchPatrol(int type);
    void onRequestFreeTextSearchResultOccur(const QString& searchWords,
                                            int scope,
                                            int searchType,
                                            int category,
                                            int destIndex,
                                            int fromIndex,
                                            int maxCnt); // searchType 0: freetext 1: category search
    void onRequestSearchAddressOccur(const QVariant& request, const QString& resultHandler=QStringLiteral("responsedSearchAddress")); // owner: 0: navigation VR service, 1: SDS service
    void onRequestSDSSearchAddressOccur(const QVariant& request);
    void onShowDestinationFreeTextSearchInputOccur(const QString& searchString);
    void onCancelRoute();
    void onWaypointChanged(TS_ROUTE_POS pos, int type);
    void onWaypointInserted(TS_ROUTE_POS pos, int type);
    void onWaypointsDeleted(int firstIndex, int lastIndex);
    void onWaypointInsertedFromSelectInMap(int slot);
    void onReplaceWaypoint(uint index);
    void onAddFavoritesPosition(TS_ROUTE_POS pos);
    void onRePlayVoiceGuidance();
    void onLanguageChanged(QString format);
    void onResetToFactorySettings();
    void onDateFormatChanged(QString format);
    void onTimeFormatChanged(QString format);
    void onGoToStandby();
    void onGoToRunning();
    void onGoRouteSearchScreen();
    void onSDIAlertForMuteChanged(bool mute);
    void onSatelliteUpdated(SKT::SatelliteDataPtr satellite);
    void onSessionCreated(bool success, int validateCode, int errCode);
    void onSearchTextChanged(QString text);
    void onAutoCompleteTextChanged(QString text);
    void onDialPOICallStatusChanged(char DialPOICallStatus);
    void onCallServiceStatusChanged(char CallServiceStatus);
    //void onIsHFPAvailable(bool PhoneCallStatus);
    void onSpeakPhreaseSampleOccur(int);
    void onShowAdjustCurrentLocationScreen(TS_ROUTE_POS pos);

    void onStartMapInCluster(int layout);
    void onStopMapInCluster();
    void onChangeMapInClusterLayout(int layout);

    // handle platform usb device mounting state 
    void onMediaMounted(const QString& mediaPath);
    void onMediaUnmounted();

    void onLanguageIndexStatusChanged(char languageIndex);
    void onColorStateStatusChanged(char colorState);
    void onMexModeStatusChanged(char mexMode);

    void onCallNumberDone(bool success);

    void onApplyHardKey(int key, int state);    
    void onDebugKeyPressed(int key);
    void showMonkeyTest();

    // hmi
    void onHmiStateChanged(uint hmiState);

    void onClientStatusGuiSenseReady();
    void setPauseMap(bool);
    void onUpdateEditFieldText();

    void onNaviSignalRFAntennaStatusOccur(uchar u8RFAntenna, uchar u8RFAntennaStatus, uchar u8ConnectionCurrent);
    void onSpcxRgEnabled(bool);
    void onChannelReroutePolicyReceived(const QString& policy);
    void onChannelServerPolicyFinished(bool success);

    void onContextSwitchRequested(QVariant args);
    void onContextStateAccepted();
    void onContextStateCompleted();
    void onPlatformNaviGadgetFocusStatusChanged(uint instanceId, bool focus);
    void onPlatformNaviGadgetFocusInteractionTypeChanged(uint instanceId, uint interactionType);

signals:
    void nddsServerChanged(bool commerce);
    void vsmServerChanged(int servertype);
    void avnextServerChanged(int serverType);
    void mediaRemounted(bool success);
    void responseToViewList(QList<int> data);
    void appLostCswFocus();
    void cswBackKeyAccepted();

private:
    void initDriveAutoConfig();
    void parseTmapClientBehavior(const QString &serviceItems);
    bool parseTmapClientBehaviorValue(const QString &item, int &ret);
    void sendGadgetPressedNotification(int instanceId, bool focused, bool pressed);

private:
    NavigationController* mController;
    NavigationProperty* mProperty;
    SKT::SatelliteDataPtr mSatellite;

    int mCountAuthOTPFail;
    QTime mTime;

    int mNightMode;
    VSM_RG_CONFIG  mRgConfig;
    bool mFirstAppear;
    bool mIsOnline;
    bool mIsBluetooth;
    bool mForeground = false;
    bool mIsSpcxRgEnabled = false;
    QScopedPointer<ProfileManager> mProfileManager;
    QList<QVariantMap> mVoiceList;
    QString mDateFormat;
    QString mTimeFormat;
    QString mMountedMediaPath;
    QString mLang;
    QList<QObject*> mMapViewList;
    QList<QVariantMap> mReservedAppSwitchJob;
    QList<QVariantMap> mReservedSystemSwitchJob;
    QPoint mDestPos;

    QObject* mPlatformController;

    int mTouchState = 0;
    int mTouchX = 0;
    int mTouchY = 0;
    QTimer mGadgetTouchTimer;

    RgSignalState::RgSignalStateK mSignalState = RgSignalState::NONE;

    OffscreenRenderer* mGadgetRenderer = nullptr;
    bool mIsResetNaviDone = false;
    bool mChannelServerPolicyFinished = false;
    bool mChannelServerPolicyDone = false;
    bool mIsRg = false;
    bool mIsInSwitchToApp = false;
    QMap<int,bool> mWidgetFocusMap;
    int mFocusWidget = -1;
};

}

#endif // NAVIGATIONCONTROLLERPROXY_H

