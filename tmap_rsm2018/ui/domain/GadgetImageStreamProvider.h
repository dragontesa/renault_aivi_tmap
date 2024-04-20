#ifndef GADGETIMAGESTREAMPROVIDER_H
#define GADGETIMAGESTREAMPROVIDER_H

#include "ImageStreamProvider.h"
#include "uiconstants.h"
#include "ImageProvider.h"

namespace SKT
{
	
class NavigationController;

class GadgetImageStreamProvider : public ImageStreamProvider
{
    Q_OBJECT
public:
    GadgetImageStreamProvider( NavigationController* controller, QObject* imageProvider, QQuickWindow* gadgetDebugWin );

    ~GadgetImageStreamProvider();

    void push( int type, const QImage& image );

private:
    NavigationController* mController;    
    QQuickWindow* mGadgetDebugWin;
    ImageProvider* mImageProvider = nullptr;
    QObject* mGadgetXxl = nullptr;
    QObject* mGadgetXl = nullptr;
    QObject* mGadgetL = nullptr;
    QObject* mGadgetM = nullptr;
    QObject* mGadgetS = nullptr;
    QObject* mGadgetTest = nullptr;
    bool mStopped = false;
};

}

#endif // GADGETIMAGESTREAMPROVIDER_H
