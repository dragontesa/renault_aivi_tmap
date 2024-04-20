#ifndef ROUTESUMMARYVIEWMEDIATOR_H
#define ROUTESUMMARYVIEWMEDIATOR_H

#include "mediator/AbstractViewMediator.h"

class QObject;
class QVariant;

namespace SKT {

class RouteSummaryViewMediator : public AbstractViewMediator
{
    Q_OBJECT
    Q_ENUMS( EventK )
public:
    enum EventK {
        Mediated,
        SummaryItemClicked,
        ViewChanged,
        Activated,
        Deactivated,
        RegistCSW,
        RemoveCSW,
        SelectItem,
        CheckItemInMapView,
        DrawRouteAll,
        RequestMapRect,
        RetranslateUi,
    };
    enum MenuK {
        MenuSimulationDriving = 0,
        MenuStart
    };

    Q_INVOKABLE RouteSummaryViewMediator( QObject* viewComponent = nullptr );
    virtual ~RouteSummaryViewMediator();

    QList<int> listNotificationInterests();

protected:
    bool onEvent( int event, const QVariant& data );
    bool handleNotification( const mvc::INotificationPtr& note );

private:
    void procSoftkey(const QVariant &data);
    void openSimulationView();

    void close();

private:
    bool mStartRgMenuChanged;
    int mCurrentSpeed = 0;
    QRect mMapRect;
    QObject* mapView = nullptr;
};

}

#endif // ROUTESUMMARYVIEWMEDIATOR_H
