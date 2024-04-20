/*
 * ThirdPartyVoiceIF.h
 *
 *  Created on: Jan 15, 2018
 *      Author: man1szh
 */

#ifndef AI_THIRDPARTY_NAVI_PRODUCTS_NAVIGATION_EXT_COMMON_DEMO_EXTERNALTHIRDPARTYNAVIGATIONAPP_SOURCE_VOICE_THIRDPARTYVOICEIF_H_
#define AI_THIRDPARTY_NAVI_PRODUCTS_NAVIGATION_EXT_COMMON_DEMO_EXTERNALTHIRDPARTYNAVIGATIONAPP_SOURCE_VOICE_THIRDPARTYVOICEIF_H_

#include <glib.h>    // For threading and main event loop functionalities
#include <QObject>
class WaveFilePlayerObserver
{
public:
    WaveFilePlayerObserver(){};
    virtual ~WaveFilePlayerObserver(){};

    /// DON'T BLOCK this Callback
    virtual void onPlaybackCompleted() = 0;
};

/*
 *
 */
namespace third{
namespace party{
namespace voice{

typedef enum
{
  TDP_AP_AUDIO_SRC_ACT_OFF = 0,
  TDP_AP_AUDIO_SRC_ACT_ON,
  TDP_AP_AUDIO_SRC_ACT_PAUSE
} Tdp_ApAudioSourceStatus;

typedef enum
{
  TDP_AP_AUDIO_SRC_MAIN_1 = 1,
  TDP_AP_AUDIO_SRC_MAIN_2,
  TDP_AP_AUDIO_SRC_MIXED_1,
  TDP_AP_AUDIO_SRC_MIXED_2,
  TDP_AP_AUDIO_SRC_INTR_1,
  TDP_AP_AUDIO_SRC_INTR_2,
  TDP_AP_AUDIO_SRC_SDS,
  TDP_AP_AUDIO_SRC_EXT_NAV
} Tdp_ApAudioSource;
class ThirdPartyVoiceIF : public QObject
{
public:
   ThirdPartyVoiceIF ( );
   virtual ~ThirdPartyVoiceIF ();
   ThirdPartyVoiceIF ( Tdp_ApAudioSource ApAudioSource);

   virtual void vOnConnectionReady();

   virtual void vOnAllcocateSignal(guchar arg_source, const gchar *arg_sink);
   virtual void vOnDeAllcocateSignal(guchar arg_source);
   virtual void vOnMuteStateSignal(guchar arg_source);
   virtual void vOnSourceActivitySignal(guchar arg_source, guint16 arg_status);

   virtual void vAudioRouteRequestCallback(bool bStatus);
   virtual void vChangedAudioStatus(guchar bStatus);
   virtual void vSourceAvailabilityCallback();
   virtual void vMuteStateCallback();
   virtual void vSourceActivityResultCallback();

};

}
}
}
#endif /* AI_THIRDPARTY_NAVI_PRODUCTS_NAVIGATION_EXT_COMMON_DEMO_EXTERNALTHIRDPARTYNAVIGATIONAPP_SOURCE_VOICE_THIRDPARTYVOICEIF_H_ */
