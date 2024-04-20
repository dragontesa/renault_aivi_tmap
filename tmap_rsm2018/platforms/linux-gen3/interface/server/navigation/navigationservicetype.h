#ifndef NAVIGATIONSERVICETYPE_H
#define NAVIGATIONSERVICETYPE_H

#include <QDBusArgument>

/**
 * @brief The NavDataCompatibility enum
 */
enum NavDataCompatibility {
    NavDataCompatibility_NAVDATA_COMPATIBLE = 0,
    NavDataCompatibility_NAVDATA_INCOMPATIBLE,
    NavDataCompatibility_NAVDATA_COMPATIBILITY_UNKNOWN,
};

/**
 * @brief The NavStatus enum
 */
enum NavStatus {
    NavStatus_NAVSTATUS_UNKNOWN = 0,
    NavStatus_NAVSTATUS_BUSY,
    NavStatus_NAVSTATUS_NAVDATA_UPDATING,
    NAVSTATUS_MAP_RECOVERY_MODE,
    NavStatus_NAVSTATUS_IDLE,
    NavStatus_NAVSTATUS_GUIDANCE_ACTIVE,
    NavStatus_NAVSTATUS_CALCULATING_ROUTE,
    NavStatus_NAVSTATUS_GUIDANCE_ACTIVE_OFFBOARD
};

/**
 * @brief The RoadClass enum
 */
enum RoadClass {
    RoadClass_ROAD_CLASS_NONE = 0,
    RoadClass_ROAD_CLASS_MOTORWAY,
    RoadClass_ROAD_CLASS_FREEWAY,
    RoadClass_ROAD_CLASS_HIGHWAY,
    RoadClass_ROAD_CLASS_ARTERIAL,
    RoadClass_ROAD_CLASS_COLLECTOR,
    RoadClass_ROAD_CLASS_LOCAL,
    RoadClass_ROAD_CLASS_ADDRESSABLESTREET,
    RoadClass_ROAD_CLASS_TRAIL
};


/**
 * @brief The DistanceUnit enum
 */
enum DistanceUnit {
    DistanceUnit_METERS = 0,
    DistanceUnit_KILOMETERS,
    DistanceUnit_MILES
};

/**
 * @brief The ManeuverSymbol enum
 */
enum ManeuverSymbol {
    ManeuverSymbol_MANEUVER_SYMBOL_NO_SYMBOL = 0,
    ManeuverSymbol_MANEUVER_SYMBOL_NO_INFO,
    ManeuverSymbol_MANEUVER_SYMBOL_FOLLOW,
    ManeuverSymbol_MANEUVER_SYMBOL_RECALC,
    ManeuverSymbol_MANEUVER_SYMBOL_DEST_REACHED,
    ManeuverSymbol_MANEUVER_SYMBOL_DEST_REACHED_LEFT,
    ManeuverSymbol_MANEUVER_SYMBOL_DEST_REACHED_RIGHT,
    ManeuverSymbol_MANEUVER_SYMBOL_INTERMEDIATEDEST_REACHED,
    ManeuverSymbol_MANEUVER_SYMBOL_INTERMEDIATEDEST_REACHED_LEFT,
    ManeuverSymbol_MANEUVER_SYMBOL_INTERMEDIATEDEST_REACHED_RIGHT,
    ManeuverSymbol_MANEUVER_SYMBOL_EXIT_LEFT,                                       // 10
    ManeuverSymbol_MANEUVER_SYMBOL_EXIT_RIGHT,
    ManeuverSymbol_MANEUVER_SYMBOL_ROUNDABOUT_TRAFFICSENSE_LEFT_STRAIGHT,
    ManeuverSymbol_MANEUVER_SYMBOL_ROUNDABOUT_TRAFFICSENSE_LEFT_HALFRIGHT,
    ManeuverSymbol_MANEUVER_SYMBOL_ROUNDABOUT_TRAFFICSENSE_LEFT_RIGHT,
    ManeuverSymbol_MANEUVER_SYMBOL_ROUNDABOUT_TRAFFICSENSE_LEFT_SHARPRIGHT,
    ManeuverSymbol_MANEUVER_SYMBOL_ROUNDABOUT_TRAFFICSENSE_LEFT_REVERSE,
    ManeuverSymbol_MANEUVER_SYMBOL_ROUNDABOUT_TRAFFICSENSE_LEFT_SHARPLEFT,
    ManeuverSymbol_MANEUVER_SYMBOL_ROUNDABOUT_TRAFFICSENSE_LEFT_LEFT,
    ManeuverSymbol_MANEUVER_SYMBOL_ROUNDABOUT_TRAFFICSENSE_LEFT_HALFLEFT,
    ManeuverSymbol_MANEUVER_SYMBOL_ROUNDABOUT_TRAFFICSENSE_RIGHT_STRAIGHT,          // 20
    ManeuverSymbol_MANEUVER_SYMBOL_ROUNDABOUT_TRAFFICSENSE_RIGHT_HALFRIGHT,
    ManeuverSymbol_MANEUVER_SYMBOL_ROUNDABOUT_TRAFFICSENSE_RIGHT_RIGHT,
    ManeuverSymbol_MANEUVER_SYMBOL_ROUNDABOUT_TRAFFICSENSE_RIGHT_SHARPRIGHT,
    ManeuverSymbol_MANEUVER_SYMBOL_ROUNDABOUT_TRAFFICSENSE_RIGHT_REVERSE,
    ManeuverSymbol_MANEUVER_SYMBOL_ROUNDABOUT_TRAFFICSENSE_RIGHT_SHARPLEFT,
    ManeuverSymbol_MANEUVER_SYMBOL_ROUNDABOUT_TRAFFICSENSE_RIGHT_LEFT,
    ManeuverSymbol_MANEUVER_SYMBOL_ROUNDABOUT_TRAFFICSENSE_RIGHT_HALFLEFT,
    ManeuverSymbol_MANEUVER_SYMBOL_STRAIGHT,
    ManeuverSymbol_MANEUVER_SYMBOL_TURN_HALFRIGHT,
    ManeuverSymbol_MANEUVER_SYMBOL_TURN_RIGHT,                                      // 30
    ManeuverSymbol_MANEUVER_SYMBOL_TURN_SHARPRIGHT,
    ManeuverSymbol_MANEUVER_SYMBOL_TURN_SHARPLEFT,
    ManeuverSymbol_MANEUVER_SYMBOL_TURN_LEFT,
    ManeuverSymbol_MANEUVER_SYMBOL_TURN_HALFLEFT,
    ManeuverSymbol_MANEUVER_SYMBOL_UTURN_LEFT,
    ManeuverSymbol_MANEUVER_SYMBOL_UTURN_RIGHT,
    ManeuverSymbol_MANEUVER_SYMBOL_KEEP_LEFT,
    ManeuverSymbol_MANEUVER_SYMBOL_KEEP_RIGHT,
    ManeuverSymbol_MANEUVER_SYMBOL_DEST_DIRECTION_001,
    ManeuverSymbol_MANEUVER_SYMBOL_DEST_DIRECTION_009,                              // 40
    ManeuverSymbol_MANEUVER_SYMBOL_DEST_DIRECTION_025,
    ManeuverSymbol_MANEUVER_SYMBOL_DEST_DIRECTION_041,
    ManeuverSymbol_MANEUVER_SYMBOL_DEST_DIRECTION_057,
    ManeuverSymbol_MANEUVER_SYMBOL_DEST_DIRECTION_073,
    ManeuverSymbol_MANEUVER_SYMBOL_DEST_DIRECTION_089,
    ManeuverSymbol_MANEUVER_SYMBOL_DEST_DIRECTION_105,
    ManeuverSymbol_MANEUVER_SYMBOL_DEST_DIRECTION_121,
    ManeuverSymbol_MANEUVER_SYMBOL_DEST_DIRECTION_137,
    ManeuverSymbol_MANEUVER_SYMBOL_DEST_DIRECTION_153,
    ManeuverSymbol_MANEUVER_SYMBOL_DEST_DIRECTION_169,                              // 50
    ManeuverSymbol_MANEUVER_SYMBOL_DEST_DIRECTION_185,
    ManeuverSymbol_MANEUVER_SYMBOL_DEST_DIRECTION_201,
    ManeuverSymbol_MANEUVER_SYMBOL_DEST_DIRECTION_217,
    ManeuverSymbol_MANEUVER_SYMBOL_DEST_DIRECTION_233,
    ManeuverSymbol_MANEUVER_SYMBOL_RIGHT_EXIT_CLOCKWISE_TO_LEFT,
    ManeuverSymbol_MANEUVER_SYMBOL_RIGHT_LANE_ENTER_EXPRESSWAY,
    ManeuverSymbol_MANEUVER_SYMBOL_LEFT_LANE_ENTER_EXPRESSWAY,
    ManeuverSymbol_MANEUVER_SYMBOL_ENTRY_STRAIGHT,
    ManeuverSymbol_MANEUVER_SYMBOL_EXIT_STRAIGHT,
    ManeuverSymbol_MANEUVER_SYMBOL_UNDERGROUND_ROAD,                                // 60
    ManeuverSymbol_MANEUVER_SYMBOL_OVERPASS,
    ManeuverSymbol_MANEUVER_SYMBOL_TUNNEL,
    ManeuverSymbol_MANEUVER_SYMBOL_RIGHT_BESIDE_UNDERGROUND_ROAD,
    ManeuverSymbol_MANEUVER_SYMBOL_LEFT_UNDERGROUND_ROAD,
    ManeuverSymbol_MANEUVER_SYMBOL_BRIDGE,
    ManeuverSymbol_MANEUVER_SYMBOL_ENTER_TOLLGATE,
    ManeuverSymbol_MANEUVER_SYMBOL_RIGHT_BESIDE_OVERPASS,
    ManeuverSymbol_MANEUVER_SYMBOL_LEFT_OVERPASS,
    ManeuverSymbol_MANEUVER_SYMBOL_REST_AREA,
    ManeuverSymbol_MANEUVER_SYMBOL_ENTRY_RIGHT,                                     // 70
    ManeuverSymbol_MANEUVER_SYMBOL_ENTRY_LEFT,
    //KOREA Symbol
// 100번 이후 값은 사용하지 않으므로 주석처리함. 2019.3.11 by hheo
//    ManeuverSymbol_MANEUVER_SYMBOL_DIR_5 = 100,
//    ManeuverSymbol_MANEUVER_SYMBOL_DIR_3,
//    ManeuverSymbol_MANEUVER_SYMBOL_DIR_1,
//    ManeuverSymbol_MANEUVER_SYMBOL_DIR_12,
//    ManeuverSymbol_MANEUVER_SYMBOL_DIR_11,
//    ManeuverSymbol_MANEUVER_SYMBOL_DIR_9,
//    ManeuverSymbol_MANEUVER_SYMBOL_DIR_7,
//    ManeuverSymbol_MANEUVER_SYMBOL_DIR_6,
//    ManeuverSymbol_MANEUVER_SYMBOL_PTURN,
//    ManeuverSymbol_MANEUVER_SYMBOL_EXP_IN_RIGHT,
//    ManeuverSymbol_MANEUVER_SYMBOL_EXP_IN_LEFT,
//    ManeuverSymbol_MANEUVER_SYMBOL_EXP_IN_STR,
//    ManeuverSymbol_MANEUVER_SYMBOL_EXP_EXIT_RIGHT,
//    ManeuverSymbol_MANEUVER_SYMBOL_EXP_EXIT_LEFT,
//    ManeuverSymbol_MANEUVER_SYMBOL_EXP_EXIT_STR,
//    ManeuverSymbol_MANEUVER_SYMBOL_UNDER_ROAD,
//    ManeuverSymbol_MANEUVER_SYMBOL_OVERPASS,
//    ManeuverSymbol_MANEUVER_SYMBOL_TUNNEL,
//    ManeuverSymbol_MANEUVER_SYMBOL_BESIDE_UNDER_ROAD,
//    ManeuverSymbol_MANEUVER_SYMBOL_BRIDGE = 120,
//    ManeuverSymbol_MANEUVER_SYMBOL_TOLLGATE,
//    ManeuverSymbol_MANEUVER_SYMBOL_BESIDE_OVERPASS,
//    ManeuverSymbol_MANEUVER_SYMBOL_REST_AREA = 124,
};

/**
 * @brief The ManeuverSymbol enum
 */
enum RGTurnCode {
    IDV_RG_turn_pt_map         = 1,         //         도곽에 의한 점
    IDV_RG_turn_pt_tile        = 2,         //         타일에 의한 점
    IDV_RG_turn_not_ser_exp    = 3,         //         고속도로에 의한 안내없음
    IDV_RG_turn_not_ser_normal = 4,         //         일반도로에 의한 안내없음
    IDV_RG_turn_not_ser_spec  = 5,         //         특수한 경우 안내없음
    IDV_RG_str_right          = 6,         //         Y자 오른쪽 안내없음
    IDV_RG_str_left           = 7,         //         Y자 왼쪽 안내없음
    IDV_RG_turn_str           = 11,        //         직진
    IDV_RG_turn_left          = 12,       //         좌회전
    IDV_RG_turn_right         = 13,        //         우회전
    IDV_RG_turn_Uturn         = 14,        //         U-turn
    IDV_RG_turn_pturn         = 15,        //         P-turn
    IDV_RG_turn_left_8        = 16,        //         8시 방향 좌회전
    IDV_RG_turn_left_10       = 17,        //         10시 방향 좌회전
    IDV_RG_turn_rignt_2       = 18,        //         2시 방향 우회전
    IDV_RG_turn_rignt_4       = 19,        //         4시 방향 우회전
    IDV_RG_turn_dir_1         = 31,        //         1시 방향
    IDV_RG_turn_dir_2         = 32,        //         2시 방향
    IDV_RG_turn_dir_3         = 33,        //         3시 방향
    IDV_RG_turn_dir_4         = 34,        //         4시 방향
    IDV_RG_turn_dir_5         = 35,        //         5시 방향
    IDV_RG_turn_dir_6         = 36,        //         6시 방향
    IDV_RG_turn_dir_7         = 37,        //         7시 방향
    IDV_RG_turn_dir_8         = 38,        //         8시 방향
    IDV_RG_turn_dir_9         = 39,        //         9시 방향
    IDV_RG_turn_dir_10        = 40,        //         10시 방향
    IDV_RG_turn_dir_11        = 41,        //         11시 방향
    IDV_RG_turn_dir_12        = 42,        //         12시 방향
    IDV_RG_turn_str_right     = 43,        //         오른쪽
    IDV_RG_turn_str_left      = 44,        //         왼쪽
    IDV_RG_lane_str   = 51,         //         직진방향
    IDV_RG_lane_left  = 52,         //         왼쪽차선
    IDV_RG_lane_right = 53,         //         오른쪽차선
    IDV_RG_lane_1   = 54,           //         1차선
    IDV_RG_lane_2   = 55,           //         2차선
    IDV_RG_lane_3   = 56,           //         3차선
    IDV_RG_lane_4   = 57,           //         4차선
    IDV_RG_lane_5   = 58,           //         5차선
    IDV_RG_lane_6   = 59,           //         6차선
    IDV_RG_lane_7   = 60,           //         7차선
    IDV_RG_lane_8   = 61,           //         8차선
    IDV_RG_lane_9   = 62,           //         9차선
    IDV_RG_lane_10  = 63,           //         10차선
    IDV_RG_first_exit = 71,         //         첫번째 출구
    IDV_RG_sec_exit = 72,           //         두번째 출구
    IDV_RG_first_lane_right  = 73,  //         첫번째 오른쪽 길
    IDV_RG_sec_lane_right    = 74,  //  두번째 오른쪽 길
    IDV_RG_first_lane_left   = 75,  //         첫번째 왼쪽 길
    IDV_RG_sec_lane_left     = 76,  //         두번째 왼쪽 길
    IDV_RG_Highpath_left     = 80,  // 하이패스차로는 좌측에 있습니다.
    IDV_RG_Highpath_right    = 81,  // 하이패스차로는 우측에 있습니다.
    IDV_RG_Highpath_center   = 82,  // 하이패스차로는 중앙에 있습니다.
    IDV_RG_exp_in_right      = 101, //         오른쪽방향에 고속도로 입구
    IDV_RG_exp_in_left       = 102, //         왼쪽방향에 고속도로 입구
    IDV_RG_exp_in_str        = 103, //         직진방향에 고속도로 입구
    IDV_RG_exp_out_right     = 104, //         오른쪽방향에 고속도로 출구
    IDV_RG_exp_out_left      = 105, //         왼쪽방향에 고속도로 출구
    IDV_RG_exp_out_str       = 106, //         직진방향에 고속도로 출구
    IDV_RG_saexp_in_right    = 111, //         오른쪽방향에 도시고속도로 입구
    IDV_RG_saexp_in_left     = 112, //         왼쪽방향에 도시고속도로 입구
    IDV_RG_saexp_in_str      = 113, //         직진방향에 도시고속도로 입구
    IDV_RG_saexp_out_right   = 114, //         오른쪽방향에 도시고속도로 출구
    IDV_RG_saexp_out_left    = 115, //         왼쪽방향에 도시고속도로 출구
    IDV_RG_saexp_out_str     = 116, //         직진방향에 도시고속도로 출구
    IDV_RG_road_right        = 117, //         오른쪽 도로
    IDV_RG_road_left  = 118,        //         왼쪽 도로
    IDV_RG_under_in   = 119,        //         지하도로
    IDV_RG_sky_in     = 120,        //         고가도로
    IDV_RG_tunnel_in  = 121,        //         터널
    IDV_RG_bridge     = 122,        //         교량
    IDV_RG_under_out  = 123,        //         지하도로 옆
    IDV_RG_sky_out    = 124,        //         고가도로 옆
    IDV_RG_dir_1      = 131,        //         1시방향
    IDV_RG_dir_2      = 132,        //         2시방향
    IDV_RG_dir_3      = 133,        //         3시방향
    IDV_RG_dir_4      = 134,        //         4시방향
    IDV_RG_dir_5      = 135,        //         5시방향
    IDV_RG_dir_6      = 136,        //         6시방향
    IDV_RG_dir_7      = 137,        //         7시방향
    IDV_RG_dir_8      = 138,        //         8시방향
    IDV_RG_dir_9      = 139,        //         9시방향
    IDV_RG_dir_10     = 140,        //         10시방향
    IDV_RG_dir_11     = 141,        //         11시방향
    IDV_RG_dir_12     = 142,        //         12시방향
    IDV_RG_rest       = 150,        //         졸음 쉼터
    IDV_RG_sa         = 151,        //         휴게소
    IDV_RG_sa_in        = 152,      // 휴게소 진입
    IDV_RG_Tol_exp      = 153,      //톨게이트 고속
    IDV_RG_Tol_normal   = 154,      //톨게이트 일반
    IDV_RG_ferry_in     = 155,      // 페리항로 진입
    IDV_RG_ferry_out    = 156,      // 페리항로 진출
    IDV_RG_startdir_1   = 161,      // 경로시작점안내           1시방향
    IDV_RG_startdir_2   = 162,      // 경로시작점안내           2시방향
    IDV_RG_startdir_3   = 163,      // 경로시작점안내           3시방향
    IDV_RG_startdir_4   = 164,      // 경로시작점안내           4시방향
    IDV_RG_startdir_5   = 165,      // 경로시작점안내           5시방향
    IDV_RG_startdir_6   = 166,      // 경로시작점안내           6시방향
    IDV_RG_startdir_7   = 167,      // 경로시작점안내           7시방향
    IDV_RG_startdir_8   = 168,      // 경로시작점안내           8시방향
    IDV_RG_startdir_9   = 169,      // 경로시작점안내           9시방향
    IDV_RG_startdir_10  = 170,      // 경로시작점안내           10시방향
    IDV_RG_startdir_11  = 171,      // 경로시작점안내           11시방향
    IDV_RG_startdir_12  = 172,      // 경로시작점안내           12시방향
    IDV_RG_dest_str     = 181,      // 목적지           직진방향
    IDV_RG_dest_left    = 182,      // 목적지           왼쪽방향
    IDV_RG_dest_right   = 183,      // 목적지          오른쪽방향
    IDV_RG_pos_right    = 184,      //         경유지
    IDV_RG_via_1        =  185,     //         첫번째 경유지
    IDV_RG_via_2        =  186,     //         두번째 경유지
    IDV_RG_via_3        =  187,     //         세번째 경유지
    IDV_RG_via_4        =  188,     //         네번째 경유지
    IDV_RG_via_5        = 189,      //         다섯번째 경유지
    IDV_RG_notice_speed = 191,      //         제한속도
    IDV_RG_notice_Accident = 192,   //         사고다발
    IDV_RG_notice_turn  = 193,      //         급커브
    IDV_RG_notice_rock  = 194,      //         낙석주의지역
    IDV_RG_notice_pol   = 195,      //         경찰청 고속도로 차로 통행준순 안내
    IDV_RG_notice_pol2  = 196,      //         경찰청 고속도로 차로 통행준순 안내
    IDV_RG_start        = 200,      //         출발지
    IDV_RG_pos_goal     = 201,      //         목적지
    IDV_RG_go_straight  = 202,      //         다음 안내시 까지 직진입니다.
    IDV_RG_goal_cross   = 203,      //         건너편에 목적지가 있습니다.
    IDV_RG_in_Route     = 210,      //         경로 진입 안내음.
    IDV_RG_in_exp       = 220,      //         고속도로에 있습니까?
    IDV_RG_in_saexp     = 221,      //         도시고속도로에 있습니까?
    IDV_RG_in_sky       = 222,      //         고가도로에 있습니까?
    IDV_RG_in_under     = 223,      //         지하도로에 있습니까?

    // 2011.06 회전교차로 안내 추가
    IDV_RG_RDir_1    = 231,      //         1시방향
    IDV_RG_RDir_2    = 232,      //         2시방향
    IDV_RG_RDir_3    = 233,      //         3시방향
    IDV_RG_RDir_4    = 234,      //         4시방향
    IDV_RG_RDir_5    = 235,      //         5시방향
    IDV_RG_RDir_6    = 236,      //         6시방향
    IDV_RG_RDir_7    = 237,      //         7시방향
    IDV_RG_RDir_8    = 238,      //         8시방향
    IDV_RG_RDir_9    = 239,      //         9시방향
    IDV_RG_RDir_10  = 240,      //         10시방향
    IDV_RG_RDir_11  = 241,      //         11시방향
    IDV_RG_RDir_12  = 242,      //         12시방향
    IDV_RG_Tollgate = 249, // 톨게이트입니다.

    IDV_RG_end = 250 // 경로안내를 종료합니다.
};

/**
 * @brief The MapPOIIconCategory enum
 */
enum MapPOIIconCategory {
    MapPOIIconCategory_MAP_POI_ICON_CATEGORY_RESTAURANT = 0,
    MapPOIIconCategory_MAP_POI_ICON_CATEGORY_GAS_STATION,
    MapPOIIconCategory_MAP_POI_ICON_CATEGORY_HOTEL,
    MapPOIIconCategory_MAP_POI_ICON_CATEGORY_ATM,
    MapPOIIconCategory_MAP_POI_ICON_CATEGORY_REST_AREA,
    MapPOIIconCategory_MAP_POI_ICON_CATEGORY_HOSPITAL,
    MapPOIIconCategory_MAP_POI_ICON_CATEGORY_PARKING_LOT,
    MapPOIIconCategory_MAP_POI_ICON_CATEGORY_PARKING,
    MapPOIIconCategory_MAP_POI_ICON_CATEGORY_FINANCIAL_INSTITUTION,
    MapPOIIconCategory_MAP_POI_ICON_CATEGORY_POST_OFFICE,
    MapPOIIconCategory_MAP_POI_ICON_CATEGORY_DEPARTMENT_STORE,
    MapPOIIconCategory_MAP_POI_ICON_CATEGORY_CONVENIENCE_STORE,
    MapPOIIconCategory_MAP_POI_ICON_CATEGORY_FAST_FOOD,
    MapPOIIconCategory_MAP_POI_ICON_CATEGORY_BANKING_FACILITIES
};

/**
 * @brief The RouteCriterion enum
 */
enum RouteCriterion {
    RouteCriterion_ROUTE_CRITERION_ECONOMIC = 0,
    RouteCriterion_ROUTE_CRITERION_FAST,
    RouteCriterion_ROUTE_CRITERION_SHORT,
    RouteCriterion_ROUTE_CRITERION_MINIMIZE_FREEWAY
};

/**
 * @brief The FreeTextSearchScope enum
 */
enum FreeTextSearchScope {
    FreeTextSearchScope_FREETEXT_SEARCH_SCOPE_POI_AT_CURRENT_POSITION = 0,
    FreeTextSearchScope_FREETEXT_SEARCH_SCOPE_POI_AT_DESTINATION_POSITION,
    FreeTextSearchScope_FREETEXT_SEARCH_SCOPE_POI_AT_LOCATION,
    FreeTextSearchScope_FREETEXT_SEARCH_SCOPE_POI_ALONG_ROUTE,
    FreeTextSearchScope_FREETEXT_SEARCH_SCOPE_POI_WHOLE_DATA,
    FreeTextSearchScope_FREETEXT_SEARCH_SCOPE_ADDRESS_WHOLE_DATA,
    FreeTextSearchScope_FREETEXT_SEARCH_SCOPE_COMBINED_WHOLE_DATA
};


/**
 * @brief Enum for FreeTextLocationTypes.
 */
enum FTSLocationType {
    FREETEXT_SEARCH_TYPE_COMBINED = 0,
    FREETEXT_SEARCH_TYPE_ADDRESS,
    FREETEXT_SEARCH_TYPE_POI
};

/**
 * @brief The SDSAddressElementType enum
 */
enum SDSAddressElementType {
    SDSAddressElementType_SDS_ADDRESS_ELEMENT_TYPE_COUNTRY = 0,
    SDSAddressElementType_SDS_ADDRESS_ELEMENT_TYPE_PLACE,
    SDSAddressElementType_SDS_ADDRESS_ELEMENT_TYPE_ROAD,
    SDSAddressElementType_SDS_ADDRESS_ELEMENT_TYPE_CROSSROAD,
    SDSAddressElementType_SDS_ADDRESS_ELEMENT_TYPE_HOUSENUMBER,
    SDSAddressElementType_SDS_ADDRESS_ELEMENT_TYPE_POSTALDISTRICT,
    SDSAddressElementType_SDS_ADDRESS_ELEMENT_TYPE_CITYDESTRICT,
    SDSAddressElementType_SDS_ADDRESS_ELEMENT_TYPE_PLACEREFINEMENT,
    SDSAddressElementType_SDS_ADDRESS_ELEMENT_TYPE_ROADREFINEMENT,
    SDSAddressElementType_SDS_ADDRESS_ELEMENT_TYPE_STATE
};

/**
 * @brief The MapRepresentation enum
 */
enum MapRepresentation {
    MapRepresentation_MAP_REPRESENTATION_2D_NORTH_UP = 0,
    MapRepresentation_MAP_REPRESENTATION_3D_CAR_HEADING,
    MapRepresentation_MAP_REPRESENTATION_2D_CAR_HEADING,
    MapRepresentation_MAP_REPRESENTATION_NOT_SET
};

/**
 * @brief The MapMultiViewMode enum
 */
enum MapMultiViewMode {
    MapMultiViewMode_MAP_MULTI_VIEW_MODE_UNCHANGED = 0,
    MapMultiViewMode_MAP_MULTI_VIEW_MODE_OFF,
    MapMultiViewMode_MAP_MULTI_VIEW_MODE_ON,
};

/**
 * @brief The MapViewMode enum
 */
enum MapViewMode {
    MapViewMode_MAP_VIEW_MODE_FULL_MAP = 0,
    MapViewMode_MAP_VIEW_MODE_SPLIT_MAP
};

/**
 * @brief The MapCameraMode enum
 */
enum MapCameraMode {
    MapCameraMode_MAP_MODE_CARSOR = 0,
};

/**
 * @brief The DeformedImageRoadType enum
 */
enum DeformedImageRoadType {
    DeformedImageRoadType_NONE = 0,
    DeformedImageRoadType_FORK,
    DeformedImageRoadType_EXIT_LEFT,
    DeformedImageRoadType_EXIT_RIGHT
};

/**
 * @brief The DeformedImageArrowType enum
 */
enum DeformedImageArrowType {
    DeformedImageArrowType_NONE = 0,
    DeformedImageArrowType_STRAIGHT,
    DeformedImageArrowType_LEFT,
    DeformedImageArrowType_RIGHT
};

/**
 * @brief The TimeType enum
 */
enum TimeType {
    TimeType_REMAININGTIME = 0,
    TimeType_ETA
};

/**
 * @brief The TimeFormat enum
 */
enum TimeFormat {
    TimeFormat_TWELVEHOURS = 0,
    TimeFormat_TWENTYFOURHOURS
};

/**
 * @brief The AdminRoadClass enum
 */
enum AdminRoadClass {
    AdminRoadClass_UNKNOWN = 0,
    AdminRoadClass_MOST_IMPORTANT,
    AdminRoadClass_SECOND_IMPORTANT,
    AdminRoadClass_THIRD_IMPORTANT,
    AdminRoadClass_FOURTH_IMPORTANT,
    AdminRoadClass_FIFTH_IMPORTANT,
    AdminRoadClass_SIXTH_IMPORTANT,
    AdminRoadClass_LEAST_IMPORTANT
};

/**
 * @brief The ManeuverRecommendationType enum
 */
enum ManeuverRecommendationType {
    ManeuverRecommendationType_PREVIEW = 0,
    ManeuverRecommendationType_PRELIMINARY,
    ManeuverRecommendationType_REGULAR,
    ManeuverRecommendationType_REPEATED
};

/**
 * @brief The SpeedUnit enum
 */
enum SpeedUnit {
    SpeedUnit_KM_PER_HOUR = 0,
    SpeedUnit_MI_PER_HOUR
};

/**
 * @brief The LaneStatus enum
 */
enum LaneStatus {
    LaneStatus_UNKNOWN = 0,
    LaneStatus_FORBIDDEN,
    LaneStatus_NOT_RECOMMENDED,
    LaneStatus_RECOMMENDED
};

/**
 * @brief The ManeuverViewType enum
 */
enum ManeuverViewType {
    ManeuverViewType_NONE = 0,
    ManeuverViewType_DEFORMED_IMAGE,
    ManeuverViewType_INTERSECTION_MAP
};

/**
 * @brief The IntersectionMapCarsorState enum
 */
enum IntersectionMapCarsorState {
    IntersectionMapCarsorState_OFF = 0,
    IntersectionMapCarsorState_ON,
    IntersectionMapCarsorState_STANDBY
};

/**
 * @brief The ZoomLevel enum
 */
enum ZoomLevel {
    ZoomLevel_ZOOM_50M = 0,
    ZoomLevel_ZOOM_100M,
    ZoomLevel_ZOOM_200M,
    ZoomLevel_ZOOM_500M,
    ZoomLevel_ZOOM_1000M,
    ZoomLevel_ZOOM_2000M,
    ZoomLevel_ZOOM_5000M,
    ZoomLevel_ZOOM_10000M
};

/**
 * @brief The ImageType enum
 */
enum ImageType {
    ImageType_PNG = 0
};

/**
 * @brief The SDSVoiceTagOptions enum
 */
enum SDSVoiceTagOptions {
    SDSVoiceTagOptions_NONE = 0,
    SDSVoiceTagOptions_CREATE_NEW_VOICETAG,
    SDSVoiceTagOptions_PLAY_VOICETAG,
    SDSVoiceTagOptions_DELETE_VOICETAG,
    SDSVoiceTagOptions_DELETE_ALL_VOICETAGS,
    SDSVoiceTagOptions_NEW_VOICETAG_CREATED,
    SDSVoiceTagOptions_NO_VOICETAG_CREATED,
    SDSVoiceTagOptions_VOICETAG_DELETED
};

/**
 * @brief The DestinationMemoryCategoryType enum
 */
enum DestinationMemoryCategoryType {
    DestinationMemoryCategoryType_ADDRESS_BOOK = 0,
    DestinationMemoryCategoryType_LAST_DESTINATIONS
};

/**
 * @brief The DirectionDescription enum
 */
enum DirectionDescription {
    DirectionDescription_NORTH = 0,
    DirectionDescription_NORTH_NORTH_EAST,
    DirectionDescription_NORTH_EAST,
    DirectionDescription_EAST_NORTH_EAST,
    DirectionDescription_EAST,
    DirectionDescription_EAST_SOUTH_EAST,
    DirectionDescription_SOUTH_EAST,
    DirectionDescription_SOUTH_SOUTH_EAST,
    DirectionDescription_SOUTH,
    DirectionDescription_SOUTH_SOUTH_WEST,
    DirectionDescription_SOUTH_WEST,
    DirectionDescription_WEST_SOUTH_WEST,
    DirectionDescription_WEST,
    DirectionDescription_WEST_NORTH_WEST,
    DirectionDescription_NORTH_WEST,
    DirectionDescription_NORTH_NORTH_WEST,
    DirectionDescription_INVALID
};

/**
 * @brief The TrafficReceptionState enum
 */
enum TrafficReceptionState {
    TrafficReceptionState_UNKNOWN = 0,
    TrafficReceptionState_OK,
    TrafficReceptionState_NOT_SUBSCRIBED,
    TrafficReceptionState_NOT_AVAILABLE,
    TrafficReceptionState_NO_ANTENNA,
    TrafficReceptionState_NO_SIGNAL,
    TrafficReceptionState_TEMPORARY_NOT_AVAILABLE,
    TrafficReceptionState_INACTIVE,
    TrafficReceptionState_NO_INTERNET_CONNECTION,
    TrafficReceptionState_AUTHENTIFICATION_INVALID,
    TrafficReceptionState_CONNECTION_LOST
};

/**
 * @brief The MapVideoStreamStatus enum
 */
enum MapVideoStreamStatus {
    MapVideoStreamStatus_UNAVAILABLE = 0,
    MapVideoStreamStatus_AVAILABLE,
    MapVideoStreamStatus_SPI_NAVI_ACTIVE
};

/**
 * \brief method to trigger the sample voice Phrase
 */
enum SampleVoicePhrase {
    SAMPLE_PHRASE_GUIDANCE_WILL_BE_AT_THIS_VOLUME = 0,
    SAMPLE_PHRASE_LOUDER,
    SAMPLE_PHRASE_QUIETER,
    SAMPLE_PHRASE_VOICE_OUTPUT
};

/**
 * \brief Enum for the reasons to which the navigation route is recalculated
 */
enum RouteRecalculationReason {
    UNKNOWN = 0,
    NO_ROUTE_AVAILABLE,
    NO_CALCULATION,
    FIRST_CALCULATION,
    OPTIMIZE_ROUTE,
    REGARDING_TRAFFIC_MESSAGE,
    DISREGARD_ROUTE,
    REGARDING_USER_TRAFFIC_MESSAGE,
    ROUTE_OPTION_CHANGED,
    ROUTE_CRITERION_CHANGED,
    ROUTE_OPTION_CRITERION_CHANGED,
    ENTRY_POINT_EVALUATION
};

/**
 * \brief Enum for waypoint operation status information
 */
enum WaypointOperationStatus {
    INSERTED = 0,
    DELETED,
    MOVED,
    REPLACED
};

/**
 * \brief Enum for destination reached status information
 */
enum DestinationReachedStatus {
    UNKNOWN2 = 0,
    WAYPOINT,
    FINAL_DESTINATION,
    WAYPOINT_SKIPPED 
};

/**
 * \brief Enum containing map light mode status such as day and night
 */
enum LightModeStatus {
    DAY = 0,
    NIGHT
};

enum GetPartNumberResult {
    SUCCESS = 0,
    NO_PART_NUMBER_SET,
    NO_PART_NUMBER_FOR_DEVICE_SET,
    DATA_UNAVAILABLE,
    META_DATA_UNAVAILABLE,
    DEVICE_UNKNOWN,
    FAILURE,
    UNKNOWN3,
};


/**
 * @brief The DestinationMemoryAddressType enum
 */
enum DestinationMemoryAddressType {
    DESTINATION_MEMORY_COUNTRY = 0,
    DESTINATION_MEMORY_STATE,
    DESTINATION_MEMORY_CITY,
    DESTINATION_MEMORY_STREET,
    DESTINATION_MEMORY_HOUSENO,
    DESTINATION_MEMORY_JUNCTION,
    DESTINATION_MEMORY_POI,
    DESTINATION_MEMORY_POI_CATEGORY,
    DESTINATION_MEMORY_POI_BRAND,
    DESTINATION_MEMORY_COUNTRY_CODE,
    DESTINATION_MEMORY_ADDRESS_DETAIL,
};


typedef QList<uint> NSWaypointOpertaionStatusList;

/**
 * @brief NSMapPOIIconCategories
 */
typedef QList<uint> NSMapPOIIconCategories;


/**
 * @brief The NSPositionWGS84 struct
 */
struct NSPositionWGS84 {
    NSPositionWGS84() : m_longitude(0), m_latitude(0) { }
    double m_longitude;
    double m_latitude;

    bool operator==(const NSPositionWGS84 &other) const {
        if (m_longitude == other.m_longitude &&
                m_latitude == other.m_latitude) {
            return true;
        }
        return false;
    }

    bool operator!=(const NSPositionWGS84 &other) const {
        return !(*this == other);
    }

};

QDBusArgument &operator<<(QDBusArgument &argument, const NSPositionWGS84& data);
const QDBusArgument &operator>>(const QDBusArgument &argument, NSPositionWGS84 &data);

/**
 * @brief The NSGlobalUpdateRegionId struct
 */
struct NSGlobalUpdateRegionId {
	NSGlobalUpdateRegionId() : m_updateRegionId(0), m_productId(0), m_supplierId(0) { }
    short m_updateRegionId;
    int m_productId;
    short m_supplierId;
};

QDBusArgument &operator<<(QDBusArgument &argument, const NSGlobalUpdateRegionId& data);
const QDBusArgument &operator>>(const QDBusArgument &argument, NSGlobalUpdateRegionId &data);

/**
 * @brief The NSGlobalNamedObjectId struct
 */
struct NSGlobalNamedObjectId {
	NSGlobalNamedObjectId() : m_namedObjectId(0), m_isValid(false) { }
    NSGlobalUpdateRegionId m_globalUpdateRegionId;
    uint m_namedObjectId;
    bool m_isValid;
};

QDBusArgument &operator<<(QDBusArgument &argument, const NSGlobalNamedObjectId& data);
const QDBusArgument &operator>>(const QDBusArgument &argument, NSGlobalNamedObjectId &data);

/**
 * @brief The NSAddressListElement struct
 */
struct NSAddressListElement {
    NSAddressListElement() : m_id(0), m_direction(0) { }
    QString m_data;
    qulonglong m_id;
    QString m_distance;
    QString m_address;
    uint m_direction;
    NSPositionWGS84 m_coordinates;
    NSGlobalNamedObjectId m_sdsNameObjectId;
};


QDBusArgument &operator<<(QDBusArgument &argument, const NSAddressListElement& data);
const QDBusArgument &operator>>(const QDBusArgument &argument, NSAddressListElement &data);

typedef QList<NSAddressListElement> NSAddressWithIdInfos;

/**
 * @brief The NSDistanceToDestination struct
 */
struct NSDistanceToDestination {
    NSDistanceToDestination() : m_distanceToDestination(0), m_distanceUnit(0) { }
    QString m_distanceToDestinationAsString;
    uint m_distanceToDestination;
    uint m_distanceUnit;
};

QDBusArgument &operator<<(QDBusArgument &argument, const NSDistanceToDestination& data);
const QDBusArgument &operator>>(const QDBusArgument &argument, NSDistanceToDestination &data);

/**
 * @brief The NSArrivalInformation struct
 */
struct NSArrivalInformation {
    NSArrivalInformation() : m_timeToDestination(0), m_estimatedTimeOfArrivalHour(0), m_estimatedTimeOfArrivalMinute(0),
        m_estimatedTimeType(0), m_displayTimeType(0), m_displayTimeFormat(0), m_destinationIndex(0) { }
    NSDistanceToDestination m_distanceToDestination;
    QString m_timeToDestinationAsString;
    int m_timeToDestination;
    QString m_estimatedTimeOfArrivalAsString;
    int m_estimatedTimeOfArrivalHour;
    int m_estimatedTimeOfArrivalMinute;
    uint m_estimatedTimeType; // 0: DESTINATION, 1: WAYPOINT
    uint m_displayTimeType;
    uint m_displayTimeFormat;
    int m_destinationIndex;
};

QDBusArgument &operator<<(QDBusArgument &argument, const NSArrivalInformation& data);
const QDBusArgument &operator>>(const QDBusArgument &argument, NSArrivalInformation &data);

typedef QList<NSArrivalInformation> NSArrivalInformationList;

/**
 * @brief The NSBarGraphInfo struct
 */
struct NSBarGraphInfo {
    NSBarGraphInfo() : m_valid(false), m_value(0), m_scale(0), m_distanceUnit(0) { }
    bool m_valid;
    ushort m_value;
    ushort m_scale;
    uint m_distanceUnit;
    QString m_barGraphMaxScale;
    QString m_barGraphMinScale;
};

QDBusArgument &operator<<(QDBusArgument &argument, const NSBarGraphInfo& data);
const QDBusArgument &operator>>(const QDBusArgument &argument, NSBarGraphInfo &data);


/**
 * @brief The NSCurrentFreeTextSearchInfo struct
 */
struct NSCurrentFreeTextSearchInfo {
    NSPositionWGS84 m_position;
    QString m_searchString;
};

QDBusArgument &operator<<(QDBusArgument &argument, const NSCurrentFreeTextSearchInfo& data);
const QDBusArgument &operator>>(const QDBusArgument &argument, NSCurrentFreeTextSearchInfo &data);

/**
 * @brief The NSDeformedImage struct
 */
struct NSDeformedImage {
    NSDeformedImage() : m_valid(false), m_deformedImageRoadType(0), m_deformedImageArrowType(0) { }
    bool m_valid;
    uint m_deformedImageRoadType;
    uint m_deformedImageArrowType;
};

QDBusArgument &operator<<(QDBusArgument &argument, const NSDeformedImage& data);
const QDBusArgument &operator>>(const QDBusArgument &argument, NSDeformedImage &data);

/**
 * @brief The NSDestinationInfoElement struct
 */
struct NSDestinationInfoElement {
    NSPositionWGS84 m_location;
    QString m_countryCode;
    QString m_subCountryCode;
    QString m_name;
};

QDBusArgument &operator<<(QDBusArgument &argument, const NSDestinationInfoElement& data);
const QDBusArgument &operator>>(const QDBusArgument &argument, NSDestinationInfoElement &data);

typedef QList<NSDestinationInfoElement> NSDestinationInfoElements;

/**
 * @brief The NSDestinationMemoryEntry struct
 */
struct NSDestinationMemoryEntry {
    NSDestinationMemoryEntry() : m_entryID(0), m_voiceTagId(0) { }
    QString m_name;
    qulonglong m_entryID;
    uint m_voiceTagId;
};

QDBusArgument &operator<<(QDBusArgument &argument, const NSDestinationMemoryEntry& data);
const QDBusArgument &operator>>(const QDBusArgument &argument, NSDestinationMemoryEntry &data);

/**
 * @brief The NSDestinationMemoryEntryItem struct
 */
struct NSDestinationMemoryEntryItem {
    NSDestinationMemoryEntryItem() : m_addressType(0) { }
    QString m_itemString;
    uint m_addressType;
    QString m_itemPhoneme;
};

QDBusArgument &operator<<(QDBusArgument &argument, const NSDestinationMemoryEntryItem& data);
const QDBusArgument &operator>>(const QDBusArgument &argument, NSDestinationMemoryEntryItem &data);

typedef QList<NSDestinationMemoryEntryItem> NSDestinationMemoryEntryItemList;

/**
 * @brief The NSDestinationMemoryEntryDetails struct
 */
struct NSDestinationMemoryEntryDetails {
    QString m_countryName;
    QString m_stateName;
    QString m_cityName;
    QString m_streetName;
    QString m_houseno;
    QString m_junctionName;
    QString m_poiName;
    QString m_poiCategoryName;
    QString m_poiBrandName;
    QString m_countryCode;
    QString m_addressDetail;
    NSDestinationMemoryEntryItemList m_sdsDestinationMemoryEntries;
};

QDBusArgument &operator<<(QDBusArgument &argument, const NSDestinationMemoryEntryDetails& data);
const QDBusArgument &operator>>(const QDBusArgument &argument, NSDestinationMemoryEntryDetails &data);

/**
 * @brief The NSDestinationMemoryDetailListElement struct
 */
struct NSDestinationMemoryDetailListElement {
    NSDestinationMemoryEntry m_entry;
    NSDestinationMemoryEntryDetails m_details;
    NSPositionWGS84 m_coordinates; // updated
};

QDBusArgument &operator<<(QDBusArgument &argument, const NSDestinationMemoryDetailListElement& data);
const QDBusArgument &operator>>(const QDBusArgument &argument, NSDestinationMemoryDetailListElement &data);

typedef QList<NSDestinationMemoryDetailListElement> NSDestinationMemoryDetailsList;

/**
 * @brief The NSHomeLocation struct
 */
struct NSHomeLocation {
    NSHomeLocation() : m_valid(false) { }
    QString m_name;
    NSPositionWGS84 m_positionWGS84;
    bool m_valid;
};

QDBusArgument &operator<<(QDBusArgument &argument, const NSHomeLocation& data);
const QDBusArgument &operator>>(const QDBusArgument &argument, NSHomeLocation &data);

/**
 * @brief The NSImage struct
 */
struct NSImage {
    NSImage() : m_imageType(0) { }
    QByteArray m_image;
    uint m_imageType;
};

QDBusArgument &operator<<(QDBusArgument &argument, const NSImage& data);
const QDBusArgument &operator>>(const QDBusArgument &argument, NSImage &data);

/**
 * @brief The NSScreenCoordinate struct
 */
struct NSScreenCoordinate {
    NSScreenCoordinate() : m_xPos(0), m_yPos(0) { }
    uint m_xPos;
    uint m_yPos;
};

QDBusArgument &operator<<(QDBusArgument &argument, const NSScreenCoordinate& data);
const QDBusArgument &operator>>(const QDBusArgument &argument, NSScreenCoordinate &data);

typedef QList<NSScreenCoordinate> NSScreenCoordinateList;

/**
 * @brief The NSIntersectionMapRoadLine struct
 */
struct NSIntersectionMapRoadLine {
    NSIntersectionMapRoadLine() : m_roadClass(0) { }
    NSScreenCoordinateList m_coordinates;
    uint m_roadClass;
};

QDBusArgument &operator<<(QDBusArgument &argument, const NSIntersectionMapRoadLine& data);
const QDBusArgument &operator>>(const QDBusArgument &argument, NSIntersectionMapRoadLine &data);

typedef QList<NSIntersectionMapRoadLine> NSIntersectionMapRoadLineList;

/**
 * @brief The NSIntersectionMap struct
 */
struct NSIntersectionMap {
    NSIntersectionMap() : m_intersectionNumber(0) { }
    NSIntersectionMapRoadLineList m_road;
    NSIntersectionMapRoadLineList m_route;
    NSScreenCoordinateList m_noEntryIcons;
    uchar m_intersectionNumber;
};

QDBusArgument &operator<<(QDBusArgument &argument, const NSIntersectionMap& data);
const QDBusArgument &operator>>(const QDBusArgument &argument, NSIntersectionMap &data);

/**
 * @brief The NSIntersectionMapCarsorPosition struct
 */
struct NSIntersectionMapCarsorPosition {
    NSIntersectionMapCarsorPosition() : m_intersectionMapId(0), m_carsorState(0), m_carsorAngle(0) { }
    uint m_intersectionMapId;
    uint m_carsorState;
    NSScreenCoordinate m_carsorPosition;
    int m_carsorAngle;
};

QDBusArgument &operator<<(QDBusArgument &argument, const NSIntersectionMapCarsorPosition& data);
const QDBusArgument &operator>>(const QDBusArgument &argument, NSIntersectionMapCarsorPosition &data);

/**
 * @brief The NSLaneRecommendation struct
 */
struct NSLaneRecommendation {
    NSLaneRecommendation() : m_laneStatus(0), m_combinedFollowingLaneDirection(0), m_guidedFollowingLaneDirection(0) { }
    uint m_laneStatus;
    int m_combinedFollowingLaneDirection;
    int m_guidedFollowingLaneDirection;
};

QDBusArgument &operator<<(QDBusArgument &argument, const NSLaneRecommendation& data);
const QDBusArgument &operator>>(const QDBusArgument &argument, NSLaneRecommendation &data);

typedef QList<NSLaneRecommendation> NSLaneRecommendationList;

/**
 * @brief The NSTrafficEnforcement struct
 */
struct NSTrafficEnforcement {
    NSTrafficEnforcement() : m_TrafficEnforcementAlertState(0), m_trafficEnforcementCameraType(0), m_distanceUnit(0), m_trafficEnforcementDistanceType(0), m_speedLimit(0) { }
    uint m_TrafficEnforcementAlertState;
    uint m_trafficEnforcementCameraType;
    QString m_distance;
    uint m_distanceUnit;
    uint m_trafficEnforcementDistanceType;
    uint m_speedLimit;
};

QDBusArgument &operator<<(QDBusArgument &argument, const NSTrafficEnforcement& data);
const QDBusArgument &operator>>(const QDBusArgument &argument, NSTrafficEnforcement &data);

/**
 * @brief The NSLocation struct
 */
struct NSLocation {
    NSLocation() : m_valid(false) { }
    QString m_name;
    NSPositionWGS84 m_positionWGS84;
    bool m_valid;

    void reset() {
        m_name.clear();
        memset(&m_positionWGS84, 0, sizeof(NSPositionWGS84));
        m_valid = false;
    }
};

QDBusArgument &operator<<(QDBusArgument &argument, const NSLocation& data);
const QDBusArgument &operator>>(const QDBusArgument &argument, NSLocation &data);

/**
 * @brief The NSLocationDetails struct
 */
struct NSLocationDetails {
    QString m_cityName;
    QString m_countryName;
    QString m_countryCode;
    QString m_stateName;
    QString m_stateCode;
    QString m_phoneNumber;
    QString m_poiAddress;
    QString m_streetName;
    QString m_zipCode;
    NSPositionWGS84 m_coordinate;
};

QDBusArgument &operator<<(QDBusArgument &argument, const NSLocationDetails& data);
const QDBusArgument &operator>>(const QDBusArgument &argument, NSLocationDetails &data);

/**
 * @brief The NSManeuverView struct
 */
struct NSManeuverView {
    NSManeuverView() : m_maneuverViewType(0), m_intersectionMapId(0) { }
    uint m_intersectionMapId;
    uint m_maneuverViewType;
    NSDeformedImage m_deformedImage;
};

QDBusArgument &operator<<(QDBusArgument &argument, const NSManeuverView& data);
const QDBusArgument &operator>>(const QDBusArgument &argument, NSManeuverView &data);

/**
 * @brief The NSNextManeuverDetails struct
 */
struct NSNextManeuverDetails {
    NSNextManeuverDetails() : m_distanceToManeuverUnit(0), m_recommendationType(0),
        m_isNewManeuver(false), m_isReRouted(false) { }
    QString m_distanceToManeuver;
    uint m_distanceToManeuverUnit;
    uint m_recommendationType;
    bool m_isNewManeuver;
    bool m_isReRouted;
};

QDBusArgument &operator<<(QDBusArgument &argument, const NSNextManeuverDetails& data);
const QDBusArgument &operator>>(const QDBusArgument &argument, NSNextManeuverDetails &data);

/**
 * @brief The NSPoiCategoryShortcut struct
 */
struct NSPoiCategoryShortcut {
//    NSPoiCategoryShortcut() : m_poiCategoryId(0), m_shortcutIndex(0) { }
    QString m_categoryName;
    uint m_poiCategoryId;
    NSImage m_icon;
    uchar m_shortcutIndex;
    NSGlobalNamedObjectId m_sdsNameObjectId;
};

QDBusArgument &operator<<(QDBusArgument &argument, const NSPoiCategoryShortcut& data);
const QDBusArgument &operator>>(const QDBusArgument &argument, NSPoiCategoryShortcut &data);

typedef QList<NSPoiCategoryShortcut> NSPoiCategoryShortcuts;

/**
 * @brief The NSPOILocation struct
 */
struct NSPOILocation {
    NSLocation m_location;
    QString m_phoneNumber;
    QString m_address;
    NSImage m_icon;

    void reset() {
        m_location.reset();
        m_address.clear();
        memset(&m_icon, 0, sizeof(NSImage));
    }
};

QDBusArgument &operator<<(QDBusArgument &argument, const NSPOILocation& data);
const QDBusArgument &operator>>(const QDBusArgument &argument, NSPOILocation &data);

typedef QList<NSPOILocation> NSPOILocations;


/**
 * @brief The NSTurntoStreet struct
 */
struct NSTurnToStreet {
    uint m_maneuverId;
    QString m_turnToStreet;
};

QDBusArgument &operator<<(QDBusArgument &argument, const NSTurnToStreet& data);
const QDBusArgument &operator>>(const QDBusArgument &argument, NSTurnToStreet &data);

/**
 * @brief The NSPositionInformation struct
 */
struct NSPositionInformation {
    NSPositionInformation() : m_mapMatchingPositionAccuracy(0), m_roadClass(0), m_vehicleSpeed(0), m_vehicleHeading(0), m_altitude(0) { }
    NSPositionWGS84 m_positionWGS84;
    double m_mapMatchingPositionAccuracy;
    QString m_countryCode;
    QString m_stateCode;
    uint m_roadClass;
    double m_vehicleSpeed;
    double m_vehicleHeading;
    int m_altitude;
    QString m_country;
    QString m_province;
    QString m_city;
    QString m_cityDistrict;
    QString m_street;
    QString m_streetNumber;

    bool operator==(const NSPositionInformation &other) const {
        if (m_positionWGS84 == other.m_positionWGS84 &&
                m_mapMatchingPositionAccuracy == other.m_mapMatchingPositionAccuracy &&
                m_countryCode == other.m_countryCode &&
                m_stateCode == other.m_stateCode &&
                m_roadClass == other.m_roadClass &&
                m_vehicleSpeed == other.m_vehicleSpeed &&
                m_vehicleHeading == other.m_vehicleHeading &&
                m_altitude == other.m_altitude &&
                m_country == other.m_country &&
                m_province == other.m_province &&
                m_city == other.m_city &&
                m_cityDistrict == other.m_cityDistrict &&
                m_street == other.m_street &&
                m_streetNumber == other.m_streetNumber) {
            return true;
        }
        return false;
    }

    bool operator!=(const NSPositionInformation &other) const {
        return !(*this == other);
    }

};

QDBusArgument &operator<<(QDBusArgument &argument, const NSPositionInformation& data);
const QDBusArgument &operator>>(const QDBusArgument &argument, NSPositionInformation &data);


/**
 * @brief The NSRefinementListElement struct
 */
struct NSRefinementListElement {
    NSRefinementListElement() : m_id(0), m_direction(0) { }
    QString m_data;
    uint m_id;
    QString m_distance;
    uint m_direction;
};

QDBusArgument &operator<<(QDBusArgument &argument, const NSRefinementListElement& data);
const QDBusArgument &operator>>(const QDBusArgument &argument, NSRefinementListElement &data);

typedef QList<NSRefinementListElement> NSRefinementWithIdInfos;

/**
 * @brief The NSRouteCalculationDuration struct
 */
struct NSRouteCalculationDuration {
    NSRouteCalculationDuration() : m_routeCalDurationToFinalDest(0), m_routeCalDurationToNextDest(0) { }
    uint m_routeCalDurationToFinalDest;
    uint m_routeCalDurationToNextDest;
};

QDBusArgument &operator<<(QDBusArgument &argument, const NSRouteCalculationDuration& data);
const QDBusArgument &operator>>(const QDBusArgument &argument, NSRouteCalculationDuration &data);

/**
 * @brief The NSSatelliteInfo struct
 */
struct NSSatelliteInfo {
    NSSatelliteInfo() : m_visibleSatellites(0), m_receivedSatellites(0), m_usedSatellites(0), m_gnssSpeed(0), m_gnssSpeedReliability(0) { }
    uchar m_visibleSatellites;
    uchar m_receivedSatellites;
    uchar m_usedSatellites;
    QString m_gnssMode;
    ushort m_gnssSpeed;
    uchar m_gnssSpeedReliability;
    NSPositionWGS84 m_gnssPosition;
};

QDBusArgument &operator<<(QDBusArgument &argument, const NSSatelliteInfo& data);
const QDBusArgument &operator>>(const QDBusArgument &argument, NSSatelliteInfo &data);


/**
 * @brief The NSSDSElementOptions struct
 */
struct NSSDSElementOptions {
    NSSDSElementOptions() : m_isAmbigious(false) { }
    bool m_isAmbigious;
};


QDBusArgument &operator<<(QDBusArgument &argument, const NSSDSElementOptions& data);
const QDBusArgument &operator>>(const QDBusArgument &argument, NSSDSElementOptions &data);


/**
 * @brief NSSDSAddressOptions
 */
struct NSSDSAddressOptions {
    NSSDSAddressOptions()
    : navigable(false)
    , ambigious(false)
    , removedInvalidEntry(false)
    , moreEntriesPossible(false)
    , houseNumberAvailable(false)
    , houseNumberValid(false) { }
    bool navigable;
    bool ambigious;
    bool removedInvalidEntry;
    bool moreEntriesPossible;
    bool houseNumberAvailable;
    bool houseNumberValid;
};

QDBusArgument &operator<<(QDBusArgument &argument, const NSSDSAddressOptions& data);
const QDBusArgument &operator>>(const QDBusArgument &argument, NSSDSAddressOptions &data);


/**
 * @brief The NSSDSAddressElement struct
 */
struct NSSDSAddressElement {
    NSSDSAddressElement() : m_addressElementType(0) { }
    uint m_addressElementType;
    QString m_data;
    NSSDSElementOptions m_elementOptions;
};

QDBusArgument &operator<<(QDBusArgument &argument, const NSSDSAddressElement& data);
const QDBusArgument &operator>>(const QDBusArgument &argument, NSSDSAddressElement &data);

typedef QList<NSSDSAddressElement> NSSDSAddress;


/**
 * @brief The NSSDSAddressWithOptions struct
 */
struct NSSDSAddressWithOptions {
    NSSDSAddressWithOptions() { }
    NSSDSAddress m_address;
    NSSDSAddressOptions m_addressOptions;
    QList<uint> m_addressElementType;
};

QDBusArgument &operator<<(QDBusArgument &argument, const NSSDSAddressWithOptions& data);
const QDBusArgument &operator>>(const QDBusArgument &argument, NSSDSAddressWithOptions &data);

/**
 * @brief The NSSDSVoiceTagId struct
 */
struct NSSDSVoiceTagId {
    NSSDSVoiceTagId() : m_destinationMemoryEntryId(0), m_sdsVoiceTagId(0), m_options(0) { }
    qulonglong m_destinationMemoryEntryId;
    QString m_destinationMemoryEntryName;
    uint m_sdsVoiceTagId;
    uint m_options;
};

QDBusArgument &operator<<(QDBusArgument &argument, const NSSDSVoiceTagId& data);
const QDBusArgument &operator>>(const QDBusArgument &argument, NSSDSVoiceTagId &data);

/**
 * @brief The NSSpeedLimit struct
 */
struct NSSpeedLimit {
    NSSpeedLimit() : m_present(false), m_value(0), m_speedUnit(0) { }
    bool m_present;
    QString m_countryCode;
    ushort m_value;
    uint m_speedUnit;
};

QDBusArgument &operator<<(QDBusArgument &argument, const NSSpeedLimit& data);
const QDBusArgument &operator>>(const QDBusArgument &argument, NSSpeedLimit &data);

/**
 * @brief The NSTourElement struct
 */
struct NSTourElement {
    NSTourElement() : m_useDefaultRouteCriterion(false), m_routeCriterion(0) { }
    NSPOILocation m_location;
    bool m_useDefaultRouteCriterion;
    uint m_routeCriterion;
};

QDBusArgument &operator<<(QDBusArgument &argument, const NSTourElement& data);
const QDBusArgument &operator>>(const QDBusArgument &argument, NSTourElement &data);

typedef QList<NSTourElement> NSTour;

/**
 * @brief The NSTrafficServiceReceptionStatus struct
 */
struct NSTrafficServiceReceptionStatus {
    NSTrafficServiceReceptionStatus() : m_trafficServiceReceptionStatus(false),
        m_trafficReceptionState(0), m_trafficMessageCount(0) { }
    QString m_serviceName;
    bool m_trafficServiceReceptionStatus;
    uint m_trafficReceptionState;
    uint m_trafficMessageCount;
};

QDBusArgument &operator<<(QDBusArgument &argument, const NSTrafficServiceReceptionStatus& data);
const QDBusArgument &operator>>(const QDBusArgument &argument, NSTrafficServiceReceptionStatus &data);

/**
 * @brief The NSWaypointListElement struct
 */
struct NSWaypointListElement {
    NSWaypointListElement() : m_routeCriterion(0) { }
    NSPositionWGS84 m_location;
    uint m_routeCriterion;
    QString m_name;
};

QDBusArgument &operator<<(QDBusArgument &argument, const NSWaypointListElement& data);
const QDBusArgument &operator>>(const QDBusArgument &argument, NSWaypointListElement &data);

typedef QList<NSWaypointListElement> NSWaypointList;

/**
 * @brief The NSWorkLocation struct
 */
struct NSWorkLocation {
    NSWorkLocation() : m_valid(false) { }
    QString m_name;
    NSPositionWGS84 m_positionWGS84;
    bool m_valid;
};

QDBusArgument &operator<<(QDBusArgument &argument, const NSWorkLocation& data);
const QDBusArgument &operator>>(const QDBusArgument &argument, NSWorkLocation &data);

/**
 * @brief The NSMapVideoStreamDimensions struct
 */
struct NSMapVideoStreamDimensions {
    NSMapVideoStreamDimensions() : m_width(0), m_height(0) { }
    uint m_width;
    uint m_height;
};

QDBusArgument &operator<<(QDBusArgument &argument, const NSMapVideoStreamDimensions& data);
const QDBusArgument &operator>>(const QDBusArgument &argument, NSMapVideoStreamDimensions &data);

/**
* @brief The NSmapVideoStreamResult struct
*/
struct NSMapVideoStreamResult {
    NSMapVideoStreamResult() : m_mapVideoStreamStatus(0), m_mapVideoStreamHandle(0) { }
    uint m_mapVideoStreamStatus;
    uint m_mapVideoStreamHandle;

    bool operator == (const NSMapVideoStreamResult &result) const {
        if (m_mapVideoStreamStatus == result.m_mapVideoStreamStatus &&
                m_mapVideoStreamHandle == result.m_mapVideoStreamHandle) {
            return true;
        }
        return false;
    }

    bool operator != (const NSMapVideoStreamResult &result) const {
        return !(*this == result);
    }
};

QDBusArgument &operator<<(QDBusArgument &argument, const NSMapVideoStreamResult& data);
const QDBusArgument &operator>>(const QDBusArgument &argument, NSMapVideoStreamResult &data);


struct NSNavDataPartNumber {
    uint result; // refer GetPartNumberResult, SUCCESS:0, NO_PART_NUMBER_SET:1, NO_PART_NUMBER_FOR_DEVICE_SET:2, DATA_UNAVAILABLE:3, META_DATA_UNAVAILABLE:4, DEVICE_UNKNOWN:5, FAILURE:6, UNKNOWN:7
    QString partNumber;
};

QDBusArgument &operator<<(QDBusArgument &argument, const NSNavDataPartNumber& data);
const QDBusArgument &operator>>(const QDBusArgument &argument, NSNavDataPartNumber &data);

Q_DECLARE_METATYPE(NSNavDataPartNumber)
Q_DECLARE_METATYPE(NSAddressListElement)
Q_DECLARE_METATYPE(NSAddressWithIdInfos)
Q_DECLARE_METATYPE(NSArrivalInformation)
Q_DECLARE_METATYPE(NSArrivalInformationList)
Q_DECLARE_METATYPE(NSBarGraphInfo)
Q_DECLARE_METATYPE(NSCurrentFreeTextSearchInfo)
Q_DECLARE_METATYPE(NSDeformedImage)
Q_DECLARE_METATYPE(NSDestinationInfoElement)
Q_DECLARE_METATYPE(NSDestinationInfoElements)
Q_DECLARE_METATYPE(NSDestinationMemoryDetailListElement)
Q_DECLARE_METATYPE(NSDestinationMemoryDetailsList)
Q_DECLARE_METATYPE(NSDestinationMemoryEntry)
Q_DECLARE_METATYPE(NSDestinationMemoryEntryDetails)
Q_DECLARE_METATYPE(NSDestinationMemoryEntryItem)
Q_DECLARE_METATYPE(NSDestinationMemoryEntryItemList)
Q_DECLARE_METATYPE(NSDistanceToDestination)
Q_DECLARE_METATYPE(NSGlobalNamedObjectId)
Q_DECLARE_METATYPE(NSGlobalUpdateRegionId)
Q_DECLARE_METATYPE(NSHomeLocation)
Q_DECLARE_METATYPE(NSImage)
Q_DECLARE_METATYPE(NSIntersectionMap)
Q_DECLARE_METATYPE(NSIntersectionMapCarsorPosition)
Q_DECLARE_METATYPE(NSIntersectionMapRoadLine)
Q_DECLARE_METATYPE(NSIntersectionMapRoadLineList)
Q_DECLARE_METATYPE(NSLaneRecommendation)
Q_DECLARE_METATYPE(NSLaneRecommendationList)
Q_DECLARE_METATYPE(NSTrafficEnforcement)
Q_DECLARE_METATYPE(NSLocation)
Q_DECLARE_METATYPE(NSLocationDetails)
Q_DECLARE_METATYPE(NSManeuverView)
Q_DECLARE_METATYPE(NSNextManeuverDetails)
Q_DECLARE_METATYPE(NSPoiCategoryShortcut)
Q_DECLARE_METATYPE(NSPoiCategoryShortcuts)
Q_DECLARE_METATYPE(NSPOILocation)
Q_DECLARE_METATYPE(NSPOILocations)
Q_DECLARE_METATYPE(NSPositionInformation)
Q_DECLARE_METATYPE(NSPositionWGS84)
Q_DECLARE_METATYPE(NSRefinementListElement)
Q_DECLARE_METATYPE(NSRefinementWithIdInfos)
Q_DECLARE_METATYPE(NSRouteCalculationDuration)
Q_DECLARE_METATYPE(NSSatelliteInfo)
Q_DECLARE_METATYPE(NSScreenCoordinate)
Q_DECLARE_METATYPE(NSScreenCoordinateList)
Q_DECLARE_METATYPE(NSSDSAddressElement)
Q_DECLARE_METATYPE(NSSDSAddress)
Q_DECLARE_METATYPE(NSSDSAddressWithOptions)
Q_DECLARE_METATYPE(NSSDSElementOptions)
Q_DECLARE_METATYPE(NSSDSVoiceTagId)
Q_DECLARE_METATYPE(NSSpeedLimit)
Q_DECLARE_METATYPE(NSTourElement)
Q_DECLARE_METATYPE(NSTour)
Q_DECLARE_METATYPE(NSTrafficServiceReceptionStatus)
Q_DECLARE_METATYPE(NSWaypointListElement)
Q_DECLARE_METATYPE(NSWaypointList)
Q_DECLARE_METATYPE(NSWorkLocation)
Q_DECLARE_METATYPE(NSMapVideoStreamDimensions)
Q_DECLARE_METATYPE(NSMapVideoStreamResult)
Q_DECLARE_METATYPE(NSTurnToStreet)

/**
 * @brief The NavigationServiceType class
 */
class NavigationServiceType
{
public:
    static void registerMetaTypes();
};

#endif // NAVIGATIONSERVICETYPE_H
