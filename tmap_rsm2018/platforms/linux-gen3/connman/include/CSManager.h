#ifndef CSMANAGER_H
#define CSMANAGER_H

#include <glib.h>
#include <arpa/inet.h>
#include <dlt/dlt.h>
    


// DLT default Macro
//#define _DEBUG_TRACE(ctx,format,args...) do { \
//    char log_buff[1024]; \
//    char final_string[1024]; \
//    snprintf(final_string, sizeof(final_string),"File:%s, LINE:%d, ",__FILE__, __LINE__); \
//    snprintf(log_buff, sizeof(log_buff), format,##args); \
//    strncat(final_string, log_buff, sizeof(final_string)); \
//    DLT_LOG(ctx, DLT_LOG_INFO, DLT_STRING(final_string)); \
//} while(0)

#define INF_TRC_CSM(format)			_INF_TRC_CSM format

#define _INF_TRC_CSM(format,args...) do { \
    char log_buff[1024]; \
    char final_string[1024]; \
    snprintf(final_string, sizeof(final_string),"FILE: %s,LINE:%d ",basename(__FILE__) ,__LINE__); \
    snprintf(log_buff, sizeof(log_buff), format,##args); \
    strncat(final_string, log_buff, sizeof(final_string)); \
    DLT_LOG(CSMG_CTX, DLT_LOG_INFO, DLT_STRING(final_string)); \
} while(0)



// DLT Macro
#define DBG_TRC_CSM(format)			_DBG_TRC_CSM format

#define _DBG_TRC_CSM(format,args...) do { \
    char log_buff[1024]; \
    char final_string[1024]; \
    snprintf(final_string, sizeof(final_string), "FILE: %s,LINE:%d " ,basename(__FILE__) ,__LINE__); \
    snprintf(log_buff, sizeof(log_buff), format,##args); \
    strncat(final_string, log_buff, sizeof(final_string)); \
    DLT_LOG(CSMG_CTX, DLT_LOG_DEBUG, DLT_STRING(final_string)); \
} while(0)



  

//forward declaration on manager_context. opaque to users
typedef struct csmanager_manager_context_struct  csmanager_manager_context;

//forward declaraion of session_context. opaque to users
typedef struct csmanager_session_context_struct csmanager_session_context;


typedef enum {
  CSMANAGER_UNINITIALIZED   = -4,      //csmanager_init has not been called
  CSMANAGER_INITIALIZED     = -3,     //initialized but not manager seen till now
  CSMANAGER_FAILURE         = -2,    //serious error occured. check log
  CSMANAGER_NOMANAGER       = -1,   //connman has not been found on bus or disappeared
  CSMANAGER_MANAGER_CONNECTED = 0  //let's go connman is there 
} csmanager_manager_state;

typedef enum  {
  CSMANAGER_SESSION_DESTROYED       = -1,   //manager disappeared and session has been destructed. Free this with csmanager_session_free.
  CSMANAGER_SESSION_DISCONNECTED    =  0,   //all right, connman is there but we did not create a session
  CSMANAGER_SESSION_CONNECTING      =  1,   //session created but not online / connected
  CSMANAGER_SESSION_CONNECTED       =  2,   // great
  CSMANAGER_SESSION_ONLINE          =  3,   // even greater
  CSMANAGER_SESSION_LOW_PRIO_BEARER =  4,   // there is a better allowed_bearer available. You may issue a reconnect. Currently this check is not implemeted
  } csmanager_session_state;

//callback function pointers for session and manager  state change. You will need methods with same signature for callbacks
typedef int (* csmanager_session_state_change_cb_func_t) (csmanager_session_context *context, csmanager_session_state state, gchar *ContextIdentifier, void *user_data);


typedef int (* csmanager_manager_state_change_cb_func_t) (csmanager_manager_context *context, csmanager_manager_state state, void *user_data);

//get manager context object
csmanager_manager_context* csmanager_new(gchar* agent_name);

//do basic initialization (dbus-connection, manager proxy),fp csmanager_session_state_change_cb_func_t will be called on statechange
gboolean  csmanager_init(csmanager_manager_context *context, csmanager_manager_state_change_cb_func_t cb,  void *user_data );

//create session for ContextIdentifier,  csmanager_session_state_change_cb_func_t will be called on statechange.
//ContextIdentifiert should be empty string "" or actually used ContextIdentifier string
//force_bind_ip might be used to force a certain ip which the socket will be bound to instead of relying on connman
//this should be used as long as connmanproxy dosen't serve this data
                            
csmanager_session_context*  csmanager_session_create(csmanager_manager_context *context, csmanager_session_state_change_cb_func_t cb, void *user_data, const gchar *ContextIdentifier, gchar *force_bind_ip);
//destroy session. frees connman session and notification
void csmanager_session_destroy(csmanager_session_context *context);
//free destroyed session
gboolean csmanager_session_free(csmanager_session_context *context);
//reconnect session to get better bearer if available. this will be signaled by csmanager_session_state with value CSMANAGER_LOW_PRIO_BEARER (currently not implemented
void csmanager_session_reconnect(csmanager_session_context *context);
//destroy session, free manager
void csmanager_free(csmanager_manager_context *context);
gboolean csmanager_session_is_online(csmanager_session_context *context);
GVariant* csmanager_session_get_settings(csmanager_session_context *context);
// get datagram or stream socket. this can also be used with e.g curl
int csmanager_session_get_bound_socketv4(csmanager_session_context *context,const uint port, gboolean datagram);
//return gateway interface ip of type long which is used in most socket implementations. this may be used e.g. to bind a socket to this address
gchar* csmanager_session_get_interface_ip(csmanager_session_context *context);
gchar* csmanager_session_get_bearer(csmanager_session_context *context);
GPtrArray* csmanager_session_get_allowed_bearers(csmanager_session_context *context); 
csmanager_session_state csmanager_session_get_state(csmanager_session_context *context);
csmanager_manager_state csmanager_manager_get_state(csmanager_manager_context *context);
#endif
