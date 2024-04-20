#ifndef SPEEDMETERVIEW_H
#define SPEEDMETERVIEW_H

#include "mediator/AbstractViewMediator.h"
#include "RgDataVo.h"
#include "mediator/app/MapViewMode.h"

namespace SKT {

class SpeedMeterViewMediator : public AbstractViewMediator
{
    Q_OBJECT
    Q_ENUMS( EventK )
public:
    enum EventK {
        Mediated = 0
    };

    Q_INVOKABLE SpeedMeterViewMediator( QObject* viewComponent = nullptr );

    ~SpeedMeterViewMediator();

    QList<int> listNotificationInterests();

protected:
    bool onEvent( int event, const QVariant& data );

    bool handleNotification( const mvc::INotificationPtr& note );

private:
    void applyRgData( const RgDataVo& rgDataVo );
    void updateNumbers( const QString& speed );

    MapViewMode::ModeK mCurrMapViewMode;

    struct NotifiableUpdate {
        QString speed;
    };

    NotifiableUpdate mUpdate;
    bool mAlert = false;
    QString mLastSpeed;
};

}

#endif // SPEEDMETERVIEW_H
