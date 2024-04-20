#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include <QString>

namespace SKT {

class Environment
{
public:
    static void init();

    enum DeviceType {
      Desktop = 0,
      Quad,
      QuadPlus
    };

    static void detectDeviceType();
    static DeviceType getDeviceType();

    static QString getResPath();
    static QString getPreferencePath();
    static QString getDatabasePath();
    static QString getRGDataPath();
    static QString getRoutePath();
    static QString getGpsLogPath();
    static QString getSysGpsLogPath();
    static QString getDataPath();
    static QString getRouteSearchPath();
    static QString getMapDataPath();
    static QString getMapUpdatePath();
    static QString getAdasDataPath();
    static QString getAdasUpdatePath();
    static QString getOnlineMapPath();
    static QString getOnlineAdasPath();
    static QString getMapUpdatePartition();
    static QString getTempPath();
    static QString getLogPath();
    static QString getScreenshotPath();
    static QString getVrPath();
    static QString getVrSymLink();
};

}

#endif // ENVIRONMENT_H
