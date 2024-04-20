#ifndef _PLATFORMS_DUMMY_PLATFORM_CONNECTIVITY_DUMMY_H_
#define _PLATFORMS_DUMMY_PLATFORM_CONNECTIVITY_DUMMY_H_

#include "platform_connectivity.h"

namespace SKT {
class PlatformConnectivityDummy: public PlatformConnectivity
{
public:
    PlatformConnectivityDummy();
    virtual ~PlatformConnectivityDummy();
    int init() override;
    int release() override;

};
}
#endif /* _PLATFORMS_DUMMY_PLATFORM_CONNECTIVITY_DUMMY_H_ */
