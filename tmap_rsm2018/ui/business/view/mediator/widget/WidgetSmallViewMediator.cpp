#include "WidgetSmallViewMediator.h"
#include "Constants.h"
#include "RgConstants.h"
#include "mediator/rg/RgUtils.h"
#include "ResourceResolver.h"
#include "ZoneConstants.h"
#include "WidgetUtils.h"
#include "WidgetConstants.h"
#include "NavigationControllerConstants.h"
#include "mediator/rg/RgUtils.h"
#include "satellitedataptr.h"
#include "ApplicationConstants.h"
#include "QmlAppHelper.h"

using namespace SKT;

WidgetSmallViewMediator::WidgetSmallViewMediator(  QObject* viewComponent )
    : AbstractViewMediator( viewComponent )
{
}

WidgetSmallViewMediator::~WidgetSmallViewMediator()
{
}

QList<int> WidgetSmallViewMediator::listNotificationInterests()
{
    static QList<int> notes = {
        RgConstants::ReplyToRouteGuidanceState,
        RgConstants::ChangedRouteGuidanceState,
        RgConstants::UpdateTbtState,
        ZoneConstants::UpdateSoftkeyRouteGuide,
        NavigationControllerConstants::ChangeTimeFormat,
        NavigationControllerConstants::ChangeLanguage,
        RgConstants::UpdateGpsTime,
        ApplicationConstants::FocusToWidget,
    };

    return notes;
}

bool WidgetSmallViewMediator::onEvent( int event, const QVariant& data )
{
    switch ( event ) {
    case Mediated:
        {
            mRgProxy = Facade::instance()->retrieveProxy<RouteGuidanceProxy>();

            sendNotification(RgConstants::RequestRouteGuidanceState, getMediatorId());
			sendNotification( RgConstants::RequestTbtStateUpdate );

#ifdef BUILD_TARGET
            if (QQmlProperty::read(mViewComponent, "asView").toBool() == false) {
                //QQmlProperty::write(mViewComponent, "verticalFlip", true);
            }
#endif
            updateLanguage();
        }
        break;
    default:
        return false;
    }

    return true;
}

bool WidgetSmallViewMediator::handleNotification( const mvc::INotificationPtr& note )
{
    switch ( note->getId() ) {
    case RgConstants::ReplyToRouteGuidanceState:
    case RgConstants::ChangedRouteGuidanceState:
        {
            if ( !mRgProxy->isSimulatorRunning() ) {
                auto m = note->getBody().toMap();
                auto isRg = m["isRg"].toBool();
                QMetaObject::invokeMethod(mViewComponent, "changeRgComponent", Q_ARG(QVariant, isRg));
            }
        }
        break;	
    case RgConstants::UpdateTbtState:
        {
            if ( !mRgProxy->isSimulatorRunning() ) {
                WidgetUtils::handleTbtUpdate( WidgetConstants::SizeSmall, mViewComponent, note );
            }
		}
		break;

    case ZoneConstants::UpdateSoftkeyRouteGuide:
        {
            if ( !mRgProxy->isSimulatorRunning() ) {
                auto m = note->getBody().toMap();
                auto remainDistance = m[QStringLiteral("remainDistance")].toInt();
                auto remainTime = m[QStringLiteral("remainTime")].toInt();
                auto remainDistanceText = RgUtils::formatDistance(remainDistance);

                QQmlProperty::write(mViewComponent, "remainDistanceText", remainDistanceText);
                QQmlProperty::write(mViewComponent, "remainTime", remainTime);
            }
        }
        break;

    case NavigationControllerConstants::ChangeTimeFormat:
        {
            auto m = note->getBody().toMap();
            auto format = m["format"].toString();
            QQmlProperty::write(mViewComponent, "timeFormat", format);
        }
        break;

    case RgConstants::UpdateGpsTime:
        parseGpsTime(note->getBody());
        break;

    case NavigationControllerConstants::ChangeLanguage:
        updateLanguage();
        break;

    case ApplicationConstants::FocusToWidget:
        {
            auto m = note->getBody().toMap();
            auto instanceId = m[CommonConstants::FocusIndex].toInt();
            if (instanceId == UIConstants::GadgetSmall) {
                auto hasFocus = m[CommonConstants::Focus].toBool();
                //QQmlProperty::write(mViewComponent, "drawOutline", hasFocus);
            }
        }
        break;

    default:
        return false;
    }

    return true;
}

void WidgetSmallViewMediator::parseGpsTime(const QVariant &v)
{
    auto gpsTime = v.value<QDateTime>();
    QQmlProperty::write(mViewComponent, "gpsTime", gpsTime);
    auto time = gpsTime.time();
    auto h = time.hour();
    auto m = time.minute();
    auto s = time.second();
    QQmlProperty::write(mViewComponent, "gpsHour", h);
    QQmlProperty::write(mViewComponent, "gpsMin", m);
    QQmlProperty::write(mViewComponent, "gpsSec", s);
}

void WidgetSmallViewMediator::updateLanguage()
{
    auto lang = QmlAppHelper::instance()->language();
    QQmlProperty::write(mViewComponent, "lang", lang);
}
