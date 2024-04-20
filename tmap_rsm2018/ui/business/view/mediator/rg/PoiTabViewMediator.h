#ifndef POITABVIEWMEDIATOR_H
#define POITABVIEWMEDIATOR_H

#include "../AbstractViewMediator.h"

namespace SKT
{

class PoiTabViewMediator : public AbstractViewMediator
{
    Q_OBJECT
    Q_ENUMS(EventK)

public:
    enum EventK {
        Mediated,
        Activated,
        AsDestinationClicked,
        AsViaClicked,
        AsBookmarkClicked,
        CancelButtonClicked,
    };

    Q_INVOKABLE PoiTabViewMediator(QObject* viewComponent = nullptr);
    virtual ~PoiTabViewMediator();

    QList<int> listNotificationInterests();

protected:
    bool onEvent( int event, const QVariant& data );
    bool handleNotification( const mvc::INotificationPtr& noti );

private:
    bool parseIsRg(const QVariant& v);
    void asVia(const QVariant& v);
    void asDestination(const QVariant& v);
    void asBookmark(const QVariant& v);
    void close(bool cancel=false);
    void showChangeDestinationDialog(const QVariant& v);

private:
    bool mIsRg;
    QVariantMap mBookmarkMap;
};

}//SKT

#endif // POITABVIEWMEDIATOR_H
