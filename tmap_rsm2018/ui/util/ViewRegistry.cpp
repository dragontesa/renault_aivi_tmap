#include "ViewRegistry.h"
#include "DebugUtils.h"

using namespace SKT;

ViewRegistry::ViewRegistry()
{
    // ViewK 상수 순서와 일치해야 함
    mViewInfo = QVector<ViewInfo>({
#ifdef QT_DEBUG
        {QStringLiteral("HardKeyView"), GuiFrame, QStringLiteral("qrc:/rsm_ui/qml/view/zone/HardKeyView.qml")},
#else
        {QStringLiteral("HardKeyView"), GuiFrame, QStringLiteral("qrc:/rsm_ui/qml/view/zone/dummy/HardKeyView.qml")},
#endif
        {QStringLiteral("HighwayTbtView"), RgPopup, QStringLiteral("qrc:/rsm_ui/qml/view/rg/HighwayTbtView.qml")},
        {QStringLiteral("LaneView"), RgPopup, QStringLiteral("qrc:/rsm_ui/qml/view/rg/LaneView.qml")},
        {QStringLiteral("MapControlView"), 0, QStringLiteral("qrc:/rsm_ui/qml/view/map/MapControlView.qml")},
        {QStringLiteral("MapModeOptionView"), 0, QStringLiteral("qrc:/rsm_ui/qml/view/map/MapModeOptionView.qml")},
        {QStringLiteral("MapView"), 0, QStringLiteral("qrc:/rsm_ui/qml/view/map/MapView.qml")},
        {QStringLiteral("NaviMenuView"), Popup|Inquiry|Menu, QStringLiteral("qrc:/rsm_ui/qml/view/menu/NaviMenuView.qml")},
        {QStringLiteral("RecentTileGridView"), Popup|Inquiry, QStringLiteral("qrc:/rsm_ui/qml/view/search/RecentTileGridView.qml")},
        {QStringLiteral("RouteCandidateView"), Popup|Inquiry, QStringLiteral("qrc:/rsm_ui/qml/view/route/RouteCandidateView.qml")},
        {QStringLiteral("RouteListView"), Popup|Inquiry, QStringLiteral("qrc:/rsm_ui/qml/view/route/RouteListView.qml")},
        {QStringLiteral("RouteRegistView"), Popup|Inquiry, QStringLiteral("qrc:/rsm_ui/qml/view/route/RouteRegistView.qml")},
        {QStringLiteral("RouteSettingView"), Popup|Inquiry, QStringLiteral("qrc:/rsm_ui/qml/view/route/RouteSettingView.qml")},
        {QStringLiteral("RouteSummaryTestView"), Popup|Inquiry, QStringLiteral("qrc:/rsm_ui/qml/view/route/RouteSummaryTestView.qml")},
        {QStringLiteral("RouteSummaryView"), Popup|Inquiry, QStringLiteral("qrc:/rsm_ui/qml/view/route/RouteSummaryView.qml")},
        {QStringLiteral("ErrorView"), Popup|Inquiry, QStringLiteral("qrc:/rsm_ui/qml/view/dbg/ErrorView.qml")},
        {QStringLiteral("SdiView"), RgPopup, QStringLiteral("qrc:/rsm_ui/qml/view/rg/SdiView.qml")},
        {QStringLiteral("SimulationView"), 0, QStringLiteral("qrc:/rsm_ui/qml/view/map/SimulationView.qml")},
        {QStringLiteral("TbtView"), RgPopup, QStringLiteral("qrc:/rsm_ui/qml/view/rg/TbtView.qml")},
        {QStringLiteral("TestView"), Overlap, QStringLiteral("qrc:/rsm_ui/qml/view/dbg/TestView.qml")},
        {QStringLiteral("MonkeyTestView"), Overlap, QStringLiteral("qrc:/rsm_ui/qml/view/dbg/MonkeyTestView.qml")},
        {QStringLiteral("TestMultiMapView"), Overlap, QStringLiteral("qrc:/rsm_ui/qml/view/dbg/TestMultiMapView.qml")},
#ifdef QT_DEBUG
        {QStringLiteral("Zone1View"), GuiFrame, QStringLiteral("qrc:/rsm_ui/qml/view/zone/Zone1View.qml")},
#else
        {QStringLiteral("Zone1View"), GuiFrame, QStringLiteral("qrc:/rsm_ui/qml/view/zone/dummy/Zone1View.qml")},
#endif
        {QStringLiteral("Zone3View"), GuiFrame, QStringLiteral("qrc:/rsm_ui/qml/view/zone/Zone3View.qml")},
        {QStringLiteral("ZoneApiTestView"), GuiFrame, QStringLiteral("qrc:/rsm_ui/qml/view/zone/ZoneApiTestView.qml")},
        {QStringLiteral("OptionMenuView"), Popup, QStringLiteral("qrc:/rsm_ui/qml/view/zone/OptionMenuView.qml")},
        {QStringLiteral("CrossImageView"), RgPopup, QStringLiteral("qrc:/rsm_ui/qml/view/rg/CrossImageView.qml")},
        {QStringLiteral("HiPassLaneView"), RgPopup, QStringLiteral("qrc:/rsm_ui/qml/view/rg/HiPassLaneView.qml")},
        {QStringLiteral("SummaryTbtView"), RgPopup, QStringLiteral("qrc:/rsm_ui/qml/view/rg/SummaryTbtView.qml")},
        {QStringLiteral("SpeedMeterView"), RgPopup, QStringLiteral("qrc:/rsm_ui/qml/view/rg/SpeedMeterView.qml")},
        {QStringLiteral("BusLaneView"), RgPopup, QStringLiteral("qrc:/rsm_ui/qml/view/rg/BusLaneView.qml")},
        {QStringLiteral("TrafficBarView"), RgPopup, QStringLiteral("qrc:/rsm_ui/qml/view/rg/TrafficBarView.qml")},
        {QStringLiteral("ChangeDestinationView"), Popup, QStringLiteral("qrc:/rsm_ui/qml/view/rg/ChangeDestinationView.qml")},
        {QStringLiteral("CommonDialogView"), Popup|Dialog, QStringLiteral("qrc:/rsm_ui/qml/view/CommonDialogView.qml")},

        {QStringLiteral("FullscreenAlertView"), RgPopup|Overlap, QStringLiteral("qrc:/rsm_ui/qml/view/rg/FullscreenAlertView.qml")},
#ifdef QT_DEBUG
        {QStringLiteral("KeyboardView"), Test, QStringLiteral("qrc:/rsm_ui/qml/view/keyboard/KeyboardView.qml")},
#else
        {QStringLiteral("KeyboardView"), Test, QStringLiteral("qrc:/rsm_ui/qml/view/keyboard/dummy/KeyboardView.qml")},
#endif

        {QStringLiteral("AddressSearchView"), Popup|Inquiry|Search, QStringLiteral("qrc:/rsm_ui/qml/view/search/AddressSearchView.qml")},
        {QStringLiteral("BookmarkPageView"), Popup|Inquiry|Search, QStringLiteral("qrc:/rsm_ui/qml/view/search/BookmarkPageView.qml")},
        {QStringLiteral("BookmarkPageEditView"), Popup|Inquiry|Search, QStringLiteral("qrc:/rsm_ui/qml/view/search/BookmarkPageEditView.qml")},
		{QStringLiteral("BookmarkView"), Popup|Inquiry|Search, QStringLiteral("qrc:/rsm_ui/qml/view/search/BookmarkView.qml")},        
        {QStringLiteral("MapSearchView"), Popup|Inquiry|Search, QStringLiteral("qrc:/rsm_ui/qml/view/search/MapSearchView.qml")},
        {QStringLiteral("RecentDestinationPageView"), Popup|Inquiry|Search, QStringLiteral("qrc:/rsm_ui/qml/view/search/RecentDestinationPageView.qml")},
        {QStringLiteral("RecentDestinationView"), Popup|Inquiry|Search, QStringLiteral("qrc:/rsm_ui/qml/view/search/RecentDestinationView.qml")},
        {QStringLiteral("RecentSearchEditView"), Popup|Inquiry|Search, QStringLiteral("qrc:/rsm_ui/qml/view/search/RecentSearchEditView.qml")},
        {QStringLiteral("RecentSearchPageView"), Popup|Inquiry|Search, QStringLiteral("qrc:/rsm_ui/qml/view/search/RecentSearchPageView.qml")},
        {QStringLiteral("NoResultPageView"), Popup|Inquiry|Search, QStringLiteral("qrc:/rsm_ui/qml/view/search/NoResultPageView.qml")},
        {QStringLiteral("SearchResultDetailView"), Popup|Inquiry|Search, QStringLiteral("qrc:/rsm_ui/qml/view/search/SearchResultDetailView.qml")},
        {QStringLiteral("SearchResultView"), Popup|Inquiry|Search, QStringLiteral("qrc:/rsm_ui/qml/view/search/SearchResultView.qml")},
        {QStringLiteral("SearchTabView"), Popup|Inquiry|Search, QStringLiteral("qrc:/rsm_ui/qml/view/search/SearchTabView.qml")},
        {QStringLiteral("TextSearchView"), Popup|Inquiry|Search, QStringLiteral("qrc:/rsm_ui/qml/view/search/TextSearchView.qml")},
        {QStringLiteral("BookmarkRegistView"), Popup|Inquiry|Search, QStringLiteral("qrc:/rsm_ui/qml/view/search/BookmarkRegistView.qml")},
        {QStringLiteral("RegionSearchView"), Popup|Inquiry|Search, QStringLiteral("qrc:/rsm_ui/qml/view/search/RegionSearchView.qml")},
        {QStringLiteral("RecentDestinationEditView"), Popup|Inquiry|Search, QStringLiteral("qrc:/rsm_ui/qml/view/search/RecentDestinationEditView.qml")},
        {QStringLiteral("IndustrySearchView"), Popup|Inquiry|Search, QStringLiteral("qrc:/rsm_ui/qml/view/search/IndustrySearchView.qml")},
        {QStringLiteral("SearchResultOptionView"), Popup|Inquiry|Search, QStringLiteral("qrc:/rsm_ui/qml/view/search/SearchResultOptionView.qml")},
        {QStringLiteral("SearchAroundView"), Popup|Inquiry|Search, QStringLiteral("qrc:/rsm_ui/qml/view/search/SearchAroundView.qml")},
        {QStringLiteral("PoiTabView"), Popup|Inquiry|Search, QStringLiteral("qrc:/rsm_ui/qml/view/rg/PoiTabView.qml")},

        {QStringLiteral("SettingAttentionSectionView"), Popup|Inquiry|Setting, QStringLiteral("qrc:/rsm_ui/qml/view/setting/SettingAttentionSectionView.qml")},
        {QStringLiteral("SettingCameraWarningSoundView"), Popup|Inquiry|Setting, QStringLiteral("qrc:/rsm_ui/qml/view/setting/SettingCameraWarningSoundView.qml")},
        {QStringLiteral("SettingCarIconView"), Popup|Inquiry|Setting, QStringLiteral("qrc:/rsm_ui/qml/view/setting/SettingCarIconView.qml")},
        {QStringLiteral("SettingGpsInfoView"), Popup|Inquiry|Setting, QStringLiteral("qrc:/rsm_ui/qml/view/setting/SettingGpsInfoView.qml")},
        {QStringLiteral("SettingGuidView"), Popup|Inquiry|Setting, QStringLiteral("qrc:/rsm_ui/qml/view/setting/SettingGuidView.qml")},
        {QStringLiteral("SettingGuidVoiceView"), Popup|Inquiry|Setting, QStringLiteral("qrc:/rsm_ui/qml/view/setting/SettingGuidVoiceView.qml")},
        {QStringLiteral("SettingInitView"), Popup|Inquiry|Setting, QStringLiteral("qrc:/rsm_ui/qml/view/setting/SettingInitView.qml")},
        {QStringLiteral("SettingMainView"), Popup|Inquiry|Setting, QStringLiteral("qrc:/rsm_ui/qml/view/setting/SettingMainView.qml")},
        {QStringLiteral("SettingMapCharSizeView"), Popup|Inquiry|Setting, QStringLiteral("qrc:/rsm_ui/qml/view/setting/SettingMapCharSizeView.qml")},
        {QStringLiteral("SettingMapUpdateView"), Inquiry|Setting, QStringLiteral("qrc:/rsm_ui/qml/view/setting/SettingMapUpdateView.qml")},
        {QStringLiteral("SettingMapView"), Popup|Inquiry|Setting, QStringLiteral("qrc:/rsm_ui/qml/view/setting/SettingMapView.qml")},
        {QStringLiteral("SettingNaviInfoView"), Popup|Inquiry|Setting, QStringLiteral("qrc:/rsm_ui/qml/view/setting/SettingNaviInfoView.qml")},
        {QStringLiteral("SettingRouteAlternateView"), Popup|Inquiry|Setting, QStringLiteral("qrc:/rsm_ui/qml/view/setting/SettingRouteAlternateView.qml")},
        {QStringLiteral("SettingRouteSearchView"), Popup|Inquiry|Setting, QStringLiteral("qrc:/rsm_ui/qml/view/setting/SettingRouteSearchView.qml")},
        {QStringLiteral("SettingSpeedResponseMapView"), Popup|Inquiry|Setting, QStringLiteral("qrc:/rsm_ui/qml/view/setting/SettingSpeedResponseMapView.qml")},
        {QStringLiteral("SettingMobileTmapView"), Popup|Inquiry|Setting, QStringLiteral("qrc:/rsm_ui/qml/view/setting/SettingMobileTmapView.qml")},
        {QStringLiteral("SettingOilKindView"), Popup|Inquiry|Setting, QStringLiteral("qrc:/rsm_ui/qml/view/setting/SettingOilKindView.qml")},
        {QStringLiteral("SettingOilBrandView"), Popup|Inquiry|Setting, QStringLiteral("qrc:/rsm_ui/qml/view/setting/SettingOilBrandView.qml")},
        {QStringLiteral("SettingOilAreaView"), Popup|Inquiry|Setting, QStringLiteral("qrc:/rsm_ui/qml/view/setting/SettingOilAreaView.qml")},
        {QStringLiteral("SettingVoiceGuidTimeView"), Popup|Inquiry|Setting, QStringLiteral("qrc:/rsm_ui/qml/view/setting/SettingVoiceGuidTimeView.qml")},
        {QStringLiteral("SettingAQIdetailView"), Popup|Inquiry|Setting|DoNotCallActivateWhenSwitchGadgetToNavi, QStringLiteral("qrc:/rsm_ui/qml/view/setting/SettingAQIdetailView.qml")},
        {QStringLiteral("SettingEvChargeTypeView"), Popup|Inquiry|Setting, QStringLiteral("qrc:/rsm_ui/qml/view/setting/SettingEvChargeTypeView.qml")},
        {QStringLiteral("SelfTestView"), Test, QStringLiteral("qrc:/rsm_ui/qml/view/setting/SelfTestView.qml")},
        {QStringLiteral("HiddenDiagSettingView"), Popup|Inquiry|Setting, QStringLiteral("qrc:/rsm_ui/qml/view/setting/HiddenDiagSettingView.qml")},
        {QStringLiteral("HiddenVsmDiagSettingView"), Popup|Inquiry|Setting, QStringLiteral("qrc:/rsm_ui/qml/view/setting/HiddenVsmDiagSettingView.qml")},
        {QStringLiteral("HiddenBoschSettingView"), Popup|Inquiry|Setting, QStringLiteral("qrc:/rsm_ui/qml/view/setting/HiddenBoschSettingView.qml")},
        {QStringLiteral("HiddenMenuLoginView"), Popup|Inquiry|Setting, QStringLiteral("qrc:/rsm_ui/qml/view/setting/HiddenMenuLoginView.qml")},
        {QStringLiteral("HiddenSettingFontDpView"), Popup|Inquiry|Setting, QStringLiteral("qrc:/rsm_ui/qml/view/setting/HiddenSettingFontDpView.qml")},        

        {QStringLiteral("ProgressIndicatorView"), Popup|Overlap, QStringLiteral("qrc:/rsm_ui/qml/view/util/ProgressIndicatorView.qml")},

        // widget
        {QStringLiteral("WidgetSmallView"), Widget, QStringLiteral("qrc:/rsm_ui/qml/view/widget/WidgetSmallView.qml")},
        {QStringLiteral("WidgetMediumView"), Widget, QStringLiteral("qrc:/rsm_ui/qml/view/widget/WidgetMediumView.qml")},
        {QStringLiteral("WidgetLargeView"), Widget, QStringLiteral("qrc:/rsm_ui/qml/view/widget/WidgetLargeView.qml")},
        {QStringLiteral("WidgetXlView"), Widget, QStringLiteral("qrc:/rsm_ui/qml/view/widget/WidgetXlView.qml")},
        {QStringLiteral("WidgetXxlView"), Widget, QStringLiteral("qrc:/rsm_ui/qml/view/widget/WidgetXxlView.qml")},
        {QStringLiteral("WidgetXxxlView"), Widget, QStringLiteral("qrc:/rsm_ui/qml/view/widget/WidgetXxxlView.qml")},

        // debug
        {QStringLiteral("TestGuidView"), Widget, QStringLiteral("qrc:/rsm_ui/qml/view/dbg/TestGuidView.qml")},        
        {QStringLiteral("TestFontView"), Widget, QStringLiteral("qrc:/rsm_ui/qml/view/dbg/TestFontView.qml")},

        // map view for map in cluster
        {QStringLiteral("MapInClusterView"), GuiFrame, QStringLiteral("qrc:/rsm_ui/qml/view/map/MapInClusterView.qml")},
        {QStringLiteral("MapInClusterSideView"), GuiFrame, QStringLiteral("qrc:/rsm_ui/qml/view/map/MapInClusterSideView.qml")},
		{QStringLiteral("MapInClusterHudView"), GuiFrame, QStringLiteral("qrc:/rsm_ui/qml/view/map/MapInClusterHudView.qml")},

        // Gadget shortcut icon test
        {QStringLiteral("GadgetShortcutIconView"), Popup|Inquiry|Setting, QStringLiteral("qrc:/rsm_ui/qml/view/setting/GadgetShortcutIconView.qml")},

    });

    // verification
    auto index = ViewRegistry::staticMetaObject.indexOfEnumerator( "ViewK" );
    auto enumerator = ViewRegistry::staticMetaObject.enumerator( index );
    for ( int i = 0, count = enumerator.keyCount(); i < count; i++ ) {
        auto enumStr = enumerator.valueToKey( i );
        if ( enumStr != mViewInfo[i].objectName ) {
            T_DEBUG( QString( "Failed to verify view info table. %1 != %2" )
                .arg( enumStr ).arg( mViewInfo[i].objectName ) );
            break;
        }
    }
}

ViewRegistry* ViewRegistry::instance()
{
    static ViewRegistry Instance;

    return &Instance;
}

const QVector<ViewRegistry::ViewInfo>& ViewRegistry::viewInfo()
{
    return mViewInfo;
}

int ViewRegistry::idFromObjectName( const QString& objectName )
{
    int ret = -1;

     for ( int id = 0, count = ViewRegistry::instance()->viewInfo().count(); id < count; id++ ) {
        auto info = ViewRegistry::instance()->viewInfo()[id];
        if ( info.objectName == objectName ) {
            ret = id;
        }
    }

    return ret;
}

int ViewRegistry::regist( QObject* view )
{
    int ret = -1;

    auto viewObjName = view->objectName();
    ret = idFromObjectName( viewObjName );
    if ( ret == -1 ) {
        T_DEBUG( QString( "Detected unknown view %1" ).arg( viewObjName ) );
    }
    else {
        QQmlProperty::write( view, QStringLiteral("regId"), ret );
    }

    return ret;
}

QString ViewRegistry::objectName( int viewId )
{
    return ViewRegistry::instance()->viewInfo()[viewId].objectName;
}

QString ViewRegistry::url( int viewId )
{
    return ViewRegistry::instance()->viewInfo()[viewId].url;
}

bool ViewRegistry::hasTraits( int viewId, int traits )
{
    return ( ViewRegistry::instance()->viewInfo()[viewId].traits & traits );
}
