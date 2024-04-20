#ifndef WIDGETXLVIEWMEDIATOR_H
#define WIDGETXLVIEWMEDIATOR_H

#include "mediator/AbstractViewMediator.h"
#include "RouteGuidanceProxy.h"

namespace SKT {

class WidgetXlViewMediator : public AbstractViewMediator
{
    Q_OBJECT

    Q_ENUMS(EventK)

public:
    enum EventK {
        Mediated = 0,
        Activated,
        SearchClicked,
        OnDestruction,
        RendererOnDestruction,
        Deactivated
    };

    Q_INVOKABLE WidgetXlViewMediator( QObject* viewComponent = nullptr );

    ~WidgetXlViewMediator();

    QList<int> listNotificationInterests();

protected:
    bool onEvent( int event, const QVariant& data );

    bool handleNotification( const mvc::INotificationPtr& note );

private:
    void parseGpsTime(const QVariant& v);
    void updateLanguage();
    void updateTrafficGuidance(int routeId);

    QObject* mRenderer = nullptr;
    bool mIsGadgetVisible = false;
    bool mIsActive = false;
    bool mIsTrafficInit = false;
    RouteGuidanceProxyPtr mRgProxy;
};

}

#endif // WIDGETXLVIEWMEDIATOR_H
