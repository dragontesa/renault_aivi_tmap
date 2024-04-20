#ifndef VSMMAPCONSTANTS_H
#define VSMMAPCONSTANTS_H

#include "Constants.h"

namespace SKT {

class VsmMapConstants {
public:
    struct Setting {
        struct Global {
            static const int Densitydpi;
            static const int TileDiskCache;
            static const int BuildingFilter;
            static const float Fovy;
            static const float NaviPoiScale;
            static const float RouteWidth;
            static const bool CopycatBuilding;
            static const bool ShowPoi;
            static const bool PoiScaleForce;
            static const bool PoiOcclusion;
            static const bool RenderSubwayline;
            static const bool RouteTi;
            static const bool Landmark;
            static const bool Outline3d;
            static const bool Outline2d;
            static const bool Only2d;
        };
        struct Cluster {
            static const int Densitydpi;
            static const int TileDiskCache;
            static const int BuildingFilter;
            static const float Fovy;
            static const float NaviPoiScale;
            static const float RouteWidth;
            static const bool CopycatBuilding;
            static const bool ShowPoi;
            static const bool ShowRoadOnly;
            static const bool PoiScaleForce;
            static const bool PoiOcclusion;
            static const bool RenderSubwayline;
            static const bool RouteTi;
            static const bool Landmark;
            static const bool Outline3d;
            static const bool Outline2d;
            static const bool Only2d;
        };
        struct ClusterHud {
            static const bool ShowPoi;
            static const bool ShowRoadOnly;
            static const bool RenderSubwayline;
        };		
    };

    struct Theme {
        struct Day {
            struct AlternateRoute {
                static const int Width;
                static const int FillColor;
                static const int StrokeColor;
            };

            struct RouteLine {
                static const int Width;
                static const int FillColor;
                static const int StrokeColor;
                static const int PassedFillColor;
                static const int PassedStrokeColor;
                struct TurnPoint {
                    static const int Width;
                    static const int FloatGap;
                    static const int FillColor;
                    static const int StrokeColor;
                };
                struct Congestion {
                    static const int NoDataFillColor;
                    static const int NoDataStrokeColor;
                    static const int GoodFillColor;
                    static const int GoodStrokeColor;
                    static const int SlowFillColor;
                    static const int SlowStrokeColor;
                    static const int BadFillColor;
                    static const int BadStrokeColor;
                };
            };
        };

        struct Night {
            struct AlternateRoute {
                static const int Width;
                static const int FillColor;
                static const int StrokeColor;
            };
            struct RouteLine {
                static const int Width;
                static const int FillColor;
                static const int StrokeColor;
                static const int PassedFillColor;
                static const int PassedStrokeColor;
                struct TurnPoint {
                    static const int Width;
                    static const int FloatGap;
                    static const int FillColor;
                    static const int StrokeColor;
                };
                struct Congestion {
                    static const int NoDataFillColor;
                    static const int NoDataStrokeColor;
                    static const int GoodFillColor;
                    static const int GoodStrokeColor;
                    static const int SlowFillColor;
                    static const int SlowStrokeColor;
                    static const int BadFillColor;
                    static const int BadStrokeColor;
                };
            };
        };

        struct RouteSelection {
            struct Candidate0 {
                struct Default {
                    static const int Width;
                    static const int FillColor;
                    static const int StrokeColor;
                    static const int ShowCongestion;
                    struct Congestion {
                        static const int NoDataFillColor;
                        static const int NoDataStrokeColor;
                        static const int GoodFillColor;
                        static const int GoodStrokeColor;
                        static const int SlowFillColor;
                        static const int SlowStrokeColor;
                        static const int BadFillColor;
                        static const int BadStrokeColor;
                    };
                };
                struct Selected {
                    static const int Width;
                    static const int ShowCongestion;
                };
            };
            struct Candidate1 {
                struct Default {
                    static const int FillColor;
                };
                struct Selected {
                    static const int Width;
                    static const int ShowCongestion;
                };
            };
            struct Candidate2 {
                struct Default {
                    static const int FillColor;
                };
                struct Selected {
                    static const int Width;
                    static const int ShowCongestion;
                };
            };
        };
    };
};

}

#endif
