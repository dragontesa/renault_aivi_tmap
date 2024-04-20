#include "SettingConstants.h"
#include "../environment.h"


using namespace SKT;

 // string
//commonConstants와 함께 사용
 const QString SettingConstants::Section                              = QStringLiteral("section");
 const QString SettingConstants::Key                                  = QStringLiteral("key");
 const QString SettingConstants::Value                                = QStringLiteral("value");
 const QString SettingConstants::Setting                              = QStringLiteral("setting");
 const QString SettingConstants::Settings                             = QStringLiteral("settings");
 const QString SettingConstants::Type                                 = QStringLiteral("type");
 const QString SettingConstants::NoMapData                            = QStringLiteral("noMapData");
 const QString SettingConstants::IncompleteMapData                    = QStringLiteral("incompleteMapData");
 const QString SettingConstants::IncompatibleMapData                  = QStringLiteral("incompatibleMapData");
 const QString SettingConstants::ErrorMapData                         = QStringLiteral("errorMapData");
 const QString SettingConstants::Count                                = QStringLiteral("count");
 //-
 const QString SettingConstants::PreferencesName                      = QStringLiteral("preferencesName");


 const QString SettingConstants::NaviVolume                           = QStringLiteral("NaviVolume");
 const QString SettingConstants::NaviVolumeUp                         = QStringLiteral("NaviVolumeUp");
 const QString SettingConstants::NaviVolumeDown                       = QStringLiteral("NaviVolumeDown");
 const QString SettingConstants::NaviMute                             = QStringLiteral("NaviMute");
 const QString SettingConstants::StoredSize                           = QStringLiteral("StoredSize");
 const QString SettingConstants::TotalSize                            = QStringLiteral("TotalSize");
 const QString SettingConstants::TotalCount                           = QStringLiteral("TotalCount");
 const QString SettingConstants::IgnorePopupFileName                  = QStringLiteral(".ignore_popup");
 const QString SettingConstants::InCompleteMapDataKo                   = QStringLiteral("지도가 온전히 복사되지 않았습니다.<br>지도를 업데이트 하려면<br>USB를 다시 넣어주세요.");

 // section
 const QString SettingConstants::Section1                             = QStringLiteral("Section1");
 const QString SettingConstants::MapMain                              = QStringLiteral("MapMain");
 const QString SettingConstants::MapSubSpeedResponse                  = QStringLiteral("MapSubSpeedResponse");
 const QString SettingConstants::MapSubMapCharacter                   = QStringLiteral("MapSubMapCharacter");
 const QString SettingConstants::MapSubCarIcon                        = QStringLiteral("MapSubCarIcon");
 const QString SettingConstants::MapSubOilType                        = QStringLiteral("MapSubOilType");
 const QString SettingConstants::MapSubChargeType                     = QStringLiteral("MapSubChargeType");
 const QString SettingConstants::GuidMain                             = QStringLiteral("GuidMain");
 const QString SettingConstants::GuidSubVoice                         = QStringLiteral("GuidSubVoice");
 const QString SettingConstants::GuidSubCamera                        = QStringLiteral("GuidSubCamera");
 const QString SettingConstants::GuidSubAttentionSection              = QStringLiteral("GuidSubAttentionSection");
 const QString SettingConstants::GuidSubVoiceGuidTime                 = QStringLiteral("GuidSubVoiceGuidTime");
 const QString SettingConstants::RouteAlternateSelection              = QStringLiteral("RouteAlternateSelection"); 
 const QString SettingConstants::NaviInfo                             = QStringLiteral("NaviInfo");
 const QString SettingConstants::DirectionGuid                        = QStringLiteral("DirectionGuid");
 const QString SettingConstants::SearchOption                         = QStringLiteral("SearchOption");
 //commonConstants와 함께 사용
 const QString SettingConstants::RouteSearch                          = QStringLiteral("RouteSearch");


 // key
const QString SettingConstants::LogEnable                             = QStringLiteral("LogEnable");
const QString SettingConstants::DisplayOption                         = QStringLiteral("DisplayOption");
const QString SettingConstants::UserName                              = QStringLiteral("UserName");
const QString SettingConstants::RecentDestinations                    = QStringLiteral("RecentDestinations");
const QString SettingConstants::NightMode                             = QStringLiteral("NightMode");
const QString SettingConstants::AddressDisplayMethod                  = QStringLiteral("AddressDisplayMethod");
const QString SettingConstants::AddressDisplay2ndMethod               = QStringLiteral("AddressDisplay2ndMethod");
const QString SettingConstants::DisplayArroundTrafficInfo             = QStringLiteral("DisplayArroundTrafficInfo");
const QString SettingConstants::AlwaysDisplayDriveLineTrafficInfo     = QStringLiteral("AlwaysDisplayDriveLineTrafficInfo");
const QString SettingConstants::ExpresswayDrivingGuidance             = QStringLiteral("ExpresswayDrivingGuidance");
const QString SettingConstants::GpsSpeed                              = QStringLiteral("GpsSpeed");
const QString SettingConstants::SkyeBox                               = QStringLiteral("SkyeBox");
const QString SettingConstants::SpeedResponseMapUse                   = QStringLiteral("SpeedResponseMapUse");
const QString SettingConstants::Distance                              = QStringLiteral("Distance");
const QString SettingConstants::Character                             = QStringLiteral("Character");
const QString SettingConstants::Icon                                  = QStringLiteral("Icon");
const QString SettingConstants::RoadViolation                         = QStringLiteral("RoadViolation");
const QString SettingConstants::SafeDrivingSoundAtMute                = QStringLiteral("SafeDrivingSoundAtMute");
const QString SettingConstants::BeepSoundGuideAtMute                  = QStringLiteral("BeepSoundGuideAtMute");
const QString SettingConstants::SpeedViolationSectionSignalCrackdown  = QStringLiteral("SpeedViolationSectionSignalCrackdown");
const QString SettingConstants::Male                                  = QStringLiteral("Male");
const QString SettingConstants::CrackdownOnSpeedViolation             = QStringLiteral("CrackdownOnSpeedViolation");
const QString SettingConstants::SectionControl                        = QStringLiteral("SectionControl");
const QString SettingConstants::SignalControl                         = QStringLiteral("SignalControl");
const QString SettingConstants::DetachableCrackdown                   = QStringLiteral("DetachableCrackdown");
const QString SettingConstants::CrackdownWithBusExclusiveCar          = QStringLiteral("CrackdownWithBusExclusiveCar");
const QString SettingConstants::ParkingControl                        = QStringLiteral("ParkingControl");
const QString SettingConstants::InterruptRegulation                   = QStringLiteral("InterruptRegulation");
const QString SettingConstants::MonitoringOfRoadShoulder              = QStringLiteral("MonitoringOfRoadShoulder");
const QString SettingConstants::CollectingTrafficInformation          = QStringLiteral("CollectingTrafficInformation");
const QString SettingConstants::ControlOfVariableVehicles             = QStringLiteral("ControlOfVariableVehicles");
const QString SettingConstants::SpeedBump                             = QStringLiteral("SpeedBump");
const QString SettingConstants::AccidentFrequent                      = QStringLiteral("AccidentFrequent");
const QString SettingConstants::ChildProtectedArea                    = QStringLiteral("ChildProtectedArea");
const QString SettingConstants::FogCaution                            = QStringLiteral("FogCaution");
const QString SettingConstants::AttentionToWildAnimals                = QStringLiteral("AttentionToWildAnimals");
const QString SettingConstants::RailroadXing                          = QStringLiteral("RailroadXing");
const QString SettingConstants::SteepSlopeSection                     = QStringLiteral("SteepSlopeSection");
const QString SettingConstants::SharpCurve                            = QStringLiteral("SharpCurve");
const QString SettingConstants::OneWay                                = QStringLiteral("OneWay");
const QString SettingConstants::FrequentSignalViolation               = QStringLiteral("FrequentSignalViolation");
const QString SettingConstants::SpeedTravelFrequently                 = QStringLiteral("SpeedTravelFrequently");
const QString SettingConstants::TrafficJamArea                        = QStringLiteral("TrafficJamArea");
const QString SettingConstants::ChangeSectionOfCar                    = QStringLiteral("ChangeSectionOfCar");
const QString SettingConstants::ChargingStation                       = QStringLiteral("ChargingStation");
const QString SettingConstants::Tunnel                                = QStringLiteral("Tunnel");
const QString SettingConstants::RestArea                              = QStringLiteral("RestArea");
const QString SettingConstants::FeeStation                            = QStringLiteral("FeeStation");
const QString SettingConstants::BadSight                              = QStringLiteral("BadSight");
const QString SettingConstants::SleepShelter                          = QStringLiteral("SleepShelter");
const QString SettingConstants::AutoRedetection                       = QStringLiteral("AutoRedetection");
const QString SettingConstants::RedetectRouteVeerOff                  = QStringLiteral("RedetectRouteVeerOff");
const QString SettingConstants::HipassMount                           = QStringLiteral("HipassMount");
const QString SettingConstants::BestPath                              = QStringLiteral("BestPath");
const QString SettingConstants::MinimumTime                           = QStringLiteral("MinimumTime");
const QString SettingConstants::FreeRoad                              = QStringLiteral("FreeRoad");
const QString SettingConstants::ShortestDistance                      = QStringLiteral("ShortestDistance");
const QString SettingConstants::ExpresswayPriority                    = QStringLiteral("ExpresswayPriority");
const QString SettingConstants::BeginnerPass                          = QStringLiteral("BeginnerPass");
const QString SettingConstants::AppVersion                            = QStringLiteral("AppVersion");
const QString SettingConstants::MapVersion                            = QStringLiteral("MapVersion");
const QString SettingConstants::SDIVersion                            = QStringLiteral("SDIVersion");
const QString SettingConstants::LocalMapVersion                       = QStringLiteral("LocalMapVersion");
const QString SettingConstants::LocalNetworkVersion                   = QStringLiteral("LocalNetworkVersion");
const QString SettingConstants::LocalPoiVersion                       = QStringLiteral("LocalPoiVersion");
const QString SettingConstants::MapMode                               = QStringLiteral("MapMode");
const QString SettingConstants::OtherRoute                            = QStringLiteral("OtherRoute");
const QString SettingConstants::SearchType                            = QStringLiteral("SearchType");
const QString SettingConstants::OilKind                               = QStringLiteral("OilKind");
const QString SettingConstants::OilBrand                              = QStringLiteral("OilBrand");
const QString SettingConstants::OilArea                               = QStringLiteral("OilArea");
//const QString SettingConstants::ChargeKind                            = QStringLiteral("ChargeKind");
const QString SettingConstants::ChargeType                            = QStringLiteral("ChargeType");
const QString SettingConstants::Km1                                   = QStringLiteral("Km1");
const QString SettingConstants::Km2                                   = QStringLiteral("Km2");
const QString SettingConstants::M300                                  = QStringLiteral("M300");
const QString SettingConstants::M600                                  = QStringLiteral("M600");


const QString SettingConstants::GPSInfoDateY                          = QStringLiteral("GPSInfoDateY");
const QString SettingConstants::GPSInfoDateM                          = QStringLiteral("GPSInfoDateM");
const QString SettingConstants::GPSInfoDateD                          = QStringLiteral("GPSInfoDateD");
const QString SettingConstants::GPSInfoTimeH                          = QStringLiteral("GPSInfoTimeH");
const QString SettingConstants::GPSInfoTimeM                          = QStringLiteral("GPSInfoTimeM");
const QString SettingConstants::GPSInfoTimeS                          = QStringLiteral("GPSInfoTimeS");
const QString SettingConstants::GPSInfoSVCnt                          = QStringLiteral("GPSInfoSVCnt");
const QString SettingConstants::GPSInfoCpxD                           = QStringLiteral("GPSInfoCpxD");
const QString SettingConstants::GPSInfoCpxM                           = QStringLiteral("GPSInfoCpxM");
const QString SettingConstants::GPSInfoCpxS                           = QStringLiteral("GPSInfoCpxS");
const QString SettingConstants::GPSInfoCpyD                           = QStringLiteral("GPSInfoCpyD");
const QString SettingConstants::GPSInfoCpyM                           = QStringLiteral("GPSInfoCpyM");
const QString SettingConstants::GPSInfoCpyS                           = QStringLiteral("GPSInfoCpyS");
const QString SettingConstants::GPSInfoMode                           = QStringLiteral("GPSInfoMode");
const QString SettingConstants::GPSInfoSpeed                          = QStringLiteral("GPSInfoSpeed");
const QString SettingConstants::GPSInfAltitude                        = QStringLiteral("GPSInfAltitude");
const QString SettingConstants::GPSInfodHdop                          = QStringLiteral("GPSInfodHdop");
const QString SettingConstants::GPSSVInfo                             = QStringLiteral("GPSSVInfo");

const QString SettingConstants::GPSSVinfoId                           = QStringLiteral("GPSSVinfoId");
const QString SettingConstants::GPSSVinfoDb                           = QStringLiteral("GPSSVinfoDb");
const QString SettingConstants::GPSSVinfoElevation                    = QStringLiteral("GPSSVinfoElevation");
const QString SettingConstants::GPSSVinfoAzimuth                      = QStringLiteral("GPSSVinfoAzimuth");

const QString SettingConstants::SessionCreated                        = QStringLiteral("SessionCreated");
const QString SettingConstants::CurrentProfileId                      = QStringLiteral("CurrentProfileId");
const QString SettingConstants::SyncedProfileId                       = QStringLiteral("SyncedProfileId");
const QString SettingConstants::SyncedProfileName                     = QStringLiteral("SyncedProfileName");
const QString SettingConstants::AuthOTPStatus                         = QStringLiteral("AuthOTPStatus");
const QString SettingConstants::AuthOTPTryAvailable                   = QStringLiteral("AuthOTPTryAvailable");
const QString SettingConstants::DeviceId                              = QStringLiteral("DeviceId");
const QString SettingConstants::FuelType                              = QStringLiteral("FuelType");


const QString SettingConstants::DateFormat                            = QStringLiteral("DateFormat");
const QString SettingConstants::TimeFormat                            = QStringLiteral("TimeFormat");


const QString SettingConstants::Off                                   = QStringLiteral("OFF");
const QString SettingConstants::On                                    = QStringLiteral("ON");

      QString SettingConstants::GPSlogPath                            = QStringLiteral("");
      QString SettingConstants::ScreenshotPath                        = QStringLiteral("");
      QString SettingConstants::ApplogPath                            = QStringLiteral("");
      QString SettingConstants::DatabasePath                          = QStringLiteral("");
      QString SettingConstants::MapUpdatePath                         = QStringLiteral("");
      QString SettingConstants::PreferencesPath                       = QStringLiteral("");
      QString SettingConstants::RgDataPath                            = QStringLiteral("");

SettingConstants::SettingConstants()
{
    SettingConstants::GPSlogPath       = Environment::getSysGpsLogPath();
    SettingConstants::ScreenshotPath   = Environment::getScreenshotPath();
    SettingConstants::ApplogPath       = Environment::getLogPath();
    SettingConstants::DatabasePath     = Environment::getDatabasePath();
    SettingConstants::MapUpdatePath    = Environment::getMapUpdatePath();    
    SettingConstants::PreferencesPath  = Environment::getPreferencePath();
    SettingConstants::RgDataPath         = Environment::getRGDataPath();
}
