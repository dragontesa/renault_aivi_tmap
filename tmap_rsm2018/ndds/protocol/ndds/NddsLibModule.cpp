// NddsModule.cpp

#include "NddsLibModule.h"
#include "NddsLibCommonData.h"

CNddsLibModule *g_this_nddslib = NULL;

IMPLEMENT_SINGLETON(CNddsLibModule)

CNddsLibModule::CNddsLibModule(void)
{
	pfnAsyncNddsUpdate = NULL;

	g_this_nddslib = this;
	bTimeOutCheckEnd = false;
	
	CNddsLibCommonData::CreateInstance();

#ifdef _LINUX
	m_pSessionDataMutex = PTHREAD_MUTEX_INITIALIZER;
	m_pNddsDeleteProcMutex = PTHREAD_MUTEX_INITIALIZER;
#else
	m_pSessionDataMutex = NULL;
	m_pNddsDeleteProcMutex = NULL;
	m_pNddsDeleteProcCond = NULL;
#endif
	m_nLastIdxProtocolSession = 0;
}

CNddsLibModule::~CNddsLibModule(void)
{
	bTimeOutCheckEnd = true;

#ifdef _LINUX
	void* status = 0;
	pthread_join(m_pTimeOutThread, (void **)&status);

#ifdef _LINUX
	NddsExternalLog(E_DEBUG, "m_pNddsDeleteProcCond 0");
	pthread_cond_broadcast(&m_pNddsDeleteProcCond);
	pthread_mutex_unlock(&m_pNddsDeleteProcMutex);
#else
	SDL_CondBroadcast(m_pNddsDeleteProcCond);
	SDL_UnlockMutex(m_pNddsDeleteProcMutex);
#endif
	pthread_join(m_pDeleteThread, (void **)&status);

	if (pthread_attr_destroy(&m_threadAttr) != 0)
	{
		DBGPRINT("\n nddslibmodule pthread_attr_destroy failed\n");
	}
#else
	SDL_WaitThread(m_pTimeOutThread, NULL);
	SDL_WaitThread(m_pDeleteThread, NULL);
#endif

	ClearProtocol();

	NDDS_PROTOCOL_SESSION_DATA* pSessionData = NULL;
	std::list<NDDS_PROTOCOL_SESSION_DATA*>::iterator itor;
//	DBGPRINT(_T("######################## CNddsModule::~CNddsModule #2 (%d)\n"), m_listProtocolAction.size());

	while(m_listProtocolAction.size())
	{
		itor = m_listProtocolAction.begin();
		pSessionData = (*itor);
		if(pSessionData->pThread)
		{
//			DBGPRINT(_T("######################## WaitThread #1 (0x%08x)\n"), pSessionData->pThread);
#ifdef _LINUX
			void* status = 0;
			pthread_join(pSessionData->pThread, (void **)&status);
#else
			SDL_WaitThread(pSessionData->pThread, NULL);
#endif
		}
	}

	for(itor = m_listProtocolCancel.begin() ; itor != m_listProtocolCancel.end() ; itor++)
	{
		pSessionData = (*itor);
		DeleteSession(pSessionData);
	}
	m_listProtocolWait.clear();

	for(itor = m_listProtocolDelete.begin() ; itor != m_listProtocolDelete.end() ; itor++)
	{
		pSessionData = (*itor);
		DeleteSession(pSessionData);
	}

#ifdef _LINUX
	pthread_mutex_destroy(&m_pSessionDataMutex);
	pthread_mutex_destroy(&m_pNddsDeleteProcMutex);
	pthread_cond_destroy(&m_pNddsDeleteProcCond);
#else
	if(m_pSessionDataMutex)
	{
		SDL_DestroyMutex(m_pSessionDataMutex);
		m_pSessionDataMutex = NULL;
	}
	if(m_pNddsDeleteProcMutex)
	{
		SDL_DestroyMutex(m_pNddsDeleteProcMutex);
		m_pNddsDeleteProcMutex = NULL;
	}
	if(m_pNddsDeleteProcCond)
	{
		SDL_DestroyCond(m_pNddsDeleteProcCond);
		m_pNddsDeleteProcCond = NULL;
	}
#endif
	CNddsLibCommonData::DestroyInstance();
}

void CNddsLibModule::Initialize(eNddsCharacterSet type, NDDSTMAP_INIT_INFO *pNddsTmapInit)
{
	CNddsLibCommonData &CommonData = CNddsLibCommonData::GetInstance();
#ifdef _LINUX
	if (pthread_mutex_init(&m_pSessionDataMutex, NULL) != 0)
	{
		DBGPRINT("\n mutex init failed\n");
	}
	if (pthread_mutex_init(&m_pNddsDeleteProcMutex, NULL) != 0)
	{
		DBGPRINT("\n mutex init failed\n");
	}
	pthread_cond_init(&m_pNddsDeleteProcCond, NULL);
#else
	m_pSessionDataMutex = SDL_CreateMutex();
	m_pNddsDeleteProcMutex = SDL_CreateMutex();
	m_pNddsDeleteProcCond = SDL_CreateCond();
#endif

	CommonData.m_eCharacterSet = type;

	if(pNddsTmapInit)
		CommonData.SetTmapInit(pNddsTmapInit);

#ifdef _LINUX
	if (pthread_attr_init(&m_threadAttr) != 0)
	{
		DBGPRINT("\n nddslibmodule pthread_attr_init failed\n");
	}

// detach 방식에서 join으로 종료처리하는 방식으로 변경. 2019.10.24 by hheo
//	if (pthread_attr_setdetachstate(&m_threadAttr, PTHREAD_CREATE_DETACHED) != 0)
//	{
//		DBGPRINT("\n nddslibmodule pthread_attr_setdetachstate failed\n");
//	}

	int error;
	error = pthread_create(&m_pTimeOutThread, NULL, &CNddsLibModule::NddsTimeOutProc, NULL);
	error = pthread_create(&m_pDeleteThread, NULL, &CNddsLibModule::NddsDeleteProc, NULL);
#else
	m_pTimeOutThread = SDL_CreateThread(NddsTimeOutProc, "NddsTimeOutProc", NULL);
	m_pDeleteThread = SDL_CreateThread(NddsDeleteProc, "NddsDeleteProc", NULL);
#endif
}

void CNddsLibModule::setTmapHost(const char *pszHostName, const unsigned int nPort)
{
	CNddsLibCommonData &CommonData = CNddsLibCommonData::GetInstance();

	CommonData.setTmapHost(pszHostName, nPort);
}

void CNddsLibModule::SetAuthAccessKey(char *pAccessKey)
{
	CNddsLibCommonData &CommonData = CNddsLibCommonData::GetInstance();

	CommonData.SetAuthAccessKey(pAccessKey);
}

void CNddsLibModule::SetASyncNddsUpdate(LPCALL_ASYNC_NDDSUPDATE_PROC pCallBack)
{
	pfnAsyncNddsUpdate = pCallBack;
}

bool CNddsLibModule::RequestProtocol(eNddsProtocolType eProtocolType, eNddsRequestType eRequestType, NDDS_COMMON_REQ *pRequestData, int nSessionID, eNddsErrorCode *eErrCode)
{
	bool bRet = false;
	int error = 0;

	CNddsLibBase *pNdds = NULL;

	NddsExternalLog(E_DEBUG, "nddslibmodule :: in RequestProtocol 1 :: eProtocolType = %d, eRequestType = %d, nSessionID = %d", eProtocolType, eRequestType, nSessionID);
	if(eProtocolType >= NDDSTYPE_TMAP_MAX)
		return false;

	pNdds = new CNddsLibTmap();

#ifdef _LINUX
	pthread_mutex_lock(&m_pSessionDataMutex);
#else
	SDL_LockMutex(m_pSessionDataMutex);
#endif

    // 이 아래 if문 이하 코드들이 가독성이 너무 떨어짐... 나중에 가독성 좋게 바꾸면 좋을듯...
	NddsExternalLog(E_DEBUG, "nddslibmodule :: in RequestProtocol 2");
	NDDS_PROTOCOL_SESSION_DATA *pSessionData = (NDDS_PROTOCOL_SESSION_DATA*)malloc(sizeof(NDDS_PROTOCOL_SESSION_DATA));
	if(pSessionData)
	{
		//DBGPRINT(_T("######################## CNddsModule::RequestProtocol (0x%08x)\n"), pSessionData);
		NddsExternalLog(E_DEBUG, "nddslibmodule :: in RequestProtocol 2-1");
		memset(pSessionData, 0x00, sizeof(NDDS_PROTOCOL_SESSION_DATA));
		pSessionData->nSessionID = nSessionID;
		pSessionData->eProtocolType = eProtocolType;
		pSessionData->eRequestType = eRequestType;
		pSessionData->eStatus = NDDSSTATUS_WAIT;
		pSessionData->nProgress = 0;
		pSessionData->eErrCode = NDDSERR_UNKNOWN;
		pSessionData->pNdds = pNdds;
		pNdds->Init(pSessionData);

		NddsExternalLog(E_DEBUG, "nddslibmodule :: in RequestProtocol 2-2");
		// Input 데이터를 Json 포멧으로 변경하여 유지한다.
		if(pNdds->MakeRequestData(pRequestData, pSessionData))
		{
			NddsExternalLog(E_DEBUG, "nddslibmodule :: in RequestProtocol 2-3");
			if(eRequestType == NDDSREQUEST_SERIALIZE)
			{
				NddsExternalLog(E_DEBUG, "nddslibmodule :: in RequestProtocol 2-4");
				if(m_listProtocolAction.size() == 0)
				{
					NddsExternalLog(E_DEBUG, "nddslibmodule :: in RequestProtocol 2-5");
					m_listProtocolAction.push_back(pSessionData);
					//통신 스레드 시작
#ifdef _LINUX
					error = pthread_create(&pSessionData->pThread, &m_threadAttr, &CNddsLibModule::NddsProtocolProc, pSessionData);
					NddsExternalLog(E_DEBUG, "nddslibmodule :: in RequestProtocol 2-7 pthread_create :: error = %d, thread addr = %x", error, (void*)pSessionData->pThread);
#else
					pSessionData->pThread = SDL_CreateThread(NddsProtocolProc, "NddsProtocolProc", pSessionData);
#endif
                    //DBGPRINT(_T("######################## CreateThread (0x%08x - 0x%08x)\n"), pSessionData, pSessionData->pThread);
				}else
				{
					NddsExternalLog(E_DEBUG, "nddslibmodule :: in RequestProtocol 2-8");
					m_listProtocolWait.push_back(pSessionData);
				}
			}
			else
			{
				NddsExternalLog(E_DEBUG, "nddslibmodule :: in RequestProtocol 2-9");
				m_listProtocolAction.push_back(pSessionData);
				//통신 스레드 시작
#ifdef _LINUX
				error = pthread_create(&pSessionData->pThread, &m_threadAttr, &CNddsLibModule::NddsProtocolProc, pSessionData);
				NddsExternalLog(E_DEBUG, "nddslibmodule :: in RequestProtocol 2-11 pthread_create :: error = %d, thread addr = %x", error, (void*)pSessionData->pThread);
#else
				pSessionData->pThread = SDL_CreateThread(NddsProtocolProc, "NddsProtocolProc", pSessionData);
#endif
			}

			bRet = true;
			*eErrCode = NDDSERR_SUCCESS;
			//*nSessionID = pSessionData->nSessionID;
			NddsExternalLog(E_DEBUG, "nddslibmodule :: in RequestProtocol 2-12");
		}
		NddsExternalLog(E_DEBUG, "nddslibmodule :: in RequestProtocol 2-13");
	}

	NddsExternalLog(E_DEBUG, "nddslibmodule :: in RequestProtocol 3");
	if(!bRet)
	{
		NddsExternalLog(E_DEBUG, "nddslibmodule :: in RequestProtocol 3-1");
		*eErrCode = NDDSERR_MAKEREQUEST;
#ifdef _LINUX
		NddsExternalLog(E_DEBUG, "nddslibmodule :: in RequestProtocol 3-2");
		pthread_mutex_unlock(&m_pSessionDataMutex);
		NddsExternalLog(E_DEBUG, "nddslibmodule :: in RequestProtocol 3-3");
#else
		SDL_UnlockMutex(m_pSessionDataMutex);
#endif
	}

	// 쓰레드 생성 실패할 경우 예외처리. 2019.10.22 by hheo
	if (error != 0)
	{
		NddsExternalLog(E_DEBUG, "nddslibmodule :: in RequestProtocol 3-4");
		*eErrCode = NDDSERR_THREADCREATE;
#ifdef _LINUX
		NddsExternalLog(E_DEBUG, "nddslibmodule :: in RequestProtocol 3-5");
		pthread_mutex_unlock(&m_pSessionDataMutex);
		NddsExternalLog(E_DEBUG, "nddslibmodule :: in RequestProtocol 3-6");
#else
		SDL_UnlockMutex(m_pSessionDataMutex);
#endif
	}

#ifdef _LINUX
	NddsExternalLog(E_DEBUG, "nddslibmodule :: in RequestProtocol 4");
	pthread_mutex_unlock(&m_pSessionDataMutex);
#else
	SDL_UnlockMutex(m_pSessionDataMutex);
#endif

	NddsExternalLog(E_DEBUG, "nddslibmodule :: in RequestProtocol 5 :: %d", bRet);
	return bRet;
}

bool CNddsLibModule::RequestJson(eNddsRequestType eRequestType, NDDS_COMMON_REQ *pRequestData, int *nSessionID, eNddsErrorCode *eErrCode)
{
	bool bRet = false;
	int error;

	CNddsLibBase *pNdds = NULL;

	pNdds = new CNddsLibTmap();

#ifdef _LINUX
	pthread_mutex_lock(&m_pSessionDataMutex);
#else
	SDL_LockMutex(m_pSessionDataMutex);
#endif

	NDDS_PROTOCOL_SESSION_DATA *pSessionData = (NDDS_PROTOCOL_SESSION_DATA*)malloc(sizeof(NDDS_PROTOCOL_SESSION_DATA));
	if(pSessionData)
	{
		memset(pSessionData, 0x00, sizeof(NDDS_PROTOCOL_SESSION_DATA));
		pSessionData->nSessionID = ++m_nLastIdxProtocolSession;
		pSessionData->eProtocolType = NDDSTYPE_JSON_STRING;
		pSessionData->eRequestType = eRequestType;
		pSessionData->eStatus = NDDSSTATUS_WAIT;
		pSessionData->nProgress = 0;
		pSessionData->eErrCode = NDDSERR_UNKNOWN;
		pSessionData->pNdds = pNdds;

		pNdds->Init(pSessionData);

		// Input 데이터를 Json 포멧으로 변경하여 유지한다.
		if(pNdds->MakeRequestData(pRequestData, pSessionData))
		{
			if(eRequestType == NDDSREQUEST_SERIALIZE)
			{
				if(m_listProtocolAction.size() == 0)
				{
					m_listProtocolAction.push_back(pSessionData);

					//통신 스레드 시작
#ifdef _LINUX
					error = pthread_create(&pSessionData->pThread, &m_threadAttr, &CNddsLibModule::NddsProtocolProc, pSessionData);
#else
					pSessionData->pThread = SDL_CreateThread(NddsProtocolProc, "NddsProtocolProc", pSessionData);
#endif
				}else
				{
					m_listProtocolWait.push_back(pSessionData);
				}
			}
			else
			{
				m_listProtocolAction.push_back(pSessionData);

				//통신 스레드 시작
#ifdef _LINUX
				error = pthread_create(&pSessionData->pThread, &m_threadAttr, &CNddsLibModule::NddsProtocolProc, pSessionData);
#else
				pSessionData->pThread = SDL_CreateThread(NddsProtocolProc, "NddsProtocolProc", pSessionData);
#endif
			}

			bRet = true;
			*eErrCode = NDDSERR_SUCCESS;
			//*nSessionID = pSessionData->nSessionID;
		}
	}

	if(!bRet)
	{
		*eErrCode = NDDSERR_MAKEREQUEST;
#ifdef _LINUX
		pthread_mutex_unlock(&m_pSessionDataMutex);
#else
		SDL_UnlockMutex(m_pSessionDataMutex);
#endif
	}

	// 쓰레드 생성 실패할 경우 예외처리. 2019.10.22 by hheo
	if (error != 0)
	{
		NddsExternalLog(E_DEBUG, "nddslibmodule :: in RequestProtocol 3-4");
		*eErrCode = NDDSERR_THREADCREATE;
#ifdef _LINUX
		NddsExternalLog(E_DEBUG, "nddslibmodule :: in RequestProtocol 3-5");
		pthread_mutex_unlock(&m_pSessionDataMutex);
		NddsExternalLog(E_DEBUG, "nddslibmodule :: in RequestProtocol 3-6");
#else
		SDL_UnlockMutex(m_pSessionDataMutex);
#endif
	}

#ifdef _LINUX
	pthread_mutex_unlock(&m_pSessionDataMutex);
#else
	SDL_UnlockMutex(m_pSessionDataMutex);
#endif

	return bRet;
}

bool CNddsLibModule::GetStatusProtocol(int nSessionID, eNddsStatus *eStatus, int *nProgress, eNddsErrorCode *eErrCode)
{
	bool bRet = false;
	NDDS_PROTOCOL_SESSION_DATA *pSessionData;

#ifdef _LINUX
	pthread_mutex_lock(&m_pSessionDataMutex);
#else
	SDL_LockMutex(m_pSessionDataMutex);
#endif

	std::list<NDDS_PROTOCOL_SESSION_DATA*>::iterator itor;

	for(itor = m_listProtocolAction.begin() ; itor != m_listProtocolAction.end() ; itor++)
	{
		if(nSessionID == (*itor)->nSessionID)
		{
			bRet = true;
			pSessionData = (*itor);

			*eStatus = pSessionData->eStatus;
			*nProgress = pSessionData->nProgress;
			*eErrCode = pSessionData->eErrCode;
			break;
		}
	}

	if(!bRet)
	{
		for(itor = m_listProtocolWait.begin() ; itor != m_listProtocolWait.end() ; itor++)
		{
			if(nSessionID == (*itor)->nSessionID)
			{
				bRet = true;
				pSessionData = (*itor);

				*eStatus = pSessionData->eStatus;
				*nProgress = pSessionData->nProgress;
				*eErrCode = pSessionData->eErrCode;
				break;
			}
		}
	}

	if(!bRet)
	{
		for(itor = m_listProtocolComplete.begin() ; itor != m_listProtocolComplete.end() ; itor++)
		{
			if(nSessionID == (*itor)->nSessionID)
			{
				bRet = true;
				pSessionData = (*itor);

				*eStatus = pSessionData->eStatus;
				*nProgress = pSessionData->nProgress;
				*eErrCode = pSessionData->eErrCode;
				break;
			}
		}
	}

#ifdef _LINUX
	pthread_mutex_unlock(&m_pSessionDataMutex);
#else
	SDL_UnlockMutex(m_pSessionDataMutex);
#endif

	return bRet;
}

bool CNddsLibModule::GetResultProtocol(int nSessionID, eNddsProtocolType *eProtocolType, NDDS_COMMON_RES **ppResultData, eNddsErrorCode *eErrCode)
{
	bool bRet = false;

	CNddsLibModule &NddsInterface = CNddsLibModule::GetInstance();
	NDDS_PROTOCOL_SESSION_DATA *pSessionData;

#ifdef _LINUX
	pthread_mutex_lock(&m_pSessionDataMutex);
#else
	SDL_LockMutex(m_pSessionDataMutex);
#endif

	std::list<NDDS_PROTOCOL_SESSION_DATA*>::iterator itor;

	for(itor = m_listProtocolComplete.begin() ; itor != m_listProtocolComplete.end() ; itor++)
	{
		if(nSessionID == (*itor)->nSessionID)
		{
			bRet = true;
			pSessionData = (*itor);
			*eProtocolType = pSessionData->eProtocolType;
			*eErrCode = pSessionData->eErrCode;
			*ppResultData = pSessionData->pResultStruct;

			pSessionData->pResultStruct = NULL; //UI에서 해당 메모리는 해제한다.
			NddsExternalLog(E_DEBUG, "nddslibmodule :: push DeleteSession before 1");
			m_listProtocolDelete.push_back(pSessionData);
			NddsExternalLog(E_DEBUG, "nddslibmodule :: push DeleteSession after 1");
			m_listProtocolComplete.erase(itor);
			break;
		}
	}

#ifdef _LINUX
	pthread_mutex_unlock(&m_pSessionDataMutex);
#else
	SDL_UnlockMutex(m_pSessionDataMutex);
#endif

	if (m_listProtocolDelete.size() > 0) {
#ifdef _LINUX
		NddsExternalLog(E_DEBUG, "m_pNddsDeleteProcCond 1");
		pthread_cond_broadcast(&m_pNddsDeleteProcCond);
		pthread_mutex_unlock(&m_pNddsDeleteProcMutex);
#else
		SDL_CondBroadcast(m_pNddsDeleteProcCond);
		SDL_UnlockMutex(m_pNddsDeleteProcMutex);
#endif
	}

	return bRet;
}

void CNddsLibModule::RemoveActionProtocol(int nSessionID)
{
	NDDS_PROTOCOL_SESSION_DATA *pSessionData;

// 이 함수는 CheckRequestSession 함수 안에서만 호출되고 있는데, 이미 CheckRequestSession을 호출하기 전에 Lock을 걸었으므로 여기서는 Lock을 하면 안 된다. 2019.11.2 by hheo
//#ifdef _LINUX
//	pthread_mutex_lock(&m_pSessionDataMutex);
//#else
//	SDL_LockMutex(m_pSessionDataMutex);
//#endif

	std::list<NDDS_PROTOCOL_SESSION_DATA*>::iterator itor;

	for(itor = m_listProtocolAction.begin() ; itor != m_listProtocolAction.end() ; itor++)
	{
		if(nSessionID == (*itor)->nSessionID)
		{
			pSessionData = (*itor);
			NddsExternalLog(E_DEBUG, "nddslibmodule :: push DeleteSession before 2");
			m_listProtocolDelete.push_back(pSessionData);
			NddsExternalLog(E_DEBUG, "nddslibmodule :: push DeleteSession after 2");
			m_listProtocolAction.erase(itor);
			break;
		}
	}

// 이 함수는 CheckRequestSession 함수 안에서만 호출되고 있는데, 이미 CheckRequestSession을 호출하기 전에 Lock을 걸었으므로 여기서는 Lock을 하면 안 된다. 2019.11.2 by hheo
//#ifdef _LINUX
//	pthread_mutex_unlock(&m_pSessionDataMutex);
//#else
//	SDL_UnlockMutex(m_pSessionDataMutex);
//#endif


	// 현재 진행중인 세션이 없고 대기중인 세션이 있으면 동작시킨다.
	CheckRequestSession();
}

bool CNddsLibModule::CancelProtocol(int nSessionID)
{
	bool bRet = false;
	NDDS_PROTOCOL_SESSION_DATA *pSessionData;

#ifdef _LINUX
	pthread_mutex_lock(&m_pSessionDataMutex);
#else
	SDL_LockMutex(m_pSessionDataMutex);
#endif

	std::list<NDDS_PROTOCOL_SESSION_DATA*>::iterator itor;

	if(nSessionID > -1)
	{
		for(itor = m_listProtocolAction.begin() ; itor != m_listProtocolAction.end() ; itor++)
		{
			if(nSessionID == (*itor)->nSessionID)
			{
				bRet = true;
				pSessionData = (*itor);

				pSessionData->eStatus = NDDSSTATUS_REQUESTCANCEL;
				break;
			}
		}

		if(!bRet)
		{
			for(itor = m_listProtocolWait.begin() ; itor != m_listProtocolWait.end() ; itor++)
			{
				if(nSessionID == (*itor)->nSessionID)
				{
					bRet = true;
					pSessionData = (*itor);

					NddsExternalLog(E_DEBUG, "nddslibmodule :: push DeleteSession before 3");
					m_listProtocolDelete.push_back(pSessionData);
					NddsExternalLog(E_DEBUG, "nddslibmodule :: push DeleteSession after 3");
					m_listProtocolWait.erase(itor);
					break;
				}
			}
		}
	}
	else
	{
		for(itor = m_listProtocolAction.begin() ; itor != m_listProtocolAction.end() ; itor++)
		{
			pSessionData = (*itor);
			pSessionData->eStatus = NDDSSTATUS_REQUESTCANCEL;
		}

		for(itor = m_listProtocolWait.begin() ; itor != m_listProtocolWait.end() ; itor++)
		{
			pSessionData = (*itor);
			NddsExternalLog(E_DEBUG, "nddslibmodule :: push DeleteSession before 4");
			m_listProtocolDelete.push_back(pSessionData);
			NddsExternalLog(E_DEBUG, "nddslibmodule :: push DeleteSession after 4");
			m_listProtocolWait.erase(itor);
		}
		bRet = true;
	}

	// 현재 진행중인 세션이 없고 대기중인 세션이 있으면 동작시킨다.
	CheckRequestSession();

#ifdef _LINUX
	pthread_mutex_unlock(&m_pSessionDataMutex);
#else
	SDL_UnlockMutex(m_pSessionDataMutex);
#endif

	return bRet;
}

void CNddsLibModule::ClearProtocol()
{
	NDDS_PROTOCOL_SESSION_DATA *pSessionData;

#ifdef _LINUX
	pthread_mutex_lock(&m_pSessionDataMutex);
#else
	SDL_LockMutex(m_pSessionDataMutex);
#endif

	std::list<NDDS_PROTOCOL_SESSION_DATA*>::iterator itor;

	for(itor = m_listProtocolWait.begin() ; itor != m_listProtocolWait.end() ; itor++)
	{
		pSessionData = (*itor);
		NddsExternalLog(E_DEBUG, "nddslibmodule :: push DeleteSession before 5");
		m_listProtocolDelete.push_back(pSessionData);
		NddsExternalLog(E_DEBUG, "nddslibmodule :: push DeleteSession after 5");
	}
	m_listProtocolWait.clear();

	for(itor = m_listProtocolAction.begin() ; itor != m_listProtocolAction.end() ; itor++)
	{
		pSessionData = (*itor);
		pSessionData->eStatus = NDDSSTATUS_REQUESTCANCEL;
	}

	for(itor = m_listProtocolComplete.begin() ; itor != m_listProtocolComplete.end() ; itor++)
	{
		pSessionData = (*itor);
		NddsExternalLog(E_DEBUG, "nddslibmodule :: push DeleteSession before 6");
		m_listProtocolDelete.push_back(pSessionData);
		NddsExternalLog(E_DEBUG, "nddslibmodule :: push DeleteSession after 6");
	}
	m_listProtocolComplete.clear();

#ifdef _LINUX
	pthread_mutex_unlock(&m_pSessionDataMutex);
#else
	SDL_UnlockMutex(m_pSessionDataMutex);
#endif

	if (m_listProtocolDelete.size() > 0) {
#ifdef _LINUX
		NddsExternalLog(E_DEBUG, "m_pNddsDeleteProcCond 2");
		pthread_cond_broadcast(&m_pNddsDeleteProcCond);
		pthread_mutex_unlock(&m_pNddsDeleteProcMutex);
#else
		SDL_CondBroadcast(m_pNddsDeleteProcCond);
		SDL_UnlockMutex(m_pNddsDeleteProcMutex);
#endif
	}
}

#ifdef _LINUX
void* CNddsLibModule::NddsProtocolProc(void *lParam)
#else
int CNddsLibModule::NddsProtocolProc(void *lParam)
#endif
{
	NddsExternalLog(E_DEBUG, "nddslibmodule :: in NddsProtocolProc 1");
	CNddsLibCommonData &CommonData = CNddsLibCommonData::GetInstance();
	NDDS_PROTOCOL_SESSION_DATA *pSessionData = (NDDS_PROTOCOL_SESSION_DATA*)lParam;

	pSessionData->nReqTime = GetTickCount();

	CNddsLibBase *pNdds = pSessionData->pNdds;
	
	eNddsProtocolType eProtocolType = pSessionData->eProtocolType;
	int nSessionID = pSessionData->nSessionID;
	eNddsRequestType eRequestType = pSessionData->eRequestType ;
	eNddsErrorCode eErrCode = NDDSERR_UNKNOWN;
	pSessionData->eStatus = NDDSSTATUS_COMMUNICATION;

	NddsExternalLog(E_DEBUG, "nddslibmodule :: in NddsProtocolProc 2");
	bool bRequestSucceeded = false;
	// 통신 요청 및 처리
	if(eProtocolType < NDDSTYPE_TMAP_NONNDDS)
	{
		NddsExternalLog(E_DEBUG, "nddslibmodule :: in NddsProtocolProc RequestServer before");
		bRequestSucceeded = pNdds->RequestServer();
		NddsExternalLog(E_DEBUG, "nddslibmodule :: in NddsProtocolProc RequestServer after");
	}
	else
	{
		NddsExternalLog(E_DEBUG, "nddslibmodule :: in NddsProtocolProc RequestNonNddsServer before");
		bRequestSucceeded = pNdds->RequestNonNddsServer();
		NddsExternalLog(E_DEBUG, "nddslibmodule :: in NddsProtocolProc RequestNonNddsServer after");
	}

	NddsExternalLog(E_DEBUG, "nddslibmodule :: in NddsProtocolProc 3");
	if(bRequestSucceeded == true)
	{
		NddsExternalLog(E_DEBUG, "nddslibmodule :: in NddsProtocolProc 3-1");
		pSessionData->eErrCode = NDDSERR_SUCCESS;;
	}
	else
	{
		NddsExternalLog(E_DEBUG, "nddslibmodule :: in NddsProtocolProc 3-2");
		pSessionData->eErrCode = pNdds->GetLastErrorCode();
	}

	eErrCode = pSessionData->eErrCode;

	NddsExternalLog(E_DEBUG, "nddslibmodule :: in NddsProtocolProc 4 :: pSessionData->eStatus = %d", pSessionData->eStatus);
// CompletedProtocol에서 이 곳으로 이동시킴. 2019.10.31. by hheo
#ifdef _LINUX
	pthread_mutex_lock(&g_this_nddslib->m_pSessionDataMutex);
#else
	SDL_LockMutex(g_this_nddslib->m_pSessionDataMutex);
#endif
	if(pSessionData->eStatus == NDDSSTATUS_COMMUNICATION)
		pSessionData->eStatus = NDDSSTATUS_COMPLETE;
	else
		pSessionData->eStatus = NDDSSTATUS_CANCELED;
	NddsExternalLog(E_DEBUG, "nddslibmodule :: in NddsProtocolProc 5 :: pSessionData->eStatus = %d", pSessionData->eStatus);

	// 완료 세션 처리
	NddsExternalLog(E_DEBUG, "nddslibmodule :: in NddsProtocolProc 6");
	g_this_nddslib->CompletedProtocol(pSessionData->nSessionID);
	NddsExternalLog(E_DEBUG, "nddslibmodule :: in NddsProtocolProc 7");

	// 현재 진행중인 세션이 없고 대기중인 세션이 있으면 동작시킨다.
	NddsExternalLog(E_DEBUG, "nddslibmodule :: in NddsProtocolProc 8");
	g_this_nddslib->CheckRequestSession();

	NddsExternalLog(E_DEBUG, "nddslibmodule :: in NddsProtocolProc 9");
// CompletedProtocol에서 이 곳으로 이동시킴. 2019.10.31. by hheo
#ifdef _LINUX
	pthread_mutex_unlock(&g_this_nddslib->m_pSessionDataMutex);
#else
	SDL_UnlockMutex(g_this_nddslib->m_pSessionDataMutex);
#endif

	NddsExternalLog(E_DEBUG, "nddslibmodule :: in NddsProtocolProc 10");
	if(eRequestType ==  NDDSREQUEST_ATONCE)
	{
		NddsExternalLog(E_DEBUG, "nddslibmodule :: in NddsProtocolProc 10-1");
		if(g_this_nddslib->pfnAsyncNddsUpdate)
		{
			NddsExternalLog(E_DEBUG, "nddslibmodule :: in NddsProtocolProc 10-2");
			//DBGPRINT(_T("NddsProtocolProc ===========> eProtocolType=%d, nSessionID=%d, eErrCode=%d\n"), eProtocolType, nSessionID, eErrCode);
			g_this_nddslib->pfnAsyncNddsUpdate(eProtocolType, nSessionID, eErrCode);
		}
	}

	NddsExternalLog(E_DEBUG, "nddslibmodule :: in NddsProtocolProc 11 :: finish");
	return 0;
}

void CNddsLibModule::CompletedProtocol(int nSessionID)
{
	bool bFind = false;
	NDDS_PROTOCOL_SESSION_DATA* pSessionData = NULL;

// CompletedProtocol을 호출하기 전으로 이동시킴. 2019.10.31. by hheo
//#ifdef _LINUX
//	pthread_mutex_lock(&m_pSessionDataMutex);
//#else
//	SDL_LockMutex(m_pSessionDataMutex);
//#endif

	std::list<NDDS_PROTOCOL_SESSION_DATA*>::iterator itor;

	// 해당 통신 세션을 찾아 완료 리스트에 넣는다.
	for(itor = m_listProtocolAction.begin() ; itor != m_listProtocolAction.end() ; itor++)
	{
		if(nSessionID == (*itor)->nSessionID)
		{
			bFind = true;

			pSessionData = (*itor);
			m_listProtocolAction.erase(itor);

			if(pSessionData->eStatus == NDDSSTATUS_COMPLETE)
			{
				m_listProtocolComplete.push_back(pSessionData);
			}else
			{
				NddsExternalLog(E_DEBUG, "nddslibmodule :: push DeleteSession before 7");
				m_listProtocolDelete.push_back(pSessionData);
				NddsExternalLog(E_DEBUG, "nddslibmodule :: push DeleteSession after 7");
			}
			break;
		}
	}

// CompletedProtocol을 호출한 이후로 이동시킴. 2019.10.31. by hheo
//#ifdef _LINUX
//	pthread_mutex_unlock(&m_pSessionDataMutex);
//#else
//	SDL_UnlockMutex(m_pSessionDataMutex);
//#endif
}

// 이 함수는 반드시 m_pSessionDataMutex의 Lock ~ Unlock 사이에서 호출되어야 한다. 2019.11.2 by hheo
void CNddsLibModule::CheckRequestSession()
{
	NDDS_PROTOCOL_SESSION_DATA* pSessionData = NULL;
	int error = 0;

	NddsExternalLog(E_DEBUG, "nddslibmodule :: in CheckRequestSession 1");
// 이미 CheckRequestSession을 호출하기 전에 Lock을 걸었으므로 여기서는 Lock을 하면 안 된다. 2019.11.2 by hheo
//#ifdef _LINUX
//	pthread_mutex_lock(&m_pSessionDataMutex);
//#else
//	SDL_LockMutex(m_pSessionDataMutex);
//#endif

	std::list<NDDS_PROTOCOL_SESSION_DATA*>::iterator itor;

	NddsExternalLog(E_DEBUG, "nddslibmodule :: in CheckRequestSession 2 :: action_size = %d, wait_size = %d", m_listProtocolAction.size(), m_listProtocolWait.size());
	if(m_listProtocolAction.size() == 0 && m_listProtocolWait.size() > 0)
	{
		NddsExternalLog(E_DEBUG, "nddslibmodule :: in CheckRequestSession 2-1");
		itor = m_listProtocolWait.begin();
		pSessionData = (*itor);
		m_listProtocolWait.erase(itor);
		m_listProtocolAction.push_back(pSessionData);

		//통신 스레드 시작
#ifdef _LINUX
		error = pthread_create(&pSessionData->pThread, &m_threadAttr, &CNddsLibModule::NddsProtocolProc, pSessionData);
		NddsExternalLog(E_DEBUG, "nddslibmodule :: in CheckRequestSession 2-3 pthread_create :: error = %d, thread addr = %x", error, (void*)pSessionData->pThread);

		// 쓰레드 생성 실패할 경우 예외처리. 2019.10.22 by hheo
		if (error != 0)
		{
			NddsExternalLog(E_DEBUG, "nddslibmodule :: in CheckRequestSession 2-4");
			pSessionData->eStatus = NDDSSTATUS_CANCELED;
			NddsExternalLog(E_DEBUG, "nddslibmodule :: in CheckRequestSession 2-5");
			RemoveActionProtocol(pSessionData->nSessionID);
			NddsExternalLog(E_DEBUG, "nddslibmodule :: in CheckRequestSession 2-6 :: pthread_create failed and finish");
			return;
		}
#else
		pSessionData->pThread = SDL_CreateThread(NddsProtocolProc, "NddsProtocolProc", pSessionData);
#endif
		//DBGPRINT(_T("######################## CreateThread (0x%08x - 0x%08x)\n"), pSessionData, pSessionData->pThread);
	}

	NddsExternalLog(E_DEBUG, "nddslibmodule :: in CheckRequestSession 3");
	if (m_listProtocolDelete.size() > 0) {
#ifdef _LINUX
		NddsExternalLog(E_DEBUG, "m_pNddsDeleteProcCond 3");
		pthread_cond_broadcast(&m_pNddsDeleteProcCond);
		pthread_mutex_unlock(&m_pNddsDeleteProcMutex);
#else
		SDL_CondBroadcast(m_pNddsDeleteProcCond);
		SDL_UnlockMutex(m_pNddsDeleteProcMutex);
#endif
	}

	NddsExternalLog(E_DEBUG, "nddslibmodule :: in CheckRequestSession 4 :: finish");

// 이미 CheckRequestSession을 호출하기 전에 Lock을 걸었으므로 여기서는 Lock을 하면 안 된다. 2019.11.2 by hheo
//#ifdef _LINUX
//	pthread_mutex_unlock(&m_pSessionDataMutex);
//#else
//	SDL_UnlockMutex(m_pSessionDataMutex);
//#endif
}

bool CNddsLibModule::DeleteSession(NDDS_PROTOCOL_SESSION_DATA *pSessionData)
{
//	DBGPRINT(_T("######################## CNddsModule::DeleteSession #1 (0x%08x)\n"), pSessionData);
	if(pSessionData == NULL)
		return true;

	int thread_result = 0, join_result = 0;
	NddsExternalLog(E_DEBUG, "nddslibmodule :: DeleteSession pthread_join :: thread addr = %x", (void*)pSessionData->pThread);
	if (pSessionData->pThread != NULL)
		join_result = pthread_join(pSessionData->pThread, (void**)thread_result);
	NddsExternalLog(E_DEBUG, "nddslibmodule :: DeleteSession pthread_join :: result = %d", join_result);

	if (join_result != 0 && join_result != ESRCH) {
		NddsExternalLog(E_DEBUG, "nddslibmodule :: DeleteSession join failed");
		return false;
	}

	if(pSessionData->pRequestData)
	{
		free(pSessionData->pRequestData);
		pSessionData->pRequestData = NULL;
	}

	if(pSessionData->pResultStruct)
	{
		if(pSessionData->pResultStruct->memory.pMemory)
		{
			free(pSessionData->pResultStruct->memory.pMemory);
			pSessionData->pResultStruct->memory.pMemory = NULL;
		}
		free(pSessionData->pResultStruct);
		pSessionData->pResultStruct = NULL;
	}

	if(pSessionData->pNdds)
	{
		delete pSessionData->pNdds;
		pSessionData->pNdds = NULL;
	}
//	DBGPRINT(_T("######################## CNddsModule::DeleteSession #2\n"));

	free(pSessionData);
	return true;
}

void CNddsLibModule::SetTmapSoqInfo(char* pInfo)
{
	CNddsLibCommonData &CommonData = CNddsLibCommonData::GetInstance();
	CommonData.SetTmapTigSoqInfo(pInfo);
}

void CNddsLibModule::SetReqDataSave(bool bLogSave, char* pszPath)
{
	CNddsLibCommonData &CommonData = CNddsLibCommonData::GetInstance();
	CommonData.m_bLogSave = bLogSave;
	if(pszPath != NULL)
		strcpy(CommonData.m_szLogPath, pszPath);
}

bool CNddsLibModule::TimeOutCheck()
{
	NDDS_PROTOCOL_SESSION_DATA* pSessionData = NULL;

#ifdef _LINUX
	pthread_mutex_lock(&m_pSessionDataMutex);
#else
	SDL_LockMutex(m_pSessionDataMutex);
#endif

	if(m_listProtocolAction.size() > 0)
	{
		unsigned long nTick = GetTickCount();

		std::list<NDDS_PROTOCOL_SESSION_DATA*>::iterator itor;

		// 해당 통신 세션을 찾아 완료 리스트에 넣는다.
		for(itor = m_listProtocolAction.begin() ; itor != m_listProtocolAction.end() ; itor++)
		{
			pSessionData = (*itor);
			if (pSessionData == nullptr)
				continue;

			if(((nTick - pSessionData->nReqTime)  > 10000) && (pSessionData->eRequestType == NDDSREQUEST_ATONCE))
			{
				pSessionData->eStatus = NDDSSTATUS_REQUESTTIMEOUT;
			}
		}

	}
	
#ifdef _LINUX
	pthread_mutex_unlock(&m_pSessionDataMutex);
#else
	SDL_UnlockMutex(m_pSessionDataMutex);
#endif

	if(bTimeOutCheckEnd)
		return false;

	return true;
}

#ifdef _LINUX
void* CNddsLibModule::NddsTimeOutProc(void *lParam)
#else
int CNddsLibModule::NddsTimeOutProc(void *lParam)
#endif
{
	while(1)
	{
		if(g_this_nddslib->TimeOutCheck() == false)
			break;

		delay(1000);
	}


	return 0;
}

void CNddsLibModule::DeleteSessionList()
{
#ifdef _LINUX
	pthread_mutex_lock(&m_pSessionDataMutex);
#else
	SDL_LockMutex(m_pSessionDataMutex);
#endif
	unsigned long nTick = GetTickCount();

	NddsExternalLog(E_DEBUG, "nddslibmodule :: DeleteSessionList 2 :: delete_size = %d", m_listProtocolDelete.size());
	NDDS_PROTOCOL_SESSION_DATA* pSession;
	NddsProtocolSessionDataList::iterator itr = m_listProtocolDelete.begin();
	while(itr != m_listProtocolDelete.end())
	{
		pSession = *itr;
		NddsExternalLog(E_DEBUG, "nddslibmodule :: DeleteSessionList 2-1 :: pSession addr = %x", (void*)pSession);
		if (pSession == NULL)
			continue;
		NddsExternalLog(E_DEBUG, "nddslibmodule :: DeleteSessionList 2-2 :: pThread addr = %x", (void*)pSession->pThread);
		if (DeleteSession(pSession) == true) {
			NddsExternalLog(E_DEBUG, "nddslibmodule :: DeleteSessionList 2-3");
			m_listProtocolDelete.erase(itr++);
		} else {
			NddsExternalLog(E_DEBUG, "nddslibmodule :: DeleteSessionList 2-4");
			++itr;
		}
		NddsExternalLog(E_DEBUG, "nddslibmodule :: DeleteSessionList 2-5");
	}

#ifdef _LINUX
	pthread_mutex_unlock(&m_pSessionDataMutex);
#else
	SDL_UnlockMutex(m_pSessionDataMutex);
#endif
	NddsExternalLog(E_DEBUG, "nddslibmodule :: DeleteSessionList 3 :: finish");
}

#ifdef _LINUX
void* CNddsLibModule::NddsDeleteProc(void *lParam)
#else
int CNddsLibModule::NddsDeleteProc(void *lParam)
#endif
{
	while(1)
	{
#ifdef _LINUX
		pthread_cond_wait(&g_this_nddslib->m_pNddsDeleteProcCond, &g_this_nddslib->m_pNddsDeleteProcMutex);
#else
		SDL_CondWait(pNddsModule->m_pNddsDeleteProcCond, pNddsModule->m_pNddsDeleteProcMutex);
#endif

		g_this_nddslib->DeleteSessionList();
		if (g_this_nddslib->bTimeOutCheckEnd == true)
			break;

		delay(1000);
	}

	return 0;
}
