################################################################################
# qt & c++ option
################################################################################

CONFIG(debug, debug|release){
    QT += core core-private qml qml-private quick widgets sql network dbus
} else {
    QT += core qml quick widgets sql network dbus
}

CONFIG += c++11
CONFIG += plugin
QMAKE_CXXFLAGS += "-Wno-ignored-qualifiers -Wno-unused-parameter -Wno-unused-variable"

################################################################################
# project root directory
################################################################################

PROJECT_ROOT = .

################################################################################
# build type
################################################################################

#CONFIG += debug_and_release
CONFIG( debug, debug|release ) {
    BUILD = debug
    #DEFINES += QT_NO_DEBUG_OUTPUT
}
else {
    BUILD = release
    DEFINES += QT_NO_DEBUG
    DEFINES += QT_NO_DEBUG_OUTPUT
}

################################################################################
# platform
################################################################################

win32 {
    PLATFORM = win32
} else: linux-gen3-g++ {
    PLATFORM = linux-gen3
} else: qnx {
    PLATFORM = qnx
} else: unix {
    PLATFORM = linux
}

################################################################################
# output
################################################################################

PROJECT = so
TEMPLATE = lib
DEFINES += TMAP_RSM_UI_DLL_LIBRARY
DEFINES += TMAP_RSM_UI_LINUX
#DEFINES += TMAP_RSM_UI_ENABLE_MVC_LOG
#DEFINES +=  TMAP_RSM_UI_ENABLE_DB_MIGRATION // 2018.3.28 migration season end! (it gets effect from test_180329 & v0.18.131)
#DEFINES += HIDE_CLUSTER_POI
#DEFINES += TEST_GADGET_RG
#DEFINES += KEEP_LAST_GADGET
TARGET = tmap_rsm_ui


## platform defines
linux-gen3-g++ {
   DEFINES += PLATFORM_GEN3
   DEFINES += BUILD_TARGET
} else:win32-g++ {
#    DEFINES += FOR_TEST
} else {
#    DEFINES += FOR_TEST
}


# VSM SDK
VSMSDKPATH = ../vsmsdk
INCLUDEPATH += $$VSMSDKPATH/build/linux_vsm_sdk/Include
LIBS += -L../vsmsdk/build/qtcreator_vsm_sdk/build -lvsmsdk

RSM2018_PREFIX = $$(RSM2018_PREFIX)
isEmpty(RSM2018_PREFIX) {
    win32 {
        RSM2018_PREFIX = $$(HOMEDRIVE)$$(HOMEPATH)
    } else {
        RSM2018_PREFIX = $$(HOME)
    }
}
RSM2018_HOME = $${RSM2018_PREFIX}/tmap_rsm2018/static
RSM2018_RESDIR = $${RSM2018_HOME}/res
ABSOLUTE_RESDIR=$$PWD/res
export(RSM2018_RESDIR)

RSM2018_800x480_RESDIR = $${RSM2018_HOME}/res/drawable-800x480
ABSOLUTE_800x480_RESDIR=$$PWD/res/drawable-800x480
export(RSM2018_800x480_RESDIR)

RSM2018_TARGET_PREFIX = $$(RSM2018_TARGET_PREFIX)

# MNS SDK
MNSSDKPath = ../mnssdk
INCLUDEPATH += \
    $$MNSSDKPath/include \
    ../ndds/interface/NddsInterface

#DEFINES += TMAP_MNS_TEST_RP

PRECOMPILED_HEADER = pch.h
CONFIG += precompile_header


################################################################################
# source
################################################################################

INCLUDEPATH += framework
INCLUDEPATH += framework/core
INCLUDEPATH += framework/mvc
INCLUDEPATH += framework/mvc/api
INCLUDEPATH += framework/mvc/impl/core
INCLUDEPATH += framework/mvc/impl/patterns
INCLUDEPATH += framework/mvc/impl/patterns/command
INCLUDEPATH += framework/mvc/impl/patterns/facade
INCLUDEPATH += framework/mvc/impl/patterns/mediator
INCLUDEPATH += framework/mvc/impl/patterns/observer
INCLUDEPATH += framework/mvc/impl/patterns/proxy
INCLUDEPATH += framework/orm
INCLUDEPATH += util
INCLUDEPATH += ../util
INCLUDEPATH += business
INCLUDEPATH += business/controller/command
INCLUDEPATH += business/controller/mediator
INCLUDEPATH += business/view
INCLUDEPATH += business/util

INCLUDEPATH += domain

INCLUDEPATH += ../common
INCLUDEPATH += ../test
INCLUDEPATH += "$$PROJECT_ROOT"

## Platform includes
linux-gen3-g++ {
    INCLUDEPATH += ../platforms/linux-gen3
    INCLUDEPATH += ../platforms/linux-gen3/announcement
    INCLUDEPATH += ../platforms/linux-gen3/interface
    INCLUDEPATH += ../platforms/linux-gen3/interface/guicontrol
    INCLUDEPATH += ../platforms/linux-gen3/interface/navigationext
    INCLUDEPATH += ../platforms/linux-gen3/interface/server/navigation
    INCLUDEPATH += ../platforms/linux-gen3/interface/HMIZone13
    INCLUDEPATH += ../platforms/linux-gen3/interface/extAgent
    INCLUDEPATH += ../platforms/linux-gen3/interface/deadreckoning
} else:win32-g++ {
} else {
}


HEADERS += \
    $$PWD/pch.h \
    $$PWD/tmap_rsm_ui_export.h \
    $$PWD/tmap_rsm_ui.h \
    $$PWD/framework/core/TApplication.h \
    $$PWD/framework/core/TContext.h \
    $$PWD/framework/core/TCore.h \
    $$PWD/framework/core/TEvent.h \
    $$PWD/framework/core/TLogger.h \
    $$PWD/framework/core/TMutex.h \
    $$PWD/framework/core/TObjectUtils.h \
    $$PWD/framework/core/TRandomUtils.h \
    $$PWD/framework/core/TSettings.h \
    $$PWD/framework/core/TState.h \
    $$PWD/framework/core/TStateEvent.h \
    $$PWD/framework/core/TStateMachine.h \
    $$PWD/framework/core/TStateTransition.h \
    $$PWD/framework/core/TStopwatch.h \
    $$PWD/framework/core/TStringUtils.h \
    $$PWD/framework/core/TTypeSafeEnum.h \
    $$PWD/framework/core/TJsonUtils.h \
    $$PWD/framework/orm/TDao.h \
    $$PWD/framework/orm/TDeleteBuilder.h \
    $$PWD/framework/orm/TFieldInfo.h \
    $$PWD/framework/orm/TInsertBuilder.h \
    $$PWD/framework/orm/TQuery.h \
    $$PWD/framework/orm/TSelectBuilder.h \
    $$PWD/framework/orm/TStatementBuilder.h \
    $$PWD/framework/orm/TTableInfo.h \
    $$PWD/framework/orm/TUpdateBuilder.h \
    $$PWD/framework/orm/IDriver.h \
    $$PWD/framework/mvc/api/IFacade.h \
    $$PWD/framework/mvc/api/ICommand.h \
    $$PWD/framework/mvc/api/IController.h \
    $$PWD/framework/mvc/api/IMediator.h \
    $$PWD/framework/mvc/api/IModel.h \
    $$PWD/framework/mvc/api/INotification.h \
    $$PWD/framework/mvc/api/INotifier.h \
    $$PWD/framework/mvc/api/IObserver.h \
    $$PWD/framework/mvc/api/IProxy.h \
    $$PWD/framework/mvc/api/IView.h \
    $$PWD/framework/mvc/impl/patterns/command/TMacroCommand.h \
    $$PWD/framework/mvc/impl/patterns/command/TSimpleCommand.h \
    $$PWD/framework/mvc/impl/patterns/facade/TFacade.h \
    $$PWD/framework/mvc/impl/patterns/mediator/TMediator.h \
    $$PWD/framework/mvc/impl/patterns/observer/TNotification.h \
    $$PWD/framework/mvc/impl/patterns/observer/TObserver.h \
    $$PWD/framework/mvc/impl/patterns/proxy/TProxy.h \
    $$PWD/framework/mvc/impl/core/TController.h \
    $$PWD/framework/mvc/impl/core/TModel.h \
    $$PWD/framework/mvc/impl/core/TView.h \
    $$PWD/framework/orm/TOrm.h \
    $$PWD/framework/mvc/TMvc.h \
    $$PWD/util/Facade.h \
    $$PWD/util/Ring.h \
    $$PWD/util/ImageProvider.h \
    $$PWD/util/ViewRegistry.h \
    $$PWD/util/CrossImageProvider.h \
    $$PWD/util/ViewTree.h \
    $$PWD/util/QmlAppHelper.h \
    $$PWD/util/ResourceResolver.h \
    $$PWD/util/DatabaseHelper.h \
    $$PWD/util/StringHelper.h \
    $$PWD/util/Csw.h \
    $$PWD/util/CswManager.h \
    $$PWD/util/PlatformHelper.h \
    $$PWD/Constants.h \
    $$PWD/business/RgConstants.h \
    $$PWD/business/RouteConstants.h \
    $$PWD/business/SearchConstants.h \
    $$PWD/business/NavibookmarkConstants.h \
    $$PWD/business/MapConstants.h \
    $$PWD/business/ZoneConstants.h \
    $$PWD/business/SimulatorConstants.h \
    $$PWD/business/SettingConstants.h \
    $$PWD/business/KeyboardConstants.h \
    $$PWD/business/PreferenceConstants.h \
    $$PWD/business/ToastsConstants.h \
    $$PWD/business/SoundConstants.h \
    $$PWD/business/NavigationControllerConstants.h \
    $$PWD/business/TestConstants.h \
    $$PWD/business/AddressConstants.h \
    $$PWD/domain/SettingProxy.h \
    $$PWD/domain/SettingDto.h \
    $$PWD/domain/RouteSummaryItemVo.h \
    $$PWD/domain/RouteSummaryVo.h \
    $$PWD/domain/VsmMapProxy.h \
    $$PWD/domain/VsmNaviProxy.h \
    $$PWD/domain/RouteSearchProxy.h \
    $$PWD/domain/RouteSummaryItemListModel.h \
    $$PWD/domain/RouteCandidateListModel.h \
    $$PWD/domain/RgDataVo.h \
    $$PWD/domain/RequestCommand.h \
    $$PWD/domain/SearchHelper.h \
    $$PWD/domain/RecentDestinationDto.h \
    $$PWD/domain/NavigationControllerProxy.h \
    $$PWD/domain/PreferenceProxy.h \
    $$PWD/domain/PreferenceVo.h \
    $$PWD/domain/RecentSearchDto.h \
    $$PWD/domain/NavibookmarkAgentProxy.h \
    $$PWD/domain/FavoriteDto.h \
    $$PWD/domain/LastRouteDto.h \
    $$PWD/domain/NavibookmarkProtocol.h \
    $$PWD/domain/NavigationControllerProtocol.h \
    $$PWD/domain/RouteManager.h \
    $$PWD/domain/RpnSearchHelper.h \
    $$PWD/domain/IntegratedSearchProxy.h \
    $$PWD/domain/AddressSearchResultVo.h \
    $$PWD/domain/RouteGuidanceProxy.h \
    $$PWD/domain/AbstractProxy.h \
    $$PWD/domain/RouteSearchRequest.h \
    $$PWD/domain/RgProtocol.h \
    $$PWD/business/controller/command/SimpleCommand.h \
    $$PWD/business/controller/command/StartupCommand.h \
    $$PWD/business/controller/command/MediateViewCommand.h \
    $$PWD/business/controller/command/ManageVsmMapCommand.h \
    $$PWD/business/controller/command/ManageRouteSearchCommand.h \
    $$PWD/business/controller/command/ManageIntegratedSearchCommand.h \
    $$PWD/business/controller/command/ManageRecentDestinationCommand.h \
    $$PWD/business/controller/command/ManageNavigationControllerCommand.h \
    $$PWD/business/controller/command/ManagePreferenceCommand.h \
    $$PWD/business/controller/command/ManageRecentSearchCommand.h \
    $$PWD/business/controller/command/ManageFavoriteCommand.h \
    $$PWD/business/controller/command/ManageSimulatorCommand.h \
    $$PWD/business/controller/command/ManageVsmNaviCommand.h \
    $$PWD/business/controller/command/ManageRouteGuidanceCommand.h \
    $$PWD/business/controller/command/ManageSettingCommand.h \
    $$PWD/business/controller/command/ManageTestCommand.h \
    $$PWD/business/view/mediator/AbstractViewMediator.h \
    $$PWD/business/view/mediator/AbstractMediator.h \
    $$PWD/business/view/mediator/map/SimulationViewMediator.h \
    $$PWD/business/view/mediator/app/ApplicationMediator.h \
    $$PWD/business/controller/mediator/AddressMediator.h \
    $$PWD/business/controller/mediator/NavibookmarkAgentMediator.h \
    $$PWD/business/view/mediator/map/MapViewMediator.h \
    $$PWD/business/view/mediator/search/RecentTileGridViewMediator.h \
    $$PWD/business/view/mediator/dbg/TestViewMediator.h \
    $$PWD/business/view/mediator/route/RouteSummaryViewMediator.h \
    $$PWD/business/view/mediator/route/RouteCandidateViewMediator.h \
    $$PWD/business/view/mediator/rg/TbtViewMediator.h \
    $$PWD/business/view/mediator/rg/LaneViewMediator.h \
    $$PWD/business/view/mediator/map/MapModeOptionViewMediator.h \
    $$PWD/business/view/mediator/setting/SettingAttentionSectionViewMediator.h \
    $$PWD/business/view/mediator/setting/SettingCameraWarningSoundViewMediator.h \
    $$PWD/business/view/mediator/setting/SettingCarIconViewMediator.h \
    $$PWD/business/view/mediator/setting/SettingGpsInfoViewMediator.h \
    $$PWD/business/view/mediator/setting/SettingGuidViewMediator.h \
    $$PWD/business/view/mediator/setting/SettingGuidVoiceViewMediator.h \
    $$PWD/business/view/mediator/setting/SettingMainViewMediator.h \
    $$PWD/business/view/mediator/setting/SettingMapCharSizeViewMediator.h \
    $$PWD/business/view/mediator/setting/SettingMapUpdateViewMediator.h \
    $$PWD/business/view/mediator/setting/SettingMapViewMediator.h \
    $$PWD/business/view/mediator/setting/SettingNaviInfoViewMediator.h \
    $$PWD/business/view/mediator/setting/SettingRouteAlternateViewMediator.h \
    $$PWD/business/view/mediator/setting/SettingRouteSearchViewMediator.h \
    $$PWD/business/view/mediator/setting/SettingSpeedResponseMapViewMediator.h \
    $$PWD/business/view/mediator/rg/SdiViewMediator.h \
    $$PWD/business/view/mediator/search/BookmarkPageViewMediator.h \
    $$PWD/business/view/mediator/search/RecentDestinationPageViewMediator.h \
    $$PWD/business/view/mediator/search/RecentSearchEditViewMediator.h \
    $$PWD/business/view/mediator/search/RecentSearchPageViewMediator.h \
    $$PWD/business/view/mediator/search/SearchResultViewMediator.h \
    $$PWD/business/view/mediator/search/SearchTabViewMediator.h \
    $$PWD/business/view/mediator/search/TextSearchViewMediator.h \
    $$PWD/business/view/mediator/search/SearchResultDetailViewMediator.h \
    $$PWD/business/view/mediator/route/RouteSettingViewMediator.h \
    $$PWD/business/view/mediator/route/RouteRegistViewMediator.h \
    $$PWD/business/view/mediator/search/MapSearchViewMediator.h \
    $$PWD/business/view/mediator/search/AddressSearchViewMediator.h \
    $$PWD/business/view/mediator/search/SearchAroundViewMediator.h \
    $$PWD/business/view/mediator/menu/NaviMenuViewMediator.h \
    $$PWD/business/view/mediator/map/MapControlViewMediator.h \
    $$PWD/business/view/mediator/keyboard/KeyboardViewMediator.h \
    $$PWD/business/view/mediator/zone/Zone1ViewMediator.h \
    $$PWD/business/view/mediator/zone/Zone3ViewMediator.h \
    $$PWD/business/view/mediator/app/MapViewMode.h \
    $$PWD/business/view/mediator/zone/HardKeyViewMediator.h \
    $$PWD/business/view/mediator/rg/CrossImageViewMediator.h \
    $$PWD/business/view/mediator/dbg/ErrorViewMediator.h \
    $$PWD/business/view/mediator/search/IndustrySearchViewMediator.h \
    $$PWD/business/view/mediator/search/BookmarkRegistViewMediator.h \
    $$PWD/business/view/mediator/rg/FullscreenAlertViewMediator.h \
    $$PWD/business/view/mediator/rg/HiPassLaneViewMediator.h \
    $$PWD/business/view/mediator/search/RecentDestinationEditViewMediator.h \
    $$PWD/business/view/mediator/rg/HighwayTbtViewMediator.h \
    $$PWD/business/view/mediator/rg/SummaryTbtViewMediator.h \
    $$PWD/business/view/mediator/rg/SpeedMeterViewMediator.h \
    $$PWD/../common/PoiHelper.h \
    $$PWD/domain/VsmNavigationProtocol.h \
    $$PWD/business/controller/command/ManageNavigationPropertyCommand.h \
    $$PWD/domain/NavigationPropertyProtocol.h \
    $$PWD/domain/NavigationPropertyProxy.h \
    $$PWD/business/NavigationPropertyConstants.h \
    $$PWD/business/view/mediator/search/RegionSearchViewMediator.h \
    $$PWD/business/view/mediator/app/CswMediator.h \
    $$PWD/domain/VmsGuideInfoObserver.h \
    $$PWD/business/view/mediator/setting/SettingMobileTmapViewMediator.h \
    $$PWD/business/CommonConstants.h \
    $$PWD/business/view/mediator/util/ProgressIndicatorViewMediator.h \
    $$PWD/business/UtilConstants.h \
    $$PWD/util/TranslatorManager.h \
    $$PWD/business/LangConstants.h \
    $$PWD/business/view/mediator/rg/BusLaneViewMediator.h \
    $$PWD/business/view/mediator/route/RouteSearchMediator.h \
    $$PWD/business/view/mediator/rg/TrafficBarViewMediator.h \
    $$PWD/business/view/mediator/rg/TrafficBarGraph.h \
    $$PWD/../common/uilayout.h \
    $$PWD/util/LayoutManager.h \
    $$PWD/business/view/mediator/search/FavoriteMediator.h \
    $$PWD/business/util/BookmarkHelper.h \
    $$PWD/util/DebugLogger.h \
    $$PWD/business/view/mediator/widget/WidgetLargeViewMediator.h \
    $$PWD/business/view/mediator/widget/WidgetMediumViewMediator.h \
    $$PWD/business/view/mediator/widget/WidgetSmallViewMediator.h \
    $$PWD/business/view/mediator/widget/WidgetXlViewMediator.h \
    $$PWD/business/view/mediator/widget/WidgetXxlViewMediator.h \
    $$PWD/domain/PoiCacheDto.h \
    $$PWD/domain/PoiExtraCacheDto.h \
    $$PWD/domain/SearchCacheDto.h \
    $$PWD/domain/OfflineJobCacheProxy.h \
    $$PWD/business/view/mediator/setting/SettingOilBrandViewMediator.h \
    $$PWD/business/view/mediator/setting/SettingOilKindViewMediator.h \
    $$PWD/business/view/mediator/widget/WidgetUtils.h \
    $$PWD/domain/ProfileManager.h \
    $$PWD/business/view/mediator/rg/RgUtils.h \
    $$PWD/business/NavigationUpdateManagerConstants.h \
    $$PWD/business/controller/command/ManageNavigationUpdateManagerCommand.h \
    $$PWD/domain/NavigationUpdateManagerProxy.h \
    $$PWD/domain/NavigationUpdateManagerProtocol.h \
    $$PWD/business/WidgetConstants.h \
    $$PWD/business/ApplicationConstants.h \
    $$PWD/util/DebugUtils.h \
    $$PWD/business/view/mediator/setting/SettingOilAreaViewMediator.h \
    $$PWD/business/UserProfileConstants.h \
    $$PWD/business/view/mediator/rg/ChangeDestinationViewMediator.h \
    $$PWD/business/view/mediator/map/SkyStateMediator.h \
    $$PWD/util/DatabaseMigration.h \
    $$PWD/business/view/mediator/search/RecentDestinationViewMediator.h \
    $$PWD/business/view/mediator/search/BookmarkViewMediator.h \
    $$PWD/business/view/mediator/search/BookmarkPageEditViewMediator.h \
    $$PWD/business/util/RouteSummaryHelper.h \
    $$PWD/business/util/RouteSummaryTurnCodeHelper.h \
    $$PWD/business/view/mediator/setting/HiddenDiagSettingViewMediator.h \  
    $$PWD/business/view/mediator/setting/HiddenVsmDiagSettingViewMediator.h \
    $$PWD/business/view/mediator/setting/HiddenMenuLoginViewMediator.h \
    $$PWD/util/MarkerManager.h \
    $$PWD/business/HiddenSettingConstants.h \
    $$PWD/business/view/mediator/setting/HiddenSettingFontDpViewMediator.h \
    $$PWD/business/view/mediator/app/CommonDialogViewMediator.h \
    $$PWD/business/view/mediator/setting/HiddenBoschSettingViewMediator.h \
    $$PWD/business/view/mediator/dbg/TestGuidViewMediator.h \
    $$PWD/business/view/mediator/setting/SettingVoiceGuidTimeViewMediator.h \
    $$PWD/business/view/mediator/search/SearchResultOptionViewMediator.h \
    $$PWD/business/view/mediator/search/NoResultPageViewMediator.h \
    $$PWD/business/view/mediator/rg/PoiTabViewMediator.h \
    $$PWD/business/view/mediator/zone/ZoneApiTestViewMediator.h \
    $$PWD/business/view/mediator/dbg/MonkeyTestViewMediator.h \
    $$PWD/business/view/mediator/map/MapConfigDebugUiControl.h \
    $$PWD/business/view/mediator/setting/SelfTestViewMediator.h \
    $$PWD/business/view/mediator/dbg/TestMultiMapViewMediator.h \
    $$PWD/business/view/mediator/route/RpStatus.h \
    $$PWD/business/view/mediator/map/MapInClusterViewMediator.h \
    $$PWD/business/view/mediator/dbg/TestFontViewMediator.h \
    $$PWD/util/QmlOffscreenRenderer.h \
    $$PWD/util/MultiSenseManager.h \
    $$PWD/business/view/mediator/setting/SettingAQIdetailViewMediator.h \
    $$PWD/util/OffscreenRenderer.h \
    $$PWD/business/view/mediator/zone/OptionMenuViewMediator.h \
    $$PWD/domain/ImageStreamProvider.h \
    $$PWD/util/OffscreenRenderer.h \
    $$PWD/business/MultiSenseConstants.h \
    $$PWD/util/PeriodicRerouteTimer.h \
    $$PWD/util/RpErrorHelper.h \
    $$PWD/util/QmlAnyNetworkAccessManagerFactory.h \
    $$PWD/business/view/mediator/setting/SettingEvChargeTypeViewMediator.h \
    $$PWD/business/VsmMapContants.h \
    $$PWD/domain/VsmMapSettingAndTheme.h \
    $$PWD/util/LoggingUtils.h \
    $$PWD/business/view/mediator/map/MapInClusterSideViewMediator.h \
    $$PWD/domain/MapInClusterImageStreamProvider.h \
    $$PWD/business/util/RouteSearchResultHelper.h \
    $$PWD/domain/IconGadgetProvider.h \
    $$PWD/business/view/mediator/setting/GadgetShortcutIconViewMediator.h \
    $$PWD/../common/uiconstants.h \
    $$PWD/business/view/mediator/map/MapInClusterHudViewMediator.h \
    $$PWD/domain/GadgetImageStreamProvider.h \
    $$PWD/business/controller/command/ManageInterfaceCommand.h \
    $$PWD/business/InterfaceConstants.h \
    $$PWD/domain/InterfaceProxy.h \
    $$PWD/business/util/SoftkeyMenu.h \
    $$PWD/util/KeyboardKeyInputProxy.h \
    $$PWD/business/view/mediator/app/SystemPopupMediator.h \
    $$PWD/util/CswHistory.h \
    $$PWD/business/ClusterConstants.h \
    $$PWD/util/GadgetOffscreenRenderer.h \
    $$PWD/util/MapInClusterGmemTest.h \
    $$PWD/util/PreciseTimer.h \
    $$PWD/util/QmlMouseEventListener.h

SOURCES += \
    $$PWD/pch.cpp \
    $$PWD/tmap_rsm_ui.cpp \
    $$PWD/framework/core/TApplication.cpp \
    $$PWD/framework/core/TContext.cpp \
    $$PWD/framework/core/TCore.cpp \
    $$PWD/framework/core/TEvent.cpp \
    $$PWD/framework/core/TLogger.cpp \
    $$PWD/framework/core/TMutex.cpp \
    $$PWD/framework/core/TObjectUtils.cpp \
    $$PWD/framework/core/TRandomUtils.cpp \
    $$PWD/framework/core/TSettings.cpp \
    $$PWD/framework/core/TState.cpp \
    $$PWD/framework/core/TStateEvent.cpp \
    $$PWD/framework/core/TStateMachine.cpp \
    $$PWD/framework/core/TStateTransition.cpp \
    $$PWD/framework/core/TStopwatch.cpp \
    $$PWD/framework/core/TStringUtils.cpp \
    $$PWD/framework/orm/TDao.cpp \
    $$PWD/framework/orm/TDeleteBuilder.cpp \
    $$PWD/framework/orm/TFieldInfo.cpp \
    $$PWD/framework/orm/TInsertBuilder.cpp \
    $$PWD/framework/orm/TQuery.cpp \
    $$PWD/framework/orm/TSelectBuilder.cpp \
    $$PWD/framework/orm/TStatementBuilder.cpp \
    $$PWD/framework/orm/TTableInfo.cpp \
    $$PWD/framework/orm/TUpdateBuilder.cpp \
    $$PWD/framework/mvc/impl/patterns/command/TMacroCommand.cpp \
    $$PWD/framework/mvc/impl/patterns/command/TSimpleCommand.cpp \
    $$PWD/framework/mvc/impl/patterns/facade/TFacade.cpp \
    $$PWD/framework/mvc/impl/patterns/mediator/TMediator.cpp \
    $$PWD/framework/mvc/impl/patterns/observer/TNotification.cpp \
    $$PWD/framework/mvc/impl/patterns/observer/TObserver.cpp \
    $$PWD/framework/mvc/impl/patterns/proxy/TProxy.cpp \
    $$PWD/framework/mvc/impl/core/TController.cpp \
    $$PWD/framework/mvc/impl/core/TModel.cpp \
    $$PWD/framework/mvc/impl/core/TView.cpp \
    $$PWD/framework/orm/TOrm.cpp \
    $$PWD/framework/mvc/TMvc.cpp \
    $$PWD/framework/core/TJsonUtils.cpp \
    $$PWD/util/Facade.cpp \
    $$PWD/util/StringHelper.cpp \
    $$PWD/util/QmlAppHelper.cpp \
    $$PWD/util/ResourceResolver.cpp \
    $$PWD/util/DatabaseHelper.cpp \
    $$PWD/util/CrossImageProvider.cpp \
    $$PWD/util/ImageProvider.cpp \
    $$PWD/util/ViewRegistry.cpp \
    $$PWD/util/Ring.cpp \
    $$PWD/util/ViewTree.cpp \
    $$PWD/util/Csw.cpp \
    $$PWD/util/CswManager.cpp \
    $$PWD/util/PlatformHelper.cpp \
    $$PWD/Constants.cpp \
    $$PWD/business/PreferenceConstants.cpp \
    $$PWD/business/SettingConstants.cpp \
    $$PWD/domain/AbstractProxy.cpp \
    $$PWD/domain/SettingProxy.cpp \
    $$PWD/domain/SettingDto.cpp \
    $$PWD/domain/RouteGuidanceProxy.cpp \
    $$PWD/domain/RouteSummaryItemVo.cpp \
    $$PWD/domain/RouteSummaryVo.cpp \
    $$PWD/domain/VsmMapProxy.cpp \
    $$PWD/domain/VsmNaviProxy.cpp \
    $$PWD/domain/RouteSearchProxy.cpp \
    $$PWD/domain/RouteSummaryItemListModel.cpp \
    $$PWD/domain/RouteCandidateListModel.cpp \
    $$PWD/domain/RgDataVo.cpp \
    $$PWD/domain/RequestCommand.cpp \
    $$PWD/domain/IntegratedSearchProxy.cpp \
    $$PWD/domain/RpnSearchHelper.cpp \
    $$PWD/domain/AddressSearchResultVo.cpp \
    $$PWD/domain/RouteManager.cpp \
    $$PWD/domain/SearchHelper.cpp \
    $$PWD/domain/RouteSearchRequest.cpp \
    $$PWD/domain/RecentDestinationDto.cpp \
    $$PWD/domain/NavigationControllerProxy.cpp \
    $$PWD/domain/PreferenceProxy.cpp \
    $$PWD/domain/PreferenceVo.cpp \
    $$PWD/domain/RecentSearchDto.cpp \
    $$PWD/domain/NavibookmarkAgentProxy.cpp \
    $$PWD/domain/FavoriteDto.cpp \
    $$PWD/domain/LastRouteDto.cpp \
    $$PWD/domain/RgProtocol.cpp \
    $$PWD/business/controller/command/SimpleCommand.cpp \
    $$PWD/business/controller/command/StartupCommand.cpp \
    $$PWD/business/controller/command/MediateViewCommand.cpp \
    $$PWD/business/controller/command/ManageSettingCommand.cpp \
    $$PWD/business/controller/command/ManageTestCommand.cpp \
    $$PWD/business/controller/command/ManageSimulatorCommand.cpp \
    $$PWD/business/controller/command/ManageVsmNaviCommand.cpp \
    $$PWD/business/controller/command/ManageRouteGuidanceCommand.cpp \
    $$PWD/business/controller/command/ManageVsmMapCommand.cpp \
    $$PWD/business/controller/command/ManageRouteSearchCommand.cpp \
    $$PWD/business/controller/command/ManageFavoriteCommand.cpp \
    $$PWD/business/controller/command/ManagePreferenceCommand.cpp \
    $$PWD/business/controller/command/ManageRecentSearchCommand.cpp \
    $$PWD/business/controller/command/ManageNavigationControllerCommand.cpp \
    $$PWD/business/controller/command/ManageRecentDestinationCommand.cpp \
    $$PWD/business/controller/command/ManageIntegratedSearchCommand.cpp \
    $$PWD/business/view/mediator/AbstractViewMediator.cpp \
    $$PWD/business/view/mediator/AbstractMediator.cpp \
    $$PWD/business/view/mediator/map/SimulationViewMediator.cpp \
    $$PWD/business/view/mediator/app/ApplicationMediator.cpp \
    $$PWD/business/controller/mediator/AddressMediator.cpp \
    $$PWD/business/controller/mediator/NavibookmarkAgentMediator.cpp \
    $$PWD/business/view/mediator/map/MapViewMediator.cpp \
    $$PWD/business/view/mediator/search/RecentTileGridViewMediator.cpp \
    $$PWD/business/view/mediator/dbg/TestViewMediator.cpp \
    $$PWD/business/view/mediator/route/RouteSummaryViewMediator.cpp \
    $$PWD/business/view/mediator/route/RouteCandidateViewMediator.cpp \
    $$PWD/business/view/mediator/rg/TbtViewMediator.cpp \
    $$PWD/business/view/mediator/rg/LaneViewMediator.cpp \
    $$PWD/business/view/mediator/map/MapModeOptionViewMediator.cpp \
    $$PWD/business/view/mediator/setting/SettingAttentionSectionViewMediator.cpp \
    $$PWD/business/view/mediator/setting/SettingCameraWarningSoundViewMediator.cpp \
    $$PWD/business/view/mediator/setting/SettingCarIconViewMediator.cpp \
    $$PWD/business/view/mediator/setting/SettingGpsInfoViewMediator.cpp \
    $$PWD/business/view/mediator/setting/SettingGuidViewMediator.cpp \
    $$PWD/business/view/mediator/setting/SettingGuidVoiceViewMediator.cpp \
    $$PWD/business/view/mediator/setting/SettingMainViewMediator.cpp \
    $$PWD/business/view/mediator/setting/SettingMapCharSizeViewMediator.cpp \
    $$PWD/business/view/mediator/setting/SettingMapUpdateViewMediator.cpp \
    $$PWD/business/view/mediator/setting/SettingMapViewMediator.cpp \
    $$PWD/business/view/mediator/setting/SettingNaviInfoViewMediator.cpp \
    $$PWD/business/view/mediator/setting/SettingRouteAlternateViewMediator.cpp \
    $$PWD/business/view/mediator/setting/SettingRouteSearchViewMediator.cpp \
    $$PWD/business/view/mediator/setting/SettingSpeedResponseMapViewMediator.cpp \
    $$PWD/business/view/mediator/rg/SdiViewMediator.cpp \
    $$PWD/business/view/mediator/search/BookmarkPageViewMediator.cpp \
    $$PWD/business/view/mediator/search/RecentDestinationPageViewMediator.cpp \
    $$PWD/business/view/mediator/search/RecentSearchEditViewMediator.cpp \
    $$PWD/business/view/mediator/search/RecentSearchPageViewMediator.cpp \
    $$PWD/business/view/mediator/search/SearchResultViewMediator.cpp \
    $$PWD/business/view/mediator/search/SearchTabViewMediator.cpp \
    $$PWD/business/view/mediator/search/TextSearchViewMediator.cpp \
    $$PWD/business/view/mediator/search/SearchResultDetailViewMediator.cpp \
    $$PWD/business/view/mediator/route/RouteSettingViewMediator.cpp \
    $$PWD/business/view/mediator/route/RouteRegistViewMediator.cpp \
    $$PWD/business/view/mediator/search/MapSearchViewMediator.cpp \
    $$PWD/business/view/mediator/search/AddressSearchViewMediator.cpp \
    $$PWD/business/view/mediator/search/SearchAroundViewMediator.cpp \
    $$PWD/business/view/mediator/menu/NaviMenuViewMediator.cpp \
    $$PWD/business/view/mediator/map/MapControlViewMediator.cpp \
    $$PWD/business/view/mediator/keyboard/KeyboardViewMediator.cpp \
    $$PWD/business/view/mediator/zone/Zone1ViewMediator.cpp \
    $$PWD/business/view/mediator/zone/Zone3ViewMediator.cpp \
    $$PWD/business/view/mediator/app/MapViewMode.cpp \
    $$PWD/business/view/mediator/zone/HardKeyViewMediator.cpp \
    $$PWD/business/view/mediator/rg/CrossImageViewMediator.cpp \
    $$PWD/business/view/mediator/dbg/ErrorViewMediator.cpp \
    $$PWD/business/view/mediator/search/IndustrySearchViewMediator.cpp \
    $$PWD/business/view/mediator/search/BookmarkRegistViewMediator.cpp \
    $$PWD/business/view/mediator/rg/FullscreenAlertViewMediator.cpp \
    $$PWD/business/view/mediator/rg/HiPassLaneViewMediator.cpp \
    $$PWD/business/view/mediator/search/RecentDestinationEditViewMediator.cpp \
    $$PWD/business/view/mediator/rg/HighwayTbtViewMediator.cpp \
    $$PWD/business/view/mediator/rg/SummaryTbtViewMediator.cpp \
    $$PWD/business/view/mediator/rg/SpeedMeterViewMediator.cpp \
    $$PWD/business/view/mediator/search/RegionSearchViewMediator.cpp \
    $$PWD/business/controller/command/ManageNavigationPropertyCommand.cpp \
    $$PWD/domain/NavigationPropertyProxy.cpp \
    $$PWD/business/SearchConstants.cpp \
    $$PWD/business/view/mediator/app/CswMediator.cpp \
    $$PWD/domain/VmsGuideInfoObserver.cpp \
    $$PWD/business/view/mediator/setting/SettingMobileTmapViewMediator.cpp \
    $$PWD/business/CommonConstants.cpp \
    $$PWD/business/view/mediator/util/ProgressIndicatorViewMediator.cpp \
    $$PWD/business/UtilConstants.cpp \
    $$PWD/business/NavigationControllerConstants.cpp \
    $$PWD/util/TranslatorManager.cpp \
    $$PWD/domain/NavigationControllerProtocol.cpp \
    $$PWD/business/LangConstants.cpp \
    $$PWD/business/MapConstants.cpp \
    $$PWD/business/view/mediator/rg/BusLaneViewMediator.cpp \
    $$PWD/business/view/mediator/route/RouteSearchMediator.cpp \
    $$PWD/business/ZoneConstants.cpp \
    $$PWD/business/view/mediator/rg/TrafficBarViewMediator.cpp \
    $$PWD/business/view/mediator/rg/TrafficBarGraph.cpp \
    $$PWD/util/LayoutManager.cpp \
    $$PWD/business/view/mediator/search/FavoriteMediator.cpp \
    $$PWD/business/util/BookmarkHelper.cpp \
    $$PWD/util/DebugLogger.cpp \
    $$PWD/business/view/mediator/widget/WidgetLargeViewMediator.cpp \
    $$PWD/business/view/mediator/widget/WidgetMediumViewMediator.cpp \
    $$PWD/business/view/mediator/widget/WidgetSmallViewMediator.cpp \
    $$PWD/business/view/mediator/widget/WidgetXlViewMediator.cpp \
    $$PWD/business/view/mediator/widget/WidgetXxlViewMediator.cpp \
    $$PWD/domain/PoiCacheDto.cpp \
    $$PWD/domain/PoiExtraCacheDto.cpp \
    $$PWD/domain/SearchCacheDto.cpp \
    $$PWD/domain/OfflineJobCacheProxy.cpp \
    $$PWD/business/view/mediator/setting/SettingOilBrandViewMediator.cpp \
    $$PWD/business/view/mediator/setting/SettingOilKindViewMediator.cpp \
    $$PWD/business/view/mediator/widget/WidgetUtils.cpp \
    $$PWD/domain/ProfileManager.cpp \
    $$PWD/business/view/mediator/rg/RgUtils.cpp \
    $$PWD/domain/NavigationUpdateManagerProxy.cpp \
    $$PWD/business/NavigationUpdateManagerConstants.cpp \
    $$PWD/business/controller/command/ManageNavigationUpdateManagerCommand.cpp \
    $$PWD/domain/NavigationUpdateManagerProtocol.cpp \
    $$PWD/business/WidgetConstants.cpp \
    $$PWD/business/ApplicationConstants.cpp \
    $$PWD/business/view/mediator/app/ApplicationMediatorViewHandler.cpp \
    $$PWD/business/view/mediator/app/ApplicationMediatorStateHandler.cpp \
    $$PWD/business/view/mediator/app/ApplicationMediatorUserProfileHandler.cpp \
    $$PWD/business/view/mediator/app/ApplicationMediatorWidgetHandler.cpp \
    $$PWD/business/view/mediator/app/ApplicationMediatorToastHandler.cpp \
    $$PWD/business/view/mediator/app/ApplicationMediatorMapViewModeHandler.cpp \
    $$PWD/business/view/mediator/app/ApplicationMediatorHiddenSettingHandler.cpp \
    $$PWD/util/DebugUtils.cpp \
    $$PWD/business/view/mediator/map/MapViewMediatorMapHandler.cpp \
    $$PWD/business/view/mediator/map/MapViewMediatorRgHandler.cpp \
    $$PWD/business/view/mediator/map/MapViewMediatorMapViewModeHandler.cpp \
    $$PWD/business/view/mediator/map/MapViewMediatorControllerHandler.cpp \
    $$PWD/business/view/mediator/map/MapViewMediatorSettingAndPreferenceHandler.cpp \
    $$PWD/business/view/mediator/map/MapViewMediatorApplicationHandler.cpp \
    $$PWD/business/view/mediator/setting/SettingOilAreaViewMediator.cpp \
    $$PWD/business/UserProfileConstants.cpp \
    $$PWD/business/view/mediator/rg/ChangeDestinationViewMediator.cpp \
    $$PWD/business/view/mediator/map/SkyStateMediator.cpp \
    $$PWD/util/DatabaseMigration.cpp \
    $$PWD/business/view/mediator/search/RecentDestinationViewMediator.cpp \
    $$PWD/business/view/mediator/search/BookmarkViewMediator.cpp \
    $$PWD/business/view/mediator/search/BookmarkPageEditViewMediator.cpp \
    $$PWD/business/util/RouteSummaryHelper.cpp \
    $$PWD/business/util/RouteSummaryTurnCodeHelper.cpp \
    $$PWD/business/view/mediator/setting/HiddenVsmDiagSettingViewMediator.cpp \
    $$PWD/business/view/mediator/setting/HiddenDiagSettingViewMediator.cpp \
    $$PWD/business/view/mediator/setting/HiddenMenuLoginViewMediator.cpp \
    $$PWD/util/MarkerManager.cpp \
    $$PWD/business/HiddenSettingConstants.cpp \
    $$PWD/business/view/mediator/setting/HiddenSettingFontDpViewMediator.cpp \
    $$PWD/business/view/mediator/app/CommonDialogViewMediator.cpp \
    $$PWD/business/view/mediator/setting/HiddenBoschSettingViewMediator.cpp \
    $$PWD/business/view/mediator/dbg/TestGuidViewMediator.cpp \
    $$PWD/business/view/mediator/setting/SettingVoiceGuidTimeViewMediator.cpp \
    $$PWD/business/view/mediator/search/SearchResultOptionViewMediator.cpp \
    $$PWD/business/TestConstants.cpp \
    $$PWD/business/view/mediator/search/NoResultPageViewMediator.cpp \
    $$PWD/business/view/mediator/rg/PoiTabViewMediator.cpp \
    $$PWD/business/view/mediator/zone/ZoneApiTestViewMediator.cpp \
    $$PWD/business/view/mediator/dbg/MonkeyTestViewMediator.cpp \
    $$PWD/business/view/mediator/map/MapConfigDebugUiControl.cpp \
    $$PWD/business/view/mediator/setting/SelfTestViewMediator.cpp \
    $$PWD/business/view/mediator/dbg/TestMultiMapViewMediator.cpp \
    $$PWD/business/view/mediator/route/RpStatus.cpp \
    $$PWD/business/view/mediator/map/MapInClusterViewMediator.cpp \
    $$PWD/business/view/mediator/dbg/TestFontViewMediator.cpp \
    $$PWD/util/QmlOffscreenRenderer.cpp \
    $$PWD/util/MultiSenseManager.cpp \
    $$PWD/business/view/mediator/setting/SettingAQIdetailViewMediator.cpp \
    $$PWD/business/view/mediator/zone/OptionMenuViewMediator.cpp \
    $$PWD/domain/ImageStreamProvider.cpp \
    $$PWD/business/view/mediator/app/ApplicationMediatorMapInClusterHandler.cpp \
    $$PWD/util/PeriodicRerouteTimer.cpp \
    $$PWD/util/RpErrorHelper.cpp \
    $$PWD/util/QmlAnyNetworkAccessManagerFactory.cpp \
    $$PWD/business/view/mediator/setting/SettingEvChargeTypeViewMediator.cpp \
    $$PWD/business/VsmMapContants.cpp \
    $$PWD/util/LoggingUtils.cpp \
    $$PWD/domain/VsmMapSettingAndTheme.cpp \
    $$PWD/business/view/mediator/map/MapInClusterSideViewMediator.cpp \
    $$PWD/domain/MapInClusterImageStreamProvider.cpp \
    $$PWD/business/util/RouteSearchResultHelper.cpp \
    $$PWD/domain/IconGadgetProvider.cpp \
    $$PWD/business/view/mediator/setting/GadgetShortcutIconViewMediator.cpp \
    $$PWD/business/view/mediator/map/MapInClusterHudViewMediator.cpp \
    $$PWD/domain/GadgetImageStreamProvider.cpp \
    $$PWD/business/controller/command/ManageInterfaceCommand.cpp \
    $$PWD/domain/InterfaceProxy.cpp \
    $$PWD/util/KeyboardKeyInputProxy.cpp \
    $$PWD/business/view/mediator/app/SystemPopupMediator.cpp \
    $$PWD/util/CswHistory.cpp \
    $$PWD/util/GadgetOffscreenRenderer.cpp \
    $$PWD/business/RgConstants.cpp \
    $$PWD/util/PreciseTimer.cpp \
    $$PWD/util/QmlMouseEventListener.cpp

################################################################################
# resource
################################################################################

DISTFILES += \
    $$PWD/tmap_rsm_ui_img.qrc \
    $$PWD/tmap_rsm_ui_qml.qrc \
    $$PWD/qml/component/internal/Toast.qml \
    $$PWD/qml/component/internal/KeyboardButton.qml \
    $$PWD/qml/component/TScrollBar.qml \
    $$PWD/qml/component/TCheckButton.qml \
    $$PWD/qml/component/TImageButton.qml \
    $$PWD/qml/component/TDialog.qml \
    $$PWD/qml/component/TFader.qml \
    $$PWD/qml/component/TFocusItem.qml \
    $$PWD/qml/component/TPopup.qml \
    $$PWD/qml/component/TImageButton.qml \
    $$PWD/qml/component/TKeyboard.qml \
    $$PWD/qml/component/TMouseArea.qml \
    $$PWD/qml/component/TPopup.qml \
    $$PWD/qml/component/TRingCountDown.qml \
    $$PWD/qml/component/TPickerButton.qml \
    $$PWD/qml/view/PopupView.qml \
    $$PWD/qml/view/View.qml \
    $$PWD/qml/view/dbg/BoschInfo.qml \
    $$PWD/qml/view/dbg/ErrorView.qml \
    $$PWD/qml/view/dbg/TestView.qml \
    $$PWD/qml/view/keyboard/KeyboardView.qml \
    $$PWD/qml/view/map/CrossHair.qml \
    $$PWD/qml/view/map/MapControlView.qml \
    $$PWD/qml/view/map/MapModeOptionView.qml \
    $$PWD/qml/view/map/MapScale.qml \
    $$PWD/qml/view/map/MapView.qml \
    $$PWD/qml/view/map/MapViewModeControl.qml \
    $$PWD/qml/view/map/SimulationControl.qml \
    $$PWD/qml/view/map/SimulationProgress.qml \
    $$PWD/qml/view/map/SimulationView.qml \
    $$PWD/qml/view/menu/NaviMenuView.qml \
    $$PWD/qml/view/rg/CrossImageView.qml \
    $$PWD/qml/view/rg/FullscreenAlertView.qml \
    $$PWD/qml/view/rg/HighwayTbtView.qml \
    $$PWD/qml/view/rg/HiPassLaneView.qml \
    $$PWD/qml/view/rg/LaneView.qml \
    $$PWD/qml/view/rg/SdiView.qml \
    $$PWD/qml/view/rg/SpeedMeterView.qml \
    $$PWD/qml/view/rg/SummaryTbtView.qml \
    $$PWD/qml/view/rg/TbtView.qml \
    $$PWD/qml/view/route/Button.qml \
    $$PWD/qml/view/route/Distance.qml \
    $$PWD/qml/view/route/DragAndDropArea.qml \
    $$PWD/qml/view/route/RouteCandidate.qml \
    $$PWD/qml/view/route/RouteCandidateArtary.qml \
    $$PWD/qml/view/route/RouteCandidateView.qml \
    $$PWD/qml/view/route/RouteListView.qml \
    $$PWD/qml/view/route/RouteRegistView.qml \
    $$PWD/qml/view/route/RouteSettingView.qml \
    $$PWD/qml/view/route/RouteSettingViewController.qml \
    $$PWD/qml/view/route/RouteSettingViewDragger.qml \
    $$PWD/qml/view/route/RouteSettingViewItem.qml \
    $$PWD/qml/view/route/RouteSummaryBar.qml \
    $$PWD/qml/view/route/RouteSummaryBarExt.qml \
    $$PWD/qml/view/route/RouteSummaryListItem.qml \
    $$PWD/qml/view/route/RouteSummaryText.qml \
    $$PWD/qml/view/route/RouteSummaryView.qml \
    $$PWD/qml/view/route/StateImage.qml \
    $$PWD/qml/view/route/Temperature.qml \
    $$PWD/qml/view/search/AddressSearchView.qml \
    $$PWD/qml/view/search/BookmarkPageView.qml \
    $$PWD/qml/view/search/BookmarkRegistView.qml \
    $$PWD/qml/view/search/IndustrySearchView.qml \
    $$PWD/qml/view/search/MapSearchView.qml \
    $$PWD/qml/view/search/MultiColumnList.qml \
    $$PWD/qml/view/search/RecentDestinationEditView.qml \
    $$PWD/qml/view/search/RecentDestinationPageView.qml \
    $$PWD/qml/view/search/RecentSearchEditView.qml \
    $$PWD/qml/view/search/RecentSearchPageView.qml \
    $$PWD/qml/view/search/RecentTileGrid.qml \
    $$PWD/qml/view/search/RecentTileGridView.qml \
    $$PWD/qml/view/search/SearchResultDetailView.qml \
    $$PWD/qml/view/search/SearchResultView.qml \
    $$PWD/qml/view/search/SearchAroundView.qml \
    $$PWD/qml/view/search/SearchTabView.qml \
    $$PWD/qml/view/search/TabBar.qml \
    $$PWD/qml/view/search/TabButton.qml \
    $$PWD/qml/view/search/TextSearchView.qml \
    $$PWD/qml/view/setting/ImageRadioButton.qml \
    $$PWD/qml/view/setting/OptionButton.qml \
    $$PWD/qml/view/setting/SettingAttentionSectionView.qml \
    $$PWD/qml/view/setting/SettingCameraWarningSoundView.qml \
    $$PWD/qml/view/setting/SettingCarIconView.qml \
    $$PWD/qml/view/setting/SettingGpsInfoView.qml \
    $$PWD/qml/view/setting/SettingGuidView.qml \
    $$PWD/qml/view/setting/SettingGuidVoiceView.qml \
    $$PWD/qml/view/setting/SettingMainView.qml \
    $$PWD/qml/view/setting/SettingMapCharSizeView.qml \
    $$PWD/qml/view/setting/SettingMapUpdateView.qml \
    $$PWD/qml/view/setting/SettingMapView.qml \
    $$PWD/qml/view/setting/SettingNaviInfoView.qml \
    $$PWD/qml/view/setting/SettingRouteAlternateView.qml \
    $$PWD/qml/view/setting/SettingRouteSearchView.qml \
    $$PWD/qml/view/setting/SettingSpeedResponseMapView.qml \
    $$PWD/qml/view/setting/TSettingGrid.qml \
    $$PWD/qml/view/setting/TSettingList.qml \
    $$PWD/qml/view/util/DebugUtils.js \
    $$PWD/qml/view/util/Formatter.js \
    $$PWD/qml/view/zone/HardKeyView.qml \
    $$PWD/qml/view/zone/IconButton.qml \
    $$PWD/qml/view/zone/SoftKey.qml \
    $$PWD/qml/view/zone/SoftkeyArrivalTime.qml \
    $$PWD/qml/view/zone/SoftKeyButtonLabel.qml \
    $$PWD/qml/view/zone/SoftkeyRemainDistance.qml \
    $$PWD/qml/view/zone/SoftkeyRemainTime.qml \
    $$PWD/qml/view/zone/StartGuideButton.qml \
    $$PWD/qml/view/zone/StatusInfomation.qml \
    $$PWD/qml/view/zone/SystemInfomationLabel.qml \
    $$PWD/qml/view/zone/Zone1View.qml \
    $$PWD/qml/view/zone/Zone3Divider.qml \
    $$PWD/qml/view/zone/Zone3NaviAddress.qml \
    $$PWD/qml/view/zone/Zone3NaviNormalDrive.qml \
    $$PWD/qml/view/zone/Zone3NaviRouteGuide.qml \
    $$PWD/qml/view/zone/Zone3Softkey.qml \
    $$PWD/qml/view/zone/Zone3SoftkeyMenu.qml \
    $$PWD/qml/view/zone/Zone3SoftkeyMenuButton.qml \
    $$PWD/qml/view/zone/Zone3SoftkeyToggleMenu.qml \
    $$PWD/qml/view/zone/Zone3View.qml \
    $$PWD/qml/view/search/TabView.qml \
    $$PWD/qml/view/search/RegionSearchView.qml \
    $$PWD/qml/GlobalConstants.js \
    $$PWD/qml/view/route/RemainTime.qml \
    $$PWD/qml/view/util/TileMenu.qml \
    $$PWD/qml/view/util/ExclusiveGroup.qml \
    $$PWD/qml/component/TPushButton.qml \
    $$PWD/qml/view/setting/SettingMobileTmapView.qml \
    $$PWD/qml/view/setting/SettingView.qml \
    $$PWD/qml/view/util/ProgressIndicatorView.qml \
    $$PWD/qml/view/rg/BusLaneView.qml \
    $$PWD/qml/view/map/MapViewLayout.qml \
    $$PWD/qml/component/internal/Keyboard.js \
    $$PWD/qml/view/rg/TrafficBarView.qml \
    $$PWD/qml/GlobalConstants_800x480.js \
    $$PWD/qml/GlobalConstants_820x1024.js \
    $$PWD/qml/view/zone/HardKeyViewLayout.qml \
    $$PWD/qml/view/zone/Zone3ViewLayout.qml \
    $$PWD/qml/view/menu/NaviMenuViewLayout.qml \
    $$PWD/qml/view/search/AddressSearchViewLayout.qml \
    $$PWD/qml/component/TRadioButton.qml \
    $$PWD/qml/view/search/AddressSearchViewLotInput.qml \
    $$PWD/qml/view/util/CommonUtils.js \
    $$PWD/qml/view/search/AddressSearchViewCategoryBar.qml \
    $$PWD/qml/view/setting/SettingList.qml \
    $$PWD/qml/view/setting/SettingGrid.qml \
    $$PWD/qml/view/rg/MarqueeText.qml \
    $$PWD/qml/view/map/GpsState.qml \
    $$PWD/qml/view/search/AddressSearchViewBldInput.qml \
    $$PWD/qml/view/widget/WidgetLargeView.qml \
    $$PWD/qml/view/widget/WidgetMediumView.qml \
    $$PWD/qml/view/widget/WidgetSmallView.qml \
    $$PWD/qml/view/widget/WidgetXlView.qml \
    $$PWD/qml/view/widget/WidgetXxlView.qml \
    $$PWD/qml/view/route/RouteCandidateViewLayout.qml \
    $$PWD/qml/view/route/RouteSummaryViewLayout.qml \
    $$PWD/qml/view/route/RouteSettingViewLayout.qml \
    $$PWD/qml/view/route/RouteRegistViewLayout.qml \
    $$PWD/qml/view/dbg/ErrorViewLayout.qml \
    $$PWD/qml/view/search/BookmarkRegistViewLayout.qml \
    $$PWD/qml/view/map/SimulationViewLayout.qml \
    $$PWD/qml/view/search/SearchAroundViewLayout.qml \
    $$PWD/qml/view/search/IndustrySearchViewLayout.qml \
    $$PWD/qml/view/setting/SettingMainViewLayout.qml \
    $$PWD/qml/view/setting/SettingSpeedResponseMapViewLayout.qml \
    $$PWD/qml/view/setting/SettingRouteSearchViewLayout.qml \
    $$PWD/qml/view/setting/SettingRouteAlternateViewLayout.qml \
    $$PWD/qml/view/setting/SettingNaviInfoViewLayout.qml \
    $$PWD/qml/view/setting/SettingMobileTmapViewLayout.qml \
    $$PWD/qml/view/setting/SettingMapUpdateViewLayout.qml \
    $$PWD/qml/view/setting/SettingMapCharSizeViewLayout.qml \
    $$PWD/qml/view/setting/SettingGuidVoiceViewLayout.qml \
    $$PWD/qml/view/setting/SettingGuidViewLayout.qml \
    $$PWD/qml/view/setting/SettingGpsInfoViewLayout.qml \
    $$PWD/qml/view/setting/SettingCarIconViewLayout.qml \
    $$PWD/qml/view/setting/SettingCameraWarningSoundViewLayout.qml \
    $$PWD/qml/view/setting/SettingAttentionSectionViewLayout.qml \
    $$PWD/qml/view/setting/SettingMapViewLayout.qml \
    $$PWD/qml/view/search/SearchResultViewLayout.qml \
    $$PWD/qml/view/search/SearchResultDetailViewLayout.qml \
    $$PWD/qml/view/search/RegionSearchViewLayout.qml \
    $$PWD/qml/view/search/MapSearchViewLayout.qml \
    $$PWD/qml/view/widget/WidgetXxlRgStatusBar.qml \
    $$PWD/qml/view/widget/WidgetXlRgStatusBar.qml \
    $$PWD/qml/view/widget/WidgetMediumArrivalTime.qml \
    $$PWD/qml/view/widget/WidgetMediumRemainDistance.qml \
    $$PWD/qml/view/widget/WidgetLargeRgStatusBar.qml \
    $$PWD/qml/view/widget/WidgetMediumRgStatusBar.qml \
    $$PWD/qml/view/widget/WidgetSmallRgStatusBar.qml \
    $$PWD/qml/view/setting/SettingOilBrandView.qml \
    $$PWD/qml/view/setting/SettingOilKindView.qml \
    $$PWD/qml/view/setting/SettingOilBrandViewLayout.qml \
    $$PWD/qml/view/setting/SettingOilKindViewLayout.qml \
    $$PWD/qml/view/widget/WidgetSmallTbt.qml \
    $$PWD/qml/view/widget/WidgetMediumTbt.qml \
    $$PWD/qml/view/widget/WidgetSdi.qml \
    $$PWD/qml/view/widget/WidgetLane.qml \
    $$PWD/qml/view/widget/WidgetTbt.qml \
    $$PWD/qml/view/search/TextSearchViewLayout.qml \
    $$PWD/qml/view/search/RecentTileGridViewLayout.qml \
    $$PWD/qml/view/search/RecentDestinationPageViewOption.qml \
    $$PWD/qml/view/map/MapControlViewLayout.qml \
    $$PWD/qml/view/map/MapScaleControl.qml \
    $$PWD/qml/view/setting/SettingOilAreaView.qml \
    $$PWD/qml/view/setting/SettingOilAreaViewLayout.qml \
    $$PWD/qml/view/rg/ChangeDestinationView.qml \
    $$PWD/qml/view/search/RecentDestinationView.qml \
    $$PWD/qml/view/search/BookmarkView.qml \
    $$PWD/qml/view/search/BookmarkPageEditView.qml \
    $$PWD/qml/GlobalConstants_1920x720.js \
    $$PWD/qml/ApplicationWindow_1920x720.qml \
    $$PWD/qml/view/zone/Zone1View_1920x720.qml \
    $$PWD/qml/view/zone/Zone1ViewLayout.qml \
    $$PWD/qml/view/zone/Zone1View_1920x720Layout.qml \
    $$PWD/qml/view/map/MapModeOptionViewLayout.qml \
    $$PWD/qml/view/setting/HiddenDiagSettingView.qml \
    $$PWD/qml/view/setting/HiddenVsmDiagSettingView.qml \
    $$PWD/qml/view/setting/HiddenMenuLoginView.qml \
    $$PWD/qml/view/setting/HiddenVsmMapAutoLevelConfig.qml \
    $$PWD/qml/view/map/GpsMapmatchTimestamp.qml \
    $$PWD/qml/view/setting/HiddenSettingFontDpView.qml \
    $$PWD/qml/view/setting/HiddenSettingFontDpViewLayout.qml \
    $$PWD/qml/view/setting/HiddenBoschSettingView.qml \
    $$PWD/qml/view/setting/SettingVoiceGuidTimeView.qml \
    $$PWD/qml/view/setting/SettingVoiceGuidTimeViewLayout.qml \
    $$PWD/qml/view/search/SearchResultOptionView.qml \
    $$PWD/qml/ApplicationWindow.qml \
    $$PWD/qml/view/search/NoResultPageView.qml \
    $$PWD/qml/view/dbg/MonkeyTestView.qml \
    $$PWD/qml/view/map/MapAutoLevelConfigLauncher.qml \
    $$PWD/qml/view/map/MapConfigDebugUi.qml \
    $$PWD/qml/view/map/MapConfigDebugUiLauncher.qml \
    $$PWD/qml/view/setting/SelfTestView.qml \
    $$PWD/qml/view/dbg/TestMultiMapView.qml \
    $$PWD/qml/view/map/MapInClusterView.qml \
    $$PWD/qml/view/CommonDialogView.qml \
    $$PWD/qml/MapInClusterRenderer.qml \
    $$PWD/qml/view/setting/SettingAQIdetailView.qml \
    $$PWD/qml/singleton/MultiSenseCoordinator.qml \
    $$PWD/qml/view/util/HighlightBar.qml \
    $$PWD/qml/view/util/PressBar.qml \
    $$PWD/qml/view/util/PressBarMouseSync.qml \
    $$PWD/qml/view/rg/PoiTabView.qml \
    $$PWD/qml/view/dbg/TestKeypad.qml \
    $$PWD/qml/component/TMarquee.qml \
    $$PWD/qml/component/TMarqueeText.qml \
    $$PWD/qml/component/TTextButton.qml \
    $$PWD/qml/view/setting/SettingEvChargeTypeView.qml \
    $$PWD/qml/view/setting/SettingEvChargeTypeViewLayout.qml \
    $$PWD/qml/view/map/MapInClusterSideView.qml \
    $$PWD/qml/view/setting/GadgetShortcutIconView.qml \
    $$PWD/qml/view/setting/GadgetShortcutIconViewLayout.qml \
    $$PWD/qml/view/map/MapInClusterHudView.qml \
    $$PWD/qml/view/util/PressBarMouseSync2.qml \
    $$PWD/qml/view/search/AddressSearchViewRoadList.qml \
    $$PWD/qml/view/search/AddressSearchViewRoadListShortcut.qml \
    $$PWD/qml/view/search/AddressSearchViewSiGunGuList.qml \
    $$PWD/qml/view/util/SelectedBar.qml \
    $$PWD/qml/view/widget/WidgetTbtForLarge.qml \
    $$PWD/qml/view/setting/CircleGraphDustDensity.qml \
    $$PWD/qml/view/setting/RectangleGraphDustDensity.qml \
    $$PWD/qml/view/setting/RectangleGraph.qml \
    $$PWD/qml/MapInClusterEmulationWindow.qml \
    $$PWD/qml/GadgetEmulationWindow.qml
################################################################################
# distribute
################################################################################

DESTDIR = "$$PROJECT_ROOT/$$BUILD"

# lang
mkpath($${RSM2018_RESDIR}/lang)
LANG_RESOURCE_FILES = $$files($${ABSOLUTE_RESDIR}/lang/*.qm)

copy_lang_res.name = copy lang resource ${QMAKE_FILE_IN}
copy_lang_res.input = LANG_RESOURCE_FILES
copy_lang_res.output = $${RSM2018_RESDIR}/lang/${QMAKE_FILE_BASE}${QMAKE_FILE_EXT}
copy_lang_res.commands = ${COPY_FILE} ${QMAKE_FILE_IN} ${QMAKE_FILE_OUT}
copy_lang_res.CONFIG += no_link target_predeps
QMAKE_EXTRA_COMPILERS += copy_lang_res

# qml
QML_IMPORT_PATH =

RCC_EXECUTABLE = $$[QT_HOST_BINS]/rcc
UI_QRC_FILE = $$PWD/tmap_rsm_ui_qml.qrc
UI_QML_FILES = $$system($${RCC_EXECUTABLE} -list $$UI_QRC_FILE)
UI_RCC_FILE = $${ABSOLUTE_RESDIR}/tmap_rsm_ui_qml.rcc

rccTarget.name = compile resource ${QMAKE_FILE_IN}
rccTarget.input = UI_QRC_FILE
rccTarget.depends = $${UI_QRC_FILE} $${UI_QML_FILES}
rccTarget.output = $${UI_RCC_FILE}
rccTarget.commands = $${RCC_EXECUTABLE} -binary -no-compress ${QMAKE_FILE_IN} -o ${QMAKE_FILE_OUT}
rccTarget.CONFIG = no_link target_predeps
QMAKE_EXTRA_COMPILERS += rccTarget

# base resources
mkpath($${RSM2018_RESDIR})
BASE_RESOURCE_FILES = \
    $${UI_RCC_FILE} \
    $$files($${ABSOLUTE_RESDIR}/*.png) \
    $$files($${ABSOLUTE_RESDIR}/*.jpg) \
    $$files($${ABSOLUTE_RESDIR}/*.otf) \
    $$files($${ABSOLUTE_RESDIR}/*.ttf)

copy_base_res.name = copy resource ${QMAKE_FILE_IN}
copy_base_res.input = BASE_RESOURCE_FILES
copy_base_res.output = $${RSM2018_RESDIR}/${QMAKE_FILE_BASE}${QMAKE_FILE_EXT}
copy_base_res.commands = ${COPY_FILE} ${QMAKE_FILE_IN} ${QMAKE_FILE_OUT}
copy_base_res.CONFIG += no_link target_predeps
QMAKE_EXTRA_COMPILERS += copy_base_res

# resources
mkpath($${RSM2018_800x480_RESDIR})
RESOURCE_FILES = \
    $$files($${ABSOLUTE_800x480_RESDIR}/*.png) \
    $$files($${ABSOLUTE_800x480_RESDIR}/*.jpg) \
    $$files($${ABSOLUTE_800x480_RESDIR}/*.otf) \
    $$files($${ABSOLUTE_800x480_RESDIR}/*.ttf)

copy_res.name = copy resource ${QMAKE_FILE_IN}
copy_res.input = RESOURCE_FILES
copy_res.output = $${RSM2018_800x480_RESDIR}/${QMAKE_FILE_BASE}${QMAKE_FILE_EXT}
copy_res.commands = ${COPY_FILE} ${QMAKE_FILE_IN} ${QMAKE_FILE_OUT}
copy_res.CONFIG += no_link target_predeps
QMAKE_EXTRA_COMPILERS += copy_res

# img resources
mkpath($${RSM2018_800x480_RESDIR}/img)
IMAGE_RESOURCE_FILES =

files = $$files($${ABSOLUTE_800x480_RESDIR}/img/*)
for (file, files) {
    exists($$file/*): next() # exclude directories
    IMAGE_RESOURCE_FILES += $${file}
}

copy_image_res.name = copy image resource ${QMAKE_FILE_IN}
copy_image_res.input = IMAGE_RESOURCE_FILES
copy_image_res.output = $${RSM2018_800x480_RESDIR}/img/${QMAKE_FILE_BASE}${QMAKE_FILE_EXT}
copy_image_res.commands = ${COPY_FILE} ${QMAKE_FILE_IN} ${QMAKE_FILE_OUT}
copy_image_res.CONFIG += no_link target_predeps
QMAKE_EXTRA_COMPILERS += copy_image_res

# img/keyboard resources
mkpath($${RSM2018_800x480_RESDIR}/img/keyboard)
KEYBOARD_RESOURCE_FILES = $$files($${ABSOLUTE_800x480_RESDIR}/img/keyboard/*)

copy_keyboard_res.name = copy keyboard resource ${QMAKE_FILE_IN}
copy_keyboard_res.input = KEYBOARD_RESOURCE_FILES
copy_keyboard_res.output = $${RSM2018_800x480_RESDIR}/img/keyboard/${QMAKE_FILE_BASE}${QMAKE_FILE_EXT}
copy_keyboard_res.commands = ${COPY_FILE} ${QMAKE_FILE_IN} ${QMAKE_FILE_OUT}
copy_keyboard_res.CONFIG += no_link target_predeps
QMAKE_EXTRA_COMPILERS += copy_keyboard_res

# img/route resources
mkpath($${RSM2018_800x480_RESDIR}/img/route)
ROUTE_RESOURCE_FILES = $$files($${ABSOLUTE_800x480_RESDIR}/img/route/*)

copy_route_res.name = copy route resource ${QMAKE_FILE_IN}
copy_route_res.input = ROUTE_RESOURCE_FILES
copy_route_res.output = $${RSM2018_800x480_RESDIR}/img/route/${QMAKE_FILE_BASE}${QMAKE_FILE_EXT}
copy_route_res.commands = ${COPY_FILE} ${QMAKE_FILE_IN} ${QMAKE_FILE_OUT}
copy_route_res.CONFIG += no_link target_predeps
QMAKE_EXTRA_COMPILERS += copy_route_res

# img/lane resources
mkpath($${RSM2018_800x480_RESDIR}/img/lane)
LANE_RESOURCE_FILES = $$files($${ABSOLUTE_800x480_RESDIR}/img/lane/*)

copy_lane_res.name = copy lane resource ${QMAKE_FILE_IN}
copy_lane_res.input = LANE_RESOURCE_FILES
copy_lane_res.output = $${RSM2018_800x480_RESDIR}/img/lane/${QMAKE_FILE_BASE}${QMAKE_FILE_EXT}
copy_lane_res.commands = ${COPY_FILE} ${QMAKE_FILE_IN} ${QMAKE_FILE_OUT}
copy_lane_res.CONFIG += no_link target_predeps
QMAKE_EXTRA_COMPILERS += copy_lane_res

# img/highway resources
mkpath($${RSM2018_800x480_RESDIR}/img/highway)
HIGHWAY_RESOURCE_FILES = $$files($${ABSOLUTE_800x480_RESDIR}/img/highway/*)

copy_highway_res.name = copy highway resource ${QMAKE_FILE_IN}
copy_highway_res.input = HIGHWAY_RESOURCE_FILES
copy_highway_res.output = $${RSM2018_800x480_RESDIR}/img/highway/${QMAKE_FILE_BASE}${QMAKE_FILE_EXT}
copy_highway_res.commands = ${COPY_DIR} ${QMAKE_FILE_IN} ${QMAKE_FILE_OUT}
copy_highway_res.CONFIG += no_link target_predeps
QMAKE_EXTRA_COMPILERS += copy_highway_res

# img/highway/online/day resources
mkpath($${RSM2018_800x480_RESDIR}/img/highway/online/day)
HIGHWAY_ONLINE_DAY_RESOURCE_FILES = $$files($${ABSOLUTE_800x480_RESDIR}/img/highway/online/day/*)

copy_highway_online_day_res.name = copy highway online day resource ${QMAKE_FILE_IN}
copy_highway_online_day_res.input = HIGHWAY_ONLINE_DAY_RESOURCE_FILES
copy_highway_online_day_res.output = $${RSM2018_800x480_RESDIR}/img/highway/online/day/${QMAKE_FILE_BASE}${QMAKE_FILE_EXT}
copy_highway_online_day_res.commands = ${COPY_FILE} ${QMAKE_FILE_IN} ${QMAKE_FILE_OUT}
copy_highway_online_day_res.CONFIG += no_link target_predeps
QMAKE_EXTRA_COMPILERS += copy_highway_online_day_res

# img/highway/online/night resources
mkpath($${RSM2018_800x480_RESDIR}/img/highway/online/night)
HIGHWAY_ONLINE_NIGHT_RESOURCE_FILES = $$files($${ABSOLUTE_800x480_RESDIR}/img/highway/online/night/*)

copy_highway_online_night_res.name = copy highway online night resource ${QMAKE_FILE_IN}
copy_highway_online_night_res.input = HIGHWAY_ONLINE_NIGHT_RESOURCE_FILES
copy_highway_online_night_res.output = $${RSM2018_800x480_RESDIR}/img/highway/online/night/${QMAKE_FILE_BASE}${QMAKE_FILE_EXT}
copy_highway_online_night_res.commands = ${COPY_FILE} ${QMAKE_FILE_IN} ${QMAKE_FILE_OUT}
copy_highway_online_night_res.CONFIG += no_link target_predeps
QMAKE_EXTRA_COMPILERS += copy_highway_online_night_res

# img/highway/offline/day resources
mkpath($${RSM2018_800x480_RESDIR}/img/highway/offline/day)
HIGHWAY_OFFLINE_DAY_RESOURCE_FILES = $$files($${ABSOLUTE_800x480_RESDIR}/img/highway/offline/day/*)

copy_highway_offline_day_res.name = copy highway offline day resource ${QMAKE_FILE_IN}
copy_highway_offline_day_res.input = HIGHWAY_OFFLINE_DAY_RESOURCE_FILES
copy_highway_offline_day_res.output = $${RSM2018_800x480_RESDIR}/img/highway/offline/day/${QMAKE_FILE_BASE}${QMAKE_FILE_EXT}
copy_highway_offline_day_res.commands = ${COPY_FILE} ${QMAKE_FILE_IN} ${QMAKE_FILE_OUT}
copy_highway_offline_day_res.CONFIG += no_link target_predeps
QMAKE_EXTRA_COMPILERS += copy_highway_offline_day_res

# img/highway/offline/night resources
mkpath($${RSM2018_800x480_RESDIR}/img/highway/offline/night)
HIGHWAY_OFFLINE_NIGHT_RESOURCE_FILES = $$files($${ABSOLUTE_800x480_RESDIR}/img/highway/offline/night/*)

copy_highway_offline_night_res.name = copy highway offline night resource ${QMAKE_FILE_IN}
copy_highway_offline_night_res.input = HIGHWAY_OFFLINE_NIGHT_RESOURCE_FILES
copy_highway_offline_night_res.output = $${RSM2018_800x480_RESDIR}/img/highway/offline/night/${QMAKE_FILE_BASE}${QMAKE_FILE_EXT}
copy_highway_offline_night_res.commands = ${COPY_FILE} ${QMAKE_FILE_IN} ${QMAKE_FILE_OUT}
copy_highway_offline_night_res.CONFIG += no_link target_predeps
QMAKE_EXTRA_COMPILERS += copy_highway_offline_night_res

# lang/en/img/highway resources
mkpath($${RSM2018_800x480_RESDIR}/lang/en/img/highway)
EN_HIGHWAY_RESOURCE_FILES = $$files($${ABSOLUTE_800x480_RESDIR}/lang/en/img/highway/*)

copy_en_highway_res.name = copy en highway resource ${QMAKE_FILE_IN}
copy_en_highway_res.input = EN_HIGHWAY_RESOURCE_FILES
copy_en_highway_res.output = $${RSM2018_800x480_RESDIR}/lang/en/img/highway/${QMAKE_FILE_BASE}${QMAKE_FILE_EXT}
copy_en_highway_res.commands = ${COPY_FILE} ${QMAKE_FILE_IN} ${QMAKE_FILE_OUT}
copy_en_highway_res.CONFIG += no_link target_predeps
QMAKE_EXTRA_COMPILERS += copy_en_highway_res

# img/sdi resources
mkpath($${RSM2018_800x480_RESDIR}/img/sdi)
SDI_RESOURCE_FILES = $$files($${ABSOLUTE_800x480_RESDIR}/img/sdi/*)

copy_sdi_res.name = copy sdi resource ${QMAKE_FILE_IN}
copy_sdi_res.input = SDI_RESOURCE_FILES
copy_sdi_res.output = $${RSM2018_800x480_RESDIR}/img/sdi/${QMAKE_FILE_BASE}${QMAKE_FILE_EXT}
copy_sdi_res.commands = ${COPY_FILE} ${QMAKE_FILE_IN} ${QMAKE_FILE_OUT}
copy_sdi_res.CONFIG += no_link target_predeps
QMAKE_EXTRA_COMPILERS += copy_sdi_res

# img/zone resources
mkpath($${RSM2018_800x480_RESDIR}/img/zone)
ZONE_RESOURCE_FILES = $$files($${ABSOLUTE_800x480_RESDIR}/img/zone/*)

copy_zone_res.name = copy_zone_resource ${QMAKE_FILE_IN}
copy_zone_res.input = ZONE_RESOURCE_FILES
copy_zone_res.output = $${RSM2018_800x480_RESDIR}/img/zone/${QMAKE_FILE_BASE}${QMAKE_FILE_EXT}
copy_zone_res.commands = ${COPY_FILE} ${QMAKE_FILE_IN} ${QMAKE_FILE_OUT}
copy_zone_res.CONFIG += no_link target_predeps
QMAKE_EXTRA_COMPILERS += copy_zone_res

# img/tbt resources
mkpath($${RSM2018_800x480_RESDIR}/img/tbt)
TBT_RESOURCE_FILES = $$files($${ABSOLUTE_800x480_RESDIR}/img/tbt/*)

copy_tbt_res.name = copy tbt resource ${QMAKE_FILE_IN}
copy_tbt_res.input = TBT_RESOURCE_FILES
copy_tbt_res.output = $${RSM2018_800x480_RESDIR}/img/tbt/${QMAKE_FILE_BASE}${QMAKE_FILE_EXT}
copy_tbt_res.commands = ${COPY_DIR} ${QMAKE_FILE_IN} ${QMAKE_FILE_OUT}
copy_tbt_res.CONFIG += no_link target_predeps
QMAKE_EXTRA_COMPILERS += copy_tbt_res

# img/tbt/online/day resources
mkpath($${RSM2018_800x480_RESDIR}/img/tbt/online/day)
TBT_ONLINE_DAY_RESOURCE_FILES = $$files($${ABSOLUTE_800x480_RESDIR}/img/tbt/online/day/*)

copy_tbt_online_day_res.name = copy tbt online day resource ${QMAKE_FILE_IN}
copy_tbt_online_day_res.input = TBT_ONLINE_DAY_RESOURCE_FILES
copy_tbt_online_day_res.output = $${RSM2018_800x480_RESDIR}/img/tbt/online/day/${QMAKE_FILE_BASE}${QMAKE_FILE_EXT}
copy_tbt_online_day_res.commands = ${COPY_FILE} ${QMAKE_FILE_IN} ${QMAKE_FILE_OUT}
copy_tbt_online_day_res.CONFIG += no_link target_predeps
QMAKE_EXTRA_COMPILERS += copy_tbt_online_day_res

# img/tbt/online/night resources
mkpath($${RSM2018_800x480_RESDIR}/img/tbt/online/night)
TBT_ONLINE_NIGHT_RESOURCE_FILES = $$files($${ABSOLUTE_800x480_RESDIR}/img/tbt/online/night/*)

copy_tbt_online_night_res.name = copy tbt online night resource ${QMAKE_FILE_IN}
copy_tbt_online_night_res.input = TBT_ONLINE_NIGHT_RESOURCE_FILES
copy_tbt_online_night_res.output = $${RSM2018_800x480_RESDIR}/img/tbt/online/night/${QMAKE_FILE_BASE}${QMAKE_FILE_EXT}
copy_tbt_online_night_res.commands = ${COPY_FILE} ${QMAKE_FILE_IN} ${QMAKE_FILE_OUT}
copy_tbt_online_night_res.CONFIG += no_link target_predeps
QMAKE_EXTRA_COMPILERS += copy_tbt_online_night_res

# img/tbt/offline/day resources
mkpath($${RSM2018_800x480_RESDIR}/img/tbt/offline/day)
TBT_OFFLINE_DAY_RESOURCE_FILES = $$files($${ABSOLUTE_800x480_RESDIR}/img/tbt/offline/day/*)

copy_tbt_offline_day_res.name = copy tbt offline day resource ${QMAKE_FILE_IN}
copy_tbt_offline_day_res.input = TBT_OFFLINE_DAY_RESOURCE_FILES
copy_tbt_offline_day_res.output = $${RSM2018_800x480_RESDIR}/img/tbt/offline/day/${QMAKE_FILE_BASE}${QMAKE_FILE_EXT}
copy_tbt_offline_day_res.commands = ${COPY_FILE} ${QMAKE_FILE_IN} ${QMAKE_FILE_OUT}
copy_tbt_offline_day_res.CONFIG += no_link target_predeps
QMAKE_EXTRA_COMPILERS += copy_tbt_offline_day_res

# img/tbt/offline/night resources
mkpath($${RSM2018_800x480_RESDIR}/img/tbt/offline/night)
TBT_OFFLINE_NIGHT_RESOURCE_FILES = $$files($${ABSOLUTE_800x480_RESDIR}/img/tbt/offline/night/*)

copy_tbt_offline_night_res.name = copy tbt offline night resource ${QMAKE_FILE_IN}
copy_tbt_offline_night_res.input = TBT_OFFLINE_NIGHT_RESOURCE_FILES
copy_tbt_offline_night_res.output = $${RSM2018_800x480_RESDIR}/img/tbt/offline/night/${QMAKE_FILE_BASE}${QMAKE_FILE_EXT}
copy_tbt_offline_night_res.commands = ${COPY_FILE} ${QMAKE_FILE_IN} ${QMAKE_FILE_OUT}
copy_tbt_offline_night_res.CONFIG += no_link target_predeps
QMAKE_EXTRA_COMPILERS += copy_tbt_offline_night_res

# img/weather resources
mkpath($${RSM2018_800x480_RESDIR}/img/weather)
WEATHER_RESOURCE_FILES = $$files($${ABSOLUTE_800x480_RESDIR}/img/weather/*)

copy_weather_res.name = copy_weather_resource ${QMAKE_FILE_IN}
copy_weather_res.input = WEATHER_RESOURCE_FILES
copy_weather_res.output = $${RSM2018_800x480_RESDIR}/img/weather/${QMAKE_FILE_BASE}${QMAKE_FILE_EXT}
copy_weather_res.commands = ${COPY_FILE} ${QMAKE_FILE_IN} ${QMAKE_FILE_OUT}
copy_weather_res.CONFIG += no_link target_predeps
QMAKE_EXTRA_COMPILERS += copy_weather_res

# img/junction resources
mkpath($${RSM2018_800x480_RESDIR}/img/junction)
JUNCTION_RESOURCE_FILES = $$files($${ABSOLUTE_800x480_RESDIR}/img/junction/*)

copy_junction_res.name = copy junction resource ${QMAKE_FILE_IN}
copy_junction_res.input = JUNCTION_RESOURCE_FILES
copy_junction_res.output = $${RSM2018_800x480_RESDIR}/img/junction/${QMAKE_FILE_BASE}${QMAKE_FILE_EXT}
copy_junction_res.commands = ${COPY_FILE} ${QMAKE_FILE_IN} ${QMAKE_FILE_OUT}
copy_junction_res.CONFIG += no_link target_predeps
QMAKE_EXTRA_COMPILERS += copy_junction_res

# img/address resources
mkpath($${RSM2018_800x480_RESDIR}/img/address)
ADDRESS_RESOURCE_FILES = $$files($${ABSOLUTE_800x480_RESDIR}/img/address/*)

copy_address_res.name = copy address resource ${QMAKE_FILE_IN}
copy_address_res.input = ADDRESS_RESOURCE_FILES
copy_address_res.output = $${RSM2018_800x480_RESDIR}/img/address/${QMAKE_FILE_BASE}${QMAKE_FILE_EXT}
copy_address_res.commands = ${COPY_FILE} ${QMAKE_FILE_IN} ${QMAKE_FILE_OUT}
copy_address_res.CONFIG += no_link target_predeps
QMAKE_EXTRA_COMPILERS += copy_address_res

# img/navimenu resources
mkpath($${RSM2018_800x480_RESDIR}/img/navimenu)
NAVIMENU_RESOURCE_FILES = $$files($${ABSOLUTE_800x480_RESDIR}/img/navimenu/*)

copy_navimenu_res.name = copy navimenu resource ${QMAKE_FILE_IN}
copy_navimenu_res.input = NAVIMENU_RESOURCE_FILES
copy_navimenu_res.output = $${RSM2018_800x480_RESDIR}/img/navimenu/${QMAKE_FILE_BASE}${QMAKE_FILE_EXT}
copy_navimenu_res.commands = ${COPY_FILE} ${QMAKE_FILE_IN} ${QMAKE_FILE_OUT}
copy_navimenu_res.CONFIG += no_link target_predeps
QMAKE_EXTRA_COMPILERS += copy_navimenu_res

# img/settingmenu resources
mkpath($${RSM2018_800x480_RESDIR}/img/settingmenu)
SETTINGMENU_RESOURCE_FILES = $$files($${ABSOLUTE_800x480_RESDIR}/img/settingmenu/*)

copy_settingmenu_res.name = copy settingmenu resource ${QMAKE_FILE_IN}
copy_settingmenu_res.input = SETTINGMENU_RESOURCE_FILES
copy_settingmenu_res.output = $${RSM2018_800x480_RESDIR}/img/settingmenu/${QMAKE_FILE_BASE}${QMAKE_FILE_EXT}
copy_settingmenu_res.commands = ${COPY_FILE} ${QMAKE_FILE_IN} ${QMAKE_FILE_OUT}
copy_settingmenu_res.CONFIG += no_link target_predeps
QMAKE_EXTRA_COMPILERS += copy_settingmenu_res


# img/industrymenu resources
mkpath($${RSM2018_800x480_RESDIR}/img/industrymenu)
INDUSTRYMENU_RESOURCE_FILES = $$files($${ABSOLUTE_800x480_RESDIR}/img/industrymenu/*)

copy_industrymenu_res.name = copy industrymenu resource ${QMAKE_FILE_IN}
copy_industrymenu_res.input = INDUSTRYMENU_RESOURCE_FILES
copy_industrymenu_res.output = $${RSM2018_800x480_RESDIR}/img/industrymenu/${QMAKE_FILE_BASE}${QMAKE_FILE_EXT}
copy_industrymenu_res.commands = ${COPY_FILE} ${QMAKE_FILE_IN} ${QMAKE_FILE_OUT}
copy_industrymenu_res.CONFIG += no_link target_predeps
QMAKE_EXTRA_COMPILERS += copy_industrymenu_res


# img/aroundmenu resources
mkpath($${RSM2018_800x480_RESDIR}/img/aroundmenu)
AROUNDMENU_RESOURCE_FILES = $$files($${ABSOLUTE_800x480_RESDIR}/img/aroundmenu/*)

copy_aroundmenu_res.name = copy aroundmenu resource ${QMAKE_FILE_IN}
copy_aroundmenu_res.input = AROUNDMENU_RESOURCE_FILES
copy_aroundmenu_res.output = $${RSM2018_800x480_RESDIR}/img/aroundmenu/${QMAKE_FILE_BASE}${QMAKE_FILE_EXT}
copy_aroundmenu_res.commands = ${COPY_FILE} ${QMAKE_FILE_IN} ${QMAKE_FILE_OUT}
copy_aroundmenu_res.CONFIG += no_link target_predeps
QMAKE_EXTRA_COMPILERS += copy_aroundmenu_res


# img/setting resources
mkpath($${RSM2018_800x480_RESDIR}/img/setting)
SETTING_RESOURCE_FILES = $$files($${ABSOLUTE_800x480_RESDIR}/img/setting/*)

copy_setting_res.name = copy setting resource ${QMAKE_FILE_IN}
copy_setting_res.input = SETTING_RESOURCE_FILES
copy_setting_res.output = $${RSM2018_800x480_RESDIR}/img/setting/${QMAKE_FILE_BASE}${QMAKE_FILE_EXT}
copy_setting_res.commands = ${COPY_FILE} ${QMAKE_FILE_IN} ${QMAKE_FILE_OUT}
copy_setting_res.CONFIG += no_link target_predeps
QMAKE_EXTRA_COMPILERS += copy_setting_res

# img/navi resources
mkpath($${RSM2018_800x480_RESDIR}/img/navi)
NAVI_RESOURCE_FILES = $$files($${ABSOLUTE_800x480_RESDIR}/img/navi/*)

copy_navi_res.name = copy navi resource ${QMAKE_FILE_IN}
copy_navi_res.input = NAVI_RESOURCE_FILES
copy_navi_res.output = $${RSM2018_800x480_RESDIR}/img/navi/${QMAKE_FILE_BASE}${QMAKE_FILE_EXT}
copy_navi_res.commands = ${COPY_FILE} ${QMAKE_FILE_IN} ${QMAKE_FILE_OUT}
copy_navi_res.CONFIG += no_link target_predeps
QMAKE_EXTRA_COMPILERS += copy_navi_res


# img/direction resources
mkpath($${RSM2018_800x480_RESDIR}/img/direction)
DIRECTION_RESOURCE_FILES = $$files($${ABSOLUTE_800x480_RESDIR}/img/direction/*)

copy_direction_res.name = copy direction resource ${QMAKE_FILE_IN}
copy_direction_res.input = DIRECTION_RESOURCE_FILES
copy_direction_res.output = $${RSM2018_800x480_RESDIR}/img/direction/${QMAKE_FILE_BASE}${QMAKE_FILE_EXT}
copy_direction_res.commands = ${COPY_FILE} ${QMAKE_FILE_IN} ${QMAKE_FILE_OUT}
copy_direction_res.CONFIG += no_link target_predeps
QMAKE_EXTRA_COMPILERS += copy_direction_res


# img/search resources
mkpath($${RSM2018_800x480_RESDIR}/img/search)
SEARCH_RESOURCE_FILES = $$files($${ABSOLUTE_800x480_RESDIR}/img/search/*)

copy_search_res.name = copy search resource ${QMAKE_FILE_IN}
copy_search_res.input = SEARCH_RESOURCE_FILES
copy_search_res.output = $${RSM2018_800x480_RESDIR}/img/search/${QMAKE_FILE_BASE}${QMAKE_FILE_EXT}
copy_search_res.commands = ${COPY_FILE} ${QMAKE_FILE_IN} ${QMAKE_FILE_OUT}
copy_search_res.CONFIG += no_link target_predeps
QMAKE_EXTRA_COMPILERS += copy_search_res

# img/hipass resources
mkpath($${RSM2018_800x480_RESDIR}/img/hipass)
HIPASS_RESOURCE_FILES = $$files($${ABSOLUTE_800x480_RESDIR}/img/hipass/*)

copy_hipass_res.name = copy hipass resource ${QMAKE_FILE_IN}
copy_hipass_res.input = HIPASS_RESOURCE_FILES
copy_hipass_res.output = $${RSM2018_800x480_RESDIR}/img/hipass/${QMAKE_FILE_BASE}${QMAKE_FILE_EXT}
copy_hipass_res.commands = ${COPY_FILE} ${QMAKE_FILE_IN} ${QMAKE_FILE_OUT}
copy_hipass_res.CONFIG += no_link target_predeps
QMAKE_EXTRA_COMPILERS += copy_hipass_res

# img/util resources
mkpath($${RSM2018_800x480_RESDIR}/img/util)
UTIL_RESOURCE_FILES = $$files($${ABSOLUTE_800x480_RESDIR}/img/util/*)

copy_util_res.name = copy util resource ${QMAKE_FILE_IN}
copy_util_res.input = UTIL_RESOURCE_FILES
copy_util_res.output = $${RSM2018_800x480_RESDIR}/img/util/${QMAKE_FILE_BASE}${QMAKE_FILE_EXT}
copy_util_res.commands = ${COPY_FILE} ${QMAKE_FILE_IN} ${QMAKE_FILE_OUT}
copy_util_res.CONFIG += no_link target_predeps
QMAKE_EXTRA_COMPILERS += copy_util_res

# img/map resources
mkpath($${RSM2018_800x480_RESDIR}/img/map)
MAP_RESOURCE_FILES = $$files($${ABSOLUTE_800x480_RESDIR}/img/map/*)

copy_map_res.name = copy map resource ${QMAKE_FILE_IN}
copy_map_res.input = MAP_RESOURCE_FILES
copy_map_res.output = $${RSM2018_800x480_RESDIR}/img/map/${QMAKE_FILE_BASE}${QMAKE_FILE_EXT}
copy_map_res.commands = ${COPY_FILE} ${QMAKE_FILE_IN} ${QMAKE_FILE_OUT}
copy_map_res.CONFIG += no_link target_predeps
QMAKE_EXTRA_COMPILERS += copy_map_res

# img/widgettbt resources
mkpath($${RSM2018_800x480_RESDIR}/img/widgettbt)
WIDGETTBT_RESOURCE_FILES = $$files($${ABSOLUTE_800x480_RESDIR}/img/widgettbt/*)

copy_widgettbt_res.name = copy widgettbt resource ${QMAKE_FILE_IN}
copy_widgettbt_res.input = WIDGETTBT_RESOURCE_FILES
copy_widgettbt_res.output = $${RSM2018_800x480_RESDIR}/img/widgettbt/${QMAKE_FILE_BASE}${QMAKE_FILE_EXT}
copy_widgettbt_res.commands = ${COPY_FILE} ${QMAKE_FILE_IN} ${QMAKE_FILE_OUT}
copy_widgettbt_res.CONFIG += no_link target_predeps
QMAKE_EXTRA_COMPILERS += copy_widgettbt_res

# img/widgetlane resources
mkpath($${RSM2018_800x480_RESDIR}/img/widgetlane)
WIDGETLANE_RESOURCE_FILES = $$files($${ABSOLUTE_800x480_RESDIR}/img/widgetlane/*)

copy_widgetlane_res.name = copy widgettbt resource ${QMAKE_FILE_IN}
copy_widgetlane_res.input = WIDGETLANE_RESOURCE_FILES
copy_widgetlane_res.output = $${RSM2018_800x480_RESDIR}/img/widgetlane/${QMAKE_FILE_BASE}${QMAKE_FILE_EXT}
copy_widgetlane_res.commands = ${COPY_FILE} ${QMAKE_FILE_IN} ${QMAKE_FILE_OUT}
copy_widgetlane_res.CONFIG += no_link target_predeps
QMAKE_EXTRA_COMPILERS += copy_widgetlane_res

# img/widgetsdi resources
mkpath($${RSM2018_800x480_RESDIR}/img/widgetsdi)
WIDGETSDI_RESOURCE_FILES = $$files($${ABSOLUTE_800x480_RESDIR}/img/widgetsdi/*)

copy_widgetsdi_res.name = copy widgetsdi resource ${QMAKE_FILE_IN}
copy_widgetsdi_res.input = WIDGETSDI_RESOURCE_FILES
copy_widgetsdi_res.output = $${RSM2018_800x480_RESDIR}/img/widgetsdi/${QMAKE_FILE_BASE}${QMAKE_FILE_EXT}
copy_widgetsdi_res.commands = ${COPY_FILE} ${QMAKE_FILE_IN} ${QMAKE_FILE_OUT}
copy_widgetsdi_res.CONFIG += no_link target_predeps
QMAKE_EXTRA_COMPILERS += copy_widgetsdi_res

# img/gadget resources
mkpath($${RSM2018_800x480_RESDIR}/img/gadget)
GADGET_RESOURCE_FILES = $$files($${ABSOLUTE_800x480_RESDIR}/img/gadget/*)

copy_gadget_res.name = copy gadget resource ${QMAKE_FILE_IN}
copy_gadget_res.input = GADGET_RESOURCE_FILES
copy_gadget_res.output = $${RSM2018_800x480_RESDIR}/img/gadget/${QMAKE_FILE_BASE}${QMAKE_FILE_EXT}
copy_gadget_res.commands = ${COPY_FILE} ${QMAKE_FILE_IN} ${QMAKE_FILE_OUT}
copy_gadget_res.CONFIG += no_link target_predeps
QMAKE_EXTRA_COMPILERS += copy_gadget_res

# language specific

# lang/en/img/tbt resources
mkpath($${RSM2018_800x480_RESDIR}/lang/en/img/tbt)
EN_TBT_RESOURCE_FILES = $$files($${ABSOLUTE_800x480_RESDIR}/lang/en/img/tbt/*)

copy_en_tbt_res.name = copy en tbt resource ${QMAKE_FILE_IN}
copy_en_tbt_res.input = EN_TBT_RESOURCE_FILES
copy_en_tbt_res.output = $${RSM2018_800x480_RESDIR}/lang/en/img/tbt/${QMAKE_FILE_BASE}${QMAKE_FILE_EXT}
copy_en_tbt_res.commands = ${COPY_FILE} ${QMAKE_FILE_IN} ${QMAKE_FILE_OUT}
copy_en_tbt_res.CONFIG += no_link target_predeps
QMAKE_EXTRA_COMPILERS += copy_en_tbt_res

# lang/en/gadget
mkpath($${RSM2018_800x480_RESDIR}/lang/en/img/gadget)
EN_GADGET_RESOURCE_FILES = $$files($${ABSOLUTE_800x480_RESDIR}/lang/en/img/gadget/*)

copy_en_gad_res.name = copy en gadget resource ${QMAKE_FILE_IN}
copy_en_gad_res.input = EN_GADGET_RESOURCE_FILES
copy_en_gad_res.output = $${RSM2018_800x480_RESDIR}/lang/en/img/gadget/${QMAKE_FILE_BASE}${QMAKE_FILE_EXT}
copy_en_gad_res.commands = ${COPY_FILE} ${QMAKE_FILE_IN} ${QMAKE_FILE_OUT}
copy_en_gad_res.CONFIG += no_link target_predeps
QMAKE_EXTRA_COMPILERS += copy_en_gad_res

# popup

# img/popup resources
mkpath($${RSM2018_800x480_RESDIR}/img/popup)
POPUP_RESOURCE_FILES = $$files($${ABSOLUTE_800x480_RESDIR}/img/popup/*)

copy_popup_res.name = copy popup resource ${QMAKE_FILE_IN}
copy_popup_res.input = POPUP_RESOURCE_FILES
copy_popup_res.output = $${RSM2018_800x480_RESDIR}/img/popup/${QMAKE_FILE_BASE}${QMAKE_FILE_EXT}
copy_popup_res.commands = ${COPY_FILE} ${QMAKE_FILE_IN} ${QMAKE_FILE_OUT}
copy_popup_res.CONFIG += no_link target_predeps
QMAKE_EXTRA_COMPILERS += copy_popup_res
