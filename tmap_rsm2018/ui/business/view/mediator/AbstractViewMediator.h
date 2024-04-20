#ifndef ABSTRACTVIEWMEDIATOR_H
#define ABSTRACTVIEWMEDIATOR_H

#include "mediator/AbstractMediator.h"

namespace SKT {

class AbstractViewMediator : public AbstractMediator
{
    Q_OBJECT
public:
    AbstractViewMediator( QObject* viewComponent = nullptr );

    virtual ~AbstractViewMediator();

protected:
    virtual void onBeforeRegister();
    virtual void onRegister();

    QObject* findChild( const QString& objectName );

    bool updateView( const QVariantMap& data );

protected slots:
    void __onEvent( int event, const QVariant& data );
    void __onDestroy( const QVariant& view );
    void __onDestroyed( QObject* );

    virtual bool onEvent( int event, const QVariant& data );
};

}

#endif // ABSTRACTVIEWMEDIATOR_H
