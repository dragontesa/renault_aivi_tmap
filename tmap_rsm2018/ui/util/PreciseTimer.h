#ifndef PRECISETIMER_H
#define PRECISETIMER_H

#include <QQmlParserStatus>

namespace SKT {

class PreciseTimerWorker;

class PreciseTimer : public QObject, public QQmlParserStatus {
    Q_OBJECT
    Q_PROPERTY(int interval READ interval WRITE setInterval)
    Q_PROPERTY(bool running READ running WRITE setRunning)
    Q_PROPERTY(bool repeat READ repeat WRITE setRepeat)
    Q_PROPERTY(bool triggeredOnStart READ triggeredOnStart WRITE setTriggeredOnStart)
public:
    PreciseTimer( QObject* parent = nullptr );
    ~PreciseTimer();

signals:
    void triggered();

public slots:
    int interval() const;
    void setInterval( int value );
    bool running() const;
    void setRunning( bool value );
    bool repeat() const;
    void setRepeat( bool value );
    bool triggeredOnStart() const;
    void setTriggeredOnStart( bool value );    

private:
    void classBegin();
    void componentComplete();

    void update();

    void onTimeout();
    void onWorkerThreadChanged();

    QVariant makeArguments();

    int mInterval = 1000;
    bool mRunning = false;
    bool mRepeat = true;
    PreciseTimerWorker* mWorker = nullptr;
    QThread* mThread = nullptr;
    bool mPostUpdate = false;
    bool mComponentComplete = false;
    bool mTriggeredOnStart = false;
};

}

#endif // PRECISETIMER_H
