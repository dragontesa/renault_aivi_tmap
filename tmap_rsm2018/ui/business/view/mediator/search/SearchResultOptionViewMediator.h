#ifndef SEARCHRESULTOPTIONVIEWMEDIATOR_H
#define SEARCHRESULTOPTIONVIEWMEDIATOR_H

#include "mediator/AbstractViewMediator.h"
#include "VSM_Enum.h"

namespace SKT {

class SearchResultOptionViewMediator : public AbstractViewMediator
{
    Q_OBJECT
    Q_ENUMS( EventK )
public:
    enum EventK {
        Activated,
        DeActivated,
        CreateView,
        CloseView,
        ModalChanged,
        RequestLang,
        ResetCsw,
        RegistCsw,
    };



    Q_INVOKABLE SearchResultOptionViewMediator( QObject* viewComponent = nullptr );

    QList<int> listNotificationInterests();

protected:
    bool onEvent( int event, const QVariant& data );

    bool handleNotification( const mvc::INotificationPtr& note );

private:
    QVariantMap mStartRouteGuidanceMap;
};
}

#endif // SEARCHRESULTOPTIONVIEWMEDIATOR_H

