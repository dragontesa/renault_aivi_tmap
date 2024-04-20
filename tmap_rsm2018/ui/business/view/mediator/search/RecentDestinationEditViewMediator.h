#ifndef RECENTDESTINATIONEDITVIEWMEDIATOR_H
#define RECENTDESTINATIONEDITVIEWMEDIATOR_H

#include "mediator/AbstractViewMediator.h"

namespace SKT {

class RecentDestinationEditViewMediator : public AbstractViewMediator
{
    Q_OBJECT
    Q_ENUMS( EventK )
public:
    enum EventK {
        Activated,
        CloseView,
        CreateView,
        SelectZone1Title,
        SelectZone1TextInput,
        FocusZone1TextInput,
        EditSoftkeyMenu,
		DeleteRecentSearch,
		ClearRecentSearch,
		DeleteRecentDestination,
		ClearRecentDestination,
		ShowToast,
        ModalChanged,
		ResetCsw,
		RegistCsw,
		RemoveCsw,
        HideZone3,
        ShowZone3,
    };

public:
    Q_INVOKABLE RecentDestinationEditViewMediator( QObject* viewComponent = nullptr );
    virtual ~RecentDestinationEditViewMediator();

    QList<int> listNotificationInterests();

protected:
    bool onEvent( int event, const QVariant& data );

    bool handleNotification( const mvc::INotificationPtr& note );

private:
    void onDialogAccepted(const QVariant& v);
    void onDialogCanceled(const QVariant& v);
};
}

#endif // RECENTDESTINATIONEDITVIEWMEDIATOR_H
