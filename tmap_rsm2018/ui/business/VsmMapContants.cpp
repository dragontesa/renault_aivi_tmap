#include "VsmMapContants.h"
#include "MapInClusterGmemTest.h".h"

#define DEFAULT_ROUTE_LINE_WIDTH (22.0f) // sp 단위
#define DEFAULT_ROUTE_TBT_FLOAT (5.0f) // tbt gap
#define DEFAULT_TURN_ARROW_SIZE (24.0f)
// 주행선
#define DEFAULT_ROUTE_BASIC_COLOR                           0x188bf4ff
#define DEFAULT_ROUTE_BASIC_OUTLINE_COLOR                   0x1e5e94ff
#define DEFAULT_ROUTE_BASIC_NIGHT_COLOR                     0x00b4ffff
#define DEFAULT_ROUTE_BASIC_NIGHT_OUTLINE_COLOR             0x036a94ff

// 대안경로선
#define DEFAULT_ROUTE_ALTERNATIVE_COLOR                     0x989aa0ff
#define DEFAULT_ROUTE_ALTERNATIVE_OUTLINE_COLOR             0x7d7f84ff
#define DEFAULT_ROUTE_ALTERNATIVE_NIGHT_COLOR               0x9fa6adff
#define DEFAULT_ROUTE_ALTERNATIVE_NIGHT_OUTLINE_COLOR       0x767d86ff

// 지나온 경로선
#define DEFAULT_ROUTE_PASSED_COLOR                          0xc6c6c6ff
#define DEFAULT_ROUTE_PASSED_OUTLINE_COLOR                  0x747474ff
#define DEFAULT_ROUTE_PASSED_NIGHT_COLOR                    0xc6c6c6ff
#define DEFAULT_ROUTE_PASSED_NIGHT_OUTLINE_COLOR            0x707070ff

// Turn Arrow
#define DEFAULT_TURNARROW_COLOR                             0xffffffff
#define DEFAULT_TURNARROW_OUTLINE_COLOR                     0x787878ff
#define DEFAULT_TURNARROW_NIGHT_COLOR                       0xffffffff
#define DEFAULT_TURNARROW_NIGHT_OUTLINE_COLOR               0x787878ff

#define DEFAULT_TRAFFIC_NODATA_COLOR                        0x6c6c6cff
#define DEFAULT_TRAFFIC_NODATA_OUTLINE_COLOR                0x464646ff
#define DEFAULT_TRAFFIC_GOOD_COLOR                          0x13bc62ff
#define DEFAULT_TRAFFIC_GOOD_OUTLINE_COLOR                  0x0c753dff
#define DEFAULT_TRAFFIC_SLOW_COLOR                          0xfb9a0dff
#define DEFAULT_TRAFFIC_SLOW_OUTLINE_COLOR                  0x9f5804ff
#define DEFAULT_TRAFFIC_BAD_COLOR                           0xf1522aff
#define DEFAULT_TRAFFIC_BAD_OUTLINE_COLOR                   0x823a2fff

#define DEFAULT_TRAFFIC_NODATA_NIGHT_COLOR                  0x727272ff
#define DEFAULT_TRAFFIC_NODATA_NIGHT_OUTLINE_COLOR          0x464646ff
#define DEFAULT_TRAFFIC_GOOD_NIGHT_COLOR                    0x13bc62ff
#define DEFAULT_TRAFFIC_GOOD_NIGHT_OUTLINE_COLOR            0x046f36ff
#define DEFAULT_TRAFFIC_SLOW_NIGHT_COLOR                    0xfb9a0dff
#define DEFAULT_TRAFFIC_SLOW_NIGHT_OUTLINE_COLOR            0x9f5804ff
#define DEFAULT_TRAFFIC_BAD_NIGHT_COLOR                     0xf1522aff
#define DEFAULT_TRAFFIC_BAD_NIGHT_OUTLINE_COLOR             0x823a2fff

using namespace SKT;

const int VsmMapConstants::Setting::Global::Densitydpi = 160;
const int VsmMapConstants::Setting::Global::TileDiskCache = 2;
const int VsmMapConstants::Setting::Global::BuildingFilter = 2;
const float VsmMapConstants::Setting::Global::Fovy = 45.0;
const float VsmMapConstants::Setting::Global::NaviPoiScale = 1.0;
const float VsmMapConstants::Setting::Global::RouteWidth = 3.0;
const bool VsmMapConstants::Setting::Global::CopycatBuilding = true;
const bool VsmMapConstants::Setting::Global::ShowPoi = true;
const bool VsmMapConstants::Setting::Global::PoiScaleForce = false;
const bool VsmMapConstants::Setting::Global::PoiOcclusion = true;
const bool VsmMapConstants::Setting::Global::RenderSubwayline = true;
const bool VsmMapConstants::Setting::Global::RouteTi = true;
const bool VsmMapConstants::Setting::Global::Landmark = true;
const bool VsmMapConstants::Setting::Global::Outline3d = false;
const bool VsmMapConstants::Setting::Global::Outline2d = true;
const bool VsmMapConstants::Setting::Global::Only2d = false;

// old
//const int VsmMapConstants::Setting::Cluster::Densitydpi = 160;
//const int VsmMapConstants::Setting::Cluster::TileDiskCache = 2;
//const int VsmMapConstants::Setting::Cluster::BuildingFilter = 2;
//const float VsmMapConstants::Setting::Cluster::Fovy = 45.0;
//const float VsmMapConstants::Setting::Cluster::NaviPoiScale = 1.0;
//const float VsmMapConstants::Setting::Cluster::RouteWidth = 3.0;
//const bool VsmMapConstants::Setting::Cluster::CopycatBuilding = true;
//const bool VsmMapConstants::Setting::Cluster::ShowPoi = true;
//const bool VsmMapConstants::Setting::Cluster::ShowRoadOnly = false;
//const bool VsmMapConstants::Setting::Cluster::PoiScaleForce = false;
//const bool VsmMapConstants::Setting::Cluster::PoiOcclusion = true;
//const bool VsmMapConstants::Setting::Cluster::RenderSubwayline = true;
//const bool VsmMapConstants::Setting::Cluster::RouteTi = true;
//const bool VsmMapConstants::Setting::Cluster::Landmark = true;
//const bool VsmMapConstants::Setting::Cluster::Outline3d = false;
//const bool VsmMapConstants::Setting::Cluster::Outline2d = true;
//const bool VsmMapConstants::Setting::Cluster::Only2d = false;

// opt 1 - Landmark, CopycatBuilding
const int VsmMapConstants::Setting::Cluster::Densitydpi = 160;
const int VsmMapConstants::Setting::Cluster::TileDiskCache = 2;
const int VsmMapConstants::Setting::Cluster::BuildingFilter = 2;
const float VsmMapConstants::Setting::Cluster::Fovy = 45.0;
const float VsmMapConstants::Setting::Cluster::NaviPoiScale = 1.0;
const float VsmMapConstants::Setting::Cluster::RouteWidth = 3.0;
const bool VsmMapConstants::Setting::Cluster::CopycatBuilding = false;
const bool VsmMapConstants::Setting::Cluster::ShowPoi = true;
const bool VsmMapConstants::Setting::Cluster::ShowRoadOnly = false;
const bool VsmMapConstants::Setting::Cluster::PoiScaleForce = false;
const bool VsmMapConstants::Setting::Cluster::PoiOcclusion = true;
const bool VsmMapConstants::Setting::Cluster::RenderSubwayline = true;
const bool VsmMapConstants::Setting::Cluster::RouteTi = true;
const bool VsmMapConstants::Setting::Cluster::Landmark = false;
const bool VsmMapConstants::Setting::Cluster::Outline3d = false;
const bool VsmMapConstants::Setting::Cluster::Outline2d = true;
const bool VsmMapConstants::Setting::Cluster::Only2d = false;

const bool VsmMapConstants::Setting::ClusterHud::ShowPoi = false;
const bool VsmMapConstants::Setting::ClusterHud::ShowRoadOnly = true;
const bool VsmMapConstants::Setting::ClusterHud::RenderSubwayline = false;

const int VsmMapConstants::Theme::RouteSelection::Candidate0::Default::Width = 6.0f;
const int VsmMapConstants::Theme::RouteSelection::Candidate0::Default::FillColor = 0xff543aff;
const int VsmMapConstants::Theme::RouteSelection::Candidate0::Default::StrokeColor = 0x00000047;
const int VsmMapConstants::Theme::RouteSelection::Candidate0::Default::ShowCongestion = false;
const int VsmMapConstants::Theme::RouteSelection::Candidate0::Default::Congestion::NoDataFillColor = DEFAULT_TRAFFIC_NODATA_COLOR;
const int VsmMapConstants::Theme::RouteSelection::Candidate0::Default::Congestion::NoDataStrokeColor = DEFAULT_TRAFFIC_NODATA_OUTLINE_COLOR;
const int VsmMapConstants::Theme::RouteSelection::Candidate0::Default::Congestion::GoodFillColor = DEFAULT_TRAFFIC_GOOD_COLOR;
const int VsmMapConstants::Theme::RouteSelection::Candidate0::Default::Congestion::GoodStrokeColor = DEFAULT_TRAFFIC_GOOD_OUTLINE_COLOR;
const int VsmMapConstants::Theme::RouteSelection::Candidate0::Default::Congestion::SlowFillColor = DEFAULT_TRAFFIC_SLOW_COLOR;
const int VsmMapConstants::Theme::RouteSelection::Candidate0::Default::Congestion::SlowStrokeColor = DEFAULT_TRAFFIC_SLOW_OUTLINE_COLOR;
const int VsmMapConstants::Theme::RouteSelection::Candidate0::Default::Congestion::BadFillColor = DEFAULT_TRAFFIC_BAD_COLOR;
const int VsmMapConstants::Theme::RouteSelection::Candidate0::Default::Congestion::BadStrokeColor = DEFAULT_TRAFFIC_BAD_OUTLINE_COLOR;
const int VsmMapConstants::Theme::RouteSelection::Candidate0::Selected::Width = 12.0f;
const int VsmMapConstants::Theme::RouteSelection::Candidate0::Selected::ShowCongestion = true;

const int VsmMapConstants::Theme::RouteSelection::Candidate1::Default::FillColor = 0x8cbe00ff;
const int VsmMapConstants::Theme::RouteSelection::Candidate1::Selected::Width = 12.0f;
const int VsmMapConstants::Theme::RouteSelection::Candidate1::Selected::ShowCongestion = true;

const int VsmMapConstants::Theme::RouteSelection::Candidate2::Default::FillColor = 0x078df7ff;
const int VsmMapConstants::Theme::RouteSelection::Candidate2::Selected::Width = 12.0f;
const int VsmMapConstants::Theme::RouteSelection::Candidate2::Selected::ShowCongestion = true;

const int VsmMapConstants::Theme::Day::AlternateRoute::Width = DEFAULT_ROUTE_LINE_WIDTH;
const int VsmMapConstants::Theme::Day::AlternateRoute::FillColor = DEFAULT_ROUTE_ALTERNATIVE_COLOR;
const int VsmMapConstants::Theme::Day::AlternateRoute::StrokeColor = DEFAULT_ROUTE_ALTERNATIVE_OUTLINE_COLOR;
const int VsmMapConstants::Theme::Day::RouteLine::Width = DEFAULT_ROUTE_LINE_WIDTH;
const int VsmMapConstants::Theme::Day::RouteLine::FillColor = DEFAULT_ROUTE_BASIC_COLOR;
const int VsmMapConstants::Theme::Day::RouteLine::StrokeColor = DEFAULT_ROUTE_BASIC_OUTLINE_COLOR;
const int VsmMapConstants::Theme::Day::RouteLine::PassedFillColor = DEFAULT_ROUTE_PASSED_COLOR;
const int VsmMapConstants::Theme::Day::RouteLine::PassedStrokeColor = DEFAULT_ROUTE_PASSED_OUTLINE_COLOR;
const int VsmMapConstants::Theme::Day::RouteLine::TurnPoint::Width = DEFAULT_TURN_ARROW_SIZE;
const int VsmMapConstants::Theme::Day::RouteLine::TurnPoint::FloatGap = DEFAULT_ROUTE_TBT_FLOAT;
const int VsmMapConstants::Theme::Day::RouteLine::TurnPoint::FillColor = DEFAULT_TURNARROW_COLOR;
const int VsmMapConstants::Theme::Day::RouteLine::TurnPoint::StrokeColor = DEFAULT_TURNARROW_OUTLINE_COLOR;
const int VsmMapConstants::Theme::Day::RouteLine::Congestion::NoDataFillColor = DEFAULT_TRAFFIC_NODATA_COLOR;
const int VsmMapConstants::Theme::Day::RouteLine::Congestion::NoDataStrokeColor = DEFAULT_TRAFFIC_NODATA_OUTLINE_COLOR;
const int VsmMapConstants::Theme::Day::RouteLine::Congestion::GoodFillColor = DEFAULT_TRAFFIC_GOOD_COLOR;
const int VsmMapConstants::Theme::Day::RouteLine::Congestion::GoodStrokeColor = DEFAULT_TRAFFIC_GOOD_OUTLINE_COLOR;
const int VsmMapConstants::Theme::Day::RouteLine::Congestion::SlowFillColor = DEFAULT_TRAFFIC_SLOW_COLOR;
const int VsmMapConstants::Theme::Day::RouteLine::Congestion::SlowStrokeColor = DEFAULT_TRAFFIC_SLOW_OUTLINE_COLOR;
const int VsmMapConstants::Theme::Day::RouteLine::Congestion::BadFillColor = DEFAULT_TRAFFIC_BAD_COLOR;
const int VsmMapConstants::Theme::Day::RouteLine::Congestion::BadStrokeColor = DEFAULT_TRAFFIC_BAD_OUTLINE_COLOR;

const int VsmMapConstants::Theme::Night::AlternateRoute::Width = DEFAULT_ROUTE_LINE_WIDTH;
const int VsmMapConstants::Theme::Night::AlternateRoute::FillColor = DEFAULT_ROUTE_ALTERNATIVE_NIGHT_COLOR;
const int VsmMapConstants::Theme::Night::AlternateRoute::StrokeColor = DEFAULT_ROUTE_ALTERNATIVE_NIGHT_OUTLINE_COLOR;
const int VsmMapConstants::Theme::Night::RouteLine::Width = DEFAULT_ROUTE_LINE_WIDTH;
const int VsmMapConstants::Theme::Night::RouteLine::FillColor = DEFAULT_ROUTE_BASIC_NIGHT_COLOR;
const int VsmMapConstants::Theme::Night::RouteLine::StrokeColor = DEFAULT_ROUTE_BASIC_NIGHT_OUTLINE_COLOR;
const int VsmMapConstants::Theme::Night::RouteLine::PassedFillColor = DEFAULT_ROUTE_PASSED_NIGHT_COLOR;
const int VsmMapConstants::Theme::Night::RouteLine::PassedStrokeColor = DEFAULT_ROUTE_PASSED_NIGHT_OUTLINE_COLOR;
const int VsmMapConstants::Theme::Night::RouteLine::TurnPoint::Width = DEFAULT_TURN_ARROW_SIZE;
const int VsmMapConstants::Theme::Night::RouteLine::TurnPoint::FloatGap = DEFAULT_ROUTE_TBT_FLOAT;
const int VsmMapConstants::Theme::Night::RouteLine::TurnPoint::FillColor = DEFAULT_TURNARROW_NIGHT_COLOR;
const int VsmMapConstants::Theme::Night::RouteLine::TurnPoint::StrokeColor = DEFAULT_TURNARROW_NIGHT_OUTLINE_COLOR;
const int VsmMapConstants::Theme::Night::RouteLine::Congestion::NoDataFillColor = DEFAULT_TRAFFIC_NODATA_NIGHT_COLOR;
const int VsmMapConstants::Theme::Night::RouteLine::Congestion::NoDataStrokeColor = DEFAULT_TRAFFIC_NODATA_NIGHT_OUTLINE_COLOR;
const int VsmMapConstants::Theme::Night::RouteLine::Congestion::GoodFillColor = DEFAULT_TRAFFIC_GOOD_NIGHT_COLOR;
const int VsmMapConstants::Theme::Night::RouteLine::Congestion::GoodStrokeColor = DEFAULT_TRAFFIC_GOOD_NIGHT_OUTLINE_COLOR;
const int VsmMapConstants::Theme::Night::RouteLine::Congestion::SlowFillColor = DEFAULT_TRAFFIC_SLOW_NIGHT_COLOR;
const int VsmMapConstants::Theme::Night::RouteLine::Congestion::SlowStrokeColor = DEFAULT_TRAFFIC_SLOW_NIGHT_OUTLINE_COLOR;
const int VsmMapConstants::Theme::Night::RouteLine::Congestion::BadFillColor = DEFAULT_TRAFFIC_BAD_NIGHT_COLOR;
const int VsmMapConstants::Theme::Night::RouteLine::Congestion::BadStrokeColor = DEFAULT_TRAFFIC_BAD_NIGHT_OUTLINE_COLOR;
