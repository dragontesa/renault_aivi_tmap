// ndds_tmap_def.h
#ifndef __NDDS_TMAP_DEF_H__
#define __NDDS_TMAP_DEF_H__

#include "ndds_base_def.h"
#include "../protocol/ndds/jsoncpp/json/json.h"
#include "singleton.h"
#include "classcfg.h"

// NDDS 비동기 업데이트 콜백 함수
typedef int (*LPCALL_ASYNC_NDDSUPDATE_PROC)(eNddsProtocolType eProtocolType, int nSessionID, int nErrCode);

//////////////////////////////////////////////////////////////////////////
// 기초 구조체 정의
//////////////////////////////////////////////////////////////////////////

// 요청 공통 해더
typedef struct tagNddsTmapCommonReq
{
	NDDS_COMMON_REQ	common;
}NDDSTMAP_COMMON_REQ;

// 응답 공통 헤더
typedef struct tagNddsTmapCommonRes
{
	NDDS_COMMON_RES	common;
}NDDSTMAP_COMMON_RES;

//////////////////////////////////////////////////////////////////////////
// "5.1. 길안내" 관련 구조체 정의
//////////////////////////////////////////////////////////////////////////

// - 길안내 요청 공통 정보(common)
typedef struct tagNddsTmapCommonDto{
	char *requestTime;
	int countRoutePlanTypes;
	ENDDS_FIRSTGUIDEOPTION *routePlanTypes;
}NDDSTMAP_COMMON_DTO;

// - 길안내 출발지 정보(departure)
typedef struct tagNddsTmapDepartureDto{
	ENDDS_DEPARTROADTYPE departRoadType;
	char *departName;
	int departXPos;
	int departYPos;
	short angle;
	short speed;
	unsigned char departDirPriority;
	unsigned char departSrchFlag;
}NDDSTMAP_DEPARTURE_DTO;

// - 길안내 목적지 정보(destination) 필드명 타입 필수
typedef struct tagNddsTmapdestinationDto{
    unsigned char destSearchFlag;
	char *destName;
	int destXPos;
	int destYPos;
	char *destPoiId;
	unsigned char destRpFlag;
	char *navSeq;
}NDDSTMAP_DESTINATION_DTO;

// - 경로요청 추가정보(additionalInfo)
typedef struct tagNddsTmapAdditionalInfoDto{
	ENDDS_TOLLCARTYPE	tollCarType;		// 톨케이트 요금 차종
	ENDDS_CAROILTYPE	carOilType;			// 차량 유류 종류
	int countAddCameraTypes;
	int countFareWeightOpts;
	ENDDS_ADDCAMERATYPES addCameraTypes[8];	// 단속카메라 추가 요청
	ENDDS_FAREWEIGHTOPTS* fareWeightOpts;	// 요금 가중치 옵션
	unsigned char	controlRouteReqFlag;		// 통제 길안내 요청 여부
	unsigned char	dynamicRoutePlanSrchFlag;	// 동적 경로탐색 사용여부
	unsigned char	hipassFlag;					// 하이패스 유무
}NDDSTMAP_ADDITIONALINFO_DTO;

// - 지난 위치 정보(pastPositionInfoDto)
typedef struct tagNddsTmapPastPositionInfoDto{
	char *pastSessionId;
	int pastXpos;
	int pastYpos;
	char *pastArrivalTime;
}NDDSTMAP_PASTPOSITIONINFO_DTO;

// - 재탐색 시 필요 데이터(tvasReSearch)
typedef struct tagNddsTmaptvasReSearchDto{
    char *searchCondData;		//탐색조건 데이터
    short lastTollgateId;		//마지막 톨게이트 ID
    int lastRid;				//마지막 매칭 RID
    int tcRid;					//이전경로 정체 RID(TVAS5.2이상에서 사용됨)
    char *preMapVersion;		//이전경로 맵버전
	int preRidsCnt;
    int	*preRids;				//이전경로 RID목록
	int preSecsCnt;
    int *preSecs;				//이전경로 RID의 소요시간 목록
    char *preTollgateIds;		//이전경로 톨게이트 ID데이터
    int	initSrchLength;			//초기탐색 직선거리
    char *initSrchSessionId;	//초기탐색 SessionId;
	unsigned char radiusInfoType;
	short radiusEntryTurnType;
	int	radiusLinkId;
	int	radiusMeshCode;
	unsigned char radiusLinkDirection;
	short radiusLeaveTurnType;
	int radiusXPos;
	int	radiusYPos;
}NDDSTMAP_TVASRESEARCH_DTO;

// - 경유지 목록(wayPoints)
typedef struct tagNddsTmapWayPointsDto{
	int x;					// 경유지 X 좌표
	int y;					// 경유지 Y 좌표
	char *poiID;			// 경유지 POI ID
	unsigned char rpFlag;			// 경유지 RP Flag
}NDDSTMAP_WAYPOINTS_DTO;

// 5.1.3. 길안내 (요청)
typedef struct tagNddsTmapPlanningRouteReq
{
	NDDSTMAP_COMMON_DTO common;
	char *tvas;
	NDDSTMAP_DEPARTURE_DTO departure;
	NDDSTMAP_DESTINATION_DTO destination;
	int nGpsTraceDataSize;		// gpsTraceData 크기
	char *gpsTraceData;
	NDDSTMAP_ADDITIONALINFO_DTO additionalInfo;
	int countWayPoints;
	NDDSTMAP_WAYPOINTS_DTO *wayPoints;
	ENDDS_SERVICE_FLAG serviceFlag;
	char *patternTime;
	ENDDS_PATTERNWEEK patternWeek;
    char guideImgResolutionCode[10];
    int countdangerAreaOptions;
	ENDDS_DANGERAREAOPTIONS dangerAreaOptions[8];
	ENDDS_FIRSTGUIDEOPTION firstGuideOption;
	NDDSTMAP_PASTPOSITIONINFO_DTO pastPositionInfoDto;
	ENDDS_ROUTE_TYPE ReSearchType;		
	NDDSTMAP_TVASRESEARCH_DTO  tvasReSearch;
	unsigned char routePlanAroundRange;
}NDDSTMAP_PLANNINGROUTE_REQ;

// - 상세위치요구 응답 데이터(departureMatch)
typedef struct tagNddsTmapdepartureMatchDto
{
	int roadCount;	// int(4)	상세위치 확인 도로개수
	unsigned char roadType;	// unsigned char(1)	상세위치 확인 도로타입
}NDDSTMAP_DEPARTUREMATCH_DTO;

// - 목적지 정보 (destination)
typedef struct tagNddsTmapDestinationResDto
{
	unsigned char compressFlag;
	char *destPoiId;
	unsigned char destRpFlag;
	unsigned char departCoordType;
	int	destXPos;
	int	destYPos;
	unsigned char destNameSize;
	unsigned char tvasCount;
	char *destName;
	char skyCode[8];	// 하늘 상태 코드(날씨 코드)
	char skyName[30];	// 하늘 상태 코드명(날씨 명)
	char rainTypeCode[2]; // 강수 형태 코드(0:현상없음, 1:비, 2:비/눈, 3:눈)
	char rainSinceOnTime[7]; //1시간 누적 강수량
	char tempC1h[6];	// 1시간 기온
}NDDSTMAP_DESTINATION_RES_DTO;

// - 경로 응답 참조 정보 (tavsRefs)
typedef struct tagNddsTmaptavsRefsDto
{
	unsigned char routePlanTypes;
	unsigned char fareWeightOpt;
	char *routeSummaryCode;
	int tvasSize;
	unsigned char *tvasData;
}NDDSTMAP_TAVSREFS_DTO;


// 5.1.3. 길안내 (응답)
typedef struct tagNddsTmapPlanningRouteRes
{
	NDDS_COMMONHEADER_RES	header;		// 공통 해더
	NDDS_MEMORYSTATUS		memory;		// 메모리 관리

	int roadCount;	// int(4)	상세위치 확인 도로개수
	unsigned char roadType;	// unsigned char(1)	상세위치 확인 도로타입

	unsigned char routeCount;			// 총경로 개수
	int	*routePlanTypes;		// 경로 탐색 옵션
	int *fareWeightOpt;			// 요금 가중치 코드 (1:유/무료 경로 제공, 2:최적요금 경로 제공, 8:무료우선 경로 제공, 16:로직에 의한 판단 경로(Default))
	int *routeSummaryCode;	// (00000 : 경로요약 데이터가 정상인 경우, EG002 : 단거리 등의 이유로 정상적인 경우의 결과코드)
	NDDSTMAP_DESTINATION_RES_DTO destination;

	int *tvasSize;				// 경로 데이터의 크기
	unsigned char **tvases;
}NDDSTMAP_PLANNINGROUTE_RES;

// - 길안내 요약 구간 정보 리스트(summaryList)
typedef struct tagNddsTmapSummaryListDto
{
	int guideType;			// 안내 속성 데이터 ID
	char *summaryName;		// 요약 명칭 데이터
	int recmdRouteLen;		// 구간거리(단위:m, 올림:1의자리)
	int recmdRouteTime;		// 구간시간 (단위:초, 올림:1의자리)
	int	speed;				// 교통수집 속도
	unsigned char congestion;		// 교통수집 혼잡도
}NDDSTMAP_SUMMARYLIST_DTO;

// - 경로 요약 정보 리스트(routeList)
typedef struct tagNddsTmapRouteListDto
{
	int	totalLength;					// 경로 총거리 (m 단위)
	int totalTime;						// 경로 총 소요시간 (초 단위)
	int totalTollgateCost;				// 톨게이트 요금정보 (원 단위)
	int	totalTaxiCost;					// 택시요금 (단위:원)
	int	estimationOption;				// 예측구분코드
	char *estimationTime;				// 출발/도착예측시간(YYYYMMDDHHMMSSS)
	int departRoadType;					// 출발지 도로 종별
	char *mainRoad;						// 전체 경로 내 주요 도로 정보 [Frome 도로명]-[To 도로명]
	int vertexFlag;						// 경로 좌표 요청 구분
	int nSummaryList;					// 길안내 요약 구간 정보 리스트 개수
	NDDSTMAP_SUMMARYLIST_DTO *summaryList;	// 길안내 요약 구간 정보 리스트
	char *vertexCoord;					// Vertex 좌표 리스트
}NDDSTMAP_ROUTELIST_DTO;

// 5.1.4. 경로요약 (요청)
typedef struct tagNddsTmapRouteSummaryInfoReq
{
	int firstGuideOption;				//[Y] 경로탐색옵션 ID  (enum 아님)
	int serviceFlag;					//[Y] 경로 서비스 구분
	char *commingTime;					//[Y] 출발/도착예측시간(YYYYMMDDHHMMSS)		
	int vertexFlag;						//[Y] 경로 좌표 요청 구분
	char *departName;					//[Y] 출발지 명칭				
	int departXPos;						//[Y] 출발지 X 좌표
	int departYPos;						//[Y] 출발지 Y 좌표
	short angle;						//[Y] 각도 (진북 기준, 0~359) * 모를경우 -1
	short speed;						//[Y] 속도(차량진행속도(Km/h)
	unsigned char departDirPriority;			//[Y] 출발지점 주행방향
	unsigned char departSrchFlag;				//[Y] 출발지 탐색 구분
	char *destName;						//[Y] 목적지 명칭
	int destXPos;						//[Y] 목적지 X 좌표
	int destYPos;						//[Y] 목적지 Y 좌표
	unsigned char destRpFlag;					//[Y] 목적지 RP Flag
	unsigned char destSearchFlag;				//[Y] 목적지 탐색 구분 ID
	char *destPoiId;					//[Y] 목적지 POI ID
	int	nWayPoints;						//[Y] 경유지 목록
	NDDSTMAP_WAYPOINTS_DTO *wayPoints;	//[Y] 경유지 목록
}NDDSTMAP_ROUTESUMMARYINFO_REQ;

// 5.1.3. 길안내 (응답)
typedef struct tagNddsTmapRouteSummaryInfoRes
{
	NDDS_COMMONHEADER_RES	header;		// 공통 해더
	NDDS_MEMORYSTATUS		memory;		// 메모리 관리

	int nRouteList;						// 경로 요약 정보 리스트 개수
	NDDSTMAP_ROUTELIST_DTO *routeList;	// 경로 요약 정보 리스트
}NDDSTMAP_ROUTESUMMARYINFO_RES;

//////////////////////////////////////////////////////////////////////////
// "5.2. POI 검색" 관련 구조체 정의
//////////////////////////////////////////////////////////////////////////
typedef struct tagNddsTmapGroupSubListsDto
{
	char *subSeq;		// 하부-순번 (그룹인 경우  1 이상의 값이 존재)
	char *subName;		// 하부-명칭
	char *subNavX;			// 하부-시설물입구X좌표
	char *subNavY;		// 하부-시설물입구Y좌표
	unsigned char subRpFlag;		// 하부-RP FLAG
	char *subPoiId;		// 하부-POI ID
	char *subNavSeq;		// 하부-입구점번호
}NDDSTMAP_GROUPSUBLISTS_DTO;

// 광고 음원 컨텐츠 정보 리스트 정보
typedef struct tagNddsTmapAdvtDetailsDto
{
	char *regDate;		// 등록일(YYYYMMDDHHMI)
	char *adType;		// 광고 타입 “IN10”: 제휴사 광고(검색키워드 배너광고)
	char *adCode;			// 광고 코드
	char *adKindCd;		// 광고종류  구분코드 "2701“: 광고, “2703“: 컨텐츠
	char *adStartDate;		// 광고 시작일자(YYYYMMDDHH24MI)
	char *adEndDate;		// 광고 종료일자(YYYYMMDDHH24MI)
	char *downloadURL;		// 광고 배너 이미지 다운로드 URL
	char *linkURL;			// Link URL
}NDDSTMAP_ADVTDETAILS_DTO;

// 통합검색 공통 POI 정보
typedef struct tagNddsTmapPoiSearchesDto
{
	char *dbkind;		// 시설물 DB ("S": SK 데이터, "A": 주소)
	char *poiId;		// POI ID
	char *name;			// 시설물 명칭
	char *orgName;		// 명칭 알리아스
	char *navX1;		// 시설물 입구 X좌표
	char *navY1;		// 시설물 입구 Y좌표
	char *centerX;		// 시설물 중심 X좌표 
	char *centerY;		// 시설물 중심 Y좌표 
	unsigned char rpFlag;		// RP FLAG
	char *parkYn;		// 주차여부	("0":가능,"1":불가능, NULL: 정보없음)  
	char *phoneNum;		// 전화번호 
	char *addr;			// 주소 
	char *primary;		// 본번 
	char *secondary;	// 부번 
	char *mlClass;		// 산/대지 구분자("0":알수없음,"1":대지,"2":산 )
	char *lcdName;		// 표출주소 대분류 
	char *mcdName;		// 표출주소 중분류 
	char *scdName;		// 표출주소 소분류 
	char *dcdName;		// 표출주소 세분류 
	char *dataKind;		// 데이터 구분자("3":주유소,"4":충전소)
	char *stId;			// 주유소브랜드	("SK","GS","기타",...)
	char *highHhSale;	// 고급휘발유판매점여부("0":판매안함,"1":판매)
	char *minOilYn;		// 최저가점포여부("1":최저가 , 0:아님)
	char *oilBaseSdt;	// 기준일 (YYYYMMDD)
	long hhPrice;		// 휘발유가격
	long ggPrice;		// 경유가격
	long llPrice;		// LPG가격 
	long highHhPrice;	// 프리미엄 휘발유 가격 
	long highGgPrice;	// 프리미엄 LPG 가격 
	char *radius;		// 거리 
	char *roadName;		// 도로명 (새주소)
	char *bldNo1;		// 건물번호1 (새주소)
	char *bldNo2;		// 건물번호2 (새주소)
	int publicTrafficType; // 대중교통 구분(0:없음, 1:버스정류장, 2:지하철)
	char *sid;			// 대중교통 구분이 1이면 버스정류장 ID, 대중교통 구분이 2이면 지하철역 ID
	char *navSeq;		// 입구점번호
	NDDSTMAP_GROUPSUBLISTS_DTO *groupSubLists; //그룹 하부 리스트
	NDDSTMAP_ADVTDETAILS_DTO *advertises; //광고정보 결과 리스트
}NDDSTMAP_POISEARCHES_DTO;

// POI 이미지 정보
typedef struct tagNddsTmapPoiImageInfosDto
{
	int seq;			// 이미지 순번
	char *fileSrc;		// POI 이미지 경로 
}NDDSTMAP_POIIMAGEINFOS_DTO;

// 제휴 주차장 정보
typedef struct tagNddsTmapPoiParkInfosDto
{
	char *cpName;		// 제공처명 예) “모두의주차장”, “파킹박”, ...
	char *mobileLinkUrl;// 모바일 웹 링크 URL
}NDDSTMAP_POIPARKINFOS_DTO;

// 광고정보 결과 리스트
typedef struct tagNddsTmapAdvertisesDto
{
	char *adType;		//광고 타입
	char *adSvcId;		// 광고 코드(광고서비스 id)
	char *adInfoType;	// 광고정보 구분(“01”: OK캐쉬백, “02”: 광고)
	char *adTitle;		// 광고제목
	char *startSdttm;	// 시작 서비스일시. YYYYMMDD+000000
	char *endSdttm;		// 종료 서비스일시. YYYYMMDD+000000
	char *linkUrl;		// Link URL
	char *iconImg;		// 아이콘 이미지명
}NDDSTMAP_ADVERTISES_DTO;

// 가맹점 상세정보
typedef struct tagNddsTmapPoiJoinInfoViewsDto
{
	char *poiId;			// POI ID
	char *joinType;			// 가맹점 종류 ("OKCB": OK캐시백, "TMEM": T맴버십, "SSCD": 삼성카드)
	char *poiName;			// 시설물 명칭
	char *cashRate;			// 현금 적립율
	char *cardRate;			// 카드 적립율
	char *workTime;			// 평일 영업시간
	char *weekendWorkTime;	// 주말 영업시간
	char *http;				// 홈페이지
	char *parkYn;			// 주차여부("0":가능, "1":불가능, "" or null : 정보없음)
	char *benefit;			// 혜택
	char *useGuide;			// 이용안내
	char *information;		// 업소 소개
	int nItem;			// 아이템 리스트 개수
	char **itemInfos;		// 아이템 리스트
}NDDSTMAP_POIJOININFOVIEWS_DTO;

// POI 최근길 정보
typedef struct tagNddsTmapPoiRecentsDto
{
    char *custName;			// 명칭
    char *noorX;			// X 좌표
    char *noorY;			// Y 좌표
    char *poiId;			// POI ID
    char *navSeq;			// 입구점번호
    char *lcdName;			// 지역 대분류명
    char *mcdName;			// 지역 중분류명
    char *scdName;			// 지역 소분류명
    char *dcdName;			// 지역 세분류명
    char *primaryBun;		// 본번
    char *secondaryBun;		// 부번
    char *mlClass;			// 산/대지 구분자(null:0,1:대지, 2:산)
    char *roadName;			// 도로명
    char *bldNo1;			// 건물번호1
    char *bldNo2;			// 건물번호2
    char *repClsName;		// 대표 업종명
    char *clsAName;			// 업종 대분류명
    char *clsBName;			// 업종 중분류명
    char *clsCName;			// 업종 소분류명
    char *clsDName;			// 업종 상세분류명
    unsigned char rpFlag;			// RP Flag
    char *telNo;			// 전화번호
    char *totalCnt;			// 총 서비스 횟수.
    char *svcDate;			// 최종 서비스일시 (YYYYMMDDHH24MISS)
}NDDSTMAP_POIRECENTS_DTO;

// POI 최근길 정보
typedef struct tagNddsTmapPoiRecentUploadDto
{
	char *custName;			// 명칭
	char *noorX;			// X 좌표
	char *noorY;			// Y 좌표
	char *poiId;			// POI ID
	char *navSeq;			// 입구점 일련번호
	unsigned char rpFlag;			// RP Flag
	char *totalCnt;			// 총 서비스 횟수. 요청의 svcUploadYn이 “Y”면 필수
    char *svcDate;			// 최종 서비스일시. 요청의 svcUploadYn이 “Y”면 필수
}NDDSTMAP_POIRECENTUPLOAD_DTO;

// POI 최근길 삭제 정보
typedef struct tagNddsTmapPoiRecentRemoveDto
{
    char *custName;			// 명칭
    char *noorX;			// X 좌표
    char *noorY;			// Y 좌표
}NDDSTMAP_POIRECENTREMOVE_DTO;

// POI 좌표 검색용 정보
typedef struct tagNddsTmapNearbyPoiDto
{
	char *dbkind;			// 시설물DB ("S": SK 데이터, "A": 주소)
	char *poiId;			// POI ID
	char *name;				// 시설물 명칭
	char *orgName;			// 명칭 알리아스
	char *navX1;			// 시설물입구 X좌표
	char *navY1;			// 시설물입구 Y좌표
	char *centerX;			// 시설물중심 X좌표
	char *centerY;			// 시설물중심 Y좌표
	unsigned char rpFlag;			// RP FLAG
	char *parkYn;			// 주차여부	("0":가능,"1":불가능, NULL: 정보없음)  
	char *telNo;			// 전화번호
	char *addr;				// 주소
	char *primary;			// 본번
	char *secondary;		// 부번
	char *mlClass;			// 산/대지구분("0":알수없음,"1":대지, "2":산)
	char *lcdName;			// 표출주소 대분류
	char *mcdName;			// 표출주소 중분류
	char *scdName;			// 표출주소 소분류
	char *dcdName;			// 표출주소 세분류
	char *roadName;			// 도로명 (새주소)
	char *bldNo1;			// 건물번호1 (새주소)
	char *bldNo2;			// 건물번호2 (새주소)
	char *radius;			// 거리
}NDDSTMAP_NEARBYPOI_DTO;

// POI Pickat용 정보
typedef struct tagNddsTmapPickatPoiDto
{
	char *pickatPoiId;		// Pickat POI ID
	char *name;				// 시설물 명칭
	char *centerX;			// 시설물중심 X좌표
	char *centerY;			// 시설물중심 Y좌표
	char *addr;				// 주소
	int pickCount;			// pick 개수
	int totalPosts;			// 포트스 카운트
	int emoGoodCount;		// 감성평가 좋아요 카운트
	int blogReviewCount;	// 블로그 리뷰 카운트
	int score;				// POI 점수
	unsigned char couponExist;		// 쿠폰 보유 여부 (true / false)
}NDDSTMAP_PICKATPOI_DTO;

// 새주소(도로명) 검색
typedef struct tagNddsTmapRoadNameDto
{
	char *roadName;			// 도로명
}NDDSTMAP_ROADNAME_DTO;

// 주소명칭 리스트
typedef struct tagNddsTmapPoiAreaCodesDto
{
	char *areaName;			// 주소명	(해당 필드의 응답 규격은 위 Request Format에 ※ 기호 부분의 설명을 참조.)
}NDDSTMAP_POIAREACODES_DTO;

// ev충전소 충전기리스트
typedef struct tagNddsTmapPoiEvStationChargerInfoDto
{
    char *bid;			//기관코드 BG:비긴스/JE:제주전기자동차서비스/KP:한국전력/HE:한국전기차충전서비스/PI:포스코ICT/SE:서울시/PW:파웤큐브/CT:씨티카/ME:환경부/JD:제주특별자치도청/TM:티맵/HM:현대자동차네비게이션
    char *sid;			//충전소ID
    char *cid;			//충전기D
    char *type;			//충전기타입(01:DC차데모 /02:승용차AC완속 /03:DC차데모+AC3상 /04:DC콤보 /05:DC차데모+DC콤보 /06:DC차데모+AC3상+DC콤보 /07:AC급속3상
    char *status;		//상태정보(0:알수없음 / 1:통신이상 /2:충전대기 /3:충전중 /4:운영중지 /5:점검중 /6:예약중)
}NDDSTMAP_POIEVSTATIONCHARGERINFO_DTO;

// ev충전소 충전기리스트
typedef struct tagNddsTmapPoiEvStationsDto
{
    char *poiId;						// Poi Id
    char *stationName;					// 충전소명
    char *addr;                         // 지번주소
    char *mngName;                      // 운영기관
    char *noorX;						// 충전소 X좌표
    char *noorY;						// 충전소 Y좌표
    char *radius;                       // 거리(km 단위)
    int  totalCnt;                      // 충전기 갯수
    NDDSTMAP_POIEVSTATIONCHARGERINFO_DTO *evChargers; // 충전기 리스트
}NDDSTMAP_POIEVSTATIONS_DTO;

// Pickat POI 상세정보 조회
typedef struct tagNddsTmapCoordinatesDto
{
	char *coordinates;			// 좌표. WGS84 (coordinates[0] : longitude, [1]: latitude)
}NDDSTMAP_COORDINATES_DTO;

typedef struct tagNddsTmapPickImagesDto
{
	char *pickImages;			// 썸네일 이미지 Url 리스트
}NDDSTMAP_PICKIMAGES_DTO;

typedef struct tagNddsTmapPicksDto
{
	char *pickId;			// Pick Id
	char *contents;			// 픽 리뷰
	char *createdAt;		// 생성시간(format:rfc3339(tz:UTC))
	char *editorUserId;		// 작성자 ID
	char *editorNickName;	// 작성자 닉네임
	char *editorProfileImage;// 사용자 프로필 이미지 url
	char *themeId;			// 테마 ID
	char *themName;			// 테마 명칭
}NDDSTMAP_PICKS_DTO;


// 5.2.1 통합 검색 (요청)
typedef struct tagNddsTmapFindPoisReq
{
	char *searchTypCd;					//[Y] 검색종류 ("A": 정확도순, "R": 거리순)
	int reqCnt;							//[Y] 요청건수
	int reqSeq;							//[Y] 페이지 번호
	char *name;							//[Y] 검색문자 시설물 명칭
	char *areaName;						//[N] 지역설정 문자 - 예) 서울 중구
	char *noorX;						//[O] X좌표 (검색종류가 거리순 일 경우 필수)
	char *noorY;						//[O] Y좌표 (검색종류가 거리순 일 경우 필수)
	char *radius;						//[O] 기본값: "0" 센터에 위임함. 꼭 반경을 정하고자 하는 경우가 아니면 "0"으로 입력한다. (단위 "1" -> 300m)
	char *poiGroupYn;					//[N] POI 그룹핑정보 유무 (Y/N) 기본은 N
}NDDSTMAP_FINDPOIS_REQ;
// 5.2.1 통합 검색 (응답)
typedef struct tagNddsTmapFindPoisRes
{
	NDDS_COMMONHEADER_RES	header;		// 공통 해더
	NDDS_MEMORYSTATUS		memory;		// 메모리 관리

	int totalCnt;						// 총개수
	int listCnt;						// 목록개수
	char *countFlag;					// 계속 구분 ("0": 다음 페이지 없음, "1": 다음 페이지 존재)
	int areaCnt;						// 지역건수
	char *areaNm;						// 지역명 (";"로 구분) - 예) "서울;부산;대구;인천;광주"
	NDDSTMAP_POISEARCHES_DTO *poiSearches;	// POI 정보
	int advtCount;							// 광고 정보 개수(Tmap 광고서버 연동)
	NDDSTMAP_ADVTDETAILS_DTO *advtDetails;  //광고 정보 리스트(Tmap 광고서버 연동)
}NDDSTMAP_FINDPOIS_RES;


// 5.2.2 주변지 검색 (요청)
typedef struct tagNddsTmapFindAroundPoisReq
{
	int reqCnt;							//[Y] 요청건수
	int reqSeq;							//[Y] 페이지 번호
	char *name;							//[Y] 검색문자 시설물 명칭
	char *noorX;						//[Y] 현위치 X좌표
	char *noorY;						//[Y] 현위치 Y좌표
	char *radius;						//[Y] 기본값: "0" 센터에 위임함. 꼭 반경을 정하고자 하는 경우가 아니면 "0"으로 입력한다. (단위 "1" -> 300m)
}NDDSTMAP_FINDAROUNDPOIS_REQ;
// 5.2.2 주변지 검색 (응답)
typedef	struct tagNddsTmapFindAroundPoisRe
{
	NDDS_COMMONHEADER_RES	header;		// 공통 해더
	NDDS_MEMORYSTATUS		memory;		// 메모리 관리

	int totalCnt;						// 총개수
	int listCnt;						// 목록개수
	char *countFlag;					// 계속 구분 ("0": 다음 페이지 없음, "1": 다음 페이지 존재)
	int areaCnt;						// 지역건수
	char *areaNm;						// 지역명 (";"로 구분) - 예) "서울;부산;대구;인천;광주"
	NDDSTMAP_POISEARCHES_DTO *poiSearches;	// POI 정보
}NDDSTMAP_FINDAROUNDPOIS_RES;


// 5.2.3. POI 상세정보 (요청)
typedef struct tagNddsTmapFindPoiDetailInfoReq
{
	char *poiId;						//[Y] POI ID
	char *navSeq;						//[N] 입구점 일련번호
	char *sizeWidth;					//[N] 화면 가로 사이즈
	char *sizeHeight;					//[N] 화면 세로 사이즈
}NDDSTMAP_FINDPOIDETAILINFO_REQ;
// 5.2.3. POI 상세정보 (응답)
typedef struct tagNddsTmapFindPoiDetailInfoRes
{
	NDDS_COMMONHEADER_RES	header;		// 공통 해더
	NDDS_MEMORYSTATUS		memory;		// 메모리 관리

	char *poiId;						// POI ID
	char *navSeq;						// 입구점 일련번호
	char *grpId;						// 시설물 그룹 ID
	char *viewId;						// 업종별 분기 구분자("3": 주유소, "4":충전소)
	char *name;							// 시설물 명칭
	char *cateNm;						// 업종명
	char *addr;							// 주소
    char *lcdName;						// 지역 대분류명
    char *mcdName;						// 지역 중분류명
    char *scdName;						// 지역 소분류명
    char *dcdName;						// 지역 세분류명
	char *primaryBun;					// 본번
	char *secondaryBun;					// 부번 
	char *mlClass;						// 산/대지 구분자 ("0": 알수없음, "1":대지, "2":산)
    char *bldAddr;						// 건물주소 표출용 주소 Ex) 서울 중구 세종대로
    char *roadName;						// 도로명
    char *bldNo1;						// 건물번호1
    char *bldNo2;						// 건물번호2
	char *navX1;						// 입구점 X 좌표
	char *navY1;						// 입구점 Y 좌표
	char *centerX;						// X 좌표
	char *centerY;						// Y 좌표
    unsigned char rpFlag;				// RP Flag
	char *zipCd;						// 우편번호
	char *menu1;						// 메뉴1
	char *menu2;						// 메뉴2
	char *menu3;						// 메뉴3
	char *menu4;						// 메뉴4
	char *menu5;						// 메뉴5
    int nCateImageSize;                 // 업종대표 이미지 사이즈
	char *cateImage;					// 업종대표 이미지, POI 이미지 정보에서 첫 번째 이미지 정보를 Base64로 encoding한 데이터
	char *telNo;						// 전화번호
	char *parkYn;						// 주차 가능 여부 ("1":가능, "0":불가능)
	char *holidayN;						// 연중무휴 Flag("1":연중무휴)
	char *http;							// 홈페이지
	char *road;							// 찾아가는 길
	char *recommPoint;					// 좋아요 평가 점수
	char *iconType;						// 아이콘 표시용 표출업종코드
	char *joinStoreYn;					// 가맹점여부정보  해당 가맹점 여부 정보 세미콜론(;) 구분자 표시
	char *joinViewType;					// 가맹점 화면 Type	가맹점여부정보와 1:1 대응하며 세미콜론(;) 구분자로 나열하여 표시
	char *highHhSale;					// 고급휘발유판매점여부 ("0":판매안함, "1":판매)
	char *oilBaseSdt;					// 기준일 (YYYYMMDD)
	long hhPrice;						// 휘발유 가격(원)
	long ggPrice;						// 경유 가격(원)
	long llPrice;						// LPG 가격(원)
	long highHhPrice;					// 프리미엄 휘발유 가격(원)
	long highGgPrice;					// 프리미엄 경유 가격(원)
	char *addInfo;						// 부가정보
	char *infoLen;						// 업소 소개 길이
	char *infomation;					// 업소 소개
	char *dayOffInfo;					// 휴무정보
	char *businessHours;				// 영업시간
	char *dayOffDate;					// 휴무일자
	int nPoiImageInfos;					// POI 이미지 정보 개수
	NDDSTMAP_POIIMAGEINFOS_DTO *poiImageInfos;	// POI 이미지 정보 리스트
	int nPoiParkInfos;					// 제휴 주차장 정보 개수
	NDDSTMAP_POIPARKINFOS_DTO *poiParkInfos; // 제휴 주차장 정보
	int nAdvertises;					// 광고정보 결과 리스트 개수
	NDDSTMAP_ADVERTISES_DTO *advertises; // 광고정보 결과 리스트 
}NDDSTMAP_FINDPOIDETAILINFO_RES;

// 5.2.4. 가맹점 상세정보 (요청)
typedef struct tagNddsTmapFindPoiJoinInfoReq
{
	char *poiId;						//[Y] POI ID
}NDDSTMAP_FINDPOIJOININFO_REQ;
// 5.2.4. 가맹점 상세정보 (응답)
typedef struct tagNddsTmapFindPoiJoinInfoRe
{
	NDDS_COMMONHEADER_RES	header;		// 공통 해더
	NDDS_MEMORYSTATUS		memory;		// 메모리 관리

	int nPoiJoinInfoViews;							// 가맹점 상세정보 개수
	NDDSTMAP_POIJOININFOVIEWS_DTO *poiJoinInfoViews;	// 가맹점 상세정보 리스트
}NDDSTMAP_FINDPOIJOININFO_RES;



// 5.2.5. POI 최근길 조회 (요청)
typedef struct tagNddsTmapFindRecentDestnationReq
{
}NDDSTMAP_FINDRECENTDESTNATION_REQ;
// 5.2.5. POI 최근길 조회 (응답)
typedef struct tagNddsTmapFindRecentDestnationRes
{
	NDDS_COMMONHEADER_RES	header;		// 공통 해더
	NDDS_MEMORYSTATUS		memory;		// 메모리 관리

	int nPoiRecents;					// POI 최근길정보 개수
	NDDSTMAP_POIRECENTS_DTO *poiRecents;	// POI 최근길정보 리스트
}NDDSTMAP_FINDRECENTDESTNATION_RES;


// 5.2.6. POI 최근길 전체 업로드 (요청)
typedef struct tagNddsTmapUploadRecentDestnationReq
{
	char *svcUploadYn;								//[N] 서비스 속성정보 업로드 여부(“Y”/”N”). default: “N”,  “Y”이면 단말의 totalCnt, svcDate 를 서버에 저장한다. “N”이면 totalCnt 초기화, svcDate는 서버기준으로 저장.
	int nPoiRecentUploads;							//[Y] POI 최근길정보 리스트 개수
    NDDSTMAP_POIRECENTUPLOAD_DTO *poiRecentUploads;	//[Y] POI 최근길정보 리스트
}NDDSTMAP_UPLOADRECENTDESTNATION_REQ;
// 5.2.6. POI 최근길 전체 업로드 (응답)
typedef struct tagNddsTmapUploadRecentDestnationRes
{
	NDDS_COMMONHEADER_RES	header;		// 공통 해더
	NDDS_MEMORYSTATUS		memory;		// 메모리 관리

}NDDSTMAP_UPLOADRECENTDESTNATION_RES;

// 5.2.7. POI 최근길 멀티 삭제 (요청)
typedef struct tagNddsTmapRemoveRecentDestnationReq
{
    int nRemovePoiRecentCnt;						//[Y] POI 최근길정보 리스트 개수
    NDDSTMAP_POIRECENTREMOVE_DTO *poiRecentRemoves;	//[Y] POI 최근길정보 리스트
}NDDSTMAP_REMOVERECENTDESTNATION_REQ;

// 5.2.7. POI 최근길 멀티 삭제 (응답)
typedef struct tagNddsTmapRemoveRecentDestnationRes
{
    NDDS_COMMONHEADER_RES	header;		// 공통 해더
    NDDS_MEMORYSTATUS		memory;		// 메모리 관리
}NDDSTMAP_REMOVERECENTDESTNATION_RES;

// 5.2.7. 좌표로 지명 검색 (요청)
typedef struct tagNddsTmapFindNearbyReq
{
	int reqCnt;							//[Y] 요청건수
	int reqSeq;							//[Y] 페이지 번호
	char *noorX;						//[Y] 현위치 X좌표
	char *noorY;						//[Y] 현위치 Y좌표
	char *radius;						//[Y] 검색반경("0" ~ "33", 단위 "1" -> 300m) 기본값:"0" 센터에 위임
	char *addrDbYn;						//[Y] 주소DB 검색 여부(Y/N) (기본적으로 "Y" 사용)
	char *poiDbYn;						//[Y] POI DB 검색 여부(Y/N) (기본적으로 "Y" 사용)
}NDDSTMAP_FINDNEARBY_REQ;
// 5.2.7. 좌표로 지명 검색 (응답)
typedef struct tagNddsTmapFindNearbyRes
{
	NDDS_COMMONHEADER_RES	header;		// 공통 해더
	NDDS_MEMORYSTATUS		memory;		// 메모리 관리

	int totalCnt;						// 총개수
	int listCnt;						// 목록개수
	char *contFlag;						// 계속 구분 ("0": 다음 페이지 없음, "1": 다음 페이지 존재)
	NDDSTMAP_NEARBYPOI_DTO *poiSearches;	// 아래 POI 정보 참조
}NDDSTMAP_FINDNEARBY_RES;


// 5.2.8. Pickat 인기검색어 조회 (요청)
typedef struct tagNddsTmapFindPickatPopularsReq
{
	int reqCnt;							//[Y] 요청건수 (max 100건)
	char *noorX;						//[Y] X좌표
	char *noorY;						//[Y] Y좌표
	char *radius;						//[Y] 검색반경 (km단위) 기본값:1.0
}NDDSTMAP_FINDPICKATPOPULARS_REQ;
// 5.2.8. Pickat 인기검색어 조회 (응답)
typedef struct tagNddsTmapFindPickatPopularsRes
{
	NDDS_COMMONHEADER_RES	header;		// 공통 해더
	NDDS_MEMORYSTATUS		memory;		// 메모리 관리

	int listCnt;						// 목록개수
	NDDSTMAP_PICKATPOI_DTO *poiSearches;	// Pickat POI 정보 리스트
}NDDSTMAP_FINDPICKATPOPULARS_RES;

// 5.2.9. 새주소(도로명) 검색 (요청)
typedef struct tagNddsTmapFindRoadNameReq
{
	int reqCnt;							//[Y] 요청건수 (max 100건)
	int reqSeq;							//[Y] 페이지 번호(1~200)
	char *largeCodeName;				//[Y] 주소 대분류 명, 예) 서울
	char *middleCodeName;				//[Y] 주소 중분류 명, 예) 중구
}NDDSTMAP_FINDROADNAME_REQ;
// 5.2.9. 새주소(도로명) 검색  (응답)
typedef struct tagNddsTmapFindRoadNameRes
{
	NDDS_COMMONHEADER_RES	header;		// 공통 해더
	NDDS_MEMORYSTATUS		memory;		// 메모리 관리

	int totalCnt;						// 총개수
	int listCnt;						// 목록개수
	char *contFlag;						// 계속 구분
	NDDSTMAP_ROADNAME_DTO *poiRoadNames;	// 새주소(도로명) 리스트
}NDDSTMAP_FINDROADNAME_RES;

// 5.2.10. Pickat POI 상세정보 조회 (요청)
typedef struct tagNddsTmapFindPickatPoiDetailReq
{
	char *poiId;						//[Y] POI ID (Tmap POI ID)
}NDDSTMAP_FINDPICKATPOIDETAIL_REQ;
// 5.2.10. Pickat POI 상세정보 조회 (응답)
typedef struct tagNddsTmapFindPickatPoiDetailRes
{
	NDDS_COMMONHEADER_RES	header;		// 공통 해더
	NDDS_MEMORYSTATUS		memory;		// 메모리 관리

	char *poiId;						// Pickat POI ID (모바일 Web 페이지 호출시 필요)
	char *tmapId;						// Tmap POI ID
	char *poiName;						// Poi 명칭
	char *address;						// 주소
	char *mainImage;					// 대표 이미지 url
	NDDSTMAP_COORDINATES_DTO *coordinates; // 좌표. WGS84 (coordinates[0] : longitude, [1]: latitude)
	char *phone;						// 전화번호
	NDDSTMAP_PICKIMAGES_DTO *pickImages;// 썸네일 이미지 Url 리스트
	NDDSTMAP_PICKS_DTO *picks;			// Review가 있는 pick 전체 목록 리스트 (최신순 정렬)
	char *mobileWebUrl;					// 모바일 웹 URL. ex) “m.pickat.com/poi/23333”
	char *nearParkingReservationUrl;	// 주변 주차장 예약하기 URL(해당 필드의 값이 나가지 않을 경우에 대한 처리 필요) 	예) http://pg.parkhere.co.kr/?poi=%EC%82%BC%EC%84%B1%EC%97%AD&px=128.32323&py=37.232324&type=WGS84&service=tmap	(poi파라미터는 URL Encoding처리해서 응답함)
}NDDSTMAP_FINDPICKATPOIDETAIL_RES;

// 5.2.11. 지역분류별 주소명 조회 (요청)
typedef struct tagNddsTmapFindAreaNamesByStepReq
{
	int reqCnt;							//[Y] 요청건수 (1~3000)[참고]지역 분류별 최대 보유건수가 2,319건(50-110(제주-제주시)의 도로명건수임) 요청건수는 이보다 큰 값을 지정해야 함
	int reqSeq;							//[Y] 페이지 번호(1~200)
	char *largeCodeName;				//[N] 주소 대분류 명, 예) 서울
	char *middleCodeName;				//[N] 주소 중분류 명, 예) 중구
	char *addressFlag;					//[O] 주소옵션(“Addr”: 읍면동검색, “Road”: 도로명검색) middleCodeName(주소중분류명)이 있을경우 필수
}NDDSTMAP_FINDAREANAMESBYSTEP_REQ;
// 5.2.11. 지역분류별 주소명 조회  (응답)
typedef struct tagNddsTmapFindAreaNamesByStepRes
{
	NDDS_COMMONHEADER_RES	header;		// 공통 해더
	NDDS_MEMORYSTATUS		memory;		// 메모리 관리

	int totalCnt;						// 총개수
	int listCnt;						// 목록개수
	char *contFlag;						// 계속 구분 (return 계속:1,종료:0)
	NDDSTMAP_POIAREACODES_DTO *poiAreaCodes;	// 주소명칭 리스트
}NDDSTMAP_FINDAREANAMESBYSTEP_RES;

// 5.2.12. 최저가 주유소(충전소)조회 (요청)
typedef struct tagNddsTmapFindPoiStationInfoReq
{
    char *noorX;						//[Y] X좌표
    char *noorY;						//[Y] Y좌표
	char *radius;						//[Y] 검색반경 (0~33) 단위 1=1km. default : 9
	char *dataKind;						//[Y] 주유소/충전소구분. 3:주유소. 4:충전소. Default : 3
    char *fuelType;						//[N] 연료타입 fuletype이 있으면 datakind를 무시(FT_GAS:휘발유, FT_DISEL:경유, FT_LPG:LPG, FT_GASPM:고급휘발유)
}NDDSTMAP_FINDPOISTATIONINFO_REQ;
// 5.2.12. 최저가 주유소(충전소)조회  (응답)
typedef struct tagNddsTmapFindPoiStationInfoRes
{
	NDDS_COMMONHEADER_RES	header;		// 공통 해더
	NDDS_MEMORYSTATUS		memory;		// 메모리 관리

	char *poiId;						// Poi Id
	char *name;							// 시설물 명칭
	long hhPrice;						// 휘발유가격 (원단위)
	long ggPrice;						// 경유가격 (원단위)
	long llPrice;						// LPG가격 (원단위)
    long highHhPrice;                   // 프리미엄휘발유가격 (원단위)
    char *dataKind;						// 데이터구분자 (3:주유소,4:충전소)
    char *reqKey;						// 주변지검색 요청 키워드(FUELTYPE_GAS:휘발유,FUELTYPE_DISEL:경유,FUELTYPE_LPG:LPG,FUELTYPE_GASPM:고급휘발유)
}NDDSTMAP_FINDPOISTATIONINFO_RES;

// 5.2.16. ev충전소 상세정보 조회 (요청)
typedef struct tagNddsTmapFindPoiEvStationInfoReq
{
    char *poiId;						// Poi Id
}NDDSTMAP_FINDPOIEVSTATIONINFO_REQ;

// 5.2.16. ev충전소 상세정보 조회 (응답)
typedef struct tagNddsTmapFindPoiEvStationInfoRes
{
    NDDS_COMMONHEADER_RES	header;		// 공통 해더
    NDDS_MEMORYSTATUS		memory;		// 메모리 관리

    char *poiId;						// Poi Id
    char *stationName;					// 충전소명
    char *addr;							// 주소
    char *roadAddr;                     // 도로명 주소
    char *addrDetail;					// 상세주소
    char *mngName;                      // 운영기관
    char *mngId;                        // 기관 관리ID
    char *tel;							// 연락처
    char *freeYn;						// 주차비무료여부
    char *reservYn;					    // 예약가능여부
    char *useTime;						// 이용가능시간
    char *payYn;						// 유료여부
    char *fee;                          // 단가
    char *updateDt;						// 충전상태수집일시(YYYYMMDD HH24:MI:SS)
    char *noorX;						// 충전소 X좌표
    char *noorY;						// 충전소 Y좌표
    int	totalCnt;						// 충전기 개수
    NDDSTMAP_POIEVSTATIONCHARGERINFO_DTO *chargerInfo;	//충전기리스트
}NDDSTMAP_FINDPOIEVSTATIONINFO_RES;

// 5.2.18.	주변 전기차 충전소 조회 (요청)
typedef struct tagNddsTmapFindPoiEvStationsByPointReq
{
    char *noorX;						//[Y] X좌표
    char *noorY;						//[Y] Y좌표
    char *radius;						//[Y] 검색반경 (0~33) 단위 1=1km. default : 9
}NDDSTMAP_FINDPOIEVSTATIONSBYPOINT_REQ;

// 5.2.18.	주변 전기차 충전소 조회 (응답)
typedef struct tagNddsTmapFindPoiEvStationsByPointRes
{
    NDDS_COMMONHEADER_RES	header;		// 공통 해더
    NDDS_MEMORYSTATUS		memory;		// 메모리 관리

    int	totalCnt;						// 충전기 개수
    NDDSTMAP_POIEVSTATIONS_DTO *evstations;	//충전기리스트
}NDDSTMAP_FINDPOIEVSTATIONSBYPOINT_RES;
//////////////////////////////////////////////////////////////////////////
// "5.3. 즐겨찾기" 관련 구조체 정의v
//////////////////////////////////////////////////////////////////////////

// POI 집으로/회사로 정보(poiMyFavorite)
typedef struct tagNddsTmapPoiMyFavoriteDto
{
    char *homeCustName;			// 집으로 명칭 (고객이 입력한 명칭)
    char *homeNoorX;			// 집으로 X좌표 (SK 정규화 좌표 7자리)
    char *homeNoorY;			// 집으로 Y좌표 (SK 정규화 좌표 7자리)
    char *homePoiId;			// 집으로 POI ID
    char *homeNavSeq;			// 집으로 입구점 일련번호

    char *homeLcdName;			// 집으로 지역대분류명
    char *homeMcdName;			// 집으로 지역중분류명
    char *homeScdName;			// 집으로 지역소분류명
    char *homeDcdName;			// 집으로 지역세분류명
    char *homePrimaryBun;		// 집으로 본번
    char *homeSecondaryBun;		// 집으로 부번
    char *homeMlClass;			// 집으로 산/대지 구분자(0:null, 1:대지, 2:산)
    char *homeRoadName;			// 집으로 도로명
    char *homeBldNo1;			// 집으로 건물번호1
    char *homeBldNo2;			// 집으로 건물번호2
    unsigned char homeRpFlag;	// 집으로 RP FLAG
    char *homeAddInfo;			// 집으로 부가정보 (행정구역명,좌표)
    char *homeInsDatetime;		// 집으로 입력일시 (YYYYMMDDHH24MISS)
    char *officeCustName;		// 회사로 명칭 (고객이 입력한 명칭)
    char *officeNoorX;			// 회사로 X좌표 (SK 정규화 좌표 7자리)
    char *officeNoorY;			// 회사로 Y좌표 (SK 정규화 좌표 7자리)
    char *officePoiId;			// 회사로 POI ID
    char *officeNavSeq;			// 회사로 입구점 일련번호
    char *officeLcdName;		// 회사로 지역대분류명
    char *officeMcdName;		// 회사로 지역중분류명
    char *officeScdName;		// 회사로 지역소분류명
    char *officeDcdName;		// 회사로 지역세분류명
    char *officePrimaryBun;		// 회사로 본번
    char *officeSecondaryBun;	// 회사로 부번
    char *officeMlClass;		// 회사로 산/대지 구분자
    char *officeRoadName;		// 회사로 도로명
    char *officeBldNo1;			// 회사로 건물번호1
    char *officeBldNo2;			// 회사로 건물번호 2
    unsigned char officeRpFlag;			// 회사로 RP FLAG
    char *officeAddInfo;		// 회사로 부가정보 (행정구역명,좌표)
    char *officeInsDatetime;	// 회사로 입력일시 (YYYYMMDDHH24MISS)
}NDDSTMAP_POIMYFAVORITE_DTO;

// POI 즐겨찾기 정보
typedef struct tagNddsTmapPoiFavoritesDto
{
    char *custName;			// 명칭
    char *noorX;			// X 좌표 (SK 정규화 좌표 7자리)
    char *noorY;			// Y 좌표 (SK 정규화 좌표 7자리)
    char *poiId;			// POI ID
    char *navSeq;			// 입구점 일련번호
    char *lcdName;			// 지역 대분류명
    char *mcdName;			// 지역 중분류명
    char *scdName;			// 지역 소분류명
    char *dcdName;			// 지역 세분류명
    char *primaryBun;		// 본번
    char *secondaryBun;		// 부번
    char *mlClass;			// 산/대지 구분자(null:0, 1:대지, 2:산)
    char *roadName;			// 도로명
    char *bldNo1;			// 건물번호1
    char *bldNo2;			// 건물번호2
    char *repClsName;		// 대표 업종명
    char *clsAName;			// 업종 대분류명
    char *clsBName;			// 업종 중분류명
    char *clsCName;			// 업종 소분류명
    char *clsDName;			// 업종 상세분류명
    unsigned char rpFlag;	// RP Flag
    char *telNo;			// 전화번호
    char *addInfo;			// 부가정보
    char *orgCustName;		// 최초명칭
    char *iconInfo;			// 아이콘정보(/별/. /하트/, /병원/ 등)
    char *insDatetime;		// 입력일시 (YYYYMMDDHH24MISS)
}NDDSTMAP_POIFAVORITES_DTO;

// POI 즐겨찾기 등록 정보
typedef struct tagNddsTmapPoiRegistFavoriteDto
{
    char *custName;			// 명칭
    char *noorX;			// X 좌표 (SK 정규화 좌표 7자리)
    char *noorY;			// Y 좌표 (SK 정규화 좌표 7자리)
    char *poiId;			// POI ID
    char *navSeq;			// 입구점 일련번호
    unsigned char rpFlag;	// RP Flag
    char *addInfo;			// 부가정보
    char *orgCustName;		// 최초 명칭
    char *iconInfo;			// 아이콘 정보(/별/, /하트/, /병원/ 등)
}NDDSTMAP_POIREGISTPOIFAVORITE_DTO;

// POI 즐겨찾기 삭제  정보
typedef struct tagNddsTmapPoiRemoveFavoriteDto
{
    char *custName;			// 명칭
    char *noorX;			// X 좌표 (SK 정규화 좌표 7자리)
    char *noorY;			// Y 좌표 (SK 정규화 좌표 7자리)
}NDDSTMAP_POIREMOVEPOIFAVORITE_DTO;

// POI 즐겨찾기 업로드 정보
typedef struct tagNddsTmapPoiUploadFavoriteDto
{
    char *custName;			// 명칭
    char *noorX;			// X 좌표 (SK 정규화 좌표 7자리)
    char *noorY;			// Y 좌표 (SK 정규화 좌표 7자리)
    char *poiId;			// POI ID
    unsigned char rpFlag;	// RP Flag
    char *addInfo;			// 부가정보
    char *orgCustName;		// 최초 명칭
    char *iconInfo;			// 아이콘 정보(/별/, /하트/, /병원/ 등)
    char *insDatetime;		// 입력일시 (YYYYMMDDHHMISS)
    char *navSeq;			// 입구점 일련번호
}NDDSTMAP_POIUPLOADFAVORITE_DTO;

// POI 즐겨찾기 단건 수정 정보
typedef struct tagNddsTmapPoiModifyFavoriteDto
{
    char *custName;			// 명칭
    char *noorX;			// X 좌표 (SK 정규화 좌표 7자리)
    char *noorY;			// Y 좌표 (SK 정규화 좌표 7자리)
    char *newCustName;		// 변경 후 명칭
    char *iconInfo;			// 아이콘 정보(/별/, /하트/, /병원/ 등)
}NDDSTMAP_POIMODIFYPOIFAVORITE_DTO;

// 출발지, 경유지, 도착지 정보
typedef struct tagNddsTmapRoutePosDto
{
	char *name;					//[Y] 시설물 명칭
	char *poiId;				//[Y] POI ID
	char *navX;					//[Y] X 좌표
	char *navY;					//[Y] Y 좌표
	unsigned char rpFlag;				//[N] RP FLAG
}NDDSTMAP_ROUTEPOS_DTO;

// 경로 즐겨찾기 정보
typedef struct tagNddsTmapRouteFavoriteDto
{
	char				*routeTitle;		//[Y] 경로제목
	char				*insDatetime;		//[Y] 입력일시(YYYYMMDDHHMISS)
	NDDSTMAP_ROUTEPOS_DTO	startRouteDto;		//[Y] 출발지 정보
	NDDSTMAP_ROUTEPOS_DTO	endRouteDto;		//[Y] 도착지 정보
	int					nMidList;			//[Y] 경유지 정보 리스트 개수
	NDDSTMAP_ROUTEPOS_DTO	*midRouteDtos;		//[N] 경유지 정보 리스트
}NDDSTMAP_ROUTEFAVORITE_DTO;

// 교통정보 즐겨찾기 리스트 정보
typedef struct tagNddsTmapTrafficFavoritesDto
{
	int					seq;				// 일련번호
	char				*infTypCd;			// 정보구분 ("1": 도로, "2": 이미지, "3": 주변교통정보)
	char				*distributeId;		// 도로/이미지ID
	char				*name;				// 정보명
	char				*noorX;				// X좌표
	char				*noorY;				// Y좌표
}NDDSTMAP_TRAFFICFAVORITES_DTO;

// 교통정보 즐겨찾기 등록 리스트 정보
typedef struct tagNddsTmapTrafficFavoriteRegistDto
{
	char				*infTypCd;			//[Y] 정보구분 ("1": 도로, "2": 이미지, "3": 주변교통정보)
	char				*distributeId;		//[N] 도로/이미지ID
	char				*name;				//[Y] 정보명
	char				*noorX;				//[N] X좌표
	char				*noorY;				//[N] Y좌표
}NDDSTMAP_TRAFFICFAVORITEREGIST_DTO;

// 상습 정체구간 정보
typedef struct tagNddsTmapNodeJamRankJamPrioritysDto
{
	short					jamPriority;	// 정체순위
	long					nodeId;			// 노드 ID
	char					*nodeName;		// 노드 명칭
	int						posX;			// X 좌표
	int						posY;			// Y 좌표
}NDDSTMAP_NODEJAMRANKJAMPRIORITYS_DTO;


// 5.3.1. POI 즐겨찾기 조회 (요청)
typedef struct tagNddsTmapFindPoiFavoriteReq
{
	char *myFavoriteYn;				//[N] POI 집으로/회사로 조회 여부(“Y”/”N”). default: “N”
}NDDSTMAP_FINDPOIFAVORITE_REQ;
// 5.3.1. POI 즐겨찾기 조회 (응답)
typedef struct tagNddsTmapFindPoiFavoriteRes
{
	NDDS_COMMONHEADER_RES	header;		// 공통 해더
	NDDS_MEMORYSTATUS		memory;		// 메모리 관리

	NDDSTMAP_POIMYFAVORITE_DTO poiMyFavorite;	// POI 집으로/회사로. myFavoriteYn가 ‘Y’일 경우 응답데이터 전달.
	int nPoiFavorites;							// POI 즐겨찾기 리스트 개수
	NDDSTMAP_POIFAVORITES_DTO *poiFavorites;	// POI 즐겨찾기 리스트
}NDDSTMAP_FINDPOIFAVORITE_RES;

// 5.3.2. POI 즐겨찾기 등록 (요청)
typedef struct tagNddsTmapRegistPoiFavoriteMultiReq
{
    int nPoiFavorites;									//[Y] POI 즐겨찾기 등록 개수
    NDDSTMAP_POIREGISTPOIFAVORITE_DTO *poiFavorites;	//[Y] POI 즐겨찾기 등록 리스트
}NDDSTMAP_REGISTPOIFAVORITEMULTI_REQ;
// 5.3.2. POI 즐겨찾기 등록 (응답)
typedef struct tagNddsTmapRegistPoiFavoriteMultiRes
{
	NDDS_COMMONHEADER_RES	header;		// 공통 해더
	NDDS_MEMORYSTATUS		memory;		// 메모리 관리

}NDDSTMAP_REGISTPOIFAVORITEMULTI_RES;


// 5.3.3. POI 즐겨찾기 전체 업로드 (요청)
typedef struct tagNddsTmapRegistAllPoiFavoriteReq
{
	char *uploadOption;							//[N] 업로드 구분정보. “MC” : 집으로/회사로 초기화,“MS” : 집으로/회사로 설정,“FI” : POI즐겨찾기 업로드,“FI_MC” : POI즐겨찾기업로드 + 집으로/회사로 초기화,“FI_MS” : POI즐겨찾기업로드 + 집으로/회사로 설정,NULL일경우 기본: FI
    bool bMyFavorite;							//
    NDDSTMAP_POIMYFAVORITE_DTO poiMyFavorite;	//[N] POI 집으로/회사로
	int nPoiFavorites;							//[Y] POI 즐겨찾기 등록 개수
    NDDSTMAP_POIUPLOADFAVORITE_DTO *poiFavorites;	//[Y] POI 즐겨찾기 등록 리스트
}NDDSTMAP_REGISTALLPOIFAVORITE_REQ;
// 5.3.3. POI 즐겨찾기 전체 업로드 (응답)
typedef struct tagNddsTmapRegistAllPoiFavoriteRes
{
	NDDS_COMMONHEADER_RES	header;		// 공통 해더
	NDDS_MEMORYSTATUS		memory;		// 메모리 관리

}NDDSTMAP_REGISTALLPOIFAVORITE_RES;

// 5.3.4. POI 즐겨찾기 멀티 삭제 (요청)
typedef struct tagNddsTmapRemovePoiFavoriteReq
{
    int nPoiFavorites;							//[Y] POI 즐겨찾기 등록 개수
    NDDSTMAP_POIREMOVEPOIFAVORITE_DTO *poiFavorites;	//[Y] POI 즐겨찾기 등록 리스트

}NDDSTMAP_REMOVEPOIFAVORITE_REQ;
// 5.3.4. POI 즐겨찾기 멀티 삭제 (응답)
typedef struct tagNddsTmapRemovePoiFavoriteRes
{
    NDDS_COMMONHEADER_RES	header;		// 공통 해더
    NDDS_MEMORYSTATUS		memory;		// 메모리 관리
}NDDSTMAP_REMOVEPOIFAVORITE_RES;

// 5.3.5. POI 즐겨찾기 단건 수정 (요청)
typedef struct tagNddsTmapModifyPoiFavoriteReq
{
    NDDSTMAP_POIMODIFYPOIFAVORITE_DTO poiFavorites;	//[Y] POI 즐겨찾기 등록 리스트
}NDDSTMAP_MODIFYPOIFAVORITE_REQ;
// 5.3.5. POI 즐겨찾기 단건 수정 (응답)
typedef struct tagNddsTmapModifyPoiFavoriteRes
{
    NDDS_COMMONHEADER_RES	header;		// 공통 해더
    NDDS_MEMORYSTATUS		memory;		// 메모리 관리
}NDDSTMAP_MODIFYPOIFAVORITE_RES;


// 5.3.4. 경로 즐겨찾기 조회 (요청)
typedef struct tagNddsTmapFindRouteReq
{
	char *myRouteYn;					//[N] 자주가는 경로 조회 여부(“Y”/”N”). default: “N”
}NDDSTMAP_FINDROUTE_REQ;
// 5.3.4. 경로 즐겨찾기 조회 (응답)
typedef struct tagNddsTmapFindRouteRes
{
	NDDS_COMMONHEADER_RES	header;		// 공통 해더
	NDDS_MEMORYSTATUS		memory;		// 메모리 관리

	NDDSTMAP_ROUTEFAVORITE_DTO	routeMyFavoriteViewDto;	// 자주가는 경로. 요청에 myRouteYn 필드가 “Y”일 경우 자주가는경로 전달
	int nRouteFavoriteViewDtos;						// 경로 즐겨찾기 리스트 개수
	NDDSTMAP_ROUTEFAVORITE_DTO	*routeFavoriteViewDtos;	// 경로 즐겨찾기 리스트
}NDDSTMAP_FINDROUTE_RES;


// 5.3.5. 경로 즐겨찾기 등록 (요청)
typedef struct tagNddsTmapRegistRouteMultiReq
{
	int						nRouteFavoriteRegistDtos;	//[Y] 경로 즐겨찾기 리스트 개수
	NDDSTMAP_ROUTEFAVORITE_DTO	*routeFavoriteRegistDtos;	//[Y] 경로 즐겨찾기 등록 리스트 
}NDDSTMAP_REGISTROUTEMULTI_REQ;
// 5.3.5. 경로 즐겨찾기 등록 (응답)
typedef struct tagNddsTmapRegistRouteMultiRes
{
	NDDS_COMMONHEADER_RES	header;		// 공통 해더
	NDDS_MEMORYSTATUS		memory;		// 메모리 관리

}NDDSTMAP_REGISTROUTEMULTI_RES;


// 5.3.6. 경로 즐겨찾기 전체 업로드 (요청)
typedef struct tagNddsTmapUploadTotalRouteReq
{
	char *uploadOption;									//[N] 업로드 구분정보
	NDDSTMAP_ROUTEFAVORITE_DTO routeMyFavoriteRegistDto;//[N] 자주가는 경로
	int						nRouteFavoriteRegistDtos;	//[Y] 경로 즐겨찾기 리스트 개수
	NDDSTMAP_ROUTEFAVORITE_DTO	*routeFavoriteRegistDtos;	//[Y] 경로 즐겨찾기 등록 리스트 
}NDDSTMAP_UPLOADTOTALROUTE_REQ;
// 5.3.6. 경로 즐겨찾기 전체 업로드 (응답)
typedef struct tagNddsTmapUploadTotalRouteRes
{
	NDDS_COMMONHEADER_RES	header;		// 공통 해더
	NDDS_MEMORYSTATUS		memory;		// 메모리 관리

}NDDSTMAP_UPLOADTOTALROUTE_RES;

// 5.3.7. 교통정보 즐겨찾기 조회 (요청)
typedef struct tagNddsTmapFindTrafficFavoritesReq
{
}NDDSTMAP_FINDTRAFFICFAVORITES_REQ;
// 5.3.7. 교통정보 즐겨찾기 조회 (응답)
typedef struct tagNddsTmapFindTrafficFavoritesRes
{
	NDDS_COMMONHEADER_RES	header;		// 공통 해더
	NDDS_MEMORYSTATUS		memory;		// 메모리 관리

	int						nTrafficFavorites;		// 교통정보 즐겨찾기 리스트 개수
	NDDSTMAP_TRAFFICFAVORITES_DTO	*trafficFavorites;	//교통정보 즐겨찾기 리스트 
}NDDSTMAP_FINDTRAFFICFAVORITES_RES;


// 5.3.8. 교통정보 즐겨찾기 등록 (요청)
typedef struct tagNddsTmapRegistTrafficFavoritesReq
{
	int						nTrafficFavoriteRegistDtos;			//[Y] 교통정보 즐겨찾기 리스트 개수
	NDDSTMAP_TRAFFICFAVORITEREGIST_DTO	*trafficFavoriteRegistDtos;	//[Y] 교통정보 즐겨찾기 리스트 
}NDDSTMAP_REGISTTRAFFICFAVORITES_REQ;
// 5.3.8. 교통정보 즐겨찾기 등록 (응답)
typedef struct tagNddsTmapRegistTrafficFavoritesRes
{
	NDDS_COMMONHEADER_RES	header;		// 공통 해더
	NDDS_MEMORYSTATUS		memory;		// 메모리 관리

}NDDSTMAP_REGISTTRAFFICFAVORITES_RES;


// 5.3.9. 교통정보 즐겨찾기 전체 업로드 (요청)
typedef struct tagNddsTmapUploadTrafficFavoritesReq
{
	int						nTrafficFavoriteRegistDtos;			//[Y] 교통정보 즐겨찾기 리스트 개수
	NDDSTMAP_TRAFFICFAVORITEREGIST_DTO	*trafficFavoriteRegistDtos;	//[Y] 교통정보 즐겨찾기 리스트 
}NDDSTMAP_UPLOADTRAFFICFAVORITES_REQ;
// 5.3.9. 교통정보 즐겨찾기 전체 업로드 (응답)
typedef struct tagNddsTmapUploadTrafficFavoritesRes
{
	NDDS_COMMONHEADER_RES	header;		// 공통 해더
	NDDS_MEMORYSTATUS		memory;		// 메모리 관리

}NDDSTMAP_UPLOADTRAFFICFAVORITES_RES;


// 5.3.10. 교통정보 즐겨찾기 단일건 삭제 (요청)
typedef struct tagNddsTmapRemoveTrafficFavoriteReq
{
	char				*infTypCd;			//[Y] 정보구분 ("1": 도로, "2": 이미지, "3": 주변교통정보)
	char				*distributeId;		//[N] 도로/이미지ID
	char				*name;				//[Y] 정보명
	char				*noorX;				//[N] X좌표
	char				*noorY;				//[N] Y좌표
}NDDSTMAP_REMOVETRAFFICFAVORITE_REQ;
// 5.3.10. 교통정보 즐겨찾기 단일건 삭제 (응답)
typedef struct tagNddsTmapRemoveTrafficFavoriteRes
{
	NDDS_COMMONHEADER_RES	header;		// 공통 해더
	NDDS_MEMORYSTATUS		memory;		// 메모리 관리

}NDDSTMAP_REMOVETRAFFICFAVORITE_RES;


// 5.3.14. 최근길/즐겨찾기 전체 리스트 조회 (요청)
typedef struct tagNddsTmapFindUserDataReq
{
}NDDSTMAP_FINDUSERDATA_REQ;
// 5.3.14. 최근길/즐겨찾기 전체 리스트 조회 (응답)
typedef struct tagNddsTmapFindUserDataRes
{
	NDDS_COMMONHEADER_RES	header;		// 공통 해더
	NDDS_MEMORYSTATUS		memory;		// 메모리 관리

	NDDSTMAP_POIMYFAVORITE_DTO poiMyFavoriteDto;		// POI 집으로/회사로 (1건)
	NDDSTMAP_ROUTEFAVORITE_DTO routeMyFavoriteDto;		// 자주가는 경로 (1건)
	int						nPoiRecent;					// 최근길 상세정보 리스트 개수
	int						nPoiFavorite;				// POI 즐겨찾기 상세정보 리스트 개수
	int						nTrafficFavorite;			// 교통 즐겨찾기 상세정보 리스트 개수
	int						nRouteFavorite;				// 경로 즐겨찾기 상세정보 리스트 개수
	NDDSTMAP_POIRECENTS_DTO *poiRecentDtos;			// 최근길 상세정보 리스트
	NDDSTMAP_POIFAVORITES_DTO *poiFavoriteDtos;				// POI 즐겨찾기 상세정보 리스트
	//NDDSTMAP_TRAFFICFAVORITES_DTO *trafficFavoriteDtos;		// 교통 즐겨찾기 상세정보 리스트
	NDDSTMAP_ROUTEFAVORITE_DTO *routeFavoriteDtos;			// 경로 즐겨찾기 상세정보 리스트
}NDDSTMAP_FINDUSERDATA_RES;



//////////////////////////////////////////////////////////////////////////
// "5.4. 교통정보" 관련 구조체 정의
//////////////////////////////////////////////////////////////////////////

// 소통정보 정보 리스트
typedef struct tagNddsTmapSimplemapCongestionDto
{
	char *smLinkId;				// 심플맵 링크ID (분류ID(5) + 세부ID(2))
	char *distributeName;		// 분류명칭
	char *stNodeName;			// 시작노드명
	char *edNodeName;			// 종료노드명
	int downtravelTime;			// 다운 방향 통행시간(초)
	long downLinkLen;			// 다운 방향 통행거리(m)
	char *downCongestion;		// 다운 방향 혼잡도 ("0": 정보없음, "1": 원활, "2": 서행, "4": 정체, "9": 통제)
	int uptravelTime;			// 업 방향 통행시간(초)
	long upLinkLen;				// 업 방향 통행거리(m)
	char *upCongestion;			// 업 방향 혼잡도 ("0": 정보없음, "1": 원활, "2": 서행, "4": 정체, "9": 통제)
	char *cctvFlag;				// CCTV 존재 여부 ("0": 없음, "1": 있음)
}NDDSTMAP_SIMPLEMAPCONGESTION_DTO;

// CCTV정보 리스트
typedef struct tagNddsTmapSimplemapCctvDto
{
	char *smLinkId;				// 심플맵 링크ID (분류ID(5) + 세부ID(2))
	int subSeq;					// cctv 순번
	char *cctvId;				// cctv ID
	char *cctvName;				// cctv 명칭
}NDDSTMAP_SIMPLEMAPCCTV_DTO;

// 돌발 정보 상세정보 리스트
typedef struct tagNddsTmapTsdSudnSudnStIdDto
{
	char *sudnStId;				// 돌발 ID
	char *sudnStLargeCd;		// 돌발정보 분류 ("A": 사고, "B": 공사, "C": 행사, "D": 재해)
	char *sudnStTypeCd;			// 돌발 상세 코드
	char *sudnStRoad;			// 발생 도로 명
	char *sudnStSection;		// 발생 구간 명
	int sudnStContLen;			// 발생 내용 길이
	char *sudnStCont;			// 발생 내용
	char *controlStSdttm;		// 돌발 시작일
	char *controlEdSdttm;		// 돌발 종료 예상일
	long occurXPos;				// 돌발 발생 X좌표
	long occurYPos;				// 돌발 발생 Y좌표
	char *laneNo;				// 차선 번호 ("01":0개차로, "02":1개차로, "03":2개차로, "04":3개차로, "05":4개차로, "06":5개차로, "07": 6개차로, "08": 7개차로, "09": 8개차로, "A0": 전체차로)
}NDDSTMAP_TSDSUDNSUDNSTID_DTO;


// 5.4.1. 상습 정체구간 조회 (요청)
typedef struct tagNddsTmapFindNodeJamRankReq
{
}NDDSTMAP_FINDNODEJAMRANK_REQ;
// 5.4.1. 상습 정체구간 조회 (응답)
typedef struct tagNddsTmapFindNodeJamRankRes
{
	NDDS_COMMONHEADER_RES	header;		// 공통 해더
	NDDS_MEMORYSTATUS		memory;		// 메모리 관리

	int repeatCnt;												// 상습 정체구간 정보 개수
	NDDSTMAP_NODEJAMRANKJAMPRIORITYS_DTO *nodeJamRankJamPrioritys;	// 상습 정체구간 정보 리스트
}NDDSTMAP_FINDNODEJAMRANK_RES;


// 5.4.2. 이미지 교통정보 (요청)
typedef struct tagNddsTmapSmImageTrafficCongestionReq
{
	char *smId;							// 심플맵 ID
	char *imageResolutionType;			// 요청 해상도 구분 (ENDDS_IMAGERESOLUTIONTYPE 참조)
	char *fileTypeType;					// 심플맵 파일 구분 ("PNG": png 이미지, "PNG2": png2 이미지)
}NDDSTMAP_SMIMAGETRAFFICCONGESTION_REQ;
// 5.4.2. 이미지 교통정보 (응답)
typedef struct tagNddsTmapSmImageTrafficCongestionRes
{
	NDDS_COMMONHEADER_RES	header;		// 공통 해더
	NDDS_MEMORYSTATUS		memory;		// 메모리 관리

	char *baseSdttm;											// 교통정보 생성 일시
	char *smName;												// 이미지 교통정보 제목
	int simplemapEncodingImageSize;								// 심플맵 이미지 크기
	unsigned char *simplemapEncodingImage;								// 심플맵 이미지(Base64 인코딩 데이터)
	int trafficCongestionSize;									// 소통정보 개수
	NDDSTMAP_SIMPLEMAPCONGESTION_DTO *simplemapCongestionDtos;		// 소통정보 정보 리스트
	int simplemapCctvCount;										// CCTV 정보 개수
	NDDSTMAP_SIMPLEMAPCCTV_DTO *simplemapCctvDtos;					// CCTV 정보 리스트
}NDDSTMAP_SMIMAGETRAFFICCONGESTION_RES;


// 5.4.3. 주변 교통정보 (요청)
typedef struct tagNddsTmapRadiusTrafficInfomationReq
{
	int currentXPos;					//[Y] X 좌표
	int currentYPos;					//[Y] Y 좌표
	int radius;							//[Y] 반경 (1 ~ 9) : 1=300m
}NDDSTMAP_RADIUSTRAFFICINFOMATION_REQ;
// 5.4.3. 주변 교통정보 (응답)
typedef struct tagNddsTmapRadiusTrafficInfomationRes
{
	NDDS_COMMONHEADER_RES	header;		// 공통 해더
	NDDS_MEMORYSTATUS		memory;		// 메모리 관리

	int nSizeData;						// 주변 교통정보 데이터 크기
	unsigned char *pTrafficData;					// 주변 교통정보 데이터(별도 주변 교통정보 규격서 참고)
}NDDSTMAP_RADIUSTRAFFICINFOMATION_RES;


// 5.4.4. 돌발 정보 조회 (요청)
typedef struct tagNddsTmapFindTsdSudnReq
{
}NDDSTMAP_FINDTSDSUDN_REQ;
// 5.4.4. 돌발 정보 조회 (응답)
typedef struct tagNddsTmapFindTsdSudnRes
{
	NDDS_COMMONHEADER_RES	header;		// 공통 해더
	NDDS_MEMORYSTATUS		memory;		// 메모리 관리

	int repeatCnt;									// 돌발 정보 개수
	NDDSTMAP_TSDSUDNSUDNSTID_DTO *tsdSudnSudnStIdDtos;	// 돌발 정보 상세정보 리스트
}NDDSTMAP_FINDTSDSUDN_RES;


// 5.4.5. VMS 가이드 정보 조회 요청
typedef struct tagNddsVmsGuideInfoReq
{
	char *rseId;
}NDDSTMAP_VMSGUIDEINFO_REQ;

// 5.4.5. VMS 가이드 정보 조회 응답
typedef struct tagNddsVmsGuideInfoRes
{
	NDDS_COMMONHEADER_RES	header;		// 공통 해더
	NDDS_MEMORYSTATUS		memory;		// 메모리 관리

	char *baseSdttm;
	char *dataSource;
	char *contentCode;
	char *contentName;
	char *content;
}NDDSTMAP_VMSGUIDEINFO_RES;



//////////////////////////////////////////////////////////////////////////
// "5.5. 대중교통 정보" 관련 구조체 정의
//////////////////////////////////////////////////////////////////////////


// 버스 도착정보 리스트 정보
typedef struct tagNddsTmapBusStationArrivesDto
{
	char *busPlainNo;					// 도착 버스 차량번호
	char *isArrive;						// 현재 정류장 도착 여부("0":운행 중, "1":도착)
	char *sectOrd;						// 현재 버스 구간 순번 ("0": 기점에서 운행 전 대기 중, "0" 보다 크면 기점으로부터 Index, null : 정보없음)
	char *traTime;						// 도착시간 정보(초 단위)
}NDDSTMAP_BUSSTATIONARRIVES_DTO;

// 버스 번호 정보 리스트 정보
typedef struct tagNddsTmapLaneInfosDto
{
	char *busNo;						// 버스 번호
	int type;							// 버스 유형
	char *blid;							// 버스 노선 아이디
	char *blid_rt;						// 버스 노선 TOPIS ID
	char *staOrd;						// 현재 정류소 순번 (null : 정보없음)
	char *laneInfo;						// 기점-종점 명
	int arriveCount;									// 버스 도착정보 개수 (MAX : 2)
	NDDSTMAP_BUSSTATIONARRIVES_DTO *busStationArrives;		// 버스 도착정보 리스트
}NDDSTMAP_LANEINFOS_DTO;

// 버스 도착정보 리스트 정보
typedef struct tagNddsTmapBusArrivalInfosDto
{
	char *plainNo;						// 도착 버스 차량번호
	char *stopFlag;						// 현재 정류장 도착 여부("0":운행 중, "1":도착)
}NDDSTMAP_BUSARRIVALINFOS_DTO;

// 정류장 정보 리스트 정보
typedef struct tagNddsTmapStopInfosDto
{
	char *index;						// 구간순번
	char *sid;							// 지하철 역/버스 정류장 ID
	char *sidRt;						// 지하철 역/버스 정류장 TOPIS ID
	char *stopNavX;						// 정류장 X좌표
	char *stopNavY;						// 정류장 Y좌표
	char *stopName;						// 정류장 명
	int arrivalInfoCount;							// 버스 도착정보 개수
	NDDSTMAP_BUSARRIVALINFOS_DTO *busArrivalInfos;		// 버스 도착정보 리스트
}NDDSTMAP_STOPINFOS_DTO;

// Lane 정보 리스트 정보
typedef struct tagNddsTmapLanePathsDto
{
	char *navX;							// X 좌표
	char *navY;							// Y 좌표
}NDDSTMAP_LANEPATHS_DTO;

// 버스 도착정보 리스트 정보
typedef struct tagNddsTmapItemArriveDetailsDto
{
	char *plainNo;						// 도착 버스 차량번호
	char *stopFlag;						// 현재 정류장 도착 여부 ("0":운행 중, "1":도착)
	char *sectOrd;						// 현재 버스 구간 순번 ("0": 기점에서 운행 전 대기 중, "0" 보다 크면 기점으로부터 Index, null : 정보없음)
	char *arrivalTime;					// 도착시간 정보(초 단위)
}NDDSTMAP_ITEMARRIVEDETAILS_DTO;

// 정류장 정보 리스트 정보
typedef struct tagNddsTmapStopInfos2Dto
{
	char *index;						// 구간순번
	char *sid;							// 지하철 역/버스 정류장 ID
	char *sidRt;						// 지하철 역/버스 정류장 TOPIS ID
	char *stopNavX;						// 정류장 X좌표
	char *stopNavY;						// 정류장 Y좌표
	char *stopName;						// 정류장 명
	int stopCount;						// 도착정보 개수(itemListDetails 개수 와 itemArriveDetails 개수의 합)
	int nItemListDetails;				// 버스 도착정보 리스트 개수
	int nItemArriveDetails;				// 버스 도착정보 리스트 개수
	NDDSTMAP_ITEMARRIVEDETAILS_DTO *itemListDetails;		// 버스 도착정보 리스트
	NDDSTMAP_ITEMARRIVEDETAILS_DTO *itemArriveDetails;		// 버스 도착정보 리스트
}NDDSTMAP_STOPINFOS2_DTO;

// 버스 정류장 / 노선 정보 리스트 정보
typedef struct tagNddsTmapBusStationRealTimeDetailsDto
{
	char *blid;							//[Y] 버스노선 ID
	char *blidRt;						//[N] 버스노선 TOPIS ID
	char *sid;							//[Y] 지하철 역 / 버스 정류장 ID
	char *sidRt;						//[Y] 지하철 역 / 버스 정류장 TOPIS ID
	char *cid;							//[N] 지역코드
}NDDSTMAP_BUSSTATIONREALTIMEDETAILS_DTO;

// 버스 도착정보 리스트 정보
typedef struct tagNddsTmapArriveDetailsDto
{
	char *busNo;						// 도착 버스 차량 번호
	int isArrive;						// 현재 정류소 도착 여부 (0:운행 중, 1:도착)
	char *sectOrd;						// 현재 버스 구간 순번 ("0": 기점에서 운행 전 대기 "0"이상: 기점으로부터의 Index, 현재 정류소 순번이면 : 현재 정류소를 향하고 있다.)
	char *traTime;						// 도착시간 정보(초 단위)
}NDDSTMAP_ARRIVEDETAILS_DTO;

// 버스 정류장 정보 리스트 정보
typedef struct tagNddsTmapBusStationRealTimesDto
{
	char *blid;							// 버스 노선 ID
	char *blidRt;						// 버스 노선 TOPIS ID
	char *sid;							// 지하철 역 / 버스 정류장 ID
	char *sidRt;						// 지하철 역 / 버스 정류장 TOPIS ID
	char *stopIndex;					// 현재 정류장 순번
	int arriveCount;					// 버스 도착정보 개수
	NDDSTMAP_ARRIVEDETAILS_DTO *arriveDetails;		// 버스 도착정보 리스트
}NDDSTMAP_BUSSTATIONREALTIMES_DTO;

// 버스 번호 정보 리스트 정보
typedef struct tagNddsTmapBusInfosDto
{
	char *busNo;						// 버스 번호
	char *type;							// 버스 유형
	char *blid;							// 버스 노선 ID
}NDDSTMAP_BUSINFOS_DTO;

// 버스 정류장 정보 리스트 정보
typedef struct tagNddsTmapBusStopsDto
{
	char *stopName;						// 정류장 명
	int busCount;						// 버스 번호 정보 개수
	NDDSTMAP_BUSINFOS_DTO *busInfos;		// 버스 번호 정보 리스트
}NDDSTMAP_BUSSTOPS_DTO;

// 주요 건물 정보 리스트 정보
typedef struct tagNddsTmapGateLinksDto
{
	char *gateLinks;					// 주요 건물 명
}NDDSTMAP_GATELINKS_DTO;

// 출구 정보 리스트 정보
typedef struct tagNddsTmapGateInfosDto
{
	char *gateNo;						// 출구 번호
	char *gateX;						// 출구 X 좌표
	char *gateY;						// 출구 Y 좌표
	int gateLinkCount;					// 주요 건물 정보 개수
	NDDSTMAP_GATELINKS_DTO *gateLinks;		// 주요 건물 정보 리스트
	int busStopCount;					// 버스 정류장 정보 개수
	NDDSTMAP_BUSSTOPS_DTO *busStops;		// 버스 정류장 정보 리스트
}NDDSTMAP_GATEINFOS_DTO;

// 환승 정보 리스트 정보
typedef struct tagNddsTmapFastExsDto
{
	char *takeLaneName;					// 환승 전 기차 호선 명
	char *takeLaneDirection;			// 환승 전 기차 방면
	char *exLaneName;					// 환승 할 기차 호선 명
	char *exLaneDirection;				// 환승 할 기차 방면
	int fastTrain;						// 환승 객차 번호
	int fastDoor;						// 환승 문 번호
}NDDSTMAP_FASTEXS_DTO;

// 이전/다음역 정보 리스트 정보
typedef struct tagNddsTmapStationsDto
{
	char *sid;							// 지하철 역 / 버스 정류장 ID
	char *name;							// 역명
}NDDSTMAP_STATIONS_DTO;


// 요일별 상/하행 첫차/막차 정보 리스트 정보
typedef struct tagNddsTmapFirstLastDetailsDto
{
	char *firstLastFlag;				// 상/하행 방면(up:상행, down:하행)
	char *guName;						// 방면 이름
	char *ordFirstTime;					// 평일 첫차 시간
	char *ordLastTime;					// 평일 막차 시간
	char *satFirstTime;					// 토요일 첫차 시간
	char *satLastTime;					// 토요일 막차 시간
	char *sunFirstTime;					// 공휴일 첫차 시간
	char *sunLastTime;					// 공휴일 막차 시간
}NDDSTMAP_FIRSTLASTDETAILS_DTO;

// 시간대별 운행 정보 리스트 정보
typedef struct tagNddsTmapTimeListValuesDto
{
	char *timeListValues;				// 운행 정보(ex:10(성수))
}NDDSTMAP_TIMELISTVALUES_DTO;

// 요일별 상/하행 운행정보 리스트 정보
typedef struct tagNddsTmapTimeDetailsDto
{
	char *day;							// 요일(ord:평일, sat:토요일, sun:공휴일)
	char *flag;							// 상/하행 방면(up:상행, down:하행)
	int timeIdx;						// 시간(5~24시)
	int timeListCount;					// 시간대별 운행 정보 개수
	char **timeListValues;		// 시간대별 운행 정보 리스트
}NDDSTMAP_TIMEDETAILS_DTO;

// 역/정류장 정보 리스트 정보
typedef struct tagNddsTmapStationInfosDto
{
	char *stationType;					// 역/정류장 구분(1:출발지, 2:목적지, 3:환승) - 결과 구분코드(searchType)의 값이 "1"일때는 정보 없음.
	char *transportFlag;				// 대중교통 구분(1:지하철, 2:버스, 4:기차, 5:고속버스, 6:시외버스, 7:비행기)
	char *startName;					// 시작 지점 명
	char *startSid;						// 시작 지점 ID
	char *startX;						// 시작 지점 X좌표
	char *startY;						// 시작 지점 Y좌표
	char *endName;						// 종료 지점 명
	char *endSid;						// 종료 지점 ID
	char *endX;							// 종료 지점 X 좌표
	char *endY;							// 종료 지점 Y 좌표
	int stopCount;						// 구간 정류장 개수
	char *laneAlias;					// 지하철 호선 정보(대중교통 구분이 지하철인 경우만 표시됨)
	char *way;							// 지하철 방면 정보(대중교통 구분이 지하철인 경우만 표시됨)
	char *laneColor;					// 지하철 색상정보(대중교통 구분이 지하철인 경우만 표시됨)
	char *trainType;					// 기차 종류(대중교통 구분이 기차인 경우만 표시됨)
	char *trainCode;					// 기차 종류 코드(대중교통 구분이 기차인 경우만 표시됨)
	int busInfoCount;					// 버스 번호 정보 개수(대중교통 구분이 버스인 경우만 표시됨)
	NDDSTMAP_BUSINFOS_DTO *busInfos;		// 버스 번호 정보 리스트(대중교통 구분이 버스인 경우만 표시됨)
}NDDSTMAP_STATIONINFOS_DTO;

// Lane 위치정보 리스트 정보
typedef struct tagNddsTmapLaneInfos2Dto
{
	char *laneX;						// Lane X좌표
	char *laneY;						// Lane Y좌표
}NDDSTMAP_LANEINFOS2_DTO;

// 대중 교통 경로안내 정보 리스트 정보
typedef struct tagNddsTmapPathFind1Stall4ViewsDto
{
	char *transportType;				// 대중교통 구분(1:지하철, 2:버스, 3:환승, 4:기차, 5:고속버스, 6:시외버스, 7:비행기)
	char *distance;						// 거리(Km 단위)
	char *time;							// 경과 시간(분 단위)
	char *payment;						// 요금(원 단위)
	char *exNum;						// 환승 개수
	char *laneVal;						// 다른 부가정보 API와 연동하기 위한 Key - 현재 사용하지 않음
	int laneInfoCount;					// Lane 위치정보 개수
	NDDSTMAP_LANEINFOS2_DTO *laneInfos;		// Lane 위치정보 리스트
	int stationCount;					// 역/정류장 정보 개수
	NDDSTMAP_STATIONINFOS_DTO *stationInfos;		// 역/정류장 정보 리스트
}NDDSTMAP_PATHFIND1STALL4VIEWS_DTO;

// 주변 정류장 / 역 정보 리스트 정보(NearStations)
typedef struct tagNddsTmapNearStationsDto
{
	int flag;							// 대중교통 구분, (1:버스, 2:지하철)
	char *dist;							// 직선 거리, 현재 정보는 m, Km단위로 변확해서 사용해야 함.
	char *sid;							// 지하철역/버스정류장 ID
	char *sidRt;						// 지하철역/버스정류장 TOPIS ID
	char *navX;							// X 좌표
	char *navY;							// Y 좌표
	char *stationName;					// 지하철 역/버스정류소 명
	char *dongName;						// 대중교통 구분이 1이면 주소, 대중교통 구분이 2이면 호선
}NDDSTMAP_NEARSTATIONS_DTO;

// POI 상세정보 리스트 정보
typedef struct tagNddsTmapPoiInfoDetailsDto
{
	char *dbKind;						// 시설물 DB(S:SK, A:주소)
	char *poiId;						// POI ID
	char *name;							// 시설물 명칭
	char *orgName;						// 명칭 alias
	char *navX1;						// 시설물 입구 X좌표
	char *navY1;						// 시설물 입구 Y좌표
	char *centerX;						// 시설물 중심 X좌표
	char *centerY;						// 시설물 중심 Y좌표
	char *rpFlag;						// RP Flag
	char *parkYn;						// 주차 여부(0:없음, 1:있음)
	char *phoneNum;						// 전화번호
	char *addr;							// 주소
	char *primary;						// 본번
	char *secondary;					// 부번
	char *mlClass;						// 산/대지 구분
	char *lcdName;						// 표출주소 대분류
	char *mcdName;						// 표출주소 중분류
	char *scdName;						// 표출주소 소분류
	char *dcdName;						// 표출주소 세분류
}NDDSTMAP_POIINFODETAILS_DTO;

// POI 통합검색 정보
typedef struct tagNddsTmapPoiInfoDto
{
	int totalCnt;						// 총 개수
	int listCnt;						// 목록 개수
	char *contFlag;						// 계속 구분
	int areaCnt;						// 지역 개수
	char *areaNm;						// 지역 명
	NDDSTMAP_POIINFODETAILS_DTO *poiInfoDetails;		// POI 상세정보 리스트
}NDDSTMAP_POIINFO_DTO;

// 정류장/역 리스트 정보
typedef struct tagNddsTmapPublicInfoDetailsDto
{
	int flag;							// 대중교통 구분(1:버스, 2:지하철, 3:버스번호)
	char *name;							// 대중교통 구분이 1,2 이면 : 정류장/역 명 대중교통 구분이 3이면 : 버스번호
	char *dong;							// 대중교통 구분이 1이면 주소 동 대중교통 구분이 2이면 호선 명 대중교통 구분이 3이면 기점-종점 명
	char *sid;							// 대중교통 구분이 1,2이면 지하철역/버스정류장 ID 대중교통 구분이 3이면 버스노선 ID
	char *sidRt;						// 지하철역/버스정류장 TOPIS ID 또는 버스노선 TOPIS ID
	int type;							// 버스번호 유형
	char *navX;							// 지하철 역/버스 정류장 X좌표
	char *navY;							// 지하철 역/버스 정류장 Y좌표
	char *cid;							// 지역코드
	char *cityName;						// 지역명칭
}NDDSTMAP_PUBLICINFODETAILS_DTO;

// - 대중교통 통합검색 정보(publicInfo)
typedef struct tagNddsTmapPublicInfoDto
{
	int transportCount;								// 정류장/역 리스트 개수
	NDDSTMAP_PUBLICINFODETAILS_DTO *publicInfoDetails;	// 정류장/역 리스트
}NDDSNDDS_PUBLICINFO_DTO_PUBLICINFO_DTO;


// 주변 CCTV 상세정보 리스트 정보
typedef struct tagNddsTmapCctvResultDetailsDto
{
	int	cctvId;						// CCTV ID
	char *navX;							// X좌표
	char *navY;							// Y좌표
	char *useYn;						// 영상배포 가능 여부(Y:가능, N:불가능)
	char *serviceYn;					// 서비스 가능 여부(Y:가능, N:불가능)
	char *name;							// CCTV 명
	char *offer;						// 제공처
	char *time;							// 생성 시간
	char *cctvUrl;						// CCTV URL
}NDDSTMAP_CCTVRESULTDETAILS_DTO;


// 5.5.1. 버스 정류장 정보(도착 시간) 요청
typedef struct tagNddsTmapFindBusStationReq
{
	char *sid;							//[Y] 지하철 역 / 버스 정류장 ID
	char *cid;							//[N] 지역코드
}NDDSTMAP_FINDBUSSTATION_REQ;
// 5.5.1. 버스 정류장 정보(도착 시간) 응답
typedef struct tagNddsTmapFindBusStationRes
{
	NDDS_COMMONHEADER_RES	header;		// 공통 해더
	NDDS_MEMORYSTATUS		memory;		// 메모리 관리

	int laneCount;						// 버스 번호 정보 개수
	NDDSTMAP_LANEINFOS_DTO *laneInfos;		// 버스 번호 정보 리스트
}NDDSTMAP_FINDBUSSTATION_RES;


// 5.5.2. 버스 노선 정보(도착 정보) 요청
typedef struct tagNddsTmapFindBusArrivalReq
{
	char *blid;							//[Y] 버스노선 ID
	char *blidRt;						//[N] 버스노선 TOPIS ID
	char *cid;							//[N] 지역코드
}NDDSTMAP_FINDBUSARRIVAL_REQ;
// 5.5.2. 버스 노선 정보(도착 정보) 응답
typedef struct tagNddsTmapFindBusArrivalRes
{
	NDDS_COMMONHEADER_RES	header;		// 공통 해더
	NDDS_MEMORYSTATUS		memory;		// 메모리 관리

	char *first;						// 첫차시간
	char *last;							// 막차시간
	char *tel;							// 회사 전화번호
	int interval;						// 배차 간격(분)
	char *distance;						// 총 운행거리(Km로 환산해야 함)
	int runTime;						// 총 운행시간(분)
	char *turnPoint;					// 회차정보
	char *laneInfo;						// 기점-종점 정보
	char *companyName;					// 회사명
	int lanePathCount;					// Lane 정보 개수
	NDDSTMAP_LANEPATHS_DTO *lanePaths;		// Lane 정보 리스트
	int stopInfoCount;					// 정류장 정보 개수
	NDDSTMAP_STOPINFOS_DTO *stopInfos;		// 정류장 정보 리스트
}NDDSTMAP_FINDBUSARRIVAL_RES;


// 5.5.3. 버스 노선 정보(도착 시간) 요청
typedef struct tagNddsTmapFindBusArrivalTimeReq
{
	char *blid;							//[Y] 버스노선 ID
	char *sid;							//[N] 지하철 역 / 버스 정류장 ID
	char *sidRt;						//[Y] 지하철 역 / 버스 정류장 TOPIS ID
	char *cid;							//[N] 지역코드
}NDDSTMAP_FINDBUSARRIVALTIME_REQ;
// 5.5.3. 버스 노선 정보(도착 시간) 응답
typedef struct tagNddsTmapFindBusArrivalTimeRes
{
	NDDS_COMMONHEADER_RES	header;		// 공통 해더
	NDDS_MEMORYSTATUS		memory;		// 메모리 관리

	char *first;						// 첫차시간
	char *last;							// 막차시간
	char *tel;							// 회사 전화번호
	int interval;						// 배차 간격(분)
	char *distance;						// 총 운행거리(Km로 환산해야 함)
	int runTime;						// 총 운행시간(분)
	char *turnPoint;					// 회차정보
	char *laneInfo;						// 기점-종점 정보
	char *companyName;					// 회사명
	int lanePathCount;					// Lane 정보 개수
	NDDSTMAP_LANEPATHS_DTO *lanePaths;		// Lane 정보 리스트
	int stopInfoCount;					// 정류장 정보 개수
	NDDSTMAP_STOPINFOS2_DTO *stopInfos;		// 정류장 정보 리스트
}NDDSTMAP_FINDBUSARRIVALTIME_RES;


// 5.5.4. 버스 정류장/노선 실시간 정보 요청
typedef struct tagNddsTmapFindBusStationRealTimeReq
{
	int busStopCount;												//[Y] 버스 정류장/노선 정보개수
	NDDSTMAP_BUSSTATIONREALTIMEDETAILS_DTO *busStationRealTimeDetails;	//[Y] 버스 정류장/노선 정보 리스트
}NDDSTMAP_FINDBUSSTATIONREALTIME_REQ;
// 5.5.4. 버스 정류장/노선 실시간 정보 응답
typedef struct tagNddsTmapFindBusStationRealTimeRes
{
	NDDS_COMMONHEADER_RES	header;		// 공통 해더
	NDDS_MEMORYSTATUS		memory;		// 메모리 관리

	int busStopCount;											// 버스 정류장/노선 정보개수
	NDDSTMAP_BUSSTATIONREALTIMES_DTO *busStationRealTimes;			// 버스 정류장/노선 정보 리스트
}NDDSTMAP_FINDBUSSTATIONREALTIME_RES;


// 5.5.5. 역 정보 요청
typedef struct tagNddsTmapFindStationInfoReq
{
	char *sid;							//[Y] 지하철 역 / 버스 정류장 ID
	char *cid;							//[N] 지역코드
}NDDSTMAP_FINDSTATIONINFO_REQ;
// 5.5.5. 역 정보 응답
typedef struct tagNddsTmapFindStationInfoRes
{
	NDDS_COMMONHEADER_RES	header;		// 공통 해더
	NDDS_MEMORYSTATUS		memory;		// 메모리 관리

	char *laneNumber;					// 역 호선 정보
	char *navX;							// 역 X좌표
	char *navY;							// 역 Y좌표
	char *tel;							// 역 전화번호
	char *offDoor;						// 역 하자 문 방향
	char *platForm;						// 플랫폼 위치
	char *crossOver;					// 반대편 횡단여부
	char *location;						// 주차장 시설 - 위치
	char *telephone;					// 주차장 시설 - 전화번호
	char *area;							// 주차장 시설 - 면적
	int fee;							// 주차장 시설 - 유/무료(1:무료, 2:유료, 그 외 - 정보 없음)
	char *normalOpen;					// 민원실 - 평일 오픈 시간
	char *weekendOpen;					// 민원실 - 주말 오픈 시간
	char *capacity;						// 자전거-보관대수
	char *name;							// 지하철 역 명
	char *address;						// 지하철 역 주소
	char *imgMap;						// 역 노선도 구간 정보
	char *imgPath;						// 역 노선도 파일명
	char *handiCapInfo;					// 장애인 시설 정보
	char *affair;						// 민원실-업무내용
	int preStationCount;				// 이전역 정보 개수
	NDDSTMAP_STATIONS_DTO *preStations;		// 이전역 정보 리스트
	int nextStationCount;				// 다음역 정보 개수
	NDDSTMAP_STATIONS_DTO *nextStations;	// 다음역 정보 리스트
	int fastExCount;					// 환승 정보 개수
	NDDSTMAP_FASTEXS_DTO *fastExs;			// 환승 정보 리스트
	int gateCount;						// 지하철 역 출구 개수
	NDDSTMAP_GATEINFOS_DTO *gateInfos;		// 출구 정보 리스트
}NDDSTMAP_FINDSTATIONINFO_RES;


// 5.5.6. 역 운행 정보 요청
typedef struct tagNddsTmapFindStationTimeTableReq
{
	char *sid;							//[Y] 지하철 역 / 버스 정류장 ID
	char *cid;							//[N] 지역코드
}NDDSTMAP_FINDSTATIONTIMETABLE_REQ;
// 5.5.6. 역 운행 정보 응답
typedef struct tagNddsTmapFindStationTimeTableRes
{
	NDDS_COMMONHEADER_RES	header;		// 공통 해더
	NDDS_MEMORYSTATUS		memory;		// 메모리 관리

	char *upWay;						// 상행 노선 명
	char *downWay;						// 하행 노선 명
	int ordUpTimeCount;					// 평일 상행 운행정보 개수
	int ordDownTimeCount;				// 평일 하행 운행정보 개수
	int satUpTimeCount;					// 토요일 상행 운행정보 개수
	int satDownTimeCount;				// 토요일 하행 운행정보 개수
	int sunUpTimeCount;					// 공휴일 상행 운행정보 개수
	int sunDownTimeCount;				// 공휴일 하행 운행정보 개수
	int firstLastUpCount;				// 상행 방면 첫차/막차 정보 개수
	int firstLastDownCount;				// 하행 방면 첫차/막차 정보 개수
	NDDSTMAP_TIMEDETAILS_DTO *timeDetails;				// 요일별 상/하행 운행정보 리스트
	NDDSTMAP_FIRSTLASTDETAILS_DTO *firstLastDetails;	// 요일별 상/하행 첫차/막차 정보 리스트
}NDDSTMAP_FINDSTATIONTIMETABLE_RES;


// 5.5.7. 노선 이미지 요청
typedef struct tagNddsTmapFindSubwayImageReq
{
	char *subwayImage;					//[Y] 이미지 url정보
}NDDSTMAP_FINDSUBWAYIMAGE_REQ;
// 5.5.7. 노선 이미지 응답
typedef struct tagNddsTmapFindSubwayImageRes
{
	NDDS_COMMONHEADER_RES	header;		// 공통 해더
	NDDS_MEMORYSTATUS		memory;		// 메모리 관리

	int nSizeImage;						// 이미지 크기
	unsigned char *subwayImage;					// 이미지 정보(Base64 Encode로 인코딩된 데이타)
}NDDSTMAP_FINDSUBWAYIMAGE_RES;


// 5.5.8. 경로안내(도시간 검색 포함) 요청
typedef struct tagNddsTmapFindPathFind1Stall4Req
{
	char *sx;							//[Y] 출발지 X좌표
	char *sy;							//[Y] 출발지 Y좌표
	char *ex;							//[Y] 목적지 X좌표
	char *ey;							//[Y] 목적지 Y좌표
	char *searchType;					//[Y] 탐색 조건(default:0)
}NDDSTMAP_FINDPATHFIND1STALL4_REQ;
// 5.5.8. 경로안내(도시간 검색 포함) 응답
typedef struct tagNddsTmapFindPathFind1Stall4Res
{
	NDDS_COMMONHEADER_RES	header;		// 공통 해더
	NDDS_MEMORYSTATUS		memory;		// 메모리 관리

	char *searchType;					// 결과 구분코드("0": 도시내, "1": 도시간)
	char *outTrafficCheck;				// 도시간 탐색결과 유/무("0": 무, "1": 유)
	char *startCid;						// 출발지 도시 코드
	char *endCid;						// 목적지 도시 코드
	int totalCount;						// 대중 교통 경로안내 개수
	NDDSTMAP_PATHFIND1STALL4VIEWS_DTO *pathFind1Stall4Views;		// 대중 교통 경로안내 정보 리스트
}NDDSTMAP_FINDPATHFIND1STALL4_RES;


// 5.5.9. 경로안내(도시내 복수 검색) 요청
typedef struct tagNddsTmapFindPathFind1Stall4PluralReq
{
	char *startSx;						//[Y] 출발도시 출발지 X좌표
	char *startSy;						//[Y] 출발도시 출발지 Y좌표
	char *startEx;						//[Y] 출발도시 목적지 X좌표
	char *startEy;						//[Y] 출발도시 목적지 Y좌표
	char *endSx;						//[Y] 목적도시 출발지 X좌표
	char *endSy;						//[Y] 목적도시 출발지 Y좌표
	char *endEx;						//[Y] 목적도시 목적지 X좌표
	char *endEy;						//[Y] 목적도시 목적지 Y좌표
	char *searchType;					//[Y] 탐색 조건(default:0)
}NDDSTMAP_FINDPATHFIND1STALL4PLURAL_REQ;
// 5.5.9. 경로안내(도시내 복수 검색) 응답
typedef struct tagNddsTmapFindPathFind1Stall4PluralRes
{
	NDDS_COMMONHEADER_RES	header;		// 공통 해더
	NDDS_MEMORYSTATUS		memory;		// 메모리 관리

	int startCount;												// 출발도시 대중교통 경로 개수
	int endCount;												// 목적도시 대중교통 경로 개수
	NDDSTMAP_PATHFIND1STALL4VIEWS_DTO *startPathFind1Stall4Views;	// 출발도시 대중 교통 경로안내 정보 리스트
	NDDSTMAP_PATHFIND1STALL4VIEWS_DTO *endPathFind1Stall4Views;		// 목적도시 대중 교통 경로안내 정보 리스트
}NDDSTMAP_FINDPATHFIND1STALL4PLURAL_RES;


// 5.5.10. 주변 정류장/역 조회(거리순) 요청
typedef struct tagNddsTmapFindNearStationReq
{
	char *sx;							//[Y] 출발지 X 좌표
	char *sy;							//[Y] 출발지 Y 좌표
	int iradius;						//[Y] 반경(m단위)
	char *sclass;						//[Y] 구분(0:모두, 1:버스, 2:지하철), 현재는 0으로 Fix
	char *cid;							//[N] 지역코드
}NDDSTMAP_FINDNEARSTATION_REQ;
// 5.5.10. 주변 정류장/역 조회(거리순) 응답
typedef struct tagNddsTmapFindNearStationRes
{
	NDDS_COMMONHEADER_RES	header;		// 공통 해더
	NDDS_MEMORYSTATUS		memory;		// 메모리 관리

	int nearStationCount;				// 주변 정류장 / 역 정보 리스트 개수
	NDDSTMAP_NEARSTATIONS_DTO *nearStations;	// 주변 정류장 / 역 정보 리스트
}NDDSTMAP_FINDNEARSTATION_RES;


// 5.5.11. 대중교통 통합 검색 요청
typedef struct tagNddsTmapFindAllTransportReq
{
	char *searchName;					//[Y] 정류소/역명 또는 버스번호
	char *cid;							//[N] 지역코드
}NDDSTMAP_FINDALLTRANSPORT_REQ;
// 5.5.11. 대중교통 통합 검색 응답
typedef struct tagNddsTmapFindAllTransportRes
{
	NDDS_COMMONHEADER_RES	header;		// 공통 해더
	NDDS_MEMORYSTATUS		memory;		// 메모리 관리
	
	int	cntPublicInfo;	// 대중교통 통합검색 정보 개수 
	int cntPoiInfo;		// POI 통합검색 정보 개수 

	NDDSNDDS_PUBLICINFO_DTO_PUBLICINFO_DTO publicInfo;	// 대중교통 통합검색 정보
	NDDSTMAP_POIINFO_DTO	poiInfo;	// POI 통합검색 정보
}NDDSTMAP_FINDALLTRANSPORT_RES;

 
// 5.5.12. 주변 CCTV 리스트 요청
typedef struct tagNddsTmapFindCctvResultReq
{
	char *sx;							//[Y] 출발지 X좌표
	char *sy;							//[Y] 출발지 Y좌표
	char *iradius;						//[Y] 반경(m단위)
}NDDSTMAP_FINDCCTVRESULT_REQ;
// 5.5.12. 주변 CCTV 리스트 응답
typedef struct tagNddsTmapFindCctvResultRes
{
	NDDS_COMMONHEADER_RES	header;		// 공통 해더
	NDDS_MEMORYSTATUS		memory;		// 메모리 관리

	int cctvCount;									// 주변 CCTV 정보 개수
	NDDSTMAP_CCTVRESULTDETAILS_DTO *cctvResultDetails;	// 주변 CCTV 상세정보 리스트
}NDDSTMAP_FINDCCTVRESULT_RES;


// 5.5.13. CCTV 정보 요청
typedef struct tagNddsTmapFindCctvResultByIdReq
{
	char *cctvId;						//[Y] CCTV ID
}NDDSTMAP_FINDCCTVRESULTBYID_REQ;
// 5.5.13. CCTV 정보 응답
typedef struct tagNddsTmapFindCctvResultByIdRes
{
	NDDS_COMMONHEADER_RES	header;		// 공통 해더
	NDDS_MEMORYSTATUS		memory;		// 메모리 관리

	int cctvId;						// CCTV ID
	char *navX;							// X좌표
	char *navY;							// Y좌표
	char *useYn;						// 영상배포 가능 여부(Y:가능, N:불가능)
	char *serviceYn;					// 서비스 가능 여부(Y:가능, N:불가능)
	char *name;							// CCTV 명
	char *offer;						// 제공처
	char *time;							// 생성 시간
	char *cctvUrl;						// CCTV URL
}NDDSTMAP_FINDCCTVRESULTBYID_RES;


//////////////////////////////////////////////////////////////////////////
// "5.6. 기타" 관련 구조체 정의
//////////////////////////////////////////////////////////////////////////
// 단말인증 (for Smart Car) 정보


// 5.6.9. 위치 공유 요청
typedef struct tagNddsTmapPositionSharingReq
{
	char *coordLongt;					//[Y] X 좌표
	char *coordLat;						//[Y] Y 좌표
	char *coordType;					//[Y] 좌표계 구분(1103:SK 정규화 좌표)
	char *addr;							//[N] 제보 장소 주소
	char *poiId;						//[N] POI ID
	char *poiName;						//[N] POI 명칭
	char *poiTel;						//[N] POI 전화번호
	char *rpFlag;						//[N] RP Flag
}NDDSTMAP_POSITIONSHARING_REQ;
// 5.6.9. 위치 공유 응답
typedef struct tagNddsTmapPositionSharingRes
{
	NDDS_COMMONHEADER_RES	header;		// 공통 해더
	NDDS_MEMORYSTATUS		memory;		// 메모리 관리

	char *tinyUrl;						// 단축 URL(MobileWeb 링크 URL)
}NDDSTMAP_POSITIONSHARING_RES;


// 5.6.10. 경로공유 - 자동차 요청
typedef struct tagNddsTmapRouteSharingCarReq
{
	unsigned char *routingData;					//[Y] 경로정보 데이터
	int routingDataSize;				//[N] 경로정보 데이타 크기
	char *routingDataVer;				//[N] 경로정보 데이타 포맷 버전
}NDDSTMAP_ROUTESHARINGCAR_REQ;
// 5.6.10. 경로공유 - 자동차 응답
typedef struct tagNddsTmapRouteSharingCarRes
{
	NDDS_COMMONHEADER_RES	header;		// 공통 해더
	NDDS_MEMORYSTATUS		memory;		// 메모리 관리

	char *tinyUrl;						// 단축 URL(MobileWeb 링크 URL)
}NDDSTMAP_ROUTESHARINGCAR_RES;


// 5.6.11. 경로공유 - 대중교통 요청
typedef struct tagNddsTmapRouteSharingPublicReq
{
	unsigned char *routingData;					//[Y] 경로정보 데이터
	int routingDataSize;				//[N] 경로정보 데이타 크기
	char *routingDataVer;				//[N] 경로정보 데이타 포맷 버전
}NDDSTMAP_ROUTESHARINGPUBLIC_REQ;
// 5.6.11. 경로공유 - 대중교통 응답
typedef struct tagNddsTmapRouteSharingPublicRes
{
	NDDS_COMMONHEADER_RES	header;		// 공통 해더
	NDDS_MEMORYSTATUS		memory;		// 메모리 관리

	char *tinyUrl;						// 단축 URL(MobileWeb 링크 URL)
}NDDSTMAP_ROUTESHARINGPUBLIC_RES;


// 5.6.12. 경로공유 - 도보 요청
typedef struct tagNddsTmapRouteSharingWalkReq
{
	unsigned char *routingData;					//[Y] 경로정보 데이터
	int routingDataSize;				//[N] 경로정보 데이타 크기
	char *routingDataVer;				//[N] 경로정보 데이타 포맷 버전
}NDDSTMAP_ROUTESHARINGWALK_REQ;
// 5.6.12. 경로공유 - 도보 응답
typedef struct tagNddsTmapRouteSharingWalkRes
{
	NDDS_COMMONHEADER_RES	header;		// 공통 해더
	NDDS_MEMORYSTATUS		memory;		// 메모리 관리

	char *tinyUrl;						// 단축 URL(MobileWeb 링크 URL)
}NDDSTMAP_ROUTESHARINGWALK_RES;


// 5.6.13. 경로공유 - 주행경로 요청
typedef struct tagNddsTmapRouteSharingDrivingReq
{
	unsigned char *routingData;					//[Y] 경로정보 데이터
	int routingDataSize;				//[N] 경로정보 데이타 크기
	char *routingDataVer;				//[N] 경로정보 데이타 포맷 버전
}NDDSTMAP_ROUTESHARINGDRIVING_REQ;
// 5.6.13. 경로공유 - 주행경로 응답
typedef struct tagNddsTmapRouteSharingDrivingRes
{
	NDDS_COMMONHEADER_RES	header;		// 공통 해더
	NDDS_MEMORYSTATUS		memory;		// 메모리 관리

	char *tinyUrl;						// 단축 URL(MobileWeb 링크 URL)
}NDDSTMAP_ROUTESHARINGDRIVING_RES;

// 5.6.19. 차량 전송 정보 저장 요청
typedef struct tagNddsTmapRegisterSendToCarInfoReq
{
	char *custName;						//[Y] POI 명칭(최대100byte)
	char *noorX;						//[Y] X좌표
	char *noorY;						//[Y] Y좌표
	char *poiId;						//[N] POI ID
	unsigned char rpFlag;				//[N] RP Flag
	char *navSeq;						//[N] 입구점 일련번호
}NDDSTMAP_REGISTERSENDTOCARINFO_REQ;
// 5.6.19. 차량 전송 정보 저장 응답
typedef struct tagNddsTmapRegisterSendToCarInfoRes
{
	NDDS_COMMONHEADER_RES	header;		// 공통 해더
	NDDS_MEMORYSTATUS		memory;		// 메모리 관리

}NDDSTMAP_REGISTERSENDTOCARINFO_RES;

// 5.6.20. 차량 전송 정보 조회 요청
typedef struct tagNddsTmapFindSendToCarInfoReq
{
}NDDSTMAP_FINDSENDTOCARINFO_REQ;
// 5.6.20. 차량 전송 정보 조회 응답
typedef struct tagNddsTmapFindSendToCarInfoRes
{
	NDDS_COMMONHEADER_RES	header;		// 공통 해더
	NDDS_MEMORYSTATUS		memory;		// 메모리 관리

	char *poiId;		// POI ID
	char *navSeq;		// 입구점 일련번호
	char *custName;		// POI 명칭
	char *noorX;		// X좌표
	char *noorY;		// Y좌표
	char *lcdName;		// 지역 대분류명
	char *mcdName;		// 지역 중분류명
	char *scdName;		// 지역 소분류명
	char *dcdName;		// 지역 세분류명
	char *primaryBun;	// 본번
	char *secondaryBun;	// 부번
	char *mlClass;		// 산/대지 구분자(null,0,1:대지, 2:산)
	char *roadName;		// 도로명
	char *bldNo1;		// 건물번호1
	char *bldNo2;		// 건물번호2
	char *repClsName;	// 대표업종명 (퀵인포에 들어갈 항목)
	char *clsAName;		// 업종 대분류명
	char *clsBName;		// 업종 중분류명
	char *clsCName;		// 업종 소분류명
	char *clsDName;		// 업종 상세분류명
	unsigned char rpFlag;	// RP Flag
	char *telNo;		// 전화번호
	char *insDatetime;	// 입력일시(YYYYMMDDHHMISS)
}NDDSTMAP_FINDSENDTOCARINFO_RES;

// 5.6.21. 차량 전송 정보 삭제 요청 요청
typedef struct tagNddsTmapRemoveSendToCarInfoReq
{
	char *custName;						//[Y] POI 명칭(최대100byte)
	char *noorX;						//[Y] X좌표
	char *noorY;						//[Y] Y좌표
}NDDSTMAP_REMOVESENDTOCARINFO_REQ;
// 5.6.21. 차량 전송 정보 삭제 요청 응답
typedef struct tagNddsTmapRemoveSendToCarInfoRes
{
	NDDS_COMMONHEADER_RES	header;		// 공통 해더
	NDDS_MEMORYSTATUS		memory;		// 메모리 관리

}NDDSTMAP_REMOVESENDTOCARINFO_RES;




//////////////////////////////////////////////////////////////////////////
// "5.7. 기타" 관련 구조체 정의
//////////////////////////////////////////////////////////////////////////

// 공지사항 컨텐츠 정보 리스트 정보
typedef struct tagNddsTmapContentsDetailsDto
{
	char *regDate;						// 공지사항 등록일(YYYYMMDDhh)
	char *title;						// 공지사항 제목
	char *content;						// 공지사항 내용
	char *url;							// 공지사항 정보 URL
	char *email;						// 공지사항 email
	char *tel;							// 공지사항 전화번호
}NDDSTMAP_CONTENTSDETAILS_DTO;

// 오픈 앱 정보 리스트 정보
typedef struct tagNddsTmapOepnAppDetailsDto
{
	char *title;						// 제목
	char *downUrl;						// 다운로드 url정보
	char *packageName;					// 패키지 이름
	char *appId;						// 앱 아이디
	char *chargeYn;						// 유/무료 구분("N":무료, "Y":유료)
	char *recommYn;						// 추천/비추천 구분("N":비추천, "Y":추천)
	int nAppImg;						// 오픈 앱 이미지 파일 크기
	char *appImg;						// 오픈 앱 이미지 파일 데이터(binary정보를 Base64로 인코딩 한 데이터 정보)
}NDDSTMAP_OPENAPPDETAILS_DTO;

// 팝업 공지사항 정보 리스트 정보
typedef struct tagNddsTmapPopupNoticeDetailsDto
{
	char *regDate;						// 공지사항 등록일(YYYYMMDDhh)
	char *title;						// 공지사항 제목
	char *content;						// 공지사항 내용
	char *url;							// 공지사항 정보 URL
	char *email;						// 공지사항 email
	char *tel;							// 공지사항 전화번호
    int nImageSize;                     // 이미지 사이즈
	char *image;						// 이미지 데이터(Base64 인코딩 데이터) 이미지 데이터가 있을 경우 공지사항을 이미지로 표시한다.
}NDDSTMAP_POPUPNOTICEDETAILS_DTO;

// 추천 검색어 정보 리스트 정보
typedef struct tagNddsTmapRecommSearchDetailsDto
{
	char *tmnWord;						// 단말 표출어
	char *recommWord;					// 실제 검색어
}NDDSTMAP_RECOMMSEARCHDETAILS_DTO;




// 5.7.1 공지사항(요금제) 요청 / 5.7.3 메뉴 공지사항 요청
typedef struct tagNddsTmapFindChargeNoticesReq
{
}NDDSTMAP_FINDCHARGENOTICES_REQ;
// 5.7.1 공지사항(요금제) 요청 / 5.7.3 메뉴 공지사항 응답
typedef struct tagNddsTmapFindChargeNoticesRes
{
	NDDS_COMMONHEADER_RES	header;		// 공통 해더
	NDDS_MEMORYSTATUS		memory;		// 메모리 관리

	char *noticeVersion;						// 공지사항(요금제) 버전 정보
	int contentsCount;							// 공지사항 컨텐츠 정보 개수
	NDDSTMAP_CONTENTSDETAILS_DTO *contentsDetails;	// 공지사항 컨텐츠 정보 리스트
}NDDSTMAP_FINDCHARGENOTICES_RES;


// 5.7.2 파일(이미지, 기타) 정보 요청
typedef struct tagNddsTmapFindFileAccessReq
{
	int nFilePath;						//[Y] 파일 정보 개수
	char **filePath;					//[Y] 이미지, 기타 파일에 대한 파일 정보(경로+파일명)
}NDDSTMAP_FINDFILEACCESS_REQ;

typedef struct tagNddsTmapFindFileDataDto{
	int nFileSize;
	unsigned char *fileData;
}NDDSTMAP_FIND_FILEDATA_DTO;

// 5.7.2 파일(이미지, 기타) 정보 응답
typedef struct tagNddsTmapFindFileAccessRes
{
	NDDS_COMMONHEADER_RES	header;		// 공통 해더
	NDDS_MEMORYSTATUS		memory;		// 메모리 관리

	int nFileData;						// 데이터 개수
	NDDSTMAP_FIND_FILEDATA_DTO *fileDataList;					// 이미지. 기타 파일에 대한 파일 데이타 배열 (binary정보를 Base64로 인코딩 한 데이터 정보)
}NDDSTMAP_FINDFILEACCESS_RES;

// 5.7.4 오픈 앱 리스트 요청
typedef struct tagNddsTmapFindOpenAppsReq
{
}NDDSTMAP_FINDOPENAPPS_REQ;
// 5.7.4 오픈 앱 리스트 응답
typedef struct tagNDDSFindOpenAppsRes
{
	NDDS_COMMONHEADER_RES	header;		// 공통 해더
	NDDS_MEMORYSTATUS		memory;		// 메모리 관리

	char *openAppVer;							// 오픈 앱 버전 정보
	int openAppCount;							// 오픈 앱 정보 개수
	NDDSTMAP_OPENAPPDETAILS_DTO *openAppDetails;	// 오픈 앱 정보 리스트
}NDDSTMAP_FINDOPENAPPS_RES;


// 5.7.9  SeedKey 정보 요청
typedef struct tagNddsTmapFindSeedKeyInfoReq
{
}NDDSTMAP_FINDSEEDKEYINFO_REQ;
// 5.7.9  SeedKey 정보 응답
typedef struct tagNddsTmapFindSeedKeyInfoRes
{
	NDDS_COMMONHEADER_RES	header;		// 공통 해더
	NDDS_MEMORYSTATUS		memory;		// 메모리 관리

	char * seedVersion;					// SeedKey 버전 정보 YYYYMMDDI (I는 인덱스) 		예) 20120131A
	char ** seedKeyInfo;				//SeedKey 파일 데이터 정보(10개) 	(Base64로 인코딩 된 데이터 정보)
}NDDSTMAP_FINDSEEDKEYINFO_RES;


// 5.7.10 App Log 저장 요청
typedef struct tagNddsTmapAppLogSaveReq
{
	unsigned char appType;						//[Y] App 구분 (0x01: BlackBox, 0x02: 교통정보, 0x03: 대중교통, 0x04: 차계부, 0x05: Bike(Tmap 자전거))
	unsigned char runType;						//[Y] App 구동 정보 (0x01 : Main App에서 구동, 0x02 : 자체 App 구동, 0x03 : Family App에서 구동)
}NDDSTMAP_APPLOGSAVE_REQ;
// 5.7.10 App Log 저장 응답
typedef struct tagNddsTmapAppLogSaveRes
{
	NDDS_COMMONHEADER_RES	header;		// 공통 해더
	NDDS_MEMORYSTATUS		memory;		// 메모리 관리

	unsigned char appType;			// 0x01 : Black Box,0x02 : 교통정보 App,0x03 : 대중교통 App,0x04 : 차계부 App,0x05 : Bike App(Tmap 자전거)
	unsigned char runType;			// App 구동 정보,0x01 : Main App에서 구동,0x02 : 자체 App 구동,	0x03 : Family App에서 구동
}NDDSTMAP_APPLOGSAVE_RES;


// 5.7.11 주행 이력 저장 요청
typedef struct tagNddsTmapRouteHistorySaveReq
{
	char *departName;				//[Y] 출발지 명칭
	int departXPos;					//[Y] 출발지 X 좌표
	int departYPos;					//[Y] 출발지 Y 좌표
	char *destName;					//[Y] 목적지 명칭
	int destXPos;					//[Y] 목적지 X 좌표
	int destYPos;					//[Y] 목적지 Y 좌표
	char *destPoiId;				//[Y] 목적지 POI ID
	unsigned char destRpFlag;				//[Y] 목적지 RP Flag
	int totalDistance;				//[Y] 주행거리(미터)
	int totalTime;					//[Y] 소요시간(분)
	int averageSpeed;				//[Y] 평균속도(km/h)
	int maxSpeed;					//[Y] 최고속도(km/h)
	char *tvasEstimationTime;		//[Y] TVAS 도착 예정 시간(YYYYMMDDHHMMSS)
	char *realEstimationTime;		//[Y] 실제 도착 시간(YYYYMMDDHHMMSS)
	int reserchCnt;					//[Y] 재탐색 횟수(모든 재탐색 횟수)
	char *routeSessionId;			//[Y] 마지막 길안내 응답받은 세션ID
	char *fuelSavingCost;			//[Y] 연료 젃감액(단위는 리터, 00.00 형식, 예: 12.03)
	char *co2Quantity;				//[Y] CO2 배출량(단위는 Kg CO2, 00.00 형식, 예 : 10.03)
	char *routeEndDivision;		//[Y] 경로 종료 구붂(1 : 경로 앆내 완료, 2 : 사용자 취소)
	int	routeRevisitingCount;		//[Y] 이탈 재탐색 횟수
	char *greenHouseGasReduction;	//[Y] 온실가스 저감량(단위는 t, 00.00 형식, 예: 0.013)
}NDDSTMAP_ROUTEHISTORYSAVE_REQ;

// 5.7.11 주행 이력 저장 응답
typedef struct tagNddsTmapRouteHistorySaveRes
{
	NDDS_COMMONHEADER_RES	header;		// 공통 해더
	NDDS_MEMORYSTATUS		memory;		// 메모리 관리

}NDDSTMAP_ROUTEHISTORYSAVE_RES;






//////////////////////////////////////////////////////////////////////////
// "5.8. 광고" 관련 구조체 정의
//////////////////////////////////////////////////////////////////////////




// 광고 음원 컨텐츠 정보 리스트 정보
typedef struct tagNddsTmapAdvtReportDetailsDto
{
	char *playDate;						//[Y] 재생 시각(YYYYMMDDHHMMSS)
	char *adType;						//[Y] 광고 음원 타입 ("AD01": 길안내 시작, "AD02": 길안내 종료,  "AD03": 시보)
	char *adPlayTime;					//[Y] 광고 음원 재생 시각(HH) (시보의 경우에만 해당되며 지정된 시각에 Play 한다.)  예) 00 ~ 23
	char *adCode;						//[Y] 광고 음원 코드 예) ST20130516A, EN20130516A, 0720130516A
	//char *adAreaCode;					//[N] 광고 음원 재생 지역 코드 - 사용하지 않음)
	char *completeType;					//[Y] 재생 성공 여부 ("11": 재생 성공(끝까지 Play한 경우) 12": 재생 성공(Play는 했으나 끝까지 Play하지 못한 경우) (사용 안함)  13": 길안내 음원과 중복으로 인한 미재생 14": 사용자 설정으로 인한 미재생(시보의 경우))
	char *runningTime;					//[Y] 실제 재생 시간(단위:초)
	char *routeSessionId;				//[N] 마지막 길안내 응답으로 받은 세션ID
}NDDSTMAP_ADVTREPORTDETAILS_DTO;


// 5.8.2. 광고 음원 노출 보고 요청
typedef struct tagNddsTmapSaveAdvertiseReq
{
	char *reportPoint;					//[Y] 보고 시점 ("11": 길안내 시작 시 보고 "12": 길안내 종료 시 보고)
	int nAdvtReportDetails;							//[Y] 광고 음원 노출 보고 개수
	NDDSTMAP_ADVTREPORTDETAILS_DTO *advtReportDetails;	//[Y] 광고 음원 노출 보고 리스트
}NDDSTMAP_SAVEADVERTISE_REQ;
// 5.8.2. 광고 음원 노출 보고 응답
typedef struct tagNddsTmapSaveAdvertiseRes
{
	NDDS_COMMONHEADER_RES	header;		// 공통 해더
	NDDS_MEMORYSTATUS		memory;		// 메모리 관리

}NDDSTMAP_SAVEADVERTISE_RES;






//////////////////////////////////////////////////////////////////////////
// "5.9. 이용약관 동의" 관련 구조체 정의
//////////////////////////////////////////////////////////////////////////


// 약관 동의 정보 리스트
typedef struct tagNddsTmapTermsDetailsDto
{
	char *title;						// 제목
	char *url;							// 약관동의 모바일 페이지 URL
	char *urlType;						// 약관동의 모바일 페이지 구분 ("B2C": B2C약관 페이지, "B2B": B2B약관 페이지)
}NDDSTMAP_TERMSDETAILS_DTO;

// 약관 동의 항목 리스트
typedef struct tagNddsTmapTermsAgreementsDto
{
	char *allowTitle;					//[Y] 약관항목 타이틀, 개별항목에 대한 타이틀을 설정한다.
	char *allowCode;					//[Y] 약관 항목 코드
	char *allowYn;						//[Y] 약관 항목 동의여부 ("Y" / "N")
}NDDSTMAP_TERMSAGREEMENTS_DTO;




// 5.9.1. 이용약관 동의 여부 조회 요청
typedef struct tagNddsTmapFindAgreementReq
{
	char *termsType;					//[Y] 이용약관 구분 ("TMS01": 어플리케이션 이용 동의 "TMS02": IDP 이용 동의)
}NDDSTMAP_FINDAGREEMENT_REQ;
// 5.9.1. 이용약관 동의 여부 조회 응답
typedef struct tagNddsTmapFindAgreementRes
{
	NDDS_COMMONHEADER_RES	header;		// 공통 해더
	NDDS_MEMORYSTATUS		memory;		// 메모리 관리

	unsigned char termsResult;					// 약관 동의 여부 (0x01 : 약관 동의 0x02 : 약관 동의 하지 않음)
	char *termsVersion;					// 약관 버전 정보 YYYYMMDDI (I는 인덱스) 예) 20120131A
	char *termsTitle;					// 약관 동의 정보 제목 (termsResult 값이 0x02일때만 데이타 셋팅)
	char *termsUrl;						// 약관 동의 모바일 페이지 URL(termsResult 값이 0x02일때만 데이타 셋팅)
	char *termsUrlType;					// 약관동의 모바일 페이지 구분("B2C": B2C약관 페이지, "B2B": B2B약관 페이지)(termsResult 값이 0x02일때만 데이타 셋팅)
    int nTermsAgreements;               /// 약관 동의 항목 개수
    NDDSTMAP_TERMSAGREEMENTS_DTO *termsAgreements;	//약관 동의 항목 리스트 (termsResult 값이 0x01일때맊 데이타 셋팅)
}NDDSTMAP_FINDAGREEMENT_RES;


// 5.9.2. 이용약관 동의 결과 저장 요청
typedef struct tagNddsTmapRegistAgreementReq
{
	char *termsVersion;					//[Y] 약관 버전 정보 (이용약관 동의 여부 조회 시 전달받은 버전) 예) 20120131A
	char *termsType;					//[Y] 이용약관 구분 ("TMS01": 어플리케이션 이용 동의, "TMS02": IDP 이용 동의)
	char *termsUrlType;					//[Y] 약관동의 모바일 페이지 구붂("B2C" 또는 "B2B")
	int nTermsAgreements;						//[Y] 약관 동의 항목 개수
	NDDSTMAP_TERMSAGREEMENTS_DTO *termsAgreements;	//[Y] 약관 동의 항목 리스트
}NDDSTMAP_REGISTAGREEMENT_REQ;
// 5.9.2. 이용약관 동의 결과 저장 응답
typedef struct tagNddsTmapRegistAgreementRes
{
	NDDS_COMMONHEADER_RES	header;		// 공통 해더
	NDDS_MEMORYSTATUS		memory;		// 메모리 관리

}NDDSTMAP_REGISTAGREEMENT_RES;

// - 헤드유닛 장치정보(headunitdeviceinfo)
typedef struct tagNddsTmapHeadUnitDeviceInfoDto{
	char *deviceId;
	char *deviceModelName;
	char *deviceModelNo;
	char *vendor;
	char *serialKey;
}NDDSTMAP_HEADUNITDEVICEINFO_DTO;

// - T아이디 인증 정보(tidAuthInfo)
typedef struct tagNddsTmapTidAuthInfoDto{
	char *idToken;
	char *ist;
}NDDSTMAP_TIDAUTHINFO_DTO;

// - T아이디 인증 요청 정보(tidAuthReqInfo)
typedef struct tagNddsTmapTidAuthReqInfoDto{
	char *state;
	int chainExternalAuth;
	NDDSTMAP_TIDAUTHINFO_DTO tidauthinfo;
}NDDSTMAP_TIDAUTHREQINFO_DTO;

// 5.6.3. 단말인증 (for Smart Car) 요청
typedef struct tagNddsTmapAuthForSmartCarReq
{
	NDDSTMAP_HEADUNITDEVICEINFO_DTO headunitdeviceinfo;
	NDDSTMAP_TIDAUTHREQINFO_DTO tidauthreqinfo;
	char *otpCode;
	char *authCode;
}NDDSTMAP_AUTHFORSMARTCAR_REQ;

// 외부 인증 정보
typedef struct tagNddsTmapExternalUserInfosDto
{
	char *provider;
	char *readableUserId;
}NDDSTMAP_EXTERNALUSERINOFS_DTO;

// 단말인증 응답 사용자 인증 결과
typedef struct tagNddsTmapAuthInfoDto
{
	int validateCode;
	char *validationSubField;
	int externalUserInfoCount;
	NDDSTMAP_EXTERNALUSERINOFS_DTO *externalUserInfos;
}NDDSTMAP_AUTHINFO_DTO;

// 단말인증 응답 기기 인증 결과
typedef struct tagNddsTmapHeadUnitDeviceAuthInfoDto
{
	int resultCode;
	int resultSubField;
}NDDSTMAP_HEADUNITDEVICEAUTHINFO_DTO;

// 단말인증 응답 각종 버전 정보
typedef struct tagNddsTmapVersionInfoDto
{
	char *menuNoticeVer;
	char *openappVer;
}NDDSTMAP_VERSIONINFO_DTO;

// 단말인증 응답 APP 업데이트 정보
typedef struct tagNddsTmapAppUpdateInfoDto
{
	char *appUpdateType;
	char *appVer;
	char *appUpdateViewFlag;
	char *appUpdateUrl;
	char *appUpdateMsg;
}NDDSTMAP_APPUPDATEINFO_DTO;

// 단말인증 응답 APP 제어 정보 리스트
typedef struct tagNddsTmapControlInfoDetailsDto
{
	char *serviceName;
	char *serviceYn;
	char *serviceItems;
}NDDSTMAP_CONTROLINFODETAILS_DTO;

// 단말인증 응답 APP 제어 정보
typedef struct tagNddsTmapAppControlInfoDto
{
	int controlInfoCount;
	NDDSTMAP_CONTROLINFODETAILS_DTO *controlInfoDetails; 
}NDDSTMAP_APPCONTROLINFO_DTO;

// 공통 응답 정보
typedef struct tagNddsTmapCommonInfoDto
{
	char *euk;
	char *eid;
}NDDSTMAP_COMMONINFO_DTO;

// T아이디 인증 응답 정보
typedef struct tagNddsTmapTidAuthResInfoDto
{
	int resultCode;
	int resultSubField;
	int resultMdnAuthInfo;
	int externalUserInfoCount;
	NDDSTMAP_EXTERNALUSERINOFS_DTO *externalUserInfos;
}NDDSTMAP_TIDAUTHRESINFO_DTO;

// Push 설정 정보
typedef struct tagNddsTmapPushInfoDto
{
	char *pushSetYn;
	char *smsYn;
	char *momentYn;
}NDDSTMAP_PUSHINFO_DTO;

// 개인 정보 데이터
typedef struct tagNddsTmapPersonalInfoDto
{
	char *email;
	char *userName;
	char *mdn;
	char *birth;
}NDDSTMAP_PERSONALINFO_DTO;

// 차량 정보 데이터
typedef struct tagNddsTmapCarInfoDto
{
	char *model;
	char *fuel;
	char *hipassYn;
	char *number;
}NDDSTMAP_CARINFO_DTO;

// 사용자 정보
typedef struct tagNddsTmapUserProfileInfoDto
{
	NDDSTMAP_PERSONALINFO_DTO *personalInfo;
	NDDSTMAP_CARINFO_DTO *carInfo;
}NDDSTMAP_USERPROFILEINFO_DTO;

// 5.6.3. 단말인증 (for Smart Car) 응답
typedef struct tagNddsTmapAuthForSmartCarRes
{
	NDDS_COMMONHEADER_RES	header;		// 공통 해더
	NDDS_MEMORYSTATUS		memory;		// 메모리 관리

	NDDSTMAP_COMMONINFO_DTO *commonInfo;
	NDDSTMAP_AUTHINFO_DTO *authInfo;
	NDDSTMAP_HEADUNITDEVICEAUTHINFO_DTO *headUnitDeviceAuthInfo;
	NDDSTMAP_TIDAUTHRESINFO_DTO *tidAuthResInfo;
	NDDSTMAP_VERSIONINFO_DTO *versionInfo;
	NDDSTMAP_APPUPDATEINFO_DTO *appUpdateInfo;
	NDDSTMAP_APPCONTROLINFO_DTO *appControlInfo;
	NDDSTMAP_PUSHINFO_DTO *pushInfo;
	NDDSTMAP_USERPROFILEINFO_DTO *userProfileInfo;
}NDDSTMAP_AUTHFORSMARTCAR_RES;

// 5.3.15. User-Profile 조회 (요청)
typedef struct tagNddsTmapFindUserProfileReg
{
}NDDSTMAP_FINDUSERPROFILE_REQ;
// 5.3.15. User-Profile 조회 (응답)
typedef struct tagNddsTmapFindUserProfileRes
{
    NDDS_COMMONHEADER_RES	header;			// 공통 해더
    NDDS_MEMORYSTATUS		memory;			// 메모리 관리

    NDDSTMAP_PERSONALINFO_DTO	userlInfo;	///user-profile
}NDDSTMAP_FINDUSERPROFILE_RES;

// 5.3.16. User-Profile 저장 (요청)
typedef struct tagNddsTmapRegistUserProfileReg
{
    NDDSTMAP_PERSONALINFO_DTO	userlInfo;	///user-profile
}NDDSTMAP_REGISTUSERPROFILE_REQ;
// 5.3.16. User-Profile 저장 (응답)
typedef struct tagNddsTmapRegistUserProfileRes
{
    NDDS_COMMONHEADER_RES	header;		// 공통 해더
    NDDS_MEMORYSTATUS		memory;		// 메모리 관리
}NDDSTMAP_REGISTUSERPROFILE_RES;

// 5.3.17. Car-Profile 조회 (요청)
typedef struct tagNddsTmapFindCarProfileReg
{
}NDDSTMAP_FINDCARPROFILE_REQ;
// 5.3.16. Car-Profile 조회 (응답)
typedef struct tagNddsTmapFindCarProfileRes
{
    NDDS_COMMONHEADER_RES	header;		// 공통 해더
    NDDS_MEMORYSTATUS		memory;		// 메모리 관리
}NDDSTMAP_FINDCARPROFILE_RES;

// 5.3.17. Car-Profile 저장 (요청)
typedef struct tagNddsTmapRegistCarProfileReg
{
    NDDSTMAP_CARINFO_DTO	carlInfo;	///car-profile
}NDDSTMAP_REGISTCARPROFILE_REQ;
// 5.3.17. Car-Profile 저장 (응답)
typedef struct tagNddsTmapRegistCarProfileRes
{
    NDDS_COMMONHEADER_RES	header;		// 공통 해더
    NDDS_MEMORYSTATUS		memory;		// 메모리 관리
}NDDSTMAP_REGISTCARPROFILE_RES;


// AutoComplete, R-Geocode, ComplexImage, AQI. for vNext. 2019.8.8 by hheo
typedef struct tagNddsTmapNonNddsReq
{
    char *pszUrl;
    char *pszDomain;
    char **ppszResult;
}NDDSTMAP_NONNDDS_REQ;
typedef struct tagNddsTmapNonNddsRes
{
    NDDS_COMMONHEADER_RES	header;		// 공통 해더
    NDDS_MEMORYSTATUS		memory;		// 메모리 관리

    char *pszResult;
    int nResult;
}NDDSTMAP_NONNDDS_RES;


#if defined(__WIN32__) || defined(_WIN32) || defined(WIN32) || defined(__WINDOWS__) || defined(__TOS_WIN__)
#include <windows.h>
inline void delay( unsigned long ms )
{
	Sleep( ms );
}

#else  /* presume POSIX */
#include <unistd.h>
#ifndef __QNXNTO__
inline void delay( unsigned long ms )
{
	usleep( ms * 1000 );
}
#endif
inline unsigned long GetTickCount() {
    struct timespec ts;
    unsigned long theTick = 0U;
    clock_gettime( CLOCK_MONOTONIC, &ts );
    theTick  = ts.tv_nsec / 1000000;
    theTick += ts.tv_sec * 1000;
    return theTick;
}
#endif

#define TIME_DELAY		10	// 프로그래스 설정을 위해 통신중 지연
#define CONNECT_TIMEOUT	5	// 서버 접속 타입아웃(sec)

#define RETRY_DELAY		100  // SSL 통신에서 실패 시 재시도 간격
#define RETRY_COUNT		100	// 서버 재접속 횟수. 재시도 간격(RETRY_COUNT)이 100ms 이므로 최대 10초 동안 재시도

#endif // __NDDS_TMAP_DEF_H__
