#ifndef RGALARMPLAYER_H
#define RGALARMPLAYER_H

#include "rgaudioplayerinterface.h"
#ifdef USE_ALSA
    #include "ThirdPartyPlayNavigationVoice.h"
class third::party::voice::ThirdPartyPlayNavigationVoice;
#endif

class OrgBoschCmNavigationNavigationExtServiceInterface;

namespace SKT {

class AlarmThread;
class RGAlarmPlayer : public RGAudioPlayerInterface
{
public:
    RGAlarmPlayer(OrgBoschCmNavigationNavigationExtServiceInterface *interface);
    virtual ~RGAlarmPlayer();

    bool isPlaying() const override;
    bool play(uint wavId, const char* wavBuffer, size_t wavSize) override;
    bool audioReady() override;
    bool playComplete() override;
    bool playLastAnnouncement() override;
    bool stop() override;

private:
    QAtomicInteger<bool> m_playing;
    AlarmThread* m_thread;
};

}

#endif // RGALARMPLAYER_H
