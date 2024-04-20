#include "rgdatapool.h"

#define RGDATA_POOL_SIZE      5

namespace SKT {

FixedPool<VSM_stDriveInfo> RGDataPool::s_fixedPool(RGDATA_POOL_SIZE);

}
