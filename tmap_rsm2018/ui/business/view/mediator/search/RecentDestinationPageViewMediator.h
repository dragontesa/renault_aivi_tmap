#ifndef RECENTDESTINATIONPAGEVIEWMEDIATOR_H
#define RECENTDESTINATIONPAGEVIEWMEDIATOR_H

#include "mediator/AbstractViewMediator.h"

namespace SKT {

class RecentDestinationPageViewMediator : public AbstractViewMediator
{
    Q_OBJECT

    Q_ENUMS( EventK )
public:
    enum EventK {
        Mediated,
        Activated,
        Deactivated,
        CreateView,
        FocusCsw,
        ResetCsw,
        RegistCsw,
        RequestRecentDestination,
        BackwardClicked,
        SelectZone1TextInput,
        SelectZone1Title,
        ActivatedForNavi,
        CloseView,
        ItemClicked,
        ItemPressed,
        QueryInDriveMode,
        ChangeSearchTabViewPage,
        HideKeyboard,
        ModalChanged,
        SelectSoftkeyIndexMenuMode,
        HideTabViewContent,
        HideView,
        ShowZone3,
        HideZone3,
        UpdateView,
        ListScrolled,
    };
    Q_INVOKABLE RecentDestinationPageViewMediator( QObject* viewComponent = nullptr );

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
    bool mIsEditMode;
    int mEditType; // page
    bool mIsGadgetSearchMode = false;
    bool mIsRg = false;
};

}

#endif // RECENTDESTINATIONPAGEVIEWMEDIATOR_H
