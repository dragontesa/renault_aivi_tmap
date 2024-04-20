// ndds_base64.h
#ifndef __NDDS_BASE64_H__
#define __NDDS_BASE64_H__

class CNddsBase64{
public:
	CNddsBase64();
	virtual ~CNddsBase64();

	static int NddsEncode_Base64(unsigned char *pData, int nDataSize, char *szEncode, int nEncodeBuf);
	static int NddsDecode_Base64(char *base64 , char *szOutMsg,int iOutMsgSize);
	static int NddsDecode_Base64(char *base64 , unsigned char *byOutData,int iOutDataSize);
};

#endif /* __NDDS_BASE64_H__ */
