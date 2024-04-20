#include "platform_connectivity_dummy.h"
#include "tmapcontroller.h"

namespace SKT {

PlatformConnectivity* TmapController::newPlatformConnectivity()
{
    return new PlatformConnectivityDummy();
}

PlatformConnectivityDummy::PlatformConnectivityDummy()
{
}

PlatformConnectivityDummy::~PlatformConnectivityDummy()
{

}

int PlatformConnectivityDummy::init()
{

}

int PlatformConnectivityDummy::release()
{

}

}