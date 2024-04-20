// common_def.h

#ifndef __COMMON_DEF_H__
#define __COMMON_DEF_H__


//////////////////////////////////////////////////////////////////////////

/// 시설물 이름 길이 제한
#define MAXLEN_POINAME      160
/// 최대 주소 글자수
#define MAXLEN_ADDRNAME		80

/// 지역명 글자수
#define MAXLEN_CDNAME		20
/// 도로명 글자수
#define MAXLEN_ROADNAME		20
/// 우편번호글자수,
#define MAXLEN_ZIPCODE		10
/// 메뉴글자
#define MAXLEN_MENU			10
/// 전화번호글자수
#define MAXLEN_TELNO		20
/// 찾아가는 길 글자수
#define MAXLEN_ROAD			300
/// 표출업종코드 글자수
#define MAXLEN_CATECODE		10
/// 가맹점여부정보 글자수
#define MAXLEN_JOINYN		20
/// 가맹점 화면타입글자수
#define MAXLEN_JOINVIEWTYPE	20
///
#define MAXLEN_BIDCODE		10
/// 메뉴글자
#define MAXLEN_DAYOFFINFO	256
/// 메뉴글자
#define MAXLEN_BUSINESSHOURS 128
/// 메뉴글자
#define MAXLEN_DAYOFFDATE	256

/// 최대 전화번호 글자수
#define MAXLEN_PHONENUMBER	14
#define MAXLEN_URL			256		// 웹사이트 글자수
#define MAXLEN_DETAILINFO		3000	// 상세정보 글자수
#define MAX_POIIMAGEINFO	5
#define MAX_POIPARKINFO		3
#define MAXLEN_DATAKIND  2
#define MAXLEN_SVCDATE  16
#define MAXLEN_DATAKIND  2
#define MAXLEN_FUELTYPE 10
#define MAXLEN_FAVADDINFO	500		// 즐겨찾기 부가 정보
#define MAX_RESULTLIST	8		// 리스트 최대 개수
#define MAXLEN_TITLE		30		///< 경로 제목 글자수
#define MAXLEN_CATENAME		20
#define MAXLEN_ID			10
#define MAXLEN_EVSTATION_CHARGERS	25

#define MAXLEN_YNTYPE		10
#define MAXLEN_USETIME		20
#define MAXLEN_ROADADDR     20		// 전기차 충전소 도로명주소
#define MAXLEN_ADDRDETAIL   30		// 전기차 충전소 상세주소
#define MAXLEN_MNGNAME      20		// 전기차 충전소 운영기관
#define MAXLEN_MNGID        10		// 전기차 충전소 기관ID
#define MAXLEN_FEE          10		// 전기차 충전 단가

#define MAXLEN_JOININFO		512	// 가맹점 상세정보 글자수
#define MAX_POIJOININFO	8

#define MAXLEN_SENDTOCARNAME 100
#define MAXLEN_USEREMAIL	64
#define MAXLEN_USERNAME		32
#define MAXLEN_USERMDN		16
#define MAXLEN_USERBIRTH	16
#define MAXLEN_CARMODEL		20
#define MAXLEN_CARFUEL		10
#define MAXLEN_CARHIPASS	2
#define MAXLEN_CARNUMBER	12

#define MAXLEN_POIRECENT	100
#define MAXLEN_POIFAVORITE	100
#define MAXLEN_ROUTEFAVORITE	100
#define MAXLEN_EVSTATIONBYPOINT	200

#define SEARCH_WAITTING_TIME 10000
#define RP_WAITTING_TIME 10000

enum NeNddsSearchErrCode
{
	//SEARCHERR_SUCCED =			1,		// 검색 요청 성공 
	//SEARCHERR_FAIL=				0,		// 검색 요청 실패
	N_SEARCHERR_DUPLICATION=  	-100,	// 검색 중복 요청
};

// Search 형태
enum NeNddsSearchSort
{
	N_SORTTYPE_NONE = 0,				// 정렬 안함
	N_SORTTYPE_NAME,					// 이름순 정렬
	N_SORTTYPE_DIST,					// 거리순 정렬
	N_SORTTYPE_PRICE,					// 가격 (유가정보 일때만 의미 있음)
	N_SORTTYPE_ACCURACY,				// 정화도순 정렬(서버 일때만 의미 있음)
};

// Search 형태
enum NeNddsSearchType
{
	N_SEARCHTYPE_NONE = 0,
	N_SEARCHTYPE_NDDSPOI,				// NDDS POI 검색
	N_SEARCHTYPE_NDDSAROUNDPOI,			// NDDS POI 검색 5.2.2. 주변지 검색
	N_SEARCHTYPE_NDDSPOIDETAILINFO,		// NDDS POI 검색 5.2.3. POI 상세정보 검색
	N_SEARCHTYPE_NDDSPOIJOININFO,		// NDDS POI 검색 5.2.4. 가맹점 상세정보 검색
	N_SEARCHTYPE_NDDSRECENTDESTNATION,	// NDDS POI 검색 5.2.5. POI 최근길 조회
    N_SEARCHTYPE_NDDSUPLOADRECENTDESTNATION,	// NDDS POI 검색 5.2.6. POI 최근길 전체 업로드
    N_SEARCHTYPE_NDDSREMOVERECENTDESTNATION,	// NDDS POI 검색 5.2.7. POI 최근길 멀티 삭제
	N_SEARCHTYPE_NDDSAREANAMESBYSTEP,	// NDDS POI 검색 5.2.11. 지역분류별 주소명 조회
    N_SEARCHTYPE_NDDSPOISTATIONINFO,	// NDDS POI 검색 5.2.12. 최저가 주유소(충전소) 조회
    N_SEARCHTYPE_NDDSPOIEVSTATIONINFO,	// NDDS POI 검색 5.2.16. 전기차 충전소 ePoi 상세 조회
    N_SEARCHTYPE_NDDSPOIEVSTATIONSBYPOINT,	// NDDS POI 검색 5.2.18. 주변 전기차 충전소 조회
};

// 자동차 유류 종류
typedef enum
{
	N_eOilTypeGasoline = 0,	// 휘발유
	N_eOilTypeDiesel,			// 경유
	N_eOilTypeLPG,			// LPG
	N_eOilTypeEV,			// EV
}NeNddsOilType;

// 주유소/충전소 폴 종류
typedef enum
{
	N_eOilCompanyType_SK = 0,	// SK에너지
	N_eOilCompanyType_SOIL,	// 에스오일
	N_eOilCompanyType_GS,	// GS칼텍스
	N_eOilCompanyType_HYUNDAE,	// 현대오일뱅크
	N_eOilCompanyType_ETC,	// 기타
}NeNddsOilCompany;

#define N_ROUTE_RQ_ADDCAMERA		9

// Route Return Value

enum NeNddsRouteErrCode
{
	N_ROUTEERR_SUCCED =			1,		// ROUTE 성공 또는
	N_ROUTEERR_FAIL=				0,		// ROUTE 실패
	N_ROUTEERR_STARTPOS=				-100,// 출발지 이상.
	N_ROUTEERR_STARTPOS2,					// 출발지 이상.
	N_ROUTEERR_GOALPOS,					// 목적지 이상.
	N_ROUTEERR_COORDINATE_ERROR,		// 출발지,경유지,목적지 좌표 오류
	N_ROUTEERR_SAMEPOINT,					// 출발지,경유지,목적지 동일 오류
	N_ROUTEERR_DISTOVER,					// 경로최대거리 설정 오류
	N_ROUTEERR_USERCANCEL,				// 사용자 취소.
	N_ROUTEERR_SERVER,					// 서버 통신 실패
	N_ROUTEERR_TIMEOUT,					// 서버 통신 시간초과
	N_ROUTEERR_DUPLICATION,				// 경로 탐색 중복 요청
};


//////////////////////////////////////////////////////////////////////////
//경로탐색 종류(다중선택 가능)

enum NeNddsMultiRoute
{
	// 	ROUTEOPTION_STATIC_OPTIMUM			= 0x00000000,	// 정적 추천 유료+무료	(0)
	// 	ROUTEOPTION_STATIC_OPTIMUM_FREE		= 0x00000001,	// 정적 추천 무료 우선	(1)
	// 	ROUTEOPTION_STATIC_SHORT			= 0x00000002,	// 정적 최단 유료+무료	(2)
	// 	ROUTEOPTION_STATIC_SHORT_FREE		= 0x00000003,	// 정적 최단 무료 우선	(3)
	//
	// 	ROUTEOPTION_PATTERN_OPTIMUM			= 0x00000004,	// 패턴 추천 유료+무료	(4)
	// 	ROUTEOPTION_PATTERN_OPTIMUM_FREE	= 0x00000005,	// 패턴 추천 무료 우선	(5)
	// 	ROUTEOPTION_PATTERN_SHORT			= 0x00000006,	// 패턴 최단 유료+무료	(6)
	// 	ROUTEOPTION_PATTERN_SHORT_FREE		= 0x00000007,	// 패턴 최단 무료 우선	(7)
	//
	// 	ROUTEOPTION_TPEG_OPTIMUM			= 0x00000008,	// TPEG 추천 유료+무료	(8 : default Option)
	// 	ROUTEOPTION_TPEG_OPTIMUM_FREE		= 0x00000009,	// TPEG 추천 무료 우선	(9)
	// 	ROUTEOPTION_TPEG_SHORT				= 0x0000000a,	// TPEG 최단 유료+무료	(10)
	// 	ROUTEOPTION_TPEG_SHORT_FREE			= 0x0000000b,	// TPEG 최단 무료 우선	(11)

	//기본 경로 옵션(최대 2개 선택)
	N_MULTIROUTE_BASIC_OPTIMUM			= 0x00000001,	//최적(추천)
	N_MULTIROUTE_BASIC_SHORTTIME			= 0x00000002,	//전문가(최소시간)
	N_MULTIROUTE_BASIC_EASY				= 0x00000004,	//초보자(직진/대로우선)
	N_MULTIROUTE_BASIC_EXPRESSWAY			= 0x00000008,	//고속도로우선
	N_MULTIROUTE_BASIC_SHORTESTWAY		= 0x00000020,	//최단
	N_MULTIROUTE_BASIC_DETAIL				= 0x00000040,	//고급 경로 // 4.0추가

	//요금 가중치 옵션(최대 2개 선택)
	N_MULTIROUTE_FEE_NORMAL				= 0x00000100,	//요금 가중치 미적용
	N_MULTIROUTE_FEE_MINIMAL				= 0x00000200,	//최소 요금(무료)
	N_MULTIROUTE_FEE_OPTIMUM				= 0x00000400,	//최적 요금
	N_MULTIROUTE_FEE_DETAIL				= 0x00000800,	//고급 요금 // 4.0추가

	//	N_MULTIROUTE_FEE_FREE					= 0x00000800,	//무료 요금

	//교통정보 옵션(택 1)
	N_MULTIROUTE_TRAFFIC_STATIC			= 0x00010000,	//TPEG 미적용
	N_MULTIROUTE_TRAFFIC_PATTERN			= 0x00020000,	//패턴 적용
	N_MULTIROUTE_TRAFFIC_TPEG				= 0x00040000,	//TPEG 적용
	N_MULTIROUTE_TRAFFIC_SERVER			= 0x00080000,	//SERVER 탐색

	//필터
	N_MULTIROUTE_BASIC_FILTER				= 0x000000ff,	//멀티라우팅 기본 옵션 얻을수 있는 필터
	N_MULTIROUTE_FEE_FILTER				= 0x0000ff00,	//멀티라우팅 요금 옵션 얻을수 있는 필터
	N_MULTIROUTE_TRAFFIC_FILTER			= 0x000f0000,	//멀티라우팅 교통정보 옵션 얻을수 있는 필터
	N_MULTIROUTE_NUMBER					= 4,			//개수
};

//경로 탐색 형태
enum NeNddsRouteType
{
	N_ROUTETYPE_NORMAL = 1,			//일반탐색
	N_ROUTETYPE_DETOUR,				//회피탐색
	N_ROUTETYPE_ADD,					//경로추가
	N_ROUTETYPE_RE_OPTION,			//옵션변경(재탐색 : 자동경로선택됨)
	N_ROUTETYPE_RE_OUTROUTE,			//경로이탈(재탐색 : 자동경로선택됨)
	N_ROUTETYPE_TPEG_RE_ROUTE,		//주기적재탐색(재탐색 : 자동경로선택됨)
	N_ROUTETYPE_COUNTERPLAN,			//대안 경로탐색(탐색후 기존경로와 비교하여 다르면 후보 경로로 유지하고 분기점에서 하나의 경로를 선택한다.) - 교통정보 재탐색
	N_ROUTETYPE_PATTERN_ARRIVAL,		//패턴 예측 경로탐색 도착시간안내
	N_ROUTETYPE_PATTERN_DEPARTURE,	//패턴 예측 경로탐색 출발시간안내
	N_ROUTETYPE_SUMMARY,			//경로 요약
};

// Favorite 형태
enum NeNddsFavoriteType
{
	N_FAVORITETYPE_NONE = 0,
	N_FAVORITETYPE_NDDSPOI,				// NDDS FAVORITE 조회
    N_FAVORITETYPE_NDDSREGISTPOI,		// NDDS FAVORITE 등록
    N_FAVORITETYPE_NDDSUPLOADPOI,		// NDDS FAVORITE 전체 업로드
    N_FAVORITETYPE_NDDSREMOVEPOI,		// NDDS FAVORITE 멀티삭제
    N_FAVORITETYPE_NDDSMODIFYPOI,		// NDDS FAVORITE 단건 수정
    N_FAVORITETYPE_NDDSREGISTERSENDTOCARINFO,	// NDDS 차량 전송 정보 저장
	N_FAVORITETYPE_NDDSFINDSENDTOCARINFO,				// NDDS 차량 전송 정보 조회
	N_FAVORITETYPE_NDDSREMOVESENDTOCARINFO,				// NDDS 차량 전송 정보 삭제
    N_FAVORITETYPE_NDDSFINDUSERPROFILE,					// NDDS user-profile 조회
    N_FAVORITETYPE_NDDSREGISTUSERPROFILE,				// NDDS user-profile 저장
    N_FAVORITETYPE_NDDSFINDCARPROFILE,					// NDDS car-profile 조회
    N_FAVORITETYPE_NDDSREGISTCARPROFILE,				// NDDS car-profile 저장
    N_FAVORITETYPE_NDDSFINDUSERDATA,					// NDDS 최근길/즐겨찾기 전체리스트 조회
};

// NonNdds Error-code
enum NeNddsNonNddsErrCode
{
    N_NONNDDSERR_DUPLICATION =  	-100,	// 중복 요청
};

typedef struct tagVERTEX
{
	int		x;
	int		y;
} stVERTEX32, *PstVERTEX32;

typedef struct tagVERTEX16
{
	short		x;
	short		y;
} stVERTEX16, *PstVERTEX16;

typedef struct
{
	int Year;
	int Month;
	int Day;
	int Hour;
	int Minute;
	int Second;
	int Week;
} stUDateTime;

// 20091026-1 yky
struct NstRouteResRID
{
	/// 결과 링크 배열
	unsigned int			m_RID;				// 링크 RID
	int			m_nLinkRC;			// 링크 통행시간
	int			m_nLinkVC;			// 링크 통행코스트 (Debug시 값 있음)
	unsigned short			m_nLenth;			// 링크 길이
	// 20100715-1 yky 코드 재정의
	unsigned char			m_RCType;			// RC구분 (1:없음(정적), 2:패턴, 3(default):TPEG)
	unsigned char			m_Cong;				// TPEG혼잡도 (0:정보없음,1:원활,2:서행,3:정체,4:통제/기타)
};

// 20091026-1 yky
// --------------------------------------------------------------------------------
// 탐색지점 정보 (출발지, 경유지, 목적지)
struct NstSchPosInfo
{
	bool			m_bPos;			// 경로탐색에서 사용 여부
	/*
	 * kiwoo 13.05.15
	 * tmap은 100byte ( tvas format )
	 */
	char			m_strName[100];
	int			m_wptX;
	int			m_wptY;
	int			m_nRpFlag;		// POI에서 제공한 값
	int			m_nPOIID;		// POI에서 제공한 값
	int			m_nAddrCD;		// POI에서 제공한 값
	int			m_nMppCnt;		// 출발지는 MPP 개수 0
	stVERTEX32		m_wpMpp[10];
	char		m_NavSeq[7];					///< 입구점번호
};

// 출발지 GPS 궤적 정보
struct NstGpsTraceInfo
{
	int					nGpsTraceDataSize;
	char*				pGpasTraceData;
};

// 재탐색 정보
struct NstReRouteInfo
{
	//탐색 조건 데이터
	int		nRouteDataSize;
    char		*pszRouteData;
	//주변링크 정보
	bool		bAroundLink;
	unsigned char		uOldRsdCode;				// 이탈 지점 기존경로 RSD 회전 코드
	unsigned char		uRsdCode;					// 진입 RSD 회전 코드
	unsigned short		uLinkID;					// 링크 id
	unsigned int		uTileCode;					// 타일코드(Link)
	unsigned char		uDirInfo;					// 방향 (0:정방향, 1:역방향)
	unsigned int		uPosX;						// 보간점 X 좌표(경도: 정수8자리 SK정규화좌표)
	unsigned int		uPosY;						// 보간점 Y 좌표(위도: 정수8자리 SK정규화좌표)
	//톨게이트 정보
	unsigned short		uPassTollID;				// 최종통과 톨게이트 ID

	int		nLastRid;				//마지막 매칭 RID
	int		nTcRid;					//이전경로 정체 RID(TVAS5.2이상에서 사용됨)
    char	szPreMapVersion[9];		//이전경로 맵버전
	int		nPreRidsCnt;
	int		*pnPreRids;				//이전경로 RID목록
	int		nPreSecsCnt;
	int		*pnPreSecs;				//이전경로 RID의 소요시간 목록
	char	szPreTollgateIds[32];		//이전경로 톨게이트 ID데이터
	int		nInitSrchLength;		//초기탐색 직선거리
    char	szInitSrchSessionId[25];	//초기탐색 SessionId;
};

struct NstPastPositionInfo{
    char szPastSessionId[25];
    int nPastXpos;
    int nPastYpos;
    char szPastArrivalTime[15];
};

// 20091026-1 yky
struct NstRouteReqLogic
{
	NeNddsRouteType		eRouteType;

	char				m_Tvas[5+1];
	unsigned int		m_uMultiOption;		// UI 경로탐색종류 (UI->Module 용)
	int					m_nCoodType;		// 경위도 포맷 (1:bessel, 2:WGS84)

	bool				m_bStGpsPosition;	// 출발지 GPS 좌표 여부
	int					m_nDirPriority;		// 주행방향 우선/비우선 옵션 (0:비우선, 1:우선)
	int					m_nStRoadType;		// 출발지 도로종별 (0:없음, 1: 고속도로, 2: 도시고속화, 3: 고가도로, 4: 지하도로, 5: 일반도로, 6:가까운도로, 7:미선택 )
	unsigned char		m_DepartSrchFlag;	// 출발지 탐색 구분(0x00 : GPS, 0x04 : 최근길, 0x05 : 즐겨찾기, 0x06 : 경위도검색, 0x07 : 명칭검색, 0x08 : 주소검색)

	int 				m_nEstRouteFlag;	// 길안내 FLAG (0:실시간, 1:예측길안내)
	int 				m_nEstRouteType;	// 예측길안내 종류 (0:실시간길안내, 1:도착시간예측, 2:출발시간예측)
	int					m_nControlRouteReqFlag; // 통제 길안내 요청 여부(0:요청안함, 1:요청함, 2:서버에서 판단)

	stUDateTime			m_ReqTime;			// 요청시간정보 Ptr
	int					m_nAngle;			// 각도(진북 기준, 0~359 )
	int					m_nSpeed;			// 차량진행 속도( Km/h)
	bool				m_bReRoute;			// 재탐색여부
	int					m_nPosInfoCnt;		// 탐색지점정보 개수 (N은 2이상 5이하)
	bool				m_bStopPosInfoFlag;// 정차지점사용 여부 (0:미사용, 1:사용)
    char                m_GuideImageCode[10];//	"NotSupported", "R170x207", "R240x252", "R320x316", "R341x415", "R480x504" ,"R480x540"

	NstSchPosInfo		m_nsPosInfo[5];		// 탐색지점정보 (0:출발지, 1:경유지1, 2:경유지2, 3:경유지3, 4:목적지)
    unsigned char		m_DestSrchFlag;     // 목적지 탐색 구분 ID

	NstGpsTraceInfo		m_nsGpsTrace;		// 출발지 GPS 궤적 정보

	//차량 정보
	int					nVechileType;		//  0 : 1종  1: 2종  2: 3종   3: 4종  4: 5종  5: 6종
	bool				m_bHipass;			// 하이패스 유무
	int					nFuelType;			// 연료 타입   0 : 휘발류  1: 경유  2: LPG

	// 추가 단속카메라 정보
	bool				m_bpAddCameraType[N_ROUTE_RQ_ADDCAMERA];	// 탐색 요청 단속카메라 추가정보 여부 ([0]선택없음, [1]버스전용차로, [2]신호, [3]이동식, [4]갓길, [5]가변차로(사용안함), [6]과적(사용안함), [7]방범(사용안함), [8]교통정보)
	bool				m_bpAddDAType[N_ROUTE_RQ_ADDCAMERA];		// 탐색 요청 위험구역 추가정보 여부 ([0]선택없음, [1]사고다발, [2]급커브, [3]급경사(사용안함), [4]로드킬(사용안함), [5]안개, [6]낙석(사용안함), [7]어린이보호, [8]철길건넌목)

	// 재탐색 정보
	NstReRouteInfo		m_nsReRouteInfo;	// 재탐색 정보
    NstPastPositionInfo m_nsPassPosInfo;
};

// 20091026-1 yky
struct NstRouteSummaryReqLogic
{
	int					m_nRouteSummaryType;
	int					m_nRouteServiceType;
	stUDateTime			m_ReqTime;			// 
	int					m_nVertexFlag;		//

	unsigned char		m_DepartDirPriority;		// 주행방향 우선/비우선 옵션 (0:비우선, 1:우선)
	unsigned char		m_DepartSrchFlag;

	int					m_nPosInfoCnt;		// 탐색지점정보 개수 (N은 2이상 5이하)
	NstSchPosInfo		m_nsPosInfo[5];		// 탐색지점정보 (0:출발지, 1:경유지1, 2:경유지2, 3:경유지3, 4:목적지)

	int					m_nAngle;			// 각도(진북 기준, 0~359 )
	int					m_nSpeed;			// 차량진행 속도( Km/h)

	unsigned char		m_DestSrchFlag;
};

struct NstRouteCollectLogic
{
	int nSearchOption[3];
    char szSessionID[25];
	unsigned int nCollectSize[3];
	unsigned char *pCollectData[3];
	char	szCollectPath[3][256];
};

struct NstNddsWeatherInfo{
	char m_SkyCode[8];
	char m_SkyName[30];
	char m_RainTypeCode[2];
	char m_RainSinceOnTime[7];
	char m_TempC1h[6];
};

struct NstNddsRouteSummaryList{
	int m_GuideType;			// 안내 속성 데이터 ID
	char m_SummaryName[64];		// 요약 명칭 데이터
	int m_RecmdRouteLen;		// 구간거리(단위:m, 올림:1의자리)
	int m_RecmdRouteTime;		// 구간시간 (단위:초, 올림:1의자리)
	int	m_Speed;				// 교통수집 속도
	unsigned char m_cCongestion;		// 교통수집 혼잡도
};

struct NstNddsRouteSummaryResult{
	int m_SessionID;
	int	m_TotalLength;					// 경로 총거리 (m 단위)
	int m_TotalTime;						// 경로 총 소요시간 (초 단위)
	int m_TotalTollgateCost;				// 톨게이트 요금정보 (원 단위)
	int	m_TotalTaxiCost;					// 택시요금 (단위:원)
	int	m_EstimationOption;				// 예측구분코드
	char m_EstimationTime[16];				// 출발/도착예측시간(YYYYMMDDHHMMSSS)
	//int m_DepartRoadType;					// 출발지 도로 종별
	//char m_MainRoad[128];						// 전체 경로 내 주요 도로 정보 [Frome 도로명]-[To 도로명]
	//int m_VertexFlag;						// 경로 좌표 요청 구분
	//int m_SummaryListCnt;					// 길안내 요약 구간 정보 리스트 개수
	//NstNddsRouteSummaryList *m_sSummaryList;	// 길안내 요약 구간 정보 리스트
	//char *m_VertexCoord;					// Vertex 좌표 리스트
};

typedef struct tagNstPoiEvStationChargerInfos{
    char szBid[MAXLEN_ID+1];		// 기관코드 BG:비긴스/JE:제주전기자동차서비스/KP:한국전력/HE:한국전기차충전서비스/PI:포스코ICT/SE:서울시/PW:파웤큐브/CT:씨티카/ME:환경부/JD:제주특별자치도청/TM:티맵/HM:현대자동차네비게이션
    char szSid[MAXLEN_ID+1];		//충전소ID
    char szCid[MAXLEN_ID+1];		//충전기ID
    char szType[MAXLEN_ID+1];		//충전기타입(01:DC차데모 /02:승용차AC완속 /03:DC차데모+AC3상 /04:DC콤보 /05:DC차데모+DC콤보 /06:DC차데모+AC3상+DC콤보 /07:AC급속3상
    int	nStatus;					//상태정보(0:알수없음 / 1:통신이상 /2:충전대기 /3:충전중 /4:운영중지 /5:점검중 /6:예약중)
}N_POI_EVSTATION_CHARGER_INFOS;

// 검색 결과 리스트 항목
typedef struct tagNstSearchList{
	int		nIdx;									// 검색 결과 인덱스
	char	szName[MAXLEN_POINAME+1];				// 명칭
	char	szStdName[MAXLEN_POINAME+1];			// 대표 명칭
	char	szAddrName[MAXLEN_ADDRNAME+1];			// 주소
	char    szMngName[MAXLEN_MNGNAME+1];            // 운영기관
	char	szPhoneNumber[MAXLEN_PHONENUMBER+1];	// 전화번호
	int		nPoiID;									// POI ID
	int		addrcode;
	char	RPFlag;									// RPFlag
	int		nDist;									// 거리
	int		nDeg;									// 방위각
	int		nKind;									// 종류(테마검색일때 이미지가 있으면 1, 와이파이주유소 이면 1, 나머지 검색은 업종코드)
	stVERTEX32	wp;									// 위치 좌표
	int		nOPIPoleCate;							// 주유소종류
	int		nOPIPrice;								// 가격
	unsigned int uGatherTime;						// 유가정보 제공일자

	char	szOilCompany[16];						// 주유소브랜드(SK, GS, 기타 등), 단말에서 브랜드 필터링시 사용
    int     nDataKind;                              // 주유소종류 (3: 주유소, 4:충전소, 6: EV충전소)
	bool	bHighGasoline;							// 고급휘발유판매점여부 (0:판매안함, 1:판매)
	bool	bLowestOilSt;							// 최저가점포여부. 출력리스트상 최저가 점포일 경우(최저가:1, 아닌경우:0). SK주유소가 최저가인 경우에만 1로 설정
	unsigned int	nOPIGasolinePrice;				// 휘발유가격(원)
	unsigned int	nOPIDieselPrice;				// 경유가격(원)
	unsigned int	nOPILPGPrice;					// LPG가격(원)
	unsigned int	nOPIHiGasolinePrice;			// 프리미엄 휘발유 가격(원)
	unsigned int	nOPIHiLPGPrice;					// 프리미엄 LPG 가격(원)
	
    char	szNavSeq[7];							// 입구점번호
    int nEvChargerTotalCnt;							// 충전기 갯수
    N_POI_EVSTATION_CHARGER_INFOS sEvChargers[MAXLEN_EVSTATION_CHARGERS]; // 충전기 리스트
}N_SEARCH_LIST;

typedef struct tagNstPoiImageInfos{
	int nSeq;										// 이미지 순번
	char szFileSrc[MAXLEN_URL+1];					// POI 이미지 경로
}N_POI_IMAGE_INFOS;

typedef struct tagNstPoiParkInfos{
	char szCpName[MAXLEN_POINAME+1];			// 제공처명 예)"모두의주차장","파킹박"
	char szParkUrl[MAXLEN_URL+1];					// 모바일 웹 링크 URL
}N_POI_PARK_INFOS;

/// POI 상세정보
typedef struct tagNstSearchPoiDetailInfo{
	int nPoiID;										// POI ID
	char szNavSeq[7];							// 입구점번호
    int nViewID;								// 업종별 분기 주유소/충전소 구분자(3:주유소,4:충전소)
    char szName[MAXLEN_POINAME+1];				// 명칭
	char szAddrName[MAXLEN_ADDRNAME+1];			// 구주소, 새주소 (새주소 요청시 DB에 새주소 정보가 없으면 구까지만 전달)

    char szLcdName[MAXLEN_CDNAME+1];			// 지역 대분류명
    char szMcdName[MAXLEN_CDNAME+1];			// 지역 중분류명
    char szScdName[MAXLEN_CDNAME+1];			// 지역 소분류명
    char szDcdName[MAXLEN_CDNAME+1];			// 지역 대분류명
    char szPrimaryBun[MAXLEN_CDNAME+1];			// 본번
    char szSecondBun[MAXLEN_CDNAME+1];			// 부번
    int	nMlClass;								// 산/대지 구분자(0:알수없음,1:대지,2:산)
    char szBldAdd[MAXLEN_ADDRNAME+1];			// 건물주소 표출용 주소 Ex)서울 중구 세종대로
    char szRoadName[MAXLEN_ROADNAME+1];			// 도로명
    char szBldNo1[MAXLEN_ROADNAME+1];			// 건물번호1
    char szBldNo2[MAXLEN_ROADNAME+1];			// 건물번호2
    stVERTEX32 centerTp;						// 중심점 좌표
    char bRpFlag;								// RP Flag
    char szZipCd[MAXLEN_ZIPCODE+1];				// 우편번호
    char szMenu1[MAXLEN_MENU+1];				// 메뉴1
    char szMenu2[MAXLEN_MENU+1];				// 메뉴2
    char szMenu3[MAXLEN_MENU+1];				// 메뉴3
    char szMenu4[MAXLEN_MENU+1];				// 메뉴4
    char szMenu5[MAXLEN_MENU+1];				// 메뉴5
    char szTelNo[MAXLEN_TELNO+1];				// 전화번호
    char szRoad[MAXLEN_ROAD+1];					// 찾아가는 길
    int nRecommandPoint;						// 좋아요 평가 점수
    char szIconType[MAXLEN_CATECODE+1];			// 아이콘 표시용 표출업종코드
    char szJoinStoreYn[MAXLEN_JOINYN+1];		// 가맹점여부정보 세미콜론(;)으로 구분(OKCB:OKCashbag, OKCB;GIFT:OKCashbag,기프티콘, OKCB;TMEM;SSCD)
    char szJoinViewType[MAXLEN_JOINVIEWTYPE+1];	// 가맹점화면 Type 세미콜론(;)으로 구분 가맹점여부정보와 1:1대응(OKCB-> ATYP / TMEM->BTYP / GIFT->XTYP / SSCD->CTYP / )

    stVERTEX32 tp;									// 위치 좌표
	char szPhoneNumber[MAXLEN_PHONENUMBER+1];		// 전화번호
	char szUrl[MAXLEN_URL+1];						// 웹사이트
	bool bHoliday;									// 연중무휴(true:연중무휴)
	char byPark;									// 주차 가능 여부(0:불가능, 1:가능, 그외:정보없음)				
	char szCateName[MAXLEN_POINAME+1];			// 업종 명칭
    char szInfo[MAXLEN_DETAILINFO+1];			// 소개글
    // sangbong.woo - s
    char szAddInfo[MAXLEN_DETAILINFO+1];			// 추가소개정보
    unsigned int m_GatherTime;
    bool m_HighGasoline;
    //X bool	m_LowestOilSt;
    int m_dataKind;                                         // 주유소종류 (3: 주유소, 4:충전소, 6: EV충전소)
    unsigned int m_nOPIGasolinePrice;
    unsigned int m_nOPIDieselPrice;
    unsigned int m_nOPILPGPrice;
    unsigned int m_nOPIHiGasolinePrice;
    unsigned int m_nOPIHiLPGPrice;
    // sangbong.woo - e

	char szDayOffInfo[MAXLEN_DAYOFFINFO+1];					// 휴무정보
	char szBusinessHours[MAXLEN_BUSINESSHOURS+1];				// 영업시간
	char szDayOffDate[MAXLEN_DAYOFFDATE+1];					// 휴무일자

	N_POI_IMAGE_INFOS sPoiImageInfos[5];			// POI 이미지 정보
	N_POI_PARK_INFOS sPoiParkInfos[3];				// 제휴 주차장 정보
}N_SEARCH_POIDETAILINFO;

/// POI 최저가
typedef struct tagNstSearchPoiStationInfo{
    int nPoiID;										// POI ID
    char szName[MAXLEN_POINAME+1];				// 명칭
    stVERTEX32 tp;									// 위치 좌표
    int m_dataKind;                                         // 주유소종류 (3: 주유소, 4:충전소, 6: EV충전소)
    unsigned int m_nOPIGasolinePrice;
    unsigned int m_nOPIDieselPrice;
    unsigned int m_nOPILPGPrice;
    unsigned int m_nOPIHiGasolinePrice;
    unsigned int m_nOPIHiLPGPrice;
}N_SEARCH_POISTATIONINFO;

//evStation Info
typedef struct tagNstSearchPoiEvStationInfo{
    int nPoiID;									///< POI ID
    char szStationName[MAXLEN_POINAME+1];       ///< 명칭
    char szAddr[MAXLEN_ADDRNAME+1];				///< 주소
    char szRoadAddr[MAXLEN_ROADADDR+1];         ///< 도로명주소
    char szAddrDetail[MAXLEN_ADDRDETAIL+1];     ///< 상세주소
    char szMngName[MAXLEN_MNGNAME+1];           ///< 운영기관
    char szMngId[MAXLEN_MNGID+1];               ///< 기관관리 ID
    char szTel[MAXLEN_PHONENUMBER+1];			///< 연락처
    bool bFreeYn;								///< 주차비무료여부
    bool bReservYn;								///< 예약가능여부
    char szUseTime[MAXLEN_USETIME+1];			///< 이용가능시간
    bool bPayYn;								///< 유료여부
    char szFee[MAXLEN_FEE+1];                   ///< 단가
    char szUpdateDt[MAXLEN_USETIME+1];			///< 충전상태수집일시(YYYYMMDD HH24:MI:SS)
    stVERTEX32	wp;								///< 위치 좌표
    int nTotalCnt;								///< 충전기 갯수
    N_POI_EVSTATION_CHARGER_INFOS sEvChargers[MAXLEN_EVSTATION_CHARGERS]; ///<충전기 리스트
}N_SEARCH_POIEVSTATIONINFO;

typedef struct tagNstPoiEvStations{
    int nPoiID;									///< POI ID
    char szStationName[MAXLEN_POINAME+1];       ///< 명칭
    stVERTEX32	wp;								///< 위치 좌표
}N_POI_EVSTATIONS;

//evStation Info
typedef struct tagNstSearchPoiEvStationsByPoint{
    int nTotalCnt;								///< 충전기 갯수
    N_POI_EVSTATIONS sEvChargers[MAXLEN_EVSTATION_CHARGERS]; ///<충전기 리스트
}N_SEARCH_POIEVSTATIONSBYPOINT;

/// 가맹점 상세정보
typedef struct tagNstSearchPoiJoinInfo{
	int	nIdx;									// 검색 결과 인덱스
	int nPoiID;						// POI ID
	char szJoinType[16];				// 가맹점 종류 ("OKCB": OK캐시백, "TMEM": T맴버십, "SSCD": 삼성카드)
	char szName[MAXLEN_POINAME+1];	// 시설물 명칭
	char szCashRate[8];			// 현금 적립율
	char szCardRate[8];			// 카드 적립율
	char szWorkTime[16];			// 평일 영업시간
	char szWeekendWorkTime[16];	// 주말 영업시간
	char szUrl[MAXLEN_URL+1];				// 홈페이지
	char byPark;			// 주차여부("0":가능, "1":불가능, "" or null : 정보없음)
	char szBenefit[512];			// 혜택
	char szUseGuide[512];			// 이용안내
	char szInformation[512];		// 업소 소개
	int nItem;			// 아이템 리스트 개수
	char szItemInfos[10][128];		// 아이템 리스트
}N_SEARCH_POIJOININFO;


///UserData : poiMyFavorite
typedef struct tagNstPoiMyFavorite{
    char	szHomeCustName[MAXLEN_POINAME+1];		// 집으로 명칭 (고객이 입력한 명칭)
    stVERTEX32 Hometp;
    int		nHomePoiId;								// 집으로 POI ID
    char	szHomeNavSeq[7];						// 집으로 입구점 일련번호
    char	szHomeLcdName[MAXLEN_CDNAME+1];			// 집으로 지역대분류
    char	szHomeMcdName[MAXLEN_CDNAME+1];			// 집으로 지역중분류
    char	szHomeScdName[MAXLEN_CDNAME+1];			// 집으로	 지역소분류
    char	szHomeDcdName[MAXLEN_CDNAME+1];			// 집으로 지역세분류
    char	szHomePrimaryBun[MAXLEN_CDNAME+1];		// 본번
    char	szHomeSecondBun[MAXLEN_CDNAME+1];		// 부번
    int		nHomeMlClass;							// 산/대지 구분자(0:알수없음,1:대지,2:산)
    char	szHomeRoadName[MAXLEN_ROADNAME+1];		// 도로명
    char	szHomeBldNo1[MAXLEN_ROADNAME+1];		// 건물번호1
    char	szHomeBldNo2[MAXLEN_ROADNAME+1];		// 건물번호2
    char	byHomeRpFlag;							// 집으로 RP FLAG
    char	szHomeAddInfo[MAXLEN_FAVADDINFO+1];		// 집으로 부가정보 (행정구역명,좌표)
    char	szHomeInsDatetime[MAXLEN_SVCDATE+1];	// 집으로 입력일시 (YYYYMMDDHH24MISS)

    char	szOfficeCustName[MAXLEN_POINAME+1];		// 회사로 명칭 (고객이 입력한 명칭)
    stVERTEX32 Officetp;
    int		nOfficePoiId;							// 회사로 POI ID
    char	szOfficeNavSeq[7];						// 회사로 입구점 일련번호
    char	szOfficeLcdName[MAXLEN_CDNAME+1];		// 회사로 지역대분류
    char	szOfficeMcdName[MAXLEN_CDNAME+1];		// 회사로 지역중분류
    char	szOfficeScdName[MAXLEN_CDNAME+1];		// 회사로 지역소분류
    char	szOfficeDcdName[MAXLEN_CDNAME+1];		// 회사로 지역세분류
    char	szOfficePrimaryBun[MAXLEN_CDNAME+1];	// 본번
    char	szOfficeSecondBun[MAXLEN_CDNAME+1];		// 부번
    int		nOfficeMlClass;							// 산/대지 구분자(0:알수없음,1:대지,2:산)
    char	szOfficeRoadName[MAXLEN_ROADNAME+1];	// 도로명
    char	szOfficeBldNo1[MAXLEN_ROADNAME+1];		// 건물번호1
    char	szOfficeBldNo2[MAXLEN_ROADNAME+1];		// 건물번호2
    char	byOfficeRpFlag;							// 회사로 RP FLAG
    char	szOfficeAddInfo[MAXLEN_FAVADDINFO+1];	// 회사로 부가정보 (행정구역명,좌표)
    char	szOfficeInsDatetime[MAXLEN_SVCDATE+1];	// 회사로 입력일시 (YYYYMMDDHH24MISS)
}N_GET_POIMYFAVORITE;

typedef struct tagNstRouteInfo{
    char		szCustName[MAXLEN_POINAME+1];				// 명칭 (고객이 입력한 명칭)
    stVERTEX32	tp;											// 좌표(sk 정규화 좌표 7자리)
    int			nPoiID;										// POI ID
    char		byRpFlag;
}N_ROUTE_INFO;

///UserData : routeMyFavorite
typedef struct tagNstRouteMyFavorite{
    char routeTitle[MAXLEN_TITLE+1];		// 경로 제목
    char insDatetime[MAXLEN_SVCDATE+1];		// 입력시일(YYYYNNDDHH24MISS)
    N_ROUTE_INFO sStartRoute;				// 출발지 정보
    N_ROUTE_INFO sEndRoute;					// 도착지 정보
    N_ROUTE_INFO sMidRoute[3];				// 경유지 정보 리스트(MAX 3건)
}N_GET_ROUTEMYFAVORITE;

///UserData : poiRecent
typedef struct tagNstPoiRecent{
    char szName[MAXLEN_POINAME+1];		// 명칭
    stVERTEX32 tp;						// X 좌표
    int nPoiID;							// POI ID
    char szNavseq[7];					// 입구점번호
    char szLcdName[MAXLEN_CDNAME+1];		// 지역대분류
    char szMcdName[MAXLEN_CDNAME+1];		// 지역중분류
    char szScdName[MAXLEN_CDNAME+1];		// 지역소분류
    char szDcdName[MAXLEN_CDNAME+1];		// 지역세분류
    char szPrimaryBun[MAXLEN_CDNAME+1];		// 본번
    char szSecondBun[MAXLEN_CDNAME+1];		// 부번
    int	nMlClass;							// 산/대지 구분자(0:알수없음,1:대지,2:산)
    char szRoadName[MAXLEN_ROADNAME+1];		// 도로명
    char szBldNo1[MAXLEN_ROADNAME+1];		// 건물번호1
    char szBldNo2[MAXLEN_ROADNAME+1];		// 건물번호2
    char szRepClsName[MAXLEN_CATENAME+1];	// 대표업종명
    char szClsAName[MAXLEN_CATENAME+1];		// 업종 대분류명
    char szClsBName[MAXLEN_CATENAME+1];		// 업종 중분류명
    char szClsCName[MAXLEN_CATENAME+1];		// 업종 소분류명
    char szClsDName[MAXLEN_CATENAME+1];		// 업종 세분류명
    char szTelNo[MAXLEN_TELNO+1];			// 전화번호
    char byRPFlag;							// RP Flag
    char szTotalCnt[7+1];				// 총 서비스 횟수, 요청의 svcUploadYn이 "Y"면 필수
    char szSvcDate[MAXLEN_SVCDATE+1];	// 최종 서비스일시, 요청의 svcUploadYn이 "Y"면 필수 (YYYYMMDDHH24MISS)
}N_GET_POIRECENT;

///UserData : poiFavorite
typedef struct tagNstPoiFavorite{
    char szCustName[MAXLEN_POINAME+1];		// 명칭
    stVERTEX32 tp;
    int nPoiID;								// POI ID
    char szNavSeq[7];						// 입구점 일련번호
    char szLcdName[MAXLEN_CDNAME+1];		// 지역대분류
    char szMcdName[MAXLEN_CDNAME+1];		// 지역중분류
    char szScdName[MAXLEN_CDNAME+1];		// 지역소분류
    char szDcdName[MAXLEN_CDNAME+1];		// 지역세분류
    char szPrimaryBun[MAXLEN_CDNAME+1];		// 본번
    char szSecondBun[MAXLEN_CDNAME+1];		// 부번
    int	nMlClass;							// 산/대지 구분자(0:알수없음,1:대지,2:산)
    char szRoadName[MAXLEN_ROADNAME+1];		// 도로명
    char szBldNo1[MAXLEN_ROADNAME+1];		// 건물번호1
    char szBldNo2[MAXLEN_ROADNAME+1];		// 건물번호2

    char szRepClsName[MAXLEN_CATENAME+1];	// 대표업종명
    char szClsAName[MAXLEN_CATENAME+1];		// 업종 대분류명
    char szClsBName[MAXLEN_CATENAME+1];		// 업종 중분류명
    char szClsCName[MAXLEN_CATENAME+1];		// 업종 소분류명
    char szClsDName[MAXLEN_CATENAME+1];		// 업종 세분류명
    char byRPFlag;							// RP Flag
    char szTelNo[MAXLEN_TELNO+1];		// 전화번호
    char szAddInfo[MAXLEN_FAVADDINFO+1];		// 집으로 부가정보 (행정구역명,좌표)
    char szOrgCustName[MAXLEN_POINAME+1];	// 최초 명칭
    char szIconInfo[MAXLEN_POINAME+1];		// 아이콘 정보 (/별/, /하트/, /병원/ 등)
    char szInsDatetime[MAXLEN_SVCDATE+1];	// 집으로 입력일시 (YYYYMMDDHH24MISS)
}N_GET_POIFAVORITE;

///UserData : routeFavorite
typedef struct tagNstRouteFavorite{
    char routeTitle[MAXLEN_TITLE+1];		// 경로 제목
    char insDatetime[MAXLEN_SVCDATE+1];		// 입력시일(YYYYNNDDHH24MISS)
    N_ROUTE_INFO sStartRoute;				// 출발지 정보
    N_ROUTE_INFO sEndRoute;					// 도착지 정보
    N_ROUTE_INFO sMidRoute[3];				// 경유지 정보 리스트(MAX 3건)
}N_GET_ROUTEFAVORITE;

/// UserData
typedef struct tagNstUserData{
    N_GET_POIMYFAVORITE sPoiMyFavorite;
    N_GET_ROUTEMYFAVORITE sRouteMyFavorite;
    int nPoiRecent;
    int nPoiFavorite;
    int nRouteFavorite;
    N_GET_POIRECENT sPoiRecent[MAXLEN_POIRECENT];
    N_GET_POIFAVORITE sPoiFavorite[MAXLEN_POIFAVORITE];
    N_GET_ROUTEFAVORITE	sRouteFavorite[MAXLEN_ROUTEFAVORITE];
}N_GET_USERDATA;

/// POI 최근길 조회
typedef struct tagNstSearchRecentDestnation{
	int	nIdx;									// 검색 결과 인덱스
	char szName[MAXLEN_POINAME+1];		// 명칭
	stVERTEX32 tp;						// X 좌표
	int nPoiID;							// POI ID
	char szNavSeq[7];				// 입구점번호
	unsigned char RPFlag;				// RP Flag
	char szSvcDate[MAXLEN_SVCDATE+1];		// 최종 서비스일시 (YYYYMMDDHH24MISS)
}N_SEARCH_RECENTDESTNATION;

typedef struct tagNstSearchUploadRecentDestnation{
    char szName[MAXLEN_POINAME+1];		// 명칭
    stVERTEX32 tp;						// X 좌표
    int nPoiID;							// POI ID
    char szNavseq[7];					// 입구점번호
    unsigned char RPFlag;				// RP Flag
    char szTotalCnt[7+1];				// 총 서비스 횟수, 요청의 svcUploadYn이 "Y"면 필수
    char szSvcDate[MAXLEN_SVCDATE+1];	// 최종 서비스일시, 요청의 svcUploadYn이 "Y"면 필수 (YYYYMMDDHH24MISS)
}N_SEARCH_UPLOADRECENTDESTNATION;

typedef struct tagNstSearchRemoveRecentDestnation{
    char szName[MAXLEN_POINAME+1];		// 명칭
    stVERTEX32 tp;						// 좌표
}N_SEARCH_REMOVERECENTDESTNATION;

/// VMS 가이드 정보 조회
typedef struct tagNstVmsGuideInfo{
	char szBaseSdttm[13];
	char szDataSource[50];
	char szContentCode[4];
	char szContentName[30];
	char szContent[128];
}N_VMSGUIDEINFO;

typedef struct tagNstAuthExternalUserInfos
{
	char szProvider[16];
	char szReadableUserId[32];
}N_AUTH_EXTERNALUSERINFOS;

typedef struct tagNstAuthAuthInfo
{
	int nValidateCode;
	char szValidationSubField[32];
	int nExternalUserInfoCount;
	N_AUTH_EXTERNALUSERINFOS sExternalUserInfos[6];
}N_AUTH_AUTHINFO;

typedef struct tagNstHeadUnitDeviceAuthInfo
{
	int nResultCode;
	int nResultSubField;
}N_AUTH_HEADUNITDEVICEAUTHINFO;

typedef struct tagNstVersionInfo
{
	char szMenuNoticeVer[16];
	char szOpenappVer[16];
}N_AUTH_VERSIONINFO;

typedef struct tagNstAppUpdateInfo
{
	char szAppUpdateType[4];
	char szAppVer[16];
	char szAppUpdateViewFlag[4];
	char szAppUpdateUrl[512];
	char szAppUpdateMsg[512];
}N_AUTH_APPUPDATEINFO;

typedef struct tagNstControlInfoDetails
{
	char szServiceName[32];
	char szServiceYn[4];
	char szServiceItems[512];
}N_AUTH_CONTROLINFODETAILS;

typedef struct tagNstAppControlInfo
{
	int nControlInfoCount;
	N_AUTH_CONTROLINFODETAILS sControlInfoDetails[6]; 
}N_AUTH_APPCONTROLINFO;

typedef struct tagNstCommonInfo
{
	char szEuk[256];
	char szEid[256];
}N_AUTH_COMMONINFO;

typedef struct tagNstTidAuthResInfo
{
	int nResultCode;
	int nResultSubField;
	int nResultMdnAuthInfo;
	int nExternalUserInfoCount;
	N_AUTH_EXTERNALUSERINFOS sExternalUserInfos[6];
}N_AUTH_TIDAUTHRESINFO;

typedef struct tagNstPushInfo
{
	char szPushSetYn[4];
	char szSmsYn[4];
	char szMomentYn[4];
}N_AUTH_PUSHINFO;

typedef struct tagNstPersonalInfo
{
	char szEmail[64];
	char szUserName[32];
	char szMdn[16];
	char szBirth[16];
}N_AUTH_PERSIONINFO;

typedef struct tagNstCarInfo
{
	char szModel[16];
	char szFuel[12];
	char szHipassYn[16];
	char szNumber[16];
}N_AUTH_CARINFO;

typedef struct tagNstUserProfileInfo
{
	N_AUTH_PERSIONINFO sPersonalInfo;
	N_AUTH_CARINFO sCarInfo;
}N_AUTH_USERPROFILEINFO;

/// 단말인증
typedef struct tagNstAuthTmapForSmartCar{
	char szAccessKey[50];
	N_AUTH_COMMONINFO sCommonInfo;
	N_AUTH_AUTHINFO sAuthInfo;
	N_AUTH_HEADUNITDEVICEAUTHINFO sHeadUnitDeviceAuthInfo;
	N_AUTH_TIDAUTHRESINFO sTidAuthResInfo;
	N_AUTH_VERSIONINFO sVersionInfo;
	N_AUTH_APPUPDATEINFO sAppUpdateInfo;
	N_AUTH_APPCONTROLINFO sAppControlInfo;
	N_AUTH_PUSHINFO sPushInfo;
	N_AUTH_USERPROFILEINFO sUserProfileInfo;
}N_AUTH_AUTHTMAPFORSMARTCAR;


/// 즐겨찾기 등록 리스트 정보(poiFavorite)
typedef struct tagRegistFavoriteInfo{
    char szCustName[MAXLEN_POINAME+1];		// 명칭
    stVERTEX32 tp;
    int nPoiID;								// POI ID
    char szNavSeq[7];						// 입구점 일련번호
    char byRPFlag;							// RP Flag
    char szAddInfo[MAXLEN_FAVADDINFO+1];	// 부가정보
    char szOrgCustName[MAXLEN_POINAME+1];	// 최초 명칭
    char szIconInfo[MAXLEN_POINAME+1];      // 아이콘 정보 (/별/, /하트/, /병원/ 등)
}DS_REGIST_POIFAVORITE_INFO;
/// 즐겨찾기 전체 업로드 정보
/// POI 즐겨찾기 집으로/회사로 등록 정보
typedef struct tagUploadMyFavoriteInfo{
    char szHomeName[MAXLEN_POINAME+1];		// 집으로 명칭
    stVERTEX32 Hometp;						// 집으로 좌표
    int nHomePoiID;							// 집으로 POI ID
    char szHomeNavSeq[7];					// 집으로 입구점 일련번호
    char byHomeRpFlag;						// 집으로 rp Flag
    char szHomeAddInfo[MAXLEN_FAVADDINFO+1];// 집으로 부가정보(행정구역명 좌표)

    char szOfficeName[MAXLEN_POINAME+1];	// 회사로 명칭
    stVERTEX32 Officetp;					// 회사로 좌표
    int nOfficePoiID;						// 회사로 POI ID
    char szOfficeNavSeq[7];					// 회사로 입구점 일련번호
    char byOfficeRpFlag;					// 회사로 rp Flag
    char szOfficeAddInfo[MAXLEN_FAVADDINFO+1];	// 회사로 부가정보(행정구역명 좌표)
}DS_UPLOAD_MYFAVORITE_INFO;

/// POI 즐겨찾기 등록 리스트 정보(poiFavorite)
typedef struct tagUploadPoiFavoriteInfo{
    char szCustName[MAXLEN_POINAME+1];		// 명칭
    stVERTEX32 tp;							// 좌표
    int nPoiID;								// POI ID
    char byRPFlag;							// RP Flag
    char szAddInfo[MAXLEN_FAVADDINFO+1];	// 부가정보
    char szOrgCustName[MAXLEN_POINAME+1];	// 최초 명칭
    char szIconInfo[MAXLEN_POINAME+1];		// 아이콘 정보 (/별/, /하트/, /병원/ 등)
    char szInsDatetime[MAXLEN_SVCDATE+1];	// 입력일시 (YYYYMMDDHH24MISS)
    char szNavSeq[7];						// 입구점 일련번호
}DS_UPLOAD_POIFAVORITE_INFO;


/// POI 즐겨찾기 삭제 리스트 정보(poiFavorite)
typedef struct tagRemovePoiFavoriteInfo{
    char szCustName[MAXLEN_POINAME+1];		// 명칭
    stVERTEX32 tp;							// 좌표
}DS_REMOVE_POIFAVORITE_INFO;

/// POI 즐겨찾기 수정 정보
typedef struct tagModifyPoiFavoriteInfo{
    char szCustName[MAXLEN_POINAME+1];		// 명칭
    stVERTEX32 tp;							// 좌표
    char szNewCustName[MAXLEN_POINAME+1];	// 변경 후 명칭
    char szIconInfo[MAXLEN_POINAME+1];		// 아이콘 정보(/별/, /하트/, /병원/ 등)
}DS_MODIFY_POIFAVORITE_INFO;

// 즐겨찾기 결과 항목
typedef struct tagNstFavoriteList{
	int	nIdx;									// 검색 결과 인덱스
	char szCustName[MAXLEN_POINAME+1];			// 명칭
	stVERTEX32 tp;	
	int nPoiID;			// POI ID
	char szNavSeq[7];			// 입구점 일련번호
	char RPFlag;			// RP Flag
	char szAddInfo[MAXLEN_FAVADDINFO+1];			// 부가정보
	char szInsDatetime[MAXLEN_SVCDATE+1];		// 입력일시 (YYYYMMDDHH24MISS)
}N_FAVORITE_LIST;

typedef struct tagNstMyFavorite{
	char	szHomeCustName[MAXLEN_POINAME+1];			// 집으로 명칭 (고객이 입력한 명칭)
	stVERTEX32 Hometp;	
	int		nHomePoiId;			// 집으로 POI ID
	char	szHomeNavSeq[7];			// 집으로 입구점 일련번호
	char	byHomeRpFlag;			// 집으로 RP FLAG
	char	szHomeAddInfo[MAXLEN_FAVADDINFO+1];			// 집으로 부가정보 (행정구역명,좌표)
	char	szHomeInsDatetime[MAXLEN_SVCDATE+1];		// 집으로 입력일시 (YYYYMMDDHH24MISS)
	char	szOfficeCustName[MAXLEN_POINAME+1];		// 회사로 명칭 (고객이 입력한 명칭)
	stVERTEX32 Officetp;
	int		nOfficePoiId;			// 회사로 POI ID
	char	szOfficeNavSeq[7];			// 회사로 입구점 일련번호
	char	byOfficeRpFlag;			// 회사로 RP FLAG
	char	szOfficeAddInfo[MAXLEN_FAVADDINFO+1];		// 회사로 부가정보 (행정구역명,좌표)
	char	szOfficeInsDatetime[MAXLEN_SVCDATE+1];	// 회사로 입력일시 (YYYYMMDDHH24MISS)
}N_MYFAVORITE;

#pragma pack(1)

struct NstNddsPoiResult
{
	char	Aliasname[MAXLEN_POINAME];

	char	m_name[MAXLEN_POINAME];
	char	m_addrname[80];
	char	m_addrname_new[80];
	int		m_X;
	int		m_Y;

	int		m_PoiID;
	int		m_Dist;
	char	m_strTell[16];	//081028

	char	m_RPFlag;

	unsigned int			m_GatherTime;
	unsigned int			m_nOPIPrice;

	char	m_strOilCompany[16];
	bool	m_HighGasoline;
	bool	m_LowestOilSt;
    int     m_dataKind;
	unsigned int			m_nOPIGasolinePrice;
	unsigned int			m_nOPIDieselPrice;
	unsigned int			m_nOPILPGPrice;
	unsigned int			m_nOPIHiGasolinePrice;
	unsigned int			m_nOPIHiLPGPrice;

	char			m_NavSeq[7];
};

struct NstNddsPoiImageInfos
{
	int m_Seq;										// 이미지 순번
	char m_FileSrc[MAXLEN_URL+1];					// POI 이미지 경로
};

struct NstNddsPoiParkInfos
{
	char m_CpName[MAXLEN_POINAME+1];			// 제공처명 예)"모두의주차장","파킹박"
	char m_ParkUrl[MAXLEN_URL+1];					// 모바일 웹 링크 URL
};

struct NstNddsPoiDetailInfo
{
	int	m_PoiID;										// POI ID
	char	m_NavSeq[7];							// 입구점번호
	char m_Name[MAXLEN_POINAME+1];				// 명칭
	char m_AddrName[MAXLEN_ADDRNAME+1];			// 구주소, 새주소 (새주소 요청시 DB에 새주소 정보가 없으면 구까지만 전달)
	int		m_X;
	int		m_Y;
	char m_PhoneNumber[MAXLEN_PHONENUMBER+1];		// 전화번호
	char m_Url[MAXLEN_URL+1];						// 웹사이트
	bool m_Holiday;									// 연중무휴(true:연중무휴)
	char m_Park;									// 주차 가능 여부(0:불가능, 1:가능, 그외:정보없음)				
	char m_CateName[MAXLEN_POINAME+1];			// 업종 명칭
    char m_Info[MAXLEN_DETAILINFO+1];			// 소개글

    char m_LcdName[MAXLEN_CDNAME+1];			// 지역 대분류명
    char m_McdName[MAXLEN_CDNAME+1];			// 지역 중분류명
    char m_ScdName[MAXLEN_CDNAME+1];			// 지역 소분류명
    char m_DcdName[MAXLEN_CDNAME+1];			// 지역 세분류명
    char m_PrimaryBun[MAXLEN_CDNAME+1];			// 본번
    char m_SecondaryBun[MAXLEN_CDNAME+1];		// 부번
    int m_MlClass;								// 산/대지 구분자(0:알수없음,1:대지,2:산)
    char m_BldAdd[MAXLEN_ADDRNAME+1];			// 건물주소 표출용 주소 Ex)서울 중구 세종대로
    char m_RoadName[MAXLEN_ROADNAME+1];			// 도로명
    char m_BldNo1[MAXLEN_ROADNAME+1];			// 건물번호1
    char m_BldNo2[MAXLEN_ROADNAME+1];			// 건물번호2
    int m_centerX;								// 중심점 X좌표
    int m_centerY;								// 중심점 Y좌표
    char m_RpFlag;								// RP Flag
    char m_ZipCd[MAXLEN_ZIPCODE+1];				// 우편번호
    char m_Menu1[MAXLEN_MENU+1];				// 메뉴1
    char m_Menu2[MAXLEN_MENU+1];				// 메뉴2
    char m_Menu3[MAXLEN_MENU+1];				// 메뉴3
    char m_Menu4[MAXLEN_MENU+1];				// 메뉴4
    char m_Menu5[MAXLEN_MENU+1];				// 메뉴5
    char m_TelNo[MAXLEN_TELNO+1];				// 전화번호
    char m_Road[MAXLEN_ROAD+1];					// 찾아가는 길
    int m_RecommandPoint;						// 좋아요 평가 점수
    char m_IconType[MAXLEN_CATECODE+1];			// 아이콘 표시용 표출업종코드
    char m_JoinStoreYn[MAXLEN_JOINYN+1];		// 가맹점여부정보 세미콜론(;)으로 구분(OKCB:OKCashbag, OKCB;GIFT:OKCashbag,기프티콘, OKCB;TMEM;SSCD)
    char m_JoinViewType[MAXLEN_JOINVIEWTYPE+1];	// 가맹점화면 Type 세미콜론(;)으로 구분 가맹점여부정보와 1:1대응(OKCB-> ATYP / TMEM->BTYP / GIFT->XTYP / SSCD->CTYP / )

    // sangbong.woo - s
    char m_AddInfo[MAXLEN_DETAILINFO+1];			// 추가소개정보
    unsigned int m_GatherTime;
    bool m_HighGasoline;
    //X bool	m_LowestOilSt;
    int m_dataKind;                                 // 주유소종류 (3: 주유소, 4:충전소, 6: EV충전소)
    unsigned int m_nOPIGasolinePrice;
    unsigned int m_nOPIDieselPrice;
    unsigned int m_nOPILPGPrice;
    unsigned int m_nOPIHiGasolinePrice;
    unsigned int m_nOPIHiLPGPrice;
    // sangbong.woo - e

	char m_DayOffInfo[MAXLEN_DAYOFFINFO+1];					// 휴무정보
	char m_BusinessHours[MAXLEN_BUSINESSHOURS+1];				// 영업시간
	char m_DayOffDate[MAXLEN_DAYOFFDATE+1];					// 휴무일자

	NstNddsPoiImageInfos m_PoiImageInfos[5];			// POI 이미지 정보
	NstNddsPoiParkInfos m_PoiParkInfos[3];				// 제휴 주차장 정보
};

struct NstNddsPoiStationInfo
{
    int	m_PoiID;								// POI ID
    char m_Name[MAXLEN_POINAME+1];				// 명칭
    int		m_X;
    int		m_Y;
    int m_dataKind;                              // 주유소종류 (3: 주유소, 4:충전소, 6: EV충전소)
    unsigned int m_nOPIGasolinePrice;
    unsigned int m_nOPIDieselPrice;
    unsigned int m_nOPILPGPrice;
    unsigned int m_nOPIHiGasolinePrice;
    unsigned int m_nOPIHiLPGPrice;
};

struct NstNddsPoiEvStationChargerInfos{
    char szBid[MAXLEN_ID+1];		// 기관코드 BG:비긴스/JE:제주전기자동차서비스/KP:한국전력/HE:한국전기차충전서비스/PI:포스코ICT/SE:서울시/PW:파웤큐브/CT:씨티카/ME:환경부/JD:제주특별자치도청/TM:티맵/HM:현대자동차네비게이션
    char szSid[MAXLEN_ID+1];		//충전소ID
    char szCid[MAXLEN_ID+1];		//충전기ID
    char szType[MAXLEN_ID+1];		//충전기타입(01:DC차데모 /02:승용차AC완속 /03:DC차데모+AC3상 /04:DC콤보 /05:DC차데모+DC콤보 /06:DC차데모+AC3상+DC콤보 /07:AC급속3상
    int	nStatus;					//상태정보(0:알수없음 / 1:통신이상 /2:충전대기 /3:충전중 /4:운영중지 /5:점검중 /6:예약중)
};

struct NstNddsPoiEvStationInfo
{
    int nPoiID;									///< POI ID
    char szStationName[MAXLEN_POINAME+1];		///< 명칭
    char szAddr[MAXLEN_ADDRNAME+1];				///< 주소
    char szRoadAddr[MAXLEN_ROADADDR+1];         ///< 도로명주소
    char szAddrDetail[MAXLEN_ADDRDETAIL+1];     ///< 상세주소
    char szMngName[MAXLEN_MNGNAME+1];           ///< 운영기관
    char szMngId[MAXLEN_MNGID+1];               ///< 기관관리 ID
    char szTel[MAXLEN_PHONENUMBER+1];			///< 연락처
    bool bFreeYn;								///< 주차비무료여부
    bool bReservYn;								///< 예약가능여부
    char szUseTime[MAXLEN_USETIME+1];			///< 이용가능시간
    bool bPayYn;								///< 유료여부
    char szFee[MAXLEN_FEE+1];                   ///< 단가
    char szUpdateDt[MAXLEN_USETIME+1];			///< 충전상태수집일시(YYYYMMDD HH24:MI:SS)
    stVERTEX32	wp;								///< 위치 좌표
    int nTotalCnt;								///< 충전기 갯수
    NstNddsPoiEvStationChargerInfos sEvChargers[MAXLEN_EVSTATION_CHARGERS]; ///<충전기 리스트
};

struct NstNddsPoiEvStations{
    int nPoiID;									///< POI ID
    char szStationName[MAXLEN_POINAME+1];		///< 명칭
    char szAddr[MAXLEN_ADDRNAME+1];				///< 주소
    char szMngName[MAXLEN_MNGNAME+1];           ///< 운영기관
    int	nDist;								///< 거리
    stVERTEX32	wp;								///< 위치 좌표
    int nTotalCnt;								///< 충전기 갯수
    NstNddsPoiEvStationChargerInfos sEvChargers[MAXLEN_EVSTATION_CHARGERS]; ///<충전기 리스트
};


struct NstNddsPoiEvStationsByPoint
{
    int nTotalCnt;								///< 충전기 갯수
    NstNddsPoiEvStations evStations[MAXLEN_EVSTATIONBYPOINT]; ///<충전기 리스트
};

struct NstNddsPoiJoinInfo
{
	int	m_PoiID;										// POI ID
	char m_JoinType[16];			// 가맹점 종류 ("OKCB": OK캐시백, "TMEM": T맴버십, "SSCD": 삼성카드)
	char m_Name[MAXLEN_POINAME+1];			// 시설물 명칭
	char m_CashRate[8];			// 현금 적립율
	char m_CardRate[8];			// 카드 적립율
	char m_WorkTime[16];			// 평일 영업시간
	char m_WeekendWorkTime[16];	// 주말 영업시간
	char m_Url[MAXLEN_URL+1];				// 홈페이지
	char m_Park;			// 주차여부("0":가능, "1":불가능, "" or null : 정보없음)
	char m_Benefit[512];			// 혜택
	char m_UseGuide[512];			// 이용안내
	char m_Information[512];		// 업소 소개
	int m_Item;			// 아이템 리스트 개수
	char m_ItemInfos[10][128];		// 아이템 리스트
};

struct NstNddsRecentDestnation
{
	char m_Name[MAXLEN_POINAME+1];		// 명칭
	int		m_X;
	int		m_Y;
	int m_PoiID;							// POI ID
	char	m_NavSeq[7];				// 입구점번호
	unsigned char m_RPFlag;				// RP Flag
	char m_SvcDate[MAXLEN_SVCDATE+1];		// 최종 서비스일시 (YYYYMMDDHH24MISS)
};

struct NstNddsVmsGuideInfo{
	char m_BaseSdttm[13];
	char m_DataSource[50];
	char m_ContentCode[4];
	char m_ContentName[30];
	char m_Content[128];
};

struct NstNddsExternalUserInfos
{
	char m_Provider[16];
	char m_ReadableUserId[32];
};

struct NstNddsAuthInfo
{
	int m_ValidateCode;
	char m_ValidationSubField[32];
	int m_ExternalUserInfoCount;
	NstNddsExternalUserInfos m_ExternalUserInfos[6];
};

struct NstNddsHeadUnitDeviceAuthInfo
{
	int m_ResultCode;
	int m_ResultSubField;
};

struct NstNddsVersionInfo
{
	char m_MenuNoticeVer[16];
	char m_OpenappVer[16];
};

struct NstNddsAppUpdateInfo
{
	char m_AppUpdateType[4];
	char m_AppVer[16];
	char m_AppUpdateViewFlag[4];
	char m_AppUpdateUrl[512];
	char m_AppUpdateMsg[512];
};

struct NstNddsControlInfoDetails
{
	char m_ServiceName[32];
	char m_ServiceYn[4];
	char m_ServiceItems[512];
};

struct NstNddsAppControlInfo
{
	int m_ControlInfoCount;
	NstNddsControlInfoDetails m_ControlInfoDetails[6]; 
};

struct NstNddsCommonInfo
{
	char m_Euk[256];
	char m_Eid[256];
};

struct NstNddsTidAuthResInfo
{
	int m_ResultCode;
	int m_ResultSubField;
	int m_ResultMdnAuthInfo;
	int m_ExternalUserInfoCount;
	NstNddsExternalUserInfos m_ExternalUserInfos[6];
};

struct NstNddsPushInfo
{
	char m_PushSetYn[4];
	char m_SmsYn[4];
	char m_MomentYn[4];
};

struct NstNddsPersonalInfo
{
	char m_Email[64];
	char m_UserName[32];
	char m_Mdn[16];
	char m_Birth[16];
};

struct NstNddsCarInfo
{
	char m_Model[16];
	char m_Fuel[12];
	char m_HipassYn[16];
	char m_Number[16];
};

struct NstNddsUserProfileInfo
{
	NstNddsPersonalInfo m_PersonalInfo;
	NstNddsCarInfo m_CarInfo;
};

struct NstNddsAuthTmapForSmartCar{
	char m_ErrorCode[7];
	char m_AccessKey[50];
	NstNddsCommonInfo m_CommonInfo;
	NstNddsAuthInfo m_AuthInfo;
	NstNddsHeadUnitDeviceAuthInfo m_HeadUnitDeviceAuthInfo;
	NstNddsTidAuthResInfo m_TidAuthResInfo;
	NstNddsVersionInfo m_VersionInfo;
	NstNddsAppUpdateInfo m_AppUpdateInfo;
	NstNddsAppControlInfo m_AppControlInfo;
	NstNddsPushInfo m_PushInfo;
	NstNddsUserProfileInfo m_UserProfileInfo;
};

// 즐겨찾기 결과 항목
struct NstNddsFavoriteResult{
	char m_CustName[MAXLEN_POINAME+1];			// 명칭
	int m_X;
	int m_Y;
	int m_PoiId;			// POI ID
	char m_NavSeq[7];			// 입구점 일련번호
	char m_RPFlag;			// RP Flag
	char m_AddInfo[MAXLEN_FAVADDINFO+1];			// 부가정보
	char m_InsDatetime[MAXLEN_SVCDATE+1];		// 입력일시 (YYYYMMDDHH24MISS)
};


struct NstNddsMyFavorite{
    char	m_HomeCustName[MAXLEN_POINAME+1];		// 집으로 명칭 (고객이 입력한 명칭)
    int		m_HomeX;
    int		m_HomeY;
    int		m_HomePoiId;							// 집으로 POI ID
    char	m_HomeNavSeq[7];						// 집으로 입구점 일련번호
    char	m_HomeLcdName[MAXLEN_CDNAME+1];			// 집으로
    char	m_HomeMcdName[MAXLEN_CDNAME+1];			// 집으로
    char	m_HomeScdName[MAXLEN_CDNAME+1];			// 집으로
    char	m_HomeDcdName[MAXLEN_CDNAME+1];			// 집으로
    char	m_HomePrimaryBun[MAXLEN_CDNAME+1];		// 본번
    char	m_HomeSecondBun[MAXLEN_CDNAME+1];		// 부번
    int		m_HomeMlClass;							// 산/대지 구분자(0:알수없음,1:대지,2:산)
    char	m_HomeRoadName[MAXLEN_ROADNAME+1];		// 도로명
    char	m_HomeBldNo1[MAXLEN_ROADNAME+1];		// 건물번호1
    char	m_HomeBldNo2[MAXLEN_ROADNAME+1];		// 건물번호2
    char	m_HomeRpFlag;							// 집으로 RP FLAG
    char	m_HomeAddInfo[MAXLEN_FAVADDINFO+1];		// 집으로 부가정보 (행정구역명,좌표)
    char	m_HomeInsDatetime[MAXLEN_SVCDATE+1];	// 집으로 입력일시 (YYYYMMDDHH24MISS)

    char	m_OfficeCustName[MAXLEN_POINAME+1];		// 회사로 명칭 (고객이 입력한 명칭)
    int		m_OfficeX;
    int		m_OfficeY;
    int		m_OfficePoiId;							// 회사로 POI ID
    char	m_OfficeNavSeq[7];						// 회사로 입구점 일련번호
    char	m_OfficeLcdName[MAXLEN_CDNAME+1];		// 회사로
    char	m_OfficeMcdName[MAXLEN_CDNAME+1];		// 회사로
    char	m_OfficeScdName[MAXLEN_CDNAME+1];		// 회사로
    char	m_OfficeDcdName[MAXLEN_CDNAME+1];		// 회사로
    char	m_OfficePrimaryBun[MAXLEN_CDNAME+1];	// 본번
    char	m_OfficeSecondBun[MAXLEN_CDNAME+1];		// 부번
    int		m_OfficeMlClass;						// 산/대지 구분자(0:알수없음,1:대지,2:산)
    char	m_OfficeRoadName[MAXLEN_ROADNAME+1];	// 도로명
    char	m_OfficeBldNo1[MAXLEN_ROADNAME+1];		// 건물번호1
    char	m_OfficeBldNo2[MAXLEN_ROADNAME+1];		// 건물번호2
    char	m_OfficeRpFlag;							// 회사로 RP FLAG
    char	m_OfficeAddInfo[MAXLEN_FAVADDINFO+1];	// 회사로 부가정보 (행정구역명,좌표)
    char	m_OfficeInsDatetime[MAXLEN_SVCDATE+1];	// 회사로 입력일시 (YYYYMMDDHH24MISS)
};

typedef struct NstNddsRouteInfo{
    char		szCustName[MAXLEN_POINAME+1];		// 명칭 (고객이 입력한 명칭)
    stVERTEX32	tp;									// 좌표(sk 정규화 좌표 7자리)
    int			nPoiID;								// POI ID
    char		byRpFlag;
};

struct NstNddsRouteMyFavorite{
    char routeTitle[MAXLEN_TITLE+1];				// 경로 제목
    char insDatetime[MAXLEN_SVCDATE+1];				// 입력시일(YYYYNNDDHH24MISS)
    NstNddsRouteInfo sStartRoute;					// 출발지 정보
    NstNddsRouteInfo sEndRoute;						// 도착지 정보
    NstNddsRouteInfo sMidRoute[3];					// 경유지 정보 리스트(MAX 3건)
};

struct NstNddsPoiFavorite{
    char szName[MAXLEN_POINAME+1];			// 명칭
    stVERTEX32 tp;							// X 좌표
    int nPoiID;								// POI ID
    char szNavseq[7];						// 입구점번호
    char szLcdName[MAXLEN_CDNAME+1];		// 지역대분류
    char szMcdName[MAXLEN_CDNAME+1];		// 지역중분류
    char szScdName[MAXLEN_CDNAME+1];		// 지역소분류
    char szDcdName[MAXLEN_CDNAME+1];		// 지역세분류
    char szPrimaryBun[MAXLEN_CDNAME+1];		// 본번
    char szSecondBun[MAXLEN_CDNAME+1];		// 부번
    int	nMlClass;							// 산/대지 구분자(0:알수없음,1:대지,2:산)
    char szRoadName[MAXLEN_ROADNAME+1];		// 도로명
    char szBldNo1[MAXLEN_ROADNAME+1];		// 건물번호1
    char szBldNo2[MAXLEN_ROADNAME+1];		// 건물번호2
    char szRepClsName[MAXLEN_CATENAME+1];	// 대표업종명
    char szClsAName[MAXLEN_CATENAME+1];		// 업종 대분류명
    char szClsBName[MAXLEN_CATENAME+1];		// 업종 중분류명
    char szClsCName[MAXLEN_CATENAME+1];		// 업종 소분류명
    char szClsDName[MAXLEN_CATENAME+1];		// 업종 세분류명
    char byRPFlag;							// RP Flag
    char szTelNo[MAXLEN_TELNO+1];			// 전화번호
    char szAddInfo[MAXLEN_DETAILINFO+1];	// 부가정보
    char szOrgCustName[MAXLEN_POINAME+1];	// 최초 명칭
    char szIconInfo[MAXLEN_POINAME+1];		// 아이콘 정보
    char szInsDatetime[MAXLEN_SVCDATE+1];	// 입력일시 (YYYYMMDDHH24MISS)
};

struct NstNddsPoiRecent{
    char szCustName[MAXLEN_POINAME+1];		// 명칭
    stVERTEX32 tp;
    int nPoiID;								// POI ID
    char szNavSeq[7];						// 입구점 일련번호
    char szLcdName[MAXLEN_CDNAME+1];		// 지역대분류
    char szMcdName[MAXLEN_CDNAME+1];		// 지역중분류
    char szScdName[MAXLEN_CDNAME+1];		// 지역소분류
    char szDcdName[MAXLEN_CDNAME+1];		// 지역세분류
    char szPrimaryBun[MAXLEN_CDNAME+1];		// 본번
    char szSecondBun[MAXLEN_CDNAME+1];		// 부번
    int	nMlClass;							// 산/대지 구분자(0:알수없음,1:대지,2:산)
    char szRoadName[MAXLEN_ROADNAME+1];		// 도로명
    char szBldNo1[MAXLEN_ROADNAME+1];		// 건물번호1
    char szBldNo2[MAXLEN_ROADNAME+1];		// 건물번호2

    char szRepClsName[MAXLEN_CATENAME+1];	// 대표업종명
    char szClsAName[MAXLEN_CATENAME+1];		// 업종 대분류명
    char szClsBName[MAXLEN_CATENAME+1];		// 업종 중분류명
    char szClsCName[MAXLEN_CATENAME+1];		// 업종 소분류명
    char szClsDName[MAXLEN_CATENAME+1];		// 업종 세분류명
    char byRPFlag;							// RP Flag
    char szTelNo[MAXLEN_TELNO+1];			// 전화번호
    char szTotalCnt[7+1];					// 총 서비스 횟수
    char szSvcDate[MAXLEN_SVCDATE+1];		// 최종서비스일시 (YYYYMMDDHH24MISS)
};

struct NstNddsRouteFavorite{
    char routeTitle[MAXLEN_TITLE+1];		// 경로 제목
    char insDatetime[MAXLEN_SVCDATE+1];		// 입력시일(YYYYNNDDHH24MISS)
    NstNddsRouteInfo sStartRoute;				// 출발지 정보
    NstNddsRouteInfo sEndRoute;					// 도착지 정보
    NstNddsRouteInfo sMidRoute[3];				// 경유지 정보 리스트(MAX 3건)
};

// UserData
struct NstNddsUserData{
    NstNddsMyFavorite		sPoiMyFavorite;
    NstNddsRouteMyFavorite	sRouteMyFavorite;
    int						nPoiRecent;
    int						nPoiFavorite;
    int						nRouteFavorite;
    NstNddsPoiFavorite		sPoiFavorite[100];
    NstNddsPoiRecent		sPoiRecent[100];
    NstNddsRouteFavorite	sRouteFavorite[100];
};

struct NstNddsNonNdds {

};

// 차량 전송 정보
struct NstNddsFavoriteSendToCarInfo{
	int m_PoiId;			// POI ID
	char m_NavSeq[7];			// 입구점 일련번호
	char m_CustName[MAXLEN_SENDTOCARNAME+1];			// 명칭
	int m_X;
	int m_Y;
	char m_addrname[80];
	char m_RPFlag;			// RP Flag
	char m_strTell[16];	//081028
	char m_InsDatetime[MAXLEN_SVCDATE+1];		// 입력일시 (YYYYMMDDHH24MISS)
};

#pragma pack()

#endif // __COMMON_DEF_H__
