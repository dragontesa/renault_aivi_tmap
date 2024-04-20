#include "appinterface.h"
#include "applicationthread.h"
#include "tmapapplication.h"

namespace SKT {

static int s_argc = 1;
static char* s_argv[] = { (char*)"AppInterface", 0 };

class TmapAppThread : public ApplicationThread {
public:
    TmapAppThread(int argc, char *argv[]) : m_argc(argc), m_argv(argv) { }
    virtual ~TmapAppThread() {}

    /**
     * @brief createApplication
     * @return
     */
    virtual QCoreApplication* createApplication() {
        //qputenv("QT_QPA_PLATFORM", "wayland-egl");
        qputenv("QT_WAYLAND_DISABLE_WINDOWDECORATION", "1");
        qputenv("QSG_INFO", "1");
        //qputenv("QSG_RENDER_LOOP", "threaded");
        //qputenv("QDBUS_DEBUG", "1");

        return new TmapApplication(m_argc, m_argv);
    }

private:
    int m_argc;
    char** m_argv;
};

AppInterface::AppInterface()
    : m_appThread(new TmapAppThread(s_argc, s_argv))
{

}

AppInterface::~AppInterface()
{
    m_appThread->quit();
}

//Start the external third party navigation
bool AppInterface::bExtThirdPartyNavigaitonStartApp()
{
    m_appThread->start();

    return true;
}

//Monitor the external third party navigation with watchdog
bool AppInterface::bExtThirdPartyNavigaitonAppWatchdog()
{
    return true;
}

//Load the persistent data of external third party navigation
bool AppInterface::bExtThirdPartyNavigaitonAppLoadPersistentData()
{
    return true;
}

//Save the persistent data of external third party navigation
bool AppInterface::bExtThirdPartyNavigaitonAppSavePersistentData()
{
    return true;
}

}   // namespace SKT

ExternalThirdPartyNavigationApp_interface* CreateExternalThirdPartyNavigationAppInterface()
{
    return new SKT::AppInterface();
}

void DeleteExternalThirdPartyNavigationAppInterface(ExternalThirdPartyNavigationApp_interface** ppInterface)
{
    if (ppInterface) {
        SKT::AppInterface* app = dynamic_cast<SKT::AppInterface*>(*ppInterface);

        if (app) {
            delete app;
        }

        *ppInterface = NULL;
    }
}

