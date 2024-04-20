#ifndef PERIODICREROUTETIMER_H
#define PERIODICREROUTETIMER_H

#include <QObject>

namespace SKT
{

class PeroidicRerouteTimer : public QObject
{
    Q_OBJECT

public:
    PeroidicRerouteTimer(QObject* parent = nullptr);

    int getRerouteTime() const;
    void setValue(int value);
    void setTimerValueForDebugMode(int value);

signals:
    void timeout();

private:
    int mValue;
    int mTimerForDebugMode;
};

}

#endif // PERIODICREROUTETIMER_H
