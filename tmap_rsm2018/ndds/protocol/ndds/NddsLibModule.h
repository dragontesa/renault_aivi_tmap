// NddsModule.h
#ifndef __NDDSLIB_MODULE_H__
#define __NDDSLIB_MODULE_H__

#include "NddsLibConst.h"
#include "NddsLibTmap.h"
#include <list>
#ifdef _WIN32
#include "SDL.h"
#else
#include <pthread.h>
#endif
typedef std::list<NDDS_PROTOCOL_SESSION_DATA*> NddsProtocolSessionDataList;

class CNddsLibModule
{
	NOT_COPYABLE(CNddsLibModule)

private:
	CNddsLibModule(void);
	virtual ~CNddsLibModule(void);

	//////////////////////////////////////////////////////////////////////////
	// 인터페이스
	//////////////////////////////////////////////////////////////////////////
public:

	/// 통신 모듈을 초기화 한다.
	/**
		@param pNddsTmapInit		[IN] Ndds Tmap 통신 기본 정보 
		@return	
	**/
	void Initialize(eNddsCharacterSet type, NDDSTMAP_INIT_INFO *pNddsTmapInit);

    /// NDDS 서버 Hostname을 변경한다.
    /**
        @param pszHostname		[IN] 서버 Hostname
        @param nPort			[IN] 서버 Port
        @return					에러코드(성공:DE_ERROR_SUCCESS, 이외 실패)
    **/
    void setTmapHost(const char *pszHostName, const unsigned int nPort);

	/// AccessKey를 변경한다
	/**
		@param pAccessKey		[IN] 변경할 Accesskey 
		@return	
	**/
	void SetAuthAccessKey(char *pAccessKey);

	/// 통신을 요청한다. (비동기)
	/**
		@param eProtocolType	[IN] 요청 프로토콜 타입
		@param eRequestType		[IN] 요청 방식 (현재 지원하지 않음)
		@param pRequestData		[IN] 요청 데이터
		@param nSessionID		[IN] 할당된 세션 아이디 (성공일때)
		@param eErrCode			[OUT] 에러코드
		@return					성공 유/무
	**/
	bool RequestProtocol(eNddsProtocolType eProtocolType, eNddsRequestType eRequestType, NDDS_COMMON_REQ *pRequestData, int nSessionID, eNddsErrorCode *eErrCode);

	/// 통신을 요청한다.(by Json String) (비동기)
	/**
		@param eRequestType		[IN] 요청 방식 (현재 지원하지 않음)
		@param pRequestData		[IN] 요청 json 데이터
		@param nSessionID		[OUT] 할당된 세션 아이디 (성공일때)
		@param eErrCode			[OUT] 에러코드
		@return					성공 유/무
	**/
	bool RequestJson(eNddsRequestType eRequestType, NDDS_COMMON_REQ *pRequestData, int *nSessionID, eNddsErrorCode *eErrCode);

	/// 통신 상태를 얻는다.
	/**
		@param nSessionID		[IN] 세션 아이디
		@param eStatus			[OUT] 진행 상태
		@param nProgress		[OUT] 진행률 (%)
		@param eErrCode			[OUT] 에러코드 (통신 완료 상태 일때만 의미 있음)
		@return					성공 유/무
	**/
	bool GetStatusProtocol(int nSessionID, eNddsStatus *eStatus, int *nProgress, eNddsErrorCode *eErrCode);

	/// 통신 결과값을 얻는다.
	/**
		@param nSessionID		[IN] 세션 아이디
		@param eServiceType		[OUT] 프로토콜 타입
		@param ppResultData		[OUT] 통신 결과 구조체 (결과 수신쪽에서 메모리 해제 필요)
		@param eErrCode			[OUT] 에러코드 (통신 완료 상태 일때만 의미 있음)
		@return					성공 유/무
	**/
	bool GetResultProtocol(int nSessionID, eNddsProtocolType *eServiceType, NDDS_COMMON_RES **ppResultData, eNddsErrorCode *eErrCode);

	/// 통신 요청을 취소한다.
	/**
		@param nSessionID		[IN] 세션 아이디
		@return					성공 유/무
	**/
	bool CancelProtocol(int nSessionID);

	/// 모든 통신을 초기화 한다.(취소)
	/**
		@return
	**/
	void ClearProtocol();

	/// T map TIG 품질 정보를 설정한다.
	/**
		@return
	**/
	void SetTmapSoqInfo(char* pInfo);

    void SetReqDataSave(bool bLogSave, char* pszPath);
	//////////////////////////////////////////////////////////////////////////
	// 내부 정의
	//////////////////////////////////////////////////////////////////////////
public:
	// 통신 요청 스레드(Send, Receive, Parsing)
#ifdef _LINUX
	static void* NddsProtocolProc(void *lParam);
#else
	static int NddsProtocolProc(void *lParam);
#endif
	// 통신 완료 처리(Session 관리)
	void CompletedProtocol(int nSessionID);
	// 요청 세션이 있는지 체크하여 스레드 실행
	void CheckRequestSession();
	// 세션을 삭제 한다.
    bool DeleteSession(NDDS_PROTOCOL_SESSION_DATA *pSessionData);
	void DeleteSessionList();

	void RemoveActionProtocol(int nSessionID);

	LPCALL_ASYNC_NDDSUPDATE_PROC	pfnAsyncNddsUpdate;	

	void SetASyncNddsUpdate(LPCALL_ASYNC_NDDSUPDATE_PROC pCallBack);

#ifdef _LINUX
	pthread_t				m_pTimeOutThread;			// 스레드
	pthread_t               m_pDeleteThread;            // Delete Session Thread
#else
	SDL_Thread*				m_pTimeOutThread;			// 스레드
	SDL_Thread*             m_pDeleteThread;            // Delete Session Thread
#endif

#ifdef _LINUX
	static void* NddsTimeOutProc(void *lParam);
#else
	static int NddsTimeOutProc(void *lParam);
#endif

#ifdef _LINUX
	static void* NddsDeleteProc(void *lParam);
#else
	static int NddsDeleteProc(void *lParam);
#endif

	bool TimeOutCheck();

	bool bTimeOutCheckEnd;
private:
#ifdef _LINUX
	pthread_attr_t				m_threadAttr;			// pthread attribute
	pthread_mutex_t 			m_pSessionDataMutex;
	pthread_mutex_t				m_pNddsDeleteProcMutex;		//스레드 동기화
	pthread_cond_t				m_pNddsDeleteProcCond;
#else
	SDL_mutex*					m_pSessionDataMutex;
	SDL_mutex*					m_pNddsDeleteProcMutex;		//스레드 동기화
	SDL_cond*					m_pNddsDeleteProcCond;
#endif
	NddsProtocolSessionDataList		m_listProtocolWait;
	NddsProtocolSessionDataList		m_listProtocolAction;
	NddsProtocolSessionDataList		m_listProtocolComplete;
	NddsProtocolSessionDataList		m_listProtocolCancel;
	NddsProtocolSessionDataList     m_listProtocolDelete;
	int						m_nLastIdxProtocolSession;

	DECLARE_SINGLETON(CNddsLibModule)
};

#endif // __NDDSLIB_MODULE_H__
