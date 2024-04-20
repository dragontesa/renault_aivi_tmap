#ifndef ZONEAPITESTVIEW_H
#define ZONEAPITESTVIEW_H

#include "../AbstractViewMediator.h"

namespace SKT
{

class ZoneApiTestViewMediator : public AbstractViewMediator
{
    Q_OBJECT

    Q_ENUMS(EventK)
public:
    enum EventK {
        Mediated,
        Activated,
        Call,
        CallKeyboard
    };
    Q_INVOKABLE ZoneApiTestViewMediator(QObject* viewComponent = nullptr);

    QList<int> listNotificationInterests();

protected:
    bool onEvent( int event, const QVariant& data );
    bool handleNotification( const mvc::INotificationPtr& note );

private:
    void call(const QVariant& v);
    void callKeyboard(const QVariant& v);

private:
    bool mIsShowKeyboard = false;
};

}

#endif // ZONEAPITESTVIEW_H
