#include <QBuffer>
#include <QByteArray>
#include <QDebug>
#include <QFile>
#include <QDir>
#include <QString>
#include <QThread>

#include <cstdio>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <fstream>

#include "filegpsprovider.h"
#include "nmeadata.h"
#include "gpsutil.h"
#include "environment.h"
#include "messagequeue.h"
#include "drdata.h"
#ifdef BUILD_TARGET
#ifdef PLATFORM_QNX
#else
#include "DrPosInterface.h"
#endif
#endif
#include "positionreceive.h"
#include "locationdatapool.h"
#include "satellitedatapool.h"
#include <vector>
#include "VSM_Map.h"

using namespace std;

namespace SKT {

#define FILEGPSREADER_MSG_READNEXT      100
#define FILEGPSREADER_MSG_STOP          101
#define FILEGPSREADER_MSG_SEEK          102
#define FILEGPSREADER_MSG_STEP          103

#define PREV_BUFFER_SIZE                2048
#define STEP_SIZE                       (10*7)
#define SEARCH_DR                       "onProxyDeadReckPosition"


#define MM_GPSDR_TEST   // GPS & DR Log
#define DR_DELAY_TIME       (0)
#define MM_GPSDR_INTERVAL   (200)

#ifdef MM_GPSDR_TEST
typedef struct tagDrGpsLogInfo
{
    qint64          timeMs;
    LocationDataPtr location;
}DR_GPSLOGINFO;
#endif

//#define DR_LOG

typedef struct tagGpsLog
{
    qint64 timeMs;
    int nDrCnt;
    uint timestamp;
    QString strNmea;
    QString strGnss;
    QString strDr[10];
 }GPS_LOG;
typedef std::vector<GPS_LOG> GPSLOG_LIST;

typedef struct tagGpsLogNew
{
    uint time;
    uint timestamp;
    int type; //(RSM 1:sock gnss, 2:gnss, 3:dr), (JLR 11:UtcConst, 12:GnssFix, 13:RawDr)
    QString strLog;
 }GPS_LOGNEW;
typedef std::vector<GPS_LOGNEW> GPSLOGNEW_LIST;

class FileGpsReader : public QThread
{
public:
    FileGpsReader(const QString &logName, PositionProvider* provider) :
        m_provider(provider),
        m_LogFileName(logName),
        m_pPositionReceive(NULL),
        m_speedRatio(1.0),
        m_idxNmea(0),
        m_idxDr(0),
        m_idxLog(0),
        m_bNewGps(0),
        m_timeMs(0),
        m_Hour(0),
        m_cnt(0)
    {
        m_pPositionReceive = new PositionReceive;
    }

    ~FileGpsReader()
    {
        m_listGpsLog.clear();
        if(m_pPositionReceive)
        {
            delete m_pPositionReceive;
            m_pPositionReceive = NULL;
        }
    }

    void readGpsFile() {
        QFile locationFile;
        locationFile.setFileName(m_LogFileName);
        if (locationFile.open(QIODevice::ReadOnly))
        {
            //초기 위치 찾기 및 로그 버전 체크
            while (!locationFile.atEnd())
            {
                QByteArray line = locationFile.readLine();
                QString strLine = QString::fromUtf8(line);
                QString strTag = strLine.section(',', 0, 0);
                if(!strTag.compare("onProxyDeadReckPosition"))
                {
                    m_bNewGps = false;
                    break;
                }else if(!strTag.compare("ProxyDeadReckPosition"))
                {
                    m_bNewGps = true;
                    break;
                }else if(!strTag.compare("NotifyUtcConstData"))
                {
                    m_bNewGps = true;
                    break;
                }
            }

            if(m_bNewGps)
            {
                GPS_LOGNEW loginfoNew;
                loginfoNew.type = 0;

                while (!locationFile.atEnd())
                {
                    QByteArray line = locationFile.readLine();
                    QString strLine = QString::fromUtf8(line);
                    QString strTag = strLine.section(',', 0, 0);

                    // NMEA 데이터가 완료 되었으면 추가 한다.
                    if(loginfoNew.type == 1 && loginfoNew.strLog.length() > 0)
                    {
                        QString strFirst = strTag.left(1);
                        if(strTag.compare("$"))
                        {
                            m_listGpsLogNew.push_back(loginfoNew);
                            loginfoNew.type = 0;
                        }
                    }

                    // RSM log
                    ///////////////////////////////////////////////////////////
                    if(!strTag.compare("ProxyDeadReckPosition"))
                    {
                        QString strLinuxTime = strLine.section(',', 1, 1); // 00098.299
                        QString strTimestamp = strLine.section(',', 2, 2); // 672967

                        loginfoNew.type = 3;
                        loginfoNew.time = (uint)(strLinuxTime.toDouble()*1000);
                        loginfoNew.timestamp = strTimestamp.toUInt();
                        loginfoNew.strLog = strLine;
                        m_listGpsLogNew.push_back(loginfoNew);
                        loginfoNew.type = 0;
                    }else if(!strTag.compare("ProxyPositionGNSS"))
                    {
                        QString strLinuxTime = strLine.section(',', 1, 1); // 00098.299
                        QString strTimestamp = strLine.section(',', 2, 2); // 672967

                        loginfoNew.type = 2;
                        loginfoNew.time = (uint)(strLinuxTime.toDouble()*1000);
                        loginfoNew.timestamp = strTimestamp.toUInt();
                        loginfoNew.strLog = strLine;
                        m_listGpsLogNew.push_back(loginfoNew);
                        loginfoNew.type = 0;
                    }else if(!strTag.compare("ProxyDetailsGNSS"))
                    {
                        QString strLinuxTime = strLine.section(',', 1, 1); // 00098.299
                        QString strTimestamp = strLine.section(',', 2, 2); // 672967

                        loginfoNew.type = 4;
                        loginfoNew.time = (uint)(strLinuxTime.toDouble()*1000);
                        loginfoNew.timestamp = strTimestamp.toUInt();
                        loginfoNew.strLog = strLine;
                        m_listGpsLogNew.push_back(loginfoNew);
                        loginfoNew.type = 0;
                    }else if(!strTag.compare("sock_gnss"))
                    {
                        QString strLinuxTime = strLine.section(',', 1, 1); // 00098.299
                        QString strTimestamp = strLine.section(',', 2, 2); // 672967

                        loginfoNew.type = 1;
                        loginfoNew.time = (uint)(strLinuxTime.toDouble()*1000);
                        loginfoNew.timestamp = strTimestamp.toUInt();
                        loginfoNew.strLog = "";
                    }else if(loginfoNew.type == 1)
                    {
                        strTag = strTag.left(1);
                        if(!strTag.compare("$"))
                        {
                            loginfoNew.strLog += strLine;
                        }
                    }
                    // JLR log
                    ///////////////////////////////////////////////////////////
                    else if(!strTag.compare("NotifyUtcConstData"))
                    {
                        QString strLinuxTime = strLine.section(',', 1, 1); // 00098.299
//                        QString strTimestamp = strLine.section(',', 2, 2); // 672967

                        loginfoNew.type = 11;
                        loginfoNew.time = (uint)(strLinuxTime.toDouble()*1000);
                        loginfoNew.timestamp = 0;
                        loginfoNew.strLog = strLine;
                        m_listGpsLogNew.push_back(loginfoNew);
                        loginfoNew.type = 0;
                    }else if(!strTag.compare("NotifyGnssFixData"))
                    {
                        QString strLinuxTime = strLine.section(',', 1, 1); // 00098.299
//                        QString strTimestamp = strLine.section(',', 2, 2); // 672967

                        loginfoNew.type = 12;
                        loginfoNew.time = (uint)(strLinuxTime.toDouble()*1000);
                        loginfoNew.timestamp = 0;
                        loginfoNew.strLog = strLine;
                        m_listGpsLogNew.push_back(loginfoNew);
                        loginfoNew.type = 0;
                    }else if(!strTag.compare("NotifyRawDrData"))
                    {
                        QString strLinuxTime = strLine.section(',', 1, 1); // 00098.299
//                        QString strTimestamp = strLine.section(',', 2, 2); // 672967

                        loginfoNew.type = 13;
                        loginfoNew.time = (uint)(strLinuxTime.toDouble()*1000);
                        loginfoNew.timestamp = 0;
                        loginfoNew.strLog = strLine;
                        m_listGpsLogNew.push_back(loginfoNew);
                        loginfoNew.type = 0;
                    }
                }
            }else
            {
                bool bGnss = false, bNmea=false;
                drdata drinfo;
                GPS_LOG loginfo;
                NmeaData nmeaData;

                loginfo.nDrCnt = 0;
                loginfo.timeMs = 0;

                while (!locationFile.atEnd())
                {
                    QByteArray line = locationFile.readLine();
                    QString strLine = QString::fromUtf8(line);
                    QString strTag = strLine.section(',', 0, 0);
                    if(!strTag.compare("onProxyDeadReckPosition"))
                    {
                        QString strTime = strLine.section(',', 2, 2);
                        //2018-4-3 2:39:26.000
                        QDateTime time = QDateTime::fromString(strTime, "yyyy-M-d H:m:s.zzz");
                       // qDebug() << "DR : " << strTime << " ==== " << time.toMSecsSinceEpoch();

                        if(loginfo.nDrCnt < 10)
                            loginfo.strDr[loginfo.nDrCnt++] = strLine;
    //                    else
    //                        qDebug() << "Check DR : " << strLine;
                    }else if(!strTag.compare("onProxyPositionGNSS"))
                    {
                        loginfo.strGnss = strLine;
                    }else
                    {
                        strTag = strTag.left(10);
                        if(!strTag.compare("Timestamp:"))
                        {
                            QString strTime = strLine.section(':', 1, 1);
                            loginfo.timestamp = strTime.toUInt();
                        }else
                        {
                            if(nmeaData.parseSentence(line.constData(), line.count(), NMEA::RMC) == NMEA::RMC)
                            {
                                QDateTime dateTime = GpsUtil::nmeaTimeToQDateTime(nmeaData.m_rmc.utc);
                                if (dateTime.isValid()) {
                                    qint64 timeMs = dateTime.toMSecsSinceEpoch();
                                    if((quint64)loginfo.timeMs > 0 && (quint64)timeMs > (quint64)loginfo.timeMs && loginfo.nDrCnt > 0)
                                        m_listGpsLog.push_back(loginfo);

                                    loginfo.timeMs = timeMs;
                                    loginfo.nDrCnt = 0;
                                    loginfo.strNmea = "";
                                    loginfo.strGnss = "";
                                    //qDebug() << "NMEA : " << loginfo.timeMs;
                                }
                            }
                            loginfo.strNmea += strLine;
                        }
                    }
                }
            }


        }
        locationFile.close();
    }
    void play() {
        if(m_bNewGps) m_timeMs = 0;
        m_jobQueue.sendEmptyMessage(FILEGPSREADER_MSG_READNEXT);
    }

    void stop() {
        m_jobQueue.sendEmptyMessage(FILEGPSREADER_MSG_STOP);
    }

    void setSpeedRatio(double ratio) {
        // FIXME: update it in the thread.
        m_speedRatio = ratio;
    }

    void seekTo(double percent) {
        long gpsLogSize = m_listGpsLog.size();
        if(m_bNewGps) {
            m_timeMs = 0;
            gpsLogSize = m_listGpsLogNew.size();
        }
        long pos = (gpsLogSize-1) * percent / 100;
        Message *msg = m_jobQueue.obtainMessage(FILEGPSREADER_MSG_SEEK,
                                                0, pos);
        m_jobQueue.sendMessage(msg);
    }

    void fastForward() {
        if(m_bNewGps) m_timeMs = 0;
        Message *msg = m_jobQueue.obtainMessage(FILEGPSREADER_MSG_STEP, 0, STEP_SIZE);
        m_jobQueue.sendMessage(msg);
    }

    void rewind() {
        if(m_bNewGps) m_timeMs = 0;
        Message *msg = m_jobQueue.obtainMessage(FILEGPSREADER_MSG_STEP, 0, -STEP_SIZE);
        m_jobQueue.sendMessage(msg);
    }

    MessageQueue *jobQueue() { return &m_jobQueue; }

    void setGpsTime (int hour) {
        m_Hour = hour;
        m_pPositionReceive->SetGpsTimeBias(hour);
    }

    void setGpsLogUsage(bool enabled)
    {
        m_pPositionReceive->SetGpsLogUsage(enabled);
    }

    void setLogPath(QString path)
    {
        m_pPositionReceive->SetLogPath(path);
    }


protected:
    void run() Q_DECL_OVERRIDE {
        readGpsFile();
        forever {
            Message *msg = m_jobQueue.next();
            if (!msg) {
                break;
            }

            switch (msg->what) {
            case FILEGPSREADER_MSG_READNEXT:
            {
                if(m_bNewGps)
                {
                    if(m_listGpsLogNew.size() > 0)
                    {
                        GPS_LOGNEW &info = m_listGpsLogNew[m_idxLog];
                        uint logTime = info.time;

                        //qInfo() << "[FileGps]pos: " << m_idxLog / (double)m_listGpsLogNew.size();

                        int nGpsMethod = m_provider->getGpsMethod();
                        if(info.type == 1) //1:sock gnss
                        {
                            LocationDataPtr location = LocationDataPool::newDataPtr();
                            bool bRet = m_pPositionReceive->InputGpsPosition(info.timestamp, info.strLog.toLocal8Bit(), info.strLog.length(), location);
                            if(bRet)
                            {
                                if(nGpsMethod == 0)
                                {
                                    emit m_provider->positionUpdated(location);
                                }

                                SatelliteDataPtr satellite = m_pPositionReceive->GetSatellite();
                                if (satellite) {
                                    emit m_provider->satelliteUpdated(satellite);
                                }
                            }
                        }else if(info.type == 2) //2:gnss
                        {
                            LocationDataPtr location = LocationDataPool::newDataPtr();
                            bool bRet = m_pPositionReceive->InputGNSSPosition(info.strLog.toLocal8Bit(), (size_t)info.strLog.length(), location, true);
                            if(bRet && (nGpsMethod == 1 || nGpsMethod == 2))
                            {
                                emit m_provider->positionUpdated(location);
                            }
                        }else if(info.type == 3) //3:dr
                        {
                            LocationDataPtr location_dr = LocationDataPool::newDataPtr();
                            bool bRet = m_pPositionReceive->InpuDeadReckPosition(info.strLog.toLocal8Bit(), info.strLog.length(), location_dr, true);
                            if(bRet)
                            {
                                emit m_provider->positionUpdated(location_dr);
                            }
                        }else if(info.type == 4) //4:gnss
                        {
                            SatelliteDataPtr satellite = SatelliteDataPool::newDataPtr();
                            bool bRet = m_pPositionReceive->InputDetailsGNSS(info.strLog.toLocal8Bit(), (size_t)info.strLog.length(), satellite, true);
                            if(bRet)
                            {
                               emit m_provider->satelliteUpdated(satellite);
                            }
                        }else if(info.type == 11) //11:UtcConst
                        {
                            m_pPositionReceive->InputUtcConstData(info.strLog.toLocal8Bit(), (size_t)info.strLog.length());
                        }else if(info.type == 12) //12:GnssFix
                        {
                            LocationDataPtr location = LocationDataPool::newDataPtr();
                            bool bRet = m_pPositionReceive->InputGnssFixData(info.strLog.toLocal8Bit(), (size_t)info.strLog.length(), location);
                            if(bRet && (nGpsMethod == 1 || nGpsMethod == 2))
                            {
                                emit m_provider->positionUpdated(location);
                            }
                        }else if(info.type == 13) //13:RawDr
                        {
                            LocationDataPtr location_dr = LocationDataPool::newDataPtr();
                            bool bRet = m_pPositionReceive->InpuRawDrData(info.strLog.toLocal8Bit(), info.strLog.length(), location_dr);
                            if(bRet)
                            {
                                if(!m_pPositionReceive->IsJrlGpsGood())
                                {
                                    m_pPositionReceive->SetJrlGpsGood();
                                    LocationDataPtr location_gnss = LocationDataPool::newDataPtr();
                                    location_gnss->init();
                                    location_gnss->setGpsMode(LocationData::PHONE_GPS);
                                    location_gnss->setDataStatus(LocationData::ACTIVE);
                                    location_gnss->setDateTime(location_dr->dateTime());
                                    location_gnss->setAccuracy(-99);
                                    location_gnss->setLatitude(location_dr->latitude());
                                    location_gnss->setLongitude(location_dr->longitude());
                                    location_gnss->setSpeed(location_dr->speed());
                                    location_gnss->setDirection(location_dr->direction());
                                    location_gnss->setTimeStamp(location_dr->timeStamp());

                                    emit m_provider->positionUpdated(location_gnss);
                                }
                                emit m_provider->positionUpdated(location_dr);
                            }
                        }

                        QString strLine = info.strLog;

                        m_idxLog++;
                        if(m_idxLog >= m_listGpsLogNew.size())
                           m_idxLog = 0;


    //                    qint64 timeMs = QDateTime::currentMSecsSinceEpoch();
                        double timeToWait = 0;
    //                    int timeDelay = timeMs - m_timeMs;
                        int logDelay = m_listGpsLogNew[m_idxLog].time - logTime;
    //                    if(m_timeMs != 0 && timeDelay < logDelay)
    //                        timeToWait = logDelay - timeDelay;
                        if(logDelay > 0)
                            timeToWait = logDelay;

    //                    m_timeMs = timeMs;
    //                    qDebug() << "[" << m_idxLog << "] log(" << timeMs << " / " << info.time << " / " << timeToWait << " / " << info.type << " : " << strLine;
                        m_jobQueue.sendEmptyMessageDelayed(FILEGPSREADER_MSG_READNEXT, (unsigned long) (timeToWait / m_speedRatio));
                    }else
                    {
                        m_jobQueue.sendEmptyMessageDelayed(FILEGPSREADER_MSG_READNEXT, 200);
                    }
                }else
                {
                    if(m_listGpsLog.size() > 0)
                    {
                        if(m_idxDr == 0)
                        {
                            if(!m_listGpsLog[m_idxNmea].strNmea.isEmpty())
                            {
                                LocationDataPtr location = LocationDataPool::newDataPtr();
                                bool bRet = m_pPositionReceive->InputGpsPosition(m_listGpsLog[m_idxNmea].timestamp, m_listGpsLog[m_idxNmea].strNmea.toLocal8Bit(), m_listGpsLog[m_idxNmea].strNmea.length(), location);
                                if(bRet)
                                {
         //                           qDebug() << "[NMEA] " << location->gpsMode() << " : " << QString::number(location->latitude(), 'f', 6) << ", " << QString::number(location->longitude(), 'f', 6);
                                    emit m_provider->positionUpdated(location);

                                    SatelliteDataPtr satellite = m_pPositionReceive->GetSatellite();
                                    if (satellite) {
        //                                qDebug() << "[SATELLITE] ";
                                        emit m_provider->satelliteUpdated(satellite);
                                    }
                                }
                            }
                            if(!m_listGpsLog[m_idxNmea].strGnss.isEmpty())
                            {
                                LocationDataPtr location = LocationDataPool::newDataPtr();
                                bool bRet = m_pPositionReceive->InputGNSSPosition(m_listGpsLog[m_idxNmea].strGnss.toLocal8Bit(), (size_t)m_listGpsLog[m_idxNmea].strGnss.length(), location, false);
                            }
                        }

                        LocationDataPtr location_dr = LocationDataPool::newDataPtr();
                        bool bRet = m_pPositionReceive->InpuDeadReckPosition(m_listGpsLog[m_idxNmea].strDr[m_idxDr].toLocal8Bit(), m_listGpsLog[m_idxNmea].strDr[m_idxDr].length(), location_dr, false);
        //                qDebug() << m_listGpsLog[m_idxNmea].strDr[m_idxDr];
                        if(bRet)
                        {
        //                    qDebug() << "[DR] " << location_dr->gpsMode() << " : " << QString::number(location_dr->latitude(), 'f', 6) << ", " << QString::number(location_dr->longitude(), 'f', 6) << ", " << location_dr->speed();
                            emit m_provider->positionUpdated(location_dr);
                        }

                        m_idxDr++;
                        if(m_idxDr >= m_listGpsLog[m_idxNmea].nDrCnt)
                        {
                            m_idxDr = 0;
                            m_idxNmea++;
                            if(m_idxNmea >= m_listGpsLog.size())
                                m_idxNmea = 0;
                        }

                        qint64 timeMs = QDateTime::currentMSecsSinceEpoch();
                        qint64 timeToWait = 0;
                        if(timeMs - m_timeMs < 200)
                            timeToWait = timeMs - m_timeMs;

                        m_timeMs = timeMs;
                    }
                    m_jobQueue.sendEmptyMessageDelayed(FILEGPSREADER_MSG_READNEXT, 200/*timeToWait / m_speedRatio*/);
                }
                break;
            }

            case FILEGPSREADER_MSG_STOP: {
                m_jobQueue.removeMessages(FILEGPSREADER_MSG_READNEXT);
                break;
            }

            case FILEGPSREADER_MSG_SEEK: {
                long offset = msg->lparam;
                if(m_bNewGps)
                {
                    m_idxLog = offset;
                }else
                {
                    m_idxNmea = offset;
                    m_idxDr = 0;
                }
                m_timeMs = 0;
                if (m_jobQueue.removeMessages(FILEGPSREADER_MSG_READNEXT)) {
                    m_jobQueue.sendEmptyMessage(FILEGPSREADER_MSG_READNEXT);
                }
                break;
            }

            case FILEGPSREADER_MSG_STEP: {
                int step = (int)msg->lparam;
                if(m_bNewGps)
                {
                    m_idxLog += step;
                    if(m_idxLog < 0 || m_idxLog >= m_listGpsLogNew.size())
                        m_idxLog = 0;

                }else
                {
                    m_idxNmea += step;
                    if(m_idxNmea < 0 || m_idxNmea >= m_listGpsLog.size())
                        m_idxNmea = 0;

                    m_idxDr = 0;
                }
                m_timeMs = 0;
                if (m_jobQueue.removeMessages(FILEGPSREADER_MSG_READNEXT)) {
                    m_jobQueue.sendEmptyMessage(FILEGPSREADER_MSG_READNEXT);
                }
                break;
            }

            default:
                break;
            }

            // return to message pool
            m_jobQueue.recycleMessage(msg);
        }
    }

public:
    PositionReceive* m_pPositionReceive;

private:
    PositionProvider *m_provider;
    MessageQueue m_jobQueue;
    // QFile *m_logFile;
    QString m_LogFileName;
    double m_speedRatio;
    unsigned long m_fileSize;
    FileType mFileType;
    int m_Hour;
    int m_cnt;


    bool m_bNewGps;

    qint64 m_timeMs;
    int m_idxNmea;
    int m_idxDr;
    GPSLOG_LIST m_listGpsLog;

    int m_idxLog;
    GPSLOGNEW_LIST m_listGpsLogNew;
};

FileGpsProvider::FileGpsProvider(const QString &logName)
    : m_logName(logName)
    , m_reader(NULL)
    , m_mmfbTime(0)
    , m_mmfbX(0)
    , m_mmfbY(0)
{
    m_pLogFileMMFB = NULL;
    m_reader = new FileGpsReader(logName, this);
}

FileGpsProvider::~FileGpsProvider()
{
    stopUpdates();
    if(m_pLogFileMMFB)
    {
        delete m_pLogFileMMFB;
        m_pLogFileMMFB = NULL;
    }

    if(m_reader)
    {
        delete m_reader;
        m_reader = NULL;
    }
}

void FileGpsProvider::startUpdates()
{

    if (!m_reader) {
        m_reader = new FileGpsReader(m_logName, this);
    }
    m_reader->start();  // thread start
    m_reader->play();
}

void FileGpsProvider::stopUpdates()
{
    if (m_reader) {
        // quit reader thread
        m_reader->jobQueue()->quit();
        m_reader->wait();
    }
}

void FileGpsProvider::sendExtraCommand(const QString &cmd)
{
    QVector<QStringRef> params = cmd.splitRef(',');

    if (params.empty())
        return;

    const QStringRef &action = params.at(0);

    if (action == "fast-forward") {
        if (m_reader) {
            m_reader->fastForward();
        }
    } else if (action == "rewind") {
        if (m_reader) {
            m_reader->rewind();
        }
    } else if (action == "seekto") {
        if (m_reader) {
            auto percent = params.at(1).toInt();
            m_reader->seekTo(percent);
        }
    }
}

void FileGpsProvider::changeGpsLogUsage(bool enabled)
{
    if(m_reader)
        m_reader->setGpsLogUsage(enabled);
}

void FileGpsProvider::applyGpsTimeBias(int bias) {
//    qDebug() << "FileGpsProvider::applyGpsTimeBias: " << bias;
    if(m_reader)
        m_reader->setGpsTime(bias);
}

#ifdef TMAP_TMC_QC_TESTING
void FileGpsProvider::changeMMFeedback(bool enabled)
{
   m_useMMFB = enabled;
}

void FileGpsProvider::changeOnlyTunnelMMFB(bool enabled)
{
    m_useOnlyTunnelMMFB = enabled;
}

void FileGpsProvider::changeMMFeedbackLog(bool enabled)
{
    m_useSaveMMFBLog = enabled;
}


void FileGpsProvider::changeMMFeedbackGpsMethod(int method)
{
    m_MmfbGpsMethod = method;
}

void FileGpsProvider::changeMMFeedbackAngleOnTunnel(int method)
{
    m_MmfbAngleOnTunnel = method;
}

void FileGpsProvider::changeMMFeedbackDRCep(bool enabled)
{
    m_MmfbDrCEP = enabled;
}

void FileGpsProvider::changeMMFeedbackDRCepRange(int method)
{
    m_MmfbDrCEPrange = method;
}

// [TMCJLR-1137] CEP 맵매칭 거리 추가
void FileGpsProvider::changeMMFeedbackDRCepMMRange(int method)
{
//    m_MmfbDrCEPrange = method;
}

void FileGpsProvider::convertGpsLogToKML()
{
}

int FileGpsProvider::getGpsMethod()
{
    return m_MmfbGpsMethod;
}

void FileGpsProvider::setLogPath(QString path)
{
    if(m_pLogFileMMFB != NULL)
        delete m_pLogFileMMFB;
    m_pLogFileMMFB = new LogFile(eLogFileType_MMFB, path);

    if (m_reader) {
        m_reader->setLogPath(path);
    }
}

void FileGpsProvider::changeOpenLRLog(bool enabled)
{
    m_useSaveOpenLRLog = enabled;
}

#endif


void FileGpsProvider::onRouteGuidanceUpdated(SKT::RGDataPtr rgdata) {
    if (rgdata == NULL) {
        return;
    }
#ifdef PRODUCT_JLR2019
    static int g_nLastMmfbCnt = 10;
    if(rgdata->m_bTunnelMapMatchingFeedback || rgdata->m_bUnderpassMapMatchingFeedback)
        g_nLastMmfbCnt = 0;

    g_nLastMmfbCnt++;
    if(g_nLastMmfbCnt > 10)
    {
//        printf("m_pPositionReceive->SetFilterGnss(true) *****************************************************\n"); fflush(stdout);
        m_reader->m_pPositionReceive->SetFilterGnss(true);
    }else{
//        printf("m_pPositionReceive->SetFilterGnss(false) ####################################################\n"); fflush(stdout);
        m_reader->m_pPositionReceive->SetFilterGnss(false);
    }
#endif
    if (!(bool)rgdata->m_bMapMatchingFeedback) return;


#ifdef TMAP_TMC_QC_TESTING

    if (!(bool)rgdata->m_bMapMatchingFeedback) return;

    if (m_useMMFB) {
        if (m_useOnlyTunnelMMFB && !(bool)rgdata->m_bTunnelMapMatchingFeedback && !(bool)rgdata->m_bUnderpassMapMatchingFeedback)
        {
            uint32_t timestamp = rgdata->m_MapMatchingFeedbackInfo.m_Timestamp;
            int32_t method = rgdata->m_MapMatchingFeedbackInfo.m_nGpsMethod;
            memset(&rgdata->m_MapMatchingFeedbackInfo, 0x00, sizeof(VSM_MapMatchingFeedbackInfo));
            rgdata->m_MapMatchingFeedbackInfo.m_Timestamp = timestamp;
            rgdata->m_MapMatchingFeedbackInfo.m_nGpsMethod = method;
            return;
        }


//        if(rgdata->m_bUnderpassMapMatchingFeedback)
//            qDebug() << "****************** MMFB :  m_bUnderpassMapMatchingFeedback";
//        else
//            qDebug() << "****************** MMFB :  m_bTunnelMapMatchingFeedback";

        if (m_useSaveMMFBLog && m_pLogFileMMFB)
        {
            QString strLog;
            QTextStream out(&strLog);

#ifdef PRODUCT_JLR2019
            struct timespec rtime;
            clock_gettime(CLOCK_MONOTONIC, &rtime);
            // JLR일때 3초 단위로 MMFB 전송한다. (정확히 1초 단위로 MMFB가 발생되지 않으므로 2.8초 기준으로 체크한다.
            double current_time = rtime.tv_sec + (double)rtime.tv_nsec/1000000000.0;
            if(current_time < (m_mmfbTime+2.8))
                return;
            if(rgdata->stCurrent.nPosSpeed < 18) // 5m/s = 18km/h
            {
                int dist = 0;
                VSMMap_GetDistanceWorld(m_mmfbX, m_mmfbY, rgdata->m_MapMatchingFeedbackInfo.m_wc.x, rgdata->m_MapMatchingFeedbackInfo.m_wc.y, dist);
                if(dist < 20) // 20m
                {
                    //qDebug() << "---------------------------- dist = " << dist;
                    return;
                }
            }

            //qDebug() << "---------------------------- send mmfb (" << rgdata->stCurrent.nPosSpeed << ")";

            m_mmfbX = rgdata->m_MapMatchingFeedbackInfo.m_wc.x;
            m_mmfbY = rgdata->m_MapMatchingFeedbackInfo.m_wc.y;
            m_mmfbTime = current_time;

            uint32_t time_tag;
            float    latitude;
            float    longitude;
            float    heading;
            uint8_t  in_tunnel;
            uint8_t  is_bifurcation_ahead;
            uint8_t  is_righthand_traffic;
            uint8_t  is_oneway;
            uint8_t  is_offroad;
            uint8_t  mm_confidence;
            uint8_t  mm_probability;
            uint8_t  num_of_lanes;
            uint8_t  navi_suppliers;

            time_tag = rgdata->m_MapMatchingFeedbackInfo.m_Timestamp;     // System time tag
            latitude = rgdata->m_MapMatchingFeedbackInfo.m_Candidates[0].m_mmLatitude;  // Map-matching latitude (unit : WGS84 in degrees)
            longitude = rgdata->m_MapMatchingFeedbackInfo.m_Candidates[0].m_mmLongitude; // Map-matching longitude (unit : WGS84 in degrees)
            heading = rgdata->m_MapMatchingFeedbackInfo.m_Candidates[0].m_segmentHeading;    // Heading of current link in the direction vehicle is driving (unit : degrees)
            in_tunnel = (rgdata->m_MapMatchingFeedbackInfo.m_Candidates[0].m_segmentFlag == 1) ? 1 : 0;   // 0 : Not in tunnel, 1 : In tunnel
            is_bifurcation_ahead = 0; //분기지점 0 : No bifurcation ahead (Set as 0 if this data is unavailable), 1 : Yes
            is_righthand_traffic = 1; // 0 : No (Japan, UK, etc), 1 : Yes (America, EU, Korea, China, etc)
            is_oneway = (rgdata->stCurrent.linkcate==1) ? 0 : 1; // 0 : No, 1 : Yes
            is_offroad = 0; // 0 : No (not matched to street (for information only)), 1 : Yes
            mm_confidence = 255; // Map match confidence level: 0-6, 255 = no confidence
            mm_probability = rgdata->m_MapMatchingFeedbackInfo.m_Candidates[0].m_probability; // Map match probability
            num_of_lanes = rgdata->m_MapMatchingFeedbackInfo.m_Candidates[0].m_lane; // Number of lanes in the direction vehicle is driving, 0 if unsure
            navi_suppliers = 1; // 0 : Nuesoft, 1 : SKP, 2 : Aotunavi, 3 : HERE

            out << time_tag << ","
                   << QString::number(latitude, 'f', 8) << ","
                   << QString::number(longitude, 'f', 8) << ","
                   << (int)heading << ","
                   << (uint) in_tunnel << ","
                   << is_bifurcation_ahead << ","
                   << is_righthand_traffic << ","
                   << is_oneway << ","
                   << is_offroad << ","
                   << mm_confidence << ","
                   << mm_probability << ","
                   << num_of_lanes << ","
                   << navi_suppliers;
#else
            out << rgdata->m_MapMatchingFeedbackInfo.m_Timestamp << ","
                   << QString::number(rgdata->m_MapMatchingFeedbackInfo.m_Candidates[0].m_deltaLatitude, 'f', 8) << ","
                   << QString::number(rgdata->m_MapMatchingFeedbackInfo.m_Candidates[0].m_deltaLongitude, 'f', 8) << ","
                   << (uint)rgdata->m_MapMatchingFeedbackInfo.m_Candidates[0].m_probability << ","
                   << QString("0x%1").arg(rgdata->m_MapMatchingFeedbackInfo.m_Candidates[0].m_segmentFlag, 2, 16, QLatin1Char('0')) << ","
                   << (int)rgdata->m_MapMatchingFeedbackInfo.m_Candidates[0].m_segmentHeading << ","
                   << (uint)rgdata->m_MapMatchingFeedbackInfo.m_Candidates[0].m_streetWidth << ","
                   << QString::number(rgdata->m_MapMatchingFeedbackInfo.m_OffroadCandidate.m_deltaLatitude, 'f', 8) << ","
                   << QString::number(rgdata->m_MapMatchingFeedbackInfo.m_OffroadCandidate.m_deltaLongitude, 'f', 8) << ","
                   << (uint)rgdata->m_MapMatchingFeedbackInfo.m_OffroadCandidate.m_probability << ","
                   << QString("0x%1").arg(rgdata->m_MapMatchingFeedbackInfo.m_OffroadCandidate.m_segmentFlag, 2, 16, QLatin1Char('0')) << ","
                   << (int)rgdata->m_MapMatchingFeedbackInfo.m_OffroadCandidate.m_segmentHeading << ","
                   << (uint)rgdata->m_MapMatchingFeedbackInfo.m_OffroadCandidate.m_streetWidth;
#endif
            m_pLogFileMMFB->WriteString("MMFB", strLog);
        }
    }
    #endif
}

}
