#ifndef ROUTEREGISTVIEWMEDIATOR_H
#define ROUTEREGISTVIEWMEDIATOR_H

#include "mediator/AbstractViewMediator.h"
#include <qglobal.h>

namespace SKT {

class RouteRegistViewMediator : public AbstractViewMediator
{
    Q_OBJECT
    Q_ENUMS(EventK)

public:
    enum EventK {
        Mediated = 0,
        CancelClicked,
        SearchItemClicked,
        ModalChanged,
        RegistCSW,
        CaptureScreen
    };

    Q_INVOKABLE RouteRegistViewMediator(QObject *viewComponent = nullptr);
    virtual ~RouteRegistViewMediator();

    QList<int> listNotificationInterests() Q_DECL_OVERRIDE;

protected:
    bool onEvent(int event, const QVariant &data) Q_DECL_OVERRIDE;
    bool handleNotification(const mvc::INotificationPtr &note) Q_DECL_OVERRIDE;

private:
    void closeView();
    void onViaItemClicked(const QVariant &);
    void hideRouteSettingView();

    void updateModal(const QVariant& v);
    void checkStartCode(int type);
    bool isStartPos() const;
};

} //SKT

#endif // ROUTEREGISTVIEWMEDIATOR_H
