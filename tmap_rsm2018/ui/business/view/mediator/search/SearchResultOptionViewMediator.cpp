#include "SearchResultOptionViewMediator.h"

#include "SettingDto.h"
#include "Constants.h"
#include "RgConstants.h"
#include "RouteConstants.h"
#include "ZoneConstants.h"
#include "SettingConstants.h"
#include "ViewRegistry.h"
#include <QJSValueIterator>
#include "NavigationControllerConstants.h"
#include "LangConstants.h"
#include "NavigationControllerProtocol.h"


using namespace SKT;

SearchResultOptionViewMediator::SearchResultOptionViewMediator( QObject* viewComponent )
    : AbstractViewMediator( viewComponent )
{

}

QList<int> SearchResultOptionViewMediator::listNotificationInterests()
{
    static QList<int> notes = {
        NavigationControllerConstants::ChangeLanguage,
        NavigationControllerConstants::ResponseController,
    };
    return notes;
}

bool SearchResultOptionViewMediator::onEvent( int event, const QVariant& data )
{
    switch ( event ) {
    case Activated:
        break;

    case DeActivated:
        break;

    case CreateView:
        sendNotification(Constants::CreateView, data);
        break;
    case CloseView:        
        sendNotification(Constants::CloseView, data);
        break;  

    case ModalChanged:
        sendNotification(ZoneConstants::UpdateModal,data.toMap());
        break;

    case RequestLang: {
             QVariantMap body;
             body.insert(QStringLiteral("type"), NavigationControllerProtocol::GetLang );
             sendNotification( NavigationControllerConstants::RequestController, body );
         }
         break;
    case ResetCsw:
        sendNotification(NavigationControllerConstants::ResetCSW);
        break;
    case RegistCsw:
        sendNotification(NavigationControllerConstants::RegistCSW, data);
        break;

    default:
        return false;
    }
    return true;
}

bool SearchResultOptionViewMediator::handleNotification( const mvc::INotificationPtr& note )
{
    switch ( note->getId() ) {
    case NavigationControllerConstants::ChangeLanguage: {
            auto data = note->getBody().value<QVariantMap>();
            auto lang = data[LangConstants::Lang].toString();
            QMetaObject::invokeMethod(mViewComponent, "__retranslateUi", Q_ARG(QVariant, QVariant::fromValue(lang)));
        }
        break;

    case NavigationControllerConstants::ResponseController: {
            QVariantMap data = note->getBody().value<QVariantMap>();
            if (data.contains(CommonConstants::Lang)) {
                QString value = data[CommonConstants::Lang].toString();
                QQmlProperty::write( mViewComponent, "__lang", QVariant::fromValue(value) );
            }
        }
        break;

    default:
        return false;
    }
    return true;
}

