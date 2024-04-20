#ifndef DRPOSITIONTEST_H
#define DRPOSITIONTEST_H

#include <QObject>
#include "drpositionserviceinterface.h"

namespace SKT {

class DrPositionTest : public QObject
{
    Q_OBJECT
public:
    DrPositionTest(QObject *parent=0);
    ~DrPositionTest();

    void init();
    void cleanUp();
    void runTest();

private slots:
    void onDeadReckPosition(uint Timestamp,
                            uint TimePosix,
                            double Longitude,
                            double Latitude,
                            int PosErrCepLarge,
                            int PosErrCepSmall,
                            int PosErrCepHeading,
                            int Heading,
                            int HeadingError,
                            double Turnrate,
                            double TurnrateError,
                            short Speed,
                            short SpeedError,
                            short Acceleration,
                            short AccelerationError,
                            int Height,
                            int HeightError,
                            ushort SensorFlags);

    void onPositionGNSS(uint Timestamp,
                        ushort Year,
                        uchar Month,
                        uchar Day,
                        uchar Hour,
                        uchar Minute,
                        uchar Second,
                        uchar FixStatus,
                        double Longitude,
                        double Latitude,
                        short Altitude,
                        double PDOP,
                        double HDOP);

private:
    ComBoschDrPositionServiceNavigationInterface *m_interface;
};

}

#endif // DRPOSITIONTEST_H
