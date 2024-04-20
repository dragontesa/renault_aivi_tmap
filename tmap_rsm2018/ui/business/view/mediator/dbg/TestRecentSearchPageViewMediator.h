#ifndef TESTRECENTSEARCHPAGEVIEWMEDIATOR_H
#define TESTRECENTSEARCHPAGEVIEWMEDIATOR_H

#include "mediator/AbstractViewMediator.h"

namespace SKT {

class TestRecentSearchPageViewMediator : public AbstractViewMediator
{
    Q_OBJECT

    Q_ENUMS( EventK )
public:
    enum EventK {
        Activated,
        Deactivated,
        BackwardClicked,
        SelectZone1TextInput,
        FocusZone1TextInput,
        ActivatedForNavi,
        CloseView,
        HideKeyboard,
        CreateView,
        ResetCsw,
        RegistCsw,
        RequestRecentSearch,
        ChangeSearchTabViewPage,
        RecentSearchTabActivated,
        ShowKeyboard,
        SelectSoftkey,
        QueryInDriveMode
    };
    Q_INVOKABLE TestRecentSearchPageViewMediator( QObject* viewComponent = nullptr );

    QList<int> listNotificationInterests();

protected:
    bool onEvent( int event, const QVariant& data );

private:
    void showChangeDestinationConfirmDialog(const QString &address);
    void startRouteGuidance();
    void close();

    bool handleNotification( const mvc::INotificationPtr& note );

    QVariantMap mStartRouteGuidanceMap;
    bool mIsActivate;
};

}

#endif // TESTRECENTSEARCHPAGEVIEWMEDIATOR_H
