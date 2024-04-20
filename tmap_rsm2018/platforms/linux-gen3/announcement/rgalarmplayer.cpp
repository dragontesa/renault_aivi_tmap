#include <QDebug>
#include <QThread>

#include "rgalarmplayer.h"
#include "navigationextserviceinterface.h"
#include "environment.h"

namespace SKT {

static const QString kParamId = QStringLiteral("wavId");
static const QString kParamBuffer = QStringLiteral("wavBuffer");
static const QString kParamSuccess = QStringLiteral("success");

class AlarmCommand
{
public:
    enum CommandType {
        Play = 0,
        PlayLastAnnouncement,
        Stop,
        PlayStatus,
        StopStatus
    };

    AlarmCommand(CommandType cmd) : m_command(cmd) {}
    AlarmCommand(CommandType cmd, const QVariantMap &params) : m_command(cmd), m_params(params) {}

    inline CommandType command() const {
        return m_command;
    }

    inline QVariant param(const QString &name) const {
        return m_params.value(name);
    }

    inline void setParam(const QString &name, const QVariant &value) {
        m_params.insert(name, value);
    }

    inline void addParam(const QString &name, const QVariant &value) {
        m_params.insert(name, value);
    }

private:
    CommandType m_command;
    QVariantMap m_params;
};

class AlarmThread : public QThread
{
    Q_OBJECT
public:
    AlarmThread(OrgBoschCmNavigationNavigationExtServiceInterface *interface)
        : m_interface(interface)
        , m_quitting(false)
        , m_isPlaying(false)
    #ifdef USE_ALSA
        , m_ThirdPartyPlayNavigationVoice(0)
    #endif

    {
        qDebug() << "RGAlarmPlayer::AlarmThread";
        if (m_interface) {
            connect(m_interface, SIGNAL(PlayMelodyStatus(uchar)),
                    this, SLOT(onPlayMelodyStatus(uchar)),
                    Qt::DirectConnection);
        }
#ifdef USE_ALSA
    m_ThirdPartyPlayNavigationVoice = third::party::voice::ThirdPartyPlayNavigationVoice::getInstance();
#endif

    }

    void play(uint wavId, const char* wavBuffer, size_t wavSize) {
        AlarmCommand cmd(AlarmCommand::Play);
        QByteArray buffer(wavBuffer, wavSize);
        //qDebug() << QString("1> buffer: %1").arg(quintptr(buffer.constData()), 0, 16);
        cmd.setParam(kParamId, wavId);
        cmd.setParam(kParamBuffer, buffer);
        post(cmd);
    }

    bool audioReady(){
       return true;
    }

    bool playComplete(){
        return true;
    }

    void playLastAnnouncement() {
        //qDebug() << "RGAlarmPlayer:: playLastAnnouncement()";
        AlarmCommand cmd(AlarmCommand::PlayLastAnnouncement);
        post(cmd);
    }

    bool isPlaying() const {
        return m_isPlaying;
    }

    void stopPlaying() {
        //qDebug() << "RGAlarmPlayer:: stopPlaying()";
        AlarmCommand cmd(AlarmCommand::Stop);
        post(cmd);
    }

    void stopThread() {
        m_mutex.lock();
        m_quitting = true;
        m_commandsNotEmpty.wakeAll();
        m_mutex.unlock();
    }

private slots:
    void onPlayMelodyStatus(uchar status) {
        //qDebug() << "onPlayMelodyStatus: status=" << status;
        playStatus(status == 1);
    }

private:
    enum PlayerState {
        None = 0,
        Play,
        Playing,
        Stop,
        Stopping
    };

    void post(const AlarmCommand &command) {
        m_mutex.lock();
        m_commands.push_back(command);
        m_commandsNotEmpty.wakeAll();
        m_mutex.unlock();
    }

    void playStatus(bool success) {
        AlarmCommand cmd(AlarmCommand::PlayStatus);
        cmd.setParam(kParamSuccess, success);
        post(cmd);
    }

    void stopStatus(bool success) {
        AlarmCommand cmd(AlarmCommand::StopStatus);
       // cmd.addParam(kParamSuccess, success);
        post(cmd);
    }

protected:
    void run() override {
        QList<AlarmCommand> playCommands;
        PlayerState state = PlayerState::None;

        forever {
            // get a new command
            m_mutex.lock();
            while (!m_quitting && m_commands.empty()) {
                m_commandsNotEmpty.wait(&m_mutex);
            }

            if (m_quitting) {
                // quit thread
                m_mutex.unlock();
                return;
            }

            AlarmCommand cmd = m_commands.first();
            m_commands.pop_front();
            m_mutex.unlock();

            // handling the command
            switch (cmd.command()) {
            case AlarmCommand::Play: {
                //qDebug() << "RGAlarmPlayer: AlarmCommand Play";
                // add wave
                playCommands.push_back(cmd);
                break;
            }
            case AlarmCommand::PlayLastAnnouncement: {
                 //qDebug() << ">>>>>>>>>>>>>> RGAlarmPlayer::  AlarmCommand::PlayLastAnnouncement()";
                    if (state == PlayerState::Playing) {
                        state = PlayerState::Stop;
                    }
                break;
            }
            case AlarmCommand::Stop: {
                //qDebug() << ">>>>>>>>>>>>>> RGAlarmPlayer: AlarmCommand Stop";
                // clear remaining waves
                playCommands.clear();
                if (state == PlayerState::Playing) {
                    state = PlayerState::Stop;
                }
                break;
            }
            case AlarmCommand::PlayStatus: {
                //qDebug() << ">>>>>>>>>>>>>>> RGAlarmPlayer: AlarmCommand PlayStatus";
                if (state == PlayerState::Playing) {
                    state = PlayerState::None;
                }
                break;
            }
            case AlarmCommand::StopStatus: {
                //qDebug() << ">>>>>>>> RGAlarmPlayer: AlarmCommand StopStatus";
                if (state == PlayerState::Stopping) {
                    state = PlayerState::None;
                }
                break;
            }
            default:
                // no-op
                break;
            }

            bool shouldWaitEvent = false;
            uint wavId;
            QByteArray wavBuffer;

            while (!shouldWaitEvent) {
                switch (state) {
                case PlayerState::None: {
                    //qDebug() << "<<<<<<<<<<<<<<<<<<<<<<<<< RGAlarmPlayer: None";
                    m_isPlaying = false;

                    // play the next wave
                    if (!playCommands.empty()) {
                        AlarmCommand playCmd = playCommands.first();

                        wavId = playCmd.param(kParamId).value<uint>();
                        wavBuffer = playCmd.param(kParamBuffer).value<QByteArray>();

                        //qDebug() << QString("2> buffer: %1").arg(quintptr(wavBuffer.constData()), 0, 16);
                        playCommands.pop_front();

                        state = PlayerState::Play;
                    } else {
                        // waiting event
                        shouldWaitEvent = true;


                        QString deleteDir = Environment::getTempPath();

                        QDir directory = QDir(deleteDir);
                        QStringList delFileList;
                        delFileList = directory.entryList(QStringList("*.wav"), QDir::Files|QDir::NoSymLinks);

                        if(delFileList.length() > 7)
                        {
                            int alarmcount = 0;
                            for(int i = 0 ; i < 5 ; i++)
                            {
                              int isAlarmFile = delFileList[i].split("alarm_").length();

                              if(isAlarmFile > 1){
                                  alarmcount ++;

                                  if(alarmcount > 2)
                                  {
                                      QString deleteFilePath = deleteDir+ "/" + delFileList[i];
                                      QFile::remove(deleteFilePath);
                                  }
                              }
                            }
                        }
                    }
                    break;
                }
                case PlayerState::Play: {
                    //qDebug() << "<<<<<<<<<<<<<<<<<<<<<<<<< RGAlarmPlayer: Play";

                    if (wavId == (uint)-1) {
                        state = PlayerState::None;
                        break;
                    }

                    m_isPlaying = true;

                    QString path = QString("%1/alarm_%2.wav")
                            .arg(Environment::getTempPath())
                            .arg(wavId);

//                    QByteArray wavBufferVolume;

//                    //wav header size : 44
//                    for(int i=0;i<44;i++)
//                    {
//                        wavBufferVolume.insert(i, wavBuffer.at(i));
//                    }

//                    //wave data 44 ~
//                    for (int i = 44; i < wavBuffer.size(); i += 2){
//                       short buf1 = wavBuffer.at(i+1);
//                       short buf2 = wavBuffer.at(i);

//                       buf1 = (short) ((buf1 & 0xff) << 8);
//                       buf2 = (short) (buf2 & 0xff);

//                       float volume = 0.2f; //기존 볼륨 기준
//                       short tempData= (short) (buf1 | buf2);
//                       tempData = (short) (tempData * volume);

//                       // convert back
//                       wavBufferVolume.insert(i, tempData);
//                       wavBufferVolume.insert(i+1, (tempData) >> 8);
//                    }

                    bool writeSuccess = false;
                    QFile wavFile(path);
                    if (wavFile.exists()) {
                        writeSuccess = true;
                    }

                    if (!writeSuccess) {
                        if (wavFile.open(QIODevice::WriteOnly)) {
                            QDataStream out(&wavFile);

                            int len = out.writeRawData(wavBuffer.constData(), wavBuffer.size());
                            if (len > 0 && len == wavBuffer.size()) {
                                writeSuccess = true;
                            }

                            wavFile.close();
                        }
                    }

                    wavBuffer.clear();
//                    wavBufferVolume.clear();

                    bool playSuccess = false;
                    if (writeSuccess && m_interface) {
                        qDebug() << "NaviMethPlayBeepMelody: path=" << path;

                        QDBusPendingReply<bool> ret = m_interface->NaviMethPlayBeepMelody(path);
                        //QDBusPendingReply<bool> ret = m_interface->NaviMethPlayBeep();
                        ret.waitForFinished();

                        if (ret.isError()) {
                            qDebug() << "NaviMethPlayBeepMelody: error=" << ret.error();
                        } else {
                            qDebug() << "NaviMethPlayBeepMelody: result=" << ret.value();
                            if (ret.value())
                                playSuccess = true;
                        }
                    }

//#ifdef USE_ALSA
//                    qDebug() << "alarm play by ALSA : "<< path.toLatin1().constData();
//                    playSuccess = m_ThirdPartyPlayNavigationVoice->playWaveFile(path.toLatin1().constData());
//                    if (playSuccess) {
//                        playStatus(playSuccess);
//                        //lastWave = wave;
//                    }
//#endif
                    state = (playSuccess) ? PlayerState::Playing : PlayerState::None;
                    //qDebug() << "<<<<<<<<<<<<<<<<<<<<< state : "<< state;
                    break;
                }
                case PlayerState::Playing:
                    //qDebug() << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< RGAlarmPlayer: Playing";
                    shouldWaitEvent = true;
                    break;
                case PlayerState::Stop: {
                    //qDebug() << "<<<<<<<<<<<<<<<<<<<< RGAlarmPlayer: Stop";
                    m_isPlaying = false;
                    bool stopSuccess = false;

                    stopSuccess = m_ThirdPartyPlayNavigationVoice->abortPlayWaveFile();
                    if (stopSuccess) {
                        stopStatus(stopSuccess);
                    }

                    state = (stopSuccess) ? PlayerState::Stopping : PlayerState::None;
                    //qDebug() << "<<<<<<<<<<<<<<<<<<<<< state #2 : "<< state;
                    break;
                }
                case PlayerState::Stopping:
                    //qDebug() << "<<<<<<<<<<<<<<<<<<<<<<<< RGAlarmPlayer: Stopping";
                    shouldWaitEvent = true;
                    break;
                default:
                    // no-op
                    shouldWaitEvent = true;
                    break;
                }
            }
        }
    }

private:
    OrgBoschCmNavigationNavigationExtServiceInterface *m_interface;
    QMutex m_mutex;
    QList<AlarmCommand> m_commands;
    QWaitCondition m_commandsNotEmpty;

    bool m_quitting;
    QAtomicInteger<bool> m_isPlaying;
#ifdef USE_ALSA
    third::party::voice::ThirdPartyPlayNavigationVoice* m_ThirdPartyPlayNavigationVoice;
#endif


};

RGAlarmPlayer::RGAlarmPlayer(OrgBoschCmNavigationNavigationExtServiceInterface *interface)
{
    qDebug() << "RGAlarmPlayer::RGAlarmPlayer";
    m_thread = new AlarmThread(interface);
    m_thread->start();
}

RGAlarmPlayer::~RGAlarmPlayer()
{
    m_thread->stopThread();
    m_thread->wait();
    delete m_thread;
}

bool RGAlarmPlayer::isPlaying() const
{
    return m_thread->isPlaying();
}

bool RGAlarmPlayer::play(uint wavId, const char* wavBuffer, size_t wavSize)
{
    m_thread->play(wavId, wavBuffer, wavSize);
    return true;
}

bool RGAlarmPlayer::audioReady()
{
    return true;
}

bool RGAlarmPlayer::playComplete()
{
    return true;
}

bool RGAlarmPlayer::playLastAnnouncement()
{
    qDebug() << "bool RGAlarmPlayer::playLastAnnouncement()";
    m_thread->playLastAnnouncement();
    return true;
}

bool RGAlarmPlayer::stop()
{
    qDebug() << "RGAlarmPlayer::  RGAlarmPlayer::stop()";
    m_thread->stopPlaying();
    return true;
}

}

#include "rgalarmplayer.moc"
