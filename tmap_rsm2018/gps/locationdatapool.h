#ifndef LOCATIONDATAPOOL_H
#define LOCATIONDATAPOOL_H

#include "locationdataptr.h"
#include "objectpool.h"

namespace SKT {

class LocationDataPool
{
public:
    static LocationDataPtr newDataPtr() {
        LocationData *location = s_fixedPool.acquire();
        if (!location) {
            // create a new one if pool is empty
            location = new LocationData;
        }
        return LocationDataPtr(location, returnToPool);
    }

private:
    static FixedPool<LocationData> s_fixedPool;

    static void returnToPool(LocationData* d) {
        if (d) {
            if (!s_fixedPool.release(d)) {
                // release here if pool is full
                delete d;
            }
        }
    }
};

}

#endif // LOCATIONDATAPOOL_H
