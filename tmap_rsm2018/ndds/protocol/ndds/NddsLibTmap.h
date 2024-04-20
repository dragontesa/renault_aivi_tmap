// CNddsLibTmap.h
#ifndef __NDDSLIB_TMAP_H__
#define __NDDSLIB_TMAP_H__

#include "NddsLibBase.h"

class CNddsLibTmap : public CNddsLibBase
{
public:
	CNddsLibTmap();
	virtual ~CNddsLibTmap();

	/// 통신 객체 초기화 한다.
	void Init(NDDS_PROTOCOL_SESSION_DATA *pSessionData);

	/// NDDS 요청 구조체를 요청 버퍼(Json)으로 변경한다.
	/**
		@param pReqStruct		[IN] 요청 구조체 포인터
		@param pSessionData		[IN] 요청 정보
								[OUT] 변경된 요청 버퍼 정보
		@return					성공여부
	**/
	bool MakeRequestData(NDDS_COMMON_REQ *pReqStruct, NDDS_PROTOCOL_SESSION_DATA *pSessionData);

	/// NDDS 서버 요청
	bool RequestServer();
	
	//////////////////////////////////////////////////////////////////////////
	// 프로토콜 데이터 생성 및 파싱
	//////////////////////////////////////////////////////////////////////////
private:
	bool AddCommonHeader(NDDSLIB_Json::Value &root, NDDS_COMMONHEADER_REQ *pCommonHeader, bool bSeedKey);		// 공통 해더 생성

	bool MakePlanningRoute(NDDSLIB_Json::Value &root, NDDSTMAP_PLANNINGROUTE_REQ *pReq);						// 5.1.3. 길안내
	bool MakeRouteSummaryInfo(NDDSLIB_Json::Value &root, NDDSTMAP_ROUTESUMMARYINFO_REQ *pReq);						// 5.1.4. 경로요약

	bool MakeFindPois(NDDSLIB_Json::Value &root, NDDSTMAP_FINDPOIS_REQ *pReq);									// 5.2.1 통합검색
	bool MakeFindAroundPois(NDDSLIB_Json::Value &root, NDDSTMAP_FINDAROUNDPOIS_REQ *pReq);						// 5.2.2  주변지 검색
	bool MakeFindPoiDetailInfos(NDDSLIB_Json::Value &root, NDDSTMAP_FINDPOIDETAILINFO_REQ *pReq);				// 5.2.3  POI 상세정보
	bool MakeFindPoiJoinInfo(NDDSLIB_Json::Value &root, NDDSTMAP_FINDPOIJOININFO_REQ *pReq);					// 5.2.4. 가맹점 상세정보
	bool MakeFindRecentDestnation(NDDSLIB_Json::Value &root, NDDSTMAP_FINDRECENTDESTNATION_REQ *pReq);			// 5.2.5. POI 최근길 조회
	bool MakeUploadRecentDestnation(NDDSLIB_Json::Value &root, NDDSTMAP_UPLOADRECENTDESTNATION_REQ *pReq);		// 5.2.6. POI 최근길 전체 업로드
    bool MakeRemoveRecentDestnation(NDDSLIB_Json::Value &root, NDDSTMAP_REMOVERECENTDESTNATION_REQ *pReq);		// 5.2.7. POI 최근길 멀티 삭제
    bool MakeFindNearBy(NDDSLIB_Json::Value &root, NDDSTMAP_FINDNEARBY_REQ  *pReq);								// 5.2.7. 좌표로 지명 검색
	bool MakeFindPickatPopulars(NDDSLIB_Json::Value &root, NDDSTMAP_FINDPICKATPOPULARS_REQ  *pReq);				// 5.2.8. Pickat 인기검색어 조회
	bool MakeFindRoadName(NDDSLIB_Json::Value &root, NDDSTMAP_FINDROADNAME_REQ  *pReq);							// 5.2.9. 새주소(도로명) 검색
	bool MakeFindPickatPoiDetail(NDDSLIB_Json::Value &root, NDDSTMAP_FINDPICKATPOIDETAIL_REQ  *pReq);			// 5.2.10. Pickat POI 상세정보 조회
	bool MakeFindAreaNamesByStep(NDDSLIB_Json::Value &root, NDDSTMAP_FINDAREANAMESBYSTEP_REQ  *pReq);			// 5.2.11. 지역분류별 주소명 조회
	bool MakeFindPoiStationInfo(NDDSLIB_Json::Value &root, NDDSTMAP_FINDPOISTATIONINFO_REQ  *pReq);				// 5.2.12. 최저가 주유소(충전소)조회
    bool MakeFindPoiEvStationInfo(NDDSLIB_Json::Value &root, NDDSTMAP_FINDPOIEVSTATIONINFO_REQ  *pReq);			// 5.2.16. ev층전소 상세정보 조회
    bool MakeFindPoiEvStationsByPoint(NDDSLIB_Json::Value &root, NDDSTMAP_FINDPOIEVSTATIONSBYPOINT_REQ  *pReq);	// 5.2.18. 주변 전기차 충전소 조회

	bool MakeFindPoiFavorite(NDDSLIB_Json::Value &root, NDDSTMAP_FINDPOIFAVORITE_REQ *pReq);					// 5.3.1. POI 즐겨찾기 조회
	bool MakeRegistPoiFavoriteMulti(NDDSLIB_Json::Value &root, NDDSTMAP_REGISTPOIFAVORITEMULTI_REQ *pReq);		// 5.3.2. POI 즐겨찾기 등록
	bool MakeRegistAllPoiFavorite(NDDSLIB_Json::Value &root, NDDSTMAP_REGISTALLPOIFAVORITE_REQ *pReq);			// 5.3.3. POI 즐겨찾기 전체 업로드
    bool MakeRemovePoiFavorite(NDDSLIB_Json::Value &root, NDDSTMAP_REMOVEPOIFAVORITE_REQ *pReq);				// 5.3.4  POI 즐겨찾기 멀티 삭제
    bool MakeModifyPoiFavorite(NDDSLIB_Json::Value &root, NDDSTMAP_MODIFYPOIFAVORITE_REQ *pReq);				// 5.3.5  POI 즐겨찾기 단건 수정

    bool MakeFindRoute(NDDSLIB_Json::Value &root, NDDSTMAP_FINDROUTE_REQ *pReq);								// 5.3.4  경로 즐겨찾기 조회
	bool MakeRegistRouteMulti(NDDSLIB_Json::Value &root, NDDSTMAP_REGISTROUTEMULTI_REQ  *pReq);					// 5.3.5. 경로 즐겨찾기 등록
	bool MakeUploadTotalRoute(NDDSLIB_Json::Value &root, NDDSTMAP_UPLOADTOTALROUTE_REQ  *pReq);					// 5.3.6. 경로 즐겨찾기 전체 업로드
	bool MakeFindTrafficFavorites(NDDSLIB_Json::Value &root, NDDSTMAP_FINDTRAFFICFAVORITES_REQ   *pReq);		// 5.3.7. 교통정보 즐겨찾기 조회
	bool MakeRegistTrafficFavorites(NDDSLIB_Json::Value &root, NDDSTMAP_REGISTTRAFFICFAVORITES_REQ   *pReq);	// 5.3.8. 교통정보 즐겨찾기 등록
	bool MakeUploadTrafficFavorites(NDDSLIB_Json::Value &root, NDDSTMAP_UPLOADTRAFFICFAVORITES_REQ   *pReq);	// 5.3.9. 교통정보 즐겨찾기 전체 업로드
	bool MakeRemoveTrafficFavorites(NDDSLIB_Json::Value &root, NDDSTMAP_REMOVETRAFFICFAVORITE_REQ   *pReq);		// 5.3.10.교통정보 즐겨찾기 단일건 삭제
    bool MakeFindUserData(NDDSLIB_Json::Value &root, NDDSTMAP_FINDUSERDATA_REQ   *pReq);						// 5.3.14.최근길/즐겨찾기 전체 리스트 조회
    bool MakeRegistUserProfile(NDDSLIB_Json::Value &root, NDDSTMAP_REGISTUSERPROFILE_REQ *pReq);				// 5.3.16. User-Profile 저장
    bool MakeRegistCarProfile(NDDSLIB_Json::Value &root, NDDSTMAP_REGISTCARPROFILE_REQ *pReq);					// 5.3.18. Car-Profile 저장

	bool MakeFindNodeJamRank(NDDSLIB_Json::Value &root, NDDSTMAP_FINDNODEJAMRANK_REQ   *pReq);					// 5.4.1. 상습 정체구간 조회
	bool MakeSmimageTrafficCongestion(NDDSLIB_Json::Value &root, NDDSTMAP_SMIMAGETRAFFICCONGESTION_REQ   *pReq);// 5.4.2. 이미지 교통정보
	bool MakeRadiusTrafficInfomtaion(NDDSLIB_Json::Value &root, NDDSTMAP_RADIUSTRAFFICINFOMATION_REQ   *pReq);	// 5.4.3. 주변 교통정보
	bool MakeFindTsdSudn(NDDSLIB_Json::Value &root,NDDSTMAP_FINDTSDSUDN_REQ *pReq);								// 5.4.4. 돌발 정보 조회
	bool MakeVmsGuideInfo( NDDSLIB_Json::Value &root, NDDSTMAP_VMSGUIDEINFO_REQ *pReq );						// 5.4.5. VMS 가이드 정보 조회

	bool MakeFindBusStation(NDDSLIB_Json::Value &root,NDDSTMAP_FINDBUSSTATION_REQ *pReq);						// 5.5.1. 버스 정류장 정보(도착 시간)
	bool MakeFindBusArrival(NDDSLIB_Json::Value &root,NDDSTMAP_FINDBUSARRIVAL_REQ *pReq);						// 5.5.2. 버스 노선 정보(도착 정보)
	bool MakeFindBusArrivalTime(NDDSLIB_Json::Value &root,NDDSTMAP_FINDBUSARRIVALTIME_REQ *pReq);				// 5.5.3. 버스 노선 정보(도착 시간)
	bool MakeFindBusStationRealTime(NDDSLIB_Json::Value &root,NDDSTMAP_FINDBUSSTATIONREALTIME_REQ *pReq);		// 5.5.4. 버스 정류장/노선 실시간 정보
	bool MakeFindStationInfo(NDDSLIB_Json::Value &root,NDDSTMAP_FINDSTATIONINFO_REQ *pReq);						// 5.5.5. 역 정보
	bool MakeFindStationTimeTable(NDDSLIB_Json::Value &root,NDDSTMAP_FINDSTATIONTIMETABLE_REQ *pReq);			// 5.5.6. 역 운행 정보
	bool MakeFindSubwayImage(NDDSLIB_Json::Value &root,NDDSTMAP_FINDSUBWAYIMAGE_REQ *pReq);						// 5.5.7. 노선 이미지
	bool MakeFindPathFind1stAll4(NDDSLIB_Json::Value &root,NDDSTMAP_FINDPATHFIND1STALL4_REQ *pReq);				// 5.5.8. 경로안내(도시간 검색 포함)
	bool MakeFindPathFind1stAll4Plural(NDDSLIB_Json::Value &root,NDDSTMAP_FINDPATHFIND1STALL4PLURAL_REQ *pReq);	// 5.5.9. 경로안내(도시내 복수 검색)
	bool MakeFindNearStation(NDDSLIB_Json::Value &root,NDDSTMAP_FINDNEARSTATION_REQ *pReq);						// 5.5.10.주변 정류장 / 역 조회(거리순)
	bool MakeFindAllTransport(NDDSLIB_Json::Value &root,NDDSTMAP_FINDALLTRANSPORT_REQ *pReq);					// 5.5.11.통합 검색
	bool MakeFindCCTVResult(NDDSLIB_Json::Value &root,NDDSTMAP_FINDCCTVRESULT_REQ *pReq);						// 5.5.12.주변 CCTV 리스트
	bool MakeFindCCTVResultById(NDDSLIB_Json::Value &root,NDDSTMAP_FINDCCTVRESULTBYID_REQ *pReq);				// 5.5.13.주변 CCTV 정보

	bool MakePositionSharing(NDDSLIB_Json::Value &root, NDDSTMAP_POSITIONSHARING_REQ *pReq);					// 5.6.9. 위치 공유
	bool MakeRouteSharingCar(NDDSLIB_Json::Value &root, NDDSTMAP_ROUTESHARINGCAR_REQ *pReq);					// 5.6.10. 경로공유 - 자동차
	bool MakeRouteSharingPublic(NDDSLIB_Json::Value &root, NDDSTMAP_ROUTESHARINGPUBLIC_REQ *pReq);				// 5.6.11. 경로공유 - 대중교통
	bool MakeRouteSharingWalk(NDDSLIB_Json::Value &root, NDDSTMAP_ROUTESHARINGWALK_REQ *pReq);					// 5.6.12. 경로공유 - 도보
	bool MakeRouteSharingDriving(NDDSLIB_Json::Value &root, NDDSTMAP_ROUTESHARINGDRIVING_REQ *pReq);			// 5.6.13. 경로공유 - 주행경로

	bool MakeRegisterSendToCarInfo(NDDSLIB_Json::Value &root, NDDSTMAP_REGISTERSENDTOCARINFO_REQ *pReq);		// 5.6.19. 차량 전송 정보 저장
	bool MakeFindSendToCarInfo(NDDSLIB_Json::Value &root, NDDSTMAP_FINDSENDTOCARINFO_REQ *pReq);				// 5.6.20. 차량 전송 정보 찾기
	bool MakeRemoveSendToCarInfo(NDDSLIB_Json::Value &root, NDDSTMAP_REMOVESENDTOCARINFO_REQ *pReq);			// 5.6.21. 차량 전송 정보 삭제

	bool MakeFindChargeNotices(NDDSLIB_Json::Value &root, NDDSTMAP_FINDCHARGENOTICES_REQ *pReq);				// 5.7.1  공지사항(요금제)
	bool MakeFindFileAccess(NDDSLIB_Json::Value &root, NDDSTMAP_FINDFILEACCESS_REQ *pReq);						// 5.7.2  파일(이미지,기타) 정보
	bool MakeFindOpenApps(NDDSLIB_Json::Value &root, NDDSTMAP_FINDOPENAPPS_REQ *pReq);							// 5.7.4  오픈 앱 리스트
	bool MakeFindSeedKeyInfo(NDDSLIB_Json::Value &root, NDDSTMAP_FINDSEEDKEYINFO_REQ *pReq);					// 5.7.9  Seed Key 정보 요청
	bool MakeAppLogSave(NDDSLIB_Json::Value &root, NDDSTMAP_APPLOGSAVE_REQ *pReq);								// 5.7.10 App Log 저장
	bool MakeRouteHistorySave(NDDSLIB_Json::Value &root, NDDSTMAP_ROUTEHISTORYSAVE_REQ *pReq);					// 5.7.11 주행 이력 저장

	bool MakeSaveAdvertise(NDDSLIB_Json::Value &root,NDDSTMAP_SAVEADVERTISE_REQ *pReq);							// 5.8.2. 광고 음원 노출 보고
	
	bool MakeFindAgreement(NDDSLIB_Json::Value &root,NDDSTMAP_FINDAGREEMENT_REQ *pReq);							// 5.9.1. 이용약관 동의 여부 조회
	bool MakeRegistAgreement(NDDSLIB_Json::Value &root,NDDSTMAP_REGISTAGREEMENT_REQ *pReq);						// 5.9.2. 이용약관 동의 결과 저장

	int MakeServiceCloseRequestData(unsigned char **pReqData,char *pServiceID,char *pServiceURI);			// 서비스 종료 생성

	bool MakeAuthForSmartCar( NDDSLIB_Json::Value &root, NDDSTMAP_AUTHFORSMARTCAR_REQ *pReq );			// 5.6.3 단말인증 (for Smart Car)

	bool ParserPlanningRoute(NDDS_COMMONHEADER_RES common_header,unsigned char *content,int nContentSize,NDDS_MEMORYSTATUS &memory, NDDSTMAP_PLANNINGROUTE_RES **ppRes);		// 5.1.3. 길안내
	bool ParserRouteSummaryInfo(NDDSLIB_Json::Value &root, NDDS_MEMORYSTATUS &memory, NDDSTMAP_ROUTESUMMARYINFO_RES **ppRes);								// 5.1.4  경로요약

	bool ParserFindPois(NDDSLIB_Json::Value &root, NDDS_MEMORYSTATUS &memory, NDDSTMAP_FINDPOIS_RES **ppRes);								// 5.2.1  통합검색
	bool ParserFindAroundPois(NDDSLIB_Json::Value &root,NDDS_MEMORYSTATUS &memory, NDDSTMAP_FINDAROUNDPOIS_RES **ppRes);					// 5.2.2  주변지 검색
	bool ParserFindPoiDetailInfos(NDDSLIB_Json::Value &root,NDDS_MEMORYSTATUS &memory, NDDSTMAP_FINDPOIDETAILINFO_RES **ppRes);				// 5.2.3  POI 상세정보
	bool ParserFindPoiJoinInfo(NDDSLIB_Json::Value &root,NDDS_MEMORYSTATUS &memory, NDDSTMAP_FINDPOIJOININFO_RES **ppRes);					// 5.2.4. 가맹점 상세정보
	bool ParserFindRecentDestnation(NDDSLIB_Json::Value &root,NDDS_MEMORYSTATUS &memory, NDDSTMAP_FINDRECENTDESTNATION_RES **ppRes);		// 5.2.5. POI 최근길 조회
	bool ParserUploadRecentDestnation(NDDSLIB_Json::Value &root,NDDS_MEMORYSTATUS &memory, NDDSTMAP_UPLOADRECENTDESTNATION_RES  **ppRes);	// 5.2.6. POI 최근길 전체 업로드
    bool ParserRemoveRecentDestnation(NDDSLIB_Json::Value &root,NDDS_MEMORYSTATUS &memory, NDDSTMAP_REMOVERECENTDESTNATION_RES  **ppRes);	// 5.2.7. POI 최근길 멀티 삭제
    bool ParserFindNearBy(NDDSLIB_Json::Value &root,NDDS_MEMORYSTATUS &memory, NDDSTMAP_FINDNEARBY_RES  **ppRes);							// 5.2.7. 좌표로 지명 검색
	bool ParserFindPickatPopulars(NDDSLIB_Json::Value &root,NDDS_MEMORYSTATUS &memory, NDDSTMAP_FINDPICKATPOPULARS_RES  **ppRes);			// 5.2.8. Pickat 인기검색어 조회
	bool ParserFindRoadName(NDDSLIB_Json::Value &root,NDDS_MEMORYSTATUS &memory, NDDSTMAP_FINDROADNAME_RES  **ppRes);						// 5.2.9. 새주소(도로명) 검색
	bool ParserFindPickatPoiDetail(NDDSLIB_Json::Value &root,NDDS_MEMORYSTATUS &memory, NDDSTMAP_FINDPICKATPOIDETAIL_RES  **ppRes);			// 5.2.10. Pickat POI 상세정보 조회
	bool ParserFindAreaNamesByStep(NDDSLIB_Json::Value &root,NDDS_MEMORYSTATUS &memory, NDDSTMAP_FINDAREANAMESBYSTEP_RES  **ppRes);			// 5.2.11. 지역분류별 주소명 조회
	bool ParserFindPoiStationInfo(NDDSLIB_Json::Value &root,NDDS_MEMORYSTATUS &memory, NDDSTMAP_FINDPOISTATIONINFO_RES  **ppRes);			// 5.2.12. 최저가 주유소(충전소)조회
    bool ParserFindPoiEvStationInfo(NDDSLIB_Json::Value &root,NDDS_MEMORYSTATUS &memory, NDDSTMAP_FINDPOIEVSTATIONINFO_RES  **ppRes);		// 5.2.16. ev충전소 상세정보 조회
    bool ParserFindPoiEvStationsByPoint(NDDSLIB_Json::Value &root,NDDS_MEMORYSTATUS &memory, NDDSTMAP_FINDPOIEVSTATIONSBYPOINT_RES  **ppRes);// 5.2.16. 주변 전기차 충전소 조회

	bool ParserPathFind1Stall4ViewsDto(NDDSLIB_Json::Value &root,NDDS_MEMORYSTATUS &memory, NDDSTMAP_PATHFIND1STALL4VIEWS_DTO**ppDto );
	bool ParserFindPoiFavorite(NDDSLIB_Json::Value &root, NDDS_MEMORYSTATUS &memory, NDDSTMAP_FINDPOIFAVORITE_RES **ppRes);					// 5.3.1. POI 즐겨찾기 조회
	bool ParserRegistPoiFavoriteMulti(NDDSLIB_Json::Value &root, NDDS_MEMORYSTATUS &memory, NDDSTMAP_REGISTPOIFAVORITEMULTI_RES **ppRes);	// 5.3.2. POI 즐겨찾기 등록
	bool ParserRegistAllPoiFavorite(NDDSLIB_Json::Value &root, NDDS_MEMORYSTATUS &memory, NDDSTMAP_REGISTALLPOIFAVORITE_RES **ppRes);		// 5.3.3. POI 즐겨찾기 전체 업로드
    bool ParserRemovePoiFavorites(NDDSLIB_Json::Value &root, NDDS_MEMORYSTATUS &memory, NDDSTMAP_REMOVEPOIFAVORITE_RES **ppRes);			// 5.3.4. POI 즐겨찾기 멀티 삭제
    bool ParserModifyPoiFavorite(NDDSLIB_Json::Value &root, NDDS_MEMORYSTATUS &memory, NDDSTMAP_MODIFYPOIFAVORITE_RES **ppRes);				// 5.3.5. POI 즐겨찾기 단건 수정

    bool ParserFindRoute(NDDSLIB_Json::Value &root, NDDS_MEMORYSTATUS &memory, NDDSTMAP_FINDROUTE_RES **ppRes);								// 5.3.4  경로 즐겨찾기 조회
	bool ParserRegistRouteMulti(NDDSLIB_Json::Value &root,NDDS_MEMORYSTATUS &memory, NDDSTMAP_REGISTROUTEMULTI_RES  **ppRes);				// 5.3.5. 경로 즐겨찾기 등록
	bool ParserUploadTotalRoute(NDDSLIB_Json::Value &root,NDDS_MEMORYSTATUS &memory, NDDSTMAP_UPLOADTOTALROUTE_RES  **ppRes);				// 5.3.6. 경로 즐겨찾기 전체 업로드
	bool ParserFindTrafficFavorites(NDDSLIB_Json::Value &root,NDDS_MEMORYSTATUS &memory, NDDSTMAP_FINDTRAFFICFAVORITES_RES  **ppRes);		// 5.3.7. 교통정보 즐겨찾기 조회
	bool ParserRegistTrafficFavorites(NDDSLIB_Json::Value &root,NDDS_MEMORYSTATUS &memory, NDDSTMAP_REGISTTRAFFICFAVORITES_RES  **ppRes);	// 5.3.8. 교통정보 즐겨찾기 등록
	bool ParserUploadTrafficFavorites(NDDSLIB_Json::Value &root,NDDS_MEMORYSTATUS &memory, NDDSTMAP_UPLOADTRAFFICFAVORITES_RES  **ppRes);	// 5.3.9. 교통정보 즐겨찾기 전체 업로드
	bool ParserRemoveTrafficFavorites(NDDSLIB_Json::Value &root,NDDS_MEMORYSTATUS &memory, NDDSTMAP_REMOVETRAFFICFAVORITE_RES  **ppRes);	// 5.3.10.교통정보 즐겨찾기 단일건 삭제
    bool ParserFindUserData(NDDSLIB_Json::Value &root,NDDS_MEMORYSTATUS &memory, NDDSTMAP_FINDUSERDATA_RES  **ppRes);						// 5.3.14.최근길/즐겨찾기 전체 리스트 조회
    bool ParserFindUserProfile(NDDSLIB_Json::Value &root, NDDS_MEMORYSTATUS &memory, NDDSTMAP_FINDUSERPROFILE_RES **ppRes);					// 5.3.15.User-Profile 조회
    bool ParserRegistUserProfile(NDDSLIB_Json::Value &root, NDDS_MEMORYSTATUS &memory, NDDSTMAP_REGISTUSERPROFILE_RES **ppRes);				// 5.3.16.User-Profile 저장
    bool ParserFindCarProfile(NDDSLIB_Json::Value &root, NDDS_MEMORYSTATUS &memory, NDDSTMAP_FINDCARPROFILE_RES **ppRes);					// 5.3.17.Car-Profile 조회
    bool ParserRegistCarProfile(NDDSLIB_Json::Value &root, NDDS_MEMORYSTATUS &memory, NDDSTMAP_REGISTCARPROFILE_RES **ppRes);				// 5.3.18.Car-Profile 저장

	bool ParserFindNodeJamRank(NDDSLIB_Json::Value &root,NDDS_MEMORYSTATUS &memory, NDDSTMAP_FINDNODEJAMRANK_RES  **ppRes);					// 5.4.1. 상습 정체구간 조회
	bool ParserSmimageTrafficCongestio(NDDSLIB_Json::Value &root,NDDS_MEMORYSTATUS &memory, NDDSTMAP_SMIMAGETRAFFICCONGESTION_RES  **ppRes);// 5.4.2. 이미지 교통정보
	bool ParserRadiusTrafficInfomtaion(NDDS_COMMONHEADER_RES common_header,unsigned char *content,int nContentSize,NDDS_MEMORYSTATUS &memory, NDDSTMAP_RADIUSTRAFFICINFOMATION_RES **ppRes);	// 5.4.3. 주변 교통정보
	bool ParserFindTsdSudn(NDDSLIB_Json::Value &root,NDDS_MEMORYSTATUS &memory, NDDSTMAP_FINDTSDSUDN_RES  **ppRes);							// 5.4.4. 돌발 정보 조회
	bool ParserVmsGuideInfo( NDDSLIB_Json::Value &root,NDDS_MEMORYSTATUS &memory,NDDSTMAP_VMSGUIDEINFO_RES **ppRes );					// 5.4.5. VMS 가이드 정보 조회

	bool ParserFindBusStation(NDDSLIB_Json::Value &root,NDDS_MEMORYSTATUS &memory, NDDSTMAP_FINDBUSSTATION_RES  **ppRes);					// 5.5.1. 버스 정류장 정보(도착 시간)
	bool ParserFindBusArrival(NDDSLIB_Json::Value &root,NDDS_MEMORYSTATUS &memory, NDDSTMAP_FINDBUSARRIVAL_RES **ppRes);					// 5.5.2. 버스 노선 정보(도착 정보)
	bool ParserFindBusArrivalTime(NDDSLIB_Json::Value &root,NDDS_MEMORYSTATUS &memory,NDDSTMAP_FINDBUSARRIVALTIME_RES **ppRes);				// 5.5.3. 버스 노선 정보(도착 시간)
	bool ParserFindBusStationRealTime(NDDSLIB_Json::Value &root,NDDS_MEMORYSTATUS &memory,NDDSTMAP_FINDBUSSTATIONREALTIME_RES **ppRes);		// 5.5.4. 버스 정류장/노선 실시간 정보
	bool ParserFindStationInfo(NDDSLIB_Json::Value &root,NDDS_MEMORYSTATUS &memory,NDDSTMAP_FINDSTATIONINFO_RES **ppRes);					// 5.5.5. 역 정보
	bool ParserFindStationTimeTable(NDDSLIB_Json::Value &root,NDDS_MEMORYSTATUS &memory,NDDSTMAP_FINDSTATIONTIMETABLE_RES **ppRes);			// 5.5.6. 역 운행 정보
	bool ParserFindSubwayImage(NDDSLIB_Json::Value &root,NDDS_MEMORYSTATUS &memory,NDDSTMAP_FINDSUBWAYIMAGE_RES **ppRes);					// 5.5.7. 노선 이미지
	bool ParserFindPathFind1stAll4(NDDSLIB_Json::Value &root,NDDS_MEMORYSTATUS &memory,NDDSTMAP_FINDPATHFIND1STALL4_RES **ppRes);			// 5.5.8. 경로안내(도시간 검색 포함)
	bool ParserFindPathFind1stAll4Plural(NDDSLIB_Json::Value &root,NDDS_MEMORYSTATUS &memory,NDDSTMAP_FINDPATHFIND1STALL4PLURAL_RES **ppRes);// 5.5.9. 경로안내(도시내 복수 검색)
	bool ParserFindNearStation(NDDSLIB_Json::Value &root,NDDS_MEMORYSTATUS &memory,NDDSTMAP_FINDNEARSTATION_RES **ppRes);							// 5.5.10.주변 정류장 / 역 조회(거리순)
	bool ParserFindAllTransport(NDDSLIB_Json::Value &root,NDDS_MEMORYSTATUS &memory,NDDSTMAP_FINDALLTRANSPORT_RES **ppRes);				// 5.5.11.통합 검색
	bool ParserFindCCTVResult(NDDSLIB_Json::Value &root,NDDS_MEMORYSTATUS &memory,NDDSTMAP_FINDCCTVRESULT_RES **ppRes);					// 5.5.12.주변 CCTV 리스트
	bool ParserFindCCTVResultById(NDDSLIB_Json::Value &root,NDDS_MEMORYSTATUS &memory,NDDSTMAP_FINDCCTVRESULTBYID_RES **ppRes);			// 5.5.13.주변 CCTV 정보

	bool ParserPositionSharing(NDDSLIB_Json::Value &root,NDDS_MEMORYSTATUS &memory, NDDSTMAP_POSITIONSHARING_RES **ppRes);				// 5.6.9. 위치 공유
	bool ParserRouteSharingCar(NDDSLIB_Json::Value &root,NDDS_MEMORYSTATUS &memory, NDDSTMAP_ROUTESHARINGCAR_RES **ppRes);				// 5.6.10. 경로공유 - 자동차
	bool ParserRouteSharingPublic(NDDSLIB_Json::Value &root,NDDS_MEMORYSTATUS &memory, NDDSTMAP_ROUTESHARINGPUBLIC_RES **ppRes);		// 5.6.11. 경로공유 - 대중교통
	bool ParserRouteSharingWalk(NDDSLIB_Json::Value &root,NDDS_MEMORYSTATUS &memory, NDDSTMAP_ROUTESHARINGWALK_RES **ppRes);			// 5.6.12. 경로공유 - 도보
	bool ParserRouteSharingDriving(NDDSLIB_Json::Value &root,NDDS_MEMORYSTATUS &memory, NDDSTMAP_ROUTESHARINGDRIVING_RES **ppRes);		// 5.6.13. 경로공유 - 주행경로

	bool ParserRegisterSendToCarInfo(NDDSLIB_Json::Value &root,NDDS_MEMORYSTATUS &memory, NDDSTMAP_REGISTERSENDTOCARINFO_RES **ppRes);	// 5.6.19. 차량 전송 정보 저장
	bool ParserFindSendToCarInfo(NDDSLIB_Json::Value &root,NDDS_MEMORYSTATUS &memory, NDDSTMAP_FINDSENDTOCARINFO_RES **ppRes);			// 5.6.20. 차량 전송 정보 조회
	bool ParserRemoveSendToCarInfo(NDDSLIB_Json::Value &root,NDDS_MEMORYSTATUS &memory, NDDSTMAP_REMOVESENDTOCARINFO_RES **ppRes);		// 5.6.21. 차량 전송 정보 삭제

	bool ParserFindChargeNotices(NDDSLIB_Json::Value &root, NDDS_MEMORYSTATUS &memory, NDDSTMAP_FINDCHARGENOTICES_RES **ppRes);			// 5.7.1  공지사항(요금제)
	bool ParserFindFileAccess(NDDSLIB_Json::Value &root, NDDS_MEMORYSTATUS &memory, NDDSTMAP_FINDFILEACCESS_RES **ppRes);				// 5.7.2  파일(이미지,기타) 정보
	bool ParserFindOpenApps(NDDSLIB_Json::Value &root, NDDS_MEMORYSTATUS &memory, NDDSTMAP_FINDOPENAPPS_RES **ppRes);					// 5.7.4  오픈 앱 리스트
	bool ParserFindSeedKeyInfo(NDDSLIB_Json::Value &root, NDDS_MEMORYSTATUS &memory, NDDSTMAP_FINDSEEDKEYINFO_RES **ppRes, NDDS_COMMONHEADER_REQ *pCommonHeader);				// 5.7.9  Seed Key 정보 요청
	bool ParserAppLogSave(NDDSLIB_Json::Value &root, NDDS_MEMORYSTATUS &memory, NDDSTMAP_APPLOGSAVE_RES **ppRes);						// 5.7.10 App Log 저장
	bool ParserRouteHistorySave(NDDSLIB_Json::Value &root, NDDS_MEMORYSTATUS &memory, NDDSTMAP_ROUTEHISTORYSAVE_RES **ppRes);			// 5.7.11 주행 이력 저장


	bool ParserSaveAdvertise(NDDSLIB_Json::Value &root,NDDS_MEMORYSTATUS &memory, NDDSTMAP_SAVEADVERTISE_RES **ppRes);					// 5.8.2. 광고 음원 노출 보고

	bool ParserFindAgreement(NDDSLIB_Json::Value &root,NDDS_MEMORYSTATUS &memory, NDDSTMAP_FINDAGREEMENT_RES **ppRes);					// 5.9.1. 이용약관 동의 여부 조회
	bool ParserRegistAgreement(NDDSLIB_Json::Value &root,NDDS_MEMORYSTATUS &memory, NDDSTMAP_REGISTAGREEMENT_RES **ppRes);				// 5.9.2. 이용약관 동의 결과 저장
	
	bool ParserAuthForSmartCar( NDDSLIB_Json::Value &root,NDDS_MEMORYSTATUS &memory,NDDSTMAP_AUTHFORSMARTCAR_RES **ppRes );				// 5.6.3 단말인증 (for Smart Car)

    bool ParserNonNddsResult(char *resData, int contentLength, NDDS_MEMORYSTATUS &memory, NDDSTMAP_NONNDDS_RES **ppRes);

	bool ParserJson(eNddsProtocolType eProtocolType,char *szJson,int jsonLength, NDDS_COMMON_RES **ppResStruct, NDDS_MEMORYSTATUS &memory);
	bool ParserBinary(eNddsProtocolType eProtocolType,unsigned char *binaryBuf,int bufSize,NDDS_COMMON_RES **ppResStruct,NDDS_MEMORYSTATUS &memory);
    bool ParserNonNdds(eNddsProtocolType eProtocolType, char *resData, int contentLength, NDDS_COMMON_RES **ppResStruct, NDDS_MEMORYSTATUS &memory);

	bool CheckServiceClose(NDDS_PROTOCOL_SESSION_DATA *pSessionData);

	NDDSTMAP_POISEARCHES_DTO* ParserPois(NDDSLIB_Json::Value element,NDDS_MEMORYSTATUS &memory,int listCnt);


private:
	// 소켓에 데이터를 보내고 / 받는다.
	bool SendRequest(unsigned char *pSendBuff, int nSendBuff, unsigned char *pReceiveBuff, int& nReceiveBuff,NDDS_HTTP_RESULTDATA &httpResult,bool isServiceClose);
};

#endif // __NDDSLIB_TMAP_H__
