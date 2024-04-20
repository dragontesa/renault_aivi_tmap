#include "build.h"
#include "tmapconfig.h"

namespace SKT {

QString Build::versionString2()
{
    return QString("%1.%2.%3%4").arg(TMAP_VERSION_MAJOR)
            .arg(TMAP_VERSION_MINOR)
            .arg(TMAP_VERSION_MICRO)
            .arg(TMAP_VERSION_BUILD_SEQ);
}

QString Build::versionString()
{
    return QString("SW%1%2.%3.%4")
            .arg(TMAP_VERSION_YEAR)
            .arg(TMAP_VERSION_WEEK)
            .arg(TMAP_VERSION_DAY)
            .arg(TMAP_VERSION_EVENTCODE);
}

QString Build::buildId()
{
    return QString(TMAP_BUILD_ID);
}

int Build::versionMajor()
{
    return QString(TMAP_VERSION_MAJOR).toInt();
}

int Build::versionMinor()
{
    return QString(TMAP_VERSION_MINOR).toInt();
}

int Build::versionMicro()
{
    return QString(TMAP_VERSION_MICRO).toInt();
}

QString Build::buildYear()
{
    return QString(TMAP_VERSION_YEAR);
}

QString Build::buildWeek()
{
    return QString(TMAP_VERSION_WEEK);
}

QString Build::buildDay()
{
    return QString(TMAP_VERSION_DAY);
}

QString Build::buildEventCode()
{
    return QString(TMAP_VERSION_EVENTCODE);
}
}
