#include "platform_connectivity_gen3.h"
#include "tmapcontroller.h"
// ecm stack for gen3
#define ENGINE_ID    "sdc-ecm"

namespace SKT {

PlatformConnectivity* TmapController::newPlatformConnectivity()
{
    return new PlatformConnectivityGen3();
}

PlatformConnectivityGen3::PlatformConnectivityGen3() :
      mEngine(NULL)
{
    
}

PlatformConnectivityGen3::~PlatformConnectivityGen3()
{

}

int PlatformConnectivityGen3::init()
{
    ERR_load_crypto_strings();
    ERR_load_SSL_strings();
    SSL_library_init();

    ENGINE *ecm = NULL;

    /* enable dynamic engines support */
    ENGINE_load_builtin_engines();
    ERR_load_ENGINE_strings();

    /* get dynamic sdc-ecm engine */
    ecm = ENGINE_by_id(ENGINE_ID);
    if (!ecm) {
        qWarning(sys) << "platform connectivity not found built engine";
        return 1;
    }

    if (!ENGINE_init(ecm)) {
        qWarning(sys) << "platform connectivity failed to init engine";
        return 2;
    }

    OPENSSL_config(NULL);

    mEngine = ecm;
    qWarning(sys) << "platform connectivity init done!";

    return 0;
}

int PlatformConnectivityGen3::release()
{
    if (mEngine) {
        ENGINE_finish(mEngine);
    }
    mEngine = NULL;
    
    return 0;
}

}
