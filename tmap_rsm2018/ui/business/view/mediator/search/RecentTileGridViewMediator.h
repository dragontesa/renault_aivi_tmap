#ifndef RECENTTILEGRIDVIEWMEDIATOR_H
#define RECENTTILEGRIDVIEWMEDIATOR_H

#include "mediator/AbstractViewMediator.h"

namespace SKT {

class RecentTileGridViewMediator : public AbstractViewMediator
{
    Q_OBJECT
    Q_ENUMS( EventK )
public:
    enum EventK {
        ActiveFocusChanged = 0,
        Activated,
        Deactivated,
        ModalChanged,
		RequestRecentDestination,
		RequestRoute,
		RequestRouteSimpleInfo,
        StartRouteGuidance,
        Mediated
    };
    Q_INVOKABLE RecentTileGridViewMediator( QObject* viewComponent = nullptr );

    ~RecentTileGridViewMediator();

    QList<int> listNotificationInterests();

protected:
    void onRegister();

    bool onEvent( int event, const QVariant& data );

    bool handleNotification( const mvc::INotificationPtr& note );

private:
    void sendByDestination(const QVariant &data);
    void showConfirmDialog(const QString &address);

private:
    bool mIsActivated;
    bool mIsVisibleChangeDestinationDialog;
    QVariant mDestData;
};

}

#endif // RECENTTILEGRIDVIEWMEDIATOR_H
