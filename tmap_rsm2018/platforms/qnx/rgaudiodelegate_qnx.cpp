
#include <QList>
#include <QDebug>

#include "rgaudiodelegate_qnx.h"
#include "platformcontroller_qnx.h"
#include "interface/rgaudioplayer.h"
#include "interface/rgalarmplayer.h"


namespace SKT {

RGAudioDelegate* RGAudioHelper::newRGAudioDelegate(PlatformController* playerData)
{
    Q_UNUSED(playerData)
    return new RGAudioDelegateQnx(playerData);
}

RGAudioDelegateQnx::RGAudioDelegateQnx(PlatformController* playerData)
    : m_volume(0.5)
{
    PlatformControllerQnx* platformController =
            qobject_cast<PlatformControllerQnx*>(playerData);

    if (platformController) {
        m_players[RGAUDIO_CHANNEL_ALARM] = new RGAlarmPlayer(platformController);
        m_players[RGAUDIO_CHANNEL_GUIDANCE] = new RGAudioPlayer(platformController);
    }
    else {
        qWarning("[RGAudioDelegateQnx] platformController is invalid");
    }

}

RGAudioDelegateQnx::~RGAudioDelegateQnx()
{
    for (auto player : m_players.values()) {
        if (player)
            delete player;
    }
}


bool RGAudioDelegateQnx::stop(int channel)
{
    qDebug("[RGAudioDelegateQnx] stop");
    RGAudioPlayerInterface *player = m_players[channel];
    if (player) {
        return player->stop();
    }
    return false;
}

bool RGAudioDelegateQnx::stopAll()
{
    qDebug("[RGAudioDelegateQnx] stopAll");
    bool retVal = true;
    for (auto player : m_players.values()) {
        if (player) {
            retVal &= player->stop();
        }
    }
    return retVal;
}

bool RGAudioDelegateQnx::isPlaying(int channel)
{
    Q_UNUSED(channel)

    RGAudioPlayerInterface *player = m_players[channel];
    if (player) {
        return player->isPlaying();
    }
    return false;
}

bool RGAudioDelegateQnx::setVolume(float volume)
{
    if (m_volume != volume) {
        m_volume = volume;
        emit platformVolumeChanged(m_volume);
    }

    return true;
}

float RGAudioDelegateQnx::getVolume() const
{
    return m_volume;
}

bool RGAudioDelegateQnx::play(int channel, int audioType, unsigned int wavId, const char* wavBuffer, size_t wavSize)
{
    Q_UNUSED(audioType)

    RGAudioPlayerInterface *player = m_players[channel];
    if (player) {
        return player->play(wavId, (const char*)wavBuffer, (size_t)wavSize);
    }
    return false;
}

bool RGAudioDelegateQnx::playLastAnnouncement(int channel)
{
    RGAudioPlayerInterface *player = m_players[channel];
    if (player) {
        return player->playLastAnnouncement();
    }
    return false;
}

}

//#include "rgaudiodelegate_qnx.moc"

