#include <QSslSocket>
#include <openssl/crypto.h>
#include <pthread.h>

namespace opensslthreadlock {

#if OPENSSL_VERSION_NUMBER < 0x10100000L    // < 1.1.0

static pthread_mutex_t *lockarray = nullptr;

static void lock_callback(int mode, int type, const char *file, int line)
{
    (void)file;
    (void)line;
    if (mode & CRYPTO_LOCK) {
        pthread_mutex_lock(&(lockarray[type]));
    } else {
        pthread_mutex_unlock(&(lockarray[type]));
  }
}

#if OPENSSL_VERSION_NUMBER >= 0x10000000L   // >= 1.0.0
static void thread_id(CRYPTO_THREADID* tid)
{
    CRYPTO_THREADID_set_numeric(tid, (unsigned long)pthread_self());
}
#else
static unsigned long thread_id()
{
    return (unsigned long)pthread_self();
}
#endif

void setup()
{
#ifndef QT_NO_SSL
    if (QSslSocket::supportsSsl()) {
        // Qt will set callbacks internally.
        // refer to https://code.qt.io/cgit/qt/qtbase.git/tree/src/network/ssl/qsslsocket_openssl.cpp?h=5.6
        qDebug() << "QSslSocket::supportsSsl() = true";
        return;
    }
#endif

    int num_locks = CRYPTO_num_locks();
    lockarray = new pthread_mutex_t[num_locks];
    for (int i = 0; i < num_locks; i++) {
        pthread_mutex_init(&(lockarray[i]), nullptr);
    }
#if OPENSSL_VERSION_NUMBER >= 0x10000000L   // >= 1.0.0
    CRYPTO_THREADID_set_callback(thread_id);
#else
    CRYPTO_set_id_callback(thread_id);
#endif
    CRYPTO_set_locking_callback(lock_callback);
}

void cleanup()
{
    if (lockarray) {
        int num_locks = CRYPTO_num_locks();
        CRYPTO_set_locking_callback(nullptr);
        for (int i = 0; i < num_locks; i++) {
            pthread_mutex_destroy(&(lockarray[i]));
        }
        delete [] lockarray;
        lockarray = nullptr;
    }
}

#else

void setup()
{

}

void cleanup()
{

}

#endif

}   // namespace opensslthreadlock
