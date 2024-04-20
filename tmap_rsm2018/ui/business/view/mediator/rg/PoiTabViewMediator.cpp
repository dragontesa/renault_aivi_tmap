#include "PoiTabViewMediator.h"

#include <QString>
#include "AddressConstants.h"
#include "MapConstants.h"
#include "RgConstants.h"
#include "RouteConstants.h"
#include "SearchConstants.h"
#include "ViewRegistry.h"
#include "PlatformHelper.h"

namespace SKT
{

PoiTabViewMediator::PoiTabViewMediator(QObject *viewComponent)
    : AbstractViewMediator(viewComponent)
{

}

PoiTabViewMediator::~PoiTabViewMediator()
{
    PlatformHelper::instance()->closeView(APP_POPUP_VIEW_NAME);
}

QList<int> PoiTabViewMediator::listNotificationInterests()
{
    return QList<int> {
        RgConstants::ReplyToRouteGuidanceState,
        Constants::CreateView,
        ZoneConstants::CswBackTriggered
    };
}

bool PoiTabViewMediator::onEvent(int event, const QVariant &data)
{
    switch (event) {
    case Mediated:
        break;

    case Activated:
        PlatformHelper::instance()->showView(APP_POPUP_VIEW_NAME);
        sendNotification(RgConstants::RequestRouteGuidanceState, getMediatorId());
        break;

    case AsViaClicked:
        asVia(data);
        close();
        break;

    case AsDestinationClicked:
        if (mIsRg) {
            close();
            showChangeDestinationDialog(data);
        } else {
            asDestination(data);
            close();
        }
        break;

    case AsBookmarkClicked:
        asBookmark(data);
        close();
        break;

    case CancelButtonClicked:
        close(true);
        break;
    }
    return true;
}

bool PoiTabViewMediator::handleNotification(const mvc::INotificationPtr &noti)
{
    switch (noti->getId()) {
    case Constants::CreateView:
        {
            auto m = noti->getBody().toMap();
            auto url = m[CommonConstants::Url].toString();
            if (url == ViewRegistry::url(ViewRegistry::CommonDialogView) ||
                    url == ViewRegistry::url(ViewRegistry::ChangeDestinationView)) {
                close(true);
            }
        }
        break;
    case RgConstants::ReplyToRouteGuidanceState: {
            mIsRg = parseIsRg(noti->getBody());
            QStringList menus;
            if (mIsRg                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         ) {
               menus.append(tr("경유지로"));
            }
            menus.append(tr("목적지로"));
            menus.append(tr("즐겨찾기 추가"));
            QQmlProperty::write(mViewComponent, "menus", menus);
        }
        break;

    case ZoneConstants::CswBackTriggered:
        {
            auto activated = QQmlProperty::read(mViewComponent, "active").toBool();
            if (activated) {
                auto state = noti->getBody().toInt();
                auto pressed = state == 1;
                if (pressed) {

                } else {
                    close(true);
                }
                sendNotification(ZoneConstants::CswBackAccepted);
            }
        }
        break;
    }
    return true;
}

bool PoiTabViewMediator::parseIsRg(const QVariant &v)
{
    auto m = v.toMap();
    auto isRg = m["isRg"].toBool();

    return isRg;
}

void PoiTabViewMediator::asVia(const QVariant &v)
{
    sendNotification(RouteConstants::BackupViaData);

    sendNotification(RouteConstants::SelectViaSlot, QVariantMap{{"slot", -1}});

    // AddVia를 호출 하면 RouteSettingView 생성.
    auto m = v.toMap();
    m[CommonConstants::Caller] = ViewRegistry::objectName(ViewRegistry::MapControlView);
    sendNotification(RouteConstants::AddVia, m);
}

void PoiTabViewMediator::asDestination(const QVariant &v)
{
    QVariantMap m;
    m[SearchConstants::Clear] = true;
    sendNotification(RouteConstants::SetDeparture, m);

    sendNotification(RouteConstants::SetArrival, v);

    m.clear();
    m[CommonConstants::Url] = ViewRegistry::url(ViewRegistry::RouteCandidateView);
    sendNotification(Constants::CreateView, m);
}

void PoiTabViewMediator::asBookmark(const QVariant &v)
{
    auto m = v.toMap();

    QVariantMap m2;
    m2[CommonConstants::Type] = CommonConstants::Favorite + "/poi";
    m2[SearchConstants::DestType] = 0; // 일반 즐겨찾기
    m2[SearchConstants::Dest] = m["name"];
    m2[CommonConstants::X] = m["x"];
    m2[CommonConstants::Y] = m["y"];
    m2[SearchConstants::PoiID] = m["poiId"];
    m2[SearchConstants::Favorite] = true;
    m2[SearchConstants::IsHome] = false;
    m2[SearchConstants::IsOffice] = false;
    m2[SearchConstants::NavSeq] = 0;
    m2[SearchConstants::RpFlag] = 0;

    sendNotification(AddressConstants::RequestAddress, m2);
}

void PoiTabViewMediator::close(bool cancel)
{
    QMetaObject::invokeMethod(mViewComponent, "close");

    QVariantMap m;
    m[CommonConstants::ObjectName] = ViewRegistry::objectName(ViewRegistry::MapControlView);
    sendNotification(Constants::CloseView, m);

    if (!cancel) {
        sendNotification(MapConstants::RemoveCalloutPopup);
    }
}

void PoiTabViewMediator::showChangeDestinationDialog(const QVariant &v)
{
    auto m = v.toMap();

    QVariantMap m2;
    m2[QStringLiteral("destX")] = m["x"];
    m2[QStringLiteral("destY")] = m["y"];
    m2[QStringLiteral("address")] = "";
    m2[QStringLiteral("name")] = m["name"];
    m2[SearchConstants::PoiID] = m[SearchConstants::PoiID];
    m2[QStringLiteral("view")] = QVariant::fromValue(mViewComponent);
    m2[QStringLiteral("okCallback")] = QStringLiteral("close");
    m2[QStringLiteral("url")] = ViewRegistry::url(ViewRegistry::ChangeDestinationView);
    PlatformHelper::instance()->setParentForSystemPopup(m2);

    sendNotification(Constants::CreateView, m2);
}

}//SKT
