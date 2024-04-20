#include "StartupCommand.h"
#include "Facade.h"
#include "mediator/app/ApplicationMediator.h"
#include "MediateViewCommand.h"
#include "Constants.h"
#include "InterfaceConstants.h"
#include "RgConstants.h"
#include "RouteConstants.h"
#include "SearchConstants.h"
#include "MapConstants.h"
#include "SettingConstants.h"
#include "SimulatorConstants.h"
#include "PreferenceConstants.h"
#include "SoundConstants.h"
#include "TestConstants.h"
#include "NavigationControllerConstants.h"
#include "NavigationUpdateManagerConstants.h"
#include "NavigationPropertyConstants.h"
#include "RouteGuidanceProxy.h"
#include "VsmMapProxy.h"
#include "SettingProxy.h"
#include "VsmNaviProxy.h"
#include "RouteSearchProxy.h"
#include "IntegratedSearchProxy.h"
#include "NavigationControllerProxy.h"
#include "NavigationUpdateManagerProxy.h"
#include "NavigationPropertyProxy.h"
#include "NavibookmarkAgentProxy.h"
#include "OfflineJobCacheProxy.h"
#include "InterfaceProxy.h"

#include "SettingDto.h"
#include "TSettings.h"
#include "ManageSettingCommand.h"
#include "ManageSimulatorCommand.h"
#include "ManageTestCommand.h"
#include "ManageVsmNaviCommand.h"
#include "ManageVsmMapCommand.h"
#include "ManageVsmNaviCommand.h"
#include "ManageRouteSearchCommand.h"
#include "ManageRouteGuidanceCommand.h"
#include "ManageIntegratedSearchCommand.h"
#include "ManageNavigationControllerCommand.h"
#include "ManageNavigationUpdateManagerCommand.h"
#include "ManageNavigationPropertyCommand.h"
#include "ManageRecentDestinationCommand.h"
#include "ManageRecentSearchCommand.h"
#include "ManageFavoriteCommand.h"
#include "ManageInterfaceCommand.h"
#include "PreferenceProxy.h"
#include "ManagePreferenceCommand.h"

#include "NavibookmarkAgentMediator.h"
#include "AddressMediator.h"
#include "mediator/app/CswMediator.h"
#include "mediator/app/SystemPopupMediator.h"
#include "mediator/route/RouteSearchMediator.h"
#include "mediator/search/FavoriteMediator.h"
#include "mediator/map/SkyStateMediator.h"

#include "NavigationControllerProtocol.h"
#include "NavigationControllerConstants.h"
#include "LangConstants.h"
#include "HiddenSettingConstants.h"
#include "qctestingfeatures.h"

using namespace SKT;

StartupCommand::StartupCommand( QObject* parent )
    : SimpleCommand( parent )
{

}

bool StartupCommand::execute( const mvc::INotificationPtr& note )
{
    T_DEBUGV( "current thread id: %d", QThread::currentThreadId() );

    auto body = note->getBody().value<QVariantMap>();
    auto qmlEngine = body["qmlEngine"].value<QQmlEngine*>();
    auto resDirPath = body["resDirPath"].toString();
    auto appView = body["appView"].value<QObject*>();
    auto gpsSerivce = body["gpsSerivce"].value<QObject*>();
    auto simulator = body["simulator"].value<QObject*>();
    auto controller = body["controller"].value<QObject*>();
    auto addressManager = body["addressManager"].value<QObject*>();
    auto prefDirPath = body["preferenceDirPath"].toString();
    auto navigationProperty = body["navigationProperty"].value<QObject*>();
    auto updateManager = body["updateManager"].value<QObject*>();
    auto hasMapData = body["hasMapData"].value<bool>();
    auto languageIndex = body["languageIndex"].toChar().toLatin1();
    auto gadgetSignal = body["gadgetSignal"].toMap();
    auto gpsRfSignal = body["gpsRfSignal"].toMap();

    // proxy 등록

    Facade::instance()->registerProxy( t_new_shared<NavibookmarkAgentProxy>() );
    Facade::instance()->registerProxy( t_new_shared<RouteGuidanceProxy>( gpsSerivce, simulator ) );
    Facade::instance()->registerProxy( t_new_shared<VsmMapProxy>() );
    Facade::instance()->registerProxy( t_new_shared<SettingProxy>() );
    Facade::instance()->registerProxy( t_new_shared<VsmNaviProxy>(controller) );
    Facade::instance()->registerProxy( t_new_shared<RouteSearchProxy>() );
    Facade::instance()->registerProxy( t_new_shared<IntegratedSearchProxy>(controller) );
    Facade::instance()->registerProxy( t_new_shared<PreferenceProxy>( prefDirPath ) );
    Facade::instance()->registerProxy( t_new_shared<NavigationPropertyProxy>(navigationProperty) );
    Facade::instance()->registerProxy( t_new_shared<NavigationUpdateManagerProxy>( controller, updateManager, navigationProperty) );
    Facade::instance()->registerProxy( t_new_shared<OfflineJobCacheProxy>() );
    Facade::instance()->registerProxy( t_new_shared<InterfaceProxy>() );
    Facade::instance()->registerProxy( t_new_shared<NavigationControllerProxy>( controller, navigationProperty, languageIndex, gadgetSignal, gpsRfSignal, body) );


    // command 등록

    Facade::instance()->registerCommand( Constants::MediateView, &MediateViewCommand::staticMetaObject );
    Facade::instance()->registerCommand( Constants::RemoveView, &MediateViewCommand::staticMetaObject );
    Facade::instance()->registerCommand( SettingConstants::RequestSetting, &ManageSettingCommand::staticMetaObject );
    Facade::instance()->registerCommand( SettingConstants::SaveSetting, &ManageSettingCommand::staticMetaObject );
    Facade::instance()->registerCommand( SettingConstants::InitSetting, &ManageSettingCommand::staticMetaObject );
    Facade::instance()->registerCommand( SettingConstants::ResetToFactorySettings, &ManageSettingCommand::staticMetaObject );
    Facade::instance()->registerCommand( SettingConstants::RequestHiddenSetting, &ManageSettingCommand::staticMetaObject );
    Facade::instance()->registerCommand( SettingConstants::ResponseHiddenSetting, &ManageSettingCommand::staticMetaObject );
    Facade::instance()->registerCommand( SettingConstants::SaveHiddenSetting, &ManageSettingCommand::staticMetaObject );
    Facade::instance()->registerCommand( SettingConstants::RequestSettingByProfileChange, &ManageSettingCommand::staticMetaObject );
    Facade::instance()->registerCommand( SimulatorConstants::StartSimulator, &ManageSimulatorCommand::staticMetaObject );
    Facade::instance()->registerCommand( SimulatorConstants::StopSimulator, &ManageSimulatorCommand::staticMetaObject );
    Facade::instance()->registerCommand( TestConstants::RunTest, &ManageTestCommand::staticMetaObject );

    Facade::instance()->registerCommand( MapConstants::AttachMapViewOnVsmNavi, &ManageVsmNaviCommand::staticMetaObject );
    Facade::instance()->registerCommand( MapConstants::SetMapViewAsGlobal, &ManageVsmMapCommand::staticMetaObject );
    Facade::instance()->registerCommand( MapConstants::DetachMapViewFromVsmNavi, &ManageVsmNaviCommand::staticMetaObject );
    Facade::instance()->registerCommand( MapConstants::ApplyMapSetting, &ManageVsmMapCommand::staticMetaObject );
    Facade::instance()->registerCommand( MapConstants::ApplyMapTheme, &ManageVsmMapCommand::staticMetaObject );

    Facade::instance()->registerCommand( HiddenSettingConstants::CaptureScreenshot, &ManageTestCommand::staticMetaObject );
    Facade::instance()->registerCommand( HiddenSettingConstants::ChangeGpsMapmatch, &ManageTestCommand::staticMetaObject );
    Facade::instance()->registerCommand( HiddenSettingConstants::ChangeGpsLogUsage, &ManageTestCommand::staticMetaObject );
    Facade::instance()->registerCommand( HiddenSettingConstants::ApplyGpsTimeBias, &ManageTestCommand::staticMetaObject );
    Facade::instance()->registerCommand( HiddenSettingConstants::ApplyVsmLastDebugSetting, &ManageTestCommand::staticMetaObject );
    Facade::instance()->registerCommand( HiddenSettingConstants::ChangeTnappServerIpPort, &ManageTestCommand::staticMetaObject );
    Facade::instance()->registerCommand( HiddenSettingConstants::ChangeNddsServer, &ManageTestCommand::staticMetaObject );
    Facade::instance()->registerCommand( HiddenSettingConstants::ChangeAvNextServer, &ManageTestCommand::staticMetaObject );
    Facade::instance()->registerCommand( HiddenSettingConstants::ChangeSafeHelperServerIpPort, &ManageTestCommand::staticMetaObject );
    Facade::instance()->registerCommand( HiddenSettingConstants::ChangeTvasVersion, &ManageTestCommand::staticMetaObject );
    Facade::instance()->registerCommand( HiddenSettingConstants::ForceCrashApp, &ManageTestCommand::staticMetaObject );
    Facade::instance()->registerCommand( HiddenSettingConstants::EnableDaynightTimeset, &ManageTestCommand::staticMetaObject );
    Facade::instance()->registerCommand( HiddenSettingConstants::DoExportLogsToUSB, &ManageTestCommand::staticMetaObject );

    Facade::instance()->registerCommand( HiddenSettingConstants::ChangeVsmLevelSettingUi, &ManageTestCommand::staticMetaObject );
    Facade::instance()->registerCommand( HiddenSettingConstants::ChangeVsmDebugSettingUi, &ManageTestCommand::staticMetaObject );
    Facade::instance()->registerCommand( HiddenSettingConstants::ChangeVsmServer, &ManageTestCommand::staticMetaObject );
    Facade::instance()->registerCommand( HiddenSettingConstants::ChangeVsmAutoLevelMap, &ManageTestCommand::staticMetaObject );
    #ifdef TMAP_TMC_QC_TESTING
    Facade::instance()->registerCommand( HiddenSettingConstants::ChangeMapmatchingFeedback, &ManageTestCommand::staticMetaObject );
    Facade::instance()->registerCommand( HiddenSettingConstants::ChangeOnlyTunnelMMFB, &ManageTestCommand::staticMetaObject );
    Facade::instance()->registerCommand( HiddenSettingConstants::ChangeMMFeedbackLog, &ManageTestCommand::staticMetaObject );
    Facade::instance()->registerCommand( HiddenSettingConstants::ChangeMMFeedbackGpsMethod, &ManageTestCommand::staticMetaObject );
    Facade::instance()->registerCommand( HiddenSettingConstants::ChangeMMFeedbackAngleOnTunnel, &ManageTestCommand::staticMetaObject );
    Facade::instance()->registerCommand( HiddenSettingConstants::ChangeMMFeedbackDrOnly, &ManageTestCommand::staticMetaObject );
    Facade::instance()->registerCommand( HiddenSettingConstants::ChangeMMFeedbackDrCEP, &ManageTestCommand::staticMetaObject );
    Facade::instance()->registerCommand( HiddenSettingConstants::ChangeMMFeedbackDrCEPrange, &ManageTestCommand::staticMetaObject );
    Facade::instance()->registerCommand( HiddenSettingConstants::ChangeMMFeedbackDrCEPMMrange, &ManageTestCommand::staticMetaObject );
    Facade::instance()->registerCommand( HiddenSettingConstants::ChangeGpsLogToKML, &ManageTestCommand::staticMetaObject );
    Facade::instance()->registerCommand( HiddenSettingConstants::ChangeMapmatchingDebugText, &ManageTestCommand::staticMetaObject );
    #endif
    Facade::instance()->registerCommand( HiddenSettingConstants::ChangeVsmConfigId, &ManageTestCommand::staticMetaObject );
    Facade::instance()->registerCommand( HiddenSettingConstants::ChangeVsmConfigVersion, &ManageTestCommand::staticMetaObject );
    #ifdef TMAP_TMC_QC_TESTING
    Facade::instance()->registerCommand( HiddenSettingConstants::ChangeVsmUseFps, &ManageTestCommand::staticMetaObject );
    Facade::instance()->registerCommand( HiddenSettingConstants::ChangeVsmDisable3dObjects, &ManageTestCommand::staticMetaObject );
    Facade::instance()->registerCommand( HiddenSettingConstants::ChangeVsmGLAntiAliasing, &ManageTestCommand::staticMetaObject );
    Facade::instance()->registerCommand( HiddenSettingConstants::ChangeVsmFlickSpeedTest, &ManageTestCommand::staticMetaObject );
    #endif
    
    #ifdef TMAP_TMC_DEV_TESTING
    Facade::instance()->registerCommand( HiddenSettingConstants::MapInClusterStartStop, &ManageTestCommand::staticMetaObject );
    Facade::instance()->registerCommand( HiddenSettingConstants::ChangeMapInClusterLayout, &ManageTestCommand::staticMetaObject );
    Facade::instance()->registerCommand( HiddenSettingConstants::ForceToOnlineState, &ManageTestCommand::staticMetaObject );
    Facade::instance()->registerCommand( HiddenSettingConstants::ChangeLanguage, &ManageTestCommand::staticMetaObject );
    #endif

    Facade::instance()->registerCommand( RouteConstants::RequestRouteSummary, &ManageVsmNaviCommand::staticMetaObject );
    Facade::instance()->registerCommand( RouteConstants::RequestRouteSummaryDetail, &ManageVsmNaviCommand::staticMetaObject );
    Facade::instance()->registerCommand( RouteConstants::CancelRoute, &ManageVsmNaviCommand::staticMetaObject );
    Facade::instance()->registerCommand( RouteConstants::SelectRoute, &ManageVsmNaviCommand::staticMetaObject);
    Facade::instance()->registerCommand( RouteConstants::SetTvas, &ManageVsmNaviCommand::staticMetaObject);
    Facade::instance()->registerCommand( SoundConstants::PlaySound, &ManageVsmNaviCommand::staticMetaObject);
    Facade::instance()->registerCommand( SoundConstants::PlayBeep, &MangeNavigationControllerCommand::staticMetaObject);
    Facade::instance()->registerCommand( RouteConstants::ResumeRoute, &ManageVsmNaviCommand::staticMetaObject);
    Facade::instance()->registerCommand( RouteConstants::ChangeReRouteIntervalTime, &ManageVsmNaviCommand::staticMetaObject);
    Facade::instance()->registerCommand( NavigationControllerConstants::UpdateCavatarIcon, &ManageVsmNaviCommand::staticMetaObject);
    Facade::instance()->registerCommand( NavigationControllerConstants::SetCavatarIcon, &ManageVsmNaviCommand::staticMetaObject);
    Facade::instance()->registerCommand( MapConstants::UpdateMapView, &ManageVsmMapCommand::staticMetaObject);
    Facade::instance()->registerCommand( RouteConstants::AddVia, &ManageVsmMapCommand::staticMetaObject);
    Facade::instance()->registerCommand( RouteConstants::InsertVia, &ManageVsmMapCommand::staticMetaObject);
    Facade::instance()->registerCommand( RouteConstants::DeleteVia, &ManageVsmMapCommand::staticMetaObject);
    Facade::instance()->registerCommand( RouteConstants::SelectViaSlot, &ManageVsmMapCommand::staticMetaObject);
    Facade::instance()->registerCommand( RouteConstants::SetArrival, &ManageVsmMapCommand::staticMetaObject);
    Facade::instance()->registerCommand( RouteConstants::SetDeparture, &ManageVsmMapCommand::staticMetaObject);
    Facade::instance()->registerCommand( RouteConstants::SetVia, &ManageVsmMapCommand::staticMetaObject);
    Facade::instance()->registerCommand( RouteConstants::SetStartCode, &ManageVsmMapCommand::staticMetaObject);
    Facade::instance()->registerCommand( RouteConstants::SetExploreCode, &ManageVsmMapCommand::staticMetaObject);
    Facade::instance()->registerCommand( RouteConstants::SetHipass, &ManageRouteSearchCommand::staticMetaObject);
    Facade::instance()->registerCommand( MapConstants::ApplyMapPosition, &ManageVsmMapCommand::staticMetaObject);
    Facade::instance()->registerCommand( MapConstants::ApplyMapViewMode, &ManageVsmNaviCommand::staticMetaObject);
    Facade::instance()->registerCommand( MapConstants::AdjustMapLevel, &ManageVsmMapCommand::staticMetaObject);
    Facade::instance()->registerCommand( MapConstants::ShowCarvata, &ManageVsmMapCommand::staticMetaObject);
    Facade::instance()->registerCommand( MapConstants::RequestMapPosition, &ManageVsmMapCommand::staticMetaObject);
    Facade::instance()->registerCommand( MapConstants::RequestMapScale, &ManageVsmMapCommand::staticMetaObject);
    Facade::instance()->registerCommand( MapConstants::RequestMapRect, &ManageVsmMapCommand::staticMetaObject);
    Facade::instance()->registerCommand( MapConstants::ResetMapPosition, &ManageVsmMapCommand::staticMetaObject);
    Facade::instance()->registerCommand( MapConstants::RendererSizeChanged, &ManageVsmMapCommand::staticMetaObject);
    Facade::instance()->registerCommand( MapConstants::ChangeMapViewMode, &ManageVsmMapCommand::staticMetaObject);
    Facade::instance()->registerCommand( MapConstants::DrawRouteAll, &ManageVsmMapCommand::staticMetaObject);
    Facade::instance()->registerCommand( MapConstants::ShowGasStationInfo, &ManageVsmMapCommand::staticMetaObject);
    Facade::instance()->registerCommand( MapConstants::HideGasStationInfo, &ManageVsmMapCommand::staticMetaObject);
    Facade::instance()->registerCommand( MapConstants::RemoveCalloutPopup, &ManageVsmMapCommand::staticMetaObject);
    Facade::instance()->registerCommand( MapConstants::RequestMapAreaName, &ManageVsmMapCommand::staticMetaObject);
    Facade::instance()->registerCommand( MapConstants::ResponseMapAreaName, &ManageVsmMapCommand::staticMetaObject);
    Facade::instance()->registerCommand( MapConstants::ShowRoute, &ManageVsmMapCommand::staticMetaObject);
    Facade::instance()->registerCommand( MapConstants::MoveMapByScreenPos, &ManageVsmMapCommand::staticMetaObject);
    Facade::instance()->registerCommand( MapConstants::MoveMapByWorldPos, &ManageVsmMapCommand::staticMetaObject);
    Facade::instance()->registerCommand( MapConstants::MoveMapByUser, &ManageRouteSearchCommand::staticMetaObject);
    Facade::instance()->registerCommand( RouteConstants::HitSearchMarker, &ManageRouteSearchCommand::staticMetaObject);
    Facade::instance()->registerCommand( SearchConstants::RequestCommand, &ManageRouteSearchCommand::staticMetaObject);
    Facade::instance()->registerCommand( RouteConstants::RequestReroute, &ManageRouteSearchCommand::staticMetaObject);
    Facade::instance()->registerCommand( RouteConstants::RequestRouteAsDestination, &ManageRouteSearchCommand::staticMetaObject);
    Facade::instance()->registerCommand( RouteConstants::RequestRouteByProfileChange, &ManageRouteSearchCommand::staticMetaObject);
    Facade::instance()->registerCommand( RouteConstants::RequestSkipViaReroute, &ManageRouteSearchCommand::staticMetaObject);
    Facade::instance()->registerCommand( RouteConstants::RemoveRouteManagerItem, &ManageRouteSearchCommand::staticMetaObject);
    Facade::instance()->registerCommand( RouteConstants::MoveRouteManagerItem, &ManageRouteSearchCommand::staticMetaObject);
    Facade::instance()->registerCommand( RouteConstants::RequestViaInfo, &ManageRouteSearchCommand::staticMetaObject);
    Facade::instance()->registerCommand( RouteConstants::ShowViaMarker, &ManageRouteSearchCommand::staticMetaObject);
    Facade::instance()->registerCommand( RouteConstants::ClearViaMarker, &ManageRouteSearchCommand::staticMetaObject);
    Facade::instance()->registerCommand( RouteConstants::BackupViaData, &ManageRouteSearchCommand::staticMetaObject);
    Facade::instance()->registerCommand( RouteConstants::RestoreViaData, &ManageRouteSearchCommand::staticMetaObject);
    Facade::instance()->registerCommand( RouteConstants::PrepareRouteCandidate, &ManageRouteSearchCommand::staticMetaObject);
    Facade::instance()->registerCommand( RouteConstants::SaveVia, &ManageRouteSearchCommand::staticMetaObject);
    Facade::instance()->registerCommand( RouteConstants::ClearVia, &ManageRouteSearchCommand::staticMetaObject);
    Facade::instance()->registerCommand( RouteConstants::CheckViaValidate, &ManageRouteSearchCommand::staticMetaObject);
    Facade::instance()->registerCommand( RouteConstants::PrimaryNddsServerUpdated, &ManageRouteSearchCommand::staticMetaObject);

    Facade::instance()->registerCommand( RgConstants::StartRouteGuidance, &ManageRouteGuidanceCommand::staticMetaObject);
    Facade::instance()->registerCommand( RgConstants::StartRouteGuidanceContinuously, &ManageRouteGuidanceCommand::staticMetaObject);
    Facade::instance()->registerCommand( RgConstants::StartRouteGuidanceByVr, &ManageRouteGuidanceCommand::staticMetaObject);
    Facade::instance()->registerCommand( RgConstants::StopRouteGuidance, &ManageRouteGuidanceCommand::staticMetaObject);
    Facade::instance()->registerCommand( RgConstants::TerminatedRouteGuidance, &ManageRouteGuidanceCommand::staticMetaObject);
    Facade::instance()->registerCommand( RgConstants::RequestRouteGuidanceState, &ManageRouteGuidanceCommand::staticMetaObject);
    Facade::instance()->registerCommand( RgConstants::RequestTrafficGuidance, &ManageRouteGuidanceCommand::staticMetaObject);
    Facade::instance()->registerCommand( ZoneConstants::UpdateSoftkeyRouteGuide, &ManageRouteGuidanceCommand::staticMetaObject);
    Facade::instance()->registerCommand( RgConstants::UpdateRouteGuidance, &ManageRouteGuidanceCommand::staticMetaObject);
    Facade::instance()->registerCommand( RgConstants::RequestGetGasStationInfo, &ManageRouteGuidanceCommand::staticMetaObject);
    Facade::instance()->registerCommand( RgConstants::RequestGetEStationInfo, &ManageRouteGuidanceCommand::staticMetaObject);
    Facade::instance()->registerCommand( RgConstants::RequestIsSoundPlaying, &ManageRouteGuidanceCommand::staticMetaObject);
    Facade::instance()->registerCommand( RgConstants::ForcePeriodicReroute, &ManageRouteGuidanceCommand::staticMetaObject);
    Facade::instance()->registerCommand( RgConstants::ChangeRouteGuideBindState, &ManageRouteGuidanceCommand::staticMetaObject);
    Facade::instance()->registerCommand( RgConstants::BlockReroute, &ManageRouteGuidanceCommand::staticMetaObject);
    Facade::instance()->registerCommand( RgConstants::RegistRecentDestination, &ManageRouteGuidanceCommand::staticMetaObject);
    Facade::instance()->registerCommand( RgConstants::RequestCurrentSpeed, &ManageRouteGuidanceCommand::staticMetaObject);
    Facade::instance()->registerCommand( RgConstants::EnableSimulatorStopBySpeed, &ManageRouteGuidanceCommand::staticMetaObject);
    Facade::instance()->registerCommand( RgConstants::UpdateRouteGuidanceState, &ManageRouteGuidanceCommand::staticMetaObject);
    Facade::instance()->registerCommand( RgConstants::RequestMovingDistance, &ManageRouteGuidanceCommand::staticMetaObject);
    Facade::instance()->registerCommand( ZoneConstants::UpdateSoftkeyAddress, &ManageRouteGuidanceCommand::staticMetaObject);

    Facade::instance()->registerCommand( SearchConstants::RequestSearch, &ManageIntegratedSearchCommand::staticMetaObject);
    Facade::instance()->registerCommand( SearchConstants::RequestCacheSearch, &ManageIntegratedSearchCommand::staticMetaObject);
    Facade::instance()->registerCommand( NavigationControllerConstants::ApplyController, &MangeNavigationControllerCommand::staticMetaObject);
    Facade::instance()->registerCommand( NavigationControllerConstants::RequestController, &MangeNavigationControllerCommand::staticMetaObject);
    Facade::instance()->registerCommand( NavigationPropertyConstants::setNavigationProperty, &ManageNavigationPropertyCommand::staticMetaObject);
    Facade::instance()->registerCommand( NavigationControllerConstants::ChangeOnlineStatus, &ManageRouteSearchCommand::staticMetaObject);
    Facade::instance()->registerCommand( NavigationControllerConstants::QueryOnlineStatus, &MangeNavigationControllerCommand::staticMetaObject);
    Facade::instance()->registerCommand( NavigationControllerConstants::RequestSkyState, &MangeNavigationControllerCommand::staticMetaObject);
    Facade::instance()->registerCommand( NavigationControllerConstants::UpdatedUserProfile, &MangeNavigationControllerCommand::staticMetaObject);
    Facade::instance()->registerCommand( NavigationControllerConstants::ChangedUserProfile, &MangeNavigationControllerCommand::staticMetaObject);
    Facade::instance()->registerCommand( NavigationControllerConstants::ModifiedUserData, &MangeNavigationControllerCommand::staticMetaObject);
    Facade::instance()->registerCommand( NavigationControllerConstants::ShowSettingVolume, &MangeNavigationControllerCommand::staticMetaObject);
    Facade::instance()->registerCommand( NavigationControllerConstants::SpeakPhreaseSample, &MangeNavigationControllerCommand::staticMetaObject);
    Facade::instance()->registerCommand( NavigationControllerConstants::CallZoneApi, &MangeNavigationControllerCommand::staticMetaObject);    
    Facade::instance()->registerCommand( NavigationControllerConstants::StartMapInClusterRenderer, &MangeNavigationControllerCommand::staticMetaObject);
    Facade::instance()->registerCommand( NavigationControllerConstants::ShowGadget, &MangeNavigationControllerCommand::staticMetaObject);
    Facade::instance()->registerCommand( NavigationControllerConstants::StartGadgetRenderer, &MangeNavigationControllerCommand::staticMetaObject);
    Facade::instance()->registerCommand( NavigationControllerConstants::SaveGadgetImage, &MangeNavigationControllerCommand::staticMetaObject);
    Facade::instance()->registerCommand( NavigationControllerConstants::SendNotiGetVoiceGuidanceActive, &MangeNavigationControllerCommand::staticMetaObject);
    Facade::instance()->registerCommand( NavigationControllerConstants::SendNotiSetOverSpeedSoundAlertInfoActive, &MangeNavigationControllerCommand::staticMetaObject);
    Facade::instance()->registerCommand( NavigationControllerConstants::FileGpsSetPos, &MangeNavigationControllerCommand::staticMetaObject);
    Facade::instance()->registerCommand( NavigationControllerConstants::FileGpsRewind, &MangeNavigationControllerCommand::staticMetaObject);
    Facade::instance()->registerCommand( NavigationControllerConstants::FileGpsFastForward, &MangeNavigationControllerCommand::staticMetaObject);
    Facade::instance()->registerCommand( NavigationControllerConstants::ReplyToViewList, &MangeNavigationControllerCommand::staticMetaObject);
    Facade::instance()->registerCommand( NavigationControllerConstants::SwitchToAppIfNecessary, &MangeNavigationControllerCommand::staticMetaObject);
    Facade::instance()->registerCommand( NavigationControllerConstants::AppLostCswFocus, &MangeNavigationControllerCommand::staticMetaObject);
    Facade::instance()->registerCommand( NavigationControllerConstants::ReserveAppSwitchJob, &MangeNavigationControllerCommand::staticMetaObject);
    Facade::instance()->registerCommand( NavigationControllerConstants::ReserveSystemSwitchJob, &MangeNavigationControllerCommand::staticMetaObject);
    Facade::instance()->registerCommand( NavigationControllerConstants::RequestAroundSearch, &MangeNavigationControllerCommand::staticMetaObject);
    Facade::instance()->registerCommand( NavigationControllerConstants::RequestContactGadgetSearch, &MangeNavigationControllerCommand::staticMetaObject);
    Facade::instance()->registerCommand( NavigationControllerConstants::RequestChannelServerPolicyFinished, &MangeNavigationControllerCommand::staticMetaObject);
    Facade::instance()->registerCommand( NavigationControllerConstants::ChannelServerPolicyFinished, &MangeNavigationControllerCommand::staticMetaObject);
    Facade::instance()->registerCommand( MapConstants::GlobalMapViewStartedToRender, &MangeNavigationControllerCommand::staticMetaObject);
    Facade::instance()->registerCommand( ZoneConstants::CswBackAccepted, &MangeNavigationControllerCommand::staticMetaObject);
    Facade::instance()->registerCommand( ApplicationConstants::ChangedSession, &MangeNavigationControllerCommand::staticMetaObject);
    Facade::instance()->registerCommand( ApplicationConstants::UserClickOTP, &MangeNavigationControllerCommand::staticMetaObject);

    Facade::instance()->registerCommand( RgConstants::ChangedUiState, &MangeNavigationControllerCommand::staticMetaObject);

    Facade::instance()->registerCommand( SearchConstants::RequestRecentDestination, &ManageRecentDestinationCommand::staticMetaObject);
    Facade::instance()->registerCommand( SearchConstants::AddRecentDestination, &ManageRecentDestinationCommand::staticMetaObject);
    Facade::instance()->registerCommand( SearchConstants::DeleteRecentDestination, &ManageRecentDestinationCommand::staticMetaObject);
    Facade::instance()->registerCommand( SearchConstants::ClearRecentDestination, &ManageRecentDestinationCommand::staticMetaObject);
    Facade::instance()->registerCommand( SearchConstants::UpdateRecentDestinationScore, &ManageRecentDestinationCommand::staticMetaObject);
    Facade::instance()->registerCommand( RouteConstants::AddSearchMarker, &ManageVsmMapCommand::staticMetaObject);
    Facade::instance()->registerCommand( RouteConstants::RemoveSearchMarker, &ManageVsmMapCommand::staticMetaObject);
    Facade::instance()->registerCommand( MapConstants::ChangeViewLevelUsingApi, &ManageVsmMapCommand::staticMetaObject);
    Facade::instance()->registerCommand( MapConstants::ChangeAutoZoom, &ManageVsmMapCommand::staticMetaObject);
    Facade::instance()->registerCommand( PreferenceConstants::AddPreferences, &ManagePreferenceCommand::staticMetaObject);
    Facade::instance()->registerCommand( PreferenceConstants::RemovePreferences, &ManagePreferenceCommand::staticMetaObject);
    Facade::instance()->registerCommand( PreferenceConstants::RequestPreference, &ManagePreferenceCommand::staticMetaObject);
    Facade::instance()->registerCommand( PreferenceConstants::SavePreference, &ManagePreferenceCommand::staticMetaObject);
    Facade::instance()->registerCommand( PreferenceConstants::ChangedPreference, &ManagePreferenceCommand::staticMetaObject);
    Facade::instance()->registerCommand( SearchConstants::RequestRecentSearch, &ManageRecentSearchCommand::staticMetaObject);
    Facade::instance()->registerCommand( SearchConstants::AddRecentSearch, &ManageRecentSearchCommand::staticMetaObject);
    Facade::instance()->registerCommand( SearchConstants::DeleteRecentSearch, &ManageRecentSearchCommand::staticMetaObject);
    Facade::instance()->registerCommand( SearchConstants::ClearRecentSearch, &ManageRecentSearchCommand::staticMetaObject);
    Facade::instance()->registerCommand( SearchConstants::RequestFavorites, &ManageFavoriteCommand::staticMetaObject);
    Facade::instance()->registerCommand( SearchConstants::AddFavorite, &ManageFavoriteCommand::staticMetaObject);
    Facade::instance()->registerCommand( SearchConstants::DeleteFavorite, &ManageFavoriteCommand::staticMetaObject);
    Facade::instance()->registerCommand( SearchConstants::ClearFavorites, &ManageFavoriteCommand::staticMetaObject);
    Facade::instance()->registerCommand( SearchConstants::RequestHomeFavorite, &ManageFavoriteCommand::staticMetaObject);
    Facade::instance()->registerCommand( SearchConstants::RequestOfficeFavorite, &ManageFavoriteCommand::staticMetaObject);
    Facade::instance()->registerCommand( SearchConstants::UpdateFavorite, &ManageFavoriteCommand::staticMetaObject);
    Facade::instance()->registerCommand( SearchConstants::QueryIsFavorite, &ManageFavoriteCommand::staticMetaObject);
    Facade::instance()->registerCommand( SearchConstants::AddItemFailure, &ManageFavoriteCommand::staticMetaObject);
    Facade::instance()->registerCommand( SearchConstants::AddItemOk, &ManageFavoriteCommand::staticMetaObject);
    Facade::instance()->registerCommand( SearchConstants::SetGadgetSearchMode, &ManageIntegratedSearchCommand::staticMetaObject);
    Facade::instance()->registerCommand( SearchConstants::RequestGadgetSearchMode, &ManageIntegratedSearchCommand::staticMetaObject);
    Facade::instance()->registerCommand( SearchConstants::UpdateGpsTime, &ManageIntegratedSearchCommand::staticMetaObject);
    Facade::instance()->registerCommand( SearchConstants::RequestGpsTime, &ManageIntegratedSearchCommand::staticMetaObject);
    Facade::instance()->registerCommand( RouteConstants::RequestNDDS, &ManageRouteSearchCommand::staticMetaObject);
    Facade::instance()->registerCommand( RgConstants::ChangeRouteOption, &ManageRouteSearchCommand::staticMetaObject);
    Facade::instance()->registerCommand( SearchConstants::RequestRouteOption, &ManageRouteSearchCommand::staticMetaObject);
    Facade::instance()->registerCommand( NavigationUpdateManagerConstants::Apply, &MangeNavigationUpdateManagerCommand::staticMetaObject);
    Facade::instance()->registerCommand( NavigationUpdateManagerConstants::Request, &MangeNavigationUpdateManagerCommand::staticMetaObject);
    Facade::instance()->registerCommand( NavigationUpdateManagerConstants::Get, &MangeNavigationUpdateManagerCommand::staticMetaObject);

    // 인터페이스 연동(VR)
    Facade::instance()->registerCommand( InterfaceConstants::ShowDeleteWaypointListScreen, &ManageInterfaceCommand::staticMetaObject);
    Facade::instance()->registerCommand( InterfaceConstants::InsertWaypoint, &ManageInterfaceCommand::staticMetaObject);
    Facade::instance()->registerCommand( InterfaceConstants::InsertWaypointFromSelectInMap, &ManageInterfaceCommand::staticMetaObject);
    Facade::instance()->registerCommand( InterfaceConstants::ReplaceWaypoint, &ManageInterfaceCommand::staticMetaObject);
    Facade::instance()->registerCommand( InterfaceConstants::ApplyWaypointListChange, &ManageInterfaceCommand::staticMetaObject);

    // application mediator 등록

    Facade::instance()->registerMediator( t_new_shared<ApplicationMediator>( appView, qmlEngine, hasMapData, gadgetSignal ) );
    QMetaObject::invokeMethod( appView, "viewMediated" );

    // launch navigation bookmark agent
    Facade::instance()->registerMediator( t_new_shared<NavibookmarkAgentMediator>());

    Facade::instance()->registerMediator( t_new_shared<AddressMediator>(addressManager));
    Facade::instance()->registerMediator( t_new_shared<CSWMediator>(nullptr));
    Facade::instance()->registerMediator( t_new_shared<SystemPopupMediator>());
    Facade::instance()->registerMediator( t_new_shared<RouteSearchMediator>());
    Facade::instance()->registerMediator( t_new_shared<FavoriteMediator>());
    Facade::instance()->registerMediator( t_new_shared<SkyStateMediator>());

    // 프로그램 시작 시 DB로 부터 설정값을 가져온다.
    auto navigationControllerProxy = Facade::instance()->retrieveProxy<NavigationControllerProxy>();
    navigationControllerProxy->requestSetting();

    auto iniPath = QDir::homePath() + QChar('/') + QLatin1String("tmap_rsm_ui_dbg.ini");
    QFile file( iniPath );
    if (file.exists()) {
        QSettings settings( iniPath, QSettings::IniFormat );
        auto lang = settings.value(QStringLiteral("lang")).toString();
        if (lang == QLatin1String("en")) {
            QVariantMap body;
            body.insert( CommonConstants::Type, NavigationControllerProtocol::ChangeLanguage );
            body.insert( LangConstants::Lang, LangConstants::En );
            sendNotification( NavigationControllerConstants::ApplyController, body );

        }
    }

    // navigation properties there needs to be synchronized
//    sendNotification( SearchConstants::RequestFavorites, QVariantMap{{"service.sds",true}});
//    sendNotification( SearchConstants::RequestRecentDestination, QVariantMap{{"service.sds",true}});
//    sendNotification( SearchConstants::RequestHomeFavorite, QVariantMap{{"service.sds",true}});
//    sendNotification( SearchConstants::RequestOfficeFavorite, QVariantMap{ {"service.sds",true}});

    return true;
}
