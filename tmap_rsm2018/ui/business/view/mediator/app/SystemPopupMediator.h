#ifndef SYSTEMPOPUPMEDIATOR_H
#define SYSTEMPOPUPMEDIATOR_H

#include "mediator/AbstractMediator.h"
#include <QVariantMap>
#include <QTimer>

namespace SKT
{

class SystemPopupMediator : public AbstractMediator
{
    Q_OBJECT

public:
    SystemPopupMediator();

    enum SystemPopupType {
        MapUpdate = 0,
        ContinueRg,
        Send2Car,
    };

    virtual QList<int> listNotificationInterests() Q_DECL_OVERRIDE;
    virtual bool handleNotification(const mvc::INotificationPtr &) Q_DECL_OVERRIDE;

private:
    void accepted(const QVariant &v);
    void canceled(const QVariant &v);
    void closeCommonDialog();
    void createContinueRgView(const QVariantMap &m);
    void createSystemView(const QVariant &v);
    void createMapUpdateView(const QVariantMap &m);
    void createSend2CarView(const QVariantMap &m);
    void create1stView();
    void delayedCreate1stView();
    bool isMapUpdate() const;

private:
    QMap<int, QVariantMap> mPopupsMap;
    QTimer mTimer;
    int mCreateMessageType = -1;
    bool mIsCommonDialogViewExist = false;
};

}

#endif // SYSTEMPOPUPMEDIATOR_H
