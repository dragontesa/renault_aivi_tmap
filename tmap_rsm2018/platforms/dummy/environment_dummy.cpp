#include <QDir>

#include "environment.h"

#define ENV_RSM2018_PREFIX      "RSM2018_PREFIX"

namespace SKT {

static Environment::DeviceType deviceType = Environment::Desktop;
static const QString baseRoot = (qgetenv(ENV_RSM2018_PREFIX).isEmpty()) ?
            QDir::homePath() : QString::fromUtf8(qgetenv(ENV_RSM2018_PREFIX));
static const QString staticRoot      =       baseRoot + "/tmap_rsm2018/static";
static const QString dynamicRoot     =      baseRoot + "/tmap_rsm2018/dynamic/navigation_kor";
static const QString mapRoot         =          baseRoot + "/tmap_rsm2018/navdata/navigation_kor";
static const QString onlineMapRoot   =    baseRoot + "/tmap_rsm2018/navdata/online_streaming";
static const QString updatePartition =  baseRoot + "/tmap_rsm2018/navdata";
static const QString dataRoot        =         baseRoot + "/tmap_rsm2018/navdata/logdata";
static const QString vrSymLink       = baseRoot + "/tmap_rsm2018/navdata/cryptnav";

static const QString resPath         =          staticRoot + "/res";
static const QString preferencePath  =   dynamicRoot + "/shared_prefs";
static const QString databasePath    =   dynamicRoot + "/databases";
static const QString rgDataPath      =   dynamicRoot + "/rg";
static const QString dataPath        =         dataRoot + "/data";
static const QString routePath       =        dataRoot + "/data/routes";
static const QString gpsPath         =          dataRoot + "/file_gps";
static const QString sysGpsPath      =       dataRoot + "/sys_gps";

static const QString routesearchPath =  dataRoot + "/routesearch";
static const QString mapDataPath     =      mapRoot + "/mapdata";
static const QString mapUpdatePath   =    mapRoot + "/update";
static const QString adasDataPath = mapRoot + "/adas";
static const QString adasUpdatePath = mapRoot + "/adasupdate";
static const QString onlineMapPath   =    onlineMapRoot + "/mapdata";
static const QString onlineAdasPath = onlineMapRoot + "/adasdata";
static const QString tempPath        =         dataRoot + "/tmp";
static const QString logPath         =          dataRoot + "/log";
static const QString screenshotPath  =   dataRoot + "/screenshot";
static const QString vrPath          = mapDataPath + "/VR";

void Environment::init()
{
    // clear temporary directory
    QDir tempDir(getTempPath());
    tempDir.removeRecursively();

    // clear temporary preference files
    QDir prefDir(getPreferencePath());
    QStringList filters;
    filters << "*.ini.*";
    for (QFileInfo info : prefDir.entryInfoList(filters, QDir::Files)) {
        QFile::remove(info.absoluteFilePath());
    }

    // create directories
    QDir dir;
    dir.mkpath(getOnlineMapPath());
    dir.mkpath(getOnlineAdasPath());
    dir.mkpath(getPreferencePath());
    dir.mkpath(getDatabasePath());
    dir.mkpath(getRGDataPath());
    dir.mkpath(getDataPath());
    dir.mkpath(getRoutePath());
    dir.mkpath(getGpsLogPath());
    dir.mkpath(getSysGpsLogPath());
    dir.mkpath(getRouteSearchPath());
    dir.mkpath(getTempPath());
    dir.mkpath(getLogPath());
    dir.mkpath(getScreenshotPath());
}

void Environment::detectDeviceType()
{
}

Environment::DeviceType Environment::getDeviceType()
{
    return Desktop;
}

QString Environment::getResPath()
{
    return resPath;
}

QString Environment::getPreferencePath()
{
    return preferencePath;
}

QString Environment::getDatabasePath()
{
    return databasePath;
}

QString Environment::getRGDataPath()
{
    return rgDataPath;
}

QString Environment::getRoutePath()
{
    return routePath;
}

QString Environment::getGpsLogPath()
{
    return gpsPath;
}

QString Environment::getSysGpsLogPath()
{
    return sysGpsPath;
}

QString Environment::getDataPath()
{
    return dataPath;
}

QString Environment::getRouteSearchPath()
{
    return routesearchPath;
}

QString Environment::getMapDataPath()
{
    return mapDataPath;
}

QString Environment::getMapUpdatePath()
{
    return mapUpdatePath;
}

QString Environment::getAdasDataPath()
{
    return adasDataPath;
}

QString Environment::getAdasUpdatePath()
{
    return adasUpdatePath;
}

QString Environment::getMapUpdatePartition()
{
    return updatePartition;
}

QString Environment::getOnlineMapPath()
{
    return onlineMapPath;
}

QString Environment::getOnlineAdasPath()
{
    return onlineAdasPath;
}

QString Environment::getTempPath()
{
    return tempPath;
}

QString Environment::getLogPath()
{
    return logPath;
}

QString Environment::getScreenshotPath()
{
    return screenshotPath;
}

QString Environment::getVrPath()
{
    return vrPath;
}

QString Environment::getVrSymLink()
{
    return vrSymLink;
}

}
