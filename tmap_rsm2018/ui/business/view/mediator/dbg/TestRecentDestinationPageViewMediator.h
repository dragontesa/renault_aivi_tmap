#ifndef TESTRECENTDESTINATIONPAGEVIEWMEDIATOR_H
#define TESTRECENTDESTINATIONPAGEVIEWMEDIATOR_H

#include "mediator/AbstractViewMediator.h"

namespace SKT {

class TestRecentDestinationPageViewMediator : public AbstractViewMediator
{
    Q_OBJECT

    Q_ENUMS( EventK )
public:
    enum EventK {
        Activated,
        Deactivated,
        CreateView,        
        ResetCsw,
        RegistCsw,
        RequestRecentDestination,
        BackwardClicked,
        SelectZone1TextInput,
        ActivatedForNavi,
        CloseView,
        ItemClicked,
        QueryInDriveMode,
        ChangeSearchTabViewPage,
        HideKeyboard,
        ModalChanged,
        SelectSoftkey
    };
    Q_INVOKABLE TestRecentDestinationPageViewMediator( QObject* viewComponent = nullptr );

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
};

}

#endif // TESTRECENTDESTINATIONPAGEVIEWMEDIATOR_H
