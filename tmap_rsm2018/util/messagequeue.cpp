#include "messagequeue.h"

#define MESSAGEQUEUE_POOL_SIZE        10

namespace SKT {

MessageQueue::MessageQueue() :
    m_pool(NULL),
    m_messages(NULL),
    m_poolSize(0),
    m_closed(false),
    m_blocked(false)
{
    m_elapsedTimer.start();
}

MessageQueue::~MessageQueue()
{
    // release all queued messages
    removeAllMessagesLocked();

    // clear pool
    Message *p = m_pool;
    while (p) {
        Message *n = p;
        p = p->next;
        delete n;
    }
}

bool MessageQueue::sendEmptyMessageDelayed(int what, unsigned long delay)
{
    Message *msg = obtainMessage();

    msg->what = what;

    return sendMessageDelayed(msg, delay);
}

bool MessageQueue::sendMessageDelayed(Message *msg, unsigned long delay)
{
    if (!msg) {
        return false;
    }

    unsigned long tick = m_elapsedTimer.elapsed();
    if (tick == 0) {
        // WORKAROUND: ensure tick must not be zero!
        tick = 1;
    }

    return enqueueMessage(msg, tick + delay);
}

bool MessageQueue::sendMessageAtFront(Message *msg)
{
    if (!msg) {
        return false;
    }

    return enqueueMessage(msg, 0);
}

Message *MessageQueue::next()
{
    unsigned long nextPollTimeout = 0;

    forever {
        QMutexLocker locker(&m_lock);

        if (nextPollTimeout != 0) {
            m_waitCond.wait(locker.mutex(), nextPollTimeout);
        }

        unsigned long now = m_elapsedTimer.elapsed();

        Message *msg = m_messages;
        if (msg) {
            unsigned long when = msg->when;
            if (now >= when) {
                m_blocked = false;
                m_messages = msg->next;
                msg->next = NULL;

                return msg;
            } else {
                nextPollTimeout = when - now;
            }
        } else {
            nextPollTimeout = ULONG_MAX;
        }

        if (m_closed) {
            return NULL;
        }

        m_blocked = true;
    }
}

bool MessageQueue::removeMessages(int what)
{
    bool found = false;

    QMutexLocker lock(&m_lock);

    Message *p = m_messages;

    while (p && p->what == what) {
        found = true;
        Message *n = p->next;
        m_messages = n;
        recycleMessage(p);
        p = n;
    }

    while (p) {
        Message *n = p->next;
        if (n) {
            if (n->what == what) {
                found = true;
                Message *nn = n->next;
                recycleMessage(n);
                p->next = nn;
                continue;
            }
        }
        p = n;
    }

    return found;
}

void MessageQueue::quit()
{
    QMutexLocker lock(&m_lock);

    if (m_closed)
        return;

    m_closed = true;
    removeFutureMessagesLocked();

    // wake
    m_waitCond.wakeAll();
}

void MessageQueue::removeAllMessagesLocked()
{
    Message *p = m_messages;
    while (p) {
        Message *n = p->next;
        recycleMessage(p);
        p = n;
    }
    m_messages = NULL;
}

void MessageQueue::removeFutureMessagesLocked()
{
    unsigned long now = m_elapsedTimer.elapsed();

    Message *p = m_messages;
    if (!p) {
        return;
    }

    if (p->when > now) {
        removeAllMessagesLocked();
    } else {
        Message *n;
        for (;;) {
            n = p->next;
            if (!n) {
                return;
            }
            if (n->when > now) {
                break;
            }
            p = n;
        }
        p->next = NULL;
        do {
            p = n;
            n = p->next;
            recycleMessage(p);
        } while (n);
    }
}

bool MessageQueue::enqueueMessage(Message *msg, unsigned long when)
{
    bool needWake = false;

    if (!msg) {
        return false;
    }

    QMutexLocker lock(&m_lock);

    if (m_closed) {
        recycleMessage(msg);
        return false;
    }

    msg->when = when;
    Message *p = m_messages;
    if (!p || when == 0 || when < p->when) {
        msg->next = p;
        m_messages = msg;

        // no need to wake up if not blocked
        needWake = m_blocked;
    } else {
        Message *prev = NULL;
        while (p && p->when <= when) {
            prev = p;
            p = p->next;
        }
        msg->next = p;
        if (prev) {
            prev->next = msg;
        }

        needWake = false;
    }

    // wake up
    if (needWake)
         m_waitCond.wakeAll();

    return true;
}

Message* MessageQueue::obtainMessage()
{
    m_poolLock.lock();

    if (m_pool) {
        Message *p = m_pool;
        m_pool = m_pool->next;
        --m_poolSize;

        m_poolLock.unlock();
        return p;
    }

    m_poolLock.unlock();

    return newMessage();
}

Message *MessageQueue::obtainMessage(int what)
{
    Message *message = obtainMessage();

    message->what = what;

    return message;
}

Message *MessageQueue::obtainMessage(int what, int wparam, long lparam)
{
    Message *message = obtainMessage();

    message->what = what;
    message->wparam = wparam;
    message->lparam = lparam;

    return message;
}

void MessageQueue::recycleMessage(Message* p)
{
    if (!p) {
        return;
    }

    p->recycled();

    m_poolLock.lock();

    if (m_poolSize < MESSAGEQUEUE_POOL_SIZE) {
        p->next = m_pool;
        m_pool = p;
        ++m_poolSize;

        m_poolLock.unlock();
        return;
    }

    m_poolLock.unlock();

    delete p;
}

}
