#include <QDebug>

#include "routesearch.h"
#include "environment.h"
#include "MNS_Interface.h"
#include "MNS_Define.h"
#include "NDDS_Define.h"
#include "NDDS_Interface.h"

#include <QFile>
#include <QDebug>
#include <QTextCodec>
#include <QDebug>
namespace SKT {

CRouteSearch* CRouteSearch::s_instance = NULL;

CRouteSearch::CRouteSearch()
{
    m_init  = false;
}

CRouteSearch::~CRouteSearch()
{
    if (s_instance) {
        s_instance->destroy();
        s_instance = NULL;
    }
}

QString CRouteSearch::toUnicode(const char* cstr){
    QTextCodec* mCpEUCKr;
    mCpEUCKr = QTextCodec::codecForName("eucKR");
    return (QString)mCpEUCKr->toUnicode(cstr);
}

void CRouteSearch::init()
{
    if (true == m_init) {
        return;
    }

    m_init = true;

#ifdef BUILD_TARGET_MNS
    QString rootPath = SKT::Environment::getMapPath() + "/TmapNavi";
    QString extendPath = SKT::Environment::getRouteSearchPath();

    TS_ENGINEINIT_EX sInitEx = {0,};
    strcpy(sInitEx.szAuthCode, "48ed800029ae");
    sInitEx.eOSType = E_OSTYPE_LINUX;
    strcpy(sInitEx.szOSVersion, "4.3");
    strcpy(sInitEx.szModelNo, "MM2018");
    strcpy(sInitEx.szAppVersion, "1.0.1");
    strcpy(sInitEx.szAppBuildNo, "000001");

   MNS_Initialize(rootPath.toLocal8Bit().data()
                  , extendPath.toLocal8Bit().data()
                  , "FGACUI5D"
                  , true
                  , callbackSearchStatus
                  , &sInitEx);

   DS_NDDSINIT_EX sNddsInitEx = {0,};
   sNddsInitEx.eCharSet = DE_CHARACTERSET_UTF8;
   sNddsInitEx.bServerCommerce = false;
   strcpy(sNddsInitEx.szAuthCode, "48ed800029ae");
   sNddsInitEx.eOSType = DE_OSTYPE_LINUX;
   strcpy(sNddsInitEx.szOSVersion, "4.3");
   strcpy(sNddsInitEx.szModelNo, "MM2018");
   strcpy(sNddsInitEx.szAppVersion, "1.0.1");
   strcpy(sNddsInitEx.szAppBuildNo, "000001");

   NDDS_Initialize(true, callbackNddsStatus, &sNddsInitEx);
#else
#endif
}

void CRouteSearch::route() {
#ifdef BUILD_TARGET_MNS
    bool isConnected = false; //server
    if(isConnected == false)
    {
        TS_ROUTE_REQ sRouteReq;
        TS_ROUTE_RES sRouteRes;
        memset(&sRouteReq, 0x00, sizeof(TS_ROUTE_REQ));
        memset(&sRouteRes, 0x00, sizeof(TS_ROUTE_RES));

        sRouteReq.eRouteType = E_ROUTETYPE_NORMAL;
        sRouteReq.nRouteOption = E_ROUTEOPTION_OPTIMUM;
        sRouteReq.nRouteTrafficType = E_ROUTETRAFFICTYPE_PATTERN;
        sRouteReq.bNVXInfoCollect = false;
        sRouteReq.bStartGpsPos = false;
        sRouteReq.eVehicleClass = E_VEHICLECLASS_2;
        sRouteReq.eOilType = E_OILTYPE_GASOLINE;
        sRouteReq.nPosCnt = 5;

        //seoul
        strcpy(sRouteReq.sPosInfo[0].szName, "start position");
        sRouteReq.sPosInfo[0].tp.x = 66569576;
        sRouteReq.sPosInfo[0].tp.y = 19687233;
        //waypoint 1
        sRouteReq.sPosInfo[1].tp.x = 66597632;
        sRouteReq.sPosInfo[1].tp.y = 19655016;
        //waypoint 2
        sRouteReq.sPosInfo[2].tp.x = 66574165;
        sRouteReq.sPosInfo[2].tp.y = 19695505;
        //waypoint 3
        sRouteReq.sPosInfo[3].tp.x = 66662899;
        sRouteReq.sPosInfo[3].tp.y = 19297294;
        //busan
        strcpy(sRouteReq.sPosInfo[4].szName, "goal position");
        sRouteReq.sPosInfo[4].tp.x = 67655769;
        sRouteReq.sPosInfo[4].tp.y = 18408392;
        QString LocalSearchPath = SKT::Environment::getRouteSearchPath() + "/RP/Route_Tvas_Local.dat";
        strcpy(sRouteRes.szCollectDataPath, LocalSearchPath.toLocal8Bit().data());
        MNS_Route(&sRouteReq, isConnected, false, &sRouteRes);
    }
    else
    {
        DS_ROUTE_REQ sNddsRouteReq;
        memset(&sNddsRouteReq, 0x00, sizeof(DS_ROUTE_REQ));

        sNddsRouteReq.eRouteType = DE_ROUTETYPE_NORMAL;
        // 다중탐색 추가
        sNddsRouteReq.nRouteOption = DE_ROUTEOPTION_OPTIMUM | DE_ROUTEOPTION_FREEWAY | DE_ROUTEOPTION_SHORTESTWAY;
        sNddsRouteReq.bStartGpsPos = false;
        sNddsRouteReq.eVehicleClass = DE_VEHICLECLASS_2;
        sNddsRouteReq.eOilType = DE_OILTYPE_GASOLINE;
        sNddsRouteReq.nPosCnt = 2;

        strcpy(sNddsRouteReq.sPosInfo[0].szName, "출발지");
        sNddsRouteReq.sPosInfo[0].tp.x = 66597632;
        sNddsRouteReq.sPosInfo[0].tp.y = 19655016;
        strcpy(sNddsRouteReq.sPosInfo[1].szName, "목적지");
        sNddsRouteReq.sPosInfo[1].tp.x = 66574165;
        sNddsRouteReq.sPosInfo[1].tp.y = 19695505;
        sNddsRouteReq.nAngle = 30;
        sNddsRouteReq.nSpeed = 80;

        QString ServerSearchPath = SKT::Environment::getRouteSearchPath() + "/RP/Route_Tvas_Server.dat";
        strcpy(sNddsRouteReq.szCollectDataPath[0], ServerSearchPath.toLocal8Bit().data());
        QString ServerSearchPath2 = SKT::Environment::getRouteSearchPath() + "/RP/Route_Tvas_Server2.dat";
        strcpy(sNddsRouteReq.szCollectDataPath[1], ServerSearchPath2.toLocal8Bit().data());
        QString ServerSearchPath3 = SKT::Environment::getRouteSearchPath() + "/RP/Route_Tvas_Server3.dat";
        strcpy(sNddsRouteReq.szCollectDataPath[2], ServerSearchPath3.toLocal8Bit().data());
        DeErrorCode error = NDDS_Route(&sNddsRouteReq, false);
        /*
        // 경로탐색 완료후에 호출해야함
        DS_WEATHERINFO_RES sWeatherinfo;
				NDDS_GetWeatherInfo(&sWeatherinfo);
				*/
				
				/*
				// VMS 가이드 정보 조회
				NDDS_VmsGuideInfo("110030", true);
				DS_VMSGUIDEINFO sVmsGuideInfo;
				NDDS_GetVmsGuideInfo(&sVmsGuideInfo);
				*/
    }
#endif
}

void CRouteSearch::stopRoute() {
#ifdef BUILD_TARGET_MNS
    MNS_StopRoute();
    NDDS_StopRoute();
#endif
}

void CRouteSearch::MySearchTest(int type) {
#ifdef BUILD_TARGET_MNS
//    const char* cSerchName = "coex";  //E_SEARCHTYPE_SISULNAME
//    const char* cSerchName = "0260000114"; //E_SEARCHTYPE_PHONENUMBER
//    const char cSerchName[] = { 0xbc, 0xba, 0xbb, 0xea, 0xb5, 0xbf, 0x31, 0x31, 0x34, 0x2d, 0x31, 0x36, 0 }; //E_SEARCHTYPE_POWERJIBUN //성산동114-16
    if(type == 4 || type == 5 || type == 6)
    {
        bool bRet;
        int i, nResult, nCnt, nTotal;
        bool bFilter = false;
        DS_SEARCH_REQ sNddsSearchReq;
        DeErrorCode eError;

        switch (type) {
        case 4:
            sNddsSearchReq.eSearchType = DE_SEARCHTYPE_SERVER;
            sNddsSearchReq.sSearchServer.eSortType = DE_SERVER_SORTTYPE_ACCURACY;
            sNddsSearchReq.sSearchServer.tpCurrent.x = 66573602;
            sNddsSearchReq.sSearchServer.tpCurrent.y = 19695617;
            strcpy(sNddsSearchReq.sSearchServer.szSearchName, "퇴계로307");
            strcpy(sNddsSearchReq.sSearchServer.szAddrName, "");
            sNddsSearchReq.sSearchServer.nDist = 0;
            sNddsSearchReq.sSearchServer.eOilType = DE_OILTYPE_GASOLINE;
            sNddsSearchReq.sSearchServer.bNewAddr = false;
            break;
        case 5:
            sNddsSearchReq.eSearchType = DE_SEARCHTYPE_SERVERADDR;
            strcpy(sNddsSearchReq.sSearchServerAddr.szAddrName1, "서울");
            strcpy(sNddsSearchReq.sSearchServerAddr.szAddrName2, "중구");
            sNddsSearchReq.sSearchServerAddr.bNewAddr = false;
            break;
        case 6:
#if 1
		       sNddsSearchReq.eSearchType = DE_SEARCHTYPE_SERVERAROUND;
		       sNddsSearchReq.sSearchServerAround.eAroundPoiType = DE_AROUNDTYPE_OPI;
		       sNddsSearchReq.sSearchServerAround.tpCurrent.x = 66573602;
		       sNddsSearchReq.sSearchServerAround.tpCurrent.y = 19695617;
		       strcpy(sNddsSearchReq.sSearchServerAround.szSearchName, "");
		       sNddsSearchReq.sSearchServerAround.nDist = 0;
		       sNddsSearchReq.sSearchServerAround.bNewAddr = false;
#else
		       sNddsSearchReq.eSearchType = DE_SEARCHTYPE_SERVERAROUND;
		       sNddsSearchReq.sSearchServerAround.eAroundPoiType = DE_AROUNDTYPE_NONE;
		       sNddsSearchReq.sSearchServerAround.tpCurrent.x = 66573602;
		       sNddsSearchReq.sSearchServerAround.tpCurrent.y = 19695617;
		       strcpy(sNddsSearchReq.sSearchServerAround.szSearchName, "주유소");
		       sNddsSearchReq.sSearchServerAround.nDist = 0;
		       sNddsSearchReq.sSearchServerAround.bNewAddr = false;
#endif
            break;            
        default:
            return;
            break;
        }

        QString search_result_file = SKT::Environment::getDataPath() + "/search_result/search_result.dat";
        QFile out(search_result_file);
//        std::ofstream outf;
//        outf.open("/opt/bosch/nddssearch_result.dat",std::ios_base::out | std::ios_base::app);
//        if (!outf) {
//            std::cerr << "file open error" << std::endl;
//            exit(1);
//        }
        if(!out.open(QIODevice::ReadWrite | QIODevice::Text)) {
            qDebug() << "[RouteSearch::MySearchTest] : file operation error!!!!";
            return;
        }
        QTextStream outf(&out);

        eError = NDDS_Search(&sNddsSearchReq, true, nCnt);
        outf << "\n\n eError : " << eError << " , nCnt : " << nCnt << endl;
        if(eError == DE_ERROR_SUCCESS && nCnt > 0) {
            DS_SEARCH_LIST list[8] = {0,};
            nCnt = 8;
            bool bNewAddr = false;
            eError = NDDS_GetSearchResultList(sNddsSearchReq.eSearchType, 0, nCnt, list);
            if(eError == DE_ERROR_SUCCESS) {
                for(int i = 0 ; i < nCnt ; i++) {
                    outf << list[i].nIdx << " : " << toUnicode(list[i].szName) << "(" << toUnicode(list[i].szAddrName) << " ," << list[i].szPhoneNumber << ")" << endl;
                }
            }
        }
        out.close();
    }
    else
    {
        const char cSerchName[] = {0xbf, 0xf9, 0xb5, 0xe5, 0xc4, 0xc5, 0xba, 0xcf, 0xb7, 0xce, 0x31, 0x31, 0x34, 0 }; //E_SEARCHTYPE_POWERNEWJIBUN//월드컵북로114

        bool bNewAddrChk = false;
        TeSearchType eSearchType;
        int nCnt = 10;
        int m_nSearchErrorCnt = 0;
        TS_SEARCH_REQ sSearchReq;
        MNS_ErrorCode eError;

        switch (type) {
        case 1:
            MNS_PowerSearchCheck(cSerchName, bNewAddrChk, eSearchType);
            break;
        case 2:
            eSearchType = TeSearchType::E_MNS_SEARCHTYPE_ADDRJIBUN;
            break;
        case 3:
            eSearchType = TeSearchType::E_MNS_SEARCHTYPE_NEWADDR;
            break;
        case 4:
            //eSearchType = TeSearchType::E_MNS_SEARCHTYPE_SERVER;
            break;
        case 5:
            //eSearchType = TeSearchType::E_MNS_SEARCHTYPE_SERVERADDR;
            break;
        case 6:
            eSearchType = TeSearchType::E_MNS_SEARCHTYPE_PREDICTION;
            break;
        default:
            break;
        }
        switch (eSearchType) {
        case E_MNS_SEARCHTYPE_NONE:
            break;
        case E_MNS_SEARCHTYPE_SISULNAME:
            sSearchReq.eSearchType = E_MNS_SEARCHTYPE_SISULNAME;
            sSearchReq.sSearchSisulName.eSortType = E_MNS_SORTTYPE_NAME;
            sSearchReq.sSearchSisulName.tpCurrent.x = 66573602;
            sSearchReq.sSearchSisulName.tpCurrent.y = 19695617;
            sSearchReq.sSearchSisulName.eSisualNameType = E_MNS_SISUALNAMETYPE_ALL;
            sSearchReq.sSearchSisulName.nCateCode = 0;
            sSearchReq.sSearchSisulName.nAddrCode[0] = 0;
            sSearchReq.sSearchSisulName.nAddrCode[1] = 0;
            sSearchReq.sSearchSisulName.nAddrCode[2] = 0;
            sSearchReq.sSearchSisulName.bNewAddr = false;
            strcpy(sSearchReq.sSearchSisulName.szSearchName, cSerchName);
            break;

        case E_MNS_SEARCHTYPE_PHONENUMBER:
            sSearchReq.eSearchType = E_MNS_SEARCHTYPE_PHONENUMBER;
            sSearchReq.sSearchPhoneNumber.eSortType = E_MNS_SORTTYPE_NAME;
            sSearchReq.sSearchPhoneNumber.tpCurrent.x = 66573602;
            sSearchReq.sSearchPhoneNumber.tpCurrent.y = 19695617;
            strcpy(sSearchReq.sSearchPhoneNumber.szPhoneNumber, cSerchName);
            break;


        case E_MNS_SEARCHTYPE_POWERJIBUN:
            sSearchReq.eSearchType = E_MNS_SEARCHTYPE_POWERJIBUN;
            sSearchReq.sSearchPowerJibun.eSortType = E_MNS_SORTTYPE_NAME;
            sSearchReq.sSearchPowerJibun.tpCurrent.x = 66573602;
            sSearchReq.sSearchPowerJibun.tpCurrent.y = 19695617;
            strcpy(sSearchReq.sSearchPowerJibun.szAddrName, cSerchName);
            break;

        case E_MNS_SEARCHTYPE_POWERNEWJIBUN:
            sSearchReq.eSearchType = E_MNS_SEARCHTYPE_POWERNEWJIBUN;
            sSearchReq.sSearchPowerNewJibun.eSortType = E_MNS_SORTTYPE_NAME;
            sSearchReq.sSearchPowerNewJibun.tpCurrent.x = 66573602;
            sSearchReq.sSearchPowerNewJibun.tpCurrent.y = 19695617;
            strcpy(sSearchReq.sSearchPowerNewJibun.szAddrName, cSerchName);

            break;

        case E_MNS_SEARCHTYPE_PREDICTION:
            sSearchReq.eSearchType = E_MNS_SEARCHTYPE_PREDICTION;
            sSearchReq.sSearchSisulName.eSortType = E_MNS_SORTTYPE_NAME;
            sSearchReq.sSearchSisulName.tpCurrent.x = 66573602;
            sSearchReq.sSearchSisulName.tpCurrent.y = 19695617;
            sSearchReq.sSearchSisulName.szSearchName[0] = 0xa4;
            sSearchReq.sSearchSisulName.szSearchName[1] = 0xb1;
            sSearchReq.sSearchSisulName.szSearchName[2] = 0xa4;
            sSearchReq.sSearchSisulName.szSearchName[3] = 0xa4;
            sSearchReq.sSearchSisulName.szSearchName[4] = 0xa4;
            sSearchReq.sSearchSisulName.szSearchName[5] = 0xb7;
            sSearchReq.sSearchSisulName.szSearchName[6] = 0xa4;
            sSearchReq.sSearchSisulName.szSearchName[7] = 0xa9;
            sSearchReq.sSearchSisulName.szSearchName[8] = 0;
            sSearchReq.sSearchSisulName.eSisualNameType = E_MNS_SISUALNAMETYPE_ALL;
            sSearchReq.sSearchSisulName.nCateCode = 0;
            sSearchReq.sSearchSisulName.nAddrCode[0] = 0;
            sSearchReq.sSearchSisulName.nAddrCode[1] = 0;
            sSearchReq.sSearchSisulName.nAddrCode[2] = 0;
            sSearchReq.sSearchSisulName.bNewAddr = false;
            break;
/*
        case E_MNS_SEARCHTYPE_SERVER:
            sSearchReq.eSearchType = E_MNS_SEARCHTYPE_SERVER;
            sSearchReq.sSearchServer.eSortType = E_SERVER_MNS_SORTTYPE_ACCURACY;
            sSearchReq.sSearchServer.tpCurrent.x = 66573602;
            sSearchReq.sSearchServer.tpCurrent.y = 19695617;
            strcpy(sSearchReq.sSearchServer.szAddrName, "");
            sSearchReq.sSearchServer.nDist = 0;
            sSearchReq.sSearchServer.eOilType = E_OILTYPE_GASOLINE;
            sSearchReq.sSearchServer.szSearchName[0] = 0xbc;
            sSearchReq.sSearchServer.szSearchName[1] = 0xba;
            sSearchReq.sSearchServer.szSearchName[2] = 0xbb;
            sSearchReq.sSearchServer.szSearchName[3] = 0xea;
            sSearchReq.sSearchServer.szSearchName[4] = 0xb5;
            sSearchReq.sSearchServer.szSearchName[5] = 0xbf;
            sSearchReq.sSearchServer.szSearchName[6] = 0x31;
            sSearchReq.sSearchServer.szSearchName[7] = 0x31;
            sSearchReq.sSearchServer.szSearchName[8] = 0x34;
            sSearchReq.sSearchServer.szSearchName[9] = 0;
            break;

        case E_MNS_SEARCHTYPE_SERVERADDR:
            sSearchReq.eSearchType = E_MNS_SEARCHTYPE_SERVERADDR;
            sSearchReq.sSearchServerAddr.szAddrName1[0] = 0xbc;
            sSearchReq.sSearchServerAddr.szAddrName1[1] = 0xad;
            sSearchReq.sSearchServerAddr.szAddrName1[2] = 0xbf;
            sSearchReq.sSearchServerAddr.szAddrName1[3] = 0xef;
            sSearchReq.sSearchServerAddr.szAddrName1[4] = 0x0;
            sSearchReq.sSearchServerAddr.szAddrName1[5] = 0x0;
            sSearchReq.sSearchServerAddr.szAddrName2[0] = 0xb0;
            sSearchReq.sSearchServerAddr.szAddrName2[1] = 0xad;
            sSearchReq.sSearchServerAddr.szAddrName2[2] = 0xb3;
            sSearchReq.sSearchServerAddr.szAddrName2[3] = 0xb2;
            sSearchReq.sSearchServerAddr.szAddrName2[4] = 0xb1;
            sSearchReq.sSearchServerAddr.szAddrName2[5] = 0xb8;
            sSearchReq.sSearchServerAddr.szAddrName2[6] = 0x0;
            sSearchReq.sSearchServerAddr.szAddrName2[7] = 0x0;
            sSearchReq.sSearchServerAddr.bNewAddr = 0;
            break;
*/
        case E_MNS_SEARCHTYPE_ADDRJIBUN:
            sSearchReq.eSearchType = E_MNS_SEARCHTYPE_ADDRJIBUN;
            sSearchReq.sSearchAddrJibun.eSortType = E_MNS_SORTTYPE_NAME;
            sSearchReq.sSearchAddrJibun.tpCurrent.x = 66573602;
            sSearchReq.sSearchAddrJibun.tpCurrent.y = 19695617;
            sSearchReq.sSearchAddrJibun.nAddrCode = 11620101;
            sSearchReq.sSearchAddrJibun.nAddrRiCode = 0;
            sSearchReq.sSearchAddrJibun.nJibunStIdx = 418;
            sSearchReq.sSearchAddrJibun.nJibunCnt = 200;
            sSearchReq.sSearchAddrJibun.cSanFlag = '1';
            strcpy(sSearchReq.sSearchAddrJibun.szJinun, "114");
            break;

        case E_MNS_SEARCHTYPE_NEWADDR:
            sSearchReq.eSearchType = E_MNS_SEARCHTYPE_NEWADDR;
            sSearchReq.sSearchNewAddr.eSortType = E_MNS_SORTTYPE_NAME;
            sSearchReq.sSearchNewAddr.tpCurrent.x = 66573602;
            sSearchReq.sSearchNewAddr.tpCurrent.y = 19695617;
            sSearchReq.sSearchNewAddr.nAddrCode = 114400534;
            sSearchReq.sSearchNewAddr.nNewAddrStIdx = 181426;
            sSearchReq.sSearchNewAddr.nNewAddrCnt = 191;
            strcpy(sSearchReq.sSearchNewAddr.szJinun, "114");
            sSearchReq.sSearchNewAddr.szAddrName[0] = 0xbf;
            sSearchReq.sSearchNewAddr.szAddrName[1] = 0xf9;
            sSearchReq.sSearchNewAddr.szAddrName[2] = 0xb5;
            sSearchReq.sSearchNewAddr.szAddrName[3] = 0xe5;
            sSearchReq.sSearchNewAddr.szAddrName[4] = 0xc4;
            sSearchReq.sSearchNewAddr.szAddrName[5] = 0xc5;
            sSearchReq.sSearchNewAddr.szAddrName[6] = 0xba;
            sSearchReq.sSearchNewAddr.szAddrName[7] = 0xcf;
            sSearchReq.sSearchNewAddr.szAddrName[8] = 0xb7;
            sSearchReq.sSearchNewAddr.szAddrName[9] = 0xce;
            sSearchReq.sSearchNewAddr.szAddrName[10] = 0;
            break;

        default:
            break;
        }

//        std::ofstream outf;
//        outf.open("/opt/bosch/search_result.dat",std::ios_base::out | std::ios_base::app);
//        if (!outf) {
//            std::cerr << "file open error" << std::endl;
//            exit(1);
//        }
        QString search_result_file = SKT::Environment::getDataPath() + "/search_result/search_result.dat";
        QFile out(search_result_file);
        if(!out.open(QIODevice::ReadWrite | QIODevice::Text)) {
            qDebug() << "[RouteSearch::MySearchTest #2] : file operation error!!!!";
            return;
        }
        QTextStream outf(&out);

        eError = MNS_Search(&sSearchReq, true, nCnt);
        outf << "\n\n eError : " << eError << " , nCnt : " << nCnt << endl;
        if(eError == E_ERROR_SUCCESS && nCnt > 0) {
            TS_MMP_DATA sMpp;
            TS_SEARCH_LIST list[8] = {0,};
            nCnt = 8;
            bool bNewAddr = false;
            eError = MNS_GetSearchResultList(sSearchReq.eSearchType,bNewAddr, 0, nCnt, list);
            if(eError == E_ERROR_SUCCESS) {
                for(int i = 0 ; i < nCnt ; i++) {
                    outf << list[i].nIdx << " : " << toUnicode(list[i].szName) << "(" << toUnicode(list[i].szAddrName) << " ," << list[i].szPhoneNumber << ")" << endl;
                }
            } else {
                m_nSearchErrorCnt++;
            }
        } else {
            if(eError != E_ERROR_SUCCESS)
                m_nSearchErrorCnt++;
        }
        out.close();
    }
#endif
}



void CRouteSearch::getNextAddrList() {
#ifdef BUILD_TARGET_MNS
    TS_POINT tp;
    tp.x = 66571920;
    tp.y = 19691179;
    TeAddressType eAddressType;
    eAddressType = TeAddressType::E_ADDRESS_OLD;
    int nAddressCode = 0;
    int nRiCode = 0;
    bool bFilter = false;
    int nIdx = 0;
    
    //    TS_RESULT_ADDR pResult;
    TS_RESULT_ADDR pResultDepth1[20] = {0,};

    int nTotal;
    bool bNext;
    MNS_ErrorCode eError;

int nCnt = 20;
    eError = MNS_GetNextAddrList(tp, eAddressType, nAddressCode, nRiCode, bFilter, nIdx, nCnt, pResultDepth1, &nTotal, &bNext); //전국(depth0)
    if (eError == E_ERROR_SUCCESS) {

//        std::ofstream outf;
//        outf.open("/opt/bosch/search_result.dat",std::ios_base::out | std::ios_base::app);
//        if (!outf) {
//            //outf << "file open error" << endl;
//            exit(1);
//        }
        QString search_result_file = SKT::Environment::getDataPath() + "/search_result/search_result.dat";
        QFile out(search_result_file);
        if(!out.open(QIODevice::ReadWrite | QIODevice::Text)) {
            qDebug() << "[RouteSearch::getNextAddrList] : file operation error!!!!";
            return;
        }
        QTextStream outf(&out);


        outf << "getNextAddr_E_ERROR_SUCCESS" << endl;
        outf << "nTotal: " << nTotal << endl;
        for (int i = 0; i < nCnt; i++) {
            //outf << "pResultDepth1.nAddrCode: " << pResultDepth1[i].nAddrCode << endl;
            outf << "pResultDepth1.nAddrCode: " << pResultDepth1[i].nAddrCode << "|" << pResultDepth1[i].nAddrRiCode << "|"
                << toUnicode(pResultDepth1[i].szAddrName) << endl;
        }

        nAddressCode = pResultDepth1[14].nAddrCode;
        TS_RESULT_ADDR pResultDepth2[50] = {0,};

nCnt = 50;
        eError = MNS_GetNextAddrList(tp, eAddressType, nAddressCode, nRiCode, bFilter, nIdx, nCnt, pResultDepth2, &nTotal, &bNext); //경북(depth1)

        if (eError == E_ERROR_SUCCESS) {
            outf << "getNextAddr_E_ERROR_SUCCESS" << endl;
            outf << "nTotal: " << nTotal << endl;
            for (int i = 0; i < nCnt; i++) {
                //outf << "pResultDepth2.nAddrCode: " << pResultDepth2[i].nAddrCode << endl;
                outf << "pResultDepth2.nAddrCode: " << pResultDepth2[i].nAddrCode << "|" << pResultDepth2[i].nAddrRiCode << "|"
                    << toUnicode(pResultDepth2[i].szAddrName) << endl;
            }

            nAddressCode = pResultDepth2[18].nAddrCode;
            TS_RESULT_ADDR pResultDepth3[50] = {0,};

nCnt = 50;
            eError = MNS_GetNextAddrList(tp, eAddressType, nAddressCode, nRiCode, bFilter, nIdx, nCnt, pResultDepth3, &nTotal, &bNext); //울진군(depth2)

            if (eError == E_ERROR_SUCCESS) {
                outf << "getNextAddr_E_ERROR_SUCCESS" << endl;
                outf << "nTotal: " << nTotal << endl;
                for (int i = 0; i < nCnt; i++) {
                    //outf << "pResultDepth3.nAddrCode: " << pResultDepth3[i].nAddrCode << endl;
                    outf << "pResultDepth3.nAddrCode: " << pResultDepth3[i].nAddrCode << "|" << pResultDepth3[i].nAddrRiCode << "|"
                        << toUnicode(pResultDepth3[i].szAddrName) << endl;
                }


                if(bNext){
                    nAddressCode = pResultDepth3[2].nAddrCode;
                    TS_RESULT_ADDR pResultDepth4[50] = {0,};

nCnt = 50;
            eError = MNS_GetNextAddrList(tp, eAddressType, nAddressCode, nRiCode, bFilter, nIdx, nCnt, pResultDepth4, &nTotal, &bNext); //금강송면(depth3)

                    if (eError == E_ERROR_SUCCESS) {
                        outf << "getNextAddr_E_ERROR_SUCCESS" << endl;
                        outf << "nTotal: " << nTotal << endl;
                        for (int i = 0; i < nCnt; i++) {
                            outf << "pResultDepth4.nAddrCode: " << pResultDepth4[i].nAddrCode << "|" << pResultDepth4[i].nAddrRiCode << "|"
                                << toUnicode(pResultDepth4[i].szAddrName) << endl;
                        }
                    }
                }
            }
        }
        out.close();
    } else {
        /*
            E_ERROR_SEARCH_MAXNAME = -399,			///< 검색어 크기가 37자를 초과 했습니다.
            E_ERROR_SEARCH_TYPE,					///< 검색 종류가 맞지 않습니다.
            E_ERROR_SEARCH_ADDRESSTYPE,				///< 주소 타입이 맞지 않습니다.
            E_ERROR_SEARCH_NOTMPP,					///< MPP 정보가 존재 하지 않습니다.
            E_ERROR_SEARCH_SERVER,					///< 검색 서버 통신 실패 했습니다.
            E_ERROR_SEARCH_DUPLICATION,				///< 검색 중복 요청.
        */
    }
#endif
}

void CRouteSearch::getNextCateList() {
#ifdef BUILD_TARGET_MNS
    int nCateCode = 0;
    bool bFilter = false;
    int nIdx = 0;
    int nCnt= 10;
    TS_RESULT_CATE pResultDepth1[10] = {0,};
    int nTotal;
    bool bNext;
    MNS_ErrorCode eError;

    eError = MNS_GetNextCateList(nCateCode, bFilter, nIdx, nCnt, pResultDepth1, &nTotal, &bNext); // 전체(depth1)
    if (eError == E_ERROR_SUCCESS) {
//        std::ofstream outf;
//        outf.open("/opt/bosch/search_result.dat",std::ios_base::out | std::ios_base::app);
//        if (!outf) {
//            //cerr << "file open error" << endl;
//            exit(1);
//        }
        QString search_result_file = SKT::Environment::getDataPath() + "/search_result/search_result.dat";
        QFile out(search_result_file);
        if(!out.open(QIODevice::ReadWrite | QIODevice::Text)) {
            qDebug() << "[RouteSearch::getNextCateList] : file operation error!!!!";
            return;
        }
        QTextStream outf(&out);

        outf << "getNextcategory_E_ERROR_SUCCESS" << endl;
        outf << "nTotal: " << nTotal << endl;
        for (int i = 0; i < nCnt; i++) {
            outf << "pResultDepth1.nCateCode: " << pResultDepth1[i].nCateCode << endl;
        }

        nCateCode = pResultDepth1[0].nCateCode;
        TS_RESULT_CATE pResultDepth2[10] = {0,};
        eError = MNS_GetNextCateList(nCateCode, bFilter, nIdx, nCnt, pResultDepth2, &nTotal, &bNext); //교통(depth2)
        if (eError == E_ERROR_SUCCESS) {
            outf << "getNextcategory_E_ERROR_SUCCESS" << endl;
            outf << "nTotal: " << nTotal << endl;
            for (int i = 0; i < nCnt; i++) {
                outf << "pResultDepth2.nCateCode: " << pResultDepth2[i].nCateCode << endl;
            }
        }
        out.close();
    } else {
        /*
            E_ERROR_SEARCH_MAXNAME = -399,			///< 검색어 크기가 37자를 초과 했습니다.
            E_ERROR_SEARCH_TYPE,					///< 검색 종류가 맞지 않습니다.
            E_ERROR_SEARCH_ADDRESSTYPE,				///< 주소 타입이 맞지 않습니다.
            E_ERROR_SEARCH_NOTMPP,					///< MPP 정보가 존재 하지 않습니다.
            E_ERROR_SEARCH_SERVER,					///< 검색 서버 통신 실패 했습니다.
            E_ERROR_SEARCH_DUPLICATION,				///< 검색 중복 요청.
        */
    }
#endif
}


void CRouteSearch::destroy()
{
    if (m_init) {
#ifdef BUILD_TARGET_MNS
        MNS_Uninitialize();
        NDDS_Uninitialize();
#else
#endif
        m_init = false;
    }
}

int CRouteSearch::callbackSearchStatus(int nMsg, long lParam1, long lParam2, void *lpParam)
{
    CRouteSearch* pRouteSearch = CRouteSearch::sharedInstance();

    switch(nMsg) {
    case E_MSGTYPE_ENGINE_INIT:
       emit pRouteSearch->RSinit(lParam1 == 0);
    break;
    case E_MSGTYPE_ROUTE_INIT:
        //emit pRouteSearch->SearchRPEnd(lParam1);
    break;
    case E_MSGTYPE_ROUTE_START:
        //emit pRouteSearch->SearchRPEnd(lParam1);
    break;
    case E_MSGTYPE_ROUTE_END:
        //emit pRouteSearch->SearchRPEnd(lParam2);
    break;
    case E_MSGTYPE_ROUTE_DATA:
//        uint dataSize = (uint)lParam2;
//        uchar data = (uchar)lpParam;
        break;
    default:
        break;
    }

    return 1;
}

int CRouteSearch::callbackNddsStatus(int nMsg, long lParam1, long lParam2, void *lpParam)
{
    CRouteSearch* pRouteSearch = CRouteSearch::sharedInstance();

    switch(nMsg) {
    case DE_MSGTYPE_NDDS_INIT:
       emit pRouteSearch->RSinit(lParam1 == 0);
    break;
    case DE_MSGTYPE_ROUTE_INIT:
        //emit pRouteSearch->SearchRPEnd(lParam1);
    break;
    case DE_MSGTYPE_ROUTE_START:
        //emit pRouteSearch->SearchRPEnd(lParam1);
    break;
    case DE_MSGTYPE_ROUTE_END:
        //emit pRouteSearch->SearchRPEnd(lParam2);
    break;
    case DE_MSGTYPE_ROUTE_DATA:
//        uint dataSize = (uint)lParam2;
//        uchar data = (uchar)lpParam;
        break;
    default:
        break;
    }

    return 1;
}
}
