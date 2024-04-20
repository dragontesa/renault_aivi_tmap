#ifndef RGSIMULATOR_H
#define RGSIMULATOR_H

#include <QObject>
#include "simulatorinterface.h"

namespace SKT {

class RGSimulatorWorker;
class RGSimulator : public QObject, public SimulatorInterface
{
    Q_OBJECT
    Q_INTERFACES(SKT::SimulatorInterface)

    Q_PROPERTY(int speed READ speed WRITE setSpeed NOTIFY speedChanged)
    Q_PROPERTY(bool repeat READ repeat WRITE setRepeat NOTIFY repeatChanged)
    Q_PROPERTY(bool pause READ pause WRITE setPause NOTIFY pauseChanged)

public:
    explicit RGSimulator(QObject* parent = 0);
    virtual ~RGSimulator();

    QObject* qobject() override { return this; }

    int speed() const override { return m_speed; }
    bool repeat() const override { return m_repeat; }
    bool pause() const override { return m_pause; }

public slots:
    void start() override;
    void stop() override;
    void setSpeed(int speed) override;
    void goNextGP() override;
    void goPreviousGP() override;
    void goTo(int dist) override;
    void setPause(bool pause) override;
    void setRepeat(bool repeat) override;

signals:
    void started();
    void stopped();
    void speedChanged(int speed);
    void repeatChanged(bool repeat);
    void pauseChanged(bool pause);
    void routeGuidanceUpdated(SKT::RGDataPtr rgData);
    void remainDistanceChanged(int distance);

private:
    void post(int action, int arg1, int arg2);

private:
    int m_speed;
    bool m_repeat;
    bool m_pause;

    RGSimulatorWorker *m_worker;
};

}

#endif // RGSIMULATOR_H
