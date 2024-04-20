#include <QDebug>
#include "rgaudiohelper.h"


namespace SKT {

RGAudioHelper::RGAudioHelper(PlatformController* platformController, QObject* parent)
    : QObject(parent)
    , m_delegate(newRGAudioDelegate(platformController))
    , m_mute(false)
    , m_mutex(QMutex::Recursive)
{
    m_platformVolume = m_delegate->getVolume();
    m_volume = fromPlatformVolume(m_platformVolume);

    connect(m_delegate, &RGAudioDelegate::platformVolumeChanged,
            this, &RGAudioHelper::onPlatformVolumeChanged);
}

RGAudioHelper::~RGAudioHelper()
{
    delete m_delegate;
}

VSM_Bool RGAudioHelper::init()
{
    // no-op
    return VSM_TRUE;
}

VSM_Bool RGAudioHelper::quit()
{
    // no-op
    return VSM_TRUE;
}

VSM_Bool RGAudioHelper::stop(VSM_Int32 channel)
{
    QMutexLocker locker(&m_mutex);

    return m_delegate->stop(channel) ? VSM_TRUE : VSM_FALSE;
}

VSM_Bool RGAudioHelper::pause(VSM_Int32 channel)
{
    // not supported
    Q_UNUSED(channel)
    return VSM_FALSE;
}

VSM_Bool RGAudioHelper::resume(VSM_Int32 channel)
{
    // not supported
    Q_UNUSED(channel)
    return VSM_FALSE;
}

VSM_Bool RGAudioHelper::isPlaying(VSM_Int32 channel)
{
    QMutexLocker locker(&m_mutex);

    return m_delegate->isPlaying(channel) ? VSM_TRUE : VSM_FALSE;
}

VSM_Bool RGAudioHelper::setVolume(VSM_UInt32 volume)
{
    // ignore setting volume by engine
    Q_UNUSED(volume)
    return VSM_TRUE;
}

VSM_Bool RGAudioHelper::play(VSM_Int32 channel, VSM_Int32 audioType, VSM_UInt32 wavId, const VSM_Byte* wavBuffer, VSM_Int32 wavSize)
{
    Q_UNUSED(audioType)
    QMutexLocker locker(&m_mutex);

//    qDebug() << "RGAudioHelper::play ["<<channel <<"]" << m_delegate->getMute(channel) << audioType;
    if(m_delegate->getMute(channel))
    {
        if(audioType == 2) //audioType == 2 : audio Setting
        {
            m_delegate->audioReady();
        }
        return VSM_TRUE;
    }

    if (m_delegate->play(channel,
                          audioType,
                          wavId,
                          (const char*)wavBuffer,
                          (size_t)wavSize) ? VSM_TRUE : VSM_FALSE) {
        return VSM_TRUE;
    } else {
        return VSM_FALSE;
    }
}

void RGAudioHelper::setNavigationVolume(int volume)
{
    QMutexLocker locker(&m_mutex);

    if (m_volume != volume) {
        m_volume = volume;

        float pltVolume = toPlatformVolume(volume);
        if (pltVolume != m_platformVolume) {
            m_platformVolume = pltVolume;
            m_delegate->setVolume(pltVolume);
        }

        emit navigationVolumeChanged(volume);
    }
}

bool RGAudioHelper::setMute(int channel, bool mute)
{
     QMutexLocker locker(&m_mutex);

    qDebug() << "onSetVoiceGuidance :: #5 " << mute << ")";

    // setMute 내에서 실행하는 mutex lock과 muteChanged에 연결된 콜체인에서 호출하는 mutex lock이,
    // 또 다른 쓰레드와 서로 교착상태에 빠지게 만드는 현상이 있어서 m_audioHelper->setMute() 내에서 호출하던 muteChanged를 setMute의 위로 빼냄. 2019.12.10 by hheo
//    if (channel == 1) emit muteChanged(mute); // fixed, navi service isVoiceGuidanceActive property has not updating

    return m_delegate->setMute(channel, mute);
}

bool RGAudioHelper::getMute(int channel)
{
    return m_delegate->getMute(channel);
}

bool RGAudioHelper::playLastAnnouncement()
{
    QMutexLocker locker(&m_mutex);

    return m_delegate->playLastAnnouncement(RGAUDIO_CHANNEL_GUIDANCE);
}

void RGAudioHelper::onPlatformVolumeChanged(float volume)
{
    QMutexLocker locker(&m_mutex);

    if (m_platformVolume != volume) {
        m_platformVolume = volume;

        int v = fromPlatformVolume(volume);
        if (v != m_volume) {
            m_volume = v;
            emit navigationVolumeChanged(v);
        }
    }
}

float RGAudioHelper::toPlatformVolume(int v)
{
    return (float(v) - RGAUDIO_VOLUME_MIN) / (RGAUDIO_VOLUME_MAX - RGAUDIO_VOLUME_MIN);
}

int RGAudioHelper::fromPlatformVolume(float v)
{
    return RGAUDIO_VOLUME_MIN + (RGAUDIO_VOLUME_MAX - RGAUDIO_VOLUME_MIN) * v;
}

}
