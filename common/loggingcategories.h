#ifndef LOGGINGCATEGORIES_H
#define LOGGINGCATEGORIES_H

#include <QLoggingCategory>

namespace SKT {

enum LOG_LEVEL {
    LOG_INFO = 0,
    LOG_DEBUG,
    LOG_WARNING,
    LOG_CRITICAL,
    LOG_FATAL
};

}

#define DECLARE_LOG_HANDLER(category) \
    namespace SKT { Q_DECLARE_LOGGING_CATEGORY(category) } \
    extern "C" void category##_log_handler( int level, const char* s );

#ifndef QT_NO_DEBUG_OUTPUT

#define IMPLMENT_LOG_HANDLER(category,...) \
    namespace SKT { Q_LOGGING_CATEGORY(category,__VA_ARGS__) } \
    extern "C" { \
    void category##_log_handler( int level, const char* s ) { \
        switch( level ) { \
        case SKT::LOG_INFO: \
            qInfo(SKT::category).noquote() << s; \
            break; \
        case SKT::LOG_DEBUG: \
            qDebug(SKT::category).noquote() << s; \
            break; \
        case SKT::LOG_WARNING: \
            qWarning(SKT::category).noquote() << s; \
            break; \
        case SKT::LOG_CRITICAL: \
            qCritical(SKT::category).noquote() << s; \
            break; \
        case SKT::LOG_FATAL: \
            qFatal( s ); \
            break; \
        } \
    } \
    }

#else
#define IMPLMENT_LOG_HANDLER(category,...) \
    namespace SKT { Q_LOGGING_CATEGORY(category,__VA_ARGS__) } \
    extern "C" { \
    void category##_log_handler( int level, const char* s ) { \
        switch( level ) { \
        case SKT::LOG_INFO: \
            qInfo(SKT::category).noquote() << s; \
            break; \
        case SKT::LOG_DEBUG: \
            break; \
        case SKT::LOG_WARNING: \
            qWarning(SKT::category).noquote() << s; \
            break; \
        case SKT::LOG_CRITICAL: \
            qCritical(SKT::category).noquote() << s; \
            break; \
        case SKT::LOG_FATAL: \
            qFatal( s ); \
            break; \
        } \
    } \
    }

#endif

DECLARE_LOG_HANDLER(sys)
DECLARE_LOG_HANDLER(ui)
DECLARE_LOG_HANDLER(vsmnavi)
DECLARE_LOG_HANDLER(vsmmap)
DECLARE_LOG_HANDLER(ndds)
DECLARE_LOG_HANDLER(localeng)

#define T_PERF_LOG QT_NO_QDEBUG_MACRO
//#define T_PERF_LOG QMessageLogger(QT_MESSAGELOG_FILE, QT_MESSAGELOG_LINE, QT_MESSAGELOG_FUNC).debug

#endif // LOGGINGCATEGORIES_H
