#include "GadgetImageStreamProvider.h"
#include "../common/navigationcontroller.h"
#include "uiconstants.h"
#include "ImageProvider.h"

using namespace SKT;

GadgetImageStreamProvider::GadgetImageStreamProvider( NavigationController* controller, QObject* imageProvider,
    QQuickWindow* gadgetDebugWin )
    : mController( controller ),
    mGadgetDebugWin( gadgetDebugWin ),
    mImageProvider( dynamic_cast<ImageProvider*>(imageProvider) )
{
#ifdef QT_DEBUG
    mGadgetTest = mGadgetDebugWin->findChild<QObject*>("gadgetTest");
    mGadgetXxl = mGadgetDebugWin->findChild<QObject*>("gadgetXxl");
    mGadgetXl = mGadgetDebugWin->findChild<QObject*>("gadgetXl");
    mGadgetL = mGadgetDebugWin->findChild<QObject*>("gadgetL");
    mGadgetM = mGadgetDebugWin->findChild<QObject*>("gadgetM");
    mGadgetS = mGadgetDebugWin->findChild<QObject*>("gadgetS");    

    connect(mGadgetDebugWin, &QQuickWindow::destroyed, this, [this]{
        mStopped = true;
    });
#endif
}

GadgetImageStreamProvider::~GadgetImageStreamProvider()
{
    qDebug() << "[Gadget]~GadgetImageStreamProvider";
}

void GadgetImageStreamProvider::push( int type, const QImage& image )
{
#ifdef QT_DEBUG
    if ( mStopped ) {
        return;
    }

    static int count = 0;    

    QImage tempImage = image.mirrored(false, true);
    mImageProvider->setNaviGadgetImageForDebug( (UIConstants::GadgetType)type, tempImage );

    QObject* gadgetDisplay = nullptr;
    switch ( type ) {
    case UIConstants::GadgetXXL:
        gadgetDisplay = mGadgetXxl;        
        break;
    case UIConstants::GadgetXL:
        gadgetDisplay = mGadgetXl;
        break;
    case UIConstants::GadgetLarge:
        gadgetDisplay = mGadgetL;
        break;
    case UIConstants::GadgetMedium:
        gadgetDisplay = mGadgetM;
        break;
    case UIConstants::GadgetSmall:
        gadgetDisplay = mGadgetS;
        break;
    case UIConstants::GadgetTest:
        gadgetDisplay = mGadgetTest;
        break;
    }

    auto source = QString( "image://ImageProvider/gadgetImage%1_%2" ).arg( UIConstants::gadgetTypeToString( type ) ).arg( count );
    //auto path = QString("/home/thirdpartydev/tmap_rsm2018/dynamic/gadget_%1.png").arg( count );
    //image.save(path);

    count++;
    //count %= 2;

    QObject updater;
    updater.connect(&updater, &QObject::destroyed, gadgetDisplay, [this, gadgetDisplay, source]{
        QQmlProperty::write( gadgetDisplay, "source", source );
    });

#else
    //qDebug() << "[Gadget][Provider]data.bits: " << image.constBits();

    mController->changeGadgetImage( image, type );
#endif
}
