#ifndef OBJECTPOOL_H
#define OBJECTPOOL_H

#include <QMutex>
#include <QVector>

namespace SKT {

template <typename T>
class ObjectPool
{
public:
    virtual T* acquire() = 0;
    virtual bool release(T* instance) = 0;
};

template <typename T>
class FixedPool : public ObjectPool<T>
{
public:
    FixedPool(int poolSize) :
        m_maxPoolSize(poolSize),
        m_pool(new QVector<T*>()) {
        // reserve
        m_pool->reserve(m_maxPoolSize);
    }

    ~FixedPool() {
        // clear all objects in the pool
        typename QVector<T*>::iterator it = m_pool->begin();
        while (it != m_pool->end()) {
            T* in = *it;
            if (in) {
                delete in;
            }
            ++it;
        }
        delete m_pool;
    }

    // will not call constructor
    T* acquire() override {
        m_mutex.lock();
        if (m_pool->size() > 0) {
            T* instance = m_pool->back();
            m_pool->pop_back();
            m_mutex.unlock();
            return instance;
        }
        m_mutex.unlock();

        // if pool is empty
        return NULL;
    }

    bool release(T* instance) override {
        m_mutex.lock();
        if (m_pool->size() < m_maxPoolSize) {
            m_pool->push_back(instance);
            m_mutex.unlock();
            return true;
        }
        m_mutex.unlock();

        // if pool is full
        return false;
    }

private:
    const int m_maxPoolSize;
    QVector<T*> *m_pool;
    QMutex m_mutex;
};

}

#endif // OBJECTPOOL_H
