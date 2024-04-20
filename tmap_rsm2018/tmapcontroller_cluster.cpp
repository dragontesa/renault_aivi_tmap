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
//int imgcnt = 0;
//bool isBlack = false;
//QImage img2, img3;
//bool isImg = false;

namespace SKT {

enum LayoutSize {
    LargeLayoutWidth   = 800,
    LargeLayoutHeight  = 400,
    SmallLayoutWidth   = 640,
    SmallLayoutHeight  = 480,
};

void TmapController::setVideoStream(const QImage& img)
{
#ifdef USE_MIC
   qDebug() << ":::::: m_videoStream setVideoStream()" << img.width() << img.height() << m_videoStreamResume;
   videostream* video;

//20191106 MODIFY TEST
   m_videoWidth   = img.width();
   m_videoHeight  = img.height();

   if(!m_videoStreamResume)
       return;

   if(img.byteCount() != LargeLayoutWidth*LargeLayoutHeight*4)
       return;

//   if(m_videoWidth == LayoutSize::LargeLayoutWidth && m_videoHeight == LayoutSize::LargeLayoutHeight){
       video = m_centerVideoStream;
        qDebug() << ":::::: m_videoStream #1" << m_videoWidth << m_videoHeight;
//       //20191106 MODIFY TEST
//        m_VideosMutex.lock();
//        if(!video->isUsingBuffer())
//        {
//            qDebug() << ":::::: setVideoStream() #3" << img.width() << img.height();
//            if(img.width() > 0 && img.height() > 0)
//               video->setImageSize(img.width(), img.height());

//            if(img.byteCount() != 0)
//                video->setBuffer((uchar*)img.constBits(), img.byteCount());
//        }
//        m_VideosMutex.unlock();
//    }else{
//       video = m_sideVideoStream;
//       //20191106 MODIFY TEST
//       video = m_centerVideoStream;
//       qDebug() << ":::::: setVideoStream #4" << m_videoWidth << m_videoHeight << img.width() << img.height();
//       QImage tempimg = img.copy(150, 0, 500, 400);
//       qDebug() << ":::::: setVideoStream #4-1" << m_videoWidth << m_videoHeight << tempimg.width() << tempimg.height();
//       QImage small = tempimg.scaled(800, 400,Qt::IgnoreAspectRatio);
//       qDebug() << ":::::: setVideoStream #4-2" << m_videoWidth << m_videoHeight << small.width() << small.height();

//       m_VideosMutex.lock();
//       if(!video->isUsingBuffer())
//       {
//           qDebug() << ":::::: setVideoStream() #3" << small.width() << small.height();
//           if(small.width() > 0 && small.height() > 0)
//              video->setImageSize(small.width(), small.height());

//           if(small.byteCount() != 0)
//               video->setBuffer((uchar*)small.constBits(), small.byteCount());
//       }
//       m_VideosMutex.unlock();
//    }
    m_VideosMutex.lock();
    if(!video->isUsingBuffer())
    {
        qDebug() << ":::::: setVideoStream() #3" << img.width() << img.height();
        if(img.width() > 0 && img.height() > 0)
            video->setImageSize(img.width(), img.height());

        if(img.byteCount() != 0)
            video->setBuffer((uchar*)img.constBits(), img.byteCount());
    }
    m_VideosMutex.unlock();
    qDebug() << ":::::: setVideoStream() #2" << img.width() << img.height();

    if(!video->isRunning())
    {
        video->initPipeline();
        video->start(QThread::HighestPriority);
    }

#endif
}

void TmapController::setVideoStreamIpPort(QString ip, ushort port, ushort videoType, int width, int height)
{
    qDebug() << "tmcontroller, setVideoStreamIpPort : ip "<<ip << port << videoType << width << height;
#ifdef USE_MIC
//    if(width == LayoutSize::LargeLayoutWidth && height == LayoutSize::LargeLayoutHeight)
//    {
        if(m_centerVideoStream == NULL)
        {
            m_centerVideoStream = new videostream();
            connect(m_centerVideoStream, &videostream::doneStartMapStream, this, &TmapController::onDoneStartMapStream);
        }

        m_centerVideoStream->setIp(ip.toUtf8().data());
        m_centerVideoStream->setPort(port);
//    }
//    else
//    {
//        if(m_sideVideoStream == NULL)
//        {
//            m_sideVideoStream = new videostream();
//            connect(m_sideVideoStream, &videostream::doneStartMapStream, this, &TmapController::onDoneStartMapStream);
//        }

//        m_sideVideoStream->setIp(ip.toUtf8().data());
//        m_sideVideoStream->setPort(port);
//    }

    m_videoWidth = width;
    m_videoHeight = height;
#endif
}

void TmapController::destroyVideoStream()
{
#ifdef USE_MIC
//    m_VideosMutex.lock();

//    if(m_videoWidth == LayoutSize::LargeLayoutWidth && m_videoHeight == LayoutSize::LargeLayoutHeight)
//    {
        if(m_centerVideoStream != NULL)
            m_centerVideoStream->setStopStream();
//    }else{
//        if(m_sideVideoStream != NULL)
//            m_sideVideoStream->setStopStream();
//    }
//    m_VideosMutex.unlock();
//    m_videoStream->destroyed();
#endif
}

void TmapController::simulationMapInCluster(int key)
{
    switch (key) {
    case Qt::Key_F8:
        {
            m_mapInCluster = !m_mapInCluster;

            if ( m_mapInCluster ) {
                emit startMapInCluster(0);
            }
            else {
                emit stopMapInCluster();
            }
        }
        break;
    }
}

bool TmapController::getStartVideoStream(int width, int height)
{
#ifdef USE_MIC
//    if(m_videoWidth == LayoutSize::LargeLayoutWidth && m_videoHeight == LayoutSize::LargeLayoutHeight)
//    {
        if(m_centerVideoStream != NULL)
           return  m_centerVideoStream->isStartStreaming(width, height);
//    }else{
//        if(m_sideVideoStream != NULL)
//           return m_sideVideoStream->isStartStreaming(width, height);
//    }
#endif
    return true;
}

void TmapController::startVideoStream(int layout)
{
    qDebug() << "tmcontroller,,startVideoStream : ";
    m_videoStreamPlay = true;
    m_videoStreamResume = true;
#ifdef USE_MIC
//    if(m_videoWidth == LayoutSize::LargeLayoutWidth && m_videoHeight == LayoutSize::LargeLayoutHeight)
//    {
        qDebug() << "tmcontroller,,startVideoStream : #1";
        if(m_centerVideoStream != NULL)
            m_centerVideoStream->setStartStream(false);
//    }else{
//        qDebug() << "tmcontroller,,startVideoStream : #2";

//        if(m_sideVideoStream != NULL)
//            m_sideVideoStream->setStartStream(false);
//    }
#endif
    //simulationWidget(Qt::Key_F3);
}

//test
void TmapController::resumeVideoStream()
{
    qDebug() << "tmcontroller,,resumeVideoStream : ";
    m_videoStreamPlay = true;
    m_videoStreamResume = true;
#ifdef USE_MIC
//    if(m_videoWidth == LayoutSize::LargeLayoutWidth && m_videoHeight == LayoutSize::LargeLayoutHeight)
//    {
        qDebug() << "tmcontroller,,resumeVideoStream : #1";
        if(m_centerVideoStream != NULL)
            m_centerVideoStream->setStartStream(true);
//    }else{
//        qDebug() << "tmcontroller,,resumeVideoStream : #2";

//        if(m_sideVideoStream != NULL)
//             m_sideVideoStream->setStartStream(true);
//    }
#endif
}

void TmapController::stopVideoStream()
{
//    m_VideosMutex.lock();
//    imgcnt = 0;
    qDebug() << "tmcontroller,,stopVideoStream : ";
    destroyVideoStream();
    m_videoStreamPlay = false;
    //simulationWidget(Qt::Key_F7);
//    m_VideosMutex.unlock();
}

void TmapController::pauseVideoStream()
{
    m_VideosMutex.lock();
    m_videoStreamPlay = false;
    qDebug() << "onDoneStartMapStream() [pauseVideoStream]" << m_videoWidth << m_videoHeight;
//    aaa = 0;
    qDebug() << "tmcontroller,,pauseVideoStream 2 ";
#ifdef USE_MIC
//    if(m_videoWidth == LayoutSize::LargeLayoutWidth && m_videoHeight == LayoutSize::LargeLayoutHeight)
//    {
        if(m_centerVideoStream)
        {
           m_centerVideoStream->setStopStream();
           qDebug() << "onDoneStartMapStream() [pauseVideoStream] #1";
        }
//    }else{
//        if(m_sideVideoStream)
//        {
//            m_sideVideoStream->setStopStream();
//            qDebug() << "onDoneStartMapStream() [pauseVideoStream] #2";
//        }
//    }
#endif
    m_videoStreamResume = false;
    m_VideosMutex.unlock();
}

//void TmapController::modifyVideoStream(int width, int height)
//{
//#ifdef USE_MIC
////20191106 MODIFY TEST
//    if((m_videoWidth == LayoutSize::LargeLayoutWidth && m_videoHeight == LayoutSize::LargeLayoutHeight) &&
//       (width == LayoutSize::SmallLayoutWidth && height == LayoutSize::SmallLayoutHeight))
//    {
//        if(m_centerVideoStream == NULL)
//        {
//            m_centerVideoStream = new videostream();
//            connect(m_centerVideoStream, &videostream::doneStartMapStream, this, &TmapController::onDoneStartMapStream);
//        }

//        m_centerVideoStream->setStopStream();

//        if(m_sideVideoStream == NULL)
//        {
//            m_sideVideoStream = new videostream();
//            connect(m_sideVideoStream, &videostream::doneStartMapStream, this, &TmapController::onDoneStartMapStream);
//            m_sideVideoStream->setStartStream(false);
//        }else{
//            m_sideVideoStream->setStartStream(true);
//        }
//    }
//    else
//    {
//        if(m_sideVideoStream == NULL)
//        {
//            m_sideVideoStream = new videostream();
//            connect(m_sideVideoStream, &videostream::doneStartMapStream, this, &TmapController::onDoneStartMapStream);
//        }

//        m_sideVideoStream->setStopStream();

//        if(m_centerVideoStream == NULL)
//        {
//            m_centerVideoStream = new videostream();
//            connect(m_centerVideoStream, &videostream::doneStartMapStream, this, &TmapController::onDoneStartMapStream);
//            \->setStartStream(false);
//        }else{
//            m_centerVideoStream->setStartStream(true);
//        }
//    }

//    m_videoWidth = width;
//    m_videoHeight = height;
//#endif
//}

void TmapController::onDoneStartMapStream()
{
//    qDebug() << "hhmictest onDoneStartMapStream()";
    emit doneStartMapVideoStream();
}

void TmapController::changeMapInClusterImage( const QImage& image )
{
    qDebug() << " mapincluster: #1";
    if ( m_saveGadgetImage ) {
        static int tick = 0;
        if ( tick > 9 ) tick = 0;
    #ifdef BUILD_TARGET
        auto path = QString("/var/opt/bosch/navdata/log/screenshot/cluster_%1.png").arg( tick++ );
    #else
        auto path = QString("/home/thirdpartydev/tmap_rsm2018/dynamic/cluster_%1.png").arg( tick++ );
    #endif
        image.save(path);
    }

//    qDebug() << "mapincluster: " << res << " file:" << path;
//    qDebug() << " mapincluster:" << path;
    setVideoStream( image );
     qDebug() << " mapincluster: #2";
}

}

