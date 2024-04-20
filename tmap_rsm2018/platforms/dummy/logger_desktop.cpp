#include "logger.h"
#include <QDomDocument>
#include <QFile>
#include <QFileSystemWatcher>
#include "logger_desktop.h"

namespace SKT {

void LoggerDesktop_MessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg);

class LoggerFilterWatcher;

LoggerDesktop::LoggerDesktop()
    : mFilterWatcher(new LoggerFilterWatcher(this, nullptr))
{
    LoggerDesktop::m_instance = this;
}

LoggerDesktop* LoggerDesktop::instance()
{
    return LoggerDesktop::m_instance;
}

void LoggerDesktop::init()
{
    if (qgetenv("TMAP_LOG_FILTER_ENABLE").toInt() == 1) {
        if (qgetenv("TMAP_LOG_FILTER_DISABLE_STDOUT").toInt() == 1) {
            freopen("/dev/null", "w", stdout);
        }

        auto filePath = qgetenv("TMAP_LOG_FILTER_FILEPATH");
        loadFilter(filePath);        
        mFilterWatcher->setPath(filePath);

        m_oldMessageHandler = qInstallMessageHandler(LoggerDesktop_MessageHandler);
    }
}

void LoggerDesktop::cleanUp()
{
    qInstallMessageHandler(m_oldMessageHandler);
}

void LoggerDesktop::messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg) {
    auto txt = qFormatLogMessage(type, context, msg);
    if (match(Message( context.category, txt, type ))) {
        m_oldMessageHandler( type, context, msg );
    }
}

bool LoggerDesktop::Filter::match(const LoggerDesktop::Message& message)
{
    if (enableRegExpContext) {
        if (enableContextId && contextRegExp.indexIn(message.contextId) < 0) {
            return false;
        }
    }
    else {
        if (enableContextId && !(message.contextId.contains(contextId))) {
            return false;
        }
    }

    if (enableRegExpPayload) {
        if (enablePayloadText && payloadRegExp.indexIn(message.payload) < 0) {
            return false;
        }
    }
    else {
        if (enablePayloadText && !(message.payload.contains(payloadText,ignoreCasePayload?Qt::CaseInsensitive:Qt::CaseSensitive))) {
            return false;
        }
    }

    if (enableLogLevelMax && !((int)message.type <= logLevelMax)) {
        return false;
    }

    if (enableLogLevelMin && !((int)message.type >= logLevelMin)) {
        return false;
    }

    return true;
}

LoggerDesktop* LoggerDesktop::m_instance = nullptr;

void LoggerDesktop::loadFilter(const QString& fileName)
{
    m_positiveFilters.clear();
    m_negativeFilters.clear();

    QDomDocument doc;
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly) || !doc.setContent(&file)) {
        return;
    }

    auto filterElemes = doc.elementsByTagName("filter");
    for (int i = 0; i < filterElemes.size(); i++) {
        Filter filter;
        auto filterElem = filterElemes.item(i);
        filter.type = filterElem.firstChildElement("type").text().toInt();
        filter.contextId = filterElem.firstChildElement("contextid").text();
        filter.payloadText = filterElem.firstChildElement("payloadtext").text();
        filter.enableRegExpContext = filterElem.firstChildElement("enableregexp_Context").text().toInt();
        filter.enableRegExpPayload = filterElem.firstChildElement("enableregexp_Payload").text().toInt();
        filter.ignoreCasePayload = filterElem.firstChildElement("ignoreCase_Payload").text().toInt();
        filter.enableFilter = filterElem.firstChildElement("enablefilter").text().toInt();
        filter.enableContextId = filterElem.firstChildElement("enablecontextid").text().toInt();
        filter.enablePayloadText = filterElem.firstChildElement("enablepayloadtext").text().toInt();
        filter.enableLogLevelMin = filterElem.firstChildElement("enableLogLevelMin").text().toInt();
        filter.enableLogLevelMax = filterElem.firstChildElement("enableLogLevelMax").text().toInt();
        filter.logLevelMax = filterElem.firstChildElement("logLevelMax").text().toInt();
        filter.logLevelMin = filterElem.firstChildElement("logLevelMin").text().toInt();

        filter.payloadRegExp.setPattern(filter.payloadText);
        filter.contextRegExp.setPattern(filter.contextId);
        filter.payloadRegExp.setCaseSensitivity(filter.ignoreCasePayload?Qt::CaseInsensitive:Qt::CaseSensitive);

        if (filter.enableFilter) {
            if (filter.type == 0) {
                m_positiveFilters.append(filter);
            }
            else {
                m_negativeFilters.append(filter);
            }
        }
    }
}

bool LoggerDesktop::match(const Message& message)
{
    bool ret = false;

    if (m_positiveFilters.isEmpty()) {
        ret = true;
    }
    else {
        for (auto filter : m_positiveFilters) {
            if (filter.match(message)) {
                ret = true;
                break;
            }
        }
    }

    if (ret) {
        if (!m_negativeFilters.isEmpty()) {
            for (auto filter : m_negativeFilters) {
                if (filter.match(message)) {
                    ret = false;
                    break;
                }
            }
        }
    }

    return ret;
}

PlatformLogger* Logger::newPlatformLogger()
{
    return new LoggerDesktop;
}

void LoggerDesktop_MessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    LoggerDesktop::instance()->messageHandler(type, context, msg);
}

LoggerFilterWatcher::LoggerFilterWatcher(LoggerDesktop* logger, QObject* parent)
    : QObject(parent),
    m_filterWatcher(new QFileSystemWatcher),
    m_logger(logger) {
}

void LoggerFilterWatcher::setPath(const QString& path)
{
    m_filterWatcher->addPath(path);

    connect(m_filterWatcher.data(), SIGNAL(fileChanged(const QString &)), this, SLOT(onFileChanged(const QString &)));
}

void LoggerFilterWatcher::onFileChanged(const QString& path)
{
    m_logger->loadFilter(path);
}

}
