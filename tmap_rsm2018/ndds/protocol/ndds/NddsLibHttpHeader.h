// NddsLibHttpHeader.h
#ifndef __NDDSLIB_HTTP_HEADER_H__
#define __NDDSLIB_HTTP_HEADER_H__

#define NDDS_HTTP_HEADER				"POST http://trs.nate.com:7005/TRS?SERVICE_ID=61001610016000A HTTP/1.1\r\n"
#define NDDS_HTTP_LENGTH				"Content-Length: %d \r\n\r\n"
//#define NDDS_HTTP_USERAGENT          "User-Agent: iPhoneOS/4.3\r\n"
#define NDDS_HTTP_USERAGENT 			"User-Agent: %.3s63SS005030%.3s%.3s221116%.8s;0;0;0;0;0&&&login&\r\n"
#define NDDS_HTTP_TIG_MESSAGE	   	"TIG-Message"
#define NDDS_HTTP_SERVICE_INFO		"TIG_Service_Info"

#define NDDS_START_SERVICE_ID		"61001610016000A"
#define NDDS_HTTP_SERVICEID_LEN		16
#define NDDS_HTTP_ACCESSKEY_LEN		50
#define NDDS_HTTP_MIN_LEN			13
#define NDDS_MAX_HTTP_BUF_SIZE		1024

#define MINE_TYPE_JSON			"application/json"
#define MINE_TYPE_XML			"application/xml"
#define MINE_TYPE_BINARY		"application/octet-stream"

typedef enum tagNddsMineType{
	NDDS_JSON,
	NDDS_XML,
	NDDS_BINARY
}NDDS_MINE_TYPE;

typedef struct tagNddsLibHttpHeaderData
{
	char	uri[512];
	char	tigQoSInfo[512];
	char 	serviceID[NDDS_HTTP_SERVICEID_LEN];
	char 	MIN[NDDS_HTTP_MIN_LEN];
	short 	width;
	short 	height;
	NDDS_MINE_TYPE mineType;
	int 	bodySize;
	char    accessKey[50];
    char    nonce[50];
    char    cih[50];
} NDDS_HTTP_HEADERDATA;

typedef struct tagNddsLibHttpResultData
{
	int 	resultCode;
	int 	TIG_Service_Info;
	int 	contentsLength;
	NDDS_MINE_TYPE contentType;
	char 	serviceID[NDDS_HTTP_SERVICEID_LEN];
	char 	accessKey[NDDS_HTTP_ACCESSKEY_LEN];
	char 	*TigMsg;
} NDDS_HTTP_RESULTDATA;


class NddsLibHttpHeader {
public:
	NddsLibHttpHeader();
	virtual ~NddsLibHttpHeader();

	static int NddsMakeTmapHeader(char *, NDDS_HTTP_HEADERDATA*, bool bAuth = false );
	static int NddsParserHeader(char *,int ,NDDS_HTTP_RESULTDATA * );

private:
	static int NddsGetServiceID(char *,char *,int);
	static int NddsGetResponseCode(char *);
    static int NddsGetcontentLength(char *httpbuf,int );
    static int NddsFindContentsLen(char *pbuf,int);
    static int NddsFindContentsLen2(char *pbuf, int isize);
	static bool	NddsMakeUserAagent(char *, NDDS_HTTP_HEADERDATA *);
	static int NddsGetMedthodeField(const char *, int ,const char *, char *,int );
	static NDDS_MINE_TYPE NddsFindContentType(char *pbuf,int isize);
	static int NddsGetAccessKey(char *,char *,int);
};

#endif /* __NDDSLIB_HTTP_HEADER_H__ */
