#include "WidgetMediumViewMediator.h"
#include "Constants.h"
#include "RgConstants.h"
#include "ZoneConstants.h"
#include "WidgetUtils.h"
#include "NavigationControllerConstants.h"
#include "mediator/rg/RgUtils.h"
#include "ApplicationConstants.h"
#include "ViewRegistry.h"
#include "satellitedataptr.h"
#include "QmlAppHelper.h"

using namespace SKT;

WidgetMediumViewMediator::WidgetMediumViewMediator(  QObject* viewComponent )
    : AbstractViewMediator( viewComponent )
{
}

WidgetMediumViewMediator::~WidgetMediumViewMediator()
{
}

QList<int> WidgetMediumViewMediator::listNotificationInterests()
{
    static QList<int> notes = {
        RgConstants::ReplyToRouteGuidanceState,
        RgConstants::ChangedRouteGuidanceState,
        ZoneConstants::UpdateSoftkeyAddress,
        ZoneConstants::UpdateSoftkeyRouteGuide,
        RgConstants::UpdateTbtState,
        RgConstants::UpdateSdiState,
        NavigationControllerConstants::ChangeTimeFormat,
        RgConstants::UpdateGpsTime,
        NavigationControllerConstants::ChangeLanguage,
        ApplicationConstants::FocusToWidget,
    };

    return notes;
}

bool WidgetMediumViewMediator::onEvent( int event, const QVariant& data )
{
    switch ( event ) {
    case Mediated:
        {
            mRgProxy = Facade::instance()->retrieveProxy<RouteGuidanceProxy>();

            sendNotification(RgConstants::RequestRouteGuidanceState, getMediatorId());
            sendNotification( RgConstants::RequestTbtStateUpdate );
            sendNotification( RgConstants::RequestSdiStateUpdate );

#ifdef BUILD_TARGET
            if (QQmlProperty::read(mViewComponent, "asView").toBool() == false) {
                //QQmlProperty::write(mViewComponent, "verticalFlip", true);
            }
#endif
            updateLanguage();
        }
        break;

    case SearchClicked:
        sendNotification(Constants::CreateView, QVariantMap{{CommonConstants::Url, ViewRegistry::url(ViewRegistry::SearchTabView)}});
        QMetaObject::invokeMethod(mViewComponent, "close");

        sendNotification(ZoneConstants::FocusActivatedZone1TextInput, data);
        break;

    default:
        return false;
    }

    return true;
}

bool WidgetMediumViewMediator::handleNotification( const mvc::INotificationPtr& note )
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
                WidgetUtils::handleTbtUpdate( WidgetConstants::SizeXXL, mViewComponent, note );
            }
        }
        break;

    case RgConstants::UpdateSdiState:
        {
            if ( !mRgProxy->isSimulatorRunning() ) {
                WidgetUtils::handleSdiUpdate( WidgetConstants::SizeXXL, mViewComponent, note );
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

    case ZoneConstants::UpdateSoftkeyAddress:
        {
            if ( !mRgProxy->isSimulatorRunning() ) {
                auto yah = note->getBody().toString();
                QQmlProperty::write(mViewComponent, "address", yah);
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
            if (instanceId == UIConstants::GadgetMedium) {
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

void WidgetMediumViewMediator::parseGpsTime(const QVariant& v)
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

void WidgetMediumViewMediator::updateLanguage()
{
    auto lang = QmlAppHelper::instance()->language();
    QQmlProperty::write(mViewComponent, "lang", lang);
}
