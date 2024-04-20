#ifndef INDUSTRYSEARCHVIEWMEDIATOR_H
#define INDUSTRYSEARCHVIEWMEDIATOR_H

#include "mediator/AbstractViewMediator.h"

namespace SKT {

class IndustrySearchViewMediator : public AbstractViewMediator
{
    Q_OBJECT
    Q_ENUMS( EventK )
    Q_ENUMS( ResultK )
    Q_ENUMS( DepthK )
public:
    enum EventK {
        Mediated,
        Activated,
        Deactivated,
        CreateView,
        SelectZone1Title,
        RequestSearch,
        CloseView,
        ReturnToFavorite,
        ReturnToViaSetting,
        ResetCsw,
        RegistCsw,
        ReturnToNaviMenu,
        RequestAddressType,
        ViewSizeChanged,
        ShowProgressIndicator,
        HideProgressIndicator,
        RemoveSearchQueue,
        RequestViewList,
        RequestLang,
        UpdateView,
        RequestSetting,
        DepthChanged,
    };
    enum ResultK {
        ResultName,
        ResultCode,
        ResultCount,
    };
    enum DepthK {
        DepthNone,
        DepthFirst,
        DepthSecond,
        DepthThird,
    };

    Q_INVOKABLE IndustrySearchViewMediator( QObject* viewComponent = nullptr );
    virtual ~IndustrySearchViewMediator();

    QList<int> listNotificationInterests();

protected:
    bool onEvent( int event, const QVariant& data );
    bool handleNotification( const mvc::INotificationPtr& note );

private:
    bool isVisible() const;
    bool isActive() const;
};

}

#endif // INDUSTRYSEARCHVIEWMEDIATOR_H
