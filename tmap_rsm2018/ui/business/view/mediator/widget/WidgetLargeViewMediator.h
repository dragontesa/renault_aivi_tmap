#ifndef WIDGETLARGEVIEWMEDIATOR_H
#define WIDGETLARGEVIEWMEDIATOR_H

#include "mediator/AbstractViewMediator.h"
#include "RouteGuidanceProxy.h"

namespace SKT {

class WidgetLargeViewMediator : public AbstractViewMediator
{
    Q_OBJECT

    Q_ENUMS(EventK)

public:
    enum EventK {
        Mediated = 0,
        SearchClicked,
        OnDestruction,
        RendererOnDestruction
    };

    Q_INVOKABLE WidgetLargeViewMediator( QObject* viewComponent = nullptr );

    ~WidgetLargeViewMediator();

    QList<int> listNotificationInterests();

protected:
    bool onEvent( int event, const QVariant& data );

    bool handleNotification( const mvc::INotificationPtr& note );

private:
    void parseGpsTime(const QVariant& v);
    void updateLanguage();

    QObject* mRenderer = nullptr;
    RouteGuidanceProxyPtr mRgProxy;
};

}

#endif // WIDGETLARGEVIEWMEDIATOR_H
