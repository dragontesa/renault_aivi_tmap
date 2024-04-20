#include <QDir>

#include "environment.h"

namespace SKT {

static Environment::DeviceType deviceType = Environment::QuadPlus;
static const QString staticRoot      = QStringLiteral("/opt/bosch/navigation_kor");
static const QString dynamicRoot     = QStringLiteral("/var/opt/bosch/dynamic/navigation_kor");
static const QString mapRoot         = QStringLiteral("/var/opt/bosch/navdata/navigation_kor_a");
static const QString onlineMapRoot   = QStringLiteral("/var/opt/bosch/navdata/online_streaming");
static const QString updatePartition = QStringLiteral("/var/opt/bosch/navdata/update");
static const QString dataRoot        = QStringLiteral("/var/opt/bosch/navdata/data");
static const QString logRoot         = QStringLiteral("/var/opt/bosch/navdata/log");
static const QString vrSymLink       = QStringLiteral("/var/opt/bosch/navdata/cryptnav");

static const QString resPath         = staticRoot + "/res";
static const QString preferencePath  = dynamicRoot + "/shared_prefs";
static const QString databasePath    = dynamicRoot + "/databases";
static const QString rgDataPath      = dataRoot + "/rg";
static const QString dataPath        = dynamicRoot + "/data";
static const QString routePath       = dynamicRoot + "/data/routes";
static const QString gpsPath         = logRoot + "/file_gps";
static const QString sysGpsPath      = logRoot + "/sys_gps";
static const QString routesearchPath = dynamicRoot + "/routesearch";
static const QString mapDataPath     = mapRoot + "/mapdata";
static const QString mapUpdatePath   = mapRoot + "/update";
static const QString adasDataPath = mapRoot + "/adas";
static const QString adasUpdatePath = mapRoot + "/adasupdate";

static const QString onlineMapPath   = onlineMapRoot + "/mapdata";
static const QString onlineAdasPath = onlineMapRoot + "/adasdata";
//static const QString tempPath        = dynamicRoot + "/tmp"; //Change voice play interface
static const QString tempPath = "/tmp/shared/extnav";
static const QString logPath         = logRoot + "/app_log";
static const QString screenshotPath  = logRoot + "/screenshot";
static const QString vrPath          = mapDataPath + "/VR";

void Environment::detectDeviceType()
{
    QFile file("/proc/device-tree/compatible");
    if (file.open(QIODevice::ReadOnly)) {
        QTextStream ts(&file);
        auto text = ts.readAll();
        qDebug() << "[Environment]compatible: " << text;
        //fsl,imx6q-nissan-aivi-s2-b2a-4gb\u0000fsl,imx6q\u0000
        if (text.indexOf("fsl,imx6q-nissan-aivi-s2-b2a-4gb") > -1) {
            qDebug() << "[Environment]Quad";
            deviceType = Environment::Quad;
        }
        //else if (text.indexOf("fsl,imx6qp-nissan-aivi-s2-b2a") > -1) {
    }
}

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

Environment::DeviceType Environment::getDeviceType()
{
    return deviceType;
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

}//SKT
