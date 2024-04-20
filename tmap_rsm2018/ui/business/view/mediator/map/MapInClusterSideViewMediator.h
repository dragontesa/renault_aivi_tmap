#ifndef MAPINCLUSTERSIDEVIEWMEDIATOR_H
#define MAPINCLUSTERSIDEVIEWMEDIATOR_H

#include "mediator/AbstractViewMediator.h"
#include "RgDataVo.h"
#include "mediator/app/MapViewMode.h"
#include "PreferenceVo.h"

namespace SKT {

class MapInClusterSideViewMediator : public AbstractViewMediator
{
    Q_OBJECT
    Q_ENUMS( EventK )
public:
    enum EventK {
        Mediated = 100,
        Activated,
        RendererOnDestruction
    };

    Q_INVOKABLE MapInClusterSideViewMediator( QObject* viewComponent = nullptr );

    ~MapInClusterSideViewMediator();

    QList<int> listNotificationInterests();

protected:
    bool onEvent( int event, const QVariant& data );
    bool handleNotification( const mvc::INotificationPtr& note );

private:
    QObject* mRenderer = nullptr;
};

}

#endif // MAPINCLUSTERSIDEVIEWMEDIATOR_H
