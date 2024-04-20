/*
 * ThirdPartyPlayNavigationVoice.h
 *
 *  Created on: Jan 12, 2018
 *      Author: man1szh
 */

#ifndef AI_THIRDPARTY_NAVI_PRODUCTS_NAVIGATION_EXT_COMMON_DEMO_EXTERNALTHIRDPARTYNAVIGATIONAPP_SOURCE_VOICE_THIRDPARTYPLAYNAVIGATIONVOICE_H_
#define AI_THIRDPARTY_NAVI_PRODUCTS_NAVIGATION_EXT_COMMON_DEMO_EXTERNALTHIRDPARTYNAVIGATIONAPP_SOURCE_VOICE_THIRDPARTYPLAYNAVIGATIONVOICE_H_

#include "ThirdPartyVoiceIF.h"
#include "EventWaiter.h"
#include "WaveFilePlayer.h"
#include <AudioManagerInterface.h>
#include <QtDBus>
#include <QDebug>

namespace third{
namespace party{
namespace voice{

class agw_audiomanager_client;

#define AGW_NAVI_SOURCE TDP_AP_AUDIO_SRC_INTR_2
#define AGW_AUDIOMANAGER_NAME "com.bosch.AutomotiveProxy"
#define AGW_AUDIOMANAGER_PATH "/com/bosch/AutomotiveProxy/AudioManager"

/*
 *
 */
class ThirdPartyPlayNavigationVoice: public ThirdPartyVoiceIF,public WaveFilePlayerObserver, boost::noncopyable
{
public:

   static ThirdPartyPlayNavigationVoice* getInstance();

   static void destroyInstance();

   bool playWaveFile(const char* pWaveFile);
   bool abortPlayWaveFile();
   int  getAudioStatus();

   void vOnConnectionReady();
   void vOnAllcocateSignal(guchar arg_source, const gchar *arg_sink);
   void vOnDeAllcocateSignal(guchar arg_source);
   void vOnMuteStateSignal(guchar arg_source);
   void vOnSourceActivitySignal(guchar arg_source, guint16 arg_status);

   void vAudioRouteRequestCallback(bool bStatus);
   void vSourceAvailabilityCallback();
   void vChangedAudioStatus(guchar bStatus);
   void vMuteStateCallback();
   void vSourceActivityResultCallback();

   void vAudioRouteRequest(guchar source, guint16 state_required) const;
   void vSourceAvailability(guchar source, gboolean bAvailability) const;
   void vMuteState(guchar source, guint16 state_required) const;
   void vSourceActivityResult(guchar source, guint16 status) const;


   virtual void onPlaybackCompleted();

private slots:
    void onServiceRegistered();
    void toOnAllocate(uchar source, QList<SINKINFO> sink);
    void toOnDeAllocate(uchar source);
    void toOnMuteState(uchar source, uchar mute_state);
    void toSourceActivity(uchar source, ushort status);

private:
    void initAudioManagerIF();
   enum TDP_PLAYING_STATUS
   {
      TDP_PLAYING_STATUS_IDLE,
      TDP_PLAYING_STATUS_REQUESTING_AUDIO_CH,
      TDP_PLAYING_STATUS_STREAMING_TO_ALSA,
      TDP_PLAYING_STATUS_ABORTING
   };



   enum TDP_MUTE_STATUS
   {
      TDP_AP_AUDIO_MUTESTATE_DEMUTE = 0,
      TDP_AP_AUDIO_MUTESTATE_MUTE,
      TDP_AP_AUDIO_MUTESTATE_DEMUTE_RUNNING
   };

   ThirdPartyPlayNavigationVoice ();

   virtual ~ThirdPartyPlayNavigationVoice ( );

   //The instance of this class
   static ThirdPartyPlayNavigationVoice* m_pThirdPartyPlayNavigationVoice;

   bool                    m_bSourceAvailability;
   guchar                  m_AudioSource;
   guint16                 m_AudioSourceStatus;
   char                    m_StrSinkName[NAME_MAX];
   char                    m_StrWaveFile[PATH_MAX];
   guchar                  m_u8MuteStatus;
   guchar                  m_u8PlayStatus;

   ComBoschAutomotiveProxyAudioManagerInterface* m_p_audiomanager;

};

}
}
}

#endif /* AI_THIRDPARTY_NAVI_PRODUCTS_NAVIGATION_EXT_COMMON_DEMO_EXTERNALTHIRDPARTYNAVIGATIONAPP_SOURCE_VOICE_THIRDPARTYPLAYNAVIGATIONVOICE_H_ */
