#ifndef RGPROTOCOL_H
#define RGPROTOCOL_H

// c++과 qml 모두 참조하는 rg 상수값 정의
// route/RGConstant.java

namespace SKT {

/** RG 상태 **/
class RgStatusCode : public QObject {
    Q_OBJECT
    Q_ENUMS( RgStatusCodeK )
public:
    enum RgStatusCodeK {
        NONE = 0, // 상태 없음.
        PLAYING = 1, // 정상주행
        NOTYET = 2, // 미진입
        PASSGOAL = 3, // 목적지 지나침.
        REROUTE = 4, // 경로이탈 (재탐색)
        TVAS_REROUTE = 5, // 경로이탈 (재탐색)
        BAD = 6,  // 매칭 도로가 존재하지 않음.
        FINISH = 7, // 종료
    };
};

/** 진행 지점 코드 **/
class RgPositionTypeCode : public QObject {
    Q_OBJECT
    Q_ENUMS( RgPositionTypeCodeK );
public:
    enum RgPositionTypeCodeK {
        NONE = 0,  // 없음
        START = 1,  // 출발지
        VIA1 = 2,  // 첫번째 경유지
        VIA2 = 3,  // 두번째 경유지
        VIA3 = 4,  // 세번째 경유지
        VIA4 = 5,  // 네번째 경유지
        GOAL = 6,  // 목적지
    };

};

/** SDI 정보 **/
class RgSdiCode : public QObject {
    Q_OBJECT
    Q_ENUMS( RgSdiCodeK );
public:
    enum RgSdiCodeK {
        SPEED_ACCIDENT_POS = 0,    // 3. 신호과속      [ 타입 : 카메라 ]
        SPEED_LIMIT_POS = 1,    // 1. 과속 (고정식)    [ 타입 : 카메라 ]
        SPEED_BLOCK_START_POS = 2,   // 구간단속 시작     [ 타입 : 카메라 ]
        SPEED_BLOCK_END_POS = 3,   // 구간단속 끝      [ 타입 : 카메라 ]
        SPEED_BLOCK_MID_POS = 4,   // 구간단속 중      [ 타입 : 카메라 ]

        TAIL = 5,       // 꼬리물기단속카메라 2011.06 추가

        SIGNAL_ACCIDENT_POS = 6,   // 4. 신호 단속      [ 타입 : 카메라 ]
        SPEED_LIMIT_DANGEROUS_AREA = 7,  // 8. 과속 (이동식)     [ 타입 : 카메라 ]

        BOX_SPEED_LIMIT_POS = 8,   // 181. 박스형이동식(신규)   [ 타입 : 카메라 ]

        BUS_LANE = 9,      // 7. 버스전용차로구간    [ 타입 : 카메라 ]
        CHANGEROAD_POS = 10,    // 17. 가변 차로 단속    [ 타입 : 카메라 ]
        ROAD_CONTROL_POS = 11,    // 14. 갓길 감시 지점    [ 타입 : 카메라 ]
        INTRUDER_AREA = 12,     // 15. 끼어들기 금지    [ 타입 : 카메라 ]
        TRAFFICINFO_COLLECT_POS = 13,  // 6. 교통정보 수집지점    [ 타입 : 위치정보 ]

        CCTV_AREA = 14,      // 182. 방범용CCTV(신규)   [ 타입 : 위치정보 ]

        OVERLOAD_DANGEROUS_AREA = 15,  // 10. 과적차량 위험구간   [ 타입 : 주의구간 ]
        LOAD_BAD_CONTROL_POS = 16,   // 16. 적재 불량 단속    [ 타입 : 카메라 ]
        PARKING_CONTROL_POS = 17,   // 13. 주차단속 지점    [ 타입 : 카메라 ]
        ONE_WAY_AREA = 18,     // 179.  일방통행도로 (신규)  [ 타입 : 주의구간 ]

        //--------------------------------------------------------------------------------------------------------

        // 과속 방지턱 관련
        RAILWAY_CROSSING = 19,    // 106. 철길 건널목       [ 타입 : 주의구간 ]
        SCHOOL_ZONE_START = 20,    // 103. 어린이 보호구역(스쿨존 시작 구간) [ 타입 : 주의구간 ]
        SCHOOL_ZONE_END = 21,    // -- , 어린이 보호구역(스쿨존 끝 구간 ) [ 타입 : 주의구간 ]
        SPEEDBUMP = 22,      // 157. 과속방지턱

        //--------------------------------------------------------------------------------------------------------

        LPG_STATION = 23,     // 158. LPG충전소     [ 타입 : 위치정보 ]
        TUNNEL_AREA = 24,     // 110. 터널 구간     [ 타입 : 위치정보 ]
        SERVICE_AREA = 25,     // 153. 휴게소      [ 타입 : 위치정보 ]
        TOLLGATE = 26,      // 155. 톨게이트     [ 타입 : 위치정보 ]

        //--------------------------------------------------------------------------------------------------------

        // 우선 순위 낮춤
        FOG_AREA = 27,      // 112. 안개주의 지역      [ 타입 : 주의구간 ]
        HAZARDOUS_AREA = 28,    // 114. 유해물질 지역      [ 타입 : 주의구간 ]

        //--------------------------------------------------------------------------------------------------------

        ACCIDEDNT_AREA = 29,    // 102. 사고다발     [ 타입 : 주의구간 ]
        SHARP_CURVE_AREA = 30,    // 101. 급커브지역     [ 타입 : 주의구간 ]
        NEW_CURVE_AREA = 31,    // 178. 급커브구간1 (신규)   [ 타입 : 주의구간 ]
        SLOPE_AREA = 32,     // 180. 급경사구간 (신규)   [ 타입 : 주의구간 ]

        ROAD_KILL_AREA = 33,    // 194. 야생동물 교통사고 잦은 구간 [ 타입 : 주의구간 ]

        VISUAL_RIGHT_AREA = 34,    // 171. 우측시야불량지점(신규)  [ 타입 : 주의구간 ]
        VISUAL_FRONT_AREA = 35,    // 170. 시야불량지점(신규)   [ 타입 : 주의구간 ]
        VISUAL_LEFT_AREA = 36,    // 172. 좌측시야불량지점(신규)  [ 타입 : 주의구간 ]
        SIGNAL_VIOLATION_AREA = 37,   // 173. 신호위반다발구간(신규)  [ 타입 : 주의구간 ]
        SPEED_DRIVING_AREA = 38,   // 174. 과속운행다발구간(신규)  [ 타입 : 주의구간 ]
        TRAFFIC_CONGEST_AREA = 39,   // 175. 교통혼잡지역(신규)   [ 타입 : 주의구간 ]
        DIRECTION_LANE = 40,    // 176. 방향별차로선택지점(신규) [ 타입 : 주의구간 ]
        WALK_CROSS_AREA = 41,    // 177. 무단횡단사고다발지점(신규) [ 타입 : 주의구간 ]
        ROAD_ACCIDEDNT_AREA = 42,   // 191. 갓길 사고 다발 지점 (신규) [ 타입 : 주의구간 ]
        SPEED_ACCIDEDNT_AREA = 43,   // 192. 과속 사발 다발 지점 (신규) [ 타입 : 주의구간 ]
        SLEEP_ACCIDEDNT_AREA = 44,   // 193. 졸음 사고 다발 지점 (신규) [ 타입 : 주의구간 ]

        //--------------------------------------------------------------------------------------------------------

        ACCIDENT_POS = 45,     // 2. 사고다발지점
        PEDESTRIAN_ACCIDENT_POS = 46,  // 5. 보행자 사고다발지점
        VEHICLE_BURGLARY_POS = 47,   // 9. 차량도난사고 상습발생지점
        FALLING_AREA = 48,     // 104. 낙석주의지역
        FREEZING_AREA = 49,     // 105. 결빙주의지역
        BOTTLENECK_POINT = 50,    // 107. 병목지점
        MERGE_POINT = 51,     // 108. 합류 도로
        CRASH_AREA = 52,     // 109. 추락주의지역
        UNDERGROUND_AREA = 53,    // 111. 지하차도 구간
        TRAFFIC_CALMING_AREA = 54,   // 113. 주택밀집지역(교통진정지역)
        INTERCHANGE = 55,     // 151. 인터체인지
        JUNCTION = 56,      // 152. 분기점
        SERVICE_AREA_LPG = 57,    // 154. 휴게소(LPG충전가능)
        BRIDGE = 58,      // 156. 교량

        HWA03 = 59,       // 제동장치사고다발지점 2011.06 추가
        HWA06 = 60,       // 중앙선침범사고다발지점 2011.06 추가
        HWA09 = 61,       // 통행위반사고다발지점 2011.06 추가

        GOAL_OPPOSITE = 62,     // 목적지 건너편 안내(Tmap 전용, GP 203을 변경해서 사용)
        REST_PLACE = 63,     // 졸음 쉼터 안내(Tmap 전용, GP 150을 변경해서 사용)

        PEDESTRIAN_ACCIDENT = 64,

        UNKNOWN
    };    
};

/** TBT 종류 **/
class RgTbtType : public QObject {
    Q_OBJECT
    Q_ENUMS( RgTbtTypeK )
public:
    enum RgTbtTypeK {
        NONE = 0,
        TG = 1,   // 톨게이트
        SA = 2,   // 휴게소
        IC = 3,   // IC
        JC = 4,   // JC
        TURN = 5,  // 회전 지점
    };    
};

class RgLaneCode : public QObject {
    Q_OBJECT
    Q_DISABLE_COPY(RgLaneCode)
    Q_PROPERTY(QString L0100 READ L0100)

    Q_PROPERTY(QString L0100 READ L0100)
    Q_PROPERTY(QString L0101 READ L0101)
    Q_PROPERTY(QString L0200 READ L0200)
    Q_PROPERTY(QString L0202 READ L0202)
    Q_PROPERTY(QString L0300 READ L0300)
    Q_PROPERTY(QString L0301 READ L0301)
    Q_PROPERTY(QString L0302 READ L0302)
    Q_PROPERTY(QString L0303 READ L0303)
    Q_PROPERTY(QString L0400 READ L0400)
    Q_PROPERTY(QString L0404 READ L0404)
    Q_PROPERTY(QString L0500 READ L0500)
    Q_PROPERTY(QString L0501 READ L0501)
    Q_PROPERTY(QString L0504 READ L0504)
    Q_PROPERTY(QString L0505 READ L0505)
    Q_PROPERTY(QString L0600 READ L0600)
    Q_PROPERTY(QString L0602 READ L0602)
    Q_PROPERTY(QString L0604 READ L0604)
    Q_PROPERTY(QString L0606 READ L0606)
    Q_PROPERTY(QString L0700 READ L0700)
    Q_PROPERTY(QString L0701 READ L0701)
    Q_PROPERTY(QString L0702 READ L0702)
    Q_PROPERTY(QString L0703 READ L0703)
    Q_PROPERTY(QString L0704 READ L0704)
    Q_PROPERTY(QString L0705 READ L0705)
    Q_PROPERTY(QString L0706 READ L0706)
    Q_PROPERTY(QString L0707 READ L0707)
    Q_PROPERTY(QString L0800 READ L0800)
    Q_PROPERTY(QString L0808 READ L0808)
    Q_PROPERTY(QString L0900 READ L0900)
    Q_PROPERTY(QString L0901 READ L0901)
    Q_PROPERTY(QString L0908 READ L0908)
    Q_PROPERTY(QString L0909 READ L0909)
    Q_PROPERTY(QString L1000 READ L1000)
    Q_PROPERTY(QString L1002 READ L1002)
    Q_PROPERTY(QString L1008 READ L1008)
    Q_PROPERTY(QString L1010 READ L1010)
    Q_PROPERTY(QString L1100 READ L1100)
    Q_PROPERTY(QString L1101 READ L1101)
    Q_PROPERTY(QString L1102 READ L1102)
    Q_PROPERTY(QString L1103 READ L1103)
    Q_PROPERTY(QString L1108 READ L1108)
    Q_PROPERTY(QString L1109 READ L1109)
    Q_PROPERTY(QString L1110 READ L1110)
    Q_PROPERTY(QString L1111 READ L1111)
    Q_PROPERTY(QString L1200 READ L1200)
    Q_PROPERTY(QString L1204 READ L1204)
    Q_PROPERTY(QString L1208 READ L1208)
    Q_PROPERTY(QString L1212 READ L1212)
    Q_PROPERTY(QString L1300 READ L1300)
    Q_PROPERTY(QString L1301 READ L1301)
    Q_PROPERTY(QString L1304 READ L1304)
    Q_PROPERTY(QString L1305 READ L1305)
    Q_PROPERTY(QString L1308 READ L1308)
    Q_PROPERTY(QString L1309 READ L1309)
    Q_PROPERTY(QString L1312 READ L1312)
    Q_PROPERTY(QString L1313 READ L1313)
    Q_PROPERTY(QString L1400 READ L1400)
    Q_PROPERTY(QString L1402 READ L1402)
    Q_PROPERTY(QString L1404 READ L1404)
    Q_PROPERTY(QString L1406 READ L1406)
    Q_PROPERTY(QString L1408 READ L1408)
    Q_PROPERTY(QString L1410 READ L1410)
    Q_PROPERTY(QString L1412 READ L1412)
    Q_PROPERTY(QString L1414 READ L1414)
    Q_PROPERTY(QString L1500 READ L1500)
    Q_PROPERTY(QString L1501 READ L1501)
    Q_PROPERTY(QString L1502 READ L1502)
    Q_PROPERTY(QString L1503 READ L1503)
    Q_PROPERTY(QString L1504 READ L1504)
    Q_PROPERTY(QString L1505 READ L1505)
    Q_PROPERTY(QString L1506 READ L1506)
    Q_PROPERTY(QString L1507 READ L1507)
    Q_PROPERTY(QString L1508 READ L1508)
    Q_PROPERTY(QString L1509 READ L1509)
    Q_PROPERTY(QString L1510 READ L1510)
    Q_PROPERTY(QString L1511 READ L1511)
    Q_PROPERTY(QString L1512 READ L1512)
    Q_PROPERTY(QString L1513 READ L1513)
    Q_PROPERTY(QString L1514 READ L1514)
    Q_PROPERTY(QString L1515 READ L1515)
    Q_PROPERTY(QString L1600 READ L1600)
    Q_PROPERTY(QString L1616 READ L1616)
    Q_PROPERTY(QString L1700 READ L1700)
    Q_PROPERTY(QString L1701 READ L1701)
    Q_PROPERTY(QString L1716 READ L1716)
    Q_PROPERTY(QString L1717 READ L1717)
    Q_PROPERTY(QString L1800 READ L1800)
    Q_PROPERTY(QString L1802 READ L1802)
    Q_PROPERTY(QString L1816 READ L1816)
    Q_PROPERTY(QString L1818 READ L1818)
    Q_PROPERTY(QString L1900 READ L1900)
    Q_PROPERTY(QString L1901 READ L1901)
    Q_PROPERTY(QString L1902 READ L1902)
    Q_PROPERTY(QString L1903 READ L1903)
    Q_PROPERTY(QString L1908 READ L1908)
    Q_PROPERTY(QString L1909 READ L1909)
    Q_PROPERTY(QString L1910 READ L1910)
    Q_PROPERTY(QString L1911 READ L1911)
    Q_PROPERTY(QString L2000 READ L2000)
    Q_PROPERTY(QString L2004 READ L2004)
    Q_PROPERTY(QString L2016 READ L2016)
    Q_PROPERTY(QString L2020 READ L2020)
    Q_PROPERTY(QString L2100 READ L2100)
    Q_PROPERTY(QString L2101 READ L2101)
    Q_PROPERTY(QString L2108 READ L2108)
    Q_PROPERTY(QString L2109 READ L2109)
    Q_PROPERTY(QString L2200 READ L2200)
    Q_PROPERTY(QString L2202 READ L2202)
    Q_PROPERTY(QString L2204 READ L2204)
    Q_PROPERTY(QString L2206 READ L2206)
    Q_PROPERTY(QString L2216 READ L2216)
    Q_PROPERTY(QString L2218 READ L2218)
    Q_PROPERTY(QString L2220 READ L2220)
    Q_PROPERTY(QString L2222 READ L2222)
    Q_PROPERTY(QString L2300 READ L2300)
    Q_PROPERTY(QString L2301 READ L2301)
    Q_PROPERTY(QString L2302 READ L2302)
    Q_PROPERTY(QString L2303 READ L2303)
    Q_PROPERTY(QString L2308 READ L2308)
    Q_PROPERTY(QString L2309 READ L2309)
    Q_PROPERTY(QString L2310 READ L2310)
    Q_PROPERTY(QString L2311 READ L2311)
    Q_PROPERTY(QString L2400 READ L2400)
    Q_PROPERTY(QString L2408 READ L2408)
    Q_PROPERTY(QString L2416 READ L2416)
    Q_PROPERTY(QString L2424 READ L2424)
    Q_PROPERTY(QString L2500 READ L2500)
    Q_PROPERTY(QString L2501 READ L2501)
    Q_PROPERTY(QString L2508 READ L2508)
    Q_PROPERTY(QString L2509 READ L2509)
    Q_PROPERTY(QString L2516 READ L2516)
    Q_PROPERTY(QString L2517 READ L2517)
    Q_PROPERTY(QString L2524 READ L2524)
    Q_PROPERTY(QString L2525 READ L2525)
    Q_PROPERTY(QString L2600 READ L2600)
    Q_PROPERTY(QString L2602 READ L2602)
    Q_PROPERTY(QString L2608 READ L2608)
    Q_PROPERTY(QString L2610 READ L2610)
    Q_PROPERTY(QString L2616 READ L2616)
    Q_PROPERTY(QString L2618 READ L2618)
    Q_PROPERTY(QString L2624 READ L2624)
    Q_PROPERTY(QString L2626 READ L2626)
    Q_PROPERTY(QString L2700 READ L2700)
    Q_PROPERTY(QString L2701 READ L2701)
    Q_PROPERTY(QString L2702 READ L2702)
    Q_PROPERTY(QString L2703 READ L2703)
    Q_PROPERTY(QString L2708 READ L2708)
    Q_PROPERTY(QString L2709 READ L2709)
    Q_PROPERTY(QString L2710 READ L2710)
    Q_PROPERTY(QString L2711 READ L2711)
    Q_PROPERTY(QString L2800 READ L2800)
    Q_PROPERTY(QString L2808 READ L2808)
    Q_PROPERTY(QString L2900 READ L2900)
    Q_PROPERTY(QString L2901 READ L2901)
    Q_PROPERTY(QString L2908 READ L2908)
    Q_PROPERTY(QString L2909 READ L2909)
    Q_PROPERTY(QString L3000 READ L3000)
    Q_PROPERTY(QString L3002 READ L3002)
    Q_PROPERTY(QString L3008 READ L3008)
    Q_PROPERTY(QString L3010 READ L3010)
    Q_PROPERTY(QString L3100 READ L3100)
    Q_PROPERTY(QString L3101 READ L3101)
    Q_PROPERTY(QString L3102 READ L3102)
    Q_PROPERTY(QString L3103 READ L3103)
    Q_PROPERTY(QString L3108 READ L3108)
    Q_PROPERTY(QString L3109 READ L3109)
    Q_PROPERTY(QString L3110 READ L3110)
    Q_PROPERTY(QString L3111 READ L3111)
    Q_PROPERTY(QString L3200 READ L3200)
    Q_PROPERTY(QString L3232 READ L3232)
    Q_PROPERTY(QString L3300 READ L3300)
    Q_PROPERTY(QString L3301 READ L3301)
    Q_PROPERTY(QString L3332 READ L3332)
    Q_PROPERTY(QString L3333 READ L3333)
    Q_PROPERTY(QString L3400 READ L3400)
    Q_PROPERTY(QString L3402 READ L3402)
    Q_PROPERTY(QString L3432 READ L3432)
    Q_PROPERTY(QString L3434 READ L3434)
    Q_PROPERTY(QString L3500 READ L3500)
    Q_PROPERTY(QString L3502 READ L3502)
    Q_PROPERTY(QString L3532 READ L3532)
    Q_PROPERTY(QString L3534 READ L3534)
    Q_PROPERTY(QString L3600 READ L3600)
    Q_PROPERTY(QString L3602 READ L3602)
    Q_PROPERTY(QString L3632 READ L3632)
    Q_PROPERTY(QString L3634 READ L3634)
    Q_PROPERTY(QString L3700 READ L3700)
    Q_PROPERTY(QString L3708 READ L3708)
    Q_PROPERTY(QString L3732 READ L3732)
    Q_PROPERTY(QString L3740 READ L3740)
    Q_PROPERTY(QString L3800 READ L3800)
    Q_PROPERTY(QString L3808 READ L3808)
    Q_PROPERTY(QString L3832 READ L3832)
    Q_PROPERTY(QString L3840 READ L3840)
    Q_PROPERTY(QString L3900 READ L3900)
    Q_PROPERTY(QString L3901 READ L3901)
    Q_PROPERTY(QString L3902 READ L3902)
    Q_PROPERTY(QString L3903 READ L3903)
    Q_PROPERTY(QString L3904 READ L3904)
    Q_PROPERTY(QString L3905 READ L3905)
    Q_PROPERTY(QString L3906 READ L3906)
    Q_PROPERTY(QString L3907 READ L3907)
    Q_PROPERTY(QString L3932 READ L3932)
    Q_PROPERTY(QString L3933 READ L3933)
    Q_PROPERTY(QString L3934 READ L3934)
    Q_PROPERTY(QString L3935 READ L3935)
    Q_PROPERTY(QString L3936 READ L3936)
    Q_PROPERTY(QString L3937 READ L3937)
    Q_PROPERTY(QString L3938 READ L3938)
    Q_PROPERTY(QString L3939 READ L3939)
    Q_PROPERTY(QString L4000 READ L4000)
    Q_PROPERTY(QString L4008 READ L4008)
    Q_PROPERTY(QString L4032 READ L4032)
    Q_PROPERTY(QString L4040 READ L4040)
    Q_PROPERTY(QString L4100 READ L4100)
    Q_PROPERTY(QString L4101 READ L4101)
    Q_PROPERTY(QString L4108 READ L4108)
    Q_PROPERTY(QString L4109 READ L4109)
    Q_PROPERTY(QString L4132 READ L4132)
    Q_PROPERTY(QString L4133 READ L4133)
    Q_PROPERTY(QString L4140 READ L4140)
    Q_PROPERTY(QString L4141 READ L4141)
    Q_PROPERTY(QString L4200 READ L4200)
    Q_PROPERTY(QString L4201 READ L4201)
    Q_PROPERTY(QString L4208 READ L4208)
    Q_PROPERTY(QString L4209 READ L4209)
    Q_PROPERTY(QString L4232 READ L4232)
    Q_PROPERTY(QString L4233 READ L4233)
    Q_PROPERTY(QString L4240 READ L4240)
    Q_PROPERTY(QString L4241 READ L4241)
    Q_PROPERTY(QString L4300 READ L4300)
    Q_PROPERTY(QString L4301 READ L4301)
    Q_PROPERTY(QString L4302 READ L4302)
    Q_PROPERTY(QString L4303 READ L4303)
    Q_PROPERTY(QString L4308 READ L4308)
    Q_PROPERTY(QString L4309 READ L4309)
    Q_PROPERTY(QString L4310 READ L4310)
    Q_PROPERTY(QString L4311 READ L4311)
    Q_PROPERTY(QString L4332 READ L4332)
    Q_PROPERTY(QString L4333 READ L4333)
    Q_PROPERTY(QString L4334 READ L4334)
    Q_PROPERTY(QString L4335 READ L4335)
    Q_PROPERTY(QString L4340 READ L4340)
    Q_PROPERTY(QString L4341 READ L4341)
    Q_PROPERTY(QString L4342 READ L4342)
    Q_PROPERTY(QString L4343 READ L4343)
    Q_PROPERTY(QString L4400 READ L4400)
    Q_PROPERTY(QString L4404 READ L4404)
    Q_PROPERTY(QString L4408 READ L4408)
    Q_PROPERTY(QString L4412 READ L4412)
    Q_PROPERTY(QString L4432 READ L4432)
    Q_PROPERTY(QString L4436 READ L4436)
    Q_PROPERTY(QString L4440 READ L4440)
    Q_PROPERTY(QString L4444 READ L4444)
    Q_PROPERTY(QString L4500 READ L4500)
    Q_PROPERTY(QString L4508 READ L4508)
    Q_PROPERTY(QString L4532 READ L4532)
    Q_PROPERTY(QString L4540 READ L4540)
    Q_PROPERTY(QString L4600 READ L4600)
    Q_PROPERTY(QString L4608 READ L4608)
    Q_PROPERTY(QString L4632 READ L4632)
    Q_PROPERTY(QString L4640 READ L4640)
    Q_PROPERTY(QString L4700 READ L4700)
    Q_PROPERTY(QString L4708 READ L4708)
    Q_PROPERTY(QString L4732 READ L4732)
    Q_PROPERTY(QString L4740 READ L4740)
    Q_PROPERTY(QString L4800 READ L4800)
    Q_PROPERTY(QString L4816 READ L4816)
    Q_PROPERTY(QString L4832 READ L4832)
    Q_PROPERTY(QString L4848 READ L4848)
    Q_PROPERTY(QString L4900 READ L4900)
    Q_PROPERTY(QString L4901 READ L4901)
    Q_PROPERTY(QString L4916 READ L4916)
    Q_PROPERTY(QString L4917 READ L4917)
    Q_PROPERTY(QString L4932 READ L4932)
    Q_PROPERTY(QString L4933 READ L4933)
    Q_PROPERTY(QString L4948 READ L4948)
    Q_PROPERTY(QString L4949 READ L4949)
    Q_PROPERTY(QString L5000 READ L5000)
    Q_PROPERTY(QString L5002 READ L5002)
    Q_PROPERTY(QString L5016 READ L5016)
    Q_PROPERTY(QString L5018 READ L5018)
    Q_PROPERTY(QString L5032 READ L5032)
    Q_PROPERTY(QString L5034 READ L5034)
    Q_PROPERTY(QString L5048 READ L5048)
    Q_PROPERTY(QString L5050 READ L5050)
    Q_PROPERTY(QString L5100 READ L5100)
    Q_PROPERTY(QString L5108 READ L5108)
    Q_PROPERTY(QString L5132 READ L5132)
    Q_PROPERTY(QString L5140 READ L5140)
    Q_PROPERTY(QString L5200 READ L5200)
    Q_PROPERTY(QString L5204 READ L5204)
    Q_PROPERTY(QString L5216 READ L5216)
    Q_PROPERTY(QString L5220 READ L5220)
    Q_PROPERTY(QString L5232 READ L5232)
    Q_PROPERTY(QString L5236 READ L5236)
    Q_PROPERTY(QString L5248 READ L5248)
    Q_PROPERTY(QString L5252 READ L5252)
    Q_PROPERTY(QString L5300 READ L5300)
    Q_PROPERTY(QString L5308 READ L5308)
    Q_PROPERTY(QString L5332 READ L5332)
    Q_PROPERTY(QString L5340 READ L5340)
    Q_PROPERTY(QString L5400 READ L5400)
    Q_PROPERTY(QString L5408 READ L5408)
    Q_PROPERTY(QString L5432 READ L5432)
    Q_PROPERTY(QString L5440 READ L5440)
    Q_PROPERTY(QString L5500 READ L5500)
    Q_PROPERTY(QString L5508 READ L5508)
    Q_PROPERTY(QString L5532 READ L5532)
    Q_PROPERTY(QString L5540 READ L5540)
    Q_PROPERTY(QString L5600 READ L5600)
    Q_PROPERTY(QString L5608 READ L5608)
    Q_PROPERTY(QString L5616 READ L5616)
    Q_PROPERTY(QString L5624 READ L5624)
    Q_PROPERTY(QString L5632 READ L5632)
    Q_PROPERTY(QString L5640 READ L5640)
    Q_PROPERTY(QString L5648 READ L5648)
    Q_PROPERTY(QString L5656 READ L5656)
    Q_PROPERTY(QString L5700 READ L5700)
    Q_PROPERTY(QString L5708 READ L5708)
    Q_PROPERTY(QString L5732 READ L5732)
    Q_PROPERTY(QString L5740 READ L5740)
    Q_PROPERTY(QString L5800 READ L5800)
    Q_PROPERTY(QString L5802 READ L5802)
    Q_PROPERTY(QString L5808 READ L5808)
    Q_PROPERTY(QString L5810 READ L5810)
    Q_PROPERTY(QString L5816 READ L5816)
    Q_PROPERTY(QString L5818 READ L5818)
    Q_PROPERTY(QString L5824 READ L5824)
    Q_PROPERTY(QString L5826 READ L5826)
    Q_PROPERTY(QString L5832 READ L5832)
    Q_PROPERTY(QString L5834 READ L5834)
    Q_PROPERTY(QString L5840 READ L5840)
    Q_PROPERTY(QString L5842 READ L5842)
    Q_PROPERTY(QString L5848 READ L5848)
    Q_PROPERTY(QString L5850 READ L5850)
    Q_PROPERTY(QString L5856 READ L5856)
    Q_PROPERTY(QString L5858 READ L5858)
    Q_PROPERTY(QString L5900 READ L5900)
    Q_PROPERTY(QString L5908 READ L5908)
    Q_PROPERTY(QString L5932 READ L5932)
    Q_PROPERTY(QString L5940 READ L5940)
    Q_PROPERTY(QString L6000 READ L6000)
    Q_PROPERTY(QString L6008 READ L6008)
    Q_PROPERTY(QString L6032 READ L6032)
    Q_PROPERTY(QString L6040 READ L6040)
    Q_PROPERTY(QString L6100 READ L6100)
    Q_PROPERTY(QString L6108 READ L6108)
    Q_PROPERTY(QString L6132 READ L6132)
    Q_PROPERTY(QString L6140 READ L6140)
    Q_PROPERTY(QString L6200 READ L6200)
    Q_PROPERTY(QString L6208 READ L6208)
    Q_PROPERTY(QString L6232 READ L6232)
    Q_PROPERTY(QString L6240 READ L6240)
    Q_PROPERTY(QString L6300 READ L6300)
    Q_PROPERTY(QString L6308 READ L6308)
    Q_PROPERTY(QString L6332 READ L6332)
    Q_PROPERTY(QString L6340 READ L6340)

public:
    RgLaneCode() {
    }

    static RgLaneCode* instance() {
        static RgLaneCode Instance;
        return &Instance;
    }

    static QObject *qmlInstance(QQmlEngine *engine, QJSEngine *scriptEngine) {
        Q_UNUSED(engine);
        Q_UNUSED(scriptEngine);

        return new RgLaneCode;
    }

    QString L0100() { return QStringLiteral( "0100" ); } //유턴 비활성화
    QString L0101() { return QStringLiteral( "0101" ); } //유턴 활성화
    QString L0200() { return QStringLiteral( "0200" ); } //좌회전 비활성화
    QString L0202() { return QStringLiteral( "0202" ); } //좌회전 활성화
    QString L0300() { return QStringLiteral( "0300" ); } //좌회전+유턴 : 비활성화
    QString L0301() { return QStringLiteral( "0301" ); } //좌회전+유턴 : 유턴 활성화
    QString L0302() { return QStringLiteral( "0302" ); } //좌회전+유턴 : 좌회전 활성화
    QString L0303() { return QStringLiteral( "0303" ); } //좌회전+유턴 : 활성화
    QString L0400() { return QStringLiteral( "0400" ); } //10시 방향 좌회전 비활성화
    QString L0404() { return QStringLiteral( "0404" ); } //10시 방향 좌회전 활성화
    QString L0500() { return QStringLiteral( "0500" ); } //10시 방향 좌회전 + 유턴 : 비활성화
    QString L0501() { return QStringLiteral( "0501" ); } //10시 방향 좌회전 + 유턴 : 유턴 활성화
    QString L0504() { return QStringLiteral( "0504" ); } //10시 방향 좌회전 + 유턴 : 10시 방향 좌회전 활성화
    QString L0505() { return QStringLiteral( "0505" ); } //10시 방향 좌회전 + 유턴 : 활성화
    QString L0600() { return QStringLiteral( "0600" ); } //10시 방향 좌회전 + 좌회전 : 비활성화
    QString L0602() { return QStringLiteral( "0602" ); } //10시 방향 좌회전 + 좌회전 : 좌회전 활성화
    QString L0604() { return QStringLiteral( "0604" ); } //10시 방향 좌회전 + 좌회전 : 10시 방향 좌회전 활성화
    QString L0606() { return QStringLiteral( "0606" ); } //10시 방향 좌회전 + 좌회전 : 활성화
    QString L0700() { return QStringLiteral( "0700" ); } //10시 방향 좌회전 + 좌회전 + 유턴 : 비활성화
    QString L0701() { return QStringLiteral( "0701" ); } //10시 방향 좌회전 + 좌회전 + 유턴 : 유턴 활성화
    QString L0702() { return QStringLiteral( "0702" ); } //10시 방향 좌회전 + 좌회전 + 유턴 : 좌회전 활성화
    QString L0703() { return QStringLiteral( "0703" ); } //10시 방향 좌회전 + 좌회전 + 유턴 : 좌회전 + 유턴 활성화
    QString L0704() { return QStringLiteral( "0704" ); } //10시 방향 좌회전 + 좌회전 + 유턴 : 10시 방향 좌회전 활성화
    QString L0705() { return QStringLiteral( "0705" ); } //10시 방향 좌회전 + 좌회전 + 유턴 : 10시 방향 좌회전 + 유턴 활성화
    QString L0706() { return QStringLiteral( "0706" ); } //10시 방향 좌회전 + 좌회전 + 유턴 : 10시 방향 좌회전 + 좌회전 활성화
    QString L0707() { return QStringLiteral( "0707" ); } //10시 방향 좌회전 + 좌회전 + 유턴 : 활성화
    QString L0800() { return QStringLiteral( "0800" ); } //직진 비활성화
    QString L0808() { return QStringLiteral( "0808" ); } //직진 활성화
    QString L0900() { return QStringLiteral( "0900" ); } //직진 + 유턴 비활성화
    QString L0901() { return QStringLiteral( "0901" ); } //직진 + 유턴 : 유턴 활성화
    QString L0908() { return QStringLiteral( "0908" ); } //직진 + 유턴 : 직진 활성화
    QString L0909() { return QStringLiteral( "0909" ); } //직진 + 유턴 활성화
    QString L1000() { return QStringLiteral( "1000" ); } //직진 + 좌회전 비활성화
    QString L1002() { return QStringLiteral( "1002" ); } //직진 + 좌회전 : 좌회전 활성화
    QString L1008() { return QStringLiteral( "1008" ); } //직진 + 좌회전 : 직진 활성화
    QString L1010() { return QStringLiteral( "1010" ); } //직진 + 좌회전 활성화
    QString L1100() { return QStringLiteral( "1100" ); } //직진 + 유턴 + 좌회전  비활성화
    QString L1101() { return QStringLiteral( "1101" ); } //직진 + 유턴 + 좌회전 : 유턴 활성화
    QString L1102() { return QStringLiteral( "1102" ); } //직진 + 유턴 + 좌회전 : 좌회전 활성화
    QString L1103() { return QStringLiteral( "1103" ); } //직진 + 유턴 + 좌회전 : 좌회전 + 유턴 활성화
    QString L1108() { return QStringLiteral( "1108" ); } //직진 + 유턴 + 좌회전 : 직진 활성화
    QString L1109() { return QStringLiteral( "1109" ); } //직진 + 유턴 + 좌회전 : 직진 + 유턴 활성화
    QString L1110() { return QStringLiteral( "1110" ); } //직진 + 유턴 + 좌회전 : 직진 + 좌회전 활성화
    QString L1111() { return QStringLiteral( "1111" ); } //직진 + 유턴 + 좌회전 활성화
    QString L1200() { return QStringLiteral( "1200" ); } //직진 + 10시방향 좌회전 : 비활성화
    QString L1204() { return QStringLiteral( "1204" ); } //직진 + 10시방향 좌회전 : 10시 방향 좌회전 활성화
    QString L1208() { return QStringLiteral( "1208" ); } //직진 + 10시방향 좌회전 : 직진 활성화
    QString L1212() { return QStringLiteral( "1212" ); } //직진 + 10시방향 좌회전 : 활성화
    QString L1300() { return QStringLiteral( "1300" ); } //직진 + 10시방향 좌회전 + 유턴 : 비활성화
    QString L1301() { return QStringLiteral( "1301" ); } //직진 + 10시방향 좌회전 + 유턴 : 유턴 활성화
    QString L1304() { return QStringLiteral( "1304" ); } //직진 + 10시방향 좌회전 + 유턴 : 10시방향 좌회전 활성화
    QString L1305() { return QStringLiteral( "1305" ); } //직진 + 10시방향 좌회전 + 유턴 : 10시방향 좌회전 + 유턴 활성화
    QString L1308() { return QStringLiteral( "1308" ); } //직진 + 10시방향 좌회전 + 유턴 : 직진 활성화
    QString L1309() { return QStringLiteral( "1309" ); } //직진 + 10시방향 좌회전 + 유턴 : 직진 + 유턴 활성화
    QString L1312() { return QStringLiteral( "1312" ); } //직진 + 10시방향 좌회전 + 유턴 : 직진 + 10시방향 좌회전 활성화
    QString L1313() { return QStringLiteral( "1313" ); } //직진 + 10시방향 좌회전 + 유턴 : 활성화
    QString L1400() { return QStringLiteral( "1400" ); } //직진 + 10시방향 좌회전 + 좌회전 비활성화
    QString L1402() { return QStringLiteral( "1402" ); } //직진 + 10시방향 좌회전 + 좌회전 : 좌회전 활성화
    QString L1404() { return QStringLiteral( "1404" ); } //직진 + 10시방향 좌회전 + 좌회전 : 10시방향 좌회전 활성화
    QString L1406() { return QStringLiteral( "1406" ); } //직진 + 10시방향 좌회전 + 좌회전 : 좌회전 + 10시방향 좌회전 활성화
    QString L1408() { return QStringLiteral( "1408" ); } //직진 + 10시방향 좌회전 + 좌회전 : 직진 활성화
    QString L1410() { return QStringLiteral( "1410" ); } //직진 + 10시방향 좌회전 + 좌회전 : 직진 + 좌회전 활성화
    QString L1412() { return QStringLiteral( "1412" ); } //직진 + 10시방향 좌회전 + 좌회전 : 직진 + 10시방향 좌회전 활성화
    QString L1414() { return QStringLiteral( "1414" ); } //직진 + 10시방향 좌회전 + 좌회전 활성화
    QString L1500() { return QStringLiteral( "1500" ); } //직진 + 10시방향 좌회전 + 좌회전 + 유턴 비활성화
    QString L1501() { return QStringLiteral( "1501" ); } //직진 + 10시방향 좌회전 + 좌회전 + 유턴  : 유턴 활성화
    QString L1502() { return QStringLiteral( "1502" ); } //직진 + 10시방향 좌회전 + 좌회전 + 유턴  : 좌회전 활성화
    QString L1503() { return QStringLiteral( "1503" ); } //직진 + 10시방향 좌회전 + 좌회전 + 유턴  : 좌회전 + 유턴 활성화
    QString L1504() { return QStringLiteral( "1504" ); } //직진 + 10시방향 좌회전 + 좌회전 + 유턴  : 10시방향 좌회전 활성화
    QString L1505() { return QStringLiteral( "1505" ); } //직진 + 10시방향 좌회전 + 좌회전 + 유턴  : 10시방향 좌회전 + 유턴 활성화
    QString L1506() { return QStringLiteral( "1506" ); } //직진 + 10시방향 좌회전 + 좌회전 + 유턴  : 10시방향 좌회전 + 좌회전 활성화
    QString L1507() { return QStringLiteral( "1507" ); } //직진 + 10시방향 좌회전 + 좌회전 + 유턴  : 10시방향 좌회전 + 좌회전 + 유턴 활성화
    QString L1508() { return QStringLiteral( "1508" ); } //직진 + 10시방향 좌회전 + 좌회전 + 유턴  : 직진 활성화
    QString L1509() { return QStringLiteral( "1509" ); } //직진 + 10시방향 좌회전 + 좌회전 + 유턴  : 직진 + 유턴 활성화
    QString L1510() { return QStringLiteral( "1510" ); } //직진 + 10시방향 좌회전 + 좌회전 + 유턴  : 직진 + 좌회전 활성화
    QString L1511() { return QStringLiteral( "1511" ); } //직진 + 10시방향 좌회전 + 좌회전 + 유턴  : 직진 + 좌회전 + 유턴 활성화
    QString L1512() { return QStringLiteral( "1512" ); } //직진 + 10시방향 좌회전 + 좌회전 + 유턴  : 직진 + 10시방향 좌회전 활성화
    QString L1513() { return QStringLiteral( "1513" ); } //직진 + 10시방향 좌회전 + 좌회전 + 유턴  : 직진 + 10시방향 좌회전 +유턴 활성화
    QString L1514() { return QStringLiteral( "1514" ); } //직진 + 10시방향 좌회전 + 좌회전 + 유턴  : 직진 + 10시방향 좌회전 + 좌회전 활성화
    QString L1515() { return QStringLiteral( "1515" ); } //직진 + 10시방향 좌회전 + 좌회전 + 유턴 활성화
    QString L1600() { return QStringLiteral( "1600" ); } //2시방향 우회전 비활성화
    QString L1616() { return QStringLiteral( "1616" ); } //2시방향 우회전 활성화
    QString L1700() { return QStringLiteral( "1700" ); } //2시방향 우회전 + 유턴 비활성화
    QString L1701() { return QStringLiteral( "1701" ); } //2시방향 우회전 + 유턴 : 유턴 활성화
    QString L1716() { return QStringLiteral( "1716" ); } //2시방향 우회전 + 유턴 : 2시방향 우회전 활성화
    QString L1717() { return QStringLiteral( "1717" ); } //2시방향 우회전 + 유턴 활성화
    QString L1800() { return QStringLiteral( "1800" ); } //2시방향 우회전 + 좌회전 비활성화
    QString L1802() { return QStringLiteral( "1802" ); } //2시방향 우회전 + 좌회전 : 좌회전 활성화
    QString L1816() { return QStringLiteral( "1816" ); } //2시방향 우회전 + 좌회전 : 2시방향 우회전 활성화
    QString L1818() { return QStringLiteral( "1818" ); } //2시방향 우회전 + 좌회전 활성화
    QString L1900() { return QStringLiteral( "1900" ); } //직진 + 유턴 + 좌회전  비활성화
    QString L1901() { return QStringLiteral( "1901" ); } //직진 + 유턴 + 좌회전 : 유턴 활성화
    QString L1902() { return QStringLiteral( "1902" ); } //직진 + 유턴 + 좌회전 : 좌회전 활성화
    QString L1903() { return QStringLiteral( "1903" ); } //직진 + 유턴 + 좌회전 : 좌회전 + 유턴 활성화
    QString L1908() { return QStringLiteral( "1908" ); } //직진 + 유턴 + 좌회전 : 직진 활성화
    QString L1909() { return QStringLiteral( "1909" ); } //직진 + 유턴 + 좌회전 : 직진 + 유턴 활성화
    QString L1910() { return QStringLiteral( "1910" ); } //직진 + 유턴 + 좌회전 : 직진 + 좌회전 활성화
    QString L1911() { return QStringLiteral( "1911" ); } //직진 + 유턴 + 좌회전 활성화
    QString L2000() { return QStringLiteral( "2000" ); } //2시방향 우회전 + 10시방향 좌회전 비활성화
    QString L2004() { return QStringLiteral( "2004" ); } //2시방향 우회전 + 10시방향 좌회전 : 10시방향 좌회전 활성화
    QString L2016() { return QStringLiteral( "2016" ); } //2시방향 우회전 + 10시방향 좌회전 : 2시방향 우회전 활성화
    QString L2020() { return QStringLiteral( "2020" ); } //2시방향 우회전 + 10시방향 좌회전 활성화
    QString L2100() { return QStringLiteral( "2100" ); } //직진 + 유턴 비활성화
    QString L2101() { return QStringLiteral( "2101" ); } //직진 + 유턴 : 유턴 활성화
    QString L2108() { return QStringLiteral( "2108" ); } //직진 + 유턴 : 직진 활성화
    QString L2109() { return QStringLiteral( "2109" ); } //직진 + 유턴 활성화
    QString L2200() { return QStringLiteral( "2200" ); } //2시방향 우회전 + 10시방향 좌회전 + 좌회전 비활성화
    QString L2202() { return QStringLiteral( "2202" ); } //2시방향 우회전 + 10시방향 좌회전 + 좌회전 : 좌회전  활성화
    QString L2204() { return QStringLiteral( "2204" ); } //2시방향 우회전 + 10시방향 좌회전 + 좌회전 : 10시방향 좌회전  활성화
    QString L2206() { return QStringLiteral( "2206" ); } //2시방향 우회전 + 10시방향 좌회전 + 좌회전 : 10시방향 좌회전 + 좌회전 활성화
    QString L2216() { return QStringLiteral( "2216" ); } //2시방향 우회전 + 10시방향 좌회전 + 좌회전 : 2시방향 좌회전 활성화
    QString L2218() { return QStringLiteral( "2218" ); } //2시방향 우회전 + 10시방향 좌회전 + 좌회전 : 2시방향 좌회전 + 좌회전 활성화
    QString L2220() { return QStringLiteral( "2220" ); } //2시방향 우회전 + 10시방향 좌회전 + 좌회전 : 2시방향 좌회전 + 10시방향 좌회전 활성화
    QString L2222() { return QStringLiteral( "2222" ); } //2시방향 우회전 + 10시방향 좌회전 + 좌회전 활성화
    QString L2300() { return QStringLiteral( "2300" ); } //직진 + 유턴 + 좌회전  비활성화
    QString L2301() { return QStringLiteral( "2301" ); } //직진 + 유턴 + 좌회전 : 유턴 활성화
    QString L2302() { return QStringLiteral( "2302" ); } //직진 + 유턴 + 좌회전 : 좌회전 활성화
    QString L2303() { return QStringLiteral( "2303" ); } //직진 + 유턴 + 좌회전 : 좌회전 + 유턴 활성화
    QString L2308() { return QStringLiteral( "2308" ); } //직진 + 유턴 + 좌회전 : 직진 활성화
    QString L2309() { return QStringLiteral( "2309" ); } //직진 + 유턴 + 좌회전 : 직진 + 유턴 활성화
    QString L2310() { return QStringLiteral( "2310" ); } //직진 + 유턴 + 좌회전 : 직진 + 좌회전 활성화
    QString L2311() { return QStringLiteral( "2311" ); } //직진 + 유턴 + 좌회전 활성화
    QString L2400() { return QStringLiteral( "2400" ); } //2시 방향 우회전 + 직진 비활성화
    QString L2408() { return QStringLiteral( "2408" ); } //2시 방향 우회전 + 직진 : 직진 활성화
    QString L2416() { return QStringLiteral( "2416" ); } //2시 방향 우회전 + 직진 : 2시 방향 우회전 활성화
    QString L2424() { return QStringLiteral( "2424" ); } //2시 방향 우회전 + 직진 활성화
    QString L2500() { return QStringLiteral( "2500" ); } //2시 방향 우회전 + 직진 + 유턴 비활성화
    QString L2501() { return QStringLiteral( "2501" ); } //2시 방향 우회전 + 직진 + 유턴 : 유턴 활성화
    QString L2508() { return QStringLiteral( "2508" ); } //2시 방향 우회전 + 직진 + 유턴 : 직진 활성화
    QString L2509() { return QStringLiteral( "2509" ); } //2시 방향 우회전 + 직진 + 유턴 : 직진 + 유턴 활성화
    QString L2516() { return QStringLiteral( "2516" ); } //2시 방향 우회전 + 직진 + 유턴 : 2시 방향 우회전 활성화
    QString L2517() { return QStringLiteral( "2517" ); } //2시 방향 우회전 + 직진 + 유턴 : 2시 방향 우회전 + 유턴 활성화
    QString L2524() { return QStringLiteral( "2524" ); } //2시 방향 우회전 + 직진 + 유턴 : 2시 방향 우회전 + 직진 활성화
    QString L2525() { return QStringLiteral( "2525" ); } //2시 방향 우회전 + 직진 + 유턴 활성화
    QString L2600() { return QStringLiteral( "2600" ); } //2시 방향 우회전 + 직진 + 좌회전 비활성화
    QString L2602() { return QStringLiteral( "2602" ); } //2시 방향 우회전 + 직진 + 좌회전 : 좌회전 활성화
    QString L2608() { return QStringLiteral( "2608" ); } //2시 방향 우회전 + 직진 + 좌회전 : 직진 활성화
    QString L2610() { return QStringLiteral( "2610" ); } //2시 방향 우회전 + 직진 + 좌회전 : 직진 + 좌회전 활성화
    QString L2616() { return QStringLiteral( "2616" ); } //2시 방향 우회전 + 직진 + 좌회전 : 2시 방향 우회전 활성화
    QString L2618() { return QStringLiteral( "2618" ); } //2시 방향 우회전 + 직진 + 좌회전 : 2시 방향 우회전 + 좌회전 활성화
    QString L2624() { return QStringLiteral( "2624" ); } //2시 방향 우회전 + 직진 + 좌회전 : 2시 방향 우회전 + 직진 활성화
    QString L2626() { return QStringLiteral( "2626" ); } //2시 방향 우회전 + 직진 + 좌회전 활성화
    QString L2700() { return QStringLiteral( "2700" ); } //직진 + 유턴 + 좌회전  비활성화
    QString L2701() { return QStringLiteral( "2701" ); } //직진 + 유턴 + 좌회전 : 유턴 활성화
    QString L2702() { return QStringLiteral( "2702" ); } //직진 + 유턴 + 좌회전 : 좌회전 활성화
    QString L2703() { return QStringLiteral( "2703" ); } //직진 + 유턴 + 좌회전 : 좌회전 + 유턴 활성화
    QString L2708() { return QStringLiteral( "2708" ); } //직진 + 유턴 + 좌회전 : 직진 활성화
    QString L2709() { return QStringLiteral( "2709" ); } //직진 + 유턴 + 좌회전 : 직진 + 유턴 활성화
    QString L2710() { return QStringLiteral( "2710" ); } //직진 + 유턴 + 좌회전 : 직진 + 좌회전 활성화
    QString L2711() { return QStringLiteral( "2711" ); } //직진 + 유턴 + 좌회전 활성화
    QString L2800() { return QStringLiteral( "2800" ); } //직진 비활성화
    QString L2808() { return QStringLiteral( "2808" ); } //직진 활성화
    QString L2900() { return QStringLiteral( "2900" ); } //직진 + 유턴 비활성화
    QString L2901() { return QStringLiteral( "2901" ); } //직진 + 유턴 : 유턴 활성화
    QString L2908() { return QStringLiteral( "2908" ); } //직진 + 유턴 : 직진 활성화
    QString L2909() { return QStringLiteral( "2909" ); } //직진 + 유턴 활성화
    QString L3000() { return QStringLiteral( "3000" ); } //직진 + 좌회전 비활성화
    QString L3002() { return QStringLiteral( "3002" ); } //직진 + 좌회전 : 좌회전 활성화
    QString L3008() { return QStringLiteral( "3008" ); } //직진 + 좌회전 : 직진 활성화
    QString L3010() { return QStringLiteral( "3010" ); } //직진 + 좌회전 활성화
    QString L3100() { return QStringLiteral( "3100" ); } //직진 + 유턴 + 좌회전  비활성화
    QString L3101() { return QStringLiteral( "3101" ); } //직진 + 유턴 + 좌회전 : 유턴 활성화
    QString L3102() { return QStringLiteral( "3102" ); } //직진 + 유턴 + 좌회전 : 좌회전 활성화
    QString L3103() { return QStringLiteral( "3103" ); } //직진 + 유턴 + 좌회전 : 좌회전 + 유턴 활성화
    QString L3108() { return QStringLiteral( "3108" ); } //직진 + 유턴 + 좌회전 : 직진 활성화
    QString L3109() { return QStringLiteral( "3109" ); } //직진 + 유턴 + 좌회전 : 직진 + 유턴 활성화
    QString L3110() { return QStringLiteral( "3110" ); } //직진 + 유턴 + 좌회전 : 직진 + 좌회전 활성화
    QString L3111() { return QStringLiteral( "3111" ); } //직진 + 유턴 + 좌회전 활성화
    QString L3200() { return QStringLiteral( "3200" ); } //우회전 비활성화
    QString L3232() { return QStringLiteral( "3232" ); } //우회전 활성화
    QString L3300() { return QStringLiteral( "3300" ); } //우회전 + 유턴 비활성화
    QString L3301() { return QStringLiteral( "3301" ); } //우회전 + 유턴 : 유턴 활성화
    QString L3332() { return QStringLiteral( "3332" ); } //우회전 + 유턴 : 우회전 활성화
    QString L3333() { return QStringLiteral( "3333" ); } //우회전 + 유턴 활성화
    QString L3400() { return QStringLiteral( "3400" ); } //우회전 + 좌회전 비활성화
    QString L3402() { return QStringLiteral( "3402" ); } //우회전 + 좌회전 : 좌회전 활성화
    QString L3432() { return QStringLiteral( "3432" ); } //우회전 + 좌회전 : 우회전 활성화
    QString L3434() { return QStringLiteral( "3434" ); } //우회전 + 좌회전 활성화
    QString L3500() { return QStringLiteral( "3500" ); } //우회전 + 좌회전 비활성화
    QString L3502() { return QStringLiteral( "3502" ); } //우회전 + 좌회전 : 좌회전 활성화
    QString L3532() { return QStringLiteral( "3532" ); } //우회전 + 좌회전 : 우회전 활성화
    QString L3534() { return QStringLiteral( "3534" ); } //우회전 + 좌회전 활성화
    QString L3600() { return QStringLiteral( "3600" ); } //우회전 + 10시방향 좌회전 비활성화
    QString L3602() { return QStringLiteral( "3602" ); } //우회전 + 10시방향 좌회전 : 10시방향 좌회전 활성화
    QString L3632() { return QStringLiteral( "3632" ); } //우회전 + 10시방향 좌회전 : 우회전 활성화
    QString L3634() { return QStringLiteral( "3634" ); } //우회전 + 10시방향 좌회전 활성화
    QString L3700() { return QStringLiteral( "3700" ); } //우회전 + 직진 비활성화
    QString L3708() { return QStringLiteral( "3708" ); } //우회전 + 직진 : 직진 활성화
    QString L3732() { return QStringLiteral( "3732" ); } //우회전 + 직진 : 우회전 활성화
    QString L3740() { return QStringLiteral( "3740" ); } //우회전 + 직진 활성화
    QString L3800() { return QStringLiteral( "3800" ); } //우회전 + 직진 비활성화
    QString L3808() { return QStringLiteral( "3808" ); } //우회전 + 직진 : 직진 활성화
    QString L3832() { return QStringLiteral( "3832" ); } //우회전 + 직진 : 우회전 활성화
    QString L3840() { return QStringLiteral( "3840" ); } //우회전 + 직진 활성화
    QString L3900() { return QStringLiteral( "3900" ); } //우회전 + 10시방향 좌회전 + 좌회전 + 유턴 비활성화
    QString L3901() { return QStringLiteral( "3901" ); } //우회전 + 10시방향 좌회전 + 좌회전 + 유턴 : 유턴 활성화
    QString L3902() { return QStringLiteral( "3902" ); } //우회전 + 10시방향 좌회전 + 좌회전 + 유턴 : 좌회전 활성화
    QString L3903() { return QStringLiteral( "3903" ); } //우회전 + 10시방향 좌회전 + 좌회전 + 유턴 : 유턴 + 좌회전 활성화
    QString L3904() { return QStringLiteral( "3904" ); } //우회전 + 10시방향 좌회전 + 좌회전 + 유턴 : 10시방향 좌회전 활성화
    QString L3905() { return QStringLiteral( "3905" ); } //우회전 + 10시방향 좌회전 + 좌회전 + 유턴 : 10시방향 좌회전 + 유턴 활성화
    QString L3906() { return QStringLiteral( "3906" ); } //우회전 + 10시방향 좌회전 + 좌회전 + 유턴 : 10시방향 좌회전 + 좌회전 활성화
    QString L3907() { return QStringLiteral( "3907" ); } //우회전 + 10시방향 좌회전 + 좌회전 + 유턴 : 10시방향 좌회전 + 좌회전 + 유턴 활성화
    QString L3932() { return QStringLiteral( "3932" ); } //우회전 + 10시방향 좌회전 + 좌회전 + 유턴 : 우회전 활성화
    QString L3933() { return QStringLiteral( "3933" ); } //우회전 + 10시방향 좌회전 + 좌회전 + 유턴 : 우회전 + 유턴 활성화
    QString L3934() { return QStringLiteral( "3934" ); } //우회전 + 10시방향 좌회전 + 좌회전 + 유턴 : 우회전 + 좌회전 활성화
    QString L3935() { return QStringLiteral( "3935" ); } //우회전 + 10시방향 좌회전 + 좌회전 + 유턴 : 우회전 + 유턴 + 좌회전 활성화
    QString L3936() { return QStringLiteral( "3936" ); } //우회전 + 10시방향 좌회전 + 좌회전 + 유턴 : 우회전 + 10시방향 좌회전 활성화
    QString L3937() { return QStringLiteral( "3937" ); } //우회전 + 10시방향 좌회전 + 좌회전 + 유턴 : 우회전 + 10시방향 좌회전 + 유턴 활성화
    QString L3938() { return QStringLiteral( "3938" ); } //우회전 + 10시방향 좌회전 + 좌회전 + 유턴 : 우회전 + 10시방향 좌회전 + 좌회전 활성화
    QString L3939() { return QStringLiteral( "3939" ); } //우회전 + 10시방향 좌회전 + 좌회전 + 유턴 활성화
    QString L4000() { return QStringLiteral( "4000" ); } //우회전 + 직진 비활성화
    QString L4008() { return QStringLiteral( "4008" ); } //우회전 + 직진 : 직진 활성화
    QString L4032() { return QStringLiteral( "4032" ); } //우회전 + 직진 : 우회전 활성화
    QString L4040() { return QStringLiteral( "4040" ); } //우회전 + 직진 활성화
    QString L4100() { return QStringLiteral( "4100" ); } //우회전 + 직진 + 유턴  비활성화
    QString L4101() { return QStringLiteral( "4101" ); } //우회전 + 직진 + 유턴 : 유턴 활성화
    QString L4108() { return QStringLiteral( "4108" ); } //우회전 + 직진 + 유턴 : 직진 활성화
    QString L4109() { return QStringLiteral( "4109" ); } //우회전 + 직진 + 유턴 : 직진 + 유턴 활성화
    QString L4132() { return QStringLiteral( "4132" ); } //우회전 + 직진 + 유턴 : 우회전 활성화
    QString L4133() { return QStringLiteral( "4133" ); } //우회전 + 직진 + 유턴 : 우회전 + 유턴 활성화
    QString L4140() { return QStringLiteral( "4140" ); } //우회전 + 직진 + 유턴 : 우회전 + 직진 활성화
    QString L4141() { return QStringLiteral( "4141" ); } //우회전 + 직진 + 유턴 활성화
    QString L4200() { return QStringLiteral( "4200" ); } //우회전 + 직진 + 좌회전  비활성화
    QString L4201() { return QStringLiteral( "4201" ); } //우회전 + 직진 + 좌회전 : 좌회전 활성화
    QString L4208() { return QStringLiteral( "4208" ); } //우회전 + 직진 + 좌회전 : 직진 활성화
    QString L4209() { return QStringLiteral( "4209" ); } //우회전 + 직진 + 좌회전 : 직진 + 좌회전 활성화
    QString L4232() { return QStringLiteral( "4232" ); } //우회전 + 직진 + 좌회전 : 우회전 활성화
    QString L4233() { return QStringLiteral( "4233" ); } //우회전 + 직진 + 좌회전 : 우회전 + 좌회전 활성화
    QString L4240() { return QStringLiteral( "4240" ); } //우회전 + 직진 + 좌회전 : 우회전 + 직진 활성화
    QString L4241() { return QStringLiteral( "4241" ); } //우회전 + 직진 + 좌회전 활성화
    QString L4300() { return QStringLiteral( "4300" ); } //우회전 + 직진 + 좌회전 + 유턴  비활성화
    QString L4301() { return QStringLiteral( "4301" ); } //우회전 + 직진 + 좌회전 + 유턴 : 유턴 활성화
    QString L4302() { return QStringLiteral( "4302" ); } //우회전 + 직진 + 좌회전 + 유턴 : 좌회전 활성화
    QString L4303() { return QStringLiteral( "4303" ); } //우회전 + 직진 + 좌회전 + 유턴 : 좌회전 + 유턴 활성화
    QString L4308() { return QStringLiteral( "4308" ); } //우회전 + 직진 + 좌회전 + 유턴 : 직진 활성화
    QString L4309() { return QStringLiteral( "4309" ); } //우회전 + 직진 + 좌회전 + 유턴 : 직진 + 유턴 활성화
    QString L4310() { return QStringLiteral( "4310" ); } //우회전 + 직진 + 좌회전 + 유턴 : 직진 + 좌회전 활성화
    QString L4311() { return QStringLiteral( "4311" ); } //우회전 + 직진 + 좌회전 + 유턴 : 직진 + 좌회전 + 유턴 활성화
    QString L4332() { return QStringLiteral( "4332" ); } //우회전 + 직진 + 좌회전 + 유턴 : 우회전 활성화
    QString L4333() { return QStringLiteral( "4333" ); } //우회전 + 직진 + 좌회전 + 유턴 : 우회전 + 유턴 활성화
    QString L4334() { return QStringLiteral( "4334" ); } //우회전 + 직진 + 좌회전 + 유턴 : 우회전 + 좌회전 활성화
    QString L4335() { return QStringLiteral( "4335" ); } //우회전 + 직진 + 좌회전 + 유턴 : 우회전 + 좌회전 + 유턴 활성화
    QString L4340() { return QStringLiteral( "4340" ); } //우회전 + 직진 + 좌회전 + 유턴 : 우회전 + 직진 활성화
    QString L4341() { return QStringLiteral( "4341" ); } //우회전 + 직진 + 좌회전 + 유턴 : 우회전 + 직진 + 유턴 활성화
    QString L4342() { return QStringLiteral( "4342" ); } //우회전 + 직진 + 좌회전 + 유턴 : 우회전 + 직진 + 좌회전 활성화
    QString L4343() { return QStringLiteral( "4343" ); } //우회전 + 직진 + 좌회전 + 유턴 활성화
    QString L4400() { return QStringLiteral( "4400" ); } //우회전 + 직진 + 10시방향 좌회전  비활성화
    QString L4404() { return QStringLiteral( "4404" ); } //우회전 + 직진 + 10시방향 좌회전 : 10시방향 좌회전 활성화
    QString L4408() { return QStringLiteral( "4408" ); } //우회전 + 직진 + 10시방향 좌회전 : 직진 활성화
    QString L4412() { return QStringLiteral( "4412" ); } //우회전 + 직진 + 10시방향 좌회전 : 직진 + 10시방향 좌회전 활성화
    QString L4432() { return QStringLiteral( "4432" ); } //우회전 + 직진 + 10시방향 좌회전 : 우회전 활성화
    QString L4436() { return QStringLiteral( "4436" ); } //우회전 + 직진 + 10시방향 좌회전 : 우회전 + 10시방향 좌회전 활성화
    QString L4440() { return QStringLiteral( "4440" ); } //우회전 + 직진 + 10시방향 좌회전 : 우회전 + 직진 활성화
    QString L4444() { return QStringLiteral( "4444" ); } //우회전 + 직진 + 10시방향 좌회전 활성화
    QString L4500() { return QStringLiteral( "4500" ); } //우회전 + 직진 비활성화
    QString L4508() { return QStringLiteral( "4508" ); } //우회전 + 직진 : 직진 활성화
    QString L4532() { return QStringLiteral( "4532" ); } //우회전 + 직진 : 우회전 활성화
    QString L4540() { return QStringLiteral( "4540" ); } //우회전 + 직진 활성화
    QString L4600() { return QStringLiteral( "4600" ); } //우회전 + 직진 비활성화
    QString L4608() { return QStringLiteral( "4608" ); } //우회전 + 직진 : 직진 활성화
    QString L4632() { return QStringLiteral( "4632" ); } //우회전 + 직진 : 우회전 활성화
    QString L4640() { return QStringLiteral( "4640" ); } //우회전 + 직진 활성화
    QString L4700() { return QStringLiteral( "4700" ); } //우회전 + 직진 비활성화
    QString L4708() { return QStringLiteral( "4708" ); } //우회전 + 직진 : 직진 활성화
    QString L4732() { return QStringLiteral( "4732" ); } //우회전 + 직진 : 우회전 활성화
    QString L4740() { return QStringLiteral( "4740" ); } //우회전 + 직진 활성화
    QString L4800() { return QStringLiteral( "4800" ); } //우회전 + 2시 방향 우회전 비활성화
    QString L4816() { return QStringLiteral( "4816" ); } //우회전 + 2시 방향 우회전 : 2시 방향 우회전 활성화
    QString L4832() { return QStringLiteral( "4832" ); } //우회전 + 2시 방향 우회전 : 우회전 활성화
    QString L4848() { return QStringLiteral( "4848" ); } //우회전 + 2시 방향 우회전 활성화
    QString L4900() { return QStringLiteral( "4900" ); } //우회전 + 2시 방향 우회전 + 유턴  비활성화
    QString L4901() { return QStringLiteral( "4901" ); } //우회전 + 2시 방향 우회전 + 유턴 : 유턴 활성화
    QString L4916() { return QStringLiteral( "4916" ); } //우회전 + 2시 방향 우회전 + 유턴 : 2시 방향 우회전 활성화
    QString L4917() { return QStringLiteral( "4917" ); } //우회전 + 2시 방향 우회전 + 유턴 : 2시 방향 우회전 + 유턴 활성화
    QString L4932() { return QStringLiteral( "4932" ); } //우회전 + 2시 방향 우회전 + 유턴 : 우회전 활성화
    QString L4933() { return QStringLiteral( "4933" ); } //우회전 + 2시 방향 우회전 + 유턴 : 우회전 + 유턴 활성화
    QString L4948() { return QStringLiteral( "4948" ); } //우회전 + 2시 방향 우회전 + 유턴 : 우회전 + 2시 방향 우회전 활성화
    QString L4949() { return QStringLiteral( "4949" ); } //우회전 + 2시 방향 우회전 + 유턴 활성화
    QString L5000() { return QStringLiteral( "5000" ); } //우회전 + 2시 방향 우회전 + 좌회전  비활성화
    QString L5002() { return QStringLiteral( "5002" ); } //우회전 + 2시 방향 우회전 + 좌회전 : 좌회전 활성화
    QString L5016() { return QStringLiteral( "5016" ); } //우회전 + 2시 방향 우회전 + 좌회전 : 2시 방향 우회전 활성화
    QString L5018() { return QStringLiteral( "5018" ); } //우회전 + 2시 방향 우회전 + 좌회전 : 2시 방향 우회전 + 좌회전 활성화
    QString L5032() { return QStringLiteral( "5032" ); } //우회전 + 2시 방향 우회전 + 좌회전 : 우회전 활성화
    QString L5034() { return QStringLiteral( "5034" ); } //우회전 + 2시 방향 우회전 + 좌회전 : 우회전 + 좌회전 활성화
    QString L5048() { return QStringLiteral( "5048" ); } //우회전 + 2시 방향 우회전 + 좌회전 : 우회전 + 2시 방향 우회전 활성화
    QString L5050() { return QStringLiteral( "5050" ); } //우회전 + 2시 방향 우회전 + 좌회전 활성화
    QString L5100() { return QStringLiteral( "5100" ); } //우회전 + 직진 비활성화
    QString L5108() { return QStringLiteral( "5108" ); } //우회전 + 직진 : 직진 활성화
    QString L5132() { return QStringLiteral( "5132" ); } //우회전 + 직진 : 우회전 활성화
    QString L5140() { return QStringLiteral( "5140" ); } //우회전 + 직진 활성화
    QString L5200() { return QStringLiteral( "5200" ); } //우회전 + 2시 방향 우회전 + 10시방향 좌회전  비활성화
    QString L5204() { return QStringLiteral( "5204" ); } //우회전 + 2시 방향 우회전 + 10시방향 좌회전 : 10시방향 좌회전 활성화
    QString L5216() { return QStringLiteral( "5216" ); } //우회전 + 2시 방향 우회전 + 10시방향 좌회전 : 2시 방향 우회전 활성화
    QString L5220() { return QStringLiteral( "5220" ); } //우회전 + 2시 방향 우회전 + 10시방향 좌회전 : 2시 방향 우회전 + 10시방향 좌회전 활성화
    QString L5232() { return QStringLiteral( "5232" ); } //우회전 + 2시 방향 우회전 + 10시방향 좌회전 : 우회전 활성화
    QString L5236() { return QStringLiteral( "5236" ); } //우회전 + 2시 방향 우회전 + 10시방향 좌회전 : 우회전 + 10시방향 좌회전 활성화
    QString L5248() { return QStringLiteral( "5248" ); } //우회전 + 2시 방향 우회전 + 10시방향 좌회전 : 우회전 + 2시 방향 우회전 활성화
    QString L5252() { return QStringLiteral( "5252" ); } //우회전 + 2시 방향 우회전 + 10시방향 좌회전 활성화
    QString L5300() { return QStringLiteral( "5300" ); } //우회전 + 직진 비활성화
    QString L5308() { return QStringLiteral( "5308" ); } //우회전 + 직진 : 직진 활성화
    QString L5332() { return QStringLiteral( "5332" ); } //우회전 + 직진 : 우회전 활성화
    QString L5340() { return QStringLiteral( "5340" ); } //우회전 + 직진 활성화
    QString L5400() { return QStringLiteral( "5400" ); } //우회전 + 직진 비활성화
    QString L5408() { return QStringLiteral( "5408" ); } //우회전 + 직진 : 직진 활성화
    QString L5432() { return QStringLiteral( "5432" ); } //우회전 + 직진 : 우회전 활성화
    QString L5440() { return QStringLiteral( "5440" ); } //우회전 + 직진 활성화
    QString L5500() { return QStringLiteral( "5500" ); } //우회전 + 직진 비활성화
    QString L5508() { return QStringLiteral( "5508" ); } //우회전 + 직진 : 직진 활성화
    QString L5532() { return QStringLiteral( "5532" ); } //우회전 + 직진 : 우회전 활성화
    QString L5540() { return QStringLiteral( "5540" ); } //우회전 + 직진 활성화
    QString L5600() { return QStringLiteral( "5600" ); } //우회전 + 2시 방향 우회전 + 직진  비활성화
    QString L5608() { return QStringLiteral( "5608" ); } //우회전 + 2시 방향 우회전 + 직진 : 직진 활성화
    QString L5616() { return QStringLiteral( "5616" ); } //우회전 + 2시 방향 우회전 + 직진 : 2시 방향 우회전 활성화
    QString L5624() { return QStringLiteral( "5624" ); } //우회전 + 2시 방향 우회전 + 직진 : 2시 방향 우회전 + 직진 활성화
    QString L5632() { return QStringLiteral( "5632" ); } //우회전 + 2시 방향 우회전 + 직진 : 우회전 활성화
    QString L5640() { return QStringLiteral( "5640" ); } //우회전 + 2시 방향 우회전 + 직진 : 우회전 + 직진 활성화
    QString L5648() { return QStringLiteral( "5648" ); } //우회전 + 2시 방향 우회전 + 직진 : 우회전 + 2시 방향 우회전 활성화
    QString L5656() { return QStringLiteral( "5656" ); } //우회전 + 2시 방향 우회전 + 직진 활성화
    QString L5700() { return QStringLiteral( "5700" ); } //우회전 + 직진 비활성화
    QString L5708() { return QStringLiteral( "5708" ); } //우회전 + 직진 : 직진 활성화
    QString L5732() { return QStringLiteral( "5732" ); } //우회전 + 직진 : 우회전 활성화
    QString L5740() { return QStringLiteral( "5740" ); } //우회전 + 직진 활성화
    QString L5800() { return QStringLiteral( "5800" ); } //우회전 + 2시 방향 우회전 + 직진 + 좌회전  비활성화
    QString L5802() { return QStringLiteral( "5802" ); } //우회전 + 2시 방향 우회전 + 직진 + 좌회전 : 좌회전 활성화
    QString L5808() { return QStringLiteral( "5808" ); } //우회전 + 2시 방향 우회전 + 직진 + 좌회전 : 직진 활성화
    QString L5810() { return QStringLiteral( "5810" ); } //우회전 + 2시 방향 우회전 + 직진 + 좌회전 : 직진 + 좌회전 활성화
    QString L5816() { return QStringLiteral( "5816" ); } //우회전 + 2시 방향 우회전 + 직진 + 좌회전 : 2시 방향 우회전 활성화
    QString L5818() { return QStringLiteral( "5818" ); } //우회전 + 2시 방향 우회전 + 직진 + 좌회전 : 2시 방향 우회전 + 좌회전 활성화
    QString L5824() { return QStringLiteral( "5824" ); } //우회전 + 2시 방향 우회전 + 직진 + 좌회전 : 2시 방향 우회전 + 직진 활성화
    QString L5826() { return QStringLiteral( "5826" ); } //우회전 + 2시 방향 우회전 + 직진 + 좌회전 : 2시 방향 우회전 + 좌회전 + 직진 활성화
    QString L5832() { return QStringLiteral( "5832" ); } //우회전 + 2시 방향 우회전 + 직진 + 좌회전 : 우회전 활성화
    QString L5834() { return QStringLiteral( "5834" ); } //우회전 + 2시 방향 우회전 + 직진 + 좌회전 : 우회전 + 좌회전 활성화
    QString L5840() { return QStringLiteral( "5840" ); } //우회전 + 2시 방향 우회전 + 직진 + 좌회전 : 우회전 + 직진 활성화
    QString L5842() { return QStringLiteral( "5842" ); } //우회전 + 2시 방향 우회전 + 직진 + 좌회전 : 우회전 + 좌회전 + 직진 활성화
    QString L5848() { return QStringLiteral( "5848" ); } //우회전 + 2시 방향 우회전 + 직진 + 좌회전 : 우회전 + 2시 방향 우회전 활성화
    QString L5850() { return QStringLiteral( "5850" ); } //우회전 + 2시 방향 우회전 + 직진 + 좌회전 : 우회전 + 2시 방향 우회전 + 좌회전 활성화
    QString L5856() { return QStringLiteral( "5856" ); } //우회전 + 2시 방향 우회전 + 직진 + 좌회전 : 우회전 + 2시 방향 우회전 + 직진 활성화
    QString L5858() { return QStringLiteral( "5858" ); } //우회전 + 2시 방향 우회전 + 직진 + 좌회전 활성화
    QString L5900() { return QStringLiteral( "5900" ); } //우회전 + 직진 비활성화
    QString L5908() { return QStringLiteral( "5908" ); } //우회전 + 직진 : 직진 활성화
    QString L5932() { return QStringLiteral( "5932" ); } //우회전 + 직진 : 우회전 활성화
    QString L5940() { return QStringLiteral( "5940" ); } //우회전 + 직진 활성화
    QString L6000() { return QStringLiteral( "6000" ); } //우회전 + 직진 비활성화
    QString L6008() { return QStringLiteral( "6008" ); } //우회전 + 직진 : 직진 활성화
    QString L6032() { return QStringLiteral( "6032" ); } //우회전 + 직진 : 우회전 활성화
    QString L6040() { return QStringLiteral( "6040" ); } //우회전 + 직진 활성화
    QString L6100() { return QStringLiteral( "6100" ); } //우회전 + 직진 비활성화
    QString L6108() { return QStringLiteral( "6108" ); } //우회전 + 직진 : 직진 활성화
    QString L6132() { return QStringLiteral( "6132" ); } //우회전 + 직진 : 우회전 활성화
    QString L6140() { return QStringLiteral( "6140" ); } //우회전 + 직진 활성화
    QString L6200() { return QStringLiteral( "6200" ); } //우회전 + 직진 비활성화
    QString L6208() { return QStringLiteral( "6208" ); } //우회전 + 직진 : 직진 활성화
    QString L6232() { return QStringLiteral( "6232" ); } //우회전 + 직진 : 우회전 활성화
    QString L6240() { return QStringLiteral( "6240" ); } //우회전 + 직진 활성화
    QString L6300() { return QStringLiteral( "6300" ); } //우회전 + 직진 비활성화
    QString L6308() { return QStringLiteral( "6308" ); } //우회전 + 직진 : 직진 활성화
    QString L6332() { return QStringLiteral( "6332" ); } //우회전 + 직진 : 우회전 활성화
    QString L6340() { return QStringLiteral( "6340" ); } //우회전 + 직진 활성화
};

/** TBT 회전 코드 **/
class RgTbtCode : public QObject {
    Q_OBJECT
    Q_ENUMS( RgTbtCodeK )
public:
    enum RgTbtCodeK {
        TURN_PT_MAP = 1, //도곽에 의한 점
        TURN_PT_TILE = 2, //타일에 의한 점
        TURN_NOT_SER_EXP = 3, //고속도로에 의한 안내없음
        TURN_NOT_SER_NORMAL = 4, //일반도로에 의한 안내없음
        TURN_NOT_SER_SPEC = 5, //특수한 경우 안내없음
        STR_RIGHT = 6, //Y자 오른쪽 안내없음
        STR_LEFT = 7, //Y자 왼쪽 안내없음

        TURN_STR = 11, // 직진
        TURN_LEFT = 12, // 좌회전
        TURN_RIGHT = 13, // 우회전
        TURN_UTURN = 14, // U-TURN
        TURN_PTURN = 15, // P-TURN
        TURN_LEFT_8 = 16, // 8시 방향 좌회전
        TURN_LEFT_10 = 17, // 10시 방향 좌회전
        TURN_RIGNT_2 = 18, // 2시 방향 우회전
        TURN_RIGNT_4 = 19, // 4시 방향 우회전

        TURN_DIR_1 = 31, // 1시 방향
        TURN_DIR_2 = 32, // 2시 방향
        TURN_DIR_3 = 33, // 3시 방향
        TURN_DIR_4 = 34, // 4시 방향
        TURN_DIR_5 = 35, // 5시 방향
        TURN_DIR_6 = 36, // 6시 방향
        TURN_DIR_7 = 37, // 7시 방향
        TURN_DIR_8 = 38, // 8시 방향
        TURN_DIR_9 = 39, // 9시 방향
        TURN_DIR_10 = 40, // 10시 방향
        TURN_DIR_11 = 41, // 11시 방향
        TURN_DIR_12 = 42, // 12시 방향
        TURN_STR_RIGHT = 43, // 오른쪽
        TURN_STR_LEFT = 44, // 왼쪽

        LANE_STR = 51, // 직진방향
        LANE_LEFT = 52, // 왼쪽차선
        LANE_RIGHT = 53, // 오른쪽차선
        LANE_1 = 54, // 1차선
        LANE_2 = 55, // 2차선
        LANE_3 = 56, // 3차선
        LANE_4 = 57, // 4차선
        LANE_5 = 58, // 5차선
        LANE_6 = 59, // 6차선
        LANE_7 = 60, // 7차선
        LANE_8 = 61, // 8차선
        LANE_9 = 62, // 9차선
        LANE_10 = 63, // 10차선

        FIRST_EXIT = 71, // 첫번째 출구
        SEC_EXIT = 72, // 두번째 출구
        FIRST_LANE_RIGHT = 73, // 첫번째 오른쪽 길
        SEC_LANE_RIGHT = 74, // 두번째 오른쪽 길
        FIRST_LANE_LEFT = 75, // 첫번째 왼쪽 길
        SEC_LANE_LEFT = 76, // 두번째 왼쪽 길

        HIGHPATH_LEFT = 80, // 하이패스차로는 좌측에 있습니다.
        HIGHPATH_RIGHT = 81, // 하이패스차로는 우측에 있습니다.
        HIGHPATH_CENTER = 82, // 하이패스차로는 중앙에 있습니다.

        EXP_IN_RIGHT = 101, // 오른쪽방향에 고속도로 입구
        EXP_IN_LEFT = 102, // 왼쪽방향에 고속도로 입구
        EXP_IN_STR = 103, // 직진방향에 고속도로 입구
        EXP_OUT_RIGHT = 104, // 오른쪽방향에 고속도로 출구
        EXP_OUT_LEFT = 105, // 왼쪽방향에 고속도로 출구
        EXP_OUT_STR = 106, // 직진방향에 고속도로 출구

        SAEXP_IN_RIGHT = 111, // 오른쪽방향에 도시고속도로 입구
        SAEXP_IN_LEFT = 112, // 왼쪽방향에 도시고속도로 입구
        SAEXP_IN_STR = 113, // 직진방향에 도시고속도로 입구
        SAEXP_OUT_RIGHT = 114, // 오른쪽방향에 도시고속도로 출구
        SAEXP_OUT_LEFT = 115, // 왼쪽방향에 도시고속도로 출구
        SAEXP_OUT_STR = 116, // 직진방향에 도시고속도로 출구
        ROAD_RIGHT = 117, // 오른쪽 도로
        ROAD_LEFT = 118, // 왼쪽 도로
        UNDER_IN = 119, // 지하도로 진입
        OVER_IN = 120, // 고가도로 진입
        TUNNEL_IN = 121, // 터널
        BRIDGE = 122, //교량
        UNDER_OUT = 123, // 지하도로 옆
        OVER_OUT = 124, // 고가도로 옆

        DIR_1 = 131, //로터리 1시방향
        DIR_2 = 132, //로터리 2시방향
        DIR_3 = 133, //로터리 3시방향
        DIR_4 = 134, //로터리 4시방향
        DIR_5 = 135, //로터리 5시방향
        DIR_6 = 136, //로터리 6시방향
        DIR_7 = 137, //로터리 7시방향
        DIR_8 = 138, //로터리 8시방향
        DIR_9 = 139, //로터리 9시방향
        DIR_10 = 140, //로터리 10시방향
        DIR_11 = 141, //로터리 11시방향
        DIR_12 = 142, //로터리 12시방향

        SA = 151, // 휴게소
        SA_IN = 152, // 휴게소 진입
        TOL_EXP = 153, //톨게이트 고속
        TOL_NORMAL = 154, //톨게이트 일반
        FERRY_IN = 155, // 페리항로 진입
        FERRY_OUT = 156,  // 페리항로 진출

        STARTDIR_1 = 161, // 경로시작점안내 1시방향
        STARTDIR_2 = 162, // 경로시작점안내 2시방향
        STARTDIR_3 = 163, // 경로시작점안내 3시방향
        STARTDIR_4 = 164, // 경로시작점안내 4시방향
        STARTDIR_5 = 165, // 경로시작점안내 5시방향
        STARTDIR_6 = 166, // 경로시작점안내 6시방향
        STARTDIR_7 = 167, // 경로시작점안내 7시방향
        STARTDIR_8 = 168, // 경로시작점안내 8시방향
        STARTDIR_9 = 169, // 경로시작점안내 9시방향
        STARTDIR_10 = 170, // 경로시작점안내 10시방향
        STARTDIR_11 = 171, // 경로시작점안내 11시방향
        STARTDIR_12 = 172, // 경로시작점안내 12시방향

        DEST_STR = 181, // 목적지 직진방향
        DEST_LEFT = 182, // 목적지 왼쪽방향
        DEST_RIGHT = 183, // 목적지 오른쪽방향
        POS_RIGHT = 184, // 경유지
        VIA_1 = 185, // 첫번째 경유지
        VIA_2 = 186, // 두번째 경유지
        VIA_3 = 187, // 세번째 경유지
        VIA_4 = 188, // 네번째 경유지
        VIA_5 = 189, // 다섯번째 경유지

        NOTICE_SPEED = 191, // 제한속도
        NOTICE_ACCIDENT = 192, // 사고다발
        NOTICE_TURN = 193, // 급커브
        NOTICE_ROCK = 194, // 낙석주의지역
        NOTICE_POL = 195, // 경찰청 고속도로 차로 통행준순 안내
        NOTICE_POL2 = 196, // 경찰청 고속도로 차로 통행준순 안내

        START = 200, // 출발지
        GOAL = 201, // 목적지

        GO_STRAIGHT = 202, // 다음 안내시 까지 직진입니다.
        IN_ROUTE = 210, // 경로 진입 안내음.
        IN_EXP = 220, // 고속도로에 있습니까?
        IN_SAEXP = 221, // 도시고속도로에 있습니까?
        IN_SKY = 222, // 고가도로에 있습니까?
        IN_UNDER = 223, // 지하도로에 있습니까?

        RDIR_1 = 231, // 1시방향
        RDIR_2 = 232, // 2시방향
        RDIR_3 = 233, // 3시방향
        RDIR_4 = 234, // 4시방향
        RDIR_5 = 235, // 5시방향
        RDIR_6 = 236, // 6시방향
        RDIR_7 = 237, // 7시방향
        RDIR_8 = 238, // 8시방향
        RDIR_9 = 239, // 9시방향
        RDIR_10 = 240, // 10시방향
        RDIR_11 = 241, // 11시방향
        RDIR_12 = 242, // 12시방향

        TEMP = 248, // 톨게이트입니다.
        TOLLGATE = 249, // 톨게이트입니다.
        END = 250, // 경로안내를 종료합니다.
    };    
};

/** 모의주행 옵션 코드 **/
class RgSimulationCode: public QObject {
    Q_OBJECT
    Q_ENUMS( RgSimulationCodeK )
public:
    enum RgSimulationCodeK {
        START = 0,  //모의주행 최초 시작 (초기화 한다.)
        FIRST = 1,  //모의주행 시작 위치로 이동
        PREV_GP = 2, //모의주행 이전 GP로 이동
        NEXT_STEP = 3, //모의주행 다음 지점으로 이동(속도값에 맞게 이동)
        NEXT_GP = 4, //모의주행 다음 GP로 이동
        POS = 5,  //모의주행 특정 위치로 이동
    };    
};

    /** 고속도로 혼잡도 **/
class RgHighwayCongestionCode : public QObject {
    Q_OBJECT
public:
    Q_ENUMS( RgHighwayCongestionCodeK )
    enum RgHighwayCongestionCodeK {
        NONE  = 0,
        GOOD  = 1,
        DELAY  = 2,
        TIEUP  = 3
    };    
};

/** 경로요약 길 혼잡도 **/
class RgRouteSummaryCongestionCode : public QObject {
    Q_OBJECT
    Q_DISABLE_COPY(RgRouteSummaryCongestionCode)
    Q_PROPERTY( QString NONE READ NONE )
    Q_PROPERTY( QString GOOD READ GOOD )
    Q_PROPERTY( QString DELAY READ DELAY )
    Q_PROPERTY( QString TIEUP READ TIEUP )
public:
    RgRouteSummaryCongestionCode() {
    }

    static QObject *qmlInstance(QQmlEngine *engine, QJSEngine *scriptEngine) {
        Q_UNUSED(engine);
        Q_UNUSED(scriptEngine);

        return new RgRouteSummaryCongestionCode;
    }

    QString NONE() { return QStringLiteral( "0" ); }
    QString GOOD() { return QStringLiteral( "1" ); }
    QString DELAY() { return QStringLiteral( "2" ); }
    QString TIEUP() { return QStringLiteral( "4" ); }
};

/** 시설물 종류 **/
class RgFacilityCode : public QObject {
    Q_OBJECT
public:
    Q_ENUMS( RgFacilityCodeK )
    enum RgFacilityCodeK {
        OIL  = 0x01,  //주유소
        GAS  = 0x02,  //충전소
        GARAGE = 0x04,  //정비소
    };    
};

/** 주유소 브랜드 (RG data)**/
class RgFuelBrandCode : public QObject {
    Q_OBJECT
public:
    Q_ENUMS( RgFuelBrandCodeK )
    enum RgFuelBrandCodeK {
        OIL_NONE = 0,
        OIL_SK = 1,   // SK 주유소
        OIL_GS = 2,   // GS칼텍스 주유소
        OIL_HYUNDAE = 3, // 현대오일 주유소
        OIL_SOIL = 4,  // S-Oil 주유소
        OIL_TIGER = 5,  // TigerOil 주유소
        OIL_INCHEON = 6, // 인천정유 주유소
        OIL_NC = 7,   // NC-Oil 주유소
        OIL_NH = 8,   // NH-Oil 주유소
        OIL_SEVEND = 9,  // SevenD-Oil 주유소
        OIL_ECONOMIC = 10, // 알뜰 주유소
        OIL_ETC = 11,  // 기타 주유소
        GAS_SK = 12,  // SK 충전소
        GAS_GS = 13,  // GS칼텍스 충전소
        GAS_HYUNDAE = 14, // 현대오일 충전소
        GAS_SOIL = 15,  // S-Oil 충전소
        GAS_E1 = 16,  // E1 충전소
        GAS_INCHEON = 17, // 인천정유 주유소
        GAS_NC = 18,  // NC-Oil 충전소
        GAS_NH = 19,  // NH-Oil 충전소
        GAS_SEVEND = 20, // SevenD-Oil 충전소
        GAS_ETC = 21,  // 기타 충전소
    };    
};

/** 주유소 브랜드 (Tvas데이터)**/
class RgFuelBrandCodeTvas : public QObject {
    Q_OBJECT
public:
    Q_ENUMS( RgFuelBrandCodeTvasK )
    enum RgFuelBrandCodeTvasK {
        OIL_NONE = 0,   // 없음
        OIL_SK = 1,   // SK 주유소
        OIL_GS = 2,   // GS칼텍스 주유소
        OIL_HYUNDAE = 3,  // 현대오일 주유소
        OIL_SOIL = 4,   // S-Oil 주유소
        OIL_TIGER = 5,  // TigerOil 주유소
        OIL_MOOPOLE = 6,  // 무폴
        GAS_SK = 7,   // SK-GAS
        GAS_GS = 8,   // GS-GAS
        GAS_HYUNDAE = 9,  // H-GAS
        GAS_SOIL = 10,  // S-OIL GAS
        GAS_MOOPOLE = 11,  // LPG 무폴
        UNCONFIRMED = 12,  // 미확인
        OIL_E1 = 13,   // E1
        OIL_INCHEON = 14,  // SK인천정유
        GAS_INCHEON = 15,  // SK인천정유GAS
        OIL_NC = 16,   // NC-OIL
        GAS_NC = 17,   // NC-OIL GAS
        OIL_NH = 18,   // NH-OIL
        GAS_NH = 19,   // NH-OIL GAS
        OIL_SEVEND = 20,  // SevenD-OIL
        GAS_SEVEND = 21,  // SevenD-OIL GAS
        OIL_ALTHEL = 22  // 알뜰주유소
    };
};

/** GPS 관련 **/
class RgGps : public QObject {
};

/** GPS 타입 **/
class RgType : public QObject {
    Q_OBJECT
    Q_ENUMS( RgTypeK )
public:
    enum RgTypeK {
        FILE  = 0,
        LOCATION = 1,
        NMEA  = 2,
        MLGPS  = 3,
        MLGPSFILE = 4,
        MYSPINGPS = 5,
        SWITCHABLE = 6,
        SWITCHABLE_FILE = 7
    };    
};

/** GPS 입력 결과값 **/
class RgResult : public QObject {
    Q_OBJECT
    Q_ENUMS( RgResultK );
public:
    enum RgResultK {
        NEEDLESS   = 0,
        NOT_VALID   = 1,
        VALID    = 2,
    };
};

/** GPS 상태값 **/
class RgSignal : public QObject {
    Q_OBJECT
    Q_ENUMS( RgSignalK )
public:
    enum RgSignalK {
        NONE  = 0,  // GPS 신호 없음
        BAD   = 1,  // GPS 신호 상태 나쁨
        GOOD  = 2,  // GPS 신호 상태 좋음
        TUNNEL  = 3,  // 터널 주행 상태
        UNDERPASS = 4  // 지하차도 주행 상태
    };    
};

/* 17.9.21 hskim rg data가 null인 경우에도 신호 상태를 처리하기위해 추가 */
class RgSignalState : public QObject {
    Q_OBJECT
    Q_ENUMS( RgSignalStateK )
public:
    enum RgSignalStateK {
        NONE  = 0,  // GPS 신호 없음
        ON   = 1,  // GPS 신호 상태 좋음
        OFF  = 2,  // GPS 신호 상태 나쁨
        BAD  = 3
    };
};

/** 음성 출력 시나리오 **/
class RgVoiceScenarioIndex : public QObject {
    Q_OBJECT
    Q_ENUMS( RgVoiceScenarioIndexK )
public:
    enum RgVoiceScenarioIndexK {
        EMPTY = 0,
        NEXTGP = 1,    // 다음 GP 안내
        REROUTEOUTROUTE = 2,  // 경로 이탈 & 사용자 재탐색 안내 (IDG_REROUTE)
        REROUTETPEG = 3,  // 교통정보 주기적 재탐색 안내 (IDG_TRAFFIC_REROUTE)
        RGSTART = 4,   // 경로안내를 시작 안내 (IDG_RG_START)
        RGEND = 5,    // 경로안내를 종료 안내 (IDG_RG_END)
        RGSTARTTMAPNAVI = 6, // TmapNavi 경로안내를 시작 안내(IDG_RG_TMAP_START)
        ROUTECANCEL = 7,  // 경로취소 안내 (IDG_CANCEL_ROUTE)
        REVERSEDRIVE = 8,  // 일반 통행 역주행시 경고 (IDG_LANE_OPPOSITE)
        APPSTART = 9,   // 어플리케이션 시작 (IDW_HELLO, IDW_NOTI_DRIVING, IDW_NOTI_SEATBELT)
        GPSCLOCK = 10,   // 정각 알림(IDW_00h+시간) : pParam=[Int32]시간
        NOTISLEEPINESS = 11, // 졸음운전 알림(IDW_NOTI_SLEEPINESS)
        OVERSPEED = 12,   // 급가속 안내(IDG_OVER_SPEED)
        NOTILONGIDLING = 13, // 급가속 안내(IDW_NOTI_LONGIDLING)
        NOTINEWSAFEDATA = 14, // 새로운 SDI(TPEG) 업데이트 안내 (IDW_NOTI_NEW_SAFEDATA, IDG_NEXT_RUN_APPLY)
        HOLIDAY = 15,    // 공휴일 안내 () : pParam=[eHoliday_Type]공휴일 종류
        ROUTEROADSUMMARY = 16,  // 경로 요약 경유도로 정보 안내
        NOTARROUNDTRAFFIC = 17, // 주변 교통정보 미제공 구간입니다.(IDW_NOT_SERVICE_AROUND_TRAFFIC)

        TMAP_STARTING = 18,
        NOT_ENTER_REQUEST_ROUTE = 19,  // 경로에 진입하지 못했습니다 경로를 다시 요청합니다.
        NOT_ENTER_QUESTION_ROUTE = 20,  // 경로에 진입하지 못했습니다 다시 요청하겠습니까?
        LEAVE_REQUSET_ROUTE = 21,   // 경로를 벗어났습니다 경로를 다시 요청합니다 // 경로 이탈 & 사용자 재탐색 안내 (IDG_REROUTE)
        LEAVE_QUESTION_ROUTE = 22,   // 경로를 벗어났습니다 경로를 다시 요청하시겠습니까? //수동 이탈 재탐색
        NOT_SAVE_CONTINUE_DATA = 23,  // 저장된 경로가 없습니다
        CONTINUE_ROUTE = 24,    // 저장된 경로를 안내하겠습니다
        DETAIL_QUESTION_IS_OVERPASS = 25, // 현재 고가도로에 있습니까?
        DETAIL_QUESTION_IS_EXPRESSWAY = 26, // 현재 도시고속도로에 있습니까?
        DETAIL_QUESTION_IS_HIGHWAY = 27, // 현재 고속도로에 있습니까?
        DETAIL_QUESTION_IS_UNDERPASS = 28, // 현재 지하도로에 있습니까?
        USEROVERSPEED = 29,     // 제한 속도를 준수하십시오.

        LEAVE_REQUSET_ROUTE_FIRST = 30  // 초기 미수신에서 수신된 후 이탈 재탐색 발생시 효과음
    };    
};

/** 고속도로모드 주유소/충전소 가격 인덱스**/
class RgHighwayFuelPriceIndex: public QObject {
    Q_OBJECT
    Q_ENUMS( RgHighwayFuelPriceIndexK )
public:
    enum RgHighwayFuelPriceIndexK {
        GASOLINE = 0,           //휘발유값
        DIESEL = 1,             //경유값
        KEROSENE = 2,           //등유값
        LPG = 3,      //LPG값
        HIGH_GRADE_GASOLINE = 4, //고급휘발류값
        HIGH_GRADE_DIESEL = 5,  //고급경유값
        HIGH_GRADE_KEROSENE = 6 //고급등유값
    };    
};

}

#endif // RGPROTOCOL_H
