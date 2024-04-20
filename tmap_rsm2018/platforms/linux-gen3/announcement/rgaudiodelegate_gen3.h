#ifndef RGAUDIODELEGATE_GEN3_H
#define RGAUDIODELEGATE_GEN3_H

#include <QObject>
#include <QMap>
#include "rgaudiohelper.h"

class QDBusInterface;

namespace SKT {

class RGAudioPlayerInterface;
class RGAudioDelegateGen3 : public RGAudioDelegate
{
    Q_OBJECT
public:
    RGAudioDelegateGen3(PlatformController *platformController);
    virtual ~RGAudioDelegateGen3();

    bool stop(int channel) override;
    bool stopAll() override;
    bool isPlaying(int channel) override;
    bool setVolume(float volume) override;
    float getVolume() const override;
    bool play(int channel, int audioType, unsigned int wavId, const char* wavBuffer, size_t wavSize) override;
    bool audioReady() override;
    bool playComplete() override;
    bool playLastAnnouncement(int channel) override;
    bool setMute(int channel, bool mute) override;
    bool getMute(int channel) override;

private:
    void initAudioManager();

private slots:
    void onAudioManagerRegistered();

private:
    QMap<int, RGAudioPlayerInterface*> m_players;
    QMap<int, bool> m_mute;
    float m_volume;
    QDBusInterface* m_audioManager;
};

}

#endif // RGAUDIODELEGATE_GEN3_H
