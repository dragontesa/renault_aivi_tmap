/*
 * agwaudiomanagerclient.h
 *
 *  Created on: Oct 12, 2017
 *      Author: man1szh
 */

#ifndef AI_THIRDPARTY_NAVI_PRODUCTS_NAVIGATION_EXT_COMMON_DEMO_EXTERNALTHIRDPARTYNAVIGATIONAPP_SOURCE_VOICE_AGW_AUDIOMANAGER_CLIENT_H_
#define AI_THIRDPARTY_NAVI_PRODUCTS_NAVIGATION_EXT_COMMON_DEMO_EXTERNALTHIRDPARTYNAVIGATIONAPP_SOURCE_VOICE_AGW_AUDIOMANAGER_CLIENT_H_

#include <stdio.h>
#include <string.h>
#include <vector>
#include <iostream>
#include <glib.h>    // For threading and main event loop functionalities
#include <stdlib.h>
#include <dlt/dlt.h>
#include <QObject>
#include <QMap>
#include <AudioManagerInterface.h>
/*****************************************************************
| includes
|----------------------------------------------------------------*/
//#include "AGWAudioManager_FI.h"
#include "ThirdPartyVoiceIF.h"

namespace third{
namespace party{
namespace voice{


#define AGW_AUDIOMANAGER_NAME "com.bosch.AutomotiveProxy"
#define AGW_AUDIOMANAGER_PATH "/com/bosch/AutomotiveProxy/AudioManager"


/*
 *
 */
class agw_audiomanager_client
{
public:
   agw_audiomanager_client ( ThirdPartyVoiceIF* pThirdPartyVoiceIF );
   virtual ~agw_audiomanager_client ( );
    void initAudioManager();
   /*---handler for signals from server----*/

//   static gpointer pfEventThread(gpointer pvArg);

//   static void vAudioRouteRequestCallback(GObject *poSourceObject, GAsyncResult *poResult, gpointer pUserData);
//   static void vSourceAvailabilityCallback(GObject *poSourceObject, GAsyncResult *poResult, gpointer pUserData);
//   static void vMuteStateCallback(GObject *poSourceObject, GAsyncResult *poResult, gpointer pUserData);
//   static void vSourceActivityResultCallback(GObject *poSourceObject, GAsyncResult *poResult, gpointer pUserData);

   void vAudioRouteRequest(guchar source, guint16 state_required) const;
   void vSourceAvailability(guchar source, gboolean bAvailability) const;
   void vMuteState(guchar source, guint16 state_required) const;
   void vSourceActivityResult(guchar source, guint16 status) const;

private slots:
    void onAudioManagerRegistered();
    void onAllocate(uchar source, const QString &sink);
    void onDeAllocate(uchar source);
    void onMuteState(uchar source, uchar mute_state);
    void onSourceActivity(uchar source, ushort status);


private:

   static ThirdPartyVoiceIF* m_pThirdPartyVoiceIF;
   //gdbus identifier
   unsigned int uWatch;

   // Pointer to the main context
   GMainContext *m_poContext;

   // Pointer to the main event loop
   GMainLoop *m_poMainLoop;

   // Pointer to the main event loop thread
   GThread *m_poEventThread;

   //DBus Proxy
//   ComBoschAutomotiveProxyAudioManager* m_poProxy;
   ComBoschAutomotiveProxyAudioManagerInterface* m_audioManager;

   //The instance of this class
   static agw_audiomanager_client* poSelf;

   /* Gdbus Callback functions */
   /*Callback when the proxy name is appears in the session bus*/
//   static void vOnNameAppeared(GDBusConnection* poConnection,
//                       const char* copcName,
//                       const char* copcNameOwner,
//                       gpointer pUserData);

//   /* Callback when the proxy name is not available
//    in the session bus*/
//   static void vOnNameVanish ( GDBusConnection* poConnection,
//                       const char* copcName,
//                       gpointer pUserData);
//   /* Callback when the proxy is created */
//   static void vNaviServiceProxyCallback (GObject* poSourceObject,
//                               GAsyncResult* poResult,
//                               gpointer pUserData);

   void covRegisterNaviSignals()const;

//   static void vHandleOnAllocate (ComBoschAutomotiveProxyAudioManager *object, guchar arg_source, const gchar *arg_sink);

//   static void vHandleOnDeAllocate (ComBoschAutomotiveProxyAudioManager *object, guchar arg_source);

//   static void vHandleOnMuteState (ComBoschAutomotiveProxyAudioManager *object, guchar arg_source);

//   static void vHandleSourceActivity (ComBoschAutomotiveProxyAudioManager *object, guchar arg_source, guint16 arg_status);


};

}
}
}


#endif /* AI_THIRDPARTY_NAVI_PRODUCTS_NAVIGATION_EXT_COMMON_DEMO_EXTERNALTHIRDPARTYNAVIGATIONAPP_SOURCE_VOICE_AGW_AUDIOMANAGER_CLIENT_H_ */
