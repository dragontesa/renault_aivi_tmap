#include "rgaudiohelper.h"

namespace SKT {

class RGAudioDelegateDummy : public RGAudioDelegate {
    Q_OBJECT
public:
    RGAudioDelegateDummy() : m_volume(0.5) {

    }

    virtual ~RGAudioDelegateDummy() { }

    bool stop(int channel) override {
        Q_UNUSED(channel)
        return true;
    }

    bool stopAll() override {
        return true;
    }

    bool isPlaying(int channel) override {
        Q_UNUSED(channel)
        return false;
    }

    bool setVolume(float volume) override {
        if (m_volume != volume) {
            m_volume = volume;
            emit platformVolumeChanged(m_volume);
        }
        return true;
    }

    float getVolume() const override {
        return m_volume;
    }

    bool play(int channel, int audioType, unsigned int wavId, const char* wavBuffer, size_t wavSize) override {
        Q_UNUSED(channel)
        Q_UNUSED(audioType)
        Q_UNUSED(wavId)
        Q_UNUSED(wavBuffer)
        Q_UNUSED(wavSize)
        return true;
    }

    bool audioReady() override {
        return true;
    }

    bool playComplete() override{
        return true;
    }

    bool playLastAnnouncement(int channel) override {
        Q_UNUSED(channel)
        return true;
    }

    bool setMute(int channel, bool mute) override{
        return false;
    }

    bool getMute(int channel) override {
        return true;
    }

private:
    float m_volume;
};

RGAudioDelegate* RGAudioHelper::newRGAudioDelegate(PlatformController* playerData)
{
    Q_UNUSED(playerData)
    return new RGAudioDelegateDummy();
}

}

#include "rgaudiodelegate_dummy.moc"

