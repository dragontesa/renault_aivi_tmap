#include "MapInClusterImageStreamProvider.h"
#include "../common/navigationcontroller.h"

using namespace SKT;

MapInClusterImageStreamProvider::MapInClusterImageStreamProvider( NavigationController* controller )
    : mController( controller )
{
}

MapInClusterImageStreamProvider::~MapInClusterImageStreamProvider()
{
}

void MapInClusterImageStreamProvider::push( int type, const QImage& image )
{
    mController->changeMapInClusterImage( image );
}
