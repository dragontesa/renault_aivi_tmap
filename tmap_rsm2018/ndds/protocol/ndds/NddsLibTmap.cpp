// Ndds.cpp

#include "NddsLibTmap.h"
#include "NddsLibConst.h"
#include "NddsLibCommonData.h"
#include "ndds_unicode.h"
#include "ndds_base64.h"
#include <time.h>

CNddsLibTmap::CNddsLibTmap()
: CNddsLibBase()
{

}

CNddsLibTmap::~CNddsLibTmap()
{

}

void CNddsLibTmap::Init(NDDS_PROTOCOL_SESSION_DATA *pSessionData)
{
    CNddsLibBase::Init(pSessionData);

}

bool CNddsLibTmap::MakeRequestData(NDDS_COMMON_REQ *pReqStruct, NDDS_PROTOCOL_SESSION_DATA *pSessionData)
{
    bool bRet;
    int  nHttpHeader;
    time_t  ltime;
    struct tm *today;
    NDDS_HTTP_HEADERDATA *pHttpHeader = &pSessionData->sHttpHeader;
    NDDS_COMMONHEADER_REQ *pCommonHeader = &pSessionData->sCommonHeader;
    CNddsLibCommonData &CommonData = CNddsLibCommonData::GetInstance();
    memcpy(pHttpHeader, &CommonData.m_sTmapHttpHeader, sizeof(NDDS_HTTP_HEADERDATA));
    memcpy(pCommonHeader, &CommonData.m_sTmapNddsCommonHeader, sizeof(NDDS_COMMONHEADER_REQ));
    time(&ltime);
#ifdef __MM2018__
    ltime += 9 * 60 * 60;       // Bosch System Firmware에서 localtime을 UTC로 링크해놨기 때문에, 우리가 알아서 서울 타임존을 적용해야 함. 2019.12.1 by hheo
#endif
    today = localtime(&ltime);
    sprintf(pCommonHeader->reqTime,"%04d%02d%02d%02d%02d%02d",
        today->tm_year + 1900,  // tm_year는 1900을 더해야 서기 연도가 됨
        today->tm_mon + 1,       // tm_mon은 1월이 0, 2월이 1... 식으로 되어 있음
        today->tm_mday,
        today->tm_hour,
        today->tm_min,
        today->tm_sec);
    //////////////////////////////////////////////////////////////////////////
    // 요청 데이터 Body 생성

    int nContentLen;
    int nJson, nAllocSize;
    wchar_t *wszJson;
    char *szJson;
    NDDSLIB_Json::Reader reader;
    NDDSLIB_Json::Value root, header, element;
    NDDSLIB_Json::StyledWriter writer;

    // 요청 데이터 추가
    bRet = true;
    // 요청 데이터 초기화
    pHttpHeader->mineType = NDDS_JSON;
    strcpy(pHttpHeader->serviceID,NDDS_START_SERVICE_ID);
    switch(pSessionData->eProtocolType)
    {
    case NDDSTYPE_JSON_STRING:
        {
            if(!((NDDS_JSON_REQ *)pReqStruct)->bIsBinaryResponse)
                pHttpHeader->mineType = NDDS_JSON;
            else
                pHttpHeader->mineType = NDDS_BINARY;

            if(((NDDS_JSON_REQ *)pReqStruct)->bUsePrevServiceId)
                memcpy(pHttpHeader->serviceID, CommonData.m_prevResponseServiceID, NDDS_HTTP_SERVICEID_LEN);
            else
                strcpy(pHttpHeader->serviceID, NDDS_START_SERVICE_ID);


            strcpy(pHttpHeader->uri, ((NDDS_JSON_REQ *)pReqStruct)->pServiceName);

            bRet = reader.parse(((NDDS_JSON_REQ *)pReqStruct)->pReqData, root);
        }
        break;
    case NDDSTYPE_TMAP_PLANNINGROUTE:							// 5.1.3. 길안내
        {
            bRet = MakePlanningRoute(root, (NDDSTMAP_PLANNINGROUTE_REQ *)pReqStruct);
            // 재탐색, 상세 위치 확인 인 경우 이전 Service ID 사용
            if((*((NDDSTMAP_PLANNINGROUTE_REQ *)pReqStruct)).ReSearchType != REQ_ROUTE)
                memcpy(pHttpHeader->serviceID, CommonData.m_prevResponseServiceID, NDDS_HTTP_SERVICEID_LEN);
            pHttpHeader->mineType = NDDS_BINARY;
            strcpy(pHttpHeader->uri, "/tmap-channel/rsd/route/planningroutemultiformat");
        }
        break;
    case NDDSTYPE_TMAP_ROUTESUMMARYINFO:							// 5.1.4. 경로요약
        {
            bRet = MakeRouteSummaryInfo(root, (NDDSTMAP_ROUTESUMMARYINFO_REQ *)pReqStruct);
            strcpy(pHttpHeader->uri, "/tmap-channel/rsd/route/routesummaryinfo");
        }
        break;
    case NDDSTYPE_TMAP_FINDPOIS:								// 5.2.1  통합검색
        {
            bRet = MakeFindPois(root, (NDDSTMAP_FINDPOIS_REQ *)pReqStruct);
            strcpy(pHttpHeader->uri, "/tmap-channel/poi/search/findpois");
        }
        break;
    case NDDSTYPE_TMAP_FINDAROUNDPOIS:						// 5.2.2  주변지 검색
        {
            bRet = MakeFindAroundPois(root, (NDDSTMAP_FINDAROUNDPOIS_REQ *)pReqStruct);
            strcpy(pHttpHeader->uri, "/tmap-channel/poi/search/findaroundpois");
        }
        break;
    case NDDSTYPE_TMAP_FINDPOIDETAILINFO:						// 5.2.3  POI 상세정보
        {
            bRet = MakeFindPoiDetailInfos(root, (NDDSTMAP_FINDPOIDETAILINFO_REQ *)pReqStruct);
            strcpy(pHttpHeader->uri, "/tmap-channel/poi/detailinfo/findpoidetailinfo");
        }
        break;
    case NDDSTYPE_TMAP_FINDPOIJOININFO:						// 5.2.4. 가맹점 상세정보
        {
            bRet = MakeFindPoiJoinInfo(root, (NDDSTMAP_FINDPOIJOININFO_REQ *)pReqStruct);
            strcpy(pHttpHeader->uri, "/tmap-channel/poi/joininfo/findpoijoininfo");
        }
        break;
    case NDDSTYPE_TMAP_FINDRECENTDESTNATION:					// 5.2.5. POI 최근길 조회
        {
            bRet = MakeFindRecentDestnation(root, (NDDSTMAP_FINDRECENTDESTNATION_REQ *)pReqStruct);
            strcpy(pHttpHeader->uri, "/tmap-channel/poi/recent/findrecentdestnation");
        }
        break;
    case NDDSTYPE_TMAP_UPLOADRECENTDESTNATION:				// 5.2.6. POI 최근길 전체 업로드
        {
            bRet = MakeUploadRecentDestnation(root, (NDDSTMAP_UPLOADRECENTDESTNATION_REQ *)pReqStruct);
            strcpy(pHttpHeader->uri, "/tmap-channel/poi/recent/uploadrecentdestnation");
        }
        break;
    case NDDSTYPE_TMAP_REMOVERECENTDESTNATION:				// 5.2.7 POI 최근길 멀티 삭제
        {
            bRet = MakeRemoveRecentDestnation(root, (NDDSTMAP_REMOVERECENTDESTNATION_REQ *)pReqStruct);
            strcpy(pHttpHeader->uri, "/tmap-channel/poi/recent/removerecentdestnations");
        }
        break;
        /*
    case NDDSTYPE_TMAP_FINDNEARBY:							// 5.2.7. 좌표로 지명 검색
        {
            bRet = MakeFindNearBy(root, (NDDSTMAP_FINDNEARBY_REQ *)pReqStruct);
            strcpy(pHttpHeader->uri, "/tmap-channel/poi/search/findnearby");
        }
        break;
    case NDDSTYPE_TMAP_FINDPICKATPOPULARS:					// 5.2.8. Pickat 인기검색어 조회
        {
            bRet = MakeFindPickatPopulars(root, (NDDSTMAP_FINDPICKATPOPULARS_REQ *)pReqStruct);
            strcpy(pHttpHeader->uri, "/tmap-channel/poi/search/findpickatpopulars");
        }
        break;
    case NDDSTYPE_TMAP_FINDROADNAME:					// 5.2.9. 새주소(도로명) 검색
        {
            bRet = MakeFindRoadName(root, (NDDSTMAP_FINDROADNAME_REQ *)pReqStruct);
            strcpy(pHttpHeader->uri, "/tmap-channel/poi/search/findroadname");
        }
        break;
    case NDDSTYPE_TMAP_FINDPICKATPOIDETAIL:					// 5.2.10. Pickat POI 상세정보 조회
        {
            bRet = MakeFindPickatPoiDetail(root, (NDDSTMAP_FINDPICKATPOIDETAIL_REQ *)pReqStruct);
            strcpy(pHttpHeader->uri, "/tmap-channel/poi/search/findpickatpoidetail");
        }
        break;
        */
    case NDDSTYPE_TMAP_FINDAREANAMESBYSTEP:					// 5.2.11. 지역분류별 주소명 조회
        {
            bRet = MakeFindAreaNamesByStep(root, (NDDSTMAP_FINDAREANAMESBYSTEP_REQ *)pReqStruct);
            strcpy(pHttpHeader->uri, "/tmap-channel/poi/search/findareanamesbystep");
        }
        break;
    case NDDSTYPE_TMAP_FINDPOISTATIONINFO:					// 5.2.12. 최저가 주유소(충전소)조회
        {
            bRet = MakeFindPoiStationInfo(root, (NDDSTMAP_FINDPOISTATIONINFO_REQ *)pReqStruct);
            strcpy(pHttpHeader->uri, "/tmap-channel/poi/search/findpoistationinfo");
        }
        break;
    case NDDSTYPE_TMAP_FINDPOIEVSTATIONINFO:				// 5.2.16 ev충전소 상세조회
        {
            bRet = MakeFindPoiEvStationInfo(root, (NDDSTMAP_FINDPOIEVSTATIONINFO_REQ *)pReqStruct);
            strcpy(pHttpHeader->uri, "/tmap-channel/poi/evstatus/findpoievstationinfo");
        }
        break;
    case NDDSTYPE_TMAP_FINDPOIEVSTATIONSBYPOINT:				// 5.2.18 주변 전기차 충전소 조회
        {
            bRet = MakeFindPoiEvStationsByPoint(root, (NDDSTMAP_FINDPOIEVSTATIONSBYPOINT_REQ *)pReqStruct);
            strcpy(pHttpHeader->uri, "/tmap-channel/poi/evstatus/findpoievstationsbypoint");
        }
        break;
    case NDDSTYPE_TMAP_FINDPOIFAVORITE:						// 5.3.1. POI 즐겨찾기 조회
        {
            bRet = MakeFindPoiFavorite(root, (NDDSTMAP_FINDPOIFAVORITE_REQ *)pReqStruct);
            strcpy(pHttpHeader->uri, "/tmap-channel/poi/favorite/findpoifavorite");
        }
        break;
    case NDDSTYPE_TMAP_REGISTPOIFAVORITEMULTI:				// 5.3.2. POI 즐겨찾기 등록
        {
            bRet = MakeRegistPoiFavoriteMulti(root, (NDDSTMAP_REGISTPOIFAVORITEMULTI_REQ *)pReqStruct);
            strcpy(pHttpHeader->uri, "/tmap-channel/poi/favorite/registpoifavoritemulti");
        }
        break;
    case NDDSTYPE_TMAP_UPLOADPOIFAVORITE:					// 5.3.3. POI 즐겨찾기 전체 업로드
        {
            bRet = MakeRegistAllPoiFavorite(root, (NDDSTMAP_REGISTALLPOIFAVORITE_REQ *)pReqStruct);
            strcpy(pHttpHeader->uri, "/tmap-channel/poi/favorite/registallpoifavorite");
        }
        break;
    case NDDSTYPE_TMAP_REMOVEPOIFAVORITE:					// 5.3.4. POI 즐겨찾기 멀티 삭제
        {
            bRet = MakeRemovePoiFavorite(root, (NDDSTMAP_REMOVEPOIFAVORITE_REQ *)pReqStruct);
            strcpy(pHttpHeader->uri, "/tmap-channel/poi/favorite/removepoifavorites");
        }
        break;
    case NDDSTYPE_TMAP_MODIFYPOIFAVORITE:					// 5.3.5 POI 즐겨찾기 단건 수정
        {
            bRet = MakeModifyPoiFavorite(root, (NDDSTMAP_MODIFYPOIFAVORITE_REQ *)pReqStruct);
            strcpy(pHttpHeader->uri, "/tmap-channel/poi/favorite/modifypoifavorite");
        }
        break;
    case NDDSTYPE_TMAP_FINDUSERPROFILE:						// 5.3.15 User-Profile 조회
        {
            //user progile 조회
            //bRet = MakeModifyPoiFavorite(root, (NDDSTMAP_MODIFYPOIFAVORITE_REQ *)pReqStruct);
            strcpy(pHttpHeader->uri, "/tmap-channel/poi/userprofile/finduserprofile");
        }
        break;
    case NDDSTYPE_TMAP_REGISTUSERPROFILE:					// 5.3.16 User-Profile 저장
        {
            //user progile 저장
            bRet = MakeRegistUserProfile(root, (NDDSTMAP_REGISTUSERPROFILE_REQ *)pReqStruct);
            strcpy(pHttpHeader->uri, "/tmap-channel/poi/userprofile/registuserprofile");
        }
        break;
    case NDDSTYPE_TMAP_FINDCARPROFILE:						// 5.3.17 Car-Profile 조회
        {
            //user progile 조회
            strcpy(pHttpHeader->uri, "/tmap-channel/poi/userprofile/findcarprofile");
        }
        break;
    case NDDSTYPE_TMAP_REGISTCARPROFILE:					// 5.3.18 Car-Profile 저장
        {
            //carr progile 저장
            bRet = MakeRegistCarProfile(root, (NDDSTMAP_REGISTCARPROFILE_REQ *)pReqStruct);
            strcpy(pHttpHeader->uri, "/tmap-channel/poi/userprofile/registcarprofile");
        }
        break;
    case NDDSTYPE_TMAP_FINDUSERDATA:								// 5.3.14. 최근길/즐겨찾기 전체 리스트 조회
        {
            bRet = MakeFindUserData(root, (NDDSTMAP_FINDUSERDATA_REQ *)pReqStruct);
            strcpy(pHttpHeader->uri, "/tmap-channel/poi/userdata/finduserdata");
        }
        break;
        /*
    case NDDSTYPE_TMAP_FINDROUTE:								// 5.3.4  경로 즐겨찾기 조회
        {
            bRet = MakeFindRoute(root, (NDDSTMAP_FINDROUTE_REQ*)pReqStruct);
            strcpy(pHttpHeader->uri, "/tmap-channel/poi/route/findroute");
        }
        break;
    case NDDSTYPE_TMAP_REGISTROUTEMULTI:							// 5.3.5. 경로 즐겨찾기 등록
        {
            bRet = MakeRegistRouteMulti(root, (NDDSTMAP_REGISTROUTEMULTI_REQ *)pReqStruct);
            strcpy(pHttpHeader->uri, "/tmap-channel/poi/route/registroutemulti");
        }
        break;
    case NDDSTYPE_TMAP_UPLOADTOTALROUTE:							// 5.3.6. 경로 즐겨찾기 전체 업로드
        {
            bRet = MakeUploadTotalRoute(root, (NDDSTMAP_UPLOADTOTALROUTE_REQ *)pReqStruct);
            strcpy(pHttpHeader->uri, "/tmap-channel/poi/route/uploadtotalroute");
        }
        break;
    case NDDSTYPE_TMAP_FINDTRAFFICFAVORITES:						// 5.3.7. 교통정보 즐겨찾기 조회
        {
            bRet = MakeFindTrafficFavorites(root, (NDDSTMAP_FINDTRAFFICFAVORITES_REQ *)pReqStruct);
            strcpy(pHttpHeader->uri, "/tmap-channel/poi/trafficfavorite/findtrafficfavorites");
        }
        break;
    case NDDSTYPE_TMAP_REGISTTRAFFICFAVORITES:					// 5.3.8. 교통정보 즐겨찾기 등록
        {
            bRet = MakeRegistTrafficFavorites(root, (NDDSTMAP_REGISTTRAFFICFAVORITES_REQ *)pReqStruct);
            strcpy(pHttpHeader->uri, "/tmap-channel/poi/trafficfavorite/registtrafficfavorites");
        }
        break;
    case NDDSTYPE_TMAP_UPLOADTRAFFICFAVORITES:					// 5.3.9. 교통정보 즐겨찾기 전체 업로드
        {
            bRet = MakeUploadTrafficFavorites(root, (NDDSTMAP_UPLOADTRAFFICFAVORITES_REQ *)pReqStruct);
            strcpy(pHttpHeader->uri, "/tmap-channel/poi/trafficfavorite/uploadtrafficfavorites");
        }
        break;
    case NDDSTYPE_TMAP_REMOVETRAFFICFAVORITE:					// 5.3.10. 교통정보 즐겨찾기 단일건 삭제
        {
            bRet = MakeRemoveTrafficFavorites(root, (NDDSTMAP_REMOVETRAFFICFAVORITE_REQ *)pReqStruct);
            strcpy(pHttpHeader->uri, "/tmap-channel/poi/trafficfavorite/removetrafficfavorite");
        }
        break;
    case NDDSTYPE_TMAP_FINDNODEJAMRANK:						// 5.4.1. 상습 정체구간 조회
        {
            bRet = MakeFindNodeJamRank(root, (NDDSTMAP_FINDNODEJAMRANK_REQ *)pReqStruct);
            strcpy(pHttpHeader->uri, "/tmap-channel/traffic/congestion/findnodejamrank");
        }
        break;
    case NDDSTYPE_TMAP_SMIMAGETRAFFICCONGESTION:				// 5.4.2. 이미지 교통정보
        {
            bRet = MakeSmimageTrafficCongestion(root, (NDDSTMAP_SMIMAGETRAFFICCONGESTION_REQ *)pReqStruct);
            strcpy(pHttpHeader->uri, "/tmap-channel/simplemap/smimagetrafficcongestion");
        }
        break;
    case NDDSTYPE_TMAP_RADIUSTRAFFICINFOMATION:				// 5.4.3. 주변 교통정보
        {
            bRet = MakeRadiusTrafficInfomtaion(root, (NDDSTMAP_RADIUSTRAFFICINFOMATION_REQ *)pReqStruct);
            pHttpHeader->mineType = NDDS_BINARY;
            strcpy(pHttpHeader->uri, "/tmap-channel/rsd/route/radiustrafficinfomation");
        }
        break;
    case NDDSTYPE_TMAP_FINDTSDSUDN:							// 5.4.4. 돌발 정보 조회
        {
            strcpy(pHttpHeader->uri, "/tmap-channel/traffic/accident/findtsdsudn");
            bRet = MakeFindTsdSudn(root, (NDDSTMAP_FINDTSDSUDN_REQ *)pReqStruct);
        }
        break;
        */
    case NDDSTYPE_TMAP_VMSGUIDEINFO:						// 5.4.5. VMS 가이드 정보 조회
        {
            strcpy(pHttpHeader->uri, "/tmap-channel/traffic/findvmsguideinfo");
            bRet = MakeVmsGuideInfo(root, (NDDSTMAP_VMSGUIDEINFO_REQ *)pReqStruct);
        }
        break;
        /*
    case NDDSTYPE_TMAP_FINDBUSSTATION:						// 5.5.1. 버스 정류장 정보(도착 시간)
        {
            strcpy(pHttpHeader->uri, "/tmap-channel/transport/findbusstation");
            bRet = MakeFindBusStation(root, (NDDSTMAP_FINDBUSSTATION_REQ *)pReqStruct);
        }
        break;
    case NDDSTYPE_TMAP_FINDBUSARRIVAL:						// 5.5.2. 버스 노선 정보(도착 정보)
        {
            strcpy(pHttpHeader->uri, "/tmap-channel/transport/findbusarrival");
            bRet = MakeFindBusArrival(root, (NDDSTMAP_FINDBUSARRIVAL_REQ *)pReqStruct);
        }
        break;
    case NDDSTYPE_TMAP_FINDBUSARRIVALTIME:					// 5.5.3. 버스 노선 정보(도착 시간)
        {
            strcpy(pHttpHeader->uri, "/tmap-channel/transport/findbusarrivaltime");
            bRet = MakeFindBusArrivalTime(root, (NDDSTMAP_FINDBUSARRIVALTIME_REQ *)pReqStruct);
        }
        break;
    case NDDSTYPE_TMAP_FINDBUSSTATIONREALTIME:				// 5.5.4. 버스 정류장/노선 실시간 정보
        {
            strcpy(pHttpHeader->uri, "/tmap-channel/transport/findbusstationrealtime");
            bRet = MakeFindBusStationRealTime(root, (NDDSTMAP_FINDBUSSTATIONREALTIME_REQ *)pReqStruct);
        }
        break;
    case NDDSTYPE_TMAP_FINDSTATIONINFO:						// 5.5.5. 역 정보
        {
            strcpy(pHttpHeader->uri, "/tmap-channel/transport/findstationinfo");
            bRet = MakeFindStationInfo(root, (NDDSTMAP_FINDSTATIONINFO_REQ *)pReqStruct);
        }
        break;
    case NDDSTYPE_TMAP_FINDSTATIONTIMETABLE:					// 5.5.6. 역 운행 정보
        {
            strcpy(pHttpHeader->uri, "/tmap-channel/transport/findstationtimetable");
            bRet = MakeFindStationTimeTable(root, (NDDSTMAP_FINDSTATIONTIMETABLE_REQ *)pReqStruct);
        }
        break;
    case NDDSTYPE_TMAP_FINDSUBWAYIMAGE:						// 5.5.7. 노선 이미지
        {
            strcpy(pHttpHeader->uri, "/tmap-channel/transport/findsubwayimage");
            bRet = MakeFindSubwayImage(root, (NDDSTMAP_FINDSUBWAYIMAGE_REQ *)pReqStruct);
        }
        break;
    case NDDSTYPE_TMAP_FINDPATHFIND1STALL4:					// 5.5.8. 경로안내(도시간 검색 포함)
        {
            strcpy(pHttpHeader->uri, "/tmap-channel/transport/findpathfind1stall4");
            bRet = MakeFindPathFind1stAll4(root, (NDDSTMAP_FINDPATHFIND1STALL4_REQ *)pReqStruct);
        }
        break;
    case NDDSTYPE_TMAP_FINDPATHFIND1STALL4PLURAL:			// 5.5.9. 경로안내(도시내 복수 검색)
        {
            strcpy(pHttpHeader->uri, "/tmap-channel/transport/findpathfind1stall4plural");
            bRet = MakeFindPathFind1stAll4Plural(root, (NDDSTMAP_FINDPATHFIND1STALL4PLURAL_REQ *)pReqStruct);
        }
        break;
    case NDDSTYPE_TMAP_FINDNEARSTATION:						// 5.5.10. 주변 정류장 / 역 조회(거리순)
        {
            strcpy(pHttpHeader->uri, "/tmap-channel/transport/findnearstation");
            bRet = MakeFindNearStation(root, (NDDSTMAP_FINDNEARSTATION_REQ *)pReqStruct);
        }
        break;
    case NDDSTYPE_TMAP_FINDALLTRANSPORT:						// 5.5.11. 통합 검색
        {
            strcpy(pHttpHeader->uri, "/tmap-channel/transport/findalltransport");
            bRet = MakeFindAllTransport(root, (NDDSTMAP_FINDALLTRANSPORT_REQ *)pReqStruct);
        }
        break;
    case NDDSTYPE_TMAP_FINDCCTVRESULT: 						// 5.5.12. 주변 CCTV 리스트
        {
            strcpy(pHttpHeader->uri, "/tmap-channel/transport/findcctvresult");
            bRet = MakeFindCCTVResult(root, (NDDSTMAP_FINDCCTVRESULT_REQ *)pReqStruct);
        }
        break;
    case NDDSTYPE_TMAP_FINDCCTVRESULTBYID:					// 5.5.13. CCTV 정보
        {
            strcpy(pHttpHeader->uri, "/tmap-channel/transport/findcctvresultbyid");
            bRet = MakeFindCCTVResultById(root, (NDDSTMAP_FINDCCTVRESULTBYID_REQ *)pReqStruct);
        }
        break;
        */
    case NDDSTYPE_TMAP_AUTHFORSMARTCAR:					// 5.6.3. 단말인증 (for Smart Car)
        {
            strcpy(pHttpHeader->uri, "/tmap-channel/etc/auth/authtmapforsmartcar");
            bRet = MakeAuthForSmartCar(root, (NDDSTMAP_AUTHFORSMARTCAR_REQ *)pReqStruct);
        }
        break;
        /*
    case NDDSTYPE_TMAP_POSITIONSHARING:						// 5.6.9. 위치 공유
        {
            strcpy(pHttpHeader->uri, "/tmap-channel/tipoff/sharingservice/positionsharing");
            bRet = MakePositionSharing(root, (NDDSTMAP_POSITIONSHARING_REQ *)pReqStruct);
        }
        break;
    case NDDSTYPE_TMAP_ROUTESHARINGCAR:						// 5.6.10. 경로공유 - 자동차
        {
            strcpy(pHttpHeader->uri, "/tmap-channel/tipoff/sharingservice/routesharingcar");
            bRet = MakeRouteSharingCar(root, (NDDSTMAP_ROUTESHARINGCAR_REQ *)pReqStruct);
        }
        break;
    case NDDSTYPE_TMAP_ROUTESHARINGPUBLIC:					// 5.6.11. 경로공유 - 대중교통
        {
            strcpy(pHttpHeader->uri, "/tmap-channel/tipoff/sharingservice/routesharingpublic");
            bRet = MakeRouteSharingPublic(root, (NDDSTMAP_ROUTESHARINGPUBLIC_REQ *)pReqStruct);
        }
        break;
    case NDDSTYPE_TMAP_ROUTESHARINGWALK:						// 5.6.12. 경로공유 - 도보
        {
            strcpy(pHttpHeader->uri, "/tmap-channel/tipoff/sharingservice/routesharingwalk");
            bRet = MakeRouteSharingWalk(root, (NDDSTMAP_ROUTESHARINGWALK_REQ *)pReqStruct);
        }
        break;
    case NDDSTYPE_TMAP_ROUTESHARINGDRIVING:					// 5.6.13. 경로공유 - 주행경로
        {
            strcpy(pHttpHeader->uri, "/tmap-channel/tipoff/sharingservice/routesharingdriving");
            bRet = MakeRouteSharingDriving(root, (NDDSTMAP_ROUTESHARINGDRIVING_REQ *)pReqStruct);
        }
        break;
        */
    case NDDSTYPE_TMAP_REGISTERSENDTOCARINFO:					// 5.6.19. 차량 전송 정보 저장
        {
            strcpy(pHttpHeader->uri, "/tmap-channel/poi/sendtocarinfo/registersendtocarinfo");
            bRet = MakeRegisterSendToCarInfo(root, (NDDSTMAP_REGISTERSENDTOCARINFO_REQ *)pReqStruct);
        }
        break;
    case NDDSTYPE_TMAP_FINDSENDTOCARINFO:					// 5.6.20. 차량 전송 정보 조회
        {
            strcpy(pHttpHeader->uri, "/tmap-channel/poi/sendtocarinfo/findsendtocarinfo");
            bRet = MakeFindSendToCarInfo(root, (NDDSTMAP_FINDSENDTOCARINFO_REQ *)pReqStruct);
        }
        break;
    case NDDSTYPE_TMAP_REMOVESENDTOCARINFO:					// 5.6.21. 차량 전송 정보 삭제
        {
            strcpy(pHttpHeader->uri, "/tmap-channel/poi/sendtocarinfo/removesendtocarinfo");
            bRet = MakeRemoveSendToCarInfo(root, (NDDSTMAP_REMOVESENDTOCARINFO_REQ *)pReqStruct);
        }
        break;
        /*
    case NDDSTYPE_TMAP_FINDCHARGENOTICES:					// 5.7.1  공지사항(요금제)
        {
            bRet = MakeFindChargeNotices(root, (NDDSTMAP_FINDCHARGENOTICES_REQ *)pReqStruct);
            strcpy(pHttpHeader->uri, "/tmap-channel/etc/noticeservice/findchargenotices");
        }
        break;
    case NDDSTYPE_TMAP_FINDFILEACCESS:						// 5.7.2  파일(이미지,기타) 정보
        {
            bRet = MakeFindFileAccess(root, (NDDSTMAP_FINDFILEACCESS_REQ *)pReqStruct);
            strcpy(pHttpHeader->uri, "/tmap-channel/etc/fileaccessservice/findfileaccess");
        }
        break;
    case NDDSTYPE_TMAP_FINDOPENAPPS:							// 5.7.4  오픈 앱 리스트
        {
            bRet = MakeFindOpenApps(root, (NDDSTMAP_FINDOPENAPPS_REQ *)pReqStruct);
            strcpy(pHttpHeader->uri, "/tmap-channel/etc/openappservice/findopenapps");
        }
        break;
    case NDDSTYPE_TMAP_FINDSEEDKEYINFO:						// 5.7.9  Seed Key 정보 요청
        {
            bRet = MakeFindSeedKeyInfo(root, (NDDSTMAP_FINDSEEDKEYINFO_REQ *)pReqStruct);
            strcpy(pHttpHeader->uri, "/tmap-channel/etc/seedkey/findseedkeyinfo");
        }
        break;
    case NDDSTYPE_TMAP_APPLOGSAVE:							// 5.7.10 App Log 저장
        {
            bRet = MakeAppLogSave(root, (NDDSTMAP_APPLOGSAVE_REQ *)pReqStruct);
            strcpy(pHttpHeader->uri, "/tmap-channel/etc/applog/save");
        }
        break;
    case NDDSTYPE_TMAP_ROUTEHISTORYSAVE:						// 5.7.11 주행 이력 저장
        {
            bRet = MakeRouteHistorySave(root, (NDDSTMAP_ROUTEHISTORYSAVE_REQ *)pReqStruct);
            strcpy(pHttpHeader->uri, "/tmap-channel/etc/routehistory/save");
        }
        break;
    case NDDSTYPE_TMAP_SAVEADVERTISE:						// 5.8.2. 광고 음원 노출 보고
        {
            strcpy(pHttpHeader->uri, "/tmap-channel/advertise/sound/saveadvertise");
            bRet = MakeSaveAdvertise(root, (NDDSTMAP_SAVEADVERTISE_REQ *)pReqStruct);
        }
        break;
    case NDDSTYPE_TMAP_FINDAGREEMENT:						// 5.9.1. 이용약관 동의 여부 조회
        {
            strcpy(pHttpHeader->uri, "/tmap-channel/terms/findagreement");
            bRet = MakeFindAgreement(root, (NDDSTMAP_FINDAGREEMENT_REQ *)pReqStruct);
        }
        break;
    case NDDSTYPE_TMAP_REGISTAGREEMENT:						// 5.9.2. 이용약관 동의 결과 저장
        {
            strcpy(pHttpHeader->uri, "/tmap-channel/terms/registagreement");
            bRet = MakeRegistAgreement(root, (NDDSTMAP_REGISTAGREEMENT_REQ *)pReqStruct);
        }
        break;
    case NDDSTYPE_TMAP_SERVICECLOSE:
        break;
        */
    case NDDSTYPE_TMAP_NONNDDS:
        {
            char *tmp;
            char *url = ((NDDSTMAP_NONNDDS_REQ *)pReqStruct)->pszUrl;
            char *domain = ((NDDSTMAP_NONNDDS_REQ *)pReqStruct)->pszDomain;

            tmp = strstr(url, "https://");
            if (tmp == nullptr)
                url = strstr(url, "/");
            else
                url = strstr(url + 8, "/");
            if (url == nullptr)
                return false;

            tmp = strstr(domain, "https://");
            if (tmp != nullptr)
                domain += 8;

            strcpy(pHttpHeader->uri, url);

            char tmpRequest[1024] = {0};
            sprintf(tmpRequest, "GET %s HTTP/1.1\r\nHost: %s\r\nContent-Type: application/json;charset=utf-8\r\n\r\n", url, domain);
            pSessionData->nRequestData = strlen(tmpRequest);
            pSessionData->pRequestData = (char*)malloc(pSessionData->nRequestData+1);
            strcpy(pSessionData->pRequestData, tmpRequest);
            return true;
        }
        break;
    default:
        assert(0);
        *pHttpHeader->uri = NULL;
        break;
    }


    if(!bRet)
    {
        return false;
    }

    AddCommonHeader(root, pCommonHeader, true);

    // Json 텍스트 생성 (UTF8)
    std::string output = writer.write( root );
    nJson = output.size();
    if(CommonData.m_eCharacterSet == NDDSCHARACTERSET_EUCKR)
    {
        nAllocSize = sizeof(wchar_t) * (nJson + 1);
        wszJson = (wchar_t *)CommonData.GetMemory(nAllocSize);
        nddsMultiByteToWideChar((char*)output.c_str(), wszJson, nAllocSize);
#ifdef _LINUX
        char *pTemp = (char*)malloc((wcslen(wszJson)+1)*2);
        UCS2toUTF8((unsigned short*)wszJson, wcslen(wszJson), pTemp);
#if 0
        if(NDDSTYPE_TMAP_PLANNINGROUTE == pSessionData->eProtocolType)
        {
            printf("*************************************************************\n");fflush(stdout);
            printf("%s\n", pTemp);fflush(stdout);
            printf("*************************************************************\n");fflush(stdout);
        }
#endif

#else
        char *pTemp = SDL_iconv_string("UTF-8", "UCS-2-INTERNAL", (char*)wszJson, (wcslen(wszJson)+1)*2);
#endif
        szJson = (char*)CommonData.GetMemory(nAllocSize);
        strcpy(szJson, pTemp);
        CommonData.FreeMemory((unsigned char*)wszJson);
#ifdef _LINUX
        free(pTemp);
#else
        SDL_free(pTemp);
#endif

    }else
    {
        nAllocSize = nJson + 1;
        szJson = (char*)CommonData.GetMemory(nAllocSize);
        strcpy(szJson, (char*)output.c_str());
    }
    DBGPRINT(_T("%s\n"), szJson);
    nContentLen = strlen(szJson);
    //////////////////////////////////////////////////////////////////////////
    // 요청 데이터 Http 해더 생성
    char *szHttpHeader = (char*)CommonData.GetMemory(1000);
    strcpy(pHttpHeader->serviceID, NDDS_START_SERVICE_ID);
    pHttpHeader->bodySize = nContentLen;
    bool bAuth = false;
    if(pSessionData->eProtocolType == NDDSTYPE_TMAP_AUTHFORSMARTCAR)
        bAuth = true;
    nHttpHeader = NddsLibHttpHeader::NddsMakeTmapHeader(szHttpHeader, pHttpHeader, bAuth);
    DBGPRINT(_T("%s\n"), szHttpHeader);

    //////////////////////////////////////////////////////////////////////////
    // 요청 데이터(Header+Body) 생성

    pSessionData->nRequestData = nHttpHeader + nContentLen;
    pSessionData->pRequestData = (char*)malloc(pSessionData->nRequestData+1);

    memcpy(pSessionData->pRequestData, szHttpHeader, nHttpHeader);
    memcpy(pSessionData->pRequestData + nHttpHeader, szJson, nContentLen);
    pSessionData->pRequestData[pSessionData->nRequestData] = NULL;

    CommonData.FreeMemory((unsigned char*)szJson);
    CommonData.FreeMemory((unsigned char*)szHttpHeader);

    return true;
}

bool CNddsLibTmap::RequestServer()
{
    if(!CNddsLibBase::RequestServer())
        return false;

    return true;
}

bool CNddsLibTmap::AddCommonHeader(NDDSLIB_Json::Value &root, NDDS_COMMONHEADER_REQ *pCommonHeader, bool bSeedKey)
{
    NDDSLIB_Json::Value header;
#if 1 // ndds test
    header["loginType"] = 0;
    header["id"] = "neonex-sk";
    header["loginInfo"] = "Tmap2015";

#endif
    header["reqTime"]	= pCommonHeader->reqTime;
    header["svcType"]	= pCommonHeader->svcType;
    header["osType"]	= pCommonHeader->osType;
    header["osVersion"]	= pCommonHeader->osVersion;
    header["deviceId"]	= pCommonHeader->deviceId;//new
    header["carrier"]	= pCommonHeader->carrier;//new
    header["modelNo"]	= pCommonHeader->modelNo;
    header["appVersion"]= pCommonHeader->appVersion;
    header["buildNo"]	= pCommonHeader->buildNo;
    header["resolution"]= pCommonHeader->resolution;
    header["using"]	    = pCommonHeader->usingInfo;
    header["screenWidth"]= pCommonHeader->screenWidth;//new
    header["screenHeight"]	= pCommonHeader->screenHeight;//new
    header["pushDeviceKey"]	=  "";//new option
    root["header"] = header;
    return true;
}

bool CNddsLibTmap::MakePlanningRoute(NDDSLIB_Json::Value &root, NDDSTMAP_PLANNINGROUTE_REQ *pReq)
{
    CNddsLibCommonData &CommonData = CNddsLibCommonData::GetInstance();
    NDDSLIB_Json::Value common,departure,destination,additionalInfo,wayPoints,pastPositionInfoDto,tvasReSearch;

    try
    {
        if(pReq->ReSearchType == REQ_NONE)
            return false;

        root["tvas"] = pReq->tvas;;
        root["resFlag"] = "1";

        // 경로탐색 옵션
        for(int i = 0 ; i < pReq->common.countRoutePlanTypes ; i++)
        {
            NDDSLIB_Json::Value typeValue;
            switch(pReq->common.routePlanTypes[i])
            {
            case TRAFFIC_RECOMMEND:			typeValue = "Traffic_Recommend";	break;
            case TRAFFIC_FREE:				typeValue = "Traffic_Free";			break;
            case TRAFFIC_MINTIME:			typeValue = "Traffic_MinTime";		break;
            case TRAFFIC_FIRSTTIME:			typeValue = "Traffic_FirstTime";	break;
            case TRAFFIC_HIGHWAY:			typeValue = "Traffic_Highway";		break;
            case SHORTEST_CHARGEDANDFREE:	typeValue = "Shortest_ChargedAndFree";	break;
            case TRAFFIC_GENERALROAD:		typeValue = "Traffic_GeneralRoad";	break;
            }
            if(!typeValue.empty())
                root["routePlanTypes"].append(typeValue);
        }

        root["detailLocFlag"] = "NotApplied";

        switch(pReq->serviceFlag)
        {
        case REALTIME:               root["serviceFlag"] = "Realtime";	break;
        case DEPARTURETIMEESTIMATION:root["serviceFlag"] = "DepartureTimeEstimation";		break;
        case ARRIVALTIMEESTIMATION:  root["serviceFlag"] = "ArrivalTimeEstimation";		break;
        }

        if(pReq->patternTime != NULL)
            root["patternTime"] = pReq->patternTime;

        switch(pReq->patternWeek)
        {
        case SUNDAY:    root["patternWeek"] = "Sunday";     break;
        case MONDAY:    root["patternWeek"] = "Monday";		break;
        case TUESDAY:   root["patternWeek"] = "Tuesday";		break;
        case WEDNESDAY: root["patternWeek"] = "Wednesday";		break;
        case THURSDAY:  root["patternWeek"] = "Thursday";		break;
        case FRIDAY:    root["patternWeek"] = "Friday";         break;
        case SATURDAY:  root["patternWeek"] = "Saturday";		break;
        case HOLIDAY:   root["patternWeek"] = "Holiday";		break;
        }

        if(pReq->guideImgResolutionCode[0] != 0x00)
        {
            root["guideImgResolutionCode"] = pReq->guideImgResolutionCode;
        }else{
            root["guideImgResolutionCode"] = "R480x540";
        }

        for(int i = 0 ; i < pReq->countdangerAreaOptions ; i++)
        {
            NDDSLIB_Json::Value type;
            switch(pReq->dangerAreaOptions[i])
            {
            case NOVALUE:           type = "NoValue";	break;
            case OCCURFREQUENTLY:	type = "OccurFrequently";	break;
            case SHARPCURVESECTION:	type = "SharpCurveSection";	break;
                //case STEEPSLOPE:		type = "SteepSlope";			break;
                //case ROADKILL:          type = "RoadKill";	break;
            case MISTAREA:          type = "MistArea";	break;
                //case FALLINGROCK:		type = "FallingRock";	break;
            case SCHOOLZONE:		type = "SchoolZone";	break;
            case TRAINCROSSING:		type = "TrainCrossing";	break;
            }
            if(!type.empty())
                root["dangerAreaOptions"].append(type);
        }

        root["routePlanAroundRange"] = "1";

        /************************************************************************/
        /* 길안내 출발지 정보(departure)                                        */
        /************************************************************************/
        // 요청지점 도로타입
        switch(pReq->departure.departRoadType)
        {
        case ROADTYPE_NONE:		root["departRoadType"] = "None";		break;
        case ROADTYPE_HIGHWAY:	root["departRoadType"] = "Highway";	break;
        case ROADTYPE_CARWAY:	root["departRoadType"] = "Carway";		break;
        case ROADTYPE_OVERPASS:	root["departRoadType"] = "Overpass";	break;
        case ROADTYPE_UNDERPASS:	root["departRoadType"] = "Underpass";	break;
        case ROADTYPE_GENERAL:	root["departRoadType"] = "General";	break;
        case ROADTYPE_NEARWAY:	root["departRoadType"] = "NearWay";	break;
        case ROADTYPE_NOTSELECTED:	root["departRoadType"] = "NotSelected";	break;
        }

        if(pReq->departure.departName)
            root["departName"] = pReq->departure.departName;
        else
            root["departName"] = "출발지";
        root["departXPos"] = pReq->departure.departXPos;
        root["departYPos"] = pReq->departure.departYPos;
        root["angle"] = pReq->departure.angle;
        root["speed"] = pReq->departure.speed;
        root["departDirPriority"] = pReq->departure.departDirPriority;
        root["departSrchFlag"] = pReq->departure.departSrchFlag;

		/************************************************************************/
		/* 길안내 목적지 정보(destination)                                      */
		/************************************************************************/
		switch(pReq->destination.destSearchFlag)
		{
        case 0:		root["destSearchFlag"] = "Gps";			break;
        case 1:		root["destSearchFlag"] = "LeaveReSearch";	break;
        case 2:		root["destSearchFlag"] = "UserResearch";	break;
        case 3:		root["destSearchFlag"] = "NearFacility";	break;
        case 4:		root["destSearchFlag"] = "RecentRouteGuide";break;
        case 5:		root["destSearchFlag"] = "Favorite";		break;
        case 6:		root["destSearchFlag"] = "LongitudeSearch";break;
        case 7:		root["destSearchFlag"] = "NameSearch";		break;
        case 8:		root["destSearchFlag"] = "AddressSearch";	break;
        case 9:		root["destSearchFlag"] = "LeisureAndLife";	break;
        case 10:	root["destSearchFlag"] = "ReservedRouteGuide";	break;
        case 11:	root["destSearchFlag"] = "LocalBuisness";	break;
        case 12:	root["destSearchFlag"] = "PhoneNumber";	break;
        case 13:	root["destSearchFlag"] = "LocalName";		break;
        //case 14:	root["destSearchFlag"] = "VoiceRouteGuide";break;
        case 15:	root["destSearchFlag"] = "AfterMapMoving";	break;
        case 16:	root["destSearchFlag"] = "MyLocationView";	break;
        case 17:	root["destSearchFlag"] = "ReceiveLocationView";break;
        case 18:	root["destSearchFlag"] = "SendingLocationView";break;
        //case 19:	root["destSearchFlag"] = "AltRouteInitSearch";	break;
        //case 20:	root["destSearchFlag"] = "AfterMapSearch";		break;
        //case 21:	root["destSearchFlag"] = "AltRouteLeaveResearch";break;
        //case 22:	root["destSearchFlag"] = "AltRouteUserResearch	";break;
        case 23:	root["destSearchFlag"] = "ForceResearch";		break;
        case 24:	root["destSearchFlag"] = "FamousRestaurant";	break;
        case 25:    root["destSearchFlag"] = "RegularResearchOuterCase";break;
        case 26:    root["destSearchFlag"] = "RegularResearchInnerCase";break;
        case 27:	root["destSearchFlag"] = "IntegrationSearch";		break;
        case 28:	root["destSearchFlag"] = "PrivateSecretary";		break;
        case 29:	root["destSearchFlag"] = "OptionChangeResearch";	break;
        case 30:	root["destSearchFlag"] = "NearLinkResearch";		break;
        case 31:	root["destSearchFlag"] = "RouteFavorite";			break;
        case 32:	root["destSearchFlag"] = "GoHome";					break;
        case 33:	root["destSearchFlag"] = "GoCompany";				break;
        case 34:	root["destSearchFlag"] = "FrequentRoute";			break;
        case 112:   root["destSearchFlag"] = "PublicTrafficeRouteSummary";break;
        case 113:	root["destSearchFlag"] = "BranchAltResearch";		break;
        case 114:	root["destSearchFlag"] = "AvoidAltResearch";		break;
        case 116:	root["destSearchFlag"] = "EndResearch";				break;
        case 118:	root["destSearchFlag"] = "WaypointSearch";			break;
        case 119:	root["destSearchFlag"] = "SpeechRecogSearch";		break;
        case 255:	root["destSearchFlag"] = "Etc";						break;
        default:    root["destSearchFlag"] = pReq->destination.destSearchFlag;break;
		}

        if(pReq->destination.destName)
            root["destName"] = pReq->destination.destName;
        else
            root["destName"] = pReq->destination.destName;
        root["destXPos"] = pReq->destination.destXPos;
        root["destYPos"] = pReq->destination.destYPos;
        if(pReq->destination.destPoiId != NULL)
            root["destPoiId"] = pReq->destination.destPoiId;
        root["destRpFlag"] = pReq->destination.destRpFlag;
        root["navSeq"] = pReq->destination.navSeq;

        if(pReq->gpsTraceData != NULL)
            root["gpsTraceData"] = pReq->gpsTraceData;

        /************************************************************************/
        /* 경로요청 추가정보(additionalInfo)                                    */
        /************************************************************************/
        // 톨케이트 요금 차종
        switch(pReq->additionalInfo.tollCarType)
        {
        case CARTYPE_NONE:				root["tollCarType"] = "None";		break;
        case CARTYPE_CAR:				root["tollCarType"] = "Car";		break;
        case CARTYPE_MEDIUMVAN:			root["tollCarType"] = "MediumVan";break;
        case CARTYPE_LARGEVAN:			root["tollCarType"] = "LargeVan";	break;
        case CARTYPE_LARGETRUCK:		root["tollCarType"] = "LargeTruck";break;
        case CARTYPE_SPECIALTRUCK:		root["tollCarType"] = "SpecialTruck";break;
        case CARTYPE_SMALLCAR:			root["tollCarType"] = "SmallCar";	break;
        case CARTYPE_TWOWHEELEDVEHICLE:	root["tollCarType"] = "TwoWheeledVehicle";break;
        }
        // 차량 유류 종류
        switch(pReq->additionalInfo.carOilType)
        {
        case OIL_NONE:		root["carOilType"] = "None";	break;
        case OIL_GASOLINE:	root["carOilType"] = "Gasoline";	break;
        case OIL_DIESEL:	root["carOilType"] = "Diesel";break;
        case OIL_LPG:		root["carOilType"] = "Lpg";	break;
        case OIL_CHAdeMO:		root["carOilType"] = "EV_CHAdeMO";	break;
        case OIL_AC3:		root["carOilType"] = "EV_AC3";	break;
        case OIL_DCCombo:		root["carOilType"] = "EV_DCCombo";	break;
        case OIL_PremiumGasoline:	root["carOilType"] = "PremiumGasoline";	break;
        }
        // 단속카메라 추가 요청
        for(int i = 0 ; i < pReq->additionalInfo.countAddCameraTypes ; i++)
        {
            NDDSLIB_Json::Value type;
            switch(pReq->additionalInfo.addCameraTypes[i])
            {
            case CAMERA_NOVALUE:		type = "NoValue";	break;
            case CAMERA_BUS:			type = "Bus";	break;
            case CAMERA_SIGNALSPEEDING:	type = "SignalSpeeding";	break;
            case CAMERA_MOVING:			type = "Moving";			break;
            case CAMERA_SHOULDERCONTROL:type = "ShoulderControl";	break;
            case CAMERA_CHANGEABLEROAD:	type = "ChangeableRoad";	break;
            case CAMERA_OVERLOADED:		type = "OverLoaded";	break;
            case CAMERA_SECURITY:		type = "Security";	break;
            case CAMERA_TRAFFIC:		type = "Traffic";	break;
            }

            if(!type.empty())
                root["addCameraTypes"].append(type);
        }

        // 요금 가중치 옵션
        for(int i = 0 ; i < pReq->additionalInfo.countFareWeightOpts ; i++)
        {
            NDDSLIB_Json::Value opts;
            switch(pReq->additionalInfo.fareWeightOpts[i])
            {
            case BOTHCHARGEDANDFREE:opts = "BothChargedAndFree";break;
            case OPTIMIZEDCHARGED:	opts = "OptimizedCharged";break;
            case MINUMCHARGED:		opts = "MinumCharged";	break;
            case FREEFIRST:			opts = "FreeFirst";		break;
            case LOGICAPPLIED:		opts = "LogicApplied";	break;
            }
            if(!opts.empty())
                root["fareWeightOpts"].append(opts);
        }

        root["controlRouteReqFlag"] = pReq->additionalInfo.controlRouteReqFlag;
        root["hipassFlag"] = pReq->additionalInfo.hipassFlag;

        /************************************************************************/
        /* 경유지 목록                                                          */
        /************************************************************************/
        if(pReq->countWayPoints > 0)
        {
            for(int i = 0; i < pReq->countWayPoints ; i++)
            {
                NDDSLIB_Json::Value point;

                point["x"] = pReq->wayPoints[i].x;
                point["y"] = pReq->wayPoints[i].y;
                if(pReq->wayPoints[i].poiID)
                    point["poiID"] = pReq->wayPoints[i].poiID;
                point["rpFlag"] = pReq->wayPoints[i].rpFlag;

                wayPoints.append(point);
            }
            root["wayPoints"] = wayPoints;
        }
        /************************************************************************/
        /* 기타 정보                                                            */
        /************************************************************************/

        if(pReq->gpsTraceData != NULL && pReq->nGpsTraceDataSize > 0)
        {
            int nAllocSize = (4 * (pReq->nGpsTraceDataSize / 3)) + (pReq->nGpsTraceDataSize % 3? 4 : 0) + 1;

            char* encodingData = (char*)CommonData.GetMemory(pReq->nGpsTraceDataSize + 1);
            memcpy(encodingData, pReq->gpsTraceData, pReq->nGpsTraceDataSize + 1);
            root["gpsTraceData"] = encodingData;

            CommonData.FreeMemory((unsigned char*)encodingData);
        }

        /************************************************************************/
        /* 지난 위치 정보(pastPositionInfoDto)                                        */
        /************************************************************************/
        if(pReq->pastPositionInfoDto.pastSessionId != NULL)
            root["pastSessionId"] = pReq->pastPositionInfoDto.pastSessionId;

        root["pastXpos"] = pReq->pastPositionInfoDto.pastXpos;
        root["pastYpos"] = pReq->pastPositionInfoDto.pastYpos;

        if(pReq->pastPositionInfoDto.pastArrivalTime != NULL)
            root["pastArrivalTime"] = pReq->pastPositionInfoDto.pastArrivalTime;

        /************************************************************************/
        /* 탐색 조건                                 */
        /************************************************************************/
        if(pReq->tvasReSearch.searchCondData != NULL)
            root["searchCondData"] = pReq->tvasReSearch.searchCondData;

        /************************************************************************/
        /* 재탐색 데이터 (tvasReSearch)                                  */
        /************************************************************************/
        root["lastTollgateId"] = pReq->tvasReSearch.lastTollgateId;
        root["lastRid"] = pReq->tvasReSearch.lastRid;
        root["tcRid"] = pReq->tvasReSearch.tcRid;

        if(pReq->tvasReSearch.preMapVersion != NULL)
            root["preMapVersion"] = pReq->tvasReSearch.preMapVersion;

        if(pReq->tvasReSearch.preRids != NULL)
        {
            for(int i=0;i<pReq->tvasReSearch.preRidsCnt;i++)
            {
                NDDSLIB_Json::Value prerids;
                prerids = pReq->tvasReSearch.preRids[i];
                root["preRids"].append(prerids);
            }
        }

        if(pReq->tvasReSearch.preSecs != NULL)
        {
            for(int i=0;i<pReq->tvasReSearch.preSecsCnt;i++)
            {
                NDDSLIB_Json::Value presecs;
                presecs = pReq->tvasReSearch.preSecs[i];
                root["preSecs"].append(presecs);
            }
        }

        if(pReq->tvasReSearch.preTollgateIds != NULL)
            root["preTollgateIds"] = pReq->tvasReSearch.preTollgateIds;

        root["initSrchLength"] = pReq->tvasReSearch.initSrchLength;

        if(pReq->tvasReSearch.initSrchSessionId != NULL)
            root["initSrchSessionId"] = pReq->tvasReSearch.initSrchSessionId;

        /************************************************************************/
        /* 경로 주변 링크                                 */
        /************************************************************************/
        root["radiusInfoType"] = pReq->tvasReSearch.radiusInfoType;
        root["radiusEntryTurnType"] = pReq->tvasReSearch.radiusEntryTurnType;
        root["radiusLinkId"] = pReq->tvasReSearch.radiusLinkId;
        root["radiusMeshCode"] = pReq->tvasReSearch.radiusMeshCode;
        root["radiusLinkDirection"] = pReq->tvasReSearch.radiusLinkDirection;
        root["radiusLeaveTurnType"] = pReq->tvasReSearch.radiusLeaveTurnType;
        root["radiusXPos"] = pReq->tvasReSearch.radiusXPos;
        root["radiusYPos"] = pReq->tvasReSearch.radiusYPos;
    }catch (...)
    {
        return false;
    }
    return true;
}

bool CNddsLibTmap::MakeFindRoute(NDDSLIB_Json::Value &root, NDDSTMAP_FINDROUTE_REQ *pReq)
{
    // 요청 데이터 없음
    return true;
}

bool CNddsLibTmap::MakeRouteSummaryInfo(NDDSLIB_Json::Value &root, NDDSTMAP_ROUTESUMMARYINFO_REQ *pReq)
{
    try
    {
        NDDSLIB_Json::Value wayPoints, commingTime;

        root["firstGuideOption"] = pReq->firstGuideOption;
        root["serviceFlag"] = pReq->serviceFlag;
        commingTime.append(pReq->commingTime);
        root["commingTime"] = commingTime;
        root["vertexFlag"] = pReq->vertexFlag;
        root["departName"] = pReq->departName;
        root["departXPos"] = pReq->departXPos;
        root["departYPos"] = pReq->departYPos;
        root["angle"] = pReq->angle;
        root["speed"] = pReq->speed;
        root["departDirPriority"] = pReq->departDirPriority;
        root["departSrchFlag"] = pReq->departSrchFlag;
        root["destName"] = pReq->destName;
        root["destXPos"] = pReq->destXPos;
        root["destYPos"] = pReq->destYPos;
        root["destRpFlag"] = pReq->destRpFlag;
        root["destSearchFlag"] = pReq->destSearchFlag;
        root["destPoiId"] = pReq->destPoiId;

        if(pReq->nWayPoints > 0)
        {
            for(int i = 0; i < pReq->nWayPoints ; i++)
            {
                NDDSLIB_Json::Value point;

                point["x"] = pReq->wayPoints[i].x;
                point["y"] = pReq->wayPoints[i].y;
                if(pReq->wayPoints[i].poiID)
                    point["poiID"] = pReq->wayPoints[i].poiID;
                point["rpFlag"] = pReq->wayPoints[i].rpFlag;

                wayPoints.append(point);
            }
            root["wayPoints"] = wayPoints;
        }
    }catch (...)
    {
        return false;
    }

    return true;
}

bool CNddsLibTmap::MakeFindPois(NDDSLIB_Json::Value &root, NDDSTMAP_FINDPOIS_REQ *pReq)
{
    try
    {
        root["reqCnt"] = pReq->reqCnt;
        root["noorX"] = pReq->noorX;
        root["noorY"] = pReq->noorY;
        root["areaName"] = pReq->areaName;
        root["name"] = pReq->name;
        root["reqSeq"] = pReq->reqSeq;
        root["radius"] = pReq->radius;
        root["searchTypCd"] = pReq->searchTypCd;
        root["poiGroupYn"] = "N";
    }catch (...)
    {
        return false;
    }


    return true;
}

bool CNddsLibTmap::MakeFindChargeNotices(NDDSLIB_Json::Value &root, NDDSTMAP_FINDCHARGENOTICES_REQ *pReq)
{
    // 요청 데이터 없음
    return true;
}

bool CNddsLibTmap::MakeFindOpenApps(NDDSLIB_Json::Value &root, NDDSTMAP_FINDOPENAPPS_REQ *pReq)
{
    // 요청 데이터 없음
    return true;
}

bool CNddsLibTmap::MakeFindSeedKeyInfo(NDDSLIB_Json::Value &root, NDDSTMAP_FINDSEEDKEYINFO_REQ *pReq)
{
    // 요청 데이터 없음
    return true;
}

bool CNddsLibTmap::MakeAppLogSave(NDDSLIB_Json::Value &root, NDDSTMAP_APPLOGSAVE_REQ *pReq)
{
    try
    {
        root["appType"] = pReq->appType;
        root["runType"] = pReq->runType;
    }catch (...)

    {
        return false;
    }
    return true;
}
// 5.7.11 주행 이력 저장 요청
bool CNddsLibTmap::MakeRouteHistorySave(NDDSLIB_Json::Value &root, NDDSTMAP_ROUTEHISTORYSAVE_REQ *pReq)
{
    try
    {
        root["departName"] = pReq->departName;
        root["departXPos"] = pReq->departXPos;
        root["departYPos"] = pReq->departYPos;
        root["destName"] = pReq->destName;
        root["destXPos"] = pReq->destXPos;
        root["destYPos"] = pReq->destYPos;
        root["destPoiId"] = pReq->destPoiId;
        root["destRpFlag"] = pReq->destRpFlag;
        root["totalDistance"] = pReq->totalDistance;
        root["totalTime"] = pReq->totalTime;
        root["averageSpeed"] = pReq->averageSpeed;
        root["maxSpeed"] = pReq->maxSpeed;
        root["tvasEstimationTime"] = pReq->tvasEstimationTime;
        root["realEstimationTime"] = pReq->realEstimationTime;
        root["reserchCnt"] = pReq->reserchCnt;
        root["routeSessionId"] = pReq->routeSessionId;
        root["fuelSavingCost"] = pReq->fuelSavingCost;
        root["co2Quantity"] = pReq->co2Quantity;
        root["routeEndDivision"] = pReq->routeEndDivision;
        root["routeRevisitingCount"] = pReq->routeRevisitingCount;
        root["greenHouseGasReduction"] = pReq->greenHouseGasReduction;
    }catch (...)
    {
        return false;
    }
    return true;
}


bool CNddsLibTmap::MakeFindAroundPois(NDDSLIB_Json::Value &root, NDDSTMAP_FINDAROUNDPOIS_REQ *pReq)
{
    try
    {
        root["reqCnt"] = pReq->reqCnt;
        root["noorX"] = pReq->noorX;
        root["noorY"] = pReq->noorY;
        root["name"] = pReq->name;
        root["reqSeq"] = pReq->reqSeq;
        root["radius"] = pReq->radius;
    }catch (...)
    {
        return false;
    }

    return true;
}

bool CNddsLibTmap::MakeFindFileAccess(NDDSLIB_Json::Value &root, NDDSTMAP_FINDFILEACCESS_REQ *pReq)
{
    NDDSLIB_Json::Value filePath;

    try
    {
        for(int i =0 ; i< pReq->nFilePath;i++)
            filePath.append(pReq->filePath[i]);

        root["filePath"]=filePath;
    }catch (...)
    {
        return false;
    }

    return true;
}


bool CNddsLibTmap::MakeRegistPoiFavoriteMulti(NDDSLIB_Json::Value &root, NDDSTMAP_REGISTPOIFAVORITEMULTI_REQ *pReq)
{
    NDDSLIB_Json::Value element;

    try
    {
        for(int i=0 ; i<pReq->nPoiFavorites ; i++)
        {
            element.clear();

            element["custName"] = pReq->poiFavorites[i].custName;
            element["noorX"] = pReq->poiFavorites[i].noorX;
            element["noorY"] = pReq->poiFavorites[i].noorY;
            element["poiId"] = pReq->poiFavorites[i].poiId;
            element["navSeq"] = pReq->poiFavorites[i].navSeq;
            element["rpFlag"] = pReq->poiFavorites[i].rpFlag;
            element["addInfo"] = pReq->poiFavorites[i].addInfo;

            root["poiFavorites"].append(element);
        }
    }catch (...)
    {
        return false;
    }

    return true;
}

// 5.3.3. POI 즐겨찾기 전체 업로드
bool CNddsLibTmap::MakeRegistAllPoiFavorite(NDDSLIB_Json::Value &root, NDDSTMAP_REGISTALLPOIFAVORITE_REQ *pReq)
{


    try
    {
        if(pReq->uploadOption != NULL)
            root["uploadOption"] = pReq->uploadOption;

        NDDSLIB_Json::Value element;
        if(pReq->poiMyFavorite.homeCustName != NULL || pReq->poiMyFavorite.homeCustName != 0)
            element["homeCustName"] = pReq->poiMyFavorite.homeCustName;
        if(pReq->poiMyFavorite.homeNoorX != NULL  || pReq->poiMyFavorite.homeNoorX != 0)
            element["homeNoorX"] = pReq->poiMyFavorite.homeNoorX;
        if(pReq->poiMyFavorite.homeNoorY != NULL || pReq->poiMyFavorite.homeNoorY != 0)
            element["homeNoorY"] = pReq->poiMyFavorite.homeNoorY;
        if(pReq->poiMyFavorite.homePoiId != NULL || pReq->poiMyFavorite.homePoiId != 0)
            element["homePoiId"] = pReq->poiMyFavorite.homePoiId;
        if(pReq->poiMyFavorite.homeNavSeq != NULL || pReq->poiMyFavorite.homeNavSeq != 0)
            element["homeNavSeq"] = pReq->poiMyFavorite.homeNavSeq;
        if(pReq->poiMyFavorite.homeRpFlag != NULL || pReq->poiMyFavorite.homeRpFlag != 0)
           element["homeRpFlag"] = pReq->poiMyFavorite.homeRpFlag;
        if(pReq->poiMyFavorite.homeAddInfo != NULL || pReq->poiMyFavorite.homeAddInfo != 0)
            element["homeAddInfo"] = pReq->poiMyFavorite.homeAddInfo;
        if(pReq->poiMyFavorite.officeCustName != NULL || pReq->poiMyFavorite.officeCustName != 0)
            element["officeCustName"] = pReq->poiMyFavorite.officeCustName;
        if(pReq->poiMyFavorite.officeNoorX != NULL || pReq->poiMyFavorite.officeNoorX != 0)
            element["officeNoorX"] = pReq->poiMyFavorite.officeNoorX;
        if(pReq->poiMyFavorite.officeNoorY != NULL || pReq->poiMyFavorite.officeNoorY != 0)
            element["officeNoorY"] = pReq->poiMyFavorite.officeNoorY;
        if(pReq->poiMyFavorite.homeCustName != NULL || pReq->poiMyFavorite.officePoiId != 0)
            element["officePoiId"] = pReq->poiMyFavorite.officePoiId;
        if(pReq->poiMyFavorite.homeCustName != NULL || pReq->poiMyFavorite.officeNavSeq != 0)
            element["officeNavSeq"] = pReq->poiMyFavorite.officeNavSeq;
        if(pReq->poiMyFavorite.homeCustName != NULL || pReq->poiMyFavorite.officeRpFlag != 0)
            element["officeRpFlag"] = pReq->poiMyFavorite.officeRpFlag;
        if(pReq->poiMyFavorite.homeCustName != NULL || pReq->poiMyFavorite.officeAddInfo != 0)
            element["officeAddInfo"] = pReq->poiMyFavorite.officeAddInfo;

        root["poiMyFavorite"] = element;

        NDDSLIB_Json::Value Favorites;
        for(int i=0 ; i<pReq->nPoiFavorites ; i++)
        {
            Favorites.clear();

            Favorites["custName"] = pReq->poiFavorites[i].custName;
            Favorites["noorX"] = pReq->poiFavorites[i].noorX;
            Favorites["noorY"] = pReq->poiFavorites[i].noorY;
            Favorites["poiId"] = pReq->poiFavorites[i].poiId;
            Favorites["rpFlag"] = pReq->poiFavorites[i].rpFlag;
            Favorites["addInfo"] = pReq->poiFavorites[i].addInfo;
            Favorites["insDatetime"] = pReq->poiFavorites[i].insDatetime;
            Favorites["navSeq"] = pReq->poiFavorites[i].navSeq;

            root["poiFavorites"].append(Favorites);
        }
    }catch (...)
    {
        return false;
    }


    return true;
}

//5.3.15. User-Profile 저장
bool CNddsLibTmap::MakeRegistUserProfile(NDDSLIB_Json::Value &root, NDDSTMAP_REGISTUSERPROFILE_REQ *pReq)
{
    NDDSLIB_Json::Value element;

    try
    {
        element.clear();

        element["Email"] = pReq->userlInfo.email;
        element["username"] = pReq->userlInfo.userName;
        element["Mdn"] = pReq->userlInfo.mdn;
        element["Birth"] = pReq->userlInfo.birth;

        root["personalInfo"].append(element);
    }catch (...)
    {
        return false;
    }

    return true;
}

//5.3.18. Car-Profile 저장
bool CNddsLibTmap::MakeRegistCarProfile(NDDSLIB_Json::Value &root, NDDSTMAP_REGISTCARPROFILE_REQ *pReq)
{
    NDDSLIB_Json::Value element;

    try
    {
        element.clear();

        element["model"] = pReq->carlInfo.model;
        element["fuel"] = pReq->carlInfo.fuel;
        element["hipassYn"] = pReq->carlInfo.hipassYn;
        element["number"] = pReq->carlInfo.number;

        root["carlInfo"].append(element);
    }catch (...)
    {
        return false;
    }

    return true;
}

//5.3.5 POI 즐겨찾기 단건 수정
bool CNddsLibTmap::MakeModifyPoiFavorite(NDDSLIB_Json::Value &root, NDDSTMAP_MODIFYPOIFAVORITE_REQ *pReq)
{
    NDDSLIB_Json::Value element;

    try
    {
        element.clear();

        element["custName"] = pReq->poiFavorites.custName;
        element["noorX"] = pReq->poiFavorites.noorX;
        element["noorY"] = pReq->poiFavorites.noorY;
        element["newCustName"] = pReq->poiFavorites.newCustName;
        element["iconInfo"] = pReq->poiFavorites.iconInfo;

        root["poiFavorites"].append(element);
    }catch (...)
    {
        return false;
    }

    return true;
}

// 5.3.4. POI 즐겨찾기 멀티 삭제
bool CNddsLibTmap::MakeRemovePoiFavorite(NDDSLIB_Json::Value &root, NDDSTMAP_REMOVEPOIFAVORITE_REQ *pReq)
{
    NDDSLIB_Json::Value element;

    try
    {
        for(int i=0 ; i<pReq->nPoiFavorites ; i++)
        {
            element.clear();

            element["custName"] = pReq->poiFavorites[i].custName;
            element["noorX"] = pReq->poiFavorites[i].noorX;
            element["noorY"] = pReq->poiFavorites[i].noorY;

            root["poiFavorites"].append(element);
        }
    }catch (...)
    {
        return false;
    }

    return true;
}

bool CNddsLibTmap::MakeFindPoiFavorite(NDDSLIB_Json::Value &root, NDDSTMAP_FINDPOIFAVORITE_REQ *pReq)
{
    try
    {
        root["myFavoriteYn"] = pReq->myFavoriteYn;
    }catch (...)
    {
        return false;
    }

    return true;
}


bool CNddsLibTmap::ParserFindChargeNotices(NDDSLIB_Json::Value &root, NDDS_MEMORYSTATUS &memory, NDDSTMAP_FINDCHARGENOTICES_RES **ppRes)
{
    CNddsLibCommonData &CommonData = CNddsLibCommonData::GetInstance();
    bool bConverting = CommonData.m_eCharacterSet == NDDSCHARACTERSET_EUCKR?true:false;

    NDDSTMAP_FINDCHARGENOTICES_RES res;
    NDDSLIB_Json::Value element;

    memset(&res, 0x00, sizeof(NDDSTMAP_FINDCHARGENOTICES_RES));

    try{
        JSON_ALLOC_STRING(root, "noticeVersion", res.noticeVersion, memory);
        res.contentsCount = root["contentsCount"].asInt();

        if(res.contentsCount > 0)
        {
            element = root["contentsDetails"];

            JSON_ALLOC_BLOCK(res.contentsDetails, NDDSTMAP_CONTENTSDETAILS_DTO*, sizeof(NDDSTMAP_CONTENTSDETAILS_DTO) * res.contentsCount, memory);
            for(int i = 0; i < res.contentsCount ; i++)
            {
                JSON_ALLOC_MULITBYTE_STRING(bConverting, element[i], "regDate", res.contentsDetails[i].regDate, memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, element[i], "title", res.contentsDetails[i].title, memory);
                JSON_ALLOC_STRING(element[i], "content", res.contentsDetails[i].content, memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, element[i], "url", res.contentsDetails[i].url, memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, element[i], "email", res.contentsDetails[i].email, memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, element[i], "tel", res.contentsDetails[i].tel, memory);
            }
        }
    }catch (...)
    {
        return false;
    }

    *ppRes = (NDDSTMAP_FINDCHARGENOTICES_RES*)malloc(sizeof(NDDSTMAP_FINDCHARGENOTICES_RES));
    memcpy(*ppRes, &res, sizeof(NDDSTMAP_FINDCHARGENOTICES_RES));

    return true;
}

NDDSTMAP_POISEARCHES_DTO* CNddsLibTmap::ParserPois(NDDSLIB_Json::Value element,NDDS_MEMORYSTATUS &memory,int listCnt)
{
    CNddsLibCommonData &CommonData = CNddsLibCommonData::GetInstance();
    bool bConverting = CommonData.m_eCharacterSet == NDDSCHARACTERSET_EUCKR?true:false;

    NDDSTMAP_POISEARCHES_DTO* res;
    JSON_ALLOC_BLOCK(res, NDDSTMAP_POISEARCHES_DTO*, sizeof(NDDSTMAP_POISEARCHES_DTO) * listCnt, memory);
    try
    {
        for(int i = 0; i < listCnt ; i++)
        {
            JSON_ALLOC_MULITBYTE_STRING(bConverting, element[i], "dbkind", res[i].dbkind, memory);
            JSON_ALLOC_MULITBYTE_STRING(bConverting, element[i], "poiId", res[i].poiId, memory);
            JSON_ALLOC_MULITBYTE_STRING(bConverting, element[i], "name", res[i].name, memory);
            JSON_ALLOC_MULITBYTE_STRING(bConverting, element[i], "orgName", res[i].orgName, memory);
            JSON_ALLOC_MULITBYTE_STRING(bConverting, element[i], "navX1", res[i].navX1, memory);
            JSON_ALLOC_MULITBYTE_STRING(bConverting, element[i], "navY1", res[i].navY1, memory);
            JSON_ALLOC_MULITBYTE_STRING(bConverting, element[i], "centerX", res[i].centerX, memory);
            JSON_ALLOC_MULITBYTE_STRING(bConverting, element[i], "centerY", res[i].centerY, memory);
            res[i].rpFlag = element[i]["rpFlag"].asUInt();
            JSON_ALLOC_MULITBYTE_STRING(bConverting, element[i], "parkYn", res[i].parkYn, memory);
            JSON_ALLOC_MULITBYTE_STRING(bConverting, element[i], "phoneNum", res[i].phoneNum, memory);
            JSON_ALLOC_MULITBYTE_STRING(bConverting, element[i], "addr", res[i].addr, memory);
            JSON_ALLOC_MULITBYTE_STRING(bConverting, element[i], "primary", res[i].primary, memory);
            JSON_ALLOC_MULITBYTE_STRING(bConverting, element[i], "secondary", res[i].secondary, memory);
            JSON_ALLOC_MULITBYTE_STRING(bConverting, element[i], "mlClass", res[i].mlClass, memory);
            JSON_ALLOC_MULITBYTE_STRING(bConverting, element[i], "lcdName", res[i].lcdName, memory);
            JSON_ALLOC_MULITBYTE_STRING(bConverting, element[i], "mcdName", res[i].mcdName, memory);
            JSON_ALLOC_MULITBYTE_STRING(bConverting, element[i], "scdName", res[i].scdName, memory);

            JSON_ALLOC_MULITBYTE_STRING(bConverting, element[i], "dcdName", res[i].dcdName, memory);
            JSON_ALLOC_MULITBYTE_STRING(bConverting, element[i], "dataKind", res[i].dataKind, memory);
            JSON_ALLOC_MULITBYTE_STRING(bConverting, element[i], "stId", res[i].stId, memory);
            JSON_ALLOC_MULITBYTE_STRING(bConverting, element[i], "highHhSale", res[i].highHhSale, memory);
            JSON_ALLOC_MULITBYTE_STRING(bConverting, element[i], "minOilYn", res[i].minOilYn, memory);
            JSON_ALLOC_MULITBYTE_STRING(bConverting, element[i], "oilBaseSdt", res[i].oilBaseSdt, memory);
            res[i].hhPrice = (long) element[i]["hhPrice"].asDouble();
            res[i].ggPrice = (long) element[i]["ggPrice"].asDouble();
            res[i].llPrice = (long) element[i]["llPrice"].asDouble();
            res[i].highHhPrice = (long) element[i]["highHhPrice"].asDouble();
            res[i].highGgPrice = (long) element[i]["highGgPrice"].asDouble();
            JSON_ALLOC_MULITBYTE_STRING(bConverting, element[i], "radius", res[i].radius, memory);
            JSON_ALLOC_MULITBYTE_STRING(bConverting, element[i], "roadName", res[i].roadName, memory);
            JSON_ALLOC_MULITBYTE_STRING(bConverting, element[i], "bldNo1", res[i].bldNo1, memory);
            JSON_ALLOC_MULITBYTE_STRING(bConverting, element[i], "bldNo2", res[i].bldNo2, memory);
            res[i].publicTrafficType = element[i]["publicTrafficType"].asUInt();
            JSON_ALLOC_MULITBYTE_STRING(bConverting, element[i], "sid", res[i].sid, memory);
            JSON_ALLOC_MULITBYTE_STRING(bConverting, element[i], "navSeq", res[i].navSeq, memory);
        }

    }catch (...)
    {
        return NULL;
    }


    return res;
}


bool CNddsLibTmap::ParserFindRoute(NDDSLIB_Json::Value &root, NDDS_MEMORYSTATUS &memory, NDDSTMAP_FINDROUTE_RES **ppRes)
{
    CNddsLibCommonData &CommonData = CNddsLibCommonData::GetInstance();
    bool bConverting = CommonData.m_eCharacterSet == NDDSCHARACTERSET_EUCKR?true:false;

    NDDSTMAP_FINDROUTE_RES res;
    NDDSLIB_Json::Value element;

    memset(&res, 0x00, sizeof(NDDSTMAP_FINDROUTE_RES));

    try
    {
        element = root["routeMyFavoriteViewDto"];
        {
            NDDSLIB_Json::Value RoutePos;

            JSON_ALLOC_MULITBYTE_STRING(bConverting, element, "routeTitle", res.routeMyFavoriteViewDto.routeTitle, memory);

            RoutePos = element["startRouteDto"];
            JSON_ALLOC_MULITBYTE_STRING(bConverting, RoutePos, "name", res.routeMyFavoriteViewDto.startRouteDto.name, memory);
            JSON_ALLOC_MULITBYTE_STRING(bConverting, RoutePos, "poiId", res.routeMyFavoriteViewDto.startRouteDto.poiId, memory);
            JSON_ALLOC_MULITBYTE_STRING(bConverting, RoutePos, "navX", res.routeMyFavoriteViewDto.startRouteDto.navX, memory);
            JSON_ALLOC_MULITBYTE_STRING(bConverting, RoutePos, "navY", res.routeMyFavoriteViewDto.startRouteDto.navY, memory);
            res.routeMyFavoriteViewDto.startRouteDto.rpFlag = RoutePos["rpFlag"].asUInt();

            RoutePos = element["endRouteDto"];
            JSON_ALLOC_MULITBYTE_STRING(bConverting, RoutePos, "name", res.routeMyFavoriteViewDto.endRouteDto.name, memory);
            JSON_ALLOC_MULITBYTE_STRING(bConverting, RoutePos, "poiId", res.routeMyFavoriteViewDto.endRouteDto.poiId, memory);
            JSON_ALLOC_MULITBYTE_STRING(bConverting, RoutePos, "navX", res.routeMyFavoriteViewDto.endRouteDto.navX, memory);
            JSON_ALLOC_MULITBYTE_STRING(bConverting, RoutePos, "navY", res.routeMyFavoriteViewDto.endRouteDto.navY, memory);
            res.routeMyFavoriteViewDto.endRouteDto.rpFlag = RoutePos["rpFlag"].asUInt();

            JSON_ALLOC_BLOCK(res.routeMyFavoriteViewDto.midRouteDtos, NDDSTMAP_ROUTEPOS_DTO*, sizeof(NDDSTMAP_ROUTEPOS_DTO) * 3, memory);

            NDDSLIB_Json::Value MidRoute = element["midRouteDto1"];
            JSON_ALLOC_MULITBYTE_STRING(bConverting, MidRoute, "name", res.routeMyFavoriteViewDto.midRouteDtos[0].name, memory);
            JSON_ALLOC_MULITBYTE_STRING(bConverting, MidRoute, "poiId", res.routeMyFavoriteViewDto.midRouteDtos[0].poiId, memory);
            JSON_ALLOC_MULITBYTE_STRING(bConverting, MidRoute, "navX", res.routeMyFavoriteViewDto.midRouteDtos[0].navX, memory);
            JSON_ALLOC_MULITBYTE_STRING(bConverting, MidRoute, "navY", res.routeMyFavoriteViewDto.midRouteDtos[0].navY, memory);
            res.routeMyFavoriteViewDto.midRouteDtos[0].rpFlag = MidRoute["rpFlag"].asUInt();

            MidRoute.clear();
            MidRoute = element["midRouteDto2"];
            JSON_ALLOC_MULITBYTE_STRING(bConverting, MidRoute, "name", res.routeMyFavoriteViewDto.midRouteDtos[1].name, memory);
            JSON_ALLOC_MULITBYTE_STRING(bConverting, MidRoute, "poiId", res.routeMyFavoriteViewDto.midRouteDtos[1].poiId, memory);
            JSON_ALLOC_MULITBYTE_STRING(bConverting, MidRoute, "navX", res.routeMyFavoriteViewDto.midRouteDtos[1].navX, memory);
            JSON_ALLOC_MULITBYTE_STRING(bConverting, MidRoute, "navY", res.routeMyFavoriteViewDto.midRouteDtos[1].navY, memory);
            res.routeMyFavoriteViewDto.midRouteDtos[1].rpFlag = MidRoute["rpFlag"].asUInt();

            MidRoute.clear();
            MidRoute = element["midRouteDto3"];
            JSON_ALLOC_MULITBYTE_STRING(bConverting, MidRoute, "name", res.routeMyFavoriteViewDto.midRouteDtos[2].name, memory);
            JSON_ALLOC_MULITBYTE_STRING(bConverting, MidRoute, "poiId", res.routeMyFavoriteViewDto.midRouteDtos[2].poiId, memory);
            JSON_ALLOC_MULITBYTE_STRING(bConverting, MidRoute, "navX", res.routeMyFavoriteViewDto.midRouteDtos[2].navX, memory);
            JSON_ALLOC_MULITBYTE_STRING(bConverting, MidRoute, "navY", res.routeMyFavoriteViewDto.midRouteDtos[2].navY, memory);
            res.routeMyFavoriteViewDto.midRouteDtos[2].rpFlag = MidRoute["rpFlag"].asUInt();
        }

        element.clear();

        element = root["routeFavoriteViewDtos"];
        res.nRouteFavoriteViewDtos = element.size();
        if(res.nRouteFavoriteViewDtos > 0)
        {
            NDDSLIB_Json::Value RoutePos;

            JSON_ALLOC_BLOCK(res.routeFavoriteViewDtos, NDDSTMAP_ROUTEFAVORITE_DTO*, sizeof(NDDSTMAP_ROUTEFAVORITE_DTO) * res.nRouteFavoriteViewDtos, memory);
            for(int i = 0; i < res.nRouteFavoriteViewDtos ; i++)
            {
                JSON_ALLOC_MULITBYTE_STRING(bConverting, element[i], "routeTitle", res.routeFavoriteViewDtos[i].routeTitle, memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, element[i], "insDatetime", res.routeFavoriteViewDtos[i].insDatetime, memory);

                RoutePos = element[i]["startRouteDto"];
                JSON_ALLOC_MULITBYTE_STRING(bConverting, RoutePos, "name", res.routeFavoriteViewDtos[i].startRouteDto.name, memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, RoutePos, "poiId", res.routeFavoriteViewDtos[i].startRouteDto.poiId, memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, RoutePos, "navX", res.routeFavoriteViewDtos[i].startRouteDto.navX, memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, RoutePos, "navY", res.routeFavoriteViewDtos[i].startRouteDto.navY, memory);
                res.routeFavoriteViewDtos[i].startRouteDto.rpFlag = RoutePos["rpFlag"].asUInt();

                RoutePos = element[i]["endRouteDto"];
                JSON_ALLOC_MULITBYTE_STRING(bConverting, RoutePos, "name", res.routeFavoriteViewDtos[i].endRouteDto.name, memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, RoutePos, "poiId", res.routeFavoriteViewDtos[i].endRouteDto.poiId, memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, RoutePos, "navX", res.routeFavoriteViewDtos[i].endRouteDto.navX, memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, RoutePos, "navY", res.routeFavoriteViewDtos[i].endRouteDto.navY, memory);
                res.routeFavoriteViewDtos[i].endRouteDto.rpFlag = RoutePos["rpFlag"].asUInt();

                NDDSLIB_Json::Value MidRoute = element[i]["midRouteDtos"];
                res.routeFavoriteViewDtos[i].nMidList = MidRoute.size();
                if(res.routeFavoriteViewDtos[i].nMidList > 0)
                {
                    JSON_ALLOC_BLOCK(res.routeFavoriteViewDtos[i].midRouteDtos, NDDSTMAP_ROUTEPOS_DTO*, sizeof(NDDSTMAP_ROUTEPOS_DTO) * res.routeFavoriteViewDtos[i].nMidList, memory);
                    for(int j = 0; j < res.routeFavoriteViewDtos[i].nMidList ; j++)
                    {
                        JSON_ALLOC_MULITBYTE_STRING(bConverting, MidRoute[j], "name", res.routeFavoriteViewDtos[i].midRouteDtos[j].name, memory);
                        JSON_ALLOC_MULITBYTE_STRING(bConverting, MidRoute[j], "poiId", res.routeFavoriteViewDtos[i].midRouteDtos[j].poiId, memory);
                        JSON_ALLOC_MULITBYTE_STRING(bConverting, MidRoute[j], "navX", res.routeFavoriteViewDtos[i].midRouteDtos[j].navX, memory);
                        JSON_ALLOC_MULITBYTE_STRING(bConverting, MidRoute[j], "navY", res.routeFavoriteViewDtos[i].midRouteDtos[j].navY, memory);
                        res.routeFavoriteViewDtos[i].midRouteDtos[j].rpFlag = MidRoute[j]["rpFlag"].asUInt();
                    }
                }
            }
        }
    }
    catch (...)
    {
        return false;
    }

    *ppRes = (NDDSTMAP_FINDROUTE_RES*)malloc(sizeof(NDDSTMAP_FINDROUTE_RES));
    memcpy(*ppRes, &res, sizeof(NDDSTMAP_FINDROUTE_RES));

    return true;
}

bool CNddsLibTmap::ParserPlanningRoute(NDDS_COMMONHEADER_RES common_header, unsigned char *orgContentData,int nContentSize,NDDS_MEMORYSTATUS &memory, NDDSTMAP_PLANNINGROUTE_RES **ppRes )
{
    CNddsLibCommonData &CommonData = CNddsLibCommonData::GetInstance();
    // 바이너리 포멧은 EUC-KR이 기본값임
    bool bConverting = CommonData.m_eCharacterSet != NDDSCHARACTERSET_EUCKR?true:false;

    NDDSTMAP_PLANNINGROUTE_RES res;
    bool bRet = false;
    unsigned char *pContent = orgContentData;
    int i=0;
    memset(&res, 0x00, sizeof(NDDSTMAP_PLANNINGROUTE_RES));
    if(atoi(common_header.errorCode) == 0)
    {
        try{
            // - 상세위치요구 응답 데이터(departureMatch)
            READ_INTEGER_DATA(pContent, res.roadCount); //tvas4.5 이상 버전에서 미사용
            READ_BYTE_DATA(pContent,res.roadType); //tvas4.5 이상 버전에서 미사용
            READ_BYTE_DATA(pContent,res.routeCount); //목적지 탐색구분 값이 경로안내종료탐색인 경우 0 리턴됨.

            // 상세 위치 확인 필요
            if(res.roadCount >= 2 && res.routeCount == 0)
            {
                *ppRes = (NDDSTMAP_PLANNINGROUTE_RES*)malloc(sizeof(NDDSTMAP_PLANNINGROUTE_RES));
                memcpy(*ppRes, &res, sizeof(NDDSTMAP_PLANNINGROUTE_RES));
                return true;
            }

            JSON_ALLOC_BLOCK(res.routePlanTypes, int*, sizeof(int)*res.routeCount, memory);
            for(i=0 ; i<res.routeCount ; i++)
            {
                READ_INTEGER_DATA(pContent, res.routePlanTypes[i]);
            }

            JSON_ALLOC_BLOCK(res.fareWeightOpt, int*, sizeof(int)*res.routeCount, memory);
            for(i=0 ; i<res.routeCount ; i++)
            {
                READ_INTEGER_DATA(pContent, res.fareWeightOpt[i]);
            }

            JSON_ALLOC_BLOCK(res.routeSummaryCode, int*, sizeof(int)*res.routeCount, memory);
            for(i=0 ; i<res.routeCount ; i++)
            {
                READ_INTEGER_DATA(pContent, res.routeSummaryCode[i]);
            }

            // - 목적지 정보 (destination)
            READ_BYTE_DATA(pContent,res.destination.compressFlag);
            ALLOC_STR_COPY(pContent,res.destination.destPoiId,NDDS_SIZE_PLANNINGROUTE_DESTPOIID,memory);
            pContent += NDDS_SIZE_PLANNINGROUTE_DESTPOIID;
            READ_BYTE_DATA(pContent,res.destination.destRpFlag);
            READ_BYTE_DATA(pContent,res.destination.departCoordType);
            READ_INTEGER_DATA(pContent,res.destination.destXPos);
            READ_INTEGER_DATA(pContent,res.destination.destYPos);
            READ_BYTE_DATA(pContent,res.destination.destNameSize);
            READ_BYTE_DATA(pContent,res.destination.tvasCount);

            if(res.destination.destNameSize > 0)
            {
                ALLOC_STR_UTF8_COPY(bConverting, (char*)pContent, res.destination.destName, res.destination.destNameSize, memory);
                pContent += res.destination.destNameSize;
            }

            READ_BYTE_DATALEN(pContent, res.destination.skyCode, 7);
            READ_BYTE_DATALEN(pContent, res.destination.skyName, 20);
            READ_BYTE_DATALEN(pContent, res.destination.rainTypeCode, 1);
            READ_BYTE_DATALEN(pContent, res.destination.rainSinceOnTime, 6);
            READ_BYTE_DATALEN(pContent, res.destination.tempC1h, 5);

            // - 경로 응답 참조 정보 (tavsRefs)
            if(res.routeCount > 0)
            {
                JSON_ALLOC_BLOCK(res.tvasSize, int*, sizeof(int)*res.routeCount, memory);
                for(i=0 ; i<res.routeCount ; i++)
                {
                    READ_INTEGER_DATA(pContent, res.tvasSize[i]);
                }

                JSON_ALLOC_BLOCK(res.tvases, unsigned char**, sizeof(unsigned char*)*res.routeCount, memory);
                for(i=0 ; i<res.routeCount ; i++)
                {
                    ALLOC_MEM_COPY(pContent, res.tvases[i], res.tvasSize[i], memory);
                    pContent += res.tvasSize[i];
                }
            }

            bRet = true;
        }
        catch (...)
        {
            return false;
        }
    }
    *ppRes = (NDDSTMAP_PLANNINGROUTE_RES*)malloc(sizeof(NDDSTMAP_PLANNINGROUTE_RES));
    memcpy(*ppRes, &res, sizeof(NDDSTMAP_PLANNINGROUTE_RES));

    return bRet;
}

bool CNddsLibTmap::ParserRouteSummaryInfo(NDDSLIB_Json::Value &root, NDDS_MEMORYSTATUS &memory, NDDSTMAP_ROUTESUMMARYINFO_RES **ppRes)
{
    CNddsLibCommonData &CommonData = CNddsLibCommonData::GetInstance();
    bool bConverting = CommonData.m_eCharacterSet == NDDSCHARACTERSET_EUCKR?true:false;

    NDDSTMAP_ROUTESUMMARYINFO_RES res;
    NDDSLIB_Json::Value routeList, summaryList;

    memset(&res, 0x00, sizeof(NDDSTMAP_ROUTESUMMARYINFO_RES));

    try
    {
        routeList = root["routeList"];

        res.nRouteList = routeList.size();
        if(res.nRouteList > 0)
        {
            JSON_ALLOC_BLOCK(res.routeList, NDDSTMAP_ROUTELIST_DTO*, sizeof(NDDSTMAP_ROUTELIST_DTO) * res.nRouteList, memory);
            for(int i = 0; i < res.nRouteList ; i++)
            {
                res.routeList[i].totalLength = routeList[i]["totalLength"].asUInt();
                res.routeList[i].totalTime = routeList[i]["totalTime"].asUInt();
                res.routeList[i].totalTollgateCost = routeList[i]["totalTollgateCost"].asUInt();
                res.routeList[i].totalTaxiCost = routeList[i]["totalTaxiCost"].asUInt();
                res.routeList[i].estimationOption = routeList[i]["estimationOption"].asUInt();
                JSON_ALLOC_MULITBYTE_STRING(bConverting, routeList[i], "estimationTime", res.routeList[i].estimationTime, memory);
                res.routeList[i].departRoadType = routeList[i]["departRoadType"].asUInt();
                JSON_ALLOC_MULITBYTE_STRING(bConverting, routeList[i], "mainRoad", res.routeList[i].mainRoad, memory);
                res.routeList[i].vertexFlag = routeList[i]["vertexFlag"].asUInt();
                summaryList = routeList[i]["summaryList"];
                res.routeList[i].nSummaryList = summaryList.size();
                if(res.routeList[i].nSummaryList > 0)
                {
                    JSON_ALLOC_BLOCK(res.routeList->summaryList, NDDSTMAP_SUMMARYLIST_DTO*, sizeof(NDDSTMAP_SUMMARYLIST_DTO) * res.routeList[i].nSummaryList, memory);
                    for(int j = 0; j < res.routeList[i].nSummaryList ; j++)
                    {
                        res.routeList[i].summaryList[j].guideType = summaryList[j]["guideType"].asUInt();
                        JSON_ALLOC_MULITBYTE_STRING(bConverting, summaryList[j], "summaryName", res.routeList[i].summaryList[j].summaryName, memory);
                        res.routeList[i].summaryList[j].recmdRouteLen = summaryList[j]["recmdRouteLen"].asUInt();
                        res.routeList[i].summaryList[j].recmdRouteTime = summaryList[j]["recmdRouteTime"].asUInt();
                        res.routeList[i].summaryList[j].speed = summaryList[j]["speed"].asUInt();
                        res.routeList[i].summaryList[j].congestion = summaryList[j]["congestion"].asUInt();
                    }
                }
                // 문제가 있어서 주석처리
                //JSON_ALLOC_MULITBYTE_STRING(bConverting, routeList[i], "vertexCoord", res.routeList[i].vertexCoord, memory);
            }
        }
    }
    catch (...)
    {
        return false;
    }

    *ppRes = (NDDSTMAP_ROUTESUMMARYINFO_RES*)malloc(sizeof(NDDSTMAP_ROUTESUMMARYINFO_RES));
    memcpy(*ppRes, &res, sizeof(NDDSTMAP_ROUTESUMMARYINFO_RES));

    return true;
}

bool CNddsLibTmap::ParserFindPois(NDDSLIB_Json::Value &root, NDDS_MEMORYSTATUS &memory, NDDSTMAP_FINDPOIS_RES **ppRes)
{
    CNddsLibCommonData &CommonData = CNddsLibCommonData::GetInstance();
    bool bConverting = CommonData.m_eCharacterSet == NDDSCHARACTERSET_EUCKR?true:false;

    NDDSTMAP_FINDPOIS_RES res;
    NDDSLIB_Json::Value element;

    memset(&res, 0x00, sizeof(NDDSTMAP_FINDPOIS_RES));

    try
    {
        res.totalCnt = root["totalCnt"].asInt();
        res.areaCnt = root["areaCnt"].asInt();
        res.listCnt = root["listCnt"].asInt();
        JSON_ALLOC_MULITBYTE_STRING(bConverting, root, "contFlag", res.countFlag, memory);
        JSON_ALLOC_MULITBYTE_STRING(bConverting, root, "areaNm", res.areaNm, memory);
        element = root["poiSearches"];

        if(res.listCnt > 0)
            res.poiSearches = ParserPois(element,memory,res.listCnt);
    }
    catch (...)
    {
        return false;
    }

    *ppRes = (NDDSTMAP_FINDPOIS_RES*)malloc(sizeof(NDDSTMAP_FINDPOIS_RES));
    memcpy(*ppRes, &res, sizeof(NDDSTMAP_FINDPOIS_RES));

    return true;
}

bool CNddsLibTmap::ParserFindAroundPois(NDDSLIB_Json::Value &root,NDDS_MEMORYSTATUS &memory, NDDSTMAP_FINDAROUNDPOIS_RES **ppRes)
{
    CNddsLibCommonData &CommonData = CNddsLibCommonData::GetInstance();
    bool bConverting = CommonData.m_eCharacterSet == NDDSCHARACTERSET_EUCKR?true:false;

    NDDSTMAP_FINDAROUNDPOIS_RES res;
    NDDSLIB_Json::Value element;

    memset(&res, 0x00, sizeof(NDDSTMAP_FINDAROUNDPOIS_RES));

    try
    {
        res.totalCnt = root["totalCnt"].asInt();
        res.listCnt = root["listCnt"].asInt();
        JSON_ALLOC_MULITBYTE_STRING(bConverting, root, "countFlag", res.countFlag, memory);
        JSON_ALLOC_MULITBYTE_STRING(bConverting, root, "areaNm", res.areaNm, memory);
        element = root["poiSearches"];

        if(res.listCnt > 0)
            res.poiSearches = ParserPois(element,memory,res.listCnt);
    }
    catch (...)
    {
        return false;
    }

    *ppRes = (NDDSTMAP_FINDAROUNDPOIS_RES*)malloc(sizeof(NDDSTMAP_FINDAROUNDPOIS_RES));
    memcpy(*ppRes, &res, sizeof(NDDSTMAP_FINDAROUNDPOIS_RES));

    return true;
}

bool CNddsLibTmap::ParserRegistPoiFavoriteMulti(NDDSLIB_Json::Value &root, NDDS_MEMORYSTATUS &memory, NDDSTMAP_REGISTPOIFAVORITEMULTI_RES **ppRes)
{
    NDDSTMAP_REGISTPOIFAVORITEMULTI_RES res;

    memset(&res, 0x00, sizeof(NDDSTMAP_REGISTPOIFAVORITEMULTI_RES));

    *ppRes = (NDDSTMAP_REGISTPOIFAVORITEMULTI_RES*)malloc(sizeof(NDDSTMAP_REGISTPOIFAVORITEMULTI_RES));
    memcpy(*ppRes, &res, sizeof(NDDSTMAP_REGISTPOIFAVORITEMULTI_RES));

    return true;
}

bool CNddsLibTmap::ParserRegistAllPoiFavorite(NDDSLIB_Json::Value &root, NDDS_MEMORYSTATUS &memory, NDDSTMAP_REGISTALLPOIFAVORITE_RES **ppRes)
{
    NDDSTMAP_REGISTALLPOIFAVORITE_RES res;

    memset(&res, 0x00, sizeof(NDDSTMAP_REGISTALLPOIFAVORITE_RES));

    *ppRes = (NDDSTMAP_REGISTALLPOIFAVORITE_RES*)malloc(sizeof(NDDSTMAP_REGISTALLPOIFAVORITE_RES));
    memcpy(*ppRes, &res, sizeof(NDDSTMAP_REGISTALLPOIFAVORITE_RES));

    return true;
}

bool CNddsLibTmap::ParserRemovePoiFavorites(NDDSLIB_Json::Value &root, NDDS_MEMORYSTATUS &memory, NDDSTMAP_REMOVEPOIFAVORITE_RES **ppRes)
{
    NDDSTMAP_REMOVEPOIFAVORITE_RES res;

    memset(&res, 0x00, sizeof(NDDSTMAP_REMOVEPOIFAVORITE_RES));

    *ppRes = (NDDSTMAP_REMOVEPOIFAVORITE_RES*)malloc(sizeof(NDDSTMAP_REMOVEPOIFAVORITE_RES));
    memcpy(*ppRes, &res, sizeof(NDDSTMAP_REMOVEPOIFAVORITE_RES));

    return true;
}


bool CNddsLibTmap::ParserModifyPoiFavorite(NDDSLIB_Json::Value &root, NDDS_MEMORYSTATUS &memory, NDDSTMAP_MODIFYPOIFAVORITE_RES **ppRes)
{
    NDDSTMAP_MODIFYPOIFAVORITE_RES res;

    memset(&res, 0x00, sizeof(NDDSTMAP_MODIFYPOIFAVORITE_RES));

    *ppRes = (NDDSTMAP_MODIFYPOIFAVORITE_RES*)malloc(sizeof(NDDSTMAP_MODIFYPOIFAVORITE_RES));
    memcpy(*ppRes, &res, sizeof(NDDSTMAP_MODIFYPOIFAVORITE_RES));

    return true;
}

bool CNddsLibTmap::ParserFindPoiFavorite(NDDSLIB_Json::Value &root, NDDS_MEMORYSTATUS &memory, NDDSTMAP_FINDPOIFAVORITE_RES **ppRes)
{
    CNddsLibCommonData &CommonData = CNddsLibCommonData::GetInstance();
    bool bConverting = CommonData.m_eCharacterSet == NDDSCHARACTERSET_EUCKR?true:false;

    NDDSTMAP_FINDPOIFAVORITE_RES res;
    NDDSLIB_Json::Value element;

    memset(&res, 0x00, sizeof(NDDSTMAP_FINDPOIFAVORITE_RES));

    try
    {
        element = root["poiMyFavorite"];
        if(element.size() > 0)
        {
            JSON_ALLOC_MULITBYTE_STRING(bConverting, element, "homeCustName", res.poiMyFavorite.homeCustName, memory);
            JSON_ALLOC_MULITBYTE_STRING(bConverting, element, "homeNoorX", res.poiMyFavorite.homeNoorX, memory);
            JSON_ALLOC_MULITBYTE_STRING(bConverting, element, "homeNoorY", res.poiMyFavorite.homeNoorY, memory);
            JSON_ALLOC_MULITBYTE_STRING(bConverting, element, "homePoiId", res.poiMyFavorite.homePoiId, memory);
            JSON_ALLOC_MULITBYTE_STRING(bConverting, element, "homeNavSeq", res.poiMyFavorite.homeNavSeq, memory);
            res.poiMyFavorite.homeRpFlag = element["homeRpFlag"].asUInt();
            JSON_ALLOC_MULITBYTE_STRING(bConverting, element, "officeAddInfo", res.poiMyFavorite.homeAddInfo, memory);

            JSON_ALLOC_MULITBYTE_STRING(bConverting, element, "officeCustName", res.poiMyFavorite.officeCustName, memory);
            JSON_ALLOC_MULITBYTE_STRING(bConverting, element, "officeNoorX", res.poiMyFavorite.officeNoorX, memory);
            JSON_ALLOC_MULITBYTE_STRING(bConverting, element, "officeNoorY", res.poiMyFavorite.officeNoorY, memory);
            JSON_ALLOC_MULITBYTE_STRING(bConverting, element, "officePoiId", res.poiMyFavorite.officePoiId, memory);
            JSON_ALLOC_MULITBYTE_STRING(bConverting, element, "officeNavSeq", res.poiMyFavorite.officeNavSeq, memory);
            res.poiMyFavorite.officeRpFlag = element["officeeRpFlag"].asUInt();
            JSON_ALLOC_MULITBYTE_STRING(bConverting, element, "officeAddInfo", res.poiMyFavorite.officeAddInfo, memory);
        }


        element.clear();

        element = root["poiFavorites"];
        res.nPoiFavorites = element.size();

        if(res.nPoiFavorites > 0)
        {
            JSON_ALLOC_BLOCK(res.poiFavorites, NDDSTMAP_POIFAVORITES_DTO*, sizeof(NDDSTMAP_POIFAVORITES_DTO) * res.nPoiFavorites, memory);
            for(int i = 0; i < res.nPoiFavorites ; i++)
            {
                JSON_ALLOC_MULITBYTE_STRING(bConverting, element[i], "custName", res.poiFavorites[i].custName, memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, element[i], "noorX", res.poiFavorites[i].noorX, memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, element[i], "noorY", res.poiFavorites[i].noorY, memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, element[i], "poiId", res.poiFavorites[i].poiId, memory);
                res.poiFavorites[i].rpFlag = element[i]["rpFlag"].asUInt();
                JSON_ALLOC_MULITBYTE_STRING(bConverting, element[i], "addInfo", res.poiFavorites[i].addInfo, memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, element[i], "insDatetime", res.poiFavorites[i].insDatetime, memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, element[i], "navSeq", res.poiFavorites[i].navSeq, memory);
            }
        }
    }
    catch (...)
    {
        return false;
    }

    *ppRes = (NDDSTMAP_FINDPOIFAVORITE_RES*)malloc(sizeof(NDDSTMAP_FINDPOIFAVORITE_RES));
    memcpy(*ppRes, &res, sizeof(NDDSTMAP_FINDPOIFAVORITE_RES));

    return true;
}


bool CNddsLibTmap::ParserFindUserProfile(NDDSLIB_Json::Value &root, NDDS_MEMORYSTATUS &memory, NDDSTMAP_FINDUSERPROFILE_RES **ppRes)
{
    NDDSTMAP_FINDUSERPROFILE_RES res;

    memset(&res, 0x00, sizeof(NDDSTMAP_FINDUSERPROFILE_RES));

    try
    {
        JSON_ALLOC_STRING(root, "Email", res.userlInfo.email, memory);
        JSON_ALLOC_STRING(root, "username", res.userlInfo.userName, memory);
        JSON_ALLOC_STRING(root, "Mdn", res.userlInfo.mdn, memory);
        JSON_ALLOC_STRING(root, "Birth", res.userlInfo.birth, memory);
    }
    catch (...)
    {
        return false;
    }

    *ppRes = (NDDSTMAP_FINDUSERPROFILE_RES*)malloc(sizeof(NDDSTMAP_FINDUSERPROFILE_RES));
    memcpy(*ppRes, &res, sizeof(NDDSTMAP_FINDUSERPROFILE_RES));

    return true;
}

bool CNddsLibTmap::ParserRegistUserProfile(NDDSLIB_Json::Value &root, NDDS_MEMORYSTATUS &memory, NDDSTMAP_REGISTUSERPROFILE_RES **ppRes)
{
    NDDSTMAP_REGISTUSERPROFILE_RES res;

    memset(&res, 0x00, sizeof(NDDSTMAP_REGISTUSERPROFILE_RES));

    try
    {
    }
    catch (...)
    {
        return false;
    }

    *ppRes = (NDDSTMAP_REGISTUSERPROFILE_RES*)malloc(sizeof(NDDSTMAP_REGISTUSERPROFILE_RES));
    memcpy(*ppRes, &res, sizeof(NDDSTMAP_REGISTUSERPROFILE_RES));

    return true;
}


bool CNddsLibTmap::ParserFindCarProfile(NDDSLIB_Json::Value &root, NDDS_MEMORYSTATUS &memory, NDDSTMAP_FINDCARPROFILE_RES **ppRes)
{
    NDDSTMAP_FINDCARPROFILE_RES res;

    memset(&res, 0x00, sizeof(NDDSTMAP_FINDCARPROFILE_RES));

    try
    {
    }
    catch (...)
    {
        return false;
    }

    *ppRes = (NDDSTMAP_FINDCARPROFILE_RES*)malloc(sizeof(NDDSTMAP_FINDCARPROFILE_RES));
    memcpy(*ppRes, &res, sizeof(NDDSTMAP_FINDCARPROFILE_RES));

    return true;
}

bool CNddsLibTmap::ParserRegistCarProfile(NDDSLIB_Json::Value &root, NDDS_MEMORYSTATUS &memory, NDDSTMAP_REGISTCARPROFILE_RES **ppRes)
{
    NDDSTMAP_REGISTCARPROFILE_RES res;

    memset(&res, 0x00, sizeof(NDDSTMAP_REGISTCARPROFILE_RES));

    try
    {
    }
    catch (...)
    {
        return false;
    }

    *ppRes = (NDDSTMAP_REGISTCARPROFILE_RES*)malloc(sizeof(NDDSTMAP_REGISTCARPROFILE_RES));
    memcpy(*ppRes, &res, sizeof(NDDSTMAP_REGISTCARPROFILE_RES));

    return true;
}

bool CNddsLibTmap::ParserFindFileAccess(NDDSLIB_Json::Value &root, NDDS_MEMORYSTATUS &memory, NDDSTMAP_FINDFILEACCESS_RES **ppRes)
{
    NDDSTMAP_FINDFILEACCESS_RES res;
    NDDSLIB_Json::Value element;

    memset(&res, 0x00, sizeof(NDDSTMAP_FINDFILEACCESS_RES));

    try
    {
        element = root["fileData"];
        res.nFileData = element.size();
        if(res.nFileData > 0)
        {
            JSON_ALLOC_BLOCK(res.fileDataList, NDDSTMAP_FIND_FILEDATA_DTO*, sizeof(NDDSTMAP_FIND_FILEDATA_DTO) * res.nFileData, memory);
            for(int i = 0 ; i < res.nFileData ; i++)
            {
                JSON_ALLOC_BASE64(element,i,unsigned char *,res.fileDataList[i].fileData,res.fileDataList[i].nFileSize,memory);
            }
        }
    }
    catch (...)
    {
        return false;
    }

    *ppRes = (NDDSTMAP_FINDFILEACCESS_RES*)malloc(sizeof(NDDSTMAP_FINDFILEACCESS_RES));
    memcpy(*ppRes, &res, sizeof(NDDSTMAP_FINDFILEACCESS_RES));

    return true;
}

bool CNddsLibTmap::ParserFindOpenApps(NDDSLIB_Json::Value &root, NDDS_MEMORYSTATUS &memory, NDDSTMAP_FINDOPENAPPS_RES **ppRes)
{
    CNddsLibCommonData &CommonData = CNddsLibCommonData::GetInstance();
    bool bConverting = CommonData.m_eCharacterSet == NDDSCHARACTERSET_EUCKR?true:false;

    NDDSTMAP_FINDOPENAPPS_RES res;
    NDDSLIB_Json::Value element;

    memset(&res, 0x00, sizeof(NDDSTMAP_FINDOPENAPPS_RES));

    try
    {
        JSON_ALLOC_STRING(root, "openAppVer", res.openAppVer, memory);
        res.openAppCount = root["openAppCount"].asUInt();
        element = root["openAppDetails"];

        if(element.size() > 0)
        {
            JSON_ALLOC_BLOCK(res.openAppDetails, NDDSTMAP_OPENAPPDETAILS_DTO*, sizeof(NDDSTMAP_OPENAPPDETAILS_DTO) * element.size(), memory);
            for(int i = 0;i<(int)element.size();i++)
            {
                JSON_ALLOC_MULITBYTE_STRING(bConverting, element[i], "title", res.openAppDetails[i].title, memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, element[i], "downUrl", res.openAppDetails[i].downUrl, memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, element[i], "packageName", res.openAppDetails[i].packageName, memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, element[i], "appId", res.openAppDetails[i].appId, memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, element[i], "chargeYn", res.openAppDetails[i].chargeYn, memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, element[i], "appImg", res.openAppDetails[i].appImg, memory);
            }
        }
    }
    catch (...)
    {
        return false;
    }

    *ppRes = (NDDSTMAP_FINDOPENAPPS_RES*)malloc(sizeof(NDDSTMAP_FINDOPENAPPS_RES));
    memcpy(*ppRes, &res, sizeof(NDDSTMAP_FINDOPENAPPS_RES));

    return true;
}


bool CNddsLibTmap::ParserFindSeedKeyInfo(NDDSLIB_Json::Value &root, NDDS_MEMORYSTATUS &memory, NDDSTMAP_FINDSEEDKEYINFO_RES **ppRes, NDDS_COMMONHEADER_REQ *pCommonHeader)
{
    CNddsLibCommonData &CommonData = CNddsLibCommonData::GetInstance();
    NDDSTMAP_FINDSEEDKEYINFO_RES res;
    NDDSLIB_Json::Value element;

    memset(&res, 0x00, sizeof(NDDSTMAP_FINDSEEDKEYINFO_RES));

    try
    {
        JSON_ALLOC_STRING(root,"seedVersion",res.seedVersion, memory);

        memset(CommonData.m_sTmapNddsCommonHeader.seedKeyVer, 0x00, NDDS_HEADER_APPVER_LEN);
        strcpy(CommonData.m_sTmapNddsCommonHeader.seedKeyVer, res.seedVersion);

        element = root["seedKeyInfo"];
        int seedkeyCount = element.size();
        if(seedkeyCount > 0)
        {
            JSON_ALLOC_BLOCK(res.seedKeyInfo, char**, sizeof(char*) * element.size(), memory);
            JSON_ALLOC_ARRAYSTRING(element,res.seedKeyInfo, memory);
        }

        for(int i =0 ; i < seedkeyCount ; i++)
        {
            std::string tempStr = element[i].asString();
            //CNddsBase64::NddsDecode_Base64((char *)tempStr.c_str(), CommonData.m_sTmapNddsCommonHeader.seedKeyTable[i],16);
        }
    }
    catch (...)
    {
        return false;
    }

    *ppRes = (NDDSTMAP_FINDSEEDKEYINFO_RES*)malloc(sizeof(NDDSTMAP_FINDSEEDKEYINFO_RES));
    memcpy(*ppRes, &res, sizeof(NDDSTMAP_FINDSEEDKEYINFO_RES));

    return true;

}

bool CNddsLibTmap::ParserAppLogSave(NDDSLIB_Json::Value &root, NDDS_MEMORYSTATUS &memory, NDDSTMAP_APPLOGSAVE_RES **ppRes)
{
    NDDSTMAP_APPLOGSAVE_RES res;
    NDDSLIB_Json::Value element;

    memset(&res, 0x00, sizeof(NDDSTMAP_APPLOGSAVE_RES));

    *ppRes = (NDDSTMAP_APPLOGSAVE_RES*)malloc(sizeof(NDDSTMAP_APPLOGSAVE_RES));
    memcpy(*ppRes, &res, sizeof(NDDSTMAP_APPLOGSAVE_RES));

    return true;
}

bool CNddsLibTmap::ParserRouteHistorySave(NDDSLIB_Json::Value &root, NDDS_MEMORYSTATUS &memory, NDDSTMAP_ROUTEHISTORYSAVE_RES **ppRes)
{
    NDDSTMAP_ROUTEHISTORYSAVE_RES res;
    NDDSLIB_Json::Value element;

    memset(&res, 0x00, sizeof(NDDSTMAP_ROUTEHISTORYSAVE_RES));

    *ppRes = (NDDSTMAP_ROUTEHISTORYSAVE_RES*)malloc(sizeof(NDDSTMAP_ROUTEHISTORYSAVE_RES));
    memcpy(*ppRes, &res, sizeof(NDDSTMAP_ROUTEHISTORYSAVE_RES));

    return true;
}

bool CNddsLibTmap::MakeFindPoiDetailInfos( NDDSLIB_Json::Value &root, NDDSTMAP_FINDPOIDETAILINFO_REQ *pReq )
{
    NDDSLIB_Json::Value element;
    try
    {
        root["poiId"] = pReq->poiId;
        root["navSeq"] = pReq->navSeq;
        root["sizeWidth"] = pReq->sizeWidth;
        root["sizeHeight"] = pReq->sizeHeight;
    }
    catch (...)
    {
        return false;
    }
    return true;
}

bool CNddsLibTmap::ParserFindPoiDetailInfos( NDDSLIB_Json::Value &root,NDDS_MEMORYSTATUS &memory, NDDSTMAP_FINDPOIDETAILINFO_RES **ppRes )
{
    CNddsLibCommonData &CommonData = CNddsLibCommonData::GetInstance();
    bool bConverting = CommonData.m_eCharacterSet == NDDSCHARACTERSET_EUCKR?true:false;

    NDDSTMAP_FINDPOIDETAILINFO_RES res;
    NDDSLIB_Json::Value element;

    memset(&res, 0x00, sizeof(NDDSTMAP_FINDPOIDETAILINFO_RES));

    try
    {
        JSON_ALLOC_STRING(root, "poiId", res.poiId, memory);
        JSON_ALLOC_STRING(root, "navSeq", res.navSeq, memory);
        JSON_ALLOC_STRING(root, "grpId", res.grpId, memory);
        JSON_ALLOC_STRING(root, "viewId", res.viewId, memory);
        JSON_ALLOC_MULITBYTE_STRING(bConverting, root, "name", res.name, memory);
        JSON_ALLOC_MULITBYTE_STRING(bConverting, root, "cateNm", res.cateNm, memory);
        JSON_ALLOC_MULITBYTE_STRING(bConverting, root, "addr", res.addr, memory);
        JSON_ALLOC_MULITBYTE_STRING(bConverting, root, "lcdName", res.lcdName, memory);
        JSON_ALLOC_MULITBYTE_STRING(bConverting, root, "mcdName", res.mcdName, memory);
        JSON_ALLOC_MULITBYTE_STRING(bConverting, root, "scdName", res.scdName, memory);
        JSON_ALLOC_MULITBYTE_STRING(bConverting, root, "dcdName", res.dcdName, memory);
        JSON_ALLOC_MULITBYTE_STRING(bConverting, root, "primaryBun", res.primaryBun, memory);
        JSON_ALLOC_MULITBYTE_STRING(bConverting, root, "secondaryBun", res.secondaryBun, memory);
        JSON_ALLOC_MULITBYTE_STRING(bConverting, root, "mlClass", res.mlClass, memory);
        JSON_ALLOC_MULITBYTE_STRING(bConverting, root, "bldAddr", res.bldAddr, memory);
        JSON_ALLOC_MULITBYTE_STRING(bConverting, root, "roadName", res.roadName, memory);
        JSON_ALLOC_MULITBYTE_STRING(bConverting, root, "bldNo1", res.bldNo1, memory);
        JSON_ALLOC_MULITBYTE_STRING(bConverting, root, "bldNo2", res.bldNo2, memory);
        JSON_ALLOC_STRING(root, "navX1", res.navX1, memory);
        JSON_ALLOC_STRING(root, "navY1", res.navY1, memory);
        JSON_ALLOC_STRING(root, "centerX", res.centerX, memory);
        JSON_ALLOC_STRING(root, "centerY", res.centerY, memory);
        res.rpFlag = root["rpFlag"].asInt();
        JSON_ALLOC_MULITBYTE_STRING(bConverting, root, "zipCd", res.zipCd,memory);
        JSON_ALLOC_MULITBYTE_STRING(bConverting, root, "menu1", res.menu1,memory);
        JSON_ALLOC_MULITBYTE_STRING(bConverting, root, "menu2", res.menu2, memory);
        JSON_ALLOC_MULITBYTE_STRING(bConverting, root, "menu3", res.menu3, memory);
        JSON_ALLOC_MULITBYTE_STRING(bConverting, root, "menu4", res.menu4, memory);
        JSON_ALLOC_MULITBYTE_STRING(bConverting, root, "menu5", res.menu5, memory);
        //JSON_ALLOC_STRING(root, "cateImage", res.cateImage, memory);
        JSON_ALLOC_BASE64(root, "cateImage",char*, res.cateImage,res.nCateImageSize,memory);
        JSON_ALLOC_MULITBYTE_STRING(bConverting, root, "telNo", res.telNo, memory);
        JSON_ALLOC_MULITBYTE_STRING(bConverting, root, "parkYn", res.parkYn, memory);
        JSON_ALLOC_MULITBYTE_STRING(bConverting, root, "holidayN", res.holidayN, memory);
        JSON_ALLOC_STRING(root, "http", res.http, memory);
        JSON_ALLOC_MULITBYTE_STRING(bConverting, root, "road", res.road, memory);
        JSON_ALLOC_MULITBYTE_STRING(bConverting, root, "recommPoint", res.recommPoint, memory);
        JSON_ALLOC_MULITBYTE_STRING(bConverting, root, "iconType", res.iconType, memory);
        JSON_ALLOC_MULITBYTE_STRING(bConverting, root, "joinStoreYn", res.joinStoreYn, memory);
        JSON_ALLOC_MULITBYTE_STRING(bConverting, root, "joinViewType", res.joinViewType, memory);
        JSON_ALLOC_MULITBYTE_STRING(bConverting, root, "highHhSale", res.highHhSale, memory);
        JSON_ALLOC_MULITBYTE_STRING(bConverting, root, "oilBaseSdt", res.oilBaseSdt, memory);
        res.hhPrice = (long) root["hhPrice"].asDouble();
        res.ggPrice = (long) root["ggPrice"].asDouble();
        res.llPrice = (long) root["llPrice"].asDouble();
        res.highHhPrice = (long) root["highHhPrice"].asDouble();
        res.highGgPrice = (long) root["highGgPrice"].asDouble();
        JSON_ALLOC_MULITBYTE_STRING(bConverting, root, "addInfo", res.addInfo, memory);
		JSON_ALLOC_MULITBYTE_STRING(bConverting, root, "dayOffInfo", res.dayOffInfo, memory);
		JSON_ALLOC_MULITBYTE_STRING(bConverting, root, "businessHours", res.businessHours, memory);
		JSON_ALLOC_MULITBYTE_STRING(bConverting, root, "dayOffDate", res.dayOffDate, memory);
        JSON_ALLOC_STRING(root, "infoLen", res.infoLen, memory);
        JSON_ALLOC_MULITBYTE_STRING(bConverting, root, "infomation", res.infomation, memory);

        element = root["poiImageInfos"];
        res.nPoiImageInfos = element.size();
        if(element.size() > 0)
        {
            JSON_ALLOC_BLOCK(res.poiImageInfos, NDDSTMAP_POIIMAGEINFOS_DTO*, sizeof(NDDSTMAP_POIIMAGEINFOS_DTO) * element.size(), memory);
            for(int i = 0;i<(int)element.size();i++)
            {
                res.poiImageInfos[i].seq = element[i]["seq"].asInt();
                JSON_ALLOC_STRING(element[i], "fileSrc",res.poiImageInfos[i].fileSrc, memory);
            }
        }

        element = root["poiParkInfos"];
        res.nPoiParkInfos = element.size();
        if(element.size() > 0)
        {
            JSON_ALLOC_BLOCK(res.poiParkInfos, NDDSTMAP_POIPARKINFOS_DTO*, sizeof(NDDSTMAP_POIPARKINFOS_DTO) * element.size(), memory);
            for(int i = 0;i<(int)element.size();i++)
            {
                JSON_ALLOC_STRING(element[i], "cpName",res.poiParkInfos[i].cpName, memory);
                JSON_ALLOC_STRING(element[i], "mobileLinkUrl",res.poiParkInfos[i].mobileLinkUrl, memory);
            }
        }

        element = root["advertises"];
        res.nAdvertises = element.size();
        if(element.size() > 0)
        {
            JSON_ALLOC_BLOCK(res.advertises, NDDSTMAP_ADVERTISES_DTO*, sizeof(NDDSTMAP_ADVERTISES_DTO) * element.size(), memory);
            for(int i = 0;i<(int)element.size();i++)
            {
                JSON_ALLOC_STRING(element[i], "adType",res.advertises[i].adType, memory);
                JSON_ALLOC_STRING(element[i], "adSvcId",res.advertises[i].adSvcId, memory);
                JSON_ALLOC_STRING(element[i], "adInfoType",res.advertises[i].adInfoType, memory);
                JSON_ALLOC_STRING(element[i], "adTitle",res.advertises[i].adTitle, memory);
                JSON_ALLOC_STRING(element[i], "startSdttm",res.advertises[i].startSdttm, memory);
                JSON_ALLOC_STRING(element[i], "endSdttm",res.advertises[i].endSdttm, memory);
                JSON_ALLOC_STRING(element[i], "linkUrl",res.advertises[i].linkUrl, memory);
                JSON_ALLOC_STRING(element[i], "iconImg",res.advertises[i].iconImg, memory);
            }
        }
    }
    catch (...)
    {
        return false;
    }

    *ppRes = (NDDSTMAP_FINDPOIDETAILINFO_RES*)malloc(sizeof(NDDSTMAP_FINDPOIDETAILINFO_RES));
    memcpy(*ppRes, &res, sizeof(NDDSTMAP_FINDPOIDETAILINFO_RES));


    return true;
}

bool CNddsLibTmap::ParserFindPoiJoinInfo( NDDSLIB_Json::Value &root,NDDS_MEMORYSTATUS &memory, NDDSTMAP_FINDPOIJOININFO_RES **ppRes )
{
    CNddsLibCommonData &CommonData = CNddsLibCommonData::GetInstance();
    bool bConverting = CommonData.m_eCharacterSet == NDDSCHARACTERSET_EUCKR?true:false;

    NDDSTMAP_FINDPOIJOININFO_RES res;
    NDDSLIB_Json::Value element;

    memset(&res, 0x00, sizeof(NDDSTMAP_FINDPOIJOININFO_RES));

    try{
        element = root["poiJoinInfoViews"];
        res.nPoiJoinInfoViews = element.size();
        if(element.size() > 0)
        {
            JSON_ALLOC_BLOCK(res.poiJoinInfoViews, NDDSTMAP_POIJOININFOVIEWS_DTO*, sizeof(NDDSTMAP_POIJOININFOVIEWS_DTO) * element.size(), memory);
            for(int i = 0;i<(int)element.size();i++)
            {
                JSON_ALLOC_MULITBYTE_STRING(bConverting, element[i], "poiId",		res.poiJoinInfoViews[i].poiId,memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, element[i], "joinType",	res.poiJoinInfoViews[i].joinType,memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, element[i], "poiName",	res.poiJoinInfoViews[i].poiName,memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, element[i], "cashRate",	res.poiJoinInfoViews[i].cashRate,memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, element[i], "cardRate",	res.poiJoinInfoViews[i].cardRate,memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, element[i], "workTime",	res.poiJoinInfoViews[i].workTime,memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, element[i], "weekendWorkTime",res.poiJoinInfoViews[i].weekendWorkTime, memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, element[i], "http",		res.poiJoinInfoViews[i].http,memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, element[i], "parkYn",		res.poiJoinInfoViews[i].parkYn,memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, element[i], "benefit",	res.poiJoinInfoViews[i].benefit,memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, element[i], "useGuide",	res.poiJoinInfoViews[i].useGuide,memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, element[i], "information",res.poiJoinInfoViews[i].information,memory);

                NDDSLIB_Json::Value itemInfos = element[i]["itemInfos"];
                res.poiJoinInfoViews[i].nItem = itemInfos.size();
                if(itemInfos.size() > 0)
                {
                    JSON_ALLOC_BLOCK(res.poiJoinInfoViews[i].itemInfos, char**, sizeof(char*) * element.size(), memory);
                    for(int j = 0;j < (int)itemInfos.size();j++)
                    {
                        JSON_ALLOC_ARRAYSTRING(itemInfos[i], res.poiJoinInfoViews[i].itemInfos,memory);
                    }
                }
            }
        }
    }catch(...)
    {
        return false;
    }


    *ppRes = (NDDSTMAP_FINDPOIJOININFO_RES*)malloc(sizeof(NDDSTMAP_FINDPOIJOININFO_RES));
    memcpy(*ppRes, &res, sizeof(NDDSTMAP_FINDPOIJOININFO_RES));

    return true;
}

bool CNddsLibTmap::MakeFindPoiJoinInfo( NDDSLIB_Json::Value &root, NDDSTMAP_FINDPOIJOININFO_REQ *pReq )
{
    NDDSLIB_Json::Value element;
    try
    {
        root["poiId"] = pReq->poiId;
    }
    catch (...)
    {

    }

    return true;
}

bool CNddsLibTmap::ParserFindRecentDestnation( NDDSLIB_Json::Value &root,NDDS_MEMORYSTATUS &memory, NDDSTMAP_FINDRECENTDESTNATION_RES **ppRes )
{
    CNddsLibCommonData &CommonData = CNddsLibCommonData::GetInstance();
    bool bConverting = CommonData.m_eCharacterSet == NDDSCHARACTERSET_EUCKR?true:false;

    NDDSTMAP_FINDRECENTDESTNATION_RES res;
    NDDSLIB_Json::Value element;

    memset(&res, 0x00, sizeof(NDDSTMAP_FINDRECENTDESTNATION_RES));
    try{
        element = root["poiRecents"];
        res.nPoiRecents = element.size();

        if(element.size() > 0)
        {
            JSON_ALLOC_BLOCK(res.poiRecents, NDDSTMAP_POIRECENTS_DTO*, sizeof(NDDSTMAP_POIRECENTS_DTO) * element.size(), memory);
            for(int i = 0;i<(int) element.size();i++)
            {
                JSON_ALLOC_MULITBYTE_STRING(bConverting, element[i], "custName",	res.poiRecents[i].custName,memory);
                JSON_ALLOC_STRING(element[i], "noorX",	res.poiRecents[i].noorX,memory);
                JSON_ALLOC_STRING(element[i], "noorY",	res.poiRecents[i].noorY,memory);
                JSON_ALLOC_STRING(element[i], "poiId",	res.poiRecents[i].poiId,memory);
                JSON_ALLOC_STRING(element[i], "navSeq",	res.poiRecents[i].navSeq,memory);
                res.poiRecents[i].rpFlag = element[i]["rpFlag"].asInt();
                JSON_ALLOC_MULITBYTE_STRING(bConverting, element[i], "lcdName",	res.poiRecents[i].lcdName,memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, element[i], "mcdName",	res.poiRecents[i].mcdName,memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, element[i], "scdName",	res.poiRecents[i].scdName,memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, element[i], "repClsName",	res.poiRecents[i].repClsName,memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, element[i], "clsAName",	res.poiRecents[i].clsAName,memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, element[i], "clsBName",	res.poiRecents[i].clsBName,memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, element[i], "clsCName",	res.poiRecents[i].clsCName,memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, element[i], "clsDName",	res.poiRecents[i].clsDName,memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, element[i], "telNo",	res.poiRecents[i].telNo,memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, element[i], "totalCnt",	res.poiRecents[i].totalCnt,memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, element[i], "svcDate",	res.poiRecents[i].svcDate,memory);
            }
        }
    }catch(...){
        return false;
    }


    *ppRes = (NDDSTMAP_FINDRECENTDESTNATION_RES*)malloc(sizeof(NDDSTMAP_FINDRECENTDESTNATION_RES));
    memcpy(*ppRes, &res, sizeof(NDDSTMAP_FINDRECENTDESTNATION_RES));

    return true;
}

bool CNddsLibTmap::MakeFindRecentDestnation( NDDSLIB_Json::Value &root, NDDSTMAP_FINDRECENTDESTNATION_REQ *pReq )
{
    // 필드 없음
    return true;
}

bool CNddsLibTmap::MakeRemoveRecentDestnation( NDDSLIB_Json::Value &root, NDDSTMAP_REMOVERECENTDESTNATION_REQ *pReq )
{
    NDDSLIB_Json::Value poiRecentUploads;

    try{
        for(int i =0 ; i< pReq->nRemovePoiRecentCnt;i++)
        {
            poiRecentUploads[i]["custName"] = pReq->poiRecentRemoves[i].custName;
            poiRecentUploads[i]["noorX"] = pReq->poiRecentRemoves[i].noorX;
            poiRecentUploads[i]["noorY"] = pReq->poiRecentRemoves[i].noorY;
        }
    }catch(...){
        return false;
    }

    root["poiRecentUploads"]=poiRecentUploads;
    return true;
}

bool CNddsLibTmap::ParserRemoveRecentDestnation( NDDSLIB_Json::Value &root,NDDS_MEMORYSTATUS &memory, NDDSTMAP_REMOVERECENTDESTNATION_RES **ppRes )
{
    NDDSTMAP_REMOVERECENTDESTNATION_RES res;
    NDDSLIB_Json::Value element;

    memset(&res, 0x00, sizeof(NDDSTMAP_REMOVERECENTDESTNATION_RES));

    *ppRes = (NDDSTMAP_REMOVERECENTDESTNATION_RES*)malloc(sizeof(NDDSTMAP_REMOVERECENTDESTNATION_RES));
    memcpy(*ppRes, &res, sizeof(NDDSTMAP_REMOVERECENTDESTNATION_RES));
    return true;
}

bool CNddsLibTmap::MakeUploadRecentDestnation( NDDSLIB_Json::Value &root, NDDSTMAP_UPLOADRECENTDESTNATION_REQ *pReq )
{
    NDDSLIB_Json::Value poiRecentUploads;

    try{
        for(int i =0 ; i< pReq->nPoiRecentUploads;i++)
        {
            NDDSLIB_Json::Value poiRecentUploadDtos;
            poiRecentUploadDtos["custName"] = pReq->poiRecentUploads[i].custName;
            poiRecentUploadDtos["noorX"] = pReq->poiRecentUploads[i].noorX;
            poiRecentUploadDtos["noorY"] = pReq->poiRecentUploads[i].noorY;
            if(pReq->poiRecentUploads[i].poiId[0] != 0x00)
                poiRecentUploadDtos["poiId"] = pReq->poiRecentUploads[i].poiId;
            poiRecentUploadDtos["navSeq"] = pReq->poiRecentUploads[i].navSeq;
            poiRecentUploadDtos["rpFlag"] = pReq->poiRecentUploads[i].rpFlag;
            if(pReq->poiRecentUploads[i].totalCnt[0] != 0x00)
                poiRecentUploadDtos["totalCnt"] = pReq->poiRecentUploads[i].totalCnt;
            if(pReq->poiRecentUploads[i].svcDate[0] != 0x00)
                poiRecentUploadDtos["svcDate"] = pReq->poiRecentUploads[i].svcDate;

            root["poiRecentUploads"].append(poiRecentUploadDtos);
        }

        if(pReq->svcUploadYn[0] != 0x00)
            root["svcUploadYn"] = pReq->svcUploadYn;
    }catch(...){
        return false;
    }

    return true;
}

bool CNddsLibTmap::ParserUploadRecentDestnation( NDDSLIB_Json::Value &root,NDDS_MEMORYSTATUS &memory, NDDSTMAP_UPLOADRECENTDESTNATION_RES **ppRes )
{
    NDDSTMAP_UPLOADRECENTDESTNATION_RES res;
    NDDSLIB_Json::Value element;

    memset(&res, 0x00, sizeof(NDDSTMAP_UPLOADRECENTDESTNATION_RES));

    *ppRes = (NDDSTMAP_UPLOADRECENTDESTNATION_RES*)malloc(sizeof(NDDSTMAP_UPLOADRECENTDESTNATION_RES));
    memcpy(*ppRes, &res, sizeof(NDDSTMAP_UPLOADRECENTDESTNATION_RES));
    return true;
}

bool CNddsLibTmap::MakeFindNearBy( NDDSLIB_Json::Value &root, NDDSTMAP_FINDNEARBY_REQ *pReq )
{
    try{
        root["reqCnt"] = pReq->reqCnt;
        root["reqSeq"] = pReq->reqSeq;
        root["radius"] = pReq->radius;
        root["noorX"] = pReq->noorX;
        root["noorY"] = pReq->noorY;
        root["addrDbYn"]= pReq->addrDbYn;
        root["poiDbYn"] = pReq->poiDbYn;
    }
    catch(...){
        return false;
    }

    return true;
}

bool CNddsLibTmap::ParserFindNearBy( NDDSLIB_Json::Value &root,NDDS_MEMORYSTATUS &memory, NDDSTMAP_FINDNEARBY_RES **ppRes )
{
    CNddsLibCommonData &CommonData = CNddsLibCommonData::GetInstance();
    bool bConverting = CommonData.m_eCharacterSet == NDDSCHARACTERSET_EUCKR?true:false;

    NDDSTMAP_FINDNEARBY_RES res;
    NDDSLIB_Json::Value element;

    memset(&res, 0x00, sizeof(NDDSTMAP_FINDNEARBY_RES));

    try{
        element = root["poiSearches"];
        res.listCnt = element.size();

        if(res.listCnt > 0)
        {
            JSON_ALLOC_BLOCK(res.poiSearches, NDDSTMAP_NEARBYPOI_DTO*, sizeof(NDDSTMAP_NEARBYPOI_DTO) * element.size(), memory);
            for(int i = 0;i<res.listCnt;i++)
            {
                JSON_ALLOC_STRING(element[i], "dbkind",	res.poiSearches[i].dbkind,memory);
                JSON_ALLOC_STRING(element[i], "poiId",	res.poiSearches[i].poiId,memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, element[i], "name",	res.poiSearches[i].name,memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, element[i], "orgName",	res.poiSearches[i].orgName,memory);
                JSON_ALLOC_STRING(element[i], "navX1",	res.poiSearches[i].navX1,memory);
                JSON_ALLOC_STRING(element[i], "navY1",	res.poiSearches[i].navY1,memory);
                JSON_ALLOC_STRING(element[i], "centerX",	res.poiSearches[i].centerX,memory);
                JSON_ALLOC_STRING(element[i], "centerY",	res.poiSearches[i].centerY,memory);
                res.poiSearches[i].rpFlag = element[i]["rpFlag"].asInt();
                JSON_ALLOC_STRING(element[i], "parkYn",	res.poiSearches[i].parkYn,memory);
                JSON_ALLOC_STRING(element[i], "telNo",	res.poiSearches[i].telNo,memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, element[i], "addr",	res.poiSearches[i].addr,memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, element[i], "primary",	res.poiSearches[i].primary,memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, element[i], "secondary",	res.poiSearches[i].secondary,memory);
                JSON_ALLOC_STRING(element[i], "mlClass",	res.poiSearches[i].mlClass,memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, element[i], "lcdName",	res.poiSearches[i].lcdName,memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, element[i], "mcdName",	res.poiSearches[i].mcdName,memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, element[i], "scdName",	res.poiSearches[i].scdName,memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, element[i], "dcdName",	res.poiSearches[i].dcdName,memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, element[i], "roadName",	res.poiSearches[i].roadName,memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, element[i], "bldNo1",	res.poiSearches[i].bldNo1,memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, element[i], "bldNo2",	res.poiSearches[i].bldNo2,memory);
                JSON_ALLOC_STRING(element[i], "radius",	res.poiSearches[i].radius,memory);
            }
        }
    }catch(...){
        return false;
    }

    *ppRes = (NDDSTMAP_FINDNEARBY_RES*)malloc(sizeof(NDDSTMAP_FINDNEARBY_RES));
    memcpy(*ppRes, &res, sizeof(NDDSTMAP_FINDNEARBY_RES));
    return true;
}

bool CNddsLibTmap::ParserFindPickatPopulars( NDDSLIB_Json::Value &root,NDDS_MEMORYSTATUS &memory, NDDSTMAP_FINDPICKATPOPULARS_RES **ppRes )
{
    CNddsLibCommonData &CommonData = CNddsLibCommonData::GetInstance();
    bool bConverting = CommonData.m_eCharacterSet == NDDSCHARACTERSET_EUCKR?true:false;

    NDDSTMAP_FINDPICKATPOPULARS_RES res;
    NDDSLIB_Json::Value element;

    memset(&res, 0x00, sizeof(NDDSTMAP_FINDPICKATPOPULARS_RES));

    try
    {
        element = root["poiSearches"];
        res.listCnt = element.size();

        if(res.listCnt > 0)
        {
            JSON_ALLOC_BLOCK(res.poiSearches, NDDSTMAP_PICKATPOI_DTO*, sizeof(NDDSTMAP_PICKATPOI_DTO) * element.size(), memory);
            for(int i = 0;i<res.listCnt;i++)
            {
                JSON_ALLOC_MULITBYTE_STRING(bConverting, element[i], "pickatPoiId",	res.poiSearches[i].pickatPoiId,memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, element[i], "name",	res.poiSearches[i].name,memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, element[i], "centerX",	res.poiSearches[i].centerX,memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, element[i], "centerY",	res.poiSearches[i].centerY,memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, element[i], "addr",	res.poiSearches[i].addr,memory);
                res.poiSearches[i].pickCount = element[i]["pickCount"].asInt();
                res.poiSearches[i].totalPosts = element[i]["totalPosts"].asInt();
                res.poiSearches[i].emoGoodCount = element[i]["emoGoodCount"].asInt();
                res.poiSearches[i].blogReviewCount = element[i]["blogReviewCount"].asInt();
                res.poiSearches[i].score = element[i]["score"].asInt();
                res.poiSearches[i].couponExist = element[i]["couponExist"].asBool();
            }
        }
    }
    catch (...)
    {
        return false;
    }

    *ppRes = (NDDSTMAP_FINDPICKATPOPULARS_RES*)malloc(sizeof(NDDSTMAP_FINDPICKATPOPULARS_RES));
    memcpy(*ppRes, &res, sizeof(NDDSTMAP_FINDPICKATPOPULARS_RES));
    return true;
}

bool CNddsLibTmap::ParserFindRoadName(NDDSLIB_Json::Value &root,NDDS_MEMORYSTATUS &memory, NDDSTMAP_FINDROADNAME_RES  **ppRes)
{
    CNddsLibCommonData &CommonData = CNddsLibCommonData::GetInstance();
    bool bConverting = CommonData.m_eCharacterSet == NDDSCHARACTERSET_EUCKR?true:false;

    NDDSTMAP_FINDROADNAME_RES res;
    NDDSLIB_Json::Value element;

    memset(&res, 0x00, sizeof(NDDSTMAP_FINDROADNAME_RES));

    try
    {
        res.totalCnt = root["totalCnt"].asInt();
        res.listCnt = root["listCnt"].asInt();
        JSON_ALLOC_MULITBYTE_STRING(bConverting, root, "contFlag", res.contFlag, memory);

        element = root["poiRoadNames"];
        int cnt = element.size();
        if(cnt > 0)
        {
            JSON_ALLOC_BLOCK(res.poiRoadNames, NDDSTMAP_ROADNAME_DTO*, sizeof(NDDSTMAP_ROADNAME_DTO) * cnt, memory);
            for(int i = 0;i<cnt;i++)
            {
                JSON_ALLOC_MULITBYTE_STRING(bConverting, element[i], "roadName", res.poiRoadNames[i].roadName,memory);
            }
        }
    }
    catch (...)
    {
        return false;
    }

    *ppRes = (NDDSTMAP_FINDROADNAME_RES*)malloc(sizeof(NDDSTMAP_FINDROADNAME_RES));
    memcpy(*ppRes, &res, sizeof(NDDSTMAP_FINDROADNAME_RES));
    return true;
}

bool CNddsLibTmap::ParserFindPickatPoiDetail(NDDSLIB_Json::Value &root,NDDS_MEMORYSTATUS &memory, NDDSTMAP_FINDPICKATPOIDETAIL_RES  **ppRes)
{
    CNddsLibCommonData &CommonData = CNddsLibCommonData::GetInstance();
    bool bConverting = CommonData.m_eCharacterSet == NDDSCHARACTERSET_EUCKR?true:false;

    NDDSTMAP_FINDPICKATPOIDETAIL_RES res;
    NDDSLIB_Json::Value element;

    memset(&res, 0x00, sizeof(NDDSTMAP_FINDPICKATPOIDETAIL_RES));

    try
    {
        JSON_ALLOC_MULITBYTE_STRING(bConverting, root, "poiId", res.poiId, memory);
        JSON_ALLOC_MULITBYTE_STRING(bConverting, root, "tmapId", res.tmapId, memory);
        JSON_ALLOC_MULITBYTE_STRING(bConverting, root, "poiName", res.poiName, memory);
        JSON_ALLOC_MULITBYTE_STRING(bConverting, root, "address", res.address, memory);
        JSON_ALLOC_MULITBYTE_STRING(bConverting, root, "mainImage", res.mainImage, memory);

        element = root["coordinates"];
        int cnt = element.size();

        if(cnt > 0)
        {
            JSON_ALLOC_BLOCK(res.coordinates, NDDSTMAP_COORDINATES_DTO*, sizeof(NDDSTMAP_COORDINATES_DTO) * cnt, memory);
            for(int i = 0;i<cnt;i++)
            {
                JSON_ALLOC_MULITBYTE_STRING(bConverting, element[i], "coordinates",	res.coordinates[i].coordinates,memory);
            }
        }
    }
    catch (...)
    {
        return false;
    }

    *ppRes = (NDDSTMAP_FINDPICKATPOIDETAIL_RES*)malloc(sizeof(NDDSTMAP_FINDPICKATPOIDETAIL_RES));
    memcpy(*ppRes, &res, sizeof(NDDSTMAP_FINDPICKATPOIDETAIL_RES));
    return true;
}

bool CNddsLibTmap::ParserFindAreaNamesByStep(NDDSLIB_Json::Value &root,NDDS_MEMORYSTATUS &memory, NDDSTMAP_FINDAREANAMESBYSTEP_RES  **ppRes)
{
    CNddsLibCommonData &CommonData = CNddsLibCommonData::GetInstance();
    bool bConverting = CommonData.m_eCharacterSet == NDDSCHARACTERSET_EUCKR?true:false;

    NDDSTMAP_FINDAREANAMESBYSTEP_RES res;
    NDDSLIB_Json::Value element;

    memset(&res, 0x00, sizeof(NDDSTMAP_FINDAREANAMESBYSTEP_RES));

    try
    {
        res.totalCnt = root["totalCnt"].asInt();
        res.listCnt = root["listCnt"].asInt();
        JSON_ALLOC_MULITBYTE_STRING(bConverting, root, "contFlag", res.contFlag, memory);

        element = root["poiAreaCodes"];
        int cnt = element.size();
        if(cnt > 0)
        {
            JSON_ALLOC_BLOCK(res.poiAreaCodes, NDDSTMAP_POIAREACODES_DTO*, sizeof(NDDSTMAP_POIAREACODES_DTO) * cnt, memory);
            for(int i = 0;i<cnt;i++)
            {
                JSON_ALLOC_MULITBYTE_STRING(bConverting, element[i], "areaName", res.poiAreaCodes[i].areaName,memory);
            }
        }
    }
    catch (...)
    {
        return false;
    }

    *ppRes = (NDDSTMAP_FINDAREANAMESBYSTEP_RES*)malloc(sizeof(NDDSTMAP_FINDAREANAMESBYSTEP_RES));
    memcpy(*ppRes, &res, sizeof(NDDSTMAP_FINDAREANAMESBYSTEP_RES));
    return true;
}

bool CNddsLibTmap::ParserFindPoiStationInfo(NDDSLIB_Json::Value &root,NDDS_MEMORYSTATUS &memory, NDDSTMAP_FINDPOISTATIONINFO_RES  **ppRes)
{
    CNddsLibCommonData &CommonData = CNddsLibCommonData::GetInstance();
    bool bConverting = CommonData.m_eCharacterSet == NDDSCHARACTERSET_EUCKR?true:false;

    NDDSTMAP_FINDPOISTATIONINFO_RES res;

    memset(&res, 0x00, sizeof(NDDSTMAP_FINDPOISTATIONINFO_RES));

    try
    {
        JSON_ALLOC_MULITBYTE_STRING(bConverting, root, "poiId", res.poiId, memory);
        JSON_ALLOC_MULITBYTE_STRING(bConverting, root, "name", res.name, memory);
        res.hhPrice = root["hhPrice"].asInt();
        res.ggPrice = root["ggPrice"].asInt();
        res.llPrice = root["llPrice"].asInt();
        res.highHhPrice = root["highHhPrice"].asInt();
        JSON_ALLOC_MULITBYTE_STRING(bConverting, root, "dataKind", res.dataKind, memory);
        JSON_ALLOC_MULITBYTE_STRING(bConverting, root, "reqKey", res.reqKey, memory);

    }
    catch (...)
    {
        return false;
    }

    *ppRes = (NDDSTMAP_FINDPOISTATIONINFO_RES*)malloc(sizeof(NDDSTMAP_FINDPOISTATIONINFO_RES));
    memcpy(*ppRes, &res, sizeof(NDDSTMAP_FINDPOISTATIONINFO_RES));
    return true;
}


bool CNddsLibTmap::ParserFindPoiEvStationInfo(NDDSLIB_Json::Value &root,NDDS_MEMORYSTATUS &memory, NDDSTMAP_FINDPOIEVSTATIONINFO_RES  **ppRes)
{
    CNddsLibCommonData &CommonData = CNddsLibCommonData::GetInstance();
    bool bConverting = CommonData.m_eCharacterSet == NDDSCHARACTERSET_EUCKR?true:false;

    NDDSTMAP_FINDPOIEVSTATIONINFO_RES res;
    NDDSLIB_Json::Value element;

    memset(&res, 0x00, sizeof(NDDSTMAP_FINDPOIEVSTATIONINFO_RES));

    try
    {
        JSON_ALLOC_MULITBYTE_STRING(bConverting, root, "poiId", res.poiId, memory);
        JSON_ALLOC_MULITBYTE_STRING(bConverting, root, "stationName", res.stationName, memory);
        JSON_ALLOC_MULITBYTE_STRING(bConverting, root, "addr", res.addr, memory);
        JSON_ALLOC_MULITBYTE_STRING(bConverting, root, "roadAddr", res.roadAddr, memory);
        JSON_ALLOC_MULITBYTE_STRING(bConverting, root, "addrDetail", res.addrDetail, memory);
        JSON_ALLOC_MULITBYTE_STRING(bConverting, root, "mngName", res.mngName, memory);
        JSON_ALLOC_MULITBYTE_STRING(bConverting, root, "mngId", res.mngId, memory);
        JSON_ALLOC_MULITBYTE_STRING(bConverting, root, "tel", res.tel, memory);
        JSON_ALLOC_MULITBYTE_STRING(bConverting, root, "freeYn", res.freeYn, memory);
        JSON_ALLOC_MULITBYTE_STRING(bConverting, root, "reservYn", res.reservYn, memory);
        JSON_ALLOC_MULITBYTE_STRING(bConverting, root, "useTime", res.useTime, memory);
        JSON_ALLOC_MULITBYTE_STRING(bConverting, root, "payYn", res.payYn, memory);
        JSON_ALLOC_MULITBYTE_STRING(bConverting, root, "fee", res.fee, memory);
        JSON_ALLOC_MULITBYTE_STRING(bConverting, root, "updateDt", res.updateDt, memory);
        JSON_ALLOC_MULITBYTE_STRING(bConverting, root, "noorX", res.noorX, memory);
        JSON_ALLOC_MULITBYTE_STRING(bConverting, root, "noorY", res.noorY, memory);
        res.totalCnt = root["totalCnt"].asInt();

        element = root["evChargers"];
        int cnt = res.totalCnt;
        if(cnt > 0)
        {
            JSON_ALLOC_BLOCK(res.chargerInfo, NDDSTMAP_POIEVSTATIONCHARGERINFO_DTO*, sizeof(NDDSTMAP_POIEVSTATIONCHARGERINFO_DTO) * cnt, memory);
            for(int i = 0;i<cnt;i++)
            {
                JSON_ALLOC_MULITBYTE_STRING(bConverting, element[i], "bid", res.chargerInfo[i].bid, memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, element[i], "sid", res.chargerInfo[i].sid, memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, element[i], "cid", res.chargerInfo[i].cid, memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, element[i], "type", res.chargerInfo[i].type, memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, element[i], "status", res.chargerInfo[i].status, memory);
            }
        }

    }
    catch (...)
    {
        return false;
    }

    *ppRes = (NDDSTMAP_FINDPOIEVSTATIONINFO_RES*)malloc(sizeof(NDDSTMAP_FINDPOIEVSTATIONINFO_RES));
    memcpy(*ppRes, &res, sizeof(NDDSTMAP_FINDPOIEVSTATIONINFO_RES));
    return true;
}

bool CNddsLibTmap::ParserFindPoiEvStationsByPoint(NDDSLIB_Json::Value &root,NDDS_MEMORYSTATUS &memory, NDDSTMAP_FINDPOIEVSTATIONSBYPOINT_RES  **ppRes)
{
    CNddsLibCommonData &CommonData = CNddsLibCommonData::GetInstance();
    bool bConverting = CommonData.m_eCharacterSet == NDDSCHARACTERSET_EUCKR?true:false;

    NDDSTMAP_FINDPOIEVSTATIONSBYPOINT_RES res;
    NDDSLIB_Json::Value element;

    memset(&res, 0x00, sizeof(NDDSTMAP_FINDPOIEVSTATIONSBYPOINT_RES));

    try
    {
        res.totalCnt = root["totalCnt"].asInt();

        element = root["evStations"];
        int cnt = res.totalCnt;

        if(cnt > 0)
        {
            JSON_ALLOC_BLOCK(res.evstations, NDDSTMAP_POIEVSTATIONS_DTO*, sizeof(NDDSTMAP_POIEVSTATIONS_DTO) * cnt, memory);
            for(int i = 0;i<cnt;i++)
            {
                JSON_ALLOC_MULITBYTE_STRING(bConverting, element[i], "poiId", res.evstations[i].poiId, memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, element[i], "stationName", res.evstations[i].stationName, memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, element[i], "addr", res.evstations[i].addr, memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, element[i], "mngName", res.evstations[i].mngName, memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, element[i], "noorX", res.evstations[i].noorX, memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, element[i], "noorY", res.evstations[i].noorY, memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, element[i], "radius", res.evstations[i].radius, memory);
                res.evstations[i].totalCnt = element[i]["totalCnt"].asInt();

                if(res.evstations[i].totalCnt > 0)
                {
                    JSON_ALLOC_BLOCK(res.evstations[i].evChargers, NDDSTMAP_POIEVSTATIONCHARGERINFO_DTO*, sizeof(NDDSTMAP_POIEVSTATIONCHARGERINFO_DTO) * res.evstations[i].totalCnt, memory);
                    NDDSLIB_Json::Value element2 = element[i]["evChargers"];
                    for(int j = 0;j<res.evstations[i].totalCnt;j++)
                    {
                        JSON_ALLOC_MULITBYTE_STRING(bConverting, element2[j], "bid", res.evstations[i].evChargers[j].bid, memory);
                        JSON_ALLOC_MULITBYTE_STRING(bConverting, element2[j], "sid", res.evstations[i].evChargers[j].sid, memory);
                        JSON_ALLOC_MULITBYTE_STRING(bConverting, element2[j], "cid", res.evstations[i].evChargers[j].cid, memory);
                        JSON_ALLOC_MULITBYTE_STRING(bConverting, element2[j], "type", res.evstations[i].evChargers[j].type, memory);
                        JSON_ALLOC_MULITBYTE_STRING(bConverting, element2[j], "status", res.evstations[i].evChargers[j].status, memory);
                    }
                }
            }
        }
    }
    catch (...)
    {
        return false;
    }

    *ppRes = (NDDSTMAP_FINDPOIEVSTATIONSBYPOINT_RES*)malloc(sizeof(NDDSTMAP_FINDPOIEVSTATIONSBYPOINT_RES));
    memcpy(*ppRes, &res, sizeof(NDDSTMAP_FINDPOIEVSTATIONSBYPOINT_RES));
    return true;
}

bool CNddsLibTmap::MakeFindPickatPopulars( NDDSLIB_Json::Value &root, NDDSTMAP_FINDPICKATPOPULARS_REQ *pReq )
{
    try
    {
        root["radius"] = pReq->radius;
        root["noorX"] = pReq->noorX;
        root["noorY"] = pReq->noorY;
        root["reqCnt"] = pReq->reqCnt;
    }
    catch (...)
    {
        return false;
    }

    return true;
}

bool CNddsLibTmap::MakeFindRoadName(NDDSLIB_Json::Value &root, NDDSTMAP_FINDROADNAME_REQ  *pReq)
{
    try
    {
        root["reqCnt"] = pReq->reqCnt;
        root["reqSeq"] = pReq->reqSeq;
        root["largeCodeName"] = pReq->largeCodeName;
        root["middleCodeName"] = pReq->middleCodeName;
    }
    catch (...)
    {
        return false;
    }

    return true;
}

bool CNddsLibTmap::MakeFindPickatPoiDetail(NDDSLIB_Json::Value &root, NDDSTMAP_FINDPICKATPOIDETAIL_REQ  *pReq)
{
    try
    {
        root["poiId"] = pReq->poiId;
    }
    catch (...)
    {
        return false;
    }

    return true;
}

bool CNddsLibTmap::MakeFindAreaNamesByStep(NDDSLIB_Json::Value &root, NDDSTMAP_FINDAREANAMESBYSTEP_REQ  *pReq)
{
    try
    {
        root["reqCnt"] = pReq->reqCnt;
        root["reqSeq"] = pReq->reqSeq;
        root["largeCodeName"] = pReq->largeCodeName;
        root["middleCodeName"] = pReq->middleCodeName;
        root["addressFlag"] = pReq->addressFlag;
    }
    catch (...)
    {
        return false;
    }

    return true;
}

bool CNddsLibTmap::MakeFindPoiStationInfo(NDDSLIB_Json::Value &root, NDDSTMAP_FINDPOISTATIONINFO_REQ  *pReq)
{
    try
    {
        root["noorX"] = pReq->noorX;
        root["noorY"] = pReq->noorY;
        root["radius"] = pReq->radius;
        root["dataKind"] = pReq->dataKind;
        root["fuelType"] = pReq->fuelType;
    }
    catch (...)
    {
        return false;
    }

    return true;
}

bool CNddsLibTmap::MakeFindPoiEvStationInfo(NDDSLIB_Json::Value &root, NDDSTMAP_FINDPOIEVSTATIONINFO_REQ  *pReq)
{
    try
    {
        root["poiId"] = pReq->poiId;
    }
    catch (...)
    {
        return false;
    }

    return true;
}

bool CNddsLibTmap::MakeFindPoiEvStationsByPoint(NDDSLIB_Json::Value &root, NDDSTMAP_FINDPOIEVSTATIONSBYPOINT_REQ  *pReq)
{
    try
    {
        root["noorX"] = pReq->noorX;
        root["noorY"] = pReq->noorY;
        root["radius"] = pReq->radius;
    }
    catch (...)
    {
        return false;
    }

    return true;
}

bool CNddsLibTmap::MakeFindUserData( NDDSLIB_Json::Value &root, NDDSTMAP_FINDUSERDATA_REQ *pReq )
{
    return true;
}

bool CNddsLibTmap::MakeRemoveTrafficFavorites( NDDSLIB_Json::Value &root, NDDSTMAP_REMOVETRAFFICFAVORITE_REQ *pReq )
{
    try{
        root["infTypCd"] = pReq->infTypCd;
        root["distributeId"] = pReq->distributeId;
        root["name"]	 = pReq->name;
        root["noorX"]	 = pReq->noorX;
        root["noorY"]	 = pReq->noorY;
    }catch(...){
        return false;
    }
    return true;
}

bool CNddsLibTmap::MakeUploadTrafficFavorites( NDDSLIB_Json::Value &root, NDDSTMAP_UPLOADTRAFFICFAVORITES_REQ *pReq )
{
    return MakeRegistTrafficFavorites(root,(NDDSTMAP_REGISTTRAFFICFAVORITES_REQ *)pReq);
}

bool CNddsLibTmap::MakeRegistTrafficFavorites( NDDSLIB_Json::Value &root, NDDSTMAP_REGISTTRAFFICFAVORITES_REQ *pReq )
{
    NDDSLIB_Json::Value trafficFavoriteRegistDtos;
    try{
        for(int i =0 ; i< pReq->nTrafficFavoriteRegistDtos;i++)
        {
            NDDSLIB_Json::Value trafficFavoriteRegistDtos;
            trafficFavoriteRegistDtos["infTypCd"] = pReq->trafficFavoriteRegistDtos[i].infTypCd;
            trafficFavoriteRegistDtos["distributeId"] = pReq->trafficFavoriteRegistDtos[i].distributeId;
            trafficFavoriteRegistDtos["name"]	= pReq->trafficFavoriteRegistDtos[i].name;
            trafficFavoriteRegistDtos["noorX"]	= pReq->trafficFavoriteRegistDtos[i].noorX;
            trafficFavoriteRegistDtos["noorY"]	= pReq->trafficFavoriteRegistDtos[i].noorY;

            root["trafficFavoriteRegistDtos"].append(trafficFavoriteRegistDtos);
        }
    }catch(...){
        return false;
    }
    return true;
}

bool CNddsLibTmap::MakeFindTrafficFavorites( NDDSLIB_Json::Value &root, NDDSTMAP_FINDTRAFFICFAVORITES_REQ *pReq )
{
    return true;
}

bool CNddsLibTmap::MakeUploadTotalRoute( NDDSLIB_Json::Value &root, NDDSTMAP_UPLOADTOTALROUTE_REQ *pReq )
{
    NDDSLIB_Json::Value routeFavoriteRegistDtos;
    try{
        root["uploadOption"]	= pReq->uploadOption;
        NDDSLIB_Json::Value startRouteDto,endRouteDto,midRouteDtos;
        // 출발지
        startRouteDto["name"]	= pReq->routeMyFavoriteRegistDto.startRouteDto.name;
        startRouteDto["poiId"]	= pReq->routeMyFavoriteRegistDto.startRouteDto.poiId;
        startRouteDto["navX"]	= pReq->routeMyFavoriteRegistDto.startRouteDto.navX;
        startRouteDto["navY"]	= pReq->routeMyFavoriteRegistDto.startRouteDto.navY;
        startRouteDto["rpFlag"]= pReq->routeMyFavoriteRegistDto.startRouteDto.rpFlag;
        // 도착지
        endRouteDto["name"]	= pReq->routeMyFavoriteRegistDto.endRouteDto.name;
        endRouteDto["poiId"]	= pReq->routeMyFavoriteRegistDto.endRouteDto.poiId;
        endRouteDto["navX"]	= pReq->routeMyFavoriteRegistDto.endRouteDto.navX;
        endRouteDto["navY"]	= pReq->routeMyFavoriteRegistDto.endRouteDto.navY;
        endRouteDto["rpFlag"]	= pReq->routeMyFavoriteRegistDto.endRouteDto.rpFlag;

        routeFavoriteRegistDtos["routeTitle"] = pReq->routeMyFavoriteRegistDto.routeTitle;
        routeFavoriteRegistDtos["startRouteDto"] = startRouteDto;
        routeFavoriteRegistDtos["endRouteDto"] = endRouteDto;

        // 경유지
        NDDSLIB_Json::Value midRoute;
        midRoute["name"] = pReq->routeMyFavoriteRegistDto.midRouteDtos[0].name;
        midRoute["poiId"] = pReq->routeMyFavoriteRegistDto.midRouteDtos[0].poiId;
        midRoute["navX"] = pReq->routeMyFavoriteRegistDto.midRouteDtos[0].navX;
        midRoute["navY"] = pReq->routeMyFavoriteRegistDto.midRouteDtos[0].navY;
        midRoute["rpFlag"] = pReq->routeMyFavoriteRegistDto.midRouteDtos[0].rpFlag;
        routeFavoriteRegistDtos["midRouteDto1"].append(midRoute);

        midRoute["name"] = pReq->routeMyFavoriteRegistDto.midRouteDtos[1].name;
        midRoute["poiId"] = pReq->routeMyFavoriteRegistDto.midRouteDtos[1].poiId;
        midRoute["navX"] = pReq->routeMyFavoriteRegistDto.midRouteDtos[1].navX;
        midRoute["navY"] = pReq->routeMyFavoriteRegistDto.midRouteDtos[1].navY;
        midRoute["rpFlag"] = pReq->routeMyFavoriteRegistDto.midRouteDtos[1].rpFlag;
        routeFavoriteRegistDtos["midRouteDto2"].append(midRoute);

        midRoute["name"] = pReq->routeMyFavoriteRegistDto.midRouteDtos[2].name;
        midRoute["poiId"] = pReq->routeMyFavoriteRegistDto.midRouteDtos[2].poiId;
        midRoute["navX"] = pReq->routeMyFavoriteRegistDto.midRouteDtos[2].navX;
        midRoute["navY"] = pReq->routeMyFavoriteRegistDto.midRouteDtos[2].navY;
        midRoute["rpFlag"] = pReq->routeMyFavoriteRegistDto.midRouteDtos[2].rpFlag;
        routeFavoriteRegistDtos["midRouteDto3"].append(midRoute);

        root["routeFavoriteRegistDtos"].append(routeFavoriteRegistDtos);

        for(int i =0 ; i< pReq->nRouteFavoriteRegistDtos;i++)
        {
            // 출발지
            startRouteDto["name"]	= pReq->routeFavoriteRegistDtos[i].startRouteDto.name;
            startRouteDto["poiId"]	= pReq->routeFavoriteRegistDtos[i].startRouteDto.poiId;
            startRouteDto["navX"]	= pReq->routeFavoriteRegistDtos[i].startRouteDto.navX;
            startRouteDto["navY"]	= pReq->routeFavoriteRegistDtos[i].startRouteDto.navY;
            startRouteDto["rpFlag"]= pReq->routeFavoriteRegistDtos[i].startRouteDto.rpFlag;
            // 도착지
            endRouteDto["name"]	= pReq->routeFavoriteRegistDtos[i].endRouteDto.name;
            endRouteDto["poiId"]	= pReq->routeFavoriteRegistDtos[i].endRouteDto.poiId;
            endRouteDto["navX"]	= pReq->routeFavoriteRegistDtos[i].endRouteDto.navX;
            endRouteDto["navY"]	= pReq->routeFavoriteRegistDtos[i].endRouteDto.navY;
            endRouteDto["rpFlag"]	= pReq->routeFavoriteRegistDtos[i].endRouteDto.rpFlag;

            routeFavoriteRegistDtos["routeTitle"] = pReq->routeFavoriteRegistDtos[i].routeTitle;
            routeFavoriteRegistDtos["startRouteDto"] = startRouteDto;
            routeFavoriteRegistDtos["endRouteDto"] = endRouteDto;

            // 경유지
            int nMidList = pReq->routeFavoriteRegistDtos[i].nMidList;
            for(int mIdx = 0 ; mIdx < nMidList ; mIdx++)
            {
                NDDSLIB_Json::Value midRoute;
                midRoute["name"] = pReq->routeFavoriteRegistDtos[i].midRouteDtos[mIdx].name;
                midRoute["poiId"] = pReq->routeFavoriteRegistDtos[i].midRouteDtos[mIdx].poiId;
                midRoute["navX"] = pReq->routeFavoriteRegistDtos[i].midRouteDtos[mIdx].navX;
                midRoute["navY"] = pReq->routeFavoriteRegistDtos[i].midRouteDtos[mIdx].navY;
                midRoute["rpFlag"] = pReq->routeFavoriteRegistDtos[i].midRouteDtos[mIdx].rpFlag;

                routeFavoriteRegistDtos["midRouteDtos"].append(midRoute);
            }
            root["routeFavoriteRegistDtos"].append(routeFavoriteRegistDtos);
        }
    }catch(...){
        return false;
    }
    return true;
}

bool CNddsLibTmap::MakeRegistRouteMulti( NDDSLIB_Json::Value &root, NDDSTMAP_REGISTROUTEMULTI_REQ *pReq )
{
    NDDSLIB_Json::Value routeFavoriteRegistDtos;
    try{
        for(int i =0 ; i< pReq->nRouteFavoriteRegistDtos;i++)
        {
            NDDSLIB_Json::Value startRouteDto,endRouteDto,midRouteDtos;
            // 출발지
            startRouteDto["name"]	= pReq->routeFavoriteRegistDtos[i].startRouteDto.name;
            startRouteDto["poiId"]	= pReq->routeFavoriteRegistDtos[i].startRouteDto.poiId;
            startRouteDto["navX"]	= pReq->routeFavoriteRegistDtos[i].startRouteDto.navX;
            startRouteDto["navY"]	= pReq->routeFavoriteRegistDtos[i].startRouteDto.navY;
            startRouteDto["rpFlag"]= pReq->routeFavoriteRegistDtos[i].startRouteDto.rpFlag;
            // 도착지
            endRouteDto["name"]	= pReq->routeFavoriteRegistDtos[i].endRouteDto.name;
            endRouteDto["poiId"]	= pReq->routeFavoriteRegistDtos[i].endRouteDto.poiId;
            endRouteDto["navX"]	= pReq->routeFavoriteRegistDtos[i].endRouteDto.navX;
            endRouteDto["navY"]	= pReq->routeFavoriteRegistDtos[i].endRouteDto.navY;
            endRouteDto["rpFlag"]	= pReq->routeFavoriteRegistDtos[i].endRouteDto.rpFlag;

            routeFavoriteRegistDtos["routeTitle"] = pReq->routeFavoriteRegistDtos[i].routeTitle;
            routeFavoriteRegistDtos["startRouteDto"] = startRouteDto;
            routeFavoriteRegistDtos["endRouteDto"] = endRouteDto;

            // 경유지
            int nMidList = pReq->routeFavoriteRegistDtos[i].nMidList;
            for(int mIdx = 0 ; mIdx < nMidList ; mIdx++)
            {
                NDDSLIB_Json::Value midRoute;
                midRoute["name"] = pReq->routeFavoriteRegistDtos[i].midRouteDtos[mIdx].name;
                midRoute["poiId"] = pReq->routeFavoriteRegistDtos[i].midRouteDtos[mIdx].poiId;
                midRoute["navX"] = pReq->routeFavoriteRegistDtos[i].midRouteDtos[mIdx].navX;
                midRoute["navY"] = pReq->routeFavoriteRegistDtos[i].midRouteDtos[mIdx].navY;
                midRoute["rpFlag"] = pReq->routeFavoriteRegistDtos[i].midRouteDtos[mIdx].rpFlag;

                routeFavoriteRegistDtos["midRouteDtos"].append(midRoute);
            }
            root["routeFavoriteRegistDtos"].append(routeFavoriteRegistDtos);
        }
    }catch(...){
        return false;
    }
    return true;
}

bool CNddsLibTmap::ParserRegistRouteMulti( NDDSLIB_Json::Value &root,NDDS_MEMORYSTATUS &memory, NDDSTMAP_REGISTROUTEMULTI_RES **ppRes )
{
    NDDSTMAP_REGISTROUTEMULTI_RES res;
    NDDSLIB_Json::Value element;

    memset(&res, 0x00, sizeof(NDDSTMAP_REGISTROUTEMULTI_RES));

    *ppRes = (NDDSTMAP_REGISTROUTEMULTI_RES*)malloc(sizeof(NDDSTMAP_REGISTROUTEMULTI_RES));
    memcpy(*ppRes, &res, sizeof(NDDSTMAP_REGISTROUTEMULTI_RES));
    return true;
}

bool CNddsLibTmap::ParserUploadTotalRoute( NDDSLIB_Json::Value &root,NDDS_MEMORYSTATUS &memory, NDDSTMAP_UPLOADTOTALROUTE_RES **ppRes )
{
    NDDSTMAP_UPLOADTOTALROUTE_RES res;
    memset(&res, 0x00, sizeof(NDDSTMAP_UPLOADTOTALROUTE_RES));
    *ppRes = (NDDSTMAP_UPLOADTOTALROUTE_RES*)malloc(sizeof(NDDSTMAP_UPLOADTOTALROUTE_RES));
    memcpy(*ppRes, &res, sizeof(NDDSTMAP_UPLOADTOTALROUTE_RES));
    return true;
}

bool CNddsLibTmap::ParserFindTrafficFavorites( NDDSLIB_Json::Value &root,NDDS_MEMORYSTATUS &memory, NDDSTMAP_FINDTRAFFICFAVORITES_RES **ppRes )
{
    CNddsLibCommonData &CommonData = CNddsLibCommonData::GetInstance();
    bool bConverting = CommonData.m_eCharacterSet == NDDSCHARACTERSET_EUCKR?true:false;

    NDDSTMAP_FINDTRAFFICFAVORITES_RES res;

    memset(&res, 0x00, sizeof(NDDSTMAP_FINDPICKATPOPULARS_RES));
    try{
        NDDSLIB_Json::Value trafficFavorites = root["trafficFavorites"];
        res.nTrafficFavorites = trafficFavorites.size();

        if(res.nTrafficFavorites > 0)
        {
            JSON_ALLOC_BLOCK(res.trafficFavorites, NDDSTMAP_TRAFFICFAVORITES_DTO*, sizeof(NDDSTMAP_TRAFFICFAVORITES_DTO) * res.nTrafficFavorites, memory);
            for(int i = 0;i<res.nTrafficFavorites;i++)
            {
                res.trafficFavorites[i].seq = trafficFavorites[i]["seq"].asInt();
                JSON_ALLOC_MULITBYTE_STRING(bConverting, trafficFavorites[i], "infTypCd",	res.trafficFavorites[i].infTypCd,memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, trafficFavorites[i], "distributeId",res.trafficFavorites[i].distributeId,memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, trafficFavorites[i], "name",	res.trafficFavorites[i].name,memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, trafficFavorites[i], "noorX",	res.trafficFavorites[i].noorX,memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, trafficFavorites[i], "noorY",	res.trafficFavorites[i].noorY,memory);
            }
        }
    }catch(...)
    {
        return false;
    }

    *ppRes = (NDDSTMAP_FINDTRAFFICFAVORITES_RES*)malloc(sizeof(NDDSTMAP_FINDTRAFFICFAVORITES_RES));
    memcpy(*ppRes, &res, sizeof(NDDSTMAP_FINDTRAFFICFAVORITES_RES));
    return true;
}

bool CNddsLibTmap::ParserRegistTrafficFavorites( NDDSLIB_Json::Value &root,NDDS_MEMORYSTATUS &memory, NDDSTMAP_REGISTTRAFFICFAVORITES_RES **ppRes )
{
    NDDSTMAP_REGISTTRAFFICFAVORITES_RES res;
    memset(&res, 0x00, sizeof(NDDSTMAP_REGISTTRAFFICFAVORITES_RES));
    *ppRes = (NDDSTMAP_REGISTTRAFFICFAVORITES_RES*)malloc(sizeof(NDDSTMAP_REGISTTRAFFICFAVORITES_RES));
    memcpy(*ppRes, &res, sizeof(NDDSTMAP_REGISTTRAFFICFAVORITES_RES));
    return true;
}

bool CNddsLibTmap::ParserUploadTrafficFavorites( NDDSLIB_Json::Value &root,NDDS_MEMORYSTATUS &memory, NDDSTMAP_UPLOADTRAFFICFAVORITES_RES **ppRes )
{
    NDDSTMAP_UPLOADTRAFFICFAVORITES_RES res;
    memset(&res, 0x00, sizeof(NDDSTMAP_UPLOADTRAFFICFAVORITES_RES));
    *ppRes = (NDDSTMAP_UPLOADTRAFFICFAVORITES_RES*)malloc(sizeof(NDDSTMAP_UPLOADTRAFFICFAVORITES_RES));
    memcpy(*ppRes, &res, sizeof(NDDSTMAP_UPLOADTOTALROUTE_RES));
    return true;
}

bool CNddsLibTmap::ParserRemoveTrafficFavorites( NDDSLIB_Json::Value &root,NDDS_MEMORYSTATUS &memory, NDDSTMAP_REMOVETRAFFICFAVORITE_RES **ppRes )
{
    NDDSTMAP_REMOVETRAFFICFAVORITE_RES res;
    memset(&res, 0x00, sizeof(NDDSTMAP_REMOVETRAFFICFAVORITE_RES));
    *ppRes = (NDDSTMAP_REMOVETRAFFICFAVORITE_RES*)malloc(sizeof(NDDSTMAP_REMOVETRAFFICFAVORITE_RES));
    memcpy(*ppRes, &res, sizeof(NDDSTMAP_REMOVETRAFFICFAVORITE_RES));
    return true;
}


bool CNddsLibTmap::ParserFindUserData( NDDSLIB_Json::Value &root,NDDS_MEMORYSTATUS &memory, NDDSTMAP_FINDUSERDATA_RES **ppRes )
{
    CNddsLibCommonData &CommonData = CNddsLibCommonData::GetInstance();
    bool bConverting = CommonData.m_eCharacterSet == NDDSCHARACTERSET_EUCKR?true:false;

    NDDSTMAP_FINDUSERDATA_RES res;
    NDDSLIB_Json::Value poiRecentDtos,poiFavoriteDtos,trafficFavoriteDtos,routeFavoriteDtos,element;

    memset(&res, 0x00, sizeof(NDDSTMAP_FINDUSERDATA_RES));
    try{
        element = root["poiMyFavoriteDto"];
        if(element.size() > 0)
        {
            JSON_ALLOC_MULITBYTE_STRING(bConverting, element, "homeCustName", res.poiMyFavoriteDto.homeCustName, memory);
            JSON_ALLOC_MULITBYTE_STRING(bConverting, element, "homeNoorX", res.poiMyFavoriteDto.homeNoorX, memory);
            JSON_ALLOC_MULITBYTE_STRING(bConverting, element, "homeNoorY", res.poiMyFavoriteDto.homeNoorY, memory);
            JSON_ALLOC_MULITBYTE_STRING(bConverting, element, "homePoiId", res.poiMyFavoriteDto.homePoiId, memory);
            JSON_ALLOC_MULITBYTE_STRING(bConverting, element, "homeNavSeq", res.poiMyFavoriteDto.homeNavSeq, memory);
            JSON_ALLOC_MULITBYTE_STRING(bConverting, element, "homeLcdName", res.poiMyFavoriteDto.homeLcdName, memory);
            JSON_ALLOC_MULITBYTE_STRING(bConverting, element, "homeMcdName", res.poiMyFavoriteDto.homeMcdName, memory);
            JSON_ALLOC_MULITBYTE_STRING(bConverting, element, "homeScdName", res.poiMyFavoriteDto.homeScdName, memory);
            JSON_ALLOC_MULITBYTE_STRING(bConverting, element, "homeDcdName", res.poiMyFavoriteDto.homeDcdName, memory);
            JSON_ALLOC_MULITBYTE_STRING(bConverting, element, "homePrimaryBun", res.poiMyFavoriteDto.homePrimaryBun, memory);
            JSON_ALLOC_MULITBYTE_STRING(bConverting, element, "homeSecondaryBun", res.poiMyFavoriteDto.homeSecondaryBun, memory);
            JSON_ALLOC_MULITBYTE_STRING(bConverting, element, "homeMlClass", res.poiMyFavoriteDto.homeMlClass, memory);
            JSON_ALLOC_MULITBYTE_STRING(bConverting, element, "homeRoadName", res.poiMyFavoriteDto.homeRoadName, memory);
            JSON_ALLOC_MULITBYTE_STRING(bConverting, element, "homeBldNo1", res.poiMyFavoriteDto.homeBldNo1, memory);
            JSON_ALLOC_MULITBYTE_STRING(bConverting, element, "homeBldNo2", res.poiMyFavoriteDto.homeBldNo2, memory);
            res.poiMyFavoriteDto.homeRpFlag = element["homeRpFlag"].asUInt();
            JSON_ALLOC_MULITBYTE_STRING(bConverting, element, "homeAddInfo", res.poiMyFavoriteDto.homeAddInfo, memory);
            JSON_ALLOC_MULITBYTE_STRING(bConverting, element, "homeInsDatetime", res.poiMyFavoriteDto.homeInsDatetime, memory);

            JSON_ALLOC_MULITBYTE_STRING(bConverting, element, "officeCustName", res.poiMyFavoriteDto.officeCustName, memory);
            JSON_ALLOC_MULITBYTE_STRING(bConverting, element, "officeNoorX", res.poiMyFavoriteDto.officeNoorX, memory);
            JSON_ALLOC_MULITBYTE_STRING(bConverting, element, "officeNoorY", res.poiMyFavoriteDto.officeNoorY, memory);
            JSON_ALLOC_MULITBYTE_STRING(bConverting, element, "officePoiId", res.poiMyFavoriteDto.officePoiId, memory);
            JSON_ALLOC_MULITBYTE_STRING(bConverting, element, "officeNavSeq", res.poiMyFavoriteDto.officeNavSeq, memory);
            JSON_ALLOC_MULITBYTE_STRING(bConverting, element, "officeLcdName", res.poiMyFavoriteDto.officeLcdName, memory);
            JSON_ALLOC_MULITBYTE_STRING(bConverting, element, "officeMcdName", res.poiMyFavoriteDto.officeMcdName, memory);
            JSON_ALLOC_MULITBYTE_STRING(bConverting, element, "officeScdName", res.poiMyFavoriteDto.officeScdName, memory);
            JSON_ALLOC_MULITBYTE_STRING(bConverting, element, "officeDcdName", res.poiMyFavoriteDto.officeDcdName, memory);
            JSON_ALLOC_MULITBYTE_STRING(bConverting, element, "officePrimaryBun", res.poiMyFavoriteDto.officePrimaryBun, memory);
            JSON_ALLOC_MULITBYTE_STRING(bConverting, element, "officeSecondaryBun", res.poiMyFavoriteDto.officeSecondaryBun, memory);
            JSON_ALLOC_MULITBYTE_STRING(bConverting, element, "officeMlClass", res.poiMyFavoriteDto.officeMlClass, memory);
            JSON_ALLOC_MULITBYTE_STRING(bConverting, element, "officeRoadName", res.poiMyFavoriteDto.officeRoadName, memory);
            JSON_ALLOC_MULITBYTE_STRING(bConverting, element, "officeBldNo1", res.poiMyFavoriteDto.officeBldNo1, memory);
            JSON_ALLOC_MULITBYTE_STRING(bConverting, element, "officeBldNo2", res.poiMyFavoriteDto.officeBldNo2, memory);
            res.poiMyFavoriteDto.officeRpFlag = element["officeeRpFlag"].asUInt();
            JSON_ALLOC_MULITBYTE_STRING(bConverting, element, "officeAddInfo", res.poiMyFavoriteDto.officeAddInfo, memory);
            JSON_ALLOC_MULITBYTE_STRING(bConverting, element, "homeInsDatetime", res.poiMyFavoriteDto.homeInsDatetime, memory);
        }

        element.clear();
        element = root["routeMyFavoriteDto"];
        {
            NDDSLIB_Json::Value RoutePos;

            JSON_ALLOC_MULITBYTE_STRING(bConverting, element, "routeTitle", res.routeMyFavoriteDto.routeTitle, memory);
            JSON_ALLOC_MULITBYTE_STRING(bConverting, element, "insDatetime", res.routeMyFavoriteDto.insDatetime, memory);

            RoutePos = element["startRouteDto"];
            JSON_ALLOC_MULITBYTE_STRING(bConverting, RoutePos, "name", res.routeMyFavoriteDto.startRouteDto.name, memory);
            JSON_ALLOC_MULITBYTE_STRING(bConverting, RoutePos, "poiId", res.routeMyFavoriteDto.startRouteDto.poiId, memory);
            JSON_ALLOC_MULITBYTE_STRING(bConverting, RoutePos, "navX", res.routeMyFavoriteDto.startRouteDto.navX, memory);
            JSON_ALLOC_MULITBYTE_STRING(bConverting, RoutePos, "navY", res.routeMyFavoriteDto.startRouteDto.navY, memory);
            res.routeMyFavoriteDto.startRouteDto.rpFlag = RoutePos["rpFlag"].asUInt();

            RoutePos = element["endRouteDto"];
            JSON_ALLOC_MULITBYTE_STRING(bConverting, RoutePos, "name", res.routeMyFavoriteDto.endRouteDto.name, memory);
            JSON_ALLOC_MULITBYTE_STRING(bConverting, RoutePos, "poiId", res.routeMyFavoriteDto.endRouteDto.poiId, memory);
            JSON_ALLOC_MULITBYTE_STRING(bConverting, RoutePos, "navX", res.routeMyFavoriteDto.endRouteDto.navX, memory);
            JSON_ALLOC_MULITBYTE_STRING(bConverting, RoutePos, "navY", res.routeMyFavoriteDto.endRouteDto.navY, memory);
            res.routeMyFavoriteDto.endRouteDto.rpFlag = RoutePos["rpFlag"].asUInt();

            JSON_ALLOC_BLOCK(res.routeMyFavoriteDto.midRouteDtos, NDDSTMAP_ROUTEPOS_DTO*, sizeof(NDDSTMAP_ROUTEPOS_DTO) * 3, memory);

            NDDSLIB_Json::Value MidRoute = element["midRouteDto1"];
            JSON_ALLOC_MULITBYTE_STRING(bConverting, MidRoute, "name", res.routeMyFavoriteDto.midRouteDtos[0].name, memory);
            JSON_ALLOC_MULITBYTE_STRING(bConverting, MidRoute, "poiId", res.routeMyFavoriteDto.midRouteDtos[0].poiId, memory);
            JSON_ALLOC_MULITBYTE_STRING(bConverting, MidRoute, "navX", res.routeMyFavoriteDto.midRouteDtos[0].navX, memory);
            JSON_ALLOC_MULITBYTE_STRING(bConverting, MidRoute, "navY", res.routeMyFavoriteDto.midRouteDtos[0].navY, memory);
            res.routeMyFavoriteDto.midRouteDtos[0].rpFlag = MidRoute["rpFlag"].asUInt();

            MidRoute.clear();
            MidRoute = element["midRouteDto2"];
            JSON_ALLOC_MULITBYTE_STRING(bConverting, MidRoute, "name", res.routeMyFavoriteDto.midRouteDtos[1].name, memory);
            JSON_ALLOC_MULITBYTE_STRING(bConverting, MidRoute, "poiId", res.routeMyFavoriteDto.midRouteDtos[1].poiId, memory);
            JSON_ALLOC_MULITBYTE_STRING(bConverting, MidRoute, "navX", res.routeMyFavoriteDto.midRouteDtos[1].navX, memory);
            JSON_ALLOC_MULITBYTE_STRING(bConverting, MidRoute, "navY", res.routeMyFavoriteDto.midRouteDtos[1].navY, memory);
            res.routeMyFavoriteDto.midRouteDtos[1].rpFlag = MidRoute["rpFlag"].asUInt();

            MidRoute.clear();
            MidRoute = element["midRouteDto3"];
            JSON_ALLOC_MULITBYTE_STRING(bConverting, MidRoute, "name", res.routeMyFavoriteDto.midRouteDtos[2].name, memory);
            JSON_ALLOC_MULITBYTE_STRING(bConverting, MidRoute, "poiId", res.routeMyFavoriteDto.midRouteDtos[2].poiId, memory);
            JSON_ALLOC_MULITBYTE_STRING(bConverting, MidRoute, "navX", res.routeMyFavoriteDto.midRouteDtos[2].navX, memory);
            JSON_ALLOC_MULITBYTE_STRING(bConverting, MidRoute, "navY", res.routeMyFavoriteDto.midRouteDtos[2].navY, memory);
            res.routeMyFavoriteDto.midRouteDtos[2].rpFlag = MidRoute["rpFlag"].asUInt();
        }
/*
        // 최근길 상세정보 리스트
        poiRecentDtos = root["poiRecentDtos"];
        res.nPoiRecent = poiRecentDtos.size();
        if(res.nPoiRecent > 0)
        {
            res.poiRecentDtos = (NDDSTMAP_POIRECENTUPLOADS_DTO *)malloc(sizeof(NDDSTMAP_POIRECENTUPLOADS_DTO)*res.nPoiRecent);
            for(int i = 0; i<res.nPoiRecent;i++)
            {
                JSON_ALLOC_MULITBYTE_STRING(bConverting, poiRecentDtos[i],"custName",res.poiRecentDtos[i].custName,memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, poiRecentDtos[i],"noorX",res.poiRecentDtos[i].noorX,memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, poiRecentDtos[i],"noorY",res.poiRecentDtos[i].noorY,memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, poiRecentDtos[i],"poiId",res.poiRecentDtos[i].poiId,memory);
                res.poiRecentDtos[i].rpFlag = poiRecentDtos[i]["rpFlag"].asInt();
            }
        }
*/
        element = root["poiRecentDtos"];
        res.nPoiRecent = element.size();

        if(element.size() > 0)
        {
            JSON_ALLOC_BLOCK(res.poiRecentDtos, NDDSTMAP_POIRECENTS_DTO*, sizeof(NDDSTMAP_POIRECENTS_DTO) * element.size(), memory);
            for(int i = 0;i<(int) element.size();i++)
            {
                JSON_ALLOC_MULITBYTE_STRING(bConverting, element[i], "custName",	res.poiRecentDtos[i].custName,memory);
                JSON_ALLOC_STRING(element[i], "noorX",	res.poiRecentDtos[i].noorX,memory);
                JSON_ALLOC_STRING(element[i], "noorY",	res.poiRecentDtos[i].noorY,memory);
                JSON_ALLOC_STRING(element[i], "poiId",	res.poiRecentDtos[i].poiId,memory);
                JSON_ALLOC_STRING(element[i], "navSeq",	res.poiRecentDtos[i].navSeq,memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, element[i], "lcdName",	res.poiRecentDtos[i].lcdName,memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, element[i], "mcdName",	res.poiRecentDtos[i].mcdName,memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, element[i], "scdName",	res.poiRecentDtos[i].scdName,memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, element[i], "dcdName",	res.poiRecentDtos[i].dcdName,memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, element[i], "primaryBun",	res.poiRecentDtos[i].primaryBun,memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, element[i], "secondaryBun",res.poiRecentDtos[i].secondaryBun,memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, element[i], "mlClass",		res.poiRecentDtos[i].mlClass,memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, element[i], "roadName",	res.poiRecentDtos[i].roadName,memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, element[i], "bldNo1",	res.poiRecentDtos[i].bldNo1,memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, element[i], "bldNo2",	res.poiRecentDtos[i].bldNo2,memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, element[i], "repClsName",	res.poiRecentDtos[i].repClsName,memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, element[i], "clsAName",	res.poiRecentDtos[i].clsAName,memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, element[i], "clsBName",	res.poiRecentDtos[i].clsBName,memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, element[i], "clsCName",	res.poiRecentDtos[i].clsCName,memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, element[i], "clsDName",	res.poiRecentDtos[i].clsDName,memory);
                res.poiRecentDtos[i].rpFlag = element[i]["rpFlag"].asInt();
                JSON_ALLOC_MULITBYTE_STRING(bConverting, element[i], "telNo",	res.poiRecentDtos[i].telNo,memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, element[i], "totalCnt",	res.poiRecentDtos[i].totalCnt,memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, element[i], "svcDate",	res.poiRecentDtos[i].svcDate,memory);
            }
        }

        // POI 즐겨찾기 상세정보 리스트
        poiFavoriteDtos = root["poiFavoriteDtos"];
        res.nPoiFavorite = poiFavoriteDtos.size();
        if(res.nPoiFavorite > 0)
        {
            res.poiFavoriteDtos =(NDDSTMAP_POIFAVORITES_DTO  *) malloc(sizeof(NDDSTMAP_POIFAVORITES_DTO )*res.nPoiFavorite);
            memset(res.poiFavoriteDtos, 0x00, sizeof(NDDSTMAP_POIFAVORITES_DTO )*res.nPoiFavorite);
            for(int i = 0; i<res.nPoiFavorite;i++)
            {
                JSON_ALLOC_MULITBYTE_STRING(bConverting, poiFavoriteDtos[i],"custName",res.poiFavoriteDtos[i].custName,memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, poiFavoriteDtos[i],"noorX",res.poiFavoriteDtos[i].noorX,memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, poiFavoriteDtos[i],"noorY",res.poiFavoriteDtos[i].noorY,memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, poiFavoriteDtos[i],"poiId",res.poiFavoriteDtos[i].poiId,memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, poiFavoriteDtos[i],"navSeq",res.poiFavoriteDtos[i].navSeq,memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, poiFavoriteDtos[i], "lcdName",	res.poiFavoriteDtos[i].lcdName,memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, poiFavoriteDtos[i], "mcdName",	res.poiFavoriteDtos[i].mcdName,memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, poiFavoriteDtos[i], "scdName",	res.poiFavoriteDtos[i].scdName,memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, poiFavoriteDtos[i], "dcdName",	res.poiFavoriteDtos[i].dcdName,memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, poiFavoriteDtos[i], "primaryBun",	res.poiFavoriteDtos[i].primaryBun,memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, poiFavoriteDtos[i], "secondaryBun",	res.poiFavoriteDtos[i].secondaryBun,memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, poiFavoriteDtos[i], "mlClass",	res.poiFavoriteDtos[i].mlClass,memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, poiFavoriteDtos[i], "roadName",	res.poiFavoriteDtos[i].roadName,memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, poiFavoriteDtos[i], "bldNo1",	res.poiFavoriteDtos[i].bldNo1,memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, poiFavoriteDtos[i], "bldNo2",	res.poiFavoriteDtos[i].bldNo2,memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, poiFavoriteDtos[i], "repClsName",	res.poiFavoriteDtos[i].repClsName,memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, poiFavoriteDtos[i], "clsAName",	res.poiFavoriteDtos[i].clsAName,memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, poiFavoriteDtos[i], "clsBName",	res.poiFavoriteDtos[i].clsBName,memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, poiFavoriteDtos[i], "clsCName",	res.poiFavoriteDtos[i].clsCName,memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, poiFavoriteDtos[i], "clsDName",	res.poiFavoriteDtos[i].clsDName,memory);
                res.poiFavoriteDtos[i].rpFlag = poiFavoriteDtos[i]["rpFlag"].asInt();
                JSON_ALLOC_MULITBYTE_STRING(bConverting, poiFavoriteDtos[i], "telNo",	res.poiFavoriteDtos[i].telNo,memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, poiFavoriteDtos[i], "addInfo",	res.poiFavoriteDtos[i].addInfo,memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, poiFavoriteDtos[i], "orgCustName",	res.poiFavoriteDtos[i].orgCustName,memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, poiFavoriteDtos[i], "iconInfo",	res.poiFavoriteDtos[i].iconInfo,memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, poiFavoriteDtos[i], "insDatetime",	res.poiFavoriteDtos[i].insDatetime,memory);
            }
        }
/*
        // 교통 즐겨찾기 상세정보 리스트
        trafficFavoriteDtos = root["trafficFavoriteDtos"];
        res.nTrafficFavorite = trafficFavoriteDtos.size();
        if(res.nTrafficFavorite > 0)
        {
            res.trafficFavoriteDtos = (NDDSTMAP_TRAFFICFAVORITES_DTO  *) malloc(sizeof(NDDSTMAP_TRAFFICFAVORITES_DTO )*res.nTrafficFavorite);
            for(int i = 0; i<res.nTrafficFavorite;i++)
            {
                JSON_ALLOC_MULITBYTE_STRING(bConverting, poiFavoriteDtos[i],"infTypCd",res.trafficFavoriteDtos[i].infTypCd,memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, poiFavoriteDtos[i],"name",res.trafficFavoriteDtos[i].name,memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, poiFavoriteDtos[i],"noorX",res.trafficFavoriteDtos[i].noorX,memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, poiFavoriteDtos[i],"noorY",res.trafficFavoriteDtos[i].noorY,memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, poiFavoriteDtos[i],"distributeId",res.trafficFavoriteDtos[i].distributeId,memory);
            }
        }
*/
        // 경로 즐겨찾기 상세정보 리스트
        routeFavoriteDtos = root["routeFavoriteDtos"];
        res.nRouteFavorite = routeFavoriteDtos.size();

        JSON_ALLOC_MULITBYTE_STRING(bConverting, routeFavoriteDtos,"routeTitle",res.routeFavoriteDtos->routeTitle,memory);
        JSON_ALLOC_MULITBYTE_STRING(bConverting, routeFavoriteDtos,"insDatetime",res.routeFavoriteDtos->insDatetime,memory);

        if(res.nRouteFavorite > 0)
        {
            res.routeFavoriteDtos = (NDDSTMAP_ROUTEFAVORITE_DTO  *)malloc(sizeof(NDDSTMAP_ROUTEFAVORITE_DTO )*res.nRouteFavorite);
            for(int i = 0; i<res.nRouteFavorite;i++)
            {
                NDDSLIB_Json::Value startRouteDto	= routeFavoriteDtos[i]["startRouteDto"];
                NDDSLIB_Json::Value endRouteDto		= routeFavoriteDtos[i]["endRouteDto"];
                NDDSLIB_Json::Value midRouteDtos	= routeFavoriteDtos[i]["midRouteDtos"];

                JSON_ALLOC_MULITBYTE_STRING(bConverting, startRouteDto,"routeTitle",res.routeFavoriteDtos[i].routeTitle,memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, startRouteDto,"insDatetime",res.routeFavoriteDtos[i].insDatetime,memory);

                JSON_ALLOC_MULITBYTE_STRING(bConverting, startRouteDto,"name",res.routeFavoriteDtos[i].startRouteDto.name,memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, startRouteDto,"navX",res.routeFavoriteDtos[i].startRouteDto.navX,memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, startRouteDto,"navY",res.routeFavoriteDtos[i].startRouteDto.navY,memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, startRouteDto,"poiId",res.routeFavoriteDtos[i].startRouteDto.poiId,memory);
                res.routeFavoriteDtos[i].startRouteDto.rpFlag = startRouteDto["rpFlag"].asInt();

                JSON_ALLOC_MULITBYTE_STRING(bConverting, endRouteDto,"name",res.routeFavoriteDtos[i].startRouteDto.name,memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, endRouteDto,"navX",res.routeFavoriteDtos[i].startRouteDto.navX,memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, endRouteDto,"navY",res.routeFavoriteDtos[i].startRouteDto.navY,memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, endRouteDto,"poiId",res.routeFavoriteDtos[i].startRouteDto.poiId,memory);
                res.routeFavoriteDtos[i].endRouteDto.rpFlag = startRouteDto["rpFlag"].asInt();


                res.routeFavoriteDtos[i].nMidList = midRouteDtos.size();
                if(res.routeFavoriteDtos[i].nMidList > 0)
                {
                    res.routeFavoriteDtos[i].midRouteDtos = (NDDSTMAP_ROUTEPOS_DTO *)malloc(sizeof(NDDSTMAP_ROUTEPOS_DTO)*res.nRouteFavorite);

                    for(int j = 0; j<res.routeFavoriteDtos[i].nMidList;j++)
                    {
                        JSON_ALLOC_MULITBYTE_STRING(bConverting, midRouteDtos[j],"name",res.routeFavoriteDtos[i].midRouteDtos[j].name,memory);
                        JSON_ALLOC_MULITBYTE_STRING(bConverting, midRouteDtos[j],"navX",res.routeFavoriteDtos[i].midRouteDtos[j].navX,memory);
                        JSON_ALLOC_MULITBYTE_STRING(bConverting, midRouteDtos[j],"navY",res.routeFavoriteDtos[i].midRouteDtos[j].navY,memory);
                        JSON_ALLOC_MULITBYTE_STRING(bConverting, midRouteDtos[j],"poiId",res.routeFavoriteDtos[i].midRouteDtos[j].poiId,memory);
                        res.routeFavoriteDtos[i].endRouteDto.rpFlag = midRouteDtos[j]["rpFlag"].asInt();
                    }
                }
            }
        }
    }catch(...)
    {
        return false;
    }


    *ppRes = (NDDSTMAP_FINDUSERDATA_RES*)malloc(sizeof(NDDSTMAP_FINDUSERDATA_RES));
    memcpy(*ppRes, &res, sizeof(NDDSTMAP_FINDUSERDATA_RES));
    return true;
}

bool CNddsLibTmap::ParserFindNodeJamRank( NDDSLIB_Json::Value &root,NDDS_MEMORYSTATUS &memory, NDDSTMAP_FINDNODEJAMRANK_RES **ppRes )
{
    CNddsLibCommonData &CommonData = CNddsLibCommonData::GetInstance();
    bool bConverting = CommonData.m_eCharacterSet == NDDSCHARACTERSET_EUCKR?true:false;

    NDDSTMAP_FINDNODEJAMRANK_RES res;
    memset(&res, 0x00, sizeof(NDDSTMAP_FINDNODEJAMRANK_RES));

    try{
        NDDSLIB_Json::Value nodeJamRankJamPrioritys = root["nodeJamRankJamPrioritys"];
        res.repeatCnt = nodeJamRankJamPrioritys.size();

        if(res.repeatCnt > 0)
        {
            JSON_ALLOC_BLOCK(res.nodeJamRankJamPrioritys, NDDSTMAP_NODEJAMRANKJAMPRIORITYS_DTO*, sizeof(NDDSTMAP_NODEJAMRANKJAMPRIORITYS_DTO) * res.repeatCnt, memory);
            for(int i = 0;i<res.repeatCnt;i++)
            {
                res.nodeJamRankJamPrioritys[i].jamPriority = nodeJamRankJamPrioritys[i]["jamPriority"].asInt();
                res.nodeJamRankJamPrioritys[i].nodeId = nodeJamRankJamPrioritys[i]["nodeId"].asUInt();
                JSON_ALLOC_MULITBYTE_STRING(bConverting, nodeJamRankJamPrioritys[i], "nodeName",	res.nodeJamRankJamPrioritys[i].nodeName,memory);
                res.nodeJamRankJamPrioritys[i].posX = nodeJamRankJamPrioritys[i]["posX"].asInt();
                res.nodeJamRankJamPrioritys[i].posY = nodeJamRankJamPrioritys[i]["posY"].asInt();
            }
        }
    }catch(...){
        return false;
    }

    *ppRes = (NDDSTMAP_FINDNODEJAMRANK_RES*)malloc(sizeof(NDDSTMAP_FINDNODEJAMRANK_RES));
    memcpy(*ppRes, &res, sizeof(NDDSTMAP_FINDNODEJAMRANK_RES));
    return true;
}

bool CNddsLibTmap::MakeFindNodeJamRank( NDDSLIB_Json::Value &root, NDDSTMAP_FINDNODEJAMRANK_REQ *pReq )
{
    // 필드 없음
    return true;
}

bool CNddsLibTmap::ParserSmimageTrafficCongestio( NDDSLIB_Json::Value &root,NDDS_MEMORYSTATUS &memory, NDDSTMAP_SMIMAGETRAFFICCONGESTION_RES **ppRes )
{
    CNddsLibCommonData &CommonData = CNddsLibCommonData::GetInstance();
    bool bConverting = CommonData.m_eCharacterSet == NDDSCHARACTERSET_EUCKR?true:false;

    NDDSTMAP_SMIMAGETRAFFICCONGESTION_RES res;
    memset(&res, 0x00, sizeof(NDDSTMAP_SMIMAGETRAFFICCONGESTION_RES));

    try{
        NDDSLIB_Json::Value simplemapCongestionDtos = root["simplemapCongestionDtos"];
        NDDSLIB_Json::Value simplemapCctvDtos = root["simplemapCctvDtos"];

        JSON_ALLOC_MULITBYTE_STRING(bConverting, root, "baseSdttm",	res.baseSdttm,memory);
        JSON_ALLOC_MULITBYTE_STRING(bConverting, root, "smName",	res.smName,memory);
        JSON_ALLOC_BASE64(root, "simplemapEncodingImage",unsigned char*, res.simplemapEncodingImage,res.simplemapEncodingImageSize,memory);
        res.trafficCongestionSize = root["trafficCongestionSize"].asInt();
        res.simplemapCctvCount = root["simplemapCctvCount"].asInt();


        // - 소통정보 정보 리스트 (simplemapCongestionDtos)
        if(res.trafficCongestionSize > 0)
        {
            JSON_ALLOC_BLOCK(res.simplemapCongestionDtos, NDDSTMAP_SIMPLEMAPCONGESTION_DTO*, sizeof(NDDSTMAP_SIMPLEMAPCONGESTION_DTO) * res.trafficCongestionSize, memory);
            for(int i = 0;i<res.trafficCongestionSize;i++)

            {
                JSON_ALLOC_STRING(simplemapCongestionDtos[i], "smLinkId",	res.simplemapCongestionDtos[i].smLinkId,memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, simplemapCongestionDtos[i], "distributeName",	res.simplemapCongestionDtos[i].distributeName,memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, simplemapCongestionDtos[i], "stNodeName",	res.simplemapCongestionDtos[i].stNodeName,memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, simplemapCongestionDtos[i], "edNodeName",	res.simplemapCongestionDtos[i].edNodeName,memory);
                res.simplemapCongestionDtos[i].downtravelTime = simplemapCongestionDtos[i]["downtravelTime"].asInt();
                res.simplemapCongestionDtos[i].downLinkLen = simplemapCongestionDtos[i]["downLinkLen"].asUInt();
                JSON_ALLOC_MULITBYTE_STRING(bConverting, simplemapCongestionDtos[i], "downCongestion",	res.simplemapCongestionDtos[i].downCongestion,memory);
                res.simplemapCongestionDtos[i].uptravelTime = simplemapCongestionDtos[i]["uptravelTime"].asInt();
                res.simplemapCongestionDtos[i].upLinkLen = simplemapCongestionDtos[i]["upLinkLen"].asUInt();
                JSON_ALLOC_MULITBYTE_STRING(bConverting, simplemapCongestionDtos[i], "upCongestion",	res.simplemapCongestionDtos[i].upCongestion,memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, simplemapCongestionDtos[i], "cctvFlag",	res.simplemapCongestionDtos[i].cctvFlag,memory);
            }
        }

        // CCTV정보 리스트 (simplemapCctvDtos)
        if(res.simplemapCctvCount > 0)
        {
            JSON_ALLOC_BLOCK(res.simplemapCctvDtos, NDDSTMAP_SIMPLEMAPCCTV_DTO*, sizeof(NDDSTMAP_SIMPLEMAPCCTV_DTO) * res.simplemapCctvCount, memory);
            for(int i = 0;i<res.simplemapCctvCount;i++)
            {
                JSON_ALLOC_MULITBYTE_STRING(bConverting, simplemapCctvDtos[i], "smLinkId",	res.simplemapCctvDtos[i].smLinkId,memory);
                res.simplemapCctvDtos[i].subSeq = simplemapCctvDtos[i]["subSeq"].asInt();
                JSON_ALLOC_MULITBYTE_STRING(bConverting, simplemapCctvDtos[i], "cctvId",	res.simplemapCctvDtos[i].cctvId,memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, simplemapCctvDtos[i], "cctvName",	res.simplemapCctvDtos[i].cctvName,memory);
            }
        }
    }catch(...){
        return false;
    }
    *ppRes = (NDDSTMAP_SMIMAGETRAFFICCONGESTION_RES*)malloc(sizeof(NDDSTMAP_SMIMAGETRAFFICCONGESTION_RES));
    memcpy(*ppRes, &res, sizeof(NDDSTMAP_SMIMAGETRAFFICCONGESTION_RES));
    return true;
}

bool CNddsLibTmap::MakeSmimageTrafficCongestion( NDDSLIB_Json::Value &root, NDDSTMAP_SMIMAGETRAFFICCONGESTION_REQ *pReq )
{
    try{
        root["smId"] = pReq->smId;
        root["imageResolutionType"] = pReq->imageResolutionType;
        root["fileTypeType"] = pReq->fileTypeType;
    }catch(...){
        return false;
    }
    return true;
}

bool CNddsLibTmap::ParserRadiusTrafficInfomtaion(NDDS_COMMONHEADER_RES common_header, unsigned char *content,int nContentSize,NDDS_MEMORYSTATUS &memory, NDDSTMAP_RADIUSTRAFFICINFOMATION_RES **ppRes )
{
    NDDSTMAP_RADIUSTRAFFICINFOMATION_RES res;
    memset(&res, 0x00, sizeof(NDDSTMAP_RADIUSTRAFFICINFOMATION_RES));
    char *binaryBuff;

    if(atoi(common_header.errorCode) == 0)
    {
        try{
            ALLOC_STR_COPY(content,binaryBuff,nContentSize,memory);
            res.pTrafficData = (unsigned char *) binaryBuff;
            res.nSizeData = nContentSize;
        }catch(...){
            return false;
        }
    }
    *ppRes = (NDDSTMAP_RADIUSTRAFFICINFOMATION_RES*)malloc(sizeof(NDDSTMAP_RADIUSTRAFFICINFOMATION_RES));
    memcpy(*ppRes, &res, sizeof(NDDSTMAP_RADIUSTRAFFICINFOMATION_RES));

    return true;
}

bool CNddsLibTmap::MakeRadiusTrafficInfomtaion( NDDSLIB_Json::Value &root, NDDSTMAP_RADIUSTRAFFICINFOMATION_REQ *pReq )
{
    try{
        root["currentXPos"] = pReq->currentXPos;
        root["currentYPos"] = pReq->currentYPos;
        root["radius"] = pReq->radius;
    }catch(...){
        return false;
    }
    return true;
}

bool CNddsLibTmap::ParserFindTsdSudn( NDDSLIB_Json::Value &root,NDDS_MEMORYSTATUS &memory, NDDSTMAP_FINDTSDSUDN_RES **ppRes )
{
    CNddsLibCommonData &CommonData = CNddsLibCommonData::GetInstance();
    bool bConverting = CommonData.m_eCharacterSet == NDDSCHARACTERSET_EUCKR?true:false;

    NDDSTMAP_FINDTSDSUDN_RES res;
    NDDSLIB_Json::Value tsdSudnSudnStIdDtos = root["tsdSudnSudnStIdDtos"];
    memset(&res, 0x00, sizeof(NDDSTMAP_FINDTSDSUDN_RES));

    try{
        res.repeatCnt = root["repeatCnt"].asInt();

        if(res.repeatCnt > 0)
        {
            JSON_ALLOC_BLOCK(res.tsdSudnSudnStIdDtos, NDDSTMAP_TSDSUDNSUDNSTID_DTO*, sizeof(NDDSTMAP_TSDSUDNSUDNSTID_DTO) * res.repeatCnt, memory);
            for(int i=0;i<res.repeatCnt;i++)
            {
                JSON_ALLOC_STRING(tsdSudnSudnStIdDtos[i], "sudnStId", res.tsdSudnSudnStIdDtos[i].sudnStId, memory);
                JSON_ALLOC_STRING(tsdSudnSudnStIdDtos[i], "sudnStLargeCd", res.tsdSudnSudnStIdDtos[i].sudnStLargeCd, memory);
                JSON_ALLOC_STRING(tsdSudnSudnStIdDtos[i], "sudnStTypeCd", res.tsdSudnSudnStIdDtos[i].sudnStTypeCd, memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, tsdSudnSudnStIdDtos[i], "sudnStRoad", res.tsdSudnSudnStIdDtos[i].sudnStRoad, memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, tsdSudnSudnStIdDtos[i], "sudnStSection", res.tsdSudnSudnStIdDtos[i].sudnStSection, memory);
                res.tsdSudnSudnStIdDtos[i].sudnStContLen = tsdSudnSudnStIdDtos[i]["sudnStContLen"].asInt();
                JSON_ALLOC_MULITBYTE_STRING(bConverting, tsdSudnSudnStIdDtos[i], "sudnStCont", res.tsdSudnSudnStIdDtos[i].sudnStCont, memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, tsdSudnSudnStIdDtos[i], "controlStSdttm", res.tsdSudnSudnStIdDtos[i].controlStSdttm, memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, tsdSudnSudnStIdDtos[i], "controlEdSdttm", res.tsdSudnSudnStIdDtos[i].controlEdSdttm, memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, tsdSudnSudnStIdDtos[i], "laneNo", res.tsdSudnSudnStIdDtos[i].laneNo, memory);
                res.tsdSudnSudnStIdDtos[i].occurXPos = tsdSudnSudnStIdDtos[i]["occurXPos"].asUInt();
                res.tsdSudnSudnStIdDtos[i].occurYPos = tsdSudnSudnStIdDtos[i]["occurYPos"].asUInt();
            }
        }
    }catch(...){
        return false;
    }

    *ppRes = (NDDSTMAP_FINDTSDSUDN_RES*)malloc(sizeof(NDDSTMAP_FINDTSDSUDN_RES));
    memcpy(*ppRes, &res, sizeof(NDDSTMAP_FINDTSDSUDN_RES));
    return true;
}

bool CNddsLibTmap::MakeFindTsdSudn( NDDSLIB_Json::Value &root,NDDSTMAP_FINDTSDSUDN_REQ *pReq )
{
    return true;
}

bool CNddsLibTmap::ParserNonNdds(eNddsProtocolType eProtocolType, char *resData, int contentLength, NDDS_COMMON_RES **ppResStruct, NDDS_MEMORYSTATUS &memory)
{
    bool bRet = true;
//    NDDS_COMMONHEADER_RES common_header;
//    memset(&common_header, 0x00, sizeof(NDDS_COMMONHEADER_RES));

    m_pSessionData->nProgress = 87;
//	DBGPRINT(_T("------------------------- %d\n"), m_pSessionData->nProgress);
    delay(TIME_DELAY);

    bRet = ParserNonNddsResult(resData, contentLength, memory, (NDDSTMAP_NONNDDS_RES**)ppResStruct);

    if(!bRet)
    {
        NDDSTMAP_COMMON_RES *pCommonRes = (NDDSTMAP_COMMON_RES*)(*ppResStruct);
//        memcpy(&pCommonRes->common.header, &common_header, sizeof(NDDS_COMMONHEADER_RES));
        m_eLastErrCode = NDDSERR_RES_BODY;
        return false;
    }

    m_pSessionData->nProgress = 93;
//	DBGPRINT(_T("------------------------- %d\n"), m_pSessionData->nProgress);

    // UI에 전달될 구조체에 공통 해더 및 메모리 상태 값 갱신
    NDDSTMAP_COMMON_RES *pCommonRes = (NDDSTMAP_COMMON_RES*)(*ppResStruct);
//    memcpy(&pCommonRes->common.header, &common_header, sizeof(NDDS_COMMONHEADER_RES));
    memcpy(&pCommonRes->common.memory, &memory, sizeof(NDDS_MEMORYSTATUS));

//    if(atoi(common_header.errorCode) != 0)
//    {
//        m_eLastErrCode = NDDSERR_RES_FAIL;
//        return false;
//    }
    return true;
}

bool CNddsLibTmap::ParserJson(eNddsProtocolType eProtocolType,char *szJson,int jsonLength, NDDS_COMMON_RES **ppResStruct, NDDS_MEMORYSTATUS &memory)
{
    CNddsLibCommonData &CommonData = CNddsLibCommonData::GetInstance();
    bool bConverting = CommonData.m_eCharacterSet == NDDSCHARACTERSET_EUCKR?true:false;

    bool bRet;
    NDDSLIB_Json::Reader reader;
    NDDSLIB_Json::Value root, header, element;
    NDDSLIB_Json::StyledWriter writer;
    NDDS_COMMONHEADER_RES common_header;
    memset(&common_header, 0x00, sizeof(NDDS_COMMONHEADER_RES));

    if(!reader.parse(szJson, root))
    {
        MakeErrorRes(memory,(NDDS_ERROR_RES **)ppResStruct);
        m_eLastErrCode = NDDSERR_JSONSENTENCE;
        return false;
    }
    m_pSessionData->nProgress = 85;
//	DBGPRINT(_T("------------------------- %d\n"), m_pSessionData->nProgress);
    delay(TIME_DELAY);

    // 공통 해더 파싱
    header = root["header"];
    if(header.empty())
    {
        MakeErrorRes(memory,(NDDS_ERROR_RES **)ppResStruct);
        m_eLastErrCode = NDDSERR_RES_COMMONHEADER;
        return false;
    }

    JSON_ALLOC_STRING(header, "errorCode", common_header.errorCode, memory);
    JSON_ALLOC_MULITBYTE_STRING(bConverting, header, "errorMessage", common_header.errorMessage, memory);
    JSON_ALLOC_STRING(header, "errorDetailCode", common_header.errorDetailCode, memory);
    JSON_ALLOC_MULITBYTE_STRING(bConverting, header, "errorDetailMessage", common_header.errorDetailMessage, memory);
    JSON_ALLOC_STRING(header, "sessionId", common_header.sessionId, memory);

    std::string output = writer.write( root );
    printf("=======================================\n");
    printf("===ParserJson==========================\n");
    printf("=======================================\n");
    printf("Json %s\n", output.c_str());
    printf("=======================================\n");

    m_pSessionData->nProgress = 87;
//	DBGPRINT(_T("------------------------- %d\n"), m_pSessionData->nProgress);
    delay(TIME_DELAY);

    // 응답 데이터 파싱
    bRet = true;
    switch(eProtocolType)
    {
        case NDDSTYPE_JSON_STRING:
            bRet = ParserResult(common_header,(unsigned char*)szJson, jsonLength,memory,(NDDS_JSON_RES **)ppResStruct);
            break;
        case NDDSTYPE_TMAP_ROUTESUMMARYINFO:		// 5.1.4  경로요약
            bRet = ParserRouteSummaryInfo(root, memory, (NDDSTMAP_ROUTESUMMARYINFO_RES**)ppResStruct);
            break;
        case NDDSTYPE_TMAP_FINDPOIS:					// 5.2.1  통합검색
            bRet = ParserFindPois(root, memory, (NDDSTMAP_FINDPOIS_RES**)ppResStruct);
            break;
        case NDDSTYPE_TMAP_FINDAROUNDPOIS:			// 5.2.2  주변지 검색
            bRet = ParserFindAroundPois(root, memory, (NDDSTMAP_FINDAROUNDPOIS_RES**)ppResStruct);
            break;
        case NDDSTYPE_TMAP_FINDPOIDETAILINFO:		// 5.2.3  POI 상세정보
            bRet = ParserFindPoiDetailInfos(root, memory, (NDDSTMAP_FINDPOIDETAILINFO_RES**)ppResStruct);
            break;
        case NDDSTYPE_TMAP_FINDPOIJOININFO:			// 5.2.4. 가맹점 상세정보
            bRet = ParserFindPoiJoinInfo(root, memory, (NDDSTMAP_FINDPOIJOININFO_RES**)ppResStruct);
            break;
        case NDDSTYPE_TMAP_FINDRECENTDESTNATION:		// 5.2.5. POI 최근길 조회
            bRet = ParserFindRecentDestnation(root, memory, (NDDSTMAP_FINDRECENTDESTNATION_RES**)ppResStruct);
            break;
        case NDDSTYPE_TMAP_UPLOADRECENTDESTNATION:	// 5.2.6. POI 최근길 전체 업로드
            bRet = ParserUploadRecentDestnation(root, memory, (NDDSTMAP_UPLOADRECENTDESTNATION_RES**)ppResStruct);
            break;
        case NDDSTYPE_TMAP_REMOVERECENTDESTNATION: // 5.2.7 POI 최근길 멀티 삭제
            bRet = ParserRemoveRecentDestnation(root, memory, (NDDSTMAP_REMOVERECENTDESTNATION_RES**)ppResStruct);
            break;
            /*
        case NDDSTYPE_TMAP_FINDNEARBY:				// 5.2.7. 좌표로 지명 검색
            bRet = ParserFindNearBy(root, memory, (NDDSTMAP_FINDNEARBY_RES **)ppResStruct);
            break;
        case NDDSTYPE_TMAP_FINDPICKATPOPULARS:		// 5.2.8. Pickat 인기검색어 조회
            bRet = ParserFindPickatPopulars(root, memory, (NDDSTMAP_FINDPICKATPOPULARS_RES **)ppResStruct);
            break;
        case NDDSTYPE_TMAP_FINDROADNAME:		// 5.2.9. 새주소(도로명) 검색
            bRet = ParserFindRoadName(root, memory, (NDDSTMAP_FINDROADNAME_RES **)ppResStruct);
            break;
        case NDDSTYPE_TMAP_FINDPICKATPOIDETAIL:		// 5.2.10. Pickat POI 상세정보 조회
            bRet = ParserFindPickatPoiDetail(root, memory, (NDDSTMAP_FINDPICKATPOIDETAIL_RES **)ppResStruct);
            break;
            */
        case NDDSTYPE_TMAP_FINDAREANAMESBYSTEP:		// 5.2.11. 지역분류별 주소명 조회
            bRet = ParserFindAreaNamesByStep(root, memory, (NDDSTMAP_FINDAREANAMESBYSTEP_RES**)ppResStruct);
            break;
        case NDDSTYPE_TMAP_FINDPOISTATIONINFO:		// 5.2.12. 최저가 주유소(충전소)조회
            bRet = ParserFindPoiStationInfo(root, memory, (NDDSTMAP_FINDPOISTATIONINFO_RES **)ppResStruct);
            break;
        case NDDSTYPE_TMAP_FINDPOIEVSTATIONINFO:	// 5.2.16. ev충전소 상세정보 조회
            bRet = ParserFindPoiEvStationInfo(root, memory, (NDDSTMAP_FINDPOIEVSTATIONINFO_RES **)ppResStruct);
            break;
        case NDDSTYPE_TMAP_FINDPOIEVSTATIONSBYPOINT:	// 5.2.18. 주변 전기차 충전소 조회
            bRet = ParserFindPoiEvStationsByPoint(root, memory, (NDDSTMAP_FINDPOIEVSTATIONSBYPOINT_RES **)ppResStruct);
            break;
        case NDDSTYPE_TMAP_FINDPOIFAVORITE:			// 5.3.1. POI 즐겨찾기 조회
            bRet = ParserFindPoiFavorite(root, memory, (NDDSTMAP_FINDPOIFAVORITE_RES**)ppResStruct);
            break;
        case NDDSTYPE_TMAP_REGISTPOIFAVORITEMULTI:	// 5.3.2. POI 즐겨찾기 등록
            bRet = ParserRegistPoiFavoriteMulti(root, memory, (NDDSTMAP_REGISTPOIFAVORITEMULTI_RES**)ppResStruct);
            break;
        case NDDSTYPE_TMAP_UPLOADPOIFAVORITE:		// 5.3.3. POI 즐겨찾기 전체 업로드
            bRet = ParserRegistAllPoiFavorite(root, memory, (NDDSTMAP_REGISTALLPOIFAVORITE_RES**)ppResStruct);
            break;
        case NDDSTYPE_TMAP_REMOVEPOIFAVORITE:		// 5.3.3. POI 즐겨찾기 멀티삭제
            bRet = ParserRemovePoiFavorites(root, memory, (NDDSTMAP_REMOVEPOIFAVORITE_RES**)ppResStruct);
            break;
        case NDDSTYPE_TMAP_MODIFYPOIFAVORITE:		// 5.3.3. POI 즐겨찾기 단건수정
             bRet = ParserModifyPoiFavorite(root, memory, (NDDSTMAP_MODIFYPOIFAVORITE_RES**)ppResStruct);
             break;
        case NDDSTYPE_TMAP_FINDUSERPROFILE:			//5.3.15. User-Profile 조회
            bRet = ParserFindUserProfile(root, memory, (NDDSTMAP_FINDUSERPROFILE_RES**)ppResStruct);
            break;
        case NDDSTYPE_TMAP_REGISTUSERPROFILE:		//5.3.15. User-Profile 저장
            bRet = ParserRegistUserProfile(root, memory, (NDDSTMAP_REGISTUSERPROFILE_RES**)ppResStruct);
            break;
        case NDDSTYPE_TMAP_FINDCARPROFILE:			//5.3.15. Car-Profile 조회
            bRet = ParserFindCarProfile(root, memory, (NDDSTMAP_FINDCARPROFILE_RES**)ppResStruct);
            break;
        case NDDSTYPE_TMAP_REGISTCARPROFILE:		//5.3.15. Car-Profile 저장
            bRet = ParserRegistCarProfile(root, memory, (NDDSTMAP_REGISTCARPROFILE_RES**)ppResStruct);
            break;
        case NDDSTYPE_TMAP_FINDUSERDATA:				// 5.3.14. 최근길/즐겨찾기 전체 리스트 조회
            bRet = ParserFindUserData(root, memory, (NDDSTMAP_FINDUSERDATA_RES **)ppResStruct);
            break;
        /*
        case NDDSTYPE_TMAP_FINDROUTE:				// 5.3.4  경로 즐겨찾기 조회
            bRet = ParserFindRoute(root, memory, (NDDSTMAP_FINDROUTE_RES**)ppResStruct);
            break;
        case NDDSTYPE_TMAP_REGISTROUTEMULTI:			// 5.3.5. 경로 즐겨찾기 등록
            bRet = ParserRegistRouteMulti(root, memory, (NDDSTMAP_REGISTROUTEMULTI_RES  **)ppResStruct);
            break;
        case NDDSTYPE_TMAP_UPLOADTOTALROUTE:			// 5.3.6. 경로 즐겨찾기 전체 업로드
            bRet = ParserUploadTotalRoute(root, memory, (NDDSTMAP_UPLOADTOTALROUTE_RES **)ppResStruct);
            break;
        case NDDSTYPE_TMAP_FINDTRAFFICFAVORITES:		// 5.3.7. 교통정보 즐겨찾기 조회
            bRet = ParserFindTrafficFavorites(root, memory, (NDDSTMAP_FINDTRAFFICFAVORITES_RES **)ppResStruct);
            break;
        case NDDSTYPE_TMAP_REGISTTRAFFICFAVORITES:	// 5.3.8. 교통정보 즐겨찾기 등록
            bRet = ParserRegistTrafficFavorites(root, memory, (NDDSTMAP_REGISTTRAFFICFAVORITES_RES **)ppResStruct);
            break;
        case NDDSTYPE_TMAP_UPLOADTRAFFICFAVORITES:	// 5.3.9. 교통정보 즐겨찾기 전체 업로드
            bRet = ParserUploadTrafficFavorites(root, memory, (NDDSTMAP_UPLOADTRAFFICFAVORITES_RES **)ppResStruct);
            break;
        case NDDSTYPE_TMAP_REMOVETRAFFICFAVORITE:	// 5.3.10. 교통정보 즐겨찾기 단일건 삭제
            bRet = ParserRemoveTrafficFavorites(root, memory, (NDDSTMAP_REMOVETRAFFICFAVORITE_RES **)ppResStruct);
            break;
        case NDDSTYPE_TMAP_FINDNODEJAMRANK:			// 5.4.1. 상습 정체구간 조회
            bRet = ParserFindNodeJamRank(root, memory, (NDDSTMAP_FINDNODEJAMRANK_RES **)ppResStruct);
            break;
        case NDDSTYPE_TMAP_SMIMAGETRAFFICCONGESTION:	// 5.4.2. 이미지 교통정보
            bRet = ParserSmimageTrafficCongestio(root, memory, (NDDSTMAP_SMIMAGETRAFFICCONGESTION_RES **)ppResStruct);
            break;
        case NDDSTYPE_TMAP_FINDTSDSUDN:				// 5.4.4. 돌발 정보 조회
            bRet = ParserFindTsdSudn(root, memory, (NDDSTMAP_FINDTSDSUDN_RES **)ppResStruct);
            break;
            */
        case NDDSTYPE_TMAP_VMSGUIDEINFO:
            bRet = ParserVmsGuideInfo(root,memory,(NDDSTMAP_VMSGUIDEINFO_RES **)ppResStruct);
            break;
            /*
        case NDDSTYPE_TMAP_FINDBUSSTATION:				// 5.5.1. 버스 정류장 정보(도착 시간)
            bRet = ParserFindBusStation(root,memory,(NDDSTMAP_FINDBUSSTATION_RES **)ppResStruct);
            break;
        case NDDSTYPE_TMAP_FINDBUSARRIVAL:				// 5.5.2. 버스 노선 정보(도착 정보)
            bRet = ParserFindBusArrival(root,memory,(NDDSTMAP_FINDBUSARRIVAL_RES **)ppResStruct);
            break;
        case NDDSTYPE_TMAP_FINDBUSARRIVALTIME:			// 5.5.3. 버스 노선 정보(도착 시간)
            bRet = ParserFindBusArrivalTime(root,memory,(NDDSTMAP_FINDBUSARRIVALTIME_RES **)ppResStruct);
            break;
        case NDDSTYPE_TMAP_FINDBUSSTATIONREALTIME:		// 5.5.4. 버스 정류장/노선 실시간 정보
            bRet = ParserFindBusStationRealTime(root,memory,(NDDSTMAP_FINDBUSSTATIONREALTIME_RES **)ppResStruct);
            break;
        case NDDSTYPE_TMAP_FINDSTATIONINFO:				// 5.5.5. 역 정보
            bRet = ParserFindStationInfo(root,memory,(NDDSTMAP_FINDSTATIONINFO_RES **)ppResStruct);
            break;
        case NDDSTYPE_TMAP_FINDSTATIONTIMETABLE:			// 5.5.6. 역 운행 정보
            bRet = ParserFindStationTimeTable(root,memory,(NDDSTMAP_FINDSTATIONTIMETABLE_RES **)ppResStruct);
            break;
        case NDDSTYPE_TMAP_FINDSUBWAYIMAGE:				// 5.5.7. 노선 이미지
            bRet = ParserFindSubwayImage(root,memory,(NDDSTMAP_FINDSUBWAYIMAGE_RES **)ppResStruct);
            break;
        case NDDSTYPE_TMAP_FINDPATHFIND1STALL4:			// 5.5.8. 경로안내(도시간 검색 포함)
            bRet = ParserFindPathFind1stAll4(root,memory,(NDDSTMAP_FINDPATHFIND1STALL4_RES **)ppResStruct);
            break;
        case NDDSTYPE_TMAP_FINDPATHFIND1STALL4PLURAL:	// 5.5.9. 경로안내(도시내 복수 검색)
            bRet = ParserFindPathFind1stAll4Plural(root,memory,(NDDSTMAP_FINDPATHFIND1STALL4PLURAL_RES **)ppResStruct);
            break;
        case NDDSTYPE_TMAP_FINDNEARSTATION:				// 5.5.10. 주변 정류장 / 역 조회(거리순)
            bRet = ParserFindNearStation(root,memory,(NDDSTMAP_FINDNEARSTATION_RES **)ppResStruct);
            break;
        case NDDSTYPE_TMAP_FINDALLTRANSPORT:				// 5.5.11. 통합 검색
            bRet = ParserFindAllTransport(root,memory,(NDDSTMAP_FINDALLTRANSPORT_RES **)ppResStruct);
            break;
        case NDDSTYPE_TMAP_FINDCCTVRESULT:				// 5.5.12. 주변 CCTV 리스트
            bRet = ParserFindCCTVResult(root,memory,(NDDSTMAP_FINDCCTVRESULT_RES **)ppResStruct);
            break;
        case NDDSTYPE_TMAP_FINDCCTVRESULTBYID:			// 5.5.13. CCTV 정보
            bRet = ParserFindCCTVResultById(root,memory,(NDDSTMAP_FINDCCTVRESULTBYID_RES **)ppResStruct);
            break;
            */
        case NDDSTYPE_TMAP_AUTHFORSMARTCAR:
            bRet = ParserAuthForSmartCar(root,memory,(NDDSTMAP_AUTHFORSMARTCAR_RES **)ppResStruct);
            break;
            /*
        case NDDSTYPE_TMAP_POSITIONSHARING:						// 5.6.9. 위치 공유
            bRet = ParserPositionSharing(root,memory,(NDDSTMAP_POSITIONSHARING_RES **)ppResStruct);
            break;
        case NDDSTYPE_TMAP_ROUTESHARINGCAR:						// 5.6.10. 경로공유 - 자동차
            bRet = ParserRouteSharingCar(root,memory,(NDDSTMAP_ROUTESHARINGCAR_RES **)ppResStruct);
            break;
        case NDDSTYPE_TMAP_ROUTESHARINGPUBLIC:					// 5.6.11. 경로공유 - 대중교통
            bRet = ParserRouteSharingPublic(root,memory,(NDDSTMAP_ROUTESHARINGPUBLIC_RES **)ppResStruct);
            break;
        case NDDSTYPE_TMAP_ROUTESHARINGWALK:						// 5.6.12. 경로공유 - 도보
            bRet = ParserRouteSharingWalk(root,memory,(NDDSTMAP_ROUTESHARINGWALK_RES **)ppResStruct);
            break;
        case NDDSTYPE_TMAP_ROUTESHARINGDRIVING:					// 5.6.13. 경로공유 - 주행경로
            bRet = ParserRouteSharingDriving(root,memory,(NDDSTMAP_ROUTESHARINGDRIVING_RES **)ppResStruct);
            break;
            */
        case NDDSTYPE_TMAP_REGISTERSENDTOCARINFO:					// 5.6.19. 차량 전송 정보 저장
            bRet = ParserRegisterSendToCarInfo(root,memory,(NDDSTMAP_REGISTERSENDTOCARINFO_RES **)ppResStruct);
            break;
        case NDDSTYPE_TMAP_FINDSENDTOCARINFO:					// 5.6.20. 차량 전송 정보 조회
            bRet = ParserFindSendToCarInfo(root,memory,(NDDSTMAP_FINDSENDTOCARINFO_RES **)ppResStruct);
            break;
        case NDDSTYPE_TMAP_REMOVESENDTOCARINFO:					// 5.6.21. 차량 전송 정보 삭제
            bRet = ParserRemoveSendToCarInfo(root,memory,(NDDSTMAP_REMOVESENDTOCARINFO_RES **)ppResStruct);
            break;
            /*

        case NDDSTYPE_TMAP_FINDCHARGENOTICES:			// 5.7.1  공지사항(요금제)
            bRet = ParserFindChargeNotices(root, memory, (NDDSTMAP_FINDCHARGENOTICES_RES **)ppResStruct);
            break;
        case NDDSTYPE_TMAP_FINDFILEACCESS:				// 5.7.2  파일(이미지,기타) 정보
            bRet = ParserFindFileAccess(root, memory, (NDDSTMAP_FINDFILEACCESS_RES**)ppResStruct);
            break;
        case NDDSTYPE_TMAP_FINDOPENAPPS:					// 5.7.4  오픈 앱 리스트
            bRet = ParserFindOpenApps(root, memory, (NDDSTMAP_FINDOPENAPPS_RES**)ppResStruct);
            break;
        case NDDSTYPE_TMAP_FINDSEEDKEYINFO:				// 5.7.9  Seed Key 정보 요청
            bRet = ParserFindSeedKeyInfo(root, memory, (NDDSTMAP_FINDSEEDKEYINFO_RES**)ppResStruct, &m_pSessionData->sCommonHeader);
            break;

        case NDDSTYPE_TMAP_SAVEADVERTISE:					// 5.8.2. 광고 음원 노출 보고
            bRet = ParserSaveAdvertise(root,memory,(NDDSTMAP_SAVEADVERTISE_RES **)ppResStruct);
            break;
        case NDDSTYPE_TMAP_FINDAGREEMENT:					// 5.9.1. 이용약관 동의 여부 조회
            bRet = ParserFindAgreement(root,memory,(NDDSTMAP_FINDAGREEMENT_RES **)ppResStruct);
            break;
        case NDDSTYPE_TMAP_REGISTAGREEMENT:					// 5.9.2. 이용약관 동의 결과 저장
            bRet = ParserRegistAgreement(root,memory,(NDDSTMAP_REGISTAGREEMENT_RES **)ppResStruct);
            break;
        case NDDSTYPE_TMAP_APPLOGSAVE:
            bRet = ParserAppLogSave(root,memory,(NDDSTMAP_APPLOGSAVE_RES **)ppResStruct);
            break;
        case NDDSTYPE_TMAP_ROUTEHISTORYSAVE:
            bRet = ParserRouteHistorySave(root,memory,(NDDSTMAP_ROUTEHISTORYSAVE_RES **)ppResStruct);
            break;
            */
        default:
            // 무조건 구조체를 생성해야 한다.
            assert(0);
            //		*ppResStruct = malloc(sizeof(NDDSTMAP_COMMON_RES));
            break;
    }

    if(!bRet)
    {
        NDDSTMAP_COMMON_RES *pCommonRes = (NDDSTMAP_COMMON_RES*)(*ppResStruct);
        memcpy(&pCommonRes->common.header, &common_header, sizeof(NDDS_COMMONHEADER_RES));
        //free(memory.pMemory);
        m_eLastErrCode = NDDSERR_RES_BODY;
        return false;
    }
    m_pSessionData->nProgress = 93;
//	DBGPRINT(_T("------------------------- %d\n"), m_pSessionData->nProgress);
    delay(TIME_DELAY);

    // UI에 전달될 구조체에 공통 해더 및 메모리 상태 값 갱신
    NDDSTMAP_COMMON_RES *pCommonRes = (NDDSTMAP_COMMON_RES*)(*ppResStruct);
    memcpy(&pCommonRes->common.header, &common_header, sizeof(NDDS_COMMONHEADER_RES));
    memcpy(&pCommonRes->common.memory, &memory, sizeof(NDDS_MEMORYSTATUS));

    if(atoi(common_header.errorCode) != 0)
    {
        m_eLastErrCode = NDDSERR_RES_FAIL;
        return false;
    }
    return true;
}

bool CNddsLibTmap::ParserBinary( eNddsProtocolType eProtocolType,unsigned char *binaryBuf,int bufSize,NDDS_COMMON_RES **ppResStruct,NDDS_MEMORYSTATUS &memory )
{
    CNddsLibCommonData &CommonData = CNddsLibCommonData::GetInstance();
    // 바이너리 포멧은 EUC-KR이 기본값임
    bool bConverting = CommonData.m_eCharacterSet != NDDSCHARACTERSET_EUCKR?true:false;

    int nPos = 0;
    bool bRet = false;
    NDDS_COMMONHEADER_RES common_header;

    memset(&common_header, 0x00, sizeof(NDDS_COMMONHEADER_RES));

    ALLOC_STR_COPY(binaryBuf,common_header.errorCode,NDDS_BINARY_ERROR_CODE_LEN,memory);
    nPos=NDDS_BINARY_ERROR_CODE_LEN;
    ALLOC_STR_UTF8_COPY(bConverting, (binaryBuf+nPos),common_header.errorMessage,NDDS_BINARY_ERROR_MESSAGE_LEN,memory);
    nPos+=NDDS_BINARY_ERROR_MESSAGE_LEN;
    ALLOC_STR_COPY(binaryBuf+nPos,common_header.errorDetailCode,NDDS_BINARY_ERROR_DETAIL_CODE_LEN,memory);
    nPos+=NDDS_BINARY_ERROR_DETAIL_CODE_LEN;
    ALLOC_STR_UTF8_COPY(bConverting, (binaryBuf+nPos),common_header.errorDetailMessage,NDDS_BINARY_ERROR_MESSAGE_LEN,memory);
    nPos+=NDDS_BINARY_ERROR_MESSAGE_LEN;
    ALLOC_STR_COPY(binaryBuf+nPos,common_header.sessionId,NDDS_BINARY_SESSION_ID_LEN,memory);
    nPos+=NDDS_BINARY_SESSION_ID_LEN;

    m_pSessionData->nProgress = 83;
//	DBGPRINT(_T("------------------------- %d\n"), m_pSessionData->nProgress);
    delay(TIME_DELAY);


        switch(eProtocolType)
        {
            case NDDSTYPE_JSON_STRING:
                bRet = ParserResult(common_header,binaryBuf+nPos,bufSize,memory,(NDDS_JSON_RES **)ppResStruct);
                break;
            case NDDSTYPE_TMAP_PLANNINGROUTE:
                bRet = ParserPlanningRoute(common_header,binaryBuf+nPos,bufSize,memory,(NDDSTMAP_PLANNINGROUTE_RES **)ppResStruct);
                break;
                /* // 소스 검증 안됨
            case NDDSTYPE_TMAP_RADIUSTRAFFICINFOMATION:
                bRet = ParserRadiusTrafficInfomtaion(common_header,binaryBuf+nPos,bufSize,memory,(NDDSTMAP_RADIUSTRAFFICINFOMATION_RES **)ppResStruct);
                break;
                */
        }


    if(!bRet)
    {
        NDDSTMAP_COMMON_RES *pCommonRes = (NDDSTMAP_COMMON_RES*)(*ppResStruct);
        memcpy(&pCommonRes->common.header, &common_header, sizeof(NDDS_COMMONHEADER_RES));
        //free(memory.pMemory);
        m_eLastErrCode = NDDSERR_RES_BODY;
        return false;
    }
    m_pSessionData->nProgress = 93;
//	DBGPRINT(_T("------------------------- %d\n"), m_pSessionData->nProgress);
    delay(TIME_DELAY);

    // UI에 전달될 구조체에 공통 해더 및 메모리 상태 값 갱신
    NDDSTMAP_COMMON_RES *pCommonRes = (NDDSTMAP_COMMON_RES*)(*ppResStruct);
    memcpy(&pCommonRes->common.header, &common_header, sizeof(NDDS_COMMONHEADER_RES));
    memcpy(&pCommonRes->common.memory, &memory, sizeof(NDDS_MEMORYSTATUS));


    if(atoi(common_header.errorCode) != 0)
    {
        m_eLastErrCode = NDDSERR_RES_FAIL;
        return false;
    }
    return true;
}

bool CNddsLibTmap::CheckServiceClose(NDDS_PROTOCOL_SESSION_DATA *pSessionData)
{
    if(pSessionData->eProtocolType == NDDSTYPE_TMAP_PLANNINGROUTE)
    {
        if(((NDDSTMAP_PLANNINGROUTE_RES *)pSessionData->pResultStruct)->roadCount > 0)
        {
            return false;
        }
    }

    if(pSessionData->eProtocolType == NDDSTYPE_JSON_STRING)
    {
        NDDS_HTTP_HEADERDATA *pHttpHeader = &pSessionData->sHttpHeader;

        if(strcmp(pHttpHeader->uri, "/tmap-channel/rsd/route/planningroute") == 0)
        {
            int roadCount;
            NDDS_JSON_RES* jsonRes = (NDDS_JSON_RES *)pSessionData->pResultStruct;
            JUST_READ_INTEGER_DATA(jsonRes->pRespData, roadCount);
            if(roadCount > 0)
                return false;
        }
        else if(strcmp(pHttpHeader->uri, "/tmap-channel/rsd/route/planningroutemultiformat") == 0)
        {
            int roadCount;
            NDDS_JSON_RES* jsonRes = (NDDS_JSON_RES *)pSessionData->pResultStruct;
            JUST_READ_INTEGER_DATA(jsonRes->pRespData, roadCount);
            if(roadCount > 1)
                return false;
        }
    }

    return true;
}

// 5.4.5 VMS 가이드 정보 조회
bool CNddsLibTmap::MakeVmsGuideInfo( NDDSLIB_Json::Value &root, NDDSTMAP_VMSGUIDEINFO_REQ *pReq )
{
    try
    {
        root["rseId"] = pReq->rseId;
    }catch (...)
    {
        return false;
    }

    return true;
}

// 5.4.5 VMS 가이드 정보 조회
bool CNddsLibTmap::ParserVmsGuideInfo( NDDSLIB_Json::Value &root,NDDS_MEMORYSTATUS &memory,NDDSTMAP_VMSGUIDEINFO_RES **ppRes )
{
    CNddsLibCommonData &CommonData = CNddsLibCommonData::GetInstance();
    bool bConverting = CommonData.m_eCharacterSet == NDDSCHARACTERSET_EUCKR?true:false;

    NDDSTMAP_VMSGUIDEINFO_RES res;

    memset(&res, 0x00, sizeof(NDDSTMAP_VMSGUIDEINFO_RES));

    try{
        JSON_ALLOC_STRING(root, "baseSdttm", res.baseSdttm, memory);
        JSON_ALLOC_STRING(root, "dataSource", res.dataSource, memory);
        JSON_ALLOC_STRING(root, "contentCode", res.contentCode, memory);
        JSON_ALLOC_STRING(root, "contentName", res.contentName, memory);
        JSON_ALLOC_STRING(root, "content", res.content, memory);
    }catch(...){
        return false;
    }

    *ppRes = (NDDSTMAP_VMSGUIDEINFO_RES*)malloc(sizeof(NDDSTMAP_VMSGUIDEINFO_RES));
    memcpy(*ppRes, &res, sizeof(NDDSTMAP_VMSGUIDEINFO_RES));
    return true;
}


bool CNddsLibTmap::MakeFindBusStation( NDDSLIB_Json::Value &root,NDDSTMAP_FINDBUSSTATION_REQ *pReq )
{
    try{
        root["sid"] = pReq->sid;
        root["cid"] = pReq->cid;
    }catch(...){
        return false;
    }

    return true;
}

bool CNddsLibTmap::ParserFindBusStation( NDDSLIB_Json::Value &root,NDDS_MEMORYSTATUS &memory, NDDSTMAP_FINDBUSSTATION_RES **ppRes )
{
    CNddsLibCommonData &CommonData = CNddsLibCommonData::GetInstance();
    bool bConverting = CommonData.m_eCharacterSet == NDDSCHARACTERSET_EUCKR?true:false;

    NDDSTMAP_FINDBUSSTATION_RES res;
    NDDSLIB_Json::Value laneInfos = root["laneInfos"];
    memset(&res, 0x00, sizeof(NDDSTMAP_FINDBUSSTATION_RES));

    try{
        res.laneCount = root["laneCount"].asInt();

        if(res.laneCount > 0)
        {
            JSON_ALLOC_BLOCK(res.laneInfos, NDDSTMAP_LANEINFOS_DTO*, sizeof(NDDSTMAP_LANEINFOS_DTO) * res.laneCount, memory);
            for(int laneIdx =0;laneIdx<res.laneCount;laneIdx++)
            {
                JSON_ALLOC_STRING(laneInfos[laneIdx], "busNo", res.laneInfos[laneIdx].busNo, memory);
                res.laneInfos[laneIdx].type = laneInfos[laneIdx]["type"].asInt();
                JSON_ALLOC_STRING(laneInfos[laneIdx], "blid", res.laneInfos[laneIdx].blid, memory);
                JSON_ALLOC_STRING(laneInfos[laneIdx], "blid_rt", res.laneInfos[laneIdx].blid_rt, memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, laneInfos[laneIdx], "staOrd", res.laneInfos[laneIdx].staOrd, memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, laneInfos[laneIdx], "laneInfo", res.laneInfos[laneIdx].laneInfo, memory);
                res.laneInfos[laneIdx].arriveCount = laneInfos[laneIdx]["arriveCount"].asInt();

                if(res.laneInfos[laneIdx].arriveCount > 0)
                {
                    NDDSLIB_Json::Value busStationArrives = laneInfos[laneIdx]["busStationArrives"];
                    JSON_ALLOC_BLOCK(res.laneInfos[laneIdx].busStationArrives, NDDSTMAP_BUSSTATIONARRIVES_DTO*, sizeof(NDDSTMAP_BUSSTATIONARRIVES_DTO) * res.laneInfos[laneIdx].arriveCount, memory);
                    for(int stationIdx=0;stationIdx<res.laneInfos[laneIdx].arriveCount;stationIdx++)
                    {
                        JSON_ALLOC_MULITBYTE_STRING(bConverting, busStationArrives[stationIdx], "busPlainNo",	res.laneInfos[laneIdx].busStationArrives[stationIdx].busPlainNo, memory);
                        JSON_ALLOC_MULITBYTE_STRING(bConverting, busStationArrives[stationIdx], "isArrive",	res.laneInfos[laneIdx].busStationArrives[stationIdx].isArrive, memory);
                        JSON_ALLOC_MULITBYTE_STRING(bConverting, busStationArrives[stationIdx], "sectOrd",		res.laneInfos[laneIdx].busStationArrives[stationIdx].sectOrd, memory);
                        JSON_ALLOC_MULITBYTE_STRING(bConverting, busStationArrives[stationIdx], "traTime",		res.laneInfos[laneIdx].busStationArrives[stationIdx].traTime, memory);
                    }
                }
            }
        }
    }catch(...){
        return false;
    }

    *ppRes = (NDDSTMAP_FINDBUSSTATION_RES*)malloc(sizeof(NDDSTMAP_FINDBUSSTATION_RES));
    memcpy(*ppRes, &res, sizeof(NDDSTMAP_FINDBUSSTATION_RES));
    return true;
}


bool CNddsLibTmap::ParserFindBusArrival( NDDSLIB_Json::Value &root,NDDS_MEMORYSTATUS &memory, NDDSTMAP_FINDBUSARRIVAL_RES **ppRes )
{
    CNddsLibCommonData &CommonData = CNddsLibCommonData::GetInstance();
    bool bConverting = CommonData.m_eCharacterSet == NDDSCHARACTERSET_EUCKR?true:false;

    NDDSTMAP_FINDBUSARRIVAL_RES res;
    NDDSLIB_Json::Value lanePaths,stopInfos;
    memset(&res, 0x00, sizeof(NDDSTMAP_FINDBUSARRIVAL_RES));

    try{
        JSON_ALLOC_STRING(root, "first", res.first, memory);
        JSON_ALLOC_STRING(root, "last", res.last, memory);
        JSON_ALLOC_STRING(root, "tel", res.tel, memory);
        res.interval = root["interval"].asInt();
        JSON_ALLOC_STRING(root, "distance", res.distance, memory);
        res.runTime = root["runTime"].asInt();
        JSON_ALLOC_STRING(root, "turnPoint", res.turnPoint, memory);
        JSON_ALLOC_STRING(root, "laneInfo", res.laneInfo, memory);
        JSON_ALLOC_STRING(root, "companyName", res.companyName, memory);
        res.lanePathCount = root["lanePathCount"].asInt();
        lanePaths = root["lanePaths"];
        res.stopInfoCount = root["stopInfoCount"].asInt();
        stopInfos = root["stopInfos"];

        if(res.lanePathCount > 0)
        {
            JSON_ALLOC_BLOCK(res.lanePaths, NDDSTMAP_LANEPATHS_DTO*, sizeof(NDDSTMAP_LANEPATHS_DTO) * res.lanePathCount, memory);
            for(int laneIdx =0;laneIdx<res.lanePathCount;laneIdx++)
            {
                JSON_ALLOC_STRING(lanePaths[laneIdx], "navX", res.lanePaths[laneIdx].navX, memory);
                JSON_ALLOC_STRING(lanePaths[laneIdx], "navY", res.lanePaths[laneIdx].navY, memory);
            }
        }

        if(res.stopInfoCount > 0)
        {
            JSON_ALLOC_BLOCK(res.stopInfos, NDDSTMAP_STOPINFOS_DTO*, sizeof(NDDSTMAP_STOPINFOS_DTO) * res.stopInfoCount, memory);
            for(int infoIdx =0;infoIdx<res.stopInfoCount;infoIdx++)
            {
                JSON_ALLOC_STRING(stopInfos[infoIdx], "index", res.stopInfos[infoIdx].index, memory);
                JSON_ALLOC_STRING(stopInfos[infoIdx], "sid", res.stopInfos[infoIdx].sid, memory);
                JSON_ALLOC_STRING(stopInfos[infoIdx], "sidRt", res.stopInfos[infoIdx].sidRt, memory);
                JSON_ALLOC_STRING(stopInfos[infoIdx], "stopNavX", res.stopInfos[infoIdx].stopNavX, memory);
                JSON_ALLOC_STRING(stopInfos[infoIdx], "stopNavY", res.stopInfos[infoIdx].stopNavY, memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, stopInfos[infoIdx], "stopName", res.stopInfos[infoIdx].stopName, memory);

                res.stopInfos[infoIdx].arrivalInfoCount = stopInfos[infoIdx]["arrivalInfoCount"].asInt();

                NDDSLIB_Json::Value busArrivalInfos = stopInfos[infoIdx]["busArrivalInfos"];
                if(res.stopInfos[infoIdx].arrivalInfoCount > 0)
                {
                    JSON_ALLOC_BLOCK(res.stopInfos[infoIdx].busArrivalInfos, NDDSTMAP_BUSARRIVALINFOS_DTO*, sizeof(NDDSTMAP_BUSARRIVALINFOS_DTO) * res.stopInfos[infoIdx].arrivalInfoCount, memory);

                    for(int busIdx = 0;busIdx < res.stopInfos[infoIdx].arrivalInfoCount;busIdx++)
                    {
                        JSON_ALLOC_MULITBYTE_STRING(bConverting, busArrivalInfos[busIdx], "plainNo", res.stopInfos[infoIdx].busArrivalInfos[busIdx].plainNo, memory);
                        JSON_ALLOC_MULITBYTE_STRING(bConverting, busArrivalInfos[busIdx], "stopFlag", res.stopInfos[infoIdx].busArrivalInfos[busIdx].stopFlag, memory);
                    }
                }
            }
        }
    }catch(...){
        return false;
    }

    *ppRes = (NDDSTMAP_FINDBUSARRIVAL_RES*)malloc(sizeof(NDDSTMAP_FINDBUSARRIVAL_RES));
    memcpy(*ppRes, &res, sizeof(NDDSTMAP_FINDBUSARRIVAL_RES));
    return true;
}

bool CNddsLibTmap::MakeFindBusArrival( NDDSLIB_Json::Value &root,NDDSTMAP_FINDBUSARRIVAL_REQ *pReq )
{
    try{
        root["blid"] = pReq->blid;
        root["blidRt"] = pReq->blidRt;
        root["cid"] = pReq->cid;
    }catch(...){
        return false;
    }

    return true;
}

bool CNddsLibTmap::MakePositionSharing(NDDSLIB_Json::Value &root, NDDSTMAP_POSITIONSHARING_REQ *pReq)
{
    try
    {
        root["coordLongt"] = pReq->coordLongt;
        root["coordLat"] = pReq->coordLat;
        root["coordType"] = pReq->coordType;
        if(pReq->addr) root["addr"] = pReq->addr;
        if(pReq->poiId) root["poiId"] = pReq->poiId;
        if(pReq->poiName) root["poiName"] = pReq->poiName;
        if(pReq->poiTel) root["poiTel"] = pReq->poiTel;
        if(pReq->rpFlag) root["rpFlag"] = pReq->rpFlag;
    }
    catch (...)
    {
        return false;
    }

    return true;
}

bool CNddsLibTmap::ParserPositionSharing(NDDSLIB_Json::Value &root,NDDS_MEMORYSTATUS &memory, NDDSTMAP_POSITIONSHARING_RES **ppRes)
{
    NDDSTMAP_POSITIONSHARING_RES res;
    memset(&res, 0x00, sizeof(NDDSTMAP_POSITIONSHARING_RES));

    try
    {
        JSON_ALLOC_STRING(root, "tinyUrl", res.tinyUrl, memory);
    }
    catch (...)
    {
        return false;
    }

    *ppRes = (NDDSTMAP_POSITIONSHARING_RES*)malloc(sizeof(NDDSTMAP_POSITIONSHARING_RES));
    memcpy(*ppRes, &res, sizeof(NDDSTMAP_POSITIONSHARING_RES));
    return true;
}

bool CNddsLibTmap::MakeRouteSharingCar(NDDSLIB_Json::Value &root, NDDSTMAP_ROUTESHARINGCAR_REQ *pReq)
{
    CNddsLibCommonData &CommonData = CNddsLibCommonData::GetInstance();
    int nAllocSize, nEncodeSize;
    char *szRoutingData;

    try
    {
        nAllocSize = (4 * (pReq->routingDataSize / 3)) + (pReq->routingDataSize % 3? 4 : 0) + 1;
        szRoutingData = (char*)CommonData.GetMemory(nAllocSize);
        nEncodeSize = CNddsBase64::NddsEncode_Base64(pReq->routingData, pReq->routingDataSize, szRoutingData, nAllocSize);

        root["routingData"] = szRoutingData;
        root["routingDataSize"] = nEncodeSize;
        if(pReq->routingDataVer) root["routingDataVer"] = pReq->routingDataVer;

        CommonData.FreeMemory((unsigned char*)szRoutingData);
    }
    catch (...)
    {
        return false;
    }


    return true;
}

bool CNddsLibTmap::ParserRouteSharingCar(NDDSLIB_Json::Value &root,NDDS_MEMORYSTATUS &memory, NDDSTMAP_ROUTESHARINGCAR_RES **ppRes)
{
    NDDSTMAP_ROUTESHARINGCAR_RES res;
    memset(&res, 0x00, sizeof(NDDSTMAP_ROUTESHARINGCAR_RES));

    try
    {
        JSON_ALLOC_STRING(root, "tinyUrl", res.tinyUrl, memory);
    }
    catch (...)
    {
        return false;
    }

    *ppRes = (NDDSTMAP_ROUTESHARINGCAR_RES*)malloc(sizeof(NDDSTMAP_ROUTESHARINGCAR_RES));
    memcpy(*ppRes, &res, sizeof(NDDSTMAP_ROUTESHARINGCAR_RES));
    return true;
}

bool CNddsLibTmap::MakeRouteSharingPublic(NDDSLIB_Json::Value &root, NDDSTMAP_ROUTESHARINGPUBLIC_REQ *pReq)
{
    CNddsLibCommonData &CommonData = CNddsLibCommonData::GetInstance();
    int nAllocSize, nEncodeSize;
    char *szRoutingData;

    try
    {
        nAllocSize = (4 * (pReq->routingDataSize / 3)) + (pReq->routingDataSize % 3? 4 : 0) + 1;
        szRoutingData = (char*)CommonData.GetMemory(nAllocSize);
        nEncodeSize = CNddsBase64::NddsEncode_Base64(pReq->routingData, pReq->routingDataSize, szRoutingData, nAllocSize);

        root["routingData"] = szRoutingData;
        root["routingDataSize"] = nEncodeSize;
        if(pReq->routingDataVer) root["routingDataVer"] = pReq->routingDataVer;

        CommonData.FreeMemory((unsigned char*)szRoutingData);
    }
    catch (...)
    {
        return false;
    }

    return true;
}

bool CNddsLibTmap::ParserRouteSharingPublic(NDDSLIB_Json::Value &root,NDDS_MEMORYSTATUS &memory, NDDSTMAP_ROUTESHARINGPUBLIC_RES **ppRes)
{
    NDDSTMAP_ROUTESHARINGPUBLIC_RES res;
    memset(&res, 0x00, sizeof(NDDSTMAP_ROUTESHARINGPUBLIC_RES));

    try
    {
        JSON_ALLOC_STRING(root, "tinyUrl", res.tinyUrl, memory);
    }
    catch (...)
    {
        return false;
    }

    *ppRes = (NDDSTMAP_ROUTESHARINGPUBLIC_RES*)malloc(sizeof(NDDSTMAP_ROUTESHARINGPUBLIC_RES));
    memcpy(*ppRes, &res, sizeof(NDDSTMAP_ROUTESHARINGPUBLIC_RES));
    return true;
}

bool CNddsLibTmap::MakeRouteSharingWalk(NDDSLIB_Json::Value &root, NDDSTMAP_ROUTESHARINGWALK_REQ *pReq)
{
    CNddsLibCommonData &CommonData = CNddsLibCommonData::GetInstance();
    int nAllocSize, nEncodeSize;
    char *szRoutingData;

    try
    {
        nAllocSize = (4 * (pReq->routingDataSize / 3)) + (pReq->routingDataSize % 3? 4 : 0) + 1;
        szRoutingData = (char*)CommonData.GetMemory(nAllocSize);
        nEncodeSize = CNddsBase64::NddsEncode_Base64(pReq->routingData, pReq->routingDataSize, szRoutingData, nAllocSize);

        root["routingData"] = szRoutingData;
        root["routingDataSize"] = nEncodeSize;
        if(pReq->routingDataVer) root["routingDataVer"] = pReq->routingDataVer;

        CommonData.FreeMemory((unsigned char*)szRoutingData);
    }
    catch (...)
    {
        return false;
    }

    return true;
}

bool CNddsLibTmap::ParserRouteSharingWalk(NDDSLIB_Json::Value &root,NDDS_MEMORYSTATUS &memory, NDDSTMAP_ROUTESHARINGWALK_RES **ppRes)
{
    NDDSTMAP_ROUTESHARINGWALK_RES res;
    memset(&res, 0x00, sizeof(NDDSTMAP_ROUTESHARINGWALK_RES));

    try
    {
        JSON_ALLOC_STRING(root, "tinyUrl", res.tinyUrl, memory);
    }
    catch (...)
    {
        return false;
    }

    *ppRes = (NDDSTMAP_ROUTESHARINGWALK_RES*)malloc(sizeof(NDDSTMAP_ROUTESHARINGWALK_RES));
    memcpy(*ppRes, &res, sizeof(NDDSTMAP_ROUTESHARINGWALK_RES));
    return true;
}

bool CNddsLibTmap::MakeRouteSharingDriving(NDDSLIB_Json::Value &root, NDDSTMAP_ROUTESHARINGDRIVING_REQ *pReq)
{
    CNddsLibCommonData &CommonData = CNddsLibCommonData::GetInstance();
    int nAllocSize, nEncodeSize;
    char *szRoutingData;

    try
    {
        nAllocSize = (4 * (pReq->routingDataSize / 3)) + (pReq->routingDataSize % 3? 4 : 0) + 1;
        szRoutingData = (char*)CommonData.GetMemory(nAllocSize);
        nEncodeSize = CNddsBase64::NddsEncode_Base64(pReq->routingData, pReq->routingDataSize, szRoutingData, nAllocSize);

        root["routingData"] = szRoutingData;
        root["routingDataSize"] = nEncodeSize;
        if(pReq->routingDataVer) root["routingDataVer"] = pReq->routingDataVer;

        CommonData.FreeMemory((unsigned char*)szRoutingData);
    }
    catch (...)
    {
        return false;
    }

    return true;
}

bool CNddsLibTmap::ParserRouteSharingDriving(NDDSLIB_Json::Value &root,NDDS_MEMORYSTATUS &memory, NDDSTMAP_ROUTESHARINGDRIVING_RES **ppRes)
{
    NDDSTMAP_ROUTESHARINGDRIVING_RES res;
    memset(&res, 0x00, sizeof(NDDSTMAP_ROUTESHARINGDRIVING_RES));

    try
    {
        JSON_ALLOC_STRING(root, "tinyUrl", res.tinyUrl, memory);
    }
    catch (...)
    {
        return false;
    }

    *ppRes = (NDDSTMAP_ROUTESHARINGDRIVING_RES*)malloc(sizeof(NDDSTMAP_ROUTESHARINGDRIVING_RES));
    memcpy(*ppRes, &res, sizeof(NDDSTMAP_ROUTESHARINGDRIVING_RES));
    return true;
}

bool CNddsLibTmap::MakeRegisterSendToCarInfo(NDDSLIB_Json::Value &root, NDDSTMAP_REGISTERSENDTOCARINFO_REQ *pReq)
{
    CNddsLibCommonData &CommonData = CNddsLibCommonData::GetInstance();

    try
    {
        root["custName"] = pReq->custName;
        root["noorX"] = pReq->noorX;
        root["noorY"] = pReq->noorY;
        root["poiId"] = pReq->poiId;
        root["rpFlag"] = pReq->rpFlag;
        root["navSeq"] = pReq->navSeq;
    }
    catch (...)
    {
        return false;
    }

    return true;
}

bool CNddsLibTmap::ParserRegisterSendToCarInfo(NDDSLIB_Json::Value &root,NDDS_MEMORYSTATUS &memory, NDDSTMAP_REGISTERSENDTOCARINFO_RES **ppRes)
{
    NDDSTMAP_REGISTERSENDTOCARINFO_RES res;
    memset(&res, 0x00, sizeof(NDDSTMAP_REGISTERSENDTOCARINFO_RES));

    *ppRes = (NDDSTMAP_REGISTERSENDTOCARINFO_RES*)malloc(sizeof(NDDSTMAP_REGISTERSENDTOCARINFO_RES));
    memcpy(*ppRes, &res, sizeof(NDDSTMAP_REGISTERSENDTOCARINFO_RES));
    return true;
}

bool CNddsLibTmap::MakeFindSendToCarInfo(NDDSLIB_Json::Value &root, NDDSTMAP_FINDSENDTOCARINFO_REQ *pReq)
{
    return true;
}

bool CNddsLibTmap::ParserFindSendToCarInfo(NDDSLIB_Json::Value &root,NDDS_MEMORYSTATUS &memory, NDDSTMAP_FINDSENDTOCARINFO_RES **ppRes)
{
    CNddsLibCommonData &CommonData = CNddsLibCommonData::GetInstance();
    bool bConverting = CommonData.m_eCharacterSet == NDDSCHARACTERSET_EUCKR?true:false;

    NDDSTMAP_FINDSENDTOCARINFO_RES res;
    memset(&res, 0x00, sizeof(NDDSTMAP_FINDSENDTOCARINFO_RES));

    try
    {
        JSON_ALLOC_MULITBYTE_STRING(bConverting, root, "poiId", res.poiId, memory);
        JSON_ALLOC_MULITBYTE_STRING(bConverting, root, "navSeq", res.navSeq, memory);
        JSON_ALLOC_MULITBYTE_STRING(bConverting, root, "custName", res.custName, memory);
        JSON_ALLOC_MULITBYTE_STRING(bConverting, root, "noorX", res.noorX, memory);
        JSON_ALLOC_MULITBYTE_STRING(bConverting, root, "noorY", res.noorY, memory);
        JSON_ALLOC_MULITBYTE_STRING(bConverting, root, "lcdName", res.lcdName, memory);
        JSON_ALLOC_MULITBYTE_STRING(bConverting, root, "mcdName", res.mcdName, memory);
        JSON_ALLOC_MULITBYTE_STRING(bConverting, root, "scdName", res.scdName, memory);
        JSON_ALLOC_MULITBYTE_STRING(bConverting, root, "dcdName", res.dcdName, memory);
        JSON_ALLOC_MULITBYTE_STRING(bConverting, root, "primaryBun", res.primaryBun, memory);
        JSON_ALLOC_MULITBYTE_STRING(bConverting, root, "secondaryBun", res.secondaryBun, memory);
        JSON_ALLOC_MULITBYTE_STRING(bConverting, root, "mlClass", res.mlClass, memory);
        JSON_ALLOC_MULITBYTE_STRING(bConverting, root, "roadName", res.roadName, memory);
        JSON_ALLOC_MULITBYTE_STRING(bConverting, root, "bldNo1", res.bldNo1, memory);
        JSON_ALLOC_MULITBYTE_STRING(bConverting, root, "bldNo2", res.bldNo2, memory);
        JSON_ALLOC_MULITBYTE_STRING(bConverting, root, "repClsName", res.repClsName, memory);
        JSON_ALLOC_MULITBYTE_STRING(bConverting, root, "clsAName", res.clsAName, memory);
        JSON_ALLOC_MULITBYTE_STRING(bConverting, root, "clsBName", res.clsBName, memory);
        JSON_ALLOC_MULITBYTE_STRING(bConverting, root, "clsCName", res.clsCName, memory);
        JSON_ALLOC_MULITBYTE_STRING(bConverting, root, "clsDName", res.clsDName, memory);
        res.rpFlag = root["rpFlag"].asUInt();
        JSON_ALLOC_MULITBYTE_STRING(bConverting, root, "telNo", res.telNo, memory);
        JSON_ALLOC_MULITBYTE_STRING(bConverting, root, "insDatetime", res.insDatetime, memory);
    }
    catch (...)
    {
        return false;
    }

    *ppRes = (NDDSTMAP_FINDSENDTOCARINFO_RES*)malloc(sizeof(NDDSTMAP_FINDSENDTOCARINFO_RES));
    memcpy(*ppRes, &res, sizeof(NDDSTMAP_FINDSENDTOCARINFO_RES));
    return true;
}

bool CNddsLibTmap::MakeRemoveSendToCarInfo(NDDSLIB_Json::Value &root, NDDSTMAP_REMOVESENDTOCARINFO_REQ *pReq)
{
    CNddsLibCommonData &CommonData = CNddsLibCommonData::GetInstance();

    try
    {
        root["custName"] = pReq->custName;
        root["noorX"] = pReq->noorX;
        root["noorY"] = pReq->noorY;
    }
    catch (...)
    {
        return false;
    }

    return true;
}

bool CNddsLibTmap::ParserRemoveSendToCarInfo(NDDSLIB_Json::Value &root,NDDS_MEMORYSTATUS &memory, NDDSTMAP_REMOVESENDTOCARINFO_RES **ppRes)
{
    NDDSTMAP_REMOVESENDTOCARINFO_RES res;
    memset(&res, 0x00, sizeof(NDDSTMAP_REMOVESENDTOCARINFO_RES));

    *ppRes = (NDDSTMAP_REMOVESENDTOCARINFO_RES*)malloc(sizeof(NDDSTMAP_REMOVESENDTOCARINFO_RES));
    memcpy(*ppRes, &res, sizeof(NDDSTMAP_REMOVESENDTOCARINFO_RES));
    return true;
}

bool CNddsLibTmap::MakeSaveAdvertise(NDDSLIB_Json::Value &root,NDDSTMAP_SAVEADVERTISE_REQ *pReq)
{
    NDDSLIB_Json::Value advtReportDetails;

    try{
        root["reportPoint"] = pReq->reportPoint;

        for(int i =0 ; i< pReq->nAdvtReportDetails;i++)
        {
            advtReportDetails.clear();

            advtReportDetails["playDate"] = pReq->advtReportDetails[i].playDate;
            advtReportDetails["adType"] = pReq->advtReportDetails[i].adType;
            advtReportDetails["adPlayTime"] = pReq->advtReportDetails[i].adPlayTime;
            advtReportDetails["adCode"] = pReq->advtReportDetails[i].adCode;
            //advtReportDetails["adAreaCode"] = pReq->advtReportDetails[i].adAreaCode; // 사용하지 않음.
            advtReportDetails["completeType"] = pReq->advtReportDetails[i].completeType;
            advtReportDetails["runningTime"] = pReq->advtReportDetails[i].runningTime;
            advtReportDetails["routeSessionId"] = pReq->advtReportDetails[i].routeSessionId;
            root["advtReportDetails"].append(advtReportDetails);
        }
    }catch(...){
        return false;
    }

    return true;
}

bool CNddsLibTmap::ParserSaveAdvertise(NDDSLIB_Json::Value &root,NDDS_MEMORYSTATUS &memory, NDDSTMAP_SAVEADVERTISE_RES **ppRes)
{
    //필드 없음

    *ppRes = (NDDSTMAP_SAVEADVERTISE_RES*)malloc(sizeof(NDDSTMAP_SAVEADVERTISE_RES));
    return true;
}

bool CNddsLibTmap::MakeFindAgreement(NDDSLIB_Json::Value &root,NDDSTMAP_FINDAGREEMENT_REQ *pReq)
{
    try{
        root["termsType"] = pReq->termsType;
    }catch(...){
        return false;
    }

    return true;
}

bool CNddsLibTmap::ParserFindAgreement(NDDSLIB_Json::Value &root,NDDS_MEMORYSTATUS &memory, NDDSTMAP_FINDAGREEMENT_RES **ppRes)
{
    CNddsLibCommonData &CommonData = CNddsLibCommonData::GetInstance();
    bool bConverting = CommonData.m_eCharacterSet == NDDSCHARACTERSET_EUCKR?true:false;

    NDDSTMAP_FINDAGREEMENT_RES res;
    NDDSLIB_Json::Value element;
    memset(&res, 0x00, sizeof(NDDSTMAP_FINDAGREEMENT_RES));

    try{
        res.termsResult = root["termsResult"].asUInt();
        JSON_ALLOC_MULITBYTE_STRING(bConverting, root, "termsVersion", res.termsVersion, memory);
        JSON_ALLOC_MULITBYTE_STRING(bConverting, root, "termsTitle", res.termsTitle, memory);
        JSON_ALLOC_MULITBYTE_STRING(bConverting, root, "termsUrl", res.termsUrl, memory);
        JSON_ALLOC_MULITBYTE_STRING(bConverting, root, "termsUrlType", res.termsUrlType, memory);

        if(res.termsResult == 0x01)
        {
            element = root["termsAgreements"];
            res.nTermsAgreements = element.size();
            if(element.size() > 0)
            {
                JSON_ALLOC_BLOCK(res.termsAgreements, NDDSTMAP_TERMSAGREEMENTS_DTO*, sizeof(NDDSTMAP_TERMSAGREEMENTS_DTO) * element.size(), memory);
                for(int i = 0;i<(int)element.size();i++)
                {
                    JSON_ALLOC_MULITBYTE_STRING(bConverting, element[i], "allowTitle",	res.termsAgreements[i].allowTitle,memory);
                    JSON_ALLOC_MULITBYTE_STRING(bConverting, element[i], "allowCode",	res.termsAgreements[i].allowCode,memory);
                    JSON_ALLOC_MULITBYTE_STRING(bConverting, element[i], "allowYn",	res.termsAgreements[i].allowYn,memory);
                }
            }
        }
    }catch(...){
        return false;
    }

    *ppRes = (NDDSTMAP_FINDAGREEMENT_RES*)malloc(sizeof(NDDSTMAP_FINDAGREEMENT_RES));
    memcpy(*ppRes, &res, sizeof(NDDSTMAP_FINDAGREEMENT_RES));
    return true;

}

bool CNddsLibTmap::MakeRegistAgreement(NDDSLIB_Json::Value &root,NDDSTMAP_REGISTAGREEMENT_REQ *pReq)
{
    NDDSLIB_Json::Value termsAgreements;

    try{
        root["termsVersion"] = pReq->termsVersion;
        root["termsType"] = pReq->termsType;
        root["termsUrlType"] = pReq->termsUrlType;

        for(int i =0 ; i< pReq->nTermsAgreements;i++)
        {
            termsAgreements.clear();

            termsAgreements["allowTitle"] = pReq->termsAgreements[i].allowTitle;
            termsAgreements["allowCode"] = pReq->termsAgreements[i].allowCode;
            termsAgreements["allowYn"] = pReq->termsAgreements[i].allowYn;
            root["termsAgreements"].append(termsAgreements);
        }
    }catch(...){
        return false;
    }

    return true;
}

bool CNddsLibTmap::ParserRegistAgreement(NDDSLIB_Json::Value &root,NDDS_MEMORYSTATUS &memory, NDDSTMAP_REGISTAGREEMENT_RES **ppRes)
{
    //필드 없음

    *ppRes = (NDDSTMAP_REGISTAGREEMENT_RES*)malloc(sizeof(NDDSTMAP_REGISTAGREEMENT_RES));
    return true;
}

bool CNddsLibTmap::MakeFindBusArrivalTime( NDDSLIB_Json::Value &root,NDDSTMAP_FINDBUSARRIVALTIME_REQ *pReq )
{
    try{
        root["blid"] = pReq->blid;
        root["cid"] = pReq->cid;
        root["sid"] = pReq->sid;
        root["sidRt"] = pReq->sidRt;
    }catch(...){
        return false;
    }

    return true;
}

// 5.5.4. 버스 정류장/노선 실시간 정보
bool CNddsLibTmap::MakeFindBusStationRealTime( NDDSLIB_Json::Value &root,NDDSTMAP_FINDBUSSTATIONREALTIME_REQ *pReq )
{
    NDDSLIB_Json::Value busStationRealTimeDetails;

    try{
        root["busStopCount"] = pReq->busStopCount;

        for(int i =0;i<pReq->busStopCount;i++){
            busStationRealTimeDetails[i]["blid"] = pReq->busStationRealTimeDetails[i].blid;
            busStationRealTimeDetails[i]["blidRt"] = pReq->busStationRealTimeDetails[i].blidRt;
            busStationRealTimeDetails[i]["sid"] = pReq->busStationRealTimeDetails[i].sid;
            busStationRealTimeDetails[i]["sidRt"] = pReq->busStationRealTimeDetails[i].sidRt;
            busStationRealTimeDetails[i]["cid"] = pReq->busStationRealTimeDetails[i].cid;
        }
        root["busStationRealTimeDetails"] = busStationRealTimeDetails;
    }catch(...){
        return false;
    }

    return true;
}

// 5.5.5. 역 정보
bool CNddsLibTmap::MakeFindStationInfo( NDDSLIB_Json::Value &root,NDDSTMAP_FINDSTATIONINFO_REQ *pReq )
{
    try{
        root["sid"] = pReq->sid;
        root["cid"] = pReq->cid;
    }catch(...){
        return false;
    }

    return true;
}

// 5.5.12. 주변 CCTV 리스트
bool CNddsLibTmap::MakeFindCCTVResult( NDDSLIB_Json::Value &root,NDDSTMAP_FINDCCTVRESULT_REQ *pReq )
{
    try{
        root["sx"] = pReq->sx;
        root["sy"] = pReq->sy;
        root["iradius"] = pReq->iradius;
    }catch(...){
        return false;
    }

    return true;
}

//5.5.13. CCTV 정보
bool CNddsLibTmap::MakeFindCCTVResultById( NDDSLIB_Json::Value &root,NDDSTMAP_FINDCCTVRESULTBYID_REQ *pReq )
{
    try{
        root["cctvId"] = pReq->cctvId;
    }catch(...){
        return false;
    }

    return true;
}

// 5.5.6. 역 운행 정보
bool CNddsLibTmap::MakeFindStationTimeTable( NDDSLIB_Json::Value &root,NDDSTMAP_FINDSTATIONTIMETABLE_REQ *pReq )
{
    try{
        root["sid"] = pReq->sid;
        root["cid"] = pReq->cid;
    }catch(...)
    {
        return false;
    }
    return true;
}

//5.5.7. 노선 이미지
bool CNddsLibTmap::MakeFindSubwayImage( NDDSLIB_Json::Value &root,NDDSTMAP_FINDSUBWAYIMAGE_REQ *pReq )
{
    try{
        root["subwayImage"] = pReq->subwayImage;
    }catch(...)
    {
        return false;
    }
    return true;
}

//5.5.8. 경로안내(도시간 검색 포함)
bool CNddsLibTmap::MakeFindPathFind1stAll4( NDDSLIB_Json::Value &root,NDDSTMAP_FINDPATHFIND1STALL4_REQ *pReq )
{
    try{
        root["sx"] = pReq->sx;
        root["sy"] = pReq->sy;
        root["ex"] = pReq->ex;
        root["ey"] = pReq->ey;
        root["searchType"]	= pReq->searchType;
    }catch(...)
    {
        return false;
    }
    return true;
}

// 5.5.9. 경로안내(도시내 복수 검색)
bool CNddsLibTmap::MakeFindPathFind1stAll4Plural( NDDSLIB_Json::Value &root,NDDSTMAP_FINDPATHFIND1STALL4PLURAL_REQ *pReq )
{
    try{
        root["startSx"] = pReq->startSx;
        root["startSy"] = pReq->startSy;
        root["startEx"] = pReq->startEx;
        root["startEy"] = pReq->startEy;
        root["endSx"]	= pReq->endSx;
        root["endSy"]	= pReq->endSy;
        root["endEx"]	= pReq->endEx;
        root["endEy"]	= pReq->endEy;
        root["searchType"] = pReq->searchType;
    }catch(...)
    {
        return false;
    }

    return true;
}

// 5.5.10. 주변 정류장 / 역 조회(거리순)
bool CNddsLibTmap::MakeFindNearStation( NDDSLIB_Json::Value &root,NDDSTMAP_FINDNEARSTATION_REQ *pReq )
{
    try{
        root["sx"] = pReq->sx;
        root["sy"] = pReq->sy;
        root["iradius"] = pReq->iradius;
        root["sclass"] = pReq->sclass;
        root["cid"] = pReq->cid;
    }catch(...)
    {
        return false;
    }
    return true;
}

// 5.5.11. 통합 검색
bool CNddsLibTmap::MakeFindAllTransport( NDDSLIB_Json::Value &root,NDDSTMAP_FINDALLTRANSPORT_REQ *pReq )
{
    try{
        root["searchName"] = pReq->searchName;
        root["cid"] = pReq->cid;
    }catch(...)
    {
        return false;
    }
    return true;
}

// 5.5.3. 버스 노선 정보(도착 시간)
bool CNddsLibTmap::ParserFindBusArrivalTime( NDDSLIB_Json::Value &root,NDDS_MEMORYSTATUS &memory,NDDSTMAP_FINDBUSARRIVALTIME_RES **ppRes )
{
    CNddsLibCommonData &CommonData = CNddsLibCommonData::GetInstance();
    bool bConverting = CommonData.m_eCharacterSet == NDDSCHARACTERSET_EUCKR?true:false;

    NDDSTMAP_FINDBUSARRIVALTIME_RES res;
    NDDSLIB_Json::Value lanePaths,stopInfos;
    memset(&res, 0x00, sizeof(NDDSTMAP_FINDBUSARRIVALTIME_RES));

    try{
        JSON_ALLOC_MULITBYTE_STRING(bConverting, root, "first", res.first, memory);
        JSON_ALLOC_MULITBYTE_STRING(bConverting, root, "last", res.last, memory);
        JSON_ALLOC_MULITBYTE_STRING(bConverting, root, "tel", res.tel, memory);
        res.interval = root["interval"].asInt();
        JSON_ALLOC_MULITBYTE_STRING(bConverting, root, "distance", res.distance, memory);
        res.runTime = root["runTime"].asInt();
        JSON_ALLOC_MULITBYTE_STRING(bConverting, root, "turnPoint", res.turnPoint, memory);
        JSON_ALLOC_MULITBYTE_STRING(bConverting, root, "laneInfo", res.laneInfo, memory);
        JSON_ALLOC_MULITBYTE_STRING(bConverting, root, "companyName", res.companyName, memory);
        res.lanePathCount = root["lanePathCount"].asInt();
        lanePaths = root["lanePaths"];
        res.stopInfoCount = root["stopInfoCount"].asInt();
        stopInfos = root["stopInfos"];

        if(res.lanePathCount > 0)
        {
            JSON_ALLOC_BLOCK(res.lanePaths, NDDSTMAP_LANEPATHS_DTO*, sizeof(NDDSTMAP_LANEPATHS_DTO) * res.lanePathCount, memory);
            for(int laneIdx =0;laneIdx<res.lanePathCount;laneIdx++)
            {
                JSON_ALLOC_STRING(lanePaths[laneIdx], "navX", res.lanePaths[laneIdx].navX, memory);
                JSON_ALLOC_STRING(lanePaths[laneIdx], "navY", res.lanePaths[laneIdx].navY, memory);
            }
        }

        // - 정류장 정보 리스트 정보(stopInfos)
        if(res.stopInfoCount > 0)
        {
            JSON_ALLOC_BLOCK(res.stopInfos, NDDSTMAP_STOPINFOS2_DTO*, sizeof(NDDSTMAP_STOPINFOS2_DTO) * res.stopInfoCount, memory);
            for(int infoIdx =0;infoIdx<res.stopInfoCount;infoIdx++)
            {
                JSON_ALLOC_STRING(stopInfos[infoIdx], "index", res.stopInfos[infoIdx].index, memory);
                JSON_ALLOC_STRING(stopInfos[infoIdx], "sid", res.stopInfos[infoIdx].sid, memory);
                JSON_ALLOC_STRING(stopInfos[infoIdx], "sidRt", res.stopInfos[infoIdx].sidRt, memory);
                JSON_ALLOC_STRING(stopInfos[infoIdx], "stopNavX", res.stopInfos[infoIdx].stopNavX, memory);
                JSON_ALLOC_STRING(stopInfos[infoIdx], "stopNavY", res.stopInfos[infoIdx].stopNavY, memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, stopInfos[infoIdx], "stopName", res.stopInfos[infoIdx].stopName, memory);

                res.stopInfos[infoIdx].stopCount = stopInfos[infoIdx]["stopCount"].asInt();

                // - 버스 도착정보 리스트 정보(itemListDetails)
                NDDSLIB_Json::Value itemListDetails = stopInfos[infoIdx]["itemListDetails"];
                res.stopInfos[infoIdx].nItemListDetails = itemListDetails.size();
                if(itemListDetails.size() > 0)
                {
                    JSON_ALLOC_BLOCK(res.stopInfos[infoIdx].itemListDetails, NDDSTMAP_ITEMARRIVEDETAILS_DTO*, sizeof(NDDSTMAP_ITEMARRIVEDETAILS_DTO) * itemListDetails.size(), memory);
                    for(int busIdx = 0;busIdx < res.stopInfos[infoIdx].nItemListDetails;busIdx++)
                    {
                        JSON_ALLOC_MULITBYTE_STRING(bConverting, itemListDetails[busIdx], "plainNo",	res.stopInfos[infoIdx].itemListDetails[busIdx].plainNo, memory);
                        JSON_ALLOC_MULITBYTE_STRING(bConverting, itemListDetails[busIdx], "stopFlag",res.stopInfos[infoIdx].itemListDetails[busIdx].stopFlag, memory);
                        JSON_ALLOC_MULITBYTE_STRING(bConverting, itemListDetails[busIdx], "sectOrd",	res.stopInfos[infoIdx].itemListDetails[busIdx].sectOrd, memory);
                        JSON_ALLOC_MULITBYTE_STRING(bConverting, itemListDetails[busIdx], "arrivalTime",res.stopInfos[infoIdx].itemListDetails[busIdx].arrivalTime, memory);
                    }
                }

                // - 버스 도착정보 리스트 정보(itemArriveDzkaetails)
                NDDSLIB_Json::Value itemArriveDetails = stopInfos[infoIdx]["itemListDetails"];
                res.stopInfos[infoIdx].nItemArriveDetails = itemArriveDetails.size();
                if(itemListDetails.size() > 0)
                {
                    JSON_ALLOC_BLOCK(res.stopInfos[infoIdx].itemArriveDetails, NDDSTMAP_ITEMARRIVEDETAILS_DTO*, sizeof(NDDSTMAP_ITEMARRIVEDETAILS_DTO) * itemListDetails.size(), memory);
                    for(int busIdx = 0;busIdx < res.stopInfos[infoIdx].nItemArriveDetails;busIdx++)
                    {
                        JSON_ALLOC_MULITBYTE_STRING(bConverting, itemArriveDetails[busIdx], "plainNo",	res.stopInfos[infoIdx].itemArriveDetails[busIdx].plainNo, memory);
                        JSON_ALLOC_MULITBYTE_STRING(bConverting, itemArriveDetails[busIdx], "stopFlag",	res.stopInfos[infoIdx].itemArriveDetails[busIdx].stopFlag, memory);
                        JSON_ALLOC_MULITBYTE_STRING(bConverting, itemArriveDetails[busIdx], "sectOrd",	res.stopInfos[infoIdx].itemArriveDetails[busIdx].sectOrd, memory);
                        JSON_ALLOC_MULITBYTE_STRING(bConverting, itemArriveDetails[busIdx], "arrivalTime",res.stopInfos[infoIdx].itemArriveDetails[busIdx].arrivalTime, memory);
                    }
                }
            }
        }
    }catch(...){
        return false;
    }

    *ppRes = (NDDSTMAP_FINDBUSARRIVALTIME_RES*)malloc(sizeof(NDDSTMAP_FINDBUSARRIVALTIME_RES));
    memcpy(*ppRes, &res, sizeof(NDDSTMAP_FINDBUSARRIVALTIME_RES));
    return true;
}

// 5.5.4. 버스 정류장/노선 실시간 정보
bool CNddsLibTmap::ParserFindBusStationRealTime( NDDSLIB_Json::Value &root,NDDS_MEMORYSTATUS &memory,NDDSTMAP_FINDBUSSTATIONREALTIME_RES **ppRes )
{
    CNddsLibCommonData &CommonData = CNddsLibCommonData::GetInstance();
    bool bConverting = CommonData.m_eCharacterSet == NDDSCHARACTERSET_EUCKR?true:false;

    NDDSTMAP_FINDBUSSTATIONREALTIME_RES res;
    NDDSLIB_Json::Value busStationRealTimes;

    memset(&res, 0x00, sizeof(NDDSTMAP_FINDBUSSTATIONREALTIME_RES));

    try{
        res.busStopCount = root["busStopCount"].asInt();

        // - 버스 정류장 정보 리스트 정보(busStationRealTimes)
        busStationRealTimes = root["busStationRealTimes"];
        if(res.busStopCount > 0)
        {
            JSON_ALLOC_BLOCK(res.busStationRealTimes, NDDSTMAP_BUSSTATIONREALTIMES_DTO*, sizeof(NDDSTMAP_BUSSTATIONREALTIMES_DTO) * res.busStopCount, memory);
            for(int i = 0 ; i < res.busStopCount ; i++)
            {
                NDDSLIB_Json::Value arriveDetails;
                JSON_ALLOC_MULITBYTE_STRING(bConverting, busStationRealTimes[i], "blid",	res.busStationRealTimes[i].blid, memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, busStationRealTimes[i], "blidRt",	res.busStationRealTimes[i].blidRt, memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, busStationRealTimes[i], "sid",	res.busStationRealTimes[i].sid, memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, busStationRealTimes[i], "sidRt",	res.busStationRealTimes[i].sidRt, memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, busStationRealTimes[i], "stopIndex",	res.busStationRealTimes[i].stopIndex, memory);
                res.busStationRealTimes[i].arriveCount = busStationRealTimes[i]["arriveCount"].asInt();

                // - 버스 도착정보 리스트 정보(arriveDetails)
                arriveDetails = busStationRealTimes[i]["arriveDetails"];
                if(res.busStationRealTimes[i].arriveCount > 0)
                {
                    JSON_ALLOC_BLOCK(res.busStationRealTimes[i].arriveDetails, NDDSTMAP_ARRIVEDETAILS_DTO*, sizeof(NDDSTMAP_ARRIVEDETAILS_DTO) * res.busStationRealTimes[i].arriveCount, memory);
                    for(int j = 0 ; j < res.busStationRealTimes[i].arriveCount ; j++)
                    {
                        JSON_ALLOC_MULITBYTE_STRING(bConverting, arriveDetails[j],"busNo",res.busStationRealTimes[i].arriveDetails[j].busNo, memory);
                        res.busStationRealTimes[i].arriveDetails[j].isArrive = arriveDetails[j]["isArrive"].asInt();
                        JSON_ALLOC_MULITBYTE_STRING(bConverting, arriveDetails[j],"sectOrd",res.busStationRealTimes[i].arriveDetails[j].sectOrd, memory);
                        JSON_ALLOC_MULITBYTE_STRING(bConverting, arriveDetails[j],"traTime",res.busStationRealTimes[i].arriveDetails[j].traTime, memory);
                    }
                }
            }
        }
    }catch(...){
        return false;
    }

    *ppRes = (NDDSTMAP_FINDBUSSTATIONREALTIME_RES*)malloc(sizeof(NDDSTMAP_FINDBUSSTATIONREALTIME_RES));
    memcpy(*ppRes, &res, sizeof(NDDSTMAP_FINDBUSSTATIONREALTIME_RES));
    return true;
}

// 5.5.5. 역 정보
bool CNddsLibTmap::ParserFindStationInfo( NDDSLIB_Json::Value &root,NDDS_MEMORYSTATUS &memory,NDDSTMAP_FINDSTATIONINFO_RES **ppRes )
{
    CNddsLibCommonData &CommonData = CNddsLibCommonData::GetInstance();
    bool bConverting = CommonData.m_eCharacterSet == NDDSCHARACTERSET_EUCKR?true:false;

    NDDSTMAP_FINDSTATIONINFO_RES res;
    NDDSLIB_Json::Value preStations,nextStations,fastExs,gateInfos;

    memset(&res, 0x00, sizeof(NDDSTMAP_FINDSTATIONINFO_RES));

    try{
        JSON_ALLOC_MULITBYTE_STRING(bConverting, root,"laneNumber",res.laneNumber, memory);
        JSON_ALLOC_MULITBYTE_STRING(bConverting, root,"navX",res.navX, memory);
        JSON_ALLOC_MULITBYTE_STRING(bConverting, root,"navY",res.navY, memory);
        JSON_ALLOC_MULITBYTE_STRING(bConverting, root,"tel",res.tel, memory);
        JSON_ALLOC_MULITBYTE_STRING(bConverting, root,"offDoor",res.offDoor, memory);
        JSON_ALLOC_MULITBYTE_STRING(bConverting, root,"platForm",res.platForm, memory);
        JSON_ALLOC_MULITBYTE_STRING(bConverting, root,"crossOver",res.crossOver, memory);
        JSON_ALLOC_MULITBYTE_STRING(bConverting, root,"location",res.location, memory);
        JSON_ALLOC_MULITBYTE_STRING(bConverting, root,"telephone",res.telephone, memory);
        JSON_ALLOC_MULITBYTE_STRING(bConverting, root,"area",res.area, memory);
        res.fee = root["fee"].asInt();
        JSON_ALLOC_MULITBYTE_STRING(bConverting, root,"normalOpen",res.normalOpen, memory);
        JSON_ALLOC_MULITBYTE_STRING(bConverting, root,"weekendOpen",res.weekendOpen, memory);
        JSON_ALLOC_MULITBYTE_STRING(bConverting, root,"capacity",res.capacity, memory);
        JSON_ALLOC_MULITBYTE_STRING(bConverting, root,"name",res.name, memory);
        JSON_ALLOC_MULITBYTE_STRING(bConverting, root,"address",res.address, memory);
        JSON_ALLOC_MULITBYTE_STRING(bConverting, root,"imgMap",res.imgMap, memory);
        JSON_ALLOC_MULITBYTE_STRING(bConverting, root,"imgPath",res.imgPath, memory);
        JSON_ALLOC_MULITBYTE_STRING(bConverting, root,"handiCapInfo",res.handiCapInfo, memory);
        JSON_ALLOC_MULITBYTE_STRING(bConverting, root,"affair",res.affair, memory);
        res.preStationCount = root["preStationCount"].asInt();
        res.nextStationCount= root["nextStationCount"].asInt();
        res.fastExCount		= root["fastExCount"].asInt();
        res.gateCount		= root["gateCount"].asInt();

        // - 이전역 정보 리스트 정보(preStations)
        preStations = root["preStations"];
        if(res.preStationCount > 0)
        {
            JSON_ALLOC_BLOCK(res.preStations, NDDSTMAP_STATIONS_DTO*, sizeof(NDDSTMAP_STATIONS_DTO) * res.preStationCount, memory);
            for(int i = 0 ; i < res.preStationCount ; i++)
            {
                JSON_ALLOC_MULITBYTE_STRING(bConverting, preStations[i],"sid",res.preStations[i].sid, memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, preStations[i],"name",res.preStations[i].name, memory);
            }
        }

        // - 다음역 정보 리스트 정보(nextStations)
        nextStations = root["preStations"];
        if(res.nextStationCount > 0)
        {
            JSON_ALLOC_BLOCK(res.nextStations, NDDSTMAP_STATIONS_DTO*, sizeof(NDDSTMAP_STATIONS_DTO) * res.nextStationCount, memory);
            for(int i = 0 ; i < res.nextStationCount ; i++)
            {
                JSON_ALLOC_MULITBYTE_STRING(bConverting, nextStations[i],"sid",res.nextStations[i].sid, memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, nextStations[i],"name",res.nextStations[i].name, memory);
            }
        }

        // - 환승 정보 리스트 정보(fastExs)
        fastExs = root["gateInfos"];
        if(res.fastExCount > 0)
        {
            JSON_ALLOC_BLOCK(res.fastExs, NDDSTMAP_FASTEXS_DTO*, sizeof(NDDSTMAP_FASTEXS_DTO) * res.fastExCount, memory);
            for(int i = 0 ; i < res.fastExCount ; i++)
            {
                JSON_ALLOC_MULITBYTE_STRING(bConverting, fastExs[i],"takeLaneName",res.fastExs[i].takeLaneName, memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, fastExs[i],"takeLaneDirection",res.fastExs[i].takeLaneDirection, memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, fastExs[i],"exLaneName",res.fastExs[i].exLaneName, memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, fastExs[i],"exLaneDirection",res.fastExs[i].exLaneDirection, memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, fastExs[i],"takeLaneDirection",res.fastExs[i].takeLaneDirection, memory);

                res.fastExs[i].fastTrain = gateInfos[i]["fastTrain"].asInt();
                res.fastExs[i].fastDoor = gateInfos[i]["fastDoor"].asInt();
            }
        }

        // - 출구 정보 리스트 정보(gateInfos)
        gateInfos = root["gateInfos"];
        if(res.gateCount > 0)
        {
            JSON_ALLOC_BLOCK(res.gateInfos, NDDSTMAP_GATEINFOS_DTO *, sizeof(NDDSTMAP_GATEINFOS_DTO ) * res.gateCount, memory);
            for(int i = 0 ; i < res.gateCount ; i++)
            {
                NDDSLIB_Json::Value gateLinks, busStops;

                JSON_ALLOC_MULITBYTE_STRING(bConverting, gateInfos[i],"gateNo",res.gateInfos[i].gateNo, memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, gateInfos[i],"gateX",res.gateInfos[i].gateX, memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, gateInfos[i],"gateY",res.gateInfos[i].gateY, memory);
                res.gateInfos[i].gateLinkCount = gateInfos[i]["gateLinkCount"].asInt();
                res.gateInfos[i].busStopCount = gateInfos[i]["busStopCount"].asInt();
                gateLinks = gateInfos[i]["gateLinks"];
                busStops = gateInfos[i]["busStops"];

                // - 주요 건물 정보 리스트 정보(gateLinks)
                if(res.gateInfos[i].gateLinkCount > 0)
                {
                    JSON_ALLOC_BLOCK(res.gateInfos[i].gateLinks, NDDSTMAP_GATELINKS_DTO*, sizeof(NDDSTMAP_GATELINKS_DTO) * res.gateInfos[i].gateLinkCount, memory);
                    for(int j = 0 ;j < res.gateInfos[i].gateLinkCount ;j++)
                    {
                        JSON_ALLOC_MULITBYTE_STRING(bConverting, gateLinks,j,res.gateInfos[i].gateLinks[j].gateLinks,memory);
                        std::string strValue = gateLinks[i].asString();
                        const char *temp = strValue.c_str();
                    }
                }

                // - 버스 정류장 정보 리스트 정보(busStops)
                if(res.gateInfos[i].busStopCount > 0)
                {
                    JSON_ALLOC_BLOCK(res.gateInfos[i].busStops, NDDSTMAP_BUSSTOPS_DTO *, sizeof(NDDSTMAP_BUSSTOPS_DTO ) * res.gateInfos[i].busStopCount, memory);
                    for(int j = 0 ;j < res.gateInfos[i].busStopCount ;j++)
                    {
                        NDDSLIB_Json::Value busInfos;
                        JSON_ALLOC_MULITBYTE_STRING(bConverting, busStops[i],"busStops",res.gateInfos[i].busStops[j].stopName,memory);
                        res.gateInfos[i].busStops[j].busCount = busStops[j]["busCount"].asInt();
                        busInfos = busStops[j]["busInfos"];
                        if(res.gateInfos[i].busStops[j].busCount > 0)
                        {
                            JSON_ALLOC_BLOCK(res.gateInfos[i].busStops[j].busInfos, NDDSTMAP_BUSINFOS_DTO*, sizeof(NDDSTMAP_BUSINFOS_DTO) * res.gateInfos[i].busStops[j].busCount, memory);
                            for(int k = 0 ;k < res.gateInfos[i].busStopCount ;k++)
                            {
                                JSON_ALLOC_MULITBYTE_STRING(bConverting, busInfos[i],"busNo",res.gateInfos[i].busStops[j].busInfos[k].busNo, memory);
                                JSON_ALLOC_MULITBYTE_STRING(bConverting, busInfos[i],"type",res.gateInfos[i].busStops[j].busInfos[k].type, memory);
                                JSON_ALLOC_MULITBYTE_STRING(bConverting, busInfos[i],"blid",res.gateInfos[i].busStops[j].busInfos[k].blid, memory);
                            }
                        }
                    }
                }
            }
        }
    }catch(...){
        return false;
    }

    *ppRes = (NDDSTMAP_FINDSTATIONINFO_RES*)malloc(sizeof(NDDSTMAP_FINDSTATIONINFO_RES));
    memcpy(*ppRes, &res, sizeof(NDDSTMAP_FINDSTATIONINFO_RES));
    return true;
}

// 5.5.6. 역 운행 정보
bool CNddsLibTmap::ParserFindStationTimeTable( NDDSLIB_Json::Value &root,NDDS_MEMORYSTATUS &memory,NDDSTMAP_FINDSTATIONTIMETABLE_RES **ppRes )
{
    CNddsLibCommonData &CommonData = CNddsLibCommonData::GetInstance();
    bool bConverting = CommonData.m_eCharacterSet == NDDSCHARACTERSET_EUCKR?true:false;

    NDDSTMAP_FINDSTATIONTIMETABLE_RES res;
    NDDSLIB_Json::Value timeDetails,firstLastDetails;

    memset(&res, 0x00, sizeof(NDDSTMAP_FINDSTATIONTIMETABLE_RES));

    try
    {
        JSON_ALLOC_MULITBYTE_STRING(bConverting, root,"upWay",res.upWay, memory);
        JSON_ALLOC_MULITBYTE_STRING(bConverting, root,"downWay",res.downWay, memory);
        res.ordUpTimeCount		= root["ordUpTimeCount"].asInt();
        res.ordDownTimeCount	= root["ordDownTimeCount"].asInt();
        res.satUpTimeCount		= root["satUpTimeCount"].asInt();
        res.satDownTimeCount	= root["satDownTimeCount"].asInt();
        res.sunUpTimeCount		= root["sunUpTimeCount"].asInt();
        res.sunDownTimeCount	= root["sunDownTimeCount"].asInt();
        res.firstLastUpCount	= root["firstLastUpCount"].asInt();
        res.firstLastDownCount	= root["firstLastDownCount"].asInt();

        timeDetails = root["timeDetails"];
        firstLastDetails = root["firstLastDetails"];

        int cntTimeDetails = timeDetails.size();
        int cntFirstLastDetails = firstLastDetails.size();

        if(cntTimeDetails > 0)
        {
            JSON_ALLOC_BLOCK(res.timeDetails, NDDSTMAP_TIMEDETAILS_DTO *, sizeof(NDDSTMAP_TIMEDETAILS_DTO) * cntTimeDetails, memory);
            for(int i = 0 ; i < cntTimeDetails ; i++)
            {
                JSON_ALLOC_MULITBYTE_STRING(bConverting, timeDetails[i],"day",res.timeDetails[i].day,memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, timeDetails[i],"flag",res.timeDetails[i].flag,memory);
                res.timeDetails[i].timeIdx = timeDetails[i]["timeIdx"].asInt();
                res.timeDetails[i].timeListCount = timeDetails[i]["timeListCount"].asInt();
                if(res.timeDetails[i].timeListCount > 0)
                {
                    JSON_ALLOC_BLOCK(res.timeDetails[i].timeListValues, char**, sizeof(char*) * res.timeDetails[i].timeListCount, memory);
                    for(int j = 0;j < res.timeDetails[i].timeListCount;j++)
                    {
                        JSON_ALLOC_MULITBYTE_STRING(bConverting, timeDetails[i]["timeListValues"],j,res.timeDetails[i].timeListValues[j],memory);
                    }
                }
            }
        }
        if(cntFirstLastDetails > 0)
        {
            JSON_ALLOC_BLOCK(res.firstLastDetails, NDDSTMAP_FIRSTLASTDETAILS_DTO*, sizeof(NDDSTMAP_FIRSTLASTDETAILS_DTO) * cntFirstLastDetails, memory);
            for(int i = 0 ; i < cntFirstLastDetails ; i++)
            {

                JSON_ALLOC_MULITBYTE_STRING(bConverting, firstLastDetails[i],"firstLastFlag",res.firstLastDetails[i].firstLastFlag,memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, firstLastDetails[i],"guName",		res.firstLastDetails[i].guName,memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, firstLastDetails[i],"ordFirstTime",	res.firstLastDetails[i].ordFirstTime,memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, firstLastDetails[i],"ordLastTime",	res.firstLastDetails[i].ordLastTime,memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, firstLastDetails[i],"satFirstTime",	res.firstLastDetails[i].satFirstTime,memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, firstLastDetails[i],"satLastTime",	res.firstLastDetails[i].satLastTime,memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, firstLastDetails[i],"sunFirstTime",	res.firstLastDetails[i].sunFirstTime,memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, firstLastDetails[i],"sunLastTime",	res.firstLastDetails[i].sunLastTime,memory);
            }
        }
    }
    catch (...)
    {
        return false;
    }

    *ppRes = (NDDSTMAP_FINDSTATIONTIMETABLE_RES*)malloc(sizeof(NDDSTMAP_FINDSTATIONTIMETABLE_RES));
    memcpy(*ppRes, &res, sizeof(NDDSTMAP_FINDSTATIONTIMETABLE_RES));
    return true;
}

bool CNddsLibTmap::ParserFindSubwayImage( NDDSLIB_Json::Value &root,NDDS_MEMORYSTATUS &memory,NDDSTMAP_FINDSUBWAYIMAGE_RES **ppRes )
{
    NDDSTMAP_FINDSUBWAYIMAGE_RES res;

    memset(&res, 0x00, sizeof(NDDSTMAP_FINDSUBWAYIMAGE_RES));

    try
    {
        JSON_ALLOC_BASE64(root,"subwayImage",unsigned char*,res.subwayImage,res.nSizeImage, memory);
    }
    catch (...)
    {
        return false;
    }

    *ppRes = (NDDSTMAP_FINDSUBWAYIMAGE_RES*)malloc(sizeof(NDDSTMAP_FINDSUBWAYIMAGE_RES));
    memcpy(*ppRes, &res, sizeof(NDDSTMAP_FINDSUBWAYIMAGE_RES));
    return true;
}

// 5.5.8. 경로안내(도시간 검색 포함), 5.5.9. 경로안내(도시내 복수 검색) 에서 공통 사용하는 파서
bool CNddsLibTmap::ParserPathFind1Stall4ViewsDto(NDDSLIB_Json::Value &root,NDDS_MEMORYSTATUS &memory, NDDSTMAP_PATHFIND1STALL4VIEWS_DTO**ppDto )
{
    CNddsLibCommonData &CommonData = CNddsLibCommonData::GetInstance();
    bool bConverting = CommonData.m_eCharacterSet == NDDSCHARACTERSET_EUCKR?true:false;

    NDDSTMAP_PATHFIND1STALL4VIEWS_DTO* pDto = *ppDto;
    int listSize = root.size();

    if(listSize > 0)
    {

        for(int i = 0 ; i < listSize ; i++)
        {
            NDDSLIB_Json::Value laneInfos, stationInfos;
            JSON_ALLOC_MULITBYTE_STRING(bConverting, root[i],"transportType",pDto[i].transportType,	memory);
            JSON_ALLOC_MULITBYTE_STRING(bConverting, root[i],"distance",		pDto[i].distance,	memory);
            JSON_ALLOC_MULITBYTE_STRING(bConverting, root[i],"time",			pDto[i].time,	memory);
            JSON_ALLOC_MULITBYTE_STRING(bConverting, root[i],"payment",		pDto[i].payment,	memory);
            JSON_ALLOC_MULITBYTE_STRING(bConverting, root[i],"exNum",		pDto[i].exNum,	memory);
            JSON_ALLOC_MULITBYTE_STRING(bConverting, root[i],"laneVal",		pDto[i].laneVal,	memory);
            pDto[i].laneInfoCount = root[i]["laneInfoCount"].asInt();
            pDto[i].stationCount = root[i]["stationCount"].asInt();

            laneInfos		= root[i]["laneInfos"];
            stationInfos	= root[i]["stationInfos"];

            if(pDto[i].laneInfoCount > 0)
            {
                JSON_ALLOC_BLOCK(pDto[i].laneInfos, NDDSTMAP_LANEINFOS2_DTO*, sizeof(NDDSTMAP_LANEINFOS2_DTO) * pDto[i].laneInfoCount, memory);
                for (int j = 0;j < pDto[i].laneInfoCount ; j ++)
                {
                    JSON_ALLOC_STRING(laneInfos[i],"laneX",pDto[i].laneInfos[i].laneX,memory);
                    JSON_ALLOC_STRING(laneInfos[i],"laneY",pDto[i].laneInfos[i].laneY,memory);
                }
            }

            if(pDto[i].stationCount > 0)
            {
                JSON_ALLOC_BLOCK(pDto[i].stationInfos, NDDSTMAP_STATIONINFOS_DTO*, sizeof(NDDSTMAP_STATIONINFOS_DTO) * pDto[i].stationCount, memory);
                for (int infoIdx = 0;infoIdx < pDto[i].stationCount ; infoIdx ++)
                {
                    JSON_ALLOC_MULITBYTE_STRING(bConverting, stationInfos[infoIdx],"stationType",pDto[i].stationInfos[infoIdx].stationType,memory);
                    JSON_ALLOC_MULITBYTE_STRING(bConverting, stationInfos[infoIdx],"transportFlag",pDto[i].stationInfos[infoIdx].transportFlag,memory);
                    JSON_ALLOC_MULITBYTE_STRING(bConverting, stationInfos[infoIdx],"startName",pDto[i].stationInfos[infoIdx].startName,memory);
                    JSON_ALLOC_MULITBYTE_STRING(bConverting, stationInfos[infoIdx],"startSid",pDto[i].stationInfos[infoIdx].startSid,memory);
                    JSON_ALLOC_MULITBYTE_STRING(bConverting, stationInfos[infoIdx],"startX",pDto[i].stationInfos[infoIdx].startX,memory);
                    JSON_ALLOC_MULITBYTE_STRING(bConverting, stationInfos[infoIdx],"startY",pDto[i].stationInfos[infoIdx].startY,memory);
                    JSON_ALLOC_MULITBYTE_STRING(bConverting, stationInfos[infoIdx],"endName",pDto[i].stationInfos[infoIdx].endName,memory);
                    JSON_ALLOC_MULITBYTE_STRING(bConverting, stationInfos[infoIdx],"endSid",pDto[i].stationInfos[infoIdx].endSid,memory);
                    JSON_ALLOC_MULITBYTE_STRING(bConverting, stationInfos[infoIdx],"endX",	pDto[i].stationInfos[infoIdx].endX,memory);
                    JSON_ALLOC_MULITBYTE_STRING(bConverting, stationInfos[infoIdx],"endY",	pDto[i].stationInfos[infoIdx].endY,memory);
                    JSON_ALLOC_MULITBYTE_STRING(bConverting, stationInfos[infoIdx],"laneAlias",pDto[i].stationInfos[infoIdx].laneAlias,memory);
                    JSON_ALLOC_MULITBYTE_STRING(bConverting, stationInfos[infoIdx],"way",pDto[i].stationInfos[infoIdx].way,memory);
                    JSON_ALLOC_MULITBYTE_STRING(bConverting, stationInfos[infoIdx],"laneColor",pDto[i].stationInfos[infoIdx].laneColor,memory);
                    JSON_ALLOC_MULITBYTE_STRING(bConverting, stationInfos[infoIdx],"trainType",pDto[i].stationInfos[infoIdx].trainType,memory);
                    JSON_ALLOC_MULITBYTE_STRING(bConverting, stationInfos[infoIdx],"trainCode",pDto[i].stationInfos[infoIdx].trainCode,memory);
                    pDto[i].stationInfos[infoIdx].busInfoCount	= stationInfos[infoIdx]["busInfoCount"].asInt();
                    pDto[i].stationInfos[infoIdx].stopCount		= stationInfos[infoIdx]["stopCount"].asInt();

                    if(pDto[i].stationInfos[infoIdx].busInfoCount > 0)
                    {
                        JSON_ALLOC_BLOCK(pDto[i].stationInfos[infoIdx].busInfos, NDDSTMAP_BUSINFOS_DTO *, sizeof(NDDSTMAP_BUSINFOS_DTO ) * pDto[i].stationInfos[infoIdx].busInfoCount, memory);
                        NDDSLIB_Json::Value busInfo = stationInfos[infoIdx]["busInfos"];
                        for (int busIdx = 0;busIdx < pDto[i].stationInfos[infoIdx].busInfoCount;busIdx++)
                        {
                            JSON_ALLOC_MULITBYTE_STRING(bConverting, busInfo[busIdx],"busNo",	pDto[i].stationInfos[infoIdx].busInfos[busIdx].busNo,memory);
                            JSON_ALLOC_MULITBYTE_STRING(bConverting, busInfo[busIdx],"blId",	pDto[i].stationInfos[infoIdx].busInfos[busIdx].blid,memory);
                            JSON_ALLOC_MULITBYTE_STRING(bConverting, busInfo[busIdx],"type",	pDto[i].stationInfos[infoIdx].busInfos[busIdx].type,memory);
                        }
                    }
                }
            }
        }
    }
    return true;
}

// 5.5.8. 경로안내(도시간 검색 포함)
bool CNddsLibTmap::ParserFindPathFind1stAll4( NDDSLIB_Json::Value &root,NDDS_MEMORYSTATUS &memory,NDDSTMAP_FINDPATHFIND1STALL4_RES **ppRes )
{
    CNddsLibCommonData &CommonData = CNddsLibCommonData::GetInstance();
    bool bConverting = CommonData.m_eCharacterSet == NDDSCHARACTERSET_EUCKR?true:false;

    NDDSTMAP_FINDPATHFIND1STALL4_RES res;
    NDDSLIB_Json::Value pathFind1Stall4Views;

    memset(&res,0x00, sizeof(NDDSTMAP_FINDPATHFIND1STALL4_RES));

    try
    {
        JSON_ALLOC_MULITBYTE_STRING(bConverting, root,"searchType",res.searchType,memory);
        JSON_ALLOC_MULITBYTE_STRING(bConverting, root,"outTrafficCheck",res.outTrafficCheck,memory);
        JSON_ALLOC_MULITBYTE_STRING(bConverting, root,"startCid",res.startCid,memory);
        JSON_ALLOC_MULITBYTE_STRING(bConverting, root,"endCid",res.endCid,memory);
        res.totalCount = root["totalCount"].asInt();

        pathFind1Stall4Views = root["pathFind1Stall4Views"];
        if(res.totalCount > 0)
        {
            JSON_ALLOC_BLOCK(res.pathFind1Stall4Views, NDDSTMAP_PATHFIND1STALL4VIEWS_DTO *, sizeof(NDDSTMAP_PATHFIND1STALL4VIEWS_DTO ) * res.totalCount, memory);
            ParserPathFind1Stall4ViewsDto(pathFind1Stall4Views,memory,&res.pathFind1Stall4Views);
        }
    }
    catch (...)
    {
        return false;
    }

    *ppRes = (NDDSTMAP_FINDPATHFIND1STALL4_RES*)malloc(sizeof(NDDSTMAP_FINDPATHFIND1STALL4_RES));
    memcpy(*ppRes, &res, sizeof(NDDSTMAP_FINDPATHFIND1STALL4_RES));

    return true;
}

// 5.5.9. 경로안내(도시내 복수 검색)
bool CNddsLibTmap::ParserFindPathFind1stAll4Plural( NDDSLIB_Json::Value &root,NDDS_MEMORYSTATUS &memory,NDDSTMAP_FINDPATHFIND1STALL4PLURAL_RES **ppRes )
{
    CNddsLibCommonData &CommonData = CNddsLibCommonData::GetInstance();
    bool bConverting = CommonData.m_eCharacterSet == NDDSCHARACTERSET_EUCKR?true:false;

    NDDSTMAP_FINDPATHFIND1STALL4PLURAL_RES res;
    memset(&res, 0x00, sizeof(NDDSTMAP_FINDPATHFIND1STALL4PLURAL_RES));
    NDDSLIB_Json::Value startPathFind1Stall4Views,endPathFind1Stall4Views;
    try
    {
        res.startCount	= root["startCount"].asInt();
        res.endCount	= root["endCount"].asInt();
        startPathFind1Stall4Views	= root["startPathFind1Stall4Views"];
        endPathFind1Stall4Views		= root["endPathFind1Stall4Views"];

        // startPathFind1Stall4Views 출발도시 대중 교통 경로안내 정보 리스트
        if(res.startCount > 0)
        {
            JSON_ALLOC_BLOCK(res.startPathFind1Stall4Views,NDDSTMAP_PATHFIND1STALL4VIEWS_DTO *,sizeof(NDDSTMAP_PATHFIND1STALL4VIEWS_DTO)*res.startCount, memory);
            ParserPathFind1Stall4ViewsDto(startPathFind1Stall4Views,memory,&res.startPathFind1Stall4Views);
        }

        // endPathFind1Stall4Views 목적도시 대중 교통 경로안내 정보 리스트
        if(res.endCount > 0)
        {
            JSON_ALLOC_BLOCK(res.endPathFind1Stall4Views,NDDSTMAP_PATHFIND1STALL4VIEWS_DTO *,sizeof(NDDSTMAP_PATHFIND1STALL4VIEWS_DTO)*res.endCount, memory);
            ParserPathFind1Stall4ViewsDto(endPathFind1Stall4Views,memory,&res.endPathFind1Stall4Views);
        }

    }
    catch (...)
    {
        return false;
    }

    *ppRes = (NDDSTMAP_FINDPATHFIND1STALL4PLURAL_RES*)malloc(sizeof(NDDSTMAP_FINDPATHFIND1STALL4PLURAL_RES));
    memcpy(*ppRes, &res, sizeof(NDDSTMAP_FINDPATHFIND1STALL4PLURAL_RES));

    return true;
}
// 5.5.10. 주변 정류장 / 역 조회(거리순)
bool CNddsLibTmap::ParserFindNearStation( NDDSLIB_Json::Value &root,NDDS_MEMORYSTATUS &memory,NDDSTMAP_FINDNEARSTATION_RES **ppRes )
{
    CNddsLibCommonData &CommonData = CNddsLibCommonData::GetInstance();
    bool bConverting = CommonData.m_eCharacterSet == NDDSCHARACTERSET_EUCKR?true:false;

    NDDSTMAP_FINDNEARSTATION_RES res;
    memset(&res, 0x00, sizeof(NDDSTMAP_FINDNEARSTATION_RES));

    try
    {
        res.nearStationCount = root["nearStationCount"].asInt();
        if(res.nearStationCount > 0)
        {
            NDDSLIB_Json::Value nearStations = root["nearStations"];
            JSON_ALLOC_BLOCK(res.nearStations,NDDSTMAP_NEARSTATIONS_DTO *,sizeof(NDDSTMAP_NEARSTATIONS_DTO)*res.nearStationCount,memory);
            for (int i = 0 ; i < res.nearStationCount ; i++)
            {

                res.nearStations[i].flag = nearStations[i]["flag"].asInt();
                JSON_ALLOC_MULITBYTE_STRING(bConverting, root,"dist",	res.nearStations[i].dist,memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, root,"sid",		res.nearStations[i].sid,memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, root,"sidRt",	res.nearStations[i].sidRt,memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, root,"navX",	res.nearStations[i].navX,memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, root,"navY",	res.nearStations[i].navY,memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, root,"stationName",res.nearStations[i].stationName,memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, root,"dongName",res.nearStations[i].dongName,memory);
            }
        }
    }
    catch (...)
    {
        return false;
    }

    *ppRes = (NDDSTMAP_FINDNEARSTATION_RES*)malloc(sizeof(NDDSTMAP_FINDNEARSTATION_RES));
    memcpy(*ppRes, &res, sizeof(NDDSTMAP_FINDNEARSTATION_RES));


    return true;
}
// 5.5.11. 통합 검색
bool CNddsLibTmap::ParserFindAllTransport( NDDSLIB_Json::Value &root,NDDS_MEMORYSTATUS &memory,NDDSTMAP_FINDALLTRANSPORT_RES **ppRes )
{
    CNddsLibCommonData &CommonData = CNddsLibCommonData::GetInstance();
    bool bConverting = CommonData.m_eCharacterSet == NDDSCHARACTERSET_EUCKR?true:false;

    NDDSTMAP_FINDALLTRANSPORT_RES res;
    memset(&res, 0x00, sizeof(NDDSTMAP_FINDALLTRANSPORT_RES));
    NDDSLIB_Json::Value publicInfo,poiInfo;
    try
    {
        publicInfo	= root["publicInfo"];
        poiInfo		= root["poiInfo"];

        // 대중교통 통합검색 정보
        res.publicInfo.transportCount = publicInfo["transportCount"].asInt();
        if(res.publicInfo.transportCount > 0)
        {
            JSON_ALLOC_BLOCK(res.publicInfo.publicInfoDetails,NDDSTMAP_PUBLICINFODETAILS_DTO*,sizeof(NDDSTMAP_PUBLICINFODETAILS_DTO)*res.publicInfo.transportCount,memory);
            NDDSLIB_Json::Value publicInfoDetails = publicInfo["publicInfoDetails"];
            for(int k = 0;k < res.publicInfo.transportCount ; k++)
            {
                res.publicInfo.publicInfoDetails[k].flag = publicInfoDetails[k]["flag"].asInt();
                res.publicInfo.publicInfoDetails[k].type = publicInfoDetails[k]["type"].asInt();
                JSON_ALLOC_MULITBYTE_STRING(bConverting, publicInfoDetails[k],"name",	res.publicInfo.publicInfoDetails[k].name,	memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, publicInfoDetails[k],"dong",	res.publicInfo.publicInfoDetails[k].dong,	memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, publicInfoDetails[k],"sid",		res.publicInfo.publicInfoDetails[k].sid,		memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, publicInfoDetails[k],"sidRt",	res.publicInfo.publicInfoDetails[k].sidRt,	memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, publicInfoDetails[k],"navX",	res.publicInfo.publicInfoDetails[k].navX,	memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, publicInfoDetails[k],"navY",	res.publicInfo.publicInfoDetails[k].navY,	memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, publicInfoDetails[k],"cid",		res.publicInfo.publicInfoDetails[k].cid,		memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, publicInfoDetails[k],"cityName",res.publicInfo.publicInfoDetails[k].cityName,memory);
            }
        }

        // POI 통합검색 정보 poiInfo
        res.poiInfo.totalCnt	= poiInfo["totalCnt"].asInt();
        res.poiInfo.listCnt		= poiInfo["listCnt"].asInt();
        res.poiInfo.areaCnt		= poiInfo["areaCnt"].asInt();
        JSON_ALLOC_MULITBYTE_STRING(bConverting, poiInfo,"contFlag",res.poiInfo.contFlag,memory);
        JSON_ALLOC_MULITBYTE_STRING(bConverting, poiInfo,"areaNm",res.poiInfo.areaNm,memory);

        NDDSLIB_Json::Value poiInfoDetails = poiInfo["poiInfoDetails"];
        int cntDetails = poiInfoDetails.size();
        if(cntDetails > 0)
        {
            JSON_ALLOC_BLOCK(res.poiInfo.poiInfoDetails,NDDSTMAP_POIINFODETAILS_DTO*,sizeof(NDDSTMAP_POIINFODETAILS_DTO)*cntDetails,memory);
            for(int i = 0;i < cntDetails ; i++)
            {
                JSON_ALLOC_MULITBYTE_STRING(bConverting, poiInfoDetails[i],"dbKind",	res.poiInfo.poiInfoDetails[i].dbKind,memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, poiInfoDetails[i],"poiId",		res.poiInfo.poiInfoDetails[i].poiId,memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, poiInfoDetails[i],"name",		res.poiInfo.poiInfoDetails[i].name,memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, poiInfoDetails[i],"orgName",	res.poiInfo.poiInfoDetails[i].orgName,memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, poiInfoDetails[i],"navX1",		res.poiInfo.poiInfoDetails[i].navX1,memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, poiInfoDetails[i],"navY1",		res.poiInfo.poiInfoDetails[i].navY1,memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, poiInfoDetails[i],"centerX",	res.poiInfo.poiInfoDetails[i].centerX,memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, poiInfoDetails[i],"centerY",	res.poiInfo.poiInfoDetails[i].centerY,memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, poiInfoDetails[i],"rpFlag",	res.poiInfo.poiInfoDetails[i].rpFlag,memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, poiInfoDetails[i],"parkYn",	res.poiInfo.poiInfoDetails[i].parkYn,memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, poiInfoDetails[i],"phoneNum",	res.poiInfo.poiInfoDetails[i].phoneNum,memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, poiInfoDetails[i],"addr",		res.poiInfo.poiInfoDetails[i].addr,memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, poiInfoDetails[i],"primary",	res.poiInfo.poiInfoDetails[i].primary,memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, poiInfoDetails[i],"secondary",	res.poiInfo.poiInfoDetails[i].secondary,memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, poiInfoDetails[i],"mlClass",	res.poiInfo.poiInfoDetails[i].mlClass,memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, poiInfoDetails[i],"lcdName",	res.poiInfo.poiInfoDetails[i].lcdName,memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, poiInfoDetails[i],"mcdName",	res.poiInfo.poiInfoDetails[i].mcdName,memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, poiInfoDetails[i],"scdName",	res.poiInfo.poiInfoDetails[i].scdName,memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, poiInfoDetails[i],"dcdName",	res.poiInfo.poiInfoDetails[i].dcdName,memory);
            }
        }
    }
    catch (...)
    {
        return false;
    }

    *ppRes = (NDDSTMAP_FINDALLTRANSPORT_RES*)malloc(sizeof(NDDSTMAP_FINDALLTRANSPORT_RES));
    memcpy(*ppRes, &res, sizeof(NDDSTMAP_FINDALLTRANSPORT_RES));
    return true;
}
//5.5.12. 주변 CCTV 리스트
bool CNddsLibTmap::ParserFindCCTVResult( NDDSLIB_Json::Value &root,NDDS_MEMORYSTATUS &memory,NDDSTMAP_FINDCCTVRESULT_RES **ppRes )
{
    CNddsLibCommonData &CommonData = CNddsLibCommonData::GetInstance();
    bool bConverting = CommonData.m_eCharacterSet == NDDSCHARACTERSET_EUCKR?true:false;

    NDDSTMAP_FINDCCTVRESULT_RES res;
    memset(&res, 0x00, sizeof(NDDSTMAP_FINDCCTVRESULT_RES));
    NDDSLIB_Json::Value cctvResultDetails;
    try
    {
        res.cctvCount = root["cctvCount"].asInt();
        cctvResultDetails = root["cctvResultDetails"];
        if(res.cctvCount > 0)
        {
            JSON_ALLOC_BLOCK(res.cctvResultDetails,NDDSTMAP_CCTVRESULTDETAILS_DTO*,sizeof(NDDSTMAP_CCTVRESULTDETAILS_DTO)*res.cctvCount,memory);
            for(int i = 0;i < res.cctvCount ; i++)
            {
                res.cctvResultDetails[i].cctvId = cctvResultDetails[i]["cctvId"].asInt();
                JSON_ALLOC_MULITBYTE_STRING(bConverting, cctvResultDetails[i],"navX",	res.cctvResultDetails[i].navX,memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, cctvResultDetails[i],"navY",	res.cctvResultDetails[i].navY,memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, cctvResultDetails[i],"useYn",	res.cctvResultDetails[i].useYn,memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, cctvResultDetails[i],"serviceYn",res.cctvResultDetails[i].serviceYn,memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, cctvResultDetails[i],"name",	res.cctvResultDetails[i].name,memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, cctvResultDetails[i],"offer",	res.cctvResultDetails[i].offer,memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, cctvResultDetails[i],"time",	res.cctvResultDetails[i].time,memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, cctvResultDetails[i],"cctvUrl",	res.cctvResultDetails[i].cctvUrl,memory);
            }
        }
    }
    catch (...)
    {
        return false;
    }

    *ppRes = (NDDSTMAP_FINDCCTVRESULT_RES*)malloc(sizeof(NDDSTMAP_FINDCCTVRESULT_RES));
    memcpy(*ppRes, &res, sizeof(NDDSTMAP_FINDCCTVRESULT_RES));
    return true;
}
// 5.5.13. CCTV 정보
bool CNddsLibTmap::ParserFindCCTVResultById( NDDSLIB_Json::Value &root,NDDS_MEMORYSTATUS &memory,NDDSTMAP_FINDCCTVRESULTBYID_RES **ppRes )
{
    CNddsLibCommonData &CommonData = CNddsLibCommonData::GetInstance();
    bool bConverting = CommonData.m_eCharacterSet == NDDSCHARACTERSET_EUCKR?true:false;

    NDDSTMAP_FINDCCTVRESULTBYID_RES res;
    memset(&res, 0x00, sizeof(NDDSTMAP_FINDCCTVRESULTBYID_RES));

    try
    {
        res.cctvId = root["cctvId"].asInt();
        JSON_ALLOC_MULITBYTE_STRING(bConverting, root,"navX",	res.navX,memory);
        JSON_ALLOC_MULITBYTE_STRING(bConverting, root,"navY",	res.navY,memory);
        JSON_ALLOC_MULITBYTE_STRING(bConverting, root,"useYn",	res.useYn,memory);
        JSON_ALLOC_MULITBYTE_STRING(bConverting, root,"serviceYn",res.serviceYn,memory);
        JSON_ALLOC_MULITBYTE_STRING(bConverting, root,"name",	res.name,memory);
        JSON_ALLOC_MULITBYTE_STRING(bConverting, root,"offer",	res.offer,memory);
        JSON_ALLOC_MULITBYTE_STRING(bConverting, root,"time",	res.time,memory);
        JSON_ALLOC_MULITBYTE_STRING(bConverting, root,"cctvUrl",	res.cctvUrl,memory);
    }
    catch (...)
    {
        return false;
    }

    *ppRes = (NDDSTMAP_FINDCCTVRESULTBYID_RES*)malloc(sizeof(NDDSTMAP_FINDCCTVRESULTBYID_RES));
    memcpy(*ppRes, &res, sizeof(NDDSTMAP_FINDCCTVRESULTBYID_RES));
    return true;
}

// 5.6.3 단말인증 (for Smart Car)
bool CNddsLibTmap::MakeAuthForSmartCar( NDDSLIB_Json::Value &root, NDDSTMAP_AUTHFORSMARTCAR_REQ *pReq )
{
    NDDSLIB_Json::Value headUnitDeviceInfo, tidAuthReqInfo, tidAuthInfo;
    CNddsLibCommonData &CommonData = CNddsLibCommonData::GetInstance();
    try
    {
        headUnitDeviceInfo["deviceId"] = CommonData.m_sTmapNddsCommonHeader.deviceIdHu;
        headUnitDeviceInfo["deviceModelName"] = CommonData.m_sTmapNddsCommonHeader.deviceModleName;
        headUnitDeviceInfo["deviceModelNo"] = CommonData.m_sTmapNddsCommonHeader.deviceModelNo;
        headUnitDeviceInfo["vendor"] = CommonData.m_sTmapNddsCommonHeader.vendor;
        headUnitDeviceInfo["serialKey"] = CommonData.m_sTmapNddsCommonHeader.serialKey;
        root["headUnitDeviceInfo"]=headUnitDeviceInfo;

		if(pReq->otpCode)
			root["otpCode"]=pReq->otpCode;

    }catch (...)
    {
        return false;
    }

    return true;
}

// 5.6.3 단말인증 (for Smart Car)
bool CNddsLibTmap::ParserAuthForSmartCar( NDDSLIB_Json::Value &root,NDDS_MEMORYSTATUS &memory,NDDSTMAP_AUTHFORSMARTCAR_RES **ppRes )
{
    CNddsLibCommonData &CommonData = CNddsLibCommonData::GetInstance();
    bool bConverting = CommonData.m_eCharacterSet == NDDSCHARACTERSET_EUCKR?true:false;

    NDDSTMAP_AUTHFORSMARTCAR_RES res;
    NDDSLIB_Json::Value commonInfo, authInfo, externalUserInfos, headUnitDeviceAuthInfo, tidAuthResInfo, versionInfo, appUpdateInfo, appControlInfo, appControlDetails, pushInfo, userProfileInfo, personalInfo, carInfo;

    memset(&res, 0x00, sizeof(NDDSTMAP_AUTHFORSMARTCAR_RES));

    try{
        commonInfo = root["commonInfo"];
        JSON_ALLOC_BLOCK(res.commonInfo, NDDSTMAP_COMMONINFO_DTO*, sizeof(NDDSTMAP_COMMONINFO_DTO), memory);
        JSON_ALLOC_MULITBYTE_STRING(bConverting, commonInfo, "euk", res.commonInfo->euk, memory);
        JSON_ALLOC_MULITBYTE_STRING(bConverting, commonInfo, "eid", res.commonInfo->eid, memory);

        authInfo = root["authInfo"];
        JSON_ALLOC_BLOCK(res.authInfo, NDDSTMAP_AUTHINFO_DTO*, sizeof(NDDSTMAP_AUTHINFO_DTO), memory);
        res.authInfo->validateCode = authInfo["validateCode"].asInt();
        JSON_ALLOC_MULITBYTE_STRING(bConverting, authInfo, "validationSubField", res.authInfo->validationSubField, memory);
        externalUserInfos = authInfo["externalUserInfos"];
        res.authInfo->externalUserInfoCount = externalUserInfos.size();
        if(res.authInfo->externalUserInfoCount > 0)
        {
            JSON_ALLOC_BLOCK(res.authInfo->externalUserInfos, NDDSTMAP_EXTERNALUSERINOFS_DTO*, sizeof(NDDSTMAP_EXTERNALUSERINOFS_DTO)*res.authInfo->externalUserInfoCount, memory);
            for(int i = 0 ; i < res.authInfo->externalUserInfoCount ; i++)
            {
                JSON_ALLOC_MULITBYTE_STRING(bConverting, externalUserInfos[i], "provider", res.authInfo->externalUserInfos[i].provider, memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, externalUserInfos[i], "readableUserId", res.authInfo->externalUserInfos[i].readableUserId, memory);
            }
        }

        headUnitDeviceAuthInfo = root["headUnitDeviceAuthInfo"];
        JSON_ALLOC_BLOCK(res.headUnitDeviceAuthInfo, NDDSTMAP_HEADUNITDEVICEAUTHINFO_DTO*, sizeof(NDDSTMAP_HEADUNITDEVICEAUTHINFO_DTO), memory);
        res.headUnitDeviceAuthInfo->resultCode = headUnitDeviceAuthInfo["resultCode"].asInt();
        res.headUnitDeviceAuthInfo->resultSubField = headUnitDeviceAuthInfo["resultSubField"].asInt();

        tidAuthResInfo = root["tidAuthResInfo"];
        JSON_ALLOC_BLOCK(res.tidAuthResInfo, NDDSTMAP_TIDAUTHRESINFO_DTO*, sizeof(NDDSTMAP_TIDAUTHRESINFO_DTO), memory);
        res.tidAuthResInfo->resultCode = tidAuthResInfo["resultCode"].asInt();
        res.tidAuthResInfo->resultSubField = tidAuthResInfo["resultSubField"].asInt();
        res.tidAuthResInfo->resultMdnAuthInfo = tidAuthResInfo["resultMdnAuthInfo"].asInt();
        externalUserInfos.clear();
        externalUserInfos = tidAuthResInfo["externalUserInfos"];
        res.tidAuthResInfo->externalUserInfoCount = externalUserInfos.size();
        if(res.tidAuthResInfo->externalUserInfoCount > 0)
        {
            JSON_ALLOC_BLOCK(res.tidAuthResInfo->externalUserInfos, NDDSTMAP_EXTERNALUSERINOFS_DTO*, sizeof(NDDSTMAP_EXTERNALUSERINOFS_DTO)*res.tidAuthResInfo->externalUserInfoCount, memory);
            for(int i = 0 ; i < res.tidAuthResInfo->externalUserInfoCount ; i++)
            {
                JSON_ALLOC_MULITBYTE_STRING(bConverting, externalUserInfos[i], "provider", res.tidAuthResInfo->externalUserInfos[i].provider, memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, externalUserInfos[i], "readableUserId", res.tidAuthResInfo->externalUserInfos[i].readableUserId, memory);
            }
        }

        versionInfo = root["versionInfo"];
        JSON_ALLOC_BLOCK(res.versionInfo, NDDSTMAP_VERSIONINFO_DTO*, sizeof(NDDSTMAP_VERSIONINFO_DTO), memory);
        JSON_ALLOC_MULITBYTE_STRING(bConverting, versionInfo, "menuNoticeVer", res.versionInfo->menuNoticeVer, memory);
        JSON_ALLOC_MULITBYTE_STRING(bConverting, versionInfo, "openappVer", res.versionInfo->openappVer, memory);

        appUpdateInfo = root["appUpdateInfo"];
        JSON_ALLOC_BLOCK(res.appUpdateInfo, NDDSTMAP_APPUPDATEINFO_DTO*, sizeof(NDDSTMAP_APPUPDATEINFO_DTO), memory);
        JSON_ALLOC_MULITBYTE_STRING(bConverting, appUpdateInfo, "appUpdateType", res.appUpdateInfo->appUpdateType, memory);
        JSON_ALLOC_MULITBYTE_STRING(bConverting, appUpdateInfo, "appVer", res.appUpdateInfo->appVer, memory);
        JSON_ALLOC_MULITBYTE_STRING(bConverting, appUpdateInfo, "appUpdateViewFlag", res.appUpdateInfo->appUpdateViewFlag, memory);
        JSON_ALLOC_MULITBYTE_STRING(bConverting, appUpdateInfo, "appUpdateUrl", res.appUpdateInfo->appUpdateUrl, memory);
        JSON_ALLOC_MULITBYTE_STRING(bConverting, appUpdateInfo, "appUpdateMsg", res.appUpdateInfo->appUpdateMsg, memory);

        appControlInfo = root["appControlInfo"];
        JSON_ALLOC_BLOCK(res.appControlInfo, NDDSTMAP_APPCONTROLINFO_DTO*, sizeof(NDDSTMAP_APPCONTROLINFO_DTO), memory);
        res.appControlInfo->controlInfoCount = appControlInfo["controlInfoCount"].asInt();
        if(res.appControlInfo->controlInfoCount > 0)
        {
            appControlDetails = appControlInfo["controlInfoDetails"];

            JSON_ALLOC_BLOCK(res.appControlInfo->controlInfoDetails, NDDSTMAP_CONTROLINFODETAILS_DTO*, sizeof(NDDSTMAP_CONTROLINFODETAILS_DTO) * res.appControlInfo->controlInfoCount, memory);
            for(int i = 0 ; i < res.appControlInfo->controlInfoCount ; i++)
            {
                JSON_ALLOC_MULITBYTE_STRING(bConverting, appControlDetails[i], "serviceName", res.appControlInfo->controlInfoDetails[i].serviceName, memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, appControlDetails[i], "serviceYn", res.appControlInfo->controlInfoDetails[i].serviceYn, memory);
                JSON_ALLOC_MULITBYTE_STRING(bConverting, appControlDetails[i], "serviceItems", res.appControlInfo->controlInfoDetails[i].serviceItems, memory);
            }
        }

        pushInfo = root["pushInfo"];
        JSON_ALLOC_BLOCK(res.pushInfo, NDDSTMAP_PUSHINFO_DTO*, sizeof(NDDSTMAP_PUSHINFO_DTO), memory);
        JSON_ALLOC_MULITBYTE_STRING(bConverting, pushInfo, "pushSetYn", res.pushInfo->pushSetYn, memory);
        JSON_ALLOC_MULITBYTE_STRING(bConverting, pushInfo, "smsYn", res.pushInfo->smsYn, memory);
        JSON_ALLOC_MULITBYTE_STRING(bConverting, pushInfo, "momentYn", res.pushInfo->momentYn, memory);

        userProfileInfo = root["userProfileInfo"];
        JSON_ALLOC_BLOCK(res.userProfileInfo, NDDSTMAP_USERPROFILEINFO_DTO*, sizeof(NDDSTMAP_USERPROFILEINFO_DTO), memory);
        personalInfo = userProfileInfo["personalInfo"];
        JSON_ALLOC_BLOCK(res.userProfileInfo->personalInfo, NDDSTMAP_PERSONALINFO_DTO*, sizeof(NDDSTMAP_PERSONALINFO_DTO), memory);
        JSON_ALLOC_MULITBYTE_STRING(bConverting, personalInfo, "email", res.userProfileInfo->personalInfo->email, memory);
        JSON_ALLOC_MULITBYTE_STRING(bConverting, personalInfo, "userName", res.userProfileInfo->personalInfo->userName, memory);
        JSON_ALLOC_MULITBYTE_STRING(bConverting, personalInfo, "mdn", res.userProfileInfo->personalInfo->mdn, memory);
        JSON_ALLOC_MULITBYTE_STRING(bConverting, personalInfo, "birth", res.userProfileInfo->personalInfo->birth, memory);
        carInfo = userProfileInfo["carInfo"];
        JSON_ALLOC_BLOCK(res.userProfileInfo->carInfo, NDDSTMAP_CARINFO_DTO*, sizeof(NDDSTMAP_CARINFO_DTO), memory);
        JSON_ALLOC_MULITBYTE_STRING(bConverting, carInfo, "email", res.userProfileInfo->carInfo->model, memory);
        JSON_ALLOC_MULITBYTE_STRING(bConverting, carInfo, "userName", res.userProfileInfo->carInfo->fuel, memory);
        JSON_ALLOC_MULITBYTE_STRING(bConverting, carInfo, "mdn", res.userProfileInfo->carInfo->hipassYn, memory);
        JSON_ALLOC_MULITBYTE_STRING(bConverting, carInfo, "birth", res.userProfileInfo->carInfo->number, memory);

    }catch(...){
        return false;
    }

    *ppRes = (NDDSTMAP_AUTHFORSMARTCAR_RES*)malloc(sizeof(NDDSTMAP_AUTHFORSMARTCAR_RES));
    memcpy(*ppRes, &res, sizeof(NDDSTMAP_AUTHFORSMARTCAR_RES));
    return true;
}

bool CNddsLibTmap::ParserNonNddsResult(char *resData, int contentLength, NDDS_MEMORYSTATUS &memory, NDDSTMAP_NONNDDS_RES **ppRes)
{
    NDDSTMAP_NONNDDS_RES res;
    bool bRet;
    memset(&res, 0x00, sizeof(NDDSTMAP_NONNDDS_RES));

    bool isBinary = false;
    char pngSignature[] = { 0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A };

    // 응답 데이터 파싱
    char *resStart = strstr(resData, "{");
    if (resStart == nullptr) {
        for (int ii = 0; ii < sizeof(pngSignature); ii++) {
            if (resData[ii] != pngSignature[ii]) {
                m_eLastErrCode = NDDSERR_RES_BODY;
                return false;
            }
        }
        isBinary = true;
    }

//    if(atoi(common_header.errorCode) == 0)	// common_header 사용 안 하기 때문에 주석처리함. 주석 해제하면 크래시 발생
    {
        try {
            if (isBinary) {
                unsigned char* pResData = (unsigned char*)resData;
                unsigned char* pResult;
                ALLOC_MEM_COPY(pResData, pResult, contentLength, memory);
                res.pszResult = (char*)pResult;
                res.nResult = contentLength;
            } else {
                ALLOC_STR_COPY(resStart, res.pszResult, strlen(resStart)+1, memory);
                res.pszResult[strlen(resStart)] = 0;
                res.nResult = strlen(resStart)+1;
            }
            bRet = true;
        }
        catch (...)
        {
            return false;
        }
    }
    *ppRes = (NDDSTMAP_NONNDDS_RES*)malloc(sizeof(NDDSTMAP_NONNDDS_RES));
    memcpy(*ppRes, &res, sizeof(NDDSTMAP_NONNDDS_RES));

    return bRet;
}
