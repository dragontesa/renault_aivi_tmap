#ifndef ADDRESSMEDIATOR_H
#define ADDRESSMEDIATOR_H

#include "mediator/AbstractMediator.h"
#include <QList>
#include <QPoint>
#include <QVariantMap>

namespace SKT
{

class AddressMediator : public AbstractMediator
{
    Q_OBJECT

public:
    AddressMediator(QObject *addressManager);
    virtual ~AddressMediator();

    virtual QList<int> listNotificationInterests() Q_DECL_OVERRIDE;
    virtual bool handleNotification(const mvc::INotificationPtr &note) Q_DECL_OVERRIDE;

    struct Item {
        QPoint pt;
        QVariantMap req;
    };

private:
    void request(const QVariantMap &req);
    void replyToFavorite(int x, int y, const QString& name, const QString &address, int destType=0);
    void onAddItemOk(const QVariantMap& m);
    void onAddItemFailure(const QVariantMap& m);

    void removeRequestedItem(const QPoint& pt);
    Item requestedItem(const QPoint& pt) const;
    QString getAddressFromRequestedCommand(int x, int y) const;

private slots:
    void reply(int x, int y, const QString &address);

private:
    QObject *mAddressManager;
    QList<Item> mRequested;
};

}//SKT

#endif // ADDRESSMEDIATOR_H
