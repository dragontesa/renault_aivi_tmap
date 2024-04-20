#ifndef RGDATAPOOL_H
#define RGDATAPOOL_H

#include "rgdataptr.h"
#include "objectpool.h"

namespace SKT {

class RGDataPool
{
public:
    static RGDataPtr newRGDataPtr() {
        VSM_stDriveInfo *rgData = s_fixedPool.acquire();
        if (!rgData) {
            // create a new one if pool is empty
            rgData = new VSM_stDriveInfo;
        }
        memset(rgData, 0, sizeof(VSM_stDriveInfo));
        return RGDataPtr(rgData, returnToPool);
    }

private:
    static FixedPool<VSM_stDriveInfo> s_fixedPool;

    static void returnToPool(VSM_stDriveInfo* d) {
        if (d) {
            if (!s_fixedPool.release(d)) {
                // release here if pool is full
                delete d;
            }
        }
    }
};

}

#endif // RGDATAPOOL_H
