/*
 * NddsLibHttpHeader.cpp
 */

#include "NddsLibHttpHeader.h"
#include "NddsLibCommonData.h"
#include "ndds_base64.h"
#include <time.h>
#include <string.h>

#ifndef MIN
#define MIN(a, b)			(((a) < (b)) ? (a) : (b))
#endif

NddsLibHttpHeader::NddsLibHttpHeader()
{
	// TODO Auto-generated constructor stub
}

NddsLibHttpHeader::~NddsLibHttpHeader()
{
}

int  NddsLibHttpHeader::NddsMakeTmapHeader(char *httpBuf, NDDS_HTTP_HEADERDATA *info, bool bAuth)
{
	CNddsLibCommonData &CommonData = CNddsLibCommonData::GetInstance();

	int nPos = 0;

	nPos += sprintf(httpBuf + nPos, "POST %s HTTP/1.1\r\n", info->uri);
    if (CommonData.m_bUseDefaultPort)
        nPos += sprintf(httpBuf + nPos, "Host: %s\r\n", CommonData.m_szTmapHost);
    else
        nPos += sprintf(httpBuf + nPos, "Host: %s:%d\r\n", CommonData.m_szTmapHost, CommonData.m_nTmapPort);
	nPos += sprintf(httpBuf + nPos, "Connection: close\r\n");

	nPos += sprintf(httpBuf + nPos, "Content-Type: %s\r\n", MINE_TYPE_JSON);
	if(info->mineType == NDDS_XML)
		nPos += sprintf(httpBuf + nPos,"Accept: %s; charset=utf-8\r\n", MINE_TYPE_XML);
	else if(info->mineType == NDDS_BINARY)
		nPos += sprintf(httpBuf + nPos,"Accept: %s; charset=utf-8\r\n", MINE_TYPE_BINARY);
	else
		nPos += sprintf(httpBuf + nPos,"Accept: %s; charset=utf-8\r\n", MINE_TYPE_JSON);

	nPos += sprintf(httpBuf + nPos, "Requester: CLIENT_SSL\r\n");

	nPos += sprintf(httpBuf + nPos, "AccessKey: %s\r\n", info->accessKey);

	time_t  ltime;
	struct tm *today; 
	char reqTime[15]={0,};

	time(&ltime);
#ifdef __MM2018__
	ltime += 9 * 60 * 60;       // Bosch System Firmware에서 localtime을 UTC로 링크해놨기 때문에, 우리가 알아서 서울 타임존을 적용해야 함. 2019.12.1 by hheo
#endif
	today = localtime(&ltime);
	sprintf(reqTime,"%04d%02d%02d%02d%02d%02d",
		today->tm_year + 1900,  // tm_year는 1900을 더해야 서기 연도가 됨
		today->tm_mon + 1,       // tm_mon은 1월이 0, 2월이 1... 식으로 되어 있음
		today->tm_mday,
		today->tm_hour,
		today->tm_min,
		today->tm_sec);

	nPos += sprintf(httpBuf + nPos,"Client_ReqTime: %s\r\n", reqTime);
	
	if((info->accessKey[0]==NULL) && (bAuth == false))
		nPos += sprintf(httpBuf + nPos,"Client_MDN: %s\r\n", "01000000000");
	
    if((info->nonce[0]==NULL) && (bAuth == false))
        nPos += sprintf(httpBuf + nPos, "Nonce: %s\r\n", info->nonce);

    if((info->cih[0]==NULL) && (bAuth == false))
        nPos += sprintf(httpBuf + nPos, "CIH: %s\r\n", info->cih);

	//	nPos += sprintf(httpBuf + nPos, "DEV_CODE: %s\r\n", "D001");	// test page
	//	nPos += sprintf(httpBuf + nPos, "DEV_CODE: %s\r\n", "D002");	// fitnesse test
	//	nPos += sprintf(httpBuf + nPos, "DEV_CODE: %s\r\n", "D003");	// 모니터링용
	//	nPos += sprintf(httpBuf + nPos, "DEV_CODE: %s\r\n", "D004");	// 기타 test
	nPos += sprintf(httpBuf + nPos, "Content-Length: %d \r\n\r\n", info->bodySize);

	return strlen(httpBuf);
}

int  NddsLibHttpHeader::NddsParserHeader(char *httpBuf, int nHttpHeder, NDDS_HTTP_RESULTDATA* resultData)
{
	char *ptrPos = httpBuf;
	char  *szTIGBase64Msg;
	char  servceInfo[2];

	assert(nHttpHeder > 0);

    if(nHttpHeder)
    {
		resultData->resultCode = NddsGetResponseCode(httpBuf);
		resultData->contentsLength = NddsGetcontentLength(httpBuf,nHttpHeder);
		// Content-Type
		resultData->contentType = NddsFindContentType(httpBuf,nHttpHeder);
		// AccessKey
		NddsGetAccessKey(resultData->accessKey,httpBuf,nHttpHeder);
		// SERVICE ID
		NddsGetServiceID(resultData->serviceID,httpBuf,NDDS_MAX_HTTP_BUF_SIZE);

		// TIG MESSAGE
		szTIGBase64Msg = (char *) malloc(nHttpHeder);
		resultData->TigMsg = (char *) malloc(nHttpHeder);
		memset(szTIGBase64Msg,0x00,nHttpHeder);
		memset(resultData->TigMsg,0x00,nHttpHeder);

		NddsGetMedthodeField(httpBuf,nHttpHeder,NDDS_HTTP_TIG_MESSAGE,szTIGBase64Msg,nHttpHeder);
		CNddsBase64::NddsDecode_Base64(szTIGBase64Msg+strlen("MSG="),resultData->TigMsg,nHttpHeder);
		
		free(szTIGBase64Msg);

		// SERVICE INFO
		NddsGetMedthodeField(httpBuf,nHttpHeder,NDDS_HTTP_SERVICE_INFO,servceInfo,2);
		resultData->TIG_Service_Info = atoi(servceInfo);

    }

	return nHttpHeder;
}


int NddsLibHttpHeader::NddsGetResponseCode(char *pHttpHeaderData)
{
    int len;
    char *token;
    char httpresp[]="HTTP/";

    len = strlen(httpresp);
    if(strncmp(pHttpHeaderData,httpresp,len)!=0)
        return 0;

    token = pHttpHeaderData;

    while(*token!=0x20)  // space skip
        token++;

   token++;
   return atoi(token);
}

char *strnstr(const char *haystack, const char *needle, size_t len)
{
	int i;
	size_t needle_len;

	/* segfault here if needle is not NULL terminated */
	if (0 == (needle_len = strlen(needle)))
		return (char *)haystack;

	for (i=0; i<=(int)(len-needle_len); i++)
	{
		if ((haystack[0] == needle[0]) &&
			(0 == strncmp(haystack, needle, needle_len)))
			return (char *)haystack;

		haystack++;
	}
	return 0;
}

int NddsLibHttpHeader::NddsGetServiceID(char *output,char *httpbuf,int lenHttpBuf)
{
	int ret=-1;
	char *pServiceID;
	char strServiceID[]="SERVICEID=";

	pServiceID= strnstr(httpbuf,strServiceID,lenHttpBuf);
	if(pServiceID != NULL)
	{
		pServiceID += strlen(strServiceID);
		strncpy(output,pServiceID,15);	
		ret = 1;
	}
	return ret;
}


int NddsLibHttpHeader::NddsGetcontentLength(char *httpbuf,int size)
{
   int ret=-1;

   if(httpbuf==NULL)
      goto EXIT;

   ret=NddsFindContentsLen(httpbuf,size);

   if (ret == 0) {
       ret = NddsFindContentsLen2(httpbuf, size);
   }

EXIT:
   return ret;
}


int NddsLibHttpHeader::NddsFindContentsLen(char *pbuf,int isize)
{

   char *pContentLenStart;
   int iContentLenStart;
   char contentLenSTR[]="Content-Length:";

   pContentLenStart= strnstr(pbuf,contentLenSTR,isize);
   if(pContentLenStart==NULL)
      return 0;

   iContentLenStart=(pContentLenStart-pbuf)+ (sizeof(contentLenSTR)-1);   //strlen(contentLenSTR)

   return atoi(&pbuf[iContentLenStart]);
}

int NddsLibHttpHeader::NddsFindContentsLen2(char *pbuf,int isize)
{
    int length = strlen(pbuf);
    unsigned char slen = static_cast<unsigned char>(strstr(&pbuf[isize], "\r\n") - &pbuf[isize]);

    int nResult = 0;
    unsigned char result[8] = {0};
    int tmp;
    for (int ii = slen + isize - 1, jj = 0; ii >= isize; ii--, jj++) {
        if ('0' <= pbuf[ii] && pbuf[ii] <= '9') {
            tmp = pbuf[ii] - 0x30;
        } else if ('a' <= pbuf[ii] && pbuf[ii] <= 'f') {
            tmp = pbuf[ii] - 'a' + 10;
        } else {
            return 0;
        }

        nResult |= (tmp & 0xf) << (jj * 4);
    }

    return nResult;
}
NDDS_MINE_TYPE NddsLibHttpHeader::NddsFindContentType(char *pbuf,int isize)
{
	char *pContentTypeStart;
	int lenContentType=0;
	char contentType[]="Content-Type:";

	pContentTypeStart= strnstr(pbuf,contentType,isize);
	if(pContentTypeStart==NULL)
		return NDDS_JSON;

	pContentTypeStart+=sizeof(contentType)-1;   //strlen(contentLenSTR)

	while(*pContentTypeStart==0x20)  // space skip
		pContentTypeStart++;
	
	char *typeBinary = MINE_TYPE_BINARY;
	int len = strlen(typeBinary);
	int val = strncmp(typeBinary,pContentTypeStart,len);
	if(val == 0)
		return NDDS_BINARY;
	else
		return NDDS_JSON;

}

int NddsLibHttpHeader::NddsGetAccessKey(char *output,char *httpbuf,int lenHttpBuf)
{
	int ret=-1;
	int nLen = 0;
	char *pAccessKeyStart = NULL;
	char *pAccessKeyEnd = NULL;
	char strAccessKey[]="AccessKey: ";

	pAccessKeyStart= strnstr(httpbuf,strAccessKey,lenHttpBuf);
	if(pAccessKeyStart != NULL)
	{
		pAccessKeyStart += strlen(strAccessKey);

		pAccessKeyEnd = pAccessKeyStart;
		while(*pAccessKeyEnd!='\r')
			pAccessKeyEnd++;

		int nSize = pAccessKeyEnd - pAccessKeyStart;
		if(nSize >= 41)
		{
			strncpy(output,pAccessKeyStart,nSize);	
			ret = 1;
		}
	}


	return ret;
}

bool NddsLibHttpHeader::NddsMakeUserAagent(char *useragent, NDDS_HTTP_HEADERDATA * info)
{
	char teleCom[4]={0x00,};
	char teleComName[4] ={0x00,};
	char phonenumber[8] ={0x30,};
	char szdeviceWidth[4] ={0x00,};
	char szdeviceHeight[4] ={0x00,};
	int		deviceWidth;
	int		deviceHeight;

	memset(useragent,0x00,sizeof(useragent));

	strncpy(teleCom,info->MIN,3);
	if(strlen(info->MIN) == 10)
		strncpy(phonenumber+1,info->MIN+3,7);
	else
		strncpy(phonenumber,info->MIN+3,8);

	if (strcmp(teleCom,"010") == 0)
		strcpy(teleComName,"010");
	else if (strcmp(teleCom,"011") == 0)
		strcpy(teleComName,"SKT");
	else if (strcmp(teleCom,"016") == 0)
		strcpy(teleComName,"KTF");
	else if (strcmp(teleCom,"017") == 0)
		strcpy(teleComName,"STI");
	else if (strcmp(teleCom,"018") == 0)
		strcpy(teleComName,"HSP");
	else if (strcmp(teleCom,"019") == 0)
		strcpy(teleComName,"LGT");
	else if (strcmp(teleCom,"IRM") == 0)
		strcpy(teleComName,"IXX");
	else if (strcmp(teleCom,"989") == 0)
		strcpy(teleComName,"989");
	else
		strcpy(teleComName,"");

	if(info->width < info->height)
	{
		deviceWidth = info->width % 1000;
		deviceHeight = info->height % 1000;
	}
	else
	{
		deviceWidth = info->height % 1000;
		deviceHeight = info->width % 1000;
	}
	sprintf(szdeviceWidth,"%03d",deviceWidth);
	sprintf(szdeviceHeight,"%03d",deviceHeight);

//    sprintf(useragent,HTTP_USERAGENT);
	sprintf(useragent,NDDS_HTTP_USERAGENT,teleComName,szdeviceWidth,szdeviceHeight,phonenumber);

	return true;
}


int NddsLibHttpHeader::NddsGetMedthodeField(const char *phttpHeader, int headerLen,const char *Method, char *OutProperty,int iOutSize)
{
   int	 szTargetlen,MethodeLen;
   char *token;

   char  lhttp[NDDS_MAX_HTTP_BUF_SIZE];
   char seps[]   = "\r\n";
   char secondseps[]   = ": \r\n";
   int bufsize=MIN(headerLen,NDDS_MAX_HTTP_BUF_SIZE);


   memset(lhttp,0x00,NDDS_MAX_HTTP_BUF_SIZE);
   memcpy(lhttp,phttpHeader,bufsize);
   OutProperty[0]='\0';
   OutProperty[iOutSize-1]='\0';


   lhttp[bufsize-1]=0;
   szTargetlen=strlen(Method);

   token = strtok( lhttp, seps);
   while( token != NULL )
   {
      MethodeLen=strlen(token);

      if(strncmp(token,Method,szTargetlen)==0)
      {
         goto FIND_METHOD;
      }

      token = strtok( NULL, seps );

   }
   token=NULL;
   goto END;

FIND_METHOD:


   if((token+szTargetlen-lhttp)>=bufsize-1)
      goto END;
   token+=szTargetlen;
   token++;             //skip ':'
   while(*token==' ')
      token++;          //skip ' '

    strncpy(OutProperty,token,iOutSize); OutProperty[iOutSize-1]=0;
   return true;
END:
   return false; 		//atoi(token);
}


