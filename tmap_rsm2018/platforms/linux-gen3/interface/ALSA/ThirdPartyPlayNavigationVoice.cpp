/*
 * ThirdPartyPlayNavigationVoice.cpp
 *
 *  Created on: Jan 12, 2018
 *      Author: man1szh
 */

#include <stdio.h>
#include <string.h>
#include <semaphore.h>

#include "ThirdPartyPlayNavigationVoice.h"

#include <dlt/dlt.h>
#include <QDebug>
//DLT_IMPORT_CONTEXT(NavigationExt_Context)

namespace third{
namespace party{
namespace voice{


ThirdPartyPlayNavigationVoice* ThirdPartyPlayNavigationVoice::m_pThirdPartyPlayNavigationVoice = NULL;

ThirdPartyPlayNavigationVoice::ThirdPartyPlayNavigationVoice () :
    m_p_audiomanager(0) {
    m_AudioSourceStatus = TDP_AP_AUDIO_SRC_ACT_OFF;
    m_u8PlayStatus = TDP_PLAYING_STATUS_IDLE;
    m_u8MuteStatus = TDP_AP_AUDIO_MUTESTATE_DEMUTE;
    memset(m_StrSinkName, 0, sizeof(m_StrSinkName) );
    memset(m_StrWaveFile, 0, sizeof(m_StrWaveFile) );
    m_AudioSource = AGW_NAVI_SOURCE;
    m_bSourceAvailability = false;
    initAudioManagerIF();
}

ThirdPartyPlayNavigationVoice::~ThirdPartyPlayNavigationVoice ( )
{
    if (m_p_audiomanager) {
        delete m_p_audiomanager;
        m_p_audiomanager = 0;
    }
}

void ThirdPartyPlayNavigationVoice::initAudioManagerIF(){
    QDBusConnection c = QDBusConnection::sessionBus();
    if (!c.isConnected()) {
        qWarning("dbus not connected.");
        return;
    }
    qWarning("ThirdPartyPlayNavigationVoice::initAudioManagerIF_dbus connected.");

    // If it is registered already, setup right away
    if (c.interface()->isServiceRegistered(AGW_AUDIOMANAGER_NAME)) {
        onServiceRegistered();
    } else {
        qDebug() << "Waiting for service:" << AGW_AUDIOMANAGER_NAME;
        QDBusServiceWatcher *serviceWatcher = new QDBusServiceWatcher(AGW_AUDIOMANAGER_NAME,
                c, QDBusServiceWatcher::WatchForRegistration, this);
        connect(serviceWatcher, &QDBusServiceWatcher::serviceRegistered, this, &ThirdPartyPlayNavigationVoice::onServiceRegistered);
    }
}

void ThirdPartyPlayNavigationVoice::onServiceRegistered() {
    qWarning("ThirdPartyPlayNavigationVoice connected.");

    QDBusConnection c = QDBusConnection::sessionBus();
    if (!c.isConnected()) {
        qWarning("dbus not connected.");
        return;
    }

    m_p_audiomanager = new ComBoschAutomotiveProxyAudioManagerInterface(AGW_AUDIOMANAGER_NAME,
                                                       AGW_AUDIOMANAGER_PATH,
                                                       c,
                                                       this);
    if (!m_p_audiomanager->isValid()) {
        qWarning("Cannot connect to service AUDIOMANAGER");
        return;
    } else {
        qWarning("ThirdPartyPlayNavigationVoice::onServiceRegistered_AUDIOMANAGER isValid");
        qDBusRegisterMetaType<SINKINFO>();
        qDBusRegisterMetaType<QList<SINKINFO>>();

        connect(m_p_audiomanager, &ComBoschAutomotiveProxyAudioManagerInterface::OnAllocate, this, &ThirdPartyPlayNavigationVoice::toOnAllocate);
        connect(m_p_audiomanager, &ComBoschAutomotiveProxyAudioManagerInterface::OnDeAllocate, this, &ThirdPartyPlayNavigationVoice::toOnDeAllocate);
        connect(m_p_audiomanager, &ComBoschAutomotiveProxyAudioManagerInterface::OnMuteState, this, &ThirdPartyPlayNavigationVoice::toOnMuteState);
        connect(m_p_audiomanager, &ComBoschAutomotiveProxyAudioManagerInterface::SourceActivity, this, &ThirdPartyPlayNavigationVoice::toSourceActivity);
        vOnConnectionReady();
    }
}

void ThirdPartyPlayNavigationVoice::toOnAllocate(uchar source, QList<SINKINFO> sink) {
    qDebug() << "ThirdPartyPlayNavigationVoice::OnAllocate";

    for(int i=0;i<sink.count();i++)
    {
        vOnAllcocateSignal((gchar)source, (const gchar*)sink.at(i).devName.toLatin1().constData());
    }
}

void ThirdPartyPlayNavigationVoice::toOnDeAllocate(uchar source) {
    qDebug() << "ThirdPartyPlayNavigationVoice::OnDeAllocate";
    vOnDeAllcocateSignal((gchar)source);
}
void ThirdPartyPlayNavigationVoice::toOnMuteState(uchar source, uchar mute_state) {
    qDebug() << "ThirdPartyPlayNavigationVoice::OnMuteState";
    vOnMuteStateSignal((gchar)source);
}
void ThirdPartyPlayNavigationVoice::toSourceActivity(uchar source, ushort status) {
    qDebug() << "ThirdPartyPlayNavigationVoice::SourceActivity";
    vOnSourceActivitySignal((gchar)source, status);
}
ThirdPartyPlayNavigationVoice* ThirdPartyPlayNavigationVoice::getInstance()
{
    qDebug() << "PlayerThread:ThirdPartyPlayNavigationVoice::getInstance";

   //DLT_LOGNavigationExt_Context,DLT_LOG_DEBUG,DLT_STRING("ThirdPartyPlayNavigationVoice::getInstance"));
   if(m_pThirdPartyPlayNavigationVoice == NULL)
    {
      m_pThirdPartyPlayNavigationVoice = new ThirdPartyPlayNavigationVoice();
    }
    return m_pThirdPartyPlayNavigationVoice;
}

void ThirdPartyPlayNavigationVoice::destroyInstance()
{
   if(m_pThirdPartyPlayNavigationVoice != NULL)
   {
      delete m_pThirdPartyPlayNavigationVoice;
   }
}

void ThirdPartyPlayNavigationVoice::onPlaybackCompleted()
{
    qDebug() << "ThirdPartyPlayNavigationVoice::onPlaybackCompleted()";
   //DLT_LOGNavigationExt_Context,DLT_LOG_DEBUG,DLT_STRING("ThirdPartyPlayNavigationVoice::onPlaybackCompleted"));

   memset(m_StrWaveFile, 0, sizeof(m_StrWaveFile) );
   m_u8PlayStatus = TDP_PLAYING_STATUS_IDLE;

   //release the audio channel/feedback the status to caller
   if (m_p_audiomanager) {
       QDBusPendingReply<bool> ret = m_p_audiomanager->AudioRouteRequest((uchar)m_AudioSource, TDP_AP_AUDIO_SRC_ACT_OFF);
       ret.waitForFinished();
       if (ret.isError()) {
       } else {
           if (ret.value())
               vAudioRouteRequestCallback(true);
       }
   }
}


bool ThirdPartyPlayNavigationVoice::playWaveFile(const char* pWaveFile)
{
   //DLT_LOGNavigationExt_Context,DLT_LOG_DEBUG,DLT_STRING("ThirdPartyPlayNavigationVoice::playWaveFile"),DLT_UINT16(m_AudioSourceStatus),DLT_UINT8(m_u8PlayStatus),DLT_STRING(pWaveFile));
   qDebug() << "ThirdPartyPlayNavigationVoice::playWaveFile";
    //1. Check audio channel status
   if (TDP_AP_AUDIO_SRC_ACT_ON == m_AudioSourceStatus)
   {
      if(TDP_PLAYING_STATUS_IDLE == m_u8PlayStatus)
      {
         if(m_StrSinkName[0] == 0)
         {
             //start play the wave file
             WaveFilePlayer &player = WaveFilePlayer::GetInst(this);
             qDebug() << "start play the wave file";

             player.Play(pWaveFile,m_StrSinkName);
             m_u8PlayStatus = TDP_PLAYING_STATUS_STREAMING_TO_ALSA;
         }
      }
      else if (TDP_PLAYING_STATUS_REQUESTING_AUDIO_CH == m_u8PlayStatus)
      {
         //there just over write the file name which want to play
         strncpy(m_StrWaveFile, pWaveFile, PATH_MAX);
      }
      else if (TDP_PLAYING_STATUS_STREAMING_TO_ALSA == m_u8PlayStatus)
      {
         //TODO stop current playing file and try to play the new file
      }
      else if (TDP_PLAYING_STATUS_ABORTING == m_u8PlayStatus)
      {
         //TODO wait for the abort success and play the new file again
      }
   }
   else
   {
       qDebug() << "ThirdPartyPlayNavigationVoice::playWaveFile_TDP_AP_AUDIO_SRC_ACT_ELSE";

      m_u8PlayStatus = TDP_PLAYING_STATUS_REQUESTING_AUDIO_CH;
      strncpy(m_StrWaveFile, pWaveFile, PATH_MAX);

      //Request channel and play the wave file when handle the audio channel
//      m_pThirdPartyPlayNavigationVoice->vAudioRouteRequest(m_AudioSource, TDP_AP_AUDIO_SRC_ACT_ON);
      if (m_p_audiomanager) {
          qDebug() << "ThirdPartyPlayNavigationVoice::playWaveFile_TDP_AP_AUDIO_SRC_ACT_ELSE";
          QDBusPendingReply<bool> ret = m_p_audiomanager->AudioRouteRequest((uchar)m_AudioSource, TDP_AP_AUDIO_SRC_ACT_ON);
          ret.waitForFinished();
          if (ret.isError()) {
              qDebug() << "ThirdPartyPlayNavigationVoice::playWaveFile::AudioRouteRequest: error=" << ret.error();
          } else {
              qDebug() << "ThirdPartyPlayNavigationVoice::playWaveFile::AudioRouteRequest: result=" << ret.value();
              if (ret.value())
                  vAudioRouteRequestCallback(true);
          }
      }
   }

   return true;
}

bool ThirdPartyPlayNavigationVoice::abortPlayWaveFile()
{
   if (m_u8PlayStatus == TDP_PLAYING_STATUS_ABORTING)
   {
      //TODO request OFF the audio channel
   }
   else if(m_u8PlayStatus == TDP_PLAYING_STATUS_STREAMING_TO_ALSA)
   {
      WaveFilePlayer &player = WaveFilePlayer::GetInst(this);

      //Abort a playing wave file is a sync operation
      player.AbortAndWait();
      player.readyPlayer();
   }

   return true;
}

void ThirdPartyPlayNavigationVoice::vOnConnectionReady()
{
   //DLT_LOGNavigationExt_Context,DLT_LOG_DEBUG,DLT_STRING(__FUNCTION__));

//   m_pThirdPartyPlayNavigationVoice->vSourceAvailability(m_AudioSource, true);
    qDebug() << "ThirdPartyPlayNavigationVoice::vOnConnectionReady:m_AudioSource: (uchar)" << (uchar)m_AudioSource;
   if (m_p_audiomanager) {
       QDBusPendingReply<> ret = m_p_audiomanager->SourceAvailability((uchar)m_AudioSource, true);
       ret.waitForFinished();
       if (ret.isError()) {
           qDebug() << "SourceAvailability: error=" << ret.error();
       } else {
           vSourceAvailabilityCallback();
       }
   }
}

void ThirdPartyPlayNavigationVoice::vOnAllcocateSignal(guchar arg_source, const gchar *arg_sink)
{
   //DLT_LOGNavigationExt_Context,DLT_LOG_DEBUG,DLT_STRING(__FUNCTION__),DLT_UINT8(arg_source), DLT_STRING(arg_sink));
   if (arg_source == m_AudioSource)
   {
      strncpy(m_StrSinkName, arg_sink, NAME_MAX);
   }
   else
   {
      //DLT_LOGNavigationExt_Context,DLT_LOG_DEBUG,DLT_STRING("Invalid source"));
   }
}

void ThirdPartyPlayNavigationVoice::vOnDeAllcocateSignal(guchar arg_source)
{
    memset(m_StrSinkName, 0, sizeof(m_StrSinkName));
   //DLT_LOGNavigationExt_Context,DLT_LOG_DEBUG,DLT_STRING("ThirdPartyPlayNavigationVoice::vOnDeAllcocateSignal: "),DLT_UINT8(arg_source));
}

void ThirdPartyPlayNavigationVoice::vOnMuteStateSignal(guchar arg_source)
{
   //DLT_LOGNavigationExt_Context,DLT_LOG_DEBUG,DLT_STRING("ThirdPartyPlayNavigationVoice::vOnMuteStateSignal: "),DLT_UINT8(arg_source));

}

void ThirdPartyPlayNavigationVoice::vOnSourceActivitySignal(guchar arg_source, guint16 arg_status)
{
   //DLT_LOGNavigationExt_Context,DLT_LOG_DEBUG,DLT_STRING("ThirdPartyPlayNavigationVoice::vOnSourceActivitySignal: "), DLT_UINT8(arg_source), DLT_UINT16(arg_status));
//    qDebug() << "vOnSourceActivitySignal:arg_status: " << arg_status;

   if (arg_source == m_AudioSource)
   {
      m_AudioSourceStatus = arg_status;

//      m_pThirdPartyPlayNavigationVoice->vSourceActivityResult(arg_source, arg_status);

      if (m_p_audiomanager) {
          QDBusPendingReply<> ret = m_p_audiomanager->SourceActivityResult((uchar)arg_source, (ushort)arg_status);
          ret.waitForFinished();
          if (ret.isError()) {
              qDebug() << "SourceActivityResult: error=" << ret.error();
          } else {
              vSourceActivityResultCallback();
          }
      }
   }
   else
   {
      //DLT_LOGNavigationExt_Context,DLT_LOG_DEBUG,DLT_STRING("ThirdPartyPlayNavigationVoice::"),DLT_STRING(__FUNCTION__),DLT_STRING("Invalid source"));
   }
}


void ThirdPartyPlayNavigationVoice::vAudioRouteRequestCallback(bool bStatus)
{
   //DLT_LOGNavigationExt_Context,DLT_LOG_DEBUG,DLT_STRING("ThirdPartyPlayNavigationVoice::"),DLT_STRING(__FUNCTION__),DLT_BOOL(bStatus));
}

void ThirdPartyPlayNavigationVoice::vChangedAudioStatus(guchar status)
{
    m_u8PlayStatus = TDP_PLAYING_STATUS_REQUESTING_AUDIO_CH;
}

void ThirdPartyPlayNavigationVoice::vSourceAvailabilityCallback()
{
   qDebug() << "RGAudioPlayer vSourceAvailabilityCallback "<< m_u8PlayStatus;
   //DLT_LOGNavigationExt_Context,DLT_LOG_DEBUG,DLT_STRING("ThirdPartyPlayNavigationVoice::"),DLT_STRING(__FUNCTION__));

   m_bSourceAvailability = true;

   if (m_u8PlayStatus == TDP_PLAYING_STATUS_REQUESTING_AUDIO_CH)
   {
       qDebug() << "RGAudioPlayer vSourceAvailabilityCallback #2";
//      m_pThirdPartyPlayNavigationVoice->vAudioRouteRequest(m_AudioSource, TDP_AP_AUDIO_SRC_ACT_ON);
       if (m_p_audiomanager) {
           qDebug() << "RGAudioPlayer vSourceAvailabilityCallback #3";
           QDBusPendingReply<bool> ret = m_p_audiomanager->AudioRouteRequest((uchar)m_AudioSource, TDP_AP_AUDIO_SRC_ACT_ON);
           ret.waitForFinished();
           if (ret.isError()) {
               qDebug() << "AudioRouteRequest: error=" << ret.error();
           } else {
               qDebug() << "AudioRouteRequest: result=" << ret.value();
               if (ret.value())
                   vAudioRouteRequestCallback(true);
           }
       }

   }
}

void ThirdPartyPlayNavigationVoice::vMuteStateCallback()
{
   //DLT_LOGNavigationExt_Context,DLT_LOG_DEBUG,DLT_STRING("ThirdPartyPlayNavigationVoice::"),DLT_STRING(__FUNCTION__),DLT_UINT8(m_u8MuteStatus));

   if (m_u8MuteStatus == TDP_AP_AUDIO_MUTESTATE_DEMUTE)
   {
      m_u8MuteStatus = TDP_AP_AUDIO_MUTESTATE_MUTE;
   }
   else if(m_u8MuteStatus == TDP_AP_AUDIO_MUTESTATE_DEMUTE_RUNNING)
   {
      m_u8MuteStatus = TDP_AP_AUDIO_MUTESTATE_DEMUTE;
   }
}

void ThirdPartyPlayNavigationVoice::vSourceActivityResultCallback()
{
   //DLT_LOGNavigationExt_Context,DLT_LOG_DEBUG,DLT_STRING("ThirdPartyPlayNavigationVoice::"),DLT_STRING(__FUNCTION__), DLT_UINT16(m_AudioSourceStatus), DLT_UINT8(m_u8PlayStatus));
//    qDebug() << "ThirdPartyPlayNavigationVoice::vSourceActivityResultCallback";

   if (TDP_AP_AUDIO_SRC_ACT_ON == m_AudioSourceStatus)
   {
      //Can play the PCM data now,
      //TODO
      if (TDP_PLAYING_STATUS_REQUESTING_AUDIO_CH == m_u8PlayStatus)
      {
         //DLT_LOGNavigationExt_Context,DLT_LOG_DEBUG,DLT_STRING("ThirdPartyPlayNavigationVoice::vSourceActivityResultCallback: Playing!"));
         //start play the wave file
         qDebug() << "ThirdPartyPlayNavigationVoice::WaveFilePlayer::GetInst";
         WaveFilePlayer &player = WaveFilePlayer::GetInst(this);

         //DLT_LOGNavigationExt_Context,DLT_LOG_DEBUG,DLT_STRING("ThirdPartyPlayNavigationVoice::vSourceActivityResultCallback: "), DLT_STRING(m_StrWaveFile), DLT_STRING(m_StrSinkName));
         qDebug() << "ThirdPartyPlayNavigationVoice::player.Play";

         player.Play(m_StrWaveFile, m_StrSinkName);

         m_u8PlayStatus = TDP_PLAYING_STATUS_STREAMING_TO_ALSA;
      }
   }
   else if(TDP_AP_AUDIO_SRC_ACT_OFF == m_AudioSourceStatus)
   {
      //Can not play the PCM data now,
      if (TDP_PLAYING_STATUS_STREAMING_TO_ALSA == m_u8PlayStatus)
      {
         //stop stream to ALSA device
         //TODO
      }
   }
   else if(TDP_AP_AUDIO_SRC_ACT_PAUSE == m_AudioSourceStatus)
   {
      //Pause play the PCM data now,
      if (TDP_PLAYING_STATUS_STREAMING_TO_ALSA == m_u8PlayStatus)
      {
         //Pause stream to ALSA device
         //TODO
      }
   }

}

int ThirdPartyPlayNavigationVoice::getAudioStatus()
{
    return m_u8PlayStatus;
}

void ThirdPartyPlayNavigationVoice::vAudioRouteRequest(guchar source, guint16 state_required) const
{
    if(m_p_audiomanager) {
        m_p_audiomanager->AudioRouteRequest((uchar)source, (uchar)state_required);
    }
}

void ThirdPartyPlayNavigationVoice::vSourceAvailability(guchar source, gboolean bAvailability) const
{
   if(m_p_audiomanager) {
        m_p_audiomanager->SourceAvailability((uchar)source, bAvailability);
   }
}

void ThirdPartyPlayNavigationVoice::vMuteState(guchar source, guint16 state_required) const
{
   if(m_p_audiomanager) {
        m_p_audiomanager->MuteState((uchar)source, (bool)state_required);
   }
}

void ThirdPartyPlayNavigationVoice::vSourceActivityResult(guchar source, guint16 status) const
{
   if(m_p_audiomanager) {
        m_p_audiomanager->SourceActivityResult((uchar)source, (ushort)status);
   }
}

} //voice
} //party
} //third

