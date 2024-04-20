#include <QDebug>
#include <dlt/dlt.h>
#include "logger.h"

#define DLT_APPID       "ENAV"

DLT_DECLARE_CONTEXT(mainDltContext)

namespace SKT {

class LoggerGen3 : public PlatformLogger
{
public:
    LoggerGen3() {}

    static void dltMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg) {
        QString txt = qFormatLogMessage(type, context, msg);
        DltLogLevelType logLevel;

        switch (type) {
        case QtInfoMsg:
            logLevel = DLT_LOG_INFO;
        case QtDebugMsg:
            logLevel = DLT_LOG_DEBUG;
            break;
        case QtWarningMsg:
            logLevel = DLT_LOG_WARN;
            break;
        case QtCriticalMsg:
            logLevel = DLT_LOG_ERROR;
            break;
        case QtFatalMsg:
            logLevel = DLT_LOG_FATAL;
            break;
        }

        DLT_LOG(mainDltContext, logLevel, DLT_UTF8(txt.toUtf8().constData()));
    }

    void init() override {
        DLT_REGISTER_APP(DLT_APPID, "T map Application");

#if 1 //def QT_DEBUG
        DLT_REGISTER_CONTEXT_LL_TS(mainDltContext, "main", "general context", DLT_LOG_OFF, DLT_TRACE_STATUS_OFF);
#else
        DLT_REGISTER_CONTEXT(mainDltContext, "main", "general context");
#endif

        // use type and time of DLT
        qSetMessagePattern("%{threadid} %{message}");

        // forward message to DLT
        qInstallMessageHandler(dltMessageHandler);
    }

    void cleanUp() override {
        qInstallMessageHandler(0);

        DLT_UNREGISTER_CONTEXT(mainDltContext);
        DLT_UNREGISTER_APP();
    }
};

PlatformLogger* Logger::newPlatformLogger()
{
    return new LoggerGen3;
}

}
