#ifndef _PLATFORMS_LINUX_GEN3_PLATFORM_CONNECTIVITY_GEN3_H_
#define _PLATFORMS_LINUX_GEN3_PLATFORM_CONNECTIVITY_GEN3_H_
#include <openssl/engine.h>
#include <openssl/err.h>
#include <openssl/ssl.h>
#include <openssl/conf.h>
#include <openssl/x509v3.h>
#include "platform_connectivity.h"

namespace SKT {
class PlatformConnectivityGen3: public PlatformConnectivity
{
    public:
    PlatformConnectivityGen3();
    virtual ~PlatformConnectivityGen3();
    int init() override;
    int release() override;

    private:
    ENGINE* mEngine;
};
}
#endif /* _PLATFORMS_LINUX_GEN3_PLATFORM_CONNECTIVITY_GEN3_H_ */
