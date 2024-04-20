#ifndef VIEWREGISTRY_H
#define VIEWREGISTRY_H

namespace SKT {

class TMAP_RSM_UI_DLLSHARED_EXPORT ViewRegistry : public QObject
{
    Q_OBJECT
public:
    enum ViewTraitK {
        GuiFrame = 1 << 1,
        Popup = 1 << 2,
        RgPopup = 1 << 3,
        Inquiry = 1 << 4,
        Overlap = 1 << 5,
        Widget = 1 << 6,
        Setting = 1 << 7,
        Search = 1 << 8,
        Menu = 1 << 9,
        Test  = 1 << 10,
        DoNotCallActivateWhenSwitchGadgetToNavi  = 1 << 11,
        Dialog = 1 << 12
    };
    Q_ENUM( ViewTraitK )

    enum ViewK {
        HardKeyView=0,
        HighwayTbtView,
        LaneView,
        MapControlView,
        MapModeOptionView,
        MapView,
        NaviMenuView,
        RecentTileGridView,
        RouteCandidateView,
        RouteListView,
        RouteRegistView,
        RouteSettingView,
        RouteSummaryTestView,
        RouteSummaryView,
        ErrorView,
        SdiView,
        SimulationView,
        TbtView,
        TestView,
        MonkeyTestView,
        TestMultiMapView,
        Zone1View,
        Zone3View,
        ZoneApiTestView,
        OptionMenuView,
        CrossImageView,        
        HiPassLaneView,
        SummaryTbtView,
        SpeedMeterView,
		BusLaneView,
		TrafficBarView,
        ChangeDestinationView,
        CommonDialogView,

        // Screen

        FullscreenAlertView,

        // Keyboard

        KeyboardView,

        // Search

        AddressSearchView,
        BookmarkPageView,
        BookmarkPageEditView,
        BookmarkView,
        MapSearchView,
        RecentDestinationPageView,
        RecentDestinationView,
        RecentSearchEditView,
        RecentSearchPageView,
        NoResultPageView,
        SearchResultDetailView,
        SearchResultView,
        SearchTabView,
        TextSearchView,
        BookmarkRegistView,
        RegionSearchView,
        RecentDestinationEditView,
        IndustrySearchView,
        SearchResultOptionView,
        SearchAroundView,
        PoiTabView,

        // Setting        

        SettingAttentionSectionView,
        SettingCameraWarningSoundView,
        SettingCarIconView,
        SettingGpsInfoView,
        SettingGuidView,
        SettingGuidVoiceView,
        SettingInitView,
        SettingMainView,
        SettingMapCharSizeView,
        SettingMapUpdateView,
        SettingMapView,
        SettingNaviInfoView,
        SettingRouteAlternateView,
        SettingRouteSearchView,
        SettingSpeedResponseMapView,
        SettingMobileTmapView,
        SettingOilKindView,
        SettingOilBrandView,
        SettingOilAreaView,
        SettingVoiceGuidTimeView,
        SettingAQIdetailView,
        SettingEvChargeTypeView,
        SelfTestView,
        HiddenDiagSettingView,
        HiddenVsmDiagSettingView,
        HiddenBoschSettingView,
        HiddenMenuLoginView,
        HiddenSettingFontDpView,

        // Util

        ProgressIndicatorView,

        // Widget

        WidgetSmallView,
        WidgetMediumView,
        WidgetLargeView,
        WidgetXlView,
        WidgetXxlView,
        WidgetXxxlView,        

        // Debug
        TestGuidView,
        TestFontView,

        // map in cluster
        MapInClusterView,
        MapInClusterSideView,
		MapInClusterHudView,

        //Gadget shortcut icon Test
        GadgetShortcutIconView
    };
    Q_ENUM( ViewK )

    struct ViewInfo {
        QString objectName;
        int traits;
        QString url;        
    };

    static int regist( QObject* view );
    static QString objectName( int viewId );	
	static int idFromObjectName( const QString& objectName );
    static QString url( int viewId );
    static bool hasTraits( int viewId, int traits );

private:
    ViewRegistry();

    static ViewRegistry* instance();

    const QVector<ViewInfo>& viewInfo();

    QVector<ViewInfo> mViewInfo;
};

}

#endif // VIEWREGISTRY_H
