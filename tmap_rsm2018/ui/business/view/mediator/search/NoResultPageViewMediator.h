#ifndef NORESULTPAGEVIEWMEDIATOR_H
#define NORESULTPAGEVIEWMEDIATOR_H

#include "mediator/AbstractViewMediator.h"

namespace SKT {

class NoResultPageViewMediator : public AbstractViewMediator
{
    Q_OBJECT

    Q_ENUMS( EventK )
public:
    enum EventK {
        Mediated,
        Activated,
        Deactivated,
        CreateView,
        CloseView,
        SelectZone1TextInput,
        FocusZone1TextInput,
        ShowKeyboard,
        HideKeyboard,
        UpdateSearchResultCount,
        RequestLang,
        UpdateView,
        ChangeSearchTabViewPage,
    };
    Q_INVOKABLE NoResultPageViewMediator( QObject* viewComponent = nullptr );

    QList<int> listNotificationInterests();

protected:
    bool onEvent( int event, const QVariant& data );

private:
    bool handleNotification( const mvc::INotificationPtr& note );

};

}

#endif // NORESULTPAGEVIEWMEDIATOR_H
