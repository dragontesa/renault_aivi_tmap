#ifndef PLATFORMCONTROLLER_QNX_H
#define PLATFORMCONTROLLER_QNX_H

#include <QPointer>
#include "platformcontroller.h"
#ifdef PLATFORM_QNX
#include "navi/service/BaseNaviService.h"
#include "navi/NaviServiceProvider.h"
#include "navi/NaviDeploy.h"
#include "appservice/AppMsgDef.h"
#include "interface/appservice.h"
#include "interface/locationservice.h"
#include "interface/keyboardclient.h"
#include "interface/popupservice.h"
#include "interface/resservice.h"
#include "interface/systemsetting.h"
#include "interface/clusterservice.h"
#include "interface/userprofileservice.h"
#include "interface/canservice.h"
#include "interface/ccfservice.h"
#ifdef USBSTATUS_QNXAPI
#include "interface/usbstatus.h"
#endif
#include "interface/btservice.h"
#include "interface/dcmservice.h"
#include "interface/tjpservice.h"
#include "interface/systemservice.h"
#include "interface/usbdetectionmanager.h"
#include "interface/partitionctrl.h"
#include "interface/connectivityservice.h"
#include "interface/authservice.h"
#include "interface/windowservice.h"
#include "interface/vrservice.h"
#include "interface/notificationservice.h"
#endif


namespace SKT {

class TmapController;

#ifdef PLATFORM_QNX
class PlatformControllerQnx : public PlatformController, public BaseNaviService
#else
class PlatformControllerQnx : public PlatformController
#endif
{
    Q_OBJECT
public:
    explicit PlatformControllerQnx(TmapController* controller);
    virtual ~PlatformControllerQnx();

    void startService() override;
    void finishService() override;
    void init() override;
    QString deviceId() const override;
    bool remount(const QString &path, bool readOnly) override;
    bool restart() override;
    bool callNumber(const QString &number) override;
    bool dimmingStatus() const override;
    bool networkOnline() const override;
    bool appForeground() const override;
    char reverseGearState() const override;
    char LanguageIndex() const override;
    char DateFormat() const override;
    char TimeFormat() const override;
    uint changeToProfile(const uchar userID) const override;
    uint copyProfile(const uchar sourceUserID, uchar destUserID) const override;
    uint createProfile(const QString &newName, uchar imageID) const override;
    uint createProfile(const QString &newName, uchar imageID, uchar &newUserID) const override;
    uint deleteProfile(uchar userID) const override;
    uint importProfileImage(const QString &sourcePath, ushort width, ushort height, ushort slotIdx, bool bSetForCurrentProfile) const override;
    uint doRegisterApp(const QString &AppName) override;
    uint doRegisterApp(const QString &AppName, QString &ApplName, bool &reloadData) const;
    uint doSetProfileImage(uchar imageID, uchar userID) const override;
    uint doSetProfileName(const QString &newName) const override;
    uint doSetProfileName(const QString &newName, uchar userID) override;
    QByteArray getProfileImages() const override;
    QStringList getProfileNames() const override;
    uint importProfileImage(const QString &sourcePath, ushort width, ushort height) const override;
    void onDataChanged(uint ApplicationPID, uint Result) const override;
    void onProfileCopied(uint ApplicationPID, uint Result) const override;
    void onProfileCreated(uint ApplicationPID, uint Result) const override;
    void onProfileDeleted(uint ApplicationPID, uint Result) const override;
    uint removeUserImage(ushort slotIdx) const override;
    uint setProfileImage(uchar imageID) const override;
    void displayZone3OnScroll(bool visibilityState) const override;
    void setEditfieldText(uchar appId, const QString &searchBarText, bool isHighLightRequired, uchar highlightStartIndex, uchar highlightCount) const override;
    void setKeyboard(uchar appId, uchar keyboardType, const QString &validCharacters)const override;
    void setSDSType(uint sdsType, uchar value)const override;
    void setZone3SubTextInfo(const QStringList &zone3SubText)const override;

    void showPopup(const QString &viewname)const override;
    void hidePopup(const QString &viewname)const override;

    // JLR2019 API
public:
#ifdef PLATFORM_QNX // System Setting
    char* getSystemLanguage();
    UI_THEME getSystemUITheme();
    UI_THEME getNaviMapUITheme();
#endif

#ifdef PLATFORM_QNX
    //Navi service test API
    virtual void requestToTest(const int& id, const bool& bFlag, const char* msg) override;

    //Navi setting APIs
    virtual void getGeneralSettings(const int32_t nReqID) override;
    virtual void setGeneralShowQibla(const int32_t nReqID, const E_NAVI_ONOFF_STATUS& status) override;
    virtual void setGeneralAddressFormat(const int32_t nReqID, const E_MAP_ADRESS_TYPE& type) override;
    virtual void setGeneralMuteGuidance(const int32_t nReqID, const E_NAVI_ONOFF_STATUS& mode) override;
    virtual void setGeneralNotificationVisualCueSafetyZone(const int32_t nReqID, const E_NAVI_ONOFF_STATUS& status) override;
    virtual void setGeneralNotificationPlaySoundSafetyZone(const int32_t nReqID, const E_NAVI_ONOFF_STATUS& status) override;
    virtual void setGeneralAlertTypeList(const int32_t nReqID, const NAVI_GENERAL_ALERT_TYPE_LIST_T& list) override;
    virtual void setGeneralWarningTypeList(const int32_t nReqID, const NAVI_GENERAL_WARNING_TYPE_LIST_T& list) override;
    virtual void setGeneralNotificationVisualCueSpeedCamera(const int32_t nReqID, const E_NAVI_ONOFF_STATUS& status) override;
    virtual void setGeneralNotificationPlaySoundSpeedCamera(const int32_t nReqID, const E_NAVI_ONOFF_STATUS& status) override;
    virtual void setGeneralNotificationLowFuel(const int32_t nReqID, const E_NAVI_ONOFF_STATUS& status) override;
    virtual void setGeneralEditNumberPlate(const int32_t nReqID, const std::string& numberPlate) override;
    virtual void setGeneralTextSize(const int32_t nReqID, const E_MAP_TEXT_SIZE& size) override;
    virtual void getHybridSettings(const int32_t nReqID) override;
    virtual void setHybridShowChargePoint(const int32_t nReqID, const E_NAVI_ONOFF_STATUS& status) override;
    virtual void setHybridShowAvailableChargeStation(const int32_t nReqID, const E_NAVI_ONOFF_STATUS& status) override;
    virtual void setHybridShowOpenChargeStation(const int32_t nReqID, const E_NAVI_ONOFF_STATUS& status) override;
    virtual void setHybridConnectorTypeList(const int32_t nReqID, const NAVI_HYBRID_CONNECTOR_TYPE_LIST_T& list) override;
    virtual void setHybridChargeSpeed(const int32_t nReqID, const NAVI_HYBRID_CHARGE_SPEED_LIST_T& list) override;
    virtual void setHybridProvider(const int32_t nReqID, const NAVI_HYBRID_PROVIDER_LIST_T& list) override;
    virtual void setHybridPaymentTypeList(const int32_t nReqID, const NAVI_HYBRID_PAYMENT_TYPE_LIST_T& list) override;
    virtual void getMapSettings(const int32_t nReqID) override;
    virtual void setMapSatellite(const int32_t nReqID, const E_NAVI_ONOFF_STATUS& status) override;
    virtual void setMapTrafficOnMap(const int32_t nReqID, const E_NAVI_ONOFF_STATUS& status) override;
    virtual void setMapTrafficOnTargetRoad(const int32_t nReqID, const NAVI_MAP_TRAFFIC_ROAD_LIST_T& roadList) override;
    virtual void setMapTrafficFlow(const int32_t nReqID, const NAVI_MAP_TRAFFIC_FLOW_LIST_T& flowList) override;
    virtual void setMapTrafficAccident(const int32_t nReqID, const E_NAVI_ONOFF_STATUS& status) override;
    virtual void setMapTrafficParking(const int32_t nReqID, const E_NAVI_ONOFF_STATUS& status) override;
    virtual void setMapTrafficSAPA(const int32_t nReqID, const E_NAVI_ONOFF_STATUS& status) override;
    virtual void setMapRecentDestination(const int32_t nReqID, const E_NAVI_ONOFF_STATUS& status) override;
    virtual void setMapFavourites(const int32_t nReqID, const E_NAVI_ONOFF_STATUS& status) override;
    virtual void setMapAutoZoom(const int32_t nReqID, const E_NAVI_ONOFF_STATUS& status) override;
    virtual void setMapViewMode(const int32_t nReqID, const E_MAP_VIEW_MODE &mode) override;
    virtual void setMapDynamicView(const int32_t nReqID, const E_MAP_DYNAMIC_VIEW_MODE& mode) override;
    virtual void setMap3DBuilding(const int32_t nReqID, const E_NAVI_ONOFF_STATUS& status) override;
    virtual void setMapPOIDisplyPreference(const int32_t nReqID, const E_NAVI_ONOFF_STATUS& status) override;
    virtual void setMapDisplaySavedPOIIcon(const int32_t nReqID, const E_NAVI_ONOFF_STATUS& status) override;
    virtual void setMapPOIDisplayOnMapList(const int32_t nReqID, const NAVI_POI_LIST_T& list) override;
    virtual void setMapHighwayMode(const int32_t nReqID, const E_NAVI_ONOFF_STATUS& status) override;
    virtual void setMapSkybox(const int32_t nReqID, const E_NAVI_ONOFF_STATUS& status) override;
    virtual void getRGSettings(const int32_t nReqID) override;
    virtual void setRGDynamicRouting(const int32_t nReqID, const E_NAVI_ONOFF_STATUS& status) override;
    virtual void setRGRouteAutoRecalculate(const int32_t nReqID, const E_NAVI_ONOFF_STATUS& status) override;
    virtual void setRGRoutingMode(const int32_t nReqID, const E_ROUTE_ROUTING_MODE& mode) override;
    virtual void setRGRoutePreferenceList(const int32_t nReqID, const NAVI_ROUTE_PREFERENCE_LIST_T& list) override;
    virtual void setRGCommuteLearnRoute(const int32_t nReqID, const E_NAVI_ONOFF_STATUS& status) override;
    virtual void setRGCommuteAutoStart(const int32_t nReqID, const E_NAVI_ONOFF_STATUS& status) override;
    virtual void setRGCommuteFewerCommand(const int32_t nReqID, const E_NAVI_ONOFF_STATUS& status) override;
    virtual void requestRGDeleteDrivingHistory(const int32_t nReqID) override;
    virtual void setRGSuggestionAlongList(const int32_t nReqID, const NAVI_ROUTE_SUGGESTION_LIST_T& list) override;
    virtual void setRGRoadSignBoard(const int32_t nReqID, const E_NAVI_ONOFF_STATUS& status) override;
    virtual void setRGRailroadCrossing(const int32_t nReqID, const E_NAVI_ONOFF_STATUS& status) override;
    virtual void setRGIntersectionView(const int32_t nReqID, const E_NAVI_ONOFF_STATUS& status) override;
    virtual void setRGAvoidPlateNumRestrict(const int32_t nReqID, const E_NAVI_ONOFF_STATUS& status) override;
    virtual void setRGArrivalTimeMode(const int32_t nReqID, const E_GUIDE_ARRIVAL_TIME& mode) override;
    virtual void setRGArrivalTimeTo(const int32_t nReqID, const E_GUIDE_ESTIMATED_TIME& mode) override;
    virtual void getVICSSettings(const int32_t nReqID) override;
    virtual void setVICSTrafficStationChoice(const int32_t nReqID, const E_NAVI_AUTOMANUAL_STATUS& status) override;
    virtual void setVICSProbeInformation(const int32_t nReqID, const E_NAVI_ONOFF_STATUS& status) override;
    virtual void setVICSETCSoundNotification(const int32_t nReqID, const E_NAVI_ONOFF_STATUS& status) override;
    virtual void setVICSDSSSFromBeacon(const int32_t nReqID, const E_NAVI_ONOFF_STATUS& status) override;
    virtual void getClusterSettings(const int32_t nReqID) override;
    virtual void setClusterJunctionViewPicture(const int32_t nReqID, const E_NAVI_ONOFF_STATUS& status) override;
    virtual void setClusterJunctionViewOnMain(const int32_t nReqID, const E_NAVI_ONOFF_STATUS& status) override;
    virtual void setClusterLaneAssistancePicture(const int32_t nReqID, const E_NAVI_ONOFF_STATUS& status) override;
    virtual void setClusterLaneAssistanceOnMain(const int32_t nReqID, const E_NAVI_ONOFF_STATUS& status) override;
    virtual void setClusterAbstractRouteOverview(const int32_t nReqID, const E_NAVI_ONOFF_STATUS& status) override;
    virtual void setClusterNextWaypoint(const int32_t nReqID, const E_NAVI_ONOFF_STATUS& status) override;
    virtual void setClusterRoutingInformation(const int32_t nReqID, const E_NAVI_ONOFF_STATUS& status) override;
    virtual void setClusterSatelite(const int32_t nReqID, const E_NAVI_ONOFF_STATUS& status) override;
    virtual void setClusterMirrorInfoPanel(const int32_t nReqID, const E_NAVI_ONOFF_STATUS& status) override;
    virtual void getVoiceSettings(const int32_t nReqID) override;
    virtual void setVoiceAnnouncementFrequencyList(const int32_t nReqID, const E_GUIDE_VOICE_FREQUENCY& selectedFrequency) override;
    virtual void setVoiceNaturalLanguageGuidance(const int32_t nReqID, const E_NAVI_ONOFF_STATUS& status) override;
    virtual void setGeneralAnnouncementDistance(const int32_t nReqID, const E_NAVI_GENERAL_ANNOUNCEMENT_DISTANCE& distance) override;
    virtual void setVoiceAnnouncementBeep(const int32_t nReqID, const E_NAVI_ONOFF_STATUS& status) override;
    virtual void setVoiceReroutingAnnouncement(const int32_t nReqID, const E_NAVI_ONOFF_STATUS& status) override;
    virtual void getAboutSettings(const int32_t nReqID) override;
    virtual void setAboutGPSInfo(const int32_t nReqID, const NAVI_ABOUT_GPS_INFO_T& info) override;
    virtual void getAccountSettings(const int32_t nReqID) override;
    virtual void requestAccountClearUserPreferences(const int32_t nReqID) override;

    void onGetGeneralSettings(int nReqID, const NAVI_GENERAL_SETTINGS& settings, const NAVI_GENERAL_DISABLED_SETTINGS& disabledsettings);
    void onRespondGeneralShowQibla(int nReqID, const NAVI_ONOFF_STATUS& status);
    void onRespondGeneralAddressFormat(int nReqID, const MAP_ADRESS_TYPE& type);
    void onRespondGeneralMuteGuidance(int nReqID, const NAVI_ONOFF_STATUS& mode, const NAVI_SETTINGS_RES_ENUM respond);
    void onRespondGeneralNotificationVisualCueSafetyZone(int nReqID, const NAVI_ONOFF_STATUS& status);
    void onRespondGeneralNotificationPlaySoundSafetyZone(int nReqID, const NAVI_ONOFF_STATUS& status);
    void onRespondGeneralAlertTypeList(int nReqID, NAVI_GENERAL_ALERT_TYPE_LIST& list, NAVI_SETTINGS_RES_ENUM respond);
    void onRespondGeneralWarningTypeList(int nReqID, NAVI_GENERAL_WARNING_TYPE_LIST& list, NAVI_SETTINGS_RES_ENUM respond);
    void onRespondGeneralNotificationVisualCueSpeedCamera(int nReqID, const NAVI_ONOFF_STATUS& status);
    void onRespondGeneralNotificationPlaySoundSpeedCamera(int nReqID, const NAVI_ONOFF_STATUS& status);
    void onRespondGeneralNotificationLowFuel(int nReqID, const NAVI_ONOFF_STATUS& status);
    void onRespondGeneralEditNumberPlate(int nReqID, const std::string& numberPlate);
    void onRespondGeneralTextSize(int nReqID, const MAP_TEXT_SIZE& size);

    void onGetHybridSettings(int nReqID, NAVI_HYBRID_SETTINGS& settings, NAVI_HYBRID_DISABLED_SETTINGS& disabledSettings);
    void onRespondHybridShowAvailableChargeStation(int nReqID, NAVI_ONOFF_STATUS status, NAVI_SETTINGS_RES_ENUM respond);
    void onRespondHybridShowOpenChargeStation(int nReqID, NAVI_ONOFF_STATUS status, NAVI_SETTINGS_RES_ENUM respond);
    void onRespondHybridConnectorTypeList(int nReqID, NAVI_HYBRID_CONNECTOR_TYPE_LIST& list, NAVI_SETTINGS_RES_ENUM respond);
    void onRespondHybridChargeSpeed(int nReqID, NAVI_HYBRID_CHARGE_SPEED_LIST& list, NAVI_SETTINGS_RES_ENUM respond);

    void onGetMapSettings(int nReqID, const NAVI_MAP_SETTINGS& settings, const NAVI_MAP_DISABLED_SETTINGS& disabledsettings);
    void notifySetMapAutoZoom(const NAVI_ONOFF_STATUS& status);

    void onRespondMapSatellite(int nReqID, NAVI_ONOFF_STATUS status, NAVI_SETTINGS_RES_ENUM respond);
    void onRespondMapTrafficOnMap(int nReqID, NAVI_ONOFF_STATUS status, NAVI_SETTINGS_RES_ENUM respond);
    void onRespondMapTrafficOnTargetRoad(int nReqID, NAVI_MAP_TRAFFIC_ROAD_LIST& road, NAVI_SETTINGS_RES_ENUM respond);
    void onRespondMapTrafficFlow(int nReqID, TRAFFIC_FLOW flow, NAVI_SETTINGS_RES_ENUM respond);
    void onRespondMapTrafficAccident(int nReqID, NAVI_ONOFF_STATUS status, NAVI_SETTINGS_RES_ENUM respond);
    void onRespondMapTrafficParking(int nReqID, NAVI_ONOFF_STATUS status, NAVI_SETTINGS_RES_ENUM respond);
    void onRespondMapTrafficSAPA(int nReqID, NAVI_ONOFF_STATUS status, NAVI_SETTINGS_RES_ENUM respond);
    void onRespondMapRecentDestination(int nReqID, NAVI_ONOFF_STATUS status, NAVI_SETTINGS_RES_ENUM respond);
    void onRespondMapFavourites(int nReqID, NAVI_ONOFF_STATUS status, NAVI_SETTINGS_RES_ENUM respond);
    void onRespondMapAutoZoom(int nReqID, NAVI_ONOFF_STATUS status, NAVI_SETTINGS_RES_ENUM respond);
    void onRespondMapViewMode(int nReqID, MAP_VIEW_MODE mode, NAVI_SETTINGS_RES_ENUM respond);
    void onRespondMapDynamicView(int nReqID, MAP_DYNAMIC_VIEW_MODE mode, NAVI_SETTINGS_RES_ENUM respond);
    void onRespondMap3DBuilding(int nReqID, NAVI_ONOFF_STATUS status, NAVI_SETTINGS_RES_ENUM respond);
    void onRespondMapPOIDisplyPreference(int nReqID, NAVI_ONOFF_STATUS status, NAVI_SETTINGS_RES_ENUM respond);
    void onRespondMapDisplaySavedPOIIcon(int nReqID, NAVI_ONOFF_STATUS status, NAVI_SETTINGS_RES_ENUM respond);
    void onRespondMapPOIDisplayOnMapList(int nReqID, NAVI_POI_LIST& list, NAVI_SETTINGS_RES_ENUM respond);
    void onRespondMapHighwayMode(int nReqID, NAVI_ONOFF_STATUS status, NAVI_SETTINGS_RES_ENUM respond);
    void onRespondMapSkybox(int nReqID, NAVI_ONOFF_STATUS status, NAVI_SETTINGS_RES_ENUM respond);

    void onGetRGSettings(int nReqID, NAVI_RG_SETTINGS& settings, const NAVI_RG_DISABLED_SETTINGS& disabledsettings);
    void onRespondRGDynamicRouting(int nReqID, NAVI_ONOFF_STATUS status, NAVI_SETTINGS_RES_ENUM respond);
    void onRespondRGRouteAutoRecalculate(int nReqID, NAVI_ONOFF_STATUS status, NAVI_SETTINGS_RES_ENUM respond);
    void onRespondRGRoutingMode(int nReqID, ROUTE_ROUTING_MODE mode, NAVI_SETTINGS_RES_ENUM respond);
    void onRespondRGRoutePreferenceList(int nReqID, NAVI_ROUTE_PREFERENCE_LIST& list, NAVI_SETTINGS_RES_ENUM respond);
    void onRespondRGCommuteLearnRoute(int nReqID, NAVI_ONOFF_STATUS status, NAVI_SETTINGS_RES_ENUM respond);
    void onRespondRGCommuteAutoStart(int nReqID, NAVI_ONOFF_STATUS status, NAVI_SETTINGS_RES_ENUM respond);
    void onRespondRGCommuteFewerCommand(int nReqID, NAVI_ONOFF_STATUS status, NAVI_SETTINGS_RES_ENUM respond);
    void onRespondRGDeleteDrivingHistory(int nReqID, NAVI_REQUEST_RESULT result, NAVI_SETTINGS_RES_ENUM respond);
    void onRespondRGSuggestionAlongList(int nReqID, NAVI_ROUTE_SUGGESTION_LIST& list, NAVI_SETTINGS_RES_ENUM respond);
    void onRespondRGRoadSignBoard(int nReqID, NAVI_ONOFF_STATUS status, NAVI_SETTINGS_RES_ENUM respond);
    void onRespondRGRailroadCrossing(int nReqID, NAVI_ONOFF_STATUS status, NAVI_SETTINGS_RES_ENUM respond);
    void onRespondRGIntersectionView(int nReqID, NAVI_ONOFF_STATUS status, NAVI_SETTINGS_RES_ENUM respond);
    void onRespondRGAvoidPlateNumRestrict(int nReqID, NAVI_ONOFF_STATUS status, NAVI_SETTINGS_RES_ENUM respond);
    void onRespondRGArrivalTimeMode(int nReqID, GUIDE_ARRIVAL_TIME mode, NAVI_SETTINGS_RES_ENUM respond);
    void onRespondRGArrivalTimeTo(int nReqID, GUIDE_ESTIMATED_TIME to, NAVI_SETTINGS_RES_ENUM respond);

    void onGetVoiceSettings(int nReqID, NAVI_VOICE_SETTINGS& settings, const NAVI_VOICE_DISABLED_SETTINGS& disabledsettings);
    void onRespondGeneralAnnouncementDistance(int nReqID, NAVI_GENERAL_ANNOUNCEMENT_DISTANCE distance, NAVI_SETTINGS_RES_ENUM respond);

    void onGetAboutSettings(int nReqID, NAVI_ABOUT_SETTINGS& settings, const NAVI_ABOUT_DISABLED_SETTINGS& disabledsettings);
    void onRespondAboutGPSInfo(int nReqID, NAVI_ABOUT_GPS_INFO& info, NAVI_SETTINGS_RES_ENUM respond);
    void onGetAccountSettings(int nReqID, NAVI_ACCOUNT_SETTINGS& settings, const NAVI_ACCOUNT_DISABLED_SETTINGS& disabledsettings);
    void onRespondAccountClearUserPreferences(int nReqID, NAVI_REQUEST_RESULT result, NAVI_SETTINGS_RES_ENUM respond);

    void onResponseSetMapCarIcon(int icon);
    void onResponseNaviVersionInfo(char* info);
    void onResponseMapUpdateLatestStatus(bool status);
    void onResponseMapUpdate(NAVI_REQUEST_RESULT result);
    void onResponseMobileTmapSyncStatus(bool status);
    void onResponseSyncMobileTmap(NAVI_REQUEST_RESULT result);
    void onResponseCancelMobileTmapSync(NAVI_REQUEST_RESULT result);
    void onResponseNaviFactoryReset(NAVI_REQUEST_RESULT result);

    void onResponseSetRouteAlternative(NAVI_ROUTE_ALTERNATIVE_LIST* list);
    void onResponseSetRouteOutOfRouteRecalculation(NAVI_ONOFF_STATUS status);
    void onResponseSetWarningSoundWhenNaviVolumeZero(NAVI_ONOFF_STATUS status);

    //Home Tile APIs
    virtual void setDestinationToHome(const int32_t& nReqID) override;
    virtual void getCommuteSuggestions(const int32_t& nReqID) override;
    virtual void setGoToNaviSearch(const int32_t& nReqID) override;
    virtual void setCancelRoute(const int32_t& nReqID) override;
    virtual void setNaviMuteStatus(const int32_t& nReqID, const bool& bMuteStatus) override;
    virtual void setGoToSetFavorites(const int32_t& nReqID) override;
    virtual void setCommuteDestination(const int32_t& nReqID, const std::string& listItemID) override;

    void onResponseSetDestinationToHome(int nReqID, NAVI_REQUEST_RESULT result);
    void onResponseGetCommuteSuggestions(int nReqID, NAVI_REQUEST_RESULT result, std::vector<NAVI_COMMUTE_INFO> commuteList);
    void onResponseSetGoToNaviSearch(int nReqID, NAVI_REQUEST_RESULT result);
    void onResponseSetCancelRoute(int nReqID, NAVI_REQUEST_RESULT result);
    void onResponseSetNaviMuteStatus(int nReqID, NAVI_REQUEST_RESULT result, bool bMuteStatus);
    void onResponseSetGoToSetFavorites(int nReqID, NAVI_REQUEST_RESULT result);
    void onResponseSetCommuteDestination(int nReqID, NAVI_REQUEST_RESULT result);

    // Common APIs
    virtual void getNaviGuideStatus(const int32_t &nReqID) override;
    virtual void getNaviMuteStatus(const int32_t &nReqID) override;
    virtual void getNaviServiceAvailable(const int32_t &nReqID) override;
    virtual void getNaviCommuteStatus(const int32_t& nReqID) override;
    virtual void getNaviFavoritesStatus(const int32_t& nReqID) override;

    void onResponseGetNaviGuideStatus(int nReqID, NAVI_REQUEST_RESULT result, bool bGuidanceRunning);
    void onResponseGetNaviMuteStatus(int nReqID, NAVI_REQUEST_RESULT result, bool bMuteStatus);
    void onResponseGetNaviServiceAvailable(int nReqID, NAVI_REQUEST_RESULT result, bool bNaviAvailable);
    void onResponseGetNaviCommuteStatus(int nReqID, NAVI_REQUEST_RESULT result, NAVI_COMMUTE_STATUS status);
    void onResponseGetNaviFavoritesStatus(int nReqID, NAVI_REQUEST_RESULT result, bool bHomeSetStatus, bool bWorkSetStatus);

    void onNotifyNaviTimezoneInfo(int utcOffset, bool bDst, std::string timezoneName);
    void onResponseGetNaviTimezoneInfo(int nReqID, NAVI_REQUEST_RESULT result, int utcOffset, bool bDst, std::string timezoneName);
    void onResponseGetNaviSharedUrl(int nReqID, NAVI_REQUEST_RESULT result, NAVI_POSITION_ROUTE_TYPE type, std::string url);
    void onResponseUnsubscribeNavStatus(int nReqID, NAVI_REQUEST_RESULT result);
    void onResponseSubscribeNavStatus(int nReqID, NAVI_REQUEST_RESULT result);
    void onResponseGetRouteInfo(int nReqID, NAVI_REQUEST_RESULT result, std::vector<NAVI_POINT_ALONG_ROUTE> routeInfo);
    void onResponseSubscribeRouteRecalculation(int nReqID, NAVI_REQUEST_RESULT result);
    void onResponseUnsubscribeRouteRecalculation(int nReqID, NAVI_REQUEST_RESULT result);
    //void onResponseGetAvailableCommutes(int nReqID, NAVI_REQUEST_RESULT result, std::vector<NAVI_COMMUTE_DESTINATION_INFO> commuteDesinationtList);
    void onResponseUpdateRouteWaypoints(int nReqID, NAVI_REQUEST_RESULT result, std::vector<NAVI_COMMUTE_DESTINATION_INFO> routeInfo);
    //void onResponseGetCommuteRoute(int nReqID, NAVI_REQUEST_RESULT result, std::vector<NAVI_POINT_ALONG_ROUTE> commutePoints);
    //void onResponseShowIconOnMap(int nReqID, NAVI_REQUEST_RESULT result);
    //void onResponseSortWaypoint(int nReqID, NAVI_REQUEST_RESULT result);
    void onResponseGetNaviSupportFunctionList(int nReqID, NAVI_REQUEST_RESULT result, NAVI_SUPPORTED_FEATURE* supportedFeature);

    void onNotifyNaviGuideStatusChanged(bool bGuidanceRunning);
    void onNotifyNaviTBTInfoChanged(NAVI_TBT_TYPE turnIcon, std::string roadname, int distance, int roundaboutExitNum, int countDownBar);
    void onNotifyNextDestinationInfoChanged(NAVI_ETA_TTA_DISTANCE etaDistance, std::string title);
    void onNotifyRouteProgressBarInfoChanged(std::vector<NAVI_HOMETILE_PROGRESS_SECTION> progressSectionList);
    void onNotifyNaviMuteStatusChanged(bool bNaviMute);
    void onNotifyNaviServiceAvailableChanged(bool bNaviAvailable);
    void onNotifyNaviCommuteStatus(NAVI_COMMUTE_STATUS commuteStatus, bool bCommuteRunning);
    void onNotifyNaviCommuteInfoChanged(std::vector<NAVI_HOMETILE_POPOVER_ITEM> commuteList);
    void onNotifyNaviFavoritesStatusChnaged(bool bHomeSetStatus, bool bWorkSetStatus);
    void onNotifyDistanceETAChanged(std::vector<NAVI_WAYPOINT_INFO> waypointList);
    void onNotifyNaviRouteChanged(int nReqID, NAVI_REQUEST_RESULT result, std::vector<NAVI_WAYPOINT_INFO> waypointList);
    void onNotifyNaviMapAdasDataNoUpdate();
    void onNotifyNaviMapAdasDataUpdate();
    void onNotifyNaviMapAdasDataFinishInstallData();

    void onNotifyNaviCurrentRoadInfoChanged(std::string currentRoad, std::string currentTown) override;
    void onNotifyNaviCameraInfoChanged(std::string cameraTitle, int distance, bool bVisible) override;
    void onNotifyPopOverItemInfoChanged(NAVI_HOMETILE_POPOVER_TYPE popOverType, std::vector<NAVI_HOMETILE_POPOVER_ITEM> popOverItemList) override;

    //Connected service(Webapp) APIs, Navi exposed API
    virtual void addToFavourites(const int32_t& nReqID, const std::string& title, const NAVI_LOCATION_ADDRESS_COORDINATE_T& locationData, const E_NAVI_FAVORITES_TYPE& favoritesType) override;
    virtual void addWaypoint(const int32_t& nReqID, const std::string& referenceWaypointID, const NAVI_LOCATION_ADDRESS_COORDINATE_T& locationData) override;
    virtual void getAddressFromGeo(const int32_t& nReqID, const NAVI_GEO_COORDINATE_T& geoCoordinates) override;
    virtual void getCurrentLocation(const int32_t& nReqID) override;
    virtual void getDestination(const int32_t& nReqID) override;
    //virtual void getFavouriteDestinationList(const int32_t& nReqID) override;
    virtual void getGeoFromAddress(const int32_t& nReqID, const NAVI_ADDRESS_T& adress) override;
    virtual void getSavedLocations(const int32_t& nReqID) override;
    virtual void getItinerary(const int32_t& nReqID) override;
    virtual void getNaviSearchFeatures(const int32_t& nReqID) override;
    virtual void getNaviSupportedFeatures(const int32_t& nReqID) override;
    virtual void getPOISAroundLocation(const int32_t& nReqID, const std::vector<std::string>& searchCategoryIDs, const NAVI_GEO_COORDINATE_T& locationData, const std::string& keyword) override;
    virtual void getRecentDestinationList(const int32_t& nReqID) override;
    virtual void getWaypointData(const int32_t& nReqID, const std::string& waypointIndex) override;
    virtual void setDestinationList(const int32_t& nReqID, const std::vector<NAVI_LOCATION_ADDRESS_COORDINATE_T>& destinationList, const bool& skipConfirmation) override;
    virtual void showLocationOnMap(const int32_t& nReqID, const std::vector<NAVI_GEO_COORDINATE_T>& locationList) override;
    virtual void subscribeDistanceETAChanged(const int32_t& nReqID) override;
    virtual void subscribeRouteChanged(const int32_t& nReqID) override;
    virtual void unSubscribeDistanceETAChanged(const int32_t& nReqID) override;
    virtual void unSubscribeRouteChanged(const int32_t& nReqID) override;
    virtual void getMapSurface(const int32_t& nReqID, const NAVI_GET_MAP_SURFACE_T& options, const std::string& folderPath) override;
    virtual void calculateRouteInfo(const int32_t& nReqID, const NAVI_LOCATION_ADDRESS_COORDINATE_T& locationData) override;
    virtual void addIconsOnMap(const int32_t& nReqID, const std::vector<NAVI_ICON_INFO_T>& icons) override;
    virtual void removeIconsOnMap(const int32_t& nReqID, const std::vector<std::string>& icons) override;

    void onResponseAddToFavourites(int nReqID, NAVI_REQUEST_RESULT result);
    void onResponseAddWaypoint(int nReqID, NAVI_REQUEST_RESULT result);
    void onResponseGetAddressFromGeo(int nReqID, NAVI_REQUEST_RESULT result, NAVI_ADDRESS* address);
    void onReponseGetCurrentLocation(int nReqID, NAVI_REQUEST_RESULT result, NAVI_LOCATION_ADDRESS_COORDINATE* locationData);
    void onResponseGetDestination(int nReqID, NAVI_REQUEST_RESULT result, NAVI_WAYPOINT_INFO* destination);
    //void onResponstGetFavouriteDestinationList(int nReqID, NAVI_REQUEST_RESULT result, NAVI_LOCATION_ADDRESS_COORDINATE_LIST* destinationList);
    void onResponstGetGeoFromAddress(int nReqID, NAVI_REQUEST_RESULT result, std::vector<NAVI_LOCATION_ADDRESS_COORDINATE> geoAddressCoordinates);
    void onResponseGetSavedLocations(int nReqID, NAVI_REQUEST_RESULT result, std::vector<NAVI_SAVED_LOCATION_INFO> savedLocatonList);
    void onResponseGetItinerary(int nReqID, NAVI_REQUEST_RESULT result, NAVI_WAYPOINT_INFO_LIST* waypointList);
    void onReponsegetNaviSearchFeatures(int nReqID, NAVI_REQUEST_RESULT result, std::vector<NAVI_SEARCH_CATEGORIES> categoriesNameAndID);
    void onResponseGetNaviSupportedFeatures(int nReqID, NAVI_REQUEST_RESULT result, std::vector<std::string> supportedAPIsJson);
    void onResponseGetPOISAroundLocation(int nReqID, NAVI_REQUEST_RESULT result, std::vector<NAVI_LOCATION_ADDRESS_COORDINATE> searchResults);
    void onResponseGetRecentDestinationList(int nReqID, NAVI_REQUEST_RESULT result, std::vector<NAVI_WAYPOINT_INFO> recentDestinations);
    void onResponseGetWaypointData(int nReqID, NAVI_REQUEST_RESULT result, NAVI_WAYPOINT_INFO* waypoint);
    void onResponseSetDestinationList(int nReqID, NAVI_REQUEST_RESULT result);
    void onResponseShowLocationOnMap(int nReqID, NAVI_REQUEST_RESULT result);
    void onResponseSubscribeDistanceETAChanged(int nReqID, NAVI_REQUEST_RESULT result);
    void onResponseSubscribeRouteChanged(int nReqID, NAVI_REQUEST_RESULT result);
    void onResponseUnSubscribeDistanceETAChanged(int nReqID, NAVI_REQUEST_RESULT result);
    void onResponseUnSubscribeRouteChanged(int nReqID, NAVI_REQUEST_RESULT result);
    void onResponseGetMapSurface(int nReqID, NAVI_REQUEST_RESULT result, std::string filename);
    void onResponseCalculateRouteInfo(int nReqID, NAVI_REQUEST_RESULT result, NAVI_WAYPOINT_INFO* routeInfo);
    void onResponseAddIconOnMap(int nReqID, NAVI_REQUEST_RESULT result);
    void onResponseRemoveIconsOnMap(int nReqID, NAVI_REQUEST_RESULT result);

    //EB
    virtual void onrequestSetNaviRouteToDestination(const int32_t& nReqID, const E_NAVI_REQUEST_RESULT& result) override;
    virtual void onrequestNaviGuideStatusChanged(const int32_t& nReqID, const E_NAVI_REQUEST_RESULT& result) override;
    virtual void onrequestSetDatabaseUsable(const int32_t& nReqID, const E_NAVI_REQUEST_RESULT& result) override;

    void requestNaviGuideStatusChanged(bool bGuidanceRunning) override;
    void requestSetNaviRouteToDestination() override;
    void requestSetDatabaseUsable(bool bBool) override;

    // Partition
    void requestPartitionOptionChange(char* path, MountOption option) override;
    void connectedPartitionOptionChange() override;

    //VR APIs
    virtual void getAddressInfoRequest(const int32_t nReqID) override;
    virtual void getAlphabetRequest(const int32_t nReqID) override;
    virtual void getSelectedCharacterRequest(const int32_t nReqID) override;
    virtual void getCountryInfoRequest(const int32_t nReqID) override;
    virtual void getDestinationExistsCheckRequest(const int32_t nReqID) override;
    virtual void getSpeechAddressUIDListRequest(const int32_t nReqID, const EAddrType aAddrType) override;
    virtual void setSpeechContextAsDestinationRequest(const int32_t nReqID, const EAddrType aAddrType) override;
    virtual void isSpeechAddrUniqueRequest(const int32_t nReqID, const EAddrType& aContext) override;
    virtual void getDBIDFileRequest(const int32_t nReqID, const std::vector<std::string> aStrDBIDList, const std::string aStrLang, const bool aWithPhonetics) override;
    virtual void getEcoFileRequest(const int32_t nReqID, const SEcoFile& aFileInfo) override;
    virtual void getEcoFileInfoRequest(const int32_t nReqID, const SEcoFileInfo& aFileInfo) override;
    virtual void getEcoFileStatusRequest(const int32_t nReqID) override;
    virtual void waitForEcoFileRequest(const int32_t nReqID) override;
    virtual void getGuidanceStatusRequest(const int32_t nReqID) override;
    virtual void routeGuidanceStopRequest(const int32_t nReqID) override;
    virtual void getAvailAddressDataRequest(const int32_t nReqID, const SGetAvailAddressData& aLocGetData) override;
    virtual void setAvailAddressDataRequest(const int32_t nReqID, const std::vector<SAddressData>& aAddrData) override;
    virtual void getNextPossibleAddressDataRequest(const int32_t nReqID, const std::vector<SAddressData>& aAddrData) override;
    virtual void getSelectedHouseNoRequest(const int32_t nReqID) override;
    virtual void setSelectedPhonenumberRequest(const int32_t nReqID, const std::string aStrPhonenumer) override;
    virtual void setPoiSearchAreaRequest(const int32_t nReqID, const ESearchArea& aSearchArea) override;
    virtual void getPOICategorySelectedRequest(const int32_t nReqID) override;
    virtual void setPoiRequest(const int32_t nReqID, const std::string& aStrPoiValue) override;
    virtual void setPrevDestContextRequest(const int32_t nReqID, const int32_t aContextId) override;
    virtual void getCheckRouteExistsRequest(const int32_t nReqID) override;
    virtual void getNaviStatusRequest(const int32_t nReqID) override;
    virtual void getStateInfoRequest(const int32_t nReqID) override;
    virtual void setSpeechContextAsWaypointRequest(const int32_t nReqID, const EAddrType aAddrType) override;
    virtual void naviWaypointsFullRequest(const int32_t nReqID) override;
    //VR Service
    void onResponseGetGuidanceStatus(int nReqID, NAVI_REQUEST_RESULT result, bool bGuidance);
    void onResponseRouteGuidanceStop(int nReqID, NAVI_REQUEST_RESULT result);
    void onGetDestinationExistsCheckResponse(int nReqID, NAVI_REQUEST_RESULT result, bool aDestinationExists);
    void onNaviWaypointsFullResponse(int nReqID, bool aNaviWaypointFull);
    void onGetCheckRouteExistsResponse(int nReqID, NAVI_REQUEST_RESULT result, bool aRouteExists);
    void speechDialogSelectNotification(const NAVI_ESpeechSelectId selectId);
    void speechPicklistSelectNotification(const int selectId);
    void onSetHomeAsDestination(int nReqID, NAVI_REQUEST_RESULT result);
    void onSetPrevDestAsDestination(int nReqID, NAVI_REQUEST_RESULT result);
    void onSetPoiAsDestination(int nReqID, NAVI_REQUEST_RESULT result);
    void onSetHomeAsWaypoint(int nReqID, NAVI_REQUEST_RESULT result);
    void onSetPrevDestAsWaypoint(int nReqID, NAVI_REQUEST_RESULT result);
    void onSetPoiAsWaypoint(int nReqID, NAVI_REQUEST_RESULT result);
    void onGetSelectedCharacter(int nReqID, NAVI_REQUEST_RESULT result, std::string aSelectedChar);
    void onGetSelectedHouseNo(int nReqID, NAVI_REQUEST_RESULT result, std::string aSelectedNo);
    void onSetSelectedPhonenumber(int nReqID, NAVI_REQUEST_RESULT result, bool bNavigable);
    void onSetPoiSearchArea(int nReqID, NAVI_REQUEST_RESULT result);
    void onResponseShowSpeechDialog(const int32_t nReqID, const NAVI_ESpeechDialogStatus aCallStatus);
    void onResponseHideSpeechDialog(const int32_t nReqID, const NAVI_ESpeechDialogStatus aCallStatus);
    void onResponseShowSpeechPicklist(const int32_t nReqID, const NAVI_ESpeechDialogStatus aCallStatus);
    void onGetSpeechAddressUIDList(int nReqID, NAVI_REQUEST_RESULT result, uint32_t aNoOfEntries, std::string aFileName);
    void onPicklistScrollResponse(const int32_t nReqID, const NAVI_ESpeechDialogStatus& aCallStatus);
    void onSelectedPicklistItemResponse(const int32_t nReqID, const NAVI_ESpeechDialogStatus& aCallStatus, const uint32_t aSelItem);
    void onGetSelectedPhonenumberResponse(const int32_t nReqID, const NAVI_ESpeechDialogStatus& aCallStatus, const std::string& aPhonenumber);
    void onGetDBIDFileResponse(int nReqID, NAVI_REQUEST_RESULT result, uint aNoOfEntries, std::string aFileName);
    void onSetPoiResponse(int nReqID, NAVI_REQUEST_RESULT result, uint aNoOfEntries, std::string aFileName);

    virtual void getNaviTimezoneInfo(const int32_t nReqID) override;
    virtual void setNaviTraceTrackingMode(const int32_t nReqID) override;
    virtual void getNaviSharedUrl(const int32_t nReqID, const E_NAVI_POSITION_ROUTE_TYPE& type) override;
    virtual void unsubscribeNavStatus(const int32_t& nReqID) override;
    virtual void subscribeNavStatus(const int32_t& nReqID) override;
    virtual void getRouteInfo(const int32_t& nReqID, const int& maxPoints, const int& waypointFrequency, const bool& frequencyType, const int& startPoint, const int& endPoint) override;
    virtual void subscribeRouteRecalculation(const int32_t& nReqID) override;
    virtual void unsubscribeRouteRecalculation(const int32_t& nReqID) override;
    //virtual void getAvailableCommutes(const int32_t& nReqID) override;
    virtual void updatePointsAlongRoute(const int32_t& nReqID, const std::vector<std::string>& pointAlongRouteID) override;
    //virtual void getCommuteRoute(const int32_t& nReqID, const int& maxPoints, const int& waypointFrequency, const bool& frequencyType, const int& startPoint, const int& endPoint) override;
    //virtual void sortWaypoint(const int32_t& nReqID) override;
    //virtual void getPOISAlongTheRoute(const int32_t& nReqID, const std::vector<std::string>& searchCategoryIDs, const std::string& keyword);
    virtual void getNaviSupportFunctionList(const int32_t& nReqID) override;

    // CAN Service
    virtual bool publishRouteStatusIGM(const CAN_ROUTE_STATUS_IGM& status) override;

    // Home-tile
    virtual void getTrafficInfoList(const int32_t& nReqID) override;
    virtual void getFuelStationList(const int32_t& nReqID) override;
    virtual void getRestAreaList(const int32_t& nReqID) override;
    virtual void showPOIonMap(const int32_t& nReqID, const NAVI_POI_INFO_T& poiInfo) override;

    void onResponseGetTrafficInfoList(int nReqID, NAVI_REQUEST_RESULT result, std::vector<NAVI_TRAFFIC_INFO> srcList);
    void onResponseGetFuelStationList(int nReqID, NAVI_REQUEST_RESULT result, std::vector<NAVI_POI_INFO> srcList);
    void onResponseGetRestAreaList(int nReqID, NAVI_REQUEST_RESULT result, std::vector<NAVI_POI_INFO> srcList);
    void onResponseShowPOIonMap(int nReqID, NAVI_REQUEST_RESULT result);

#endif

    bool onResponseCmdGoBack(int requestID, REQ_GO_BACK_RESULT eResult);
    bool requestShowSystemComponent(int component) override;
    bool requestHideSystemComponent(int component) override;
    bool requestGoNavi(SHOW_OPT eOpt) override;
    bool requestGoShared() override;

    bool onResponseReadyToSuspendSystemResult(int requestID, REQ_RESULT_MSG eResult) override;
    bool onResponseSuspendSystemResult(int requestID, REQ_RESULT_MSG eResult) override;
    bool onResponseReadyToResumeSystemResult(int requestID, REQ_RESULT_MSG eResult) override;
    bool onResponseResumeSystemResult(int requestID, REQ_RESULT_MSG eResult) override;
    bool requestWaitingReadyToSuspend() override;

    bool getConnectivityStatusSync(bool &bConnected) override;

    bool InsertKeyboardSuggestionData(uint32_t userInputID, char* userInputText, KEYBOARD_CANDIDATE_DATA_TYPE iconType, QVariantList &data, int nTotalCount) override;
    bool InsertKeyboardSuggestionListData(char* userInputText, uint32_t startIndex, uint32_t endIndex, bool isRecent, QStringList &data) override;
    bool requestShowKeyboard(KEYBOARD_TYPE eKeyboardType, char* preFilledText, char* defaultText, bool isCapitalized, uint32_t maxLength) override;
    bool requestShowKeyboard2(KEYBOARD_TYPE eKeyboardType, KEYBOARD_INIT_TYPE eInitType, char* defaultText, char* suggestionListCtx, unsigned int count, bool isCapitalized, unsigned int maxLength) override;
    bool requestHideKeyboard() override;

    bool requestShowPopup(POPUP_TYPE ePopupType, POPUP_SIDE_TYPE ePopupSideType, POPUP_SIZE ePopupSize, NAVI_POPUP_INFO *pPopupInfo, uint32_t priority) override;
    bool requestHidePopup(uint32_t handle) override;
    bool requestShowSpecialPopup(SPECIAL_POPUP_TYPE eSpecialPopupType, NAVI_POPUP_INFO *pPopupInfo, uint32_t priority);

    bool requestSetTurnByTurnInformation(char* roadName, JUNCTION_TYPE junction, int distance, int exitNumber) override;
    bool requestSetDistance(int distance) override;
    void requestSetNavigationLayout(NAVIGATION_TYPE naviType, NAVI_VIDEO_INFORMATION layout) override;
    void requestGetClusterType() override;
    void requestGetHudActivation() override;
    void requestGetClusterVideoType() override;
    void requestSetHUDLayout(const HUD_VIEW_MODE &viewMode, const NAVI_VIDEO_INFORMATION& videoLayout) override;
//    void requestSetHUDLayout(const bool &enable, const NAVI_VIDEO_INFORMATION& videoLayout) override;

    void notifyInitCcf();
    bool requestResource(int channel);
    bool withdrawResource(int channel, int sessionId);

    bool requestGetAuthToken(std::string& reqURL, std::string& resToken, int& resHttpCode) override;

    bool requestCreateSessionHandle() override;
    bool requestFlushContext(int handle) override;
    bool requestSetWindowPosition(int handle, char* windowName, int posX, int posY) override;
    bool requestCreateWindowGroup(char* winGroupName, int width, int height) override;
    bool requestRegisterGroup(int handle, char* windowName, char* winGroupName) override;
    bool requestSetWindowSize(int handle, char* windowName, int width, int height) override;
    bool requestSetWindowVisible(int handle, char* windowName, int visible) override;
    std::list<std::string> getCreatedWindowList() override;

    // Notification Service
    bool requestAddNotification(NOTIFY_TYPE eNotifyType, char* notifyContext);
    bool requestRemoveNotification(unsigned int handle);
    bool requestUpdateNotification(unsigned int handle, char* notifyContext);
    bool requestDecreaseNumberOfGroupNotification(unsigned int handle, unsigned int number);

#ifdef PLATFORM_QNX
    ResService* getResource();
#endif

    bool bIsLPM;
    bool isLPMMode() override;

    void setLocationCallback(qnx_location_callback callback) override;
    qnx_location_callback m_locationcb;

    void changeMapmathingFeedback(bool enabled) override;
#ifdef PLATFORM_QNX
    void updateMmfb(LOCSRV_MMFB &mmfb) override;
#endif
    void requestGetPhoneAvailable() override;
    void requestGetUSBDeviceList() override;

    void setRestAreasAvailability(NAVI_REST_AREA_AVAIL value) override;
    void setRestArRouteGuideStatus(NAVI_REST_AREA_ROUTE value) override;
    void setRestAreaResponse(NAVI_REST_AREA_RESPONSE value) override;

    bool setRouteInfo(NAVI_ROUTE_INFO_T routeInfo) override;

signals:
    void ResponseRequestResourceAudio(int result, int sessionId, int sessionState, const char *pDevice);
    void ResponseWithdrawResourceAudio(int result, int sessionId);
    void ResponseRequestResourceAlarm(int result, int sessionId, int sessionState, const char *pDevice);
    void ResponseWithdrawResourceAlarm(int result, int sessionId);


private slots:
    void onEngineInitialized(bool success);
    void onApplicationWindowCreated();


    void onRegisterApplicationResult(REQ_RESULT_MSG eResult);
    void onForegroundStateChanged(bool foreground);
    void onCmdGoBack(int requestID);
    void onShowSystemComponentResult(int requestID, REQ_RESULT_MSG result);
    void onHideSystemComponentResult(int requestID, REQ_RESULT_MSG result);
    void onCmdShowApplication(int requestID, char* strAppRole, char* strIntent);
    void onCmdHideApplication(int requestID, char* strAppRole, char* strIntent);
    void onCmdResumeApplication(int requestID, char* strAppRole, char* strIntent);
    void onPhoneAvailableChanged(bool available);
    void onCmdReadyToSuspendSystem(int requestID);
    void onCmdReadyToResumeSystem(int requestID);
    void onWaitingReadyToSuspend(WAITING_SUSPEND_RES eResult);
    void onResponseShowKeyboard(KEYBOARD_RESPONSE Response);
    void onResponseHideKeyboard(KEYBOARD_RESPONSE Response);
    void onNotifyKeyboardResult(KEYBOARD_RESULT eResult, char* userInputText);
    void onRequestCandidateInfo(unsigned int inputID, char *userInputText);
    void onRequestCandidateList(char* userInputText, unsigned int startIndex, unsigned int endIndex);
    void onNotifyKeyboardError(KEYBOARD_ERROR eError);
    void onResponseShowPopup(unsigned int handle, POPUP_RESPONSE eResponse);
    void onResponseHidePopup(unsigned int handle, POPUP_RESPONSE eResponse);
    void onNotifyPopupResult(unsigned int handle, POPUP_RESULT eResult, bool isChecked);
    void onResponseUpdatePopup(unsigned int handle, POPUP_RESPONSE eResponse);
    void onNotifyStatusChanged(unsigned int handle, POPUP_DISPLAY_STATUS ePopupDisplayStatus);
    void onButtonEvent(SWITCH eSwitchType, BUTTON eButton, ACTION eAction);
    void onRotaryEvent(ROTARY eRotary, ROTATION eRotation, int step);
    void onSystemPopupVisibilityChanged(bool isVisibleSystemPopup);
    void onCmdTerminateApplication();

    void onResponseRequestResourceAudio(int result, int sessionId, int sessionState, const char *pDevice);
    void onResponseWithdrawResourceAudio(int result, int sessionId);
    void onResponseRequestResourceAlarm(int result, int sessionId, int sessionState, const char *pDevice);
    void onResponseWithdrawResourceAlarm(int result, int sessionId);

    void onSystemLanguageUpdate(const char *langcode);
    void onSystemUIThemeUpdate(UI_THEME theme);
    void onNaviMapUIThemeUpdate(UI_THEME theme);

    void onResponseSetTurnByTurnInformation(REQUEST_RESULT result);
    void onResponseSetDistance(REQUEST_RESULT result);
    void onNotifyViewLayoutChangeFromCluster(CLUSTER_VIEW_MODE mode);
    void onResponseSetNavigationLayout(REQUEST_RESULT result);
    void onResponseGetClusterType(CLUSTER_TYPE type);
    void onResponseGetHUDActivation(HUD_ACTIVATION activation);
    void onResponseGetClusterVideoType(CLUSTER_HUD_INFORMATION videoTypeInformation);
    void onResponseSetHUDLayout(REQUEST_RESULT result);
    void onNotifyHudViewModeChangeFromCluster(HUD_VIEW_MODE mode);

    void onResponseGetUserLoginStatus(int profileId, bool loginStatus, const char* tokenInfo);
    void onBrProfileExpired(const char* aliasName);
    void onBrUpdateLoginStatus(bool loginStatus, int profileId, const char* aliasName, const char* tokenInfo);

    void onResponseGetFuelType(CCF_FUEL_TYPE type);
    void onResponseGetVIN(char* vin);

    void onDCMFatigueStateToNavi(NAVI_DCM_FATIGUE state);
    void onRequestToFindRestAreas(NAVI_REQ_TO_FIND_REST_AREAS area);
    void onReqToDisplayRestAreas(NAVI_REQ_TO_DISPLAY_REST_AREAS area);

    void onDistanceToEmpty(int dist);
    void onLowFuelLevel(bool lowfuel);
    void onFuelGaugePosition(int fuelLevel);
    void onHVBatteryUsableSOCDisp(int usableState);
    void onHVBattLevelColourDisp(int colourSet);

    void onTJPStatusChange(NAVI_TJP_STATUS_T tjpStatusInfo);

#ifdef USBSTATUS_QNXAPI
    void onUsbStatusNoti(USB_STATUS eUsbStatus, char* szPath);
#endif

    void onDeviceListChanged(std::list<DeviceInfo> list);
    void onDeviceChanged(std::string MountPath, EventType type, MountStatus status);

    void onResponsePartitionOptionChange(MountResult result);

    void onResponseConnectivityStatus(bool bConnected);

    void onVehicleTokenRefreshed(char* token, char* vin);

    void onCreateSessionHandle(int handle);
    void onFlushContext(WINDOW_RESULT result);
    void onSetWindowPosition(WINDOW_RESULT result);
    void onCreateWindowGroup(WINDOW_RESULT result);
    void onRegisterGroup(WINDOW_RESULT result);
    void onSetWindowSize(WINDOW_RESULT result);
    void onSetWindowVisible(WINDOW_RESULT result);
    //VR Service
    void onShowSpeechDialogRequest(const int nReqID, const NAVI_SSpeechDialogData& aDialogData);
    void onHideSpeechDialogRequest(const int nReqID);
    void onShowSpeechPicklistRequest(const int nReqID, const NAVI_SSpeechPicklistData aDialogData);
    void onPicklistScrollRequest(const int nReqID, const NAVI_EScrollDir& aScrollDir);
    void onSelectedPicklistItemRequest(const int nReqID, const unsigned int aListItem);
    void onGetSelectedPhonenumberRequest(const int nReqID);

    // Notification Service
    void onResponseAddNotification(unsigned int handle, NOTIFY_RESPONSE eResponse);
    void onResponseRemoveNotification(unsigned int handle, NOTIFY_RESPONSE eResponse);
    void onResponseUpdateNotification(unsigned int handle, NOTIFY_RESPONSE eResponse);
    void onResponseDecreaseNumberOfGroupNotification(unsigned int handle, NOTIFY_RESPONSE eResponse);

    void onResponseClickButton(unsigned int handle, char* btnName);
    void onListNotificationChange(unsigned int numberNotification, bool bHighPriority);
    void onFeedbackRemoveNotificationTimeout(unsigned int handle, NOTIFY_RESPONSE eResponse);
    void onNotifyRemoveNotiCardByUserAction(unsigned int handle, NOTIFY_RESPONSE eResponse);

private:
    TmapController* m_controller;

#ifdef PLATFORM_QNX
    NaviServiceProvider *m_pNaviProvider;
    NaviDeploy *m_pNaviDeploy;

    WrapperApp *m_WrapperApp;
    WrapperLocation *m_WrapperLocation;
    WrapperResource *m_WrapperRes;
    WrapperKeyboard *m_WrapperKeyboard;
    WrapperPopup *m_WrapperPopup;
    WrapperSystemsetting *m_WrapperSystemsetting;
    WrapperCluster *m_WrapperCluster;
    WrapperUserprofile *m_WrapperUserprofile;
    WrapperCan *m_WrapperCan;
    WrapperCcf *m_WrapperCcf;
    WrapperBluetooth *m_WrapperBt;
    WrapperDcm *m_WrapperDcm;
    WrapperTjp *m_WrapperTjp;
    WrapperSystem *m_WrapperSystem;
    WrapperUsbMgr *m_WrapperUsbMgr;
    WrapperPartitionCtrl *m_WrapperPartitionCtrl;
    WrapperConnectivity *m_WrapperConnectivity;
    WrapperAuth *m_WrapperAuth;
    WrapperWindow *m_WrapperWindow;
    WrapperVr *m_WrapperVr;
    WrapperNotification *m_WrapperNotification;

    AppService *m_AppService;
    LocationService *m_LocationService;
    KeyboardClient *m_KeyboardClient;
    PopupService *m_PopupService;
    ResService *m_ResService;
    SystemSetting *m_SystemSetting;
    ClusterService *m_ClusterService;
    UserprofileService *m_UserprofileService;
    CanService *m_CanService;
    CcfService *m_CcfService;
    BtService *m_BtService;
#ifdef USBSTATUS_QNXAPI
    UsbStatus *m_UsbStatus;
#endif
    DcmService *m_DcmService;
    TjpService *m_TjpService;
    SystemService *m_SystemService;
    UsbDetectionManager *m_UsbDetectionManager;
    PartitionCtrl *m_PartitionCtrl;
    ConnectivityService *m_ConnectivityService;
    AuthService *m_AuthService;
    WindowService *m_WindowService;
    VrService *m_VrService;
    NotificationService *m_NotificationService;
#endif
    QPointer<QThread> m_Thread;

    int m_nCurrEcoElemType;

public:
    virtual bool onClientConnected(const char* name, const char* path, const char* ver);
    virtual bool onClientDisconnected(const char* name, const char* path);

    virtual bool onServiceConnected(const char* name, const char* path, const char* ver);
    virtual bool onServiceDisconnected(const char* name, const char* path);

    virtual bool onHandleMessages(const char* cmd, const void* data, const int& size);

private:
    void notifyInitValues(const char* path);

    //operator overload
#ifdef PLATFORM_QNX
    friend NAVI_GEO_COORDINATE_T & operator<<(NAVI_GEO_COORDINATE_T & dest, NAVI_GEO_COORDINATE & src);
    friend NAVI_GEO_COORDINATE & operator<<(NAVI_GEO_COORDINATE & dest, NAVI_GEO_COORDINATE_T & src);
    friend NAVI_ADDRESS & operator<<(NAVI_ADDRESS & dest, const NAVI_ADDRESS_T & src);
    friend NAVI_ADDRESS_T & operator<<(NAVI_ADDRESS_T & dest, const NAVI_ADDRESS & src);
    friend NAVI_LOCATION_ADDRESS_COORDINATE & operator<<(NAVI_LOCATION_ADDRESS_COORDINATE & dest, const NAVI_LOCATION_ADDRESS_COORDINATE_T & src);
    friend NAVI_LOCATION_ADDRESS_COORDINATE_T & operator<<(NAVI_LOCATION_ADDRESS_COORDINATE_T & dest, const NAVI_LOCATION_ADDRESS_COORDINATE & src);
    friend NAVI_ETA_TTA_DISTANCE_T & operator<<(NAVI_ETA_TTA_DISTANCE_T & dest, const NAVI_ETA_TTA_DISTANCE & src);
    friend QSharedPointer <NAVI_LOCATION_ADDRESS_COORDINATE> operator<<(QSharedPointer <NAVI_LOCATION_ADDRESS_COORDINATE> dest, const NAVI_LOCATION_ADDRESS_COORDINATE_T& src);

#endif
};

}

#endif // PLATFORMCONTROLLER_DUMMY_H
