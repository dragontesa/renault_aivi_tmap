#include "RouteSummaryViewMediator.h"
#include "Constants.h"
#include "RgConstants.h"
#include "RouteConstants.h"
#include "MapConstants.h"
#include "ZoneConstants.h"
#include "SimulatorConstants.h"
#include "NavigationControllerConstants.h"
#include <QVariant>
#include "mediator/app/MapViewMode.h"
#include "ViewRegistry.h"
#include "Csw.h"
#include "RouteSummaryItemVo.h"
#include "RouteSummaryItemListModel.h"
#include "LangConstants.h"
#include "QmlAppHelper.h"

using namespace SKT;

RouteSummaryViewMediator::RouteSummaryViewMediator( QObject* viewComponent )
    : AbstractViewMediator( viewComponent )
    , mStartRgMenuChanged(false)
{
}

RouteSummaryViewMediator::~RouteSummaryViewMediator()
{
}

QList<int> RouteSummaryViewMediator::listNotificationInterests()
{
    static QList<int> noties = {
        RouteConstants::CloseRouteSummaryView,
        ZoneConstants::DoSoftKeyFunction,
        ZoneConstants::DoSoftkeyIndexMenuFunction,
        RouteConstants::UpdateRouteSummarySimple,
        RouteConstants::UpdateRouteSummaryDetail,
        NavigationControllerConstants::ChangeLanguage,

        MapConstants::ResponseMapRect,
        RgConstants::ReplyToCurrentSpeed
    };

    return noties;
}

bool RouteSummaryViewMediator::onEvent( int event, const QVariant& data )
{
    switch (event) {
    case Mediated: {
            mapView = data.value<QObject*>();
            sendNotification(RouteConstants::BlockSetTvas, true);

            auto lang = QmlAppHelper::instance()->language();
            QQmlProperty::write(mViewComponent, "lang", lang);
        }
        break;

    case ViewChanged:
        sendNotification(MapConstants::UpdateMapViewPadding, data);
        break;

    case RetranslateUi:
        {
            QVariantMap m = data.toMap();

            auto title = m["title"];
            sendNotification(ZoneConstants::SelectZone1Title, title);

            auto menus = m["menus"];
            sendNotification(ZoneConstants::SelectSoftkeyMenuMode, menus);
        }
        break;

    case Activated: {
            QVariantMap m = data.toMap();
            auto menus = m["menus"];
            auto title = m["title"];
            sendNotification(MapConstants::ChangeMapViewMode,QVariantMap{{CommonConstants::Mode,MapViewMode::MapInquiry}});
            auto objectName = QQmlProperty::read(mViewComponent, CommonConstants::ObjectName).toString();
            sendNotification(ZoneConstants::RegistSoftkey, objectName);
            sendNotification(NavigationControllerConstants::CollectCSW, QVariant::fromValue(mViewComponent));

            sendNotification(ZoneConstants::SelectSoftkeyIndexMenuMode, menus);
            sendNotification(ZoneConstants::SelectZone1Title, title);
            sendNotification(MapConstants::ShowCarvata, false);//+
            sendNotification(RgConstants::BlockReroute, true);

            sendNotification(MapConstants::HideGasStationInfo);
            sendNotification(MapConstants::ShowTrafficInfoOnRouteLineForce, true);
            sendNotification(MapConstants::EnableCallout, false);

            auto caller = QQmlProperty::read(mViewComponent, "caller").toString();
            if (caller == ViewRegistry::objectName(ViewRegistry::MapModeOptionView)) {
            }
            QMetaObject::invokeMethod(mViewComponent, "drawRouteAll");

            // 주행중이고 MapInQuery 모드에서는 경로를 표시하지 않음
            // RouteSummaryView에서 강제적으로 경로를 표시하도록 함.
            m.clear();
            m["show"] = true;
            sendNotification(MapConstants::ShowRoute, m);
        }
        break;

    case Deactivated:
        sendNotification(RouteConstants::BlockSetTvas, false);

        sendNotification(NavigationControllerConstants::ResetCSW);
        sendNotification(ZoneConstants::SelectSoftkeyMenuMode, QStringList{});
        sendNotification(MapConstants::ShowTrafficInfoOnRouteLineForce, false);
        sendNotification(MapConstants::EnableCallout, true);
        break;

    case DrawRouteAll:
        {
            QVariantMap m;
            m["mapView"] = QVariant::fromValue(mapView);
            sendNotification(MapConstants::RequestMapRect, m);

            m["mapRect"] = mMapRect;
            m["caller"] = ViewRegistry::objectName(ViewRegistry::RouteSummaryView);
            sendNotification(MapConstants::DrawRouteAll, m);
        }
        break;

    case RemoveCSW:
        {
            QVariantMap m;
            m["csw"] = data;
            //sendNotification(NavigationControllerConstants::RemoveCSW, data);
        }
        break;

    case CheckItemInMapView:
        {
            sendNotification(MapConstants::RequestMapRect, QVariantMap{{"mapView", QVariant::fromValue(mapView)}});

            auto m = data.toMap();
            auto x = m[QStringLiteral("x")].toInt();
            auto y = m[QStringLiteral("y")].toInt();
            auto isInMap = mMapRect.contains(QPoint{x, y});

            QQmlProperty::write(mViewComponent, "isInMap", isInMap);
        }
        break;

    case RequestMapRect:
        sendNotification(MapConstants::RequestMapRect, QVariantMap{{"mapView", QVariant::fromValue(mapView)}});
        break;

    default:
        return false;
    }

    return true;
}

bool RouteSummaryViewMediator::handleNotification( const mvc::INotificationPtr& noti )
{
    switch (noti->getId()) {
    case NavigationControllerConstants::ChangeLanguage: {
            auto m = noti->getBody().toMap();
            auto lang = m[LangConstants::Lang].toString();
            QQmlProperty::write(mViewComponent, "lang", lang);

            QMetaObject::invokeMethod(mViewComponent, "retranslateUi");
        }
        break;

    case RouteConstants::CloseRouteSummaryView:
        QMetaObject::invokeMethod(mViewComponent, "close");
        break;

    case RouteConstants::UpdateRouteSummarySimple:
        QMetaObject::invokeMethod(mViewComponent, "updateRouteSummarySimple", Q_ARG(QVariant, noti->getBody()));
        break;

    case RouteConstants::UpdateRouteSummaryDetail:
        {
            auto m = noti->getBody().toMap();
            auto model = m["model"].value<RouteSummaryItemListModelPtr>();
            auto remainDistance = m["remainDistance"].toInt();

            int selectedId = 0;
            if (remainDistance) {
                int totalDistance = 0;
                for (auto i = model->count()-1; 0 <= i; i--) {
                    auto vo = model->get(i).value<RouteSummaryItemVoPtr>();
                    totalDistance += vo->turnRemainDistance();
//                    if (totalDistance >= remainDistance) {
//                        selectedId = i;
//                        break;
//                    }
                }
            }
            m["select"] = selectedId;
            m["itemCount"] = model->count();
            QMetaObject::invokeMethod(mViewComponent, "updateRouteSummaryDetail", Q_ARG(QVariant, m));
        }
        break;

    case ZoneConstants::DoSoftKeyFunction:
    case ZoneConstants::DoSoftkeyIndexMenuFunction:
        procSoftkey(noti->getBody());
        break;

    case MapConstants::ResponseMapRect:
        mMapRect = noti->getBody().toRect();
        QQmlProperty::write(mViewComponent, "mapSize", mMapRect.size());
        break;

    case RgConstants::ReplyToCurrentSpeed:
        mCurrentSpeed = noti->getBody().toInt();
        break;

    default:
        // Do nothing
        return false;
    }

    return true;
}

void RouteSummaryViewMediator::procSoftkey(const QVariant &data)
{
    QVariantMap m = data.toMap();
    if (m["name"].toString() != "RouteSummaryView")
        return;

    QString function = m["function"].toString();
    if (function == "menu") {
        QString menu = m["menu"].toString();
        int index = -1;
        if (m.contains("index")) {
            index = m["index"].toInt();
            menu.clear();
        }

        if (index == MenuSimulationDriving || menu == tr("모의 주행")) {
            openSimulationView();
        } else if (index == MenuStart || menu == tr("안내 시작") || menu == "StartGuideButton.qml") {
            auto routeId = QQmlProperty::read(mViewComponent, "routeId").toInt();
            auto routeOption = QQmlProperty::read(mViewComponent, "routeOption").toInt();
            auto filename = QQmlProperty::read(mViewComponent, "filename").toString();
            QVariantMap m;
            m["routeId"] = routeId;
            m["routeOption"] = routeOption;
            m["filename"] = filename;
            sendNotification(RgConstants::PrepareStartRouteGuidance, m);

            sendNotification(RgConstants::StartRouteGuidance, m); //+TODO : route id
            sendNotification(Constants::CloseView, QVariantMap{{CommonConstants::ObjectName, ViewRegistry::objectName(ViewRegistry::RouteCandidateView)}});
            close();
        }
    } else if (function == "back") {
        QString caller = QQmlProperty::read(mViewComponent, CommonConstants::Caller).toString();
        sendNotification(Constants::ShowView, QVariantMap{{CommonConstants::ObjectName,caller}});
        close();

        if (caller == ViewRegistry::objectName(ViewRegistry::MapModeOptionView)) {
            sendNotification(MapConstants::ShowGasStationInfo);
        }
    }
}

void RouteSummaryViewMediator::openSimulationView()
{
    // 현재 속도가 10km/h 이상인 경우 모의 주행 진입하지 않음
    sendNotification(RgConstants::RequestCurrentSpeed);

    qDebug() << "RouteCandidateViewMediator::openSimulationView(" << mCurrentSpeed << "km/h)";

    if (mCurrentSpeed < MAX_SPEED_TO_OPEN_SIMULATION_VIEW) {
        sendNotification(NavigationControllerConstants::ResetCSW);

        sendNotification(Constants::CreateView,
                         QVariantMap{{CommonConstants::Url,ViewRegistry::url(ViewRegistry::SimulationView)}, {CommonConstants::Caller,ViewRegistry::objectName(ViewRegistry::RouteSummaryView)}});
        sendNotification(SimulatorConstants::StartSimulator, 0); //+TODO : route id
        QQmlProperty::write( mViewComponent, CommonConstants::Visible, false );
    } else {
        sendNotification(ToastsConstants::ShowToast, QVariantMap{
                             {CommonConstants::Position, ToastsConstants::ToastUp},
                             {CommonConstants::Message, "안전을 위해 주행 중에는 모의주행을 실행할 수 없습니다."}
                         });
    }
}

void RouteSummaryViewMediator::close()
{
    sendNotification(NavigationControllerConstants::ResetCSW);
    QMetaObject::invokeMethod(mViewComponent, "close");
}
