#include "logger.h"

namespace SKT {

class LoggerDummy : public PlatformLogger
{
public:
    LoggerDummy() {}
    void init() override {}
    void cleanUp() override {}
};

PlatformLogger* Logger::newPlatformLogger()
{
    return new LoggerDummy;
}

}

