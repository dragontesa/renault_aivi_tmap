/*
 * agwaudiomanagerclient.cpp
 *
 *  Created on: Oct 12, 2017
 *      Author: man1szh
 */

#include "agw_audiomanager_client.h"

#include "ThirdPartyPlayNavigationVoice.h"
#include <QDBusArgument>
#include <QDBusConnection>
#include <QDBusConnectionInterface>
#include <QDBusInterface>
#include <QDBusMetaType>
#include <QDBusServiceWatcher>
#include <QDebug>
#include <QList>

DLT_IMPORT_CONTEXT(NavigationExt_Context);

namespace third{
namespace party{
namespace voice{

agw_audiomanager_client * agw_audiomanager_client::poSelf = NULL;
ThirdPartyVoiceIF* agw_audiomanager_client::m_pThirdPartyVoiceIF = NULL;

agw_audiomanager_client::agw_audiomanager_client(ThirdPartyVoiceIF* pThirdPartyVoiceIF)
{
   DLT_LOG(NavigationExt_Context,DLT_LOG_DEBUG,DLT_STRING(__FUNCTION__));
    m_audioManager = NULL;
   uWatch = 0;
//   m_poProxy=NULL;

   /* No more than one instance of agw_audiomanager_client should be created*/
   poSelf = this;

   m_poMainLoop = NULL;
   m_poContext = NULL;
   m_poEventThread = NULL;

   m_poEventThread = g_thread_new((const gchar*)"agw_audiomanager_clientThread", pfEventThread, NULL);
   m_pThirdPartyVoiceIF = pThirdPartyVoiceIF;

}




agw_audiomanager_client::~agw_audiomanager_client()
{
   DLT_LOG(NavigationExt_Context,DLT_LOG_DEBUG,DLT_STRING(__FUNCTION__));
    if (m_audioManager) {
        delete m_audioManager;
        m_audioManager = 0;
    }
   /*Stops watching a server name*/
   g_bus_unwatch_name (uWatch);

   if( NULL != m_poProxy )
   {
      g_object_unref(m_poProxy);
      m_poProxy = NULL;
   }

   if(NULL != m_poMainLoop)
   {
      g_main_loop_quit (m_poMainLoop);
      g_main_loop_unref (m_poMainLoop);
      m_poMainLoop = NULL;
   }

   if(NULL != m_poEventThread)
   {
      /* Waits for the Event Thread to terminate execution */
      g_thread_join(m_poEventThread);
      m_poEventThread = NULL;
   }

   if (NULL != m_poContext)
   {
      g_main_context_unref (m_poContext);
      m_poContext = NULL;
   }
   poSelf == NULL;
}


void agw_audiomanager_client::vAudioRouteRequestCallback(GObject *poSourceObject, GAsyncResult *poResult, gpointer pUserData)
{
   DLT_LOG(NavigationExt_Context,DLT_LOG_DEBUG,DLT_STRING("agw_audiomanager_client::vAudioRouteRequestCallback"));

   (void*) pUserData; // To avoid lint warning
   (void*) poSourceObject; // To avoid lint warning
   bool bRetVal = FALSE;
   GError *poGErr=NULL;
   gboolean p_out_bStatus = FALSE;

   if( NULL != poSelf->m_poProxy  )
   {
      bRetVal = com_bosch_automotive_proxy_audio_manager_call_audio_route_request_finish(poSelf->m_poProxy, &p_out_bStatus, poResult, &poGErr);

      if(m_pThirdPartyVoiceIF != NULL)
      {
         m_pThirdPartyVoiceIF->vAudioRouteRequestCallback(p_out_bStatus);
      }

      DLT_LOG(NavigationExt_Context,DLT_LOG_DEBUG,DLT_STRING("vAudioRouteRequestCallback - Result:"),DLT_BOOL(p_out_bStatus));
   }
   else
   {
      DLT_LOG(NavigationExt_Context,DLT_LOG_DEBUG,DLT_STRING("Proxy not created"));
   }

}

void agw_audiomanager_client::vAudioRouteRequest(guchar source, guint16 state_required) const
{
   DLT_LOG(NavigationExt_Context,DLT_LOG_DEBUG,DLT_STRING("agw_audiomanager_client::vAudioRouteRequest"));
   DLT_LOG(NavigationExt_Context,DLT_LOG_DEBUG,DLT_STRING("Source: "),DLT_UINT8(source));
   DLT_LOG(NavigationExt_Context,DLT_LOG_DEBUG,DLT_STRING("state_required: "),DLT_UINT8(state_required));

   if ( NULL != poSelf->m_poProxy )
   {
      com_bosch_automotive_proxy_audio_manager_call_audio_route_request(poSelf->m_poProxy,source, state_required, NULL, vAudioRouteRequestCallback, NULL);
   }
   else
   {
      DLT_LOG(NavigationExt_Context,DLT_LOG_DEBUG,DLT_STRING("Proxy not created"));
   }

   if (m_audioManager) {
       QDBusPendingReply<bool> ret = m_audioManager->AudioRouteRequest((uchar)source, (uchar)state_required);
       ret.waitForFinished();
       if (ret.isError()) {
           qDebug() << "AudioRouteRequest: error=" << ret.error();
       } else {
           qDebug() << "AudioRouteRequest: result=" << ret.value();
           if (ret.value())
               m_pThirdPartyVoiceIF->vAudioRouteRequestCallback(true);
       }
   }
}

void agw_audiomanager_client::vSourceAvailabilityCallback(GObject *poSourceObject, GAsyncResult *poResult, gpointer pUserData)
{
   DLT_LOG(NavigationExt_Context,DLT_LOG_DEBUG,DLT_STRING("agw_audiomanager_client::vSourceAvailabilityCallback"));

   (void*) pUserData; // To avoid lint warning
   (void*) poSourceObject; // To avoid lint warning
   bool bRetVal = FALSE;
   GError *poGErr=NULL;

   if( NULL != poSelf->m_poProxy  )
   {
      bRetVal = com_bosch_automotive_proxy_audio_manager_call_source_availability_finish(poSelf->m_poProxy, poResult, &poGErr);

      if(m_pThirdPartyVoiceIF != NULL)
      {
         m_pThirdPartyVoiceIF->vSourceAvailabilityCallback();
      }
   }
   else
   {
      DLT_LOG(NavigationExt_Context,DLT_LOG_DEBUG,DLT_STRING("Proxy not created"));
   }
}

void agw_audiomanager_client::vSourceAvailability(guchar source, gboolean bAvailability) const
{
   DLT_LOG(NavigationExt_Context,DLT_LOG_DEBUG,DLT_STRING("agw_audiomanager_client::vSourceAvailability"));
   DLT_LOG(NavigationExt_Context,DLT_LOG_DEBUG,DLT_STRING("Source: "),DLT_UINT8(source));
   DLT_LOG(NavigationExt_Context,DLT_LOG_DEBUG,DLT_STRING("bAvailability: "),DLT_BOOL(bAvailability));

   if ( NULL != poSelf->m_poProxy )
   {
      com_bosch_automotive_proxy_audio_manager_call_source_availability(poSelf->m_poProxy,source, bAvailability, NULL, vSourceAvailabilityCallback, NULL);
   }
   else
   {
      DLT_LOG(NavigationExt_Context,DLT_LOG_DEBUG,DLT_STRING("Proxy not created"));
   }

   if (m_audioManager) {
       QDBusPendingReply<> ret = m_audioManager->SourceAvailability((uchar)source, (bool)bAvailability);
       ret.waitForFinished();
       if (ret.isError()) {
           qDebug() << "SourceAvailability: error=" << ret.error();
       } else {
           qDebug() << "SourceAvailability: result=" << ret.value();
           if (ret.value())
               m_pThirdPartyVoiceIF->vSourceAvailabilityCallback();
       }
   }
}

void agw_audiomanager_client::vMuteStateCallback(GObject *poSourceObject, GAsyncResult *poResult, gpointer pUserData)
{
   DLT_LOG(NavigationExt_Context,DLT_LOG_DEBUG,DLT_STRING("agw_audiomanager_client::vMuteStateCallback"));

   (void*) pUserData; // To avoid lint warning
   (void*) poSourceObject; // To avoid lint warning
   bool bRetVal = FALSE;
   GError *poGErr=NULL;

   if( NULL != poSelf->m_poProxy  )
   {
      bRetVal = com_bosch_automotive_proxy_audio_manager_call_mute_state_finish(poSelf->m_poProxy, poResult, &poGErr);

      if(m_pThirdPartyVoiceIF != NULL)
      {
         m_pThirdPartyVoiceIF->vMuteStateCallback();
      }
   }
   else
   {
      DLT_LOG(NavigationExt_Context,DLT_LOG_DEBUG,DLT_STRING("Proxy not created"));
   }



}

void agw_audiomanager_client::vMuteState(guchar source, guint16 state_required) const
{
   DLT_LOG(NavigationExt_Context,DLT_LOG_DEBUG,DLT_STRING("agw_audiomanager_client::vMuteState"));
   DLT_LOG(NavigationExt_Context,DLT_LOG_DEBUG,DLT_STRING("Source: "),DLT_UINT8(source));
   DLT_LOG(NavigationExt_Context,DLT_LOG_DEBUG,DLT_STRING("state_required: "),DLT_UINT8(state_required));

   if ( NULL != poSelf->m_poProxy )
   {
      com_bosch_automotive_proxy_audio_manager_call_mute_state(poSelf->m_poProxy,source, state_required, NULL, vMuteStateCallback, NULL);
   }
   else
   {
      DLT_LOG(NavigationExt_Context,DLT_LOG_DEBUG,DLT_STRING("Proxy not created"));
   }
   if (m_audioManager) {
       QDBusPendingReply<> ret = m_audioManager->MuteState((uchar)source, (bool)state_required);
       ret.waitForFinished();
       if (ret.isError()) {
           qDebug() << "MuteState: error=" << ret.error();
       } else {
           qDebug() << "MuteState: result=" << ret.value();
           if (ret.value())
               m_pThirdPartyVoiceIF->vMuteStateCallback();
       }
   }

}

void agw_audiomanager_client::vSourceActivityResultCallback(GObject *poSourceObject, GAsyncResult *poResult, gpointer pUserData)
{
   DLT_LOG(NavigationExt_Context,DLT_LOG_DEBUG,DLT_STRING("agw_audiomanager_client::vSourceActivityResultCallback"));

   (void*) pUserData; // To avoid lint warning
   (void*) poSourceObject; // To avoid lint warning
   bool bRetVal = FALSE;
   GError *poGErr=NULL;

   if( NULL != poSelf->m_poProxy  )
   {
      bRetVal = com_bosch_automotive_proxy_audio_manager_call_source_activity_result_finish(poSelf->m_poProxy, poResult, &poGErr);

      if(m_pThirdPartyVoiceIF != NULL)
      {
         m_pThirdPartyVoiceIF->vSourceActivityResultCallback();
      }
   }
   else
   {
      DLT_LOG(NavigationExt_Context,DLT_LOG_DEBUG,DLT_STRING("Proxy not created"));
   }
}

void agw_audiomanager_client::vSourceActivityResult(guchar source, guint16 status) const
{
   DLT_LOG(NavigationExt_Context,DLT_LOG_DEBUG,DLT_STRING("agw_audiomanager_client::vSourceActivityResult"));
   DLT_LOG(NavigationExt_Context,DLT_LOG_DEBUG,DLT_STRING("Source: "),DLT_UINT8(source));
   DLT_LOG(NavigationExt_Context,DLT_LOG_DEBUG,DLT_STRING("status: "),DLT_UINT8(status));

   if ( NULL != poSelf->m_poProxy )
   {
      com_bosch_automotive_proxy_audio_manager_call_source_activity_result(poSelf->m_poProxy,source, status, NULL, vSourceActivityResultCallback, NULL);
   }
   else
   {
      DLT_LOG(NavigationExt_Context,DLT_LOG_DEBUG,DLT_STRING("Proxy not created"));
   }
   if (m_audioManager) {
       QDBusPendingReply<> ret = m_audioManager->SourceActivityResult((uchar)source, (ushort)status);
       ret.waitForFinished();
       if (ret.isError()) {
           qDebug() << "SourceActivityResult: error=" << ret.error();
       } else {
           qDebug() << "SourceActivityResult: result=" << ret.value();
           if (ret.value())
               m_pThirdPartyVoiceIF->vSourceActivityResultCallback();
       }
   }
}


gpointer agw_audiomanager_client::pfEventThread(gpointer pvArg)
{
   (void*) pvArg;

   DLT_LOG(NavigationExt_Context,DLT_LOG_DEBUG,DLT_STRING("agw_audiomanager_client::pfEventThread"));

   g_type_init();


   poSelf->m_poContext = g_main_context_new();
   g_main_context_push_thread_default(poSelf->m_poContext);
   poSelf->m_poMainLoop = g_main_loop_new(poSelf->m_poContext, FALSE);

   poSelf->uWatch = g_bus_watch_name ( G_BUS_TYPE_SESSION ,
         AGW_AUDIOMANAGER_NAME,
      G_BUS_NAME_WATCHER_FLAGS_AUTO_START ,
      vOnNameAppeared ,
      vOnNameVanish ,
      NULL,
      NULL);


   /* Enters the main event dispatcher loop */
   g_main_loop_run(poSelf->m_poMainLoop);

   return NULL;
}

void agw_audiomanager_client::vOnNameAppeared(GDBusConnection *poConnection, const char *copcName, const char *copcNameOwner,gpointer pUserData)
{
   DLT_LOG(NavigationExt_Context,DLT_LOG_DEBUG,DLT_STRING("agw_audiomanager_client::vOnNameAppeared"));

   (void*) pUserData; // To avoid lint warning

   /* Asynchronously creates a m_poProxy for the D-Bus interface */
   /* When the operation is finished,
      vNaviServiceProxyCallback will be invoked*/
   com_bosch_automotive_proxy_audio_manager_proxy_new (  poConnection,
//       G_DBUS_PROXY_FLAGS_DO_NOT_LOAD_PROPERTIES,
         G_DBUS_PROXY_FLAGS_NONE,
         AGW_AUDIOMANAGER_NAME,
         AGW_AUDIOMANAGER_PATH,
      NULL,
      vNaviServiceProxyCallback,
      NULL );
}

void agw_audiomanager_client::vNaviServiceProxyCallback
   (
       GObject *poSourceObject,
       GAsyncResult *poResult,
       gpointer pUserData
   )
{
   DLT_LOG(NavigationExt_Context,DLT_LOG_WARN,DLT_STRING("agw_audiomanager_client::vNaviServiceProxyCallback"));

   (void*) pUserData; // To avoid lint warning
   (void*) poSourceObject; // To avoid lint warning
   GError *poGError = NULL;
   /*Finishes an operation started with adapter_proxy_new().*/
   if (NULL == poSelf->m_poProxy)
   {
      poSelf->m_poProxy = com_bosch_automotive_proxy_audio_manager_proxy_new_finish ( poResult, &poGError);

      if (NULL == poSelf->m_poProxy)
      {
         DLT_LOG(NavigationExt_Context,DLT_LOG_WARN,DLT_STRING("com_bosch_automotive_proxy_audio_manager_proxy_new_finish Fail"));
      }
      else
      {
         poSelf->covRegisterNaviSignals();
      }
   }
   else
   {
      DLT_LOG(NavigationExt_Context,DLT_LOG_WARN,DLT_STRING("agw_audiomanager_client::vNaviServiceProxyCallback =>  Proxy object already available"));
   }

}

void agw_audiomanager_client::vOnNameVanish(GDBusConnection *poConnection, const char *copcName, gpointer pUserData)
{
   DLT_LOG(NavigationExt_Context,DLT_LOG_DEBUG,DLT_STRING("agw_audiomanager_client: vOnNameVanish, Copc Name: "),DLT_STRING(copcName));

   (void*) pUserData; // To avoid lint warning
   (void*) poConnection; // To avoid lint warning
}



void agw_audiomanager_client::covRegisterNaviSignals()const
{
   DLT_LOG(NavigationExt_Context,DLT_LOG_WARN,DLT_STRING("agw_audiomanager_client: covRegisterNaviSignals"));

   g_signal_connect( poSelf->m_poProxy, "on-allocate", G_CALLBACK(vHandleOnAllocate), NULL );
   g_signal_connect( poSelf->m_poProxy, "on-de-allocate", G_CALLBACK(vHandleOnDeAllocate), NULL );
   g_signal_connect( poSelf->m_poProxy, "on-mute-state", G_CALLBACK(vHandleOnMuteState), NULL );
   g_signal_connect( poSelf->m_poProxy, "source-activity", G_CALLBACK(vHandleSourceActivity), NULL );


   if(m_pThirdPartyVoiceIF != NULL)
   {
      m_pThirdPartyVoiceIF->vOnConnectionReady();
   }

   DLT_LOG(NavigationExt_Context,DLT_LOG_DEBUG,DLT_STRING("agw_audiomanager_client: covRegisterNaviSignals Signals registered Successfully"));

}

void agw_audiomanager_client::vHandleOnAllocate (ComBoschAutomotiveProxyAudioManager *object, guchar arg_source, const gchar *arg_sink)
{
   DLT_LOG(NavigationExt_Context,DLT_LOG_DEBUG,DLT_STRING("agw_audiomanager_client: vHandleOnAllocate"),DLT_UINT8(arg_source), DLT_STRING(arg_sink));
   DLT_LOG(NavigationExt_Context,DLT_LOG_DEBUG,DLT_STRING("Source: "),DLT_UINT8(arg_source));
   DLT_LOG(NavigationExt_Context,DLT_LOG_DEBUG,DLT_STRING("Sink: "), DLT_STRING(arg_sink));

   if(m_pThirdPartyVoiceIF != NULL)
   {
      m_pThirdPartyVoiceIF->vOnAllcocateSignal(arg_source, arg_sink);
   }

   //todo
}

void agw_audiomanager_client::vHandleOnDeAllocate (ComBoschAutomotiveProxyAudioManager *object, guchar arg_source)
{
   DLT_LOG(NavigationExt_Context,DLT_LOG_DEBUG,DLT_STRING("agw_audiomanager_client: vHandleOnAllocate"));
   DLT_LOG(NavigationExt_Context,DLT_LOG_DEBUG,DLT_STRING("Source: "),DLT_UINT8(arg_source));

   if(m_pThirdPartyVoiceIF != NULL)
   {
      m_pThirdPartyVoiceIF->vOnDeAllcocateSignal(arg_source);
   }

   //todo
}

void agw_audiomanager_client::vHandleOnMuteState (ComBoschAutomotiveProxyAudioManager *object, guchar arg_source)
{
   DLT_LOG(NavigationExt_Context,DLT_LOG_DEBUG,DLT_STRING("agw_audiomanager_client: vHandleOnAllocate"));
   DLT_LOG(NavigationExt_Context,DLT_LOG_DEBUG,DLT_STRING("Source: "),DLT_UINT8(arg_source));

   if(m_pThirdPartyVoiceIF != NULL)
   {
      m_pThirdPartyVoiceIF->vOnMuteStateSignal(arg_source);
   }

   //todo
}

void agw_audiomanager_client::vHandleSourceActivity (ComBoschAutomotiveProxyAudioManager *object, guchar arg_source, guint16 arg_status)
{
   DLT_LOG(NavigationExt_Context,DLT_LOG_DEBUG,DLT_STRING("agw_audiomanager_client: vHandleOnAllocate"));
   DLT_LOG(NavigationExt_Context,DLT_LOG_DEBUG,DLT_STRING("Source: "),DLT_UINT8(arg_source));
   DLT_LOG(NavigationExt_Context,DLT_LOG_DEBUG,DLT_STRING("Status: "),DLT_UINT16(arg_status));

   if(m_pThirdPartyVoiceIF != NULL)
   {
      m_pThirdPartyVoiceIF->vOnSourceActivitySignal(arg_source, arg_status);
   }

   // feedback the vSourceActivityResult
   poSelf->vSourceActivityResult(AGW_NAVI_SOURCE, TDP_AP_AUDIO_SRC_ACT_ON);
}

void agw_audiomanager_client::onAllocate(uchar source, const QString &sink) {
    if(m_pThirdPartyVoiceIF != NULL)
    {
        const gchar* arg_sink;
        arg_sink = sink.toLatin1().data();
       m_pThirdPartyVoiceIF->vOnAllcocateSignal((guchar)source, arg_sink);
    }
}
void agw_audiomanager_client::onDeAllocate(uchar source) {
    if(m_pThirdPartyVoiceIF != NULL)
    {
       m_pThirdPartyVoiceIF->vOnDeAllcocateSignal((guchar)source);
    }
}
void agw_audiomanager_client::onMuteState(uchar source, uchar mute_state) {
    if(m_pThirdPartyVoiceIF != NULL)
    {
       m_pThirdPartyVoiceIF->vOnMuteStateSignal((guchar)source);
    }
}
void agw_audiomanager_client::onSourceActivity(uchar source, ushort status) {
    if(m_pThirdPartyVoiceIF != NULL)
    {
       m_pThirdPartyVoiceIF->vOnSourceActivitySignal((guchar)source, (guint16)status);
    }

    // feedback the vSourceActivityResult
    m_audioManager->SourceActivityResult(AGW_NAVI_SOURCE, TDP_AP_AUDIO_SRC_ACT_ON);
}




}
}
}




