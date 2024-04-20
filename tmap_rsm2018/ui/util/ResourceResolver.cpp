#include "ResourceResolver.h"
#include "QmlAppHelper.h"
#include "LangConstants.h"
#include <QScreen>

#define TMAP_RSM_UI_ENABLE_OVERALL_USE_OF_IMAGEPROVIDER

using namespace SKT;

namespace {
    const QString ImageProviderProtocol = QStringLiteral("image://ImageProvider/");
}

ResourceResolver::ResourceResolver()
    : mLogicalDpi( 160 ),
    mPhysicalDpi( 0 )
{
//#ifndef BUILD_TARGET
//     QScreen* screen = QApplication::primaryScreen();
//     if (screen) {
//         mLogicalDpi = screen->logicalDotsPerInch();
//         mPhysicalDpi = screen->physicalDotsPerInch();
//     }
//#endif
}

ResourceResolver* ResourceResolver::instance()
{
    static ResourceResolver Instance;

    return &Instance;
}

void ResourceResolver::initialize( const QString& dirPath )
{
    mDirPath = dirPath;
    mPhysicalDpi = QGuiApplication::primaryScreen()->physicalDotsPerInch();

    T_DEBUGV( "physicalDotsPerInch = %d", QGuiApplication::primaryScreen()->physicalDotsPerInch() );

    auto iniPath = QDir::homePath() + QChar('/') + QLatin1String("tmap_rsm_ui_dbg.ini");
    QFile file( iniPath );
    if (file.exists()) {
        QSettings settings( iniPath, QSettings::IniFormat );
        auto logicalDpi = settings.value(QStringLiteral("logicalDpi"),0).toInt();
        if (logicalDpi != 0) {
            T_DEBUGV( "logicalDpi = %d", logicalDpi );
            mLogicalDpi = logicalDpi;
        }
    }

    QResource::registerResource( mDirPath + "/tmap_rsm_ui_qml.rcc" );
}

QString ResourceResolver::url( bool tryLocal, bool checkIfExists, const QString& protocol, const QString& path, const QString& path1, const QString& path2, const QString& path3, const QString& path4, const QString& path5 )
{
    QString ret;

    QString tempPath;
    if ( path.isEmpty() == false ) {
        tempPath.append( QChar('/') );
        tempPath.append( path );
    }
    if ( path1.isEmpty() == false ) {
        tempPath.append( QChar('/') );
        tempPath.append( path1 );
    }
    if ( path2.isEmpty() == false ) {
        tempPath.append( QChar('/') );
        tempPath.append( path2 );
    }
    if ( path3.isEmpty() == false ) {
        tempPath.append( QChar('/') );
        tempPath.append( path3 );
    }
    if ( path4.isEmpty() == false ) {
        tempPath.append( QChar('/') );
        tempPath.append( path4 );
    }
    if ( path5.isEmpty() == false ) {
        tempPath.append( QChar('/') );
        tempPath.append( path5 );
    }
	
	// 17.07.05 hskim 리소스 선택
    //	A. 현재 해상도(레이아웃) lang 경로의 리소스 선택
	//		/res/drawable-820x1024/lang/en/img
    //	B. A.에 없으면 현재 해상도 경로의 리소스 선택
	//		/res/drawable-820x1024/img
    //	C. B.에 없으면 디폴트 해상도 lang 경로의 리소스 선택
	//		/res/drawable-800x480/lang/en/img
    //	D. C.에 없으면 디폴트 해상도 경로의 리소스 선택
	//		/res/drawable-800x480/img
	//	* 차후 디폴트 해상도 두지 않고 리소스가 해상도 별로 존재하면 현재 해상도에 대한 리소스만을 선택하도록 간소화

    auto lang = QmlAppHelper::instance()->language();

    // check A : layout - lang
    if ( tryLocal ) {
        if ( lang != LangConstants::Default ) {
            QString resPath;
            resPath.append( mDirPath );
            resPath.append( QChar('/') );
            resPath.append( QmlAppHelper::instance()->layoutResDir() );
            resPath.append( QStringLiteral("/lang/") );
            resPath.append( QmlAppHelper::instance()->language() );
            resPath.append( tempPath );

            QFileInfo fi( resPath );
            if ( fi.isFile() && fi.exists() ) {
                ret = resPath;
            }
        }
    }

    // check B : layout
    if ( ret.isEmpty() ) {
        QString resPath;
        resPath.append( mDirPath );
        resPath.append( QChar('/') );
        resPath.append( QmlAppHelper::instance()->layoutResDir() );
        resPath.append( tempPath );

        if ( !checkIfExists ) {
            ret = resPath;
        }
        else {
            QFileInfo fi( resPath );
            if ( fi.isFile() && fi.exists() ) {
                ret = resPath;
            }
        }
    }

    // check C : default layout - lang
    if ( ret.isEmpty() ) {
        if ( tryLocal ) {
            if ( lang != LangConstants::Default ) {
                QString resPath;
                resPath.append( mDirPath );
                resPath.append( QChar('/') );
                resPath.append( QmlAppHelper::instance()->defaultLayoutResDir() );
                resPath.append( QStringLiteral("/lang/") );
                resPath.append( QmlAppHelper::instance()->language() );
                resPath.append( tempPath );

                if ( !checkIfExists ) {
                    ret = resPath;
                }
                else {
                    QFileInfo fi( resPath );
                    if ( fi.isFile() && fi.exists() ) {
                        ret = resPath;
                    }
                }
            }
        }
    }

    // check D : default layout
    if ( ret.isEmpty() ) {
        QString resPath;
        resPath.append( mDirPath );
        resPath.append( QChar('/') );
        resPath.append( QmlAppHelper::instance()->defaultLayoutResDir() );
        resPath.append( tempPath );

        if ( !checkIfExists ) {
            ret = resPath;
        }
        else {
            QFileInfo fi( resPath );
            if ( fi.isFile() && fi.exists() ) {
                ret = resPath;
            }
        }
    }

    if ( ret.isEmpty() ) {
            T_DEBUG( QString( "Detected invalid resource path '%1'" ).arg( tempPath ) );
    }
    else {
        ret = protocol + ret;
    }

    return ret;
}

QString ResourceResolver::imageUrl( const QString& path, const QString& path1, const QString& path2, const QString& path3 )
{
    QString ret;

    if ( !path.isEmpty() || !path1.isEmpty() || !path2.isEmpty() || !path3.isEmpty() ) {
#ifndef TMAP_RSM_UI_ENABLE_OVERALL_USE_OF_IMAGEPROVIDER
        if ( path.indexOf( QStringLiteral("cross") ) > -1 ) {            
            ret = url( false, false, ImageProviderProtocol, QStringLiteral("img"), path, path1, path2, path3 );
        }
        else {
            ret = url( true, true, QStringLiteral("file:"), QStringLiteral("img"), path, path1, path2, path3 );
        }
#else
        if ( path.indexOf( QStringLiteral(".sci") ) > -1 ) {
            ret = url( true, true, QStringLiteral("file:"), QStringLiteral("img"), path, path1, path2, path3 );
        }
        else {
            if ( path.indexOf( QStringLiteral("cross") ) > -1 ) {
                ret = url( false, false, ImageProviderProtocol, QStringLiteral("img"), path, path1, path2, path3 );
            }
            else {
                ret = url( true, true, ImageProviderProtocol, QStringLiteral("img"), path, path1, path2, path3 );
            }
        }
#endif
    }

    return ret;
}

QString ResourceResolver::imagePath( const QString& path, const QString& path1, const QString& path2, const QString& path3 )
{
    QString ret;

    if ( !path.isEmpty() || !path1.isEmpty() || !path2.isEmpty() || !path3.isEmpty() ) {
        ret = url( true, true, "", QStringLiteral("img"), path, path1, path2, path3 );
    }

    return ret;
}

void ResourceResolver::setLogicalDpi( qreal dpi )
{
	mLogicalDpi = dpi;
}	

int ResourceResolver::pixelToDp( int pixelSize )
{	
    return qRound( pixelSize * ( 160 / mLogicalDpi ) * ( mPhysicalDpi / 160 ) );
}

int ResourceResolver::centerAlignOffset(int pixelSize)
{
    switch (pixelSize) {
    case 16:
    case 18:
    case 20:
    case 21:
    case 22:
    case 24:
    case 25:
    case 27:
    case 28:
    case 29:
    case 30:
    case 32:
    case 33:
    case 34:
    case 35:
    case 38:
    case 40:
    case 50:
        return 0;
    default:
        return 0;
    }
}
