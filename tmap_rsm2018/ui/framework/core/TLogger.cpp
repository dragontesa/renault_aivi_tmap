#include "TLogger.h"
#include "TJsonUtils.h"

using namespace SKT;

TLogger::TLogger(void)
    : mSaveLog( false )
{
}

TLogger::~TLogger(void)
{
}

TLogger* TLogger::instance()
{
    static TLogger _instance;
    return &_instance;
}
void TLogger::setSavePath( const QString& path )
{
    mSaveLog = true;
    mSavePath = path;
}

void TLogger::write( const QString& msg )
{
    QString buf;
    buf.append( msg );

    if ( mSaveLog ) {
        buf.append( QStringLiteral("\r\n") );

        QFile outFile( mSavePath );
        outFile.open( QIODevice::WriteOnly | QIODevice::Append );
        QTextStream ts( &outFile );
        ts << buf;
    }
}
