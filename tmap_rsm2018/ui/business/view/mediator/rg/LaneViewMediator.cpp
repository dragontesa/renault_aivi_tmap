#include "LaneViewMediator.h"
#include "Constants.h"
#include "RgConstants.h"
#include "MapConstants.h"
#include "RgProtocol.h"
#include "mediator/app/MapViewMode.h"
#include "StringHelper.h"
#include "ResourceResolver.h"
#include "CommonConstants.h"
#include "TestConstants.h"
#include "RgUtils.h"
#include "SimulatorConstants.h"
#include "ViewRegistry.h"

using namespace SKT;

LaneViewMediator::LaneViewMediator( QObject* viewComponent )
    : AbstractViewMediator( viewComponent ),
    mCurMapViewMode( MapViewMode::Normal ),
    mCurMapViewModeReason( MapViewMode::None )
{
}

QList<int> LaneViewMediator::listNotificationInterests()
{
    static QList<int> notes = {
        RgConstants::UpdateRouteGuidance,
        MapConstants::ApplyMapViewMode,
        TestConstants::RunTest,
        RgConstants::RequestLaneStateUpdate,
        RgConstants::StartRouteGuidance,
        RgConstants::StopRouteGuidance,
        RgConstants::TerminatedRouteGuidance,
        SimulatorConstants::StopSimulator,
        RgConstants::StartRouteGuidance,
        RgConstants::ReplyToUiState,
        RgConstants::ChangedUiState
    };

    return notes;
}

bool LaneViewMediator::onEvent( int event, const QVariant& data )
{
    switch ( event ) {
    case Mediated:
        {
            sendNotification(MapConstants::NotifyMapViewMode);
            sendNotification(RgConstants::RequestUiState);
        }
        break;
    default:
        return false;
    }

    return true;
}

bool LaneViewMediator::handleNotification( const mvc::INotificationPtr& note )
{
    switch ( note->getId() ) {
    case RgConstants::UpdateRouteGuidance:
        {
            auto vo = note->getBody().value<RgDataVo>();
            updateView( vo );
        }
        return true;
    case RgConstants::ReplyToUiState:
    case RgConstants::ChangedUiState:
        {
            auto visible = QQmlProperty::read( mViewComponent, CommonConstants::Visible ).toBool();
            auto m = note->getBody().toMap();
            auto item = m[QStringLiteral("item")].toInt();
            auto value = m[QStringLiteral("value")].toInt();

            if ( item == RgConstants::HighwayVisible ) {
                mHighwayVisible = value;
            }
            else if ( item == RgConstants::HiPassLaneVisible ) {
                mHiPassLaneVisible = value;
            }
            if ( mHiPassLaneVisible || mHighwayVisible ) {
                QQmlProperty::write( mViewComponent, QStringLiteral("visible"), false );
            }
        }
        break;
    case RgConstants::RequestLaneStateUpdate:
        {
            sendNotification(RgConstants::UpdateLaneState, QVariantMap{
                {QStringLiteral("item"),RgConstants::LaneVisible},
                {QStringLiteral("laneIndex"),0},
                {QStringLiteral("value"),mLastState.visibleList[0]}
            });

            auto value = QVariantList{{
                QVariant::fromValue(mUpdate.imagesList[0]),
                QVariant::fromValue(mUpdate.coverImagesList[0]),
            }};

            sendNotification(RgConstants::UpdateLaneState, QVariantMap{
                {QStringLiteral("item"),RgConstants::LaneImageList},
                {QStringLiteral("laneIndex"),0},
                {QStringLiteral("value"),value}
            });

            sendNotification(RgConstants::UpdateLaneState, QVariantMap{
                {QStringLiteral("item"),RgConstants::LaneDistance},
                {QStringLiteral("laneIndex"),0},
                {QStringLiteral("value"),mUpdate.distList[0]}
            });

            // TODO 복수이면 위젯이 요청하면 mUpdate를 여러벌로( widget id )
            mUpdate = NotifiableUpdate();
        }
        break;
    case MapConstants::ApplyMapViewMode:
        {
            auto data = note->getBody().value<QVariantMap>();
            mCurMapViewMode = (MapViewMode::ModeK)data[CommonConstants::Mode].toInt();
            mCurMapViewModeReason = (MapViewMode::ReasonK)data[QStringLiteral("reason")].toInt();
        }
        return true;
    case RgConstants::StartRouteGuidance:
        {
            T_DEBUG( QString( "StartRouteGuidance #1" ) );
            mLastState = NotifiableUpdate();
            mUpdate = NotifiableUpdate();
            QMetaObject::invokeMethod(mViewComponent, "reset");
            sendNotification(RgConstants::UpdateLaneState, QVariantMap{
                {QStringLiteral("item"),RgConstants::LaneReset}
            });
        }
        break;
    case TestConstants::RunTest:
        {
            auto data = note->getBody().value<QVariantMap>();
            mTest = data["test"].toInt();
        }
        return true;
    }

    return false;
}

void LaneViewMediator::updateView( const RgDataVo& rgDataVo )
{
    if (rgDataVo.isNull()) {
        return;
    }

    if ( !MapViewMode::isInRouteGuidance( mCurMapViewMode ) ) {
        return;
    }

    updateLane( 0, rgDataVo );
}

//#define TMAP_RSM_ENABLE_TEST_LANEVIEW

void LaneViewMediator::updateLane( int laneIndex, const RgDataVo& rgDataVo )
{
    if ( mHighwayVisible ) {
        return;
    }

    auto turnCode = rgDataVo.nLaneTurnCode( laneIndex );
    auto laneCount = rgDataVo.nLaneCount( laneIndex );
    auto laneDist = rgDataVo.nLaneDist( laneIndex );

#ifdef TMAP_RSM_ENABLE_TEST_LANEVIEW
//    turnCode = RgTbtCode::TURN_LEFT;
//    laneCount = 5;
//    laneDist = 500;
#endif

    auto visible = false;
    auto validLaneCount = laneCount > -1 && laneCount < 16;
    if ( !validLaneCount ) {
        T_DEBUG( QString( "#1 [%1] Invalid lane count %2" ).arg( laneIndex ).arg( laneCount ) );
    }
    auto bLane = rgDataVo.bLane( laneIndex );
#ifdef TMAP_RSM_ENABLE_TEST_LANEVIEW
    //bLane = true;
#endif
    if ( !bLane ) {
        //T_DEBUG( QString( "#2 [%1] bLane == false" ).arg( laneIndex ) );
    }
    auto validDist = laneDist > 0 && laneDist <= LIMIT_LANE_AREA_DISTANCE;
    if ( !validDist ) {
        //T_DEBUG( QString( "#3 [%1] invalid dist %2" ).arg( laneIndex ).arg( laneDist ) );
    }

    if ( validLaneCount && bLane && validDist ) {
        visible = true;
    }

    mLastState.visibleList[laneIndex] = visible;
    if ( mUpdate.visibleList[laneIndex] != visible ) {
        mUpdate.visibleList[laneIndex] = visible;
        sendNotification(RgConstants::UpdateLaneState, QVariantMap{
            {QStringLiteral("item"),RgConstants::LaneVisible},
            {QStringLiteral("laneIndex"),laneIndex},
            {QStringLiteral("value"),visible}
        });
    }

    QQmlProperty::write( mViewComponent, CommonConstants::Visible, visible );
    if ( !visible ) {
        return;
    }

    if(laneCount > 9)
        laneCount = 9;

    QString img;
    QStringList imgList;
    QStringList imgFullPathList;

    QString coverImg;
    QStringList coverImgList;
    QStringList coverImgFullPathList;

    bool isFirstLane = true;

    for(int i = 0; i < laneCount; i++) {
        int etc = rgDataVo.nLaneEtcInfo(laneIndex, i);
#ifdef TMAP_RSM_ENABLE_TEST_LANEVIEW
        //etc = 0x40;
#endif
        img.clear();
        coverImg.clear();

        if((etc & 0x40) == 0x40) {
            img = isFirstLane? QStringLiteral("bus_b.png") : QStringLiteral("bus2_b.png");
        }
        else if((etc & 0x08) == 0x08) {
            if(rgDataVo.nLaneAvailable(laneIndex, i) != 0) {
                img = isFirstLane ? QStringLiteral("under_a.png") : QStringLiteral("under2_a.png");
            }
            else {
                img = isFirstLane ? QStringLiteral("under_b.png") : QStringLiteral("under2_b.png");
            }
        }
        else if((etc & 0x04) == 0x04){
            if(rgDataVo.nLaneAvailable(laneIndex, i) != 0) {
                img = isFirstLane ? QStringLiteral("high_a.png") : QStringLiteral("high2_a.png");
            }
            else {
                img = isFirstLane ? QStringLiteral("high_b.png") : QStringLiteral("high2_b.png");
            }
        }
        else{
            if((etc & 0x01) == 0x01) {
                coverImg = isFirstLane ? QStringLiteral("pocket_a.png") : QStringLiteral("pocket2_a.png");
            }
            else if((etc & 0x02) == 0x02) {
                coverImg = isFirstLane ? QStringLiteral("pocket_b.png") : QStringLiteral("pocket2_b.png");
            }

            QString key;
            if(rgDataVo.nLaneTurnInfo(laneIndex, i) > 0 && rgDataVo.nLaneTurnInfo(laneIndex, i) < 64) {
                key = getLaneFilter(rgDataVo.nLaneTurnInfo(laneIndex, i), rgDataVo.nLaneAvailable(laneIndex, i));

                if(!key.isEmpty()) {
                    if(turnCode == RgTbtCode::UNDER_IN || turnCode == RgTbtCode::OVER_IN) {
                        if(key == RgLaneCode::instance()->L0808())
                            key = RgLaneCode::instance()->L0800();
                        else if(key == RgLaneCode::instance()->L2808())
                            key = RgLaneCode::instance()->L2800();
                    }
                    img =  RgUtils::laneImage(laneIndex, key);
                }
            }
        }

        imgList.append( img );
        auto imgFullPath = ResourceResolver::instance()->imageUrl( QStringLiteral("lane"), img );
        imgFullPathList.append( imgFullPath );

        qDebug() << "[RG][LaneView]lane: " << i << " img: " << img << " cover: " << coverImg;

        coverImgList.append( coverImg );
        auto coverImgFullPath = ResourceResolver::instance()->imageUrl( QStringLiteral("lane"), coverImg );
        coverImgFullPathList.append( coverImgFullPath );
    }

    //T_DEBUG( QString( "#1 [%1] length %2" ).arg( laneIndex ).arg( imgFullPathList.length() ) );

    QQmlProperty::write( mViewComponent, QStringLiteral("laneImages"), imgFullPathList );
    QQmlProperty::write( mViewComponent, QStringLiteral("laneCoverImages"), coverImgFullPathList );

    mLastState.imagesList[laneIndex] = imgList;
    if ( mUpdate.imagesList[laneIndex] != imgList ) {
        mUpdate.imagesList[laneIndex] = imgList;
        mUpdate.coverImagesList[laneIndex] = coverImgList;
        auto value = QVariantList{{
            QVariant::fromValue(mUpdate.imagesList[laneIndex]),
            QVariant::fromValue(mUpdate.coverImagesList[laneIndex]),
        }};

        sendNotification(RgConstants::UpdateLaneState, QVariantMap{
            {QStringLiteral("item"),RgConstants::LaneImageList},
            {QStringLiteral("laneIndex"),laneIndex},
            {QStringLiteral("value"),value}
        });
    }

    imgFullPathList.clear();
    auto distNumber = QString( "%1m" ).arg( rgDataVo.nLaneDist( laneIndex ) );
    for(int i = 0; i < distNumber.length(); i++){
        auto ch = distNumber.at(i);
        auto distImg =  RgUtils::laneDistanceNumberImage(laneIndex,ch);
        //T_DEBUG( QString("distImg: %1").arg( distImg ) );
        img = ResourceResolver::instance()->imageUrl( QStringLiteral("lane"), distImg );
        imgFullPathList.append( img );
        if ( ch == QChar('k') ) {
            i++;
        }
    }

    QQmlProperty::write( mViewComponent, QStringLiteral("distNumberImages"), imgFullPathList );

    mLastState.distList[laneIndex] = distNumber;
    if ( mUpdate.distList[laneIndex] != distNumber ) {
        mUpdate.distList[laneIndex] = distNumber;
        sendNotification(RgConstants::UpdateLaneState, QVariantMap{
            {QStringLiteral("item"),RgConstants::LaneDistance},
            {QStringLiteral("laneIndex"),laneIndex},
            {QStringLiteral("value"),mUpdate.distList[laneIndex]}
        });
    }
}

QString LaneViewMediator::getLaneFilter(int laneInfo, int available)
{
    QString lanetext = QString( QStringLiteral("%1") ).arg( laneInfo );
    if(lanetext.length() == 1){
        lanetext = QStringLiteral("0") + lanetext;
    }

    QString availabletext = QString( QStringLiteral("%1") ).arg( available );
    if(availabletext.length() == 1){
        availabletext = QStringLiteral("0") + availabletext;
    }

    return lanetext + availabletext;
}

