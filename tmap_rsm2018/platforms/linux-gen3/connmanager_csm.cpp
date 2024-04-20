#if defined(BUILD_APP) && !defined(TMAP_CONNMAN_PROXY_API)
#include "connmanager_csm.h"
#include <QDebug>

// #define CONTAINER_IP "10.19.229.142"
#define CONTAINER_IP "10.19.229.146"
#define CONTEXT_ID "any" //"connman.tmap.csmctx"
#define AGENT_NAME  "lxc_extnav"

const QString connmanAlias = QStringLiteral("net.connman: ");
#define connmanDebug(msg) qDebug() << connmanAlias << msg;
// #define connmanDebug(msg)

namespace SKT {

csmanager_session_context* ConnManager::m_ConnmanSessionContext = nullptr;
csmanager_manager_context* ConnManager::m_ConnmanManagerContext = nullptr;
ConnManager* ConnManager::connmanInstance = nullptr;

DLT_DECLARE_CONTEXT(CSMG_CTX)

// DLT info level macro
#define INF_TRACE(format)     _INF_TRC_CSM format


// DLT debug level macro
#define DBG_TRACE(format)     _DBG_TRC_CSM format


ConnManager::ConnManager(QObject *parent)
: QObject(parent)
, m_state(State::Offline)
{
	connmanInstance = this;
}

ConnManager::~ConnManager()
{
	if (m_ConnmanSessionContext)
		csmanager_session_destroy(m_ConnmanSessionContext);
	if (m_ConnmanManagerContext)
		csmanager_free(m_ConnmanManagerContext);

    DLT_UNREGISTER_CONTEXT(CSMG_CTX);
}

void ConnManager::init()
{
  DLT_REGISTER_CONTEXT(CSMG_CTX, "ENAV", "CSManager Tmap context for Logging");       
	m_ConnmanManagerContext = csmanager_new(AGENT_NAME);
	connmanDebug(QStringLiteral("init manager context created:%1").arg((quintptr)m_ConnmanManagerContext,-1,16,QChar('0')));
	csmanager_init(m_ConnmanManagerContext,  &ConnManager::onManagerStateUpdate ,nullptr);
}

//this will be called on session state change (connected, online, disconnected)
int ConnManager::onSessionStateUpdated(csmanager_session_context *context,
									   csmanager_session_state state,
									   gchar *ContextIdentifier, 
									   void *user_data)
{
  INF_TRACE(("got new session state: %i", state));
  connmanDebug(QStringLiteral("got new session state -> %1").arg(state));
  if ( state == CSMANAGER_SESSION_CONNECTING ) {
    INF_TRACE(("session connectiong ..."));
    connmanDebug(QStringLiteral("Session connecting"));
  }
  else if (state >= CSMANAGER_SESSION_CONNECTED) {
    INF_TRACE(("session connected ..."));
    INF_TRACE(("bearer %s" , csmanager_session_get_bearer(context)));
    INF_TRACE(("bind ip %s" , csmanager_session_get_interface_ip(context)));
    connmanDebug(QStringLiteral("session connected and being online"));
    connmanDebug(QStringLiteral("bearer %1").arg(csmanager_session_get_bearer(context)));
    connmanDebug(QStringLiteral("bind ip %1").arg(csmanager_session_get_interface_ip(context)));
  }
  else {
    INF_TRACE(("session is not connected!!"));
  	connmanDebug(QStringLiteral("session is not connected !"))
  }
 
  connmanInstance->updateState(state);
}


//this will be called on manager state change (connman appears, vanishes, ..)
int ConnManager::onManagerStateUpdate(csmanager_manager_context *context,
											 csmanager_manager_state state,
											 void *user_data)
{
  INF_TRACE(("got new manager state: %i [context:%p]", state,context));
  connmanDebug(QStringLiteral("got new manager state -> %1 [context:%2]").arg(state).arg((quintptr)context,-1,16,QChar('0')));
  if (state == CSMANAGER_MANAGER_CONNECTED){
    //manager available. let's create session
    INF_TRACE(("start create a session ..."));
    m_ConnmanSessionContext = csmanager_session_create(m_ConnmanManagerContext, &ConnManager::onSessionStateUpdated, NULL, CONTEXT_ID,  CONTAINER_IP);
    INF_TRACE(("end create session: %p", m_ConnmanSessionContext));
  }

}

void ConnManager::updateState(csmanager_session_state state)
{
  INF_TRACE(("update state %i", state));
  connmanDebug(QStringLiteral("update state %1").arg(state));
  m_state = (State)state;

	emit stateChanged((State)state);
}

}

#endif
