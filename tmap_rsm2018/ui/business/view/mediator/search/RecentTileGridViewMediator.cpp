#include "RecentTileGridViewMediator.h"
#include "Constants.h"
#include "ApplicationConstants.h"
#include "RouteConstants.h"
#include "CommonConstants.h"
#include "UserProfileConstants.h"
#include "SearchConstants.h"
#include "MapConstants.h"
#include "mediator/app/MapViewMode.h"
#include "RecentDestinationDto.h"
#include "StringHelper.h"
#include "../common/searchrequest.h"
#include "../mns/rpnsearch.h"
#include "ViewRegistry.h"
#include "RouteSearchRequest.h"
#include "NavigationControllerConstants.h"
#include "Csw.h"
#include "PlatformHelper.h"
using namespace SKT;

RecentTileGridViewMediator::RecentTileGridViewMediator( QObject* viewComponent )
    : AbstractViewMediator( viewComponent )
    , mIsActivated(false)
    , mIsVisibleChangeDestinationDialog(false)
{
    // 17.5.25 hskim
    // 시나리오에 의해
    //sendNotification(MapConstants::ChangeMapViewMode,QVariantMap{{CommonConstants::Mode,MapViewMode::Normal}});
}

RecentTileGridViewMediator::~RecentTileGridViewMediator()
{
    if (mIsVisibleChangeDestinationDialog) {
        // dialog 가 떠 있는 상태에서 통합 검색을 눌러 뷰가 종료될 때
        // 소프트키 modal처리를 해주어야 한다.

        // ChangeDestinationView가 닫힐 때 자동 처리 되도록 수정
        // 17.10.17 ymhong
        auto viewName = ViewRegistry::objectName(ViewRegistry::ChangeDestinationView);
        sendNotification(Constants::CloseView, QVariantMap{{CommonConstants::ObjectName, viewName}});
    }
    T_DEBUG( "I'm done." );
}

void RecentTileGridViewMediator::onRegister()
{
    AbstractViewMediator::onRegister();
}

QList<int> RecentTileGridViewMediator::listNotificationInterests()
{
    static QList<int> notes = {
        MapConstants::ResponseDriveMode,
        SearchConstants::ResponseRecentDestination,
        RouteConstants::UpdateSummary,
        SearchConstants::CloseRecentTileGridView,
        ApplicationConstants::UpdatedUserProfile,
    };

    return notes;
}

bool RecentTileGridViewMediator::onEvent( int event, const QVariant& data )
{
    switch ( event ) {
    case ActiveFocusChanged:
        {
            // 17.5.30 hskim 현재 운행 맵뷰 모드를 고려하지 않고 Normal로 바꾸는 초기 코드로 주석 처리합니다
//            auto jsVal = data.value<QJSValue>();
//            auto activeFocus = jsVal.property( "activeFocus" ).toInt();
//            if ( !activeFocus ) {
//                sendNotification(MapConstants::ChangeMapViewMode,
//                    QVariantMap{{CommonConstants::Mode,MapViewMode::Normal},{"reason",MapViewMode::RecentTileGridViewLostFocus}}
//                );
//            }
        }
        break;

        // 17.07.05 - ymhong.
        // handleNotification()에 있어야 하는 코드 같음.
//    case RouteConstants::RequestRoute:
//        sendNotification(RouteConstants::RequestRoute);
//        break;
    case Mediated:
        {
            auto iniPath = QDir::homePath() + QChar('/') + QLatin1String("tmap_rsm_ui_dbg.ini");
            QFile file( iniPath );
            if (file.exists()) {
                QSettings settings( iniPath, QSettings::IniFormat );
                auto benchRecentTileGridFontSize = settings.value(QStringLiteral("benchRecentTileGridFontSize"),0).toInt();
                if (benchRecentTileGridFontSize != 0) {
                    QMetaObject::invokeMethod(mViewComponent,"changeRecentTileGridFontSize",Q_ARG(QVariant,QVariant::fromValue(benchRecentTileGridFontSize)));
                }
            }
        }
        break;

    case Activated:
        {
            mIsActivated = true;
            sendNotification(NavigationControllerConstants::ResetCSW);
            void * csw = data.toList().at(0).value<Csw*>();
            sendNotification(NavigationControllerConstants::RegistCSW, data);
        }
        break;

    case Deactivated:
        mIsActivated = false;
        sendNotification(NavigationControllerConstants::ResetCSW);
        sendNotification(NavigationControllerConstants::RemoveCSW, data);
        break;

    case RequestRecentDestination:
        sendNotification(SearchConstants::RequestRecentDestination, data);
        break;

    case RequestRoute:
        mDestData = data;
        sendNotification(MapConstants::RequestDriveMode);
    	break;

    case RequestRouteSimpleInfo:
        {
            auto jsVal = data.value<QJSValue>();
            int mapX = jsVal.property(CommonConstants::X).toInt();
            int mapY = jsVal.property(CommonConstants::Y).toInt();
            auto id = jsVal.property(SearchConstants::Id).toInt();
            RouteSearchRequest req(this);
            req.requestSummary(id, mapX, mapY);
        }
        break;

    default:
        return false;
    }

    return true;
}

bool RecentTileGridViewMediator::handleNotification( const mvc::INotificationPtr& note )
{
    switch ( note->getId() )
    {
        case SearchConstants::ResponseRecentDestination:
        {
            auto objectName = QQmlProperty::read(mViewComponent, CommonConstants::ObjectName).toString();
            auto data = note->getBody().value<QVariantMap>();
            auto owner = data[CommonConstants::Owner].toString();
            if (owner != objectName)
                return false;

            auto reply = data[SearchConstants::Data].value<QList<RecentDestinationDtoPtr> >();
            QVariantMap r;
            int i = 0;
            for ( RecentDestinationDtoPtr dto : reply )
            {
                QVariantList v;
                uint arrivalTime = 0; // TODO : reqeust rp to arrival time from MNS
                v << dto->id()
                  << dto->dest()
                  << dto->address()
                  << dto->destType()
                  << dto->mapX()
                  << dto->mapY()
                  << dto->hit()
                  << arrivalTime
                  << dto->poiID();
                r.insert(QString::number(i),QVariant::fromValue(v));
                i++;
            }

            T_DEBUG("Responsed to recent dest: "+QString::number(i));
            QMetaObject::invokeMethod(mViewComponent,"pushRecentDestList",Q_ARG(QVariant,QVariant::fromValue(r)),Q_ARG(QVariant,i));
        }
        break;

    case RouteConstants::UpdateSummary:
        {
            auto summary = note->getBody().value<QVariantMap>();
            auto reqData = summary[SearchConstants::ReqData].value<QVariantMap>();
            auto mapX    = reqData[CommonConstants::X].toInt();
            auto mapY    = reqData[CommonConstants::Y].toInt();

            int distance    = summary[SearchConstants::Dist].toInt();
            int spentTime   = summary[SearchConstants::Time].toInt();
            QVariantList r;
            r << reqData[SearchConstants::Id].toInt() << distance << spentTime;

            QMetaObject::invokeMethod(mViewComponent,"updateRouteSimpleInfo",
                    Q_ARG(QVariant,QVariant::fromValue(mapX)),
                    Q_ARG(QVariant,QVariant::fromValue(mapY)),
                    Q_ARG(QVariant,QVariant::fromValue(r))
                    );
        }
        break;

    case MapConstants::ResponseDriveMode:
        {
            if (mIsActivated) {
                auto driveMode = note->getBody().toInt();
                if (driveMode == MapViewMode::Navi) {
                    auto m = mDestData.toMap();
                    auto address = m[CommonConstants::Name].toString();
                    if (address.isEmpty())
                        address = m[CommonConstants::Address].toString();

                    showConfirmDialog(address);

                    mIsVisibleChangeDestinationDialog = true;
                } else {
                    sendByDestination(mDestData);
                }
            }
        }
        break;

      case SearchConstants::CloseRecentTileGridView:
        QMetaObject::invokeMethod(mViewComponent, "close");
        break;
      case ApplicationConstants::UpdatedUserProfile: {
        // reload view data
        QMetaObject::invokeMethod(mViewComponent, "resetViewData");
        QMetaObject::invokeMethod(mViewComponent, "viewMediated");
      }
        break;

    default : break;
    }

    return false;
}

void RecentTileGridViewMediator::sendByDestination(const QVariant &data)
{
    auto m = data.toMap();

    auto mapX = m[CommonConstants::X].toInt();
    auto mapY = m[CommonConstants::Y].toInt();
    auto address = m[CommonConstants::Address].toString();
    auto poiId = m[SearchConstants::PoiID].toInt();
    auto id = m[SearchConstants::Id].toInt();

    sendNotification(Constants::CloseView, QVariantMap{{CommonConstants::ObjectName,
        ViewRegistry::objectName(ViewRegistry::RouteCandidateView)}});
    int x = mapX;
    int y = mapY;

    QVariantMap body;
    body[SearchConstants::Clear] = true;
    sendNotification(RouteConstants::SetDeparture, body);
    body.clear();
    body[SearchConstants::PoiID] = poiId;
    body[CommonConstants::X] = x;
    body[CommonConstants::Y] = y;
    body[CommonConstants::Address] = address;
    // 0~2까지는 자주가는 경로, 나머지는 최근목적지
    body[SearchConstants::DestType] = (id < 3) ? RouteConstants::EXPLORER_ROUTE_FREQUENT_DES
                                               : RouteConstants::EXPLORER_RECENT_DES;
    sendNotification(RouteConstants::SetArrival, body);
    sendNotification(Constants::CreateView,QVariantMap{{CommonConstants::Url,ViewRegistry::url(ViewRegistry::RouteCandidateView)}});

    QMetaObject::invokeMethod(mViewComponent, "close");
}

void RecentTileGridViewMediator::showConfirmDialog(const QString &address)
{
    auto x = mDestData.toMap()[CommonConstants::X].toInt();
    auto y = mDestData.toMap()[CommonConstants::Y].toInt();

    QVariantMap m;

    m[QStringLiteral("url")] = ViewRegistry::url(ViewRegistry::ChangeDestinationView);
    m[QStringLiteral("destX")] = x;
    m[QStringLiteral("destY")] = y;
    m[QStringLiteral("address")] = address;
    m[QStringLiteral("view")] = QVariant::fromValue(mViewComponent);
    PlatformHelper::instance()->setParentForSystemPopup(m);
    sendNotification(Constants::CreateView, m);
}
