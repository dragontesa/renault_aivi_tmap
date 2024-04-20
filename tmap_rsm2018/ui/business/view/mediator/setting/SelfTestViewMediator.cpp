#include "SelfTestViewMediator.h"
#include "SettingDto.h"
#include "Constants.h"
#include "ZoneConstants.h"
#include "SearchConstants.h"
#include "SettingConstants.h"
#include "KeyboardConstants.h"
#include "NavigationControllerProtocol.h"
#include <QJSValueIterator>
#include "NavigationControllerConstants.h"
#include "QmlAppHelper.h"

#include "RgConstants.h"
#include "SimulatorConstants.h"
#include "RouteConstants.h"

#include "PreferenceConstants.h"
#include "PreferenceVo.h"

#include "UtilConstants.h"
#include "ViewRegistry.h"


using namespace SKT;

SelfTestViewMediator::SelfTestViewMediator( QObject* viewComponent )
    : AbstractViewMediator( viewComponent )
{
}

QList<int> SelfTestViewMediator::listNotificationInterests()
{
    static QList<int> notes = {
        SettingConstants::ApplySetting,
        ZoneConstants::DoSoftKeyFunction,
        KeyboardConstants::ProcessKeyboard,
        NavigationControllerConstants::ResponseController,
        PreferenceConstants::ApplyPreference,
        PreferenceConstants::ChangedPreference,
        SearchConstants::SearchWait,
//        KeyboardConstants::PullDownKeyboard,
//        KeyboardConstants::PullUpKeyboard,
    };

    return notes;
}

bool SelfTestViewMediator::onEvent( int event, const QVariant& data )
{
    T_DEBUGV( "event: %d", event );

    QJSValue jsVal = data.value<QJSValue>();
    int x = jsVal.property( CommonConstants::X ).toInt();
    int y = jsVal.property( CommonConstants::Y ).toInt();

    switch ( event ) {
    case Activated: {
            sendNotification(ZoneConstants::RegistSoftkey, ViewRegistry::objectName(ViewRegistry::SelfTestView));
            sendNotification(ZoneConstants::SelectSoftkeyIndexMenuMode, data);
        }
        break;

    case Deactivated:
        sendNotification(ZoneConstants::SelectSoftkeyIndexMenuMode, QStringList{});
        break;

    case CreateView:
        sendNotification(Constants::CreateView, data);
        break;

    case CloseView:
        sendNotification(SettingConstants::ClosedSelfTestView);
        sendNotification(Constants::CloseView, data);        
        break;
    case MouseClick:
        QmlAppHelper::instance()->testMouseClick(Qt::LeftButton,Qt::NoModifier,QPoint(x, y),0);
        break;
    case MousePress:
        QmlAppHelper::instance()->testMousePress(Qt::LeftButton,Qt::NoModifier,QPoint(x, y),0);
        break;
    case MouseMove:
        QmlAppHelper::instance()->testMouseMove(QPoint(x,y),10);
    case MouseRelease:
        QmlAppHelper::instance()->testMouseRelease(Qt::LeftButton,Qt::NoModifier,QPoint(x, y),0);
        break;
    case RegistKeyboardInput:
        sendNotification(KeyboardConstants::RegistKeyboardInput, data);
        break;
    case ShowKeyboard:
        sendNotification(KeyboardConstants::ShowKeyboard, data);
        break;
    case HideKeyboard:
        sendNotification(KeyboardConstants::HideKeyboard, data);
        break;
    case CloseAllView: {
            sendNotification(RgConstants::CloseNaviMenuAll);
            sendNotification(SimulatorConstants::CloseSimulationView);
            sendNotification(RouteConstants::CloseRouteCandidateView);
            sendNotification(RouteConstants::CloseRouteSummaryView);
            sendNotification(KeyboardConstants::HideKeyboard);
         }
         break;
    case SaveScenario: {
            QString key = jsVal.property( CommonConstants::Key ).toString();
            QString value = jsVal.property( CommonConstants::Value ).toString();
            QString objectName = ViewRegistry::objectName(ViewRegistry::SelfTestView);
            sendNotification( PreferenceConstants::SavePreference,
                QVariantMap{
                    { CommonConstants::Caller            , objectName },
                    { SettingConstants::PreferencesName  , PreferenceConstants::Test },
                    { CommonConstants::Section           , objectName },
                    { CommonConstants::Key               , key},
                    { CommonConstants::Value             , value}
                } );
        }
        break;
    case GetScenario: {
            QString key = jsVal.property( CommonConstants::Key ).toString();
            QString objectName = ViewRegistry::objectName(ViewRegistry::SelfTestView);
            sendNotification( PreferenceConstants::RequestPreference,
                QVariantMap{
                    { CommonConstants::Caller             , objectName },
                    { SettingConstants::PreferencesName   , PreferenceConstants::Test },
                    { CommonConstants::Section            , objectName },
                    { CommonConstants::Key                , key}
                } );
        }
        break;
    default:
        return false;
    }

    return true;
}

bool SelfTestViewMediator::handleNotification( const mvc::INotificationPtr& note )
{
    switch ( note->getId() ) {
    case KeyboardConstants::ProcessKeyboard: {
             QVariantMap args = note->getBody().value<QVariantMap>();
             QString owner = args[CommonConstants::Owner].toString();
             if (owner != ViewRegistry::objectName(ViewRegistry::SelfTestView))
                 return false;
             auto key = args[CommonConstants::Key];
             auto text = args[CommonConstants::Text];
             QMetaObject::invokeMethod(mViewComponent, "__processKeyboard", Q_ARG(QVariant, QVariant::fromValue(key)), Q_ARG(QVariant, QVariant::fromValue(text)) );
         }
         break;

    case PreferenceConstants::ApplyPreference:
        {
            auto result = note->getBody().value<QVariantMap>();
            auto req = result[SearchConstants::Request].value<QVariantMap>();
            if ( req[CommonConstants::Caller].toString() == ViewRegistry::objectName(ViewRegistry::SelfTestView) ) {
                auto resp = result[SearchConstants::Response].value<PreferenceVo>();
                auto key = resp.key();
                auto value = resp.value();
                QMetaObject::invokeMethod(mViewComponent, "__getScenario", Q_ARG(QVariant, QVariant::fromValue(key)), Q_ARG(QVariant, QVariant::fromValue(value)) );
            }
        }
        break;

    case SearchConstants::SearchWait: {
            QVariantMap result = note->getBody().value<QVariantMap>();
            if (!result.contains(CommonConstants::Value))
                break;
            bool wait = result[CommonConstants::Value].toBool();
            QMetaObject::invokeMethod(mViewComponent, "__wait", Q_ARG(QVariant, QVariant::fromValue(wait)), Q_ARG(QVariant, QVariant::fromValue(result)) );
        }
        break;

    case KeyboardConstants::PullDownKeyboard:
        QMetaObject::invokeMethod(mViewComponent, "__pullDownKeyboard");
        break;
    case KeyboardConstants::PullUpKeyboard:
        QMetaObject::invokeMethod(mViewComponent, "__pullUpKeyboard");
        break;

    default:
        return false;
    }
    return true;
}
