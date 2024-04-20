#ifndef NAVIMENUVIEWMEDIATOR_H
#define NAVIMENUVIEWMEDIATOR_H

#include "mediator/AbstractViewMediator.h"

namespace SKT {

class NaviMenuViewMediator : public AbstractViewMediator
{
    Q_OBJECT
    Q_ENUMS( EventK )
public:
    enum EventK {
        Activated,
        Deactivated,
        CreateView,
        CloseView,
        SelectZone1TextInput,
        ActivatedForNavi,
        RequestSetting,
        SaveSetting,
        ViewSizeChanged,
        RequestRoute,
        SelectZone1Title,
        RequestRecentDestination,
        Request1DepthIndustrySearch,
        ModalChanged,
        StartRouteGuidance,
        ResetCsw,
        RegistCsw,
        ShowRegistHomeDialog,
        ShowRegistOfficeDialog,
        RequestViewList,
        RequestLang,
    };
    Q_INVOKABLE NaviMenuViewMediator( QObject* viewComponent = nullptr );

    QList<int> listNotificationInterests();

protected:
    bool onEvent( int event, const QVariant& data );

    bool handleNotification( const mvc::INotificationPtr& note );

private:
    void parseFavorites(const QVariant &data);
    void sendByDestination(const QVariant &);
    void showConfirmDialog();
    bool isVisible() const;
    bool isActive() const;

private:
    QVariant mArrivalData;
    QVariant mRegistHomeAndOfficeData;
};

}

#endif // NAVIMENUVIEWMEDIATOR_H
