#include "TApplication.h"

using namespace SKT;

TApplication::TApplication(int& argc, char ** argv)
    : QApplication( argc, argv )
{
}

QString TApplication::path( const QString& path, const QString& path2,
    const QString& path3, const QString& path4 ) const
{
    QString ret;

    ret.append( QCoreApplication::applicationDirPath() );

    if ( path.isEmpty() == false ) {
        ret.append( QChar('/') );
        ret.append( path );
    }
    else if ( path2.isEmpty() == false ) {
        ret.append( QChar('/') );
        ret.append( path2 );
    }
    else if ( path3.isEmpty() == false ) {
        ret.append( QChar('/') );
        ret.append( path3 );
    }
    else if ( path4.isEmpty() == false ) {
        ret.append( QChar('/') );
        ret.append( path4 );
    }

    return ret;
}
