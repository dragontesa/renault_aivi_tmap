#include <QDir>

#include "environment.h"

namespace SKT {

//static const QString staticRoot = QDir::cleanPath(QDir::currentPath() + "/..");
static const QString staticRoot = QStringLiteral("/ivinavi/SKT");
static const QString dynamicRoot = QStringLiteral("/ivinusr/");
static const QString mapRoot = QStringLiteral("/ivimap/SKT/map_root");
static const QString onlineMapRoot = QStringLiteral("/ivimcache/map_online");
static const QString updatePartition = QStringLiteral("/ivimap/SKT");
static const QString dataRoot = QStringLiteral("/ivinusr/data");
static const QString logRoot = QStringLiteral("/ivinusr/log");
static const QString vrSymLink       = QStringLiteral("/ivinusr/cryptnav");

static const QString resPath = staticRoot + "/res";
static const QString preferencePath = dynamicRoot + "/shared_prefs";
static const QString databasePath = dynamicRoot + "/databases";
static const QString dataPath = dataRoot;
static const QString routePath = dataRoot + "/routes";
static const QString gpsPath = logRoot + "/file_gps";
static const QString sysGpsPath = logRoot + "/sys_gps";
static const QString routesearchPath = dataRoot + "/routesearch";
static const QString mapDataPath = mapRoot + "/mapdata";
static const QString mapUpdatePath = mapRoot+ "/update";
static const QString onlineMapPath = onlineMapRoot + "/mapdata";
static const QString tempPath = dynamicRoot + "/tmp";
static const QString logPath = logRoot + "/app_log";
static const QString screenshotPath = logRoot + "/screenshot";
static const QString vrPath         = mapDataPath + "/VR";

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
    dir.mkpath(getDataPath());
    dir.mkpath(getRoutePath());
    dir.mkpath(getGpsLogPath());
    dir.mkpath(getRouteSearchPath());
    dir.mkpath(getTempPath());
    dir.mkpath(getLogPath());
    dir.mkpath(getSysGpsLogPath());
    dir.mkpath(getScreenshotPath());
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

QString Environment::getRoutePath()
{
    return routePath;
}

QString Environment::getGpsLogPath()
{
    return gpsPath;
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

QString Environment::getMapUpdatePartition()
{
    return updatePartition;
}

QString Environment::getOnlineMapPath()
{
    return onlineMapPath;
}

QString Environment::getTempPath()
{
    return tempPath;
}

QString Environment::getLogPath()
{
    return logPath;
}

QString Environment::getSysGpsLogPath()
{
    return sysGpsPath;
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
