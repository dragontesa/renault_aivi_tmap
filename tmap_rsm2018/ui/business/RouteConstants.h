#ifndef ROUTECONSTANTS_H
#define ROUTECONSTANTS_H

#include "Constants.h"

namespace SKT {

class RouteConstants {
public:
    enum NotificationK {
        UpdateSummary = Constants::Route,
        UpdateRouteSummary,
        UpdateRouteSummaryDetail,
        UpdateRouteSummarySimple,
        UpdateRouteSummaryByReroute,
        RequestRouteSummary,
        RequestRouteSummaryDetail,
        RequestCommandResponsed,
        RerouteCommandRequested,
        RerouteCommandResponsed,
        SelectRoute,
        ChangeRoute,
        RequestRoute,
        RequestRouteWithOtherOptions,
        RequestRouteAsDestination,
        ResponseRouteAsDestination,
        RequestRouteByProfileChange,
        UpdateRoute,
        CancelRoute,
        RestoreRoute,
        RestoreTvas,
        SetTvas,
        BlockSetTvas,
        RestoreViaData,
        ResumeRoute,
        BackupViaData,
        RequestReroute,
        RequestSkipViaReroute,
        UpdateRouteCandidate,
        DoReroute,
        AddVia,
        InsertVia,
        DeleteVia,
        AddViaMarker,
        SelectViaSlot,
        SelectRouteTypes,
        SetDeparture,
        SetArrival,
        SetVia,
        SetStartCode,
        SetExploreCode,
        ClearVia,
        CheckViaValidate,
        CheckViaValidateIsOk,
        CheckViaValidateIsFail,
        RequestRouteUsingViaAsArrival,
        ShowErrorDialog,
        RequestViaMenuText,
        ReplyToViaMenuText,
        UpdateRequestRouteDebugText,

        ClearRouteDirectory,
        SetErrorCode,

        MoveRouteManagerItem,
        RemoveRouteManagerItem,
        RequestViaInfo,
        UpdateViaInfo,

        ShowViaMarker,
        ClearViaMarker,

        AddSearchMarker,
        RemoveSearchMarker,
        HitSearchMarker,
        PrepareRouteCandidate,
        ResetRouteCandidateOtherPass,

        SaveVia,

        ResetRouteCandidateView,
        CloseRouteCandidateView,
        CloseRouteSummaryView,
        CloseRouteSettingView,
        ErrorViewClosed,
        RequestRouteFailed,

        RequestNDDS,
        UpdateView,

        ChangeReRouteIntervalTime,
        Hitting,
        RelayHitting,

        SetHipass,
        PrimaryNddsServerUpdated,
    };

    /** 경로 탐색 시 출발지 탐색 구분 코드 **/
    enum StartSearchCode {
        /**GPS**/
        EXPLORER_GPS_START = 0x0,
        /** 최근 길안내(최근목적지)**/
        EXPLORER_RECENT_START = 0x4,
        /** 즐겨찾기**/
        EXPLORER_BOOKMARK_START = 0x5,
        /** 경위도 검색**/
        EXPLORER_MAP_START = 0x6,
        /** 명칭검색**/
        EXPLORER_NAME_START = 0x7,
        /**주소검색 **/
        EXPLORER_ADDRESS_START = 0x8
    };

    /** 경로 탐색 시 목적지 탐색 구분 코드 **/
    enum DestSearchCode {
        /**초기 탐색**/
        EXPLORER_FIRST_DES = 0xFF,
        /**이탈재탐색**/
        EXPLORER_RE_DES = 0x01,
        /**사용자재탐색**/
        EXPLORER_RE_USER_DES = 0x02,
        /** 주변 시설물찾기**/
        EXPLORER_ROUNDINFO_DES = 0x03,
        /** 최근 길안내(최근목적지)**/
        EXPLORER_RECENT_DES = 0x04,
        /** 즐겨찾기**/
        EXPLORER_BOOKMARK_DES = 0x05,
        /** 경위도검색**/
        EXPLORER_LONGITUDE_N_LATITUDE_DES = 0x06,
        /** 명칭검색**/
        EXPLORER_NAME_DES = 0x07,
        /** 주소검색**/
        EXPLORER_ADDRESS_DES = 0x08,
        /** 레저&라이프**/
        EXPLORER_LEISURE_N_LIFE_DES	= 0x09,
        /** 예약된길안내**/
        EXPLORER_BOOKED_ROAD_DES = 0x0A,
        /** 지역업종검색**/
        EXPLORER_LOCAL_BUSINESS_TYPE_DES = 0x0B,
        /** 전화번호검색**/
        EXPLORER_TELEPHONE_NUM_DES = 0x0C,
        /** 지역명칭검색**/
        EXPLORER_LOCAL_NAME_DES	= 0x0D,
        /** 음성길안내**/
        EXPLORER_VOICE_ROAD_GUIDE_DES = 0x0E,
        /** 지도이동 후 요청(EMP사용) - 최초 화면에서 지도 이동 후 길안내를 요청했을 경우에만 해당함 **/
        EXPLORER_AFTER_MOVING_MAP_DES = 0x0F,
        /** 내위치보기**/
        EXPLORER_MY_POSTION_DES	= 0x10,
        /** 수신위치보기**/
        EXPLORER_RECEIVE_POSTION_DES = 0x11,
        /** 발신위치보기**/
        EXPLORER_SEND_POSTION_DES = 0x12,
        /** 대안경로**/
        EXPLORER_OTHER_DES = 0x13,
        /** 대안경로이탈재탐색**/
        EXPLORER_REOTHER_DES = 0x15,
        /** 대안경로 사용자재탐색**/
        EXPLORER_REOTHER_USER_DES = 0x16,
        /** 새주소(현재 사용안함)**/
        EXPLORER_NEW_ADDRESS_DES = 0x17,
        /** 모텔/맛집**/
        EXPLORER_MOTEL_N_RESTURANT_DES = 0x18,
        /** 정주기 재탐색 - 차량이 안내점 구간 외부에 있음 **/
        EXPLORER_REROUTE_PERIOD_OUTSIDE_DES	= 0x19,
        /** 정주기 재탐색 – 차량이 안내점 구간 내부에 있음 **/
        EXPLORER_REROUTE_PERIOD_INSIDE_DES = 0x1A,
        /** 통합검색**/
        EXPLORER_TSEARCH_DES = 0x1B,
        /** 비서 개인화**/
        EXPLORER_MPOST_DES = 0x1C,

        /** 탐색 옵션 변경**/
        EXPLORER_CHANGE_ROUTE_OPTION_DES = 0x1D,
        /** 주변 링크 재탐색**/
        EXPLORER_NEAR_LINK_RE_DES = 0x1E,
        /** 경로 즐겨찾기**/
        EXPLORER_ROUTE_BOOKMARK_DES = 0x1F,
        /** 집으로 **/
        EXPLORER_HOME_DES = 0x20,
        /** 회사로 **/
        EXPLORER_OFFICE_DES = 0x21,
        /** 자주가는 경로로 **/
        EXPLORER_ROUTE_FREQUENT_DES = 0x22,

        /** 강제 재탐색(DRG 포인트)**/
        EXPLORER_FORCED_RE_SEARCH_DES	= 0x23,
    };
};

}

#endif
