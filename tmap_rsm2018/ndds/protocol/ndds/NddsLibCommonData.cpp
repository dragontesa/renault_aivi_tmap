// NddsCommonData.cpp

#include "NddsLibCommonData.h"

IMPLEMENT_SINGLETON(CNddsLibCommonData)

CNddsLibCommonData::CNddsLibCommonData(void)
{
	*m_szTmapHost = NULL;
	m_nTmapPort = 0;
    m_bLogSave = false;
    memset(&m_szLogPath,0x00,sizeof(m_szLogPath));
	memset(&m_sTmapHttpHeader,0x00,sizeof(NDDS_HTTP_HEADERDATA));
	memset(&m_sTmapNddsCommonHeader,0x00,sizeof(NDDS_COMMONHEADER_REQ));

	m_eCharacterSet = NDDSCHARACTERSET_UTF8;

	// 메모리풀 생성
    std::size_t size = 1500000;
	m_pMemoryPool = new CNddsMemoryPool(size);
}

CNddsLibCommonData::~CNddsLibCommonData(void)
{
	if(m_pMemoryPool)
	{
		delete m_pMemoryPool;
		m_pMemoryPool = NULL;
	}
}

void CNddsLibCommonData::SetTmapInit(NDDSTMAP_INIT_INFO *pInit)
{
	switch(pInit->nddsServerType)
	{
	case NDDSSERVER_COMMERCE:
		strcpy(m_szTmapHost, NDDSLIBCONST::NDDS_TMAP_HOST_COMMERCE_1);
		m_nTmapPort = NDDSLIBCONST::NDDS_TMAP_PORT;
		break;
    case NDDSSERVER_DEVELOPMENT:
        strcpy(m_szTmapHost, NDDSLIBCONST::NDDS_TMAP_HOST_DEVELOPMENT);
        m_nTmapPort = NDDSLIBCONST::NDDS_TMAP_PORT;
        break;
    case NDDSSERVER_NONE:
        strcpy(m_szTmapHost, pInit->szServerURL);
        m_nTmapPort = pInit->nServerPort;
        m_bUseDefaultPort = (strstr(m_szTmapHost, VNEXT_DOMAIN) != nullptr);
        break;
	default:
		strcpy(m_szTmapHost, NDDSLIBCONST::NDDS_TMAP_HOST_DEVELOPMENT);
		m_nTmapPort = NDDSLIBCONST::NDDS_TMAP_PORT;
		break;
	}

	strcpy(m_sTmapHttpHeader.serviceID, NDDS_START_SERVICE_ID);
	strncpy(m_sTmapHttpHeader.MIN, pInit->phoneNumber,NDDS_HTTP_MIN_LEN-1);
	m_sTmapHttpHeader.width = pInit->width;
	m_sTmapHttpHeader.height = pInit->height;
	m_sTmapHttpHeader.tigQoSInfo[0] = NULL;
	strcpy(m_sTmapHttpHeader.accessKey, pInit->accessKey);
	// NDDS 공통 헤더
	/*
	0x47 : NDDS T map
	0x48 : NDDS K/U Tmap
	0x90 : NDDS Tmap for SmartCar-KIA
	0x91 : NDDS Tmap for SmartCar-JLR
	0x92 : NDDS Tmap for SmartCar-SCAR
	0x93 : NDDS Tmap for SmartCar-RSM
	0x94 : NDDS Tmap for SmartCar-FORD
	0x60 : NDDS Tmap for TMAP_B2B
	*/
	m_sTmapNddsCommonHeader.svcType = 0x93; // ndds 5.0
	strcpy(m_sTmapNddsCommonHeader.osType, pInit->osType);
	strcpy(m_sTmapNddsCommonHeader.osVersion, pInit->osVersion);
	strcpy(m_sTmapNddsCommonHeader.modelNo, pInit->modelNo);
	strcpy(m_sTmapNddsCommonHeader.appVersion, pInit->appVersion);
	strcpy(m_sTmapNddsCommonHeader.buildNo,	pInit->buildNo);
	strcpy(m_sTmapNddsCommonHeader.resolution, pInit->resolution);
	strcpy(m_sTmapNddsCommonHeader.usingInfo, pInit->usingInfo);
	strcpy(m_sTmapNddsCommonHeader.macAddress, pInit->macAddress);
	m_sTmapNddsCommonHeader.screenWidth = pInit->width;
	m_sTmapNddsCommonHeader.screenHeight = pInit->height;
	strcpy(m_sTmapNddsCommonHeader.deviceId, pInit->deviceId);
	strcpy(m_sTmapNddsCommonHeader.carrier, pInit->carrier);
	strcpy(m_sTmapNddsCommonHeader.deviceIdHu, pInit->deviceIdHu);
	strcpy(m_sTmapNddsCommonHeader.deviceModleName, pInit->deviceModleName);
	strcpy(m_sTmapNddsCommonHeader.deviceModelNo, pInit->deviceModelNo);
	strcpy(m_sTmapNddsCommonHeader.vendor, pInit->vendor);
	strcpy(m_sTmapNddsCommonHeader.serialKey, pInit->serialKey);
}

void CNddsLibCommonData::setTmapHost(const char *pszHostName, const unsigned int nPort)
{
    strcpy(m_szTmapHost, pszHostName);
    m_nTmapPort = nPort;
    m_bUseDefaultPort = (strstr(m_szTmapHost, VNEXT_DOMAIN) != nullptr);
}

void CNddsLibCommonData::SetAuthAccessKey(char *pAccessKey)
{
	strcpy(m_sTmapHttpHeader.accessKey, pAccessKey);
}

void CNddsLibCommonData::SetTmapTigSoqInfo(char *pInfo)
{
	if(pInfo != NULL)
	{
		memset(m_sTmapHttpHeader.tigQoSInfo,0x00,sizeof(m_sTmapHttpHeader.tigQoSInfo));
		strcpy(m_sTmapHttpHeader.tigQoSInfo, pInfo);
	}
}
