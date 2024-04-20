#include "ImageProvider.h"
#include "ResourceResolver.h"
#include "CrossImageProvider.h"
#include <QFile>
#include <QImageReader>

using namespace SKT;

ImageProvider::ImageProvider()
    : QQuickImageProvider( QQuickImageProvider::Image,
    QQuickImageProvider::ForceAsynchronousImageLoading )
{
}

#ifdef QT_DEBUG
void ImageProvider::setNaviGadgetImageForDebug( UIConstants::GadgetType type, const QImage& image )
{
    QMutexLocker locker( &mMutex );

    switch ( type ) {
    case UIConstants::GadgetXXL:
        mGadgetXxlImage = image;
        break;
    case UIConstants::GadgetXL:
        mGadgetXlImage = image;
        break;
    case UIConstants::GadgetLarge:
        mGadgetLImage = image;
        break;
    case UIConstants::GadgetMedium:
        mGadgetMImage = image;
        break;
    case UIConstants::GadgetSmall:
        mGadgetSImage = image;
        break;
    case UIConstants::GadgetTest:
        mGadgetTestImage = image;
        break;
    }
}
#endif

QImage ImageProvider::requestImage(const QString &id, QSize *size, const QSize &requestedSize)
{    
    QMutexLocker locker( &mMutex );

    QImage image;

    int pos = -1;

    if ( ( pos = id.indexOf( QStringLiteral("onlineCross") ) ) > -1 ) {
        image = mOnlineCrossImage;
    }
    else
#ifdef QT_DEBUG
    if ( ( pos = id.indexOf( QStringLiteral("gadgetImage") ) ) > -1 ) {
        if ( ( pos = id.indexOf( UIConstants::gadgetTypeToString( UIConstants::GadgetXXL ) ) ) > -1 ) {
            image = mGadgetXxlImage;
        }
        else if ( ( pos = id.indexOf( UIConstants::gadgetTypeToString( UIConstants::GadgetXL ) ) ) > -1 ) {
            image = mGadgetXlImage;
        }
        else if ( ( pos = id.indexOf( UIConstants::gadgetTypeToString( UIConstants::GadgetLarge ) ) ) > -1 ) {
            image = mGadgetLImage;
        }
        else if ( ( pos = id.indexOf( UIConstants::gadgetTypeToString( UIConstants::GadgetMedium ) ) ) > -1 ) {
            image = mGadgetMImage;
        }
        else if ( ( pos = id.indexOf( UIConstants::gadgetTypeToString( UIConstants::GadgetSmall ) ) ) > -1 ) {
            image = mGadgetSImage;
        }
        else if ( ( pos = id.indexOf( UIConstants::gadgetTypeToString( UIConstants::GadgetTest ) ) ) > -1 ) {
            image = mGadgetTestImage;
        }
    }
    else
#endif
    if ( ( pos = id.indexOf( QStringLiteral("cross/") ) ) > -1 ) {
        bool loaded = false;
        auto temp = id.mid( pos );
        auto toks = temp.split( QChar( '/' ) );
        if ( toks.count() == 3 ) {
            bool ok = false;
            auto dayOrNight = toks[toks.count() - 2].toInt(&ok);
            if ( ok ) {
                auto nId = toks[toks.count() - 1].toInt(&ok);
                if ( ok ) {
                    auto bytes = CrossImageProvider::instance()->getImage( nId, dayOrNight );
                    if ( image.loadFromData( bytes ) ) {                        
                        loaded = true;
#ifdef QT_DEBUG
                        auto path = QString("/home/thirdpartydev/tmap_rsm2018/dynamic/cross_%1.png").arg( QTime::currentTime().msecsSinceStartOfDay() );
                        //bool res = image.save( path );
                        //T_DEBUG(QString("Succeeded to load id: %1 res: %2 path: %3").arg(id).arg(res).arg(path));
#endif
                    }
                }
            }
        }
        if ( !loaded ) {
            T_DEBUG( QString( "Failed to load image '%1' #2" ).arg( id ) );
        }
    }
    else {
        QImageReader imageReader(id);
        if (!imageReader.read( &image )) {
            auto error = imageReader.error();
            auto errorStr = imageReader.errorString();
            T_DEBUG( QString( "[ImageReader]Failed to load image: '%1' error: %2 errorStr: %3" ).arg( id ).arg(error).arg(errorStr) );
        }
    }

    if ( image.isNull() ) {
        T_DEBUG( QString( "Failed to load image '%1' #3" ).arg( id ) );
    }
    else {
        //T_DEBUG( "Reloaded! " + id );

        if (requestedSize.width() > 0 && requestedSize.height() > 0) {
            image = image.scaled(requestedSize);
        }
        *size = QSize( image.width(), image.height() );
    }

    return image;
}

void ImageProvider::updateImage( ApplicationConstants::ImageType type, const QImage& image )
{
    QMutexLocker locker( &mMutex );

    if ( type == ApplicationConstants::OnlineCrossImage ) {
        qDebug() << "[Cross]ImageProvider::updateImage";
        mOnlineCrossImage = image;
    }
}
