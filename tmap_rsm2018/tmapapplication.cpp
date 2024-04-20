#include <QDebug>
#include <QSurfaceFormat>
#include <QTextCodec>

#include "tmapapplication.h"
#include "tmapcontroller.h"
#include "build.h"
#include "environment.h"
#include "logger.h"

namespace SKT {

TmapApplication::TmapApplication(int &argc, char **argv)
    : QGuiApplication(argc, argv)
{
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));

    Environment::init();

    m_logger = new Logger;
    m_logger->init();

    Environment::detectDeviceType();

#ifndef QT_NO_DEBUG_OUTPUT
    qDebug(sys) << "[Perf][Init]after main()";

    qDebug(sys) << "TmapApplication created.";
    qDebug(sys) << "Version:" << Build::versionString();
    qDebug(sys) << "Build ID:" << Build::buildId();
#endif

    // specify default surface format
    QSurfaceFormat surfaceFormat;
    surfaceFormat.setRedBufferSize(8);
    surfaceFormat.setGreenBufferSize(8);
    surfaceFormat.setBlueBufferSize(8);
    surfaceFormat.setAlphaBufferSize(0);
    surfaceFormat.setRenderableType(QSurfaceFormat::OpenGLES);
    QSurfaceFormat::setDefaultFormat(surfaceFormat);

    m_controller = new TmapController();
}

TmapApplication::~TmapApplication()
{
    if (m_controller) {
        delete m_controller;
        m_controller = NULL;
    }

    m_logger->cleanUp();
    delete m_logger;
}

}
