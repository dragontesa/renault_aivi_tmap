#include <QDebug>
#include <QByteArray>
#include <QBuffer>
#include <stdint.h>
#include <stdio.h>
#include <iomanip>
#include <iostream>
#include <sstream>

#include "drpositionprovider_gen3.h"
#include "drpositionserviceinterface.h"
#include "DrPosInterface.h"
#include "socket_client.h"
#include "gpsutil.h"
#include "locationdatapool.h"
#include "interface/extAgent/navigationExtAgent.h"
#include "satellitedatapool.h"

#define DRPOSITION_VERBOSE_LOG
#ifdef DRPOSITION_VERBOSE_LOG
    #include <fstream>
    #include <iostream>
    #include <iomanip>
#endif
#define SERVICE_NAME            "App.Core.AppHmi_Navigation_Ext"
#define NAVIGATIONEXT_PATH      "/com/bosch/cm/ai/navigation/ext"

using namespace std;
using namespace com::bosch::DrPositionService;

namespace SKT {

static const ushort DR_VALID_FLAGS = VALID_TIMESTAMP | VALID_POSITION | VALID_HEADING | VALID_SPEED | VALID_ALTITUDE;
static const ushort DR_SENSOR_VALID_FLAGS = SF_USED_ODOMETER | SF_USED_GYROSCOPE | SF_USED_GNSS | SF_USED_MMFB | SF_CALI_ODOMETER | SF_CALI_GYROSCOPE;

static const int KOREA_SOUTH = 11762; //(ISO_ALPHA_3_KOR)
static const int KOREA_STANDARD_TIME = 32400; //(UTC + 9Hour)

DrPositionProviderGen3::DrPositionProviderGen3()
    : m_drposition(0)
    , m_start(false)
    , m_extAgentServer(0)
    , m_useGpsLog(false)
    , m_pPositionReceive(NULL)
    , m_RGData(0)
    , m_useSaveMMFBLog(false)
{
    gnss_socket_client_init();
    extAgentType::registerMetaTypes();

    m_pPositionReceive = new PositionReceive;
    m_pLogFileMMFB = NULL;
    m_pLogFilePosition = NULL;
}

DrPositionProviderGen3::~DrPositionProviderGen3()
{
    if (m_drposition) {
        delete m_drposition;
        m_drposition = 0;
    }
    if(m_pPositionReceive)
    {
        delete m_pPositionReceive;
        m_pPositionReceive = NULL;
    }
    if(m_pLogFileMMFB)
    {
        delete m_pLogFileMMFB;
        m_pLogFileMMFB = NULL;
    }
    if(m_pLogFilePosition)
    {
        delete m_pLogFilePosition;
        m_pLogFilePosition = NULL;
    }
}

void DrPositionProviderGen3::startUpdates()
{
    if (m_start) {
        return;
    }

    QDBusConnection c = QDBusConnection::sessionBus();
    if (!c.isConnected()) {
        qWarning("dbus not connected.");
        return;
    }

    m_start = true;

    // If it is registered already, setup right away
    if (c.interface()->isServiceRegistered(DBUS_SERVICE_DRPS)) {
        onDrPositionRegistered();
    } else {
        qDebug() << "Waiting for service:" << DBUS_SERVICE_DRPS;
        QDBusServiceWatcher *drPositionWatcher = new QDBusServiceWatcher(DBUS_SERVICE_DRPS,
                c, QDBusServiceWatcher::WatchForRegistration, this);
        connect(drPositionWatcher, SIGNAL(serviceRegistered(QString)), this, SLOT(onDrPositionRegistered()));
    }

    // gps.log가 없을 때 타겟에서 아래 함수를 호출 하면 비정상 동작함.
    // 이미지 및 DB열기 실패.
    // 19.01.03 ymhong.
    //gnss_socket_client_start(gnssSocketClientCallback, this);

    QDBusConnection extConn = QDBusConnection::sessionBus();
    if (!extConn.isConnected()) {
        qWarning("dbus not connected.");
        return;
    }

    // If it is registered already, setup right away
    if (extConn.interface()->isServiceRegistered(SERVICE_NAME)) {
        onExtRegistered();
    } else {
        qDebug() << "Waiting for service:" << SERVICE_NAME;
        QDBusServiceWatcher *serviceWatcher = new QDBusServiceWatcher(SERVICE_NAME,
                extConn, QDBusServiceWatcher::WatchForRegistration, this);
        connect(serviceWatcher, SIGNAL(serviceRegistered(QString)), this, SLOT(onExtRegistered()));
    }
}

void DrPositionProviderGen3::stopUpdates()
{
    if (!m_start) {
        return;
    }

    gnss_socket_client_stop();

    if (m_drposition) {
        delete m_drposition;
        m_drposition = 0;
    }

    m_start = false;
}

void DrPositionProviderGen3::sendExtraCommand(const QString &cmd)
{
    Q_UNUSED(cmd)
}

void DrPositionProviderGen3::changeGpsLogUsage(bool enabled)
{
    m_useGpsLog = enabled;
}
void DrPositionProviderGen3::applyGpsTimeBias(int bias) {
    qDebug() << "DrPositionProviderGen3::applyGpsTimeBias bias: " << bias;
    m_pPositionReceive->SetGpsTimeBias(bias);
}

#ifdef TMAP_TMC_QC_TESTING
void DrPositionProviderGen3::changeMMFeedback(bool enabled)
{
   m_useMMFB = enabled;
}

void DrPositionProviderGen3::changeOnlyTunnelMMFB(bool enabled)
{
    m_useOnlyTunnelMMFB = enabled;
}

void DrPositionProviderGen3::changeMMFeedbackLog(bool enabled)
{
    m_useSaveMMFBLog = enabled;
}

void DrPositionProviderGen3::changeMMFeedbackGpsMethod(int method)
{
    m_MmfbGpsMethod = method;
}

void DrPositionProviderGen3::changeMMFeedbackAngleOnTunnel(int method)
{
    m_MmfbAngleOnTunnel = method;
}

void DrPositionProviderGen3::changeMMFeedbackDRCep(bool enabled)
{
    m_MmfbDrCEP = enabled;
}

void DrPositionProviderGen3::changeMMFeedbackDRCepRange(int method)
{
    m_MmfbDrCEPrange = method;
}

void DrPositionProviderGen3::changeMMFeedbackDRCepMMRange(int method)
{
//    m_MmfbDrCEPMMrange = method;
}

void DrPositionProviderGen3::convertGpsLogToKML()
{
    // TODO something
}

int DrPositionProviderGen3::getGpsMethod()
{
    return m_MmfbGpsMethod;
}

void DrPositionProviderGen3::setLogPath(QString path)
{
    if(m_pLogFilePosition != NULL)
        delete m_pLogFilePosition;
    m_pLogFilePosition = new LogFile(eLogFileType_GPS, path);

    if(m_pLogFileMMFB != NULL)
        delete m_pLogFileMMFB;
    m_pLogFileMMFB = new LogFile(eLogFileType_MMFB, path);
}


#endif

void DrPositionProviderGen3::onDrPositionRegistered()
{
    if (!m_start) {
        return;
    }

    QDBusConnection c = QDBusConnection::sessionBus();
    if (!c.isConnected()) {
        qWarning("dbus not connected.");
        return;
    }

    if (m_drposition) {
        return;
    }

    m_drposition = new ComBoschDrPositionServiceNavigationInterface(DBUS_SERVICE_DRPS, DBUS_PATH_DRPS, c);
    if (!m_drposition->isValid()) {
        qWarning() << "Cannot connect to service " << DBUS_SERVICE_DRPS;
        return;
    }

    qDebug() << "success conn" << DBUS_SERVICE_DRPS;

    connect(m_drposition, &ComBoschDrPositionServiceNavigationInterface::ProxyDeadReckPosition,
            this, &DrPositionProviderGen3::onProxyDeadReckPosition);
    connect(m_drposition, &ComBoschDrPositionServiceNavigationInterface::ProxyMountingAngles,
            this, &DrPositionProviderGen3::onProxyMountingAngles);
    connect(m_drposition, &ComBoschDrPositionServiceNavigationInterface::ProxyPositionGNSS,
            this, &DrPositionProviderGen3::onProxyPositionGNSS);
    connect(m_drposition, &ComBoschDrPositionServiceNavigationInterface::ProxyDetailsGNSS,
            this, &DrPositionProviderGen3::onProxyDetailsGNSS);

    QDBusPendingReply<VersionInfo> positionVersion = m_drposition->GetPositionVersion();
    positionVersion.waitForFinished();
    if (positionVersion.isError()) {
        qWarning() << "GetPositionVersion failed:" << positionVersion.error();
    } else {
        VersionInfo version = positionVersion.value();
        qDebug() << QString("GetPositionVersion: %1.%2.%3 %4")
                    .arg(version.m_major).arg(version.m_minor).arg(version.m_micro)
                    .arg(version.m_date);
    }

    bool extAgentEnabled = qgetenv("TMAP_ENABLE_EXTAGNET_SERVER").toInt();
    extAgentEnabled = true;
    if (extAgentEnabled) {
        qDebug() << "Registering extAgentServer...";
        m_extAgentServer = new OrgBoschCmNavigationNavigationExtAgentInterface();

        // set properties

        // register a service

        m_extAgentServer->registerService();
    }
}

void DrPositionProviderGen3::onExtRegistered() {
    QDBusConnection c = QDBusConnection::sessionBus();
    if (!c.isConnected()) {
        qWarning("dbus not connected.");
        return;
    }

    m_navigationExt = new OrgBoschCmNavigationNavigationExtServiceInterface(SERVICE_NAME, NAVIGATIONEXT_PATH, c);
    if (!m_navigationExt->isValid()) {
        qWarning("Cannot connect to service NavigationExtService");
        return;
    }

}

void DrPositionProviderGen3::onProxyDeadReckPosition(uint Timestamp, uint TimePosix, double Longitude, double Latitude, int PosErrCepLarge, int PosErrCepSmall, int PosErrCepHeading, int Heading, int HeadingError, double Turnrate, double TurnrateError, short Speed, short SpeedError, short Acceleration, short AccelerationError, int Height, int HeightError, ushort SensorFlags, ushort DrInfoValidity)
{
    if (!m_start) {
        return;
    }

    LocationDataPtr location = LocationDataPool::newDataPtr();
    bool bRet = m_pPositionReceive->InpuDeadReckPosition(Timestamp, TimePosix, Longitude, Latitude, PosErrCepLarge, PosErrCepSmall, PosErrCepHeading, Heading, HeadingError, Turnrate, TurnrateError, Speed, SpeedError, Acceleration, AccelerationError, Height, HeightError, SensorFlags, DrInfoValidity, location);
#ifdef DRPOSITION_VERBOSE_LOG
    if (m_useGpsLog && m_pLogFilePosition)
    {
        QString strLog;
        QTextStream out(&strLog);
        out << Timestamp            << ","
            << location->dateTime().toString("yyyy-M-d H:m:s.zzz") << ","
            << QString::number(Longitude, 'f', 8) << ","
            << QString::number(Latitude, 'f', 8) << ","
            << PosErrCepLarge       << ","
            << PosErrCepSmall       << ","
            << PosErrCepHeading     << ","
            << Heading              << ","
            << HeadingError         << ","
            << QString::number(Turnrate, 'f', 8) << ","
            << QString::number(TurnrateError, 'f', 8) << ","
            << Speed                << ","
            << SpeedError           << ","
            << Acceleration         << ","
            << AccelerationError    << ","
            << Height               << ","
            << HeightError          << ","
            << QString("0x%1").arg(SensorFlags, 4, 16, QLatin1Char('0')) << ","
            << QString("0x%1").arg(DrInfoValidity, 4, 16, QLatin1Char('0'));

        m_pLogFilePosition->WriteString("ProxyDeadReckPosition", strLog);
    } // use gps log
#endif

    if(bRet)
    {
        emit positionUpdated(location);
    }

    qulonglong ullTimePosix = TimePosix;
    qulonglong TimePosixMillisec = ullTimePosix * 1000;             // DR에서 들어오는 값은 sec 단위이고 deadreckoning_info signal로 보내야 하는 값은 msec 단위이다
    double fMPerSec = static_cast<double>(Speed) / 100.0;           // DR에서 들어오는 값은 cm/s 단위이고 deadreckoning_info signal로 보내야 하는 값은 m/s 단위이다
    ushort nKmPerHour = static_cast<ushort>(Speed * 0.036);         // DR에서 들어오는 값은 cm/s 단위이고 velocity signal로 보내야 하는 값은 km/h 단위이다

//    qDebug() << "hhtest " << TimePosix << ", " << ullTimePosix << ", " << TimePosixMillisec << ", " << sizeof(long long) << ", " << sizeof(quint64) << ", " << sizeof(qulonglong);

    emit m_extAgentServer->deadreckoning_info(TimePosixMillisec, Timestamp, Latitude, Longitude, Heading, fMPerSec, Height, 0, 0, Turnrate, Acceleration, 0, 0, drInfoSource::DR_INFO_SOURCE_DR, DrInfoValidity);
    emit m_extAgentServer->velocity(nKmPerHour, velocitiySource::VELOCITY_SOURCE_DR);
    TimeZoneData tz;
        tz.TimeZoneDifference = KOREA_STANDARD_TIME;
        tz.DSTDifference = 0;
        tz.DST_State = 0;
    emit m_extAgentServer->position(Latitude, Longitude, posSource::POSITION_SOURCE_DR, KOREA_SOUTH, countryCodeSource::COUNTRY_CODE_SOURCE_MAP, tz, countryCodeSource::COUNTRY_CODE_SOURCE_MAP);
}

void DrPositionProviderGen3::onProxyMountingAngles(uchar AngRX, uchar AngRY, uchar AngRZ, uchar AngSX, uchar AngSY, uchar AngSZ, uchar AngTX, uchar AngTY, uchar AngTZ, uchar Status, uint Reserved1, uint Reserved2)
{
    MountAngles angles;
    angles.AngRX = AngRX;
    angles.AngRY = AngRY;
    angles.AngRZ = AngRZ;
    angles.AngSX = AngSX;
    angles.AngSY = AngSY;
    angles.AngSZ = AngSZ;
    angles.AngTX = AngTX;
    angles.AngTY = AngTY;
    angles.AngTZ = AngTZ;
//    qDebug() << "before send mounting::" << AngRX << AngRY << AngRZ << AngSX << AngSY << AngSZ << AngTX << AngTY << AngTZ << Status;
    emit m_extAgentServer->mounting(angles, Status);
}

void DrPositionProviderGen3::onProxyPositionGNSS(uint Timestamp, ushort Year, uchar Month, uchar Day, uchar Hour, uchar Minute, uchar Second, uchar FixStatus, double Longitude, double Latitude, short Altitude, double PDOP, double HDOP)
{
    int month = static_cast<int>(Month);
    int day = static_cast<int>(Day);
    int hour = static_cast<int>(Hour);
    int minute = static_cast<int>(Minute);
    int second = static_cast<int>(Second);
    int fixStatus = static_cast<int>(FixStatus);

    LocationDataPtr location = LocationDataPool::newDataPtr();
    bool bRet = m_pPositionReceive->InputGNSSPosition(Timestamp, Year, Month, Day, Hour, Minute, Second, FixStatus, Longitude, Latitude, Altitude, PDOP, HDOP, location);

#ifdef DRPOSITION_VERBOSE_LOG
    if (m_useGpsLog && m_pLogFilePosition)
    {
        QString strLog;
        QTextStream out(&strLog);
        out << Timestamp       << ","
               << Year            << ","
               << month           << ","
               << day             << ","
               << hour            << ","
               << minute          << ","
               << second          << ","
               << fixStatus       << ","
               << QString::number(Longitude, 'f', 8) << ","
               << QString::number(Latitude, 'f', 8) << ","
               << Altitude        << ","
               << QString::number(PDOP, 'f', 4) << ","
               << QString::number(HDOP, 'f', 4);

        m_pLogFilePosition->WriteString("ProxyPositionGNSS", strLog);
    }
#endif

    int nGpsMethod = getGpsMethod();
    if(bRet && (nGpsMethod == 1 || nGpsMethod == 2))
    {
        emit positionUpdated(location);
    }
}

void DrPositionProviderGen3::onProxyDetailsGNSS(uint Timestamp, ushort SatellitesUsed, ushort Speed, ushort Heading, SvList SpaceVehicles)
{
//    qDebug() << "called DrPositionProviderGen3::onProxyDetailsGNSS";
//    qDebug() << "values: Timestamp(" << Timestamp << ") SatellitesUsed(" << SatellitesUsed << ") Speed(" << Speed << ") Heading(" << Heading << ")";
//    for (int ii = 0; ii < SpaceVehicles.size(); ii++) {
//        qDebug() << "SvList[" << ii << "] " << SpaceVehicles.at(ii).m_svID << SpaceVehicles.at(ii).m_status << SpaceVehicles.at(ii).m_azimuth << SpaceVehicles.at(ii).m_elevation << SpaceVehicles.at(ii).m_SNR;
//    }

    RsmSvList list;
    RSM_GNSS_CHANNEL gnssChannel;
    for (int i = 0; i < SpaceVehicles.size(); i++)
    {
        const GNSS_CHANNEL &sv = SpaceVehicles.at(i);
        gnssChannel.m_svID = sv.m_svID;
        gnssChannel.m_status = sv.m_status;
        gnssChannel.m_azimuth = sv.m_azimuth;
        gnssChannel.m_elevation = sv.m_elevation;
        gnssChannel.m_SNR = sv.m_SNR;
        list.append(gnssChannel);
    }

    SatelliteDataPtr satellite = SatelliteDataPool::newDataPtr();
    bool bRet = m_pPositionReceive->InputDetailsGNSS(Timestamp, SatellitesUsed, Speed, Heading, list, satellite);
    list.clear();

#ifdef DRPOSITION_VERBOSE_LOG
    if (m_useGpsLog && m_pLogFilePosition)
    {
        QString strLog;
        QTextStream out(&strLog);
        out << Timestamp       << ","
               << SatellitesUsed  << ","
               << Speed           << ","
               << Heading;

        for (int i = 0; i < SpaceVehicles.size(); i++)
        {
            const GNSS_CHANNEL &sv = SpaceVehicles.at(i);

            out << ","
                   << sv.m_svID       << ","
                   << sv.m_status  << ","
                   << sv.m_azimuth  << ","
                   << sv.m_elevation  << ","
                   << sv.m_SNR;
        }

        m_pLogFilePosition->WriteString("ProxyDetailsGNSS", strLog);
    }
#endif

    if(bRet)
    {
       emit satelliteUpdated(satellite);
    }
}

void DrPositionProviderGen3::onRouteGuidanceUpdated(SKT::RGDataPtr rgdata) {
    if (rgdata == NULL) {
        return;
    }
    m_RGData = rgdata;

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

        MMCandidateList m_MMCandidateList;
        MMCandidate m_MMCandidate;
        m_MMCandidate.m_deltaLatitude   = m_RGData->m_MapMatchingFeedbackInfo.m_Candidates[0].m_deltaLatitude;
        m_MMCandidate.m_deltaLongitude  = m_RGData->m_MapMatchingFeedbackInfo.m_Candidates[0].m_deltaLongitude;
        m_MMCandidate.m_probability     = m_RGData->m_MapMatchingFeedbackInfo.m_Candidates[0].m_probability;
        m_MMCandidate.m_segmentFlag     = m_RGData->m_MapMatchingFeedbackInfo.m_Candidates[0].m_segmentFlag;
        m_MMCandidate.m_segmentHeading  = m_RGData->m_MapMatchingFeedbackInfo.m_Candidates[0].m_segmentHeading;
        m_MMCandidate.m_streetWidth     = m_RGData->m_MapMatchingFeedbackInfo.m_Candidates[0].m_streetWidth;
        m_MMCandidateList.append(m_MMCandidate);

        m_MMCandidate.m_deltaLatitude   = m_RGData->m_MapMatchingFeedbackInfo.m_OffroadCandidate.m_deltaLatitude;
        m_MMCandidate.m_deltaLongitude  = m_RGData->m_MapMatchingFeedbackInfo.m_OffroadCandidate.m_deltaLongitude;
        m_MMCandidate.m_probability     = m_RGData->m_MapMatchingFeedbackInfo.m_OffroadCandidate.m_probability;
        m_MMCandidate.m_segmentFlag     = m_RGData->m_MapMatchingFeedbackInfo.m_OffroadCandidate.m_segmentFlag;
        m_MMCandidate.m_segmentHeading  = m_RGData->m_MapMatchingFeedbackInfo.m_OffroadCandidate.m_segmentHeading;
        m_MMCandidate.m_streetWidth     = m_RGData->m_MapMatchingFeedbackInfo.m_OffroadCandidate.m_streetWidth;

        m_drposition->MapMatchingFeedback(m_RGData->m_MapMatchingFeedbackInfo.m_Timestamp,
                                          m_MMCandidateList,
                                          m_MMCandidate
                                          );

        if (m_useSaveMMFBLog && m_pLogFileMMFB)
        {
            QString strLog;
            QTextStream out(&strLog);
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

            m_pLogFileMMFB->WriteString("MMFB", strLog);
        }
    }
    #endif

}


void DrPositionProviderGen3::parseGpsData(unsigned int timestamp, const char *buffer, size_t bufferLen)
{
    return;

    LocationDataPtr location = LocationDataPool::newDataPtr();
    bool bRet = m_pPositionReceive->InputGpsPosition(timestamp, buffer, bufferLen, location);

#ifdef DRPOSITION_VERBOSE_LOG
    if (m_useGpsLog && m_pLogFilePosition)
    {
        if (bufferLen)
        {
            QString strLog;
            QTextStream out(&strLog);
            out << timestamp << "\n";
            out << buffer;

            m_pLogFilePosition->WriteString("sock_gnss", strLog);
        }
    }
#endif

    if(bRet)
    {
        int nGpsMethod = getGpsMethod();
        if(nGpsMethod == 0)
        {
            emit positionUpdated(location);
        }

        SatelliteDataPtr satellite = m_pPositionReceive->GetSatellite();
        if (satellite) {
            emit satelliteUpdated(satellite);
        }
    }
}

void DrPositionProviderGen3::dumpNmeaPacket(unsigned int timestamp, const char *data, size_t size)
{
    QByteArray bytes = QByteArray::fromRawData(data, size);

    QBuffer buffer(&bytes);
    if (!buffer.open(QIODevice::ReadOnly)) {
        return;
    }

    forever {
        QByteArray line = buffer.readLine();
        if (line.isNull()) {    // end of stream
            break;
        }

        qDebug() << "NMEA: timestamp" << timestamp << line;
    }

    buffer.close();
}

void DrPositionProviderGen3::gnssSocketClientCallback(unsigned int timestamp, const char *buffer, size_t bufferLen, void *userdata)
{
        // qDebug() << "gnssSocketClientCallback";
    DrPositionProviderGen3 *provider = reinterpret_cast<DrPositionProviderGen3*>(userdata);
    if (provider) {
        provider->parseGpsData(timestamp, buffer, bufferLen);
    }
}

QString DrPositionProviderGen3::getStringFromUnsignedChar(uchar *str) {
    qDebug() << "getStringFromUnsignedChar_in: " << *str ;

    QString result = "";
    int lengthOfString = strlen(reinterpret_cast<const char*>(str));
    qDebug() << "lengthOfString: " << lengthOfString ;
//    int v = strlen(str);
//    qDebug() << "v: " << v ;
    int c = strlen((char*)str);
    qDebug() << "c: " << c ;


    QString s;
    for(int i = 0; i < lengthOfString; i++) {
        s = QString( "%1" ).arg( str[i], 0, 16 );
        if( s.length() == 1 )
            result.append( "0" );

        result.append( s );
    }

    qDebug() << "getStringFromUnsignedChar_out: " << *str ;
    if (result.isEmpty()) {
        result = "00";
    }
    return result;
}

void DrPositionProviderGen3::changeOpenLRLog(bool)
{
}

}
