#ifndef SKYSTATEMEDIATOR_H
#define SKYSTATEMEDIATOR_H

#include "mediator/AbstractViewMediator.h"
#include "navigationcontroller.h"

namespace SKT
{

class SkyStateMediator : public AbstractViewMediator
{
    Q_OBJECT

public:
    Q_INVOKABLE SkyStateMediator( QObject* viewComponent = nullptr );

    QList<int> listNotificationInterests();

protected:
    bool handleNotification( const mvc::INotificationPtr& note );

private:
    void updateSkyState();
    void parseNightMode(const QVariantMap &m);
    void printSkyState(int skyState);

private:
    int mNightMode;
    NavigationController::SunState mSunState;
    NavigationController::WeatherState mWeatherState;
};

}

#endif // SKYSTATEMEDIATOR_H
