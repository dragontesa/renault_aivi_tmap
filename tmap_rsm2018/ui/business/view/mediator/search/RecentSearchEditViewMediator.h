#ifndef RECENTSEARCHEDITVIEWMEDIATOR_H
#define RECENTSEARCHEDITVIEWMEDIATOR_H

#include "mediator/AbstractViewMediator.h"

namespace SKT {

class RecentSearchEditViewMediator : public AbstractViewMediator
{
    Q_OBJECT
    Q_ENUMS( EventK )
public:
    enum EventK {
        Activated,
        CreateView,
        CloseView,
        SelectZone1Title,
        SelectZone1TextInput,
        FocusZone1TextInput,
        EditSoftkeyMenu,
		DeleteRecentSearch,
		ClearRecentSearch,
		ShowToast,
        ModalChanged,
		ResetCsw,
        RegistCsw,
        RemoveCsw,
        HideZone3,
        ShowZone3,
    };
    Q_INVOKABLE RecentSearchEditViewMediator( QObject* viewComponent = nullptr );

    QList<int> listNotificationInterests();

protected:
    bool onEvent( int event, const QVariant& data );

    bool handleNotification( const mvc::INotificationPtr& note );

private:
    void onDialogAccepted(const QVariant& v);
    void onDialogCanceled(const QVariant& v);

private:
    QString mDeleteAllMessage;
};

}

#endif // RECENTSEARCHEDITVIEWMEDIATOR_H
