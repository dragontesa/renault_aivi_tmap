#ifndef ROUTESETTINGVIEWMEDIATOR_H
#define ROUTESETTINGVIEWMEDIATOR_H

#include "mediator/AbstractViewMediator.h"
#include <qglobal.h>

namespace SKT {

class RouteSettingViewMediator : public AbstractViewMediator
{
    Q_OBJECT
    Q_ENUMS(EventK)

public:
    enum EventK {
        Mediated = 0,
        Activated,
        Deactivated,
        PoiClicked,
        PoiDeleteClicked,
        VisibleChanged,
        ViewSizeChanged,
        ItemMoveDown,
        ItemMoveUp,
        Sync,
        RegistCSW,
        RemoveCSW,
        SelectCSW,
        SetupTitleAndZoneMenu
    };
    enum MenuK {
        MenuRoutePlanning = 0
    };

    Q_INVOKABLE RouteSettingViewMediator(QObject *viewComponent = nullptr);
    virtual ~RouteSettingViewMediator();

    QList<int> listNotificationInterests() Q_DECL_OVERRIDE;

protected:
    bool onEvent(int event, const QVariant& data) Q_DECL_OVERRIDE;
    bool handleNotification(const mvc::INotificationPtr& note) Q_DECL_OVERRIDE;

private:
    void addViaFail(const QVariant& data);
    void changeLanguage(const QVariant& data);
    void processSearch(const QVariant &data);
    void processSearchFail(const QVariant& data);
    void procSoftkey(const QVariant &data);
    void close();
    void sync(const QVariant &data);
    void updateIsRg(const QVariantMap& data);
    QString caller() const;
    QString getTitleText(int index) const;
    QString getViaMenuText(int index) const;
    QRect mRect;
    bool mIsRg = false;
    QVariant mSearchMap;
    QString mViaMenuText;
    bool mIsRouteCandidateViewExist = false;
    bool mCloseCalled = false;
};

} // SKT

#endif // ROUTESETTINGVIEWMEDIATOR_H
