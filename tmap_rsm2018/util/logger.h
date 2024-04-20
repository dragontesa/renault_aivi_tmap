#ifndef LOGGER_H
#define LOGGER_H

namespace SKT {

class PlatformLogger
{
public:
    virtual ~PlatformLogger() {}
    virtual void init() = 0;
    virtual void cleanUp() = 0;
};

class Logger
{
public:
    Logger();
    ~Logger();

    void init();
    void cleanUp();

private:
    PlatformLogger* newPlatformLogger();

private:
    PlatformLogger* m_platformLogger;
};

}

#endif // LOGGER_H
