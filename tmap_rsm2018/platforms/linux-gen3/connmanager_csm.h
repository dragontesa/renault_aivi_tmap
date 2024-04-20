#ifndef TMAP_RSM2018_PLATFORMS_LINUX_GEN3_CONNMANAGER_CSM_H_
#define TMAP_RSM2018_PLATFORMS_LINUX_GEN3_CONNMANAGER_CSM_H_
#if defined(BUILD_APP) && !defined(TMAP_CONNMAN_PROXY_API)
#include <QObject>
#ifdef __cplusplus
extern "C" {
#include "connman/include/CSManager.h"
}
#endif

namespace SKT {

class ConnManager : public QObject
{
    Q_OBJECT

    Q_PROPERTY(State state READ state NOTIFY stateChanged)

public:
    enum State {
    	Destroyied=-1,
        Offline = 0,
        Idle,
        Ready,
        Online,
        Reserved,
    };

    ConnManager(QObject *parent = 0);
    ~ConnManager();

    void init();
    State state() const { return m_state; }

signals:
    void stateChanged(ConnManager::State state);

private:
    static int onSessionStateUpdated(csmanager_session_context *context, 
    								csmanager_session_state state,
    	  							gchar *ContextIdentifier,
    	   							void *user_data);
    static int onManagerStateUpdate( csmanager_manager_context *context,
    										csmanager_manager_state state,
    										void *user_data);

    void updateState(csmanager_session_state state);
private:
    State m_state;
    static csmanager_session_context* m_ConnmanSessionContext;
    static csmanager_manager_context* m_ConnmanManagerContext;
    static ConnManager* connmanInstance;
};

}

#endif

#endif // TMAP_RSM2018_PLATFORMS_LINUX_GEN3_CONNMANAGER_CSM_H_
