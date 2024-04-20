// NddsBase.cpp

#include "NddsLibBase.h"
#include "NddsLibConst.h"
#include "NddsLibCommonData.h"
#include "ndds_openssl.h"
#include "ndds_unicode.h"
#include <stdio.h>
#include <string.h>

#define NDDS_SOCKET_ERROR -1

// 통신중 사용자 취소 인지 체크 한다.
#define CHECK_NDDS_USER_CANCEL(a)                                       \
{                                                                       \
    if (m_pSessionData->eStatus == NDDSSTATUS_REQUESTCANCEL)            \
    {                                                                   \
        m_eLastErrCode = NDDSERR_USERCANCEL;                            \
        NddsExternalLog(E_DEBUG, "%sNDDS FAIL USERCANCLE :: code = %d", (a == 1 ? "" : "non"), m_eLastErrCode);    \
        goto NDDS_FAIL;                                                 \
    }                                                                   \
    if (m_pSessionData->eStatus == NDDSSTATUS_REQUESTTIMEOUT)           \
    {                                                                   \
        m_eLastErrCode = NDDSERR_TIMEOUT;                               \
        NddsExternalLog(E_DEBUG, "%sNDDS FAIL TIMEOUT :: code = %d", (a == 1 ? "" : "non"), m_eLastErrCode);       \
        goto NDDS_FAIL;                                                 \
    }                                                                   \
}

// 통신중 실패
#define FAIL_NDDS_PROTOCOL(errorCode, b)                                            \
{                                                                                   \
    if ((abs(errorCode) | abs(NDDSERR_HTTPPROTOCOL)) == abs(errorCode)) {           \
        m_eLastErrCode = (eNddsErrorCode)(0-(abs(errorCode) & 0xffff));             \
    } else {                                                                        \
        m_eLastErrCode = errorCode;                                                 \
    }                                                                               \
    NddsExternalLog(E_DEBUG, "%sNDDS FAIL NORMAL :: code = %d, oriCode = %d", (b == 1 ? "" : "non"), m_eLastErrCode, errorCode);   \
    goto NDDS_FAIL;                                                                 \
}


CNddsLibBase::CNddsLibBase()
{
    m_eLastErrCode = NDDSERR_SUCCESS;
    m_pSessionData = nullptr;
    m_pHttpHeaderData = nullptr;
    m_pNDDSCommonHeaderInfo = nullptr;
    m_bUseECMEngine = false;
}

CNddsLibBase::~CNddsLibBase()
{
}

void CNddsLibBase::Init(NDDS_PROTOCOL_SESSION_DATA *pSessionData)
{
    CNddsLibCommonData &CommonData = CNddsLibCommonData::GetInstance();

    m_pSessionData = pSessionData;

    strcpy(m_szHost, CommonData.m_szTmapHost);
    m_nPort = CommonData.m_nTmapPort;

    m_bUseECMEngine = (strstr(m_szHost, VNEXT_DOMAIN) != nullptr);
    if (m_bUseECMEngine == true) {
        m_nPort = 443;
        m_bUseDefaultPort = true;
    }

    m_pHttpHeaderData = &pSessionData->sHttpHeader;
    m_pNDDSCommonHeaderInfo = &pSessionData->sCommonHeader;
}

bool CNddsLibBase::RequestServer()
{
    bool bRet;
    int nRetryCount = 0;
    int nWriteBytes, nSendBytes;
    int nReadBytes, nReceiveBytes, nHeaderReceive;
    unsigned char *pReceiveBuffer = nullptr;
    unsigned char *pReceiveTmp = nullptr;
    NDDS_HTTP_RESULTDATA httpResult;

    int contentsLength = 500000, nTotalLength = 500000;
    bool bHttpHeader = false;
    int nProgressStart = 0;
    int nProgressStep = 0;
    int nPort = 443;
    char szHost[256] = {0,};
    char *pszEndOfPort;
    char *pszStartOfHost;
    char *pszStartOfPort;
    unsigned char *pServiceCloseBuf = nullptr;
    const char *pReqData;
    int nReqData;

    FILE* lgfp = NULL;


    NddsExternalLog(E_DEBUG, "nddslibbase :: in RequestServer 1");
#ifdef __SSL_SOCKET__
    SSL_VARS ssl_vars;
    eNddsErrorCode result = NDDSERR_SUCCESS;
#endif

    CNddsLibCommonData &CommonData = CNddsLibCommonData::GetInstance();

    if ((CommonData.m_bLogSave) && (strlen(CommonData.m_szLogPath) > 0)) {
        NddsExternalLog(E_DEBUG, "nddslibbase :: in RequestServer ---- LOG FILE OPEN");
        char fname[256] = {0};
        sprintf(fname, "%s/%d_%d_%d_%d.ndds.log", CommonData.m_szLogPath, m_pSessionData->eProtocolType, m_pSessionData->eRequestType, m_pSessionData->nReqTime, m_pSessionData->nSessionID);
        lgfp = fopen(fname, "w");
    }

    NddsExternalLog(E_DEBUG, "nddslibbase :: in RequestServer 2");
    if (m_pSessionData == nullptr) {
        FAIL_NDDS_PROTOCOL(NDDSERR_SESSION_NULL, 1);
    }
    if (m_pSessionData->pRequestData == nullptr) {
        FAIL_NDDS_PROTOCOL(NDDSERR_REQUEST_NULL, 1);
    }

    NddsExternalLog(E_DEBUG, "nddslibbase :: in RequestServer 3");
    pReqData = const_cast<const char*>(m_pSessionData->pRequestData);
    nReqData = strlen(pReqData);

    m_pSessionData->nProgress = 1;
    CHECK_NDDS_USER_CANCEL(1);

    NddsExternalLog(E_DEBUG, "nddslibbase :: in RequestServer 4");
    pszStartOfHost = strstr(const_cast<char*>(pReqData), "Host: ");
    if (pszStartOfHost == nullptr) {
        FAIL_NDDS_PROTOCOL(NDDSERR_HTTPPROTOCOL, 1);
    }
    pszStartOfHost += 6;

    NddsExternalLog(E_DEBUG, "nddslibbase :: in RequestServer 5");
    pszEndOfPort = strstr(const_cast<char*>(pszStartOfHost), "\r\n");
    if (pszEndOfPort == nullptr) {
        FAIL_NDDS_PROTOCOL(NDDSERR_HTTPPROTOCOL, 1);
    }

    strncpy(szHost, pszStartOfHost, static_cast<size_t>(pszEndOfPort - pszStartOfHost));

    NddsExternalLog(E_DEBUG, "nddslibbase :: in RequestServer 6");
    pszStartOfPort = strstr(szHost, ":");
    if (pszStartOfPort == nullptr)
    {
        nPort = 443;
        strcat(szHost, ":https");
    }
    else
    {
        pszStartOfPort++;
        nPort = atoi(pszStartOfPort);
    }


    //////////////////////////////////////////////////////////////////////////
    // 서비스 요청

    NddsExternalLog(E_DEBUG, "nddslibbase :: in RequestServer 7 - hostname: %s, port: %d", m_szHost, m_nPort);
    if ((result = static_cast<eNddsErrorCode>(ndds_ssl_open_and_connect(m_szHost, static_cast<int>(m_nPort), &ssl_vars, m_bUseECMEngine, nRetryCount))) != NDDSERR_SUCCESS) {
        FAIL_NDDS_PROTOCOL(result, 1);
    }

    NddsExternalLog(E_DEBUG, "nddslibbase :: in RequestServer 8");
    m_pSessionData->nProgress = 5;
    delay(TIME_DELAY);
    CHECK_NDDS_USER_CANCEL(1);

#if 0
    //////////////////////////////////////////////////////////////////////////
    // 요청 데이터 송신
    //////////////////////////////////////////////////////////////////////////

    if ((CommonData.m_bLogSave) && (strlen(CommonData.m_szLogPath)>0))
    {
        FILE *fpNddsBuf = nullptr;
        char szSavePath[300] = {};
        sprintf(szSavePath, "%s/NDDSREQ_%d_%d.log", CommonData.m_szLogPath, m_pSessionData->nReqTime, m_pSessionData->eProtocolType);
        printf("is dir szSavePath=%s\n", szSavePath);
        fpNddsBuf = fopen(szSavePath, "w");
        if (fpNddsBuf)
        {
            fwrite(pReqData, nReqData, 1, fpNddsBuf);
            fclose(fpNddsBuf);
            fpNddsBuf = nullptr;
        }
    }
#endif

    nProgressStart = m_pSessionData->nProgress;
    nProgressStep = (nReqData/40960 + 1) * 3;
    if (nProgressStep > 30) nProgressStep = 30;

    nSendBytes = 0;
    if (lgfp != NULL)
    {
        fwrite(pReqData, nReqData, 1, lgfp);
        fwrite("\n\n", 2, 1, lgfp);
        fflush(lgfp);
    }
    NddsExternalLog(E_DEBUG, "%s", (char*)pReqData);
    NddsExternalLog(E_DEBUG, "ndds send before :: %d", nReqData);
    while (1)
    {
        nWriteBytes = BIO_write(ssl_vars.sbio, pReqData + nSendBytes, nReqData - nSendBytes);
        if (nWriteBytes == NDDS_SOCKET_ERROR)
        {
            if (nRetryCount >= RETRY_COUNT)// || BIO_should_write(ssl_vars.sbio) <= 0)
            {
                FAIL_NDDS_PROTOCOL(NDDSERR_SOCKETSEND, 1);
            }
            nRetryCount++;
            delay(RETRY_DELAY);
            continue;
        }

        nSendBytes += nWriteBytes;

        m_pSessionData->nProgress = nProgressStart + nProgressStep * nSendBytes / nReqData;
        delay(TIME_DELAY);
        CHECK_NDDS_USER_CANCEL(1);

        //전송 완료
        if (nSendBytes == nReqData) {
            NddsExternalLog(E_DEBUG, "ndds send complete :: %d", nSendBytes);
            break;
        }
    }

    m_pSessionData->nProgress = nProgressStart + nProgressStep;
    delay(TIME_DELAY);
    CHECK_NDDS_USER_CANCEL(1);

    //////////////////////////////////////////////////////////////////////////
    // 응답 데이터 수신
    //////////////////////////////////////////////////////////////////////////

    nReceiveBytes = 0;
    pReceiveTmp = CommonData.GetMemory(COMM_RECIVE_BUFFER + 1);
    pReceiveBuffer = pReceiveTmp;

    nProgressStep = 80 - m_pSessionData->nProgress;
    nProgressStart = m_pSessionData->nProgress;
    NddsExternalLog(E_DEBUG, "ndds recv before");
    while (1)
    {
        NddsExternalLog(E_DEBUG, "ndds recv 1 :: calc NeedRecvBytes");
        int nNeedReceiveBytes = COMM_RECIVE_BUFFER;
        if (bHttpHeader == false)       // header 수신 중인 경우
        {
            nNeedReceiveBytes = COMM_RECIVE_BUFFER - nReceiveBytes;
            NddsExternalLog(E_DEBUG, "ndds recv 1-1 :: nReceiveBytes = %d, nNeedReceiveBytes = %d", nReceiveBytes, nNeedReceiveBytes);
        }
        else if (nTotalLength - nReceiveBytes < COMM_RECIVE_BUFFER)     // header 이후 부분 수신 중이면서, 더 받아야할 크기가 COMM_RECEIVE_BUFFER보다 작은 경우
        {
            nNeedReceiveBytes = nTotalLength - nReceiveBytes;
            NddsExternalLog(E_DEBUG, "ndds recv 1-2 :: nReceiveBytes = %d, nNeedReceiveBytes = %d, nTotalLength = %d", nReceiveBytes, nNeedReceiveBytes, nTotalLength);
        }

        if (nNeedReceiveBytes <= 0) // 다 받은 경우
        {
            NddsExternalLog(E_DEBUG, "ndds recv complete1 :: nReceiveBytes = %d", nReceiveBytes);
            pReceiveBuffer[nReceiveBytes] = 0;
            break;
        }

        NddsExternalLog(E_DEBUG, "ndds recv 2 :: BIO_read");
        nReadBytes = BIO_read(ssl_vars.sbio, pReceiveBuffer + nReceiveBytes, nNeedReceiveBytes);
        NddsExternalLog(E_DEBUG, "ndds recv 2-1 :: nReadBytes = %d", nReadBytes);
        if (nReadBytes == NDDS_SOCKET_ERROR)
        {
            NddsExternalLog(E_DEBUG, "ndds recv 2-2 :: nRetryCount = %d", nRetryCount);
            if (nRetryCount >= RETRY_COUNT)// || BIO_should_read(ssl_vars.sbio) <= 0)
            {
                NddsExternalLog(E_DEBUG, "ndds recv fail1 :: %d", NDDSERR_SOCKETRECEIVE);
                FAIL_NDDS_PROTOCOL(NDDSERR_SOCKETRECEIVE, 1);
            }
            nRetryCount++;
            delay(RETRY_DELAY);
            NddsExternalLog(E_DEBUG, "ndds recv 2-4 :: nRetryCount = %d", nRetryCount);
            continue;
        }

        //데이터 수신이 완료
        if (nReadBytes == 0)
        {
            NddsExternalLog(E_DEBUG, "ndds recv 2-5 :: nRetryCount = %d, nReadBytes = %d, nReceiveBytes = %d, nTotalLength = %d", nRetryCount, nReadBytes, nReceiveBytes, nTotalLength);
            // 데이터가 아직 안받아졌으면 다시 받는다.
            if (nReceiveBytes < nTotalLength)
            {
                if (nRetryCount < RETRY_COUNT)
                {
                    nRetryCount++;
                    delay(RETRY_DELAY);
                    NddsExternalLog(E_DEBUG, "ndds recv 2-6 :: nRetryCount = %d, nReadBytes = %d, nReceiveBytes = %d, nTotalLength = %d", nRetryCount, nReadBytes, nReceiveBytes, nTotalLength);
                    continue;
                }
                else
                {
                    NddsExternalLog(E_DEBUG, "ndds recv fail2 :: %d", NDDSERR_SOCKETRECEIVE);
                    FAIL_NDDS_PROTOCOL(NDDSERR_SOCKETRECEIVE, 1);
                }
            }

            NddsExternalLog(E_DEBUG, "ndds recv complete2 :: %d", nReceiveBytes);
            pReceiveBuffer[nReceiveBytes] = 0;
            break;
        }

        if (lgfp != NULL)
        {
            fwrite(pReceiveBuffer + nReceiveBytes, nReadBytes, 1, lgfp);
            fflush(lgfp);
        }
        NddsExternalLog(E_DEBUG, "ndds recv 2-7 :: nRetryCount = %d, nReadBytes = %d, nReceiveBytes = %d, nTotalLength = %d", nRetryCount, nReadBytes, nReceiveBytes, nTotalLength);
        nReceiveBytes += nReadBytes;

        NddsExternalLog(E_DEBUG, "ndds recv 3 :: check header :: bHttpHeader = %d, nReceiveBytes = %d", bHttpHeader, nReceiveBytes);
        if (!bHttpHeader && nReceiveBytes>4)
        {
            char *pos;
            pReceiveBuffer[nReceiveBytes] = 0;
            NddsExternalLog(E_DEBUG, "ndds recv 3-1");
            pos = strstr((char*)pReceiveBuffer, "\r\n\r\n");
            NddsExternalLog(E_DEBUG, "ndds recv 3-2 :: pos = %x", (void*)pos);
            if (pos != nullptr)
            {
                nHeaderReceive = static_cast<int>((pos - reinterpret_cast<char*>(pReceiveBuffer)) + 4);
                NddsExternalLog(E_DEBUG, "ndds recv 3-3 :: nHeaderReceive = %d", nHeaderReceive);
                //////////////////////////////////////////////////////////////////////////
                // 응답 Http 헤더 파싱
                memset(&httpResult, 0x00, sizeof(httpResult));
                if (NddsLibHttpHeader::NddsParserHeader(reinterpret_cast<char*>(pReceiveBuffer), nHeaderReceive, &httpResult) > 0)
                {
                    nTotalLength = nHeaderReceive + httpResult.contentsLength;
                    NddsExternalLog(E_DEBUG, "ndds recv 3-4 :: nRetryCount = %d, nTotalLength = %d, nHeaderReceive = %d, httpResult.contentsLength = %d", nRetryCount, nTotalLength, nHeaderReceive, httpResult.contentsLength);

                    // Http 해더 수신이 완료되면 전체 수신 받을 메모리를 재할당 하고 이전 받은 데이터를 복사한다.
                    pReceiveBuffer = CommonData.GetMemory(nTotalLength+1);
                    memcpy(pReceiveBuffer, pReceiveTmp, static_cast<size_t>(nReceiveBytes));
                    CommonData.FreeMemory(pReceiveTmp);
                    pReceiveTmp = nullptr;

                    bHttpHeader = true;
                    NddsExternalLog(E_DEBUG, "ndds recv 3-5 :: header complete :: nRetryCount = %d, nTotalLength = %d, nHeaderReceive = %d, httpResult.contentsLength = %d, nReceiveBytes = %d", nRetryCount, nTotalLength, nHeaderReceive, httpResult.contentsLength, nReceiveBytes);
                }
            }
        }

        m_pSessionData->nProgress = nProgressStart + nProgressStep * nReceiveBytes / nTotalLength;
        delay(TIME_DELAY);
        CHECK_NDDS_USER_CANCEL(1);
    }

#ifdef __SSL_SOCKET__
    ndds_ssl_close(&ssl_vars);
#endif

    m_pSessionData->nProgress = 82;
    delay(TIME_DELAY);
    CHECK_NDDS_USER_CANCEL(1);

    if (!bHttpHeader || nReceiveBytes == 0)
    {
        FAIL_NDDS_PROTOCOL(NDDSERR_SOCKETRECEIVE_EMPTY, 1);
    }

    //////////////////////////////////////////////////////////////////////////

    NDDS_MEMORYSTATUS memory;
    memory.nMemory = nReceiveBytes;
    memory.pMemory = (unsigned char*)malloc(memory.nMemory);
    memory.pUse = memory.pMemory;
    memory.nUse = 0;
    memset(memory.pMemory,0x00,memory.nMemory);

    //////////////////////////////////////////////////////////////////////////
    // 응답 데이터 파싱

//    DBGPRINT("navi_native", "\n%s\n", pReceiveBuffer);
/*
    static QString str;
    str.sprintf((char*)pReceiveBuffer);
    static QStringList strs = str.split("\r\n");
    for (int ii = 0; ii < strs.length(); ii++) {
        NddsExternalLog(E_DEBUG, "hhorigtest %d, %s", ii, strs.at(ii));
    }
*/
    if (httpResult.resultCode == 200)
    {
        if (httpResult.contentType == NDDS_BINARY)
            bRet = ParserBinary(m_pSessionData->eProtocolType, pReceiveBuffer+nHeaderReceive, httpResult.contentsLength-NDDS_BINARY_COMMON_HEADER_LEN, &m_pSessionData->pResultStruct, memory);
        else
            bRet = ParserJson(m_pSessionData->eProtocolType, (char *)pReceiveBuffer+nHeaderReceive, httpResult.contentsLength, &m_pSessionData->pResultStruct, memory);
    }
    else
    {
        bRet = false;
        m_eLastErrCode = NDDSERR_HTTPPROTOCOL;
        m_pSessionData->pResultStruct = (NDDS_COMMON_RES*)malloc(sizeof(NDDS_COMMON_RES));
        memset(m_pSessionData->pResultStruct, 0x00, sizeof(NDDS_COMMON_RES));
        NddsExternalLog(E_DEBUG, "NDDS FAIL HTTPPROTOCOL :: %d", httpResult.resultCode);
    }

    if (pReceiveBuffer)
    {
        CommonData.FreeMemory(pReceiveBuffer);
        pReceiveBuffer = nullptr;
    }

    // HTTP 파싱 결과 설정.
    ((NDDS_COMMONHEADER_RES *)m_pSessionData->pResultStruct)->tigServiceInfo = httpResult.TIG_Service_Info;
    ((NDDS_COMMONHEADER_RES *)m_pSessionData->pResultStruct)->tnappResultCode = httpResult.resultCode;
    ALLOC_STR_COPY(httpResult.TigMsg,((NDDS_COMMONHEADER_RES *)m_pSessionData->pResultStruct)->tigMessage,strlen(httpResult.TigMsg),memory);
    free(httpResult.TigMsg);
    if (strlen(httpResult.accessKey) > 0)
        ALLOC_STR_COPY(httpResult.accessKey,((NDDS_COMMONHEADER_RES *)m_pSessionData->pResultStruct)->accessKey,strlen(httpResult.accessKey),memory);
    //////////////////////////////////////////////////////////////////////////////

    memcpy(CommonData.m_prevResponseServiceID, httpResult.serviceID, 15);

    m_pSessionData->nProgress = 95;
    delay(TIME_DELAY);
    CHECK_NDDS_USER_CANCEL(1);


//    if (CheckServiceClose(m_pSessionData))
//    {
//        CHECK_NDDS_USER_CANCEL();

//        if (bRet)
//        {
//            /************************************************************************/
//            /* 서비스 종료                                                          */
//            /************************************************************************/

//            int nServiceCloseBuf = MakeServiceCloseRequestData(&pServiceCloseBuf, httpResult.serviceID);

//            if ((s = socket(PF_INET, SOCK_STREAM, 0)) < 0)
//            {
//                FAIL_NDDS_PROTOCOL(NDDSERR_SOCKETCREATE);
//            }
//            CHECK_NDDS_USER_CANCEL();

//#ifdef __SSL_SOCKET__
//            SSLeay_add_ssl_algorithms();
//            meth = TLSv1_2_client_method();
//            SSL_load_error_strings();
//            ctx = SSL_CTX_new (meth);

//            if (ctx == nullptr)
//                FAIL_NDDS_PROTOCOL(NDDSERR_SSL_CTX);

//            SSL_CTX_set_cipher_list(ctx, "ECDHE-RSA-AES256-GCM-SHA384");
//#endif

//            if (connect(s, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
//            {
//                FAIL_NDDS_PROTOCOL(NDDSERR_SOCKETCONNECT);
//            }
//            CHECK_NDDS_USER_CANCEL();

//#ifdef __SSL_SOCKET__

//            // Now we have TCP conncetion. Start SSL negotiation.
//            // create a new SSL structure for a connection
//            ssl = SSL_new ( ctx );
//            if (ssl == nullptr)
//                FAIL_NDDS_PROTOCOL(NDDSERR_SSL_NEW);

//            // connect the SSL object with a file descriptor
//        #ifdef WIN321
//            SSL_set_fd (ssl, m_pSocket->m_hSocket);
//        #else
//            //SSL_set_fd (ssl, m_pSocket->m_nAddr);
//            SSL_set_fd (ssl, s);
//        #endif

//            // initiate the TLS/SSL handshake with an TLS/SSL server
//            err = SSL_connect ( ssl );
//            if (err == -1)
//                FAIL_NDDS_PROTOCOL(NDDSERR_SSL_CONNECT);

//            /*
//             * Following two steps are optional and not required for
//             * data exchange to be successful.
//             */

//            //printf ("SSL connection using %s\n", SSL_get_cipher (ssl));

//            SSL_get_cipher ( ssl );

//            // Get server's certificate (note: beware of dynamic allocation) - opt
//            server_cert = SSL_get_peer_certificate ( ssl );
//            if (server_cert == nullptr)
//                FAIL_NDDS_PROTOCOL(NDDSERR_SSL_CERT);

//            //print_x509(ssl);

//            /*
//            printf ("Server certificate:\n");
//            str = X509_NAME_oneline (X509_get_subject_name (server_cert),0,0);
//            CHK_NULL(str);
//            printf ("\t subject: %s\n", str);
//            free (str);

//            str = X509_NAME_oneline (X509_get_issuer_name  (server_cert),0,0);
//            CHK_NULL(str);
//            printf ("\t issuer: %s\n", str);
//            free (str);
//            */

//            /*
//             * We could do all sorts of certificate verification stuff here before
//             * deallocating the certificate.
//             */
//            X509_free ( server_cert );
//#endif

//            nSendBytes = 0;
//            while (1)
//            {
//#ifdef __SSL_SOCKET__
//                nWriteBytes = SSL_write (ssl, pServiceCloseBuf+nSendBytes, nServiceCloseBuf-nSendBytes);
//#else
//                //nWriteBytes = m_pSocket->Send(pServiceCloseBuf+nSendBytes, nServiceCloseBuf-nSendBytes);
//                const char *pServiceCloseBuf2 = (const char*)pServiceCloseBuf;
//                nWriteBytes = send(s, pServiceCloseBuf2 , strlen(pServiceCloseBuf2) , 0);
//#endif
//                CHECK_NDDS_USER_CANCEL();

//                nSendBytes += nWriteBytes;

//                if (nWriteBytes == NDDS_SOCKET_ERROR)
//                {
//                    FAIL_NDDS_PROTOCOL(NDDSERR_SOCKETSEND);
//                }

//                //전송 완료
//                if (nSendBytes == nServiceCloseBuf)
//                    break;
//            }

//            if (pServiceCloseBuf)
//            {
//                CNddsLibCommonData &CommonData = CNddsLibCommonData::GetInstance();
//                CommonData.FreeMemory((unsigned char*)pServiceCloseBuf);
//            }

//#ifdef __SSL_SOCKET__
//            if (ssl != nullptr)
//            {
//                SSL_shutdown (ssl);
//                SSL_free (ssl);
//                ssl = nullptr;
//            }
//            if (ctx != nullptr)
//            {
//                SSL_CTX_free (ctx);
//                ctx = nullptr;
//            }
//#endif

//#ifdef _LINUX
//            close(s);
//#else
//            closesocket(s);
//#endif
//        }

//        m_pSessionData->nProgress = 100;

//        return bRet;
//    }

    m_pSessionData->nProgress = 100;

    if (lgfp != NULL)
    {
        fflush(lgfp);
        fclose(lgfp);
        lgfp = NULL;
    }

    return bRet;

NDDS_FAIL:

    if (pServiceCloseBuf)
    {
        CNddsLibCommonData &CommonData = CNddsLibCommonData::GetInstance();
        CommonData.FreeMemory((unsigned char*)pServiceCloseBuf);
    }

#ifdef __SSL_SOCKET__
    ndds_ssl_close(&ssl_vars);
#endif

    if (pReceiveBuffer)
    {
        CommonData.FreeMemory(pReceiveBuffer);
        pReceiveBuffer = nullptr;
    }

    if (lgfp != NULL)
    {
        fflush(lgfp);
        fclose(lgfp);
        lgfp = NULL;
    }

    return false;
}

bool CNddsLibBase::RequestNonNddsServer()
{
    bool bRet;
    int nRetryCount = 0;
    int nWriteBytes, nSendBytes;
    int nReadBytes, nReceiveBytes, nHeaderReceive;
    unsigned char *pReceiveBuffer = nullptr;
    unsigned char *pReceiveTmp = nullptr;
    NDDS_HTTP_RESULTDATA httpResult;

    int contentsLength = 500000, nTotalLength = 500000;
    bool bHttpHeader = false;
    int nProgressStart = 0;
    int nProgressStep = 0;
    int nPort = 443;
    char szHost[256] = {0,};
    char *pszEndOfPort;
    char *pszStartOfHost;
    char *pszStartOfPort;
    unsigned char *pServiceCloseBuf = nullptr;
    const char *pReqData;
    int nReqData;

    FILE* lgfp = NULL;


    NddsExternalLog(E_DEBUG, "nddslibbase :: in RequestNonNddsServer 1");
#ifdef __SSL_SOCKET__
    SSL_VARS ssl_vars;
    eNddsErrorCode result = NDDSERR_SUCCESS;
#endif

    CNddsLibCommonData &CommonData = CNddsLibCommonData::GetInstance();

    if ((CommonData.m_bLogSave) && (strlen(CommonData.m_szLogPath) > 0)) {
        NddsExternalLog(E_DEBUG, "nddslibbase :: in RequestNonNddsServer ---- LOG FILE OPEN");
        char fname[256] = {0};
        sprintf(fname, "%s/%d_%d_%d_%d.nonndds.log", CommonData.m_szLogPath, m_pSessionData->eProtocolType, m_pSessionData->eRequestType, m_pSessionData->nReqTime, m_pSessionData->nSessionID);
        lgfp = fopen(fname, "w");
    }

    NddsExternalLog(E_DEBUG, "nddslibbase :: in RequestNonNddsServer 2");
    if (m_pSessionData == nullptr) {
        FAIL_NDDS_PROTOCOL(NDDSERR_SESSION_NULL, 0);
    }
    if (m_pSessionData->pRequestData == nullptr) {
        FAIL_NDDS_PROTOCOL(NDDSERR_REQUEST_NULL, 0);
    }

    NddsExternalLog(E_DEBUG, "nddslibbase :: in RequestNonNddsServer 3");
    pReqData = const_cast<const char*>(m_pSessionData->pRequestData);
    nReqData = static_cast<int>(strlen(pReqData));

    m_pSessionData->nProgress = 1;
    CHECK_NDDS_USER_CANCEL(0);

    NddsExternalLog(E_DEBUG, "nddslibbase :: in RequestNonNddsServer 4");
    pszEndOfPort = strstr(const_cast<char*>(pReqData), "\r\nContent-Type:");
    if (pszEndOfPort == nullptr) {
        FAIL_NDDS_PROTOCOL(NDDSERR_HTTPPROTOCOL, 0);
    }

    NddsExternalLog(E_DEBUG, "nddslibbase :: in RequestNonNddsServer 5");
    pszStartOfHost = strstr(const_cast<char*>(pReqData), "Host: ");
    if (pszStartOfHost == nullptr) {
        FAIL_NDDS_PROTOCOL(NDDSERR_HTTPPROTOCOL, 0);
    }
    pszStartOfHost += 6;

    strncpy(szHost, pszStartOfHost, static_cast<size_t>(pszEndOfPort - pszStartOfHost));

    NddsExternalLog(E_DEBUG, "nddslibbase :: in RequestNonNddsServer 6");
    pszStartOfPort = strstr(szHost, ":");
    if (pszStartOfPort == nullptr)
    {
        nPort = 443;
    }
    else
    {
        pszStartOfPort++;
        nPort = atoi(pszStartOfPort);
        pszStartOfPort = nullptr;
    }


    //////////////////////////////////////////////////////////////////////////
    // 서비스 요청

    NddsExternalLog(E_DEBUG, "nddslibbase :: in RequestNonNddsServer 7 - hostname: %s, port: %d", szHost, nPort);
    if ((result = static_cast<eNddsErrorCode>(ndds_ssl_open_and_connect(szHost, static_cast<int>(nPort), &ssl_vars, m_bUseECMEngine, nRetryCount))) != NDDSERR_SUCCESS) {
        FAIL_NDDS_PROTOCOL(result, 0);
    }

    NddsExternalLog(E_DEBUG, "nddslibbase :: in RequestNonNddsServer 8");
    m_pSessionData->nProgress = 5;
    delay(TIME_DELAY);
    CHECK_NDDS_USER_CANCEL(0);

#if 0
    //////////////////////////////////////////////////////////////////////////
    // 요청 데이터 송신
    //////////////////////////////////////////////////////////////////////////

    if ((CommonData.m_bLogSave) && (strlen(CommonData.m_szLogPath)>0))
    {
        FILE *fpNddsBuf = nullptr;
        char szSavePath[300] = {};
        sprintf(szSavePath, "%s/NDDSREQ_%d_%d.log", CommonData.m_szLogPath, m_pSessionData->nReqTime, m_pSessionData->eProtocolType);
        printf("is dir szSavePath=%s\n", szSavePath);
        fpNddsBuf = fopen(szSavePath, "w");
        if (fpNddsBuf)
        {
            fwrite(pReqData, nReqData, 1, fpNddsBuf);
            fclose(fpNddsBuf);
            fpNddsBuf = nullptr;
        }
    }
#endif

    nProgressStart = m_pSessionData->nProgress;
    nProgressStep = (nReqData/40960 + 1) * 3;
    if (nProgressStep > 30) nProgressStep = 30;

    nSendBytes = 0;
    if (lgfp != NULL)
    {
        fwrite(pReqData, nReqData, 1, lgfp);
        fwrite("\n\n", 2, 1, lgfp);
        fflush(lgfp);
    }
    NddsExternalLog(E_DEBUG, "%s", (char*)pReqData);
    NddsExternalLog(E_DEBUG, "nonndds send before :: %d", nReqData);
    while (1)
    {
        nWriteBytes = BIO_write(ssl_vars.sbio, pReqData + nSendBytes, nReqData - nSendBytes);
        if (nWriteBytes == NDDS_SOCKET_ERROR)
        {
            if (nRetryCount >= RETRY_COUNT)// || BIO_should_write(ssl_vars.sbio) <= 0)
            {
                FAIL_NDDS_PROTOCOL(NDDSERR_SOCKETSEND, 0);
            }
            nRetryCount++;
            delay(RETRY_DELAY);
            continue;
        }

        nSendBytes += nWriteBytes;

        m_pSessionData->nProgress = nProgressStart + nProgressStep * nSendBytes / nReqData;
        delay(TIME_DELAY);
        CHECK_NDDS_USER_CANCEL(0);

        //전송 완료
        if (nSendBytes == nReqData) {
            NddsExternalLog(E_DEBUG, "nonndds send complete :: %d", nSendBytes);
            break;
        }
    }

    m_pSessionData->nProgress = nProgressStart + nProgressStep;
    delay(TIME_DELAY);
    CHECK_NDDS_USER_CANCEL(0);

    //////////////////////////////////////////////////////////////////////////
    // 응답 데이터 수신
    //////////////////////////////////////////////////////////////////////////

    nReceiveBytes = 0;
    pReceiveTmp = CommonData.GetMemory(COMM_RECIVE_BUFFER + 1);
    pReceiveBuffer = pReceiveTmp;

    nProgressStep = 80 - m_pSessionData->nProgress;
    nProgressStart = m_pSessionData->nProgress;
    NddsExternalLog(E_DEBUG, "nonndds recv before");
    while (1)
    {
        NddsExternalLog(E_DEBUG, "nonndds recv 1 :: calc NeedRecvBytes");
        int nNeedReceiveBytes = COMM_RECIVE_BUFFER;
        if (bHttpHeader == false)       // header 수신 중인 경우
        {
            nNeedReceiveBytes = COMM_RECIVE_BUFFER - nReceiveBytes;
            NddsExternalLog(E_DEBUG, "nonndds recv 1-1 :: nReceiveBytes = %d, nNeedReceiveBytes = %d", nReceiveBytes, nNeedReceiveBytes);
        }
        else if (nTotalLength - nReceiveBytes < COMM_RECIVE_BUFFER)     // header 이후 부분 수신 중이면서, 더 받아야할 크기가 COMM_RECEIVE_BUFFER보다 작은 경우
        {
            nNeedReceiveBytes = nTotalLength - nReceiveBytes;
            NddsExternalLog(E_DEBUG, "nonndds recv 1-2 :: nReceiveBytes = %d, nNeedReceiveBytes = %d, nTotalLength = %d", nReceiveBytes, nNeedReceiveBytes, nTotalLength);
        }

        if (nNeedReceiveBytes <= 0) // 다 받은 경우
        {
            NddsExternalLog(E_DEBUG, "nonndds recv complete1 :: nReceiveBytes = %d", nReceiveBytes);
            pReceiveBuffer[nReceiveBytes] = 0;
            if (bHttpHeader && httpResult.contentType != NDDS_BINARY)
                NddsExternalLog(E_DEBUG, "%s", (char*)pReceiveBuffer);
            break;
        }

        NddsExternalLog(E_DEBUG, "nonndds recv 2 :: BIO_read");
        nReadBytes = BIO_read(ssl_vars.sbio, pReceiveBuffer + nReceiveBytes, nNeedReceiveBytes);
        NddsExternalLog(E_DEBUG, "nonndds recv 2-1 :: nReadBytes = %d", nReadBytes);
        if (nReadBytes == NDDS_SOCKET_ERROR)
        {
            NddsExternalLog(E_DEBUG, "nonndds recv 2-2 :: nRetryCount = %d", nRetryCount);
            if (nRetryCount >= RETRY_COUNT)// || BIO_should_read(ssl_vars.sbio) <= 0)
            {
                NddsExternalLog(E_DEBUG, "nonndds recv fail1 :: %d", NDDSERR_SOCKETRECEIVE);
                FAIL_NDDS_PROTOCOL(NDDSERR_SOCKETRECEIVE, 0);
            }
            nRetryCount++;
            delay(RETRY_DELAY);
            NddsExternalLog(E_DEBUG, "nonndds recv 2-4 :: nRetryCount = %d", nRetryCount);
            continue;
        }

        //데이터 수신이 완료
        if (nReadBytes == 0)
        {
            NddsExternalLog(E_DEBUG, "nonndds recv 2-5 :: nRetryCount = %d, nReadBytes = %d, nReceiveBytes = %d, nTotalLength = %d", nRetryCount, nReadBytes, nReceiveBytes, nTotalLength);
            // 데이터가 아직 안받아졌으면 다시 받는다.
            if (nReceiveBytes < nTotalLength)
            {
                if (nRetryCount < RETRY_COUNT)
                {
                    nRetryCount++;
                    delay(RETRY_DELAY);
                    NddsExternalLog(E_DEBUG, "nonndds recv 2-6 :: nRetryCount = %d, nReadBytes = %d, nReceiveBytes = %d, nTotalLength = %d", nRetryCount, nReadBytes, nReceiveBytes, nTotalLength);
                    continue;
                }
                else
                {
                    NddsExternalLog(E_DEBUG, "nonndds recv fail2 :: %d", NDDSERR_SOCKETRECEIVE);
                    FAIL_NDDS_PROTOCOL(NDDSERR_SOCKETRECEIVE, 1);
                }
            }

            NddsExternalLog(E_DEBUG, "nonndds recv complete2 :: %d", nReceiveBytes);
            pReceiveBuffer[nReceiveBytes] = 0;
            if (bHttpHeader && httpResult.contentType != NDDS_BINARY)
                NddsExternalLog(E_DEBUG, "%s", (char*)pReceiveBuffer);
            break;
        }

        if (bHttpHeader && httpResult.contentType != NDDS_BINARY)
        {
            pReceiveBuffer[nReceiveBytes + nReadBytes] = 0;
            NddsExternalLog(E_DEBUG, "%s", (char*)(pReceiveBuffer + nReceiveBytes));
        }

        if (lgfp != NULL)
        {
            fwrite(pReceiveBuffer + nReceiveBytes, nReadBytes, 1, lgfp);
            fflush(lgfp);
        }
        NddsExternalLog(E_DEBUG, "nonndds recv 2-7 :: nRetryCount = %d, nReadBytes = %d, nReceiveBytes = %d, nTotalLength = %d", nRetryCount, nReadBytes, nReceiveBytes, nTotalLength);
        nReceiveBytes += nReadBytes;

        NddsExternalLog(E_DEBUG, "nonndds recv 3 :: check header :: bHttpHeader = %d, nReceiveBytes = %d", bHttpHeader, nReceiveBytes);
        if (!bHttpHeader && nReceiveBytes > 4)
        {
            char *endOfHeader;
            pReceiveBuffer[nReceiveBytes] = 0;
            NddsExternalLog(E_DEBUG, "nonndds recv 3-1");
            endOfHeader = strstr(reinterpret_cast<char*>(pReceiveBuffer), "\r\n\r\n");
            NddsExternalLog(E_DEBUG, "nonndds recv 3-2 :: endOfHeader = %x", (void*)endOfHeader);
            if (endOfHeader != nullptr)
            {
                nHeaderReceive = static_cast<int>((endOfHeader - reinterpret_cast<char*>(pReceiveBuffer)) + 4);
                NddsExternalLog(E_DEBUG, "nonndds recv 3-3 :: nHeaderReceive = %d", nHeaderReceive);
                //////////////////////////////////////////////////////////////////////////
                // 응답 Http 헤더 파싱
                memset(&httpResult, 0x00, sizeof(httpResult));
                if (NddsLibHttpHeader::NddsParserHeader(reinterpret_cast<char*>(pReceiveBuffer), nHeaderReceive, &httpResult) > 0)
                {
                    if (httpResult.resultCode != 200) {
                        int err = (abs(NDDSERR_HTTPPROTOCOL)) | (httpResult.resultCode << 16);
                        FAIL_NDDS_PROTOCOL(eNddsErrorCode(0 - err), 0);
                    }

                    nTotalLength = nHeaderReceive + httpResult.contentsLength;
                    NddsExternalLog(E_DEBUG, "nonndds recv 3-4 :: nRetryCount = %d, nTotalLength = %d, nHeaderReceive = %d, httpResult.contentsLength = %d", nRetryCount, nTotalLength, nHeaderReceive, httpResult.contentsLength);

                    char *startOfBody = strstr(endOfHeader, "{");
                    if (startOfBody == nullptr) {
                        startOfBody = strstr(endOfHeader, "PNG");
                        if (startOfBody == nullptr) {
                            startOfBody = reinterpret_cast<char*>(&pReceiveBuffer[nReceiveBytes]);
                        }
                    }

                    // Http 해더 수신이 완료되면 전체 수신 받을 메모리를 재할당 하고 이전 받은 데이터를 복사한다.
                    if (nReceiveBytes > nTotalLength) {
                        pReceiveBuffer = CommonData.GetMemory(nReceiveBytes+1);
                    } else {
                        pReceiveBuffer = CommonData.GetMemory(nTotalLength+1);
                    }
                    memcpy(pReceiveBuffer, pReceiveTmp, static_cast<size_t>(nReceiveBytes));
                    pReceiveBuffer[nReceiveBytes] = 0;
                    CommonData.FreeMemory(pReceiveTmp);
                    pReceiveTmp = nullptr;

                    bHttpHeader = true;
                    NddsExternalLog(E_DEBUG, "nonndds recv 3-5 :: header complete :: nRetryCount = %d, nTotalLength = %d, nHeaderReceive = %d, httpResult.contentsLength = %d, nReceiveBytes = %d", nRetryCount, nTotalLength, nHeaderReceive, httpResult.contentsLength, nReceiveBytes);
                    if (bHttpHeader && httpResult.contentType != NDDS_BINARY)
                        NddsExternalLog(E_DEBUG, "%s", (char*)pReceiveBuffer);
                }
            }
		}

#ifdef __ECM__
		if (strstr(m_pSessionData->pRequestData, "GET /tmap/geo/reversegeocoding?version=") == m_pSessionData->pRequestData) {      // geocode
			char *endOfHeader = strstr(reinterpret_cast<char*>(pReceiveBuffer), "\r\n\r\n");
			char *startOfBody = strstr(endOfHeader, "{");
			if (strrchr((char*)pReceiveBuffer, '}') != nullptr) {
				*(strrchr((char*)pReceiveBuffer, '}') + 1) = '\0';
				break;
			}
		}
#else
		if (strstr(m_pSessionData->pRequestData, "GET /tmap/geo/reversegeocoding?version=") == m_pSessionData->pRequestData) {      // geocode
			char *endOfHeader = strstr(reinterpret_cast<char*>(pReceiveBuffer), "\r\n\r\n");
			char *startOfBody = strstr(endOfHeader, "{");
			if (nTotalLength + ((startOfBody) - (endOfHeader + 4)) <= (nReceiveBytes - 2)) {
				if (strrchr((char*)pReceiveBuffer, '}') != nullptr) {
					*(strrchr((char*)pReceiveBuffer, '}') + 1) = '\0';
					break;
				}
			}
		}
#endif

        m_pSessionData->nProgress = nProgressStart + nProgressStep * nReceiveBytes / nTotalLength;
        delay(TIME_DELAY);
        CHECK_NDDS_USER_CANCEL(0);
    }


#ifdef __SSL_SOCKET__
    ndds_ssl_close(&ssl_vars);
#endif

    m_pSessionData->nProgress = 82;
    delay(TIME_DELAY);
    CHECK_NDDS_USER_CANCEL(0);

    if (!bHttpHeader || nReceiveBytes == 0)
    {
        FAIL_NDDS_PROTOCOL(NDDSERR_SOCKETRECEIVE_EMPTY, 0);
    }

    //////////////////////////////////////////////////////////////////////////

    //////////////////////////////////////////////////////////////////////////
    // 응답 데이터 파싱

//    DBGPRINT("navi_native", "\n%s\n", pReceiveBuffer);

    if (httpResult.resultCode == 200)
    {
        NDDS_MEMORYSTATUS memory;
        memory.nMemory = nReceiveBytes;
        memory.pMemory = (unsigned char*)malloc(memory.nMemory);
        memory.pUse = memory.pMemory;
        memory.nUse = 0;
        memset(memory.pMemory,0x00,memory.nMemory);

        bRet = ParserNonNdds(m_pSessionData->eProtocolType, ((char*)pReceiveBuffer) + nHeaderReceive, httpResult.contentsLength, &m_pSessionData->pResultStruct, memory);
    }
    else
    {
        bRet = false;
        m_eLastErrCode = NDDSERR_HTTPPROTOCOL;
        m_pSessionData->pResultStruct = (NDDS_COMMON_RES*)malloc(sizeof(NDDS_COMMON_RES));
        memset(m_pSessionData->pResultStruct, 0x00, sizeof(NDDS_COMMON_RES));
        NddsExternalLog(E_DEBUG, "nonNDDS FAIL HTTPPROTOCOL :: %d", httpResult.resultCode);
    }

    if (pReceiveBuffer)
    {
        CommonData.FreeMemory(pReceiveBuffer);
        pReceiveBuffer = nullptr;
    }

    m_pSessionData->nProgress = 100;

    if (lgfp != NULL)
    {
        fflush(lgfp);
        fclose(lgfp);
        lgfp = NULL;
    }

    return bRet;

NDDS_FAIL:

    if (pServiceCloseBuf)
    {
        CNddsLibCommonData &CommonData = CNddsLibCommonData::GetInstance();
        CommonData.FreeMemory((unsigned char*)pServiceCloseBuf);
    }

#ifdef __SSL_SOCKET__
    ndds_ssl_close(&ssl_vars);
#endif

    if (pReceiveBuffer)
    {
        CommonData.FreeMemory(pReceiveBuffer);
        pReceiveBuffer = nullptr;
    }

    if (lgfp != NULL)
    {
        fflush(lgfp);
        fclose(lgfp);
        lgfp = NULL;
    }

    return false;
}

// TNAPP 에러 처리용
bool CNddsLibBase::MakeErrorRes(NDDS_MEMORYSTATUS &memory, NDDS_ERROR_RES **ppRes )
{
    NDDS_ERROR_RES res;
    memset(&res, 0x00, sizeof(NDDS_ERROR_RES));

    *ppRes = (NDDS_ERROR_RES*)malloc(sizeof(NDDS_ERROR_RES));
    memcpy(*ppRes, &res, sizeof(NDDS_ERROR_RES));
    return true;
}

int CNddsLibBase::MakeServiceCloseRequestData(unsigned char **pReqData, char *pServiceID)
{
    CNddsLibCommonData &CommonData = CNddsLibCommonData::GetInstance();

    int nHttpHeader;

    // 요청 데이터 Body 생성
    int nJson,nContentLen, nAllocSize;
    wchar_t *wszJson;
    char *szJson;
    NDDSLIB_Json::Value root;
    NDDSLIB_Json::StyledWriter writer;
    root.clear();

    AddCommonHeader(root, &m_pSessionData->sCommonHeader, true);

    // Json 텍스트 생성 (UTF8)
    std::string output = writer.write( root );
    nJson = output.size();
    if (CommonData.m_eCharacterSet == NDDSCHARACTERSET_EUCKR)
    {
        nAllocSize = sizeof(wchar_t) * (nJson + 1);
        wszJson = (wchar_t *)CommonData.GetMemory(nAllocSize);
        nddsMultiByteToWideChar((char*)output.c_str(), wszJson, nAllocSize);
#ifdef _LINUX
        char *pTemp = (char*)malloc((wcslen(wszJson)+1)*2);
        UCS2toUTF8((unsigned short*)wszJson, wcslen(wszJson), pTemp);
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
    }
    else
    {
        nAllocSize = nJson + 1;
        szJson = (char*)CommonData.GetMemory(nAllocSize);
        strcpy(szJson, (char*)output.c_str());
    }

    nContentLen = strlen(szJson);
    unsigned char *pBuffer = CommonData.GetMemory(nContentLen + 1000);

    //////////////////////////////////////////////////////////////////////////
    // 요청 데이터 Http 해더 생성

    memcpy(m_pHttpHeaderData->serviceID, pServiceID, 15);
    m_pHttpHeaderData->bodySize = nContentLen;
    nHttpHeader = NddsLibHttpHeader::NddsMakeTmapHeader((char*)pBuffer, m_pHttpHeaderData);

    //////////////////////////////////////////////////////////////////////////
    // 요청 데이터(Header+Body) 생성

    memcpy(pBuffer + nHttpHeader, szJson, nContentLen);

    *pReqData = pBuffer;

    CommonData.FreeMemory((unsigned char*)szJson);

    return nHttpHeader+nContentLen;
}

bool CNddsLibBase::ParserResult(NDDS_COMMONHEADER_RES common_header, unsigned char *orgContentData, int nContentSize, NDDS_MEMORYSTATUS &memory, NDDS_JSON_RES **ppRes)
{
    NDDS_JSON_RES res;
    bool bRet;
    unsigned char *pContent = orgContentData;
    memset(&res, 0x00, sizeof(NDDS_JSON_RES));

    if (atoi(common_header.errorCode) == 0)
    {
        try{
            res.nRespLength = nContentSize;
            ALLOC_MEM_COPY(pContent, res.pRespData, nContentSize, memory);
            pContent += nContentSize;
            bRet = true;
        }
        catch (...)
        {
            return false;
        }
    }
    *ppRes = (NDDS_JSON_RES*)malloc(sizeof(NDDS_JSON_RES));
    memcpy(*ppRes, &res, sizeof(NDDS_JSON_RES));

    return bRet;
}
