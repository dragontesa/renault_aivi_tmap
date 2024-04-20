#ifndef WIDGETMEDIUMVIEWMEDIATOR_H
#define WIDGETMEDIUMVIEWMEDIATOR_H

#include "mediator/AbstractViewMediator.h"
#include "RouteGuidanceProxy.h"

namespace SKT {

class WidgetMediumViewMediator : public AbstractViewMediator
{
    Q_OBJECT

    Q_ENUMS(EventK)

public:
    enum EventK {
        Mediated = 0,
        OnDestruction,
        SearchClicked
    };

    Q_INVOKABLE WidgetMediumViewMediator( QObject* viewComponent = nullptr );

    ~WidgetMediumViewMediator();

    QList<int> listNotificationInterests();

protected:
    bool onEvent( int event, const QVariant& data );

    bool handleNotification( const mvc::INotificationPtr& note );

private:
    void parseGpsTime(const QVariant& v);
    void updateLanguage();

    RouteGuidanceProxyPtr mRgProxy;
};

}

#endif // WIDGETMEDIUMVIEWMEDIATOR_H
