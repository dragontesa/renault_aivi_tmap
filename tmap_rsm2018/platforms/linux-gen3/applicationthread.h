#ifndef APPLICATIONTHREAD_H
#define APPLICATIONTHREAD_H

#include <pthread.h>

class QCoreApplication;

namespace SKT {

class ApplicationThread
{
public:
    ApplicationThread();
    virtual ~ApplicationThread();

    void start();
    void quit();
    void waitUntilTerminated();

protected:
    // called by Application Thread
    virtual QCoreApplication* createApplication() { return NULL; }

private:
    pthread_t m_thread;
    pthread_mutex_t m_mutex;
    pthread_cond_t m_condVar;
    static void* runApplication(void* data);

    QCoreApplication* m_application;
    bool m_started;
};

}

#endif // APPLICATIONTHREAD_H
