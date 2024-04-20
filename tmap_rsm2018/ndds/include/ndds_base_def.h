// ndds_base_def.h

#ifndef __NDDS_BASE_DEF_H__
#define __NDDS_BASE_DEF_H__

#include "global.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <wchar.h>
#ifdef _LINUX
#include <pthread.h>
#endif

//////////////////////////////////////////////////////////////////////////
// 배열 데이터 크기 정의
//////////////////////////////////////////////////////////////////////////
#define NDDS_BINARY_ERROR_CODE_LEN				6
#define NDDS_BINARY_ERROR_MESSAGE_LEN			100
#define NDDS_BINARY_ERROR_DETAIL_CODE_LEN		10
#define NDDS_BINARY_ERROR_DETAIL_MESSAGE_LEN	100
#define NDDS_BINARY_SESSION_ID_LEN				24
#define NDDS_BINARY_COMMON_HEADER_LEN			240
#define NDDS_LOGIN_INFO_LEN						64

#define NDDS_HEADER_REQTIME_LEN					15
#define NDDS_HEADER_ID_LEN					90
#define NDDS_HEADER_MAC_LEN						50
#define NDDS_HEADER_APPVER_LEN					10
#define NDDS_HEADER_DEFAULT_LEN					30
#define NDDS_HEADER_MDN_LEN						64

#define NDDS_HEADER_CARRIER_LEN					20
#define NDDS_HEADER_DEVICEID_LEN				21

//////////////////////////////////////////////////////////////////////////
// 에러 코드 및 프로토콜 타입 정의
//////////////////////////////////////////////////////////////////////////

#define NDDS_SIZE_PLANNINGROUTE_ROUTESUMMARYCODE 5
#define NDDS_SIZE_PLANNINGROUTE_DESTPOIID		10


// NDDS 에러 코드
typedef enum
{
	NDDSERR_SUCCESS					= 1,			// 성공
	NDDSERR_UNKNOWN					= -1000,		// 실패(알수 없음)
	NDDSERR_MULTIPLEACCESS,							// 다중접속 실패++
	NDDSERR_MEMORYLACK,								// 메모리 부족
	NDDSERR_USERCANCEL,								// 사용자 취소
	NDDSERR_MAKEREQUEST,							// 요청 데이터 생성 실패
	NDDSERR_SOCKETCREATE,							// 소켓 생성 실패
	NDDSERR_SOCKETHOST,								// 소켓 호스트 확인 실패
	NDDSERR_SOCKETCONNECT,							// 소켓 접속 실패
	NDDSERR_SOCKETSEND,								// 소켓 요청 데이터 전송 실패
	NDDSERR_SOCKETRECEIVE,							// 소켓 데이터 수신 실패
	NDDSERR_SOCKETRECEIVE_EMPTY,					// 소켓 데이터 수신 제로(값이 없음)
	NDDSERR_HTTPPROTOCOL,							// Http 프로토콜 에러
	NDDSERR_JSONSENTENCE,							// 결과 데이터 JSON 파싱 실패
	NDDSERR_RES_COMMONHEADER,						// 결과 데이터 공통해더 파싱 실패
	NDDSERR_RES_BODY,								// 결과 데이터 파싱 실패
	NDDSERR_RES_FAIL,								// 서버 실패(공통해더에 에러 코드 및 메세지 존재)
	NDDSERR_TIMEOUT,								// 타임아웃
	NDDSERR_SERVICE_CLOSE_FAIL,						// 서비스 종료 실패
	NDDSERR_SSL_CTX,								// SSL CTX 생성 실패
	NDDSERR_SSL_NEW,								// SSL 생성 실패
	NDDSERR_SSL_CONNECT,							// SSL 연결 실패
	NDDSERR_SSL_CERT,								// SSL 서버 인증 실패
    NDDSERR_SSL_ECM_INIT,                           // SSL ECM 생성 실패
    NDDSERR_SSL_ECM_CONF,                           // SSL ECM 설정 실패
    NDDSERR_SSL_ECM_CONF_OPT,                       // SSL ECM 설정 실패
    NDDSERR_SSL_ECM_REL_CONF,                       // SSL ECM 설정 실패
    NDDSERR_SSL_ECM_CONF_FINISH,                    // SSL ECM 설정 마무리 실패
    NDDSERR_SSL_ECM_TO_CTX,                         // SSL ECM CTX 생성 실패
    NDDSERR_SSL_ECM_BIO_GET_SSL,                    // SSL ECM SSL 가져오기 실패
    NDDSERR_SSL_ECM_CONNECT,                        // SSL ECM 연결 실패
    NDDSERR_SSL_ECM_HANDSHAKE,                      // SSL ECM Handshake 실패
    NDDSERR_SESSION_NULL,                           // session pointer가 NULL인 경우
    NDDSERR_REQUEST_NULL,                           // request data pointer가 NULL인 경우
    NDDSERR_RESULT_NULL,                            // result data pointer가 NULL인 경우
    NDDSERR_THREADCREATE,                           // socket thread 생성 실패
}eNddsErrorCode;

// NDDS 인터페이스 문자 코드
typedef enum
{
	NDDSCHARACTERSET_UTF8		= 0,				// UTF-8
	NDDSCHARACTERSET_EUCKR,							// EUC-KR
}eNddsCharacterSet;

// NDDS Server 타입
typedef enum
{
	NDDSSERVER_NONE					= 0,	// 미설정 
	NDDSSERVER_DEVELOPMENT,					// 개발서버
	NDDSSERVER_SAME_COMMERCE,				// 준상용서버
	NDDSSERVER_COMMERCE,					// 상용서버
}eNddsServerType;

// NDDS 프로토콜 타입
typedef enum
{
	NDDSTYPE_NONE = 0,							//

	NDDSTYPE_JSON_STRING,

	NDDSTYPE_TMAP_PLANNINGROUTE,						// 5.1.3. 길안내
	NDDSTYPE_TMAP_ROUTESUMMARYINFO,						// 5.1.4. 경로요약

	NDDSTYPE_TMAP_FINDPOIS,								// 5.2.1  통합검색
	NDDSTYPE_TMAP_FINDAROUNDPOIS,						// 5.2.2  주변지 검색
	NDDSTYPE_TMAP_FINDPOIDETAILINFO,					// 5.2.3  POI 상세정보
	NDDSTYPE_TMAP_FINDPOIJOININFO,						// 5.2.4. 가맹점 상세정보
	NDDSTYPE_TMAP_FINDRECENTDESTNATION,					// 5.2.5. POI 최근길 조회
    NDDSTYPE_TMAP_UPLOADRECENTDESTNATION,				// 5.2.6. POI 최근길 전체 업로드
    NDDSTYPE_TMAP_REMOVERECENTDESTNATION,				// 5.2.7. POI 최근길 멀티 삭제
	/* // 소스 검증 안됨
	NDDSTYPE_TMAP_UPLOADRECENTDESTNATION,				// 5.2.6. POI 최근길 전체 업로드
	NDDSTYPE_TMAP_FINDNEARBY,							// 5.2.7. 좌표로 지명 검색
	NDDSTYPE_TMAP_FINDPICKATPOPULARS,					// 5.2.8. Pickat 인기검색어 조회
	NDDSTYPE_TMAP_FINDROADNAME,							// 5.2.9. 새주소(도로명) 검색
	NDDSTYPE_TMAP_FINDPICKATPOIDETAIL,					// 5.2.10. Pickat POI 상세정보 조회
	*/
	NDDSTYPE_TMAP_FINDAREANAMESBYSTEP,					// 5.2.11. 지역분류별 주소명 조회
    NDDSTYPE_TMAP_FINDPOISTATIONINFO,					// 5.2.12. 최저가 주유소(충전소)조회
    NDDSTYPE_TMAP_FINDPOIEVSTATIONINFO,					// 5.2.16. ev충전소 상세조회
    NDDSTYPE_TMAP_FINDPOIEVSTATIONSBYPOINT,				// 5.2.16. 주변 전기차 충전소 조회

	NDDSTYPE_TMAP_FINDPOIFAVORITE,						// 5.3.1. POI 즐겨찾기 조회
    NDDSTYPE_TMAP_REGISTPOIFAVORITEMULTI,				// 5.3.2. POI 즐겨찾기 등록
    NDDSTYPE_TMAP_UPLOADPOIFAVORITE,					// 5.3.3. POI 즐겨찾기 전체 업로드
    NDDSTYPE_TMAP_REMOVEPOIFAVORITE,                    // 5.3.4. POI 즐겨찾기 멀티 삭제
    NDDSTYPE_TMAP_MODIFYPOIFAVORITE,                    // 5.3.5. POI 쯜겨찾기 단건 수정

    NDDSTYPE_TMAP_FINDUSERPROFILE,						// 5.3.15. User-Profile 조회
    NDDSTYPE_TMAP_REGISTUSERPROFILE,					// 5.3.16. car-Profile 저장
    NDDSTYPE_TMAP_FINDCARPROFILE,						// 5.3.17. User-Profile 조회
    NDDSTYPE_TMAP_REGISTCARPROFILE,						// 5.3.18. car-Profile 저장
    NDDSTYPE_TMAP_FINDUSERDATA,							// 5.3.14. 최근길/즐겨찾기 전체 리스트 조회
	/*
     *
	NDDSTYPE_TMAP_FINDROUTE,							// 5.3.4  경로 즐겨찾기 조회
	NDDSTYPE_TMAP_REGISTROUTEMULTI,						// 5.3.5. 경로 즐겨찾기 등록
	NDDSTYPE_TMAP_UPLOADTOTALROUTE,						// 5.3.6. 경로 즐겨찾기 전체 업로드
	NDDSTYPE_TMAP_FINDTRAFFICFAVORITES,					// 5.3.7. 교통정보 즐겨찾기 조회
	NDDSTYPE_TMAP_REGISTTRAFFICFAVORITES,				// 5.3.8. 교통정보 즐겨찾기 등록
	NDDSTYPE_TMAP_UPLOADTRAFFICFAVORITES,				// 5.3.9. 교통정보 즐겨찾기 전체 업로드
    NDDSTYPE_TMAP_REMOVETRAFFICFAVORITE,				// 5.3.10. 교통정보 즐겨찾기 단일건 삭제

	NDDSTYPE_TMAP_FINDNODEJAMRANK,						// 5.4.1. 상습 정체구간 조회
	NDDSTYPE_TMAP_SMIMAGETRAFFICCONGESTION,				// 5.4.2. 이미지 교통정보
	NDDSTYPE_TMAP_RADIUSTRAFFICINFOMATION,				// 5.4.3. 주변 교통정보
	NDDSTYPE_TMAP_FINDTSDSUDN,							// 5.4.4. 돌발 정보 조회
	*/
	NDDSTYPE_TMAP_VMSGUIDEINFO,							// 5.4.5. VMS 가이드 정보 조회
	/*
	NDDSTYPE_TMAP_FINDBUSSTATION,						// 5.5.1. 버스 정류장 정보(도착 시간)
	NDDSTYPE_TMAP_FINDBUSARRIVAL,						// 5.5.2. 버스 노선 정보(도착 정보)
	NDDSTYPE_TMAP_FINDBUSARRIVALTIME,					// 5.5.3. 버스 노선 정보(도착 시간)
	NDDSTYPE_TMAP_FINDBUSSTATIONREALTIME,				// 5.5.4. 버스 정류장/노선 실시간 정보
	NDDSTYPE_TMAP_FINDSTATIONINFO,						// 5.5.5. 역 정보
	NDDSTYPE_TMAP_FINDSTATIONTIMETABLE,					// 5.5.6. 역 운행 정보
	NDDSTYPE_TMAP_FINDSUBWAYIMAGE,						// 5.5.7. 노선 이미지
	NDDSTYPE_TMAP_FINDPATHFIND1STALL4,					// 5.5.8. 경로안내(도시간 검색 포함)
	NDDSTYPE_TMAP_FINDPATHFIND1STALL4PLURAL,			// 5.5.9. 경로안내(도시내 복수 검색)
	NDDSTYPE_TMAP_FINDNEARSTATION,						// 5.5.10. 주변 정류장 / 역 조회(거리순)
	NDDSTYPE_TMAP_FINDALLTRANSPORT,						// 5.5.11. 통합 검색
	NDDSTYPE_TMAP_FINDCCTVRESULT,						// 5.5.12. 주변 CCTV 리스트
	NDDSTYPE_TMAP_FINDCCTVRESULTBYID,					// 5.5.13. CCTV 정보
	*/
	NDDSTYPE_TMAP_AUTHFORSMARTCAR,						// 5.6.3. 단말인증 (for Smart Car)
	/*
	NDDSTYPE_TMAP_POSITIONSHARING,						// 5.6.9. 위치 공유
	NDDSTYPE_TMAP_ROUTESHARINGCAR,						// 5.6.10. 경로공유 - 자동차
	NDDSTYPE_TMAP_ROUTESHARINGPUBLIC,					// 5.6.11. 경로공유 - 대중교통
	NDDSTYPE_TMAP_ROUTESHARINGWALK,						// 5.6.12. 경로공유 - 도보
	NDDSTYPE_TMAP_ROUTESHARINGDRIVING,					// 5.6.13. 경로공유 - 주행경로
	*/
	NDDSTYPE_TMAP_REGISTERSENDTOCARINFO,				// 5.6.19. 차량 전송 정보 저장
	NDDSTYPE_TMAP_FINDSENDTOCARINFO,					// 5.6.20. 차량 전송 정보 조회
	NDDSTYPE_TMAP_REMOVESENDTOCARINFO,					// 5.6.21. 차량 전송 정보 삭제

	/*
	NDDSTYPE_TMAP_FINDCHARGENOTICES,					// 5.7.1  공지사항(요금제)
	NDDSTYPE_TMAP_FINDFILEACCESS,						// 5.7.2  파일(이미지,기타) 정보
	NDDSTYPE_TMAP_FINDOPENAPPS,							// 5.7.4  오픈 앱 리스트
	NDDSTYPE_TMAP_FINDSEEDKEYINFO,						// 5.7.9  Seed Key 정보 요청
	NDDSTYPE_TMAP_APPLOGSAVE,							// 5.7.10 App Log 저장 
	NDDSTYPE_TMAP_ROUTEHISTORYSAVE,						// 5.7.11 주행 이력 저장
	NDDSTYPE_TMAP_SAVEADVERTISE,						// 5.8.2. 광고 음원 노출 보고
	NDDSTYPE_TMAP_ARRIVALREPORT,						// 5.8.3. 경로 종료 예측 보고 (도착 5분전) - 사용하지 않음
	
	NDDSTYPE_TMAP_FINDAGREEMENT,						// 5.9.1. 이용약관 동의 여부 조회
	NDDSTYPE_TMAP_REGISTAGREEMENT,						// 5.9.2. 이용약관 동의 결과 저장

	NDDSTYPE_TMAP_SERVICECLOSE,							// 서비스 종료. 
*/
    NDDSTYPE_TMAP_NONNDDS,                              // AutoComplete, R-Geocode, ComplexImage, AQI. for vNext. 2019.8.8 by hheo

    NDDSTYPE_TMAP_MAX,
}eNddsProtocolType;

// NDDS 프로토콜 요청 방식
typedef enum
{
	NDDSREQUEST_SERIALIZE = 0,						// 순차적 으로 요청
	NDDSREQUEST_ATONCE,								// 바로 요청
}eNddsRequestType;

// NDDS 프로토콜 진행 상태
typedef enum
{
	NDDSSTATUS_WAIT = 0,							// 대기 중
	NDDSSTATUS_COMMUNICATION,						// 통신 중
	NDDSSTATUS_REQUESTCANCEL,						// 취소 요청 중
	NDDSSTATUS_REQUESTTIMEOUT,						// 타임아웃
	NDDSSTATUS_CANCELED,							// 취소됨
	NDDSSTATUS_COMPLETE								// 통신 완료됨
}eNddsStatus;


//////////////////////////////////////////////////////////////////////////
// 기초 구조체 정의
//////////////////////////////////////////////////////////////////////////

// Ndds Tmap 통신 초기화 정보
typedef struct tagNddsTmapInitInfo
{
	eNddsServerType		tnAppServerType;	// [Y] TNApp 서버 타입 (개발, 상용 지원)
	eNddsServerType		nddsServerType;		// [Y] Ndds 서버 타입 (개발, 준상용, 상용 지원)

    char    szServerURL[50];                           // 서버URL
    int     nServerPort;                               // 서버Port

	char	osType[10];				// [Y] 클라이언트 OS 구분 ("AND": Android, "IOS": iOS, "WIN": window mobile, "ETC": 기타)
	char	osVersion[30];			// [Y] 단말 OS 버전
	char	modelNo[30];			// [Y] 단말 모델명	예) SHW-M250S
	char	appVersion[10];			// [Y] Application Version 	예) "3.2.7", "4.1"
	char	buildNo[10];			// [Y] 빌드번호(6자리)	빌드번호가 4자리일 경우 뒤에 "00"을 붙여서 전달한다.	예) 312200
	char	resolution[10];			// [Y] 해상도 구분 ("WVGA", "WSVGA", "HVGA", "XGA" 등)
	char	usingInfo[10];			// [Y] 서비스 사용 구분 ("MAIN": Main App에서 사용, "WIDGET": Widget에서 사용)
	char	macAddress[18];			// [Y] MAC Address
	char	phoneNumber[20];		// [Y] 핸드폰 번호
	int		width;					// [Y] 해상도
	int		height;					// [Y] 해상도
	char	deviceId[21];			// [Y] 단말 식별을 위해 단말에서 생성한 ID
	char	carrier[21];			// [Y] 이동사 구분자

	char	deviceIdHu[20+1];		// [Y] 기기(HeadUnit)의 Device ID 예) A1:B2:C3:D4:E5:F6
	char	deviceModleName[24+1];	// [Y] 기기(HeadUnit)의 단말 모델명 예) K5
	char	deviceModelNo[24+1];	// [Y] 기기(HeadUnit)의 단말 모델 번호 예) JF, LF 등
	char	vendor[24+1];			// [N] 단말 제조사명 예) KIA
	char	serialKey[32+1];		// [O] Whitelist일 경우 필수 예 ) RSM01SGA44JK43AE2ZQJIWMOM
	char	accessKey[50];			// [N] AccessKey
}NDDSTMAP_INIT_INFO;

// 응답 구조체 메모리 관리
typedef struct tagNddsMemoryStatus
{
	unsigned char *pMemory;					// 할당된 메모리 블럭 (메모리 해제시 필요)
	int nMemory;					// 할당된 메모리 블럭 크기
	unsigned char *pUse;						// 현재 사용할 메모리 블럭 위치
	int nUse;						// 사용된 메모리 블럭 크기
}NDDS_MEMORYSTATUS;

// 요청 공통 헤더
typedef struct tagNddsCommonHeaderReq{
	char	reqTime[NDDS_HEADER_REQTIME_LEN];
	char	id[NDDS_HEADER_ID_LEN];
	int		svcType;
	char	osType[NDDS_HEADER_DEFAULT_LEN];
	char	osVersion[NDDS_HEADER_DEFAULT_LEN];
	char	modelNo[NDDS_HEADER_DEFAULT_LEN];
	char	appVersion[NDDS_HEADER_APPVER_LEN];
	char	buildNo[NDDS_HEADER_APPVER_LEN];
	char	resolution[NDDS_HEADER_APPVER_LEN];
	char	seedKeyVer[NDDS_HEADER_APPVER_LEN];
	char	usingInfo[NDDS_HEADER_APPVER_LEN];
	char	macAddress[NDDS_HEADER_MAC_LEN];
	char	deviceId[NDDS_HEADER_DEVICEID_LEN];
	char	carrier[NDDS_HEADER_CARRIER_LEN];
	int		screenWidth;
	int		screenHeight;

	char	deviceIdHu[20+1];
	char	deviceModleName[24+1];	
	char	deviceModelNo[24+1];
	char	vendor[24+1];
	char	serialKey[32+1];
}NDDS_COMMONHEADER_REQ;

typedef struct tagNddsCommonReq
{
}NDDS_COMMON_REQ;

// 응답 공통 헤더
typedef struct tagNddsCommonHeaderRes
{
	int	tnappResultCode;				// TNAPP 응답 코드 (TNAPP 규격서 참조)
	int tigServiceInfo;					// 안내문구타입 (0:미가입,1:B2C 정액제, 2:B2C 종량제,3:B2B FreeZone)
	char *tigMessage;					// TIG Message
	char *errorCode;					// 오류코드 (6자리 문자열) 000000": 정상처리
	char *errorMessage;					// 오류메시지 (오류 발생시에만 존재함)
	char *errorDetailCode;				// 상세 오류코드 (오류 발생시에만 존재함)
	char *errorDetailMessage;			// 상세 오류메시지 (오류 발생시에만 존재함)
	char *sessionId;					// 세션 ID, 모든 요청에 unique 한 고유 식별 ID (log 조회 및 debug 용도)
	char *accessKey;					// Tmap Android 4.5이상 사용. 요청시의 AccessKey값이 갱신되어 변경된 경우 Client는 변경저장하여야 함.
}NDDS_COMMONHEADER_RES;

typedef struct tagNddsCommonRes
{
	NDDS_COMMONHEADER_RES	header;		// 공통 해더
	NDDS_MEMORYSTATUS		memory;		// 메모리 관리
}NDDS_COMMON_RES;

typedef struct tagNddsErrorRes
{
	NDDS_COMMONHEADER_RES	header;		// 공통 해더
	NDDS_MEMORYSTATUS		memory;		// 메모리 관리
}NDDS_ERROR_RES;

// json 직접 요청
typedef struct tagNddsJsonReq
{
	bool bIsBinaryResponse;				//응답시 binary 사용 여부
	bool bUsePrevServiceId;				//이전 서비스 id 사용 여부
	char *pServiceName;					//서비스 채널 명칭
	char *pReqData;						//json 요청 string
}NDDS_JSON_REQ;

// json 직접 응답
typedef struct tagNddsJsonRes
{
	NDDS_COMMONHEADER_RES	header;		// 공통 해더
	NDDS_MEMORYSTATUS		memory;		// 메모리 관리

	int nRespLength;					// 응답 데이터 길이
	unsigned char *pRespData;					// json/binary 응답 데이터
}NDDS_JSON_RES;

//  6.2. 요일 코드
typedef enum{
	SUNDAY = 1,		//일요일
	MONDAY,		//월요일
	TUESDAY,	//화요일
	WEDNESDAY,	//수요일
	THURSDAY,	//목요일
	FRIDAY,		//금요일
	SATURDAY,	//토요일
	HOLIDAY		//공휴일
}ENDDS_PATTERNWEEK;

// 6.3. 안내 이미지 해상도 코드
typedef enum{
	NOTSUPPORTED = 1,//	미제공
	R170X207,	// 	170X207
	R240X252,	//	240X252
	R320X316,	//	320X316
	R341X415,	//	341X415
	R480X504,	//	480X504
	R480X540	//	480X540
}ENDDS_GUIDEIMGRESOLUTIONCODE;

// 6.4 위험지역 안내 선택 옵션
typedef enum{
	NOVALUE = 1,		//	선택 없음
	OCCURFREQUENTLY,	//	사고다발
	SHARPCURVESECTION,	//	급커브
	STEEPSLOPE,		//	급경사(사용안함)
	ROADKILL,		//	로드킬(사용안함)
	MISTAREA,		//	안개
	FALLINGROCK,	//	낙석(사용안함)
	SCHOOLZONE,		//	어린이보호
	TRAINCROSSING	//	철길건널목
}ENDDS_DANGERAREAOPTIONS;

// 6.5 경로 탐색 옵션
typedef enum{
	TRAFFIC_RECOMMEND = 1,	// 	교통최적+추천
	TRAFFIC_FREE,		//	교통최적+무료우선
	TRAFFIC_MINTIME,	//	교통최적+최소시갂
	TRAFFIC_FIRSTTIME,	//	교통최적+초보
	TRAFFIC_HIGHWAY,	//	교통최적+고속도로우선
	SHORTEST_CHARGEDANDFREE, //	최단거리+유/무료
	TRAFFIC_GENERALROAD //	최단거리+일반도로
}ENDDS_FIRSTGUIDEOPTION;

// 6.6 톨케이트 요금 차종 코드
typedef enum{
	CARTYPE_NONE = 1,			//	미선택
	CARTYPE_CAR,			//	승용차
	CARTYPE_MEDIUMVAN,		//	중형승합차
	CARTYPE_LARGEVAN,		//	대형승합차
	CARTYPE_LARGETRUCK,		//	대형화물차
	CARTYPE_SPECIALTRUCK,	//	특수화물차
	CARTYPE_SMALLCAR,		//	경차
	CARTYPE_TWOWHEELEDVEHICLE //이륜차
}ENDDS_TOLLCARTYPE;

// 6.7 단속카메라 추가 코드
typedef enum{
	CAMERA_NOVALUE = 1,		//		선택없음
	CAMERA_BUS,			//		버스전용차로 단속카메라
	CAMERA_SIGNALSPEEDING,	//		싞호 단속카메라
	CAMERA_MOVING,			//		이동식 단속카메라
	CAMERA_SHOULDERCONTROL,//		갓길 단속카메라
	CAMERA_CHANGEABLEROAD,	//		가변차로 단속카메라(사용안함)
	CAMERA_OVERLOADED,		//		과적 단속카메라(사용안함)
	CAMERA_SECURITY,		//		방범 단속카메라(사용안함)
	CAMERA_TRAFFIC			//		교통정보 단속카메라
}ENDDS_ADDCAMERATYPES;

// 요청지점 도로타입
typedef enum{
    ROADTYPE_NONE = 1,		//값없음
    ROADTYPE_HIGHWAY,		//고속도로
    ROADTYPE_CARWAY,		//자동차전용도로
    ROADTYPE_OVERPASS,		//고가도로
    ROADTYPE_UNDERPASS,		//지하차도
    ROADTYPE_GENERAL,		//일반도로
    ROADTYPE_NEARWAY,		//가까운도로
    ROADTYPE_NOTSELECTED	//미선택
}ENDDS_DEPARTROADTYPE;

// 길안내 요청 종류 
typedef enum{
	REQ_NONE = 0,			
	REQ_ROUTE,				// 탐색 
	REQ_REROUTE,			// 재 탐색 
	REQ_REROUTE_DETAILLOC	// 상세위치 확인 
}ENDDS_ROUTE_TYPE;

// 차량 유류 종류 
typedef enum{
    OIL_NONE = 1,		// 미선택(DEFAULT)
	OIL_GASOLINE,		// 휘발유
	OIL_DIESEL,			// 경유
	OIL_LPG,			// LPG
	OIL_CHAdeMO,		// 전기(DC차데모)
	OIL_AC3,			// 전기(AC3)
    OIL_DCCombo,		// 전기(DC콤보)
    OIL_PremiumGasoline	// 고급휘발유
}ENDDS_CAROILTYPE;

// 요금 가중치 옵션 
typedef enum{
	BOTHCHARGEDANDFREE = 1, //유/무료 경로
	OPTIMIZEDCHARGED,	//최적요금 경로
	MINUMCHARGED,		//최소요금 경로
	FREEFIRST,			//무료우선 경로
	LOGICAPPLIED		//로직에 의한 판단경로(DEFAULT)
}ENDDS_FAREWEIGHTOPTS;

// 경로 서비스 구분
typedef enum{
	REALTIME = 1,					// 실시간
	DEPARTURETIMEESTIMATION,	// 출발시갂 예측 길안내
	ARRIVALTIMEESTIMATION		//도착시갂 예측 길안내
}ENDDS_SERVICE_FLAG;

// 이미지 교통정보 해상도 구분
typedef enum
{
// 	P_0109_0065,
// 	P_0120_0100,
// 	P_0128_0116,
// 	P_0176_0150,
// 	P_0240_0220,
// 	P_0288_0147,
// 	P_0299_0177,
// 	P_0320_0240,
	P_0499_0364,
// 	P_0499_0365,
// 	P_0701_0328,
// 	P_0702_0328,
	P_0800_0800_8,
	P_0800_0800_24
//	P_9999_9999
}ENDDS_IMAGERESOLUTIONTYPE;

#endif // __NDDS_DEF_H__
