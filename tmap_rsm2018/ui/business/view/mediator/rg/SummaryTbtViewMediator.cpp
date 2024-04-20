#include "SummaryTbtViewMediator.h"
#include "Constants.h"
#include "CommonConstants.h"
#include "RgConstants.h"
#include "MapConstants.h"
#include "RgProtocol.h"
#include "mediator/app/MapViewMode.h"
#include "ResourceResolver.h"
#include "StringHelper.h"
#include "ViewRegistry.h"
#include "RgUtils.h"
#include "SimulatorConstants.h"
#include "NavigationControllerConstants.h"
#include "TestConstants.h"

using namespace SKT;

SummaryTbtViewMediator::SummaryTbtViewMediator( QObject* viewComponent )
    : AbstractViewMediator( viewComponent ),
    mCurMapViewMode( MapViewMode::Normal ),
    mCurMapViewModeReason( MapViewMode::None )
{
    mList = findChild(QStringLiteral("list"));

#ifndef BUILD_TARGET
    QQmlProperty::write( mViewComponent, "isBuildTarget", false );
#endif
}

QList<int> SummaryTbtViewMediator::listNotificationInterests()
{
    static QList<int> notes = {
        RgConstants::UpdateRouteGuidance,
        MapConstants::ApplyMapViewMode,
        RgConstants::UpdateView,
        RgConstants::RequestUiState,
        RgConstants::ReplyToUiState,
        RgConstants::ChangedUiState,
        RgConstants::StartRouteGuidance,
        TestConstants::RunTest,
        NavigationControllerConstants::ResponseSkyState,
        NavigationControllerConstants::UpdateSkyState,
        RgConstants::UpdateHighwayRouteGuidance,
        NavigationControllerConstants::ChangeOnlineStatus,
        NavigationControllerConstants::ResponseOnlineStatus,
    };

    return notes;
}

bool SummaryTbtViewMediator::onEvent( int event, const QVariant& data )
{
    switch ( event ) {
    case Mediated:
        {
            sendNotification(MapConstants::NotifyMapViewMode);
            sendNotification( NavigationControllerConstants::RequestSkyUpdate );
            sendNotification(NavigationControllerConstants::QueryOnlineStatus);
        }
        break;
    case ClosingTouchAreaClicked:
    case ArrowClicked:
        {
            auto m = data.toMap();
            auto byCsw = m["byCsw"].toBool();
            QQmlProperty::write( mViewComponent, CommonConstants::Visible, false );
            sendNotification(RgConstants::UpdateView, QVariantMap{
                {CommonConstants::ObjectName, ViewRegistry::objectName( ViewRegistry::TbtView )},
                {CommonConstants::Visible, true},
                {"cswFocus", byCsw}
            });
        }
        break;
    case VisibleChanged:
        {
            auto m = data.toMap();
            auto visible = m[QStringLiteral("visible")].toBool();
            sendNotification(RgConstants::ChangedUiState,
                QVariantMap{
                    {CommonConstants::Caller, ViewRegistry::objectName(ViewRegistry::SummaryTbtView)},
                    {QStringLiteral("item"), RgConstants::SummaryTbtVisible},
                    {CommonConstants::Value, visible}
                }
            );
        }
        break;    
    case RegistCsw:
        {
            sendNotification(NavigationControllerConstants::RegistCSW, data);
        }
        break;
    case RemoveCsw:
        {
            sendNotification(NavigationControllerConstants::RemoveCSW, data);
        }
        break;
    case SelectCsw:
        {
            sendNotification(NavigationControllerConstants::SelectCSW, data);
        }
        break;
    default:
        return false;
    }

    return true;
}

// 복잡교차로 -> 고속도로뷰 -> TBT 상세 -> 교통정보바

bool SummaryTbtViewMediator::handleNotification( const mvc::INotificationPtr& note )
{
    switch ( note->getId() ) {
    case RgConstants::UpdateView:
        {
            auto data = note->getBody().value<QVariantMap>();
            auto objectName = data[CommonConstants::ObjectName].toString();
            if ( objectName == ViewRegistry::objectName( ViewRegistry::SummaryTbtView ) ) {
                auto visible = data[CommonConstants::Visible].toBool();
                if ( visible ) {
                    if ( mCrossImageVisible || mHighwayVisible ) {
                        visible = false;
                    }
                }
                QQmlProperty::write( mViewComponent, CommonConstants::Visible, visible );
                auto cswFocus = data["cswFocus"].toBool();
                if (cswFocus) {
                    sendNotification(NavigationControllerConstants::SelectCSWByObjectName, "rightArrowCsw");
                }
            }
        }
        break;
    case RgConstants::RequestUiState:
        {
            auto data = note->getBody().value<QVariantMap>();
            auto caller = data[CommonConstants::Caller];
            if ( caller != ViewRegistry::objectName(ViewRegistry::SummaryTbtView) ) {
                auto visible = QQmlProperty::read( mViewComponent, CommonConstants::Visible ).toBool();
                sendNotification(RgConstants::ReplyToUiState,
                    QVariantMap{
                        {CommonConstants::Caller, ViewRegistry::objectName(ViewRegistry::SummaryTbtView)},
                        {QStringLiteral("item"), RgConstants::SummaryTbtVisible},
                        {CommonConstants::Value, visible}
                    }
                );
            }
        }
        break;
    case RgConstants::ReplyToUiState:
    case RgConstants::ChangedUiState:
        {
            auto visible = QQmlProperty::read( mViewComponent, CommonConstants::Visible ).toBool();
            auto m = note->getBody().toMap();
            auto item = m[QStringLiteral("item")].toInt();
            auto value = m[QStringLiteral("value")].toInt();
            if ( item == RgConstants::CrossImageVisible ) {
                mCrossImageVisible = value;
            }
            else if ( item == RgConstants::HighwayVisible ) {
                mHighwayVisible = value;
            }
            if ( visible && ( mCrossImageVisible || mHighwayVisible ) ) {
                QQmlProperty::write( mViewComponent, QStringLiteral("visible"), false );
            }
        }
        break;
    case MapConstants::ApplyMapViewMode:
        {
            auto data = note->getBody().value<QVariantMap>();
            mCurMapViewMode = (MapViewMode::ModeK)data[CommonConstants::Mode].toInt();
            mCurMapViewModeReason = (MapViewMode::ReasonK)data[QStringLiteral("reason")].toInt();
        }
        break;
    case RgConstants::UpdateRouteGuidance:
        {
//            return true;
            auto vo = note->getBody().value<RgDataVo>();
            updateView( vo );
        }
        break;
    case RgConstants::StartRouteGuidance:
        {
            T_DEBUG( QString( "StartRouteGuidance #1" ) );
            mLastDistances = QStringList();
            QMetaObject::invokeMethod(mViewComponent, "reset");
        }
        break;
    case TestConstants::RunTest:
        {
            auto data = note->getBody().value<QVariantMap>();
            mTest = data["test"].toInt();

            T_DEBUGV("[Marquee] mTest: %d", mTest);
        }
        break;
    //case NavigationControllerConstants::ResponseSkyState:
    case NavigationControllerConstants::UpdateSkyState: {
            auto m = note->getBody().toMap();
            auto nightMode = m["nightMode"].toBool();
            //qDebug() << "[Summary]nightMode: " << nightMode;
            QQmlProperty::write( mViewComponent, "dayOrNight", nightMode ? QStringLiteral("night") : QStringLiteral("day") );
        }
        break;
    case RgConstants::UpdateHighwayRouteGuidance:
        {
            auto vo = note->getBody().value<RgDataVo>();
            bool update = true;
            if (update) {
                if ( !vo.isNull() ) {
                    if ( vo.nShowHighway() == 1 ) {
                        mIsInHighway = true;
                    }
                    else {
                        mIsInHighway = false;
                    }
                }
            }
        }
        break;
    case NavigationControllerConstants::ResponseOnlineStatus:
    case NavigationControllerConstants::ChangeOnlineStatus:
        {
            mIsOnline = note->getBody().toBool();
            QQmlProperty::write( mViewComponent, QStringLiteral("onlineOrOffline"), mIsOnline ? QStringLiteral("online") : QStringLiteral("offline"));
        }
        break;
    default:
        return false;
    }

    return true;
}

QVariantMap SummaryTbtViewMediator::makeModel(
    const QString& img, const QString& name,
    const QVariantList& distance, const QString& progress,
    bool noName
    )
{
    QVariantMap model;

    model[QStringLiteral("img")] = img;
    model[QStringLiteral("name")] = name;
    model[QStringLiteral("distanceLength")] = distance.length();
    model[QStringLiteral("distance")] = distance;
    model[QStringLiteral("progress")] = progress;    
    model[QStringLiteral("noName")] = noName;

    return model;
}

void SummaryTbtViewMediator::updateView( const RgDataVo& rgDataVo )
{
    if ( rgDataVo.isNull() ) {
        //T_DEBUG( QString( "#1 rgDataVo.isNull()" ) );
        return;
    }

    if ( !MapViewMode::isInRouteGuidance( mCurMapViewMode ) ) {
        //T_DEBUG( QString( "#1 MapViewMode::isInRouteGuidance" ) );
        return;
    }

    if ( mIsInHighway ) {
        QQmlProperty::write( mViewComponent, QStringLiteral("visible"), false );
        return;
    }

    QVariantList listModel;
    QString img;
    QString distStr;
    QString congestion;
    QString imgPath;

    auto length = rgDataVo.summaryTbtList_length();
    bool summaryTbtListUpdated = rgDataVo.summaryTbtListUpdated();
    if ( length != mLastDistances.count() ) {
        summaryTbtListUpdated = true;
    }

//    for ( int i = 0; i < length; i++ ) {
//        auto name = rgDataVo.summaryTbtList_szTBTMainText( i );
//        int dist = rgDataVo.summaryTbtList_nAccDist(i) - rgDataVo.nAccDist();
//        T_DEBUG( QString( "#1 [%1] name %2 dist %3" ).arg( i ).arg( name ).arg( dist ) );
//    }

    if ( !summaryTbtListUpdated ) {
        for ( int i = 0; i < length; i++ ) {
            int dist = rgDataVo.summaryTbtList_nAccDist(i) - rgDataVo.nAccDist();
            distStr = RgUtils::formatDistance( dist );

            //T_DEBUG( QString( "#1 [%1] dist %2 %3 rgAccDist %4 accDist %5" ).arg( i ).arg( dist ).arg( distStr ).arg( rgDataVo.nAccDist() ).arg( rgDataVo.summaryTbtList_nAccDist(i) ) );

            if ( mLastDistances.count() > i && distStr != mLastDistances[i] ) {
                mLastDistances[i] = distStr;
                QVariantList distImages;
                for(int j = 0; j < distStr.length(); j++){
                    auto ch = distStr.at(j);
                    img = RgUtils::summaryTbtListDistanceNumberImage(ch);
                    if ( !img.isEmpty() ) {
                        imgPath = ResourceResolver::instance()->imageUrl( QStringLiteral("tbt"), img );
                    }
                    else {
                        imgPath = QString();
                        // TOCHECK '-'
                        //T_DEBUG( QString( "Unexpected Distance Char %1" ).arg( ch ) );
                    }

                    distImages.append( imgPath );
                    if ( ch == QChar('k') ) {
                        j++;
                    }
                }
				
				if ( distImages.count() > 5 ) {
					T_DEBUG( QString( "#2 [%1] dist %2" ).arg( i ).arg( distStr ) );
					for ( auto dist : distStr ) {
						T_DEBUG( QString( "dist %2" ).arg( dist ) );
					}
				}

                QMetaObject::invokeMethod(mViewComponent, "setListModelDistance", Q_ARG(QVariant, i), Q_ARG(QVariant, QVariant::fromValue( distImages ) ));
            }
        }
    }
    else {
        mLastDistances.clear();

        for ( int i = 0; i < length; i++ ) {            
            // img
            auto turnCode = rgDataVo.summaryTbtList_nTurnCode( i );
            img = RgUtils::summaryTbtSecondImage( turnCode );
            if ( !img.isEmpty() ) {
                imgPath = ResourceResolver::instance()->imageUrl( QStringLiteral("tbt"), img );
            }
            else {
                imgPath = QString();
                T_DEBUG( QString( "Unexpected Turn Code %1" ).arg( turnCode ) );
            }

            // name
            auto name = rgDataVo.summaryTbtList_szTBTMainText( i );

            // dist
            int dist = rgDataVo.summaryTbtList_nAccDist( i ) - rgDataVo.nAccDist();
            distStr = RgUtils::formatDistance( dist );
            mLastDistances.append( distStr );
            QVariantList distImages;
            for(int j = 0; j < distStr.length(); j++){
                auto ch = distStr.at(j);
                distImages.append( ResourceResolver::instance()->imageUrl( QStringLiteral("tbt"), RgUtils::summaryTbtListDistanceNumberImage(ch) ) );
                if ( ch == QChar('k') ) {
                    j++;
                }
            }

//            qDebug() << QString( "[SummaryTbtView]index: %1 name: %2 dist: %3" )
//                .arg( i )
//                .arg( name )
//                .arg( dist );

            // congestion
            congestion = RgUtils::summaryTbtCongestionImage( rgDataVo.summaryTbtList_nCong(i) );

            // no name
            //bool noName = name.isEmpty() || name.indexOf( QStringLiteral( "회전" ) ) > -1 || name.indexOf( QStringLiteral( "목적지" ) ) > -1;
            bool noName = name.isEmpty();

            // model
            auto model = makeModel( imgPath, name, distImages, congestion, noName );
            listModel.append( model );

            //T_DEBUG( QString( "#1 [%1] name %2 noName %3 dist %4 %5 rgAccDist %6 accDist %7" ).arg( i ).arg( name ).arg( noName ).arg( dist ).arg( distStr ).arg( rgDataVo.nAccDist() ).arg( rgDataVo.summaryTbtList_nAccDist(i) ) );
        }

        QMetaObject::invokeMethod(mViewComponent, "setListModel", Q_ARG(QVariant, QVariant::fromValue( listModel ) ));
    }
}
