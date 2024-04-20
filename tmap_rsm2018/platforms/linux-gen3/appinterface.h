#ifndef APPINTERFACE_H
#define APPINTERFACE_H

#include <memory>
#include "ExternalThirdPartyNavigationApp_interface.h"

namespace SKT {

class ApplicationThread;

class AppInterface : public ExternalThirdPartyNavigationApp_interface
{
public:
    AppInterface();
    virtual ~AppInterface();

    //Start the external third party navigation
    virtual bool bExtThirdPartyNavigaitonStartApp();

    //Monitor the external third party navigation with watchdog
    virtual bool bExtThirdPartyNavigaitonAppWatchdog();

    //Load the persistent data of external third party navigation
    virtual bool bExtThirdPartyNavigaitonAppLoadPersistentData();

    //Save the persistent data of external third party navigation
    virtual bool bExtThirdPartyNavigaitonAppSavePersistentData();

private:
    std::unique_ptr<ApplicationThread> m_appThread;
};

}

#endif // APPINTERFACE_H
