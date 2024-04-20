#include "BusLaneViewMediator.h"
#include "Constants.h"
#include "CommonConstants.h"
#include "RgConstants.h"
#include "MapConstants.h"
#include "RgProtocol.h"
#include "mediator/app/MapViewMode.h"
#include "LangConstants.h"
#include "NavigationControllerConstants.h"

namespace SKT {

BusLaneViewMediator::BusLaneViewMediator(QObject* viewComponent) :
    AbstractViewMediator(viewComponent),
	mCurMapViewMode( MapViewMode::Normal ),
	mCurMapViewModeReason( MapViewMode::None ),
	mBusLaneOldId(0)
{
	mBusLane = findChild("busLimitFree");
}

QList<int> BusLaneViewMediator::listNotificationInterests()
{
    static QList<int> notes = {
        RgConstants::ShowBusLaneGuidance,
        MapConstants::ApplyMapViewMode,
        NavigationControllerConstants::ChangeLanguage
    };

    return notes;
}

bool BusLaneViewMediator::onEvent( int event, const QVariant& data )
{
	switch ( event ) {
	case Mediated:
	{
		sendNotification(MapConstants::NotifyMapViewMode);
	}
	break;
	default:
		return false;
	}

	return true;
}

bool BusLaneViewMediator::handleNotification( const mvc::INotificationPtr& note )
{
	switch (note->getId())
	{
	case RgConstants::ShowBusLaneGuidance:
	{
		auto busLaneId = note->getBody().toInt();
		if (mBusLaneOldId != busLaneId)
		   updateBusLane(busLaneId);
	}
		break;
	case MapConstants::ApplyMapViewMode:
	{
		auto data = note->getBody().value<QVariantMap>();
        mCurMapViewMode = (MapViewMode::ModeK)data[CommonConstants::Mode].toInt();
		mCurMapViewModeReason = (MapViewMode::ReasonK)data[QStringLiteral("reason")].toInt();
		// GUI test code
//		if ( MapViewMode::isInRouteGuidance( mCurMapViewMode ) ) {
//		   updateBusLane(1);
//		}
	}
	 break;
    case NavigationControllerConstants::ChangeLanguage: {
        auto data = note->getBody().value<QVariantMap>();
        auto lang = data[LangConstants::Lang].toString();
        QMetaObject::invokeMethod(mViewComponent, "retranslateUi", Q_ARG(QVariant, lang));
    }
        break;
	default:
		return false;
	}

	return true;
}

void BusLaneViewMediator::updateBusLane(int busLaneId)
{
	mBusLaneOldId = busLaneId;
	bool isLimitFree = true;
	QQmlProperty::write( mBusLane, QStringLiteral("isLimitFree"), isLimitFree );
}

}
