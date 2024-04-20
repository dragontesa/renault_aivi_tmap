#include <QDebug>
#include <QThread>
#include <QQueue>

#include "rgaudioplayer.h"
#include "navigationextserviceinterface.h"
#include "SharedMemory.h"
#include "QDateTime"
#include "environment.h"

//#define TEST_PLAY

namespace SKT {

static const QString kParamWave = QStringLiteral("wave");
static const QString kParamSuccess = QStringLiteral("sucess");

#pragma pack(1)
struct WaveHeader {
    qint8      riff[4];            ///<  4    RIFF 파일이라는 지정
    quint32    totalDataSize;      ///<  4    현재부터 마지막까지의 파일 크기, 즉 파일크기-4
    qint8      wave[4];            ///<  4    웨이브 파일("WAVE")
    qint8      fmt[4];             ///<  4    자식 청크 시작 ("fmt ")
    quint32    sizeOfWaveFormat;   ///<  4    웨이브 포맷 사이즈
    qint16     pcmWaveFormat;      ///<  2    PCM 웨이브 포맷 = 1
    qint16     nchannel;           ///<  2    채널 수 (모노 = 1, 스테레오 = 2 )
    quint32    samplingPerSec;     ///<  4    샘플링의 수( 보통 8000, 11025, 22050 )
    quint32    avgBytesPerSec;     ///<  4    초당 샘플 바이트 수
    qint16     bytesPerSample;     ///<  2    샘플당 바이트 수 ( 1 또는 2 )
    qint16     bitsPerSample;      ///<  2    샘플당 비트 수 ( 보통 8 또는 16 )
    qint8      dataStart[4];       ///<  4    데이타 청크 시작("data")
    quint32    dataSize;           ///<  4    데이타 크기
};
#pragma pack()

class PlayerCommand
{
public:
    enum CommandType {
        Play = 0,
        PlayLastAnnouncement,
        Stop,
        PlayStatus,
        StopStatus
    };

    PlayerCommand(CommandType cmd) : m_command(cmd) {}
    PlayerCommand(CommandType cmd, const QVariantMap &params) : m_command(cmd), m_params(params) {}

    inline CommandType command() const {
        return m_command;
    }

    inline QVariant param(const QString &name) const {
        return m_params.value(name);
    }

    inline void addParam(const QString &name, const QVariant &value) {
        m_params.insert(name, value);
    }

private:
    CommandType m_command;
    QVariantMap m_params;
};

class PlayerThread : public QThread
{
    Q_OBJECT
public:
    PlayerThread(OrgBoschCmNavigationNavigationExtServiceInterface *interface)
        : m_interface(interface)
        , m_quitting(false)
        , m_isPlaying(false)
    #ifdef USE_ALSA
        , m_ThirdPartyPlayNavigationVoice(0)
    #endif
    {
        qDebug() << "PlayerThread";
        if (m_interface) {
            connect(m_interface, SIGNAL(NaviSigNaviAnnouncementStatus(uint)),
                    this, SLOT(onNaviSigNaviAnnouncementStatus(uint)),
                    Qt::DirectConnection);
//            connect(m_interface, SIGNAL(NaviSigNaviTTSStatus(uint)),
//                    this, SLOT(onNaviSigNaviTTSStatus(uint)),
//                    Qt::DirectConnection);
        }
#ifdef USE_ALSA
        m_ThirdPartyPlayNavigationVoice = third::party::voice::ThirdPartyPlayNavigationVoice::getInstance();
#endif
    }

    void play(const char* wavBuffer, size_t wavSize) {
        PlayerCommand cmd(PlayerCommand::Play);
        QByteArray buffer(wavBuffer, wavSize);  // deep copy
        //qDebug() << QString("1> buffer: %1 %2").arg(quintptr(buffer.constData()), 0, 16).arg(buffer.size());
        cmd.addParam(kParamWave, buffer);
        post(cmd);
    }

    bool audioReady(){
        m_ThirdPartyPlayNavigationVoice->vChangedAudioStatus((guchar)1);
        m_ThirdPartyPlayNavigationVoice->vSourceAvailabilityCallback();

        QTimer::singleShot(1000 * 3, this, [=](){
            if(!isPlaying())
            {
                m_ThirdPartyPlayNavigationVoice->onPlaybackCompleted();
            }
        });

        return true;
    }

    bool playComplete(){
        m_ThirdPartyPlayNavigationVoice->onPlaybackCompleted();
        return true;
    }

    void playLastAnnouncement() {
        PlayerCommand cmd(PlayerCommand::PlayLastAnnouncement);
        post(cmd);
    }

    bool isPlaying() const {
         qDebug() << "RGAudioPlayer: isPlaying " <<m_ThirdPartyPlayNavigationVoice->getAudioStatus();
//         m_isPlaying = true;
//         if(m_ThirdPartyPlayNavigationVoice->getAudioStatus() == 0)
//         {
//             m_isPlaying = false;
//         }
         return m_isPlaying;
    }

    void stopPlaying() {
        PlayerCommand cmd(PlayerCommand::Stop);
        post(cmd);
    }

    void stopThread() {
        m_mutex.lock();
        m_quitting = true;
        m_commandsNotEmpty.wakeAll();
        m_mutex.unlock();
    }

private slots:
    void onNaviSigNaviAnnouncementStatus(uint status) {
        qDebug() << "onNaviSigNaviAnnouncementStatus: status=" << status;

        switch (status) {
        case NAVI_ANNOUNCEMENT_STATUS_PLAY_FAIL:
        case NAVI_ANNOUNCEMENT_STATUS_PLAY_OK:
            m_isPlaying = false;
            playStatus(status == NAVI_ANNOUNCEMENT_STATUS_PLAY_OK);            
            break;
        case NAVI_ANNOUNCEMENT_STATUS_ABORT_FAIL:
        case NAVI_ANNOUNCEMENT_STATUS_ABORT_OK:
            m_isPlaying = false;
            stopStatus(status == NAVI_ANNOUNCEMENT_STATUS_ABORT_OK);
            break;
        default:
            // no-op
            break;
        }
    }

    void onNaviSigNaviTTSStatus(uint status) {
        qDebug() << "onNaviSigNaviTTSStatus: status=" << status;

        switch(status){
        case NAVI_TTS_STATUS_READY:
            break;
        case NAVI_TTS_STATUS_NOT_READY:
            break;
        default:
            break;
        }
    }

private:
    enum PlayerState {
        None = 0,
        Play,
        Playing,
        Stop,
        Stopping
    };

    void post(const PlayerCommand &command) {
        m_mutex.lock();
        m_commands.push_back(command);
        m_commandsNotEmpty.wakeAll();
        m_mutex.unlock();
    }


    void playStatus(bool success) {
        PlayerCommand cmd(PlayerCommand::PlayStatus);
        cmd.addParam(kParamSuccess, success);
        post(cmd);
    }

    void stopStatus(bool success) {
        PlayerCommand cmd(PlayerCommand::StopStatus);
        cmd.addParam(kParamSuccess, success);
        post(cmd);
    }

protected:
    void run() override {
        QQueue<QByteArray> waveQueue;
        PlayerState state = None;
        QByteArray lastWave;
        qDebug() << "RGAudioPlayer: void run() override ";

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

            PlayerCommand cmd = m_commands.first();
            m_commands.pop_front();
            m_mutex.unlock();

            qDebug() << "RGAudioPlayer: cmd.command() " << cmd.command();

            // handling the command
            switch (cmd.command()) {
            case PlayerCommand::Play: {
                qDebug() << "20191010 RGAudioPlayer:add wave ::::::::::::::::::::::::::::";

                // add wave
                QByteArray w = cmd.param(kParamWave).value<QByteArray>();
                waveQueue.enqueue(w);
                break;
            }
            case PlayerCommand::PlayLastAnnouncement: {
                if (lastWave.size() > 0) {
                    // clear remaining waves and then play the last announcement
                    waveQueue.clear();
                    if (state == PlayerState::Playing) {
                        state = PlayerState::Stop;
                    }
                    waveQueue.enqueue(lastWave);
                }
                break;
            }
            case PlayerCommand::Stop: {
                // clear remaining waves
                waveQueue.clear();
//                if (state == PlayerState::Playing)
                {
                    state = PlayerState::Stop;
                }
                break;
            }
            case PlayerCommand::PlayStatus: {
                if (state == PlayerState::Playing) {
                    state = PlayerState::None;
                }
                break;
            }
            case PlayerCommand::StopStatus: {
                if (state == PlayerState::Stopping) {
                    state = PlayerState::None;
                }
                break;
            }
            default:
                // no-op
                break;
            }
            qDebug() << "20191010 RGAudioPlayer: shouldWaitEvent state #0" << state;

            bool shouldWaitEvent = false;
            QByteArray wave;
            QString path;
            while (!shouldWaitEvent) {
                qDebug() << "20191010 RGAudioPlayer: shouldWaitEvent state #1" << state;
                switch (state) {
                case PlayerState::None: {
                    // play the next wave
                    if (!waveQueue.empty()) {
                        wave = waveQueue.dequeue();
                        state = PlayerState::Play;
                    } else {
                        // waiting event
                        shouldWaitEvent = true;

                        QString deleteDir = Environment::getTempPath();

                        QDir directory = QDir(deleteDir);
                        QStringList delFileList;
                        delFileList = directory.entryList(QStringList("*.wav"), QDir::Files|QDir::NoSymLinks);
                        qDebug() << "20191010 RGAudioPlayer: shouldWaitEvent state #3" << delFileList.length();

                        if(delFileList.length() > 7)
                        {
                            qDebug() << "RGAudioPlayer: shouldWaitEvent state #3";

                            for(int i = 0 ; i < 5 ; i++){
                              int isRGfile = delFileList[i].split("rg_").length();

                              if(isRGfile > 1){
                                  qDebug() << "RGAudioPlayer: shouldWaitEvent state #4" << isRGfile;

                                  QString deleteFilePath = deleteDir+ "/" + delFileList[i];
                                  QFile::remove(deleteFilePath);
                              }
                            }
                        }
                    }
                    break;
                }
                case PlayerState::Play: {
                    qDebug() << "RGAudioPlayer: Play (size=" << wave.size() << ")";

                    //qDebug() << QString("2> buffer: %1 %2").arg(quintptr(wave.constData()), 0, 16).arg(wave.size());

                    bool writeSuccess = false;
                    if (m_interface && wave.size() > 0) {
#ifndef TEST_PLAY
//                        ThirdPartyToolSet::SharedMemoryWriter writer;
//                        if (writer.Open() == 0) {
//                            if (writer.Write(wave.constData(), wave.size()) == (size_t)wave.size()) {
//                                writeSuccess = true;
//                            } else {
//                                qWarning() << "SharedMemoryWriter: failed to write";
//                            }
//                            writer.Close();
//                        } else {
//                            qWarning() << "SharedMemoryWriter: failed to open";
//                        }
                        //save
                        path = QString("%1/rg_%2.wav")
                                .arg(Environment::getTempPath())
                                .arg(QString::number(QDateTime::currentMSecsSinceEpoch()));

                        qDebug() << "20191010 RGAudioPlayer: shouldWaitEvent play";

                        QFile wavFile(path);
                        if (wavFile.exists()) {
                            writeSuccess = true;
                        }

                        if (!writeSuccess) {
                            if (wavFile.open(QIODevice::WriteOnly)) {
                                QDataStream out(&wavFile);

                                int len = out.writeRawData(wave.constData(), wave.size());
                                if (len > 0 && len == wave.size()) {
                                    writeSuccess = true;
                                }

                                wavFile.close();
                            }
                        }
#endif
                    }

                    bool playSuccess = false;
                    if (writeSuccess) {
//test
//#ifdef USE_ALSA
//                        playSuccess = m_ThirdPartyPlayNavigationVoice->playWaveFile(path.toLatin1().constData());
//                        if (playSuccess) {
//                            playStatus(playSuccess);
//                            lastWave = wave;
//                        }
//#else
                        const WaveHeader* header = (WaveHeader*)wave.constData();

                        QStringList contents;
                        contents << path.toLatin1();
                        //Change voice play interface
                        qDebug() << "NaviMethPlayNaviAnnouncement: " << path.toLatin1();
                        QDBusPendingReply<bool> ret = m_interface->NaviMethPlayNaviAnnouncementByAudioSource(
                                    NaviAnnouncementType::NAVI_ANNOUNCEMENT_TYPE_RECORD_FILE,
                                    NaviAnnouncementAudioSource::NAVI_ANNOUNCEMENT_AUDIOSOURCE_REGULAR,
                                    contents);
                        ret.waitForFinished();

                        if (ret.isError()) {
                            qDebug() << "NaviMethPlayNaviAnnouncement: error=" << ret.error();
                        } else {
                            qDebug() << "NaviMethPlayNaviAnnouncement: result=" << ret.value();
                            if (ret.value()) {
                                m_isPlaying = true;
                                playSuccess = true;
                                lastWave = wave;
                            }
                        }
//#endif
                    }
                    qDebug() << "RGAudioPlayer playSuccess: " << playSuccess;
                    wave.clear();
                    state = (playSuccess) ? PlayerState::Playing : PlayerState::None;
                    break;
                }
                case PlayerState::Playing:
                    shouldWaitEvent = true;
                    qDebug() << "RGAudioPlayer Playing: ";
                    break;
                case PlayerState::Stop: {
                    bool stopSuccess = false;

                    stopSuccess = m_ThirdPartyPlayNavigationVoice->abortPlayWaveFile();
                    if (stopSuccess) {
                        stopStatus(stopSuccess);
                        //Change voice play interface
                        QDBusPendingReply<bool> ret = m_interface->NaviMethAbortNaviAnnouncement();
                        ret.waitForFinished();
                        if (ret.isError()) {
                            qDebug() << "NaviMethAbortNaviAnnouncement: error=" << ret.error();
                        }
                    }

                    state = (stopSuccess) ? PlayerState::Stopping : PlayerState::None;
                    qDebug() << "RGAudioPlayer Stopping: ";
                    break;
                }
                case PlayerState::Stopping:
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
    QList<PlayerCommand> m_commands;
    QWaitCondition m_commandsNotEmpty;

    bool m_quitting;
    QAtomicInteger<bool> m_isPlaying;
#ifdef USE_ALSA
    third::party::voice::ThirdPartyPlayNavigationVoice* m_ThirdPartyPlayNavigationVoice;
#endif

};

RGAudioPlayer::RGAudioPlayer(OrgBoschCmNavigationNavigationExtServiceInterface *interface)
{
    qDebug() << "RGAudioPlayer::RGAudioPlayer";
    m_thread = new PlayerThread(interface);
    m_thread->start(QThread::HighestPriority);
}

RGAudioPlayer::~RGAudioPlayer()
{
    m_thread->stopThread();
    m_thread->wait();
    delete m_thread;
}

bool RGAudioPlayer::isPlaying() const
{
    return m_thread->isPlaying();
}

bool RGAudioPlayer::play(uint wavId, const char* wavBuffer, size_t wavSize)
{
    Q_UNUSED(wavId)

    m_thread->play(wavBuffer, wavSize);
    return true;
}

bool RGAudioPlayer::audioReady()
{
    qDebug()<<"bool RGAudioDelegateGen3::audioReady() #3";
    m_thread->audioReady();
    return true;
}

bool RGAudioPlayer::playComplete()
{
    qDebug()<<"RGAudioPlayer playComplete";
    m_thread->playComplete();
    return true;
}

bool RGAudioPlayer::playLastAnnouncement()
{
    m_thread->playLastAnnouncement();
    return true;
}

bool RGAudioPlayer::stop()
{
    m_thread->stopPlaying();
    return true;
}

}

#include "rgaudioplayer.moc"
