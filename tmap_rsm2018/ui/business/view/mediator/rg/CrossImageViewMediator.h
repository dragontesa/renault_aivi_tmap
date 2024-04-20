#ifndef CROSSIMAGEVIEW_H
#define CROSSIMAGEVIEW_H

#include "mediator/AbstractViewMediator.h"
#include "RgDataVo.h"
#include "mediator/app/MapViewMode.h"

namespace SKT {

class CrossImageViewMediator : public AbstractViewMediator
{
    Q_OBJECT
    Q_ENUMS( EventK )
public:
    enum EventK {
        Mediated = 0,
        VisibleChanged
    };

    Q_INVOKABLE CrossImageViewMediator( QObject* viewComponent = nullptr );

    ~CrossImageViewMediator();

    QList<int> listNotificationInterests();

protected:
    bool onEvent( int event, const QVariant& data );

    bool handleNotification( const mvc::INotificationPtr& note );

private:
    void applyRgData( const RgDataVo& rgDataVo );

    QObject* mImage;
    QString mNightMode;
    int mTest = -1;
    MapViewMode::ModeK mCurrMapViewMode;
    bool mIsOnline;
    QObject* mRenderer = nullptr;
    QString mLastImagePath;
    bool mLastVisible = false;
    bool mIsVnextEnabled = false;
    bool mIsTbtListExpanded = false;
};

}

#endif // CROSSIMAGEVIEW_H
