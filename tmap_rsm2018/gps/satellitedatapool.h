#ifndef SATELLITEDATAPOOL_H
#define SATELLITEDATAPOOL_H

#include "satellitedataptr.h"
#include "objectpool.h"

namespace SKT {

class SatelliteDataPool
{
public:
    static SatelliteDataPtr newDataPtr() {
        SatelliteData *sat = s_fixedPool.acquire();
        if (!sat) {
            // create a new one if pool is empty
            sat = new SatelliteData;
        }
        return SatelliteDataPtr(sat, returnToPool);
    }

private:
    static FixedPool<SatelliteData> s_fixedPool;

    static void returnToPool(SatelliteData* d) {
        if (d) {
            if (!s_fixedPool.release(d)) {
                // release here if pool is full
                delete d;
            }
        }
    }
};

}

#endif // SATELLITEDATAPOOL_H
