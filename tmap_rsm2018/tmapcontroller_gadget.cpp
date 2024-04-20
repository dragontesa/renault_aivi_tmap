#include <QQmlContext>
#include <QQuickWindow>
#include <QDateTime>
#include <QDir>
#include <QSettings>
#include <QTimeZone>
#include <QOpenGLFramebufferObject>
#include <QImage>
#include <QOpenGLContext>
#include <QResource>

#include "tmapcontroller.h"
#include "platformcontroller.h"
#include "tmapproperty.h"
#include "build.h"

#include "rgsimulator.h"
#include "gpsservice.h"
#include "environment.h"
#include "map.h"
#include "mapviewquick.h"
#include "routesearch.h"
#include "rpnsearch.h"
#include "worldpoint.h"
#include "rgaudiohelper.h"
#include "searchmanager.h"
#include "addressmanager.h"
//#include "markermanager.h"
#include "fpscalculator.h"
#include "mapswitcher.h"
#include "packagemanager.h"
#include "sessionmanager.h"
#include "cipherservice.h"
#include "daynightdetector.h"
#include "debugkeyfilter.h"

#include "uilayout.h"
#include "uiconstants.h"
#include "tmap_rsm_ui.h"

#include "VSM_Map.h"
#include "VSM_Navi.h"
#include "VSM_Navi_Audio.h"
#include "VSM_Util.h"
#include "MNS_Define.h"
#include "qctestingfeatures.h"

#include "hardkey.h"
#include "datetimeutil.h"
#include "hostnames.h"

//#include <sys/time.h>

//#define USE_FBO_FOR_GADGET

namespace SKT {

void TmapController::simulationGadget(bool shift, int key)
{
    QQuickWindow *window = qobject_cast<QQuickWindow*>(m_qmlEngine->rootObjects().first());
    bool show = !shift;

    auto gadgetType = UIConstants::GadgetNone;

    if (key == Qt::Key_F9) {
        emit showGadget(82, false, false);
        emit showGadget(35, true, false);
    }
    else if (key == Qt::Key_F11) {
        emit contextSwitchRequested( QVariantMap{
            {QStringLiteral("request"), NAVIGATION_SERVICE}
        } );
        emit appForegroundChanged( true );
    }
    else if (key == Qt::Key_F12) {
        emit appForegroundChanged( false );
    }
    else {
        switch (key) {
        case Qt::Key_F1:{
                gadgetType = UIConstants::GadgetXXL;
            }
            break;
        case Qt::Key_F2:
            gadgetType = UIConstants::GadgetXL;
            break;
        //case Qt::Key_F3:
            //gadgetType = UIConstants::GadgetLarge;
            //break;
        case Qt::Key_F4:
            gadgetType = UIConstants::GadgetLarge;
            break;
        case Qt::Key_F5:
            gadgetType = UIConstants::GadgetMedium;
            break;
        case Qt::Key_F6:
            gadgetType = UIConstants::GadgetSmall;
            break;
        case Qt::Key_F7:
            gadgetType = UIConstants::GadgetIconFuel;
            break;
        case Qt::Key_F8:
            gadgetType = UIConstants::GadgetIconParking;
            break;
        //case Qt::Key_F9:
            //gadgetType = UIConstants::GadgetLarge;
            //break;
        }

        emit showGadget(gadgetType, show, false);
    }
}

void TmapController::simulationGadgetTouch(uint state, uint x, uint y, uint timeStamp, int instanceId)
{
    emit gadgetTouched(state, x, y, timeStamp, instanceId);
}

void TmapController::onReadyToRunGadget()
{
    m_readyToRunGadget = true;  
}

void TmapController::onPlatformActivateExternalImageProvider(bool activate, int instanceId)
{
    //qDebug() << "[Gadget][TmapController]onPlatformActivateExternalImageProvider activate: " << activate << " instanceId: " << instanceId;

//    if((m_InstanceId == instanceId) && (m_IsDrawRender == activate)) {
//        qDebug() << "[Gadget][TmapController]onPlatformActivateExternalImageProvider #1 activate: " << activate << " instanceId: " << instanceId;
//        return;
//    }

//    m_InstanceId = instanceId;
//    m_IsDrawRender = activate;

    auto gadgetType = UIConstants::GadgetNone;

    if(instanceId == Nav_Gadget_Map_XXXL) {
        gadgetType = UIConstants::GadgetXXXL;
    } else if(instanceId == Nav_Gadget_Map_XXL) {
        gadgetType = UIConstants::GadgetXXL;
    } else if(instanceId == Nav_Gadget_Map_XL) {
        gadgetType = UIConstants::GadgetXL;
    } else if(instanceId == Nav_Gadget_Map_L) {
        gadgetType = UIConstants::GadgetLarge;
    } else if(instanceId == Nav_Gadget_Map_M) {
        gadgetType = UIConstants::GadgetMedium;
    } else if(instanceId == Nav_Gadget_Map_S) {
        gadgetType = UIConstants::GadgetSmall;
    } else if(instanceId == Nav_Gadget_OilStation) {
        gadgetType = UIConstants::GadgetIconFuel;
    } else if(instanceId == Nav_Gadget_Parking) {
        gadgetType = UIConstants::GadgetIconParking;
    } else if(instanceId == Nav_Gadget_AQI) {
        gadgetType = UIConstants::GadgetIconAqi;
    }

    m_gadgetSignal["isValid"] = true;
    m_gadgetSignal["gadgetType"] = (int)gadgetType;
    m_gadgetSignal["gadgetActivate"] = activate;

#ifdef BUILD_TARGET
    emit showGadget(gadgetType, activate, false);
#else
    emit showGadget(gadgetType, activate, true);
#endif
}

void TmapController::onPlatformExternalImageProviderTouch(quint8 touchState, uint xPos, uint yPos, uint timeStamp, int instanceId)
{
    emit gadgetTouched(touchState, xPos, yPos, timeStamp, instanceId);
}

void TmapController::changeGadgetImage(const QImage& image, int type)
{
    //qDebug() << "[Gadget][TmapController]data.bits: " << image.constBits();
    //qDebug() << "[Gadget][TmapController]changeGadgetImage type: " << type << " bytes: " << image.byteCount();

    if ( m_saveGadgetImage ) {
        static int tick = 0;
        if ( tick > 9 ) tick = 0;
#ifdef BUILD_TARGET
        auto path = QString("/var/opt/bosch/navdata/log/screenshot/gadget_%1_%2.png").arg(type).arg( tick++ );
#else
        auto path = QString("/home/thirdpartydev/tmap_rsm2018/dynamic/gadget_%1_%2.png").arg(type).arg( tick++ );
#endif
        image.save(path);
    }

    //emit sendGadgetImage(image, type);

    if(image.width() > 0 && image.height() >0) {
        m_platformController->sendGadgetImage(image, type);
    }
    else {
        qDebug() << "[GadgetSwitch]changeGadgetImage type: " << type << " width: " << image.width() << " height: " << image.height();
    }
}

void TmapController::changeIconGadgetImage( const QImage& image, int type )
{
    //qDebug() << "[Gadget][TmapController]changeIconGadgetImage #5 type: " << type << " bytes: " << image.byteCount();

    //m_saveGadgetImage = true;

    if ( m_saveGadgetImage ) {
#ifdef BUILD_TARGET
        auto path = QString("/var/opt/bosch/navdata/log/screenshot/gadget_%1_%2.png").arg(type).arg( QTime::currentTime().msecsSinceStartOfDay() );
#else
        auto path = QString("/home/thirdpartydev/tmap_rsm2018/dynamic/gadget_%1_%2.png").arg(type).arg( QTime::currentTime().msecsSinceStartOfDay() );
#endif
        bool res = image.save( path );
        //qDebug() << "[Gadget][TmapController]changeIconGadgetImage #6 %1" << res;
    }

    //emit sendGadgetImage(image, type);
    m_platformController->sendGadgetImage(image, type);
}

void TmapController::saveGadgetImage( bool save )
{
    //qDebug() << "[Gadget][TmapController]saveGadgetImage #1 save: " << save;

    m_saveGadgetImage = save;
}

void TmapController::stopSendingGadgetImage()
{
    emit stopSendingGadgetImageRequested();
}


}

