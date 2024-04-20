#include <QDebug>
#include <QDir>
#include <QDateTime>
#include <QMutex>

#include "logger.h"
#include "environment.h"

namespace SKT {

class FileLogger : public PlatformLogger
{
public:
    static FileLogger* instance() { return s_instance; }

    FileLogger() : m_fp(0), m_mutex(QMutex::Recursive) {
        s_instance = this;
    }

    void init() override {
        QString path = Environment::getLogPath() + QDir::separator() +
                QString("log_%1.log").arg(QDateTime::currentDateTime().toString("yyyyMMddHHmmss"));
        m_fp = fopen(path.toLatin1().constData(), "w+t");
        qInstallMessageHandler(qtMessageHandler);
    }

    void cleanUp() override {
        qInstallMessageHandler(0);
        if (m_fp) {
            fclose(m_fp);
            m_fp = 0;
        }
    }

private:
    void messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg) {
        QMutexLocker locker(&m_mutex);
        if (m_fp) {
            QString txt = qFormatLogMessage(type, context, msg);
            fprintf(m_fp, "%s\n", txt.toUtf8().constData());
            fflush(m_fp);
        }
    }

    static void qtMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg) {
        instance()->messageHandler(type, context, msg);
    }

    static FileLogger* s_instance;
    FILE *m_fp;
    QMutex m_mutex;
};

FileLogger* FileLogger::s_instance = 0;

class ConsoleLogger : public PlatformLogger
{
public:
    void init() override {
        qInstallMessageHandler(messageHandler);
    }

    void cleanUp() override {
        qInstallMessageHandler(0);
    }

private:
    static void messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg) {
        QString txt = qFormatLogMessage(type, context, msg);
        fprintf(stdout, "%s\n", txt.toUtf8().constData());
        fflush(stdout);
    }
};

Logger::Logger()
{
    int logger = qgetenv("TMAP_LOGGER").toInt();
    if (logger == 1) {
        m_platformLogger = new ConsoleLogger;
    } else if (logger == 2) {
        m_platformLogger = new FileLogger;
    } else {
        m_platformLogger = newPlatformLogger();
    }
}

Logger::~Logger()
{
    delete m_platformLogger;
}

void Logger::init()
{
    QString pattern = qgetenv("TMAP_LOG_MESSAGE_PATTERN");
    if (pattern.isEmpty()) {
        pattern = QLatin1String("[%{category}][%{type}]%{threadid} %{time process} %{message}");
    }
    qSetMessagePattern(pattern);
    m_platformLogger->init();
}

void Logger::cleanUp()
{
    m_platformLogger->cleanUp();
}

}
