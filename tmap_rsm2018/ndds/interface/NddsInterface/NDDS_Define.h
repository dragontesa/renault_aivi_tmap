#ifndef __NDDS_DEFINE_H__
#define __NDDS_DEFINE_H__

#include <stdio.h>

#ifdef _WIN32
#	ifdef NDDS_EXPORTS
#		define NDDS_API __declspec(dllexport)
#	else
#		define NDDS_API
#	endif
#else
#	ifdef NDDS_EXPORTS
#		define NDDS_API	__attribute__ ((visibility("default")))
#	else
#		define NDDS_API
#	endif
#endif

#pragma pack(4)
//////////////////////////////////////////////////////////////////////////
/// 공통
//////////////////////////////////////////////////////////////////////////

#define D_POSNAME_LEN			100		///< 경로 지점 명칭 최대 크기
#define D_POS_CNT				5		///< 경로 지점 최대 개수
#define D_MPP_CNT				10		///< MPP 최대 개수
#define D_MAX_PATH				256		///< Max Path 크기
#define D_MAX_GPS_TRACE		1024		///< Max GPS궤적 크기
#define D_ADD_CAMERATYPE		9		///< 단속 카메라 추가 개수

/// NDDS 콜백 함수 정의
/**
	@param nMsg		NDDS 콜백 메세지 종류(DeMsgType)
	@param lParam1	DeMsgType 종류별 설명 참조
	@param lParam2	DeMsgType 종류별 설명 참조
	@param lpParam	DeMsgType 종류별 설명 참조
	@return
**/
typedef int (*LPCALL_NDDS)(int nMsg, int nSessionID, long lParam1, long lParam2, void *lpParam);

/// NDDS 콜백 메세지 종류

/** 신규 추가 TYPE은 제일 하단에 추가 바람.**/
enum DeMsgType{
	DE_MSGTYPE_NDDS_INIT = 1,				///< NDDS 초기화 완료							[ lParam1:오류 코드]
	DE_MSGTYPE_ROUTE_INIT,					///< 경로탐색 초기화 중(Input 데이터 설정)		[ lParam1:탐색옵션(DeRouteOption) ]
	DE_MSGTYPE_ROUTE_START,					///< 경로탐색 시작								[ lParam1:탐색옵션(DeRouteOption) ]
	DE_MSGTYPE_ROUTE_END,					///< 경로탐색 완료								[ lParam1:탐색옵션(DeRouteOption), lParam2:탐색성공개수, lpParam:탐색결과(DS_ROUTE_RESULT) - 에러 종류 포함 ]
	DE_MSGTYPE_ROUTE_DATA,                   ///< 경로탐색결과데이터                        [ lParam1:탐색옵션(DeRouteOption), lParam2:데이터사이즈, lpParam:탐색결과데이터]

	DE_MSGTYPE_SEARCH_INIT,					///< 검색 초기화 중(Input 데이터 설정)			[ lParam1:검색종류(DeSearchType) ]
	DE_MSGTYPE_SEARCH_START,					///< 검색 시작								[ lParam1:검색종류(DeSearchType) ]
	DE_MSGTYPE_SEARCH_END,					///< 검색 완료								[ lParam1:검색종류(DeSearchType), lParam2:검색개수 ]

	DE_MSGTYPE_FAVORITE_INIT,					///< 즐겨찾기 초기화 중(Input 데이터 설정)			[ lParam1:검색종류(DeSearchType) ]
	DE_MSGTYPE_FAVORITE_START,					///< 즐겨찾기 시작								[ lParam1:검색종류(DeSearchType) ]
	DE_MSGTYPE_FAVORITE_END,					///< 즐겨찾기 완료								[ lParam1:검색종류(DeSearchType), lParam2:검색개수 ]

	DE_MSGTYPE_TRAFFICINFO_INIT,			///< 교통정보 초기화 중						[ ]
	DE_MSGTYPE_TRAFFICINFO_START,			///< 교통정보 시작								[ ]
	DE_MSGTYPE_TRAFFICINFO_END,				///< 교통정보 완료								[ lParam1:조회결과]

	DE_MSGTYPE_AUTH_INIT,					///< 인증 초기화 중							[ ]
	DE_MSGTYPE_AUTH_START,					///< 인증 시작								[ ]
	DE_MSGTYPE_AUTH_PROGRESS,				///< 인증 진행중								[ lParam1:, lParam2:, lpParam:ErrorCode ]
	DE_MSGTYPE_AUTH_CHANGE,					///< 인증 AccessKey 변경됨					[ lParam1:, lParam2:, lpParam:AccessKey ]
	DE_MSGTYPE_AUTH_END,					///< 인증 완료								[ lParam1:사용자인증결과, lParam2:단말인증결과, lpParam:AccessKey ]

    DE_MSGTYPE_NONNDDS_END,
};

/// NDDS 에러 코드 정의
enum DeErrorCode
{
	DE_ERROR_SUCCESS = 0,					///< 성공
	DE_ERROR_UNKNOWN = -1,					///< 실패(알수없음)

	/// 공통 에러 (-99)
	DE_ERROR_FAIL = -99,					///< 일반적인 실패
	DE_ERROR_INIT_NDDS,						///< NDDS 초기화 실패
#ifdef _WIN32
	DE_ERROR_INIT_SDL,						///< SDL 초기화 실패
#endif
	DE_ERROR_NOT_INITIALIZED,

	DE_ERROR_NOT_REQMEMORY,					///< 요청 메모리 할당되지 않음
	DE_ERROR_INVALID_PARAM,					///< 파라미터가 허용된 범위의 값이 아님
    DE_ERROR_SERVER_INFO,                   ///< 서버정보 오류

	/// 경로 탐색 관련 에러 (-299)
	DE_ERROR_ROUTE_MULTIOPTION = -299,		///< 멀티 경로 요청함 (지원하지 않음)
	DE_ERROR_ROUTE_POSITION,					///< 경로 탐색 지점 정보가 잘 못되었습니다.
	DE_ERROR_ROUTE_POSITIONSAME,				///< 경로 탐색 지점이 동일 합니다.
	DE_ERROR_ROUTE_USERCANCEL,				///< 경로 탐색 사용자 취소 했습니다.
	DE_ERROR_ROUTE_DISTOVER,					///< 경로 최대거리를 초과 했습니다.
	DE_ERROR_ROUTE_SERVER,					///< 경로 서버 통신 실패 했습니다.
	DE_ERROR_ROUTE_FAIL,						///< 경로 탐색 실패 했습니다.
	DE_ERROR_ROUTE_EMPTY,					///< 탐색된 경로가 없습니다.
	DE_ERROR_ROUTE_DUPLICATION,				///< 경로 탐색 중복 요청
	DE_ERROR_ROUTE_TIMEOUT,				///< 경로 탐색 시간 초과.

	/// 검색 관련 에러 (-399)
	DE_ERROR_SEARCH_MAXNAME = -399,			///< 검색어 크기가 37자를 초과 했습니다.
	DE_ERROR_SEARCH_TYPE,					///< 검색 종류가 맞지 않습니다.
	DE_ERROR_SEARCH_ADDRESSTYPE,			///< 주소 타입이 맞지 않습니다.
	DE_ERROR_SEARCH_NOTMPP,					///< MPP 정보가 존재 하지 않습니다.
	DE_ERROR_SEARCH_SERVER,					///< 검색 서버 통신 실패 했습니다.
	DE_ERROR_SEARCH_DUPLICATION,			///< 검색 중복 요청.
	DE_ERROR_SEARCH_POIID,					///< POI ID 오류.

	/// 즐겨찾기 관련 에러 (-499)
	DE_ERROR_FAVORITE_MAXNAME = -499,			///< 검색어 크기가 37자를 초과 했습니다.
	DE_ERROR_FAVORITE_TYPE,					///< 검색 종류가 맞지 않습니다.
	DE_ERROR_FAVORITE_ADDRESSTYPE,			///< 주소 타입이 맞지 않습니다.
	DE_ERROR_FAVORITE_NOTMPP,					///< MPP 정보가 존재 하지 않습니다.
	DE_ERROR_FAVORITE_SERVER,					///< 검색 서버 통신 실패 했습니다.
	DE_ERROR_FAVORITE_DUPLICATION,			///< 검색 중복 요청.
	DE_ERROR_FAVORITE_POIID,					///< POI ID 오류.

	/// 인증 관련 에러 (-599)
	DE_ERROR_AUTH_OTPCODE_MAX = -599,		///< 인증코드 6자 초과

    /// NonNdds 관련 에러 (-899)
    DE_ERROR_OUTPARAM_IS_NULL = -899,       ///< Output Parameter is NullPointer
    DE_ERROR_NONNDDS_FAIL
};

/// 자동차 유류 종류
enum DeOilType
{
	DE_OILTYPE_GASOLINE = 1,				///< 휘발유
	DE_OILTYPE_DIESEL,						///< 경유
	DE_OILTYPE_LPG,							///< LPG
	DE_OILTYPE_CHAdeMO,						///< 전기(DC차데모)
	DE_OILTYPE_AC3,							///< 전기(AC3)
	DE_OILTYPE_DCCOMBO,						///< 전기(DC콤보)
	DE_OILTYPE_PREMIUMGASOLINE,				///< 프리미엄 가솔린
};

/// 클라이언트 OS 타입
enum DeOsType
{
	DE_OSTYPE_NODE = 0,						///< 미정의
	DE_OSTYPE_ANDROID,						///< Android
	DE_OSTYPE_IOS,							///< iPhone
	DE_OSTYPE_WINDOWS,						///< Windows PC
	DE_OSTYPE_WINCE,							///< Windows CE
	DE_OSTYPE_WINMOBILE,						///< Windows Mobile
	DE_OSTYPE_QNX,							///< QNX
	DE_OSTYPE_LINUX,							///< Linux
	DE_OSTYPE_ETC							///< 기타 OS
};

/// 문자셋
enum DeCharacterSet
{
	DE_CHARACTERSET_UTF8 = 0,					///< UTF8
	DE_CHARACTERSET_EUCKR						///< EUC-KR
};

/// NDDS 초기화 데이터 (통신형)
typedef struct tagDNddsInitEx
{
	bool bServerCommerce;							///< 서버 타입 (true:상용서버, flase:개발서버)
	char szServerURL[50];                           ///< 서버URL
	int nServerPort;                                ///< 서버Port
	char szAuthCode[12+1];							///< 인증 코드
	DeOsType eOSType;								///< 클라이언트 OS 종료
	char szOSVersion[29+1];							///< 단말 OS 버전
	char szModelNo[29+1];							///< 단말 모델명	예) "MM2014"
	char szAppVersion[9+1];							///< App 버전 (3자리) 예) 1.0.0
	char szAppBuildNo[6+1];							///< 빌드번호(6자리)
	char szDeviceID[20+1];							///< 장치ID
	char szDeviceIDHu[20+1];						///< 기기(HeadUnit)의 Device ID 예) A1:B2:C3:D4:E5:F6
	char szDeviceModleName[24+1];					///< 기기(HeadUnit)의 단말 모델명 예) K5
	char szDeviceModelNo[24+1];						///< 기기(HeadUnit)의 단말 모델 번호 예) JF, LF 등
	char szVendor[24+1];							///< 단말 제조사명 예) KIA
	char szSerialKey[32+1];							///< Whitelist일 경우 필수 예 ) RSM01SGA44JK43AE2ZQJIWMOM
	char szAccessKey[50];							///< AccessKey
	DeCharacterSet eCharSet;						///< 문자셋
}DS_NDDSINIT_EX;

/// 좌표 구조체
typedef struct tagDPoint
{
	int x;						///< X 좌표
	int y;						///< Y 좌표
} DS_POINT;

/// 경로 탐색 완료 콜백 추가 정보 (DE_MSGTYPE_ROUTE_END)
typedef struct tagDRouteResult
{
	DeErrorCode eErrorCode;					///< 에러코드
}DS_ROUTE_RESULT;

/// 날짜 및 시간 정보
typedef struct tagDDateTime
{	
	int nYear;							///< 년
	int nMonth;							///< 월
	int nDay;							///< 일
	int nHour;							///< 시
	int nMinute;						///< 분
	int nSecond;						///< 초
	int nWeek;							///< 요일 (0:일, 1:월, 2:화, 3:수, 4:목, 5:금, 6:토)
}DS_DATETIME;

//////////////////////////////////////////////////////////////////////////
/// 경로 탐색
//////////////////////////////////////////////////////////////////////////


/// 경로 탐색 요청 종류
enum DeRouteType
{
	DE_ROUTETYPE_NORMAL = 1,				///< 일반탐색
	DE_ROUTETYPE_ADD,					///< 경로추가
	DE_ROUTETYPE_RE_OPTION,				///< 옵션변경(재탐색 : 자동경로선택됨)
	DE_ROUTETYPE_RE_OUTROUTE,			///< 경로이탈(재탐색 : 자동경로선택됨)
	DE_ROUTETYPE_TPEG_RE_ROUTE,			///< 주기적재탐색(재탐색 : 자동경로선택됨)
	DE_ROUTETYPE_PATTERN_ARRIVAL,		///< 패턴 예측 경로탐색 도착시간안내
	DE_ROUTETYPE_PATTERN_DEPARTURE,		///< 패턴 예측 경로탐색 출발시간안내
	DE_ROUTETYPE_SUMMARY,				///< 경로요약
};

/// 경로 탐색 옵션 (다중 선택 가능)
enum DeRouteOption
{
	DE_ROUTEOPTION_OPTIMUM		= 0x0001,	///< 추천 경로 우선
	DE_ROUTEOPTION_EXPRESSWAY	= 0x0002,	///< 고속도로 우선
	DE_ROUTEOPTION_FREEWAY		= 0x0004,	///< 일반도로 우선
	DE_ROUTEOPTION_SHORTESTWAY	= 0x0008,	///< 최단 경로 우선
	DE_ROUTEOPTION_EASY			= 0X0010,	///< 초보자 경로 우선
	DE_ROUTEOPTION_SHORTTIME	= 0X0020,	///< 최단시간 경로 우선
};

/// 자동차 종별
enum DeVehicleClass
{
    DE_VEHICLECLASS_1 = 1,				///< 1종 : 승용차, 소형 승합차
    DE_VEHICLECLASS_2,					///< 2종 : 중형 승합차, 중형 화물차
    DE_VEHICLECLASS_3,					///< 3종 : 대형 승합차, 2축 대형 화물차
    DE_VEHICLECLASS_4,					///< 4종 : 3축 대형 화물차
    DE_VEHICLECLASS_5,					///< 4축 이상 특수화물차
    DE_VEHICLECLASS_6					///< 6종 : 경차
};

/// 경로 지점 타입
enum DePosType
{
	DE_POSTYPE_NONE = 0,					///< 정보 없음
	DE_POSTYPE_START,					///< 출발지
	DE_POSTYPE_VIA1,						///< 첫번째 경유지
	DE_POSTYPE_VIA2,						///< 두번째 경유지
	DE_POSTYPE_VIA3,						///< 세번째 경유지
	DE_POSTYPE_GOAL,						///< 목적지
};

/// 요청지점 도로 타입
enum DeRoadType
{
	DE_ROADTYPE_NONE = 0,					///< 값 없음(default)
	DE_ROADTYPE_HIGHWAY,					///< 고속도로
	DE_ROADTYPE_CARWAY,						///< 자동차 전용도로(도시고속화 도로)
	DE_ROADTYPE_OVERPASS,					///< 고가도로
	DE_ROADTYPE_UNDERPASS,					///< 지하도로
	DE_ROADTYPE_GENERAL,					///< 일반도로
	DE_ROADTYPE_NEARWAY,					///< 가까운 도로
	DE_ROADTYPE_NOTSELECTED,				///< 미선택
};


/// 탐색 요청 지점 정보 (출발지, 경유지, 목적지)
typedef struct tagDRoutePos
{
	char szName[D_POSNAME_LEN];		///< 명칭
	DS_POINT tp;						///< 좌표
	int nPoiID;							///< POI ID (POI에서 제공한 값)
	int nAddrCode;						///< 주소 코드 (POI에서 제공한 값)
	unsigned char byRPFlag;				///< RPFlag (POI에서 제공한 값)
	int nMppCnt;						///< MPP 좌표 개수 (POI에서 제공한 값)
	DS_POINT tpMpp[D_MPP_CNT];			///< MPP 좌표 배열 (POI에서 제공한 값)
	char szNavSeq[7];						///< 입구점번호
}DS_ROUTE_POS;

// 탐색 요청 GPS 궤적 정보
typedef struct tagDGPSTraceData
{
	int nGpsTraceDataSize;
	char *pGpsTraceData;					// Encode_Base64 변환 데이터
}DS_ROUTE_GPSTRACE;

typedef struct tagDReRouteInfo
{
	//탐색 조건 데이터
	int			nRouteDataSize;
    char		*pszRouteData;				// Encode_Base64 변환 데이터
	//주변링크 정보
	bool		bAroundLink;
	unsigned char	uOldRsdCode;				// 이탈 지점 기존경로 RSD 회전 코드
	unsigned char	uRsdCode;					// 진입 RSD 회전 코드
	unsigned short	uLinkID;					// 링크 id
	unsigned int	uTileCode;					// 타일코드(Link)
	unsigned char	uDirInfo;					// 방향 (0:정방향, 1:역방향)
	unsigned int	uPosX;						// 보간점 X 좌표(경도: 정수8자리 SK정규화좌표)
	unsigned int	uPosY;						// 보간점 Y 좌표(위도: 정수8자리 SK정규화좌표)
	//톨게이트 정보
	unsigned short	uPassTollID;				// 최종통과 톨게이트 ID

	int		nLastRid;				//마지막 매칭 RID
	int		nTcRid;					//이전경로 정체 RID(TVAS5.2이상에서 사용됨)
    char	szPreMapVersion[9];		//이전경로 맵버전
	int		nPreRidsCnt;			//이전경로 RID목록 개수
	int		*pnPreRids;				//이전경로 RID목록
	int		nPreSecsCnt;				//이전경로 RID의 소요시간 목록 개수
	int		*pnPreSecs;				//이전경로 RID의 소요시간 목록
	char	szPreTollgateIds[32];		//이전경로 톨게이트 ID데이터
	int		nInitSrchLength;		//초기탐색 직선거리
    char	szInitSrchSessionId[25];	//초기탐색 SessionId;
}DS_ROUTE_REROUTEINFO;

// - 지난 위치 정보(pastPositionInfo)
typedef struct tagDPastPositionInfo{
    char szPastSessionId[25];
    int nPastXpos;
    int nPastYpos;
    char szPastArrivalTime[15];
}DS_PASTPOSITIONINFO;

/// 경로 탐색 요청 데이터
typedef struct tagDRouteServerReq
{
	char szTvas[5+1];					///< TVAS버전 5.0이상
	int nRouteOption;					///< 탐색 옵션 (DeRouteOption |로 다중선택 최대 3개. DE_ROUTEOPTION_OPTIMUM | DE_ROUTEOPTION_FREEWAY)
	bool bStartGpsPos;					///< 출발지 GPS 좌표 여부 (true 이면 출발지는 UI에서 마지막 GPS 정보를 사용 - GPS 궤적 정보를 채워서 요청)
	DS_DATETIME sDataTime;				///< 시간 (로컬실시간길안내, 서버실시간길안내:현재시간, 도착시간예측:출발시간, 출발시간예측:도착시간)
	DeVehicleClass eVehicleClass;		///< 자동차 종별
	bool bHipass;						///< 하이패스 유무
	DeOilType eOilType;					///< 유류 종류
	int nPosCnt;						///< 탐색 요청 지점 개수 (출발지/목적지 포함하여 2~5개)
	DS_ROUTE_POS sPosInfo[D_POS_CNT];	///< 탐색 요청 지점 정보 (출발지, [경유지1,2,3], 목적지)
	DeRoadType eRoadType;				///< 출발지 도로 타입
	unsigned char byDepartDirPriority;	///< 출발지점 주행방향(0 : 주행방향 비우선(default), 1 : 주행방향 우선)
	unsigned char byDepartSrchFlag;		///< 출발지 탐색 구분(0x00 : GPS, 0x04 : 최근길, 0x05 : 즐겨찾기, 0x06 : 경위도검색, 0x07 : 명칭검색, 0x08 : 주소검색)
	int nControlRouteReqFlag;			///< 통제 길안내 요청 여부(0:요청안함, 1:요청함, 2:서버에서 판단)
	int	nAngle;							///< 각도(진북 기준, 0~359 )
	int	nSpeed;							///< 차량진행 속도( Km/h)
    unsigned char byDestSrchFlag;		///< 목적지 탐색 구분 ID(ID는 "T map 채널 연동규격서 참고_NDDS" 문서 참고)
	DS_ROUTE_GPSTRACE sGpsTraceData;	///< GPS 궤적 정보
	bool bpAddCameraType[D_ADD_CAMERATYPE];		///< 탐색 요청 단속카메라 추가정보 여부 ([0]선택없음, [1]버스전용차로, [2]신호, [3]이동식, [4]갓길, [5]가변차로(사용안함), [6]과적(사용안함), [7]방범(사용안함), [8]교통정보)
	bool bpAddDAType[D_ADD_CAMERATYPE];		///< 탐색 요청 위험구역 추가정보 여부 ([0]선택없음, [1]사고다발, [2]급커브, [3]급경사(사용안함), [4]로드킬(사용안함), [5]안개, [6]낙석(사용안함), [7]어린이보호, [8]철길건넌목)
	DS_ROUTE_REROUTEINFO sReRouteInfo;	///< 재탐색 정보
    DS_PASTPOSITIONINFO sPassPosInfo;   ///< 지난 위치 정보
	char	szCollectDataPath[3][D_MAX_PATH];			///< 탐색 결과 파일 저장 될 파일 경로
    char    szGuideImageCode[10];      /// "NotSupported", "R170x207", "R240x252", "R320x316", "R341x415", "R480x504" ,"R480x540"
}DS_ROUTE_SERVER_REQ;

/// 경로 탐색 요청 종류
enum DeRouteSummaryType
{
	DE_ROUTESUMMARYTYPE_OPTIMUM = 0,			///< 교통최적+추천
	DE_ROUTESUMMARYTYPE_FREE,					///< 교통최적+무료우선
	DE_ROUTESUMMARYTYPE_SHORT = 10,				///< 최단거리+유/무료
};

/// 경로 서비스 구분
enum DeRouteServiceType
{
	DE_ROUTESERVICETYPE_REALTIME_SIMPLEX = 0,			///< 실시간(단방향)
	DE_ROUTESERVICETYPE_REALTIME_DUPLEX,				///< 실시간(양방향)
	DE_ROUTESERVICETYPE_ARRIVAL,						///< 도착시간 예측 길안내
	DE_ROUTESERVICETYPE_DEPARTURE = 10,					///< 출발시간 예측 길안내
};

/// 출발지 탐색 구분
enum DeRouteSrchFlag
{
	DE_ROUTESRCHFLAG_GPS = 0x00,				///< 실시간(단방향)
	DE_ROUTESRCHFLAG_RECENT = 0x04,				///< 실시간(양방향)
	DE_ROUTESRCHFLAG_FAVORITE = 0x05,			///< 도착시간 예측 길안내
	DE_ROUTESRCHFLAG_LONLAT = 0x06,				///< 출발시간 예측 길안내
	DE_ROUTESRCHFLAG_POINAME = 0x07,			///< 출발시간 예측 길안내
	DE_ROUTESRCHFLAG_ADDR = 0x08,				///< 출발시간 예측 길안내
};

/// 경로 요약 요청 데이터
typedef struct tagDRouteServerSummaryReq
{
	DeRouteSummaryType eRouteSummaryType;				///< 탐색 요청 종류
	DeRouteServiceType eRouteServiceType;				///< 경로 서비스 구분
	DS_DATETIME sDataTime;				///< 시간 (로컬실시간길안내, 서버실시간길안내:현재시간, 도착시간예측:출발시간, 출발시간예측:도착시간)
	int nVertexFlag;					///< 경로 좌표 요청 구분(0 : 경로 좌표 요청 안함(default), 1 : 경로 좌표 요청)
	int nPosCnt;						///< 탐색 요청 지점 개수 (출발지/목적지 포함하여 2~5개)
	DS_ROUTE_POS sPosInfo[D_POS_CNT];	///< 탐색 요청 지점 정보 (출발지, [경유지1,2,3], 목적지)
	int	nAngle;							///< 각도(진북 기준, 0~359 )
	int	nSpeed;							///< 차량진행 속도( Km/h)
	unsigned char byDepartDirPriority;			///< 출발지점 주행방향(0 : 주행방향 비우선(default), 1 : 주행방향 우선)
	unsigned char byDepartSrchFlag;				///< 출발지 탐색 구분(0x00 : GPS, 0x04 : 최근길, 0x05 : 즐겨찾기, 0x06 : 경위도검색, 0x07 : 명칭검색, 0x08 : 주소검색)
	unsigned char byDestSrchFlag;				///< 목적지 탐색 구분
}DS_ROUTE_SERVERSUMMARY_REQ;

/// 경로 탐색 요청 데이터
typedef struct tagDRouteReq
{
	DeRouteType eRouteType;				///< 탐색 요청 종류
	DS_ROUTE_SERVER_REQ sRouteServerReq; ///< 길안내 경로탐색
	DS_ROUTE_SERVERSUMMARY_REQ sRouteServerSummaryReq; ///< 경로요약
}DS_ROUTE_REQ;

/// 경로 탐색 결과 날씨 데이터
typedef struct tagDWeatherInfoRes
{
	char szSkyCode[8];			///< 하늘 상태 코드(날씨 코드)
	char szSkyName[30];			///< 하늘 상태 코드명(날씨 명)
	char szRainTypeCode[2];		///< 강수 형태 코드(0:현상없음, 1:비, 2:비/눈, 3:눈)
	char szRainSinceOnTime[7];	///< 1시간 누적 강수량
	char szTempC1h[6];			///< 1시간 기온
}DS_WEATHERINFO_RES;

/// 경로 탐색 결과 날씨 데이터
typedef struct tagDRouteSummaryInfoRes
{
	int nTotalTime;				///< 경로 총 소요시간 (초 단위)
	int nEstimationOption;		///< 예측구분코드
	char szEstimationTime[16];	///< 출발/도착예측시간(YYYYMMDDHHMMSSS)

}DS_ROUTESUMMARYINFO_RES;

////////////////////////////////////////////////////////////////////////////////////////
// 검색 관련
////////////////////////////////////////////////////////////////////////////////////////


#define D_MAX_RESULTLIST		8		///< 검색 결과 최대 개수
#define D_MAXLEN_SISULNAME  	160		///< 시설물 이름 길이 제한
#define D_MAXLEN_ADDRNAME		80		///< 최대 주소 글자수
#define D_MAXLEN_CATENAME		28		///< 최대 업종 글자수
#define D_MAXLEN_PHONENUMBER	14		///< 최대 전화번호 글자수
#define D_MAXLEN_URL			256		///< 웹사이트 글자수
#define D_MAXLEN_DETAILINFO		3000	///< 상세정보 글자수
#define D_MAX_POIIMAGEINFO		5		///< POI 상세정보 이미지 개수
#define D_MAX_POIPARKINFO		3		///< POI 상세정보 주차장 개수
#define D_MAXLEN_SVCDATE	16		///< 최종 서비스일시(YYYYMMDDHH24MISS)
#define D_MAXLEN_DATAKIND  2			///< 주유소/충전소 타입
#define D_MAXLEN_FUELTYPE  10			///< 연료타입
#define D_MAXLEN_FAVADDINFO 100		///< 즐겨찾기 부가정보
#define D_MAXELN_FAVORITELIST		100		///< 등록 최대 개수
#define D_MAXLEN_CDNAME		20		///< 지역 분류면 글자수
#define D_MAXLEN_ROADNAME	20		///< 도로명 글자수
#define D_MAXLEN_ZIPCODE	10		///< 우편번호 글자수
#define D_MAXLEN_MENU		10		///< 메뉴 글자수
#define D_MAXLEN_TELNO		20		///< 전화번호 글자수
#define D_MAXLEN_ROAD		300		///< 찾아오는길 글자수
#define D_MAXLEN_CATECODE	10		///< 아이콘표출용 업종코드 글자수
#define D_MAXLEN_JOINYN		20		///< 가맹점여부정보 글자수
#define D_MAXLEN_JOINVIEWTYPE	20	///< 가맹점 화면 타입 글자수
#define D_MAXLEN_TITLE		30		///< 경로 제목 글자수
#define D_MAXLEN_CLSNAME	20		///< 업종 글자수
#define D_MAXLEN_ADDINFO	100		///< 부가정보
#define D_MAXLEN_ROUTERECENT	100 ///< 최근길 리스트 최대개수
#define D_MAXLEN_POIFAVORITE	100 ///< poi 즐겨찾기 최대개수
#define D_MAXLEN_ROUTEFAVORITE	100 ///< 경로 즐겨찾기 최대개수
#define D_MAXLEN_IDCODE		 	10	///< 충전기 코드 글자수
#define D_MAXLEN_USETIME	20		///< 전기차 충전기 이용시간
#define D_MAXLEN_EVCHARGER	25		///< 전기차 충전기 최대개수
#define D_MAXLEN_ROADADDR   20		///< 전기차 충전소 도로명주소
#define D_MAXLEN_ADDRDETAIL 30		///< 전기차 충전소 상세주소
#define D_MAXLEN_MNGNAME    20		///< 전기차 충전소 운영기관
#define D_MAXLEN_MNGID      10		///< 전기차 충전소 기관ID
#define D_MAXLEN_FEE        10		///< 전기차 충전 단가
#define D_MAXLEN_DAYOFFINFO	256
#define D_MAXLEN_BUSINESSHOURS 128
#define D_MAXLEN_DAYOFFDATE	256

/// 검색 종류 타입
enum DeSearchType
{
	DE_SEARCHTYPE_NONE = 0,				///< 미정의
	DE_SEARCHTYPE_SERVER,				///< NDDS POI 검색
	DE_SEARCHTYPE_SERVERAROUND,			///< NDDS POI 검색 5.2.2. 주변지 검색
	DE_SEARCHTYPE_SERVERPOIDETAILINFO,	///< NDDS POI 검색 5.2.3. POI 상세정보 검색
	DE_SEARCHTYPE_SERVERPOIJOININFO,	///< NDDS POI 검색 5.2.4. 가맹점 상세정보 검색
	DE_SEARCHTYPE_SERVERRECENTDESTNATION,///< NDDS POI 검색 5.2.5. POI 최근길 조회
    DE_SEARCHTYPE_SERVERUPLOADRECENTDESTNATION,///< NDDS POI 검색 5.2.6. POI 최근길 전체 업로드
    DE_SEARCHTYPE_SERVERREMOVERECENTDESTNATION,///< NDDS POI 검색 5.2.7. POI 최근길 멀티삭제
	DE_SEARCHTYPE_SERVERADDR,			///< NDDS 주소 검색 5.2.11. 지역분류별 주소명 조회
    DE_SEARCHTYPE_SERVERPOISTATIONINFO,	///< NDDS 주소 검색 5.2.12. 최저가 주유소(충전소) 조회
    DE_SEARCHTYPE_SERVERUSERDATA,		///< NDDS 검색 5.2.14. 최근길/즐겨찾기 조회
    DE_SEARCHTYPE_SERVERPOIEVSTATIONINFO,///< NDDS 검색 5.2.16. 전기차 충전소 ePoi상세조회
    DE_SEARCHTYPE_SERVERPOIEVSTATIONSBYPOINT,///< NDDS 검색 5.2.16. 전기차 충전소 ePoi상세조회
};

/// Search 정렬 방식
enum DeSearchSort
{
	DE_SORTTYPE_NONE = 0,				///< 정렬 안함
	DE_SORTTYPE_NAME,					///< 이름순 정렬
	DE_SORTTYPE_DIST,					///< 거리순 정렬
	DE_SORTTYPE_PRICE,					///< 가격 (유가정보 일때만 의미 있음)
};

/// 서버 Search 정렬 방식
enum DeServerSearchSort
{
	DE_SERVER_SORTTYPE_ACCURACY = 0,		///< 정확도순 정렬
	DE_SERVER_SORTTYPE_DIST,				///< 거리순 정렬
};

/// 명칭 검색 종류
enum DeSisualNameType
{
	DE_SISUALNAMETYPE_ALL = 0,			///< 전체 (POI + 전화번호)
	DE_SISUALNAMETYPE_POI,				///< POI
	DE_SISUALNAMETYPE_TEL				///< 전화번호
};


/// 주유소 종류
enum DeOilStation
{
	DE_OILSTATION_ALL = 0,				///< 전체
	DE_OILSTATION_SK,					///< SK
	DE_OILSTATION_GS,					///< GS칼텍스
	DE_OILSTATION_HYUNDAE,				///< 현대오일
	DE_OILSTATION_SOIL,					///< S-OIL
	DE_OILSTATION_ETC,					///< 기타
};

/// 주변지 검색 종류
enum DeAroundPoiType
{
	DE_AROUNDTYPE_NONE = 0,					///< 정보 없음
	DE_AROUNDTYPE_OPI,						///< 주유소
	DE_AROUNDTYPE_LPG,						///< 충전소
	DE_AROUNDTYPE_RESTAURANT,				///< 음식점
	DE_AROUNDTYPE_MART,						///< 마트/편의점
	DE_AROUNDTYPE_PARK,						///< 주차장
	DE_AROUNDTYPE_GARAGE,					///< 정비소
	DE_AROUNDTYPE_BANK,						///< 은행/ATM
	DE_AROUNDTYPE_HOSPITAL,					///< 병원/약국
	DE_AROUNDTYPE_TRAFFIC,					///< 교통
	DE_AROUNDTYPE_SHOPPING,					///< 쇼핑
	DE_AROUNDTYPE_LODGMENT,					///< 숙박
	DE_AROUNDTYPE_TOUR,						///< 관광/레저/문화
	DE_AROUNDTYPE_OPI_EV,					///< 전기차
};

/// 서버 POI 검색 요청 데이터
typedef struct tagDSearchServerReq
{
	DeSearchType eSearchType;						///< 검색 종류

	DeServerSearchSort eSortType;					///< 정렬 기준
	DS_POINT tpCurrent;								///< 현위치 좌표 (UI 제공 좌표)
	char szSearchName[D_MAXLEN_SISULNAME+1];		///< 검색어 (최대 37byte)
	char szAddrName[D_MAXLEN_SISULNAME+1];			///< 주소 필터 (필터 없을때 0으로 설정)
	int nDist;										///< 주변 검색 거리 (0:default 센터 위임, 1이상:300m 단위) 예) 3:900m
	DeOilType eOilType;								///< 유류 종류
	bool bNewAddr;									///< POI주소를 새주소로 표출할것인지 구주소로 표출할것인지 구분(구주소:false, 새주소:true)
}DS_SEARCH_SERVER_REQ;

/// 서버 주변지 검색 요청 데이터
typedef struct tagDSearchServerAroundReq
{
	DeSearchType eSearchType;						///< 검색 종류

	DS_POINT tpCurrent;								///< 현위치 좌표 (UI 제공 좌표)
	DeAroundPoiType eAroundPoiType;					///< 주변지 종류
	char szSearchName[D_MAXLEN_SISULNAME+1];		///< 검색어 (최대 37byte)
	int nDist;										///< 주변 검색 거리 (0:default 센터 위임, 1이상:300m 단위) 예) 3:900m
	bool bNewAddr;									///< 주소필터링시 주소코드가 구주소인지 새주소인지 구분(구주소:false, 새주소:true)
}DS_SEARCH_SERVERAROUND_REQ;

// 서버 POI 상세정보 요청 데이터
typedef struct tagDSearchServerPoiDetailInfoReq
{
	DeSearchType eSearchType;						///< 검색 종류

	int	nPoiID;										///< POI ID
	char navSeq[7];								///< 입구점번호
	int nSizeWidth;									///< 화면 가로 사이즈
	int nSizeHeight;								///< 화면 세로 사이즈
}DS_SEARCH_SERVERPOIDETAILINFO_REQ;

// 서버 가맹점 상세정보 요청 데이터
typedef struct tagDSearchServerPoiJoinInfoReq
{
	DeSearchType eSearchType;						///< 검색 종류

	int	nPoiID;										///< POI ID
}DS_SEARCH_SERVERPOIJOININFO_REQ;

// 서버 POI 최근길 조회 요청 데이터
typedef struct tagDSearchServerRecentDestnationReq
{
	DeSearchType eSearchType;						///< 검색 종류
}DS_SEARCH_SERVERRECENTDESTNATION_REQ;

/// POI 최근길 전체 업로드
typedef struct tagDSearchUploadRecentDestnation{
    char szName[D_MAXLEN_SISULNAME+1];				///< 명칭
    DS_POINT tp;									///< 위치 좌표
    int nPoiID;										///< POI ID
    char szNavseq[7];								///< 입구점번호
    unsigned char byRPFlag;							///< RPFlag (경로 탐색시 사용)
    char nToltalCnt[D_MAXLEN_SVCDATE+1];			///< 총 서비스 횟수, 요청의 svcUploadYn이 "Y"면 필수
    char szSvcDate[D_MAXLEN_SVCDATE+1];				///< 최종 서비스일시 (YYYYMMDDHH24MISS) 요청의 svcUploadYn이 "Y"면 필수
}DS_SEARCH_UPLOAD_RECENT_DESTNATION;

/// POI 최근길 삭제 정보
typedef struct tagDSearchRemoveRecentDestnation{
    char szName[D_MAXLEN_SISULNAME+1];				///< 명칭
    DS_POINT tp;									///< 위치 좌표
}DS_SEARCH_REMOVE_RECENT_DESTNATION;

// 서버 POI 최근길 전체 업로드 요청 데이터
typedef struct tagDSearchServerUploadRecentDestnationReq
{
    DeSearchType eSearchType;						///< 검색 종류

    int nUploadCnt;									// 업로드 poi 개수
    char szSvcUploadYn[7+1];						///서비스 속성정보 업로드 여부("Y"/"N")
    DS_SEARCH_UPLOAD_RECENT_DESTNATION sUploadRecent[100];
}DS_SEARCH_SERVERUPLOADRECENTDESTNATION_REQ;

/// 서버 POI 최근길 멀티 삭제 요청 데이터
typedef struct tagSearchServerRemoveRecentDestnation{
    DeSearchType eSearchType;								///< 검색 종료

    int nRemoveCnt;											///< 삭제 poi 개수
    DS_SEARCH_REMOVE_RECENT_DESTNATION sRemoveRecent[100];	///< 삭제 최근길 리스트
}DS_SEARCH_SERVERREMOVERECENTDESTNATION_REQ;

/// 서버 주소 검색 요청 데이터
typedef struct tagDSearchServerAddrReq
{
	DeSearchType eSearchType;						///< 검색 종류

	char szAddrName1[D_MAXLEN_SISULNAME+1];		///< 검색어 (최대 37byte)
	char szAddrName2[D_MAXLEN_SISULNAME+1];		///< 검색어 (최대 37byte)
	bool bNewAddr;									///< 주소필터링시 주소코드가 구주소인지 새주소인지 구분(구주소:false, 새주소:true)
}DS_SEARCH_SERVERADDR_REQ;

/// 최저가 주유소 검색 요청 데이터
typedef struct tagDSearchServerPoiStationInfoReq
{
    DeSearchType eSearchType;						///< 검색 종류

    DS_POINT tp;									///< 현위치 좌표
    int nDist;										///< 주변 검색 거리 (9:default 센터 위임, 1이상:300m 단위) 예) 3:900m
    char szDataKind[D_MAXLEN_DATAKIND];				///< 3:주유소, 4:충전소(3:default)
    char szFuelType[D_MAXLEN_FUELTYPE];				///< 연료타입 fuletype이 있으면 datakind를 무시(FT_GAS:휘발유, FT_DISEL:경유, FT_LPG:LPG, FT_GASPM:고급휘발유)
}DS_SEARCH_SERVERPOISTATIONINFO_REQ;

/// ev충전소 상세검색 요청 데이터
typedef struct tagDSearchServerPoiEvStationInfoReq
{
    DeSearchType eSearchType;						///< 검색 종류

    int nPoiID;										///< 현위치 좌표
}DS_SEARCH_SERVERPOIEVSTATIONINFO_REQ;

/// ev충전소 상세검색 요청 데이터
typedef struct tagDSearchServerPoiEvStationsByPointReq
{
    DeSearchType eSearchType;						///< 검색 종류

    DS_POINT tp;									///< 현위치 좌표
    int nDist;										///< 주변 검색 거리 (9:default 센터 위임, 1이상:300m 단위) 예) 3:900m
}DS_SEARCH_SERVERPOIEVSTATIONSBYPOINT_REQ;

/// 검색 요청 (공통)
typedef union tagDSearchReq
{
	DeSearchType eSearchType;						///< 검색 종류

	DS_SEARCH_SERVER_REQ sSearchServer;				///< NDDS POI 검색
	DS_SEARCH_SERVERAROUND_REQ sSearchServerAround;	///< NDDS 주변지 검색
	DS_SEARCH_SERVERPOIDETAILINFO_REQ sSearchServerPoiDetailInfo;	///< NDDS POI 상세정보
	DS_SEARCH_SERVERPOIJOININFO_REQ sSearchServerPoiJoinInfo;	///< NDDS 가맹점 상세정보
	DS_SEARCH_SERVERRECENTDESTNATION_REQ sSearchRecentDestnation;   ///< POI 최근길 검색
    DS_SEARCH_SERVERUPLOADRECENTDESTNATION_REQ sSearchUploadRecentDestnation;   ///< POI 최근길 전체 업로드
    DS_SEARCH_SERVERREMOVERECENTDESTNATION_REQ sSearchRemoveRecentDestnation;   ///< POI 최근길 멀티 삭제
	DS_SEARCH_SERVERADDR_REQ sSearchServerAddr;		///< NDDS 주소 검색
    DS_SEARCH_SERVERPOISTATIONINFO_REQ sSearchServerPoiStationInfo;		///< NDDS 최저가 주유소(충전소) 검색
    DS_SEARCH_SERVERPOIEVSTATIONINFO_REQ sSearchServerPoiEvStationInfo;		///< NDDS ev충전소 검색
    DS_SEARCH_SERVERPOIEVSTATIONSBYPOINT_REQ sSearchServerPoiEvStationsByPoint;		///< NDDS ev충전소 검색
}DS_SEARCH_REQ;

// 전기차 충전기 리스트
typedef struct tagDEvStationChargerInfo
{
    char szBid[D_MAXLEN_IDCODE+1];	// 기관코드 BG:비긴스/JE:제주전기자동차서비스/KP:한국전력/HE:한국전기차충전서비스/PI:포스코ICT/SE:서울시/PW:파웤큐브/CT:씨티카/ME:환경부/JD:제주특별자치도청/TM:티맵/HM:현대자동차네비게이션
    char szSid[D_MAXLEN_IDCODE+1];	//충전소ID
    char szCid[D_MAXLEN_IDCODE+1];	//충전기D
    char szType[D_MAXLEN_IDCODE+1];	//충전기타입(01:DC차데모 /02:승용차AC완속 /03:DC차데모+AC3상 /04:DC콤보 /05:DC차데모+DC콤보 /06:DC차데모+AC3상+DC콤보 /07:AC급속3상
    int nStatus;					//상태정보(0:알수없음 / 1:통신이상 /2:충전대기 /3:충전중 /4:운영중지 /5:점검중 /6:예약중)
}DS_POI_EVSTATION_CHARGERINFO;

/// 검색 결과 리스트
typedef struct tagDSearchList{
	int nIdx;										///< 검색 결과 인덱스
	char szName[D_MAXLEN_SISULNAME+1];				///< 명칭
	char szStdName[D_MAXLEN_SISULNAME+1];			///< 대표 명칭
	char szAddrName[D_MAXLEN_ADDRNAME+1];			///< 구주소, 새주소 (새주소 요청시 DB에 새주소 정보가 없으면 구까지만 전달)
	char szMngName[D_MAXLEN_MNGNAME+1];             ///< ev충전소 운영기관
	char szPhoneNumber[D_MAXLEN_PHONENUMBER+1];		///< 전화번호
	int	nPoiID;										///< POI ID
	int	nAddrCode;									///< 주소 코드
	unsigned char byRPFlag;							///< RPFlag (경로 탐색시 사용)
	int	nDist;										///< 현위치에서 거리
	int	nDeg;										///< 방위각
	int	nKind;										///< 테마검색 일때 : 이미지 존재 1, 유가정보 일때 : 와이파이 주유소  1, 나머지 검색 : 업종코드
	DS_POINT tp;									///< 위치 좌표
	int	nOPIPoleCate;								///< 주유소종류 1:SK, 2:GS, 3:H-OIL, 4:S-OIL, 5:T-OIL, 6:무폴, 7:SK-GAS, 8:GS-GAS, 9:H-GAS, 10:S-OIL GAS, 11:LPG 무폴, 12:미확인, 13:E1, 14:SK인천정유, 15:SK인천정유GAS, 16:NC-OIL, 17:NC-OIL GAS, 18:NH-OIL, 19:NH-OIL GAS, 20:sevenD-OIL, 21:sevenD-OIL GAS, 22:알뜰
	int nOPIPrice;									///< 가격(유가 정보 일때만 유효) -1:TPEG 수신정보 없음, 0:TPEG 수신 되었으나 가격 정보 없음
	DS_DATETIME sGatherTime;						///< 유가정보 제공일자 (유가 정보 일때만 유효)

	char szOilCompany[16];							///< 주유소브랜드(SK, GS, 기타 등), 단말에서 브랜드 필터링시 사용
    int  nDataKind;                                 ///< 주유소종류 (3: 주유소, 4:충전소, 6: EV충전소)
	bool bHighGasoline;								///< 고급휘발유판매점여부 (0:판매안함, 1:판매)
	bool bLowestOilSt;								///< 최저가점포여부. 출력리스트상 최저가 점포일 경우(최저가:1, 아닌경우:0). SK주유소가 최저가인 경우에만 1로 설정
	unsigned int nOPIGasolinePrice;					///< 휘발유가격(원)
	unsigned int nOPIDieselPrice;					///< 경유가격(원)
	unsigned int nOPILPGPrice;						///< LPG가격(원)
	unsigned int nOPIHiGasolinePrice;				///< 프리미엄 휘발유 가격(원)
	unsigned int nOPIHiLPGPrice;					///< 프리미엄 LPG 가격(원)
	char	szNavSeq[7];							///< 입구점번호
    int	nEvChargerTotalCnt;                         ///< 충전기 개수
    DS_POI_EVSTATION_CHARGERINFO sChargerInfo[D_MAXLEN_EVCHARGER];	///< 충전기리스트
}DS_SEARCH_LIST;


///UserData :: POI 집으로/회사로 정보 poiMyFavorteDto
typedef struct tagDPoiMyFavorite{
    char		szHomeCustName[D_MAXLEN_SISULNAME+1];			// 집으로 명칭 (고객이 입력한 명칭)
    DS_POINT	Hometp;											// 집으로 좌표(sk 정규화 좌표 7자리)
    int			nHomePoiID;										// 집으로 POI ID
    char		szHomeNavSeq[7];								// 집으로 입구점 일련번호
    char		szHomeLcdName[D_MAXLEN_CDNAME+1];				// 집으로 지역 대분류명
    char		szHomeMcdName[D_MAXLEN_CDNAME+1];				// 집으로 지역 중분류명
    char		szHomeScdName[D_MAXLEN_CDNAME+1];				// 집으로 지역 소분류명
    char		szHomeDcdName[D_MAXLEN_CDNAME+1];				// 집으로 지역 세분류명
    char		szHomePrimaryBun[D_MAXLEN_CDNAME+1];			// 집으로 본번
    char		szHomeSecondaryBun[D_MAXLEN_CDNAME+1];			// 집으로 부번
    int			nHomeMlClass;									// 집으로 산/대지 구분자(0:알수없음,1:대지,2:산)
    char		szHomeBldAdd[D_MAXLEN_ADDRNAME+1];				// 집으로 건물주소 표출용 주소 Ex)서울 중구 세종대로
    char		szHomeRoadName[D_MAXLEN_ROADNAME+1];			// 집으로 도로명
    char		szHomeBldNo1[D_MAXLEN_ROADNAME+1];				// 집으로 건물번호1
    char		szHomeBldNo2[D_MAXLEN_ROADNAME+1];				// 집으로 건물번호2
    char		byHomeRpFlag;									// 집으로 RP FLAG
    char		szHomeAddInfo[D_MAXLEN_FAVADDINFO+1];			// 집으로 부가정보 (행정구역명,좌표)
    char		szHomeInsDatetime[D_MAXLEN_SVCDATE+1];			// 집으로 입력시일(YYYYNNDDHH24MISS)

    char		szOfficeCustName[D_MAXLEN_SISULNAME+1];			// 회사로 명칭 (고객이 입력한 명칭)
    DS_POINT	Officetp;										// 회사로 좌표(sk 정규화 좌표 7자리)
    int			nOfficePoiID;									// 회사로 POI ID
    char		szOfficeNavSeq[7];								// 회사로 입구점 일련번호
    char		szOfficeLcdName[D_MAXLEN_CDNAME+1];				// 회사로 지역 대분류명
    char		szOfficeMcdName[D_MAXLEN_CDNAME+1];				// 회사로 지역 중분류명
    char		szOfficeScdName[D_MAXLEN_CDNAME+1];				// 회사로 지역 소분류명
    char		szOfficeDcdName[D_MAXLEN_CDNAME+1];				// 회사로 지역 세분류명
    char		szOfficePrimaryBun[D_MAXLEN_CDNAME+1];			// 회사로 본번
    char		szOfficeSecondaryBun[D_MAXLEN_CDNAME+1];		// 회사로 부번
    int			nOfficeMlClass;									// 회사로 산/대지 구분자(0:알수없음,1:대지,2:산)
    char		szOfficeBldAdd[D_MAXLEN_ADDRNAME+1];			// 회사로 건물주소 표출용 주소 Ex)서울 중구 세종대로
    char		szOfficeRoadName[D_MAXLEN_ROADNAME+1];			// 회사로 도로명
    char		szOfficeBldNo1[D_MAXLEN_ROADNAME+1];			// 회사로 건물번호1
    char		szOfficeBldNo2[D_MAXLEN_ROADNAME+1];			// 회사로 건물번호2
    char		byOfficeRpFlag;									// 회사로 RP FLAG
    char		szOfficeAddInfo[D_MAXLEN_FAVADDINFO+1];			// 회사로 부가정보 (행정구역명,좌표)
    char		szOfficeInsDatetime[D_MAXLEN_SVCDATE+1];		// 집으로 입력시일(YYYYNNDDHH24MISS)
}DS_POI_MYFAVORITE;

//UserData :: 출발지/목적지/경유지정보 startRouteDto
typedef struct tagRouteInfo{
    char		szCustName[D_MAXLEN_SISULNAME+1];				// 명칭 (고객이 입력한 명칭)
    DS_POINT	tp;												// 좌표(sk 정규화 좌표 7자리)
    int			nPoiID;											// POI ID
    char		byRpFlag;										// RP FLAG
}DS_ROUTE_INFO;

///UserData :: 자주가는 경로 정보 routeMyFavoriteDto
typedef struct tagRouteFavorite{
    char routeTitle[D_MAXLEN_TITLE+1];			// 경로 제목
    char insDatetime[D_MAXLEN_SVCDATE+1];		// 입력시일(YYYYNNDDHH24MISS)
    DS_ROUTE_INFO sStartRoute;					// 출발지 정보
    DS_ROUTE_INFO sEndRoute;					// 도착지 정보
    DS_ROUTE_INFO sMidRoute[3];					// 경유지 정보 리스트(MAX 3건)
}DS_ROTUE_MYFAVORITE;

////UserData :: 최근길 리스트 정보 popiRecentDtos
typedef struct tagDRouteRecentInfo{
    int		nPoiID;									///< POI ID
    char	szNavSeq[7];							///< 입구점번호
    char	szName[D_MAXLEN_SISULNAME+1];			///< 명칭
    DS_POINT tp;									///< 위치 좌표
    char szLcdName[D_MAXLEN_CDNAME+1];				///< 지역 대분류명
    char szMcdName[D_MAXLEN_CDNAME+1];				///< 지역 중분류명
    char szScdName[D_MAXLEN_CDNAME+1];				///< 지역 소분류명
    char szDcdName[D_MAXLEN_CDNAME+1];				///< 지역 세분류명
    char szPrimaryBun[D_MAXLEN_CDNAME+1];			///< 본번
    char szSecondaryBun[D_MAXLEN_CDNAME+1];			///< 부번
    int nMlClass;									///< 산/대지 구분자(0:알수없음,1:대지,2:산)
    char szRoadName[D_MAXLEN_ROADNAME+1];			///< 도로명
    char szBldNo1[D_MAXLEN_ROADNAME+1];				///< 건물번호1
    char szBldNo2[D_MAXLEN_ROADNAME+1];				///< 건물번호2
    char szRepClsName[D_MAXLEN_CLSNAME+1];			///< 대표업종명 (퀵인포에 들어갈 항목)
    char szClsAName[D_MAXLEN_CLSNAME+1];			///< 업종 대분류명
    char szClsBName[D_MAXLEN_CLSNAME+1];			///< 업종 중분류명
    char szClsCName[D_MAXLEN_CLSNAME+1];			///< 업종 소분류명
    char szClsDName[D_MAXLEN_CLSNAME+1];			///< 업종 세분류명
    char bRpFlag;									///< RP Flag
    char szTelNo[D_MAXLEN_TELNO+1];					///< 전화번호
    int szTotalCnt;									///< 총 서비스 횟수
    char szSvcDate[D_MAXLEN_SVCDATE+1];				///< 최종 서비스 일시
}DS_ROUTE_RECENT;

////UserData :: Poi 즐겨찾기 리스트 정보
typedef struct tagDRouteFavoriteInfo{
    char szRouteTitle[D_MAXLEN_TITLE+1];			///< 경로명칭
    char szInsDateTime[D_MAXLEN_SVCDATE+1];			///< 입력시일
    DS_ROUTE_INFO sStartRoute;						///< 출발지 정보
    DS_ROUTE_INFO sEndRoute;						///< 도착지 정보
    DS_ROUTE_INFO sMidRoute[3];					///< 경유지 정보 릿트( MAX 3건)
}DS_ROUTE_FAVORITE;

typedef struct tagDPoiImageInfos{
	int nSeq;										///< 이미지 순번
	char szFileSrc[D_MAXLEN_URL+1];					///< POI 이미지 경로
}DS_POI_IMAGE_INFOS;

typedef struct tagDPoiParkInfos{
	char szCpName[D_MAXLEN_SISULNAME+1];			///< 제공처명 예)"모두의주차장","파킹박"
	char szParkUrl[D_MAXLEN_URL+1];					///< 모바일 웹 링크 URL
}DS_POI_PARK_INFOS;

/// POI 상세정보
typedef struct tagDSearchPoiDetailInfo{
	int nPoiID;										///< POI ID
	char	szNavSeq[7];							///< 입구점번호
	char szName[D_MAXLEN_SISULNAME+1];				///< 명칭
	char szAddrName[D_MAXLEN_ADDRNAME+1];			///< 구주소, 새주소 (새주소 요청시 DB에 새주소 정보가 없으면 구까지만 전달)
	DS_POINT tp;									///< 위치 좌표
	char szPhoneNumber[D_MAXLEN_PHONENUMBER+1];		///< 전화번호
	char szUrl[D_MAXLEN_URL+1];						///< 웹사이트
	bool bHoliday;									///< 연중무휴(true:연중무휴)
	char byPark;									///< 주차 가능 여부(0:불가능, 1:가능, 그외:정보없음)				
	char szCateName[D_MAXLEN_SISULNAME+1];			///< 업종 명칭
	char szInfo[D_MAXLEN_DETAILINFO+1];				///< 소개글

    char szLcdName[D_MAXLEN_CDNAME+1];				///< 지역 대분류명
    char szMcdName[D_MAXLEN_CDNAME+1];				///< 지역 중분류명
    char szScdName[D_MAXLEN_CDNAME+1];				///< 지역 소분류명
    char szDcdName[D_MAXLEN_CDNAME+1];				///< 지역 세분류명
    char szPrimaryBun[D_MAXLEN_CDNAME+1];			///< 본번
    char szSecondaryBun[D_MAXLEN_CDNAME+1];			///< 부번
    int nMlClass;									///< 산/대지 구분자(0:알수없음,1:대지,2:산)
    char szBldAdd[D_MAXLEN_ADDRNAME+1];				///< 건물주소 표출용 주소 Ex)서울 중구 세종대로
    char szRoadName[D_MAXLEN_ROADNAME+1];			///< 도로명
    char szBldNo1[D_MAXLEN_ROADNAME+1];				///< 건물번호1
    char szBldNo2[D_MAXLEN_ROADNAME+1];				///< 건물번호2
    DS_POINT centerTp;								///< 중심점 좌표
    char bRpFlag;									///< RP Flag
    char szZipCd[D_MAXLEN_ZIPCODE+1];				///< 우편번호
    char szMenu1[D_MAXLEN_MENU+1];					///< 메뉴1
    char szMenu2[D_MAXLEN_MENU+1];					///< 메뉴2
    char szMenu3[D_MAXLEN_MENU+1];					///< 메뉴3
    char szMenu4[D_MAXLEN_MENU+1];					///< 메뉴4
    char szMenu5[D_MAXLEN_MENU+1];					///< 메뉴5
    char szTelNo[D_MAXLEN_TELNO+1];					///< 전화번호
    char szRoad[D_MAXLEN_ROAD+1];					///< 찾아가는 길
    int nRecommandPoint;							///< 좋아요 평가 점수
    char szIconType[D_MAXLEN_CATECODE+1];			///< 아이콘 표시용 표출업종코드
    char szJoinStoreYn[D_MAXLEN_JOINYN+1];			///< 가맹점여부정보 세미콜론(;)으로 구분(OKCB:OKCashbag, OKCB;GIFT:OKCashbag,기프티콘, OKCB;TMEM;SSCD)
    char szJoinViewType[D_MAXLEN_JOINVIEWTYPE+1];	///< 가맹점화면 Type 세미콜론(;)으로 구분 가맹점여부정보와 1:1대응(OKCB-> ATYP / TMEM->BTYP / GIFT->XTYP / SSCD->CTYP / )

    // sangbong.woo - s
    char szAddInfo[D_MAXLEN_DETAILINFO+1]; 			///< 추가 소개글
    unsigned int m_GatherTime;                      ///< Data 수집시점/시간
    int  nDataKind;                                 ///< 주유소종류 (3: 주유소, 4:충전소, 6: EV충전소)
    bool bHighGasoline;								///< 고급휘발유판매점여부 (0:판매안함, 1:판매)
    //x bool bLowestOilSt;								///< 최저가점포여부. 출력리스트상 최저가 점포일 경우(최저가:1, 아닌경우:0). SK주유소가 최저가인 경우에만 1로 설정
    unsigned int nOPIGasolinePrice;					///< 휘발유가격(원)
    unsigned int nOPIDieselPrice;					///< 경유가격(원)
    unsigned int nOPILPGPrice;						///< LPG가격(원)
    unsigned int nOPIHiGasolinePrice;				///< 프리미엄 휘발유 가격(원)
    unsigned int nOPIHiLPGPrice;					///< 프리미엄 LPG 가격(원)
    // sangbong.woo - e

	char szDayOffInfo[D_MAXLEN_DAYOFFINFO+1];							///< 휴무정보
	char szBusinessHours[D_MAXLEN_BUSINESSHOURS+1];						///< 영업시간
	char szDayOffDate[D_MAXLEN_DAYOFFDATE+1];							///< 휴무일자

	DS_POI_IMAGE_INFOS sPoiImageInfos[5];			///< POI 이미지 정보
	DS_POI_PARK_INFOS sPoiParkInfos[3];				///< 제휴 주차장 정보
}DS_SEARCH_POI_DETAIL_INFO;

typedef struct tagDSearchPoiStationInfo{
    int nPoiID;										///< POI ID
    char szName[D_MAXLEN_SISULNAME+1];				///< 명칭
    DS_POINT tp;									///< 위치 좌표
    int  nDataKind;                                 ///< 주유소종류 (3: 주유소, 4:충전소, 6: EV충전소)
    char fuelType[D_MAXLEN_FUELTYPE];               ///< 연료타입
    unsigned int nOPIGasolinePrice;					///< 휘발유가격(원)
    unsigned int nOPIDieselPrice;					///< 경유가격(원)
    unsigned int nOPILPGPrice;						///< LPG가격(원)
    unsigned int nOPIHiGasolinePrice;				///< 프리미엄 휘발유 가격(원)
    unsigned int nOPIHiLPGPrice;					///< 프리미엄 LPG 가격(원)
}DS_SEARCH_POI_STATION_INFO;

/// 가맹점 상세정보
typedef struct tagDSearchPoiJoinInfo{
	int nIdx;										///< 검색 결과 인덱스
	int nPoiID;										///< POI ID
	char szJoinType[16];							///< 가맹점 종류 ("OKCB": OK캐시백, "TMEM": T맴버십, "SSCD": 삼성카드)
	char szName[D_MAXLEN_SISULNAME+1];				///< 명칭
	char szCashRate[8];								///< 현금 적립율
	char szCardRate[8];								///< 카드 적립율
	char szWorkTime[16];							///< 평일 영업시간
	char szWeekendWorkTime[16];						///< 주말 영업시간
	char szUrl[D_MAXLEN_URL+1];						///< 웹사이트
	char byPark;									///< 주차 가능 여부(0:불가능, 1:가능, 그외:정보없음)	
	char szBenefit[512];							///< 혜택
	char szUserGuide[512];							///< 이용안내
	char szInformation[512];						///< 업소 소개
	int nItem;										///< 소개글 개수
	char szItemInfos[8][128];						///< 소개글

}DS_SEARCH_POI_JOIN_INFO;

/// POI 최근길 조회
typedef struct tagDSearchRecentDestnation{
	char szName[D_MAXLEN_SISULNAME+1];				///< 명칭
	DS_POINT tp;									///< 위치 좌표
	int nPoiID;										///< POI ID
	char szNavSeq[7];								///< 입구점번호
	unsigned char byRPFlag;							///< RPFlag (경로 탐색시 사용)
	char szSvcDate[D_MAXLEN_SVCDATE+1];		///< 최종 서비스일시 (YYYYMMDDHH24MISS)
}DS_SEARCH_RECENT_DESTNATION;

////////////////////////////////////////////////////////////////////////////////////////
// 즐겨찾기 관련
////////////////////////////////////////////////////////////////////////////////////////
#define D_MAXLEN_SENDTOCARNAME  	100
#define D_MAXLEN_USEREMAIL		  	64
#define D_MAXLEN_USERNAME		  	32
#define D_MAXLEN_USERMDN		  	16
#define D_MAXLEN_USERBIRTH		  	16
#define D_MAXLEN_CARMODEL		  	20
#define D_MAXLEN_CARFUEL		  	10
#define D_MAXLEN_CARHIPASS		  	2
#define D_MAXLEN_CARNUMBER		  	12

/// 즐겨찾기 종류 타입
enum DeFavoriteType
{
	DE_FAVORITETYPE_NONE = 0,				///< 미정의
	DE_FAVORITETYPE_SERVER,				///< NDDS 즐겨찾기 조회
	DE_FAVORITETYPE_REGISTERSENDTOCARINFO,	///< NDDS 차량 전송 정보 저장(NDDS 콜백 lParam2에 성공(1) 및 실패(0) 값 전달됨)
	DE_FAVORITETYPE_FINDSENDTOCARINFO,		///< NDDS 차량 전송 정보 조회
	DE_FAVORITETYPE_REMOVESENDTOCAR,		///< NDDS 차량 전송 정보 삭제(NDDS 콜백 lParam2에 성공(1) 및 실패(0) 값 전달됨)
    DE_FAVORITETYPE_SERVER_REGIST,		///< NDDS 즐겨찾기 등록
    DE_FAVORITETYPE_SERVER_UPLOAD,		///< NDDS 즐겨찾기 전체 업로드
    DE_FAVORITETYPE_SERVER_REMOVE,		///< NDDS 즐겨찾기 멀티 삭제
    DE_FAVORITETYPE_SERVER_MODIFY,		///< NDDS 즐겨찾기 단건 수정
    DE_FAVORITETYPE_USERPROFILE_FIND,	///< NDDS user-profile 조회
    DE_FAVORITETYPE_USERPROFILE_REGIST,	///< NDDS user-profile 저장
    DE_FAVORITETYPE_CARPROFILE_FIND,	///< NDDS car-profile 조회
    DE_FAVORITETYPE_CARPROFILE_REGIST,	///< NDDS car-profile 저장
    DE_FAVORITETYPE_USERDATA_FIND,      ///< NDDS 최근길/즐겨찾기 전체리스트 조회
};

/// 서버 POI 즐겨찾기 요청 데이터
typedef struct tagDFavoriteServerReq
{
    DeFavoriteType eFavoriteType;					///< 즐겨찾기 종류
    bool bMyFavorite;								///< POI 집으로/회사로 조회 여부(“Y:true”/”N:false”). default: “N”
}DS_FAVORITE_SERVER_REQ;


//즐겨찾기 등록 리스트 정보
typedef struct tagDRegistFavoriteList{
    DeFavoriteType eFavoriteType;						///< 즐겨찾기 종류
    char szCustName[D_MAXLEN_SISULNAME+1];			// 명칭
    DS_POINT tp;									// 좌표
    int nPoiID;										// POI ID
    char szNavSeq[7];								// 입구점 일련번호
    char byRPFlag;									// RP Flag
    char szAddInfo[D_MAXLEN_FAVADDINFO+1];			// 부가정보
    char szOrgCustName[D_MAXLEN_SISULNAME+1];		// 최초명칭
    char szIconInfo[D_MAXLEN_FAVADDINFO+1];			// 아이콘 정보
}DS_REGIST_FAVORITE_LIST;

///즐겨찾기 등록 요청 데이터
typedef struct tagDRegistFavoriteReq
{
    DeFavoriteType eFavoriteType;						///< 즐겨찾기 종류
    int nPoiRegistCnt;									// 즐겨찾기 등록 개수
    DS_REGIST_FAVORITE_LIST sRegistFavoriteInfo[D_MAXELN_FAVORITELIST+1];

}DS_FAVORITE_REGIST_REQ;

/// 즐겨찾기 집으로/회사로
typedef struct tagDUploadPoiMyFavorite{
    char		szHomeCustName[D_MAXLEN_SISULNAME+1];			// 집으로 명칭 (고객이 입력한 명칭)
    DS_POINT	Hometp;											// 집으로 좌표(sk 정규화 좌표 7자리)
    int			nHomePoiID;										// 집으로 POI ID
    char		szHomeNavSeq[7];								// 집으로 입구점 일련번호
    char		byHomeRpFlag;									// 집으로 RP FLAG
    char		szHomeAddInfo[D_MAXLEN_FAVADDINFO+1];			// 집으로 부가정보 (행정구역명,좌표)
    char		szOfficeCustName[D_MAXLEN_SISULNAME+1];			// 회사로 명칭 (고객이 입력한 명칭)
    DS_POINT	Officetp;										// 회사로 좌표(sk 정규화 좌표 7자리)
    int			nOfficePoiID;									// 회사로 POI ID
    char		szOfficeNavSeq[7];								// 회사로 입구점 일련번호
    char		byOfficeRpFlag;									// 회사로 RP FLAG
    char		szOfficeAddInfo[D_MAXLEN_FAVADDINFO+1];		// 회사로 부가정보 (행정구역명,좌표)
}DS_UPLOAD_POIMYFAVORITE;

/// POI 즐겨찾기 업로드 리스트 정보(poiFavorite)
typedef struct tagDUploadPoiFavoriteInfo{
    char szCustName[D_MAXLEN_SISULNAME+1];		// 명칭
    DS_POINT tp;								// 좌표
    int nPoiID;									// POI ID
    char byRPFlag;								// RP Flag
    char szAddInfo[D_MAXLEN_FAVADDINFO+1];		// 부가정보
    char szOrgCustName[D_MAXLEN_SISULNAME+1];	// 최초 명칭
    char szIconInfo[D_MAXLEN_SISULNAME+1];		// 아이콘 정보 (/별/, /하트/, /병원/ 등)
    char szInsDatetime[D_MAXLEN_SVCDATE+1];		// 입력일시 (YYYYMMDDHH24MISS)
    char szNavSeq[7];							// 입구점 일련번호
}DS_UPLOAD_POIFAVORITE;

// 즐겨찾기 업로드 리스트
typedef struct tagDUploadFavoriteReq
{
    DeFavoriteType eFavoriteType;						///< 즐겨찾기 종류

    char szUploadOption[7];								///< "FI":즐겨찾기 업로드 / "FI_MC":POI즐겨찾기업로드 +집으로/회사로 초기화 / "FI_MS":POI즐겨찾기업로드 +집으로/회사로 설정 / NULL인 경우 기본 FI
    bool bMyFavorite;									//집으로/회사로 정보 유무
    DS_UPLOAD_POIMYFAVORITE	sUploadpoiMyFavorite;		///< 집으로/회사로 정보
    int nUploadPoiCnt;
    DS_UPLOAD_POIFAVORITE	sUploadFavoriteInfo[D_MAXELN_FAVORITELIST+1];///< POI 즐겨찾기 등록 리스트
}DS_FAVORITE_UPLOAD_REQ;


/// POI 즐겨찾기 삭제 정보(poiFavorite)
typedef struct tagDRemovePoiFavoriteInfo{
    char szCustName[D_MAXLEN_SISULNAME+1];		// 명칭
    DS_POINT tp;								// 좌표
}DS_REMOVE_POIFAVORITE;

// 즐겨찾기 멀티 삭제 리스트
typedef struct tagDRemoveFavoriteReq
{
    DeFavoriteType eFavoriteType;						///< 즐겨찾기 종류
    int nRemovePoiRecentCnt;
    DS_REMOVE_POIFAVORITE	sRemoveFavoriteInfo[D_MAXELN_FAVORITELIST+1];///< POI 즐겨찾기 등록 리스트
}DS_FAVORITE_REMOVE_REQ;

// POI 즐겨찾기 단건 수정 정보(poiFavorite)
typedef struct tagDModifyPoiFavoriteInfo{
    DeFavoriteType eFavoriteType;						///< 즐겨찾기 종류
    char szCustName[D_MAXLEN_SISULNAME+1];		// 명칭
    DS_POINT tp;								// 좌표
    char szNewCustName[D_MAXLEN_SISULNAME+1];	// 변경 후 명칭
    char szIconInfo[D_MAXLEN_FAVADDINFO+1];		// 아이콘 정보
}DS_MODIFY_POIFAVORITE;

// 즐겨찾기 단건 수정 리스트
typedef struct tagDModifyFavoriteReq
{
    DeFavoriteType eFavoriteType;						///< 즐겨찾기 종류
    DS_MODIFY_POIFAVORITE	sModifyFavoriteInfo;		///< POI 즐겨찾기 단건 수정
}DS_FAVORITE_MODIFY_REQ;


/// user-profile 조회 요청 데이터
typedef struct tagDFavoriteFindUserProfileReq
{
    DeFavoriteType eFavoriteType;						///< 즐겨찾기 종류
}DS_FAVORITE_FINDUSERPROFILE_REQ;

/// user-profile 저장 요청 데이터
typedef struct tagDFavoriteRegistUserProfileReq
{
    DeFavoriteType eFavoriteType;						///< 즐겨찾기 종류
    char szEmail[D_MAXLEN_USEREMAIL];					///< 이메일
    char szUserName[D_MAXLEN_USERNAME];					///< 이름
    char szMdn[D_MAXLEN_USERMDN];						///< 핸드폰 번호
    char szBirth[D_MAXLEN_USERBIRTH];					///< 생년월일(YYYYMMDD)
}DS_FAVORITE_REGISTUSERPROFILE_REQ;


/// car-profile 조회 요청 데이터
typedef struct tagDFavoriteFindCarProfileReq
{
    DeFavoriteType eFavoriteType;						///< 즐겨찾기 종류
}DS_FAVORITE_FINDCARPROFILE_REQ;

/// car-profile 저장 요청 데이터
typedef struct tagDFavoriteRegistCarProfileReq
{
    DeFavoriteType eFavoriteType;						///< 즐겨찾기 종류
    char szModel[D_MAXLEN_CARMODEL];					///< 차종(경차:CT_COMPACT, 승용차:CT_NORMAL, 중형승합차:CT_MIDDLE, 대형승합차:CT_LARGE, 대형화물차:CT_TRUCK, 특수화물차:CT_SPECIAL)
    char szFuel[D_MAXLEN_CARFUEL];						///< 유종(휘발유:FT_GAS, 경유:FT_DIESEL, LPG:FT_LPG, 고급휘발유:FT_GAS)
    char szHipassYn[D_MAXLEN_CARHIPASS];				///< 하이패스차량 여부 "Y","N"
    char szNumber[D_MAXLEN_CARNUMBER];					///< 차량번호 예)03마1004
}DS_FAVORITE_REGISTCARPROFILE_REQ;

/// 최근길/즐겨찾기 전체리스트 조회 요청 데이터
typedef struct tagDFavoriteFindUserDataReq
{
    DeFavoriteType eFavoriteType;						///< 즐겨찾기 종류
}DS_FAVORITE_FINDUSERDATA_REQ;

/// 서버 차량 전송 정보 저장 요청 데이터
typedef struct tagDFavoriteRegisterSendToCarInfoReq
{
	DeFavoriteType eFavoriteType;						///< 즐겨찾기 종류

	char szCustName[D_MAXLEN_SENDTOCARNAME+1];		///< POI 명칭(최대100byte)
	DS_POINT tp;									///< 위치 좌표
	int nPoiID;										///< POI ID
	unsigned char byRPFlag;							///< RP Flag
	char szNavSeq[7];								///< 입구점 일련번호
}DS_FAVORITE_REGISTERSENDTOCARINFO_REQ;

/// 서버차량 전송 정보 조회 요청 데이터
typedef struct tagDFavoriteFindSendToCarInfoReq
{
	DeFavoriteType eFavoriteType;						///< 즐겨찾기 종류

	bool	bNewAddr;								///< POI주소를 새주소로 표출할것인지 구주소로 표출할것인지 구분(구주소:false, 새주소:true)
}DS_FAVORITE_FINDSENDTOCARINFO_REQ;

/// 서버 차량 전송 정보 삭제 요청 데이터
typedef struct tagDFavoriteRemoveSendToCarInfoReq
{
	DeFavoriteType eFavoriteType;						///< 즐겨찾기 종류

	char szCustName[D_MAXLEN_SENDTOCARNAME+1];		///< POI 명칭(최대100byte)
	DS_POINT tp;									///< 위치 좌표
}DS_FAVORITE_REMOVESENDTOCARINFO_REQ;

/// 즐겨찾기 요청 (공통)
typedef union tagDFavoriteReq
{
	DeFavoriteType eFavoriteType;						///< 즐겨찾기 종류

	DS_FAVORITE_SERVER_REQ sFavoriteServer;				///< NDDS POI 검색
    DS_FAVORITE_REGIST_REQ sRegistFavoriteServer;		///< NDDS POI 등록
    DS_FAVORITE_UPLOAD_REQ sUploadFavoriteServer;		///< NDDS POI 전체 업로드
    DS_FAVORITE_REMOVE_REQ sRemoveFavoriteServer;		///< NDDS POI 멀티 삭제
    DS_FAVORITE_MODIFY_REQ sModifyFavoriteServer;		///< NDDS POI 단건 수정

	DS_FAVORITE_REGISTERSENDTOCARINFO_REQ sFavoriteRegistSendToCarInfo;		///< NDDS 차량 전송 정보 저장
	DS_FAVORITE_FINDSENDTOCARINFO_REQ sFavoriteFindSendToCarInfo;			///< NDDS 차량 전송 정보 조회
	DS_FAVORITE_REMOVESENDTOCARINFO_REQ sFavoriteRemoveSendToCarInfo;		///< NDDS 차량 전송 정보 삭제

    DS_FAVORITE_FINDUSERPROFILE_REQ sFavoriteFindUserProfile;				///< NDDS user-profile 정보 조회
    DS_FAVORITE_REGISTUSERPROFILE_REQ sFavoriteRegistUserProfile;			///< NDDS user-profile 정보 저장
    DS_FAVORITE_FINDCARPROFILE_REQ sFavoriteFindCarProfile;					///< NDDS car-profile 정보 조회
    DS_FAVORITE_REGISTCARPROFILE_REQ sFavoriteRegistCarProfile;				///< NDDS car-profile 정보 저장
    DS_FAVORITE_FINDCARPROFILE_REQ	sFavoriteFindUserData;					///< NDDS USerData 정보 조회
}DS_FAVORITE_REQ;

/// 즐겨찾기 집으로/회사로
typedef struct tagDMyFavorite{
	char	szHomeCustName[D_MAXLEN_SISULNAME+1];			// 집으로 명칭 (고객이 입력한 명칭)
	DS_POINT Hometp;	
	int		nHomePoiID;			// 집으로 POI ID
	char	szHomeNavSeq[7];			// 집으로 입구점 일련번호
	char	byHomeRpFlag;			// 집으로 RP FLAG
	char	szHomeAddInfo[D_MAXLEN_FAVADDINFO+1];			// 집으로 부가정보 (행정구역명,좌표)
	char	szHomeInsDatetime[D_MAXLEN_SVCDATE+1];		// 집으로 입력일시 (YYYYMMDDHH24MISS)
	char	szOfficeCustName[D_MAXLEN_SISULNAME+1];		// 회사로 명칭 (고객이 입력한 명칭)
	DS_POINT Officetp;
	int		nOfficePoiID;			// 회사로 POI ID
	char	szOfficeNavSeq[7];			// 회사로 입구점 일련번호
	char	byOfficeRpFlag;			// 회사로 RP FLAG
	char	szOfficeAddInfo[D_MAXLEN_FAVADDINFO+1];		// 회사로 부가정보 (행정구역명,좌표)
	char	szOfficeInsDatetime[D_MAXLEN_SVCDATE+1];	// 회사로 입력일시 (YYYYMMDDHH24MISS)
}DS_MYFAVORITE;

/// 즐겨찾기 리스트
typedef struct tagDFavoriteList{
	int		nIdx;									// 검색 결과 인덱스
	char	szCustName[D_MAXLEN_SISULNAME+1];				///< 명칭
	DS_POINT tp;									///< 위치 좌표
	int		nPoiID;										///< POI ID
	char	szNavSeq[7];							///< 입구점번호
	char szLcdName[D_MAXLEN_CDNAME+1];				///< 지역 대분류명
	char szMcdName[D_MAXLEN_CDNAME+1];				///< 지역 중분류명
	char szScdName[D_MAXLEN_CDNAME+1];				///< 지역 소분류명
	char szDcdName[D_MAXLEN_CDNAME+1];				///< 지역 세분류명
	char szPrimaryBun[D_MAXLEN_CDNAME+1];			///< 본번
	char szSecondaryBun[D_MAXLEN_CDNAME+1];			///< 부번
	int nMlClass;									///< 산/대지 구분자(0:알수없음,1:대지,2:산)
	char szRoadName[D_MAXLEN_ROADNAME+1];			///< 도로명
	char szBldNo1[D_MAXLEN_ROADNAME+1];				///< 건물번호1
	char szBldNo2[D_MAXLEN_ROADNAME+1];				///< 건물번호2
	char szRepClsName[D_MAXLEN_CLSNAME+1];			///< 대표업종명 (퀵인포에 들어갈 항목)
	char szClsAName[D_MAXLEN_CLSNAME+1];			///< 업종 대분류명
	char szClsBName[D_MAXLEN_CLSNAME+1];			///< 업종 중분류명
	char szClsCName[D_MAXLEN_CLSNAME+1];			///< 업종 소분류명
	char szClsDName[D_MAXLEN_CLSNAME+1];			///< 업종 세분류명
	char byRPFlag;									///< RP Flag
	char szTelNo[D_MAXLEN_TELNO+1];					///< 전화번호
	char szAddInfo[D_MAXLEN_ADDINFO+1];			///< 부가정보
	char szOrgCustName[D_MAXLEN_SISULNAME+1];		///< 최초명칭
	char szIncoInfo[D_MAXLEN_SISULNAME+1];		///< 아이콘 정보
	char szInsDatetime[D_MAXLEN_SVCDATE+1];				///< 입력일시
}DS_FAVORITE_LIST;

/// 즐겨찾기 등록 리스트 정보(poiFavorite)
typedef struct tagDPoiFavoriteInfo{
    char szCustName[D_MAXLEN_SISULNAME+1];		// 명칭
    DS_POINT tp;								// 좌표
    int nPoiID;									// POI ID
    char szNavSeq[7];							// 입구점 일련번호
    char byRPFlag;								// RP Flag
    char szAddInfo[D_MAXLEN_FAVADDINFO+1];		// 부가정보
    char szOrgCustName[D_MAXLEN_SISULNAME+1];	// 최초 명칭
    char szIconInfo[D_MAXLEN_SISULNAME+1];	// 아이콘 정보 (/별/, /하트/, /병원/ 등)
}DS_POIFAVORITE_INFO;

/// UserData 결과 리스트
typedef struct tagDUserInfo{
	DS_POI_MYFAVORITE	sPoiMyFavorite;		///< POI 집으로/회사로(1건)
	DS_ROTUE_MYFAVORITE sRouteMyFavorite;	///< 자주가는 경로(1건)
	DS_ROUTE_RECENT		sRouteRecent[D_MAXLEN_ROUTERECENT];		///< 최근길 리스트(최대 100건)
	DS_FAVORITE_LIST	sPoiFavorite[D_MAXLEN_POIFAVORITE];		///< POI 즐겨찾기 리스트(최대 100건)
	DS_ROUTE_FAVORITE	sRouteFavorite[D_MAXLEN_ROUTEFAVORITE];	///< 경로 즐겨찾기 리스트(최대 100건)
}DS_USER_INFO;

/// ev충전소 상세정보 결과
typedef struct tagDEvStationInfo{
    int poiId;									// Poi Id
    char szStationName[D_MAXLEN_SISULNAME+1];	// 충전소명
    char szAddr[D_MAXLEN_ADDRNAME+1];			// 주소
    char szRoadAddr[D_MAXLEN_ROADADDR+1];       // 도로명주소
    char szAddrDetail[D_MAXLEN_ADDRDETAIL+1];   // 상세주소
    char szMngName[D_MAXLEN_MNGNAME+1];         // 운영기관
    char szMngId[D_MAXLEN_MNGID+1];             // 기관관리 ID
    char szTel[D_MAXLEN_PHONENUMBER+1];			// 연락처
    bool bFreeYn;								// 주차비무료여부
    bool bReservYn;								// 예약가능여부
    char szUseTime[D_MAXLEN_USETIME+1];			// 이용가능시간
    bool bPayYn;                                // 유료여부
    char szFee[D_MAXLEN_FEE+1];                 // 단가
    char szUpdateDt[D_MAXLEN_USETIME+1];						// 충전상태수집일시(YYYYMMDD HH24:MI:SS)
    DS_POINT tp;								// 좌표
    int	nTotalCnt;						// 충전기 개수
    DS_POI_EVSTATION_CHARGERINFO chargerInfo[D_MAXLEN_EVCHARGER];	//충전기리스트
}DS_EVSTATION_INFO;

/// 차량 전송 정보
typedef struct tagDSendToCarInfo{
	char szCustName[D_MAXLEN_SENDTOCARNAME+1];		///< 명칭
	DS_POINT tp;	
	int nPoiID;										///< POI ID
	char szNavSeq[7];								///< 입구점 일련번호
	char byRPFlag;									///< RP Flag
	char szAddrName[D_MAXLEN_ADDRNAME+1];			///< 구주소, 새주소 (새주소 요청시 DB에 새주소 정보가 없으면 구까지만 전달)
	char szPhoneNumber[D_MAXLEN_PHONENUMBER+1];		///< 전화번호
	char szInsDatetime[D_MAXLEN_SVCDATE+1];			///< 입력일시 (YYYYMMDDHH24MISS)
}DS_SENDTOCARINFO;

typedef struct tagDVmsGuideInfo{
	char szBaseSdttm[13];							///< 안전도우미 정보의 기준 일시(YYYYMMDDhhmm)
	char szDataSource[50];							///< 데이터 출처
	char szContentCode[4];							///< 안전도우미 분류코드
	char szContentName[30];							///< 안전도우미 분류명
	char szContent[128];							///< 안전도우미 내용
}DS_VMSGUIDEINFO;

typedef struct tagDAuthExternalUserInfos
{
	char szProvider[16];								///< 외부 인증 타입
	char szReadableUserId[32];						///< 사용자가 사용하는 ID
}DS_AUTH_EXTERNALUSERINFOS;

typedef struct tagDAuthAuthInfo
{
	int nValidateCode;								///< 사용자 확인 결과(성공:2000, 실패:3001(사용자정보없음), 3002(사용자정보변경), 4001(authCode 확인 실패), 9001(black list model no 사용자))
	char szValidationSubField[32];					///< 사용자 확인 실패 사유(3002일때 사용, "DEVICE_ID":Device ID변경, "CARRIER":Carrier 변경)
	int nExternalUserInfoCount;						///< 외부 인증 정보 개수
	DS_AUTH_EXTERNALUSERINFOS sExternalUserInfos[6];	///< 외부 인증 정보
}DS_AUTH_AUTHINFO;

typedef struct tagDHeadUnitDeviceAuthInfo
{
	int nResultCode;								///< 기기인증 여부(성공:2000, 실패:3000,3001(사용불가, 인증실패))
	int nResultSubField;							///< 기기인증 실패 사유(resultCode가 3001일때 사용), “101” : 사용 기간 만료,“102” : 등록 기간 만료,“103” : 등록 가능한 단말 건수 초과,“201” : 미등록 정보 오류 (deviceModelName or deviceModelNo or vendor),“202” : 미등록 정보 오류 (deviceID),“301” : 사용중지 deviceID,“302” : 사용중지 serialKey
}DS_AUTH_HEADUNITDEVICEAUTHINFO;

typedef struct tagDVersionInfo
{
	char szMenuNoticeVer[16];						///< 메뉴 공지사항 버전(YYYYMMDDI, I는 인덱스), ex)20121024A
	char szOpenappVer[16];							///< 오픈 앱 버전(YYYYMMDDI, I는 인덱스), ex)20121024A
}DS_AUTH_VERSIONINFO;

typedef struct tagDAppUpdateInfo
{
	char szAppUpdateType[4];						///< 앱 업데이트 타입(“”, “0” : 해당없음, “1” : 권고,“2” : 강제)
	char szAppVer[16];								///< 앱 버전(00.00.00), ex)2.0.0 또는 4.12.4(appUpdateType 이 “1”, “2” 일 경우만 의미 있음)
	char szAppUpdateViewFlag[4];					///< 앱 업데이트 권고 표출 flag(“0”: 해당 없음, “1” : 항상, “2” : 일 1회) (appUpdateType 이 “권고”일 때만 의미 있음)
	char szAppUpdateUrl[512];						///< 앱 최신 앱버전 다운로드 url(appUpdateType 이 “1”, “2” 일 경우만 의미 있음)
	char szAppUpdateMsg[512];						///< 앱 제/선택 업그레이드 설정 문구(appUpdateType 이 “1”, “2” 일 경우만 의미 있음)
}DS_AUTH_APPUPDATEINFO;

typedef struct tagDControlInfoDetails
{
	char szServiceName[32];							///< 제어 서비스 명
	char szServiceYn[4];							///< 사용여부. (“Y”, “N”). * 로그일 경우 수집여부 * 재탐색일 경우 동작 여부 (N 이면 단말 Default 동작이며, Y 인 경우 동작이 override) *  HPS Rf Data 수집일 경우 수집여부
	char szServiceItems[512];						///< 서비스 항목
}DS_AUTH_CONTROLINFODETAILS;

typedef struct tagDAppControlInfo
{
	int nControlInfoCount;							///< 앱 제어정보 개수
	DS_AUTH_CONTROLINFODETAILS sControlInfoDetails[6]; ///< 앱 제어정보 리스트
}DS_AUTH_APPCONTROLINFO;

typedef struct tagDCommonInfo
{
	char szEuk[256];								///< 암호화 된 UserKey. Usekey가 없는 경우(인증실패 시 포함) null
	char szEid[256];								///< 암호화 된 UserId. UserId 가 없는 경우(인증실패 시 포함) null
}DS_AUTH_COMMONINFO;

typedef struct tagDTidAuthResInfo
{
	int nResultCode;								///< 사용자 확인 결과 코드
	int nResultSubField;							///< 실패사유(nResultCode가 4001일 때 사용)
	int nResultMdnAuthInfo;							///< 요청시 MDN+DeviceID+Carrier 입력시 유효
	int nExternalUserInfoCount;						///< 외부 인증 정보 개수
	DS_AUTH_EXTERNALUSERINFOS sExternalUserInfos[6];	///< 외부 인증 정보 리스트
}DS_AUTH_TIDAUTHRESINFO;

typedef struct tagDPushInfo
{
	char szPushSetYn[4];							///< PUSH 설정 여부(Y/N)
	char szSmsYn[4];								///< SMS 수신 여부(Y/N)
	char szMomentYn[4];								///< 모멘티 사용 여부(default ‘Y’)
}DS_AUTH_PUSHINFO;

typedef struct tagDPersonalInfo
{
	char szEmail[64];								///< 이메일
	char szUserName[32];							///< 이름
	char szMdn[16];									///< 핸드폰 번호
	char szBirth[16];								///< 생년월일(YYYYMMDD)
}DS_AUTH_PERSIONINFO;

typedef struct tagDCarInfo
{
	char szModel[16];								///< 차종
	char szFuel[12];								///< 유종
	char szHipassYn[16];							///< 하이패스차량 여부 Y,N
	char szNumber[16];								///< 차량번호
}DS_AUTH_CARINFO;

typedef struct tagDUserProfileInfo
{
	DS_AUTH_PERSIONINFO sPersonalInfo;				///< 개인 정보 데이터 없을 경우 null
	DS_AUTH_CARINFO sCarInfo;						///< 차량 정보 데이터 없을 경우 null
}DS_AUTH_USERPROFILEINFO;

typedef struct tagDAuthResult{
	char szErrorCode[7];							///< 인증결과 에러코드(Tmap에서 탈퇴 또는 로그아웃시:201501, AccessKey없이 개인정보 조회시:010105)
	char szAccessKey[50];							///< AccessKey					
	DS_AUTH_COMMONINFO sCommInfo;					///< 공통 응답 정보
	DS_AUTH_AUTHINFO sAuthInfo;						///< 사용자 인증 결과
	DS_AUTH_HEADUNITDEVICEAUTHINFO sHeadUnitDeviceAuthInfo;	///< 기기(HeadUnit)의 인증 결과
	DS_AUTH_TIDAUTHRESINFO sTidAuthResInfo;			///< T아이디 인증 응답 정보(T아이디로 로그인 할 경우)
	DS_AUTH_VERSIONINFO sVersionInfo;				///< 각종 버전 정보
	DS_AUTH_APPUPDATEINFO sAppUpdateInfo;			///< APP 업데이트 정보
	DS_AUTH_APPCONTROLINFO sAppControlInfo;			///< APP 제어 정보
	DS_AUTH_PUSHINFO sPushInfo;						///< Push 설정 정보(요청 공통헤더의 pushDeviceKey가 존재하고 인증이 성공할 경우 응답데이터 설정 그외 null)
	DS_AUTH_USERPROFILEINFO sUserProfileInfo;		///< 사용자 정보
}DS_AUTHRESULT;

#pragma pack()

#endif // __NDDS_DEFINE_H__
