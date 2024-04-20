#include "MediateViewCommand.h"
#include "Facade.h"
#include "Constants.h"
#include "CommonConstants.h"

//#define TMAP_RSM_UI_ENABLE_MVC_LOG

using namespace SKT;

MediateViewCommand::MediateViewCommand( QObject* parent )
    : SimpleCommand( parent )
{

}

bool MediateViewCommand::execute( const mvc::INotificationPtr& note )
{
    switch ( note->getId() ) {
    case Constants::MediateView:
        {
            QString source;
            QObject* view = nullptr;
            QObject* parentView = nullptr;
            auto body = note->getBody();
            auto jsVal = body.value<QJSValue>();
            if ( jsVal.isNull() ) {
                source = jsVal.property( CommonConstants::Url ).toString();
                view = jsVal.property( QStringLiteral("view") ).toQObject();
                parentView = jsVal.property( QStringLiteral("parentView") ).toQObject();
            }
            else {
                auto args = body.value<QVariantMap>();
                source = args[CommonConstants::Url].toString();
                view = args[QStringLiteral("view")].value<QObject*>();
                parentView = args[QStringLiteral("parentView")].value<QObject*>();
            }

            QFileInfo fi( source );

            QString mediatorClassName;
            mediatorClassName.append( QStringLiteral("SKT::") );
            mediatorClassName.append( fi.baseName() );
            mediatorClassName.append( QStringLiteral("Mediator") );
            mediatorClassName.append( QStringLiteral("*") );

            auto type = QMetaType::type( mediatorClassName.T_CSTR() );
            if ( type == 0 ) {
                T_DEBUG( QJsonObject({
                    {"type","Mediate"},
                    {"task","Mediate"},
                    {"state","Error"},
                    {"error","Could not find mediator"},
                    {"to",mediatorClassName},
                    {"to",fi.fileName()}
                }) );
            }
            else {
                QString mediatorName;
                mediatorName.append( QStringLiteral("SKT::") );
                mediatorName.append( fi.baseName() );
                mediatorName.append( QStringLiteral("Mediator") );

                //Facade::instance()->removeMediator( mediatorName );

                auto mo = QMetaType::metaObjectForType( type );
                auto obj = mo->newInstance( Q_ARG(QObject*,view) );
                auto instance = dynamic_cast<mvc::IMediator*>(obj);
                mvc::IMediatorPtr mediatorPtr( instance );
                Facade::instance()->registerMediator( mediatorPtr );
                QMetaObject::invokeMethod( view, "viewMediated" );

#ifdef TMAP_RSM_UI_ENABLE_MVC_LOG
                T_DEBUG( QJsonObject({
                    {"type","Mediate"},
                    {"task","Mediate"},
                    {"state","Done"},
                    {"to",mediatorClassName},
                    {"to",fi.fileName()}
                }) );
#endif
            }
        }
        break;
    case Constants::RemoveView:
        {
            auto body = note->getBody();
            auto jsVal = body.value<QJSValue>();
            auto source = jsVal.property( CommonConstants::Url).toString();
            auto view = jsVal.property( QStringLiteral("view") ).toQObject();

            QFileInfo fi( source );

            QString mediatorName;
            mediatorName.append( QStringLiteral("SKT::") );
            mediatorName.append( fi.baseName() );
            mediatorName.append( QStringLiteral("Mediator") );

#ifdef TMAP_RSM_UI_ENABLE_MVC_LOG
            T_DEBUG( QJsonObject({
                {"type","Mediate"},
                {"task","RemoveView"},
                {"state","Done"},
                {"mediator",mediatorName}
            }) );
#endif

            Facade::instance()->removeMediator( mediatorName );
        }
        break;
    default:
        return false;
    }

    return true;
}
