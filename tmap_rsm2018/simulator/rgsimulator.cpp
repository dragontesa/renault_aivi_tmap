#include <QDebug>
#include <QThread>
#include <QTimer>

#include "rgsimulator.h"
#include "rgdatapool.h"

#include "VSM_Navi.h"


#define SIMULATOR_DEFAULT_SPEED     70
#define SIMULATOR_DEFAULT_REPEAT    true

namespace SKT {

class RGSimulatorWorker : public QObject
{
    Q_OBJECT
public:
    explicit RGSimulatorWorker(RGSimulator *simulator)
        : m_simulator(simulator)
        , m_step(VSM_SimulationStart)
        , m_speed(SIMULATOR_DEFAULT_SPEED)
        , m_moveDist(0)
        , m_repeat(SIMULATOR_DEFAULT_REPEAT)
        , m_paused(false)
        , m_timer(NULL)
    {

    }

    enum SimulatorAction {
        UpdateSpeed,
        UpdateRepeat,
        UpdateStep,
        UpdatePause
    };

public slots:
    void doStarted();
    void doFinished();
    void doJob(int action, int arg1, int arg2);
    void doStep();

private:
    RGSimulator *m_simulator;
    VSM_SimulationOption m_step;
    int m_speed;
    int m_moveDist;
    bool m_repeat;
    bool m_paused;

    QTimer *m_timer;
};

RGSimulator::RGSimulator(QObject* parent)
    : QObject(parent)
    , m_speed(SIMULATOR_DEFAULT_SPEED)
    , m_repeat(SIMULATOR_DEFAULT_REPEAT)
    , m_pause(false)
    , m_worker(0)
{

}

RGSimulator::~RGSimulator()
{
    stop();
}

void RGSimulator::start()
{
    if (!m_worker) {
        QThread* thread = new QThread();
        m_worker = new RGSimulatorWorker(this);
        m_worker->moveToThread(thread);

        connect(thread, &QThread::started, m_worker, &RGSimulatorWorker::doStarted);
        connect(thread, &QThread::finished, m_worker, &RGSimulatorWorker::doFinished);
        connect(thread, &QThread::finished, m_worker, &RGSimulatorWorker::deleteLater);
        connect(thread, &QThread::finished, thread, &QThread::deleteLater);

        emit started();

        thread->start();
    }
}

void RGSimulator::stop()
{
    if (m_worker) {
        QThread* thread = m_worker->thread();
        thread->quit();
        thread->wait();

        m_worker = 0;

        emit stopped();
    }
}

void RGSimulator::setSpeed(int speed)
{
    if (m_speed != speed) {
        m_speed = speed;
        post(RGSimulatorWorker::UpdateSpeed, speed, 0);
        emit speedChanged(speed);
    }
}

void RGSimulator::goNextGP()
{
    post(RGSimulatorWorker::UpdateStep, VSM_SimulationNextGP, 0);
}

void RGSimulator::goPreviousGP()
{
    post(RGSimulatorWorker::UpdateStep, VSM_SimulationPrevGP, 0);
}

void RGSimulator::goTo(int dist)
{
    post(RGSimulatorWorker::UpdateStep, VSM_SimulationPos, dist);
}

void RGSimulator::setPause(bool pause)
{
    if (m_pause != pause) {
        m_pause = pause;
        post(RGSimulatorWorker::UpdatePause, pause, 0);
        emit pauseChanged(pause);
    }
}

void RGSimulator::setRepeat(bool repeat)
{
    if (m_repeat != repeat) {
        m_repeat = repeat;
        post(RGSimulatorWorker::UpdateRepeat, repeat, 0);
        emit repeatChanged(repeat);
    }
}

void RGSimulator::post(int action, int arg1, int arg2)
{
    if (m_worker) {
        // add a new job into worker queue.
        QMetaObject::invokeMethod(m_worker,
                                  "doJob",
                                  Q_ARG(int, action),
                                  Q_ARG(int, arg1),
                                  Q_ARG(int, arg2));
    }
}

void RGSimulatorWorker::doStarted()
{
    m_timer = new QTimer();
    m_timer->setTimerType(Qt::PreciseTimer);
    m_timer->setInterval(1000);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(doStep()));
    m_timer->start();

    doStep();
}

void RGSimulatorWorker::doFinished()
{
    if (m_timer) {
        delete m_timer;
        m_timer = NULL;
    }
}

void RGSimulatorWorker::doJob(int action, int arg1, int arg2)
{
    bool applyNow = false;

    // process job
    switch (action) {
    case UpdateSpeed:
        m_speed = arg1;
        break;
    case UpdateRepeat:
        m_repeat = arg1;
        break;
    case UpdateStep:
        m_step = (VSM_SimulationOption)arg1;
        m_moveDist = arg2;
        applyNow = true;
        break;
    case UpdatePause:
        m_paused = arg1;
        if (m_timer) {
            if (m_paused) {
                m_timer->stop();
            } else {
                m_timer->start();
                applyNow = true;
            }
        }
        break;
    default:
        // no-op
        break;
    }

    if (applyNow) {
        doStep();
    }
}

void RGSimulatorWorker::doStep()
{
    RGDataPtr rgData = RGDataPool::newRGDataPtr();

    if (!VSMNavi_GetRouteGuidanceSimulator(m_step, m_speed, m_moveDist, *rgData)) {
        return;
    }

    // emit signal
    emit m_simulator->routeGuidanceUpdated(rgData);

    emit m_simulator->remainDistanceChanged(rgData->nTotalDist);

    // do next step
    VSM_SimulationOption nextStep;

    switch (m_step) {
    case VSM_SimulationStart:
        nextStep = VSM_SimulationFirst;
        break;
    case VSM_SimulationFirst:
        nextStep = VSM_SimulationNextStep;
        break;
    case VSM_SimulationNextStep:
    default:
        nextStep = VSM_SimulationNextStep;
        if (m_repeat && rgData->nTotalDist <= 0) {
            nextStep = VSM_SimulationFirst;
        }
        break;
    }

    m_step = nextStep;
}

}

#include "rgsimulator.moc"
