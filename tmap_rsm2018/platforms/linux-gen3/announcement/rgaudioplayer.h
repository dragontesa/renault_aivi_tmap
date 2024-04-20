#ifndef RGAUDIOPLAYER_H
#define RGAUDIOPLAYER_H

#include "rgaudioplayerinterface.h"
#ifdef USE_ALSA
    #include "ThirdPartyPlayNavigationVoice.h"
class third::party::voice::ThirdPartyPlayNavigationVoice;
#endif
class OrgBoschCmNavigationNavigationExtServiceInterface;

namespace SKT {

class PlayerThread;
class RGAudioPlayer : public RGAudioPlayerInterface
{
public:
    RGAudioPlayer(OrgBoschCmNavigationNavigationExtServiceInterface *interface);
    virtual ~RGAudioPlayer();

    bool isPlaying() const override;
    bool play(uint wavId, const char* wavBuffer, size_t wavSize) override;
    bool audioReady() override;
    bool playComplete() override;
    bool playLastAnnouncement() override;
    bool stop() override;

private:
    PlayerThread *m_thread;
    bool m_isPlaying;
#ifdef USE_ALSA
    third::party::voice::ThirdPartyPlayNavigationVoice* m_ThirdPartyPlayNavigationVoice;
#endif
};

}

#endif // RGAUDIOPLAYER_H
