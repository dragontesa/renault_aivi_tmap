/*!
    \class DebugLogger
    \brief 디버그 출력 필터링 및 저장
*/

#include "DebugLogger.h"
#include <QDebug>

namespace {

void MessageHandler( QtMsgType type, const QMessageLogContext &context, const QString &msg );

typedef QString (*MessageFormatter)( const QString& file, int line, const QString& func, const QString& msg, const QString& tag );

QString DefaultMessageFormatter( const QString& file, int line, const QString& func, const QString& msg, const QString& tag );

}

using namespace SKT;

namespace SKT {

class DebugLoggerImpl {
public:
    struct Filter {
        bool isInclusion = false;
        QString text;
        QString file;
        QString func;
        QString msg;
    };

    QString mFilterSeparator;
    QList<Filter> mInclusions;
    QList<Filter> mExclusions;
    bool mSave;
    QMap<QString,MessageFormatter> mMsgFormatters;
    MessageFormatter mMsgFormatter;
    bool mIncludeOtherLogs;
    bool mIncludeQtInfo;
    bool mIncludeQtWarn;
    bool mIncludeQtCritical;
    bool mIncludeQtFatal;
    bool mApplyFilterToOtherLogs;
    bool mApplyFilterToQtLogs;
    QtMessageHandler mDefaultMessageHandler;

    static DebugLoggerImpl* instance() {
        return DebugLoggerImpl::Instance;
    }

    DebugLoggerImpl() {
        DebugLoggerImpl::Instance = this;

        mFilterSeparator = QStringLiteral("::");
        mMsgFormatter = DefaultMessageFormatter;

        auto iniPath = QDir::homePath() + QChar('/') + QStringLiteral("tmap_rsm_ui_dbg.ini");
        QFile file( iniPath );
        auto exists = file.exists();
        if ( exists ) {
            // message formatter
            mMsgFormatters[QStringLiteral("default")] = DefaultMessageFormatter;

            // settings
            QSettings settings( iniPath, QSettings::IniFormat );
            mFilterSeparator = settings.value( QStringLiteral("debugLogFilterSeparator"), QStringLiteral("::") ).toString();

            auto trimStringList = [&](const QStringList& list)->QStringList{
                QStringList ret;
                for ( auto item : list ) {
                    item = item.trimmed();
                    if ( !item.isEmpty() ) {
                        ret.append( item );
                    }
                }
                return ret;
            };

            auto parseFilter = [&](bool isInclusion, const QString& item)->Filter{
                Filter ret;
                ret.text = item;
                ret.isInclusion = isInclusion;
                if ( item.indexOf( mFilterSeparator ) > -1 ) {
                    auto toks = item.split( mFilterSeparator );
                    if ( toks.count() > 0 ) {
                        ret.file = toks[0];
                    }
                    if ( toks.count() > 1 ) {
                        ret.func = toks[1];
                    }
                    if ( toks.count() > 2 ) {
                        ret.msg = toks[2];
                    }
                }
                else {
                    ret.msg = item;
                }
                return ret;
            };

            auto tempList = trimStringList( settings.value( QStringLiteral("debugLogInclusions") ).toStringList() );
            for ( auto item : tempList ) {
                auto filter = parseFilter( true, item );
                mInclusions.append( filter );
            }

            tempList = trimStringList( settings.value( QStringLiteral("debugLogExclusions") ).toStringList() );
            for ( auto item : tempList ) {
                auto filter = parseFilter( false, item );
                mExclusions.append( filter );
            }

            auto debugDisableStandOutput = settings.value( QStringLiteral("debugDisableStandOutput"), false ).toBool();
            if ( debugDisableStandOutput ) {
                freopen("/dev/null", "w", stdout);
            }

            auto msgFormatter = settings.value( QStringLiteral("debugLogMessageFormatter"), QStringLiteral("default") ).toString();
            mMsgFormatter = mMsgFormatters[msgFormatter];            
            mIncludeOtherLogs = settings.value( QStringLiteral("debugLogIncludeOtherLogs"), false ).toBool();
            mIncludeQtInfo = settings.value( QStringLiteral("debugLogIncludeQtInfo"), false ).toBool();
            mIncludeQtWarn = settings.value( QStringLiteral("debugLogIncludeQtWarning"), false ).toBool();
            mIncludeQtCritical = settings.value( QStringLiteral("debugLogIncludeQtCritical"), false ).toBool();
            mIncludeQtFatal = settings.value( QStringLiteral("debugLogIncludeQtFatal"), false ).toBool();
            mDefaultMessageHandler = qInstallMessageHandler( MessageHandler );
            mApplyFilterToOtherLogs = settings.value( QStringLiteral("debugLogApplyFilterToOtherLogs"), false ).toBool();
            mApplyFilterToQtLogs = settings.value( QStringLiteral("debugLogApplyFilterToQtLogs"), false ).toBool();
			
            mSave = settings.value( QStringLiteral("debugLogSave") ).toBool();
            if ( mSave ) {
                auto logPath = QDir::homePath() + QChar('/') + QStringLiteral("tmap_rsm_ui_dbg.log");
                TLogger::instance()->setSavePath( logPath );
            }
        }
    }

    QString messageFilter( const QString& file, int line, const QString& func, const QString& msg, const QString& tag ) {
        QString buf;

        bool doOutput = false;

        auto doFilter = [&](const Filter& filter)->bool{
            bool eval = false;
            if ( msg.indexOf( filter.text ) > -1 ) {
                eval = true;
            }
            else {
                if ( !filter.file.isEmpty() ) {
                    if ( file.indexOf( filter.file ) > -1 ) {
                        eval = true; // in the case where only file is specified and meets this filter
                                    // ex) Application::
                        if ( !filter.func.isEmpty() ) {
                            eval = false;
                            if ( func.indexOf( filter.func ) > -1 ) {
                                eval = true; // in the case where both file and function are specified and meet this filter
                                            // ex) Application::onViewTreeVisibilityChanged
                            }
                            if ( !filter.msg.isEmpty() ) {
                                eval = false;
                                if ( msg.indexOf( filter.msg ) > -1 ) {
                                    eval = true; // in the case where file, function and message are specified and meet this filter
                                                // ex) Application::onViewTreeVisibilityChanged::#3
                                }
                            }
                        }
                        else if ( !filter.msg.isEmpty() ) {
                            eval = false;
                            if ( msg.indexOf( filter.msg ) > -1 ) {
                                eval = true; // in the case where file and message are specified and meet this filter
                                            // ex) Application::::#3
                            }
                        }
                    }
                }
                else {
                    if ( !filter.msg.isEmpty() ) {
                        if ( msg.indexOf( filter.msg ) > -1 ) {
                            eval = true; // in the case where only message is specified and meets this filter
                                        // ex) Arrived!
                        }
                    }
                }
            }

            return eval;
        };

        if ( mInclusions.isEmpty() ) {
            doOutput = true; // in the case where there is no inclusion filter
        }
        else {
            for ( auto filter : mInclusions ) {
                bool eval = doFilter( filter );
                if ( eval ) {
                    doOutput = true;
                    break;
                }
            }
        }

        if ( !mExclusions.isEmpty() ) {
            for ( auto filter : mExclusions ) {
                bool eval = doFilter( filter );
                if ( eval ) {
                    doOutput = false;
                    break;
                }
            }
        }

        if ( doOutput ) {
            buf = mMsgFormatter( file, line, func, msg, tag );
        }

        return buf;
    }

private:
    static DebugLoggerImpl* Instance;
};

}

DebugLoggerImpl* DebugLoggerImpl::Instance = nullptr;

DebugLogger* DebugLogger::instance()
{
    static DebugLogger Instance;

    return &Instance;
}

DebugLogger::DebugLogger()
    : mImpl( new DebugLoggerImpl )
{    
}

DebugLogger::~DebugLogger()
{
}

void DebugLogger::write( const QString& file, int line, const QString& func, const QJsonObject& jsonObj, const QString& tag )
{
    // QJsonDocument는 key들을 알파벳 순서로 변경시킨다
    QJsonDocument doc(jsonObj);
    auto msg = doc.toJson(QJsonDocument::Indented);

    auto filtered = mImpl->messageFilter( file, line, func, msg, tag );
    if ( !filtered.isEmpty() ) {
        qDebug(ui).noquote() << filtered;
        SKT::TLogger::instance()->write( filtered );
    }
}

void DebugLogger::write( const QString& file, int line, const QString& func, const QString& msg, const QString& tag )
{
    auto filtered = mImpl->messageFilter( file, line, func, msg, tag );
    if ( !filtered.isEmpty() ) {
        qDebug(ui).noquote() << filtered;
        SKT::TLogger::instance()->write( filtered );
    }
}

namespace {

// message handler

void MessageHandler( QtMsgType type, const QMessageLogContext &context, const QString &msg )
{    
    QString buf = msg;
    QString tag;
    bool output = false;
    bool filter = false;

    switch(type) {
    case QtInfoMsg:
        if ( DebugLoggerImpl::instance()->mIncludeQtInfo ) {       
            if ( DebugLoggerImpl::instance()->mApplyFilterToQtLogs ) {
                filter = true;
            }
            tag = QStringLiteral("[info]");
            output = true;
        }
        break;
    case QtDebugMsg: {
#ifndef QT_NO_DEBUG_OUTPUT
            if ( context.category == ui().categoryName() ) {
                output = true;
            }
            else {
                if ( DebugLoggerImpl::instance()->mIncludeOtherLogs ) {
                    output = true;
                    if ( DebugLoggerImpl::instance()->mApplyFilterToOtherLogs ) {
                        filter = true;
                    }
                }
                else {
                    buf.clear();
                }
            }
#endif
        }
        break;
    case QtWarningMsg:
        if ( DebugLoggerImpl::instance()->mIncludeQtWarn ) {
            if ( DebugLoggerImpl::instance()->mApplyFilterToQtLogs ) {
                filter = true;
            }
            tag = QStringLiteral("[warn]");
            output = true;
        }
        break;
    case QtCriticalMsg:
        if ( DebugLoggerImpl::instance()->mIncludeQtCritical ) {
            if ( DebugLoggerImpl::instance()->mApplyFilterToQtLogs ) {
                filter = true;
            }
            tag = QStringLiteral("[critical]");
            output = true;
        }
        break;
    case QtFatalMsg:
        if ( DebugLoggerImpl::instance()->mIncludeQtFatal ) {
            if ( DebugLoggerImpl::instance()->mApplyFilterToQtLogs ) {
                filter = true;
            }
            tag = QStringLiteral("[fatal]");
            output = true;
        }
        break;
    default:
        break;
    }

    if ( output ) {
        if ( filter ) {
            buf = DebugLoggerImpl::instance()->messageFilter( "", 0, "", buf, tag );
        }
        if ( !buf.isEmpty() ) {
            if ( !tag.isEmpty() ) {
                buf = tag + buf;
            }
            DebugLoggerImpl::instance()->mDefaultMessageHandler( type, context, buf );
            SKT::TLogger::instance()->write( buf );
        }
    }
}

// message formatters

QString DefaultMessageFormatter( const QString& file, int line, const QString& func, const QString& msg, const QString& tag )
{
    QString buf;

    QFileInfo fi( file );

    if ( !file.isEmpty() ) {
        buf = QString( QStringLiteral("%1:%2(%3) ") )
            //.arg( QDateTime::currentDateTime().toString( "yyyy-MM-dd hh:mm:ss " ) )
            .arg( fi.fileName() ).arg( func ).arg( line );
    }
    else {
//        buf = QString( QStringLiteral("%1 ") )
//            .arg( QDateTime::currentDateTime().toString( "yyyy-MM-dd hh:mm:ss " ) );
    }

    if ( !msg.isEmpty() ) {
        buf.append( msg );
    }

    return buf;
}

}
