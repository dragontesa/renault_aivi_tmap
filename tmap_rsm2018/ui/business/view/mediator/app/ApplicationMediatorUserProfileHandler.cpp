#include "ApplicationMediator.h"
#include "Constants.h"
#include "CommonConstants.h"
#include "ApplicationConstants.h"
#include "ViewRegistry.h"
#include "KeyboardConstants.h"
#include "MapConstants.h"
#include "RgConstants.h"
#include "RouteConstants.h"
#include "NavigationControllerConstants.h"
#include "SimulatorConstants.h"
#include "ZoneConstants.h"
#include "PlatformHelper.h"
#include "PreferenceConstants.h"
#include "SettingConstants.h"

using namespace SKT;
void ApplicationMediator::handleUserProfileChange( const mvc::INotificationPtr& note )
{
    static bool showableTileView = false;

    switch ( note->getId() ) {
        case ApplicationConstants::UpdatedUserProfile: {
            if (mHasMapData) {

                showableTileView = mViewIdList.contains(ViewRegistry::RecentTileGridView);
#ifdef AIVI_QC_PROFILE_SYNC_TEST
                QVariantMap msg =note->getBody().toMap();
                QString info = msg[CommonConstants::Message].toString();
                if (!info.isEmpty()) {
                    QVariantMap toast;
                    toast[CommonConstants::Position] = ToastsConstants::ToastUp;
                    toast[CommonConstants::Message]  = info;
                    sendNotification(ToastsConstants::ShowToast, toast);
                }
#endif
            }
        }
            // TODO: 프로파일 DB가 변경될 경우 관련 있는 화면(최근목적지,즐겨찾기 등)은 다시 DB를 불러오도록 할 필요가 있다.
        break;
        case ApplicationConstants::ChangedUserProfile: {
            if (mHasMapData) {
                auto m = note->getBody().toMap();
                auto profileId = m["profileId"].toInt();
                auto isRg = m["isRg"].toBool();
                qDebug() << "[OTP] ChangedUserProfile. id->" << profileId << ", isRg->" << isRg;

                showableTileView = mViewIdList.contains(ViewRegistry::RecentTileGridView);
                closeUserProfileDependentViews();

                // TMCRSM-2758
                // 경로안내는 아니지만 경로 요약 화면 등에서 tvas가 설정된 경우 없애준다.
                sendNotification(RouteConstants::CancelRoute);

                sendNotification(SettingConstants::RequestSettingByProfileChange);

                sendNotification(RouteConstants::RequestRouteByProfileChange);
                sendNotification(MapConstants::ShowRoute, QVariantMap{{"show", true}});
            }
        }
        break;
    }
}

void ApplicationMediator::onUserProfileChangeFinished()
{
    mLastActiveView = nullptr;

    // MapView
    createView(
            QVariantMap{
        {CommonConstants::Url,ViewRegistry::url(ViewRegistry::MapView)},
        {QStringLiteral("parent"),QVariant::fromValue(mMapViewWrap)}
    });
}
