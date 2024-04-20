#include "logger.h"
#include <QDomDocument>
#include <QFile>
#include <QFileSystemWatcher>

namespace SKT {

void LoggerDesktop_MessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg);

class LoggerDesktop;

class LoggerFilterWatcher : public QObject
{
    Q_OBJECT
public:
    LoggerFilterWatcher(LoggerDesktop* logger, QObject* parent);

    void setPath(const QString& path);

private slots:
    void onFileChanged(const QString& path);

private:
    QScopedPointer<QFileSystemWatcher> m_filterWatcher;
    LoggerDesktop* m_logger = nullptr;
};

class LoggerDesktop : public PlatformLogger
{
public:
    LoggerDesktop();

    static LoggerDesktop* instance();

    void init() override;

    void cleanUp() override;

    void messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg);

    void loadFilter(const QString& fileName);

private:
    enum class Type {
        Fatal = 1,
        Error,
        Warn,
        Info,
        Debug,
        Verbose
    };

    struct Message {
        Message( const QString& contextId, const QString& payload , QtMsgType type )
            : contextId(contextId), payload(payload) {
            switch (type) {
            case QtDebugMsg:
                this->type = Type::Debug;
                break;
            case QtWarningMsg:
                this->type = Type::Warn;
                break;
            case QtCriticalMsg:
                this->type = Type::Error;
                break;
            case QtFatalMsg:
                this->type = Type::Fatal;
                break;
            case QtInfoMsg:
                this->type = Type::Info;
                break;
            }
        }
        QString contextId;
        QString payload;
        Type type;
    };

    struct Filter {
        int type = 0;
//        QString name;
//        QString ecuId;
//        QString applicationId;
        QString contextId;
//        QString headerText;
        QString payloadText;
        int enableRegExpContext = 0;
//        int enableRegExpHeader = 0;
        int enableRegExpPayload = 0;
//        int ignoreCaseHeader = 0;
        int ignoreCasePayload = 0;
        int enableFilter = 0;
//        int enableEcuId = 0;
//        int enableApplicationId = 0;
        int enableContextId = 0;
//        int enableHeaderText = 0;
        int enablePayloadText = 0;
//        int enableCtrlMsgs = 0;
        int enableLogLevelMin = 0;
        int enableLogLevelMax = 0;
//        int enableMarker = 0;
//        QString filterColour;
        int logLevelMax = 0;
        int logLevelMin = 0;
        QRegExp payloadRegExp;
        QRegExp contextRegExp;

        bool match(const Message& message);
    };

    bool match(const Message& message);

    static LoggerDesktop* m_instance;
    QList<Filter> m_positiveFilters;
    QList<Filter> m_negativeFilters;
    QtMessageHandler m_oldMessageHandler;    
    QScopedPointer<LoggerFilterWatcher> mFilterWatcher;
};

}

