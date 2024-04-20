#ifndef VSMMAPSETTINGANDTHEME_H
#define VSMMAPSETTINGANDTHEME_H

namespace SKT {

enum class VsmMapType {
    Global,
    Cluster,
    Gadget,
	ClusterHud
};

struct VsmMapSetting {
    int densitydpi;
    int tileDiskCache;
    int buildingFilter;
    float fovy;
    float naviPoiScale;
    float routeWidth;
    bool copycatBuilding;
    bool showPoi;
    bool showRoadOnly;
    bool poiScaleForce;
    bool poiOcclusion;
    bool renderSubwayline;
    bool routeTi;
    bool landmark;
    bool outline3d;
    bool outline2d;
    bool only2d;
};

struct VsmMapThemeCongestion {
    int noDataFillColor;
    int noDataStrokeColor;
    int goodFillColor;
    int goodStrokeColor;
    int slowFillColor;
    int slowStrokeColor;
    int badFillColor;
    int badStrokeColor;
};

struct VsmMapThemeTurnPoint {
    int fillColor;
    int strokeColor;
    int width;
    int floatGap;
};

struct VsmMapThemeAlternateRoute {
    int width;
    int fillColor;
    int strokeColor;
};

struct VsmMapThemeRouteLine {
    int width;
    int fillColor;
    int strokeColor;
    int passedFillColor;
    int passedStrokeColor;
    VsmMapThemeCongestion congestion;
    VsmMapThemeTurnPoint turnPoint;
};

struct VsmMapThemeRouteSelection {
    float width;
    int fillColor;
    int strokeColor;
    bool showCongestion;
    VsmMapThemeCongestion congestion;
};

struct VsmMapThemeRouteSelectionCandidate {
    VsmMapThemeRouteSelection defoult;
    VsmMapThemeRouteSelection selected;
};

struct VsmMapThemeOilPopup {
    float scale;
};

struct VsmMapThemeCavartar {
    float width;
};

struct VsmMapTheme {
    VsmMapThemeAlternateRoute alternateRoute;
    VsmMapThemeRouteLine routeLine;
    struct RouteSelection {
        VsmMapThemeRouteSelectionCandidate candidate[3];
    };
    RouteSelection routeSelection;
    VsmMapThemeOilPopup oilPopup;
    VsmMapThemeCavartar cavartar;
};

}

Q_DECLARE_METATYPE(SKT::VsmMapSetting)
Q_DECLARE_METATYPE(SKT::VsmMapTheme)

#endif // VSMMAPSETTINGANDTHEME_H
