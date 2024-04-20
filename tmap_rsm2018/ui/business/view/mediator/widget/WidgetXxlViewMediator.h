#ifndef WIDGETXXLVIEWMEDIATOR_H
#define WIDGETXXLVIEWMEDIATOR_H

#include "mediator/AbstractViewMediator.h"
#include "RouteGuidanceProxy.h"

namespace SKT {

class WidgetXxlViewMediator : public AbstractViewMediator
{
    Q_OBJECT

    Q_ENUMS(EventK)

public:
    enum EventK {
        Mediated = 0,
        SearchClicked,
        OnDestruction,
        RendererOnDestruction,
        Activated,
        Deactivated
    };

    Q_INVOKABLE WidgetXxlViewMediator( QObject* viewComponent = nullptr );

    ~WidgetXxlViewMediator();

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

#endif // WIDGETXXLVIEWMEDIATOR_H
