#ifndef ZONE3VIEWMEDIATOR_H
#define ZONE3VIEWMEDIATOR_H

#include "mediator/AbstractViewMediator.h"

namespace SKT {

class Zone3ViewMediator : public AbstractViewMediator
{
    Q_OBJECT
    Q_ENUMS( EventK )
public:
    enum EventK {
        Mediated,
        BackClicked,
        NaviClicked,
        MenuClicked,
        Menu2Clicked,
        OptionClicked,
        Option2Clicked,
        FaderClicked,
        MenuCleared,
        MenuCreated,
        RemoveSoftkeySection,
        CreateView,
        OpenOptionMenuView,
        OpenOptionMenuView2,
        OptionMenuClicked,
        OptionMenu2Clicked,
    };

    Q_INVOKABLE Zone3ViewMediator( QObject* viewComponent = nullptr );

    QList<int> listNotificationInterests();

protected:
    bool onEvent( int event, const QVariant& data );
    bool handleNotification( const mvc::INotificationPtr& note );

private slots:
    void updateSoftkeyRouteGuideUsingCache();

private:
    void updateSoftkeyRouteGuide(const QVariantMap &m);

    QString currentViewName(const QVariant &data);
    QString menuName(const QVariant &data);
    void blockSoftkey(bool block);

    void parseGpsTime(const QVariant& v);

    void showZone3(const QVariant& v);
    void hideZone3(const QVariant& v);

    QString mPoi;
    bool mBlockSoftkey;
    bool mShowKeyboard;
    QVariantMap mRgDataCache;
    QTimer mRgDataCacheTimer;
};

}//SKT

#endif // ZONE3VIEWMEDIATOR_H
