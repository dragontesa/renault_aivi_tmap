#ifndef WIDGETSMALLVIEWMEDIATOR_H
#define WIDGETSMALLVIEWMEDIATOR_H

#include "mediator/AbstractViewMediator.h"
#include "RouteGuidanceProxy.h"

namespace SKT {

class WidgetSmallViewMediator : public AbstractViewMediator
{
    Q_OBJECT
    Q_ENUMS( EventK )
public:
    enum EventK {
        Mediated = 0,
        OnDestruction
    };

    Q_INVOKABLE WidgetSmallViewMediator( QObject* viewComponent = nullptr );

    ~WidgetSmallViewMediator();

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

#endif // WIDGETSMALLVIEWMEDIATOR_H
