#ifndef TESTGUIDVIEWMEDIATOR_H
#define TESTGUIDVIEWMEDIATOR_H

#include "mediator/AbstractViewMediator.h"

namespace SKT
{

class TestGuidViewMediator : public AbstractViewMediator
{
    Q_OBJECT
    Q_ENUMS( EventK )

public:
    enum EventK {
        Mediated,
        Activated,
        Deactivated
    };

    Q_INVOKABLE TestGuidViewMediator(QObject* viewComponent = nullptr);

    QList<int> listNotificationInterests() Q_DECL_OVERRIDE;

protected:
    bool onEvent(int event, const QVariant& data) Q_DECL_OVERRIDE;

private:
    bool handleNotification(const mvc::INotificationPtr& note) Q_DECL_OVERRIDE;
};

}

#endif // TESTGUIDVIEWMEDIATOR_H
