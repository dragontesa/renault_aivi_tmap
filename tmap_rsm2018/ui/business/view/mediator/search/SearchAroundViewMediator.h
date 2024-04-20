#ifndef SEARCHAROUNDVIEWMEDIATOR_H
#define SEARCHAROUNDVIEWMEDIATOR_H

#include "mediator/AbstractViewMediator.h"

namespace SKT {

class SearchAroundViewMediator : public AbstractViewMediator
{
    Q_OBJECT
    Q_ENUMS( EventK )
public:
    enum EventK {
        Mediated,
        Activated,
        Deactivated,
        CreateView,
        RequestSetting,
        RequestSearch,
        SelectZone1Title,
        ModalChanged,
        CloseView,
        ReturnToFavorite,
        ReturnToViaSetting,
        ReturnToNaviMenu,
        ViewSizeChanged,
        ResetCsw,
        RegistCsw,
        RequestAddressType,
        RequestViewList,
        RequestLang,
        UpdateView,
    };


    Q_INVOKABLE SearchAroundViewMediator( QObject* viewComponent = nullptr );

    QList<int> listNotificationInterests();

protected:
    bool onEvent( int event, const QVariant& data );
    bool handleNotification( const mvc::INotificationPtr& note );

private:
    bool isVisible() const;
    bool isActive() const;
};

}

#endif // SEARCHAROUNDVIEWMEDIATOR_H
