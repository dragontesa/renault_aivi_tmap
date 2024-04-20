#include <QtDBus>
#include "drpositiontest.h"
#include "DrPosInterface.h"

namespace SKT {

DrPositionTest::DrPositionTest(QObject *parent)
    : QObject(parent)
    , m_interface(0)
{

}

DrPositionTest::~DrPositionTest()
{
    cleanUp();
}

void DrPositionTest::init()
{
    QDBusConnection c = QDBusConnection::sessionBus();
    if (!c.isConnected()) {
        qWarning("dbus not connected.");
        return;
    }

    m_interface = new ComBoschDrPositionServiceNavigationInterface(DBUS_SERVICE_DRPS,
                                                                   DBUS_PATH_DRPS,
                                                                   c);
}

void DrPositionTest::cleanUp()
{
    if (m_interface) {
        delete m_interface;
        m_interface = 0;
    }
}

void DrPositionTest::runTest()
{
    if (!m_interface) {
        return;
    }

    if (!m_interface->isValid()) {
        qWarning("Cannot connect to DrPosition service");
        return;
    }

    QDBusPendingReply<VersionInfo> r = m_interface->GetPositionVersion();
    r.waitForFinished();
    if (r.isError()) {
        qWarning() << "GetPositionVersion failed:" << r.error();
    } else {
        DrVersion v = r.value();
        qDebug() << QString("GetPositionVersion: $1.$2.$3 $4")
                    .arg(v.m_major).arg(v.m_minor).arg(v.m_micro)
                    .arg(v.m_date);
    }

    connect(m_interface, SIGNAL(ProxyPositionGNSS(uint,ushort,uchar,uchar,uchar,uchar,uchar,uchar,double,double,short,double,double)),
            this, SLOT(onPositionGNSS(uint,ushort,uchar,uchar,uchar,uchar,uchar,uchar,double,double,short,double,double));
    connect(m_interface, SIGNAL(ProxyDeadReckPosition(uint,uint,double,double,int,int,int,int,int,double,double,short,short,short,short,int,int,ushort, ushort)),
            this, SLOT(onDeadReckPosition(uint,uint,double,double,int,int,int,int,int,double,double,short,short,short,short,int,int,ushort, ushort));
}


void DrPositionTest::onProxyDeadReckPosition(uint Timestamp, uint TimePosix, double Longitude, double Latitude, int PosErrCepLarge, int PosErrCepSmall, int PosErrCepHeading, int Heading, int HeadingError, double Turnrate, double TurnrateError, short Speed, short SpeedError, short Acceleration, short AccelerationError, int Height, int HeightError, ushort SensorFlags, ushort DrInfoValidity);


void DrPositionTest::onProxyDeadReckPosition(uint Timestamp, uint TimePosix, double Longitude, double Latitude, int PosErrCepLarge, int PosErrCepSmall, int PosErrCepHeading, int Heading, int HeadingError, double Turnrate, double TurnrateError, short Speed, short SpeedError, short Acceleration, short AccelerationError, int Height, int HeightError, ushort SensorFlags){
    qDebug() << "onProxyDeadReckPosition";

//    std::cout << "Timestamp: "          << Timestamp << std::endl;
//    std::cout << "TimePosix: "          << TimePosix << std::endl;
//    std::cout << "Longitude: "          << Longitude << std::endl;
//    std::cout << "Latitude: "           << Latitude << std::endl;
//    std::cout << "PosErrCepLarge: "     << PosErrCepLarge << std::endl;
//    std::cout << "PosErrCepSmall: "     << PosErrCepSmall << std::endl;
//    std::cout << "PosErrCepHeading: "   << PosErrCepHeading << std::endl;
//    std::cout << "Heading: "            << Heading << std::endl;
//    std::cout << "HeadingError: "       << HeadingError << std::endl;
//    std::cout << "Turnrate: "           << Turnrate << std::endl;
//    std::cout << "TurnrateError: "      << TurnrateError << std::endl;
//    std::cout << "Speed: "              << Speed << std::endl;
//    std::cout << "SpeedError: "         << SpeedError << std::endl;
//    std::cout << "Acceleration: "       << Acceleration << std::endl;
//    std::cout << "AccelerationError: "  << AccelerationError << std::endl;
//    std::cout << "Height: "             << Height << std::endl;
//    std::cout << "HeightError: "        << HeightError << std::endl;
//    std::cout << "SensorFlags: "        << SensorFlags << std::endl;
}

void DrPositionTest::onPositionGNSS(uint Timestamp, ushort Year, uchar Month, uchar Day, uchar Hour, uchar Minute, uchar Second, uchar FixStatus, double Longitude, double Latitude, short Altitude, double PDOP, double HDOP)
{
    qDebug() << "onPositionGNSS";

//    std::cout << "Timestamp: " << Timestamp << std::endl;
//    std::cout << "Year: "      << Year << std::endl;
//    std::cout << "Month: "     << Month << std::endl;
//    std::cout << "Day: "       << Day << std::endl;
//    std::cout << "Hour: "      << Hour << std::endl;
//    std::cout << "Minute: "    << Minute << std::endl;
//    std::cout << "Second: "    << Second << std::endl;
//    std::cout << "FixStatus: " << FixStatus << std::endl;
//    std::cout << "Longitude: " << Longitude << std::endl;
//    std::cout << "Latitude: "  << Latitude << std::endl;
//    std::cout << "Altitude: "  << Altitude << std::endl;
//    std::cout << "PDOP: "      << PDOP << std::endl;
//    std::cout << "HDOP: "      << HDOP << std::endl;
}
}
