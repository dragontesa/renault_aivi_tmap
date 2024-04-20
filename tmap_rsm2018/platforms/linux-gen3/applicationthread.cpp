#include <QCoreApplication>
#include "applicationthread.h"

namespace SKT {

ApplicationThread::ApplicationThread() :
    m_thread(0),
    m_application(NULL),
    m_started(false)
{
    pthread_mutex_init(&m_mutex, NULL);
    pthread_cond_init(&m_condVar, NULL);
}

ApplicationThread::~ApplicationThread()
{
    quit();

    pthread_cond_destroy(&m_condVar);
    pthread_mutex_destroy(&m_mutex);
}

void ApplicationThread::start()
{
    if (!m_thread) {
        int thread_id = pthread_create(&m_thread, NULL, runApplication, this);
        if (thread_id < 0) {
            // error
        }
    }
}

void ApplicationThread::quit()
{
    if (m_thread) {
        // wait until QApplication is created.
        pthread_mutex_lock(&m_mutex);
        while (!m_started) {
            pthread_cond_wait(&m_condVar, &m_mutex);
        }
        if (m_application) {
            QMetaObject::invokeMethod(m_application, "quit", Qt::QueuedConnection);
        }
        pthread_mutex_unlock(&m_mutex);

        // wait until thread is terminated.
        pthread_join(m_thread, NULL);
        m_thread = 0;
    }

    m_started = false;
}

void ApplicationThread::waitUntilTerminated()
{
    if (m_thread) {
        // wait until thread is terminated.
        pthread_join(m_thread, NULL);
        m_thread = 0;
    }

    m_started = false;
}

void* ApplicationThread::runApplication(void* data)
{
    ApplicationThread* appThread = reinterpret_cast<ApplicationThread*>(data);
    QCoreApplication* app = appThread->createApplication();

    pthread_mutex_lock(&appThread->m_mutex);
    appThread->m_application = app;
    appThread->m_started = true;
    pthread_cond_signal(&appThread->m_condVar);
    pthread_mutex_unlock(&appThread->m_mutex);

    if (app) {
        // run main loop
        app->exec();
    }

    pthread_mutex_lock(&appThread->m_mutex);
    appThread->m_application = NULL;
    pthread_mutex_unlock(&appThread->m_mutex);

    if (app) {
        delete app;
    }

    return NULL;
}

}
