#ifndef SEARCHTABTVIEWMEDIATOR_H
#define SEARCHTABTVIEWMEDIATOR_H

#include "mediator/AbstractViewMediator.h"

namespace SKT {

class SearchTabViewMediator : public AbstractViewMediator
{
    Q_OBJECT

    Q_ENUMS( EventK )
public:
    enum EventK {
        Mediated = 0,        
        CreateView,
        CloseView,
        ResetCsw,
        RegistCsw,
        ActiveTabPageChanged,
        RequestLang,
        RequestViewList,
        TabTouched,
    };
    Q_INVOKABLE SearchTabViewMediator( QObject* viewComponent = nullptr );

    QList<int> listNotificationInterests();

protected:
    bool onEvent( int event, const QVariant& data );

private:
    bool handleNotification( const mvc::INotificationPtr& note );

    QMap<int,bool> mTabPageViewActivations;
};

}

#endif // SEARCHTABTVIEWMEDIATOR_H
