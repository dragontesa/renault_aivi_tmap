#ifndef SIMULATIONVIEWMEDIATOR_H
#define SIMULATIONVIEWMEDIATOR_H

#include "mediator/AbstractViewMediator.h"
#include <QList>

class QObject;
class QVariant;

namespace SKT {

class SimulationViewMediator : public AbstractViewMediator {
    Q_OBJECT

    Q_ENUMS( EventK )
public:
    enum EventK {
        Mediated,
        Activated,
        Deactivated,
        ResetTimer,
        SetupTitle,
        VolumeClicked,
    };
    Q_INVOKABLE SimulationViewMediator(QObject *viewComponent = 0);
    ~SimulationViewMediator();

    QList<int> listNotificationInterests();

protected:
    virtual bool onEvent(int event, const QVariant &data);
    virtual bool handleNotification(const mvc::INotificationPtr &noti);

private:
    void procSoftkey(const QVariant &data);
    void close();
    void applySetting(const QVariant& v);
    void goBack(bool cancelRoute=false);

private:
    bool mUseSpeedResponseMap = false;
    bool mMapViewLevelInitialized = false;
    bool mAppForeground = true;
};

}

#endif // SIMULATIONVIEWMEDIATOR_H
