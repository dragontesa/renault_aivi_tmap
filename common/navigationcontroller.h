#ifndef NAVIGATIONCONTROLLER_H
#define NAVIGATIONCONTROLLER_H

#include <QObject>
#include "navigationdata.h"

namespace SKT {

class SessionManagerInterface;
class GpsManagerInterface;
class SimulatorInterface;
class CipherServiceInterface;

class NavigationController
{
public:
    virtual ~NavigationController() {}

    /**
     * @brief qobject
     * @return
     */
    virtual QObject* qobject() = 0;

    /**
     * @brief sessionManager
     * @return
     */
    virtual SessionManagerInterface* sessionManager() = 0;

    /**
     * @brief gpsManager
     * @return
     */
    virtual GpsManagerInterface* gpsManager() = 0;

    /**
     * @brief simulatorManager
     * @return
     */
    virtual SimulatorInterface* simulatorManager() = 0;

    /**
     * @brief cipherService
     * @return
     */
    virtual CipherServiceInterface* cipherService(bool asQaes = false) = 0;

    /**
     * @brief online
     * @return true if network state is online.
     */
    virtual bool online() const = 0;

    /**
     * @brief appForeground
     * @return true if application is the foreground state.
     */
    virtual bool appForeground() const = 0;

    /**
     * @brief appVersion
     * @return
     */
    virtual QString appVersion() const = 0;

    /**
     * @brief navigationVolume
     * @return
     */
    virtual int navigationVolume() const = 0;

    /**
     * @brief muteState
     * @return
     */
    virtual bool muteState(int channel) const = 0;

    /**
     * @brief currentPosition
     * @param longitude
     * @param latitude
     * @return
     */
    virtual bool currentPosition(double &longitude, double &latitude) = 0;

    /**
     * @brief currentPosition
     * @param wX
     * @param wY
     * @return
     */
    virtual bool currentPosition(int &wX, int &wY) = 0;

    enum SunState {
        SunDawn = 0,
        SunDay,
        SunTwilight,
        SunNight
    };

    /**
     * @brief sunState
     * @return
     */
    virtual SunState sunState() const = 0;

    enum WeatherState {
        WeatherSunny = 0,
        WeatherRainy,
        WeatherSnowy
    };

    /**
     * @brief weatherState
     * @return
     */
    virtual WeatherState weatherState() const = 0;

    virtual void onReadyToRunGadget() = 0;

    virtual void stopSendingGadgetImage() = 0;

public slots:
    /**
     * @brief setNavigationVolume
     * @param volume
     */
    virtual void setNavigationVolume(int volume) = 0;

    /**
     * @brief setMuteState
     * @param mute
     */
    virtual bool setMuteState(int channel, bool mute) = 0;

    /**
     * @brief callNumber
     * @param number
     */
    virtual bool callNumber(const QString &number) = 0;

signals: // to Navi
    void onlineChanged(bool online);
    void appForegroundChanged(bool foreground);
    void navigationVolumeChanged(int volume);
    void muteStateChanged(bool mute);
    void sunStateChanged(SKT::NavigationController::SunState sun);
    void weatherStateChanged(SKT::NavigationController::WeatherState weather);
    //void cswPressed(int);
    void showGadget(int, bool, bool);
    void startMapInCluster();
    void stopMapInCluster();

    void mapViewModeChanged(int viewMode);
    void viewLevelChanged(int level, int subLevel);
    void trafficInfoScreenChanged(bool isShow);
    void moveToCurrentPositionOccur();
    void goViaPointEditScreenOccur();
    void goRouteSearchScreenOccur();
    void goPoiDetailScreenOccur(const QVariant& detail);

    // search for service
    void requestFreeTextSearchResultOccur(const QString& searchWords, int scope, int destIndex, int fromIndex, int maxCnt);
    void responsedFreeTextSearchResult(const QVariant& result);
    void requestSearchAddressOccur(const QVariant& address);
    void requestSDSSearchAddressOccur(const QVariant& address); // sds service
    void responsedSearchAddress(const QVariant& address);
    void responsedSDSSearchAddress(const QVariant& address); // sds service
    void responsedLocationDetail(const QVariant& detail);
    void showDestinationFreeTextSearchInputOccur(const QString& searchString);

    // route
    void requestRouteOccur(int option, bool apply);
    void requestRouteOccur(TS_ROUTE_POS pos, int option, bool apply);
    void requestRouteOccur(QList<TS_ROUTE_POS> waypoints, int option, bool apply);
    void requestReRouteOccur(int option);
    void requestReRouteOccur(TS_ROUTE_POS pos, int option);
    void requestReRouteOccur(QList<TS_ROUTE_POS> waypoints, int option);
    void cancelRouteOccur();
    void waypointChanged(TS_ROUTE_POS pos, int type);
    
    void addFavoritesPositionOccur(TS_ROUTE_POS pos);
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
    void languageIndexStatusChanged(char languageIndex);
    void callNumberDone(bool success);    

    void gadgetTouched(uint touchState, uint xPos, uint yPos, uint timeStamp, int instanceId);

    void naviVolumeTouched(bool enabled);
    
    void speakPhreaseSampleOccur(int phraseType);


    // hmi
    void hmiStateChanged(uint hmiState);
    void naviShutdownReadyFinished(bool ok);

    // gps antenna
    void naviSignalRFAntennaStatusOccur(uchar u8RFAntenna, uchar u8RFAntennaStatus, uchar u8ConnectionCurrent);

    enum ContextSwitchRequest {
        MAP_MAIN = 304,
        NAVIGATION_SERVICE = 334
    };

    enum ContextSwitchResponse {
        CONTEXT_STATE_ACCEPTED = 0,
        CONTEXT_STATE_REJECTED = 1,
        CONTEXT_STATE_COMPLETED =2,
        CONTEXT_STATE_INVALID =3
    };
    void contextSwitchRequested(QVariant args);

public slots:
    // configuration
    virtual void changeHostBaseName(const QString& hostBaseName) = 0;

    // map
    virtual bool setMapViewMode(int viewMode) = 0;
    virtual bool setViewLevel(int level, int subLevel) = 0;
    virtual bool setViewLevel(ViewLevelInfo level) = 0;
    virtual bool setTrafficInfoScreen(bool isShow) = 0;
    virtual bool moveToCurrentPosition() = 0;
    virtual bool goViaPointEditScreen() = 0;
    virtual bool goRouteSearchScreen() = 0;

    // search
    virtual void requestFreeTextSearchResult(const QString& searchWords,
                                             int scope,
                                             int searchType,
                                             int category,
                                             int destIndex,
                                             int fromIndex,
                                             int maxCnt) = 0;
    virtual void requestSearchAddress(const QVariant& address) = 0;
    virtual void requestSDSSearchAddress(const QVariant& address) = 0; // sds service
    virtual void requestLocationDetail(const TS_ROUTE_POS& location) = 0;
    virtual void goPoiDetailScreen(const QVariant& detail) = 0;
    virtual void showDestinationFreeTextSearchInput(const QString& searchString) = 0;

    // route
    virtual bool requestRoute(int option, bool apply) = 0;
    virtual bool requestRoute(TS_ROUTE_POS pos, int option, bool apply) = 0;
    virtual bool requestRoute(QList<TS_ROUTE_POS> waypoints, int option, bool apply) = 0;
    virtual bool requestReRoute(int option = -1) = 0;
    virtual bool requestReRoute(TS_ROUTE_POS pos, int option = -1) = 0;
    virtual bool requestReRoute(QList<TS_ROUTE_POS> waypoints, int option = -1) = 0;
    virtual bool cancelRoute() = 0;
    virtual bool setWaypoint(TS_ROUTE_POS pos, int type) = 0;

    virtual bool addFavoritesPosition(TS_ROUTE_POS pos) = 0;
    virtual bool rePlayVoiceGuidance() = 0;
    virtual bool setSDIAlertForMute(bool mute) = 0;

    virtual void onGearStateChanged(QString state) = 0;
    virtual void onLanguageChanged(QString lang) = 0;
    virtual void onResetToFactorySettings() = 0;
    virtual void onDateFormatChanged(QString format) = 0;
    virtual void onTimeFormatChanged(QString format) = 0;
    virtual void onGoToStandby() = 0;
    virtual void onGoToRunning() = 0;    

    virtual void changeMapInClusterImage( const QImage& image ) = 0;
    virtual void changeIconGadgetImage( const QImage& image, int type ) = 0;
    virtual void changeGadgetImage(const QImage& image, int type) = 0;
    virtual void saveGadgetImage( bool save ) = 0;

    // hmi
    virtual void onHmiStateChanged(uint hmiState) = 0;

    // ui context
    virtual void onMapStartedToRender() = 0;
    virtual void respondTocontextSwitchRequest(QVariant args) = 0;

public:
    virtual bool VSMWorldToWGS84(int x, int y, double &gx, double &gy) = 0;
    virtual bool WGS84ToVSMWorld(double gx, double gy, int &x, int &y) = 0;
    virtual bool getVSMWorldToWGS84(int x, int y, double &gx, double &gy) = 0;
    virtual bool getWGS84ToVSMWorld(double gx, double gy, int &x, int &y) = 0;
    virtual bool getMapCenterLocation(TS_ROUTE_POS &location) = 0;
    virtual QString getDeviceId() = 0;
    virtual int getCarconfigurationValue(const uchar key ) = 0;

    virtual void onCrossImageVisibleChanged(bool visible) = 0; // from ui

signals: // to platform
    void CompleteMapUpdate();
};

}

Q_DECLARE_METATYPE(SKT::NavigationController::SunState)
Q_DECLARE_METATYPE(SKT::NavigationController::WeatherState)
Q_DECLARE_INTERFACE(SKT::NavigationController, "com.skt.tmap.NavigationController/1.0")

#endif // NAVIGATIONCONTROLLER_H
