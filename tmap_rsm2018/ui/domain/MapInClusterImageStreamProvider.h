#ifndef MAPINCLUSTERIMAGESTREAMPROVIDER_H
#define MAPINCLUSTERIMAGESTREAMPROVIDER_H

#include "ImageStreamProvider.h"

namespace SKT
{
	
class NavigationController;

class MapInClusterImageStreamProvider : public ImageStreamProvider
{
public:
    MapInClusterImageStreamProvider( NavigationController* controller );

    ~MapInClusterImageStreamProvider();

    void push( int type, const QImage& image );

private:
    NavigationController* mController;
};

}

#endif // MAPINCLUSTERIMAGESTREAMPROVIDER_H
