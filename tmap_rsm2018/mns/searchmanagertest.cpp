#include "searchmanagertest.h"

#if  DEBUG_GETNEXTLIST
    #include "environment.h"
    #include <QFile>
    #include "textdecoder.h"
    #include "MNS_Interface.h"
    #include "NDDS_Interface.h"
    #include <QDebug>
#endif

namespace SKT {

SearchManagerTest* SearchManagerTest::s_instance = NULL;


SearchManagerTest::SearchManagerTest()
{

}

SearchManagerTest::~SearchManagerTest()
{

}
#if  DEBUG_GETNEXTLIST
void printAddressInfo(QString tag, TeAddressType eAddressType, QTextStream *out, TS_RESULT_ADDR *pResultDepth, int nCnt, int nTotal, bool bNext) {
    if(nCnt > 1) {
        *out << tag << "|";
        *out << "nCnt: " << nCnt << "\t";
        *out << "nTotal: " << nTotal << "\t";
        *out << "bNext: " << bNext <<endl;
    }
    for (int i = 0; i < nCnt; i++) {
        if(eAddressType != E_ADDRESS_NEW) {
            *out << tag
                << "|nAddrCode: " << pResultDepth[i].nAddrCode
                << "|JibunStIdx:" << pResultDepth[i].nJibunStIdx
                << "|JibunCnt:" << pResultDepth[i].nJibunCnt
                << "|Ri:" << pResultDepth[i].nAddrRiCode
                << "\t\t|" << TextDecoder::instance()->fromCP949(pResultDepth[i].szAddrName)
                << endl;
        } else {
            *out << tag
                << "|nAddrCode: " << pResultDepth[i].nAddrCode
                << "|NewAddrStIdx:" << pResultDepth[i].nNewAddrStIdx
                << "|NewAddrCnt:" << pResultDepth[i].nNewAddrCnt
                << "\t\t|" << TextDecoder::instance()->fromCP949(pResultDepth[i].szAddrName)
                << endl;
        }
    }
}
#endif
void SearchManagerTest::requestSearch_getNextAddressList(bool bNewAddr)
{
#if DEBUG_GETNEXTLIST
    TS_POINT tp;
    tp.x = 0;
    tp.y = 0;
    int nRiCode = 0;
    bool bFilter = false;
    int nIdx = 0;
    int nMaxCnt = 0;
    int nCnt = 0;
    int nTotal = 0;
    int nAddressCode = 0;
    bool bNext;
    TeAddressType eAddressType;
    eAddressType = bNewAddr?E_ADDRESS_NEW:E_ADDRESS_OLD;
    MNS_ErrorCode eError;

    //    TS_RESULT_ADDR pResult;
    nMaxCnt = nCnt = 30;
    TS_RESULT_ADDR pResultDepth1[30] = {0,};

    nAddressCode = 0;
    eError = MNS_GetNextAddrList(tp, eAddressType, nAddressCode, nRiCode, bFilter, nIdx, nCnt, pResultDepth1, &nTotal, &bNext);
    if ((eError != E_ERROR_SUCCESS)||(nTotal <= 0) ){
        return;
    }
    /*
     * pResultDepth0.nAddrCode: 0
        11000000|0|서울
        26000000|0|부산
        27000000|0|대구
        28000000|0|인천
        29000000|0|광주
        30000000|0|대전
        31000000|0|울산
        36000000|0|세종
        41000000|0|경기
        42000000|0|강원
        43000000|0|충북
        44000000|0|충남
        45000000|0|전북
        46000000|0|전남
        47000000|0|경북
        48000000|0|경남
        50000000|0|제주
    */
    eError = MNS_GetNextAddrList(tp, eAddressType, nAddressCode, nRiCode, bFilter, nIdx, nCnt, pResultDepth1, &nTotal, &bNext); //전국(depth1)
    if (eError == E_ERROR_SUCCESS && bNext && nTotal>0) {
        QString search_result_file = SKT::Environment::getDataPath() + "/search_result/search_result.dat";
        QFile::remove(search_result_file);
        QFile outf(search_result_file);

        if(!outf.open(QIODevice::ReadWrite | QIODevice::Text)) {
            qDebug() << "[RPnSearch::requestSearch_getNextAddressList] : file operation error!!!!";
            return;
        }
        QTextStream out(&outf);

        out << "getNextAddr_E_ERROR_SUCCESS" << "\n";
        printAddressInfo("pResultDepth1", eAddressType, &out, pResultDepth1, nCnt, nTotal, bNext);
        /*
         * pResultDepth1.nAddrCode: 00000000
            11000000|JibunStdIdx: 0|JibunCnt: 0|서울<< 0
            26000000|JibunStdIdx: 0|JibunCnt: 0|부산
            27000000|JibunStdIdx: 0|JibunCnt: 0|대구
            28000000|JibunStdIdx: 0|JibunCnt: 0|인천<< 3
            29000000|JibunStdIdx: 0|JibunCnt: 0|광주
            30000000|JibunStdIdx: 0|JibunCnt: 0|대전
            31000000|JibunStdIdx: 0|JibunCnt: 0|울산
            36000000|JibunStdIdx: 0|JibunCnt: 0|세종
            41000000|JibunStdIdx: 0|JibunCnt: 0|경기
            42000000|JibunStdIdx: 0|JibunCnt: 0|강원
            43000000|JibunStdIdx: 0|JibunCnt: 0|충북
            44000000|JibunStdIdx: 0|JibunCnt: 0|충남
            45000000|JibunStdIdx: 0|JibunCnt: 0|전북<< 12
            46000000|JibunStdIdx: 0|JibunCnt: 0|전남
            47000000|JibunStdIdx: 0|JibunCnt: 0|경북<< 14
            48000000|JibunStdIdx: 0|JibunCnt: 0|경남
            50000000|JibunStdIdx: 0|JibunCnt: 0|제주
        */
        //전북 nAddressCode = pResultDepth1[12].nAddrCode;
        //인천 nAddressCode = pResultDepth1[3].nAddrCode;
        nAddressCode = pResultDepth1[14].nAddrCode;
        TS_RESULT_ADDR pResultDepth2[30] = {0,};

        nCnt = nMaxCnt;
        eError = MNS_GetNextAddrList(tp, eAddressType, nAddressCode, nRiCode, bFilter, nIdx, nCnt, pResultDepth2, &nTotal, &bNext); //(depth2)

        if (eError == E_ERROR_SUCCESS && bNext && nTotal>0) {
            out << "getNextAddr_E_ERROR_SUCCESS" << endl;
            printAddressInfo("pResultDepth2", eAddressType, &out, pResultDepth2, nCnt, nTotal, bNext);
            /*
             * pResultDepth2.nAddrCode: 11000000
                11000000|0|<서울>
                11680000|0|강남구
                11740000|0|강동구
                11305000|0|강북구
                11500000|0|강서구
                11620000|0|관악구<< 5
                11215000|0|광진구
                11530000|0|구로구
                11545000|0|금천구
                11350000|0|노원구
                11320000|0|도봉구
                11230000|0|동대문구
                11590000|0|동작구
                11440000|0|마포구
                11410000|0|서대문구
                11650000|0|서초구
                11200000|0|성동구
                11290000|0|성북구
                11710000|0|송파구
                11470000|0|양천구
            */
            /*
             * pResultDepth2.nAddrCode: 45000000
                450000000|0|<전북>
                457900000|0|고창군
                451300000|0|군산시
                452100000|0|김제시
                451900000|0|남원시
                457300000|0|무주군
                458000000|0|부안군
                457700000|0|순창군
                457100000|0|완주군
                451400000|0|익산시
                457500000|0|임실군
                457400000|0|장수군 << 11
                451130000|0|전주시 덕진구
                451110000|0|전주시 완산구
                451800000|0|정읍시
                457200000|0|진안군
            */
            /*
             * pResultDepth2.nAddrCode: 28000000
                28000000|0|<인천>
                28710000|0|강화군
                28245000|0|계양구 <<2
                28170000|0|남구
                28200000|0|남동구
                28140000|0|동구
                28237000|0|부평구
                28260000|0|서구
                28185000|0|연수구
                28720000|0|옹진군
                28110000|0|중구
            */
            /*
                47000000|JibunStIdx:0|JibunCnt:0|Ri:0		|<경북>
                47290000|JibunStIdx:0|JibunCnt:0|Ri:0		|경산시
                47130000|JibunStIdx:0|JibunCnt:0|Ri:0		|경주시
                47830000|JibunStIdx:0|JibunCnt:0|Ri:0		|고령군
                47190000|JibunStIdx:0|JibunCnt:0|Ri:0		|구미시
                47720000|JibunStIdx:0|JibunCnt:0|Ri:0		|군위군
                47150000|JibunStIdx:0|JibunCnt:0|Ri:0		|김천시 <<6
                47280000|JibunStIdx:0|JibunCnt:0|Ri:0		|문경시
                47920000|JibunStIdx:0|JibunCnt:0|Ri:0		|봉화군
                47250000|JibunStIdx:0|JibunCnt:0|Ri:0		|상주시
                47840000|JibunStIdx:0|JibunCnt:0|Ri:0		|성주군
                47170000|JibunStIdx:0|JibunCnt:0|Ri:0		|안동시
                47770000|JibunStIdx:0|JibunCnt:0|Ri:0		|영덕군
                47760000|JibunStIdx:0|JibunCnt:0|Ri:0		|영양군
                47210000|JibunStIdx:0|JibunCnt:0|Ri:0		|영주시
                47230000|JibunStIdx:0|JibunCnt:0|Ri:0		|영천시
                47900000|JibunStIdx:0|JibunCnt:0|Ri:0		|예천군
                47940000|JibunStIdx:0|JibunCnt:0|Ri:0		|울릉군
                47930000|JibunStIdx:0|JibunCnt:0|Ri:0		|울진군
                47730000|JibunStIdx:0|JibunCnt:0|Ri:0		|의성군
                47820000|JibunStIdx:0|JibunCnt:0|Ri:0		|청도군
                47750000|JibunStIdx:0|JibunCnt:0|Ri:0		|청송군
                47850000|JibunStIdx:0|JibunCnt:0|Ri:0		|칠곡군
                47111000|JibunStIdx:0|JibunCnt:0|Ri:0		|포항시 남구
                47113000|JibunStIdx:0|JibunCnt:0|Ri:0		|포항시 북구
            */
            //장수군 nAddressCode = pResultDepth2[11].nAddrCode;
            //계양구 nAddressCode = pResultDepth2[2].nAddrCode;
            //김천시 nAddressCode = pResultDepth2[6].nAddrCode;
            nAddressCode = pResultDepth2[6].nAddrCode;
            TS_RESULT_ADDR pResultDepth3[30] = {0,};

            nCnt = nMaxCnt;
            eError = MNS_GetNextAddrList(tp, eAddressType, nAddressCode, nRiCode, bFilter, nIdx, nCnt, pResultDepth3, &nTotal, &bNext); //(depth3)

            if (eError == E_ERROR_SUCCESS) {
                out << "getNextAddr_E_ERROR_SUCCESS" << endl;
                printAddressInfo("pResultDepth3", eAddressType, &out, pResultDepth3, nCnt, nTotal, bNext);
                /*
                 * pResultDepth3.nAddrCode:11620000
                    11620000|0|<관악구>
                    11620103|0|남현동
                    11620101|0|봉천동 <<2
                    11620102|0|신림동
                    .....
                    45740000|0|<장수군>
                    45740350|0|계남면
                    45740360|0|계북면
                    45740320|0|번암면<< 3
                    45740310|0|산서면
                    45740335|0|장계면
                    45740250|0|장수읍
                    45740340|0|천천면
                    ......
                    28245000|0|<계양구>
                    28245119|0|갈현동
                    28245102|0|계산동
                    28245111|0|귤현동
                    28245115|0|노오지동
                    28245122|0|다남동
                    28245110|0|동양동
                    28245120|0|둑실동
                    28245121|0|목상동
                    28245109|0|박촌동
                    28245108|0|방축동
                    28245107|0|병방동
                    28245112|0|상야동
                    28245104|0|서운동
                    28245116|0|선주지동
                    28245118|0|오류동
                    28245106|0|용종동 << 16
                    28245117|0|이화동
                    28245105|0|임학동
                    28245103|0|작전동
                */
                /*
                    116200001|NewAddrStdIdx: 258974|NewAddrCnt: 23|nCnt:30|nTotal:842|bNext:1|과천대로
                    116200002|NewAddrStdIdx: 258997|NewAddrCnt: 141|nCnt:30|nTotal:842|bNext:1|관악로
                    116200003|NewAddrStdIdx: 259138|NewAddrCnt: 39|nCnt:30|nTotal:842|bNext:1|관악로10길
                    116200004|NewAddrStdIdx: 259177|NewAddrCnt: 19|nCnt:30|nTotal:842|bNext:1|관악로11가길
                    ....
                */
                if(E_ADDRESS_OLD == eAddressType ) {
                    //번암면 nAddressCode = pResultDepth3[3].nAddrCode;
                    //용종동 nAddressCode = pResultDepth3[16].nAddrCode;
                    nAddressCode = pResultDepth3[2].nAddrCode;
                    TS_RESULT_ADDR pResultDepth4[30] = {0,};

                    nCnt = nMaxCnt;
                    eError = MNS_GetNextAddrList(tp, eAddressType, nAddressCode, nRiCode, bFilter, nIdx, nCnt, pResultDepth4, &nTotal, &bNext); //(depth4)

                    if (eError == E_ERROR_SUCCESS ) {
                        out << "getNextAddr_E_ERROR_SUCCESS" << endl;
                        printAddressInfo("pResultDepth4", eAddressType, &out, pResultDepth4, nCnt, nTotal, bNext);

                        if(bNext) {
                            int subTotal = nTotal;
                            out << "getNextAddr_with RiCode" << endl;
                            for (int k = 1; k<nTotal; k++){
                                nAddressCode = pResultDepth4[k].nAddrCode;
                                nRiCode = pResultDepth4[k].nAddrRiCode;
                                TS_RESULT_ADDR pResultDepth5[30] = {0,};

                                nCnt = nMaxCnt;
                                eError = MNS_GetNextAddrList(tp, eAddressType, nAddressCode, nRiCode, bFilter, nIdx, nCnt, pResultDepth5, &subTotal, &bNext); //(depth5)

                                if (eError == E_ERROR_SUCCESS) {
                                    printAddressInfo("pResultDepth5-1", eAddressType, &out, pResultDepth5, nCnt, nTotal, bNext);
                                }

                            }
                        }
                    }
                }
            }
        }
        outf.close();
    }
#endif
     return;
}

#if DEBUG_GETNEXTLIST
void subOldAddressInfo(TS_POINT tp, int nTotalCnt, TS_RESULT_ADDR *pMother)
{
    TS_RESULT_ADDR pResultTemp[2] = {0,};
    int last_nTotal = 30;
    bool last_bNext = false;
    for( int j = 1 ; j < nTotalCnt; j++) {
        int temp_Cnt = 2;
        MNS_GetNextAddrList(tp, E_ADDRESS_OLD, pMother[j].nAddrCode, pMother[j].nAddrRiCode, false, 0, temp_Cnt, pResultTemp, &last_nTotal, &last_bNext);
        pMother[j].nJibunCnt = pResultTemp[0].nJibunCnt;
        pMother[j].nJibunStIdx = pResultTemp[0].nJibunStIdx;
        pMother[j].nAddrRiCode = pResultTemp[0].nAddrRiCode;
    }
}
#endif

void SearchManagerTest::requestSearch_getNextAddressList_3depth(bool bNewAddr)
{
#if DEBUG_GETNEXTLIST
    TS_POINT tp;
    tp.x = 0;
    tp.y = 0;
    int nRiCode = 0;
    bool bFilter = false;
    int nIdx = 0;
    int nMaxCnt = 0;
    int nCnt = 0;
    int nTotal = 0;
    int nAddressCode = 0;
    bool bNext;
    TeAddressType eAddressType;
    eAddressType = bNewAddr?E_ADDRESS_NEW:E_ADDRESS_OLD;
    MNS_ErrorCode eError;

    //    TS_RESULT_ADDR pResult;
    nMaxCnt = nCnt = 30;
    TS_RESULT_ADDR pResultDepth1[30] = {0,};

    nAddressCode = 0;
    eError = MNS_GetNextAddrList(tp, eAddressType, nAddressCode, nRiCode, bFilter, nIdx, nCnt, pResultDepth1, &nTotal, &bNext);
    if ((eError != E_ERROR_SUCCESS)||(nTotal <= 0) ){
        return;
    }
    /*
     * pResultDepth0.nAddrCode: 0
        11000000|0|서울
        26000000|0|부산
        27000000|0|대구
        28000000|0|인천
        29000000|0|광주
        30000000|0|대전
        31000000|0|울산
        36000000|0|세종
        41000000|0|경기
        42000000|0|강원
        43000000|0|충북
        44000000|0|충남
        45000000|0|전북
        46000000|0|전남
        47000000|0|경북
        48000000|0|경남
        50000000|0|제주
    */
    eError = MNS_GetNextAddrList(tp, eAddressType, nAddressCode, nRiCode, bFilter, nIdx, nCnt, pResultDepth1, &nTotal, &bNext); //전국(depth1)
    if (eError == E_ERROR_SUCCESS && bNext && nTotal>0) {
        QString search_result_file = SKT::Environment::getDataPath() + "/search_result/search_result.dat";
        QFile::remove(search_result_file);
        QFile outf(search_result_file);

        if(!outf.open(QIODevice::ReadWrite | QIODevice::Text)) {
            qDebug() << "[RPnSearch::requestSearch_getNextAddressList] : file operation error!!!!";
            return;
        }
        QTextStream out(&outf);

        out << "getNextAddr_E_ERROR_SUCCESS" << "\n";
        printAddressInfo("pResultDepth1", eAddressType, &out, pResultDepth1, nCnt, nTotal, bNext);
        /*
         * pResultDepth1.nAddrCode: 00000000
            11000000|JibunStdIdx: 0|JibunCnt: 0|서울<< 0
            26000000|JibunStdIdx: 0|JibunCnt: 0|부산
            27000000|JibunStdIdx: 0|JibunCnt: 0|대구
            28000000|JibunStdIdx: 0|JibunCnt: 0|인천<< 3
            29000000|JibunStdIdx: 0|JibunCnt: 0|광주
            30000000|JibunStdIdx: 0|JibunCnt: 0|대전
            31000000|JibunStdIdx: 0|JibunCnt: 0|울산
            36000000|JibunStdIdx: 0|JibunCnt: 0|세종
            41000000|JibunStdIdx: 0|JibunCnt: 0|경기
            42000000|JibunStdIdx: 0|JibunCnt: 0|강원
            43000000|JibunStdIdx: 0|JibunCnt: 0|충북
            44000000|JibunStdIdx: 0|JibunCnt: 0|충남
            45000000|JibunStdIdx: 0|JibunCnt: 0|전북<< 12
            46000000|JibunStdIdx: 0|JibunCnt: 0|전남
            47000000|JibunStdIdx: 0|JibunCnt: 0|경북
            48000000|JibunStdIdx: 0|JibunCnt: 0|경남
            50000000|JibunStdIdx: 0|JibunCnt: 0|제주
        */
        nAddressCode = pResultDepth1[12].nAddrCode;
        TS_RESULT_ADDR pResultDepth2[30] = {0,};

        nCnt = nMaxCnt;
        eError = MNS_GetNextAddrList(tp, eAddressType, nAddressCode, nRiCode, bFilter, nIdx, nCnt, pResultDepth2, &nTotal, &bNext); //(depth2)

        if (eError == E_ERROR_SUCCESS && bNext && nTotal>0) {
            out << "getNextAddr_E_ERROR_SUCCESS" << endl;
            printAddressInfo("pResultDepth2", eAddressType, &out, pResultDepth2, nCnt, nTotal, bNext);
            /*
             * pResultDepth2.nAddrCode: 11000000
                11000000|0|<서울>
                11680000|0|강남구
                11740000|0|강동구
                11305000|0|강북구
                11500000|0|강서구
                11620000|0|관악구<< 5
                11215000|0|광진구
                11530000|0|구로구
                11545000|0|금천구
                11350000|0|노원구
                11320000|0|도봉구
                11230000|0|동대문구
                11590000|0|동작구
                11440000|0|마포구
                11410000|0|서대문구
                11650000|0|서초구
                11200000|0|성동구
                11290000|0|성북구
                11710000|0|송파구
                11470000|0|양천구
            */
            /*
             * pResultDepth2.nAddrCode: 45000000
                450000000|0|<전북>
                457900000|0|고창군
                451300000|0|군산시
                452100000|0|김제시
                451900000|0|남원시
                457300000|0|무주군
                458000000|0|부안군
                457700000|0|순창군
                457100000|0|완주군
                451400000|0|익산시
                457500000|0|임실군
                457400000|0|장수군 << 11
                451130000|0|전주시 덕진구
                451110000|0|전주시 완산구
                451800000|0|정읍시
                457200000|0|진안군
            */
            /*
             * pResultDepth2.nAddrCode: 28000000
                28000000|0|<인천>
                28710000|0|강화군
                28245000|0|계양구 <<2
                28170000|0|남구
                28200000|0|남동구
                28140000|0|동구
                28237000|0|부평구
                28260000|0|서구
                28185000|0|연수구
                28720000|0|옹진군
                28110000|0|중구
            */
            nAddressCode = pResultDepth2[11].nAddrCode;
            TS_RESULT_ADDR pResultDepth3[30] = {0,};

            nCnt = nMaxCnt;
            eError = MNS_GetNextAddrList(tp, eAddressType, nAddressCode, nRiCode, bFilter, nIdx, nCnt, pResultDepth3, &nTotal, &bNext); //(depth3)

            if (eError == E_ERROR_SUCCESS) {
                out << "getNextAddr_E_ERROR_SUCCESS" << endl;
                printAddressInfo("pResultDepth3", eAddressType, &out, pResultDepth3, nCnt, nTotal, bNext);
                /*
                 * pResultDepth3.nAddrCode:11620000
                    11620000|0|<관악구>
                    11620103|0|남현동
                    11620101|0|봉천동 <<2
                    11620102|0|신림동
                    .....
                    45740000|0|<장수군>
                    45740350|0|계남면
                    45740360|0|계북면
                    45740320|0|번암면<< 3
                    45740310|0|산서면
                    45740335|0|장계면
                    45740250|0|장수읍
                    45740340|0|천천면
                    ......
                    28245000|0|<계양구>
                    28245119|0|갈현동
                    28245102|0|계산동
                    28245111|0|귤현동
                    28245115|0|노오지동
                    28245122|0|다남동
                    28245110|0|동양동
                    28245120|0|둑실동
                    28245121|0|목상동
                    28245109|0|박촌동
                    28245108|0|방축동
                    28245107|0|병방동
                    28245112|0|상야동
                    28245104|0|서운동
                    28245116|0|선주지동
                    28245118|0|오류동
                    28245106|0|용종동 <<
                    28245117|0|이화동
                    28245105|0|임학동
                    28245103|0|작전동
                */
                /*
                    116200001|NewAddrStdIdx: 258974|NewAddrCnt: 23|nCnt:30|nTotal:842|bNext:1|과천대로
                    116200002|NewAddrStdIdx: 258997|NewAddrCnt: 141|nCnt:30|nTotal:842|bNext:1|관악로
                    116200003|NewAddrStdIdx: 259138|NewAddrCnt: 39|nCnt:30|nTotal:842|bNext:1|관악로10길
                    116200004|NewAddrStdIdx: 259177|NewAddrCnt: 19|nCnt:30|nTotal:842|bNext:1|관악로11가길
                    ....
                */
                if(E_ADDRESS_OLD == eAddressType ) {
                    int nlast_totalCnt_of_pResultDepth3 = nTotal;
                    nAddressCode = pResultDepth3[3].nAddrCode;
                    nRiCode = pResultDepth3[3].nAddrRiCode;

                    int last_resultCount = 30;
                    TS_RESULT_ADDR pResultDepth4[30] = {0,};
                    int last_nTotal = 0;
                    bool last_bNext = false;

                    eError = MNS_GetNextAddrList(tp, eAddressType, nAddressCode, nRiCode, false, 0, last_resultCount, pResultDepth4, &last_nTotal, &last_bNext);

                    if ((eError == E_ERROR_SUCCESS) && last_nTotal < 0 && last_bNext == false && last_resultCount == 1 ) {
                        //get sub address info
                        subOldAddressInfo(tp, nlast_totalCnt_of_pResultDepth3, pResultDepth3);
                        out << "getNextAddr_E_ERROR_SUCCESS" << endl;
                        printAddressInfo("pResultDepth3-1", eAddressType, &out, pResultDepth3, nCnt, nTotal, bNext);
                    } else if ((eError == E_ERROR_SUCCESS) && last_bNext && last_nTotal>0 ) {
                        int nlast_totalCnt_of_pResultDepth4 = last_nTotal;
                        nAddressCode = pResultDepth4[1].nAddrCode;
                        nRiCode = pResultDepth4[1].nAddrRiCode;
                        out << "getNextAddr_E_ERROR_SUCCESS" << endl;
                        printAddressInfo("pResultDepth4", eAddressType, &out, pResultDepth4, last_resultCount, last_nTotal, last_bNext);
                        nCnt = last_resultCount;
                        nTotal = last_nTotal;
                        bNext = last_bNext;
                        eError = MNS_GetNextAddrList(tp, eAddressType, nAddressCode, nRiCode, false, 0, last_resultCount, pResultDepth4, &last_nTotal, &last_bNext);

                         if ((eError == E_ERROR_SUCCESS) && last_nTotal < 0 && last_bNext == false && last_resultCount == 1 ) {
                             //get sub address info
                             subOldAddressInfo(tp, nlast_totalCnt_of_pResultDepth4, pResultDepth4);
                             out << "getNextAddr_E_ERROR_SUCCESS" << endl;
                             printAddressInfo("pResultDepth4-1", eAddressType, &out, pResultDepth4, nCnt, nTotal, bNext);
                         }
                    } else {

                    }
                }
            }
        }
        outf.close();
    }
#endif
     return;
}

#if  DEBUG_GETNEXTLIST
void printCategoryInfo(QString tag, QTextStream *out, TS_RESULT_CATE *pResultDepth, int nCnt, int nTotal, bool bNext) {
    for (int i = 0; i < nCnt; i++) {
        *out << tag
             << "|nCateCode: " << pResultDepth[i].nCateCode
             << "|nItemCnt: " << pResultDepth[i].nItemCnt
             << "|szCateName: " << TextDecoder::instance()->fromCP949(pResultDepth[i].szCateName)
             << endl;
    }
}
#endif

void SearchManagerTest::requestSearch_getNextCateList() {
#if DEBUG_GETNEXTLIST
    int nCateCode = 0;
    bool bFilter = false;
    int nIdx = 0;
    int nCnt = 10;
    TS_RESULT_CATE pResultDepth1[10] = {0,};
    int nTotal;
    bool bNext;
    MNS_ErrorCode eError;

    eError = MNS_GetNextCateList(nCateCode, bFilter, nIdx, nCnt, pResultDepth1, &nTotal, &bNext); // 전체(depth1)
    if (eError == E_ERROR_SUCCESS) {
        QString search_result_file = SKT::Environment::getDataPath() + "/search_result/category_result.dat";
        QFile::remove(search_result_file);
        QFile out(search_result_file);
        if(!out.open(QIODevice::ReadWrite | QIODevice::Text)) {
            qDebug() << "[RouteSearch::getNextCateList] : file operation error!!!!";
            return;
        }
        QTextStream outf(&out);

        outf << "getNextcategory_E_ERROR_SUCCESS" << endl;
        outf << "nTotal: " << nTotal << endl;
        printCategoryInfo("pResultDepth1", &outf, pResultDepth1, nCnt, nTotal, bNext);

        nCateCode = pResultDepth1[0].nCateCode;
        TS_RESULT_CATE pResultDepth2[10] = {0,};
        eError = MNS_GetNextCateList(nCateCode, bFilter, nIdx, nCnt, pResultDepth2, &nTotal, &bNext); //교통(depth2)
        if (eError == E_ERROR_SUCCESS) {
            outf << "getNextcategory_E_ERROR_SUCCESS" << endl;
            outf << "nTotal: " << nTotal << endl;
            printCategoryInfo("pResultDepth2", &outf, pResultDepth2, nCnt, nTotal, bNext);

            nCateCode = pResultDepth2[1].nCateCode;
            TS_RESULT_CATE pResultDepth3[10] = {0,};
            eError = MNS_GetNextCateList(nCateCode, bFilter, nIdx, nCnt, pResultDepth3, &nTotal, &bNext); //교통시설(depth3)
            if (eError == E_ERROR_SUCCESS) {
                outf << "getNextcategory_E_ERROR_SUCCESS" << endl;
                outf << "nTotal: " << nTotal << endl;
                printCategoryInfo("pResultDepth3", &outf, pResultDepth3, nCnt, nTotal, bNext);


                nCateCode = pResultDepth3[3].nCateCode;
                TS_RESULT_CATE pResultDepth4[10] = {0,};
                eError = MNS_GetNextCateList(nCateCode, bFilter, nIdx, nCnt, pResultDepth4, &nTotal, &bNext); //교통시설(depth3)
                if (eError == E_ERROR_SUCCESS) {
                    outf << "getNextcategory_E_ERROR_SUCCESS" << endl;
                    outf << "nTotal: " << nTotal << endl;
                    printCategoryInfo("pResultDepth4", &outf, pResultDepth4, nCnt, nTotal, bNext);
                }
            }
        }


        out.close();
    }
#endif
}


}   //namespace SKT
