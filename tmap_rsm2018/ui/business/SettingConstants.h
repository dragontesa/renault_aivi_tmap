#ifndef SETTINGCONSTANTS_H
#define SETTINGCONSTANTS_H

#include "Constants.h"
#include "ZoneConstants.h"

#include "../../../ndds/include/common_def.h"

#include <QString>
#include <QObject>

namespace SKT {

class TMAP_RSM_UI_DLLSHARED_EXPORT SettingConstants : public QObject
{
    Q_OBJECT
    Q_ENUMS( OilTypeK )
    Q_ENUMS( NightModeK )
    Q_ENUMS( AddressDisplayMethodK )
    Q_ENUMS( DistanceViewK )
    Q_ENUMS( CharacterK )
    Q_ENUMS( CarIconK )
    Q_ENUMS( RoadViolationK )
    Q_ENUMS( MapModeK )
    Q_ENUMS( OtherRouteK )
    Q_ENUMS( OilBrandK )
    Q_ENUMS( OilAreaK)
    Q_ENUMS( SearchTypeK )
    Q_ENUMS( VolumeK )
    Q_ENUMS( CarConfReqTypeK )
    Q_ENUMS( CarConfResFuelTypeK )
    Q_ENUMS( ChargeTypeK )


    // string
    //commonConstants와 함께 사용
    Q_PROPERTY(QString Section                                  MEMBER  Section                             CONSTANT)
    Q_PROPERTY(QString Key                                      MEMBER  Key                                 CONSTANT)
    Q_PROPERTY(QString Value                                    MEMBER  Value                               CONSTANT)
    Q_PROPERTY(QString Setting                                  MEMBER  Setting                             CONSTANT)
    Q_PROPERTY(QString Settings                                 MEMBER  Settings                            CONSTANT)
    Q_PROPERTY(QString Type                                     MEMBER  Type                                CONSTANT)
    Q_PROPERTY(QString NoMapData                                MEMBER  NoMapData                           CONSTANT)
    Q_PROPERTY(QString IncompleteMapData                        MEMBER  IncompleteMapData                   CONSTANT)
    Q_PROPERTY(QString IncompatibleMapData                      MEMBER  IncompatibleMapData                 CONSTANT)
    Q_PROPERTY(QString ErrorMapData                             MEMBER  ErrorMapData                        CONSTANT)
    Q_PROPERTY(QString Count                                    MEMBER  Count                               CONSTANT)
    //-
    Q_PROPERTY(QString PreferencesName                          MEMBER  PreferencesName                     CONSTANT)


    Q_PROPERTY(QString NaviVolume                               MEMBER  NaviVolume                          CONSTANT)
    Q_PROPERTY(QString NaviVolumeUp                             MEMBER  NaviVolumeUp                        CONSTANT)
    Q_PROPERTY(QString NaviVolumeDown                           MEMBER  NaviVolumeDown                      CONSTANT)
    Q_PROPERTY(QString NaviMute                                 MEMBER  NaviMute                            CONSTANT)
    Q_PROPERTY(QString StoredSize                               MEMBER  StoredSize                          CONSTANT)
    Q_PROPERTY(QString TotalSize                                MEMBER  TotalSize                           CONSTANT)
    Q_PROPERTY(QString TotalCount                               MEMBER  TotalCount                          CONSTANT)
    Q_PROPERTY(QString IgnorePopupFileName                      MEMBER  IgnorePopupFileName                 CONSTANT)
    Q_PROPERTY(QString InCompleteMapDataKo                       MEMBER  InCompleteMapDataKo                 CONSTANT)



    // section
    Q_PROPERTY(QString Section1                                 MEMBER Section1                             CONSTANT)
    Q_PROPERTY(QString MapMain                                  MEMBER MapMain                              CONSTANT)
    Q_PROPERTY(QString MapSubSpeedResponse                      MEMBER MapSubSpeedResponse                  CONSTANT)
    Q_PROPERTY(QString MapSubMapCharacter                       MEMBER MapSubMapCharacter                   CONSTANT)
    Q_PROPERTY(QString MapSubCarIcon                            MEMBER MapSubCarIcon                        CONSTANT)
    Q_PROPERTY(QString MapSubOilType                            MEMBER MapSubOilType                        CONSTANT)
    Q_PROPERTY(QString MapSubChargeType                         MEMBER MapSubChargeType                     CONSTANT)
    Q_PROPERTY(QString GuidMain                                 MEMBER GuidMain                             CONSTANT)
    Q_PROPERTY(QString GuidSubVoice                             MEMBER GuidSubVoice                         CONSTANT)
    Q_PROPERTY(QString GuidSubCamera                            MEMBER GuidSubCamera                        CONSTANT)
    Q_PROPERTY(QString GuidSubAttentionSection                  MEMBER GuidSubAttentionSection              CONSTANT)
    Q_PROPERTY(QString RouteAlternateSelection                  MEMBER RouteAlternateSelection              CONSTANT)
    Q_PROPERTY(QString GuidSubVoiceGuidTime                     MEMBER GuidSubVoiceGuidTime                 CONSTANT)
    Q_PROPERTY(QString NaviInfo                                 MEMBER NaviInfo                             CONSTANT)
    Q_PROPERTY(QString DirectionGuid                            MEMBER DirectionGuid                        CONSTANT)
    Q_PROPERTY(QString SearchOption                             MEMBER SearchOption                         CONSTANT)    
    //commonConstants와 함께 사용
    Q_PROPERTY(QString RouteSearch                              MEMBER RouteSearch                          CONSTANT)



    // key
    Q_PROPERTY(QString LogEnable                                MEMBER LogEnable                            CONSTANT)
    Q_PROPERTY(QString DisplayOption                            MEMBER DisplayOption                        CONSTANT)
    Q_PROPERTY(QString UserName                                 MEMBER UserName                             CONSTANT)
    Q_PROPERTY(QString RecentDestinations                       MEMBER RecentDestinations                   CONSTANT)
    Q_PROPERTY(QString NightMode                                MEMBER NightMode                            CONSTANT)
    Q_PROPERTY(QString AddressDisplayMethod                     MEMBER AddressDisplayMethod                 CONSTANT)
    Q_PROPERTY(QString AddressDisplay2ndMethod                  MEMBER AddressDisplay2ndMethod              CONSTANT)
    Q_PROPERTY(QString DisplayArroundTrafficInfo                MEMBER DisplayArroundTrafficInfo            CONSTANT)
    Q_PROPERTY(QString AlwaysDisplayDriveLineTrafficInfo        MEMBER AlwaysDisplayDriveLineTrafficInfo    CONSTANT)
    Q_PROPERTY(QString ExpresswayDrivingGuidance                MEMBER ExpresswayDrivingGuidance            CONSTANT)
    Q_PROPERTY(QString GpsSpeed                                 MEMBER GpsSpeed                             CONSTANT)
    Q_PROPERTY(QString SkyeBox                                  MEMBER SkyeBox                              CONSTANT)
    Q_PROPERTY(QString SpeedResponseMapUse                      MEMBER SpeedResponseMapUse                  CONSTANT)
    Q_PROPERTY(QString Distance                                 MEMBER Distance                             CONSTANT)
    Q_PROPERTY(QString Character                                MEMBER Character                            CONSTANT)
    Q_PROPERTY(QString Icon                                     MEMBER Icon                                 CONSTANT)
    Q_PROPERTY(QString RoadViolation                            MEMBER RoadViolation                        CONSTANT)
    Q_PROPERTY(QString SafeDrivingSoundAtMute                   MEMBER SafeDrivingSoundAtMute               CONSTANT)
    Q_PROPERTY(QString BeepSoundGuideAtMute                     MEMBER BeepSoundGuideAtMute                 CONSTANT)
    Q_PROPERTY(QString SpeedViolationSectionSignalCrackdown     MEMBER SpeedViolationSectionSignalCrackdown CONSTANT)
    Q_PROPERTY(QString Male                                     MEMBER Male                                 CONSTANT)
    Q_PROPERTY(QString CrackdownOnSpeedViolation                MEMBER CrackdownOnSpeedViolation            CONSTANT)
    Q_PROPERTY(QString SectionControl                           MEMBER SectionControl                       CONSTANT)
    Q_PROPERTY(QString SignalControl                            MEMBER SignalControl                        CONSTANT)
    Q_PROPERTY(QString DetachableCrackdown                      MEMBER DetachableCrackdown                  CONSTANT)
    Q_PROPERTY(QString CrackdownWithBusExclusiveCar             MEMBER CrackdownWithBusExclusiveCar         CONSTANT)
    Q_PROPERTY(QString ParkingControl                           MEMBER ParkingControl                       CONSTANT)
    Q_PROPERTY(QString InterruptRegulation                      MEMBER InterruptRegulation                  CONSTANT)
    Q_PROPERTY(QString MonitoringOfRoadShoulder                 MEMBER MonitoringOfRoadShoulder             CONSTANT)
    Q_PROPERTY(QString CollectingTrafficInformation             MEMBER CollectingTrafficInformation         CONSTANT)
    Q_PROPERTY(QString ControlOfVariableVehicles                MEMBER ControlOfVariableVehicles            CONSTANT)
    Q_PROPERTY(QString SpeedBump                                MEMBER SpeedBump                            CONSTANT)
    Q_PROPERTY(QString AccidentFrequent                         MEMBER AccidentFrequent                     CONSTANT)
    Q_PROPERTY(QString ChildProtectedArea                       MEMBER ChildProtectedArea                   CONSTANT)
    Q_PROPERTY(QString FogCaution                               MEMBER FogCaution                           CONSTANT)
    Q_PROPERTY(QString AttentionToWildAnimals                   MEMBER AttentionToWildAnimals               CONSTANT)
    Q_PROPERTY(QString RailroadXing                             MEMBER RailroadXing                         CONSTANT)
    Q_PROPERTY(QString SteepSlopeSection                        MEMBER SteepSlopeSection                    CONSTANT)
    Q_PROPERTY(QString SharpCurve                               MEMBER SharpCurve                           CONSTANT)
    Q_PROPERTY(QString OneWay                                   MEMBER OneWay                               CONSTANT)
    Q_PROPERTY(QString FrequentSignalViolation                  MEMBER FrequentSignalViolation              CONSTANT)
    Q_PROPERTY(QString SpeedTravelFrequently                    MEMBER SpeedTravelFrequently                CONSTANT)
    Q_PROPERTY(QString TrafficJamArea                           MEMBER TrafficJamArea                       CONSTANT)
    Q_PROPERTY(QString ChangeSectionOfCar                       MEMBER ChangeSectionOfCar                   CONSTANT)
    Q_PROPERTY(QString ChargingStation                          MEMBER ChargingStation                      CONSTANT)
    Q_PROPERTY(QString Tunnel                                   MEMBER Tunnel                               CONSTANT)
    Q_PROPERTY(QString RestArea                                 MEMBER RestArea                             CONSTANT)
    Q_PROPERTY(QString FeeStation                               MEMBER FeeStation                           CONSTANT)
    Q_PROPERTY(QString BadSight                                 MEMBER BadSight                             CONSTANT)
    Q_PROPERTY(QString SleepShelter                             MEMBER SleepShelter                         CONSTANT)
    Q_PROPERTY(QString AutoRedetection                          MEMBER AutoRedetection                      CONSTANT)
    Q_PROPERTY(QString RedetectRouteVeerOff                     MEMBER RedetectRouteVeerOff                 CONSTANT)
    Q_PROPERTY(QString HipassMount                              MEMBER HipassMount                          CONSTANT)
    Q_PROPERTY(QString BestPath                                 MEMBER BestPath                             CONSTANT)
    Q_PROPERTY(QString MinimumTime                              MEMBER MinimumTime                          CONSTANT)
    Q_PROPERTY(QString FreeRoad                                 MEMBER FreeRoad                             CONSTANT)
    Q_PROPERTY(QString ShortestDistance                         MEMBER ShortestDistance                     CONSTANT)
    Q_PROPERTY(QString ExpresswayPriority                       MEMBER ExpresswayPriority                   CONSTANT)
    Q_PROPERTY(QString BeginnerPass                             MEMBER BeginnerPass                         CONSTANT)
    Q_PROPERTY(QString AppVersion                               MEMBER AppVersion                           CONSTANT)
    Q_PROPERTY(QString MapVersion                               MEMBER MapVersion                           CONSTANT)
    Q_PROPERTY(QString SDIVersion                               MEMBER SDIVersion                           CONSTANT)
    Q_PROPERTY(QString LocalMapVersion                          MEMBER LocalMapVersion                      CONSTANT)
    Q_PROPERTY(QString LocalNetworkVersion                      MEMBER LocalNetworkVersion                  CONSTANT)
    Q_PROPERTY(QString LocalPoiVersion                          MEMBER LocalPoiVersion                      CONSTANT)

    Q_PROPERTY(QString MapMode                                  MEMBER MapMode                              CONSTANT)
    Q_PROPERTY(QString OtherRoute                               MEMBER OtherRoute                           CONSTANT)
    Q_PROPERTY(QString SearchType                               MEMBER SearchType                           CONSTANT)
    Q_PROPERTY(QString OilKind                                  MEMBER OilKind                              CONSTANT)
    Q_PROPERTY(QString OilBrand                                 MEMBER OilBrand                             CONSTANT)
    Q_PROPERTY(QString OilArea                                  MEMBER OilArea                              CONSTANT)
    //Q_PROPERTY(QString ChargeKind                               MEMBER ChargeKind                           CONSTANT)
    Q_PROPERTY(QString ChargeType                               MEMBER ChargeType                           CONSTANT)

    Q_PROPERTY(QString Km1                                      MEMBER Km1                                  CONSTANT)
    Q_PROPERTY(QString Km2                                      MEMBER Km2                                  CONSTANT)
    Q_PROPERTY(QString M300                                     MEMBER M300                                 CONSTANT)
    Q_PROPERTY(QString M600                                     MEMBER M600                                 CONSTANT)



    Q_PROPERTY(QString GPSInfoDateY                             MEMBER GPSInfoDateY                         CONSTANT)
    Q_PROPERTY(QString GPSInfoDateM                             MEMBER GPSInfoDateM                         CONSTANT)
    Q_PROPERTY(QString GPSInfoDateD                             MEMBER GPSInfoDateD                         CONSTANT)
    Q_PROPERTY(QString GPSInfoTimeH                             MEMBER GPSInfoTimeH                         CONSTANT)
    Q_PROPERTY(QString GPSInfoTimeM                             MEMBER GPSInfoTimeM                         CONSTANT)
    Q_PROPERTY(QString GPSInfoTimeS                             MEMBER GPSInfoTimeS                         CONSTANT)
    Q_PROPERTY(QString GPSInfoSVCnt                             MEMBER GPSInfoSVCnt                         CONSTANT)
    Q_PROPERTY(QString GPSInfoCpxD                              MEMBER GPSInfoCpxD                          CONSTANT)
    Q_PROPERTY(QString GPSInfoCpxM                              MEMBER GPSInfoCpxM                          CONSTANT)
    Q_PROPERTY(QString GPSInfoCpxS                              MEMBER GPSInfoCpxS                          CONSTANT)
    Q_PROPERTY(QString GPSInfoCpyD                              MEMBER GPSInfoCpyD                          CONSTANT)
    Q_PROPERTY(QString GPSInfoCpyM                              MEMBER GPSInfoCpyM                          CONSTANT)
    Q_PROPERTY(QString GPSInfoCpyS                              MEMBER GPSInfoCpyS                          CONSTANT)
    Q_PROPERTY(QString GPSInfoMode                              MEMBER GPSInfoMode                          CONSTANT)
    Q_PROPERTY(QString GPSInfoSpeed                             MEMBER GPSInfoSpeed                         CONSTANT)
    Q_PROPERTY(QString GPSInfAltitude                           MEMBER GPSInfAltitude                       CONSTANT)
    Q_PROPERTY(QString GPSInfodHdop                             MEMBER GPSInfodHdop                         CONSTANT)
    Q_PROPERTY(QString GPSSVInfo                                MEMBER GPSSVInfo                            CONSTANT)
    Q_PROPERTY(QString GPSSVinfoId                              MEMBER GPSSVinfoId                          CONSTANT)
    Q_PROPERTY(QString GPSSVinfoDb                              MEMBER GPSSVinfoDb                          CONSTANT)
    Q_PROPERTY(QString GPSSVinfoElevation                       MEMBER GPSSVinfoElevation                   CONSTANT)
    Q_PROPERTY(QString GPSSVinfoAzimuth                         MEMBER GPSSVinfoAzimuth                     CONSTANT)

    Q_PROPERTY(QString SessionCreated                           MEMBER SessionCreated                       CONSTANT)
    Q_PROPERTY(QString CurrentProfileId                         MEMBER CurrentProfileId                     CONSTANT)
    Q_PROPERTY(QString SyncedProfileId                          MEMBER SyncedProfileId                      CONSTANT)
    Q_PROPERTY(QString SyncedProfileName                        MEMBER SyncedProfileName                    CONSTANT)
    Q_PROPERTY(QString AuthOTPStatus                            MEMBER AuthOTPStatus                        CONSTANT)
    Q_PROPERTY(QString AuthOTPTryAvailable                      MEMBER AuthOTPTryAvailable                  CONSTANT)
    Q_PROPERTY(QString DeviceId                                 MEMBER DeviceId                             CONSTANT)
    Q_PROPERTY(QString FuelType                                 MEMBER FuelType                             CONSTANT)
    Q_PROPERTY(QString DateFormat                               MEMBER DateFormat                           CONSTANT)
    Q_PROPERTY(QString TimeFormat                               MEMBER TimeFormat                           CONSTANT)


    Q_PROPERTY(QString Off                                      MEMBER Off                                  CONSTANT)
    Q_PROPERTY(QString On                                       MEMBER On                                   CONSTANT)

    Q_PROPERTY(QString GPSlogPath                               MEMBER GPSlogPath                           CONSTANT)
    Q_PROPERTY(QString ScreenshotPath                           MEMBER ScreenshotPath                       CONSTANT)
    Q_PROPERTY(QString ApplogPath                               MEMBER ApplogPath                           CONSTANT)
    Q_PROPERTY(QString DatabasePath                             MEMBER DatabasePath                         CONSTANT)
    Q_PROPERTY(QString MapUpdatePath                            MEMBER MapUpdatePath                        CONSTANT)
    Q_PROPERTY(QString PreferencesPath                          MEMBER PreferencesPath                      CONSTANT)



public:

    enum NotificationK {
        UpdateRouteGuidance = Constants::Setting,
        ApplySetting,
        SaveSetting,
        RequestSetting,
        ChangedSetting,
        RequestSettingByProfileChange,
        SavedSetting,
        InitSetting,
        ResetToFactorySettings,
        UpdateView,
        SaveHiddenSetting, // save setting for all keys on a section
        ApplyHiddenKeySetting, // save key setting of a section
        RequestHiddenSetting, // request setting for all keys on a section
        ResponseHiddenSetting, // response to RequestHiddenSetting or RequestHiddenKeySetting
        ClosedSelfTestView,
        ChangedOtherRouteSetting,
        DialogAccepted,
        DialogCanceled,
        DialogMenuClicked,
        RequestHiddenSettingByReset,
        ExitMapUpdate, // Map update중 View 빠져 나옴(SystemPopupMediator에서 사용)
        SavedSettingFromObjectName,
    };


    enum CarConfReqTypeK {
        VIHICLE_MODEL = 1,
        VIHICLE_FUEL = 18,
        VIHICLE_PETROL = 19,
    };

    enum CarConfResFuelTypeK {
        FUEL_PETROL,
        FUEL_DIESEL,
        FUEL_HYBRID,
        FUEL_HYBRIDPETROL,
        FUEL_ELECTRICITY,
    };

    enum OilTypeK
    {
        OilTypeGasoline           = N_eOilTypeGasoline,	// 휘발유
        OilTypeDiesel             = N_eOilTypeDiesel,   // 경유
        OilTypeLPG                = N_eOilTypeLPG,		// LPG
        OilTypeElec               = N_eOilTypeEV,       // 전기
        // TMCRSM-353 - "고급휘발유" 추가
        OilTypePremiumGasoline,                         // 고급휘발유
        OilTypeNone,
        OilTypeEnd,
    };


    enum VolumeK
    {
        DefaultVolume= 20,
        MaxVolume    = 40
    };

    enum NightModeK
    {
        NightModeAuto,
        NightModeDaylight,
        NightModeNight,
    };

    enum AddressDisplayMethodK
    {        
        AddressDisplayMethodRoadName,
        AddressDisplayMethodLandNumber,
    };

    enum DistanceViewK
    {
        DistanceFarView,
        DistanceBasicView,
        DistanceDetailView,
    };

    enum CharacterK
    {
        CharacterSmall,
        CharacterNormal,
        CharacterBig,
    };

    enum CarIconK
    {
        CarIconBasic0,
        CarIconBasic1,
        CarIconBasic2,
        CarIconCustomer0,
        CarIconCustomer1,
        CarIconCustomer2,
    };

    enum RoadViolationK
    {
        RoadViolation1Km,
        RoadViolation600m,
        RoadViolation300m,

    };

    enum MapModeK
    {
        MapModeBirdView,
        MapModeDriveDirection,
        MapModeNorthDirection,

    };

    enum OtherRouteK
    {
        OtherRouteTMapOptimalPath,
        OtherRouteMinimalTime,
        OtherRouteFreeRoad,
        OtherRouteShortestDistance,
        OtherRouteHighwayPriority,
        OtherRouteBeginnerPass,
    };

    enum OilBrandK
    {
        OilBrandAll,
        OilBrandSK,
        OilBrandS_Oil,
        OilBrandGS,
        OilBrandHyunDaeOilBank,
        OilBrandEtc,
        OilBrandEnd,
    };    

    enum OilAreaK
    {
        OilAreaUpTo2Km,
        OilAreaUpTo5Km,
        OilAreaUpTo7Km,
        OilAreaUpTo10Km,
    };

    enum SearchTypeK
    {
        SearchTypeWithinRadius,
        SearchTypeAlongRoute,
        SearchTypeEnd,
    };

    enum ChargeTypeK
    {
        ChargeTypeAll,
        ChargeTypeAC3,
        ChargeTypeDCDemo,
        ChargeTypeDCCombo,
        ChargeTypeSlowSpeed,
        ChargeTypeEnd,
    };

    SettingConstants();

    static SettingConstants* instance() {
        static SettingConstants Instance;
        return &Instance;
    }

    static QObject *qmlInstance(QQmlEngine *engine, QJSEngine *scriptEngine) {
        Q_UNUSED(engine);
        Q_UNUSED(scriptEngine);

        return new SettingConstants;
    }


public:
    // string
    //commonConstants와 함께 사용
    static const QString Section;
    static const QString Key;
    static const QString Value;
    static const QString Setting;
    static const QString Settings;
    static const QString Type;
    static const QString NoMapData;
    static const QString IncompleteMapData;
    static const QString IncompatibleMapData;
    static const QString ErrorMapData;    
    static const QString Count;
    //-
    static const QString PreferencesName;


    static const QString NaviVolume;
    static const QString NaviVolumeUp;
    static const QString NaviVolumeDown;
    static const QString NaviMute;
    static const QString StoredSize;
    static const QString TotalSize;
    static const QString TotalCount;
    static const QString IgnorePopupFileName;
    static const QString InCompleteMapDataKo;


    // section
    static const QString Section1;
    static const QString MapMain;
    static const QString MapSubSpeedResponse;
    static const QString MapSubMapCharacter;
    static const QString MapSubCarIcon;
    static const QString MapSubOilType;
    static const QString MapSubChargeType;
    static const QString GuidMain;
    static const QString GuidSubVoice;
    static const QString GuidSubCamera;
    static const QString GuidSubAttentionSection;
    static const QString GuidSubVoiceGuidTime;
    static const QString RouteAlternateSelection;    
    static const QString NaviInfo;
    static const QString DirectionGuid;
    static const QString SearchOption;
    //commonConstants와 함께 사용
    static const QString RouteSearch;


    // key
    static const QString LogEnable;
    static const QString DisplayOption;
    static const QString UserName;
    static const QString RecentDestinations;
    static const QString NightMode;
    static const QString AddressDisplayMethod;
    static const QString AddressDisplay2ndMethod;
    static const QString DisplayArroundTrafficInfo;
    static const QString AlwaysDisplayDriveLineTrafficInfo;
    static const QString ExpresswayDrivingGuidance;
    static const QString GpsSpeed;
    static const QString SkyeBox;
    static const QString SpeedResponseMapUse;
    static const QString Distance;
    static const QString Character;
    static const QString Icon;
    static const QString RoadViolation;
    static const QString SafeDrivingSoundAtMute;
    static const QString BeepSoundGuideAtMute;
    static const QString SpeedViolationSectionSignalCrackdown;
    static const QString Male;
    static const QString CrackdownOnSpeedViolation;
    static const QString SectionControl;
    static const QString SignalControl;
    static const QString DetachableCrackdown;
    static const QString CrackdownWithBusExclusiveCar;
    static const QString ParkingControl;
    static const QString InterruptRegulation;
    static const QString MonitoringOfRoadShoulder;
    static const QString CollectingTrafficInformation;
    static const QString ControlOfVariableVehicles;
    static const QString SpeedBump;
    static const QString AccidentFrequent;
    static const QString ChildProtectedArea;
    static const QString FogCaution;
    static const QString AttentionToWildAnimals;
    static const QString RailroadXing;
    static const QString SteepSlopeSection;
    static const QString SharpCurve;
    static const QString OneWay;
    static const QString FrequentSignalViolation;
    static const QString SpeedTravelFrequently;
    static const QString TrafficJamArea;
    static const QString ChangeSectionOfCar;
    static const QString ChargingStation;
    static const QString Tunnel;
    static const QString RestArea;
    static const QString FeeStation;
    static const QString BadSight;
    static const QString SleepShelter;
    static const QString AutoRedetection;
    static const QString RedetectRouteVeerOff;
    static const QString HipassMount;
    static const QString BestPath;
    static const QString MinimumTime;
    static const QString FreeRoad;
    static const QString ShortestDistance;
    static const QString ExpresswayPriority;
    static const QString BeginnerPass;
    static const QString AppVersion;
    static const QString MapVersion ;    
    static const QString SDIVersion;
    static const QString LocalMapVersion;
    static const QString LocalNetworkVersion;
    static const QString LocalPoiVersion;
    static const QString MapMode;
    static const QString OtherRoute;
    static const QString SearchType;
    static const QString OilKind;
    static const QString OilBrand;
    static const QString OilArea;
    //static const QString ChargeKind;
    static const QString ChargeType;
    static const QString Km1;
    static const QString Km2;
    static const QString M300;
    static const QString M600;



    static const QString GPSInfoDateY  ;
    static const QString GPSInfoDateM  ;
    static const QString GPSInfoDateD  ;
    static const QString GPSInfoTimeH  ;
    static const QString GPSInfoTimeM  ;
    static const QString GPSInfoTimeS  ;
    static const QString GPSInfoSVCnt  ;
    static const QString GPSInfoCpxD   ;
    static const QString GPSInfoCpxM   ;
    static const QString GPSInfoCpxS   ;
    static const QString GPSInfoCpyD   ;
    static const QString GPSInfoCpyM   ;
    static const QString GPSInfoCpyS   ;
    static const QString GPSInfoMode   ;
    static const QString GPSInfoSpeed  ;
    static const QString GPSInfAltitude;
    static const QString GPSInfodHdop  ;
    static const QString GPSSVInfo     ;
    static const QString GPSSVinfoId   ;
    static const QString GPSSVinfoDb   ;
    static const QString GPSSVinfoElevation ;
    static const QString GPSSVinfoAzimuth ;

    static const QString SessionCreated;
    static const QString CurrentProfileId;
    static const QString SyncedProfileId;
    static const QString SyncedProfileName;
    static const QString AuthOTPStatus;
    static const QString AuthOTPTryAvailable;
    static const QString DeviceId;
    static const QString FuelType;


    static const QString DateFormat;
    static const QString TimeFormat;

    static const QString Off;
    static const QString On;

    static       QString GPSlogPath;
    static       QString ScreenshotPath;
    static       QString ApplogPath;
    static       QString DatabasePath;
    static       QString MapUpdatePath;
    static       QString PreferencesPath;
    static       QString RgDataPath;
};

}
#endif // SETTINGCONSTANTS_H
