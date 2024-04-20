#include <QDebug>
#include <QFile>
#include <QDir>
#include <QMutex>
#include <QObject>
#include <QThread>

#include <stdio.h>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>

#include "gpsservice.h"
#include "positionprovider.h"
#include "positioncache.h"
#include "filegpsprovider.h"
#include "environment.h"
#include "gpsutil.h"
#include "rgdatapool.h"

#include "VSM_Navi.h"

#define DEFAULT_LATITUDE        37.566656
#define DEFAULT_LONGITUDE       126.978421
#define MSPOS_MAX_LENGTH        256

using namespace std;

namespace SKT {

class GpsServiceWorker : public QObject
{
    Q_OBJECT
public:
    explicit GpsServiceWorker(GpsService *service, QObject* parent = nullptr)
        : QObject(parent)
        , m_service(service)
        , m_isRGDataSubscribed(true)
    {
        QString positionData = Environment::getPreferencePath() + QDir::separator() + QLatin1String("position.ini");

        m_gpslocation = LocationDataPool::newDataPtr();
        m_gpslocation->init();
        m_positionCache = new PositionCache(positionData,
                                            DEFAULT_LONGITUDE,
                                            DEFAULT_LATITUDE);
    }

    ~GpsServiceWorker() {
        delete m_positionCache;
    }

    void lastPosition(double &lon, double &lat) const {
        m_positionCache->position(lon, lat);
    }

    void subscribeRouteGuidance(bool subscribe) {
        QMutexLocker locker(&m_mutex);
        m_isRGDataSubscribed = subscribe;
    }

    void setLogPath(QString path) {m_logPath = path;};

public slots:
    void processLocation(SKT::LocationDataPtr location);
    void processSatellites(SKT::SatelliteDataPtr satellite);

private:
    GpsService *m_service;
    PositionCache *m_positionCache;
    LocationDataPtr m_gpslocation;
    bool m_isRGDataSubscribed;
    QMutex m_mutex;
    QString m_logPath;
};

GpsService *g_pGpsService = NULL;

GpsService::GpsService(QObject* parent)
    : QObject(parent)
    , m_useSaveMMFBLog(false)
    , m_useSaveOpenLRLog(true)

{
    qRegisterMetaType<SKT::RGDataPtr>();
    qRegisterMetaType<SKT::LocationDataPtr>();
    qRegisterMetaType<SKT::SatelliteDataPtr>();

#ifdef PLATFORM_QNX
    m_locationcb = NULL;
    m_platformController = NULL;
#endif
    // 초기 가상주행할 GPS 로그는 내비설정값으로 한다.
    QString gpsFileName = Environment::getGpsLogPath() + QDir::separator() + QLatin1String("gps.log");
    QFile logFile(gpsFileName);
    if (logFile.exists()) {
        qDebug() << "Use File Gps";
        m_provider = new FileGpsProvider(gpsFileName);
    } else {
        m_provider = newGpsProvider();
#ifdef PLATFORM_QNX
        m_locationcb = m_provider->m_locationcb;
#endif
    }

    m_logPath = Environment::getSysGpsLogPath() + QDir::separator();

    QThread* thread = new QThread();
    m_worker = new GpsServiceWorker(this);
    m_worker->setLogPath(m_logPath);
    m_worker->moveToThread(thread);

    connect(thread, &QThread::finished, m_worker, &GpsServiceWorker::deleteLater);
    connect(thread, &QThread::finished, thread, &QThread::deleteLater);
    connect(m_provider, &PositionProvider::positionUpdated,
            m_worker, &GpsServiceWorker::processLocation);
    connect(m_provider, &PositionProvider::satelliteUpdated,
            m_worker, &GpsServiceWorker::processSatellites);
    connect(this, &GpsService::routeGuidanceUpdated, m_provider, &PositionProvider::onRouteGuidanceUpdated);
    thread->start();

#ifdef TMAP_TMC_QC_TESTING
    m_provider->changeMMFeedbackLog(m_useSaveMMFBLog);
#endif
    m_provider->setLogPath(m_logPath);
    m_pLogFileMM = new LogFile(eLogFileType_MM, m_logPath);

    g_pGpsService = this;
}

GpsService::~GpsService()
{
    g_pGpsService = NULL;
    m_provider->stopUpdates();
    delete m_provider;

    QThread* thread = m_worker->thread();
    if (thread->isRunning()) {
        thread->quit();
        thread->wait();
    }

    if(m_pLogFileMM)
    {
        delete m_pLogFileMM;
        m_pLogFileMM = NULL;
    }

    // m_worker will be deleted when the thread is finished.
}


void GpsService::InitNaviEngine()
{
    double lon, lat;
    m_worker->lastPosition(lon, lat);
    VSMNavi_InputPostion(lon, lat, VSM_TRUE);

    ////////////////////////////////////////////////////
    QString routePath = Environment::getRoutePath();
    QString ndsPath = Environment::getAdasDataPath() + "/nds";
    QString updatePath = Environment::getOnlineAdasPath();
    char szRoutePath[1024] = {0,};
    char szNdsPath[1024] = {0,};
    char szUpdatePath[1024] = {0,};
    qsnprintf(szRoutePath, sizeof(szRoutePath), "%s", routePath.toUtf8().constData());
    qsnprintf(szNdsPath, sizeof(szNdsPath), "%s", ndsPath.toUtf8().constData());
    qsnprintf(szUpdatePath, sizeof(szUpdatePath), "%s", updatePath.toUtf8().constData());

    bool bSaveOpenLRLog = false;
#ifdef PRODUCT_JLR2019
    VSMNavi_SetAdasUpdate(MountCallback,
                          szNdsPath,
                          szUpdatePath,
                          "https://tmcmap.tmap.co.kr/");
    bSaveOpenLRLog = true;
#endif
    VSMNavi_SetRoutePath(szRoutePath, bSaveOpenLRLog);
}

void GpsService::MountCallback(bool enable)
{
    if(g_pGpsService)
    {
        g_pGpsService->OnMountMapFolder(enable);
    }
}

void GpsService::OnMountMapFolder(bool enable)
{
#ifdef PLATFORM_QNX
    if(m_platformController)
    {
        if(enable)
            m_platformController->requestPartitionOptionChange((char*)"/ivimap",MountOption::MO_READ_WRITE);
        else
            m_platformController->requestPartitionOptionChange((char*)"/ivimap",MountOption::MO_READ_ONLY);
    }
#endif
}

#ifdef PLATFORM_QNX
void GpsService::SetPlatformController(PlatformController *platformController)
{
    m_platformController = platformController;
    if(m_provider)
        m_provider->SetPlatformController(platformController);
}
#endif

void GpsService::lastPosition(double &lon, double &lat) const
{
    m_worker->lastPosition(lon, lat);
}

void GpsService::subscribeRouteGuidance(bool subscribe)
{
    m_worker->subscribeRouteGuidance(subscribe);
}

void GpsService::startUpdates()
{
    // subscribe position data
    m_provider->startUpdates();
}

void GpsService::stopUpdates()
{
    // unsubscribe position data
    m_provider->stopUpdates();
}

void GpsService::sendExtraCommand(const QString &cmd)
{
    qDebug() << "[MENU] ***************** sendExtraCommand = " << cmd;
    m_provider->sendExtraCommand(cmd);
}


void GpsService::changeGpsLogUsage(bool enabled)
{
//    enabled = true;
    qDebug() << "[MENU] ***************** changeGpsLogUsage = " << enabled;
    m_provider->changeGpsLogUsage(enabled);
}

/**
 * @param bias 시간(Hour)
 * apply bias to gps time
 */
void GpsService::applyGpsTimeBias(int bias)
{
    qDebug() << "[MENU] ***************** GpsService::applyGpsTimeBias:" << bias;
    m_provider->applyGpsTimeBias(bias);
}

#ifdef TMAP_TMC_QC_TESTING
void GpsService::changeMMFeedback(bool enabled)
{
//    enabled = true;
    qDebug() << "[MENU] ***************** changeMMFeedback = " << enabled;
    // 맵매칭 피드백은 무조건 true  임시 설정
//    enabled = true;
    m_provider->changeMMFeedback(enabled);
}

void GpsService::changeOnlyTunnelMMFB(bool enabled)
{
//    enabled = true;
    qDebug() << "[MENU] ***************** changeOnlyTunnelMMFB = " << enabled;
    m_provider->changeOnlyTunnelMMFB(enabled);
}

void GpsService::changeMMFeedbackLog(bool enabled) {
//    enabled = true;
    qDebug() << "[MENU] ***************** changeMMFeedbackLog = " << enabled;
    m_useSaveMMFBLog = enabled;
    m_provider->changeMMFeedbackLog(enabled);
}

void GpsService::changeMMFeedbackGpsMethod(int method)
{
    qDebug() << "[MENU] ***************** changeMMFeedbackGpsMethod = " << method;
    if(method <= 0 || method > 2) method = 1;
    m_provider->changeMMFeedbackGpsMethod(method);
    VSMNavi_SetGpsMethod(method);
}

void GpsService::changeMMFeedbackAngleOnTunnel(int method)
{
    qDebug() << "[MENU] ***************** changeMMFeedbackAngleOnTunnel = " << method;
    m_provider->changeMMFeedbackAngleOnTunnel(method);
    VSMNavi_SetMMAngleOnTunnel(method==1);
}

void GpsService::changeMMFeedbackDRCep(bool enabled)
{
    qDebug() << "[MENU] ***************** changeMMFeedbackDRCep = " << enabled;
    m_provider->changeMMFeedbackDRCep(enabled);
    VSMNavi_SetDrCep(enabled);
}

void GpsService::changeMMFeedbackDRCepRange(int method)
{
    qDebug() << "[MENU] ***************** changeMMFeedbackDRCepRange = " << method;
    m_provider->changeMMFeedbackDRCepRange(method);
    VSMNavi_SetDrCepRange(method);
}

// [TMCJLR-1137] CEP 맵매칭 거리 추가
void GpsService::changeMMFeedbackDRCepMMRange(int method)
{
    qDebug() << "[MENU] ***************** changeMMFeedbackDRCepMMRange = " << method;
    m_provider->changeMMFeedbackDRCepMMRange(method);
    VSMNavi_SetDrCepMMRange(method);
}

void GpsService::changeOpenLRLog(bool enabled)
{
    qDebug() << "[MENU] ***************** changeOpenLRLog = " << enabled;
    m_useSaveOpenLRLog = enabled;
//    m_provider->changeOpenLRLog(enabled);
}

void GpsService::convertGpsLogToKML()
{
    qDebug() << "[MENU] ***************** changeGpsLogToKML";
//    m_provider->convertGpsLogToKML();

    VSM_Char szInput[256], szOutput[256];

    QDir dir(m_logPath);
    dir.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
    dir.setSorting(QDir::Name);
    dir.setNameFilters(QStringList() << "position*.dat");
    QStringList fileList = dir.entryList();

    for (int i = 0; i < fileList.size(); i++)
    {
        QFileInfo fileInfo = fileList.at(i);
        sprintf(szInput, "%s/%s", m_logPath.toStdString().c_str(), fileInfo.fileName().toStdString().c_str());
        sprintf(szOutput, "%s", m_logPath.toStdString().c_str());
        VSMNavi_Gps2Kml(szInput, szOutput);
    }
}

int GpsService::getGpsMethod()
{
    return m_provider->getGpsMethod();
}

void GpsService::setLogPath(QString path)
{
}

#endif

void GpsServiceWorker::processLocation(SKT::LocationDataPtr location)
{
    if (!location) {
        return;
    }
    // DR 첫번째 데이터가 아니면 각도 보정용으로만 사용한다.
    if(location->gpsMode() == LocationData::DR_GPS && !location->isDrActive())
    {
         VSMNavi_InputDrAdditional(location->speed(), location->direction());
        return;
    }

    // store the last position
    if (location->dataStatus() == LocationData::ACTIVE) {
        m_positionCache->setPosition(location->longitude(),
                                     location->latitude());
        emit m_service->locationUpdated(location);
    }

    // GPS 수신이면 DR 처리후 처리하기 위해 임시 유지 한다.
    if(location.data()->gpsMode() == LocationData::PHONE_GPS)
    {
       m_gpslocation->init();
       m_gpslocation = location;
       return;
    }


    QString mmLogName = m_logPath + "mapmatch_" + location->dateTime().toString("yyyy_M_d") + ".dat";

    // generate mspos
    static char mspos[MSPOS_MAX_LENGTH];
    int mspos_length;
    VsmGpsResult result;

    location.data()->drSolutionStation();
    //////////////////////////////////////////////////////////////////////////
    // DR  엔진 전달
    mspos_length = GpsUtil::getMSPOSFromLocation(mspos, sizeof(mspos), *location.data());
    if (mspos_length > 0)
    {
        // 맵매칭 시작 로그 저장
        if (m_service->m_useSaveMMFBLog && m_service->m_pLogFileMM)
        {
            QString strLog;
            QTextStream out(&strLog);
            out << location.data()->timeStamp() << ","
                << QString::number(location.data()->longitude(), 'f', 8) << ","
                << QString::number(location.data()->latitude(), 'f', 8);

            m_service->m_pLogFileMM->WriteString("MM_DR", strLog);
        }

        // qDebug() << "MSPOS(DR) : " << QString::number(location.data()->longitude(), 'g', 11) << " , " << QString::number(location.data()->latitude(), 'g', 10);

        // inject gps data into VSM engine

        VSMNavi_InputDiffDrAndGnss(m_gpslocation.data()->longitude(), m_gpslocation.data()->latitude(), location.data()->longitude(), location.data()->latitude());
        VSMNavi_InputGpsData(location.data()->timeStamp(), true, location.data()->longitude(), location.data()->latitude(), location->direction(), location.data()->altitude(), mspos, mspos_length, result);

        if(m_isRGDataSubscribed)
        {
            if (result == VsmGpsResultValid) {
                RGDataPtr rgData = RGDataPool::newRGDataPtr();

                if (VSMNavi_GetRouteGuidance(*rgData)) {
                    // 맵매칭 완료 및 피드백 생성
                    if (m_service->m_useSaveMMFBLog && m_service->m_pLogFileMM)
                    {
                        QString strLog;
                        QTextStream out(&strLog);
                        out << location.data()->timeStamp() << ","
                            << QString::number(rgData->stCurrent.longitude, 'f', 8) << ","
                            << QString::number(rgData->stCurrent.latitude, 'f', 8);

                        m_service->m_pLogFileMM->WriteString("MM2", strLog);
                    }

                    emit m_service->routeGuidanceUpdated(rgData);
                } else {
                    emit m_service->routeGuidanceUpdated(RGDataPtr());
                }
            } else if (result == VsmGpsResultNotValid) {
                emit m_service->routeGuidanceUpdated(RGDataPtr());
            }
        }
    }


    // 기존 수신된 GPS 엔진 전달
    if(m_gpslocation.data()->timeStamp() > 0)
    {
        m_gpslocation->setTimeStamp(location->timeStamp());

        mspos_length = GpsUtil::getMSPOSFromLocation(mspos, sizeof(mspos), *m_gpslocation.data());
        if (mspos_length > 0)
        {
            // 맵매칭 시작 로그 저장
            if (m_service->m_useSaveMMFBLog && m_service->m_pLogFileMM)
            {
                QString strLog;
                QTextStream out(&strLog);
                out << m_gpslocation.data()->timeStamp() << ","
                    << QString::number(m_gpslocation.data()->longitude(), 'f', 8) << ","
                    << QString::number(m_gpslocation.data()->latitude(), 'f', 8);

                m_service->m_pLogFileMM->WriteString("MM_GPS", strLog);
            }

            // qDebug() << "MSPOS(GPS) : " << QString::number(m_gpslocation.data()->longitude(), 'g', 11) << " , " << QString::number(m_gpslocation.data()->latitude(), 'g', 10);

            // inject gps data into VSM engine
            VSMNavi_InputGpsData(m_gpslocation.data()->timeStamp(), false, m_gpslocation.data()->longitude(), m_gpslocation.data()->latitude(), m_gpslocation.data()->direction(), m_gpslocation.data()->altitude(), mspos, mspos_length, result);

            if (m_isRGDataSubscribed)
            {
                if (result == VsmGpsResultValid)
                {
                    RGDataPtr rgData = RGDataPool::newRGDataPtr();

                    if (VSMNavi_GetRouteGuidance(*rgData))
                    {
                        // 맵매칭 완료 및 피드백 생성
                        if (m_service->m_useSaveMMFBLog && m_service->m_pLogFileMM)
                        {
                            QString strLog;
                            QTextStream out(&strLog);
                            out << m_gpslocation.data()->timeStamp() << ","
                                << QString::number(rgData->stCurrent.longitude, 'f', 8) << ","
                                << QString::number(rgData->stCurrent.latitude, 'f', 8);

                            m_service->m_pLogFileMM->WriteString("MM2", strLog);
                        }

                        emit m_service->routeGuidanceUpdated(rgData);
                    } else {
                        emit m_service->routeGuidanceUpdated(RGDataPtr());
                    }
                } else if (result == VsmGpsResultNotValid) {
                    emit m_service->routeGuidanceUpdated(RGDataPtr());
                }
            }
        }
        m_gpslocation->init();
    }
}

void GpsServiceWorker::processSatellites(SKT::SatelliteDataPtr satellite)
{
    if (!satellite) {
        return;
    }

    emit m_service->satelliteUpdated(satellite);
}

}

#include "gpsservice.moc"
