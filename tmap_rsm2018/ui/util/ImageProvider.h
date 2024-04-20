#ifndef IMAGEPROVIDER_H
#define IMAGEPROVIDER_H

#include <qquickimageprovider.h>
#include "uiconstants.h"
#include "ApplicationConstants.h"

#define TMAP_IMAGE_PROVIDER_ENABLE_

namespace SKT {

class ImageProvider : public QQuickImageProvider, public QObject
{
public:
    ImageProvider();

    QImage requestImage(const QString &id, QSize *size, const QSize &requestedSize);

#ifdef QT_DEBUG
    void setNaviGadgetImageForDebug( UIConstants::GadgetType type, const QImage& image );
#endif

    void updateImage( ApplicationConstants::ImageType type, const QImage& image );

private:
    QMutex mMutex;
#ifdef QT_DEBUG
    QImage mGadgetXxlImage;
    QImage mGadgetXlImage;
    QImage mGadgetLImage;
    QImage mGadgetMImage;
    QImage mGadgetSImage;
    QImage mGadgetTestImage;
#endif
    QImage mOnlineCrossImage;
};

}

#endif // IMAGEPROVIDER_H
