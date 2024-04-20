#ifndef CRASHLOGGER_H
#define CRASHLOGGER_H

#include <QFile>

namespace SKT {

typedef QMap<int, int> MAP_INDEX;       // key: start index, value: length

class CrashLogger
{
private:
    CrashLogger() {}
//    static CrashLogger *instance();

public:
    static bool run();

private:
    static bool parseCrashDump(QFile &file, QString &log);
    static bool saveCrashLog(QFile &file, const QString &log);
};

}

#endif // CRASHLOGGER_H
