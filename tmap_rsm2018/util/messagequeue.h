#ifndef MESSAGEQUEUE_H
#define MESSAGEQUEUE_H

#include <QMutex>
#include <QWaitCondition>
#include <QElapsedTimer>

namespace SKT {

class MessageQueue;

/**
 * @brief The Base Message class
 */
class Message {
public:
    Message() : what(0), wparam(0), lparam(0), when(0), next(NULL) {}
    virtual ~Message() {}

    int what;
    int wparam;
    long lparam;

protected:
    virtual void recycled() {
        what = 0;
        wparam = 0;
        lparam = 0;
        when = 0;
    }

protected:
    unsigned long when;

private:
    Message *next;

    friend class MessageQueue;
};

/**
 * @brief The MessageQueue class
 */
class MessageQueue
{
public:
    MessageQueue();
    virtual ~MessageQueue();

    Message *next();

    bool sendEmptyMessage(int what) { return sendEmptyMessageDelayed(what, 0); }
    bool sendEmptyMessageDelayed(int what, unsigned long delay);

    bool sendMessage(Message *msg) { return sendMessageDelayed(msg, 0); }
    bool sendMessageDelayed(Message *msg, unsigned long delay);
    bool sendMessageAtFront(Message *msg);

    Message *obtainMessage();
    Message *obtainMessage(int what);
    Message *obtainMessage(int what, int wparam, long lparam);

    bool removeMessages(int what);
    void recycleMessage(Message *p);
    void quit();

protected:
    // support custom message
    virtual Message *newMessage() const { return new Message(); }

private:
    bool enqueueMessage(Message *msg, unsigned long when);
    void removeFutureMessagesLocked();
    void removeAllMessagesLocked();

private:
    Message *m_pool;
    Message *m_messages;

    QMutex m_poolLock;
    int m_poolSize;
    QMutex m_lock;
    QWaitCondition m_waitCond;
    bool m_closed;
    bool m_blocked;

    QElapsedTimer m_elapsedTimer;
};

}

#endif // MESSAGEQUEUE_H
