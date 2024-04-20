#include <QDBusArgument>
#include <QDBusConnection>
#include <QDBusConnectionInterface>
#include <QDBusInterface>
#include <QDBusMetaType>
#include <QDBusServiceWatcher>
#include <QDebug>
#include <QList>

#include "rgaudiodelegate_gen3.h"
#include "rgalarmplayer.h"
#include "rgaudioplayer.h"
#include "navigationextserviceinterface.h"
#include "../platformcontroller_gen3.h"

#define AM_SERVICE_NAME             "org.genivi.audiomanager"
#define AM_OBJECT_PATH              "/org/genivi/audiomanager/CommandInterface"
#define AM_INTERFACE                "org.genivi.audiomanager.CommandInterface"

#define AM_SINK_ID                  1
#define AM_PROPERTY_TYPE            37
#define AM_MIN_VOLUME               5
#define AM_MAX_VOLUME               40
#define AM_DEFAULT_VOLUME           (18+AM_MIN_VOLUME)

#define TO_PERCENT(v)               ((float(v)-AM_MIN_VOLUME)/(AM_MAX_VOLUME-AM_MIN_VOLUME))
#define FROM_PERCENT(v)             (AM_MIN_VOLUME+(float(v)*(AM_MAX_VOLUME-AM_MIN_VOLUME)))

namespace SKT {

typedef struct {
    short type;
    short value;
} SoundProperty;

typedef QList<SoundProperty> SoundProperties;

}

Q_DECLARE_METATYPE(SKT::SoundProperty)
Q_DECLARE_METATYPE(SKT::SoundProperties)


QDBusArgument &operator<<(QDBusArgument &argument, const SKT::SoundProperty &p)
{
    argument.beginStructure();
    argument << p.type;
    argument << p.value;
    argument.endStructure();
    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, SKT::SoundProperty &p)
{
    argument.beginStructure();
    argument >> p.type;
    argument >> p.value;
    argument.endStructure();
    return argument;
}

namespace SKT {

RGAudioDelegate* RGAudioHelper::newRGAudioDelegate(PlatformController *platformController)
{
    return new RGAudioDelegateGen3(platformController);
}

RGAudioDelegateGen3::RGAudioDelegateGen3(PlatformController *playerData)
    : m_volume(TO_PERCENT(AM_DEFAULT_VOLUME))
    , m_audioManager(NULL)
{
    qDebug() << "RGAudioDelegateGen3::RGAudioDelegateGen31";

    qDBusRegisterMetaType<SKT::SoundProperty>();
    qDBusRegisterMetaType<SKT::SoundProperties>();

    PlatformControllerGen3* platformController =
            qobject_cast<PlatformControllerGen3*>(playerData);

    OrgBoschCmNavigationNavigationExtServiceInterface* ext = NULL;
    if (platformController) {
        ext = platformController->navigationExt();
    }
    qDebug() << "RGAudioDelegateGen3::RGAudioDelegateGen32";

    m_players[RGAUDIO_CHANNEL_ALARM] = new RGAlarmPlayer(ext);
    m_players[RGAUDIO_CHANNEL_GUIDANCE] = new RGAudioPlayer(ext);

    m_mute[RGAUDIO_CHANNEL_ALARM] = false;
    m_mute[RGAUDIO_CHANNEL_GUIDANCE] = false;

//    initAudioManager();
}

RGAudioDelegateGen3::~RGAudioDelegateGen3()
{
    for (auto player : m_players.values()) {
        if (player)
            delete player;
    }
}

bool RGAudioDelegateGen3::stop(int channel)
{
    RGAudioPlayerInterface *player = m_players[channel];
    if (player) {
        return player->stop();
    }
    return false;
}

bool RGAudioDelegateGen3::stopAll()
{
    bool retVal = true;
    for (auto player : m_players.values()) {
        if (player) {
            retVal &= player->stop();
        }
    }
    return retVal;
}

bool RGAudioDelegateGen3::isPlaying(int channel)
{
    RGAudioPlayerInterface *player = m_players[channel];
    if (player) {
        return player->isPlaying();
    }
    return false;
}

bool RGAudioDelegateGen3::setVolume(float volume)
{
    if (!m_audioManager || !m_audioManager->isValid()) {
        qWarning() << "AudioManager is not valid!";
        return false;
    }

    bool retVal = false;

    ushort sinkID = AM_SINK_ID;
    SKT::SoundProperty prop;
    prop.type = AM_PROPERTY_TYPE;
    prop.value = FROM_PERCENT(volume);

    QList<QVariant> args;
    args << QVariant::fromValue(sinkID);
    args << QVariant::fromValue(prop);

    qDebug() << "SetMainSinkSoundProperty sinkID=" << sinkID
             << "type=" << prop.type << "value=" << prop.value;

    QDBusReply<short> reply = m_audioManager->callWithArgumentList(QDBus::Block,
                                                                   QLatin1String("SetMainSinkSoundProperty"),
                                                                   args);
    if (reply.isValid()) {
        short result = reply.value();
        qDebug() << "SetMainSinkSoundProperty result:" << result;
        if (result == 0) {
            m_volume = volume;
            retVal = true;
        }
    } else {
        qDebug() << "SetMainSinkSoundProperty error:" << reply.error();
    }

    if (retVal) {
        emit platformVolumeChanged(volume);
    }

    return retVal;
}

float RGAudioDelegateGen3::getVolume() const
{
    return m_volume;
}

bool RGAudioDelegateGen3::play(int channel, int audioType, unsigned int wavId, const char* wavBuffer, size_t wavSize)
{
    Q_UNUSED(audioType)

    RGAudioPlayerInterface *player = m_players[channel];
    if (player) {
        if(!player->isPlaying())
            return player->play(wavId, (const char*)wavBuffer, (size_t)wavSize);
        else
            return false;
    }
    return false;
}

bool RGAudioDelegateGen3::audioReady()
{
    RGAudioPlayerInterface *player =  m_players[1];
    if(player){
        player->audioReady();
    }
    return true;
}

bool RGAudioDelegateGen3::playComplete()
{
    RGAudioPlayerInterface * player = m_players[1];
    if(player)
    {
        player->playComplete();
    }
    return true;
}

bool RGAudioDelegateGen3::playLastAnnouncement(int channel)
{
    RGAudioPlayerInterface *player = m_players[channel];
    if (player) {
        return player->playLastAnnouncement();
    }
    return false;
}

bool RGAudioDelegateGen3::setMute(int channel, bool mute)
{
    m_mute[channel] = mute;
    stop(channel);
    return true;
}

bool RGAudioDelegateGen3::getMute(int channel)
{
    return m_mute[channel];
}

void RGAudioDelegateGen3::initAudioManager()
{
    qDebug() << "RGAudioDelegateGen3::initAudioManager";

    QDBusConnection conn = QDBusConnection::systemBus();
    if (!conn.isConnected()) {
        qWarning() << "initAudioManager::system bus is not connected.";
        return;
    }

    if (conn.interface()->isServiceRegistered(AM_SERVICE_NAME)) {
        onAudioManagerRegistered();
    } else {
        qDebug() << "Waiting for service:" << AM_SERVICE_NAME;
        QDBusServiceWatcher *serviceWatcher = new QDBusServiceWatcher(AM_SERVICE_NAME,
                conn, QDBusServiceWatcher::WatchForRegistration, this);
        connect(serviceWatcher, SIGNAL(serviceRegistered(QString)), this, SLOT(onAudioManagerRegistered()));
    }
}

void RGAudioDelegateGen3::onAudioManagerRegistered()
{
    qDebug() << "RGAudioDelegateGen3::onAudioManagerRegistered";

    QDBusConnection conn = QDBusConnection::systemBus();
    if (!conn.isConnected()) {
        qWarning() << "initAudioManager::system bus is not connected.";
        return;
    }

    m_audioManager = new QDBusInterface(AM_SERVICE_NAME,
                                        AM_OBJECT_PATH,
                                        AM_INTERFACE,
                                        conn,
                                        this);
    if (!m_audioManager->isValid()) {
        qWarning() << "AudioManager is not valid!";
        return;
    }

    ushort sinkID = AM_SINK_ID;

    QList<QVariant> args;
    args << QVariant::fromValue(sinkID);
    QDBusMessage reply = m_audioManager->callWithArgumentList(QDBus::Block,
                                                              QLatin1String("GetListMainSinkSoundProperties"),
                                                              args);
    if (reply.type() == QDBusMessage::ReplyMessage && reply.arguments().count() == 2) {
        short result = qdbus_cast<short>(reply.arguments().at(0));
        qDebug() << "GetListMainSinkSoundProperties result:" << result;

        if (result == 0) {
            SKT::SoundProperties properties = qdbus_cast<SKT::SoundProperties>(reply.arguments().at(1));
            for (auto p : properties) {
                if (p.type == AM_PROPERTY_TYPE) {
                    qDebug() << "Found sink property: type=" << p.type << " value=" << p.value;
                    float percent = TO_PERCENT(p.value);
                    qDebug() << "RGAudioDelegateGen3::m_volume: " << m_volume;
                    if (m_volume != percent) {
                        m_volume = percent;
                        emit platformVolumeChanged(percent);
                    }
                    break;
                }
            }
        }
    }

    // TODO: monitor property changed?

}

}
