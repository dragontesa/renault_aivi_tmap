#ifndef BUILD_H
#define BUILD_H

#include <QString>

namespace SKT {

class Build
{
public:
    static QString versionString();
    static QString buildId();

    static int versionMajor();
    static int versionMinor();
    static int versionMicro();

    static QString versionString2();
    static QString buildYear();
    static QString buildWeek();
    static QString buildDay();
    static QString buildEventCode();

private:
    Build();
    ~Build();
    Q_DISABLE_COPY(Build)
};

}

#endif // BUILD_H
