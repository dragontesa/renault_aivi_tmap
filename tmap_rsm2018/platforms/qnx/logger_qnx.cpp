#include <QDebug>
#include "logger.h"
#include "dlt/dlt_cpp_extension.hpp"

LOG_DEFINE_APP_IDS("NAVI", "Navi Service");
LOG_DECLARE_DEFAULT_CONTEXT(navi, "ENAV", "Navi Service Deault Context");

namespace SKT {

class LoggerQnx : public PlatformLogger
{
public:
    LoggerQnx() {}

    static void dltMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg) {
        QString txt = qFormatLogMessage(type, context, msg);
        //DltLogLevelType logLevel;

        switch (type) {
        default:
        case QtDebugMsg:
            //logLevel = DLT_LOG_DEBUG;
            LOGD() << txt.toLocal8Bit().constData();
            break;
        case QtWarningMsg:
            //logLevel = DLT_LOG_WARN;
            LOGW() << txt.toLocal8Bit().constData();
            break;
        case QtCriticalMsg:
            //logLevel = DLT_LOG_ERROR;
            LOGE() << txt.toLocal8Bit().constData();
            break;
        case QtFatalMsg:
            //logLevel = DLT_LOG_FATAL;
            LOGC() << txt.toLocal8Bit().constData();
            break;
        case QtInfoMsg:
            //logLevel = DLT_LOG_INFO;
            LOGI() << txt.toLocal8Bit().constData();
        }

        //DLT_LOG(mainDltContext, logLevel, DLT_UTF8(txt.toUtf8().constData()));
    }
    void init() override {
        // use type and time of DLT
        qSetMessagePattern("%{threadid} %{message}");

        // forward message to DLT
        qInstallMessageHandler(dltMessageHandler);
    }
    void cleanUp() override {
        qInstallMessageHandler(0);
    }
};

PlatformLogger* Logger::newPlatformLogger()
{
    return new LoggerQnx;
}

}

