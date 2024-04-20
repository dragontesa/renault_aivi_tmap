#include "PreciseTimer.h"
#include <QTimer>

namespace SKT {

static const QEvent::Type PreciseTimerStart = QEvent::Type(QEvent::User + 1);
static const QEvent::Type PreciseTimerStop = QEvent::Type(QEvent::User + 2);
static const QEvent::Type PreciseTimerUpdate = QEvent::Type(QEvent::User + 3);
static const QString PreciseTimerInterval = QStringLiteral("PreciseTimerInterval");
static const QString PreciseTimerRepeat = QStringLiteral("PreciseTimerRepeat");
static const QString PreciseTimerRunning = QStringLiteral("PreciseTimerRunning");
static const QString PreciseTimerTriggeredOnStart = QStringLiteral("PreciseTimerTriggeredOnStart");

class PreciseTimerEvent : public QEvent
{
public:
    PreciseTimerEvent( QEvent::Type type, const QVariant& args = QVariant() )
        : QEvent( type )
        , mArgs( args ) {
    }

    const QVariant mArgs;
};

class PreciseTimerWorker : public QObject
{
    Q_OBJECT
public:
    PreciseTimerWorker( QObject* parent = nullptr ) : QObject( parent ) {

    }

    ~PreciseTimerWorker() {
        qDebug() << "[PreciseTimer][Worker]~PreciseTimerWorker thread: " << QThread::currentThread();
    }

signals:
    void timeout();
    void threadChanged();

private:
    virtual bool event( QEvent* event ) {
        int type = event->type();
        switch ( type ) {
        case QEvent::ThreadChange: {                
                qDebug() << "[PreciseTimer][Worker]event(ThreadChange) thread: " << QThread::currentThread();

                emit threadChanged();
            }
            break;
        case PreciseTimerUpdate: {
                qDebug() << "[PreciseTimer][Worker]event(PreciseTimerUpdate) thread: " << QThread::currentThread();

                auto e = dynamic_cast<PreciseTimerEvent*>( event );
                auto args = e->mArgs.toMap();
                auto interval = args[PreciseTimerInterval].toInt();
                auto repeat = args[PreciseTimerRepeat].toBool();
                auto running = args[PreciseTimerRunning].toBool();
                auto triggeredOnStart = args[PreciseTimerTriggeredOnStart].toBool();

                if ( !mTimer ) {
                    mTimer = new QTimer( this );
                    mTimer->setTimerType( Qt::PreciseTimer );
                    connect( mTimer, &QTimer::timeout, this, &PreciseTimerWorker::onTimeout );
                }

                qDebug() << "[PreciseTimer][Worker]event(PreciseTimerUpdate) interval: " << interval << " repeat: " << " running: " << running << " triggeredOnStart: " << triggeredOnStart;

                mTimer->stop();

                if ( running ) {
                    mTimer->setInterval( interval );
                    mTimer->setSingleShot( !repeat );
                    mTimer->start();

                    if ( triggeredOnStart ) {
                        emit timeout();
                    }
                }
            }
            break;
        case PreciseTimerStart: {
                mTimer->start();
            }
            break;
        case PreciseTimerStop: {
                mTimer->stop();
            }
            break;
        }

        return false;
    }

    void onTimeout() {
        qDebug() << "[PreciseTimer][Worker]onTimeout thread: " << QThread::currentThread();

        emit timeout();
    }

private:
    QTimer* mTimer = nullptr;
};

}

using namespace SKT;

PreciseTimer::PreciseTimer( QObject* parent )
    : QObject( parent )
{
}

PreciseTimer::~PreciseTimer()
{
    qDebug() << "[PreciseTimer]~PreciseTimer thread: " << QThread::currentThread();

    if ( !mWorker ) {
        delete mWorker;
        mWorker = nullptr;
    }
}

void PreciseTimer::classBegin()
{

}

void PreciseTimer::componentComplete()
{
    qDebug() << "[PreciseTimer]componentComplete thread: " << QThread::currentThread();

    mComponentComplete = true;

    update();
}

int PreciseTimer::interval() const
{
    return mInterval;
}

void PreciseTimer::setInterval( int value )
{   
    if ( mInterval != value ) {
        mInterval = value;

        update();
    }
}

bool PreciseTimer::running() const
{
    return mRunning;
}

void PreciseTimer::setRunning( bool value )
{
    qDebug() << "[PreciseTimer]setRunning value: " << value << " thread: " << QThread::currentThread();

    if ( mRunning != value ) {
        mRunning = value;

        update();
    }
}

bool PreciseTimer::repeat() const
{
    return mRepeat;
}

void PreciseTimer::setRepeat( bool value )
{
    if ( mRunning != value ) {
        mRepeat = value;

        update();
    }
}

bool PreciseTimer::triggeredOnStart() const
{
    return mTriggeredOnStart;
}

void PreciseTimer::setTriggeredOnStart( bool value )
{
    mTriggeredOnStart = value;
}

QVariant PreciseTimer::makeArguments()
{
    auto args = QVariantMap{
        {PreciseTimerInterval, mInterval},
        {PreciseTimerRunning, mRunning},
        {PreciseTimerRepeat, mRepeat},
        {PreciseTimerTriggeredOnStart, mTriggeredOnStart}
    };

    return args;
}

void PreciseTimer::update()
{
    if ( !mComponentComplete ) {
        return;
    }

    if ( !mWorker ) {
        mPostUpdate = true;

        mWorker = new PreciseTimerWorker();
        QObject::connect( mWorker, &PreciseTimerWorker::timeout, this, &PreciseTimer::onTimeout );
        QObject::connect( mWorker, &PreciseTimerWorker::threadChanged, this, &PreciseTimer::onWorkerThreadChanged );

        mThread = new QThread( this );
        mWorker->moveToThread( mThread );
        mThread->start();
    }
    else {
        QCoreApplication::postEvent( mWorker, new PreciseTimerEvent( PreciseTimerUpdate, makeArguments() ) );
    }
}

void PreciseTimer::onTimeout()
{
    qDebug() << "[PreciseTimer]onTimeout thread: " << QThread::currentThread();

    emit triggered();
}

void PreciseTimer::onWorkerThreadChanged()
{
    qDebug() << "[PreciseTimer]onWorkerThreadChanged thread: " << QThread::currentThread();

    if ( mPostUpdate ) {
        QCoreApplication::postEvent( mWorker, new PreciseTimerEvent( PreciseTimerUpdate, makeArguments() ) );
        mPostUpdate = false;
    }
}

#include "PreciseTimer.moc"

