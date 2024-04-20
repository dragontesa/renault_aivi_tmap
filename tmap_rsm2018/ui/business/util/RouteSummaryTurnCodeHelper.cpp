#include "RouteSummaryTurnCodeHelper.h"
#include "RouteSummaryHelper.h"

namespace SKT
{

enum TbtCode {
    TURN_PT_MAP = 1,	//도곽에 의한 점
    TURN_PT_TILE = 2,	//타일에 의한 점
    TURN_NOT_SER_EXP = 3,	//고속도로에 의한 안내없음
    TURN_NOT_SER_NORMAL = 4,	//일반도로에 의한 안내없음
    TURN_NOT_SER_SPEC = 5,	//특수한 경우 안내없음
    STR_RIGHT = 6,	//Y자 오른쪽 안내없음
    STR_LEFT = 7,	//Y자 왼쪽 안내없음

    TURN_STR = 11,	//	직진
    TURN_LEFT = 12,	//	좌회전
    TURN_RIGHT = 13,	//	우회전
    TURN_UTURN = 14,	//	U-TURN
    TURN_PTURN = 15,	//	P-TURN
    TURN_LEFT_8 = 16,	//	8시 방향 좌회전
    TURN_LEFT_10 = 17,	//	10시 방향 좌회전
    TURN_RIGNT_2 = 18,	//	2시 방향 우회전
    TURN_RIGNT_4 = 19,	//	4시 방향 우회전

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

    EXP_IN_RIGHT = 101,	//	오른쪽방향에 고속도로 입구
    EXP_IN_LEFT = 102,	//	왼쪽방향에 고속도로 입구
    EXP_IN_STR = 103,	//	직진방향에 고속도로 입구
    EXP_OUT_RIGHT = 104,	//	오른쪽방향에 고속도로 출구
    EXP_OUT_LEFT = 105,	//	왼쪽방향에 고속도로 출구
    EXP_OUT_STR = 106,	//	직진방향에 고속도로 출구

    SAEXP_IN_RIGHT = 111,	//	오른쪽방향에 도시고속도로 입구
    SAEXP_IN_LEFT = 112,	//	왼쪽방향에 도시고속도로 입구
    SAEXP_IN_STR = 113,	//	직진방향에 도시고속도로 입구
    SAEXP_OUT_RIGHT = 114,	//	오른쪽방향에 도시고속도로 출구
    SAEXP_OUT_LEFT = 115,	//	왼쪽방향에 도시고속도로 출구
    SAEXP_OUT_STR = 116,	//	직진방향에 도시고속도로 출구
    ROAD_RIGHT = 117,	//	오른쪽 도로
    ROAD_LEFT = 118,	//	왼쪽 도로
    UNDER_IN = 119,	//	지하도로 진입
    OVER_IN = 120,	//	고가도로 진입
    TUNNEL_IN = 121,	//	터널
    BRIDGE = 122,	//교량
    UNDER_OUT = 123,	//	지하도로 옆
    OVER_OUT = 124,	//	고가도로 옆

    DIR_1 = 131,	//로터리 1시방향
    DIR_2 = 132,	//로터리 2시방향
    DIR_3 = 133,	//로터리 3시방향
    DIR_4 = 134,	//로터리 4시방향
    DIR_5 = 135,	//로터리 5시방향
    DIR_6 = 136,	//로터리 6시방향
    DIR_7 = 137,	//로터리 7시방향
    DIR_8 = 138,	//로터리 8시방향
    DIR_9 = 139,	//로터리 9시방향
    DIR_10 = 140,	//로터리 10시방향
    DIR_11 = 141,	//로터리 11시방향
    DIR_12 = 142,	//로터리 12시방향

    SA = 151,	//	휴게소
    SA_IN = 152, // 휴게소 진입
    TOL_EXP = 153, //톨게이트 고속
    TOL_NORMAL = 154, //톨게이트 일반
    FERRY_IN = 155,	// 페리항로 진입
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

    START = 200,	//	출발지
    GOAL = 201,	//	목적지

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

int RouteSummaryTurnCodeHelper::fromRsdTurnCode(int rsdTurnCode)
{
    switch (rsdTurnCode) {
    case TbtCode::STR_RIGHT: return 10;
    case TbtCode::STR_LEFT: return 9;

    case TbtCode::TURN_STR: return 1;
    case TbtCode::TURN_STR_LEFT: return 9;
    case TbtCode::TURN_STR_RIGHT: return 10;
    case TbtCode::TURN_LEFT: return 2;
    case TbtCode::TURN_RIGHT: return 3;
    case TbtCode::TURN_UTURN: return 6;
    case TbtCode::TURN_PTURN: return 11;
    case TbtCode::TURN_LEFT_8: return 7;
    case TbtCode::TURN_LEFT_10: return 5;
    case TbtCode::TURN_RIGNT_2: return 4;
    case TbtCode::TURN_RIGNT_4: return 8;

    case TbtCode::LANE_STR: return 1;
    case TbtCode::LANE_LEFT: return 1;
    case TbtCode::LANE_RIGHT: return 1;
    case TbtCode::LANE_1: return 1;
    case TbtCode::LANE_2: return 1;

    case TbtCode::FIRST_LANE_RIGHT: return 10;
    case TbtCode::SEC_LANE_RIGHT: return 10;
    case TbtCode::FIRST_LANE_LEFT: return 9;
    case TbtCode::SEC_LANE_LEFT: return 9;

    case TbtCode::EXP_IN_RIGHT: return 16;
    case TbtCode::EXP_IN_LEFT: return 14;
    case TbtCode::EXP_IN_STR: return 12;
    case TbtCode::EXP_OUT_RIGHT: return 17;
    case TbtCode::EXP_OUT_LEFT: return 15;
    case TbtCode::EXP_OUT_STR: return 13;

    case TbtCode::SAEXP_IN_RIGHT: return 16;
    case TbtCode::SAEXP_IN_LEFT: return 14;
    case TbtCode::SAEXP_IN_STR: return 12;
    case TbtCode::SAEXP_OUT_RIGHT: return 17;
    case TbtCode::SAEXP_OUT_LEFT: return 15;
    case TbtCode::SAEXP_OUT_STR: return 13;
    case TbtCode::ROAD_RIGHT: return 10;
    case TbtCode::ROAD_LEFT: return 9;
    case TbtCode::UNDER_IN: return 36;
    case TbtCode::OVER_IN: return 33;
    case TbtCode::TUNNEL_IN: return 29;
    case TbtCode::UNDER_OUT: return 34;
    case TbtCode::OVER_OUT: return 31;

    case TbtCode::DIR_1: return 40;
    case TbtCode::DIR_2: return 40;
    case TbtCode::DIR_3: return 37;
    case TbtCode::DIR_4: return 42;
    case TbtCode::DIR_5: return 42;
    case TbtCode::DIR_6: return 43;
    case TbtCode::DIR_7: return 46;
    case TbtCode::DIR_8: return 46;
    case TbtCode::DIR_9: return 44;
    case TbtCode::DIR_10: return 45;
    case TbtCode::DIR_11: return 45;
    case TbtCode::DIR_12: return 47;

    case TbtCode::SA: return 22;
    case TbtCode::SA_IN: return 22;
    case TbtCode::TOL_EXP: return 28;
    case TbtCode::TOL_NORMAL: return 28;

    case TbtCode::NOTICE_POL: return 16;
    case TbtCode::NOTICE_POL2: return 16;

    case TbtCode::TEMP: return 1;
    case TbtCode::TOLLGATE: return 28;
    //경로상세
    case TbtCode::START: return ROUTE_SUMMARY_POI_DEPARTURE;//24;
    case TbtCode::VIA_1: return ROUTE_SUMMARY_POI_VIA1;
    case TbtCode::VIA_2: return ROUTE_SUMMARY_POI_VIA2;
    case TbtCode::VIA_3: return ROUTE_SUMMARY_POI_VIA3;
    case TbtCode::GOAL: return ROUTE_SUMMARY_POI_ARRIVAL;//23;
    case TbtCode::TURN_NOT_SER_EXP: return 1;
    case TbtCode::TURN_NOT_SER_NORMAL: return 1;
    case TbtCode::TURN_NOT_SER_SPEC: return 1;
    };
}

bool RouteSummaryTurnCodeHelper::isSummaryCode(int rsdTurnCode)
{
    switch (rsdTurnCode) {
    case TbtCode::START:
    case TbtCode::VIA_1:
    case TbtCode::VIA_2:
    case TbtCode::VIA_3:
    case TbtCode::GOAL:
    case TbtCode::TURN_NOT_SER_EXP:
    case TbtCode::TURN_NOT_SER_NORMAL:
    case TbtCode::TURN_NOT_SER_SPEC:
        return true;

    default:
        return false;
    }
}

bool RouteSummaryTurnCodeHelper::isNoRg(int rsd)
{
    switch (rsd) {
    case TbtCode::TURN_NOT_SER_EXP:
    case TbtCode::TURN_NOT_SER_NORMAL:
    case TbtCode::TURN_NOT_SER_SPEC:
        return true;

    default:
        return false;
    }
}

}//SKT
