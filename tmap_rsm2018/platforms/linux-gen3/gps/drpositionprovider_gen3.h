#ifndef DRPOSITIONPROVIDER_H
#define DRPOSITIONPROVIDER_H

#include "positionprovider.h"
#include "nmeadata.h"
#include "interface/navigationext/navigationextserviceinterface.h"
#include "interface/navigationext/navigationexttype.h"
#include "interface/extAgent/extagenttype.h"
#include "positionreceive.h"
#include "logfile.h"
#include "rgdatapool.h"
#include "../interface/deadreckoning/drpositiontype.h"

class ComBoschDrPositionServiceNavigationInterface;
class OrgBoschCmNavigationNavigationExtAgentInterface;
class OrgBoschCmNavigationNavigationExtServiceInterface;

namespace SKT {
enum posSource {
    POSITION_SOURCE_CAN = 1,
    POSITION_SOURCE_GNSS = 2,
    POSITION_SOURCE_DR = 3
};

enum countryCodeSource {
    COUNTRY_CODE_SOURCE_DONOTUSE = 0,
    COUNTRY_CODE_SOURCE_MAP = 2,
    COUNTRY_CODE_SOURCE_OTHER = 3
};

enum velocitiySource {
    VELOCITY_SOURCE_INVALID= 0,
    VELOCITY_SOURCE_ODOMETER= 1,
    VELOCITY_SOURCE_CAN = 2,
    VEOCITY_SOURCE_GNSS= 3,
    VELOCITY_SOURCE_DR= 4
};

enum drInfoSource {
    DR_INFO_SOURCE_INVALID= 0,
    DR_INFO_SOURCE_GNSS= 1,
    DR_INFO_SOURCE_DR= 2
};

class GpsService;
class DrPositionProviderGen3 : public PositionProvider
{
    Q_OBJECT
public:
    DrPositionProviderGen3();
    virtual ~DrPositionProviderGen3();
    OrgBoschCmNavigationNavigationExtAgentInterface *extAgentServer() {return m_extAgentServer; }

public slots:
    void startUpdates() override;
    void stopUpdates() override;
    void sendExtraCommand(const QString &cmd) override;
    void changeGpsLogUsage(bool enabled) override;
    void applyGpsTimeBias(int bias) override;
    void onRouteGuidanceUpdated(SKT::RGDataPtr rgdata) override;
    
    TMAP_TMC_QC__MMFB_OVERRIDE

private slots:
    void onDrPositionRegistered();
    void onExtRegistered();
    void onProxyDeadReckPosition(uint Timestamp, uint TimePosix, double Longitude, double Latitude, int PosErrCepLarge, int PosErrCepSmall, int PosErrCepHeading, int Heading, int HeadingError, double Turnrate, double TurnrateError, short Speed, short SpeedError, short Acceleration, short AccelerationError, int Height, int HeightError, ushort SensorFlags, ushort DrInfoValidity);
    void onProxyMountingAngles(uchar AngRX, uchar AngRY, uchar AngRZ, uchar AngSX, uchar AngSY, uchar AngSZ, uchar AngTX, uchar AngTY, uchar AngTZ, uchar Status, uint Reserved1, uint Reserved2);
    void onProxyPositionGNSS(uint Timestamp, ushort Year, uchar Month, uchar Day, uchar Hour, uchar Minute, uchar Second, uchar FixStatus, double Longitude, double Latitude, short Altitude, double PDOP, double HDOP);
    void onProxyDetailsGNSS(uint Timestamp, ushort SatellitesUsed, ushort Speed, ushort Heading, SvList SpaceVehicles);
private:
    void parseGpsData(unsigned int timestamp, const char* buffer, size_t bufferLen);
    static void dumpNmeaPacket(unsigned int timestamp, const char* data, size_t size);
    static void gnssSocketClientCallback(unsigned int timestamp, const char* buffer, size_t bufferLen, void *userdata);
    QString getStringFromUnsignedChar(uchar *str);

private:
    ComBoschDrPositionServiceNavigationInterface *m_drposition;
    OrgBoschCmNavigationNavigationExtAgentInterface *m_extAgentServer;

    PositionReceive* m_pPositionReceive;
    LogFile *m_pLogFilePosition;
    LogFile *m_pLogFileMMFB;

    QMutex m_logMutex;

    bool m_start;
    bool m_useGpsLog; // gps log usage , default is true, by lckxx 2018.1.3;
    TMAP_TMC_QC_MMFB_DECLARE  // mmfb usage, by lckxx 2018.4.18
    OrgBoschCmNavigationNavigationExtServiceInterface *m_navigationExt;
    RGDataPtr   m_RGData;
};

}


#endif // DRPOSITIONPROVIDER_H
