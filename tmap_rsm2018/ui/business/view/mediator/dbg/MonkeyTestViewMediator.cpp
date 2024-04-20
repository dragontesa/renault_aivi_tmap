#include "MonkeyTestViewMediator.h"
#include "Constants.h"
#include "RouteConstants.h"
#include "SettingConstants.h"
#include "TestConstants.h"
#include "ViewRegistry.h"
#include "NavigationControllerConstants.h"
#include "NavigationControllerProtocol.h"
#include "LangConstants.h"
#include "uiconstants.h"
#include "ViewRegistry.h"
#include "ApplicationConstants.h"
#include "WidgetConstants.h"
#include "NavigationControllerConstants.h"
#include "NavigationControllerProxy.h"
#include "TRandomUtils.h"
#include "QmlAppHelper.h"
#include "KeyboardConstants.h"
#include "PreferenceConstants.h"
#include "PreferenceVo.h"

using namespace SKT;

MonkeyTestViewMediator::MonkeyTestViewMediator( QObject* viewComponent )
    : AbstractViewMediator( viewComponent )
{
}

QList<int> MonkeyTestViewMediator::listNotificationInterests()
{
    static QList<int> notes = {
        ApplicationConstants::ReplyToViewList,
        PreferenceConstants::ApplyPreference,
        ZoneConstants::DoSoftKeyFunction,
        KeyboardConstants::ProcessKeyboard,
        PreferenceConstants::ApplyPreference,
        PreferenceConstants::ChangedPreference,
        KeyboardConstants::PullDownKeyboard,
        KeyboardConstants::PullUpKeyboard,
    };

    return notes;
}

bool MonkeyTestViewMediator::onEvent( int event, const QVariant& data )
{
    switch ( event ) {
    case Mediated: {
            sendNotification( PreferenceConstants::RequestPreference,
                QVariantMap{
                    { CommonConstants::Caller, QStringLiteral("MonkeyTestViewMediator") },
                    { QStringLiteral("preferencesName"), PreferenceConstants::Test },
                    { CommonConstants::Section, QStringLiteral("MonkeyTest") }
                } );
        }
        break;
    case RandClickTestClicked: {
            auto m = data.toMap();
            mRepeatsPerSecond = m["repeatsPerSecond"].toInt();
            mDurationMins = m["durationMins"].toInt();
            T_DEBUG( QString( "RandClickTestClicked mRepeatsPerSecond: %1 mDurationMins: %2" ).arg(mRepeatsPerSecond).arg(mDurationMins) );

            monkeyTestRandClick();
            QMetaObject::invokeMethod(mViewComponent, "close" );
        }
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
        break;
    }

    return false;
}

void MonkeyTestViewMediator::monkeyTestRandClick()
{
    sendNotification(PreferenceConstants::SavePreference, QVariantMap{
         {"preferencesName",PreferenceConstants::Test},
         {CommonConstants::Section,QStringLiteral("MonkeyTest")},
         {CommonConstants::Key,QStringLiteral("repeatsPerSecond")},
         {CommonConstants::Value,mRepeatsPerSecond}
     });

    sendNotification(PreferenceConstants::SavePreference, QVariantMap{
         {"preferencesName",PreferenceConstants::Test},
         {CommonConstants::Section,QStringLiteral("MonkeyTest")},
         {CommonConstants::Key,QStringLiteral("durationMins")},
         {CommonConstants::Value,mDurationMins}
     });

    // mRepeatsPerSecond / 2
    int interval = 1000 / mRepeatsPerSecond;
    int repeats = mDurationMins * 60 * mRepeatsPerSecond;
    T_DEBUG( QString( "repeats: %1 mDurationMins: %2 mRepeatsPerSecond: %3" ).arg(repeats).arg(mDurationMins).arg(mRepeatsPerSecond) );
    for ( int i = 0; i < repeats; i++ ) {
        QPointF pos;
        pos.rx() = TRandomUtils::instance()->randInt( 0, 819 );
        pos.ry() = TRandomUtils::instance()->randInt( 0, 1023 );
        QmlAppHelper::instance()->testMouseClick(Qt::LeftButton,Qt::NoModifier,pos,interval);
    }
}

bool MonkeyTestViewMediator::handleNotification( const mvc::INotificationPtr& note )
{
    switch ( note->getId() ) {
    case PreferenceConstants::ApplyPreference:
        {
            auto result = note->getBody().value<QVariantMap>();
            auto req = result[QStringLiteral("request")].value<QVariantMap>();
            if ( req[CommonConstants::Caller].toString() == QStringLiteral("MonkeyTestViewMediator") ) {
                auto resp = result[QStringLiteral("response")].value<QMap<QString,PreferenceVo>>();
                mRepeatsPerSecond = resp["repeatsPerSecond"].value().toInt();
                mDurationMins = resp["durationMins"].value().toInt();
                QQmlProperty::write( mViewComponent, "repeatsPerSecond", mRepeatsPerSecond );
                QQmlProperty::write( mViewComponent, "durationMins", mDurationMins );

                T_DEBUG( QString( "ApplyPreference mRepeatsPerSecond: %1 mDurationMins: %2" ).arg(mRepeatsPerSecond).arg(mDurationMins) );
            }
        }
        break;
    case KeyboardConstants::ProcessKeyboard: {
             QVariantMap args = note->getBody().value<QVariantMap>();
             QString owner = args[CommonConstants::Owner].toString();
             QString objectName = QQmlProperty::read(mViewComponent, CommonConstants::ObjectName).toString();
             if (owner != objectName)
                 return false;
             auto key = args[CommonConstants::Key];
             auto text = args[CommonConstants::Text];
             QMetaObject::invokeMethod(mViewComponent, "__processKeyboard", Q_ARG(QVariant, QVariant::fromValue(key)), Q_ARG(QVariant, QVariant::fromValue(text)) );
         }
         break;

    case KeyboardConstants::PullDownKeyboard:
        QMetaObject::invokeMethod(mViewComponent, "__pullDownKeyboard");
        break;
    case KeyboardConstants::PullUpKeyboard:
        QMetaObject::invokeMethod(mViewComponent, "__pullUpKeyboard");
        break;

    }

    return false;
}
