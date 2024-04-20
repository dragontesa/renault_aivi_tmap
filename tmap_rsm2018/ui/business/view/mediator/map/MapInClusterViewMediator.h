#ifndef MAPINCLUSTERVIEWMEDIATOR_H
#define MAPINCLUSTERVIEWMEDIATOR_H

#include "mediator/AbstractViewMediator.h"
#include "RgDataVo.h"
#include "mediator/app/MapViewMode.h"
#include "PreferenceVo.h"

namespace SKT {

class MapInClusterViewMediator : public AbstractViewMediator
{
    Q_OBJECT
    Q_ENUMS( EventK )
public:
    enum EventK {
        Mediated = 100,
        Activated,
        RendererOnDestruction,
        RendererCreated
    };

    Q_INVOKABLE MapInClusterViewMediator( QObject* viewComponent = nullptr );

    ~MapInClusterViewMediator();

    QList<int> listNotificationInterests();

protected:
    bool onEvent( int event, const QVariant& data );
    bool handleNotification( const mvc::INotificationPtr& note );

private:

    QObject* mRenderer = nullptr;
};

}

#endif // MAPINCLUSTERVIEWMEDIATOR_H
