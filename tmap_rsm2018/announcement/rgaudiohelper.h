#ifndef RGAUDIOHELPER_H
#define RGAUDIOHELPER_H

#include <QObject>
#include <QMutex>

#include "VSM_Navi_Audio.h"

#define RGAUDIO_CHANNEL_ALARM       0
#define RGAUDIO_CHANNEL_GUIDANCE    1
#define RGAUDIO_VOLUME_MIN          0
#define RGAUDIO_VOLUME_MAX          40

namespace SKT {

class PlatformController;

class RGAudioDelegate : public QObject
{
    Q_OBJECT
public:
    virtual ~RGAudioDelegate() { }

    virtual bool stop(int channel) = 0;
    virtual bool stopAll() = 0;
    virtual bool isPlaying(int channel) = 0;

    /**
     * @brief setVolume
     * @param volume (0.0-1.0)
     * @return
     */
    virtual bool setVolume(float volume) = 0;

    /**
     * @brief getVolume
     * @return
     */
    virtual float getVolume() const = 0;

    virtual bool play(int channel, int audioType, unsigned int wavId, const char* wavBuffer, size_t wavSize) = 0;
    virtual bool audioReady() = 0;
    virtual bool playComplete() = 0;
    virtual bool playLastAnnouncement(int channel) = 0;

    virtual bool setMute(int channel, bool mute) = 0;
    virtual bool getMute(int channel) = 0;
signals:
    void platformVolumeChanged(float volume);
};

class RGAudioHelper : public QObject, public VSMNaviAudioInterface
{
    Q_OBJECT
public:
    RGAudioHelper(PlatformController* platformController, QObject* parent = 0);
    virtual ~RGAudioHelper();

    /**
     * @brief setNavigationVolume
     * @param volume (0~40)
     */
    void setNavigationVolume(int volume);

    /**
     * @brief navigationVolume
     * @return
     */
    int navigationVolume() const { return m_volume; }

    bool setMute(int channel, bool mute);
    bool getMute(int channel);

    bool mute() const { return m_mute; }

    bool playLastAnnouncement();

private slots:
    void onPlatformVolumeChanged(float volume);

private:
    RGAudioDelegate* newRGAudioDelegate(PlatformController* playerData);
    static float toPlatformVolume(int volume);
    static int fromPlatformVolume(float volume);

    // implement VSMNaviAudioInterface
    VSM_Bool init() override;
    VSM_Bool quit() override;
    VSM_Bool stop(VSM_Int32 channel) override;
    VSM_Bool pause(VSM_Int32 channel) override;
    VSM_Bool resume(VSM_Int32 channel) override;
    VSM_Bool isPlaying(VSM_Int32 channel) override;
    VSM_Bool setVolume(VSM_UInt32 volume) override;
    VSM_Bool play(VSM_Int32 channel, VSM_Int32 audioType, VSM_UInt32 wavId, const VSM_Byte* wavBuffer, VSM_Int32 wavSize) override;

signals:
    void navigationVolumeChanged(int volume);
    void muteChanged(bool mute);

private:
    RGAudioDelegate* m_delegate;
    bool m_mute;
    int m_volume;
    float m_platformVolume;
    QMutex m_mutex;
};

}

#endif // RGAUDIOHELPER_H
