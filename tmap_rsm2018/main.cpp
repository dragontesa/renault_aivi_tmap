#include "tmapapplication.h"

using namespace SKT;

int main(int argc, char *argv[])
{
#ifdef WIN32
    // force to use ANGLE OpenGLES2 implementation
    QCoreApplication::setAttribute(Qt::AA_UseOpenGLES);
#endif

#ifdef USE_WAYLAND
    qputenv("QT_QPA_PLATFORM", "wayland-egl");
    qputenv("QT_WAYLAND_DISABLE_WINDOWDECORATION", "1");
    qputenv("QT_XKB_CONFIG_ROOT", "/usr/share/X11/xkb");
#endif

#ifdef QT_DEBUG
    qputenv("TMAP_VSM_LOG", "1");
    qputenv("TMAP_VSM_LOGLEVEL", "4");
#endif

    //qputenv("QDBUS_DEBUG", "1");
    qputenv("QSG_INFO", "1");
    qputenv("QT_QML_TYPELOADER_MINIMUM_TRIM_THRESHOLD", "256");
    qputenv("QT_WAYLAND_DISABLE_WINDOWDECORATION", "1");

    TmapApplication app(argc, argv);

    return app.exec();
}
