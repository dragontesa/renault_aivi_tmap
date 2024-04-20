#ifndef TESTFONTVIEWMEDIATOR_H
#define TESTFONTVIEWMEDIATOR_H

#include "../view/mediator/AbstractViewMediator.h"

namespace SKT
{

class TestFontViewMediator : public AbstractViewMediator
{
    Q_OBJECT

public:
    Q_INVOKABLE TestFontViewMediator(QObject* parent = nullptr);

    QList<int> listNotificationInterests();

protected:
    bool onEvent( int event, const QVariant& data );

    bool handleNotification( const mvc::INotificationPtr& note );
};

}

#endif // TESTFONTVIEWMEDIATOR_H
