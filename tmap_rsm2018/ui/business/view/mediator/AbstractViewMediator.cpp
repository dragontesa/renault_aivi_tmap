#include "mediator/AbstractViewMediator.h"
#include "Constants.h"
#include "CommonConstants.h"

using namespace SKT;

AbstractViewMediator::AbstractViewMediator( QObject* viewComponent )
    : AbstractMediator( viewComponent )
{
}

AbstractViewMediator::~AbstractViewMediator()
{
}

void AbstractViewMediator::onBeforeRegister()
{
    AbstractMediator::onBeforeRegister();

    auto regId = QQmlProperty::read(mViewComponent, QStringLiteral("regId"));
    setMediatorId( regId );
}

void AbstractViewMediator::onRegister()
{    
    QObject::connect(mViewComponent, SIGNAL(onEvent(int,QVariant)),
        this, SLOT(__onEvent(int, const QVariant&)));

    QObject::connect(mViewComponent, SIGNAL(onDestroy(QVariant)),
        this, SLOT(__onDestroy(const QVariant&)));

    QObject::connect(mViewComponent, SIGNAL(destroyed(QObject*)),
        this, SLOT(__onDestroyed(QObject*)));
}

QObject* AbstractViewMediator::findChild( const QString& objectName )
{
    if ( mViewComponent == nullptr ) {
        T_DEBUG( QString( "Detected an attempt to access uninitialized view component object '%1'" ).arg( objectName ) );
    }

    auto obj = mViewComponent->findChild<QObject*>( objectName );
    if ( obj == nullptr ) {
        T_DEBUG( QString( "Could not find object with name '%1'" ).arg( objectName ) );
    }

    return obj;
}

bool AbstractViewMediator::updateView( const QVariantMap& data )
{
    auto objectName = QQmlProperty::read(mViewComponent, CommonConstants::ObjectName).toString();

    if ( mViewComponent == nullptr ) {
        T_DEBUG( QString( "Detected an attempt to access uninitialized view component object '%1'" ).arg( objectName ) );
        return false;
    }

    if (data.contains(CommonConstants::ObjectName)) {
        auto objName = data[CommonConstants::ObjectName].toString();
        if (objName != objectName)
            return false;
    }

    for ( auto it = data.constBegin(); it != data.constEnd(); it++ ){
        if (CommonConstants::Url == it.key() || CommonConstants::ObjectName == it.key())
            continue;
       QQmlProperty::write( mViewComponent, it.key(), QVariant::fromValue( it.value() ) );
    }

    return true;
}

void AbstractViewMediator::__onEvent( int event, const QVariant& data )
{
    bool isProcessed = onEvent( event, data );

#ifdef TMAP_RSM_UI_ENABLE_MVC_LOG
    T_DEBUG( QJsonObject({
        {"mediator",getMediatorName()},
        {"type","Event"},
        {"id",Constants::enumToString( event )},
        {"isProcessed",isProcessed}
    }) );
#endif
}

void AbstractViewMediator::__onDestroy( const QVariant& view )
{
    mViewComponent->deleteLater();
}

void AbstractViewMediator::__onDestroyed( QObject* )
{
    mViewComponent = nullptr;
}

bool AbstractViewMediator::onEvent( int event, const QVariant& data )
{
    return true;
}
