#ifndef REGIONSEARCHVIEW_H
#define REGIONSEARCHVIEW_H

#include "mediator/AbstractViewMediator.h"

namespace SKT {

class RegionSearchViewMediator : public AbstractViewMediator
{
    Q_OBJECT
    Q_ENUMS( EventK )
public:
    enum EventK {
        Mediated,
        Activated,
        CreateView,
        CloseView,
        UpdateView,
        SelectZone1Title,
        RequestSearch,
        ResetCsw,
        RegistCsw,
        RequestAddressType,
        RequestLang,
    };

    Q_INVOKABLE RegionSearchViewMediator( QObject* viewComponent = nullptr );

    QList<int> listNotificationInterests();

protected:
    bool onEvent( int event, const QVariant& data );

    bool handleNotification( const mvc::INotificationPtr& note );
};

}

#endif // REGIONSEARCHVIEW_H
