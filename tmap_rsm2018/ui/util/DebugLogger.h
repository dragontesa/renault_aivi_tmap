#ifndef DEBUGLOGGERIMPL_H
#define DEBUGLOGGERIMPL_H

#ifdef QT_DEBUG
#define T_DEBUG(msg) SKT::DebugLogger::instance()->write( __FILE__, __LINE__, __FUNCTION__, msg, "[UI]" )
#define T_DEBUGV(...) SKT::DebugLogger::instance()->write( __FILE__, __LINE__, __FUNCTION__, QString().sprintf( __VA_ARGS__ ), "[UI]" )
#else
#define T_DEBUG(msg)
#define T_DEBUGV(...)
#endif

namespace SKT {

class DebugLoggerImpl;

class DebugLogger
{
public:
    static DebugLogger* instance();

    ~DebugLogger();

    void write( const QString& file, int line, const QString& func, const QJsonObject& jsonObj, const QString& tag = QString() );
    void write( const QString& file, int line, const QString& func, const QString& msg, const QString& tag = QString() );

private:
    DebugLogger();

    QScopedPointer<DebugLoggerImpl> mImpl;
};

}

#endif // DEBUGLOGGERIMPL_H
